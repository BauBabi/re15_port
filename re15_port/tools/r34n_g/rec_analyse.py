#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht) - wertet eine DuckStation-Bildschirmaufnahme des ORIGINALS aus.

Aufnahme: ds_rec_fb0.py (Bildspeicher 0 der VRAM-Ansicht, 600x450 = 320x240 * 1.875, 30 fps)
bzw. ds_rec_small.py (kleiner Ausschnitt, 60 fps). DuckStation laeuft dabei mit
[Debug] ShowVRAM=true -> Bildspeicher 0 liegt auf dem Schirm bei (0,48)..(600,498).

Modus "voll" (Standard): skaliert auf 320x240 (flags=area), teilt die Aufnahme an Kamerawechseln
(mittlere Bilddifferenz > 8) in Abschnitte und schreibt je Abschnitt eine Aenderungskarte
(max-min je Pixel, *3) neben das erste Bild: <praefix>_seg_<start>.png. Dazu je Schild-Rechteck
(Cut 2 / Cut 3 / Cut 10, gemessen an der BSS-Dekodierung) die Laeufe, in denen das Schild sichtbar
ist, mit min/max von Blau-Pixelzahl und Helligkeit.

Modus "klein": rohe Bilder des Ausschnitts, Zahl der Bilder mit irgendeiner Aenderung.

  C:/Python310/python.exe re15_port/tools/r34n_g/rec_analyse.py voll  <aufnahme> <praefix>
  C:/Python310/python.exe re15_port/tools/r34n_g/rec_analyse.py klein <aufnahme> <breite> <hoehe>
"""
import subprocess, sys
import numpy as np

FFMPEG = "C:/ProgramData/chocolatey/bin/ffmpeg"
SIGNS = {"cut2": (285, 50, 313, 64), "cut3": (292, 6, 320, 22), "cut10": (262, 0, 288, 11)}


def frames(path, vf, w, h):
    raw = subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-i", path] +
                         (["-vf", vf] if vf else []) +
                         ["-f", "rawvideo", "-pix_fmt", "rgb24", "-"],
                         capture_output=True, check=True).stdout
    d = np.frombuffer(raw, dtype=np.uint8)
    n = d.size // (w * h * 3)
    return d[:n * w * h * 3].reshape(n, h, w, 3)


def voll(path, praefix):
    from PIL import Image
    a = frames(path, "scale=320:240:flags=area", 320, 240)
    n = len(a)
    L = a.astype(np.int16).mean(axis=3)
    dif = np.abs(np.diff(L, axis=0)).mean(axis=(1, 2))
    cuts = [0] + [i + 1 for i, v in enumerate(dif) if v > 8] + [n]
    print("Bilder %d" % n)
    for i in range(len(cuts) - 1):
        s, e = cuts[i], cuts[i + 1]
        if e - s < 8:
            continue
        rng = L[s:e].max(axis=0) - L[s:e].min(axis=0)
        print("Abschnitt %5d..%5d (%4d Bilder): Pixel mit Hub>40: %d" % (s, e - 1, e - s, int((rng > 40).sum())))
        img = np.clip(rng * 3, 0, 255).astype(np.uint8)
        Image.fromarray(np.concatenate([a[s], np.stack([img] * 3, axis=2)], axis=1)).save(
            "%s_seg_%05d.png" % (praefix, s))
    ai = a.astype(np.int16)
    for name, (x0, y0, x1, y1) in SIGNS.items():
        reg = ai[:, y0:y1, x0:x1, :]
        blue = ((reg[..., 2] > 150) & (reg[..., 2] > reg[..., 0] + 60)).sum(axis=(1, 2))
        Lr = reg.mean(axis=(1, 2, 3))
        vis = np.where(blue > 100)[0]
        runs = []
        if len(vis):
            s = p = vis[0]
            for v in vis[1:]:
                if v != p + 1:
                    runs.append((s, p)); s = v
                p = v
            runs.append((s, p))
        print("Schild %-5s sichtbar in %d Bildern" % (name, len(vis)))
        for s, e in runs:
            print("   Lauf %5d..%5d  blau %d..%d  L %.1f..%.1f" % (s, e, blue[s:e + 1].min(), blue[s:e + 1].max(),
                                                               Lr[s:e + 1].min(), Lr[s:e + 1].max()))


def klein(path, w, h):
    a = frames(path, None, w, h).astype(np.int16)
    d = np.abs(np.diff(a, axis=0)).sum(axis=(1, 2, 3))
    print("Bilder %d, Bilder mit Aenderung gegen das vorige: %d, groesste Summe %d" % (len(a), int((d > 0).sum()), int(d.max()) if len(d) else 0))
    blue = ((a[..., 2] > 150) & (a[..., 2] > a[..., 0] + 60)).sum(axis=(1, 2))
    print("Blau-Pixel je Bild: %d..%d" % (blue.min(), blue.max()))


if __name__ == "__main__":
    if sys.argv[1] == "voll":
        voll(sys.argv[2], sys.argv[3])
    else:
        klein(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]))
