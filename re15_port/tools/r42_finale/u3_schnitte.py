# Werkzeug zu include/re15_finale_r42.h (RE15_FI_FORM_*, RE15_FI_CUT_*): python re15_port/tools/r42_finale/u3_schnitte.py
# U3 Runde 3 (Pruefbericht s1 r2, R1): Fixpunkte und Schnitte in Wagen C mit VERDECKUNG und "Form ausser Bild".
#
# Neu gegenueber u3/u3_suche3.py (Runde 2):
#   - Die Form ist kein Punkt mehr, sondern das echte Modell BAHNSTEIG_FORM.EMD in der Pose der Sequenz (Clip 2 Bild 0 = BB_C_STEHEN,
#     form_halten in finale_r42.c), Ecken + Flaechenmitten, je Kamera als Tiefenpuffer 80x60 (4x4 Bildpunkte je Zelle, 3x3-Minimum
#     gegen Loecher). Weltlage = RotMatrixY(Gier) wie spieler_vor (x' = c*x + s*z, z' = -s*x + c*z) + Fixpunkt.
#   - Sichtbarkeit eines Punkts = im Bild UND vor der Form (Tiefe kleiner als der Puffer an seiner Zelle).
#   - "Game over" (RE2 Schnitt 11): Form AUSSER Bild (RE2: +86,5 Grad). Kosten + 6 * Bedeckung (Anteil der Bildzellen mit Form),
#     Spieler (Koerperpunkte) zu mindestens 60 % sichtbar.
#   - W1/W2/W3: Gegenstand ist die Wiederholungsrakete. Quelle = Schuetze + gemessener Muendungsversatz (finale.log Runde 2:
#     Quelle (11450,-3024,-22103) bei Schuetze (11701,-22019) usw.), Bahn von der Quelle zur Form bis zum Einfrieren (W1 812, W2 970,
#     W3: 0x270A1000 wandert 30 je Bild, 10 Bilder - finale.log T568-575). Mindestens 60 % der Bahnpunkte sichtbar.
#   - Jeder Rollenwechsel ist ein Schnitt (Game over != W1, W1 != W2, W2 != W3) und W3 != "Game over". W1 == W3 ist erlaubt: mit den
#     fuenf Wagen-C-Schnitten gibt es keine Lage mit drei verschiedenen W-Kameras und sichtbarer Rakete (diag5.py); --streng = Runde 2.
#   - W2 "Schuetze ausser Bild" (RE2 +48 Grad) ist nur noch Kosten (|u| geklemmt 1,5): hart gibt es keine Loesung, W2 und W3 brauchen
#     dann stets dieselbe Kamera (diag5.py). Mit --w2hart wie Runde 2.
# Kosten je Rolle wie Runde 2 (PORT-WAHL): |du| (geklemmt +-1,5) + |ln(s/s*)| fuer Form und Gegenstand, Ziele aus ROOM7020 (re2check.py).
import math, os, struct, sys, itertools, json
import numpy as np
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import form_emd as FE
from form_kamera import blickmatrix

def rid(path, base):
    d = open(path, 'rb').read(); out = {}
    for c in range(16):
        fl, h, ex, ey, ez, tx, ty, tz, m = struct.unpack_from('<HHiiiiiiI', d, base + 32 * c)
        out[c] = (h >> 7, np.array((ex, ey, ez), float), np.array((tx, ty, tz), float))
    return out
RE2 = rid(os.path.join(REPO, 'info/re2leon/PL0/RDT/ROOM7020.RDT'), 0x64)
P50 = rid(os.path.join(REPO, 're15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT'), 0x60)
WC = (11, 12, 13, 14, 15)
KAM = {}
for k in list(WC):
    h, e, t = P50[k]; KAM[k] = (h, e, blickmatrix(e, t))

