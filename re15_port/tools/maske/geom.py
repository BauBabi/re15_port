"""Geometrie und Tiefe fuer nachgezeichnete Vordergrundmasken — gemeinsame Grundlage.

Diese Datei traegt die MESSERGEBNISSE der Untersuchung vom 2026-09-03
(analysis/esp_masken_2026-09-03/) in eine dauerhafte Form. Jede Konstante hier steht
mit ihrem Beleg; nichts ist geschaetzt.

Kameramathematik = Python-Fassung von re15_camera_build_view (FUN_80053ca4):
LookAt mit GANZZAHLIGER Normalisierung, H = fov>>7, sx = 160 + H*vx/vz.
Die C-Division schneidet gegen null ab — Pythons // rundet ab, deshalb tdiv().
"""
import os
import struct

import numpy as np
from PIL import Image
from scipy import ndimage

# ----------------------------------------------------------------------------
# Gemessene Konstanten
# ----------------------------------------------------------------------------

# Spielweites Maximum der Maskenzahl je Cut (RDT-Header Byte[7]; ROOM3000/3001 Cut 3).
MAX_MASKS_PER_CUT = 105

# Tiefenfaktor auf den geschaetzten Bodenkontakt. DIREKT gegen die Kuenstler-Tiefen
# gemessen (build/tiefenkalib.py): 5151 Original-Maskenrechtecke aus allen Cuts, die
# sowohl Rechtecke als auch einen SLD-Atlas haben; als Region wird die ECHTE
# Kuenstler-Silhouette benutzt, damit nur der Tiefenrechner gemessen wird.
#   Faktor  Medianfehler  |Fehler|<8   zu NAH (verdeckt zu viel)   zu FERN (wirkungslos)
#    0.80      -9.0          15.7 %            50.7 %                    30.9 %
#    0.90      +2.0          20.7 %            35.5 %                    41.8 %
#    1.00     +13.0          17.5 %            24.2 %                    55.8 %
#    1.10     +24.0          12.9 %            16.3 %                    68.5 %
#    1.20     +34.0           7.9 %            12.1 %                    78.9 %
# 0.90 ist das Optimum der direkten Messung: kleinster Medianfehler (+2 von rund 80)
# und hoechster Anteil praktisch richtiger Werte. Groessere Faktoren senken zwar die
# schaedliche Richtung "zu nah", aber nur, indem sie die Maske insgesamt wirkungslos
# machen (bei 1.10 sind schon zwei Drittel zu fern).
# ⛔ Die frueher hier stehende Begruendung war in sich widerspruechlich: sie berief
# sich auf "eine falsche Maske ist schlimmer als eine fehlende" und waehlte dann den
# Wert mit MEHR falscher Verdeckung als 1.00. Der Zahlensatz stammte ausserdem aus
# einem Ersatzmass (nachgebaute Verdeckung), nicht aus den Tiefen selbst.
DEPTH_FACTOR = 0.90

# Bodenhoehe fuer den Kontaktpunkt. y0 = 0 ist die Hauptebene; Raeume mit mehreren
# Stockwerken sind damit nicht erfasst — das ist die bekannte Restschwaeche des
# Tiefenmodells (q3d: Verhaeltnis Ist/Soll 1.08 bei y0=0).
FLOOR_Y = 0


