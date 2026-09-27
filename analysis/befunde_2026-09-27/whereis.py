#!/usr/bin/env python3
"""whereis.py <overlay.BIN> <addr-hex> [...]
Zu jeder Adresse: Anfang der umschliessenden Funktion (Rueckwaertssuche nach
`jr ra` + Delay-Slot), alle `jal <funcstart>` im selben Overlay, und alle
Wortstellen, die <funcstart> als Zeiger enthalten (Dispatch-Tabellen)."""
import struct, sys

BASE = 0x80100000

def load(p):
    d = open(p, "rb").read()
    n = len(d) // 4
    return struct.unpack("<%dI" % n, d[:n*4])

def func_start(w, i):
    # rueckwaerts bis zum `jr ra` (0x03e00008) der VORIGEN Funktion
    for j in range(i, 0, -1):
        if w[j] == 0x03e00008:
            return j + 2          # Delay-Slot ueberspringen
    return 0

def main():
    path = sys.argv[1]
    w = load(path)
    n = len(w)
    for a_s in sys.argv[2:]:
        a = int(a_s, 16)
        i = (a - BASE) // 4
        fs = func_start(w, i)
        fa = BASE + fs*4
        want_jal = 0x0C000000 | ((fa & 0x0FFFFFFF) >> 2)
        callers = [BASE + k*4 for k in range(n) if w[k] == want_jal]
        ptrs = [BASE + k*4 for k in range(n) if w[k] == fa]
        print(f"0x{a:08x}  func=0x{fa:08x}  (+0x{a-fa:x})")
        print(f"    jal-Aufrufer: {' '.join('0x%08x' % c for c in callers) or '(keine)'}")
        print(f"    Zeiger-Stellen: {' '.join('0x%08x' % p for p in ptrs) or '(keine)'}")

main()
