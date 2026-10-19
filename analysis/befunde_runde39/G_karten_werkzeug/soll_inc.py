"""Erzeugt re15_port/tests/unit/r39_karten_soll.inc: die vier Soll-Bilder (rechte Haelften der Nutzer-Vorlagen
map1F/2F/3F/B2_correct.png, auf 320x240 zurueckgerechnet = G_karten_bilder/soll_*.png, Dossier M1) als Klassenbild
(komp.farbe_klasse: 0 Panel, 1 Koerper, 2 Wand, 3 Koerper ueber Wand, 4 Tuer, 5 Treppe, 6 aktuell, 7 Sonstiges),
zeilenweise lauflaengen-kodiert. Aktuell (6) zaehlt wie Koerper (1) - der Nutzer stand in seinem Abzug in einem Raum.
Aufruf: python soll_inc.py <ausgabe.inc>"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from komp import farbe_klasse, KL
import numpy as np
from PIL import Image
BD = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'G_karten_bilder')
BLAETTER = [(1, 'mapB2'), (2, 'map1F'), (3, 'map2F'), (4, 'map3F')]
out = ['/* GENERIERT von analysis/befunde_runde39/G_karten_werkzeug/soll_inc.py - NICHT VON HAND AENDERN.',
       ' * Runde 39 Spur G: Soll-Klassenbilder der Kartenblaetter B2/1F/2F/3F aus den Nutzer-Vorlagen (rechte Haelfte).',
       ' * Je Blatt 240 Zeilen, je Zeile Laeufe (Klasse, Laenge) bis 320; Klassen wie komp.py farbe_klasse. */']
for seite, name in BLAETTER:
    k = farbe_klasse(np.array(Image.open(os.path.join(BD, 'soll_%s.png' % name)).convert('RGB')))
    k[k == KL['C']] = KL['B']; k[k == KL['CW']] = KL['X']; k[k > 7] = 7
    werte = []
    for y in range(240):
        x = 0
        while x < 320:
            c = int(k[y, x]); n = 1
            while x + n < 320 and int(k[y, x + n]) == c: n += 1
            werte += [c, n]; x += n
    out.append('static const unsigned short k_soll_%d[] = { /* %s, %d Werte */' % (seite, name, len(werte)))
    for i in range(0, len(werte), 24):
        out.append('    ' + ', '.join(str(v) for v in werte[i:i + 24]) + ',')
    out.append('};')
out.append('static const struct { int seite; const unsigned short *rle; int n; } k_soll[] = {')
for seite, name in BLAETTER:
    out.append('    { %d, k_soll_%d, (int)(sizeof k_soll_%d / sizeof k_soll_%d[0]) },' % (seite, seite, seite, seite))
out.append('};')
open(sys.argv[1], 'w', newline='\n').write('\n'.join(out) + '\n')
print('geschrieben', sys.argv[1])
