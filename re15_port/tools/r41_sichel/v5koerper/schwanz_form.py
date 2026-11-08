"""SF V5 wie Foto - Koerper, Teil a) Schwanzform (Nutzer 1/3): Querschnitt je Gelenk auf die Foto-Breite, EIN geschlossenes Stueck.

Die Schwanz-Meshes 16-22 (V4-Generator birkin_formen.kette_meshes_v4, Seiten 11) bestehen je Glied aus Ringen zu 11 Ecken: Anfangsring
(Manschette, Mitte (0, -ueberlapp x r, 0), ueberlapp 0,45), im Glied 18 der Zwischenring 'bauch' (t 0,15, Radius 1,374 x linear,
Mitte 0,011 x Radius nach aussen), Endring (Mitte (0, L, 0)), dazu im Glied 16 der Deckel-Apex (-ue - 0,8 r) und im Glied 22 die
Spitze (L + dorn 0,8 x r). Ringe sind auf die Gehrung der Ruhe-Kette gekippt; die Normalen sind radial (q x 4096) und haengen nicht
vom Radius ab. Diese Stufe aendert NUR Ecken: Ring um seine Mitte skaliert (v' = c + s (v - c)), Manschette und Apex nach derselben
Regel wie der Generator neu gelegt; Flaechen, UV, Normalen, Skelett, Keyframes bytegleich.

Regeln (Nutzer 3 "ein zusammenhaengendes Stueck ... diverse komische Huckel"):
  * Radius je Gelenk j (j = 0 Wurzel im Rumpf, 1 = Beginn, ..., 6 = Knick-Endstueck, 7 = Ende vor der Spitze) monoton fallend von der
    Wurzel zur Spitze: Endring von Glied k und Anfangsring von Glied k + 1 tragen denselben Radius (deckungsgleich in der Ruhe-Kette,
    die Manschette liegt im Vorglied, solange das Vorglied dort nicht duenner ist).
  * Zwischenring 'bauch' im Glied 18: Radius linear zwischen den Gelenken 2 und 3 an seiner Stelle, Mitte auf der Achse (kein Wulst).
  * Radien je Gelenk aus der Foto-Breite (v5f_gemein.foto_breiten, Inkreis-Durchmesser, Median 3) in Pose P2 nachgestellt
    (passen(): Faktor (Soll / Ist)^0,8 je Gelenk an seinem f, danach Monotonie, 12 Durchgaenge).
"""
import os, sys, copy, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v5f_gemein as F

S_ = 11
UEBERLAPP = 0.45
DORN = 0.8
BAUCH_T = 0.15


def ringe(m):
    """Liste der Ringe eines Schwanz-Mesh: (Eckindizes, Mitte, Radius); Apex als Ring mit einer Ecke."""
    P = np.array([v[:3] for v in m['v']], float)
    out = []
    for k in range(0, len(P), S_):
        idx = list(range(k, min(k + S_, len(P))))
        c = P[idx].mean(0)
        out.append((idx, c, float(np.linalg.norm(P[idx] - c, axis=1).mean()) if len(idx) > 1 else 0.0))
    return out


def radien_lesen(meshes):
    """Radien je Gelenk 0..7 und bauch aus den Meshes 16-22."""
    r = {}
    for i in range(7):
        rg = [x for x in ringe(meshes[16 + i]) if len(x[0]) == S_]
        if i == 0:
            r[0] = rg[0][2]
        r[i + 1] = rg[-1][2]
        if len(rg) == 3:
            r['bauch'] = rg[1][2]
    return r


def setzen(meshes, rad):
    """rad = {0..7: Radius, optional 'bauch': dict(t, anteil, aussen)}; setzt die Ringe aller Glieder (Kopie).
    bauch (Glied 18): Lage t (Anteil der Gliedlaenge), Radius = linear + anteil x (r2 - linear) (anteil <= 1: nie dicker als das
    Gelenk davor), Mitte aussen x Radius in der Richtung der Generator-Mitte (nach aussen, weg von der Bogenmitte)."""
    ms = copy.deepcopy(meshes)
    for i in range(7):
        m = ms[16 + i]
        P = np.array([v[:3] for v in m['v']], float)
        rg = ringe(meshes[16 + i])
        voll = [x for x in rg if len(x[0]) == S_]
        r0n, r1n = float(rad[i]), float(rad[i + 1])
        L = voll[-1][1][1]                      # Endring-Mitte y = Gliedlaenge
        for j, (idx, c, r) in enumerate(rg):
            if len(idx) == 1:                   # Apex
                p = P[idx[0]].copy()
                if p[1] < 0:                    # Deckel (Glied 16)
                    p[1] = -UEBERLAPP * r0n - 0.8 * r0n
                else:                           # Spitze (Glied 22)
                    p[1] = L + DORN * r1n
                P[idx[0]] = p
                continue
            if j == 0:                          # Anfangsring (Manschette)
                cn, rn = np.array([0.0, -UEBERLAPP * r0n, 0.0]), r0n
            elif j == len(voll) - 1:            # Endring
                cn, rn = np.array([c[0], L, c[2]]), r1n
            else:                               # bauch
                b = rad.get('bauch') or dict(t=c[1] / L, anteil=0.0, aussen=0.0)
                t = float(b['t'])
                lin = (1.0 - t) * r0n + t * r1n
                rn = lin + float(b['anteil']) * (r0n - lin)
                u = np.array([c[0], 0.0, c[2]]); u = u / max(np.linalg.norm(u), 1e-9)
                cn = np.array([0.0, t * L, 0.0]) + u * float(b['aussen']) * rn
            P[idx] = cn + (P[idx] - c) * (rn / r)
        for k, v in enumerate(m['v']):
            v[:3] = [int(round(x)) for x in P[k]]
    return ms