def tdiv(a, b):
    """Ganzzahldivision mit C-Semantik (gegen null abschneiden)."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


def isqrt(n):
    return int(np.sqrt(float(n)))


def build_view(fov, px, py, pz, tx, ty, tz):
    """-> (R[9], t[3], H) oder None. Spiegelt re15_camera_build_view."""
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = isqrt(dx * dx + dy * dy + dz * dz)
    if dist == 0:
        return None
    horiz = isqrt(dx * dx + dz * dz)
    sp = tdiv(-dy * 4096, dist)
    cp = tdiv(horiz * 4096, dist)
    if horiz != 0:
        sy = tdiv(dx * 4096, horiz)
        cy = tdiv(dz * 4096, horiz)
        R = [cy, 0, -sy,
             (sp * sy) >> 12, cp, (sp * cy) >> 12,
             (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    else:
        R = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
    t = [(R[0] * -px + R[1] * -py + R[2] * -pz) >> 12,
         (R[3] * -px + R[4] * -py + R[5] * -pz) >> 12,
         (R[6] * -px + R[7] * -py + R[8] * -pz) >> 12]
    return R, t, (fov >> 7)


def vz_at_floor(R, t, H, sx, sy, y0=FLOOR_Y):
    """Kamera-Z des Punktes, an dem der Sehstrahl durch (sx,sy) die Ebene y=y0 trifft."""
    a = (sx - 160.0) / H
    b = (sy - 120.0) / H
    k = R[1] * a + R[4] * b + R[7]
    c = R[1] * t[0] + R[4] * t[1] + R[7] * t[2]
    if abs(k) < 1e-9:
        return None
    vz = (y0 * 4096.0 + c) / k
    return vz if vz > 1 else None


def cut_view(rdt, cam_off, cut):
    rec = cam_off + cut * 32
    fov = struct.unpack_from("<H", rdt, rec + 2)[0]
    P = struct.unpack_from("<iii", rdt, rec + 4)
    T = struct.unpack_from("<iii", rdt, rec + 0x10)
    return build_view(fov, *P, *T)


def depth_map(rdt, cam_off, cut, region):
    """Tiefenkarte (240x320) aus dem Bodenkontakt JE BILDSPALTE, 0 = kein Vordergrund.

    Je Spalte, nicht je Objekt: ein Tisch, der in die Tiefe laeuft, hat keine
    einheitliche Tiefe (Streuung in einer Kuenstler-Gruppe im Median 24.7 %), und
    zwei benachbarte Objekte verschmelzen leicht zu einer Zusammenhangskomponente —
    dann bekaeme das ferne den Kontakt des nahen.

    Laeuft die Silhouette in einer Spalte unten aus dem Bild, wird der unterste
    Bildpunkt genommen: das Objekt steht dann VOR allem, was in dieser Spalte
    erreichbar ist. (Die frueher benutzte Behandlung "kein Kontakt sichtbar -> sehr
    weit" war falsch herum und machte solche Masken wirkungslos.)
    """
    v = cut_view(rdt, cam_off, cut)
    if not v:
        return None
    R, t, H = v
    if H <= 0:
        return None
    lab, n = ndimage.label(region, structure=np.ones((3, 3)))
    dep = np.zeros((240, 320), np.int32)
    for k in range(1, n + 1):
        m = (lab == k)
        for x in np.where(m.any(0))[0]:
            rows = np.where(m[:, x])[0]
            yb = int(rows.max())
            z = vz_at_floor(R, t, H, x + 0.5, float(min(yb, 239)))
            if not z:
                continue
            dep[rows, x] = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
    return dep


def _largest_rect(mask):
    """Groesstes achsenparalleles Vollrechteck -> (y, x, h, w, flaeche)."""
    h, w = mask.shape
    height = np.zeros(w, np.int32)
    best = (0, 0, 0, 0, 0)
    for y in range(h):
        height = np.where(mask[y], height + 1, 0)
        stack = []
        for x in range(w + 1):
            cur = height[x] if x < w else 0
            start = x
            while stack and stack[-1][1] >= cur:
                sx, sh = stack.pop()
                area = sh * (x - sx)
                if area > best[4]:
                    best = (y - sh + 1, sx, sh, x - sx, area)
                start = sx
            stack.append((start, cur))
    return best


def rects_from_mask(mask, budget, min_area=16, max_kante=40):
    """Region mit Rechtecken ueberdecken -> [(x, y, w, h)].

    ⛔ MAX_KANTE IST DER KERN (Nutzer-Befund 2026-09-03, error2.png: "Leon verschwindet
    fast vollstaendig im Raum"). Die erste Fassung nahm gierig immer das GROESSTE
    Vollrechteck. Ergebnis: wenige, riesige Rechtecke — 26 Stueck fuer 65 % Bildflaeche.
    Jedes bekommt EINE Tiefe. Ueber eine fliehende Wand oder einen langen Tisch
    schwankt die echte Tiefe aber stark, und der eine Wert verdeckt dann alles
    dahinter. Gemessen mit der echten Begehbarkeit der Engine (RE15_POCC_SCAN,
    Zaehlung "figur_verschluckt"):
        ROOM1170 Cut 2, KUENSTLER-Masken   ->  0 %
        ROOM1140 Cut 0, meine (grosse Rechtecke) -> 29 %
        ROOM1130 Cut 0, meine                    -> 43 %
    Die Kuenstler benutzen viele KLEINE Rechtecke mit je eigener Tiefe (ROOM1210 Cut 4:
    77 Rechtecke fuer 54 % Flaeche). Genau das macht die Kantenbegrenzung hier: sie
    zwingt die Zerlegung auf Stuecke, ueber die die Tiefe noch annaehernd konstant ist.

    Grosszuegiges Ueberdecken bleibt ungefaehrlich, weil die Feinmaskierung die
    TRANSPARENZ macht (Palettenindex 0 wird nicht gezeichnet) — das gilt unveraendert.
    """
    m = mask.copy()
    out = []
    while len(out) < budget:
        y, x, hh, ww, area = _largest_rect(m)
        if area < min_area:
            break
        m[y:y + hh, x:x + ww] = False
        # in Stuecke zerlegen, ueber die die Tiefe noch aussagekraeftig ist
        for oy in range(0, hh, max_kante):
            for ox in range(0, ww, max_kante):
                if len(out) >= budget:
                    break
                out.append((int(x + ox), int(y + oy),
                            int(min(max_kante, ww - ox)), int(min(max_kante, hh - oy))))
        if not m.any():
            break
    return out


def pack_section(groups, masks):
    """Bytes im ORIGINAL-Sektionslayout: Header 4 B, Gruppen 8 B, Masken 12 B.

    groups: [(anzahl, anker_dx, anker_dy)]
    masks:  [(src_x, src_y, dst_x, dst_y, w, h, depth)]  — dst als Bildkoordinate;
            gespeichert wird dst - anker als u8, wie das Original es erwartet
            (Bildposition = Gruppenanker + Masken-Byte, @0x8003940c/0x8003941c).
    """
    out = bytearray()
    out += struct.pack("<HH", len(groups), len(masks) & 0xFFFF)
    for (n, dx, dy) in groups:
        # +2 wird vom Original NIE gelesen (der Zeichner setzt CLUT 0x7800 als
        # Immediate @0x80039498); der authorisierte Wert steht trotzdem drin.
        out += struct.pack("<HHhh", n, 0x7800, dx, dy)
    gi = 0
    used = 0
    for (sx, sy, dx, dy, w, h, dep) in masks:
        while gi < len(groups) and used >= groups[gi][0]:
            gi += 1
            used = 0
        ax, ay = (groups[gi][1], groups[gi][2]) if gi < len(groups) else (0, 0)
        used += 1
        out += struct.pack("<BBBBHH", sx & 0xFF, sy & 0xFF,
                           (dx - ax) & 0xFF, (dy - ay) & 0xFF, dep, 0)
        out += struct.pack("<HH", w, h)     # size-Feld High-Nibble 0 = rechteckig
    return bytes(out)


def pack_container(sections, n_cut):
    """Seitendaten-Container "R15M" (s. re15_pri.h). sections: {cut: bytes}."""
    head = 4 + 4 + 4 + 4 * n_cut
    body = b""
    offs = [0] * n_cut
    for c in sorted(sections):
        if c >= n_cut:
            continue
        offs[c] = head + len(body)
        body += sections[c]
    return b"R15M" + struct.pack("<II", 1, n_cut) \
        + struct.pack("<%dI" % n_cut, *offs) + body


# ----------------------------------------------------------------------------
# Asset-Zugriff (hier, damit der Selbsttest ohne tkinter/skimage laeuft)
# ----------------------------------------------------------------------------

def load_rdt(cd, room):
    for st in range(1, 7):
        p = os.path.join(cd, "STAGE%d" % st, "%s.RDT" % room)
        if os.path.exists(p):
            return open(p, "rb").read(), st
    return None, 0


def load_bg(ppm_dir, room_id, cut):
    """Hintergrund eines Cuts aus dem Abzug von probe_bg_dump."""
    p = os.path.join(ppm_dir, "ROOM%03X%02d.ppm" % (room_id >> 4, cut))
    if not os.path.exists(p):
        return None
    return np.asarray(Image.open(p).convert("RGB"), np.uint8)


def original_has_masks(rdt, cam, cut):
    """Traegt das ORIGINAL fuer diesen Cut Masken? Dann wird er nicht angefasst."""
    po = struct.unpack_from("<I", rdt, cam + cut * 32 + 0x1C)[0]
    if po + 4 > len(rdt):
        return False
    gc, mc = struct.unpack_from("<HH", rdt, po)
    return not (gc == 0xFFFF or gc == 0 or mc == 0 or gc > 256)


def depth_map_objekt(rdt, cam_off, cut, region, fuss=None, ebene=0):
    """Tiefenkarte EINES Objekts.

    fuss=None : wie bisher je Bildspalte aus dem untersten Punkt der Silhouette.
                Richtig fuer Gegenstaende, deren Silhouette unten wirklich den Boden
                beruehrt und die in die Tiefe laufen (Tisch, Schrank, Pult).
    ebene=y0  : Hoehe der Ebene, mit der der Sehstrahl geschnitten wird (0 = Boden,
                negativ = darueber; PSX-Y zeigt nach unten).
                ⛔ WARUM DAS NOETIG IST (Nutzer-Befund 2026-09-04, "an der Ecke sieht es
                noch schlecht aus"): die untere Silhouettenkante eines TISCHES liegt
                nicht auf dem Boden, sondern auf der TISCHPLATTE. Schneidet man ihren
                Sehstrahl trotzdem mit dem Boden, landet der Punkt DAHINTER und die
                Maske wird zu fern. Gemessen an ROOM1140 Cut 0: Leons Bodenschatten bei
                Bild (287,208) hat vz 5495, verdeckt wird also nur mit Tiefe < 85.9;
                die Bodenebene ergab an der Tischecke (288,175) Tiefe 97 — der Schatten
                wurde ueber die Tischkante gemalt. Mit der Ebene -700 sind es 79.5.
                -700 ist zugleich der kleinste Wert mit Abstand zur Schwelle (bei -400
                waeren es 87, immer noch zu fern) und passt zur Groessenordnung: der
                Spieler ist 1500 hoch, ein Konferenztisch also knapp die Haelfte.
    fuss=y    : Bildzeile des Bodenkontakts. Fuer Gegenstaende auf duennen Beinen oder
                Stangen (Fahne, Stativ, Mikrofonstaender, Stuhl), deren Silhouette den
                Kontakt NICHT zeigt. Die Tiefe gilt dann fuer das ganze Objekt und wird
                in der Spalte des Objektschwerpunkts gemessen — ein senkrecht stehender
                Gegenstand hat genau EINE Entfernung.
                Beleg fuer die Notwendigkeit: ROOM1140 Cut 3, Spalte x=215 — Maske endet
                bei y=143 (Tiefe 126, vz~9000), der Fahnenteller steht bei y=178
                (vz 7401, Tiefe ~104). Leon bei vz 7981 wurde deshalb nicht verdeckt.
    """
    v = cut_view(rdt, cam_off, cut)
    if not v:
        return None
    R, t, H = v
    if H <= 0:
        return None
    dep = np.zeros((240, 320), np.int32)
    if fuss is not None:
        xs = np.nonzero(region.any(0))[0]
        if len(xs) == 0:
            return None
        cx = float(xs.mean()) + 0.5
        z = vz_at_floor(R, t, H, cx, float(min(int(fuss), 239)), ebene)
        if not z:
            return None
        dep[region] = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
        return dep
    for x in np.where(region.any(0))[0]:
        rows = np.where(region[:, x])[0]
        yb = int(rows.max())
        z = vz_at_floor(R, t, H, x + 0.5, float(min(yb, 239)), ebene)
        if not z:
            continue
        dep[rows, x] = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
    return dep


def rects_gitter(region, budget=None, kapazitaet=256 * 256, kanten=(8, 10, 12, 16, 20, 24, 32, 40, 48, 64)):
    """Region mit einem GITTER aus Rechtecken ueberdecken — vollstaendig, ohne Treppe.

    ⛔ WARUM (Nutzer-Befund 2026-09-04, fehler/error.png: "sonst koennte es nicht zu den
    Ueberblendungen kommen"): rects_from_mask nahm gierig immer das groesste Rechteck,
    das GANZ INNERHALB der Region liegt. An einer schraegen Kante bleibt dabei
    zwangslaeufig ein Saum aus duennen Dreiecken uebrig, den kein volles Rechteck mehr
    fasst — er fiel unter min_area bzw. aus dem Budget und wurde NICHT gezeichnet.
    Ergebnis im Spiel: die Tischkante war eine grobe Treppe aus 8-10 Bildpunkten,
    obwohl die Freistellung des Nutzers eine glatte Diagonale ist.

    Ein Rechteck DARF ueber die Region hinausragen: die Feinmaskierung macht der Atlas
    (Palettenindex 0 wird nicht gezeichnet). Deshalb wird hier einfach der umschliessende
    Kasten gekachelt und jede Kachel behalten, die Regionpixel enthaelt — damit ist JEDER
    Regionpunkt gedeckt und die Kante ist exakt die der Freistellung.

    Die Kachelgroesse wird so gewaehlt, dass die Kachelzahl ins Maskenbudget passt
    (RDT-Header Byte[7], spielweit hoechstens 105) und die Summe der Kachelflaechen in
    das 256x256-Atlasblatt.
    """
    budget = budget or MAX_MASKS_PER_CUT
    ys, xs = np.nonzero(region)
    if len(ys) == 0:
        return []
    x0, x1 = int(xs.min()), int(xs.max()) + 1
    y0, y1 = int(ys.min()), int(ys.max()) + 1
    best = None
    for k in kanten:
        boxes = []
        flaeche = 0
        for gy in range(y0, y1, k):
            for gx in range(x0, x1, k):
                w = min(k, x1 - gx)
                h = min(k, y1 - gy)
                if region[gy:gy + h, gx:gx + w].any():
                    boxes.append((gx, gy, w, h))
                    flaeche += w * h
        if not boxes:
            continue
        if len(boxes) <= budget and flaeche <= kapazitaet:
            # kleinste Flaeche gewinnt = engste Ueberdeckung
            if best is None or flaeche < best[0]:
                best = (flaeche, boxes)
    if best is None:
        # Notnagel: groebste Kachelung, damit ueberhaupt etwas gedeckt ist
        k = kanten[-1]
        boxes = []
        for gy in range(y0, y1, k):
            for gx in range(x0, x1, k):
                w = min(k, x1 - gx); h = min(k, y1 - gy)
                if region[gy:gy + h, gx:gx + w].any():
                    boxes.append((gx, gy, w, h))
        return boxes[:budget]
    return best[1]
