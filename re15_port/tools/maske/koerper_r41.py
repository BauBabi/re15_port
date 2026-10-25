"""Runde 41 Spur M, Nachbesserung 3 (Abnahme 2): KOERPER einer SCA-Zelle als Prisma - Eintritt UND Austritt des Sehstrahls je
Bildpunkt, mit der FORM, die der Kollisions-Resolver des Ports fragt (re15_collision.c, FUN_8003b0a4; dieselbe Form fragt der
C-Ausschluss test_r37_masken36.c ueber form_drin / r41_form).

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 3".

Warum ein eigenes Modul: geometrie.quader_auf_band liefert nur den EINTRITT (naechster Treffer), und geom.zellen_solid_test kennt
Zellentyp 2 nicht ("typ 2/8/9 gibt ebenfalls None (Form hier nicht modelliert)" -> Huellrechteck). Der C-Ausschluss urteilt fuer
Typ 2 aber mit der RAUTE des Resolvers: push_diag2 = LAB_8003d00c (ghidra1_V2.txt:145450-145916, re15_collision.c), Quadrant aus
quadrant_of (Mitte cx = x + (breite >> 1), cz = z + (tiefe >> 1)), je Quadrant die Kante von der Kanten-Mitte zur Ecken-Mitte:
  q0 (px >= cx, pz >= cz): fest, wenn pz < z_max - (tiefe/2) * (px - cx) / (breite/2)   (Kante (cx, z_max) - (x_max, cz))
  q1 (px <  cx, pz >= cz): fest, wenn pz - cz < (tiefe/2) * (px - x) / (breite/2)        (Kante (x, cz) - (cx, z_max))
  q2 (px >= cx, pz <  cz): fest, wenn pz - z > (tiefe/2) * (px - cx) / (breite/2)        (Kante (cx, z) - (x_max, cz))
  q3 (px <  cx, pz <  cz): Kante (x, cz) - (cx, z)
(Radius 0, wie form_drin im Ausschluss) = die Raute mit den Ecken (x, cz), (cx, z_max), (x_max, cz), (cx, z). Typ 1 = Kasten,
Typ 4..7 = halber Kasten (Schraege wie geom.zellen_solid_test, Belege dort), Typ 3 = Kreis (push_circle, Mitte aus der Breite).
Die Hoehe ist eine MESSUNG (IoU-Maximum der Koerper-Silhouette gegen den Gegenstand, wie geometrie.hoehe_messen), keine
Spielkonstante. Saeule = ohne Deckel.

Rechnung: Strahl Kamera -> Bildpunkt (Ansicht wie geometrie.quader_auf_band: Richtung R^-1 * (sx - 160, sy - 120, H)), Prisma =
konvexes Vieleck x [Deckel, Bandboden] -> Cyrus-Beck mit den Halbebenen des Vielecks und den zwei y-Ebenen; Kreis quadratisch.
Kamera-z (vz) = (R[6..8] . q) / 4096 + t[2] wie geometrie.quader_auf_band._eintragen. Eintritt = erster Punkt im Koerper vor
der Kamera, Austritt = letzter.
"""
import numpy as np

BAND_HOEHE = 1800.0          # Band-Boden y = -Band * 0x708 (re15_collision.h BAND_Y, test_r37_masken36.c BAND_Y)


def bucket(vz):
    """OT-Stufe der Figur: (1023 * vz) >> 16 (@0x8002565c otz>>4 mit ZSF3 = 341 @0x80066c70; geometrie.bucket)."""
    return (1023 * np.asarray(vz, np.int64).clip(0)) >> 16


