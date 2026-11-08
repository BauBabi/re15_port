import sys,re,glob
def load(tag):
    L=glob.glob(sys.argv[1]+'/wd_'+tag+'/lauf_*')[0]
    st={};dk={};dn={}
    for l in open(L+'/state.log',errors='replace'):
        m=re.match(r'F(\d+) pad=\S+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)',l)
        if not m: continue
        f=int(m.group(1))
        fm=re.search(r't=30 st=(\d+) ss1=(\d+) ss2=\d+ ss3=\d+ g=\w+ mo=(\d+) af=(\d+)[^@]*@\((-?\d+),(-?\d+),r(\d+)\)\] hp=(-?\d+)',l)
        st[f]=(m.group(2),m.group(3),m.group(6))+((fm.group(5),fm.group(6),fm.group(1),fm.group(3),fm.group(8)) if fm else ())
    for l in open(L+'/deck.log'):
        m=re.match(r'F(\d+) cam=(-?\d+)(.*)',l)
        if m:
            f=int(m.group(1)); s11=re.search(r' s11=(\d+)/(\d+)',m.group(3)); s0=re.search(r' s0=(\d+)/(\d+)',m.group(3))
            dk[f]=(m.group(2), s11.groups() if s11 else None, s0.groups() if s0 else None)
    for l in open(sys.argv[1]+'/dunkel_'+tag+'.txt'):
        m=re.match(r'F(\d+) dunkel=(\d+)',l)
        if m: dn[int(m.group(1))]=int(m.group(2))
    return st,dk,dn
a=load(sys.argv[2]); b=load(sys.argv[3])
f0,f1=int(sys.argv[4]),int(sys.argv[5]); step=int(sys.argv[6])
diff=0
for f in range(f0,f1+1):
    if a[0].get(f)!=b[0].get(f) or (a[1].get(f) or ('',))[0]!=(b[1].get(f) or ('',))[0]: diff+=1
    if (f-f0)%step==0 or (a[2].get(f,0)>600):
        print(f,'st',a[0].get(f),'cam',a[1].get(f),'dn',a[2].get(f),'| K4 cam',(b[1].get(f) or ('',))[0],'dn',b[2].get(f))
print('Bilder mit abweichendem Zustand/Schnitt',diff)
