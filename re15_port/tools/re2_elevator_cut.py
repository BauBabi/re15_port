#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_elevator_cut.py — schneidet die ZWEI RE2-Fahrstuhl-SEs aus ROOM21B0.RDT
und baut daraus die Mini-VAB-Bank shared_assets/RE2/ELEVSE.VBS.

⛔ Der Ton ist eine RE2-ERGAENZUNG, KEIN RE1.5-Original.
   RE1.5 fuehrt in ROOM1080/1081/4020/4021 exakt dasselbe Fahrskript wie RE2 in
   ROOM21B0/ROOMB1B0 (32 bitgleiche Bytes, s.u.), aber ohne die zwei Se_on davor.

Quelle (NUR LESEN): info/re2leon/PL0/RDT/ROOM21B0.RDT
Gegenprobe:         info/re2leon/PL0/RDT/ROOMB1B0.RDT (dieselben zwei Wellen, bitgleich)

Alle Zahlen sind selbst nachgelesene DATEI-Byte-Offsets in ROOM21B0.RDT
(verifiziert 2026-09-26, Ausgabe des Selbsttests unten):

  RDT-Kopf 0x08/0x0C/0x10 :  EDT=0x5BC4  VH=0x5C84  VB=0x6AA4
  VH "pBAV" ver=7  nprog=2 ntone=18 nvag=18
  Se_on-Rohbytes          :  @0x2756  36 02 11 01 01 00 00 00 00 00 00 00
                             @0x2784  36 02 12 01 01 00 00 00 00 00 00 00
  EDT-Record se 0x11      :  @0x5C08  00 00 e3 00  -> prog0 tone14 prio3 voice-16 extra0
  EDT-Record se 0x12      :  @0x5C0C  00 00 f3 00  -> prog0 tone15 prio3 voice-16 extra0
  Tone prog0/14           :  @0x6664  32 B  vol90  pan64 center103 shift57 min=max73 vag16
  Tone prog0/15           :  @0x6684  32 B  vol110 pan64 center101 shift 0 min=max74 vag17
  VAG 16 (1-basiert)      :  @0x1BA34  16400 B  sha1 ac0b9a13a8806802575e60f37e9691ef7b1ef846
  VAG 17 (1-basiert)      :  @0x1FA44   6336 B  sha1 5a9aca0d211de60a78871e7bff63371333ab4a3c
  (Gegenprobe ROOMB1B0: VAG 6 @0x7D68 / VAG 7 @0xBD78, SHA-1 identisch.)

EINZIGE Daten-Aenderung beim Schnitt, und warum sie noetig ist:
  Die Mini-Bank traegt nur 2 Wellen, also lauten die 1-basierten VAG-Indizes darin 1 und 2.
  Tone-Byte +0x16 wird deshalb 16 -> 1 bzw. 17 -> 2 umgesetzt. Jedes andere Tone-Byte
  (vol/pan/center/shift/min/max/ADSR) wird BYTE-FUER-BYTE uebernommen, damit der Pitch
  identisch bleibt (761 / 861, s. Riegel).

Ausgabe:
  build/extracted/re2_elev/   — alle Zwischenstaende (roh geschnitten)
  re15_port/shared_assets/RE2/ELEVSE.VBS — die Auslieferungs-Bank

