#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""toene39_se_cut.py — Runde 39 Spur B "toene": schneidet die vier Toene fuer Schublade (ROOM11F0/11F1), Lueftungs-
gitter und Schrank (ROOM1190/1191) und baut daraus die Mini-VAB-Bank TOENE39.VBS (Satzformat wie ELZASE.VBS /
TUERSE.VBS, tools/r36_l/re2_elza_se_cut.py).

⛔ ERGAENZUNG, KEIN RE1.5-ORIGINAL AN DIESEN STELLEN. RE1.5 spielt dort nichts: ROOM11F0 sub19 (@0x017B6 Cut_chg 11 auf
   die geoeffnete Schublade), ROOM1190 sub10 (@0x02796 Schrank obj 5 faehrt) und sub11 (Luke obj 4, je Hund) enthalten
   kein Se_on. NUTZER-VORGABE (AUFTRAG Runde 39): Schublade "aus Resident Evil 2 ... wenn dort eine Schublade geoeffnet
   wird", Gitter "von den oeffnenden Gittern der Spinnen aus STAGE2", Schrank "aus Resident Evil 2 von den bewegenden
   Regalen in der 2. Etage". Dossier: analysis/befunde_runde39/B_toene.md.

QUELLEN (alles selbst gelesen, Selbsttest gegen die zitierten Bytes unten):
  Satz 0 SCHUBLADE  RE2 ROOM1130 Raumbank-Satz 0x28. RE2-Schubladen-Handler = AOT-Tabelle @0x800a73c4[13] -> 0x80052044:
                    @0x80052114 `lui a0,0x228` (Delay-Slot) / @0x80052188 `addu a1,zero,zero` / @0x8005218c
                    `jal 0x8005ba28` = Se_on(2,0x28) im selben Aufruf wie der Kamera-Schnitt @0x800521d8 `jal 0x8002c7ac`.
                    Vier RE2-Raeume tragen eine Schublade (Aot_set sce 13): 10E0/1130/1150/2010; ROOM1130 = "ROOM113.SAP -
                    Changing clothes/locked drawer sounds?" der Nutzer-SAP-Liste (RE2_SOUND_LISTE.md), Schublade @0x00D50.
  Satz 1 GITTER_AUF RE1.5 STAGE2 ROOM2060 snd0-Satz 0x0E: sub02 @0x0134A `36 02 0e 00 03 02 ..` Se_on(2,0x0E) vor dem
                    Aufklappen des Schachtgitters (@0x01356 Speed_set(5,25), @0x0135A For 20). Bank 2 = snd0 (FUN_80045024).
  Satz 2 GITTER_ZU  RE1.5 STAGE2 ROOM2060 snd0-Satz 0x0D: sub02 @0x01376 `36 02 0d 00 00 02 ..` Se_on(2,0x0D), wenn das
                    Gitter in die Ruhelage zurueckschlaegt (nach @0x01368 Speed_set(5,-25) For 20).
  Satz 3 REGAL      RE2 ROOM1120 (Bibliothek 2F) Raumbank-Satz 0x0C: sub04 @0x02F2E `36 02 0c 01 04 00 ..` vor
                    @0x02F58 `18 08` Gosub sub08 (Regalfahrt For 30 x 2 Bilder @0x03296); 0x0E/0x10 = dieselbe Welle.

EINZIGE Daten-Aenderungen beim Schnitt (wie re2_elza_se_cut.py):
  (1) Tone-Byte +0x16 (VAG-Index, 1-basiert) -> Lage in der Mini-Bank.
  (2) EDT-Byte 2 oberes Nibble (Tone-Index im Programm) -> Lage in der Mini-Bank; Byte 1 (Programm) -> 0.
  Alles andere (vol/pan/center/shift/min/max/ADSR/Modus, Prioritaet, Stimme, die Welle) ist bytegleich.

Ausgabe (Standard = Ermittlungsstand): build/r39_toene/bank/TOENE39.VBS + re2_toene39_bank.inc
Mit --install: re15_port/shared_assets/RE2/TOENE39.VBS + re15_port/engine/src/gen/re2_toene39_bank.inc
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
RE2_RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
RE15_PSX = os.path.join(PORT, "shared_assets", "PSX")

