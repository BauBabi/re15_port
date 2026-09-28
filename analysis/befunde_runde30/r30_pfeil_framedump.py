#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_pfeil_framedump.py - Nachschliff Runde 30, Spur pfeil: Abnahme am FRAMEDUMP.

Wertet die Framedumps eines Laufs von r30_pfeil_lauf.sh aus (960x720 = 3x, beschleunigter
Renderer, der komplett komponierte Frame VOR SDL_RenderPresent):

  * Welche Textseite steht still bei (25,30)? Bestimmt aus dem BILD selbst: je Referenzseite
    (FILE25_title/p01..p17_page.TIM, gezeichnet wie RE2 @0x80076170-84) der Anteil der
    Glyphen-Pixel, deren Farbe im Abzug gleich ist (5 Bit je Kanal). Nur Bilder mit einer
    Seite >= 90 % gleich zaehlen als "Seite steht" (sonst faehrt sie gerade).
  * Glyphen-Pixel = sichtbarer Texel der Textseite (CLUT-Farbe != 0x0000). Ueberdeckt =
    Glyphen-Pixel, dessen Farbe im Abzug NICHT die der Textseite ist.
  * Pfeil-Lage: Rahmen der nicht-schwarzen Pixel links der Textseite (x < 25) und rechts von
    ihr (x > 280), jeweils y 100..135.

