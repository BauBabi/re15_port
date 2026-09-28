#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""r30_diary_satz_pruefung.py - Abnahme des Satzes AM ARTEFAKT (Runde 30, E1 / Nachtrag J).

Prueft NICHT das Protokoll des Satzwerkzeugs, sondern die geschriebenen TIM-Dateien:

  1. Nutzertext: analysis/befunde_runde30/irons_diary_en.txt (die woertliche Uebersetzung des
     Nutzers, AUFTRAG.md Abschnitt J) gegen die Eingabe, die das Satzwerkzeug gelesen hat
     (<satz>/satz_eingabe.txt; zeichengenau, nur Zeilenenden vereinheitlicht).
  2. Ruecklesen: jede FILE25_pNN_page.TIM wird Zeile fuer Zeile per Glyphenvergleich
     (Kern-Indizes 1..6) wieder in Text verwandelt; Kernpixel ohne Glyphe werden gezaehlt.
  3. Wortfolge: die zurueckgelesenen Woerter aller Seiten gegen die Wortfolge des
     Nutzertexts - jede Abweichung wird gemeldet.
  4. Seitenregel: jede Seite, deren erste Zeile ein Datum ist, und jedes Datum, das
     NICHT in einer ersten Zeile steht (deutsche und englische Datumsschreibung).
  5. Format: Dateigroesse, Kopf, CLUT und Bildkopf jeder Seite gegen FILE08_p01_page.TIM;
     Titel/p00/Illustration gegen FILE08; Dateibestand: p00..pN lueckenlos, KEIN p(N+1).
  6. Konstruktionen: kein zurueckgelesenes Zeichen (Textseiten UND Titelseite) darf eine
     KONSTRUIERTE Glyphe sein (Umlaute, Eszett der deutschen Fassung).
  7. Nur wenn --tim und --satz verschieden sind: jede Datei im --tim-Verzeichnis byte-gleich
     der Satz-Ausgabe (md5).

Aufruf:
  python analysis/befunde_runde30/r30_diary_satz_pruefung.py [--satz DIR] [--tim DIR] [--soll TXT]
    --satz  Ausgabe des Satzwerkzeugs (re2_doc_font.json, satz_eingabe.txt, FILE25_*); Default
            build/r30_n_diary_en
    --tim   Verzeichnis, dessen FILE25_*.TIM geprueft werden; Default = --satz. Fuer die
            Abnahme: re15_port/shared_assets/RE2/FILES (die KOPIERTEN Dateien)
    --soll  Nutzertext; Default analysis/befunde_runde30/irons_diary_en.txt
