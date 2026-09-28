#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_diary_en_framedump.py - Abnahme des Lesers AM FRAMEDUMP des echten Spiels (Nachtrag J).

Liest die Framedumps von r30_diary_en_lauf.sh (320x240, RE15_FRAMEDUMP = Readback vor
SDL_RenderPresent, beschleunigter Renderer) und vergleicht sie mit dem Schirmbild, das sich aus
den nach shared_assets KOPIERTEN TIM-Dateien ergibt:
    Grund schwarz                          RE2 FUN_8002bda8(2,0) @0x80071d8c-94
    Illustration bei (100,60), 128 x (256-H) ab v = H        @0x80072584-94, @0x800760b8-dc
    Textseite bei (25,30)                                     @0x80076170-84
    Texel mit CLUT-Farbe 0x0000 durchsichtig (psx-spx)
Geprueft je Abzug:
  (a) ZUORDNUNG: gegen jede Kandidaten-Seite (englisch: Titel, p01..pN; deutsch: die alten
      p01..p17 aus dem Commit vor dem Austausch) der Anteil gleicher sichtbarer Textseiten-Pixel
      (5 Bit je Kanal) - die erwartete Seite muss die beste sein, die beste DEUTSCHE deutlich
      schlechter;
  (b) GLEICHHEIT: sichtbare Pixel der erwarteten Textseite, die im Abzug gleich sind;
  (c) GRUND: nicht-schwarze Pixel ausserhalb Textseite und Illustration, mit Huellrechteck
      (erwartet: nur Pfeile/Ende-Marke bei y 110..123 und die Fusszeile).
