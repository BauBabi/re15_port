#!/usr/bin/env python3
"""Kennzahlen + Spektrogramme der Schuss-Kandidaten (Abspielrate = Tonhoehe wie Port/RE2 @0x8005bbec/f8)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wellen import *
import numpy as np
from PIL import Image

def resample_to_44k(pcm, pitch):
    step = pitch << 4; frac = 0; pos = 0; out = []
    n = len(pcm)
    while pos < n:
        out.append(pcm[pos]); frac += step; pos += frac >> 16; frac &= 0xffff
    return np.array(out, float)

def kennz(name, pcm, pitch, h):
    rate = 44100.0 * pitch / 4096.0
    x = pcm; a = np.abs(x)
    w = max(1, int(rate * 0.005)); env = np.convolve(a, np.ones(w)/w, mode='same')
    pk = env.max(); ip = int(env.argmax())
    def bis(db):
        idx = np.where(env > pk * 10**(db/20.0))[0]
        return idx[-1]/rate*1000 if len(idx) else 0
    def seg(t0, t1):
        s = x[int(t0*rate):int(t1*rate)]
        if len(s) < 64: return (0, 0, 0)
        sp = np.abs(np.fft.rfft(s*np.hanning(len(s))))**2 + 1e-9
        fr = np.fft.rfftfreq(len(s), 1/rate)
        cen = float((sp*fr).sum()/sp.sum())
        flat = float(np.exp(np.mean(np.log(sp)))/np.mean(sp))
        lo = float(sp[fr < 250].sum()/sp.sum())
        return cen, flat, lo
    c1 = seg(0, 0.05); c2 = seg(0.05, 0.30)
    e = x**2; e50 = e[:int(rate*0.05)].sum()/e.sum()
    rms_peak = np.sqrt(np.max(np.convolve(e, np.ones(int(rate*0.01))/int(rate*0.01), mode='same')))
    print("%-36s %s rate %5.0f  Spitze %4.1fms  -20dB %4.0fms  -40dB %4.0fms  E0-50 %.2f | 0-50ms: Schw %5.0f Hz flach %.2f <250Hz %.2f | 50-300ms: Schw %5.0f flach %.2f <250 %.2f | Spitze %5.0f" % (
        name, h, rate, ip/rate*1000, bis(-20), bis(-40), e50, c1[0], c1[1], c1[2], c2[0], c2[1], c2[2], np.max(a)))

def spektro(x44, titel):
    n = 1024; hop = 256
    frames = []
    for i in range(0, max(1, len(x44)-n), hop):
        s = x44[i:i+n] * np.hanning(n)
        frames.append(20*np.log10(np.abs(np.fft.rfft(s))[:n//2] + 1e-3))
    S = np.array(frames).T
    S = np.clip((S - 40) / 60.0, 0, 1)    # 40..100 dB
    img = (255 * (1 - S[::-1, :])).astype(np.uint8)
    # auf 700 ms Breite normieren (hop 256 @44.1k = 5.8 ms -> 120 Spalten)
    W = 120
    if img.shape[1] < W:
        img = np.pad(img, ((0, 0), (0, W - img.shape[1])), constant_values=255)
    img = img[:, :W]
    return Image.fromarray(img).resize((W*2, 256))

if __name__ == '__main__':
    kand = [
        ('ALT  RE2 40A0/08 Partnerin->Alligator', welle_re2(R2+'ROOM40A0.RDT', 0x08)),
        ('NEU  RE2 4040/0A Annette (Art 3)', welle_re2(R2+'ROOM4040.RDT', 0x0A)),
        ('RE2 5000/0D Partnerin Ada (Skript)', welle_re2(R2+'ROOM5000.RDT', 0x0D)),
        ('RE2 ENEMSE30 VAG6 Ada-KI-Schuss', None),
        ('RE2 1010/0C Kendo (Schrot)', welle_re2(R2+'ROOM1010.RDT', 0x0C)),
        ('RE2 1120/05', welle_re2(R2+'ROOM1120.RDT', 0x05)),
        ('RE2 1120/06', welle_re2(R2+'ROOM1120.RDT', 0x06)),
        ('RE2 ARMS02 SE0 L0 (VP70)', welle_arms(R2S+'ARMS02.EDH', R2S+'ARMS02.VB', 0, 0)),
        ('RE2 ARMS03 SE0 L0 (Browning)', welle_arms(R2S+'ARMS03.EDH', R2S+'ARMS03.VB', 0, 0)),
        ('RE1.5 ARMS03 SE0 L0 (Elza)', welle_arms(SND+'ARMS03.EDH', SND+'ARMS03.VB', 0, 0)),
        ('RE1.5 ARMS03 SE0 L1 (Elza)', welle_arms(SND+'ARMS03.EDH', SND+'ARMS03.VB', 0, 1)),
        ('RE1.5 ARMS02 SE0 L0 (Leon)', welle_arms(SND+'ARMS02.EDH', SND+'ARMS02.VB', 0, 0)),
    ]
    e30 = enemse_vags(30)[6]
    kand[3] = (kand[3][0], (e30[0], e30[1], e30[2], e30[3]))
    bilder = []
    for name, (pcm, p, h, ti) in kand:
        kennz(name, pcm, p, h)
        bilder.append(spektro(resample_to_44k(pcm.astype(int).tolist(), p), name))
    W = bilder[0].size[0]; H = bilder[0].size[1]
    cols = 4
    rows = (len(bilder)+cols-1)//cols
    out = Image.new('L', (cols*(W+6), rows*(H+6)), 128)
    for i, b in enumerate(bilder):
        out.paste(b, ((i % cols)*(W+6), (i//cols)*(H+6)))
    out.save(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'spektro_kandidaten.png'))
