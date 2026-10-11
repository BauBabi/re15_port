import re,sys
# gyspur.py <gy.log> [schritt] [von] [bis]: je Schritt Leon + Gorillas (Slot, st, Lage, d, los)
pat=re.compile(r"\[(\d+) t=27 st=(\d+)/(\d+)/(\d+)/(\d+) g=(\w+) mo=(-?\d+) af=(-?\d+) @\((-?\d+),(-?\d+),(-?\d+)\)[^\]]*rot=(-?\d+) los=(\d+)")
fn=sys.argv[1]; st=int(sys.argv[2]) if len(sys.argv)>2 else 30
a=int(sys.argv[3]) if len(sys.argv)>3 else 0; b=int(sys.argv[4]) if len(sys.argv)>4 else 10**9
for line in open(fn):
    m0=re.match(r"F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+)",line)
    if not m0: continue
    f=int(m0.group(1))
    if f<a or f>b or f%st: continue
    px,pz=int(m0.group(3)),int(m0.group(5))
    out="F%d R%s L(%d,%d)"%(f,m0.group(2),px,pz)
    for m in pat.finditer(line):
        s=int(m.group(1)); x=int(m.group(9)); z=int(m.group(11))
        d=int(((x-px)**2+(z-pz)**2)**0.5)
        out+=" | S%d %s/%s/%s/%s mo%s (%d,%d) d%d r%s los%s"%(s,m.group(2),m.group(3),m.group(4),m.group(5),m.group(7),x,z,d,m.group(12),m.group(13))
    print(out)
