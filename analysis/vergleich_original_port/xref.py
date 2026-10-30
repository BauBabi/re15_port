#!/usr/bin/env python3
"""xref.py STAGEn ADDR... : wer ruft (jal) bzw. welche Tabelle enthaelt die Adresse."""
import struct, sys, os
BASE=0x80100000
d=open(os.path.join(os.path.dirname(__file__),'../../info/Re1.5/PSX/BIN',sys.argv[1]+'.BIN'),'rb').read()
w=struct.unpack_from('<%dI'%(len(d)//4),d)
for a in sys.argv[2:]:
    A=int(a,16); out=[]
    for i,x in enumerate(w):
        if x>>26==3 and ((x&0x3ffffff)<<2|0x80000000)==A: out.append('jal@%08x'%(BASE+i*4))
        elif x==A: out.append('tab@%08x'%(BASE+i*4))
    print(a, ' '.join(out) or '-')
# Adress-Bildung lui/addiu (jalr ueber Register)
for a in sys.argv[2:]:
    A=int(a,16); hi=(A+0x8000)>>16; lo=A&0xffff; out=[]
    for i,x in enumerate(w):
        if x>>26==0x0f and (x&0xffff)==hi:
            r=(x>>16)&31
            for j in range(i+1,min(i+8,len(w))):
                y=w[j]
                if y>>26 in (0x09,0x0d) and (y>>21)&31==r and (y&0xffff)==lo: out.append('lui/addiu@%08x'%(BASE+i*4)); break
    if out: print(a,'adressiert:',' '.join(out))
