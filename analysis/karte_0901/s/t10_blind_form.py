import sys,time,statistics,math,json; sys.path.insert(0,r'analysis\karte_0901\s')
from base import *
from reg2 import *
print('BLINDTEST Formregistrierung: Zeile IGNORIERT, nur RDT-SCA + MAP-Kachel')
print('%-7s %-6s %6s %6s | %5s %5s %6s %6s | %5s %5s %6s %6s | %7s %7s'%(
  'Raum.Z','pg/rc','IoU_w','IoU_s','ox_w','oy_w','sx_w','sy_w','ox_s','oy_s','sx_s','sy_s','dMed','dMax'))
res=[]
for r in ECHT:
    ks=[k for k in zonen(r) if ZONES[k]['sx']>1] or [zonen(r)[0]]
    k=ks[0]; z=ZONES[k]
    B,R=bmask(z['page'],z['rect'],True)
    t0=time.time()
    iou_s,ox,oy,sx,sy=suche(ZCELLS[k],B,R)
    w=zeile(r)[2]
    Aw,x0,y0=raster0(ZCELLS[k],w[2],w[3])
    # IoU der WAHREN Zeile
    Aset=set(); 
    from reg import flaeche,iou,maske
    Bs,_=maske(z['page'],z['rect'],True)
    iou_w=iou(flaeche(ZCELLS[k],*w),Bs)
    P=pts_of(k,4)
    dd=[math.hypot(proj(a,c,ox,oy,sx,sy)[0]-proj(a,c,*w)[0], proj(a,c,ox,oy,sx,sy)[1]-proj(a,c,*w)[1]) for a,c in P]
    res.append((r,iou_w,iou_s,(ox,oy,sx,sy),w,statistics.median(dd),max(dd),time.time()-t0))
    print('%04X.%d  %2d/%-2d %6.3f %6.3f | %5d %5d %6d %6d | %5d %5d %6d %6d | %7.1f %7.1f  (%.0fs)'%(
      r,k[1],z['page'],z['rect'],iou_w,iou_s,w[0],w[1],w[2],w[3],ox,oy,sx,sy,statistics.median(dd),max(dd),time.time()-t0))
    sys.stdout.flush()
json.dump([(a,b,c,d,e,f,g) for (a,b,c,d,e,f,g,h) in res],open(r'analysis\karte_0901\s\blind_form.json','w'))
med=[x[5] for x in res]; mx=[x[6] for x in res]
print()
print('Median der Median-Abweichung: %.1f px   Median der Max-Abweichung: %.1f px   groesster Max: %.1f px'%(
  statistics.median(med),statistics.median(mx),max(mx)))
print('Suche schlaegt die WAHRE Zeile im IoU in %d von %d Faellen'%(sum(1 for x in res if x[2]>x[1]),len(res)))
