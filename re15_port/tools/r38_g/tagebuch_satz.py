#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tagebuch_satz.py - Runde 38 Spur G: Satz von "Roys Diary" (Bildsatz FILE30) aus dem Nutzerwortlaut.

Dossier: analysis/befunde_runde38/G_tagebuch.md. Baut auf re15_port/tools/re2_doc_satz.py (Atlas, Glyphen, Metrik,
Kontur, TIM-Format) und re15_port/tools/r34n_e/doc_satz_brief.py (Satzspiegel der Vorlage, Titel, Gruen) auf -
beide UNVERAENDERT importiert, damit die md5-Listen von FILE25..29 gueltig bleiben.

TEXT (keine Abschrift): analysis/befunde_runde38/AUFTRAG.md Z. 37 ("- Header: Roys Diary") und Z. 39..59 (Text),
entpackt nach den Regeln von tools/r34n_e/texte_aus_auftrag.py: Zitatpraefix '>' und genau EIN folgendes Leerzeichen
weg, fuehrende Tabs weg; "- Header: " ist die Beschriftung des Nutzers. Eine Zeile '>' allein = Leerzeile.
EINZIGE ZEICHEN-ABBILDUNG: der typografische Apostroph U+2019 (im Nutzertext 9-mal) wird zum einzigen Apostroph
der RE2-Dokumentschrift (ASCII 0x27, Atlas-Glyphe "'" - gemessen an RE2s eigenen Seiten, z.B. FILE16 S01 "It's
all over."). Sonst bleibt jedes Zeichen, wie es dasteht.

SATZREGELN:
  * ZEILEN = NUTZER-VORGABE: jede Nutzerzeile beginnt eine Satzzeile, jede Leerzeile bleibt, ausser am Kopf einer
    Seite (dieselbe Regel wie doc_satz_brief.py, Runde 34 Nacht E: "mit Absaetzen").
  * SEITEN = RE2-TAGEBUCHFORM: jeder Datumseintrag beginnt eine neue Seite. Gemessen in `zensus_datum()` an den
    RE2-Transkriptionen: jeder Datumskopf der sechs Tagebuecher FILE00/07/08/15/16/17 steht als ERSTE Zeile
    einer Seite (20 von 20; die Einsatzberichte FILE04/09 setzen eine Kopfzeile davor).
    Die Datumsform traegt die englische Ordinal-Endung ("September 23rd", FILE16 S01); re2_doc_satz.DATUM kennt
    sie nicht, DATUM_ORD hier ergaenzt sie.
  * VORLAGE FILE16 = RE2-Dokument 16 "Chief's diary" (NUTZER-VORGABE: "Hintergrund koennte FILE16_title_paper.png
    sein"): Textseite = Kopf/CLUT von FILE16_p01_page.TIM (256 x 176; H 176 = RE2-Record @0x800AA144 + 16*4,
    gen/re2_files_toc.inc {6,176}), Titelseite = FILE16_title_page.TIM, Papier = byte-gleiche Kopie von
    FILE16_title_paper.TIM. Rand/Grenze an FILE16s eigenen Seiten gemessen (doc_satz_brief.vorlage_satzspiegel).
  * CODE GRUEN (NUTZER-VORGABE "wie alle Codes in den Dokumenten"): doc_satz_brief Optional 5 - Kernindex v -> v+8,
    CLUT 9..14 = TEX.TIM CLUT-Zeile 482 (Steuerbyte 0x05 Arg 1 des Textmalers FUN_80028868 @0x80028974-94), wie
    FILE28 "4312" / FILE29 "5632" (Runde 35 Spur F) und wie das Original die Codes faerbt (ROOM1011 msg 18
    @0x1231: `05 01 0c 11 0d 0f 05 00` = "0513" gruen).
  * TITEL: Versalien wie alle 25 RE2-Titelseiten; passt er in eine Zeile, steht er auf der Rasterzeile der Vorlage.

AUSGABE <out>/FILE30/: FILE30_title_page.TIM, FILE30_p00_page.TIM (= Titelseite), FILE30_p01..pNN_page.TIM,
FILE30_title_paper.TIM, FILE30_satz.txt (Bericht, max_page), *_schirm.png (RE2-Schirmlayout), satz_eingabe.txt,
pruefung.txt (Rueckleseprobe). Mit --installieren werden die TIM nach re15_port/shared_assets/RE2/FILES kopiert.

Aufruf:  C:/Python310/python.exe re15_port/tools/r38_g/tagebuch_satz.py [--out build/r38_g/satz] [--installieren]
"""
import argparse
import hashlib
import json
import os
import re
import shutil
import sys
from collections import Counter

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..")))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..", "r34n_e")))
import re2_doc_satz as R  # noqa: E402
import doc_satz_brief as B  # noqa: E402

REPO = R.REPO
AUFTRAG = os.path.join(REPO, "analysis", "befunde_runde38", "AUFTRAG.md")
ZEILE_KOPF, ZEILE_TEXT0, ZEILE_TEXT1 = 37, 39, 59          # 1-basiert, gegen den Inhalt geprueft (unten)
DOC = 30                                                     # Bildsatz: erster freier hinter FILE29 (Runde 34)
VORLAGE = 16                                                 # RE2 "Chief's diary"
GRUEN = ["0513"]
APOSTROPH_TYPO = "\u2019"

# Datum als ganze Zeile: "September 22nd" / "September 28th" (RE2: "September 23rd", "July 21st", "August 8th").
DATUM_ORD = re.compile(r"^[A-Z][a-z]+ \d{1,2}(st|nd|rd|th)?(, \d{4})?:?$")


def entpacken(z):
    if not z.startswith(">"):
        raise SystemExit("keine Zitatzeile: %r" % z)
    z = z[1:]
    if z.startswith(" "):
        z = z[1:]
    return z.lstrip("\t")


def text_aus_auftrag():
    zeilen = open(AUFTRAG, encoding="utf-8").read().replace("\r", "").split("\n")   # auch CRLF-Auscheckung
    kopf = entpacken(zeilen[ZEILE_KOPF - 1])
    if not kopf.startswith("- Header: "):
        raise SystemExit("Z. %d ist nicht der Kopf: %r" % (ZEILE_KOPF, kopf))
    if entpacken(zeilen[ZEILE_KOPF]) != "- Text:":
        raise SystemExit("Z. %d ist nicht 'Text:'" % (ZEILE_KOPF + 1))
    name = kopf[len("- Header: "):]
    text = [entpacken(z) for z in zeilen[ZEILE_TEXT0 - 1:ZEILE_TEXT1]]
    if not DATUM_ORD.match(text[0]) or not text[-1].startswith("I just hope"):
        raise SystemExit("Textgrenzen passen nicht: %r .. %r" % (text[0], text[-1]))
    nach = [entpacken(z) for z in zeilen[ZEILE_TEXT1:ZEILE_TEXT1 + 3]]
    if nach[0] != "" or nach[1] != "" or not nach[2].startswith("Den Code bitte"):
        raise SystemExit("hinter dem Text steht nicht die Gruen-Anweisung: %r" % nach)
    return name, "\n".join(text) + "\n", nach


def zensus_datum():
    """Je Datumskopf der RE2-Transkriptionen: steht er als erste Zeile einer Seite?"""
    erste, sonst, belege = 0, 0, []
    for doc in range(25):
        try:
            tr = R.transkription(doc)
        except OSError:
            continue
        for key, lines in tr.items():
            if key == "TITEL":
                continue
            for k, ln in enumerate(lines):
                if DATUM_ORD.match(ln.strip()):
                    if k == 0:
                        erste += 1
                        belege.append("FILE%02d %s %r" % (doc, key, ln.strip()))
                    else:
                        sonst += 1
                        belege.append("FILE%02d %s Zeile %d %r (NICHT oben)" % (doc, key, k, ln.strip()))
    return erste, sonst, belege


def seiten_bauen(font, text, rand, xmax, nz):
    nutzer = text.split("\n")
    while nutzer and nutzer[-1] == "":
        nutzer.pop()
    eintraege = []
    for z in nutzer:
        if DATUM_ORD.match(z.strip()):
            eintraege.append([z])
            continue
        if not eintraege:
            if z.strip():
                raise SystemExit("Text vor dem ersten Datum: %r" % z)
            continue
        eintraege[-1].append(z)
    seiten = []
    for e in eintraege:
        while e and e[-1] == "":
            e.pop()
        zl = []
        for z in e:
            if z == "":
                zl.append(("", None))
                continue
            for s in R.umbrechen(font, z, rand, xmax):
                zl.append((s, rand))
        while zl:
            seite = zl[:nz]
            zl = zl[nz:]
            while zl and zl[0][0] == "":
                zl.pop(0)                 # keine Leerzeile am Kopf einer Folgeseite
            seiten.append(seite)
    return eintraege, seiten


def farben(clut):
    return [((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3) for c in clut]


def png(out, name, px, clut, papier, W):
    from PIL import Image
    farbe = farben(clut)
    h = len(px)
    a = Image.new("RGBA", (W, h), (0, 0, 0, 0))
    for y in range(h):
        for x in range(W):
            if px[y][x]:
                a.putpixel((x, y), farbe[px[y][x]] + (255,))
    a.save(os.path.join(out, name + ".png"))
    # RE2-Schirmlayout (doc_satz_brief.png): Illustration bei (100,60), 128 x (256-H), abgetastet ab v = H
    # (@0x80072584-94, @0x800760b8-dc), Textseite bei (25,30) (@0x80076170-84), Grund schwarz.
    s = Image.new("RGB", (320, 240), (0, 0, 0))
    for v in range(256 - h):
        for u in range(128):
            c = papier["clut"][papier["px"][h + v][u]]
            if c != 0:
                s.putpixel((100 + u, 60 + v), ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3))
    for y in range(h):
        for x in range(W):
            if px[y][x] and 0 <= 25 + x < 320 and 0 <= 30 + y < 240:
                s.putpixel((25 + x, 30 + y), farbe[px[y][x]])
    s.save(os.path.join(out, name + "_schirm.png"))


def pruefen(font, out, seiten_n, soll_text, titel, gruen_soll):
    """Rueckleseprobe AM ARTEFAKT (die geschriebenen TIM): Wortfolge, Zeilenanfaenge, Gruen, Titel."""
    sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..", "r34n_e")))
    import satz_pruefung as P
    L = []
    ok = True
    woerter, reste, marken, zeilen_alle = [], 0, 0, []
    gruen_px, gruen_zeilen = 0, []
    for s in range(1, seiten_n + 1):
        t = P.tim(os.path.join(out, "FILE%02d_p%02d_page.TIM" % (DOC, s)))
        px = [[(v - B.GRUEN_VERSATZ) if 1 + B.GRUEN_VERSATZ <= v <= 6 + B.GRUEN_VERSATZ else v for v in row]
              for row in t["px"]]
        zl = []
        for k in range(t["H"] // 16):
            txt, rest = P.ruecklesen(font, px, k * 16)
            reste += rest
            zl.append(txt)
            g = sum(1 for y in range(k * 16, k * 16 + 16) for v in t["px"][y]
                    if 1 + B.GRUEN_VERSATZ <= v <= 6 + B.GRUEN_VERSATZ)
            if g:
                gruen_px += g
                gruen_zeilen.append((s, k, txt, g))
        while zl and zl[-1] == "":
            zl.pop()
        for z in zl:
            woerter += z.split()
            marken += sum(1 for w in z.split() if "<" in w)
        zeilen_alle.append(zl)
    ok &= (reste == 0 and marken == 0)
    L.append("[2] %d Textseiten zurueckgelesen: Kernpixel ohne Glyphe %d, Abstaende ausserhalb der Metrik %d"
             % (seiten_n, reste, marken))
    sw = soll_text.split()
    wg = (sw == woerter)
    ok &= wg
    L.append("[3] Wortfolge: Satztext %d Woerter, zurueckgelesen %d: %s" % (len(sw), len(woerter),
                                                                            "WORTGLEICH" if wg else "ABWEICHEND"))
    if not wg:
        for i, (a, b) in enumerate(zip(sw, woerter)):
            if a != b:
                L.append("    erste Abweichung Wort %d: Soll %r Ist %r" % (i, a, b))
                break
    anfaenge = [z.strip() for zl in zeilen_alle for z in zl if z.strip()]
    fehl = [u for u in [z for z in soll_text.split("\n") if z.strip()]
            if not any(u == a or u.startswith(a + " ") for a in anfaenge)]
    ok &= not fehl
    L.append("[4] Nutzerzeilen, die NICHT am Anfang einer Satzzeile beginnen: %d %s" % (len(fehl), fehl[:3]))
    for s, zl in enumerate(zeilen_alle, 1):
        L.append("    p%02d: %s" % (s, " / ".join(zl)))
    # [5] Gruen: genau die Kernpixel des Tokens, nur in seiner Zeile
    ok_g = (gruen_px == gruen_soll and len(gruen_zeilen) == 1 and GRUEN[0] in gruen_zeilen[0][2])
    ok &= ok_g
    L.append("[5] gruene Kernpixel (Index 9..14) %d, Soll %d (Kernpixel der Glyphen von %r); Zeilen %s: %s"
             % (gruen_px, gruen_soll, GRUEN[0], [(s, k, g) for s, k, _, g in gruen_zeilen],
                "STIMMT" if ok_g else "FALSCH"))
    # [6] Titel zurueckgelesen
    tt = P.tim(os.path.join(out, "FILE%02d_title_page.TIM" % DOC))
    tz = []
    for (y0, y1) in P.banden(tt):
        best = None
        for top in (y0 - 4, y0 - 3, y0 - 5):
            txt, rest = P.ruecklesen(font, tt["px"], top)
            if txt and rest == 0 and (best is None or len(txt) > len(best)):
                best = txt
        tz.append(best or "")
    tp = open(os.path.join(out, "FILE%02d_title_page.TIM" % DOC), "rb").read()
    p0 = open(os.path.join(out, "FILE%02d_p00_page.TIM" % DOC), "rb").read()
    pap = open(os.path.join(out, "FILE%02d_title_paper.TIM" % DOC), "rb").read()
    vpap = open(os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % VORLAGE), "rb").read()
    ok_t = (" ".join(tz) == titel and tp == p0 and pap == vpap)
    ok &= ok_t
    L.append("[6] Titel zurueckgelesen %r (Soll %r), title_page == p00: %s, title_paper == FILE%02d: %s"
             % (tz, titel, tp == p0, VORLAGE, pap == vpap))
    ok &= not os.path.exists(os.path.join(out, "FILE%02d_p%02d_page.TIM" % (DOC, seiten_n + 1)))
    L.append("[7] Dateibestand p00..p%02d, kein p%02d" % (seiten_n, seiten_n + 1))
    L.append("PRUEFUNG FILE%02d: %s" % (DOC, "ALLE BESTANDEN" if ok else "FEHLER"))
    return ok, L


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", default=os.path.join(REPO, "build", "r38_g", "satz"))
    ap.add_argument("--font", default=None, help="Atlas re2_doc_font.json; Standard: <out>/atlas neu bauen")
    ap.add_argument("--installieren", action="store_true", help="FILE30_*.TIM nach shared_assets/RE2/FILES kopieren")
    args = ap.parse_args()

    font_pfad = args.font
    if not font_pfad:
        adir = os.path.join(args.out, "atlas")
        font_pfad = os.path.join(adir, "re2_doc_font.json")
        if not os.path.exists(font_pfad):
            os.makedirs(adir, exist_ok=True)
            R.atlas_bauen(adir)
    font = json.load(open(font_pfad, encoding="utf-8"))

    name, nutzertext, nach = text_aus_auftrag()
    satztext = nutzertext.replace(APOSTROPH_TYPO, "'")
    titel = name.upper()
    out = os.path.join(args.out, "FILE%02d" % DOC)
    os.makedirs(out, exist_ok=True)
    open(os.path.join(out, "nutzertext.txt"), "w", encoding="utf-8", newline="").write(nutzertext)
    open(os.path.join(out, "satz_eingabe.txt"), "w", encoding="utf-8", newline="").write(satztext)
    R.zeichen_pruefen(font, satztext, titel, out, False)

    bericht = []
    bericht.append("Quelle: analysis/befunde_runde38/AUFTRAG.md (md5 %s) Kopf Z. %d, Text Z. %d..%d"
                   % (hashlib.md5(open(AUFTRAG, "rb").read()).hexdigest(), ZEILE_KOPF, ZEILE_TEXT0, ZEILE_TEXT1))
    bericht.append("   Listenname %r, Titel %r; Nutzertext %d Zeichen md5 %s, Zeilen %d (leer %d)"
                   % (name, titel, len(nutzertext), hashlib.md5(nutzertext.encode("utf-8")).hexdigest(),
                      nutzertext.count("\n"), sum(1 for z in nutzertext.split("\n")[:-1] if z == "")))
    nicht_ascii = Counter(c for c in nutzertext if ord(c) > 126)
    bericht.append("   Zeichen ausserhalb ASCII: %s -> im Satz '\\'' (U+0027); Satztext md5 %s"
                   % ({("U+%04X" % ord(c)): n for c, n in nicht_ascii.items()},
                      hashlib.md5(satztext.encode("utf-8")).hexdigest()))
    bericht.append("   Zeilen danach (gehoeren NICHT dazu): %r" % nach)
    erste, sonst, belege = zensus_datum()
    bericht.append("Zensus RE2-Tagebuecher: Datumskoepfe als ERSTE Seitenzeile %d, anderswo %d" % (erste, sonst))
    for b in belege:
        bericht.append("   " + b)

    vor_seite = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_p01_page.TIM" % VORLAGE))
    vor_titel = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_page.TIM" % VORLAGE))
    papier = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % VORLAGE))
    vor_gruen = B.gruen_vorlage(vor_seite)
    W, H = vor_seite["W"], vor_seite["H"]
    nz = H // R.ZEILE
    rand, xmax, rand_vert, n_zeilen = B.vorlage_satzspiegel(font, VORLAGE)
    bericht.append("Vorlage FILE%02d: Seite %dx%d, %d Zeilen je Seite; Satzspiegel gemessen: Rand %d (Verteilung %s, "
                   "%d Zeilen), Grenze x %d" % (VORLAGE, W, H, nz, rand, rand_vert.most_common(4), n_zeilen, xmax))

    # ---- Titelseite
    t_wmax, t_beleg = B.titel_breite_max()
    t_lines, t_regel = B.titel_zeilen(font, titel, t_wmax)
    tb = R.baender(vor_titel)
    if len(t_lines) == 1:
        tops = [(tb[0][0] // R.ZEILE) * R.ZEILE if tb else (nz // 2) * R.ZEILE]
    else:
        top0 = H // 2 - len(t_lines) * R.ZEILE // 2
        tops = [top0 + i * R.ZEILE for i in range(len(t_lines))]
    bericht.append("Titel %r: %s (breiteste einzeilige RE2-Titelzeile %d, %s); Vorlagenband %s -> Oberkanten %s"
                   % (titel, t_regel, t_wmax, t_beleg, tb[:1], tops))
    px = [[0] * W for _ in range(H)]
    for tl, top in zip(t_lines, tops):
        a, b = R.breite_von(font, tl, 0)
        rand_t = (W - (b - a + 1)) // 2 - a
        R.zeile_setzen(font, px, tl, rand_t, top)
        x0, x1 = R.breite_von(font, tl, rand_t)
        bericht.append("   %r Oberkante %d x %d..%d (Mitte %.1f)" % (tl, top, x0, x1, (x0 + x1) / 2.0))
    px = R.kontur_ziehen(px)
    for nm in ("FILE%02d_title_page" % DOC, "FILE%02d_p00_page" % DOC):
        n = R.tim_schreiben_4bpp(os.path.join(out, nm + ".TIM"), vor_titel, px)
        png(out, nm, px, vor_titel["clut"], papier, W)
        bericht.append("%s.TIM  %d B" % (nm, n))
    src = os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % VORLAGE)
    dst = os.path.join(out, "FILE%02d_title_paper.TIM" % DOC)
    shutil.copyfile(src, dst)
    bericht.append("FILE%02d_title_paper.TIM  %d B = byte-gleiche Kopie von FILE%02d_title_paper.TIM"
                   % (DOC, os.path.getsize(dst), VORLAGE))

    # ---- Textseiten
    eintraege, seiten = seiten_bauen(font, satztext, rand, xmax, nz)
    bericht.append("Eintraege: %s" % [(e[0], len(e)) for e in eintraege])
    gruen_soll = 0
    for tok in GRUEN:
        for ch in tok:
            gruen_soll += sum(1 for row in font["glyphen"][ch]["bitmap"] for c in row if 1 <= int(c, 16) <= 6)
    gruen_treffer = []
    for i, seite in enumerate(seiten):
        px = [[0] * W for _ in range(H)]
        xm, xl = 0, W
        for k, (z, pen) in enumerate(seite):
            if not z:
                continue
            R.zeile_setzen(font, px, z, pen, k * R.ZEILE)
            for tok, gx0, gx1 in B.gruen_faerben(font, px, z, pen, k * R.ZEILE, GRUEN):
                gruen_treffer.append((i + 1, k, tok, gx0, gx1))
            x0, x1 = R.breite_von(font, z, pen)
            xm, xl = max(xm, x1), min(xl, x0)
        px = B.kontur_ziehen_g(px)
        nm = "FILE%02d_p%02d_page" % (DOC, i + 1)
        hat_gruen = any(t[0] == i + 1 for t in gruen_treffer)
        vor = vor_gruen if hat_gruen else vor_seite
        n = R.tim_schreiben_4bpp(os.path.join(out, nm + ".TIM"), vor, px)
        png(out, nm, px, vor["clut"], papier, W)
        bericht.append("%s.TIM  %d B  %d Zeilen  Kern-x %d..%d%s" % (nm, n, len(seite), xl, xm,
                                                                  "  (CLUT 9..14 gruen)" if hat_gruen else ""))
        for z, pen in seite:
            bericht.append("      | %s" % z)
    for tok in GRUEN:
        if not any(t[2] == tok for t in gruen_treffer):
            raise SystemExit("Gruen-Token %r kommt im Text nicht vor" % tok)
    bericht.append("GRUEN (Steuerbyte 0x05 Arg 1 -> TEX.TIM CLUT-Zeile 482, @0x80028974-94): CLUT 9..14 = %s"
                   % ["%04x" % c for c in vor_gruen["clut"][9:15]])
    for s, k, tok, gx0, gx1 in gruen_treffer:
        bericht.append("   p%02d Zeile %d (y %d..%d): %r Kern-x %d..%d gruen" % (s, k, k * 16, k * 16 + 15, tok, gx0, gx1))
    bericht.append("")
    bericht.append("max_page (RE2-Record +0, u16) = %d   y_off (+2, u8) = %d   H = %d" % (len(seiten), 256 - H, H))

    ok, L = pruefen(font, out, len(seiten), satztext, titel, gruen_soll)
    bericht += [""] + L
    md5 = []
    for f in sorted(os.listdir(out)):
        if f.endswith(".TIM"):
            md5.append("%s %s" % (hashlib.md5(open(os.path.join(out, f), "rb").read()).hexdigest(), f))
    bericht += [""] + ["md5 " + m for m in md5]
    open(os.path.join(out, "FILE%02d_satz.txt" % DOC), "w", encoding="utf-8").write("\n".join(bericht) + "\n")
    print("\n".join(bericht))
    if not ok:
        raise SystemExit("PRUEFUNG FEHLGESCHLAGEN")
    if args.installieren:
        for f in sorted(os.listdir(out)):
            if f.endswith(".TIM"):
                shutil.copyfile(os.path.join(out, f), os.path.join(R.FILES, f))
        print("installiert: %d Dateien nach %s" % (len(md5), R.FILES))


if __name__ == "__main__":
    main()
