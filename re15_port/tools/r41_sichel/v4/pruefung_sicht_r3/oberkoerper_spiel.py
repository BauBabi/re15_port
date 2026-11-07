"""Pruefung V4 r3 Sicht, Beobachtung (i) im SPIEL: Oberkoerper (Buckel/Schultern/Brust) im Spielbild (RE15_FRAMEDUMP registriert,
Lauf sfv4/r/V4r3 und V4r2) gegen das Video n1468 (h118, roh) und n1485 (h135, entblendet = Vorlage 1); Polygone je Bild von Hand
auf dem Koordinatengitter (grid_118.png, grid_video135.png, grid_spiel135.png) gesetzt; Spielbild im Videoraum (Gauss 1,5, V=a+bP)."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R = sys.argv[1]
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def st(img, poly):
    m = np.zeros((480, 640), np.uint8); cv2.fillPoly(m, [np.array(poly, np.int32)], 1); m = m.astype(bool)
    px = img[m]; lab = G.lab(px); C = np.hypot(lab[:, 1], lab[:, 2])
    return dict(n=int(m.sum()), lab=G.r1(lab.mean(0)), L_p10_50_90=G.r1(np.percentile(lab[:, 0], [10, 50, 90])),
                hell30=round(float((lab[:, 0] > 30).mean()), 3), rosagrau=round(float(((lab[:, 0] > 25) & (C < 12)).mean()), 3),
                rot=round(float(((px[:, 0] - px[:, 1]) >= 15).mean()), 3))
PV = {118: [(318, 190), (335, 170), (370, 160), (410, 162), (440, 175), (465, 195), (470, 230), (455, 255), (330, 255), (315, 225)],
      135: [(297, 175), (320, 150), (360, 140), (410, 138), (450, 150), (490, 165), (505, 195), (500, 235), (470, 250), (330, 250), (300, 230)]}
PS = {118: [(262, 225), (290, 195), (330, 170), (380, 162), (430, 170), (460, 195), (475, 230), (470, 265), (430, 280), (300, 280), (268, 265)],
      135: [(262, 205), (290, 170), (330, 150), (380, 148), (430, 160), (470, 180), (492, 215), (488, 255), (450, 268), (300, 268), (268, 250)]}
WAND = [(10, 150), (60, 150), (60, 300), (10, 300)]
out = {}
for h in (118, 135):
    v = S(R + '/video_h%d.png' % h) if h == 118 else np.asarray(Image.open(R + '/vorlage1_entbl.png').convert('RGB')).astype(float)
    d = {'video': st(v, PV[h]), 'wand_video': st(v, WAND)}
    for k in ('V4r3', 'V4r2'):
        s = S(R + '/spiel_%s_h%d.png' % (k, h)); V = G.videoraum(s)
        d[k + '_videoraum'] = st(V, PS[h]); d[k + '_schirm'] = st(s, PS[h]); d['wand_' + k] = st(s, WAND)
        d['dE00_' + k] = round(G.de00(d['video']['lab'], d[k + '_videoraum']['lab']), 1)
    out['h%d' % h] = d
    vis = [v.copy(), S(R + '/spiel_V4r3_h%d.png' % h)]
    cv2.polylines(vis[0], [np.array(PV[h], np.int32)], True, (255, 255, 0), 1); cv2.polylines(vis[1], [np.array(PS[h], np.int32)], True, (255, 255, 0), 1)
    Image.fromarray(np.concatenate([x[110:300, 240:520] for x in vis], 1).astype(np.uint8)).save(R + '/poly_spiel_h%d.png' % h)
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
# Drittel (B)-links / Mitte / (B)-rechts je Polygon: wo liegt das Rot, wo das helle Rosa-Grau?
def drittel(img, poly):
    m = np.zeros((480, 640), np.uint8); cv2.fillPoly(m, [np.array(poly, np.int32)], 1); m = m.astype(bool)
    xs = np.array(poly)[:, 0]; x0, x1 = xs.min(), xs.max(); xx = np.arange(640)[None, :]; res = {}
    for i, nm in enumerate(('B_links', 'Mitte', 'B_rechts')):
        mm = m & (xx >= x0 + i * (x1 - x0) / 3) & (xx < x0 + (i + 1) * (x1 - x0) / 3); px = img[mm]; lab = G.lab(px); C = np.hypot(lab[:, 1], lab[:, 2])
        res[nm] = dict(lab=G.r1(lab.mean(0)), rot=round(float(((px[:, 0] - px[:, 1]) >= 15).mean()), 3), a12=round(float((lab[:, 1] > 12).mean()), 3),
                       hell30=round(float((lab[:, 0] > 30).mean()), 3), rosagrau=round(float(((lab[:, 0] > 25) & (C < 12)).mean()), 3), L90=round(float(np.percentile(lab[:, 0], 90)), 1))
    res['rot_mitte_durch_aussen'] = round(res['Mitte']['rot'] / max(1e-3, (res['B_links']['rot'] + res['B_rechts']['rot']) / 2), 2)
    res['rosagrau_aussen_durch_mitte'] = round(((res['B_links']['rosagrau'] + res['B_rechts']['rosagrau']) / 2) / max(1e-3, res['Mitte']['rosagrau']), 2)
    return res
out2 = {}
for h in (118, 135):
    v = S(R + '/video_h%d.png' % h) if h == 118 else np.asarray(Image.open(R + '/vorlage1_entbl.png').convert('RGB')).astype(float)
    out2['h%d_video' % h] = drittel(v, PV[h])
    for k in ('V4r3', 'V4r2'):
        out2['h%d_%s_videoraum' % (h, k)] = drittel(G.videoraum(S(R + '/spiel_%s_h%d.png' % (k, h))), PS[h])
json.dump(out2, open(sys.argv[2].replace('.json', '_drittel.json'), 'w'), indent=1)
for k, d in out2.items():
    print(k, 'rot M/A %.2f rosagrau A/M %.2f' % (d['rot_mitte_durch_aussen'], d['rosagrau_aussen_durch_mitte']), ' | '.join('%s L%s rot%.2f a12 %.2f h30 %.2f rg %.2f L90 %.1f' % (n, d[n]['lab'], d[n]['rot'], d[n]['a12'], d[n]['hell30'], d[n]['rosagrau'], d[n]['L90']) for n in ('B_links', 'Mitte', 'B_rechts')))
