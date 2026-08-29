#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_map_tables.py — generiert die beiden Kartensystem-Tabellen des Ports:

  engine/src/re15_map_rooms.h    Raum-Id -> (Seite, Rect-Index)   [RE2-Faerbung]
  engine/src/re15_map_row_fix.h  Slot -> reparierte Marker-Zeile  [Marker-Reparatur]

Eingabe: eine Final-Zuordnungs-JSON  { "1020": [seite, rect, "quelle"], ... }
(Basis-Raeume; Szenario-Varianten erben automatisch — Seite/Slot haengen nur am
Raum-Index: FUN_8004b568 liest 0x800b0fe2).

Ableitung/Belege: analysis/nutzer_batch_2026-08-30b/map-zuordnung.md.
Marker-Formel: FUN_800473f8 @0x8004741c-528 —
  px = ((wx+32000)*10*xscl >>20 +5)/10 + xoff  ->  xscl = round(ax*2^20), xoff = bx-32000*ax
  py = -((wz+32000)*10*zscl >>20 +5)/10 + yoff ->  zscl = round(az*2^20), yoff = bz+32000*az
mit T aus dem Footprint-Bbox->Rect-Fit (ax,bx,az,bz). Eine Fix-Zeile wird NUR
emittiert, wenn die EXE-Zeile @0x800768B0+8*Slot Platzhalter ist ({0,0,1,1} bzw.
Skalen <=16) ODER ihre Projektion das zugewiesene Rect verfehlt (IoU < 0.30).