# ---- Form: Clip 2 Bild 0 ------------------------------------------------------------------------------------------------------
form = FE.lese_emd(open(os.path.join(REPO, 're15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD'), 'rb').read())
R_, T_ = FE.pose_aus_bild(form, form['clips'][2]['bilder'][0])
pts = []
for i in range(len(form['knochen'])):
    m = form['meshes'][i]
    if not m['ecken']: continue
    v = np.array(m['ecken'], float) @ (R_[i].astype(float).T / 4096.0) + T_[i]
    pts.append(v)
    for f in m['dreiecke'] + m['vierecke']:
        pts.append(v[f['e']].mean(0, keepdims=True))
FORM_ALL = np.vstack(pts)
rng = np.random.default_rng(1)
FORM_S = FORM_ALL   # Runde 3: alle Ecken + Flaechenmitten (die Stichprobe 6000 war an Grenzfaellen nicht stabil)
NW, NH, ZEL = 80, 60, 4

def welt(P, F, gier):
    a = gier * 2 * math.pi / 4096; c, s = math.cos(a), math.sin(a)
    x = c * P[:, 0] + s * P[:, 2] + F[0]; z = -s * P[:, 0] + c * P[:, 2] + F[1]
    return np.c_[x, P[:, 1], z]

def proj(k, W):
    h, e, R = KAM[k]
    v = (W - e) @ R.T
    with np.errstate(divide='ignore', invalid='ignore'):
        u = 160 + h * v[:, 0] / v[:, 2]; w = 120 + h * v[:, 1] / v[:, 2]
    return u, w, v[:, 2]

