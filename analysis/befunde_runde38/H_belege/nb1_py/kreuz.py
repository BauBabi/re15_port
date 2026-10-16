#!/usr/bin/env python3
"""Unabhaengige Kreuzkorrelation (Abnahme 0, Spur H R38).

Eigener PSX-ADPCM-Decoder (nicht der des Ports), Welle direkt aus der Quell-RDT gelesen
(RE2 info/re2leon bzw. RE1.5 shared_assets), Tonhoehe ueber eine SUCHE (Halbton-Raster
+-3 HT um den erwarteten Wert), Abtastung wie der Mischer (Schritt = pitch<<4, Q16).

Aufruf:
  kreuz.py <cap.raw> <exit_bild> <quelle> <bild> [ab_ms lang_ms]
  quelle: re2:<RDT>:<satz>          RE2-Raumbank (RDT-Kopf +8: EDT/VH/VB)
          r15:<RDT>:<bank>:<se>     RE1.5-Raumbank (RDT-Kopf +8+bank*12)
"""
import sys, struct, math

COEF = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]

def vag_decode(buf):
    out = []
    s1 = s2 = 0
    for i in range(0, len(buf) - 15, 16):
        sh = buf[i] & 0x0F
        fi = (buf[i] >> 4) & 0x0F
        if fi > 4:
            fi = 0
        flags = buf[i + 1]
        k0, k1 = COEF[fi]
        for j in range(14):
            b = buf[i + 2 + j]
            for nib in (b & 0x0F, (b >> 4) & 0x0F):
                v = nib - 16 if nib >= 8 else nib
                v = (v << 12) >> sh
                v = v + ((s1 * k0 + s2 * k1 + 32) >> 6)
                v = max(-32768, min(32767, v))
                out.append(v)
                s2, s1 = s1, v
        if flags & 1 and i > 0:  # loop-end/end flag: Ende der Welle
            break
    return out

PITCH_LUT = [int(round(4096 * 2 ** (i / 192.0))) for i in range(192)]

def note2pitch2(note, fine, center, shift):
    s = ((fine & 0xffff) + (shift & 0xff)) >> 3
    frac, carry = s, 0
    if s > 15:
        frac, carry = s - 16, 1
    sem = carry + note + 0x3c - center
    octv = sem // 12 - 5 if sem >= 0 else -5
    idx = (sem % 12) * 16 + frac
    idx = max(0, min(191, idx))
    p = PITCH_LUT[idx]
    if octv < 0:
        p >>= (-octv)
    elif octv >= 1:
        p = 0x3FFF
    return p & 0xffff

def bank(d, edt, vh, vb, se):
    rec = d[edt + se * 4: edt + se * 4 + 4]
    prog = rec[1] & 0x7f
    tone = rec[2] >> 4
    assert d[vh:vh + 4] == b'pBAV', d[vh:vh + 4]
    ps = struct.unpack_from('<H', d, vh + 0x12)[0]
    tb = vh + 0x20 + 0x800 + prog * 16 * 32 + tone * 32
    t = d[tb: tb + 32]
    vag = struct.unpack_from('<H', t, 0x16)[0]
    vt = vh + 0x20 + 0x800 + ps * 16 * 32
    sizes = [struct.unpack_from('<H', d, vt + 2 * i)[0] * 8 for i in range(256)]
    off = vb + sum(sizes[1:vag])
    sz = sizes[vag]
    center, shift, mn = t[4], t[5], t[6]
    pitch = note2pitch2(mn, shift, center, shift)
    return rec, off, sz, pitch, (t[2], t[3], center, shift, mn)

def quelle(spec):
    kind, rest = spec.split(':', 1)
    if kind == 're2':
        path, satz = rest.rsplit(':', 1)
        d = open(path, 'rb').read()
        edt, vh, vb = struct.unpack_from('<3I', d, 8)
        return d, bank(d, edt, vh, vb, int(satz, 0))
    if kind == 'r15':
        path, b, se = rest.rsplit(':', 2)
        d = open(path, 'rb').read()
        edt, vh, vb = struct.unpack_from('<3I', d, 8 + int(b) * 12)
        return d, bank(d, edt, vh, vb, int(se, 0))
    raise SystemExit('quelle?')

def resample(pcm, pitch):
    step = pitch << 4
    frac = 0
    pos = 0
    out = []
    n = len(pcm)
    while pos < n:
        out.append(pcm[pos])
        frac += step
        pos += frac >> 16
        frac &= 0xffff
    return out

def main():
    cap, exitb, spec, bild = sys.argv[1], int(sys.argv[2]), sys.argv[3], int(sys.argv[4])
    ab_ms = int(sys.argv[5]) if len(sys.argv) > 5 else 0
    lang_ms = int(sys.argv[6]) if len(sys.argv) > 6 else 400
    raw = open(cap, 'rb').read()
    n = len(raw) // 4
    x = [0.0] * n
    for i in range(n):
        l, r = struct.unpack_from('<hh', raw, i * 4)
        x[i] = (l + r) * 0.5
    takte = n // 1470
    off = takte - 1 - exitb
    d, (rec, voff, vsz, pitch, tinfo) = quelle(spec)
    pcm = vag_decode(d[voff: voff + vsz])
    print('quelle %s: EDT %s VAG @0x%X %d B -> %d Samples, Ton vol/pan/center/shift/min %s, pitch 0x%03x' % (
        spec, rec.hex(), voff, vsz, len(pcm), tinfo, pitch))
    ab = ab_ms * 441 // 10
    lang = lang_ms * 441 // 10

    def best(t, b):
        tt = t[ab: ab + lang]
        stt = sum(v * v for v in tt)
        if stt <= 0:
            return -2, 0
        l0 = (off + b - 2) * 1470 + ab
        l1 = (off + b + 6) * 1470 + ab
        bk, bl = -2.0, 0
        # grob (Schritt 7), dann fein
        def k(lag):
            if lag < 0 or lag + len(tt) > n:
                return 0.0
            sxt = 0.0; sxx = 0.0
            for i, v in enumerate(tt):
                a = x[lag + i]; sxt += a * v; sxx += a * a
            return sxt / math.sqrt(sxx * stt) if sxx > 0 else 0.0
        for lag in range(l0, l1 + 1, 7):
            c = k(lag)
            if c > bk:
                bk, bl = c, lag
        for lag in range(bl - 7, bl + 8):
            c = k(lag)
            if c > bk:
                bk, bl = c, lag
        return bk, (bl - ab - (off + b) * 1470) / 1470.0

    # Tonhoehen-Suche: erwarteter Wert + Nachbarn (je 1/2 HT, +-3 HT)
    res = []
    for hs in range(-6, 7):
        p = int(round(pitch * 2 ** (hs / 24.0)))
        t = resample(pcm, p)
        k, v = best(t, bild)
        res.append((k, hs, p, v))
    res.sort(reverse=True)
    for k, hs, p, v in res[:4]:
        print('  pitch 0x%03x (%+.1f HT): korrelation %.3f versatz %.2f Bilder' % (p, hs / 2.0, k, v))
    t = resample(pcm, pitch)
    kg, vg = best(t, bild - 60)
    print('  gegenprobe 60 Bilder frueher (erwartete Tonhoehe): %.3f' % kg)
    # Pegel je Bild um das Ereignis
    lv = []
    for b in range(bild - 6, bild + 16):
        s = (off + b) * 1470
        seg = x[s: s + 1470]
        pk = max(abs(v) for v in seg) if seg else 0
        lv.append('%d:%d' % (b, pk))
    print('  Spitze je Bild:', ' '.join(lv))

if __name__ == '__main__':
    main()
