import sys,os,math
sys.path.insert(0,'../../../../re15_port/tools')
import numpy as np, form_kamera as FK
exec(open('zw/tri.py',encoding='utf-8').read().split('for G2 in')[0])
def m_of(p): return Y(G1).T@(FK.strahl_auf_boden(K1,np.array(p,float))-L1)
Kp={'K1':m_of((398,374)),'K3':m_of((312,452)),'K4':m_of((352,432)),'R':m_of((335,425))}
def w(m,L2,G2): return Y(G2)@m+L2
erg=[]
for G2 in range(1000,1300,20):
  for x in range(1400,2600,50):
    for z in range(-24400,-22900,50):
      L2=np.array([x,0.,z])
      a=FK.projiziere(K2,w(Kp['K1'],L2,G2))
      h=FK.projiziere(K2,w(0.5*(Kp['K4']+Kp['K3']),L2,G2))
      q,d=tri((326,350),(338,325),L2,G2)
      e=abs(a[0]-395)*0.5+max(0,a[1]-380)*1.0+0.5*abs(h[0]-235)+0.5*abs(h[1]-418)+d/20+max(0,-q[0]-500)/40
      erg.append((e,G2,x,z,a,h,q,d))
erg.sort(key=lambda t:t[0])
seen={}
for e,G2,x,z,a,h,q,d in erg[:4000]:
    if seen.get(G2,0)>=2: continue
    seen[G2]=seen.get(G2,0)+1
    print('G2 %d L2 (%d,%d) e %.1f K1 %s Hand %s Ellb x %.0f h %.0f z %.0f Rest %.0f'%(G2,x,z,e,np.round(a),np.round(h),q[0],-q[1],q[2],d))
