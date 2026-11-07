# Abnahme 5 G5A: eigener MIPS-Disassembler fuer die RE2-EXE (PS-X EXE, Kopf 0x800, t_addr @0x18) und Overlays.
# Aufruf: re2dis5.py <exe> <start_hex> <n_instr> | re2dis5.py <exe> word <addr_hex> [n]
import sys, struct, capstone
def lade(p):
    b = open(p, 'rb').read()
    if b[:8] == b'PS-X EXE':
        t_addr, t_size = struct.unpack_from('<II', b, 0x18)
        return b, t_addr, 0x800
    return b, 0x80100000, 0
def off(addr, base, hdr): return addr - base + hdr
if __name__ == '__main__':
    p = sys.argv[1]; b, base, hdr = lade(p)
    if sys.argv[2] == 'word':
        a = int(sys.argv[3], 16); n = int(sys.argv[4]) if len(sys.argv) > 4 else 1
        for i in range(n):
            o = off(a + 4*i, base, hdr); print('%08x: %08x' % (a + 4*i, struct.unpack_from('<I', b, o)[0]))
        sys.exit(0)
    a = int(sys.argv[2], 16); n = int(sys.argv[3])
    md = capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32 + capstone.CS_MODE_LITTLE_ENDIAN)
    o = off(a, base, hdr)
    for ins in md.disasm(b[o:o+4*n], a):
        print('%08x: %s  %-7s %s' % (ins.address, ins.bytes.hex(' '), ins.mnemonic, ins.op_str))
