#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_elza_se_cut.py — Runde 36 Spur L: schneidet die zwei RE2-Wellen fuer die Elza-Szenen und baut daraus
die Mini-VAB-Bank ELZASE.VBS (Satzformat wie TUERSE.VBS / ELEVSE.VBS, tools/re2_door_se_cut.py).

⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (Beta -> Retail). RE1.5 spielt in beiden Szenen nichts:
   ROOM1211 sub02 (Kendo-Befreiung) und ROOM1021 sub02 (Kendo erschiesst Roy) enthalten kein Se_on; alle 284
   Se_on der ausgelieferten RDTs liegen auf Bank 2 (Raumbank), keiner ist ein Schuss (ROOM1041 msg 3 traegt statt
   eines Tons den Platzhaltertext "gunshot"). Dossier: analysis/befunde_runde36/L_elza.md.

RE2-VORBILD (alles selbst aus info/re2leon gelesen):
  Satz 0 AUF    ROOM1140 Raumbank-Satz 0x25 = der Aufschliess-Ton des Tuer-Handlers 0x80051514:
                @0x8005164c jal 0x8002fe38 (msg 5 "You unlocked the door using the ..."), @0x80051654 lui a0,0x225,
                @0x80051658 jal 0x8005ba28, @0x8005165c addu a1,zero,zero (Bank 2 = Raumbank, Satz 0x25, nicht
                positional). Zensus (re2_raum_se_zensus.py 0x25): 7 Wellen-Familien; ROOM1140 = Raum der
                "Locked/Unlocked door"-SAP (RE2_SOUND_LISTE.md ROOM114) und Quelle des Port-Tuer-zu-Tons ZU_A
                (gleiche Raumbank, Satz 0x16) -> zu und auf aus EINER RE2-Tuerfamilie.
  Satz 1 SCHUSS ROOM4040 Raumbank-Satz 0x0A = der RE2-SKRIPT-PISTOLENSCHUSS (Runde 38 Spur H, korrigiert;
                analysis/befunde_runde38/H_elzatoene.md §2.3): ROOM4040 feuert ihn in einer Skriptszene fuenfmal
                (@0x014E4/@0x01528/@0x01558/@0x01578/@0x01668 `36 02 0a 00 03 01 ..`, die ersten zwei mit
                Muendungsfeuer `3a 00 18 ..` @0x014F0/@0x01534; Work_set(1,0) @0x014BE, Plc_motion(0,0x11) @0x014D6);
                dieselbe Welle (sha1
                994b137d4007, 5824 B) schiesst in ROOM4090 @0x02A4E und ROOM6020 @0x00B94 (Nutzer-SAP-Liste
                "ROOM602.SAP - ... a gunshot") und liegt nur in Raumbaenken (4040/4090/6020/6110/D040/F110).
                Runde 36 hatte ROOM40A0 Satz 0x08 genommen — das ist KEIN Schuss: einziger Gegner des Raums ist der
                Alligator (Sce_em_set @0x025A0 `44 00 00 23 ..`), und Se_on(2,8) @0x02896 steht in einer While-Schleife
                (@0x0288A `0f 04 82 00` + Ck(5,0x0b)) am Alligator (Ursprung Modus 2 Index 0).
                Gegenprobe: RE2s Kendo-Schuss ROOM1010 Satz 0x0C ist bytegleich ARMS07 VAG 2 (Schrotflinte,
                sha1 42bbd9b31614) — John haelt in RE1.5 aber eine PISTOLE (Bildmessung, Runde-36-Dossier §P4).

EINZIGE Daten-Aenderungen beim Schnitt (wie re2_door_se_cut.py):
  (1) Tone-Byte +0x16 (VAG-Index, 1-basiert) -> Lage in der Mini-Bank.
  (2) EDT-Byte 2 oberes Nibble (Tone-Index im Programm) -> Lage in der Mini-Bank; Byte 1 (Programm) -> 0.
  Alles andere (vol/pan/center/shift/min/max/ADSR, Prioritaet, Stimme, die Welle) ist bytegleich.

Ausgabe (Standard = Ermittlungsstand): build/r36_elza/bank/ELZASE.VBS + re2_elza_bank.inc
Mit --install: re15_port/shared_assets/RE2/ELZASE.VBS + re15_port/engine/src/gen/re2_elza_bank.inc
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

