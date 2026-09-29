#!/usr/bin/env python3
"""tuer_rest_bogen.py - Kontaktbogen der Runde 33 (Port-Archive): je gebauter Seite
RE1.5-Ausschnitt | Sequenz Anfang | Sequenz Mitte + Wahl (Port-Archiv, Basis, Variante, gemalter Griff).

Die Sequenzbilder kommen aus der ECHTEN exe (Pruefhaken RE15_TUER_SEITE + RE15_TUER_BOGEN,
beschleunigter Renderer, Rueckleser vor dem Present; kein AUTOSHOT/SOFTWARE_RENDER) - wie
tuer_kontaktbogen.py der Runde 31. Die exe laeuft als KOPIE unter eigenem Namen (re15_pc_r33t.exe im
selben Ordner, damit DLLs/Assets gleich aufgeloest werden): andere Agenten beenden re15_pc.exe per Namen.

    python re15_port/tools/tueren/tuer_rest_bogen.py --erzeugen   # Bilder (build/r33_tueren/bogen/bilder)
    python re15_port/tools/tueren/tuer_rest_bogen.py              # Boegen + verkleinerte jpg
"""
import argparse
import glob
import json
import os
import shutil
import subprocess
import sys

from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
PLAN = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest", "plan.json")
ARCHIVE = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest", "archive.json")
AUS = os.path.join(REPO, "build", "r33_tueren", "bogen")
BILDER = os.path.join(AUS, "bilder")
BELEGE = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest_belege")
EXE = os.path.join(PORT, "build", "platform", "pc", "re15_pc.exe")
KOPIE = os.path.join(PORT, "build", "platform", "pc", "re15_pc_r33t.exe")
ZELLE = (240, 180)
TEXT_B = 300
JE_BOGEN = 13


def t1_dir():
    for k in (os.path.join(REPO, "build", "r31_tueren", "t1"),
              os.path.join(REPO, "..", "..", "..", "build", "r31_tueren", "t1")):
        if os.path.isdir(k):
            return os.path.abspath(k)
    raise SystemExit("build/r31_tueren/t1 fehlt")


def seiten():
    plan = json.load(open(PLAN, encoding="utf-8"))
    gebaut = {e["kennung"]: e for e in json.load(open(ARCHIVE, encoding="utf-8"))["archive"]}
    out = []
    for tid, e in plan["tueren"].items():
        if e["archiv"] not in gebaut:
            continue
        for s in e["seiten"]:
            if s.get("bau") and "variante" in s:
                out.append(dict(tuer=tid, gruppe=e["gruppe"], archiv=e["archiv"], basis=gebaut[e["archiv"]]["basis"], **s))
    out.sort(key=lambda s: (s["gruppe"], int(s["id"][1:])))
    return out


def ausschnitt(sid, zensus):
    t1 = t1_dir()
    best = None
    z = zensus.get(sid)
    if z:
        a = z.get("ausschnitte") or {}
        if isinstance(a, dict):
            for c in a.get("cuts", []):
                if c.get("ok") and c.get("px_h") and c.get("px_w"):
                    f = glob.glob(os.path.join(t1, "re15_seiten", "%s_*_c%02d_aus.png" % (sid, c["cut"])))
                    if f and (best is None or c["px_h"] * c["px_w"] > best[0]):
                        best = (c["px_h"] * c["px_w"], f[0])
    if best:
        return best[1]
    f = sorted(glob.glob(os.path.join(t1, "re15_seiten", "%s_*_aus.png" % sid)))
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


def erzeugen(liste):
    os.makedirs(BILDER, exist_ok=True)
    shutil.copy2(EXE, KOPIE)
    env = dict(os.environ)
    env.update(RE15_TUER_SEITE=",".join(s["id"] for s in liste), RE15_TUER_BOGEN=BILDER.replace("\\", "/"),
               RE15_TUER_SCHNELL="1", RE15_NOAUDIO="1")
    log = os.path.join(AUS, "lauf.log")
    with open(log, "w") as f:
        r = subprocess.run([KOPIE], cwd=os.path.dirname(KOPIE), env=env, timeout=1800, stderr=f, stdout=f)
    print("exe (Kopie) beendet mit", r.returncode, "- Protokoll", log)


def boegen(liste):
    zensus = {s["id"]: s for s in json.load(open(os.path.join(t1_dir(), "zensus.json"), encoding="utf-8"))["seiten"]}
    os.makedirs(AUS, exist_ok=True)
    os.makedirs(BELEGE, exist_ok=True)
    namen = []
    for b in range(0, len(liste), JE_BOGEN):
        teil = liste[b:b + JE_BOGEN]
        W = 3 * ZELLE[0] + TEXT_B + 8
        H = len(teil) * (ZELLE[1] + 4) + 24
        bogen = Image.new("RGB", (W, H), (16, 16, 16))
        d = ImageDraw.Draw(bogen)
        d.text((4, 4), "Runde 33 Port-Archive: RE1.5-Ausschnitt | Sequenz Anfang | Sequenz Mitte (echte exe)",
               fill=(255, 255, 0))
        for i, s in enumerate(teil):
            y = 24 + i * (ZELLE[1] + 4)
            bogen.paste(zelle(ausschnitt(s["id"], zensus)), (0, y))
            for k, art in enumerate(("anfang", "mitte")):
                p = os.path.join(BILDER, "%s_%s.ppm" % (s["id"], art))
                bogen.paste(zelle(p), ((k + 1) * ZELLE[0], y))
            x = 3 * ZELLE[0] + 6
            d.text((x, y + 4), "%s %s %s" % (s["id"], s["tuer"], s["raum"]), fill=(255, 255, 255))
            d.text((x, y + 22), "%s = %s V%d" % (s["archiv"], s["basis"], s["variante"]), fill=(120, 255, 120))
            d.text((x, y + 40), "Griff gemalt: %s" % s.get("griff", "-"), fill=(200, 200, 200))
            h = s["herkunft"]
            for j in range(0, min(len(h), 120), 40):
                d.text((x, y + 58 + j // 40 * 16), h[j:j + 40], fill=(160, 160, 160))
        name = os.path.join(AUS, "bogen_%02d.png" % (b // JE_BOGEN + 1))
        bogen.save(name)
        klein = bogen.copy()
        klein.thumbnail((1100, 3000))
        jpg = os.path.join(BELEGE, "pilot_kontaktbogen_%02d.jpg" % (b // JE_BOGEN + 1))
        klein.save(jpg, quality=85)
        namen.append((name, jpg))
    for n in namen:
        print(n[0], "->", n[1])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--erzeugen", action="store_true")
    ap.add_argument("--nur", default="")
    a = ap.parse_args()
    liste = seiten()
    if a.nur:
        liste = [s for s in liste if s["id"] in a.nur.split(",") or s["archiv"] in a.nur.split(",")]
    print("%d Seiten" % len(liste))
    if a.erzeugen:
        erzeugen(liste)
    boegen(liste)
    return 0


if __name__ == "__main__":
    sys.exit(main())
