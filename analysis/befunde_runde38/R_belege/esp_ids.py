#!/usr/bin/env python3
"""List room ESP effect ids (RDT+0x4C idh, +0x50 ptr_end) and dump the row block of one id.
Mirrors re15_esp.c esp_parse_core / esp_rowblk / re15_esp_row_stream byte for byte."""
import struct, sys

def u16(b, o): return b[o] | (b[o+1] << 8)
def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]
def s32(b, o): return struct.unpack_from("<i", b, o)[0]

def parse(raw, idh, pe, zero_ok=False):
    if (idh == 0 and not zero_ok) or idh + 8 > len(raw): return []
    if u32(raw, idh) == 0xFFFFFFFF: return []
    effs = []
    for i in range(64):
        eid = raw[idh + i]
        if eid == 0xFF: break
        ent = s32(raw, pe - i*4)
        st = idh + ent
        w0 = u32(raw, st)
        ca, cb = w0 & 0xFFFF, w0 >> 16
        end = st + (ca*2 + cb + 2)*4
        effs.append(dict(id=eid, start=st, end=end, ca=ca, cb=cb))
    return effs

def rows(raw, eff, sub):
    blk = eff['end']
    sub_off = u16(raw, blk + (sub & 7)*2)
    base = blk + sub_off*4
    streams = u16(raw, base)
    p = base + 4
    out = []
    for s in range(streams):
        nr = u16(raw, p)
        rr = []
        for r in range(nr):
            ro = p + 4 + r*40
            rr.append(raw[ro:ro+40])
        out.append((p, nr, rr))
        p += 4 + nr*40
    return streams, out

def main():
    path = sys.argv[1]
    raw = open(path, 'rb').read()
    if path.upper().endswith('.ESP'):
        pe = ((len(raw) + 3) & ~3) - 4
        effs = parse(raw, 0, pe, True)
    else:
        effs = parse(raw, u32(raw, 0x4C), u32(raw, 0x50))
    print(path, 'ids:', ' '.join('0x%02x(a%d,b%d)' % (e['id'], e['ca'], e['cb']) for e in effs))
    if len(sys.argv) > 2:
        want = int(sys.argv[2], 0)
        subs = [int(sys.argv[3], 0)] if len(sys.argv) > 3 else range(8)
        for e in effs:
            if e['id'] != want: continue
            for sub in subs:
                try:
                    n, st = rows(raw, e, sub)
                except Exception as ex:
                    print(' sub', sub, 'ERR', ex); continue
                print(' id 0x%02x sub %d: streams=%d' % (want, sub, n))
                for (p, nr, rr) in st:
                    print('  stream @0x%05x nrows=%d' % (p, nr))
                    for k, r in enumerate(rr):
                        h = [struct.unpack_from('<h', r, o)[0] for o in range(0, 40, 2)]
                        print('   row%d A=%d B=%d w=%d h=%d acc=(%d,%d,%d) p0e=%d vel=(%d,%d,%d) p16=%d ang=(%d,%d,%d) p1e=%d eul=(%d,%d,%d) gate=%d'
                              % (k, h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7], h[8], h[9], h[10], h[11], h[12], h[13], h[14], h[15], h[16], h[17], h[18], h[19]))

main()
