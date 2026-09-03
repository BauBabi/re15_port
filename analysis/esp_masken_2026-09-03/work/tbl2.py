"""Engine-Regel-Gegenprobe: L = u16 aus der Overlay-Tabelle @_DAT_800b52c8,
   Trailer = (u32 @L-8 = SLD-Offset, u32 @L-4 = Flag).  Ueber ALLE 1119 Chunks."""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]

ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}
bsscache = {}

stat = collections.Counter()
detail = []
for r in ok:
    f = r['f'].replace(os.sep, '/')
    st = int(f.split('STAGE')[1][0])
    base = os.path.basename(f)
    room = int(base[4:7], 16) & 0xFF
    cut = r['c']
    d = ovl[st]
    o = ADDR[st] - 0x80100000 + room * 0x20 + cut * 2
    L = struct.unpack_from('<H', d, o)[0]
    if f not in bsscache:
        bsscache[f] = open(f, 'rb').read()
    ch = bsscache[f][cut * 0x10000:(cut + 1) * 0x10000]
    sld = r['pred'] if r['sldsize'] else (r['scanoff'] if r['scanoff'] >= 0 else -1)
    if L < 8 or L > 0x10000:
        stat['L ausserhalb'] += 1
        detail.append(('L-range', f, cut, L, sld))
        continue
    t0 = struct.unpack_from('<I', ch, L - 8)[0]
    t1 = struct.unpack_from('<I', ch, L - 4)[0]
    if sld < 0:
        if t1 == 0:
            stat['kein SLD: Flag@L-4 == 0  OK'] += 1
        else:
            stat['kein SLD: Flag != 0  FEHLER'] += 1
            detail.append(('noSLD-flag', f, cut, L, t0, t1))
    else:
        if t1 != 0 and t0 == sld:
            stat['SLD: Trailer zeigt korrekt  OK'] += 1
        else:
            stat['SLD: Trailer FALSCH'] += 1
            detail.append(('SLD-bad', f, cut, 'L=%d' % L, 't0=%d' % t0, 't1=%d' % t1, 'sld=%d' % sld,
                           'P_gemessen=%d' % r['P']))

for k, v in sorted(stat.items()):
    print("%-40s %d" % (k, v))
print()
for x in detail[:20]:
    print("  ", x)
