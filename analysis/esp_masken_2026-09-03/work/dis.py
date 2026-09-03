"""Winziger MIPS-R3000-Disassembler, nur so viel wie fuer FUN_800c47e8 noetig."""
import struct, sys

R = ['zero', 'at', 'v0', 'v1', 'a0', 'a1', 'a2', 'a3', 't0', 't1', 't2', 't3', 't4', 't5', 't6', 't7',
     's0', 's1', 's2', 's3', 's4', 's5', 's6', 's7', 't8', 't9', 'k0', 'k1', 'gp', 'sp', 'fp', 'ra']
SPECIAL = {0x00: 'sll', 0x02: 'srl', 0x03: 'sra', 0x04: 'sllv', 0x06: 'srlv', 0x07: 'srav',
           0x08: 'jr', 0x09: 'jalr', 0x18: 'mult', 0x19: 'multu', 0x1a: 'div', 0x1b: 'divu',
           0x10: 'mfhi', 0x12: 'mflo', 0x20: 'add', 0x21: 'addu', 0x22: 'sub', 0x23: 'subu',
           0x24: 'and', 0x25: 'or', 0x26: 'xor', 0x27: 'nor', 0x2a: 'slt', 0x2b: 'sltu'}
OP = {0x02: 'j', 0x03: 'jal', 0x04: 'beq', 0x05: 'bne', 0x06: 'blez', 0x07: 'bgtz',
      0x08: 'addi', 0x09: 'addiu', 0x0a: 'slti', 0x0b: 'sltiu', 0x0c: 'andi', 0x0d: 'ori',
      0x0e: 'xori', 0x0f: 'lui', 0x20: 'lb', 0x21: 'lh', 0x23: 'lw', 0x24: 'lbu', 0x25: 'lhu',
      0x28: 'sb', 0x29: 'sh', 0x2b: 'sw'}


def dis(w, pc):
    op = w >> 26
    rs, rt, rd = (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
    sa, fn = (w >> 6) & 31, w & 63
    imm = w & 0xFFFF
    simm = imm - 0x10000 if imm & 0x8000 else imm
    if w == 0:
        return 'nop'
    if op == 0:
        m = SPECIAL.get(fn, 'spec%02x' % fn)
        if m in ('sll', 'srl', 'sra'):
            return '%-6s %s,%s,%d' % (m, R[rd], R[rt], sa)
        if m == 'jr':
            return 'jr     %s' % R[rs]
        if m in ('mfhi', 'mflo'):
            return '%-6s %s' % (m, R[rd])
        if m in ('mult', 'multu', 'div', 'divu'):
            return '%-6s %s,%s' % (m, R[rs], R[rt])
        return '%-6s %s,%s,%s' % (m, R[rd], R[rs], R[rt])
    m = OP.get(op, 'op%02x' % op)
    if m in ('beq', 'bne'):
        return '%-6s %s,%s,0x%08x' % (m, R[rs], R[rt], pc + 4 + simm * 4)
    if m in ('blez', 'bgtz'):
        return '%-6s %s,0x%08x' % (m, R[rs], pc + 4 + simm * 4)
    if m == 'lui':
        return 'lui    %s,0x%x' % (R[rt], imm)
    if m in ('j', 'jal'):
        return '%-6s 0x%08x' % (m, (pc & 0xF0000000) | ((w & 0x3FFFFFF) << 2))
    if m in ('lb', 'lh', 'lw', 'lbu', 'lhu', 'sb', 'sh', 'sw'):
        return '%-6s %s,%d(%s)' % (m, R[rt], simm, R[rs])
    return '%-6s %s,%s,%s0x%x' % (m, R[rt], R[rs], '-' if simm < 0 else '', abs(simm))


if __name__ == '__main__':
    path, off, base, n = sys.argv[1], int(sys.argv[2], 0), int(sys.argv[3], 0), int(sys.argv[4])
    d = open(path, 'rb').read()
    for i in range(n):
        w = struct.unpack_from('<I', d, off + i * 4)[0]
        pc = base + i * 4
        print("  %08x  %08x  %s" % (pc, w, dis(w, pc)))
