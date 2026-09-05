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
import struct, json, os, sys, math, collections, random, statistics
import io
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))  # fuer grundriss.py

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
EXE  = open(os.path.join(ROOT, 'info', 'Re1.5', 'PSX.EXE'), 'rb').read()
GAP  = 1500          # Beruehrungs-Toleranz beim Verschmelzen (Welt-Einheiten)
# Bis zu wievielen Pixeln Abweichung darf man die beiden Projektionen einer
# Tuer MITTELN? Darueber liegen die Zeichnungen gegenlaeufig, und das Mittel
# legt die Marke dorthin, wo KEINE Seite eine Tuer hat (ROOM1210<->ROOM1220).
MITTEL_MAX = 4
MIN_FRAC = 0.03      # Splitter unter diesem Anteil der Raumflaeche verwerfen

def fo(a): return a - 0x80010000 + 0x800
def u16(a): return struct.unpack_from('<H', EXE, fo(a))[0]
def s16(a): return struct.unpack_from('<h', EXE, fo(a))[0]
def u32(a): return struct.unpack_from('<I', EXE, fo(a))[0]

# ---- ⛔ BLATT 3 (2F) TRAEGT IM ORIGINAL DIE TABELLE VON BLATT 2 (1F) -------------
# Nutzer-Befund 2026-09-04 (fehler/error1.png + expect.png): auf 2F fehlt das Rechteck
# unter dem Treppensymbol, und "ich haette das Treppenhaus woanders erwartet, naemlich
# wie in expect.png dargestellt".
#
# GEMESSEN, nicht geschlossen:
#   1. Die beiden Seiten-Tabellen sind VERSCHIEDENE Adressen (Blatt 2 @0x8007636C,
#      Blatt 3 @0x800763F0), ihre Eintraege aber BYTE-IDENTISCH - Blatt 3 hat nur den
#      letzten der elf nicht. Zwei verschiedene Stockwerke koennen nicht dieselbe
#      Kachel-Geometrie haben.
#   2. Jedes Blatt DATA/MAP0x.PIX enthaelt zweierlei: oben einen Streifen aus einzeln
#      gezeichneten Raum-KACHELN (die Quelle der SPRTs) und unten den fertigen
#      GRUNDRISS des Stockwerks. Baut man die Seite nach ihrer Tabelle zusammen, kommt
#      genau dieser Grundriss heraus - Blatt 2 zu 93,0 % Jaccard, Blatt 4 zu 98,7 %.
#      Blatt 3 kommt auf 57,3 %: die 1F-Kachelecken schneiden die 2F-Kunst an den
#      falschen Stellen, und die untere Haelfte des 2F-Streifens wird nie abgetastet.
#      Genau diesen ungenutzten Grundriss zeigt expect.png - der Nutzer hat recht.
#   3. Der 2F-Streifen enthaelt 10 Raumkacheln (Zusammenhangskomponenten >= 80 px,
#      ohne Kompass/Massstab) - genau so viele Rechtecke fuehrt die Seite.
#
# HERLEITUNG DER ERSATZTABELLE (tools/karte_2f_tabelle.py schreibt dieselben Zahlen):
#   * uv = Ecke der Streifen-Komponente; w/h = ihre Groesse auf das naechste Vielfache
#     von 8 aufgerundet (an allen 7 Kacheln von Blatt 4 und 8 von 11 auf Blatt 2
#     nachgemessen - so schneidet das Original).
#   * Die Bildschirmlage kommt aus dem GRUNDRISS des Blatts: jede Kachel wird dort per
#     Schablonensuche wiedergefunden (8 von 10 zu 100,0 %, eine zu 94,2 %, die kleinste
#     zu 100,0 %). Bildschirm = Fundstelle + Versatz.
#   * Der Versatz (-25,-71) ist ebenfalls GEMESSEN, nicht gewaehlt. Drei unabhaengige
#     Herleitungen treffen sich:
#       1. Blatt 2 (1F) hat denselben Versatz: passt man seinen Zusammenbau auf den
#          Grundriss seines eigenen Blatts, kommt (-25,-71) heraus (Jaccard 0,930).
#       2. ROOM1080, die Fahrstuhlkabine, ist auf 1F UND 2F gezeichnet - dieselbe
#          Kachel uv(168,40). Blatt 2 setzt sie auf (109,134); die 2F-Fundstelle
#          (134,205) minus (-25,-71) ergibt (109,134). EXAKT.
#       3. ROOM1060, das Treppenhaus, ebenso: uv(168,16), Blatt 2 (119,134),
#          2F-Fundstelle (143,205) -> (118,134). EIN Pixel.
#     In die Rechnung, die die 2F-Kacheln im Grundriss verortet, ist von Blatt 2
#     nichts eingegangen - die beiden Schaechte sind eine echte Gegenprobe.
#     (Erste Fassung: die Seite auf die Bildschirmmitte der intakten Blaetter setzen,
#     (-23,-74). Das war eine WAHL; die Schaechte lagen damit 1-3 px neben ihren
#     1F-Positionen. Verworfen zugunsten des Belegs.)
#   * GEGENPROBE: der Zusammenbau trifft den Grundriss des Blatts zu 93,1 % Jaccard,
#     also so gut wie die echte Tabelle von Blatt 2 (93,0 %).
# Kontrollschalter fuer die Gegenprobe: RE15_KEIN_RECT_FIX=1 schaltet die
# Ersatztabelle ab, damit sich messen laesst, was allein an ihr haengt.
RECT_FIX = {} if os.environ.get('RE15_KEIN_RECT_FIX') == '1' else {
    3: [(102, 116,  40, 40, 128, 16),
        (118, 134,  24, 24, 168, 16),
        (135, 155,  56, 32, 192, 16),
        (135,  76,  72, 88,   0, 32),
        (109, 134,  16, 16, 168, 40),
        (146, 114,  72, 32, 184, 48),
        (186, 114,  40, 48,  72, 64),
        (179,  67,  48, 48, 112, 74),
        (102,  76,  48, 48, 160, 80),
        (156,  76,  48, 40, 208, 80)],
}

def rects(page):
    if page in RECT_FIX:
        return [(r[0], r[1], r[2], r[3]) for r in RECT_FIX[page]]
    cnt = u16(0x80076840 + page * 8); ptr = u32(0x80076844 + page * 8)
    return [(s16(ptr + i*12), s16(ptr + i*12+2), s16(ptr + i*12+4), s16(ptr + i*12+6))
            for i in range(cnt)]

def rect_uv(page, i):
    """Kachel-Ecke (u,v) des Rechtecks: Byte +8 und +10 des 12-Byte-Eintrags."""
    if page in RECT_FIX:
        return RECT_FIX[page][i][4], RECT_FIX[page][i][5]
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

_maskcache = {}
def _maske(pg, r):
    if (pg, r) in _maskcache: return _maskcache[(pg, r)]
    pix = page_pix(pg)
    if pix is None: _maskcache[(pg, r)] = set(); return _maskcache[(pg, r)]
    RX, RY, RW, RH = rects(pg)[r]; U, V = rect_uv(pg, r); m = set()
    for j in range(RH):
        for i in range(RW):
            if V + j < 256 and U + i < 256 and pix[V + j][U + i] != 0: m.add((RX + i, RY + j))
    _maskcache[(pg, r)] = m
    return m

_bercache = {}
def _beruehren(pg, a, b):
    """Beruehren sich die GEZEICHNETEN Flaechen zweier Rechtecke? Zwei Raeume, die eine
    Tuer verbindet, muessen das - der Kuenstler hat sie aneinander gesetzt. Das ist eine
    Aussage der KACHEL und damit unabhaengig von unserer Projektion."""
    if a == b: return True
    k = (pg, min(a, b), max(a, b))
    if k in _bercache: return _bercache[k]
    ma = _maske(pg, a); mb = _maske(pg, b); hit = False
    for (x, y) in ma:
        if hit: break
        for dx, dy in ((1,0),(-1,0),(0,1),(0,-1),(1,1),(-1,-1),(1,-1),(-1,1)):
            if (x + dx, y + dy) in mb: hit = True; break
    _bercache[k] = hit
    return hit

# ⛔ WAS ES KOSTET, EINE ZONE GAR NICHT ZUZUORDNEN — BLEIBT BEI 30.
# Ein A/B gegen die aus der ausgelieferten Zeile @0x800768b0 bekannten Zuordnungen sah
# hoehere Werte besser aus:
#     30 -> 22/30 richtig, 7,0 % der begehbaren Flaeche ohne Zone
#     60 -> 22/31 richtig, 4,6 %
#    120 -> 27/33 richtig, 3,9 %
#    250 -> 26/33 richtig, 3,5 %
# ⛔ ABER: dieses Guetemass kennt NUR Raeume MIT Zeile. Bei 120 warf der Zuordner die
# ganze Seite 4 um und legte ROOM1120 auf Rect 3 - das ist ROOM1170s zweiter Bereich -,
# womit die gemeinsame Tuer zu ROOM1130 49 px danebenlag (mit Rect 5 und der gemessenen
# 180-Grad-Lage: 3 px). Genau die Stub-Raeume, um die es dem Nutzer geht, sieht das
# Aggregat nicht. Ein Proxy, der die betroffene Menge nicht enthaelt, darf nicht
# entscheiden - deshalb bleibt es bei 30, und die Luecke auf Seite 3 (ROOM10D0 ohne
# Rechteck) wird offen benannt statt zugerechnet.
KEINE_ZUORDNUNG = 30.0

def kachel_zeigt_tuer(pg, r, mx, my, tol=4):
    """Zeichnet die Kachel DIESES Rechtecks an dieser Stelle schon eine Tuer?

    ⛔ Der Schluessel ist (Seite, Rechteck), NICHT die Seite allein. Die Rechtecke
    einer Seite ueberlappen sich auf dem Schirm (Seite 4: Rect 4 ueberlappt 1, 2, 3,
    5 und 6); wer alle Symbole der SEITE gegen eine Position haelt, findet mit einem
    Suchfenster von +-7 px fast immer eines. Genau das passierte bis v0.3.70: fuer
    ROOM1130 meldete der Pruefer an ALLEN VIER Tueren "malt die Kachel schon",
    obwohl der Katalog auf diesem Rechteck nur EIN Symbol kennt (147,84) - also
    wurde keine einzige eigene Marke gesetzt. Der Nutzer sah das als fehlende Tueren
    (2026-09-01: "die Tuer Richtung Ausgang 3F fehlt in der Zeichnung").
    """
    for gx, gy, gw, gh, _wand in GLYPHEN.get((pg, r), ()):
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
                # 12 B je Eintrag (lib_sca v1.5): u16 width, u16 density, s16 x, s16 z,
                # u8 type, u8 u0, u8 u1, u8 floor. Das FLOOR-Byte kam bisher nicht mit -
                # gebraucht wird es, seit die Karte "pro Raum pro ETAGE" gezeichnet wird
                # (Nutzer 2026-09-02).
                w, dep, x, z = struct.unpack_from('<HHhh', d, s + 24 + 12*i)
                typ, u0, u1, flr = struct.unpack_from('<BBBB', d, s + 24 + 12*i + 8)
                sca.append((x, z, w, dep, flr))
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
                        yaw = struct.unpack_from('<h', b, 20)[0]
                        stg, rmd = b[22], b[23]
                    else:
                        rx, rz, rw, rd = struct.unpack_from('<hhhh', b, 4)
                        nx, ny, nz = struct.unpack_from('<hhh', b, 22)
                        yaw = struct.unpack_from('<h', b, 28)[0]
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
                    doors.append({'slot': b[1], 'sce': b[2],
                                  'lx': rx + rw//2, 'lz': rz + rd//2,
                                  'rx': rx, 'rz': rz, 'rw': rw, 'rd': rd,
                                  # Door_aot_set pc[4] = das BAND (Etage) der Tuer,
                                  # obj[0x82]; das Original gattet die Interaktion
                                  # darauf (FUN_8002bd44 @0x8002bf38).
                                  'band': b[4],
                                  'nx': nx, 'ny': ny, 'nz': nz, 'yaw': yaw,
                                  'dest': ((stg+1) << 12) | (rmd << 4)})
                pc += sz
    # ⛔ DIESELBE TUER ZWEIMAL EINGETRAGEN = ZWEI STIMMEN IM LOESER.
    # Neun Verbindungen im Spiel fuehren denselben Durchgang in ZWEI Aot_set-Records
    # (gleiches Ziel, gleicher Trigger, gleicher Spawn, gleicher Yaw) - z.B.
    # ROOM1140 -> ROOM1170 beide mit Trigger (-7250,-250) und Spawn (-21937,-25713).
    # Fuer das Spiel ist das harmlos (zwei AOT-Slots auf derselben Flaeche), fuer den
    # Grundriss-Loeser nicht: er zaehlt zwei Kanten und opfert deshalb systematisch
    # jede Verbindung, die nur EINEN Record hat.
    # Genau so ist der vom Nutzer gemeldete Fall entstanden: auf Blatt 4 haelt der
    # Loeser die (doppelt gezaehlte) Verbindung ROOM1140<->ROOM1170 und laesst
    # ROOM1130<->ROOM1170 mit 62 px offen - obwohl beide physisch EINE Tuer sind und
    # die Wahl damit unentschieden waere.
    # Nutzer 2026-09-02: "ich springe beim wechsel von room 1170 zu room 1120 von dem
    # Rechteck und der Tuer oben bis hin zum Rechteck ganz weit weg."
    _gesehen = set()
    _eindeutig = []
    for _d in doors:
        _k = (_d['dest'], _d['lx'], _d['lz'], _d['nx'], _d['nz'])
        if _k in _gesehen:
            continue
        _gesehen.add(_k)
        _eindeutig.append(_d)
    doors = _eindeutig

    # ⛔ NICHT ZUSAMMENFASSEN: EIN SLOT MIT ZWEI ZIELEN BLEIBT ZWEI KANTEN.
    # 14 Raumeintraege tragen zwei Aot_set-Records auf DEMSELBEN Slot mit identischem
    # Trigger und Spawn, aber verschiedenem Ziel (ROOM5050 Slot 0 -> ROOM5040 /
    # ROOM5120, ROOM3090 Slot 0 -> ROOM30C0 / ROOM30D0, ROOM1160 Slot 0 -> ROOM1180 /
    # ROOM1230, ...). Ein Slot ist EIN AOT-Platz, der zweite Aot_set ueberschreibt den
    # ersten - es sieht also nach EINER Tuer aus, deren Ziel das Skript umschaltet.
    # Sie deshalb zu EINER Kante zusammenzufassen liegt nahe und wurde am 2026-09-03
    # gebaut und GEMESSEN - mit beiden moeglichen Wahlen:
    #
    #   ohne Zusammenfassen          <= 4 px 104 (67 %)   Audit 2 Fehler
    #   Variante der Engine behalten <= 4 px 103 (66 %)   Audit 4 Fehler
    #   zuletzt geschriebene         <= 4 px  95 (62 %)   Audit 3 Fehler
    #
    # Beide Varianten sind SCHLECHTER. Die zusaetzliche Kante ist echte Information:
    # ueber den Spielverlauf fuehrt die Tuer wirklich zu BEIDEN Nachbarn, und auf einer
    # Schema-Karte des ganzen Gebaeudes sind beide Verbindungen real. Sie haelt das
    # Blatt zusammen; ohne sie fallen Raeume auseinander (getrennte Nachbarn 1 -> 2).
    # Zurueckgenommen - die Begruendung steht hier, damit die Idee nicht ungeprueft
    # wiederkehrt.
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
        xi, zi, wi, di = sca[i][:4]
        for j in range(i+1, n):
            xj, zj, wj, dj = sca[j][:4]
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
    # ⛔ EINE INSEL IST KEIN BEREICH. Innere Zellen (Pfeiler, Moebel, Tresen) liegen
    # oft weiter als GAP von der Umfassungswand entfernt und bilden dadurch eine eigene
    # Zusammenhangskomponente - ihre Bbox steckt aber VOLLSTAENDIG in der des Raums.
    # Als eigene Zone gefuehrt richtet sie Schaden an: zone_index_at() waehlt die
    # KLEINSTE Zone, die den Spieler enthaelt, also die Insel; deren winziges Rechteck
    # wird rot gefaerbt und liegt dabei komplett unter dem gruenen Rechteck der
    # umgebenden Zone (die Op-Liste wird von hinten gerastert). Der Raum sah dadurch
    # NIE rot aus - Nutzer 2026-09-03: "ROOM 1070 wird nicht rot hervorgehoben wenn ich
    # drin stehe." Betroffen waren ROOM1020, ROOM1070, ROOM1110, ROOM1140, ROOM11F0
    # und ROOM30C0.
    behalten = []
    for i, o in enumerate(out):
        bb = o[0]
        drin = False
        for j, p in enumerate(out):
            if i == j:
                continue
            qb = p[0]
            if (qb[0] <= bb[0] and bb[1] <= qb[1] and qb[2] <= bb[2] and bb[3] <= qb[3]
                    and p[1] > o[1]):
                drin = True
                break
        if not drin:
            behalten.append(o)
    return [o[0] for o in behalten]

def _lebende_paare():
    """Raumpaare, die die ENGINE beim Betreten in mindestens EINER Richtung aufstellt.

    Quelle: tools/engine_tueren.txt (erzeugt von integration_map_uebergang mit
    RE15_TUER_DUMP). Fehlt die Datei, wird nicht gefiltert - der Generator laeuft dann
    wie zuvor, nur ohne diese Verbesserung."""
    pfad = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'engine_tueren.txt')
    if not os.path.exists(pfad):
        return None
    paare = set()
    with io.open(pfad, encoding='utf-8') as f:
        for z in f:
            z = z.strip()
            if not z or z.startswith('#'):
                continue
            t = z.split()
            if len(t) < 2:
                continue
            try:
                a = int(t[0], 16)
                b = int(t[1], 16)
            except ValueError:
                continue
            paare.add((a & 0xFFF0, b & 0xFFF0))
    return paare or None


def _paar_detail(B, pg):
    """Alle Anlege-Lagen des kantenreichsten Paares mit ihren Resten."""
    paare = collections.defaultdict(list)
    for idx, (a, pa, b, pb) in enumerate(B.kanten):
        paare[tuple(sorted((a, b)))].append(idx)
    if not paare:
        return
    schl = max(paare, key=lambda k: len(paare[k]))
    idxs = paare[schl]
    if len(idxs) < 2:
        return
    a0, b0 = schl
    print("   [Detail Blatt %d] ROOM%04X z%d <-> ROOM%04X z%d, %d Kanten"
          % (pg, a0 >> 4, a0 & 15, b0 >> 4, b0 & 15, len(idxs)))
    gesehen = set()
    for i in idxs:
        (a, pa, b, pb) = B.kanten[i]
        for st in B.anlegen(a, (0.0, 0.0, 0, 0), pa, b, pb):
            k = (round(st[0], 1), round(st[1], 1), st[2], st[3])
            if k in gesehen:
                continue
            gesehen.add(k)
            lage = {a: (0.0, 0.0, 0, 0), b: st}
            reste = []
            for j in idxs:
                (a2, pa2, b2, pb2) = B.kanten[j]
                reste.append(B.kantenrest(lage, a2, pa2, b2, pb2))
            print("      Anker Kante %d -> dx=%7.1f dy=%7.1f k=%d sp=%d  Reste %s"
                  % (i, st[0], st[1], st[2], st[3],
                     " ".join("%5.1f" % r for r in reste)))


def _paar_probe(B, pg):
    """Wie viele Kanten eines Raumpaares kann EINE Anlege-Lage gleichzeitig erfuellen?"""
    paare = collections.defaultdict(list)
    for idx, (a, pa, b, pb) in enumerate(B.kanten):
        paare[tuple(sorted((a, b)))].append(idx)
    for schl, idxs in sorted(paare.items()):
        if len(idxs) < 2:
            continue
        a0, b0 = schl
        bestn, bestst = 0, None
        for i in idxs:
            (a, pa, b, pb) = B.kanten[i]
            for st in B.anlegen(a, (0.0, 0.0, 0, 0), pa, b, pb):
                lage = {a: (0.0, 0.0, 0, 0), b: st}
                n = 0
                for j in idxs:
                    (a2, pa2, b2, pb2) = B.kanten[j]
                    if a2 not in lage or b2 not in lage:
                        continue
                    if B.kantenrest(lage, a2, pa2, b2, pb2) <= 1.0:
                        n += 1
                if n > bestn:
                    bestn, bestst = n, (st, a, b)
        # was erreicht die AKTUELLE Loesung?
        print("   [Paar Blatt %d] ROOM%04X z%d <-> ROOM%04X z%d: %d Kanten, EINE Lage "
              "schafft hoechstens %d%s"
              % (pg, a0 >> 4, a0 & 15, b0 >> 4, b0 & 15, len(idxs), bestn,
                 ("  (Drehung %d, Spiegel %d)" % (bestst[0][2], bestst[0][3]))
                 if bestst else ""))


def _ort_probe(B, pg, spec):
    """Alle Lagen EINES Ortes durchmessen: Tuerreste und Eindringtiefen."""
    import grundriss as _g
    _, raum, zone = spec.split(':')
    ziel = (int(raum.replace('ROOM', ''), 16) << 4) | int(zone)
    lage = B.beste_lage()
    if ziel not in lage:
        print("   [Ortprobe] ROOM%04X z%d liegt nicht auf Blatt %d"
              % (ziel >> 4, ziel & 15, pg))
        return
    meine = [(a, pa, b, pb) for (a, pa, b, pb) in B.kanten if ziel in (a, b)]
    print("   [Ortprobe Blatt %d] ROOM%04X z%d, %d eigene Kanten"
          % (pg, ziel >> 4, ziel & 15, len(meine)))

    def bewerte(st):
        alt = lage[ziel]
        lage[ziel] = st
        reste = []
        for (a, pa, b, pb) in meine:
            if a in lage and b in lage:
                reste.append((B.kantenrest(lage, a, pa, b, pb),
                              'ROOM%04X' % ((b if a == ziel else a) >> 4)))
        ka = B.kasten(ziel, st)
        tiefen = []
        for o in lage:
            if o == ziel:
                continue
            kb = B.kasten(o, lage[o])
            ox = min(ka[0]+ka[2], kb[0]+kb[2]) - max(ka[0], kb[0])
            oy = min(ka[1]+ka[3], kb[1]+kb[3]) - max(ka[1], kb[1])
            if ox > 0 and oy > 0 and min(ox, oy) > _g.WAND_RAND:
                tiefen.append((min(ox, oy), 'ROOM%04X' % (o >> 4)))
        k = B.kosten(lage)
        lage[ziel] = alt
        return reste, tiefen, k

    kand = [('AKTUELL', lage[ziel])]
    # ⛔ AUCH DIE ZWISCHENSCHRITTE. anlegen() liefert nur Lagen, die eine Tuer EXAKT
    # treffen. Die Frage "was kostet es, das Rechteck ein Stueck herauszuschieben"
    # beantwortet keine davon - dafuer wird die aktuelle Lage in 1-px-Schritten
    # verschoben und jeder Schritt mit demselben Mass bewertet.
    _ax, _ay, _ak, _asp = lage[ziel]
    for _dx in range(-10, 11):
        for _dy in range(-10, 11):
            if _dx == 0 and _dy == 0:
                continue
            kand.append(('x%+d y%+d' % (_dx, _dy), (_ax + _dx, _ay + _dy, _ak, _asp)))
    for (a, pa, b, pb) in meine:
        wer, anker, p_anker, p_wer = (b, a, pa, pb) if a != ziel else (a, b, pb, pa)
        for st in B.anlegen(anker, lage[anker], p_anker, wer, p_wer):
            kand.append(('an ROOM%04X' % (anker >> 4), st))
    gesehen = set()
    for name, st in kand:
        sch = tuple(round(v, 1) for v in st)
        if sch in gesehen:
            continue
        gesehen.add(sch)
        reste, tiefen, k = bewerte(st)
        print("      %-14s Tuerreste %-30s | steckt in %-24s | getrennt %d, deckung %d"
              % (name,
                 ", ".join("%s %.0f px" % (t, v) for v, t in reste) or "-",
                 ", ".join("%s %d px" % (t, v) for v, t in tiefen) or "NICHTS",
                 k[5], k[1]))


