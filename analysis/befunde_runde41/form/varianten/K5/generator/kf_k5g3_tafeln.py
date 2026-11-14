#!/usr/bin/env python3
"""K5g3 Tafeln (roh, kein Gamma; gleiche Kette/gleiches Licht je Zeile fuer alle Spalten) - Fortschreibung von kf_k5g2_tafeln.py (Tafeln 01-09):
    python kf_k5g3_tafeln.py <K5g3-Ordner> <aus-Ordner> [nur=01,02,...]
01/02 Bild 1/2: Vorlage | K5e heute | K5g; 03 Details nah; 04 echtes Spielfenster (gdigrab, kf_k5g2_fenster.sh); 05 Kopf/Hals von unten;
06 rundum weiss (K5c | K5e | K5g); 07 Familie (G4 | EM30 | K5e | K5g); 08 Gegner im Spiellicht (Framedumps kf_k5f_spiel.sh);
09 Zonen je Nutzerbild (Nutzerbild | K5g3 gleiche Pose, Flecken nah). 04 liest <K5g3-Ordner>/spiel/{heute,k5g}_*.png (kf_k5g3_fenster.sh),
08 liest die Framedumps der Original-Gegner (Messer K5f, scratchpad k5f/spiel, nur lesen) und die eigenen K5e_f1/K5g3-Paare (kf_k5g3_zlauf.sh).
"""
import json
import os
import shutil
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import riegel_k5d as R  # noqa: E402
import kf_k5d_tafeln as T  # noqa: E402
import kf_k5e_tafeln as T5  # noqa: E402
import kf_k5g2_tex as TX  # noqa: E402

v4f = R.v4f
reihe, stapeln, kachel, speichern = T.reihe, T.stapeln, T.kachel, T.speichern
K5E = os.path.join(K5, 'ruhe', 'K5e_f1')
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f/spiel'
SCR = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/k5g3tex'
SP3 = SCR + '/spiel'
NUTZER = 'C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'


def nah(mo, clip, az, el, meshes=(21, 22, 23), gross=760):
    img, K = rl.weiss_id(mo, clip, az, el, gross)
    m = np.isin(K['mesh'], meshes)
    if not m.any():
        return cv2.resize(img, (300, 300), interpolation=cv2.INTER_AREA)
    ys, xs = np.nonzero(m)
    cy, cx = int((ys.min() + ys.max()) / 2), int((xs.min() + xs.max()) / 2)
    r = int(max(ys.max() - ys.min(), xs.max() - xs.min()) / 2 + 12)
    a = img[max(cy - r, 0):cy + r, max(cx - r, 0):cx + r]
    return cv2.resize(a, (300, 300), interpolation=cv2.INTER_NEAREST)


def figur(normal, key, bild, hoehe=300, sp=None):
    sp = sp or SP
    n = cv2.cvtColor(cv2.imread(os.path.join(sp, normal, bild)), cv2.COLOR_BGR2RGB)
    k = cv2.cvtColor(cv2.imread(os.path.join(sp, key, bild)), cv2.COLOR_BGR2RGB)
    m = (n != k).any(2)
    ys, xs = np.nonzero(m)
    y0, y1, x0, x1 = max(ys.min() - 6, 0), ys.max() + 7, max(xs.min() - 6, 0), xs.max() + 7
    a = n[y0:y1, x0:x1].copy()
    f = hoehe / float(a.shape[0])
    return cv2.resize(a, (max(int(a.shape[1] * f), 1), hoehe), interpolation=cv2.INTER_NEAREST), int(m.sum())


ZTAG = None


