"""Aufgabe C.3 — die 8 STAGE5-Raeume mit Masken aber (angeblich) ohne SLD."""
import struct, os, json, collections

BIN = 'info/Re1.5/PSX/BIN/STAGE%d.BIN'
ADDR = {1: 0x8011eae4, 2: 0x8011713c, 3: 0x8011d0f8, 4: 0x80118590, 5: 0x8011df18, 6: 0x80101e3c}
rows = [json.loads(l) for l in open('analysis/esp_masken_2026-09-03/work/orig_all.jsonl')]
ok = [r for r in rows if 'err' not in r]
ovl = {st: open(BIN % st, 'rb').read() for st in ADDR}
sweep = json.load(open('analysis/esp_masken_2026-09-03/sweep.json'))

idx = {}
for r in ok:
    f = r['f'].replace(os.sep, '/')
    st = int(f.split('STAGE')[1][0])
    room = int(os.path.basename(f)[4:7], 16) & 0xFF
    idx[(st, room, r['c'])] = r

ziel = ['5030', '5040', '5060', '50A0', '50C0', '5110', '5120', '5140']
gesamt_masken = 0
gesamt_sld = 0
for rid in ziel:
    st = int(rid[0])
    room = int(rid[1:3], 16)
    sw = sweep.get('ROOM' + rid, {})
    print("== ROOM%s  (BSS-Datei ROOM%s.BSS, Raum-Index %d)  nCut=%s hdr7=%s" %
          (rid, rid[:3], room, sw.get('nCut'), sw.get('hdr7')))
    for c in sw.get('cuts', []):
        r = idx.get((st, room, c['cut']))
        if r is None:
            print("   cut %2d: masken=%-3d  -> KEIN Chunk in der BSS-Datei" % (c['cut'], c['masks']))
            continue
        L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + c['cut'] * 2)[0]
        sld = r['pred'] if r['sldsize'] else (r['scanoff'] if r['scanoff'] >= 0 else -1)
        sz = r['sldsize'] or r['scansz']
        ch = open(r['f'], 'rb').read()[c['cut'] * 0x10000:(c['cut'] + 1) * 0x10000]
        t0 = struct.unpack_from('<I', ch, L - 8)[0] if 8 <= L <= 0x10000 else -1
        t1 = struct.unpack_from('<I', ch, L - 4)[0] if 8 <= L <= 0x10000 else -1
        print("   cut %2d: masken=%-3d sweep.sld=%s | BSS: sld@%-6s size=%-6s | L=%-6d Trailer=(%d,%d)" %
              (c['cut'], c['masks'], c['sld'], sld if sld >= 0 else '-', sz or '-', L, t0, t1))
        gesamt_masken += 1 if c['masks'] else 0
        gesamt_sld += 1 if (sld >= 0 and t1) else 0
    print()
print("Cuts mit Maskenrecords: %d   davon mit im Spiel AKTIVEM SLD-Atlas: %d" % (gesamt_masken, gesamt_sld))
