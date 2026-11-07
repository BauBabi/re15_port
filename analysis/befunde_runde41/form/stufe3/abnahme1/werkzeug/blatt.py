import sys,os,glob
from PIL import Image,ImageDraw,ImageFont
# blatt.py <bilddir> <aus.png> <von> <bis> <schritt> <spalten> [breite] [titel] [labelfn: idx->text via offset,fps: off=F0-bild]
d,aus,von,bis,schritt,sp=sys.argv[1],sys.argv[2],int(sys.argv[3]),int(sys.argv[4]),int(sys.argv[5]),int(sys.argv[6])
bw=int(sys.argv[7]) if len(sys.argv)>7 else 320
titel=sys.argv[8] if len(sys.argv)>8 else ''
fmap=sys.argv[9] if len(sys.argv)>9 else ''   # "bild0,F0,fps" -> F = F0 + (i-bild0)*30/fps
idx=list(range(von,bis+1,schritt))
ims=[]
for i in idx:
    p=os.path.join(d,'b_%04d.png'%i)
    if os.path.exists(p): ims.append((i,Image.open(p).convert('RGB')))
w0,h0=ims[0][1].size; bh=int(h0*bw/w0)
rows=(len(ims)+sp-1)//sp
th=24 if titel else 0
out=Image.new('RGB',(sp*bw,th+rows*(bh+16)),(20,20,24))
dr=ImageDraw.Draw(out)
try: fnt=ImageFont.truetype('C:/Windows/Fonts/consola.ttf',12); ft=ImageFont.truetype('C:/Windows/Fonts/segoeui.ttf',15)
except: fnt=ft=None
if titel: dr.text((6,3),titel,fill=(230,230,230),font=ft)
for k,(i,im) in enumerate(ims):
    x=(k%sp)*bw; y=th+(k//sp)*(bh+16)
    out.paste(im.resize((bw,bh)),(x,y))
    lab='Bild %d'%i
    if fmap:
        b0,F0,fps=[float(v) for v in fmap.split(',')]
        lab+='  ~F%d'%round(F0+(i-b0)*30/fps)
    dr.text((x+4,y+bh+1),lab,fill=(240,200,120),font=fnt)
out.save(aus); print(aus,out.size)
