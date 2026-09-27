#!/usr/bin/env python3
"""Erzeugt das 3D-Modell der SICHERUNG (Item 0x40 "Fuse") aus den Original-Pixeln.

WARUM DIESES WERKZEUG UEBERHAUPT:
Das Sicherungs-Raetsel in ROOM1050 ist im Auslieferungsstand herausgeschnitten, die
Kunst dafuer aber vollstaendig vorhanden (Cut 7 = Sockel leer, Cut 8 = Sicherung drin).
Das Item 0x40 hat Name, Inventar-Icon und Item-Bild — was fehlt, ist ein WELT-Modell
fuer die Fundstelle (ein Item, das sichtbar im Raum liegt, ist im Port ein RDT-Prop,
siehe scd_vm.c:405-418 "AUFGENOMMENE ITEMS: Welt-Modelle").

⛔ ABGRENZUNG — was hier Beleg ist und was Nachbau:
  BELEG    Silhouette, Gliederung und Farben stammen PIXELWEISE aus den ausgelieferten
           Hintergruenden ROOM1050 BG07/BG08. Der Massstab ist aus der RDT-Kamera und
           der SCA-Wand GERECHNET (s.u.), nicht geschaetzt.
  NACHBAU  Die Rundung (Achtkant) und die Rueckseite. Das Original hat fuer diesen
           Gegenstand KEIN 3D-Modell — weder RE1.5 noch RE2 haben Item-Meshes; beide
           zeigen Items als 2D-Bilder. Es gibt hier also nichts zu disassemblieren.
           Die Vorderseite ist original, die Rueckseite ist gespiegelt.

MESSUNGEN (alle reproduzierbar, Werkzeuge in Klammern):
  Kamera Cut 7 UND Cut 8, ROOM1050.RDT Kameratabelle @0x60 (+7*32 / +8*32):
      flag=0 fov=26684 pos=(16171,-2733,-8185) tgt=(18240,-1974,-8706)
      -> IDENTISCH bis auf pri_offset (0x518 / 0x51C) = zwei Zustaende EINES Blicks.
  Projektionsdistanz H = fov>>7 = 208   (render_pc.c:30-38 fov_to_screen_dist)
  Wandebene x=17200 aus der SCA-Zelle x[17200..18100] z[-15500..10200] type=1
      (RDT SCA-Block @0x550, Eintragslayout re15_rdt.h:63-71)
  -> z_view der Kastenebene ~1055, daraus 5.07 Welteinheiten je Bildpixel;
     die Montageplatte steht vor der Wand, angesetzt mit 4.9 units/px.
  Massstabsanker: Spieler-Kopfhoehe 1700 units (main.c:5296) => 1 unit ~ 1 mm.

  Silhouette als DIFFERENZ BG08-BG07 (= genau das, was hinzukommt; eine Schwelle auf
  BG08 allein verliert die beschattete untere Kappe — erster Anlauf mass 14 px zu kurz):
      y 92..162, Breite durchgehend 10-13 px
  Gliederung aus dem Helligkeitsprofil der Mittelachse (x195..201):
        obere Metallkappe  y  97..113  lum 218-239, glatt   (17 px)
        Keramikkoerper     y 114..151  lum 125-230, unruhig (38 px, Beschriftung)
        untere Metallkappe y 152..162  lum  73-116, dunkel  (11 px, vom Klemmenblock
                                       angeschnitten -> fuer das freistehende Modell
                                       symmetrisch zur oberen Kappe angesetzt)
  Massstab AUS DER RUECKPROJEKTION abgeleitet (sicherung_abnahme.py), nicht vorgerechnet:
        5.63 Welteinheiten je Bildpixel auf der Ebene x=17100

  Daraus die Modellmasse (units = mm):
        Gesamtlaenge 406, Keramik D59.3 / Laenge 214, Kappen D60.7 / Laenge 96

Aufruf:
    python re15_port/tools/sicherung_modell.py --ziel build/sicherung
erzeugt sicherung.obj / .mtl / _tex.png und eine Vergleichsansicht.
"""
import argparse
import io
import os
import struct
import sys

