"""SF V8 Runde 5 Stufe n) Einsatz an Mesh 8 gegen den Keil in Szenentakt F332 (P5, Messblatt V8 Abschnitt 11 "Runde 5d").

Nutzer 2026-11-09: "es gibt immer noch eine Lücke zwischen Rücken und Kopf beim 2 beiner Birkin. Das ist in der Vorlage so wie es
aussieht nicht so." Auftrag Runde 5 (iii): "statt des duennen Kragens ein kleines, geschlossenes Nacken-Volumen an Mesh 8 ..., mit
Original-EM30-Haut (vorhandene Texel, unverzerrt), im Nacken und nicht vor dem Gesicht."

Was das ist: ein geschlossener Koerper aus 14 Ecken (Pol oben, zwei Ringe zu je 6 Ecken, Pol unten), 12 Dreiecken und 6 Vierecken,
angehaengt an Mesh 8 (Knochen 8); die bisherigen Ecken, Flaechen und ihre Nummern bleiben. Ort und Groesse stehen in nacken_v8.json und
sind PORT-WAHL aus einer Messung (kein RE-Wert, die Form gibt es auf keiner Disc):
  - Der Keil in F332 (Suchlauf i: 108 px, 72 tiefer als 1,5 px, groesste Tiefe 6,3 px) liegt NICHT am Kopfgelenk (111, -971, -133),
    sondern zwischen der Unterkante der Rueckenflaechen 8 v24 / 8 v4 und der Kopfunterseite ueber der (W)-rechten Schulter. Rueckprojektion
    der Keil-Bildpunkte in den Raum von Knochen 8 (Raster 70 E., nur Punkte <= 300 E. vom Mesh; sfv8/r5p6/scan2.py): ein Koerper mit
    Halbmesser 80 um (350, -860, -410) schliesst F332 auf 2 px / 0 tief / 1,0 px, ohne eine der 16 Probe-Ansichten zu aendern.
  - Kleinster Koerper, der F332 schliesst und in PV / P1 / P2 am wenigsten zu sehen ist (sfv8/r5p6/scan4.py, 108 Lagen, Halbpixel-Lage 0):
    Halbmesser 50 um (350, -860, -370) - F332 4 px / 0 / 1,0 px; sichtbar PV 32, P1 46, P2 43 Bildpunkte (Halbmesser 80: 93 / 113 / 80).
    In allen 7 Halbpixel-Lagen des Riegels (sfv8/r5p6/scan5.py, 144 Lagen) haelt Halbmesser 50 dort NICHT (F332 81 px / 46 tief / 6,1 px
    in 3 der 7 Lagen: der Koerper ist dann nur etwa 3 px breit und die Zeichenreihenfolge kippt); Halbmesser 65 um (350, -860, -370) haelt
    (6 px / 1 tief / 2,0 px), wird aber in PV ueber 4 und in P2 ueber 7 Gesichts-Bildpunkte von V7 gezeichnet (Unterkiefer).
  - Gesetzt (sfv8/r5p6/scan6.py, 450 Lagen, je Lage F332 in 7 Halbpixel-Lagen und 19 Ansichten gegen das Gesicht von V7): Halbmesser 65 um
    (350, -820, -370) - F332 hoechstens 8 px / 1 tief / 2,0 px, in keiner Ansicht ueber dem Gesicht (Riegel S16 (c), alle 177 Ansichten),
    sichtbar PV 59, P1 67, P2 59 Bildpunkte; die Nachbarlagen (x 390; y -860; y -780 mit Halbmesser 80) schliessen F332 ebenfalls.
  Der Einsatz sitzt damit rund 350 E. vom Kopfgelenk entfernt unter dem Unterkiefer ueber der (W)-rechten Schulter, NICHT im Nacken: dort
  liegt der Keil von F332. In PV / P1 / P2 ist er als kleiner Knoten unter dem Kiefer zu sehen (Tafel 15).
Haut: jede neue Flaeche liegt auf der CLUT-Zeile und Texturseite der Wirtsflaeche (nacken_v8.json "wirt", eine Hautflaeche von Mesh 8
direkt neben dem Einsatz) und bekommt eine UV, die ihre Form winkeltreu abbildet (ebene Abwicklung der Flaeche, ein Massstab = Median
Texel je Einheit der Dreiecke von Mesh 8), mit dem Schwerpunkt auf dem Inkreis-Mittelpunkt der Wirts-UV. Die Texel unter jeder neuen
Flaeche liegen damit innerhalb der Wirtsflaeche (assert unten; Riegel S7a prueft Original-Texel und Original-Nutzung).
Der Port zeichnet ohne Tiefenpuffer nach mittlerer Dreieckstiefe: Riegel S16 (b) und (c) zaehlen in allen 177 Ansichten, ob Mesh 8 bzw.
der Einsatz ueber Gesichts-Bildpunkten von V7 gezeichnet wird.
"""
import numpy as np
import kopf_orig_bau as KB            # noqa: E402

SEGMENTE = 6            # Ecken je Ring (wie die Schwanzglieder der Form: wenige, grosse Flaechen); PORT-WAHL
BREITE_GRAD = 35.0      # Breitenkreis der beiden Ringe; PORT-WAHL (aehnlich grosse Flaechen bei 6 Segmenten)


