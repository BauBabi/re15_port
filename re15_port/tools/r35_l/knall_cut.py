#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""knall_cut.py — Spur L (Runde 35): die zwei Knall-Wellen der 1150-Montage als eingebackene
Tonbank im DO2-Tonteil-Format (das die vorhandene Tuerbank-Schnittstelle re15_audio_re2_tuer_laden
laedt: Kopf 0xC38 = [SE-Map @0][VH "pBAV" @0x10][Nachspann u32 vh_off @0xC30], VB @0xC38 —
audio_pc.c TORSE_EDT_SIZE, @0x80014e48 addiu a3,s0,3120 / @0x80014f84 ori s1,s1,0x1c38).

Satz 0 "Tuerknall"  = RE2 DOOR04.DO2 Tonteil, Tone 2 = EDT-Satz 1 `00 00 24 17` (Door_exit = Tuerschlag), VAG 3
                      (9216 B ADPCM, 16128 Abtastwerte, Huellkurven-Maximum 13100 bei 0,2 s —
                      lautester Tuerschlag der 27 RE2-Tuerarchive, Messung im Dossier §2.5).
                      ⛔ RE2-ERGAENZUNG (Beta -> Retail, "Sound ist RE2"); PORT-WAHL der Tuer.
Satz 1 "Knall 1030" = RE1.5 ROOM1030.RDT Raumbank snd0 (Kopf +0x08: EDT @0x3760, VH @0x37E0,
                      VB @0x5280), Se_on-Satz 0x0c (sub08 @0x02776 `36 02 0c 00 00 00 cc dd f8 f8
                      f0 a7`): EDT[0x0c] @0x3790 = `00 00 77 15` -> Programm 0 Tone 7, Tone-Satz
                      @0x3FE0 (vol 127, pan 64, center 92, min=max 68) -> VAG 6 @VB+0x3920 =
                      Datei 0x8BA0, 8080 B (14112 Abtastwerte, 1,28 s, Spitze 32728).
                      Das ist der Knall der ROOM1030-Zwischensequenz ("so wie bei der Cutscene
                      von ROOM 1030").
Aufruf: python re15_port/tools/r35_l/knall_cut.py   -> engine/src/gen/knall_bank.inc
"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT1030 = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE1", "ROOM1030.RDT")
DOOR04  = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "DOOR", "DOOR04.DO2")
OUT_INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "knall_bank.inc")

VAB_HDR, VAB_PROG_TABLE, VAB_TONE_SEG, VAB_SIZE_TABLE = 32, 128 * 16, 16 * 32, 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # 3104 = 0xC20
VH_OFF, TRAILER_OFF, VB_OFF = 0x10, 0xC30, 0xC38
assert VH_OFF + VH_SIZE == TRAILER_OFF


def die(m):
    sys.stderr.write("knall_cut: FEHLER: %s\n" % m); sys.exit(1)


