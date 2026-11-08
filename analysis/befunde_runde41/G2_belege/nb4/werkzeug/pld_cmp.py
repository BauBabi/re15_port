# Vergleich RE2-PL00-Basisclip mit RE1.5-PL00-Clip: EDD-Eintraege + EMR-Keyframe-Bytes (EMR-Kopf u16 armature, u16 frames, u16 bones, u16 elementgroesse)
import struct,sys
def lade(p):
    d=open(p,'rb').read(); dr=struct.unpack_from('<I',d,0)[0]
    edd=struct.unpack_from('<I',d,dr)[0]; emr=struct.unpack_from('<I',d,dr+4)[0]
    return d,edd,emr
def clip(d,edd,c):
    w=struct.unpack_from('<I',d,edd+4*c)[0]; n=w&0xffff; off=w>>16
    return [struct.unpack_from('<I',d,edd+off+4*i)[0] for i in range(n)]
def kf(d,emr,i):
    arm,fr,cnt,es=struct.unpack_from("<HHHH",d,emr); rel=0
    return d[emr+fr+i*es:emr+fr+(i+1)*es],(rel,arm,fr,cnt,es)
a=lade(sys.argv[1]); b=lade(sys.argv[2])
ca,cb=int(sys.argv[3]),int(sys.argv[4])
A=clip(a[0],a[1],ca); B=clip(b[0],b[1],cb)
print('RE2 clip',ca,len(A),'RE1.5 clip',cb,len(B))
print('EMR-Kopf RE2',kf(a[0],a[2],0)[1],'RE1.5',kf(b[0],b[2],0)[1])
gleich=0
for i,(x,y) in enumerate(zip(A,B)):
    ka=kf(a[0],a[2],x&0xfff)[0]; kb=kf(b[0],b[2],y&0xfff)[0]
    g=(ka==kb); gleich+=g
    if i<3 or not g: print(' Bild',i,'EDD',hex(x),hex(y),'Keyframe gleich' if g else 'VERSCHIEDEN')
print('gleich',gleich,'von',min(len(A),len(B)))
