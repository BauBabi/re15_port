import sys
# rawdis.py <exe|st1..st6> <loaddr_hex> <hiaddr_hex>  -- dependency-freier MIPS-Disasm (kein capstone noetig)
# exe: PSX.EXE file_off = 0x800 + (addr - 0x80010000); stN: STAGEN.BIN file_off = addr - 0x80100000 (KEIN Header)
import os
_ROOT = os.path.join("C:" + os.sep, "workspace", "git", "reAi_v2", "info", "Re1.5")
EXE = os.path.join(_ROOT, "PSX.EXE")
ST  = lambda n: os.path.join(_ROOT, "PSX", "BIN", "STAGE%s.BIN" % n)
which=sys.argv[1]; lo=int(sys.argv[2],16); hi=int(sys.argv[3],16)
if which=="exe":
    path=EXE; off=lambda a: 0x800+(a-0x80010000)
else:
    path=ST(which[2] if len(which)>2 else "1"); off=lambda a: a-0x80100000
data=open(path,"rb").read()
regs=["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"]
def dis(a,i):
    op=i>>26; rs=(i>>21)&31; rt=(i>>16)&31; rd=(i>>11)&31; sh=(i>>6)&31; fn=i&63; imm=i&0xffff
    s=imm-0x10000 if imm&0x8000 else imm
    R=regs
    if i==0: return "nop"
    if op==0:
        if fn==0x21 or fn==0x20: return f"addu {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x23 or fn==0x22: return f"subu {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x25: return f"or {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x24: return f"and {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x26: return f"xor {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x27: return f"nor {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x00: return f"sll {R[rd]},{R[rt]},{sh}"
        if fn==0x02: return f"srl {R[rd]},{R[rt]},{sh}"
        if fn==0x03: return f"sra {R[rd]},{R[rt]},{sh}"
        if fn==0x04: return f"sllv {R[rd]},{R[rt]},{R[rs]}"
        if fn==0x06: return f"srlv {R[rd]},{R[rt]},{R[rs]}"
        if fn==0x07: return f"srav {R[rd]},{R[rt]},{R[rs]}"
        if fn==0x08: return f"jr {R[rs]}"
        if fn==0x09: return f"jalr {R[rd]},{R[rs]}"
        if fn==0x18: return f"mult {R[rs]},{R[rt]}"
        if fn==0x19: return f"multu {R[rs]},{R[rt]}"
        if fn==0x1a: return f"div {R[rs]},{R[rt]}"
        if fn==0x1b: return f"divu {R[rs]},{R[rt]}"
        if fn==0x12: return f"mflo {R[rd]}"
        if fn==0x10: return f"mfhi {R[rd]}"
        if fn==0x2a: return f"slt {R[rd]},{R[rs]},{R[rt]}"
        if fn==0x2b: return f"sltu {R[rd]},{R[rs]},{R[rt]}"
        return f".word 0x{i:08x} (spec fn=0x{fn:02x})"
    if op==2: return f"j 0x{((a+4)&0xf0000000)|((i&0x3ffffff)<<2):08x}"
    if op==3: return f"jal 0x{((a+4)&0xf0000000)|((i&0x3ffffff)<<2):08x}"
    if op==4: return f"beq {R[rs]},{R[rt]},0x{a+4+(s<<2):08x}"
    if op==5: return f"bne {R[rs]},{R[rt]},0x{a+4+(s<<2):08x}"
    if op==6: return f"blez {R[rs]},0x{a+4+(s<<2):08x}"
    if op==7: return f"bgtz {R[rs]},0x{a+4+(s<<2):08x}"
    if op==1:
        if rt==0: return f"bltz {R[rs]},0x{a+4+(s<<2):08x}"
        if rt==1: return f"bgez {R[rs]},0x{a+4+(s<<2):08x}"
        if rt==17: return f"bgezal {R[rs]},0x{a+4+(s<<2):08x}"
        return f"regimm rt={rt}"
    if op==8: return f"addi {R[rt]},{R[rs]},{s}"
    if op==9: return f"addiu {R[rt]},{R[rs]},{s}"
    if op==0xa: return f"slti {R[rt]},{R[rs]},{s}"
    if op==0xb: return f"sltiu {R[rt]},{R[rs]},{s}"
    if op==0xc: return f"andi {R[rt]},{R[rs]},0x{imm:x}"
    if op==0xd: return f"ori {R[rt]},{R[rs]},0x{imm:x}"
    if op==0xe: return f"xori {R[rt]},{R[rs]},0x{imm:x}"
    if op==0xf: return f"lui {R[rt]},0x{imm:x}"
    if op==0x20: return f"lb {R[rt]},{s}({R[rs]})"
    if op==0x21: return f"lh {R[rt]},{s}({R[rs]})"
    if op==0x22: return f"lwl {R[rt]},{s}({R[rs]})"
    if op==0x23: return f"lw {R[rt]},{s}({R[rs]})"
    if op==0x24: return f"lbu {R[rt]},{s}({R[rs]})"
    if op==0x25: return f"lhu {R[rt]},{s}({R[rs]})"
    if op==0x26: return f"lwr {R[rt]},{s}({R[rs]})"
    if op==0x28: return f"sb {R[rt]},{s}({R[rs]})"
    if op==0x29: return f"sh {R[rt]},{s}({R[rs]})"
    if op==0x2a: return f"swl {R[rt]},{s}({R[rs]})"
    if op==0x2b: return f"sw {R[rt]},{s}({R[rs]})"
    if op==0x2e: return f"swr {R[rt]},{s}({R[rs]})"
    if op==0x12: return f"cop2 0x{i:08x}"
    if op==0x32: return f"lwc2 ${(i>>16)&31},{s}({R[rs]})"
    if op==0x3a: return f"swc2 ${(i>>16)&31},{s}({R[rs]})"
    return f".word 0x{i:08x} (op=0x{op:02x})"
a=lo
while a<hi:
    o=off(a)
    i=int.from_bytes(data[o:o+4],"little")
    print(f"{a:08x} [{o:06x}]: {i:08x}  {dis(a,i)}")
    a+=4
