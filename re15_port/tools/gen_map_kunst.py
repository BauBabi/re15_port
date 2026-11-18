# -*- coding: utf-8 -*-
"""KARTE r42 Stufe 3 Runde 2: die GEMALTE Flaeche jeder Kachel als Zeilen-Spannen (engine/src/re15_map_kunst.h).

Wozu: der Positionsmarker wird bisher nur ins RECHTECK der Kachel geklemmt (re15_inv_screen.c, Rand-Reserve).
Ein Rechteck ist aber nur die Bounding-Box; die gemalte Kachel darin kann L-, U- oder ringfoermig sein (Memory
reai-v2-bbox-ist-nicht-kunst). An ECHTEN Ankunftslagen (Door_aot_set-Spawn) stand der Marker deshalb in sechs
Raeumen 3-8 px neben der eigenen Kachel (Pruefbericht Stufe 3 Runde 1, Vorpruefung s3r2/vorpruef_vorher.txt):
6000 (Gang nach 50B0, ungemalt), 4040/4050/4080 (Ankunft an der Tuer, auf der Nachbarkachel), 1090, 11A0.

Quelle (nichts gewaehlt):
  - Rechtecke {x,y,w,h,u,v}: Seitentabelle @0x80076840 (u16 Anzahl, u32 Zeiger je Seite, 12 B je Rechteck,
    u = Byte +8, v = Byte +10) - dieselbe Lesung wie gen_map_zones.rects()/rect_uv() inkl. RECT_FIX Seite 3;
  - Texel: DATA/MAP0x.PIX (Datei-Id u16 @0x80074c4c + 2*Seite = 12..24 -> MAP01..MAP0D), headerlos,
    256x256 4 bpp, 128 B je Zeile, unteres Nibble = linkes Pixel. RAUMKOERPER = Index != 0 und != 4: Index 0 ist
    transparent (der Kachel-SPRT zeichnet ihn nicht), Index 4 ist die WANDLINIE (CLUT-Index 4 = 0x5ad6, abgemessen
    an DATA/MAP05.PIX Seite 4 Rect 4, Beleg re15_inv_screen.c Tuer-Nische). Auf der Wandlinie stand der Punkt halb
    ausserhalb des Raums (Abzug L6000_t0 Stufe 3 Runde 2: um 0.07); im Koerper steht er auf der Raumfarbe.
Ausgabe je Rechteck, das eine Zone traegt: Spannen (y, x0, Laenge) relativ zur Rechteck-Ecke.

Aufruf: python re15_port/tools/gen_map_kunst.py        (schreibt engine/src/re15_map_kunst.h)
        python re15_port/tools/gen_map_kunst.py --pruef (vergleicht nur, schreibt nichts; Exit 1 bei Abweichung)
"""
import io, os, re, sys

HIER = os.path.dirname(os.path.abspath(__file__))
_s = io.open(os.path.join(HIER, 'gen_map_zones.py'), encoding='utf-8').read()
G = {'__name__': 'kunst', '__file__': os.path.join(HIER, 'gen_map_zones.py')}
exec(compile(_s[:_s.index('def main(')], 'gen', 'exec'), G)
rects, rect_uv, page_pix = G['rects'], G['rect_uv'], G['page_pix']
KOPF = os.path.join(HIER, '..', 'engine', 'src', 're15_map_zones.h')
AUS = os.path.join(HIER, '..', 'engine', 'src', 're15_map_kunst.h')


def benutzte_rechtecke():
    t = io.open(KOPF, encoding='utf-8').read()
    i = t.index('s_map_zones[] = {'); j = t.index('\n};', i)
    out = set()
    for z in t[i:j].split('\n'):
        if not z.strip().startswith('{'):
            continue
        m = [int(x, 0) for x in re.findall(r'0x[0-9A-Fa-f]+|-?\d+', z.split('/*')[0])]
        if len(m) >= 17 and m[6] != 255:
            out.add((m[5], m[6]))
    return sorted(out)


