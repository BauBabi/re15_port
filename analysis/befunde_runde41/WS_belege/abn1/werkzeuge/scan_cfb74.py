# Unabhaengiger Schreiber-Scan auf 0x800cfb74..0x800cfb77 (RE2-Leon), mit einfacher Konstantenverfolgung je Register.
import struct, os, glob, sys
REPO = r'C:\workspace\git\reAi_v2'
TARGET_LO, TARGET_HI = 0x800cfb74, 0x800cfb77
def bins():
    exe = open(os.path.join(REPO,'info','re2leon','PSX.EXE'),'rb').read()
    taddr, tsize = struct.unpack_from('<II', exe, 0x18)
    yield 'PSX.EXE', exe[0x800:0x800+tsize], taddr
    for p in sorted(glob.glob(os.path.join(REPO,'info','re2leon','COMMON','BIN','*.BIN'))):
        d = open(p,'rb').read()
        yield os.path.basename(p), d, 0x80100000
def s16(x): return x-0x10000 if x & 0x8000 else x
STORES = {0x28:'sb',0x29:'sh',0x2b:'sw',0x2a:'swl',0x2e:'swr'}
REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
hits = []
for name, d, base in bins():
    n = len(d)//4
    reg = {}
    for i in range(n):
        a = base + 4*i
        w = struct.unpack_from('<I', d, 4*i)[0]
        op = w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; imm=w&0xffff
        if op in STORES:
            if rs in reg:
                ea = (reg[rs] + s16(imm)) & 0xffffffff
                if TARGET_LO - 3 <= ea <= TARGET_HI:
                    hits.append((name, a, STORES[op], REGS[rt], s16(imm), REGS[rs], ea))
            continue
        # Register-Schreiber verfolgen
        if op == 0x0f:   # lui
            reg[rt] = (imm << 16) & 0xffffffff; continue
        if op == 0x09:   # addiu
            if rs in reg: reg[rt] = (reg[rs] + s16(imm)) & 0xffffffff
            else: reg.pop(rt, None)
            continue
        if op == 0x0d:   # ori
            if rs in reg: reg[rt] = reg[rs] | imm
            else: reg.pop(rt, None)
            continue
        # alles andere, das rt/rd schreibt, verwirft den Wert
        if op == 0:
            fn = w & 63
            if fn not in (0x08, 0x09, 0x0c, 0x0d, 0x18,0x19,0x1a,0x1b, 0x11, 0x13):  # jr/jalr/syscall/break/mult/div/mthi/mtlo
                reg.pop(rd, None)
            if fn == 0x09: reg.pop(rd, None)
            continue
        if op in (0x02, 0x03):  # j/jal
            if op == 0x03: 
                for r in (2,3,4,5,6,7,8,9,10,11,12,13,14,15,24,25,31): reg.pop(r, None)
            continue
        if op in (0x04,0x05,0x06,0x07,0x01): continue  # branches
        if op in (0x20,0x21,0x22,0x23,0x24,0x25,0x26, 0x0a,0x0b,0x0c,0x0e,0x08, 0x32):
            reg.pop(rt, None); continue
        if op == 0x12:  # cop2
            continue
        reg.pop(rt, None)
for h in hits:
    print('%-18s @%#010x  %-4s %s,%d(%s)  -> ea %#010x' % h)
print('Treffer:', len(hits))
