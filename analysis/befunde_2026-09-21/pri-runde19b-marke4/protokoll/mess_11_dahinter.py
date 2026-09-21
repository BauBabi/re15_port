"""Stehen die Gegenstaende rechts der Freistellung (grauer Kasten, blaue Tonne, das
Chromrohr x68..76 y120..140) VOR oder HINTER dem Spieler an Marke F3843?

Zwei unabhaengige Wege:
 (a) Kameramatrix: Sockelzeile -> Weltpunkt am Boden -> Kamera-z, gegen den Spieler.
 (b) Nullmodell OHNE Matrix: ein Gegenstand am Fuss des Spielers haette seinen Sockel
     auf welcher Bildzeile? (Der Spieler steht auf scr y=179, befund.log F3843.)
"""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie

rdt, _ = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
PX, PZ = 418, 26497

pf = geometrie.vz_at_floor(R, t, H, 77.5, 179.5, 0)
print("Gegenprobe Spieler: Bildpunkt (77.5,179.5) -> Kamera-z %.0f (befund.log: vz Fuss 6073)" % pf)
Pp = geometrie.welt_am_boden(R, t, H, 77.5, 179.5, 0)
print("                     -> Welt (%.0f, %.0f)  (befund.log pos 418 / 26497)" % Pp)
BK = lambda v: int(np.floor(1023.0 * v / 65536.0))
print("Spieler-Bucket am Fuss: %d\n" % BK(6073))

print("Sockelzeile -> Kamera-z / Tiefe:")
for name, sx, sy in (("grauer Kasten links   x62 Sockel y155", 62.5, 155.5),
                     ("grauer Kasten rechts  x78 Sockel y153", 78.5, 153.5),
                     ("blaue Tonne           x90 Sockel y149", 90.5, 149.5),
                     ("blaue Tonne rechts    x99 Sockel y148", 99.5, 148.5),
                     ("Stuhl-Standpunkt (aufrecht)   x37 y238", 37.0, 238.5)):
    vz = geometrie.vz_at_floor(R, t, H, sx, sy, 0)
    P = geometrie.welt_am_boden(R, t, H, sx, sy, 0)
    print("  %-40s vz %6.0f  Tiefe %3d  Welt (%7.0f,%7.0f)  %s"
          % (name, vz, BK(vz), P[0], P[1], "VOR dem Spieler" if vz < 6073 else "HINTER dem Spieler"))

print("\nNullmodell ohne Matrix: ein Gegenstand, der am FUSS des Spielers steht, hat seinen")
print("Sockel auf Bildzeile 179 (befund.log scr=(77,179)). Die Sockel oben liegen bei")
print("y148..155, also 24..31 Zeilen HOEHER -> weiter weg. Kein Matrixargument noetig.")

# Das Chromrohr x68..76 y120..140 hat KEINEN sichtbaren Bodenkontakt in diesem Fenster.
# Es liegt aber im Bild UEBER dem grauen Kasten und der Tonne (y120..140 gegen y148..155),
# also weiter oben als beide Sockel; ein Gegenstand VOR dem Spieler muesste seinen Sockel
# unterhalb Zeile 179 haben.
print("\nDas Chromrohr x68..76 y120..140 endet oben im Bild und zeigt keinen Bodenkontakt.")
print("Es liegt ganz oberhalb der Sockelzeilen des Kastens (155) und der Tonne (149) und")
print("damit weit oberhalb der Sockelzeile 179 des Spielers.")
