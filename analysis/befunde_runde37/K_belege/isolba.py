import struct
BIN=r'C:/Users/mjoedicke/Downloads/ePSXe2018/Biohazard 1.5 (MZD Mod) Update 25-01-2025.bin'
EXE=r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/info/Re1.5/PSX.EXE'
f=open(BIN,'rb')
def sec(lba):  # raw 2352 mode2 form1: data at +24
    f.seek(lba*2352+24); return f.read(2048)
pvd=sec(16); assert pvd[1:6]==b'CD001', pvd[:8]
root_lba=struct.unpack_from('<I',pvd,156+2)[0]; root_len=struct.unpack_from('<I',pvd,156+10)[0]
def listdir(lba,ln):
    out={}; n=(ln+2047)//2048
    for k in range(n):
        d=sec(lba+k); o=0
        while o<2048:
            rl=d[o]
            if rl==0: break
            e_lba=struct.unpack_from('<I',d,o+2)[0]; e_len=struct.unpack_from('<I',d,o+10)[0]
            nl=d[o+32]; name=d[o+33:o+33+nl].decode('latin1').split(';')[0]
            out[name]=(e_lba,e_len,d[o+25]&2)
            o+=rl
    return out
root=listdir(root_lba,root_len)
print('root', sorted(root)[:20])
psx=root.get('PSX') or root.get('psx')
base=root
if psx: base=listdir(psx[0],psx[1])
emd=base.get('EMD')
ed=listdir(emd[0],emd[1]); print('EMD dir', {k:v[:2] for k,v in ed.items() if k not in ('\x00','\x01')})
exe=open(EXE,'rb').read(); t=struct.unpack_from('<I',exe,0x18)[0]
def rd(a,n): o=0x800+a-t; return exe[o:o+n]
for fid in (0x26,0x27):
    e=rd(0x8006f440+fid*8,8)
    lo=struct.unpack_from('<H',e,0)[0]; hi=e[2]
    print('file %#x: table bytes %s -> LBA %d'%(fid,e.hex(),hi*0x10000+lo))