def halbebenen(typ, x, z, w, d):
    """Konvexes Vieleck der Zelle als Halbebenen (nx, nz, c) mit 'drin' <=> nx*px + nz*pz <= c; None fuer Kreis/unbekannt."""
    X0, Z0, X1, Z1 = float(x), float(z), float(x + w), float(z + d)
    kasten = [(-1.0, 0.0, -X0), (1.0, 0.0, X1), (0.0, -1.0, -Z0), (0.0, 1.0, Z1)]
    typ = int(typ)
    if typ == 1:
        return kasten
    if typ == 2:
        cx, cz = float(x + (int(w) >> 1)), float(z + (int(d) >> 1))       # quadrant_of_cell: Mitte mit >> 1
        ecken = [(X0, cz), (cx, Z0), (X1, cz), (cx, Z1)]                   # gegen den Uhrzeigersinn in (x, z)
        hs = []
        for i in range(4):
            (ax, az), (bx, bz) = ecken[i], ecken[(i + 1) % 4]
            nx, nz = (bz - az), -(bx - ax)                                  # Normale nach aussen fuer diese Reihenfolge
            c = nx * ax + nz * az
            # Seite pruefen: die Mitte muss drin sein
            if nx * cx + nz * cz > c:
                nx, nz, c = -nx, -nz, -c
            hs.append((nx, nz, c))
        return hs
    if w <= 0:
        return None
    m = d / float(w)
    # Schraegen wie geom.zellen_solid_test (Belege dort): Halbebene der festen Seite
    if typ == 5:      # pz >= Z0 + m*(px-X0)   <=>  m*px - pz <= m*X0 - Z0
        return kasten + [(m, -1.0, m * X0 - Z0)]
    if typ == 6:      # pz <= Z0 + m*(px-X0)   <=>  -m*px + pz <= Z0 - m*X0
        return kasten + [(-m, 1.0, Z0 - m * X0)]
    if typ == 4:      # pz >= Z1 - m*(px-X0)   <=>  -m*px - pz <= -Z1 - m*X0
        return kasten + [(-m, -1.0, -Z1 - m * X0)]
    if typ == 7:      # pz <= Z1 - m*(px-X0)   <=>  m*px + pz <= Z1 + m*X0
        return kasten + [(m, 1.0, Z1 + m * X0)]
    return None


def drin(typ, x, z, w, d, px, pz):
    """Punkt(e) im Grundriss der Zelle (Radius 0)."""
    hs = halbebenen(typ, x, z, w, d)
    px = np.asarray(px, float); pz = np.asarray(pz, float)
    if hs is None:
        if int(typ) == 3:
            cr = w / 2.0
            return (px - (x + cr)) ** 2 + (pz - (z + cr)) ** 2 <= cr * cr + 1e-6
        raise ValueError("Zellentyp %d nicht modelliert" % typ)
    ok = np.ones(np.broadcast(px, pz).shape, bool)
    for nx, nz, c in hs:
        ok &= nx * px + nz * pz <= c + 1e-6
    return ok


def ansicht(R, t, H):
    Rm = np.array(R, float).reshape(3, 3) / 4096.0
    Ri = np.linalg.inv(Rm)
    c = Ri.dot(-np.array(t, float))
    sx, sy = np.meshgrid(np.arange(320) - 160.0, np.arange(240) - 120.0)
    d = np.stack([sx, sy, np.full_like(sx, float(H))], -1) @ Ri.T
    return c, d


def _vz(R, t, c, d, s):
    q = c + s[..., None] * d
    return (q[..., 0] * R[6] + q[..., 1] * R[7] + q[..., 2] * R[8]) / 4096.0 + t[2]


