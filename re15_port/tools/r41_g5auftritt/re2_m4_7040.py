#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_m4_7040.py - Runde 41 G5-Auftritt, Nachbesserung 5 (M4): Tunnellampen und Beben-Ton des Vorlaufs aus RE2 ROOM7040.

Quelle: info/re2leon/PL0/RDT/ROOM7040.RDT (RE2 Leon, Stage 7 Raum 04). Kopf: 8 Byte Zaehler (Byte 2 = 7 Objektmodelle), dann 23
Offsets; [0] EDT 0x6018, [1] VH 0x60D8, [2] VB 0x6EF8, [10] Objektmodell-Liste 0x244 (je 8 Byte: TIM, MD1).

  R704_LAMPE.MD1  = RDT[0x28A4 : 0x3080]    Modell 3 = Modell 4 (Liste @0x244 + 3*8 / + 4*8: TIM 0x33844, MD1 0x28A4). 2012 Byte:
                    Kopf + 1 Mesh (Laengenfeld 1492 = ohne UV-Tabellen; Dreieck-UVs @12+1480, Viereck-UVs @12+1696, 19*16 -> 2012;
                    0x3080 = MD1 von Modell 5). Tunnelwand 3600 x 5098 (x = 0) mit Lampengehaeuse (x 0..180).
  R704_LAMPE.TIM  = RDT[0x33844 : +34336]   8 bpp, CLUT 256 x 3 @ (0,480), Bild 128 x 256 @ (0,0); Laenge = 8 + 1548 + 32780.
  R704_BEBEN.VBS  = Mini-Bank (Satzformat wie KANISTER2080.VBS, tools/r41_kanister2080/kanister2080_se_cut.py), Satz 0 =
                    ROOM7040 EDT-Satz 12 `00 01 13 20` = sub04 Se_on `36 02 0c 00 01 00 ..` @SCD 0x016AC (Handler @0x80056428:
                    Code (pc[1]<<24)|(pc[2]<<16) -> `jal 0x8005ba28` @0x80056530; Lage = Spieler (Typ 1, Tabelle @0x80011458[1]))
  engine/src/gen/r41_g5a_beben_bank.inc   Groessen der Mini-Bank fuer den Lader (platform/pc/src/g5lampen_pc.c)
  R704.ESP        = RDT[0x35D8 : 0x36FC]   (Nachbesserung 6) Raum-ESP = Kopf [18] bis [19]+4 wie GATOR40A0.ESP (ROOM40A0 [0x59CC:0x65E8]);
                    Ids `19 ff ..`, Bank 0x19 = Brocken (sub05 8 x Sce_espr_on Effekt 0x19). Der RE2-Raumlader FUN_8001bba4 ruft
                    FUN_8001bca0(+0x50, +0x54, +0x50, 8) und FUN_8001bd38(+0x5c, +0x58). Die Textur der Bank (Tafel rueckwaerts vor
                    RDT[21]: RDT[0x1B1E0] = 0 -> RDT[0x18984 : 0x1B1C4], 10304 B) ist BYTEGLEICH shared_assets/RE2/GATOR40A0_19.TIM
                    und wird nicht doppelt abgelegt (hier geprueft).

Aufruf: python re15_port/tools/r41_g5auftritt/re2_m4_7040.py [--install]   (ohne --install nach build/r41_g5a_m4)
"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM7040.RDT")
MD1_VON, MD1_BIS = 0x28A4, 0x3080
TIM_VON, TIM_LEN = 0x33844, 34336
VAB_HDR = 32; VAB_PROG_TABLE = 128 * 16; VAB_TONE_SEG = 16 * 32; VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE
SE_MAP_BYTES = 0x20
SAETZE = [("BEBEN", 12, "00011320")]


def die(m):
    sys.stderr.write("re2_m4_7040: FEHLER: %s\n" % m)
    sys.exit(1)


