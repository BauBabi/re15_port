import re,sys,os
# fest_nach.py <dir...>: wie F_abn3/fest_split.py (Abnahme 3): fest = aktiv, kein Angriff (Sub 5/6/8/15), >= 90 Bilder im
# Anker 400 (euklidisch), Leon > 3000; je Bild fern (d >= 6001 = 0x1771) oder nah. Zusaetzlich: nur Bilder NACH dem ersten
# Angriff irgendeines Gorillas ("nach dem ersten Angriff", Abnahme 3 Mangel 1).
pat=re.compile(r"\[(\d+) t=27 st=(\d+)/(\d+)/(\d+)/(\d+) g=(\w+) mo=(-?\d+) af=(-?\d+) @\((-?\d+),(-?\d+),(-?\d+)\)")
def ausw(fn, room="11C0"):
    rows=[]
    for line in open(fn):
        m0=re.match(r"F(\d+) R(\w+) PL\((-?\d+),(-?\d+),(-?\d+)",line)
        if not m0: continue
        if m0.group(2)!=room:
            if rows: break
            continue
        g={}
        for m in pat.finditer(line): g[int(m.group(1))]=(int(m.group(3)),int(m.group(9)),int(m.group(11)))
        rows.append((int(m0.group(1)),int(m0.group(3)),int(m0.group(5)),g))
    erst={}
    for (f,px,pz,g) in rows:
        for s,(sub,x,z) in g.items():
            if sub in (5,6,8,15) and s not in erst: erst[s]=f
    e0=min(erst.values()) if erst else None
    res={}
    for s in (2,3):
        akt=None; fest=0; fern=0; nfest=0; nfern=0; anker=None; a0=None; seg=[]
        def schliessen(endf):
            nonlocal fest,fern,nfest,nfern
            if anker is not None and endf-a0>=90:
                fest+=endf-a0; fern+=sum(seg)
                if e0 is not None:
                    nach=[fl for (ff,fl) in zip(range(a0,endf),seg) if ff>e0]
                    nfest+=len(nach); nfern+=sum(nach)
        for (f,px,pz,g) in rows:
            if s not in g: continue
            sub,x,z=g[s]; d=((x-px)**2+(z-pz)**2)**0.5
            if akt is None and sub not in (0,1): akt=f
            ok = akt is not None and sub not in (5,6,8,15) and d>3000
            if ok and anker is not None and ((x-anker[0])**2+(z-anker[1])**2)**0.5 < 400:
                seg.append(1 if d>=6001 else 0); continue
            schliessen(f)
            anker=(x,z) if ok else None; a0=f; seg=[1 if (ok and d>=6001) else 0] if ok else []
        if rows: schliessen(rows[-1][0]+1)
        res[s]=(erst.get(s),fest,fern,nfest,nfern)
    return res,e0,(rows[-1][0] if rows else None)
for d in sys.argv[1:]:
    r,e0,lf=ausw(os.path.join(d,"gy.log"))
    print("%-38s erster Angriff F%s | %s | letztes F%s"%(os.path.basename(d),e0," | ".join("S%d Angriff F%s fest %d (fern %d) nach %d (fern %d)"%(s,v[0],v[1],v[2],v[3],v[4]) for s,v in sorted(r.items())),lf))
