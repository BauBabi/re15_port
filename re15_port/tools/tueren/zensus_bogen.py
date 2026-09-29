#!/usr/bin/env python3
"""zensus_bogen.py - RE2-Referenzblaetter, Texturuebersicht und Kontaktboegen je RE1.5-Raum
(R31 T1, Schritte 3 und 5).

Eingaben: build/r31_tueren/t1/{re15_seiten,re2_seiten}.json (zensus_ausschnitte.py),
          build/r31_tueren/t1/zensus.json (zensus_alles.py, Tueren/Seiten/Kandidaten).
Ausgaben: build/r31_tueren/t1/re2/DOORxx.png, re2/uebersicht.png, boegen/ROOMxxx0.png
"""
import json
import os
import re
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L     # noqa: E402
import zensus_mass as M    # noqa: E402

KATALOG_MD = os.path.join(L.REPO, "analysis", "tor_1170", "04_tuerkatalog.md")
GELB, WEISS, GRAU, ROT, GRUEN = (255, 230, 0), (235, 235, 235), (150, 150, 150), (255, 70, 70), (80, 230, 80)


def katalog():
    """{nr: dict(bewegung, textur)} aus der Katalogtabelle 04_tuerkatalog.md §2."""
    aus = {}
    for z in open(KATALOG_MD, encoding="utf-8"):
        m = re.match(r"\|\s*DOOR([0-9A-F]{2})\s*\|(.*)\|\s*$", z)
        if not m:
            continue
        sp = [s.strip() for s in m.group(2).split("|")]
        if len(sp) >= 6:
            aus[int(m.group(1), 16)] = dict(bewegung=sp[4], textur=sp[5])
    return aus


def _font(g=11):
    from PIL import ImageFont
    for f in ("C:/Windows/Fonts/consola.ttf", "C:/Windows/Fonts/arial.ttf"):
        if os.path.exists(f):
            return ImageFont.truetype(f, g)
    return ImageFont.load_default()


def _bild(pfad):
    from PIL import Image
    return Image.open(os.path.join(L.REPO, pfad)).convert("RGB")


def _auf_hoehe(im, h):
    from PIL import Image
    w = max(1, int(round(im.size[0] * h / float(im.size[1]))))
    return im.resize((w, h), Image.NEAREST)


def _umbruch(text, n):
    aus, zeile = [], ""
    for w in text.split():
        if len(zeile) + len(w) + 1 > n:
            aus.append(zeile)
            zeile = w
        else:
            zeile = (zeile + " " + w).strip()
    if zeile:
        aus.append(zeile)
    return aus


