#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur F "dining1141": zwei Sprach-Schnitte aus den Nutzer-Aufnahmen (synchro/).

  python re15_port/tools/r38_dining/sprache_schneiden.py [--pruefen]

Beide Schnitte liegen in einer GEMESSENEN Sprechpause (10-ms-RMS < 0,02 des Spitzenwerts, mindestens 80 ms):
  1. synchro/STAGE1/room1141/main40.wav  = room1141/main02.wav ab 1,24 s
     ("Same here. Are you okay? What's happened?" -> "Are you okay? What's happened?"; Pause 0,98-1,50 s).
     Fuer ROOM1141 msg 40 (Port-Id, VERTRAG 1.2) = msg 2 ohne "Same here..." (Runde 37 Nachtrag 14 Punkt 5).
     Id 2 behaelt ihre Aufnahme (VERTRAG 1.2: Ids mit Nutzer-WAV nie umwidmen).
  2. synchro/STAGE1/room1141/main42.wav (Hilferuf, Sprach-Id 42 ohne Text) = room1171/main01.wav ab 1,95 s
     ("We need to get this thing off of here. Help me out." -> "Help me out."; Pause 1,87-2,04 s) — Johns
     Hilferuf vom Dach (Johns eigene Stimme; voice/Kendo hat kein "Help", gemessen per Transkription).
     Kein Message-Name: der Stimmen-Lader (mainNN.wav) liest die Datei NICHT; sie spielt nur ueber den
     Port-Ton der Spur (platform/pc/src/dining_r38_pc.c). Der Nutzer kann sie durch eine eigene Aufnahme ersetzen.
Ausgabe: 16-bit PCM mono in der Rate der Quelle, 5 ms Ein-/Ausblendung gegen Knackser.
"""
import hashlib, os, sys
import numpy as np
import scipy.io.wavfile as wf

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
SYN = os.path.join(REPO, "synchro", "STAGE1")

SCHNITTE = (
    # (Quelle, sha1 der Quelle, Schnitt ab [s], Pause [von, bis], Ziel)
    ("room1141/main02.wav", None, 1.24, (0.98, 1.50), "room1141/main40.wav"),
    ("room1171/main01.wav", None, 1.95, (1.87, 2.04), "room1141/main42.wav"),
)


def lesen(p):
    import warnings
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        sr, x = wf.read(p)
    return sr, x


def pausen(sr, x):
    y = x.astype(np.float64)
    if y.ndim > 1:
        y = y.mean(axis=1)
    y /= max(1.0, np.abs(y).max())
    win = int(sr * 0.01)
    n = len(y) // win
    rms = np.sqrt((y[:n * win].reshape(n, win) ** 2).mean(axis=1))
    still = rms <= 0.02
    out, s = [], None
    for i, a in enumerate(still):
        if a and s is None:
            s = i
        if not a and s is not None:
            if i - s >= 8:
                out.append((s * 0.01, i * 0.01))
            s = None
    if s is not None:
        out.append((s * 0.01, n * 0.01))
    return out


def schnitt(sr, x, t0):
    a = int(round(t0 * sr))
    y = x[a:].astype(np.float64)
    if y.ndim > 1:
        y = y.mean(axis=1)
    f = int(0.005 * sr)
    ramp = np.linspace(0.0, 1.0, f)
    y[:f] *= ramp
    y[-f:] *= ramp[::-1]
    if x.dtype != np.int16:
        y = y / max(1.0, np.abs(y).max()) * 32767.0
    return np.clip(np.round(y), -32768, 32767).astype(np.int16)


def main():
    pruefen = "--pruefen" in sys.argv
    fehler = 0
    for quelle, _sha, t0, pause, ziel in SCHNITTE:
        pq = os.path.join(SYN, quelle)
        roh = open(pq, "rb").read()
        sr, x = lesen(pq)
        ps = pausen(sr, x)
        treffer = [p for p in ps if p[0] - 0.011 <= t0 <= p[1] + 0.011]
        print("%s: %d Hz, %.2f s, sha1 %s, Pausen %s" % (quelle, sr, len(x) / sr, hashlib.sha1(roh).hexdigest(),
              ", ".join("%.2f-%.2f" % p for p in ps)))
        if not treffer or abs(treffer[0][0] - pause[0]) > 0.011 or abs(treffer[0][1] - pause[1]) > 0.011:
            sys.exit("ABBRUCH: Schnitt %.2f s liegt nicht in der gemessenen Pause %r" % (t0, pause))
        y = schnitt(sr, x, t0)
        pz = os.path.join(SYN, ziel)
        if pruefen:
            ok = os.path.exists(pz)
            if ok:
                sr2, z = lesen(pz)
                ok = sr2 == sr and len(z) == len(y) and np.array_equal(z, y)
            print("  %s %s" % (ziel, "ok" if ok else "FEHLT/ANDERS"))
            fehler += 0 if ok else 1
        else:
            wf.write(pz, sr, y)
            print("  -> %s (%.2f s, sha1 %s)" % (ziel, len(y) / sr, hashlib.sha1(open(pz, "rb").read()).hexdigest()))
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
