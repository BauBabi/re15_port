# -*- coding: utf-8 -*-
"""KARTE r42: Zeilen + Tuermarken fuer STAGE2 (Blatt 6) und STAGE3 (Blatt 7).

PORT-ERGAENZUNG nach der Original-Formel FUN_800473f8 @0x8004741c-0x80047528
    mx = +-((((wx+32000)*10*sx)>>20)+5)/10 + ox ,  my = -+((((wz+32000)*10*sy)>>20)+5)/10 + oy
(Spiegel je Achse = flip_x/flip_z der Zonenzeile, re15_map_zone_marker).

Je ORT (Raum, Zone) steht im PLAN das gemalte Rechteck. Die Zeile entsteht so:
  * AUSGELIEFERT (@0x800768b0 + 8*Slot, Massstab > 1): unveraendert uebernommen.
  * GLEICHER RAHMEN: Ort, dessen Tuer-Spawn im Nachbarn neben dem eigenen Trigger liegt
    (ROOM2070 z0 <-> ROOM2000: Trigger (-19800,-6200), Spawn (-18650,-6450)) -> Zeile des Nachbarn.
  * HERGELEITET ("min"): die Kollisions-Bbox der Zone mittig in die GEMALTE Flaeche des
    Rechtecks (DATA/MAP0<p+1>.PIX), Massstab je Achse = min(Blattmassstab, passend). Der
    Blattmassstab ist die ausgelieferte Zeile des Blatts (STAGE2: ROOM2000 @0x800769E0
    2340/2308; STAGE3: Median der ausgelieferten Zeilen des Blatts). Spiegel: die Wahl, bei
    der die Tueren zu den schon gesetzten Nachbarn am besten aufeinander fallen (die
    Tuerdaten stecken NICHT im Fit, sie waehlen nur unter den 4 Spiegelungen).

Pruefgroessen ausserhalb des Fits: Tuerpaar-Luecke (beide Seiten derselben Tuer), Abstand
zum gemalten Tuersymbol (analysis/kartensymbole/symbolkatalog.csv).

Aufruf: python re15_port/tools/karte_r42_zeilen.py <6|7>
"""
import io, os, re, sys, itertools, statistics

HIER = os.path.dirname(os.path.abspath(__file__))
_s = io.open(os.path.join(HIER, 'gen_map_zones.py'), encoding='utf-8').read()
G = {'__name__': 'k42', '__file__': os.path.join(HIER, 'gen_map_zones.py')}
exec(compile(_s[:_s.index('def main(')], 'gen', 'exec'), G)
rects, read_rdt, u16, s16 = G['rects'], G['read_rdt'], G['u16'], G['s16']
BASES = [0, 38, 50, 65, 77, 98]


def zeile(rid):
    st = (rid >> 12) - 1
    slot = BASES[st] + ((rid >> 4) & 0xFF)
    va = 0x800768b0 + 8 * slot
    return s16(va), s16(va + 2), u16(va + 4), u16(va + 6), va


def sf(v, s):
    return ((((v + 32000) * 10 * s) >> 20) + 5) // 10


def proj(x, z, row):
    ox, oy, sx, sy, fx, fz = row[:6]
    a = sf(x, sx)
    b = sf(z, sy)
    return ((-a if fx else a) + ox, (b if fz else -b) + oy)


def gemalt(pg, r):
    s = G['_kunst_flaeche'](pg, r)
    xs = [p[0] for p in s]
    ys = [p[1] for p in s]
    return min(xs), min(ys), max(xs), max(ys)


def zeile_min(bbox, ziel, fx, fz, skala):
    x0, z0, x1, z1 = bbox
    X0, Y0, X1, Y1 = ziel
    sx = min(skala[0], round((X1 - X0) * 2 ** 20 / max(1, x1 - x0)))
    sy = min(skala[1], round((Y1 - Y0) * 2 ** 20 / max(1, z1 - z0)))
    cx = (x0 + x1) // 2
    cz = (z0 + z1) // 2
    ox = round((X0 + X1) / 2 - (-sf(cx, sx) if fx else sf(cx, sx)))
    oy = round((Y0 + Y1) / 2 - (sf(cz, sy) if fz else -sf(cz, sy)))
    return (ox, oy, sx, sy, fx, fz)


def symbol(pg, p):
    b = None
    for gx, gy, gw, gh, w in G['GLYPHEN'].get(pg, ()):
        d = max(abs(p[0] - gx), abs(p[1] - gy))
        if b is None or d < b:
            b = d
    return b


