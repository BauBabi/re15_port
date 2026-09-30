import struct, glob, os, sys, collections
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "re15_port", "shared_assets", "PSX")
def u16(b,o): return b[o] | (b[o+1]<<8)
def u32(b,o): return struct.unpack_from("<I", b, o)[0]
def s32(b,o): return struct.unpack_from("<i", b, o)[0]
def parse(raw, idh, pe, tb=0, te=0):
    effs=[]
    if idh+8>len(raw) or u32(raw,idh)==0xFFFFFFFF: return effs
    for i in range(96):
        ide = raw[idh+i]
        if ide==0xFF: break
        entry = s32(raw, pe - i*4)
        st = idh + entry
        w0 = u32(raw, st)
        ca, cb = w0 & 0xffff, w0>>16
        end = st + (ca*2+cb+2)*4
        effs.append(dict(id=ide, start=st, end=end))
    starts = sorted(e['start'] for e in effs)
    for e in effs:
        nxt = [s for s in starts if s > e['start']]
        bound = min(nxt) if nxt else None
        if bound is None:
            # last effect: bound by id header / ptr table / tim area
            cands=[x for x in (idh, pe-4*len(effs)+4, tb) if x and x>e['end']]
            bound = min(cands) if cands else len(raw)
        e['bound']=bound
    return effs
def rows_of(raw, e):
    blk = e['end']; L = e['bound']-blk
    out=[]
    seen=set()
    for sub in range(8):
        so = u16(raw, blk+sub*2); base = so*4
        if base < 16 or base+4 > L: continue
        if base in seen: 
            pass
        seen.add(base)
        streams = u16(raw, blk+base)
        if streams==0 or streams>16: continue
        p = base+4
        ok=True; rows=[]
        for s in range(streams):
            if p+4>L: ok=False; break
            nr = u16(raw, blk+p)
            if nr==0 or nr>64 or p+4+nr*40>L: ok=False; break
            for r in range(nr):
                o = blk+p+4+r*40
                rows.append((sub,s,r,raw[o:o+40]))
            p += 4+nr*40
        if ok: out.extend(rows)
    return out
IMPL = {0,3,4,5,8,9,10,11,15,16,17,18,38}
stats=collections.Counter(); wh=collections.Counter(); zero=[]; unimpl=collections.Counter(); unimpl_wh=collections.Counter()
def scan(name, raw, effs):
    for e in effs:
        for sub,s,r,row in rows_of(raw,e):
            A=u16(row,0); B=u16(row,2); w=u16(row,4); h=u16(row,6)
            if A>60: continue   # not a routine selector (garbage) — table has 48 entries
            stats['rows']+=1
            wh[(A,w,h)]+=1
            if A not in IMPL: unimpl[A]+=1; unimpl_wh[(A,w,h)]+=1
            if (w==0 or h==0) and A in IMPL: zero.append((name,e['id'],sub,s,r,A,B,w,h,row[0x0e]))
core=open(os.path.join(ROOT,"DATA/CORE00.ESP"),"rb").read()
scan("CORE00", core, parse(core,0,((len(core)+3)&~3)-4))
n=0
for f in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    raw=open(f,"rb").read()
    if len(raw)<0x5c: continue
    idh,pe,tb,te=u32(raw,0x4c),u32(raw,0x50),u32(raw,0x54),u32(raw,0x58)
    if idh==0: continue
    try: effs=parse(raw,idh,pe,tb,te)
    except Exception: continue
    n+=1; scan(os.path.basename(f), raw, effs)
print("RDTs",n,"rows",stats['rows'])
print("A<=60 (w,h) histogram (top):")
for k,v in sorted(wh.items(), key=lambda kv:-kv[1])[:60]: print("   A=%2d w=%04x h=%04x x%d"%(k[0],k[1],k[2],v))
print("unimplemented A:",sorted(unimpl.items()))
print("implemented-A rows with w==0 or h==0:",len(zero))
for z in zero[:60]: print("  ",z)
