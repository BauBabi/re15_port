#!/usr/bin/env python3
"""Gegenpruefung R34: Sammel-Scans (reproduzieren die im Dossier genannten Inline-Befunde).
Aufruf: scans_gegen.py <scan> [ram.bin]
  ptr7c      - alle `lw rX,124(..)` (Aktor+0x7c) mit Store ueber rX in 40 Folgeinstruktionen (fand FUN_8002b498 @0x8002b4c0)
  w98        - alle sh/sb auf Offset 152/153 (Aktor+0x98) ausser sp/at-Basis (SCD Member_set @0x80041220 u. a.)
  resolver   - alle `jal 0x80012d60` mit a0/a2 (nur @0x80018008 Typ 0 und @0x800185b8 Typ 2)
  clutmap    - je Effekt-Spawn (`jal 0x80019700/0x800199d4` mit `lui a0` davor) die CLUT-Zeile
  re2struct  - Strukturmuster von R29 (lh +0x2a -> blez -> 0x5556) in RE2 PSX.EXE + COMMON/BIN und RE1.5 (Kontrolle)
RAM-Dump (fuer nachgeladenen Code ab 0x800bf000) via ss_dump_gegen.py; DEBUG.BIN laedt bei 0x800c0000."""
import struct, sys, os, glob, collections
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, '.claude', 'skills', 're15-psx-disasm', 'scripts'))
import re15_disasm as D
R = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
def exe_bin(p):
    d = open(p, 'rb').read(); t_addr, t_size = struct.unpack_from('<II', d, 0x18); return t_addr, d[0x800:0x800 + t_size]
def re15_bins(ram=None):
    a, d = exe_bin(os.path.join(REPO, 'info/Re1.5/PSX.EXE')); out = [('PSX.EXE', a, d)]
    for p in sorted(glob.glob(os.path.join(REPO, 'info/Re1.5/PSX/BIN/*.BIN'))):
        b = os.path.basename(p); out.append((b, 0x800c0000 if b == 'DEBUG.BIN' else 0x80100000, open(p, 'rb').read()))
    if ram:
        r = open(ram, 'rb').read(); out.append(('RAM-800bf000', 0x800bf000, r[0xbf000:0xd0000]))
    return out
def words(data): return [struct.unpack_from('<I', data, i * 4)[0] for i in range(len(data) // 4)]
def dis(w, a): return D.dis_one(w, a)[0]
def scan_ptr7c(bins):
    for name, base, data in bins:
        W = words(data)
        for i, w in enumerate(W):
            if w >> 26 == 0x23 and (w & 0xffff) == 124 and ((w >> 21) & 31) != 29:
                rt = (w >> 16) & 31
                st = ['%08x %s' % (base + j * 4, dis(W[j], base + j * 4)) for j in range(i + 1, min(len(W), i + 40))
                      if ((W[j] >> 21) & 31) == rt and (W[j] >> 26) in (0x28, 0x29, 0x2b, 0x2a, 0x2e)]
                if st: print('%-12s %08x lw %s,124(%s) | %s' % (name, base + i * 4, R[rt], R[(w >> 21) & 31], '; '.join(st[:4])))
def scan_w98(bins):
    for name, base, data in bins:
        W = words(data)
        for i, w in enumerate(W):
            if (w >> 26) in (0x28, 0x29) and (w & 0xffff) in (152, 153) and ((w >> 21) & 31) not in (29, 1):
                print('%-12s %08x %s' % (name, base + i * 4, ' ; '.join(dis(W[k], base + k * 4) for k in range(max(0, i - 4), i + 1))))
def scan_resolver(bins):
    jw = 0x0c000000 | ((0x80012d60 >> 2) & 0x3ffffff)
    for name, base, data in bins:
        W = words(data)
        for i, w in enumerate(W):
            if w == jw:
                a0 = a2 = '?'
                for k in list(range(max(0, i - 14), i)) + [i + 1]:
                    t = dis(W[k], base + k * 4); p = t.split(' ')
                    if len(p) < 2 or p[0] not in ('ori', 'addiu', 'addu', 'andi', 'lbu', 'lhu', 'lw', 'lh'): continue
                    if p[1].startswith('a2,'): a2 = t
                    if p[1].startswith('a0,'): a0 = t
                print('%-12s @%08x a0: %-24s a2: %s' % (name, base + i * 4, a0, a2))
def scan_clutmap(bins):
    basep = {3: 0x7811 >> 6, 8: 0x7911 >> 6, 0: 0x7951 >> 6, 2: 0x7a51 >> 6, 4: 0x7ad1 >> 6}
    sp = {0x0c000000 | ((0x80019700 >> 2) & 0x3ffffff), 0x0c000000 | ((0x800199d4 >> 2) & 0x3ffffff)}
    rows = collections.defaultdict(set)
    for name, base, data in bins:
        W = words(data)
        for i, w in enumerate(W):
            if w in sp:
                for k in range(i - 1, max(-1, i - 24), -1):
                    x = W[k]
                    if x >> 26 == 0x0f and ((x >> 16) & 31) == 4:
                        imm = x & 0xffff; cat, sub = imm >> 8, imm & 0xff
                        if cat in basep: rows[basep[cat] + (sub >> 3)].add('%s eff%d sub%#x' % (name, cat, sub))
                        break
    for r in sorted(rows): print('CLUT-Zeile (272,%d): %s' % (r, sorted(rows[r])[:10]))
def scan_re2struct():
    files = [('RE2 PSX.EXE',) + exe_bin(os.path.join(REPO, 'info/re2leon/PSX.EXE'))]
    for p in sorted(glob.glob(os.path.join(REPO, 'info/re2leon/COMMON/BIN/*.BIN'))): files.append(('RE2 ' + os.path.basename(p), 0x80100000, open(p, 'rb').read()))
    files.append(('RE15 PSX.EXE',) + exe_bin(os.path.join(REPO, 'info/Re1.5/PSX.EXE')))
    for name, base, data in files:
        W = words(data); hits = []
        for i, w in enumerate(W):
            if w >> 26 == 0x21 and (w & 0xffff) == 42:
                rt = (w >> 16) & 31
                for j in range(i + 1, min(len(W), i + 4)):
                    if W[j] >> 26 == 0x06 and ((W[j] >> 21) & 31) == rt and any((W[k] & 0xffff) == 0x5556 and W[k] >> 26 == 0x0d for k in range(j, min(len(W), j + 40))):
                        hits.append(hex(base + i * 4))
        print('%-22s R29-Muster: %s' % (name, hits))
if __name__ == '__main__':
    what = sys.argv[1]; ram = sys.argv[2] if len(sys.argv) > 2 else None
    if what == 're2struct': scan_re2struct()
    else: {'ptr7c': scan_ptr7c, 'w98': scan_w98, 'resolver': scan_resolver, 'clutmap': scan_clutmap}[what](re15_bins(ram))
