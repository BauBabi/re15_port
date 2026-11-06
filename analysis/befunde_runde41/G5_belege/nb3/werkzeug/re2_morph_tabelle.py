"""re2_morph_tabelle.py - C-Tabelle fuer den Zeichen-Riegel (test_r41_g5auftritt.c) aus den RE2-RAM-Vertexpuffern der Masse.

Quelle: re2_v_sNN.txt (Abnahme G5 2, sonde/), Kopf "nv w0 w1 w2 w3", danach nv Zeilen x y z = RE2-RAM Vertexpuffer 0x80132be0
(Mesh 2 nach FUN_8004bf90, Savestate sNN). Ausgabe: je Savestate Szenenbild, Gewichte, Bbox aller Vertices und jeder 10. Vertex.
Aufruf: python re2_morph_tabelle.py <ordner mit re2_v_sNN.txt>
"""
import sys, os

BILD = {"s06": 165, "s09": 313, "s13": 479, "s17": 677}
d = sys.argv[1]
print("/* Erzeugt von G5_belege/nb3/werkzeug/re2_morph_tabelle.py aus RE2-RAM 0x80132be0 (Mesh 2, 635 Vertices). */")
print("static const struct { const char *ss; int bild; int16_t w[4]; int16_t bb[6]; int16_t v[64][3]; } k_re2_masse[4] = {")
for ss, bild in BILD.items():
    zeilen = open(os.path.join(d, "re2_v_%s.txt" % ss)).read().split("\n")
    kopf = [int(x) for x in zeilen[0].split()]
    nv = kopf[0]
    v = [tuple(int(x) for x in z.split()) for z in zeilen[1:1 + nv]]
    assert len(v) == 635
    bb = [min(p[0] for p in v), max(p[0] for p in v), min(p[1] for p in v), max(p[1] for p in v),
          min(p[2] for p in v), max(p[2] for p in v)]
    sub = [v[i] for i in range(0, 640, 10)]
    print("    { \"%s\", %d, { %d, %d, %d, %d }, { %d, %d, %d, %d, %d, %d }, {" % ((ss, bild) + tuple(kopf[1:5]) + tuple(bb)))
    for k in range(0, 64, 8):
        print("        " + " ".join("{%d,%d,%d}," % p for p in sub[k:k + 8]))
    print("    } },")
print("};")
