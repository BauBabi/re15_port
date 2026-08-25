#!/usr/bin/env python3
"""RE2-SCD-Walker (reparierte Fassung, 2026-08-27).

Laengen kommen aus re2_scd_lens.py (aus den 143 Handlern der Dispatch-Tabelle
@0x800a74c8 abgeleitet; RE2 hat KEINE Laengen-Datentabelle, FUN_800536c4 laedt
task->pc nach jedem Handler neu).

RDT-Offsets (23 Worte ab Datei 0x08): [16] = SCD_INIT, [17] = SCD_MAIN.
Verifiziert an ROOM1120: 0x2928 + subs[25]=0x10b4 = 0x39DC.

Sub-Grenzen: subs[i+1] - subs[i]; der LETZTE Sub eines Blocks endet an der
naechstgroesseren Sektionsadresse minus 0..3 Byte 4er-Alignment-Padding
(Padding-Bytes sind NICHT genullt, siehe ROOM1020 INIT @0x18C2 = "ed 47").
"""
import struct, os, sys, glob
HERE=os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0,HERE)
from re2_scd_lens import TABLE as LENS
REPO=os.path.abspath(os.path.join(HERE,"..","..",".."))
RDTDIR=os.path.join(REPO,"info","re2leon","PL0","RDT")

def scd_blocks(d):
    offs=struct.unpack_from("<23I",d,8); out=[]
    for idx,name in ((16,"INIT"),(17,"MAIN")):
        base=offs[idx]
        if base==0 or base>=len(d): continue
        n=struct.unpack_from("<H",d,base)[0]
        if n==0 or n%2 or n>0x800: continue
        out.append((name,base,list(struct.unpack_from("<%dH"%(n//2),d,base))))
    return out

def block_end(d,base,subs,i,offs):
    if i+1<len(subs): return base+subs[i+1]
    cand=[o for o in offs if base<o<=len(d)]
    return min(cand,default=len(d))

def walk(d,s,e,last=False,L=LENS):
    """-> (ops, status). ops = [(addr,opcode,len)]

    Der LETZTE Sub eines Blocks hat keine im RDT gespeicherte Endadresse: hinter
    ihm folgt entweder 0..3 Byte Alignment-Padding oder direkt eine Sektion, die
    in der 23er-Offsettabelle GAR NICHT steht (z.B. ROOMA010 @0x1804). Regel:
    Ende = hinter dem letzten evt_end (0x01), das vor dem ersten ungueltigen
    Opcode bzw. vor `e` erreicht wurde."""
    ops=[]; pc=s; prev=None; last_end=None; last_n=0
    while pc<e:
        if last and prev==0x01 and (e-pc<=3 or d[pc]>0x8E or L[d[pc]]==0):
            return ops,"ok"
        op=d[pc]
        if op>0x8E or L[op]==0:
            if last and last_end is not None: return ops[:last_n],"ok(trunc@0x%X)"%last_end
            return ops,"unk:%02X@+%d"%(op,pc-s)
        if pc+L[op]>e:
            if last and prev==0x01: return ops,"ok"
            if last and last_end is not None: return ops[:last_n],"ok(trunc@0x%X)"%last_end
            return ops,"over:%d"%(pc+L[op]-e)
        ops.append((pc,op,L[op])); prev=op; pc+=L[op]
        if op==0x01: last_end=pc; last_n=len(ops)
    return ops,("ok" if pc==e or (last and prev==0x01) else "short")

def all_subs():
    for p in sorted(glob.glob(os.path.join(RDTDIR,"ROOM*.RDT"))):
        d=open(p,"rb").read(); offs=struct.unpack_from("<23I",d,8)
        for name,base,subs in scd_blocks(d):
            for i in range(len(subs)):
                s=base+subs[i]; e=block_end(d,base,subs,i,offs)
                if e<=s or e>len(d): continue
                yield os.path.basename(p),d,name,base,i,s,e,(i+1==len(subs))

if __name__=="__main__":
    tot=nlast=bad=badlast=trunc=0; ex=[]
    for fn,d,name,base,i,s,e,last in all_subs():
        tot+=1; nlast+=(0 if last else 1)
        ops,st=walk(d,s,e,last)
        if st.startswith("ok(trunc"): trunc+=1
        if not st.startswith("ok"):
            bad+=1; badlast+=(1 if last else 0)
            if len(ex)<25: ex.append("  %s %s sub%-3d 0x%05X..0x%05X %s%s"%(fn,name,i,s,e,st," [LETZTER]" if last else ""))
    print("Subs gesamt %d (nicht-letzte %d) - echte Desyncs %d (davon letzte Subs %d); letzte Subs mit hergeleitetem Ende: %d"%(tot,nlast,bad,badlast,trunc))
    for x in ex: print(x)
