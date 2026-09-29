#!/usr/bin/env python3
"""tuer_archiv_bauen.py - Runde 33 / Thema T: PORT-EIGENE Tuerarchive im RE2-Aufbau.

Aufruf
    python re15_port/tools/tueren/tuer_archiv_bauen.py              # alle gebauten Archive (Stufe 1)
    python re15_port/tools/tueren/tuer_archiv_bauen.py --nur P07G   # eines
    python re15_port/tools/tueren/tuer_archiv_bauen.py --pruefen    # nur lesen + pruefen, nichts schreiben

Eingabe
    analysis/befunde_runde33/tueren_rest/plan.json   (tuer_rest_plan.py: Archive, Tueren, Varianten)
    analysis/befunde_runde31/tueren_03/zuordnung.json (Port-Schluessel je Seite: Raum, Flaeche, Band)
    info/re2leon/COMMON/DOOR/DOORxx.DO2               (Basis-Archive, RE2 Retail, unveraendert gelesen)
    build/r31_tueren/t1/re15_seiten/*_entz.png        (RE1.5-Blaetter entzerrt 128x218, nur FARBE)
    extracted/PSX/STAGE1/ROOM126/ROOM1260{7,9}.bmp    (Messingleiter, nur Farbton)

Ausgabe
    re15_port/shared_assets/RE15DOOR/<Kennung>.DO2    Port-Archiv (RE2-Aufbau: Tonteil | Auffuellung |
                                                      Modellteil = +0 MD1-Versatz, +4 TIM-Versatz, SCD,
                                                      MD1, TIM) - Tonteil, SCD, MD1 BYTEGLEICH dem
                                                      Basis-Archiv, nur die TIM ist neu (gleiche Laenge).
    re15_port/engine/src/gen/re15_tuer_eigen.inc     Tabelle der Port-Archive (Kennung, Basis, Groessen,
                                                      FNV-1a der Datei) + Zuordnungszeilen der Seiten.
    analysis/befunde_runde33/tueren_rest/archive.json Protokoll (Farben, Palette, Pruefungen).
    build/r33_tueren/archiv/<Kennung>_tim.png         Vorschau der Textur (4fach).

WARUM EIN EIGENES ARCHIV (und nicht nur eine TIM)
    Der Laeufer (platform/pc/src/door_scene_pc.c) liest jedes Archiv im RE2-Aufbau gleich; ein
    Port-Archiv ist damit ein RE2-Archiv mit anderer Textur - die Maschine, der Ton, der Griff-Tausch
    und die Pruefhaken laufen unveraendert. Var 15 (Payload+12) bleibt die Basis-Nummer, die Skripte
    sehen also genau das Archiv, zu dem sie gehoeren.

WERKZEUG-ENTSCHEIDUNG Datei statt eingebacken (analysis/befunde_runde33/tueren_rest_plan.md 4):
    25 geplante Archive x 53..81 KB = ~1,6 MB; als C-Feld waeren das ~100 000 Zeilen .inc (das Tor:
    39 KB = 2 513 Zeilen) in jeder Uebersetzung der Engine. Die PSX hat keinen Laeufer (die Anfrage
    verfaellt dort), braucht die Bytes also nicht. Dateien gehen denselben Weg wie shared_assets/RE2/DOOR
    (re15_pc_read_shared) und werden im Paket geprueft (release/make_package.sh).

⛔ PORT-WAHL, KEINE Original-Adresse: die Textur jedes Archivs ist eine Bauentscheidung. Belegt
sind die Rechenwege: Blattfarbe c = NCCT der Blattflaeche mit den RE2-Konstanten
(tor_sequenz_bauen.ncct_eckfarbe: BK 68 @0x800142e8, L @0x8009a470, LCM 1600 @0x8009a490, RGBC 0x808080
@0x80014b58) = 73; gezeigt = Texel * c / 128 (psx-spx GPU:1438-1446). Ziel "gezeigt = gemalt" wie beim
Tor (analysis/befunde_runde32/tor_helligkeit.md 3.2) -> Texel = gemalt * 128 / c.
"""
import argparse
import glob
import hashlib
import json
import os
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
sys.path.insert(0, HIER)
import do2_format as fmt                 # noqa: E402
import tor_sequenz_bauen as tsb          # noqa: E402  (ncct_eckfarbe, RE2-Tuerlicht)
import tuer_zuordnung_gen as tzg         # noqa: E402  (c_div, raum_id)

PLAN = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest", "plan.json")
ZUORDNUNG = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_03", "zuordnung.json")
RE2_DOOR = os.path.join(REPO, "info", "re2leon", "COMMON", "DOOR")
AUS_DIR = os.path.join(PORT, "shared_assets", "RE15DOOR")
AUS_INC = os.path.join(PORT, "engine", "src", "gen", "re15_tuer_eigen.inc")
AUS_JSON = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest", "archive.json")
VORSCHAU = os.path.join(REPO, "build", "r33_tueren", "archiv")


