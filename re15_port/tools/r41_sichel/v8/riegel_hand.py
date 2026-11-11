#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 6a (P6) und 6c (P7): Riegel S17 - linke Hand (Mesh 12) und linker Unterarm (Mesh 11) gegen das Messblatt Hand
(analysis/befunde_runde41/SF_MESSBLATT_HAND.md) - und Riegel S20 Handstellung. Wird von riegel_v8.py als S17 und S20 gerufen;
allein:  python riegel_hand.py [EMD]

Gemessen wird mit hand_mass.py - dieselbe Messregel an den fuenf Nutzerbildern (birkin20-24) und am Modell. Toleranz je Mass =
gemessene Streuung der fuenf Vorlagebilder (kleinster .. groesster Wert) + 1 Videopixel Rasterkante; bei Verhaeltnissen ohne Zuschlag.
RUNDE 6c (P7): gemessen wird in den ECHTEN Szenenposen (hand_szene.py): fuenf Paar-Posen mit Vorlagenbild (birkin20 <-> h125,
birkin22 <-> h126, birkin23 <-> h128, birkin21 <-> h130, birkin24 <-> h132) und drei Kontroll-Posen ohne Vorlage (h100, h110, h135).
Bis Runde 6b galten die Ersatz-Posen P1/P2/PV; dort lag der Arm anders als im Spiel, die verdrehte Hand (118 Grad) blieb unbemerkt.

(a) Breitenprofil: in JEDER Paar-Pose alle acht Stationen (Ellbogen, Unterarm-Mitte, Handgelenk, Handwurzel, breite Stelle, Krallenwurzel,
    Krallen-Mitte, Spitzen = 2 Videopixel vor dem Handende, gemessen quer zur wahren Finger-Achse) in der Streuung der Vorlage.
    In allen acht Posen: Handgelenk / Unterarm-Mitte >= kleinstes Verhaeltnis der Vorlage (einseitig: keine Einschnuerung am Uebergang).
    Die 10 + 10 Profilstellen von Unterarm und Hand stehen als Angabe dabei (Mittel der Paar-Posen, Zahl ausserhalb der Streuung).
(b) Handgelenk: Breite in jeder Paar-Pose in der Streuung der Vorlage; dazu in ALLEN Keyframes die Unterarmspitze (Ecke 17 von Mesh 11) im
    Handkoerper (konvexe Huelle von Mesh 12) - die Gelenkenden ueberlappen, keine Luecke.
(c) Handbreite / Handlaenge in jeder Paar-Pose in der Streuung der Vorlage; Handlaenge in der Streuung.
(d) Krallen in jeder Paar-Pose: Laenge Ringmitte -> Spitze der drei Fingerkrallen in 15.0 .. 23.1 px, der Daumenkralle in 6.7 .. 22.6 px
    (V7-Messblatt, einzelne Krallen sind im Messblatt Hand nicht aufloesbar), je +- 1 px; Wurzeldicke je Kralle nicht duenner als
    in V7 in derselben Pose - 1 px Rasterkante (Nutzer: "Sind die Krallen zu duenn?"); Daumen (Gabel ab Handgelenk, Laenge, Wurzeldicke)
    in der Streuung der Vorlage.
(e) Krallen kreuzen sich in keiner der acht Posen (die Vorlage belegt kein Kreuzen).
(f) Daumenkralle (Ecken 18-22) CLUT-Zeile 491, Fingerkrallen 492, uebrige Hand 490; u/v aller Flaechen von Mesh 11 und 12
    bytegleich dem Original-EM30 (0 fremde Texel), CLUT-Felder von Mesh 11 wie V7.
(g) Durchdringung: Zahl der Keyframes ALLER Clips, in denen Krallenachsen (Regel T-V7h: 11 Punkte Ringmitte -> Spitze) oder
    Hand-Ecken in einem Mesh ausser Unterarm und Hand stecken - nicht mehr als in V7 und in V8 r5d. Dazu getrennt die Keyframes der
    Szene (hand_szene.SZENE_KF: Clip 0, 11 und 1, wie der Hangar-Auftritt sie zeigt): nicht mehr als bei r5d (1).
