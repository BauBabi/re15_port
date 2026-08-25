#!/usr/bin/env python3
"""Leitet die RE1.5-SCD-Opcode-Laengen DIREKT aus den 95 Handlern der Dispatch-Tabelle
@0x800744a8 der RE1.5 PSX.EXE ab. Der Dispatcher FUN_8003f0a0 ruft nur den Handler;
jeder Handler schiebt task->pc (Feld +0x1c) selbst weiter (`sw vX,28(a0)`).

Symbolische Ausfuehrung: Register tracken "PC + k".
  lw rD,0x1c(a0)   -> rD = PC+0
  addiu rD,rS,imm  -> PC+(k+imm)
  sw   rS,0x1c(a0) -> STORE k  (= Laenge)
Mehrere STOREs = variable Laenge (z.B. 0x2C Aot_set: 20 oder 28 je pc[3]&0x80).
"""
import struct, os, sys
HERE=os.path.dirname(os.path.abspath(__file__))
REPO=os.path.abspath(os.path.join(HERE,"..","..",".."))
EXE=os.path.join(REPO,"info","Re1.5","PSX.EXE")
D=open(EXE,"rb").read()
TADDR=struct.unpack_from("<I",D,0x18)[0]
def fo(a): return 0x800+(a-TADDR)
def w32(a): return struct.unpack_from("<I",D,fo(a))[0]

DISPATCH=0x800744a8; NOPS=95
TAB=[w32(DISPATCH+4*i) for i in range(NOPS)]
LOADS={0x20:1,0x21:2,0x23:4,0x24:1,0x25:2}
STORES={0x28:1,0x29:2,0x2b:4}

def analyse(start,end):
    sym={}; reads=[]; stores=[]; task={4}
    a=start
    while a<end:
        w=w32(a); op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; fn=w&63
        imm=w&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
        if op==0 and fn==0x21:
            if rt==0 and rs in sym: sym[rd]=sym[rs]
            elif rs==0 and rt in sym: sym[rd]=sym[rt]
            else:
                if (rs in task and rt==0) or (rt in task and rs==0): task.add(rd)
                sym.pop(rd,None)
        elif op==9:
            if rs in sym: sym[rt]=sym[rs]+simm
            else: sym.pop(rt,None)
        elif op in LOADS:
            if rs in task and simm==0x1c and LOADS[op]==4: sym[rt]=0
            else:
                if rs in sym: reads.append((a,sym[rs]+simm,LOADS[op]))
                sym.pop(rt,None)
        elif op in STORES:
            if rs in task and simm==0x1c and STORES[op]==4 and rt in sym: stores.append((a,sym[rt]))
        else:
            if op==0:
                if fn not in (8,9): sym.pop(rd,None)
            elif op in (0x0f,0x0c,0x0d,0x0e,0x0a,0x0b): sym.pop(rt,None)
        a+=4
    return reads,stores

order=sorted(set(TAB))
def nxt(a):
    i=order.index(a); return order[i+1] if i+1<len(order) else a+0x400

if __name__=="__main__":
    print("op  handler      pc+=            max_read")
    for i,h in enumerate(TAB):
        r,s=analyse(h,min(nxt(h),h+0x600))
        sd=sorted(set(k for _,k in s))
        mx=max([k+wd for _,k,wd in r],default=0)
        print("%02x  0x%08x  %-14s  %s"%(i,h,",".join(str(x) for x in sd) or "-",mx or "-"))
