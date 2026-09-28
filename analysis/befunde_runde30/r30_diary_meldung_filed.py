#!/usr/bin/env python3
"""r30_diary_meldung_filed.py - BELEG: die Meldung "The <name> has been filed."

RE2 zeigt sie nach dem Schliessen des Lesers (Aufruf FUN_8002fe38(0x00af0010, 0xe400, 10, 0)
@0x80072838-4c bzw. @0x80072964-78). a1 & 0xc00 == 0x400 waehlt die Systembank; im
lateinischen Zweig ist das  Basis 0x800A075C + u16[0x800A1E7C + id*2].

RE1.5 traegt DENSELBEN Satz als Prompt-Skript 5 in DEBUG.BIN (Bank @0x800c4fc6, Auswahl
FUN_80027e68) - aber KEIN Aufrufer uebergibt die 5 (siehe Ausgabe unten: alle
`jal 0x80027e68` in EXE, DEBUG.BIN und STAGE1..5 mit ihrem a1/a2).
"""
import os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re2_disasm as D


def zeichen(c):
    if c == 0:
        return " "
    if c == 0x3a:
        return "'"
    if c == 0x57:
        return "."
    if c == 0x1b:
        return "?"
    ch = c + 0x24
    if 0x30 <= ch <= 0x39 or 0x41 <= ch <= 0x5a or 0x61 <= ch <= 0x7a:
        return chr(ch)
    return "<%02x>" % c


def re2():
    d = open(os.path.join(REPO, "info", "re2leon", "PSX.EXE"), "rb").read()
    fo = lambda a: a - 0x80010000 + 0x800
    print("== RE2 Systembank (lateinisch)  Basis 0x800A075C, Offsets u16 @0x800A1E7C")
    for i in range(12):
        off = struct.unpack_from("<H", d, fo(0x800a1e7c + i * 2))[0]
        a = 0x800a075c + off
        out = []
        k = 0
        roh = []
        while k < 200:
            c = d[fo(a) + k]
            roh.append(c)
            k += 1
            if c == 0xfe:
                out.append("<ENDE>")
                break
            if c in (0xf8, 0xf9, 0xfa, 0xfb):
                arg = d[fo(a) + k]
                roh.append(arg)
                k += 1
                out.append({0xf8: "<NAME %02x>", 0xf9: "<farbe %02x>", 0xfa: "<fa %02x>",
                            0xfb: "<JA/NEIN %02x>"}[c] % arg)
                continue
            if c == 0xfc:
                out.append(" / ")
                continue
            if c >= 0xf0:
                out.append("<%02x>" % c)
                continue
            out.append(zeichen(c))
        print("  [%2d] @0x%08x  %s" % (i, a, "".join(out)))
        if i == 10:
            print("        roh: " + " ".join("%02x" % b for b in roh))


def re15():
    d = open(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "BIN", "DEBUG.BIN"), "rb").read()
    B = 0x800c0000
    base = 0x800c4fc6
    n = struct.unpack_from("<H", d, base - B)[0] // 2
    print()
    print("== RE1.5 Prompt-Skripte  DEBUG.BIN @0x800c4fc6, %d Eintraege" % n)
    for i in range(n - 1):                      # der letzte Eintrag grenzt an die naechste Bank
        off = struct.unpack_from("<H", d, base + i * 2 - B)[0]
        end = base + struct.unpack_from("<H", d, base + (i + 1) * 2 - B)[0]
        a = base + off
        out = []
        k = 0
        while a + k < end:
            c = d[a + k - B]
            k += 1
            if c == 1:
                out.append("<ENDE %02x>" % d[a + k - B]); k += 1; continue
            if c == 2:
                out.append("<sprung %02x %02x>" % (d[a + k - B], d[a + k + 1 - B])); k += 2; continue
            if c == 3:
                out.append("<JA/NEIN>"); continue
            if c == 5:
                out.append("<farbe %02x>" % d[a + k - B]); k += 1; continue
            if c == 6:
                out.append("<NAME %02x>" % d[a + k - B]); k += 1; continue
            if c == 8:
                out.append(" / "); continue
            out.append(zeichen(c))
        print("  [%d] @0x%08x  %s" % (i, a, "".join(out)))
        if i == 5:
            print("        roh: " + " ".join("%02x" % b for b in d[a - B:end - B]))


def aufrufer():
    print()
    print("== RE1.5: alle `jal 0x80027e68` (Prompt-Oeffner) mit dem zuletzt gesetzten a1/a2")
    quellen = [("PSX.EXE", os.path.join(REPO, "info", "Re1.5", "PSX.EXE"), 0x80010000, 0x800),
               ("DEBUG.BIN", os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "DEBUG.BIN"),
                0x800c0000, 0)]
    for s in range(1, 7):
        quellen.append(("STAGE%d.BIN" % s,
                        os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "STAGE%d.BIN" % s),
                        0x80100000, 0))

    def reg(c):
        p = c.split(" ")
        return p[1].split(",")[0] if len(p) > 1 else ""
    for name, pfad, base, hdr in quellen:
        d = open(pfad, "rb").read()
        n = (len(d) - hdr) // 4
        for i in range(n):
            w = struct.unpack_from("<I", d, hdr + i * 4)[0]
            if (w >> 26) == 3 and (((w & 0x3ffffff) << 2) | 0x80000000) == 0x80027e68:
                a = base + i * 4
                ctx = []
                for k in range(-10, 2):
                    if i + k < 0:
                        continue
                    ww = struct.unpack_from("<I", d, hdr + (i + k) * 4)[0]
                    ctx.append(D.dis_one(ww, a + k * 4)[0])
                a1 = [c for c in ctx if reg(c) == "a1"]
                a2 = [c for c in ctx if reg(c) == "a2"]
                print("  %-10s @0x%08x   a1: %-22s a2: %s"
                      % (name, a, a1[-1] if a1 else "-", a2[-1] if a2 else "-"))


re2()
re15()
aufrufer()