S20 Handstellung: siehe s20().

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
import hand_szene as SZ               # noqa: E402
import hand_r6 as H6                  # noqa: E402
import hand_suche as HS               # noqa: E402
import riegel_v7hand as R7            # noqa: E402

GATTER = HM.STATIONEN                 # alle acht Stationen des Messblatts
DAUMEN_MASSE = ('gabel_ab_handgelenk', 'laenge', 'dicke_wurzel')
# (g) Vergleichszahl V8 r5d (Asset c4976c62, git show 5820af00f:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD), gemessen mit
# durchdringung() am 2026-11-10 (analysis/befunde_runde41/SF_belege/v8/runde6a/durchdringung.txt): von 912 Keyframes
# Krallenachse im Koerper 26, Hand-Ecke im Koerper 68, zusammen 68; Unterarmspitze ausserhalb des Handkoerpers 912 (wie V7).
R5D_KF = dict(krallen_kf=26, hand_kf=68, zusammen_kf=68, gelenk_offen_kf=912)
# davon in den Szenen-Keyframes (hand_szene.SZENE_KF): 1 (Clip 11 Bild 38, drei Hand-Ecken in Brust und Oberarm), gemessen 2026-11-11
R5D_SZENE_KF = 1


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


def _szene(liste):
    return sorted({(e['clip'], e['bild']) for e in liste if e['bild'] in SZ.SZENE_KF.get(e['clip'], ())})


