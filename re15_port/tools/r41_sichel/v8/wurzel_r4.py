"""SF V8 Runde 4 (Befund S10/O5 der Pruefung V8 r3): Wurzel von hinten - Stufe w (v8_bau) und Hilfen fuer die Rueckensuche.

Befund (Zahlen-Pruefer r3, echte Framedumps mit Marker-Assets): "In den echten Framedumps 46 090 px (... z-Test des Port-Renderers), je Bild
etwa 3100-3250 px in F328-F340". Sicht-Pruefer r3 (5): "Ein dunkelgrauer Keil mit harten Kanten und einer Luecke zur Haut".
Gemessen (Runde 4, sfv8/r4): der Port zeichnet mit Maler-Reihenfolge (render_pc.c: "SDL_RenderGeometry has no depth buffer, so painter's
draw order decides"; f_rast ebenso). Die Flaechen des Wurzelrohrs (Mesh 16 Ring 0 -> Ring 1, 1,5 r0 = 708 E. lang) und des Kegels liegen
zum Teil IN dem Rumpf, ihre Dreiecks-Mitte liegt aber naeher an der Kamera als die grossen Rueckenflaechen davor - sie werden UEBER die
Haut gemalt. Einbetten allein hilft deshalb nicht (Probe sfv8/r4 H1: alle Wurzel-Ecken >= 20 E. unter der Haut, trotzdem 12 636 sichtbare
Wurzel-Bildpunkte in Takt F314-F345 offline, V8 r3 20 471).
Regel Stufe w (PORT-WAHL; Nutzer: "Tail directly smooth into the back - no hill or anything...", frueher "vom rücken aus geschlossen ein
zusammenhängendes stück Das direkt I. den Rücken übergeht"):
  1. Jede Ecke von Ring 1 (Wurzelring) gleitet entlang der Gliedachse (y im Raum von Knochen 16), bis sie WURZEL_TIEF unter der Haut von
     Mesh 8 liegt (Halbierung im Bereich GLEIT_BEREICH) - Ring 1 wird zur Schnittlinie Schwanz/Haut. Ecken, deren Achsenlinie die Haut im
     Bereich nicht schneidet (oben am Bogen: das Rohr laeuft dort ueber den Ruecken), bleiben bei y 0; die Rueckensuche (ruecken_r4.py)
     hebt die Haut darueber (Kosten "Wurzel bedeckt"). Diese Ecken ruecken vorher radial, bis sie WURZEL_TIEF unter der Haut liegen
     (Einzug bis ZUG_MIN, Aufweitung bis ZUG_MAX x Radius): Ring 1 folgt der Haut von unten. Probe sfv8/r4: festes 0,85/0,9 (nur Einzug)
     senkte die verdeckte Schwanz-Oberkante unter die steigende Haut (Riegel S2c 5-6 px); Gleiten entlang der Achse nach hinten (-100 bis
     -600 E.) verschlechterte S2b/S2c (3,7-5,9 / 4-8).
  2. Ring 0 = Ring 1, Kegelspitze = Mitte von Ring 1: Wurzelrohr und Kegel haben keine Flaeche mehr (nichts kann ueber die Haut gemalt werden).
  Knochen 16 liegt in jedem Keyframe auf seiner Ruhedrehung gegen Knochen 8 (Stufe s) und im Spiel ausserhalb der Szenenfuehrung h19-h74 auf
  dem Keyframe (schwanz_r41.c SW_V8_STARR; gemessen sfv8/szene_v8r3/debug.log: Glied 0->1 gegen das Ziel 0,0-0,1 Grad in Takt F318-F360).
Flaechen, UV, Zeile 487 und Eckenzahl bleiben. Kein RE-Wert.
"""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402

WURZEL_TIEF = 15.0                    # E. unter der Haut (1 px in PV bei 65,4 px je 1000 E.)
GLEIT_BEREICH = (-900.0, 600.0)       # E. entlang der Gliedachse (Glied 541 E., Rohr 708 E.)
ZUG_MAX = 1.40                        # aufweiten hoechstens auf 1,4 r0 (Probe sfv8/r4: Ring-1-Oberkante folgt der steigenden Haut; Riegel S2c)
ZUG_MIN = 0.70                        # Ecken ohne Schnitt: hoechstens radial auf 0,70 r0 einziehen (Riegel S4 Wurzel >= 55 px; feste 0,85
                                      # ergaben 55,5 px, Lauf sfv8/r4 rs4c). Grund (Lauf rs4a): die ueber Ring 1 gehobene Haut stand in PV bei
                                      # x 364-374 3-4 px ueber der Vorlage (Riegel S2b 2,8); mit Einzug folgt Ring 1 der Haut, nicht umgekehrt.
