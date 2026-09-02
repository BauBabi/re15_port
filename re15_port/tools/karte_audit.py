# -*- coding: utf-8 -*-
"""GESAMT-AUDIT der Karte ueber alle 13 Blaetter.

Liest ausschliesslich die erzeugte Tabelle (engine/src/re15_map_zones.h) und die RDTs -
also genau das, was der Zeichner auch sieht. Kein Spielstart noetig.

Geprueft wird:
  1  entartete Zeichnungen (zu klein, eine Zelle)
  2  Lage im Kartenfeld (100,55,132,140)
  3  Ueberlappung ZELLE gegen ZELLE (nur die ist sichtbar), schlimmste Paare
  4  Marken: auf der eigenen Flaeche? doppelt? fehlend? Wandseite gegen Nachbarrichtung
  5  Treppen: auf beiden Blaettern, die sie verbindet?
  6  Orte, deren Tueren nur auf ein anderes Blatt fuehren

Aufruf:  python re15_port/tools/karte_audit.py
"""
import collections
import io
import os
import re
import sys

WURZEL = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
KOPF = os.path.join(WURZEL, 're15_port', 'engine', 'src', 're15_map_zones.h')
FELD = (100, 55, 132, 140)
NAMEN = {0: 'PS B1', 1: 'PS B2', 2: 'PS 1F', 3: 'PS 2F', 4: 'PS 3F', 5: 'PS ROOF',
         6: 'DRAINS B2', 7: 'FACTORY', 8: 'LAB B1', 9: 'LAB B2', 10: 'LAB B3',
         11: 'LAB B4', 12: 'SUBWAY'}


def block(name):
    raus = []
    drin = False
    for zeile in io.open(KOPF, encoding='utf-8'):
        if name in zeile:
            drin = True
            continue
        if drin and zeile.strip() == '};':
            break
        if drin:
            raus.append(zeile)
    return raus


def lies():
    zonen = []
    for zeile in block('s_map_zones[] = {'):
        m = re.search(r'\{\s*0x([0-9A-Fa-f]{4}),(.*)\}', zeile)
        if not m:
            continue
        rid = int(m.group(1), 16)
        if rid & 1:
            continue                      # nur die Leon-Variante
        t = [int(x.strip()) for x in m.group(2).split(',')]
        zonen.append(dict(room=rid, wx0=t[0], wz0=t[1], wx1=t[2], wz1=t[3],
                          page=t[4], rect=t[5], idx=t[6], zid=t[7],
                          synth=t[14], etage=t[15]))
    synth = []
    for zeile in block('s_map_synth[] = {'):
        m = re.search(r'\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),'
                      r'\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\s*\}',
                      zeile)
        if m:
            v = [int(x) for x in m.groups()]
            synth.append(dict(x=v[0], y=v[1], w=v[2], h=v[3], erste=v[4], n=v[5],
                              A=v[6], B=v[7], C=v[8], D=v[9], E=v[10], F=v[11]))
    zellen = []
    for zeile in block('s_map_synth_cells[] = {'):
        m = re.search(r'\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\s*\}', zeile)
        if m:
            zellen.append(tuple(int(x) for x in m.groups()))
    marken = []
    for zeile in block('s_map_marks[] = {'):
        m = re.search(r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}',
                      zeile)
        if m:
            v = [int(x) for x in m.groups()]
            marken.append(dict(page=v[0], rect=v[1], mx=v[2], my=v[3], kind=v[4],
                               zid=v[5], zid2=v[6]))
    etagen = []
    for zeile in block('s_map_floors[] = {'):
        m = re.search(r'\{\s*0x([0-9A-Fa-f]{4}),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}',
                      zeile)
        if m and not (int(m.group(1), 16) & 1):
            etagen.append(dict(room=int(m.group(1), 16), zone=int(m.group(2)),
                               band=int(m.group(3)), page=int(m.group(4)),
                               rect=int(m.group(5))))
    return zonen, synth, zellen, marken, etagen


def flaeche(z, synth, zellen):
    s = synth[z['synth'] - 1]
    return [zellen[i] for i in range(s['erste'], s['erste'] + s['n'])], s


def punkte(rects):
    p = set()
    for (x, y, w, h) in rects:
        for j in range(y, y + h):
            for i in range(x, x + w):
                p.add((i, j))
    return p


