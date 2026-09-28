#!/usr/bin/env python3
"""r30_port_frames_check.py - Runde 30 / Thema D, ABNAHME (e) Helligkeit + Periode am Fenster.

Prueft JEDES Bild einer gdigrab-Aufnahme (r30_port_capture_hwnd.py) pixelgenau gegen das
Original-Modell der Menuezeilen. Das Modell ist das von r30_port_frame_check.py (rechnet nur
aus DATA/TITLEU.TIM und DATA/TMOJI.TIM, an 12 Original-Bildpuffern geprueft); hier wird es
lediglich ueber alle Bilder gefahren und um die Zeitachse ergaenzt.

Je Bild: bester Pulswert der aktiven Zeile + Zahl abweichender Pixel in den vier Regionen
(aktive Zeile, zwei inaktive Zeilen, Copyright).
Zusammenfassung:
  - Bilder mit 0 abweichenden Pixeln in ALLEN vier Regionen
  - gesehene Pulswerte (Soll: alle 32 von 0x80 bis 0xBE)
  - Pulsperiode aus der Folge der erkannten Pulswerte und den Zeitstempeln der Aufnahme
    (Ruecksprung auf 0x80), Haltedauer je Pulswert

Aufruf: r30_port_frames_check.py <aufnahme-verzeichnis> [cursor] [--ab N]
        --ab N : die ersten N Bilder ueberspringen (Titel-Einblende)
Rueckgabe 0 = jedes gepruefte Bild deckt sich in allen vier Regionen, 1 = Abweichung.
"""
import os, subprocess, sys, statistics
import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import r30_port_frame_check as M

# Assets des Baums, in dem dieses Werkzeug liegt (byte-gleich zum Hauptbaum)
M.ASSETS = os.path.abspath(os.path.join(HERE, "..", "..", "..", "re15_port", "shared_assets", "PSX", "DATA"))
FFPROBE = r"C:\ProgramData\chocolatey\bin\ffprobe.exe"

def pts_list(mkv):
    r = subprocess.run([FFPROBE, "-v", "error", "-select_streams", "v:0", "-show_entries", "frame=pts_time",
                        "-of", "csv=p=0", mkv], capture_output=True, text=True)
    return [float(x.strip().strip(",")) for x in r.stdout.splitlines() if x.strip().strip(",")]

