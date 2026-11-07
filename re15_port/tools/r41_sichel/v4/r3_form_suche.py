"""SF V4 Runde 3 (Bauer): Schwanzgelenke 2-4, Durchmesser 2-4 und Tiefe 2-3 an die T2-Drehung nach dem EINEN Messverfahren
(t2_mass_v4.py, Entscheid O2: Pruefer-Mittellinie, Original 65,8 / 69,5 / 70,1 Grad, Ziel 68,4 +-5) nachstellen, ohne T1-T6 und TA
zu verletzen (Marge 0,003 H). (1+1)-Evolutionsstrategie mit Schrittweitenanpassung (Erfolg x1,3, Misserfolg x0,93).

    python r3_form_suche.py <saat> <schritte>     -> je Verbesserung eine Zeile "it N dreh D pen P {json}" auf stdout

Die Suche maximiert die Drehung; uebernommen wird ein Stand im Zielband (Dossier V4 Runde 3: Lauf saat 1, Schritt 78, 70,6 Grad).
"""
import sys, os, json, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import r2_expt as X, t2_mass_v4 as T, v4_gemein as G, b2, birkin_formen as BF


def viol(s):
    v=[]
    H=s['H']
    t2=s['T2']; v+= [abs(t2[0]+0.245)-0.03, abs(t2[1]-0.036)-0.03]
    t3=s['T3']; v+= [abs(t3[0]+0.265)-0.03, abs(t3[1]-0.257)-0.03]
    t4=s['T4']; v+= [abs(t4[0]+0.245)-0.03, abs(t4[1]-0.397)-0.03, abs(s['ende_ueber_sohle']-0.37)-0.03]
    t1=s['T1']; v+= [abs(t1[0]-0.15)-0.03, abs(t1[1]-0.767)-0.03]
    for nm,p,q in (('b',(357,243),s['beginn']),('k',(290,308),s['knick']),('e',(295,343),s['ende'])):
        v+= [abs(q[0]-p[0])/H-0.03, abs(q[1]-p[1])/H-0.03]
    f=s['T5']
    for k,(z,t) in {0.1:(0.03,0.015),0.3:(0.065,0.02),0.5:(0.125,0.025),0.7:(0.14,0.025)}.items(): v.append(abs(f[k]-z)-t)
    reihe=[f[k] for k in sorted(f) if 0.1<=k<=0.7+1e-9]; v.append(max([reihe[i]-reihe[i+1] for i in range(len(reihe)-1)]+[0])-0.01)
    v+= [0.025-s['end_breite'], s['end_breite']-0.05, 0.12-s['end_laenge'], s['end_laenge']-0.17, 75-s['end_richtung'] if False else 0, s['taille']-0.035, s['taille']-s['schmal_vor_knick']]
    v+= [(75-s['end_richtung'])/100, (s['end_richtung']-95)/100]
    return np.array(v)

base=copy.deepcopy(BF.SCHWANZ_V4)
def bau(x):
    g=np.array(base['gelenke'],float); d=np.array(base['durchmesser'],float); t=list(base['tiefe'])
    g[2]=x[0:2]; g[3]=x[2:4]; g[4]=x[4:6]; d[2],d[3],d[4]=x[6:9]; t[2],t[3]=x[9],x[10]
    return dict(gelenke=g.round(4).tolist(), durchmesser=d.round(4).tolist(), tiefe=[round(v,4) for v in t])
def wert(x):
    sw=bau(x); BF.SCHWANZ_V4.update(copy.deepcopy(base))
    blob=X.bauen(sw); mo=b2.modell(blob)
    from riegel_v4 import schwanz
    Z,s,(img,ids,V,fr,rm)=schwanz(mo,G.posen())
    r=T.port(ids,s['H'])
    v=viol(s); pen=1e4*np.sum(np.maximum(0,v+0.003)**2)
    return r['drehung'], pen, sw
if __name__ != '__main__':
    raise SystemExit
rng=np.random.default_rng(int(sys.argv[1]) if len(sys.argv)>1 else 1)
g=np.array(base['gelenke'],float); d=base['durchmesser']; t=base['tiefe']
x=np.r_[g[2],g[3],g[4],d[2],d[3],d[4],t[2],t[3]]
sd=np.r_[[0.012]*6,[0.012]*3,[0.03]*2]
dd,pen,sw=wert(x); f=-dd+pen*5
print('start',dd,pen,flush=True)
for it in range(int(sys.argv[2]) if len(sys.argv)>2 else 250):
    y=x+rng.normal(0,1,len(x))*sd
    y[9:11]=np.clip(y[9:11],0,0.15)
    try: d2,p2,sw2=wert(y)
    except Exception as e: continue
    f2=-d2+p2*5
    if f2<f:
        x,f,dd,pen,sw=y,f2,d2,p2,sw2; sd*=1.3
        print('it %d dreh %.1f pen %.3f %s'%(it,dd,pen,json.dumps(sw)),flush=True)
    else: sd*=0.93
    sd=np.maximum(sd,0.002)
