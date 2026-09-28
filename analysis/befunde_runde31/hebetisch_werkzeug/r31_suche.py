# -*- coding: utf-8 -*-
"""Runde 31 / H — Suche nach Sitzen: Granate LINKS, Sicherung RECHTS (Cut 4), beide im Kuppelfach.
Harte Bedingungen (je Gegenstand, jede Probe = Punkt, Kantenprobe, Flaechenmitte, r31_geo.proben):
  (1) Luft unter der GESCHLOSSENEN Kuppel >= 0   (Prop 1/2 wie ausgeliefert)
  (2) Luft unter den OFFENEN Deckeln >= 0          (Prop 1 +150 z, Prop 2 -150 z; For @0x0FC0)
  (3) Grundriss im Achteck des Fachbodens (Punkte @0x121AC..@0x1221C)
  (4) kein Durchdringen Granate <-> Sicherung (Abstand zur Zylinder-Mantelflaeche >= 1)
Hoehenfeld der Kuppel auf 1er-Raster (numpy), Nachpruefung der Besten mit der exakten
Dreiecksrechnung (r31_geo.Fach.luft). NUR PLANUNG."""
import math, os, sys, itertools
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_geo as G  # noqa

X0, X1, Z0, Z1 = -500, -60, 700, 1820


def feld(tris):
    xs = np.arange(X0, X1 + 1, dtype=np.float64)
    zs = np.arange(Z0, Z1 + 1, dtype=np.float64)
    XX, ZZ = np.meshgrid(xs, zs, indexing='ij')
    best = np.full(XX.shape, np.inf)
    for (a, b, c) in tris:
        den = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2])
        if den == 0:
            continue
        l1 = ((b[2] - c[2]) * (XX - c[0]) + (c[0] - b[0]) * (ZZ - c[2])) / den
        l2 = ((c[2] - a[2]) * (XX - c[0]) + (a[0] - c[0]) * (ZZ - c[2])) / den
        l3 = 1 - l1 - l2
        m = (l1 >= -1e-9) & (l2 >= -1e-9) & (l3 >= -1e-9)
        y = l1 * a[1] + l2 * b[1] + l3 * c[1]
        h = G.BODEN - y
        m &= h > 0.5
        best = np.where(m & (h < best), h, best)
    return best


def achteck_maske():
    pts = [(-485, 1260), (-432, 958), (-280, 875), (-128, 958), (-74, 1260), (-128, 1562), (-280, 1645), (-432, 1562)]
    xs = np.arange(X0, X1 + 1); zs = np.arange(Z0, Z1 + 1)
    XX, ZZ = np.meshgrid(xs, zs, indexing='ij')
    ok = np.ones(XX.shape, bool)
    for i in range(8):
        (x1, z1), (x2, z2) = pts[i], pts[(i + 1) % 8]
        c = (x2 - x1) * (ZZ - z1) - (z2 - z1) * (XX - x1)
        ok &= c <= 0   # Umlaufsinn: pruefen unten per Mittelpunkt
    if not ok[int(-280 - X0), int(1260 - Z0)]:
        ok = np.ones(XX.shape, bool)
        for i in range(8):
            (x1, z1), (x2, z2) = pts[i], pts[(i + 1) % 8]
            c = (x2 - x1) * (ZZ - z1) - (z2 - z1) * (XX - x1)
            ok &= c >= 0
    return ok


class Schnell:
    def __init__(self, raum='ROOM1150.RDT'):
        self.f = G.Fach(raum)
        self.hz = feld(self.f.zu)
        self.ho = feld(self.f.offen)
        self.acht = achteck_maske()

    def luft(self, P):
        """P: (n,3) numpy. -> (luft_zu, luft_offen, raus)"""
        ix = np.clip(np.floor(P[:, 0] - X0).astype(int), 0, self.hz.shape[0] - 2)
        iz = np.clip(np.floor(P[:, 2] - Z0).astype(int), 0, self.hz.shape[1] - 2)
        h = G.BODEN - P[:, 1]
        # konservativ: kleinste Kuppelhoehe der 4 Nachbarzellen
        kz = np.minimum.reduce([self.hz[ix, iz], self.hz[ix + 1, iz], self.hz[ix, iz + 1], self.hz[ix + 1, iz + 1]])
        ko = np.minimum.reduce([self.ho[ix, iz], self.ho[ix + 1, iz], self.ho[ix, iz + 1], self.ho[ix + 1, iz + 1]])
        lz = np.min(np.where(np.isinf(kz), -1e9, kz) - h)
        drin = (P[:, 2] > G.OFFEN_LO) & (P[:, 2] < G.OFFEN_HI)
        lo_arr = np.where(drin | np.isinf(ko), 1e9, ko - h)
        lo = np.min(lo_arr)
        raus = int(np.sum(~(self.acht[ix, iz] & self.acht[ix + 1, iz + 1])))
        return lz, lo, raus


