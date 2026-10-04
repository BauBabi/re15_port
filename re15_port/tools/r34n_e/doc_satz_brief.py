#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""doc_satz_brief.py - Runde 34 Nacht, Spur E: Satz der vier neuen Dokumente (FILE26..FILE29).

Baut auf re15_port/tools/re2_doc_satz.py auf (Atlas, Glyphen, Metrik, Kontur, TIM-Format
UNVERAENDERT uebernommen - dort gemessen, Dossier analysis/befunde_runde30/irons-diary-dokument.md
§6). Neu sind nur drei Dinge, die das Irons Diary nicht brauchte, jede aus RE2s eigenen Seiten
GEMESSEN (Bericht <out>/FILEnn_satz.txt nennt die Zaehlung):

  1. TEXT OHNE DATUM. re2_doc_satz.py verlangt einen Datumskopf je Eintrag (Tagebuch-Regel) und
     bricht sonst ab. Briefe/Notizen (RE2: "Memo to LEON", "Mail to the chief", "Police
     memorandum") haben keinen. Hier ist der ganze Text EIN Eintrag ohne Kopfzeile; jede Zeile
     des Nutzertexts beginnt eine neue Satzzeile, Leerzeilen bleiben (ausser am Kopf einer
     Folgeseite) - dieselben Satzregeln wie §6.3, nur ohne Datum.
  2. RAND UND GRENZE DER VORLAGE. re2_doc_satz.py nimmt Rand 10 / Grenze 249 (FILE08 gemessen).
     Jede RE2-Vorlage hat ihren eigenen Satzspiegel (FILE00 Rand 9, FILE02 10, FILE06 4 ...).
     Hier wird er aus den Seiten der Vorlage gemessen: Rand = Modus (Kern-x - Linkslage) der
     Zeilenanfaenge mit gesicherten Kleinbuchstaben, Grenze = groesstes Kernpixel-x der Vorlage.
  3. TITEL, DER NICHT IN EINE ZEILE PASST. RE2 bricht lange Titel um (FILE01 3 Zeilen, FILE20 3,
     FILE22 2): Zeilenabstand 16, jede Zeile fuer sich waagerecht mittig. Breiter als die
     breiteste einzeilige RE2-Titelzeile (gemessen) -> zwei Zeilen, Umbruch am Wortende mit der
     schmalsten breitesten Zeile; der Block steht senkrecht mittig um H/2 (FILE20/FILE22 H 176:
     Blockmitte 88 = H/2).
  Optional 4. UNTERSCHRIFT RECHTSBUENDIG (--unterschrift): RE2 setzt eine EINZEILIGE Unterschrift
     rechtsbuendig ("William Birkin" FILE05/FILE06 p03/p06/p09: Kern-x1 250/249/250/250/249/250).
     Die Kante wird aus diesen sechs Zeilen gemessen (Modus).

  Optional 5. CODES GRUEN (--gruen TOKEN, mehrfach; Runde 35 Spur F, NUTZER-VORGABE "der Code
     darin mit diesem Gruen hervorgehoben"). Das Gruen ist das der Spieltexte, in denen das
     Original seine Codes faerbt: ROOM1110.RDT @0x0DB0 `61 05 01 10 0f 0d 0e 05 00` ("4312") und
     ROOM1230.RDT @0x172F `61 05 01 11 12 0f 0e 05 00` ("5632") — Steuerbyte 0x05 Argument 1.
     Textmaler FUN_80028868: `andi v1,v0,0x4` @0x80028974 ... `addiu v1,v1,480` @0x80028984 ...
     `ori s5,v0,0x10` @0x80028994 -> CLUT-Zeile y = 480 + (N&3)*2 + ((N&4)!=0) = 482 fuer N = 1
     = Zeile 2 des CLUT-Blocks von DATA/TEX.TIM (Kopf x 256 y 480, 32x24; Zeile 2 @0x94).
     Die weisse Zeile 0 (@0x14) traegt in 1..6 dieselben Farbworte wie der Kern-Grauverlauf 1..6
     jeder RE2-Dokumentseite (wird hier geprueft, sonst Abbruch) -> Kernindex v des Tokens wird
     v + 8 (9..14), CLUT-Eintraege 9..14 der Seite = TEX.TIM Zeile 2 Eintraege 1..6, aus der Datei
     gelesen. Index 9..14 nutzt keine der 191 RE2-Textseiten (Zensus F_inhalt.md Punkt 2). Die
     Kontur (Index 8) bleibt die der Seite.

Aufruf:
  python re15_port/tools/r34n_e/doc_satz_brief.py --out build/r34n_e/satz/FILE26 \
      --font build/r34n_e/atlas/re2_doc_font.json \
      --text analysis/befunde_runde34_nacht/E_texte/dok1_police_officer.txt \
      --titel "POLICE OFFICER'S FINAL DIARY ENTRY" --doc 26 --vorlage 0
"""
import argparse
import os
import struct
import sys
from collections import Counter

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HIER, "..")))
import re2_doc_satz as R  # noqa: E402

ZEILE = R.ZEILE
SICHER = set("abcdefghiklmnoprstuvwy")      # dieselbe Eichmenge wie der Atlas (re2_doc_satz.py)


# ------------------------------------------------------------------ Messungen an RE2
def vorlage_satzspiegel(font, vorlage):
    """Rand (Modus) und Grenze (groesstes Kern-x) der Textseiten eines RE2-Dokuments."""
    tr = R.transkription(vorlage)
    rand = Counter()
    xmax = 0
    zeilen = 0
    for key, lines in tr.items():
        if key == "TITEL":
            continue
        p = os.path.join(R.FILES, "FILE%02d_p%02d_page.TIM" % (vorlage, int(key[1:])))
        if not os.path.exists(p):
            continue
        t = R.tim_lesen(p)
        for k, txt in enumerate(lines):
            if k >= t["H"] // ZEILE or not txt.strip():
                continue
            sg = R.segmente(t, k * ZEILE, k * ZEILE + ZEILE - 1)
            if not sg:
                continue
            zeilen += 1
            xmax = max(xmax, sg[-1][1])
            c = txt[0]
            if c != " " and c in SICHER and c in font["glyphen"]:
                rand[sg[0][0] - font["glyphen"][c]["links"]] += 1
    if not rand:
        raise SystemExit("Vorlage FILE%02d: kein Zeilenanfang messbar" % vorlage)
    return rand.most_common(1)[0][0], xmax, rand, zeilen


def unterschrift_kante():
    """Kern-x1 der einzeiligen, eingerueckten RE2-Unterschriften 'William Birkin' (FILE05/06)."""
    x1 = Counter()
    for doc in (5, 6):
        tr = R.transkription(doc)
        for key, lines in tr.items():
            if key == "TITEL":
                continue
            p = os.path.join(R.FILES, "FILE%02d_p%02d_page.TIM" % (doc, int(key[1:])))
            t = R.tim_lesen(p)
            for k, txt in enumerate(lines):
                if k < t["H"] // ZEILE and txt.strip() == "William Birkin" and txt.startswith(" "):
                    sg = R.segmente(t, k * ZEILE, k * ZEILE + ZEILE - 1)
                    x1[sg[-1][1]] += 1
    return x1.most_common(1)[0][0], x1


def titel_breite_max():
    """Breiteste EINZEILIGE Titelzeile aller 25 RE2-Titelseiten (Kern-x0..x1)."""
    best = (0, None)
    for doc in range(25):
        tr = R.transkription(doc)
        lines = [l for l in tr.get("TITEL", []) if l.strip()]
        if len(lines) != 1:
            continue
        t = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_page.TIM" % doc))
        for (y0, y1) in R.baender(t):
            sg = R.segmente(t, y0, y1)
            if sg:
                w = sg[-1][1] - sg[0][0] + 1
                if w > best[0]:
                    best = (w, "FILE%02d %r x %d..%d" % (doc, lines[0], sg[0][0], sg[-1][1]))
    return best


# ------------------------------------------------------------------ Gruen (Optional 5)
GRUEN_VERSATZ = 8                 # Kern 1..6 -> 9..14 (9..14 auf keiner RE2-Textseite belegt)
TEX_TIM = os.path.join(R.REPO, "re15_port", "shared_assets", "PSX", "DATA", "TEX.TIM")


def tex_zeile(n_attr):
    """Die 16 Farbworte der CLUT-Zeile, die FUN_80028868 fuer Steuerbyte 0x05 Argument n waehlt
    (@0x80028974-94: y = 480 + (n&3)*2 + ((n&4)!=0)), aus DATA/TEX.TIM."""
    b = open(TEX_TIM, "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    assert magic == 0x10 and flags & 8, "TEX.TIM ohne CLUT"
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, 8)
    y = 480 + (n_attr & 3) * 2 + (1 if n_attr & 4 else 0)
    zeile = y - cy
    assert cx == 256 and 0 <= zeile < ch, "TEX.TIM-CLUT nicht bei (256,480)"
    return list(struct.unpack_from("<16H", b, 8 + 12 + zeile * cw * 2))


def gruen_vorlage(vor):
    """Kopie der Seitenvorlage mit CLUT 9..14 = gruene Textmaler-Zeile 1..6 (Argument 1)."""
    weiss, gruen = tex_zeile(0), tex_zeile(1)
    if vor["clut"][1:7] != weiss[1:7]:
        raise SystemExit("Kernverlauf der Vorlage != TEX.TIM Zeile 0 (1..6): %s / %s"
                         % (vor["clut"][1:7], weiss[1:7]))
    clut = list(vor["clut"])
    for i in range(1, 7):
        clut[i + GRUEN_VERSATZ] = gruen[i]
    roh = bytearray(vor["clut_roh"])
    for i in range(1, 7):
        struct.pack_into("<H", roh, 12 + 2 * (i + GRUEN_VERSATZ), gruen[i])
    neu = dict(vor)
    neu["clut"] = clut
    neu["clut_roh"] = bytes(roh)
    return neu


def ist_kern_g(v):
    return 1 <= v <= 6 or 1 + GRUEN_VERSATZ <= v <= 6 + GRUEN_VERSATZ


def kontur_ziehen_g(px):
    """R.kontur_ziehen mit Kern 1..6 UND 9..14 (sonst ueberschriebe die Kontur gruene Kerne)."""
    H = len(px)
    W = len(px[0])
    neu = [row[:] for row in px]
    for y in range(H):
        for x in range(W):
            if ist_kern_g(px[y][x]):
                continue
            if any(0 <= y + dy < H and 0 <= x + dx < W and ist_kern_g(px[y + dy][x + dx])
                   for dy in (-1, 0, 1) for dx in (-1, 0, 1)):
                neu[y][x] = R.KONTUR
    return neu


def gruen_faerben(font, px, z, pen, y0, tokens):
    """Kernpixel der Glyphen jedes Tokens in Zeile z auf v + 8. Rueckgabe: [(token, x0, x1)]."""
    treffer = []
    l = R.lauf(font, z, pen)                  # nur Nicht-Leerzeichen, in Textreihenfolge
    for tok in tokens:
        start = 0
        while True:
            i = z.find(tok, start)
            if i < 0:
                break
            start = i + len(tok)
            k0 = len(z[:i].replace(" ", ""))
            k1 = k0 + len(tok.replace(" ", ""))
            xs = []
            for ch, x in l[k0:k1]:
                g = font["glyphen"][ch]
                for r, row in enumerate(g["bitmap"]):
                    for s, chx in enumerate(row):
                        v = int(chx, 16)
                        if 1 <= v <= 6 and 0 <= y0 + r < len(px) and 0 <= x + s < len(px[0]):
                            px[y0 + r][x + s] = v + GRUEN_VERSATZ
                            xs.append(x + s)
            treffer.append((tok, min(xs), max(xs)))
    return treffer


# ------------------------------------------------------------------ Satz
def titel_zeilen(font, titel, wmax):
    a, b = R.breite_von(font, titel, 0)
    if b - a + 1 <= wmax:
        return [titel], "eine Zeile (Breite %d <= %d)" % (b - a + 1, wmax)
    w = titel.split(" ")
    best = None
    for i in range(1, len(w)):
        l1, l2 = " ".join(w[:i]), " ".join(w[i:])
        b1 = R.breite_von(font, l1, 0)
        b2 = R.breite_von(font, l2, 0)
        m = max(b1[1] - b1[0] + 1, b2[1] - b2[0] + 1)
        if best is None or m < best[0]:
            best = (m, [l1, l2])
    if best[0] > wmax:
        raise SystemExit("Titel passt auch zweizeilig nicht: %r" % titel)
    return best[1], "zwei Zeilen (einzeilig %d > %d; breiteste Zeile zweizeilig %d)" % (
        b - a + 1, wmax, best[0])


def setze(args):
    import json
    font = json.load(open(args.font, encoding="utf-8"))
    vor_seite = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_p01_page.TIM" % args.vorlage))
    gruen = list(args.gruen or [])
    vor_seite_gruen = gruen_vorlage(vor_seite) if gruen else None   # CLUT 9..14 = TEX.TIM Zeile 2
    gruen_treffer = []
    vor_titel = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_page.TIM" % args.vorlage))
    papier = R.tim_lesen(os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % args.vorlage))
    W, H = vor_seite["W"], vor_seite["H"]
    nz = H // ZEILE
    text = open(args.text, encoding="utf-8").read().replace("\r", "")
    os.makedirs(args.out, exist_ok=True)
    R.zeichen_pruefen(font, text, args.titel, args.out, False)
    open(os.path.join(args.out, "satz_eingabe.txt"), "w", encoding="utf-8", newline="").write(text)

    rand, xmax, rand_verteilung, n_zeilen = vorlage_satzspiegel(font, args.vorlage)
    if args.rand is not None:
        rand = args.rand
    if args.xmax is not None:
        xmax = args.xmax
    sig_x1, sig_vert = unterschrift_kante()
    t_wmax, t_wmax_beleg = titel_breite_max()

    # ---- Textzeilen: jede Nutzerzeile beginnt eine Satzzeile; '' = Leerzeile
    nutzer = text.split("\n")
    while nutzer and nutzer[-1] == "":
        nutzer.pop()
    letzte = max(i for i, z in enumerate(nutzer) if z.strip())
    zl = []                               # (text, pen_start)
    for i, z in enumerate(nutzer):
        if z == "":
            zl.append(("", None))
            continue
        if args.unterschrift and i == letzte:
            a, b = R.breite_von(font, z, 0)
            zl.append((z, sig_x1 - b))
            continue
        for s in R.umbrechen(font, z, rand, xmax):
            zl.append((s, rand))
    seiten = []
    while zl:
        seite = zl[:nz]
        zl = zl[nz:]
        while zl and zl[0][0] == "":
            zl.pop(0)                     # keine Leerzeile am Kopf einer Folgeseite (§6.3)
        seiten.append(seite)

    farbe = [((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
             for c in (vor_seite_gruen or vor_seite)["clut"]]   # 0..8 gleich, 9..14 gruen
    from PIL import Image

    def png(px, name):
        h = len(px)
        a = Image.new("RGBA", (W, h), (0, 0, 0, 0))
        b = Image.new("RGB", (W, h), (18, 26, 32))
        for y in range(h):
            for x in range(W):
                v = px[y][x]
                if v:
                    a.putpixel((x, y), farbe[v] + (255,))
                    b.putpixel((x, y), farbe[v])
        a.save(os.path.join(args.out, name + ".png"))
        b.save(os.path.join(args.out, name + "_lesbar.png"))
        # RE2-Schirmlayout: Illustration bei (100,60), 128 x (256-H), abgetastet ab v = H
        # (@0x80072584-94, @0x800760b8-dc), Textseite bei (25,30) (@0x80076170-84), Grund schwarz.
        s = Image.new("RGB", (320, 240), (0, 0, 0))
        for v in range(256 - h):
            for u in range(128):
                c = papier["clut"][papier["px"][h + v][u]]
                if c != 0:
                    s.putpixel((100 + u, 60 + v), ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3))
        for y in range(h):
            for x in range(W):
                v = px[y][x]
                if v and 0 <= 25 + x < 320 and 0 <= 30 + y < 240:
                    s.putpixel((25 + x, 30 + y), farbe[v])
        s.save(os.path.join(args.out, name + "_schirm.png"))

    bericht = []
    bericht.append("Vorlage FILE%02d: Seite %dx%d (H %d, %d Zeilen je Seite)" % (args.vorlage, W, H, H, nz))
    bericht.append("Satzspiegel der Vorlage gemessen: Rand %d (Verteilung %s, %d Zeilen), Grenze x %d"
                   % (rand, rand_verteilung.most_common(4), n_zeilen, xmax))
    if args.unterschrift:
        bericht.append("Unterschrift rechtsbuendig: Kern-x1 %d = Modus der RE2-Unterschriften "
                       "'William Birkin' FILE05/06 %s" % (sig_x1, sorted(sig_vert.items())))
    # ---- Titel
    t_lines, t_regel = titel_zeilen(font, args.titel, t_wmax)
    bericht.append("Titel %r: %s; breiteste einzeilige RE2-Titelzeile %d (%s)"
                   % (args.titel, t_regel, t_wmax, t_wmax_beleg))
    tb = R.baender(vor_titel)
    if len(t_lines) == 1:
        tops = [(tb[0][0] // ZEILE) * ZEILE if tb else (nz // 2) * ZEILE]
        bericht.append("   Titelzeile auf der Rasterzeile der Vorlage (erstes Band y %d -> Oberkante %d)"
                       % (tb[0][0] if tb else -1, tops[0]))
    else:
        top0 = H // 2 - len(t_lines) * ZEILE // 2
        tops = [top0 + i * ZEILE for i in range(len(t_lines))]
        bericht.append("   Block mittig um H/2 = %d: Zeilenoberkanten %s" % (H // 2, tops))
    px = [[0] * W for _ in range(H)]
    for tl, top in zip(t_lines, tops):
        a, b = R.breite_von(font, tl, 0)
        rand_t = (W - (b - a + 1)) // 2 - a
        R.zeile_setzen(font, px, tl, rand_t, top)
        x0, x1 = R.breite_von(font, tl, rand_t)
        bericht.append("   %r Oberkante %d  x %d..%d (Mitte %.1f)" % (tl, top, x0, x1, (x0 + x1) / 2.0))
    px = R.kontur_ziehen(px)
    for name in ("FILE%02d_title_page" % args.doc, "FILE%02d_p00_page" % args.doc):
        n = R.tim_schreiben_4bpp(os.path.join(args.out, name + ".TIM"), vor_titel, px)
        png(px, name)
        bericht.append("%s.TIM  %d B" % (name, n))
    src = os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % args.vorlage)
    dst = os.path.join(args.out, "FILE%02d_title_paper.TIM" % args.doc)
    open(dst, "wb").write(open(src, "rb").read())
    bericht.append("FILE%02d_title_paper.TIM  %d B = byte-gleiche Kopie von FILE%02d_title_paper.TIM"
                   % (args.doc, os.path.getsize(dst), args.vorlage))
    # ---- Textseiten
    for i, seite in enumerate(seiten):
        px = [[0] * W for _ in range(H)]
        xm = 0
        xl = W
        for k, (z, pen) in enumerate(seite):
            if z:
                R.zeile_setzen(font, px, z, pen, k * ZEILE)
                if gruen:
                    for tok, gx0, gx1 in gruen_faerben(font, px, z, pen, k * ZEILE, gruen):
                        gruen_treffer.append((i + 1, k, tok, gx0, gx1))
                x0, x1 = R.breite_von(font, z, pen)
                xm = max(xm, x1)
                xl = min(xl, x0)
        px = kontur_ziehen_g(px) if gruen else R.kontur_ziehen(px)
        name = "FILE%02d_p%02d_page" % (args.doc, i + 1)
        hat_gruen = any(t[0] == i + 1 for t in gruen_treffer)
        # nur Seiten MIT gruenem Token bekommen die erweiterte CLUT; alle anderen bleiben byte-gleich
        n = R.tim_schreiben_4bpp(os.path.join(args.out, name + ".TIM"),
                                 vor_seite_gruen if hat_gruen else vor_seite, px)
        png(px, name)
        bericht.append("%s.TIM  %d B  %d Zeilen  Kern-x %d..%d" % (name, n, len(seite), xl, xm))
        for z, pen in seite:
            bericht.append("      | %s%s" % (z, "" if pen in (None, rand) else "   [Stift %d]" % pen))
    if gruen:
        for tok in gruen:
            if not any(t[2] == tok for t in gruen_treffer):
                raise SystemExit("--gruen %r kommt im Text nicht vor" % tok)
        bericht.append("GRUEN (Steuerbyte 0x05 Arg 1 -> TEX.TIM CLUT-Zeile 2, @0x80028974-94): "
                       "CLUT 9..14 = %s" % ["%04x" % c for c in vor_seite_gruen["clut"][9:15]])
        for seite_nr, zeile, tok, gx0, gx1 in gruen_treffer:
            bericht.append("   p%02d Zeile %d (y %d..%d): %r Kern-x %d..%d gruen"
                           % (seite_nr, zeile, zeile * ZEILE, zeile * ZEILE + ZEILE - 1, tok, gx0, gx1))
    bericht.append("")
    bericht.append("max_page (RE2-Record +0, u16) = %d   y_off (+2, u8) = %d   H = %d"
                   % (len(seiten), 256 - H, H))
    open(os.path.join(args.out, "FILE%02d_satz.txt" % args.doc), "w", encoding="utf-8").write(
        "\n".join(bericht) + "\n")
    print("\n".join(bericht))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", required=True)
    ap.add_argument("--font", required=True)
    ap.add_argument("--text", required=True)
    ap.add_argument("--titel", required=True)
    ap.add_argument("--doc", type=int, required=True)
    ap.add_argument("--vorlage", type=int, required=True)
    ap.add_argument("--rand", type=int, default=None, help="Stiftlage; Standard = gemessen an der Vorlage")
    ap.add_argument("--xmax", type=int, default=None, help="Grenze; Standard = gemessen an der Vorlage")
    ap.add_argument("--gruen", action="append", default=None,
                    help="Token (z.B. Code), dessen Glyphen im Textmaler-Gruen gesetzt werden (mehrfach)")
    ap.add_argument("--unterschrift", action="store_true",
                    help="letzte Textzeile rechtsbuendig wie RE2s einzeilige Unterschriften")
    setze(ap.parse_args())


if __name__ == "__main__":
    main()
