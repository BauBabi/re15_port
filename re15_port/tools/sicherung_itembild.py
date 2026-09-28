#!/usr/bin/env python3
"""WEG 2 (Runde 30, Thema H): Item-Bild + Inventar-Icon der Sicherung AUS DEM WELT-MODELL.

Der Spieler findet im Hebetisch das Welt-Modell (gen/sicherung_prop.inc), bekommt im
Aufnahme-Modal aber das ausgelieferte Item-Bild 0x40 zu sehen — und das zeigt einen ANDEREN
Gegenstand (dunkler Sechskant mit Messingstift; byte-gleich RE2 "Fuse Case", s. Dossier
analysis/befunde_runde30/sicherung.md §3.4). Dieses Werkzeug rendert stattdessen GENAU das
Modell, das im Spiel liegt, in die beiden Bildformate des Inventars.

Ohne --inc schreibt das Werkzeug NUR nach --ziel (Standard build/r30_sicherung/). Mit
--inc re15_port/engine/src/gen/sicherung_itembild.inc entsteht die C-Einbindung, die
engine/src/sicherung_itembild.c in den GELADENEN Puffer einsetzt. shared_assets/ fasst
das Werkzeug nie an (es liest dort nur).

REIHENFOLGE: erst tools/sicherung_engine_export.py (Modell), dann dieses Werkzeug — es
rendert aus gen/sicherung_prop.inc. Abnahme bytegenau:
analysis/befunde_runde30/sicherung_werkzeug/bau_bytevergleich.py.

WAS BELEGT IST UND WAS NICHT
  BELEGT   Geometrie + Textur       = die eingebackenen Bytes re15_sicherung_md1/_tim,
                                      also exakt das, was der Renderer im Raum zeichnet
           Bildformat Item-Bild     = TIM 8bpp+CLUT, 112x72, CLUT @+0x14, Bild @+0x220,
                                      Blocklaenge 0x3000 (engine/src/itps_common.c:16-20)
           Kopf-Rechtecke           = CLUT (0,489) 256x1 / Bild (832,256) 56x72 — die Werte
                                      der 53 Bloecke, deren CHECK-Foto der Port anzeigt
                                      (platform/pc/src/inv_render_pc.c:378-400)
           Rahmen + Hintergrundblau = Pixel fuer Pixel aus dem ausgelieferten Block 0x40
           Icon-Format              = 40x30, 1 Byte je Pixel = Index in ST_00.TIM CLUT-Zeile 0
                                      (platform/pc/src/inv_render_pc.c:258-273, 350-352)
           Lage/Groesse im Bild     = Hauptachse und Laenge des Gegenstands im ausgelieferten
                                      Bild 0x40 (gemessen: Achse 147.4 Grad, 83.9 px / Icon 36.4 px)
           Hintergrund              = Item-Bild: Rahmen 2 px + flaches Wort 0x1C00; Icon: Verlauf
                                      aus den Indizes 0xE3/0xE4/0xE6 (haeufigster je Bildpunkt
                                      ueber alle 72 Tiles)
  PORT-WAHL, KEINE ORIGINAL-ADRESSE
           Blickwinkel-Neigung (KIPP_GRAD), Lichtrichtung (LICHT), Umgebung/Streuung
           (UMGEBUNG/STREU). Ein Item-Bild der Rohr-Sicherung hat es nie gegeben; diese vier
           Groessen sind reine Darstellung. Hinter ihnen steht KEINE Messung und keine
           Instruktion — sie sind so gewaehlt, dass das Rohr im Bild wie der alte Gegenstand
           schraeg liegt und plastisch wirkt.

    python re15_port/tools/sicherung_itembild.py [--ziel build/r30_sicherung]
"""
import argparse
import math
import os
import re
import struct

import numpy as np
from PIL import Image

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "sicherung_prop.inc")