def spannen(pg, r, aus=(0, 4)):
    """aus = Indizes, die NICHT zaehlen: (0, 4) = Raumkoerper, (0,) = die ganze Kunst samt Wandlinie (Stufe 3 Runde 4)."""
    pix = page_pix(pg)
    rx, ry, rw, rh = rects(pg)[r]
    u, v = rect_uv(pg, r)
    out = []
    for dy in range(rh):
        dx = 0
        while dx < rw:
            if 0 <= v + dy < 256 and 0 <= u + dx < 256 and pix[v + dy][u + dx] not in aus:
                a = dx
                while dx < rw and 0 <= u + dx < 256 and pix[v + dy][u + dx] not in aus:
                    dx += 1
                out.append((dy, a, dx - a))
            else:
                dx += 1
    return rx, ry, out


def erzeugen():
    idx, sp, sa = [], [], []
    for pg, r in benutzte_rechtecke():
        rx, ry, s = spannen(pg, r)
        _, _, a = spannen(pg, r, (0,))
        idx.append((pg, r, rx, ry, len(sp), len(s), len(sa), len(a)))
        sp.extend(s)
        sa.extend(a)
    z = ['/* GENERIERT von tools/gen_map_kunst.py - NICHT VON HAND AENDERN.',
         ' * Der RAUMKOERPER jeder Karten-Kachel (Index != 0 und != 4 = ohne Wandlinie, DATA/MAP0x.PIX, Kachel-uv aus der Seitentabelle',
         ' * @0x80076840) als Zeilen-Spannen relativ zur Rechteck-Ecke. Verbraucher: re15_karte_r42_kunst_klemmen',
         ' * (karte_r42.c) - PORT-WAHL r42/karte Stufe 3 Runde 2, siehe dort.',
         ' * Stufe 3 Runde 4: dazu die GANZE Kunst (Index != 0, samt Wandlinie) - re15_karte_r42_ring_klemmen prueft damit, ob der',
         ' * Ring des Punkts auf einer FREMDEN Kachel liegt (Uebergang zum Nachbarraum). */',
         'typedef struct { unsigned char page, rect; short rx, ry; unsigned short erst, anzahl, erst_a, anzahl_a; } re15_map_kunst_idx_t;',
         'typedef struct { unsigned char y, x0, len; } re15_map_kunst_spanne_t;',
         'static const re15_map_kunst_idx_t s_map_kunst_idx[] = {']
    for pg, r, rx, ry, e, n, ea, na in idx:
        z.append('    { %2d, %2d, %3d, %3d, %4d, %3d, %4d, %3d },' % (pg, r, rx, ry, e, n, ea, na))
    z.append('};')
    z.append('static const re15_map_kunst_spanne_t s_map_kunst_spannen[] = {')
    for k in range(0, len(sp), 8):
        z.append('    ' + ' '.join('{%d,%d,%d},' % t for t in sp[k:k + 8]))
    z.append('};')
    z.append('static const re15_map_kunst_spanne_t s_map_kunst_alles[] = {')
    for k in range(0, len(sa), 8):
        z.append('    ' + ' '.join('{%d,%d,%d},' % t for t in sa[k:k + 8]))
    z.append('};')
    z.append('#define RE15_MAP_KUNST_IDX_N %d' % len(idx))
    return '\n'.join(z) + '\n', len(idx), len(sp)


if __name__ == '__main__':
    text, ni, ns = erzeugen()
    if '--pruef' in sys.argv:
        alt = io.open(AUS, encoding='utf-8').read() if os.path.exists(AUS) else ''
        print('gleich' if alt == text else 'ABWEICHUNG')
        sys.exit(0 if alt == text else 1)
    io.open(AUS, 'w', encoding='utf-8', newline='\n').write(text)
    print('re15_map_kunst.h: %d Rechtecke, %d Spannen' % (ni, ns))
