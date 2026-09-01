# -*- coding: utf-8 -*-
# Unabhaengiger RDT-Scanner: alle Aot_set-Records von ROOM1170, roh.
import os, struct, sys
ROOT = r'C:\workspace\git\reAi_v2'
os.chdir(ROOT)
sys.path.insert(0, 'analysis/karte_audit_0901')
from sonde_lib import *          # zieht SZ / NAMES aus dem Generator
SZ = G['SZ']; NAMES = G['NAMES']

p = 're15_port/shared_assets/PSX/STAGE1/ROOM1170.RDT'
d = open(p,'rb').read()
print("Datei", p, len(d), "Bytes")
offs = {nm: struct.unpack_from('<I', d, 0x20+4*i)[0] for i,nm in enumerate(NAMES)}
for k,v in offs.items():
    if v: print(f"  {k:12s} 0x{v:06X}")
print()

def walk(name):
    s = offs[name]
    ends = sorted(v for v in offs.values() if v > s) + [len(d)]
    e = ends[0]
    n0 = struct.unpack_from('<H', d, s)[0]
    nsub = n0//2
    ptrs = [struct.unpack_from('<H', d, s+2*i)[0] for i in range(nsub)]
    out=[]
    for si,pp in enumerate(ptrs):
        end = ptrs[si+1] if si+1<nsub else (e-s)
        end = min(end, e-s)
        pc = pp
        while pc < end:
            op = d[s+pc]
            sz = SZ.get(op)
            if sz is None:
                out.append(('DESYNC', name, si, s+pc, op)); break
            out.append((op, name, si, s+pc, d[s+pc:s+pc+sz]))
            pc += sz
    return out, nsub

allrec=[]
for nm in ('mainScd','subScd'):
    recs, nsub = walk(nm)
    print(f"{nm}: {nsub} Unterprogramme")
    allrec += recs

print()
print("=== 0x3b / 0x68  TUEREN  (roh) ===")
slot=0
DOORS=[]
for r in allrec:
    if r[0] not in (0x3b,0x68): continue
    op, nm, si, off, b = r
    if op==0x3b:
        rx,rz,rw,rd = struct.unpack_from('<hhhh', b, 6)
        nx,ny,nz = struct.unpack_from('<hhh', b, 14)
        stg,rmd = b[22],b[23]
    else:
        rx,rz,rw,rd = struct.unpack_from('<hhhh', b, 4)
        nx,ny,nz = struct.unpack_from('<hhh', b, 22)
        stg,rmd = b[30],b[31]
    dest = ((stg&0xf)<<12) | ((rmd//10)<<8) | ((rmd%10)<<4)
    rec = dict(op=op,off=off,slot=b[1],sce=b[2],band=b[4],rx=rx,rz=rz,rw=rw,rd=rd,
               lx=rx+rw//2, lz=rz+rd//2, nx=nx,ny=ny,nz=nz,stg=stg,rmd=rmd,dest=dest,
               raw=b.hex(), sub=si, scd=nm)
    DOORS.append(rec)
    print(f"  @0x{off:06X} {nm} sub{si:02d} op0x{op:02x} slot={b[1]:2d} sce={b[2]:2d} pc4(BAND)={b[4]:2d} "
          f"rect=({rx},{rz},{rw},{rd}) mitte=({rec['lx']},{rec['lz']}) ziel=({nx},{ny},{nz}) "
          f"stg=0x{stg:02x} rm={rmd} -> ROOM{dest:04X}")
    print(f"        raw {b.hex()}")

print()
print("=== 0x2c type 12/13  TREPPEN (roh) ===")
STAIRS=[]
for r in allrec:
    if r[0]!=0x2c: continue
    op,nm,si,off,b = r
    if b[2] not in (12,13): continue
    rx,rz,rw,rd = struct.unpack_from('<hhhh', b, 6)
    rec=dict(off=off,slot=b[1],axis=b[2],band=b[4],count=b[16],
             x=rx+rw//2, z=rz+rd//2, rx=rx,rz=rz,rw=rw,rd=rd,raw=b.hex(),sub=si,scd=nm)
    STAIRS.append(rec)
    print(f"  @0x{off:06X} {nm} sub{si:02d} slot={b[1]:2d} sce={b[2]:2d}(Achse {'X' if b[2]==12 else 'Z'}) "
          f"pc4(BAND)={b[4]:2d} count(pc16)={b[16]} rect=({rx},{rz},{rw},{rd}) mitte=({rec['x']},{rec['z']})")
    print(f"        raw {b.hex()}")

print()
print("=== 0x46 Aot_reset ===")
for r in allrec:
    if r[0]!=0x46: continue
    op,nm,si,off,b=r
    print(f"  @0x{off:06X} {nm} sub{si:02d} slot={b[1]} sce={b[2]} raw {b.hex()}")

import pickle
pickle.dump({'doors':DOORS,'stairs':STAIRS}, open('analysis/karte_0901/_a1.pkl','wb'))
