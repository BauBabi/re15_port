import sys,os,math
sys.path.insert(0,'../../../../re15_port/tools')
import numpy as np, form_kamera as FK
K1,K2=FK.lade_kamera(1),FK.lade_kamera(2)
def Y(g):
    a=g*2*math.pi/4096;c,s=math.cos(a),math.sin(a)
    return np.array([[c,0,s],[0,1,0],[-s,0,c]])
L1=np.array([-18534.,0,-18707]);G1=2152
def strahl_model(K,L,G,p):
    e,d=FK.strahl(K,np.array(p,float))
    Yt=Y(G).T
    return Yt@(np.array(e)-L), Yt@d
def tri(p1,p2,L2,G2):
    a,da=strahl_model(K1,L1,G1,p1); b,db=strahl_model(K2,L2,G2,p2)
    # kuerzester Abstand zweier Strahlen
    w=a-b; A=np.array([[da@da,-da@db],[da@db,-db@db]]); r=-np.array([w@da,w@db])
    s,t=np.linalg.solve(A,r)
    q1=a+s*da;q2=b+t*db
    return (q1+q2)/2,np.linalg.norm(q1-q2)
for G2 in (1040,1081,1140,1200,1260,1336):
  for L2 in ((2000.,0,-23300),(1800.,0,-23400),(2200.,0,-23200)):
    L2=np.array(L2)
    print('G2',G2,'L2',L2[[0,2]])
    for nm,p1,p2 in (('Ellbogen',(326,350),(338,325)),('Mitte',(322,372),(285,360)),('Hand',(335,420),(245,415)),('Hand2',(330,440),(230,420)),('FussFR',(398,374),(395,378)),('Kuppe S',(405,238),(417,251))):
        q,d=tri(p1,p2,L2,G2)
        print('   %-8s model x %6.0f h %6.0f z %6.0f  Rest %5.0f'%(nm,q[0],-q[1],q[2],d))
