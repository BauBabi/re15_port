import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
import v4_gemein as G, b2
mo=b2.modell(EMD_V7)
d=json.load(open(S8+'/m9_pose.json'))
ps=[d['fein'][0],d['fein'][1],d['fein'][3]]+[f for f in d['fein'] if f['clip']==12][:1]
V=vorlage().astype(np.uint8)
tiles=[V]
for p in ps:
    img,ids=G.zeichne(mo,p); tiles.append(img)
    bl=(0.5*V+0.5*img).astype(np.uint8); tiles.append(bl)
W=Image.new('RGB',(3*320,3*340))
for i,X in enumerate(tiles):
    W.paste(Image.fromarray(np.asarray(X).astype(np.uint8)).crop((220,130,540,470)),((i%3)*320,(i//3)*340))
W.save(S8+'/m9_kandidaten.png')