# ---- PLAENE ---------------------------------------------------------------------------
# Ort: (rect, Quelle, Zielkasten oder None, bbox oder None, zid, Notiz)
#   Quelle 'echt' = ausgelieferte Zeile, 'wie:<rid>.<z>' = Zeile eines Orts im selben Rahmen,
#   'min' = hergeleitet, 'min:fx,fz' = hergeleitet mit festem Spiegel.
PLAENE = {
    6: dict(skala=(2340, 2308), orte={
        (0x2000, 0): (0, 'echt', None, None, 37, 'U-Kanal'),
        (0x2010, 0): (1, 'min', None, None, 38, 'Westraum, gemalte Tuer (86,129) Ostwand'),
        (0x2020, 0): (4, 'min', None, (-23000, -5850, 15100, 5300), 113, 'Kanal r4, NEU (fehlte)'),
        (0x2030, 0): (6, 'min:1,1', None, None, 39, 'Raum mit Achteck, Spiegel aus 4 Tueren'),
        (0x2040, 0): (5, 'min', None, (-21500, -8300, 25750, 4350), 40, 'langer Kanal r5'),
        (0x2050, 0): (2, 'min', None, None, 41, 'Raum oben links'),
        (0x2060, 0): (7, 'min:1,1', None, None, 42, 'gemalte Tuer (199,124) Westwand, Spiegel aus 2 Tueren'),
        (0x2070, 0): (3, 'wie:2000.0', None, None, 43, 'Gang 2000-2030'),
        (0x2070, 1): (3, 'min:1,0', None, None, 44, 'Gang-Kopie ohne Spawn'),
        (0x2080, 0): (8, 'min', (227, 131, 240, 179), None, 46, 'Schacht r8'),
        (0x2080, 1): (8, 'min', (193, 151, 227, 158), None, 47, 'Querstueck r8'),
        (0x2090, 0): (9, 'min', None, None, 48, 'gemalte Treppe (241,174)'),
        (0x20A0, 0): (10, 'min', None, (-5600, -28900, 29000, 7000), 114, 'PORT-WAHL r10'),
    }),
    7: dict(skala=None, orte={
        (0x3000, 0): (0, 'echt', None, None, 49, ''),
        (0x3010, 0): (1, 'echt', None, None, 50, ''),
        (0x3020, 0): (2, 'min', None, None, 51, ''),
        (0x3030, 0): (3, 'echt', None, None, 52, 'Bbox exakt auf r3'),
        (0x3040, 0): (4, 'echt', None, None, 53, ''),
        (0x3050, 0): (5, 'min', None, None, 54, 'Tuer 3040->3050 auf r4-Westsymbol'),
        (0x3060, 0): (6, 'echt', None, None, 55, 'Tueren auf r6-Symbolen'),
        (0x3070, 0): (7, 'echt', None, None, 56, ''),
        (0x3080, 0): (8, 'echt', None, None, 57, ''),
        (0x3090, 0): (9, 'echt', None, None, 58, 'Tueren auf r9/r10/r11'),
        (0x30A0, 0): (10, 'min', None, None, 59, ''),
        (0x30B0, 0): (11, 'min', None, None, 60, ''),
        (0x30C0, 0): (12, 'min', None, None, 61, ''),
        (0x30D0, 0): (2, 'min', None, (-27200, -30618, -5300, -11418), 51, 'Suedteil = Variante von 3020, teilt dessen zid'),
        (0x30D0, 1): (12, 'min', None, (-27200, -14300, -5275, 1650), 61, 'Nordteil = Variante von 30C0, teilt dessen zid'),
        (0x30E0, 1): (13, 'echt', None, None, 64, '88 % auf r13'),
    }),
}


def zonen_bbox():
    kopf = io.open(os.path.join(HIER, '..', 'engine', 'src', 're15_map_zones.h'), encoding='utf-8').read()
    i = kopf.index('s_map_zones[] = {')
    j = kopf.index('\n};', i)
    out = {}
    for z in kopf[i:j].split('\n'):
        if not z.strip().startswith('{'):
            continue
        m = [int(x, 0) for x in re.findall(r'0x[0-9A-Fa-f]+|-?\d+', z.split('/*')[0])]
        if len(m) >= 17 and not m[16] and (m[0], m[7]) not in out:
            out[(m[0], m[7])] = (m[1], m[2], m[3], m[4])
    return out


def tueren(rid):
    r = read_rdt(rid)
    return r[1] if r else []


