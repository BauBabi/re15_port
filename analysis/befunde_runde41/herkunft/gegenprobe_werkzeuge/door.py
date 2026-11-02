import glob, os, re, struct, collections
exec(open('zen2.py').read().split('alle={}')[0])
for tag,base in (('DISC','Version_40%/PSX'),('MOD','info/Re1.5/PSX')):
    tgt=collections.Counter(); hit=[]
    for p in sorted(glob.glob(R+base+'/STAGE*/ROOM*.RDT')):
        d=open(p,'rb').read()
        if len(d)<92: continue
        for idx,a,e in regions(d):
            for m in re.finditer(b'\x3b',d[a:e]):
                o=a+m.start(); b=d[o:o+40]
                if len(b)<32: continue
                k=8 if b[3]&0x80 else 0
                st,rm,cut=b[22+k],b[23+k],b[24+k]
                if st<7 and rm<0x30 and cut<0x20:
                    if st==1: tgt[(os.path.basename(p),'2%02X'%rm)]+=1
                    if st==1 and rm in (8,9,10): hit.append((os.path.basename(p),hex(o),b[:32].hex(' ')))
    print(tag,'Tueren (roh, SCD) nach Stufe 2:',sorted(set((r) for (_,r) in tgt)))
    print('   nach 208/209/20A:',hit)
