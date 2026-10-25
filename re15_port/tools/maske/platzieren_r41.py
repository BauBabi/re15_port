"""Runde 41 Spur M: UNPLATZIERTE Teile des ORIGINAL-Vordergrundatlas der Stages 3..6 an ihre Stelle setzen.
Nachbesserung 4 (Abnahme 3, Mangel 2): Befund + Vorschlag; Nachbesserung 5 (Abnahme 4, Mangel 1 = Entscheid M-E2): eingebaut, die
Tiefe je Teil aus seiner KOLLISIONSZELLE (nicht mehr die Generator-Regel "Bodenkontakt auf y = 0, x 0,90" des NB4-Vorschlags).

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitte "Nachbesserung 4" und "Nachbesserung 5".

BEFUND (NB4): Der Generator (analysis/esp_masken_2026-09-03/gen_masken.py build_cut) saet nur aus VOLLSTAENDIG deckenden 16x16-Bloecken
des Kuenstler-Atlas (gen_region.find_blocks: `if cnt != n: continue`, NCC >= 0.97) und waechst von dort (region_grow) nur innerhalb
schon belegter Verschiebungen. Duenne Teile - Gelaender, Rohre, Kabel, Lampen - haben nie einen solchen Block; liegen sie im
Atlas als EIGENE Komponente (8-zusammenhaengend, Index != 0), wurden sie nie platziert und fehlen im Spiel ganz. Gemessen an den 216
Generator-Sektionen: 800 unplatzierte Komponenten (>= 16 Texel, 410 127 Texel), z.B. ROOM30E0 Cut 7: das Gelaender des Stegs (alle
Pfosten und der Handlauf).

ENTSCHEID M-E2 (AUFTRAG.md 2026-10-25 14:29): "Spur M setzt die Teile des Original-Atlas ein, in ALLEN Kameras (auch auf der Restliste
von Nachtrag 27), sofern je Teil Lage (Bildvergleich gegen den Hintergrund) und Tiefe (Zelle mit RDT-Offset) belegt sind. M-E1 gilt
weiter fuer AUTOMATISCHE Masken ohne Original-Teil."

REGEL (je Cut, je unplatzierter Komponente K des Original-Atlas = SLD des BSS-Chunks, Entpacker wie FUN_800c47e8 / original.atlas):
  1. LAGE = Verschiebung s mit der kleinsten mittleren quadratischen Farbabweichung (maskierte SSD ueber ALLE ganzzahligen s per
     FFT, nur Texel von K, nur Texel im Bild; mindestens 90 % von K im Bild). Grund: der Atlasinhalt ist der Hintergrund an der
     Zielstelle (gen_region Kopf, Q1) - an der richtigen Lage bleibt nur die Paletten-Rundung.
  2. Annahme nur, wenn (a) RMS <= RMS_MAX und (b) die beste NICHT benachbarte Lage (|ds| > 2) mindestens EINDEUTIG-mal schlechter ist.
     Kalibriert an den 422 vom Generator platzierten (verifizierten) Teilen >= 16 Texel derselben Sektionen (Dossier NB4): die SSD
     findet 410 an der Generator-Lage; RMS dort Median 9,66, 99 % 18,91, Maximum 20,24 -> RMS_MAX = 18,91 (99 %-Punkt); Verhaeltnis
     zweitbeste/beste bei den 12 abweichenden Faellen hoechstens 1,47 -> EINDEUTIG = 1,5 (darueber lag keiner).
  3. Texel von K, deren Farbe an der Lage um mehr als 48 abweicht (|dR|+|dG|+|dB|, Grenze der R37-Blit-Pruefung), bleiben weg
     (wie das Wachstum des Generators je Texel prueft). Das Teil = die uebrigen Texel an ihrer Bildstelle.
  4. Bildpunkte, die schon ein vorhandenes Rechteck des Cuts deckend zeichnet (gezeichnet ODER ausgeschlossen), bleiben weg -
     die neuen Rechtecke kommen nur dazu (alter Teil jeder Sektion bytegleich, Probe unten).
  5. KOERPER je Teil (M-E2): (a) Zuordnung von Hand (zuordnung_teile_r41.json: Raum, Cut, Bildpunkt im Teil, RDT-Offset der Zelle,
     Beleg), sonst (b) automatisch wie fuer die Generator-Objekte (NB1/NB2 tiefe_zelle_r41.zelle_geometrie, Regel der
     Nutzer-Freistellungen raum.py "zelle"): je begehbarem Band (geom.begehbare_baender) die solide Sperrzelle mit der Mehrheit der
     Bodenkontakte des Teils (geometrie.standpunkte + zelle_zuordnen) - gilt nur, wenn GENAU EIN Band eine Zelle liefert.
     Koerper = Zellform des Kollisions-Resolvers (koerper_r41: Typ 1 Kasten, 2 Raute push_diag2, 3 Kreis, 4..7 Schraegen) x
     [Bandboden + Hoehe, Bandboden]; HOEHE = Oberkante des Teils auf der Eintrittsflaeche (oberkante(): je Bildpunkt des Teils die Hoehe,
     in der sein Sehstrahl in das Prisma ueber dem Grundriss eintritt, davon das Maximum). Nicht das IoU-Maximum der Generator-Objekte
     (koerper_r41.hoehe_messen): fuer duenne Teile auf breiten Zellen faellt es auf den Rand des Messbereichs (gemessen NB5: 88 von 266
     Teilen bei -25, z.B. das Steg-Gelaender ROOM30E0 C9 auf Zelle 0x00600, 2000 breit); wo es traegt, stimmen beide ueberein (ROOM30E0 C7
     Gelaender auf Zelle 0x005E8: IoU 1450..1525, Oberkante 1494..1596; ROOM3050 C6: 925/650/2025 gegen 910/714/2075).
     KEIN Koerper -> das Teil bleibt weg (Grund im Bericht, Liste "Fuer den Nutzer: Freistellungen"): Zelltyp 8/9 (Kapsel, Form
     nicht modelliert), Kamera ueber dem Grundriss der Zelle (Nahfeld, kein 3D-Urteil: r41_koerper_davor -1), Saeule mit
     nicht-rechteckiger Zelle, kein Punkt des Teils getroffen. Beleg je Teil: RDT-Offset + 12 Bytes des SCA-Eintrags
     (zellen3d_r41.sca_beleg) in der Modell-Tabelle.
  6. TIEFE je Bildpunkt = bucket(Eintritt des Sehstrahls in den Koerper) (koerper_r41.ein_aus mit der Kamera des SPIELS, ansicht_spiel():
     SquareRoot0 wie re15_camera_build_view / FUN_80053ca4; bucket(vz) = (1023*vz)>>16: Figur-OT
     @0x8002565c sra v1,v1,4 mit ZSF3 = 341 @0x80066c70; Masken-OT = depth @0x80039650; re15_pri_mask_occludes: verdeckt gdw.
     depth < bucket(vz_Figur)). Punkte des Teils, die der Strahl nicht trifft (ueber der gemessenen Hoehe, neben dem Grundriss),
     erben den naechsten getroffenen Punkt DESSELBEN Teils (geometrie._erben - die Regel der Nutzer-Freistellungen raum.py "zelle"
     und der NB1-Stufe 2). Je Bildspalte des Teils die groesste Tiefe; Rechtecke = Grundstuecke gleicher Spaltentiefe, gierig
     zusammengelegt (schliessen_r41.stuecke_bilden/zusammenlegen); RECHTECK-TIEFE = MAXIMUM ueber seine Punkte -> eine Figur VOR dem
     Koerper wird nie verdeckt (vz_Figur < Eintritt: bucket(vz_Figur) <= bucket(Eintritt) <= Tiefe).
  7. PLATZ: hoechstens MAX_MASKEN = 128 Rechtecke je Cut (re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT, NB5). Das Original legt die Zahl als
     BYTE an (FUN_80039270 @0x8003928c lbu v0,7(a0) = RDT-Kopfbyte 7; @0x80039294 sll 2 / @0x800392ac sll 5 / @0x800392c4 sll 5 = 4 +
     32 + 32 Bytes Arena je Maske) - dort ist 255 die Grenze; 128 = Breite der Port-Tabelle des Ausschlusses (masken36_r37.c bits[4]).
     Reicht der Platz nicht, kommen die groessten Teile zuerst.
  8. ATLAS: das eigene Blatt des Cuts (_PRIcc.TIM) behaelt alle vorhandenen Fenster; neue Texel kommen an die erste freie Stelle,
     Farbe = Hintergrund an der Zielstelle; Palette des Blatts neu (255 Farben Median-Schnitt, Index 0 = durchsichtig - Gelaender-
     Luecken bleiben offen).
  9. MODELL-TABELLE (tests/unit/gen_r41_masken36_zellen.inc, --tabelle/--tabelle-aus): je neuem Rechteck eine Zeile (Zelle, Typ, Band,
     Hoehe, Saeule, Tiefe, RDT-Offset + 12 Bytes). Danach rechnet test_r37_masken36 "erzeuge" den Ausschluss der Spur P (R37) neu -
     mit dem 3D-Urteil (r41_koerper_davor) fuer die neuen Rechtecke wie fuer die Generator-Rechtecke mit Modell: ein Rechteck, das an
     einem erreichbaren Stehplatz einen Figurpunkt ohne Zelle davor UND ohne seinen Koerper davor verdecken wuerde, faellt weg.
Leon- und Elza-Datei bleiben bytegleich.

Aufruf:
  python re15_port/tools/maske/platzieren_r41.py --ppm <probe_bg_dump-Ordner> --cache <Regionen-Cache> --out <Ordner>
         --tabelle <Modell-Tabelle vorher> --tabelle-aus <Modell-Tabelle nachher> [--inc <Riegel-Tabelle>] [ROOMxxxx ...]
"""
import argparse
import json
import os
import struct
import sys