def _t1_seiten():
    """RE1.5-Ausschnitte der Runde 31 (T1). Im Arbeitsbaum fehlt build/ -> Hauptbaum (nur lesen)."""
    for kand in (os.path.join(REPO, "build", "r31_tueren", "t1", "re15_seiten"),
                 os.path.join(REPO, "..", "..", "..", "build", "r31_tueren", "t1", "re15_seiten")):
        if os.path.isdir(kand):
            return os.path.abspath(kand)
    raise SystemExit("build/r31_tueren/t1/re15_seiten fehlt (Runde-31-Ausschnitte)")


def _extracted():
    for kand in (os.path.join(REPO, "extracted", "PSX"), os.path.join(REPO, "..", "..", "..", "extracted", "PSX")):
        if os.path.isdir(kand):
            return os.path.abspath(kand)
    raise SystemExit("extracted/PSX fehlt")


BLATT_V = 218      # Blatt = v 0..217 (Mesh 0 aller Standardblaetter, r33_ana_uv.py)
LUM = np.array([0.299, 0.587, 0.114])

# Blattflaeche zur Kamera (Drehung 0 = V0/V2, Rueckseite bei Drehung 2048 = V1/V3 dieselbe),
# Tuerlicht ohne Flag 0x1000 (die Skripte bleiben unveraendert): c = 73 (tor_helligkeit.md 2.2).
C_BLATT = tsb.ncct_eckfarbe(tsb.SCHILD_NORMALE, tsb.BK_NORMAL, 0)
if C_BLATT != 73 or tsb.ncct_eckfarbe((-4096, 0, 0), tsb.BK_NORMAL, 2048) != 73:
    raise SystemExit("Blatt-Eckfarbe %d statt 73 (08_re_zeichnen 4.3)" % C_BLATT)


# ==============================================================================================
# Lesen / Masken
# ==============================================================================================
def re2_lesen(name):
    d = open(os.path.join(RE2_DOOR, name + ".DO2"), "rb").read()
    o = fmt.Do2.lesen(d)
    md1 = fmt.Md1.lesen(o.md1)
    tim, n = fmt.Tim.lesen(o.tim)
    if n != len(o.tim):
        raise SystemExit("%s: TIM %d B, gelesen %d" % (name, len(o.tim), n))
    return d, o, md1, tim


def rgba_aus_tim(tim):
    """(H, W, 4) uint8 + (H, W) 15-Bit-Werte (ohne Umrechnung) + Transparenz (Wert 0x0000)."""
    w, h = tim.breite_px, tim.hoehe_px
    idx = np.frombuffer(tim.pix, np.uint8).reshape(h, w)
    clut = np.array(tim.clut[:256], np.uint16)
    wert = clut[idx]
    r = (wert & 31).astype(np.int32)
    g = ((wert >> 5) & 31).astype(np.int32)
    b = ((wert >> 10) & 31).astype(np.int32)
    rgb = np.stack([r << 3 | r >> 2, g << 3 | g >> 2, b << 3 | b >> 2], -1)
    a = np.where(wert == 0, 0, 255)
    return np.concatenate([rgb, a[..., None]], -1).astype(np.uint8), wert


def uv_maske(md1, meshes, w=128, h=256):
    """Texel, die die Dreiecke der genannten Meshes lesen (Polygon inkl. Rand)."""
    m = Image.new("L", (w, h), 0)
    d = ImageDraw.Draw(m)
    for i in meshes:
        for t in md1.meshes[i].tri_tex:
            d.polygon([(t[0], t[1]), (t[3], t[4]), (t[6], t[7])], fill=255, outline=255)
        for q in md1.meshes[i].quad_tex:
            d.polygon([(q[0], q[1]), (q[3], q[4]), (q[9], q[10]), (q[6], q[7])], fill=255, outline=255)
    return np.asarray(m) > 0


def blur(a, s):
    """Gauss-Tiefpass (float, getrennt in v und u, Rand gespiegelt), je Kanal."""
    a = np.asarray(a, np.float64)
    if a.ndim == 3:
        return np.stack([blur(a[..., k], s) for k in range(a.shape[-1])], -1)
    r = max(1, int(3 * s + 0.5))
    x = np.arange(-r, r + 1, dtype=np.float64)
    k = np.exp(-x * x / (2 * s * s))
    k /= k.sum()
    p = np.pad(a, ((r, r), (0, 0)), mode="reflect")
    a = sum(k[i] * p[i:i + a.shape[0]] for i in range(2 * r + 1))
    p = np.pad(a, ((0, 0), (r, r)), mode="reflect")
    return sum(k[i] * p[:, i:i + a.shape[1]] for i in range(2 * r + 1))


