#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""gen_debug_jump_table.py — erzeugt include/debug_jump_table.h aus DEBUG.BIN.

SATZFORMAT, byte-gemessen (nicht aus einem Decompilat uebernommen):
    +0  u16  BELEGT-Flagge   (1 = im Menue anwaehlbar, 0 = wird uebersprungen)
    +2  s16  Spawn-X
    +4  s16  Spawn-Z
    +6  u8   Band            (+7 Fuellbyte)
    +8  14 B Name
    +22 4 B  Fuellbytes
  = 26 Byte = 13 Halbwoerter je Satz, 49 Saetze je Stage, 6 Stages.
Nachgerechnet an Satz 0: Datei-Offset 0x263A = `01 00 5a 55 a8 cb 00 00` + "BATH-LOCKERS".

ADRESSIERUNG: das Menue rechnet 637*stage + 13*idx HALBWOERTER ab 0x800C263A
(@0x8001476C-0x800147A8) — Tabellenanfang in DEBUG.BIN also 0x263A, der NAME liegt 8 Byte
weiter (der Anzeige-Zeiger des Originals ist 0x800C2642).

⛔ DIE BELEGT-PRUEFUNG IST DAS HALBWORT BEI +0, NICHT DER NAME:
    800147a8  lhu v0,0(at)              ; at = 0x800C263A + Satzversatz
    800147b0  beq v0,zero,0x800146e8    ; leer -> weiter
    800146e8  lbu v0,0(a1)              ; Stage
    80014704  addiu v1,v1,1             ; Raumindex dieser Stage ++
    80014714  sb   v1,0(at)             ; zurueckschreiben -> naechster Satz wird geprueft
Ein Satz mit Flagge 0 ist im Menue also NICHT erreichbar, auch wenn er einen Namen traegt.
Gemessene Folge: Spiel-STAGE6 (Tabellen-Stage 5) hat GENAU VIER anwaehlbare Eintraege —
GATE PLATFORM, MAIN PLATFORM, SHELTER 01-02, TRAIN MAIN CAR. "ENDING" (Satz 4) traegt
Flagge 0 und die Fuellkoordinaten `9a 99 eb f0` (= -26214/-3861), dieselben wie "OPENING"
(Stage 0, Satz 36) — beides sind unfertige Saetze.

⛔ FRUEHERER FEHLER, hiermit behoben: der erzeugte Header trug nur die NAMEN, und das Makro
RE15_DBG_SLOT_USED behalf sich mit `name[0] != ' '`. Das meldet "EMPTY"-Saetze und sogar
leere Namen als belegt — also das Gegenteil. Der Header traegt jetzt die echte Flagge.
Ausserdem hat dieses Skript seinen Header zuletzt gar nicht mehr geschrieben (es druckte nur
eine Zusammenfassung); das ist ebenfalls behoben.
"""
import io
import os
import sys

BASE, REC, SLOTS, STAGES = 0x263A, 26, 49, 6
NAME_OFF = 8

src = sys.argv[1] if len(sys.argv) > 1 else "info/Re1.5/PSX/BIN/DEBUG.BIN"
dst = sys.argv[2] if len(sys.argv) > 2 else "re15_port/include/debug_jump_table.h"

d = open(src, "rb").read()
need = BASE + STAGES * SLOTS * REC
if len(d) < need:
    raise SystemExit("%s ist zu klein: %d B, gebraucht %d" % (src, len(d), need))

names, flags = [], []
for st in range(STAGES):
    rn, rf = [], []
    for i in range(SLOTS):
        o = BASE + (st * 637 + i * 13) * 2
        rec = d[o:o + REC]
        rf.append(rec[0] | (rec[1] << 8))
        rn.append(rec[NAME_OFF:NAME_OFF + 14].split(b"\x00")[0]
                  .decode("ascii", "replace").strip())
    names.append(rn)
    flags.append(rf)

L = []
L.append("/* debug_jump_table.h — die JUMP-Liste des ORIGINAL-Debug-Menues, aus DEBUG.BIN erzeugt.")
L.append(" *")
L.append(" * ERZEUGT von tools/gen_debug_jump_table.py — NICHT von Hand editieren.")
L.append(" * Beleg-Block (Satzformat, Adressierung, Belegt-Pruefung) steht im Kopf des Generators.")
L.append(" *")
L.append(" * Satz = 26 B: +0 u16 BELEGT-Flagge, +2 s16 X, +4 s16 Z, +6 u8 Band, +8 14 B Name.")
L.append(" * Anwaehlbar ist ein Satz NUR bei Flagge != 0 (`lhu v0,0(at)` @0x800147A8 /")
L.append(" * `beq v0,zero,0x800146E8` @0x800147B0 -> Index++ und erneut pruefen).")
L.append(" */")
L.append("#ifndef RE15_DEBUG_JUMP_TABLE_H")
L.append("#define RE15_DEBUG_JUMP_TABLE_H")
L.append("")
L.append("#include <stdint.h>")
L.append("")
L.append("#define RE15_DBG_JUMP_STAGES %d" % STAGES)
L.append("#define RE15_DBG_JUMP_SLOTS  %d" % SLOTS)
L.append("")
L.append("static const char *const re15_dbg_jump_name[RE15_DBG_JUMP_STAGES][RE15_DBG_JUMP_SLOTS] = {")
for st in range(STAGES):
    L.append("    { /* Stage %d = Spiel-STAGE%d, %d anwaehlbar */"
             % (st, st + 1, sum(1 for f in flags[st] if f)))
    for i in range(0, SLOTS, 4):
        L.append("      " + " ".join('"%s",' % names[st][k] for k in range(i, min(i + 4, SLOTS))))
    L.append("    },")
L.append("};")
L.append("")
L.append("/* Die BELEGT-Flagge aus Satz+0 — das ist die Pruefung des Originals. */")
L.append("static const uint16_t re15_dbg_jump_flag[RE15_DBG_JUMP_STAGES][RE15_DBG_JUMP_SLOTS] = {")
for st in range(STAGES):
    L.append("    { /* Stage %d */" % st)
    for i in range(0, SLOTS, 8):
        L.append("      " + " ".join("%d," % flags[st][k] for k in range(i, min(i + 8, SLOTS))))
    L.append("    },")
L.append("};")
L.append("")
L.append("/* Belegt = Flagge != 0 (@0x800147A8/@0x800147B0). NICHT der Name: Saetze mit dem")
L.append(" * Namen \"EMPTY\" und sogar der Satz \"ENDING\" tragen Flagge 0 und sind unerreichbar. */")
L.append("#define RE15_DBG_SLOT_USED(st, idx) (re15_dbg_jump_flag[st][idx] != 0)")
L.append("")
L.append("/* Raumnummer aus Stage und Index (@0x8001D644 / @0x8001D660). */")
L.append("#define RE15_DBG_JUMP_ROOM(st, idx) ((uint16_t)((((st) + 1) << 8) | (idx)))")
L.append("")
L.append("#endif /* RE15_DEBUG_JUMP_TABLE_H */")

io.open(dst, "w", encoding="utf-8", newline="\n").write("\n".join(L) + "\n")
print("%s -> %s" % (src, dst))
print("anwaehlbare Saetze je Stage (Spiel-STAGE1..6): %s"
      % [sum(1 for f in r if f) for r in flags])
for st in range(STAGES):
    us = [names[st][i] for i in range(SLOTS) if flags[st][i]]
    print("  Spiel-STAGE%d (%2d): %s" % (st + 1, len(us), ", ".join(us) if len(us) <= 6
                                         else ", ".join(us[:6]) + " ..."))
