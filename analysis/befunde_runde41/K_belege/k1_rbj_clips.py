#!/usr/bin/env python3
"""Spur K (Runde 41): Wurzelhoehe je CLIP eines RBJ-Records (RDT +0x5C; EDD: u16 Bildzahl/u16 Offset je Clip,
Bildwort & 0xFFF = Keyframe, & 0x8000 = Tween; Keyframe-Bytes 2..3 = Wurzel-y s16).
    python k1_rbj_clips.py <STAGE> <RAUM> <rec>
"""
import os, sys, struct, statistics
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
s16 = lambda b, o: struct.unpack_from('<h', b, o)[0]
st, raum, rec = sys.argv[1], sys.argv[2], int(sys.argv[3])
rdt = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE' + st, 'ROOM%s.RDT' % raum), 'rb').read()
s = u32(rdt, 0x5C); total = u32(rdt, s)
pre = u32(rdt, s + total + rec*8); edd = u32(rdt, s + total + rec*8 + 4)
emr = rdt[s + pre + 4: s + edd]; o_frm, ksz = u16(emr, 2), u16(emr, 6)
e = rdt[s + edd:]
nclip = u16(e, 2) // 4
print('ROOM%s rec%d Marker 0x%x: %d Clips' % (raum, rec, u32(rdt, s + pre), nclip))
for c in range(nclip):
    cnt, off = u16(e, c*4), u16(e, c*4 + 2)
    kfs = [u32(e, off + i*4) & 0xFFF for i in range(cnt)]
    ys = [s16(emr, o_frm + k*ksz + 2) for k in kfs]
    print('  Clip %2d: %3d Bilder  Wurzel-y Bild0 %6d  min %6d  max %6d' % (c, cnt, ys[0] if ys else 0, min(ys) if ys else 0, max(ys) if ys else 0))
