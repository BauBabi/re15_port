#!/usr/bin/env python3
"""Leitet die RE2-SCD-Opcode-Laengen DIREKT aus den 143 Handlern der
Dispatch-Tabelle @0x800a74c8 ab (RE2 hat KEINE Laengen-Datentabelle:
FUN_800536c4 @0x8005371c ruft nur den Handler, jeder Handler schiebt
task->pc (+0x1c) selbst weiter).

Symbolische Ausfuehrung ueber den linearen Instruktionsstrom jedes Handlers:
  Register tracken den Wert "PC + k".
  lw rD,0x1c(task)   -> rD = PC+0
  addiu rD,rS,imm    -> PC+(k+imm)
  sw   rS,0x1c(task) -> STORE  (exakte Laenge, wenn konstant)
  l[bhw]*  rD,off(rS) mit rS=PC+k -> READ bei k+off (Mindestlaenge)
"""
import struct, sys, os
HERE=os.path.dirname(os.path.abspath(__file__))
REPO=os.path.abspath(os.path.join(HERE,"..","..",".."))
EXE=os.path.join(REPO,"info","re2leon","PSX.EXE")
DATA=open(EXE,"rb").read()
TADDR=struct.unpack_from("<I",DATA,0x18)[0]
def fo(a): return 0x800+(a-TADDR)
def w32(a): return struct.unpack_from("<I",DATA,fo(a))[0]

DISPATCH=0x800a74c8
NOPS=0x8F
TAB=[w32(DISPATCH+i*4) for i in range(NOPS)]

LOADS={0x20:1,0x21:2,0x23:4,0x24:1,0x25:2}   # lb lh lw lbu lhu -> width
STORES={0x28:1,0x29:2,0x2b:4}

def analyse(start, end):
    """end = exklusive obere Schranke (naechster Handler)."""
    sym={}          # reg -> k  (Wert = PC + k)
    reads=[]        # (addr, k+off, width)
    stores=[]       # (addr, k)
    a=start
    # a0 = task-Zeiger; wir erkennen den PC-Load ueber Offset 0x1c(a0)
    taskregs={4}    # a0
    while a<end:
        w=w32(a); op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; fn=w&63
        imm=w&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
        if op==0 and fn==0x21:      # addu rd,rs,rt
            if rt==0 and rs in sym: sym[rd]=sym[rs]
            elif rs==0 and rt in sym: sym[rd]=sym[rt]
            else:
                if rs in taskregs and rt==0: taskregs.add(rd)
                elif rt in taskregs and rs==0: taskregs.add(rd)
                sym.pop(rd,None)
        elif op==9:                 # addiu rt,rs,imm
            if rs in sym: sym[rt]=sym[rs]+simm
            else: sym.pop(rt,None)
        elif op in LOADS:
            if rs in taskregs and simm==0x1c and LOADS[op]==4:
                sym[rt]=0           # rt = PC
            else:
                if rs in sym: reads.append((a,sym[rs]+simm,LOADS[op]))
                sym.pop(rt,None)
        elif op in STORES:
            if rs in taskregs and simm==0x1c and STORES[op]==4 and rt in sym:
                stores.append((a,sym[rt]))
        else:
            # alles andere zerstoert das Zielregister
            if op==0:
                if fn not in (8,9): sym.pop(rd,None)   # jr/jalr lassen rd
            elif op in (0x0f,):  sym.pop(rt,None)      # lui
            elif op in (0x0c,0x0d,0x0e,0x0a,0x0b): sym.pop(rt,None)
        a+=4
    return reads,stores

order=sorted(set(TAB))
def nxt(addr):
    i=order.index(addr)
    return order[i+1] if i+1<len(order) else addr+0x200

if __name__=="__main__":
    print("op  handler      store(pc+=)   max_read  reads")
    for i,h in enumerate(TAB):
        reads,stores=analyse(h,min(nxt(h),h+0x400))
        sd=sorted(set(k for _,k in stores))
        mx=max([k+wd for _,k,wd in reads],default=0)
        print("%02x  0x%08x  %-12s  %-8s  %s"%(i,h,",".join(str(x) for x in sd) or "-",mx or "-",
              ",".join("%d(%d)"%(k,wd) for _,k,wd in reads[:12])))
