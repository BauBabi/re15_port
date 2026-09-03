"""Welche Cuts haben im Original einen aktiven Atlas, aber KEINE vorextrahierte PRI##.TIM im Port?"""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}
sweep = json.load(open('analysis/esp_masken_2026-09-03/sweep.json'))
PORT = 're15_port/shared_assets/PSX/BSS'

fehlt = collections.Counter()
fehlt_mit_masken = collections.Counter()
hat = 0
nfehlt = 0
for rid, sw in sorted(sweep.items()):
    st = int(rid[4])
    room = int(rid[5:7], 16)
    if st not in ADDR:
        continue
    src = 'info/Re1.5/PSX/STAGE%d/ROOM%X%02X.BSS' % (st, st, room)
    if not os.path.exists(src):
        continue
    d = open(src, 'rb').read()
    for c in sw.get('cuts', []):
        cut = c['cut']
        if (cut + 1) * 0x10000 > len(d):
            continue
        L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + cut * 2)[0]
        if not (8 <= L <= 0x10000):
            continue
        ch = d[cut * 0x10000:(cut + 1) * 0x10000]
        if struct.unpack_from('<I', ch, L - 4)[0] == 0:
            continue
        tim = os.path.join(PORT, rid, 'PRI%02d.TIM' % cut)
        if os.path.exists(tim):
            hat += 1
        else:
            nfehlt += 1
            fehlt[rid] += 1
            if c['masks']:
                fehlt_mit_masken[rid] += 1

print("Cuts mit aktivem Atlas im Original: %d" % (hat + nfehlt))
print("   davon mit vorextrahierter PRI##.TIM im Port: %d" % hat)
print("   davon OHNE (Port zeichnet dort nichts):      %d" % nfehlt)
print()
print("Fehlend je Raum (davon mit Maskenrecords):")
for rid, n in sorted(fehlt.items()):
    print("   %s: %d fehlend, davon %d mit Masken" % (rid, n, fehlt_mit_masken.get(rid, 0)))
