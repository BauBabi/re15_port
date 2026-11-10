#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 6a (P6): Riegel S17 - linke Hand (Mesh 12) und linker Unterarm (Mesh 11) gegen das Messblatt Hand
(analysis/befunde_runde41/SF_MESSBLATT_HAND.md). Wird von riegel_v8.py als S17 gerufen; allein:  python riegel_hand.py [EMD]

Gemessen wird mit hand_mass.py - dieselbe Messregel an den fuenf Nutzerbildern (birkin20-24) und am Modell. Toleranz je Mass =
gemessene Streuung der fuenf Vorlagebilder (kleinster .. groesster Wert) + 1 Videopixel Rasterkante; bei Verhaeltnissen ohne Zuschlag.
Die Vorlage zeigt den linken Arm NUR in der Pose P1 (in P2 und PV liegt er hinter dem Rumpf) - dort gibt es kein Vorlagemass.

(a) Breitenprofil: P1 alle acht Stationen (Ellbogen, Unterarm-Mitte, Handgelenk, Handwurzel, breite Stelle, Krallenwurzel,
    Krallen-Mitte, Spitzen = 2 Videopixel vor dem Handende) in der Streuung der Vorlage.
    P2 und PV (kein Vorlagemass): Handgelenk / Unterarm-Mitte >= kleinstes Verhaeltnis der Vorlage in P1 (einseitig: keine
    Einschnuerung am Uebergang; das Verhaeltnis zweier etwa runder Querschnitte haengt wenig vom Blick ab).
    Die 10 + 10 Profilstellen von Unterarm und Hand stehen als Angabe dabei (Zahl ausserhalb der Streuung).
(b) Handgelenk: Breite in P1 in der Streuung der Vorlage; dazu in ALLEN Keyframes die Unterarmspitze (Ecke 17 von Mesh 11) im
    Handkoerper (konvexe Huelle von Mesh 12) - die Gelenkenden ueberlappen, keine Luecke.
(c) Handbreite / Handlaenge in P1 in der Streuung der Vorlage; Handlaenge in der Streuung.
(d) Krallen in P1: Laenge Ringmitte -> Spitze der drei Fingerkrallen in 15.0 .. 23.1 px, der Daumenkralle in 6.7 .. 22.6 px
    (V7-Messblatt, einzelne Krallen sind im Messblatt Hand nicht aufloesbar), je +- 1 px; Wurzeldicke je Kralle nicht duenner als
    in V7 - 1 px Rasterkante (Nutzer: "Sind die Krallen zu duenn?"); Daumen (Gabel ab Handgelenk, Laenge, Wurzeldicke) in der Streuung der Vorlage.
    Der Daumenwinkel ist Angabe.
(e) Krallen kreuzen sich nicht in P1, P2, PV (die Vorlage belegt kein Kreuzen).
(f) Daumenkralle (Ecken 18-22) CLUT-Zeile 491, Fingerkrallen 492, uebrige Hand 490; u/v aller Flaechen von Mesh 11 und 12
    bytegleich dem Original-EM30 (0 fremde Texel), CLUT-Felder von Mesh 11 wie V7.
(g) Durchdringung: Zahl der Keyframes ALLER Clips, in denen Krallenachsen (Regel T-V7h: 11 Punkte Ringmitte -> Spitze) oder
    Hand-Ecken in einem Mesh ausser Unterarm und Hand stecken - nicht mehr als in V7 und in V8 r5d. Steigt die Zahl, nennt die Zeile
    Clip und Bild der hinzugekommenen Keyframes (Kacheln: analysis/befunde_runde41/SF_belege/v8/runde6a/).

