import sys
from PIL import Image
S=r"C:\Users\mjoedicke\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\L39"
def mess(mit,ohne,sk,thr=30):
    A=Image.open(S+"/runs/"+mit+"/f.ppm").convert("RGB"); B=Image.open(S+"/runs/"+ohne+"/f.ppm").convert("RGB")
    pa,pb=A.load(),B.load(); W,H=A.size
    x0,y0,x1,y1=(100*sk,55*sk,170*sk,100*sk)  # Pinnwand-Fenster
    pts=[]; sdl=0.0
    for y in range(y0,y1):
        for x in range(x0,x1):
            ca,cb=pa[x,y],pb[x,y]
            d=abs(ca[0]-cb[0])+abs(ca[1]-cb[1])+abs(ca[2]-cb[2])
            if d>thr:
                pts.append((x,y)); la=0.299*ca[0]+0.587*ca[1]+0.114*ca[2]; lb=0.299*cb[0]+0.587*cb[1]+0.114*cb[2]; sdl+=abs(la-lb)
    if not pts: print("%s: 0 Pixel"%mit); return
    xs=[p[0] for p in pts]; ys=[p[1] for p in pts]
    print("%s (Skala %d): %d Pixel (|dR|+|dG|+|dB| > %d), Huelle x %d..%d (%d breit) y %d..%d (%d hoch), Sum|dL| %.0f"%(
        mit,sk,len(pts),thr,min(xs),max(xs),max(xs)-min(xs)+1,min(ys),max(ys),max(ys)-min(ys)+1,sdl))
    # Ausschnitt mit/ohne x4 (bei Skala 1) bzw. x2 (Skala 3)
    f=4 if sk==1 else 2
    cx0,cy0,cx1,cy1=(min(xs)-8*sk,min(ys)-6*sk,max(xs)+9*sk,max(ys)+7*sk)
    a=A.crop((cx0,cy0,cx1,cy1)); b=B.crop((cx0,cy0,cx1,cy1))
    out=Image.new("RGB",((cx1-cx0)*f*2+6,(cy1-cy0)*f),(255,0,255))
    out.paste(a.resize(((cx1-cx0)*f,(cy1-cy0)*f),Image.NEAREST),(0,0)); out.paste(b.resize(((cx1-cx0)*f,(cy1-cy0)*f),Image.NEAREST),((cx1-cx0)*f+6,0))
    out.save(S+"/img/"+mit+"_x%d.png"%f)
for mit,ohne,sk in [(a,b,int(c)) for a,b,c in (x.split(":") for x in sys.argv[1:])]:
    mess(mit,ohne,sk)
