#!/usr/bin/env python3
"""msg_decode.py - alle RE2-Raumtexte (*.msg) mit dem Zeichensatz des Spiels lesen.

Zeichensatz wie bei den Item-Namen (dort an "Knife"/"Hand Gun"/"Magnum"
verifiziert, siehe re2_items.py):
    0x00 = Leerzeichen        0x01 = '.'
    0x13..0x1C = '0'..'9'     0x1D..0x36 = 'A'..'Z'     0x3D..0x56 = 'a'..'z'
    0x3A = '\'' 0x3B = '-'
Steuercodes des Textrenderers aus FUN_80031070:
    0xF7 = Ende (80031130: addiu v0,zero,247 / beq -> Funktionsende)
    0xEE = 238, 0xEF = 239, 0xF0 = 240  (Sprung-/Formatcodes, 8003115C..80031190)
Alles andere wird als {NN} ausgegeben, damit nichts stillschweigend wegfaellt.

Aufruf:
    msg_decode.py all                 alle *.msg unter PL0/RDT dekodieren
    msg_decode.py grep <wort> [...]   nur Treffer zeigen (Gross/Klein egal)
    msg_decode.py one <datei.msg>
"""
import sys, os, glob

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

CTRL = {0xF7: "<END>", 0xEE: "<EE>", 0xEF: "<EF>", 0xF0: "<F0>",
        0xF8: "<F8>", 0xF9: "<F9>", 0xFA: "<FA>", 0xFB: "<FB>",
        0xFC: "<FC>", 0xFD: "<FD>", 0xFE: "<NL>", 0xFF: "<FF>"}


def dec(b):
    if b == 0x00: return " "
    if b == 0x01: return "."
    if b == 0x02: return ","
    if 0x0C <= b <= 0x15: return chr(ord("0") + b - 0x0C)
    if 0x1D <= b <= 0x36: return chr(ord("A") + b - 0x1D)
    if b == 0x37: return "!"
    if b == 0x38: return "?"
    if b == 0x3A: return "'"
    if b == 0x3B: return "-"
    if 0x3D <= b <= 0x56: return chr(ord("a") + b - 0x3D)
    return CTRL.get(b, "{%02X}" % b)


def text(path):
    d = open(path, "rb").read()
    return "".join(dec(b) for b in d)


def files():
    return sorted(glob.glob(os.path.join(RDT, "room*", "msg", "*.msg")))


if __name__ == "__main__":
    mode = sys.argv[1]
    if mode == "one":
        print(text(sys.argv[2]))
    elif mode == "all":
        fs = files()
        print("# %d Nachrichten-Dateien" % len(fs))
        for f in fs:
            print("%-62s %s" % (os.path.relpath(f, RDT), text(f)))
    elif mode == "grep":
        words = [w.lower() for w in sys.argv[2:]]
        fs = files()
        n = 0
        for f in fs:
            t = text(f)
            tl = t.lower()
            if any(w in tl for w in words):
                n += 1
                print("%-62s %s" % (os.path.relpath(f, RDT), t))
        print("--- %d Treffer in %d Dateien fuer %s ---" % (n, len(fs), words))