def main():
    global ZTAG
    neu, aus = sys.argv[1], sys.argv[2]
    import hashlib
    ZTAG = hashlib.sha256(open(os.path.join(neu, 'BAHNSTEIG_FORM.TIM'), 'rb').read()).hexdigest()[:8]
    nur = sys.argv[3].split(',') if len(sys.argv) > 3 else None
    os.makedirs(aus, exist_ok=True)
    def soll(n):
        return nur is None or n in nur
    st = None
    vor = {n: v4f.original(n) for n in (1, 2)}
    if soll('01') or soll('02') or soll('03'):
        st = {'K5e heute': R.Stand(K5E), 'K5g3': R.Stand(neu)}
    for n, datei in ((1, '01_bild1.png'), (2, '02_bild2.png')):
        if not soll(datei[:2]):
            continue
        z1 = reihe([vor[n]] + [st[k].d[n]['sauber'] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] + ['%s: Disc-Licht x k, ohne Filter' % k for k in st])
        z2 = reihe([vor[n]] + [st[k].d[n]['video'][15] for k in st], ['Vorlage Bild %d (Video-Standbild)' % n] + ['%s: Disc-Licht x k + Videofilter Saat 15' % k for k in st])
        speichern(os.path.join(aus, datei), stapeln([z1, z2], 'K5g3, Bild %d: Vorlage | K5e heute (Spiel-Asset) | K5g. Gleiche Szenenkette je Zeile, roh, kein Gamma.' % n))
    mods = None
    if any(soll(x) for x in ('03', '05', '06', '07', '10')):
        mods = {'K5c': rl.fl.lade_var(rl.BASIS), 'K5e': rl.fl.lade_var(K5E), 'K5g3': rl.fl.lade_var(neu)}
    if soll('03'):
        zeilen = []
        fall = [('Schwanzspitze (Bild 1)', 1, (205, 380, 345, 480)), ('Beine/Musterung (Bild 1)', 1, (190, 250, 440, 400)),
                ('Buckel vorn (Bild 2)', 2, (240, 140, 380, 230)), ('Kopf/Schultern vorn (Bild 2)', 2, (200, 190, 440, 290)),
                ('Krallen vorn (Bild 2)', 2, (150, 290, 500, 480))]
        for titel, n, box in fall:
            f = 3 if (box[2] - box[0]) <= 150 else 2
            for art in ('video', 'sauber'):
                bilder = [kachel(vor[n], box, f)] + [kachel(st[k].d[n]['video'][15] if art == 'video' else st[k].d[n]['sauber'], box, f) for k in st]
                zeilen.append(reihe(bilder, ['Vorlage: ' + titel, 'K5e heute (%s)' % ('Videofilter' if art == 'video' else 'ohne Filter'),
                                             'K5g3 (%s)' % ('Videofilter' if art == 'video' else 'ohne Filter')]))
        speichern(os.path.join(aus, '03_details_nah.png'), stapeln(zeilen, 'K5g3 Details nah: Vorlage | K5e heute | K5g3 (Ausschnitte x2/x3 nearest, Kette wie 01/02, roh)'))
    if soll('04'):
        F = os.path.join(os.path.dirname(aus.rstrip('/\\')), 'spiel')
        lade = lambda p: cv2.cvtColor(cv2.imread(os.path.join(F, p)), cv2.COLOR_BGR2RGB)
        leer = np.full((480, 640, 3), 24, np.uint8)
        z = []
        for titel, n, datei in (('5080 Schnitt 1, Bild-1-Lage (Clip 0 Bild 11)', 1, 'pose1_r5080.png'),
                                ('5080 Schnitt 1, von vorn (Clip 12 Bild 0)', 0, 'vorn_r5080.png'),
                                ('5090 Schnitt 11, Bild-2-Pose (Clip 12 Bild 0)', 2, 'pose2_r5090_s11.png'),
                                ('5090 Schnitt 11, Griff (Clip 27 Bild 20, Hals-Unterseite)', 0, 'griff_r5090_s11.png')):
            z.append(reihe([v4f.original(n) if n else leer, lade('heute_' + datei), lade('k5g_' + datei)],
                           ['Original-Standbild Bild %d' % n if n else '(kein Original-Standbild zu dieser Pose)', 'K5e heute: ' + titel, 'K5g3: ' + titel]))
        speichern(os.path.join(aus, '04_spiel.png'), stapeln(z, 'K5g3 im echten Spielfenster (gdigrab des eigenen Fensters, kein AUTOSHOT/SOFTWARE_RENDER, Variante ueber '
                                                                'RE15_RE2_ASSET_ROOT): Original | K5e heute | K5g3, roh'))
    if soll('05'):
        zeilen = []
        for clip in (2, 1):
            for az, el in ((60, 10), (0, -60), (90, -45)):
                ks = [(nah(mods[k], clip, az, el), '%s Clip %d (%s) %d/%d' % (k, clip, 'Ruhe' if clip == 2 else 'Maul offen', az, el)) for k in ('K5c', 'K5e', 'K5g3')]
                zeilen.append(reihe([a for a, _t in ks], [t for _a, t in ks]))
        speichern(os.path.join(aus, '05_kopf_hals_unten.png'), stapeln(zeilen, 'Kopf, Lippe, Unterkiefer und Hals-Unterseite nah, weisses Licht: K5c | K5e heute | K5g3, '
                                                                          'Clip 2 Ruhe und Clip 1 Maul offen, roh'))
    if soll('06'):
        zeilen = [reihe([T5.weiss(mods[k], 2, az, el) for k in ('K5c', 'K5e', 'K5g3')], ['%s, %s (Clip 2)' % (k, wn) for k in ('K5c', 'K5e heute', 'K5g3')])
                  for wn, az, el in rl.WINKEL4]
        speichern(os.path.join(aus, '06_rundum.png'), stapeln(zeilen, 'Rundum unter weissem Licht (Tafel-07-Licht, 4 Winkel, Ruhe-Clip): K5c | K5e heute | K5g3, roh'))
    if soll('07'):
        g4 = rl.PL.re2(0x34)
        g4.T = rl._Tex(g4)
        em = rl.fl.lade_em30()
        fam = (('RE2-G4 Original (Clip 0)', g4, 0), ('Hangar-Birkin EM30 (Clip 0)', em, 0), ('K5e heute (Clip 2)', mods['K5e'], 2), ('K5g3 (Clip 2)', mods['K5g3'], 2))
        zeilen = [reihe([T5.weiss(mo, c, az, el) for _t, mo, c in fam], ['%s, %s' % (t, wn) for t, _mo, _c in fam]) for wn, az, el in rl.WINKEL4]
        speichern(os.path.join(aus, '07_familie.png'), stapeln(zeilen, 'Familie unter weissem Licht, gleiche Behandlung: RE2-G4 | Hangar-Birkin RE1.5 | K5e heute | K5g3, roh'))
    if soll('08'):
        reihen_ = [('Zombie EM10 1140 (Raumlicht)', 'zombie_n', 'zombie_k', 'dump_000600.ppm'), ('Gorilla EM27 11C0', 'gorilla_normal', 'gorilla_key', None),
                   ('EM30 Hangar 50A0 Schnitt 4', 'hc4_normal', 'hc4_key', 'dump_000600.ppm'), ('Tyrant EM2B 50A0 Schnitt 4', 'hc4_normal', 'hc4_key2b', 'dump_000600.ppm'),
                   ('K5e heute 5080 Bild-1-Lage', 'f80_riegel_k5e_f1_normal', 'f80_riegel_k5e_f1_key', 'dump_000440.ppm'),
                   ('K5g3 5080 Bild-1-Lage', 'f80_riegel_%s_normal' % ZTAG, 'f80_riegel_%s_key' % ZTAG, 'dump_000440.ppm')]
        bilder, titel = [], []
        for t, n, k, b in reihen_:
            if b is None:
                best = None
                for f in sorted(os.listdir(os.path.join(SP, n))):
                    if f.endswith('.ppm') and os.path.exists(os.path.join(SP, k, f)):
                        c_ = (cv2.imread(os.path.join(SP, n, f)) != cv2.imread(os.path.join(SP, k, f))).any(2).sum()
                        if best is None or c_ > best[0]:
                            best = (c_, f)
                b = best[1]
            a, px = figur(n, k, b, sp=SP3 if n.startswith('f80_riegel') else SP)
            bilder.append(a)
            titel.append('%s (%s, %d px)' % (t, b[:-4], px))
        speichern(os.path.join(aus, '08_gegner_spiellicht.png'), stapeln([reihe(bilder[:3], titel[:3]), reihe(bilder[3:], titel[3:])],
                                                                         'Gegner im eigenen Raumlicht, Framedump des echten Renderers, Ausschnitt um die Figur, gleiche Hoehe 300 px, roh'))
    if soll('09'):
        t09(neu, aus)
    print('Tafeln fertig:', aus)


