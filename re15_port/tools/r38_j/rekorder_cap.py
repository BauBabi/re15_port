#!/usr/bin/env python3
"""rekorder_cap.py - Runde 38 Spur J: RE15_AUDIO_CAP_SYNC-Mitschnitt gegen die Rekorder-Aufnahme des Nutzers.

Sucht die WAV (synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav, linear auf 44100 Hz gerechnet wie audio_pc.c
re15_voice_load_clip) im Mitschnitt (Kreuzkorrelation ueber das Mono-Mittel, je Tick 1470 Stereo-Frames) und gibt
Anfangs-/End-Tick aus, dazu die Ticks mit Piep-Anteil (1350..1550 Hz > 0,3, Welle RE2 ROOM2110 Satz 0x0D) und
den Anteil der Aufnahme am Mitschnitt (Restpegel nach Abzug der Aufnahme).

Aufruf: python rekorder_cap.py cap.raw wav [--von T] [--bis T]
"""
import argparse
import struct

import numpy as np

T = 1470


def wav_lesen(p):
    b = open(p, "rb").read()
    o = 12
    rate = ch = bits = None
    data = None
    while o + 8 <= len(b):
        cid = b[o:o + 4]
        sz = struct.unpack_from("<I", b, o + 4)[0]
        if cid == b"fmt ":
            _tag, ch, rate, _br, _ba, bits = struct.unpack_from("<HHIIHH", b, o + 8)
        elif cid == b"data":
            data = b[o + 8:o + 8 + sz]
            break
        o += 8 + sz + (sz & 1)
    assert bits == 16
    x = np.frombuffer(data, dtype="<i2").astype(np.float64)
    if ch > 1:
        x = x.reshape(-1, ch).mean(axis=1)
    # linear wie audio_pc.c: out_n = src_n * 44100 / rate; sp = i * rate / 44100 (Q16)
    out_n = len(x) * 44100 // rate
    i = np.arange(out_n, dtype=np.int64)
    sp = (i * rate * 65536) // 44100
    si = sp >> 16
    fr = (sp & 0xFFFF) / 65536.0
    s1 = np.minimum(si + 1, len(x) - 1)
    return x[si] + (x[s1] - x[si]) * fr


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cap")
    ap.add_argument("wav")
    ap.add_argument("--von", type=int, default=0)
    ap.add_argument("--bis", type=int, default=10 ** 9)
    a = ap.parse_args()
    m = np.fromfile(a.cap, dtype="<i2").astype(np.float64).reshape(-1, 2).mean(axis=1)
    w = wav_lesen(a.wav)
    n = len(m) // T
    lo = max(0, a.von) * T
    hi = min(n, a.bis) * T
    seg = m[lo:hi]
    # Kreuzkorrelation per FFT
    L = 1
    while L < len(seg) + len(w):
        L *= 2
    c = np.fft.irfft(np.fft.rfft(seg, L) * np.conj(np.fft.rfft(w, L)), L)[:len(seg) - len(w) + 1]
    k = int(np.argmax(c))
    start = lo + k
    ende = start + len(w)
    norm = float(np.sqrt((w * w).sum() * (seg[k:k + len(w)] ** 2).sum()) + 1e-9)
    print("WAV Samples %d (44100 Hz) = %.2f Ticks" % (len(w), len(w) / T))
    print("Korrelation %.3f" % (float(c[k]) / norm))
    print("WAV Anfang Sample %d = Tick %d (+%d)" % (start, start // T, start % T))
    print("WAV Ende   Sample %d = Tick %d (+%d) -> letzter Tick mit WAV %d" % (ende, ende // T, ende % T,
                                                                           (ende - 1) // T))
    # Verstaerkung (Mischer: smp * 0x3FFF >> 14 je Kanal, Mono-Mittel) schaetzen und abziehen
    g = float((seg[k:k + len(w)] * w).sum() / ((w * w).sum() + 1e-9))
    rest = seg[k:k + len(w)] - g * w
    print("Verstaerkung %.3f, Rest-RMS %.1f gegen WAV-RMS %.1f" % (g, float(np.sqrt((rest ** 2).mean())),
                                                                   float(np.sqrt(((g * w) ** 2).mean()))))
    f = np.fft.rfftfreq(T, 1 / 44100.0)
    fen = np.hanning(T)
    pieps = []
    for t in range(max(0, a.von), min(n, a.bis)):
        x = m[t * T:(t + 1) * T]
        X = np.abs(np.fft.rfft(x * fen)) ** 2
        tot = X[1:].sum() + 1e-9
        if X[(f > 1350) & (f < 1550)].sum() / tot > 0.3:
            pieps.append(t)
    print("Piep-Ticks (1450 Hz > 0,3):", (pieps[0], pieps[-1], len(pieps)) if pieps else "keine")


if __name__ == "__main__":
    main()
