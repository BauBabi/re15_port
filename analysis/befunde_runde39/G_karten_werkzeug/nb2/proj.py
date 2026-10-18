# FUN_800473f8 (@0x8004741c-0x80047528) mit der Zeile ROOM1210 @0x800769B8 = (177,140,2496,2250)
def tr(v):  # C-Division Richtung 0
    return int(v/10) if v>=0 else -int(-v/10)
def px(x, ox=177, sx=2496): t=((x+32000)*10*sx)>>20; return tr(t+5)+ox
def py(z, oy=140, sy=2250): t=((z+32000)*10*sy)>>20; return -tr(t+5)+oy if False else tr(-(t+5))+oy
if __name__=='__main__':
    for nm,x in [('W-Wand @0x1BEC x0',-28908),('W-Wand innen',-27358),('O-Wand @0x1C40 x0 (innen Kammer)',-22397),('O-Wand @0x1C40 x1',-21185),
                 ('Flurwand @0x1BC8 x0',-22150),('Flurwand @0x1BC8 x1',-21090),('Kammer Mitte min',-26828),('Kammer Mitte max',-22928),
                 ('Grenze -22128',-22128),('Grenze -22127',-22127),('Abnahme -25500',-25500)]:
        print(f'{nm:34s} x {x:7d} -> px {px(x)}')
    for nm,z in [('N-Wand @0x1BF8 z0',-19898),('N-Wand @0x1BF8 z1',-17948),('S-Wand @0x1BE0 z0',-28496),('S-Wand innen',-26696),
                 ('Flurwand @0x1BC8 Ende',-20746),('O-Wand @0x1C40 Anfang',-22520),('O-Wand @0x1C40 Ende',-27560),
                 ('Kammer Mitte max z',-20440),('Kammer Mitte min z',-26240),('Durchgang z -21196',-21196),('Durchgang z -22070',-22070),('Abnahme -21700',-21700)]:
        print(f'{nm:34s} z {z:7d} -> py {py(z)}')
