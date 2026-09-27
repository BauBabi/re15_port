#!/usr/bin/env python3
"""scan_op.py <bin> <mnemonic-fragment> [ctx]
Disassembliert das ganze Overlay (roh @0x80100000) und zeigt jede Zeile, deren
Text das Fragment enthaelt, mit ctx Zeilen Umgebung.
Beispiele:  scan_op.py EMS26.BIN ",0(s0)"      scan_op.py EMOVL21_S0.BIN "60(s"  """
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scan_sh import dis, BASE

def main():
    path, frag = sys.argv[1], sys.argv[2]
    ctx = int(sys.argv[3]) if len(sys.argv) > 3 else 0
    d = open(path, "rb").read()
    tot = len(d) // 4
    w = struct.unpack("<%dI" % tot, d[:tot*4])
    txt = [dis(w[i], BASE + i*4) for i in range(tot)]
    hits = [i for i in range(tot) if frag in txt[i]]
    print(f"# {os.path.basename(path)}  Treffer fuer '{frag}': {len(hits)}")
    for i in hits:
        if ctx == 0:
            print(f"  {BASE+i*4:08x}: {txt[i]}")
        else:
            print(f"--- 0x{BASE+i*4:08x}")
            for j in range(max(0, i-ctx), min(tot, i+ctx+1)):
                m = ">>" if j == i else "  "
                print(f"  {m} {BASE+j*4:08x}: {txt[j]}")

main()
