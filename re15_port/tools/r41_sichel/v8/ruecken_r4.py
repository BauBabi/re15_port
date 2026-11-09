"""SF V8 Runde 4: Rueckenecken von Mesh 8 nach den Befunden der Pruefung V8 r3 -> ruecken_v8.json.

    python ruecken_r4.py --kette KETTE_JSON --start RUECKEN_JSON --aus AUS_JSON [--basis EMD_V7] [--runden N] [--nur-start]

Befunde (Pruefung V8 r3):
  O3 Schlitz (Zahlen-Pruefer: "In F344-F378 klafft links neben dem Kopf ein Loch durch die Silhouette: 807 Loch-Pixel"; Sicht-Pruefer (2):
     "Links daneben laeuft ein gerader dunkler Grat vom Kopf nach oben") - Ursache gemessen (r4_gemein): Huegel-Flaeche v14 von vorn Rueckseite.
  S10/O5 Wurzel (Zahlen-Pruefer: 46 090 px; Sicht-Pruefer: "dunkelgrauer Keil mit harten Kanten") - Stufe w (wurzel_r4.py) nimmt dem
     Wurzelrohr die Flaeche; die Ecken von Ring 1, deren Achse die Haut nicht schneidet (oben am Bogen), deckt die Haut von Mesh 8 hier zu.
  O7 Schlauch ("Von schraeg hinten liegt der Bogen noch als Wulst auf dem Ruecken") - die gehobene Haut bildet den Ansatz des Schwanzes.
Verfahren wie ruecken_r3.py (deterministische Koordinatensuche, Schritte 40/20/10/5 E., feste Reihenfolge), Kosten = ruecken_r3 (PV-Riegel
S2/S3, Vorderansicht P1 + Szene Takt F344/F354/F364 gegen V7) und neu (PORT-WAHL):
  + W_SCHLITZ je Schlitz-Bildpunkt (r4_gemein.schlitz, P1 + Szene Takt F336-F372 jeder zweite; Soll 0)
  + W_BEDECKT je E. fehlender Bedeckung der Ring-1-Ecken (wurzel_r4.bedeckung: Summe (WURZEL_TIEF - Tiefe)+; Soll 0)
  + W_S4 (S4_WURZEL - Riegel S4 Wurzel)+ (Ring 1 zieht in Stufe w radial unter die Haut)
Lauf e (--es N): statt der Koordinatensuche eine (1+1)-Evolution mit fester Saat ueber mehrere Huegel-Ecken zugleich (gleiche Kosten).
Kandidaten: wie ruecken_r3 (Ecken bei x 380-452 nahe der Oberkontur in PV) + die Ecken der Flaechen ueber Ring 1 (naechste Flaeche je Ecke,
wurzel_r4.naechste). Jede Probe mit Stufe w wie im Bau. Kein RE-Wert, PORT-WAHL.
"""
import os, sys, json, argparse, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import kopf_orig_bau as KB   # noqa: E402
import v8_bau as VB          # noqa: E402
import r3_gemein as R3       # noqa: E402
import r4_gemein as R4       # noqa: E402
import wurzel_r4 as W4       # noqa: E402
import ruecken_r3 as RR3     # noqa: E402

SCHRITTE = (40.0, 20.0, 10.0, 5.0)
W_SCHLITZ = 2.0
W_BEDECKT = 0.2
W_S4, S4_WURZEL = 3.0, 56.0        # Riegel S4 Wurzel >= 55 px mit 1 px Abstand (Ring 1 zieht in Stufe w radial ein)


class Ruecken4(RR3.Ruecken):
    def __init__(self, basis, kette):
        super().__init__(basis, kette)
        self.mo7 = V.modell(basis)
        self.posen = R4.vorn_posen()
        self.s7 = [R4.schlitz_menge(self.mo7, p) for _n, p in self.posen]

    def blob_mit(self, delta):
        ms = copy.deepcopy(self.ms)
        for k, dl in delta.items():
            ms[8]['v'][int(k)][:3] = [int(round(self.P0[int(k)][j] + dl[j])) for j in range(3)]
        W4.stufe_w(KB.packen(self.blob, self.d, ms, self.tim), ms, {})
        self._ms = ms
        return KB.packen(self.blob, self.d, ms, self.tim)

    def kosten(self, delta):
        c, m = super().kosten(delta)                      # baut blob_mit (Stufe w), PV + vorn
        blob = KB.packen(self.blob, self.d, self._ms, self.tim)
        mo = V.modell(blob)
        sl = [(n, int((R4.schlitz_menge(mo, p) & ~s7).sum())) for (n, p), s7 in zip(self.posen, self.s7)]
        bd = W4.bedeckung(blob, self._ms)
        fehlt = float(sum(max(0.0, W4.WURZEL_TIEF - t) for t in bd))
        m['schlitz'] = sl
        m['bedeckung'] = bd
        import riegel_v8 as RG
        m['s4_wurzel'] = RG.s4(mo)['wurzel']
        return c + W_SCHLITZ * sum(v for _n, v in sl) + W_BEDECKT * fehlt + W_S4 * max(0.0, S4_WURZEL - m['s4_wurzel']), m


