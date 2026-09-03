"""L (Overlay-Tabelle) gegen die Inhaltsregel: O = 8 + 4*(floor((B+10)/32)+1)."""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]
ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}

dn = collections.Counter()   # kein SLD:  L - O_content
ds = collections.Counter()   # mit SLD:   L - (align4(sldend)+8)
sec = collections.Counter()
zero_tail = collections.Counter()
bsscache = {}
for r in ok:
    f = r['f'].replace(os.sep, '/')
    st = int(f.split('STAGE')[1][0])
    room = int(os.path.basename(f)[4:7], 16) & 0xFF
    cut = r['c']
    L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + cut * 2)[0]
    B = r['bits']
    O = 8 + 4 * ((B + 10) // 32 + 1)
    sld = r['pred'] if r['sldsize'] else (r['scanoff'] if r['scanoff'] >= 0 else -1)
    if sld < 0:
        dn[L - O] += 1
    else:
        ds[L - (((r['sldend'] if r['sldsize'] else r['scanend']) + 3) // 4 * 4 + 8)] += 1
    sec[(L + 2047) // 2048] += 1
    # Nullen zwischen O und L bei kein-SLD-Faellen
    if sld < 0:
        if f not in bsscache:
            bsscache[f] = open(f, 'rb').read()
        ch = bsscache[f][cut * 0x10000:(cut + 1) * 0x10000]
        zero_tail[all(b == 0 for b in ch[O:L])] += 1

print("kein SLD:  L - O_inhalt  ->", dict(dn))
print("mit  SLD:  L - (align4(sldEnde)+8) ->", dict(ds))
print("Bytes zwischen O und L sind Null:", dict(zero_tail))
print("CdRead-Sektoren ceil(L/2048):", dict(sorted(sec.items())))
