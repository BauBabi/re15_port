"""Roh-Dump des Chunk-Endes: 24 B vor L, je Klasse."""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]
ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}
bsscache = {}
groups = collections.defaultdict(list)

for r in ok:
    f = r['f'].replace(os.sep, '/')
    st = int(f.split('STAGE')[1][0])
    room = int(os.path.basename(f)[4:7], 16) & 0xFF
    cut = r['c']
    L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + cut * 2)[0]
    O = 8 + 4 * ((r['bits'] + 10) // 32 + 1)
    sld = r['pred'] if r['sldsize'] else (r['scanoff'] if r['scanoff'] >= 0 else -1)
    key = ('SLD' if sld >= 0 else 'kein', r['ver'], L - O)
    if len(groups[key]) < 3:
        if f not in bsscache:
            bsscache[f] = open(f, 'rb').read()
        ch = bsscache[f][cut * 0x10000:(cut + 1) * 0x10000]
        words = [struct.unpack_from('<I', ch, p)[0] for p in range(max(0,L-32), L, 4)]
        groups[key].append((os.path.basename(f), cut, O, L, sld,
                            ' '.join('%d' % w if w < 0x10000 else '%08x' % w for w in words)))

for k in sorted(groups, key=lambda x: (x[0], x[1], x[2])):
    print("== %s  ver%d  L-O=%d   (n_beispiele<=3)" % k)
    for g in groups[k]:
        print("   %-14s c%-2d O=%-6d L=%-6d sld=%-6d | letzte 8 dwords vor L: %s" % g)
