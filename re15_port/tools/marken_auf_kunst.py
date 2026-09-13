# -*- coding: utf-8 -*-
"""MARKEN AUF DER KUNST: sitzt jede Tuer-/Treppenmarke auf gemaltem Grundriss?

⛔ WARUM ES DAS GIBT (Nutzer-Befund 2026-09-04, fehler/error1.png): auf 2F stand das
Treppensymbol auf leerem Blau - "da fehlt das komplette Rechteck". Der Kasten eines
Rechtecks ist groesser als die Zeichnung darin (die Kachel enthaelt Schwarz), eine Marke
kann also INNERHALB ihres Rechtecks liegen und trotzdem auf nichts sitzen. Genau das
sieht der Spieler, und keine Kasten-Pruefung faengt es.

Gemessen wird an der Kachel, aus der der Zeichner blittet: Bildpunkt der Marke
= (u + (mx - x), v + (my - y)) im Blatt DATA/MAP0x.PIX; Palettenindex 0 = nicht gezeichnet.

    python re15_port/tools/marken_auf_kunst.py [--blatt N]
"""
import io, os, re, sys

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, '..', '..'))
DATA = os.path.join(WURZEL, 're15_port', 'shared_assets', 'PSX', 'DATA')
KOPF = os.path.join(WURZEL, 're15_port', 'engine', 'src', 're15_map_zones.h')
sys.path.insert(0, HIER)
_s = io.open(os.path.join(HIER, 'gen_map_zones.py'), encoding='utf-8').read()
_ns = {'__name__': 'mak', '__file__': os.path.join(HIER, 'gen_map_zones.py')}
exec(compile(_s[:_s.index('def main(')], 'gen', 'exec'), _ns)
rects, rect_uv = _ns['rects'], _ns['rect_uv']


def tabelle(name):
    s = io.open(KOPF, encoding='utf-8').read()
    i = s.index('s_map_%s[] = {' % name); j = s.index('};', i)
    aus = []
    for z in s[i:j].split('\n'):
        if not z.strip().startswith('{'):
            continue
        m = re.findall(r'0x[0-9A-Fa-f]+|-?\d+', z.split('/*')[0])
        if m:
            aus.append([int(x, 0) for x in m])
    return aus


def kachel(pg):
    d = open(os.path.join(DATA, 'MAP%02X.PIX' % (pg + 1)), 'rb').read()
    return [[((d[y * 128 + (u >> 1)] >> 4) if (u & 1) else (d[y * 128 + (u >> 1)] & 0xF))
             for u in range(256)] for y in range(256)]


def schema_kaesten():
    """Die Schema-Zeichnungen (s_map_synth) mit ihrer Seite aus den Zonen."""
    zonen = tabelle('zones')
    synth = tabelle('synth')
    aus = []
    gesehen = set()
    for z in zonen:
        # zones-Zeile: room, wx0, wz0, wx1, wz1, page, rect, idx, zid, ox, oy,
        #              sx, sy, flipx, flipy, synth, ...
        if len(z) < 16:
            continue
        page, sy_idx = z[5], z[15]
        if not sy_idx or sy_idx > len(synth) or sy_idx in gesehen:
            continue
        gesehen.add(sy_idx)
        x, y, w, h = synth[sy_idx - 1][:4]
        aus.append((page, x, y, w, h, z[0], sy_idx))
    return aus


def pruefe_schema(nur):
    """⛔ EIGENER RIEGEL (2026-09-13): eine SCHEMA-Zeichnung fuellt ihren Kasten - faellt
    er auf bemalte Texel, uebermalt er die Kunst des Nachbarn. Der Unit-Test kann das
    nicht sehen: er kennt nur die Rechteck-BBOX, und die reicht bei gedreht montierten
    Kacheln weit ueber die Kunst hinaus (ROOM1050s Rect 0 auf Blatt 2). Gemessen an der
    Kachel, aus der der Zeichner blittet - wie bei den Marken oben."""
    kaesten = schema_kaesten()
    if not kaesten:
        print('Schema-Zeichnungen: keine')
        return 0
    schlecht = 0
    for (pg, x, y, w, h, room, si) in kaesten:
        if nur is not None and pg != nur:
            continue
        px = kachel(pg); rr = rects(pg)
        treffer = 0
        for yy in range(y, y + h):
            for xx in range(x, x + w):
                for ri, (rx, ry, rw, rh) in enumerate(rr):
                    if rx <= xx < rx + rw and ry <= yy < ry + rh:
                        u, v = rect_uv(pg, ri)
                        su, sv = u + (xx - rx), v + (yy - ry)
                        if 0 <= su < 256 and 0 <= sv < 256 and px[sv][su]:
                            treffer += 1
                        break
        marke = 'OK' if treffer == 0 else '*** UEBERMALT KUNST ***'
        print('Schema %d (ROOM%04X, Blatt %d) Kasten (%d,%d) %dx%d: %d von %d Punkten '
              'auf bemalter Flaeche  %s'
              % (si, room, pg, x, y, w, h, treffer, w * h, marke))
        if treffer:
            schlecht += 1
    return schlecht


def main(argv):
    nur = int(argv[argv.index('--blatt') + 1]) if '--blatt' in argv else None
    schlecht_schema = pruefe_schema(nur)
    marken = tabelle('marks')
    n_ges = n_leer = 0
    for pg in range(13):
        if nur is not None and pg != nur:
            continue
        mm = [(k, m) for k, m in enumerate(marken) if m[0] == pg and m[1] != 255]
        if not mm:
            continue
        px = kachel(pg); rr = rects(pg)
        leer = []
        for k, m in mm:
            _, ri, mx, my, kind = m[:5]
            if ri >= len(rr):
                leer.append((k, ri, mx, my, 'Rechteck %d gibt es nicht' % ri)); continue
            x, y, w, h = rr[ri]
            u, v = rect_uv(pg, ri)
            su, sv = u + (mx - x), v + (my - y)
            if not (0 <= su < 256 and 0 <= sv < 256):
                leer.append((k, ri, mx, my, 'ausserhalb des Blatts')); continue
            # 3x3 um den Bildpunkt: eine Marke darf an der Wand kleben
            treffer = any(px[sv + b][su + a]
                          for b in (-1, 0, 1) for a in (-1, 0, 1)
                          if 0 <= sv + b < 256 and 0 <= su + a < 256)
            if not treffer:
                leer.append((k, ri, mx, my, 'Kachelpunkt (%d,%d) ist Index 0' % (su, sv)))
        n_ges += len(mm); n_leer += len(leer)
        print('Blatt %2d: %3d Marken mit Rechteck, %d auf leerer Kunst' % (pg, len(mm), len(leer)))
        for (k, ri, mx, my, warum) in leer:
            print('    Marke #%-3d rect %2d (%3d,%3d)  %s' % (k, ri, mx, my, warum))
    print('GESAMT: %d Marken, %d auf leerer Kunst (%.1f %%)'
          % (n_ges, n_leer, 100.0 * n_leer / max(1, n_ges)))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
