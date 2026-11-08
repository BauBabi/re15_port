import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
sys.path.insert(0,r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
import kopf_orig_bau as K, json
A=np.load(S8+'/m17_adern.npz'); ader=A['ader']; zeile=A['zeile']
F=json.load(open(S8+'/m18_flaechen.json'))
blob=open(EMD_V7,'rb').read(); d,dir_off,ms,tim=K.teile(blob); m=ms[8]
vis={r['f']:r for r in F['8'] if r['x']<0 and r['pv_px']>=10}
sel=[]
for art,fs,uvs,n in (('d',m['tf'],m['tuv'],3),('v',m['qf'],m['quv'],4)):
    for i,(g,uv) in enumerate(zip(fs,uvs)):
        k='%s%d'%(art,i)
        if k in vis: sel.append((k,uv,n,vis[k]['pv_px'],vis[k]['zeile']))
M0=np.zeros(ader.shape,bool)
for k,uv,n,px,z in sel: M0|=K.raster(uv,n,(uv[5]&0xF)*128,ader.shape)
ys,xs=np.nonzero(M0)
print('sichtbare (W)-rechts-Flaechen Mesh 8 (>=10 px in PV):',len(sel),[(k,px,z) for k,uv,n,px,z in sel])
print('Texel',M0.sum(),'u',xs.min(),xs.max(),'v',ys.min(),ys.max(),'Ader heute',int((M0&ader).sum()),'= %.1f %%'%(100*(M0&ader).sum()/M0.sum()))
res={}
for name,zz,u0,u1 in (('A Zeile 480 Seite 0',480,0,128),('B Zeile 481 Seite 1',481,128,256)):
    erl=(zeile==zz); erl[:,:u0]=False; erl[:,u1:]=False
    best=[]
    for dv in range(-ys.min(),256-ys.max()):
        for du in range(u0-xs.min(),u1-xs.max()):
            yy=ys+dv; xx=xs+du
            if erl[yy,xx].all(): best.append((int(ader[yy,xx].sum()),du,dv))
    best.sort(reverse=True); res[name]=best[:5]
    print(name,'beste Schuebe',best[:5],'= %.1f %%'%(100*best[0][0]/M0.sum()) if best else '')
json.dump(dict(flaechen=[(k,px,z) for k,uv,n,px,z in sel],texel=int(M0.sum()),ader_heute=int((M0&ader).sum()),schuebe=res),open(S8+'/m23b.json','w'))
