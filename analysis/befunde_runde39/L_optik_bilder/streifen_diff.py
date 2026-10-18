import sys
from PIL import Image
S=r"C:\Users\mjoedicke\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\L39"
def diff(a,b,x0,y0,x1,y1,thr=24):
    A=Image.open(a).convert("RGB"); B=Image.open(b).convert("RGB")
    pa,pb=A.load(),B.load()
    cols={}
    n=0
    for y in range(y0,y1+1):
        for x in range(x0,x1+1):
            ca,cb=pa[x,y],pb[x,y]
            if max(abs(ca[i]-cb[i]) for i in range(3))>thr:
                n+=1; cols[x]=cols.get(x,0)+1
    return n,cols
for name,(a,b) in {"ALT v_mit/v_ohne":(S+r"\runs\v_mit\f.ppm",S+r"\runs\v_ohne\f.ppm"),
                   "NEU n2_mit/n2_ohne":(S+r"\runs\n2_mit\f.ppm",S+r"\runs\n2_ohne\f.ppm")}.items():
    n,cols=diff(a,b,150,40,200,170)
    print(name,"Punkte>24 in x150..200 y40..170:",n)
    print("  je Spalte:"," ".join("%d:%d"%(x,cols[x]) for x in sorted(cols)))
    nl=sum(v for x,v in cols.items() if x<=172); nr=sum(v for x,v in cols.items() if x>=173)
    print("  Spalten <=172 (jenseits der gemalten Ecke):",nl," | >=173 (Wand):",nr)
# Bildstreifen zum Ansehen: alt-mit, alt-ohne, neu-mit, neu-ohne  (x 140..200, y 40..170) x4
ims=[Image.open(p).convert("RGB").crop((140,40,200,170)) for p in (S+r"\runs\v_mit\f.ppm",S+r"\runs\v_ohne\f.ppm",S+r"\runs\n2_mit\f.ppm",S+r"\runs\n2_ohne\f.ppm")]
W,H=ims[0].size; out=Image.new("RGB",(4*W*4+3*8,H*4),(255,0,255))
for i,im in enumerate(ims): out.paste(im.resize((W*4,H*4),Image.NEAREST),(i*(W*4+8),0))
out.save(S+r"\img\n2_alt_neu_mit_ohne_x4.png"); print("bild ok")
