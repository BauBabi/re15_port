import sys, struct, numpy as np
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/tools")
from form_kamera import blickmatrix
d=open(r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT","rb").read()
cams={}
for k in range(16):
    e=d[0x60+32*k:0x60+32*k+32]
    fl,fov=struct.unpack_from("<HH",e,0); v=struct.unpack_from("<6i",e,4)
    cams[k]=(np.array(v[:3],float),np.array(v[3:],float),fov>>7)
def proj(cut,p):
    a,z,h=cams[cut]; R=blickmatrix(a,z); v=R@(np.asarray(p,float)-a)
    if v[2]<=0: return None
    return (160+h*v[0]/v[2], 120+h*v[1]/v[2], v[2])
pts={k:[float(x) for x in s.split(",")] for k,s in (a.split("=") for a in sys.argv[2:])}
for cut in [int(c) for c in sys.argv[1].split(",")]:
    print("Cut",cut)
    for n,(x,z) in pts.items():
        f=proj(cut,(x,0,z)); hd=proj(cut,(x,-1700,z)); top=proj(cut,(x,-4500,z))
        fmt=lambda q: "hinter" if q is None else "(%4d,%4d) tief %5d"%(q[0],q[1],q[2])
        print("  %-8s Fuss %s Kopf %s y-4500 %s"%(n,fmt(f),fmt(hd),fmt(top)))