def main():
    a = open(RDT1030, "rb").read()
    d = open(DOOR04, "rb").read()
    edt0, vh0, vb0 = struct.unpack_from("<III", a, 8)
    if (edt0, vh0, vb0) != (0x3760, 0x37E0, 0x5280): die("ROOM1030 Kopf-Offsets weichen ab")
    if a[0x2776:0x2776 + 12] != bytes.fromhex("36020c000000ccddf8f8f0a7"):
        die("Se_on @0x2776 weicht ab: %s" % a[0x2776:0x2782].hex())
    edt_c = a[edt0 + 0x0c * 4:edt0 + 0x0c * 4 + 4]
    if edt_c != bytes.fromhex("00007715"): die("EDT[0x0c] weicht ab: %s" % edt_c.hex())
    prog, tone = edt_c[1] & 0x7f, edt_c[2] >> 4
    nprog0 = struct.unpack_from("<H", a, vh0 + 18)[0]
    tone_rec_a = a[vh0 + VAB_HDR + VAB_PROG_TABLE + (prog * 16 + tone) * 32:][:32]
    vag_a = struct.unpack_from("<H", tone_rec_a, 0x16)[0]
    sizes_a = [struct.unpack_from("<H", a, vh0 + VAB_HDR + VAB_PROG_TABLE + nprog0 * VAB_TONE_SEG + 2 * i)[0] * 8
               for i in range(0, 256)]
    off_a = sum(sizes_a[1:vag_a])
    wav_a = a[vb0 + off_a:vb0 + off_a + sizes_a[vag_a]]
    if (vag_a, off_a, len(wav_a)) != (6, 0x3920, 8080): die("ROOM1030 VAG weicht ab: %d %#x %d" % (vag_a, off_a, len(wav_a)))

    vh_d = struct.unpack_from("<I", d, TRAILER_OFF)[0]
    if vh_d != 0x10 or d[vh_d:vh_d + 4] != b"pBAV": die("DOOR04 Tonteil weicht ab")
    nprog_d = struct.unpack_from("<H", d, vh_d + 18)[0]
    tone_rec_d = d[vh_d + VAB_HDR + VAB_PROG_TABLE + 2 * 32:][:32]        # Programm 0, Tone 2 = EDT-Satz 1 (Door_exit)
    vag_d = struct.unpack_from("<H", tone_rec_d, 0x16)[0]
    sizes_d = [struct.unpack_from("<H", d, vh_d + VAB_HDR + VAB_PROG_TABLE + nprog_d * VAB_TONE_SEG + 2 * i)[0] * 8
               for i in range(0, 256)]
    off_d = sum(sizes_d[1:vag_d])
    wav_d = d[VB_OFF + off_d:VB_OFF + off_d + sizes_d[vag_d]]
    if (vag_d, len(wav_d)) != (3, 9216): die("DOOR04 VAG weicht ab: %d %d" % (vag_d, len(wav_d)))

    # --- Bank: SE-Map Satz 0 = Tuerknall (DOOR04-EDT-Form `00 00 14 16` mit Tone 0),
    #     Satz 1 = Knall 1030 (ROOM1030-EDT-Form `00 00 77 15` mit Tone 1) ----------------
    se_map = bytearray(VH_OFF)
    se_map[0:4] = bytes([0x00, 0x00, (0 << 4) | 0x04, 0x17])   # DOOR04-EDT-Satz 1 `00 00 24 17`, Tone 2 -> 0
    se_map[4:8] = bytes([0x00, 0x00, (1 << 4) | 0x07, 0x15])
    vh = bytearray(VH_SIZE)
    vh[0:4] = b"pBAV"
    struct.pack_into("<I", vh, 4, struct.unpack_from("<I", a, vh0 + 4)[0])      # Version
    struct.pack_into("<I", vh, 8, struct.unpack_from("<I", a, vh0 + 8)[0])      # Bank-Id
    struct.pack_into("<I", vh, 12, VH_SIZE + len(wav_d) + len(wav_a))
    struct.pack_into("<H", vh, 18, 1); struct.pack_into("<H", vh, 20, 16); struct.pack_into("<H", vh, 22, 2)
    vh[24], vh[25] = a[vh0 + 24], a[vh0 + 25]
    vh[VAB_HDR:VAB_HDR + 16] = a[vh0 + VAB_HDR:vh0 + VAB_HDR + 16]            # Programm 0 wie ROOM1030
    tb = VAB_HDR + VAB_PROG_TABLE
    t0 = bytearray(tone_rec_d); struct.pack_into("<H", t0, 0x16, 1); vh[tb:tb + 32] = t0
    t1 = bytearray(tone_rec_a); struct.pack_into("<H", t1, 0x16, 2); vh[tb + 32:tb + 64] = t1
    sz = tb + VAB_TONE_SEG
    struct.pack_into("<H", vh, sz + 2, len(wav_d) // 8); struct.pack_into("<H", vh, sz + 4, len(wav_a) // 8)
    head = bytes(se_map) + bytes(vh) + struct.pack("<II", VH_OFF, 0)
    assert len(head) == VB_OFF
    bank = head + wav_d + wav_a

    os.makedirs(os.path.dirname(OUT_INC), exist_ok=True)
    with open(OUT_INC, "w") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/r35_l/knall_cut.py -- DO NOT EDIT.\n"
                " * Knall-Tonbank der 1150-Montage im DO2-Tonteil-Format (re15_audio_re2_tuer_laden):\n"
                " *   Satz 0 = RE2 DOOR04.DO2 Tone 2 = EDT-Satz 1 (Door_exit, VAG 3, %d B, sha1 %s)\n"
                " *   Satz 1 = RE1.5 ROOM1030.RDT snd0 Se_on 0x0c = Tone 7 VAG 6 @Datei 0x8BA0 (%d B, sha1 %s)\n"
                " * Kopf 0xC38 (SE-Map @0, VH @0x10, Nachspann @0xC30), VB @0xC38. */\n"
                % (len(wav_d), hashlib.sha1(wav_d).hexdigest(), len(wav_a), hashlib.sha1(wav_a).hexdigest()))
        o.write("#define KNALL_BANK_SIZE %du\n" % len(bank))
        o.write("#define KNALL_SE_TUER   0\n#define KNALL_SE_1030   1\n")
        o.write("static const uint8_t k_knall_bank[KNALL_BANK_SIZE] = {\n")
        for i in range(0, len(bank), 16):
            o.write("    " + ", ".join("0x%02x" % x for x in bank[i:i + 16]) + ",\n")
        o.write("};\n")
    print("knall_bank.inc: %d B (Tuerknall %d B sha1 %s, Knall 1030 %d B sha1 %s)" %
          (len(bank), len(wav_d), hashlib.sha1(wav_d).hexdigest()[:12], len(wav_a), hashlib.sha1(wav_a).hexdigest()[:12]))


if __name__ == "__main__":
    main()