# ----------------------------------------------------------------------------
# RE2-Referenzblaetter
# ----------------------------------------------------------------------------
def re2_blaetter():
    from PIL import Image, ImageDraw
    tex = M.texturen()
    kat = katalog()
    seiten = json.load(open(os.path.join(L.AUS, "re2_seiten.json")))
    ausdir = os.path.join(L.AUS, "re2")
    os.makedirs(ausdir, exist_ok=True)
    f, fk = _font(13), _font(11)
    index = {}
    for nr in sorted(tex):
        t = tex[nr]
        kand = [v for v in seiten if v["archiv"] == nr and v.get("gewaehlt")]
        # hoechstens 3 Ausschnitte, verschiedene Raeume zuerst; Rang = Pixelhoehe x Frontalitaet
        # (Breite/Hoehe im Bild gegen 1640/3549, hoechstens 1)
        def rang(v):
            g = v["gewaehlt"][0]
            q = np.array(g["quad"], float)
            pw = 0.5 * (np.linalg.norm(q[1] - q[0]) + np.linalg.norm(q[2] - q[3]))
            return -g["px_h"] * min(1.0, pw / max(1.0, g["px_h"]) / (L.BLATT_B_RE2 / L.BLATT_H))
        kand.sort(key=rang)
        wahl, raeume = [], set()
        for v in kand:
            if v["raum"] not in raeume:
                wahl.append(v)
                raeume.add(v["raum"])
            if len(wahl) == 3:
                break
        for v in kand:
            if len(wahl) == 3:
                break
            if v not in wahl:
                wahl.append(v)
        B = Image.new("RGB", (1320, 640), (24, 24, 28))
        dr = ImageDraw.Draw(B)
        k = kat.get(nr, {})
        dr.text((10, 6), "DOOR%02X  (%s)  %s" % (nr, t["pfad"], "Textur wie DOOR%02X" % t["gleich_wie"] if t["gleich_wie"] != nr else ""),
                fill=GELB, font=f)
        for i, zl in enumerate(_umbruch("Textur: " + k.get("textur", "?"), 150)[:2] + _umbruch("Bewegung: " + k.get("bewegung", "?"), 150)[:1]):
            dr.text((10, 24 + 14 * i), zl, fill=WEISS, font=fk)
        # Textur 2-fach, Linie bei v = 218 (Blattbereich u 0..126, v 0..217)
        rgba = t["rgba"]
        tim = Image.fromarray(rgba[..., :3]).resize((256, 512), Image.NEAREST)
        B.paste(tim, (10, 76))
        dr.rectangle([10, 76, 10 + 254, 76 + 436], outline=GRUEN)
        dr.line([(10, 76 + 438), (266, 76 + 438)], fill=ROT)
        dr.text((10, 592), "Blatt u0..126 v0..217 (gruen), Beschlag v219..255", fill=GRAU, font=fk)
        besch = Image.fromarray(rgba[219:256, :, :3]).resize((512, 148), Image.NEAREST)
        B.paste(besch, (280, 76))
        dr.text((280, 228), "Beschlagstreifen v219..255 (4-fach)", fill=GRAU, font=fk)
        x = 280
        if not wahl:
            dr.text((280, 260), "kein Door_aot_set mit diesem Archiv in einem sichtbaren Leon-Raum", fill=ROT, font=f)
        for v in wahl:
            g = v["gewaehlt"][0]
            aus = _auf_hoehe(_bild(g["ausschnitt"]), 300)
            if aus.size[0] > 190:
                aus = aus.resize((190, int(300 * 190 / aus.size[0])), Image.NEAREST)
            ent = _bild(g["entzerrt"])
            B.paste(aus, (x, 250))
            B.paste(ent, (x + aus.size[0] + 6, 250))
            dr.text((x, 556), "ROOM%s c%d  var %s" % (v["raum"], g["cut"], ",".join(str(a) for a in v["varianten"])),
                    fill=WEISS, font=fk)
            dr.text((x, 570), "Band %d  -> %s  %s" % (v["band"], v["ziel_basis"], v["id"]), fill=GRAU, font=fk)
            x += aus.size[0] + 6 + 128 + 14
        ziel = os.path.join(ausdir, "DOOR%02X.png" % nr)
        B.save(ziel)
        index[nr] = dict(blatt=L.rel(ziel), ausschnitte=[v["id"] for v in wahl],
                         re2_seiten=len(kand), gleich_wie=t["gleich_wie"])
    # Uebersicht aller 55 Texturen
    spalten = 11
    zeilen = (len(tex) + spalten - 1) // spalten
    U = Image.new("RGB", (spalten * 140 + 10, zeilen * 290 + 10), (24, 24, 28))
    dr = ImageDraw.Draw(U)
    for i, nr in enumerate(sorted(tex)):
        x, y = 10 + (i % spalten) * 140, 10 + (i // spalten) * 290
        U.paste(Image.fromarray(tex[nr]["rgba"][..., :3]), (x, y + 18))
        dr.text((x, y + 2), "DOOR%02X%s" % (nr, " =%02X" % tex[nr]["gleich_wie"] if tex[nr]["gleich_wie"] != nr else ""),
                fill=GELB, font=f)
    U.save(os.path.join(ausdir, "uebersicht.png"))
    return index


# ----------------------------------------------------------------------------
# Kontaktboegen je RE1.5-Raum
# ----------------------------------------------------------------------------
def kontaktboegen(Z):
    """Z = zensus.json-Inhalt. Je Grundraum ein Bogen: jede begehbare Tuerseite eine Zeile."""
    from PIL import Image, ImageDraw
    tex = M.texturen()
    seiten = {s["id"]: s for s in Z["seiten"]}
    tueren = {t["id"]: t for t in Z["tueren"]}
    ausdir = os.path.join(L.AUS, "boegen")
    os.makedirs(ausdir, exist_ok=True)
    f, fk = _font(13), _font(11)
    raeume = {}
    for s in Z["seiten"]:
        raeume.setdefault(s["basis"], []).append(s)
    erg = {}
    ZH = 250
    for basis in sorted(raeume):
        ss = [s for s in raeume[basis] if not s["nullflaeche"]]
        rest = [s for s in raeume[basis] if s["nullflaeche"]]
        if not ss:
            continue
        H = 40 + ZH * len(ss) + 16 * len(rest) + 10
        B = Image.new("RGB", (1720, H), (24, 24, 28))
        dr = ImageDraw.Draw(B)
        dr.text((10, 8), "ROOM%s0  %s   -  %d begehbare Tuerseite(n)   [rot = Blatt verfeinert, gelb = Datenlage]" % (
            basis, ss[0].get("raumname", ""), len(ss)), fill=GELB, font=f)
        for i, s in enumerate(ss):
            y = 36 + i * ZH
            dr.line([(0, y - 3), (1720, y - 3)], fill=(60, 60, 70))
            t = tueren.get(s.get("tuer"), {})
            kopf = [
                "%s  %s" % (s["id"], s.get("tuer", "")),
                "-> ROOM%s0 %s" % (s["ziel_basis"], s.get("zielname", "")),
                "Band %d  %s  Kante %s%s" % (s["band"], s["form"], s.get("kantenlaenge", "?"),
                                              " BREIT" if (s.get("kantenlaenge") or 0) >= 3000 else ""),
                "Mitte (%d, %d)" % (s["mitte"][0], s["mitte"][1]),
                "Kat.: %s%s" % (t.get("kategorie", "?"), "  INERT" if t.get("inert") else ""),
                "Engine: %s" % s.get("engine"),
                "Gegenseite: %s" % (s.get("gegenseite") or "-"),
                "Zwilling von %s" % s["zwilling_von"] if s.get("zwilling_von") else "",
            ]
            for j, zl in enumerate(kopf):
                dr.text((8, y + 2 + 14 * j), zl, fill=WEISS if j else GELB, font=fk)
            a = s.get("ausschnitte") or {}
            gw = a.get("gewaehlt") or []
            x = 190
            if not gw:
                dr.text((x, y + 60), "in keinem Cut sichtbar", fill=ROT, font=f)
            else:
                g = gw[0]
                aus = _auf_hoehe(_bild(g["ausschnitt"]), 228)
                if aus.size[0] > 260:
                    aus = aus.resize((260, int(228 * 260 / aus.size[0])), Image.NEAREST)
                B.paste(aus, (x, y + 4))
                dr.text((x, y + 234), "c%d%s  %.0f px" % (g["cut"], " aktiv" if g.get("aktiv") else "", g["px_h"]),
                        fill=GRAU, font=fk)
                x += 266
                voll = _bild(g["vollbild"])
                B.paste(voll.resize((240, 180), Image.BILINEAR), (x, y + 4))
                dr.text((x, y + 188), "Vollbild c%d" % g["cut"], fill=GRAU, font=fk)
                if len(gw) > 1:
                    dr.text((x, y + 204), "weitere Cuts: %s" % ", ".join("c%d" % q["cut"] for q in gw[1:]),
                            fill=GRAU, font=fk)
                x += 246
                B.paste(_bild(g["entzerrt"]), (x, y + 4))
                dr.text((x, y + 226), "entzerrt", fill=GRAU, font=fk)
                x += 134
            # Gegenseite (andere Raumseite derselben Tuer)
            gs = seiten.get(s.get("gegenseite") or "")
            if gs and (gs.get("ausschnitte") or {}).get("gewaehlt"):
                gg = gs["ausschnitte"]["gewaehlt"][0]
                aus = _auf_hoehe(_bild(gg["ausschnitt"]), 150)
                if aus.size[0] > 150:
                    aus = aus.resize((150, int(150 * 150 / aus.size[0])), Image.NEAREST)
                B.paste(aus, (x, y + 4))
                dr.text((x, y + 158), "Gegenseite %s" % gs["id"], fill=GRAU, font=fk)
                dr.text((x, y + 172), "ROOM%s0 c%d" % (gs["basis"], gg["cut"]), fill=GRAU, font=fk)
            x += 156
            # Top-5 RE2
            for k, c in enumerate((s.get("kandidaten") or [])[:5]):
                nr = c["archiv"]
                im = Image.fromarray(tex[nr]["blatt"])
                if c.get("gespiegelt"):
                    im = im.transpose(Image.FLIP_LEFT_RIGHT)
                im = im.resize((96, 164), Image.BILINEAR)
                B.paste(im, (x + k * 102, y + 4))
                dr.text((x + k * 102, y + 172), "%d. DOOR%02X" % (k + 1, nr), fill=GELB, font=fk)
                dr.text((x + k * 102, y + 186), "%.3f%s" % (c["wert"], " sp" if c.get("gespiegelt") else ""),
                        fill=WEISS, font=fk)
        y = 36 + len(ss) * ZH
        for s in rest:
            dr.text((8, y), "%s  Flaeche 0 (Skript-Uebergang) -> ROOM%s0 %s  Band %d  %s" % (
                s["id"], s["ziel_basis"], s.get("zielname", ""), s["band"], s.get("tuer", "")), fill=GRAU, font=fk)
            y += 16
        ziel = os.path.join(ausdir, "ROOM%s0.png" % basis)
        B.save(ziel)
        erg[basis] = dict(bogen=L.rel(ziel), seiten=len(ss))
    return erg
