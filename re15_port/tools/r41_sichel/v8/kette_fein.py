"""SF V8 Runde 2: Feinsuche der Ruhekette am GERENDERTEN Mesh (statt Kreisen) -> kette_v8.json.

    python kette_fein.py START_JSON AUS_JSON [ITERATIONEN] [--ruecken JSON] [--seed N]

Anlass (Pruefung V8 r1, Zahlen-Pruefer): S2 rot ((a) Max 7, (b) Sattel 3,7, (c) Naht 5,0); kette_suche.py rechnet mit Kreisen, die am
Gelenk 17 5-7 px vom Mesh abwichen (Messung sfv8/r2 fein.py: Manschette ueber der Oberkante, in v8_bau behoben). Diese Suche baut fuer
jeden Kandidaten die Meshes 16-22 und die PV-Pose mit v8_bau (nur Keyframe PV) und misst wie riegel_v8 auf der Silhouette
(V.b2.umriss_schnell, alle Knochen): S1 (22 T-Stationen), S2a/S2b/S2c, S4 (riegel_v8.s4), Spitze.
Kosten (Huerden mit Abstand zur Riegel-Schwelle; PORT-WAHL):
  5 (S1-Mittel - 3)+ + 3 (S1-Max - 6,5)+ + 5 (S2a-Mittel - 3)+ + 3 (S2a-Max - 5)+ + 10 (S2b - 1,5)+ + 4 (S2c - 2,5)+
  + 3 (56 - Wurzel)+ + 3 (37 - Mitte)+ + 3 (Ende - 9,5)+ + 5 (nicht monoton) + 0,3 S1-Mittel + 0,3 S2a-Mittel + 0,2 S2b
  + 0,01 |(x - x0) / Skala|^2
Parameter x (kette_suche.Suche.kette, eben): Wurzel (3), Gier/Nick des ersten Glieds, Rollwinkel der Biegeebene, 6 Biegungen
(0..25 Grad), Gliedlaenge, Rohrlaenge B, Rohrknick (Grad, v8_bau.rohr_drehung), Verjuengung, Radius Wurzel / Gelenk 17.
Verfahren: (1+1)-Evolution mit fester Saat (deterministisch), Schrittweite x1,5 bei Erfolg, sonst x0,93^0,25 (0,05-2,0).
Kein RE-Wert; das Ergebnis ist PORT-WAHL.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import v8_bau as VB          # noqa: E402
import kette_suche as KS     # noqa: E402
import riegel_v8 as RG       # noqa: E402

SKAL = np.array([50, 50, 50, 0.05, 0.05] + [0.1, 0.1] * 6 + [20, 0.1, 5.0, 0.1, 10.0, 10.0])
FREI = np.array([i not in (8, 10, 12, 14, 16) for i in range(23)])     # eben: x[8..16 gerade] unbenutzt


class Fein:
    def __init__(self, basis, ruecken):
        self.basis = basis
        self.ru = ruecken
        self.S = KS.Suche(basis, ruecken)
        self.kpv = V.BB.kf_von_clip(V.modell(basis), V.PV['clip'], V.PV['bild'])
        self.ov = {int(k): v for k, v in V.lade('m13b_oben.json')['oben_vorlage_maske'].items()}
        self.st = V.stationen()

    def kette(self, x):
        q, rp, pts, Pm, rad = self.S.kreise(x)
        return dict(gelenke_b8=[[round(float(c), 1) for c in p] for p in pts], L=int(round(x[17])), radien=[round(float(r), 1) for r in rad],
                    hinten=round(float(np.clip(x[18], KS.B_MIN, KS.B_MAX)), 3), rohr_grad=round(float(x[19]), 2),
                    verjuengung=round(float(np.clip(x[20], 0.3, 1.0)), 3),
                    biegung_grad=[round(float(np.degrees(KS.WINKEL_MAX * 0.5 * (1 + np.tanh(x[5 + 2 * j])))), 2) for j in range(6)],
                    bild=[[round(float(c), 1) for c in qq] for qq in q[11::KS.KREISE_JE_GLIED]])

    def messen(self, x):
        blob, _ = VB.bauen(None, 'sr', kette=self.kette(x), ruecken=self.ru, nur_kf=[self.kpv], blob=self.basis)
        mo = V.modell(blob)
        mr = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16)))
        ms = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16, 23)))
        alle = mr | ms
        top = lambda m, a, b: {x_: int(np.nonzero(m[:, x_])[0].min()) for x_ in range(a, b + 1) if m[:, x_].any()}
        e = V.aussenkante(alle, self.st)
        d = np.array([(v - s['ziel']) if v is not None else 30.0 for v, s in zip(e, self.st)])
        om = top(alle, 280, 470)
        da = np.array([om[x_] - self.ov[x_] for x_ in range(330, 386) if x_ in om and x_ in self.ov])
        sb, sbx = RG.sattel(om, 300, 420)
        tr, ts = top(mr, 300, 459), top(ms, 300, 459)
        oben = [x_ for x_ in range(330, 421) if x_ in ts and x_ in tr and ts[x_] <= tr[x_]]
        if oben:
            xn = max(oben)
            sc = max([ts[x_] - tr[x_] for x_ in range(xn - 15, xn + 16) if x_ in ts and x_ in tr])
        else:
            xn, sc = None, 30.0
        s4 = RG.s4(mo)
        q = self.S.kreise(x)[0]
        return dict(s1m=float(np.abs(d).mean()), s1x=float(np.abs(d).max()), s2am=float(np.abs(da).mean()), s2ax=float(np.abs(da).max()),
                    s2b=float(sb), s2bx=int(sbx), s2c=float(sc), naht=xn, wurzel=float(s4['wurzel']),
                    mitte=float(min(s4['s90_130'])) if s4['s90_130'] else 0.0, ende=float(s4['ende']), mono=bool(s4['monoton']),
                    spitze=[round(float(c), 1) for c in q[-1]])

    def kosten(self, x, x0):
        m = self.messen(x)
        h = lambda v: max(0.0, v)
        c = (5 * h(m['s1m'] - 3.0) + 3 * h(m['s1x'] - 6.5) + 5 * h(m['s2am'] - 3.0) + 3 * h(m['s2ax'] - 5.0) + 10 * h(m['s2b'] - 1.5)
             + 4 * h(m['s2c'] - 2.5) + 3 * h(56 - m['wurzel']) + 3 * h(37 - m['mitte']) + 3 * h(m['ende'] - 9.5) + (0 if m['mono'] else 5)
             + 0.3 * m['s1m'] + 0.3 * m['s2am'] + 0.2 * m['s2b'] + 0.01 * float(np.sum(((x - x0) / SKAL) ** 2)))
        return c, m


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('start'); ap.add_argument('aus'); ap.add_argument('n', nargs='?', type=int, default=1600)
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--seed', type=int, default=7)
    a = ap.parse_args()
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    F = Fein(basis, json.load(open(a.ruecken)))
    d0 = json.load(open(a.start))
    x0 = np.array(d0['x'], float)
    if len(x0) < 21:
        x0 = np.r_[x0, 0.0, 0.6][:21]
    if len(x0) < 23:
        x0 = np.r_[x0, 450.0, 417.0]
    rng = np.random.RandomState(a.seed)
    xb = x0.copy(); cb, mb = F.kosten(xb, x0)
    print('Start', round(cb, 3), mb)
    sig = 1.0
    for it in range(a.n):
        x = xb + sig * rng.randn(23) * FREI * SKAL
        c, m = F.kosten(x, x0)
        if c < cb:
            xb, cb, mb = x, c, m
            sig *= 1.5
        else:
            sig *= 0.93 ** 0.25
        sig = float(np.clip(sig, 0.05, 2.0))
        if it % 200 == 0:
            print(it, round(cb, 3), round(sig, 3), {k: v for k, v in mb.items() if k != 'spitze'}, flush=True)
    print('Ende', round(cb, 3), mb)
    out = dict(F.kette(xb), x=[float(v) for v in xb], eben=True, fein=mb, kosten=round(cb, 4),
               quelle='kette_fein.py (Start %s, Ruecken %s, %d Schritte, Saat %d) auf kette_suche.py (eben)' % (
                   os.path.basename(a.start), os.path.basename(a.ruecken), a.n, a.seed))
    json.dump(out, open(a.aus, 'w'), indent=1)


if __name__ == '__main__':
    main()
