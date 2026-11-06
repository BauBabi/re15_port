import sys, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import decompress_sav, find_vram_base
MOD=open(r"C:\workspace\git\reAi_v2\info\Re1.5\PSX.EXE","rb").read()
DISC=open(r"C:\workspace\git\reAi_v2\Version_40%\PSX.EXE","rb").read()
def ew(d,a): return struct.unpack_from("<I",d,0x800+a-0x80010000)[0]
class R:
    def __init__(self,path):
        self.blob=decompress_sav(path)
        # signature from common region 0x8001e9ec (drawer, identical in both)
        sig=MOD[0x800+0x8001e9ec-0x80010000:0x800+0x8001e9ec-0x80010000+64]
        pos=self.blob.find(sig)
        if pos<0: raise RuntimeError("no sig")
        self.base=pos-0x1e9ec
        w=self.u32(0x8001af28)
        self.exe="mod" if w==ew(MOD,0x8001af28) else ("disc" if w==ew(DISC,0x8001af28) else "?%08x"%w)
        # count matching words mod vs disc over text
        self.vram_base=find_vram_base(self.blob)
    def o(self,a): return self.base+(a-0x80000000)
    def u8(self,a): return self.blob[self.o(a)]
    def u16(self,a): return struct.unpack_from("<H",self.blob,self.o(a))[0]
    def s16(self,a): return struct.unpack_from("<h",self.blob,self.o(a))[0]
    def u32(self,a): return struct.unpack_from("<I",self.blob,self.o(a))[0]
    def s32(self,a): return struct.unpack_from("<i",self.blob,self.o(a))[0]
    def bytes(self,a,n): return self.blob[self.o(a):self.o(a)+n]
def exe_match(r):
    m=d=0
    for a in range(0x80010000,0x800bf000,4):
        w=r.u32(a)
        if w==ew(MOD,a) and w!=ew(DISC,a): m+=1
        if w==ew(DISC,a) and w!=ew(MOD,a): d+=1
    return m,d
if __name__=="__main__":
    import os
    for f in sys.argv[1:]:
        try:
            r=R(f); m,d=exe_match(r)
            print("%-45s exe=%s modwords=%d discwords=%d room=%04x stage=%d"%(os.path.basename(f),r.exe,m,d,r.u16(0x800b0fe2),r.u16(0x800b0fe6)))
        except Exception as e: print(os.path.basename(f),"ERR",e)
