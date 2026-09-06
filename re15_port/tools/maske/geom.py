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


def welt_am_boden(R, t, H, sx, sy, y0=FLOOR_Y):
    """-> (wx, wz) des Punktes, an dem der Sehstrahl durch (sx,sy) die Ebene y=y0
    trifft. Das ist der STANDPUNKT eines Gegenstands, dessen Fuss dort im Bild liegt."""
    vz = vz_at_floor(R, t, H, sx, sy, y0)
    if vz is None:
        return None
    vx = (sx - 160.0) * vz / H
    vy = (sy - 120.0) * vz / H
    ax, ay, az = vx - t[0], vy - t[1], vz - t[2]
    wx = (R[0] * ax + R[3] * ay + R[6] * az) / 4096.0
    wz = (R[2] * ax + R[5] * ay + R[8] * az) / 4096.0
    return wx, wz


def vz_der_senkrechten(R, t, H, wx, wz, sy):
    """Kamera-Z der SENKRECHTEN Weltlinie durch (wx, ?, wz) in der Bildzeile sy.

    ⛔ WOZU (Nutzer-Befund 2026-09-07, ROOM10E0 Cut 3, Drehstuhl): "der Drehstuhl muss
    den Charakter weiter oben ueberdecken, aber weiter unten vom Charakter ueberdeckt
    werden". Mit EINER Tiefe je Objekt geht das nicht - und zwar genau andersherum:
    die Kamera schaut von oben, also ist der KOPF naeher als die Fuesse (gemessen 469
    Einheiten Unterschied). Eine einzelne Schwelle mitten in der Figur verdeckt deshalb
    immer das UNTERE Ende und laesst das obere frei. Der Nutzer will das Gegenteil, und
    das ist auch das physikalisch Richtige: die Rueckenlehne des Stuhls steht HOCH und
    ist damit NAEHER an der Kamera als sein Fussgestell.
    Ein aufrecht stehender Gegenstand hat also keine Tiefe, sondern ein Tiefen-PROFIL
    ueber die Bildzeilen - und das ist geschlossen ausrechenbar, weil seine Silhouette
    die Projektion einer SENKRECHTEN Weltlinie ist:
        vy = a*y + b   mit a = R4/4096, b = (R3*wx + R5*wz)/4096 + t1
        vz = c*y + d   mit c = R7/4096, d = (R6*wx + R8*wz)/4096 + t2
        sy - 120 = H * vy / vz
      ->  y  = (H*b - (sy-120)*d) / ((sy-120)*c - H*a)
      ->  vz = c*y + d
    Gegenprobe eingebaut: fuer sy = die Bildzeile des Bodenpunktes muss vz gleich
    vz_at_floor sein (geom.selbsttest)."""
    a = R[4] / 4096.0
    b = (R[3] * wx + R[5] * wz) / 4096.0 + t[1]
    c = R[7] / 4096.0
    d = (R[6] * wx + R[8] * wz) / 4096.0 + t[2]
    n = (sy - 120.0) * c - H * a
    if abs(n) < 1e-9:
        return None
    y = (H * b - (sy - 120.0) * d) / n
    vz = c * y + d
    return vz if vz > 1 else None