def _vollsuche(B, pg):
    """Erschoepfende Tiefensuche: wie viele Tuerkanten sind GLEICHZEITIG erfuellbar?

    Jede Kante, an der ein noch nicht gesetzter Ort haengt, liefert ueber B.anlegen()
    alle Lagen, in denen dieser Ort mit seiner Tuerwand exakt auf der Wand des schon
    gesetzten Ortes liegt. Mehr Freiheit gibt es nicht - eine Kante ist entweder ueber
    eine dieser Lagen erfuellt oder gar nicht. Damit ist die Suche endlich und
    vollstaendig."""
    import itertools
    orte = list(B.zimmer)
    nachbarn = collections.defaultdict(list)
    for _i, (a, pa, b, pb) in enumerate(B.kanten):
        nachbarn[a].append((_i, b, pa, pb))
        nachbarn[b].append((_i, a, pb, pa))
    import grundriss as _g
    bestes = {'n': -1, 'vers': 10 ** 9, 'lage': None}
    sauber = {'n': -1, 'lage': None}      # beste Lage OHNE Versenkung
    gesehen = set()

    def versenkt(lage):
        """Paare, die tiefer als eine Wand (WAND_RAND) ineinanderstecken."""
        v = 0
        ks = [B.kasten(o, lage[o]) for o in lage]
        for _p in range(len(ks)):
            for _q in range(_p + 1, len(ks)):
                ka, kb = ks[_p], ks[_q]
                ox = min(ka[0] + ka[2], kb[0] + kb[2]) - max(ka[0], kb[0])
                oy = min(ka[1] + ka[3], kb[1] + kb[3]) - max(ka[1], kb[1])
                if ox > _g.WAND_RAND and oy > _g.WAND_RAND:
                    v += 1
        return v

    def erfuellt(lage):
        n = 0
        for (a, pa, b, pb) in B.kanten:
            if a not in lage or b not in lage:
                continue
            qa = B.wandpunkt(a, lage[a], B.punkt(a, lage[a], *pa))
            qb = B.wandpunkt(b, lage[b], B.punkt(b, lage[b], *pb))
            if max(abs(qa[0] - qb[0]), abs(qa[1] - qb[1])) <= 1.0:
                n += 1
        return n

    def tiefer(lage, tiefe):
        if len(lage) > tiefe:
            pass
        schluessel = tuple(sorted((o, tuple(round(v, 1) for v in lage[o])) for o in lage))
        if schluessel in gesehen:
            return
        gesehen.add(schluessel)
        if len(gesehen) > 400000:
            return
        n = erfuellt(lage)
        if len(lage) == len(orte):
            # ⛔ ERST DIE KANTEN, DANN DIE OPTIK. Unter den kantenreichsten Lagen wird
            # die mit der geringsten Versenkung behalten - so beantwortet die Suche
            # BEIDE Fragen: wie viele Tueren gehen, und muss dabei wirklich ein
            # Rechteck im anderen stecken?
            v = versenkt(lage)
            if (n, -v) > (bestes['n'], -bestes['vers']):
                bestes['n'] = n
                bestes['vers'] = v
                bestes['lage'] = dict(lage)
            # Die GEGENFRAGE: wie viele Tueren gehen, wenn KEIN Rechteck im anderen
            # stecken darf? Erst beide Zahlen zusammen sagen, was der Tausch kostet.
            if v == 0 and n > sauber['n']:
                sauber['n'] = n
                sauber['lage'] = dict(lage)
        # ⛔ UEBER JEDE ANLEGEKANTE VERZWEIGEN. Ein erster Wurf nahm nur die erste
        # gefundene Kante und war damit unvollstaendig: welche Kante man zum Anlegen
        # benutzt, bestimmt die Lage des Ortes, und eine andere Kante haette eine Lage
        # ergeben koennen, die MEHR Kanten erfuellt. Die Aussage "hoechstens N" waere
        # dann ein Suchartefakt gewesen.
        # Der zu setzende Ort wird deterministisch gewaehlt (der erste offene Nachbar
        # ueberhaupt) - das ist erlaubt, weil die Reihenfolge der Orte das Ergebnis nicht
        # aendert. Verzweigt wird ueber ALLE Kanten, die ihn an etwas schon Gesetztes
        # binden koennen.
        ziel = None
        for o in list(lage):
            for (_i, nb, po, pnb) in nachbarn[o]:
                if nb not in lage:
                    ziel = nb
                    break
            if ziel is not None:
                break
        if ziel is not None:
            for o in list(lage):
                for (_i, nb, po, pnb) in nachbarn[o]:
                    if nb != ziel:
                        continue
                    for st in B.anlegen(o, lage[o], po, nb, pnb):
                        lage[ziel] = st
                        tiefer(lage, tiefe + 1)
                        del lage[ziel]
            return
        # kein Nachbar mehr erreichbar: Rest frei danebenstellen
        # ⛔ AUCH HIER DIE VERSENKUNG BEWERTEN. Blatt 4 hat 8 Orte, aber nur 6 Kanten -
        # die beste Lage kommt deshalb aus DIESEM Zweig, und ohne die Bewertung blieb
        # der Sentinel stehen und die Suche meldete Unsinn (1000000000 Paare).
        if len(lage) < len(orte):
            v = versenkt(lage)
            if (n, -v) > (bestes['n'], -bestes['vers']):
                bestes['n'] = n
                bestes['vers'] = v
                bestes['lage'] = dict(lage)
            if v == 0 and n > sauber['n']:
                sauber['n'] = n
                sauber['lage'] = dict(lage)

    for w in orte:
        gesehen.clear()
        tiefer({w: (0.0, 0.0, 0, 0)}, 0)
    print("   [Vollsuche Blatt %d] %d Orte, %d Kanten -> hoechstens %d gleichzeitig "
          "erfuellbar" % (pg, len(orte), len(B.kanten), bestes['n']))
    print("   [Vollsuche Blatt %d] beste Lage: %d Kanten, dabei %d Paare, die tiefer als %g px ineinanderstecken"
          % (pg, bestes['n'], bestes['vers'], _g.WAND_RAND))
    print("   [Vollsuche Blatt %d] beste Lage OHNE Versenkung: %d Kanten "
          "(= der Tausch kostet %d Tuer(en))"
          % (pg, sauber['n'], max(0, bestes['n'] - sauber['n'])))
    # ⛔ "KANTE ERFUELLT" IST JA/NEIN BEI <= 1 px UND VERDECKT DIE HOEHE DES FEHLERS.
    # Eine Tuer, die um 2 px danebenliegt, zaehlt genauso als Verlust wie eine um 40 px.
    # Fuer die Entscheidung "Tuer gegen Optik" braucht es den RESTBETRAG je Kante.
    for _name, _b in (('mit Versenkung', bestes), ('ohne Versenkung', sauber)):
        if not _b['lage']:
            continue
        _r = []
        for (a, pa, b, pb) in B.kanten:
            if a in _b['lage'] and b in _b['lage']:
                _r.append((B.kantenrest(_b['lage'], a, pa, b, pb),
                           'ROOM%04X z%d<->ROOM%04X z%d' % (a >> 4, a & 15, b >> 4, b & 15)))
        _r.sort(reverse=True)
        print("   [Vollsuche Blatt %d] %-16s Reste: %s"
              % (pg, _name, ", ".join("%s %.0f px" % (t, v) for v, t in _r)))
    if bestes['lage'] is not None:
        for (a, pa, b, pb) in B.kanten:
            lg = bestes['lage']
            if a not in lg or b not in lg:
                print("      ROOM%04X z%d <-> ROOM%04X z%d : Ort nicht gesetzt"
                      % (a >> 4, a & 15, b >> 4, b & 15))
                continue
            qa = B.wandpunkt(a, lg[a], B.punkt(a, lg[a], *pa))
            qb = B.wandpunkt(b, lg[b], B.punkt(b, lg[b], *pb))
            r = max(abs(qa[0] - qb[0]), abs(qa[1] - qb[1]))
            print("      ROOM%04X z%d <-> ROOM%04X z%d : Rest %.1f px"
                  % (a >> 4, a & 15, b >> 4, b & 15, r))


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
    sca_all = {}
    roh_zonen = {}
    for b in bases:
        got = read_rdt(b) or read_rdt(b + 1)
        if not got: continue
        sca, doors, stairs = got
        sca_all[b] = sca
        zs = zones_of(sca)
        if zs: roh_zonen[b] = zs
        doors_all[b] = doors
        stairs_all[b] = stairs

    # ---- SCHABLONEN-BEREICHE AUSSORTIEREN -------------------------------------
    # ⛔ Nutzer 2026-09-01: "Generell sind die Darstellungen in 2F noch sehr kaputt."
    #
    # GEMESSEN: 12 Kollisions-Bereiche in 11 verschiedenen Raeumen (1080, 10D0, 1100,
    # 11F0, 2050, 2060 zweimal, 30E0, 4020, 5030, 5050, 5110) bestehen aus DERSELBEN
    # Anordnung von vier Zellen - 23950x1000 oben, 1000x17000 links, 1000x17000 rechts,
    # 23950x1000 unten, also einem LEEREN RAHMEN von 23950 x 17000. Nur die vier Waende
    # sind Kollisionszellen; im Inneren kann der Spieler gar nicht stehen. Das ist kein
    # Ort, sondern eine Schablone im RDT.
    #
    # Der Zuordner nahm sie trotzdem und gab ihnen das Karten-Rechteck, waehrend der
    # ECHTE Grundriss desselben Raums leer ausging. Folge: ROOM10D0 - der 2F-Flur, an
    # dem auf dieser Etage alles haengt - verlor 88 % seiner begehbaren Flaeche,
    # ROOM3050 96 %, ROOM1020 85 %. Steht der Spieler dort, findet zone_at KEINE Zone;
    # die Karte faellt auf die Vorgabeseite zurueck und der Marker springt. Das ist
    # Report 4 ("beim Gang vom Treppenhaus durch die Tuer springt er") und Report 5.
    #
    # KRITERIUM OHNE FREIE ZAHL: die Zellen einer Zone, auf ihre Bbox-Ecke bezogen,
    # bilden eine FORM. Kommt dieselbe Form in ZWEI VERSCHIEDENEN RAEUMEN vor, ist sie
    # kopiert und beschreibt keinen Raum. Translationsinvariant, also findet es auch die
    # verschobenen Kopien (eine reine Bbox-Gleichheit fand nur 10 der 12).
    # Ein Schwellwert auf den begehbaren Anteil waere FALSCH: die Schablonen liegen bei
    # 23,4 %, aber die echten Raeume ROOM1160 (15,6 %) und ROOM50C0 (19,9 %) liegen
    # darunter - gemessen, es gibt dort keine Luecke.
    def _form(zellen, bb):
        x0, x1, z0, z1 = bb
        return frozenset((c[0] - x0, c[1] - z0, c[2], c[3]) for c in zellen
                         if x0 <= c[0] <= x1 and z0 <= c[1] <= z1)
    _raeume = collections.defaultdict(set)
    for _b, _zs in roh_zonen.items():
        for _bb in _zs:
            f = _form(sca_all.get(_b, ()), _bb)
            if f: _raeume[f].add(_b)
    _n_weg = 0
    for _b, _zs in sorted(roh_zonen.items()):
        keep = []
        for _bb in _zs:
            f = _form(sca_all.get(_b, ()), _bb)
            if f and len(_raeume[f]) >= 2: _n_weg += 1; continue
            keep.append(_bb)
        if keep: zinfo[_b] = keep
    print(f"{_n_weg} Schablonen-Bereiche aussortiert "
          f"(Zellform in mehreren Raeumen identisch)")

    # ---- BELEGTE ZUORDNUNGEN, die die Kostenheuristik nicht findet ------------
    # ⛔ ROOM10C0 -> Seite 3 Rect 4 ist ERZWUNGEN, nicht gewaehlt.
    # Nutzer 2026-09-01: "Bei 2F, wenn ich vom Treppenhaus in die Tuer gehe, springt er
    # komisch an eine andere Stelle in der Map" und "ab der 2. Etage ist die Darstellung
    # nach dem Treppenhaus immer noch ueberall komplett falsch."
    # Zwei gemessene Bedingungen legen es fest:
    #   1. Das Treppenhaus ROOM1060 liegt auf Band 4 auf Seite 3 Rect 9 (Etagen-Tabelle,
    #      aus den Tueren des Raums abgeleitet; Rect (3,9) und (2,9) sind dieselbe
    #      Zeichnung - gleiche Geometrie (109,134,16,16) und gleiche Kachel-uv).
    #   2. Zwei Raeume, die eine Tuer verbindet, muessen Rechtecke haben, deren
    #      GEZEICHNETE Flaechen sich beruehren - der Kuenstler hat sie aneinander
    #      gesetzt. An der Kachel messbar, unabhaengig von jeder Projektion.
    # ROOM10C0 ist der einzige Raum mit einer Tuer zum Treppenhaus; nur Rect 2 und Rect 4
    # beruehren Rect 9. Mit Rect 2 laesst sich der uebrige Tuergraph NICHT erfuellen
    # (ROOM10D0 haengt an vier Nachbarn, Rect 2 hat nur zwei Beruehrungen).
    # Vollstaendige Aufzaehlung: 36 Zuordnungen erfuellen alle Tuerkanten der Seite, und
    # in 36 von 36 liegt ROOM10C0 auf Rect 4. Alle anderen Zonen der Seite sind mit
    # 25-67 % unbestimmt und werden deshalb NICHT festgesetzt - die Kostenheuristik
    # verteilt sie um diese eine Vorgabe herum.
    # Der Generator hatte ROOM10C0 auf Rect 6, und Rect 6 beruehrt Rect 9 nicht: die Tuer
    # aus dem Treppenhaus fuehrte in eine Zeichnung am anderen Ende des Blattes.
    # ROOM1120 -> Seite 4 Rect 5 ist ebenfalls BELEGT (nicht gewaehlt): auf diesem
    # Rechteck liegt die Tuer nach ROOM1130 mit der gemessenen 180-Grad-Lage 5 px vom
    # gemalten Symbol (150,126) und 3 px von ROOM1130s eigener Position (156,125); alle
    # sieben Rechtecke der Seite wurden in normaler Lage durchgerechnet, das beste kam
    # auf 18/7 px. Ohne diese Vorgabe liess die lokale Suche ROOM1120 ganz weg, sobald
    # eine andere Vorgabe die Startlage verschob - die Zuordnung ist gegenueber solchen
    # Verschiebungen fragil (KEINE_ZUORDNUNG ist bewusst billig, siehe dort).
    # ⛔ TREPPENHAUS UND FAHRSTUHL WAREN VERTAUSCHT.
    # Nutzer 2026-09-01: "In ROOM 1120 haette ich erwartet, dass er das Treppenhaus
    # rechts zeichnet, nicht darueber. Da haette ich den Fahrstuhl erwartet."
    # Seite 2 fuehrt zwei kleine Zeichnungen: Rect 8 (uv 128,40) ist ein 16x17-Kasten,
    # Rect 9 (uv 168,40) ein 10x10-Kasten. Die Kostenheuristik hatte ROOM1060 (das
    # Treppenhaus) auf den KLEINEN und ROOM1080 (die Fahrstuhlkabine) auf den GROSSEN
    # gelegt. Zwei unabhaengige Messungen sagen das Gegenteil:
    #   GROESSE: ROOM1060 misst 11100x12750 Welteinheiten = 24x27 px im ausgelieferten
    #     Massstab, ROOM1080 nur 6200x6100 = 14x13 px. Der 16x17-Kasten gehoert dem
    #     Treppenhaus, der 10x10-Kasten der Kabine.
    #   TUERLAGE: ROOM1120s Tuer zur KABINE projiziert auf Seite 4 nach (135,146); die
    #     Mitte des dortigen Rect 0 - dieselbe Zeichnung wie Rect 9, uv (168,40) - liegt
    #     bei (135,145). Ein Pixel. Die Tuer zum Treppenhaus liegt 7 px daneben.
    # Damit stand auf dem 3F-Blatt die Kabine als "Treppenhaus" beschriftet an der
    # Stelle, an der der Nutzer die Kabine erwartet hat.
    # ⛔ DIE ALTE VORGABE (0x10C0, 0) -> (3, 4) IST ENTFALLEN.
    # Sie war auf der Seiten-Tabelle hergeleitet, die das Original fuer Blatt 3 fuehrt -
    # und das ist die Tabelle von Blatt 2 (BEFUND.md §44, byte-identisch bis auf den
    # elften Eintrag). Jede Aussage darin ("ROOM1060 liegt auf Rect 9", "nur Rect 2 und
    # Rect 4 beruehren Rect 9") zaehlt Rechtecke des FALSCHEN Stockwerks. Mit der
    # Ersatztabelle (RECT_FIX) haben die Indizes eine andere Bedeutung; die Zuordnung
    # kommt jetzt wieder aus der Kostenheuristik, die dieselbe Beruehrungs-Bedingung
    # verwendet.
    # Die beiden Schaechte auf Blatt 3 sind GASTLAGEN (ihre Heimat ist Blatt 2) und
    # stehen deshalb weiter unten in GAST_FIX.
    # ⛔ BLATT 3 (2F): ZWEI ZUORDNUNGEN SIND DURCH DIE DATEN ERZWUNGEN.
    # Nutzer-Befunde 2026-09-05 (fehler/error01.png, error02.png): "sobald ich eine Tuer
    # weiter gehe in den Flur werde ich auf der Map woanders hin geportet" und "wenn ich
    # noch weiter gehe ... geht garnichts mehr bei der Map. Ich werde noch nicht einmal
    # mehr angezeigt" (ROOM10D0).
    #
    # Der Tuergraph der Etage (aus den Door_aot_set-Records):
    #     ROOM1060 (Treppenhaus, Rect 1) -- ROOM10C0 -- ROOM10D0 -- {10E0, 10F0, 1100}
    #     ROOM10C0 -- ROOM1080 (Fahrstuhlkabine, Rect 4)
    # Zwei Raeume, die eine Tuer verbindet, muessen Rechtecke haben, deren gezeichnete
    # Flaechen sich beruehren. Beruehrungen auf Blatt 3 (Ersatztabelle, s. RECT_FIX):
    #     Rect 1 beruehrt {0,2,3,4}    Rect 4 beruehrt {0,1}    Rect 3 beruehrt {0,1,2,5,6,7,8,9}
    #
    #   ROOM10C0 muss Rect 1 UND Rect 4 UND das Rechteck von ROOM10D0 beruehren.
    #   Rect 0 erfuellt alle drei; Rect 2 beruehrt Rect 4 NICHT. Damit bleibt genau
    #   Rect 0 uebrig - keine Wahl, eine Folgerung.
    #
    #   ROOM10D0 misst im ausgelieferten Massstab 63 x 91 px. Das einzige Rechteck der
    #   Seite, in das das passt, ist Rect 3 (72x88); alle anderen sind hoechstens 48x48
    #   oder 72x32. (Genau daran scheiterte es bisher: mit der alten, von Blatt 2
    #   kopierten Tabelle war das groesste Rechteck der Seite 72x64, und die
    #   Kostenheuristik liess den Raum lieber ganz weg - er hatte deshalb weder rote
    #   Hervorhebung noch Spielermarker.)
    ZONE_FIX = {
        (0x1130, 0): (4, 4),
        (0x1120, 0): (4, 5),
        (0x1060, 0): (2, 8),      # Treppenhaus -> der 16x17-Kasten
        (0x1080, 0): (2, 9),      # Fahrstuhlkabine -> der 10x10-Kasten
        (0x10C0, 0): (3, 0),      # einziges Rect, das Treppenhaus UND Kabine beruehrt
        (0x10D0, 0): (3, 3),      # einziges Rect, in das 63x91 px passen
    }

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

    def zone_at(room, x, z):
        """KLEINSTE passende Zone. Zonen ueberlappen (ein Flur liegt in der Bbox des
        Saals), und zustaendig ist die engere - sonst schreibt ein Durchgang seine Marke
        in die Zeichnung des Nachbarbereichs."""
        best = None; best_a = 0
        nah = None; nah_d = None
        for i, (x0, x1, z0, z1) in enumerate(zinfo.get(room, [])):
            dx = max(x0 - x, 0, x - x1)
            dz = max(z0 - z, 0, z - z1)
            d = dx * dx + dz * dz
            if nah is None or d < nah_d:
                nah, nah_d = i, d
            if x0 - GAP <= x <= x1 + GAP and z0 - GAP <= z <= z1 + GAP:
                a = (x1 - x0) * (z1 - z0)
                if best is None or a < best_a: best, best_a = i, a
        # ⛔ RUECKFALL AUF DIE NAECHSTE ZONE - wie die Engine (zone_index_at in
        # re15_map_zones.c). Die Zonen-Bboxen decken einen unregelmaessigen Grundriss
        # nicht luecklos ab; ein Punkt kann zwischen zwei Zonen liegen. Ohne Rueckfall
        # fiel die Marke still weg - so hat ROOM2080 sein Treppensymbol verloren
        # (Treppe bei x = -19550, Zone 0 ab -17650, Zone 1 bis -21450). Und: Generator
        # und Laufzeit muessen dieselbe Zone waehlen, sonst zeichnen sie verschiedene
        # Karten.
        return best if best is not None else nah

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
                if r is None: c += KEINE_ZUORDNUNG; continue
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
                # ⛔ Der Deckel bei 40 px ist GEMESSEN unschaedlich. Ein Audit hatte
                # vermutet, er lasse 189-px-Fehlzuordnungen gratis durch; A/B ueber die
                # 32 aus der ausgelieferten Zeile bekannten Zuordnungen (Deckel 40/400/
                # 1000, Kantengewicht 1/2, zusaetzliche Beruehrungsstrafe 0/60/120):
                # ALLE Varianten liefern 21/32 - der Deckel ist nicht der Hebel.
                c += min(math.hypot(pa_[0]-pb_[0], pa_[1]-pb_[1]), 40.0)
            return c
        best = None
        rng = random.Random(9000 + pg)
        # HARTE Vorgaben dieser Seite: fest zugewiesen, nie veraendert, ihr Rechteck ist
        # fuer alle anderen gesperrt.
        fest = {}
        for _k, _v in ZONE_FIX.items():
            if _v[0] != pg: continue
            _i = key2idx.get(_k)
            if _i is not None: fest[_i] = _v[1]
        # ⛔ "GAR NICHT ZUORDNEN" NUR, WENN ES AUCH ZU WENIG RECHTECKE GIBT.
        # Nutzer-Befund 2026-09-05 (fehler/error01.png): "wenn ich noch weiter gehe in
        # 2F und aus dem Korridor raus, geht garnichts mehr bei der Map. Ich werde noch
        # nicht einmal mehr angezeigt" - ROOM10D0 hatte kein Rechteck, also weder rote
        # Hervorhebung noch Spielermarker. Auf Blatt 3 stehen 10 Rechtecke fuer 6 Zonen;
        # eine Zone wegzulassen spart dort nur Kosten, es gibt keinen Zwang dazu.
        # KEINE_ZUORDNUNG bleibt bei 30 (das A/B dort steht und gilt weiter) - die
        # Moeglichkeit entfaellt nur da, wo sie sachlich nicht noetig ist. Wo Zonen und
        # Rechtecke knapp sind (Blatt 6: 13 Zonen auf 11 Rechtecke), bleibt sie stehen,
        # sonst waere die Suche unloesbar.
        _frei = [r for r in range(len(R)) if r not in fest.values()]
        _knapp = len(_frei) < (len(zl) - len(fest))
        opts = _frei + ([None] if _knapp else [])
        for _ in range(80):
            a = dict(fest)
            order = [i for i in range(len(zl)) if i not in fest]; rng.shuffle(order)
            for i in order:
                cand = []
                for r in opts:
                    a[i] = r; cand.append((cost(a), r))
                a[i] = min(cand)[1]
            for _ in range(60):
                improved = False
                for i in range(len(zl)):
                    if i in fest: continue
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
    ZONE_DROP = {
        (0x1110, 1),
        # ROOM1140 Zone 1 -> Seite 4 Rect 3: die Zone ist nachweislich KEIN Ort,
        # sondern der solide Moebelblock im Briefing-Raum (byte-true Kollisions-Sonde
        # FUN_8003b0a4, PR=450: 1,2 % der Rasterpunkte frei gegen 66 % bei einer echten
        # Zone; alle 19 SCA-Zellen Band 0, keine Tuer, keine Treppe, Bbox vollstaendig
        # in Zone 0). Sie belegte damit das Rechteck, das in Wahrheit ROOM1170s zweiten
        # Bereich auf dem 3F-Blatt zeigt: Seite 4 Rect 3 und Seite 5 Rect 0 sind
        # DIESELBE Zeichnung (22 von 1152 Pixeln Unterschied, alle Index 4->1 = zwei
        # Tuersymbole), und die beiden Ausgaenge des zweiten Bereichs fuehren nach
        # ROOM1130 und ROOM1140 - beide auf Seite 4 = 3F.
        (0x1140, 1),
    }
    for key in ZONE_DROP:
        assign.pop(key, None)

    # ---- BELEGTE ZUORDNUNGEN, die die Kostenheuristik nicht findet ------------
    # ⛔ ROOM10C0 -> Seite 3 Rect 4 ist ERZWUNGEN, nicht gewaehlt.
    # Nutzer 2026-09-01: "Bei 2F, wenn ich vom Treppenhaus in die Tuer gehe, springt er
    # komisch an eine andere Stelle in der Map" und "ab der 2. Etage ist die Darstellung
    # nach dem Treppenhaus immer noch ueberall komplett falsch."
    # Zwei gemessene Bedingungen legen es fest:
    #   1. Das Treppenhaus ROOM1060 liegt auf Band 4 auf Seite 3 Rect 9 (Etagen-Tabelle,
    #      aus den Tueren des Raums abgeleitet; Rect (3,9) und (2,9) sind dieselbe
    #      Zeichnung - gleiche Geometrie (109,134,16,16) und gleiche Kachel-uv).
    #   2. Zwei Raeume, die eine Tuer verbindet, muessen Rechtecke haben, deren
    #      GEZEICHNETE Flaechen sich beruehren - der Kuenstler hat sie aneinander
    #      gesetzt. Das ist an der Kachel messbar und unabhaengig von jeder Projektion.
    # ROOM10C0 ist der einzige Raum mit einer Tuer zum Treppenhaus; nur Rect 2 und Rect 4
    # beruehren Rect 9. Mit Rect 2 laesst sich der uebrige Tuergraph der Seite NICHT mehr
    # erfuellen (ROOM10D0 haengt an vier Nachbarn, Rect 2 hat nur zwei Beruehrungen).
    # Vollstaendige Aufzaehlung: 36 Zuordnungen erfuellen alle Tuerkanten, und in
    # 36 von 36 liegt ROOM10C0 auf Rect 4 (alle anderen Zonen der Seite sind mit 25-67 %
    # unbestimmt und werden deshalb NICHT festgesetzt - die Kostenheuristik verteilt sie
    # um diese eine Vorgabe herum).
    # Der Generator hatte ROOM10C0 auf Rect 6, und Rect 6 beruehrt Rect 9 nicht: die Tuer
    # aus dem Treppenhaus fuehrte damit in eine Zeichnung am anderen Ende des Blattes.
    # (ZONE_FIX steht weiter oben und geht als HARTE Vorgabe in die Suche ein; hier
    # bleibt nur die Sicherung, falls eine Vorgabe erst nach der Suche gilt.)
    for key, val in ZONE_FIX.items():
        room, zi = key
        if room in zinfo and zi < len(zinfo[room]) and assign.get(key) != val:
            for k2, v2 in list(assign.items()):
                if v2 == val and k2 != key: del assign[k2]
            assign[key] = val

    # ================= MASSSTAB AUS DER AUSGELIEFERTEN TABELLE ==================
    # ⛔ ZURUECKGENOMMEN am 2026-09-01: hier stand bis v0.3.70 eine SELBST GERECHNETE
    # Eichung (Massstab aus der gezeichneten Flaeche, Versatz per Symbol-Zuordnung,
    # danach zwei Partner-Durchgaenge und ein Federmodell). Sie war ein Rate-Defekt im
    # Messgewand: 37 von 37 geeichten Zonen schoben ihre begehbare Flaeche aus dem
    # eigenen Rechteck heraus (an den SCA-Zellen gemessen klemmten 46-100 % der
    # Rasterpunkte, 6 Zonen zu 100 %), und ihr Guetemass war wertlos, weil 29 der 37
    # Zonen nur EIN gemaltes Symbol haben - der Versatz wurde daraufhin gerechnet, der
    # Fehler war per Konstruktion 0,0 px. Das ist die Ursache von "ab Police Station 2F
    # ist die Karte vollkommen im Eimer" (Nutzer 2026-09-01).
    #
    # Das Original braucht nichts davon: es liefert die Zeilen MIT. FUN_800473f8
    # @0x8004741c-0x80047528 rechnet
    #     mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
    #     my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
    # mit einer Zeile {s16 ox, s16 oy, u16 sx, u16 sy} je Raum-Index @0x800768b0
    # (8 B/Zeile; Zeilenindex = Stage-Basis + Raum-Nummer, dieselben Basen wie der
    # Seiten-Setzer @0x8004b568: +0/+38/+50/+65/+77/+98). sx/sy laedt das Original mit
    # `lhu` - vorzeichenlos, es gibt also keine gespiegelten Kacheln.
    #
    # 29 Zeilen sind echt, der Rest ist der Stub {0,0,1,1}. Die echten Zeilen sagen
    # zusaetzlich, WELCHES Rechteck gemeint ist: projiziert man die SCA-Zellen des Raums
    # mit seiner Zeile, faellt das Ergebnis auf genau ein Rechteck seiner Seite. Genau so
    # kommen die vier Zuordnungen zustande, die vorher von Hand geraten waren
    # (1020 -> 2/1, 1070 -> 2/3, 1200 -> 1/2, 1210 -> 1/3), und die Tueren landen dabei
    # 2,0-4,5 px auf ihren gemalten Symbolen.
    #
    # WACHE (sonst kommt die Zeile nicht rein): >=75 % der begehbaren Punkte muessen im
    # Rechteck liegen, die Projektion muss >=50 % des Rechtecks fuellen, und wo Symbole
    # gemalt sind, duerfen die Tueren hoechstens 8 px danebenliegen. Faellt eine Zeile
    # durch, bleibt es bei der Bbox-Streckung auf dem zugeordneten Rechteck - die klemmt
    # nie, weil sie die Eingabe beschneidet.
    def _zeile(rid):
        idx = BASES[(rid >> 12) - 1] + ((rid >> 4) & 0xff)
        a = 0x800768b0 + idx * 8
        return s16(a), s16(a + 2), u16(a + 4), u16(a + 6)

    def _proj(wx, wz, ox, oy, sx, sy):
        return (((((wx + 32000) * 10 * sx) >> 20) + 5) // 10 + ox,
                -(((((wz + 32000) * 10 * sy) >> 20) + 5) // 10) + oy)

    def _sca_punkte(rid):
        pts = []
        for (x, zz, w, dep) in (c[:4] for c in sca_all.get(rid, ())):
            for i in range(4):
                for j in range(4):
                    pts.append((x + w * (2 * i + 1) // 8, zz + dep * (2 * j + 1) // 8))
        return pts

    eichung = {}          # (room, zi) -> (ox, oy, sx, sy)
    kandidaten = []
    for b in sorted(zinfo):
        ox, oy, sx, sy = _zeile(b)
        if sx <= 1 or sy <= 1: continue                 # Stub-Zeile
        pg = page_of(b)
        if pg is None or pg == 0xd: continue
        pts = _sca_punkte(b)
        if not pts: continue
        P = [_proj(a, c, ox, oy, sx, sy) for a, c in pts]
        px0 = min(p[0] for p in P); px1 = max(p[0] for p in P)
        py0 = min(p[1] for p in P); py1 = max(p[1] for p in P)
        for ri, R in enumerate(rects(pg)):
            if R[2] <= 0 or R[3] <= 0: continue
            q = sum(1 for p in P if R[0] <= p[0] <= R[0] + R[2] - 1 and
                                    R[1] <= p[1] <= R[1] + R[3] - 1) / float(len(P))
            fx = min(px1, R[0] + R[2] - 1) - max(px0, R[0]) + 1
            fy = min(py1, R[1] + R[3] - 1) - max(py0, R[1]) + 1
            deck = (fx * fy) / float(R[2] * R[3]) if (fx > 0 and fy > 0) else 0.0
            if q < 0.75 or deck < 0.50: continue
            gl = [(g[0], g[1]) for g in GLYPHEN.get((pg, ri), ())]
            dp = [_proj(d['lx'], d['lz'], ox, oy, sx, sy)
                  for d in doors_all.get(b, []) if not (d['rw'] == 0 and d['rd'] == 0)]
            sf = None
            if gl and dp:
                n = len(dp); m = len(gl); k = min(n, m)
                for wahl in itertools.permutations(range(m), k):
                    for sub in itertools.combinations(range(n), k):
                        f = sum(((dp[sub[t]][0] - gl[wahl[t]][0]) ** 2 +
                                 (dp[sub[t]][1] - gl[wahl[t]][1]) ** 2) ** 0.5
                                for t in range(k)) / k
                        if sf is None or f < sf: sf = f
                if sf is not None and sf > 8.0: continue
            kandidaten.append((q * min(1.0, deck * 1.4), b, pg, ri, ox, oy, sx, sy))
    # Ein Rechteck gehoert einem Raum. Zwei Raeume teilen sich gelegentlich EINE Zeile
    # (5040/5120 und 50A0/5140 tun das) - dann bekommt der bessere Treffer das Rechteck,
    # der andere faellt auf die Bbox-Streckung zurueck.
    belegt = set(); n_orig = 0
    # ⛔ EINE BELEGTE VORGABE IST HART - AUCH GEGEN DIE MASSSTABSZEILE.
    # Der Block unten schrieb bisher `assign` bedingungslos um ("Das abgeleitete
    # Rechteck gewinnt gegen die geometrische Zuordnung") und hat damit ZONE_FIX
    # ausgehebelt, obwohl dessen Kommentar es ausdruecklich als HARTE Vorgabe fuehrt.
    # FOLGE, gemessen 2026-09-04: ROOM1060 (Treppenhaus) und ROOM1080 (Fahrstuhl) landeten
    # BEIDE auf Blatt 2 Rect 2 - zusammen mit ROOM1040. Von dort erbte sich der Fehler auf
    # die Gastblaetter: auf 3F trug ROOM1060 dasselbe Rect 5 wie ROOM1120, weshalb die
    # Paarung ihrer gemeinsamen Tuer mit A == B rechnete und die Marke auf ROOM1120s
    # eigene Westwand legte (Nutzer 2026-09-04: "This door shouldn't be there").
    _fix_rects = set(ZONE_FIX.values())
    for (_w, b, pg, ri, ox, oy, sx, sy) in sorted(kandidaten, key=lambda t: -t[0]):
        if (pg, ri) in belegt: continue
        if any(k[0] == b for k in eichung): continue
        if (pg, ri) in _fix_rects and ZONE_FIX.get((b, 0)) != (pg, ri):
            continue                      # das Rechteck ist per Vorgabe vergeben
        if (b, 0) in ZONE_FIX and ZONE_FIX[(b, 0)] != (pg, ri):
            continue                      # dieser Raum ist per Vorgabe woanders
        # die Zeile gilt fuer den Raum; sie gehoert an die Zone, die dieses Rechteck haelt
        zi = None
        for i in range(len(zinfo[b])):
            if assign.get((b, i)) == (pg, ri): zi = i; break
        if zi is None:
            for i in range(len(zinfo[b])):
                if assign.get((b, i)) is not None: zi = i; break
        if zi is None: zi = 0
        belegt.add((pg, ri))
        eichung[(b, zi)] = (ox, oy, sx, sy)
        # Das abgeleitete Rechteck gewinnt gegen die geometrische Zuordnung.
        for k2, v2 in list(assign.items()):
            if v2 == (pg, ri) and k2 != (b, zi): del assign[k2]
        assign[(b, zi)] = (pg, ri)
        n_orig += 1
    print(f"{n_orig} Zonen mit der ausgelieferten Massstabszeile @0x800768b0 "
          f"(Rechteck aus der Zeile abgeleitet)")

    # ================= ORIENTIERUNG EINZELNER ZEICHNUNGEN =======================
    # ⛔ Nutzer 2026-09-01: "Im Room 1120 lande ich wieder ganz komisch ploetzlich auf der
    # falschen Seite, wenn ich vom ROOM 1130 aus rein komme."
    #
    # GEMESSEN: ROOM1120s Tuer nach ROOM1130 liegt in seiner eigenen Bbox bei Anteil
    # (0.11, 0.23) - unten links. Die Kachel malt dieselbe Tuer bei (150,126) an der
    # OSTwand, und ROOM1130 zeichnet denselben Durchgang bei (156,125). Zwei
    # voneinander unabhaengige Quellen sagen also oben rechts; unsere Streckung sagte
    # unten links. Um 180 Grad gedreht landet sie bei (155,128): 5 px vom gemalten
    # Symbol, 3 px von ROOM1130s Position.
    #
    # Die Gegenhypothese "falsches Rechteck" ist geprueft und verworfen: kein Rechteck
    # der Seite 4 kommt in normaler Lage naeher als 18 px an den Nachbarn (Rect 3),
    # und dessen Symbolabstand ist 9 px. Rect 5 gedreht: 3 px / 5 px.
    #
    # Warum das ueberhaupt vorkommen kann: jeder RDT-Raum hat sein EIGENES lokales
    # Koordinatensystem; nichts zwingt Welt-+X auf Karten-+x. Fuer die 33 Raeume mit
    # ausgelieferter Zeile @0x800768b0 legt die Zeile die Abbildung fest (sx/sy per
    # `lhu`, also positiv - jene Raeume sind ausgerichtet). Fuer die 39 Stub-Raeume
    # legt sie NICHTS fest; dort sind die gemalten Symbole und die Nachbarn die
    # einzigen Zeugen.
    #
    # WACHE: Es kommt nur ein Eintrag hinein, wenn (1) der Raum den Stub hat, (2)
    # MINDESTENS ZWEI unabhaengige Beobachtungen dieselbe Drehung waehlen, (3) beide
    # danach <= 8 px liegen und (4) die Gegenhypothese "anderes Rechteck, normale Lage"
    # gemessen schlechter ist. ROOM5070 (zwei Symbole, aber X und 180 Grad gleichauf,
    # kein Nachbar-Beleg) und ROOM5020 (nur eine Beobachtung) erfuellen das NICHT und
    # bleiben deshalb ungedreht.
    ZONE_ORIENT = {
        (0x1120, 0): (1, 1),   # Symbol 8->5 px, Nachbar ROOM1130 40->3 px
        (0x2050, 0): (1, 1),   # Symbol 33->4 px, Nachbar 56->6 px
    }

    # ================= GRUNDRISS AUS KOLLISION UND TUERGRAPH ===================
    # ⛔ Nutzer-Entscheidung 2026-09-01: "du darfst davon abweichen, wenn du die Original
    # Symbole verwendest, und damit eine nahezu perfekte Karte kreieren kannst."
    # Sein Modell: "ein neues Kartenstueck schliesst genau da an, wo man den Raum davor
    # durch die Tuer verlassen hat."
    #
    # Warum ueberhaupt: die gemalte Karte des Originals ist unvollstaendig und nicht
    # massstabsgetreu. Nur 33 der 72 Karten-Raeume tragen eine Massstabszeile
    # @0x800768b0, 33 der 100 Raeume haben gar kein Rechteck, und ROOM1100 misst echt
    # 41x47 px gegen ein gemaltes Rechteck von 24x24. Selbst MIT den ausgelieferten
    # Zeilen liegen die beiden Enden eines Durchgangs im Median 7,5 px auseinander.
    # Der Loeser (tools/grundriss.py) erreicht 0-2 % Ueberlappung bei beruehrenden
    # Durchgaengen - dort passt ein Kartenstueck also wirklich ans andere.
    import grundriss as _gr
    _MED = {}
    for _st in range(1, 7):
        _xs = []; _ys = []
        for _r in sorted(zinfo):
            if (_r >> 12) != _st: continue
            _o, _p, _a, _b2 = _zeile(_r)
            if _a > 1 and _b2 > 1: _xs.append(_a); _ys.append(_b2)
        if _xs:
            _MED[_st] = ((1 << 20) / statistics.median(_xs),
                         (1 << 20) / statistics.median(_ys))
    # ⛔ JE ORT, NICHT JE RAUM. Ein RDT-Raum ist nicht immer EIN Ort: 26 der 103
    # Basis-Raeume zerfallen in mehrere getrennte Zonen. Ein Loeser je RAUM zeichnete
    # dann alle Zonen als einen Klumpen und haengte denselben an jede Zone - gemessen an
    # ROOM1080, dessen Fahrstuhlkabine 14x13 px misst, dessen Grundriss aber 34x22 px
    # gross war, und an ROOM1140, dessen beide Zonen denselben Kasten trugen.
    # Ein ORT ist deshalb (Raum, Zone); seine Zellen sind die Kollisionszellen in der
    # Zonen-Box, seine Tueren die Tueren, die darin liegen, und das Ziel einer Tuer ist
    # der Ort, in dem ihr Spawn-Punkt liegt.
    # ================= ETAGEN-VORLAUF ==========================================
    # Welche (Raum, Zone) gehoert auf WELCHES Blatt, je BAND? Ein Raum kann ueber
    # mehrere Ebenen reichen; welche gilt, haengt am Band (+0x82, Aot_set pc[4]; das
    # Original gattet die Tuer-Interaktion darauf, FUN_8002bd44 @0x8002bf38).
    # Erkennbar an den Tueren: fuehren die Tueren des einen Bandes auf ein anderes
    # Blatt als die des anderen, dann reicht der Ort ueber beide.
    # ⛔ DAS MUSS VOR DEM LOESER STEHEN. Frueher lief diese Rechnung erst nach der
    # Rechteck-Vergabe und setzte ein zugeteiltes Kunst-Rechteck voraus - damit kam sie
    # auf 12 Zeilen und der Loeser wusste beim Setzen nichts davon. Jetzt bekommt der
    # Ort auf JEDEM Blatt, das eines seiner Baender erreicht, eine eigene Lage vom
    # Loeser; die Zweitzeichnung ist dann kein in ein fremdes Rechteck eingepasster
    # Abklatsch mehr, sondern ein richtig gesetzter Grundriss.
    ETAGEN = []          # (raum, zone, band, seite)
    for _b in sorted(zinfo):
        for _zi in range(len(zinfo[_b])):
            _bd = {}
            for _d in doors_all.get(_b, ()):
                if _d['rw'] == 0 and _d['rd'] == 0: continue
                if zone_at(_b, _d['lx'], _d['lz']) != _zi: continue
                _zp = page_of(_d['dest'])
                if _zp is None or _zp == 0xd: continue
                _bd.setdefault(_d['band'], set()).add(_zp)
            if len(_bd) < 2: continue
            for _band in sorted(_bd):
                _ziel = sorted(_bd[_band])
                if len(_ziel) != 1: continue
                ETAGEN.append((_b, _zi, _band, _ziel[0]))
    # ⛔ EINE EINZELNE ZEILE IST KEINE ETAGEN-UMSCHALTUNG. Fuehren beide Baender auf
    # DASSELBE Blatt, reicht der Ort nicht ueber Ebenen - er haette dann eine Gast-Lage
    # ohne Etagen-Bit und waere fuer immer unsichtbar.
    _et_grp = {}
    for (_b, _zi, _band, _zp) in ETAGEN:
        _et_grp.setdefault((_b, _zi), set()).add(_zp)
    ETAGEN = [e for e in ETAGEN if len(_et_grp[(e[0], e[1])]) >= 2]

    # ---- RAEUME OHNE EIGENE TUEREN: die Etage kommt vom BESUCHER ---------------
    # Nutzer 2026-09-01: "In ROOM 1120 ... da haette ich den Fahrstuhl erwartet."
    # Die Fahrstuhlkabine ROOM1080 fuehrt NULL eigene Tuer-Datensaetze - sie wird
    # betreten, nicht durchschritten; die Regel oben findet fuer sie nichts. Ihre Etage
    # steckt in dem Raum, AUS DEM man kommt: zu ihr fuehren drei Tueren, aus ROOM1040
    # (1F), ROOM10C0 (2F) und ROOM1120 (3F) - alle mit Band 0.
    _hat_tuer = {b for b in zinfo
                 if any(not (d['rw'] == 0 and d['rd'] == 0) for d in doors_all.get(b, ()))}
    _et_vorh = {(e[0], e[1], e[3]) for e in ETAGEN}
    _n_bes = 0
    for _b in sorted(zinfo):
        if _b in _hat_tuer: continue
        for _zi in range(len(zinfo[_b])):
            for _y in sorted(zinfo):
                for _d in doors_all.get(_y, ()):
                    if _d['rw'] == 0 and _d['rd'] == 0: continue
                    if _d['dest'] != _b: continue
                    _py = page_of(_y)
                    if _py is None or _py == 0xd: continue
                    if (_b, _zi, _py) in _et_vorh: continue
                    ETAGEN.append((_b, _zi, _d['band'], _py))
                    _et_vorh.add((_b, _zi, _py))
                    _n_bes += 1

    GAST = {}            # seite -> {(raum, zone)}
    for (_b, _zi, _band, _zp) in ETAGEN:
        if _zp != page_of(_b):
            GAST.setdefault(_zp, set()).add((_b, _zi))
    print(f"{len(ETAGEN)} Etagen-Zeilen aus dem Tuergraph ({_n_bes} ueber den BESUCHER), "
          f"{sum(len(v) for v in GAST.values())} Gast-Lagen auf fremden Blaettern")

    def _ort(rid, zi):
        return (rid << 4) | zi

    def _ort_von_punkt(rid, wx, wz):
        zi = zone_at(rid, wx, wz)
        if zi is None:
            return None
        return _ort(rid, zi)

    grundrisse = {}      # (seite, raum, zone) -> (abbildung, kasten, zellen)
    FELDX, FELDY, FELDW, FELDH = _gr.FELD

    # ---- (1) Eingabe je Blatt: welche Orte, mit welchen Zellen und Tueren ----------
    # ---- VARIANTEN-RAEUME: ZWEI SKRIPTZUSTAENDE, EIN ORT --------------------
    # Ein Raumpaar, das an EINEM Tuerschlitz (gleicher Trigger UND gleicher Spawn) zwei
    # verschiedene Ziele traegt, KANN derselbe Raum in zwei Skriptzustaenden sein. Belegt
    # ist das erst, wenn beide dieselbe Zellzahl haben und JEDE Tuer auf derselben
    # Weltkoordinate liegt - dann unterscheiden sie sich nur im Ziel. Gemessen an
    # ROOM5030/ROOM5110: vier Schlitze, alle geteilt, 75/75 Zellen, Ziele paarweise
    # verschieden (5040/5120, 50A0/5140, 5070/5130). Behalten wird der, dessen aeussere
    # Kollisionsbox die andere ENTHAELT; der andere erbt dessen Rechteck.
    # Ohne die Zusammenlegung liegen die Zustaende uebereinander: Blatt 9 hatte 37,6 %
    # Ueberlappung, mit ihr 3,0 % - bei unveraenderten Live-Werten.
    # Abschalten (Messung): RE15_ALIAS=0
    _ALIAS = {}
    if os.environ.get('RE15_ALIAS', '1') != '0':
        def _bb(r):
            _z = [c for c in sca_all.get(r, ()) if c[2] > 0 and c[3] > 0]
            if not _z:
                return None
            return (min(c[0] for c in _z), min(c[1] for c in _z),
                    max(c[0] + c[2] for c in _z), max(c[1] + c[3] for c in _z))
        _kand = set()
        for _r in sorted(doors_all):
            _pS = collections.defaultdict(list)
            for _d in doors_all[_r]:
                if _d['rw'] == 0 and _d['rd'] == 0:
                    continue
                _pS[(_d['lx'], _d['lz'], _d['nx'], _d['nz'])].append(_d['dest'])
            for _k, _ds in _pS.items():
                _u = sorted(set(_ds))
                if len(_u) == 2:
                    _kand.add(tuple(_u))
        def _zellzahl(r):
            return len([c for c in sca_all.get(r, ()) if c[2] > 0 and c[3] > 0])

        def _schlitze(r):
            return set((d['lx'], d['lz']) for d in doors_all.get(r, ())
                       if not (d['rw'] == 0 and d['rd'] == 0))

        for (_a, _b) in sorted(_kand):
            _ba, _bb2 = _bb(_a), _bb(_b)
            if not _ba or not _bb2:
                continue
            # VOLLER BELEG oder gar nicht: dieselbe Zellzahl UND jede Tuer auf derselben
            # Weltkoordinate. Ohne das ist "derselbe Raum" bloss vermutet - und die
            # schwachen Kandidaten wurden live gemessen: schlimmster Sprung 22 -> 164 px.
            _sa, _sb = _schlitze(_a), _schlitze(_b)
            if _zellzahl(_a) != _zellzahl(_b) or not _sa or _sa != _sb:
                continue
            if _ba[0] <= _bb2[0] and _ba[1] <= _bb2[1] and _bb2[2] <= _ba[2] and _bb2[3] <= _ba[3]:
                _behalt, _weg2 = _a, _b
            elif _bb2[0] <= _ba[0] and _bb2[1] <= _ba[1] and _ba[2] <= _bb2[2] and _ba[3] <= _bb2[3]:
                _behalt, _weg2 = _b, _a
            else:
                continue
            _ALIAS[_weg2] = _behalt
        # Transitiver Schritt ueber die GETEILTEN SCHLITZE eines Varianten-Paares.
        # Zwei Skriptzustaende desselben Raums tragen ihre Tueren auf denselben
        # Weltkoordinaten und unterscheiden sich nur im ZIEL - also ist jedes Zielpaar
        # an einem geteilten Schlitz wieder ein Varianten-Paar. Gemessen an
        # ROOM5030/ROOM5110 (alle vier Schlitze geteilt): das liefert
        # ROOM5040/ROOM5120, ROOM50A0/ROOM5140 und ROOM5070/ROOM5130.
        def _ziele(_r):
            return dict(((d['lx'], d['lz']), d['dest']) for d in doors_all.get(_r, ())
                        if not (d['rw'] == 0 and d['rd'] == 0))
        _neu = True
        while _neu:
            _neu = False
            for _weg, _beh in sorted(_ALIAS.items()):
                _za, _zb = _ziele(_weg), _ziele(_beh)
                for _sl in sorted(set(_za) & set(_zb)):
                    _ta, _tb = _za[_sl], _zb[_sl]
                    if _ta == _tb or _ta in _ALIAS or _tb in _ALIAS:
                        continue
                    _ba, _bb2 = _bb(_ta), _bb(_tb)
                    if not _ba or not _bb2:
                        continue
                    if _ba[0] <= _bb2[0] and _ba[1] <= _bb2[1] and \
                            _bb2[2] <= _ba[2] and _bb2[3] <= _ba[3]:
                        _ALIAS[_tb] = _ta
                    elif _bb2[0] <= _ba[0] and _bb2[1] <= _ba[1] and \
                            _ba[2] <= _bb2[2] and _ba[3] <= _bb2[3]:
                        _ALIAS[_ta] = _tb
                    else:
                        continue
                    _neu = True
        if _ALIAS:
            print("   %d Varianten-Raeume zusammengelegt: %s"
                  % (len(_ALIAS), ", ".join("ROOM%04X->ROOM%04X" % (k, v)
                                            for k, v in sorted(_ALIAS.items()))))

    _eingabe = {}
    _lebend = _lebende_paare()
    if _lebend is None:
        print("   (tools/engine_tueren.txt fehlt - Phantom-Tueren werden NICHT gefiltert)")
    for _pg in range(13):
        _orte = []
        _rd = {}
        _hier = [(b, zi) for b in sorted(zinfo) if page_of(b) == _pg
                 for zi in range(len(zinfo[b]))]
        _hier += sorted(GAST.get(_pg, ()))
        for _b, _zi in _hier:
            if _ALIAS and _b in _ALIAS: continue     # Alias-Raum wird nicht gesetzt
            if _zi >= len(zinfo[_b]): continue
            _x0, _x1, _z0, _z1 = zinfo[_b][_zi]
            _z = [c for c in sca_all.get(_b, ())
                  if _x0 <= c[0] + c[2] // 2 <= _x1 and _z0 <= c[1] + c[3] // 2 <= _z1]
            if not _z: continue
            _tu = []
            _gesehen = set()
            _quellen = list(doors_all.get(_b, ()))
            for _al, _beh2 in sorted(_ALIAS.items()):
                if _beh2 == _b:
                    _quellen += list(doors_all.get(_al, ()))
            for _d in _quellen:
                if zone_at(_b, _d['lx'], _d['lz']) != _zi: continue
                # ⛔ PHANTOM-VERBINDUNGEN BINDEN DEN LOESER NICHT.
                # Die RDT-Records enthalten Tueren, die das Spiel an dieser Stelle gar
                # nicht aufstellt (inerte Aot_set mit sce == 0, skriptgeschuetzte).
                # Gemessen 2026-09-02 gegen die Engine (tools/engine_tueren.txt): 10 von
                # 123 Raumpaaren werden in KEINER Richtung installiert, darunter
                # ROOM1140<->ROOM1170 - genau die Kante, die Blatt 4 nachweislich
                # unloesbar machte (RE15_VOLLSUCHE=4: hoechstens 5 von 6). Solche Paare
                # duerfen die Anordnung nicht einschraenken.
                if False and _lebend is not None and _d['dest'] != _b:
                    _p1 = (_b & 0xFFF0, _d['dest'] & 0xFFF0)
                    _p2 = (_p1[1], _p1[0])
                    if _p1 not in _lebend and _p2 not in _lebend:
                        continue
                _e = dict(_d)
                _zt = _ort_von_punkt(_d['dest'], _d['nx'], _d['nz'])
                if _zt is None:
                    _zt = _ort(_d['dest'], 0)
                if _ALIAS and (_zt >> 4) in _ALIAS:
                    _zt = (_ALIAS[_zt >> 4] << 4) | (_zt & 15)
                _e['dest'] = _zt
                if _ALIAS:
                    _sig = (_d['lx'], _d['lz'], _zt)
                    if _sig in _gesehen:
                        continue
                    _gesehen.add(_sig)
                _tu.append(_e)
            _o = _ort(_b, _zi)
            _orte.append(_o)
            _rd[_o] = (_z, _tu, stairs_all.get(_b, ()))
        if _orte:
            _eingabe[_pg] = (_orte, _rd)

    # ---- (2) BLATTGRAPH: zwei Blaetter haengen zusammen, wenn ein ORT auf beiden liegt
    # DAS IST DER SCHLUESSEL ZUM STAPELN. Bis hierher bekam jedes Blatt seinen EIGENEN
    # Massstab aus dem Einpassen - gemessen 458 Welteinheiten je Pixel auf dem Dach gegen
    # 1524 auf B2, also derselbe Bau in Faktor 3,3 verschiedenen Groessen, je nachdem,
    # welche Seite man aufschlaegt. Und ein Treppenhaus, das auf drei Blaettern liegt,
    # stand auf jedem woanders.
    # Ein Ort, der ueber Ebenen reicht (Treppenhaus, Fahrstuhl, ROOM1170s zweiter
    # Bereich), ist auf beiden Blaettern DERSELBE Ort - er muss dort gleich herum und an
    # derselben Stelle liegen. Damit haengen die Blaetter zusammen; gemessen bilden
    # 1-2-3-4-5 (Polizeiwache) und 8-9-10-11 (Labor) je eine Komponente.
    _teilt = collections.defaultdict(set)
    for _pg, (_orte, _rd) in _eingabe.items():
        for _o in _orte:
            _teilt[_o].add(_pg)
    _nb_blatt = collections.defaultdict(set)
    for _o, _ps in _teilt.items():
        _ps = sorted(_ps)
        for _i in range(len(_ps)):
            for _j in range(_i + 1, len(_ps)):
                _nb_blatt[_ps[_i]].add(_ps[_j])
                _nb_blatt[_ps[_j]].add(_ps[_i])
    _komp = []
    _gesehen = set()
    for _pg in sorted(_eingabe):
        if _pg in _gesehen: continue
        _kk = []
        _q = [_pg]
        _gesehen.add(_pg)
        while _q:
            _p = _q.pop(0)
            _kk.append(_p)
            for _n in sorted(_nb_blatt[_p]):
                if _n not in _gesehen and _n in _eingabe:
                    _gesehen.add(_n)
                    _q.append(_n)
        _komp.append(sorted(_kk))
    print("Blatt-Komponenten (Blaetter, die Orte teilen): " +
          ", ".join("{" + ",".join(str(x) for x in k) + "}" for k in _komp))

    # ---- (3) je Komponente loesen: Posen weitergeben, ausrichten, EINMAL einpassen --
    for _k in _komp:
        _stage = min((_eingabe[p][0][0] >> 4) >> 12 for p in _k)
        _ex0, _ey0 = _MED.get(_stage, (460.0, 460.0))
        # Reihenfolge: das groesste Blatt zuerst, dann immer das mit den meisten
        # gemeinsamen Orten - so gibt jedes Blatt seine Posen an das naechste weiter.
        _reihe = []
        _offen = sorted(_k, key=lambda p: -len(_eingabe[p][0]))
        _reihe.append(_offen.pop(0))
        while _offen:
            _hab = set(o for q in _reihe for o in _eingabe[q][0])
            _offen.sort(key=lambda p: (-len(set(_eingabe[p][0]) & _hab),
                                       -len(_eingabe[p][0])))
            _reihe.append(_offen.pop(0))
        _posen = {}
        _bl = {}
        _lg = {}
        for _pg in _reihe:
            _orte, _rd = _eingabe[_pg]
            _B = _gr.Blatt(_orte, _rd, _ex0, _ey0)
            if os.environ.get('RE15_PAAR_DETAIL') == str(_pg):
                _paar_detail(_B, _pg)
            if os.environ.get('RE15_PAAR_PROBE') == str(_pg):
                _paar_probe(_B, _pg)
            if os.environ.get('RE15_VOLLSUCHE') == str(_pg):
                _vollsuche(_B, _pg)
            if (os.environ.get('RE15_ORT_PROBE') or '').split(':')[0] == str(_pg):
                _ort_probe(_B, _pg, os.environ['RE15_ORT_PROBE'])
            if os.environ.get('RE15_KANTEN_DUMP') == str(_pg):
                print("   [Kanten Blatt %d] %d Orte, %d Tuerkanten, %d Notkanten"
                      % (_pg, len(_orte), len(_B.kanten), len(_B.notkanten)))
                for (_a, _pa, _bb, _pb) in _B.kanten:
                    print("      ROOM%04X z%d  <->  ROOM%04X z%d"
                          % (_a >> 4, _a & 15, _bb >> 4, _bb & 15))
                print("      Notkanten (einseitige Tueren):")
                for (_a, _pa, _bb, _pb) in _B.notkanten:
                    print("         ROOM%04X z%d  ->  ROOM%04X z%d"
                          % (_a >> 4, _a & 15, _bb >> 4, _bb & 15))
                for _o in _orte:
                    print("      Ort ROOM%04X z%d: %d Tuer-Datensaetze -> %s"
                          % (_o >> 4, _o & 15, len(_rd[_o][1]),
                             ", ".join("ROOM%04X" % (_e['dest'] >> 4)
                                       for _e in _rd[_o][1]) or "KEINE"))
                _fehlt = []
                for _o in _orte:
                    for _d in _rd[_o][1]:
                        _z = _d['dest']
                        if _z not in _orte:
                            _fehlt.append((_o, _z))
                print("      NICHT auf diesem Blatt aufloesbar: %d" % len(_fehlt))
                for _o, _z in _fehlt:
                    print("         ROOM%04X z%d -> ROOM%04X z%d (Ort fehlt auf dem Blatt)"
                          % (_o >> 4, _o & 15, _z >> 4, _z & 15))
            # ⛔ NUR DIE POSE WEITERGEBEN, NICHT DIE LAGE. Eine festgenagelte LAGE aus
            # einem anderen Blatt macht die Kanten dieses Blattes unerfuellbar: gemessen
            # blieb der Kanten-Rest auf den Seiten 3 und 4 bei 31 bzw. 20 px stehen,
            # waehrend die Blaetter ohne feste Lagen auf 0 kamen. Die gemeinsame
            # AUSRICHTUNG (ein geteilter Raum liegt ueberall gleich herum) reicht.
            _B.feste_posen = dict((o, (_posen[o][2], _posen[o][3]))
                                  for o in _orte if o in _posen)
            _lage = _B.loesen_roh()
            if not _lage: continue
            for _o, _st in _lage.items():
                _posen.setdefault(_o, _st)
            _bl[_pg] = _B
            _lg[_pg] = _lage
        if not _lg: continue
        # AUSRICHTEN: jedes Blatt so verschieben, dass die geteilten Orte aufeinander
        # fallen. Breitensuche ueber den Blattgraph.
        _start = _reihe[0] if _reihe[0] in _lg else sorted(_lg)[0]
        _fest = set([_start])
        _q = [_start]
        while _q:
            _p = _q.pop(0)
            for _n in sorted(_nb_blatt[_p]):
                if _n in _fest or _n not in _lg: continue
                _dx = []
                _dy = []
                for _o in set(_lg[_p]) & set(_lg[_n]):
                    _dx.append(_lg[_p][_o][0] - _lg[_n][_o][0])
                    _dy.append(_lg[_p][_o][1] - _lg[_n][_o][1])
                if _dx:
                    _mx = sum(_dx) / len(_dx)
                    _my = sum(_dy) / len(_dy)
                    for _o in list(_lg[_n]):
                        _ox, _oy, _kx, _sp = _lg[_n][_o]
                        _lg[_n][_o] = (_ox + _mx, _oy + _my, _kx, _sp)
                _fest.add(_n)
                _q.append(_n)
        # ---- EINPASSEN: JE BLATT, NICHT JE GEBAEUDE -----------------------------
        # ⛔ GEMESSEN UND VERWORFEN (2026-09-02): ein GEMEINSAMER Massstab fuer alle
        # Blaetter eines Gebaeudes laesst die Stockwerke stapeln - dieselbe Groesse, ein
        # Ursprung, ein geteilter Raum an derselben Stelle. Er kostet aber genau das,
        # was die Karte lesbar macht: die Vereinigung aller Blaetter der Polizeiwache
        # misst 376x324 px, das Kartenfeld 132x140, also Faktor 0,32 - ROOM1120 faellt
        # von 34x36 auf 14x15 px, das Dach auf einen 20-px-Kasten in einem leeren Blatt,
        # und jedes Blatt nutzt noch ein Viertel seiner Flaeche. Der Grund: unsere
        # Stockwerke sind NICHT deckungsgleich. Der Loeser kennt nur Nachbarschaft durch
        # Tueren; wohin ein Stockwerk als Ganzes zeigt, sagen die Daten nicht. Es zu
        # erzwingen hiesse raten.
        # Der Nutzer hat Stapeln auch nie verlangt - sein Mass war "ein neues
        # Kartenstueck schliesst da an, wo man den Raum davor verlassen hat".
        # GEBLIEBEN ist der gemeinsame RAHMEN beim Loesen (feste_lagen): die Blaetter
        # einer Komponente wachsen um dieselben Anker, ein geteilter Raum liegt ueberall
        # gleich HERUM, und die Nachbarschaften bleiben ueber die Etagen hinweg
        # stimmig. Nur das Einpassen geschieht je Blatt.
        for _pg in sorted(_lg):
            _B = _bl[_pg]
            _B.ex, _B.ey = _ex0, _ey0
            _roh = set()
            for _o, _st in _lg[_pg].items():
                _roh |= _B.pixel(_o, _st)
            if not _roh: continue
            _xs = [q[0] for q in _roh]; _ys = [q[1] for q in _roh]
            _br = max(_xs) - min(_xs) + 1; _ho = max(_ys) - min(_ys) + 1
            # Sicherheitsrand: das Entzerren schiebt bis zu sechs Pixel nach aussen.
            _f = min(1.0, FELDW / float(_br), FELDH / float(_ho)) * 0.94
            _minx, _miny = min(_xs), min(_ys)
            # Das Entzerren schiebt nach aussen; passt das Blatt danach nicht mehr ins
            # Feld, wird es kleiner gerechnet und noch einmal probiert (Seite 4 ragte
            # sonst 143 px hoch aus dem 140 px hohen Feld).
            for _versuch in range(4):
                _B.ex = _ex0 / _f
                _B.ey = _ey0 / _f
                _neu_lage = dict((_o, (FELDX + (_st[0] - _minx) * _f,
                                       FELDY + (_st[1] - _miny) * _f, _st[2], _st[3]))
                                 for _o, _st in _lg[_pg].items())
                # (Kein Entzerren mehr: Rechtecke werden Wand an Wand angelegt, es
                # gibt nichts auseinanderzuschieben. Wo ein Rundweg im Tuergraph nicht
                # schliesst, ueberlappen zwei Rechtecke - das ist gewollt, "der Raum
                # ueberlappt die Wand" (Nutzer 2026-09-02), und allemal besser als eine
                # Luecke.)
                _end = set()
                for _o, _st in _neu_lage.items():
                    _end |= _B.pixel(_o, _st)
                if not _end: break
                _ex_ = [q[0] for q in _end]; _ey_ = [q[1] for q in _end]
                if (max(_ex_) - min(_ex_) + 1) <= FELDW and                    (max(_ey_) - min(_ey_) + 1) <= FELDH:
                    break
                _f *= 0.92
            # Nach dem Entzerren zurueck ins Feld schieben: das Schieben laeuft nach
            # aussen, Seite 4 ragte danach 143 px hoch aus dem 140 px hohen Feld.
            # MITTIG ins Feld setzen. Ohne das klebt jedes Blatt an der linken oberen
            # Ecke des Kartenfeldes, weil dort das Einpassen seinen Ursprung hat - auf
            # dem Abzug aller 13 Blaetter deutlich zu sehen.
            _end = set()
            for _o, _st in _neu_lage.items():
                _end |= _B.pixel(_o, _st)
            if _end:
                _ex_ = [q[0] for q in _end]; _ey_ = [q[1] for q in _end]
                _sx = FELDX + (FELDW - (max(_ex_) - min(_ex_) + 1)) // 2 - min(_ex_)
                _sy = FELDY + (FELDH - (max(_ey_) - min(_ey_) + 1)) // 2 - min(_ey_)
                if _sx or _sy:
                    _neu_lage = dict((_o, (_st[0] + _sx, _st[1] + _sy, _st[2], _st[3]))
                                     for _o, _st in _neu_lage.items())
            for _o in sorted(_neu_lage):
                _neu = _neu_lage[_o]
                _px = _B.pixel(_o, _neu)
                if not _px: continue
                _xs2 = [q[0] for q in _px]
                _ys2 = [q[1] for q in _px]
                _kasten = (min(_xs2), min(_ys2),
                           max(_xs2) - min(_xs2) + 1, max(_ys2) - min(_ys2) + 1)
                _rects = []
                for (cx, cz, cw, cd) in _B.zellen(_o):
                    _pts = [_gr.dreh(cx, cz, _neu[2], _neu[3]),
                            _gr.dreh(cx + cw, cz, _neu[2], _neu[3]),
                            _gr.dreh(cx, cz + cd, _neu[2], _neu[3]),
                            _gr.dreh(cx + cw, cz + cd, _neu[2], _neu[3])]
                    _X = [q[0] for q in _pts]
                    _Z = [q[1] for q in _pts]
                    _x0 = int(round(_neu[0] + min(_X) / _B.ex))
                    _x1 = int(round(_neu[0] + max(_X) / _B.ex))
                    _y0 = int(round(_neu[1] - max(_Z) / _B.ey))
                    _y1 = int(round(_neu[1] - min(_Z) / _B.ey))
                    if _x1 - _x0 < 1 or _y1 - _y0 < 1: continue
                    _rects.append((_x0, _y0, _x1 - _x0, _y1 - _y0))
                if not _rects: continue
                grundrisse[(_pg, _o >> 4, _o & 15)] = (_B.abbildung(_neu), _kasten, _rects)
            if os.environ.get('GRUNDRISS_KANTEN'):
                for _ki, (_ka, _kpa, _kb, _kpb) in enumerate(_B.kanten):
                    if _ka not in _neu_lage or _kb not in _neu_lage: continue
                    _q1 = _B.punkt(_ka, _neu_lage[_ka], *_kpa)
                    _q2 = _B.punkt(_kb, _neu_lage[_kb], *_kpb)
                    _dd = max(abs(_q1[0]-_q2[0]), abs(_q1[1]-_q2[1]))
                    if _dd > 8:
                        print("      KANTE Seite %2d ROOM%04X z%d <-> ROOM%04X z%d : %.0f px"
                              % (_pg, _ka >> 4, _ka & 15, _kb >> 4, _kb & 15, _dd))
                _dab = set()
                for _o in _neu_lage:
                    _dab.add(_o)
                for (_ka, _kpa, _kb, _kpb) in _B.kanten:
                    _dab.discard(-1)
                _hat = set()
                for (_ka, _kpa, _kb, _kpb) in _B.kanten:
                    _hat.add((min(_ka,_kb), max(_ka,_kb)))
                for (_ka, _kpa, _kb, _kpb) in _B.notkanten:
                    _k2 = (min(_ka,_kb), max(_ka,_kb))
                    if _k2 not in _hat:
                        print("      NOTKANTE (nicht ausgeglichen) Seite %2d "
                              "ROOM%04X z%d <-> ROOM%04X z%d"
                              % (_pg, _ka >> 4, _ka & 15, _kb >> 4, _kb & 15))
            _rr = sorted(_B.rest(_neu_lage))
            if _rr:
                print("        Kanten-Rest: Median %.0f px, <=2 px: %d von %d, "
                      "schlimmster %.0f px"
                      % (_rr[len(_rr) // 2], sum(1 for x in _rr if x <= 2), len(_rr),
                         _rr[-1]))
            if os.environ.get('RE15_LAGE_DUMP'):
                for _o in sorted(_neu_lage):
                    _st = _neu_lage[_o]
                    print("   [Lage] Blatt %d ROOM%04X z%d  dx=%.1f dy=%.1f k=%d sp=%d"
                          % (_pg, _o >> 4, _o & 15, _st[0], _st[1], _st[2], _st[3]))
                for (_a, _pa, _bb, _pb) in _B.kanten:
                    _qa = _B.wandpunkt(_a, _neu_lage[_a], _B.punkt(_a, _neu_lage[_a], *_pa))
                    _qb = _B.wandpunkt(_bb, _neu_lage[_bb], _B.punkt(_bb, _neu_lage[_bb], *_pb))
                    _rest = max(abs(_qa[0] - _qb[0]), abs(_qa[1] - _qb[1]))
                    print("   [Kante] Blatt %d ROOM%04X z%d <-> ROOM%04X z%d  "
                          "k=%d/%d sp=%d/%d  Rest %.1f px"
                          % (_pg, _a >> 4, _a & 15, _bb >> 4, _bb & 15,
                             _neu_lage[_a][2], _neu_lage[_bb][2],
                             _neu_lage[_a][3], _neu_lage[_bb][3], _rest))
            _kn = _B.kosten(_neu_lage)
            print("Seite %2d: Grundriss aus %d/%d Orten, %.1f %% Ueberlappung, "
                  "%d/%d Durchgaenge deckungsgleich, %d GETRENNT, %d VERSENKT, "
                  "%dx%d px, %.0f Welteinheiten je Pixel"
                  % (_pg, len(_lg[_pg]), len(_eingabe[_pg][0]), _kn[0], _kn[1],
                     len(_B.kanten), _kn[5], _kn[6], _kn[2], _kn[3], _B.ex))

    if _ALIAS:
        # Der Alias-Raum bekommt Rechteck, Abbildung und damit auch die Hervorhebung
        # des behaltenen - er ist derselbe Ort.
        for (_pg2, _rm, _zi2), _v in list(grundrisse.items()):
            for _al, _beh in _ALIAS.items():
                if _rm == _beh:
                    grundrisse[(_pg2, _al, _zi2)] = _v
    print(f"{len(grundrisse)} Orte mit Grundriss aus der Kollisionsgeometrie")

    # ⛔ WER GEWINNT: DIE KUNST ODER DER GERECHNETE GRUNDRISS?
    # Der Vorrang steht an ZWEI Stellen (hier und beim Fuellen von `synth`); sie muessen
    # dieselbe Antwort geben, sonst faellt eine Zone durch beide Raster und verschwindet
    # ganz aus der Tabelle (gemessen: 234 -> 66 Zeilen, nur noch 31 von 96 Raeumen).
    # ⛔ SEIT 2026-09-04 GEWINNT DIE KUNST (Nutzer: "wollten wir jetzt ja doch das
    # Original Kartenmaterial nutzen und nur die Tueren durch RE2 Tueren ersetzen und
    # Treppensymbole hinzufuegen"). Wo eine Zuordnung existiert, traegt die Zone das
    # gemalte Rechteck des Originals; gezeichnet wird nur noch, was das Original gar
    # nicht malt. RE15_KUNST=0 stellt die alte Grundriss-Loesung wieder her.
    # ⛔ SEIT 2026-09-04 IST DIE ORIGINAL-KUNST DER AUSLIEFERUNGSSTAND.
    # Nutzer: "ich moechte das Original Kartenmaterial nutzen mit Tuer und
    # Treppenmarkern von Resident Evil 2 und du musst es nur gebacken bekommen den
    # Spieler Marker darauf korrekt zu positionieren". Das ist genau das RE2-Modell:
    # die Kunst ist das BILD, die Position kommt aus einer eigenen Abbildung je Zone.
    # (Der frueher hier stehende Vorbehalt - "der Port zeichnet die Kacheln gar nicht" -
    # war ein Messfehler, siehe BEFUND §36: die Messschiene kannte nur FILL-Ops und
    # nicht die SPRT-Kacheln der Originalkarte.)
    # RE15_KUNST=0 stellt die gerechnete Grundriss-Loesung wieder her,
    # RE15_KUNST=misch zeichnet zusaetzlich die Raeume, die das Original nicht malt.
    # ⛔ SEIT 2026-09-04 IST DIE ORIGINAL-KUNST DER AUSLIEFERUNGSSTAND.
    # Nutzer: "ich moechte das Original Kartenmaterial nutzen mit Tuer und Treppenmarkern
    # von Resident Evil 2 und du musst es nur gebacken bekommen den Spieler Marker darauf
    # korrekt zu positionieren". Das ist das RE2-Modell: die Kunst ist das BILD, die
    # Position kommt aus einer eigenen Abbildung je Zone (eichung).
    #
    # Der Weg dahin steht in BEFUND.md §32-§38. Die drei Bausteine:
    #   1. Die Zuordnung Zone -> gemaltes Rechteck kommt AUS DEN ZEILEN (@0x800768b0 bzw.
    #      aus der Tuerkette hergeleitet), nicht aus einer Kostenfunktion - sonst
    #      widersprechen sich Zuordnung und Projektion. Die Heuristik bleibt Rueckfall.
    #   2. Jede zugeordnete Zone bekommt ihre Zeile als eigene Abbildung.
    #   3. to_map() folgt dem gemalten Rechteck statt dem nicht gezeichneten Grundriss.
    #
    # Gemessen: 89 von 96 Raeumen auf der Karte, 80 sichtbar rot, 164 von 207 Tueren mit
    # Symbol, Marker 80/80 im eigenen Rechteck und 0 im fremden.
    # RE15_KUNST=0 stellt die gerechnete Grundriss-Loesung wieder her.
    KUNST_VOR = os.environ.get('RE15_KUNST') != '0'

    # ================================================================================
    # ⛔ MIT DER KUNST KOMMT DIE ZUORDNUNG AUS DEN ZEILEN, NICHT AUS EINER KOSTENFUNKTION
    #
    # Bisher stammten Zuordnung (Zone -> gemaltes Rechteck) und Projektion (Weltpunkt ->
    # Kartenpunkt) aus ZWEI unabhaengigen Quellen: einer Kostenheuristik und der Zeile
    # @0x800768b0. Sie widersprechen sich, und auf der gemalten Karte faellt der
    # zeilen-projizierte Marker aus seinem zugeordneten Rechteck und wird an dessen Kante
    # geklemmt - gemessen wurde die Marker-Genauigkeit dadurch SCHLECHTER statt besser
    # (BEFUND §34: Median 35 px gegen 11 px).
    #
    # Hier wird die Zone durch IHRE EIGENE ZEILE projiziert und das Rechteck genommen,
    # das der projizierte Kasten am besten trifft. Damit stimmen Zuordnung und Projektion
    # per KONSTRUKTION ueberein. Der Weg ist belegt (BEFUND §22): die gemalten Rechtecke
    # SIND die Kollisionsboxen - von 38 Raeumen mit ausgelieferter Zeile treffen 30 ihr
    # Rechteck zu mindestens 50 %, vier exakt. Gemessen ueber alle Zonen mit Zeile:
    # 78 Zonen zugeordnet, Deckung im Median 98 %.
    #
    # Die Zeilen kommen aus tools/gen_marker_zeilen.py (ausgeliefert wo vorhanden, sonst
    # aus der Tuerkette hergeleitet, Auslassprobe Median 7 px). ⛔ Dessen Blatt-Tabelle
    # wird hier UEBERSCHRIEBEN: sie stammt sonst aus dem zuletzt erzeugten Header, und
    # der Generator soll nicht von seinem eigenen vorherigen Ergebnis abhaengen.
    # ================================================================================
    _zdeck = {}
    if KUNST_VOR:
        import gen_marker_zeilen as _GM
        _GM.BLATT = dict((b, page_of(b)) for b in zinfo
                         if page_of(b) is not None and page_of(b) != 0xd)
        _GM.ZONEN_N = collections.Counter(dict((b, len(zinfo[b])) for b in zinfo))
        _zeilen, _zq = _GM.alle_zeilen()
        _neu, _guete = {}, []
        for b in sorted(zinfo):
            pg = page_of(b)
            if pg is None or pg == 0xd or b not in _zeilen:
                continue
            _ox, _oy, _sx, _sz = _zeilen[b]
            R = rects(pg)
            if not R:
                continue
            for i, (x0, x1, z0, z1) in enumerate(zinfo[b]):
                px0 = _GM.karte_x(x0, _ox, _sx); px1 = _GM.karte_x(x1, _ox, _sx)
                py0 = _GM.karte_y(z1, _oy, _sz); py1 = _GM.karte_y(z0, _oy, _sz)
                px0, px1 = min(px0, px1), max(px0, px1)
                py0, py1 = min(py0, py1), max(py0, py1)
                fl = max(1, (px1 - px0) * (py1 - py0))
                best = None
                for ri, (rx, ry, rw, rh) in enumerate(R):
                    ox2 = min(px1, rx + rw) - max(px0, rx)
                    oy2 = min(py1, ry + rh) - max(py0, ry)
                    if ox2 <= 0 or oy2 <= 0:
                        continue
                    # relativ zur KLEINEREN Flaeche - ein Korridor in einem grossen
                    # Rechteck zaehlt, ohne dass das grosse Rechteck alles einsammelt
                    an = (ox2 * oy2) / float(min(fl, rw * rh))
                    if best is None or an > best[0]:
                        best = (an, ri)
                if best and best[0] >= 0.35:
                    _neu[(b, i)] = (pg, best[1])
                    _guete.append(best[0])
                    _zdeck[(b, i)] = best[0]
        _guete.sort()
        # ⛔ DIE KOSTENHEURISTIK BLEIBT ALS RUECKFALL STEHEN.
        # Die Zuordnung aus den Zeilen ist die genauere (Deckung im Median 98 %), aber
        # sie deckt nur, wo der projizierte Kasten ein Rechteck ueber 35 % trifft. Wer
        # sie ERSETZT statt zu ergaenzen, verliert die uebrigen Zonen ganz - und damit
        # den Raum von der Karte: gemessen 2026-09-04 blieben 75 von 96 Raeumen uebrig,
        # 21 waren gar nicht mehr da. Ein Raum ohne jede Zuordnung ist schlechter als
        # einer mit einer heuristischen.
        _uebernommen = 0
        for _k, _v in assign.items():
            if _k not in _neu:
                _neu[_k] = _v
                _uebernommen += 1
        print("Zuordnung AUS DEN ZEILEN: %d Zonen (Deckung Median %.0f %%), "
              "%d aus der Kostenheuristik uebernommen"
              % (len(_zdeck), 100 * _guete[len(_guete) // 2] if _guete else 0,
                 _uebernommen))
        assign = _neu

        # ⛔ DIE ZEILE WIRD ZUR ABBILDUNG DER ZONE - ABER NUR, WO SIE SICH BEWEIST.
        # Eine Zone ohne eigene Abbildung streckt ihre Weltbox linear in ihr Rechteck.
        # Das ist auf einem GEMALTEN Rechteck ungenau (es ist nicht massstabsgetreu zur
        # Kollision, BEFUND §22), aber robust. Die Zeile ist das Gegenteil: wo sie stimmt,
        # sitzt der Marker exakt; wo sie schlecht hergeleitet ist, sitzt er weit daneben.
        # Gemessen (BEFUND §35): pauschal ueber alle Zonen angewandt steigen die
        # Uebergaenge innerhalb 2 px von 2 % auf 12 %, der Median wird aber von 22 auf
        # 28 px SCHLECHTER - die Zeile hilft den guten Faellen und schadet den schlechten.
        # Der Riegel ist deshalb ihre eigene Deckung: nur wo der zeilen-projizierte
        # Kasten sein Rechteck zu mindestens RE15_ZEILEN_DECKUNG trifft, wird sie zur
        # Abbildung. Sonst bleibt die Streckung.
        # ⛔ SCHWELLE 0 - GEMESSEN. Ein erster Wurf liess nur Zeilen mit >= 80 %
        # Deckung zur Abbildung werden, aus Sorge, eine schlecht hergeleitete Zeile
        # koenne schaden. Das Gegenteil stimmt: die Zeile ist per Konstruktion
        # tuer-konsistent (sie ENTSTEHT aus den Tuerpunkten), die Streckung ist es nie.
        #     Schwelle | Zonen | Median | <=2 px | <=8 px | Spruenge > 16 px
        #        80 %  |  47   | 22 px  |  15 %  |  36 %  |  72
        #         0 %  |  57   | 14 px  |  20 %  |  42 %  |  63
        # Der Riegel bleibt als Hebel (RE15_ZEILEN_DECKUNG), damit die Messung
        # nachvollziehbar ist.
        _schwelle = float(os.environ.get('RE15_ZEILEN_DECKUNG', '0.0'))
        _n_eich = 0
        for (b, i), _d in sorted(_zdeck.items()):
            if _d < _schwelle:
                continue
            if (b, i) in eichung:
                continue                      # ausgelieferte Eichung schlaegt alles
            eichung[(b, i)] = _zeilen[b]
            _n_eich += 1
        print("   davon mit eigener Abbildung (Deckung >= %.0f %%): %d"
              % (100 * _schwelle, _n_eich))

    rows = []
    zid = 0   # globale Zonen-Nummer; beide Szenario-Varianten teilen sie
    zid_von = {}
    for b, zs in sorted(zinfo.items()):
        for i, bb in enumerate(zs):
            pr = assign.get((b, i))
            if pr is None: continue
            # ⛔ Ein Ort, eine Zeichnung. Hat der Loeser fuer diese Zone einen Grundriss
            # geliefert, gilt der - sonst stuenden beide Zeichnungen uebereinander.
            if not KUNST_VOR and (page_of(b), b, i) in grundrisse: continue
            for var in (0, 1):
                rows.append((b + var, bb, pr[0], pr[1], i, zid))
            zid_von[(b, i)] = zid
            zid += 1

    # ================================================================================
    # ⛔ EINE KOPIE AUF DEM ANDEREN BLATT - KEIN UMZUG.
    #
    # Nutzer 2026-09-04 (finding/error1, error2), woertlich: "wir wollen im Prinzip den
    # linken Teil der Roof Karte einfach quasi kopiert/eingesetzt haben in 3F dort wo in
    # error2 die Luecke ist." Und auf Nachfrage: "ich habe auch nichts von umzug gesagt,
    # sondern von Kopie".
    #
    # ⛔ EIN UMZUG IST NACHWEISLICH FALSCH und wurde am 2026-08-31 schon einmal
    # zurueckgenommen (siehe der Kommentar bei zpos weiter unten): ROOM1170s zweiter
    # Bereich ist EIN Kollisions-Zusammenhang - kleiner Raum OBEN, Treppe, Absatz UNTEN.
    # Wer ihn ganz nach unten legt, schaltet die Karte schon beim Betreten des oberen
    # Raums um. Der Bereich gehoert zu BEIDEN Ebenen; die Umschaltung haengt am BAND.
    #
    # Ein Ort, dessen Band auf ein anderes Blatt fuehrt, braucht dort also eine ZWEITE
    # Zeile (etage = 1). In der Grundriss-Loesung lieferte sie der Loeser; mit der Kunst
    # entfiel sie ersatzlos, und die Etagen-Tabelle zeigte auf ein rect 255, das es nicht
    # mehr gibt - daher das Loch auf 3F und der fehlende Marker.
    #
    # ⛔ DIE EIGENE ZEILE GILT AUF EINEM FREMDEN BLATT NICHT: sie setzt den Raum auf SEIN
    # Blatt. Anker sind stattdessen die NACHBARN dort (der Ort haengt an einer Tuer, sein
    # Rechteck muss deren beruehren); die FORM entscheidet unter den Kandidaten, denn der
    # Ort ist auf beiden Blaettern derselbe.
    # ================================================================================
    if KUNST_VOR:
        _n_gast = 0
        for (_b, _zi, _band, _zp) in ETAGEN:
            if (_b, _zi) not in zid_von:
                continue
            _heim = assign.get((_b, _zi))
            if _heim is None or _heim[0] == _zp:
                continue                      # schon auf diesem Blatt zu Hause
            if any(r[0] == _b and r[2] == _zp and r[4] == _zi for r in rows):
                continue                      # Zweitzeile steht schon
            _R = rects(_zp)
            if not _R:
                continue
            _belegt2 = set(r[3] for r in rows if r[2] == _zp and not (len(r) > 7 and r[7]))
            _heimR = rects(_heim[0])[_heim[1]]
            _sw, _sh = float(_heimR[2]), float(_heimR[3])
            _nachbar = []
            for _d in doors_all.get(_b, []):
                if _d['rw'] == 0 and _d['rd'] == 0:
                    continue
                for _r2 in rows:
                    if (_r2[0] & 0xFFF0) == _d['dest'] and _r2[2] == _zp:
                        _nachbar.append(_R[_r2[3]])
            # ⛔ EINE EXAKT PASSENDE FLAECHE DARF EINEN SCHWAECHEREN BEWOHNER VERDRAENGEN.
            # Nutzer-Anker 2026-09-04: "Du hast unten in 3F 2 Tueren bei der Map von
            # RE 1.5 die nach draussen fuehren. Das sind GENAU DIE 2 Tueren die du das
            # oben gesehen hast. an die Muss das Rechteck quasi anschliessen."
            # Nachgemessen mit der Symbol-Erkennung (tools/tuersymbole.py): auf Blatt 4
            # traegt Rect 3 genau diese zwei gemalten Tueren - und es ist 48x24, also
            # EXAKT die Groesse von ROOM1170s Dachflaeche (Blatt 5 Rect 0). Der Kuenstler
            # hat den Bereich dort also selbst gezeichnet; er war nur ROOM1120
            # zugeordnet, dessen Kasten 42x47 misst und nicht exakt passt.
            # Regel: passt die Groesse EXAKT und die des Bewohners nicht, wird getauscht -
            # der Bewohner zieht auf sein bestes freies Rechteck.
            _tausch = None
            for _ri, (_rx, _ry, _rw, _rh) in enumerate(_R):
                if _rw != int(_sw) or _rh != int(_sh):
                    continue
                if _ri not in _belegt2:
                    continue
                _bew = None
                for _r2 in rows:
                    if _r2[2] == _zp and _r2[3] == _ri and not (len(_r2) > 7 and _r2[7]):
                        _bew = (_r2[0] & 0xFFF0, _r2[4])
                        break
                if _bew is None:
                    continue
                _bR = rects(_zp)
                _bx0, _bx1, _bz0, _bz1 = zinfo[_bew[0]][_bew[1]]
                _bo = _zeilen.get(_bew[0])
                if not _bo:
                    continue
                _bw = abs(_GM.karte_x(_bx1, 0, _bo[2]) - _GM.karte_x(_bx0, 0, _bo[2]))
                _bh = abs(_GM.karte_y(_bz1, 0, _bo[3]) - _GM.karte_y(_bz0, 0, _bo[3]))
                if _bw == _rw and _bh == _rh:
                    continue          # der Bewohner passt selbst exakt - kein Tausch
                _frei = None
                for _fri, (_fx2, _fy2, _fw, _fh) in enumerate(_bR):
                    if _fri in _belegt2 or _fri == _ri:
                        continue
                    _g = (min(_bw, _fw) / float(max(_bw, _fw))) *                          (min(_bh, _fh) / float(max(_bh, _fh)))
                    if _frei is None or _g > _frei[0]:
                        _frei = (_g, _fri)
                if _frei and _frei[0] >= 0.5:
                    _tausch = (_ri, _bew, _frei[1])
                    break
            if _tausch:
                _ri2, _bew, _neu_ri = _tausch
                for _k in range(len(rows)):
                    if (rows[_k][2] == _zp and rows[_k][3] == _ri2 and
                            (rows[_k][0] & 0xFFF0) == _bew[0] and rows[_k][4] == _bew[1]):
                        rows[_k] = rows[_k][:3] + (_neu_ri,) + rows[_k][4:]
                assign[_bew] = (_zp, _neu_ri)
                # ⛔ DIE EICHUNG GEHOERT ZU EINEM BESTIMMTEN RECHTECK - NACH DEM UMZUG
                # IST SIE UNGUELTIG. Sie wurde oben (Zeile ~1180) GEMEINSAM mit der
                # Rechteck-Zuordnung aus der ausgelieferten Massstabszeile gewonnen; der
                # Tausch schrieb bisher nur `assign` und `rows` um und liess sie stehen.
                # NUTZER-BEFUND 2026-09-04 (fehler/error1.png, drei Anmerkungen zu
                # ROOM1120): dessen Eichung (101,190,2296,2312) war fuer Blatt 4 Rect 3
                # (152,89)+48x24 hergeleitet; der Tausch gab Rect 3 an ROOM1170 z1 und
                # schob ROOM1120 auf Rect 5 (120,119)+40x40. Gemessen mit der alten
                # Eichung auf dem neuen Rechteck:
                #   Bbox projiziert nach x148..190 y90..137 - das Rechteck liegt bei
                #   x120..159 y119..158. Nur 315 von 1755 Kollisionspunkten (18 %) landen
                #   ohne Klemmung darin; der Spielermarker bewegt sich in einer 11x18-Ecke
                #   eines 40x40-Rechtecks, und BEIDE vertikalen Tueren (-> ROOM1060
                #   Treppenhaus, -> ROOM1080 Aufzug) projizieren diagonal daneben und
                #   werden auf denselben Eckpixel (159,119) geklemmt. Das ist die
                #   gemeinsame Ursache aller drei Anmerkungen des Nutzers.
                # Ohne Eichung faellt to_map auf die Bbox-Streckung zurueck, die das
                # zugewiesene Rechteck per Konstruktion ausfuellt.
                if eichung.pop(_bew, None) is not None:
                    print("      Eichung von ROOM%04X z%d verworfen - sie galt fuer "
                          "Rect %d, nicht fuer Rect %d" % (_bew[0], _bew[1], _ri2, _neu_ri))
                for var in (0, 1):
                    rows.append((_b + var, zinfo[_b][_zi], _zp, _ri2, _zi,
                                 zid_von[(_b, _zi)], 0, 1))
                print("   Tausch auf Blatt %d: Rect %d geht an ROOM%04X z%d "
                      "(exakte Groesse), ROOM%04X z%d zieht auf Rect %d"
                      % (_zp, _ri2, _b, _zi, _bew[0], _bew[1], _neu_ri))
                _n_gast += 1
                continue
            # ⛔ BELEGTE GASTLAGEN. Wie ZONE_FIX fuer die Heimat, nur fuer die
            # Zweitzeichnung auf einem fremden Blatt. Belegt ueber DREI unabhaengige
            # Messungen (Untersuchung 2026-09-04, adversarisch gegengeprueft):
            #   1. MASSSTAB am GEMALTEN Kasten (nicht an der registrierten Kachel):
            #      ROOM1080 6200x6100 auf Rect 0s 10x10-Kasten = 620/610 wu/px,
            #      Anisotropie 1,02. ROOM1060 11100x12750 auf Rect 1s 18x22-Kasten =
            #      617/580, Anisotropie 1,06. ROOM1160, das Rect 1 bisher hielt,
            #      braeuchte 931/595 = 1,56 und findet auf dem ganzen Blatt nichts
            #      unter 1,28.
            #   2. KACHEL-WIEDERHOLUNG: nur fuenf Kachelgruppen im ganzen Spiel sind
            #      ueber Blattgrenzen pixelgleich - und es ist genau die Menge der
            #      stockwerkuebergreifenden Raeume. uv(168,40) liegt auf Blatt 2/3/4,
            #      uv(168,16) auf Blatt 2/4.
            #   3. TUERLAGE: ROOM1120s Tuer zur Kabine projiziert nach (135,146); die
            #      Mitte von Rect 0 liegt bei (135,145). Ein Pixel.
            # Und es sind genau die zwei Stellen, auf die der Nutzer gezeigt hat:
            # "here i would have expect the Elevator door" (133,145) = Rect 0,
            # "Here I would have expect the staircase door" (139,153) = Rect 1s
            # gemaltes West-Tuersymbol (136..141, 152..156).
            # Blatt 3 (2F) kam 2026-09-04 dazu: mit der Ersatztabelle (RECT_FIX,
            # BEFUND.md §44) fuehrt die Seite dieselben zwei Schacht-Kacheln wie
            # Blatt 4 - uv(168,16) 18x22 und uv(168,40) 10x10, gleiche Slots, gleiche
            # Groessen. Auf Blatt 4 ist die Zuordnung gemessen (die Tuer aus ROOM1120
            # zur Kabine liegt 1 px von der Mitte des uv(168,40)-Kastens, die zum
            # Treppenhaus 7 px daneben); dazu die Groesse: ROOM1060 misst 24x27 px im
            # ausgelieferten Massstab, ROOM1080 nur 14x13.
            GAST_FIX = {(0x1080, 0, 4): 0, (0x1060, 0, 4): 1,
                        (0x1080, 0, 3): 4, (0x1060, 0, 3): 1}
            _gf = GAST_FIX.get((_b, _zi, _zp))
            if _gf is not None and _gf < len(_R):
                for _k in range(len(rows) - 1, -1, -1):
                    if (rows[_k][2] == _zp and rows[_k][3] == _gf
                            and (rows[_k][0] & 0xFFF0) != _b):
                        del rows[_k]      # der bisherige Bewohner raeumt
                for var in (0, 1):
                    rows.append((_b + var, zinfo[_b][_zi], _zp, _gf, _zi,
                                 zid_von[(_b, _zi)], 0, 1))
                print("   Gastlage belegt: Blatt %d Rect %d -> ROOM%04X z%d"
                      % (_zp, _gf, _b, _zi))
                _n_gast += 1
                continue
            _best = None
            for _ri, (_rx, _ry, _rw, _rh) in enumerate(_R):
                if _ri in _belegt2:
                    continue
                _ber = 0
                for (_nx, _ny, _nw, _nh) in _nachbar:
                    if (min(_rx + _rw, _nx + _nw) - max(_rx, _nx) >= -2 and
                            min(_ry + _rh, _ny + _nh) - max(_ry, _ny) >= -2):
                        _ber = 1
                        break
                _fx = min(_sw, _rw) / max(_sw, _rw)
                _fy = min(_sh, _rh) / max(_sh, _rh)
                _wert = (_fx * _fy) ** 0.5 + (0.5 if _ber else 0.0)
                if _best is None or _wert > _best[0]:
                    _best = (_wert, _ri)
            if not _best or _best[0] < 0.5:
                continue
            for var in (0, 1):
                rows.append((_b + var, zinfo[_b][_zi], _zp, _best[1], _zi,
                             zid_von[(_b, _zi)], 0, 1))
            _n_gast += 1
        print("Zweitzeilen auf fremden Blaettern (Kunst-Kopie): %d" % _n_gast)

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
    zpos = {}                       # (room, zi) -> (bbox, page, rect)
    for b, zs in sorted(zinfo.items()):
        for i, bb in enumerate(zs):
            pr = assign.get((b, i))
            if pr is not None: zpos[(b, i)] = (bb, pr[0], pr[1])
    # ⛔ UND DASSELBE JE BLATT - SONST BLEIBT DIE TUER AUF DER FALSCHEN ETAGE.
    # Ein Ort kann auf zwei Blaettern gezeichnet sein (Heimat + Zweitzeile). Die MARKE
    # waehlt ihr Blatt ueber das BAND ihrer Tuer (blatt_fuer_band), to_map() hat den
    # Wunsch auf der Kunst aber ignoriert und immer das Heimatblatt geliefert.
    # Folge im Nutzer-Report 2026-09-04: ROOM1170s zwei Tueren nach ROOM1130/ROOM1140
    # tragen BAND 0 (also 3F), lagen aber auf dem DACH-Blatt - "auf roof ebene die beiden
    # Tueren nur wieder raus, die da nicht hin gehoeren" - und die 3F-Kopie blieb leer.
    zpos_blatt = {}                 # (room, zi, page) -> (bbox, page, rect)
    for _r in rows:
        _rm = _r[0] & 0xFFF0
        zpos_blatt[(_rm, _r[4], _r[2])] = (_r[1], _r[2], _r[3])
    def _gr(room, zi, pg=None):
        """Der Grundriss dieses Ortes auf DIESEM Blatt - ohne Angabe auf seinem
        eigenen. Ein Ort kann auf mehreren Blaettern liegen (Etagen)."""
        return grundrisse.get(((page_of(room) if pg is None else pg), room, zi))

    def _geo(pg, r, room=None, zi=None):
        """Die Flaeche, in der eine Marke liegt: das gemalte Rechteck - oder, seit die
        Raeume als Grundriss gezeichnet werden, dessen Kasten (Kennung rect = 255)."""
        if r == 255:
            g = _gr(room, zi, pg)
            if g: return g[1]
            t = synth.get((room, zi))
            if t: return (t[0], t[1], t[2], t[3])
            return None
        R = rects(pg)
        return R[r] if r < len(R) else None

    _klemm = {'xy': False}   # von to_map gefuellt: lag die rohe Projektion diagonal
                             # ausserhalb des Rechtecks? (siehe dort)

    def to_map(room, zi, wx, wz, pg_wunsch=None):
        _klemm['xy'] = False
        # ⛔ GRUNDRISS ZUERST. Seit der Loeser jeden Raum aus seiner Kollision setzt,
        # ist SEINE Abbildung die gueltige - die Marken muessen derselben folgen, sonst
        # rechnen sie gegen ein Rechteck, das gar nicht mehr gezeichnet wird, und die
        # Tuersymbole liegen irgendwo (im Abzug des 3F-Blattes fehlten sie ganz).
        # Formel wie in der Engine (re15_map_zone_abbildung):
        #     mx = (A*wx + B*wz) >> 16 + C ,  my = (D*wx + E*wz) >> 16 + F
        # ⛔ ...ABER NUR, WENN DER GRUNDRISS AUCH GEZEICHNET WIRD.
        # Der Loeser berechnet seine Grundrisse fuer ALLE 117 Orte, auch wenn die
        # Original-Kunst gezeichnet wird - dann steht hier eine Abbildung auf einen
        # Kasten, den niemand malt. Genau der Fehler, den der Kommentar oben beschreibt,
        # nur in die andere Richtung: gemessen 2026-09-04 trugen im Kunst-Stand ALLE
        # 152 Tuermarken rect = 255 und lagen damit auf verschwundenen Kaesten - von 184
        # Tueren fanden 172 gar keine Marke (Auslieferungsstand: 229 von 229).
        # Hat die Zone ein gemaltes Rechteck, gilt DIESES.
        g = None if (KUNST_VOR and assign.get((room, zi)) is not None)             else _gr(room, zi, pg_wunsch)
        if g:
            (A, B2, C, D, E, F), kasten, _z = g
            mx = (A * wx + B2 * wz) // 65536 + C
            my = (D * wx + E * wz) // 65536 + F
            kx, ky, kw, kh = kasten
            mx = min(max(mx, kx), kx + kw - 1)
            my = min(max(my, ky), ky + kh - 1)
            return (page_of(room) if pg_wunsch is None else pg_wunsch), 255, mx, my
        e = None
        if pg_wunsch is not None:
            e = zpos_blatt.get((room, zi, pg_wunsch))
        if e is None:
            e = zpos.get((room, zi))
        if not e: return None
        (x0, x1, z0, z1), pg, r = e
        ei = eichung.get((room, zi))
        if ei:
            ox, oy, sx, sy = ei
            mx, my = _proj(wx, wz, ox, oy, sx, sy)
            R = rects(pg)[r]
            # ⛔ MERKEN, OB DIE KLEMMUNG BEIDE ACHSEN GETROFFEN HAT. Trifft sie nur eine,
            # bleibt die andere Koordinate eine echte Messung - die Tuer sitzt dann auf
            # genau dieser Wand. Trifft sie BEIDE, liegt der Punkt diagonal ausserhalb des
            # Rechtecks, und das Ergebnis ist die ECKE - kein Messwert, sondern der
            # Anschlag. Siehe _klemm_xy unten.
            _kx = (mx < R[0]) or (mx > R[0] + R[2] - 1)
            _ky = (my < R[1]) or (my > R[1] + R[3] - 1)
            _klemm['xy'] = bool(_kx and _ky)
            if mx < R[0]: mx = R[0]
            if mx > R[0] + R[2] - 1: mx = R[0] + R[2] - 1
            if my < R[1]: my = R[1]
            if my > R[1] + R[3] - 1: my = R[1] + R[3] - 1
            return pg, r, mx, my
        R = rects(pg)[r]
        if x1 <= x0 or z1 <= z0: return None
        fx = min(max(wx - x0, 0), x1 - x0)
        fz = min(max(wz - z0, 0), z1 - z0)
        _or = ZONE_ORIENT.get((room, zi))
        if _or:
            if _or[0]: fx = (x1 - x0) - fx
            if _or[1]: fz = (z1 - z0) - fz
        # z GESPIEGELT — wie die Original-Markerformel (FUN_800473f8 negiert das
        # z-Ergebnis) und wie re15_map_zones.c seit der Korrektur vom 2026-08-31.
        return pg, r, R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0)

    # ================= SCHEMA-ZEICHNUNG AUS DER KOLLISIONS-BOX =================
    # Nutzer 2026-09-01: "Wo die Kartenlage fehlt oder unklar ist, die Collision-Box des
    # Raums nutzen."
    #
    # 42 Zonen haben KEIN Karten-Rechteck - teils weil das Original fuer den Raum keins
    # fuehrt, teils weil die Zuordnung nicht belegbar ist (ROOM10D0, der 2F-Flur, misst
    # im ausgelieferten Massstab 70 x 89 px; das groesste Rechteck seiner Seite ist
    # 72 x 64). Bisher blieben diese Raeume auf der Karte LEER, und der Spieler-Marker
    # fand dort gar keine Zone.
    #
    # Statt dessen wird der Raum jetzt aus seinen eigenen KOLLISIONSZELLEN gezeichnet
    # (RDT +0x20, dieselben Daten, aus denen die Zonen kommen). Das ist eine
    # PORT-ERGAENZUNG und ausdruecklich keine Rekonstruktion: das Original zeichnet dort
    # nichts, und seine Kunst ist auch nicht massstabsgetreu zur Kollision (ROOM1100
    # misst echt 41 x 47 px, sein gemaltes Rechteck 24 x 24).
    #
    # MASSSTAB: der Median der ausgelieferten Zeilen @0x800768b0 derselben Stage -
    # STAGE1 459/464, STAGE2 448/454, STAGE3 481/448, STAGE4 683/648, STAGE5 601/612,
    # STAGE6 485/455 Welteinheiten je Pixel. Gemessen, nicht gewaehlt. Passt der Raum
    # damit nicht in das Kartenfeld, wird gleichmaessig verkleinert, bis er hineinpasst.
    # LAGE: ueber eine Tuer zu einem bereits gezeichneten Nachbarn derselben Seite - die
    # Tuer muss dort liegen, wo der Nachbar denselben Durchgang zeichnet. Ohne solchen
    # Nachbarn wird der Raum mittig ins Kartenfeld gesetzt.
    SICHT = (100, 55, 132, 140)          # Kartenfeld: alle Rechtecke aller Seiten liegen darin
    def _stage_massstab():
        import statistics as _st
        out = {}
        for st in range(1, 7):
            xs = []; ys = []
            for rid in sorted(zinfo):
                if (rid >> 12) != st: continue
                _o, _p, _a, _b = _zeile(rid)
                if _a > 1 and _b > 1: xs.append(_a); ys.append(_b)
            if xs: out[st] = ((1 << 20) / _st.median(xs), (1 << 20) / _st.median(ys))
        return out
    MASS = _stage_massstab()

    synth = {}        # (seite, raum, zone) -> (x, y, w, h, [zellen], abbildung|None)
    # Die Grundrisse aus dem Loeser haben Vorrang: sie sind zusammenhaengend, ihre
    # Durchgaenge beruehren sich, und sie tragen eine affine Abbildung fuer den Marker.
    #
    # ⛔ DIESER VORRANG IST DER GRUND, WARUM DIE KARTE ZU 100 % SELBST GEZEICHNET IST.
    # Die Zuordnung auf die gemalten Original-Rechtecke wird oben BERECHNET (assign,
    # 75 von 103 Zonen) und hier bedingungslos ueberschrieben - gemessen 2026-09-04:
    # 234 von 234 Zonen tragen rect = 255. Nutzer: "Die Map hat noch immer die
    # selbstgezeichneten Raeume."
    # RE15_KUNST=1 dreht den Vorrang um: wo eine Zuordnung existiert, gewinnt das
    # gemalte Rechteck des Originals; nur der Rest wird gezeichnet.
    # ⛔ KEIN MISCHEN. Ein Loeser-Grundriss steht in SEINEM Koordinatensystem, die
    # gemalte Kachel in dem des Kuenstlers - es gibt keinen gemeinsamen Weltraum
    # (BEFUND §26). Beides auf ein Blatt zu legen erzeugt Ueberlappungen, und weil die
    # Kacheln zuerst eingetragen werden (frueher = oben, Memory
    # reai-v2-zeichenreihenfolge-invers), verschwinden die Kaesten darunter: gemessen
    # 2026-09-04 zeigten dann nur noch 4 von 96 Raeumen ihre Hervorhebung.
    # Mit RE15_KUNST (Default) zeichnet der Port deshalb NUR die Kunst - so wie das
    # Original, das fuer diese Raeume ebenfalls nichts malt. RE15_KUNST=misch stellt die
    # Mischung zum Nachmessen wieder her.
    _MISCH = os.environ.get('RE15_KUNST') == 'misch'
    for _k, (_ab, _kasten, _rects) in sorted(grundrisse.items()):
        if KUNST_VOR and not _MISCH:
            continue
        if KUNST_VOR and assign.get((_k[1], _k[2])) is not None:
            continue
        synth[_k] = (_kasten[0], _kasten[1], _kasten[2], _kasten[3], _rects, _ab)
    for b in sorted(zinfo):
        pg = page_of(b)
        if pg is None or pg == 0xd: continue
        for zi, bb in enumerate(zinfo[b]):
            if (pg, b, zi) in synth: continue       # hat schon einen Grundriss
            if assign.get((b, zi)) is not None: continue
            if KUNST_VOR and not _MISCH: continue   # nur die Kunst, siehe oben
            x0, x1, z0, z1 = bb
            if x1 <= x0 or z1 <= z0: continue
            ex, ey = MASS.get(b >> 12, (460.0, 460.0))
            w = (x1 - x0) / ex; h = (z1 - z0) / ey
            if w < 4 or h < 4: continue
            # gleichmaessig verkleinern, bis der Raum ins Kartenfeld passt
            f = min(1.0, SICHT[2] / w, SICHT[3] / h)
            ex /= f; ey /= f
            w = (x1 - x0) / ex; h = (z1 - z0) / ey
            # Lage ueber einen gezeichneten Nachbarn
            ox = oy = None
            for d in doors_all.get(b, []):
                if d['rw'] == 0 and d['rd'] == 0: continue
                if zone_at(b, d['lx'], d['lz']) != zi: continue
                nb = None
                for zj in range(len(zinfo.get(d['dest'], []))):
                    pr = assign.get((d['dest'], zj))
                    if pr and pr[0] == pg: nb = (d['dest'], zj, pr); break
                if not nb: continue
                mp = to_map(nb[0], nb[1], d['nx'], d['nz'])
                if not mp: continue
                # die eigene Tuer soll dort liegen
                fx = (d['lx'] - x0) / ex
                fy = h - (d['lz'] - z0) / ey
                ox = mp[2] - fx; oy = mp[3] - fy
                break
            if ox is None:
                ox = SICHT[0] + (SICHT[2] - w) / 2.0
                oy = SICHT[1] + (SICHT[3] - h) / 2.0
            # ⛔ NICHT AUF DIE KUNST LEGEN. Die gemalten Rechtecke des Originals sind
            # NICHT massstabsgetreu zur Kollision (ROOM1100 misst echt 41x47 px, sein
            # Rechteck 24x24). Eine massstabsgetreue Kollisions-Zeichnung ueberdeckt
            # deshalb die Nachbarn: ROOM10D0 anker-genau gesetzt lag ueber ROOM10C0,
            # ROOM10E0, ROOM10F0 und ROOM1100 zugleich.
            # Der Anker bleibt die bevorzugte Lage; ueberlappt sie gemalte Rechtecke
            # DIESER Seite, wird die naechstgelegene freie Stelle im Kartenfeld genommen
            # und, wenn keine frei ist, die Zeichnung verkleinert, bis eine frei wird.
            kunst = [rects(pg)[rr] for (bb2, zz2) in [(0, 0)] for rr in range(len(rects(pg)))
                     if any(v == (pg, rr) for v in assign.values())]
            def _ueberlappt(px, py, pw, ph):
                for (RX, RY, RW, RH) in kunst:
                    if px < RX + RW and RX < px + pw and py < RY + RH and RY < py + ph:
                        return True
                return False
            def _suche(pw, ph):
                kand = []
                for gy in range(SICHT[1], SICHT[1] + SICHT[3] - int(ph) + 1, 2):
                    for gx in range(SICHT[0], SICHT[0] + SICHT[2] - int(pw) + 1, 2):
                        if _ueberlappt(gx, gy, pw, ph): continue
                        kand.append(((gx - ox) ** 2 + (gy - oy) ** 2, gx, gy))
                return min(kand) if kand else None
            ox = max(SICHT[0], min(ox, SICHT[0] + SICHT[2] - w))
            oy = max(SICHT[1], min(oy, SICHT[1] + SICHT[3] - h))
            if _ueberlappt(ox, oy, w, h):
                treffer = _suche(w, h)
                schrumpf = 1.0
                while treffer is None and schrumpf > 0.34:
                    schrumpf -= 0.08
                    treffer = _suche(w * schrumpf, h * schrumpf)
                if treffer is not None:
                    ex /= schrumpf; ey /= schrumpf
                    w = (x1 - x0) / ex; h = (z1 - z0) / ey
                    ox, oy = treffer[1], treffer[2]
                else:
                    continue        # kein Platz: lieber nichts zeichnen als Matsch
            # Zellen in Karten-Pixel (nur flaechige; duenne Waende ergeben keine Flaeche)
            zellen = []
            for (cx, cz, cw, cd) in sorted(set(c[:4] for c in sca_all.get(b, ()))):
                if not (x0 <= cx <= x1 and z0 <= cz <= z1): continue
                px0 = ox + (cx - x0) / ex; px1 = ox + (cx + cw - x0) / ex
                py1 = oy + h - (cz - z0) / ey; py0 = oy + h - (cz + cd - z0) / ey
                # ⛔ AUF DEN KASTEN BESCHNEIDEN. Eine Zelle kann mit ihrer ECKE in der
                # Zonen-Bbox liegen und weit darueber hinausragen (ROOM10D0 fuehrt eine
                # 2000 x 26250 grosse Zelle). Ungeschnitten wurden daraus senkrechte
                # Streifen, die ueber das ganze Kartenfeld liefen - im Abzug des
                # 2F-Blattes deutlich zu sehen.
                # ⛔ NACH dem Runden beschneiden, nicht davor: sonst schiebt die
                # Rundung die rechte/untere Kante wieder einen Pixel hinaus (17 von 692
                # Zellen taten das).
                ix0 = max(int(round(ox)), int(round(max(px0, ox))))
                iy0 = max(int(round(oy)), int(round(max(py0, oy))))
                ix1 = min(int(round(ox + w)), int(round(min(px1, ox + w))))
                iy1 = min(int(round(oy + h)), int(round(min(py1, oy + h))))
                iw = ix1 - ix0; ih = iy1 - iy0
                if iw < 1 or ih < 1: continue
                zellen.append((ix0, iy0, iw, ih))
            if not zellen: continue
            synth[(pg, b, zi)] = (int(round(ox)), int(round(oy)),
                              max(1, int(round(w))), max(1, int(round(h))), zellen, None)
    print(f"{len(synth)} Zonen ohne Rechteck bekommen eine Zeichnung aus ihrer Kollisions-Box")

    # Synthetische Zonen bekommen eine eigene Zeilen-Nummer und einen Platz in der
    # Zonen-Tabelle. Ihr "Rechteck" ist der Kasten der Schema-Zeichnung; als Kennung
    # traegt die Zeile rect = 255.
    synth_liste = []          # (x, y, w, h, erste_zelle, n_zellen, abbildung)
    synth_zellen = []
    # Behaltene Raeume zuerst, damit der Varianten-Raum ihren Index schon vorfindet.
    _si_von = {}
    for (pg, b, zi) in sorted(synth, key=lambda k: (1 if k[1] in _ALIAS else 0, k)):
        x, y, w, h, zellen, ab = synth[(pg, b, zi)]
        # ⛔ MINDESTENS 4 px KANTE. Ein Rechteck darunter ist auf dem Schirm kein Raum
        # mehr, sondern ein Strich - die Audit-Pruefung B2 nennt das seit v0.3.93 einen
        # Fehler. Es entsteht durch RUNDUNG, nicht durch die Geometrie: das Einpassen
        # rechnet Ursprung und Massstab aus den Extremwerten des Blattes, und schon eine
        # Verschiebung an anderer Stelle rundet jedes Rechteck neu. Gemessen am
        # 2026-09-03 an ROOM2080 z1 auf Blatt 6: 4x4 -> 4x3 px, nachdem der
        # Entsenkungs-Nachlauf einen Ort verschoben hatte, der auf zwei Blaettern liegt.
        # Geklemmt wird die ANZEIGE, nicht die Abbildung - die Marker-Rechnung haengt an
        # `ab` und bleibt unberuehrt.
        if w < 4: w = 4
        if h < 4: h = 4
        _quelle = (pg, _ALIAS.get(b, b), zi)
        if b in _ALIAS and _quelle in _si_von:
            # DERSELBE ORT: Zeichnung teilen statt ein zweites Mal anlegen.
            si = _si_von[_quelle]
        else:
            si = len(synth_liste)
            synth_liste.append((x, y, w, h, len(synth_zellen), len(zellen),
                                ab or (0, 0, 0, 0, 0, 0)))
            synth_zellen.extend(zellen)
        _si_von[(pg, b, zi)] = si
        bb = zinfo[b][zi]
        # ⛔ EINE ZEILE JE BLATT. Ein Ort, der ueber mehrere Ebenen reicht, hat auf
        # jedem seiner Blaetter eine eigene Lage - aber DIESELBE Zonen-Nummer (zid), denn
        # er ist derselbe Ort und die Marken haengen daran. Die Zweitzeile traegt
        # etage = 1; ihre Sichtbarkeit haengt dann am ETAGEN-Bit, nicht am Zonen-Bit
        # (sonst kommt der Fehler zurueck, den der Nutzer am 2026-09-01 gemeldet hat:
        # "im Room 1130 gibt es unten links schon ein Rechteck, obwohl ich noch im
        # Eingangsbereich stehe" - dort galt eine 3F-Zeichnung als bekannt, sobald man
        # das Treppenhaus irgendwo betreten hatte).
        gast = 1 if pg != page_of(b) else 0
        _zq = (_ALIAS.get(b, b), zi)
        if b in _ALIAS and _zq in zid_von:
            # DERSELBE ORT: ein Besucht-Bit fuer beide Skriptzustaende.
            zid_von[(b, zi)] = zid_von[_zq]
        elif (b, zi) not in zid_von:
            zid_von[(b, zi)] = zid
            zid += 1
        for var in (0, 1):
            rows.append((b + var, bb, pg, 255, zi, zid_von[(b, zi)], si + 1, gast))


    o.append("static const re15_map_zone_t s_map_zones[] = {")
    for _row in rows:
        room, bb, pg, r, zi, zd = _row[:6]
        _sy = _row[6] if len(_row) > 6 else 0
        _et = _row[7] if len(_row) > 7 else 0
        ei = eichung.get((room & 0xFFF0, zi), (0, 0, 0, 0))
        _o = ZONE_ORIENT.get((room & 0xFFF0, zi), (0, 0))
        o.append(f"    {{ 0x{room:04X}, {bb[0]:6d}, {bb[2]:6d}, {bb[1]:6d}, {bb[3]:6d}, {pg:2d}, {r:3d}, {zi}, {zd:3d},"
                 f" {ei[0]:5d}, {ei[1]:5d}, {ei[2]:5d}, {ei[3]:5d}, {_o[0]}, {_o[1]}, {_sy:3d}, {_et} }},")
    o.append("};")
    belegte_rects = {(p_, r_) for (p_, r_) in assign.values()}

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
    # ⛔ AUS DEM VORLAUF, NICHT MEHR UEBER DIE KACHEL-uv (2026-09-02). Frueher wurde
    # die Zweitzeichnung als GEMALTES Rechteck des Originals gesucht, ueber die
    # Gleichheit der Kachel-uv - das setzte voraus, dass der Ort ueberhaupt ein
    # Kunst-Rechteck hat, und lieferte 12 Zeilen. Seit der Loeser den Ort auf JEDEM
    # Blatt setzt, das eines seiner Baender erreicht (Vorlauf ETAGEN/GAST), ist die
    # Zweitzeichnung ein richtiger Grundriss und die Zeile traegt rect = 255.
    # ⛔ DAS RECHTECK MUSS AUS DER ZONENZEILE KOMMEN, NICHT PAUSCHAL 255 SEIN.
    # NUTZER-BEFUND 2026-09-04 (fehler/error2.png und error3.png, zweimal derselbe Satz:
    # "does not turn red when i am in the room") - ROOM1170s unterer Bereich blieb sowohl
    # auf dem ROOF-Blatt als auch auf 3F gruen, obwohl der Spielermarker sichtbar darin
    # stand.
    #
    # URSACHE: re15_map_rect_state (re15_map_zones.c:312-320) entscheidet die rote
    # Hervorhebung so:
    #       hat_etage = re15_map_floor_lookup(zn->room, zn->idx, band, &fp, &fr);
    #       if (hat_etage) { if (page == fp && rect_idx == fr) return CURRENT; }
    # Mit rect = 255 kann `rect_idx == fr` fuer ein GEMALTES Rechteck (Index 0..13) nie
    # zutreffen - der Raum wird also NIE rot. Gemessen ueber die ganze Tabelle: ALLE 22
    # Etagen-Zeilen trugen 255, waehrend ihre Zonenzeile ein echtes Rechteck nennt
    # (ROOM1060, ROOM1080, ROOM10A0, ROOM10F0, ROOM1170, ROOM11A0, ROOM3080, ROOM4020,
    # ROOM4070, ROOM50D0). Kein einziger stockwerkuebergreifender Raum konnte rot werden.
    #
    # Die 255 stammt aus dem Kommentar unten ("die Zweitzeichnung ist ein richtiger
    # Grundriss") - das galt, solange der Loeser Schema-Zeichnungen setzte. Seit die
    # Original-Kunst der Auslieferungsstand ist (KUNST_VOR), traegt die Gast-Zeile ein
    # GEMALTES Rechteck, und die Etagen-Zeile muss dasselbe nennen.
    #
    # Der Marker war davon nicht betroffen: er holt sein Rechteck ueber
    # re15_map_zone_fuer aus der ZONE (re15_inv_screen.c:655) und ueberschreibt fr.
    # Genau deshalb stand er sichtbar in einem gruenen Rechteck - der Widerspruch, den
    # der Nutzer gemeldet hat.
    _zonen_rect = {}
    for _row in rows:
        _rroom, _rbb, _rpg, _rr, _rzi = _row[:5]
        _zonen_rect.setdefault((_rroom & 0xFFF0, _rzi, _rpg), _rr)
    floors = [(b, zi, band, zp, _zonen_rect.get((b, zi, zp), 255))
              for (b, zi, band, zp) in ETAGEN]
    _mit_kunst = sum(1 for f in floors if f[4] != 255)
    print("   Etagen-Zeilen mit gemaltem Rechteck: %d von %d"
          % (_mit_kunst, len(floors)))

    # Filter und Besucher-Regel stehen im VORLAUF (ETAGEN), damit der Loeser die
    # Gast-Lagen schon beim Setzen kennt.


    # ---- BLATT EINER MARKE FOLGT IHREM EIGENEN BAND ---------------------------
    # ⛔ Nutzer 2026-09-01: "Bei ROOM 1170 zeigt er die Tuer, die eigentlich fuer die
    # Etage unten gedacht ist, bereits bei der Roof-Etage an."
    # Gemessen: ROOM1170s zweiter Bereich fuehrt DREI Tueren - zwei mit Band 0
    # (-> ROOM1130 und ROOM1140, beide auf dem 3F-Blatt) und eine mit Band 4
    # (-> ROOM1170 selbst, ROOF). Das Band steht im Aot_set-Datensatz der Tuer,
    # pc[4] = obj[0x82]; das Original gattet die Tuer-Interaktion darauf
    # (FUN_8002bd44 @0x8002bf38). Der Port hatte alle Marken auf die Seite der ZONE
    # gelegt (ROOF) und sie von dort auf die Etagenblaetter kopiert - die beiden
    # 3F-Tueren standen damit auf dem Dach.
    # Jetzt bestimmt das Band der Marke ihr Blatt. Fuer eine Treppe faellt das von
    # selbst richtig: sie liegt als ZWEI Datensaetze vor, einer je Ende, und jedes
    # Ende traegt sein eigenes Band - also erscheint sie auf beiden Etagen, die sie
    # verbindet, ohne Sonderregel.
    etagen_der_zone = {}
    for (froom, fzi, fband, fpg, fr) in floors:
        etagen_der_zone.setdefault((froom, fzi), []).append((fband, fpg, fr))

    def blatt_fuer_band(room, zi, band):
        """(Seite, Rect) fuer dieses Band - oder None, wenn die Zone keine Etagen hat."""
        e = etagen_der_zone.get((room, zi))
        if not e or len(e) < 2: return None
        best = None; bestd = None
        for (fband, fpg, fr) in e:
            d = abs(fband - band)
            if bestd is None or d < bestd: best, bestd = (fpg, fr), d
        return best

    # ---- MARKEN: Treppen, in Karten-Koordinaten vorberechnet ------------------
    # Der Nutzer: "die [Tuer] ist auf der Karte nicht eingezeichnet ... auserdem
    # muesste links im kleinen rechteck die Treppe eingezeichnet sein."
    # Position: Welt -> Zone -> Rechteck (dieselbe lineare Abbildung wie der Marker).
    def _gr_zelle(room, zi, pg, x, y):
        """1, wenn (x,y) auf der gezeichneten Flaeche dieses Ortes liegt. Damit eine
        verschobene Marke die Wand nicht verlaesst."""
        g = _gr(room, zi, pg)
        if not g: return False
        for (cx, cy, cw, ch) in g[2]:
            if cx <= x < cx + cw and cy <= y < cy + ch:
                return True
        return False

    def snap_grundriss(room, zi, mx, my, senk, pg=None):
        """Wie snap_wall, aber auf der SILHOUETTE des Grundrisses statt auf der
        Original-Kachel. snap_wall liest die gemalte Kachel (DATA/MAP0x.PIX,
        Palettenindex 0 = ausserhalb) - fuer einen Grundriss gibt es keine Kachel,
        dafuer die exakte Zellgeometrie, und die ist die bessere Quelle.
        Gleiche Kostenregel wie snap_wall: quer zur Wand wird ein schmales Fenster
        mitgesucht und doppelt gewichtet, damit die Wandachse fuehrend bleibt."""
        g = _gr(room, zi, pg)
        if not g: return mx, my, (3 if senk else 0)
        belegt = set()
        for (x, y, w, h) in g[2]:
            for j in range(y, y + h):
                for i in range(x, x + w):
                    belegt.add((i, j))
        if not belegt: return mx, my, (3 if senk else 0)
        QUER = 3
        best = None
        if senk:
            for dj in range(-QUER, QUER + 1):
                j = my + dj
                for (i, jj) in belegt:
                    if jj != j: continue
                    if (i - 1, j) in belegt and (i + 1, j) in belegt: continue
                    k = abs(i - mx) + 2 * abs(dj)
                    seite = 3 if (i + 1, j) in belegt else 1     # 3 = West, 1 = Ost
                    if best is None or k < best[0]: best = (k, i, j, seite)
        else:
            for di in range(-QUER, QUER + 1):
                i = mx + di
                for (ii, j) in belegt:
                    if ii != i: continue
                    if (i, j - 1) in belegt and (i, j + 1) in belegt: continue
                    k = abs(j - my) + 2 * abs(di)
                    seite = 0 if (i, j + 1) in belegt else 2     # 0 = Nord, 2 = Sued
                    if best is None or k < best[0]: best = (k, i, j, seite)
        if best is None: return mx, my, (3 if senk else 0)
        return best[1], best[2], best[3]

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
    # ⛔ DIESELBE ZAEHLUNG WIE DIE ZONEN-TABELLE. Hier lief frueher eine ZWEITE,
    # eigene Numerierung, die nur die Zonen mit gemaltem Rechteck zaehlte. Solange
    # ausschliesslich solche Zonen eine Zeile bekamen, stimmte sie zufaellig ueberein;
    # seit die Grundriss-Zonen eigene Zeilen tragen, zeigte sie auf fremde Zonen - und
    # die Sichtbarkeit einer Marke haengt genau daran (re15_map_mark_get prueft
    # s_visited[zid]).
    zid_of = zid_von

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
            # ⛔ IN WELTKOORDINATEN MITTELN, NICHT IN KARTENPIXELN. Die beiden Enden
            # einer Treppe tragen VERSCHIEDENE Baender (hier 0 und 2, bzw. 2 und 4) und
            # gehoeren damit auf verschiedene BLAETTER - eine Treppe verbindet ja zwei
            # Ebenen. Wer sie in Kartenpixeln mittelt, friert dabei EIN Blatt ein: nach
            # dem Umbau auf Gast-Lagen landeten so alle vier Treppen-Datensaetze von
            # ROOM1170 auf dem Dach-Blatt, und auf 3F fehlte die Treppe ganz
            # (Pin test_map_mark_band). Gemittelt wird deshalb der WELT-Punkt; jedes Ende
            # wird anschliessend mit der Abbildung SEINES Blattes projiziert.
            if za is None or zb is None: continue
            st_merge[_i] = ((A['x'] + B['x']) // 2, (A['z'] + B['z']) // 2)
            st_merge[_j] = ((A['x'] + B['x']) // 2, (A['z'] + B['z']) // 2)
            _used.add(_i); _used.add(_j)

        for kind, lst in ((0, doors_all.get(b, [])), (1, st_list)):
            for _n, m in enumerate(lst):
                wx = m['lx'] if kind == 0 else m['x']
                wz = m['lz'] if kind == 0 else m['z']
                if kind == 0 and b in _ALIAS:
                    _doppelt = False
                    for _k in doors_all.get(_ALIAS[b], ()):
                        if abs(_k['lx'] - wx) <= max(_k['rw'], m['rw']) and                                 abs(_k['lz'] - wz) <= max(_k['rd'], m['rd']):
                            _doppelt = True
                            break
                    if _doppelt:
                        continue
                _sp = (os.environ.get('RE15_MARKEN_DUMP', '').lower()
                       == ('%04x' % b))
                zi = zone_at(b, wx, wz)
                if zi is None:
                    if _sp:
                        print("   [Marke] ROOM%04X %s #%d bei (%d,%d): KEINE ZONE"
                              % (b, 'Tuer' if kind == 0 else 'Treppe', _n, wx, wz))
                    continue
                # ⛔ DAS BLATT KOMMT ZUERST, DANN DIE PROJEKTION. Frueher wurde die
                # Marke auf dem EIGENEN Blatt projiziert und danach auf das Blatt ihres
                # Bandes umgerechnet - erst per Rechteck-Versatz, spaeter per Einpassung
                # in ein fremdes Kunst-Rechteck. Seit der Ort auf jedem seiner Blaetter
                # eine eigene Lage vom Loeser hat, ist das ueberfluessig: die Marke wird
                # gleich mit der Abbildung IHRES Blattes gerechnet.
                _bl = blatt_fuer_band(b, zi, m.get('band', 0))
                _pgw = _bl[0] if _bl else None
                if kind == 1 and _n in st_merge:
                    wx, wz = st_merge[_n]        # gemeinsamer WELT-Punkt beider Enden
                mp = to_map(b, zi, wx, wz, _pgw)
                _klemm_xy = _klemm['xy']
                if not mp:
                    mp = to_map(b, zi, wx, wz)
                    _klemm_xy = _klemm['xy']
                if not mp:
                    if _sp:
                        print("   [Marke] ROOM%04X %s #%d Zone z%d: KEINE ABBILDUNG "
                              "(Blattwunsch %s)"
                              % (b, 'Tuer' if kind == 0 else 'Treppe', _n, zi, _pgw))
                    continue
                if _sp:
                    print("   [Marke] ROOM%04X %s #%d Zone z%d -> Blatt %d (%d,%d)"
                          % (b, 'Tuer' if kind == 0 else 'Treppe', _n, zi,
                             mp[0], mp[2], mp[3]))
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
                        R = _geo(pg, r, b, zi)
                        if R is None: continue
                        senk = (min(mx - R[0], R[0] + R[2] - 1 - mx) <
                                min(my - R[1], R[1] + R[3] - 1 - my))
                    if r == 255:
                        mx, my, mkind = snap_grundriss(b, zi, mx, my, senk, pg)
                    else:
                        mx, my, mkind = snap_wall(pg, r, mx, my, senk)
                    # ⛔ EIN RECHTECK IM RECHTECK: DIE TUER GEHOERT AUF DEN INNEREN RAND.
                    # Nutzer-Befund 2026-09-05 (fehler/error.png): "This door is wrong"
                    # und "Staircase 2F only has one door!". Gemeint ist dieselbe Marke:
                    # ROOM10C0s Tuer zur FAHRSTUHLKABINE ROOM1080. Die Kabine ist auf
                    # Blatt 3 als eigenes kleines Rechteck gezeichnet (Rect 4, (109,134)
                    # 16x16) und liegt VOLLSTAENDIG im Rechteck des Flurs (Rect 0,
                    # (102,116) 40x40). snap_wall() rueckt die Marke auf eine Wand der
                    # FLUR-Kachel - sie landete bei (126,134), also zwei Pixel neben der
                    # Kabine und genau auf der Oberkante des Treppenhauses (Rect 1,
                    # (118,134)). Fuer den Spieler sah das aus wie eine zweite Tuer AM
                    # TREPPENHAUS, das auf 2F nur eine hat.
                    # Wo zwei Rechtecke ineinander liegen, gibt es keine gemeinsame
                    # Kante - die Grenze IST der Rand des inneren. Die Marke wird deshalb
                    # dorthin gezogen, aber nur in genau diesem Fall (Ziel-Rechteck
                    # vollstaendig im eigenen); beruehrende Rechtecke bleiben unberuehrt,
                    # dort trifft snap_wall die gemeinsame Wand bereits.
                    _ziel = m.get('dest')
                    if _ziel is not None:
                        _zr = None
                        for _zzi in range(4):
                            _c = _zonen_rect.get((_ziel & 0xFFF0, _zzi, pg))
                            if _c is not None and _c != 255 and _c != r:
                                _zr = _c
                                break
                        if _zr is not None:
                            _R = rects(pg)
                            if r < len(_R) and _zr < len(_R):
                                ax, ay, aw, ah = _R[r]
                                bx, by, bw, bh = _R[_zr]
                                drin = (bx >= ax and by >= ay and
                                        bx + bw <= ax + aw and by + bh <= ay + ah)
                                if drin:
                                    # naechster Punkt auf dem Rand des inneren Rechtecks
                                    cx = min(max(mx, bx), bx + bw - 1)
                                    cy = min(max(my, by), by + bh - 1)
                                    dl, dr_ = cx - bx, bx + bw - 1 - cx
                                    do, du = cy - by, by + bh - 1 - cy
                                    kleinst = min(dl, dr_, do, du)
                                    if kleinst == dl:   mx, my, mkind = bx, cy, 3
                                    elif kleinst == dr_: mx, my, mkind = bx + bw - 1, cy, 1
                                    elif kleinst == do:  mx, my, mkind = cx, by, 0
                                    else:                mx, my, mkind = cx, by + bh - 1, 2
                    # ⛔ HIER NOCH NICHT VERWERFEN. Bis v0.3.80 fiel eine Marke schon in
                    # Durchgang 1 weg, wenn ihr eigenes Rechteck die Tuer malt - und ihr
                    # Partner blieb als freie Einzelmarke stehen, weil die Paarung erst in
                    # Durchgang 2 laeuft (15 Paare game-weit). Der Durchgang wird jetzt als
                    # EINHEIT behandelt: erst paaren, dann filtern.
                else:
                    # TREPPE: Sprossen quer zur Laufrichtung. map_x = welt_x,
                    # map_y = -welt_z, also X-Treppe -> senkrechte Sprossen.
                    mkind = 5 if m.get('axis') == 12 else 4
                vor.append({'room': b, 'zi': zi, 'idx': _n, 'kind': kind, 'pg': pg, 'r': r,
                            'mx': mx, 'my': my, 'seite': mkind, 'klemm_xy': _klemm_xy,
                            'zid': zid_of.get((b, zi), 0), 'd': m})
                # ⛔ KEINE ZWEITE MARKE AUF DEM EIGENEN BLATT DES ORTES.
                # Versucht am 2026-09-03, weil 12 von 244 Tueren beim Davorstehen
                # kein Symbol zeigten (die Marke liegt auf dem Blatt des BANDES,
                # der Spieler sieht das Blatt seines Ortes). Die Tuer zusaetzlich
                # auf dem eigenen Blatt zu zeichnen brachte +2 sichtbare Symbole -
                # und verletzte eine ausdrueckliche Anforderung: der Pin
                # unit_map_mark_band haelt fest, dass das ROOF-Blatt KEINE Tuer des
                # unteren Bereichs von ROOM1170 traegt (aus einem Nutzer-Report
                # abgeleitet). Dazu kam ein weiteres Paar Symbole uebereinander.
                # Zurueckgenommen: zwei sichtbare Symbole wiegen eine gesetzte
                # Anforderung nicht auf.
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
        if A['pg'] != B['pg']:
            # Verschiedene Blaetter: jede Seite zeigt ihre eigene Tuer. Die beiden
            # Datensaetze duerfen dabei NICHT als verbraucht gelten - sonst nimmt ein
            # folgenloses Paar einem dritten Datensatz den Partner weg.
            continue
        # ⛔ NUR VERSCHMELZEN, WENN SICH DIE RECHTECKE BERUEHREN.
        # Die Verschmelzung setzt beide Enden auf EINE Position und stuetzt sich dabei
        # auf "die beiden Rechtecke teilen sich eine Kante". Trifft das nicht zu, wandert
        # die Marke in ein fremdes Rechteck: gemessen an ROOM10C0 <-> ROOM1060 auf dem
        # 2F-Blatt - Rect 8 (164,77,24,24) und Rect 9 (109,134,16,16) liegen 84 bzw.
        # 110 px auseinander, und die Verschmelzung schob ROOM10C0s Tuer in das
        # 16x16-Kaestchen des Treppenhauses, wo das Original nichts malt (Katalog: 0
        # Symbole auf Rect (3,8)). Die Beruehrung ist eine Aussage der KACHEL und
        # braucht keine freie Zahl.
        # ⛔ BERUEHRUNGS-PROBE NUR FUER GEMALTE RECHTECKE. Sie liest die Kachel
        # (DATA/MAP0x.PIX) - fuer einen Grundriss gibt es keine, und der Loeser hat die
        # Beruehrung ohnehin selbst hergestellt (er heftet die Raeume an genau dieser
        # Tuer aneinander). Fuer Grundrisse wird deshalb der Abstand der KAESTEN
        # geprueft: was der Loeser gesetzt hat, liegt beieinander.
        if A['r'] == 255 or B['r'] == 255:
            _KA = _geo(A['pg'], A['r'], A['room'], A['zi'])
            _KB = _geo(B['pg'], B['r'], B['room'], B['zi'])
            _nah = False
            if _KA and _KB:
                _dx = max(_KB[0] - (_KA[0] + _KA[2]), _KA[0] - (_KB[0] + _KB[2]), 0)
                _dy = max(_KB[1] - (_KA[1] + _KA[3]), _KA[1] - (_KB[1] + _KB[3]), 0)
                _nah = (_dx + _dy) <= 2
            if not _nah:
                belegt.add(_i); belegt.add(_j)
                continue
        elif not _beruehren(A['pg'], A['r'], B['r']):
            belegt.add(_i); belegt.add(_j)
            continue
        belegt.add(_i); belegt.add(_j)
        # ⛔ NICHT MITTELN. Die beiden Rechtecke liegen auf dem Blatt so, wie der
        # Kuenstler sie gesetzt hat; unsere zwei linearen Projektionen koennen weit
        # auseinander fallen (gemessen ROOM1130 <-> ROOM1120: (152,125) gegen
        # (120,149), 34 px). Der Mittelwert lag dann ausserhalb BEIDER Rechtecke.
        # Genommen wird die Position, die am dichtesten am NACHBAR-Rechteck liegt -
        # das ist die, die wirklich auf der gemeinsamen Kante sitzt.
        def _abstand(X, Y):
            _R = _geo(Y['pg'], Y['r'], Y['room'], Y['zi'])
            if _R is None: return 0
            RX, RY, RW, RH = _R
            dx = max(RX - X['mx'], 0, X['mx'] - (RX + RW - 1))
            dy = max(RY - X['my'], 0, X['my'] - (RY + RH - 1))
            return dx + dy
        W = A if _abstand(A, B) <= _abstand(B, A) else B
        cx, cy = W['mx'], W['my']
        # ============ DIE MARKE SITZT AUF DER GEMEINSAMEN KANTE ===================
        # Nutzer 2026-09-02: "Die Tueren sind durch die Bank weg alle falsch platziert
        # ... Nach dem Durchgehen der Tuer ist man in der Mitte des Raumes in ein
        # Rechteck das nicht an das vorherige anschliesst."
        #
        # ⛔ URSACHE, LIVE GEMESSEN (nicht modelliert): die Position kam bis hierher aus
        # ZWEI getrennten Projektionen - jeder Raum projizierte seinen eigenen
        # Tuer-Datensatz und schnappte ihn an die naechste Wand SEINES Rechtecks. Beide
        # Schritte sind unzuverlaessig:
        #   - Der Tuerpunkt liegt im Median nur 3 px von der naechsten Wand, aber nur
        #     4 px vor der ZWEITnaechsten; bei 23 % der Tueren betraegt der Vorsprung
        #     <= 1 px. "Naechste Wand" ist dort ein Muenzwurf, kein Kriterium.
        #   - Danach entschied ein Vergleich, welche der beiden Projektionen gewinnt.
        #     Die Verliererseite behielt eine Wand, die mit der Gewinnerseite nichts zu
        #     tun hat.
        # Ergebnis auf dem Schirm: 58 % der Symbole lagen auf einer anderen Wand als
        # der, an der der Loeser die beiden Raeume verheftet hat.
        #
        # Der Nutzer hat den einfachen Weg vorgegeben ("Er schliesst EXAKT an die Wand
        # der Karte vom Raum davor an, ueberlappt die Wand also") und dazu den RE2-Stil
        # angeboten, in dem eine Tuer ein kurzer Balken IN der Wandlinie ist. Beides
        # zeigt auf dieselbe Loesung: die Marke wird nicht mehr projiziert und
        # geschnappt, sondern aus der BERUEHRUNG DER BEIDEN RECHTECKE bestimmt. Damit
        # ist sie per Konstruktion auf der Wand, die die zwei Raeume teilen - sie kann
        # gegenueber den Raeumen gar nicht mehr falsch liegen.
        _KA2 = _geo(A['pg'], A['r'], A['room'], A['zi'])
        _KB2 = _geo(B['pg'], B['r'], B['room'], B['zi'])
        _seite_kante = None
        if _KA2 and _KB2:
            _ax, _ay, _aw, _ah = _KA2
            _bx, _by, _bw, _bh = _KB2
            # Ueberdeckung der beiden Rechtecke (sie ueberlappen die Wand, siehe oben).
            _ux0 = max(_ax, _bx); _ux1 = min(_ax + _aw, _bx + _bw)
            _uy0 = max(_ay, _by); _uy1 = min(_ay + _ah, _by + _bh)
            if _ux1 > _ux0 or _uy1 > _uy0:
                # Die gemeinsame Wand laeuft LAENGS der laengeren Ueberdeckungsseite;
                # die kuerzere ist die Wanddicke.
                _br = _ux1 - _ux0
                _ho = _uy1 - _uy0
                # ⛔ DIE LAGE LAENGS DER WAND KOMMT AUS DER ROHEN PROJEKTION, NICHT
                # AUS DEM GESCHNAPPTEN WERT. Ein erster Wurf klemmte W['mx'] in die
                # Ueberdeckung - aber W['mx'] war in Durchgang 1 bereits an die
                # "naechste Wand" des eigenen Rechtecks gezogen worden, und genau diese
                # Wand ist der Muenzwurf, den dieser Umbau abschafft. Folge, gemessen:
                # die drei Durchgaenge ROOM1000<->ROOM1050 projizieren sauber auf
                # x=144/164/167 (Gegenseite 143/164/167 - eine Uebereinstimmung von
                # 0-1 px!), landeten nach Schnappen+Klemmen aber auf x=132 und 173,
                # den ECKEN der Ueberdeckung. Zwei der drei Marken fielen danach als
                # Dublette weg, und alle drei Tueren zogen den Marker auf dieselbe Ecke.
                # Genommen wird das Mittel der beiden ROHEN Projektionen: beide Raeume
                # sagen unabhaengig voneinander, wo die Tuer auf der Wand sitzt.
                _rohA = to_map(A['room'], A['zi'], A['d']['lx'], A['d']['lz'], A['pg'])
                _rohB = to_map(B['room'], B['zi'], B['d']['lx'], B['d']['lz'], B['pg'])
                # ⛔ NUR EINE DUENNE UEBERDECKUNG IST EINE WAND. Wo zwei Rechtecke
                # sich grossflaechig ueberlappen (auf Seite 7/9/11 bis zu 31 %),
                # existiert keine gemeinsame Wandlinie - und "Mitte der Ueberdeckung"
                # legte das Symbol dann quer durch den Raum. Gemessen: zehn Symbole
                # lagen exakt auf der MITTELLINIE ihres Nachbarn (ROOM4050: fuenf
                # Marken auf y=119 = genau die Mitte von y[103..135]), was der Pin
                # unit_map_durchgang als "zeigt vom Nachbarn weg" gemeldet hat.
                # Dort gilt die rohe Projektion: beide Raeume projizieren dieselbe Tuer
                # unabhaengig voneinander und stimmen auf 0-1 px ueberein (gemessen an
                # ROOM1000<->ROOM1050: 144/143, 164/164, 167/167). Diese Uebereinstimmung
                # ist das bessere Zeugnis als jede Konstruktion aus den Kaesten.
                _duenn = min(_br, _ho) <= 4
                # ⛔ NUR MITTELN, WENN BEIDE SEITEN UEBEREINSTIMMEN (siehe Kopf-
                # kommentar zu ROOM1210 <-> ROOM1220): weichen sie um mehr als
                # MITTEL_MAX Pixel ab, liegen die beiden Zeichnungen laengs der Wand
                # gegenlaeufig, und das Mittel legt die Marke an eine Stelle, an der
                # KEINE der beiden Seiten eine Tuer hat.
                _rohW = _rohA if W is A else _rohB
                def _laengs(i):
                    if not (_rohA and _rohB):
                        return None
                    if abs(_rohA[i] - _rohB[i]) <= MITTEL_MAX:
                        return (_rohA[i] + _rohB[i]) // 2
                    return _rohW[i] if _rohW else None
                if not _duenn:
                    _mx = _laengs(2); _my = _laengs(3)
                    if _mx is not None: cx = _mx
                    if _my is not None: cy = _my
                    # ⛔ AUCH OHNE WANDLINIE BLEIBT DIE MARKE IM GEMEINSAMEN BEREICH.
                    # Ein erster Wurf liess die rohe Projektion hier ungeklemmt stehen -
                    # der Pin unit_map_durchgang hat daraufhin zwei Marken gemeldet, die
                    # 5 bzw. 10 px ausserhalb der Ueberdeckung lagen (ROOM3090<->ROOM30D0,
                    # ROOM3060<->ROOM3010). Optisch ist das genau der Fehler, den der
                    # Nutzer benannt hat: ein Tuersymbol schwebt im Rauminneren statt
                    # dort zu sitzen, wo die zwei Raeume einander beruehren. Bei einer
                    # grossen Ueberdeckung verschiebt die Klemmung kaum etwas, sie
                    # verhindert nur das Ausbrechen.
                    cx = min(max(cx, _ux0), max(_ux0, _ux1 - 1))
                    cy = min(max(cy, _uy0), max(_uy0, _uy1 - 1))
                    # ⛔ DIE ACHSE GILT AUCH OHNE DUENNE WAND. Bis 2026-09-04 stand hier
                    # _seite_kante = None, und dann entschied weiter unten die Richtung zur
                    # MITTE des Nachbar-Rechtecks ueber die Achse. Genau das ist der
                    # Muenzwurf, den der Kommentar zwoelf Zeilen tiefer selbst verbietet
                    # ("Wo eine Ueberdeckung existiert, gilt sie") - er war fuer
                    # Ueberdeckungen dicker als 4 px nie umgesetzt.
                    # NUTZER-BEFUND 2026-09-04 (fehler/error1.png, "Wrong Rotation, Need 90
                    # Degree turn"): Marke #64, ROOM1130 <-> ROOM1150. Ueberdeckung _br=8
                    # (x), _ho=40 (y) - eine eindeutig SENKRECHTE Wand. min(8,40)=8 > 4,
                    # also _duenn=False, also _seite_kante=None; die Mittenrichtung ergab
                    # _zx=-10, _zy=+15 -> |zx|<|zy| -> Seite 2 (Sued) = WAAGERECHTER Balken.
                    # Die Ueberdeckung sagt das Gegenteil und ist die staerkere Aussage:
                    # sie misst, wo die Wand VERLAEUFT, die Mitten nur, wo der Nachbar
                    # ungefaehr liegt.
                    # ⛔ NUR DIE ACHSE, NICHT DIE LAGE. _duenn regelt weiterhin allein die
                    # POSITION (Mitte der Ueberdeckung vs. rohe Projektion) - die war
                    # getrennt eingemessen und wird hier nicht angefasst.
                    # Gleichstand (_br == _ho) bleibt der bisherigen Entscheidung
                    # ueberlassen: da sagt die Ueberdeckung nichts.
                    # ⛔ EINE UEBERDECKUNG IST NUR DANN EINE WAND, WENN SIE FUER BEIDE
                    # RECHTECKE SCHMAL IST. Ein erster Wurf nahm einfach die laengere
                    # Ueberdeckungsseite als Wandrichtung. Gemessen mit dem Pin
                    # unit_map_durchgang stieg "Symbol zeigt vom Nachbarn weg" damit von
                    # 1 auf 9 - und die neuen Faelle waren genau die, in denen ein
                    # Rechteck IM anderen steckt:
                    #     Blatt 7 (206,106): Ueberdeckung x=8, aber B ist selbst nur 8 breit
                    #     Blatt 7 (224,175): Ueberdeckung x=24, aber A ist selbst nur 24 breit
                    #     Blatt 2 (111, 84): Ueberdeckung x=17 gegen ein 24 breites B
                    # Dort gibt es keine gemeinsame Wandlinie, und die Ueberdeckung sagt
                    # nichts - genau die Einsicht, die der Kommentar oben schon als
                    # `_duenn` formuliert hatte, nur an einer festen Zahl (<= 4 px)
                    # statt an den Rechtecken gemessen.
                    # Der Nutzer-Fall ROOM1130 <-> ROOM1150 besteht diese Probe klar:
                    # Ueberdeckung x=8 bei zwei je 32 px breiten Rechtecken (8 < 16).
                    _wand = None
                    if _ho > _br and _br * 2 < min(_aw, _bw):
                        _wand = 'senk'
                    elif _br > _ho and _ho * 2 < min(_ah, _bh):
                        _wand = 'waag'
                    # ⛔ UND NUR BEI ZWEI VERSCHIEDENEN ZEICHNUNGEN. Teilen sich zwei Zonen
                    # DASSELBE gemalte Rechteck (21 Marken game-weit), ist die
                    # "Ueberdeckung" das ganze Rechteck und beschreibt keine Wand; dort
                    # standen 6 der 9 neuen Fehlfaelle.
                    if (_ax, _ay, _aw, _ah) == (_bx, _by, _bw, _bh):
                        _wand = None
                    if _wand == 'waag':
                        _seite_kante = 2 if (_by + _bh / 2.0) > (_ay + _ah / 2.0) else 0
                    elif _wand == 'senk':
                        _seite_kante = 1 if (_bx + _bw / 2.0) > (_ax + _aw / 2.0) else 3
                    else:
                        _seite_kante = None
                elif _br >= _ho:
                    # waagerechte Wand: Hoehe = Wanddicke, Laenge in x
                    cy = (_uy0 + _uy1) // 2
                    _mx = _laengs(2)
                    if _mx is not None:
                        cx = _mx
                    elif _rohA:
                        cx = _rohA[2]
                    elif _rohB:
                        cx = _rohB[2]
                    cx = min(max(cx, _ux0), max(_ux0, _ux1 - 1))
                    _seite_kante = 2 if (_by + _bh / 2.0) > (_ay + _ah / 2.0) else 0
                else:
                    cx = (_ux0 + _ux1) // 2
                    _my = _laengs(3)
                    if _my is not None:
                        cy = _my
                    elif _rohA:
                        cy = _rohA[3]
                    elif _rohB:
                        cy = _rohB[3]
                    cy = min(max(cy, _uy0), max(_uy0, _uy1 - 1))
                    _seite_kante = 1 if (_bx + _bw / 2.0) > (_ax + _aw / 2.0) else 3
        # Beide Datensaetze bekommen dieselbe Position UND dieselbe Wandseite, damit
        # daraus optisch EIN Symbol wird. Die Seite des ersten gewinnt (deterministisch);
        # weil sie auf derselben Wand sitzen, sind es ohnehin gespiegelte Nischen.
        # ⛔ DIE WANDSEITE MUSS ZUM NACHBARN ZEIGEN. Bisher kam sie aus der lokalen
        # Silhouette (auf welcher Seite der Raum weitergeht) - das benennt die Wand, in
        # der die Tuer sitzt, aber nicht, welche der beiden Seiten dieser Wand zum
        # Nachbarn schaut. Gemessen am 2026-09-02: 34 von 93 gepaarten Tuersymbolen
        # (37 %) zeigten VOM Nachbarn WEG, und die Nische ist gerichtet - der Nutzer sah
        # das als "Tueren oft falsch rotiert".
        # Bei einem Paar ist die Richtung bekannt: sie zeigt zur Zeichnung des anderen
        # Raums. Die WANDACHSE (senkrecht/waagerecht) bleibt, wie die Silhouette sie
        # bestimmt hat; nur das Vorzeichen wird korrigiert.
        # ⛔ DIE ACHSE IST EINE AUSSAGE DER AUSLIEFERUNGSDATEN, NICHT UNSERER ZUORDNUNG.
        # Sie kommt aus der Streckung des Aot_set-Trigger-Rechtecks (senk = rd > rw,
        # Rechteck-Semantik Ecke+Ausdehnung FUN_80042b64 @0x80042b68-7c) und wird von
        # snap_wall/snap_grundriss unveraendert weitergereicht. Die gemalten
        # Karten-Rechtecke sind UNSERE Zuordnung; sie duerfen nur das VORZEICHEN
        # bestimmen, nicht die Achse.
        #
        # ⛔ GEMESSEN GEGEN EINE UNABHAENGIGE WAHRHEIT, nicht gegen die eigene Regel.
        # Vergleichsmass sind die GEMALTEN Original-Tuersymbole
        # (analysis/kartensymbole/symbolkatalog.csv, Spalte `ausrichtung`), zugeordnet
        # ueber die Lage (<= 8 px auf demselben Blatt). Drei Varianten, gleicher Baum:
        #     vor allen Fixes                    18 richtig /  8 falsch
        #     Achse aus der Ueberdeckung         20 richtig /  7 falsch
        #     Achse aus dem Tuer-Rechteck (hier) 23 richtig /  4 falsch
        # Die Ueberdeckungs-Regel sah auf MEINER eigenen Schranke ("Achse passt zur Lage
        # der beiden Rechtecke") mit 26/0 besser aus als diese hier mit 20/6 - aber
        # genau diese Schranke misst dieselbe Ueberdeckung, aus der die Regel entsteht.
        # Eine selbstbestaetigende Metrik (Memory reai-v2-selbstbestaetigende-metrik);
        # entschieden hat der externe Massstab.
        _seite = W['seite']
        _andere = B if W is A else A
        _gk = _geo(_andere['pg'], _andere['r'], _andere['room'], _andere['zi'])
        if _gk:
            _zx = _gk[0] + _gk[2] / 2.0 - cx
            _zy = _gk[1] + _gk[3] / 2.0 - cy
            # ⛔ AUCH DIE ACHSE AUS DER NACHBARRICHTUNG. Ein erster Wurf behielt die
            # Achse der Silhouette und drehte nur das Vorzeichen - bei drei Symbolen lag
            # schon die ACHSE falsch (Seite 7: Symbol (156,136) als Sued-Wand, waehrend
            # der Nachbar 9 px WESTLICH liegt), und dann hilft kein Vorzeichen.
            # Bei einem PAAR ist die Richtung bekannt und schlaegt die Silhouette.
            # Die Achse bleibt, wie das Tuer-Rechteck sie sagt; nur das VORZEICHEN
            # kommt aus der Richtung zum Nachbarn.
            if W['seite'] % 2 == 1:
                _seite = 1 if _zx > 0 else 3      # senkrecht: Ost / West
            else:
                _seite = 2 if _zy > 0 else 0      # waagerecht: Sued / Nord
        # ⛔ DIE BERUEHRUNG SCHLAEGT DIE SCHWERPUNKT-RICHTUNG. Der Vergleich der beiden
        # Rechteck-Mitten sagt, wo der Nachbar UNGEFAEHR liegt; die Ueberdeckung sagt,
        # wo die Wand WIRKLICH verlaeuft. Bei zwei langgestreckten Raeumen, die sich
        # ueber Eck beruehren, zeigen die Mitten diagonal und die Achse wird zur
        # Muenzwurf-Entscheidung - genau die Fehlerklasse, die diesen Umbau ausgeloest
        # hat. Wo eine Ueberdeckung existiert, gilt sie.
        if _seite_kante is not None:
            # ⛔ DIE SEITE GEHOERT ZUM GEWINNER, NICHT ZU A. _seite_kante zeigt von A
            # nach B; die Marke traegt aber zid = W['zid'] und zid2 = die des anderen,
            # und die Nische muss zu zid2 zeigen. Ist W der Datensatz B, ist die
            # Richtung damit genau umgekehrt. Der Pin unit_map_durchgang hat das
            # gefunden (39 gepaarte Symbole zeigten vom Nachbarn weg) - er hatte recht,
            # nicht der erste Wurf dieses Umbaus.
            # Auch hier nur das Vorzeichen: die Ueberdeckung darf die Achse des
            # Tuer-Rechtecks nicht umwerfen (siehe die Messung oben).
            _sk = _seite_kante if W is A else (_seite_kante + 2) % 4
            if _sk % 2 == W['seite'] % 2:
                _seite = _sk
        for X in (A, B):
            X['mx'], X['my'], X['seite'] = cx, cy, _seite
        # EIN Datensatz genuegt. Beide zu behalten hiesse: dieselbe Stelle zweimal
        # zeichnen, jeder in der Farbe SEINES Rechtecks (einer gruen "besucht", einer
        # rot "aktueller Raum") - optisch wieder eine Doppelung. Der zweite wird
        # deshalb still gelegt, seine Zone aber als ZWEITE Sichtbarkeits-Zone im ersten
        # vermerkt: die Marke erscheint, sobald EINER der beiden Raeume besucht ist.
        W['zid2'] = (B if W is A else A)['zid']
        (B if W is A else A)['weg'] = True

    # ============ DURCHGANG 2b: KACHEL- UND SPIEGELWAND-FILTER ==================
    # Nutzer 2026-09-01: "In so ziemlich allen Faellen sind die Tueren auf beiden Seiten
    # eingezeichnet statt nur einmalig."
    #
    # GEMESSEN (Zensus ueber alle 302 Tuer-Datensaetze, 132 Durchgangs-Paare): Die
    # Doppelung ist NICHT zweimal Port-Marke - das kommt nur bei 2 Paaren vor. Sie ist
    # Port-Marke NEBEN GEMALTEM SYMBOL: 40 der 134 lebenden Marken sitzen <=4 px vor
    # einem Symbol, das die Kachel in einem NACHBAR-Rechteck malt, und 25 davon auf der
    # gegenueberliegenden Wandseite - also als zweite Nische an derselben Wandlinie.
    # kachel_zeigt_tuer sah dort nicht hin, weil es nur GLYPHEN[(pg, eigenes rect)]
    # prueft; die Rechtecke einer Seite ueberlappen sich aber auf dem Schirm.
    #
    # Auf 44 von 111 gezeichneten Durchgaengen (40 %) waren so >=2 Zeichnungen zu sehen,
    # auf den Blaettern 1/2/4 sogar 18 von 27 (67 %) - das sind die "so ziemlich alle
    # Faelle" des Reports.
    #
    # DAS ORIGINAL MALT EINEN DURCHGANG NIE ZWEIMAL: die Signatur "zwei Stempel auf
    # derselben Wandlinie mit gegenueberliegender Wandseite" kommt in 0 von 2775
    # Symbolpaaren vor. Der Nutzerwunsch stellt also Original-Verhalten wieder her.
    #
    # Regel (gemessen gegen vier Alternativen, siehe analysis/karte_0901/B_doppelte_tueren.md):
    #   * Kachel-Test ueber das EIGENE und das PARTNER-Rechteck,
    #   * plus SPIEGELWAND-Test: faellt weg, wenn auf derselben Seite ein gemaltes Symbol
    #     <=8 px entfernt liegt, dessen Wandseite die gegenueberliegende ist.
    # Restdoppelung danach: 0 (die Wandseite geht in den Kachel-Test nicht ein, ist also
    # eine unabhaengige Pruefgroesse). Ein blanker seitenweiter Test ohne Wandseite ist
    # VERWORFEN: er hatte am 2026-09-01 bereits alle vier ROOM1130-Tueren geschluckt.
    GEGENWAND = {0: 'S', 1: 'W', 2: 'N', 3: 'E'}      # Port-Seite 0=N 1=O 2=S 3=W

    def _spiegelsymbol(v):
        """Gemaltes Symbol <=8 px auf der GEGENUEBERLIEGENDEN Wand, irgendwo auf der Seite."""
        soll = GEGENWAND.get(v['seite'])
        if soll is None: return False
        for (gx, gy, gw, gh, wand) in GLYPHEN.get(v['pg'], ()):
            if wand != soll: continue
            if ((v['mx'] - gx) ** 2 + (v['my'] - gy) ** 2) ** 0.5 <= 8.0: return True
        return False

    partner = {}
    for _d, _i, _j in kand:
        if _i in belegt and _j in belegt:
            partner[id(tueren[_i])] = tueren[_j]
            partner[id(tueren[_j])] = tueren[_i]
    # ⛔ AUF DER ORIGINAL-KUNST DARF DIE GEMALTE TUER DIE RE2-MARKE NICHT VERDRAENGEN.
    # Diese Unterdrueckung stammt aus der Zeit der gerechneten Grundrisse: zeigt die
    # gemalte Kachel an dieser Stelle schon eine Tuer, waere eine zusaetzliche Marke
    # doppelt gemoppelt. Seit die KUNST der Auslieferungsstand ist, kehrt sich das um -
    # der Nutzer will ausdruecklich "Tuer und Treppenmarker von Resident Evil 2" AUF dem
    # Original-Material. Gemessen 2026-09-04 mit aktiver Unterdrueckung: von 184 Tueren
    # trugen nur 12 eine sichtbare Marke, 172 hatten gar keine.
    # Die gemalten RE1.5-Schwenke bleiben darunter stehen; sie zu entfernen ist ein
    # eigener Schritt (BEFUND §27 hat das Verfahren gemessen: die Lauflaenge trennt sie
    # von der Wand, 6/6 auf Blatt 4 ohne ein einziges Wandpixel).
    _RE2_TUEREN = os.environ.get('RE15_RE2_TUEREN', '1' if KUNST_VOR else '0') == '1'
    n_kachel = n_spiegel = n_partner = 0
    for v in vor:
        if v['kind'] != 0 or v.get('weg'): continue
        if _RE2_TUEREN:
            continue
        P = partner.get(id(v))
        raus = None
        if kachel_zeigt_tuer(v['pg'], v['r'], v['mx'], v['my']):
            raus = 'kachel'; n_kachel += 1
        elif P is not None and P['pg'] == v['pg'] and              kachel_zeigt_tuer(v['pg'], P['r'], v['mx'], v['my']):
            raus = 'partner'; n_partner += 1
        elif _spiegelsymbol(v):
            raus = 'spiegel'; n_spiegel += 1
        if raus:
            v['weg'] = True
            # Der Durchgang ist EINE Einheit: faellt ein Ende, faellt auch das andere.
            if P is not None: P['weg'] = True
    print(f"Tuermarken gefiltert: {n_kachel} eigene Kachel, {n_partner} Partner-Rechteck, "
          f"{n_spiegel} Spiegelwand")

    # ================= DURCHGANG 3: ausgeben ====================================
    # ---- MARKEN AUF DIE ZWEIT-ZEICHNUNG MITNEHMEN -----------------------------
    # Eine Zone, die auf mehreren Etagenblaettern gezeichnet ist (Etagen-Tabelle),
    # braucht ihre Marken auf JEDEM dieser Blaetter - sonst verschwinden Treppen und
    # Tueren, sobald die Karte auf die andere Etage schaltet. Die Kacheln sind
    # identisch (ROOM1170s zweiter Bereich: Seite 5 Rect 0 und Seite 4 Rect 3
    # unterscheiden sich in 22 von 1152 Pixeln, alle Tuersymbole), der Versatz ist
    # deshalb exakt die Differenz der Rechteck-Ecken.
    # ---- (ENTFALLEN) MARKEN AUF DIE ZWEIT-ZEICHNUNG MITNEHMEN -----------------
    # Bis v0.3.80 wurden hier alle Marken einer Zone auf jedes Etagenblatt kopiert, auf
    # dem die Zone gezeichnet ist. Das war die Ursache dafuer, dass ROOM1170s beide
    # 3F-Tueren auf dem ROOF-Blatt standen (Nutzer 2026-09-01). Seit die Marke ueber ihr
    # eigenes Band auf ihr Blatt geht (blatt_fuer_band), ist die Kopie nicht nur
    # ueberfluessig, sondern wuerde die Doppelung wieder herstellen.
    zusatz = []
    vor.extend(zusatz)

    # ---- ZWEI VERSCHIEDENE TUEREN DUERFEN NICHT AUFEINANDER LIEGEN ------------
    # ⛔ Nutzer 2026-09-02: "Tueren sind noch nicht sauber gesetzt."
    # Gemessen nach dem Umbau: 10 Markenpaare lagen hoechstens 2 px auseinander - einer
    # davon derselbe Durchgang zweimal (die Paarung hatte ihn nicht gefunden), NEUN
    # dagegen zwei VERSCHIEDENE Tueren. Seit die Raeume aneinanderstossen, liegen ihre
    # Projektionen dichter beieinander, und die Wandsuche zieht beide auf dasselbe
    # Randpixel; eine Tuer verdeckt dann die andere.
    # Sie werden LAENGS IHRER WAND auseinandergeschoben - quer waere die Marke von der
    # Wand weg, und dort gehoert sie nicht hin.
    _offen = [v for v in vor if not v.get('weg')]

    def _frei(kand, ausser, seite):
        """Ist diese Stelle weit genug von JEDER anderen Tuermarke des Blattes?"""
        for _o in _offen:
            if _o is ausser or _o['pg'] != seite or _o['seite'] >= 4:
                continue
            if abs(_o['mx'] - kand[0]) + abs(_o['my'] - kand[1]) <= 3:
                return False
        return True

    for _durchlauf in range(4):
        _bewegt = 0
        for _a in range(len(_offen)):
            for _b in range(_a + 1, len(_offen)):
                A2, B2 = _offen[_a], _offen[_b]
                if A2['pg'] != B2['pg']: continue
                # ⛔ NUR TUEREN. Ein erster Wurf schob auch Treppenmarken auseinander -
                # damit fielen Duplikate, die vorher an derselben Stelle zusammenfielen
                # und von der Schluessel-Entdopplung geschluckt wurden, wieder
                # auseinander und wurden ZWEIMAL gezeichnet (Treppensymbole 30 -> 52).
                if A2['seite'] >= 4 or B2['seite'] >= 4: continue
                if A2['room'] == B2['room'] and A2['zi'] == B2['zi'] and                    A2.get('zid2', 255) == B2.get('zid2', 255):
                    continue                      # dieselbe Tuer, absichtlich gleich
                if A2.get('zid2', 255) == B2['zid'] and B2.get('zid2', 255) == A2['zid']:
                    continue                      # derselbe Durchgang, absichtlich gleich
                if abs(A2['mx'] - B2['mx']) + abs(A2['my'] - B2['my']) > 3: continue
                # ⛔ LAENGS DER WAND schieben - quer waere die Marke von der Wand weg.
                # Und die neue Stelle muss (a) noch auf der Zeichnung liegen und (b) von
                # JEDER anderen Marke weit genug weg sein. Ohne (b) schob ein erster Wurf
                # die eine Marke nur auf die naechste (5 Paare blieben stehen).
                _erledigt = False
                for _wer in (B2, A2):
                    _laengs_y = _wer['seite'] in (1, 3)
                    for _schub in (3, -3, 5, -5, 7, -7, 9, -9, 12, -12):
                        if _laengs_y:
                            _nx, _ny = _wer['mx'], _wer['my'] + _schub
                        else:
                            _nx, _ny = _wer['mx'] + _schub, _wer['my']
                        if not _gr_zelle(_wer['room'], _wer['zi'], _wer['pg'], _nx, _ny):
                            continue
                        if not _frei((_nx, _ny), _wer, _wer['pg']):
                            continue
                        # ⛔ EIN GEPAARTES SYMBOL BLEIBT AUF DER GEMEINSAMEN KANTE.
                        # Es sitzt auf der Ueberdeckung der beiden Rechtecke; schiebt man
                        # es darueber hinaus, steht es neben der Stelle, an der die zwei
                        # Raeume sich beruehren (Audit E: ROOM2040 <-> ROOM2050 und
                        # ROOM3090 <-> ROOM30D0, je 3 px daneben).
                        _z2 = _wer.get('zid2', 255)
                        if _z2 != 255:
                            _ka = _geo(_wer['pg'], _wer['r'], _wer['room'], _wer['zi'])
                            _kb = None
                            for _q in vor:
                                if _q.get('zid') == _z2 and _q['pg'] == _wer['pg']:
                                    _kb = _geo(_q['pg'], _q['r'], _q['room'], _q['zi'])
                                    break
                            if _ka and _kb:
                                _ax0 = max(_ka[0], _kb[0])
                                _ax1 = min(_ka[0] + _ka[2], _kb[0] + _kb[2])
                                _ay0 = max(_ka[1], _kb[1])
                                _ay1 = min(_ka[1] + _ka[3], _kb[1] + _kb[3])
                                if _ax1 > _ax0 and _ay1 > _ay0:
                                    if not (_ax0 <= _nx < _ax1 and _ay0 <= _ny < _ay1):
                                        continue
                        _wer['mx'], _wer['my'] = _nx, _ny
                        _bewegt += 1
                        _erledigt = True
                        break
                    if _erledigt:
                        break
        if not _bewegt:
            break

    # ⛔ EINE DUBLETTE DARF IHRE SICHTBARKEITS-ZONE NICHT MITNEHMEN.
    # Zwei Raeume koennen an derselben Stelle dieselbe Treppe/Tuer eintragen (ROOM30C0
    # und ROOM30D0 beide bei (160,112) auf Blatt 7). Ein erster Wurf behielt schlicht die
    # erste und warf die zweite weg - das Symbol war dann da, haengte aber am falschen
    # Raum: es erschien, sobald ROOM30C0 besucht war, und blieb aus, wenn der Spieler nur
    # ROOM30D0 kannte. Das Audit meldete "ROOM30D0 hat Treppen, aber kein Symbol".
    # Richtig ist dasselbe wie bei gepaarten Tueren: EINE Marke, ZWEI Zonen.
    # ⛔ ZWEI TUEREN AUF EINEM PIXEL SIND EIN SYMBOL, EGAL MIT WELCHER ACHSE.
    # Bis 2026-09-04 stand die SEITE im Dubletten-Schluessel. Zwei Marken am selben
    # Punkt mit verschiedener Achse ueberlebten damit beide und wurden als KREUZ aus
    # einem waagerechten und einem senkrechten Balken gezeichnet. Gemessen an der
    # erzeugten Tabelle: Blatt 8 rect 2 (142,83) trug schon vorher zwei Tueren
    # DESSELBEN Ortes (zid 67, einmal Nord, einmal West) uebereinander.
    # Der Schluessel ist deshalb die POSITION; Tueren und Treppen bleiben getrennt,
    # weil eine Treppe nur drei Sprossen ohne deckenden Grund zeichnet und die
    # darunterliegende gemalte Tuer absichtlich durchscheinen laesst (siehe den
    # Kommentar zur Treppen-Marke in re15_inv_screen.c).
    # ⛔ DIE GEPAARTE AUSSAGE GEWINNT. Welche Achse ueberlebt, ist keine Geschmacks-
    # frage: eine GEPAARTE Marke kennt beide Rechtecke und damit die gemeinsame Wand,
    # eine ungepaarte hat ihre Achse nur aus der Silhouette ihres eigenen Rechtecks.
    # Ohne diese Regel entschiede die Reihenfolge - also der Muenzwurf, den dieser
    # Umbau gerade abschafft.
    # ⛔ EINE MARKE, DEREN LAGE NUR DER ANSCHLAG IST, IST KEINE MARKE.
    # NUTZER-BEFUND 2026-09-04 (fehler/error1.png, "wrong shouldn't exist"): auf Blatt 4
    # stand ein Tuersymbol in der Nordost-ECKE von ROOM1120s Rechteck, direkt an
    # ROOM1140s Rechteck - es las sich als Tuer zwischen zwei Raeumen, die keine haben.
    #
    # URSACHE, NACHGERECHNET: to_map KLEMMT die Projektion in das gemalte Rechteck.
    # ROOM1120 (Rect 5 = x120..159, y119..158, Eichung 101/190/2296/2312) projiziert
    # seine drei Tueren so:
    #     #2 -> ROOM1130 (-8450,-2900)  roh (153,126)  IM Rechteck        -> echte Marke
    #     #0 -> ROOM1060  (-700, 9550)  roh (170, 98)  ausserhalb x UND y -> Ecke (159,119)
    #     #1 -> ROOM1080  ( 1300, 6400) roh (174,105)  ausserhalb x UND y -> Ecke (159,119)
    # Die ROOM1060-Tuer rettet die PAARUNG: sie zieht die Marke auf die gemeinsame Kante
    # mit ROOM1060s Rechteck (120,151). Die ROOM1080-Tuer hat keine Gegenseite - ROOM1080
    # traegt NULL Tuer-Datensaetze - und blieb auf dem Klemmwert stehen.
    #
    # ⛔ DIE REGEL IST NICHT "UNGEPAART = WEG". 98 von 201 Marken sind ungepaart (u.a.
    # ROOM1160s Tuer auf demselben Blatt, die der Nutzer NICHT beanstandet hat); dieser
    # Filter loeschte die halbe Karte. Verworfen wird nur, was BEIDES ist: ohne Partner
    # UND diagonal ausserhalb geklemmt. Eine Klemmung in nur EINER Achse bleibt eine
    # Aussage - die andere Koordinate ist gemessen, die Tuer sitzt auf dieser Wand.
    _weg_klemm = [v for v in vor
                  if not v.get('weg') and v['kind'] == 0
                  and v.get('zid2', 255) == 255 and v.get('klemm_xy')]
    for v in _weg_klemm:
        v['weg'] = True
    print("   %d ungepaarte Tuermarken verworfen (Lage war nur der Klemm-Anschlag): %s"
          % (len(_weg_klemm),
             ', '.join('ROOM%04X#%d@Blatt%d(%d,%d)' % (v['room'], v['idx'], v['pg'],
                                                       v['mx'], v['my'])
                       for v in _weg_klemm) or '-'))

    seen = {}
    for v in vor:
        if v.get('weg'): continue
        key = (v['pg'], v['r'], v['mx'], v['my'], v['seite'] > 3)
        if key in seen:
            erste = seen[key]
            _erste_gepaart = erste.get('zid2', 255) != 255
            if (v['zid'] != erste['zid'] and not _erste_gepaart):
                erste['zid2'] = v['zid']
            if not _erste_gepaart and v.get('zid2', 255) != 255:
                erste['seite'] = v['seite']
            continue
        seen[key] = v
        marks.append(v)
    marks = [(v['pg'], v['r'], v['mx'], v['my'], v['seite'], v['zid'],
              v.get('zid2', 255)) for v in marks]

    o.append("")
    o.append("/* MARKEN in Karten-Pixeln. kind = TUER mit WANDSEITE 0=Nord 1=Ost")
    o.append(" * 2=Sued 3=West (die Nische zeigt nach innen), 4/5 = Treppe mit")
    o.append(" * waagerechten bzw. senkrechten Sprossen.")
    o.append(" * Treppen stammen aus den SCD-Zonen Aot_set Typ 12/13 (die Band-Wechsel-")
    o.append(" * Zonen), Tueren aus den Tuer-Datensaetzen. Gezeichnet werden sie nur fuer")
    o.append(" * Zonen, die der Spieler schon gesehen hat.")
    o.append(" * auf_partner = 1: die Marke liegt auf der GEMALTEN Flaeche des Rechtecks")
    o.append(" * der zweiten Zone (zid2). Nur dann darf sie schon sichtbar sein, wenn")
    o.append(" * bloss die zweite Zone besucht ist - sonst schwebt sie im Leeren.")
    o.append(" * ACHTUNG - der KASTEN reicht dafuer nicht (Nutzer-Befund 2026-09-05,")
    o.append(" * \"This door is flying\"): er ist nur die Bounding-Box, die Kachel darin")
    o.append(" * enthaelt Schwarz. Blatt 3 Marke (141,117) liegt in Rect 0 (102,116)")
    o.append(" * 40x40, dessen Kachel traegt dort aber Index 0. Gemessen ueber alle")
    o.append(" * Marken mit Partner-Rechteck: 47 liegen auf dessen Kachel, 23 nicht. */")
    o.append("typedef struct { unsigned char page, rect; short mx, my;")
    o.append("                 unsigned char kind, zid, zid2, auf_partner;")
    o.append("               } re15_map_mark_t;")
    o.append("static const re15_map_mark_t s_map_marks[] = {")
    for pg, r, mx, my, kind, zd, zd2 in sorted(marks):
        _ap = 0
        if zd2 != 255 and r != 255:
            _pr = None
            for _z in rows:
                if _z[5] == zd2 and _z[2] == pg:
                    _pr = _z[3]
                    break
            if _pr is not None and _pr != 255:
                _R = rects(pg)
                _px = page_pix(pg)
                if _px is not None and _pr < len(_R):
                    _rx, _ry, _rw, _rh = _R[_pr]
                    _u, _v = rect_uv(pg, _pr)
                    for _b in (-1, 0, 1):
                        for _a in (-1, 0, 1):
                            _sx, _sy = mx + _a, my + _b
                            if not (_rx <= _sx < _rx + _rw and _ry <= _sy < _ry + _rh):
                                continue
                            _tx, _ty = _u + (_sx - _rx), _v + (_sy - _ry)
                            if 0 <= _tx < 256 and 0 <= _ty < 256 and _px[_ty][_tx]:
                                _ap = 1
        o.append(f"    {{ {pg:2d}, {r:2d}, {mx:4d}, {my:4d}, {kind}, {zd:3d}, {zd2:3d},"
                 f" {_ap} }},")
    o.append("};")
    o.append("")
    o.append("/* ETAGEN: Band -> (Kartenseite, Rechteck). Aus den Tueren des Raums")
    o.append(" * abgeleitet (Band der Tuer -> Seite des Zielraums), Ziel-Rechteck ueber")
    o.append(" * die gleiche Kachel-uv gefunden. Siehe tools/gen_map_zones.py. */")
    o.append("typedef struct { unsigned short room; unsigned char zone, band, page, rect; } re15_map_floor_t;")
    o.append("static const re15_map_floor_t s_map_floors[] = {")
    for room, zi, band, pg, r in sorted(floors):
        for var in (0, 1):
            o.append(f"    {{ 0x{room + var:04X}, {zi}, {band:2d}, {pg:2d}, {r:2d} }},")
    o.append("};")
    print(f"{len(floors)} Etagen-Eintraege")

    o.append("")
    o.append("/* SCHEMA-ZEICHNUNGEN aus der KOLLISIONS-BOX. Fuer Zonen, denen kein")
    o.append(" * Karten-Rechteck des Originals zugeordnet werden konnte (rect == 255 in")
    o.append(" * s_map_zones). Massstab = Median der ausgelieferten Zeilen @0x800768b0")
    o.append(" * derselben Stage, Lage ueber eine Tuer zu einem gezeichneten Nachbarn.")
    o.append(" * ACHTUNG PORT-ERGAENZUNG: das Original zeichnet diese Raeume gar nicht,")
    o.append(" * und seine Kunst ist auch nicht massstabsgetreu zur Kollision. Siehe")
    o.append(" * tools/gen_map_zones.py, Abschnitt SCHEMA-ZEICHNUNG. */")
    o.append("/* Die AFFINE ABBILDUNG bildet Weltkoordinaten auf Kartenpixel ab:")
    o.append(" *     mx = (A*wx + B*wz) / 65536 + C ,  my = (D*wx + E*wz) / 65536 + F")
    o.append(" * Drehung und Spiegelung des Raums stecken in den Vorzeichen, der")
    o.append(" * Massstab im Betrag. A==0 && D==0 heisst: keine Abbildung (Rueckfall auf")
    o.append(" * die Bbox-Streckung in den Kasten). */")
    o.append("typedef struct { short x, y, w, h; unsigned short erste, n;")
    o.append("                 int a, b, c, d, e, f; } re15_map_synth_t;")
    o.append("static const re15_map_synth_t s_map_synth[] = {")
    for (x, y, w, h, e, n, ab) in synth_liste:
        o.append(f"    {{ {x:4d}, {y:4d}, {w:4d}, {h:4d}, {e:4d}, {n:3d},"
                 f" {ab[0]:7d}, {ab[1]:7d}, {ab[2]:4d}, {ab[3]:7d}, {ab[4]:7d}, {ab[5]:4d} }},")
    o.append("};")
    o.append("typedef struct { short x, y, w, h; } re15_map_synth_cell_t;")
    o.append("static const re15_map_synth_cell_t s_map_synth_cells[] = {")
    for (x, y, w, h) in synth_zellen:
        o.append(f"    {{ {x:4d}, {y:4d}, {w:4d}, {h:4d} }},")
    o.append("};")
    print(f"{len(synth_liste)} Schema-Zeichnungen mit {len(synth_zellen)} Zellen")

    o.append("/* ACHTUNG - ERSATZ-SEITENTABELLE. Das Original fuehrt fuer Blatt 3 (2F)")
    o.append(" * die Tabelle von Blatt 2 (1F): verschiedene Adressen (@0x8007636C /")
    o.append(" * @0x800763F0), byte-identische Eintraege, Blatt 3 nur ohne den elften.")
    o.append(" * Gemessen: baut man eine Seite nach ihrer Tabelle zusammen, kommt der")
    o.append(" * fertige Grundriss heraus, den dasselbe Blatt in seiner unteren Haelfte")
    o.append(" * traegt - Blatt 2 zu 93,0 %, Blatt 4 zu 98,7 %, Blatt 3 nur zu 57,3 %.")
    o.append(" * Die Zeilen hier sind aus den KACHELN DES 2F-BLATTS hergeleitet (Ecke +")
    o.append(" * Groesse auf 8 aufgerundet, Lage per Schablonensuche im Grundriss des")
    o.append(" * Blatts, 8/10 zu 100,0 %); Zusammenbau 93,1 %. Herleitung und Belege:")
    o.append(" * tools/gen_map_zones.py (RECT_FIX) und analysis/karte_grundriss/BEFUND.md.")
    o.append(" * page == 255 beendet die Liste. */")
    o.append("typedef struct { unsigned char page, u, v; short x, y, w, h; }"
             " re15_map_rectfix_t;")
    o.append("static const re15_map_rectfix_t s_map_rectfix[] = {")
    for _pg in sorted(RECT_FIX):
        for (_x, _y, _w, _h, _u, _v) in RECT_FIX[_pg]:
            o.append(f"    {{ {_pg:3d}, {_u:3d}, {_v:3d}, {_x:4d}, {_y:4d},"
                     f" {_w:3d}, {_h:3d} }},")
    o.append("};")
    print(f"{sum(len(v) for v in RECT_FIX.values())} Ersatz-Rechtecke "
           f"fuer Blatt {sorted(RECT_FIX)}")

    dst = os.path.join(ROOT, 're15_port', 'engine', 'src', 're15_map_zones.h')
    _txt = chr(10).join(o) + chr(10)
    _bad = [(_i, _c) for _i, _c in enumerate(_txt) if ord(_c) > 127]
    if _bad:
        _i = _bad[0][0]
        raise SystemExit("NICHT-ASCII im Kopf an Position %d: %r" %(_i,_bad[0][1])+'\n'+"Kontext: %r"%(_txt[max(0,_i-140):_i+60],))

    # Kommentar-Bilanz. Ein zu frueh geschlossenes /* ... */ macht den Rest des
    # Absatzes zu Code - der Compiler meldet dann Unsinn weit hinter der Ursache
    # ("invalid digit '9' in octal constant" fuer eine Masszahl im Fliesstext).
    # Dreimal passiert (2026-09-05), deshalb hier eine Bilanz VOR dem Schreiben.
    _tiefe, _auf = 0, 0
    for _ln, _zeile in enumerate(o, 1):
        # ⛔ Die Tiefe am ZEILENANFANG entscheidet, nicht die danach: die
        # Schlusszeile eines Absatzes ("... md */") faengt mit "* " an UND schliesst
        # den Kommentar. Wer erst abtastet und dann prueft, meldet genau sie
        # faelschlich an - einmal passiert, direkt beim ersten Lauf.
        if _tiefe == 0 and _zeile.lstrip().startswith('* '):
            raise SystemExit(
                'KOMMENTAR-BILANZ Zeile %d: Fortsetzungszeile ausserhalb eines '
                'Kommentars (das /* wurde vorher geschlossen)%s  %r'
                % (_ln, chr(10), _zeile))
        _j = 0
        while _j < len(_zeile) - 1:
            if _tiefe == 0 and _zeile[_j:_j+2] == '/*':
                _tiefe, _auf, _j = 1, _ln, _j + 2
            elif _tiefe == 1 and _zeile[_j:_j+2] == '*/':
                _tiefe, _j = 0, _j + 2
            else:
                _j += 1
    if _tiefe:
        raise SystemExit('KOMMENTAR-BILANZ: /* aus Zeile %d nie geschlossen' % _auf)

    open(dst, 'w', encoding='ascii', newline=chr(10)).write(_txt)
    print(f"\n{len(rows)} Zonen-Eintraege ({len(rows)//2} Zonen x 2 Varianten) -> {dst}")

if __name__ == '__main__':
    main()
