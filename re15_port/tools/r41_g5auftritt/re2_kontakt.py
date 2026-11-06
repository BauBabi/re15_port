#!/usr/bin/env python3
"""re2_kontakt.py - Runde 41 G5-Auftritt: Kontaktblatt der RE2-Szene (room7040 sub06 ab der Armierung).

Was es zeigt: je Schluesselbild die RE2-Kamera mit ihrem RE2-Hintergrund (info/re2leon/COMMON/BSS/ROOM704/ROOM704nn.bmp, RE2-Decoder
des Extraktionswerkzeugs) und darin als MARKEN, wo RE2 die Figuren hat:
  - Leon (gelber Kasten 600 x 1800): RE2s Pos_set-Lagen der Szene (@0x018B0 1400, @0x0192A 6500, @0x01956 11340, @0x0196C 14850, z -23570)
  - G5 (rote Linie): Ursprung + 4494 = Massenfront (g5-optik.md 5), aus der Port-Simulation des RE2-Overlays (enemy_ai_boss_g5.c,
    [T0] -9000 @0x801011d0, Root-Spur Clip 1 / Clip 4)
  - Tentakel-Spitzen (cyan): aus derselben Simulation, nur ausgefahrene Arme (scaleX > 0)
Es ist KEINE Aufnahme aus RE2 (kein Spielstand im Zug, Dossier 6), sondern RE2-Kamera + RE2-Bild + RE2-Ablauf.

Aufruf: re2_kontakt.py <spur_5090.txt> <ausgabe.png>
  (Spur: RE15_G5A_SPUR=<pfad> test_r41_g5auftritt -> <pfad>_5090.txt; Spalten s. Testquelle)
"""
import math
import os
import sys

from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
BSS = os.path.join(REPO, "info", "re2leon", "COMMON", "BSS", "ROOM704")

# RE2 room7040 RID (@0x64 + n*32): fov, Auge, Ziel
KAM = {
    9: (29623, (2700, -882, -29052), (-1584, -4608, -14850)),
    10: (26684, (8946, -936, -25902), (-1710, -3546, -21690)),
    5: (26684, (14220, -1854, -24606), (3582, -2196, -23076)),
    6: (26684, (21042, -882, -24390), (11898, -3150, -23076)),
}
# (Bild, RE2-Schnitt, Text)
BILDER = [
    (0, 9, "Armierung [T0] @0x018DE"),
    (12, 9, "[T1] Arm 0 tritt aus (t=10)"),
    (45, 9, "Arme 0/2/3 draussen (t=10/30/40)"),
    (91, 9, "[T2] Hereinziehen beginnt"),
    (180, 9, "[T2] Koerper kommt herein"),
    (271, 9, "[T3] Clip 3, aufrichten"),
    (400, 9, "[T5] Speere 0x901"),
    (437, 10, "Cut_chg(10) @0x0193A"),
    (560, 10, "[T6] Clip 4 kriecht +3946"),
    (700, 10, "[T7/T8] Clip 2"),
    (807, 5, "Cut_chg(5) @0x01950"),
    (950, 5, "[T10] Wedeln 0xA01"),
    (1126, 5, "[T14] -> Sub 1 Kampf"),
    (1162, 6, "Cut_chg(6), Plc_ret @0x0197C"),
]


def ansicht(fov, pos, ziel):
    """re15_camera_build_view (FUN_80053ca4-Zwilling): Ganzzahl-LookAt, H = fov >> 7."""
    dx, dy, dz = (ziel[i] - pos[i] for i in range(3))
    dist = math.isqrt(dx * dx + dy * dy + dz * dz)
    hor = math.isqrt(dx * dx + dz * dz)
    sp, cp = int(-dy * 4096 / dist), int(hor * 4096 / dist)
    sy, cy = int(dx * 4096 / hor), int(dz * 4096 / hor)
    r = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    t = [(r[k * 3] * -pos[0] + r[k * 3 + 1] * -pos[1] + r[k * 3 + 2] * -pos[2]) >> 12 for k in range(3)]
    return r, t, fov >> 7


