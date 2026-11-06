# Leser B v3: Schwanz der Sichel-Form in der Seitenansicht (KI-Video 4 _slp, 1280x960), Masse S1-S6.
# Maske: dE (CIE-Lab, OpenCV float Lab = D65/sRGB) zum Hintergrund-Modus bg_modus_ki.png > 6, ODER (dE > 3 UND a* > 1,5 UND a* > b*, schliesst den beigen Wandkasten x 550-575 y 465-505 aus;
#   Hintergrund a* -3..0, Schwanz a* 2..13), ODER Rot (a* > 12, L* > 8),
#   im Schwanzfenster x 470-760, y 340-760; Oeffnen 3x3, Schliessen 5x5, Loecher gefuellt.
# Wurzelschnitt: Kerbe = an der Lupe gesetzter Punkt (Unterseite Schwanz trifft Arm/Ruecken), auf die Kontur eingerastet;
#   Gegenpunkt = naechster Konturpunkt oberhalb (y < Kerbe - 15) mit Segment zu >= 90 % in der Maske.
#   Schwanz = Maskenkomponente links vom Schnitt, die den untersten Punkt mit x < Kerbe-40 enthaelt.
# Mittellinie: geodaetischer Abstand vom Schnitt (skimage MCP_Geometric), Baender a 3 px -> Schwerpunkte,
#   gleitendes Mittel 5; Bogenlaenge s 0..1 (0 = Schnittmitte, 1 = Spitze = groesster geodaetischer Abstand).
# Dicke: senkrecht zur Tangente (Tangente ueber +-6 px Bogen) bis Maskenrand beidseits.
# Aufruf: python b_schwanz.py <sfv3-dir> <outdir> n:H:kx:ky:xlim:ylim ...   (Kerbe kx,ky; Ausschluss x>=xlim fuer y>=ylim)
import sys, os, json, cv2, numpy as np
from skimage.graph import MCP_Geometric

S, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)


def lab(i):
    return cv2.cvtColor(i.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)


bg = lab(cv2.imread(f"{S}/tmp/bg_modus_ki.png"))
X0, X1, Y0, Y1 = 470, 760, 340, 760
DE_SCHWELLE = float(os.environ.get("DE_SCHWELLE", "6"))
res = {}


def rotmaske(raw):
    hsv = cv2.cvtColor(raw, cv2.COLOR_BGR2HSV)
    hue = hsv[..., 0].astype(int) * 2
    sat = hsv[..., 1]
    return ((hue <= 20) | (hue >= 340)) & (sat >= 90)


