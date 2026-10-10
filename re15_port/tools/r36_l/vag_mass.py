#!/usr/bin/env python3
"""vag_mass.py — Runde 36 Spur L: PSX-VAG (SPU-ADPCM) dekodieren und die Huellkurve messen
(Anschlag, Spitze, Dauer, Nulldurchgangsrate = stimmhaft/rauschhaft). Optional WAV schreiben.
Dekoder = Standard-SPU-ADPCM (Filter-Koeffizienten wie re15_vag_adpcm_decode, vab_common.c).
Bibliothek: dekodieren(bytes) -> [int16], mass(pcm, rate) -> dict."""
import struct, wave

F = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]


def dekodieren(vag):
    out = []
    s1 = s2 = 0
    for i in range(0, len(vag) - 15, 16):
        hdr, flags = vag[i], vag[i + 1]
        shift, filt = hdr & 0x0F, (hdr >> 4) & 0x0F
        if filt > 4:
            filt = 0
        f0, f1 = F[filt]
        for k in range(14):
            b = vag[i + 2 + k]
            for nib in (b & 0x0F, b >> 4):
                v = nib - 16 if nib >= 8 else nib
                smp = (v << 12) >> shift
                smp += (s1 * f0 + s2 * f1 + 32) >> 6
                smp = max(-32768, min(32767, smp))
                out.append(smp)
                s2, s1 = s1, smp
        if flags & 1 and i > 0:
            break
    return out


def mass(pcm, rate, fenster_ms=10):
    w = max(1, rate * fenster_ms // 1000)
    env = []
    for j in range(0, len(pcm) - w + 1, w):
        seg = pcm[j:j + w]
        env.append((sum(x * x for x in seg) / w) ** 0.5)
    if not env:
        return {}
    pk = max(env)
    i_pk = env.index(pk)
    i_an = next(i for i, e in enumerate(env) if e >= 0.5 * pk)
    zc = sum(1 for i in range(1, len(pcm)) if (pcm[i - 1] < 0) != (pcm[i] < 0)) / max(1, len(pcm))
    ende = max(i for i, e in enumerate(env) if e >= 0.05 * pk)
    return dict(samples=len(pcm), dauer_s=len(pcm) / rate, anschlag_ms=i_an * fenster_ms,
                spitze_ms=i_pk * fenster_ms, hoerbar_ms=(ende + 1) * fenster_ms,
                spitze=max(abs(x) for x in pcm), nulldurchgang=zc)


def wav(path, pcm, rate):
    with wave.open(path, "wb") as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
        w.writeframes(struct.pack("<%dh" % len(pcm), *pcm))