def lampe(a):
    offs = struct.unpack_from("<23I", a, 8)
    if (a[2], offs[10]) != (7, 0x244): die("Kopf: %d Objektmodelle, Liste @0x%x" % (a[2], offs[10]))
    for m in (3, 4):
        tim, md1 = struct.unpack_from("<II", a, 0x244 + 8 * m)
        if (tim, md1) != (TIM_VON, MD1_VON): die("Modell %d: TIM 0x%x MD1 0x%x" % (m, tim, md1))
    if struct.unpack_from("<II", a, 0x244 + 8 * 5)[1] != MD1_BIS: die("MD1 von Modell 5 != 0x%x" % MD1_BIS)
    md1 = a[MD1_VON:MD1_BIS]
    ln, _, oc = struct.unpack_from("<III", md1, 0)
    h = struct.unpack_from("<14I", md1, 12)
    if (ln, oc) != (1492, 2) or 12 + h[13] + h[12] * 16 != len(md1): die("MD1-Form %d %d %s" % (ln, oc, h))
    tim = a[TIM_VON:TIM_VON + TIM_LEN]
    magic, fl = struct.unpack_from("<II", tim, 0)
    cl = struct.unpack_from("<IHHHH", tim, 8)
    im = struct.unpack_from("<IHHHH", tim, 8 + cl[0])
    if (magic, fl, cl[2], cl[3], cl[4]) != (0x10, 9, 480, 256, 3) or 8 + cl[0] + im[0] != TIM_LEN:
        die("TIM-Form %s %s" % (cl, im))
    return md1, tim


