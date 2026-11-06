import sys, struct
sys.path.insert(0, ".")
from re2ram4 import Ram, SN
for n in (1,2,3,5,9,11,12,20):
    r = Ram(SN % n)
    p = r.u32(0x800ce338)
    vals = [r.s16(p + 2*i) for i in range(10)]
    room = r.u32(0x800ce324)
    print("s%02d *(0x800ce338)=%08x  room=%08x rel=%x  hw: %s  cut=%d" % (n, p, room, p-room, vals, r.u8(0x800ce324-0x1cdc+0x1cdc) ))
