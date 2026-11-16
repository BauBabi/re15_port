"""Plan-Probe UV: alle Flaechen Mesh 8 (W)-rechts (Zeile 480) gemeinsam um (du, dv) verschieben (Form bleibt, Naehte bleiben stimmig),
so dass moeglichst viele Ader-Texel (Original-Aderkarte, Bereich A Zeile 480) darunter liegen; Nebenbedingung: jeder Texel des
verschobenen UV liegt auf einem Texel, den das Original mit Zeile 480 benutzt. Danach Rot-Anteil in PV (nur im Speicher)."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
sys.path.insert(0,r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
import kopf_orig_bau as K, json
A=np.load(S8+'/m17_adern.npz'); ader=A['ader']; zeile=A['zeile']
blob=open(EMD_V7,'rb').read(); d,dir_off,ms,tim=K.teile(blob)
m=ms[8]
sel=[]
for art,fs,uvs,n in (('d',m['tf'],m['tuv'],3),('v',m['qf'],m['quv'],4)):
    for i,(g,uv) in enumerate(zip(fs,uvs)):
        z=(uv[2]>>6)&0x1FF; x=np.mean([m['v'][k][0] for k in g[1::2]])
        if x<0 and z==480: sel.append((art,i,uv,n))
M0=np.zeros(ader.shape,bool)
for art,i,uv,n in sel: M0|=K.raster(uv,n,(uv[5]&0xF)*128,ader.shape)
ys,xs=np.nonzero(M0); print('Flaechen',len(sel),'Texel',M0.sum(),'u',xs.min(),xs.max(),'v',ys.min(),ys.max(),'Ader heute',int((M0&ader).sum()))
erlaubt=(zeile==480); erlaubt[:,128:]=False
best=[]
for dv in range(-ys.min(),256-ys.max()):
    for du in range(-xs.min(),128-xs.max()):
        yy=ys+dv; xx=xs+du
        if not erlaubt[yy,xx].all(): continue
        best.append((int(ader[yy,xx].sum()),du,dv))
best.sort(reverse=True)
print('beste Schuebe (Ader-Texel, du, dv):',best[:8],'Kandidaten',len(best))
json.dump(dict(flaechen=[(a,i) for a,i,_,_ in sel],texel=int(M0.sum()),ader_heute=int((M0&ader).sum()),beste=best[:20]),open(S8+'/m23_uvschub.json','w'))