import numpy as np
from scipy import ndimage
from scipy.signal import fftconvolve

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import geom                      # noqa: E402
import geometrie as G            # noqa: E402
import koerper_r41 as K          # noqa: E402
import maskenbild                # noqa: E402
import original                  # noqa: E402
import schliessen_r41 as SR      # noqa: E402
import tiefe_zelle_r41 as TZ     # noqa: E402
import zellen3d_r41 as Z3        # noqa: E402

PSX = SR.PSX
MASKS = SR.MASKS
MAX_MASKEN = 128                 # re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT (NB5): Original = Byte-Zahl (FUN_80039270 @0x8003928c lbu v0,7(a0),
                                 # Grenze 255); 128 = Breite der Port-Tabelle des Ausschlusses (masken36_r37.c bits[4]) - Kopf Regel 7
MIN_TEXEL = 16                   # kleinste Komponente (wie die Kalibrierung; kleiner ist die Lage nicht belegbar)
RMS_MAX = 18.91                  # 99 %-Punkt der RMS an der Generator-Lage (Kalibrierung, Kopf)
EINDEUTIG = 1.5                  # zweitbeste/beste > 1,47 (groesster Wert der abweichenden Kalibrierfaelle)
IM_BILD = 0.9                    # Anteil der Texel im Bild
FARBE_MAX = 48                   # R37 §2.1 Blit-Pruefung: |dR|+|dG|+|dB| > 48 zaehlt als Abweichung


def _corr(img, kern):
    return fftconvolve(img, kern[::-1, ::-1], mode="full")


