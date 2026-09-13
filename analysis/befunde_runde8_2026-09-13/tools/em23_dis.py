#!/usr/bin/env python3
"""Minimaler MIPS-R3000-Disassembler fuer EM23_OVL_0000.BIN (@0x80100000)."""
import struct, sys, os
REPO="c:/workspace/git/reAi_v2"
OVL=os.environ.get("OVL", REPO+"/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN")
BASE=0x80100000
D=open(OVL,"rb").read()
R=["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
   "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
def dis(w,a):
    op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; sh=(w>>6)&31; fn=w&63
    imm=w&0xffff; s=imm-0x10000 if imm&0x8000 else imm
    tgt=((a+4)&0xf0000000)|((w&0x3ffffff)<<2)
    if w==0: return "nop"
    if op==0:
        m={0x20:"add",0x21:"addu",0x22:"sub",0x23:"subu",0x24:"and",0x25:"or",0x26:"xor",
           0x27:"nor",0x2a:"slt",0x2b:"sltu",0x18:"mult",0x19:"multu",0x1a:"div",0x1b:"divu"}
        if fn in (0x00,0x02,0x03): return "%s %s,%s,%d"%({0:"sll",2:"srl",3:"sra"}[fn],R[rd],R[rt],sh)
        if fn in (0x04,0x06,0x07): return "%s %s,%s,%s"%({4:"sllv",6:"srlv",7:"srav"}[fn],R[rd],R[rt],R[rs])
        if fn==0x08: return "jr %s"%R[rs]
        if fn==0x09: return "jalr %s,%s"%(R[rd],R[rs])
        if fn==0x10: return "mfhi %s"%R[rd]
        if fn==0x12: return "mflo %s"%R[rd]
        if fn in (0x18,0x19,0x1a,0x1b): return "%s %s,%s"%(m[fn],R[rs],R[rt])
        if fn in m: return "%s %s,%s,%s"%(m[fn],R[rd],R[rs],R[rt])
        return ".word 0x%08X"%w
    if op==1:
        n={0:"bltz",1:"bgez",16:"bltzal",17:"bgezal"}.get(rt,"b?%d"%rt)
        return "%s %s,0x%08X"%(n,R[rs],a+4+s*4)
    if op==2: return "j 0x%08X"%tgt
    if op==3: return "jal 0x%08X"%tgt
    if op==4: return "beq %s,%s,0x%08X"%(R[rs],R[rt],a+4+s*4)
    if op==5: return "bne %s,%s,0x%08X"%(R[rs],R[rt],a+4+s*4)
    if op==6: return "blez %s,0x%08X"%(R[rs],a+4+s*4)
    if op==7: return "bgtz %s,0x%08X"%(R[rs],a+4+s*4)
    if op==8: return "addi %s,%s,%d"%(R[rt],R[rs],s)
    if op==9: return "addiu %s,%s,%d"%(R[rt],R[rs],s)
    if op==10: return "slti %s,%s,%d"%(R[rt],R[rs],s)
    if op==11: return "sltiu %s,%s,%d"%(R[rt],R[rs],s)
    if op==12: return "andi %s,%s,0x%X"%(R[rt],R[rs],imm)
    if op==13: return "ori %s,%s,0x%X"%(R[rt],R[rs],imm)
    if op==14: return "xori %s,%s,0x%X"%(R[rt],R[rs],imm)
    if op==15: return "lui %s,0x%X"%(R[rt],imm)
    ld={0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",0x28:"sb",0x29:"sh",0x2b:"sw"}
    if op in ld: return "%s %s,%d(%s)"%(ld[op],R[rt],s,R[rs])
    if op==0x12: return "cop2 0x%07X"%(w&0x1ffffff)
    return ".word 0x%08X"%w
def main():
    a=int(sys.argv[1],16); n=int(sys.argv[2]) if len(sys.argv)>2 else 48
    for i in range(n):
        ad=a+i*4; off=ad-BASE
        if off<0 or off+4>len(D): break
        w=struct.unpack_from("<I",D,off)[0]
        print("0x%08X  %08X  %s"%(ad,w,dis(w,ad)))
main()
