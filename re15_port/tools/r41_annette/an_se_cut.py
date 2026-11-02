#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""an_se_cut.py — Runde 41 Spur AN (Nachbesserung 1): schneidet die vier RE2-Toene der Annette-Szene (ROOM5090/6030 im
Port) und baut daraus die Mini-VAB-Bank ANNETTE_SE.VBS. Satzformat wie ELZASE.VBS / LIFT6030.VBS / TOENE39.VBS
(tools/fahrstuhl_r37/re2_lift6030_cut.py, tools/r39_b/toene39_se_cut.py). Satznummern = RE15_AN_SE_* in
include/re15_annette_r41.h. Dossier: analysis/befunde_runde41/AN_belege/an_se.md.

⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (Beta -> Retail, Sound ist RE2). Vorbild ist die RE2-Claire-Szene "Annette trifft
   den mutierten William" = info/re2clair/PL1/RDT/ROOM6141.RDT MAIN sub02 @0x00E32..0x01192 (Zeitachse und Aufnahme:
   analysis/befunde_runde41/AN_belege/re2_claire/zeitachse.md).

QUELLEN (alles selbst gelesen; der Selbsttest unten prueft jedes zitierte Byte):

  Satz 0 KRACH   ROOM6141 sub02 @0x01040 `36 03 0b 00 ..` Se_on(Bank 3, 0x0B). Bank 3 ist in ROOM6141 KEINE RDT-Bank
                 (Kopf +0x14 = 0), sondern die ENEMSE-Bank, die der Raum-Lader zur Laufzeit waehlt. Kette (Claire PAL
                 SLES_109.72, mit Capstone disassembliert):
                   Se_on-Handler = SCD-Tabelle 0x8009f4f8[0x36] = 0x8005278c: @0x800527ac `lbu a3,1(s0)` (Bank),
                     @0x800527b0 `lh a0,2(s0)` (Satz), @0x8005287c-98 a0 = Bank<<24 | Satz<<16, `jal 0x8005734c`;
                   SE-Verteiler 0x8005734c: @0x80057354 `srl t1,a0,0x18` (Bank), @0x800573b0 `lw a1,0x3658(at)` =
                     EDT-Zeiger[Bank] @0x800d3658+4*Bank, @0x800573b8 + Satz*4, @0x800573ec `lw a2,-0xf78(at)` =
                     VH-Zeiger[Bank] @0x800cf088+4*Bank, Ton = VH + Prog*0x200 + Ton*0x20 + 0x820 (@0x8005740c-1c);
                   ENEMSE-Lader @0x80055b90: @0x80055bd0 `lbu s5,-0x4221(s5)` = Bank-Nummer 0x800cbddf,
                     @0x80055bf8/@0x80055c00 `ori s1,s1,0x8610` / `sw s1,0x3664(at)` -> EDT-Zeiger[3] = 0x801f8610,
                     @0x80055c2c `lw v0,-0x4d8(at)` = TOC 0x8009fb28 + Bank*0x10 (Leon: 0x800a7b1c);
                   Bank-Nummer = Zeile der Raum-Paar-Tabelle 0x8009f464 (Leon 0x800a7400), Suche @0x8004f1b8
                     (Leon FUN_80052b38) mit RAUM_A/B @0x800d07b0/1. Raum-A = Ton-Byte +7 des Birkin-Spawns
                     ROOM6141 @0x00C82 `44 00 01 33 02 60 00 31` = 0x31 (Annette @0x00C6C traegt 0x00).
                 Zeile 64 = `31 28` -> Bank 64. TOC[64] = `a8 0e 00 00 47 0b 00 00 20 1e 01 00 49 0b 00 58`:
                   EDT 0xEA8 B ab Sektor 2887 (= ENEMSE.VBS @0x5A3800), VBD 0x11E20 B ab Sektor 2889 (@0x5A4800).
                 GEMESSEN (DuckStation-Savestates der echten Claire-Disc, AN_belege/re2_claire, Lauf 2 s10/s49/s50):
                   RAUM_A/B = `31 00`, Bank-Nummer 0x800cbddf = 64, EDT-Zeiger[3] = 0x801f8610, VH-Zeiger[3] =
                   0x801f8690, RAM-EDT[0x0B] = `00 00 d2 04`; EDT + VH-Kopf im RAM == ENEMSE.VBS @0x5A3800 (Sektor 2887).
                 Satz 0x0B `00 00 d2 04` -> Prog 0 Ton 13 -> VAG 6 (1,40 s). Leon-EXE: Zeile 64 und TOC[64] gleich,
                 Leon-ENEMSE.VBS @0x5AEC20 bitgleich.
  Satz 1 STURZ   ROOM6141 sub02 @0x010CE `36 02 0a 00 ..` Se_on(2,0x0A) direkt nach @0x010CA `34 17 01 00`
                 Member_set 0x17=1 (Birkin faellt). Bank 2 = Raumbank (RDT-Kopf +0x08/+0x0C/+0x10 = EDT 0xA830,
                 VH 0xA8F0, VB 0xB710). Satz `00 01 23 20`: Prog 1 Ton 2, Byte3>>5 = 1 Zusatzlage -> Ton 2 UND Ton 3
                 (beide VAG 10, gleiche Ton-Bytes). Die Mini-Bank behaelt beide Lagen (Ton 1 und 2, eine Welle).
  Satz 2 BRUELL  ROOM6141 sub02 @0x01104 `36 02 0c 00 ..` Se_on(2,0x0C) direkt nach @0x01102 `29 0a` (Schnitt 0x0A).
                 Satz `00 00 c3 01`: Prog 0 Ton 12 -> VAG 11 (3,26 s).
  Satz 3 ZUG_AN  RE2 Leon ROOM7020 (Nutzerliste RE2_SOUND_LISTE.md "ROOM702.SAP - Train Powering Up") MAIN sub04 =
                 beide Stecker gesetzt: @0x01466 `22 04 88 01` Set(4,0x88)=1 - das Flag heisst "Strom fuer den Zug":
                 sub03 @0x01448 `2b 00 03 00 ff ff` msg 3 "The power already has been supplied to the train!" (bei
                 (4,0x88)=1), ROOM7030 sub12 @0x026B6/@0x026BA Ck(4,0x88)=0 -> msg 0 "...The motor cannot be activated
                 due to the insufficient power supply." Nach Sce_item_lost 0x54/0x55 @0x01476/@0x01478 und Sleep 10
                 @0x014A6 folgt @0x014AA `36 02 0c 00 ..` Se_on(2,0x0C) = DER Ton des Hochfahrens: Satz `00 00 93 00`
                 -> Prog 0 Ton 9 -> VAG 15 (3,62 s), einziger Satz der Folge mit steigendem Ton (Spitze 1066 -> 1528 Hz
                 zwischen 0,45 und 1,50 s, Gegenprobe-Werkzeug AN_belege/werkzeug/an_se_gegenprobe.py). Die anderen
                 Toene von sub04: 0x0A @0x0145A Stecker (0,66 s, vor dem Flag), 0x13 @0x014EC 1580-Hz-Piepton zur
                 Meldung msg 1 "The emergency mode has been activated ...", 0x0B @0x01532 Rauschstoss nach dem Licht-
                 Wechsel (Cut_replace @0x01524-2D). Gegenprobe: Claire ROOM7021 traegt Satz, Ton und Welle bitgleich.

