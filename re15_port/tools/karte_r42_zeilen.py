# -*- coding: utf-8 -*-
"""KARTE r42: Zeilen, Tuer- und Treppenmarken fuer STAGE2 (Blatt 6) und STAGE3 (Blatt 7).

Runde 2 (Pruefbericht r42 Stufe 1 Runde 1: Spiegelungen aus je EINER Tuer gewaehlt, selbstmessende
Kennzahl, Treppenmarken geloescht). Neu gefasst:

PROJEKTION = Original-Formel FUN_800473f8 @0x8004741c-0x80047528
    mx = +-((((a+32000)*10*sx)>>20)+5)/10 + ox ,  my = -+((((b+32000)*10*sy)>>20)+5)/10 + oy
mit a = Welt-x, b = Welt-z (ausgeliefert, feste Vorzeichen). PORT-ERGAENZUNG fuer Stub-Raeume:
Vorzeichen flip_x/flip_z und ACHSENTAUSCH (re15_room.h Feld tausch: a = Welt-z, b = Welt-x).

DREHUNG statt Spiegel. Ein Raum kann gegen das Blatt nur GEDREHT liegen (die ausgelieferten Zeilen
haben feste Vorzeichen, also dieselbe Haendigkeit): R0 (0,0), R180 (1,1), R90cw = Nord->Ost
(Tausch, 0,1), R90ccw = Nord->West (Tausch, 1,0). Je hergeleitetem Ort steht die Drehung im PLAN
und wird hier GEPRUEFT: jede Tuer gibt ihre Weltseite (Trigger-Form + Lage in der Zonen-Bbox,
gegengeprueft am Ankunfts-Yaw des Gegen-Datensatzes) und - wo vorhanden - unabhaengige Kartenseiten:
  (a) GEMALTES Tuersymbol an der Grenze zum Nachbar-Rechteck (analysis/kartensymbole/symbolkatalog.csv),
  (b) Projektion der Gegen-Tuer mit der AUSGELIEFERTEN Zeile des Nachbarn (@0x800768b0 + 8*Slot),
  (c) Lage des Nachbar-Rechtecks (Seitentabelle @0x80076840).
Gefordert: mindestens ZWEI Stimmen fuer die Drehung (aus zwei Tueren oder zwei Belegarten), KEINE
dagegen. Die Tuerdaten waehlen nur diese diskrete Drehung; Versatz und Massstab kommen allein aus der
Kollisions-Bbox und der gemalten Flaeche (zeile_rot). Pruefgroessen danach (NICHT im Fit):
Abstand Tuer-Projektion -> gemaltes Tuersymbol, Treppen-Projektion -> gemaltes Treppensymbol.

TUERMARKEN: je Durchgang EINE Marke. Hat das Original die Tuer GEMALT (Katalogsymbol an der Grenze
der beiden Rechtecke, Zuordnung ueber die Nachbarschaft), sitzt die Marke auf dessen Wandlinie wie in
STAGE1 (W: x-1, O: x+w, N: y-1, S: y+h; gemessen an Blatt 1/2/4) - sonst stuenden zwei Tuerzeichen
auf dem Blatt (Pruefbericht R4: 3040<->3060). Ohne gemaltes Symbol: Mitte der beiden Projektionen,
auf die gemeinsame Kante geklemmt.

TREPPENMARKEN: aus den SCD-Zonen Aot_set Typ 12/13 (read_rdt; sce 12 = X-Achse, 13 = Z-Achse,
Handler LAB_80043500 / LAB_800435cc), Enden gepaart wie gen_map_zones.py (gleiche Achse, gleiche
Stufenzahl, Bandabstand = count), Weltmitte mit der Zeile der Zone projiziert. Sprossen quer zur
Laufrichtung AUF DEM BLATT: X-Treppe ohne Tausch -> kind 5, mit Tausch -> kind 4.

Aufruf: python re15_port/tools/karte_r42_zeilen.py <6|7> [belege]
"""
import io, os, re, sys, csv, math, statistics

HIER = os.path.dirname(os.path.abspath(__file__))
_s = io.open(os.path.join(HIER, 'gen_map_zones.py'), encoding='utf-8').read()
G = {'__name__': 'k42', '__file__': os.path.join(HIER, 'gen_map_zones.py')}
exec(compile(_s[:_s.index('def main(')], 'gen', 'exec'), G)
rects, read_rdt, u16, s16 = G['rects'], G['read_rdt'], G['u16'], G['s16']
BASES = [0, 38, 50, 65, 77, 98]