# (name, datei, kopf-offset der Bank, satz, EDT-Rohsatz, EDT-Datei-Off, Ton-Datei-Off, VAG-Datei-Off, VAG-Laenge, sha1)
#   kopf-offset: RE2 Raumbank = RDT +0x08 (EDT/VH/VB, re2_elza_se_cut.py); RE1.5 snd0 = RDT +0x08 (Bank 2,
#   re15_audio.h / FUN_80045024 caseD_2), snd1 = +0x14.
SAETZE = [
    ("SCHUBLADE", os.path.join(RE2_RDT, "ROOM1130.RDT"), 0x08, 0x28, "00005416", 0x11AD8, 0x123B8, 0x15A08, 5216,
     "c58644421c0f913a1cd4726fdc1ec8d9d2dcf01c"),
    ("GITTER_AUF", os.path.join(RE15_PSX, "STAGE2", "ROOM2060.RDT"), 0x08, 0x0E, "0000b111", 0x05B70, 0x06538,
     0x0CA38, 2416, "afecf6f0205526cf392d86c082a35be1667ddd00"),
    ("GITTER_ZU", os.path.join(RE15_PSX, "STAGE2", "ROOM2060.RDT"), 0x08, 0x0D, "0000a211", 0x05B6C, 0x06518,
     0x0C278, 1984, "bc83fd5bb9021f790aa6d49dd474b2417c4845fb"),
    ("REGAL", os.path.join(RE2_RDT, "ROOM1120.RDT"), 0x08, 0x0C, "00006300", 0x14CA4, 0x15614, 0x35C44, 10512,
     "e855511764c6b72fdffb3cb7b21ca91f21583ac4"),
]

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x20
TRAILER = 8


