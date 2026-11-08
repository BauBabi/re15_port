#!/usr/bin/env python3
"""Messblatt RUHE Teil 2: Materialplan K5e (nur Messung, kein Bau).
Liest das Spiel-Asset K5c (stufe3/basis_k5c, TIM = shared_assets/RE2/BAHNSTEIG_FORM.TIM sha256 7187ff09...), die G4-Originalzeilen
(Seiten 0-2 = CLUT-Zeilen 0-2 der K5c-TIM, bytegleich G4, FARBE.md V3.1/E.1) und die Herkunft je Zeile (farbe/K5c_farbe_v3/bau.json
'zeilen', Nummern vor E.1; Abbildung 17 -> 16 aus stufe3/basis_k5c/zeilen.json).
Schreibt messung/material.json.   python ruhe_material.py"""
import json
import math
import os
import pickle
import sys

import cv2
import numpy as np

import rl

sys.path.insert(0, rl.K5 + '/generator')
import kf_g4 as KG  # noqa: E402
import gelenke as GL  # noqa: E402
FE = rl.fl.FE

GELENKE_BA = ["Huefte l", "Huefte r", "Muffe Huefte l", "Muffe Huefte r", "Knie l", "Knie r", "Fuss l", "Fuss r", "Schulter l (Arm l - Brust)",
              "Schulter r (Arm r - Brust)", "Muffe Schulter l", "Muffe Schulter r", "Ellbogen l", "Ellbogen r", "Hand l", "Hand r", "Muffe Hand l",
              "Muffe Hand r"]          # Nutzer 2026-11-08 "Verbindungsstuecke der Beine und Arme" (riegel_k5d.GELENKE_BA + Muffe Schulter l)
KRALLEN = (14, 15, 19, 20, 28, 29, 30, 31)


def lab(rgb):
    return rl.v4f.lab(np.asarray(rgb, float).reshape(-1, 3)).reshape(-1, 3).astype(float)


def zeilen_rgb(tim):
    cl = np.array(tim['cluts'], np.int64)
    return np.stack([cl & 31, (cl >> 5) & 31, (cl >> 10) & 31], -1).astype(float) * 255.0 / 31.0, cl


def flaechen(ordner):
    emd = open(os.path.join(ordner, 'FORM.EMD'), 'rb').read()
    tim = FE.tim_lesen(open(os.path.join(ordner, 'FORM.TIM'), 'rb').read())
    form = FE.lese_emd(emd)
    px = tim['pixel']
    out = {}
    for mi, m in enumerate(form['meshes']):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi, f in enumerate(m[art]):
                seite = int(f['page']) & 0xF
                po = seite * 128
                uv = f['uv']
                pts = [uv[0], uv[1], uv[3], uv[2]] if len(uv) == 4 else uv
                p = np.array([[a + po, b] for a, b in pts], np.int32)
                mk = np.zeros(px.shape, np.uint8)
                cv2.fillPoly(mk, [p], 1)
                cv2.polylines(mk, [p], True, 1)
                z = ((int(f['clut']) >> 6) & 0x1FF) - tim['clut_y']
                out[(mi, vi, fi)] = dict(zeile=z, seite=seite, idx=px[mk.astype(bool)], e=list(f['e']))
    return out, form, tim


