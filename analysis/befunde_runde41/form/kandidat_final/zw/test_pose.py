import sys,json
sys.argv=['x']
import numpy as np
import passung as PA
p=PA.start_params('passung_stand.json')
p['w2'][:]=0
for G,L in ((1040,(2200,0,-23900)),(1100,(2100,0,-23800)),(1000,(2250,0,-23950))):
    q=dict(p); q['lage2']=np.array(L,float); q['gier2']=np.array(float(G))
    PA.ansehen(q,'zw/same_%d.png'%G)