def die(msg):
    sys.stderr.write("toene39_se_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def main():
    install = "--install" in sys.argv
    if install:
        out_vbs = os.path.join(PORT, "shared_assets", "RE2", "TOENE39.VBS")
        out_inc = os.path.join(PORT, "engine", "src", "gen", "re2_toene39_bank.inc")
    else:
        od = os.path.join(REPO, "build", "r39_toene", "bank")
        out_vbs = os.path.join(od, "TOENE39.VBS")
        out_inc = os.path.join(od, "re2_toene39_bank.inc")
    os.makedirs(os.path.dirname(out_vbs), exist_ok=True)
    os.makedirs(os.path.dirname(out_inc), exist_ok=True)
    if len(SAETZE) * 4 > SE_MAP_BYTES or len(SAETZE) > 16:
        die("zu viele Saetze fuer SE-Map/Ton-Segment")

    se_map = bytearray(SE_MAP_BYTES)
    vh_out = bytearray(VH_SIZE)
    vbd = bytearray()
    ref_hdr = None
    rows = []
    for i, (name, p, kopf, sid, edt_hex, edt_off, tone_off, vag_off, vag_len, sha) in enumerate(SAETZE):
        if not os.path.isfile(p):
            die("Quelle fehlt: %s" % p)
        a = open(p, "rb").read()
        edt, vh, vb = struct.unpack_from("<III", a, kopf)
        quelle = os.path.relpath(p, REPO).replace(os.sep, "/")
        if a[vh:vh + 4] != b"pBAV":
            die("%s: kein pBAV @0x%X" % (quelle, vh))
        # --- Selbsttest gegen die zitierten Bytes ---------------------------------
        if edt + sid * 4 != edt_off:
            die("%s Satz 0x%02X: EDT-Offset 0x%X != 0x%X" % (quelle, sid, edt + sid * 4, edt_off))
        raw = a[edt_off:edt_off + 4]
        if raw.hex() != edt_hex:
            die("%s Satz 0x%02X @0x%X: %s != %s" % (quelle, sid, edt_off, raw.hex(), edt_hex))
        prog = raw[1] & 0x7F
        tone = raw[2] >> 4
        nprog = struct.unpack_from("<H", a, vh + 0x12)[0]
        if vh + 0x820 + prog * 0x200 + tone * 0x20 != tone_off:
            die("%s: Tone-Offset weicht ab (0x%X)" % (quelle, vh + 0x820 + prog * 0x200 + tone * 0x20))
        t = bytearray(a[tone_off:tone_off + 32])
        vagi = struct.unpack_from("<H", t, 0x16)[0]
        sizetab = vh + 0x820 + nprog * 0x200
        pos = vb
        for k in range(0, vagi):
            pos += struct.unpack_from("<H", a, sizetab + k * 2)[0] << 3
        vlen = struct.unpack_from("<H", a, sizetab + vagi * 2)[0] << 3
        if (pos, vlen) != (vag_off, vag_len):
            die("%s: VAG %d liegt @0x%X/%d B, erwartet @0x%X/%d B" % (quelle, vagi, pos, vlen, vag_off, vag_len))
        wav = a[vag_off:vag_off + vag_len]
        got = hashlib.sha1(wav).hexdigest()
        if got != sha:
            die("%s VAG %d sha1 %s != %s" % (quelle, vagi, got, sha))
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
        rows.append((i, name, quelle, sid, raw, bytes(rec), tone_off, bytes(t), vag_off, vag_len, sha))

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
    blob = edt_rec + bytes(vbd)
    open(out_vbs, "wb").write(blob)

    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/r39_b/toene39_se_cut.py -- DO NOT EDIT.\n"
                " * Satz-TOC der Mini-Bank shared_assets/RE2/TOENE39.VBS (Runde 39 Spur B, Ergaenzung, kein RE1.5-Original).\n"
                " * Satzformat wie ELZASE.VBS: [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @size-8][VBD].\n"
                " * Herkunft je Satz (Datei, Datei-Offsets):\n")
        for (i, name, quelle, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write(" *   %d %-10s %s Satz 0x%02X  EDT %s  Tone @0x%05X  VAG @0x%05X %5d B  sha1 %s\n"
                    % (i, name, quelle, sid, raw.hex(" "), tone_off, vag_off, vag_len, sha))
        o.write(" */\n")
        o.write("#define RE2_TOENE39_EDT_OFF   %du\n" % 0)
        o.write("#define RE2_TOENE39_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_TOENE39_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_TOENE39_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_TOENE39_SE_COUNT  %d\n" % len(rows))
        o.write("#define RE2_TOENE39_GROESSE   %du   /* md5 %s */\n" % (len(blob), hashlib.md5(blob).hexdigest()))
        for (i, name, quelle, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write("#define RE2_TOENE39_SE_%-10s %d   /* %s Satz 0x%02X, VAG %d B, sha1 %s */\n"
                    % (name, i, quelle.split("/")[-1], sid, vag_len, sha[:12]))
        for (i, name, quelle, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
            o.write("#define RE2_TOENE39_Q_%s_VAG_OFF  0x%05Xu  /* Quelle %s */\n" % (name, vag_off, quelle))
            o.write("#define RE2_TOENE39_Q_%s_VAG_LEN  %du\n" % (name, vag_len))
            o.write("#define RE2_TOENE39_Q_%s_TON_OFF  0x%05Xu\n" % (name, tone_off))
            o.write("#define RE2_TOENE39_Q_%s_EDT_OFF  0x%05Xu\n" % (name, SAETZE[i][5]))

    for (i, name, quelle, sid, raw, rec, tone_off, t, vag_off, vag_len, sha) in rows:
        print("  Satz %d %-10s %s 0x%02X  EDT %s -> %s  Tone @0x%05X mode%d vol%d pan%d center%d shift%d "
              "note%d  VAG @0x%05X %d B sha1 %s"
              % (i, name, quelle, sid, raw.hex(" "), rec.hex(" "), tone_off, t[1], t[2], t[3], t[4], t[5],
                 t[6], vag_off, vag_len, sha[:12]))
    print("  TOENE39.VBS: edt_size=%d (vh_off=0x%X) vbd_off=%d vbd_size=%d gesamt=%d B  md5 %s"
          % (len(edt_rec), SE_MAP_BYTES, len(edt_rec), len(vbd), len(blob), hashlib.md5(blob).hexdigest()))
    print("  -> %s" % out_vbs)
    print("  -> %s" % out_inc)
    return 0


if __name__ == "__main__":
    sys.exit(main())
