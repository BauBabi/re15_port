import re, struct, sys, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r34g_c/analysis/befunde_runde34_granaten/port_inventar_werkzeug')
import esp_rows as E
core = open(E.P,'rb').read()
effs = {e['id']: e for e in E.parse(core)}
def coord(eid, i):
    e = effs[eid]; off = e['start'] + 8 + e['ca']*8 + i*4
    u,v,dx,dy = struct.unpack_from('<BBbb', core, off); return u,v,dx,dy
def parse(p):
    rows=[]; cur=None
    for line in open(p):
        if line.startswith('id='):
            m=re.search(r'id=(\d+) sub=(\d+) eidx=(-?\d+) frame=(\d+).*slot=(\d+).* fl=([0-9a-f]+) .*F=(\d+)',line)
            cur=dict(id=int(m[1]),sub=int(m[2]),frame=int(m[4]),slot=int(m[5]),fl=m[6],F=int(m[7]))
        elif line.strip().startswith('->') and cur:
            m=re.search(r'sx=(-?\d+) sy=(-?\d+) S=(\d+) n=(\d+) c0=(\d+) w16=(-?\d+)',line)
            cur.update(sx=int(m[1]),sy=int(m[2]),S=int(m[3]),n=int(m[4]),c0=int(m[5]),w16=int(m[6]))
            rows.append(cur); cur=None
    return rows
def quads(r):
    out=[]
    S=r['S']; w16=r['w16']; step=w16//S if S else 0
    for q in range(r['n']):
        u,v,dx,dy = coord(r['id'], r['c0']+q)
        x0 = r['sx'] + ((dx*step)>>16); y0 = r['sy'] + ((dy*step)>>16)
        x1 = r['sx'] + ((dx*step + w16)>>16); y1 = r['sy'] + ((dy*step + w16)>>16)
        out.append((x0,y0,x1,y1,q))
    return out
def load(p):
    d=open(p,'rb').read(); parts=d.split(b'\n',3); w,h=map(int,parts[1].split()); return w,h,parts[3]
A,B = sys.argv[1], sys.argv[2]
ra, rb = parse(A+'/fx.log'), parse(B+'/fx.log')
for f in sorted(os.listdir(A)):
    if not f.startswith('f_'): continue
    F=int(f[2:8]); w,h,da=load(A+'/'+f); _,_,db=load(B+'/'+f)
    diffs=[((i//3)%w,(i//3)//w) for i in range(0,len(da),3) if da[i:i+3]!=db[i:i+3]]
    if not diffs: continue
    att={}
    for (x,y) in diffs:
        hits=[]
        for tag,rows in (('alt',ra),('neu',rb)):
            for r in rows:
                if r['F']!=F: continue
                for (x0,y0,x1,y1,q) in quads(r):
                    if x0<=x<x1 and y0<=y<y1: hits.append('%s:id%d.%d.q%d'%(tag,r['id'],r['sub'],q))
        key=' '.join(sorted(set(h.split(':')[1] for h in hits))) or 'KEIN QUAD'
        att[key]=att.get(key,0)+1
    print('F%d %d px: %s'%(F,len(diffs),'; '.join('%s=%d'%(k,v) for k,v in att.items())))
