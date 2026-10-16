#!/usr/bin/env python3
"""Bass (<=250 Hz) / Hoehen (>=2.5 kHz) je Welle, einmal wie der Port-Mischer (Halten, audio_pc.c audio_callback)
und einmal bandbegrenzt (Interpolation wie das SPU-Gauss-Filter, hier ideal) — Quelle vs. Mischerartefakt."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wellen import *
from klangbild import resample_to_44k
from terz import terz, FC
import numpy as np
from scipy.signal import resample
def ideal44(pcm, pitch):
    rate = 44100.0*pitch/4096.0
    n = int(round(len(pcm)*44100/rate))
    return resample(pcm, n)
def kz(t):
    lo = 10*np.log10(np.sum(10**(t[FC.index(63):FC.index(250)+1]/10)))
    hi = 10*np.log10(np.sum(10**(t[FC.index(2500):]/10)))
    return lo, hi
W = [
 ('ALT  40A0/08 Partnerin (e552)', welle_re2(R2+'ROOM40A0.RDT', 0x08)),
 ('NEU  4040/0A Annette (994b)', welle_re2(R2+'ROOM4040.RDT', 0x0A)),
 ('RE2 5000/0D Partnerin (4482)', welle_re2(R2+'ROOM5000.RDT', 0x0D)),
 ('RE2 ENEMSE30/6 Ada-KI (db90)', enemse_vags(30)[6]),
 ('RE2 1010/0C Kendo (42bb)', welle_re2(R2+'ROOM1010.RDT', 0x0C)),
 ('RE1.5 ARMS02 SE0 Handgun', welle_arms(SND+'ARMS02.EDH', SND+'ARMS02.VB', 0)),
 ('RE1.5 ARMS03 SE0 Browning', welle_arms(SND+'ARMS03.EDH', SND+'ARMS03.VB', 0)),
 ('RE1.5 ARMS07 SE0 Redhawk', welle_arms(SND+'ARMS07.EDH', SND+'ARMS07.VB', 0)),
 ('RE1.5 ARMS08 SE0 Schrot', welle_arms(SND+'ARMS08.EDH', SND+'ARMS08.VB', 0)),
 ('RE1.5 ARMS13 SE0', welle_arms(SND+'ARMS13.EDH', SND+'ARMS13.VB', 0)),
 ('RE1.5 ARMS14 SE0', welle_arms(SND+'ARMS14.EDH', SND+'ARMS14.VB', 0)),
]
print('%-32s %6s | %-22s | %-22s' % ('Welle', 'Rate', 'Port (Halten) Bass/Hoeh', 'bandbegrenzt Bass/Hoeh'))
for n, w in W:
    pcm, p = w[0], w[1]
    a = kz(terz(resample_to_44k(pcm.astype(int).tolist(), p)))
    b = kz(terz(ideal44(pcm, p)))
    print('%-32s %6.0f | %6.1f dB / %6.1f dB | %6.1f dB / %6.1f dB' % (n, 44100*p/4096, a[0], a[1], b[0], b[1]))
