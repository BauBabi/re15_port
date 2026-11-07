"""Pruefung V4 r4 Sicht: Kopf-Struktur (Sobel-Gradient der Helligkeit) im SPIELBILD h135/h118 (RE15_FRAMEDUMP registriert, Videoraum)
gegen das Original (Vorlage 1 entblendet, Kopfbox 368-416 x 139-191 / Rumpfbox 352-448 x 196-292, 2 px erodiert, wie K11 und
PS2_kopf_textur_vorn). Kopf im Spielbild = Kreis um die Kuppelmitte (von Hand auf grid_spiel135/grid_118), Rumpf = Box unter dem Kopf.
Dazu der Anteil glatter Pixel (Gradient < 8) im Kopf."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R = sys.argv[1]
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def grad(img): Lm = G.lum(img).astype(np.float32); return np.hypot(cv2.Sobel(Lm, cv2.CV_32F, 1, 0), cv2.Sobel(Lm, cv2.CV_32F, 0, 1))
def kreis(c, r): m = np.zeros((480, 640), np.uint8); cv2.circle(m, c, r, 1, -1); return m.astype(bool)
def box(b): m = np.zeros((480, 640), bool); m[b[1]:b[3]+1, b[0]:b[2]+1] = True; return m
def er(m, n=2): return cv2.erode(m.astype(np.uint8), np.ones((2*n+1, 2*n+1), np.uint8)).astype(bool)
o = np.asarray(Image.open(R + '/vorlage1_entbl.png').convert('RGB')).astype(float); go = grad(o)
out = {'orig': dict(kopf=round(float(go[er(box((368, 139, 416, 191)))].mean()), 1), rumpf=round(float(go[er(box((352, 196, 448, 292)))].mean()), 1),
                    kopf_glatt=round(float((go[er(box((368, 139, 416, 191)))] < 8).mean()), 3))}
# Fenster-Spanne des Originals: Kopfbox um +-3 px verschoben (wie Riegel K11)
sp = [float(go[er(box((368+dx, 139+dy, 416+dx, 191+dy)))].mean()) for dx in (-3, 0, 3) for dy in (-3, 0, 3)]
out['orig']['kopf_spanne_pm3'] = [round(min(sp), 1), round(max(sp), 1)]
KOPF = {135: ((360, 191), 28), 118: ((358, 222), 26)}
RUMPF = {135: (320, 228, 420, 300), 118: (320, 255, 420, 320)}
for h in (135, 118):
    for k in ('V4r4', 'V4r3'):
        V = G.videoraum(S(R + '/spiel_%s_h%d.png' % (k, h))); g = grad(V); mk = er(kreis(*KOPF[h])); mr = er(box(RUMPF[h]))
        d = dict(kopf=round(float(g[mk].mean()), 1), rumpf=round(float(g[mr].mean()), 1), kopf_glatt=round(float((g[mk] < 8).mean()), 3),
                 kopf_lab=G.r1(G.lab(V[mk]).mean(0)), kopf_L_p10_90=G.r1(np.percentile(G.lab(V[mk])[:, 0], [10, 90])))
        d['kopf_durch_rumpf'] = round(d['kopf'] / d['rumpf'], 2); d['kopf_anteil_original'] = round(d['kopf'] / out['orig']['kopf'], 2)
        out['%s_h%d' % (k, h)] = d
out['orig']['kopf_durch_rumpf'] = round(out['orig']['kopf'] / out['orig']['rumpf'], 2)
vis = S(R + '/spiel_V4r4_h135.png'); cv2.circle(vis, *KOPF[135], (255, 255, 0), 1); cv2.rectangle(vis, RUMPF[135][:2], RUMPF[135][2:], (0, 255, 255), 1)
v2 = S(R + '/spiel_V4r4_h118.png'); cv2.circle(v2, *KOPF[118], (255, 255, 0), 1); cv2.rectangle(v2, RUMPF[118][:2], RUMPF[118][2:], (0, 255, 255), 1)
Image.fromarray(np.concatenate([vis[120:330, 280:460], v2[150:350, 280:460][:210] if v2[150:350].shape[0] >= 210 else v2[150:360, 280:460]], 1).astype(np.uint8)).save(R + '/kopf_struktur_spiel_fenster.png')
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
# Innenflaeche (ohne Kopfumriss): Original Kopfbox auf 2/3 verkleinert, Spiel Kreis r x 0,6; dazu Lum-Streuung (Fleckigkeit)
ob = (376, 148, 408, 182); out2 = {'orig_innen': dict(grad=round(float(go[box(ob)].mean()), 1), glatt=round(float((go[box(ob)] < 8).mean()), 3),
                                                      lum_std=round(float(G.lum(o)[box(ob)].std()), 1))}
sp = [float(go[box((ob[0]+dx, ob[1]+dy, ob[2]+dx, ob[3]+dy))].mean()) for dx in (-3, 0, 3) for dy in (-3, 0, 3)]
out2['orig_innen']['grad_spanne_pm3'] = [round(min(sp), 1), round(max(sp), 1)]
for h in (135, 118):
    for k in ('V4r4', 'V4r3'):
        V = G.videoraum(S(R + '/spiel_%s_h%d.png' % (k, h))); g = grad(V); c, r = KOPF[h]; mk = kreis(c, int(r * 0.6))
        out2['%s_h%d_innen' % (k, h)] = dict(grad=round(float(g[mk].mean()), 1), glatt=round(float((g[mk] < 8).mean()), 3), lum_std=round(float(G.lum(V)[mk].std()), 1))
json.dump(out2, open(sys.argv[2].replace('.json', '_innen.json'), 'w'), indent=1); print(json.dumps(out2))