ITEM = 0x40
ITPS_BLOCK = 0x3000          # itps_common.c:16  ITPS_STRIDE
ITPS_CLUT = 0x14             # itps_common.c:18
ITPS_BILD = 0x220            # itps_common.c:19
W, H = 112, 72               # re15_itps.h:17-18
IW, IH = 40, 30              # item_icon_common.c:14 (1200 B je Tile)

# gemessen am ausgelieferten Bild 0x40 mit der VOLLSTAENDIGEN Maske (alles, was nicht
# Hintergrundwort 0x1C00 / Hintergrundindex 0xE3,0xE4,0xE6 ist; Werkzeug
# analysis/befunde_runde30/sicherung_werkzeug/achse_altes_bild.py):
#   Item-Bild: 1972 px, Achse 147.4 Grad, Laenge 1.-99. Perzentil 83.9 px (Spanne 92.0)
#   Icon:       368 px, Achse 147.4 Grad, Laenge 1.-99. Perzentil 36.4 px (Spanne 38.7)
# (die erste Fassung mass mit einer Maske ohne die dunklen Flaechen: 150 Grad / 83.6 / 36.5)
ACHSE_GRAD = 147.4           # Hauptachse des Gegenstands im Bild
LAENGE_ITPS = 83.9           # px
LAENGE_ICON = 36.4           # px
# ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE (reine Darstellung, keine Messung dahinter — das
# Original hat von der Rohr-Sicherung kein Item-Bild):
KIPP_GRAD = 25.0             # das rechte obere Ende kommt dem Betrachter entgegen
LICHT = (-0.45, -0.70, 0.55)  # von links oben vorn
UMGEBUNG = 0.45
STREU = 0.75
SS = 6                       # Ueberabtastung
MARKE = "weg2"


def inc_bytes(name):
    t = open(INC, "r", encoding="utf-8").read()
    m = re.search(r"%s\[(\d+)\]\s*=\s*\{(.*?)\};" % re.escape(name), t, re.S)
    b = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", m.group(2)))
    assert len(b) == int(m.group(1))
    return b


def rgb15(c):
    return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


def tim_rgba(d):
    flag = struct.unpack_from("<I", d, 4)[0]
    assert flag == 9, "erwartet 8bpp+CLUT"
    ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, 8)
    clut = struct.unpack_from("<%dH" % cw, d, 20)
    o = 8 + ln
    l2, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, o)
    w = iw * 2
    idx = np.frombuffer(d, np.uint8, w * ih, o + 12).reshape(ih, w)
    a = np.zeros((ih, w, 4), np.uint8)
    for i, c in enumerate(clut):
        if c:
            a[idx == i] = rgb15(c) + (255,)
    return a