# ==============================================================================================
# Gemalte RE1.5-Farbe (Tiefpass der entzerrten Blaetter, gemessen)
# ==============================================================================================
def re15_blattfeld(auswahl, griff_box=(0, 90, 40, 150), rand=3, nur_zeilen=False):
    """Median-Feld (218x128x3) ueber die gewaehlten Ausschnitte. auswahl = [(Seite, Cut, Griffseite)].
    Griffseite rechts -> waagerecht gespiegelt (Texturlage: Griffkante bei u klein, V0 von vorn).
    Der Griff (hell, Box u0..40 v90..150 nach dem Spiegeln) wird durch den Zeilenmedian der Blattmitte
    ersetzt, dann Tiefpass (sigma 4) - die Ausschnitte sind vergroesserte 30..95-px-Bilder, sie tragen
    nur die Farbe, keine Zeichnung (tueren_rest_plan.md 2.2)."""
    ordner = _t1_seiten()
    felder, benutzt = [], []
    for seite, cut, griff in auswahl:
        f = glob.glob(os.path.join(ordner, "%s_*_%s_entz.png" % (seite, cut)))
        if not f:
            raise SystemExit("Ausschnitt %s %s fehlt" % (seite, cut))
        a = np.asarray(Image.open(f[0]).convert("RGB"), np.float64)
        if a.shape[:2] != (BLATT_V, 128):
            raise SystemExit("%s: %s statt 218x128" % (f[0], a.shape))
        if griff == "rechts":
            a = a[:, ::-1]
        u0, v0, u1, v1 = griff_box
        zeile = np.median(a[:, 44:104], axis=1)
        if nur_zeilen:
            # Doppeltuer: der Ausschnitt zeigt BEIDE Fluegel (Fuge + Druecker in der Mitte), die Textur
            # ist EIN Fluegel -> nur das Zeilenprofil der Fluegelflaechen (u 16..48 und 80..112)
            zeile = np.median(np.concatenate([a[:, 16:48], a[:, 80:112]], 1), axis=1)
            a[:] = zeile[:, None, :]
        a[v0:v1, u0:u1] = zeile[v0:v1, None, :]
        a[:, :rand] = a[:, rand:rand + 1]
        a[:, -rand:] = a[:, -rand - 1:-rand]
        felder.append(blur(a, 4))
        benutzt.append(os.path.basename(f[0]))
    # Getrennt in Zeilen x Spalten (Vorschau 1: ein Median-Feld je Punkt trug oertliche Flecken
    # einzelner Ausschnitte, z. B. ein gruenes Notausgangsschild oben): Zeilenprofil = Median der
    # Zeilenmediane, Spaltenfaktor = Median der normierten Spaltenmittel (Helligkeit), beide geglaettet.
    F = np.array(felder)                                   # (n, 218, 128, 3)
    zeilen = np.median(np.median(F[:, :, 16:112], axis=2), axis=0)        # (218, 3)
    spL = (F @ LUM).mean(axis=1)                                         # (n, 128)
    spalt = np.median(spL / np.maximum(spL[:, 16:112].mean(1, keepdims=True), 1.0), axis=0)
    zeilen = blur(zeilen[:, None, :], 6 if nur_zeilen else 3)[:, 0, :]   # Doppeltuer: Drueckerzeile glaetten
    spalt = blur(spalt[None, :], 3)[0]
    if nur_zeilen:
        spalt = np.ones_like(spalt)
    return zeilen[:, None, :] * spalt[None, :, None], benutzt


# ==============================================================================================
# Blech-Korn aus einer RE2-Blatt-Textur (fein, 1..2 Texel), Merkmale vorher ueberdeckt
# ==============================================================================================
def blech_korn(rgba, flicken, sigma=1.5, kappe=3.0):
    """Relatives Feinkorn (218x128) der RE2-Blatt-Textur: L / Tiefpass(L) - 1, nachdem die Rechtecke
    `flicken` = [(u0, v0, u1, v1, dv)] mit den Zeilen v-dv ueberdeckt sind (Lueftungsschlitze u. ae.).
    Flecken (> 3 Texel) fallen durch den Tiefpass sigma 1,5 heraus; Ausreisser ueber kappe * sigma
    werden gekappt (Kratzer, Kanten der Flecken)."""
    L = rgba[:BLATT_V, :, :3].astype(np.float64) @ LUM
    tief = blur(L, sigma)
    rel = L / np.maximum(tief, 1.0) - 1.0
    # Flicken im KORN, nicht in L: das Korn ist oertlich, so entsteht an der Flickenkante keine
    # Helligkeitsstufe (die L-Flicken zeichneten das Rechteck nach, Vorschau 1 angesehen)
    for u0, v0, u1, v1, dv in flicken:
        rel[v0:v1, u0:u1] = rel[v0 - dv:v1 - dv, u0:u1]
    s = rel.std()
    return np.clip(rel, -kappe * s, kappe * s), L


def texel_fuers_tuerlicht(gemalt):
    """Texel = gemalt * 128 / c (c = 73), 8 Bit, gekappt auf 255 (Kappung wird gezaehlt)."""
    t = gemalt * 128.0 / C_BLATT
    return np.clip(t, 0, 255), int((t > 255).any(-1).sum())


