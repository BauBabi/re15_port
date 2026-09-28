#!/usr/bin/env python3
"""Disassembliert BIN/DEBUG.BIN (Statusschirm-/Textmodul, laedt nach 0x800C0000).

re15_disasm.py kennt nur PSX.EXE und die STAGE-Overlays; dieses Werkzeug benutzt dessen
Dekoder (dis_one) auf den DEBUG.BIN-Bytes. Dateioffset = Adresse - 0x800C0000 (dieselbe
Regel wie re15_port/tools/gen_item_prompt_data.py, dort byte-verifiziert).

    python analysis/befunde_runde30/sicherung_werkzeug/dis_debugbin.py 0x800c0258 40
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
import re15_disasm  # noqa: E402

BASE = 0x800C0000


def main():
    a = int(sys.argv[1], 0)
    n = int(sys.argv[2], 0) if len(sys.argv) > 2 else 40
    d = open(os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "DEBUG.BIN"), "rb").read()
    print("; info/Re1.5/PSX/BIN/DEBUG.BIN (%d B) @0x%08x, Datei 0x%05x" % (len(d), a, a - BASE))
    for i in range(n):
        adr = a + i * 4
        w = struct.unpack_from("<I", d, adr - BASE)[0]
        s, call = re15_disasm.dis_one(w, adr)
        print("  %08x: %08x  %s" % (adr, w, s))


if __name__ == "__main__":
    main()
