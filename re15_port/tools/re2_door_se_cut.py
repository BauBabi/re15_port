#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_door_se_cut.py — schneidet die RE2-"Tuer verschlossen"-Wellen aus den RE2-Raumbaenken
und baut daraus die Mini-VAB-Bank TUERSE.VBS (Satzformat wie ELEVSE.VBS).

⛔ Die Toene sind eine RE2-ERGAENZUNG, KEIN RE1.5-Original. RE1.5s Tuer-/Text-Handler
   spielen nichts (LAB_80043084 @0x80043084 ruft nur FUN_80027e68; LAB_800430bc @0x800430bc
   hat keinen Schloss-Test; kein `jal 0x80045024` zwischen 0x80042bac und 0x800437ff).
   Dossier: analysis/befunde_runde30/tuer-verschlossen.md

RE2-VORBILD (alles selbst disassembliert / aus den Bytes gelesen):
  EXE-Tuer-Handler PTR_800a73c4[1] = 0x80051514:
    @0x800515a8 lbu a1,15(s2)        key_id;  @0x800515b0 andi v0,a1,0x80  = verschlossen
    @0x800515d0 lbu s0,16(s2)        key_type
    @0x8005160c lui a0,0x216 / @0x80051610 jal 0x8005ba28   key_type 0xFF (von innen verriegelt)
    @0x800516a0 lui a0,0x216 / @0x800516a4 jal 0x8005ba28   Schluessel fehlt
    => Se_on(0x02160000): Bank 2 = RAUMBANK (FUN_80059e54: DAT_800dbb80 = [RDT+8]), Satz 0x16
  Raumskripte spielen DENSELBEN Satz, z.B. ROOM2110.RDT sub09 @Datei 0x01BBC
    `36 02 16 00 00 00 44 a4 f8 f8 93 cc` direkt hinter Message_on 6 @0x01BB6
    ("It's electronically locked. There's a card reader on the left.")
  Der Satz 0x16 traegt JE RAUM eine andere Welle. Huellkurven-Messung
  (analysis/befunde_runde30/tools/r30_re2_wellen_familien.py) ergibt drei Geraeusche, die in
  Schloss-Raeumen vorkommen; je eines wird hier in bester Abtastrate geschnitten:

  Bank-Satz  RE2-Quelle            Welle (sha1)   Bytes   wofuer RE2 sie nimmt
  0  ZU_A    ROOM1140 Satz 0x16    cf1414572aea    7184   Polizeirevier, 9 Raeume
  1  ZU_B    ROOM1050 Satz 0x16    60e753ac5e56    3232   Polizeirevier, 6 Raeume
  2  ZU_E    ROOM2110 Satz 0x16    be2f6ea9caaa   10272   Kartenleser-Tuer + Kanal/Fabrik/Labor

Mit --mit-aufschliessen kommen zwei weitere Saetze dazu (NICHT beauftragt, nur vorbereitet):
  3  AUF_KEY ROOM1140 Satz 0x25    e12a441e5282    5264   @0x80051658 Schluessel benutzt
  4  AUF_HIER ROOM20A0 Satz 0x26   d1d99d659a37    7568   @0x800515f8 von dieser Seite entriegelt

EINZIGE Daten-Aenderungen beim Schnitt, und warum:
  (1) Tone-Byte +0x16 (VAG-Index, 1-basiert) wird auf die Lage in der Mini-Bank umgesetzt.
  (2) EDT-Byte 2, oberes Nibble (Tone-Index im Programm) wird auf die Lage in der Mini-Bank
      umgesetzt; unteres Nibble (Prioritaet) und Byte 3 (Stimme/Zusatzlagen) bleiben.
  Jedes andere Byte (vol/pan/center/shift/min/max/ADSR, die Welle selbst) ist bytegleich.

Ausgabe (Standard, NUR Ermittlungsstand — fasst weder shared_assets noch engine an):
  build/r30_tuer-verschlossen/bank/TUERSE.VBS
  build/r30_tuer-verschlossen/bank/re2_door_bank.inc
Mit --install (Bau-Agent):
  re15_port/shared_assets/RE2/TUERSE.VBS
  re15_port/engine/src/gen/re2_door_bank.inc
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