# ==============================================================================================
# Rezepte (je Kennung). Rueckgabe: neues RGBA-Bild 256x128 + Protokoll.
# ==============================================================================================
# Frontale, scharfe Ausschnitte der G1-Seiten (tueren_rest_plan.md 5.1, Bogen tueren_rest_belege/
# g1_re15_entz.jpg angesehen): Seite, Cut, gemalte Griffseite.
G1_AUSWAHL = [("S001", "c03", "rechts"), ("S002", "c06", "links"), ("S004", "c04", "links"),
              ("S003", "c00", "rechts"), ("S005", "c09", "rechts"), ("S014", "c00", "links"),
              ("S056", "c00", "links"), ("S039", "c00", "links"), ("S036", "c07", "rechts"),
              ("S119", "c02", "links"), ("S078", "c02", "links"), ("S021", "c04", "links"),
              ("S033", "c00", "rechts"), ("S023", "c00", "rechts"), ("S024", "c02", "rechts"),
              ("S025", "c05", "rechts"), ("S000", "c00", "rechts"), ("S020", "c04", "rechts"),
              ("S019", "c01", "links")]
# DOOR07-Blatt (Bogen build/r33_tueren/d07_grid.png, gemessen mit Zeilenprofil): Lueftungsschlitze
# u 26..100 v 164..206 -> Flicken u 24..102 v 162..208 aus den Zeilen 46 hoeher (v 116..162, glattes Blech).
D07_SCHLITZE = (24, 162, 103, 209, 46)
# gemaltes Druecker-Schild des DOOR07 (Umriss u 0..20 v 94..122, liegt unter dem Druecker-Mesh am
# Anhaengepunkt z -3404 -> u ~7, y -3352 -> v ~111): volles Korn, damit der Umriss sichtbar bleibt.
D07_SCHILD = (0, 92, 22, 125)
D07_SCHILD_FLICKEN = (0, 90, 24, 127, -40)   # Umriss aus den Zeilen 40 tiefer (v 130..166, glatt)
KORN_GLATT = 0.5   # PORT-WAHL "glattes Blatt" (Merkmal Runde 31): RE2-Korn halb so stark


def rezept_glattes_blech(basis_rgba, korn_quelle, auswahl, schild=None, flicken=(D07_SCHLITZE,),
                         nur_zeilen=False):
    feld, benutzt = re15_blattfeld(auswahl, nur_zeilen=nur_zeilen)
    korn, _ = blech_korn(korn_quelle, flicken)
    t_tief, gekappt = texel_fuers_tuerlicht(feld)
    k = np.full(korn.shape, KORN_GLATT)
    if schild:
        u0, v0, u1, v1 = schild
        k[v0:v1, u0:u1] = 1.0
    blatt = np.clip(t_tief * (1.0 + k * korn)[..., None], 0, 255)
    out = basis_rgba.copy()
    out[:BLATT_V, :, :3] = np.round(blatt).astype(np.uint8)
    out[:BLATT_V, :, 3] = 255
    proto = dict(auswahl=benutzt, gemalt_mittel=[round(x, 1) for x in feld.reshape(-1, 3).mean(0)],
                 gemalt_mitte=[round(x, 1) for x in feld[80:140].reshape(-1, 3).mean(0)],
                 texel_mittel=[round(x, 1) for x in blatt.reshape(-1, 3).mean(0)], gekappt=gekappt,
                 c_blatt=C_BLATT, korn=KORN_GLATT, korn_std=round(float(korn.std()), 4))
    return out, proto


def rezept_P07G(basis):
    """Schild-Umriss des DOOR07 (Pfeilform) entfaellt wie bei P1DG: RE1.5 malt ein Rechteckschild, das
    in den 30..95-px-Ausschnitten nicht aufzuloesen ist -> nicht gemalt statt geraten; der Druecker
    (Mesh 1) bleibt."""
    rgba = basis["rgba"]
    return rezept_glattes_blech(rgba, rgba, G1_AUSWAHL, schild=None, flicken=(D07_SCHLITZE, D07_SCHILD_FLICKEN))


# G4 (T026, T054): graue glatte Stahl-Doppeltuer, zwei Druecker an der Fuge (Bogen angesehen).
# S100 (c04/c11) trifft nur die Kachelwand neben der Tuer (Bogen pilot_aus angesehen) -> nicht benutzt.
G4_AUSWAHL = [("S045", "c08", "links"), ("S045", "c09", "links"), ("S049", "c00", "links"),
              ("S104", "c00", "links"), ("S104", "c01", "links")]


