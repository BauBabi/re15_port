"""Zahlen-Pruefer V4 r4 (Kopie pz2_schwanz): Schwanz-Riegel T1/T2/TA am b2-Render Pose "85" mit eigenem Code (Mittellinie aus pz2_mess.json).
T1 nach Messblatt-Wortlaut (unterster Pixel von Id 16 an Id 7/8, 4er-Nachbarschaft) UND alternativ (Ids 16-22 an Id 7/8).
T2: Drehung = groesste Richtungsaenderung in 0,10 H Bogenlaenge, (a) zentrierte Tangenten +-0,025 H, (b) zwei 0,05-H-Sehnen;
Lage der Biegung = Mitte des Fensters mit der groessten Drehung. TA: Port-Pixel gegen Nutzer-Linienenden (gleiche Kamera)."""
import json, sys, numpy as np, cv2
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz4/'
nm = sys.argv[1] if len(sys.argv) > 1 else 'V4'
d = np.load(SP + 'r_%s_85.npz' % nm); ids, idv = d['ids'], d['idv']
M = json.load(open(SP + 'mess_V4_V4R3_V3.json'))[nm]['seite']
H, so, sch = M['H'], M['sohle'], M['scheitel']
T = M['T']; P = np.array(T['_P']); s = np.array(T['_s']); W = np.array(T['_W'])
kr = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]], np.uint8)
rumpf = cv2.dilate(np.isin(ids, (7, 8)).astype(np.uint8), kr) > 0
out = {}
for regel, m in (('Id16', (ids == 16) & rumpf), ('Ids16-22', np.isin(ids, range(16, 23)) & rumpf)):
    ys, xs = np.nonzero(m)
    if not len(ys):
        out[regel] = None; continue
    i = int(np.argmax(ys + 1e-3 * xs)); b = (int(xs[i]), int(ys[i]))
    out[regel] = dict(p=b, hinter=round((sch - b[0]) / H, 3), ueber=round((so - b[1]) / H, 3),
                      ok=bool(abs((sch - b[0]) / H - 0.15) <= 0.03 and abs((so - b[1]) / H - 0.767) <= 0.03))
# sichtbare Pixel je Schwanz-Id und die Id-16-Pixel ueber/unter
out['px_je_id'] = {k: int((ids == k).sum()) for k in range(16, 23)}
def at(x):
    return np.array([np.interp(x, s, P[:, 0]), np.interp(x, s, P[:, 1])])
def ang(v):
    return np.degrees(np.arctan2(v[1], -v[0]))
for regel in ('Id16', 'Ids16-22'):
    if not out[regel]: continue
    beg = np.array(out[regel]['p'], float)
    best_t = (0, 0); best_s = (0, 0)
    for x in np.arange(0, s[-1] - 0.10 * H, 0.5):
        if at(x)[0] > beg[0] - 0.10 * H:
            continue
        t0 = at(x + 0.025 * H) - at(max(0, x - 0.025 * H)); t1 = at(min(s[-1], x + 0.125 * H)) - at(x + 0.075 * H)
        w = ang(t1) - ang(t0)
        if w > best_t[0]: best_t = (w, x)
        a = at(x + 0.05 * H) - at(x); b_ = at(x + 0.10 * H) - at(x + 0.05 * H)
        w2 = ang(b_) - ang(a)
        if w2 > best_s[0]: best_s = (w2, x)
    bm = at(best_t[1] + 0.05 * H)
    out[regel]['T2'] = dict(dreh_tangente=round(float(best_t[0]), 1), dreh_sehnen=round(float(best_s[0]), 1),
                            biegung=[round(float(v), 1) for v in bm], rel=[round(float((bm[0] - beg[0]) / H), 3), round(float((bm[1] - beg[1]) / H), 3)])
    jk = T['_jk']; kn = P[jk]; en = P[-1]
    out[regel]['T3rel'] = [round(float((kn[0] - beg[0]) / H), 3), round(float((kn[1] - beg[1]) / H), 3)]
    out[regel]['T4rel'] = [round(float((en[0] - beg[0]) / H), 3), round(float((en[1] - beg[1]) / H), 3)]
# TA absolut: Nutzer-Linienenden (Messblatt 1a), Port-Punkte; Bezug H Port (264) und H2 Video (253)
nut = dict(Beginn=(357, 243), Knick=(290, 308), Ende=(295, 343))
port = dict(Beginn=out['Id16']['p'] if out['Id16'] else out['Ids16-22']['p'], Knick=P[T['_jk']].tolist(), Ende=P[-1].tolist())
port['Beginn_alt'] = out['Ids16-22']['p']
out['TA'] = {}
for k, q in port.items():
    p = nut[k.split('_')[0]]
    out['TA'][k] = dict(port=[round(float(v), 1) for v in q], nutzer=p, dx_H=round((q[0] - p[0]) / H, 3), dy_H=round((q[1] - p[1]) / H, 3),
                        ok=bool(abs(q[0] - p[0]) / H <= 0.03 and abs(q[1] - p[1]) / H <= 0.03))
# Biegung absolut: Video-Biegung = Nutzer-Beginn + Messblatt-Relativmass in H2 (253): (357 - 0,245*253, 243 + 0,036*253)
vb = (357 - 0.245 * 253, 243 + 0.036 * 253)
bm = out['Id16']['T2']['biegung'] if out['Id16'] else out['Ids16-22']['T2']['biegung']
out['Biegung_abs'] = dict(video=[round(v, 1) for v in vb], port=bm, dx_H=round((bm[0] - vb[0]) / H, 3), dy_H=round((bm[1] - vb[1]) / H, 3))
# Richtungsprofil Tangente +-0,025 H
out['profil'] = [(round(float(x / H), 3), round(float(ang(at(min(s[-1], x + 0.025 * H)) - at(max(0, x - 0.025 * H)))), 0), [round(float(v)) for v in at(x)])
                 for x in np.arange(0, s[-1], 0.025 * H)]
json.dump(out, open(SP + 'schwanz_%s.json' % nm, 'w'), indent=1)
print(json.dumps({k: v for k, v in out.items() if k != 'profil'}, indent=1))
print('Profil (s/H, Grad, Punkt):')
for p in out['profil']: print('  ', p)