def lage_suchen(maske, farbe, bild):
    """maske (h,w) bool, farbe (h,w,3) Atlasfarben, bild (240,320,3). -> [(dx, dy, mittel_d2, n_im_bild)] je beste Lage zuerst;
    dx/dy = Lage der linken oberen Ecke des Ausschnitts im Bild."""
    h, w = maske.shape
    m = maske.astype(np.float64)
    B = bild.astype(np.float64)
    O = np.ones(bild.shape[:2])
    n_on = _corr(O, m)
    t1 = _corr(O, m * (farbe.astype(np.float64) ** 2).sum(2))
    t2 = np.zeros_like(n_on)
    t3 = np.zeros_like(n_on)
    for ch in range(3):
        t2 += _corr(B[..., ch], m * farbe[..., ch])
        t3 += _corr(B[..., ch] ** 2, m)
    ssd = np.maximum(t1 - 2 * t2 + t3, 0.0)          # FFT-Rundung kann knapp negativ werden
    ok = n_on >= IM_BILD * m.sum() - 0.5
    md = np.where(ok, ssd / np.maximum(n_on, 1.0), np.inf)
    out = []
    for k in np.argsort(md, axis=None)[:60]:
        iy, ix = np.unravel_index(int(k), md.shape)
        if not np.isfinite(md[iy, ix]):
            break
        out.append((int(ix - (w - 1)), int(iy - (h - 1)), float(md[iy, ix]), int(round(n_on[iy, ix]))))
    return out


def platzieren(idx, clut, bild, grown):
    """-> Liste dict je angenommener Komponente: komp, n, shift (Atlas -> Bild), rms, rms2, maske (Atlas, verifizierte Texel)."""
    pal = SR.clut_rgb(clut).astype(np.int32)
    A = pal[idx]
    lab, n = ndimage.label(idx != 0, structure=np.ones((3, 3)))
    belegt = set()
    for m in grown.values():
        belegt |= set(int(c) for c in np.unique(lab[m]) if c)
    erg = []
    for c in range(1, n + 1):
        if c in belegt:
            continue
        ys, xs = np.nonzero(lab == c)
        if len(ys) < MIN_TEXEL:
            continue
        y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
        best = lage_suchen(lab[y0:y1, x0:x1] == c, A[y0:y1, x0:x1], bild)
        if not best:
            continue
        b0 = best[0]
        b1 = next((b for b in best[1:] if abs(b[0] - b0[0]) > 2 or abs(b[1] - b0[1]) > 2), None)
        rms = float(np.sqrt(b0[2]))
        rms2 = float(np.sqrt(b1[2])) if b1 else None
        e = dict(komp=int(c), n=int(len(ys)), box=(int(x0), int(y0), int(x1), int(y1)), shift=(int(b0[0] - x0), int(b0[1] - y0)), rms=round(rms, 2),
                 rms2=round(rms2, 2) if rms2 is not None else None)
        e["angenommen"] = bool(rms <= RMS_MAX and (rms2 is None or rms2 >= EINDEUTIG * max(rms, 1e-6)))
        if e["angenommen"]:
            dx, dy = e["shift"]
            X, Y = xs + dx, ys + dy
            im = (X >= 0) & (X < 320) & (Y >= 0) & (Y < 240)
            d = np.full(len(ys), 999)
            d[im] = np.abs(A[ys[im], xs[im]] - bild[Y[im], X[im]].astype(np.int32)).sum(1)
            ok = d <= FARBE_MAX
            mk = np.zeros(idx.shape, bool)
            mk[ys[ok], xs[ok]] = True
            e["maske"] = mk
            e["verifiziert"] = int(ok.sum())
        erg.append(e)
    return erg


HAND = {}                        # (raum, cut) -> Eintraege aus zuordnung_teile_r41.json (Kopf Regel 5a)
_SQRT0 = []                      # SquareRoot0-Tabelle (lazy, aus PSX.EXE)


def _sqrt0_tabelle():
    """Mantissentabelle der BIOS-Wurzel SquareRoot0 (0x80065f60): 192 x u16 @0x8007d984, gelesen aus info/Re1.5/PSX.EXE
    (Datei-Offset 0x800 + Adresse - t_addr, t_addr = Kopf +0x18) - dieselbe Tabelle wie engine/src/re15_math.c."""
    if not _SQRT0:
        exe = open(os.path.join(SR.REPO, "info", "Re1.5", "PSX.EXE"), "rb").read()
        taddr = struct.unpack_from("<I", exe, 0x18)[0]
        _SQRT0.extend(struct.unpack_from("<192H", exe, 0x800 + 0x8007d984 - taddr))
    return _SQRT0


def sqrt0(x):
    """SquareRoot0 wie re15_squareroot0 (re15_math.c; LZCR, gerade Zahl, Tabelle, Q12)."""
    x &= 0xFFFFFFFF
    if x == 0:
        return 0
    y = (~x & 0xFFFFFFFF) if (x & 0x80000000) else x
    n = 32 - y.bit_length()
    t2 = n & ~1
    t1 = (31 - t2) >> 1
    t4 = ((x << (t2 - 24)) & 0xFFFFFFFF) if t2 >= 24 else (x >> (24 - t2))
    return ((_sqrt0_tabelle()[t4 - 64] << t1) & 0xFFFFFFFF) >> 12


def _tdiv(a, b):
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


def _s16(v):
    return ((v + 0x8000) & 0xFFFF) - 0x8000


def ansicht_spiel(rdt, cut):
    """Kamera des Spiels (R[9], t[3], H) wie re15_camera_build_view = FUN_80053ca4: Betraege mit SquareRoot0 (jal 0x80053d60 /
    0x80053dd8 -> 0x80065f60) statt exakter Wurzel, abschneidende Division, (int16)-Casts, >>12. geom.cut_view rechnet mit der exakten
    Wurzel - die Sehstrahlen lagen dadurch bis zu einer OT-Stufe neben denen des Spiels (gemessen NB5: 3 880 von 98 740 Punkten,
    test_r37_masken36 teile)."""
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    rec = cam + cut * 32
    fov = struct.unpack_from("<H", rdt, rec + 2)[0]
    px, py, pz = struct.unpack_from("<iii", rdt, rec + 4)
    tx, ty, tz = struct.unpack_from("<iii", rdt, rec + 0x10)
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = sqrt0(dx * dx + dy * dy + dz * dz)
    if dist == 0:
        return None
    horiz = sqrt0(dx * dx + dz * dz)
    sp = _s16(_tdiv(-dy * 4096, dist))
    cp = _s16(_tdiv(horiz * 4096, dist))
    if horiz != 0:
        sy = _s16(_tdiv(dx * 4096, horiz))
        cy = _s16(_tdiv(dz * 4096, horiz))
        R = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    else:
        R = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
    t = [(R[0] * -px + R[1] * -py + R[2] * -pz) >> 12, (R[3] * -px + R[4] * -py + R[5] * -pz) >> 12,
         (R[6] * -px + R[7] * -py + R[8] * -pz) >> 12]
    return R, t, fov >> 7