def s17(blob, ref_blob, mit_g=True):
    vor = _vorlage()
    mo, mo7 = V.modell(blob), V.modell(ref_blob)
    P = {pn: HM.modell(mo, HM.POSEN[pn]) for pn in HM.POSEN}
    P7 = {pn: HM.modell(mo7, HM.POSEN[pn]) for pn in HM.PAARE}
    st = {pn: P[pn]['stationen'] for pn in P}
    a_aus = {pn: [s for s in GATTER if not _in(st[pn][s], vor['st'][s])] for pn in HM.PAARE}
    eng = {pn: round(st[pn]['handgelenk'] / st[pn]['unterarm_mitte'], 3) for pn in P}
    mittel = {s: round(float(np.mean([st[pn][s] for pn in HM.PAARE])), 1) for s in GATTER}
    prof = {}
    for nm, key, (med, lo, hi) in zip(('unterarm', 'hand'), ('profil_unterarm', 'profil_hand'), vor['streu'][1:3]):
        x = np.mean([np.array(P[pn][key], float) for pn in HM.PAARE], 0)
        prof[nm] = dict(werte=[round(float(q), 1) for q in x], ausser=[int(i) for i in np.where((x < lo - 1) | (x > hi + 1))[0]],
                        vorlage=[round(float(q), 1) for q in med])
    a = dict(posen={pn: {s: st[pn][s] for s in GATTER} for pn in HM.PAARE}, mittel=mittel, vorlage={s: [round(q, 1) for q in vor['st'][s]] for s in GATTER},
             ausser={pn: v for pn, v in a_aus.items() if v}, eng=eng, eng_min=round(vor['eng'][0], 3), profil=prof,
             ok=not any(a_aus.values()) and all(e >= vor['eng'][0] for e in eng.values()))
    g = _dd(blob) if mit_g else None
    g7 = _dd(ref_blob) if mit_g else None
    hg = {pn: st[pn]['handgelenk'] for pn in HM.PAARE}
    b = dict(handgelenk=hg, vorlage=[round(q, 1) for q in vor['st']['handgelenk']],
             gelenk_offen_kf=g['gelenk_offen_kf'] if g else None, keyframes=g['keyframes'] if g else None)
    b['ok'] = all(_in(x, vor['st']['handgelenk']) for x in hg.values()) and (g is None or g['gelenk_offen_kf'] == 0)
    bl = {pn: round(st[pn]['hand_breit'] / P[pn]['hand_laenge'], 3) for pn in HM.PAARE}
    hl = {pn: P[pn]['hand_laenge'] for pn in HM.PAARE}
    c = dict(breite_durch_laenge=bl, vorlage=[round(q, 3) for q in vor['bl']], hand_laenge=hl, vorlage_laenge=[round(q, 1) for q in vor['hand_laenge']],
             hand_breit={pn: st[pn]['hand_breit'] for pn in HM.PAARE})
    c['ok'] = all(_in(x, vor['bl'], 0.0) for x in bl.values()) and all(_in(x, vor['hand_laenge']) for x in hl.values())
    kl = {pn: {n: (k['laenge'], k['dicke_wurzel']) for n, k in sorted(P[pn]['krallen'].items())} for pn in HM.PAARE}
    kl7 = {pn: {n: (k['laenge'], k['dicke_wurzel']) for n, k in sorted(P7[pn]['krallen'].items())} for pn in HM.PAARE}
    lang_aus = sorted({'%s %s' % (pn, n) for pn in kl for n, k in kl[pn].items() if not _in(k[0], HS.DAUMEN_VORLAGE if n == 'T' else HS.FINGER_VORLAGE)})
    duenn = sorted({'%s %s' % (pn, n) for pn in kl for n, k in kl[pn].items() if n in kl7[pn] and k[1] < kl7[pn][n][1] - 1.0})
    dm = {pn: (P[pn].get('daumen') or {}) for pn in HM.PAARE}
    d_aus = sorted({'%s %s' % (pn, k) for pn in dm for k in DAUMEN_MASSE if k not in dm[pn] or not _in(dm[pn][k], vor['daumen'][k])})

    def spanne(werte):
        werte = list(werte)
        return (round(float(min(werte)), 1), round(float(max(werte)), 1)) if werte else None

    namen = sorted({n for pn in kl for n in kl[pn]})
    d = dict(krallen={n: (spanne(kl[pn][n][0] for pn in kl if n in kl[pn]), spanne(kl[pn][n][1] for pn in kl if n in kl[pn])) for n in namen},
             v7={n: (spanne(kl7[pn][n][0] for pn in kl7 if n in kl7[pn]), spanne(kl7[pn][n][1] for pn in kl7 if n in kl7[pn])) for n in namen},
             laenge_ausser=lang_aus, duenner_als_v7=duenn,
             daumen={k: spanne(dm[pn][k] for pn in dm if k in dm[pn]) for k in DAUMEN_MASSE},
             daumen_vorlage={k: [round(q, 1) for q in v_] for k, v_ in vor['daumen'].items()}, daumen_ausser=d_aus, ok=not lang_aus and not duenn and not d_aus)
    e = dict(kreuzen={pn: P[pn]['kreuzen'] for pn in P})
    e['ok'] = not any(e['kreuzen'].values())
    f = zeilen_pruefen(blob, ref_blob)
    out = dict(a=a, b=b, c=c, d=d, e=e, f=f)
    if mit_g:
        gg = dict(neu={k: g[k] for k in ('keyframes', 'krallen_kf', 'hand_kf', 'zusammen_kf', 'clips', 'je_knochen', 'tiefste')},
                  v7={k: g7[k] for k in ('krallen_kf', 'hand_kf', 'zusammen_kf', 'clips')}, r5d=R5D_KF, liste=g['liste'])
        alt = {(e_['clip'], e_['bild']) for e_ in g7['liste']}
        neu = sorted({(e_['clip'], e_['bild']) for e_ in g['liste']} - alt)
        weg = sorted(alt - {(e_['clip'], e_['bild']) for e_ in g['liste']})
        gg['je_clip'] = _je_clip({(e_['clip'], e_['bild']) for e_ in g['liste']})
        gg['je_clip_v7'] = _je_clip(alt)
        gg['dazu'] = [list(x) for x in neu]
        gg['dazu_je_clip'] = _je_clip(neu)
        gg['weg_je_clip'] = _je_clip(weg)
        gg['szene'] = [list(x) for x in _szene(g['liste'])]
        gg['szene_v7'] = [list(x) for x in _szene(g7['liste'])]
        gg['szene_kf'] = sum(len(v) for v in SZ.SZENE_KF.values())
        gg['ok_alle'] = g['zusammen_kf'] <= max(g7['zusammen_kf'], R5D_KF['zusammen_kf']) and g['krallen_kf'] <= max(g7['krallen_kf'], R5D_KF['krallen_kf'])
        gg['ok_szene'] = len(gg['szene']) <= R5D_SZENE_KF
        gg['ok'] = gg['ok_alle'] and gg['ok_szene']
        out['g'] = gg
    out['ok'] = all(out[k]['ok'] for k in out)
    out['teile_rot'] = [k for k in 'abcdefg' if k in out and not out[k]['ok']]
    return out


