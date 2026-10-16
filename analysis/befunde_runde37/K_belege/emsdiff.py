import struct,hashlib
exe=open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/info/Re1.5/PSX.EXE','rb').read()
t=struct.unpack_from('<I',exe,0x18)[0]
def rd(a,n): o=0x800+a-t; return exe[o:o+n]
base=r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/re15_port/shared_assets/PSX/EMD/'
E0=open(base+'CDEMD0.EMS','rb').read(); E1=open(base+'CDEMD1.EMS','rb').read()
def blob(E,tab,i):
    s,z=struct.unpack_from('<II',rd(tab+i*8,8)); return E[s*2048:s*2048+z] if z else None
def secs(b):
    do=struct.unpack_from('<I',b,0)[0]; n=(len(b)-do)//4
    d=list(struct.unpack_from('<%dI'%n,b,do)); return d+[do]
def edd(b,o):
    n=struct.unpack_from('<H',b,o+2)[0]//4
    return [struct.unpack_from('<H',b,o+j*4)[0] for j in range(n)]
for i in range(64):
    a=blob(E0,0x80072f38,i); b=blob(E1,0x80073178,i)
    if not a and not b: continue
    if a==b: print('type %#x identical'%(i+0x10)); continue
    sa=secs(a); sb=secs(b)
    parts=[]
    names=['d0','edd1','emr2','edd3','emr4','edd5','emr6','md1','tim']
    for k in range(min(len(sa),len(sb))-1):
        if k>=9: break
        pa=a[sa[k]:sa[k+1]]; pb=b[sb[k]:sb[k+1]]
        if pa!=pb:
            extra=''
            if k in (1,3,5) and len(pa)>=4 and len(pb)>=4:
                try:
                    ea=edd(a,sa[k]); eb=edd(b,sb[k])
                    diff=[(j,ea[j] if j<len(ea) else None,eb[j] if j<len(eb) else None) for j in range(max(len(ea),len(eb))) if (ea[j] if j<len(ea) else None)!=(eb[j] if j<len(eb) else None)]
                    extra=' clipdiff %s'%diff
                except Exception as e: extra=' err'
            parts.append('%s(%d/%d)%s'%(names[k],len(pa),len(pb),extra))
    print('type %#x differs: %s'%(i+0x10,'; '.join(parts)))
