#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tuer_kontaktbogen.py - Kontaktbogen je abgedeckter Tuerseite (Runde 31, Stufe 4, Abschnitt 6).

Je Seite eine Zeile: RE1.5-Ausschnitt der Seite (T1: build/r31_tueren/t1/re15_seiten/<S>_*_aus.png,
die Kamera mit dem groessten Umriss laut zensus.json) | Sequenzbild Anfang (Tuer zu, Griff sichtbar)
| Sequenzbild Mitte (Oeffnen) - beide aus der ECHTEN exe (Pruefhaken RE15_TUER_SEITE +
RE15_TUER_BOGEN, beschleunigter Renderer, RE15_FRAMEDUMP-freier Rueckleser vor dem Present),
dazu die Wahl (Archiv, Variante, Griff-Tausch, Tonfamilie) aus zuordnung.json.

Ablauf:
    1. Bilder erzeugen (echte exe; ohne Fenster-Vorlauf, der Haken spielt direkt nach dem Start):
       python re15_port/tools/tueren/tuer_kontaktbogen.py --erzeugen
       (= RE15_TUER_SEITE=ALLE RE15_TUER_BOGEN=build/r31_tueren/t4/bilder RE15_TUER_SCHNELL=1 re15_pc.exe)
    2. Boegen bauen:
       python re15_port/tools/tueren/tuer_kontaktbogen.py
       -> build/r31_tueren/t4/kontaktbogen_NN.png, verkleinert analysis/befunde_runde31/tueren_belege/t4_kontaktbogen_NN.jpg
"""
import argparse
import glob
import json
import os
import subprocess
import sys

from PIL import Image, ImageDraw, ImageFont

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
ZUORDNUNG = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_03", "zuordnung.json")
ZENSUS = os.path.join(REPO, "build", "r31_tueren", "t1", "zensus.json")
SEITEN = os.path.join(REPO, "build", "r31_tueren", "t1", "re15_seiten")
AUS = os.path.join(REPO, "build", "r31_tueren", "t4")
BILDER = os.path.join(AUS, "bilder")
BELEGE = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_belege")
EXE = os.path.join(PORT, "build", "platform", "pc", "re15_pc.exe")

ZELLE = (240, 180)
ZEILEN_JE_BOGEN = 12
TEXT_B = 250


def seiten_lesen():
    d = json.load(open(ZUORDNUNG, encoding="utf-8"))
    out = []
    for t in d["tueren"]:
        for s in t["seiten"]:
            if s.get("abgedeckt"):
                out.append((t, s))
    out.sort(key=lambda ts: int(ts[1]["id"][1:]))
    return out


def ausschnitt(sid, zensus):
    """RE1.5-Ausschnitt: ok-Kamera mit dem groessten Umriss, sonst die erste Datei."""
    best = None
    z = zensus.get(sid)
    if z:
        a = z.get("ausschnitte") or {}
        if isinstance(a, dict):
            for c in a.get("cuts", []):
                if c.get("ok") and c.get("px_h") and c.get("px_w"):
                    f = glob.glob(os.path.join(SEITEN, "%s_*_c%02d_aus.png" % (sid, c["cut"])))
                    if f and (best is None or c["px_h"] * c["px_w"] > best[0]):
                        best = (c["px_h"] * c["px_w"], f[0])
    if best:
        return best[1]
    f = sorted(glob.glob(os.path.join(SEITEN, "%s_*_aus.png" % sid)))
    return f[0] if f else None


def zelle(pfad):
    im = Image.new("RGB", ZELLE, (40, 0, 40))
    if pfad and os.path.exists(pfad):
        b = Image.open(pfad).convert("RGB")
        b.thumbnail(ZELLE)
        im.paste(b, ((ZELLE[0] - b.width) // 2, (ZELLE[1] - b.height) // 2))
    else:
        ImageDraw.Draw(im).text((8, 8), "fehlt", fill=(255, 80, 80))
    return im


def erzeugen():
    os.makedirs(BILDER, exist_ok=True)
    env = dict(os.environ)
    env.update(RE15_TUER_SEITE="ALLE", RE15_TUER_BOGEN=BILDER.replace("\\", "/"), RE15_TUER_SCHNELL="1")
    cwd = os.path.dirname(EXE)
    print("starte", EXE)
    r = subprocess.run([EXE], cwd=cwd, env=env, timeout=3600)
    print("exe beendet mit", r.returncode, "- Protokoll:", os.path.join(cwd, "debug.log"))


def boegen():
    zensus = {s["id"]: s for s in json.load(open(ZENSUS, encoding="utf-8"))["seiten"]}
    liste = seiten_lesen()
    os.makedirs(AUS, exist_ok=True)
    os.makedirs(BELEGE, exist_ok=True)
    try:
        font = ImageFont.truetype("arial.ttf", 13)
    except OSError:
        font = ImageFont.load_default()
    fehlend = []
    n_boegen = (len(liste) + ZEILEN_JE_BOGEN - 1) // ZEILEN_JE_BOGEN
    for b in range(n_boegen):
        teil = liste[b * ZEILEN_JE_BOGEN:(b + 1) * ZEILEN_JE_BOGEN]
        breite = TEXT_B + 3 * ZELLE[0] + 8
        bogen = Image.new("RGB", (breite, 22 + len(teil) * (ZELLE[1] + 4)), (16, 16, 16))
        dr = ImageDraw.Draw(bogen)
        dr.text((4, 4), "RE1.5-Ausschnitt | Sequenz Anfang | Sequenz Mitte   (Bogen %d/%d)" % (b + 1, n_boegen),
                fill=(255, 255, 255), font=font)
        for i, (t, s) in enumerate(teil):
            y = 22 + i * (ZELLE[1] + 4)
            sid = s["id"]
            anf = os.path.join(BILDER, "%s_anfang.ppm" % sid)
            mit = os.path.join(BILDER, "%s_mitte.ppm" % sid)
            if not (os.path.exists(anf) and os.path.exists(mit)):
                fehlend.append(sid)
            k = s["schluessel"]
            txt = ["%s  %s" % (sid, t["id"]), "%s -> %s" % (s["raum"], k.get("ziel")),
                   "%s V%d" % (s["archiv"], s["variante"]),
                   "Griff %s %s" % (s.get("griff_seite"), s.get("griff_form")),
                   "Tausch %s" % (s.get("griff_tausch") or "-"), "Ton %s" % s.get("tonfamilie"),
                   (s.get("variante_herkunft") or "")[:34]]
            for j, z in enumerate(txt):
                dr.text((4, y + 4 + j * 16), z, fill=(255, 230, 120) if j == 0 else (220, 220, 220), font=font)
            for c, p in enumerate((ausschnitt(sid, zensus), anf, mit)):
                bogen.paste(zelle(p), (TEXT_B + c * ZELLE[0], y))
        ziel = os.path.join(AUS, "kontaktbogen_%02d.png" % (b + 1))
        bogen.save(ziel)
        klein = bogen.resize((bogen.width * 2 // 3, bogen.height * 2 // 3), Image.LANCZOS)
        klein.save(os.path.join(BELEGE, "t4_kontaktbogen_%02d.jpg" % (b + 1)), quality=72)
        print("geschrieben:", ziel)
    print("Seiten %d, Boegen %d, ohne Sequenzbilder: %s" % (len(liste), n_boegen, " ".join(fehlend) or "keine"))
    return 0 if not fehlend else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--erzeugen", action="store_true", help="erst die Bilder mit der echten exe erzeugen")
    a = ap.parse_args()
    if a.erzeugen:
        erzeugen()
    sys.exit(boegen())


if __name__ == "__main__":
    main()
