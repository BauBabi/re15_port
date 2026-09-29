# Pruefer 1 (Runde 31, Tueren): Messwerkzeug, kein Spielcode. Dossier analysis/befunde_runde31/tueren_04_pruefer1.md
import json, math, sys
W='C:/workspace/git/reAi_v2/.claude/worktrees/r31_tueren/'
z=json.load(open(W+'build/r31_tueren/t1/zensus.json',encoding='utf-8'))
S={s['id']:s for s in z['seiten']}
d=json.load(open(W+'analysis/befunde_runde31/tueren_03/zuordnung.json',encoding='utf-8'))
side2t={}
for t in d['tueren']:
    for s in t['seiten']: side2t[s['id']]=t
def gegen(sid):
    """Seiten, die in den Raum von sid fuehren und deren Ziel nahe der Tuer liegt"""
    s=S[sid]; raum=s['saetze'][0]['raum'][:3]
    t=side2t[sid]; out=[]
    for o in t['seiten']:
        if o['id']==sid: continue
        os_=S[o['id']]
        for zz in os_['ziele']:
            out.append((o['id'],zz))
    return out
for sid in sys.argv[1:]:
    s=S[sid]; cx,cz=s['mitte']
    print(sid, s['saetze'][0]['raum'], 'mitte',(cx,cz),'rect',s['rect'],'pts',s['pts'] if s['form']=='viereck' else '', 'band',s['band'])
    for oid,zz in gegen(sid):
        (sx,sy,sz),yaw,cut,_=zz
        dx,dz=cx-sx,cz-sz
        ry=int(round(math.atan2(-dz,dx)/(2*math.pi)*4096))%4096
        dist=math.hypot(dx,dz)
        # stand 620 vor Mitte, oder am Spawn
        px=int(cx-620*math.cos(ry*2*math.pi/4096)); pz=int(cz+620*math.sin(ry*2*math.pi/4096))
        print('   via',oid,'spawn',(sx,sy,sz),'yaw',yaw,'cut',cut,'-> ry',ry,'dist',int(dist),'| Stand620',(px,pz))
