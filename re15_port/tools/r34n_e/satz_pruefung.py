# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Abnahme des Satzes FILE26..29 AM ARTEFAKT (die geschriebenen TIM).

Vorbild analysis/befunde_runde30/r30_diary_satz_pruefung.py (Irons Diary): jede Textseite wird Zeile
fuer Zeile per Glyphenvergleich (Kern-Indizes 1..6) zurueckgelesen - dieselbe Funktion `ruecklesen`,
hier importiert - und gegen den Nutzertext geprueft:
  [1] Eingabe des Satzes (satz_eingabe.txt) zeichengleich dem Nutzertext (E_texte/dok*.txt)
  [2] Kernpixel ohne Glyphe = 0, Abstaende ausserhalb der Metrik = 0
  [3] Wortfolge zurueckgelesen == Wortfolge des Nutzertexts (jedes Wort, Gross/klein, Satzzeichen)
  [4] ZEILENFOLGE: jede Nutzerzeile beginnt eine Satzzeile (Anfangswort der Nutzerzeilen steht am
      Zeilenanfang einer Seite), Leerzeilen innerhalb einer Seite erhalten
  [5] Titelseite zurueckgelesen == Titel in Versalien (je Rasterband), title_page == p00,
      title_paper byte-gleich der Vorlage, Kopf/CLUT/Bildkopf jeder Seite == Vorlage p01
  [6] Dateibestand p00..pN lueckenlos, KEIN p(N+1); N == max_page aus FILEnn_satz.txt
  [7] konstruierte Glyphen zurueckgelesen: 0

