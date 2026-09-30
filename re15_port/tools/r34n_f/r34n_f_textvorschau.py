#!/usr/bin/env python3
"""r34n_f_textvorschau.py — Runde 34 Nacht, Spur F: VORSCHAU der neuen Leichen-Texte in der
Spielschrift, Seite fuer Seite, neben den Original-Texten.

Kein Ersatz fuer die Abnahme an der echten exe (Dossier 6.2) — nur die Kontrolle, dass die
Glyphenfolgen das Wort ergeben, das der Nutzer verlangt, bevor Port-Code entsteht.

Schrift wie der PC-Zeichner (platform/pc/src/render_pc.c re15_msgfont_ensure / re15_msgfont_glyph):
  DATA/TEX.TIM, 4bpp, Schriftseite x in [256,512), Zelle (code&15)*16 / (code>>4)*16+32,
  Palette = CLUT-Zeile ((attr&3)*2 + (attr>>2)), Index 0 transparent;
  Vorschub = DEBUG.BIN[0x4416+code] (FUN_80028868), Zeilenhoehe 16, Kasten-Ursprung (34,180).
Steuercodes wie re15_msg_layout (msg_common.c): 05 N Farbe, 08 Zeilenumbruch, 02 N Seite, 01 Ende.

Aufruf: C:/Python310/python.exe re15_port/tools/r34n_f/r34n_f_textvorschau.py <ausgabe.png>
"""
import os, struct, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")


def lade_tim(path):
    d = open(path, "rb").read()
    magic, flags = struct.unpack_from("<II", d, 0)
    assert magic == 0x10, "kein TIM"
    bpp = flags & 7
    assert bpp == 0, "TEX.TIM muss 4bpp sein"
    o = 8
    assert flags & 8, "TEX.TIM ohne CLUT"
    bnum, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, o)
    clut = list(struct.unpack_from("<%dH" % (cw * ch), d, o + 12))
    o += bnum
    bnum, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, o)
    px = d[o + 12:o + bnum]
    breite = iw * 4
    return clut, cw, px, breite, ih


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else "leichen_textvorschau.png"
    clut, cw, px, breite, hoehe = lade_tim(os.path.join(PSX, "DATA", "TEX.TIM"))
    dbg = open(os.path.join(PSX, "BIN", "DEBUG.BIN"), "rb").read()
    wid = [dbg[0x4416 + c] for c in range(256)]
    pitch = breite // 2

    def idx(ax, ay):
        vx = 256 + ax
        b = px[ay * pitch + (vx >> 1)]
        return (b >> 4) if (vx & 1) else (b & 15)

    stride = 32 if cw >= 32 else 16

    def farbe(attr, e):
        row = (attr & 3) * 2 + ((attr >> 2) & 1)
        c = clut[row * stride + e]
        return (((c >> 0) & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)

    def seiten(raw):
        """[(Seite, bytes)] getrennt an 02 N; Kopf 04 xx wird uebersprungen."""
        i = 2 if raw[:1] == b"\x04" else 0
        cur, out = bytearray(), []
        while i < len(raw):
            b = raw[i]
            if b == 0x01:
                break
            if b == 0x02:
                out.append(bytes(cur)); cur = bytearray(); i += 2; continue
            if b in (0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
                cur += raw[i:i + 2]; i += 2; continue
            cur.append(b); i += 1
        out.append(bytes(cur))
        return out

    def zeichne(img, x0, y0, raw):
        penx, peny, attr = x0, y0, 0
        i = 0
        while i < len(raw):
            b = raw[i]
            if b == 0x05:
                attr = raw[i + 1] & 7; i += 2; continue
            if b in (0x04, 0x06, 0x09, 0x0A, 0x0B):
                i += 2; continue
            if b == 0x08:
                penx = x0; peny += 16; i += 1; continue
            if b == 0x00:
                penx += wid[0]; i += 1; continue
            col, row = (b & 15) * 16, (b >> 4) * 16 + 32
            for gy in range(16):
                for gx in range(16):
                    e = idx(col + gx, row + gy)
                    if e:
                        X, Y = penx + gx, peny + gy
                        if 0 <= X < img.width and 0 <= Y < img.height:
                            img.putpixel((X, Y), farbe(attr, e))
            penx += wid[b]; i += 1

    sys.path.insert(0, HERE)
    import r34n_f_zensus as Z
    kopf, ende, seite = b"\x04\x02", b"\x01\x00", b"\x02\x00"
    N = Z.NEU
    neu = [
        ("ROOM1110 NEU lang (Port-Nachricht 20)", kopf + N["police_dead"] + seite + N["he_is_holding"] + N[" something"] + b"\x57" + ende),
        ("ROOM1110 NEU kurz (Port-Nachricht 21)", kopf + N["police_dead"] + ende),
        ("ROOM1230 NEU lang (Port-Nachricht 22)", kopf + N["miserable"] + seite + N["he_is_holding"] + N[" something"] + b"\x57" + ende),
        ("ROOM1230 NEU kurz (Port-Nachricht 23)", kopf + N["miserable"] + ende),
    ]
    orig = []
    for room, mid in (("1110", 0), ("1230", 10)):
        d = Z.rdt(room)
        for i, st, en in Z.msg_block(d):
            if i == mid:
                orig.append(("ROOM%s ORIGINAL msg %d" % (room, mid), Z.msg_bytes(d, st, en)))
    alle = orig[:1] + neu[:2] + orig[1:] + neu[2:]
    kaestchen = []
    for titel, raw in alle:
        for k, pg in enumerate(seiten(raw)):
            kaestchen.append(("%s - Seite %d" % (titel, k + 1), pg))
    W, H = 320, 44
    img = Image.new("RGB", (W * 2, H * ((len(kaestchen) + 1) // 2)), (16, 16, 24))
    from PIL import ImageDraw
    dr = ImageDraw.Draw(img)
    for n, (titel, pg) in enumerate(kaestchen):
        x0, y0 = (n % 2) * W, (n // 2) * H
        dr.rectangle([x0, y0, x0 + W - 1, y0 + H - 1], outline=(60, 60, 80))
        dr.text((x0 + 3, y0 + 1), titel, fill=(150, 150, 170))
        zeichne(img, x0 + 8, y0 + 13, pg)
    img = img.resize((img.width * 2, img.height * 2), Image.NEAREST)
    img.save(out)
    print("Vorschau: %s (%d Seiten)" % (out, len(kaestchen)))


if __name__ == "__main__":
    main()
