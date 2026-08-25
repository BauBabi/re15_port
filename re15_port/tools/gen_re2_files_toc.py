#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""gen_re2_files_toc.py — erzeugt engine/src/gen/re2_files_toc.inc aus der RE2-PSX.EXE.

Die drei Tabellen sind selbst nachgerechnet (nicht aus einem Decompilat uebernommen):

  0x800a9ad0  Dokument -> erster Slot, 25 x u8   (`lbu a0,-25904(at)` @0x8006d480)
              gelesen: {0,7,14,17,21,32,43,54,62,68,76,79,83,88,92,97,107,115,126,133,
                        136,144,151,157,174}, danach 0x00 = Ende -> genau 25 Dokumente.
  0x800aa144  Dokument-Record, 25 x 4 B {u16 max_page; u8 y_off; u8 pad}
              max_page `lhu s4,-24252(at)` @0x80076224
              y_off    `lhu s3,-24250(at)` @0x8007603c, dann
                       `addiu s1,zero,256` @0x80076040 / `subu s5,s1,s3` @0x80076044
              => Seitenhoehe H = 256 - y_off, gemessen eines von 112/128/144/176.
  0x800a94b4  Slot-Record, 191 x 8 B {u32 groesse; u16 sektor_lo; u8 sektor_hi; u8 pad}
              groesse `lw v0,-27468(at)` @0x8006d4d8
              sektor  `lhu a0,-27464(at)` @0x8006d518 / `lbu v0,-27462(at)` @0x8006d50c
              Byte-Offset in FILES.TIM = sektor * 0x800.

  Seite -> Slot (`beq v1,zero` @0x8006d484, `addiu v0,a0,1` @0x8006d488):
      Titelseite  : slot = first[doc]
      Folgeseite p: slot = first[doc] + 1 + p

⛔ max_page ist KEIN fester Wert: der Rookie-Sonderfall patcht ihn zur Laufzeit
(`sh 1,-24160(at)` @0x80068368 schreibt nach 0x800aa1a0 = Record 23). Der Port muss die
Seitenzahl je Dokument deshalb als VERAENDERLICHE fuehren. Fuer die Dokumente 23 und 24
liegen ohnehin 17 Slots vor, waehrend der Record nur 3 Seiten meldet.
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
OUT = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re2_files_toc.inc")

DOC_TAB, CNT_TAB, REC_TAB = 0x800A9AD0, 0x800AA144, 0x800A94B4
N_DOC, N_SLOT = 25, 191


def main():
    d = open(EXE, "rb").read()
    taddr = struct.unpack_from("<I", d, 0x18)[0]

    def fo(a):
        return 0x800 + (a - taddr)

    first = [d[fo(DOC_TAB) + i] for i in range(N_DOC)]
    assert d[fo(DOC_TAB) + N_DOC] == 0, "Tabelle endet nicht bei 25"
    meta = []
    for i in range(N_DOC):
        mp, yo = struct.unpack_from("<HB", d, fo(CNT_TAB + i * 4))[0:2]
        assert 256 - yo in (112, 128, 144, 176), "unerwartete Seitenhoehe %d" % (256 - yo)
        meta.append((mp, 256 - yo))
    slots = []
    for i in range(N_SLOT):
        o = fo(REC_TAB + i * 8)
        size = struct.unpack_from("<I", d, o)[0]
        sect = struct.unpack_from("<H", d, o + 4)[0] | (d[o + 6] << 16)
        slots.append((size, sect * 0x800))

    L = []
    L.append("/* GENERIERT von re15_port/tools/gen_re2_files_toc.py - NICHT von Hand aendern.")
    L.append(" * Quelle: info/re2leon/PSX.EXE, Tabellen @0x800A9AD0 / @0x800AA144 / @0x800A94B4.")
    L.append(" * Belege stehen im Kopf des Generators. */")
    L.append("")
    L.append("#define RE2_FILES_DOC_COUNT  %d" % N_DOC)
    L.append("#define RE2_FILES_SLOT_COUNT %d" % N_SLOT)
    L.append("")
    L.append("/* Dokument -> erster Slot (@0x800A9AD0, gelesen @0x8006d480) */")
    L.append("static const unsigned char re2_files_first_slot[RE2_FILES_DOC_COUNT] = {")
    L.append("    " + ", ".join(str(v) for v in first))
    L.append("};")
    L.append("")
    L.append("/* Dokument-Record (@0x800AA144): Startwert der Seitenzahl + Seitenhoehe.")
    L.append(" * max_page ist ein RAM-Wert und darf wachsen (Rookie-Patch @0x80068368). */")
    L.append("typedef struct { unsigned short max_page; unsigned short page_h; } re2_files_doc_t;")
    L.append("static const re2_files_doc_t re2_files_doc[RE2_FILES_DOC_COUNT] = {")
    for i, (mp, h) in enumerate(meta):
        L.append("    { %3d, %3d },   /* Dokument %2d */" % (mp, h, i))
    L.append("};")
    L.append("")
    L.append("/* Slot-Record (@0x800A94B4): Groesse + Byte-Offset in FILES.TIM. */")
    L.append("typedef struct { unsigned int size; unsigned int off; } re2_files_slot_t;")
    L.append("static const re2_files_slot_t re2_files_slot[RE2_FILES_SLOT_COUNT] = {")
    for i, (sz, off) in enumerate(slots):
        L.append("    { %6d, 0x%06X },   /* Slot %3d */" % (sz, off, i))
    L.append("};")
    L.append("")
    L.append("/* Seite -> Slot: Titelseite = page < 0 (@0x8006d484-98). */")
    L.append("static int re2_files_slot_for(int doc, int page)")
    L.append("{")
    L.append("    if (doc < 0 || doc >= RE2_FILES_DOC_COUNT) return -1;")
    L.append("    int s = (int)re2_files_first_slot[doc] + (page < 0 ? 0 : 1 + page);")
    L.append("    return (s >= 0 && s < RE2_FILES_SLOT_COUNT) ? s : -1;")
    L.append("}")
    L.append("")

    outdir = os.path.dirname(OUT)
    if not os.path.isdir(outdir):
        os.makedirs(outdir)
    open(OUT, "w", newline="\n").write("\n".join(L))
    print("geschrieben: %s (%d Dokumente, %d Slots)" % (OUT, N_DOC, N_SLOT))


if __name__ == "__main__":
    main()