def ort_von(orte, bb, rid, x, z, slack=1500):
    best = None
    for (r, i) in orte:
        if r != rid:
            continue
        x0, z0, x1, z1 = bb[(r, i)]
        d = max(x0 - x, 0, x - x1) + max(z0 - z, 0, z - z1)
        if best is None or d < best[0]:
            best = (d, (r, i))
    return best[1] if best and best[0] <= slack else None


def paare(orte, bb):
    raeume = sorted({r for r, _ in orte})
    out = []
    for R in raeume:
        for d in tueren(R):
            N = d['dest']
            if N == R or N not in raeume:
                continue
            best = None
            for e in tueren(N):
                if e['dest'] != R:
                    continue
                dd = (abs(e['nx'] - d['lx']) + abs(e['nz'] - d['lz'])
                      + abs(d['nx'] - e['lx']) + abs(d['nz'] - e['lz']))
                if dd <= 8000 and (best is None or dd < best[0]):
                    best = (dd, e)
            if not best:
                continue
            u = ort_von(orte, bb, R, d['lx'], d['lz'])
            v = ort_von(orte, bb, N, best[1]['lx'], best[1]['lz'])
            if u and v:
                out.append((u, v, d, best[1]))
    return out


def loese(pg, log=print):
    plan = PLAENE[pg]
    orte = plan['orte']
    bb = zonen_bbox()
    for u, o in orte.items():
        if o[3]:
            bb[u] = o[3]
    skala = plan['skala']
    rows = {}
    for u, o in orte.items():
        if o[1] == 'echt':
            z = zeile(u[0])
            assert z[2] > 1, '%04X ohne ausgelieferte Zeile' % u[0]
            rows[u] = (z[0], z[1], z[2], z[3], 0, 0, 'ausgeliefert @0x%08X' % z[4])
    if skala is None:
        skala = (int(statistics.median(r[2] for r in rows.values())),
                 int(statistics.median(r[3] for r in rows.values())))
    for u, o in orte.items():
        if o[1].startswith('wie:'):
            a, b = o[1][4:].split('.')
            rows[u] = rows[(int(a, 16), int(b))][:6] + ('selber Rahmen wie ROOM%s' % a,)
    P = paare(orte, bb)
    offen = [u for u in orte if u not in rows]

    def anker(u):
        return [(a, b, d, e) for (a, b, d, e) in P if a == u and b in rows]

    while offen:
        offen.sort(key=lambda u: -len(anker(u)))
        u = offen.pop(0)
        o = orte[u]
        ziel = o[2] or gemalt(pg, o[0])
        if o[1].startswith('min:'):
            wahl = [tuple(int(t) for t in o[1][4:].split(','))]
        else:
            wahl = list(itertools.product((0, 1), (0, 1)))
        best = None
        for fx, fz in wahl:
            row = zeile_min(bb[u], ziel, fx, fz, skala)
            ab = []
            for (a, b, d, e) in anker(u):
                p1 = proj(d['lx'], d['lz'], row)
                p2 = proj(e['lx'], e['lz'], rows[b])
                ab.append(max(abs(p1[0] - p2[0]), abs(p1[1] - p2[1])))
            sc = (max(ab) if ab else 0, sum(ab))
            if best is None or sc < best[0]:
                best = (sc, row, ab)
        rows[u] = best[1] + ('hergeleitet, Anker %s' % best[2],)
        log('  %04X.%d rect %d %s  Anker-Abstaende %s' % (u[0], u[1], o[0], best[1][:6], best[2]))
    return rows, P, bb, skala