def koerper_aus_zelle(rdt, R, t, H, teil, z, band, anteil, quelle, off=None):
    """Koerper (Kopf Regel 5) einer Zelle z = (x, z, breite, tiefe, typ) auf `band` fuer das Teil (bool 240x320).
    -> dict mit zelle/typ/band/hoehe/iou/saeule/off/bytes/ve/hit oder {"grund": ...}."""
    x, zz, w, d, typ = (int(v) for v in z)
    if typ not in (1, 2, 3, 4, 5, 6, 7):
        return {"grund": "Zelltyp %d (Kapsel, Form nicht modelliert)" % typ}
    if K.kamera_im_grundriss(R, t, typ, x, zz, w, d):
        return {"grund": "Kamera ueber dem Grundriss der Zelle (%d,%d %dx%d): kein 3D-Urteil" % (x, zz, w, d)}
    hoehe = oberkante(R, t, H, typ, x, zz, w, d, band, teil)
    if hoehe is None:
        return {"grund": "kein Sehstrahl des Teils tritt in den Grundriss der Zelle (%d,%d %dx%d)" % (x, zz, w, d)}
    saeule = False
    ve, va, hit = K.ein_aus(R, t, H, typ, x, zz, w, d, band, hoehe, saeule)
    if not (hit & teil).any():
        return {"grund": "kein Punkt des Teils getroffen"}
    u = float((hit | teil).sum())
    iou = float((hit & teil).sum()) / u if u else 0.0       # nur Bericht/Kommentar der Modell-Zeile (Silhouette des Koerpers gegen das Teil)
    if off is None:
        off, by = Z3.sca_beleg(rdt, (x, zz, w, d), typ, band)
    else:
        by = bytes(rdt[off:off + 12])
    return dict(zelle=(x, zz, w, d), typ=typ, band=int(band), hoehe=int(hoehe), iou=round(float(iou), 3), saeule=bool(saeule),
                anteil=round(float(anteil), 3) if anteil is not None else None, quelle=quelle, off=int(off), bytes=by, ve=ve, hit=hit)


def oberkante(R, t, H, typ, x, zz, w, d, band, teil):
    """Kopf Regel 5: HOEHE des Koerpers = Oberkante des Teils auf der Eintrittsflaeche - je Bildpunkt des Teils die Hoehe ueber dem
    Bandboden, in der sein Sehstrahl in das (nach oben offene) Prisma ueber dem Grundriss der Zelle eintritt (koerper_r41.ein_aus mit
    Saeule), davon das Maximum. Damit tritt JEDER Strahl des Teils, der den Grundriss kreuzt, durch eine Seitenflaeche (bzw. am hoechsten
    Punkt durch den Deckel) in den Koerper ein - so hoch reicht der Gegenstand im Bild. -> negative Hoehe (PSX-Y nach unten) oder None."""
    ve, _va, hit = K.ein_aus(R, t, H, typ, x, zz, w, d, band, -1.0, True)
    m = hit & teil
    if not m.any():
        return None
    c, dd = K.ansicht(R, t, H)
    r = np.array(R[6:9], float) / 4096.0
    den = (dd * r).sum(-1)
    s = (ve - t[2] - c.dot(r)) / np.where(np.abs(den) > 1e-12, den, 1e-12)
    y_ein = c[1] + s * dd[..., 1]
    ueber = (-K.BAND_HOEHE * band) - y_ein[m]           # Hoehe ueber dem Bandboden (positiv nach oben)
    return -max(1, int(np.ceil(float(ueber.max()))))


def koerper_teil(rdt, room, cut, R, t, H, teil, baender, hull):
    """Kopf Regel 5: Koerper des Teils - Zuordnung von Hand, sonst die Sperrzelle mit der Mehrheit der Bodenkontakte (genau ein Band)."""
    for e in HAND.get((room, cut), []):
        px, py = e["punkt"]
        if 0 <= py < 240 and 0 <= px < 320 and teil[py, px]:
            z, b = TZ.zelle_von_hand(rdt, e)
            g = koerper_aus_zelle(rdt, R, t, H, teil, z, b, None, "Hand " + e["zelle"], off=int(e["zelle"], 16))
            if "grund" in g:
                g["grund"] = "Zuordnung von Hand %s: %s" % (e["zelle"], g["grund"])
            return g
    treffer, gruende = [], []
    for b in baender:
        y0 = -b * geom.BAND_HOEHE
        zellen = geom.sca_sperrzellen(rdt, b) or []
        if not zellen:
            gruende.append("Band %d: keine Sperrzelle" % b)
            continue
        stand, eigene, _ohne = G.standpunkte(teil, R, t, H, y0, hull)
        if not stand:
            gruende.append("Band %d: kein Bodenkontakt" % b)
            continue
        z, anteil = G.zelle_zuordnen(teil, eigene, R, t, H, y0, zellen, hull)
        if z is None:
            gruende.append("Band %d: keine Zelle traegt die Mehrheit der Bodenkontakte (%.2f)" % (b, anteil))
            continue
        treffer.append((z, b, anteil))
    if not treffer:
        return {"grund": "; ".join(gruende) or "kein begehbares Band"}
    if len(treffer) > 1:
        return {"grund": "Zelle auf mehreren Baendern %s" % [tr[1] for tr in treffer]}
    z, b, anteil = treffer[0]
    return koerper_aus_zelle(rdt, R, t, H, teil, z, b, anteil, "automatisch")


