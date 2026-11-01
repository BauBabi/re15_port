import struct,collections
def imms(path,lo,hi,base=0x80100000):
    d=open(path,'rb').read()
    c=collections.Counter()
    hi=min(hi,base+len(d))
    for a in range(lo,hi,4):
        x=struct.unpack_from('<I',d,a-base)[0]; op=x>>26
        if op in (8,9,0xa,0xb,0xc,0xd):
            imm=x&0xffff; s=imm-0x10000 if (imm&0x8000 and op in (8,9,0xa,0xb)) else imm
            if abs(s)>=16: c[s]+=1
    return c
g=imms('Version_40%/PSX/BIN/STAGE2.BIN',0x8010c448,0x8010ee9c)
r=imms('info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN',0x80100000,0x80200000)
z=imms('Version_40%/PSX/BIN/STAGE1.BIN',0x8010d7f8,0x80112020)  # Hund als Nullmodell
skip={-14460,-13760,-13735,-13736,0x800b,0x8012,0x8011,0x8010,0x800a,0x800c,0x800d,-32768}
gs={k for k in g if k not in skip}; rs={k for k in r if k not in skip}; zs={k for k in z if k not in skip}
print('Gator40 Konstanten',len(gs),'RE2-EM23',len(rs),'Hund40',len(zs))
print('gemeinsam Gator40&RE2:',len(gs&rs),sorted(gs&rs))
print('gemeinsam Gator40&Hund40:',len(gs&zs))
print('nur Gator40 (nicht RE2):',sorted(gs-rs))
print('Jaccard G/RE2 %.2f  G/Hund %.2f'%(len(gs&rs)/len(gs|rs),len(gs&zs)/len(gs|zs)))
