import sys,struct,os
R=r'C:/workspace/git/reAi_v2'
sys.path.insert(0,R+'/re15_port/tools')
import rdt_objekt_ansicht as A
import numpy as np
from PIL import Image
for nm,p in (('disc',R+'/Version_40%/PSX/STAGE2/ROOM2090.RDT'),):
    d=open(p,'rb').read()
    pr=A.props_lesen(d)
    for i,(t,m) in enumerate(pr):
        px,pal,cy=A.tim_decode(t)
        print(i,px.shape,pal.shape)
        im=pal[0][px]
        Image.fromarray(im.astype('uint8')).save(f'../alligator_belege/{nm}_room2090_obj{i}_tim.png')
    modelle=[]
    for t,m in pr[:1]:
        modelle.append((0,A.PropModell(t,m)))
    try:
        A.blatt('ROOM2090 DISC',modelle,'../alligator_belege/disc_room2090_obj0_blatt.png')
    except Exception as ex:
        print('blatt',ex)
