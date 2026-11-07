"""Pruefung V4 r2 Sicht (Wiederholung): Schirmfarben OHNE Videoraum-Abbildung - Spielbild h135 (RE15_FRAMEDUMP, registriert)
mit den Ids der Buehne V4 Pose 135 gegen Vorlage 1 entblendet (Boxen); Kopf-Kontrast gegen die Umgebung."""
import sys, os, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R2 = sys.argv[1]; OUT = sys.argv[2]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)
def er(m, n=2): return cv2.erode(m.astype(np.uint8), np.ones((2*n+1, 2*n+1), np.uint8)).astype(bool)
def box(b): m = np.zeros((480, 640), bool); x0, y0, x1, y1 = b; m[y0:y1+1, x0:x1+1] = True; return m
def st(img, m): lab = G.lab(img[m]); return dict(n=int(m.sum()), rgb=G.r1(img[m].mean(0)), lab=G.r1(lab.mean(0)), L50=round(float(np.median(lab[:, 0])), 1))
o1 = L(R2 + '/vorlage1_entbl.png'); sp = S(R2 + '/spiel_V4r2_h135.png')[:480]; bu = L(R2 + '/V4_135.png')[:480]
ids = np.load(R2 + '/V4_135_ids.npy')[:480]
# Bild-Registrierung pruefen: Wand links (Box 10-60 x 150-300) in Spiel und Buehne
out = {'wand_spiel': st(sp, box((10, 150, 60, 300))), 'wand_buehne': st(bu, box((10, 150, 60, 300))), 'wand_vorlage1': st(o1, box((10, 150, 60, 300)))}
ys, xs = np.nonzero(ids == 9); kb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
# Ring um den Kopf: Rumpf/Hals-Pixel im Band 0-12 px um die Kopfbox
ring_p = (cv2.dilate((ids == 9).astype(np.uint8), np.ones((25, 25), np.uint8)).astype(bool)) & np.isin(ids, (7, 8, 10, 13))
okb = (368, 139, 416, 191); ring_o = box((356, 127, 428, 203)) & ~box(okb)
out['kopf_spiel'] = st(sp, er(ids == 9)); out['kopf_buehne'] = st(bu, er(ids == 9)); out['kopf_orig'] = st(o1, er(box(okb)))
out['kopfring_spiel'] = st(sp, ring_p); out['kopfring_buehne'] = st(bu, ring_p); out['kopfring_orig'] = st(o1, ring_o)
for k in ('spiel', 'buehne', 'orig'):
    out['kopf_minus_ring_L_' + k] = round(out['kopf_' + k]['lab'][0] - out['kopfring_' + k]['lab'][0], 1)
# Kopf-Oberflaeche: Anteil der Pixel mit kleiner Nachbar-Differenz (glatte Flaeche) im Kopf, Spiel vs Original (Original unscharf -> nur Spiel/Buehne berichten)
for nm, img in (('spiel', sp), ('buehne', bu)):
    Lm = G.lab(img)[..., 0]; g = np.hypot(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 1, 0), cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 0, 1))
    m = er(ids == 9); out['kopf_glatt_anteil_grad<8_' + nm] = round(float((g[m] < 8).mean()), 3)
    mr = er(ids == 8); out['rumpf_glatt_anteil_grad<8_' + nm] = round(float((g[mr] < 8).mean()), 3)
IDS = {'Rumpf': [8], 'Oberarm_B_links': [13], 'Oberarm_B_rechts': [10], 'Unterarm_B_rechts': [11], 'Unterarm_B_links': [14], 'Jeans': [0, 1, 4], 'Unterschenkel': [2, 3, 5, 6], 'Sichel_B_links': [15], 'Hals_7': [7]}
BOX = {'Rumpf': (352, 196, 448, 292), 'Oberarm_B_links': (300, 186, 345, 250), 'Oberarm_B_rechts': (455, 196, 500, 262), 'Unterarm_B_rechts': (468, 268, 508, 330), 'Jeans': (362, 298, 428, 344), 'Unterschenkel': (356, 352, 392, 430)}
for k, v in IDS.items():
    m = er(np.isin(ids, v)); out['spiel_' + k] = st(sp, m)
    if k in BOX: o = st(o1, er(box(BOX[k]))); out['orig_' + k] = o; out['dE00_spiel_roh_' + k] = round(G.de00(o['lab'], out['spiel_' + k]['lab']), 1)
out['kopfbox_port'] = kb
json.dump(out, open(OUT + '/PS2_schirm_roh.json', 'w'), indent=1, default=str)
for k, v in out.items(): print(k, v)