def zeilen(s):
    gr = lambda t: 'GRUEN' if t['ok'] else 'ROT'
    a, b, c, d, e, f = (s[k] for k in 'abcdef')
    z = ['S17 Hand und Unterarm links (P6/P7, Messblatt Hand, echte Szenenposen): %s%s'
         % ('GRUEN' if s['ok'] else 'ROT', '' if s['ok'] else ' in (%s)' % ', '.join(s['teile_rot']))]
    z.append('   S17 (a) Breitenprofil, Mittel der fuenf Paar-Posen, Stationen 1-8 Modell | Vorlage min..max: %s; ausserhalb (+- 1 px) je Paar-Pose: %s; '
             'Handgelenk durch Unterarm-Mitte in den acht Posen %s (>= %.3f) -> %s'
             % (' '.join('%.1f|%.1f..%.1f' % (a['mittel'][k], a['vorlage'][k][0], a['vorlage'][k][2]) for k in GATTER), a['ausser'] or 'keine',
                ' '.join('%.3f' % x for x in a['eng'].values()), a['eng_min'], gr(a)))
    pu, ph = a['profil']['unterarm'], a['profil']['hand']
    z.append('   S17 (a) Angabe Profil (Mittel der Paar-Posen): Unterarm 10-100 %% %s (Vorlage %s), ausserhalb an Stelle %s; Hand 0-90 %% %s (Vorlage %s), ausserhalb an Stelle %s'
             % (pu['werte'], pu['vorlage'], pu['ausser'], ph['werte'], ph['vorlage'], ph['ausser']))
    z.append('   S17 (b) Handgelenk je Paar-Pose %s px (Vorlage %.1f..%.1f, Median %.1f); Unterarmspitze ausserhalb des Handkoerpers in %s von %s Keyframes (Soll 0) -> %s'
             % (' '.join('%.1f' % x for x in b['handgelenk'].values()), b['vorlage'][0], b['vorlage'][2], b['vorlage'][1], b['gelenk_offen_kf'], b['keyframes'], gr(b)))
    z.append('   S17 (c) Handbreite / Handlaenge je Paar-Pose %s (Vorlage %.3f..%.3f, Median %.3f); Handlaenge %s px (Vorlage %.1f..%.1f +- 1), breite Stelle %s px -> %s'
             % (' '.join('%.3f' % x for x in c['breite_durch_laenge'].values()), c['vorlage'][0], c['vorlage'][2], c['vorlage'][1],
                ' '.join('%.1f' % x for x in c['hand_laenge'].values()), c['vorlage_laenge'][0], c['vorlage_laenge'][2],
                ' '.join('%.1f' % x for x in c['hand_breit'].values()), gr(c)))
    z.append('   S17 (d) Krallen in den Paar-Posen, je Kralle (Laenge min..max, Wurzeldicke min..max): %s; V7: %s; Laenge ausserhalb (Finger 15.0..23.1, Daumen 6.7..22.6, +- 1 px): %s; '
             'duenner als V7 - 1 px (selbe Pose): %s; Daumen min..max %s gegen Vorlage %s, ausserhalb: %s -> %s'
             % (d['krallen'], d['v7'], d['laenge_ausser'] or 'keine', d['duenner_als_v7'] or 'keine', d['daumen'], d['daumen_vorlage'], d['daumen_ausser'] or 'keine', gr(d)))
    z.append('   S17 (e) Krallen kreuzen in den acht Posen: %s (Soll keine) -> %s' % ([pn for pn, k in e['kreuzen'].items() if k] or 'keine', gr(e)))
    z.append('   S17 (f) Mesh 12 Flaechen je CLUT-Zeile %s (Soll 491: 6, 492: 18, Rest 490), falsche Zeile: %d, fremde Texel (u/v ungleich Original-EM30, Mesh 11 + 12): %d, '
             'Mesh 11 UV und CLUT wie V7: %s -> %s' % (f['zeilen_mesh12'], len(f['fehler']), f['fremde_texel'], 'ja' if f['mesh11_clut_wie_v7'] else 'NEIN', gr(f)))
    if 'g' in s:
        g = s['g']
        n, v7, r5 = g['neu'], g['v7'], g['r5d']
        z.append('   S17 (g) Durchdringung, Keyframes von %d: Krallenachse im Koerper %d (V7 %d, r5d %d), Hand-Ecke im Koerper %d (V7 %d, r5d %d), zusammen %d (V7 %d, r5d %d) -> %s; '
                 'in den %d Szenen-Keyframes %d (V7 %d, r5d %d; Soll <= %d): %s -> %s; Clips %s; je Knochen %s -> %s'
                 % (n['keyframes'], n['krallen_kf'], v7['krallen_kf'], r5['krallen_kf'], n['hand_kf'], v7['hand_kf'], r5['hand_kf'], n['zusammen_kf'], v7['zusammen_kf'],
                    r5['zusammen_kf'], 'gruen' if g['ok_alle'] else 'rot', g['szene_kf'], len(g['szene']), len(g['szene_v7']), R5D_SZENE_KF, R5D_SZENE_KF,
                    ' '.join('%d/%d' % (c_, b_) for c_, b_ in g['szene']) or '-', 'gruen' if g['ok_szene'] else 'rot', n['clips'], n['je_knochen'], gr(g)))
        z.append('   S17 (g) je Clip neu %s, V7 %s; gegenueber V7 dazu %s (Bilder: %s), weggefallen %s; tiefste (Clip, Bild, Knochen, Punkte Krallen, Hand): %s'
                 % (g['je_clip'], g['je_clip_v7'], g['dazu_je_clip'] or 'keine', ' '.join('%d/%d' % (c_, b_) for c_, b_ in g['dazu']) or '-', g['weg_je_clip'] or 'keine',
                    [(t['clip'], t['bild'], t['knochen'], t['krallen'], t['hand']) for t in n['tiefste']]))
    return z