def marken(pg, rows, P, orte):
    out = []
    gesehen = set()
    for (a, b, d, e) in P:
        if (b, a, e['slot'], d['slot']) in gesehen:
            continue
        gesehen.add((a, b, d['slot'], e['slot']))
        p1 = proj(d['lx'], d['lz'], rows[a])
        p2 = proj(e['lx'], e['lz'], rows[b])
        mx = (p1[0] + p2[0]) // 2
        my = (p1[1] + p2[1]) // 2
        luecke = max(abs(p1[0] - p2[0]), abs(p1[1] - p2[1]))
        # EINE Marke je Durchgang, auf die GEMEINSAME KANTE der beiden Rechtecke geklemmt
        # (Ueberdeckung der Kaesten, wie die STAGE1-Marken - Pin unit_map_durchgang "[Kante]").
        # Liegt sie dort, traegt sie auch das Rechteck des Partners -> auf_partner = 1:
        # sichtbar, sobald EINE der beiden Seiten besucht ist (re15_map_mark_get).
        ax, ay, aw, ah = rects(pg)[orte[a][0]]
        bx, by, bw, bh = rects(pg)[orte[b][0]]
        ux0, ux1 = max(ax, bx), min(ax + aw, bx + bw) - 1
        uy0, uy1 = max(ay, by), min(ay + ah, by + bh) - 1
        partner = 0
        if ux0 <= ux1 and uy0 <= uy1:
            mx = min(max(mx, ux0), ux1)
            my = min(max(my, uy0), uy1)
            partner = 1
        # Wandseite = Richtung zum NACHBARN (Mitte seines Kastens), 0=N 1=O 2=S 3=W.
        # Achse: die gemeinsame Kante ist duenn in EINER Richtung - dort liegt die Wand.
        vx = bx + bw // 2 - mx
        vy = by + bh // 2 - my
        if partner and (ux1 - ux0) != (uy1 - uy0):
            senkrecht = (ux1 - ux0) < (uy1 - uy0)
        else:
            senkrecht = abs(vx) >= abs(vy)
        if senkrecht:
            kind = 1 if vx > 0 else 3
        else:
            kind = 2 if vy > 0 else 0
        out.append((pg, orte[a][0], mx, my, kind, orte[a][4], orte[b][4], partner,
                    '%04X.%d<->%04X.%d' % (a[0], a[1], b[0], b[1]), luecke, symbol(pg, (mx, my))))
    # VARIANTEN-RAEUME teilen die zid (Regel v0.3.96, ROOM5030/5110): dieselbe Marke nur einmal.
    einmal = []
    for m in out:
        if not any(m[:8] == n[:8] for n in einmal):
            einmal.append(m)
    out = einmal
    # Tueren zu Orten AUSSERHALB dieses Blatts (2000->1260 Leitern, 3000->11A0, 3080->4000):
    # eine ungepaarte Marke (zid2 255) an der eigenen Projektion, an die naechste Kante geklemmt.
    raeume = {r for r, _ in orte}
    bb = zonen_bbox()
    for u, o in orte.items():
        if o[3]:
            bb[u] = o[3]
    for u in sorted(orte):
        if u not in rows:
            continue
        gesehen_p = set()
        for d in tueren(u[0]):
            if d['dest'] in raeume or d['dest'] == u[0] or d['sce'] == 0:
                continue
            if ort_von(orte, bb, u[0], d['lx'], d['lz']) != u:
                continue
            x, y, w, h = rects(pg)[orte[u][0]]
            mx, my = proj(d['lx'], d['lz'], rows[u])
            mx = min(max(mx, x), x + w - 1)
            my = min(max(my, y), y + h - 1)
            if (mx, my) in gesehen_p:
                continue
            gesehen_p.add((mx, my))
            dl, dr, do, du = abs(mx - x), abs(x + w - 1 - mx), abs(my - y), abs(y + h - 1 - my)
            k = min(dl, dr, do, du)
            kind = 3 if k == dl else 1 if k == dr else 0 if k == do else 2
            out.append((pg, orte[u][0], mx, my, kind, orte[u][4], 255, 0,
                        '%04X.%d->%04X (anderes Blatt)' % (u[0], u[1], d['dest']), 0, symbol(pg, (mx, my))))
    return out


if __name__ == '__main__':
    pg = int(sys.argv[1])
    plan = PLAENE[pg]
    rows, P, bb, skala = loese(pg)
    print('Blattmassstab', skala)
    for u in sorted(rows):
        o = plan['orte'][u]
        r = rows[u]
        x0, z0, x1, z1 = bb[u]
        print('    { 0x%04X, %6d, %6d, %6d, %6d, %2d, %3d, %d, %3d, %5d, %5d, %5d, %5d, %d, %d,   0, 0 },'
              '   /* %s; %s */' % (u[0], x0, z0, x1, z1, pg, o[0], u[1], o[4], r[0], r[1], r[2], r[3],
                                  r[4], r[5], o[5], r[6]))
    gaps = []
    for m in marken(pg, rows, P, plan['orte']):
        gaps.append(m[9])
        print('    { %2d, %2d, %4d, %4d, %d, %3d, %3d, %d },   /* %s Luecke %d px, Symbol %s px */' % m)
    if gaps:
        print('Tuerpaare', len(gaps), 'Luecke Median', statistics.median(gaps), 'max', max(gaps))