def rezept_P1DG(basis):
    """Blatt aus glattem Blech: Korn vom DOOR07 (Schlitze ueberdeckt, gleiche Blechklasse), Farbe aus
    den RE1.5-Seiten von T026/T054; das DOOR1D-Lueftungsgitter und der Griffkasten entfallen (RE1.5:
    'ohne Felder, ohne Fenster', Druecker an der Mittelfuge). DOOR07s Schild-Umriss wird NICHT
    uebernommen (der 1D-Druecker sitzt tiefer, Anker y -2850) -> Schildflaeche ebenfalls geflickt."""
    d07 = re2_lesen("DOOR07")
    korn_q, _ = rgba_aus_tim(d07[3])
    flicken = (D07_SCHLITZE, D07_SCHILD_FLICKEN)
    return rezept_glattes_blech(basis["rgba"], korn_q, G4_AUSWAHL, schild=None, flicken=flicken,
                                nur_zeilen=True)


# Messing (ROOM12607.bmp x 126..141 y 15..149 und ROOM12609.bmp x 203..225 y 0..149, Leiterpunkte
# R > B + 12 und L > 30; gemessen: Median-Verhaeltnis R/L G/L B/L = 1,109/1,021/0,596 bzw. 1,116/1,019/0,567)
MESSING = np.array([1.1125, 1.020, 0.5815])


def rezept_P16M(basis):
    """Leiter: Helligkeit je Texel bleibt die der RE2-Leiter (die Sequenz zeigt die Leiter gross im
    RE2-Licht), der Farbton wird der der gemalten Messingleiter ROOM1260: RGB = L * (R/L, G/L, B/L).
    Nur die Texel, die die Leiter-Meshes lesen (die blauen/magenta Texel liest kein Dreieck)."""
    rgba = basis["rgba"].copy()
    m = uv_maske(basis["md1"], range(len(basis["md1"].meshes)))
    L = rgba[..., :3].astype(np.float64) @ LUM
    neu = np.clip(L[..., None] * MESSING[None, None, :], 0, 255)
    sel = m & (rgba[..., 3] > 0)
    rgba[sel, :3] = np.round(neu[sel]).astype(np.uint8)
    gekappt = int((L[sel][:, None] * MESSING > 255).any(-1).sum())
    return rgba, dict(messing_verhaeltnis=MESSING.tolist(), texel_umgefaerbt=int(sel.sum()), gekappt=gekappt,
                      L_mittel=round(float(L[sel].mean()), 1))


REZEPTE = {"P07G": rezept_P07G, "P1DG": rezept_P1DG, "P16M": rezept_P16M}


