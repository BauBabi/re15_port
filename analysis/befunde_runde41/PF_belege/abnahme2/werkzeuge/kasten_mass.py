"""Abnahme 2 PF: Scherung und Seitenverhaeltnis des Kasteninhalts 2010 im Fensterbild (eigenes Werkzeug).

1) Fensterbild (960x720, gdigrab) ueber die RDT-Kamera ROOM2010 Cut 3 (@0xC0) auf die Port-Flaechen entzerrt:
   innen  = Ebene x 5400, z -2000 (links, Angel) .. -3300 (rechts), y -3650 .. -50
   fluegel= Ebene z -2000 (rot_y 1024), x 4100 (links, freier Rand) .. 5400 (rechts, Angel), y -3650 .. -50
   (Lage: tools/r41_nachtrag30/schrank_bild.py X_WAND/Z_ANGEL/Z_FREI/Y_OBEN/Y_UNTEN; unveraendert durch NB1)
2) Scherung ohne Vorlage: Projektionsprofil. Senkrechte = Winkel, unter dem die Spaltensummen von |d/dx| am schaerfsten sind;
   Waagrechte = dasselbe mit Zeilensummen von |d/dy|. Rand der Flaeche (8 %) ausgespart, weil die Flaechenkanten selbst per Bau
   achsparallel sind.
3) Seitenverhaeltnis: NCC-Gittersuche (sx, sy, Verschiebung) des entzerrten Port-Inhalts gegen ROOM2060 BG06, das ueber die
   RDT-Kamera ROOM2060 Cut 6 (@0x120) auf die 2060-Flaechen entzerrt ist. sx/sy = 1: unverzerrt.
"""
import sys, math, json
import numpy as np, cv2
import kam

S = 4.0          # Welteinheiten je Bildpunkt des entzerrten Bilds


def entzerre(bild, k, punkt, faktor):
    """bild (HxWx3), k Kamera, punkt(u,v)->Welt (Arrays), faktor = Fensterpunkte je logischem Bildpunkt."""
    P = punkt
    p = k.proj(P)
    mx = (p[..., 0] * faktor - 0.5).astype(np.float32)
    my = (p[..., 1] * faktor - 0.5).astype(np.float32)
    return cv2.remap(bild, mx, my, cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT, borderValue=0)


def port_flaechen(bild, raum='2010', cut=3):
    k = kamera_cache(raum, cut)
    f = bild.shape[1] / 320.0
    nu, nv = int(1300 / S), int(3600 / S)
    U, V = np.meshgrid((np.arange(nu) + .5) * S, (np.arange(nv) + .5) * S)
    innen = np.stack([np.full_like(U, 5400.), -3650 + V, -2000 - U], -1)
    fluegel = np.stack([4100 + U, -3650 + V, np.full_like(U, -2000.)], -1)
    return entzerre(bild, k, innen, f), entzerre(bild, k, fluegel, f)


_kc = {}


def kamera_cache(raum, cut):
    if (raum, cut) not in _kc:
        _kc[(raum, cut)] = kam.kamera(raum, cut)
    return _kc[(raum, cut)]


def grau(b):
    return cv2.cvtColor(b, cv2.COLOR_RGB2GRAY).astype(np.float32) if b.ndim == 3 else b.astype(np.float32)


def winkel_profil(g, achse, rand=0.08, bereich=8.0, schritt=0.05):
    """Dominanter Winkel (Grad) der Senkrechten (achse='s') bzw. Waagrechten (achse='w') im Bild g (grau).
    Positiv: Senkrechte oben nach rechts geneigt / Waagrechte rechts nach unten geneigt (Bildkoordinaten)."""
    h, w = g.shape
    y0, y1, x0, x1 = int(h * rand), int(h * (1 - rand)), int(w * rand), int(w * (1 - rand))
    if achse == 's':
        e = np.abs(cv2.Sobel(g, cv2.CV_32F, 1, 0, ksize=3))[y0:y1, x0:x1]
    else:
        e = np.abs(cv2.Sobel(g, cv2.CV_32F, 0, 1, ksize=3))[y0:y1, x0:x1]
    hh, ww = e.shape
    yy, xx = np.mgrid[0:hh, 0:ww].astype(np.float32)
    best = []
    for a in np.arange(-bereich, bereich + 1e-9, schritt):
        t = math.tan(math.radians(a))
        if achse == 's':        # Linie x = x0 + t*(yc - y): oben nach rechts bei a > 0
            pos = xx + t * (yy - hh / 2)
            n = ww
        else:                   # Linie y = y0 + t*(x - xc): rechts nach unten bei a > 0
            pos = yy - t * (xx - ww / 2)
            n = hh
        idx = np.round(pos).astype(int)
        ok = (idx >= 0) & (idx < n)
        prof = np.bincount(idx[ok], weights=e[ok], minlength=n)
        best.append((float(np.sum(prof ** 2)), a))
    best.sort()
    v = np.array([b[0] for b in sorted(best, key=lambda z: z[1])])
    a_best = best[-1][1]
    return a_best, v


def ncc_suche(port, ref, sx_bereich=(0.7, 1.3), schritt=0.01, scher=0.0):
    """port, ref: grau, gleicher Massstab (S). Sucht sx, sy, so dass ref(x*sx, y*sy) ~ port (Vorlage = Port-Inhalt ohne Rand)."""
    h, w = port.shape
    vor = port[int(h * .06):int(h * .94), int(w * .1):int(w * .9)]
    def norm(a):
        a = cv2.GaussianBlur(a, (0, 0), 1.0)
        return a
    vor = norm(vor)
    refn = norm(ref)
    erg = []
    for sx in np.arange(sx_bereich[0], sx_bereich[1] + 1e-9, schritt):
        for sy in np.arange(sx_bereich[0], sx_bereich[1] + 1e-9, schritt):
            M = np.array([[sx, scher * sx, 0], [0, sy, 0]], np.float32)
            t = cv2.warpAffine(vor, M, (int(vor.shape[1] * sx + abs(scher) * vor.shape[0] * sx) + 2, int(vor.shape[0] * sy) + 2),
                               flags=cv2.INTER_LINEAR)
            if t.shape[0] > refn.shape[0] or t.shape[1] > refn.shape[1]:
                continue
            r = cv2.matchTemplate(refn, t, cv2.TM_CCOEFF_NORMED)
            _, m, _, loc = cv2.minMaxLoc(r)
            erg.append((m, sx, sy, loc))
    erg.sort()
    return erg[-1]


if __name__ == '__main__':
    pass
