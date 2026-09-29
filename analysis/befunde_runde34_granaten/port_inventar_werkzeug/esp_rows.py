#!/usr/bin/env python3
"""Runde 34 (Granaten) - Port-Inventar: Zeilen (Row-VM) der Granaten-Effekte in CORE00.ESP.

Parst DATA/CORE00.ESP GENAU wie der Port (engine/src/re15_esp.c):
  re15_esp_parse_global  :163-168  ptr_end = round_up4(size)-4, idh_off = 0
  esp_parse_core         :87-148   id-Array vorwaerts, Zeiger-Tabelle abwaerts, eff_start = entry + idh,
                                    eff_end = eff_start + (count_a*2 + count_b + 2)*4
  esp_rowblk             :188-200  Row-Block = eff_end
  re15_esp_row_streams   :202-211  sub_off = u16 @ rowblk + (sub & 7)*2 ; base = sub_off*4 ; streams = u16 @base
  re15_esp_row_stream    :213-236  je Stream: u16 nrows (+2 pad), nrows x 40 Byte
  esp_fx_seed_header     :749-758  CLUT = hdr u16 @+4 + ((sub&0xff)>>3)*0x40, TPAGE = hdr u16 @+6
Zeilenfelder wie re15_esp.h:125-131 (A, B, w, h, accel, 0x0e, vel, 0x16, angvel, 0x1e, euler, 0x26).

Aufruf: esp_rows.py [id:sub ...]  Default: 4:0x0d 3:0x19 3:0x0b  (Granate + Kinder laut Runde-30-Dossier)
Nur lesend.
"""
import os
import struct
import sys

P = os.path.join(os.path.dirname(__file__), '..', '..', '..', 're15_port', 'shared_assets', 'PSX',
                 'DATA', 'CORE00.ESP')


def u16(b, o):
    return struct.unpack_from('<H', b, o)[0]


def s16(b, o):
    return struct.unpack_from('<h', b, o)[0]


def u32(b, o):
    return struct.unpack_from('<I', b, o)[0]


def parse(b):
    size = len(b)
    ptr_end = ((size + 3) & ~3) - 4
    effs = []
    for i in range(8):
        eid = b[i]
        if eid == 0xFF:
            break
        ent = struct.unpack_from('<i', b, ptr_end - 4 * i)[0]
        start = 0 + ent
        w0 = u32(b, start)
        ca, cb = w0 & 0xffff, w0 >> 16
        end = start + (ca * 2 + cb + 2) * 4
        effs.append(dict(id=eid, start=start, end=end, ca=ca, cb=cb,
                         hdr_clut=u16(b, start + 4), hdr_tpage=u16(b, start + 6)))
    return effs


def rows(b, e, sub):
    blk = e['end']
    sub_off = u16(b, blk + (sub & 7) * 2)
    base = blk + sub_off * 4
    streams = u16(b, base)
    out = []
    p = base + 4
    for s in range(streams):
        nr = u16(b, p)
        rr = []
        for r in range(nr):
            o = p + 4 + r * 40
            rr.append((o, b[o:o + 40]))
        out.append((p, nr, rr))
        p += 4 + nr * 40
    return sub_off, base, streams, out


def fmt_row(o, r):
    return (f'@0x{o:05X} A={u16(r,0)} B={u16(r,2)} wh=({s16(r,4)},{s16(r,6)}) '
            f'acc=({s16(r,8)},{s16(r,10)},{s16(r,12)}) p0e=0x{u16(r,14):04x} '
            f'vel=({s16(r,16)},{s16(r,18)},{s16(r,20)}) p16=0x{u16(r,22):04x} '
            f'ang=({s16(r,24)},{s16(r,26)},{s16(r,28)}) p1e=0x{u16(r,30):04x} '
            f'eul=({s16(r,32)},{s16(r,34)},{s16(r,36)}) p26=0x{u16(r,38):04x}')


def main():
    b = open(P, 'rb').read()
    effs = parse(b)
    print(f'CORE00.ESP {len(b)} B, ids: ' + ', '.join(
        f"{e['id']}@0x{e['start']:X}..0x{e['end']:X}(a{e['ca']}/b{e['cb']} clut0x{e['hdr_clut']:04x} tp0x{e['hdr_tpage']:04x})"
        for e in effs))
    reqs = sys.argv[1:] or ['4:0x0d', '3:0x19', '3:0x0b']
    for rq in reqs:
        eid, sub = [int(x, 0) for x in rq.split(':')]
        e = next((x for x in effs if x['id'] == eid), None)
        if not e:
            print(f'Effekt {eid}: nicht in CORE00')
            continue
        sub_off, base, streams, out = rows(b, e, sub)
        clut = (e['hdr_clut'] + ((sub & 0xff) >> 3) * 0x40) & 0xffff
        print(f'\nEffekt {eid} sub 0x{sub:02x}: Row-Sub (sub&7)={sub & 7}, sub_off={sub_off} '
              f'-> base @0x{base:05X}, Streams {streams}, CLUT-Seed 0x{clut:04x}, TPAGE 0x{e["hdr_tpage"]:04x}')
        for si, (p, nr, rr) in enumerate(out):
            print(f'  Stream {si} @0x{p:05X}: {nr} Zeilen')
            for o, r in rr:
                print('    ' + fmt_row(o, r))


if __name__ == '__main__':
    main()