import numpy as np
from PIL import Image

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# --- gemessene Geometrie, Einheiten = Welteinheiten (1 unit ~ 1 mm) ---------
# Durchmesser NICHT mit dem Laengen-Massstab rechnen: die Silhouette eines Zylinders
# steht senkrecht zur Blickachse, dort gilt D = Breite_px * z_view / H (z_view 1014..1151).
# Gemessen: obere Kappe D 60.7, Keramik D 59.3. Die Achtkant-Silhouette ist 1.98*R breit
# (Eckenlage bei 67.5 Grad zur Blickachse), daher R = D / 1.98.
# Rueckgekoppelt aus sicherung_abnahme.py: der GERENDERTE Achtkant projiziert rund
# 9 % breiter als 1.98*R — die vordere Facette liegt naeher an der Kamera und weitet
# die Silhouette perspektivisch auf. Der Faktor 0.919 ist gemessen, nicht gewaehlt:
# er bringt die projizierte Breite auf die des Originals.
KAPPE_R = 28.2     # D 60.7 <- 12.15 px * 1039 / 208, dann * 0.919
KOERP_R = 27.6     # D 59.3 <- 11.28 px * 1093 / 208, dann * 0.919
L_KAPPE_O = 96.0   # 17 px * 5.63
L_KOERPER = 214.0  # 38 px * 5.63
L_KAPPE_U = 96.0   # wie oben: die untere Kappe ist im Bild vom Klemmenblock
                   # angeschnitten (nur 11 px sichtbar) — ein freistehender
                   # Gegenstand ist symmetrisch.
SEITEN = 8         # Achtkant — PSX-Budget (siehe Memory psx-laufzeitbudget)

# --- Fundstellen der Original-Pixel ----------------------------------------
BG_DIRS = [
    os.path.join(REPO, "build", "bg_ppm"),
    os.path.join(REPO, ".claude", "worktrees", "wf_5ebaf1dc-c6e-1", "build", "bg_ppm"),
]
# (x0, y0, breite, hoehe) der Sicherung im 320x240-Hintergrund
QUELLE_BG08 = (191, 97, 13, 66)    # rechte, frisch eingesetzte Sicherung
QUELLE_BG07 = (141, 100, 12, 56)   # linke, im Auslieferungsstand vorhandene


def bg_laden(name):
    for d in BG_DIRS:
        p = os.path.join(d, name)
        if os.path.exists(p):
            return np.asarray(Image.open(p).convert("RGB"), np.uint8)
    raise SystemExit("Hintergrund %s nicht gefunden — probe_bg_dump laufen lassen "
                     "(re15_port/tools/maske/editor.py:21)" % name)