def koerper(mitte, radius):
    """-> (Ecken, Normalen, Dreiecke, Vierecke); Umlaufsinn wie Mesh 8 (gemessen: (v1 - v0) x (v2 - v0) zeigt GEGEN die Eckennormalen,
    77 von 77 Dreiecken und 27 von 27 Vierecken in beiden Haelften 0,1,2 und 1,3,2)."""
    c, r = np.array(mitte, float), np.array(radius, float)
    b = np.radians(BREITE_GRAD)
    P = [c + r * [0, -1, 0]]
    for s in (-1, 1):
        for k in range(SEGMENTE):
            ph = 2 * np.pi * k / SEGMENTE
            P.append(c + r * [np.cos(b) * np.cos(ph), s * np.sin(b), np.cos(b) * np.sin(ph)])
    P.append(c + r * [0, 1, 0])
    P = np.array(P)
    N = (P - c) / r ** 2
    N /= np.linalg.norm(N, axis=1)[:, None]
    unten = 1 + 2 * SEGMENTE
    T, Q = [], []
    for k in range(SEGMENTE):
        a, b_ = 1 + k, 1 + (k + 1) % SEGMENTE
        for f in ([0, a, b_], [unten, SEGMENTE + b_, SEGMENTE + a]):
            g = np.cross(P[f[1]] - P[f[0]], P[f[2]] - P[f[0]])
            T.append(f if g @ N[f[0]] < 0 else [f[0], f[2], f[1]])
        f = [a, SEGMENTE + a, b_, SEGMENTE + b_]
        g = np.cross(P[f[1]] - P[f[0]], P[f[2]] - P[f[0]])
        Q.append(f if g @ N[f[0]] < 0 else [f[1], f[0], f[3], f[2]])
    return P, N, T, Q


def dichte(m):
    """Median Texel je Einheit ueber die Dreiecke von Mesh 8 (Wurzel aus UV-Flaeche / Raumflaeche)."""
    P = np.array([v[:3] for v in m['v']], float)
    out = []
    for f, uv in zip(m['tf'], m['tuv']):
        a, b, c = P[f[1]], P[f[3]], P[f[5]]
        fr = 0.5 * np.linalg.norm(np.cross(b - a, c - a))
        fu = 0.5 * abs((uv[3] - uv[0]) * (uv[7] - uv[1]) - (uv[4] - uv[1]) * (uv[6] - uv[0]))
        if fr > 0 and fu > 0:
            out.append(np.sqrt(fu / fr))
    return float(np.median(out))


def inkreis(uv):
    A = np.array([[uv[0], uv[1]], [uv[3], uv[4]], [uv[6], uv[7]]], float)
    la = np.array([np.linalg.norm(A[1] - A[2]), np.linalg.norm(A[0] - A[2]), np.linalg.norm(A[0] - A[1])])
    return (la[:, None] * A).sum(0) / la.sum()


def abwicklung(Q, s, mitte):
    """Ebene, winkeltreue Abwicklung der Flaeche (Ecken Q im Raum) mit Massstab s, Schwerpunkt auf mitte, auf ganze Texel gerundet."""
    e1 = Q[1] - Q[0]
    x = e1 / np.linalg.norm(e1)
    n = np.cross(e1, Q[2] - Q[0])
    y = np.cross(n / np.linalg.norm(n), x)
    E = np.array([[(q - Q[0]) @ x, (q - Q[0]) @ y] for q in Q]) * s
    E += np.asarray(mitte, float) - E.mean(0)
    return [[int(round(u)), int(round(v))] for u, v in E]


def anwenden(ms, t, par, prot):
    m = ms[8]
    P, N, T, Q = koerper(par['mitte'], par['radius'])
    assert par['wirt'][0] == 'd'
    wirt = m['tuv'][int(par['wirt'][1:])]
    s = dichte(m)
    mi = inkreis(wirt)
    po = (wirt[5] & 0xF) * 128
    maske = KB.raster(wirt, 3, po)
    v0, n0 = len(m['v']), len(m['n'])
    for p, n in zip(P, N):
        m['v'].append([int(round(x)) for x in p] + [0])
        m['n'].append([int(round(x * 4096)) for x in n] + [0])
    Pg = np.array([v[:3] for v in m['v'][v0:]], float)
    uvs = []
    for f in T:
        e = abwicklung(Pg[f], s, mi)
        uv = [e[0][0], e[0][1], wirt[2], e[1][0], e[1][1], wirt[5], e[2][0], e[2][1], 0]
        assert not (KB.raster(uv, 3, po) & ~maske).any(), ('Texel ausserhalb der Wirtsflaeche', f, uv)
        m['tf'].append([n0 + f[0], v0 + f[0], n0 + f[1], v0 + f[1], n0 + f[2], v0 + f[2]])
        m['tuv'].append(uv)
        uvs.append(uv)
    for f in Q:
        e = abwicklung(Pg[f], s, mi)
        uv = [e[0][0], e[0][1], wirt[2], e[1][0], e[1][1], wirt[5], e[2][0], e[2][1], 0, e[3][0], e[3][1], 0]
        assert not (KB.raster(uv, 4, po) & ~maske).any(), ('Texel ausserhalb der Wirtsflaeche', f, uv)
        m['qf'].append([x for i in f for x in (n0 + i, v0 + i)])
        m['quv'].append(uv)
        uvs.append(uv)
    prot['nacken'] = dict(mitte=list(par['mitte']), radius=list(par['radius']), ecken=len(P), dreiecke=len(T), vierecke=len(Q),
                          erste_ecke=v0, wirt=par['wirt'], wirt_uv=list(wirt), zeile=(wirt[2] >> 6) & 0x1FF,
                          texel_je_einheit=round(s, 5), uv=uvs, quelle=par.get('quelle'))
