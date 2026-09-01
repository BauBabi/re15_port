# -*- coding: utf-8 -*-
"""Schirm-Bild: Kachel-Pixel + Header-Marken um eine Stelle herum (ASCII)."""
import sys, os, re, collections, csv
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL

H = open(r're15_port\engine\src\re15_map_zones.h', encoding='utf-8').read()
blk = H.split('s_map_marks[] = {')[1].split('};')[0]
MK = [dict(zip(('pg','r','mx','my','kind','zid','zid2'), map(int, m.groups())))
      for m in re.finditer(r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', blk)]

def schirm(pg, x0, y0, x1, y1, titel=''):
    """Alle Rechtecke der Seite in den Schirm-Puffer malen, dann Marken drauf."""
    pix = page_pix(pg)
    W, Hh = x1-x0+1, y1-y0+1
    buf = [[' ']*W for _ in range(Hh)]
    for ri, (RX, RY, RW, RH) in enumerate(rects(pg)):
        U, V = rect_uv(pg, ri)
        for j in range(RH):
            for i in range(RW):
                sx, sy = RX+i, RY+j
                if not (x0 <= sx <= x1 and y0 <= sy <= y1): continue
                yy, xx = V+j, U+i
                if not (0 <= yy < 256 and 0 <= xx < 256): continue
                v = pix[yy][xx]
                if v: buf[sy-y0][sx-x0] = '#' if v == 4 else '.'
    for k in MK:
        if k['pg'] != pg: continue
        if x0 <= k['mx'] <= x1 and y0 <= k['my'] <= y1:
            buf[k['my']-y0][k['mx']-x0] = 'M' if k['kind'] <= 3 else 'T'
    print(f"\n--- Seite {pg}  x {x0}..{x1}  y {y0}..{y1}   {titel}")
    print('     ' + ''.join(str((x0+i)//100%10) for i in range(W)))
    print('     ' + ''.join(str((x0+i)//10%10) for i in range(W)))
    print('     ' + ''.join(str((x0+i)%10) for i in range(W)))
    for j in range(Hh):
        print(f"{y0+j:4d} " + ''.join(buf[j]))
    print("     '#' = Palettenindex 4 (gemalte Linie/Tuerblatt), '.' = sonstige Kachel,")
    print("     'M' = Port-Tuermarke, 'T' = Port-Treppenmarke")

schirm(4, 150, 140, 178, 165, "ROOM1130 r4 <-> ROOM1140 r6, Durchgang (160,149)/(160,150)")
schirm(4, 138, 78, 176, 100, "ROOM1130 r4 <-> ROOM1150 r2 (144,87)/(145,85) und ->1170 (168,89)")
schirm(4, 142, 116, 168, 134, "ROOM1130 r4 <-> ROOM1120 r5 (152,125)/(120,149)")
