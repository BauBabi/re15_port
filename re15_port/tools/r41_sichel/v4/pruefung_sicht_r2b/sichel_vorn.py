"""Pruefung V4 r2 Sicht (Wiederholung): dunkle Sichel (B)-links im Vorlagebild 1 gegen die Sichel Id 15 in V4 Pose 135 (Videoraum
und Schirm); 5x5-Fenster auf Punkten der Sichel im Original, Schwarzwert-Abzug 17 mit angegeben; Grau-Oberseite Schwanz in Vorlage 2."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R2 = sys.argv[1]; OUT = sys.argv[2]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
o1 = L(R2 + '/vorlage1_entbl.png'); o1r = L(r'C:/workspace/git/reAi_v2/Version_80%/Beschreibung1_Vorlage.png')
o2 = L(r'C:/workspace/git/reAi_v2/Version_80%/Beschreibung2_Vorlage.png')
v4 = L(R2 + '/V4_135.png'); ids = np.load(R2 + '/V4_135_ids.npy')[:480]; V = G.videoraum(v4)
out = {}
def f5(img, p, sub=0): x, y = p; w = img[y-2:y+3, x-2:x+3].reshape(-1, 3) - sub; w = np.clip(w, 0, 255); return dict(rgb=G.r1(w.mean(0)), lab=G.r1(G.lab(w).mean(0)))
PTS = [(318, 280), (310, 300), (304, 320), (300, 340), (300, 360), (302, 380), (306, 400)]
out['orig1_sichelpfad_entbl'] = {str(p): f5(o1, p) for p in PTS}
out['orig1_sichelpfad_roh'] = {str(p): f5(o1r, p) for p in PTS}
out['orig1_sichelpfad_roh_minus17'] = {str(p): f5(o1r, p, 17) for p in PTS}
m = cv2.erode((ids == 15).astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
out['V4_Id15_videoraum'] = dict(n=int(m.sum()), lab=G.r1(G.lab(V[m]).mean(0))); out['V4_Id15_schirm'] = dict(lab=G.r1(G.lab(v4[m]).mean(0)))
ys, xs = np.nonzero(ids == 15); out['V4_Id15_box'] = [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())]
ol = np.mean([out['orig1_sichelpfad_entbl'][str(p)]['lab'] for p in PTS], 0); out['orig1_sichel_mittel_entbl'] = G.r1(ol)
out['dE00_sichel'] = round(G.de00(list(ol), out['V4_Id15_videoraum']['lab']), 1)
# Schwanz-Oberseite Vorlage 2 (helles Band) gegen V4 graue Oberflaeche: 5x5 an 4 Punkten
v485 = L(R2 + '/V4_85.png'); V85 = G.videoraum(v485)
P2 = [(300, 225), (310, 222), (320, 216), (330, 212)]
out['orig2_schwanz_oben'] = {str(p): f5(o2, p) for p in P2}
out['V4_schwanz_oben_videoraum'] = {str(p): f5(V85, p) for p in P2}
out['dE00_schwanz_oben'] = {str(p): round(G.de00(out['orig2_schwanz_oben'][str(p)]['lab'], out['V4_schwanz_oben_videoraum'][str(p)]['lab']), 1) for p in P2}
json.dump(out, open(OUT + '/PS2_sichel_schwanzoben.json', 'w'), indent=1)
for k, v in out.items(): print(k, v)
