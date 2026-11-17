"""Texel an der Grenze Bauch/Flanke je Stand (roh, ohne Spiellicht): welche Texel-Spalten die Ecken auf der Fleischgrenze +-theta_b benutzen,
und deren Farbe (L*, C*, Farbton h) aus der CLUT-Zeile der Flaeche."""
import sys, json, collections, numpy as np
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0]=[T+'/v8',T+'/v6kopf',T+'/v4']
import kopf_orig_bau as KB, riegel_schwanz_r8 as RS, schwanz_r8 as Q, v4_gemein as G
phi={int(k):float(v) for k,v in json.load(open(T+'/v8/unterseite_v8.json'))['innen_phi'].items()}
def lab_row(t,z):
    c=t['clut'][z-t['cy']].astype(int)
    return G.lab(np.stack([(c&31)*255/31.,((c>>5)&31)*255/31.,((c>>10)&31)*255/31.],-1)[None])[0]
alt16=RS._alt16(RS.eingang())
for n in sys.argv[2:]:
    blob=open(sys.argv[1]+'/%s.EMD'%n,'rb').read(); _,_,ms,tim=KB.teile(blob); t=KB.tim_teile(tim)
    sp=collections.defaultdict(list)   # (seite der Grenze, klasse) -> Texel (z, seite, u, v)
    for b in range(16,23):
        m=ms[b]; P=np.array([v[:3] for v in m['v']],float); eh,eq=Q.achsen(phi[b]); th,gr=RS.ring_winkel(b,P,eh,eq)
        bm=RS.bauch_flaechen(b,m,alt16)
        # Ecken auf der Grenze: |theta| innerhalb 2,5 Grad der Grenze; Vorzeichen aus der Querkomponente
        for key,F,U in (('d','tf','tuv'),('v','qf','quv')):
            for i,(f,uv) in enumerate(zip(m[F],m[U])):
                e=list(f[1::2]); k='%s%d'%(key,i)
                if b==16 and tuple(e) in alt16: continue
                kl='Bauch' if k in bm else 'Flanke'
                z,s_=(uv[2]>>6)&0x1FF,uv[5]&0xF
                for j,q in enumerate(e):
                    if q in th and abs(th[q]-gr[q])<=RS.TOL_GRAD:
                        c=(P[q]@eq)
                        seite='+tb' if c>0 else '-tb'
                        sp[(seite,kl)].append((z,s_,uv[3*j],uv[3*j+1]))
    print('== %s' % n)
    for key in sorted(sp):
        L=[]; cols=collections.Counter()
        for z,s_,u,v in sp[key]:
            cols[(z,s_,u)]+=1
        for (z,s_,u),cnt in sorted(cols.items()):
            vs=[v for (zz,ss,uu,v) in sp[key] if (zz,ss,uu)==(z,s_,u)]
            lab=lab_row(t,z)[t['pix'][min(vs):max(vs)+1, s_*128+u]]
            C=np.hypot(lab[:,1],lab[:,2]); h=np.degrees(np.arctan2(lab[:,2],lab[:,1]))%360
            print('  %-4s %-6s Zeile %d Seite %d Spalte u %3d (v %d-%d, %3d Ecken): L* min %4.1f Median %4.1f, C* %4.1f, h %3.0f' % (
                key[0],key[1],z,s_,u,min(vs),max(vs),cnt,lab[:,0].min(),np.median(lab[:,0]),np.median(C),np.median(h)))
