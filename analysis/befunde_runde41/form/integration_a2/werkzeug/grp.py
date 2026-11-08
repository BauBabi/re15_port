import sys,re,glob
S=sys.argv[1]; tag=sys.argv[2]
L=glob.glob(S+'/wd_'+tag+'/lauf_*')[0]
st={};dk={};dn={}
for l in open(L+'/state.log',errors='replace'):
    m=re.match(r'F(\d+) pad=\S+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)',l)
    if not m: continue
    fm=re.search(r't=30 st=(\d+) ss1=(\d+) ss2=\d+ ss3=\d+ g=\w+ mo=(\d+) af=(\d+)[^@]*@\((-?\d+),(-?\d+),r(\d+)\)\] hp=(-?\d+)',l)
    st[int(m.group(1))]=(int(m.group(2)),int(m.group(3)),int(m.group(6)),fm and int(fm.group(5)),fm and int(fm.group(6)),fm and fm.group(1)+'/'+fm.group(2)+'/mo'+fm.group(3))
for l in open(L+'/deck.log'):
    m=re.match(r'F(\d+) cam=(-?\d+)(.*)',l)
    if m: dk[int(m.group(1))]=(int(m.group(2)),dict((int(a),(int(b),int(c))) for a,b,c in re.findall(r' s(\d+)=(\d+)/(\d+)',m.group(3))))
for l in open(S+'/dunkel_'+tag+'.txt'):
    m=re.match(r'F(\d+) dunkel=(\d+)',l)
    if m: dn[int(m.group(1))]=int(m.group(2))
runs=[];cur=None
for f in sorted(dn):
    if dn[f]>600:
        if cur and f<=cur[-1]+3: cur.append(f)
        else:
            cur=[f]; runs.append(cur)
print(tag,'Bilder >600:',sum(1 for f in dn if dn[f]>600),'max',max(dn.values()))
for r in runs:
    f=r[0]; s=st.get(f); d=dk.get(f)
    fs=max(((k,v) for k,v in (d[1].items() if d else []) if 0<k<19),key=lambda kv:kv[1][1],default=None)
    print(' F%d-F%d n=%d max %d cut %s Leon x %s z %s, Form x %s z %s %s, Formslot %s, Leon px %s' % (r[0],r[-1],len(r),max(dn[x] for x in r), d and d[0], s and s[0], s and s[1], s and s[3], s and s[4], s and s[5], fs, d and d[1].get(0)))
