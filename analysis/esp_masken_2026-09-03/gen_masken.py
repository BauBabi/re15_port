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

Dieser erste Wert war gegen die KUENSTLER-Regionen kalibriert. Der Generator
benutzt aber seine EIGENEN, unvollstaendigen Regionen; end-zu-ende gemessen
(q12_faktor_schnell.py, 209 Cuts, 1.92 Mio gemeinsame Pixel):

  Faktor   Uebereinstimmung   falsch_verdeckt   falsch_gezeigt
   0.70        75.3 %            10.8 %            13.9 %
   0.75        75.4 %             8.8 %            15.7 %
   0.80        75.2 %             7.1 %            17.6 %
   0.90        73.7 %             4.6 %            21.8 %
   1.00        70.7 %             3.0 %            26.3 %

Gewaehlt: 0.90. Die hoechste Uebereinstimmung liegt bei 0.75/0.80 — danach haette
ich zuerst gegriffen. Das widerspricht aber dem Grundsatz, nach dem dieser ganze
Zusatz gebaut ist: eine FEHLENDE Maske laesst alles wie heute, eine FALSCHE malt
Hintergrund ueber die Figur und ist sichtbar schlimmer. Wer danach entscheidet,
gewichtet "falsch_verdeckt" hoeher als die Trefferquote — und dann ist 0.90 die
Zeile (4.6 % statt 7.1 %, fuer 1.5 Punkte weniger Uebereinstimmung).
Der gemessene Versatz betrug 1.32; das Optimum bei etwa 1/1.32 bestaetigt, dass die
Groessenordnung aus der Messung faellt und nicht aus dem Bauch.

⛔ NICHT als Beleg tauglich: die Sonde RE15_POCC_SCAN. Sie zaehlt Bodenpunkte, deren
Projektion IN einem Maskenrechteck liegt — Punkte VOR dem Objekt projizieren unter
das Rechteck und kommen gar nicht vor. Sie erkennt also zu WEITE Tiefen (Faktor 1.10
ergab 0 von 12), ist gegen zu NAHE aber blind. Die Kalibrierung muss aus der
Gegenueberstellung mit den Kuenstlerschwellen kommen, nicht aus dieser Zaehlung.

