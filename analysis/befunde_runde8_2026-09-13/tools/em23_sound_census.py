#!/usr/bin/env python3
"""Vollzensus ALLER jal-Ziele im EM23-Overlay; Sound-Calls mit a0-Rekonstruktion."""
import struct, os, sys
REPO="c:/workspace/git/reAi_v2"
OVL=REPO+"/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN"
BASE=0x80100000
D=open(OVL,"rb").read()
N=len(D)//4
W=[struct.unpack_from("<I",D,i*4)[0] for i in range(N)]

def a0_before(i, depth=24):
    """Rekonstruiere a0 (reg 4) rueckwaerts: li/ori/lui+ori/addu a0,zero. Delay-Slot i+1 zuerst."""
    order=[i+1]+list(range(i-1, max(-1,i-1-depth), -1))
    lui=None
    for j in order:
        if j<0 or j>=N: continue
        w=W[j]; op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; fn=w&63; imm=w&0xffff
        if op==9 and rt==4 and rs==0: return ("li",imm)
        if op==13 and rt==4 and rs==0: return ("ori",imm)
        if op==13 and rt==4 and rs==4:
            # ori a0,a0,imm -> braucht vorheriges lui a0
            for k in range(j-1, max(-1,j-8), -1):
                w2=W[k]
                if (w2>>26)==15 and ((w2>>16)&31)==4: return ("lui+ori",((w2&0xffff)<<16)|imm)
            return ("ori a0,a0",imm)
        if op==15 and rt==4: lui=imm
        if op==0 and fn==0x21 and rd==4:
            return ("addu a0,%d,%d"%(rs,rt), None)
        if op==0x24 and rt==4: return ("lbu",None)
        if op==0x20 and rt==4: return ("lb",None)
        if op==0x23 and rt==4: return ("lw",None)
    return ("?",None)

def a1_before(i, depth=20):
    order=[i+1]+list(range(i-1, max(-1,i-1-depth), -1))
    for j in order:
        if j<0 or j>=N: continue
        w=W[j]; op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; fn=w&63; imm=w&0xffff
        s=imm-0x10000 if imm&0x8000 else imm
        if op==9 and rt==5: return "addiu a1,r%d,%d"%(rs,s)
        if op==0 and fn==0x21 and rd==5: return "addu a1,r%d,r%d"%(rs,rt)
        if op==13 and rt==5: return "ori a1,r%d,0x%X"%(rs,imm)
    return "?"

tgts={}
for i,w in enumerate(W):
    if (w>>26)==3:
        t=((BASE+i*4+4)&0xf0000000)|((w&0x3ffffff)<<2)
        tgts.setdefault(t,[]).append(BASE+i*4)
print("=== ALLE jal-Ziele (Adresse: Anzahl) ===")
for t in sorted(tgts):
    print("  0x%08X : %2d  %s"%(t,len(tgts[t]), "EXE" if t<0x80100000 else "OVL"))

SND={0x8005bd6c:"ENEMSE(id,ent)",0x8005ba28:"Se_on(packed,pos)",0x8005b9f4:"SE_stop/mod",
     0x8005b6f0:"BGM?",0x8005b2e4:"bgm_fade",0x8005c92c:"voice_busy"}
print("\n=== SOUND-CALLS ===")
for t in sorted(SND):
    if t not in tgts: continue
    for a in tgts[t]:
        i=(a-BASE)//4
        k,v=a0_before(i)
        av=("0x%08X"%v) if v is not None else "-"
        print("  %-22s @0x%08X   a0=%s (%s)  a1: %s"%(SND[t],a,av,k,a1_before(i)))
