#!/usr/bin/env python3
"""discard_maskenzensus.py — die FREEZE-MASKE der 16 Wegwerf-Benutzungsstellen.

Der Fix von Runde 23 (die Abfrage wartet nur noch auf das Belegt-Bit des
Nachrichtensystems, RE2 @0x800517f0 `andi v0,v0,0x80` / @0x800517f4) haengt an EINER
gemessenen Tatsache: die ausloesende `Message_on`-Anweisung jeder Benutzungsstelle traegt
eine Maske, die BEIDE relevanten Pause-Bits enthaelt —

    0x01000000  RE15_PAUSE_PAD   Eingabe auf 0xf000   @0x800304f4 lw DAT_800aca40 /
                                                      @0x800304f8 lui v1,0x100 /
                                                      @0x80030514 andi v0,v0,0xf000 /
                                                      @0x8003051c sw DAT_800ac768
    0x02000000  RE15_PAUSE_SCD   Frame-Runner kehrt um @0x8003f040 lw /
                                                      @0x8003f044 lui v1,0x200 /
                                                      @0x8003f04c bne

Die Maske ist das Halbwort pc[2..3] des Opcodes, <<16 (op_message_on in
engine/src/scd_vm.c, byte-true @0x80040508 -> @0x8004051c). Solange sie steht, kann in der
Spanne "vorgemerkt" WEDER der Spieler NOCH das Skript etwas tun — es gibt also keinen
Ueberhang, den man absichern muesste, und keinen Raumwechsel, den die Vormerkung
ueberleben muesste.

Aufruf:  python re15_port/tools/discard_maskenzensus.py [assets-dir]
         (Default: re15_port/shared_assets/PSX)
"""
import os
import re
import subprocess
import sys

# Dieselbe Tabelle, die der Motor benutzt (engine/src/gen/discard_sites.inc).
SITES = [(0x1090, 9), (0x10D0, 9), (0x10D1, 9), (0x1100, 4), (0x1101, 4),
         (0x11E0, 9), (0x11E0, 12), (0x11E1, 9), (0x11E1, 12),
         (0x1230, 9), (0x1231, 9), (0x3010, 1), (0x3011, 1),
         (0x3050, 5), (0x3051, 5), (0x4000, 2)]

PAUSE_PAD = 0x01000000
PAUSE_SCD = 0x02000000

HIER = os.path.dirname(os.path.abspath(__file__))
TOOL = os.path.join(HIER, "scd_dump_room.py")


def main():
    assets = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
        HIER, os.pardir, "shared_assets", "PSX")
    beide = 0
    for room, msg in SITES:
        pfad = os.path.join(assets, "STAGE%d" % (room >> 12), "ROOM%04X.RDT" % room)
        if not os.path.exists(pfad):
            print("ROOM%04X msg %-2d  RDT fehlt: %s" % (room, msg, pfad))
            continue
        out = subprocess.run([sys.executable, TOOL, pfad],
                             capture_output=True, text=True).stdout
        region = None
        treffer = []
        for ln in out.splitlines():
            m = re.match(r"=== (\w+)\s+\[file", ln.strip())
            if m:
                region = m.group(1)
                continue
            m = re.search(r"Message_on\s+2b ([0-9a-f]{2}) ([0-9a-f]{2}) ([0-9a-f]{2})", ln)
            if m and int(m.group(1), 16) == msg:
                maske = (int(m.group(2), 16) | (int(m.group(3), 16) << 8)) << 16
                off = re.match(r"\s*0x([0-9A-F]+)", ln).group(1)
                treffer.append((region, off, maske))
        for region, off, maske in treffer:
            pad = "PAD" if (maske & PAUSE_PAD) else "---"
            scd = "SCD" if (maske & PAUSE_SCD) else "---"
            if (maske & PAUSE_PAD) and (maske & PAUSE_SCD):
                beide += 1
            print("ROOM%04X msg %-2d  %-6s @0x%05s  Maske 0x%08X  %s %s"
                  % (room, msg, region, off, maske, pad, scd))
        if not treffer:
            print("ROOM%04X msg %-2d  KEIN Message_on %d im SCD" % (room, msg, msg))
    print("\n%d von %d Benutzungsstellen tragen BEIDE Bits (PAD 0x01000000 + SCD 0x02000000)."
          % (beide, len(SITES)))
    return 0 if beide == len(SITES) else 1


if __name__ == "__main__":
    sys.exit(main())
