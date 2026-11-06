"""Spur SF "Schwanz V2": Bildleisten fuer den Nutzer (analysis/befunde_runde41/SF_belege/v2/). Kacheln 2x (1280er Koordinaten
wie das Formblatt V2), mindestens 480 px breit, beschriftet, Zahlen unter jeder Kachel.

python leisten_v2.py seite|vorn|halten|gang|gesamt|kopf|enden|rundum  (Pfade unten: EMD = Varianten, LAEUFE = Laufordner)
Spalten: Original (Video 4, Schwarzwert abgezogen) | KI (_slp-Freisteller birkinNN) | Referenz 2 | V1 | V2.
Offline-Kacheln (seite, vorn, kopf, enden): Buehne in der Spielkamera ROOM50A0 Schnitt 12 bei der an das Original angepassten
Pose (anpassen_v2.py, Koerper von Referenz 2); dieselbe Pose fuer alle Modelle. Lauf-Kacheln (halten, gang, gesamt): RE15_FRAMEDUMP
der Laeufe lauf_v2.sh (Ref2 = Spiel-Asset mit dem neuen Bau, V1/V2 = Overlay-Wurzel).
"""
import sys, os, json, re
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import mass_v2 as MS
import koerper_mass as KM
import birkin_buehne as BB
import emd_ansichtsblatt as EA
import birkin_formen as BF

