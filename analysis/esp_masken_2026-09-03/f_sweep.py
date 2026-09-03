import struct, os, sys, glob, json
ROOT = 're15_port/shared_assets/PSX'
sys.path.insert(0, 'analysis/esp_masken_2026-09-03')
from sld import sld_decompress

def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def s16(b,o): return struct.unpack_from('<h',b,o)[0]
def u32(b,o): return struct.unpack_from('<I',b,o)[0]

def parse_pri(d, off):
    """returns dict(groups=[(n,base,dx,dy)], masks=[...]) or None"""
    if off+4 > len(d): return None
    gc = u16(d,off); mc = u16(d,off+2)
    if gc==0xFFFF and mc==0xFFFF: return None
    if gc==0 or mc==0: return None
    if gc>256 or mc>256: return None
    groups=[]; p=off+4
    for i in range(gc):
        if p+8>len(d): return None
        groups.append((u16(d,p), u16(d,p+2), s16(d,p+4), s16(d,p+6)))
        p+=8
    masks=[]
    total=sum(g[0] for g in groups)
    gi=0; used=0
    for i in range(total):
        if p+8>len(d): break
        srcX,srcY,dxl,dyl = d[p],d[p+1],d[p+2],d[p+3]
        depth = u16(d,p+4); size_field = u16(d,p+6); size_b = d[p+7]
        p+=8
        if (size_b & 0xf0)==0:
            if p+4>len(d): break
            w=u16(d,p); h=u16(d,p+2); p+=4; rect=True
        else:
            w=h=(size_b>>4)*8; rect=False
        while gi<gc and used>=groups[gi][0]:
            gi+=1; used=0
        ax,ay = (groups[gi][2],groups[gi][3]) if gi<gc else (0,0)
        used+=1
        masks.append(dict(srcX=srcX,srcY=srcY,dstX=(dxl+ax),dstY=(dyl+ay),
                          depth=depth,w=w,h=h,rect=rect,g=gi))
    return dict(groups=groups, decl=mc, masks=masks)

def bss_trailers(c):
    out=[]
    for p in range(8,len(c)-8,4):
        w0,w1 = struct.unpack_from('<2I',c,p)
        if w1==0 or w1>0xff: continue
        if not (8<=w0<p): continue
        out.append((p,w0,w1))
    return out

def sld_at(c, off):
    size = u32(c,off)
    if not (0x100 <= size <= 0x40000): return None
    try:
        out,end = sld_decompress(c, off+4, size)
    except Exception as e:
        return ('ERR', str(e))
    return (size, out, end)

def scan_room(rdtpath, bsspath):
    d=open(rdtpath,'rb').read()
    nCut=d[1]; hdr7=d[7]
    cam=u32(d,0x24)
    cuts=[]
    for i in range(nCut):
        base=cam+i*32
        if base+32>len(d): break
        pri=u32(d,base+0x1c)
        cuts.append(dict(i=i, pri_off=pri, pri=(None if pri==0xFFFFFFFF else parse_pri(d,pri))))
    bss=None
    if bsspath and os.path.exists(bsspath):
        b=open(bsspath,'rb').read()
        bss=[]
        for ci in range(len(b)//0x10000):
            c=b[ci*0x10000:(ci+1)*0x10000]
            t=bss_trailers(c)
            e=dict(chunk=ci, trailers=t)
            if t:
                p,w0,w1=t[0]
                r=sld_at(c,w0)
                if r and r[0]!='ERR':
                    e['sld']=(w0,r[0]); e['tim']=r[1][:32]
                    e['sld_end']=r[2]
                else: e['sld_err']=r
            bss.append(e)
    return dict(nCut=nCut, hdr7=hdr7, cuts=cuts, bss=bss, bsspath=bsspath)

if __name__=='__main__':
    pass
