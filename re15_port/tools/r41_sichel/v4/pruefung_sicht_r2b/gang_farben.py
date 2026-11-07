"""Pruefung V4 r2 Sicht (Wiederholung): Farbe des Oberkoerpers ueber den Gang - Video roh n1450/n1468/n1485 gegen Spiel V4 r2
und V3 r2 h100/h118/h135 (RE15_FRAMEDUMP registriert). Figurmaske = Pixel mit dE00 > 8 zum Median-Hintergrund? -> nicht
verfuegbar fuer das Spiel; daher Box ueber dem Oberkoerper (Buckel/Rumpf) je Bild und Wand-Kontrolle (Box 10-60 x 150-300)."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
R2 = sys.argv[1]
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def st(img, b): x0, y0, x1, y1 = b; w = img[y0:y1, x0:x1].reshape(-1, 3); lab = G.lab(w); return dict(lab=G.r1(lab.mean(0)), L_p10_50_90=G.r1(np.percentile(lab[:, 0], [10, 50, 90])), rot=round(float(((w[:, 0]-w[:, 1]) >= 15).mean()), 2))
BOX = (345, 175, 445, 270)
out = {}
for h in (100, 118, 135):
    v = S(R2 + '/video_h%d.png' % h); s4 = S(R2 + '/spiel_V4r2_h%d.png' % h); s3 = S(R2 + '/spiel_V3r2_h%d.png' % h)
    d = {'video': st(v, BOX), 'V4': st(s4, BOX), 'V3r2': st(s3, BOX), 'wand_video': st(v, (10, 150, 60, 300)), 'wand_V4': st(s4, (10, 150, 60, 300))}
    d['dE00_V4'] = round(G.de00(d['video']['lab'], d['V4']['lab']), 1); d['dE00_V3r2'] = round(G.de00(d['video']['lab'], d['V3r2']['lab']), 1)
    out['h%d' % h] = d
print(json.dumps(out, indent=1)); json.dump(out, open(sys.argv[2] + '/PS2_gang_farben.json', 'w'), indent=1)
