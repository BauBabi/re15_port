import sys, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\analysis\befunde_runde41\AN_belege\re2_claire\werkzeug")
from re2c_ss import SS
LAUF3 = r"C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\an\capture\lauf3"
s = SS(LAUF3 + "\e22.sav")
ram = s.ram()
open("e22_ram.bin","wb").write(ram)
tgt = int(sys.argv[1],16)
w = 0x0C000000 | ((tgt >> 2) & 0x3ffffff)
for o in range(0, 0x200000, 4):
    if struct.unpack_from("<I", ram, o)[0] == w:
        print("%08x" % (0x80000000 + o))