# (name, raum, re2-satz, erwarteter EDT-Rohsatz, EDT-Datei-Offset, Tone-Datei-Offset,
#  VAG-Datei-Offset, VAG-Laenge, sha1) — alles selbst nachgelesene DATEI-Byte-Offsets.
SAETZE = [
    ("ZU_A", "1140", 0x16, "00007416", 0x01448, 0x01DB0, 0x02870, 7184,
     "cf1414572aea7ba1f3364129a5021b8fc8111b4b"),
    ("ZU_B", "1050", 0x16, "00008416", 0x019FC, 0x02384, 0x074A4, 3232,
     "60e753ac5e5697bae5b2f3caa4d3eb1dc1308708"),
    ("ZU_E", "2110", 0x16, "00007400", 0x02EFC, 0x03864, 0x07054, 10272,
     "be2f6ea9caaa6a9996a73c5a3f3fb0baf6b56aa3"),
]
ZUSATZ = [
    ("AUF_KEY", "1140", 0x25, "00008416", 0x01484, 0x01DD0, 0x04480, 5264,
     "e12a441e528242816cc556dabbcc92a83d9580b9"),
    ("AUF_HIER", "20A0", 0x26, "00006416", 0x02474, 0x02D7C, 0x07D8C, 7568,
     "d1d99d659a37a8e58ac427b36e166b8d3124411a"),
]

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x20
TRAILER = 8


