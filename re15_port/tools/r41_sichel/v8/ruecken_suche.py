"""SF V8: Suche der Rueckenecken (Mesh 8) gegen die Oberkontur der Vorlage in PV -> ruecken_v8.json (Messblatt V8 6b).

    python ruecken_suche.py [--basis EMD] [--kette JSON] [--aus JSON]
        --kette: Rumpf MIT dem V8-Schwanz (v8_bau Stufe s aus kette_v8.json) - Kontur = Oberkontur Schwanz u Rumpf (Runde 2 der Suche);
                 ohne --kette nur der Rumpf (Runde 1)

Verfahren (deterministisch): Kandidaten = Ecken von Mesh 8, deren Bildpunkt in PV bei x 368-450 hoechstens 14 px unter der Oberkontur
des Rumpfs (Knochen 0-15) liegt. Jede Ecke bewegt sich nur senkrecht zum Sehstrahl in die Richtung, die ihren Bildpunkt um 1 px nach
oben schiebt (kleinste Bewegung, Jacobi der Projektion), Betrag a_i px. Ziel (Messblatt 3.2 / Riegel S2a, S3): Oberkontur des Rumpfs
x 372-445 = Oberkontur der Vorlage-Figurmaske (m13b_oben.json oben_vorlage_maske); Kosten = Mittel |d| + 0,5 Max |d| + 0,02 Summe a_i^2
(kleine Verschiebungen); |a_i| <= 10 px. Mit --kette ausserdem (Riegel S2b/S3, Nutzer "Little red hill from the back",
"smooth straight to head without further hills"): 2 x (5,5 - H)+ + 2 x (H - 9,5)+ (Huegel H ueber der Sehne x 405 -> 447 bei x 428-445,
sonst H = 0), 2 x (Sattel x 300-420 - 1,5)+, 2 x (Sattel x 380-447 - 1,5)+, 2 x (Kuppe - 3,5)+, Kontur x 330-445. Koordinatensuche (die Kontur ist pixelgenau, Powell bleibt am Start haengen - Lauf 1:
Kosten 8,37 -> 8,10): je Ecke a_i aus -10..10 px in 1-px-Schritten, beste Wahl, 4 Runden in fester Reihenfolge, Start 0.
Ergebnis = ganzzahlige Verschiebung je Ecke (Modelleinheiten).
Kein RE-Wert, PORT-WAHL; die Zahlen sind das Suchergebnis.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import kopf_orig_bau as KB   # noqa: E402
import ruecken_v8 as RV8     # noqa: E402

X0, X1 = 372, 445
A_MAX = float(os.environ.get('V8_A_MAX', '10'))   # Runde 2: per Umgebung weiter (Lauf sfv8/r2 rs2: 15)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=V.ASSET)
    ap.add_argument('--aus', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--kette')
    ap.add_argument('--start', help='ruecken-JSON einer frueheren Runde: deren Verschiebungen sind die Basis (fest), gesucht wird darauf')
    ap.add_argument('--x-min', type=int, default=None, help='nur Kandidaten mit Bild-x >= x-min')
    ap.add_argument('--x-max', type=int, default=450, help='nur Kandidaten mit Bild-x <= x-max')
    ap.add_argument('--riegel', action='store_true', help='Runde 2: Kosten = Riegel S2/S3 (riegel_v8) auf der Silhouette mit Schwanz')
    a = ap.parse_args()
    blob = open(a.basis, 'rb').read()
    sha_basis = V.sha(blob)
    if a.kette:
        import v8_bau as VB
        blob, _p = VB.bauen(a.basis, 's', kette=json.load(open(a.kette)))
    import riegel_v8 as RG
    d, _o, ms, tim = KB.teile(blob)
    if a.start:
        RV8.ruecken(ms, json.load(open(a.start)), {})
        blob = KB.packen(blob, d, ms, tim)
        d, _o, ms, tim = KB.teile(blob)
    mo = V.modell(blob)
    R, T = V.pose_r_t(mo)
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    ov = {int(k): v for k, v in V.lade('m13b_oben.json')['oben_vorlage_maske'].items()}
    m8 = ms[8]
    P = np.array([v[:3] for v in m8['v']], float)
    q = V.proj(V.PV, P @ R8.T + T8)
    rumpf = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16)))
    top = {x: int(np.nonzero(rumpf[:, x])[0].min()) for x in range(300, 471) if rumpf[:, x].any()}
    xmin = a.x_min if a.x_min is not None else (330 if a.kette else 368)
    kand = [i for i, (x, y) in enumerate(q) if xmin <= round(x) <= a.x_max and y <= top.get(int(round(x)), 999) + 14]
    J = {}
    for i in kand:
        Jm = np.zeros((2, 3))
        for k in range(3):
            e = np.zeros(3); e[k] = 5.0
            Jm[:, k] = (V.proj(V.PV, (P[i] + e) @ R8.T + T8)[0] - V.proj(V.PV, (P[i] - e) @ R8.T + T8)[0]) / 10.0
        J[i] = Jm.T @ np.linalg.solve(Jm @ Jm.T, np.array([0.0, -1.0]))
    xs = np.arange(330 if a.kette else X0, X1 + 1)
    ziel = np.array([ov[x] for x in xs], float)

    def form(av):
        ms2 = [dict(m) for m in ms]
        v = [list(x) for x in m8['v']]
        for i, ai in zip(kand, av):
            v[i][:3] = [int(round(P[i][k] + ai * J[i][k])) for k in range(3)]
        ms2[8] = dict(m8, v=v)
        return ms2

    def kontur(av):
        mo2 = V.modell(KB.packen(blob, d, form(av), tim))
        r = V.b2.umriss_schnell(mo2, V.PV, knochen=list(range(23 if a.kette else 16)))
        return np.array([np.nonzero(r[:, x])[0].min() if r[:, x].any() else 999 for x in xs], float)

    def form_masse(k):
        om = {int(x_): int(y_) for x_, y_ in zip(xs, k)}
        H, hx = RG.huegel(om, 405, 447) if all(x_ in om for x_ in (405, 447)) else (0.0, 0)
        if not 428 <= hx <= 445:
            H = 0.0
        hk = RG.huckel({x_: om[x_] for x_ in range(385, 426) if x_ in om}, prom=0, w=20)
        kp = max([h['prom'] for h in hk], default=0.0)
        return H, RG.sattel(om, 330, 420)[0], RG.sattel(om, 380, 447)[0], kp

    # Runde 2 (Lauf sfv8/r2 rs2: ohne Formziel entstand bei x 429/430 eine Stufe 187 -> 192 statt eines Huegels): Ziel der Oberkontur
    # x KX0-KX1 = die Kuppel der Vorlage ohne Dellen = obere Huelle (konkave Majorante) der Vorlage-Oberkontur (m13b_oben.json) -
    # Nutzer "Little red hill from the back", "smooth straight to head without further hills...."
    KX0, KX1 = 400, 447
    _xs = [x_ for x_ in range(KX0, KX1 + 1) if x_ in ov]
    _H = []
    for p_ in sorted((x_, -ov[x_]) for x_ in _xs):
        while len(_H) >= 2 and (_H[-1][0] - _H[-2][0]) * (p_[1] - _H[-2][1]) - (_H[-1][1] - _H[-2][1]) * (p_[0] - _H[-2][0]) >= 0:
            _H.pop()
        _H.append(p_)
    kuppel_ziel = {x_: float(np.interp(x_, [q[0] for q in _H], [-q[1] for q in _H])) for x_ in range(KX0, KX1 + 1)}

    def riegel_kosten(av):
        """Runde 2 (--riegel; Pruefung V8 r1: S2 rot (a) Max 7, (b) Sattel 3,7, (c) Naht 5,0; S3 rot (b) H 3,0 bei x 423, (d) Sattel 2,3):
        Kosten = die Riegelgroessen S2/S3 selbst (riegel_v8.s2_s3 auf der Silhouette aller Knochen, j_huckel), als Huerden mit 0,5 px
        Abstand zur Schwelle, dazu S2a gegen die Vorlage und 0,02 x Summe a_i^2 (kleine Verschiebungen)."""
        mo2 = V.modell(KB.packen(blob, d, form(av), tim))
        r = V.b2.umriss_schnell(mo2, V.PV, knochen=list(range(23)))
        omk = {x_: int(np.nonzero(r[:, x_])[0].min()) for x_ in range(KX0, KX1 + 1) if r[:, x_].any()}
        kuppel = float(np.mean([abs(omk[x_] - kuppel_ziel[x_]) for x_ in omk]))
        bone = np.where(r, 0, -1)
        r2, r3, om = RG.s2_s3(mo2, bone)
        h = lambda v: max(0.0, v)
        Hh, Hx = r3['b_huegel']
        c = (2 * r2['a']['mittel'] + 2 * h(r2['a']['max'] - 5.5) + 4 * h(r2['b'][0] - 1.5) + 4 * h(r2['c']['huckel_px'] - 2.5)
             + 4 * h(r3['a_kuppe'] - 3.5) + 4 * h(5.5 - Hh) + 4 * h(Hh - 9.5) + 2 * (h(430 - Hx) + h(Hx - 443))
             + 4 * len(r3['c_weitere']) + 8 * h(r3['d_sattel_kopf'][0] - 1.5) + 4 * h(r3['e_K'] - 2.5) + 0.02 * float(np.sum(av ** 2))
             + 2 * kuppel)
        return c, dict(S2=r2, S3=r3, kuppel_mittel=round(kuppel, 2))

    def kosten(av):
        av = np.clip(av, -A_MAX, A_MAX)
        if a.riegel:
            return riegel_kosten(av)[0]
        k = kontur(av)
        dd = k - ziel
        c = float(np.mean(np.abs(dd)) + 0.5 * np.max(np.abs(dd)) + 0.02 * np.sum(av ** 2))
        if a.kette:
            H, s_j, s_k, kp = form_masse(k)
            c += 2 * max(0.0, 5.5 - H) + 2 * max(0.0, H - 9.5) + 2 * max(0.0, s_j - 1.5) + 2 * max(0.0, s_k - 1.5) + 2 * max(0.0, kp - 3.5)
        return c

    x = np.zeros(len(kand))
    c0 = kosten(x)
    c = c0
    for _runde in range(4):
        besser = False
        for j in range(len(kand)):
            for w in np.arange(-A_MAX, A_MAX + 0.5, 1.0):
                if w == x[j]:
                    continue
                y = x.copy(); y[j] = w
                cy = kosten(y)
                if cy < c - 1e-9:
                    c, x, besser = cy, y, True
        if not besser:
            break
    ecken = {}
    basis_e = json.load(open(a.start))['ecken'] if a.start else {}
    for i, ai in zip(kand, x):
        dv = [int(round(ai * J[i][k])) for k in range(3)]
        b0 = basis_e.get(str(i), [0, 0, 0])
        dv = [dv[k] + b0[k] for k in range(3)]
        if any(dv):
            ecken[str(i)] = dv
    for k_, v_ in basis_e.items():
        ecken.setdefault(k_, v_)
    ecken = {k_: ecken[k_] for k_ in sorted(ecken, key=int)}
    dd = kontur(x) - ziel
    out = dict(quelle='ruecken_suche.py (Basis %s sha256 %s, Kette %s, Start %s%s), Ziel Vorlage-Oberkontur x %d-%d' % (
                   os.path.basename(a.basis), sha_basis, a.kette and os.path.basename(a.kette), a.start and os.path.basename(a.start),
                   ', Riegel S2/S3' if a.riegel else '', xs[0], X1),
               riegel=riegel_kosten(np.clip(x, -A_MAX, A_MAX))[1] if a.riegel else None,
               masse=dict(zip(('H', 'sattel_330_420', 'sattel_380_447', 'kuppe'), form_masse(kontur(x)))) if a.kette else None,
               kandidaten=kand, a_px={str(i): round(float(ai), 2) for i, ai in zip(kand, x)}, ecken=ecken,
               kosten_start=round(c0, 3), kosten=round(kosten(x), 3), d_mittel=round(float(np.mean(np.abs(dd))), 2),
               d_max=round(float(np.max(np.abs(dd))), 1))
    json.dump(out, open(a.aus, 'w'), indent=1)
    print('Kandidaten', kand)
    print('Kosten %.3f -> %.3f, Kontur x %d-%d |d| Mittel %.2f Max %.1f' % (c0, out['kosten'], X0, X1, out['d_mittel'], out['d_max']))
    print('Ecken', ecken)


if __name__ == '__main__':
    main()
