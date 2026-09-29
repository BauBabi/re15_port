#!/usr/bin/env python
"""re2fx_katalog.py - Runde 34 Spur D, O9: der RE2-Sprite-Katalog der Aufschlag-Kinder und der Abgleich
mit dem Offscreen-Bild der Port-Maschine (tests/unit/probe_r34_re2fx_bild.c).

UNABHAENGIGER Dekoder (keine Zeile aus re2_fx.c / re2fx_pc.c): liest RE2 CORE00.ESP und TEX.TIM direkt.
  CORE00.ESP  8 Id-Bytes, Bank-Offsets rueckwaerts ab dem letzten Wort (FUN_8001bca0 @0x8001bcc8-2c),
              Bank = {u16 n1, u16 n2, u16 clut, u16 tpage, n1 x 8 B Anim {cell, nprim, dauer, size},
              n2 x 4 B UV {u, v, cx s8, cy s8}} (FUN_80077ed0 @0x80077f2c-80078150).
  TEX.TIM     4 bpp; der Lader FUN_80076a40 legt das Bild nach VRAM (768,256) (@0x80076a64-a8) und die
              CLUT nach (256, 480) (@0x80076b00-0c). Seite TPage 0x1E = VRAM-x 896 = Bild-hw 128..191,
              0x1F = x 960 = hw 192..255; CLUT-Wort x-Feld 0x11 = Spalte 272 = CLUT-Eintraege 16..31.

Aufruf:
  python re2fx_katalog.py [--aus DIR]                  Katalog-Blaetter der Aufschlag-Kinder -> DIR
  python re2fx_katalog.py --vergleich BILDDIR [--aus]  zusaetzlich: probe-Ausschnitte (re2fx_crops.txt)
                                                        texelgenau gegen den Katalog-Dekoder, PPM -> PNG
Rueckgabe 0 = Abgleich ohne Abweichung.
"""
import os
import struct
import sys

try:
    from PIL import Image
except ImportError:  # pragma: no cover
    Image = None

HERE = os.path.dirname(os.path.abspath(__file__))
RE2 = os.path.join(HERE, '..', 'shared_assets', 'RE2')

# Aufschlag-Kinder (RE2 PSX.EXE): Op 49 @0x80021780-0x80021924, Op 48 @0x80021094-0x800210c4,
# Bodenflamme 0x0505 @0x80021114, Folgeflamme 0x0504 @0x8001fdf0; Skripte/Start-Anim aus CORE00.ESP.
KINDER = [
    (0x030F2000, 'Saeure Bild 0 / Phase 4'),
    (0x040C2000, 'Saeure Bild 0'),
    (0x041D1800, 'Saeure Bild 0'),
    (0x031F2000, 'Saeure Phase 1'),
    (0x03142000, 'Saeure Phase 2'),
    (0x040D2800, 'Saeure Phase 3'),
    (0x040C2800, 'Brand Bild 0'),
    (0x041D2700, 'Brand Bild 0'),
    (0x05051C00, 'Bodenflamme (Anim rng%3 .. Schleife)'),
    (0x05041600, 'Folgeflamme (Anim 0 + rng%5)'),
]


def lade():
    esp = open(os.path.join(RE2, 'CORE00.ESP'), 'rb').read()
    tim = open(os.path.join(RE2, 'TEX.TIM'), 'rb').read()
    return esp, tim


def baenke(esp):
    ende = ((len(esp) + 3) & ~3) - 4
    out = {}
    for i, b in enumerate(esp[:8]):
        if b == 0xFF:
            break
        out[b] = struct.unpack_from('<i', esp, ende - 4 * i)[0]
    return out


