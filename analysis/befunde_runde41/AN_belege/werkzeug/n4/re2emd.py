import sys, struct
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\tools')
import emd_ansichtsblatt as EA
EMS = r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\shared_assets\RE2\CDEMD0.EMS'
u32 = lambda b,o: struct.unpack_from('<I',b,o)[0]
def emd(kind=0x49):
    ems=open(EMS,'rb').read()
    s,l={0x47:(4525,51252),0x49:(4665,51252)}[kind]
    return ems[s*2048:s*2048+l]
def pairs(e):
    d=[u32(e,u32(e,0)+i*4) for i in range(8)]+[u32(e,0)]
    return d
if __name__=='__main__':
    e=emd(); d=pairs(e); print('dir',[hex(x) for x in d])
    sk=EA.emr_parse(e[d[2]:d[3]])
    print('bones',sk.bone_count,'kf',sk.kf_size,'parent',sk.parent)
    for p in range(3):
        edd=e[d[1+2*p]:d[2+2*p]]; emr=e[d[2+2*p]:d[3+2*p]]
        clips,frames=EA.edd_parse(edd)
        print('pair',p+1,'clips',len(clips),[c for f,c in clips])
