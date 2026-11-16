"""Messtabelle je h (jedes 2. Bild h16-h104): Video / vorher / nachher - Breite s .3/.5/.7, Spitze (Lage, Abstand zum Video), Austritt,
Rolle phi der sichtbaren Glieder (0 = Bauch zur Kamera, 90 = Flanke), groesster sichtbarer Gelenkwinkel, groesste Verdrehung.

    python tabelle_md.py <mess_vorher.json> <mess_nachher.json> <ausgabe.md>
"""
import sys, os, json, math
V = json.load(open(sys.argv[1]))['je_h']; N = json.load(open(sys.argv[2]))['je_h']
# Bogen h76-h100: Videobezug = einheitliche Ablesung (bogen_v8.json), nicht die Linienablesung der Analyse
B = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'bogen_v8.json')))['je_h']
for h in range(76, 101, 2):
    for D in (V, N):
        r = D.get(str(h))
        if not r:
            continue
        b = B.get(str(h))
        r['video']['v_spitze'] = b['spitze'] if b else None
        r['video']['v_sicht'] = 'bogen (Ablesung)' if b else ('bogen, Spitze verdeckt' if h == 82 else 'bogen, unsicher')
        r['spitze_fehler'] = round(math.hypot(r['spitze'][0] - b['spitze'][0], r['spitze'][1] - b['spitze'][1]), 1) if b else None
        if b:
            r['video']['v_wurzel'] = b['austritt']
            if r.get('austritt'):
                r['austritt_fehler'] = round(math.hypot(r['austritt'][0] - b['austritt'][0], r['austritt'][1] - b['austritt'][1]), 1)


def br(b):
    return '/'.join('-' if x is None else ('%g' % x) for x in b[1:4]) if b else '-'


def pt(p):
    return '(%d,%d)' % (round(p[0]), round(p[1])) if p else '-'


def rolle(r):
    ph = [x for x in r['phi'] if x is not None]
    return ('%d-%d' % (round(min(ph)), round(max(ph)))) if ph else '-'


def sicht_gelenk(r):
    """groesster Gelenkwinkel zwischen zwei sichtbaren Gliedern (phi != None = Querschnitt vorhanden)."""
    ph = r['phi']; g = r['gelenk']
    w = [g[j] for j in range(6) if ph[j] is not None and ph[j + 1] is not None]
    return ('%d' % round(max(w))) if w else '-'


def tw(r):
    t = [abs(x) for x in r['twist'] if x is not None]
    return ('%d' % round(max(t))) if t else '-'


z = ['| h | Video sichtbar | Video Breite s.3/.5/.7 | Video Spitze | vorher Breite | vorher Spitze (Abst.) | vorher Rolle | '
     'nachher Breite | nachher Spitze (Abst.) | nachher Austritt (Abst.) | nachher Rolle | nachher Knick sichtbar max | nachher Verdrehung max |',
     '|---|---|---|---|---|---|---|---|---|---|---|---|---|']
for h in range(16, 105, 2):
    v, n = V.get(str(h)), N.get(str(h))
    if not v or not n:
        continue
    vid = n['video']
    sichtbar = vid.get('v_sicht') or '-'
    z.append('| %d | %s | %s | %s | %s | %s (%s) | %s | %s | %s (%s) | %s (%s) | %s | %s | %s |' % (
        h, sichtbar, br(vid.get('v_breiten')), pt(vid.get('v_spitze')),
        br(v.get('breiten')), pt(v['spitze']), v.get('spitze_fehler', '-') if v.get('spitze_fehler') is not None else '-', rolle(v),
        br(n.get('breiten')), pt(n['spitze']), n.get('spitze_fehler') if n.get('spitze_fehler') is not None else '-',
        pt(n.get('austritt')), n.get('austritt_fehler') if n.get('austritt_fehler') is not None else '-', rolle(n), sicht_gelenk(n), tw(n)))
open(sys.argv[3], 'w', encoding='utf-8').write('\n'.join(z) + '\n')
print('\n'.join(z))
