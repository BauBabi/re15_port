# Sucht Schleifen ueber Teil-Records (Schritt -172/+172) in allen Disc-Overlays + EXE; zeigt Startwerte (Index/Offset) im Umfeld.
import struct,sys,importlib.util
R=r'C:/workspace/git/reAi_v2'
spec=importlib.util.spec_from_file_location('d',R+'/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py'); m=importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
def scan(path,base,skip=0):
    d=open(path,'rb').read()[skip:]
    n=len(d)//4
    w=struct.unpack_from('<%dI'%n,d)
    for i,x in enumerate(w):
        op=x>>26; rs=(x>>21)&31; rt=(x>>16)&31; imm=x&0xffff; s=imm-0x10000 if imm&0x8000 else imm
        if op==9 and s in (172,-172) and rs==rt:
            a=base+i*4
            ctx=[]
            for j in range(max(0,i-14),i+1):
                t,_=m.dis_one(w[j],base+j*4)
                if t.startswith(('ori','addiu')) and ('zero' in t):
                    ctx.append(f'{base+j*4:08x}:{t}')
            print(f'{path[-12:]} {a:08x}: {m.dis_one(x,a)[0]} | {ctx}')
for s in (1,2,3,4,5,6):
    scan(R+f'/Version_40%/PSX/BIN/STAGE{s}.BIN',0x80100000)
scan(R+'/Version_40%/PSX.EXE',0x80010000,0x800)
