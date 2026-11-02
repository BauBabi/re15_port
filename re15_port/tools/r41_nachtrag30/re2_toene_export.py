#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur N (Nachtraege 30, 36): die zwei RE2-Toene der Ventilkette, BYTEGLEICH nach re15_port/shared_assets/RE2/.

1. SOUND/SUB_34.BGM = info/re2leon/COMMON/SOUND/SUB_34.BGM (RE2-Alarm ohne Musik, 27812 B): die Sirene aus ROOM6000.
   Beleg (Dossier N_nachtrag30.md §5): RE1.5 ROOM6000 = Tabellen-Eintrag 0xFF65 (UNK_80074828[0x62]) -> MAIN25, und MAIN25.BGM ist BYTEGLEICH RE2
   MAIN11.BGM; RE2 selbst setzt beim Alarm alle Labor-Raeume auf MAIN 0x11, ROOM6000 @ROOM6040 0x02360 `57 00 00 06 11 ff 00 00`. Sirene dort =
   Kanal 2, Programm 1, VAG 2 (4384 B), Note 60, 0,804 s an / 0,142 s aus. SUB_34 traegt dieselbe Welle (VAG 2 bytegleich) und dieselbe
   Stimme (VAG 1 bytegleich), ohne die Musik: Kanal 1 + Echo Kanal 2, Periode 0,946 s, Schleife `b0 63 14` .. `b0 63 1e`.
2. BRUECKE4080.EDT/.VH/.VB = snd0 von RE2 ROOM4080 (zentrale Bruecke des Kanalsystems, msg 31 "Insert the valve handle and rotate it to move the
   central bridge up or down"): sub13 @0x031EE senkt die Bruecke, @0x031F2 `36 02 0f 00 ...` Se_on Satz 0x0F = Motor (VAG 13, Flags 2..3 =
   Dauerschleife), @0x03314 `36 02 10 01 ...` Se_on Satz 0x10 = Halt unten (VAG 14), beide Stimme 6 (EDT Byte 3 = 0x16): der Halt schneidet den Motor.

Aufruf: python re2_toene_export.py [--pruefen]   (--pruefen: nur vergleichen, nichts schreiben). Rueckgabe 0 = alles bytegleich.
"""
import hashlib
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
RE2 = os.path.join(REPO, "info", "re2leon")
DST = os.path.join(REPO, "re15_port", "shared_assets", "RE2")
MAIN25 = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "SOUND", "MAIN25.BGM")


def bank_sub(d):
    n = len(d)
    vb, vh, sq2 = struct.unpack_from("<III", d, n - 16)
    return vb, vh, sq2


def vags(d, vh, vb):
    nprog = struct.unpack_from("<H", d, vh + 0x12)[0]
    nvag = struct.unpack_from("<H", d, vh + 0x16)[0]
    groessen = [g << 3 for g in struct.unpack_from("<%dH" % (nvag + 1), d, vh + 0x820 + nprog * 0x200)]
    pos, aus = vb, []
    for g in groessen:
        aus.append(d[pos:pos + g])
        pos += g
    return aus


def main():
    nur_pruefen = "--pruefen" in sys.argv
    fehler = 0
    # ---- 1. Alarm ---------------------------------------------------------------------------------------------
    sub34 = open(os.path.join(RE2, "COMMON", "SOUND", "SUB_34.BGM"), "rb").read()
    main11 = open(os.path.join(RE2, "COMMON", "SOUND", "MAIN11.BGM"), "rb").read()
    main25 = open(MAIN25, "rb").read()
    print("MAIN25.BGM (RE1.5) == MAIN11.BGM (RE2):", main25 == main11, "md5", hashlib.md5(main25).hexdigest())
    fehler += 0 if main25 == main11 else 1
    vb, vh, sq2 = bank_sub(sub34)
    v34 = vags(sub34, vh, vb)
    n = len(main25)
    mvh, mvb = struct.unpack_from("<I", main25, n - 8)[0], struct.unpack_from("<I", main25, n - 12)[0]
    vm = vags(main25, mvh, mvb)
    for k in (1, 2):
        g = v34[k] == vm[k]
        print("  SUB_34 VAG %d == MAIN25 VAG %d: %s (%d B, sha1 %s)" % (k, k, g, len(v34[k]), hashlib.sha1(v34[k]).hexdigest()[:12]))
        fehler += 0 if g else 1
    z = os.path.join(DST, "SOUND", "SUB_34.BGM")
    if nur_pruefen:
        ok = os.path.exists(z) and open(z, "rb").read() == sub34
        print("  %-22s %s" % ("SOUND/SUB_34.BGM", "ok" if ok else "FEHLT/ANDERS"))
        fehler += 0 if ok else 1
    else:
        open(z, "wb").write(sub34)
        print("  -> SOUND/SUB_34.BGM %d B md5 %s" % (len(sub34), hashlib.md5(sub34).hexdigest()))
    # ---- 2. Bruecke -------------------------------------------------------------------------------------------
    pfad = os.path.join(RE2, "PL0", "RDT", "ROOM4080.RDT")
    rdt = open(pfad, "rb").read()
    src = os.path.join(RE2, "PL0", "RDT", "room4080")
    for off, roh, was in ((0x031F2, "36020f0000004c04d8dc5c12", "sub13 Se_on 0x0F Motor"), (0x03314, "3602100100008403f8f89411", "sub13 Se_on 0x10 Halt")):
        g = rdt[off:off + 12].hex() == roh
        print("ROOM4080 @0x%05X %s: %s" % (off, was, "gleich" if g else "ABWEICHUNG " + rdt[off:off + 12].hex()))
        fehler += 0 if g else 1
    for quelle, ziel, kopf in (("snd0.edt", "BRUECKE4080.EDT", 0x08), ("snd0.vh", "BRUECKE4080.VH", 0x0C), ("snd0.vb", "BRUECKE4080.VB", 0x10)):
        b = open(os.path.join(src, quelle), "rb").read()
        o = struct.unpack_from("<I", rdt, kopf)[0]
        g = rdt[o:o + len(b)] == b
        print("%-9s RDT-Kopf +0x%02X @0x%05X %6d B %s md5 %s" % (quelle, kopf, o, len(b), "gleich" if g else "ABWEICHUNG", hashlib.md5(b).hexdigest()))
        fehler += 0 if g else 1
        if quelle == "snd0.edt":
            print("   EDT Satz 0x0F = %s, Satz 0x10 = %s" % (b[0x3C:0x40].hex(" "), b[0x40:0x44].hex(" ")))
            fehler += 0 if b[0x3C:0x44] == bytes.fromhex("0001531600016416") else 1
        p = os.path.join(DST, ziel)
        if nur_pruefen:
            ok = os.path.exists(p) and open(p, "rb").read() == b
            print("  %-22s %s" % (ziel, "ok" if ok else "FEHLT/ANDERS"))
            fehler += 0 if ok else 1
        else:
            open(p, "wb").write(b)
            print("  -> %s" % ziel)
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