def dreh(pts, ry):
    w = ry * 2 * math.pi / 4096
    c, s = math.cos(w), math.sin(w)
    P = np.array(pts, float)
    return np.stack([P[:, 0] * c + P[:, 2] * s, P[:, 1], -P[:, 0] * s + P[:, 2] * c], 1)


def abstand(PG, s_pos, s_ry, halb=203, r=26):
    w = s_ry * 2 * math.pi / 4096
    ax = np.array([math.cos(w), 0, -math.sin(w)])
    d = PG - np.array(s_pos, float)
    t = d @ ax
    radial = np.sqrt(np.maximum(0, np.sum(d * d, 1) - t * t))
    innen = np.abs(t) <= halb
    a_in = np.where(radial < r, np.maximum(radial - r, np.abs(t) - halb), radial - r)
    a_out = np.hypot(np.maximum(0, radial - r), np.abs(t) - halb)
    return float(np.min(np.where(innen, a_in, a_out)))


def main():
    sch = Schnell(sys.argv[1] if len(sys.argv) > 1 else 'ROOM1150.RDT')
    sich, gran = G.modelle()
    # --- 1) zulaessige Sicherungs-Sitze -------------------------------------------------
    s_ok = []
    for ry in list(range(0, 2048, 64)):
        Ps0 = dreh(sich, ry)
        for xs in range(-440, -119, 10):
            for zs in range(1150, 1501, 10):
                P = Ps0 + np.array([xs, -1062, zs])
                lz, lo, raus = sch.luft(P)
                if lz >= 0 and lo >= 0 and raus == 0:
                    anteil = float(np.mean((P[:, 2] > G.OFFEN_LO) & (P[:, 2] < G.OFFEN_HI)))
                    s_ok.append((ry, xs, zs, lz, lo, anteil, float(P[:, 2].mean())))
    print('zulaessige Sicherungs-Sitze: %d' % len(s_ok))
    # je Drehung: der am weitesten RECHTS liegende (groesste mittlere z), und groesster Oeffnungsanteil
    for ry in sorted(set(s[0] for s in s_ok)):
        kand = [s for s in s_ok if s[0] == ry]
        r = max(kand, key=lambda s: s[6])
        print('  Sicherung ry %4d: %3d Sitze; am weitesten rechts x %d z %d (z-Mitte %.0f, Luft zu %.1f offen %.1f, in Oeffnung %.2f)'
              % (ry, len(kand), r[1], r[2], r[6], r[3], r[4], r[5]))
    # --- 2) zulaessige Granaten-Sitze -----------------------------------------------------
    g_ok = []
    for ry in (0, 256, 512, 768, 1024, 1280, 1536, 1792):
        Pg0 = dreh(gran, ry)
        for yc in (-1091, -1088):
            for xg in range(-400, -159, 5):
                for zg in range(1040, 1301, 5):
                    P = Pg0 + np.array([xg, yc, zg])
                    lz, lo, raus = sch.luft(P)
                    if lz >= 0 and lo >= 0 and raus == 0:
                        anteil = float(np.mean((P[:, 2] > G.OFFEN_LO) & (P[:, 2] < G.OFFEN_HI)))
                        g_ok.append((ry, xg, yc, zg, lz, lo, anteil, float(P[:, 2].mean()), P))
    print('zulaessige Granaten-Sitze: %d' % len(g_ok))
    for ry in sorted(set(g[0] for g in g_ok)):
        for yc in (-1091, -1088):
            kand = [g for g in g_ok if g[0] == ry and g[2] == yc]
            if not kand:
                print('  Granate ry %4d yc %d: keine' % (ry, yc)); continue
            l = min(kand, key=lambda g: g[7])
            voll = [g for g in kand if g[6] >= 0.999]
            lv = min(voll, key=lambda g: g[7]) if voll else None
            print('  Granate ry %4d yc %d: %4d Sitze; am weitesten links x %d z %d (z-Mitte %.0f, in Oeffnung %.2f)%s'
                  % (ry, yc, len(kand), l[1], l[3], l[7], l[6],
                     ('; ganz in der Oeffnung am weitesten links x %d z %d' % (lv[1], lv[3])) if lv else ''))
    np.save(os.path.join(H, '..', '..', '..', 'build', 'r31_hebetisch', 's_ok.npy'), np.array([s[:7] for s in s_ok]))
    np.save(os.path.join(H, '..', '..', '..', 'build', 'r31_hebetisch', 'g_ok.npy'), np.array([g[:8] for g in g_ok]))


if __name__ == '__main__':
    main()
