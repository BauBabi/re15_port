# Runde 37 Spur D NB1: RE2-SCD-Block roh ausgeben (Walk = tools/re2_sicherung/re2_scd_walk.py, Bloecke direkt aus dem RDT).
import os, sys, struct
BAUM = sys.argv[1]; room = sys.argv[2]; blocks = sys.argv[3:]
sys.path.insert(0, os.path.join(BAUM, "tools", "re2_sicherung"))
os.environ.setdefault("RE15_INFO_DIR", "C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information")
import re2_scd_walk as W
raw = open(os.path.join(BAUM, "info", "re2leon", "PL0", "RDT", "ROOM%s.RDT" % room), "rb").read()
def s16(r, o): return struct.unpack_from("<h", r, o)[0]
for name, s, e in W.rdt_blocks(raw):
    if blocks and name not in blocks: continue
    recs, st = W.walk(raw[s:e])
    print("=== ROOM%s %s @0x%05X..0x%05X %s" % (room, name, s, e, st))
    for off, op, r in recs:
        extra = ""
        nm = W.NAMES.get(op, "?")
        if op in (0x2C, 0x4E) or nm.lower().startswith("aot_set") or nm.lower().startswith("door_aot"):
            if len(r) >= 14:
                extra = " slot=%d sce=%d sat=0x%02x floor=%d rect=(%d,%d,%d,%d)" % (r[1], r[2], r[3], r[4], s16(r,6), s16(r,8), s16(r,10), s16(r,12))
        if op == 0x34:
            extra = " member=0x%02x val=%d" % (r[1], s16(r, 2))
        if op == 0x32 and len(r) >= 8:
            extra = " pos=(%d,%d,%d)" % (s16(r,2), s16(r,4), s16(r,6))
        if op == 0x2E:
            extra = " kind=%d idx=%d" % (r[1], r[2])
        print("  @0x%05X %-26s %s%s" % (s + off, nm, r.hex(" "), extra))