# ------------------------------------------------------------------ S20 Handstellung (P7)
# Schranken (alle aus Messwerten, analysis/befunde_runde41/SF_sichel.md Abschnitt "V8 Runde 6c (P7)"):
S20_VERDREHUNG = 5.0      # Grad. V7 3,1 (Capcoms Hand, nur die Rundung der V7-Krallen), Stand 6b 118,0; die Reihe C1 zeigt bei +- 5 Grad
                          # in beiden Haltungs-Massen weniger als 1 Punkt Unterschied, ab 10 Grad 2 Punkte
S20_PUNKTE = 0.05         # Anteil der 912 Keyframes: 5 Punkte = Wirkung von 10 bis 20 Grad Verdrehung in der Reihe C1 (86,0 -> 84,6 / 82,1)
S20_KAMERA = 30.0         # Grad zwischen Handruecken und Blick: cos 30 = 0,87; 0,87 x Vorlage-Median 44,5 px = 38,7 px, das ist die
                          # schmalste Vorlage (37,8 px) - weiter weggedreht kann die breite Stelle die Vorlage nicht mehr erreichen
S20_HOEHE = 0.01          # Figurhoehen: 3 Videopixel bei 307 px Figur = je 1 px Rasterkante an Scheitel, Sohle und Messpunkt