Bericht: <satz>/satz_pruefung.txt bzw. <satz>/satz_pruefung_kopierte_dateien.txt
"""
import argparse, glob, hashlib, json, os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")
DOC = 25
ZEILE = 16
# Satzregel des Werkzeugs (re2_doc_satz.py DATUM): deutsch "18. September 1998", englisch
# "September 18, 1998" / "September 19".
DATUM = re.compile(r"^(\d{1,2}\. [A-Z][a-z]+( \d{4})?|[A-Z][a-z]+ \d{1,2}(, \d{4})?):?$")


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
    ap = argparse.ArgumentParser()
    ap.add_argument("--satz", default=os.path.join(REPO, "build", "r30_n_diary_en"))
    ap.add_argument("--tim", default=None)
    ap.add_argument("--soll", default=os.path.join(HERE, "irons_diary_en.txt"))
    args = ap.parse_args()
    SATZ = os.path.abspath(args.satz)
    TIMD = os.path.abspath(args.tim) if args.tim else SATZ
    kopiert = os.path.normcase(TIMD) != os.path.normcase(SATZ)
    aus = []
    ok = True

    def p(s=""):
        aus.append(s)

    p("Satz-Ausgabe: %s" % os.path.relpath(SATZ, REPO))
    p("Gepruefte TIM: %s%s" % (os.path.relpath(TIMD, REPO), "  (KOPIERTE Dateien)" if kopiert else ""))
    p()

    # ---- 1. Nutzertext
    soll = open(args.soll, encoding="utf-8").read().replace("\r", "")
    ist = open(os.path.join(SATZ, "satz_eingabe.txt"), encoding="utf-8").read().replace("\r", "")
    p("[1] Nutzertext %s (%d Zeichen, md5 %s) gegen satz_eingabe.txt (%d Zeichen)"
      % (os.path.relpath(args.soll, REPO), len(soll), hashlib.md5(soll.encode("utf-8")).hexdigest(),
         len(ist)))
    if soll == ist:
        p("    ZEICHENGLEICH")
    else:
        ok = False
        for i, (x, y) in enumerate(zip(soll, ist)):
            if x != y:
                p("    ABWEICHUNG ab Zeichen %d: %r gegen %r" % (i, soll[i:i + 40], ist[i:i + 40]))
                break
        else:
            p("    ABWEICHUNG in der Laenge: %d gegen %d" % (len(soll), len(ist)))

    # ---- 2. Ruecklesen
    font = json.load(open(os.path.join(SATZ, "re2_doc_font.json"), encoding="utf-8"))
    konstr = set(c for c, g in font["glyphen"].items() if g["quelle"].startswith("KONSTRUKTION"))
    vorlage = tim(os.path.join(FILES, "FILE08_p01_page.TIM"))
    seiten = []
    n = 1
    while os.path.exists(os.path.join(TIMD, "FILE%02d_p%02d_page.TIM" % (DOC, n))):
        seiten.append(n)
        n += 1
    p()
    p("[2] Ruecklesen von %d Textseiten (FILE%02d_p01..p%02d)" % (len(seiten), DOC, seiten[-1]))
    woerter = []
    gelesen = ""
    reste_ges = 0
    regel = []
    fmt_ab = 0
    for n in seiten:
        pf = os.path.join(TIMD, "FILE%02d_p%02d_page.TIM" % (DOC, n))
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
        p("    p%02d  %d Zeilen" % (n, len(zl)))
        for k, s in enumerate(zl):
            p("        %d | %s" % (k, s))
            if DATUM.match(s) and k != 0:
                regel.append("p%02d Zeile %d: Datum %r steht NICHT am Seitenkopf" % (n, k, s))
            woerter += s.split()
            gelesen += s
        if zl and DATUM.match(zl[0]):
            regel.append("p%02d beginnt mit Datum %r" % (n, zl[0]))
    p("    Kernpixel ohne Glyphe: %d   Marken <+n>/<-n> (Abstand ausserhalb der Metrik): %d"
      % (reste_ges, sum(1 for w in woerter if "<" in w)))
    if reste_ges or any("<" in w for w in woerter):
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
    soll_daten = [ln.strip() for ln in soll.split("\n") if DATUM.match(ln.strip())]
    p("    Daten im Nutzertext: %d %r" % (len(soll_daten), soll_daten))
    p("    Seiten mit Datum am Kopf: %d   (%s)"
      % (len(kopf), " ".join(r.split()[0] for r in kopf)))
    if len(soll_daten) != len(kopf):
        ok = False

    # ---- 5. Format + Dateibestand
    p()
    p("[5] Format gegen FILE08_p01_page.TIM (%d B, Kopf+CLUT+Bildkopf %d B, VRAM-Lage %s)"
      % (len(vorlage["roh"]), len(vorlage["kopf"]), vorlage["vram"]))
    p("    Textseiten mit abweichender Groesse oder abweichendem Kopf: %d von %d" % (fmt_ab, len(seiten)))
    if fmt_ab:
        ok = False
    for name, vor in (("title_page", "FILE08_title_page.TIM"), ("p00_page", "FILE08_p00_page.TIM"),
                      ("title_paper", "FILE08_title_paper.TIM")):
        a = open(os.path.join(TIMD, "FILE%02d_%s.TIM" % (DOC, name)), "rb").read()
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
    a = open(os.path.join(TIMD, "FILE%02d_title_page.TIM" % DOC), "rb").read()
    b = open(os.path.join(TIMD, "FILE%02d_p00_page.TIM" % DOC), "rb").read()
    p("    title_page gegen p00: %s" % ("BYTE-GLEICH" if a == b else "ABWEICHEND"))
    if a != b:
        ok = False
    t = tim(os.path.join(TIMD, "FILE%02d_title_page.TIM" % DOC))
    tz = []
    for k in range(t["H"] // ZEILE):
        s, rest = ruecklesen(font, t["px"], k * ZEILE)
        reste_ges += rest
        if s:
            tz.append((k, s))
            gelesen += s
    p("    Titelseite zurueckgelesen: %r" % tz)
    bestand = sorted(os.path.basename(f) for f in glob.glob(os.path.join(TIMD, "FILE%02d_*" % DOC)))
    soll_bestand = sorted(["FILE%02d_title_page.TIM" % DOC, "FILE%02d_title_paper.TIM" % DOC] +
                          ["FILE%02d_p%02d_page.TIM" % (DOC, i) for i in range(0, len(seiten) + 1)])
    tim_bestand = [f for f in bestand if f.endswith(".TIM")]
    p("    Dateibestand FILE%02d_*.TIM: %d Dateien (Soll %d = Titel + Illustration + p00..p%02d); "
      "max_page = %d" % (DOC, len(tim_bestand), len(soll_bestand), len(seiten), len(seiten)))
    if tim_bestand != soll_bestand:
        ok = False
        p("    ABWEICHUNG Bestand: zu viel %r, fehlt %r"
          % (sorted(set(tim_bestand) - set(soll_bestand)), sorted(set(soll_bestand) - set(tim_bestand))))

    # ---- 6. Konstruktionen
    p()
    treffer = sorted(set(c for c in gelesen if c in konstr))
    p("[6] Konstruierte Glyphen im Atlas: %r" % "".join(sorted(konstr)))
    p("    davon zurueckgelesen (Textseiten + Titel): %d %r" % (len(treffer), treffer))
    if treffer:
        ok = False

    # ---- 7. Kopie byte-gleich der Satz-Ausgabe
    if kopiert:
        p()
        ab7 = 0
        for f in tim_bestand:
            x = open(os.path.join(TIMD, f), "rb").read()
            q = os.path.join(SATZ, f)
            y = open(q, "rb").read() if os.path.exists(q) else b""
            if x != y:
                ab7 += 1
                p("    ABWEICHEND: %s" % f)
        p("[7] Kopierte Dateien gegen die Satz-Ausgabe: %d von %d byte-gleich"
          % (len(tim_bestand) - ab7, len(tim_bestand)))
        if ab7:
            ok = False

    p()
    p("ERGEBNIS: %s" % ("ALLE PRUEFUNGEN BESTANDEN" if ok else "ABWEICHUNGEN - siehe oben"))
    txt = "\n".join(aus) + "\n"
    name = "satz_pruefung_kopierte_dateien.txt" if kopiert else "satz_pruefung.txt"
    open(os.path.join(SATZ, name), "w", encoding="utf-8").write(txt)
    sys.stdout.buffer.write(txt.encode("utf-8"))
    return 0 if ok else 1


sys.exit(main())
