#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_tuer_tabelle.py - RE2-Tuerarchiv-Tabelle fuer den Port erzeugen und RE2-Archive pruefen/kopieren.

Runde 31, Teil T2 (analysis/befunde_runde31/tueren_02_re2.md, Abschnitt 6, Schritt 1). Werkzeug fuer den
Bau-Agenten; schreibt NUR dorthin, wohin es ausdruecklich gerufen wird (--inc / --kopiere-nach).

Quelle (RE2 Leon info/re2leon/PSX.EXE, t_addr 0x80010000, selbst gelesen):
  Tabelle @0x8009a520, 55 x 12 B: u16 Tonteil-Groesse (Tonlader FUN_80014cd0 @0x80014d94 lhu s4,0(s1)),
    u16 Modellteil-Groesse, u32 Sektor des Modellteils (Modellteil = Datei + Sektor*0x800; Lader FUN_80015064
    @0x800150b0 lw t2,4(v1), @0x800150f4 lhu v1,2(v1)), u8 Pruefsumme Ton (XOR erstes Byte je 512 B),
    u8 Pruefsumme Modell.
  Dateinummern @0x8009a4b0 (u16 je Archiv; Typ 0 = 234).
  Sonderfall Typ 40 (DOOR28): keine MD1-Umsetzung (tpage +21 / CLUT +31) - @0x80013d78 lbu v1,12(v0),
    @0x80013d7c addiu v0,zero,40, @0x80013d80 beq -> @0x80013d94 a2 = a3 = 0; MD1 steht schon auf 0x95/0x7fc0.
Pruefung je Archiv: Sektor*0x800 + Modellteil == Dateigroesse, Pruefsumme Ton, Tonteil-Ende < Modellteil.

Aufruf:
  python re15_port/tools/tueren/re2_tuer_tabelle.py                      # nur pruefen, Tabelle zeigen
  python re15_port/tools/tueren/re2_tuer_tabelle.py --inc <pfad.inc>     # C-Tabelle schreiben
  python re15_port/tools/tueren/re2_tuer_tabelle.py --kopiere-nach re15_port/shared_assets/RE2/DOOR --nur 00,2E
"""
import argparse
import hashlib
import os
import shutil
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
DOOR = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR")
TAB = 0x8009A520
DATNR = 0x8009A4B0
N = 55


def exe_rd(d, addr, n):
    t_addr = struct.unpack_from("<I", d, 0x18)[0]
    o = 0x800 + addr - t_addr
    return d[o:o + n]


def lesen():
    exe = open(EXE, "rb").read()
    zeilen = []
    for i in range(N):
        e = exe_rd(exe, TAB + i * 12, 12)
        ton, mod, sek = struct.unpack_from("<HHI", e, 0)
        datnr = struct.unpack("<H", exe_rd(exe, DATNR + i * 2, 2))[0]
        p = os.path.join(DOOR, "DOOR%02X.DO2" % i)
        d = open(p, "rb").read()
        ck = 0
        for o in range(0, ton, 512):
            ck ^= d[o]
        ok = (sek * 0x800 + mod == len(d)) and ck == e[8] and ton <= sek * 0x800
        zeilen.append({"nr": i, "roh": e.hex(" "), "ton": ton, "modell": mod, "sektor": sek, "ck_ton": e[8],
                       "ck_mod": e[9], "datei_nr": datnr, "groesse": len(d), "sha1": hashlib.sha1(d).hexdigest(),
                       "ok": ok, "pfad": p})
    return zeilen


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--inc")
    ap.add_argument("--kopiere-nach")
    ap.add_argument("--nur", default="")
    a = ap.parse_args()
    z = lesen()
    bad = [r["nr"] for r in z if not r["ok"]]
    print("Archive %d, Pruefung gut %d, schlecht %s" % (len(z), len(z) - len(bad), bad))
    if bad:
        sys.exit(1)
    for r in z:
        print("DOOR%02X  @0x%08X  %s  Ton 0x%04X  Modell 0x%04X @0x%05X  Datei %6d  sha1 %s" % (
            r["nr"], TAB + r["nr"] * 12, r["roh"], r["ton"], r["modell"], r["sektor"] * 0x800, r["groesse"], r["sha1"][:12]))
    if a.inc:
        with open(a.inc, "w", newline="\n") as f:
            f.write("/* Erzeugt von re15_port/tools/tueren/re2_tuer_tabelle.py - NICHT von Hand aendern.\n")
            f.write(" * RE2-Tuerarchiv-Tabelle aus info/re2leon/PSX.EXE @0x8009a520 (55 x 12 B):\n")
            f.write(" *   Tonteil-Groesse (FUN_80014cd0 @0x80014d94), Modellteil-Groesse, Sektor (FUN_80015064\n")
            f.write(" *   @0x800150b0/@0x800150f4), Pruefsumme Ton. Modellteil = Datei + Sektor * 0x800.\n")
            f.write(" * Typ 40 (DOOR28) ohne MD1-Umsetzung: @0x80013d7c addiu v0,zero,40 / @0x80013d80 beq. */\n")
            f.write("typedef struct { uint16_t ton, modell; uint32_t sektor; uint8_t ck_ton; uint32_t datei; } re2_tuer_arch_t;\n")
            f.write("static const re2_tuer_arch_t re2_tuer_arch[%d] = {\n" % N)
            for r in z:
                f.write("    { 0x%04X, 0x%04X, %2d, 0x%02X, %6d },  /* DOOR%02X @0x%08X %s */\n" % (
                    r["ton"], r["modell"], r["sektor"], r["ck_ton"], r["groesse"], r["nr"], TAB + r["nr"] * 12, r["roh"]))
            f.write("};\n")
        print("geschrieben:", a.inc)
    if a.kopiere_nach:
        nur = [int(x, 16) for x in a.nur.split(",") if x] or list(range(N))
        os.makedirs(a.kopiere_nach, exist_ok=True)
        for i in nur:
            r = z[i]
            ziel = os.path.join(a.kopiere_nach, "DOOR%02X.DO2" % i)
            shutil.copyfile(r["pfad"], ziel)
            assert hashlib.sha1(open(ziel, "rb").read()).hexdigest() == r["sha1"]
            print("kopiert DOOR%02X.DO2 (%d B, sha1 %s)" % (i, r["groesse"], r["sha1"][:12]))


if __name__ == "__main__":
    main()
