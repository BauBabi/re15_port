#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_pfeil_ueberdeckung.py - Nachschliff Runde 30, Spur pfeil.

Rechnet OHNE das Spiel aus den Original-Dateien:
  (1) welche Pixel der Blaetter-Pfeile auf sichtbaren Texeln der Textseite liegen, fuer
      a) RE2s Pfeile und Ende-Marke an RE2s Lage (FUN_800724b4 / FUN_800761b8),
      b) RE1.5s Pfeile an RE1.5s Lage (Negativ-Kontrolle = der Stand vor dem Umbau),
      je Seite (Titel + p01..pN, N = letzte vorhandene Datei) und je Wipp-Stellung;
  (2) RE2s Wipp-Takt (Zaehler 0x800d5c18, Stellung 0x800d5c19) fuer beide Leser.

Belege (info/re2leon/PSX.EXE, selbst disassembliert):
  Sprites anlegen FUN_80075fd0: Pfeil-Paare u = 42 / 28 (`addiu s3,zero,56` @0x80076104,
    `addiu s3,s3,-14` @0x80076108 / @0x80076168), v = 12 (`sb s6(=12),-1(s0)` @0x80076140),
    w = 12 (`sh s6,2(s0)` @0x80076144), h = 13 (`addiu v0,zero,13` @0x80076128),
    CLUT GetClut(256,492) (@0x80076118-1c), Code 0x66 (@0x80076120), r=g=b=128 (@0x80076130-38)
  Zeichnen FUN_800724b4 (Aufnahme-Leser) / FUN_800761b8 (FILE-Schirm):
    Seite == max -> Ende-Marke x 280 y 110 u 56 v 12 42x14 CLUT (256,490) (@0x800725cc-604)
    sonst Pfeil rechts x = 282 + 3*Stellung, y 110 (@0x80072650-670)
    Pfeil links x = 12 - 3*Stellung, y 110 (@0x800726b8-d4), nur Zustand 0 und Seite != 0
      (@0x800726d8-f8)
  CLUT-Zeilen: Lader-Wort 0x0a1b @0x80068588 -> VRAM-Zeile 490 + k = Datei-Zeile k.
  Textseite bei (25,30): @0x80076170-84.
RE1.5 (Negativ-Kontrolle): emit_file_arrows, DEBUG.BIN 0x800c7528: links x = 0x14 - off,
  y 0x70, 16x16, uv (0x70,0x38), CLUT TEX.TIM Zeile 15 (Selektor 7); rechts x = 0x11c + off,
  uv (0x70,0x48), CLUT Zeile 15/12/13 je Stellung; off = 0 oder 4 (@0x800c75ac-e4).

