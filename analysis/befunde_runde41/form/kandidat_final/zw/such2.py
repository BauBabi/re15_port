import sys,os,math
sys.path.insert(0,'../../../../re15_port/tools')
import numpy as np, form_kamera as FK
K1,K2=FK.lade_kamera(1),FK.lade_kamera(2)
def Y(g):
    a=g*2*math.pi/4096;c,s=math.cos(a),math.sin(a)
    return np.array([[c,0,s],[0,1,0],[-s,0,c]])
L1=np.array([-18534.,0,-18707]);G1=2152
def m_of(p): return Y(G1).T@(FK.strahl_auf_boden(K1,np.array(p,float))-L1)
K={'K1':m_of((398,374)),'K2':m_of((232,433)),'K3':m_of((312,452)),'K4':m_of((352,432)),'K5':m_of((258,395)),'R':m_of((335,425))}
for k,v in K.items(): print(k,np.round(v))
def w(m,L2,G2): return Y(G2)@m+L2
best=[]
for G2 in range(1000,1400,20):
  for x in range(1000,3000,50):
    for z in range(-24400,-22400,50):
      L2=np.array([x,0.,z])
      a=FK.projiziere(K2,w(K['K1'],L2,G2))
      b=FK.projiziere(K2,w(K['R'],L2,G2))
      # K1 soll auf (395,376), R auf Y (240,418)
      e=np.hypot(a[0]-395,a[1]-376)+np.hypot(b[0]-240,b[1]-418)
      best.append((e,G2,x,z,a,b))
best.sort(key=lambda t:t[0])
seen=set()
for e,G2,x,z,a,b in best[:400]:
    if G2 in seen: continue
    seen.add(G2)
    L2=np.array([x,0.,z])
    print('G2 %d L2 (%d,%d) e %.1f K1 %s R %s  K2 %s K3 %s K4 %s'%(G2,x,z,e,np.round(a),np.round(b),np.round(FK.projiziere(K2,w(K['K2'],L2,G2))),np.round(FK.projiziere(K2,w(K['K3'],L2,G2))),np.round(FK.projiziere(K2,w(K['K4'],L2,G2)))))
