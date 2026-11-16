"""SF V8 Runde 4: Feinsuche der unteren Biegungen nach Pruefung V8 r3 (S1 Kontur T) -> kette_v8.json.

    python kette_r4.py START_JSON AUS_JSON [ITERATIONEN] [--ruecken JSON] [--seed N]

Befund (Zahlen-Pruefer r3): "S1 Kontur T: soll <=4/<=8 und r2-Wert 1,8/5,0 halten; ist 3,05/7,0" (GELB). Gemessen (sfv8/r4): die
Abweichung liegt bei s 120-160 (d = -3/-6/-4/-7/-6 px, Modell weiter AUSSEN als die Nutzerlinie T); dort hat Runde 3 (O4 "Schwanz unten
breiter") die Radien angehoben - die Breite kam nach aussen statt nach innen.
Verfahren: kette_fein.Fein (gerenderte Silhouette, (1+1)-Evolution mit fester Saat), aber
  - Wurzel, erstes Glied, Gliedlaenge, Rohr und die Radien bleiben (x 0-4, 17-22 fest; Uebergang J und Wurzel wie gesucht, Stufe w);
    frei sind nur die sechs Biegungen (x 5, 7, 9, 11, 13, 15); mit --mit-radien (Lauf b) auch Radius Wurzel / Gelenk 17 (x 21, 22):
    ohne Wurzelrohr bildet das Band Ring 1 -> Ring 2 den Uebergang J, die Schwanz-Oberkante lag bei x 340-356 3-4 px unter der Vorlage
    (Riegel S2b 2,8 bei x 362, Lauf sfv8/r4 rs4a);
  - Bau mit Stufe w (wurzel_r4) statt e;
  - Kosten wie kette_fein Runde 3, S1-Huerden auf den r2-Wert: 5 (S1-Mittel - S1_MITTEL)+ + 3 (S1-Max - S1_MAX)+ (PORT-WAHL;
    r2 1,8 / 5,0), Wurzel-Tiefe entfaellt (Stufe w), S4b unten wie Runde 3.
Kein RE-Wert; das Ergebnis ist PORT-WAHL.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import v8_bau as VB          # noqa: E402
import kette_fein as KF      # noqa: E402
import riegel_v8 as RG       # noqa: E402

S1_MITTEL, S1_MAX = 2.0, 5.0
FREI = np.array([i in (5, 7, 9, 11, 13, 15) for i in range(23)])


class Fein4(KF.Fein):
    def messen(self, x):
        alt = VB.bauen

        def bauen_w(basis, stufen, **kw):
            return alt(basis, stufen.replace('e', 'w'), **kw)
        VB.bauen = bauen_w
        try:
            m = super().messen(x)
        finally:
            VB.bauen = alt
        m['r3']['wurzel_tief'] = [KF.WURZEL_TIEF]
        return m

    def kosten(self, x, x0):
        c, m = super().kosten(x, x0)
        h = lambda v: max(0.0, v)
        c += 5 * h(m['s1m'] - S1_MITTEL) + 3 * h(m['s1x'] - S1_MAX)
        return c, m


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('start'); ap.add_argument('aus'); ap.add_argument('n', nargs='?', type=int, default=600)
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--seed', type=int, default=7)
    ap.add_argument('--mit-radien', action='store_true', help='Lauf b: auch Radius Wurzel / Gelenk 17 frei (x 21, 22)')
    a = ap.parse_args()
    frei = FREI | (np.arange(23) >= 21) if a.mit_radien else FREI
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    F = Fein4(basis, json.load(open(a.ruecken)))
    d0 = json.load(open(a.start))
    x0 = np.array(d0['x'], float)
    rng = np.random.RandomState(a.seed)
    xb = x0.copy(); cb, mb = F.kosten(xb, x0)
    print('Start', round(cb, 3), {k: v for k, v in mb.items() if k != 'spitze'}, flush=True)
    sig = 1.0
    for it in range(a.n):
        x = xb + sig * rng.randn(23) * frei * KF.SKAL
        c, m = F.kosten(x, x0)
        if c < cb:
            xb, cb, mb = x, c, m
            sig *= 1.5
        else:
            sig *= 0.93 ** 0.25
        sig = float(np.clip(sig, 0.05, 2.0))
        if it % 50 == 0:
            print(it, round(cb, 3), round(sig, 3), dict(s1m=round(mb['s1m'], 2), s1x=mb['s1x'], s2am=round(mb['s2am'], 2), s2ax=mb['s2ax'],
                                                       s2b=mb['s2b'], s2c=mb['s2c'], r3=mb['r3']), flush=True)
    print('Ende', round(cb, 3), mb)
    k = F.kette(xb)
    alt = json.load(open(a.start))
    for f in ('hinten', 'rohr_grad', 'verjuengung', 'L'):
        assert k[f] == alt[f], f
    out = dict(k, x=[float(v) for v in xb], eben=True, fein=mb, kosten=round(cb, 4),
               quelle='kette_r4.py (Start %s, Ruecken %s, %d Schritte, Saat %d; frei: Biegungen%s) auf kette_fein.py' % (
                   os.path.basename(a.start), os.path.basename(a.ruecken), a.n, a.seed, ' und Radius Wurzel/Gelenk 17' if a.mit_radien else ''))
    json.dump(out, open(a.aus, 'w'), indent=1)


if __name__ == '__main__':
    main()
