# -*- coding: utf-8 -*-
"""Nachtrag K: das Hand+Granate-Netz aus PLD/PL00W09.PLW (dir[2] MD1) gegen die blosse Hand
(PLD/PL00.MD1 Mesh 11, der Teil, den die Waffe ersetzt - Skill re15-weapon-render §2).

PLW-Kopf: word0 = Verzeichnis-Offset, word1 = Anzahl (4); Verzeichnis = 4 u32 am Offset
(dir[0] EDD, dir[1] EMR, dir[2] MD1, dir[3] TIM).
Frage: welche Punkte/Flaechen gehoeren zur GRANATE (nicht zur Hand)?
"""
import os, sys, struct, collections
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sicherung_werkzeug'))
from r30_lib import PSX, md1_lesen  # noqa


def plw_dir(d):
    off, n = struct.unpack_from('<II', d, 0)
    return off, n, list(struct.unpack_from('<%dI' % n, d, off))


def zusammenhang(m):
    """Zusammenhaengende Teilnetze (ueber gemeinsame Punkte) je Liste tris/quads."""
    teile = []
    for art, vl, fl in (('tri', m['tv'], m['tris']), ('quad', m['qv'], m['quads'])):
        par = list(range(len(vl)))
        def f(a):
            while par[a] != a:
                par[a] = par[par[a]]; a = par[a]
            return a
        for fc in fl:
            for a in fc[1:]:
                ra, rb = f(fc[0]), f(a)
                if ra != rb: par[ra] = rb
        gr = collections.defaultdict(list)
        for i, fc in enumerate(fl):
            gr[f(fc[0])].append(i)
        for k, fis in gr.items():
            pts = sorted({v for i in fis for v in fl[i]})
            xs = [vl[p][0] for p in pts]; ys = [vl[p][1] for p in pts]; zs = [vl[p][2] for p in pts]
            teile.append((art, len(fis), len(pts), (min(xs), max(xs)), (min(ys), max(ys)), (min(zs), max(zs)), fis))
    return teile


for name in ('PL00W09.PLW', 'PL04W09.PLW', 'PL00W03.PLW'):
    d = open(os.path.join(PSX, 'PLD', name), 'rb').read()
    off, n, dr = plw_dir(d)
    print('%s: %d B, Verzeichnis @0x%X n=%d: %s' % (name, len(d), off, n, ' '.join('0x%X' % x for x in dr)))
    md1 = d[dr[2]:dr[3]]
    m = md1_lesen(md1)
    print('   MD1 @0x%X..0x%X (%d B): nobj=%d' % (dr[2], dr[3], len(md1), m['nobj']))
    for mi, me in enumerate(m['meshes']):
        print('   Mesh %d: %d Dreieckspunkte / %d Dreiecke, %d Viereckspunkte / %d Vierecke' % (
            mi, len(me['tv']), len(me['tris']), len(me['qv']), len(me['quads'])))
        pages = collections.Counter((u[2], u[5]) for u in me['tuv'])
        pages.update((u[2], u[5]) for u in me['quv'])
        print('      (clut,page):', ', '.join('(0x%04X,0x%04X) x%d' % (c, p, k) for (c, p), k in pages.items()))
        for t in zusammenhang(me):
            print('      Teilnetz %-4s %2d Flaechen %2d Punkte  x%s y%s z%s' % t[:6])
    tim = d[dr[3]:off]
    if len(tim) >= 20:
        magic, flag = struct.unpack_from('<II', tim, 0)
        clen, cx, cy, cw, ch = struct.unpack_from('<IHHHH', tim, 8)
        blen, bx, by, bw, bh = struct.unpack_from('<IHHHH', tim, 8 + clen)
        print('   TIM @0x%X: flag=%d CLUT (%d,%d) %dx%d  Bild (%d,%d) %dhw x %d = %dx%d px' % (
            dr[3], flag, cx, cy, cw, ch, bx, by, bw, bh, bw * (2 if flag & 7 == 1 else 4), bh))

hand = md1_lesen(open(os.path.join(PSX, 'PLD', 'PL00.MD1'), 'rb').read())
h = hand['meshes'][11]
print('PL00.MD1 Mesh 11 (rechte Hand): %d/%d Dreiecke, %d/%d Vierecke' % (len(h['tv']), len(h['tris']), len(h['qv']), len(h['quads'])))
for t in zusammenhang(h):
    print('      Teilnetz %-4s %2d Flaechen %2d Punkte  x%s y%s z%s' % t[:6])
