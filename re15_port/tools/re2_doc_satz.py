#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_doc_satz.py - Schriftsatz fuer EIGENE Dokumente im RE2-FILE-Format.

RE2s Dokumentseiten sind gerasterte 4bpp-Bilder (kein Zeichenstrom). Dieses Werkzeug
gewinnt den Schriftsatz AUS DEN ORIGINALSEITEN und setzt damit neuen Text:

  atlas   liest die 191 Textseiten + 25 Titelseiten aus shared_assets/RE2/FILES/ und die
          Transkriptionen aus extracted_re2_dokumente/texte/transkription/, schneidet je
          Zeichen die Glyphe (Palettenindizes 1..6) und misst Vorschub/Linkslage.
          Ausgabe: <out>/re2_doc_font.json, <out>/glyphen_atlas.png, <out>/atlas_bericht.txt
  satz    setzt einen Text (UTF-8) auf Seiten 256xH und schreibt PNG + TIM im exakten
          FILE-Format (Koepfe/Masse/CLUT byte-gleich einer Vorlagenseite).

NICHTS HIER IST GERATEN - jede Groesse ist eine Messung an den Originalen, der Bericht
nennt die Zaehlung dazu. Was NICHT gemessen werden kann, ist als KONSTRUKTION oder
ANALOGIE markiert (Umlaute, Eszett; Linkslage seltener Versalien).

Gemessene Regeln (Bericht: atlas_bericht.txt):
  * Zeilenraster 16 px; Versalhoehe Zeilen +4..+11, x-Hoehe +7..+11.
  * Glyphenkern = Indizes 1..6 (Grauverlauf), Kontur = Index 8 in der 8er-Nachbarschaft
    des Kerns, Papier = Index 0 (durchsichtig). CLUT[0..8] ist auf allen 191 Seiten gleich.
  * Proportionale Schrift mit GANZZAHLIGEM Vorschub (die meisten Zeichen 9, i 5, l/j/I 7,
    Komma/Punkt/Apostroph 6, Leerzeichen 6).
  * Punktfolgen "..": Abstand 4 statt 6; "!!": 6 statt 9.

Aufruf:
  python re15_port/tools/re2_doc_satz.py atlas --out build/r30_irons-diary-dokument
  python re15_port/tools/re2_doc_satz.py satz  --out build/r30_n_diary_en \
         --font build/r30_n_diary_en/re2_doc_font.json \
         --text analysis/befunde_runde30/irons_diary_en.txt \
         --titel "IRONS DIARY" --doc 25 --vorlage 8

  Vor dem Satz prueft `satz` jedes Zeichen von Text und Titel gegen den Atlas (Bericht
  <out>/zeichen_pruefung.txt): fehlt eine Glyphe -> Abbruch; ist sie eine KONSTRUKTION
  (Umlaute, Eszett) -> Abbruch, ausser mit --konstruktion-erlaubt (die deutsche Fassung
  der Runde 30 E brauchte das; die englische J braucht es nicht).