# ----------------------------------------------------------------------------
# BODENEBENEN EINES RAUMS (Kollisionsdaten)
# ----------------------------------------------------------------------------
# ⛔ NUTZER-BEFUND 2026-09-05 (fehler/error03.png, error04.png): "Die Gelaender
# scheinen mich gleich mal garnicht zu ueberdecken." GEMESSEN im laufenden Spiel
# (RE15_PRI_LOG, ROOM1060 Cut 0):
#     Spieler (20268,-14400,26482) | Figur-Tiefe 4800..6862 | 81 Masken Tiefe 265..414
#     (Schwelle 16960..26496) | verdeckend 0
# Die Masken lagen zwei- bis viermal zu weit weg. Ursache ist FLOOR_Y = 0: ROOM1060
# ist ein TREPPENHAUS und traegt fuenf Bodenebenen. Das Podest, auf dem der Spieler
# steht, liegt bei y = -14400; schneidet man den Sehstrahl trotzdem mit y = 0, landet
# der Punkt weit dahinter.
#
# DIE EBENEN STEHEN IN DEN DATEN, sie werden nicht geschaetzt: jede SCA-Zelle traegt
# ein floor-Byte, das Band ist floor>>4, und die Hoehe eines Bandes ist
#     y = -band * 0x708
# Das ist die Umkehrung von re15_collision_band_from_y (engine: "Convert a spawn Y to
# a floor band, as the original does: -(Y / 0x708)"). Gegenprobe: ROOM1060 fuehrt die
# Baender 0,2,4,6,8 -> y = 0,-3600,-7200,-10800,-14400, und der gemessene Spieler-Y auf
# dem Podest ist exakt -14400.
#     ROOM1060 175 Zellen, Baender {0,2,4,6,8}
#     ROOM1120 195 Zellen, Baender {0,1,2}
#     ROOM1130  55 Zellen, Band {0}      ROOM1140 95 Zellen, Band {0}
# (Die beiden letzten sind einetagig - ihre bereits geprueften Masken aendern sich
# durch die Bandwahl also nicht.)
BAND_HOEHE = 0x708          # engine/include/re15_collision.h: band = -(Y / 0x708)


def sca_zellen(rdt):
    """SCA-Zellen (x, z, breite, tiefe, band) aus dem RDT (+0x20, Eintrag 'collision').

    Layout 12 B je Zelle (lib_sca v1.5): u16 width, u16 density, s16 x, s16 z,
    u8 type, u8, u8, u8 floor — dasselbe, was tools/gen_map_zones.py liest.
    """
    try:
        # Offset-Tabelle @0x20; Eintrag 0 = collision (Reihenfolge wie
        # tools/gen_map_zones.py NAMES: collision, camera, zone, light, ...).
        s = struct.unpack_from("<I", rdt, 0x20)[0]
    except struct.error:
        return []
    if not (0 < s < len(rdt) - 24):
        return []
    counts = struct.unpack_from("<5I", rdt, s + 4)
    n = sum(counts)
    if not (0 < n < 2000) or s + 24 + 12 * n > len(rdt):
        return []
    aus = []
    for i in range(n):
        w, dep, x, z = struct.unpack_from("<HHhh", rdt, s + 24 + 12 * i)
        flr = rdt[s + 24 + 12 * i + 11]
        aus.append((x, z, w, dep, flr >> 4))
    return aus


def boden_ebenen(rdt):
    """Bodenebenen des Raums, NAH ZUERST: [(y0, [(x,z,w,d), ...]), ...].

    Nah zuerst heisst: das hoechste Band zuerst (y am staerksten negativ). Ein
    Sehstrahl, der von oben kommt, trifft die obere Ebene frueher - und ein Objekt
    steht auf der obersten Flaeche, die es unter sich hat.
    """
    nach_band = {}
    for (x, z, w, d, band) in sca_zellen(rdt):
        nach_band.setdefault(band, []).append((x, z, w, d))
    return [(-band * BAND_HOEHE, nach_band[band]) for band in sorted(nach_band, reverse=True)]


def _welt_von_sicht(R, t, vx, vy, vz):
    """Sichtkoordinaten -> Weltkoordinaten. R ist orthonormal mal 4096, also R^T/4096."""
    ax, ay, az = vx - t[0], vy - t[1], vz - t[2]
    wx = (R[0] * ax + R[3] * ay + R[6] * az) / 4096.0
    wy = (R[1] * ax + R[4] * ay + R[7] * az) / 4096.0
    wz = (R[2] * ax + R[5] * ay + R[8] * az) / 4096.0
    return wx, wy, wz


