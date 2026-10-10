#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""knall_cut.py — Knall-Tonbank der 1150-Montage (Runde 35 Spur L; Runde 36 Spur K: Satz 0 ersetzt)
als eingebackene Tonbank im DO2-Tonteil-Format (das die vorhandene Tuerbank-Schnittstelle
re15_audio_re2_tuer_laden laedt: Kopf 0xC38 = [SE-Map @0][VH "pBAV" @0x10][Nachspann u32 vh_off @0xC30],
VB @0xC38 — audio_pc.c TORSE_EDT_SIZE, @0x80014e48 addiu a3,s0,3120 / @0x80014f84 ori s1,s1,0x1c38).
Der Kopf traegt genau EIN Programm (VH 0xC20 B = 32 + 128*16 + 1*512 + 256*2), also hoechstens 16 Tones.

Satz 0 "Tuerknall"  = RUNDE 36 SPUR K (NUTZER-VORGABE "kein Tuer schliessen, sondern ein Tuer-Knall-Sound aus RE2",
                      RE2_SOUND_LISTE.md: ROOM106 "Zombies breaking in"): RE2 ROOM1060.RDT Raumbank snd0
                      (RDT-Kopfworte +0x08/+0x0C/+0x10 = EDT @0x03F58 / VH @0x04018 / VB @0x04C38; Bank 2 =
                      Raumbank: RE2 PSX.EXE @0x80059f1c lw a1,8(a2) / @0x80059f44 sw a1,-17536(at) = 0x800dbb80),
                      EDT-Satz 0x0a @0x03F80 `00 00 85 20` = Programm 0, Tone 8, Prio-Nibble 5, byte3 0x20 =
                      Direkt-Stimme + 1 Zusatzlage (Tone 9) — gespielt von RE2 ROOM1060 sub05 @0x00F48
                      `36 02 0a 01 00 00 dc 88 f8 f8 3d 05` = EIN Schlag gegen die Huettentuer, hinter der die
                      Zombies stehen (Dossier analysis/befunde_runde36/K_szene1150.md §K1.2). Tone 8/9 (fine 0/1) ->
                      VAG 5 @Datei 0x066B8, 8768 B, center 91 / min 67 -> 11025 Hz.
                      (Runde 35: RE2 DOOR04.DO2 Tone 2 = Door_exit — der Schliesston einer Tuer, vom Nutzer verworfen.)
Satz 1 "Knall 1030" = RE1.5 ROOM1030.RDT Raumbank snd0 (Kopf +0x08: EDT @0x3760, VH @0x37E0,
                      VB @0x5280), Se_on-Satz 0x0c (sub08 @0x02776 `36 02 0c 00 00 00 cc dd f8 f8
                      f0 a7`): EDT[0x0c] @0x3790 = `00 00 77 15` -> Programm 0 Tone 7, Tone-Satz
                      @0x3FE0 (vol 127, pan 64, center 92, min=max 68) -> VAG 6 @VB+0x3920 =
                      Datei 0x8BA0, 8080 B (14112 Abtastwerte, 1,28 s, Spitze 32728).
                      Das ist der Knall der ROOM1030-Zwischensequenz ("so wie bei der Cutscene
                      von ROOM 1030").
Bank-Belegung: Tone 0/1 = ROOM1060 Tone 8/9 (VAG 1), Tone 2 = ROOM1030 Tone 7 (VAG 2);
SE-Map Satz 0 = `00 00 05 20` (Tone 0, Prio-Nibble/byte3 woertlich aus `00 00 85 20`),
Satz 1 = `00 00 27 15` (Tone 2, Prio-Nibble/byte3 woertlich aus `00 00 77 15`).
Programm 0: Attribute aus ROOM1030 (mvol 0x7f wie ROOM1060/DOOR04, VH-Master 127 in allen drei), Tone-Zahl 3.
Aufruf: python re15_port/tools/r35_l/knall_cut.py   -> engine/src/gen/knall_bank.inc
"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT1030 = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE1", "ROOM1030.RDT")
RDT1060 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM1060.RDT")
OUT_INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "knall_bank.inc")

