#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_map_zones.py — erzeugt engine/src/re15_map_zones.h: die KARTEN-ZONEN.

HINTERGRUND (Nutzer-Report 2026-08-30): "Schon im ersten Bereich room 1170 laufen wir
oben durch eine Tuer ... nach Durchlauf ist der Marker unten im grossen Rechteck statt
oben im kleinen, und das kleine ist nicht hervorgehoben."
URSACHE: Ein RDT-"Raum" ist nicht immer EIN Ort. ROOM1170 enthaelt zwei raeumlich
getrennte Bereiche, verbunden durch eine SELBST-TUER (`dest == eigener Raum`, Sprung
von z=+15250 nach z=-26500). Das alte Modell "ein Raum = ein Rechteck" kann so etwas
nicht darstellen: Marker und Hervorhebung blieben im Bereich, in dem man gestartet ist.
26 der 103 Basis-Raeume sind betroffen.

DAS ZONEN-MODELL: Jeder ZUSAMMENHAENGENDE Bereich der Kollisionsgeometrie ist eine
eigene Zone mit eigenem Karten-Rechteck und eigener Marker-Abbildung. Die aktive Zone
folgt der Spielerposition. Damit stimmen Marker, Hervorhebung und (weil Zonen auf
verschiedenen Seiten liegen duerfen) auch die Etage.

VERFAHREN
  1. SCA-Zellen je Raum (RDT +0x20) zu Bereichen verschmelzen (Beruehrung/Ueberlappung
     mit Toleranz GAP); Splitter unter 3 % der Raumflaeche verwerfen (Moebel).
  2. Je Karten-Seite die Zonen den Rechtecken zuordnen: Kosten aus Seitenverhaeltnis,
     Flaechen-Rang und Tuer-Graph (Tuer-Position und Eintritts-Position derselben Tuer
     muessen auf denselben Kartenpunkt fallen); Optimierung per Hill-Climbing mit
     Neustarts. Nicht zuordenbare Zonen bleiben leer (faerben sich nie falsch).
  3. Marker: lineare Abbildung Zonen-Bbox -> Rechteck, y gespiegelt (Welt-z waechst
     nach Sueden, Karten-y nach unten).