# ⛔ ZWEI NAHELIEGENDE ABLEITUNGEN - BEIDE GEMESSEN UND VERWORFEN.
# Die Ebene eines Cuts laesst sich NICHT aus den Daten erschliessen:
#
#   1. "Die Zelle sagt es": pro Bildspalte den Sehstrahl mit jeder Bandebene
#      schneiden und die naechste nehmen, deren Treffer in einer Zelle DIESES Bandes
#      liegt. Faellt aus, weil die Baender eines Raums DIESELBEN Zellen tragen:
#          ROOM1060  y=-14400 gegen y=-10800/-7200/-3600: Zellen IDENTISCH (6 von 6)
#          ROOM1120  alle drei Baender identisch (13 von 13)
#      In ROOM1120 waehlte die Regel fuer 9 von 55 Spalten y=-1800, obwohl der
#      Spieler dort gemessen auf y=0 steht.
#   2. "Die Kamera sagt es": das Blickziel des Kamerasatzes (RDT +0x10) auf das
#      naechste Band runden. Trifft in ROOM1060 jedes Band auf 226 Einheiten genau
#      (Cut 0 Ziel -14174 -> -14400), scheitert aber in ROOM1120: Cut 0 Ziel -2739
#      -> -3600, gemessen ist der Boden dort 0. Die Zielhoehe liegt je nach Cut
#      ueber ODER unter dem Boden (ROOM1140 Cut 3: +7224 bei Boden 0).
#
# Was bleibt, ist die MESSUNG im laufenden Spiel: die Hoehe, auf der der Spieler in
# diesem Cut steht (RE15_PRI_LOG -> "Spieler (x,y,z)"). Sie steht als "ebene" je Cut
# in analysis/esp_masken_2026-09-03/auswahl.json, mit dem Messwert als Beleg.
# boden_ebenen() bleibt als PRUEFUNG: hat ein Raum mehr als ein Band und fehlt die
# Angabe, bricht raum.py ab, statt stillschweigend y=0 zu nehmen - genau das war der
# Fehler, der die Gelaender in ROOM1060 wirkungslos machte.


_BEGEHBAR = {}


def begehbare_baender(rid):
    """Baender, auf denen der Spieler in diesem Raum ueberhaupt STEHEN kann.

    ⛔ WARUM DIE ROHEN KOLLISIONSBAENDER NICHT REICHEN: die Baender eines Raums tragen
    DIESELBEN Zellen (ROOM1060 y=-14400 gegen -10800/-7200/-3600: 6 von 6 identisch;
    ROOM1120 alle drei: 13 von 13). Ein Raum kann also Baender fuehren, die nie jemand
    betritt - ROOM1120 hat drei, gemessen steht der Spieler dort aber in JEDEM Cut auf
    y=0.

    Zwei Quellen sagen, welche Baender wirklich vorkommen, beide aus dem Original:
      * TUEREN: jeder Door_aot_set-Record traegt die Spawn-Position im ZIELraum; die
        Hoehe ist band*0x708 (engine re15_collision_band_from_y). Gemessen:
            ROOM1060 <- ROOM1040 y=0 | <- ROOM10C0 y=-7200 | <- ROOM1120 y=-14400
            ROOM1120 <- ROOM1060 y=0 | <- ROOM1130 y=0
      * TREPPEN: jeder Treppen-Record nennt sein Band (rec+0xC). ROOM1060 fuehrt acht
        Records auf den Baendern 8,6,4,2 - genau die Zwischenpodeste, die keine Tuer
        erreicht. ROOM1120/1130/1140 fuehren keinen einzigen.
    Zusammen: ROOM1060 {0,2,4,6,8}, ROOM1120 {0}, ROOM1130 {0}, ROOM1140 {0}.
    """
    if rid in _BEGEHBAR:
        return _BEGEHBAR[rid]
    import glob
    hier = os.path.dirname(os.path.abspath(__file__))
    wurzel = os.path.abspath(os.path.join(hier, '..', '..', '..'))
    gen = os.path.join(wurzel, 're15_port', 'tools', 'gen_map_zones.py')
    ns = {'__name__': 'geom_baender', '__file__': gen}
    src = open(gen, encoding='utf-8').read()
    exec(compile(src[:src.index('def main(')], 'gen', 'exec'), ns)
    read_rdt = ns['read_rdt']
    baender = set()
    got = read_rdt(rid) or read_rdt(rid + 1)
    if got:
        for st in got[2]:
            baender.add(int(st['band']))
    muster = os.path.join(wurzel, 're15_port', 'shared_assets', 'PSX', 'STAGE*', 'ROOM*.RDT')
    for pfad in glob.glob(muster):
        andere = int(os.path.basename(pfad)[4:8], 16)
        if andere & 1:
            continue
        g2 = read_rdt(andere)
        if not g2:
            continue
        for d in g2[1]:
            if d['dest'] == (rid & 0xFFF0):
                baender.add(int(round(-d['ny'] / float(BAND_HOEHE))))
    _BEGEHBAR[rid] = baender
    return baender


