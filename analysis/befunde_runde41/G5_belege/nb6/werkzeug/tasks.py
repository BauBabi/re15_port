# RE2-SCD-Taskplaetze 0x800d7860 + i*0x174 (FUN_800536c4) je Savestate: Status-Byte, pc (+0x1c) als RDT-Offset
import sys
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_g5auftritt\analysis\befunde_runde41\G5_belege\abnahme4\werkzeug")
from re2ram4 import Ram, SN
for n in [int(x) for x in sys.argv[1:]]:
    r = Ram(SN % n)
    rdt = r.u32(0x800ce324)
    print("=== s%02d RDT 0x%08X" % (n, rdt))
    for i in range(10):
        a = 0x800d7860 + i * 0x174
        pc = r.u32(a + 0x1c)
        kopf = " ".join("%02x" % r.u8(a + k) for k in range(0x20))
        rel = pc - rdt if rdt <= pc < rdt + 0x40000 else -1
        print("  Platz %d @%08X pc %08X (RDT+0x%05X) kopf %s" % (i, a, pc, rel if rel >= 0 else 0, kopf))
