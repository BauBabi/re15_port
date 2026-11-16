import sys, os, numpy as np, cv2, json
sys.path.insert(0,'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/V4F_skripte')
import v4f
O=os.path.dirname(os.path.abspath(__file__))+'/asset'
k=v4f.kand(O)
print('Clips',len(k.mo.clips),'Meshes',len(k.mo.meshes))
for n in (1,2):
    s,K=v4f.szene_id(O,n)
    cv2.imwrite('szene%d.png'%n,cv2.cvtColor(s,cv2.COLOR_RGB2BGR))
    np.savez_compressed('kennung%d.npz'%n,**{a:K[a] for a in ('mesh','flaeche','zeile','eintrag','maske')})
    m=K['mesh']; print(n,'Modellpixel',int(K['maske'].sum()),'Meshes sichtbar',sorted(set(np.unique(m[m>=0]).tolist())))
    # Gegenprobe Kandidat.rendern
    sa,alb,roh,kk=k.rendern(n)
    print('  Gegenprobe Kandidat.rendern: Pixel verschieden',int((np.abs(sa.astype(int)-s.astype(int)).max(2)>0).sum()))
