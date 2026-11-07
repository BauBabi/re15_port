import sys,os,math
sys.path.insert(0,'../../../../re15_port/tools')
import numpy as np, form_kamera as FK
exec(open('zw/tri.py',encoding='utf-8').read().split('for G2 in')[0])
for G2,L2 in ((1040,(2200,0,-23900)),(1020,(2150,0,-23900)),(1100,(2050,0,-23850)),(1100,(1900,0,-23800))):
    L2=np.array(L2,float)
    print('G2',G2,'L2',L2[[0,2]])
    for nm,p1,p2 in (('Dome-Mitte',(314,253),(338,250)),('Scheitel',(311,187),(320,156)),('Dome rechts B1 <-> rand B2',(381,250),(418,250)),('Dome links B1 <-> oben',(247,233),(262,215)),('Ellbogen',(326,350),(338,325)),('Fuss FR',(398,374),(395,378)),('Schulter',(405,238),(417,251)),('Z1<->Q1',(320,240),(275,200)),('Hand',(335,420),(245,415))):
        q,d=tri(p1,p2,L2,G2)
        print('   %-28s model x %6.0f h %6.0f z %6.0f  Rest %5.0f'%(nm,q[0],-q[1],q[2],d))
