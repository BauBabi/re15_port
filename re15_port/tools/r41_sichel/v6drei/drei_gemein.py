"""SF V6 drei Punkte (Haar, Schwanzspitze, Kralle) - gemeinsame Helfer fuer Bau, Riegel und Leisten (Bauer, 2026-11-08).

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Masse = Bildmessung an den
Originalfotos (SF_MESSBLATT_FOTO.md, a909bd2cd) und am Modell (v4_gemein.zeichne, Spielkamera 50A0/12, RDT-Licht).

Schwanzkette: b2.kette_punkte setzte die Spitze mit |relpos 22| an (V5f: 243 = Endring von Mesh 22 bei 242, gleichwertig). Ab V6 drei
ist Glied 22 laenger als relpos 22 (Glied 21); die Kette endet darum am Endring von Mesh 22 (laenge_22). kette_punkte wird hier fuer alle
Werkzeuge (T-F1, schwanz_form.passen, Riegel) ersetzt; fuer V5f ergibt das dieselben Punkte (+-1 Einheit).
"""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
R41 = os.path.dirname(HIER)
for _p in (os.path.join(R41, 'v4'), os.path.join(R41, 'foto'), os.path.join(R41, 'v6kopf'), os.path.join(R41, 'v5koerper'), HIER):
    if _p not in sys.path:
        sys.path.insert(0, _p)
import v5f_gemein as F          # noqa: E402
import kopf_orig_bau as KB      # noqa: E402

G, V, b2, BB = F.G, F.V, F.b2, F.BB
SCHWANZ = tuple(range(16, 23))
NUTZER2 = {'Beginn': (357, 243), 'Biegung': (295, 252), 'Knick': (290, 308), 'Ende': (295, 343)}   # f_schwanz.NUTZER (Messblatt 5)
KRALLE_Y = 330                     # f_krallen.Y_FINGER: Krallenflaechen = alle Ecken y >= 330 (Messblatt 6)


def laenge_22(mo):
    """Endring-Mitte (y) von Mesh 22 im Knochenraum: Mittel der Ecken, die nicht auf der Achse liegen (Apex) und in der hinteren
    Haelfte des Glieds liegen."""
    m = mo.meshes[22]
    P = np.array([v[:3] for v in m.tv], float)
    r = np.hypot(P[:, 0], P[:, 2])
    y_ap = P[:, 1].max()
    ring = P[(r > 5) & (P[:, 1] > 0.5 * y_ap)]
    return float(ring[:, 1].mean()) if len(ring) else float(np.linalg.norm(np.array(mo.skel.rel[22], float)))


def kette_punkte(mo, kf, R_T=None):
    """Gelenke 16..22 und Ende (Endring Mesh 22) im Modellraum (Pose kf)."""
    R, T = R_T if R_T is not None else mo.pose(kf)
    P = [T[b].astype(float) for b in SCHWANZ]
    P.append(T[22].astype(float) + R[22].astype(float)[:, 1] / 4096.0 * laenge_22(mo))
    return np.array(P)


b2.kette_punkte = kette_punkte          # fuer v5f_gemein.kette_bild / schwanz_form.passen


def apex_bild(mo, pose):
    """Bildpunkt der Schwanzspitze = Apex von Mesh 22 (Ecke mit groesstem y)."""
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    R, T = mo.pose(kf)
    P = np.array([v[:3] for v in mo.meshes[22].tv], float)
    a = P[np.argmax(P[:, 1])]
    w = (R[22].astype(float) / 4096.0) @ a + T[22].astype(float)
    return b2.projiziere(pose, w[None])[0]


def punkt_zu_maske(p, maske):
    """Abstand (px) eines Bildpunkts zur naechsten Maskenzelle; 0, wenn der Punkt auf der Maske liegt."""
    ys, xs = np.nonzero(maske)
    if not len(xs):
        return float('inf')
    return float(np.sqrt(((xs - p[0]) ** 2 + (ys - p[1]) ** 2).min()))


def krallen_flaechen(m):
    """Schluessel der Krallenflaechen von Mesh 12 (bearbeitbares Mesh-dict): alle Ecken y >= KRALLE_Y."""
    import koerper_bau as KBAU
    P = np.array([v[:3] for v in m['v']], float)
    return [k for (k, _uv), f in zip(KBAU.flaechen_schluessel(m), m['tf'] + m['qf']) if all(P[j, 1] >= KRALLE_Y for j in f[1::2])]
