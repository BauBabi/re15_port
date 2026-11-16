"""Karte der Ader-Texel im Original-EM30 (CDEMD0.EMS Typ 0x30): Texelfarbe = Original-Palette der Zeile, die die Original-Flaechen
auf diesen Texel legen. Ader = rot (Paletten-a* >= 15) und duenn (verschwindet unter Oeffnung 5x5); Flaechen-Rot (Wunde/Auge) = bleibt."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
sys.path.insert(0,r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
import kopf_orig_bau as K, birkin_formen as BF, json, cv2
from scipy import ndimage as nd
import v4_gemein as G
o=BF.lade_em30(); d,dir_off,ms,tim=K.teile(o); t=K.tim_teile(tim)
tex=np.load(S8+'/m15_orig_tex.npy'); lut=np.load(S8+'/m15_orig_lut.npy')
# Zeile je Texel aus den Original-Flaechen (Mehrheit), dazu Mesh-Nutzung
zeile=np.full(tex.shape,-1,int); nutz={}
for i,m in enumerate(ms):
    for uv,n in [(u,3) for u in m['tuv']]+[(u,4) for u in m['quv']]:
        z=(uv[2]>>6)&0x1FF; po=(uv[5]&0xF)*128
        mk=K.raster(uv,n,po,tex.shape)
        zeile[mk]=z
        for q in zip(*np.nonzero(mk)): nutz.setdefault(q,set()).add(i)
LUTL=np.stack([G.lab(lut[z][...,:3].astype(float)) for z in range(lut.shape[0])])
Lt=np.zeros(tex.shape+(3,)); 
for z in range(3):
    m=zeile==480+z; Lt[m]=LUTL[z][tex[m]]
benutzt=zeile>=0
LMAX=float(os.environ.get("LMAX","38")); rot=benutzt&(Lt[...,1]>=15)&(Lt[...,0]<=LMAX)
blob=nd.binary_opening(rot,np.ones((5,5)))
blob=nd.binary_dilation(blob,np.ones((3,3)))&rot
ader=rot&~blob
# Regionen
reg={'A Arm-Adernbaum p0 Zeile 480 (u 0-77 v 0-155)':(0,0,78,156),'B Brust-Adern p1 Zeile 481 (u 128-255 v 0-90)':(128,0,256,91),
     'C Augapfel/Schulter p0 (u 78-127 v 0-100)':(78,0,128,101),'D Bein/Hose p1+p0 Rest':(0,156,256,256),'E Klauenarm p2 Zeile 482 (u 256-300)':(256,0,301,170)}
out={}
for n,(u0,v0,u1,v1) in reg.items():
    s=(slice(v0,v1),slice(u0,u1))
    ms_=set(); 
    for (vv,uu) in zip(*np.nonzero(ader[s])): ms_|=nutz.get((vv+v0,uu+u0),set())
    zz,cc=np.unique(zeile[s][ader[s]],return_counts=True)
    ix=np.unique(tex[s][ader[s]])
    out[n]=dict(texel_benutzt=int(benutzt[s].sum()),ader=int(ader[s].sum()),flaechenrot=int(blob[s].sum()),ader_anteil=round(100*ader[s].sum()/max(1,benutzt[s].sum()),1),
        zeilen=dict(zip([int(z) for z in zz],[int(c) for c in cc])),meshes=sorted(ms_),indizes=len(ix),
        ader_lab_median=[round(float(v),1) for v in np.median(Lt[s][ader[s]],0)] if ader[s].sum() else None,
        haut_lab_median=[round(float(v),1) for v in np.median(Lt[s][benutzt[s]&~rot[s]],0)] if (benutzt[s]&~rot[s]).sum() else None)
    print(n,out[n])
# Index-Exklusivitaet: Ader-Indizes je Zeile, wie oft dieselben Indizes in Nicht-Ader-Texeln derselben Zeile vorkommen
for z in (480,481,482):
    ia=tex[ader&(zeile==z)]; ih=tex[benutzt&~rot&(zeile==z)]
    ua=np.unique(ia); hh=np.isin(ih,ua).mean()*100
    print('Zeile',z,'Ader-Indizes',len(ua),'Anteil Haut-Texel mit Ader-Index %.1f %%'%hh)
np.savez_compressed(S8+'/m17_adern.npz',ader=ader,blob=blob,rot=rot,zeile=zeile,Lt=Lt)
json.dump(out,open(S8+'/m17_adern.json','w'),indent=1,ensure_ascii=False)
# Karte x3
rgb=np.zeros(tex.shape+(3,),np.uint8)
for z in range(3):
    m=zeile==480+z; rgb[m]=lut[z][tex[m]][...,:3]
g=(0.35*rgb.mean(2,keepdims=True)).repeat(3,2).astype(np.uint8)
kar=g.copy(); kar[ader]=(255,40,40); kar[blob]=(255,200,0); kar[~benutzt]=(0,0,40)
k3=cv2.resize(np.hstack([rgb,kar]),None,fx=2,fy=2,interpolation=cv2.INTER_NEAREST)
for (u0,v0,u1,v1) in reg.values():
    for off in (0,384):
        cv2.rectangle(k3,((u0+off)*2,v0*2),((u1+off)*2-1,v1*2-1),(0,255,255),1)
Image.fromarray(k3).save(S8+'/tafeln/05_aderkarte_original_em30.png')
