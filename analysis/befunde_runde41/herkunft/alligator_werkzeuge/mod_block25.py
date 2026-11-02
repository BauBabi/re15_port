import sys,struct,hashlib
sys.path.insert(0,'.')
from emd_tab import *
b25=blocks(R+'/info/Re1.5/PSX/EMD/CDEMD0.EMS')[25][1]
D=E.emd_dir(b25); print('dir',D,'len',len(b25))
sec=[(D[i],(D[i+1] if i+1<len(D) else u32(b25,0))-D[i]) for i in range(len(D))]
print(sec)
md=b25[D[7]:D[8]]
print('md1 head',struct.unpack_from('<III',md,0),len(md))
print(md[:12].hex())
emr=b25[D[2]:D[3]]
s=E.emr_parse(emr); print('kno',s.bone_count,'rel',s.rel,'parent',s.parent,'kf_size',s.kf_size,'kf',s.kf_count)
clips,fr=E.edd_parse(b25[D[1]:D[2]]); print('clips',len(clips),[c[1] for c in clips])
print('dir4-6',[b25[D[i]:D[i]+16].hex() for i in (3,4,5,6)])
