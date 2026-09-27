#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_re15_elev_anchors.py — erzeugt engine/src/gen/re15_elev_se.inc.

Die ANKER kommen aus den DATEN, nicht aus einer Raumnummer im Code.

Gesucht wird die 32-Byte-Folge, die in RE1.5 GENAU eine Fahrstuhl-Fahrt ist
(selbst gemessen 2026-09-26 ueber alle 240 RE1.5-RDTs und alle 250 RE2-RDTs):

  22 01 1c 01   Set   bank1 bit28 = 1     <- hier setzt RE2 Se_on(bank2, id 0x11)
  09 0a 08 00   Sleep 8
  22 01 1c 00   Set   bank1 bit28 = 0
  09 0a 5a 00   Sleep 90                  = DIE FAHRT
  22 01 1c 01   Set   bank1 bit28 = 1     <- hier setzt RE2 Se_on(bank2, id 0x12)
  09 0a 08 00   Sleep 8
  22 01 1c 00   Set   bank1 bit28 = 0
  09 0a 14 00   Sleep 20                  = ANKUNFT

Warum die VOLLEN 32 Byte und nicht 16: die zweite Haelfte allein
(`...09 0a 14 00`) steht auch in ROOM5090/5091/6030/6031 (RE1.5) und in
RE2 ROOM4080 — gemessen. Erst die zusammenhaengenden 32 Byte sind eindeutig:
4 Dateien, je 3 Treffer, 0 Fehltreffer in beiden Baeumen.

RE2-Beleg fuer die Zuordnung der beiden Ids (info/re2leon/PL0/RDT/ROOM21B0.RDT,
selbst gelesen): @0x2756 `36 02 11 01 01 00 00 00 00 00 00 00` steht unmittelbar
vor dem ersten Puls @0x2762, @0x2784 `36 02 12 ...` unmittelbar vor dem zweiten
@0x2790. Operanden-Layout: LAB_80041624 @0x80041644 `lbu a3,1(s0)` = bank,
@0x80041648 `lh a0,2(s0)` = id -> bank 2 = SND0, id 0x11 / 0x12.

⛔ Der Ton ist eine RE2-ERGAENZUNG. RE1.5 ist hier stumm (kein Se_on im Skript;
   die 11 bzw. 12 direkten SE-Emitter-Rufe in STAGE1.BIN/STAGE4.BIN sitzen
   ausschliesslich in Gegner-Zustandsroutinen, und die vier Fahrstuhlraeume
   spawnen keinen einzigen Gegner — Details im Kopf von scd_elev_se.c).

=== WELLE 2 (2026-09-27): DIE ZWEITE FAHRT-GESTALT ==========================

Nutzer-Auftrag woertlich: "fahrstuhl sound muss ueberall bei fahrstuehlen rein.
Der fehlt weil REsident Evil 1.5 eine 40% Beta ist und unvollstaendig."

RE1.5 hat DREI Fahrstuhlkabinen = SECHS RDTs, nicht zwei:
  1) ROOM1080/1081  STAGE1 "ELEVATOR"        3 Fahrten je Datei  -> SIG1
  2) ROOM3080/3081  STAGE3 "WAREHOUSE LIFT"  2 Fahrten je Datei  -> SIG2  (NEU)
  3) ROOM4020/4021  STAGE4 "A-2 ELEVATOR"    3 Fahrten je Datei  -> SIG1

Die WAREHOUSE LIFT benutzt ein ANDERES Fahrskript und faellt darum durch SIG1.
Bytes selbst gelesen (ROOM3080.RDT @0x09FE):

  22 01 1c 01   Set   bank1 bit28 = 1     <- Puls 1 = Fahrt   (Se_on id 0x11)
  09 0a 3c 00   Sleep 60
  22 01 1c 00   Set   bank1 bit28 = 0
  22 01 1d 01   Set   bank1 bit29 = 1     <- Puls 2 = Ankunft (Se_on id 0x12)
  09 0a 3c 00   Sleep 60
  22 01 1d 00   Set   bank1 bit29 = 0
  22 01 1c 01   Set   bank1 bit28 = 1

Unterschiede zu SIG1: Schlafzeiten 60/60 statt 8/90/8/20, und ein ZWEITES Bit
(0x1D neben 0x1C). Deshalb greift SIG1 dort nicht (gemessen: 0 Treffer).

EINDEUTIGKEIT GEMESSEN, nicht behauptet — dieser Generator scannt beide
Signaturen ueber ALLE RDTs und bricht ab, wenn eine Signatur ausserhalb der
erwarteten Dateien trifft:
  SIG1 (32 Byte): 12 Treffer in 4 Dateien
  SIG2 (28 Byte):  4 Treffer in 2 Dateien (ROOM3080 @0x09FE/@0x0BDA,
                                           ROOM3081 @0x07FE/@0x0842)
  0 Fehltreffer.

Der Ton fehlt auch in ROOM3080 im ORIGINAL: die drei Se_on des Raumes
(@0x0095C/@0x00970/@0x00984, `36 02 0a ..` = Bank 2 Id 0x0A) liegen rund 0xA0
Byte VOR der Fahrt, in einem anderen Abschnitt. Wozu sie gehoeren, ist nicht
bestimmt — nur, dass sie NICHT in der Fahrt liegen.

