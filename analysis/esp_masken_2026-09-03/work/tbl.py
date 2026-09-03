import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
# _DAT_800b52c8 = <addr>  aus RE_15_Quellcode_Overlays/STAGE<N>/FUN_*.c (Init-Funktion des Overlays)
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}

rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]

meas = {}
for r in ok:
    f = r['f'].replace(os.sep, '/')
    st = int(f.split('STAGE')[1][0])
    base = os.path.basename(f)              # ROOM10A.BSS
    room = int(base[4:7], 16) & 0xFF        # 0x10A & 0xFF = 0x0A
    meas[(st, room, r['c'])] = r

for shift in (0, 0x800):
    print("=== Datei-Offset = vaddr - 0x80100000 + 0x%x" % shift)
    tot = hit = 0
    for st in (1, 2, 3, 4, 5, 6):
        d = open(BIN % st, 'rb').read()
        off = ADDR[st] - 0x80100000 + shift
        sub = hitsub = 0
        bad = []
        for (s2, room, cut), r in sorted(meas.items()):
            if s2 != st:
                continue
            o = off + room * 0x20 + cut * 2
            if o < 0 or o + 2 > len(d):
                continue
            v = struct.unpack_from('<H', d, o)[0]
            sub += 1
            if r['P'] > 0 and v == r['P']:
                hitsub += 1
            elif r['P'] > 0:
                bad.append((room, cut, hex(v), r['P']))
        print("  STAGE%d @0x%08x file+0x%05x : %d/%d Treffer (Chunks mit SLD-Trailer)" %
              (st, ADDR[st], off, hitsub, sum(1 for (s2, ro, c), r in meas.items() if s2 == st and r['P'] > 0)))
        if bad[:3]:
            print("     Beispiel-Fehlschlaege:", bad[:3])
        tot += sum(1 for (s2, ro, c), r in meas.items() if s2 == st and r['P'] > 0)
        hit += hitsub
    print("  GESAMT %d/%d" % (hit, tot))