SCR = os.environ.get('SFV2', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2')
PRUEF = os.environ.get('SF_PRUEFER', os.path.join(os.path.dirname(SCR), 'leser', 'P_sichel'))
AUS = os.path.join(b2.BELEGE, 'v2')
EMD = {'Ref2': 'EM30_HANGAR.EMD', 'V1': 'EM30_V1_SPITZ.EMD', 'V2': 'EM30_V2_SPITZ.EMD',
       'V1 Kralle': 'EM30_V1_KRALLE.EMD', 'V2 Kralle': 'EM30_V2_KRALLE.EMD'}
LAEUFE = {'Ref2': 'R2n', 'V1': 'V1s', 'V2': 'V2s', 'V1 Kralle': 'V1k', 'V2 Kralle': 'V2k'}
NAME = {'Ref2': 'Referenz 2 (84e8a4d97, Modell unveraendert)', 'V1': 'V1: Schwanz V2 + Kopf V2, Koerper Ref2',
        'V2': 'V2: Schwanz V2 + Kopf V2 + Koerper K1-K10'}
GRUEN, GELB, MAG, CYAN, ORANGE = (0, 230, 0), (255, 230, 0), (255, 0, 255), (0, 230, 230), (255, 150, 0)
_mo = {}


def modell(k):
    if k not in _mo:
        _mo[k] = b2.modell(os.path.join(SCR, 'emd', EMD[k]))
    return _mo[k]


def posen(art):
    return json.load(open(os.path.join(SCR, 'posen_%s.json' % art)))


def x2(im):
    return cv2.resize(im, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)


def linie(im, y, x0, x1, col, txt=None, gestr=False):
    y = int(round(y))
    if gestr:
        for x in range(int(x0), int(x1), 14):
            cv2.line(im, (x, y), (min(x + 8, int(x1)), y), col, 2)
    else:
        cv2.line(im, (int(x0), y), (int(x1), y), col, 2)
    if txt:
        b2.schrift(im, txt, x1 + 4, y + 5, col, 0.5)


def kachel(im1280, box, titel, unten):
    x0, y0, x1, y1 = box
    t = np.ascontiguousarray(im1280[y0:y1, x0:x1])
    b2.schrift(t, titel, 6, 22, (255, 255, 0), 0.6)
    zeilen = unten if isinstance(unten, (list, tuple)) else [unten]
    band = np.full((22 * len(zeilen) + 8, t.shape[1], 3), 16, np.uint8)
    for i, z in enumerate(zeilen):
        b2.schrift(band, z, 6, 19 + 22 * i, (255, 255, 255), 0.5)
    return np.vstack([t, band])


# ═══════════════════════════════════════════════════ Seitenansicht h83/h85/h92
SEITE_KI = {83: '11', 85: '12', 92: '13'}
SOHLE_O = 871
FB_SEITE = {83: (0.152, 0.32), 85: (0.166, 0.33), 92: (0.154, 0.44)}   # Formblatt V2 Punkt 5/9: Wurzelmitte, Spitze ue. Sohle


def seite_original(h, ki):
    S2 = json.load(open(os.path.join(PRUEF, 'seite2.json')))[SEITE_KI[h]]
    S3 = json.load(open(os.path.join(PRUEF, 'seite3.json')))[SEITE_KI[h]]
    if ki:
        im, _m = b2.ki(h)
    else:
        im = x2(b2.video_sw(h))
    im = np.ascontiguousarray(im)
    ok = S2['OK']; H = SOHLE_O - ok
    rx, ry = S3['root_mitte']
    pfad = np.array(S3['pfad'], float)
    linie(im, ok, 480, 1000, CYAN, 'OK'); linie(im, SOHLE_O, 480, 1000, CYAN, 'Sohle H=%d' % H)
    cv2.polylines(im, [np.r_[[[rx, ry]], pfad].astype(np.int32).reshape(-1, 1, 2)], False, MAG, 2)
    cv2.circle(im, (int(rx), int(ry)), 7, GELB, -1)
    linie(im, ok + 0.16 * H, 820, 1000, GELB, 'Wurzel 0,15-0,17 H')
    sx, sy = pfad[-1]
    linie(im, sy, 440, 620, MAG)
    w, sp = FB_SEITE[h]
    zahl = ['Wurzelmitte %.3f H unter OK (Formblatt Punkt 5)' % w,
            'Dicke 0,2/0,5/0,9: 0,135 / 0,065-0,07 / 0,034 H (S10)' if not ki else 'Dicke KI 0,2: 60-62 px = 0,12 H',
            'Spitze %.2f H ueber Sohle (Punkt 9)' % sp]
    return im, zahl


def seite_modell(k, h, pose):
    mo = modell(k)
    im, _ids = b2.zeichne(mo, pose, b2.video_sw(h))
    m = MS.schwanz_mass(mo, pose, versenkt=0 if k == 'Ref2' else BF.SCHWANZ_V2['versenkt'])
    im = np.ascontiguousarray(x2(im))
    ok, so, H = 2 * m['ok'], 2 * m['sohle'], 2 * m['H']
    linie(im, ok, 480, 1000, CYAN, 'OK'); linie(im, so, 480, 1000, CYAN, 'Sohle')
    # Vorgabe (Formblatt) gruen: Wurzel 0,15-0,17 H, Spitze 0,32-0,44 H ueber der Sohle
    linie(im, ok + 0.15 * H, 820, 1000, GRUEN, gestr=True); linie(im, ok + 0.17 * H, 820, 1000, GRUEN, 'Vorgabe Wurzel', True)
    linie(im, so - 0.32 * H, 430, 600, GRUEN, gestr=True); linie(im, so - 0.44 * H, 430, 600, GRUEN, 'Vorgabe Spitze', True)
    if 'mittellinie' in m:
        P = 2 * m['mittellinie']
        cv2.polylines(im, [P.astype(np.int32).reshape(-1, 1, 2)], False, MAG, 2)
        cv2.circle(im, tuple(int(v) for v in P[0]), 7, GELB, -1)
        cv2.circle(im, tuple(int(v) for v in P[-1]), 6, MAG, -1)
    return im, MS.text(m), m


def leiste_seite():
    P = posen('seite')
    zeilen, tab = [], {}
    for h in (83, 85, 92):
        pose = P[str(h)]
        box = (440, 300, 1000, 920)
        r = []
        im, z = seite_original(h, False); r.append(kachel(im, box, 'Original h%d (Schwarzwert abgezogen) x2' % h, z))
        im, z = seite_original(h, True); r.append(kachel(im, box, 'KI _slp birkin%s h%d' % (b2.KI_VON_H[h], h), z))
        for k in ('Ref2', 'V1', 'V2'):
            im, z, m = seite_modell(k, h, pose)
            r.append(kachel(im, box, '%s h%d (Buehne)' % (k, h), z))
            tab[(h, k)] = m
        zeilen.append(r)
    kopf = ['SF Schwanz V2 - Seitenansicht h83/h85/h92: Original | KI | Referenz 2 | V1 | V2 (V1/V2 mit spitzem Ende)',
            'Linien: cyan OK/Sohle, gelb Wurzelmitte, magenta Mittellinie bis zur Spitze, gruen gestrichelt Vorgabe Formblatt V2 '
            '(Wurzel 0,15-0,17 H unter OK, Spitze 0,32-0,44 H ueber Sohle). Modelle: Buehne in der Spielkamera 50A0/12, Pose '
            'an das Original angepasst (anpassen_v2.py, Koerper Ref2), gleich fuer alle drei.',
            'H = Oberkante bis Sohle im selben Bild. Die Pose des Ports steht aufrechter als das Original (Koerperhaltung = Spur H).']
    b2.leiste(zeilen, kopf, os.path.join(AUS, '01_seite_h83_h85_h92.jpg'))
    return tab


# ═══════════════════════════════════════════════════ Vorderansicht h118/h125
def vorn_original(h, ki):
    im = b2.ki(h)[0] if ki else x2(b2.video_sw(h))
    im = np.ascontiguousarray(im)
    ok, so = (315, 884) if h == 118 else (293, 894)
    H = so - ok
    linie(im, ok, 560, 1010, CYAN, 'OK'); linie(im, so, 560, 1010, CYAN, 'Sohle')
    for f, t in ((0.42, 'Bund 0,42'), (0.57, 'Saum 0,57')):
        linie(im, ok + f * H, 560, 1010, GRUEN, t)
    cv2.rectangle(im, (int(785 - 0.075 * H), int(ok + 0.08 * H)), (int(785 + 0.075 * H), int(ok + 0.26 * H)), (255, 60, 60), 2)
    return im, ['Original: Bund 0,42 / Saum 0,57 H unter OK, Breite 0,57-0,65 H', 'Krallen 0,30-0,36 / Sichel 0,17-0,27 H ue. Sohle',
                'rote Zone 0,08-0,26 H (rot), Kopfkuppe hell']


def vorn_modell(k, h, pose):
    mo = modell(k)
    im, ids = b2.zeichne(mo, pose, b2.video_sw(h))
    m = KM.koerper_mass(mo, pose)
    sicht = int(((ids >= 16) & (ids <= 22)).sum())
    im = np.ascontiguousarray(x2(im))
    ok, so, H = 2 * m['ok'], 2 * m['sohle'], 2 * m['H']
    linie(im, ok, 560, 1010, CYAN, 'OK'); linie(im, so, 560, 1010, CYAN, 'Sohle')
    for f, t in ((0.42, 'Vorgabe Bund'), (0.57, 'Vorgabe Saum')):
        linie(im, ok + f * H, 560, 1010, GRUEN, t, True)
    for key, t in (('bund', 'Bund'), ('saum', 'Saum')):
        if m[key] is not None:
            linie(im, ok + m[key] * H, 560, 1010, ORANGE, t)
    z = KM.text(m) + ['Schwanz sichtbar: %d Bildpunkte (640er)' % sicht]
    return im, z, dict(m, schwanz_sichtbar=sicht)


def leiste_vorn():
    P = posen('vorn')
    zeilen, tab = [], {}
    for h in (118, 125):
        pose = P[str(h)]
        box = (500, 260, 1100, 940)
        r = []
        im, z = vorn_original(h, False); r.append(kachel(im, box, 'Original h%d x2' % h, z))
        im, z = vorn_original(h, True); r.append(kachel(im, box, 'KI birkin%s h%d' % (b2.KI_VON_H[h], h), z))
        for k in ('Ref2', 'V1', 'V2'):
            im, z, m = vorn_modell(k, h, pose)
            r.append(kachel(im, box, '%s h%d (Buehne)' % (k, h), z)); tab[(h, k)] = m
        zeilen.append(r)
    kopf = ['SF Schwanz V2 - Vorderansicht h118/h125: Original | KI | Referenz 2 | V1 | V2',
            'gruen Vorgabe Formblatt V2 (K3/K4), orange Ist (Jeans-Bildpunkte), cyan OK/Sohle. Schwanz von vorn hinter dem Koerper (S15).',
            'Buehne bei der an das Original angepassten Pose (anpassen_v2.py), H mit der wahren Sohle (nicht der Letterbox).']
    b2.leiste(zeilen, kopf, os.path.join(AUS, '02_vorn_h118_h125.jpg'))
    return tab


# ═══════════════════════════════════════════════════ Lauf-Kacheln
def lauf_bild(run, h):
    os.environ.setdefault('NB6_RUNS', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/r')
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'nb2neu'))
    import c6
    im = c6.port(os.path.join(SCR, 'r', run), h)
    return None if im is None else cv2.cvtColor(im, cv2.COLOR_BGR2RGB)


