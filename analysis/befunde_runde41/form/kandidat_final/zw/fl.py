import sys,os,math
sys.path.insert(0,'../../../../re15_port/tools')
import numpy as np, form_kamera as FK
exec(open('zw/tri.py',encoding='utf-8').read().split('for G2 in')[0])
G2=1060;L2=np.array([1850.,0,-23550.])
for nm,m in (('FL spiegel',(-760,0,-1852)),('FL spiegel oben',(-760,-1000,-1852)),('FL formblatt',(-1141,0,-610)),('FL b',(-1000,0,-1200)),('FL c',(-900,0,-1500)),('D',(-727,0,-522))):
    m=np.array(m,float)
    print(nm,'B1',np.round(FK.projiziere(K1,Y(G1)@m+L1)),'B2',np.round(FK.projiziere(K2,Y(G2)@m+L2)))
