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

BASES = [0, 38, 50, 65, 77, 98]
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
                        stairs.append({'x': rx + rw//2, 'z': rz + rd//2})
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
                    doors.append({'lx': rx + rw//2, 'lz': rz + rd//2,
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
    FLOOR_FIX = {}
    for key, val in FLOOR_FIX.items():
        if key in assign: assign[key] = val

    o.append("static const re15_map_zone_t s_map_zones[] = {")
    for room, bb, pg, r, zi, zd in rows:
        o.append(f"    {{ 0x{room:04X}, {bb[0]:6d}, {bb[2]:6d}, {bb[1]:6d}, {bb[3]:6d}, {pg:2d}, {r:2d}, {zi}, {zd:3d} }},")
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
        R = rects(pg)[r]
        if x1 <= x0 or z1 <= z0: return None
        fx = min(max(wx - x0, 0), x1 - x0)
        fz = min(max(wz - z0, 0), z1 - z0)
        # z GESPIEGELT — wie die Original-Markerformel (FUN_800473f8 negiert das
        # z-Ergebnis) und wie re15_map_zones.c seit der Korrektur vom 2026-08-31.
        return pg, r, R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0)
    def zone_at(room, wx, wz):
        best, best_a = None, 0
        for i, (x0, x1, z0, z1) in enumerate(zinfo.get(room, [])):
            if x0 - GAP <= wx <= x1 + GAP and z0 - GAP <= wz <= z1 + GAP:
                a = (x1 - x0) * (z1 - z0)
                if best is None or a < best_a: best, best_a = i, a
        return best
    marks = []                      # (page, rect, mx, my, kind, zid)
    seen = set()
    zid_of = {}
    _z = 0
    for b, zs in sorted(zinfo.items()):
        for i in range(len(zs)):
            if assign.get((b, i)) is not None:
                zid_of[(b, i)] = _z; _z += 1
    for b in sorted(zinfo):
        # TUEREN (kind 0) und TREPPEN (kind 1). Beide Positionen kommen aus
        # RE1.5-Daten: Tueren aus den Tuer-Datensaetzen des Raums (SCD-Opcode
        # 0x3b/0x68, Mitte des Tuer-Rechtecks), Treppen aus den Band-Wechsel-Zonen
        # (Aot_set Typ 12/13). Die Tuer-Striche waren schon einmal drin und sassen
        # falsch — Ursache war NICHT die Position, sondern die fehlende z-Spiegelung
        # der Projektion (das Original negiert z, FUN_800473f8 @0x800474b0). Seit
        # deren Korrektur landen sie dort, wo die Tueren wirklich sind (Nutzer
        # 2026-08-31: "du koenntest die Tuer Markierungen nehmen die schon von
        # RE 1.5 kommen - dort wo die Tueren sind, koennen sie eingezeichnet werden").
        for kind, lst in ((0, doors_all.get(b, [])), (1, stairs_all.get(b, []))):
            for m in lst:
                wx = m['lx'] if kind == 0 else m['x']
                wz = m['lz'] if kind == 0 else m['z']
                zi = zone_at(b, wx, wz)
                if zi is None: continue
                mp = to_map(b, zi, wx, wz)
                if not mp: continue
                pg, r, mx, my = mp
                key = (pg, r, mx, my, kind)
                if key in seen: continue
                seen.add(key)
                marks.append((pg, r, mx, my, kind, zid_of.get((b, zi), 0)))
    o.append("")
    o.append("/* MARKEN: Tueren (kind 0) und Treppen (kind 1), bereits in Karten-Pixeln.")
    o.append(" * Treppen stammen aus den SCD-Zonen Aot_set Typ 12/13 (die Band-Wechsel-")
    o.append(" * Zonen), Tueren aus den Tuer-Datensaetzen. Gezeichnet werden sie nur fuer")
    o.append(" * Zonen, die der Spieler schon gesehen hat. */")
    o.append("typedef struct { unsigned char page, rect; short mx, my; unsigned char kind, zid; } re15_map_mark_t;")
    o.append("static const re15_map_mark_t s_map_marks[] = {")
    for pg, r, mx, my, kind, zd in sorted(marks):
        o.append(f"    {{ {pg:2d}, {r:2d}, {mx:4d}, {my:4d}, {kind}, {zd:3d} }},")
    o.append("};")
    dst = os.path.join(ROOT, 're15_port', 'engine', 'src', 're15_map_zones.h')
    open(dst, 'w', encoding='ascii', newline=chr(10)).write(chr(10).join(o) + chr(10))
    print(f"\n{len(rows)} Zonen-Eintraege ({len(rows)//2} Zonen x 2 Varianten) -> {dst}")

if __name__ == '__main__':
    main()
