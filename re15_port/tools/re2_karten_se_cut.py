#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_karten_se_cut.py - Runde 36 Spur G (Punkt 3): RE2-"Aufschliessen"-Wellen als Mini-Bank KARTSE.VBS.

⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL. RE1.5 ist beim Aufschliessen stumm: ROOM10D0 sub19 @Datei 0x0199E
   (Message_on 5 "You've opened the lock.") und alle uebrigen Aufschliess-Bloecke haben kein Se_on
   (analysis/befunde_runde36/G_tueren.md G3; Zensus tools/gen_r36_aufschliessen.py).

RE2-VORBILD (selbst gelesen, Walker analysis/befunde_runde30/tools/r30_re2_scd.py):
  Kartenleser-Tuer ROOM2110.RDT sub08 (nach Sce_Item_lost der Karte, sub06 @0x01AEA/@0x01AEC):
    @0x01B4C Se_on(2,0x0C)  Sleep 15 (@0x01B58)  @0x01B5C Se_on(2,0x0D)  Sleep 15 (@0x01B8C)  @0x01B90 Se_on(2,0x0B)
  Se_on 0x36 @0x80056428 -> jal 0x8005ba28 @0x80056530, a0 = Bank<<24 | Satz<<16; Bank 2 = Raumbank
  (FUN_80059e54 @0x80059f44: 0x800dbb80 = [RDT+8]).
  Von dieser Seite entriegelt (key_type 0xFE): EXE-Tuer-Handler @0x800515ec jal 0x8002fe38 (Text 10
  "You unlocked it.") + @0x800515f4/@0x800515f8 lui a0,0x226 / jal 0x8005ba28 = Raumbank-Satz 0x26.

Saetze der Mini-Bank (alles DATEI-Byte-Offsets in info/re2leon/PL0/RDT, beim Lauf gegen die Bytes geprueft):
  0 KARTE_EIN ROOM2110 Satz 0x0C  Karte einziehen (Klack, breitbandig 2..4 kHz)
  1 PIEP      ROOM2110 Satz 0x0D  Piep (stehender Ton 1450 Hz, 0,49 s)
  2 AUF_E     ROOM2110 Satz 0x0B  elektrisches Aufschliessen (tief, 1,20 s)
  3 AUF_HIER  ROOM20A0 Satz 0x26  von dieser Seite entriegelt (dieselbe Welle in 9 von 11 RE2-Raeumen mit
                                  Satz 0x26, analysis/befunde_runde30/tuer-verschlossen.md 3.7)
Satzformat wie TUERSE.VBS / ELEVSE.VBS (tools/re2_door_se_cut.py, FUN_8005a09c):
  [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @edt_size-8][VBD].
Zwei Aenderungen gegenueber den Quellsaetzen (wie re2_door_se_cut.py): (1) Tone[+0x16] = VAG-Index der
Mini-Bank, (2) EDT-Satz: Programm 0, Ton = Satznummer (Prio/Stimme/Zusatzlagen bleiben).

Aufruf: python re15_port/tools/re2_karten_se_cut.py [--install]
  ohne --install: build/r36_tueren/bank/KARTSE.VBS + re2_karten_bank.inc
  mit  --install: re15_port/shared_assets/RE2/KARTSE.VBS + re15_port/engine/src/gen/re2_karten_bank.inc
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

# (name, raum, satz, EDT-Rohsatz, EDT-Datei-Offset, Tone-Datei-Offset, VAG-Datei-Offset, VAG-Laenge, sha1)
SAETZE = [
    ("KARTE_EIN", "2110", 0x0C, "00006416", 0x02ED4, 0x03844, 0x064B4, 2976,
     "9064a95ecede"),
    ("PIEP", "2110", 0x0D, "00004301", 0x02ED8, 0x03804, 0x09874, 4640,
     "b03b6dffae66"),
    ("AUF_E", "2110", 0x0B, "00005300", 0x02ED0, 0x03824, 0x0AA94, 11264,
     "da805fa3511e"),
    ("AUF_HIER", "20A0", 0x26, "00006416", 0x02474, 0x02D7C, 0x07D8C, 7568,
     "d1d99d659a37"),
]

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x20
TRAILER = 8


