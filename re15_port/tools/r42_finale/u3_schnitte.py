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
#
# Runde 4 (Pruefbericht s1 r3, R1): "Game over" ist KEINE Lauf-Kamera. Harte Regel aus den RVD-Bytes (Kamera-Wechselzonen, RDT-Kopf
# +0x28, 20 Byte je Eintrag: Flag, Stockwerk, von, nach, 4 Ecken x/z; je Kamera ist der ERSTE Eintrag das Anker-Viereck FUN_80014324 =
# rdt_common.c re15_rdt_get_region_quad, die uebrigen sind echte Wechsel). Lauf-Kamera = Ziel ("nach") eines echten Wechsels.
#   RE2 ROOM7020 (@0x26C-0x44B): echte Wechsel nur nach 0..3 (z. B. `01ff0001` @0x00280, `01ff0203` @0x002F8) - Schnitt 11 ("Game
#     over", `jal 0x8002c7ac / addiu a0,zero,0xb` @0x801036d8) ist keine Lauf-Kamera, der Wechsel ist darum immer ein Schnitt.
#   ROOM5090 Wagen C (@0x47C-0x557): 11->12 @0x00490, 12->11 @0x004B8, 12->13 @0x004CC, 13->12 @0x004F4, 13->14 @0x00508,
#     14->13 @0x00530; Kamera 15 hat nur ihren Anker `01ff0f00` @0x00544 (SCD-Kamera: Cut_chg(15) im Auftritt der Riesenform).
# Warum es den Schnitt braucht: RE2 zeichnet das Bild, in dem der Schnitt angefordert wird, nicht (FUN_8002b968: Dirty @0x8002ba0c,
# `jal 0x8002bdf4` @0x8002ba50, `j 0x8002bd60` @0x8002bb58 ueber die DrawOTag @0x8002bd40-58; RE1.5 gleich @0x80021558/@0x80021560,
# Port kamerahalt_r41.c). Ist die "Game over"-Kamera die gerade gezeigte, gibt es keinen Wechsel und das Versetzen auf die Fixpunkte
# ist im selben Bild zu sehen (Runde 3: Schnitt 12, Leon springt 6191 nach vorn).
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

def lauf_kameras(path):
    """Ziele der echten RVD-Wechsel (ohne den Anker je Kamera) -> Menge, dazu die Belegzeilen."""
    d = open(path, 'rb').read(); o = struct.unpack_from('<I', d, 0x28)[0]
    ziele, beleg, vorher = set(), [], None
    while d[o] != 0xFF:
        von, nach = d[o + 2], d[o + 3]
        if von == vorher:
            ziele.add(nach); beleg.append('%02x%02x%02x%02x @0x%05X (%d->%d)' % (d[o], d[o + 1], von, nach, o, von, nach))
        vorher = von; o += 20
    return ziele, beleg
LAUF_RE2, BELEG_RE2 = lauf_kameras(os.path.join(REPO, 'info/re2leon/PL0/RDT/ROOM7020.RDT'))
LAUF_50, BELEG_50 = lauf_kameras(os.path.join(REPO, 're15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT'))
assert 11 not in LAUF_RE2, 'RE2 Schnitt 11 waere eine Lauf-Kamera'
GO_ERLAUBT = tuple(k for k in WC if k not in LAUF_50)    # Runde 4: nur (15,)
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
        if vk >= 0.6 and k in GO_ERLAUBT:                       # Runde 4: "Game over" keine Lauf-Kamera (Kopf)
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

def zeile(fx):
    """--gemeinsam (Runde 3 / Runde 4 Schritt 0): alle vier Rollen an EINER Lage."""
    out = []
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
            if best: out.append((best[0], fx, fz, w) + best[1:])
    return out

def zeile_w(fx):
    """Runde 4 Schritt 1: Lage der Wiederholungen (W1/W2/W3, jeder Wechsel ein Schnitt, W1 = W3 erlaubt)."""
    out = []
    for fz in range(-24750, -22000, 250):
        for w in range(256):
            a = w * 2 * math.pi / 256; dx, dz = math.cos(a), math.sin(a)
            if not all(drin(fx + dx * d, fz + dz * d) for d in (10000, 5000, 3000)): continue
            tab = bewerte((fx, fz), w)
            best = None
            for a1, a2, a3 in itertools.product(WC, WC, WC):
                if a1 == a2 or a2 == a3 or a3 in GO_ERLAUBT and len(GO_ERLAUBT) == 1: continue   # W3 != "Game over" (Schnitt 15)
                z = [tab[a1].get('W1'), tab[a2].get('W2'), tab[a3].get('W3')]
                if any(v is None for v in z): continue
                s = sum(v[0] for v in z)
                if best is None or s < best[0]: best = (s, (a1, a2, a3), [v[0] for v in z], [v[3] for v in z], [v[4] for v in z])
            if best: out.append((best[0], fx, fz, w) + best[1:])
    return out