class Tex:
    def __init__(self, tim):
        csz = struct.unpack_from('<I', tim, 8)[0]
        cw, ch = struct.unpack_from('<HH', tim, 16)
        self.clut = [struct.unpack_from('<H', tim, 20 + 2 * k)[0] for k in range(cw * ch)]
        self.cw = cw
        im = 8 + csz
        self.iw, self.ih = struct.unpack_from('<HH', tim, im + 8)
        self.pix = tim[im + 12: im + 12 + self.iw * self.ih * 2]

    def texel(self, tpage, clut, u, v):
        seite = tpage & 0x1F
        col = (seite & 0xF) * 64 - 768                    # Bild-hw-Spalte der Seite (Bild bei VRAM-x 768)
        hw = struct.unpack_from('<H', self.pix, ((v % 256) * self.iw + col + u // 4) * 2)[0]
        idx = (hw >> ((u & 3) * 4)) & 0xF
        zeile = ((clut >> 6) & 0x1FF) - 480               # CLUT-y 480 + Zeile
        spalte = (clut & 0x3F) * 16 - 256                 # CLUT-x 256 + Spalte
        return self.clut[zeile * self.cw + spalte + idx]


def rgb(c):
    return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


def skript_start(esp, bank_off, sub):
    """Start-Anim und TPage-OR des Skripts (Step 0) — fuer Op 1 = step[2]."""
    n1, n2, clut, tp = struct.unpack_from('<HHHH', esp, bank_off)
    tab = bank_off + 8 + 8 * n1 + 4 * n2
    woff = struct.unpack_from('<H', esp, tab + 2 * (sub & 7))[0]
    st = tab + 4 * woff + 8                               # nparts, pad, nsteps, pad, dann Step 0
    step = esp[st: st + 24]
    return step


def katalog(esp, tex, aus):
    bk = baenke(esp)
    zeilen = []
    for code, name in KINDER:
        bank, sub, skala = code >> 24, (code >> 16) & 0xFF, code & 0xFFFF
        off = bk[bank]
        n1, n2, clut0, tp0 = struct.unpack_from('<HHHH', esp, off)
        step = skript_start(esp, off, sub)
        opa, opb, anim0 = step[0], step[1], step[2]
        tpor = struct.unpack_from('<H', step, 0x14)[0]
        if opb == 27:                                     # Bodenflamme: Op 27 setzt TPage |= 0x20, Anim rng%3
            tpor, anim0 = 0x20, 0
        clut = clut0 + ((sub >> 3) << 6)
        tpage = tp0 | tpor
        # Anim-Folge bis ENDE (dauer 0 & cell 0) bzw. eine Schleifenrunde
        folge, k, gesehen = [], anim0, set()
        while k < n1 and k not in gesehen and len(folge) < 40:
            gesehen.add(k)
            cell, nprim, dauer, size = esp[off + 8 + 8 * k: off + 12 + 8 * k]
            if dauer == 0 and cell == 0:
                break
            if dauer == 0xFF:
                k = cell
                continue
            folge.append((k, cell, nprim, size))
            k += 1
        zeilen.append((code, name, clut, tpage, folge, off, n1))
    if Image is None:
        print('PIL fehlt - kein Bild')
        return zeilen
    zelle = 72
    breite = max(len(z[4]) for z in zeilen) * zelle
    bild = Image.new('RGB', (breite + 8, len(zeilen) * zelle + 8), (64, 64, 64))
    px = bild.load()
    for r, (code, name, clut, tpage, folge, off, n1) in enumerate(zeilen):
        abr = (tpage >> 5) & 3
        for c, (k, cell, nprim, size) in enumerate(folge):
            ox, oy = 4 + c * zelle + zelle // 2, 4 + r * zelle + zelle // 2
            uvo = off + 8 + 8 * n1
            for p in range(nprim):
                u, v, cx, cy = struct.unpack_from('<BBbb', esp, uvo + 4 * (cell + p))
                for dy in range(size):
                    for dx in range(size):
                        t = tex.texel(tpage, clut, u + dx, v + dy)
                        if t == 0:
                            continue
                        x, y = ox + cx + dx, oy + cy + dy
                        if not (0 <= x < bild.width and 0 <= y < bild.height):
                            continue
                        b = px[x, y]
                        f = rgb(t)
                        if t & 0x8000:
                            if abr == 0:
                                n = tuple((bb + ff) // 2 for bb, ff in zip(b, f))
                            elif abr == 1:
                                n = tuple(min(255, bb + ff) for bb, ff in zip(b, f))
                            elif abr == 2:
                                n = tuple(max(0, bb - ff) for bb, ff in zip(b, f))
                            else:
                                n = tuple(min(255, bb + ff // 4) for bb, ff in zip(b, f))
                        else:
                            n = f
                        px[x, y] = n
    os.makedirs(aus, exist_ok=True)
    pfad = os.path.join(aus, 're2fx_katalog.png')
    bild.resize((bild.width * 2, bild.height * 2), Image.NEAREST).save(pfad)
    print('Katalog:', pfad)
    for code, name, clut, tpage, folge, off, n1 in zeilen:
        print('  0x%08X %-36s CLUT 0x%04X TPage 0x%04X ABR %d Anim %s' % (
            code, name, clut, tpage, (tpage >> 5) & 3, [f[0] for f in folge]))
    return zeilen


def vergleich(tex, bilddir, aus):
    fehler = 0
    n = 0
    cur = None
    zeilen = []
    for line in open(os.path.join(bilddir, 're2fx_crops.txt')):
        if line.startswith('CROP'):
            if cur:
                fehler += pruef(tex, cur, zeilen)
                n += 1
            f = line.split()
            cur = (int(f[1], 16), int(f[2], 16), int(f[3]), int(f[4]), int(f[5]), int(f[6]))
            zeilen = []
        else:
            zeilen.append([int(w, 16) for w in line.split()])
    if cur:
        fehler += pruef(tex, cur, zeilen)
        n += 1
    print('Ausschnitte verglichen: %d, abweichende Texel: %d' % (n, fehler))
    if Image is not None:
        for name in sorted(os.listdir(bilddir)):
            if name.endswith('.ppm'):
                Image.open(os.path.join(bilddir, name)).save(os.path.join(aus, name[:-4] + '.png'))
    return fehler


def pruef(tex, crop, zeilen):
    tp, cl, u0, v0, u1, v1 = crop
    f = 0
    for j, v in enumerate(range(v0, v1)):
        for i, u in enumerate(range(u0, u1)):
            if zeilen[j][i] != tex.texel(tp, cl, u, v):
                f += 1
    return f


def main():
    aus = os.path.join(HERE, '..', '..', 'build', 'r34g_d', 'katalog')
    if '--aus' in sys.argv:
        aus = sys.argv[sys.argv.index('--aus') + 1]
    esp, tex_raw = lade()
    tex = Tex(tex_raw)
    katalog(esp, tex, aus)
    if '--vergleich' in sys.argv:
        f = vergleich(tex, sys.argv[sys.argv.index('--vergleich') + 1], aus)
        return 1 if f else 0
    return 0


if __name__ == '__main__':
    sys.exit(main())