def tiefe_teil(k, teil):
    """Kopf Regel 6: Tiefe je Bildpunkt des Teils = bucket(Eintritt), Fehltreffer erben den naechsten Treffer desselben Teils.
    -> (dep int 240x320, 0 ausserhalb des Teils; Zahl der geerbten Punkte)."""
    vz = np.where(k["hit"] & teil, k["ve"], 0.0)
    geerbt = int((teil & ~(k["hit"] & teil)).sum())
    vz = G._erben(vz, teil)
    dep = np.where(teil, K.bucket(vz), 0).astype(np.int32)     # ein_aus liefert nur vz > 64 -> bucket >= 1
    return dep, geerbt


def spaltentiefe(dep, teil):
    """Je Bildspalte des Teils die groesste Tiefe (Kopf Regel 6)."""
    sp = np.where(teil, dep, 0).max(0)
    return np.where(teil, sp[None, :], 0).astype(np.int32)


def naechster_index(rgb, pal):
    """rgb (N,3) -> Index 1..255 der naechsten Palettenfarbe (Index 0 = durchsichtig bleibt frei)."""
    p = pal[1:].astype(np.int32)
    d = ((rgb[:, None, :].astype(np.int32) - p[None, :, :]) ** 2).sum(2)
    return (np.argmin(d, 1) + 1).astype(np.uint8)


