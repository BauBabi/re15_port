#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur F "gatorszene", Nachbesserung 4 (Nachtrag 29): Lage der Leiche samt Kette WEITER WEG von der Leiter.

  python re15_port/tools/r41_gatorszene/leiche_lage_n4.py [--liste N]

NUTZER-VORGABE (AUFTRAG.md Nachtrag 29, 2026-10-25 12:08): "Die Zombie Leiche inklusive der Kette - Schluessel - Aligator Fressen
etc. muss weiter weg sein, damit man am Start des Kampfes eine Chance hat." Orchestrator: Abstand Gator <-> Spieler am Kampfbeginn
mindestens wie am Ende des RE2-Intros in ROOM40A0. Freiheit (Nachtrag 6): "Du darfst auch die Startposition des Aligators und die
Position der schwimmenden Zombie Leiche, die sichtbar den Schluessel hat aendern".

Sollwert (tools/r41_gatorszene/probe_re2_intro_ende.c, Adressen dort): RE2-Intro-Ende Wurzel (503,-24100) Gier 2048, Leon
(-10800,-25800) -> Abstand Wurzel <-> Leon 11430 (Welteinheiten; Leon ist in beiden Spielen gleich gross).

Gleiche Form wie leiche_lage.py (Fassung 1), mit drei Aenderungen:
  (1) die Anlauf-Richtung des Gators ist frei (Raster 128 statt fest Gier 1024 aus dem Norden),
  (2) Wasser = Becken des Boss-Moduls (enemy_ai_boss_gator.c gb_becken_draussen, ROOM2090-SCA: Aussenwaende x[-8900..7200]
      z[-27000..-5400] ohne Plattform x[-1700..1850] z[-20000..-12450] und Rampe x[1850..7200] z[-18100..-14500]); ein Rand m
      gilt gegen diese Vereinigung (die Innengrenzen der drei sce-8-Zonen sind kein Ufer),
  (3) die Kette muss den Abstand einhalten.
Pruefungen je Lage:
  - Leiche: alle Huellpunkte des Modells (MD1 @0x2054) mit Rand im Bild des Cuts (x 24..296, y 40..205, wie Fassung 1), die vier
    Aussenpunkte mit Rand 250 im Wasser (wie Fassung 1);
  - Buehne (gatorszene_r41.c ga_buehne): Kopf im Koppelbild 13 auf der Rumpfmitte, Start S = Mitte - Kopf13 - Weg4 - Weg6;
    Rig-Vektoren aus der Messzeile "[gatorszene] Buehne: ... Kopf13-Versatz (45,-3055) Weg4 (0,-1328) Weg6 (-16,-6332)" bei
    Gier 1024 (Welt (x,z) = (vz, -vx) -> Rig (vx,vz) = (-z, x)): Kopf13 (3055,45), Weg4 (1328,0), Weg6 (6332,-16);
  - Weg im Wasser: Wurzel von S bis zur Kau-Lage mit Rand 900 (wie S in Fassung 1), Kopf (Wurzel + Kopf13) mit Rand 500;
  - Kau-Lage = Wurzel im Koppelbild + (Clip 11 - Clip 4 Bild 13) = (3132 - 2802) x 2/3 = 220 Rig-x (EMR-Bytes 6..11; Weg4
    1328 = 2/3 x (off4[13] - off4[0] 810) -> off4[13] = 2802; Clip 11 fest 3132);
  - Abstand: Kau-Lage <-> Leiter-Ankunft (-8200,-25550) (ROOM2080 Slot 1 @0x00BC2) minus 700 (Wende Clip 0 <= 28 Bilder + Schrei
    Clip 10 Wurzel 0 -> -1025 x 2/3; Abnahme 3 gemessen Kau-Lage -> Kampfbeginn (295,487)) >= 11430;
  - Wert = Bildgroesse^2 x Licht des Schluessels (wie Fassung 1); Neigung wie Fassung 1 (<= 60 Grad, Puls unter der Kappung).