PORT-WAHL: die Form steht auf keiner Disc, es gibt keine @0x-Adresse; alle Schwellen sind Messwerte der Vorlage.
"""
import os, sys, json
import numpy as np
from scipy.spatial import Delaunay

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
sys.path.insert(0, os.path.join(V.R41, 'v7hand'))
import kopf_orig_bau as KB            # noqa: E402
import hand_gemein as HG              # noqa: E402
import hand_mass as HM                # noqa: E402
import hand_r6 as H6                  # noqa: E402
import hand_suche as HS               # noqa: E402
import riegel_v7hand as R7            # noqa: E402

GATTER = HM.STATIONEN                 # alle acht Stationen des Messblatts
DAUMEN_MASSE = ('gabel_ab_handgelenk', 'laenge', 'dicke_wurzel')
# (g) Vergleichszahl V8 r5d (Asset c4976c62, git show 5820af00f:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD), gemessen mit
# durchdringung() am 2026-11-10 (analysis/befunde_runde41/SF_belege/v8/runde6a/durchdringung.txt): von 912 Keyframes
# Krallenachse im Koerper 26, Hand-Ecke im Koerper 68, zusammen 68; Unterarmspitze ausserhalb des Handkoerpers 912 (wie V7).
R5D_KF = dict(krallen_kf=26, hand_kf=68, zusammen_kf=68, gelenk_offen_kf=912)


def _vorlage():
    vor = {k: HM.vorlage(k) for k in HM.VORLAGE_BILDER}
    st = {s: [v['stationen'][s] for v in vor.values()] for s in HM.STATIONEN}
    d = {k: [v['daumen'][k] for v in vor.values() if v.get('daumen')] for k in DAUMEN_MASSE + ('winkel_zur_handachse',)}
    hl = [v['hand_laenge'] for v in vor.values()]
    bl = [v['stationen']['hand_breit'] / v['hand_laenge'] for v in vor.values()]
    eng = [v['stationen']['handgelenk'] / v['stationen']['unterarm_mitte'] for v in vor.values()]
    streu, _ = HS.vorlage_streuung(vor)
    return dict(st={s: (min(x), float(np.median(x)), max(x)) for s, x in st.items()}, daumen={k: (min(x), float(np.median(x)), max(x)) for k, x in d.items()},
                hand_laenge=(min(hl), float(np.median(hl)), max(hl)), bl=(min(bl), float(np.median(bl)), max(bl)), eng=(min(eng), float(np.median(eng)), max(eng)),
                streu=streu)


def _in(x, sp, rand=1.0):
    return sp[0] - rand <= x <= sp[2] + rand


def _keyframes(mo):
    erst = {}
    for c, (_f, k) in enumerate(mo.clips):
        for b in range(k):
            erst.setdefault(int(HG.BB.kf_von_clip(mo, c, b)), (c, b))
    return erst


def durchdringung(blob):
    """Regel T-V7h (v7hand/riegel_v7hand.keyframes), dazu die Ecken der Hand: Keyframes aller Clips, in denen ein Punkt der Hand in
    einem Mesh ausser Unterarm (11) und Hand (12) steckt. Dazu (b): Unterarmspitze im Handkoerper."""
    mo = V.modell(blob)
    erst = _keyframes(mo)
    P12 = np.array([v[:3] for v in mo.meshes[12].tv], float)
    P11 = np.array([v[:3] for v in mo.meshes[11].tv], float)
    roh = {bb: (np.array([v[:3] for v in mo.meshes[bb].tv], float), R7._tris(mo.meshes[bb])) for bb in range(len(mo.meshes))}
    achsen = []
    for name in ('T', 'A', 'B', 'C'):
        sp, ring = H6.KRALLEN[name]
        m = P12[ring].mean(0)
        achsen += [m + (P12[sp] - m) * t for t in np.linspace(0, 1, 11)]
    lok = np.vstack([np.array(achsen), P12])
    nk = len(achsen)
    liste, ohne_gelenk = [], []
    for kf, (c, b) in sorted(erst.items()):
        R, T = mo.pose(kf)
        w = lambda bb, P: P @ (R[bb].astype(float) / 4096).T + T[bb].astype(float)
        pts = w(12, lok)
        if Delaunay(pts[nk:]).find_simplex(w(11, P11[[17]]))[0] < 0:
            ohne_gelenk.append((c, b))
        for bb in range(len(mo.meshes)):
            if bb in (11, 12) or len(roh[bb][0]) < 5:
                continue
            Q = w(bb, roh[bb][0])
            try:
                hull = Delaunay(Q)
            except Exception:
                continue
            kand = np.where(hull.find_simplex(pts) >= 0)[0]
            tief = [int(i) for i in kand if R7._innen(pts[i], Q, roh[bb][1])]
            if tief:
                liste.append(dict(clip=c, bild=b, kf=kf, knochen=bb, krallen=sum(1 for i in tief if i < nk), hand=sum(1 for i in tief if i >= nk)))
    kk = {(e['clip'], e['bild']) for e in liste if e['krallen']}
    hh = {(e['clip'], e['bild']) for e in liste if e['hand']}
    je = {}
    for e in liste:
        je[e['knochen']] = je.get(e['knochen'], 0) + 1
    tiefste = sorted(liste, key=lambda e: -(e['krallen'] + e['hand']))[:6]
    return dict(keyframes=len(erst), krallen_kf=len(kk), hand_kf=len(hh), zusammen_kf=len(kk | hh), clips=sorted({c for c, _b in kk | hh}),
                je_knochen=je, tiefste=tiefste, gelenk_offen_kf=len(ohne_gelenk), gelenk_offen=ohne_gelenk[:10], liste=liste)


def _dd(blob):
    """durchdringung() mit Ablage je sha256 (912 Keyframes x 30 Meshes dauern etwa 3 Minuten); fehlt die Ablage, wird neu gemessen."""
    pf = os.path.join(V.SFV8, 'cache', 'dd_%s.json' % V.sha(blob)[:16])
    if os.path.exists(pf):
        return json.load(open(pf, encoding='utf-8'))
    g = durchdringung(blob)
    os.makedirs(os.path.dirname(pf), exist_ok=True)
    json.dump(g, open(pf, 'w', encoding='utf-8', newline=chr(10)))
    return json.loads(json.dumps(g))


def _je_clip(paare):
    je = {}
    for c, _b in paare:
        je[c] = je.get(c, 0) + 1
    return dict(sorted(je.items()))


def zeilen_pruefen(blob, ref_blob):
    """(f): CLUT-Zeilen und Texel von Mesh 11 und 12 gegen Original-EM30 und V7."""
    ms, ov, v7 = KB.teile(blob)[2], H6.original(), KB.teile(ref_blob)[2]
    out = dict(fehler=[])
    zeile = lambda uv: (uv[2] >> 6) & 0x1FF
    m = ms[12]
    kr = {n: set([H6.KRALLEN[n][0]] + H6.KRALLEN[n][1]) for n in H6.KRALLEN}
    zahl = {}
    for fl, uvs in ((m['tf'], m['tuv']), (m['qf'], m['quv'])):
        for f, uv in zip(fl, uvs):
            ecken = set(f[1::2])
            soll = HG.Z_DAUMEN if ecken & kr['T'] and len(ecken & (kr['T'] | {22})) == len(ecken) else \
                HG.Z_FINGER if any(len(ecken & (kr[n] | {w_})) == len(ecken) for n, w_ in (('A', 49), ('B', 31), ('C', 44))) else HG.Z_HAND
            zahl[zeile(uv)] = zahl.get(zeile(uv), 0) + 1
            if zeile(uv) != soll:
                out['fehler'].append('Mesh 12 Flaeche %s Zeile %d statt %d' % (sorted(ecken), zeile(uv), soll))
    out['zeilen_mesh12'] = zahl
    fremd = 0
    for nr in (11, 12):
        for k in ('tuv', 'quv'):
            for a, b in zip(ms[nr][k], ov[nr][k]):
                fremd += int([x for i, x in enumerate(a) if i != 2] != [x for i, x in enumerate(b) if i != 2])
            assert len(ms[nr][k]) == len(ov[nr][k])
    out['fremde_texel'] = fremd
    out['mesh11_clut_wie_v7'] = all(list(a) == list(b) for k in ('tuv', 'quv') for a, b in zip(ms[11][k], v7[11][k]))
    out['ok'] = not out['fehler'] and fremd == 0 and out['mesh11_clut_wie_v7'] and zahl == {HG.Z_DAUMEN: 6, HG.Z_FINGER: 18, HG.Z_HAND: len(m['tf']) + len(m['qf']) - 24}
    return out


def s17(blob, ref_blob, mit_g=True):
    vor = _vorlage()
    mo, mo7 = V.modell(blob), V.modell(ref_blob)
    P = {pn: HM.modell(mo, HM.POSEN[pn]) for pn in ('P1', 'P2', 'PV')}
    p1, v7 = P['P1'], HM.modell(mo7, HM.POSEN['P1'])
    st = p1['stationen']
    a_aus = [s for s in GATTER if not _in(st[s], vor['st'][s])]
    eng = {pn: round(P[pn]['stationen']['handgelenk'] / P[pn]['stationen']['unterarm_mitte'], 3) for pn in ('P2', 'PV')}
    prof = {}
    for nm, r, (med, lo, hi) in zip(('unterarm', 'hand'), (p1['profil_unterarm'], p1['profil_hand']), vor['streu'][1:3]):
        x = np.array(r, float)
        prof[nm] = dict(werte=[round(float(q), 1) for q in x], ausser=[int(i) for i in np.where((x < lo - 1) | (x > hi + 1))[0]],
                        vorlage=[round(float(q), 1) for q in med])
    a = dict(p1={s: st[s] for s in HM.STATIONEN}, vorlage={s: [round(q, 1) for q in vor['st'][s]] for s in HM.STATIONEN}, ausser=a_aus,
             eng=eng, eng_min=round(vor['eng'][0], 3), profil=prof, ok=not a_aus and all(e >= vor['eng'][0] for e in eng.values()))
    g = _dd(blob) if mit_g else None
    g7 = _dd(ref_blob) if mit_g else None
    b = dict(handgelenk=st['handgelenk'], vorlage=[round(q, 1) for q in vor['st']['handgelenk']],
             gelenk_offen_kf=g['gelenk_offen_kf'] if g else None, keyframes=g['keyframes'] if g else None)
    b['ok'] = _in(st['handgelenk'], vor['st']['handgelenk']) and (g is None or g['gelenk_offen_kf'] == 0)
    bl = st['hand_breit'] / p1['hand_laenge']
    c = dict(breite_durch_laenge=round(bl, 3), vorlage=[round(q, 3) for q in vor['bl']], hand_laenge=p1['hand_laenge'],
             vorlage_laenge=[round(q, 1) for q in vor['hand_laenge']], hand_breit=st['hand_breit'])
    c['ok'] = _in(bl, vor['bl'], 0.0) and _in(p1['hand_laenge'], vor['hand_laenge'])
    kl = {n: (k['laenge'], k['dicke_wurzel'], k['winkel_zur_handachse']) for n, k in sorted(p1['krallen'].items())}
    kl7 = {n: (k['laenge'], k['dicke_wurzel']) for n, k in sorted(v7['krallen'].items())}
    lang_aus = [n for n, k in kl.items() if not _in(k[0], HS.DAUMEN_VORLAGE if n == 'T' else HS.FINGER_VORLAGE)]
    duenn = [n for n, k in kl.items() if k[1] < kl7[n][1] - 1.0]
    dm = p1.get('daumen') or {}
    d_aus = [k for k in DAUMEN_MASSE if k not in dm or not _in(dm[k], vor['daumen'][k])]
    d = dict(krallen=kl, v7=kl7, laenge_ausser=lang_aus, duenner_als_v7=duenn, daumen={k: dm.get(k) for k in DAUMEN_MASSE + ('winkel_zur_handachse', 'seite')},
             daumen_vorlage={k: [round(q, 1) for q in v_] for k, v_ in vor['daumen'].items()}, daumen_ausser=d_aus, ok=not lang_aus and not duenn and not d_aus)
    e = dict(kreuzen={pn: P[pn]['kreuzen'] for pn in P})
    e['ok'] = not any(e['kreuzen'].values())
    f = zeilen_pruefen(blob, ref_blob)
    out = dict(a=a, b=b, c=c, d=d, e=e, f=f)
    if mit_g:
        gg = dict(neu={k: g[k] for k in ('keyframes', 'krallen_kf', 'hand_kf', 'zusammen_kf', 'clips', 'je_knochen', 'tiefste')},
                  v7={k: g7[k] for k in ('krallen_kf', 'hand_kf', 'zusammen_kf', 'clips')}, r5d=R5D_KF, liste=g['liste'])
        alt = {(e['clip'], e['bild']) for e in g7['liste']}
        neu = sorted({(e['clip'], e['bild']) for e in g['liste']} - alt)
        weg = sorted(alt - {(e['clip'], e['bild']) for e in g['liste']})
        gg['je_clip'] = _je_clip({(e['clip'], e['bild']) for e in g['liste']})
        gg['je_clip_v7'] = _je_clip(alt)
        gg['dazu'] = [list(x) for x in neu]
        gg['dazu_je_clip'] = _je_clip(neu)
        gg['weg_je_clip'] = _je_clip(weg)
        gg['ok'] = g['zusammen_kf'] <= max(g7['zusammen_kf'], R5D_KF['zusammen_kf']) and g['krallen_kf'] <= max(g7['krallen_kf'], R5D_KF['krallen_kf'])
        out['g'] = gg
    out['ok'] = all(out[k]['ok'] for k in out)
    out['teile_rot'] = [k for k in 'abcdefg' if k in out and not out[k]['ok']]
    return out


def zeilen(s):
    gr = lambda t: 'GRUEN' if t['ok'] else 'ROT'
    a, b, c, d, e, f = (s[k] for k in 'abcdef')
    z = ['S17 Hand und Unterarm links (P6, Messblatt Hand): %s%s' % ('GRUEN' if s['ok'] else 'ROT', '' if s['ok'] else ' in (%s)' % ', '.join(s['teile_rot']))]
    z.append('   S17 (a) Breitenprofil P1, Stationen 1-8 Modell | Vorlage min..max: %s; ausserhalb (+- 1 px): %s; '
             'P2 / PV Handgelenk durch Unterarm-Mitte %.3f / %.3f (>= %.3f) -> %s'
             % (' '.join('%.1f|%.1f..%.1f' % (a['p1'][k], a['vorlage'][k][0], a['vorlage'][k][2]) for k in GATTER), a['ausser'] or 'keine',
                a['eng']['P2'], a['eng']['PV'], a['eng_min'], gr(a)))
    z.append('   S17 (a) Angabe Profil P1: Unterarm 10-100 %% %s (Vorlage %s), ausserhalb an Stelle %s; Hand 0-90 %% %s (Vorlage %s), ausserhalb an Stelle %s'
             % (a['profil']['unterarm']['werte'], a['profil']['unterarm']['vorlage'], a['profil']['unterarm']['ausser'],
                a['profil']['hand']['werte'], a['profil']['hand']['vorlage'], a['profil']['hand']['ausser']))
    z.append('   S17 (b) Handgelenk P1 %.1f px (Vorlage %.1f..%.1f, Median %.1f); Unterarmspitze ausserhalb des Handkoerpers in %s von %s Keyframes (Soll 0) -> %s'
             % (b['handgelenk'], b['vorlage'][0], b['vorlage'][2], b['vorlage'][1], b['gelenk_offen_kf'], b['keyframes'], gr(b)))
    z.append('   S17 (c) Handbreite / Handlaenge P1 %.3f (Vorlage %.3f..%.3f, Median %.3f); Handlaenge %.1f px (Vorlage %.1f..%.1f), breite Stelle %.1f px -> %s'
             % (c['breite_durch_laenge'], c['vorlage'][0], c['vorlage'][2], c['vorlage'][1], c['hand_laenge'], c['vorlage_laenge'][0], c['vorlage_laenge'][2], c['hand_breit'], gr(c)))
    z.append('   S17 (d) Krallen P1 (Laenge, Wurzeldicke, Winkel): %s; V7 (Laenge, Dicke): %s; Laenge ausserhalb (Finger 15.0..23.1, Daumen 6.7..22.6, +- 1 px): %s; '
             'duenner als V7 - 1 px: %s; Daumen %s gegen Vorlage %s, ausserhalb: %s -> %s'
             % (d['krallen'], d['v7'], d['laenge_ausser'] or 'keine', d['duenner_als_v7'] or 'keine', d['daumen'], d['daumen_vorlage'], d['daumen_ausser'] or 'keine', gr(d)))
    z.append('   S17 (e) Krallen kreuzen: %s (Soll keine) -> %s' % (e['kreuzen'], gr(e)))
    z.append('   S17 (f) Mesh 12 Flaechen je CLUT-Zeile %s (Soll 491: 6, 492: 18, Rest 490), falsche Zeile: %d, fremde Texel (u/v ungleich Original-EM30, Mesh 11 + 12): %d, '
             'Mesh 11 UV und CLUT wie V7: %s -> %s' % (f['zeilen_mesh12'], len(f['fehler']), f['fremde_texel'], 'ja' if f['mesh11_clut_wie_v7'] else 'NEIN', gr(f)))
    if 'g' in s:
        g = s['g']
        z.append('   S17 (g) Durchdringung, Keyframes von %d: Krallenachse im Koerper %d (V7 %d, r5d %d), Hand-Ecke im Koerper %d (V7 %d, r5d %d), zusammen %d (V7 %d, r5d %d); '
                 'Clips %s; je Knochen %s -> %s' % (g['neu']['keyframes'], g['neu']['krallen_kf'], g['v7']['krallen_kf'], g['r5d']['krallen_kf'], g['neu']['hand_kf'],
                                                   g['v7']['hand_kf'], g['r5d']['hand_kf'], g['neu']['zusammen_kf'], g['v7']['zusammen_kf'], g['r5d']['zusammen_kf'],
                                                   g['neu']['clips'], g['neu']['je_knochen'], gr(g)))
        z.append('   S17 (g) je Clip neu %s, V7 %s; gegenueber V7 dazu %s (Bilder: %s), weggefallen %s; tiefste (Clip, Bild, Knochen, Punkte Krallen, Hand): %s'
                 % (g['je_clip'], g['je_clip_v7'], g['dazu_je_clip'] or 'keine', ' '.join('%d/%d' % (c, b) for c, b in g['dazu']) or '-', g['weg_je_clip'] or 'keine',
                    [(t['clip'], t['bild'], t['knochen'], t['krallen'], t['hand']) for t in g['neu']['tiefste']]))
    return z


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--v7', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--json')
    ap.add_argument('--ohne-g', action='store_true')
    a = ap.parse_args()
    s = s17(open(a.emd, 'rb').read(), open(a.v7, 'rb').read(), mit_g=not a.ohne_g)
    print('Riegel S17 fuer %s\nsha256 %s' % (a.emd, V.sha(open(a.emd, 'rb').read())))
    print('\n'.join(zeilen(s)))
    if a.json:
        json.dump(s, open(a.json, 'w', encoding='utf-8', newline='\n'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(0 if s['ok'] else 1)


if __name__ == '__main__':
    main()