def die(msg):
    sys.stderr.write("re2_karten_se_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def main():
    install = "--install" in sys.argv
    if install:
        out_vbs = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "KARTSE.VBS")
        out_inc = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re2_karten_bank.inc")
    else:
        od = os.path.join(REPO, "build", "r36_tueren", "bank")
        out_vbs = os.path.join(od, "KARTSE.VBS")
        out_inc = os.path.join(od, "re2_karten_bank.inc")
    os.makedirs(os.path.dirname(out_vbs), exist_ok=True)
    os.makedirs(os.path.dirname(out_inc), exist_ok=True)

    se_map = bytearray(SE_MAP_BYTES)
    vh_out = bytearray(VH_SIZE)
    vbd = bytearray()
    ref_hdr = None
    rows = []
    for i, (name, room, sid, edt_hex, edt_off, tone_off, vag_off, vag_len, sha) in enumerate(SAETZE):
        p = os.path.join(RDT, "ROOM%s.RDT" % room)
        if not os.path.isfile(p):
            die("Quelle fehlt: %s" % p)
        a = open(p, "rb").read()
        edt, vh, vb = struct.unpack_from("<III", a, 8)
        if a[vh:vh + 4] != b"pBAV":
            die("ROOM%s: kein pBAV @0x%X" % (room, vh))
        if edt + sid * 4 != edt_off:
            die("ROOM%s Satz 0x%02X: EDT-Offset 0x%X != 0x%X" % (room, sid, edt + sid * 4, edt_off))
        raw = a[edt_off:edt_off + 4]
        if raw.hex() != edt_hex:
            die("ROOM%s Satz 0x%02X @0x%X: %s != %s" % (room, sid, edt_off, raw.hex(), edt_hex))
        prog = raw[1] & 0x7F
        tone = raw[2] >> 4
        if raw[3] >> 5:
            die("ROOM%s Satz 0x%02X: Zusatzlagen (%d) - die Mini-Bank fasst nur einlagige Saetze" % (room, sid, raw[3] >> 5))
        nprog = struct.unpack_from("<H", a, vh + 0x12)[0]
        if vh + 0x820 + prog * 0x200 + tone * 0x20 != tone_off:
            die("ROOM%s: Tone-Offset 0x%X != 0x%X" % (room, vh + 0x820 + prog * 0x200 + tone * 0x20, tone_off))
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
        if not got.startswith(sha):
            die("ROOM%s VAG %d sha1 %s != %s..." % (room, vagi, got, sha))
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
        rows.append((i, name, room, sid, raw, bytes(rec), tone_off, bytes(t), vag_off, vag_len, got))

    hdr, prog0 = ref_hdr
    vh_out[0:4] = b"pBAV"
    vh_out[4:8] = hdr[4:8]
    vh_out[8:12] = hdr[8:12]
    struct.pack_into("<I", vh_out, 12, VH_SIZE + len(vbd))
    vh_out[16:18] = hdr[16:18]
    struct.pack_into("<H", vh_out, 18, 1)
    struct.pack_into("<H", vh_out, 20, 16)
    struct.pack_into("<H", vh_out, 22, len(SAETZE))
    vh_out[24] = hdr[24]
    vh_out[25] = hdr[25]
    vh_out[26:32] = hdr[26:32]
    vh_out[VAB_HDR:VAB_HDR + 16] = prog0
    vh_out[VAB_HDR] = len(SAETZE)

    trailer = struct.pack("<II", SE_MAP_BYTES, 0)
    edt_rec = bytes(se_map) + bytes(vh_out) + trailer
    open(out_vbs, "wb").write(edt_rec + bytes(vbd))

    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/re2_karten_se_cut.py -- DO NOT EDIT.\n"
                " * Runde 36 Spur G: Satz-TOC der Mini-Bank shared_assets/RE2/KARTSE.VBS (RE2-\"Aufschliessen\").\n"
                " * Satzformat wie TUERSE.VBS / ELEVSE.VBS (FUN_8005a09c):\n"
                " * [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @size-8][VBD].\n"
                " * Herkunft je Satz (RE2 info/re2leon/PL0/RDT, Datei-Offsets):\n")
        for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write(" *   %d %-9s ROOM%s Satz 0x%02X  EDT %s  Tone @0x%05X  VAG @0x%05X %5d B  sha1 %s\n"
                    % (i, name, room, sid, raw.hex(" "), tone_off, vag_off, vag_len, sha))
        o.write(" */\n")
        o.write("#define RE2_KARTE_EDT_OFF   %du\n" % 0)
        o.write("#define RE2_KARTE_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_KARTE_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_KARTE_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_KARTE_SE_COUNT  %d\n" % len(rows))
        o.write("#define RE2_KARTE_DATEI_SHA1 \"%s\"\n" % hashlib.sha1(edt_rec + bytes(vbd)).hexdigest())
        for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write("#define RE2_KARTE_SE_%-9s %d   /* RE2 ROOM%s Satz 0x%02X, %d B */\n" % (name, i, room, sid, vag_len))

    for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
        print("  Satz %d %-9s ROOM%s 0x%02X  EDT %s -> %s  Tone @0x%05X vol%d pan%d center%d shift%d note%d  "
              "VAG @0x%05X %d B sha1 %s" % (i, name, room, sid, raw.hex(" "), rec.hex(" "), tone_off, t[2], t[3], t[4],
                                            t[5], t[6], vag_off, vag_len, sha[:12]))
    print("  KARTSE.VBS: edt_size=%d vbd_size=%d gesamt=%d B  sha1 %s" % (
        len(edt_rec), len(vbd), len(edt_rec) + len(vbd), hashlib.sha1(edt_rec + bytes(vbd)).hexdigest()))
    print("  -> %s\n  -> %s" % (out_vbs, out_inc))
    return 0


if __name__ == "__main__":
    sys.exit(main())
