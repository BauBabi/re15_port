#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_diary_satz_pruefung.py - Abnahme des Prototyps AM ARTEFAKT (Runde 30, Thema E1).

Prueft NICHT das Protokoll des Satzwerkzeugs, sondern die geschriebenen TIM-Dateien:

  1. Nutzertext: der Codeblock aus AUFTRAG.md Abschnitt E gegen nutzertext.txt
     (zeichengenau, nur Zeilenenden vereinheitlicht).
  2. Ruecklesen: jede FILE25_pNN_page.TIM wird Zeile fuer Zeile per Glyphenvergleich
     (Kern-Indizes 1..6) wieder in Text verwandelt.
  3. Wortfolge: die zurueckgelesenen Woerter aller Seiten gegen die Wortfolge des
     Nutzertexts - jede Abweichung wird gemeldet.
  4. Seitenregel: jede Seite, deren erste Zeile ein Datum ist, und jedes Datum, das
     NICHT in einer ersten Zeile steht.
  5. Format: Dateigroesse, Kopf, CLUT und Bildkopf jeder Seite gegen FILE08_p01_page.TIM.

Aufruf: python analysis/befunde_runde30/r30_diary_satz_pruefung.py [ausgabeverzeichnis]
"""
import json, os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "build", "r30_irons-diary-dokument")
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")
DOC = 25
ZEILE = 16


def tim(pfad):
    b = open(pfad, "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    assert magic == 0x10, pfad
    o = 8
    clen = struct.unpack_from("<I", b, o)[0]
    o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, o)
    d = b[o + 12:o + ilen]
    assert (flags & 7) == 0, "kein 4bpp: " + pfad
    W = iw * 4
    px = []
    for y in range(ih):
        row = []
        for x in range(W):
            v = d[y * iw * 2 + x // 2]
            row.append((v >> 4) if (x & 1) else (v & 15))
        px.append(row)
    return {"roh": b, "W": W, "H": ih, "px": px, "kopf": b[:o + 12], "vram": (ix, iy)}


def kern(v):
    return 1 <= v <= 6


def ruecklesen(font, px, y0):
    """Eine Rasterzeile -> Text. Liefert (text, reste) ; reste = Kernpixel ohne Glyphe."""
    W = len(px[0])
    band = [[px[y0 + r][x] if kern(px[y0 + r][x]) else 0 for x in range(W)] for r in range(ZEILE)]
    gl = []
    for ch, g in font["glyphen"].items():
        zellen = [(r, s, int(c, 16)) for r, row in enumerate(g["bitmap"])
                  for s, c in enumerate(row) if int(c, 16)]
        if zellen:
            c0 = min(s for _, s, _ in zellen)
            gl.append((ch, g, zellen, c0))
    gl.sort(key=lambda t: -len(t[2]))
    text = ""
    pen_soll = None
    vor = None
    while True:
        xc = None
        for x in range(W):
            if any(band[r][x] for r in range(ZEILE)):
                xc = x
                break
        if xc is None:
            break
        treffer = None
        for ch, g, zellen, c0 in gl:
            xl = xc - c0
            if all(0 <= xl + s < W and band[r][xl + s] == v for r, s, v in zellen):
                # die Glyphe muss die linke Spalte xc auch wirklich VOLLSTAENDIG erklaeren
                spalte = set(r for r in range(ZEILE) if band[r][xc])
                if spalte <= set(r for r, s, _ in zellen if xl + s == xc):
                    treffer = (ch, g, zellen, xl)
                    break
        if treffer is None:
            return text, sum(1 for r in range(ZEILE) for x in range(W) if band[r][x])
        ch, g, zellen, xl = treffer
        pen = xl - g["links"]
        if pen_soll is not None:
            soll = pen_soll
            if vor == ch and ch in font["folge"]:
                soll += font["folge"][ch] - g["vorschub"]
            d = pen - soll
            if d >= font["leer"]:
                text += " " * (d // font["leer"])
                if d % font["leer"]:
                    text += "<+%d>" % (d % font["leer"])
            elif d != 0:
                text += "<%+d>" % d
        text += ch
        for r, s, _ in zellen:
            band[r][xl + s] = 0
        pen_soll = pen + g["vorschub"]
        vor = ch
    return text, 0


def main():
    aus = []
    ok = True

    def p(s=""):
        aus.append(s)

    # ---- 1. Nutzertext
    auftrag = open(os.path.join(HERE, "AUFTRAG.md"), encoding="utf-8").read().replace("\r", "")
    e = auftrag.index("## E ")
    a = auftrag.index("```\n", e) + 4
    z = auftrag.index("\n```", a)
    soll = auftrag[a:z]
    ist = open(os.path.join(OUT, "nutzertext.txt"), encoding="utf-8").read().replace("\r", "")
    p("[1] Nutzertext AUFTRAG.md Abschnitt E (%d Zeichen) gegen nutzertext.txt (%d Zeichen)"
      % (len(soll), len(ist.rstrip("\n"))))
    if soll.strip("\n") == ist.strip("\n"):
        p("    ZEICHENGLEICH")
    else:
        ok = False
        for i, (x, y) in enumerate(zip(soll, ist)):
            if x != y:
                p("    ABWEICHUNG ab Zeichen %d: %r gegen %r" % (i, soll[i:i + 40], ist[i:i + 40]))
                break

    # ---- 2. Ruecklesen
    font = json.load(open(os.path.join(OUT, "re2_doc_font.json"), encoding="utf-8"))
    vorlage = tim(os.path.join(FILES, "FILE08_p01_page.TIM"))
    seiten = []
    n = 1
    while os.path.exists(os.path.join(OUT, "FILE%02d_p%02d_page.TIM" % (DOC, n))):
        seiten.append(n)
        n += 1
    p()
    p("[2] Ruecklesen von %d Textseiten (FILE%02d_p01..p%02d)" % (len(seiten), DOC, seiten[-1]))
    woerter = []
    zeilen_ges = 0
    reste_ges = 0
    datum = re.compile(r"^\d{1,2}\. [A-Z][a-z]+( \d{4})?:?$")
    regel = []
    fmt_ab = 0
    for n in seiten:
        pf = os.path.join(OUT, "FILE%02d_p%02d_page.TIM" % (DOC, n))
        t = tim(pf)
        if len(t["roh"]) != len(vorlage["roh"]) or t["kopf"] != vorlage["kopf"]:
            fmt_ab += 1
        zl = []
        for k in range(t["H"] // ZEILE):
            s, rest = ruecklesen(font, t["px"], k * ZEILE)
            reste_ges += rest
            zl.append(s)
        while zl and zl[-1] == "":
            zl.pop()
        zeilen_ges += len(zl)
        p("    p%02d  %d Zeilen" % (n, len(zl)))
        for k, s in enumerate(zl):
            p("        %d | %s" % (k, s))
            if datum.match(s) and k != 0:
                regel.append("p%02d Zeile %d: Datum %r steht NICHT am Seitenkopf" % (n, k, s))
            woerter += s.split()
        if zl and datum.match(zl[0]):
            regel.append("p%02d beginnt mit Datum %r" % (n, zl[0]))
    p("    Kernpixel ohne Glyphe: %d   Marken <+n>/<-n> (Abstand ausserhalb der Metrik): %d"
      % (reste_ges, sum(1 for w in woerter if "<" in w)))
    if reste_ges:
        ok = False

    # ---- 3. Wortfolge
    soll_w = soll.split()
    p()
    p("[3] Wortfolge: Nutzertext %d Woerter, zurueckgelesen %d Woerter" % (len(soll_w), len(woerter)))
    ab = [(i, x, y) for i, (x, y) in enumerate(zip(soll_w, woerter)) if x != y]
    if len(soll_w) == len(woerter) and not ab:
        p("    WORTGLEICH (jedes Wort, jede Gross-/Kleinschreibung, jedes Satzzeichen)")
    else:
        ok = False
        for i, x, y in ab[:20]:
            p("    ABWEICHUNG Wort %d: Soll %r  Ist %r" % (i, x, y))

    # ---- 4. Seitenregel
    p()
    p("[4] Seitenregel 'je Datum eine neue Seite'")
    for r in regel:
        p("    " + r)
        if "NICHT" in r:
            ok = False
    kopf = [r for r in regel if "beginnt" in r]
    soll_daten = [ln.strip() for ln in soll.split("\n") if datum.match(ln.strip())]
    p("    Daten im Nutzertext: %d   Seiten mit Datum am Kopf: %d" % (len(soll_daten), len(kopf)))
    if len(soll_daten) != len(kopf):
        ok = False

    # ---- 5. Format
    p()
    p("[5] Format gegen FILE08_p01_page.TIM (%d B, Kopf+CLUT+Bildkopf %d B, VRAM-Lage %s)"
      % (len(vorlage["roh"]), len(vorlage["kopf"]), vorlage["vram"]))
    p("    Textseiten mit abweichender Groesse oder abweichendem Kopf: %d von %d" % (fmt_ab, len(seiten)))
    if fmt_ab:
        ok = False
    for name, vor in (("title_page", "FILE08_title_page.TIM"), ("p00_page", "FILE08_p00_page.TIM"),
                      ("title_paper", "FILE08_title_paper.TIM")):
        a = open(os.path.join(OUT, "FILE%02d_%s.TIM" % (DOC, name)), "rb").read()
        b = open(os.path.join(FILES, vor), "rb").read()
        if name == "title_paper":
            gl = (a == b)
            p("    FILE%02d_%s.TIM %d B gegen %s: %s" % (DOC, name, len(a), vor,
                                                          "BYTE-GLEICH" if gl else "ABWEICHEND"))
        else:
            gl = (len(a) == len(b) and a[:64] == b[:64])
            p("    FILE%02d_%s.TIM %d B gegen %s %d B: Kopf+CLUT+Bildkopf (64 B) %s"
              % (DOC, name, len(a), vor, len(b), "BYTE-GLEICH" if gl else "ABWEICHEND"))
        if not gl:
            ok = False
    a = open(os.path.join(OUT, "FILE%02d_title_page.TIM" % DOC), "rb").read()
    b = open(os.path.join(OUT, "FILE%02d_p00_page.TIM" % DOC), "rb").read()
    p("    title_page gegen p00: %s" % ("BYTE-GLEICH" if a == b else "ABWEICHEND"))
    t = tim(os.path.join(OUT, "FILE%02d_title_page.TIM" % DOC))
    tz = []
    for k in range(t["H"] // ZEILE):
        s, rest = ruecklesen(font, t["px"], k * ZEILE)
        if s:
            tz.append((k, s))
    p("    Titelseite zurueckgelesen: %r" % tz)

    p()
    p("ERGEBNIS: %s" % ("ALLE PRUEFUNGEN BESTANDEN" if ok else "ABWEICHUNGEN - siehe oben"))
    txt = "\n".join(aus) + "\n"
    open(os.path.join(OUT, "satz_pruefung.txt"), "w", encoding="utf-8").write(txt)
    sys.stdout.buffer.write(txt.encode("utf-8"))


main()