def t09(neu, aus):
    """je Nutzerbild: Nutzerbild | K5g3 in derselben Pose (Bild 1 bzw. 2, ohne Filter) mit den Zonen-Stellen; dazu die Flecken nah (K5e | K5g)."""
    Z = TX.lade_zonen()
    st = {'K5e heute': R.Stand(K5E), 'K5g3': R.Stand(neu)}
    zeilen = []
    dateien = sorted(f for f in os.listdir(NUTZER) if f.lower().endswith('.png'))
    for dname in dateien:
        b = cv2.imdecode(np.fromfile(os.path.join(NUTZER, dname), np.uint8), cv2.IMREAD_UNCHANGED)
        if b is None:
            continue
        if b.shape[2] == 4:
            a = b[..., 3:4] / 255.0
            b = (b[..., :3] * a + 255 * (1 - a)).astype(np.uint8)
        b = cv2.cvtColor(b, cv2.COLOR_BGR2RGB)
        zs = [z for z in Z['zonen'] if z.get('nutzerbild') and z['nutzerbild'].replace('ü', 'u') == dname.replace('ü', 'u')]
        n = zs[0]['bild'] if zs else (2 if 'vorn' in dname.lower() or 'gesicht' in dname.lower() else 1)
        h = 360
        b = cv2.resize(b, (int(b.shape[1] * h / b.shape[0]), h), interpolation=cv2.INTER_AREA)
        sa = st['K5e heute'].d[n]['sauber']; sb = st['K5g3'].d[n]['sauber']
        zeilen.append(reihe([b, cv2.resize(sa, (480, 360)), cv2.resize(sb, (480, 360))],
                            ['Ihr Bild: %s' % dname, 'K5e heute, Bild %d gleiche Pose' % n, 'K5g3, Bild %d gleiche Pose' % n]))
    # Flecken nah (weisses Licht, Ausschnitt um die Traegerflaechen)
    mods = {'K5e': rl.fl.lade_var(K5E), 'K5g3': rl.fl.lade_var(neu)}
    tj = json.load(open(TX.TRAEGER, encoding='utf-8'))['zonen']
    nah_z = []
    for nr in ('2', '3', '9', '11', '12', '14', '16', '18', '19', '24', '27', '29'):
        teile = sorted(set(TX.schluessel(k)[0] for k in tj[nr]))
        z = next(zz for zz in Z['zonen'] if zz['nr'] == nr)
        az, el = (90, 10) if z['bild'] == 2 else (-90, 15)
        for k in ('K5e', 'K5g3'):
            nah_z.append((nah(mods[k], 2, az, el, meshes=teile), '%s: %s (%s)' % (k, z['nutzerwort'][:34], z['nr'])))
    for i in range(0, len(nah_z), 6):
        zeilen.append(reihe([a for a, _t in nah_z[i:i + 6]], [t for _a, t in nah_z[i:i + 6]]))
    speichern(os.path.join(aus, '09_zonen.png'), stapeln(zeilen, 'Ihre Bilder neben dem Modell in derselben Pose (ohne Videofilter) und Ihre Flecken nah unter weissem Licht: '
                                                                 'je Fleck K5e heute | K5g3, roh'))




if __name__ == '__main__':
    main()
