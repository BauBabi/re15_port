"""Generator: aus einem vorhandenen Vordergrundatlas die fehlende Maskengeometrie erzeugen.

Zielmenge sind die 238 Cuts, in denen der Prototyp die Vordergrund-GRAFIK enthaelt,
aber keine Maskenrecords (die Kuenstler kamen nicht mehr dazu). Ausgabe ist exakt das
Original-Format (Gruppen + Rechtecke + depth), damit PC und PSX denselben Zeichner
benutzen koennen.

TRICK FUER DIE GRUPPENANKER: das Original rechnet Bildposition = Gruppenanker (s16)
+ Masken-Byte (u8). Da jede zurueckgewonnene Verschiebung konstant ist, wird je
Verschiebung EINE Gruppe angelegt mit Anker = Verschiebung und Masken-Byte = src.
Dann ist dst = src + Verschiebung, also genau richtig, und das u8-Feld reicht immer.

WARUM GROSSZUEGIGE RECHTECKE UNGEFAEHRLICH SIND: Rechtecke ueberdecken die
unregelmaessige Silhouette zwangslaeufig mit; auch die Kuenstler haben das getan
(Obergrenze IoU Silhouette-gegen-Rechteckvereinigung = 0.826). Die Feinmaskierung
macht die TRANSPARENZ: Palettenindex 0 wird nicht gezeichnet. Ueberdeckte Pixel
ausserhalb der Silhouette sind im Atlas transparent und damit wirkungslos.

TIEFE: Bodenkontakt je zusammenhaengendem Objekt.

⛔ DER SICHERHEITSFAKTOR WAR EINE ERFUNDENE ZAHL. Erst stand hier 1.10 "konservativ
nach hinten". Die Sonde RE15_POCC_SCAN hat gemessen, was das kostet: in ROOM3040
Cut 0 liegen alle 12 erreichbaren Punkte hinter der Maske bei vz 8882..9457, die
Schwelle aber bei 9984 — die Maske verdeckte NIE, der ganze Nutzen war weg.
Der Faktor ist jetzt an den 480 Kuenstler-Cuts kalibriert (q8_faktor.py):

  Faktor   Uebereinstimmung   falsch_verdeckt   falsch_gezeigt
   0.90        77.9 %             7.9 %            14.1 %
   1.00        77.1 %             5.2 %            17.7 %
   1.10        74.5 %             3.5 %            22.0 %

Gewaehlt: 1.00 — die Schaetzung wird so benutzt, wie sie gemessen ist. Jede
Abweichung davon waere selbst wieder eine geratene Zahl. "falsch_verdeckt" (die
Figur verschwindet, obwohl sie davor steht) ist der sichtbar schlimmere Fehler,
"falsch_gezeigt" entspricht dem heutigen Zustand.

Bekannte Ursache der Restabweichung: die zurueckgewonnene Region deckt rund die
Haelfte der Silhouette ab und verfehlt haeufig den unteren, kontrastarmen Teil des
Objekts — dadurch liegt der geschaetzte Bodenkontakt zu HOCH und damit zu weit.
Bessere Abdeckung verbessert also direkt die Tiefe.
"""
import os, sys, struct
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb
from q3b_tiefe import build_view, vz_at_floor
from gen_region import find_blocks

MAX_MASKS_PER_CUT = 105        # spielweites Maximum (RDT-Header Byte[7], ROOM3000/3001 Cut 3)
NCC_MIN           = 0.97       # gemessen: 99.7 % der so markierten Pixel sind richtig
DEPTH_SAFETY      = 1.00       # KALIBRIERT, nicht geraten — s. q8_faktor.py

def _largest_rect(mask):
    """Groesstes achsenparalleles Vollrechteck in einer Boolmaske -> (y,x,h,w,flaeche)."""
    h, w = mask.shape
    height = np.zeros(w, np.int32)
    best = (0,0,0,0,0)
    for y in range(h):
        row = mask[y]
        height = np.where(row, height+1, 0)
        stack = []
        for x in range(w+1):
            cur = height[x] if x < w else 0
            start = x
            while stack and stack[-1][1] >= cur:
                sx, sh = stack.pop()
                area = sh*(x-sx)
                if area > best[4]: best = (y-sh+1, sx, sh, x-sx, area)
                start = sx
            stack.append((start, cur))
    return best

def rects_from_mask(mask, budget):
    """Region grob mit wenigen Rechtecken ueberdecken (Transparenz maskiert fein nach)."""
    m = mask.copy(); out = []
    while len(out) < budget:
        y, x, hh, ww, area = _largest_rect(m)
        if area < 16: break                     # Kleinkram lohnt kein eigenes Rechteck
        out.append((x, y, ww, hh))
        m[y:y+hh, x:x+ww] = False
        if not m.any(): break
    return out