# ==============================================================================================
# TIM bauen: geschuetzte Texel exakt, Rest 5 Bit gerundet, Palette <= 256 mit Index 0 = durchsichtig
# ==============================================================================================
def tim_bauen(rgba, basis_tim, basis_wert, schutz, unbenutzt=None):
    """schutz = Texel, die ein Griff-/Anbau-Mesh liest (bleiben bytegleich); unbenutzt = Texel, die kein
    Dreieck liest (bekommen am Ende den naechsten Paletteneintrag, belegen also keinen Platz)."""
    h, w = rgba.shape[:2]
    if unbenutzt is None:
        unbenutzt = np.zeros((h, w), bool)
    schutz = schutz & ~unbenutzt
    rgb = rgba[..., :3].astype(np.int32)
    v5 = np.minimum(31, (rgb + 4) >> 3)                      # runden statt abschneiden (Runde 32)
    wert = (v5[..., 0] | (v5[..., 1] << 5) | (v5[..., 2] << 10)).astype(np.uint16)
    wert = np.where(wert == 0, 0x8000, wert).astype(np.uint16)   # deckendes Schwarz = 0x8000 (wie RE2)
    wert = np.where(rgba[..., 3] == 0, 0, wert).astype(np.uint16)
    wert = np.where(schutz, basis_wert, wert).astype(np.uint16)  # Griff-/Beschlagtexel bytegleich
    geschuetzt = sorted({int(x) for x in np.unique(basis_wert[schutz])} - {0})
    frei = sorted({int(x) for x in np.unique(wert[~schutz & ~unbenutzt])} - {0} - set(geschuetzt))
    plaetze = 255
    info = dict(farben_geschuetzt=len(geschuetzt), farben_blatt=len(frei))
    if len(geschuetzt) + len(frei) <= plaetze:
        palette = geschuetzt + frei
        info["quantisiert"] = False
        fehler = 0
    else:
        rest = plaetze - len(geschuetzt)
        maske = (~schutz) & (~unbenutzt) & (wert != 0)
        px = wert[maske]
        pr = np.stack([(px & 31), (px >> 5) & 31, (px >> 10) & 31], -1).astype(np.uint8) << 3
        bild = Image.fromarray(pr.reshape(1, -1, 3))
        q = bild.quantize(colors=rest, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
        pal = q.getpalette()[:rest * 3]
        neu = sorted({(pal[i * 3] >> 3) | ((pal[i * 3 + 1] >> 3) << 5) | ((pal[i * 3 + 2] >> 3) << 10)
                      for i in range(len(pal) // 3)} - {0} - set(geschuetzt))
        palette = geschuetzt + neu[:rest]
        p5 = np.array([[c & 31, (c >> 5) & 31, (c >> 10) & 31] for c in neu[:rest]], np.int32)
        pk = np.stack([px & 31, (px >> 5) & 31, (px >> 10) & 31], -1).astype(np.int32)
        wahl = ((pk[:, None, :] - p5[None, :, :]) ** 2).sum(-1).argmin(1)
        fehler = int(np.abs(pk - p5[wahl]).max())
        wert = wert.copy()
        wert[maske] = np.array(neu[:rest], np.uint16)[wahl]
        info["quantisiert"] = True
    info["groesster_fehler_5bit"] = fehler
    # unbenutzte Texel: naechster Paletteneintrag (kein Dreieck liest sie; Wert 0 bleibt 0)
    um = unbenutzt & (wert != 0)
    if um.any():
        p5 = np.array([[c & 31, (c >> 5) & 31, (c >> 10) & 31] for c in palette], np.int32)
        pu = wert[um]
        pk = np.stack([pu & 31, (pu >> 5) & 31, (pu >> 10) & 31], -1).astype(np.int32)
        wahl = np.concatenate([((pk[i:i + 2048, None, :] - p5[None]) ** 2).sum(-1).argmin(1)
                               for i in range(0, len(pk), 2048)])
        wert = wert.copy()
        wert[um] = np.array(palette, np.uint16)[wahl]
    info["texel_unbenutzt"] = int(unbenutzt.sum())
    clut = [0x0000] + palette
    clut += [0x0000] * (256 - len(clut))
    nach = {c: i + 1 for i, c in enumerate(palette)}
    idx = np.zeros((h, w), np.uint8)
    for c, i in nach.items():
        idx[wert == c] = i
    idx[wert == 0] = 0
    tim = fmt.Tim(flags=basis_tim.flags, clut_rect=basis_tim.clut_rect, clut=clut,
                  pix_rect=basis_tim.pix_rect, pix=idx.tobytes())
    info["palette_belegt"] = len(palette) + 1
    # Rueckprobe: geschuetzte Texel exakt
    rueck = np.array(clut, np.uint16)[idx]
    if not np.array_equal(rueck[schutz], basis_wert[schutz]):
        raise SystemExit("geschuetzte Texel nicht bytegleich")
    return tim, info


# ==============================================================================================
# Archiv schreiben + pruefen
# ==============================================================================================
def fnv1a(d):
    h = 2166136261
    for b in d:
        h = ((h ^ b) * 16777619) & 0xFFFFFFFF
    return h


def archiv_bauen(kennung, spec, schreiben=True):
    basis_name = spec["basis"]
    d_basis, o, md1, tim = re2_lesen(basis_name)
    rgba, wert = rgba_aus_tim(tim)
    blatt = uv_maske(md1, [0])
    andere = uv_maske(md1, range(1, len(md1.meshes))) if len(md1.meshes) > 1 else np.zeros_like(blatt)
    basis = dict(name=basis_name, rgba=rgba, wert=wert, md1=md1, tim=tim)
    neu_rgba, proto = REZEPTE[kennung](basis)
    # Schutz: Texel, die ein Nicht-Blatt-Mesh liest (Griff, Beschlag, Anbauteile). Beim Leiter-Rezept
    # sind alle Meshes "Blatt" (Umfaerben erwuenscht) -> dort nur Texel ausserhalb jeder UV schuetzen.
    unbenutzt = ~uv_maske(md1, range(len(md1.meshes)))
    if kennung.startswith("P16"):
        schutz = np.zeros_like(blatt)   # alle Leiter-Meshes werden umgefaerbt
    else:
        schutz = andere                 # auch Ueberlappung Blatt/Griff (DOOR24) bleibt Griff
    neu_tim, qinfo = tim_bauen(neu_rgba, tim, wert, schutz, unbenutzt)
    tim_b = neu_tim.schreiben()
    if len(tim_b) != len(o.tim):
        raise SystemExit("%s: TIM %d B statt %d" % (kennung, len(tim_b), len(o.tim)))
    neu = fmt.Do2("re2", md1=o.md1, skripte=list(o.skripte), tim=tim_b, ton_vorspann=o.ton_vorspann,
                  vh=o.vh, ton_nachspann=o.ton_nachspann, vb=o.vb)
    datei = neu.schreiben()
    # Pruefungen: gleiche Groesse, gleicher Tonteil, gleiche Skripte + MD1, Lader liest, TIM 128x256/8 bit/1 CLUT
    bef = fmt.lader_pruefung(datei)
    if not bef["lesbar"]:
        raise SystemExit("%s: Lader-Pruefung %s" % (kennung, bef["fehler"]))
    rueck = fmt.Do2.lesen(datei)
    lage_alt, lage_neu = o.lage(), rueck.lage()
    assert len(datei) == len(d_basis), (kennung, len(datei), len(d_basis))
    assert datei[:lage_alt["tim"]] == d_basis[:lage_alt["tim"]], "%s: vor der TIM nicht bytegleich" % kennung
    assert lage_neu == lage_alt
    t2, _ = fmt.Tim.lesen(rueck.tim)
    assert (t2.farbtiefe, t2.breite_px, t2.hoehe_px, t2.clut_rect[2] * t2.clut_rect[3]) == (8, 128, 256, 256)
    tab = neu.re2_tabelleneintrag()
    eintrag = dict(kennung=kennung, basis=basis_name, basis_nr=int(basis_name[4:], 16),
                   ton=tab["ton_groesse"], modell=tab["modell_groesse"], sektor=tab["modell_sektor"],
                   datei=len(datei), fnv=fnv1a(datei), sha1=hashlib.sha1(datei).hexdigest(),
                   basis_sha1=hashlib.sha1(d_basis).hexdigest(), tim_off=lage_alt["tim"],
                   rezept=spec["rezept"], ton_familie=spec["ton"], griff=spec["griff"],
                   protokoll=proto, palette=qinfo)
    if schreiben:
        os.makedirs(AUS_DIR, exist_ok=True)
        with open(os.path.join(AUS_DIR, kennung + ".DO2"), "wb") as f:
            f.write(datei)
        os.makedirs(VORSCHAU, exist_ok=True)
        vor, _ = rgba_aus_tim(t2)
        alt = rgba
        bild = np.concatenate([alt[..., :3], np.zeros((256, 4, 3), np.uint8), vor[..., :3]], 1)
        Image.fromarray(bild).resize((bild.shape[1] * 3, 256 * 3), Image.NEAREST).save(
            os.path.join(VORSCHAU, kennung + "_tim.png"))
    return eintrag


# ==============================================================================================
# Zuordnungszeilen der Seiten (Schluessel wie tuer_zuordnung_gen.py: Raum + Flaeche + Band)
# ==============================================================================================
def zeilen_bauen(plan, gebaut):
    z = json.load(open(ZUORDNUNG, encoding="utf-8"))
    seiten_r31 = {}
    for t in z["tueren"]:
        for s in t["seiten"]:
            seiten_r31[s["id"]] = (t, s)
    zeilen = []
    for tid, e in plan["tueren"].items():
        if e["archiv"] not in gebaut:
            continue
        for sp in e["seiten"]:
            if not sp.get("bau") or "variante" not in sp:
                continue
            t, s = seiten_r31[sp["id"]]
            k = s["schluessel"]
            sz = sorted({(x["datei"], x["off"], x["slot"], x["skript"]) for x in k["saetze"]})
            for rn in k["raeume"]:
                zz = dict(seite=sp["id"], tuer=tid, raum=tzg.raum_id(rn), band=k["band"], archiv=e["archiv"],
                          basis=gebaut[e["archiv"]]["basis_nr"], variante=sp["variante"], herkunft=sp["herkunft"],
                          ziel=k.get("ziel"), saetze=[x for x in sz if x[0].startswith(rn)])
                if k["form"] == "rechteck":
                    x, zc, w, dd = k["rect"]
                    zz.update(form=0, x=x + tzg.c_div(w, 2), z=zc + tzg.c_div(dd, 2), hw=abs(w) // 2,
                              hh=abs(dd) // 2, qx=[0] * 4, qz=[0] * 4)
                else:
                    pts = k["pts"]
                    zz.update(form=1, x=0, z=0, hw=0, hh=0, qx=[p[0] for p in pts], qz=[p[1] for p in pts])
                if not zz["saetze"]:
                    raise SystemExit("%s %s: kein Satz in %s" % (sp["id"], tid, rn))
                zeilen.append(zz)
    # Schluessel eindeutig (gleiche Flaeche = gleiche Wahl) und kein Zusammenstoss mit der RE2-Tabelle
    alt = {}
    for zr in tzg.lesen():
        alt[(zr["raum"], zr["form"], zr["x"], zr["z"], zr["hw"], zr["hh"], tuple(zr["qx"]), tuple(zr["qz"]), zr["band"])] = zr["seite"]
    schl = {}
    for zz in zeilen:
        key = (zz["raum"], zz["form"], zz["x"], zz["z"], zz["hw"], zz["hh"], tuple(zz["qx"]), tuple(zz["qz"]), zz["band"])
        if key in alt:
            raise SystemExit("%s: Flaeche schon in der RE2-Tabelle (%s)" % (zz["seite"], alt[key]))
        if key in schl and schl[key] != (zz["archiv"], zz["variante"]):
            raise SystemExit("%s: Flaeche doppelt mit anderer Wahl" % zz["seite"])
        schl[key] = (zz["archiv"], zz["variante"])
    zeilen.sort(key=lambda r: (r["raum"], r["seite"]))
    return zeilen


def inc_schreiben(eintraege, zeilen, pfad):
    L = ["/* Erzeugt von re15_port/tools/tueren/tuer_archiv_bauen.py - NICHT von Hand aendern.",
         " * Runde 33 / Thema T (analysis/befunde_runde33/tueren_rest_plan.md, tueren_rest_pilot.md).",
         " * ⛔ PORT-WAHL, KEINE Original-Adresse: port-eigene Tuerarchive im RE2-Aufbau (Tonteil, SCD, MD1",
         " * bytegleich dem Basis-Archiv, nur die TIM neu) und ihre Zuordnung zu RE1.5-Tuerseiten.",
         " * Dateien: shared_assets/RE15DOOR/<kennung>.DO2; Groessen wie die RE2-Tabelle @0x8009a520",
         " * (Tonteil, Modellteil, Sektor), fnv = FNV-1a 32 ueber die ganze Datei. */",
         "static const re15_tuer_eigen_t re15_tuer_eigen[%d] = {" % max(1, len(eintraege))]
    for i, e in enumerate(eintraege):
        L.append("    /* %d: %s = %s + Textur '%s' (Ton %s) */" % (i + 1, e["kennung"], e["basis"], e["rezept"].replace("*/", "* /"), e["ton_familie"]))
        L.append("    { \"%s\", 0x%02X, %d, %d, %d, %d, 0x%08Xu }," % (e["kennung"], e["basis_nr"], e["ton"], e["modell"],
                                                                     e["sektor"], e["datei"], e["fnv"]))
    L.append("};")
    L.append("")
    idx = {e["kennung"]: i + 1 for i, e in enumerate(eintraege)}
    L.append("/* Zuordnung RE1.5-Tuerseite -> Port-Archiv (Spalten wie gen/tuer_zuordnung.inc; re2_nr = Basis-Archiv")
    L.append(" * = var 15, eigen = Index+1 in re15_tuer_eigen[]). Variante nach tueren_02_re2.md 2.4 (Herkunft je Zeile). */")
    L.append("static const re15_tuer_zeile_t re15_tuer_zeilen_eigen[%d] = {" % max(1, len(zeilen)))
    for z in zeilen:
        ofs = ", ".join("%s@0x%X %s Slot %d" % (x[0], x[1], x[3], x[2]) for x in z["saetze"])
        L.append("    /* %s %s ROOM%04X -> %s | %s | %s (Basis DOOR%02X) V%d (%s) */" % (
            z["seite"], z["tuer"], z["raum"], z["ziel"], ofs, z["archiv"], z["basis"], z["variante"],
            z["herkunft"].replace("*/", "* /")))
        L.append("    { 0x%04X, %d, %d, %6d, %6d, %5d, %5d, {%d, %d, %d, %d}, {%d, %d, %d, %d}, 0x%02X, %d, 0, 0xFF, %d, %d, 0x%05X, %d }," % (
            z["raum"], z["form"], z["band"], z["x"], z["z"], z["hw"], z["hh"], *z["qx"], *z["qz"], z["basis"],
            z["variante"], int(z["seite"][1:]), int(z["tuer"][1:]), z["saetze"][0][1], idx[z["archiv"]]))
    L.append("};")
    with open(pfad, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(L) + "\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--nur", default="")
    ap.add_argument("--pruefen", action="store_true")
    a = ap.parse_args()
    plan = json.load(open(PLAN, encoding="utf-8"))
    kennungen = [k for k in plan["archive"] if k in REZEPTE]
    if a.nur:
        kennungen = [k for k in kennungen if k in a.nur.split(",")]
    eintraege = []
    for k in kennungen:
        e = archiv_bauen(k, plan["archive"][k], schreiben=not a.pruefen)
        eintraege.append(e)
        print("%s: %s -> %d B, fnv %08X, Palette %d (quantisiert %s), %s" % (
            k, e["basis"], e["datei"], e["fnv"], e["palette"]["palette_belegt"], e["palette"]["quantisiert"],
            {x: e["protokoll"][x] for x in e["protokoll"] if x not in ("auswahl",)}))
    gebaut = {e["kennung"]: e for e in eintraege}
    zeilen = zeilen_bauen(plan, gebaut)
    print("Zuordnung: %d Zeilen, %d Seiten, %d Tueren" % (
        len(zeilen), len({z["seite"] for z in zeilen}), len({z["tuer"] for z in zeilen})))
    if a.pruefen or a.nur:
        if a.nur and not a.pruefen:
            print("(--nur: .inc und archive.json NICHT geschrieben)")
        return 0
    inc_schreiben(eintraege, zeilen, AUS_INC)
    with open(AUS_JSON, "w", encoding="utf-8") as f:
        json.dump(dict(archive=eintraege, zeilen=len(zeilen),
                       seiten=sorted({z["seite"] for z in zeilen}),
                       tueren=sorted({z["tuer"] for z in zeilen})), f, ensure_ascii=False, indent=1)
    print("geschrieben:", AUS_INC, AUS_JSON)
    return 0


if __name__ == "__main__":
    sys.exit(main())
