#!/usr/bin/env python3
"""Findet je STAGE*.BIN die Schreibzugriffe auf die Entity-Dispatch-Tabelle @0x80072bac (Typ -> Handler).
Einfaches Register-Tracking (lui/addiu/ori) in linearer Reihenfolge; Ausgabe Typ -> Handler je Stage."""
import struct, sys
BASE=0x80100000; TAB=0x80072bac; N=0x60
def scan(path):
    d=open(path,'rb').read(); n=len(d)//4
    reg={}; out={}
    for i in range(n):
        w=struct.unpack_from('<I',d,i*4)[0]; op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31
        imm=w&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
        if op==0x0f: reg[rt]=imm<<16
        elif op==0x09 and rs in reg: reg[rt]=(reg[rs]+simm)&0xffffffff
        elif op==0x0d and rs in reg: reg[rt]=reg[rs]|imm
        elif op==0x2b and rs in reg:   # sw rt, simm(rs)
            a=(reg[rs]+simm)&0xffffffff
            if TAB<=a<TAB+N*4 and rt in reg and BASE<=reg[rt]<BASE+0x40000:
                out[(a-TAB)//4]=(reg[rt],BASE+i*4)
        elif op in (0x02,0x03) or (op==0 and (w&0x3f) in (8,9)): pass
        if op==0x23 and rt in reg: del reg[rt]   # lw ueberschreibt
    return out
if __name__=='__main__':
    for p in sys.argv[1:]:
        o=scan(p); print(p)
        for t in sorted(o): print('  Typ 0x%02x -> 0x%08x  (sw @0x%08x)'%(t,o[t][0],o[t][1]))