def zeile_go(fx):
    """Runde 4 Schritt 2: eigene Lage fuer "Game over" auf einer Nicht-Lauf-Kamera (PORT-WAHL, Kopf von re15_finale_r42.h).
    Hart: Form ganz im Wagen, Spieler (7668 vor der Form, RE2-Fixpunkte) zu mindestens 80 % sichtbar, Formmitte ausser Bild (RE2 11:
    +86,5 Grad). Kosten wie oben: k1 gegen RE2 Schnitt 11 + 6 x Bedeckung."""
    out = []
    for fz in range(-26500, -20000, 250):
        for w in range(256):
            a = w * 2 * math.pi / 256; dx, dz = math.cos(a), math.sin(a)
            pl = (fx + dx * 7668, fz + dz * 7668)
            if not drin(*pl): continue
            gier = (-w * 16) & 4095
            FW = welt(FORM_ALL, (fx, fz), gier)
            if not im_wagen(FW): continue
            for k in GO_ERLAUBT:
                zb, bed = zpuffer(k, FW)
                vk = sicht(k, zb, koerper(pl[0], pl[1], dx, dz)).mean()
                ms = mass_p(k, pl)
                if vk < 0.8 or ms is None: continue
                uc, vc, zc = proj(k, np.array([[fx, -1500.0, fz]]))
                if zc[0] > 50 and 0 <= uc[0] < 320 and 0 <= vc[0] < 240: continue
                Z = ZIEL['GameOver'][1]
                kk = min(k1(ms, (Z[0] * sp, Z[1])) for sp in (1, -1)) + 6.0 * bed
                out.append((kk, fx, fz, w, gier, k, ms[0], ms[1], bed, vk, float(uc[0]), pl))
    return out

def main():
    print('Lauf-Kameras RE2 ROOM7020:', sorted(LAUF_RE2), '| ROOM5090:', sorted(LAUF_50), '| "Game over" erlaubt:', GO_ERLAUBT)
    for b in BELEG_50:
        if int(b.split('(')[1].split('-')[0]) >= 11: print('  5090', b)
    from multiprocessing import Pool
    n = int(os.environ.get('U3_PROZESSE', '1'))
    if '--gemeinsam' in sys.argv:
        res = []
        with Pool(n) as pool:
            for r in pool.map(zeile, range(-1000, 25001, 500)): res.extend(r)
        res.sort(key=lambda r: r[0])
        print(len(res), 'Loesungen (alle vier Rollen an einer Lage)')
        for r in res[:25]:
            s, fx, fz, w, ks, kost, bed, vis = r
            print('%.2f F=(%d,%d) Achse %d/256 (Gier %d) GO %d W1 %d W2 %d W3 %d  Kosten [%s] Bedeckung [%s] sichtbar [%s]' % (
                s, fx, fz, w, (-w * 16) & 4095, *ks, ' '.join('%.2f' % v for v in kost), ' '.join('%.2f' % v for v in bed),
                ' '.join('%.2f' % v for v in vis)))
        return
    res = []
    with Pool(n) as pool:
        for r in pool.map(zeile_w, range(-1000, 25001, 500)): res.extend(r)
    res.sort(key=lambda r: r[0])
    print('Schritt 1 - Wiederholungen:', len(res), 'Loesungen')
    for r in res[:10]:
        s, fx, fz, w, ks, kost, bed, vis = r
        print('%.2f F=(%d,%d) Achse %d/256 (Gier %d) W1 %d W2 %d W3 %d  Kosten [%s] Bedeckung [%s] sichtbar [%s]' % (
            s, fx, fz, w, (-w * 16) & 4095, *ks, ' '.join('%.2f' % v for v in kost), ' '.join('%.2f' % v for v in bed),
            ' '.join('%.2f' % v for v in vis)))
    go = []
    with Pool(n) as pool:
        for r in pool.map(zeile_go, range(-1000, 25001, 250)): go.extend(r)
    best = min(r[0] for r in go)
    go.sort(key=lambda r: (r[0] > best + 0.01, -r[9], r[0]))   # Gleichstand (0,01): groesste Sichtbarkeit des Spielers
    print('Schritt 2 - "Game over" mit eigener Lage:', len(go), 'Lagen')
    for r in go[:12]:
        print('%.2f F=(%d,%d) Achse %d/256 (Gier %d) Schnitt %d  Spieler u %+.2f Groesse %.3f  Bedeckung %.2f  sichtbar %.2f  Formmitte u %.0f'
              '  Spieler (%d,%d)' % (r[:11] + (int(r[11][0]), int(r[11][1]))))

if __name__ == '__main__':
    main()
