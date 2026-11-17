import sys,numpy as np,cv2,json
sys.path.insert(0,'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/V4F_skripte')
import v4f
Z=sys.argv[1]
zv=dict(np.load('zonen_vorlage.npz'))
FARB={14:(255,80,80),15:(80,255,80),28:(80,80,255),30:(255,255,0),19:(255,0,255),20:(0,255,255),29:(255,160,0),31:(160,0,255),25:(255,255,255),27:(200,200,200)}
def panel(n,ken,box,zonen,titel,meshes):
    x0,y0,x1,y1=box
    if ken is None: im=cv2.cvtColor(v4f.original(n),cv2.COLOR_RGB2BGR)
    else:
        im=cv2.imread('szene%d.png'%n) if ken=='asset' else cv2.cvtColor(v4f.szene_id('C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/stufe3/basis_k5c',n)[0],cv2.COLOR_RGB2BGR)
        K=dict(np.load('kennung%d.npz'%n)) if ken=='asset' else dict(np.load('kennung_k5c_%d.npz'%n))
        for b in meshes:
            s=K['mesh']==b; im[s]=(im[s]*0.2+np.array(FARB[b][::-1])*0.8).astype(np.uint8)
    for z in zonen:
        cnt,_=cv2.findContours(zv[z].astype(np.uint8),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE); cv2.drawContours(im,cnt,-1,(232,162,0),1)
    q=cv2.resize(im[y0:y1,x0:x1],None,fx=3,fy=3,interpolation=cv2.INTER_NEAREST)
    k=np.full((20,q.shape[1],3),255,np.uint8); cv2.putText(k,titel,(3,14),0,0.42,(0,0,0),1)
    return np.vstack([k,q])
def legende(meshes,w):
    k=np.full((22,w,3),255,np.uint8); x=4
    for b in meshes:
        cv2.rectangle(k,(x,5),(x+12,17),FARB[b][::-1],-1); cv2.putText(k,str(b),(x+15,16),0,0.42,(0,0,0),1); x+=45
    return k
for name,n,box,zon,meshes in (('krallen_vorn_links',2,(380,340,500,440),['krallen_vorn_links','fussabschnitt_vorn_links'],[14,15,28,30]),
                              ('krallen_vorn_rechts',2,(180,370,300,480),['krallen_vorn_rechts','hand_vorn_rechts'],[19,20,29,31]),
                              ('krallen_hinten_links',1,(200,340,300,440),['fusskrallen_hinten_links','fuss_hinten_links'],[25])):
    ps=[panel(n,None,box,zon,'Vorlage + Nutzer',meshes),panel(n,'asset',box,zon,'Asset K5e_f1 (eingezogen)',meshes),panel(n,'k5c',box,zon,'K5c (vor dem Einziehen)',meshes)]
    sp=np.full((ps[0].shape[0],4,3),255,np.uint8)
    t=np.hstack([ps[0],sp,ps[1],sp,ps[2]])
    cv2.imwrite(Z+'/tafeln/krallen_%s.png'%name.replace('krallen_',''),np.vstack([t,legende(meshes,t.shape[1])]))
f=cv2.imread('fuesse_rundum.png'); cv2.imwrite(Z+'/tafeln/krallen_fuesse_rundum.png',f[:600])
