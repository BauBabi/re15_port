#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""dokument_satz.py - Runde 41 Spur C, Nachbesserung 4 (Nachtrag 13): Satz des Dokuments "Technician's Note"
(Bildsatz FILE31) aus dem Nutzerwortlaut.

Dossier: analysis/befunde_runde41/C_ventilkette.md §0e (B4-4). Baut auf re15_port/tools/re2_doc_satz.py (Atlas, Glyphen,
Metrik, Kontur, TIM-Format) und re15_port/tools/r34n_e/doc_satz_brief.py (Satzspiegel der Vorlage, Titel) auf - beide
UNVERAENDERT importiert, damit die md5-Listen von FILE25..30 gueltig bleiben. Form wie tools/r38_g/tagebuch_satz.py.

TEXT (keine Abschrift): analysis/befunde_runde41/AUFTRAG.md, Nachtrag 13, die Zitatzeilen von "> These amateurs" bis
"> is anyone supposed to work with something like this....". Die Zeilenumbrueche dort sind die des AUFTRAGs (Umbruch
mitten im Satz bei ~120 Zeichen) -> die Zeilen werden mit EINEM Leerzeichen zu dem einen Absatz des Nutzers verbunden
(Nachtrag 13 Folge 2: "nur Umbruch/Seiten fuer den Leser"). Zitatpraefix '>' und genau EIN folgendes Leerzeichen weg.
EINZIGE ZEICHEN-ABBILDUNG: U+2014 (Gedankenstrich, im Nutzertext 2-mal) -> '-' (ASCII 0x2D). Die RE2-Dokumentschrift hat
keinen Gedankenstrich (Atlas: 85 Zeichen, nur '-'), und RE2 setzt Gedankenstriche selbst als '-' ohne Leerzeichen:
FILE22 S01 " -This report demands" / "  immediate attention-", FILE24 S11 "using the activator -VAM-". Sonst bleibt jedes
Zeichen, wie es dasteht.

SATZREGELN:
  * EIN Absatz (der Nutzertext ist ein Absatz), Umbruch an Wortgrenzen (re2_doc_satz.umbrechen, Satzspiegel der Vorlage),
    Seitenwechsel nach H/16 Zeilen (Zeilenraster 16, re2_doc_satz.ZEILE).
  * VORLAGE FILE17 = RE2-Dokument 17 "SEWER MANAGER DIARY" (PORT-WAHL: das Tagebuch des Kanal-/Klaeranlagen-Verwalters,
    thematisch die Anlage von ROOM2030; Form wie FILE16 fuer Roys Diary): Textseite = Kopf/CLUT von FILE17_p01_page.TIM
    (256 x 176; H 176 = RE2-Record @0x800AA144 + 17*4, gen/re2_files_toc.inc {9,176}), Titelseite = FILE17_title_page.TIM,
    Papier = byte-gleiche Kopie von FILE17_title_paper.TIM. Rand/Grenze an FILE17s eigenen Seiten gemessen
    (doc_satz_brief.vorlage_satzspiegel).
  * TITEL "TECHNICIAN'S NOTE" (Vorschlag, Nachtrag 13 Folge 2 "Titel als Vorschlag"): Versalien wie alle 25 RE2-Titelseiten;
    passt er in eine Zeile, steht er auf der Rasterzeile der Vorlage.

AUSGABE <out>/FILE31/: FILE31_title_page.TIM, FILE31_p00_page.TIM (= Titelseite), FILE31_p01..pNN_page.TIM,
FILE31_title_paper.TIM, FILE31_satz.txt (Bericht, max_page), *_schirm.png, pruefung (Rueckleseprobe am Artefakt).
Mit --installieren werden die TIM nach re15_port/shared_assets/RE2/FILES kopiert.

