"""Belegbilder V3B_* (ohne Aufhellung; Rueckrechnung e wird roh gezeigt, beschriftet)."""
from gem import *
import json, glob
OUT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/'
NT = FORM + '/nutzer_teile/'
R = json.load(open(S + 'messung.json'))
KI = sorted(glob.glob(r'C:/workspace/git/reAi_v2/Version_80%/vlcsnap-2026-11-04-16h2*.png'))
FONT = cv2.FONT_HERSHEY_SIMPLEX


def gross(img, box, s):
    x0, y0, x1, y1 = box
    return cv2.resize(np.ascontiguousarray(img[y0:y1, x0:x1]), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)


def kontur(a, m, box, s, f, d=1):
    x0, y0, x1, y1 = box
    mm = cv2.resize(m[y0:y1, x0:x1].astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
    cs, _ = cv2.findContours(mm, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, cs, -1, f, d)


def gitter(a, box, s, schritt=50):
    x0, y0, x1, y1 = box
    for x in range(x0 - x0 % schritt + schritt, x1, schritt):
        cv2.line(a, ((x - x0) * s, 0), ((x - x0) * s, 8), (0, 255, 0), 1)
        cv2.putText(a, str(x), ((x - x0) * s + 2, 20), FONT, 0.42, (0, 255, 0), 1)
    for y in range(y0 - y0 % schritt + schritt, y1, schritt):
        cv2.line(a, (0, (y - y0) * s), (8, (y - y0) * s), (0, 255, 0), 1)
        cv2.putText(a, str(y), (10, (y - y0) * s + 5), FONT, 0.42, (0, 255, 0), 1)


def titel(a, t):
    cv2.rectangle(a, (0, a.shape[0] - 22), (a.shape[1], a.shape[0]), (25, 25, 25), -1)
    cv2.putText(a, t, (4, a.shape[0] - 7), FONT, 0.45, (235, 235, 235), 1)
    return a


def kopf(t, w):
    a = np.full((26, w, 3), 18, np.uint8)
    cv2.putText(a, t, (6, 18), FONT, 0.5, (235, 235, 235), 1)
    return a


def reihe(panels):
    h = max(p.shape[0] for p in panels)
    ps = [cv2.copyMakeBorder(p, 0, h - p.shape[0], 0, 4, cv2.BORDER_CONSTANT, value=(18, 18, 18)) for p in panels]
    return np.hstack(ps)


def knochenbild(kn, box, s):
    rng = np.random.default_rng(3)
    lut = rng.integers(60, 230, (40, 3)).astype(np.uint8)
    a = np.full(kn.shape + (3,), 30, np.uint8)
    m = kn >= 0
    a[m] = lut[kn[m]]
    g = gross(a, box, s)
    x0, y0, x1, y1 = box
    for b in np.unique(kn[y0:y1, x0:x1]):
        if b < 0:
            continue
        ys, xs = np.nonzero(kn[y0:y1, x0:x1] == b)
        if len(xs) < 30:
            continue
        cv2.putText(g, str(b), (int(np.median(xs) * s), int(np.median(ys) * s)), FONT, 0.5, (0, 0, 0), 2)
        cv2.putText(g, str(b), (int(np.median(xs) * s), int(np.median(ys) * s)), FONT, 0.5, (255, 255, 255), 1)
    return g


# --------------------------- V3B_1: Bild 1, Stelle F1 ---------------------------
V1, e1, T1, c1 = lade(1)
F1 = np.load(S + 'F1.npy'); kopf1 = np.load(S + 'kopf1.npy'); kn1 = np.load(S + 'fid1.npz')['kn']
L1 = lab(e1).reshape(480, 640, 3)
kern = F1 & (L1[..., 0] >= 20)
box = (270, 190, 420, 330); s = 4
pan = []
for nm, img in (('Original Bild 1 (Video, roh)', V1), ('Rueckrechnung e (Schirmfarbe, roh)', np.clip(e1, 0, 255).astype(np.uint8))):
    g = gross(img, box, s)[:, :, ::-1].copy()
    kontur(g, F1, box, s, (255, 0, 255), 2)
    kontur(g, kern, box, s, (255, 255, 255), 1)
    kontur(g, kopf1, box, s, (0, 255, 255), 1)
    gitter(g, box, s)
    pan.append(titel(g, nm))
g = knochenbild(kn1, box, s)[:, :, ::-1].copy()
kontur(g, F1, box, s, (255, 0, 255), 2); kontur(g, kopf1, box, s, (0, 255, 255), 1); gitter(g, box, s)
pan.append(titel(g, 'K5c Knochen-ID Bild-1-Pose (0 = Becken)'))
ki = cv2.imread(KI[0])
kb = (box[0] * 2, box[1] * 2, box[2] * 2, box[3] * 2)
g = gross(ki, kb, 2).copy()
kontur(g, cv2.resize(F1.astype(np.uint8), (1280, 960), interpolation=cv2.INTER_NEAREST) > 0, kb, 2, (255, 0, 255), 2)
pan.append(titel(g, 'KI-Fassung 16h26m21 (nur Zone, keine Farbe)'))
blatt = reihe(pan)
blatt = np.vstack([kopf('V3B_1 Bild 1 x4, x 270-420 y 190-330. Magenta = F1 (e: L*>=12, a*>=8, 708 px), weiss = Kern L*>=20 (307 px), gelb = K5c Hals/Kopf/Kiefer ohne Verdeckung (Knochen 21-23). Kein Bild aufgehellt.', blatt.shape[1]), blatt])
cv2.imwrite(OUT + 'V3B_1_bild1_F1_x4.png', blatt)

# --------------------------- V3B_2: Bild 2, Maul ---------------------------
V2, e2, T2, c2 = lade(2)
kl = np.load(S + 'kl2.npy'); kn2 = np.load(S + 'fid2.npz')['kn']
uk = cv2.imread(NT + 'maske_unterkiefer.png', 0) > 0
ok = cv2.imread(NT + 'maske_oberkiefer.png', 0) > 0
z2 = ziel(2)
box = (140, 260, 350, 425); s = 4
FARB = {0: (40, 40, 40), 1: (230, 60, 60), 2: (230, 230, 230), 3: (255, 200, 0), 4: (0, 200, 255), 5: (120, 80, 120)}
pan = []
g = gross(V2, box, s)[:, :, ::-1].copy()
kontur(g, uk, box, s, (255, 0, 255), 2); kontur(g, ok, box, s, (0, 255, 0), 1)
kontur(g, (z2 == 255), box, s, (255, 255, 255), 1)
for (x, y), t in (((217, 275), 'Auge r (W)'), ((280, 275), 'Auge l (W)')):
    if box[1] <= y < box[3]:
        cv2.circle(g, ((x - box[0]) * s, (y - box[1]) * s), 10, (0, 255, 255), 1)
gitter(g, box, s)
pan.append(titel(g, 'Original Bild 2 roh; magenta UK, gruen OK, weiss sichere Figur'))
a = np.zeros((480, 640, 3), np.uint8)
for k_, f in FARB.items():
    a[kl == k_] = f[::-1]
g = gross(a, box, s)
kontur(g, uk, box, s, (255, 0, 255), 2); kontur(g, (z2 == 255), box, s, (255, 255, 255), 1); gitter(g, box, s)
pan.append(titel(g, 'Klassen: rot Band, weiss hell-neutral, gelb, blau geclippt, grau dunkel'))
g = gross(np.clip(e2, 0, 255).astype(np.uint8), box, s)[:, :, ::-1].copy()
kontur(g, uk, box, s, (255, 0, 255), 2); gitter(g, box, s)
pan.append(titel(g, 'Rueckrechnung e (roh)'))
row1 = reihe(pan)
pan = []
g = knochenbild(kn2, box, s)[:, :, ::-1].copy()
kontur(g, uk, box, s, (255, 0, 255), 2); gitter(g, box, s)
pan.append(titel(g, 'K5c Knochen-ID Bild-2-Pose (22 Kopf, 23 UK, 18/29 Arm r)'))
ki = cv2.imread(KI[1]); kb = tuple(v * 2 for v in box)
g = gross(ki, kb, 2).copy()
kontur(g, cv2.resize(uk.astype(np.uint8), (1280, 960), interpolation=cv2.INTER_NEAREST) > 0, kb, 2, (255, 0, 255), 2)
pan.append(titel(g, 'KI-Fassung 16h26m29 (nur Zone)'))
fe = np.zeros((480, 640), bool)
lb = np.load(S + 'lb2.npy')
g = gross(V2, box, s)[:, :, ::-1].copy()
fe = c2 & (z2 < 255)
kontur(g, fe, box, s, (0, 200, 255), 2); kontur(g, uk & (kl == 1), box, s, (255, 0, 0), 1); kontur(g, uk & (kl == 2), box, s, (255, 255, 255), 1)
gitter(g, box, s)
pan.append(titel(g, 'orange: geclippt ausserh. sicherer Figur; blau Band, weiss hell im UK'))
row2 = reihe(pan)
w = max(row1.shape[1], row2.shape[1])
row1 = cv2.copyMakeBorder(row1, 0, 0, 0, w - row1.shape[1], cv2.BORDER_CONSTANT, value=(18, 18, 18))
row2 = cv2.copyMakeBorder(row2, 4, 0, 0, w - row2.shape[1], cv2.BORDER_CONSTANT, value=(18, 18, 18))
blatt = np.vstack([kopf('V3B_2 Bild 2 x4, x 140-350 y 260-425. Klassen in e (form_video Umkehr Bild 2). Kein Bild aufgehellt.', w), row1, row2])
cv2.imwrite(OUT + 'V3B_2_bild2_maul_x4.png', blatt)

# --------------------------- V3B_3: schwarz-rot ---------------------------
rows = []
for n, V, e, T, c, box in ((1, V1, e1, T1, c1, (185, 180, 440, 480)), (2, V2, e2, T2, c2, (140, 140, 470, 480))):
    fig = (ziel(n) >= 128) & ~Z.LOGO & ~Z.BODEN
    LT = lab(T).reshape(480, 640, 3); h = hue(LT)
    dunkel = LT[..., 0] < 20
    dr = dunkel & (LT[..., 1] >= 8)
    rot = (LT[..., 1] >= 15) & (LT[..., 0] >= 20) & (h >= -30) & (h <= 70)
    hell = (LT[..., 0] >= 45) & (LT[..., 1] < 10)
    a = np.full((480, 640, 3), 70, np.uint8)
    a[fig] = (110, 110, 110)
    a[fig & dunkel] = (20, 20, 20)
    a[fig & dr] = (90, 0, 20)
    a[fig & rot] = (240, 70, 70)
    a[fig & hell] = (190, 210, 230)
    a[fig & c] = (0, 200, 255)
    s = 3
    g0 = gross(V, box, s)[:, :, ::-1].copy(); kontur(g0, fig, box, s, (255, 255, 255), 1); gitter(g0, box, s)
    g1 = gross(a, box, s)[:, :, ::-1].copy(); gitter(g1, box, s)
    rows.append(reihe([titel(g0, 'Bild %d Original roh, weiss = Figur (ziel2 >= 128)' % n),
                       titel(g1, 'T-Klassen: schwarz L*<20, dunkelrot L*<20 a*>=8, rot, hellblau hell, blau geclippt')]))
w = max(r.shape[1] for r in rows)
rows = [cv2.copyMakeBorder(r, 4, 0, 0, w - r.shape[1], cv2.BORDER_CONSTANT, value=(18, 18, 18)) for r in rows]
cv2.imwrite(OUT + 'V3B_3_schwarzrot_x3.png', np.vstack([kopf('V3B_3 schwarz-rote Flaechen, T = e / Licht (Bild 1 0,487/0,432/0,374; Bild 2 0,385/0,342/0,307). Rot: a*>=15, L*>=20, Ton -30..70 Grad.', w)] + rows))

# --------------------------- V3B_4: Gegenprobe Farbfelder ---------------------------
C = R['C_gegenprobe']
felder = [('e Bild 1 F1 gemessen', C['T_aus_bild1']['e1_gemessen']), ('e Bild 2 Band gemessen', C['T_aus_bild2']['e2_gemessen']),
          ('T aus Bild 1', C['T_aus_bild1']['Tc']), ('T aus Bild 2', C['T_aus_bild2']['Tc']), ('T gemeinsam', C['T_gemeinsam']['Tc']),
          ('T gemeinsam, Licht 1 frei', [174, 95, 96]), ('Zahn T', C['zahn_T'])]
for k_ in ('orange_255_140_40', 'gelb_255_200_60', 'rot_220_60_30'):
    felder.append(('Probe ' + k_, C['probe_' + k_]['Tc']))
w = 150
a = np.full((300, w * len(felder), 3), 18, np.uint8)
for i, (t, c) in enumerate(felder):
    a[0:100, i * w + 4:(i + 1) * w - 4] = np.clip(c, 0, 255)[::-1]
    cv2.putText(a, t[:22], (i * w + 4, 118), FONT, 0.36, (230, 230, 230), 1)
    cv2.putText(a, '(%d,%d,%d)' % tuple(int(round(x)) for x in c), (i * w + 4, 134), FONT, 0.36, (230, 230, 230), 1)
    L = lab1(c)
    cv2.putText(a, 'Lab %.0f/%.0f/%.0f' % tuple(L), (i * w + 4, 150), FONT, 0.36, (230, 230, 230), 1)
VP = C['video_raum']
spalten = [('Original V Bild 1 F1', VP['bezug_hin_und_zurueck']['bild1_original'], None), ('Original V Bild 2 Band', VP['bezug_hin_und_zurueck']['bild2_original'], None)]
for k_ in ('T_aus_bild1', 'T_aus_bild2', 'T_gemeinsam', 'orange_255_140_40', 'gelb_255_200_60', 'rot_220_60_30'):
    spalten.append(('V1 aus ' + k_, VP[k_]['bild1_video_vorher'], VP[k_]['bild1_dE']))
    spalten.append(('V2 aus ' + k_, VP[k_]['bild2_video_vorher'], VP[k_]['bild2_dE']))
w2 = 110
b = np.full((150, w2 * len(spalten), 3), 18, np.uint8)
for i, (t, c, d) in enumerate(spalten):
    b[0:80, i * w2 + 3:(i + 1) * w2 - 3] = np.clip(c, 0, 255)[::-1]
    cv2.putText(b, t[:18], (i * w2 + 3, 96), FONT, 0.32, (230, 230, 230), 1)
    cv2.putText(b, t[18:36], (i * w2 + 3, 110), FONT, 0.32, (230, 230, 230), 1)
    cv2.putText(b, '(%d,%d,%d)' % tuple(c), (i * w2 + 3, 124), FONT, 0.32, (230, 230, 230), 1)
    if d is not None:
        cv2.putText(b, 'dE %.1f' % d, (i * w2 + 3, 138), FONT, 0.32, (230, 230, 230), 1)
W = max(a.shape[1], b.shape[1])
a = cv2.copyMakeBorder(a, 0, 0, 0, W - a.shape[1], cv2.BORDER_CONSTANT, value=(18, 18, 18))
b = cv2.copyMakeBorder(b, 0, 0, 0, W - b.shape[1], cv2.BORDER_CONSTANT, value=(18, 18, 18))
cv2.imwrite(OUT + 'V3B_4_gegenprobe.png', np.vstack([kopf('V3B_4 oben: Schirmfarben e und Texelkandidaten T. unten: Videofarbe (form_video vorwaerts, eingebettet ins e-Bild) gegen Original, dE Lab.', W), a, b]))
print('ok')
