"""Belege fuer den Bericht: Hexdumps + Marker/Version-Korrelation."""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]
ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}


def getbits(d, start, n):
    v = 0
    for i in range(start, start + n):
        w = struct.unpack_from("<H", d, 8 + (i // 16) * 2)[0]
        v = (v << 1) | ((w >> (15 - (i % 16))) & 1)
    return v


# 1) Marker <-> Version
c = collections.Counter()
for r in ok:
    d = open(r['f'], 'rb').read()[r['c'] * 0x10000:(r['c'] + 1) * 0x10000]
    c[(r['ver'], format(getbits(d, r['bits'], 10), '010b'))] += 1
print("Endmarker (10 Bit ab B) nach BSS-Version:")
for k, v in sorted(c.items()):
    print("   ver %d : %s  -> %d Chunks" % (k[0], k[1], v))
print()

# 2) Hexdumps
def dump(path, cut, label):
    st = int(path.split('STAGE')[1][0])
    room = int(os.path.basename(path)[4:7], 16) & 0xFF
    r = None
    for x in ok:
        if x['f'].replace(os.sep, '/') == path and x['c'] == cut:
            r = x
    d = open(path, 'rb').read()[cut * 0x10000:(cut + 1) * 0x10000]
    L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + cut * 2)[0]
    B = r['bits']
    O = 8 + 4 * ((B + 10) // 32 + 1)
    print("--- %s  %s cut %d" % (label, os.path.basename(path), cut))
    print("    Kopf      +0x0000: %s   (rlw=%d id=0x%04x quant=%d ver=%d)" %
          (d[0:8].hex(' '), r['rlw'], r['id'], r['q'], r['ver']))
    print("    B (VLC-Bits, 300 Makrobloecke) = %d ; letztes VLC-Byte = +0x%04x" % (B, (B - 1) // 8 + 8))
    print("    Endmarker 10 Bit @B          = %s" % format(getbits(d, B, 10), '010b'))
    print("    O = 8+4*(floor((B+10)/32)+1) = %d = +0x%04x" % (O, O))
    print("    +0x%04x..: %s   <- u32 entpackte Groesse = %d, dann SLD-Strom" %
          (O, d[O:O + 16].hex(' '), struct.unpack_from('<I', d, O)[0]))
    print("    Tabellenwert L (Overlay @0x%08x + %d*0x20 + %d*2) = %d = 0x%04x" %
          (ADDR[st], room, cut, L, L))
    print("    +0x%04x (L-8): %s   <- Trailer {SLD-Offset=%d, Flag=%d}" %
          (L - 8, d[L - 8:L].hex(' '), struct.unpack_from('<I', d, L - 8)[0],
           struct.unpack_from('<I', d, L - 4)[0]))
    print("    CdRead-Sektoren = (L+0x7ff)>>11 = %d" % ((L + 0x7FF) >> 11))
    print()


dump('info/Re1.5/PSX/STAGE1/ROOM102.BSS', 0, 'MIT Atlas (ver 3)')
dump('info/Re1.5/PSX/STAGE1/ROOM103.BSS', 3, 'MIT Atlas, 4 Byte Extra-Pad (bits%32=22)')
dump('info/Re1.5/PSX/STAGE1/ROOM100.BSS', 0, 'OHNE Atlas (ver 2)')
dump('info/Re1.5/PSX/STAGE1/ROOM102.BSS', 11, 'OHNE Atlas (ver 3, Selbstzeiger-Schwanz)')
dump('info/Re1.5/PSX/STAGE3/ROOM305.BSS', 14, 'ANOMALIE: Atlas in der Datei, L schneidet ihn ab')

# 3) Overlay-Tabelle roh
for st, room in ((1, 2), (5, 10)):
    o = ADDR[st] - 0x80100000 + room * 0x20
    print("Overlay-Tabelle STAGE%d @0x%08x, Raum %d (Datei-Offset 0x%05x), 16 u16:" %
          (st, ADDR[st] + room * 0x20, room, o))
    print("   ", ovl[st][o:o + 32].hex(' '))
    print("   ", [struct.unpack_from('<H', ovl[st], o + i * 2)[0] for i in range(16)])
