"""V7 Meshes 8/13 (+10,14): je Flaeche Zeile, Seite (Knochenraum x des Schwerpunkts; x<0 = (W)-rechts), Ader-Texel unter dem UV
(Original-Aderkarte m17), sichtbare Pixel in PV."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
sys.path.insert(0,r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
import kopf_orig_bau as K, json
blob=open(EMD_V7,'rb').read(); d,dir_off,ms,tim=K.teile(blob)
A=np.load(S8+'/m17_adern.npz'); ader=A['ader']
Z=np.load(S8+'/m10_pv_v7.npz'); bone=Z['bone'][:480]; fl=Z['flaeche'][:480]
print('mesh keys',list(ms[8].keys()))
out={}
for mi in (8,13,10,14):
    m=ms[mi]; rows=[]
    for art,faces,uvs,n in (('d',m['tf'],m['tuv'],3),('v',m['qf'],m['quv'],4)):
        for fi,(g,uv) in enumerate(zip(faces,uvs)):
            ie=g[1::2]; P=np.array([m['v'][i][:3] for i in ie],float)
            z=(uv[2]>>6)&0x1FF; po=(uv[5]&0xF)*128
            mk=K.raster(uv,n,po,ader.shape)
            key='%s%d'%(art,fi)
            vis=int(((bone==mi)&(fl==key)).sum())
            rows.append(dict(f=key,zeile=int(z),x=round(float(P[:,0].mean())),texel=int(mk.sum()),ader=int((mk&ader).sum()),pv_px=vis,
                             u=[int(mk.nonzero()[1].min()),int(mk.nonzero()[1].max())],v=[int(mk.nonzero()[0].min()),int(mk.nonzero()[0].max())]))
    out[mi]=rows
    R=[r for r in rows]; wr=[r for r in R if r['x']<0]; wl=[r for r in R if r['x']>=0]
    def summ(rs): 
        zs={}
        for r in rs: zs[r['zeile']]=zs.get(r['zeile'],0)+1
        return dict(n=len(rs),zeilen=zs,mit_ader=sum(1 for r in rs if r['ader']>0),ader_texel=sum(r['ader'] for r in rs),pv_px=sum(r['pv_px'] for r in rs),pv_px_mit_ader=sum(r['pv_px'] for r in rs if r['ader']>0))
    print('mesh',mi,'(W)-rechts x<0',summ(wr)); print('mesh',mi,'(W)-links x>=0',summ(wl))
json.dump(out,open(S8+'/m18_flaechen.json','w'))