def textur_bauen(breite=32, hoehe=128):
    """Mantel-Textur AUS DEN ORIGINALPIXELN.

    Die Sicherung ist im Hintergrund 13 px breit; das ist die auf die Bildebene
    projizierte VORDERSEITE, also 180 Grad Mantel. Fuer die volle Umwicklung wird
    sie gespiegelt — die Rueckseite sieht das Original nie.
    Es wird NICHT interpoliert (NEAREST): jeder Texel traegt einen Originalpixel,
    keine erfundenen Zwischenwerte.
    """
    bg = bg_laden("ROOM10508.ppm")
    x0, y0, w, h = QUELLE_BG08
    roh = bg[y0:y0 + h, x0:x0 + w]                      # 66 x 13 Originalpixel
    halb = Image.fromarray(roh).resize((breite // 2, hoehe), Image.NEAREST)
    voll = Image.new("RGB", (breite, hoehe))
    voll.paste(halb, (0, 0))
    voll.paste(halb.transpose(Image.FLIP_LEFT_RIGHT), (breite // 2, 0))
    return voll, roh


def ring(radius, y, n=SEITEN, phase=0.5):
    """Ein Achtkant-Ring. phase=0.5 legt eine FLAECHE nach vorn (nicht eine Kante),
    damit die Vorderseite dem Originalpixel-Streifen entspricht."""
    import math
    return [(radius * math.sin(2 * math.pi * (i + phase) / n), y,
             radius * math.cos(2 * math.pi * (i + phase) / n)) for i in range(n)]


def modell_bauen():
    """Rotationskoerper aus 4 Ringen + 2 Deckeln. Ursprung: Mitte des Fusses."""
    y0 = 0.0
    y1 = L_KAPPE_U
    y2 = L_KAPPE_U + L_KOERPER
    y3 = L_KAPPE_U + L_KOERPER + L_KAPPE_O

    ringe = [
        (ring(KAPPE_R, y0), 0.00),   # Fuss der unteren Kappe
        (ring(KAPPE_R, y1), 0.26),   # Oberkante untere Kappe
        (ring(KOERP_R, y1), 0.26),   # Absatz auf den Keramikdurchmesser
        (ring(KOERP_R, y2), 0.74),   # Oberkante Keramik
        (ring(KAPPE_R, y2), 0.74),   # Absatz auf die obere Kappe
        (ring(KAPPE_R, y3), 1.00),   # Deckel der oberen Kappe
    ]

    v, vt, faces = [], [], []
    for pts, _ in ringe:
        for p in pts:
            v.append(p)
    # UV: u laeuft um den Mantel, v ist die gemessene Hoehe
    for _, tv in ringe:
        for i in range(SEITEN + 1):
            vt.append((i / SEITEN, tv))

    def vi(r, i):
        return r * SEITEN + (i % SEITEN) + 1

    def ti(r, i):
        return r * (SEITEN + 1) + i + 1

    for r in range(len(ringe) - 1):
        for i in range(SEITEN):
            a, b = vi(r, i), vi(r, i + 1)
            c, d = vi(r + 1, i + 1), vi(r + 1, i)
            ta, tb = ti(r, i), ti(r, i + 1)
            tc, td = ti(r + 1, i + 1), ti(r + 1, i)
            faces.append(((a, ta), (b, tb), (c, tc), (d, td)))

    # Deckel: Mittelpunkte
    v.append((0.0, y0, 0.0)); unten_c = len(v)
    v.append((0.0, y3, 0.0)); oben_c = len(v)
    vt.append((0.5, 0.02)); t_unten = len(vt)
    vt.append((0.5, 0.98)); t_oben = len(vt)
    for i in range(SEITEN):
        faces.append(((unten_c, t_unten), (vi(0, i + 1), t_unten), (vi(0, i), t_unten)))
        r = len(ringe) - 1
        faces.append(((oben_c, t_oben), (vi(r, i), t_oben), (vi(r, i + 1), t_oben)))
    return v, vt, faces


def schreiben(ziel, v, vt, faces, tex):
    os.makedirs(ziel, exist_ok=True)
    tex.save(os.path.join(ziel, "sicherung_tex.png"))
    with io.open(os.path.join(ziel, "sicherung.mtl"), "w", encoding="utf-8") as f:
        f.write("newmtl sicherung\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\nd 1\nillum 1\n"
                "map_Kd sicherung_tex.png\n")
    with io.open(os.path.join(ziel, "sicherung.obj"), "w", encoding="utf-8") as f:
        f.write("# RE1.5 Item 0x40 'Fuse' — Welt-Modell fuer die Fundstelle.\n")
        f.write("# Silhouette+Farben: ROOM1050 BG08 (192,97,13x68) / BG07 (141,100,12x56).\n")
        f.write("# Massstab: RDT-Kamera Cut7/8 @0x60 fov 26684 (H=208), SCA-Wand x=17200,\n")
        f.write("#           4.9 Welteinheiten je Bildpixel; 1 unit ~ 1 mm.\n")
        f.write("# Laenge %.0f, Keramik ⌀%.0f, Kappen ⌀%.0f. +Y = Laengsachse.\n"
                % (L_KAPPE_U + L_KOERPER + L_KAPPE_O, KOERP_R * 2, KAPPE_R * 2))
        f.write("mtllib sicherung.mtl\no Sicherung\nusemtl sicherung\n")
        for p in v:
            f.write("v %.3f %.3f %.3f\n" % p)
        for t in vt:
            f.write("vt %.5f %.5f\n" % t)
        for fc in faces:
            f.write("f " + " ".join("%d/%d" % (a, b) for a, b in fc) + "\n")
    return len(v), len(faces)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ziel", default=os.path.join(REPO, "build", "sicherung"))
    a = ap.parse_args()
    tex, roh = textur_bauen()
    v, vt, faces = modell_bauen()
    nv, nf = schreiben(a.ziel, v, vt, faces, tex)
    print("Sicherung: %d Punkte, %d Flaechen, Textur %dx%d aus %d Originalpixeln"
          % (nv, nf, tex.size[0], tex.size[1], roh.shape[0] * roh.shape[1]))
    print("Laenge %.0f units (%.1f cm), Keramik D%.0f, Kappen D%.0f"
          % (L_KAPPE_U + L_KOERPER + L_KAPPE_O,
             (L_KAPPE_U + L_KOERPER + L_KAPPE_O) / 10.0, KOERP_R * 2, KAPPE_R * 2))
    print("geschrieben nach", a.ziel)


if __name__ == "__main__":
    main()