Aufruf:  python re15_port/tools/gen_map_tables.py <final_assign.json>
"""
import struct, json, sys, os

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
EXE = open(os.path.join(ROOT, 'info', 'Re1.5', 'PSX.EXE'), 'rb').read()

def fo(a): return a - 0x80010000 + 0x800
def u16(a): return struct.unpack_from('<H', EXE, fo(a))[0]
def s16(a): return struct.unpack_from('<h', EXE, fo(a))[0]
def u32(a): return struct.unpack_from('<I', EXE, fo(a))[0]

def rects(page):
    cnt = u16(0x80076840 + page * 8); ptr = u32(0x80076844 + page * 8)
    return [(s16(ptr + i * 12), s16(ptr + i * 12 + 2),
             s16(ptr + i * 12 + 4), s16(ptr + i * 12 + 6)) for i in range(cnt)]

def rowv(slot):
    a = 0x800768B0 + slot * 8
    return s16(a), u16(a + 2), u16(a + 4), u16(a + 6)

BASES = [0, 38, 50, 65, 77, 98]
def page_slot(rid):
    stage = (rid >> 12) - 1; room = (rid >> 4) & 0xFF
    if stage == 0:
        if room >= 0x26: return None
        pg = 2 if room <= 11 else 3 if room <= 17 else 4 if room <= 22 \
            else 5 if room == 23 else 0 if room <= 29 else 1
        return pg, BASES[0] + room
    if stage == 1: return None if room >= 0xc else ((6 if room <= 9 else 0xd), BASES[1] + room)
    if stage == 2: return None if room >= 0x20 else (7, BASES[2] + room)
    if stage == 3: return None if room >= 0xc else (8, BASES[3] + room)
    if stage == 4:
        if room >= 0x15: return None
        pg = 0xa if 12 <= room <= 14 else 0xb if room in (15, 16) else 9
        return pg, BASES[4] + room
    if stage == 5: return None if room >= 0x8 else (0xc, BASES[5] + room)
    return None

def project(r, wx, wz):
    xoff, yoff, xscl, zscl = r
    t = (((wx + 32000) * 10 * xscl) & 0xFFFFFFFF)
    if t >= 0x80000000: t -= 0x100000000
    t = (t >> 20) + 5; t = int(t / 10) if t >= 0 else -int(-t / 10)
    t2 = (((wz + 32000) * 10 * zscl) & 0xFFFFFFFF)
    if t2 >= 0x80000000: t2 -= 0x100000000
    t2 = (t2 >> 20) + 5; t2 = -t2; t2 = int(t2 / 10) if t2 >= 0 else -int(-t2 / 10)
    return t + xoff, t2 + yoff

NAMES = ["collision", "camera", "zone", "light", "md1ptr", "floor", "block", "message",
         "mainScd", "subScd", "extraScd", "effect", "x50", "espTim", "modelTim", "anim"]

def footprint(rid):
    for var in (0, 1):
        rn = rid + var
        stage = (rn >> 12)
        p = os.path.join(ROOT, 're15_port', 'shared_assets', 'PSX',
                         f'STAGE{stage}', f'ROOM{rn:04X}.RDT')
        if not os.path.exists(p): continue
        d = open(p, 'rb').read()
        if len(d) < 0x100: continue
        s = struct.unpack_from('<I', d, 0x20)[0]
        if not (0 < s < len(d) - 24): continue
        counts = struct.unpack_from('<5I', d, s + 4)
        n = sum(counts)
        if not (0 < n < 2000 and s + 24 + 12 * n <= len(d)): continue
        xs = []; zs = []; xe = []; ze = []
        for i in range(n):
            w, dep, x, z = struct.unpack_from('<HHhh', d, s + 24 + 12 * i)
            xs.append(x); zs.append(z); xe.append(x + w); ze.append(z + dep)
        return min(xs), max(xe), min(zs), max(ze)
    return None

def iou(a, b):
    ax0, ay0, ax1, ay1 = a; bx0, by0, bx1, by1 = b
    w = min(ax1, bx1) - max(ax0, bx0); h = min(ay1, by1) - max(ay0, by0)
    inter = max(0, w) * max(0, h)
    ua = (ax1 - ax0) * (ay1 - ay0) + (bx1 - bx0) * (by1 - by0) - inter
    return inter / ua if ua > 0 else 0

def main():
    assign = json.load(open(sys.argv[1]))
    rows = []       # (room, page, rect, quelle)
    fixes = []      # (slot, room, xoff, yoff, xscl, zscl, grund)
    for k, v in sorted(assign.items()):
        base = int(k, 16); page, rect, src = v[0], v[1], v[2]
        ps = page_slot(base)
        assert ps and ps[0] == page, f"{k}: Seite {page} != stage-init {ps}"
        for var in (0, 1):
            rows.append((base + var, page, rect, src))
        fp = footprint(base)
        if not fp: continue
        R = rects(page)[rect]
        # Marker-Reparatur noetig?
        slot = ps[1]
        er = rowv(slot)
        need = er[2] <= 16 or er[3] <= 16
        if not need:
            x0, y1 = project(er, fp[0], fp[2]); x1, y0 = project(er, fp[1], fp[3])
            need = iou((x0, y0, x1, y1), (R[0], R[1], R[0] + R[2], R[1] + R[3])) < 0.30
        if need:
            ax = R[2] / (fp[1] - fp[0]); az = R[3] / (fp[3] - fp[2])
            bx = R[0] - ax * fp[0]; bz = R[1] + az * fp[3]
            xscl = round(ax * (1 << 20)); zscl = round(az * (1 << 20))
            xoff = round(bx - 32000 * ax); yoff = round(bz + 32000 * az)
            assert 0 < xscl < 65536 and 0 < zscl < 65536 and -32768 <= xoff < 32768 and 0 <= yoff < 65536, (k, xoff, yoff, xscl, zscl)
            grund = 'Platzhalter-Zeile' if (er[2] <= 16 or er[3] <= 16) else f'EXE-Zeile verfehlt Rect (IoU<0.30, EXE={er})'
            fixes.append((slot, base, xoff, yoff, xscl, zscl, grund))
    # ---- re15_map_rooms.h ----
    o = []
    o.append("/* GENERIERT von tools/gen_map_tables.py (Ableitung: analysis/nutzer_batch_")
    o.append(" * 2026-08-30b/map-zuordnung.md): Raum-Id -> (Karten-Seite, Rect-Index der")
    o.append(" * EXE-Rect-Liste @0x80076840). Quellen je Basis-Raum: 'anker' = Footprint-")
    o.append(" * Projektion durch die EXE-eigene Marker-Zeile (@0x800768B0+8*Slot, Formel")
    o.append(" * FUN_800473f8 @0x8004741c-528), bestes IoU-Rect; 'tuer' = Tuer-Graph-Solver")
    o.append(" * (Transform-Fit Footprint->Rect, Tuer-Positions-Kosten beider Tuerseiten);")
    o.append(" * 'visuell' = Pruef-Agent-Korrektur am gerenderten Composite. Szenario-")
    o.append(" * Varianten erben die Basis-Zuordnung. Mehrere Raeume je Rect sind legitim.")
    o.append(" * Nicht zugeordnete Raeume fehlen bewusst -> Rect bleibt im Stock-Neutralton. */")
    o.append("typedef struct { unsigned short room; unsigned char page, rect; } re15_map_room_rect_t;")
    o.append("static const re15_map_room_rect_t s_map_room_rects[] = {")
    for room, page, rect, src in rows:
        o.append(f"    {{ 0x{room:04X}, {page:2d}, {rect:2d} }},  /* {src} */")
    o.append("};")
    open(os.path.join(ROOT, 're15_port', 'engine', 'src', 're15_map_rooms.h'), 'w').write('\n'.join(o) + '\n')
    # ---- re15_map_row_fix.h ----
    o = []
    o.append("/* GENERIERT von tools/gen_map_tables.py — MARKER-REPARATUR (Nutzer-Auftrag")
    o.append(" * 2026-08-30: 'die Karte reparieren'). 65 der 120 Basis-Raeume tragen im")
    o.append(" * Auslieferungsstand nur die Platzhalter-Marker-Zeile {0,0,1,1} (der Stock-")
    o.append(" * Positionsmarker projiziert dort auf ~(0,0)); einzelne weitere Zeilen")
    o.append(" * verfehlen ihr Rect. Diese Tabelle liefert fuer solche Slots reparierte")
    o.append(" * Parameter derselben Formel FUN_800473f8 @0x8004741c-528, hergeleitet aus")
    o.append(" * dem Kollisions-Footprint des Raums (RDT-SCA) und seinem zugeordneten Rect:")
    o.append(" *   xscl=round(ax*2^20), xoff=bx-32000*ax, zscl=round(az*2^20), yoff=bz+32000*az.")
    o.append(" * RE15_MAP_STOCK=1 schaltet die Reparatur ab (byte-true Auslieferungsstand).")
    o.append(" * Typedef re15_map_row_fix_t: re15_room.h. */")
    o.append("static const re15_map_row_fix_t s_map_row_fixes[] = {")
    for slot, room, xoff, yoff, xscl, zscl, grund in sorted(fixes):
        o.append(f"    {{ {slot:3d}, {xoff:5d}, {yoff:5d}, {xscl:5d}, {zscl:5d} }},  /* 0x{room:04X}: {grund} */")
    o.append("};")
    open(os.path.join(ROOT, 're15_port', 'engine', 'src', 're15_map_row_fix.h'), 'w').write('\n'.join(o) + '\n')
    print(f"{len(rows)} Zuordnungs-Eintraege ({len(rows)//2} Basis-Raeume), {len(fixes)} Marker-Fixes")

if __name__ == '__main__':
    main()