def kandidaten(R, delta, x0=380):
    out = set(RR3.kandidaten(R, delta))
    blob = R.blob_mit(delta)
    if x0 < 380:                                          # Runde 4 Lauf b: auch die Ecken nahe der Oberkontur bei x x0-380 (S2b-Sattel bei x 362)
        mo = V.modell(blob)
        Rr, T = V.pose_r_t(mo)
        P = np.array([v[:3] for v in R._ms[8]['v']], float)
        q = V.proj(V.PV, P @ (Rr[8].astype(float) / 4096.0).T + T[8].astype(float))
        top = R3.oben(V.b2.umriss_schnell(mo, V.PV, knochen=list(range(23))), 300, 470)
        for i, (x, y) in enumerate(q):
            xi = int(round(x))
            if x0 <= xi < 380 and xi in top and y - top[xi] <= 16:
                out.add(i)
    ms = R._ms
    P8 = np.array([v[:3] for v in ms[8]['v']], float)
    T8 = W4.tris_von(ms[8])
    for q in W4.ring1_b8(V.modell(blob), ms):
        _d, _s, ti, _w, _n = W4.naechste(q, P8, T8)
        out |= set(T8[ti][1])
    return sorted(out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--kette', required=True); ap.add_argument('--start', required=True); ap.add_argument('--aus', required=True)
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--runden', type=int, default=3)
    ap.add_argument('--nur-start', action='store_true')
    ap.add_argument('--x0', type=int, default=380, help='Kandidaten ab Bild-x (Runde 4 Lauf b: 330)')
    ap.add_argument('--schritt-max', type=float, default=40.0)
    ap.add_argument('--es', type=int, default=0, help='Iterationen (1+1)-Evolution statt Koordinatensuche')
    ap.add_argument('--es-ecken', default='0,4,16,17,30,46,47,48,49,50,51,53')
    ap.add_argument('--es-sigma', type=float, default=15.0)
    ap.add_argument('--seed', type=int, default=7)
    a = ap.parse_args()
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    R = Ruecken4(basis, json.load(open(a.kette)))
    st = json.load(open(a.start))
    delta = {str(k): [float(c) for c in v] for k, v in st['ecken'].items()}
    c0, m0 = R.kosten(delta)
    print('Start Kosten %.3f' % c0, json.dumps(m0, default=str), flush=True)
    if a.nur_start:
        return
    cb, mb = c0, m0
    if a.es:
        # Runde 4 Lauf e: (1+1)-Evolution mit fester Saat ueber die Ecken --es-ecken (mehrere Ecken je Schritt; die Koordinatensuche fand
        # am Huegel (S3b 4,5 px, Soll 5-10) keinen Einzelschritt ohne Schlitz/Vorder-Ecke, Probe sfv8/r4)
        ecken = [int(e) for e in a.es_ecken.split(',')]
        rng = np.random.RandomState(a.seed)
        sig = a.es_sigma
        for it in range(a.es):
            dn = {k: list(v) for k, v in delta.items()}
            for e in ecken:
                cur = list(dn.get(str(e), [0.0, 0.0, 0.0]))
                dn[str(e)] = [float(round(cur[j] + sig * rng.randn())) for j in range(3)]
            c, m = R.kosten(dn)
            if c < cb - 1e-9:
                cb, mb, delta = c, m, dn
                sig = min(sig * 1.5, 60.0)
            else:
                sig = max(sig * 0.93 ** 0.25, 3.0)
            if it % 25 == 0:
                print('ES %d Kosten %.3f sigma %.1f' % (it, cb, sig), json.dumps(dict(S2=mb['S2'], S3=mb['S3'], s4_wurzel=mb['s4_wurzel']), default=str), flush=True)
        kand = []
    else:
        kand = kandidaten(R, delta, a.x0)
    print('Kandidaten', kand, flush=True)
    for s in [t for t in SCHRITTE if t <= a.schritt_max]:
        for rnd in range(a.runden):
            besser = False
            for i in kand:
                for ax in range(3):
                    for sg in (1, -1):
                        dn = {k: list(v) for k, v in delta.items()}
                        cur = list(dn.get(str(i), [0.0, 0.0, 0.0]))
                        cur[ax] += sg * s
                        dn[str(i)] = cur
                        c, m = R.kosten(dn)
                        if c < cb - 1e-9:
                            cb, mb, delta, besser = c, m, dn, True
            print('Schritt %g Runde %d Kosten %.3f' % (s, rnd, cb), json.dumps(dict(S2=mb['S2'], S3=mb['S3'], schlitz=mb['schlitz'],
                  bedeckung=mb['bedeckung'], s4_wurzel=mb['s4_wurzel'], vorn=mb['vorn']), default=str), flush=True)
            if not besser:
                break
    out = dict(quelle='ruecken_r4.py (Runde 4, O3 Schlitz + S10 Wurzel; Basis V7 %s, Kette %s, Start %s)' % (
        V.V7_SHA[:8], os.path.basename(a.kette), os.path.basename(a.start)),
        ecken={k: [int(round(c)) for c in v] for k, v in sorted(delta.items(), key=lambda kv: int(kv[0])) if any(abs(c) >= 0.5 for c in v)},
        start_quelle=st.get('quelle'), kosten_start=round(c0, 4), kosten=round(cb, 4), masse=mb,
        verfahren=('ES %d Saat %d Ecken %s' % (a.es, a.seed, a.es_ecken)) if a.es else ('Koordinatensuche x0 %d Schritt <= %g' % (a.x0, a.schritt_max)))
    json.dump(out, open(a.aus, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