EINZIGE Daten-Aenderungen beim Schnitt (wie re2_lift6030_cut.py):
  (1) Tone-Byte +0x16 (VAG-Index, 1-basiert) und +0x14 (Programm) -> Lage in der Mini-Bank (Programm 0).
  (2) EDT-Byte 2 oberes Nibble (Ton im Programm) und Byte 1 (Programm) -> Lage in der Mini-Bank.
  vol/pan/center/shift/min/max/ADSR/Modus, Prioritaet, Stimme, Zusatzlagen (Byte 3) und die Wellen sind bytegleich.
  VH-Kopf und Programm-Zeile 0 kommen aus der Quelle von Satz 0 (ENEMSE-Bank 64, Prog 0), Zahlen (Groesse, nprog 1,
  Tonsegment 16, nvag) neu gesetzt - genau wie toene39_se_cut.py.

Ausgabe (Standard = Ermittlungsstand): build/r41_annette/se/ANNETTE_SE.VBS + re2_annette_se_bank.inc
Mit --install: re15_port/shared_assets/RE2/ANNETTE_SE.VBS + re15_port/engine/src/gen/re2_annette_se_bank.inc
Optional --claire-exe PFAD: prueft zusaetzlich die Claire-PAL-Anker (Paar-Zeile 64, TOC[64], Lader-Instruktionen).
"""
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))


def _info_wurzeln():
    """info/ dieses Baums, dann info/ des Haupt-Checkouts (git common dir): info/re2clair (Claire-Disc, 2026-11-02)
    ist unversioniert und liegt nur im Haupt-Checkout, nicht in den Worktrees. RE15_INFO=<dir> geht vor."""
    w = []
    if os.environ.get("RE15_INFO"):
        w.append(os.environ["RE15_INFO"])
    w.append(os.path.join(REPO, "info"))
    try:
        import subprocess
        gd = subprocess.run(["git", "-C", REPO, "rev-parse", "--git-common-dir"], capture_output=True,
                            text=True).stdout.strip()
        if gd:
            w.append(os.path.join(os.path.dirname(os.path.abspath(os.path.join(REPO, gd))), "info"))
    except OSError:
        pass
    return w


def info(*teile):
    for wz in _info_wurzeln():
        p = os.path.join(wz, *teile)
        if os.path.isfile(p):
            return p
    return os.path.join(REPO, "info", *teile)


R6141 = info("re2clair", "PL1", "RDT", "ROOM6141.RDT")
R6140 = info("re2leon", "PL0", "RDT", "ROOM6140.RDT")
R7020 = info("re2leon", "PL0", "RDT", "ROOM7020.RDT")
R7021 = info("re2clair", "PL1", "RDT", "ROOM7021.RDT")
R7030 = info("re2leon", "PL0", "RDT", "ROOM7030.RDT")
VBS_C = info("re2clair", "COMMON", "SOUND", "ENEMSE.VBS")
VBS_L = info("re2leon", "COMMON", "SOUND", "ENEMSE.VBS")
EXE_L = info("re2leon", "PSX.EXE")

SEKTOR = 2048
ENEMSE_BANK = 64

# --- Skript-Anker (Datei, Datei-Offset, Rohbytes, was) ------------------------------------------------------------
SKRIPT = [
    (R6141, 0x00C6C, "4400004900080000", "MAIN sub00 Sce_em_set Annette Typ 0x49, Ton-Byte +7 = 0x00"),
    (R6141, 0x00C82, "4400013302600031", "MAIN sub00 Sce_em_set Birkin Typ 0x33, Ton-Byte +7 = 0x31 (= Raum-A)"),
    (R6141, 0x01024, "2907", "sub02 Cut_chg 7"),
    (R6141, 0x01040, "36030b000000000000000000", "sub02 Se_on(3,0x0B) KRACH"),
    (R6141, 0x010CA, "34170100", "sub02 Member_set 0x17=1 (Birkin faellt)"),
    (R6141, 0x010CE, "36020a000000000000000000", "sub02 Se_on(2,0x0A) STURZ"),
    (R6141, 0x01102, "290a", "sub02 Cut_chg 0x0A"),
    (R6141, 0x01104, "36020c000000000000000000", "sub02 Se_on(2,0x0C) BRUELL"),
    (R7020, 0x01448, "2b000300ffff", "sub03 msg 3 'The power already has been supplied to the train!'"),
    (R7020, 0x01466, "22048801", "sub04 Set(4,0x88)=1 Strom fuer den Zug"),
    (R7020, 0x01476, "6254", "sub04 Sce_item_lost 0x54"),
    (R7020, 0x01478, "6255", "sub04 Sce_item_lost 0x55"),
    (R7020, 0x014A6, "090a0a00", "sub04 Sleep 10"),
    (R7020, 0x014AA, "36020c000000000000000000", "sub04 Se_on(2,0x0C) ZUG_AN (Hochfahren)"),
    (R7030, 0x026B6, "21048800", "sub12 Ck(4,0x88)=0 (Zugmotor ohne Strom)"),
    (R7030, 0x026BA, "2b000000ffff", "sub12 msg 0 '... motor cannot be activated ...'"),
]

# --- Leon-EXE (info/re2leon/PSX.EXE, Datei = VA - 0x80010000 + 0x800) ----------------------------------------------
EXE_ANKER = [
    (0x800A7400 + 2 * ENEMSE_BANK, "3128", "Raum-Paar-Tabelle Zeile 64 {0x31,0x28}"),
    (0x800A7B1C + 0x10 * ENEMSE_BANK, "a80e0000470b0000201e0100490b0058", "ENEMSE-TOC[64] EDT 0xEA8@2887 VBD 0x11E20@2889"),
]
# --- Claire PAL (SLES_109.72, nicht im Repo; nur mit --claire-exe) ---------------------------------------------------
CLAIRE_ANKER = [
    (0x8009F464 + 2 * ENEMSE_BANK, "3128", "Raum-Paar-Tabelle Zeile 64"),
    (0x8009FB28 + 0x10 * ENEMSE_BANK, "a80e0000470b0000201e0100490b0058", "ENEMSE-TOC[64]"),
    (0x8009F4F8 + 4 * 0x36, "8c270580", "SCD-Tabelle[0x36] = 0x8005278c (Se_on)"),
    (0x800527AC, "01000792", "lbu a3,1(s0) - Bank"),
    (0x800527B0, "02000486", "lh a0,2(s0) - Satz"),
    (0x80052894, "d35c010c", "jal 0x8005734c"),
    (0x80057354, "024e0400", "srl t1,a0,0x18"),
    (0x800573B0, "5836258c", "lw a1,0x3658(at) - EDT-Zeiger[Bank]"),
    (0x800573EC, "88f0268c", "lw a2,-0xf78(at) - VH-Zeiger[Bank]"),
    (0x80055BD0, "dfbdb592", "lbu s5,-0x4221(s5) - Bank-Nummer 0x800cbddf"),
    (0x80055BF8, "10863136", "ori s1,s1,0x8610"),
    (0x80055C00, "643631ac", "sw s1,0x3664(at) - EDT-Zeiger[3] = 0x801f8610"),
    (0x80055C2C, "28fb228c", "lw v0,-0x4d8(at) - TOC 0x8009fb28"),
    (0x8004F1D8, "64f4c624", "addiu a2,a2,-0xb9c - Paar-Tabelle 0x8009f464"),
]

# (name, quelle, EDT-Basis, VH, VB, satz, EDT-Rohsatz, [Ton-Datei-Off je Lage], VAG-Index, VAG-Off, VAG-Laenge, sha1,
#  Gegenprobe (datei, VAG-Off))
ENEMSE_EDT = 2887 * SEKTOR          # TOC[64] Sektor 2887
ENEMSE_EDT_SIZE = 0xEA8             # TOC[64] +0
ENEMSE_VBD = 2889 * SEKTOR          # TOC[64] Sektor 2889
ENEMSE_VBD_SIZE = 0x11E20           # TOC[64] +8
SAETZE = [
    ("KRACH", VBS_C, ENEMSE_EDT, ENEMSE_EDT + 0x80, ENEMSE_VBD, 0x0B, "0000d204", [0x5A4240], 6, 0x5AEC20, 13104,
     "e6b624dd2e64", (VBS_L, 0x5AEC20)),
    ("STURZ", R6141, 0xA830, 0xA8F0, 0xB710, 0x0A, "00012320", [0x0B350, 0x0B370], 10, 0x11B60, 9568,
     "0348b34ff963", (R6140, 0x0910C)),
    ("BRUELL", R6141, 0xA830, 0xA8F0, 0xB710, 0x0C, "0000c301", [0x0B290], 11, 0x140C0, 9072,
     "dba3cffcb83c", (R6140, 0x0B66C)),
    ("ZUG_AN", R7020, 0x5C90, 0x5D50, 0x6B70, 0x0C, "00009300", [0x06690], 15, 0x167F0, 12736,
     "0bab6e45719e", (R7021, 0x165B4)),
]
# erwartete Abspiel-Tonhoehe (note2pitch2 der Ton-Bytes, vab_common.c) - Gegenprobe gegen die Aufnahme in an_se.md
PITCH = {"KRACH": 0x5F3, "STURZ": 0x400, "BRUELL": 0x1C4, "ZUG_AN": 0x23A}

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x10

LUT = [
    [0x1000, 0x100E, 0x101D, 0x102C, 0x103B, 0x104A, 0x1059, 0x1068, 0x1078, 0x1087, 0x1096, 0x10A5, 0x10B5, 0x10C4, 0x10D4, 0x10E3],
    [0x10F3, 0x1103, 0x1113, 0x1122, 0x1132, 0x1142, 0x1152, 0x1162, 0x1172, 0x1182, 0x1193, 0x11A3, 0x11B3, 0x11C4, 0x11D4, 0x11E5],
    [0x11F5, 0x1206, 0x1216, 0x1227, 0x1238, 0x1249, 0x125A, 0x126B, 0x127C, 0x128D, 0x129E, 0x12AF, 0x12C1, 0x12D2, 0x12E3, 0x12F5],
    [0x1306, 0x1318, 0x132A, 0x133C, 0x134D, 0x135F, 0x1371, 0x1383, 0x1395, 0x13A7, 0x13BA, 0x13CC, 0x13DE, 0x13F1, 0x1403, 0x1416],
    [0x1428, 0x143B, 0x144E, 0x1460, 0x1473, 0x1486, 0x1499, 0x14AC, 0x14BF, 0x14D3, 0x14E6, 0x14F9, 0x150D, 0x1520, 0x1534, 0x1547],
    [0x155B, 0x156F, 0x1583, 0x1597, 0x15AB, 0x15BF, 0x15D3, 0x15E7, 0x15FB, 0x1610, 0x1624, 0x1638, 0x164D, 0x1662, 0x1676, 0x168B],
    [0x16A0, 0x16B5, 0x16CA, 0x16DF, 0x16F4, 0x170A, 0x171F, 0x1734, 0x174A, 0x175F, 0x1775, 0x178B, 0x17A1, 0x17B6, 0x17CC, 0x17E2],
    [0x17F9, 0x180F, 0x1825, 0x183B, 0x1852, 0x1868, 0x187F, 0x1896, 0x18AC, 0x18C3, 0x18DA, 0x18F1, 0x1908, 0x191F, 0x1937, 0x194E],
    [0x1965, 0x197D, 0x1995, 0x19AC, 0x19C4, 0x19DC, 0x19F4, 0x1A0C, 0x1A24, 0x1A3C, 0x1A55, 0x1A6D, 0x1A85, 0x1A9E, 0x1AB7, 0x1ACF],
    [0x1AE8, 0x1B01, 0x1B1A, 0x1B33, 0x1B4C, 0x1B66, 0x1B7F, 0x1B98, 0x1BB2, 0x1BCC, 0x1BE5, 0x1BFF, 0x1C19, 0x1C33, 0x1C4D, 0x1C67],
    [0x1C82, 0x1C9C, 0x1CB7, 0x1CD1, 0x1CEC, 0x1D07, 0x1D22, 0x1D3D, 0x1D58, 0x1D73, 0x1D8E, 0x1DA9, 0x1DC5, 0x1DE0, 0x1DFC, 0x1E18],
    [0x1E34, 0x1E50, 0x1E6C, 0x1E88, 0x1EA4, 0x1EC1, 0x1EDD, 0x1EFA, 0x1F16, 0x1F33, 0x1F50, 0x1F6D, 0x1F8A, 0x1FA7, 0x1FC5, 0x1FE2],
]


def die(msg):
    sys.stderr.write("an_se_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def rel(p):
    """Quellname ab 'info/' (unabhaengig davon, ob die Datei im Worktree oder im Haupt-Checkout lag)."""
    q = os.path.abspath(p).replace(os.sep, "/")
    k = q.rfind("/info/")
    return q[k + 1:] if k >= 0 else os.path.relpath(p, REPO).replace(os.sep, "/")


def note2pitch2(note, fine, center, shift):
    """Wie vab_common.c re15_vab_note2pitch2 (Aufruf audio_pc.c: (min, shift, center, shift))."""
    s = ((fine & 0xFFFF) + (shift & 0xFF)) >> 3
    frac, carry = s, 0
    if s > 15:
        frac, carry = s - 16, 1
    sem = carry + note + 0x3C - center
    octv = int(sem / 12) - 5
    idx = max(0, min(191, (sem % 12) * 16 + frac))
    p = LUT[idx // 16][idx % 16]
    if octv < 0:
        p >>= (-octv) & 31
    elif octv >= 1:
        p = 0x3FFF
    return p & 0xFFFF


def abtastwerte(w):
    """Abtastwerte bis zum Endblock (Flag 1 ohne Wiederholung) - die Wellen sind Einmal-Toene (Flags ... 1, 7)."""
    for b in range(0, len(w), 16):
        if (w[b + 1] & 1) and not (w[b + 1] & 2):
            return (b // 16 + 1) * 28
    return len(w) // 16 * 28


def lesen(p, cache):
    if p not in cache:
        if not os.path.isfile(p):
            die("Quelle fehlt: %s" % p)
        cache[p] = open(p, "rb").read()
    return cache[p]


def main():
    install = "--install" in sys.argv
    claire_exe = None
    if "--claire-exe" in sys.argv:
        claire_exe = sys.argv[sys.argv.index("--claire-exe") + 1]
    if install:
        out_vbs = os.path.join(PORT, "shared_assets", "RE2", "ANNETTE_SE.VBS")
        out_inc = os.path.join(PORT, "engine", "src", "gen", "re2_annette_se_bank.inc")
    else:
        od = os.path.join(REPO, "build", "r41_annette", "se")
        out_vbs = os.path.join(od, "ANNETTE_SE.VBS")
        out_inc = os.path.join(od, "re2_annette_se_bank.inc")
    os.makedirs(os.path.dirname(out_vbs), exist_ok=True)
    os.makedirs(os.path.dirname(out_inc), exist_ok=True)
    cache = {}

    # --- Selbsttest 1: Skript-Anker -----------------------------------------------------------------------------
    for p, off, roh, was in SKRIPT:
        d = lesen(p, cache)
        got = d[off:off + len(roh) // 2].hex()
        if got != roh:
            die("%s @0x%05X (%s): %s != %s" % (rel(p), off, was, got, roh))
    r6141 = lesen(R6141, cache)
    if struct.unpack_from("<5I", r6141, 8) != (0xA830, 0xA8F0, 0xB710, 0, 0):
        die("ROOM6141 Kopf +0x08..+0x18 weicht ab (Bank 3 darf keine RDT-Bank sein)")
    # --- Selbsttest 2: Leon-EXE (Paar-Zeile 64, TOC[64]) ----------------------------------------------------------
    exe = lesen(EXE_L, cache)
    for va, roh, was in EXE_ANKER:
        o = va - 0x80010000 + 0x800
        if exe[o:o + len(roh) // 2].hex() != roh:
            die("Leon-EXE @0x%08X (Datei 0x%X, %s): %s != %s" % (va, o, was, exe[o:o + len(roh) // 2].hex(), roh))
    if claire_exe:
        ce = open(claire_exe, "rb").read()
        for va, roh, was in CLAIRE_ANKER:
            o = va - 0x80010000 + 0x800
            if ce[o:o + len(roh) // 2].hex() != roh:
                die("Claire-EXE @0x%08X (%s): %s != %s" % (va, was, ce[o:o + len(roh) // 2].hex(), roh))
        print("  Claire-PAL-Anker: %d bytegleich (%s)" % (len(CLAIRE_ANKER), claire_exe))
    vbs = lesen(VBS_C, cache)
    if struct.unpack_from("<I", vbs, ENEMSE_EDT + ENEMSE_EDT_SIZE - 8)[0] != 0x80:
        die("ENEMSE Bank 64: Trailer-VH-Offset != 0x80")
    if ENEMSE_VBD + ENEMSE_VBD_SIZE > len(vbs):
        die("ENEMSE Bank 64 VBD ausserhalb der Datei")

    # --- Saetze ---------------------------------------------------------------------------------------------------
    se_map = bytearray(SE_MAP_BYTES)
    vh_out = bytearray(VH_SIZE)
    vbd = bytearray()
    zeilen = []
    ton_i = 0
    ref = None
    for i, (name, p, edt, vh, vb, sid, edt_hex, tone_offs, vagi_soll, vag_off, vag_len, sha, gegen) in enumerate(SAETZE):
        a = lesen(p, cache)
        if a[vh:vh + 4] != b"pBAV":
            die("%s: kein pBAV @0x%X" % (rel(p), vh))
        raw = a[edt + sid * 4:edt + sid * 4 + 4]
        if raw.hex() != edt_hex:
            die("%s Satz 0x%02X @0x%X: %s != %s" % (rel(p), sid, edt + sid * 4, raw.hex(), edt_hex))
        prog, tone, lagen = raw[1] & 0x7F, raw[2] >> 4, (raw[3] >> 5) + 1
        if lagen != len(tone_offs):
            die("%s Satz 0x%02X: %d Lagen statt %d" % (rel(p), sid, lagen, len(tone_offs)))
        nprog = struct.unpack_from("<H", a, vh + 0x12)[0]
        sizetab = vh + 0x820 + nprog * 0x200
        toene = []
        for k, toff in enumerate(tone_offs):
            if vh + 0x820 + prog * 0x200 + (tone + k) * 0x20 != toff:
                die("%s Satz 0x%02X Lage %d: Tone-Offset weicht ab" % (rel(p), sid, k))
            t = bytearray(a[toff:toff + 32])
            if struct.unpack_from("<H", t, 0x16)[0] != vagi_soll:
                die("%s Satz 0x%02X Lage %d: VAG %d statt %d" % (rel(p), sid, k, struct.unpack_from("<H", t, 0x16)[0],
                                                                vagi_soll))
            toene.append(t)
        pos = vb
        for k in range(vagi_soll):
            pos += struct.unpack_from("<H", a, sizetab + k * 2)[0] << 3
        vlen = struct.unpack_from("<H", a, sizetab + vagi_soll * 2)[0] << 3
        if (pos, vlen) != (vag_off, vag_len):
            die("%s: VAG %d @0x%X/%d B statt @0x%X/%d B" % (rel(p), vagi_soll, pos, vlen, vag_off, vag_len))
        wav = a[vag_off:vag_off + vag_len]
        h = hashlib.sha1(wav).hexdigest()
        if not h.startswith(sha):
            die("%s VAG %d sha1 %s" % (rel(p), vagi_soll, h))
        gp, goff = gegen
        if lesen(gp, cache)[goff:goff + vag_len] != wav:
            die("Gegenprobe %s @0x%X: Welle %s nicht bitgleich" % (rel(gp), goff, name))
        t0 = toene[0]
        pitch = note2pitch2(t0[6], t0[5], t0[4], t0[5])
        if pitch != PITCH[name]:
            die("%s: Tonhoehe 0x%X statt 0x%X" % (name, pitch, PITCH[name]))
        if ref is None:
            ref = (a[vh:vh + 32], a[vh + 32 + prog * 16:vh + 48 + prog * 16])
        # --- in die Mini-Bank legen: Ton(e) ab ton_i, eine Welle (VAG i+1) ---------------------------------------
        for k, t in enumerate(toene):
            t = bytearray(t)
            struct.pack_into("<H", t, 0x14, 0)                         # Aenderung (1): Programm 0
            struct.pack_into("<H", t, 0x16, i + 1)                     # Aenderung (1): VAG-Index der Mini-Bank
            o = VAB_HDR + VAB_PROG_TABLE + (ton_i + k) * 32
            vh_out[o:o + 32] = t
        rec = bytearray(raw)
        rec[1] = 0                                                     # Aenderung (2): Programm 0
        rec[2] = ((ton_i & 0xF) << 4) | (raw[2] & 0x0F)                # Aenderung (2): Ton-Lage
        se_map[i * 4:i * 4 + 4] = rec
        struct.pack_into("<H", vh_out, VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + 2 * (i + 1), vag_len // 8)
        n = abtastwerte(wav)
        zeilen.append(dict(i=i, name=name, quelle=rel(p), sid=sid, raw=bytes(raw), rec=bytes(rec), ton_i=ton_i,
                           lagen=lagen, tone_offs=tone_offs, toene=[bytes(t) for t in toene], vagi=vagi_soll,
                           vag_off=vag_off, vag_len=vag_len, sha=h, pitch=pitch, n=n,
                           sek=n / (44100.0 * pitch / 4096.0), gegen="%s @0x%05X" % (rel(gp), goff)))
        ton_i += lagen
        vbd += wav
    if ton_i > 16:
        die("Tonsegment voll")

    hdr, prog0 = ref
    vh_out[0:4] = b"pBAV"
    vh_out[4:12] = hdr[4:12]
    struct.pack_into("<I", vh_out, 12, VH_SIZE + len(vbd))
    vh_out[16:18] = hdr[16:18]
    struct.pack_into("<H", vh_out, 18, 1)                              # nprog
    struct.pack_into("<H", vh_out, 20, 16)                             # Tonsegment = 16 Toene
    struct.pack_into("<H", vh_out, 22, len(SAETZE))                    # nvag
    vh_out[24:32] = hdr[24:32]
    vh_out[VAB_HDR:VAB_HDR + 16] = prog0
    vh_out[VAB_HDR] = ton_i                                            # Toene in Programm 0
    trailer = struct.pack("<II", SE_MAP_BYTES, 0)
    edt_rec = bytes(se_map) + bytes(vh_out) + trailer
    datei = edt_rec + bytes(vbd)
    open(out_vbs, "wb").write(datei)
    fnv = 2166136261
    for c in datei:
        fnv = ((fnv ^ c) * 16777619) & 0xFFFFFFFF

    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/r41_annette/an_se_cut.py -- DO NOT EDIT.\n"
                " * Satz-TOC der Mini-Bank shared_assets/RE2/ANNETTE_SE.VBS (Runde 41 Spur AN, RE2-Ergaenzung: Toene der\n"
                " * Annette-Szene nach RE2 Claire ROOM6141 sub02, Hochfahren nach RE2 Leon ROOM7020 sub04).\n"
                " * Satzformat wie ELZASE.VBS: [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @size-8][VBD].\n"
                " * Herkunft (Datei-Offsets; Belege an_se.md):\n")
        for z in zeilen:
            o.write(" *   %d %-6s %s Satz 0x%02X EDT %s -> %s, Ton %s, VAG %d @0x%05X %5d B sha1 %s\n"
                    % (z["i"], z["name"], z["quelle"], z["sid"], z["raw"].hex(" "), z["rec"].hex(" "),
                       "/".join("@0x%05X" % t for t in z["tone_offs"]), z["vagi"], z["vag_off"], z["vag_len"],
                       z["sha"][:12]))
            o.write(" *       Gegenprobe bitgleich: %s\n" % z["gegen"])
        o.write(" * KRACH: Bank 3 = ENEMSE-Bank 64 (Raum-Paar-Tabelle Zeile 64 `31 28`, TOC[64] Sektor 2887/2889).\n */\n")
        o.write("#define RE2_ANSE_EDT_OFF   0u\n")
        o.write("#define RE2_ANSE_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_ANSE_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_ANSE_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_ANSE_DATEI_GROESSE %du\n" % len(datei))
        o.write("#define RE2_ANSE_DATEI_FNV  0x%08Xu\n" % fnv)
        o.write("#define RE2_ANSE_SE_COUNT  %d\n" % len(zeilen))
        o.write("#define RE2_ANSE_TON_COUNT %d\n" % ton_i)
        for z in zeilen:
            o.write("#define RE2_ANSE_SE_%-7s %d   /* %s Satz 0x%02X, %d Lage(n), VAG %d B */\n"
                    % (z["name"], z["i"], z["quelle"].split("/")[-1], z["sid"], z["lagen"], z["vag_len"]))
        for z in zeilen:
            o.write("#define RE2_ANSE_PITCH_%-7s 0x%03Xu  /* note2pitch2 der Ton-Bytes @0x%05X = %.1f Hz */\n"
                    % (z["name"], z["pitch"], z["tone_offs"][0], 44100.0 * z["pitch"] / 4096.0))
        for z in zeilen:
            o.write("#define RE2_ANSE_LAGEN_%-7s %d\n" % (z["name"], z["lagen"]))
        for z in zeilen:
            o.write("#define RE2_ANSE_ABTAST_%-7s %d   /* bis zum Endblock = %.3f s */\n" % (z["name"], z["n"], z["sek"]))
        o.write("/* Quell-Ton-Bytes je Satz (erste Lage), fuer die Probe: alles ausser +0x14..+0x17 muss gleich sein */\n")
        o.write("#ifdef RE2_ANSE_MIT_QUELLEN\n")
        o.write("static const unsigned char re2_anse_quell_ton[%d][32] = {\n" % len(zeilen))
        for z in zeilen:
            o.write("    { %s },\n" % ", ".join("0x%02X" % b for b in z["toene"][0]))
        o.write("};\n")
        o.write("static const unsigned char re2_anse_quell_edt[%d][4] = {\n" % len(zeilen))
        for z in zeilen:
            o.write("    { %s },\n" % ", ".join("0x%02X" % b for b in z["raw"]))
        o.write("};\n")
        o.write("#endif\n")

    for z in zeilen:
        t = z["toene"][0]
        print("  %d %-6s %s 0x%02X EDT %s -> %s  Ton %s mode%d vol%d pan%d center%d shift%d note%d  VAG %d @0x%05X "
              "%d B  pitch 0x%X = %.1f Hz  %d Abtastw. = %.3f s  sha1 %s  Gegenprobe %s"
              % (z["i"], z["name"], z["quelle"], z["sid"], z["raw"].hex(" "), z["rec"].hex(" "),
                 "/".join("@0x%05X" % x for x in z["tone_offs"]), t[1], t[2], t[3], t[4], t[5], t[6], z["vagi"],
                 z["vag_off"], z["vag_len"], z["pitch"], 44100.0 * z["pitch"] / 4096.0, z["n"], z["sek"],
                 z["sha"][:12], z["gegen"]))
    print("  ANNETTE_SE.VBS: edt_size=%d (vh_off=0x%X) vbd_size=%d gesamt=%d B FNV 0x%08X sha1 %s"
          % (len(edt_rec), SE_MAP_BYTES, len(vbd), len(datei), fnv, hashlib.sha1(datei).hexdigest()))
    print("  -> %s" % out_vbs)
    print("  -> %s" % out_inc)
    return 0


if __name__ == "__main__":
    sys.exit(main())
