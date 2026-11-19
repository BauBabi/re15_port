import sys, collections
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/tools')
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r4')
import rvdflut
pts,d,qs=rvdflut.begehbar(0x4050)
S=200; P=set(pts)
seen=set(); comps=[]
for p in pts:
    if p in seen: continue
    q=collections.deque([p]); seen.add(p); c=[]
    while q:
        x,z=q.popleft(); c.append((x,z))
        for a,b in ((S,0),(-S,0),(0,S),(0,-S)):
            n=(x+a,z+b)
            if n in P and n not in seen: seen.add(n); q.append(n)
    if len(c)>50: comps.append(c)
def f(v,s): return ((((v+32000)*10*s)>>20)+5)//10
for c in sorted(comps,key=lambda c:(-max(z for x,z in c),min(x for x,z in c))):
    xs=[x for x,z in c]; zs=[z for x,z in c]
    print(len(c),'x',min(xs),max(xs),'z',min(zs),max(zs))
