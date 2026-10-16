#!/usr/bin/env python3
"""Terzband-Spektren (0-150 ms) der Schuss-Kandidaten + Abstand zur Klasse der Spieler-Waffenschuesse (RE1.5 ARMS)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wellen import *
import numpy as np
from klangbild import resample_to_44k
FC = [63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000]
def terz(x44, t1=0.15):
    s = x44[:int(44100*t1)]
    sp = np.abs(np.fft.rfft(s*np.hanning(len(s))))**2
    fr = np.fft.rfftfreq(len(s), 1/44100)
    b = []
    for fc in FC:
        lo, hi = fc/2**(1/6), fc*2**(1/6)
        b.append(sp[(fr >= lo) & (fr < hi)].sum() + 1e-9)
    b = np.array(b); b = 10*np.log10(b/b.sum())
    return b
def zoh(pcm, pitch):   # wie der Port-Mischer (audio_pc.c audio_callback: pcm[pos], kein Interpolieren)
    return resample_to_44k(pcm.astype(int).tolist(), pitch)
if __name__ == '__main__':
    ref = {
      'RE1.5 ARMS02 SE0 (Leon Handgun)': welle_arms(SND+'ARMS02.EDH', SND+'ARMS02.VB', 0),
      'RE1.5 ARMS03 SE0 (Elza Browning)': welle_arms(SND+'ARMS03.EDH', SND+'ARMS03.VB', 0),
    }
    for a in ('ARMS04','ARMS05','ARMS06','ARMS07','ARMS08'):
        try: ref['RE1.5 %s SE0' % a] = welle_arms(SND+a+'.EDH', SND+a+'.VB', 0)
        except Exception as ex: print(a, ex)
    kand = {
      'ALT  RE2 40A0/08 Partnerin': welle_re2(R2+'ROOM40A0.RDT', 0x08),
      'NEU  RE2 4040/0A Annette (Art 3)': welle_re2(R2+'ROOM4040.RDT', 0x0A),
      'RE2 5000/0D Partnerin (Skript)': welle_re2(R2+'ROOM5000.RDT', 0x0D),
      'RE2 ENEMSE30 VAG6 Ada-KI': enemse_vags(30)[6][:3] + (None,),
      'RE2 1010/0C Kendo Schrot (Art 3)': welle_re2(R2+'ROOM1010.RDT', 0x0C),
    }
    R = {k: terz(zoh(v[0], v[1])) for k, v in ref.items()}
    mitte = np.mean(np.array(list(R.values())), axis=0)
    print('Terzmitten:', ' '.join(str(f) for f in FC))
    for k, v in R.items():
        print('%-34s %s  Abstand zur Klassenmitte %.1f dB' % (k, ' '.join('%4.0f' % x for x in v), np.sqrt(np.mean((v-mitte)**2))))
    print('%-34s %s' % ('KLASSENMITTE Spieler-Waffen', ' '.join('%4.0f' % x for x in mitte)))
    for k, v in kand.items():
        t = terz(zoh(v[0], v[1]))
        d = np.sqrt(np.mean((t-mitte)**2))
        lo = 10*np.log10(np.sum(10**(t[FC.index(63):FC.index(250)+1]/10)))
        hi = 10*np.log10(np.sum(10**(t[FC.index(2500):]/10)))
        print('%-34s %s  Abstand %.1f dB  <=250Hz %5.1f dB  >=2.5kHz %5.1f dB' % (k, ' '.join('%4.0f' % x for x in t), d, lo, hi))
    for k, v in R.items():
        t = v
        lo = 10*np.log10(np.sum(10**(t[FC.index(63):FC.index(250)+1]/10)))
        hi = 10*np.log10(np.sum(10**(t[FC.index(2500):]/10)))
        print('%-34s <=250Hz %5.1f dB  >=2.5kHz %5.1f dB' % (k, lo, hi))
