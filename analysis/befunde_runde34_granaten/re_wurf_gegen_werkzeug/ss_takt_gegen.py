#!/usr/bin/env python3
"""Gegenpruefung R34: liest je sauberem Savestate 0x800b5456 (VSync-Takt), 0x800aca38 (Modusbits),
0x800acaec (Spieler +0x98 Zielhoehe/Status), 0x800b2354 (Spieler-Hitbox-Versatz 6 B), 0x800b5358 (Licht-Latch).
PATCHED-EXE_* ausgelassen (CLAUDE.md)."""
import sys, os, glob, struct, collections
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', '.claude', 'skills', 're15-savestate-ghidra', 'scripts'))
from re15_ss import Ram
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
cnt = collections.Counter(); bits = collections.Counter(); offs = collections.Counter()
for p in sorted(glob.glob(os.path.join(REPO, 'stage_saves', '*.sav'))):
    n = os.path.basename(p)
    if n.startswith('PATCHED'): continue
    try:
        r = Ram(p)
    except Exception as e:
        print('%-40s FEHLER %s' % (n, e)); continue
    def rd(a, k):
        o = r.base + (a - 0x80000000); return r.blob[o:o + k]
    vs = rd(0x800b5456, 1)[0]; m = struct.unpack('<I', rd(0x800aca38, 4))[0]
    st = struct.unpack('<H', rd(0x800acaec, 2))[0]; off = rd(0x800b2354, 6).hex(); la = rd(0x800b5358, 1)[0]
    print('%-40s vsync=%d aca38=%08x acaec=%04x hbofs=%s latch=%d' % (n, vs, m, st, off, la))
    cnt[vs] += 1; bits[st & 0x1fff] += 1; offs[off] += 1
print('vsync', dict(cnt)); print('acaec&0x1fff', {hex(k): v for k, v in bits.items()}); print('hitbox-ofs', dict(offs))