def lauf_log(run):
    R = {}
    for L in open(os.path.join(SCR, 'r', run, 'debug.log'), errors='ignore'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) F\d+ B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)', L)
        if m:
            R.setdefault(int(m.group(1)), {})['B'] = [int(m.group(i)) for i in range(2, 7)]
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanz B y (-?\d+) (.*)', L)
        if m:
            R.setdefault(int(m.group(1)), {})['kette'] = [tuple(int(v) for v in t) for t in
                                                          re.findall(r'\((-?\d+),(-?\d+),(-?\d+)\)', m.group(3))]
    return R


def griff(run):
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'nb2neu'))
    import griff6
    return griff6.abstaende(os.path.join(SCR, 'r', run))


def lauf_zeile(h, spalten, box, unten_fn, ki_spalte=False):
    r = [kachel(x2(b2.video_sw(h)), box, 'Original h%d x2' % h, unten_fn('Video', h))]
    if ki_spalte:
        k = b2.ki(h)[0]
        r.append(kachel(k if k is not None else np.zeros((960, 1280, 3), np.uint8), box, 'KI h%d' % h, ['']))
    for k in spalten:
        im = lauf_bild(LAEUFE[k], h)
        im = np.zeros((480, 640, 3), np.uint8) if im is None else im
        r.append(kachel(x2(im), box, '%s h%d (Lauf %s)' % (k, h, LAEUFE[k]), unten_fn(k, h)))
    return r


