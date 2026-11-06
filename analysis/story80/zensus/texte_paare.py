#!/usr/bin/env python3
"""Gegenueberstellung fuer den Textabgleich (Arbeitsgrundlage, keine Bewertung):
je Raumnummer die 40%-Disc-Zeilen (aus analysis/story_begegnungen/TEXTE_40.md: Nr., Ausloeser, Sprecher, JP, DE)
und die englischen Zeilen der Port-RDTs (Mod-Baum re15_port/shared_assets/PSX) beider Szenario-Dateien mit
Aufrufern aus scd_zensus.py (lebend/tot, Sub, Bedingung) sowie die Port-Overrides aus
re15_port/engine/src/gen/story_r36_texte.inc.
Aufruf: python texte_paare.py <port.json> <disc.json> <ausgabe.md>"""
import sys, os, re, json
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
P = json.load(open(sys.argv[1], encoding='utf-8')); D = json.load(open(sys.argv[2], encoding='utf-8'))

def texte40():
    t = open(os.path.join(REPO, 'analysis', 'story_begegnungen', 'TEXTE_40.md'), encoding='utf-8').read()
    out = {}; titel = {}; cur = None
    for ln in t.splitlines():
        m = re.match(r'## Raum (\w+): (.*)', ln)
        if m: cur = m.group(1).upper(); titel[cur] = m.group(2); continue
        m = re.match(r'\| (\d+) \| (.*?) \| (.*?) \| (.*?) \| (.*?) \|$', ln)
        if m and cur:
            out.setdefault(cur, []).append(dict(nr=int(m.group(1)), ausl=m.group(2), spr=m.group(3), jp=m.group(4), de=m.group(5)))
    return out, titel

def overrides():
    t = open(os.path.join(REPO, 're15_port', 'engine', 'src', 'gen', 'story_r36_texte.inc'), encoding='utf-8').read()
    out = {}
    for m in re.finditer(r'/\* ROOM(\w{4}) msg (\d+) \((\w+)\) — (.*?)\n(.*?)\*/', t, re.S):
        body = m.group(5)
        alt = re.search(r'alt @(0x\w+) \(\d+ B\): "(.*)"', body); neu = re.search(r'neu \(\d+ B\): "(.*)"', body)
        out[(m.group(1), int(m.group(2)))] = dict(art=m.group(3), grund=m.group(4).strip(),
                                                  alt=alt.group(2) if alt else None, neu=neu.group(1) if neu else None)
    return out

T40, TIT = texte40(); OV = overrides()

def aufrufer(r, i):
    a = [m for m in r['msgs'] if m['msg'] == i]
    if not a: return 'kein Aufruf'
    s = []
    for m in a:
        c = (' wenn ' + ' / '.join(m['cond'])) if m['cond'] else ''
        s.append('%s %s@0x%X %s%s' % ('LEBT' if m['live'] else 'TOT', m['sub'], m['pc'], m['how'], c))
    return '; '.join(s)

def mod_zeilen(k):
    r = P.get(k)
    if r is None: return ['  (keine Datei im Port-Baum)']
    if 'stub' in r: return ['  (4-Byte-Platzhalterdatei)']
    out = []
    for i in sorted(r['texts'], key=int):
        ov = OV.get((k, int(i)))
        z = '  - %s #%s: "%s"  [%s]' % (k, i, r['texts'][i], aufrufer(r, int(i)))
        if ov: z += '\n      PORT-OVERRIDE story_r36 (%s): "%s"' % (ov['art'], ov['neu'])
        out.append(z)
    for (rk, i), ov in OV.items():
        if rk == k and str(i) not in r['texts']:
            out.append('  - %s #%d: PORT-NEU story_r36: "%s"' % (k, i, ov['neu']))
    return out or ['  (kein Text)']

lines = ['# Textpaare Port (Mod-Baum) gegen 40%-Disc — Arbeitsgrundlage', '',
         'Erzeugt von texte_paare.py. LEBT/TOT = statische Erreichbarkeit des aufrufenden Subs (scd_zensus.py).', '']
done = set()
raeume = sorted(set(k[:3] for k in P) | set(T40))
lines.append('## A. Raeume mit Text auf der 40%-Disc'); lines.append('')
for rn in raeume:
    if rn not in T40: continue
    done.add(rn)
    lines.append('### Raum %s — %s' % (rn, TIT.get(rn, '')))
    lines.append('Disc (TEXTE_40.md):')
    for z in T40[rn]:
        lines.append('  - D%s #%d [%s | %s] %s  =  %s' % (rn, z['nr'], z['ausl'], z['spr'], z['jp'], z['de']))
    for v in '01':
        k = rn + v
        lines.append('Port %s (%s):' % (k, 'Leon' if v == '0' else 'Elza'))
        lines.extend(mod_zeilen(k))
    lines.append('')
lines.append('## B. Raeume ohne Disc-Text (Disc-Datei ohne Text oder nur im Mod-Baum)'); lines.append('')
for rn in raeume:
    if rn in done: continue
    ks = [rn + v for v in '01' if rn + v in P and 'stub' not in P[rn + v] and P[rn + v]['texts']]
    ks += [k for (k, i) in OV if k[:3] == rn and k not in ks]
    if not ks: continue
    disc = 'Disc-Datei vorhanden, ohne Text' if any(rn + v in D for v in '01') else 'NUR MOD-BAUM'
    lines.append('### Raum %s — %s' % (rn, disc))
    for k in sorted(set(ks)):
        lines.append('Port %s (%s):' % (k, 'Leon' if k[3] in '02468ACE' else 'Elza'))
        lines.extend(mod_zeilen(k))
    lines.append('')
open(sys.argv[3], 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('Disc-Raeume mit Text', len(done), 'Overrides', len(OV))
