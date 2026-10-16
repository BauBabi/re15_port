import struct,hashlib
base=r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/re15_port/shared_assets/PSX/'
def blob(ems,sector,size):
    d=open(base+'EMD/'+ems,'rb').read(); return d[sector*2048:sector*2048+size]
L=blob('CDEMD0.EMS',1644,127640); E=blob('CDEMD1.EMS',1639,142272)
def secs(b):
    do=struct.unpack_from('<I',b,0)[0]; d=list(struct.unpack_from('<9I',b,do)); d.append(do); return d
for nm in ('MD1','EDD','EMR','TIM'):
    f=open(base+'PLD/ELLIOT.'+nm,'rb').read()
    h=hashlib.md5(f).hexdigest()[:12]
    hits=[]
    for bn,b in (('L',L),('E',E)):
        i=b.find(f[:64])
        if i>=0: hits.append('%s@%#x eq=%s'%(bn,i,b[i:i+len(f)]==f))
    print('ELLIOT.%s %d B md5 %s hits %s'%(nm,len(f),h,hits))
# PL05.PLD
p=open(base+'PLD/PL05.PLD','rb').read()
print('PL05.PLD',len(p),hashlib.md5(p).hexdigest()[:12])
for nm in ('MD1','EDD','EMR','TIM'):
    f=open(base+'PLD/ELLIOT.'+nm,'rb').read()
    i=p.find(f[:64]); print('  ELLIOT.%s in PL05 @%#x eq=%s'%(nm,i, i>=0 and p[i:i+len(f)]==f))
print('L secs',[hex(x) for x in secs(L)]); print('E secs',[hex(x) for x in secs(E)])