Aufruf: python analysis/befunde_runde30/r30_diary_en_framedump.py <laufverzeichnis> [alter-commit]
"""
import os, struct, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")
LAUF = sys.argv[1]
ALT = sys.argv[2] if len(sys.argv) > 2 else "29391b2b"     # letzter Stand mit deutschem Satz


def tim_bytes(b):
    magic, flags = struct.unpack_from("<II", b, 0)
    assert magic == 0x10
    o = 8
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o)
    clut = struct.unpack_from("<%dH" % (cw * ch), b, o + 12)
    o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, o)
    d = b[o + 12:o + ilen]
    if (flags & 7) == 0:
        W = iw * 4
        px = [[(d[y * iw * 2 + x // 2] >> 4) if (x & 1) else (d[y * iw * 2 + x // 2] & 15)
               for x in range(W)] for y in range(ih)]
    else:
        W = iw * 2
        px = [[d[y * W + x] for x in range(W)] for y in range(ih)]
    return W, ih, clut, px


def rgb5(c):
    return (c & 31, (c >> 5) & 31, (c >> 10) & 31)


def seite_pixel(b):
    """{(x,y) am Schirm: (r5,g5,b5)} der sichtbaren Texel einer Textseite bei (25,30)."""
    W, H, clut, px = tim_bytes(b)
    out = {}
    for y in range(H):
        for x in range(W):
            c = clut[px[y][x]]
            if c != 0 and 0 <= 25 + x < 320 and 0 <= 30 + y < 240:
                out[(25 + x, 30 + y)] = rgb5(c)
    return out, H


def ppm(p):
    b = open(p, "rb").read()
    felder, i = [], 0
    while len(felder) < 4:                      # "P6" w h maxval, je durch Leerraum getrennt
        while b[i:i + 1].isspace():
            i += 1
        j = i
        while not b[j:j + 1].isspace():
            j += 1
        felder.append(b[i:j])
        i = j
    i += 1                                      # GENAU ein Leerraum vor den Bilddaten
    assert felder[0] == b"P6" and felder[3] == b"255"
    w, h = int(felder[1]), int(felder[2])
    d = b[i:i + w * h * 3]
    assert len(d) == w * h * 3
    return w, h, d


def main():
    kand = {}
    kand["t"] = seite_pixel(open(os.path.join(FILES, "FILE25_title_page.TIM"), "rb").read())
    n = 1
    while os.path.exists(os.path.join(FILES, "FILE25_p%02d_page.TIM" % n)):
        kand["p%02d" % n] = seite_pixel(open(os.path.join(FILES, "FILE25_p%02d_page.TIM" % n), "rb").read())
        n += 1
    N = n - 1
    for k in range(1, 30):
        try:
            b = subprocess.check_output(["git", "-C", REPO, "show",
                                         "%s:re15_port/shared_assets/RE2/FILES/FILE25_p%02d_page.TIM" % (ALT, k)],
                                        stderr=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
            break
        kand["de_p%02d" % k] = seite_pixel(b)
    W, H, pclut, ppx = tim_bytes(open(os.path.join(FILES, "FILE25_title_paper.TIM"), "rb").read())
    H_seite = kand["t"][1]
    papier = {}
    for v in range(H - H_seite):
        for u in range(W):
            c = pclut[ppx[H_seite + v][u]]
            if c != 0:
                papier[(100 + u, 60 + v)] = rgb5(c)

    # Zeitplan von r30_diary_en_lauf.sh: F310+90k = Seite k (0 = Titel), k = N+1 = Ende-Stellung
    plan = [(310 + 90 * k, "t" if k == 0 else "p%02d" % min(k, N),
             "Titel" if k == 0 else ("Ende-Stellung" if k == N + 1 else "Seite %d" % k))
            for k in range(N + 2)]
    plan.append((2200, "t", "Leser aus der FILE-Liste, Titel"))
    aus = []
    aus.append("Framedump-Abnahme %s  (englisch: Titel + p01..p%02d; deutsch aus %s: %d Seiten)"
               % (os.path.relpath(LAUF, REPO), N, ALT, len([k for k in kand if k.startswith("de_")])))
    alles_ok = True
    for f, soll, was in plan:
        pf = os.path.join(LAUF, "mess", "f%06d.ppm" % f)
        if not os.path.exists(pf):
            aus.append("F%d %-32s FEHLT" % (f, was))
            alles_ok = False
            continue
        w, h, d = ppm(pf)
        assert (w, h) == (320, 240), (w, h)

        def pix(x, y):
            i = (y * w + x) * 3
            return (d[i] >> 3, d[i + 1] >> 3, d[i + 2] >> 3)
        wertung = {}
        for name, (sp, _) in kand.items():
            gleich = sum(1 for (x, y), c in sp.items() if pix(x, y) == c)
            wertung[name] = (gleich, len(sp))
        best = max(wertung, key=lambda k: wertung[k][0] / max(1, wertung[k][1]))
        de = [k for k in wertung if k.startswith("de_")]
        best_de = max(de, key=lambda k: wertung[k][0] / max(1, wertung[k][1])) if de else None
        g, t = wertung[soll]
        # Grund: nicht-schwarz ausserhalb Textseite + Illustration
        sp = kand[soll][0]
        rest = [(x, y) for y in range(h) for x in range(w)
                if (x, y) not in sp and (x, y) not in papier and pix(x, y) != (0, 0, 0)]
        box = ""
        if rest:
            # Huellrechtecke je Band (Pfeile y ~110..123, Fusszeile darunter)
            baender = {}
            for x, y in rest:
                key = "y<150" if y < 150 else "y>=150"
                bx = baender.setdefault(key, [x, x, y, y, 0])
                bx[0] = min(bx[0], x); bx[1] = max(bx[1], x)
                bx[2] = min(bx[2], y); bx[3] = max(bx[3], y); bx[4] += 1
            box = "; ".join("%s: %d px x %d..%d y %d..%d" % (k, v[4], v[0], v[1], v[2], v[3])
                            for k, v in sorted(baender.items()))
        ok = (best == soll) and g == t
        alles_ok &= ok
        aus.append("F%d %-32s Soll %-4s beste %-4s  gleich %d/%d  (beste deutsche %s %d/%d)  %s"
                   % (f, was, soll, best, g, t, best_de,
                      wertung[best_de][0] if best_de else 0, wertung[best_de][1] if best_de else 0,
                      "OK" if ok else "ABWEICHUNG"))
        aus.append("        Grund ausserhalb Textseite/Illustration nicht schwarz: %d  %s"
                   % (len(rest), box))
    aus.append("")
    aus.append("ERGEBNIS: %s" % ("ALLE ABZUEGE ZEIGEN DIE ERWARTETE ENGLISCHE SEITE PIXELGLEICH"
                                 if alles_ok else "ABWEICHUNGEN - siehe oben"))
    txt = "\n".join(aus) + "\n"
    open(os.path.join(LAUF, "framedump_abnahme.txt"), "w", encoding="utf-8").write(txt)
    sys.stdout.write(txt)
    return 0 if alles_ok else 1


sys.exit(main())