KEGEL = -150.0                        # Kegelspitze 150 E. VOR der Mitte von Ring 1 (Gliedachse, in den Schwanz hinein; Probe sfv8/r4: 0 -> 18 px,
                                      # +100..+350 (in den Rumpf) 64-198 px, -150 -> 1 px sichtbare Wurzel in Takt F314-F345)
KEGEL_UMKEHR = False                  # Runde 5 (Zahlen-Pruefer r4 S2c "8,0 px bei x 376 ... in allen 7 Halbpixel-Varianten 7-8 px"; gemessen sfv8/r5
                                      # j2.py: mit Rueckseiten-Culling war das Wurzelende des Schwanzes OFFEN - die umgekehrt gewundene Kappe ist in PV eine
                                      # Rueckseite, die verdeckte Schwanz-Oberkante fiel bei x 372-376 um 4 px ab; Runde 4 KEGEL_UMKEHR = True gegen einen
                                      # 1-px-Splitter in Takt F323, den jetzt die Rueckenecken 38/41 (ruecken_v8.json Runde 5) zudecken). Geschlossen:
                                      # S2c 8 -> 4 px (alle 7 Varianten 3-4), Riegel S10 0.
S_ = 11


def tris_von(m):
    T = []
    for i, f in enumerate(m['tf']):
        T.append(('d%d' % i, list(f[1::2])))
    for i, f in enumerate(m['qf']):
        e = f[1::2]
        T.append(('v%d' % i, [e[0], e[1], e[3]])); T.append(('v%d' % i, [e[0], e[3], e[2]]))
    return T


def naechste(p, P, T):
    """Naechster Punkt auf den Dreiecken T (Ecken P): (Abstand, +1 innen / -1 aussen, Dreieck, Gewichte, Normale nach innen).
    Die Flaechen von Mesh 8 sind so gewunden, dass (b - a) x (c - a) nach innen zeigt (r3_gemein.abstand_innen). Vektorisiert ueber alle
    Dreiecke: Lotfusspunkt in der Ebene, falls im Dreieck, sonst der naechste Punkt der drei Kanten."""
    F = np.array([f for _k, f in T])
    a, b, c = P[F[:, 0]], P[F[:, 1]], P[F[:, 2]]
    n = np.cross(b - a, c - a)
    nn = np.linalg.norm(n, axis=1)
    ok = nn > 1e-9
    n = n / np.where(ok, nn, 1.0)[:, None]
    d = ((p - a) * n).sum(1)
    q = p - d[:, None] * n
    v0, v1, v2 = b - a, c - a, q - a
    d00 = (v0 * v0).sum(1); d01 = (v0 * v1).sum(1); d11 = (v1 * v1).sum(1); d20 = (v2 * v0).sum(1); d21 = (v2 * v1).sum(1)
    den = d00 * d11 - d01 * d01 + 1e-12
    wv = (d11 * d20 - d01 * d21) / den; ww = (d00 * d21 - d01 * d20) / den; wu = 1 - wv - ww
    innen = (wu >= 0) & (wv >= 0) & (ww >= 0)
    best_d = np.where(innen, np.abs(d), np.inf)
    best_w = np.stack([wu, wv, ww], 1)
    best_q = q.copy()
    for (i0, i1), (s, e) in (((0, 1), (a, b)), ((1, 2), (b, c)), ((2, 0), (c, a))):
        t = np.clip(((p - s) * (e - s)).sum(1) / np.maximum(((e - s) * (e - s)).sum(1), 1e-12), 0, 1)
        qe = s + t[:, None] * (e - s)
        de = np.linalg.norm(p - qe, axis=1)
        bes = de < best_d
        best_d = np.where(bes, de, best_d)
        we = np.zeros((len(F), 3)); we[:, i0] = 1 - t; we[:, i1] = t
        best_w = np.where(bes[:, None], we, best_w)
        best_q = np.where(bes[:, None], qe, best_q)
    best_d = np.where(ok, best_d, np.inf)
    ti = int(np.argmin(best_d))
    sg = 1.0 if (p - best_q[ti]) @ n[ti] > 0 else -1.0
    return float(best_d[ti]), sg, ti, best_w[ti], n[ti]


def tiefe(q, P8, T8):
    d, s, _ti, _w, _n = naechste(q, P8, T8)
    return d * s


def b16_zu_b8(mo):
    """Knochenraum 16 -> 8 (Ruhe; in jedem Keyframe gleich, Stufe s)."""
    import v8_bau as VB
    R, T = mo.pose(VB.BB_kf(mo, V.PV))
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    R16, T16 = R[16].astype(float) / 4096.0, T[16].astype(float)
    return R8.T @ R16, R8.T @ (T16 - T8)


def ring1_b8(mo, ms):
    A, t = b16_zu_b8(mo)
    P = np.array([v[:3] for v in ms[16]['v']], float)
    return P[S_:2 * S_] @ A.T + t


