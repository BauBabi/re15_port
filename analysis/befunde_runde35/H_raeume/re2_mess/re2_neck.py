"""re2_neck.py - Leons Blick-Felder (PL+0x1B8/+0x1C0/+0x1C1, Part8 +0x98..+0xA5) je Halte-Bild der RE2-Mitschnitte (NB4)."""
import sys, struct, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import re2_frames as F
D = os.path.join(os.path.dirname(os.path.abspath(__file__)), "daten")
PL = 0x800CFBF8
for run in sorted(os.listdir(D)):
    p = os.path.join(D, run, "frames.bin")
    if not os.path.exists(p): continue
    fr = F.frames(p)
    print("==", run, len(fr), "frames")
    for i, f in enumerate(fr):
        pl = f["pl"]
        tgt = struct.unpack_from("<I", pl, 0x1b8)[0]
        fl, nb = pl[0x1c0], pl[0x1c1]
        o = nb * 0xAC
        lp = f["lparts"]
        rot = struct.unpack_from("<3h", lp, o + 0x68)
        acc = struct.unpack_from("<2h", lp, o + 0x98)
        st = struct.unpack_from("<2H", lp, o + 0x9c)
        cl = struct.unpack_from("<2h", lp, o + 0xa0)
        sp = lp[o+0xa4], lp[o+0xa5]
        tg = "SELF" if tgt == PL else ("HOLDER" if tgt == f["holder"] else hex(tgt))
        if i < 3 or i % 6 == 0 or i == len(fr) - 1:
            print(f" {i:2d} tgt={tg} fl={fl:02x} nb={nb} rot={rot} acc={acc} step={st} clamp={cl} spd={sp} yaw={F.ent_yaw(pl)} 1c2={pl[0x1c2]:02x} 170={struct.unpack_from('<3h',pl,0x170)}")
