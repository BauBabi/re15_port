# Sollwerte fuer den Port-Test: alle lebenden RE2-FX-Plaetze aus Savestate s02 (= sub05-Bild 39) als JSON
import sys, json, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_g5auftritt\analysis\befunde_runde41\G5_belege\abnahme4\werkzeug")
from re2ram4 import Ram, SN
r = Ram(SN % 2)
out = []
for i in range(96):
    a = 0x800D8CF0 + i * 0x7C
    st = r.s16(a + 0x18) & 0xffff
    if not st: continue
    b = bytes(r.u8(a + k) for k in range(0x7C))
    out.append({"platz": i, "status": st, "bank": b[0x1C], "sub": b[0x1E], "opA": b[0], "opB": b[1], "s2": b[2],
                "vel": list(struct.unpack_from("<3h", b, 0x0C)), "acc": list(struct.unpack_from("<3b", b, 0x08)),
                "lokal": list(struct.unpack_from("<3h", b, 0x24)), "ofs": list(struct.unpack_from("<3h", b, 0x2C)),
                "welt": list(struct.unpack_from("<3h", b, 0x34)), "tpage": struct.unpack_from("<H", b, 0x2A)[0],
                "clut": struct.unpack_from("<H", b, 0x32)[0], "skala": struct.unpack_from("<H", b, 0x3A)[0],
                "anim": b[0x21], "countdown": b[0x20], "roh": b.hex()})
json.dump(out, open("soll_s02_pool.json", "w"), indent=1)
print(len(out), "Plaetze ->", "soll_s02_pool.json")
