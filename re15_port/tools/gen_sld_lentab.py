#!/usr/bin/env python3
"""Erzeugt engine/src/gen/sld_lentab.inc — die Cut-Laengentabelle aus den STAGE-Overlays.

WOZU: Der Vordergrundatlas eines Cuts wird ueber einen Trailer am Ende der geladenen
Cut-Daten gefunden; die dafuer noetige Laenge L steht als u16 in einer Tabelle im
STAGE-Overlay (s. re15_sld.h, FUN_80021bbc @0x80021d4c/50/5c). Der PC-Port liest sie
direkt aus BIN/STAGE<n>.BIN. Auf der PSX waere das absurd: fuer 2 Byte je (Raum, Cut)
eine bis zu 137 KB grosse Datei von CD zu holen — und zwar bei jedem Stage-Wechsel.

Die vollstaendige Tabelle aller 6 Stages sind 6 * 256 * 16 * 2 Byte, davon aber nur die
tatsaechlich belegten Zeilen; komprimiert auf die belegten Raeume sind es wenige KB.
Diese Datei erzeugt genau das als C-Array.

⛔ DRIFTSCHUTZ: der Unit-Test unit_sld_atlas vergleicht die erzeugte Tabelle gegen die
Originaldateien. Weicht sie ab, faellt der Test — eine Kopie von Spieldaten darf nie
unbemerkt auseinanderlaufen.

Aufruf:  python re15_port/tools/gen_sld_lentab.py <cd-wurzel> <ziel.inc>
"""
import os
import struct
import sys

# Datei-Offsets der Tabelle je Stage (disasm-belegt, s. re15_sld.h).
TBL = {1: 0x1EAE4, 2: 0x1713C, 3: 0x1D0F8, 4: 0x18590, 5: 0x1DF18, 6: 0x01E3C}
COLS = 16
ROW  = 0x20


def main(argv):
    if len(argv) < 3:
        print("Aufruf: gen_sld_lentab.py <cd-wurzel> <ziel.inc>", file=sys.stderr)
        return 2
    cd, dst = argv[1], argv[2]

    rows = []          # (stage, room_index, [16 u16])
    for stage in sorted(TBL):
        p = os.path.join(cd, "BIN", "STAGE%d.BIN" % stage)
        if not os.path.exists(p):
            print("fehlt: %s" % p, file=sys.stderr)
            return 1
        blob = open(p, "rb").read()
        base = TBL[stage]
        for rr in range(0x100):
            # Nur Zeilen, zu denen es auch eine BSS-Datei gibt — der Rest ist
            # unbenutzter Overlay-Speicher und wuerde die Tabelle nur aufblaehen.
            bss = os.path.join(cd, "STAGE%d" % stage, "ROOM%X%02X.BSS" % (stage, rr))
            if not os.path.exists(bss):
                continue
            off = base + rr * ROW
            if off + ROW > len(blob):
                continue
            vals = list(struct.unpack_from("<%dH" % COLS, blob, off))
            rows.append((stage, rr, vals))

    with open(dst, "w", encoding="utf-8", newline="\n") as f:
        f.write("/* ERZEUGT von re15_port/tools/gen_sld_lentab.py — NICHT von Hand aendern.\n")
        f.write(" *\n")
        f.write(" * Cut-Laengentabelle der STAGE-Overlays (s. re15_sld.h): L = Byte-Laenge der\n")
        f.write(" * vom CD geladenen Cut-Daten, deren letzte 8 Byte den Trailer mit dem Offset\n")
        f.write(" * des SLD-Vordergrundatlas tragen. Quelle: BIN/STAGE<n>.BIN an den\n")
        f.write(" * disasm-belegten Offsets 0x1EAE4 / 0x1713C / 0x1D0F8 / 0x18590 / 0x1DF18 / 0x01E3C.\n")
        f.write(" * Gegen Drift abgesichert durch unit_sld_atlas. */\n")
        f.write("typedef struct { unsigned char stage, room; unsigned short len[%d]; }\n" % COLS)
        f.write("        re15_sld_lenrow_t;\n\n")
        f.write("static const re15_sld_lenrow_t re15_sld_lentab[] = {\n")
        for stage, rr, vals in rows:
            f.write("    { %d, 0x%02X, { %s } },\n"
                    % (stage, rr, ", ".join("0x%04X" % v for v in vals)))
        f.write("};\n")
        f.write("#define RE15_SLD_LENTAB_COUNT %d\n" % len(rows))

    print("%d Zeilen (Raeume) geschrieben nach %s" % (len(rows), dst))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