# Drehung -> (flip_x, flip_z, tausch) und Abbildung der Weltseite auf die Kartenseite.
ROT = {'R0': (0, 0, 0), 'R180': (1, 1, 0), 'R90cw': (0, 1, 1), 'R90ccw': (1, 0, 1)}
SEITE = {'R0': {'E': 'E', 'N': 'N', 'W': 'W', 'S': 'S'}, 'R180': {'E': 'W', 'N': 'S', 'W': 'E', 'S': 'N'},
         'R90cw': {'N': 'E', 'E': 'S', 'S': 'W', 'W': 'N'}, 'R90ccw': {'N': 'W', 'W': 'S', 'S': 'E', 'E': 'N'}}
GEGEN = {'N': 'S', 'S': 'N', 'E': 'W', 'W': 'E'}


def zeile(rid):
    st = (rid >> 12) - 1
    slot = BASES[st] + ((rid >> 4) & 0xFF)
    va = 0x800768b0 + 8 * slot
    return s16(va), s16(va + 2), u16(va + 4), u16(va + 6), va


def sf(v, s):
    return ((((v + 32000) * 10 * s) >> 20) + 5) // 10


def proj(x, z, row):
    ox, oy, sx, sy, fx, fz, ts = row[:7]
    a = sf(z if ts else x, sx)
    b = sf(x if ts else z, sy)
    return ((-a if fx else a) + ox, (b if fz else -b) + oy)


def gemalt(pg, r):
    s = G['_kunst_flaeche'](pg, r)
    xs = [p[0] for p in s]
    ys = [p[1] for p in s]
    return min(xs), min(ys), max(xs), max(ys)


def zeile_rot(bbox, ziel, rot, skala):
    """Kollisions-Bbox mittig in den Zielkasten, Massstab je KARTEN-Achse hoechstens der des Blatts."""
    fx, fz, ts = ROT[rot]
    x0, z0, x1, z1 = bbox
    a0, a1 = (z0, z1) if ts else (x0, x1)
    b0, b1 = (x0, x1) if ts else (z0, z1)
    X0, Y0, X1, Y1 = ziel
    sx = min(skala[0], round((X1 - X0) * 2 ** 20 / max(1, a1 - a0)))
    sy = min(skala[1], round((Y1 - Y0) * 2 ** 20 / max(1, b1 - b0)))
    ca, cb = (a0 + a1) // 2, (b0 + b1) // 2
    ox = round((X0 + X1) / 2 - (-sf(ca, sx) if fx else sf(ca, sx)))
    oy = round((Y0 + Y1) / 2 - (sf(cb, sy) if fz else -sf(cb, sy)))
    return (ox, oy, sx, sy, fx, fz, ts)