def leiste_halten():
    G = {k: griff(LAEUFE[k]) for k in ('Ref2', 'V1', 'V2')}
    Lg = {k: lauf_log(LAEUFE[k]) for k in ('Ref2', 'V1', 'V2')}

    def unten(k, h):
        if k == 'Video':
            return ['Formblatt S14: h24 waagerecht B-links, h61 B-rechts schraeg zum Boden']
        d = G[k].get(h); kt = Lg[k].get(h, {}).get('kette')
        z = ['Griff-Abstand %d E. / %d px' % (d['welt'], d['bild']) if d else 'kein Griff']
        if kt:
            z.append('Spitze %d E. ueber dem Boden' % (-kt[-1][1]))
        return z
    zeilen = [lauf_zeile(h, ('Ref2', 'V1', 'V2'), (160, 60, 1120, 900), unten, True) for h in (24, 61)]
    b2.leiste(zeilen, ['SF Schwanz V2 - Halten h24/h61: Original | KI | Referenz 2 | V1 | V2 (Spiellaeufe, RE15_FRAMEDUMP)',
                       'Schwanzbahn der Szene (NB3/NB4) unveraendert; fuer V2 liegen die Durchgangspunkte 550 hinter der Koerperebene '
                       '(birkin_formen.c BF_V2_VIA_TIEFE), weil die Wurzel oben am Buckel sitzt.'],
              os.path.join(AUS, '03_halten_h24_h61.jpg'))


def leiste_gang():
    Lg = {k: lauf_log(LAEUFE[k]) for k in ('Ref2', 'V1', 'V2')}
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'nb2neu'))
    import beine6
    B = {k: beine6.beine(os.path.join(SCR, 'r', LAEUFE[k])) for k in ('Ref2', 'V1', 'V2')}

    def unten(k, h):
        if k == 'Video':
            return ['Original: Schwanzbogen h77-h98 (Seite), ab h101 hinter dem Koerper']
        z = []
        b = B[k].get(h)
        if b:
            z.append('Knie %d/%d Grad' % (b['knie_r'], b['knie_l']))
        e = Lg[k].get(h, {})
        if 'B' in e and 'kette' in e:
            clip, bild, gier, x, zz = e['B']
            mo = modell(k)
            pose = dict(clip=clip, bild=bild, gier=gier, x=x, z=zz)
            _i, ids = b2.zeichne(mo, pose, None, licht=False)
            ys = np.nonzero(((ids >= 0) & (ids < 16)).any(1))[0]
            H = ys.max() - ys.min() if len(ys) else 1
            sicht = int(((ids >= 16) & (ids <= 22)).sum())
            sp = -e['kette'][-1][1]
            z.append('Spitze %d E. ue. Boden; Schwanz sichtbar ~%d px' % (sp, sicht))
        return z
    hs = list(range(84, 133, 6))
    zeilen = [lauf_zeile(h, ('Ref2', 'V1', 'V2'), (300, 140, 1060, 900), unten) for h in hs]
    b2.leiste(zeilen, ['SF Schwanz V2 - Gang h84-h132: Original | Referenz 2 | V1 | V2 (Spiellaeufe)',
                       'Knie = Kniewinkel (beine6.py). Spitze ueber dem Boden aus der Messzeile (Kette). "Schwanz sichtbar" = Buehne bei '
                       'der Lauf-Pose (Keyframe-Schwanz), Naeherung.'],
              os.path.join(AUS, '04_gang_h84-h132.jpg'))


