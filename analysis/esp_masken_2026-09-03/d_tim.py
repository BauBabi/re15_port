import struct, zlib, sys
P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
d=open(P,'rb').read()
def dump_tim(off, name):
    magic, flags = struct.unpack_from("<II", d, off)
    bpp = flags & 3; hasclut = (flags>>3)&1
    print("%s @0x%X: magic=0x%X flags=0x%X bpp_code=%d clut=%d"%(name,off,magic,flags,bpp,hasclut))
    p = off+8
    cl=None
    if hasclut:
        clen, cx, cy, cw, chh = struct.unpack_from("<IHHHH", d, p)
        print("   CLUT: len=%d org=(%d,%d) size=%dx%d"%(clen,cx,cy,cw,chh))
        cl = d[p+12:p+clen]
        p += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, p)
    print("   IMG : len=%d org=(%d,%d) size=%dx%d (words)"%(ilen,ix,iy,iw,ih))
    img = d[p+12:p+ilen]
    if bpp==1:   # 8bpp
        W = iw*2; H = ih
    elif bpp==0: # 4bpp
        W = iw*4; H = ih
    else:
        W = iw; H = ih
    print("   -> pixel dims %dx%d"%(W,H))
    # build RGB with CLUT palette 0
    out = bytearray(W*H*3)
    def c15(v):
        r=(v&31)<<3; g=((v>>5)&31)<<3; b=((v>>10)&31)<<3
        return r,g,b
    pal=[]
    if cl:
        for i in range(min(256, len(cl)//2)):
            pal.append(c15(struct.unpack_from("<H", cl, i*2)[0]))
    for y in range(H):
        for x in range(W):
            if bpp==1:
                idx = img[y*W+x] if y*W+x < len(img) else 0
            else:
                bi = y*(W//2) + x//2
                b = img[bi] if bi<len(img) else 0
                idx = (b & 0xf) if (x&1)==0 else (b>>4)
            r,g,b_ = pal[idx] if idx < len(pal) else (idx,idx,idx)
            o=(y*W+x)*3; out[o]=r; out[o+1]=g; out[o+2]=b_
    raw=b''.join(b'\x00'+bytes(out[y*W*3:(y+1)*W*3]) for y in range(H))
    def ch(t,dd):
        c=t+dd; return struct.pack(">I",len(dd))+c+struct.pack(">I",zlib.crc32(c)&0xffffffff)
    open(name,'wb').write(b'\x89PNG\r\n\x1a\n'+ch(b'IHDR',struct.pack(">IIBBBBB",W,H,8,2,0,0,0))+ch(b'IDAT',zlib.compress(raw,9))+ch(b'IEND',b''))
    print("   wrote", name)
dump_tim(0x1DAC8, r"C:\workspace\git\reAi_v2\analysis\esp_masken_2026-09-03\room1140\prop0_model_tim.png")
dump_tim(0x1A628, r"C:\workspace\git\reAi_v2\analysis\esp_masken_2026-09-03\room1140\esp_tim.png")