def ebene_aus_kamera(rdt, cam_off, cut, rid=None):
    """-> (y0, guete) oder None. y0 = Bodenebene dieses Cuts, guete = Uneindeutigkeit.

    Der Kamerasatz eines Cuts traegt seinen BLICKZIELPUNKT (RDT +0x10). Er liegt in der
    Naehe des Bodens, auf dem der Spieler in dieser Ansicht steht - aber nicht exakt
    darauf (mal darueber, mal darunter). Brauchbar ist er deshalb nur, wenn er
    EINDEUTIG auf ein Band faellt:

        guete = Abstand zum naechsten Band / Bandabstand

    GEMESSEN:
        ROOM1060 (Baender 0,-3600,...,-14400, Abstand 3600)
            Cut 0 Ziel -14174 -> -14400  guete 0.06   <- im Spiel gemessen: -14400 ✓
            Cut 1 Ziel -10631 -> -10800  guete 0.05
            Cut 2 Ziel  -7088 ->  -7200  guete 0.03
            Cut 4 Ziel     -1 ->      0  guete 0.00
        ROOM1120 (Baender 0,-1800,-3600, Abstand 1800)
            Cut 0 Ziel  -2739 ->  -3600  guete 0.48   <- im Spiel gemessen: 0 ✗
            Cut 2 Ziel  -1373 ->  -1800  guete 0.24   <- im Spiel gemessen: 0 ✗

    Ueber der Schwelle GUETE_MAX ist die Angabe wertlos; raum.py bricht dann ab und
    verlangt den gemessenen Wert in auswahl.json, statt stillschweigend y=0 zu nehmen.
    Bei nur EINEM Band ist nichts zu entscheiden - dann gilt dieses Band.
    """
    ebenen = [y for y, _ in boden_ebenen(rdt)]
    if rid is not None:
        ok = begehbare_baender(rid)
        gefiltert = [y for y in ebenen if int(round(-y / float(BAND_HOEHE))) in ok]
        if gefiltert:
            ebenen = gefiltert
    if not ebenen:
        return None
    if len(ebenen) == 1:
        return ebenen[0], 0.0
    rec = cam_off + cut * 32
    if rec + 0x1C > len(rdt):
        return None
    ty = struct.unpack_from("<i", rdt, rec + 0x14)[0]
    if abs(ty) > 1 << 22:          # unbenutzter Kamerasatz (Mustermuell)
        return None
    schritt = min(abs(a - b) for a in ebenen for b in ebenen if a != b)
    nah = min(ebenen, key=lambda y: abs(y - ty))
    return nah, abs(nah - ty) / float(schritt)


GUETE_MAX = 0.20    # s. ebene_aus_kamera: 0.06 (ROOM1060, richtig) gegen 0.24/0.48
                    # (ROOM1120, falsch). Dazwischen liegt keine gemessene Zahl.


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