Aufruf:  python re15_port/tools/gen_map_zones.py [--json <zonen.json>]
"""
import itertools
import struct, json, os, sys, math, collections, random

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
EXE  = open(os.path.join(ROOT, 'info', 'Re1.5', 'PSX.EXE'), 'rb').read()
GAP  = 1500          # Beruehrungs-Toleranz beim Verschmelzen (Welt-Einheiten)
MIN_FRAC = 0.03      # Splitter unter diesem Anteil der Raumflaeche verwerfen

def fo(a): return a - 0x80010000 + 0x800
def u16(a): return struct.unpack_from('<H', EXE, fo(a))[0]
def s16(a): return struct.unpack_from('<h', EXE, fo(a))[0]
def u32(a): return struct.unpack_from('<I', EXE, fo(a))[0]

def rects(page):
    cnt = u16(0x80076840 + page * 8); ptr = u32(0x80076844 + page * 8)
    return [(s16(ptr + i*12), s16(ptr + i*12+2), s16(ptr + i*12+4), s16(ptr + i*12+6))
            for i in range(cnt)]

def rect_uv(page, i):
    """Kachel-Ecke (u,v) des Rechtecks: Byte +8 und +10 des 12-Byte-Eintrags."""
    ptr = u32(0x80076844 + page * 8)
    return EXE[fo(ptr + i*12 + 8)], EXE[fo(ptr + i*12 + 10)]

BASES = [0, 38, 50, 65, 77, 98]
# ---- KARTEN-KACHEL EINER SEITE (DATA/MAP0x.PIX) ---------------------------------
# Die Grundriss-Grafik ist die einzige verlaessliche Quelle fuer den Verlauf der WAENDE:
# ein Karten-Rechteck ist nur die Bounding-Box, der gezeichnete Raum darin kann
# L-foermig sein (ROOM1130 ist ein abgeknickter Flur). Wer eine Tuermarke auf die
# Rechteck-KANTE rueckt, setzt sie dort ausserhalb des Raums ab.
# id-Tabelle @0x80074c4c (u16 je Seite), id 12..24 -> MAP01..MAP0D; headerlos,
# 256x256 4bpp, 128 Byte je Zeile, unteres Nibble = linkes Pixel.
_PIX = {}
def page_pix(page):
    if page in _PIX: return _PIX[page]
    pid = u16(0x80074c4c + page * 2)
    img = None
    if 12 <= pid <= 24:
        fp = os.path.join(ROOT, 're15_port', 'shared_assets', 'PSX', 'DATA', 'MAP%02X.PIX' % (pid - 11))
        if os.path.exists(fp):
            raw = open(fp, 'rb').read()
            img = [[0] * 256 for _ in range(256)]
            for y in range(256):
                base = y * 128
                if base + 128 > len(raw): break
                row = img[y]
                for xb in range(128):
                    bb = raw[base + xb]
                    row[xb * 2] = bb & 0xF
                    row[xb * 2 + 1] = bb >> 4
    _PIX[page] = img
    return img

# ---- EINGEZEICHNETE TUERSYMBOLE DER ORIGINAL-KACHELN --------------------------
# Nutzer 2026-09-01: "praktisch fast ueberall befindet sich die Tuer auf beiden
# Seiten der Map, obwohl es ein und dieselbe Tuer ist" und "in ROOM1130 ist oben
# links die Tuerzeichnung 2x da".
#
# URSACHE: Das Original fuehrt Tueren NICHT als Daten - es MALT sie in die
# Grundriss-Kachel. Der Port zeichnete zusaetzlich eigene Marken obendrauf.
# Belegt (analysis/kartensymbole/): 92 Innenlinien-Gruppen ueber alle 119
# Rechtecke, davon 90 Tuerblaetter, alle in Palettenindex 4 = rgb(176,176,176);
# keine einzige Wandlinie hat je eine Luecke - eine Tuer ist immer ein 5x5-Stempel
# nach innen. Projiziert man die RDT-Tuer-Datensaetze mit der ORIGINAL-Formel
# FUN_800473f8 + Skalentabelle @0x800768b0, landen sie im Median 3,0 px auf einem
# gezeichneten Symbol (STAGE1: 2,1 px) gegen 19,4 px im Nullmodell; ROOM1150s
# einzige Tuer trifft das Symbol in ROOM1130s Rechteck mit 0,0 px.
#
# Also: wo die Kachel schon eine Tuer zeigt, zeichnet der Port GAR NICHTS.
def _glyphs():
    p = os.path.join(ROOT, 'analysis', 'kartensymbole', 'symbolkatalog.csv')
    out = {}
    if not os.path.exists(p): return out
    with open(p, encoding='utf-8') as f:
        kopf = f.readline().rstrip('\n').split(',')
        ix = {n: i for i, n in enumerate(kopf)}
        for zeile in f:
            t = zeile.rstrip('\n').split(',')
            if len(t) < len(kopf): continue
            if t[ix['typ']] not in ('TUER', 'TUER2'): continue
            pg = int(t[ix['seite']])
            x  = int(t[ix['screen_x']]); y = int(t[ix['screen_y']])
            w  = int(t[ix['px_w']]);     h = int(t[ix['px_h']])
            out.setdefault(pg, []).append((x + w // 2, y + h // 2, w, h,
                                           t[ix['wand']].strip()))
            # ⛔ Zusaetzlich NACH RECHTECK: die Rechtecke einer Seite ueberlappen sich
            # (Seite 4: r1 und r5 teilen x144..159/y137..158). Wer die Symbole
            # geometrisch einem Rechteck zuordnet, schiebt ROOM1160s Tuer nach
            # ROOM1120 - und dessen Eichung legt dann die falsche Tuer auf das Symbol
            # ("bei ROOM1120 komme ich auf der falschen Seite raus"). Der Katalog kennt
            # das richtige Rechteck aus der Kachel-uv.
            out.setdefault((pg, int(t[ix['rect']])), []).append(
                (x + w // 2, y + h // 2, w, h, t[ix['wand']].strip()))
    return out
GLYPHEN = _glyphs()

def kachel_zeigt_tuer(pg, mx, my, tol=5):
    """Zeichnet die Kachel an dieser Stelle schon eine Tuer?"""
    for gx, gy, gw, gh, _wand in GLYPHEN.get(pg, ()):
        if abs(mx - gx) <= tol + gw // 2 and abs(my - gy) <= tol + gh // 2:
            return True
    return False

def page_of(rid):
    stage = (rid >> 12) - 1; room = (rid >> 4) & 0xFF
    if stage == 0:
        if room >= 0x26: return None
        return 2 if room <= 11 else 3 if room <= 17 else 4 if room <= 22 \
               else 5 if room == 23 else 0 if room <= 29 else 1
    if stage == 1: return None if room >= 0xc else (6 if room <= 9 else 0xd)
    if stage == 2: return None if room >= 0x20 else 7
    if stage == 3: return None if room >= 0xc else 8
    if stage == 4:
        if room >= 0x15: return None
        return 0xa if 12 <= room <= 14 else 0xb if room in (15, 16) else 9
    if stage == 5: return None if room >= 0x8 else 0xc
    return None

# ---- RDT: SCA-Zellen + Tueren ------------------------------------------------
SZ = {0x00:1,0x01:2,0x02:1,0x03:4,0x04:4,0x05:2,0x06:4,0x07:4,0x08:2,0x09:4,0x0A:3,0x0B:1,
0x0C:1,0x0D:6,0x0E:2,0x0F:4,0x10:2,0x11:4,0x12:2,0x13:4,0x14:6,0x15:4,0x16:2,0x17:6,0x18:2,
0x19:2,0x1A:2,0x1B:6,0x1C:1,0x1D:1,0x1E:1,0x20:1,0x21:4,0x22:4,0x23:6,0x24:4,0x25:3,0x26:6,
0x27:4,0x28:1,0x29:2,0x2A:1,0x2B:4,0x2C:20,0x2D:34,0x2E:3,0x2F:4,0x30:1,0x31:1,0x32:8,0x33:8,
0x34:4,0x35:3,0x36:12,0x37:4,0x38:3,0x39:4,0x3A:16,0x3B:32,0x3C:2,0x3D:4,0x3E:6,0x3F:4,0x40:8,
0x41:10,0x42:1,0x43:4,0x44:20,0x45:3,0x46:10,0x47:2,0x48:16,0x49:8,0x4A:2,0x4B:3,0x4C:5,0x4D:22,
0x4E:22,0x4F:4,0x50:22,0x51:4,0x52:4,0x53:3,0x54:6,0x55:6,0x56:6,0x57:4,0x58:4,0x59:4,0x5A:6,
0x5B:4,0x5C:4,0x5D:4,0x5E:4,0x5F:2,0x60:14,0x61:4,0x62:2,0x63:1,0x64:16,0x65:2,0x66:1,0x67:28,
0x68:40,0x69:30,0x6A:6,0x6B:4,0x6C:1,0x6D:4,0x6E:6,0x6F:2,0x70:1,0x71:1,0x72:16,0x73:8,0x74:4,
0x75:22,0x76:3,0x77:4,0x78:6,0x79:1,0x7A:16,0x7B:16,0x7C:6,0x7D:6,0x7E:6,0x7F:6,0x80:2,0x81:3,
0x82:3,0x83:1,0x84:2,0x85:6,0x86:1,0x87:1,0x88:3,0x89:1,0x8A:6,0x8B:6,0x8C:1,0x8D:24,0x8E:2}
NAMES = ["collision","camera","zone","light","md1ptr","floor","block","message",
         "mainScd","subScd","extraScd","effect","x50","espTim","modelTim","anim"]

def read_rdt(rid):
    stage = rid >> 12
    p = os.path.join(ROOT, 're15_port', 'shared_assets', 'PSX', f'STAGE{stage}', f'ROOM{rid:04X}.RDT')
    if not os.path.exists(p): return None
    d = open(p, 'rb').read()
    if len(d) < 0x100: return None
    offs = {nm: struct.unpack_from('<I', d, 0x20 + 4*i)[0] for i, nm in enumerate(NAMES)}
    sca = []
    s = offs['collision']
    if 0 < s < len(d) - 24:
        counts = struct.unpack_from('<5I', d, s + 4); n = sum(counts)
        if 0 < n < 2000 and s + 24 + 12*n <= len(d):
            for i in range(n):
                w, dep, x, z = struct.unpack_from('<HHhh', d, s + 24 + 12*i)
                sca.append((x, z, w, dep))
    doors = []
    stairs = []
    # Slots, die irgendwo im Raum per Aot_reset (Opcode 0x46, 10 B: pc[1]=Slot, pc[2]=sce)
    # NEU BEWAFFNET werden. Ein Record mit sce 0 ist nur dann dauerhaft tot, wenn sein Slot
    # hier NICHT vorkommt (der AOT-Verteiler ueberspringt sce 0, @0x80042f48-50).
    rearmed = set()
    for name in ('mainScd', 'subScd'):
        s0 = offs[name]
        e0 = (sorted(v for v in offs.values() if v > s0) + [len(d)])[0]
        for off in range(s0, min(e0, len(d)) - 10):
            if d[off] == 0x46 and d[off + 2] != 0:
                rearmed.add(d[off + 1])
    for name in ('mainScd', 'subScd'):
        s = offs[name]
        ends = sorted(v for v in offs.values() if v > s) + [len(d)]
        e = ends[0]
        if s == 0 or e <= s: continue
        try: n0 = struct.unpack_from('<H', d, s)[0]
        except struct.error: continue
        if n0 == 0 or n0 > 0x200 or n0 % 2: continue
        nsub = n0 // 2
        ptrs = [struct.unpack_from('<H', d, s + 2*i)[0] for i in range(nsub)]
        for si, pp in enumerate(ptrs):
            end = ptrs[si+1] if si+1 < nsub else (e - s)
            if end > e - s: end = e - s
            pc = pp
            while pc < end:
                op = d[s+pc] if s+pc < len(d) else None
                sz = SZ.get(op)
                if sz is None: break
                if op == 0x2c and s+pc+sz <= len(d):
                    b = d[s+pc:s+pc+sz]
                    # Aot_set (Kurzform, 20 B): Typ 12/13 = TREPPEN-Zone
                    # (scd_vm.c: 'SCD Aot_set types 12/13 = die STAIR-Band-Zonen')
                    if b[2] in (12, 13):
                        rx, rz, rw, rd = struct.unpack_from('<hhhh', b, 6)
                        # sce SELBST ist die Achse: 12 = X, 13 = Z (re15_aot.h, Handler
                        # LAB_80043500 liest x @0x80043510-14, LAB_800435cc liest z
                        # @0x800435dc-e0). chain = pc[4] = das BAND dieses Treppenendes,
                        # count = pc[16] = Zahl der ueberquerten Baender (lbu @0x800435b8
                        # bzw. @0x8004367c).
                        stairs.append({'x': rx + rw//2, 'z': rz + rd//2,
                                       'axis': b[2], 'band': b[4], 'count': b[16]})
                if op in (0x3b, 0x68) and s+pc+sz <= len(d):
                    b = d[s+pc:s+pc+sz]
                    if op == 0x3b:
                        rx, rz, rw, rd = struct.unpack_from('<hhhh', b, 6)
                        nx, ny, nz = struct.unpack_from('<hhh', b, 14)
                        stg, rmd = b[22], b[23]
                    else:
                        rx, rz, rw, rd = struct.unpack_from('<hhhh', b, 4)
                        nx, ny, nz = struct.unpack_from('<hhh', b, 22)
                        stg, rmd = b[30], b[31]
                    # ⛔ ZWEI FILTER — sonst landen Marken auf Tueren, die es nicht gibt
                    # (Nutzer 2026-08-31: "auf dem grossen Rechteck sind wieder 3 Tueren
                    # eingezeichnet, wobei nur eine oben existiert"; ROOM1170-Hof).
                    #
                    # (1) RECHTECK 0x0 = SKRIPT-WARP, keine begehbare Tuer.
                    #     Der Original-Trefftest FUN_80042b64 ist Ecke+Ausdehnung, nicht
                    #     Mitte+Halbe: @0x80042b68 lh x0 / @0x80042b6c lw px /
                    #     @0x80042b70 lhu w / @0x80042b74 subu / @0x80042b78 sltu — Treffer
                    #     genau dann, wenn (u32)(px-x0) <= w, ebenso in z @0x80042b84-98.
                    #     Bei w = d = 0 heisst das GLEICHHEIT auf beiden Achsen, also genau
                    #     EIN Weltpunkt. Solche Records werden per Aot_on aus dem Skript
                    #     gefeuert (in ROOM1170 Slot 2/3 @Datei 0x129C/0x12CC, beide auf
                    #     Welt (0,0) = der Eintritts-Warp aus ROOM1240) — man laeuft nicht
                    #     durch sie hindurch, sie gehoeren nicht auf die Karte.
                    if rw == 0 and rd == 0:
                        pc += sz
                        continue
                    # (2) sce-BYTE == 0 = INERTER Record. Der AOT-Verteiler springt ueber
                    #     Eintraege mit sce 0 (@0x80042f48-50); ohne ein spaeteres Aot_reset,
                    #     das denselben Slot mit sce != 0 neu bewaffnet, ist der Record im
                    #     Leon-Szenario dauerhaft tot. In ROOM1170 ist das Slot 1
                    #     (@Datei 0x001226, Welt (5660,-7940), Ziel ROOM10B0) — die Tuer
                    #     existiert nur in Elzas RDT (ROOM1171), und der Generator liest
                    #     ROOM1171 nie (main(): `read_rdt(b) or read_rdt(b+1)`, der erste
                    #     Aufruf liefert immer ein Tupel). Gegenprobe im Port: der Live-Lauf
                    #     von scd_room_reenter installiert Slot 1 als Typ NONE.
                    if b[2] == 0 and b[1] not in rearmed:
                        pc += sz
                        continue
                    doors.append({'lx': rx + rw//2, 'lz': rz + rd//2,
                                  'rx': rx, 'rz': rz, 'rw': rw, 'rd': rd,
                                  # Door_aot_set pc[4] = das BAND (Etage) der Tuer,
                                  # obj[0x82]; das Original gattet die Interaktion
                                  # darauf (FUN_8002bd44 @0x8002bf38).
                                  'band': b[4],
                                  'nx': nx, 'nz': nz,
                                  'dest': ((stg+1) << 12) | (rmd << 4)})
                pc += sz
    return sca, doors, stairs

def zones_of(sca):
    """Zusammenhaengende Bereiche der Kollisionsgeometrie."""
    n = len(sca)
    if not n: return []
    parent = list(range(n))
    def find(a):
        while parent[a] != a: parent[a] = parent[parent[a]]; a = parent[a]
        return a
    for i in range(n):
        xi, zi, wi, di = sca[i]
        for j in range(i+1, n):
            xj, zj, wj, dj = sca[j]
            if (xi < xj+wj+GAP and xj < xi+wi+GAP and zi < zj+dj+GAP and zj < zi+di+GAP):
                ra, rb = find(i), find(j)
                if ra != rb: parent[rb] = ra
    grp = collections.defaultdict(list)
    for i in range(n): grp[find(i)].append(sca[i])
    out = []
    for g in grp.values():
        xs = [c[0] for c in g]; zs = [c[1] for c in g]
        xe = [c[0]+c[2] for c in g]; ze = [c[1]+c[3] for c in g]
        bb = (min(xs), max(xe), min(zs), max(ze))
        out.append((bb, (bb[1]-bb[0]) * (bb[3]-bb[2]), len(g)))
    tot = sum(o[1] for o in out) or 1
    out = [o for o in out if o[1] >= MIN_FRAC * tot]
    out.sort(key=lambda o: -o[1])
    return [o[0] for o in out]

def main():
    room_ids = []
    for line in open(os.path.join(ROOT, 're15_port', 'include', 're15_room_list.h')):
        for tok in line.replace(',', ' ').split():
            if tok.startswith('0x') and len(tok) == 6:
                try: room_ids.append(int(tok, 16))
                except ValueError: pass
    bases = sorted(set(r & 0xFFF0 for r in room_ids))

    # Zonen + Tueren je Basis-Raum
    zinfo = {}
    doors_all = {}
    stairs_all = {}
    for b in bases:
        got = read_rdt(b) or read_rdt(b + 1)
        if not got: continue
        sca, doors, stairs = got
        zs = zones_of(sca)
        if zs: zinfo[b] = zs
        doors_all[b] = doors
        stairs_all[b] = stairs

    # Zonen je Seite sammeln
    by_page = collections.defaultdict(list)      # page -> [(room, zi, bbox)]
    for b, zs in zinfo.items():
        pg = page_of(b)
        if pg is None or pg == 0xd: continue
        for i, bb in enumerate(zs):
            by_page[pg].append((b, i, bb))

    def zone_of_point(room, x, z):
        for i, (x0, x1, z0, z1) in enumerate(zinfo.get(room, [])):
            if x0 - GAP <= x <= x1 + GAP and z0 - GAP <= z <= z1 + GAP: return i
        return None

    # Tuer-Kanten zwischen ZONEN derselben Seite
    edges = collections.defaultdict(list)
    for b, doors in doors_all.items():
        pa = page_of(b)
        if pa is None or pa == 0xd: continue
        for d in doors:
            dest = d['dest'] & 0xFFF0
            pb = page_of(dest)
            if pb != pa: continue
            za = zone_of_point(b, d['lx'], d['lz'])
            zb = zone_of_point(dest, d['nx'], d['nz'])
            if za is None or zb is None: continue
            if (b, za) == (dest, zb): continue
            edges[pa].append(((b, za), d['lx'], d['lz'], (dest, zb), d['nx'], d['nz']))

    assign = {}
    for pg in sorted(by_page):
        R = rects(pg)
        zl = by_page[pg]
        if not R: continue
        def T(bb, r):
            x0, x1, z0, z1 = bb
            if x1 <= x0 or z1 <= z0: return None
            ax = r[2] / (x1 - x0); az = r[3] / (z1 - z0)
            return ax, r[0] - ax*x0, az, r[1] + az*z1
        def apply(t, wx, wz): return t[0]*wx + t[1], -t[2]*wz + t[3]
        def aspect_pen(bb, r):
            fw = bb[1]-bb[0]; fd = bb[3]-bb[2]
            if fw <= 0 or fd <= 0 or r[2] <= 0 or r[3] <= 0: return 3.0
            return abs(math.log((fw/fd) / (r[2]/r[3])))
        # Flaechen-Rang beider Seiten
        zrank = {k: i for i, k in enumerate(sorted(range(len(zl)),
                 key=lambda i: -((zl[i][2][1]-zl[i][2][0])*(zl[i][2][3]-zl[i][2][2]))))}
        rrank = {i: k for k, i in enumerate(sorted(range(len(R)), key=lambda i: -(R[i][2]*R[i][3])))}
        key2idx = {(zl[i][0], zl[i][1]): i for i in range(len(zl))}
        def cost(a):
            c = 0.0
            used = collections.Counter(v for v in a.values() if v is not None)
            for i, z in enumerate(zl):
                r = a.get(i)
                if r is None: c += 30.0; continue
                c += 8.0 * aspect_pen(z[2], R[r])
                c += 1.5 * abs(zrank[i] - rrank[r])
            for r, k in used.items():
                if k > 1: c += 12.0 * (k-1)
            for (ka, lx, lz, kb, nx, nz) in edges.get(pg, []):
                ia = key2idx.get(ka); ib = key2idx.get(kb)
                if ia is None or ib is None: continue
                ra = a.get(ia); rb = a.get(ib)
                if ra is None or rb is None: continue
                ta = T(zl[ia][2], R[ra]); tb = T(zl[ib][2], R[rb])
                if not ta or not tb: continue
                pa_ = apply(ta, lx, lz); pb_ = apply(tb, nx, nz)
                c += min(math.hypot(pa_[0]-pb_[0], pa_[1]-pb_[1]), 40.0)
            return c
        best = None
        rng = random.Random(9000 + pg)
        opts = list(range(len(R))) + [None]
        for _ in range(80):
            a = {}
            order = list(range(len(zl))); rng.shuffle(order)
            for i in order:
                cand = []
                for r in opts:
                    a[i] = r; cand.append((cost(a), r))
                a[i] = min(cand)[1]
            for _ in range(60):
                improved = False
                for i in range(len(zl)):
                    cur = a[i]; base = cost(a)
                    for r in opts:
                        if r == cur: continue
                        a[i] = r
                        cc = cost(a)
                        if cc < base - 1e-9: base = cc; cur = r; improved = True
                    a[i] = cur
                if not improved: break
            c = cost(a)
            if best is None or c < best[0]: best = (c, dict(a))
        c, a = best
        for i, z in enumerate(zl):
            if a.get(i) is not None:
                assign[(z[0], z[1])] = (pg, a[i])
        n_ok = sum(1 for v in a.values() if v is not None)
        print(f"Seite {pg:2d}: {n_ok}/{len(zl)} Zonen zugeordnet ({len(R)} Rects), Kosten {c:7.1f}")

    # ---- Ausgabe ----
    # ---- BELEGTE ZUORDNUNGEN, die die Kostenheuristik nicht findet -------------
    # ROOM1130 blieb unzugeordnet (Seite 4: 5 von 6 Zonen). Rect 4 (144,80) 32x80 ist
    # unbelegt und gehoert ROOM1130 — unabhaengig doppelt belegt (2026-08-31):
    #  * ROOM1130 gehoert auf Seite 4: Sprungtabelle @0x8001103c Eintrag 19 -> 0x8004b710,
    #    Fall-Through bis `ori v0,zero,0x4` @0x8004b758 -> `sb v0,DAT_800b260e` @0x8004b88c.
    #  * TUER-KOHAERENZ statt Groessen-Lineal: mit dem original-geeichten Anker
    #    ROOM1150 -> Rect 2 (Skalenzeile 21 @0x80076958, Marker-Formel FUN_800473f8)
    #    ergibt Rect 4 einen mittleren Tuerpositions-Fehler von 11,1 px, jedes andere
    #    Rechteck 23,6..31,9 px; in der vollstaendigen Suche ueber alle 720 injektiven
    #    Zuordnungen tragen die sechs besten Loesungen ALLE 1130 -> Rect 4.
    #  * Seitenverhaeltnis (massstabsfrei): Rect 4 = 0,400, ROOM1130 = 0,384,
    #    naechster Raum 0,782.
    # ⛔ NICHT angefasst: ROOM1140 Zone 1 -> Rect 3. Die Zone ist nachweislich KEIN Ort,
    # sondern der solide Moebelblock im Briefing-Raum (byte-true Kollisions-Sonde
    # FUN_8003b0a4: 1,2 % der Rasterpunkte frei, gegen 66 % bei einer echten Zone; alle
    # 19 SCA-Zellen Band 0, keine Tuer, keine Treppe). Genau deshalb wird Rect 3 nie
    # BESUCHT und damit nie gezeichnet — die falsche Zuordnung haelt es unsichtbar.
    # Nimmt man sie weg, faellt Rect 3 auf "ohne Zone" und wird DAUERHAFT GRAU gemalt,
    # also sichtbar falsch. In Wahrheit zeigt Rect 3 ROOM1170s zweiten Bereich noch
    # einmal auf dem 3F-Blatt (dieselbe Zeichnung wie Seite 5 Rect 0, 22 von 1152 Pixeln
    # Unterschied, alle Index 4->1 = zwei Tuersymbole; deren Lage passt bei WAHREM
    # Massstab mit 5,14 px Restfehler zu 1170s zwei 3F-Tueren nach 1130/1140). Das
    # sauber abzubilden braucht eine Zone auf ZWEI Seiten — das kann das Datenmodell
    # heute nicht, und ein Schnellschuss verschoebe die zid-Nummern (Save-Besucht-Bits).
    # ---- FALSCHE ZUORDNUNGEN ENTFERNEN ---------------------------------------
    # ROOM1110 Zone 1 -> Seite 3 Rect 9: diese Zuordnung stammt aus der Kostenheuristik
    # und blockiert das Rechteck, das nach der Beleglage die 2F-Zeichnung des
    # TREPPENHAUSES ist:
    #   * Seite 2 Rect 9 und Seite 3 Rect 9 sind in Position UND Kachel identisch
    #     ((109,134) 16x16, uv(168,40)) - und pixelgleich (Verifikation 2026-08-31:
    #     page2-r9 == page3-r9 == page4-r0, 0 von 256 Pixeln Unterschied).
    #   * Seite 2 Rect 9 gehoert ROOM1060, dem Treppenhaus.
    #   * ROOM1060 spannt nachweislich 1F/2F/3F - seine eigenen Tueren sagen es:
    #     Band 8 -> ROOM1120 (Seite 4 "3F"), Band 4 -> ROOM10C0 (Seite 3 "2F"),
    #     Band 0 -> ROOM1040 (Seite 2 "1F").
    # Ein Raum, der auf drei aufeinanderfolgenden Etagenblaettern an derselben Stelle
    # mit derselben Zeichnung steht, IST das Treppenhaus.
    # ⛔ EHRLICH: das ist eine Abwaegung, kein Beweis. Weder ROOM1110 Zone 1
    # (20,2 x 4,4 Kartenpixel) noch ROOM1060 (24,2 x 27,8) passt groessenmaessig gut auf
    # 16x16; entschieden hat die Dreifach-Zeichnung. Ohne diese Zeile fehlt dem
    # Treppenhaus die Etage 2F (Nutzer 2026-09-01: "gehe ich die 2 Treppen runter, bin
    # ich auf Ebene 2F").
    ZONE_DROP = { (0x1110, 1) }
    for key in ZONE_DROP:
        assign.pop(key, None)

    ZONE_FIX = { (0x1130, 0): (4, 4) }
    for key, val in ZONE_FIX.items():
        room, zi = key
        if room in zinfo and zi < len(zinfo[room]):
            assign[key] = val

    rows = []
    zid = 0   # globale Zonen-Nummer; beide Szenario-Varianten teilen sie
    for b, zs in sorted(zinfo.items()):
        for i, bb in enumerate(zs):
            pr = assign.get((b, i))
            if pr is None: continue
            for var in (0, 1):
                rows.append((b + var, bb, pr[0], pr[1], i, zid))
            zid += 1
    o = []
    o.append("/* GENERIERT von tools/gen_map_zones.py - KARTEN-ZONEN.")
    o.append(" * Ein RDT-Raum ist nicht immer EIN Ort: 26 der 103 Basis-Raeume zerfallen in")
    o.append(" * mehrere zusammenhaengende Bereiche (ROOM1170 z.B. in zwei, verbunden durch")
    o.append(" * eine Selbst-Tuer). Jede Zone traegt ihr eigenes Karten-Rechteck und ihre")
    o.append(" * eigene Marker-Abbildung; die aktive Zone folgt der Spielerposition.")
    o.append(" * Herleitung + Verfahren: analysis/nutzer_batch_2026-08-30b/map-zonen.md */")
    o.append(" * Der Typ re15_map_zone_t steht in include/re15_room.h. */")
    o.pop()   # den provisorischen Kommentar-Abschluss der Kopfzeile ersetzen
    # ---- ETAGEN: KEINE Hand-Korrektur mehr ------------------------------------
    # ⛔ ZURUECKGENOMMEN am 2026-08-31. Ich hatte ROOM1170s zweiten Bereich komplett
    # auf Seite 4 gelegt ("die Ebene hinter der Treppe"). Das war falsch, weil dieser
    # Bereich als EIN Kollisions-Zusammenhang vorliegt: kleiner Raum OBEN + Treppe +
    # Absatz UNTEN. Die Umlegung schaltete die Karte schon beim Betreten des kleinen
    # Raums auf die untere Etage. Folge (Nutzer-Report v0.3.69): das kleine Rechteck
    # verschwand von Seite 5, und weil ein Rect OHNE Zone als "unbekannt" grau
    # gezeichnet wird, standen dort "beide Rechtecke direkt" sichtbar.
    # Die Ebene ist im Spiel das BAND (+0x82, ROOM1170 fuehrt 0/2/4 — re15_aot.h:104);
    # die Umschaltung muss daran haengen, nicht an der Zone. Bis das gemessen ist,
    # bleibt die automatische Zuordnung stehen — sie war vom Nutzer als richtig
    # bestaetigt ("das sieht fuer das grosse und kleine Rechteck einwandfrei aus").
    # ================= EICHUNG DER PROJEKTION ==================================
    # Bis hierher wurde die Zonen-Bbox linear ins Rechteck gestreckt. Das ist eine
    # Annahme: das Rechteck ist die ZEICHNUNG des Raums, kein Massstabsraster - und
    # der Nutzer hat die Folgen gesehen ("bei ROOM1120 komme ich laut Map auf der
    # falschen Seite raus": der Eintritt aus ROOM1130 landete unten LINKS, waehrend
    # die zugehoerige gemalte Tuer oben RECHTS sitzt).
    #
    # Das Original rechnet anders - FUN_800473f8 @0x8004741c-0x80047528:
    #     mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
    #     my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
    # mit einer Zeile {ox, oy, sx, sy} je Raum-Index @0x800768b0. Nur 13 der 38
    # Zeilen sind ueberhaupt geeicht, der Rest ist der Stub {0,0,1,1} - deshalb
    # konnte der Port sie nicht einfach uebernehmen.
    #
    # Die fehlenden Zeilen lassen sich BESTIMMEN (kein freier Vierparameter-Fit):
    #   MASSSTAB aus der gezeichneten Flaeche: das Rechteck IST die Zeichnung des
    #     Raums, also bildet dessen Kollisions-Bbox auf die nicht-transparenten
    #     Pixel der Kachel ab -> sx, sy liegen fest.
    #   VERSATZ aus den GEMALTEN Tuersymbolen: die projizierten Tueren muessen
    #     darauf liegen -> ox, oy liegen fest.
    # Uebernommen wird die Eichung nur, wenn sie den mittleren Abstand zu den
    # Symbolen gegenueber der Bbox-Streckung um mehr als 1,5 px verbessert.
    def _gezeichnet(pg, r):
        pix = page_pix(pg)
        if pix is None: return None
        RX, RY, RW, RH = rects(pg)[r]; U, V = rect_uv(pg, r)
        xs = []; ys = []
        for j in range(RH):
            for i in range(RW):
                if V + j < 256 and U + i < 256 and pix[V + j][U + i] != 0:
                    xs.append(RX + i); ys.append(RY + j)
        if not xs: return None
        return min(xs), max(xs), min(ys), max(ys)

    def _proj(wx, wz, ox, oy, sx, sy):
        return (((((wx + 32000) * 10 * sx) >> 20) + 5) // 10 + ox,
                -(((((wz + 32000) * 10 * sy) >> 20) + 5) // 10) + oy)

    def _paarfehler(pts, gly):
        n = len(pts); m = len(gly); k = min(n, m)
        if k == 0: return None
        best = None
        for wahl in itertools.permutations(range(m), k):
            for sub in itertools.combinations(range(n), k):
                f = sum(((pts[sub[i]][0] - gly[wahl[i]][0]) ** 2 +
                         (pts[sub[i]][1] - gly[wahl[i]][1]) ** 2) ** 0.5
                        for i in range(k)) / k
                if best is None or f < best: best = f
        return best

    eichung = {}          # (room, zi) -> (ox, oy, sx, sy)
    for (b, i), pr in sorted(assign.items()):
        pg, r = pr
        gly = [(gx, gy, wd) for gx, gy, gw, gh, wd in GLYPHEN.get((pg, r), ())]
        if not gly: continue
        tueren = [d for d in doors_all.get(b, []) if not (d['rw'] == 0 and d['rd'] == 0)]
        if not tueren: continue
        bb = zinfo[b][i]
        x0, x1, z0, z1 = bb
        if x1 <= x0 or z1 <= z0: continue
        gz = _gezeichnet(pg, r)
        if not gz: continue
        gx0, gx1, gy0, gy1 = gz
        sx = int(round((gx1 - gx0 + 1) / (x1 - x0) * (1 << 20)))
        sy = int(round((gy1 - gy0 + 1) / (z1 - z0) * (1 << 20)))
        if not (400 <= sx <= 6000 and 400 <= sy <= 6000): continue
        px = [((((d['lx'] + 32000) * 10 * sx) >> 20) + 5) // 10 for d in tueren]
        py = [-(((((d['lz'] + 32000) * 10 * sy) >> 20) + 5) // 10) for d in tueren]
        n = len(tueren); m = len(gly); k = min(n, m)
        besteich = None
        for wahl in itertools.permutations(range(m), k):
            for sub in itertools.combinations(range(n), k):
                # ⛔ ACHSEN-BEDINGUNG: ein Symbol in einer Ost-/Westwand kann nur zu
                # einer Tuer gehoeren, deren Trigger-Rechteck laengs Z gestreckt ist
                # (senkrechte Wand), und umgekehrt. Ohne diese Bedingung ist die
                # Zuordnung bei einem Symbol und mehreren Tueren beliebig - in
                # ROOM1120 legte sie die Tuer nach ROOM1060 auf das Symbol der
                # ROOM1130-Tuer, und der Eintritt landete 28 px zu tief.
                if any((gly[wahl[t]][2] in ('W', 'E')) !=
                       (tueren[sub[t]]['rd'] > tueren[sub[t]]['rw']) for t in range(k)):
                    continue
                ox = int(round(sum(gly[wahl[t]][0] - px[sub[t]] for t in range(k)) / k))
                oy = int(round(sum(gly[wahl[t]][1] - py[sub[t]] for t in range(k)) / k))
                f = _paarfehler([_proj(d['lx'], d['lz'], ox, oy, sx, sy) for d in tueren], gly)
                if f is not None and (besteich is None or f < besteich[0]):
                    besteich = (f, ox, oy)
        if besteich is None: continue
        # Vergleich mit der bisherigen Bbox-Streckung
        R = rects(pg)[r]
        ptsB = []
        for d in tueren:
            fx = min(max(d['lx'] - x0, 0), x1 - x0); fz = min(max(d['lz'] - z0, 0), z1 - z0)
            ptsB.append((R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0)))
        fB = _paarfehler(ptsB, gly)
        if fB is None or besteich[0] + 1.5 >= fB: continue
        eichung[(b, i)] = (besteich[1], besteich[2], sx, sy)
    print(f"{len(eichung)} Zonen geeicht (Massstab aus der Zeichnung, Versatz aus den Tuersymbolen)")

    def zone_at(room, wx, wz):
        best, best_a = None, 0
        for i, (x0, x1, z0, z1) in enumerate(zinfo.get(room, [])):
            if x0 - GAP <= wx <= x1 + GAP and z0 - GAP <= wz <= z1 + GAP:
                a = (x1 - x0) * (z1 - z0)
                if best is None or a < best_a: best, best_a = i, a
        return best

    def _proj_z(room, zi, wx, wz):
        ei = eichung.get((room, zi))
        pr = assign.get((room, zi))
        if pr is None: return None
        pg, r = pr
        if ei:
            ox, oy, sx, sy = ei
            return pg, r, _proj(wx, wz, ox, oy, sx, sy)
        bb = zinfo[room][zi]
        x0, x1, z0, z1 = bb
        if x1 <= x0 or z1 <= z0: return None
        R = rects(pg)[r]
        fx = min(max(wx - x0, 0), x1 - x0); fz = min(max(wz - z0, 0), z1 - z0)
        return pg, r, (R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0))

    # ---- ZWEITER EICH-DURCHGANG: Partner-Tuer als zusaetzliche Bedingung -------
    # In ROOM1120 gibt es DREI Tueren, aber nur EIN gemaltes Symbol - welche Tuer
    # darauf gehoert, ist aus dem Raum allein nicht entscheidbar (die Achsen-Bedingung
    # laesst zwei zu). Der erste Durchgang legte die Tuer nach ROOM1060 darauf; damit
    # landete der Eintritt aus ROOM1130 quer durch den Raum - "auf der falschen Seite
    # raus".
    # Die Entscheidung faellt ueber den NACHBARN: ein Durchgang ist EIN Ort, die Tuer
    # muss also dort liegen, wo der Nachbarraum seine Gegen-Tuer zeichnet. Fuer 1120
    # heisst das: die 1130-Tuer gehoert auf (150,126) - 6 px von 1130s eigener
    # Gegen-Tuer (156,125) -, nicht auf (138,152), 30 px entfernt.
    def _partnerziele():
        """(room, zi, Tuer-Index) -> Punkt, an dem der NACHBAR seine Gegen-Tuer malt."""
        alle = []
        for b in sorted(zinfo):
            for di, d in enumerate(doors_all.get(b, [])):
                if d['rw'] == 0 and d['rd'] == 0: continue
                zi = zone_at(b, d['lx'], d['lz'])
                if zi is None or assign.get((b, zi)) is None: continue
                alle.append((b, zi, di, d))
        ziel = {}
        for i in range(len(alle)):
            rA, zA, iA, A = alle[i]
            for j in range(len(alle)):
                if i == j: continue
                rB, zB, iB, B = alle[j]
                if A['dest'] != rB or B['dest'] != rA: continue
                if abs(A['nx'] - B['lx']) + abs(A['nz'] - B['lz']) > 4000: continue
                if abs(B['nx'] - A['lx']) + abs(B['nz'] - A['lz']) > 4000: continue
                pb = _proj_z(rB, zB, B['lx'], B['lz'])
                pa_pg = assign[(rA, zA)][0]
                if pb and pb[0] == pa_pg:
                    ziel[(rA, zA, iA)] = pb[2]
        return ziel

    for _durchgang in range(2):
        ziele = _partnerziele()
        for (b, i), pr in sorted(assign.items()):
            pg, r = pr
            gly = [(gx, gy, wd) for gx, gy, gw, gh, wd in GLYPHEN.get((pg, r), ())]
            if not gly: continue
            tueren = [(di, d) for di, d in enumerate(doors_all.get(b, []))
                      if not (d['rw'] == 0 and d['rd'] == 0)
                      and zone_at(b, d['lx'], d['lz']) == i]
            if not tueren: continue
            ei = eichung.get((b, i))
            if not ei: continue
            _ox, _oy, sx, sy = ei
            px = [((((d['lx'] + 32000) * 10 * sx) >> 20) + 5) // 10 for _di, d in tueren]
            py = [-(((((d['lz'] + 32000) * 10 * sy) >> 20) + 5) // 10) for _di, d in tueren]
            n = len(tueren); m = len(gly); k = min(n, m)
            best = None
            for wahl in itertools.permutations(range(m), k):
                for sub in itertools.combinations(range(n), k):
                    if any((gly[wahl[t]][2] in ('W', 'E')) !=
                           (tueren[sub[t]][1]['rd'] > tueren[sub[t]][1]['rw'])
                           for t in range(k)):
                        continue
                    ox = int(round(sum(gly[wahl[t]][0] - px[sub[t]] for t in range(k)) / k))
                    oy = int(round(sum(gly[wahl[t]][1] - py[sub[t]] for t in range(k)) / k))
                    f = 0.0
                    for t in range(k):
                        f += (((px[sub[t]] + ox - gly[wahl[t]][0]) ** 2 +
                               (py[sub[t]] + oy - gly[wahl[t]][1]) ** 2) ** 0.5)
                    f /= k
                    # Partner-Bedingung: jede Tuer mit bekanntem Nachbarziel zieht dorthin
                    np_ = 0; fp = 0.0
                    for t in range(n):
                        z = ziele.get((b, i, tueren[t][0]))
                        if not z: continue
                        fp += (((px[t] + ox - z[0]) ** 2 + (py[t] + oy - z[1]) ** 2) ** 0.5)
                        np_ += 1
                    if np_: f += fp / np_
                    if best is None or f < best[0]: best = (f, ox, oy)
            if best is not None and (best[1], best[2]) != (_ox, _oy):
                eichung[(b, i)] = (best[1], best[2], sx, sy)

    # ================= TUER-KOHAERENZ ==========================================
    # Nutzer 2026-09-01: "bei ROOM1120 komme ich laut Map auf der falschen Seite raus."
    # Gemessen war das NICHT ein Fehler in 1120: seine beiden Tueren liegen auf ihren
    # gemalten Symbolen (2-3 px). Der Fehler ist, dass ROOM1130 und ROOM1120 DIESELBE
    # Tuer an zwei verschiedenen Stellen zeichnen - 1130 bei (156,125), 1120 bei
    # (138,152). Jede Zone fuer sich stimmt, zusammen passen sie nicht.
    #
    # Ein Durchgang ist EIN Ort. Die Eichungen einer Seite muessen also zusaetzlich
    # zueinander passen. Der Massstab bleibt dabei fest (er ist geometrisch aus der
    # Zeichnung bestimmt, und er ist im Spiel praktisch global: RE1.5s geeichte Zeilen
    # liegen bei sx ~ 2280 = 460 Welteinheiten je Pixel, RE2 rechnet fest mit 450 -
    # FUN_8006e1f0 @0x8006e1f0 `(X+28000)/450`). Verschoben wird nur der VERSATZ, also
    # zwei Freiheitsgrade je Zone.
    #
    # Verfahren: Federmodell, per Gauss-Seidel geloest.
    #   * SYMBOL-Feder (stark): eine Zone mit gemalten Tuersymbolen bleibt darauf.
    #   * PAAR-Feder (schwach): die beiden Enden eines Durchgangs ziehen zusammen.
    # Uebernommen wird eine Verschiebung nur, wenn sie den Abstand zu den gemalten
    # Symbolen NICHT verschlechtert (Toleranz 1 px) - die schon belegten Lagen sind
    # damit geschuetzt.
    # Alle Tueren mit ihrer Zone einsammeln
    tuer_liste = []       # (room, zi, door)
    for b in sorted(zinfo):
        for d in doors_all.get(b, []):
            if d['rw'] == 0 and d['rd'] == 0: continue
            zi = zone_at(b, d['lx'], d['lz'])
            if zi is None or assign.get((b, zi)) is None: continue
            tuer_liste.append((b, zi, d))
    # Paare finden (Ziel des einen liegt am Ort des anderen und umgekehrt)
    paare = []
    for i in range(len(tuer_liste)):
        rA, zA, A = tuer_liste[i]
        for j in range(i + 1, len(tuer_liste)):
            rB, zB, B = tuer_liste[j]
            if A['dest'] != rB or B['dest'] != rA: continue
            if abs(A['nx'] - B['lx']) + abs(A['nz'] - B['lz']) > 4000: continue
            if abs(B['nx'] - A['lx']) + abs(B['nz'] - A['lz']) > 4000: continue
            pa = _proj_z(rA, zA, A['lx'], A['lz']); pb = _proj_z(rB, zB, B['lx'], B['lz'])
            if not pa or not pb or pa[0] != pb[0]: continue     # nur auf gleicher Seite
            if (rA, zA) == (rB, zB): continue
            paare.append(((rA, zA), (rB, zB), (A['lx'], A['lz']), (B['lx'], B['lz'])))
    # Federmodell
    schub = {}            # (room,zi) -> [dx, dy]
    for k in set([p[0] for p in paare] + [p[1] for p in paare]):
        schub[k] = [0.0, 0.0]
    ankersym = {k for k in schub if k in eichung}
    for _ in range(60):
        for (kA, kB, wA, wB) in paare:
            pa = _proj_z(kA[0], kA[1], wA[0], wA[1]); pb = _proj_z(kB[0], kB[1], wB[0], wB[1])
            if not pa or not pb: continue
            ax, ay = pa[2][0] + schub[kA][0], pa[2][1] + schub[kA][1]
            bx, by = pb[2][0] + schub[kB][0], pb[2][1] + schub[kB][1]
            dx, dy = (bx - ax) / 2.0, (by - ay) / 2.0
            wa = 0.10 if kA in ankersym else 0.40
            wb = 0.10 if kB in ankersym else 0.40
            schub[kA][0] += dx * wa; schub[kA][1] += dy * wa
            schub[kB][0] -= dx * wb; schub[kB][1] -= dy * wb
    # Uebernehmen, wenn die Symbol-Lage nicht schlechter wird
    def _symfehler(room, zi, dx, dy):
        pr = assign.get((room, zi))
        if pr is None: return None
        pg, r = pr
        gly = [(gx, gy) for gx, gy, gw, gh, wd in GLYPHEN.get((pg, r), ())]
        if not gly: return None
        pts = []
        for d in doors_all.get(room, []):
            if d['rw'] == 0 and d['rd'] == 0: continue
            if zone_at(room, d['lx'], d['lz']) != zi: continue
            p = _proj_z(room, zi, d['lx'], d['lz'])
            if p: pts.append((p[2][0] + dx, p[2][1] + dy))
        if not pts: return None
        return sum(min(((px - gx) ** 2 + (py - gy) ** 2) ** 0.5 for gx, gy in gly)
                   for px, py in pts) / len(pts)
    n_koh = 0
    for k, (dx, dy) in schub.items():
        idx, idy = int(round(dx)), int(round(dy))
        if idx == 0 and idy == 0: continue
        alt = _symfehler(k[0], k[1], 0, 0)
        neu = _symfehler(k[0], k[1], idx, idy)
        if alt is not None and neu is not None and neu > alt + 1.0: continue
        ei = eichung.get(k)
        if ei:
            eichung[k] = (ei[0] + idx, ei[1] + idy, ei[2], ei[3])
        else:
            # ohne Eichung: eine erzeugen, Massstab aus der gezeichneten Flaeche
            pr = assign.get(k);
            if pr is None: continue
            pg, r = pr
            gz = _gezeichnet(pg, r)
            bb = zinfo[k[0]][k[1]]
            x0, x1, z0, z1 = bb
            if not gz or x1 <= x0 or z1 <= z0: continue
            sx = int(round((gz[1] - gz[0] + 1) / (x1 - x0) * (1 << 20)))
            sy = int(round((gz[3] - gz[2] + 1) / (z1 - z0) * (1 << 20)))
            if not (400 <= sx <= 6000 and 400 <= sy <= 6000): continue
            p0 = _proj_z(k[0], k[1], x0, z1)     # irgendein Referenzpunkt
            if not p0: continue
            q = _proj(x0, z1, 0, 0, sx, sy)
            eichung[k] = (p0[2][0] - q[0] + idx, p0[2][1] - q[1] + idy, sx, sy)
        n_koh += 1
    print(f"{len(paare)} Tuerpaare, {n_koh} Zonen kohaerent nachgezogen")

    o.append("static const re15_map_zone_t s_map_zones[] = {")
    for room, bb, pg, r, zi, zd in rows:
        ei = eichung.get((room & 0xFFF0, zi), (0, 0, 0, 0))
        o.append(f"    {{ 0x{room:04X}, {bb[0]:6d}, {bb[2]:6d}, {bb[1]:6d}, {bb[3]:6d}, {pg:2d}, {r:2d}, {zi}, {zd:3d},"
                 f" {ei[0]:5d}, {ei[1]:5d}, {ei[2]:5d}, {ei[3]:5d} }},")
    o.append("};")
    # ---- MARKEN: Treppen, in Karten-Koordinaten vorberechnet ------------------
    # Der Nutzer: "die [Tuer] ist auf der Karte nicht eingezeichnet ... auserdem
    # muesste links im kleinen rechteck die Treppe eingezeichnet sein."
    # Position: Welt -> Zone -> Rechteck (dieselbe lineare Abbildung wie der Marker).
    zpos = {}                       # (room, zi) -> (bbox, page, rect)
    for b, zs in sorted(zinfo.items()):
        for i, bb in enumerate(zs):
            pr = assign.get((b, i))
            if pr is not None: zpos[(b, i)] = (bb, pr[0], pr[1])
    def to_map(room, zi, wx, wz):
        e = zpos.get((room, zi))
        if not e: return None
        (x0, x1, z0, z1), pg, r = e
        ei = eichung.get((room, zi))
        if ei:
            ox, oy, sx, sy = ei
            mx, my = _proj(wx, wz, ox, oy, sx, sy)
            R = rects(pg)[r]
            if mx < R[0]: mx = R[0]
            if mx > R[0] + R[2] - 1: mx = R[0] + R[2] - 1
            if my < R[1]: my = R[1]
            if my > R[1] + R[3] - 1: my = R[1] + R[3] - 1
            return pg, r, mx, my
        R = rects(pg)[r]
        if x1 <= x0 or z1 <= z0: return None
        fx = min(max(wx - x0, 0), x1 - x0)
        fz = min(max(wz - z0, 0), z1 - z0)
        # z GESPIEGELT — wie die Original-Markerformel (FUN_800473f8 negiert das
        # z-Ergebnis) und wie re15_map_zones.c seit der Korrektur vom 2026-08-31.
        return pg, r, R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0)
    def snap_wall(pg, r, mx, my, senk):
        """Rueckt eine Tuermarke auf die GEZEICHNETE Wand und nennt die WANDSEITE.

        Das Karten-Rechteck ist nur die Bounding-Box; der gezeichnete Raum darin kann
        L-foermig sein (ROOM1130 ist ein abgeknickter Flur, ROOM1170s zweiter Bereich
        eine L-Form). Die Wand steht deshalb in der Original-Grafik DATA/MAP0x.PIX:
        Palettenindex 0 = transparent = ausserhalb.

        ⛔ TOLERANZ QUER ZUR WAND. Ein erster Wurf suchte die Wand nur in der EINEN
        Zeile/Spalte der Marke. Liegt die Projektion dort einen Pixel neben dem
        richtigen Raumteil, springt die Marke in einen ganz anderen Arm der L-Form:
        ROOM1170s Tuer nach ROOM1130 stand korrekt bei y=81 (Oberkante des linken
        Blocks), landete aber bei y=94 (Oberkante des unteren Arms), weil ihre Spalte
        18 gerade nicht mehr zum linken Block (Spalten 0..17) gehoert - der Nutzer sah
        "die Tuer oben links im kleinen Rechteck fehlt". Deshalb wird jetzt ein
        schmales Fenster quer zur Wand mitgesucht und die Kante mit dem kleinsten
        Gesamtversatz genommen (quer zaehlt doppelt, damit die Wandachse fuehrend
        bleibt).

        Rueckgabe: (mx, my, seite), seite 0=Nord 1=Ost 2=Sued 3=West - die Wand, in der
        die Tuer sitzt (der Raum liegt jeweils gegenueber).
        """
        R = rects(pg)
        if r >= len(R): return mx, my, (3 if senk else 0)
        RX, RY, RW, RH = R[r]
        pix = page_pix(pg)
        if pix is None: return mx, my, (3 if senk else 0)
        U, V = rect_uv(pg, r)

        def drawn(i, j):
            if not (0 <= i < RW and 0 <= j < RH): return False
            yy, xx = V + j, U + i
            if not (0 <= yy < 256 and 0 <= xx < 256): return False
            return pix[yy][xx] != 0

        QUER = 3          # wie weit laengs der Wand gesucht wird
        i0, j0 = mx - RX, my - RY
        best = None       # (kosten, i, j, seite)
        if senk:
            for dj in range(-QUER, QUER + 1):
                j = j0 + dj
                if not (0 <= j < RH): continue
                for i in range(RW):
                    if not drawn(i, j): continue
                    if not (i == 0 or not drawn(i - 1, j) or i == RW - 1 or not drawn(i + 1, j)):
                        continue
                    kosten = abs(i - i0) + 2 * abs(dj)
                    seite = 3 if drawn(i + 1, j) else 1        # 3 = West, 1 = Ost
                    if best is None or kosten < best[0]:
                        best = (kosten, i, j, seite)
            if best is None: return mx, my, 3
            return RX + best[1], RY + best[2], best[3]
        else:
            for di in range(-QUER, QUER + 1):
                i = i0 + di
                if not (0 <= i < RW): continue
                for j in range(RH):
                    if not drawn(i, j): continue
                    if not (j == 0 or not drawn(i, j - 1) or j == RH - 1 or not drawn(i, j + 1)):
                        continue
                    kosten = abs(j - j0) + 2 * abs(di)
                    seite = 0 if drawn(i, j + 1) else 2        # 0 = Nord, 2 = Sued
                    if best is None or kosten < best[0]:
                        best = (kosten, i, j, seite)
            if best is None: return mx, my, 0
            return RX + best[1], RY + best[2], best[3]

    marks = []                      # (page, rect, mx, my, kind, zid)
    zid_of = {}
    _z = 0
    for b, zs in sorted(zinfo.items()):
        for i in range(len(zs)):
            if assign.get((b, i)) is not None:
                zid_of[(b, i)] = _z; _z += 1

    # ================= DURCHGANG 1: jede Marke einzeln berechnen =================
    vor = []      # dict je Marke, mit Herkunft fuer die Paarbildung
    for b in sorted(zinfo):
        # ---- TREPPEN: EIN Aufgang, EINE Marke -------------------------------
        # Eine Treppe liegt als ZWEI Band-Wechsel-Zonen vor, eine je Ende (Nutzer
        # 2026-08-31: "auch da wird links und rechts ein Treppensymbol gezeichnet,
        # fuer ein und dieselbe Treppe"). Das Paar ist aus den Daten erkennbar:
        # gleiche Achse (sce 12 = X / 13 = Z), gleiche Stufenzahl count, und der
        # Band-Abstand ist GENAU count (chain = Band dieses Endes, count = Zahl der
        # ueberquerten Baender, lbu @0x800435b8 / @0x8004367c). Gemessen in ROOM1170:
        # Achse Z Band 0 <-> 2 (2780 Einheiten) und Achse X Band 2 <-> 4 (2660),
        # also zwei Aufgaenge aus vier Datensaetzen. Greedy ueber die kuerzeste
        # Entfernung, damit bei zwei Enden auf demselben Band richtig gepaart wird.
        st_list = stairs_all.get(b, [])
        st_merge, _used, _cand = {}, set(), []
        for _i in range(len(st_list)):
            for _j in range(_i + 1, len(st_list)):
                A, B = st_list[_i], st_list[_j]
                if A['axis'] != B['axis'] or A['count'] != B['count']: continue
                if abs(A['band'] - B['band']) != A['count']: continue
                _cand.append((abs(A['x'] - B['x']) + abs(A['z'] - B['z']), _i, _j))
        for d, _i, _j in sorted(_cand):
            if _i in _used or _j in _used: continue
            A, B = st_list[_i], st_list[_j]
            za, zb = zone_at(b, A['x'], A['z']), zone_at(b, B['x'], B['z'])
            if za is None or zb is None: continue
            pa, pb = to_map(b, za, A['x'], A['z']), to_map(b, zb, B['x'], B['z'])
            if not pa or not pb or pa[0] != pb[0]: continue
            cx, cy = (pa[2] + pb[2]) // 2, (pa[3] + pb[3]) // 2
            st_merge[_i] = (pa[0], pa[1], cx, cy)
            st_merge[_j] = (pb[0], pb[1], cx, cy)
            _used.add(_i); _used.add(_j)

        for kind, lst in ((0, doors_all.get(b, [])), (1, st_list)):
            for _n, m in enumerate(lst):
                wx = m['lx'] if kind == 0 else m['x']
                wz = m['lz'] if kind == 0 else m['z']
                zi = zone_at(b, wx, wz)
                if zi is None: continue
                if kind == 1 and _n in st_merge:
                    pg, r, mx, my = st_merge[_n]
                else:
                    mp = to_map(b, zi, wx, wz)
                    if not mp: continue
                    pg, r, mx, my = mp
                if kind == 0:
                    # ---- WANDACHSE AUS DEM TUER-RECHTECK ---------------------
                    # Das Trigger-Rechteck ist LAENGS DER WAND gestreckt: man laeuft
                    # laengs der Wand auf die Tuer zu, quer dazu ist nur die Laibung
                    # tief. Gemessen in ROOM1130 (alle vier Nutzer-Beobachtungen
                    # getroffen): ->1140 1000x4000 und ->1120/1150 1000x2000 = Z
                    # gestreckt = senkrechte Wand; ->1170 2000x1000 = X = waagerecht.
                    # In ROOM1170 sind alle vier X-gestreckt, was der Nutzer dort als
                    # richtig bestaetigt hat. Rechteck ist Ecke+Ausdehnung
                    # (FUN_80042b64 @0x80042b68-7c), rw/rd sind volle Kantenlaengen.
                    if m['rw'] != m['rd']:
                        senk = m['rd'] > m['rw']
                    else:
                        R = rects(pg)[r]
                        senk = (min(mx - R[0], R[0] + R[2] - 1 - mx) <
                                min(my - R[1], R[1] + R[3] - 1 - my))
                    mx, my, mkind = snap_wall(pg, r, mx, my, senk)
                    if kachel_zeigt_tuer(pg, mx, my):
                        continue          # die Kachel malt sie bereits
                else:
                    # TREPPE: Sprossen quer zur Laufrichtung. map_x = welt_x,
                    # map_y = -welt_z, also X-Treppe -> senkrechte Sprossen.
                    mkind = 5 if m.get('axis') == 12 else 4
                vor.append({'room': b, 'idx': _n, 'kind': kind, 'pg': pg, 'r': r,
                            'mx': mx, 'my': my, 'seite': mkind,
                            'zid': zid_of.get((b, zi), 0), 'd': m})

    # ============ DURCHGANG 2: EINE TUER = EINE MARKE (auch ueber Raumgrenzen) ====
    # Nutzer 2026-08-31: "sobald ich einen Raum betreten habe, wird die Tuer noch ein
    # 2. mal gezeichnet. Aber es ist die gleiche Tuer. Die darf nur einmal gezeichnet
    # werden." Ein Durchgang zwischen zwei Raeumen traegt je Raum EINEN Datensatz;
    # liegen beide Raeume auf derselben Kartenseite, zeichnete die Karte ihn zweimal.
    # Gemessen auf Seite 4: ROOM1130 (152,125) und ROOM1120 (153,125) sind derselbe
    # Durchgang, ebenso (144,87)/(145,84) [1130<->1150] und (160,150)/(160,149)
    # [1130<->1140].
    #
    # Das Paar ist aus den Daten erkennbar und braucht keine Annahme: der ZIELPUNKT
    # des einen Datensatzes (nx/nz = Spawn hinter der Tuer, im Zielraum) liegt am ORT
    # des anderen, und umgekehrt. Gemessen ROOM1130<->ROOM1170: 1170s Tuer zielt auf
    # (2650,15550), 1130s Tuer liegt bei (2700,14650) - 950 Einheiten; rueckwaerts
    # 950. Dieselbe Regel deckt die SELBST-Tuer ab (Zielraum == eigener Raum).
    tueren = [v for v in vor if v['kind'] == 0]
    kand = []
    for _i in range(len(tueren)):
        A = tueren[_i]
        for _j in range(_i + 1, len(tueren)):
            B = tueren[_j]
            if A['d']['dest'] != B['room'] or B['d']['dest'] != A['room']: continue
            d1 = abs(A['d']['nx'] - B['d']['lx']) + abs(A['d']['nz'] - B['d']['lz'])
            d2 = abs(B['d']['nx'] - A['d']['lx']) + abs(B['d']['nz'] - A['d']['lz'])
            if d1 > 4000 or d2 > 4000: continue
            kand.append((d1 + d2, _i, _j))
    belegt = set()
    for _d, _i, _j in sorted(kand):
        if _i in belegt or _j in belegt: continue
        A, B = tueren[_i], tueren[_j]
        belegt.add(_i); belegt.add(_j)
        if A['pg'] != B['pg']:
            continue        # verschiedene Blaetter: jede Seite zeigt ihre eigene Tuer
        # ⛔ NICHT MITTELN. Die beiden Rechtecke liegen auf dem Blatt so, wie der
        # Kuenstler sie gesetzt hat; unsere zwei linearen Projektionen koennen weit
        # auseinander fallen (gemessen ROOM1130 <-> ROOM1120: (152,125) gegen
        # (120,149), 34 px). Der Mittelwert lag dann ausserhalb BEIDER Rechtecke.
        # Genommen wird die Position, die am dichtesten am NACHBAR-Rechteck liegt -
        # das ist die, die wirklich auf der gemeinsamen Kante sitzt.
        def _abstand(X, Y):
            RX, RY, RW, RH = rects(Y['pg'])[Y['r']]
            dx = max(RX - X['mx'], 0, X['mx'] - (RX + RW - 1))
            dy = max(RY - X['my'], 0, X['my'] - (RY + RH - 1))
            return dx + dy
        W = A if _abstand(A, B) <= _abstand(B, A) else B
        cx, cy = W['mx'], W['my']
        # Beide Datensaetze bekommen dieselbe Position UND dieselbe Wandseite, damit
        # daraus optisch EIN Symbol wird. Die Seite des ersten gewinnt (deterministisch);
        # weil sie auf derselben Wand sitzen, sind es ohnehin gespiegelte Nischen.
        for X in (A, B):
            X['mx'], X['my'], X['seite'] = cx, cy, W['seite']
        # EIN Datensatz genuegt. Beide zu behalten hiesse: dieselbe Stelle zweimal
        # zeichnen, jeder in der Farbe SEINES Rechtecks (einer gruen "besucht", einer
        # rot "aktueller Raum") - optisch wieder eine Doppelung. Der zweite wird
        # deshalb still gelegt, seine Zone aber als ZWEITE Sichtbarkeits-Zone im ersten
        # vermerkt: die Marke erscheint, sobald EINER der beiden Raeume besucht ist.
        W['zid2'] = (B if W is A else A)['zid']
        (B if W is A else A)['weg'] = True

    # ================= DURCHGANG 3: ausgeben ====================================
    seen = set()
    for v in vor:
        if v.get('weg'): continue
        key = (v['pg'], v['r'], v['mx'], v['my'], v['seite'])
        if key in seen: continue
        seen.add(key)
        marks.append((v['pg'], v['r'], v['mx'], v['my'], v['seite'], v['zid'],
                      v.get('zid2', 255)))

    o.append("")
    o.append("/* MARKEN in Karten-Pixeln. kind = TUER mit WANDSEITE 0=Nord 1=Ost")
    o.append(" * 2=Sued 3=West (die Nische zeigt nach innen), 4/5 = Treppe mit")
    o.append(" * waagerechten bzw. senkrechten Sprossen.")
    o.append(" * Treppen stammen aus den SCD-Zonen Aot_set Typ 12/13 (die Band-Wechsel-")
    o.append(" * Zonen), Tueren aus den Tuer-Datensaetzen. Gezeichnet werden sie nur fuer")
    o.append(" * Zonen, die der Spieler schon gesehen hat. */")
    o.append("typedef struct { unsigned char page, rect; short mx, my;")
    o.append("                 unsigned char kind, zid, zid2; } re15_map_mark_t;")
    o.append("static const re15_map_mark_t s_map_marks[] = {")
    for pg, r, mx, my, kind, zd, zd2 in sorted(marks):
        o.append(f"    {{ {pg:2d}, {r:2d}, {mx:4d}, {my:4d}, {kind}, {zd:3d}, {zd2:3d} }},")
    o.append("};")
    # ================= ETAGEN-TABELLE ==========================================
    # Nutzer 2026-08-31/09-01: "wenn ich im Treppenhaus oben bin, bin ich auf Ebene 3F.
    # Gehe ich die 2 Treppen runter, bin ich auf Ebene 2F, danach noch einmal runter
    # auf 1F. Bei und im Treppenhaus bin ich IMMER auf Ebene 1F laut Karte."
    #
    # Der Treppenhaus-Raum ist ROOM1060 (bzw. ROOM10A0); beide liegen auf Kartenseite 2,
    # und deren Titelbild heisst woertlich "POLICE STATION 1F" - daher die feste 1F.
    # Das Original kennt keinen Etagenbegriff innerhalb eines Raums: der Seiten-Setzer
    # @0x8004b568 liest nur die Raumnummer (Sprungtabelle @0x8001103c), und im ganzen
    # Kartencode kommt das Spieler-Band DAT_800acad6 nicht vor. Die Umschaltung ist also
    # eine PORT-ERGAENZUNG - aber sie ist vollstaendig aus den Daten ABGELEITET, nicht
    # geraten:
    #
    #   (1) Jede Tuer traegt ihr BAND (Door_aot_set pc[4], re15_aot.h) und ihren
    #       Zielraum. Der Zielraum hat eine bekannte Kartenseite, und die Seite traegt
    #       im Titelbild ihre Etage. Fuer ROOM1060 gemessen:
    #           Band 8 -> ROOM1120 (Seite 4 = "3F")
    #           Band 4 -> ROOM10C0 (Seite 3 = "2F")
    #           Band 0 -> ROOM1040 (Seite 2 = "1F")
    #       Das deckt sich exakt mit der Beobachtung: je Treppenlauf werden 2 Baender
    #       ueberquert (count), zwei Laeufe also 4 - 8 -> 4 -> 0.
    #   (2) Der Raum ist auf JEDER dieser Etagen gezeichnet, mit DERSELBEN Kachel:
    #       ROOM1060 belegt Seite 2 Rect 9 uv(168,40); Seite 3 Rect 9 und Seite 4 Rect 0
    #       tragen dieselbe uv. Das Ziel-Rechteck wird deshalb ueber die Kachel-uv
    #       gesucht - kein Raten, eine Gleichheitspruefung.
    belegte_rects = {(p_, r_) for (p_, r_) in assign.values()}
    floors = []          # (room, band, page, rect)
    for b in sorted(zinfo):
        eigene = assign.get((b, 0))
        if eigene is None: continue
        pg0, r0 = eigene
        uv0 = rect_uv(pg0, r0)
        baender = {}
        for d in doors_all.get(b, []):
            if d['rw'] == 0 and d['rd'] == 0: continue
            zp = page_of(d['dest'])
            if zp is None or zp == 0xd: continue
            baender.setdefault(d['band'], set()).add(zp)
        if len(baender) < 2: continue          # nur Raeume ueber mehrere Etagen
        for band in sorted(baender):
            ziel = sorted(baender[band])
            if len(ziel) != 1: continue        # mehrdeutig -> auslassen
            zp = ziel[0]
            # ⛔ Die uv-Gleichheit allein reicht NICHT: Kachel-Wiederverwendung ist im
            # Spiel die Regel (mehrere Seiten teilen pixelgleiche Rechtecke). Das
            # Ziel-Rechteck muss deshalb zusaetzlich FREI sein - gehoert es schon einem
            # anderen Raum, ist es nicht die Zweitzeichnung DIESES Raums. Ohne diese
            # Bedingung landete ROOM1170s Band 0 auf Seite 4 Rect 4, und das ist
            # ROOM1130.
            treffer = [i for i, _ in enumerate(rects(zp))
                       if rect_uv(zp, i) == uv0 and
                          ((zp, i) == (pg0, r0) or (zp, i) not in belegte_rects)]
            if len(treffer) != 1: continue     # Kachel dort nicht eindeutig
            floors.append((b, band, zp, treffer[0]))
    o.append("")
    o.append("/* ETAGEN: Band -> (Kartenseite, Rechteck). Aus den Tueren des Raums")
    o.append(" * abgeleitet (Band der Tuer -> Seite des Zielraums), Ziel-Rechteck ueber")
    o.append(" * die gleiche Kachel-uv gefunden. Siehe tools/gen_map_zones.py. */")
    o.append("typedef struct { unsigned short room; unsigned char band, page, rect; } re15_map_floor_t;")
    o.append("static const re15_map_floor_t s_map_floors[] = {")
    for room, band, pg, r in sorted(floors):
        for var in (0, 1):
            o.append(f"    {{ 0x{room + var:04X}, {band:2d}, {pg:2d}, {r:2d} }},")
    o.append("};")
    print(f"{len(floors)} Etagen-Eintraege")

    dst = os.path.join(ROOT, 're15_port', 'engine', 'src', 're15_map_zones.h')
    open(dst, 'w', encoding='ascii', newline=chr(10)).write(chr(10).join(o) + chr(10))
    print(f"\n{len(rows)} Zonen-Eintraege ({len(rows)//2} Zonen x 2 Varianten) -> {dst}")

if __name__ == '__main__':
    main()
