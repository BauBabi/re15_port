# Runde 37 Spur D NB1: alle RE2-SCD-Saetze Member_set (0x34) / Member_set2 (0x35) mit Member 0x11 (= Entity+0x106 nFloor,
# RE2 FUN_80055cb0 case 0x11 `*(u8*)(p+0x83)`) — mit vorangehendem Work_set.
import os, sys, glob, struct
BAUM = sys.argv[1]
sys.path.insert(0, os.path.join(BAUM, "tools", "re2_sicherung"))
os.environ.setdefault("RE15_INFO_DIR", "C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information")
import re2_scd_walk as W
RDT = os.path.join(BAUM, "info", "re2leon", "PL0", "RDT")
for f in sorted(glob.glob(os.path.join(RDT, "ROOM*.RDT"))):
    raw = open(f, "rb").read()
    room = os.path.basename(f)[4:8]
    for name, s, e in W.rdt_blocks(raw):
        recs, st = W.walk(raw[s:e])
        ws = None
        for off, op, r in recs:
            if op == 0x2E:
                ws = (r[1], r[2])
            if op == 0x34 and r[1] == 0x11:
                v = struct.unpack_from("<h", r, 2)[0]
                print("%s %s @0x%05X member_set 0x11 = %d  work_set=%s  [%s]" % (room, name, s + off, v, ws, st))
            if op == 0x35 and len(r) > 1 and r[1] == 0x11:
                print("%s %s @0x%05X member_set2 0x11 %s work_set=%s" % (room, name, s + off, r.hex(" "), ws))
