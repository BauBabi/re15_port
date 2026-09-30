#!/usr/bin/env python3
"""fn_start.py - Gegenpruefung Runde 34: Funktionsanfang zu einer EXE-/Overlay-Adresse suchen
(rueckwaerts bis zum `addiu sp,sp,-N`, dem ein `jr ra` + Delay-Slot vorausgeht) und die Aufrufer
(jal / Datenwort) des Anfangs listen.
Aufruf: python fn_start.py 0x80038180 [--bin STAGE1.BIN]

⚠ GRENZE (in der Pruefung selbst erlebt): Handler ohne eigenen Stackrahmen (Blatt-Funktionen, die mit
`lui`/`lbu` beginnen) werden uebersprungen -> das Werkzeug meldete fuer 0x80038180..0x80038c24 den Anfang
0x80037c1c, obwohl dort die Unterzustands-Handler 0x80037fd8 / 0x80038314 / 0x80038850 liegen (Tabellen
0x80073fb0 / 0x80073ff0). Ergebnis immer gegen die Dispatch-Tabelle pruefen.
"""
import os, sys, struct, subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm as R  # noqa: E402


def main():
    a = int(sys.argv[1], 16)
    binname = None
    if "--bin" in sys.argv:
        binname = sys.argv[sys.argv.index("--bin") + 1]
    data, fo, path = R.load(a, binname)
    x = a
    while True:
        w = struct.unpack_from("<I", data, fo(x))[0]
        if (w >> 16) == 0x27bd and (w & 0x8000):  # addiu sp,sp,-N
            # vorausgehend jr ra (+ Delay-Slot)?
            w1 = struct.unpack_from("<I", data, fo(x - 8))[0]
            w2 = struct.unpack_from("<I", data, fo(x - 4))[0]
            if w1 == 0x03e00008 or w2 == 0x03e00008:
                break
        x -= 4
        if a - x > 0x4000:
            print("kein Anfang gefunden")
            return
    print(f"Funktionsanfang fuer 0x{a:08x}: 0x{x:08x}")
    subprocess.run([sys.executable, os.path.join(HERE, "zensus.py"), "ziel", hex(x)])


if __name__ == "__main__":
    main()
