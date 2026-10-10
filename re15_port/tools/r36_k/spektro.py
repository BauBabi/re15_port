import sys, wave, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# Aufruf: python spektro.py <arbeitsordner>  (darin wav/ aus knall_zensus.py --wav <arbeitsordner>/wav und door04_vag3.raw)
S = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))
W = os.path.join(S, "wav")


def lies(fn):
    with wave.open(os.path.join(W, fn)) as w:
        r = w.getframerate(); x = np.frombuffer(w.readframes(w.getnframes()), dtype="<i2").astype(float)
    return x, r


def auf44(x, r):
    n = int(len(x) * 44100 / r)
    return np.interp(np.arange(n) * r / 44100.0, np.arange(len(x)), x)


def mix(fns, vols):
    parts = [auf44(*lies(f)) * v / 127.0 for f, v in zip(fns, vols)]
    n = max(len(p) for p in parts)
    m = np.zeros(n)
    for p in parts: m[:len(p)] += p
    return m


KAND = [
    ("RE2 DOOR04 Door_exit (IST-Stand Satz 0)", None),
    ("RE2 ROOM2050 Se_on 0x0b (sub05 Barrikade bricht, 3 Lagen)", (["ROOM2050_se0b_t9_vag4.wav", "ROOM2050_se0b_t10_vag6.wav", "ROOM2050_se0b_t11_vag6.wav"], [127, 127, 127])),
    ("RE2 ROOM2050 Se_on 0x0a (sub03/04, 3 Lagen)", (["ROOM2050_se0a_t7_vag4.wav", "ROOM2050_se0a_t8_vag4.wav", "ROOM2050_se0a_t9_vag4.wav"], [127, 127, 127])),
    ("RE2 ROOM1060 Se_on 0x0a (sub05 Tuer-Schlaege, 2 Lagen)", (["ROOM1060_se0a_t8_vag5.wav", "ROOM1060_se0a_t9_vag5.wav"], [127, 127])),
    ("RE2 ROOM1060 Se_on 0x0b (sub05)", (["ROOM1060_se0b_t10_vag6.wav"], [127])),
    ("RE2 ROOM2070 Satz 0x0a (kein Se_on)", (["ROOM2070_se0a_t6_vag5.wav"], [127])),
]


def main():
    door = np.fromfile(os.path.join(S, "door04_vag3.raw"), dtype="<i2").astype(float) if os.path.exists(os.path.join(S, "door04_vag3.raw")) else None
    fig, ax = plt.subplots(len(KAND), 2, figsize=(14, 3 * len(KAND)))
    for i, (nm, k) in enumerate(KAND):
        if k is None:
            if door is None: continue
            x = auf44(door, 16397)
        else:
            x = mix(*k)
        t = np.arange(len(x)) / 44100.0
        ax[i][0].plot(t, x, lw=0.4); ax[i][0].set_title(nm, fontsize=9); ax[i][0].set_xlim(0, 3.0); ax[i][0].set_ylim(-70000, 70000)
        ax[i][1].specgram(x, NFFT=1024, Fs=44100, noverlap=768, cmap="magma", vmin=-20, vmax=60)
        ax[i][1].set_ylim(0, 8000); ax[i][1].set_xlim(0, 3.0)
        env = [int(np.sqrt((x[j:j + 2205] ** 2).mean())) for j in range(0, len(x), 2205)]
        print("%-60s %.2f s  Spitze %6d  Huelle 50ms: %s" % (nm, len(x) / 44100.0, int(np.abs(x).max()), " ".join(map(str, env[:30]))))
    plt.tight_layout(); plt.savefig(os.path.join(S, "kandidaten.png"), dpi=70)


main()
