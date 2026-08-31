import sys, struct, zlib, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/skills/re15-room-capture/scripts')
from re15_ss import decompress_sav
def vram_of(sav):
    blob = decompress_sav(sav)
    pos = blob.find(b"GPU-VRAM")
    if pos < 0: raise RuntimeError("kein GPU-VRAM-Tag")
    return blob[pos+8: pos+8 + 1024*512*2]
def png(path, w, h, rows):
    raw=b''.join(b'\x00'+bytes(v for px in r for v in px) for r in rows)
    def ch(t,d): return struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d)&0xffffffff)
    open(path,'wb').write(b'\x89PNG\r\n\x1a\n'+ch(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+ch(b'IDAT',zlib.compress(raw))+ch(b'IEND',b''))
def dump(sav, out, x0=0, y0=0, w=1024, h=512):
    v = vram_of(sav)
    rows=[]
    for y in range(y0, y0+h):
        row=[]
        for x in range(x0, x0+w):
            p=struct.unpack_from('<H', v, (y*1024+x)*2)[0]
            row.append(((p&31)<<3, ((p>>5)&31)<<3, ((p>>10)&31)<<3))
        rows.append(row)
    png(out, w, h, rows)
    return out
if __name__ == "__main__":
    a=sys.argv
    if len(a)>=6: dump(a[1],a[2],int(a[3]),int(a[4]),int(a[5]),int(a[6]))
    else: dump(a[1],a[2])
    print("->", a[2])