Nebenbefund zum Anker: bank1 bit0x1C/0x1D ist die Fahrstuhl-BELEUCHTUNG, nicht
"Fahrstuhl faehrt" (ROOM6030 verschraenkt dieselben Pulse mit Cut_chg 4/5
@0x01214-0x01250 und hat @0x01004 eine Blink-Schleife). Die Signaturen sind also
der Flacker-Rhythmus einer Fahrt — gemessen eindeutig, aber keine Semantik.
"""

import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
ASSETS = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
OUT = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re15_elev_se.inc")

SE_FIRST = 0x11     # Puls 1 -> Fahrgeraeusch
SE_SECOND = 0x12    # Puls 2 -> Ankunft

# (Name, Bytes, Offset des ZWEITEN Pulses, erwartete Dateien)
SIGS = [
    ("SIG1 ELEVATOR / A-2 ELEVATOR",
     bytes.fromhex("22011c01090a080022011c00090a5a00"
                   "22011c01090a080022011c00090a1400"),
     0x10,
     ("ROOM1080.RDT", "ROOM1081.RDT", "ROOM4020.RDT", "ROOM4021.RDT")),
    ("SIG2 WAREHOUSE LIFT",
     bytes.fromhex("22011c01090a3c0022011c00"
                   "22011d01090a3c0022011d0022011c01"),
     0x0C,
     ("ROOM3080.RDT", "ROOM3081.RDT")),
]
SIG_MAX = max(len(s) for _n, s, _d, _f in SIGS)

def main():
    hits = []
    n_rdt = 0
    fremd = []
    for dp, _dn, fn in os.walk(ASSETS):
        for f in sorted(fn):
            if not f.upper().endswith(".RDT"):
                continue
            n_rdt += 1
            p = os.path.join(dp, f)
            d = open(p, "rb").read()
            for si, (_name, sig, _delta, erwartet) in enumerate(SIGS):
                i = d.find(sig)
                while i >= 0:
                    try:
                        room = int(f[4:8], 16)
                    except ValueError:
                        room = 0
                    hits.append((room, i, f, si))
                    if f.upper() not in erwartet:
                        fremd.append((f, i, si))
                    i = d.find(sig, i + 1)
    if fremd:
        print("FEHLTREFFER (Signatur ausserhalb der erwarteten Dateien): %r" % (fremd,))
        return 1
    hits.sort()
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    L = []
    L.append("/* AUTO-GENERATED by re15_port/tools/gen_re15_elev_anchors.py -- DO NOT EDIT.")
    L.append(" * Quelle: alle ROOM*.RDT unter re15_port/shared_assets/PSX (%d RDTs durchsucht)." % n_rdt)
    L.append(" * Siehe den Generator-Kopf fuer die Belege. */")
    L.append("")
    L.append("/* Die Fahrt-Gestalten. RE1.5 hat DREI Fahrstuhlkabinen mit ZWEI verschiedenen")
    L.append(" * Fahrskripten; jede Gestalt traegt den Byte-Offset ihrer beiden Pulse. */")
    L.append("#define RE15_ELEV_SIG_MAX %d" % SIG_MAX)
    L.append("#define RE15_ELEV_SIG_COUNT %d" % len(SIGS))
    L.append("typedef struct {")
    L.append("    unsigned char len;        /* Laenge der Signatur in Byte    */")
    L.append("    unsigned char delta2;     /* Byte-Offset des ZWEITEN Pulses */")
    L.append("    unsigned char bytes[RE15_ELEV_SIG_MAX];")
    L.append("} re15_elev_sig_t;")
    L.append("static const re15_elev_sig_t s_re15_elev_sigs[RE15_ELEV_SIG_COUNT] = {")
    for name, sig, delta, _f in SIGS:
        L.append("    {   /* %s */" % name)
        L.append("        %d, 0x%02X, {" % (len(sig), delta))
        for r in range(0, len(sig), 8):
            L.append("        " + ", ".join("0x%02x" % c for c in sig[r:r + 8]) + ",")
        L.append("    } },")
    L.append("};")
    L.append("")
    L.append("/* Puls -> RE2-SE-Id (ROOM21B0.RDT @0x2756 / @0x2784). */")
    L.append("#define RE15_ELEV_SE_FIRST  0x%02X   /* Puls 1 = Fahrt   */" % SE_FIRST)
    L.append("#define RE15_ELEV_SE_SECOND 0x%02X   /* Puls 2 = Ankunft */" % SE_SECOND)
    L.append("")
    L.append("/* GEMESSENE Fundstellen - nur fuer den Riegel (tests), nicht fuer die Laufzeit:")
    L.append(" * die Laufzeit sucht die Signaturen im geladenen RDT-Puffer selbst. */")
    L.append("#define RE15_ELEV_HIT_COUNT %d" % len(hits))
    L.append("typedef struct { unsigned short room; unsigned int off; unsigned char sig; }"
             " re15_elev_hit_t;")
    L.append("static const re15_elev_hit_t s_re15_elev_hits[RE15_ELEV_HIT_COUNT] = {")
    for room, off, f, si in hits:
        L.append("    { 0x%04X, 0x%04X, %d },   /* %s */" % (room, off, si, f))
    L.append("};")
    with open(OUT, "w", newline="\n") as o:
        o.write("\n".join(L) + "\n")
    print("%d RDTs durchsucht, %d Fundstellen -> %s" % (n_rdt, len(hits), OUT))
    for room, off, f, si in hits:
        print("  %s  0x%04X  SIG%d" % (f, off, si + 1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
