import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import v4_gemein as G, b2, birkin_buehne as BB
mo=b2.modell(EMD_V7)
print('meshes',len(mo.meshes),'bones',mo.skel.bone_count,'clips',len(mo.clips), 'clut_y',mo.clut_y,'lut',mo.lut.shape,'tex',mo.texw,mo.texh)
P2=dict(clip=12,bild=48,gier=861,x=-8182,z=-21330)
out=[]
for d in (0,1,2,3,4,6):
    p=dict(P2); p['bild']=(48+d)%mo.clips[12][1]
    img,ids=G.zeichne(mo,p)
    out.append(img)
V=vorlage()
W=Image.new('RGB',(4*320,2*340))
tiles=[V]+out+[G.vorlage(2)]
for i,X in enumerate(tiles):
    W.paste(Image.fromarray(np.asarray(X).astype(np.uint8)).crop((220,130,540,470)),((i%4)*320,(i//4)*340))
W.save(S8+'/m6_p2_bilder.png')
print(mo.clips[12])
