"""orig_zopf_inc.py <frames.txt> <ziel.inc> — erzeugt aus der Original-Messung (orig_zopf_gdb.py, DuckStation-GDB,
Haltepunkte @0x80024c30 / @0x80024e10) den Riegel-Datensatz re15_port/tests/unit/r38_zopf_orig.inc:
  k_r38_orig_start[4][20]  Glieder 17..20 VOR Bild 0: off[3] m[9] t[3] rot[3] v94 v96 (Teil +0x2c.. +0x40.. +0x54.. +0x60.. +0x94/+0x96)
  k_r38_orig_ein[N][35]    Eingang je Bild: Spieler rot (+0x68..) pos (+0x34..), Teil 0 +0x18 m[9] t[3], Teil 8 +0x18 m[9] t[3],
                           Teil 0 rot.vy/vz (+0x62/+0x64), Teil 8 rot.vy (+0x5c2), Teil 8 +0x94 (+0x5f4), 0x800acc0c
  k_r38_orig_hash[N]       FNV-1a 32 ueber die 80 Sollwerte NACH dem Treiber (je Glied m[9] t[3] rot[3] v94 v96, dann Teil 8 +0x40
                           m[9] t[3]), jeder Wert als int32 little-endian
  k_r38_orig_phase         Phase je Bild (r ruhe, d drehung, n nachlauf, g gehen, s stehen)"""
import struct, sys

N_EIN = 6 + 12 + 12 + 5
N_VOR = 20
N_NACH = 17


def lesen(pfad):
    bilder = []
    for z in open(pfad):
        t = z.split()
        if not t or t[0] != "F":
            continue
        phase = t[2]
        zahlen = [int(x) for x in t[3:] if x[0].isdigit() or x[0] == "-"]
        assert len(zahlen) == N_EIN + 4 * N_VOR + 4 * N_NACH + 12, (t[1], len(zahlen))
        ein = zahlen[:N_EIN]
        vor = [zahlen[N_EIN + k * N_VOR:N_EIN + (k + 1) * N_VOR] for k in range(4)]
        o = N_EIN + 4 * N_VOR
        nach = zahlen[o:o + 4 * N_NACH] + zahlen[o + 4 * N_NACH:o + 4 * N_NACH + 12]
        bilder.append((phase, ein, vor, nach))
    return bilder


def fnv(werte):
    h = 0x811C9DC5
    for w in werte:
        for b in struct.pack("<i", w):
            h ^= b
            h = (h * 0x01000193) & 0xFFFFFFFF
    return h


def main():
    bilder = lesen(sys.argv[1])
    n = len(bilder)
    z = ["/* r38_zopf_orig.inc - ERZEUGT von analysis/befunde_runde38/D_belege/orig_zopf_inc.py aus orig_zopf_frames.txt",
         " * (ORIGINAL RE1.5, DuckStation-GDB, Elza PL04 in ROOM1121, Haltepunkte @0x80024c30 / @0x80024e10; Dossier",
         " * D_zopf.md Abschnitt 4.6). %d Bilder. Nicht von Hand aendern. */" % n,
         "#define R38_ORIG_BILDER %d" % n,
         "static const int32_t k_r38_orig_start[4][%d] = {" % N_VOR]
    for g in bilder[0][2]:
        z.append("    { %s }," % ", ".join(map(str, g)))
    z.append("};")
    z.append("static const int32_t k_r38_orig_ein[R38_ORIG_BILDER][%d] = {" % N_EIN)
    for b in bilder:
        z.append("    { %s }," % ", ".join(map(str, b[1])))
    z.append("};")
    z.append("static const uint32_t k_r38_orig_hash[R38_ORIG_BILDER] = {")
    for i in range(0, n, 8):
        z.append("    %s," % ", ".join("0x%08xu" % fnv(b[3]) for b in bilder[i:i + 8]))
    z.append("};")
    z.append('static const char k_r38_orig_phase[R38_ORIG_BILDER + 1] = "%s";' % "".join(b[0][0] for b in bilder))
    open(sys.argv[2], "w", newline="\n").write("\n".join(z) + "\n")
    print("%d Bilder -> %s" % (n, sys.argv[2]))


if __name__ == "__main__":
    main()