def main():
    F, form, tim = flaechen(rl.BASIS)
    rgb, cl = zeilen_rgb(tim)
    labz = lab(rgb.reshape(-1, 3)).reshape(rgb.shape)
    bau = json.load(open(rl.K5 + '/farbe/K5c_farbe_v3/bau.json', encoding='utf-8'))
    abb = json.load(open(rl.BASIS + '/zeilen.json', encoding='utf-8'))['abbildung']
    basis16 = {abb[k]: v for k, v in bau['zeilen']['basis'].items() if abb.get(k, -1) >= 0}
    gruppe16 = {abb[k]: v for k, v in bau['zeilen']['gruppe'].items() if abb.get(k, -1) >= 0}
    basis16.update({0: 0, 1: 1})
    erg = dict(quelle='ruhe_material.py; Asset K5c = stufe3/basis_k5c (TIM sha256 7187ff09...), Herkunft farbe/K5c_farbe_v3/bau.json zeilen')
    # ---- Zeilen: Flaechen, Seiten, G4-Quellzeile
    zt = {}
    for k, d in F.items():
        t = zt.setdefault(d['zeile'], dict(flaechen=0, seiten={}, meshes=set()))
        t['flaechen'] += 1
        t['seiten'][d['seite']] = t['seiten'].get(d['seite'], 0) + 1
        t['meshes'].add(k[0])
    erg['zeilen_k5c'] = {str(z): dict(flaechen=t['flaechen'], seiten=t['seiten'], meshes=sorted(t['meshes']), g4_quellzeile=basis16.get(z),
                                      gruppe=gruppe16.get(z, {0: 'G4-Zeile 0 (Kopf: Maulinneres F2, Augen F10, Lippe)', 1: 'G4-Zeile 1 (Krallen)',
                                                                5: 'F1-Leuchtstelle', 6: 'Maul-Band', 7: 'Zaehne'}.get(z)))
                         for z, t in sorted(zt.items())}
    # ---- G4-Rampen (Zeilen 0-2 = G4 bytegleich): Klassen je Eintrag wie kf_g4.r4_eintraege (Ader = Rot-Gewicht w > 0), hell = L* >= R4_L_HELL
    nutz = {s: np.zeros(256) for s in range(4)}
    for k, d in F.items():
        np.add.at(nutz[d['seite']], d['idx'], 1)
    g4 = {}
    for s in range(3):
        L, C, h, w = KG.r4_eintraege([int(x) for x in cl[s]])
        n = nutz[s]
        ok = (cl[s] != 0) & (n > 0)
        klassen = dict(haut=ok & (w == 0) & (L[:, 0] < KG.R4_L_HELL), ader=ok & (w > 0), hell=ok & (w == 0) & (L[:, 0] >= KG.R4_L_HELL))
        g4[s] = {}
        for kn, sel in klassen.items():
            if not sel.any():
                g4[s][kn] = dict(eintraege=0)
                continue
            wt = n[sel]
            Lm = (L[sel] * wt[:, None]).sum(0) / wt.sum()
            g4[s][kn] = dict(eintraege=int(sel.sum()), texelzugriffe=int(wt.sum()), lab_mittel=[round(float(x), 1) for x in Lm],
                             C_mittel=round(float((C[sel] * wt).sum() / wt.sum()), 1),
                             ton_kreismittel=round(math.degrees(math.atan2((wt * np.sin(np.radians(h[sel]))).sum(), (wt * np.cos(np.radians(h[sel]))).sum())), 1),
                             L_p10_p50_p90=[round(float(np.percentile(np.repeat(L[sel, 0], wt.astype(int)), q)), 1) for q in (10, 50, 90)])
    erg['g4_rampen'] = {str(s): v for s, v in g4.items()}
    # ---- K5c-Gruppenzeilen: Haut-Eintraege (Klasse aus der G4-Quellzeile) texelgewichtet je Zeile -> Farbton, C*, L* (Anker E30)
    gz = {}
    for z, s in basis16.items():
        if z in (0, 1, 5, 6, 7):
            continue
        L0, C0, h0, w0 = KG.r4_eintraege([int(x) for x in cl[s]])
        n = np.zeros(256)
        for k, d in F.items():
            if d['zeile'] == z:
                np.add.at(n, d['idx'], 1)
        sel = (cl[s] != 0) & (n > 0) & (w0 == 0) & (L0[:, 0] < KG.R4_L_HELL)
        Lz = labz[z][sel]
        wt = n[sel]
        a, b = (Lz[:, 1] * wt).sum() / wt.sum(), (Lz[:, 2] * wt).sum() / wt.sum()
        gz[z] = dict(gruppe=gruppe16.get(z), g4_seite=s, texel=int(wt.sum()), L=round(float((Lz[:, 0] * wt).sum() / wt.sum()), 1),
                     C=round(float((np.hypot(Lz[:, 1], Lz[:, 2]) * wt).sum() / wt.sum()), 1), ton=round(math.degrees(math.atan2(b, a)), 1))
    erg['k5c_gruppenzeilen_haut'] = {str(z): v for z, v in gz.items()}
    # Hauptfamilie (Nutzer 2026-11-08: "einheitlich grau/blau"): Zeilen mit Farbton < 0 Grad; texelgewichtet
    fam = [v for v in gz.values() if v['ton'] < 0]
    warm = [v for v in gz.values() if v['ton'] >= 0]
    wt = np.array([v['texel'] for v in fam], float)
    hh = np.radians([v['ton'] for v in fam])
    erg['haut_anker_e30'] = dict(
        familie_zeilen=[v['gruppe'] for v in fam], warm_zeilen=[(v['gruppe'], v['ton']) for v in warm],
        ton=round(math.degrees(math.atan2((wt * np.sin(hh)).sum(), (wt * np.cos(hh)).sum())), 1),
        C=round(float((wt * [v['C'] for v in fam]).sum() / wt.sum()), 2), L=round(float((wt * [v['L'] for v in fam]).sum() / wt.sum()), 2),
        texel_familie=int(wt.sum()), texel_warm=int(sum(v['texel'] for v in warm)))
    # ---- Materialflaechen
    gl = GL.gelenk_flaechen(rl.BASIS)
    gl = gl[0] if isinstance(gl, tuple) else gl
    namen = {}
    for k, nm in gl.items():
        namen.setdefault(nm, []).append(k)
    gelenk = sorted(k for k, nm in gl.items() if nm in GELENKE_BA)
    erg['gelenkkoerper_alle'] = {nm: len(v) for nm, v in sorted(namen.items())}
    spitze = sorted(k for k in F if k[0] == 6)
    # Buckel vorn: Mesh-10-Flaechen mit Pixeln in Vorlagenfleck B2-F01/F02 (Kennungs-Render K5c Bild 2), dann zusammenhaengend ueber gemeinsame Ecken
    fl = np.load(rl.K5 + '/feinschliff/messung_v4f_flecken.npz')
    s2, K2 = rl.v4f.szene_id(rl.BASIS, 2)
    fm = fl['B2-F01'] | fl['B2-F02']
    treffer = {}
    sel = fm & K2['maske']
    for me, ff in zip(K2['mesh'][sel], K2['flaeche'][sel]):
        key = (int(me), int(ff // 100000), int(ff % 100000))
        treffer[key] = treffer.get(key, 0) + 1
    b10 = {k: n for k, n in treffer.items() if k[0] == 10 and n >= 10}
    # Zusammenhang: Flaechen mit gemeinsamer Ecke, groesste Komponente
    keys = list(b10)
    par = {k: k for k in keys}

    def wurzel(x):
        while par[x] != x:
            par[x] = par[par[x]]
            x = par[x]
        return x
    for i, a in enumerate(keys):
        for b in keys[i + 1:]:
            if set(F[a]['e']) & set(F[b]['e']):
                par[wurzel(a)] = wurzel(b)
    komp = {}
    for k in keys:
        komp.setdefault(wurzel(k), []).append(k)
    buckel = sorted(max(komp.values(), key=lambda v: sum(b10[k] for k in v))) if komp else []
    erg['buckel_auswahl'] = dict(regel='Mesh 10, >= 10 px in B2-F01|B2-F02 (Kennungs-Render K5c Bild 2), groesste Eck-Komponente',
                                 treffer_mesh10={'%d|%d|%d' % k: n for k, n in sorted(b10.items())},
                                 andere_meshes={'%d|%d|%d' % k: n for k, n in sorted(treffer.items()) if k[0] != 10},
                                 komponenten=[len(v) for v in komp.values()], px_gesamt=int(fm.sum()), px_mesh10_auswahl=int(sum(b10[k] for k in buckel)))
    # Kopf: Lippensaum (K5d lippe_k5c.pkl 'lc' = 140 Saumflaechen, Messblatt V4F 2.4), Augen (G4-F10-Rechteck), Maulinneres = Rest Zeile 0
    lp = pickle.load(open(rl.K5 + '/feinschliff/K5d/lippe_k5c.pkl', 'rb'))
    lkeys = lp['keys']
    saum = sorted({tuple(lkeys[i]) for i in lp['lc']})
    aug_r = bau['sonder']['Augen']['ziel_rechteck']

    def in_auge(k):
        d = F[k]
        return False
    # Augen = Zeile-0-Flaechen, deren UV im G4-F10-Zielrechteck liegt (kf_g4 bau: Augen -> Rechteck (91,64,24,10) Seite 0)
    form_f = {}
    for mi, m in enumerate(form['meshes']):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi, f in enumerate(m[art]):
                form_f[(mi, vi, fi)] = f
    augen = sorted(k for k, d in F.items() if d['zeile'] == 0 and d['seite'] == 0 and
                   all(aug_r[0] - 1 <= u <= aug_r[0] + aug_r[2] + 1 and aug_r[1] - 1 <= v <= aug_r[1] + aug_r[3] + 1 for u, v in form_f[k]['uv']))
    z0 = sorted(k for k, d in F.items() if d['zeile'] == 0)
    maul = sorted(set(z0) - set(saum) - set(augen))
    band = sorted(k for k, d in F.items() if d['zeile'] == 6)
    zahn = sorted(k for k, d in F.items() if d['zeile'] == 7)
    f1 = sorted(k for k, d in F.items() if d['zeile'] == 5)
    kralle = sorted(k for k in F if k[0] in KRALLEN)
    sonder = set(gelenk) | set(spitze) | set(buckel) | set(maul) | set(augen) | set(zahn) | set(f1)
    haut = sorted(k for k in F if k not in sonder)

    def info(ks):
        z, s, m = {}, {}, {}
        for k in ks:
            d = F[k]
            z[d['zeile']] = z.get(d['zeile'], 0) + 1
            s[d['seite']] = s.get(d['seite'], 0) + 1
            m[k[0]] = m.get(k[0], 0) + 1
        return dict(flaechen=len(ks), k5c_zeilen=dict(sorted(z.items())), seiten=dict(sorted(s.items())), meshes=dict(sorted(m.items())))
    mats = dict(Haut=haut, Gelenk=gelenk, Fleisch_Schwanzspitze=spitze, Fleisch_Buckel=buckel, Kralle=kralle, Zahn=zahn, Auge=augen,
                Maulinneres=maul, F1=f1, Lippensaum=[k for k in saum if k not in augen], Maul_Band=band)
    erg['material'] = {nm: dict(info(ks), liste=['%d|%d|%d' % k for k in ks]) for nm, ks in mats.items()}
    erg['material']['Kralle']['hinweis'] = 'Teilmenge von Haut (Kralle = Haut der Hand)'
    erg['material']['Lippensaum']['hinweis'] = 'Teilmenge von Haut (Lippe und Unterkiefer aussen = Haut)'
    erg['material']['Maul_Band']['hinweis'] = 'entfaellt als Material; Flaechen in Haut'
    # Kontrolle: jede Flaeche genau einem Grundmaterial
    grund = ['Haut', 'Gelenk', 'Fleisch_Schwanzspitze', 'Fleisch_Buckel', 'Zahn', 'Auge', 'Maulinneres', 'F1']
    alle = [k for nm in grund for k in mats[nm]]
    erg['kontrolle'] = dict(flaechen=len(F), zugeordnet=len(alle), doppelt=len(alle) - len(set(alle)))
    # Seite der Band-/Zahn-/F1-Texel: Seite 3 traegt G4-Indizes der Quellzeile (bau.json basis: Band 0, Zaehne 0, F1 2)
    json.dump(erg, open(os.path.join(rl.OUT, 'material.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=lambda o: sorted(o) if isinstance(o, set) else str(o))
    print(json.dumps({k: v for k, v in erg.items() if k not in ('material',)}, ensure_ascii=False, default=str)[:6000])
    print({nm: {kk: vv for kk, vv in v.items() if kk != 'liste'} for nm, v in erg['material'].items()})


if __name__ == '__main__':
    main()
