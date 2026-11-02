# -*- coding: utf-8 -*-
"""Wohin zeigen die Knochenachsen je Pose im Bild? Hilfe beim Bau."""
import numpy as np, sys
import figur, schau
from schau import FK
f=figur.bauen(); fig=figur.fertig(f)
for n in (1,2):
    kam=FK.lade_kamera(n); p=fig['posen']['bild%d'%n]
    R,T=schau.fk(fig['eltern'],fig['versatz'],p['wurzel'],p['winkel'])
    Y=schau.ry(p['gier']*schau.E); la=np.array(p['lage'],float)
    print('Bild',n,'Kamera-vorn',kam['R'][2].round(2))
    for i,nm in enumerate(f.namen):
        o=Y@T[i]+la
        b=FK.projiziere(kam,o)
        d=FK.tiefe(kam,o)
        vk=[]
        for a,na in ((0,'x'),(1,'y'),(2,'z')):
            e=np.zeros(3); e[a]=300
            q=Y@(T[i]+R[i]@e)+la
            bq=FK.projiziere(kam,q)
            dz=FK.tiefe(kam,q)-d
            vk.append('%s(%+.0f,%+.0f,%+.0f)'%(na,*(bq-b),dz))
        print('%2d %-16s (%4.0f,%4.0f) T %5.0f h %5.0f  %s'%(i,nm,b[0],b[1],d,-o[1],' '.join(vk)))