for arg in sys.argv[3:]:
    n, H, kx0, ky0, kx1, ky1 = map(int, arg.split(":"))
    raw = cv2.imread(f"{S}/ki/n{n}.png")
    f = lab(raw)
    d = cv2.GaussianBlur(np.sqrt(((f - bg) ** 2).sum(2)), (5, 5), 0)
    a_s = cv2.GaussianBlur(f[..., 1], (5, 5), 0)
    m = ((d > DE_SCHWELLE) | ((d > 3) & (a_s > 1.5) & (a_s > cv2.GaussianBlur(f[..., 2], (5, 5), 0))) | ((f[..., 1] > 12) & (f[..., 0] > 8))).astype(np.uint8) * 255
    w = np.zeros_like(m); w[Y0:Y1, X0:X1] = 1
    if kx1 > 0: w[ky1:, kx1:] = 0   # Arm/Bein/Sichel rechts unten ausschliessen: y >= ylim und x >= xlim
    m *= w
    m = cv2.morphologyEx(m, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
    m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, np.ones((5, 5), np.uint8))
    k, l, st, _ = cv2.connectedComponentsWithStats(m)
    i = 1 + np.argmax(st[1:, 4]); keep = (l == i)
    # Komponenten > 80 px, die bis auf <= 6 px an die Hauptkomponente heranreichen, anhaengen (Spitze, durch Sockelleiste getrennt)
    for _ in range(3):
        near = cv2.dilate(keep.astype(np.uint8), np.ones((13, 13), np.uint8)) > 0
        for j in range(1, k):
            if st[j, 4] > 80 and not keep[l == j].any() and near[l == j].any():
                keep |= (l == j)
    m = keep.astype(np.uint8) * 255
    m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, np.ones((9, 9), np.uint8))
    # nur kleine Loecher (< 400 px) fuellen - der Spalt zwischen Schwanz und Koerper bleibt offen
    inv = (m == 0).astype(np.uint8); k2, l2, st2, _ = cv2.connectedComponentsWithStats(inv, connectivity=4)
    for j in range(1, k2):
        if st2[j, 4] < 400: m[l2 == j] = 255
    cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); c = max(cs, key=len)
    # Kerbe: an der Lupe (Konturbild b_kontur.py) gesetzter Punkt (kx0,ky0), auf den naechsten Konturpunkt eingerastet
    dd_ = np.hypot(c[:, 0, 0] - kx0, c[:, 0, 1] - ky0); jj = int(np.argmin(dd_))
    nx, ny = int(c[jj, 0, 0]), int(c[jj, 0, 1])
    cand = []
    for (px, py) in c[:, 0]:
        if py < ny - 15 and px <= nx + 10:
            L_ = int(np.hypot(px - nx, py - ny))
            xs = np.linspace(nx, px, L_ + 1).astype(int); ys = np.linspace(ny, py, L_ + 1).astype(int)
            if (m[ys, xs] > 0).mean() >= 0.9:
                cand.append((L_, int(px), int(py)))
    if not cand:
        dbg = raw.copy(); cv2.drawContours(dbg, [c], -1, (0, 255, 0), 1); cv2.circle(dbg, (nx, ny), 4, (0, 0, 255), -1)
        cv2.imwrite(f"{OUT}/fehler{n}.png", dbg[Y0:Y1, X0:X1]); print(n, "kein Gegenpunkt, Kerbe", nx, ny); continue
    cl, ux, uy = min(cand)
    mt = m.copy(); cv2.line(mt, (nx, ny), (ux, uy), 0, 2)
    ys, xs = np.where(mt[:, :nx - 40] > 0); j = np.argmax(ys); seed = (ys[j], xs[j])
    k, l, st, _ = cv2.connectedComponentsWithStats(mt, connectivity=4)
    tail = (l == l[seed]).astype(np.uint8)
    cut = np.zeros_like(m); cv2.line(cut, (nx, ny), (ux, uy), 1, 4)
    start = np.argwhere((cut > 0) & (tail > 0))
    cost = np.where(tail > 0, 1.0, np.inf)
    mcp = MCP_Geometric(cost); g, _ = mcp.find_costs([tuple(p) for p in start])
    g = g.astype(float); g[tail == 0] = np.nan; g[~np.isfinite(g)] = np.nan
    gmax = np.nanmax(g); tip = np.unravel_index(np.nanargmax(g), g.shape)
    pts = []
    for b0 in np.arange(0, gmax, 3):
        sel = (g >= b0) & (g < b0 + 3)
        if sel.sum() > 0:
            yy, xx = np.where(sel); pts.append((xx.mean(), yy.mean()))
    P = np.array(pts); Pk = np.copy(P)
    for q in range(2, len(P) - 2):
        Pk[q] = P[q - 2:q + 3].mean(0)
    Pk = np.vstack([Pk, [tip[1], tip[0]]])
    seg = np.hypot(*np.diff(Pk, axis=0).T); sl = np.concatenate([[0], np.cumsum(seg)]); Ltot = sl[-1]

    def pt(tt):
        tt = min(max(tt, 0), Ltot)
        qq = min(max(np.searchsorted(sl, tt), 1), len(sl) - 1)
        return Pk[qq - 1] + (Pk[qq] - Pk[qq - 1]) * ((tt - sl[qq - 1]) / max(seg[qq - 1], 1e-6))

    def at(sf):
        t = sf * Ltot; p = pt(t)
        a_ = pt(t - 6); b_ = pt(t + 6); tv = (b_ - a_) / np.linalg.norm(b_ - a_); nv = np.array([-tv[1], tv[0]])
        wd = []
        for sgn in (1, -1):
            r = 0
            while True:
                x, y = (p + sgn * nv * (r + 1)).round().astype(int)
                if not (0 <= y < tail.shape[0] and 0 <= x < tail.shape[1]) or tail[y, x] == 0:
                    break
                r += 1
            wd.append(r)
        return p, tv, nv, wd[0] + wd[1] + 1, wd

    prof = {}
    for sf in [0.0, 0.1, 0.2, 0.3, 0.5, 0.7, 0.9, 0.95]:
        p, tv, nv, wdt, wd = at(sf)
        prof[str(sf)] = dict(x=round(float(p[0]), 1), y=round(float(p[1]), 1), breite_px=int(wdt), breite_H=round(wdt / H, 4))
    # Winkel: Mittellinie gleichabstaendig (1 px) neu abgetastet, Gauss-geglaettet (sigma 0,015 H), Tangente zentral
    tt = np.arange(0, Ltot, 1.0); C = np.array([pt(t) for t in tt])
    from scipy.ndimage import gaussian_filter1d
    sig = 0.015 * H; Cs = np.stack([gaussian_filter1d(C[:, 0], sig, mode='nearest'), gaussian_filter1d(C[:, 1], sig, mode='nearest')], 1)
    v = np.gradient(Cs, axis=0); angf = np.degrees(np.unwrap(np.arctan2(v[:, 1], v[:, 0])))
    ts = tt[::2]; ang = angf[::2]
    np.save(f"{OUT}/mittellinie{n}.npy", Cs)
    win = max(1, int(round(0.05 * H / 2.0)))
    dA = np.abs(ang[win:] - ang[:-win]); maxd = float(dA.max()); smax = float(ts[np.argmax(dA)] / Ltot)
    kn = dA > 20
    knicke = int(((kn[1:].astype(int) - kn[:-1].astype(int)) == 1).sum() + int(kn[0]))
    total_turn = float(ang[-1] - ang[0])
    te = cv2.erode(tail, np.ones((5, 5), np.uint8)); sel = te > 0
    rot = rotmaske(raw)
    gg = np.where(sel, g, np.nan); end = sel & (gg >= 0.85 * gmax)
    wurzel = sel & (gg <= 0.3 * gmax)
    res[n] = dict(H=H, kerbe=[nx, ny], gegenpunkt=[ux, uy], schnittlaenge_px=cl, schnittlaenge_H=round(cl / H, 3),
                  schnittmitte=[(nx + ux) / 2, (ny + uy) / 2], spitze=[int(tip[1]), int(tip[0])],
                  bogenlaenge_px=round(float(Ltot), 1), bogenlaenge_H=round(float(Ltot / H), 3),
                  dicke=prof, max_winkel_je_005H=round(maxd, 1), bei_s=round(smax, 2), knicke=knicke,
                  gesamtdrehung_grad=round(total_turn, 1),
                  rgb=[round(float(v), 1) for v in raw[sel][:, ::-1].mean(0)], lab=[round(float(v), 2) for v in f[sel].mean(0)],
                  rotanteil=round(float(rot[sel].mean()), 3), pixel=int(sel.sum()),
                  wurzel_lab=[round(float(v), 2) for v in f[wurzel].mean(0)], wurzel_rotanteil=round(float(rot[wurzel].mean()), 3),
                  ende_rgb=[round(float(v), 1) for v in raw[end][:, ::-1].mean(0)], ende_lab=[round(float(v), 2) for v in f[end].mean(0)],
                  ende_rotanteil=round(float(rot[end].mean()), 3))
    np.save(f"{OUT}/tail{n}.npy", tail); np.save(f"{OUT}/ang{n}.npy", np.vstack([ts / Ltot, ang]))
    np.save(f"{OUT}/geo{n}.npy", g)
    o = raw.copy(); o[tail > 0] = (0.6 * o[tail > 0] + [0, 70, 0]).clip(0, 255)
    cs2, _ = cv2.findContours(tail, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(o, cs2, -1, (0, 255, 0), 1)
    cv2.line(o, (nx, ny), (ux, uy), (0, 255, 255), 2)
    cv2.polylines(o, [Pk.round().astype(np.int32)], False, (255, 0, 255), 1)
    for sf in [0.1, 0.2, 0.3, 0.5, 0.7, 0.9]:
        p, tv, nv, wdt, wd = at(sf)
        a_ = (p + nv * wd[0]).round().astype(int); b_ = (p - nv * wd[1]).round().astype(int)
        cv2.line(o, tuple(map(int, a_)), tuple(map(int, b_)), (0, 200, 255), 1)
        cv2.putText(o, f"{sf}", (int(b_[0]) + 3, int(b_[1])), 0, 0.35, (0, 200, 255), 1)
    cv2.circle(o, (int(tip[1]), int(tip[0])), 3, (0, 0, 255), -1)
    cv2.putText(o, f"KI n{n} h{n - 1350} H={H}px", (X0 - 25, Y0 - 12), 0, 0.5, (0, 255, 255), 1)
    cv2.imwrite(f"{OUT}/ov{n}.png", o[Y0 - 30:Y1, X0 - 30:X1 + 80])
    print(n, json.dumps(res[n]))
json.dump(res, open(f"{OUT}/schwanz_{'_'.join(a.split(':')[0] for a in sys.argv[3:])}.json", "w"), indent=1)