def md1_dreiecke(d):
    """-> Liste (3 Punkte, 3 UV). Quads wie der Port geteilt: (0,1,3) + (0,3,2)."""
    (tv, tvc, tn, tnc, tf, tfc, tu,
     qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", d, 12)
    V = [struct.unpack_from("<3h", d, 12 + tv + k * 8) for k in range(tvc)]
    Q = [struct.unpack_from("<3h", d, 12 + qv + k * 8) for k in range(qvc)]
    aus = []
    for k in range(tfc):
        n0, v0, n1, v1, n2, v2 = struct.unpack_from("<6H", d, 12 + tf + k * 12)
        u = struct.unpack_from("<BBHBBHBBH", d, 12 + tu + k * 12)
        aus.append(([V[v0], V[v1], V[v2]], [(u[0], u[1]), (u[3], u[4]), (u[6], u[7])]))
    for k in range(qfc):
        n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from("<8H", d, 12 + qf + k * 16)
        u = struct.unpack_from("<BBHBBHBBHBBH", d, 12 + qu + k * 16)
        p = [Q[v0], Q[v1], Q[v2], Q[v3]]
        t = [(u[0], u[1]), (u[3], u[4]), (u[6], u[7]), (u[9], u[10])]
        aus.append(([p[0], p[1], p[3]], [t[0], t[1], t[3]]))
        aus.append(([p[0], p[3], p[2]], [t[0], t[3], t[2]]))
    return aus


def drehung():
    """Modell-X (Laengsachse) -> Bildrichtung ACHSE, um KIPP aus der Bildebene geneigt.
    Bildkoordinaten: x rechts, y UNTEN, z zum Betrachter."""
    a = math.radians(ACHSE_GRAD)
    k = math.radians(KIPP_GRAD)
    # Laengsachse im Bild; das Ende "rechts oben" (= -Achse) kommt nach vorn
    ex = np.array([math.cos(a) * math.cos(k), math.sin(a) * math.cos(k), -math.sin(k)])
    ex = -ex                                       # +X des Modells zeigt nach rechts oben
    hilf = np.array([0.0, 0.0, 1.0])
    ey = np.cross(hilf, ex)
    ey /= np.linalg.norm(ey)
    ez = np.cross(ex, ey)
    return np.stack([ex, ey, ez], 1)               # Spalten = Bild der Modellachsen


def rendern(tris, tex, breite, hoehe, laenge_px, mitte):
    R = drehung()
    pts = np.array([p for t in tris for p in t[0]], float)
    laenge_modell = pts[:, 0].max() - pts[:, 0].min()
    proj_l = laenge_modell * math.cos(math.radians(KIPP_GRAD))
    s = laenge_px / proj_l * SS
    Wb, Hb = breite * SS, hoehe * SS
    farbe = np.zeros((Hb, Wb, 3), float)
    deck = np.zeros((Hb, Wb), bool)
    zb = np.full((Hb, Wb), -np.inf)
    L = np.array(LICHT, float)
    L /= np.linalg.norm(L)
    for p, uv in tris:
        q = (R @ np.array(p, float).T).T
        x = q[:, 0] * s + mitte[0] * SS
        y = q[:, 1] * s + mitte[1] * SS
        z = q[:, 2]
        n = np.cross(q[1] - q[0], q[2] - q[0])
        ln = np.linalg.norm(n)
        if ln < 1e-9:
            continue
        n /= ln
        if n[2] < 0:
            n = -n
        # Flaechen, die der Betrachter von der KANTE sieht, tragen nichts zum Bild bei,
        # ziehen beim Rastern aber Striche ueber die Silhouette hinaus (gemessen am ersten
        # Abzug: schwarze Linien neben dem Koerper). Das sind die Absatzringe Kappe/Koerper
        # (Radius 28.2 gegen 27.6) und die jeweils seitlichen Mantelflaechen.
        if n[2] < 0.08:
            continue
        hell = UMGEBUNG + STREU * max(0.0, float(n @ np.array([L[0], L[1], L[2]])))
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        xa, xb = max(int(min(x)), 0), min(int(max(x)) + 1, Wb - 1)
        ya, yb = max(int(min(y)), 0), min(int(max(y)) + 1, Hb - 1)
        if xa > xb or ya > yb:
            continue
        yy, xx = np.mgrid[ya:yb + 1, xa:xb + 1]
        px, py = xx + 0.5, yy + 0.5
        w0 = ((x[1] - px) * (y[2] - py) - (x[2] - px) * (y[1] - py)) / fl
        w1 = ((x[2] - px) * (y[0] - py) - (x[0] - px) * (y[2] - py)) / fl
        w2 = 1 - w0 - w1
        drin = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        zz = w0 * z[0] + w1 * z[1] + w2 * z[2]
        u = np.clip((w0 * uv[0][0] + w1 * uv[1][0] + w2 * uv[2][0]).astype(int), 0, tex.shape[1] - 1)
        v = np.clip((w0 * uv[0][1] + w1 * uv[1][1] + w2 * uv[2][1]).astype(int), 0, tex.shape[0] - 1)
        t = tex[v, u]
        vorn = drin & (zz > zb[ya:yb + 1, xa:xb + 1]) & (t[..., 3] > 0)
        zb[ya:yb + 1, xa:xb + 1][vorn] = zz[vorn]
        farbe[ya:yb + 1, xa:xb + 1][vorn] = np.clip(t[..., :3][vorn] * hell, 0, 255)
        deck[ya:yb + 1, xa:xb + 1][vorn] = True
    f = farbe.reshape(hoehe, SS, breite, SS, 3).mean((1, 3))
    a = deck.reshape(hoehe, SS, breite, SS).mean((1, 3))
    # Farbe der gedeckten Unterpixel (nicht mit Schwarz verduennen)
    with np.errstate(invalid="ignore", divide="ignore"):
        f = np.where(a[..., None] > 0, f / np.maximum(a[..., None], 1e-9), 0)
    return f, a


def wort(r, g, b, stp=True):
    return ((int(b) >> 3) << 10) | ((int(g) >> 3) << 5) | (int(r) >> 3) | (0x8000 if stp else 0)


def objektmaske_blau(rgb):
    a = rgb.astype(int)
    blau = (a[..., 2] > a[..., 0] + 25) & (a[..., 2] > a[..., 1] + 25)
    tief = (a[..., 0] < 20) & (a[..., 1] < 20)
    return ~(blau | tief)


def schwerpunkt(m):
    ys, xs = np.nonzero(m)
    return float(xs.mean()), float(ys.mean())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ziel", default=os.path.join(REPO, "build", "r30_sicherung"))
    ap.add_argument("--md1", default=None,
                    help="rohes MD1 statt der eingebackenen Bytes (z.B. die auf Z-Ordnung "
                         "berichtigte Fassung build/r30_sicherung/sicherung_zordnung.md1)")
    ap.add_argument("--inc", default=None,
                    help="zusaetzlich die C-Einbindung schreiben (Bau-Agent: "
                         "re15_port/engine/src/gen/sicherung_itembild.inc)")
    ap.add_argument("--marke", default="weg2",
                    help="Vorsatz der Ausgabedateien (Standard weg2)")
    a = ap.parse_args()
    os.makedirs(a.ziel, exist_ok=True)
    global MARKE
    MARKE = a.marke

    if a.md1:
        tris = md1_dreiecke(open(a.md1, "rb").read())
        print("Modell aus", a.md1)
    else:
        tris = md1_dreiecke(inc_bytes("re15_sicherung_md1"))
    tex = tim_rgba(inc_bytes("re15_sicherung_tim"))

    # ---------------- Item-Bild 112x72 ----------------------------------------
    itps = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
    blk = bytearray(itps[ITEM * ITPS_BLOCK:(ITEM + 1) * ITPS_BLOCK])
    clut0 = list(struct.unpack_from("<256H", blk, ITPS_CLUT))
    idx0 = np.frombuffer(bytes(blk), np.uint8, W * H, ITPS_BILD).reshape(H, W)
    wort0 = np.array(clut0, np.uint16)[idx0]
    rgb0 = np.stack([(wort0 & 31) << 3, ((wort0 >> 5) & 31) << 3, ((wort0 >> 10) & 31) << 3], -1)
    # PLATTE (berichtigt, Fortsetzungs-Agent): das ausgelieferte Bild 0x40 besteht aus einem
    # 2 Pixel breiten Rahmen (Zeile/Spalte 0 = 0x739C, 1 = 0x4210; unten/rechts gespiegelt)
    # und einer FLACHEN Innenflaeche 0x1C00 — gemessen 5244 von 7344 Innenpunkten, der Rest
    # ist Gegenstand samt Kantenglaettung (itps_hintergrund_40.py). Die erste Fassung liess
    # alle "fast schwarzen" Punkte als Hintergrund stehen; das waren die dunklen Flaechen des
    # ALTEN Gegenstands, sie blieben als Striche neben dem neuen stehen.
    rand = 2
    innen = np.zeros((H, W), bool)
    innen[rand:-rand, rand:-rand] = True
    werte, zahl = np.unique(wort0[innen], return_counts=True)
    hg = int(werte[zahl.argmax()])
    m0 = innen & (wort0 != hg)
    # Lage und Groesse des alten Gegenstands OHNE seine Kantenglaettung (blau-dominante Punkte)
    kern = m0 & ~((rgb0[..., 2].astype(int) > rgb0[..., 0].astype(int) + 25) &
                  (rgb0[..., 2].astype(int) > rgb0[..., 1].astype(int) + 25))
    print("Item-Bild 0x40: Hintergrundwort 0x%04X = RGB %s (%d von %d Innenpunkten), "
          "alter Gegenstand %d px (Kern %d px), Schwerpunkt (%.1f, %.1f)"
          % (hg, rgb15(hg), zahl.max(), int(innen.sum()), int(m0.sum()), int(kern.sum()),
             *schwerpunkt(kern)))
    platte = wort0.copy()
    platte[innen] = hg
    m0 = kern
    f, alpha = rendern(tris, tex, W, H, LAENGE_ITPS, schwerpunkt(m0))
    neu = alpha >= 0.5
    # Palette: alle Plattenworte + adaptive Farben fuer den Gegenstand
    plattenworte = sorted(set(platte.flatten().tolist()))
    frei = 256 - len(plattenworte)
    obj = Image.fromarray(np.clip(f, 0, 255).astype(np.uint8))
    q = obj.quantize(colors=min(frei, 200), method=Image.MEDIANCUT, dither=Image.NONE)
    pal = np.array(q.getpalette()[:3 * min(frei, 200)], np.uint8).reshape(-1, 3)
    qi = np.asarray(q)
    clut = plattenworte + [wort(*pal[i]) for i in range(len(pal))]
    clut += [0x8000] * (256 - len(clut))
    index = {w: i for i, w in enumerate(plattenworte)}
    bild = np.zeros((H, W), np.uint8)
    for y in range(H):
        for x in range(W):
            bild[y, x] = (len(plattenworte) + qi[y, x]) if neu[y, x] else index[int(platte[y, x])]
    assert all(c != 0 for c in clut), "CLUT-Wort 0x0000 waere durchsichtig (itps_common.c:69)"
    # Block schreiben: Kopf mit den Rechtecken der 53 'fertigen' Bloecke
    aus = bytearray(ITPS_BLOCK)
    struct.pack_into("<II", aus, 0, 0x10, 0x09)
    struct.pack_into("<IHHHH", aus, 8, 12 + 512, 0, 489, 256, 1)
    struct.pack_into("<256H", aus, ITPS_CLUT, *clut)
    struct.pack_into("<IHHHH", aus, 8 + 524, 12 + W * H, 832, 256, W // 2, H)
    aus[ITPS_BILD:ITPS_BILD + W * H] = bild.tobytes()
    open(os.path.join(a.ziel, MARKE + "_itps_block_40.bin"), "wb").write(aus)
    dek = np.array([rgb15(c) for c in clut], np.uint8)[bild]
    Image.fromarray(dek).save(os.path.join(a.ziel, MARKE + "_itps_40.png"))
    Image.fromarray(dek).resize((W * 4, H * 4), Image.NEAREST).save(
        os.path.join(a.ziel, MARKE + "_itps_40_4x.png"))
    fehler = np.abs(dek[neu].astype(int) - np.clip(f, 0, 255)[neu]).mean()
    print("  neues Item-Bild: Gegenstand %d px, Palette %d Plattenworte + %d Objektfarben, "
          "Quantisierungsfehler im Gegenstand %.2f (Mittel |dRGB| je Kanal, von 255)"
          % (int(neu.sum()), len(plattenworte), len(pal), fehler))
    unveraendert = int((platte[~neu] == wort0[~neu]).sum())
    print("  ausserhalb des neuen Gegenstands: %d Pixel, davon %d wortgleich mit dem Original "
          "(der Rest lag unter dem ALTEN Gegenstand und traegt das Hintergrundwort)"
          % (int((~neu).sum()), unveraendert))

    # ---------------- Icon 40x30 -----------------------------------------------
    pix = open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read()
    st00 = open(os.path.join(PSX, "DATA", "ST_00.TIM"), "rb").read()
    ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", st00, 8)
    cl = list(struct.unpack_from("<256H", st00, 20))           # CLUT-Zeile 0
    tiles = np.frombuffer(pix, np.uint8).reshape(-1, IH, IW)
    t0 = tiles[ITEM]
    rgbt = np.array([rgb15(c) for c in cl], np.uint8)[t0]
    # PLATTE (berichtigt): der Icon-Hintergrund ist ein Verlauf aus genau drei Indizes
    # (0xE3/0xE4/0xE6 — die einzigen blau-dominanten Indizes, die in den Ecken aller 72 Tiles
    # vorkommen; icon_platte.py). Unter dem alten Gegenstand gilt je Bildpunkt der haeufigste
    # dieser drei ueber alle Tiles; wo Tile 0x40 selbst Hintergrund zeigt, bleibt sein Wert.
    ecken = np.concatenate([tiles[:, :3, :3].ravel(), tiles[:, :3, -3:].ravel(),
                            tiles[:, -3:, :3].ravel(), tiles[:, -3:, -3:].ravel()])
    ew, ez = np.unique(ecken, return_counts=True)
    hgi = [int(i) for i, z in zip(ew, ez)
           if z >= 20 and rgb15(cl[i])[2] > rgb15(cl[i])[0] + 25 and rgb15(cl[i])[2] > rgb15(cl[i])[1] + 25]
    mi = ~np.isin(t0, hgi)
    plat = t0.copy()
    for y in range(IH):
        for x in range(IW):
            if mi[y, x]:
                z_ = [int((tiles[:, y, x] == i).sum()) for i in hgi]
                plat[y, x] = hgi[int(np.argmax(z_))]
    print("Icon 0x40: Hintergrund-Indizes %s, alter Gegenstand %d px"
          % (" ".join("0x%02X" % i for i in hgi), int(mi.sum())))
    fi, ai = rendern(tris, tex, IW, IH, LAENGE_ICON, schwerpunkt(mi))
    neui = ai >= 0.5
    palrgb = np.array([rgb15(c) for c in cl], float)
    erlaubt = np.array([c != 0 for c in cl])                  # 0x0000 = durchsichtig
    icon = plat.copy()
    fehl = []
    for y in range(IH):
        for x in range(IW):
            if neui[y, x]:
                d = np.abs(palrgb - fi[y, x]).sum(1)
                d[~erlaubt] = 1e9
                k = int(d.argmin())
                icon[y, x] = k
                fehl.append(np.abs(palrgb[k] - fi[y, x]).mean())
    open(os.path.join(a.ziel, MARKE + "_icon_tile_40.bin"), "wb").write(icon.tobytes())
    deki = np.array([rgb15(c) for c in cl], np.uint8)[icon]
    Image.fromarray(deki).save(os.path.join(a.ziel, MARKE + "_icon_40.png"))
    Image.fromarray(deki).resize((IW * 8, IH * 8), Image.NEAREST).save(
        os.path.join(a.ziel, MARKE + "_icon_40_8x.png"))
    print("Icon 0x40: alter Gegenstand %d px, neuer %d px; %d verschiedene Palettenindizes benutzt; "
          "Fehler gegen die FESTE ST_00-Palette %.2f (Mittel |dRGB| je Kanal), groesster %.1f"
          % (int(mi.sum()), int(neui.sum()), len(set(icon[neui].tolist())),
             float(np.mean(fehl)), float(np.max(fehl))))

    # der Block fuehrt das 40x30-Icon ein zweites Mal bei +0x21A0 (66 von 72 ausgelieferten
    # Bloecken sind dort bytegleich mit ihrem ITEMALL-Tile; Quelle des Aufnahme-Uploads
    # FUN_800492b8, platform/pc/src/inv_render_pc.c:301-305) - mitziehen
    aus[0x21A0:0x21A0 + IW * IH] = icon.tobytes()
    open(os.path.join(a.ziel, MARKE + "_itps_block_40.bin"), "wb").write(aus)
    if a.inc:
        NL = chr(10)
        with open(a.inc, "w", encoding="utf-8", newline=NL) as f:
            f.write(NL.join([
                "/* GENERIERT von re15_port/tools/sicherung_itembild.py - NICHT HAND-EDITIEREN.",
                " *",
                " * Item-Bild (ITPS-Block, 0x3000 B) und Inventar-Icon (ITEMALL-Tile, 1200 B) der",
                " * SICHERUNG, gerendert aus dem Welt-Modell. Herleitung: analysis/befunde_runde30/",
                " * sicherung.md. Der Block traegt die Rechtecke der 53 RE1.5-Bloecke",
                " * (crect (0,489) 256x1, prect (832,256) 56x72) und bei +0x21A0 das Icon.",
                " *",
                " * BELEGT (am ausgelieferten Block 0x40 @Datei 0xC0000 / Tile 0x40 @0x12C00 gemessen):",
                " *   Rahmen 2 px (0x739C / 0x4210) und Hintergrundwort 0x1C00 (5244 von 7344",
                " *   Innenpunkten); Icon-Hintergrund Indizes 0xE3/0xE4/0xE6; Lage und Laenge des",
                " *   Gegenstands: Achse %.1f Grad, %.1f px im Bild, %.1f px im Icon." % (
                    ACHSE_GRAD, LAENGE_ITPS, LAENGE_ICON),
                " * PORT-WAHL, KEINE ORIGINAL-ADRESSE (reine Darstellung, keine Messung dahinter):",
                " *   Blickneigung %.0f Grad, Licht (%.2f,%.2f,%.2f), Umgebung %.2f, Streuung %.2f." % (
                    KIPP_GRAD, LICHT[0], LICHT[1], LICHT[2], UMGEBUNG, STREU),
                " */", "", ""]))
            for name, b in (("re15_sicherung_itps_block", bytes(aus)),
                            ("re15_sicherung_icon_tile", icon.tobytes())):
                f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
                for i, x in enumerate(b):
                    f.write((NL + "    " if i % 16 == 0 else "") + "0x%02x," % x)
                f.write(NL + "};" + NL + NL)
        print("C-Einbindung:", a.inc)

    # ---------------- Vergleichsblatt ------------------------------------------
    alt = Image.fromarray(rgb0.astype(np.uint8)).resize((W * 4, H * 4), Image.NEAREST)
    neu_b = Image.fromarray(dek).resize((W * 4, H * 4), Image.NEAREST)
    alt_i = Image.fromarray(rgbt).resize((IW * 8, IH * 8), Image.NEAREST)
    neu_i = Image.fromarray(deki).resize((IW * 8, IH * 8), Image.NEAREST)
    blatt = Image.new("RGB", (W * 8 + 30, H * 4 + IH * 8 + 30), (24, 24, 28))
    blatt.paste(alt, (10, 10))
    blatt.paste(neu_b, (20 + W * 4, 10))
    blatt.paste(alt_i, (10, 20 + H * 4))
    blatt.paste(neu_i, (20 + W * 4, 20 + H * 4))
    blatt.save(os.path.join(a.ziel, MARKE + "_vorher_nachher.png"))
    print("geschrieben nach", a.ziel)


if __name__ == "__main__":
    main()
