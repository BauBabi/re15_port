# Pruefer 1 (Runde 31, Tueren): Messwerkzeug, kein Spielcode. Dossier analysis/befunde_runde31/tueren_04_pruefer1.md
# Ablaufbogen eines Echtlaufs: Abdunkeln (alter Raum) | Sequenz | Ende | neuer Raum (FRAMEDUMP)
import sys, os, glob
from PIL import Image, ImageDraw, ImageFont
Z=sys.argv[1]; out=sys.argv[2]
font=ImageFont.truetype('arial.ttf',14)
def pick(pattern, idx):
    fs=sorted(glob.glob(os.path.join(Z,pattern)))
    if not fs: return []
    r=[]
    for i in idx:
        if i=='last': r.append(fs[-1])
        elif isinstance(i,float): r.append(fs[min(len(fs)-1,int(i*(len(fs)-1)))])
        else:
            f=[x for x in fs if x.endswith('_%03d.ppm'%i) or x.endswith('_%06d.ppm'%i)]
            if f: r.append(f[0])
    return r
bilder=pick('serie/a_abdunkeln_*.ppm',[1,16,32])+pick('serie/b_tuer_*.ppm',[0,0.15,0.3,0.45,0.6,0.75,0.9,'last'])+pick('serie/c_ende_*.ppm',['last'])
neu=[int(x) for x in (sys.argv[3].split(',') if len(sys.argv)>3 else '0,4,8,12,16,24,40,80,160,240'.split(','))]
bilder+=pick('f_*.ppm',neu)
n=len(bilder); cols=5; rows=(n+cols-1)//cols
B=Image.new('RGB',(cols*322,rows*262),(30,30,30)); d=ImageDraw.Draw(B)
for k,f in enumerate(bilder):
    x=(k%cols)*322; y=(k//cols)*262
    B.paste(Image.open(f).convert('RGB').resize((320,240),Image.LANCZOS),(x,y+20))
    lab=os.path.basename(os.path.dirname(f))+'/'+os.path.basename(f) if 'serie' in f else os.path.basename(f)
    d.text((x+2,y+2),lab.replace('.ppm',''),fill=(255,230,120),font=font)
B.save(out); print(out,n)
