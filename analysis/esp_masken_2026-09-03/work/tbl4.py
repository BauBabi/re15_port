"""Chunks OHNE SLD: wo hoert der Inhalt wirklich auf?  Last-non-zero vs O(Inhalt) vs L(Tabelle)."""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]
ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}
bsscache = {}

c_lo = collections.Counter()
c_lnz = collections.Counter()
c_ver = collections.Counter()
ex = []
for r in ok:
    f = r['f'].replace(os.sep, '/')
    if r['sldsize'] or r['scanoff'] >= 0:
        continue
    st = int(f.split('STAGE')[1][0])
    room = int(os.path.basename(f)[4:7], 16) & 0xFF
    cut = r['c']
    L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + cut * 2)[0]
    O = 8 + 4 * ((r['bits'] + 10) // 32 + 1)
    if f not in bsscache:
        bsscache[f] = open(f, 'rb').read()
    ch = bsscache[f][cut * 0x10000:(cut + 1) * 0x10000]
    nz = len(ch)
    while nz > 0 and ch[nz - 1] == 0:
        nz -= 1
    c_lo[(L - O, r['ver'])] += 1
    c_lnz[L - nz] += 1
    c_ver[(r['ver'], L - O)] += 1
    if len(ex) < 8 and L - O == 12:
        ex.append((os.path.basename(f), cut, 'ver%d' % r['ver'], 'B=%d' % r['bits'],
                   'O=%d' % O, 'L=%d' % L, 'lastNZ=%d' % nz,
                   'bytes[O-8..L]=' + ch[O - 8:L].hex()))

print("L-O nach Version:", dict(sorted(c_lo.items())))
print()
print("L - lastNonZero:", dict(sorted(c_lnz.items())))
print()
for e in ex:
    print("  ", e)
