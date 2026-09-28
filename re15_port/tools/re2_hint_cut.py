#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_hint_cut.py - schneidet den RE2-KARTENHINWEIS-Ton aus ROOM3010.RDT und baut
daraus die Mini-VAB-Bank HINTSE.VBS (Satzformat wie ELEVSE.VBS, tools/re2_elevator_cut.py).

Der Ton ist eine RE2-ERGAENZUNG, KEIN RE1.5-Original: RE1.5 hat keinen Kartenhinweis
(SCD-Tabelle endet bei 0x5E, RE2s Hinweis-Opcode ist 0x84), und die Welle liegt in
keiner RE1.5-Datei (analysis/befunde_runde30/tools/r30_karte3010_welle_suche.py:
3193 Dateien, 0 Treffer). Dossier: analysis/befunde_runde30/karte-3010.md.

RE2-Beleg: FUN_8006F1C4 @0x8006F234 "lui a0,0x22b" / @0x8006F238 "jal 0x8005ba28"
= Se_on(Bank 2 = Raumbank, Satz 0x2B).

Quelle (NUR LESEN): info/re2leon/PL0/RDT/ROOM3010.RDT
Gegenproben:        ROOM3040 / ROOM30B0 / ROOM6030 (dieselbe Welle, sha1 bitgleich)

Selbst nachgelesene DATEI-Byte-Offsets in ROOM3010.RDT:
  RDT-Kopf 0x08/0x0C/0x10 : EDT=0x1F778  VH=0x1F838  VB=0x20658
  SCD-Record              : @0x026EE  84 02   (Hinweis 2), danach 01 00 = Evt_end
  EDT-Record se 0x2B      : @0x1F824  00 01 23 00 -> Programm 1, Ton 2, Prio 3
  Tone prog1/2            : @0x20298  vol80 pan64 center85 shift0 min=max61 vag18
  VAG 18 (1-basiert)      : @0x39788  4480 B  sha1 eb386970f9a996369889d101b68314681138ff99

EINZIGE Daten-Aenderung beim Schnitt: die Mini-Bank traegt nur EINE Welle, ihr
1-basierter VAG-Index ist 1. Tone-Byte +0x16 des Tons prog1/2 wird 18 -> 1 gesetzt,
alle anderen Tone-VAG-Indizes auf 0 (keine Welle in dieser Bank). Beide
Programm-Eintraege und beide Tone-Segmente werden sonst BYTE-FUER-BYTE uebernommen,
damit der EDT-Satz "00 01 23 00" unveraendert gilt (Programm 1 bleibt Programm 1).

Satzformat HINTSE.VBS:
  0x0000  SE-Map, 0x2C Saetze a 4 B (Index 0..0x2B) = 0xB0 Byte, nur Satz 0x2B belegt,
          alle anderen 00 00 00 00 (wie ELEVSE.VBS; re15_edt_decode meldet dafuer
          rec.empty, der Port spielt dann nichts)
  0x00B0  VH "pBAV": 32 + 128*16 + 2*512 + 256*2 = 3616 B
  0x0ED0  Trailer 8 B: u32 vh_off (=0xB0), u32 0
  0x0ED8  VBD: VAG1 (4480 B)

Aufruf:
  re2_hint_cut.py             -> build/r30_karte-3010/HINTSE.VBS + re2_hint_bank.inc
  re2_hint_cut.py --install   -> re15_port/shared_assets/RE2/HINTSE.VBS +
                                 re15_port/engine/src/gen/re2_hint_bank.inc  (BAU-Phase)
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
SRC = os.path.join(RDT, "ROOM3010.RDT")

SE_ID = 0x2B
HDR = (0x1F778, 0x1F838, 0x20658)
SCD_OFF, SCD_RAW = 0x026EE, bytes.fromhex("84020100")
EDT_OFF, EDT_RAW = 0x1F824, bytes.fromhex("00012300")
TONE_OFF = 0x20298
TONE_HEAD = bytes.fromhex("0000504055003d3d")
VAG_OFF, VAG_LEN = 0x39788, 4480
VAG_SHA = "eb386970f9a996369889d101b68314681138ff99"
# Gegenproben: (Datei, EDT-Satz-Offset, VAG-Offset)
GEGEN = [("ROOM3040.RDT", 0x1E850, 0x2CAA4),
         ("ROOM30B0.RDT", 0x1C43C, 0x2F790),
         ("ROOM6030.RDT", 0x0E1D4, 0x28668)]

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
NPROG = 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + NPROG * VAB_TONE_SEG + VAB_SIZE_TABLE
SE_MAP_BYTES = (SE_ID + 1) * 4
TRAILER = 8