def ein_aus(R, t, H, typ, x, z, w, d, band, hoehe, saeule):
    """-> (vz_ein, vz_aus, treffer) je Bildpunkt 240x320 fuer den Koerper der Zelle (Prisma). hoehe negativ (nach oben)."""
    c, dd = ansicht(R, t, H)
    y0 = -BAND_HOEHE * band
    s0 = np.full((240, 320), 1e-9)
    s1 = np.full((240, 320), np.inf)
    ok = np.ones((240, 320), bool)
    # y-Schicht: Bandboden y0 (unten, PSX-Y nach unten), Deckel y0 + hoehe
    ylo = -np.inf if saeule else y0 + hoehe
    dy = dd[..., 1]
    par = np.abs(dy) < 1e-12
    with np.errstate(divide="ignore", invalid="ignore"):
        a = (ylo - c[1]) / dy
        b = (y0 - c[1]) / dy
    lo = np.where(par, -np.inf, np.minimum(a, b))
    hi = np.where(par, np.inf, np.maximum(a, b))
    ok &= ~(par & ((c[1] < ylo) | (c[1] > y0)))
    s0 = np.maximum(s0, lo)
    s1 = np.minimum(s1, hi)
    hs = halbebenen(typ, x, z, w, d)
    if hs is None:
        if int(typ) != 3:
            raise ValueError("Zellentyp %d nicht modelliert" % typ)
        cr = w / 2.0
        cx, cz = x + cr, z + cr
        ox, oz = c[0] - cx, c[2] - cz
        A = dd[..., 0] ** 2 + dd[..., 2] ** 2
        B = 2.0 * (ox * dd[..., 0] + oz * dd[..., 2])
        C = ox * ox + oz * oz - cr * cr
        disk = B * B - 4 * A * C
        ok &= (disk >= 0) & (A > 1e-12)
        with np.errstate(divide="ignore", invalid="ignore"):
            wz = np.sqrt(np.maximum(disk, 0.0))
            r0 = (-B - wz) / (2 * A)
            r1 = (-B + wz) / (2 * A)
        s0 = np.maximum(s0, np.where(ok, r0, s0))
        s1 = np.minimum(s1, np.where(ok, r1, s1))
    else:
        for nx, nz, cc in hs:
            den = nx * dd[..., 0] + nz * dd[..., 2]
            num = cc - (nx * c[0] + nz * c[2])
            par = np.abs(den) < 1e-12
            ok &= ~(par & (num < 0))
            with np.errstate(divide="ignore", invalid="ignore"):
                tt = num / den
            s1 = np.where(~par & (den > 0), np.minimum(s1, tt), s1)
            s0 = np.where(~par & (den < 0), np.maximum(s0, tt), s0)
    ok &= s0 < s1
    ve = np.where(ok, _vz(R, t, c, dd, np.where(ok, s0, 0.0)), 0.0)
    va = np.where(ok, _vz(R, t, c, dd, np.where(ok, np.minimum(s1, 1e7), 0.0)), 0.0)
    ok &= ve > 64            # vor der Kamera (proj: vz <= 64 faellt weg)
    return np.where(ok, ve, 0.0), np.where(ok, va, 0.0), ok


def kamera_im_grundriss(R, t, typ, x, z, w, d):
    """Kamera UEBER/IN dem Grundriss (Huellrechteck, wie r41_koerper_davor -1: Nahfeld, kein Urteil)."""
    Rm = np.array(R, float).reshape(3, 3) / 4096.0
    c = np.linalg.inv(Rm).dot(-np.array(t, float))
    return (x <= c[0] <= x + w) and (z <= c[2] <= z + d)


def hoehe_messen(region, R, t, H, typ, x, z, w, d, band):
    """IoU-Maximum der Koerper-Silhouette gegen die Region (wie geometrie.hoehe_messen: -100..-4000 in 100ern, dann +-100 in
    25ern; Saeule wenn die IoU bis zum Rand waechst). -> (hoehe, iou, saeule) oder None."""
    reg = region.astype(bool)

    def iou(h, saeule=False):
        _, _, tr = ein_aus(R, t, H, typ, x, z, w, d, band, h, saeule)
        u = float((tr & reg).sum())
        v = float((tr | reg).sum())
        return (u / v) if v else 0.0

    grob = list(range(-100, -4001, -100))
    werte = [(h, iou(h)) for h in grob]
    hb, ib = max(werte, key=lambda q: q[1])
    if ib <= 0.0:
        return None
    saeule = (hb == grob[-1]) and all(werte[i][1] <= werte[i + 1][1] + 1e-9 for i in range(len(werte) - 6, len(werte) - 1))
    fein = [(h, iou(h)) for h in range(hb + 100, hb - 101, -25) if h < 0]
    hb, ib = max(fein + [(hb, ib)], key=lambda q: q[1])
    return hb, ib, saeule
