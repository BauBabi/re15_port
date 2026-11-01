import struct,sys,io,os
sys.stdout=io.TextIOWrapper(sys.stdout.buffer,encoding='utf-8',errors='replace')
R=r'C:/workspace/git/reAi_v2'
def tab(p):
    d=open(p,'rb').read()
    base=0x60734  # 0x8006ff34
    out={}
    for st in range(7):
        for r in range(49):
            o=base+st*1274+r*26
            en,x,z,c=struct.unpack_from('<HhhH',d,o)
            nm=d[o+8:o+22]
            out[(st,r)]=(en,x,z,c,nm,d[o+22:o+26].hex())
    return out
D=tab(R+'/Version_40%/PSX.EXE'); M=tab(R+'/info/Re1.5/PSX.EXE')
print('Bytes gleich Disc/Mod:',D==M)
for st in range(1,2):
    for r in range(49):
        e=D[(st,r)]
        if e[0] or e[4].strip(b' \0'):
            bss=os.path.exists(R+f'/Version_40%/PSX/STAGE{st+1}/ROOM{st+1}{r:02X}.BSS')
            rd=[x for x in '01' if os.path.exists(R+f'/Version_40%/PSX/STAGE{st+1}/ROOM{st+1}{r:02X}{x}.RDT')]
            print(f'ROOM{st+1}{r:02X} en={e[0]} x={e[1]} z={e[2]} p6={e[3]} name={e[4].decode("ascii","replace")!r} rest={e[5]} BSS={bss} RDT={rd}  Mod-gleich={D[(st,r)]==M[(st,r)]}')
print('--- andere Stages, Stichprobe Namen mit WANI/KUMO/ZONBIE:')
for k,e in D.items():
    if any(t in e[4] for t in (b'WANI',b'KUMO',b'ZONBIE',b'BOSS')): print(k,e)
print('Eintraege je Stage mit Namen',{st:sum(1 for r in range(49) if D[(st,r)][4].strip(b' \0')) for st in range(7)})
