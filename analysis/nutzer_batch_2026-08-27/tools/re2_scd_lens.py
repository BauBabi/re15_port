#!/usr/bin/env python3
"""Erzeugt die RE2-SCD-Laengentabelle aus re2_scd_oplen.py (Handler-Stores)
und traegt die vier per Daten bestimmten Kontrollfluss-Laengen nach."""
import sys, os
HERE=os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0,HERE)
from re2_scd_oplen import TAB, analyse, nxt, NOPS

def derive():
    lens={}; info={}
    for i,h in enumerate(TAB):
        reads,stores=analyse(h,min(nxt(h),h+0x800))
        sd=sorted(set(k for _,k in stores))
        mx=max([k+w for _,k,w in reads],default=0)
        info[i]=(h,sd,mx)
        lens[i]=sd[0] if len(sd)==1 else None
    return lens,info

LENS,INFO=derive()
# Handler mit langem Rumpf -> groesseres Fenster noetig (0x44, 0x8E)
MANUAL_STORE={0x44:22, 0x8e:24}     # @0x800576e4 addiu v1,v1,22 / @0x80057d78 addiu v1,v1,24
for k,v in MANUAL_STORE.items(): LENS[k]=v

# Kontrollfluss: Handler setzt den PC absolut -> Laenge aus Operand-Reads/Semantik
CTRL={
 0x03:4,   # evt_chain : lbu a1,3(v0) @0x80053888 -> Operand bei +3 => >=4
 0x07:4,   # else_ck   : lhu v0,2(v1) @0x80053974 -> s16 @+2      => 4
 0x13:4,   # switch    : addiu a3,a3,4 @0x80054040 (Header) vor dem Case-Scan
 0x17:6,   # goto      : lh t0,4(a1)  @0x80054178 -> s16 @+4      => 6
 0x18:2,   # gosub     : addiu v1,v1,2 @0x800541b4; sw v1,324(v0) = Ruecksprung-PC = pc+2
}
LENS.update(CTRL)
# empirisch aus den RDT-Daten bestimmt (siehe Dossier §3)
EMPIRICAL={0x01:2, 0x10:2, 0x19:2, 0x1a:2}
LENS.update(EMPIRICAL)

TABLE=[LENS.get(i) or 0 for i in range(256)]

if __name__=="__main__":
    for r in range(0,0x90,16):
        print("/*0x%02X*/ "%r + ",".join("%d"%TABLE[r+c] for c in range(16)))