Aufruf: python re15_port/tools/r34n_e/satz_pruefung.py [satz-wurzel]   (Default build/r34n_e/satz)
"""
import hashlib
import json
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import re2_doc_satz as R  # noqa: E402
import struct  # noqa: E402

ZEILE = 16


# ---- tim() und ruecklesen() WOERTLICH aus analysis/befunde_runde30/r30_diary_satz_pruefung.py
#      (dort ohne __main__-Schutz, deshalb nicht importierbar) --------------------------------
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




class _P30:
    tim = staticmethod(tim)
    ruecklesen = staticmethod(ruecklesen)


P30 = _P30

TEXTE = os.path.join(REPO, "analysis", "befunde_runde34_nacht", "E_texte")
DOKS = [(26, 0, "dok1_police_officer"), (27, 8, "dok2_elliot"), (28, 2, "dok3_marvin"), (29, 6, "dok4_armory")]


def banden(t):
    """Rasterbaender der Titelseite (y-Bereiche mit Kernpixeln), wie re2_doc_satz.baender."""
    z = [any(1 <= v <= 6 for v in r) for r in t["px"]]
    out, y = [], 0
    while y < t["H"]:
        if z[y]:
            a = y
            while y < t["H"] and z[y]:
                y += 1
            out.append((a, y - 1))
        else:
            y += 1
    return out


def pruefe(wurzel, doc, vorlage, name):
    d = os.path.join(wurzel, "FILE%02d" % doc)
    L = []
    ok = True
    soll = open(os.path.join(TEXTE, name + ".txt"), encoding="utf-8").read().replace("\r", "")
    titel = open(os.path.join(TEXTE, name + "_titel.txt"), encoding="utf-8").read().strip()
    ist = open(os.path.join(d, "satz_eingabe.txt"), encoding="utf-8").read().replace("\r", "")
    gleich = soll == ist
    ok &= gleich
    L.append("[1] Nutzertext %s (%d Zeichen, md5 %s) gegen satz_eingabe.txt: %s"
             % (name, len(soll), hashlib.md5(soll.encode()).hexdigest()[:8], "ZEICHENGLEICH" if gleich else "ABWEICHEND"))
    font = json.load(open(os.path.join(REPO, "build", "r34n_e", "atlas", "re2_doc_font.json"), encoding="utf-8"))
    konstr = set(c for c, g in font["glyphen"].items() if g["quelle"].startswith("KONSTRUKTION"))
    vor = P30.tim(os.path.join(R.FILES, "FILE%02d_p01_page.TIM" % vorlage))
    seiten = []
    n = 1
    while os.path.exists(os.path.join(d, "FILE%02d_p%02d_page.TIM" % (doc, n))):
        seiten.append(n)
        n += 1
    woerter, reste, marken, fmt, zeilen_alle, konstr_n = [], 0, 0, 0, [], 0
    for s in seiten:
        t = P30.tim(os.path.join(d, "FILE%02d_p%02d_page.TIM" % (doc, s)))
        if len(t["roh"]) != len(vor["roh"]) or t["kopf"] != vor["kopf"]:
            fmt += 1
        zl = []
        for k in range(t["H"] // 16):
            txt, rest = P30.ruecklesen(font, t["px"], k * 16)
            reste += rest
            zl.append(txt)
        while zl and zl[-1] == "":
            zl.pop()
        for z in zl:
            woerter += z.split()
            marken += sum(1 for w in z.split() if "<" in w)
            konstr_n += sum(1 for c in z if c in konstr)
        zeilen_alle.append(zl)
    ok &= (reste == 0 and marken == 0)
    L.append("[2] %d Textseiten zurueckgelesen: Kernpixel ohne Glyphe %d, Abstaende ausserhalb der Metrik %d"
             % (len(seiten), reste, marken))
    sw = soll.split()
    wg = (sw == [w.strip() for w in woerter])
    ok &= wg
    L.append("[3] Wortfolge: Nutzertext %d Woerter, zurueckgelesen %d: %s" % (len(sw), len(woerter), "WORTGLEICH" if wg else "ABWEICHEND"))
    if not wg:
        for i, (a, b) in enumerate(zip(sw, woerter)):
            if a != b:
                L.append("    erste Abweichung Wort %d: Soll %r Ist %r" % (i, a, b))
                break
    # [4] Zeilenanfaenge: jede nichtleere Nutzerzeile muss mit dem Anfang einer Satzzeile beginnen
    anfaenge = [z.strip() for zl in zeilen_alle for z in zl if z.strip()]
    fehl = 0
    for u in [z for z in soll.split("\n") if z.strip()]:
        if not any(a.startswith(u.split()[0]) and u.startswith(a[:len(u)] if len(a) > len(u) else a.split()[0])
                   for a in anfaenge):
            fehl += 1
    ok &= (fehl == 0)
    L.append("[4] Nutzerzeilen, die NICHT am Anfang einer Satzzeile beginnen: %d" % fehl)
    for s, zl in zip(seiten, zeilen_alle):
        L.append("    p%02d: %s" % (s, " / ".join(zl)))
    # [5] Titel
    tt = P30.tim(os.path.join(d, "FILE%02d_title_page.TIM" % doc))
    tz = []
    for (y0, y1) in banden(tt):
        # Versalien liegen in Glyphenzeile +4..+11, der Apostroph reicht eine Zeile hoeher -
        # die Bandoberkante ist also y0-4 ODER y0-3: die Lage nehmen, die restlos liest.
        best = None
        for top in (y0 - 4, y0 - 3, y0 - 5):
            txt, rest = P30.ruecklesen(font, tt["px"], top)
            if txt and rest == 0 and (best is None or len(txt) > len(best)):
                best = txt
        tz.append(best or "")
    titel_ok = " ".join(tz) == titel.upper()
    tp = open(os.path.join(d, "FILE%02d_title_page.TIM" % doc), "rb").read()
    p0 = open(os.path.join(d, "FILE%02d_p00_page.TIM" % doc), "rb").read()
    pap = open(os.path.join(d, "FILE%02d_title_paper.TIM" % doc), "rb").read()
    vpap = open(os.path.join(R.FILES, "FILE%02d_title_paper.TIM" % vorlage), "rb").read()
    ok &= titel_ok and tp == p0 and pap == vpap and fmt == 0
    L.append("[5] Titelseite zurueckgelesen %r (Soll %r): %s; title_page == p00: %s; title_paper == FILE%02d: %s; "
             "Seiten mit abweichendem Kopf: %d" % (tz, titel.upper(), "GLEICH" if titel_ok else "ABWEICHEND",
                                                 tp == p0, vorlage, pap == vpap, fmt))
    # [6] Dateibestand / max_page
    satz = open(os.path.join(d, "FILE%02d_satz.txt" % doc), encoding="utf-8").read()
    mp = int(re.search(r"max_page \(RE2-Record \+0, u16\) = (\d+)", satz).group(1))
    ok &= (mp == len(seiten)) and not os.path.exists(os.path.join(d, "FILE%02d_p%02d_page.TIM" % (doc, len(seiten) + 1)))
    L.append("[6] Dateibestand p00..p%02d, kein p%02d; max_page laut Satz %d: %s"
             % (len(seiten), len(seiten) + 1, mp, "STIMMT" if mp == len(seiten) else "FALSCH"))
    ok &= (konstr_n == 0)
    L.append("[7] konstruierte Glyphen zurueckgelesen: %d" % konstr_n)
    L.append("ERGEBNIS FILE%02d: %s" % (doc, "ALLE PRUEFUNGEN BESTANDEN" if ok else "FEHLER"))
    return ok, L


def main():
    wurzel = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "build", "r34n_e", "satz")
    alles = True
    aus = []
    for doc, vorlage, name in DOKS:
        ok, L = pruefe(wurzel, doc, vorlage, name)
        alles &= ok
        aus += L + [""]
    aus.append("GESAMT: %s" % ("ALLE VIER BESTANDEN" if alles else "FEHLER"))
    print("\n".join(aus))
    open(os.path.join(wurzel, "satz_pruefung.txt"), "w", encoding="utf-8").write("\n".join(aus) + "\n")


if __name__ == "__main__":
    main()
