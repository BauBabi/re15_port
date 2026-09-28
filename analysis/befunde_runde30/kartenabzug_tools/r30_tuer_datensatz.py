#!/usr/bin/env python
"""Runde 30 karten-marken-abzug: Tuer-Datensaetze (SCD Door_aot_set 0x3b, 32 B) eines
Raums mit DATEI-OFFSET ausgeben. Sucht NICHT ueber eine Opcode-Laengentabelle, sondern
ueber das Byte-Muster des Records selbst (Opcode 0x3b, Rechteck, Zielraum) - damit ist
das Ergebnis unabhaengig von der Opcode-Tabelle des Generators.
Aufruf: python r30_tuer_datensatz.py <ROOMxxxx.RDT> [...]"""
import struct, sys, os

NAMES = ["collision","camera","zone","light","md1ptr","floor","block","message",
         "mainScd","subScd","extraScd","effect","x50","espTim","modelTim","anim"]

def main():
    for p in sys.argv[1:]:
        d = open(p, 'rb').read()
        offs = {nm: struct.unpack_from('<I', d, 0x20 + 4*i)[0] for i, nm in enumerate(NAMES)}
        print("==", os.path.basename(p), len(d), "B  mainScd @0x%X subScd @0x%X" % (offs['mainScd'], offs['subScd']))
        for name in ('mainScd', 'subScd'):
            s = offs[name]
            e = (sorted(v for v in offs.values() if v > s) + [len(d)])[0]
            for off in range(s, e - 32):
                if d[off] != 0x3b: continue
                b = d[off:off+32]
                slot, sce, sat, band = b[1], b[2], b[3], b[4]
                rx, rz, rw, rd = struct.unpack_from('<hhhh', b, 6)
                nx, ny, nz = struct.unpack_from('<hhh', b, 14)
                yaw = struct.unpack_from('<h', b, 20)[0]
                stg, rmd, cut = b[22], b[23], b[24]
                # Plausibilitaet: Ziel-Stage 0..6, Slot < 32, sce in (0,1,2), rw/rd >= 0
                if stg > 6 or slot > 31 or sce > 2 or rw < 0 or rd < 0: continue
                if rw > 12000 or rd > 12000: continue
                print("  %s @Datei 0x%05X  slot %d sce %d sat 0x%02X band %d  "
                      "Rechteck x %d z %d w %d d %d (Mitte %d,%d)  Ziel ROOM%X%02X0 cut %d  "
                      "Ankunft (%d,%d,%d) yaw %d"
                      % (name, off, slot, sce, sat, band, rx, rz, rw, rd,
                         rx + rw//2, rz + rd//2, stg + 1, rmd, cut, nx, ny, nz, yaw))
                print("      Bytes:", b.hex(' '))

if __name__ == '__main__':
    main()
