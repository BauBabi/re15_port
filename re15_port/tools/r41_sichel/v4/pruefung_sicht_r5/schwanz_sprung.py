"""Pruefung V4 r5 Sicht: Schwanz in den Sprungbildern h68/h76 (Spiel V4r5/V4r4 gegen Video). Spiel: Schwanz = Pixel in der Hand-Box,
die sich vom leeren Raum (Median der Spielbilder h-8..h135, Schritt 3, Lauf V4r5) um > 25 (Summe |dRGB|) unterscheiden; Video: Pixel in der Box mit a* > 8
(rosa-roter Schwanzstreifen, h68) bzw. Differenz zum Video-Hintergrund ist nicht verfuegbar -> nur h68. Kennzahlen Schirm und Videoraum."""
import sys, json, numpy as np, cv2
H = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, H + '/v4'); sys.path.insert(0, H + '/nb2neu')
import v4_gemein as G, c6
S = G.SFV4 + '/sicht_r5'; out = {}
def rd(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
bg = rd(S + '/spiel_median_bg.png')  # Median der Spielbilder h-8..h135 (Schritt 3) des Laufs V4r5
BOX = {68: (430, 335, 525, 400), 76: (420, 200, 540, 260)}
def st(img, m, raw):
    a = G.lab(img)[m]; r = G.lab(raw)[m]
    return dict(n=int(m.sum()), videoraum=G.r1(a.mean(0)), a_p50_90=G.r1(np.percentile(a[:, 1], [50, 90])), schirm=G.r1(r.mean(0)),
                schirm_L45=round(float((r[:, 0] > 45).mean()), 3), schirm_L_p10_90=G.r1(np.percentile(r[:, 0], [10, 90])), schirm_a_gt20=round(float((r[:, 1] > 20).mean()), 3))
for h, (x0, y0, x1, y1) in BOX.items():
    box = np.zeros((480, 640), bool); box[y0:y1, x0:x1] = True; d = {}
    if h == 68:
        v = rd(S + '/video_h68.png'); mv = box & (G.lab(v)[..., 1] > 8); d['video'] = st(v, mv, v)
    for run in ('V4r5', 'V4r4'):
        s = rd(S + '/spiel_%s_h%d.png' % (run, h)); m = box & (np.abs(s - bg).sum(2) > 25)
        m = cv2.morphologyEx(m.astype(np.uint8), cv2.MORPH_OPEN, np.ones((2, 2), np.uint8)).astype(bool)
        d[run] = st(G.videoraum(s), m, s)
        if 'video' in d: d[run]['dE00_video'] = round(G.de00(d['video']['videoraum'], d[run]['videoraum']), 1)
    out['h%d' % h] = d
json.dump(out, open(sys.argv[1], 'w'), indent=1); print(json.dumps(out, indent=1))