def s20(blob, ref_blob):
    """S20 Handstellung (P7, Nutzer: "sieht es so aus als waere die linke Hand verkehrt herum gedreht").
    (a) Verdrehung der Hand um ihre Laengsachse gegen den Unterarm, verglichen mit Capcoms Original-EM30, ueber alle 912 Keyframes: Betrag des
        Medians <= 5 Grad, kein Keyframe ueber 10 Grad; dazu der Anteil der Keyframes mit Handruecken nach vorn und mit Daumen zur Koerpermitte
        nicht mehr als 5 Punkte unter dem kleineren Wert von V7 und Original (die beiden Staende mit Capcoms Hand-Verhaeltnis).
    (b) In den fuenf Paar-Posen: Winkel zwischen Handruecken und Kamera <= 30 Grad (0 = Handruecken zur Kamera, 180 = Handflaeche) und breite
        Stelle der Hand in der Streuung der Vorlage +- 1 px.
    (c) In den fuenf Paar-Posen: Hoehe von Handgelenk und Krallenspitze ueber der Sohle, in Figurhoehen, in der Streuung der fuenf
        Vorlagebilder +- 0,01."""
    vor = {k: HM.vorlage(k) for k in HM.VORLAGE_BILDER}
    mo, mo7, mo0 = V.modell(blob), V.modell(ref_blob), V.modell(SZ.BF.lade_em30())
    vd, h, h7 = SZ.verdrehung(mo, mo0), SZ.haltung_zahlen(mo, mo0), SZ.haltung_zahlen(mo7, mo0)
    soll_r = min(h7['ruecken_vorn'], h['ruecken_vorn_orig']) - S20_PUNKTE
    soll_d = min(h7['daumen_mitte'], h['daumen_mitte_orig']) - S20_PUNKTE
    a = dict(verdrehung=[round(vd[k], 2) for k in ('min', 'median', 'max')], keyframes=vd['keyframes'], ruecken_vorn=round(h['ruecken_vorn'], 3),
             daumen_mitte=round(h['daumen_mitte'], 3), v7=(round(h7['ruecken_vorn'], 3), round(h7['daumen_mitte'], 3)),
             original=(round(h['ruecken_vorn_orig'], 3), round(h['daumen_mitte_orig'], 3)), soll=(round(soll_r, 3), round(soll_d, 3)))
    a['ok'] = bool(abs(vd['median']) <= S20_VERDREHUNG and max(abs(vd['min']), abs(vd['max'])) <= 2 * S20_VERDREHUNG
                   and h['ruecken_vorn'] >= soll_r and h['daumen_mitte'] >= soll_d)
    P = {pn: HM.modell(mo, HM.POSEN[pn]) for pn in HM.PAARE}
    kam = {pn: SZ.kamera_winkel(mo, HM.POSEN[pn]) for pn in HM.PAARE}
    br = [v['stationen']['hand_breit'] for v in vor.values()]
    b = dict(winkel={pn: round(kam[pn]['winkel'], 1) for pn in kam}, breit={pn: P[pn]['stationen']['hand_breit'] for pn in P}, vorlage=(min(br), max(br)))
    b['ok'] = bool(all(abs(w) <= S20_KAMERA for w in b['winkel'].values()) and all(min(br) - 1.0 <= x <= max(br) + 1.0 for x in b['breit'].values()))
    vw = [v['handgelenk_ueber_boden'] / v['figur_hoehe'] for v in vor.values()]
    vt = [v['spitze_ueber_boden'] / v['figur_hoehe'] for v in vor.values()]
    c = dict(handgelenk={pn: round(P[pn]['handgelenk_ueber_boden'] / P[pn]['figur_hoehe'], 3) for pn in P},
             spitze={pn: round(P[pn]['spitze_ueber_boden'] / P[pn]['figur_hoehe'], 3) for pn in P},
             vorlage_handgelenk=(round(min(vw), 3), round(max(vw), 3)), vorlage_spitze=(round(min(vt), 3), round(max(vt), 3)))
    c['ok'] = bool(all(min(vw) - S20_HOEHE <= x <= max(vw) + S20_HOEHE for x in c['handgelenk'].values())
                   and all(min(vt) - S20_HOEHE <= x <= max(vt) + S20_HOEHE for x in c['spitze'].values()))
    out = dict(a=a, b=b, c=c)
    out['ok'] = all(out[k]['ok'] for k in 'abc')
    out['teile_rot'] = [k for k in 'abc' if not out[k]['ok']]
    return out