VAB_HDR, VAB_PROG_TABLE, VAB_TONE_SEG, VAB_SIZE_TABLE = 32, 128 * 16, 16 * 32, 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # 3104 = 0xC20
VH_OFF, TRAILER_OFF, VB_OFF = 0x10, 0xC30, 0xC38
assert VH_OFF + VH_SIZE == TRAILER_OFF


def die(m):
    sys.stderr.write("knall_cut: FEHLER: %s\n" % m); sys.exit(1)


def welle(d, vh, vb, prog, tone):
    """(Tone-Satz 32 B, VAG-Index, VAG-Bytes) fuer Programm/Tone einer Raumbank."""
    nprog = struct.unpack_from("<H", d, vh + 18)[0]
    rec = d[vh + VAB_HDR + VAB_PROG_TABLE + (prog * 16 + tone) * 32:][:32]
    vag = struct.unpack_from("<H", rec, 0x16)[0]
    sizes = [struct.unpack_from("<H", d, vh + VAB_HDR + VAB_PROG_TABLE + nprog * VAB_TONE_SEG + 2 * i)[0] * 8
             for i in range(0, 256)]
    off = sum(sizes[1:vag])
    return rec, vag, off, d[vb + off:vb + off + sizes[vag]]


def main():
    a = open(RDT1030, "rb").read()
    r = open(RDT1060, "rb").read()
    # --- ROOM1030 (RE1.5): Se_on 0x0c ----------------------------------------------------------
    edt0, vh0, vb0 = struct.unpack_from("<III", a, 8)
    if (edt0, vh0, vb0) != (0x3760, 0x37E0, 0x5280): die("ROOM1030 Kopf-Offsets weichen ab")
    if a[0x2776:0x2776 + 12] != bytes.fromhex("36020c000000ccddf8f8f0a7"):
        die("Se_on @0x2776 weicht ab: %s" % a[0x2776:0x2782].hex())
    edt_c = a[edt0 + 0x0c * 4:edt0 + 0x0c * 4 + 4]
    if edt_c != bytes.fromhex("00007715"): die("EDT[0x0c] weicht ab: %s" % edt_c.hex())
    tone_a, vag_a, off_a, wav_a = welle(a, vh0, vb0, edt_c[1] & 0x7f, edt_c[2] >> 4)
    if (vag_a, off_a, len(wav_a)) != (6, 0x3920, 8080): die("ROOM1030 VAG weicht ab: %d %#x %d" % (vag_a, off_a, len(wav_a)))

    # --- ROOM1060 (RE2): Se_on 0x0a (sub05 @0x00F48) ------------------------------------------
    edt1, vh1, vb1 = struct.unpack_from("<III", r, 8)
    if (edt1, vh1, vb1) != (0x03F58, 0x04018, 0x04C38): die("RE2 ROOM1060 Kopf-Offsets weichen ab")
    if r[0x0F48:0x0F48 + 12] != bytes.fromhex("36020a010000dc88f8f83d05"):
        die("RE2 ROOM1060 Se_on @0x0F48 weicht ab: %s" % r[0x0F48:0x0F54].hex())
    edt_r = r[edt1 + 0x0a * 4:edt1 + 0x0a * 4 + 4]
    if edt_r != bytes.fromhex("00008520"): die("RE2 ROOM1060 EDT[0x0a] weicht ab: %s" % edt_r.hex())
    prog_r, tone_r, extra_r = edt_r[1] & 0x7f, edt_r[2] >> 4, edt_r[3] >> 5
    if (prog_r, tone_r, extra_r) != (0, 8, 1): die("RE2 ROOM1060 EDT[0x0a] zerlegt anders: %d %d %d" % (prog_r, tone_r, extra_r))
    t8, vag8, off8, wav8 = welle(r, vh1, vb1, 0, 8)
    t9, vag9, off9, wav9 = welle(r, vh1, vb1, 0, 9)
    if (vag8, vag9, vb1 + off8, len(wav8)) != (5, 5, 0x066B8, 8768):
        die("RE2 ROOM1060 Tone 8/9 weichen ab: VAG %d/%d @%#x %d B" % (vag8, vag9, vb1 + off8, len(wav8)))
    for rec in (t8, t9, tone_a):
        if rec[2] != 127: die("Tone-vol != 127")
    if r[vh1 + 24] != 127 or a[vh0 + 24] != 127 or r[vh1 + 33] != 0x7f or a[vh0 + 33] != 0x7f:
        die("VH-Master/Programm-mvol weichen ab (Pegelgesetz waere nicht dasselbe)")

    # --- Bank ------------------------------------------------------------------------------------
    se_map = bytearray(VH_OFF)
    se_map[0:4] = bytes([0x00, 0x00, (0 << 4) | (edt_r[2] & 0x0f), edt_r[3]])   # `00 00 05 20`: Tone 0 (+1 Lage)
    se_map[4:8] = bytes([0x00, 0x00, (2 << 4) | (edt_c[2] & 0x0f), edt_c[3]])   # `00 00 27 15`: Tone 2
    vh = bytearray(VH_SIZE)
    vh[0:4] = b"pBAV"
    struct.pack_into("<I", vh, 4, struct.unpack_from("<I", a, vh0 + 4)[0])      # Version
    struct.pack_into("<I", vh, 8, struct.unpack_from("<I", a, vh0 + 8)[0])      # Bank-Id
    struct.pack_into("<I", vh, 12, VH_SIZE + len(wav8) + len(wav_a))
    struct.pack_into("<H", vh, 18, 1); struct.pack_into("<H", vh, 20, 3); struct.pack_into("<H", vh, 22, 2)
    vh[24], vh[25] = a[vh0 + 24], a[vh0 + 25]
    vh[VAB_HDR:VAB_HDR + 16] = a[vh0 + VAB_HDR:vh0 + VAB_HDR + 16]            # Programm 0 wie ROOM1030 ...
    vh[VAB_HDR] = 3                                                            # ... mit 3 Tones
    tb = VAB_HDR + VAB_PROG_TABLE
    for i, (rec, vag) in enumerate(((t8, 1), (t9, 1), (tone_a, 2))):
        t = bytearray(rec); struct.pack_into("<H", t, 0x16, vag); vh[tb + 32 * i:tb + 32 * (i + 1)] = t
    sz = tb + VAB_TONE_SEG
    struct.pack_into("<H", vh, sz + 2, len(wav8) // 8); struct.pack_into("<H", vh, sz + 4, len(wav_a) // 8)
    head = bytes(se_map) + bytes(vh) + struct.pack("<II", VH_OFF, 0)
    assert len(head) == VB_OFF
    bank = head + wav8 + wav_a

    os.makedirs(os.path.dirname(OUT_INC), exist_ok=True)
    with open(OUT_INC, "w", encoding="ascii") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/r35_l/knall_cut.py -- DO NOT EDIT.\n"
                " * Knall-Tonbank der 1150-Montage im DO2-Tonteil-Format (re15_audio_re2_tuer_laden):\n"
                " *   Satz 0 = RE2 ROOM1060.RDT snd0 EDT 0x0a `00 00 85 20` (Se_on sub05 @0x00F48) = Tone 8+9 -> VAG 5\n"
                " *            @Datei 0x066B8 (%d B, sha1 %s) - Runde 36 Spur K (vorher DOOR04 Door_exit)\n"
                " *   Satz 1 = RE1.5 ROOM1030.RDT snd0 Se_on 0x0c = Tone 7 VAG 6 @Datei 0x8BA0 (%d B, sha1 %s)\n"
                " * Kopf 0xC38 (SE-Map @0, VH @0x10, Nachspann @0xC30), VB @0xC38. */\n"
                % (len(wav8), hashlib.sha1(wav8).hexdigest(), len(wav_a), hashlib.sha1(wav_a).hexdigest()))
        o.write("#define KNALL_BANK_SIZE %du\n" % len(bank))
        o.write("#define KNALL_SE_TUER   0\n#define KNALL_SE_1030   1\n")
        o.write("static const uint8_t k_knall_bank[KNALL_BANK_SIZE] = {\n")
        for i in range(0, len(bank), 16):
            o.write("    " + ", ".join("0x%02x" % x for x in bank[i:i + 16]) + ",\n")
        o.write("};\n")
    print("knall_bank.inc: %d B (Tuerknall RE2 ROOM1060 %d B sha1 %s, Knall 1030 %d B sha1 %s)" %
          (len(bank), len(wav8), hashlib.sha1(wav8).hexdigest()[:12], len(wav_a), hashlib.sha1(wav_a).hexdigest()[:12]))


if __name__ == "__main__":
    main()