def cut_bearbeiten(rdt, room, cut, sek, idx_eigen, clut_eigen, idx_orig, clut_orig, bild, grown, reserve=0):
    """-> (Sektion, (Index, Palette), Bericht, Modell-Zeilen) bzw. (None, None, Bericht, [])."""
    rid = int(room[4:], 16)
    bericht = {"cut": cut}
    alt = []
    deck_alt = np.zeros((240, 320), bool)
    belegt = np.zeros(idx_eigen.shape, bool)
    for m in sek["masks"]:
        X, Y = SR.bildlage(sek, m)
        sx, sy, w, h, d0 = m[0], m[1], m[6], m[7], m[4]
        alt.append((sx, sy, X, Y, w, h, d0))
        belegt[sy:sy + h, sx:sx + w] = True
        op = idx_eigen[sy:sy + h, sx:sx + w] != 0
        ys, xs = np.nonzero(op)
        Xs, Ys = xs + X, ys + Y
        ok = (Xs >= 0) & (Xs < 320) & (Ys >= 0) & (Ys < 240)
        deck_alt[Ys[ok], Xs[ok]] = True
    budget = MAX_MASKEN - len(alt) - reserve
    komps = platzieren(idx_orig, clut_orig, bild, grown)
    bericht["komponenten"] = komps                      # Liste wird am Ende ohne Masken abgelegt (neu_px erst unten bekannt)
    ang = sorted([e for e in komps if e["angenommen"]], key=lambda e: -e["verifiziert"])
    bericht["budget"] = budget
    if budget <= 0 or not ang:
        bericht["neu"] = 0
        return None, None, bericht, []
    v = ansicht_spiel(rdt, cut)                           # Kamera des Spiels (SquareRoot0), nicht geom.cut_view
    if not v or v[2] <= 0:
        bericht["neu"] = 0
        bericht["grund"] = "keine Kamera"
        return None, None, bericht, []
    R, t, H = v
    hull = G.huelle(rdt)
    baender = sorted(b for b in geom.begehbare_baender(rid) if 0 <= b < 16)
    stuecke_alle = []
    dep_cut = np.zeros((240, 320), np.int32)              # Spaltentiefe je Bildpunkt (Kopf Regel 6) aus dem Koerper SEINES Teils
    komp_cut = np.zeros((240, 320), np.int32)             # Komponente je Bildpunkt
    koerper_von = {}                                      # Komponente -> Koerper (Modell-Zeilen)
    for e in ang:
        dx, dy = e["shift"]
        ys, xs = np.nonzero(e["maske"])
        X, Y = xs + dx, ys + dy
        ok = (X >= 0) & (X < 320) & (Y >= 0) & (Y < 240)
        teil = np.zeros((240, 320), bool)
        teil[Y[ok], X[ok]] = True
        schirm = teil & ~deck_alt & (dep_cut == 0)        # Regel 4; je Punkt eine Komponente
        if not schirm.any():
            e["neu_px"] = 0
            continue
        k = koerper_teil(rdt, room, cut, R, t, H, teil, baender, hull)
        if "grund" in k:
            e["neu_px"] = 0
            e["ohne_koerper"] = k["grund"]
            e["ohne_koerper_px"] = int(schirm.sum())
            continue
        dep, _geerbt = tiefe_teil(k, teil)
        dsp = spaltentiefe(dep, schirm)
        e["neu_px"] = int(schirm.sum())
        e["koerper"] = {kk: (list(vv) if isinstance(vv, tuple) else vv) for kk, vv in k.items() if kk not in ("ve", "hit", "bytes")}
        e["koerper"]["sca"] = k["bytes"].hex(" ")
        e["geerbt"] = int((schirm & ~k["hit"]).sum())
        e["tiefe"] = [int(dep[schirm].min()), int(dep[schirm].max())]
        dep_cut[schirm] = dsp[schirm]
        komp_cut[schirm] = e["komp"]
        koerper_von[e["komp"]] = k
        for s in SR.stuecke_bilden({(dx, dy): schirm}, dsp):
            s["sh"] = (dx, dy, e["komp"])                 # zusammenlegen vereint nur Stuecke DESSELBEN Teils (ein Koerper je Rechteck)
            stuecke_alle.append(s)
    if not stuecke_alle:
        bericht["neu"] = 0
        return None, None, bericht, []
    wahl = None
    for stufe in SR.STUFEN + (24, 32, 48, 64, 1024):
        neu = SR.zusammenlegen(stuecke_alle, stufe, budget)
        if len(neu) <= budget:
            wahl = (stufe, neu)
            break
    if wahl is None:
        # Platz reicht nicht: die groessten Stuecke zuerst (Kopf Regel 7)
        stufe, neu = 1024, sorted(SR.zusammenlegen(stuecke_alle, 1024, budget), key=lambda s: -len(s["ys"]))[:budget]
    else:
        stufe, neu = wahl
    pal = SR.clut_rgb(clut_eigen).astype(np.int32)
    rgb_at = pal[idx_eigen].astype(np.uint8)          # alte Texel: ihre bisherige Farbe
    op_at = idx_eigen != 0
    plaetze, verloren = [], 0
    for s in sorted(neu, key=lambda s: -SR._flaeche(s["box"])):
        x0, y0, x1, y1 = s["box"]
        w, h = x1 - x0, y1 - y0
        pl = SR.frei_platz(belegt, w, h)
        if pl is None:
            verloren += len(s["ys"])
            continue
        ax, ay = pl
        belegt[ay:ay + h, ax:ax + w] = True
        ys, xs = s["ys"], s["xs"]
        rgb_at[ys - y0 + ay, xs - x0 + ax] = bild[ys, xs]   # neue Texel: Hintergrund an der Zielstelle
        op_at[ys - y0 + ay, xs - x0 + ax] = True
        plaetze.append((s, ax, ay))
    if not plaetze:
        bericht["neu"] = 0
        bericht["atlas_verloren"] = verloren
        return None, None, bericht, []
    # Palette fuer das ganze Blatt neu (255 Farben Median-Schnitt, Index 0 = durchsichtig; schliessen_r41.palette_und_index):
    # die vorhandene Palette kennt die Farben der neuen Teile nicht (gemessen ROOM30E0 C9: naechste Farbe im Mittel 51,6 daneben).
    idx_neu, pal_neu = SR.palette_und_index(rgb_at, op_at)
    pal_neu_full = np.zeros((256, 3), np.int32)
    pal_neu_full[1:1 + len(pal_neu)] = pal_neu
    alt_abw = np.abs(pal_neu_full[idx_neu[idx_eigen != 0]] - pal[idx_eigen[idx_eigen != 0]]).sum(1)
    groups = [(g[0], g[2], g[3]) for g in sek["groups"]]
    masks = list(alt)
    neue, modell = [], []
    for j, (s, ax, ay) in enumerate(sorted(plaetze, key=lambda q: (q[0]["box"][1], q[0]["box"][0]))):
        x0, y0, x1, y1 = s["box"]
        d = int(dep_cut[s["ys"], s["xs"]].max())            # Kopf Regel 6: Rechteck-Tiefe = Maximum ueber seine Punkte
        neue.append((ax, ay, x0, y0, x1 - x0, y1 - y0, d))
        komp = int(s["sh"][2])
        k = koerper_von[komp]
        modell.append((rid, cut, len(masks) + j, k["zelle"], k["typ"], k["band"], k["hoehe"], k["saeule"], d, k["off"], k["bytes"],
                       k["iou"], komp))
    for m in neue:
        groups.append((1, m[2] - m[0], m[3] - m[1]))
    sec = geom.pack_section(groups, masks + neue)
    # Farbe der neuen Texel gegen den Hintergrund (eigene Palette)
    abw = []
    for (s, ax, ay) in plaetze:
        x0, y0 = s["box"][0], s["box"][1]
        ii = idx_neu[s["ys"] - y0 + ay, s["xs"] - x0 + ax]
        abw.append(np.abs(pal_neu_full[ii] - bild[s["ys"], s["xs"]].astype(np.int32)).sum(1))
    abw = np.concatenate(abw) if abw else np.zeros(0)
    bericht.update(neu=len(neue), stufe=stufe, rechtecke=len(masks) + len(neue), neu_px=int(sum(len(s["ys"]) for s, _, _ in plaetze)),
                   atlas_verloren=verloren, farbe_mittel=round(float(abw.mean()), 2) if len(abw) else 0.0,
                   farbe_ueber48=round(float((abw > FARBE_MAX).mean()), 4) if len(abw) else 0.0,
                   neue_rechtecke=[list(m) for m in neue], erstes_neues=len(masks),
                   alte_texel_abw_mittel=round(float(alt_abw.mean()), 2) if len(alt_abw) else 0.0,
                   alte_texel_abw_max=int(alt_abw.max()) if len(alt_abw) else 0)
    return sec, (idx_neu, pal_neu), bericht, modell


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--ppm", required=True, help="probe_bg_dump-Ordner (ROOMrrrcc.ppm)")
    ap.add_argument("--cache", required=True, help="Regionen-Cache des Generators (ROOMxxxx_cc.npz, schliessen_r41.gewachsen)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--quelle", default=MASKS)
    ap.add_argument("--inc", default=None, help="C-Tabelle fuer den Riegel (tests/unit/gen_r41_masken36_platziert.inc)")
    ap.add_argument("--reserve", action="append", default=[], help="RAUM:CUT:N - N Plaetze des Cuts frei lassen (z.B. fuer teilen_r41.py)")
    ap.add_argument("--bericht-dazu", default=None, help="platzieren_r41.json eines frueheren Laufs: Raeume, die hier nicht laufen, von dort")
    ap.add_argument("--tabelle", default=None, help="Modell-Tabelle VORHER (tests/unit/gen_r41_masken36_zellen.inc), Zeilen bleiben")
    ap.add_argument("--tabelle-aus", default=None, help="Modell-Tabelle NACHHER: vorhandene Zeilen + je neuem Rechteck eine Zeile (Kopf Regel 9)")
    ap.add_argument("--zuordnung", default=os.path.join(HIER, "zuordnung_teile_r41.json"),
                    help="Zellen von Hand je Teil (Kopf Regel 5a): {Raum: {Cut: [{teil, punkt, zelle, beleg}]}}")
    a = ap.parse_args()
    if a.zuordnung and os.path.exists(a.zuordnung):
        for room, cuts in json.load(open(a.zuordnung, encoding="utf-8")).items():
            if room.startswith("ROOM"):
                for cut, liste in cuts.items():
                    HAND[(room, int(cut))] = liste
    zeilen_neu = []
    reserve = {}
    for r in a.reserve:
        rr, cc, nn = r.split(":")
        reserve[(int(rr, 16), int(cc))] = int(nn)
    os.makedirs(a.out, exist_ok=True)
    bericht = {}
    for f in sorted(os.listdir(a.quelle)):
        if not (len(f) == 12 and f.endswith(".MSK") and f[4] in "3456"):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        if rid & 1 or (a.raeume and room not in a.raeume):
            continue
        blob = open(os.path.join(a.quelle, f), "rb").read()
        if open(os.path.join(a.quelle, "ROOM%04X.MSK" % (rid | 1)), "rb").read() != blob:
            raise SystemExit("%s: Elza-Datei nicht bytegleich" % room)
        secs, n_cut = SR.sektionen(blob)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "%s.RDT" % room), "rb").read()
        at = original.atlas(room)
        offs = struct.unpack_from("<%dI" % n_cut, blob, 12)
        alle = {}
        for cut in range(n_cut):
            if offs[cut]:
                ende = min([o for o in offs if o > offs[cut]] + [len(blob)])
                alle[cut] = blob[offs[cut]:ende]
        b_raum, tims = {}, {}
        for cut, sek in sorted(secs.items()):
            tp = os.path.join(a.quelle, "%s_PRI%02d.TIM" % (room, cut))
            gp = os.path.join(a.cache, "%s_%02d.npz" % (room, cut))
            if cut not in at or not os.path.exists(tp) or not os.path.exists(gp):
                continue
            bild = SR.bg_lesen(a.ppm, rid, cut)
            if bild is None:
                continue
            idx_e, clut_e = maskenbild.lies_tim(tp)
            z = np.load(gp)
            grown = {(int(k.split("_")[0]), int(k.split("_")[1])): z[k] for k in z.files}
            idx_o, clut_o = at[cut]
            sec, idx_neu, b, modell = cut_bearbeiten(rdt, room, cut, sek, idx_e, clut_e, idx_o, clut_o, bild, grown,
                                                     reserve.get((rid, cut), 0))
            b["komponenten"] = [{k: v for k, v in e.items() if k != "maske"} for e in b.get("komponenten", [])]
            b_raum[cut] = b
            if sec is None:
                continue
            # Probe: alter Teil der Sektion bytegleich (Gruppen und Rechtecke)
            o = offs[cut]
            ng, nm = len(sek["groups"]), len(sek["masks"])
            if sec[4:4 + 8 * ng] != blob[o + 4:o + 4 + 8 * ng]:
                raise SystemExit("%s Cut %d: alte Gruppen nicht bytegleich" % (room, cut))
            o_neu = 4 + 8 * (ng + b["neu"])
            if sec[o_neu:o_neu + 12 * nm] != blob[o + 4 + 8 * ng:o + 4 + 8 * ng + 12 * nm]:
                raise SystemExit("%s Cut %d: alte Rechtecke nicht bytegleich" % (room, cut))
            alle[cut] = sec
            tims[cut] = SR.atlasmod.tim_aus(idx_neu[0], idx_neu[1])
            zeilen_neu += modell
            print("%s C%-2d +%3d Rechtecke (%3d -> %3d, Stufe %d), %5d neue Punkte, Farbe Mittel %.1f, alte Texel %.2f (max %d), "
                  "Atlas verloren %d" % (room, cut, b["neu"], nm, b["rechtecke"], b["stufe"], b["neu_px"], b["farbe_mittel"],
                                         b["alte_texel_abw_mittel"], b["alte_texel_abw_max"], b["atlas_verloren"]), flush=True)
        if tims:
            out = geom.pack_container(alle, n_cut)
            for r in (rid, rid | 1):
                open(os.path.join(a.out, "ROOM%04X.MSK" % r), "wb").write(out)
                for cut, tb in tims.items():
                    open(os.path.join(a.out, "ROOM%04X_PRI%02d.TIM" % (r, cut)), "wb").write(tb)
        bericht[room] = b_raum
    if a.bericht_dazu:
        for room, cuts in json.load(open(a.bericht_dazu)).items():
            bericht.setdefault(room, cuts)
    json.dump(bericht, open(os.path.join(a.out, "platzieren_r41.json"), "w"), indent=1, default=str)
    if a.inc:
        inc_schreiben(a.inc, bericht)
    if a.tabelle_aus:
        tabelle_schreiben(a.tabelle, a.tabelle_aus, zeilen_neu)


