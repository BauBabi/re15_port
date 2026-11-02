import sys, struct, glob, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c_scd import subs_of, RDT
B = r"C:/workspace/git/reAi_v2/info/re2clair/COMMON/BIN/"
tab = {}
for s in range(1, 8):
    d = open(B + "STAGE%d.BIN" % s, "rb").read()
    i = d.find(bytes.fromhex("0c80013c1c6122ac"))+4
    if i < 4: print("stage", s, "kein sw 0x800c611c"); continue
    w = struct.unpack_from("<I", d, i-12)[0]; hi = struct.unpack_from("<I", d, i-16)[0]
    imm = w & 0xffff; imm = imm-0x10000 if imm & 0x8000 else imm
    addr = ((hi & 0xffff) << 16) + imm
    tab[s] = (d, addr - 0x8011a000, i, w, hi, addr)
    print("STAGE%d.BIN sw@0x%X lui=%08x addiu=%08x -> Tabelle 0x%08X = Datei 0x%X" % (s, i, hi, w, addr, addr-0x8011a000))
for p in sorted(glob.glob(RDT + "/ROOM*.RDT")):
    fn = os.path.basename(p); st = int(fn[4], 16)
    if st not in tab: continue
    for d, name, i, s, e, ops, stt in subs_of(p):
        for (a, op, ln) in ops:
            if op == 0x59:
                n = d[a+2]; sd, o = tab[st][0], tab[st][1]
                c, b, l = struct.unpack_from("<3H", sd, o+6*n)
                print(fn, name, "sub%02d" % i, "@0x%05X" % a, "xa %02X" % n, "VOICE1%d ch %d blk %d len %d" % (st, c, b, l))