Aufruf: python analysis/befunde_runde30/r30_pfeil_ueberdeckung.py [ausgabedatei]
"""
import os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
ST0 = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ST0.TIM")
TEX = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "DATA", "TEX.TIM")
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")
TEXT_X, TEXT_Y = 25, 30          # @0x80076170 / @0x8007617c


def tims(b):
    """Alle TIMs einer Datei: [(offset, flags, clut(x,y,w,h,[werte]) | None, bild(x,y,w,h,bytes))]."""
    o, out = 0, []
    while o + 8 <= len(b):
        magic, flags = struct.unpack_from("<II", b, o)
        if magic != 0x10:
            break
        p = o + 8
        clut = None
        if flags & 8:
            ln, x, y, w, h = struct.unpack_from("<IHHHH", b, p)
            clut = (x, y, w, h, struct.unpack_from("<%dH" % (w * h), b, p + 12))
            p += ln
        ln, x, y, w, h = struct.unpack_from("<IHHHH", b, p)
        out.append((o, flags, clut, (x, y, w, h, b[p + 12:p + ln])))
        p += ln
        o = p
    return out


def texel4(img, u, v):
    x, y, w, h, d = img
    if v >= h or u >= w * 4:
        return None
    by = d[v * w * 2 + u // 2]
    return (by >> 4) if (u & 1) else (by & 15)


def seite_sichtbar(name):
    """Menge der sichtbaren Schirm-Pixel einer Textseite bei (25,30) (CLUT-Farbe != 0)."""
    t = tims(open(os.path.join(FILES, name), "rb").read())[0]
    _, flags, clut, img = t
    cw = clut[2]
    vis = set()
    bw, bh = img[2] * 4, img[3]
    for v in range(bh):
        for u in range(bw):
            i = texel4(img, u, v)
            if clut[4][i] != 0:
                vis.add((TEXT_X + u, TEXT_Y + v))
    return vis, bh


def sprite_pixel(img, clut_zeile, x, y, u0, v0, w, h):
    """Sichtbare Schirm-Pixel eines 4bpp-Sprites (CLUT-Farbe != 0 = gezeichnet)."""
    out = set()
    for dy in range(h):
        for dx in range(w):
            i = texel4(img, u0 + dx, v0 + dy)
            if i is None:
                continue
            if clut_zeile[i] != 0:
                out.add((x + dx, y + dy))
    return out


def main():
    aus = []
    st0 = tims(open(ST0, "rb").read())
    assert st0[1][0] == 0x10820, hex(st0[1][0])
    _, _, c2, img2 = st0[1]
    cw = c2[2]
    zeile = lambda k: c2[4][k * cw:(k + 1) * cw]
    re2_pfeil_clut = zeile(492 - 490)          # GetClut(256,492) @0x80076118-1c
    re2_ende_clut = zeile(490 - 490)           # GetClut(256,490) @0x800725cc-d0
    tex = tims(open(TEX, "rb").read())[0]
    _, _, tc, timg = tex
    tcw = tc[2]
    tzeile = lambda k: tc[4][k * tcw:(k + 1) * tcw][:16]
    # RE1.5 Selektor s -> TEX.TIM CLUT-Zeile 8 + s (re15_inv_screen.h, clut selector 0..7)
    re15_clut = {s: tzeile(8 + s) for s in range(8)}

    # Seiten aus dem DATEIBESTAND (Nachtrag J: englischer Satz, max_page 15; vorher deutsch 17)
    letzte = 1
    while os.path.exists(os.path.join(FILES, "FILE25_p%02d_page.TIM" % (letzte + 1))):
        letzte += 1
    seiten = ["FILE25_title_page.TIM"] + ["FILE25_p%02d_page.TIM" % p for p in range(1, letzte + 1)]
    end = len(seiten)                          # Seitenzahl = max_page + 1
    aus.append("FILE25: Titel + p01..p%02d, Seitenzahl %d, Ende-Stellung = Seite %d"
               % (letzte, end, end))
    aus.append("RE2 ST0.TIM zweites TIM @0x%x, Bild %dx%d Texel" % (st0[1][0], img2[2] * 4, img2[3]))
    for name, u0, cz in (("Pfeil links", 28, re2_pfeil_clut), ("Pfeil rechts", 42, re2_pfeil_clut)):
        px = sprite_pixel(img2, cz, 0, 0, u0, 12, 12, 13)
        aus.append("  %-12s sichtbare Texel %d, Spalten %d..%d" % (
            name, len(px), min(p[0] for p in px), max(p[0] for p in px)))
    px = sprite_pixel(img2, re2_ende_clut, 0, 0, 56, 12, 42, 14)
    aus.append("  %-12s sichtbare Texel %d, Spalten %d..%d" % (
        "Ende-Marke", len(px), min(p[0] for p in px), max(p[0] for p in px)))

    gesamt_re2 = gesamt_re15 = 0
    aus.append("")
    aus.append("Seite | Stellung | RE2: links rechts/Ende | RE1.5 (vorher): links rechts")
    for pg in range(end + 1):                  # 0..end-1 Seiten, end = Ende-Stellung
        name = seiten[min(pg, end - 1)]
        vis, H = seite_sichtbar(name)
        for b in (0, 1):
            # --- RE2 ---
            links = rechts = set()
            if pg != 0 and pg != end:          # Zustand 0 und Seite != 0 @0x800726d8-f8
                links = sprite_pixel(img2, re2_pfeil_clut, 12 - 3 * b, 110, 28, 12, 12, 13)
            if pg >= end - 1:                  # Seite == max -> Ende-Marke @0x800725c4
                rechts = sprite_pixel(img2, re2_ende_clut, 280, 110, 56, 12, 42, 14)
            else:
                rechts = sprite_pixel(img2, re2_pfeil_clut, 282 + 3 * b, 110, 42, 12, 12, 13)
            ul, ur = len(links & vis), len(rechts & vis)
            gesamt_re2 += ul + ur
            # --- RE1.5 (Negativ-Kontrolle), off = 4 * b ---
            off = 4 * b
            l15 = r15 = set()
            if pg != 0:
                l15 = sprite_pixel(timg, re15_clut[7], 0x14 - off, 0x70, 0x70, 0x38, 16, 16)
            sel = 5 if pg == end else (4 if pg == end - 1 else 7)
            r15 = sprite_pixel(timg, re15_clut[sel], 0x11c + off, 0x70, 0x70, 0x48, 16, 16)
            vl, vr = len(l15 & vis), len(r15 & vis)
            gesamt_re15 += vl + vr
            bb = sorted(l15 & vis)
            aus.append("%-5s | %d | %4d %4d | %4d %4d %s" % (
                ("Ende" if pg == end else "t" if pg == 0 else "p%02d" % pg), b, ul, ur, vl, vr,
                ("x %d-%d y %d-%d" % (min(p[0] for p in bb), max(p[0] for p in bb),
                                       min(p[1] for p in bb), max(p[1] for p in bb))) if bb else ""))
    aus.append("")
    aus.append("SUMME Pfeil-Pixel auf Textseiten-Pixeln: RE2-Lage %d, RE1.5-Lage (vorher) %d"
               % (gesamt_re2, gesamt_re15))

    # (2) Wipp-Takt: RE2 Zustand 0 (Aufnahme @0x800727b8-81c, Schwelle 0x51 @0x80072800)
    #     und Zustand 13 (FILE-Schirm @0x8006d0a0-104, Schwelle 0x33 @0x8006d0e8).
    #     Start nach jeder Ankunft: Stellung 0, Zaehler 2 (@0x80072aa0-b0 / @0x8006d290-9c).
    aus.append("")
    for name, schwelle in (("Aufnahme-Leser", 0x51), ("FILE-Schirm-Leser", 0x33)):
        c, b = 2, 0
        folge = []
        for f in range(200):
            if b:
                if c < 10:
                    b = 0
                c -= 2
            else:
                if c >= schwelle:
                    b = 1
                c += 2
            c &= 0xff
            folge.append(b)
        wechsel = [i for i in range(1, len(folge)) if folge[i] != folge[i - 1]]
        aus.append("%s (Schwelle %d): gezeichnete Stellung je Bild nach der Ankunft, "
                   "Wechsel bei Bild %s; Laeufe %s" % (
                       name, schwelle, wechsel[:6],
                       [wechsel[i + 1] - wechsel[i] for i in range(min(5, len(wechsel) - 1))]))
    txt = "\n".join(aus) + "\n"
    if len(sys.argv) > 1:
        open(sys.argv[1], "w", encoding="utf-8").write(txt)
    sys.stdout.write(txt)


main()