Aufruf:  C:/Python310/python.exe re15_port/tools/r41_ventilkette/dokument_satz.py [--out build/r41_ventilkette/satz] [--installieren]
"""
import argparse
import hashlib
import json
import os
import shutil
import sys
from collections import Counter

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..")))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..", "r34n_e")))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..", "r38_g")))
import re2_doc_satz as R  # noqa: E402
import doc_satz_brief as B  # noqa: E402
import tagebuch_satz as T  # noqa: E402  (png() - Schirmlayout, unveraendert)

REPO = R.REPO
AUFTRAG = os.path.join(REPO, "analysis", "befunde_runde41", "AUFTRAG.md")
ANFANG, ENDE = "> These amateurs", "> is anyone supposed to work with something like this...."
DOC = 31                                                     # Bildsatz: erster freier hinter FILE30 (Runde 38)
VORLAGE = 17                                                 # RE2 "SEWER MANAGER DIARY"
TITEL = "TECHNICIAN'S NOTE"
GEDANKENSTRICH = "\u2014"


def entpacken(z):
    if not z.startswith(">"):
        raise SystemExit("keine Zitatzeile: %r" % z)
    z = z[1:]
    if z.startswith(" "):
        z = z[1:]
    return z


def text_aus_auftrag():
    zeilen = open(AUFTRAG, encoding="utf-8").read().replace("\r", "").split("\n")
    a = [i for i, z in enumerate(zeilen) if z.startswith(ANFANG)]
    e = [i for i, z in enumerate(zeilen) if z == ENDE]
    if len(a) != 1 or len(e) != 1 or e[0] < a[0]:
        raise SystemExit("Textgrenzen nicht eindeutig: Anfang %s Ende %s" % (a, e))
    roh = [entpacken(z) for z in zeilen[a[0]:e[0] + 1]]
    if any(not z.strip() for z in roh):
        raise SystemExit("Leerzeile im Nutzertext - mehr als ein Absatz?")
    davor, danach = zeilen[a[0] - 1], zeilen[e[0] + 1]
    if davor.strip() != ">" or danach.strip() != ">":
        raise SystemExit("vor/nach dem Text steht keine leere Zitatzeile: %r / %r" % (davor, danach))
    return a[0] + 1, e[0] + 1, roh


def seiten_bauen(font, absatz, rand, xmax, nz):
    zl = R.umbrechen(font, absatz, rand, xmax)
    return [zl[i:i + nz] for i in range(0, len(zl), nz)]


def pruefen(font, out, seiten_n, soll_text, titel):
    """Rueckleseprobe AM ARTEFAKT (die geschriebenen TIM): Wortfolge, Titel, Dateibestand."""
    import satz_pruefung as P
    L = []
    ok = True
    woerter, reste, marken = [], 0, 0
    zeilen_alle = []
    for s in range(1, seiten_n + 1):
        t = P.tim(os.path.join(out, "FILE%02d_p%02d_page.TIM" % (DOC, s)))
        zl = []
        for k in range(t["H"] // 16):
            txt, rest = P.ruecklesen(font, t["px"], k * 16)
            reste += rest
            zl.append(txt)
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
    for s, zl in enumerate(zeilen_alle, 1):
        L.append("    p%02d: %s" % (s, " / ".join(zl)))
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
    ap.add_argument("--out", default=os.path.join(REPO, "build", "r41_ventilkette", "satz"))
    ap.add_argument("--font", default=None, help="Atlas re2_doc_font.json; Standard: <out>/atlas neu bauen")
    ap.add_argument("--installieren", action="store_true", help="FILE31_*.TIM nach shared_assets/RE2/FILES kopieren")
    args = ap.parse_args()

    font_pfad = args.font
    if not font_pfad:
        adir = os.path.join(args.out, "atlas")
        font_pfad = os.path.join(adir, "re2_doc_font.json")
        if not os.path.exists(font_pfad):
            os.makedirs(adir, exist_ok=True)
            R.atlas_bauen(adir)
    font = json.load(open(font_pfad, encoding="utf-8"))

    z0, z1, roh = text_aus_auftrag()
    nutzertext = " ".join(roh)
    satztext = nutzertext.replace(GEDANKENSTRICH, "-")
    out = os.path.join(args.out, "FILE%02d" % DOC)
    os.makedirs(out, exist_ok=True)
    open(os.path.join(out, "nutzertext.txt"), "w", encoding="utf-8", newline="").write(nutzertext + "\n")
    open(os.path.join(out, "satz_eingabe.txt"), "w", encoding="utf-8", newline="").write(satztext + "\n")
    R.zeichen_pruefen(font, satztext, TITEL, out, False)

    bericht = []
    bericht.append("Quelle: analysis/befunde_runde41/AUFTRAG.md (md5 %s) Nachtrag 13, Z. %d..%d (%d Zitatzeilen, mit einem "
                   "Leerzeichen verbunden)" % (hashlib.md5(open(AUFTRAG, "rb").read()).hexdigest(), z0, z1, len(roh)))
    nicht_ascii = Counter(c for c in nutzertext if ord(c) > 126)
    bericht.append("   Nutzertext %d Zeichen md5 %s; ausserhalb ASCII: %s -> im Satz '-' (U+002D); Satztext md5 %s"
                   % (len(nutzertext), hashlib.md5(nutzertext.encode("utf-8")).hexdigest(),
                      {("U+%04X" % ord(c)): n for c, n in nicht_ascii.items()},
                      hashlib.md5(satztext.encode("utf-8")).hexdigest()))

    vor_seite = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_p01_page.TIM" % VORLAGE))
    vor_titel = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_page.TIM" % VORLAGE))
    papier = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % VORLAGE))
    W, H = vor_seite["W"], vor_seite["H"]
    nz = H // R.ZEILE
    rand, xmax, rand_vert, n_zeilen = B.vorlage_satzspiegel(font, VORLAGE)
    bericht.append("Vorlage FILE%02d: Seite %dx%d, %d Zeilen je Seite; Satzspiegel gemessen: Rand %d (Verteilung %s, "
                   "%d Zeilen), Grenze x %d" % (VORLAGE, W, H, nz, rand, rand_vert.most_common(4), n_zeilen, xmax))

    # ---- Titelseite (Form tagebuch_satz.py)
    t_wmax, t_beleg = B.titel_breite_max()
    t_lines, t_regel = B.titel_zeilen(font, TITEL, t_wmax)
    tb = R.baender(vor_titel)
    if len(t_lines) == 1:
        tops = [(tb[0][0] // R.ZEILE) * R.ZEILE if tb else (nz // 2) * R.ZEILE]
    else:
        top0 = H // 2 - len(t_lines) * R.ZEILE // 2
        tops = [top0 + i * R.ZEILE for i in range(len(t_lines))]
    bericht.append("Titel %r: %s (breiteste einzeilige RE2-Titelzeile %d, %s); Vorlagenband %s -> Oberkanten %s"
                   % (TITEL, t_regel, t_wmax, t_beleg, tb[:1], tops))
    th = vor_titel["H"]
    px = [[0] * W for _ in range(th)]
    for tl, top in zip(t_lines, tops):
        a, b = R.breite_von(font, tl, 0)
        rand_t = (W - (b - a + 1)) // 2 - a
        R.zeile_setzen(font, px, tl, rand_t, top)
        x0, x1 = R.breite_von(font, tl, rand_t)
        bericht.append("   %r Oberkante %d x %d..%d (Mitte %.1f)" % (tl, top, x0, x1, (x0 + x1) / 2.0))
    px = R.kontur_ziehen(px)
    for nm in ("FILE%02d_title_page" % DOC, "FILE%02d_p00_page" % DOC):
        n = R.tim_schreiben_4bpp(os.path.join(out, nm + ".TIM"), vor_titel, px)
        T.png(out, nm, px, vor_titel["clut"], papier, W)
        bericht.append("%s.TIM  %d B" % (nm, n))
    src = os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % VORLAGE)
    dst = os.path.join(out, "FILE%02d_title_paper.TIM" % DOC)
    shutil.copyfile(src, dst)
    bericht.append("FILE%02d_title_paper.TIM  %d B = byte-gleiche Kopie von FILE%02d_title_paper.TIM"
                   % (DOC, os.path.getsize(dst), VORLAGE))

    # ---- Textseiten
    seiten = seiten_bauen(font, satztext, rand, xmax, nz)
    for i, seite in enumerate(seiten):
        px = [[0] * W for _ in range(H)]
        xm, xl = 0, W
        for k, z in enumerate(seite):
            R.zeile_setzen(font, px, z, rand, k * R.ZEILE)
            x0, x1 = R.breite_von(font, z, rand)
            xm, xl = max(xm, x1), min(xl, x0)
        px = R.kontur_ziehen(px)
        nm = "FILE%02d_p%02d_page" % (DOC, i + 1)
        n = R.tim_schreiben_4bpp(os.path.join(out, nm + ".TIM"), vor_seite, px)
        T.png(out, nm, px, vor_seite["clut"], papier, W)
        bericht.append("%s.TIM  %d B  %d Zeilen  Kern-x %d..%d" % (nm, n, len(seite), xl, xm))
        for z in seite:
            bericht.append("      | %s" % z)
    bericht.append("")
    bericht.append("max_page (RE2-Record +0, u16) = %d   y_off (+2, u8) = %d   H = %d" % (len(seiten), 256 - H, H))

    ok, L = pruefen(font, out, len(seiten), satztext, TITEL)
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