def proj(v, p):
    r, t, h = v
    x = (r[0] * p[0] + r[1] * p[1] + r[2] * p[2]) / 4096 + t[0]
    y = (r[3] * p[0] + r[4] * p[1] + r[5] * p[2]) / 4096 + t[1]
    z = (r[6] * p[0] + r[7] * p[1] + r[8] * p[2]) / 4096 + t[2]
    if z <= 64:
        return None
    return (160 + h * x / z, 120 + h * y / z)


def leon_x(b):
    return 1400 if b < 437 else (6500 if b < 807 else (11340 if b < 1162 else 14850))


def main():
    spur = {}
    with open(sys.argv[1]) as f:
        for zeile in f:
            w = [int(v) for v in zeile.split()]
            spur[w[0]] = w
    S = 2
    kacheln = []
    for b, schnitt, text in BILDER:
        fov, pos, ziel = KAM[schnitt]
        v = ansicht(fov, pos, ziel)
        im = Image.open(os.path.join(BSS, "ROOM704%02d.bmp" % schnitt)).convert("RGB").resize((320 * S, 240 * S))
        d = ImageDraw.Draw(im)

        def linie(a, c, farbe, breite=2):
            pa, pc = proj(v, a), proj(v, c)
            if pa and pc:
                d.line([(pa[0] * S, pa[1] * S), (pc[0] * S, pc[1] * S)], fill=farbe, width=breite)

        w = spur.get(min(b, max(spur)))
        if w:
            front = w[4] + 4494
            for z in (-26884, -23400, -19870):
                linie((front, 0, z), (front, -4000, z), (255, 40, 40), 3)
            linie((front, -4000, -26884), (front, -4000, -19870), (255, 40, 40), 3)
            for i in range(4):
                sc, tx, ty, tz = w[11 + i * 4: 15 + i * 4]
                if sc > 0:
                    p = proj(v, (tx, ty, tz))
                    if p:
                        d.ellipse([p[0] * S - 6, p[1] * S - 6, p[0] * S + 6, p[1] * S + 6], outline=(0, 255, 255), width=3)
        lx = leon_x(b)
        for (a, c) in (((lx - 300, 0, -23570), (lx - 300, -1800, -23570)), ((lx + 300, 0, -23570), (lx + 300, -1800, -23570)),
                       ((lx - 300, -1800, -23570), (lx + 300, -1800, -23570)), ((lx - 300, 0, -23570), (lx + 300, 0, -23570))):
            linie(a, c, (255, 230, 0), 2)
        d.rectangle([0, 0, 320 * S, 22], fill=(0, 0, 0))
        d.text((4, 5), "RE2 Schnitt %d  Bild %d: %s" % (schnitt, b, text), fill=(255, 255, 0))
        kacheln.append(im)
    spalten = 4
    zeilen = (len(kacheln) + spalten - 1) // spalten
    blatt = Image.new("RGB", (320 * S * spalten, 240 * S * zeilen + 40), (0, 0, 0))
    for k, im in enumerate(kacheln):
        blatt.paste(im, ((k % spalten) * 320 * S, (k // spalten) * 240 * S))
    d = ImageDraw.Draw(blatt)
    d.text((6, 240 * S * zeilen + 8), "RE2 room7040 sub06 ab der Armierung (30 Bilder/s). RE2-Hintergrund + RE2-Kamera; Marken: gelb Leon "
           "(RE2-Pos_set), rot G5-Massenfront, cyan Tentakel-Spitzen (Simulation des RE2-Overlays). Keine Emulator-Aufnahme.",
           fill=(255, 255, 255))
    blatt.save(sys.argv[2])
    print("geschrieben:", sys.argv[2], blatt.size)


if __name__ == "__main__":
    main()