def beben(a):
    edt, vh, vb = struct.unpack_from("<III", a, 8)
    if (edt, vh, vb) != (0x6018, 0x60D8, 0x6EF8): die("Sektionsoffsets weichen ab: %x %x %x" % (edt, vh, vb))
    if a[vh:vh + 4] != b"pBAV": die("kein pBAV")
    nprog = struct.unpack_from("<H", a, vh + 0x12)[0]
    sizetab = vh + 0x820 + nprog * 0x200
    se_map = bytearray(SE_MAP_BYTES); vh_out = bytearray(VH_SIZE); vbd = bytearray()
    vag_neu = {}; ton_n = 0; rows = []
    for si, (name, rec, hexraw) in enumerate(SAETZE):
        raw = a[edt + rec * 4: edt + rec * 4 + 4]
        if raw.hex() != hexraw: die("Satz %d: %s != %s" % (rec, raw.hex(), hexraw))
        prog = raw[1] & 0x7F; tone = raw[2] >> 4; lagen = (raw[3] >> 5) + 1
        erst = ton_n
        for k in range(lagen):
            t = bytearray(a[vh + 0x820 + prog * 0x200 + (tone + k) * 0x20:][:32])
            vagi = struct.unpack_from("<H", t, 0x16)[0]
            if vagi not in vag_neu:
                pos = vb
                for j in range(vagi): pos += struct.unpack_from("<H", a, sizetab + j * 2)[0] << 3
                ln = struct.unpack_from("<H", a, sizetab + vagi * 2)[0] << 3
                wav = a[pos:pos + ln]
                vag_neu[vagi] = (len(vag_neu) + 1, pos, ln, hashlib.sha1(wav).hexdigest())
                struct.pack_into("<H", vh_out, VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + 2 * vag_neu[vagi][0], ln // 8)
                vbd += wav
            struct.pack_into("<H", t, 0x16, vag_neu[vagi][0])
            vh_out[VAB_HDR + VAB_PROG_TABLE + ton_n * 32: VAB_HDR + VAB_PROG_TABLE + (ton_n + 1) * 32] = t
            ton_n += 1
        r = bytearray(raw); r[1] = 0; r[2] = ((erst & 0xF) << 4) | (raw[2] & 0x0F)
        se_map[si * 4: si * 4 + 4] = r
        rows.append((si, name, rec, raw.hex(" "), bytes(r).hex(" "), prog, tone, lagen,
                     [(v, vag_neu[v][1], vag_neu[v][2], vag_neu[v][3][:12]) for v in sorted(vag_neu)]))
    hdr = a[vh:vh + 32]; prog0 = a[vh + 32:vh + 48]
    vh_out[0:4] = b"pBAV"; vh_out[4:8] = hdr[4:8]; vh_out[8:12] = hdr[8:12]
    struct.pack_into("<I", vh_out, 12, VH_SIZE + len(vbd)); vh_out[16:18] = hdr[16:18]
    struct.pack_into("<H", vh_out, 18, 1); struct.pack_into("<H", vh_out, 20, 16)
    struct.pack_into("<H", vh_out, 22, len(vag_neu)); vh_out[24] = hdr[24]; vh_out[25] = hdr[25]; vh_out[26:32] = hdr[26:32]
    vh_out[VAB_HDR:VAB_HDR + 16] = prog0; vh_out[VAB_HDR] = ton_n
    edt_rec = bytes(se_map) + bytes(vh_out) + struct.pack("<II", SE_MAP_BYTES, 0)
    return edt_rec, bytes(vbd), rows


ESP_VON, ESP_BIS = 0x35D8, 0x36FC
TIM19_VON, TIM19_BIS = 0x18984, 0x1B1C4


def esp(a):
    offs = struct.unpack_from("<23I", a, 8)
    if (offs[18], offs[19] + 4, offs[20]) != (ESP_VON, ESP_BIS, TIM19_VON): die("ESP-Offsets %x %x %x" % (offs[18], offs[19], offs[20]))
    e = a[ESP_VON:ESP_BIS]
    if e[:8] != bytes([0x19] + [0xFF] * 7): die("ESP-Ids %s" % e[:8].hex())
    if struct.unpack_from("<I", a, offs[21] - 4)[0] != 0: die("TIM-Tafel vor RDT[21] != 0")
    t = a[TIM19_VON:TIM19_BIS]
    g = open(os.path.join(REPO, "re15_port", "shared_assets", "RE2", "GATOR40A0_19.TIM"), "rb").read()
    if t != g: die("RDT[0x18984:0x1B1C4] != shared_assets/RE2/GATOR40A0_19.TIM")
    return e


def main():
    install = "--install" in sys.argv
    a = open(RDT, "rb").read()
    e = esp(a)
    md1, tim = lampe(a)
    edt_rec, vbd, rows = beben(a)
    out_dir = os.path.join(REPO, "re15_port", "shared_assets", "RE2") if install else os.path.join(REPO, "build", "r41_g5a_m4")
    inc_dir = os.path.join(REPO, "re15_port", "engine", "src", "gen") if install else out_dir
    os.makedirs(out_dir, exist_ok=True); os.makedirs(inc_dir, exist_ok=True)
    for name, data in (("R704_LAMPE.MD1", md1), ("R704_LAMPE.TIM", tim), ("R704_BEBEN.VBS", edt_rec + vbd), ("R704.ESP", e)):
        open(os.path.join(out_dir, name), "wb").write(data)
        print("  %s %d B md5 %s" % (os.path.join(out_dir, name), len(data), hashlib.md5(data).hexdigest()))
    with open(os.path.join(inc_dir, "r41_g5a_beben_bank.inc"), "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/r41_g5auftritt/re2_m4_7040.py -- DO NOT EDIT.\n"
                " * Mini-Bank shared_assets/RE2/R704_BEBEN.VBS = ROOM7040-Raumbank Satz 12 (RE2 ROOM7040 sub04 Se_on @SCD 0x016AC).\n"
                " * [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer u32 vh_off @size-8][VBD]. */\n")
        for (si, name, rec, rawh, neuh, prog, tone, lagen, vags) in rows:
            o.write(" /* Satz %d %-6s ROOM7040 EDT %d  %s -> %s  Prog %d Ton %d Lagen %d  VAGs %s */\n"
                    % (si, name, rec, rawh, neuh, prog, tone, lagen, vags))
        o.write("#define R41_G5A_BEBEN_EDT_SIZE  %du\n#define R41_G5A_BEBEN_VBD_OFF   %du\n#define R41_G5A_BEBEN_VBD_SIZE  %du\n"
                % (len(edt_rec), len(edt_rec), len(vbd)))
        o.write("#define R41_G5A_SE_BEBEN        0   /* ROOM7040 Satz 12 (sub04 Se_on 36 02 0c 00 01 00 @SCD 0x016AC) */\n")
    for r in rows: print("  Satz", r[0], r[1], "EDT", r[2], r[3], "->", r[4], "Prog", r[5], "Ton", r[6], "Lagen", r[7], r[8])


if __name__ == "__main__":
    main()