def _bereiche(xs):
    """[1,2,3,7,8] -> "1-3, 7-8" — kompakte Spaltenliste fuer Meldungen."""
    aus, i = [], 0
    while i < len(xs):
        j = i
        while j + 1 < len(xs) and xs[j + 1] == xs[j] + 1:
            j += 1
        aus.append(str(xs[i]) if j == i else "%d-%d" % (xs[i], xs[j]))
        i = j + 1
    return ", ".join(aus)


def depth_map_objekt(rdt, cam_off, cut, region, fuss=None, ebene=None,
                     bodenkante=None, bericht=None, aufrecht=None):
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
    bodenkante=(x0, x1)
              : Bildspalten, in denen die UNTERE SILHOUETTENKANTE WIRKLICH der
                Bodenkontakt ist. Alle uebrigen Spalten erben die Tiefe der naechsten
                Spalte darin.
                ⛔ WARUM (Nutzer-Befund 2026-09-06, F9-Marke ROOM1130 Cut 3: "wenn Leon
                rennt, sieht man noch seinen Arm durchblitzen"): bei einer WAND ist die
                untere Silhouettenkante nur auf einem TEIL der Breite der Bodenkontakt.
                Links davon ist sie die seitliche Kante des Pfeilers - eine SENKRECHTE
                Weltkante, die im Bild steil ansteigt. Die Spaltenregel liest sie
                trotzdem als Bodenkontakt und bekommt absurde Entfernungen.
                Gemessen an ROOM1130 Cut 3 (Freistellung 49x202 bei x=67), Spalte ->
                Tiefe: 94..115 ergeben 70..83 und ihre Weltpunkte liegen alle auf
                EINER Geraden x = -3734..-3946, z = 4231..5189 (das ist der Wandfuss);
                89 ergibt 94, 84 -> 141, 78 -> 345, 76 -> 684, 74 -> 32523, und
                67..73 treffen den Boden ueberhaupt nicht mehr (Sehstrahl ueber dem
                Horizont) - diese sieben Spalten liess das Werkzeug bisher STILL leer.
                Der Spieler stand laut Log bei vz 6398/6166/5935; verdeckt wird nur mit
                Tiefe < 92,7. Die Spalten 67..89 lagen alle darueber, deshalb wurde
                das Messer ueber die Wand gezeichnet.
                ⛔ Die Grenze ist NICHT automatisch zu finden: die Raumhuelle
                (x -8650..6000, z -18400..19750) verwirft nur die Spalten 74..78, und
                der Test "Weltpunkt liegt auf einer SCA-Bodenzelle" laesst 81, 82 und
                90 faelschlich durch. Sie wird deshalb wie ein Messpunkt EINGETRAGEN.
    """
    v = cut_view(rdt, cam_off, cut)
    if not v:
        return None
    R, t, H = v
    if H <= 0:
        return None
    y0 = 0 if ebene is None else ebene

    def _z(sx, sy):
        return vz_at_floor(R, t, H, sx, sy, y0)

    dep = np.zeros((240, 320), np.int32)
    if aufrecht is not None:
        # ---- AUFRECHT STEHENDER GEGENSTAND: Tiefe JE BILDZEILE ------------------
        # Ein Stuhl, ein Stativ, ein Pfosten steht senkrecht. Seine Silhouette ist die
        # Projektion einer SENKRECHTEN Weltlinie, und deren Kamera-Tiefe faellt mit der
        # Hoehe: was oben im Bild liegt, ist NAEHER an der Kamera. Genau das verlangt
        # der Nutzer (2026-09-07, ROOM10E0 Cut 3): "der Drehstuhl muss den Charakter
        # weiter oben ueberdecken, aber weiter unten vom Charakter ueberdeckt werden".
        # Mit EINER Tiefe ist das unmoeglich - sie verdeckt immer das UNTERE Ende der
        # Figur (deren Fuesse sind ferner als ihr Kopf) und laesst das obere frei.
        xs = np.nonzero(region.any(0))[0]
        ys = np.nonzero(region.any(1))[0]
        if len(xs) == 0 or len(ys) == 0:
            return None
        if aufrecht == "spalten":
            # ---- SENKRECHTE FLAECHE (Wand): je Spalte ihr eigener Standpunkt -----
            # Eine Wand hat BEIDES: sie laeuft in die Tiefe (das ist die Spaltenregel)
            # UND sie steht senkrecht (das ist das Zeilenprofil). Die Spaltenregel
            # allein gibt JEDER Zeile die Tiefe des BODENS - fuer die Oberkante der
            # Wand ist das zu fern, und was davor an der Wand entlanglaeuft, blitzt
            # oben durch. Hier bekommt jeder Bildpunkt die Tiefe der Wandflaeche.
            n_ok = 0; ohne = []
            gmin = 10 ** 9; gmax = 0
            # ⛔ ERST ALLE STANDPUNKTE, DANN ZEICHNEN. Ueber einer TUEROEFFNUNG hat die
            # Wand keinen Bodenkontakt - dort steht nur der Sturz. Diese Spalten blieben
            # sonst still leer (dieselbe Klasse wie ROOM1130, s. "bodenkante"); sie
            # erben den Standpunkt der naechsten Spalte, die einen hat. Das ist keine
            # Naeherung: es ist DIESELBE Wandflaeche.
            # ⛔ "bodenkante" gilt AUCH HIER, und sie ist noetig: eine Spalte kann
            # einen Bodenpunkt liefern und trotzdem den falschen. In ROOM1130 Cut 3
            # gemessen - ohne die Einschraenkung ergeben die Spalten 74..88 (dort ist
            # die untere Kante die SEITE des Pfeilers, nicht der Boden) Tiefen bis 1023,
            # und fuenf Spalten auf Koerperhoehe blieben wirkungslos.
            bx0, bx1 = (int(bodenkante[0]), int(bodenkante[1]))                 if bodenkante is not None else (-10 ** 9, 10 ** 9)
            # ⛔ EIN FUSSPUNKT AUSSERHALB DES RAUMS IST KEIN FUSSPUNKT.
            # Ueber einer Tueroeffnung endet die Maske am Sturz; die Bodenregel
            # verlaengert den Sehstrahl dann bis weit hinter die Raumwand. Das ist
            # nachpruefbar und braucht keine gewaehlte Schranke: die Huelle der
            # SCA-Kollisionszellen ist die Ausdehnung des Raums.
            # Gemessen an ROOM10E0 (2026-09-07): verwirft bei 07_02 genau die 38
            # Spalten ueber der Tuer (196..231 und zwei am Rand) und bei 00_01, 01,
            # 03, 06 KEINE einzige - also keine Fehlalarme.
            _zx = []; _zz = []
            for (_cx, _cz, _cw, _cd, _b) in sca_zellen(rdt):
                _zx += [_cx, _cx + _cw]; _zz += [_cz, _cz + _cd]
            _hx0, _hx1 = (min(_zx), max(_zx)) if _zx else (-10 ** 9, 10 ** 9)
            _hz0, _hz1 = (min(_zz), max(_zz)) if _zz else (-10 ** 9, 10 ** 9)
            stand = {}
            for x in xs:
                rr = np.nonzero(region[:, x])[0]
                if len(rr) == 0:
                    continue
                if not (bx0 <= int(x) <= bx1):
                    ohne.append(int(x)); continue
                Px = welt_am_boden(R, t, H, float(x) + 0.5,
                                   float(min(int(rr.max()), 239)), y0)
                if Px is None or not (_hx0 <= Px[0] <= _hx1 and _hz0 <= Px[1] <= _hz1):
                    ohne.append(int(x))
                else:
                    stand[int(x)] = Px
            if not stand:
                return None
            _mit = sorted(stand)
            for x in ohne:
                stand[x] = stand[min(_mit, key=lambda g: abs(g - x))]
            for x in xs:
                rr = np.nonzero(region[:, x])[0]
                if len(rr) == 0 or int(x) not in stand:
                    continue
                Px = stand[int(x)]
                for y in rr:
                    z = vz_der_senkrechten(R, t, H, Px[0], Px[1], float(y) + 0.5)
                    if not z:
                        continue
                    v = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
                    dep[y, x] = v
                    gmin = min(gmin, v); gmax = max(gmax, v)
                n_ok += 1
            if bericht is not None:
                bericht.append("aufrecht/spalten: %d Spalten, Tiefe %d..%d%s"
                               % (n_ok, gmin if n_ok else 0, gmax,
                                  "" if not ohne else
                                  "; %d Spalten ohne eigenen Bodenkontakt (geerbt): %s"
                                  % (len(ohne), _bereiche(sorted(ohne)))))
            return dep if n_ok else None
        cx = float(xs.mean()) + 0.5
        yb = float(min(int(ys.max()) if aufrecht is True else int(aufrecht), 239))
        P = welt_am_boden(R, t, H, cx, yb, y0)
        if P is None:
            if bericht is not None:
                bericht.append("aufrecht: Fusspunkt (%.0f,%.0f) trifft die Ebene "
                               "y=%d nicht" % (cx, yb, y0))
            return None
        wx, wz = P
        n_ok = 0
        for y in ys:
            z = vz_der_senkrechten(R, t, H, wx, wz, float(y) + 0.5)
            if not z:
                continue
            dep[y, region[y]] = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
            n_ok += 1
        if bericht is not None:
            gut = dep[region]
            gut = gut[gut > 0]
            bericht.append("aufrecht: Standpunkt Welt(%.0f,%.0f) aus Bild(%.0f,%.0f); "
                           "Tiefe %d (oben) bis %d (unten) ueber %d von %d Zeilen"
                           % (wx, wz, cx, yb,
                              int(gut.min()) if len(gut) else 0,
                              int(gut.max()) if len(gut) else 0, n_ok, len(ys)))
        return dep if n_ok else None
    if fuss is not None:
        xs = np.nonzero(region.any(0))[0]
        if len(xs) == 0:
            return None
        cx = float(xs.mean()) + 0.5
        z = _z(cx, float(min(int(fuss), 239)))
        if not z:
            return None
        dep[region] = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
        return dep
    spalten = list(np.where(region.any(0))[0])
    tiefen = {}
    for x in spalten:
        rows = np.where(region[:, x])[0]
        yb = int(rows.max())
        z = _z(x + 0.5, float(min(yb, 239)))
        if z:
            tiefen[x] = max(1, min(1023, int(z * DEPTH_FACTOR / 64.0)))
    # ⛔ ABDECKUNG MELDEN. Spalten, deren Sehstrahl den Boden gar nicht trifft, blieben
    # frueher STILL leer — in ROOM1130 Cut 3 waren das sieben von 49, und genau dort
    # blitzte der Arm durch. Eine Messschiene, die ihre Abdeckung nicht ausgibt, sieht
    # den Fall nie ([[reai-v2-schiene-abdeckung]]).
    if bericht is not None:
        ohne = [int(x) for x in spalten if x not in tiefen]
        if spalten:
            gr = max(tiefen.values()) if tiefen else 0
            bericht.append("%d von %d Spalten mit Bodenkontakt, groesste Tiefe %d%s"
                           % (len(tiefen), len(spalten), gr,
                              "" if not ohne else
                              "; OHNE Bodenkontakt: %s" % _bereiche(ohne)))
    if bodenkante is not None:
        # Nur die angegebenen Spalten tragen ihren eigenen Bodenkontakt; alle
        # anderen erben den der naechstgelegenen davon.
        bx0, bx1 = int(bodenkante[0]), int(bodenkante[1])
        gueltig = sorted(x for x in tiefen if bx0 <= x <= bx1)
        if not gueltig:
            return None
        for x in spalten:
            if bx0 <= x <= bx1 and x in tiefen:
                continue
            tiefen[x] = tiefen[min(gueltig, key=lambda g: abs(g - x))]
    for x in spalten:
        if x not in tiefen:
            continue
        dep[np.where(region[:, x])[0], x] = tiefen[x]
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
