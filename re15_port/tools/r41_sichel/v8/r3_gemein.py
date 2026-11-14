"""SF V8 Runde 3 (Bauer, 2026-11-09): gemeinsame Helfer fuer die Vorgaben O1-O8 des Orchestrators nach Pruefung V8 r2.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante.
Ansichten (alle Spielkamera 50A0 Cut 12, b2.raum; die Szene laeuft durchgehend in Cut 12, state.log "cam=12"):
  PV     Clip 17 Bild 47 Gier 587 (Messblatt V8 Abschnitt 1, Vorlage n1439)
  P1     Clip 20 Bild 139 Gier 1422 x -8420 z -21908 (Foto 1 = Beschreibung1_Vorlage = n1485, Messblatt Foto)
  P2     Clip 12 Bild 48 Gier 861 x -8182 z -21330 (Foto 2 = Beschreibung2_Vorlage = n1435)
  SZENE  Birkin-Posen der freigegebenen Hangar-Szene aus "[hangar-takt] hN FN B clip C bild B gier G (x,z)" (sfv4/r/V7_szene/debug.log,
         gleich in sfv8/szene_v8r2: 144 Zeilen ohne Unterschied); gesichert in szene_posen_r3.json. Von vorn: Takt F344-F364 (Clip 1, Gier
         1515; Sicht-Pruefer r2 "F350/F360/F370, die lange Phase, in der Birkin frontal zur Kamera steht"), von hinten: Takt F320-F332
         (Clip 11 Bild 56-68, Gier 2494 -> 1593; Sicht-Pruefer r2 "von hinten F328-F346"). Die Framedump-Nummer liegt etwa 6 ueber der
         Takt-Nummer (Sichtvergleich offline Takt F328 = Framedump f000334, Tafel 08/10).
"""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402

P1 = dict(clip=20, bild=139, gier=1422, x=-8420, z=-21908)      # Messblatt Foto (v6_gemein.posen P1)
P2 = dict(clip=12, bild=48, gier=861, x=-8182, z=-21330)        # Messblatt Foto (v6_gemein.posen P2)
VORN_TAKTE = (344, 354, 364)          # Szene von vorn (Clip 1 Bild 11/21/31, Gier 1515)
HINTEN_TAKTE = (320, 322, 324, 326, 328, 330, 332)   # Szene von hinten (Clip 11 Bild 56-68)


def szene_posen():
    return {int(k): v for k, v in json.load(open(os.path.join(HIER, 'szene_posen_r3.json')))['posen'].items()}


def oben(maske, x0, x1):
    """Oberkontur (kleinstes y je Spalte) einer Maske, x0..x1 einschliesslich."""
    o = {}
    for x in range(x0, x1 + 1):
        ys = np.nonzero(maske[:, x])[0]
        if len(ys):
            o[x] = int(ys.min())
    return o


def spitzen(o, w=4, h=2.5):
    """Konvexe Ecken der Oberkontur: Spalten, deren Punkt um >= h px ueber dem Mittel der Nachbarn bei +-w liegt."""
    out = []
    for x in sorted(o):
        if x - w in o and x + w in o:
            if 0.5 * (o[x - w] + o[x + w]) - o[x] >= h:
                out.append(x)
    return out


def dreiecke(m):
    P = np.array([v[:3] for v in m['v']], float)
    T = [P[list(f[1::2])] for f in m['tf']]
    for f in m['qf']:
        i = f[1::2]
        T.append(P[[i[0], i[1], i[3]]]); T.append(P[[i[0], i[3], i[2]]])
    return np.array(T)


def abstand_innen(P, T):
    """Je Punkt (Knochenraum des Meshes): (Abstand zur naechsten Flaeche, +1 innen / -1 aussen). Die Flaechen von Mesh 8 sind so
    gewunden, dass (b - a) x (c - a) nach innen zeigt (gemessen: Mittel des Vorzeichens gegen den Schwerpunkt -1,0 = alle)."""
    P = np.atleast_2d(np.asarray(P, float))
    a, b, c = T[:, 0], T[:, 1], T[:, 2]
    n = np.cross(b - a, c - a)
    n /= np.linalg.norm(n, axis=1)[:, None] + 1e-12
    out = []
    for p in P:
        d = ((p - a) * n).sum(1)
        q = p - d[:, None] * n
        v0, v1, v2 = b - a, c - a, q - a
        d00 = (v0 * v0).sum(1); d01 = (v0 * v1).sum(1); d11 = (v1 * v1).sum(1); d20 = (v2 * v0).sum(1); d21 = (v2 * v1).sum(1)
        den = d00 * d11 - d01 * d01 + 1e-12
        vv = (d11 * d20 - d01 * d21) / den; ww = (d00 * d21 - d01 * d20) / den; uu = 1 - vv - ww
        innen = (uu >= 0) & (vv >= 0) & (ww >= 0)
        dist = np.where(innen, np.abs(d), np.inf)
        for s, e in ((a, b), (b, c), (c, a)):
            t = np.clip(((p - s) * (e - s)).sum(1) / ((e - s) * (e - s)).sum(1), 0, 1)
            dist = np.minimum(dist, np.linalg.norm(p - (s + t[:, None] * (e - s)), axis=1))
        i = int(np.argmin(dist))
        out.append((float(dist[i]), 1.0 if d[i] > 0 else -1.0))
    return out


def in_knochen8(mo, b, P, kf):
    """Punkte im Raum des Knochens b -> Raum von Knochen 8 (Pose kf)."""
    R, T = mo.pose(kf)
    Rb, Tb = R[b].astype(float) / 4096.0, T[b].astype(float)
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    return ((np.atleast_2d(P) @ Rb.T + Tb) - T8) @ R8
