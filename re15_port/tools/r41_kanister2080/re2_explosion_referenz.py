#!/usr/bin/env python3
"""Runde 41 Spur G, Nachbesserung 3 (Nachtrag 28): RE2-REFERENZBILD der Kanister-Explosion des Alligators - unabhaengig vom Port
aus den RE2-Dateien gerendert (Dossier analysis/befunde_runde41/G_kanister2080.md 10.1).

Je Effekt-Quelle der R1-Folge (EM23 R1 P0 @0x801021ec: Spritzring FUN_8010338c, Fontaene FUN_80103eb4) die Anim-Bilder des Skripts,
so wie RE2s Paketbau FUN_80077ed0 sie aus den Zellen baut (Zelle = u, v, dx, dy; Groesse = Anim-Eintrag Byte 3; Bild = Anim-Eintraege
ab step[2] bis zum Ende-Eintrag), mit der Palette des Bankkopfs + (Sub >> 3) und dem Mischmodus des Schritts (Status & 0x1000 =
halbtransparent, TPage | step[0x14] Bits 5-6 = ABR; 0 = 0,5B + 0,5F, 1 = B + F). Kern-Banken aus CORE00.ESP mit RE2 TEX.TIM (Seiten
0x1E/0x1F, CLUT-Zeile aus dem Bankkopf), Raum-Banken aus der Raum-ESP von ROOM40A0 (effect.esp = RDT 0x59CC) mit IHRER TIM aus dem
RDT (espXX.tim, Palette = Sub >> 3) - ohne die VRAM-Umlage des Ports (gator_fx_r41_pc.c), damit die Gegenprobe unabhaengig ist.

Aufruf: python re15_port/tools/r41_kanister2080/re2_explosion_referenz.py <out.png>
"""
import os, struct, sys
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
CORE = os.path.join(REPO, 're15_port', 'shared_assets', 'RE2', 'CORE00.ESP')
TEX = os.path.join(REPO, 're15_port', 'shared_assets', 'RE2', 'TEX.TIM')
RAUM = os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'room40a0')


def rd16(b, o): return struct.unpack_from('<H', b, o)[0]
def rd32(b, o): return struct.unpack_from('<I', b, o)[0]


def banken(raw):
    al = (len(raw) + 3) & ~3; ende = al - 4; out = {}
    for n in range(8):
        i = raw[n]
        if i == 0xff: break
        off = rd32(raw, ende - 4 * n); w = rd32(raw, off)
        out[i] = (off, off + ((w & 0xffff) * 2 + (w >> 16) + 2) * 4)
    return out


def tim_lesen(d):
    csz, cx, cy, cw, ch = struct.unpack_from('<IHHHH', d, 8)
    cl = [[rd16(d, 20 + (r * cw + i) * 2) for i in range(cw)] for r in range(ch)]
    o = 8 + csz
    _, ix, iy, iw, ih = struct.unpack_from('<IHHHH', d, o)
    return cl, iw, ih, d[o + 12:o + 12 + iw * 2 * ih]


