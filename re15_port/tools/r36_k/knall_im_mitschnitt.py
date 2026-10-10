#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""knall_im_mitschnitt.py — Runde 36 Spur K (K1, Messung vorher/nachher): wo und wie laut liegt welcher Knall im
deterministischen Ton-Mitschnitt der echten exe (RE15_AUDIO_CAP_SYNC=cap.raw: s16 stereo 44100 Hz, je Spielbild
1470 Frames, audio_pc.c)?

Referenzen (selbst dekodiert, knall_zensus.adpcm):
  DOOR04  = RE2 DOOR04.DO2 Tone 2 -> VAG 3 @ 16397 Hz (Runde-35-Satz 0, Door_exit)
  RE2_1060 = RE2 ROOM1060.RDT Tone 8 -> VAG 5 @ 11025 Hz (Runde-36-Satz 0, sub05 @0x00F48)
  R1030   = RE1.5 ROOM1030.RDT Tone 7 -> VAG 6 @ 11025 Hz (Satz 1)
Ausgabe je Referenz: die besten Fundstellen (normierte Kreuzkorrelation der ersten 0,4 s), Zeit, Spielbild (= Zeit*30),
Huellkurven-Maximum (50 ms) im Fenster 0..1,5 s ab Fundstelle.
Aufruf: python knall_im_mitschnitt.py <cap.raw> [n_treffer]
"""
import os, struct, sys
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, HERE)
import knall_zensus as K  # noqa: E402


def ref_door04():
    d = open(os.path.join(REPO, "re15_port", "shared_assets", "RE2", "DOOR", "DOOR04.DO2"), "rb").read()
    vh = struct.unpack_from("<I", d, 0xC30)[0]
    _, tones, sizes = K.vab(d, vh)
    v = tones[(0, 2)]["vag"]; off = sum(sizes[1:v])
    return K.adpcm(d[0xC38 + off:0xC38 + off + sizes[v]]), 16397


def ref_raum(pfad, prog, tone):
    d = open(pfad, "rb").read()
    edt, vh, vb = struct.unpack_from("<III", d, 8)
    _, tones, sizes = K.vab(d, vh)
    t = tones[(prog, tone)]; v = t["vag"]; off = sum(sizes[1:v])
    return K.adpcm(d[vb + off:vb + off + sizes[v]]), 44100.0 * 2 ** ((t["mn"] - t["center"]) / 12.0)


def auf44(x, r):
    n = int(len(x) * 44100 / r)
    return np.interp(np.arange(n) * r / 44100.0, np.arange(len(x)), x)


def ncorr(sig, ref):
    n, m = len(sig), len(ref)
    rn = ref - ref.mean(); rr = np.sqrt((rn * rn).sum()) or 1.0
    L = 1 << int(np.ceil(np.log2(n + m)))
    c = np.fft.irfft(np.fft.rfft(sig, L) * np.conj(np.fft.rfft(rn, L)), L)[:n - m + 1]
    cs = np.concatenate([[0], np.cumsum(sig)]); cs2 = np.concatenate([[0], np.cumsum(sig * sig)])
    s1 = cs[m:] - cs[:-m]; s2 = cs2[m:] - cs2[:-m]
    den = np.sqrt(np.maximum(s2 - s1 * s1 / m, 1e-9)) * rr
    return c / den


def main():
    cap = np.fromfile(sys.argv[1], dtype="<i2").astype(np.float64).reshape(-1, 2).mean(axis=1)
    ntr = int(sys.argv[2]) if len(sys.argv) > 2 else 6
    refs = {
        "DOOR04": ref_door04(),
        "RE2_1060": ref_raum(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM1060.RDT"), 0, 8),
        "R1030": ref_raum(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE1", "ROOM1030.RDT"), 0, 7),
    }
    print("Mitschnitt: %d Abtastwerte = %.1f s = %d Spielbilder" % (len(cap), len(cap) / 44100.0, len(cap) // 1470))
    for nm, (x, r) in refs.items():
        ref = auf44(x, r)[:int(0.4 * 44100)]
        c = ncorr(cap, ref)
        treffer = []
        cc = c.copy()
        for _ in range(ntr):
            i = int(np.argmax(cc))
            if cc[i] < 0.5: break
            seg = cap[i:i + int(1.5 * 44100)]
            env = [np.sqrt((seg[j:j + 2205] ** 2).mean()) for j in range(0, len(seg) - 2205, 2205)]
            treffer.append((i / 44100.0, i // 1470, float(c[i]), int(max(env)) if env else 0))
            cc[max(0, i - 44100):i + 44100] = -1
        treffer.sort()
        print("%-8s (%5.0f Hz): %s" % (nm, r, "; ".join("t=%.2f s Bild %d r=%.3f Huelle-max %d" % t for t in treffer) or "kein Treffer >= 0,5"))


if __name__ == "__main__":
    main()
