#!/usr/bin/env python3
"""r30_karte3010_abnahme_auswertung.py - wertet die Framedump-Reihe der Abnahme aus
(Runde 30, Thema B, Bau S10). Liest build/r30_karte-3010_bau/<marke>_NNNNNN.ppm (960x720,
3-fach von 320x240) und <marke>.debug.log.

Je Bild:
  - Zielkachel (156,76) 48x40 (Schirm x3 = (468,228) 144x120): Mittelwert der INNEREN
    Flaeche (Rand 4 px abgezogen) -> Klasse "rot" (Rotanteil hoch) / "umriss";
  - Titelkachel (30,30) 88x32: Hash, gegen die Blatt-Titel aus dem Log-Zustand gruppiert;
  - Spielermarker: gibt es im Kartenbereich ein 8x8-Sprite der Marker-Farbe? (nur Bericht)
Gegen das Log: [hint]-Zeilen (Bild, Wanduhr, Phase) und [input-script]-Zeilen.
Ausgabe: Tabelle auf stdout + PNG-Abzuege ausgewaehlter Bilder.
"""
import os, re, sys, glob, hashlib
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
OUT = os.path.join(REPO, "build", "r30_karte-3010_bau")
MARKE = sys.argv[1] if len(sys.argv) > 1 else "lauf_a"
S = 3  # Skalierung 320 -> 960

def lade(f):
    im = Image.open(f).convert("RGB")
    return im

def mittel(im, x, y, w, h):
    px = im.crop((x * S, y * S, (x + w) * S, (y + h) * S)).getdata()
    n = len(px)
    return tuple(sum(p[i] for p in px) / n for i in range(3))

def main():
    log = open(os.path.join(OUT, MARKE + ".debug.log"), encoding="utf-8", errors="replace").read().splitlines()
    hint = []
    for l in log:
        m = re.match(r"\[hint\] F(\d+) (begin|t=(\d+) us schritt=(\d+) (rot|umriss)|schliessen)", l)
        if m:
            hint.append((int(m.group(1)), m.group(2), int(m.group(3) or 0), m.group(5)))
    beginn = next((h[0] for h in hint if h[1] == "begin"), None)
    zu = next((h[0] for h in hint if h[1] == "schliessen"), None)
    print("Log: Hinweis-Beginn F%s, Schliessen F%s" % (beginn, zu))
    wechsel = [h for h in hint if h[3]]
    # Phasendauern in Wanduhr (aus dem Log)
    ts = [h[2] for h in wechsel]
    d = [b - a for a, b in zip(ts, ts[1:])]
    rot_d = [d[i] for i in range(len(d)) if wechsel[i][3] == "rot"]
    um_d = [d[i] for i in range(len(d)) if wechsel[i][3] == "umriss"]
    tone = [h[2] for h in wechsel if h[3] == "rot"]
    td = [b - a for a, b in zip(tone, tone[1:])]
    if d:
        print("Phasenwechsel (Log, Wanduhr): %d; Dauer rot  min/mittel/max %.1f / %.1f / %.1f ms"
              % (len(wechsel), min(rot_d) / 1e3, sum(rot_d) / len(rot_d) / 1e3, max(rot_d) / 1e3))
        print("                                   Dauer umriss min/mittel/max %.1f / %.1f / %.1f ms"
              % (min(um_d) / 1e3, sum(um_d) / len(um_d) / 1e3, max(um_d) / 1e3))
        print("                                   Ton-Abstand  min/mittel/max %.1f / %.1f / %.1f ms  (%d Toene)"
              % (min(td) / 1e3, sum(td) / len(td) / 1e3, max(td) / 1e3, len(tone)))
        print("   Soll: Phase 39/59.826 s = 651.9 ms, Ton-Abstand 78/59.826 s = 1303.8 ms, Aufloesung 1 Bild")
    # Bilder
    files = sorted(glob.glob(os.path.join(OUT, MARKE + "_*.ppm")))
    rows = []
    for f in files:
        fr = int(re.search(r"_(\d+)\.ppm$", f).group(1))
        im = lade(f)
        ziel = mittel(im, 156 + 4, 76 + 4, 48 - 8, 40 - 8)
        titel = hashlib.md5(im.crop((30 * S, 30 * S, 118 * S, 62 * S)).tobytes()).hexdigest()[:8]
        rows.append((fr, ziel, titel, im))
    # Log-Zustand je Bild (letzter Wechsel <= Bild)
    def log_phase(fr):
        p = None
        for h in wechsel:
            if h[0] <= fr: p = h[3]
        return p
    titel_gruppen = {}
    for fr, z, t, im in rows:
        titel_gruppen.setdefault(t, []).append(fr)
    print("Titelkachel-Gruppen (Hash: Bilder):")
    for t, frs in titel_gruppen.items():
        print("   %s: F%d..F%d (%d Bilder)" % (t, min(frs), max(frs), len(frs)))
    # Klassifikation im Hinweis-Fenster
    passt = gesamt = 0
    for fr, z, t, im in rows:
        if beginn is None or zu is None or not (beginn + 8 <= fr < zu): continue
        klasse = "rot" if z[0] > 25 else "umriss"   # rot-Fuellung ueber dem Panel ~(49,13,55), Umriss ~(4,20,94)
        lp = log_phase(fr) or "umriss"
        gesamt += 1
        passt += (klasse == lp)
    print("Hinweis-Fenster F%d..F%d: %d Bilder, Pixelklasse == Log-Phase in %d" % (beginn + 8, zu - 1, gesamt, passt))
    for fr, z, t, im in rows:
        if beginn and fr in (beginn + 10, beginn + 30, beginn + 40, beginn + 60):
            print("   F%d Ziel-Mittel RGB (%.0f,%.0f,%.0f) Log-Phase %s" % (fr, z[0], z[1], z[2], log_phase(fr)))
    # Abzuege
    def png(fr, name):
        for r in rows:
            if r[0] == fr:
                r[3].save(os.path.join(OUT, "%s_%s_F%d.png" % (MARKE, name, fr)))
                print("   PNG %s_%s_F%d.png" % (MARKE, name, fr))
                return
    if beginn and zu:
        # je ein Bild mitten in einer roten und einer Umriss-Phase
        for want in ("rot", "umriss"):
            for fr, z, t, im in rows:
                if beginn + 20 <= fr < zu - 4 and log_phase(fr) == want and log_phase(fr - 4) == want and log_phase(fr + 4) == want:
                    png(fr, "hinweis_" + want); break
        png(zu + 30 - ((zu + 30) % 2), "nach_schliessen")
    for extra in sys.argv[2:]:
        png(int(extra), "bild")

if __name__ == "__main__":
    main()
