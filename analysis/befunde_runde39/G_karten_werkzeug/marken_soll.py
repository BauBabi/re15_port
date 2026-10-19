"""Marken-Verschiebungen laut Nutzer-Soll (Tuer-/Treppen-Ops) im Op-Modell."""
import copy
# (seite): Liste (art, alt_mx, alt_my, neu_mx, neu_my)  art: 'tw' Tuer waagerecht, 'ts' Tuer senkrecht,
#          'tr4' Treppe waagerechte Sprossen, 'tr5' senkrechte Sprossen
VERSCHIEB={
 2:[('ts',120,151,119,151),('tr4',127,144,122,144),('tr4',135,146,133,146),('tr4',187,100,183,100),('tr4',196,99,194,99)],
 3:[('ts',188,118,186,118),('tr4',126,144,121,144),('tr4',134,146,132,146)],
 4:[('tw',168,89,166,89),('tr5',168,106,168,107)],
 1:[('ts',187,74,186,74),('tw',165,139,165,138),('tr4',177,115,173,115),('tr4',186,112,183,112)],
}
TUER=(224,168,40); TREPPE=(240,240,216)
def mark_ops_von(o):
    return
def verschiebe(ops, seite):
    ops=copy.deepcopy(ops); n=0
    for art,ax,ay,nx,ny in VERSCHIEB.get(seite,[]):
        dx,dy=nx-ax,ny-ay
        treffer=[]
        for o in ops:
            if o['kind']!=4: continue
            col=(o['r'],o['g'],o['b'])
            if art=='ts' and col==TUER and o['w']==1 and o['h']==5 and o['x']==ax and o['y']==ay-2: treffer.append(o)
            if art=='tw' and col==TUER and o['w']==5 and o['h']==1 and o['x']==ax-2 and o['y']==ay: treffer.append(o)
            if art=='tr4' and col==TREPPE and o['w']==5 and o['h']==1 and o['x']==ax-2 and o['y'] in (ay-2,ay,ay+2): treffer.append(o)
            if art=='tr5' and col==TREPPE and o['w']==1 and o['h']==5 and o['y']==ay-2 and o['x'] in (ax-2,ax,ax+2): treffer.append(o)
        erw={'ts':1,'tw':1,'tr4':3,'tr5':3}[art]
        assert len(treffer)==erw, (seite,art,ax,ay,len(treffer))
        for o in treffer: o['x']+=dx; o['y']+=dy; n+=1
    return ops