def texel(pix, iw, x, y):
    b = pix[y * iw * 2 + x // 2]
    return (b >> 4) if (x & 1) else (b & 15)


def c15(c): return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


def skript(raw, bank, sub):
    off, tab = banken(raw)[bank]
    woff = rd16(raw, tab + (sub & 7) * 2); s = tab + woff * 4
    p = s + 4; nst = rd16(raw, p)
    return off, [raw[p + 4 + k * 24:p + 4 + k * 24 + 24] for k in range(nst)]


def sichtbarer_schritt(st):
    """(Anim-Start, Status) der sichtbaren Phase: Op 1/2/11/78 setzen Status := step[0x12] und Anim := step[2] (Op 1 @0x8001dc3c-4c);
    Op 4 wartet und ruft dann Op 1 auf DEMSELBEN Schritt (@0x8001df78). Ohne solchen Schritt: Spawn-Anim 0 und Status 0xA003
    (Befoerderung aufgeschobener Plaetze @0x8001d5f8-608)."""
    for s in st:
        if s[0] in (1, 2, 4, 11, 78) and (rd16(s, 0x12) & 0x2000):
            return s[2], rd16(s, 0x12), rd16(s, 0x14)
    return 0, 0xA003, 0


def anims_ab(raw, off, a0):
    """Anim-Eintraege ab a0 bis zum Ende-Eintrag {Zelle 0, Dauer 0} bzw. Schleife (Dauer 0xFF)."""
    n1 = rd16(raw, off); out = []
    a = a0
    while a < n1 and len(out) < 24:
        e = raw[off + 8 + a * 8: off + 16 + a * 8]
        if e[2] == 0 or e[2] == 0xff: break
        out.append((a, e)); a += 1
    return out


def bild(raw, off, e, quelle, zeile_clut, cx_off, abr, halb, groesse=96):
    """Ein Anim-Bild wie FUN_80077ed0 (Bildpunkt je Texel, Mitte = Ursprung) auf dunklem Grund."""
    n1 = rd16(raw, off); uvt = off + (n1 * 4 + 4) * 2
    cell, nprim, size = e[0], e[1], e[3]
    im = Image.new('RGB', (groesse, groesse), (24, 24, 30)); px = im.load()
    cl, iw, ih, pix, u_basis = quelle
    for k in range(nprim):
        u, v, dx, dy = struct.unpack_from('<BBbb', raw, uvt + (cell + k) * 4)
        for yy in range(size):
            for xx in range(size):
                X = groesse // 2 + dx + xx; Y = groesse // 2 + dy + yy
                if not (0 <= X < groesse and 0 <= Y < groesse): continue
                tu, tv = u_basis + u + xx, v + yy
                if tu >= iw * 4 or tv >= ih: continue
                c = cl[zeile_clut][cx_off + texel(pix, iw, tu, tv)]
                if c == 0: continue
                f = c15(c); g = px[X, Y]
                if halb and abr == 1: px[X, Y] = tuple(min(255, g[i] + f[i]) for i in range(3))
                elif halb and abr == 0: px[X, Y] = tuple((g[i] + f[i]) // 2 for i in range(3))
                else: px[X, Y] = f
    return im


def main(out):
    core = open(CORE, 'rb').read(); raum = open(os.path.join(RAUM, 'effect.esp'), 'rb').read()
    tcl, tiw, tih, tpix = tim_lesen(open(TEX, 'rb').read())
    reihen = []
    # Kern-Banken: TEX.TIM-Seite 0x1E (Bild-hw 128..191 -> Texel 512..) / 0x1F (Texel 768..), CLUT-Spalte x 272 = Eintrag 16
    for (bank, sub, name) in [(3, 0x0f, 'Spritzring Kern 3/0x0f (6x am Kanister)'), (0, 6, 'Fontaene Kern 0/6 (Part 7)'),
                              (0, 3, 'Fontaene Kern 0/3 (Part 7)')]:
        off, st = skript(core, bank, sub)
        clut = rd16(core, off + 4); tpage = rd16(core, off + 6)
        zeile = (clut >> 6) - 480 + (sub >> 3)
        a_start, status, tp_or = sichtbarer_schritt(st)
        abr = ((tpage | tp_or) >> 5) & 3
        halb = (status & 0x1000) != 0
        u_basis = ((tpage & 0x1f) - 0x1c) * 256       # TEX.TIM liegt ab VRAM x 768 = Seite 0x1C
        quelle = (tcl, tiw, tih, tpix, u_basis)
        bilder = [bild(core, off, e, quelle, zeile, (clut & 0x3f) * 16 - 256, abr, halb) for (a, e) in anims_ab(core, off, a_start)]
        reihen.append(('%s: CLUT-Zeile %d, ABR %d%s, Anim ab %d' % (name, 480 + zeile, abr, ' halbtransp.' if halb else '', a_start), bilder))
    # Raum-Banken: eigene TIM (espXX.tim), Palette = Sub >> 3
    for (bank, sub, name) in [(0x26, 0, 'Fontaene Raum 0x26/0 (Op-69-Kind)'), (0x26, 1, 'Raum 0x26/1'), (0x26, 2, 'Raum 0x26/2'),
                              (0x08, 2, 'Fontaene Raum 0x08/2 (Part 6)'), (0x09, 8, 'Op-41-Kind Raum 0x09/8'), (0x09, 9, 'Raum 0x09/9')]:
        off, st = skript(raum, bank, sub)
        t = tim_lesen(open(os.path.join(RAUM, 'esp%02X.tim' % bank if bank != 0x1a else 'esp1A.tim'), 'rb').read())
        a_start, status, tp_or = sichtbarer_schritt(st)
        abr = (tp_or >> 5) & 3
        halb = (status & 0x1000) != 0
        quelle = (t[0], t[1], t[2], t[3], 0)
        bilder = [bild(raum, off, e, quelle, sub >> 3, 0, abr, halb) for (a, e) in anims_ab(raum, off, a_start)]
        reihen.append(('%s: Palette %d, ABR %d%s, Anim ab %d' % (name, sub >> 3, abr, ' halbtransp.' if halb else '', a_start), bilder))
    W = 96 * 2 + 2; spalten = max(len(b) for _, b in reihen)
    sheet = Image.new('RGB', (spalten * W, len(reihen) * (W + 14)), (0, 0, 0))
    dr = ImageDraw.Draw(sheet)
    for r, (titel, bilder) in enumerate(reihen):
        y = r * (W + 14)
        dr.text((2, y), titel, fill=(255, 255, 0))
        for c, im in enumerate(bilder):
            sheet.paste(im.resize((96 * 2, 96 * 2), Image.NEAREST), (c * W, y + 14))
    sheet.save(out)
    print(out, sheet.size, ['%s: %d Bilder' % (t, len(b)) for t, b in reihen])


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else 're2_explosion_referenz.png')