"""
import argparse, glob, json, os, re, struct, sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
FILES = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "FILES")
TRANS = os.path.join(REPO, "extracted_re2_dokumente", "texte", "transkription")

ZEILE = 16                      # Zeilenraster, gemessen (Baender der Seiten, s. Bericht)
KONTUR = 8                      # Kontur-Index


# ------------------------------------------------------------------ TIM lesen/schreiben
def tim_lesen(pfad):
    b = open(pfad, "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    if magic != 0x10:
        raise ValueError("kein TIM: " + pfad)
    o = 8
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o)
    clut = list(struct.unpack_from("<%dH" % (cw * ch), b, o + 12))
    clut_roh = b[o:o + clen]
    o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, o)
    daten = b[o + 12:o + ilen]
    bpp = 4 if (flags & 7) == 0 else 8
    W = iw * 4 if bpp == 4 else iw * 2
    px = []
    for y in range(ih):
        if bpp == 4:
            zeile = []
            for x in range(W):
                by = daten[y * (W // 2) + x // 2]
                zeile.append((by & 15) if x % 2 == 0 else (by >> 4))
        else:
            zeile = list(daten[y * W:(y + 1) * W])
        px.append(zeile)
    return dict(roh=b, flags=flags, clut=clut, clut_roh=clut_roh, clut_rect=(cx, cy, cw, ch),
                img_rect=(ix, iy, iw, ih), W=W, H=ih, px=px, bpp=bpp)


def tim_schreiben_4bpp(pfad, vorlage, px):
    """4bpp-TIM mit dem Kopf und der CLUT der Vorlage (byte-gleich), neuer Bildinhalt."""
    H = len(px)
    W = len(px[0])
    assert W == vorlage["W"] and vorlage["bpp"] == 4
    ix, iy, iw, ih = vorlage["img_rect"]
    daten = bytearray()
    for y in range(H):
        for x in range(0, W, 2):
            daten.append((px[y][x] & 15) | ((px[y][x + 1] & 15) << 4))
    out = bytearray()
    out += struct.pack("<II", 0x10, vorlage["flags"])
    out += vorlage["clut_roh"]
    out += struct.pack("<IHHHH", 12 + len(daten), ix, iy, iw, H)
    out += daten
    open(pfad, "wb").write(out)
    return len(out)


def ist_kern(v):
    return 1 <= v <= 6


# ------------------------------------------------------------------ Transkription
def transkription(doc):
    seiten = {}
    cur = None
    pfad = os.path.join(TRANS, "FILE%02d.txt" % doc)
    for ln in open(pfad, encoding="utf-8").read().split("\n"):
        m = re.match(r"^=== (\w+) ===", ln)
        if m:
            cur = m.group(1)
            seiten[cur] = []
            continue
        if cur is not None:
            seiten[cur].append(ln.rstrip())
    for k in seiten:
        while seiten[k] and seiten[k][-1] == "":
            seiten[k].pop()
    return seiten


def segmente(t, y0, y1):
    W = t["W"]
    sp = [any(ist_kern(t["px"][y][x]) for y in range(y0, y1 + 1)) for x in range(W)]
    out = []
    x = 0
    while x < W:
        if sp[x]:
            a = x
            while x < W and sp[x]:
                x += 1
            out.append((a, x - 1))
        else:
            x += 1
    return out


def baender(t):
    z = [any(ist_kern(v) for v in r) for r in t["px"]]
    out = []
    y = 0
    while y < t["H"]:
        if z[y]:
            a = y
            while y < t["H"] and z[y]:
                y += 1
            out.append((a, y - 1))
        else:
            y += 1
    return out


def ausschnitt(t, x0, x1, y0, y1):
    return tuple(tuple(t["px"][y][x] if ist_kern(t["px"][y][x]) else 0
                       for x in range(x0, x1 + 1)) for y in range(y0, y1 + 1))


# ------------------------------------------------------------------ ATLAS
def atlas_bauen(out_dir):
    import numpy as np
    glyphen = defaultdict(Counter)
    zeilen = []                                   # (doc, seite, zeile, [(c,x0,x1)], text)
    stat = Counter()
    for doc in range(25):
        tr = transkription(doc)
        for key, lines in tr.items():
            if key == "TITEL":
                continue
            name = os.path.join(FILES, "FILE%02d_p%02d_page.TIM" % (doc, int(key[1:])))
            if not os.path.exists(name):
                stat["seite_fehlt"] += 1
                continue
            t = tim_lesen(name)
            for k in range(t["H"] // ZEILE):
                y0, y1 = ZEILE * k, ZEILE * k + ZEILE - 1
                sg = segmente(t, y0, y1)
                txt = lines[k] if k < len(lines) else ""
                zeichen = [c for c in txt if c != " "]
                if not sg and not zeichen:
                    continue
                stat["zeilen"] += 1
                if len(sg) != len(zeichen):
                    stat["zeilen_verworfen_segmentzahl"] += 1
                    continue
                row = []
                for (x0, x1), c in zip(sg, zeichen):
                    glyphen[c][ausschnitt(t, x0, x1, y0, y1)] += 1
                    row.append((c, x0, x1))
                zeilen.append((doc, int(key[1:]), k, row, txt))
                stat["zeilen_benutzt"] += 1
    kanon = {c: v.most_common(1)[0][0] for c, v in glyphen.items()}
    zeichen = sorted(kanon)
    ci = {c: i for i, c in enumerate(zeichen)}
    N = len(zeichen)

    # ---- Paarabstaende; Gleichungen nur OHNE Leerzeichen (dort sitzt die Ausgleichs-Streuung)
    paar = defaultdict(Counter)
    for doc, pg, k, row, txt in zeilen:
        idx = [i for i, ch in enumerate(txt) if ch != " "]
        for j in range(len(row) - 1):
            a, ax0, _ = row[j]
            b, bx0, _ = row[j + 1]
            paar[(a, b, idx[j + 1] - idx[j] - 1)][bx0 - ax0] += 1

    def folgepaar(a, b):
        return a == b and a in ".!"

    def loesen(extra):
        A = []
        y = []
        w = []
        for (a, b, nsp), v in paar.items():
            if nsp != 0 or folgepaar(a, b):
                continue
            d, n = v.most_common(1)[0]
            r = np.zeros(2 * N)
            r[ci[a]] = 1                                              # r[a] + lb[b] = d
            r[N + ci[b]] = 1
            A.append(r)
            y.append(d)
            w.append(n)
        for c, d, n in extra:                                         # lb[c] = d
            r = np.zeros(2 * N)
            r[N + ci[c]] = 1
            A.append(r)
            y.append(d)
            w.append(n)
        r = np.zeros(2 * N)
        r[N + ci["n"]] = 1                                            # Eichung lb['n'] = 0
        A.append(r)
        y.append(0)
        w.append(1e6)
        A = np.array(A)
        y = np.array(y, float)
        w = np.sqrt(np.array(w, float))
        return np.linalg.lstsq(A * w[:, None], y * w, rcond=None)[0]

    sol = loesen([])
    rr = {c: int(round(sol[ci[c]])) for c in zeichen}
    lb = {c: int(round(sol[N + ci[c]])) for c in zeichen}

    # ---- Seitenrand je Seite (Modus) und Linkslage aus Zeilenanfaengen
    sicher = set("abcdefghiklmnoprstuvwy")
    proseite = defaultdict(list)
    for doc, pg, k, row, txt in zeilen:
        if not txt.startswith(" "):
            proseite[(doc, pg)].append(row)
    rand = {}
    anfang = defaultdict(Counter)
    for key, rows in proseite.items():
        c = Counter(r[0][1] - lb[r[0][0]] for r in rows if r[0][0] in sicher)
        if not c or c.most_common(1)[0][1] < 3:
            continue
        rand[key] = c.most_common(1)[0][0]
        for r in rows:
            anfang[r[0][0]][r[0][1] - rand[key]] += 1
    extra = [(c, v.most_common(1)[0][0], v.most_common(1)[0][1]) for c, v in anfang.items()]
    sol = loesen(extra)
    rr = {c: int(round(sol[ci[c]])) for c in zeichen}
    lb = {c: int(round(sol[N + ci[c]])) for c in zeichen}

    # ---- Belege je Zeichen zaehlen. Zwei Quellen: Paare (ohne und mit EINEM Leerzeichen)
    #      gegen einen schon gesicherten Partner, und Zeilenanfaenge gegen den Seitenrand.
    #      Die Menge der gesicherten Zeichen waechst dabei schrittweise (Ziffern und
    #      Satzzeichen haengen im Original fast nur an Ziffern bzw. an Leerzeichen).
    LEER0 = 6                                      # wird unten aus den Daten bestaetigt
    gesichert = set(sicher)
    for runde in range(4):
        bel_r = defaultdict(Counter)
        bel_l = defaultdict(Counter)
        for (a, b, nsp), v in paar.items():
            if nsp > 1 or folgepaar(a, b):
                continue
            for d, n in v.items():
                if b in gesichert:
                    bel_r[a][d - lb[b] - nsp * LEER0] += n
                if a in gesichert:
                    bel_l[b][d - rr[a] - nsp * LEER0] += n
        for c, v in anfang.items():
            for d, n in v.items():
                bel_l[c][d] += n
        for c in zeichen:
            if c in sicher:
                continue                           # die Eichmenge bleibt beim LSQ-Ergebnis
            if bel_l[c]:
                lb[c] = bel_l[c].most_common(1)[0][0]
            if bel_r[c]:
                rr[c] = bel_r[c].most_common(1)[0][0]
        for c in zeichen:
            if c in gesichert or not bel_l[c] or not bel_r[c]:
                continue
            (lv, ln), (rv, rn) = bel_l[c].most_common(1)[0], bel_r[c].most_common(1)[0]
            if ln >= 3 and rn >= 3 and ln * 10 >= 6 * sum(bel_l[c].values())                     and rn * 10 >= 6 * sum(bel_r[c].values()):
                gesichert.add(c)
    # Leerzeichen
    leer = Counter()
    for (a, b, nsp), v in paar.items():
        if nsp == 1 and a in sicher and b in sicher:
            for d, n in v.items():
                leer[d - rr[a] - lb[b]] += n
    leer_w = leer.most_common(1)[0][0]
    folge = {}
    for c in ".!":
        v = paar.get((c, c, 0))
        if v:
            folge[c] = (v.most_common(1)[0][0], sum(v.values()), v.most_common(1)[0][1])

    # ---- Gegenprobe: wie viele Originalzeilen setzt die Metrik PIXELGENAU nach?
    exakt = 0
    gepr = 0
    for doc, pg, k, row, txt in zeilen:
        if txt.startswith(" "):
            continue
        gepr += 1
        pen = row[0][1] - lb[row[0][0]]
        j = 0
        ok = True
        vor = None
        for ch in txt:
            if ch == " ":
                pen += leer_w
                vor = None
                continue
            if vor == ch and ch in folge:
                pen += folge[ch][0] - (rr[ch] + lb[ch])
            if row[j][1] != pen + lb[ch]:
                ok = False
                break
            pen += rr[ch] + lb[ch]
            vor = ch
            j += 1
        exakt += ok

    # ---- Konturregel + CLUT-Zensus ueber alle Seiten
    kz = Counter()
    clutvar = Counter()
    for f in sorted(glob.glob(os.path.join(FILES, "FILE*_page.TIM"))):
        if int(os.path.basename(f)[4:6]) >= 25:
            continue                   # nur RE2s Saetze 0..24; FILE25 ist die EIGENE Ausgabe
        t = tim_lesen(f)
        W, H, px = t["W"], t["H"], t["px"]
        clutvar[tuple(t["clut"][:9])] += 1
        kz["seiten"] += 1
        for y in range(H):
            for x in range(W):
                v = px[y][x]
                if ist_kern(v):
                    kz["kern"] += 1
                    continue
                nah = any(0 <= y + dy < H and 0 <= x + dx < W and ist_kern(px[y + dy][x + dx])
                          for dy in (-1, 0, 1) for dx in (-1, 0, 1))
                if nah:
                    kz["kontur_soll"] += 1
                    if v != KONTUR:
                        kz["kontur_fehlt"] += 1
                elif v != 0:
                    kz["nichtnull_ohne_kern"] += 1

    # ---- Titelseiten: gleiche Glyphen? Lage?
    titel = []
    t_ok = t_bad = 0

    def trimmen(bm):
        ys = [i for i, r in enumerate(bm) if any(r)]
        return tuple(bm[ys[0]:ys[-1] + 1])
    ktrim = {c: trimmen(b) for c, b in kanon.items()}
    for doc in range(25):
        tr = transkription(doc)
        lines = [l for l in tr.get("TITEL", []) if l.strip()]
        t = tim_lesen(os.path.join(FILES, "FILE%02d_title_page.TIM" % doc))
        grp = []
        for b in baender(t):
            if grp and b[0] - grp[-1][1] <= 3:
                grp[-1] = (grp[-1][0], b[1])
            else:
                grp.append(b)
        if len(grp) != len(lines):
            continue
        for (y0, y1), txt in zip(grp, lines):
            sg = segmente(t, y0, y1)
            zs = [c for c in txt if c != " "]
            if len(sg) != len(zs):
                continue
            for (x0, x1), c in zip(sg, zs):
                if c in ktrim and ktrim[c] == trimmen(ausschnitt(t, x0, x1, y0, y1)):
                    t_ok += 1
                else:
                    t_bad += 1
            titel.append(dict(doc=doc, H=t["H"], y0=y0, y1=y1, x0=sg[0][0], x1=sg[-1][1], text=txt))

    # ---- Font-Datei
    font = dict(zeile=ZEILE, leer=leer_w, folge={c: v[0] for c, v in folge.items()},
                kontur=KONTUR, glyphen={})
    for c in zeichen:
        n = sum(glyphen[c].values())
        font["glyphen"][c] = dict(
            bitmap=["".join("%x" % v for v in r) for r in kanon[c]],
            breite=len(kanon[c][0]), vorschub=rr[c] + lb[c], links=lb[c],
            n=n, n_kanon=glyphen[c].most_common(1)[0][1], varianten=len(glyphen[c]),
            beleg_vorschub_minus_links=sorted(bel_r[c].items(), key=lambda kv: -kv[1])[:3],
            beleg_links=sorted(bel_l[c].items(), key=lambda kv: -kv[1])[:3],
            quelle="gemessen")
    for c in zeichen:                              # Luecken ausdruecklich markieren
        g = font["glyphen"][c]
        g["gesichert"] = c in gesichert
        if g["breite"] == 7 and sum(n for _, n in g["beleg_links"]) < 2:
            # Alle 7 breiten Versalien mit mehreren Belegen liegen bei 0. Ein EINZELNER Beleg
            # hinter einem Leerzeichen zaehlt nicht: genau dort streut das Original
            # (Leerzeichen 5..9 statt 6, s. Bericht).
            g["links"] = 0
            g["vorschub"] = rr[c]
            g["quelle"] = ("gemessen; LINKSLAGE NICHT BELEGT (%s) - 0 gesetzt in ANALOGIE zu den "
                           "uebrigen 7 breiten Versalien" % (g["beleg_links"] or "kein Beleg"))
        elif not g["beleg_links"]:
            g["links"] = 0
            g["vorschub"] = rr[c]
            g["quelle"] = ("gemessen; LINKSLAGE NICHT BEOBACHTET - 0 gesetzt in ANALOGIE zu den "
                           "uebrigen Zeichen gleicher Kernbreite")
        if not g["beleg_vorschub_minus_links"]:
            g["vorschub"] = 9
            g["quelle"] = ("gemessen; VORSCHUB NICHT BEOBACHTET (steht im Original nur vor "
                           "Zeilenende/Leerzeichen) - 9 gesetzt in ANALOGIE")

    # ---- KONSTRUKTIONEN (nicht im englischen Satz enthalten)
    def bm(c):
        return [list(int(ch, 16) for ch in r) for r in font["glyphen"][c]["bitmap"]]

    def fertig(rows):
        return ["".join("%x" % v for v in r) for r in rows]
    ipunkt = 1                                     # Index des i-Punkts (Zeile 5 der Glyphe 'i')

    def punkte(basis, zeile, spalten, name, text):
        rows = bm(basis)
        for s in spalten:
            rows[zeile][s] = ipunkt
        g = dict(font["glyphen"][basis])
        g.update(bitmap=fertig(rows), n=0, n_kanon=0, varianten=0,
                 quelle="KONSTRUKTION: " + text)
        font["glyphen"][name] = g
    punkte("a", 5, (1, 4), "ä", "'a' + zwei Punkte (Index 1 wie der i-Punkt) in Zeile 5, Spalten 1 und 4")
    punkte("o", 5, (1, 3), "ö", "'o' + zwei Punkte in Zeile 5, Spalten 1 und 3")
    punkte("u", 5, (2, 4), "ü", "'u' + zwei Punkte in Zeile 5, Spalten 2 und 4")
    punkte("A", 2, (2, 4), "Ä", "'A' + zwei Punkte in Zeile 2, Spalten 2 und 4")
    punkte("O", 2, (2, 4), "Ö", "'O' + zwei Punkte in Zeile 2, Spalten 2 und 4")
    punkte("U", 2, (2, 4), "Ü", "'U' + zwei Punkte in Zeile 2, Spalten 2 und 4")
    rows = bm("B")
    rows[4] = [0, 6, 3, 3, 3, 5, 0]                # Kopf gerundet, linke Serife entfernt
    rows[7] = [0, 1, 0, 3, 2, 6, 0]                # Steg vom Stamm geloest
    rows[11] = [3, 1, 0, 3, 3, 3, 6]               # Fuss: Stamm mit Serife, Luecke, Bauch
    g = dict(font["glyphen"]["B"])
    g.update(bitmap=fertig(rows), n=0, n_kanon=0, varianten=0,
             quelle="KONSTRUKTION: Versal 'B', Zeilen 4/7/11 geaendert (Kopf gerundet, Steg "
                    "und Fuss vom Stamm geloest)")
    font["glyphen"]["ß"] = g

    os.makedirs(out_dir, exist_ok=True)
    json.dump(font, open(os.path.join(out_dir, "re2_doc_font.json"), "w", encoding="utf-8"),
              ensure_ascii=False, indent=1)

    # ---- Bericht
    L = []
    L.append("RE2-Dokumentschrift - Messbericht (re2_doc_satz.py atlas)")
    L.append("Quelle: %s  +  %s" % (os.path.relpath(FILES, REPO), os.path.relpath(TRANS, REPO)))
    L.append("")
    L.append("Zeilen mit Inhalt: %d   benutzt: %d   verworfen (Segmentzahl != Zeichenzahl): %d"
             % (stat["zeilen"], stat["zeilen_benutzt"], stat["zeilen_verworfen_segmentzahl"]))
    L.append("Glyphen (Zeichen): %d   Instanzen: %d"
             % (N, sum(sum(v.values()) for v in glyphen.values())))
    L.append("Instanzen, die NICHT die haeufigste Bitmap ihres Zeichens tragen: %d"
             % sum(sum(v.values()) - v.most_common(1)[0][1] for v in glyphen.values()))
    L.append("")
    L.append("GEGENPROBE Metrik: %d von %d linksbuendigen Originalzeilen werden PIXELGENAU "
             "nachgesetzt" % (exakt, gepr))
    L.append("Leerzeichen: %d  (Verteilung %s)"
             % (leer_w, sorted(leer.items(), key=lambda kv: -kv[1])[:5]))
    for c, v in folge.items():
        L.append("Folge '%s%s': Abstand %d  (%d von %d Paaren)" % (c, c, v[0], v[2], v[1]))
    L.append("")
    L.append("KONTURREGEL (Index 8 = 8er-Nachbarschaft des Kerns), alle %d Seiten:" % kz["seiten"])
    L.append("  Kernpixel %d, Kontur-Sollpixel %d, davon NICHT Index 8: %d, Nichtnull ohne Kern: %d"
             % (kz["kern"], kz["kontur_soll"], kz["kontur_fehlt"], kz["nichtnull_ohne_kern"]))
    L.append("CLUT[0..8]-Varianten ueber alle Seiten: %d" % len(clutvar))
    for kv, n in clutvar.items():
        L.append("  %s  x%d" % (" ".join("%04x" % c for c in kv), n))
    L.append("")
    L.append("TITELSEITEN: Glyphen identisch mit den Textseiten-Glyphen: %d, abweichend: %d"
             % (t_ok, t_bad))
    for t in titel:
        L.append("  doc %2d H=%3d  y %3d..%3d  x %3d..%3d  Mitte x=%.1f  %r"
                 % (t["doc"], t["H"], t["y0"], t["y1"], t["x0"], t["x1"],
                    (t["x0"] + t["x1"]) / 2.0, t["text"]))
    L.append("")
    L.append("SEITENRAND (Modus der Stiftlage am Zeilenanfang) je Seite:")
    for key in sorted(rand):
        if key[0] in (7, 8, 16):
            L.append("  FILE%02d p%02d  Rand %d" % (key[0], key[1], rand[key]))
    L.append("  Verteilung ueber alle Seiten: %s" % sorted(Counter(rand.values()).items()))
    L.append("")
    L.append("METRIK je Zeichen  (Vorschub, Linkslage, Kernbreite; Belege = (Wert, Anzahl))")
    for c in sorted(font["glyphen"]):
        g = font["glyphen"][c]
        L.append("  %-3r n=%4d  Vorschub %2d  links %2d  breit %d  V-L %s  L %s  [%s]"
                 % (c, g["n"], g["vorschub"], g["links"], g["breite"],
                    g["beleg_vorschub_minus_links"], g["beleg_links"], g["quelle"]))
    open(os.path.join(out_dir, "atlas_bericht.txt"), "w", encoding="utf-8").write(
        "\n".join(L) + "\n")

    # ---- Atlas-Bild
    try:
        from PIL import Image
    except ImportError:
        Image = None
    if Image is not None:
        vor = tim_lesen(os.path.join(FILES, "FILE08_p01_page.TIM"))
        farbe = [((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3) for c in vor["clut"]]
        alle = sorted(font["glyphen"])
        sp = 16
        zl = (len(alle) + sp - 1) // sp
        img = Image.new("RGB", (sp * 12 + 4, zl * 20 + 4), (18, 26, 32))
        for i, c in enumerate(alle):
            g = font["glyphen"][c]
            gx = 4 + (i % sp) * 12
            gy = 4 + (i // sp) * 20
            kern = set()
            for r, row in enumerate(g["bitmap"]):
                for s, chx in enumerate(row):
                    v = int(chx, 16)
                    if v:
                        kern.add((s, r))
                        img.putpixel((gx + s, gy + r), farbe[v])
            for (s, r) in list(kern):
                for dy in (-1, 0, 1):
                    for dx in (-1, 0, 1):
                        if (s + dx, r + dy) not in kern:
                            xx, yy = gx + s + dx, gy + r + dy
                            if 0 <= xx < img.width and 0 <= yy < img.height:
                                img.putpixel((xx, yy), farbe[KONTUR])
            if g["quelle"].startswith("KONSTRUKTION"):
                for s in range(10):
                    img.putpixel((gx - 1 + s, gy + 18), (200, 60, 60))   # rot = Konstruktion
        img = img.resize((img.width * 4, img.height * 4), Image.NEAREST)
        img.save(os.path.join(out_dir, "glyphen_atlas.png"))
    print("atlas: %d Zeichen, Gegenprobe %d/%d Zeilen pixelgenau -> %s"
          % (len(font["glyphen"]), exakt, gepr, out_dir))


# ------------------------------------------------------------------ SATZ
def lauf(font, text, rand):
    """Liefert [(zeichen, x_links_des_Kerns)] einer Zeile bei Stiftstart `rand`."""
    pen = rand
    vor = None
    g_vor = None
    out = []
    for ch in text:
        if ch == " ":
            pen += font["leer"]
            vor = None
            continue
        g = font["glyphen"][ch]
        if vor == ch and ch in font["folge"]:
            pen += font["folge"][ch] - g_vor["vorschub"]
        out.append((ch, pen + g["links"]))
        pen += g["vorschub"]
        vor = ch
        g_vor = g
    return out


def breite_von(font, text, rand=0):
    l = lauf(font, text, rand)
    if not l:
        return None, None
    return l[0][1], max(x + font["glyphen"][c]["breite"] - 1 for c, x in l)


def zeile_setzen(font, px, text, rand, y0):
    for ch, x in lauf(font, text, rand):
        g = font["glyphen"][ch]
        for r, row in enumerate(g["bitmap"]):
            for s, chx in enumerate(row):
                v = int(chx, 16)
                if v and 0 <= y0 + r < len(px) and 0 <= x + s < len(px[0]):
                    px[y0 + r][x + s] = v


def kontur_ziehen(px):
    H = len(px)
    W = len(px[0])
    neu = [row[:] for row in px]
    for y in range(H):
        for x in range(W):
            if ist_kern(px[y][x]):
                continue
            if any(0 <= y + dy < H and 0 <= x + dx < W and ist_kern(px[y + dy][x + dx])
                   for dy in (-1, 0, 1) for dx in (-1, 0, 1)):
                neu[y][x] = KONTUR
    return neu


def umbrechen(font, absatz, rand, x_max):
    zeilen = []
    cur = ""
    woerter = []
    for w in absatz.split(" "):
        if w == "":
            continue
        # SATZREGEL (keine Messung): eine alleinstehende Punktfolge beginnt keine Zeile,
        # sie bleibt mit einem Leerzeichen am Wort davor haengen.
        if woerter and set(w) == {"."}:
            woerter[-1] += " " + w
        else:
            woerter.append(w)
    for w in woerter:
        probe = w if not cur else cur + " " + w
        _, x1 = breite_von(font, probe, rand)
        if x1 > x_max and cur:
            zeilen.append(cur)
            cur = w
        else:
            cur = probe
    if cur:
        zeilen.append(cur)
    for z in zeilen:
        _, x1 = breite_von(font, z, rand)
        if x1 > x_max:
            raise SystemExit("Wort passt nicht in die Zeile: %r (x1=%d > %d)" % (z, x1, x_max))
    return zeilen


# SATZREGEL (keine Messung): eine Zeile, die nur aus einem Datum besteht, beginnt einen Eintrag.
# Deutsch "18. September 1998" / "19. September"; englisch (Runde 30 J) "September 18, 1998" /
# "September 19" - so schreiben auch RE2s Tagebuecher ("September 26th", FILE-Transkription).
DATUM = re.compile(r"^(\d{1,2}\. [A-Z][a-z]+( \d{4})?"
                   r"|[A-Z][a-z]+ \d{1,2}(, \d{4})?):?$")


def eintraege(text):
    """Zerlegt den Text in Eintraege [kopfzeile, [absaetze...]]; '' = Leerzeile."""
    out = []
    cur = None
    for ln in text.replace("\r", "").split("\n"):
        s = ln.strip()
        if DATUM.match(s):
            cur = [s, []]
            out.append(cur)
            continue
        if cur is None:
            if s:
                raise SystemExit("Text vor dem ersten Datum: %r" % s)
            continue
        cur[1].append(s)
    for e in out:
        while e[1] and e[1][0] == "":
            e[1].pop(0)                 # RE2 setzt den Text DIREKT unter das Datum (FILE08 p01)
        while e[1] and e[1][-1] == "":
            e[1].pop()
    return out


def zeichen_pruefen(font, text, titel, out_dir, konstruktion_erlaubt):
    """VOR dem Satz: hat jedes Zeichen von Text und Titel eine GEMESSENE Glyphe?

    Abbruch bei (a) Zeichen ohne Glyphe, (b) Zeichen, deren Glyphe eine KONSTRUKTION ist
    (Umlaute/Eszett, Abschnitt 6.2 des Dossiers), solange --konstruktion-erlaubt fehlt.
    Zeichen mit NICHT gesicherter Metrik (Atlas: weniger als 3 Belege oder unter 60 %) werden
    mit ihren Stellen gemeldet: ob ihr Vorschub im Text wirkt (ein Zeichen folgt in derselben
    Zeile ohne Leerzeichen) und ob ihre Linkslage wirkt (sie folgt einem Zeichen ohne
    Leerzeichen oder beginnt eine Zeile)."""
    from collections import Counter as _C
    zaehl = _C(c for c in text + titel if c not in " \n\r")
    fehlt = sorted(c for c in zaehl if c not in font["glyphen"])
    konstr = sorted(c for c in zaehl if c in font["glyphen"]
                    and font["glyphen"][c]["quelle"].startswith("KONSTRUKTION"))
    L = ["Zeichenpruefung vor dem Satz (re2_doc_satz.py satz)",
         "Text %d Zeichen, Titel %r; verschiedene Zeichen ausser Leerraum: %d"
         % (len(text), titel, len(zaehl)), ""]
    L.append("%-5s %6s %7s %8s %6s %6s %9s  %s" % ("Zeich", "Text", "Orig-n", "Vorschub",
                                                   "links", "breit", "gesichert", "Quelle"))
    for c in sorted(zaehl):
        g = font["glyphen"].get(c)
        if g is None:
            L.append("%-5r %6d  FEHLT" % (c, zaehl[c]))
            continue
        L.append("%-5r %6d %7d %8d %6d %6d %9s  %s" % (c, zaehl[c], g["n"], g["vorschub"],
                                                       g["links"], g["breite"],
                                                       "ja" if g.get("gesichert") else "NEIN",
                                                       g["quelle"]))
    L.append("")
    L.append("Nicht gesicherte Metrik - wirkt sie im Text?")
    zeilen = [z for z in (text + "\n" + titel).replace("\r", "").split("\n")]
    for c in sorted(zaehl):
        g = font["glyphen"].get(c)
        if g is None or g.get("gesichert"):
            continue
        vor = links = 0
        stellen = []
        for z in zeilen:
            for i, ch in enumerate(z):
                if ch != c:
                    continue
                folgt = i + 1 < len(z) and z[i + 1] != " "
                vor += folgt
                links += 1                  # Linkslage wirkt IMMER (Stift + links = Kern-x)
                stellen.append(z[max(0, i - 12):i + 4])
        L.append("  %r: %d Stellen; Vorschub wirkt an %d (%s); Linkslage %d aus %s; %s"
                 % (c, len(stellen), vor, "folgenlos" if vor == 0 else "WIRKT",
                    g["links"], g["beleg_links"], stellen))
    L.append("")
    L.append("FEHLENDE Glyphen: %d %r" % (len(fehlt), fehlt))
    L.append("KONSTRUKTIONEN im Text: %d %r%s" % (len(konstr), konstr,
             " (erlaubt per --konstruktion-erlaubt)" if konstr and konstruktion_erlaubt else ""))
    open(os.path.join(out_dir, "zeichen_pruefung.txt"), "w", encoding="utf-8").write(
        "\n".join(L) + "\n")
    if fehlt:
        raise SystemExit("Zeichen ohne Glyphe: %r (Bericht %s)"
                         % (fehlt, os.path.join(out_dir, "zeichen_pruefung.txt")))
    if konstr and not konstruktion_erlaubt:
        raise SystemExit("Zeichen mit KONSTRUIERTER Glyphe: %r - nur mit --konstruktion-erlaubt"
                         % konstr)
    return zaehl


def satz(args):
    font = json.load(open(args.font, encoding="utf-8"))
    vor_seite = tim_lesen(os.path.join(FILES, "FILE%02d_p01_page.TIM" % args.vorlage))
    vor_titel = tim_lesen(os.path.join(FILES, "FILE%02d_title_page.TIM" % args.vorlage))
    papier = tim_lesen(os.path.join(FILES, "FILE%02d_title_paper.TIM" % args.vorlage))
    W, H = vor_seite["W"], vor_seite["H"]
    nz = H // ZEILE
    text = open(args.text, encoding="utf-8").read()
    os.makedirs(args.out, exist_ok=True)
    zeichen_pruefen(font, text, args.titel, args.out, args.konstruktion_erlaubt)
    open(os.path.join(args.out, "satz_eingabe.txt"), "w", encoding="utf-8",
         newline="").write(text)                  # woertliche Kopie der Eingabe (Abnahme [1])

    seiten = []                                   # je Seite eine Liste von hoechstens nz Zeilen
    for kopf, absaetze in eintraege(text):
        zl = [kopf]
        for a in absaetze:
            if a == "":
                zl.append("")
            else:
                zl += umbrechen(font, a, args.rand, args.xmax)
        while zl:
            seite = zl[:nz]
            zl = zl[nz:]
            while zl and zl[0] == "":
                zl.pop(0)                         # keine Leerzeile am Kopf einer Folgeseite
            seiten.append(seite)

    os.makedirs(args.out, exist_ok=True)
    farbe = [((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
             for c in vor_seite["clut"]]
    try:
        from PIL import Image
    except ImportError:
        Image = None

    def png(px, name):
        if Image is None:
            return
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
        # Vorschau im RE2-Schirmlayout: Bild bei (100,60), 128 x (256-H), abgetastet ab v=H
        # (@0x80072584-90, @0x800760b8-dc); darueber die Textseite bei (25,30)
        # (DAT_800d5c4c/4e = 0x19/0x1e am Ende von FUN_80075fd0).
        s = Image.new("RGB", (320, 240), (0, 0, 0))
        for v in range(256 - h):
            for u in range(128):
                c = papier["clut"][papier["px"][h + v][u]]
                if c != 0:                                   # PSX: Texel 0x0000 = durchsichtig
                    s.putpixel((100 + u, 60 + v),
                               ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3))
        for y in range(h):
            for x in range(W):
                v = px[y][x]
                if v and 0 <= 25 + x < 320 and 0 <= 30 + y < 240:
                    s.putpixel((25 + x, 30 + y), farbe[v])
        s.save(os.path.join(args.out, name + "_schirm.png"))

    bericht = []
    # Titelseite: eine Zeile, waagerecht mittig, auf der Rasterzeile der Vorlage
    tb = baender(vor_titel)
    t_zeile = (tb[0][0] // ZEILE) if tb else (nz // 2)
    a, b = breite_von(font, args.titel, 0)
    rand_t = (W - (b - a + 1)) // 2 - a
    px = [[0] * W for _ in range(H)]
    zeile_setzen(font, px, args.titel, rand_t, t_zeile * ZEILE)
    px = kontur_ziehen(px)
    x0, x1 = breite_von(font, args.titel, rand_t)
    for name in ("FILE%02d_title_page" % args.doc, "FILE%02d_p00_page" % args.doc):
        n = tim_schreiben_4bpp(os.path.join(args.out, name + ".TIM"), vor_titel, px)
        png(px, name)
        bericht.append("%s.TIM  %d B  Titel %r  Rasterzeile %d  x %d..%d (Mitte %.1f)"
                       % (name, n, args.titel, t_zeile, x0, x1, (x0 + x1) / 2.0))
    # Illustration: byte-gleiche Kopie der Vorlage
    src = os.path.join(FILES, "FILE%02d_title_paper.TIM" % args.vorlage)
    dst = os.path.join(args.out, "FILE%02d_title_paper.TIM" % args.doc)
    open(dst, "wb").write(open(src, "rb").read())
    bericht.append("FILE%02d_title_paper.TIM  %d B  = byte-gleiche Kopie von "
                   "FILE%02d_title_paper.TIM" % (args.doc, os.path.getsize(dst), args.vorlage))
    # Textseiten
    for i, seite in enumerate(seiten):
        px = [[0] * W for _ in range(H)]
        xm = 0
        for k, z in enumerate(seite):
            if z:
                zeile_setzen(font, px, z, args.rand, k * ZEILE)
                xm = max(xm, breite_von(font, z, args.rand)[1])
        px = kontur_ziehen(px)
        name = "FILE%02d_p%02d_page" % (args.doc, i + 1)
        n = tim_schreiben_4bpp(os.path.join(args.out, name + ".TIM"), vor_seite, px)
        png(px, name)
        bericht.append("%s.TIM  %d B  %d Zeilen  groesstes x1 = %d" % (name, n, len(seite), xm))
        for z in seite:
            bericht.append("      | " + z)
    bericht.append("")
    bericht.append("max_page (RE2-Record +0, u16) = %d   y_off (+2, u8) = %d   H = %d"
                   % (len(seiten), 256 - H, H))
    open(os.path.join(args.out, "FILE%02d_satz.txt" % args.doc), "w", encoding="utf-8").write(
        "\n".join(bericht) + "\n")
    print("satz: Titel + %d Textseiten -> %s" % (len(seiten), args.out))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    a = sub.add_parser("atlas")
    a.add_argument("--out", required=True)
    s = sub.add_parser("satz")
    s.add_argument("--out", required=True)
    s.add_argument("--font", required=True)
    s.add_argument("--text", required=True)
    s.add_argument("--titel", required=True)
    s.add_argument("--doc", type=int, default=25)
    s.add_argument("--vorlage", type=int, default=8)
    s.add_argument("--rand", type=int, default=10,
                   help="Stiftlage am Zeilenanfang; FILE08 p02/p03 gemessen 10, p01 12")
    s.add_argument("--konstruktion-erlaubt", action="store_true",
                   help="konstruierte Glyphen (Umlaute, Eszett) zulassen; sonst Abbruch")
    s.add_argument("--xmax", type=int, default=249,
                   help="groesstes erlaubtes Kernpixel-x; FILE08 gemessen 243/246/249")
    args = ap.parse_args()
    if args.cmd == "atlas":
        atlas_bauen(args.out)
    else:
        satz(args)


if __name__ == "__main__":
    main()