"""
import math, os, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import leiche_lage as L   # lade(), licht(), M(), mul(), mv(), norm(), S(), C(), KEY_LOKAL, MITTE_X, LEICHE_Y

KOPF13_R = (3055, 45)
WEG4_R = (1328, 0)
WEG6_R = (6332, -16)
KAU_R = 220
LEITER = (-8200, -25550)
SOLL_RE2 = 11430
WENDE_SCHREI = 700
BECKEN = (-8900, 7200, -27000, -5400)
BLOECKE = ((-1700, 1850, -20000, -12450), (1850, 7200, -18100, -14500))


def draussen(X, Z, m):
    """gb_becken_draussen (enemy_ai_boss_gator.c) fuer Felder: > 0 = nicht mit Rand m im Wasser."""
    X = np.asarray(X, float); Z = np.asarray(Z, float)
    d = np.zeros(np.broadcast(X, Z).shape)
    d = np.maximum(d, (BECKEN[0] + m) - X); d = np.maximum(d, X - (BECKEN[1] - m))
    d = np.maximum(d, (BECKEN[2] + m) - Z); d = np.maximum(d, Z - (BECKEN[3] - m))
    for x0, x1, z0, z1 in BLOECKE:
        x0 -= m; x1 += m; z0 -= m; z1 += m
        innen = (X > x0) & (X < x1) & (Z > z0) & (Z < z1)
        t = np.minimum(np.minimum(X - x0, x1 - X), np.minimum(Z - z0, z1 - Z))
        d = np.where(innen, np.maximum(d, t), d)
    return d


def welt(theta, v):
    """Rig (vx, vz) -> Welt (x, z) bei Gier theta (FUN_800152c8: x = c*vx + s*vz, z = -s*vx + c*vz)."""
    c, s = L.C(theta), L.S(theta)
    return (c * v[0] + s * v[1], -s * v[0] + c * v[1])


def kette(theta, mx, mz):
    k13 = welt(theta, KOPF13_R); w4 = welt(theta, WEG4_R); w6 = welt(theta, WEG6_R); kau = welt(theta, (KAU_R, 0))
    r13 = (mx - k13[0], mz - k13[1])
    a = (r13[0] - w4[0], r13[1] - w4[1])
    s = (a[0] - w6[0], a[1] - w6[1])
    return s, r13, (r13[0] + kau[0], r13[1] + kau[1]), k13


def weg_ok(s, kau, k13):
    t = np.linspace(0.0, 1.0, 60)
    X = s[0] + (kau[0] - s[0]) * t; Z = s[1] + (kau[1] - s[1]) * t
    if (draussen(X, Z, 900) > 0).any():
        return False
    return not (draussen(X + k13[0], Z + k13[1], 500) > 0).any()


def huelle(V):
    """Teilmenge der Modellpunkte, die alle Projektions-Extrema traegt (Extrempunkte in 400 Richtungen)."""
    rng = np.random.default_rng(41)
    D = rng.normal(size=(400, 3)); D /= np.linalg.norm(D, axis=1)[:, None]
    idx = set(np.argmax(V @ D.T, axis=0).tolist())
    return V[sorted(idx)]


def bildkasten(V, x, z, ry, cam):
    p, t, sd = cam
    f = np.array(L.norm([t[i] - p[i] for i in range(3)])); r = np.array(L.norm([f[2], 0, -f[0]])); u = np.cross(f, r)
    W = V @ np.array(L.M(0, ry, 0)).reshape(3, 3).T + np.array([x, L.LEICHE_Y, z]) - np.array(p, float)
    Zc = W @ f
    if Zc.min() <= 300:
        return None
    bx = 160 + sd * (W @ r) / Zc; by = 120 + sd * (W @ u) / Zc
    return bx.min(), by.min(), bx.max(), by.max()


def kandidaten(cut, cams, lichter, H):
    p, t, sd = cams[cut]
    f = np.array(L.norm([t[i] - p[i] for i in range(3)])); r = np.array(L.norm([f[2], 0, -f[0]])); u = np.cross(f, r)
    xs = np.arange(-8500, 7000, 100); zs = np.arange(-26800, -5600, 100)
    GX, GZ = np.meshgrid(xs, zs)
    out = []
    for ry in range(0, 4096, 128):
        R = np.array(L.M(0, ry, 0)).reshape(3, 3)
        ok = np.ones(GX.shape, bool)
        for e in ((-444, 0, 0), (2626, 0, 0), (1090, 0, -1049), (1090, 0, 822)):
            w = R @ np.array(e, float)
            ok &= draussen(GX + w[0], GZ + w[2], 250) <= 0
        if not ok.any():
            continue
        Hw = H @ R.T
        bmin = np.full(GX.shape, 1e9); bmax = np.full(GX.shape, -1e9)
        cmin = np.full(GX.shape, 1e9); cmax = np.full(GX.shape, -1e9)
        for h in Hw:
            Qx = GX + h[0] - p[0]; Qy = L.LEICHE_Y + h[1] - p[1]; Qz = GZ + h[2] - p[2]
            Zc = Qx * f[0] + Qy * f[1] + Qz * f[2]
            ok &= Zc > 300
            Zs = np.where(Zc > 300, Zc, 1e9)
            bx = 160 + sd * (Qx * r[0] + Qy * r[1] + Qz * r[2]) / Zs
            by = 120 + sd * (Qx * u[0] + Qy * u[1] + Qz * u[2]) / Zs
            bmin = np.minimum(bmin, bx); bmax = np.maximum(bmax, bx); cmin = np.minimum(cmin, by); cmax = np.maximum(cmax, by)
        ok &= (bmin >= 24) & (bmax <= 296) & (cmin >= 40) & (cmax <= 205)
        kl = L.mv(L.M(0, ry, 0), L.KEY_LOKAL)
        for iz, ix in zip(*np.nonzero(ok)):
            x = int(GX[iz, ix]); z = int(GZ[iz, ix])
            kw = [x + kl[0], L.LEICHE_Y + kl[1], z + kl[2]]
            gr = 180 * sd / math.dist(kw, p)
            out.append((gr * gr * L.licht(lichter[cut], kw, (0, -1, 0)), x, z, ry, gr, kw))
    out.sort(reverse=True)
    return out


def ketten_pruefen(x, z, ry):
    mx = x + L.MITTE_X * L.C(ry); mz = z - L.MITTE_X * L.S(ry)
    best = None
    for theta in range(0, 4096, 128):
        s, r13, kau, k13 = kette(theta, mx, mz)
        ab = math.dist(kau, LEITER) - WENDE_SCHREI
        if ab < SOLL_RE2 or not weg_ok(s, kau, k13):
            continue
        if best is None or ab > best[0]:
            best = (ab, theta, (mx, mz), s, kau)
    return best


def neigung(cams, lichter, cut, ry, kw):
    Rp = L.M(0, ry, 0)
    cam = cams[cut][0]
    dv = L.norm([cam[i] - kw[i] for i in range(3)])
    grenze = 255 * 0x80 / 0xBE
    bn = None
    for rx in range(-1024, 1025, 16):
        for rz in range(-1024, 1025, 16):
            Rw = L.mul(Rp, L.M(rx, 0, rz))
            n = [-Rw[1], -Rw[4], -Rw[7]]
            kipp = math.degrees(math.acos(max(-1.0, min(1.0, -n[1]))))
            cv = sum(n[i] * dv[i] for i in range(3))
            if kipp > 60 or cv <= 0:
                continue
            l = max(L.licht(lichter[cut], kw, n, k) for k in range(3))
            if l > grenze:
                continue
            if bn is None or cv * l > bn[0]:
                bn = (cv * l, rx, rz, kipp, cv, l)
    return bn, grenze


def main():
    liste = int(sys.argv[sys.argv.index("--liste") + 1]) if "--liste" in sys.argv else 1
    cams, lichter, zonen, vs = L.lade()
    V = np.array(vs, float)
    H = huelle(V)
    print("Huelle: %d von %d Modellpunkten" % (len(H), len(V)))
    # Vergleich: die Original-Lagen der beiden treibenden Leichen (Obj_model_set obj 2 @0x00A6C, obj 3 @0x00A8E)
    for name, x, z, ry in (("obj 2 @0x00A6C", -1725, -21723, 685), ("obj 3 @0x00A8E", -25, -7423, 1656),
                           ("Fassung 1", -5500, -23700, 384)):
        k = ketten_pruefen(x, z, ry)
        print("%-16s (%d,%d) Gier %d: %s" % (name, x, z, ry, "keine Kette mit Abstand >= %d im Wasser" % SOLL_RE2 if k is None else
              "Anlauf-Gier %d, Abstand - %d = %.0f" % (k[1], WENDE_SCHREI, k[0])))
    beste = []
    for cut in range(8):
        n = 0
        for wert, x, z, ry, gr, kw in kandidaten(cut, cams, lichter, H):
            k = ketten_pruefen(x, z, ry)
            if k is None:
                continue
            ab, th, mitte, s, kau = k
            if n == 0:
                beste.append((wert, cut, x, z, ry, th, mitte, s, kau, ab, gr, kw))
            print("Cut %d: Wert %6.0f Leiche (%d,%d) Gier %4d, Anlauf-Gier %4d, Mitte (%.0f,%.0f), S (%.0f,%.0f), Kau-Lage "
                  "(%.0f,%.0f), Abstand - %d = %.0f, Schluessel %.1f Bildpunkte" %
                  (cut, wert, x, z, ry, th, mitte[0], mitte[1], s[0], s[1], kau[0], kau[1], WENDE_SCHREI, ab, gr))
            n += 1
            if n >= liste:
                break
        if n == 0:
            print("Cut %d: keine Lage" % cut)
    wert, cut, x, z, ry, th, mitte, s, kau, ab, gr, kw = max(beste)
    box = bildkasten(V, x, z, ry, cams[cut])
    print("GEWAEHLT Cut %d: Leiche (%d,%d,%d) Gier %d, Anlauf-Gier %d; Bildkasten (alle %d Punkte) (%.0f,%.0f)-(%.0f,%.0f)" %
          (cut, x, L.LEICHE_Y, z, ry, th, len(V), *box))
    print("  Rumpfmitte (%.0f,%.0f), Start S (%.0f,%.0f), Kau-Lage (%.0f,%.0f), Abstand Kau-Lage <-> Leiter-Ankunft %.0f "
          "(- %d = %.0f >= %d)" % (mitte[0], mitte[1], s[0], s[1], kau[0], kau[1], ab + WENDE_SCHREI, WENDE_SCHREI, ab, SOLL_RE2))
    bn, grenze = neigung(cams, lichter, cut, ry, kw)
    _, rx, rz, kipp, cv, l = bn
    print("  Puls-Grenze Licht <= %.2f: gewaehlt Licht %.1f -> Spitze %.1f; Schluessel Welt (%.0f,%.0f,%.0f)" %
          (grenze, l, l * 0xBE / 0x80, *kw))
    print("  Neigung lokal rx %d rz %d (Kippung %.1f Grad): cos %.3f Licht %.1f; ~%.1f x %.1f Bildpunkte" %
          (rx, rz, kipp, cv, l, gr, gr * cv))
    return 0


if __name__ == "__main__":
    sys.exit(main())