def leiste_gesamt():
    G = {k: griff(LAEUFE[k]) for k in ('Ref2', 'V1')}

    def unten(k, h):
        if k == 'Video':
            return ['']
        d = G[k].get(h)
        return ['Griff-Abstand %d E.' % d['welt'] if d else '-']
    for teil, hs in (('a', range(0, 67, 6)), ('b', range(72, 133, 6))):
        zeilen = [lauf_zeile(h, ('Ref2', 'V1'), (60, 40, 1160, 900), unten) for h in hs]
        b2.leiste(zeilen, ['SF Schwanz V2 - Gesamtleiste V1 h%d-h%d: Original | Referenz 2 | V1 (Spiellaeufe R2n / V1s)' % (hs[0], hs[-1]),
                           'Ansprung, Griff (Abstand Sichelhand - Griffknochen), Wurf, Flucht, Gang - die Szene muss als Ganzes stimmen.'],
                  os.path.join(AUS, '06%s_gesamt_v1_h%d-h%d.jpg' % (teil, hs[0], hs[-1])))


# ═══════════════════════════════════════════════════ Kopf, Enden, Rundum
def leiste_kopf():
    Pv, Ps = posen('vorn'), posen('seite')
    zeilen = []
    for h, P, art in ((118, Pv, 'vorn'), (125, Pv, 'vorn'), (85, Ps, 'Seite')):
        pose = P[str(h)]
        k, _m = b2.ki(h)
        mo = modell('Ref2')
        pk = b2.projiziere(pose, b2.kette_punkte(mo, BB.kf_von_clip(mo, pose['clip'], pose['bild']))[:1])
        if art == 'vorn':
            box = (640, 260, 940, 560)
        else:
            box = (720, 330, 1000, 630)
        f = 2
        r = []
        kk = cv2.resize(k[box[1]:box[3], box[0]:box[2]], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
        r.append(kachel(kk, (0, 0, kk.shape[1], kk.shape[0]), 'KI birkin%s h%d (%s)' % (b2.KI_VON_H[h], h, art),
                        ['KI: offenes Maul mit Zaehnen, Auge oben, Schulterauge B-links']))
        for key in ('Ref2', 'V1', 'V2'):
            im, _ids = b2.zeichne(modell(key), pose, b2.video_sw(h))
            im = x2(im)[box[1]:box[3], box[0]:box[2]]
            im = cv2.resize(im, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
            r.append(kachel(im, (0, 0, im.shape[1], im.shape[0]), '%s h%d (%s, Buehne)' % (key, h, art),
                            ['Maul/Zaehne/Auge = Textur auf EM30-Kopfgeometrie' if key != 'Ref2' else 'Referenz 2: Wundfleisch, kein Gesicht']))
        zeilen.append(r)
    b2.leiste(zeilen, ['SF Schwanz V2 - Kopf nah: KI | Referenz 2 | V1 | V2 (Nutzer-Entscheid F4 2026-11-06, PORT-WAHL, Quelle KI)',
                       'Spielkamera 50A0/12, angepasste Pose, Ausschnitt x4. Helle Kuppe und rote Zone nach dem Original (K1/K2).'],
              os.path.join(AUS, '05_kopf_nah.jpg'))


def leiste_enden():
    P = posen('seite')
    pose = P['85']
    zeilen = []
    for gruppe in (('V1', 'V1 Kralle'), ('V2', 'V2 Kralle')):
        r = []
        for k in gruppe:
            mo = modell(k)
            im, _ids = b2.zeichne(mo, pose, b2.video_sw(85))
            P3 = b2.kette_punkte(mo, BB.kf_von_clip(mo, pose['clip'], pose['bild']))
            sp = b2.projiziere(pose, P3[-1:])[0] * 2
            x0, y0 = int(sp[0] - 130), int(sp[1] - 150)
            c = x2(im)[max(0, y0):y0 + 260, max(0, x0):x0 + 260]
            c = cv2.resize(c, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
            r.append(kachel(c, (0, 0, c.shape[1], c.shape[0]), '%s h85 Schwanzende x4' % k,
                            ['spitz: Dorn 4 x Endradius' if 'Kralle' not in k else 'Kralle: EM30-Klauenhand (Mesh 12) x1,4/1,2']))
        zeilen.append(r)
    k, _m = b2.ki(85)
    kk = cv2.resize(k[560:820, 520:780], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    zeilen.append([kachel(kk, (0, 0, kk.shape[1], kk.shape[0]), 'KI birkin12 h85 Schwanzende x2', ['KI: Hand mit roter Kralle (Seite)']),
                   kachel(x2(b2.video_sw(85))[560:820, 520:780].repeat(2, 0).repeat(2, 1), (0, 0, 520, 520), 'Original h85 x4',
                          ['Original loest das Ende nicht auf (F2)'])])
    b2.leiste(zeilen, ['SF Schwanz V2 - Schwanzende (F2): spitz | Kralle, je V1 und V2; KI und Original zum Vergleich'],
              os.path.join(AUS, '07_schwanzende_spitz_kralle.jpg'))


def leiste_rundum():
    for k in ('Ref2', 'V1', 'V2', 'V1 Kralle', 'V2 Kralle'):
        mo = modell(k)
        tafeln = []
        for clip, bild, zusatz in ((1, 0, 'Gang B0'),):
            kf = BB.kf_von_clip(mo, clip, bild)
            V, N, F = mo.geometrie(kf)
            mitte = (V.min(0) + V.max(0)) / 2
            ausd = (V.max(0) - V.min(0)).max()
            for grad, name in ((0, 'vorn'), (60, ''), (120, 'hinten links'), (180, 'hinten'), (240, ''), (270, 'rechte Flanke')):
                a = np.radians(grad)
                depth = np.array([np.cos(a), 0.0, np.sin(a)])          # wie birkin_buehne.rundum: 0 Grad = vorn
                right = np.array([-np.sin(a), 0.0, np.cos(a)])
                col, alp = EA.render(mo, V - mitte, N, F, right, np.array([0.0, 1.0, 0.0]), depth, 480, 480, 480 * 0.92 / ausd, 240, 240)
                b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8)
                b[alp == 0] = (92, 96, 104)
                tafeln.append(kachel(b, (0, 0, 480, 480), '%s %d Grad %s' % (k, grad, name), ['']))
        # Nah: Kopf vorn und Schwanz rechte Flanke
        kf = BB.kf_von_clip(mo, 1, 0)
        V, N, F = mo.geometrie(kf)
        R, T = mo.pose(kf)
        for titel, mitte, right, depth, sk in (
                ('Kopf vorn nah', T[9].astype(float) + np.array([150.0, -150, 0]), np.array([0, 0, 1.0]), np.array([1.0, 0, 0]), 0.45),
                ('Schwanz rechte Flanke nah', (b2.kette_punkte(mo, kf).mean(0)), np.array([1.0, 0, 0]), np.array([0, 0, -1.0]), 0.17)):
            col, alp = EA.render(mo, V - mitte, N, F, right, np.array([0.0, 1.0, 0.0]), depth, 480, 480, sk, 240, 240)
            b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8)
            b[alp == 0] = (92, 96, 104)
            tafeln.append(kachel(b, (0, 0, 480, 480), '%s %s' % (k, titel), ['']))
        zeilen = [tafeln[:4], tafeln[4:]]
        b2.leiste(zeilen, ['SF Schwanz V2 - Rundum %s (Clip 1 Bild 0, neutrales Licht), dazu Kopf und Schwanz nah' % k],
                  os.path.join(AUS, '08_rundum_%s.jpg' % k.replace(' ', '_')))


def main():
    os.makedirs(AUS, exist_ok=True)
    was = sys.argv[1:] or ['seite', 'vorn', 'halten', 'gang', 'gesamt', 'kopf', 'enden', 'rundum']
    tab = {}
    for w in was:
        r = globals()['leiste_' + w]()
        if isinstance(r, dict):
            tab.update({'%s %s' % k: {kk: (vv if not isinstance(vv, np.ndarray) else None) for kk, vv in v.items()}
                        for k, v in r.items()})
        print('fertig', w, flush=True)
    if tab:
        json.dump(tab, open(os.path.join(AUS, 'masse_%s.json' % '_'.join(was)), 'w'), indent=1, default=float)


if __name__ == '__main__':
    main()
