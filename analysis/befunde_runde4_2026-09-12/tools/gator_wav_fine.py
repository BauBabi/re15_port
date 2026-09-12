#!/usr/bin/env python3
"""Feinanalyse der Bank-17-SEs: 10ms-Huellkurve (ASCII), ZCR, Transienten."""
import wave, struct, math, os
OUT = "c:/workspace/git/reAi_v2/analysis/befunde_runde4_2026-09-12/gator_se_wavs"
for se in range(6):
    fn = [f for f in os.listdir(OUT) if f.startswith("bank17_se%02d" % se)][0]
    w = wave.open(os.path.join(OUT, fn))
    rate = w.getframerate(); n = w.getnframes()
    pcm = struct.unpack("<%dh" % n, w.readframes(n))
    w.close()
    win = max(1, rate // 100)   # 10 ms
    rms = []
    zcr = []
    for i in range(0, n, win):
        seg = pcm[i:i+win]
        rms.append(math.sqrt(sum(s*s for s in seg)/len(seg)))
        z = sum(1 for a, b in zip(seg, seg[1:]) if (a < 0) != (b < 0))
        zcr.append(z * rate / (2*len(seg)))   # geschaetzte Grundfrequenz-Naehe (Hz)
    peak = max(rms) or 1
    # ASCII-Huellkurve: 1 Zeichen je 30ms (3 Fenster gemittelt)
    bars = " .:-=+*#%@"
    env = ""
    for i in range(0, len(rms), 3):
        v = sum(rms[i:i+3])/len(rms[i:i+3])
        env += bars[min(9, int(v/peak*9.999))]
    # Transienten: Fenster, in denen RMS um Faktor >=4 gegen Vorfenster springt
    trans = [i*0.01 for i in range(1, len(rms)) if rms[i] > 4*max(rms[i-1], peak*0.02)]
    # mittlere ZCR im lauten Bereich
    loud_z = [z for r, z in zip(rms, zcr) if r > peak*0.3]
    print("SE %d  %s  rate=%d  dauer=%.2fs" % (se, fn, rate, n/rate))
    print("   env(30ms/Z): |%s|" % env)
    print("   ZCR(laut)=%d Hz (Median %d)  Transienten(x4-Sprung)@ %s" % (
        sum(loud_z)/max(1,len(loud_z)),
        sorted(loud_z)[len(loud_z)//2] if loud_z else 0,
        ", ".join("%.2fs" % t for t in trans[:8]) or "-"))
