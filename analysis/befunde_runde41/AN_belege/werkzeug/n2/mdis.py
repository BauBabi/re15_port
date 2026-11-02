import sys, struct
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS32, CS_MODE_LITTLE_ENDIAN
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
ram = open(sys.argv[1] if len(sys.argv) > 3 and sys.argv[3].endswith('.bin') else "e22_ram.bin","rb").read()
def dis(a, n):
    for i in range(n):
        x = a + 4*i; w = struct.unpack_from("<I", ram, x & 0x1fffff)[0]
        ins = list(md.disasm(struct.pack("<I", w), x))
        print("  %08x: %08x  %s" % (x, w, (ins[0].mnemonic + " " + ins[0].op_str) if ins else "?"))
if __name__ == "__main__":
    dis(int(sys.argv[1],16), int(sys.argv[2]))