def depth_map_for_cut(d, cam, cut, screen_regions):
    """-> Funktion (x,y) -> depth, aus dem Bodenkontakt je zusammenhaengendem Objekt."""
    rec = cam + cut*32
    fov = struct.unpack_from("<H", d, rec+2)[0]
    P = struct.unpack_from("<iii", d, rec+4); T = struct.unpack_from("<iii", d, rec+0x10)
    v = build_view(fov, *P, *T)
    if not v: return None
    R, t, H = v
    if H <= 0: return None
    lab, n = ndimage.label(screen_regions, structure=np.ones((3,3)))
    dep = np.zeros((240,320), np.int32)
    for k in range(1, n+1):
        m = (lab == k)
        cols = np.where(m.any(0))[0]
        if len(cols) == 0: continue
        vzs = []
        for x in cols:
            yb = int(np.where(m[:, x])[0].max())
            if yb >= 239: continue              # laeuft aus dem Bild -> kein Bodenkontakt
            z = vz_at_floor(R, t, H, x+0.5, float(yb), 0)
            if z: vzs.append(z)
        if not vzs:
            z = 4096.0*16                        # kein Kontakt sichtbar -> sehr weit = wirkungslos
        else:
            z = float(np.median(vzs))
        dep[m] = max(1, min(1023, int(z*DEPTH_SAFETY/64.0)))
    return dep

def build_cut(d, cam, stage, rid, cut):
    """-> (groups, masks) im Original-Layout, oder None."""
    A = atlas_rgb(stage, rid, cut); B = bg_rgb(stage, rid, cut)
    if A is None or B is None: return None
    at, idx = A
    lum = lambda a: a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114
    opaque = idx != 0
    blocks = find_blocks(lum(at), opaque, lum(B), block=16, stride=4, min_ncc=NCC_MIN)
    if not blocks: return None
    ah, aw = idx.shape
    best = np.full((ah,aw), -1.0); sxm = np.zeros((ah,aw),np.int32); sym = np.zeros((ah,aw),np.int32)
    for ax, ay, dx, dy, v in blocks:
        sl = (slice(ay,ay+16), slice(ax,ax+16))
        upd = best[sl] < v
        best[sl] = np.where(upd, v, best[sl]); sxm[sl] = np.where(upd, dx, sxm[sl]); sym[sl] = np.where(upd, dy, sym[sl])
    have = opaque & (best >= 0)
    if have.sum() < 64: return None
    # Bildschirmregion (fuer die Tiefe) + Regionen je Verschiebung (fuer die Rechtecke)
    screen = np.zeros((240,320), bool)
    ys, xs = np.nonzero(have); by = ys+sym[ys,xs]; bx = xs+sxm[ys,xs]
    ok = (by>=0)&(by<240)&(bx>=0)&(bx<320)
    screen[by[ok], bx[ok]] = True
    dep = depth_map_for_cut(d, cam, cut, screen)
    if dep is None: return None
    shifts = {}
    for (y,x,dx,dy) in zip(ys[ok],xs[ok],sxm[ys,xs][ok],sym[ys,xs][ok]):
        shifts.setdefault((int(dx),int(dy)), []).append((y,x))
    groups, masks = [], []
    budget = MAX_MASKS_PER_CUT
    for (dx,dy), px in sorted(shifts.items(), key=lambda kv: -len(kv[1])):
        if budget <= 0: break
        m = np.zeros((ah,aw), bool)
        yy = np.array([p[0] for p in px]); xx = np.array([p[1] for p in px])
        m[yy,xx] = True
        rs = rects_from_mask(m, min(budget, max(4, budget//max(1,len(shifts)))))
        if not rs: continue
        gm = []
        for (x,y,w,h) in rs:
            sxp, syp = x+dx, y+dy
            if sxp < 0 or syp < 0 or sxp+w > 320 or syp+h > 240: continue
            dv = int(np.median(dep[syp:syp+h, sxp:sxp+w][dep[syp:syp+h, sxp:sxp+w] > 0])) \
                 if (dep[syp:syp+h, sxp:sxp+w] > 0).any() else 0
            if dv <= 0: continue
            gm.append((x, y, w, h, dv))
        if not gm: continue
        groups.append((len(gm), dx, dy)); masks.extend(gm); budget -= len(gm)
    if not masks: return None
    return groups, masks

def pack_section(groups, masks):
    """Bytes im Original-Layout: Header(4) + Gruppen(8) + Masken(8 bzw. 12)."""
    out = bytearray()
    out += struct.pack("<HH", len(groups), len(masks) & 0xFFFF)
    for (n, dx, dy) in groups:
        out += struct.pack("<HHhh", n, 0x7800, dx, dy)     # +2 wird vom Original nie gelesen
    for (x, y, w, h, dep) in masks:
        out += struct.pack("<BBBBHH", x & 0xFF, y & 0xFF, x & 0xFF, y & 0xFF, dep, 0)
        out += struct.pack("<HH", w, h)                     # size-Feld high-nibble 0 = rechteckig
    return bytes(out)
