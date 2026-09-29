import sys, os
def load(p):
    d=open(p,'rb').read()
    # P6\nW H\n255\n
    parts=d.split(b'\n',3)
    w,h=map(int,parts[1].split()); return w,h,parts[3]
a,b=sys.argv[1],sys.argv[2]
frames=sorted(f for f in os.listdir(a) if f.startswith('f_') and f.endswith('.ppm'))
tot=0
for f in frames:
    wa,ha,da=load(os.path.join(a,f)); wb,hb,db=load(os.path.join(b,f))
    n=0; bb=[10**9,10**9,-1,-1]
    for i in range(0,len(da),3):
        if da[i:i+3]!=db[i:i+3]:
            n+=1; px=(i//3)%wa; py=(i//3)//wa
            bb=[min(bb[0],px),min(bb[1],py),max(bb[2],px),max(bb[3],py)]
    tot+=n
    print(f, wa,'x',ha, 'diff px:', n, ('bbox %s'%bb) if n else '')
print('total', tot)
