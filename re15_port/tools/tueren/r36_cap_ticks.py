#!/usr/bin/env python3
"""r36_cap_ticks.py - Runde 36 Spur G: RE15_AUDIO_CAP_SYNC-Mitschnitt je Audio-Tick auswerten.

RE15_AUDIO_CAP_SYNC (platform/pc/src/audio_pc.c) schreibt je re15_audio_tick() genau RE15_AUDIO_RATE/30 = 1470
Stereo-Frames s16le (44100 Hz). Ein Tick ist ein Spielbild ab Programmstart (Titel eingeschlossen); die Zuordnung
Tick <-> Raumbild F (RE15_AUFSCHLIESS_LOG) ist ein fester Versatz je Lauf.

Je Tick: Pegel (RMS, Mono-Mittel), Anteile 1350..1550 Hz (Piep der RE2-Welle b03b6dff), unter 250 Hz
(Aufschliessen da805fa3 / Riegel d1d99d65), 2..4 kHz (Klack 9064a95e), Spitzenfrequenz.
Mit --gegen <cap2.raw>: Pegel der Differenz und der erste Tick, ab dem beide Laeufe abweichen (Gegenlauf ohne Ton).

Aufruf: python r36_cap_ticks.py cap.raw <von> <bis> [--gegen andere.raw] [--nur-piep]
"""
import argparse

import numpy as np

T = 1470


def lade(p):
    return np.fromfile(p, dtype="<i2").astype(np.float64).reshape(-1, 2).mean(axis=1)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cap")
    ap.add_argument("von", type=int)
    ap.add_argument("bis", type=int)
    ap.add_argument("--gegen", default="")
    ap.add_argument("--nur-piep", action="store_true")
    a = ap.parse_args()
    m = lade(a.cap)
    g = lade(a.gegen) if a.gegen else None
    n = len(m) // T
    if g is not None:
        n = min(n, len(g) // T)
    print("Ticks", n)
    f = np.fft.rfftfreq(T, 1 / 44100.0)
    fenster = np.hanning(T)
    erster = None
    for t in range(n):
        x = m[t * T:(t + 1) * T]
        diff = None
        if g is not None:
            d = x - g[t * T:(t + 1) * T]
            diff = float(np.sqrt((d * d).mean()))
            if erster is None and diff > 1.0:
                erster = t
        if not (a.von <= t < a.bis):
            continue
        X = np.abs(np.fft.rfft(x * fenster)) ** 2
        tot = X[1:].sum() + 1e-9
        piep = X[(f > 1350) & (f < 1550)].sum() / tot
        if a.nur_piep and piep <= 0.3:
            continue
        tief = X[(f > 20) & (f < 250)].sum() / tot
        hoch = X[(f > 2000) & (f < 4000)].sum() / tot
        spitze = f[1 + int(np.argmax(X[1:]))]
        z = "t%4d rms %8.1f  1450Hz %4.2f  <250Hz %4.2f  2-4kHz %4.2f  Spitze %7.1f" % (
            t, float(np.sqrt((x * x).mean())), piep, tief, hoch, spitze)
        if diff is not None:
            z += "  rms(a-b) %8.1f" % diff
        print(z)
    if g is not None:
        print("erster abweichender Tick:", erster)


if __name__ == "__main__":
    main()
