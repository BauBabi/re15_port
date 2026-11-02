import struct
R='C:/workspace/git/reAi_v2/'
def words(path,a,b):
    d=open(path,'rb').read(); return [struct.unpack_from('<I',d,a-0x80100000+i)[0] for i in range(0,b-a,4)]
for tag,base in (('DISC','Version_40%'),('MOD','info/Re1.5')):
    s3=words(R+base+'/PSX/BIN/STAGE3.BIN',0x80116230,0x8011b724)
    s5=words(R+base+'/PSX/BIN/STAGE5.BIN',0x80116a44,0x8011bf38)
    delta=0x814
    eq=same_masked=0; diffs=[]
    for i,(x,y) in enumerate(zip(s3,s5)):
        if x==y: eq+=1; continue
        op=x>>26
        if op in (2,3) and (y>>26)==op and ((((y&0x3ffffff)<<2)|0x80000000)-(((x&0x3ffffff)<<2)|0x80000000))==delta: same_masked+=1; continue
        if (x&0xffff0000)==(y&0xffff0000): same_masked+=1; continue
        diffs.append(i)
    print(tag,'Woerter',len(s3),len(s5),'gleich',eq,'gleich bis auf Verschiebung/Immediate',same_masked,'echt verschieden',len(diffs), [hex(0x80116230+4*i) for i in diffs[:10]])