def monoton(rad):
    out = dict(rad)
    for j in range(1, 8):
        out[j] = min(out[j], out[j - 1])
    return out


def passen(meshes, packen, durchgaenge=12, expo=0.8, log=None):
    """packen(meshes) -> EMD-Blob. Radien 1..7 auf die Foto-Breite (Pose P2), Gelenk 0 mit Gelenk 1 skaliert (>= r1)."""
    soll = F.foto_breiten()
    fs = np.array([r['f'] for r in soll]); ws = np.array([r['soll_H'] for r in soll])
    ws[0] = ws[1]; ws[-1] = ws[-2]               # f 0 / 1: Maskenende (Inkreis = halbe Breite) -> Nachbarwert
    r_alt = radien_lesen(meshes)
    rad = {j: r_alt[j] for j in range(8)}
    rad = monoton(rad)
    P = F.V.posen()['P2']
    verlauf = []
    for it in range(durchgaenge):
        ms = setzen(meshes, rad)
        mo = F.V.modell(packen(ms))
        mb, kp = F.modell_breiten(mo, P)
        fm = np.array([r['f'] for r in mb]); wm = np.array([r['d_H'] for r in mb])
        gf = F.gelenk_f(kp)
        fehler = [abs(wm[k] / ws[k] - 1) for k in range(1, 10)]
        verlauf.append(dict(it=it, radien={str(k): round(v, 1) for k, v in rad.items()}, breiten=[round(x, 4) for x in wm],
                            max_fehler=round(max(fehler), 3)))
        if log:
            print(it, 'max Fehler %.3f' % max(fehler), [round(x, 3) for x in wm], {k: round(v) for k, v in rad.items()}, file=log, flush=True)
        neu = dict(rad)
        for j in range(1, 8):
            f = gf[j - 1]
            ist = float(np.interp(f, fm[1:10], wm[1:10])); s_ = float(np.interp(f, fs[1:10], ws[1:10]))
            neu[j] = rad[j] * (s_ / max(ist, 1e-4)) ** expo
        neu[0] = max(r_alt[0] * neu[1] / r_alt[1], neu[1])
        rad = monoton(neu)
    best = min(verlauf, key=lambda v: v['max_fehler'])
    rb = {int(k): v for k, v in best['radien'].items()}
    return setzen(meshes, rb), dict(radien_alt={str(k): round(v, 1) for k, v in r_alt.items()}, radien_neu={str(k): v for k, v in rb.items()},
                                    soll=[dict(f=r['f'], roh=r['d_H'], soll=r['soll_H']) for r in soll], verlauf=verlauf, gewaehlt=best['it'])


def profil_knicke_geo(meshes):
    """Steigungsknick (Grad) der Mantellinie aus der tatsaechlichen Ringgeometrie: je Glied Abschnitte zwischen aufeinanderfolgenden
    Ringen (Mitte y entlang der Gliedachse, mittlerer Radius); Knick = Winkelaenderung zwischen letztem Abschnitt von Glied k und erstem
    von Glied k + 1 sowie innerhalb eines Glieds (bauch)."""
    seg = []
    for i in range(7):
        voll = [x for x in ringe(meshes[16 + i]) if len(x[0]) == S_]
        for a, b in zip(voll[:-1], voll[1:]):
            seg.append((16 + i, float(b[1][1] - a[1][1]), a[2], b[2]))
    w = [np.degrees(np.arctan2(e - a, L)) for _g, L, a, e in seg]
    return [dict(zwischen='Glied %d / %d' % (seg[k][0], seg[k + 1][0]), knick=round(float(abs(w[k + 1] - w[k])), 2)) for k in range(len(seg) - 1)]
