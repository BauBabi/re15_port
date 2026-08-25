#!/usr/bin/env python3
"""Sucht in ALLEN RE2-Raumskripten das Fenster-/Gitter-Muster:
  A) mehrere Sce_em_set (0x44) auf DERSELBEN Linie (gleiches x oder z), in der
     anderen Achse gestaffelt
  B) aot_set (0x2C) / aot_set_4p (0x67) im selben Sub
  C) Obj_model_set (0x2D) im selben Sub
  D) der RMW-Block  work_set(3,n) / member_copy(v,1) / calc(v |= k) / member_set2(1,v)
     der be_flg-Bits im Entity setzt (Entity+0x02, Member #1)
"""
import sys,os,struct
HERE=os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0,HERE)
import re2_scd_walk as W

def em(d,a):   # sce_em_set 0x44 (22 B)  /  0x8E (24 B)
    return dict(id=d[a+2], typ=d[a+3], flag=struct.unpack_from('<H',d,a+4)[0],
                b8=d[a+8], b9=d[a+9],
                x=struct.unpack_from('<h',d,a+10)[0],
                y=struct.unpack_from('<h',d,a+12)[0],
                z=struct.unpack_from('<h',d,a+14)[0],
                dir=struct.unpack_from('<h',d,a+16)[0])

def scan():
    res=[]
    for fn,d,name,base,i,s,e,last in W.all_subs():
        ops,st=W.walk(d,s,e,last)
        if not st.startswith("ok"): continue
        ems=[(a,em(d,a)) for a,o,l in ops if o==0x44]
        e8e=[a for a,o,l in ops if o==0x8E]
        aots=[(a,o) for a,o,l in ops if o in (0x2C,0x67)]
        objs=[a for a,o,l in ops if o==0x2D]
        # D) RMW-Bloecke
        rmw=[]
        for k in range(len(ops)-3):
            seq=[ops[k+j][1] for j in range(4)]
            if seq==[0x2E,0x3D,0x26,0x35]:
                a0,a1,a2,a3=[ops[k+j][0] for j in range(4)]
                if d[a1+2]==1 and d[a3+1]==1:
                    v=struct.unpack_from('<H',d,a2+2)[0]
                    rmw.append((d[a0+1],d[a0+2],v&0xff,v>>8,struct.unpack_from('<h',d,a2+4)[0]))
        if len(ems)<2: continue
        # gemeinsame Linie?
        for axis in ('x','z'):
            other='z' if axis=='x' else 'x'
            groups={}
            for a,m in ems: groups.setdefault(m[axis],[]).append((a,m))
            for val,g in groups.items():
                if len(g)<3: continue
                oth=[m[other] for _,m in g]
                if len(set(oth))!=len(oth): continue
                res.append(dict(room=fn[4:8],file=fn,blk=name,sub=i,off=s,axis=axis,
                                line=val,others=sorted(oth),ems=g,aots=aots,objs=objs,
                                rmw=rmw,e8e=e8e))
                break
            else: continue
            break
    return res

if __name__=="__main__":
    r=scan()
    print("Subs mit >=3 Sce_em_set auf einer Linie: %d\n"%len(r))
    for x in r:
        typs=sorted(set(m['typ'] for _,m in x['ems']))
        flags=sorted(set(m['flag'] for _,m in x['ems']))
        print("%s %s sub%-3d @Datei 0x%05X  %d x EM typ=%s flag=%s"%(
            x['file'],x['blk'],x['sub'],x['off'],len(x['ems']),
            ",".join("0x%02X"%t for t in typs),",".join("0x%04X"%f for f in flags)))
        print("      Linie %s=%d, %s gestaffelt: %s"%(x['axis'],x['line'],
            'z' if x['axis']=='x' else 'x',x['others']))
        print("      aot: %s   obj_model_set: %d   0x8E: %d   RMW-Bloecke: %s"%(
            ",".join("0x%02X@0x%05X"%(o,a) for a,o in x['aots']) or "-",
            len(x['objs']),len(x['e8e']),
            ("%d x work_set(k=%d)|calc op%d var%d val%d"%(len(x['rmw']),x['rmw'][0][0],x['rmw'][0][2],x['rmw'][0][3],x['rmw'][0][4])) if x['rmw'] else "-"))
        print()
