# -*- coding: utf-8 -*-
import sys, os, numpy as np, cv2
sys.path.insert(0,'../../../../re15_port/tools')
import form_pruefung as P
k=P.Kandidat('form.json')
for n in (1,2):
    s,a,m,lk=k.rendern(n)
    o=P.original(n)
    g=P.FV.video_filter(s,bild=n,entzerren=False,rauschen=False)
    M=m.copy()
    # nur Grundhaut: Albedo dunkel
    b=(a.sum(2)<160)&M
    print('Bild',n,'Pixel',b.sum())
    print(' Albedo',a[b].mean(0).round(1),' Render',s[b].mean(0).round(1),' Faktor',(s[b].mean(0)/np.maximum(a[b].mean(0),1)).round(3), 'p10/50/90 R',np.percentile(s[b][:,0],[10,50,90]))
    print(' Filter',g[b].mean(0).round(1),' Original',o[b].mean(0).round(1))
    F=(s[...,0].astype(float)/np.maximum(a[...,0],1))
    F[~b]=0
    cv2.imwrite('lauf/faktor%d.png'%n,np.clip(F*160,0,255).astype(np.uint8))
