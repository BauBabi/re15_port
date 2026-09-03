"""Gegenpruefung zu F_masken_ohne_atlas.md — UNABHAENGIGE Neuvermessung.

Liest NICHT f_groundtruth.json, sondern erhebt alles neu aus:
  info/Re1.5/PSX/BIN/STAGE{1..6}.BIN   (Stage-Tabelle DAT_800b52c8)
  info/Re1.5/PSX/STAGE{1..6}/ROOM*.RDT (Kamera-Cuts + pri_offset + Masken)
  info/Re1.5/PSX/STAGE{1..6}/ROOM*.BSS (Trailer A-8/A-4 + SLD-Block)

Maskenparser = 1:1 aus dem Decompile FUN_800392d4 (RE_15_Quellcode_V2):
  Section: u16 groupCount, u16 maskCountDeclared
  Group  : u16 count, u16 base(+2, NIE gelesen), s16 destX, s16 destY
  Mask   : u8 srcX, u8 srcY, u8 dstX, u8 dstY, u16 depth, u16 size6,
           falls (size6 & 0xf000)==0 -> +u16 w, u16 h (12 B), sonst w=h=(size6>>12)<<3 (8 B)
"""
import struct, glob, os, sys, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sld import sld_decompress, find_sld

TBL = {1: 0x8011EAE4, 2: 0x8011713C, 3: 0x8011D0F8,
       4: 0x80118590, 5: 0x8011DF18, 6: 0x80101E3C}
ROOT = 'info/Re1.5/PSX'
OVL = {s: open('%s/BIN/STAGE%d.BIN' % (ROOT, s), 'rb').read() for s in range(1, 7)}


def table_A(stage, room, cut):
    return struct.unpack_from('<H', OVL[stage],
                              TBL[stage] - 0x80100000 + room * 0x20 + cut * 2)[0]


def parse_masks(d, off):
    gc, mc = struct.unpack_from('<HH', d, off)
    if (gc == 0xFFFF and mc == 0xFFFF) or gc == 0 or mc == 0:
        return 0, []
    p = off + 4 + gc * 8
    out = []
    for i in range(gc):
        n = struct.unpack_from('<H', d, off + 4 + i * 8)[0]
        base = struct.unpack_from('<H', d, off + 4 + i * 8 + 2)[0]
        for _ in range(n):
            sx, sy = d[p], d[p + 1]
            v6 = struct.unpack_from('<H', d, p + 6)[0]
            if (v6 & 0xf000) == 0:
                w, h = struct.unpack_from('<HH', d, p + 8); p += 12
            else:
                w = h = (v6 >> 12) << 3; p += 8
            out.append((sx, sy, w, h, base))
    return len(out), out


def sweep():
    rows = []
    for stage in range(1, 7):
        for rdt in sorted(glob.glob('%s/STAGE%d/ROOM*.RDT' % (ROOT, stage))):
            name = os.path.basename(rdt)[4:8]
            room = int(name[1:3], 16)
            d = open(rdt, 'rb').read()
            if len(d) < 0x60:
                rows.append(dict(rdt=name, stub=True, size=len(d))); continue
            cs = struct.unpack_from('<I', d, 0x24)[0]
            bss = '%s/STAGE%d/ROOM%d%02X.BSS' % (ROOT, stage, stage, room)
            b = open(bss, 'rb').read() if os.path.exists(bss) else None
            for c in range(d[1]):
                po = struct.unpack_from('<I', d, cs + c * 32 + 0x1C)[0]
                nm, ms = parse_masks(d, po) if po < len(d) else (None, [])
                A = table_A(stage, room, c)
                atlas = present = None
                if b is not None and (c + 1) * 0x10000 <= len(b):
                    ch = b[c * 0x10000:(c + 1) * 0x10000]
                    if 8 <= A <= 0x10000:
                        present = struct.unpack_from('<I', ch, A - 4)[0]
                        if present:
                            so = struct.unpack_from('<I', ch, A - 8)[0]
                            sz = struct.unpack_from('<I', ch, so)[0]
                            try:
                                full, end = sld_decompress(ch, so + 4, sz)
                                atlas = [so, sz, full[:4] == b'\x10\x00\x00\x00', end]
                            except Exception as e:
                                atlas = [so, sz, 'ERR:' + str(e), None]
                rows.append(dict(rdt=name, stage=stage, room=room, cut=c, nCut=d[1],
                                 hdr7=d[7], po=po, masks=nm, A=A, present=present,
                                 atlas=atlas, chunks=(len(b) // 0x10000) if b else 0,
                                 maxYh=max([y + h for _, y, _, h, _ in ms], default=0),
                                 maxXw=max([x + w for x, _, w, _, _ in ms], default=0),
                                 bases=sorted(set(m[4] for m in ms))))
    return rows


if __name__ == '__main__':
    rows = sweep()
    json.dump(rows, open(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                      'g_gegenpruefung.json'), 'w'))
    cuts = [r for r in rows if 'cut' in r]
    ok = lambda r: bool(r['atlas']) and r['atlas'][2] is True
    wm = [r for r in cuts if r['masks']]
    print('RDTs', len(set(r['rdt'] for r in rows)), 'davon Stubs',
          len([r for r in rows if r.get('stub')]), 'Cuts', len(cuts))
    print('Cuts mit Masken:', len(wm))
    print('Cuts mit Masken OHNE gueltigen Atlas:', len([r for r in wm if not ok(r)]))
    print('Cuts mit Atlas:', len([r for r in cuts if ok(r)]))
    print('Cuts mit Atlas ohne Masken:', len([r for r in cuts if ok(r) and not r['masks']]))
