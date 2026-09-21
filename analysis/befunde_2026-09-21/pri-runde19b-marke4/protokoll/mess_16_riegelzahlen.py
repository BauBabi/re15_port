"""Die Zahlen, die der Riegel r19b_marke4 in C nachrechnen muss."""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie, abnahme, maskenbild

CD = "re15_port/shared_assets/PSX"
rdt, _ = geom.load_rdt(CD, "ROOM10D0"); cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
print("Kamerasatz Cut 7: H=%d  R[7]=%d (>0 = Kamera schaut nach unten, oben ist NAEHER)"
      % (H, R[7]))
reg = np.load("build/r19b/reg0701.npy")
deck = np.load("build/r19b/deck.npy"); tief = np.load("build/r19b/tief.npy")

print("\n(1) ABDECKUNG: %d Soll, %d gedeckt, %d ungedeckt"
      % (int(reg.sum()), int((reg & deck).sum()), int((reg & ~deck).sum())))

print("\n(2) ZEILEN-INVARIANTE: Tiefen je Bildzeile")
schlecht = 0
tiefen_je_zeile = {}
for y in range(240):
    c = np.nonzero(reg[y])[0]
    if len(c) == 0:
        continue
    d = np.unique(tief[y, c])
    tiefen_je_zeile[y] = d
    if len(d) != 1:
        schlecht += 1
        print("   y%3d: %d verschiedene Tiefen %s" % (y, len(d), d))
print("   Zeilen mit Punkten: %d, davon mit MEHR als einer Tiefe: %d"
      % (len(tiefen_je_zeile), schlecht))

print("\n(3) MONOTONIE ueber die Zeilen:")
ys = sorted(tiefen_je_zeile)
seq = [int(tiefen_je_zeile[y][0]) for y in ys]
fall = [(ys[i], seq[i], ys[i + 1], seq[i + 1]) for i in range(len(seq) - 1) if seq[i + 1] < seq[i]]
print("   Zeile %d Tiefe %d ... Zeile %d Tiefe %d ; Rueckschritte: %d"
      % (ys[0], seq[0], ys[-1], seq[-1], len(fall)))

print("\n(4) FUSSABDRUCK-SCHRANKE aus dem Kamerasatz:")
ymax = int(np.nonzero(reg.any(1))[0].max())
cols = np.nonzero(reg[ymax])[0]
vz = [geometrie.vz_at_floor(R, t, H, x + 0.5, ymax + 0.5, 0) for x in cols]
print("   tiefste Silhouettenzeile %d, dort Spalten %s" % (ymax, list(cols)))
print("   Bodenpunkt-Kamera-z dort: %s -> groesstes %0.0f -> Eimer %d"
      % ([int(v) for v in vz], max(vz), int(np.floor(1023 * max(vz) / 65536.0))))
grenze = int(np.floor(1023 * max(vz) / 65536.0))
d = tief[reg]
print("   ausgelieferte Tiefen der 2677 Punkte: %d..%d ; darueber: %d"
      % (d.min(), d.max(), int((d > grenze).sum())))

print("\n(5) GEGENPROBE — Spaltenregel (das verworfene Modell):")
B = np.load("build/r19b/depB.npy")
schlecht_b = 0
for y in range(240):
    c = np.nonzero(reg[y])[0]
    if len(c) and len(np.unique(B[y, c])) != 1:
        schlecht_b += 1
print("   Zeilen mit MEHR als einer Tiefe: %d ; Tiefen %d..%d ; ueber der Schranke %d: %d Punkte"
      % (schlecht_b, B[reg].min(), B[reg].max(), grenze, int((B[reg] > grenze).sum())))
for x in range(69, 73):
    rr = np.nonzero(reg[:, x])[0]
    print("   Spalte %d: ausgeliefert %d..%d, Spaltenregel %d..%d"
          % (x, tief[rr, x].min(), tief[rr, x].max(), B[rr, x].min(), B[rr, x].max()))

# Referenz-PBM schreiben
abnahme.pbm_schreiben("re15_port/tests/unit/probes/r19b_marke4_stuhl.pbm", reg)
print("\nreferenz r19b_marke4_stuhl.pbm geschrieben, %d Punkte" % int(reg.sum()))
zz = abnahme.pbm_lesen("re15_port/tests/unit/probes/r19b_marke4_stuhl.pbm")
print("zurueckgelesen bitgleich: %s" % bool((zz == reg).all()))
