# -*- coding: utf-8 -*-
"""Zensus A, Schritt 2: die AUSGELIEFERTE Maske gegen die begehbaren Standplaetze.
Reine Messung, kein Schreiben. Gibt die ABDECKUNG der Schiene mit aus."""
import json, os, struct, sys
import numpy as np
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme

CD='re15_port/shared_assets/PSX'; PPM='build/bg_ppm'
AUS=json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
DUMP='build/floor_dump.txt'

def shipped(room,cut):
    blob=open(os.path.join(CD,'MASKS',room+'.MSK'),'rb').read()
    ms=maskenbild.masken(blob,cut) or []
    idx=maskenbild.lies_tim_bytes(open(os.path.join(CD,'MASKS','%s_PRI%02d.TIM'%(room,cut)),'rb').read())[0]
    return (ms,)+abnahme.deckung_und_tiefe(ms,idx)

def objekte_vz(room,cut,rdt,cam,bg):
    """[(name, region, vzmap, y0, quelle)] wie bau_p2 sie baut."""
    import bau_p2, maske_aus_png
    rid=int(room[4:],16)
    R,t,H=geom.cut_view(rdt,cam,cut)
    e=AUS[room][str(cut)]
    baender=sorted(geom.begehbare_baender(rid))
    out=[]
    for o in e.get('objekte') or []:
        y0 = int(o['ebene']) if o.get('ebene') is not None else (-baender[0]*geom.BAND_HOEHE if len(baender)==1 else 0)
        band=int(round(-y0/float(geom.BAND_HOEHE)))
        if 'quader' in o:
            q=[int(x) for x in o['quader']]
            vz,tr=geometrie.quader_auf_band(R,t,H,q[0],q[0]+q[2],q[1],q[1]+q[3],q[4],y0)
            r=bau_p2.kaesten_anwenden(tr.copy(),o,{})
            out.append((o.get('name','?'),r,np.where(r,vz,0.0),y0,'quader'))
        elif 'png' in o:
            r=maske_aus_png.setze(o['png'],o['x'],o['y'],o.get('massstab',1),alpha_schwelle=bau_p2.ALPHA_SCHWELLE)
            if r is None: continue
            if o.get('teil'): r=bau_p2.farbteil(r,bg,o['teil'])
            r=bau_p2.kaesten_anwenden(r,o,{})
            if not r.any(): continue
            ber=[]
            vzm,info=geometrie.tiefe_geometrie(rdt,R,t,H,r,y0,band,o,None,ber)
            if vzm is None: continue
            out.append((o.get('name','?'),r,vzm,y0,'png'))
    return out

ZIELE=[('ROOM10F0',4),('ROOM10F0',5),('ROOM1100',1),('ROOM1100',2),('ROOM10D0',7)]
for room,cut in ZIELE:
    rid=int(room[4:],16)
    rdt,st=geom.load_rdt(CD,room)
    cam=struct.unpack_from('<I',rdt,0x24)[0]
    R,t,H=geom.cut_view(rdt,cam,cut)
    bg=geom.load_bg(PPM,rid,cut)
    ms,deck,tief=shipped(room,cut)
    floor=abnahme.floor_aus_dump(DUMP,rid)
    nfloor=sum(len(v) for v in floor.values())
    objs=objekte_vz(room,cut,rdt,cam,bg)
    stand,stand_y0=abnahme.standlinie([(r,v,y0) for (_,r,v,y0,_) in objs])
    sch=abnahme.standplatz_schiene(R,t,H,deck,tief,stand,floor,stand_y0=stand_y0)
    print('='*78)
    print('%s C%d  Bodenpunkte im Dump: %d (Baender %s)'%(room,cut,nfloor,sorted(floor)))
    print('  ABDECKUNG der Schiene: %d Standplaetze projiziert, %d beruehren die Maske  (%.1f %% der Bodenpunkte)'
          %(sch['plaetze'],sch['beruehrt'],100.0*sch['beruehrt']/max(1,nfloor)))
    print('  VOR   n=%4d  verdeckt=%4d  teilweise=%4d     <- verdeckt MUSS 0 sein'%(sch['VORn'],sch['VORverd'],sch['VORteil']))
    print('  HINTER n=%4d  frei=%4d'%(sch['HINTn'],sch['HINTfrei']))
    if sch['vor_rest']:
        print('  VOR-Rest (Band,x,z,Quote): %s'%', '.join('(%d,%d,%d,%.2f)'%q for q in sch['vor_rest'][:15]))
    # Zuordnung der VOR-Faelle zu EINEM Objekt: welches Objekt deckt dort?