def tabelle_schreiben(vorher, nachher, zeilen_neu):
    """Kopf Regel 9: Modell-Tabelle = vorhandene Zeilen (unveraendert, Text wie gelesen) + je neuem Rechteck eine Zeile im Format von
    kacheln_r41.tabelle_schreiben / zellen3d_r41 (Raum, Cut, Rechteck, Zelle x z breite tiefe, Typ, Band, Hoehe, Saeule, Tiefe,
    RDT-Offset, 12 SCA-Bytes; Kommentar IoU + Herkunft)."""
    import re
    alt_zeilen, kopf = [], []
    if vorher:
        for z in open(vorher, encoding="utf-8").read().splitlines():
            if re.match(r"\s*\{ 0x([0-9A-F]{4}),", z) and not z.strip().startswith("{ 0x0000, 0xFF"):
                alt_zeilen.append(z)
            elif not alt_zeilen and not z.startswith("static const") and not z.strip().startswith("{ 0x0000"):
                kopf.append(z)
    neu_txt = []
    for (rid, cut, i, zl, typ, band, hoehe, saeule, tiefe, o, b, iou, komp) in zeilen_neu:
        neu_txt.append("    { 0x%04X, %2d, %3d, %6d, %6d, %5d, %5d, %d, %2d, %5d, %d, %3d, 0x%05X, { %s } },   /* IoU %.2f - Original-Atlas-Teil %d, platzieren_r41.py (NB5, M-E2) */"
                       % (rid, cut, i, zl[0], zl[1], zl[2], zl[3], typ, band, hoehe, 1 if saeule else 0, tiefe, o,
                          ", ".join("0x%02x" % c for c in b), iou, komp))
    rx = re.compile(r"\s*\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+),")

    def schluessel(z):
        m = rx.match(z)
        return (int(m.group(1), 16), int(m.group(2)), int(m.group(3)))
    alle = sorted(alt_zeilen + neu_txt, key=schluessel)
    if len(set(schluessel(z) for z in alle)) != len(alle):
        raise SystemExit("Modell-Tabelle: doppelte Zeile (Raum, Cut, Rechteck)")
    with open(nachher, "w", newline="\n") as f:
        f.write("/* Erzeugt von re15_port/tools/maske/kacheln_r41.py (Nachbesserung 3) auf zellen3d_r41.py (Nachbesserung 2) und\n"
                " * platzieren_r41.py (Nachbesserung 5, Entscheid M-E2: Teile des Original-Vordergrundatlas mit der Tiefe ihrer Zelle) -\n"
                " * NICHT von Hand aendern. Runde 41 Spur M: Rechtecke der Stages 3..6 mit Zellen-Modell (Koerper).\n"
                " * Je Zeile: Raum (Leon-Datei; Elza xxx1 ist bytegleich), Cut, Rechteck, SCA-Zelle x z breite tiefe, Typ, Band,\n"
                " * gemessene Hoehe (IoU-Maximum, negativ = nach oben), Saeule, Tiefe im MSK; Beleg: RDT-Offset und die\n"
                " * 12 Bytes des SCA-Eintrags dort (u16 breite, u16 tiefe, s16 x, s16 z, typ, u0, u1, floor); Kommentar: IoU.\n"
                " * Benutzt von tests/unit/test_r37_masken36.c (r41_koerper_davor) und tests/unit/test_r41_masken36.c (zellen3d). */\n"
                "static const r41_zellen_modell_t r41_masken36_zellen[] = {\n")
        for z in alle:
            f.write(z + "\n")
        f.write("    { 0x0000, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 } }   /* Ende */\n};\n")
    print("Modell-Tabelle: %d vorhandene + %d neue Zeilen -> %s" % (len(alt_zeilen), len(neu_txt), nachher), flush=True)


