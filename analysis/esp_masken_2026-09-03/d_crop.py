import struct, zlib, sys
def readpng(p):
    raw=open(p,'rb').read(); i=8; w=h=None; idat=b''
    while i<len(raw):
        ln=struct.unpack_from(">I",raw,i)[0]; typ=raw[i+4:i+8]; data=raw[i+8:i+8+ln]; i+=12+ln
        if typ==b'IHDR': w,h=struct.unpack_from(">II",data,0)
        elif typ==b'IDAT': idat+=data
    dec=zlib.decompress(idat); px=bytearray(w*h*3); stride=w*3; prev=bytearray(stride); pos=0
    for y in range(h):
        ft=dec[pos]; pos+=1; line=bytearray(dec[pos:pos+stride]); pos+=stride
        if ft==1:
            for x in range(3,stride): line[x]=(line[x]+line[x-3])&0xff
        elif ft==2:
            for x in range(stride): line[x]=(line[x]+prev[x])&0xff
        elif ft==3:
            for x in range(stride):
                a=line[x-3] if x>=3 else 0; line[x]=(line[x]+((a+prev[x])>>1))&0xff
        elif ft==4:
            for x in range(stride):
                a=line[x-3] if x>=3 else 0; b=prev[x]; c=prev[x-3] if x>=3 else 0
                p_=a+b-c; pa=abs(p_-a); pb=abs(p_-b); pc=abs(p_-c)
                pr=a if (pa<=pb and pa<=pc) else (b if pb<=pc else c)
                line[x]=(line[x]+pr)&0xff
        px[y*stride:(y+1)*stride]=line; prev=line
    return w,h,px
def writepng(p,w,h,px):
    raw=b''.join(b'\x00'+bytes(px[y*w*3:(y+1)*w*3]) for y in range(h))
    def ch(t,dd):
        c=t+dd; return struct.pack(">I",len(dd))+c+struct.pack(">I",zlib.crc32(c)&0xffffffff)
    open(p,'wb').write(b'\x89PNG\r\n\x1a\n'+ch(b'IHDR',struct.pack(">IIBBBBB",w,h,8,2,0,0,0))+ch(b'IDAT',zlib.compress(raw,9))+ch(b'IEND',b''))
src,dst,x0,y0,x1,y1,scale = sys.argv[1],sys.argv[2],int(sys.argv[3]),int(sys.argv[4]),int(sys.argv[5]),int(sys.argv[6]),int(sys.argv[7])
w,h,px=readpng(src)
cw=x1-x0; chh=y1-y0
ow=cw*scale; oh=chh*scale
out=bytearray(ow*oh*3)
for y in range(oh):
    for x in range(ow):
        sx=x0+x//scale; sy=y0+y//scale
        if 0<=sx<w and 0<=sy<h:
            o=(sy*w+sx)*3; q=(y*ow+x)*3
            out[q]=px[o]; out[q+1]=px[o+1]; out[q+2]=px[o+2]
writepng(dst,ow,oh,out)
print("wrote",dst,ow,oh)
