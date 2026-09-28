#!/usr/bin/env python3
"""Ist der Birkin-Modul (Typ 0x30) in STAGE5.BIN derselbe Code wie in STAGE3.BIN, nur verschoben?

Belege der Registrierung (Overlay-Installer, RE_15_Quellcode_Overlays):
  STAGE3_overlay.c: _DAT_80072c6c = 0x80116230 (Wurzel)  / _DAT_800ac818 = 0x8011a7c4 (Opfer-FSM)
  STAGE5_overlay.c: _DAT_80072c6c = 0x80116a44 (Wurzel)  / _DAT_800ac818 = 0x8011afd8 (Opfer-FSM)
  Differenz beider Paare = 0x814.
Overlay-Dateien liegen RAW ab 0x80100000 (kein 0x800-Kopf, Skill re15-psx-disasm).
Vergleich Wort fuer Wort ueber [start, ende) gegen [start+0x814, ende+0x814); ein abweichendes Wort
gilt als VERSCHIEBUNG, wenn es ein jal/j ist, dessen Ziel sich um genau 0x814 unterscheidet, oder ein
lui/addiu/ori/lw/sw/lh/sh/lb/sb/lbu/lhu mit gleichem Opcode+Registern (Adress-Immediate). Rest = echt."""
import os, sys, struct
WURZEL = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
def lade(n):
    return open(os.path.join(WURZEL, "info", "Re1.5", "PSX", "BIN", n), "rb").read()
s3, s5 = lade("STAGE3.BIN"), lade("STAGE5.BIN")
def wort(d, a): return struct.unpack_from("<I", d, a - 0x80100000)[0]
start = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0x80116230
ende  = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x8011b4d0
DELTA = 0x814
gleich = versch = echt = 0
beispiele = []
for a in range(start, ende, 4):
    w3, w5 = wort(s3, a), wort(s5, a + DELTA)
    if w3 == w5:
        gleich += 1; continue
    op3, op5 = w3 >> 26, w5 >> 26
    if op3 == op5 and op3 in (2, 3):                       # j / jal
        t3 = ((w3 & 0x3ffffff) << 2) | 0x80000000
        t5 = ((w5 & 0x3ffffff) << 2) | 0x80000000
        if t5 - t3 == DELTA: versch += 1; continue
    if op3 == op5 and (w3 >> 16) == (w5 >> 16):            # gleicher Opcode+rs+rt, anderes Immediate
        versch += 1; continue
    if op3 == op5 == 0 and (w3 & 0x3f) == (w5 & 0x3f) == 0x08:  # jr (Sprungtabellen-Register)
        versch += 1; continue
    echt += 1
    if len(beispiele) < 12: beispiele.append((a, w3, w5))
print(f"Bereich 0x{start:08x}..0x{ende:08x} (STAGE3) gegen +0x{DELTA:x} (STAGE5): "
      f"{(ende-start)//4} Worte, gleich {gleich}, Verschiebung {versch}, ECHT verschieden {echt}")
for a, w3, w5 in beispiele:
    print(f"  0x{a:08x}: {w3:08x} vs 0x{a+DELTA:08x}: {w5:08x}")
