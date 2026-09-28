#!/usr/bin/env python3
"""Flag-Zensus fuer die ROOM5080-Generator-Folge.

Welche Stellen in ALLEN ausgelieferten RDTs lesen/schreiben die Flags, die ROOM5080
abfragt:  (3,48) (3,49)  sub00-Schranke / sub01-Setzer
          (12,31)        sub02 Ja/Nein-Antwort
          (5,28) (5,32)  Birkin-Tod / Plc_dest-Ankunft (Bank 5 = DAT_800b1028)
          (1,27) (2,7)   Folge-Klammern in sub02/sub04
Opcodes: Ck 0x21 (bank,bit,val), Set 0x22 (bank,bit,op). Walk = scd_walk_lib (die eine
Laengentabelle). Ausgabe: Raum, Region, Datei-Offset, Opcode, Bytes."""
import os, sys, glob
HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools"))
import scd_walk_lib as L

ZIELE = {(3, 48), (3, 49), (12, 31), (5, 28), (5, 32)}
if len(sys.argv) > 1 and sys.argv[1] == "--klammern":
    ZIELE |= {(1, 27), (2, 7)}

treffer = 0
for p in sorted(glob.glob(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x100:
        continue
    name = os.path.basename(p)[:-4]
    for (kind, ri), ops in sorted(L.regionen(d).items()):
        for pc, op, sz in ops:
            if op not in (0x21, 0x22):
                continue
            bank, bit, val = d[pc + 1], d[pc + 2], d[pc + 3]
            if (bank, bit) in ZIELE:
                treffer += 1
                print(f"{name} {kind}{ri:02d} @0x{pc:05X} {L.NAMES[op]:4s} ({bank},{bit}) val={val}  "
                      f"bytes={d[pc:pc+sz].hex(' ')}")
print(f"# {treffer} Treffer")