Aufruf: python r30_pfeil_framedump.py <laufverzeichnis> [ausgabe.txt] [--bogen bogen.png]
"""
import os, re, struct, sys
import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")
TEXT_X, TEXT_Y = 25, 30          # @0x80076170 / @0x8007617c (RE2)
NAMEN = ["t"] + ["p%02d" % p for p in range(1, 18)]


def tim4(path):
    b = open(path, "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    p = 8
    ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, p)
    clut = np.array(struct.unpack_from("<%dH" % (cw * ch), b, p + 12), dtype=np.uint16)
    p += ln
    ln, x, y, w, h = struct.unpack_from("<IHHHH", b, p)
    d = np.frombuffer(b[p + 12:p + ln], dtype=np.uint8).reshape(h, w * 2)
    idx = np.empty((h, w * 4), dtype=np.uint8)
    idx[:, 0::2] = d & 15
    idx[:, 1::2] = d >> 4
    col = clut[idx]
    return col                              # 15-Bit-Farbe je Texel, 0 = durchsichtig


def referenzen():
    refs = []
    for n in NAMEN:
        f = "FILE25_title_page.TIM" if n == "t" else "FILE25_%s_page.TIM" % n
        col = tim4(os.path.join(FILES, f))
        h, w = col.shape
        vis = col != 0
        r5 = (col & 31).astype(np.int16)
        g5 = ((col >> 5) & 31).astype(np.int16)
        b5 = ((col >> 10) & 31).astype(np.int16)
        refs.append((n, vis, np.stack([r5, g5, b5], axis=-1), h, w))
    return refs


def rahmen(maske, x0, y0):
    ys, xs = np.nonzero(maske)
    if len(xs) == 0:
        return None
    return (x0 + int(xs.min()), y0 + int(ys.min()), x0 + int(xs.max()), y0 + int(ys.max()))


def main():
    lauf = sys.argv[1]
    aus_pfad = sys.argv[2] if len(sys.argv) > 2 and not sys.argv[2].startswith("--") else None
    bogen = sys.argv[sys.argv.index("--bogen") + 1] if "--bogen" in sys.argv else None
    refs = referenzen()
    mess = os.path.join(lauf, "mess")
    bilder = sorted(f for f in os.listdir(mess) if f.endswith(".ppm"))
    zeilen, gruppen, proben = [], {}, {}
    for f in bilder:
        F = int(re.findall(r"\d+", f)[0])
        img = np.asarray(Image.open(os.path.join(mess, f)).convert("RGB"))
        sy, sx = img.shape[0] // 240, img.shape[1] // 320
        s = img[sy // 2::sy, sx // 2::sx][:240, :320].astype(np.int16) >> 3   # 5 Bit
        best = None
        for n, vis, rgb, h, w in refs:
            ab = s[TEXT_Y:TEXT_Y + h, TEXT_X:TEXT_X + w]
            gleich = np.all(ab == rgb, axis=-1) & vis
            q = gleich.sum() / max(1, vis.sum())
            if best is None or q > best[1]:
                best = (n, q, int(vis.sum() - gleich.sum()), vis & ~np.all(ab == rgb, axis=-1))
        n, q, ueber, ueber_maske = best
        nicht_schwarz = np.any(s != 0, axis=-1)
        links = rahmen(nicht_schwarz[100:136, 0:TEXT_X], 0, 100)
        rechts = rahmen(nicht_schwarz[100:136, TEXT_X + 256:320], TEXT_X + 256, 100)
        steht = q >= 0.90
        ub = rahmen(ueber_maske, TEXT_X, TEXT_Y) if ueber else None
        zeilen.append("F%d %s %.4f ueberdeckt %d %s | links %s | rechts %s%s" % (
            F, n, q, ueber, ub or "", links, rechts, "" if steht else "  (faehrt)"))
        if steht:
            key = (n, links[0] if links else None, rechts[0] if rechts else None)
            g = gruppen.setdefault(key, [0, 0, F, F])
            g[0] += 1
            g[1] = max(g[1], ueber)
            g[3] = F
            proben.setdefault(key, (F, s))
    aus = ["Lauf %s: %d Abzuege" % (lauf, len(bilder)), ""]
    aus.append("Seite | Pfeil links x0 | Pfeil rechts x0 | Bilder | max. ueberdeckte Glyphen-Pixel | erstes..letztes Bild")
    summe = 0
    seiten_stellungen = {}
    for key in sorted(gruppen, key=lambda k: (NAMEN.index(k[0]), str(k[1]), str(k[2]))):
        c, m, f0, f1 = gruppen[key]
        summe += m
        seiten_stellungen.setdefault(key[0], set()).add((key[1], key[2]))
        aus.append("%-4s | %-4s | %-4s | %4d | %4d | F%d..F%d" % (key[0], key[1], key[2], c, m, f0, f1))
    aus.append("")
    aus.append("Seiten mit stehender Textseite: %d von 18; Summe der Maxima ueberdeckter Glyphen-Pixel: %d"
               % (len(seiten_stellungen), summe))
    aus.append("")
    aus += zeilen
    txt = "\n".join(aus) + "\n"
    if aus_pfad:
        open(aus_pfad, "w", encoding="utf-8").write(txt)
    sys.stdout.write("\n".join(aus[:len(gruppen) + 6]) + "\n")
    if bogen:
        # Kontaktbogen: je (Seite, Stellung) ein Ausschnitt x 0..60 / 250..320, y 95..140,
        # 4x vergroessert, Glyphen-Pixel der Referenz, die im Abzug fehlen, rot umrandet.
        keys = sorted(proben, key=lambda k: (NAMEN.index(k[0]), str(k[1]), str(k[2])))
        zw, zh = (60 + 70) * 4 + 8, 45 * 4
        B = Image.new("RGB", (zw * 2 + 8, (zh + 14) * ((len(keys) + 1) // 2) + 8), (40, 40, 40))
        from PIL import ImageDraw
        dr = ImageDraw.Draw(B)
        for i, key in enumerate(keys):
            F, s = proben[key]
            rgb8 = (s.astype(np.uint8) << 3)
            teil = np.concatenate([rgb8[95:140, 0:60], np.full((45, 2, 3), 80, np.uint8),
                                   rgb8[95:140, 250:320]], axis=1)
            t = Image.fromarray(teil).resize((teil.shape[1] * 4, teil.shape[0] * 4), Image.NEAREST)
            x0 = 4 + (i % 2) * (zw + 4)
            y0 = 4 + (i // 2) * (zh + 14)
            B.paste(t, (x0, y0 + 12))
            dr.text((x0, y0), "%s links %s rechts %s  F%d" % (key[0], key[1], key[2], F), fill=(255, 255, 0))
        B.save(bogen)


main()