Ursache des Versatzes: die zurueckgewonnene Region deckt rund die Haelfte der
Silhouette ab und verfehlt haeufig den unteren, kontrastarmen Teil des Objekts —
dadurch liegt der geschaetzte Bodenkontakt zu HOCH und damit zu weit. Bessere
Abdeckung verbessert also direkt die Tiefe (Wachstum: 43.9 % -> 74.3 %).
"""
import os, sys, struct
import numpy as np
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb
from q3b_tiefe import build_view, vz_at_floor
from gen_region import find_blocks, region_grow

MAX_MASKS_PER_CUT = 105        # spielweites Maximum (RDT-Header Byte[7], ROOM3000/3001 Cut 3)
NCC_MIN           = 0.97       # gemessen: 99.7 % der so markierten Pixel sind richtig
DEPTH_SAFETY      = 0.90       # END-ZU-ENDE kalibriert — s. q12_faktor_schnell.py

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
    """-> Tiefenkarte (240x320), 0 = kein Vordergrund.

    ⛔ ERST stand hier EIN Wert je zusammenhaengendem Objekt (Median ueber alle
    Spalten). Die Abnahmepruefung gegen die Kuenstlerdaten (q9) zeigte damit nur
    67.5 % richtige Verdeckungsentscheidungen. Zwei Gruende:
      - Ein langer Tisch, der in die Tiefe laeuft, hat KEINE einheitliche Tiefe;
        die Kuenstler setzten pro Kachel eigene Werte (Streuung in einer Gruppe
        im Median 24.7 %).
      - Bessere Abdeckung laesst benachbarte Objekte zu EINER Komponente
        verschmelzen; dann bekam das ferne Objekt den Bodenkontakt des nahen.
        Sichtbar geworden in ROOM1220: Flaeche stieg, Verdeckung fiel von 63/76
        auf 30/97.
    Jetzt: Tiefe JE BILDSPALTE aus dem Bodenkontakt der Komponente in genau dieser
    Spalte. Das haelt die Tiefenstaffelung innerhalb eines Objekts und macht die
    Verschmelzung zweier Objekte weitgehend harmlos, weil jede Spalte ihren eigenen
    Kontaktpunkt benutzt.
    """
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
        for x in cols:
            rows = np.where(m[:, x])[0]
            yb = int(rows.max())
            # ⛔ FALSCH HERUM GEDACHT (gemessen, q10): laeuft die Silhouette in dieser
            # Spalte unten aus dem Bild, hatte ich sie als "kein Bodenkontakt sichtbar
            # -> sehr weit -> wirkungslos" behandelt. Genau umgekehrt: ein Objekt, dessen
            # Umriss den unteren Bildrand erreicht, steht VOR allem, was in dieser Spalte
            # erreichbar ist — sein Bodenkontakt liegt UNTERHALB des Bildes, also noch
            # naeher als der unterste sichtbare Bodenpunkt. Der Fehler erzeugte den
            # Ausläufer im Tiefenverhaeltnis (90 %-Quantil 12.3 = die Klemmung auf 1023)
            # und machte betroffene Masken vollstaendig wirkungslos.
            yq = float(yb) if yb < 239 else 239.0
            zz = vz_at_floor(R, t, H, x+0.5, yq, 0)
            if not zz: continue
            z = zz
            dep[rows, x] = max(1, min(1023, int(z*DEPTH_SAFETY/64.0)))
    return dep


def build_cut(d, cam, stage, rid, cut):
    """-> (groups, masks) im Original-Layout, oder None.

    Zwei Stufen: die Blockabstimmung findet die Verschiebungen (teuer, aber
    zuverlaessig), das Wachstum fuellt die duennen Strukturen auf, die nie ein
    vollstaendig undurchsichtiges 16x16-Fenster haben. Gemessen ueber 217 Cuts mit
    bekannter Wahrheit: Abdeckung 43.9 % -> 74.3 %, Praezision bleibt 99.39 %.
    Die bessere Abdeckung verbessert zugleich die TIEFE, weil der Bodenkontakt nicht
    mehr am fehlenden unteren Objektrand vorbei geschaetzt wird.
    """
    A = atlas_rgb(stage, rid, cut); B = bg_rgb(stage, rid, cut)
    if A is None or B is None: return None
    at, idx = A
    lum = lambda a: a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114
    opaque = idx != 0
    al, bl_ = lum(at), lum(B)
    blocks = find_blocks(al, opaque, bl_, block=16, stride=4, min_ncc=NCC_MIN)
    if not blocks: return None
    grown = region_grow(al, opaque, bl_, blocks, block=16)
    if not grown: return None

    ah, aw = idx.shape
    screen = np.zeros((240,320), bool)
    for (dx, dy), m in grown.items():
        ys, xs = np.nonzero(m); by = ys+dy; bx = xs+dx
        ok = (by>=0)&(by<240)&(bx>=0)&(bx<320)
        screen[by[ok], bx[ok]] = True
    if screen.sum() < 64: return None
    dep = depth_map_for_cut(d, cam, cut, screen)
    if dep is None: return None

    groups, masks = [], []
    budget = MAX_MASKS_PER_CUT
    order = sorted(grown.items(), key=lambda kv: -int(kv[1].sum()))
    for (dx, dy), m in order:
        if budget <= 0: break
        share = max(4, budget // max(1, len(order)))
        rs = rects_from_mask(m, min(budget, share))
        gm = []
        for (x, y, w, h) in rs:
            sxp, syp = x+dx, y+dy
            if sxp < 0 or syp < 0 or sxp+w > 320 or syp+h > 240: continue
            win = dep[syp:syp+h, sxp:sxp+w]
            if not (win > 0).any(): continue
            gm.append((x, y, w, h, int(np.median(win[win > 0]))))
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
