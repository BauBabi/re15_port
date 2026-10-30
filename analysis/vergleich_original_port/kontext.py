#!/usr/bin/env python3
"""kontext.py STAGEn ADDR... — fuer eine Overlay-Funktion: Aufrufer (jal), Tabellen-Fundstellen mit Tabellenanfang,
Index und Lade-Stelle, plus ob der Port Tabellenanfang/Ladestelle/Aufrufer zitiert."""
import os, re, struct, sys, subprocess
BASE = 0x80100000
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
st = sys.argv[1]
d = open(os.path.join(ROOT, 'info/Re1.5/PSX/BIN', st + '.BIN'), 'rb').read()
w = struct.unpack_from('<%dI' % (len(d) // 4), d)
ov = lambda x: BASE <= x < BASE + len(d)

_port = None
def port_hat(a):
    global _port
    if _port is None:
        _port = set()
        for top in ('engine', 'platform', 'include', 'tools'):
            for dp, dn, fs in os.walk(os.path.join(ROOT, 're15_port', top)):
                for f in fs:
                    if f.endswith(('.c', '.h')):
                        for m in re.finditer(r'(?:0x|FUN_|LAB_|DAT_)(80[0-9a-fA-F]{6})', open(os.path.join(dp, f), errors='replace').read()):
                            _port.add(int(m.group(1), 16))
    return a in _port

def lader(T):
    hi = (T + 0x8000) >> 16; lo = T & 0xffff; out = []
    for i, x in enumerate(w):
        if x >> 26 == 0x0f and (x & 0xffff) == hi:
            r = (x >> 16) & 31
            for j in range(i + 1, min(i + 10, len(w))):
                y = w[j]
                if (y >> 21) & 31 == r and (y & 0xffff) == lo and (y >> 26) in (0x09, 0x23): out.append(BASE + j * 4); break
    return out

def funk_von(a):
    """Funktionsanfang: rueckwaerts bis hinter `jr ra`+Delay oder Prolog."""
    k = (a - BASE) // 4
    while k > 2:
        if (w[k] >> 16) == 0x27bd and (w[k] & 0x8000): return BASE + k * 4
        if w[k - 2] == 0x03e00008: return BASE + k * 4
        k -= 1
    return BASE

for s in sys.argv[2:]:
    A = int(s, 16)
    print('== %s 0x%08x  (Port zitiert Start: %s)' % (st, A, 'ja' if port_hat(A) else 'nein'))
    for i, x in enumerate(w):
        if x >> 26 == 3 and (((x & 0x3ffffff) << 2) | 0x80000000) == A:
            c = BASE + i * 4; f = funk_von(c)
            print('   jal @0x%08x in Funktion 0x%08x (Port: %s)' % (c, f, 'ja' if port_hat(f) or port_hat(c) else 'nein'))
        elif x == A:
            k = i
            while k > 0 and (w[k - 1] == 0 or (ov(w[k - 1]) and w[k - 1] % 4 == 0)): k -= 1
            T = BASE + k * 4; idx = i - k
            ld = []
            for kk in range(k, i + 1):
                ld += [(BASE + kk * 4, l) for l in lader(BASE + kk * 4)]
            lds = ', '.join('0x%08x@0x%08x in 0x%08x' % (t, l, funk_von(l)) for t, l in ld[:4])
            pz = any(port_hat(t) or port_hat(l) or port_hat(funk_von(l)) for t, l in ld) or port_hat(T)
            print('   Tabelle 0x%08x[%d] (Eintrag @0x%08x); geladen: %s; Port kennt Tabelle/Lader: %s'
                  % (T, idx, BASE + i * 4, lds or '-', 'ja' if pz else 'nein'))