def zpuffer(k, W):
    u, w, z = proj(k, W)
    ok = (z > 50) & (u >= 0) & (u < 320) & (w >= 0) & (w < 240)
    zb = np.full((NH + 2, NW + 2), np.inf)
    np.minimum.at(zb, ((w[ok] // ZEL).astype(int) + 1, (u[ok] // ZEL).astype(int) + 1), z[ok])
    d = zb.copy()                                                    # 3x3-Minimum
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            d[1:-1, 1:-1] = np.minimum(d[1:-1, 1:-1], zb[1 + dy:NH + 1 + dy, 1 + dx:NW + 1 + dx])
    d = d[1:-1, 1:-1]
    return d, float(np.isfinite(d).mean())

def sicht(k, zb, P):
    u, w, z = proj(k, P)
    ok = (z > 50) & (u >= 0) & (u < 320) & (w >= 0) & (w < 240)
    vis = np.zeros(len(P), bool)
    i = np.where(ok)[0]
    vis[i] = z[i] < zb[(w[i] // ZEL).astype(int), (u[i] // ZEL).astype(int)]
    return vis

def mass(cam_h, e, R, p):
    v = R @ (np.array((p[0], -1500.0, p[1])) - e) if len(p) == 2 else R @ (np.asarray(p, float) - e)
    if v[2] <= 50: return None
    return (v[0] / v[2]) * cam_h / 160.0, cam_h / math.hypot(v[0], v[2])

def mass_re2(c, p2):
    h, e, t = RE2[c]; vx, vz = t[0] - e[0], t[2] - e[2]; n = math.hypot(vx, vz); vx /= n; vz /= n
    px, pz = p2[0] - e[0], p2[1] - e[2]; tief = px * vx + pz * vz; quer = px * vz - pz * vx
    return (quer / tief) * h / 160.0, h / math.hypot(px, pz)
def mass_p(k, p2):
    h, e, R = KAM[k]
    t = e + R[2] * 1000.0
    vx, vz = t[0] - e[0], t[2] - e[2]; n = math.hypot(vx, vz); vx /= n; vz /= n
    px, pz = p2[0] - e[0], p2[1] - e[2]; tief = px * vx + pz * vz; quer = px * vz - pz * vx
    if tief <= 50: return None
    return (quer / tief) * h / 160.0, h / math.hypot(px, pz)

T = (-14547, -14636); L = (-19025, -20861)
S = {10000: (-20405, -22741), 5000: (-17476, -18688), 3000: (-16304, -17067)}
ZIEL = {'GameOver': (None, mass_re2(11, L)), 'W1': (mass_re2(8, T), mass_re2(8, S[10000])),
        'W2': (mass_re2(9, T), mass_re2(9, S[5000])), 'W3': (mass_re2(10, T), mass_re2(10, S[3000]))}

def k1(m, z):
    if m is None: return 9.0
    u = max(-1.5, min(1.5, m[0])); uz = max(-1.5, min(1.5, z[0])); k = abs(u - uz)
    if abs(z[0]) <= 1 and abs(m[0]) <= 1: k += abs(math.log(m[1] / z[1]))
    return k

def drin(x, z): return -1000 <= x <= 25000 and -24900 <= z <= -22000
FREEZE = {10000: 812, 5000: 970, 3000: 3000 - 300}       # W3: Quelle wandert 10 x 30 (finale.log Runde 2 T568-575)
MUEND_VOR, MUEND_SEITE, MUEND_Y = None, None, -3024.0

def muendung(F, dx, dz, d):
    # Versatz Quelle - Schuetze aus finale.log Runde 2 (Achse 4/256): (-251,-84) / (-251,-76) / (-251,-73) -> in Achsenkoordinaten
    a = 4 * 2 * math.pi / 256; ax, az = math.cos(a), math.sin(a)
    ox, oz = -251.0, -76.0
    vor = ox * ax + oz * az; seite = -ox * az + oz * ax
    sx, sz = F[0] + dx * d, F[1] + dz * d
    return np.array((sx + vor * dx - seite * dz, MUEND_Y, sz + vor * dz + seite * dx))

def bahn(F, dx, dz, d):
    q = muendung(F, dx, dz, d); ziel = np.array((F[0], MUEND_Y, F[1]))
    v = ziel - q; L_ = np.linalg.norm(v[[0, 2]])
    ende = (L_ - FREEZE[d]) / L_ if d != 3000 else 300.0 / L_
    return np.array([q + v * t for t in np.linspace(0, ende, 8)])

def koerper(px, pz, dx, dz):
    sx, sz = -dz, dx
    P = [(px, y, pz) for y in (-300, -900, -1500)] + [(px + sx * 250, -900, pz + sz * 250), (px - sx * 250, -900, pz - sz * 250)]
    return np.array(P, float)

# Wagen C innen (SCA ROOM5090 @RDT+0x20, gen_map_zones.read_rdt): Waende x -1443 / 25492, z -26884 / -19870; Vorraum am Ende
# x 21642..23962 nur z -24749..-22215 offen. Die Form (alle Ecken, waagrecht) muss drinstehen, Toleranz 300 (Runde 3: die Loesung
# ohne diese Regel stellte die Form mit dem Schwanz durch die Stirnwand, Schnitt 14 sieht das von der Seite).
def im_wagen(FW, tol=300.0):
    x, z = FW[:, 0], FW[:, 2]
    if x.min() < -1443 - tol or x.max() > 25492 + tol or z.min() < -26884 - tol or z.max() > -19870 + tol: return False
    v = (x > 21642 + tol) & (x < 23962 - tol)
    return not np.any(v & ((z < -24749 - tol) | (z > -22215 + tol)))

def bewerte(F, w, voll=False):
    a = w * 2 * math.pi / 256; dx, dz = math.cos(a), math.sin(a)
    gier = (-w * 16) & 4095
    FW = welt(FORM_ALL if voll else FORM_S, F, gier)
    tab = {k: {} for k in WC}
    if not im_wagen(FW): return tab
    for k in WC:
        zb, bed = zpuffer(k, FW)
        h, e, R = KAM[k]
        mf = mass_p(k, F)
        # Game over: Spieler 7668 vor der Form
        pl = (F[0] + dx * 7668, F[1] + dz * 7668)
        vk = sicht(k, zb, koerper(pl[0], pl[1], dx, dz)).mean()
        ms = mass_p(k, pl)
        r = {}
        if vk >= 0.6:
            sp = min((1, -1), key=lambda sp: k1(ms, (ZIEL['GameOver'][1][0] * sp, ZIEL['GameOver'][1][1])))
            r['GameOver'] = (k1(ms, (ZIEL['GameOver'][1][0] * sp, ZIEL['GameOver'][1][1])) + 6.0 * bed, ms, mf, bed, vk)
        for nm, d in (('W1', 10000), ('W2', 5000), ('W3', 3000)):
            if mf is None or abs(mf[0]) > 0.7: continue
            B = bahn(F, dx, dz, d)
            vb = sicht(k, zb, B).mean()
            if vb < 0.6: continue
            sh = (F[0] + dx * d, F[1] + dz * d)
            vs = sicht(k, zb, koerper(sh[0], sh[1], dx, dz)).mean()   # Schuetze (unsichtbar, nur fuer die Rolle)
            ms2 = mass_p(k, sh)
            im = ms2 is not None and abs(ms2[0]) <= 1
            if nm in ('W1', 'W3') and not im: continue
            if nm == 'W2' and im and '--w2hart' in sys.argv: continue   # Runde 3: weich (Kosten |u - 1,5|), siehe Kopf
            if nm == 'W3':                                            # RE2: Schuetze 1181 vor der Form 4112 -> Quelle vor der Form
                q = muendung(F, dx, dz, d)
                if not sicht(k, zb, q[None, :])[0]: continue
                h_, e_, R_k = KAM[k]                                  # und naeher an der Kamera als die Form (Mitte, Hoehe -1500)
                if (R_k @ (q - e_))[2] >= (R_k @ (np.array((F[0], -1500.0, F[1])) - e_))[2]: continue
            zf, zs = ZIEL[nm]
            best = None
            for sp in (1, -1):
                kk = k1(ms2, (zs[0] * sp, zs[1])) + k1(mf, (zf[0] * sp, zf[1]))
                if best is None or kk < best[0]: best = (kk, ms2, mf, bed, vb)
            r[nm] = best
        tab[k] = r
    return tab

def main():
    res = []
    grob = '--fein' not in sys.argv
    for fx in range(-1000, 25001, 500):
        for fz in range(-24750, -22000, 250):
            for w in range(256):
                a = w * 2 * math.pi / 256; dx, dz = math.cos(a), math.sin(a)
                if not all(drin(fx + dx * d, fz + dz * d) for d in (7668, 10000, 5000, 3000)): continue
                tab = bewerte((fx, fz), w)
                best = None
                for g, a1, a2, a3 in itertools.product(WC, WC, WC, WC):
                    if g in (a1, a3) or a1 == a2 or a2 == a3: continue      # jeder Rollenwechsel ist ein Schnitt (RE2 11 -> 8 -> 9 -> 10)
                    if a1 == a3 and '--streng' in sys.argv: continue        # Runde 2: drei verschiedene W-Kameras (gibt es nicht, diag5.py)
                    z = [tab[g].get('GameOver'), tab[a1].get('W1'), tab[a2].get('W2'), tab[a3].get('W3')]
                    if any(v is None for v in z): continue
                    s = sum(v[0] for v in z)
                    if best is None or s < best[0]: best = (s, (g, a1, a2, a3), [v[0] for v in z], [v[3] for v in z], [v[4] for v in z])
                if best: res.append((best[0], fx, fz, w) + best[1:])
    res.sort(key=lambda r: r[0])
    print(len(res), 'Loesungen')
    for r in res[:25]:
        s, fx, fz, w, ks, kost, bed, vis = r
        print('%.2f F=(%d,%d) Achse %d/256 (Gier %d) GO %d W1 %d W2 %d W3 %d  Kosten [%s] Bedeckung [%s] sichtbar [%s]' % (
            s, fx, fz, w, (-w * 16) & 4095, *ks, ' '.join('%.2f' % v for v in kost), ' '.join('%.2f' % v for v in bed),
            ' '.join('%.2f' % v for v in vis)))
    json.dump([list(map(lambda x: x if not isinstance(x, tuple) else list(x), r)) for r in res[:200]], open(os.path.join(os.getcwd(), 'u3_schnitte_top.json'), 'w'))

if __name__ == '__main__':
    main()