Satzformat von ELEVSE.VBS (dasselbe wie ein ENEMSE.VBS-Bank-Satz, audio_pc.c:1000-1007
"EDT-Record = [SE-Map @0 .. vh_off) [VAB-VH pBAV @vh_off] [Trailer], vh_off = u32
@edt[edt_size-8]" / FUN_8005a09c):
  0x0000  SE-Map, 0x13 Saetze a 4 B (Index 0..0x12), mit Nullen auf 0x50 aufgefuellt
  0x0050  VH "pBAV": 32 + 128*16 + 1*512 + 256*2 = 3104 B
  0x0C70  Trailer 8 B: u32 vh_off (=0x50), u32 0
  0x0C78  VBD: VAG1 (16400 B) + VAG2 (6336 B)
Die Groessen stehen als Konstanten in engine/src/gen/re15_elev_se.inc (vom selben Lauf
erzeugt), damit der Leser keinen neuen Parser braucht.
"""

import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))

SRC_A = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM21B0.RDT")
SRC_B = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOMB1B0.RDT")
OUT_TMP = os.path.join(REPO, "build", "extracted", "re2_elev")
OUT_VBS = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "ELEVSE.VBS")

# --- selbst nachgelesene Anker in ROOM21B0.RDT ------------------------------
SEON_OFF = {0x11: 0x2756, 0x12: 0x2784}          # Se_on-Rohbytes (12 B)
SEON_RAW = {0x11: bytes.fromhex("360211010100000000000000"),
            0x12: bytes.fromhex("360212010100000000000000")}
EDT_OFF = {0x11: 0x5C08, 0x12: 0x5C0C}
EDT_RAW = {0x11: bytes.fromhex("0000e300"), 0x12: bytes.fromhex("0000f300")}
TONE_OFF = {0x11: 0x6664, 0x12: 0x6684}          # prog0 tone14 / tone15
VAG_OFF = {0x11: 0x1BA34, 0x12: 0x1FA44}
VAG_LEN = {0x11: 16400, 0x12: 6336}
VAG_SHA = {0x11: "ac0b9a13a8806802575e60f37e9691ef7b1ef846",
           0x12: "5a9aca0d211de60a78871e7bff63371333ab4a3c"}
# Gegenprobe ROOMB1B0 (bitgleiche Wellen an anderer Stelle)
VAG_OFF_B = {0x11: 0x7D68, 0x12: 0xBD78}

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x50          # 0x13*4 = 76, auf 0x50 aufgefuellt
TRAILER = 8


def die(msg):
    sys.stderr.write("re2_elevator_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def main():
    if not os.path.isfile(SRC_A):
        die("Quelle fehlt: %s" % SRC_A)
    a = open(SRC_A, "rb").read()
    b = open(SRC_B, "rb").read() if os.path.isfile(SRC_B) else None

    edt, vh, vb = struct.unpack_from("<III", a, 8)
    if (edt, vh, vb) != (0x5BC4, 0x5C84, 0x6AA4):
        die("ROOM21B0 Kopf-Offsets weichen ab: EDT=0x%X VH=0x%X VB=0x%X" % (edt, vh, vb))
    if a[vh:vh + 4] != b"pBAV":
        die("kein pBAV @0x%X" % vh)

    os.makedirs(OUT_TMP, exist_ok=True)

    # --- 1. Selbsttest gegen die zitierten Bytes ---------------------------
    for sid in (0x11, 0x12):
        got = a[SEON_OFF[sid]:SEON_OFF[sid] + 12]
        if got != SEON_RAW[sid]:
            die("Se_on 0x%02X @0x%X weicht ab: %s" % (sid, SEON_OFF[sid], got.hex()))
        got = a[EDT_OFF[sid]:EDT_OFF[sid] + 4]
        if got != EDT_RAW[sid]:
            die("EDT 0x%02X @0x%X weicht ab: %s" % (sid, EDT_OFF[sid], got.hex()))
        wav = a[VAG_OFF[sid]:VAG_OFF[sid] + VAG_LEN[sid]]
        sha = hashlib.sha1(wav).hexdigest()
        if sha != VAG_SHA[sid]:
            die("VAG 0x%02X @0x%X sha1 %s != %s" % (sid, VAG_OFF[sid], sha, VAG_SHA[sid]))
        if b is not None:
            wb = b[VAG_OFF_B[sid]:VAG_OFF_B[sid] + VAG_LEN[sid]]
            if hashlib.sha1(wb).hexdigest() != VAG_SHA[sid]:
                die("ROOMB1B0-Gegenprobe 0x%02X weicht ab" % sid)
        open(os.path.join(OUT_TMP, "vag_%02X.vag" % sid), "wb").write(wav)
        open(os.path.join(OUT_TMP, "edt_%02X.bin" % sid), "wb").write(EDT_RAW[sid])
        tone = a[TONE_OFF[sid]:TONE_OFF[sid] + 32]
        open(os.path.join(OUT_TMP, "tone_%02X.bin" % sid), "wb").write(tone)
        print("  se 0x%02X: VAG @0x%X %d B sha1 %s  (ROOMB1B0 @0x%X bitgleich)"
              % (sid, VAG_OFF[sid], VAG_LEN[sid], sha, VAG_OFF_B[sid]))

    # --- 2. Mini-Bank bauen ------------------------------------------------
    se_map = bytearray(SE_MAP_BYTES)
    for sid in (0x11, 0x12):
        se_map[sid * 4:sid * 4 + 4] = EDT_RAW[sid]

    vh_out = bytearray(VH_SIZE)
    vh_out[0:4] = b"pBAV"
    struct.pack_into("<I", vh_out, 4, struct.unpack_from("<I", a, vh + 4)[0])   # version = 7
    struct.pack_into("<I", vh_out, 8, struct.unpack_from("<I", a, vh + 8)[0])   # bank id
    struct.pack_into("<I", vh_out, 12, VH_SIZE + VAG_LEN[0x11] + VAG_LEN[0x12])  # fsize
    struct.pack_into("<H", vh_out, 18, 1)    # nprog
    struct.pack_into("<H", vh_out, 20, 16)   # ntone (Tone-Segment ist immer 16 Saetze)
    struct.pack_into("<H", vh_out, 22, 2)    # nvag
    vh_out[24] = a[vh + 24]                  # master volume
    vh_out[25] = a[vh + 25]                  # master pan
    # Programm-Tabelle: Programm 0 byte-fuer-byte aus ROOM21B0 (mvol/mpan @+1/+4)
    vh_out[VAB_HDR:VAB_HDR + 16] = a[vh + VAB_HDR:vh + VAB_HDR + 16]
    # Tone-Segment Programm 0: alle 16 Saetze aus ROOM21B0 uebernehmen, damit die
    # Nachbar-Tones (Layer-Abbruch) genauso aussehen; nur die VAG-Indizes 16/17 -> 1/2.
    tone_base_src = vh + VAB_HDR + VAB_PROG_TABLE
    tone_base_dst = VAB_HDR + VAB_PROG_TABLE
    vh_out[tone_base_dst:tone_base_dst + VAB_TONE_SEG] = \
        a[tone_base_src:tone_base_src + VAB_TONE_SEG]
    for t in range(16):
        o = tone_base_dst + t * 32
        vag1 = struct.unpack_from("<H", vh_out, o + 0x16)[0]
        if t == 14 and vag1 == 16:
            struct.pack_into("<H", vh_out, o + 0x16, 1)
        elif t == 15 and vag1 == 17:
            struct.pack_into("<H", vh_out, o + 0x16, 2)
        else:
            struct.pack_into("<H", vh_out, o + 0x16, 0)   # kein Sample in dieser Mini-Bank
    # VAG-Groessentabelle: Eintrag 0 bleibt 0, dann 1-basiert die zwei Wellen (8-B-Einheiten)
    sz_base = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG
    struct.pack_into("<H", vh_out, sz_base + 2 * 1, VAG_LEN[0x11] // 8)
    struct.pack_into("<H", vh_out, sz_base + 2 * 2, VAG_LEN[0x12] // 8)

    vh_off = SE_MAP_BYTES
    trailer = struct.pack("<II", vh_off, 0)
    edt_rec = bytes(se_map) + bytes(vh_out) + trailer
    vbd = a[VAG_OFF[0x11]:VAG_OFF[0x11] + VAG_LEN[0x11]] + \
          a[VAG_OFF[0x12]:VAG_OFF[0x12] + VAG_LEN[0x12]]

    os.makedirs(os.path.dirname(OUT_VBS), exist_ok=True)
    open(os.path.join(OUT_TMP, "ELEVSE.VBS"), "wb").write(edt_rec + vbd)
    open(OUT_VBS, "wb").write(edt_rec + vbd)

    print("  ELEVSE.VBS: edt_size=%d (vh_off=0x%X) vbd_off=%d vbd_size=%d  gesamt=%d B"
          % (len(edt_rec), vh_off, len(edt_rec), len(vbd), len(edt_rec) + len(vbd)))
    print("  -> %s" % OUT_VBS)
    print("  -> %s (Zwischenstaende)" % OUT_TMP)
    return 0


if __name__ == "__main__":
    sys.exit(main())
