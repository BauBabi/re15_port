import json,glob,os
from PIL import Image,ImageDraw
SP=r"/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad".replace('/c/','C:/')
N=json.load(open(SP+'/gator40/ncc.json'))
def sheet(v,times,out,cols=8,W=200,H=150):
    rows=(len(times)+cols-1)//cols
    S=Image.new('RGB',(cols*W,rows*H),(0,0,0)); d=ImageDraw.Draw(S)
    for k,t in enumerate(times):
        f=SP+f'/gator40/v/{v}/f_{int(t*2)+1:05d}.jpg'
        if not os.path.exists(f): continue
        im=Image.open(f).resize((W,H))
        S.paste(im,((k%cols)*W,(k//cols)*H))
        m=int(t)//60; s=t-m*60
        d.text(((k%cols)*W+3,(k//cols)*H+2),f'V{v} {m}:{s:04.1f}',fill=(255,255,0))
    S.save(out,quality=85); print(out,len(times),S.size)
# Video 1: 104-116 s
sheet('1',[104+0.5*i for i in range(0,24)],'../alligator_belege/v1_104s.jpg')
# Video 2: alle Kanalbilder (bester Raum 2xx >=0.70), hoechstens eines je 4 s
L=[]
last=-99
for t,b in N['2']:
    n,sc=b[0]
    if n[0]=='2' and sc>=0.68 and t-last>=4:
        L.append(t); last=t
print(len(L),L[:60])
sheet('2',L[:64],'../alligator_belege/v2_kanal.jpg')
for v in '3456':
    L=[];last=-99
    for t,b in N[v]:
        n,sc=b[0]
        if n[0]=='2' and sc>=0.55 and t-last>=3:
            L.append(t); last=t
    print(v,L)
