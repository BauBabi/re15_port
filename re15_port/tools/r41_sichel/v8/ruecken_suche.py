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
A_MAX = 10.0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=V.ASSET)
    ap.add_argument('--aus', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--kette')
    ap.add_argument('--start', help='ruecken-JSON einer frueheren Runde: deren Verschiebungen sind die Basis (fest), gesucht wird darauf')
    ap.add_argument('--x-min', type=int, default=None, help='nur Kandidaten mit Bild-x >= x-min')
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
    kand = [i for i, (x, y) in enumerate(q) if xmin <= round(x) <= 450 and y <= top.get(int(round(x)), 999) + 14]
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

    def kosten(av):
        av = np.clip(av, -A_MAX, A_MAX)
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
    out = dict(quelle='ruecken_suche.py (Basis %s sha256 %s, Kette %s), Ziel Vorlage-Oberkontur x %d-%d' % (
                   os.path.basename(a.basis), sha_basis, a.kette and os.path.basename(a.kette), xs[0], X1),
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
