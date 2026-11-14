import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
from gitter import gitter
dE=np.load(S8+'/m4_dET.npy')
V=vorlage()
gitter(V,(255,170,395,345),5,S8+'/g_schwanz_x5.png',gamma=0.5)
d3=np.repeat(np.clip(dE*10,0,255)[...,None],3,2)
gitter(d3,(255,170,395,345),5,S8+'/g_schwanz_dE_x5.png')
