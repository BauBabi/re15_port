# Leser A V4: Leisten-Panel (Original) per NCC gegen das Videobild n=1350+h verorten -> Abbildung Panel -> 640er-Koordinaten
import sys, json, numpy as np, cv2
SB='C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v3/'
fr=np.load(sys.argv[1]+'/frames1300.npy')
def finde(strip,ncol,orig_col,row_y,h,s,patch=(0.2,0.25,0.8,0.75)):
    S=cv2.imread(SB+strip)[:,:,::-1]; W=S.shape[1]//ncol
    x0=orig_col*W; py0,py1=row_y
    pu0=int(x0+patch[0]*W); pu1=int(x0+patch[2]*W); pv0=int(py0+patch[1]*(py1-py0)); pv1=int(py0+patch[3]*(py1-py0))
    P=cv2.cvtColor(S[pv0:pv1,pu0:pu1].copy(),cv2.COLOR_RGB2GRAY).astype(np.float32)
    F=fr[1350+h-1300]; Fs=cv2.resize(F,(640*s,480*s),interpolation=cv2.INTER_NEAREST)
    G=cv2.cvtColor(Fs,cv2.COLOR_RGB2GRAY).astype(np.float32)
    r=cv2.matchTemplate(G,P,cv2.TM_CCOEFF_NORMED); _,mv,_,ml=cv2.minMaxLoc(r)
    # Strip-Pixel (u,v) in Spalte c -> Bild ((u-c*W - (pu0-x0) + ml0)/s, (v-pv0+ml1)/s)
    return dict(strip=strip,W=W,s=s,ncc=round(float(mv),3),pu0_rel=pu0-x0,pv0=pv0,mx=int(ml[0]),my=int(ml[1]))
out={}
# 02_vorn: 5 Spalten, Zeilen grob: oben 50..630, unten 680..1260 (Originalpixel 3000er)
out['vorn_h118']=finde('02_vorn_h118_h125.jpg',5,1,(60,700),118,2)
out['seite_h83']=finde('01_seite_h83_h85_h92.jpg',5,1,(80,700),83,2)
out['kopf_h118']=finde('05_kopf_nah.jpg',5,1,(60,530),118,4)
out['kopf_h85']=finde('05_kopf_nah.jpg',5,1,(1085,1550),85,4)
for k,v in out.items(): print(k,v)
json.dump(out,open(sys.argv[1]+'/leisten_map.json','w'),indent=1)
