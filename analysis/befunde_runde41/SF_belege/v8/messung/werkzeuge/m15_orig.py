import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
sys.path.insert(0,r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
import kopf_orig_bau as K, birkin_formen as BF, hashlib, json
import b2
o=BF.lade_em30(); print('orig len',len(o),hashlib.sha256(o).hexdigest()[:16])
d,dir_off,ms,tim=K.teile(o); t=K.tim_teile(tim)
print('tim cx cy cw',t['cx'],t['cy'],t['cw'],'clut',t['clut'].shape,'pix',t['pix'].shape)
mo=b2.modell(o)
print('orig meshes',len(mo.meshes),'clips',len(mo.clips),'lut',mo.lut.shape,'clut_y',mo.clut_y,'tex',mo.texw,mo.texh)
# Welche CLUT-Zeile nutzt welches Mesh, Texelbereiche
for i,m in enumerate(ms):
    zs={}; 
    for uv in m['tuv']+m['quv']:
        z=(uv[2]>>6)&0x1FF; po=(uv[5]&0xF)*128; zs[z]=zs.get(z,0)+1
    us=[uv[k]+(uv[5]&0xF)*128 for uv in m['tuv']+m['quv'] for k in (0,3,6)]; vs=[uv[k] for uv in m['tuv']+m['quv'] for k in (1,4,7)]
    print('mesh',i,'flaechen',len(m['tuv'])+len(m['quv']),'zeilen',zs,'u',min(us),max(us),'v',min(vs),max(vs))
np.save(S8+'/m15_orig_tex.npy',mo.tex); np.save(S8+'/m15_orig_lut.npy',mo.lut)
for z in range(mo.lut.shape[0]):
    rgb=mo.lut[z][mo.tex][...,:3]
    Image.fromarray(rgb.astype(np.uint8)).resize((768,512),Image.NEAREST).save(S8+'/m15_orig_tex_zeile%d.png'%z)
