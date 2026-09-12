#!/usr/bin/env python3
import struct
REPO="c:/workspace/git/reAi_v2"
emd=open(REPO+"/re15_port/shared_assets/RE2/EM23.EMD","rb").read()
diroff,dircnt=struct.unpack_from("<II",emd,0)
dirs=[struct.unpack_from("<I",emd,diroff+4*i)[0] for i in range(dircnt)]
order=sorted(range(dircnt),key=lambda i:dirs[i]); ends={}
for k,i in enumerate(order): ends[i]=dirs[order[k+1]] if k+1<len(order) else diroff

def parse_edd(base,size):
    d=emd[base:base+size]; clips=[]
    cnt0,off0=struct.unpack_from("<HH",d,0); n=off0//4
    clips.append((0,cnt0)); cur=4
    for i in range(1,n):
        c,o=struct.unpack_from("<HH",d,cur); clips.append(((o-off0)//4,c)); cur+=4
    return [[struct.unpack_from("<I",d,off0+4*(f+j))[0] for j in range(c)] for f,c in clips]

def parse_emr(base,size):
    d=emd[base:base+size]
    bto,kfo,bc,kfs=struct.unpack_from("<HHHH",d,0)
    relpos=[struct.unpack_from("<hhh",d,8+6*b) for b in range(bc)]
    return dict(bones=bc,kfsize=kfs,kfo=kfo,kfdata=d[kfo:],relpos=relpos)

e3=parse_edd(dirs[5],ends[5]-dirs[5]); m3=parse_emr(dirs[6],ends[6]-dirs[6])
print("PAAR3: %d clips, EMR %d bones kf=%dB nkf=%d"%(len(e3),m3['bones'],m3['kfsize'],len(m3['kfdata'])//m3['kfsize']))
print("EDD3 raw clip table:", emd[dirs[5]:dirs[5]+16].hex())
print("relpos bones:",m3['relpos'])
for ci,fw in enumerate(e3):
    print("\n clip %d: %d frames"%(ci,len(fw)))
    for j,w in enumerate(fw):
        k=w&0xFFF; b=k*m3['kfsize']
        x,y,z,sx,sy,sz=struct.unpack_from("<hhhhhh",m3['kfdata'],b)
        fl="" if w>>12==0 else "  FLAGS=0x%08X"%w
        if j%5==0 or w>>12 or j==len(fw)-1:
            print("  f%03d kf%4d  off=(%6d,%6d,%6d) spd=(%6d,%6d,%6d)%s"%(j,k,x,y,z,sx,sy,sz,fl))
# angles of victim root bone (bone0) over clip1 to see the "Rumschleudern"
def angles(kf,bone):
    base=kf*m3['kfsize']+12
    bits=bone*36
    out=[]
    for c in range(3):
        bo=bits+c*12; byi=bo>>3; sh=bo&7
        v=(m3['kfdata'][byi] | (m3['kfdata'][byi+1]<<8) | (m3['kfdata'][byi+2]<<16) if byi+2<len(m3['kfdata']) else 0)
        # simpler: read within frame
        out.append(None)
    return out
