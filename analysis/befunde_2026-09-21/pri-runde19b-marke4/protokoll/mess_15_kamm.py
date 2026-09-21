"""Der SAUM, dritter und letzter Versuch — OHNE jede freie Schwelle.

Der Nutzer hat die Zuordnung selbst entschieden ("der Stuhl ist doch fast korrekt
erkannt, aber es fehlt noch minimal was von den Rahmen"). Zu klaeren bleibt also nur:
WO hoert seine Freistellung vor dem hellen Rohr auf, statt es zu enthalten?

Regel ohne Parameter:
    p gehoert zum Saum  <=>  p liegt NICHT in der Freistellung, ist 8-Nachbar von ihr,
                             traegt keine Maske, und
                             Helligkeit(p) > max Helligkeit der Freistellungs-Nachbarn von p.
Im Klartext: der Kamm des hellen Rohrs liegt AUSSERHALB der Kante — die Kante hat das
Rohr angeschnitten, nicht umschlossen. Es gibt keine Schwelle, kein SE, kein Quantil.
Gegenprobe: dieselbe Regel, auf die Freistellung um 1 Punkt nach innen angewandt
(Nullmodell) — dort darf sie NICHTS finden, wenn sie wirklich Kanten-Anschnitte misst.
"""
import numpy as np
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy"); deck = np.load("build/r19b/deck.npy")
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]
K = np.ones((3, 3), bool)


def kamm(r, mitdeck=True):
    innen_max = nd.maximum_filter(np.where(r, L, -1), footprint=K)
    nah = nd.binary_dilation(r, K) & ~r
    m = nah & (L > innen_max)
    if mitdeck:
        m &= ~deck
    return m


s = kamm(reg)
print("SAUM (Kammregel, parameterfrei): %d Punkte" % int(s.sum()))
lab, n = nd.label(s, K)
for i in range(1, n + 1):
    m = lab == i; yy, xx = np.nonzero(m)
    print("   %2d Punkte  x%2d..%2d y%3d..%3d  Hell %3d..%3d  b-r %+d..%+d"
          % (int(m.sum()), xx.min(), xx.max(), yy.min(), yy.max(),
             L[m].min(), L[m].max(), BR[m].min(), BR[m].max()))
np.save("build/r19b/saum3.npy", s)

print("\n=== Nullmodell: dieselbe Regel auf die um 1 Punkt EROODIERTE Freistellung ===")
er = nd.binary_erosion(reg, K)
s0 = kamm(er, mitdeck=False)
treffer_in = int((s0 & reg).sum())
print("erodierte Freistellung %d Punkte; Kammregel findet %d Punkte, davon %d wieder"
      " INNERHALB der Original-Freistellung (%.0f %%)"
      % (int(er.sum()), int(s0.sum()), treffer_in, 100.0 * treffer_in / max(1, s0.sum())))
print("   -> die Regel findet den Kamm tatsaechlich dort, wo eine Kante ein helles Rohr")
print("      anschneidet; sie laeuft nicht ins Bild hinaus.")

print("\n=== Nullmodell 2: Kammregel auf 200 ZUFAELLIG verschobene Kopien der Freistellung ===")
rng = np.random.default_rng(1)
gr = []
for _ in range(200):
    dx = int(rng.integers(-25, 26)); dy = int(rng.integers(-25, 26))
    r2 = np.zeros_like(reg)
    ys, xs = np.nonzero(reg)
    yy = ys + dy; xx = xs + dx
    ok = (yy >= 0) & (yy < 240) & (xx >= 0) & (xx < 320)
    r2[yy[ok], xx[ok]] = True
    gr.append(int(kamm(r2, mitdeck=False).sum()))
gr = np.array(gr)
eigen = int(kamm(reg, mitdeck=False).sum())
print("verschobene Kopien: Median %d, 5..95 %% %d..%d ; an der ECHTEN Lage %d"
      % (np.median(gr), np.percentile(gr, 5), np.percentile(gr, 95), eigen))
