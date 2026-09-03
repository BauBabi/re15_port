"""Q14: Lassen sich die Vordergrundregionen aus dem BILD lernen?

Letzte verbliebene Option fuer die 1468 Cuts ohne jede Vordergrunddaten: kein Atlas
(also keine Kunst zum Wiederfinden), und die Kollision taugt nicht (q13: 26-28 %
Praezision, weil SCA vor allem Raumwaende enthaelt).

⛔ ABNAHMESCHWELLE, VOR DEM LAUF FESTGELEGT: Die Regel dieses ganzen Zusatzes lautet,
dass eine FEHLENDE Maske alles laesst wie heute, eine FALSCHE aber Hintergrund ueber
die Figur malt. Fuer die Atlas-Cuts wurde deshalb eine Praezision von 99.4-99.7 %
erreicht und akzeptiert. Ein gelernter Segmentierer wird das nicht halten; als Grenze
fuer eine Auslieferung setze ich >= 95 % Praezision bei einer Ausbeute, die den
Aufwand lohnt (>= 25 %). Wird das nicht erreicht, werden diese Cuts NICHT automatisch
gefuellt — dann ist ein Werkzeug fuer Handarbeit die ehrlichere Antwort.

Aufteilung NACH RAUM, nicht nach Cut: Cuts desselben Raums zeigen dieselben Objekte,
eine cutweise Aufteilung waere ein Datenleck und wuerde die Guete schoenrechnen.
"""
import os, sys, glob, struct, random
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import bg_rgb, TBL, u32, ROOT
from q3b_tiefe import groups_masks

import torch, torch.nn as nn, torch.nn.functional as F

def dataset():
    X, Y, ROOMS = [], [], []
    for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
        name=os.path.basename(rdt)[:-4]; rid=int(name[4:],16)
        if rid & 1: continue
        stage=(rid>>12)&0xF
        if stage not in TBL: continue
        d=open(rdt,'rb').read()
        if len(d)<0x60: continue
        nCut=d[1]; cam=u32(d,0x24)
        if not cam or cam+32*nCut>len(d): continue
        for c in range(nCut):
            ms=[m for m in groups_masks(d,u32(d,cam+c*32+0x1C)) if m[3]>0 and m[4]>0]
            if len(ms)<4: continue
            B=bg_rgb(stage,rid,c)
            if B is None: continue
            t=np.zeros((240,320),np.float32)
            for (gi,X_,Y_,w,h,dep) in ms:
                x0,x1=max(0,X_),min(320,X_+w); y0,y1=max(0,Y_),min(240,Y_+h)
                if x1>x0 and y1>y0: t[y0:y1,x0:x1]=1.0
            if t.sum()<200: continue
            X.append((B.astype(np.float32)/255.0).transpose(2,0,1))
            Y.append(t); ROOMS.append(rid>>4)
    return np.array(X), np.array(Y), np.array(ROOMS)

class Net(nn.Module):
    """Kleines U-Net. Bewusst klein: 480 Beispiele tragen kein grosses Modell."""
    def __init__(s):
        super().__init__()
        ch=[3,24,48,96]
        s.e1=nn.Sequential(nn.Conv2d(ch[0],ch[1],3,padding=1), nn.ReLU(), nn.Conv2d(ch[1],ch[1],3,padding=1), nn.ReLU())
        s.e2=nn.Sequential(nn.Conv2d(ch[1],ch[2],3,padding=1), nn.ReLU(), nn.Conv2d(ch[2],ch[2],3,padding=1), nn.ReLU())
        s.e3=nn.Sequential(nn.Conv2d(ch[2],ch[3],3,padding=1), nn.ReLU(), nn.Conv2d(ch[3],ch[3],3,padding=1), nn.ReLU())
        s.d2=nn.Sequential(nn.Conv2d(ch[3]+ch[2],ch[2],3,padding=1), nn.ReLU())
        s.d1=nn.Sequential(nn.Conv2d(ch[2]+ch[1],ch[1],3,padding=1), nn.ReLU())
        s.out=nn.Conv2d(ch[1],1,1)
    def forward(s,x):
        e1=s.e1(x); e2=s.e2(F.max_pool2d(e1,2)); e3=s.e3(F.max_pool2d(e2,2))
        d2=s.d2(torch.cat([F.interpolate(e3,scale_factor=2),e2],1))
        d1=s.d1(torch.cat([F.interpolate(d2,scale_factor=2),e1],1))
        return s.out(d1)

def main():
    X,Y,R=dataset()
    print("Beispiele %d aus %d Raeumen"%(len(X),len(set(R.tolist()))))
    rooms=sorted(set(R.tolist())); random.Random(7).shuffle(rooms)
    k=max(1,len(rooms)//5); test=set(rooms[:k])
    tr=np.array([r not in test for r in R]); te=~tr
    print("Training %d / Test %d Cuts (%d Testraeume)"%(tr.sum(),te.sum(),len(test)))
    dev='cuda' if torch.cuda.is_available() else 'cpu'
    net=Net().to(dev); opt=torch.optim.Adam(net.parameters(),3e-4)
    # Klassenausgleich: Vordergrund ist die Minderheit, ohne Gewicht kollabiert das
    # Modell auf "alles Hintergrund" (erster Versuch: Verlust 0.48, Ausgabe konstant).
    pos = float(Y.mean()); pw = torch.tensor([(1.0-pos)/max(pos,1e-6)], device=dev)
    print("Vordergrundanteil %.1f %% -> pos_weight %.2f"%(100*pos, pw.item()))
    Xtr=torch.tensor(X[tr]); Ytr=torch.tensor(Y[tr]).unsqueeze(1)
    Xte=torch.tensor(X[te]).to(dev); Yte=torch.tensor(Y[te]).unsqueeze(1).to(dev)
    n=len(Xtr); bs=8
    for ep in range(160):
        net.train(); perm=torch.randperm(n)
        for i in range(0,n,bs):
            idx=perm[i:i+bs]
            xb=Xtr[idx].to(dev); yb=Ytr[idx].to(dev)
            opt.zero_grad()
            lg=net(xb)
            bce=F.binary_cross_entropy_with_logits(lg,yb,pos_weight=pw)
            pr=torch.sigmoid(lg)
            dice=1.0-(2*(pr*yb).sum()+1.0)/((pr*pr).sum()+(yb*yb).sum()+1.0)
            loss=bce+dice
            loss.backward(); opt.step()
        if ep%40==39:
            net.eval()
            with torch.no_grad(): p=torch.sigmoid(net(Xte))
            print("  Epoche %2d  Verlust %.4f"%(ep+1,loss.item()))
    net.eval()
    with torch.no_grad(): P=torch.sigmoid(net(Xte)).cpu().numpy()[:,0]
    T=Yte.cpu().numpy()[:,0]
    print("\nSchwelle  Praezision  Ausbeute   (Testraeume, nie im Training gesehen)")
    for thr in (0.5,0.7,0.9,0.95,0.99):
        pm=P>thr
        hit=(pm&(T>0.5)).sum(); mine=pm.sum(); theirs=(T>0.5).sum()
        print("   %.2f      %5.1f %%     %5.1f %%"%(thr,100*hit/max(1,mine),100*hit/max(1,theirs)))

main()
