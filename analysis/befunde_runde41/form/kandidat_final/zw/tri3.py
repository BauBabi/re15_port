import sys,os,math
sys.path.insert(0,'../../../../re15_port/tools')
import numpy as np, form_kamera as FK
exec(open('zw/tri.py',encoding='utf-8').read().split('for G2 in')[0])
G2=1060;L2=np.array([1850.,0,-23550.])
print('Kamera 1 im Modell',np.round(Y(G1).T@(np.array(K1['auge'])-L1)),' Kamera 2 im Modell',np.round(Y(G2).T@(np.array(K2['auge'])-L2)))
for nm,p1,p2 in (('Dome-Mitte',(314,253),(338,250)),('Scheitel',(311,187),(320,156)),('Dome rechts B1/B2 rand',(381,250),(418,250)),('Dome links',(247,233),(262,215)),('Ellbogen',(326,350),(338,325)),('Schulter S',(405,238),(417,251)),('Z1<->Q1',(320,240),(275,200)),('Z2<->',(317,265),(262,235)),('Z3<->B10',(326,293),(280,275)),('Z3<->B11',(326,293),(258,263)),('Z5<->P',(400,285),(365,303)),('Z9<->Q4',(415,250),(411,217)),('Bauch L1',(362,322),(345,345)),('Hand',(335,420),(245,415)),('Hakenfaust vs Kugel',(226,380),(228,240))):
    q,d=tri(p1,p2,L2,G2)
    print('   %-24s model x %6.0f h %6.0f z %6.0f  Rest %5.0f'%(nm,q[0],-q[1],q[2],d))
