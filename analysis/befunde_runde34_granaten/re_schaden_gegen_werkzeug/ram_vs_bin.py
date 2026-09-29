#!/usr/bin/env python3
"""ram_vs_bin.py - Gegenpruefung Runde 34: vergleicht einen Adressbereich im Savestate-RAM mit der
statischen Binaerdatei (EXE bzw. STAGEn.BIN ueber re15_disasm.load).
Aufruf: python ram_vs_bin.py <save> <addr_hex> <n_bytes> [--bin STAGE1.BIN]
"""
import os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-psx-disasm", "scripts"))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
import re15_disasm as R  # noqa: E402
import re15_ss  # noqa: E402


def main():
    sav = sys.argv[1]
    if not os.path.isabs(sav):
        sav = os.path.join(REPO, sav)
    a = int(sys.argv[2], 16)
    n = int(sys.argv[3], 0)
    binname = sys.argv[sys.argv.index("--bin") + 1] if "--bin" in sys.argv else None
    data, fo, path = R.load(a, binname)
    st = data[fo(a):fo(a) + n]
    r = re15_ss.Ram(sav)
    rm = r.bytes(a, n)
    same = st == rm
    print(f"{os.path.basename(sav)} @0x{a:08x} +{n}: {'GLEICH' if same else 'VERSCHIEDEN'}  ({os.path.basename(path)})")
    if not same:
        for i in range(0, n, 16):
            s1 = st[i:i + 16].hex(" ")
            s2 = rm[i:i + 16].hex(" ")
            mark = "" if s1 == s2 else "  <--"
            print(f"  {a+i:08x} bin: {s1}\n  {'':8s} ram: {s2}{mark}")


if __name__ == "__main__":
    main()