# (name, raum, re2-satz, EDT-Rohsatz, EDT-Datei-Offset, Tone-Datei-Offset, VAG-Datei-Offset, VAG-Laenge, sha1)
SAETZE = [
    ("AUF", "1140", 0x25, "00008416", 0x01484, 0x01DD0, 0x04480, 5264,
     "e12a441e528242816cc556dabbcc92a83d9580b9"),
    # Runde 38 Spur H: RE2-Skript-Pistolenschuss ROOM4040 Satz 0x0A (vorher ROOM40A0 0x08 = Alligator-Schleife).
    ("SCHUSS", "4040", 0x0A, "00007300", 0x0BF60, 0x0C8F8, 0x18CC8, 5824,
     "994b137d40070755fa17b026430cd98bfd36bd08"),
]

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x20
TRAILER = 8


def die(msg):
    sys.stderr.write("re2_elza_se_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def main():
    install = "--install" in sys.argv
    if install:
        out_vbs = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "ELZASE.VBS")
        out_inc = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re2_elza_bank.inc")
    else:
        od = os.path.join(REPO, "build", "r36_elza", "bank")
        out_vbs = os.path.join(od, "ELZASE.VBS")
        out_inc = os.path.join(od, "re2_elza_bank.inc")
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
    vh_out[4:8] = hdr[4:8]
    vh_out[8:12] = hdr[8:12]
    struct.pack_into("<I", vh_out, 12, VH_SIZE + len(vbd))
    vh_out[16:18] = hdr[16:18]
    struct.pack_into("<H", vh_out, 18, 1)                            # nprog
    struct.pack_into("<H", vh_out, 20, 16)                           # ntone (Segment = 16 Saetze)
    struct.pack_into("<H", vh_out, 22, len(SAETZE))                  # nvag
    vh_out[24] = hdr[24]
    vh_out[25] = hdr[25]
    vh_out[26:32] = hdr[26:32]
    vh_out[VAB_HDR:VAB_HDR + 16] = prog0
    vh_out[VAB_HDR] = len(SAETZE)

    trailer = struct.pack("<II", SE_MAP_BYTES, 0)
    edt_rec = bytes(se_map) + bytes(vh_out) + trailer
    open(out_vbs, "wb").write(edt_rec + bytes(vbd))

    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/r36_l/re2_elza_se_cut.py -- DO NOT EDIT.\n"
                " * Satz-TOC der Mini-Bank shared_assets/RE2/ELZASE.VBS (Runde 36 Spur L, RE2-Ergaenzung).\n"
                " * Satzformat wie TUERSE.VBS: [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @size-8][VBD].\n"
                " * Herkunft je Satz (RE2 info/re2leon/PL0/RDT, Datei-Offsets):\n")
        for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write(" *   %d %-7s ROOM%s Satz 0x%02X  EDT %s  Tone @0x%05X  VAG @0x%05X %5d B  sha1 %s\n"
                    % (i, name, room, sid, raw.hex(" "), tone_off, vag_off, vag_len, sha))
        o.write(" */\n")
        o.write("#define RE2_ELZA_EDT_OFF   %du\n" % 0)
        o.write("#define RE2_ELZA_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_ELZA_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_ELZA_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_ELZA_SE_COUNT  %d\n" % len(rows))
        for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write("#define RE2_ELZA_SE_%-7s %d   /* RE2 ROOM%s Satz 0x%02X, %d B */\n"
                    % (name, i, room, sid, vag_len))

    for (i, name, room, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
        print("  Satz %d %-7s ROOM%s 0x%02X  EDT %s -> %s  Tone @0x%05X vol%d pan%d center%d shift%d "
              "note%d  VAG @0x%05X %d B sha1 %s"
              % (i, name, room, sid, raw.hex(" "), rec.hex(" "), tone_off, t[2], t[3], t[4], t[5],
                 t[6], vag_off, vag_len, sha[:12]))
    print("  ELZASE.VBS: edt_size=%d (vh_off=0x%X) vbd_off=%d vbd_size=%d gesamt=%d B  md5 %s"
          % (len(edt_rec), SE_MAP_BYTES, len(edt_rec), len(vbd), len(edt_rec) + len(vbd),
             hashlib.md5(edt_rec + bytes(vbd)).hexdigest()))
    print("  -> %s" % out_vbs)
    print("  -> %s" % out_inc)
    return 0


if __name__ == "__main__":
    sys.exit(main())