def zeilen20(s):
    gr = lambda t: 'GRUEN' if t['ok'] else 'ROT'
    a, b, c = s['a'], s['b'], s['c']
    z = ['S20 Handstellung links (P7): %s%s' % ('GRUEN' if s['ok'] else 'ROT', '' if s['ok'] else ' in (%s)' % ', '.join(s['teile_rot']))]
    z.append('   S20 (a) Verdrehung Hand gegen Unterarm, verglichen mit Capcoms Original, %d Keyframes: min %.2f Median %.2f max %.2f Grad (Soll Median <= %.0f, alle <= %.0f); '
             'Handruecken nach vorn in %.1f %% der Keyframes (V7 %.1f, Original %.1f, Soll >= %.1f), Daumen zur Koerpermitte in %.1f %% (V7 %.1f, Original %.1f, Soll >= %.1f) -> %s'
             % (a['keyframes'], a['verdrehung'][0], a['verdrehung'][1], a['verdrehung'][2], S20_VERDREHUNG, 2 * S20_VERDREHUNG, 100 * a['ruecken_vorn'], 100 * a['v7'][0],
                100 * a['original'][0], 100 * a['soll'][0], 100 * a['daumen_mitte'], 100 * a['v7'][1], 100 * a['original'][1], 100 * a['soll'][1], gr(a)))
    z.append('   S20 (b) Paar-Posen %s: Winkel Handruecken zur Kamera %s Grad (Soll Betrag <= %.0f; 0 = Ruecken zur Kamera, 180 = Handflaeche); breite Stelle %s px '
             '(Vorlage %.1f..%.1f +- 1) -> %s' % (' '.join(b['winkel']), ' '.join('%.1f' % x for x in b['winkel'].values()), S20_KAMERA,
                                                  ' '.join('%.1f' % x for x in b['breit'].values()), b['vorlage'][0], b['vorlage'][1], gr(b)))
    z.append('   S20 (c) Paar-Posen, Hoehe ueber der Sohle in Figurhoehen: Handgelenk %s (Vorlage %.3f..%.3f +- %.2f), Krallenspitze %s (Vorlage %.3f..%.3f +- %.2f) -> %s'
             % (' '.join('%.3f' % x for x in c['handgelenk'].values()), c['vorlage_handgelenk'][0], c['vorlage_handgelenk'][1], S20_HOEHE,
                ' '.join('%.3f' % x for x in c['spitze'].values()), c['vorlage_spitze'][0], c['vorlage_spitze'][1], S20_HOEHE, gr(c)))
    return z


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--v7', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--json')
    ap.add_argument('--ohne-g', action='store_true')
    a = ap.parse_args()
    blob, ref = open(a.emd, 'rb').read(), open(a.v7, 'rb').read()
    s = s17(blob, ref, mit_g=not a.ohne_g)
    t = s20(blob, ref)
    print('Riegel S17 und S20 fuer %s\nsha256 %s' % (a.emd, V.sha(blob)))
    print('\n'.join(zeilen(s) + zeilen20(t)))
    if a.json:
        json.dump(dict(S17=s, S20=t), open(a.json, 'w', encoding='utf-8', newline='\n'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(0 if s['ok'] and t['ok'] else 1)


if __name__ == '__main__':
    main()