def main():
    a = sys.argv[1:]
    d = a[0]
    cursor, ab = 0, 0
    i = 1
    while i < len(a):
        if a[i] == "--ab": ab = int(a[i + 1]); i += 2
        else: cursor = int(a[i]); i += 1
    frames = sorted(f for f in os.listdir(os.path.join(d, "frames")) if f.endswith(".png"))
    pts = pts_list(os.path.join(d, "capture.mkv"))
    if len(pts) != len(frames):
        print("Warnung: %d Zeitstempel, %d Bilder" % (len(pts), len(frames)))
    bg = M.load_tim16(os.path.join(M.ASSETS, "TITLEU.TIM"))
    pix, clut = M.load_tim8(os.path.join(M.ASSETS, "TMOJI.TIM"))
    # Modell je Pulswert einmal vorausrechnen
    act = {}
    y0 = M.ROWY[cursor]; v0 = 16 * (cursor + 1)
    for val in range(0x80, 0xc0, 2):
        act[val] = M.sim_strip(bg, pix, clut, y0, v0, 16, 0, val)
    inact = {}
    for row in range(3):
        if row != cursor:
            inact[row] = M.sim_strip(bg, pix, clut, M.ROWY[row], 16 * (row + 1), 16, 192, 0x80)
    cop = M.sim_strip(bg, pix, clut, 0xc8, 0x52, 20, 0, 0x80)

    res = []
    scale = None
    lum_act, lum_in = [], []
    for k, f in enumerate(frames):
        if k < ab: continue
        im = Image.open(os.path.join(d, "frames", f)).convert("RGB")
        if im.size != (320, 240):
            if im.size[0] % 320 or im.size[1] % 240:
                print("Bildgroesse %s ist kein ganzzahliges Vielfaches von 320x240" % (im.size,)); return 2
            scale = im.size[0] // 320
            im = im.resize((320, 240), Image.NEAREST)
        raw = np.array(im).astype(np.int32)
        got = raw >> 3
        reg = got[y0:y0 + 17, 0x20:0x120]
        # mittlere Helligkeit der aktiven und einer inaktiven Zeile (unabhaengig vom Modell)
        def lum(y):
            q = (raw[y:y + 17, 0x20:0x120] >> 3) << 3
            return float(((q[..., 0] * 299 + q[..., 1] * 587 + q[..., 2] * 114) // 1000).mean())
        lum_act.append((pts[k] if k < len(pts) else -1.0, lum(y0)))
        lum_in.append(lum(M.ROWY[(cursor + 1) % 3]))
        best = min(((int((act[v] != reg).any(axis=-1).sum()), v) for v in act), key=lambda t: t[0])
        bad_in = 0
        for row in inact:
            r = got[M.ROWY[row]:M.ROWY[row] + 17, 0x20:0x120]
            bad_in += int((inact[row] != r).any(axis=-1).sum())
        bad_c = int((cop != got[0xc8:0xc8 + 21, 0x20:0x120]).any(axis=-1).sum())
        res.append((k, pts[k] if k < len(pts) else -1.0, best[1], best[0], bad_in, bad_c))
    n = len(res)
    clean = [r for r in res if r[3] == 0 and r[4] == 0 and r[5] == 0]
    print("Bilder geprueft        : %d (ab Bild %d)%s" % (n, ab, "" if scale is None else "  Massstab %d" % scale))
    print("deckungsgleich (4/4)   : %d von %d" % (len(clean), n))
    print("aktive Zeile           : %d Bilder mit Abweichung, groesste %d von 4352 Pixeln" % (
        sum(1 for r in res if r[3]), max(r[3] for r in res)))
    print("inaktive Zeilen        : %d Bilder mit Abweichung, groesste %d von 8704 Pixeln" % (
        sum(1 for r in res if r[4]), max(r[4] for r in res)))
    print("Copyright              : %d Bilder mit Abweichung, groesste %d von 5376 Pixeln" % (
        sum(1 for r in res if r[5]), max(r[5] for r in res)))
    vals = sorted(set(r[2] for r in clean))
    # Pulswerte, die im Modell DASSELBE Bild ergeben, sind am Bild nicht zu unterscheiden
    # (5-Bit-Kanaele: (texel * wert) >> 7 aendert sich nicht bei jedem Schritt um 2)
    klassen = []
    for v in sorted(act):
        for kl in klassen:
            if (act[kl[0]] == act[v]).all():
                kl.append(v); break
        else:
            klassen.append([v])
    gesehen = [kl for kl in klassen if kl[0] in vals]
    print("Pulswerte am Bild      : %d von %d unterscheidbaren Stufen gesehen" % (len(gesehen), len(klassen)))
    mehr = [kl for kl in klassen if len(kl) > 1]
    if mehr:
        print("                         nicht unterscheidbar: %s" % "; ".join(
            "/".join("%02x" % v for v in kl) for kl in mehr))
    fehlt = [kl for kl in klassen if kl[0] not in vals]
    if fehlt:
        print("                         nicht gesehen: %s" % " ".join("%02x" % kl[0] for kl in fehlt))
    for r in res:
        if r[3] or r[4] or r[5]:
            print("  erstes abweichendes Bild %d (t=%.3f): bester Pulswert 0x%02x, aktiv %d, inaktiv %d, Copyright %d" % (
                r[0], r[1], r[2], r[3], r[4], r[5]))
            break
    # Zeitachse: Wechsel des erkannten Pulswerts
    if clean and pts:
        runs = []
        for r in res:
            if r[3]: continue
            if not runs or runs[-1][0] != r[2]: runs.append([r[2], r[1], 1])
            else: runs[-1][2] += 1
        hold = [(b[1] - a[1]) * 1000.0 for a, b in zip(runs, runs[1:])]
        resets = [x[1] for x in runs[1:] if x[0] == 0x80]
        per = [(b - a) * 1000.0 for a, b in zip(resets, resets[1:])]
        steps = [(b[0] - a[0]) for a, b in zip(runs, runs[1:])]
        print("Pulsstufen in Folge    : %d; Schritte %s" % (len(runs), sorted(set(steps))))
        if hold:
            print("Haltedauer je Stufe    : Median %.2f ms   Mittel %.3f ms   min %.2f   max %.2f" % (
                statistics.median(hold), statistics.mean(hold), min(hold), max(hold)))
        if per:
            print("PULSPERIODE (Fenster)  : Median %.1f ms   Mittel %.1f ms   min %.1f   max %.1f   (n=%d)" % (
                statistics.median(per), statistics.mean(per), min(per), max(per), len(per)))
    # Periode aus der HELLIGKEIT der aktiven Zeile (steigende Durchgaenge durch die Mitte) -
    # haengt nicht am Modell und gilt deshalb auch fuer einen Stand, dessen Helligkeits-
    # Abbildung noch nicht stimmt
    ys = [y for _, y in lum_act]; ts = [t for t, _ in lum_act]
    print("Helligkeit aktive Zeile: min %.3f max %.3f Spanne %.3f   inaktive Zeile: Spanne %.3f" % (
        min(ys), max(ys), max(ys) - min(ys), max(lum_in) - min(lum_in)))
    if max(ys) - min(ys) > 0.01 and ts[0] >= 0:
        mid = (max(ys) + min(ys)) / 2
        cross = []
        for j in range(1, len(ys)):
            if ys[j - 1] < mid <= ys[j]:
                cross.append(ts[j])
        per2 = [(b - a) * 1000.0 for a, b in zip(cross, cross[1:])]
        if per2:
            print("PERIODE aus Helligkeit : Median %.1f ms   Mittel %.1f ms   min %.1f   max %.1f   (n=%d)" % (
                statistics.median(per2), statistics.mean(per2), min(per2), max(per2), len(per2)))
    ok = (len(clean) == n)
    print("ERGEBNIS: %s" % ("jedes Bild deckt sich pixelgenau mit dem Original-Modell" if ok else "ABWEICHUNG"))
    return 0 if ok else 1

if __name__ == "__main__":
    sys.exit(main())
