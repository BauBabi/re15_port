import struct
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
ram = open("e22_ram.bin","rb").read()
def ins(x):
    w = struct.unpack_from("<I", ram, x & 0x1fffff)[0]
    i = list(md.disasm(struct.pack("<I", w), x))
    return (i[0].mnemonic + " " + i[0].op_str) if i else "?"
for a in [int(l,16) for l in open("sites.txt")]:
    lines = []
    for k in range(-24, 2):
        t = ins(a + 4*k)
        if any(r in t.split(",")[0] for r in ("$a0","$a2","$a3")) or "0x5f68" in t or "0x8009" in t:
            lines.append("%08x %s" % (a+4*k, t))
    print("== %08x | " % a + " ; ".join(lines[-7:]))
