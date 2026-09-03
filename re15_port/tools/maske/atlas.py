"""Baut aus Hintergrundbild + markierter Region einen 8-bpp-Sony-TIM als Vordergrundatlas.

WARUM ueberhaupt ein Atlas: Die Maskenrecords adressieren die Textur mit u8-Koordinaten
(srcX, srcY je ein Byte, @0x80039408/0x80039418). Ein 320 Pixel breites Bild ist damit
NICHT adressierbar — der Hintergrund kann also nicht direkt als Maskentextur dienen,
obwohl er genau die richtigen Pixel enthaelt. Genau deshalb legt auch das Original je
Cut einen eigenen 256-breiten Atlas an: er ist eine gepackte Kopie derselben Pixel,
weil der Hintergrund im Framebuffer liegt und nicht sampelbar ist.

Fuer die Cuts, in denen der Prototyp gar keinen Atlas hinterlassen hat, wird er hier
erzeugt: die markierten Bereiche werden in ein 256x256-Blatt gepackt (Regalverfahren),
die Pixel kommen aus dem Hintergrund, nicht markierte Stellen bekommen Palettenindex 0
und sind damit durchsichtig.

FARBEN: 255 nutzbare Eintraege (Index 0 ist die Transparenz). Der dabei entstehende
Quantisierungsfehler ist derselbe, den die Original-Atlanten auch haben — gemessen
liegt deren Abweichung zum MDEC-Hintergrund bei 12.4 von 765 (Summe ueber RGB).
"""
import struct

import numpy as np
from PIL import Image

ATLAS_W = 256
ATLAS_H = 256
CLUT_X = 0
CLUT_Y = 480            # wie im Original (`ori v0,zero,0x7800` @0x80039498 = getClut(0,480))


def shelf_pack(boxes, w=ATLAS_W, h=ATLAS_H):
    """Regalverfahren: Kaesten nach Hoehe sortiert in Reihen legen.

    -> {index: (ax, ay)} fuer die untergebrachten, plus Liste der nicht passenden.
    """
    order = sorted(range(len(boxes)), key=lambda i: -boxes[i][3])
    place = {}
    rejected = []
    x = y = row_h = 0
    for i in order:
        bw, bh = boxes[i][2], boxes[i][3]
        if bw > w or bh > h:
            rejected.append(i)
            continue
        if x + bw > w:
            x = 0
            y += row_h
            row_h = 0
        if y + bh > h:
            rejected.append(i)
            continue
        place[i] = (x, y)
        x += bw
        row_h = max(row_h, bh)
    return place, rejected


def build(bg_rgb, region, boxes):
    """-> (tim_bytes, placement) oder (None, None).

    bg_rgb: (240,320,3) uint8 — der dekodierte Hintergrund des Cuts
    region: (240,320) bool   — die markierte Vordergrundflaeche
    boxes:  [(x, y, w, h)]   — Bildschirm-Rechtecke (aus geom.rects_from_mask)
    """
    place, rejected = shelf_pack(boxes)
    if not place:
        return None, None

    idx = np.zeros((ATLAS_H, ATLAS_W), np.uint8)
    rgb = np.zeros((ATLAS_H, ATLAS_W, 3), np.uint8)
    opaque = np.zeros((ATLAS_H, ATLAS_W), bool)
    for i, (ax, ay) in place.items():
        x, y, w, h = boxes[i]
        rgb[ay:ay + h, ax:ax + w] = bg_rgb[y:y + h, x:x + w]
        opaque[ay:ay + h, ax:ax + w] = region[y:y + h, x:x + w]

    # Palette NUR aus den undurchsichtigen Pixeln bilden — Schwarz aus den Luecken
    # wuerde sonst Eintraege verbrauchen und die Farben der echten Flaeche verschlechtern.
    pix = rgb[opaque]
    if len(pix) == 0:
        return None, None
    tmp = Image.fromarray(pix.reshape(-1, 1, 3))
    pal_img = tmp.quantize(colors=255, method=Image.MEDIANCUT)
    pal = np.array(pal_img.getpalette()[:255 * 3], np.uint8).reshape(255, 3)

    # Zuordnung aller undurchsichtigen Pixel auf die Palette (Index 1..255).
    flat = rgb[opaque].astype(np.int32)
    d = ((flat[:, None, :] - pal[None, :, :].astype(np.int32)) ** 2).sum(2)
    idx[opaque] = (np.argmin(d, 1) + 1).astype(np.uint8)

    # CLUT: 256 Eintraege BGR555. Index 0 = 0x0000 (bei ABE=0 durchsichtig).
    clut = np.zeros(256, np.uint16)
    r = (pal[:, 0] >> 3).astype(np.uint16)
    g = (pal[:, 1] >> 3).astype(np.uint16)
    b = (pal[:, 2] >> 3).astype(np.uint16)
    clut[1:] = r | (g << 5) | (b << 10)

    tim = bytearray()
    tim += struct.pack("<II", 0x10, 0x09)                     # Magic, 8 bpp + CLUT
    tim += struct.pack("<IHHHH", 12 + 512, CLUT_X, CLUT_Y, 256, 1)
    tim += clut.tobytes()
    tim += struct.pack("<IHHHH", 12 + ATLAS_W * ATLAS_H, 0, 0, ATLAS_W // 2, ATLAS_H)
    tim += idx.tobytes()
    return bytes(tim), place
