"""Kreuztabelle ueber ALLE Raumvarianten: Maskenrecords (RDT) x aktiver SLD-Atlas (BSS+Overlay-Tabelle)."""
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

cross = collections.Counter()
fehlt = collections.Counter()
per_room = {}
for rid, sw in sorted(sweep.items()):
    st = int(rid[4])
    room = int(rid[5:7], 16)
    if st not in ADDR:
        continue
    na = nm = nb = 0
    for c in sw.get('cuts', []):
        r = idx.get((st, room, c['cut']))
        if r is None:
            fehlt[rid] += 1
            continue
        L = struct.unpack_from('<H', ovl[st], ADDR[st] - 0x80100000 + room * 0x20 + c['cut'] * 2)[0]
        ch = open(r['f'], 'rb').read()[c['cut'] * 0x10000:(c['cut'] + 1) * 0x10000]
        t0 = struct.unpack_from('<I', ch, L - 8)[0] if 8 <= L <= 0x10000 else 0
        t1 = struct.unpack_from('<I', ch, L - 4)[0] if 8 <= L <= 0x10000 else 0
        akt = 1 if t1 else 0
        msk = 1 if c['masks'] else 0
        cross[(msk, akt)] += 1
        na += akt
        nm += msk
        nb += 1
    per_room[rid] = (nb, nm, na)

print("Kreuztabelle ueber %d Cuts (alle 6 Stages, beide Spielervarianten):" % sum(cross.values()))
print("   Masken  Atlas   Cuts")
for (m, a), n in sorted(cross.items()):
    print("   %-7s %-7s %d" % ("ja" if m else "nein", "ja" if a else "nein", n))
print()
print("Cuts ohne BSS-Chunk (Datei kuerzer als nCut):", sum(fehlt.values()), dict(list(fehlt.items())[:10]))
print()
print("Raeume mit Masken aber OHNE Atlas (pro Raum):")
for rid, (nb, nm, na) in sorted(per_room.items()):
    if nm and na < nm:
        print("   %s: cuts=%d masken=%d atlas=%d" % (rid, nb, nm, na))
