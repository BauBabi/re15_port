# Leser A V4: V3-r2-Spalte (5. Panel) der Leisten in 640er-Bildkoordinaten zurueckrechnen (Mittel ueber s x s Strip-Pixel)
import sys, json, numpy as np, cv2
SB='C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v3/'
O=sys.argv[1]; M=json.load(open(O+'/leisten_map.json'))
ROWS={'vorn_h118':(60,630),'seite_h83':(95,690),'kopf_h118':(75,530),'kopf_h85':(1095,1550)}
for k,m in M.items():
    S=cv2.imread(SB+m['strip'])[:,:,::-1].astype(np.float32); W=m['W']; s=m['s']
    for col,name in [(4,'V3'),(1,'ORIG')]:
        can=np.zeros((480,640,3),np.float32); cnt=np.zeros((480,640),np.float32)
        v0,v1=ROWS[k]
        for v in range(v0,v1):
            for u in range(col*W,(col+1)*W):
                x=(u-col*W-m['pu0_rel']+m['mx'])//s; y=(v-m['pv0']+m['my'])//s
                if 0<=x<640 and 0<=y<480: can[y,x]+=S[v,u]; cnt[y,x]+=1
        ok=cnt>=s*s
        img=np.zeros_like(can); img[ok]=can[ok]/cnt[ok][:,None]
        np.save(O+'/port_%s_%s.npy'%(k,name),img); np.save(O+'/port_%s_%s_ok.npy'%(k,name),ok)
        ys,xs=np.where(ok); print(k,name,'Bereich x%d-%d y%d-%d'%(xs.min(),xs.max(),ys.min(),ys.max()))
        cv2.imwrite(O+'/port_%s_%s.png'%(k,name),img[:,:,::-1].astype(np.uint8))
