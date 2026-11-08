"""Pruefung V4 r5 Sicht: (a) gelbe Texel (b* > 20, a* < 12, L* > 25) je Mesh auf der Buehne (neutral, Pose 135/85) V4 r5 / V4 r4 / REF2;
(b) neue gelbe Flecken im Spielbild V4r5 gegen V4r4 (Komponenten >= 6 px) und Farbe des Videos an derselben Stelle;
(c) grelle Ausreisser (C* > 45) und blaue Pixel (b* < -15) je Mesh auf der Buehne."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
R = sys.argv[1]; out = {}
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
def per(ids, m): u, c = np.unique(ids[m], return_counts=True); return dict(zip([int(x) for x in u], [int(x) for x in c]))
for nm in ('V4', 'V4r4', 'REF2'):
    for p in ('135', '85'):
        a = G.lab(S(R + '/%s_%s_neutral.png' % (nm, p))); ids = np.load(R + '/%s_%s_ids.npy' % (nm, p))[:480]; fig = ids >= 0
        C = np.hypot(a[..., 1], a[..., 2]); g = fig & (a[..., 2] > 20) & (a[..., 1] < 12) & (a[..., 0] > 25)
        out['buehne_%s_%s' % (nm, p)] = dict(gelb=int(g.sum()), gelb_je_mesh=per(ids, g), gelb_lab=G.r1(a[g].mean(0)) if g.any() else None,
                                              grell_C45=int((fig & (C > 45)).sum()), grell_je_mesh=per(ids, fig & (C > 45)), blau=int((fig & (a[..., 2] < -15)).sum()))
for h in (85, 90, 100, 110, 118, 125, 135):
    a = G.lab(S(R + '/spiel_V4r5_h%d.png' % h)); b = G.lab(S(R + '/spiel_V4r4_h%d.png' % h))
    neu = ((a[..., 2] > 20) & (a[..., 0] > 25)) & ~((b[..., 2] > 20) & (b[..., 0] > 25)) & (a[..., 1] < 12)
    n, lab, st, _ = cv2.connectedComponentsWithStats(neu.astype(np.uint8), connectivity=8)
    out['spiel_h%d_neu_gelb' % h] = [dict(bbox=[int(v) for v in st[i, :4]], px=int(st[i, 4]), lab=G.r1(a[lab == i].mean(0))) for i in range(1, n) if st[i, 4] >= 6]
for h, (x, y, w, hh), vid in ((118, (274, 219, 10, 9), 'video_h118.png'), (135, (279, 197, 12, 7), 'vorlage1_entbl.png')):
    v = G.lab(S(R + '/' + vid)); sp = G.lab(G.videoraum(S(R + '/spiel_V4r5_h%d.png' % h)))
    out['fleck_h%d' % h] = dict(video=G.r1(v[y:y+hh, x:x+w].reshape(-1, 3).mean(0)), spiel_V4r5_videoraum=G.r1(sp[y:y+hh, x:x+w].reshape(-1, 3).mean(0)),
                               video_bmax_umkreis20=round(float(v[y-20:y+hh+20, x-20:x+w+20, 2].max()), 1))
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out)[:3000])
