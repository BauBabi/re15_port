#!/usr/bin/env python3
"""Gegenpruefung R34: Klammer-Tiefe einer C-Datei ab einer Startzeile verfolgen (Kommentare/Strings ignoriert).
Aufruf: brace_depth.py <datei> <startzeile> <zeile1> [zeile2 ...]
Meldet je Zielzeile die offenen Bloecke (Zeilennummern der oeffnenden Klammern) und wo der Startblock schliesst."""
import sys
path = sys.argv[1]; start = int(sys.argv[2]); targets = set(int(x) for x in sys.argv[3:])
L = open(path, encoding='utf-8', errors='replace').read().split('\n')
stack = []; in_bc = False
BS = chr(92)
for i in range(start - 1, len(L)):
    s = L[i]; out = []; j = 0
    while j < len(s):
        if in_bc:
            k = s.find('*/', j)
            if k < 0: j = len(s); break
            in_bc = False; j = k + 2; continue
        if s.startswith('/*', j): in_bc = True; j += 2; continue
        if s.startswith('//', j): break
        if s[j] in ('"', "'"):
            q = s[j]; k = j + 1
            while k < len(s) and s[k] != q:
                k += 2 if s[k] == BS else 1
            j = k + 1; continue
        out.append(s[j]); j += 1
    for ch in out:
        if ch == '{': stack.append(i + 1)
        elif ch == '}' and stack: stack.pop()
    if i + 1 in targets:
        print('Zeile %d: Tiefe %d, offene Bloecke ab Zeilen %s' % (i + 1, len(stack), stack))
    if not stack and i + 1 > start:
        print('Startblock (Zeile %d) schliesst bei Zeile %d' % (start, i + 1)); break
