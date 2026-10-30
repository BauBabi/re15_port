import sys,struct,os,glob,json,re
TOOL=open('re15_port/tools/scd_dump_room.py',encoding='utf-8').read().replace('\nmain()\n','\n')
ns={}; exec(compile(TOOL,'scd','exec'),ns)
op_size=ns['op_size'];rdt_section_end=ns['rdt_section_end'];section_regions=ns['section_regions'];u16=ns['u16'];u32=ns['u32'];s16=ns['s16']
EM={0x40:'MARVIN',0x42:'ADA',0x45:'IRONS',0x47:'ROY(0x47)',0x49:'JOHN',0x4B:'SHERRY',0x4D:'ANNETTE'}
base=sys.argv[1]
print('Datei  Slot Figur        x      z   Param Bewegt? Skript')
for s in range(1,7):
    for p in sorted(glob.glob(f'{base}/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); n=os.path.basename(p)[4:8]
        ms,ss=u32(d,0x40),u32(d,0x44)
        ems=[];works=set();neck=[];dest=[]
        for sec,tag in ((ms,'main'),(ss,'sub')):
            if sec==0 or sec>=len(d): continue
            se=rdt_section_end(d,sec)
            for (o,e,idx) in section_regions(d,sec,se):
                pc=sec+o; end=sec+e; lbl=f'{tag}{idx:02d}'; cur=None
                while pc<end:
                    op=d[pc]; sz=op_size(d,pc)
                    if sz is None or pc+sz>end: break
                    if op==0x44 and 0x40<=d[pc+2]<=0x4F:
                        ems.append((d[pc+1],d[pc+2],s16(d,pc+8),s16(d,pc+12),d[pc+4],lbl))
                    if op==0x2E:
                        cur=(d[pc+1],d[pc+2])
                        if d[pc+1]==2: works.add(d[pc+2])
                    pc+=sz
        for sl,t,x,z,pa,lbl in ems:
            park='PARK' if x==-30000 and z==-30000 else ''
            print(f'{n}  {sl:3d}  {EM.get(t,hex(t)):11s} {x:7d} {z:7d} {pa:3d}  {"ja" if sl in works else "nein":4s} {lbl} {park}')
