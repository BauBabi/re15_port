import subprocess,glob,os,re,sys,collections,struct
R='C:/workspace/git/reAi_v2'
for tag,base in (('DISC',R+'/Version_40%/PSX'),('MOD',R+'/info/Re1.5/PSX')):
    print('==',tag)
    for f in sorted(glob.glob(base+'/STAGE*/ROOM*.RDT')):
        out=subprocess.run([sys.executable,R+'/re15_port/tools/scd_dump_room.py',f],capture_output=True,text=True).stdout
        for l in out.splitlines():
            if 'Door_aot_set' in l:
                off=l.split()[0]
                raw=l.split('Door_aot_set')[1].split()
                b=bytes(int(x,16) for x in raw if re.fullmatch('[0-9a-f]{2}',x))
                lf=b[3]&0x80
                k=8 if lf else 0
                nx,ny,nz,nd=struct.unpack_from('<hhhh',b,14+k)
                st,rm,cut=b[22+k],b[23+k],b[24+k]
                src=os.path.basename(f)
                if (st in (1,2) and src.startswith('ROOM2')) or (st==1) or src.startswith('ROOM209') or src.startswith('ROOM208'):
                    print(f'  {src} {off} -> stage {st} room {rm:02X} cut {cut} pos ({nx},{ny},{nz}) dir {nd}  raw {b.hex()}')