def stufe_w(blob, ms, prot, tief=WURZEL_TIEF, ring_versatz=None):
    """ring_versatz (Runde 5, kette_v8.json "ring1_versatz"): {Ecke 11-21: (x, y, z) E. im Raum von Knochen 16} nach dem Gleiten/Einziehen,
    vor Ring 0 = Ring 1 und der Kegelspitze."""
    A, t = b16_zu_b8(V.modell(blob))
    m8 = ms[8]; P8 = np.array([v[:3] for v in m8['v']], float); T8 = tris_von(m8)
    m = ms[16]
    P = np.array([v[:3] for v in m['v']], float)
    neu = P.copy(); liste = []
    for i in range(S_, 2 * S_):
        f = lambda y: tiefe(np.array([P[i][0], y, P[i][2]]) @ A.T + t, P8, T8)
        a, b = GLEIT_BEREICH
        if not (f(a) > tief > f(b)) or f(0.0) < tief:
            # (auch: die Ecke liegt bei y 0 ausserhalb - Gleiten nach hinten verlaengert das verdeckte Ende nach rechts in PV, Riegel S2c)
            # radial im Raum von Knochen 16 (Ringmitte (0, 0, 0)), bis die Ecke `tief` unter der Haut liegt: einziehen (hoechstens ZUG_MIN) oder
            # aufweiten (hoechstens ZUG_MAX) - Ring 1 folgt der Haut von unten (Schwanzansatz weitet sich in den Ruecken)
            g = lambda z: tiefe(np.array([P[i][0] * z, 0.0, P[i][2] * z]) @ A.T + t, P8, T8)
            if g(1.0) >= tief:
                lo_, hi_ = 1.0, ZUG_MAX
                if g(hi_) >= tief:
                    z = hi_
                else:
                    for _ in range(24):
                        c = 0.5 * (lo_ + hi_)
                        if g(c) >= tief:
                            lo_ = c
                        else:
                            hi_ = c
                    z = lo_
            elif g(ZUG_MIN) < tief:
                z = ZUG_MIN
            else:
                lo_, hi_ = ZUG_MIN, 1.0
                for _ in range(24):
                    c = 0.5 * (lo_ + hi_)
                    if g(c) >= tief:
                        lo_ = c
                    else:
                        hi_ = c
                z = lo_
            neu[i][0] *= z; neu[i][2] *= z
            liste.append(dict(ecke=i, y=int(round(neu[i][1])), schnitt=False, zug=round(z, 3)))
            continue
        for _ in range(30):
            c = 0.5 * (a + b)
            if f(c) > tief:
                a = c
            else:
                b = c
        neu[i][1] = 0.5 * (a + b)
        liste.append(dict(ecke=i, y=int(round(neu[i][1])), schnitt=True))
    for k, dv in sorted((ring_versatz or {}).items(), key=lambda kv: int(kv[0])):
        neu[int(k)] = neu[int(k)] + np.asarray(dv, float)
        liste.append(dict(ecke=int(k), versatz_r5=[float(x) for x in dv]))
    for i in range(S_):
        neu[i] = neu[i + S_]
    kegel = [i for i in range(3 * S_, len(P)) if P[i][1] < 0]
    for i in kegel:
        neu[i] = neu[S_:2 * S_].mean(0) - np.array([0.0, KEGEL, 0.0])
    for i in list(range(2 * S_)) + kegel:
        m['v'][i][:3] = [int(round(x)) for x in neu[i]]
    if KEGEL_UMKEHR:
        # Kegel-Dreiecke (eine Ecke = Kegelspitze) umgekehrt gewunden: mit der Spitze IM Schwanz zeigen sie sonst schraeg nach aussen und ein
        # Splitter an der Oberkante von Ring 1 blieb sichtbar (Probe sfv8/r4: 1 px in Takt F323); umgekehrt sind sie von aussen Rueckseiten
        for j, f in enumerate(m['tf']):
            if set(f[1::2]) & set(kegel):
                m['tf'][j] = [f[2], f[3], f[0], f[1], f[4], f[5]]
                uv = m['tuv'][j]
                m['tuv'][j] = [uv[3], uv[4], uv[2], uv[0], uv[1], uv[5], uv[6], uv[7], uv[8]]
    prot['wurzel_w'] = dict(wurzel_tief=tief, ring1=liste, kegel=kegel)


def bedeckung(blob, ms, tief=WURZEL_TIEF):
    """Je Ecke von Ring 1: Tiefe unter der Haut von Mesh 8 (E., + innen). Kosten der Rueckensuche: Summe (tief - Tiefe)+."""
    mo = V.modell(blob)
    m8 = ms[8]; P8 = np.array([v[:3] for v in m8['v']], float); T8 = tris_von(m8)
    return [round(tiefe(q, P8, T8), 1) for q in ring1_b8(mo, ms)]
