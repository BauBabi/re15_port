"""Alle Port-Nachrichten in engine/src/gen/*.inc (Byte-Felder), die eine Ja/Nein-Auswahl (0x03) tragen: Frageseite wie fragen_zensus.py."""
import re, glob, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fragen_zensus import seiten
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/engine/src/gen'
n = 0
for p in sorted(glob.glob(os.path.join(GEN, '*.inc'))):
    t = open(p, errors='replace').read()
    for m in re.finditer(r'(?:static\s+)?const\s+(?:unsigned char|uint8_t)\s+(\w+)\s*\[\s*\d*\s*\]\s*=\s*\{([^}]*)\}', t):
        name, body = m.group(1), m.group(2)
        try: b = bytes(int(x, 0) for x in re.findall(r'0x[0-9a-fA-F]+|\d+', body))
        except ValueError: continue
        if len(b) > 2000 or b[:2] != b'\x04\x02': continue
        s, frage = seiten(b)
        if not frage: continue
        n += 1
        z = s[-1]
        ok = len(z) == 1 or (len(z) == 2 and 0x22 + z[1] < 160)
        print('%-28s %-48s Seiten %d, Frageseite %d Zeile(n) %s -> %s' % (os.path.basename(p), name, len(s), len(z), [0x22 + w for w in z], 'frei' if ok else 'UEBERDECKT'))
print('Port-Fragen:', n)