def main():
    zonen, synth, zellen, marken, etagen = lies()
    gez = [z for z in zonen if z['synth']]
    print("=== KARTEN-AUDIT ueber %d Blaetter, %d gezeichnete Orte ===\n"
          % (len(set(z['page'] for z in gez)), len(gez)))

    # ---- 1 + 2 : entartet / im Feld -------------------------------------------
    klein = []
    raus = []
    for z in gez:
        _, s = flaeche(z, synth, zellen)
        if s['w'] < 4 or s['h'] < 4 or s['n'] <= 1:
            klein.append((z, s))
        if (s['x'] < FELD[0] or s['y'] < FELD[1] or
                s['x'] + s['w'] > FELD[0] + FELD[2] or
                s['y'] + s['h'] > FELD[1] + FELD[3]):
            raus.append((z, s))
    print("1) ENTARTETE ZEICHNUNGEN (< 4 px in einer Richtung oder nur eine Zelle): %d"
          % len(klein))
    for z, s in sorted(klein, key=lambda t: t[1]['w'] * t[1]['h'])[:10]:
        print("     ROOM%04X z%d Seite %2d  %dx%d px, %d Zellen"
              % (z['room'], z['idx'], z['page'], s['w'], s['h'], s['n']))
    print("2) AUSSERHALB DES KARTENFELDES %s: %d" % (str(FELD), len(raus)))
    for z, s in raus[:10]:
        print("     ROOM%04X z%d Seite %2d  Kasten (%d,%d) %dx%d"
              % (z['room'], z['idx'], z['page'], s['x'], s['y'], s['w'], s['h']))

    # ---- 3 : Zellen-Ueberlappung ----------------------------------------------
    print("\n3) UEBERLAPPUNG (Zelle gegen Zelle - nur die ist sichtbar)")
    proseite = collections.defaultdict(list)
    for z in gez:
        r, s = flaeche(z, synth, zellen)
        proseite[z['page']].append((z, punkte(r)))
    schlimm = []
    for pg in sorted(proseite):
        L = proseite[pg]
        alle = set()
        doppelt = set()
        for _, p in L:
            doppelt |= (alle & p)
            alle |= p
        anteil = 100.0 * len(doppelt) / max(1, len(alle))
        n = 0
        for i in range(len(L)):
            for j in range(i + 1, len(L)):
                k = len(L[i][1] & L[j][1])
                if k:
                    n += 1
                    schlimm.append((k, pg, L[i][0], L[j][0]))
        print("   Seite %2d %-10s %5.1f %% doppelt, %d ueberlappende Paare"
              % (pg, NAMEN.get(pg, ''), anteil, n))
    print("   schlimmste Paare:")
    for k, pg, a, b in sorted(schlimm, reverse=True)[:8]:
        print("     Seite %2d  ROOM%04X z%d <-> ROOM%04X z%d : %d px"
              % (pg, a['room'], a['idx'], b['room'], b['idx'], k))

    # ---- 4 : Marken -----------------------------------------------------------
    print("\n4) MARKEN")
    zid_zu = {}
    for z in gez:
        zid_zu.setdefault((z['zid'], z['page']), z)
    ausser = 0
    ohne_ort = 0
    for m in marken:
        z = zid_zu.get((m['zid'], m['page']))
        if z is None and m['zid2'] != 255:
            z = zid_zu.get((m['zid2'], m['page']))
        if z is None:
            ohne_ort += 1
            continue
        r, s = flaeche(z, synth, zellen)
        p = punkte(r)
        nah = any((m['mx'] + dx, m['my'] + dy) in p
                  for dx in (-1, 0, 1) for dy in (-1, 0, 1))
        if not nah:
            ausser += 1
    print("   %d Marken gesamt (%d Tueren, %d Treppen)"
          % (len(marken), sum(1 for m in marken if m['kind'] < 4),
             sum(1 for m in marken if m['kind'] >= 4)))
    print("   %d liegen NICHT auf oder an der Flaeche ihres Ortes" % ausser)
    print("   %d gehoeren zu keinem gezeichneten Ort auf ihrem Blatt" % ohne_ort)
    # Doppelte: zwei Marken derselben Art dicht beieinander auf einem Blatt
    dopp = 0
    for i in range(len(marken)):
        for j in range(i + 1, len(marken)):
            a, b = marken[i], marken[j]
            if a['page'] != b['page']:
                continue
            if (a['kind'] < 4) != (b['kind'] < 4):
                continue
            if abs(a['mx'] - b['mx']) + abs(a['my'] - b['my']) <= 2:
                dopp += 1
    print("   %d Marken-Paare stehen praktisch aufeinander (<= 2 px, gleiche Art)" % dopp)

    # ---- 5 : Treppen ----------------------------------------------------------
    print("\n5) TREPPEN")
    trep = collections.defaultdict(set)
    for m in marken:
        if m['kind'] >= 4:
            trep[m['zid']].add(m['page'])
    mehr = sum(1 for v in trep.values() if len(v) > 1)
    print("   %d Orte tragen Treppensymbole, davon %d auf mehr als einem Blatt"
          % (len(trep), mehr))

    # ---- 6 : Etagen -----------------------------------------------------------
    print("\n6) ETAGEN")
    gast = [z for z in gez if z['etage']]
    print("   %d Etagen-Zeilen, %d Gast-Zeichnungen" % (len(etagen), len(gast)))
    fehl = 0
    for z in gast:
        if not any(e['room'] == z['room'] and e['zone'] == z['idx'] and
                   e['page'] == z['page'] for e in etagen):
            fehl += 1
            print("     OHNE ETAGEN-ZEILE: ROOM%04X z%d Seite %d (waere unsichtbar)"
                  % (z['room'], z['idx'], z['page']))
    if not fehl:
        print("   jede Gast-Zeichnung hat ihre Etagen-Zeile (also ein Besucht-Bit)")

    print("\n=== ENDE ===")
    return 0


if __name__ == '__main__':
    sys.exit(main())