def inc_schreiben(pfad, bericht):
    """Tabelle fuer tests/unit/test_r41_masken36.c (platziert): je Sektion der erste neue Rechteck-Index und die Zahl, je
    angenommener Komponente ihr Atlas-Kasten, die Verschiebung und die Zahl der verifizierten Texel."""
    sek, komp = [], []
    for room, cuts in sorted(bericht.items()):
        rid = int(room[4:], 16)
        for cut, b in sorted(cuts.items(), key=lambda kv: int(kv[0])):
            if not b.get("neu"):
                continue
            sek.append((rid, int(cut), b["erstes_neues"], b["neu"], b["neu_px"]))
            for e in b["komponenten"]:
                if e.get("angenommen") and e.get("neu_px", 0) > 0:
                    komp.append((rid, int(cut), e["komp"], e["box"][0], e["box"][1], e["box"][2], e["box"][3], e["shift"][0],
                                 e["shift"][1], e["verifiziert"]))
    NL = chr(10)
    kopf = ["/* Erzeugt von re15_port/tools/maske/platzieren_r41.py (Runde 41 Spur M, Nachbesserung 4/5) - NICHT von Hand aendern.",
            " * Unplatzierte Kuenstler-Komponenten des SLD-Atlas der Stages 3..6, per Lage-Suche platziert, Tiefe aus der Kollisionszelle",
            " * (Entscheid M-E2; Regel im Kopf des Werkzeugs). Nur Teile mit Koerper (Zelle mit RDT-Offset) stehen hier.",
            " * r41_platz_sek: Raum (Leon-Datei; Elza xxx1 bytegleich), Cut, erster neuer Rechteck-Index, Zahl neuer Rechtecke, neue Punkte.",
            " * r41_platz_komp: Raum, Cut, Komponente, Atlas-Kasten x0 y0 x1 y1 (exklusiv), Verschiebung Atlas->Bild dx dy, verifizierte Texel. */"]
    zeilen = kopf + ["static const r41_platz_sek_t r41_platz_sek[] = {"]
    zeilen += ["    { 0x%04X, %2d, %3d, %3d, %5d }," % s for s in sek]
    zeilen += ["    { 0x0000, 0xFF, 0, 0, 0 }   /* Ende */", "};", "static const r41_platz_komp_t r41_platz_komp[] = {"]
    zeilen += ["    { 0x%04X, %2d, %3d, %3d, %3d, %3d, %3d, %4d, %4d, %5d }," % k for k in komp]
    zeilen += ["    { 0x0000, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0 }   /* Ende */", "};"]
    with open(pfad, "w", newline=NL) as f:
        f.write(NL.join(zeilen) + NL)
    print("Tabelle: %d Sektionen, %d Komponenten -> %s" % (len(sek), len(komp), pfad))


def SR_tim_mit_index(tb, idx):
    """8-bpp-TIM: CLUT-Block bytegleich lassen, Indexbild ersetzen (wie kacheln_r41.tim_mit_index)."""
    magic, flag = struct.unpack_from("<II", tb, 0)
    if magic != 0x10:
        raise SystemExit("kein TIM")
    o = 8
    if flag & 8:
        o += struct.unpack_from("<I", tb, o)[0]
    blen, px, py, pw, ph = struct.unpack_from("<IHHHH", tb, o)
    if idx.shape != (ph, pw * 2):
        raise SystemExit("TIM-Groesse %s != %s" % (idx.shape, (ph, pw * 2)))
    return tb[:o + 12] + idx.astype(np.uint8).tobytes() + tb[o + 12 + pw * 2 * ph:]


if __name__ == "__main__":
    main()
