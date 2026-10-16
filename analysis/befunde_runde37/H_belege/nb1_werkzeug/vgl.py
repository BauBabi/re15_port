import re, sys
def besuche(p):
    out=[]; cur=None; last=-1
    for l in open(p, errors='replace'):
        m=re.match(r'^F(\d+) ', l)
        if not m: continue
        f=int(m.group(1))
        if cur is None or f<last: cur=[]; out.append(cur)
        last=f; cur.append(l.rstrip('\n'))
    return out
a=besuche(sys.argv[1]); b=besuche(sys.argv[2])
for i,(x,y) in enumerate(zip(a,b)):
    n=min(len(x),len(y)); diff=[k for k in range(n) if x[k]!=y[k]]
    print(f"Besuch {i}: Zeilen {len(x)}/{len(y)}, abweichend {len(diff)}" + (f", erste F-Zeile {x[diff[0]][:60]}" if diff else ", Zeile fuer Zeile gleich"))