def die(msg):
    sys.stderr.write("re2_door_se_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def main():
    install = "--install" in sys.argv
    saetze = list(SAETZE) + (list(ZUSATZ) if "--mit-aufschliessen" in sys.argv else [])
    if install:
        out_vbs = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "TUERSE.VBS")
        out_inc = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re2_door_bank.inc")
    else:
        od = os.path.join(REPO, "build", "r30_tuer-verschlossen", "bank")
        out_vbs = os.path.join(od, "TUERSE.VBS")
        out_inc = os.path.join(od, "re2_door_bank.inc")
    os.makedirs(os.path.dirname(out_vbs), exist_ok=True)
    os.makedirs(os.path.dirname(out_inc), exist_ok=True)

    se_map = bytearray(SE_MAP_BYTES)
    vh_out = bytearray(VH_SIZE)
    vbd = bytearray()
    ref_hdr = None
    rows = []
    for i, (name, room, sid, edt_hex, edt_off, tone_off, vag_off, vag_len, sha) in enumerate(saetze):
        p = os.path.join(RDT, "ROOM%s.RDT" % room)
        if not os.path.isfile(p):
            die("Quelle fehlt: %s" % p)
        a = open(p, "rb").read()
        edt, vh, vb = struct.unpack_from("<III", a, 8)
        if a[vh:vh + 4] != b"pBAV":
            die("ROOM%s: kein pBAV @0x%X" % (room, vh))
        # --- Selbsttest gegen die zitierten Bytes ---------------------------------
        if edt + sid * 4 != edt_off:
            die("ROOM%s Satz 0x%02X: EDT-Offset 0x%X != 0x%X" % (room, sid, edt + sid * 4, edt_off))
        raw = a[edt_off:edt_off + 4]
        if raw.hex() != edt_hex:
            die("ROOM%s Satz 0x%02X @0x%X: %s != %s" % (room, sid, edt_off, raw.hex(), edt_hex))
        prog = raw[1] & 0x7F
        tone = raw[2] >> 4
        nprog = struct.unpack_from("<H", a, vh + 0x12)[0]
        if vh + 0x820 + prog * 0x200 + tone * 0x20 != tone_off:
            die("ROOM%s: Tone-Offset weicht ab" % room)
        t = bytearray(a[tone_off:tone_off + 32])
        vagi = struct.unpack_from("<H", t, 0x16)[0]
        sizetab = vh + 0x820 + nprog * 0x200
        pos = vb
        for k in range(0, vagi):
            pos += struct.unpack_from("<H", a, sizetab + k * 2)[0] << 3
        vlen = struct.unpack_from("<H", a, sizetab + vagi * 2)[0] << 3
        if (pos, vlen) != (vag_off, vag_len):
            die("ROOM%s: VAG %d liegt @0x%X/%d B, erwartet @0x%X/%d B" % (room, vagi, pos, vlen, vag_off, vag_len))
        wav = a[vag_off:vag_off + vag_len]
        got = hashlib.sha1(wav).hexdigest()
        if got != sha:
            die("ROOM%s VAG %d sha1 %s != %s" % (room, vagi, got, sha))
        # --- in die Mini-Bank legen ----------------------------------------------
        struct.pack_into("<H", t, 0x16, i + 1)                       # Aenderung (1)
        vh_out[VAB_HDR + VAB_PROG_TABLE + i * 32:VAB_HDR + VAB_PROG_TABLE + (i + 1) * 32] = t
        rec = bytearray(raw)
        rec[1] = 0                                                   # Programm 0 der Mini-Bank
        rec[2] = ((i & 0xF) << 4) | (raw[2] & 0x0F)                  # Aenderung (2)
        se_map[i * 4:i * 4 + 4] = rec
        struct.pack_into("<H", vh_out, VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + 2 * (i + 1), vag_len // 8)
        vbd += wav
        if ref_hdr is None:
            ref_hdr = (a[vh:vh + 32], a[vh + 32:vh + 48])
        rows.append((i, name, room, sid, raw, bytes(rec), tone_off, bytes(t), vag_off, vag_len, sha))

    hdr, prog0 = ref_hdr
    vh_out[0:4] = b"pBAV"
    vh_out[4:8] = hdr[4:8]                                           # version = 7
    vh_out[8:12] = hdr[8:12]                                         # bank id
    struct.pack_into("<I", vh_out, 12, VH_SIZE + len(vbd))           # fsize
    vh_out[16:18] = hdr[16:18]                                       # reserviert (0xEEEE)
    struct.pack_into("<H", vh_out, 18, 1)                            # nprog
    struct.pack_into("<H", vh_out, 20, 16)                           # ntone (Segment = 16 Saetze)
    struct.pack_into("<H", vh_out, 22, len(saetze))                  # nvag
    vh_out[24] = hdr[24]                                             # master volume
    vh_out[25] = hdr[25]                                             # master pan
    vh_out[26:32] = hdr[26:32]
    vh_out[VAB_HDR:VAB_HDR + 16] = prog0                             # Programm 0 bytegleich
    vh_out[VAB_HDR] = len(saetze)                                    # ... bis auf die Tone-Zahl

    trailer = struct.pack("<II", SE_MAP_BYTES, 0)
    edt_rec = bytes(se_map) + bytes(vh_out) + trailer
    open(out_vbs, "wb").write(edt_rec + bytes(vbd))

    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/re2_door_se_cut.py -- DO NOT EDIT.\n"
                " * Satz-TOC der Mini-Bank shared_assets/RE2/TUERSE.VBS (RE2-\"Tuer verschlossen\").\n"
                " * Satzformat wie ELEVSE.VBS / ein ENEMSE-Bank-Satz (FUN_8005a09c):\n"
                " * [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @size-8][VBD].\n"
                " * Herkunft je Satz (RE2 info/re2leon/PL0/RDT, Datei-Offsets):\n")
        for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write(" *   %d %-8s ROOM%s Satz 0x%02X  EDT %s  Tone @0x%05X  VAG @0x%05X %5d B  sha1 %s\n"
                    % (i, name, room, sid, raw.hex(" "), tone_off, vag_off, vag_len, sha))
        o.write(" */\n")
        o.write("#define RE2_DOOR_EDT_OFF   %du\n" % 0)
        o.write("#define RE2_DOOR_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_DOOR_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_DOOR_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_DOOR_SE_COUNT  %d\n" % len(rows))
        for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write("#define RE2_DOOR_SE_%-8s %d   /* RE2 ROOM%s Satz 0x%02X, %d B */\n"
                    % (name, i, room, sid, vag_len))

    for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
        print("  Satz %d %-8s ROOM%s 0x%02X  EDT %s -> %s  Tone @0x%05X vol%d pan%d center%d shift%d "
              "note%d  VAG @0x%05X %d B sha1 %s"
              % (i, name, room, sid, raw.hex(" "), rec.hex(" "), tone_off, t[2], t[3], t[4], t[5],
                 t[6], vag_off, vag_len, sha[:12]))
    print("  TUERSE.VBS: edt_size=%d (vh_off=0x%X) vbd_off=%d vbd_size=%d gesamt=%d B  md5 %s"
          % (len(edt_rec), SE_MAP_BYTES, len(edt_rec), len(vbd), len(edt_rec) + len(vbd),
             hashlib.md5(edt_rec + bytes(vbd)).hexdigest()))
    print("  -> %s" % out_vbs)
    print("  -> %s" % out_inc)
    return 0


if __name__ == "__main__":
    sys.exit(main())