def die(msg):
    sys.stderr.write("re2_hint_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def main():
    install = "--install" in sys.argv
    if not os.path.isfile(SRC):
        die("Quelle fehlt: %s" % SRC)
    a = open(SRC, "rb").read()

    # --- 1. Selbsttest gegen die zitierten Bytes ---------------------------
    if struct.unpack_from("<III", a, 8) != HDR:
        die("Kopf-Offsets weichen ab: %s" % (struct.unpack_from("<III", a, 8),))
    edt, vh, vb = HDR
    if a[vh:vh + 4] != b"pBAV":
        die("kein pBAV @0x%X" % vh)
    if struct.unpack_from("<H", a, vh + 18)[0] != NPROG:
        die("nprog != %d" % NPROG)
    if a[SCD_OFF:SCD_OFF + 4] != SCD_RAW:
        die("SCD-Record @0x%X weicht ab: %s" % (SCD_OFF, a[SCD_OFF:SCD_OFF + 4].hex()))
    if a[EDT_OFF:EDT_OFF + 4] != EDT_RAW:
        die("EDT-Satz @0x%X weicht ab" % EDT_OFF)
    if EDT_OFF != edt + SE_ID * 4:
        die("EDT-Satz-Offset passt nicht zum Kopf")
    prog = EDT_RAW[1] & 0x7F
    tone = EDT_RAW[2] >> 4
    if TONE_OFF != vh + VAB_HDR + VAB_PROG_TABLE + prog * VAB_TONE_SEG + tone * 32:
        die("Tone-Offset passt nicht zu Programm %d Ton %d" % (prog, tone))
    t = a[TONE_OFF:TONE_OFF + 32]
    if t[:8] != TONE_HEAD:
        die("Tone @0x%X weicht ab: %s" % (TONE_OFF, t[:8].hex()))
    vag1 = struct.unpack_from("<H", t, 0x16)[0]
    sz_tab = vh + VAB_HDR + VAB_PROG_TABLE + NPROG * VAB_TONE_SEG
    off = sum(struct.unpack_from("<H", a, sz_tab + i * 2)[0] * 8 for i in range(1, vag1))
    size = struct.unpack_from("<H", a, sz_tab + vag1 * 2)[0] * 8
    if (vb + off, size) != (VAG_OFF, VAG_LEN):
        die("VAG %d liegt @0x%X (%d B), erwartet @0x%X (%d B)" % (vag1, vb + off, size, VAG_OFF, VAG_LEN))
    wav = a[VAG_OFF:VAG_OFF + VAG_LEN]
    sha = hashlib.sha1(wav).hexdigest()
    if sha != VAG_SHA:
        die("VAG sha1 %s != %s" % (sha, VAG_SHA))
    print("  se 0x%02X: EDT @0x%X %s -> prog %d tone %d; Tone @0x%X vol%d pan%d center%d shift%d "
          "min%d max%d vag%d" % (SE_ID, EDT_OFF, EDT_RAW.hex(" "), prog, tone, TONE_OFF,
                                 t[2], t[3], t[4], t[5], t[6], t[7], vag1))
    print("  VAG %d @0x%X %d B sha1 %s" % (vag1, VAG_OFF, VAG_LEN, sha))
    for name, e_off, v_off in GEGEN:
        p = os.path.join(RDT, name)
        if not os.path.isfile(p):
            print("  Gegenprobe %s: Datei fehlt (uebersprungen)" % name)
            continue
        b = open(p, "rb").read()
        be = struct.unpack_from("<I", b, 8)[0]
        if e_off != be + SE_ID * 4:
            die("%s: EDT-Satz-Offset passt nicht" % name)
        if hashlib.sha1(b[v_off:v_off + VAG_LEN]).hexdigest() != VAG_SHA:
            die("%s: Welle @0x%X weicht ab" % (name, v_off))
        print("  Gegenprobe %s: EDT[0x2B] @0x%X = %s, Welle @0x%X bitgleich"
              % (name, e_off, b[e_off:e_off + 4].hex(" "), v_off))

    # --- 2. Mini-Bank bauen ------------------------------------------------
    se_map = bytearray(SE_MAP_BYTES)
    se_map[SE_ID * 4:SE_ID * 4 + 4] = EDT_RAW

    vh_out = bytearray(VH_SIZE)
    vh_out[0:4] = b"pBAV"
    struct.pack_into("<I", vh_out, 4, struct.unpack_from("<I", a, vh + 4)[0])   # version
    struct.pack_into("<I", vh_out, 8, struct.unpack_from("<I", a, vh + 8)[0])   # bank id
    struct.pack_into("<I", vh_out, 12, VH_SIZE + VAG_LEN)                       # fsize
    struct.pack_into("<H", vh_out, 18, NPROG)                                   # nprog
    struct.pack_into("<H", vh_out, 20, struct.unpack_from("<H", a, vh + 20)[0]) # ntone
    struct.pack_into("<H", vh_out, 22, 1)                                       # nvag
    vh_out[24] = a[vh + 24]
    vh_out[25] = a[vh + 25]
    # Programm-Tabelle (128 Eintraege) und beide Tone-Segmente byte-fuer-byte
    vh_out[VAB_HDR:VAB_HDR + VAB_PROG_TABLE] = a[vh + VAB_HDR:vh + VAB_HDR + VAB_PROG_TABLE]
    tb_src = vh + VAB_HDR + VAB_PROG_TABLE
    tb_dst = VAB_HDR + VAB_PROG_TABLE
    vh_out[tb_dst:tb_dst + NPROG * VAB_TONE_SEG] = a[tb_src:tb_src + NPROG * VAB_TONE_SEG]
    geaendert = 0
    for k in range(NPROG * 16):
        o = tb_dst + k * 32
        alt = struct.unpack_from("<H", vh_out, o + 0x16)[0]
        neu = 1 if k == prog * 16 + tone else 0
        if alt != neu:
            geaendert += 1
        struct.pack_into("<H", vh_out, o + 0x16, neu)
    sz_base = tb_dst + NPROG * VAB_TONE_SEG
    struct.pack_into("<H", vh_out, sz_base + 2, VAG_LEN // 8)

    vh_off = SE_MAP_BYTES
    edt_rec = bytes(se_map) + bytes(vh_out) + struct.pack("<II", vh_off, 0)
    vbd = wav
    blob = edt_rec + vbd

    if install:
        out_vbs = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "HINTSE.VBS")
        out_inc = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re2_hint_bank.inc")
    else:
        out_vbs = os.path.join(REPO, "build", "r30_karte-3010", "HINTSE.VBS")
        out_inc = os.path.join(REPO, "build", "r30_karte-3010", "re2_hint_bank.inc")
    os.makedirs(os.path.dirname(out_vbs), exist_ok=True)
    os.makedirs(os.path.dirname(out_inc), exist_ok=True)
    open(out_vbs, "wb").write(blob)
    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/re2_hint_cut.py -- DO NOT EDIT.\n"
                " * Satz-TOC der Mini-Bank shared_assets/RE2/HINTSE.VBS (Satzformat wie\n"
                " * ELEVSE.VBS): [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off\n"
                " * @size-8][VBD]. Quelle: RE2 ROOM3010.RDT, EDT[0x2B] @0x1F824, Tone @0x20298,\n"
                " * VAG 18 @0x39788 (4480 B, sha1 eb386970...). */\n")
        o.write("#define RE2_HINT_EDT_OFF   %du\n" % 0)
        o.write("#define RE2_HINT_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_HINT_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_HINT_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_HINT_VAG1_SIZE %du\n" % VAG_LEN)
        o.write("/* Der Satz, den RE2 im Hinweis-Zeichner spielt: lui a0,0x22b @0x8006F234. */\n")
        o.write("#define RE2_HINT_SE        0x2B\n")
    print("  HINTSE.VBS: edt_size=%d (vh_off=0x%X) vbd_off=%d vbd_size=%d gesamt=%d B  md5 %s"
          % (len(edt_rec), vh_off, len(edt_rec), len(vbd), len(blob), hashlib.md5(blob).hexdigest()))
    print("  geaenderte Tone-VAG-Indizes: %d von %d (Ziel-Ton 18 -> 1, alle anderen -> 0)"
          % (geaendert, NPROG * 16))
    print("  -> %s" % out_vbs)
    print("  -> %s" % out_inc)
    return 0


if __name__ == "__main__":
    sys.exit(main())
