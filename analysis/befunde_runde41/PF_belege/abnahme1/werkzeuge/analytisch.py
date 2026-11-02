"""Mass 2c: die Abbildung des Generators (schrank_bild.py) analytisch gegen die RDT-Kamera ROOM2060 Cut 6 (@0x120, H = 26684>>7).
Kette: Port-Welt (z' bzw. Abstand von der Angel, y') -> Texel (u, v) (md1_gitter: UV 0..w-1 / 0..h-1 ueber die Flaeche) -> BG06-Bildpunkt (Generator-Homographie)
-> 2060-Welt auf der Ebene der Flaeche (Strahl der Kamera): Rueckwand z = 4500 (achsparallel; Z0 aendert nur den Massstab), Fluegel = senkrechte Ebene durch
die Angel mit der Richtung aus dem Fluchtpunkt seiner Ober-/Unterkante (fluegel.py).
Ausgabe (gleiche Konvention wie inhalt.py/fluegel.py): V = Port-Inhalt waagrecht/senkrecht gegen 2060 (1 = unverzerrt, < 1 = waagrecht gestaucht),
Neigung der Port-Senkrechten und -Waagrechten in der 2060-Welt (0 = keine Scherung)."""
import io, contextlib, numpy as np, cv2, geo
with contextlib.redirect_stdout(io.StringIO()):
    import fluegel
H = 208.0
k = geo.kamera('2060', 6)
def seitenverhaeltnis(q):
    kk = np.array([[H, 0, 160], [0, H, 120], [0, 0, 1.]]); m = cv2.getPerspectiveTransform(np.float32([(0, 0), (1, 0), (1, 1), (0, 1)]), np.float32(q)); ki = np.linalg.inv(kk)
    return float(np.linalg.norm(ki @ m[:, 0]) / np.linalg.norm(ki @ m[:, 1]))
def generator(q, w, h, ziel, alt=False):
    if alt:     # vor Spur PF: Viereck stumpf auf w x h
        return np.linalg.inv(cv2.getPerspectiveTransform(np.float32(q), np.float32([(0, 0), (w, 0), (w, h), (0, h)]))), None, 1.0
    a = seitenverhaeltnis(q); hb = float(h); wb = a * hb; wk = min(ziel * hb, wb); x0 = (wb - wk) / 2
    m = cv2.getPerspectiveTransform(np.float32(q), np.float32([(0, 0), (wb, 0), (wb, hb), (0, hb)]))
    s = np.array([[w / wk, 0, -x0 * w / wk], [0, h / hb, 0], [0, 0, 1.]])
    return np.linalg.inv(s @ m), a, wk / wb
def strahl(px, py):
    return k['r'] * (px - 160) / k['H'] + k['u'] * (py - 120) / k['H'] + k['f']
def wand(px, py):                      # Rueckwand z = 4500 -> (x, y)
    r = strahl(px, py); t = (4500.0 - k['p'][2]) / r[2]; p = k['p'] + t * r; return np.array([p[0], p[1]])
ANGEL = np.array([-4850.0, 0, 4500.0]); N = np.cross(fluegel.d, [0, 1, 0])
def fluegelebene(px, py):              # Fluegel -> (Abstand von der Angel, y)
    r = strahl(px, py); t = ((ANGEL - k['p']) @ N) / (r @ N); p = k['p'] + t * r; return np.array([-(p - ANGEL) @ fluegel.d, p[1]])
def auswerten(T, w, h, port_uv, ebene, punkte):
    def kette(a, yp):
        u, v = port_uv(a, yp); q = T @ np.array([u + .5, v + .5, 1.]); return ebene(q[0] / q[2], q[1] / q[2])
    erg = []
    for a, yp in punkte:
        e = 5.0; p0 = kette(a, yp); da = (kette(a + e, yp) - p0) / e; dy = (kette(a, yp + e) - p0) / e
        erg.append((np.linalg.norm(dy) / np.linalg.norm(da), np.degrees(np.arctan2(dy[0], dy[1])), np.degrees(np.arctan2(da[1], abs(da[0])))))
    return np.array(erg)
ZIEL = 1300 / 3600
Q_INNEN = [(48, 26), (122, 28), (126, 165), (62, 186)]
Q_EIGEN = [(48, 30), (128, 37), (131, 167), (62, 186)]
Q_TUER = [(0, 28), (48, 42), (60, 182), (22, 236)]
pk_innen = [(a, y) for a in np.linspace(-3200, -2100, 5) for y in np.linspace(-3500, -200, 7)]    # a = z' (links -2000 -> rechts -3300; +a = nach links)
uv_innen = lambda zp, yp: ((-2000 - zp) / 1300.0 * 127, (yp + 3650) / 3600.0 * 191)
pk_tuer = [(a, y) for a in np.linspace(150, 1150, 5) for y in np.linspace(-3400, -300, 7)]          # a = Abstand von der Angel (rechts 0 -> links 1300)
uv_tuer = lambda s_, yp: ((1300 - s_) / 1300.0 * 63, (yp + 3650) / 3600.0 * 191)
for name, Q, w, uv, ebene, pk, alt in [
        ('Rueckwand vorher (Q_INNEN stumpf)', Q_INNEN, 128, uv_innen, wand, pk_innen, True),
        ('Rueckwand nachher (Q_INNEN, PF)', Q_INNEN, 128, uv_innen, wand, pk_innen, False),
        ('Rueckwand mit Ecken nach Horizont', Q_EIGEN, 128, uv_innen, wand, pk_innen, False),
        ('Fluegel vorher (Q_TUER stumpf)', Q_TUER, 64, uv_tuer, fluegelebene, pk_tuer, True),
        ('Fluegel nachher (Q_TUER, PF)', Q_TUER, 64, uv_tuer, fluegelebene, pk_tuer, False)]:
    T, a, anteil = generator(Q, w, 192, ZIEL, alt)
    e = auswerten(T, w, 192, uv, ebene, pk)
    print('%-36s B/H %s | V %.3f (%.3f..%.3f) | Port-Senkrechte %+.1f (%+.1f..%+.1f) | Port-Waagrechte %+.1f (%+.1f..%+.1f) Grad' % (
        name, ('%.3f' % a) if a else '  -  ', np.median(e[:, 0]), e[:, 0].min(), e[:, 0].max(),
        np.median(e[:, 1]), e[:, 1].min(), e[:, 1].max(), np.median(e[:, 2]), e[:, 2].min(), e[:, 2].max()))
