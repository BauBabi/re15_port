import numpy as np, cv2, glob, os, json
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
GR={'hinten':'Rückseite.png','vorn':'birkin_form3_02.png'}
LAB={'hinten':['Rücken.png','Schwanz_hinten.png','Kopf_von_hinten.png','linkes_bein_hinten.png','rechtes_bein_hinten.png','linker_rechter_Fuß_hinten.png','Bein_Vorne_rechts.png'],
     'vorn':['Gesicht vorne.png','Vorne_Oberkörper.png','vorne_rechte_Hand_und_Kralle.png','Vorderes_Linkes_Bein_mit_fuß.png']}
res={}
for g,gf in GR.items():
    B=lies(D+gf).astype(np.int16); h,w=B.shape[:2]
    for lf in LAB[g]:
        L=lies(D+lf).astype(np.int16); H,W=L.shape[:2]
        best=None
        for dy in range(0,H-h+1):
          for dx in range(0,W-w+1):
            S=L[dy:dy+h,dx:dx+w]
            eq=(np.abs(S-B).max(2)==0).mean()
            if best is None or eq>best[0]: best=(eq,dx,dy)
        eq,dx,dy=best
        S=L[dy:dy+h,dx:dx+w]; d=np.abs(S-B).max(2)
        # outside region: is canvas outside base transparent/white?
        out=np.ones((H,W),bool); out[dy:dy+h,dx:dx+w]=False
        print(g,lf,(W,H),'dx,dy',dx,dy,'gleich %.4f'%eq,'diff>0',(d>0).sum(),'diff>8',(d>8).sum(), 'aussen alpha>0',(L[...,3][out]>0).sum())
        res[lf]=dict(grund=gf,dx=dx,dy=dy,gleich=float(eq))
json.dump(res,open('versatz.json','w'),ensure_ascii=False,indent=1)