# ---- gemalte Symbole (Katalog) ---------------------------------------------------------------
def _katalog():
    p = os.path.join(HIER, '..', '..', 'analysis', 'kartensymbole', 'symbolkatalog.csv')
    out = []
    for row in csv.DictReader(io.open(p, encoding='utf-8')):
        x, y, w, h = int(row['screen_x']), int(row['screen_y']), int(row['px_w']), int(row['px_h'])
        out.append(dict(pg=int(row['seite']), rect=int(row['rect']), typ=row['typ'], wand=row['wand'].strip(),
                        x=x, y=y, w=w, h=h, cx=x + w // 2, cy=y + h // 2))
    return out
KATALOG = _katalog()


def wandpunkt(s, wand=None):
    """Lage der Tuer auf der WANDLINIE neben dem gemalten Symbol (STAGE1: W x-1, O x+w, N y-1, S y+h)."""
    w = wand or s['wand']
    if w == 'W':
        return s['x'] - 1, s['cy'], 3
    if w == 'E':
        return s['x'] + s['w'], s['cy'], 1
    if w == 'N':
        return s['cx'], s['y'] - 1, 0
    return s['cx'], s['y'] + s['h'], 2


# ---- PLAENE ---------------------------------------------------------------------------
# Ort: dict(rect, quelle, rot, zid, notiz, ziel=None, bbox=None, fit=None)
#   quelle 'echt' = ausgelieferte Zeile (R0 per Definition), 'wie:<rid>.<z>' = Zeile eines Orts im
#   selben Rahmen, 'min' = hergeleitet (zeile_rot). ziel = Zielkasten auf dem Blatt (sonst gemalte
#   Flaeche des Rechtecks), bbox = Zonen-Kasten im Header (sonst unveraendert), fit = Kasten nur fuer
#   die Zeile (Zonen-Kasten bleibt).
def O(rect, quelle, zid, notiz, rot=None, **kw):
    d = dict(rect=rect, quelle=quelle, zid=zid, notiz=notiz, rot=rot or 'R0', ziel=None, bbox=None, fit=None)
    d.update(kw)
    return d


PLAENE = {
    6: dict(skala=(2340, 2308), orte={
        (0x2000, 0): O(0, 'echt', 37, 'U-Kanal'),
        (0x2010, 0): O(1, 'min', 38, 'Westraum', 'R180'),
        (0x2020, 0): O(4, 'min', 113, 'Kanal r4', 'R180', bbox=(-23000, -5850, 15100, 5300)),
        (0x2030, 0): O(6, 'min', 39, 'Raum mit Achteck', 'R180'),
        (0x2040, 0): O(5, 'min', 40, 'langer Kanal r5', 'R180', bbox=(-21500, -8300, 25750, 4350)),
        (0x2050, 0): O(2, 'min', 41, 'Raum oben links', 'R180'),
        (0x2060, 0): O(7, 'min', 42, 'Raum r7', 'R180'),
        (0x2070, 0): O(3, 'wie:2000.0', 43, 'Gang 2000-2030 im Rahmen von ROOM2000'),
        (0x2070, 1): O(3, 'min', 44, 'Gang-Kopie ohne Spawn (unerreichbar)', 'R180'),
        (0x2080, 0): O(8, 'min', 46, 'Schacht r8', 'R0', ziel=(227, 131, 240, 179)),
        (0x2080, 1): O(8, 'min', 47, 'Querstueck r8', 'R0', ziel=(193, 151, 227, 158)),
        (0x2090, 0): O(9, 'min', 48, 'Raum r9', 'R0'),
        (0x20A0, 0): O(10, 'min', 114, 'PORT-WAHL r10', 'R90ccw', bbox=(-5600, -28900, 29000, 7000),
                       port_wahl='Blatt 0xD ohne Kunst; eine Tuer aufs Blatt, Nordwand (Trigger 3000x1000 + '
                                 'Ankunfts-Yaw) -> Westseite von r10: die einzige DREHUNG, die das abbildet'),
    }),
    7: dict(skala=None, orte={
        (0x3000, 0): O(0, 'echt', 49, ''),
        (0x3010, 0): O(1, 'echt', 50, ''),
        (0x3020, 0): O(2, 'min', 51, '', 'R0'),
        (0x3030, 0): O(3, 'echt', 52, ''),
        (0x3040, 0): O(4, 'echt', 53, ''),
        (0x3050, 0): O(5, 'min', 54, 'Vierteldrehung', 'R90cw'),
        (0x3060, 0): O(6, 'echt', 55, ''),
        (0x3070, 0): O(7, 'echt', 56, ''),
        (0x3080, 0): O(8, 'echt', 57, ''),
        (0x3090, 0): O(9, 'echt', 58, ''),
        (0x30A0, 0): O(10, 'min', 59, 'Vierteldrehung', 'R90ccw'),
        (0x30B0, 0): O(11, 'min', 60, '', 'R0'),
        (0x30C0, 0): O(12, 'min', 61, '', 'R0'),
        (0x30D0, 0): O(2, 'min', 51, 'Suedteil = Variante von 3020', 'R0', bbox=(-27200, -30618, -5300, -11418)),
        (0x30D0, 1): O(12, 'min', 61, 'Nordteil = Variante von 30C0', 'R0', bbox=(-27200, -14300, -5275, 1650)),
        # 30E0 Zone 0 = L-Gang 3050 -> 3030. Die AUSGELIEFERTE 30E0-Zeile @0x80076AB0 gilt fuer Zone 1
        # (Tueren ->3010/->3070 auf den r13-Symbolen); fuer Zone 0 legt sie die Tueren auf (163,122) /
        # (170,128), 30 bzw. 38 px neben die Gegenstuecke (gemaltes (187,98) E in r5 und die
        # ausgelieferte 3030-Projektion (208,106)). Gemalt ist der Gang als Suedband von r4 (Innen
        # y93..103 zwischen der Innenlinie y92 und der Suedwand y104, x192..245) - dort enden beide.
        (0x30E0, 0): O(4, 'min', 63, 'Suedband von r4 (geteilt mit ROOM3040)', 'R0',
                       ziel=(192, 93, 245, 103), fit=(1775, 250, 29450, 9150)),
        (0x30E0, 1): O(13, 'echt', 64, ''),
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


def treppen(rid):
    r = read_rdt(rid)
    return r[2] if r else []


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


def gegen_tuer(R, d):
    """Der reziproke Datensatz im Zielraum (Spawn hier am eigenen Trigger und umgekehrt)."""
    best = None
    for e in tueren(d['dest']):
        if e['dest'] != R:
            continue
        dd = (abs(e['nx'] - d['lx']) + abs(e['nz'] - d['lz']) + abs(d['nx'] - e['lx']) + abs(d['nz'] - e['lz']))
        if dd <= 8000 and (best is None or dd < best[0]):
            best = (dd, e)
    return best[1] if best else None


def paare(orte, bb):
    raeume = sorted({r for r, _ in orte})
    out = []
    for R in raeume:
        for d in tueren(R):
            N = d['dest']
            if N == R or N not in raeume:
                continue
            e = gegen_tuer(R, d)
            if not e:
                continue
            u = ort_von(orte, bb, R, d['lx'], d['lz'])
            v = ort_von(orte, bb, N, e['lx'], e['lz'])
            if u and v:
                out.append((u, v, d, e))
    return out


def kasten(orte):
    bb = zonen_bbox()
    for u, o in orte.items():
        if o['bbox']:
            bb[u] = o['bbox']
    return bb


# ---- Belege fuer die Drehung --------------------------------------------------------------------
def yawseite(yaw):
    a = (yaw % 4096) / 4096 * 2 * math.pi
    fx, fz = math.cos(a), -math.sin(a)          # 0 = +x, 1024 = -z (gemessen an 3010/3060/3090)
    if abs(fx) >= abs(fz):
        return 'W' if fx > 0 else 'E'            # die Tuer liegt HINTER dem Ankommenden
    return 'S' if fz > 0 else 'N'


def weltseite(box, d, e):
    x0, z0, x1, z1 = box
    cx, cz = (x0 + x1) / 2, (z0 + z1) / 2
    if d['rw'] < d['rd']:
        s = 'W' if d['lx'] < cx else 'E'
    elif d['rd'] < d['rw']:
        s = 'S' if d['lz'] < cz else 'N'
    else:
        n = {'W': (d['lx'] - x0) / (x1 - x0), 'E': (x1 - d['lx']) / (x1 - x0),
             'S': (d['lz'] - z0) / (z1 - z0), 'N': (z1 - d['lz']) / (z1 - z0)}
        s = min(n, key=n.get)
    y = yawseite(e['yaw']) if e else None
    if d['rw'] == d['rd'] and y:
        # QUADRATISCHER Trigger sagt nichts ueber die Wandachse (2050->2000, 2080->2090 sitzen in
        # einer Ecke): dann gilt die Blickrichtung des Ankommenden aus dem Gegen-Datensatz.
        return y, y
    return s, y


def kartenseite(R, p, tol=6):
    x, y, w, h = R
    d = {'W': abs(p[0] - x), 'E': abs(p[0] - (x + w - 1)), 'N': abs(p[1] - y), 'S': abs(p[1] - (y + h - 1))}
    s = min(d, key=d.get)
    inn = x - tol <= p[0] <= x + w - 1 + tol and y - tol <= p[1] <= y + h - 1 + tol
    return (s if d[s] <= tol and inn else None), d[s]


def grenz_symbole(pg, ra, rb, treppe=False):
    """Gemalte Tuersymbole (mit treppe=True auch Treppensymbole) auf der Grenze zwischen Rechteck ra
    und rb (Ueberdeckung der Kaesten +-4)."""
    A, B = rects(pg)[ra], rects(pg)[rb]
    ux0, ux1 = max(A[0], B[0]) - 4, min(A[0] + A[2], B[0] + B[2]) + 3
    uy0, uy1 = max(A[1], B[1]) - 4, min(A[1] + A[3], B[1] + B[3]) + 3
    out = []
    for s in KATALOG:
        if s['pg'] != pg or len(s['wand']) != 1:
            continue
        if not (s['typ'].startswith('TUER') or (treppe and s['typ'] == 'TREPPE')):
            continue
        if s['rect'] not in (ra, rb):
            continue
        if ux0 <= s['cx'] <= ux1 and uy0 <= s['cy'] <= uy1:
            out.append(s)
    return out


ECKE = {'R0': {'NE': 'NE', 'NW': 'NW', 'SE': 'SE', 'SW': 'SW'}, 'R180': {'NE': 'SW', 'NW': 'SE', 'SE': 'NW', 'SW': 'NE'},
        'R90cw': {'NE': 'SE', 'SE': 'SW', 'SW': 'NW', 'NW': 'NE'}, 'R90ccw': {'NE': 'NW', 'NW': 'SW', 'SW': 'SE', 'SE': 'NE'}}


def weltecke(box, d):
    x0, z0, x1, z1 = box
    fx, fz = (d['lx'] - x0) / (x1 - x0), (d['lz'] - z0) / (z1 - z0)
    if min(fx, 1 - fx) < 0.3 and min(fz, 1 - fz) < 0.3:
        return ('N' if fz > 0.5 else 'S') + ('E' if fx > 0.5 else 'W')
    return None


def kartenecke(R, p, tol=6):
    x, y, w, h = R
    dx = min(abs(p[0] - x), abs(p[0] - (x + w - 1)))
    dy = min(abs(p[1] - y), abs(p[1] - (y + h - 1)))
    if dx <= tol and dy <= tol:
        return (('N' if abs(p[1] - y) < abs(p[1] - (y + h - 1)) else 'S') +
                ('W' if abs(p[0] - x) < abs(p[0] - (x + w - 1)) else 'E'))
    return None


def passt(welt, karte):
    """Drehungen, die die Welt-Angabe (Seite 'N' oder Ecke 'NE') auf die Karten-Angabe abbilden."""
    out = []
    for r in SEITE:
        if len(welt) == 1:
            bild = SEITE[r][welt]
        else:
            bild = ECKE[r][welt]
        if len(karte) == 1 and karte in bild:
            out.append(r)
        elif len(karte) == 2 and (bild == karte or (len(bild) == 1 and bild in karte)):
            out.append(r)
    return out


def belege(pg, u, orte, bb):
    """-> Liste (Tuer, Welt-Angabe, Belegart, Karten-Angabe, Drehungen) fuer den Ort u.
    Welt-Angabe = Wandseite der Tuer (Trigger-Form + Lage, gegengeprueft am Ankunfts-Yaw), bei einem
    QUADRATISCHEN Trigger in einer Ecke der Zonen-Bbox die Ecke (z.B. 'SE').
    Karten-Stimmen: (a) gemaltes TUERsymbol, das NUR an der Grenze zu diesem einen Nachbarn liegt
    (Treppensymbole zaehlen nicht: ihr Wandbuchstabe sagt, an welcher Wand die Treppe GEMALT ist, nicht
    wohin der Durchgang fuehrt - (99,103) W in r0 liegt an der Westwand, 2000->2050 geht nach Norden);
    (b) Gegen-Tuer mit AUSGELIEFERTER Zeile (die Zeile ist ungedreht, also gibt die Trigger-Form der
    Gegen-Tuer die Wandachse AUF DEM BLATT; die Seite ist die naehere Kante des eigenen Rechtecks);
    (c) Lage des Nachbar-Rechtecks (nicht, wenn eines das andere fast ganz enthaelt)."""
    o = orte[u]
    R = rects(pg)[o['rect']]
    nb_rects = {}
    for d in tueren(u[0]):
        for k, v in orte.items():
            if k[0] == d['dest'] and v['rect'] != o['rect']:
                nb_rects.setdefault(v['rect'], set()).add(d['dest'])
    out = []
    for d in tueren(u[0]):
        if ort_von(orte, bb, u[0], d['lx'], d['lz']) != u or d['dest'] == u[0]:
            continue
        e = gegen_tuer(u[0], d)
        ws, ys = weltseite(bb[u], d, e)
        if ys and ys != ws:
            out.append((d, ws + '/' + ys, '-', None, None))
            continue
        welt = ws
        if d['rw'] == d['rd']:
            ecke = weltecke(bb[u], d)
            if ecke and ws in ecke:
                welt = ecke
        if e:
            z = zeile(d['dest'])
            if z[2] > 1:
                p = proj(e['lx'], e['lz'], (z[0], z[1], z[2], z[3], 0, 0, 0))
                x, y, w, h = R
                if x - 6 <= p[0] <= x + w + 5 and y - 6 <= p[1] <= y + h + 5:
                    if e['rd'] < e['rw']:
                        ms = 'N' if abs(p[1] - y) < abs(p[1] - (y + h - 1)) else 'S'
                    elif e['rw'] < e['rd']:
                        ms = 'W' if abs(p[0] - x) < abs(p[0] - (x + w - 1)) else 'E'
                    else:
                        ms = kartenecke(R, p) or kartenseite(R, p)[0]
                    if ms:
                        out.append((d, welt, '(b) %04X ausgeliefert @0x%08X -> (%d,%d), Gegen-Trigger %dx%d' % (
                            d['dest'], z[4], p[0], p[1], e['rw'], e['rd']), ms, passt(welt, ms)))
        nb = [(v['rect'], k) for k, v in orte.items() if k[0] == d['dest']]
        for rn, k in nb:
            if e and ort_von(orte, bb, d['dest'], e['lx'], e['lz']) not in (k, None):
                continue
            for s2 in grenz_symbole(pg, o['rect'], rn):
                andere = [r2 for r2 in nb_rects if r2 != rn and s2 in grenz_symbole(pg, o['rect'], r2)]
                if andere:
                    continue                     # Symbol an der Grenze zu mehreren Nachbarn: keine Stimme
                ms = s2['wand'] if s2['rect'] == o['rect'] else GEGEN[s2['wand']]
                out.append((d, welt, '(a) gemalt %s (%d,%d) %s auf r%d' % (s2['typ'], s2['cx'], s2['cy'], s2['wand'], s2['rect']),
                            ms, passt(welt, ms)))
            if rn != o['rect']:
                N = rects(pg)[rn]
                ux0, ux1 = max(R[0], N[0]), min(R[0] + R[2], N[0] + N[2])
                uy0, uy1 = max(R[1], N[1]), min(R[1] + R[3], N[1] + N[3])
                if ux1 - ux0 <= 0 or uy1 - uy0 <= 0:
                    continue
                if (ux1 - ux0) * (uy1 - uy0) * 5 >= min(R[2] * R[3], N[2] * N[3]) * 4:
                    continue                     # ein Rechteck (fast) im anderen: die Lage sagt keine Seite
                if (ux1 - ux0) < (uy1 - uy0):
                    ms = 'W' if ux0 - R[0] < R[0] + R[2] - ux1 else 'E'
                else:
                    ms = 'N' if uy0 - R[1] < R[1] + R[3] - uy1 else 'S'
                out.append((d, welt, '(c) Nachbar-Rechteck r%d' % rn, ms, passt(welt, ms)))
    return out


def pruefe_drehung(pg, rid, orte, bb, log=print):
    """Drehung je RAUM (alle Zonen eines RDT teilen das Koordinatensystem)."""
    us = sorted(u for u in orte if u[0] == rid and orte[u]['quelle'] == 'min')
    rots = {orte[u]['rot'] for u in us}
    assert len(rots) == 1, 'ROOM%04X: Zonen mit verschiedener Drehung %s' % (rid, rots)
    rot = rots.pop()
    bl = []
    for u in us:
        bl += belege(pg, u, orte, bb)
    dafuer = [b for b in bl if b[4] and rot in b[4]]
    dagegen = [b for b in bl if b[4] and rot not in b[4]]
    tueren_dafuer = {(b[0]['dest'], b[0]['lx'], b[0]['lz']) for b in dafuer}
    arten = {b[2][:3] for b in dafuer}
    schnitt = set(SEITE)
    for b in bl:
        if b[4]:
            schnitt &= set(b[4])
    ok = len(dafuer) >= 2 and (len(tueren_dafuer) >= 2 or len(arten) >= 2) and not dagegen and schnitt == {rot}
    port = [orte[u].get('port_wahl') for u in us if orte[u].get('port_wahl')]
    log('  ROOM%04X (%s) %s: %d dafuer (%d Tueren, Arten %s), %d dagegen -> %s' % (
        rid, ','.join('z%d r%d' % (u[1], orte[u]['rect']) for u in us), rot, len(dafuer), len(tueren_dafuer),
        ''.join(sorted(arten)), len(dagegen), 'BELEGT' if ok else ('PORT-WAHL: ' + port[0]) if port else 'NICHT BELEGT'))
    for b in bl:
        d = b[0]
        log('      ->%04X trig(%d,%d) %dx%d Welt %s | %s | Karte %s -> %s' % (
            d['dest'], d['lx'], d['lz'], d['rw'], d['rd'], b[1], b[2], b[3], b[4]))
    return ok, dafuer, dagegen


def loese(pg, log=print):
    plan = PLAENE[pg]
    orte = plan['orte']
    bb = kasten(orte)
    skala = plan['skala']
    rows = {}
    for u, o in orte.items():
        if o['quelle'] == 'echt':
            z = zeile(u[0])
            assert z[2] > 1, '%04X ohne ausgelieferte Zeile' % u[0]
            rows[u] = (z[0], z[1], z[2], z[3], 0, 0, 0, 'ausgeliefert @0x%08X' % z[4])
    if skala is None:
        skala = (int(statistics.median(r[2] for r in rows.values())),
                 int(statistics.median(r[3] for r in rows.values())))
    for u, o in orte.items():
        if o['quelle'].startswith('wie:'):
            a, b = o['quelle'][4:].split('.')
            rows[u] = rows[(int(a, 16), int(b))][:7] + ('selber Rahmen wie ROOM%s' % a,)
    for u, o in sorted(orte.items()):
        if u in rows:
            continue
        ziel = o['ziel'] or gemalt(pg, o['rect'])
        row = zeile_rot(o['fit'] or bb[u], ziel, o['rot'], skala)
        rows[u] = row + ('hergeleitet %s' % o['rot'],)
    return rows, paare(orte, bb), bb, skala


def projektion_pruefung(pg, rows, P, orte):
    """Abstand der Tuer-Projektionen zum gemalten Tuersymbol desselben Durchgangs (nicht im Fit)."""
    out = []
    gesehen = set()
    for (a, b, d, e) in P:
        if (b, a, e['slot'], d['slot']) in gesehen:
            continue
        gesehen.add((a, b, d['slot'], e['slot']))
        syms = grenz_symbole(pg, orte[a]['rect'], orte[b]['rect'])
        p1 = proj(d['lx'], d['lz'], rows[a])
        p2 = proj(e['lx'], e['lz'], rows[b])
        for s in syms:
            wx, wy, _ = wandpunkt(s)
            out.append(('%04X.%d<->%04X.%d' % (a[0], a[1], b[0], b[1]), (wx, wy),
                        max(abs(p1[0] - wx), abs(p1[1] - wy)), max(abs(p2[0] - wx), abs(p2[1] - wy)),
                        rows[a][7][:5], rows[b][7][:5]))
    return out


def marken(pg, rows, P, orte, bb):
    out = []
    gesehen = set()
    belegt = set()
    kand = []
    for (a, b, d, e) in P:
        if (b, a, e['slot'], d['slot']) in gesehen:
            continue
        gesehen.add((a, b, d['slot'], e['slot']))
        p1 = proj(d['lx'], d['lz'], rows[a])
        p2 = proj(e['lx'], e['lz'], rows[b])
        mx, my = (p1[0] + p2[0]) // 2, (p1[1] + p2[1]) // 2
        syms = grenz_symbole(pg, orte[a]['rect'], orte[b]['rect'])
        kand.append((a, b, d, e, p1, p2, mx, my, syms))
    # gemalte Symbole den Durchgaengen zuordnen: je Symbol der naechste Durchgang (gierig, 1:1)
    paarung = []
    for i, k in enumerate(kand):
        for s in k[8]:
            paarung.append((max(abs(s['cx'] - k[6]), abs(s['cy'] - k[7])), i, id(s), s))
    zu = {}
    for dist, i, sid, s in sorted(paarung, key=lambda t: t[0]):
        if i in zu or sid in belegt or dist > 30:
            continue
        zu[i] = s
        belegt.add(sid)
    # VARIANTEN-RAEUME (Regel v0.3.96: ROOM30D0 = 3020 + 30C0, gleiche Zonen-Nummern) zeigen denselben
    # Durchgang: gleiches Zonen-Paar -> dasselbe gemalte Symbol.
    for i, k in enumerate(kand):
        if i in zu:
            continue
        paar = {orte[k[0]]['zid'], orte[k[1]]['zid']}
        for i2, s in list(zu.items()):
            if {orte[kand[i2][0]]['zid'], orte[kand[i2][1]]['zid']} == paar and s in k[8]:
                zu[i] = s
                break
    for i, (a, b, d, e, p1, p2, mx, my, syms) in enumerate(kand):
        luecke = max(abs(p1[0] - p2[0]), abs(p1[1] - p2[1]))
        ax, ay, aw, ah = rects(pg)[orte[a]['rect']]
        bx, by, bw, bh = rects(pg)[orte[b]['rect']]
        if i in zu:
            s = zu[i]
            mx, my, kind = wandpunkt(s)
            # Traeger = der Ort, dessen Rechteck das Symbol malt (sonst schwebt die Marke)
            if s['rect'] != orte[a]['rect']:
                a, b = b, a
                ax, ay, aw, ah, bx, by, bw, bh = bx, by, bw, bh, ax, ay, aw, ah
            herkunft = 'gemaltes Symbol (%d,%d) %s r%d' % (s['cx'], s['cy'], s['wand'], s['rect'])
        else:
            ux0, ux1 = max(ax, bx), min(ax + aw, bx + bw) - 1
            uy0, uy1 = max(ay, by), min(ay + ah, by + bh) - 1
            if ux0 <= ux1 and uy0 <= uy1:
                mx = min(max(mx, ux0), ux1)
                my = min(max(my, uy0), uy1)
            vx, vy = bx + bw // 2 - mx, by + bh // 2 - my
            if ux0 <= ux1 and uy0 <= uy1 and (ux1 - ux0) != (uy1 - uy0):
                senkrecht = (ux1 - ux0) < (uy1 - uy0)
            else:
                senkrecht = abs(vx) >= abs(vy)
            kind = (1 if vx > 0 else 3) if senkrecht else (2 if vy > 0 else 0)
            herkunft = 'Mitte der Projektionen (kein gemaltes Symbol)'
        partner = 1 if (bx <= mx < bx + bw and by <= my < by + bh) else 0
        out.append((pg, orte[a]['rect'], mx, my, kind, orte[a]['zid'], orte[b]['zid'], partner,
                    '%04X.%d<->%04X.%d' % (a[0], a[1], b[0], b[1]), luecke, herkunft))
    einmal = []
    for m in out:
        if not any(m[:8] == n[:8] for n in einmal):
            einmal.append(m)
    out = einmal
    # Tueren auf ein anderes Blatt: ungepaarte Marke an der eigenen Projektion, an die naechste Kante.
    raeume = {r for r, _ in orte}
    for u in sorted(orte):
        if u not in rows:
            continue
        gesehen_p = set()
        for d in tueren(u[0]):
            if d['dest'] in raeume or d['dest'] == u[0] or d['sce'] == 0:
                continue
            if ort_von(orte, bb, u[0], d['lx'], d['lz']) != u:
                continue
            x, y, w, h = rects(pg)[orte[u]['rect']]
            mx, my = proj(d['lx'], d['lz'], rows[u])
            mx = min(max(mx, x), x + w - 1)
            my = min(max(my, y), y + h - 1)
            if (mx, my) in gesehen_p:
                continue
            gesehen_p.add((mx, my))
            dl, dr, do, du = abs(mx - x), abs(x + w - 1 - mx), abs(my - y), abs(y + h - 1 - my)
            k = min(dl, dr, do, du)
            kind = 3 if k == dl else 1 if k == dr else 0 if k == do else 2
            out.append((pg, orte[u]['rect'], mx, my, kind, orte[u]['zid'], 255, 0,
                        '%04X.%d->%04X (anderes Blatt)' % (u[0], u[1], d['dest']), 0, 'eigene Projektion'))
    return out


def treppen_marken(pg, rows, orte, bb):
    out = []
    for rid in sorted({r for r, _ in orte}):
        st = treppen(rid)
        merge, used, cand = {}, set(), []
        for i in range(len(st)):
            for j in range(i + 1, len(st)):
                A, B = st[i], st[j]
                if A['axis'] != B['axis'] or A['count'] != B['count']:
                    continue
                if abs(A['band'] - B['band']) != A['count']:
                    continue
                cand.append((abs(A['x'] - B['x']) + abs(A['z'] - B['z']), i, j))
        for dd, i, j in sorted(cand):
            if i in used or j in used:
                continue
            used |= {i, j}
            merge[i] = merge[j] = ((st[i]['x'] + st[j]['x']) // 2, (st[i]['z'] + st[j]['z']) // 2)
        punkte = []
        for i, s in enumerate(st):
            p = merge.get(i, (s['x'], s['z']))
            if (p, s['axis']) not in punkte:
                punkte.append((p, s['axis']))
        for (wx, wz), axis in punkte:
            # Zone: die enthaltende, sonst die naechste (wie re15_map_zone_at)
            u = ort_von(orte, bb, rid, wx, wz, slack=10 ** 9)
            if u is None or u not in rows:
                continue
            x0, z0, x1, z1 = bb[u]
            drin = x0 <= wx <= x1 and z0 <= wz <= z1
            mx, my = proj(wx, wz, rows[u])
            x, y, w, h = rects(pg)[orte[u]['rect']]
            if not drin and not (x <= mx < x + w and y <= my < y + h):
                out.append((None, rid, (wx, wz), axis, 'ausserhalb jeder Zone und des Rechtecks - OFFEN'))
                continue
            mx = min(max(mx, x), x + w - 1)
            my = min(max(my, y), y + h - 1)
            ts = rows[u][6]
            kind = (5 if axis == 12 else 4) if not ts else (4 if axis == 12 else 5)
            if any(t[0] == (pg, orte[u]['rect'], mx, my, kind, orte[u]['zid'], 255, 0) for t in out):
                continue                         # Varianten-Raum (30D0 = 30C0): dieselbe Marke nur einmal
            out.append(((pg, orte[u]['rect'], mx, my, kind, orte[u]['zid'], 255, 0), rid, (wx, wz), axis,
                        'Aot_set sce %d %s' % (axis, 'Paar gemittelt' if (wx, wz) in merge.values() else 'einzeln')))
    return out


if __name__ == '__main__':
    pg = int(sys.argv[1])
    plan = PLAENE[pg]
    orte = plan['orte']
    rows, P, bb, skala = loese(pg)
    print('Blattmassstab', skala)
    print('== DREHUNG (Belege, Pruefbericht R1: nur Drehungen, je zwei Stimmen)')
    for rid in sorted({u[0] for u in orte if orte[u]['quelle'] == 'min'}):
        pruefe_drehung(pg, rid, orte, bb)
    print('== ZEILEN')
    for u in sorted(rows):
        o = orte[u]
        r = rows[u]
        x0, z0, x1, z1 = bb[u]
        print('    { 0x%04X, %6d, %6d, %6d, %6d, %2d, %3d, %d, %3d, %5d, %5d, %5d, %5d, %d, %d,   0, 0, %d },'
              '   /* %s; %s */' % (u[0], x0, z0, x1, z1, pg, o['rect'], u[1], o['zid'], r[0], r[1], r[2], r[3],
                                  r[4], r[5], r[6], o['notiz'], r[7]))
    print('== TUERMARKEN')
    for m in marken(pg, rows, P, orte, bb):
        print('    { %2d, %2d, %4d, %4d, %d, %3d, %3d, %d },   /* %s, Luecke %d px, %s */' % m)
    print('== TREPPENMARKEN')
    for t in treppen_marken(pg, rows, orte, bb):
        if t[0] is None:
            print('    /* ROOM%04X Treppe (%d,%d) sce %d: %s */' % (t[1], t[2][0], t[2][1], t[3], t[4]))
        else:
            print('    { %2d, %2d, %4d, %4d, %d, %3d, %3d, %d },' % t[0],
                  '  /* ROOM%04X Treppe (%d,%d) %s */' % (t[1], t[2][0], t[2][1], t[4]))
    print('== PRUEFGROESSE (nicht im Fit): Tuer-Projektion -> gemaltes Symbol')
    for p in projektion_pruefung(pg, rows, P, orte):
        print('   %-22s Symbol-Wand %s  eigene %2d px  Gegenseite %2d px  (%s / %s)' % p)
