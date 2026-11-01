import glob,os
from PIL import Image,ImageDraw
SP=r"/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad".replace('/c/','C:/')
def sheet(v,start,end,step,out,cols=10,W=160,H=120):
    ts=[start+step*i for i in range(int((end-start)/step))]
    rows=(len(ts)+cols-1)//cols
    S=Image.new('RGB',(cols*W,rows*H),(0,0,0)); d=ImageDraw.Draw(S)
    for k,t in enumerate(ts):
        f=SP+f'/gator40/v/{v}/f_{int(t*2)+1:05d}.jpg'
        if not os.path.exists(f): continue
        im=Image.open(f).resize((W,H))
        S.paste(im,((k%cols)*W,(k//cols)*H))
        m=int(t)//60; s=t-m*60
        d.text(((k%cols)*W+2,(k//cols)*H+1),f'{m}:{s:04.1f}',fill=(255,255,0))
    S.save(out,quality=82); print(out,len(ts),S.size)
D=SP+'/gator40/'
sheet('2',0,290,3.5,D+'b2a.jpg')
sheet('2',290,580,3.5,D+'b2b.jpg')
sheet('2',580,860,3.5,D+'b2c.jpg')
sheet('1',0,140,1.5,D+'b1.jpg')
sheet('3',0,74,1.5,D+'b3.jpg',cols=10)
sheet('4',0,44,1.0,D+'b4.jpg',cols=10)
sheet('5',0,54,1.0,D+'b5.jpg',cols=10)
sheet('6',0,59,1.0,D+'b6.jpg',cols=10)
