# ton_probe.py: Tuerknall-Ereignis gegen die Referenz bei verschiedenen Abspielraten (FFT-Korrelation).
import sys, numpy as np
S = "C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/spurL_n1"
src = open(S + "/ton_diff.py").read().split("mit = np.fromfile")[0]
sys.argv = ["x"]
exec(src)


def ncorr_fft(sig, ref):
    n = len(sig); m = len(ref)
    if n < m: return 0.0, 0
    rn = ref - ref.mean(); rr = np.sqrt((rn * rn).sum()) or 1.0
    L = 1 << int(np.ceil(np.log2(n + m)))
    c = np.fft.irfft(np.fft.rfft(sig, L) * np.conj(np.fft.rfft(rn, L)), L)[: n - m + 1]
    cs = np.concatenate([[0], np.cumsum(sig)]); cs2 = np.concatenate([[0], np.cumsum(sig * sig)])
    s1 = cs[m:] - cs[:-m]; s2 = cs2[m:] - cs2[:-m]
    den = np.sqrt(np.maximum(s2 - s1 * s1 / m, 1e-9)) * rr
    r = c / den
    i = int(np.argmax(r)); return float(r[i]), i


mit = np.fromfile(S + "/runs/a_mit/cap.raw", dtype=np.int16).astype(np.float64).reshape(-1, 2)
ohne = np.fromfile(S + "/runs/a_ohne/cap.raw", dtype=np.int16).astype(np.float64).reshape(-1, 2)
d = mit - ohne
raw = ref_tuer(); raw30 = ref_1030()
for a, nm in ((1909, "Tuer 1150"), (1975, "Tuer 1130"), (2156, "Knall 1040"), (2524, "Tuer 1030")):
    seg = d[(a - 5) * TICK:(a + 45) * TICK]
    M = seg.mean(axis=1)
    env = [int(np.sqrt((M[i:i + 2205] ** 2).mean())) for i in range(0, len(M) - 2205, 2205)]
    best = []
    for hz in (5512, 8000, 11025, 16397, 22050, 32794, 44100):
        for rn, rw in (("DOOR04", raw), ("1030", raw30)):
            r = resample(rw, hz)
            rr, o = ncorr_fft(M, r[: min(len(r), int(0.4 * 44100))])
            best.append((rr, rn, hz, o))
    best.sort(reverse=True)
    print(nm, "Huellkurve 50ms:", env[:24])
    print("   beste Korrelationen:", [(round(b[0], 3), b[1], b[2]) for b in best[:4]])
r = resample(raw, 16397)
print("Referenz DOOR04 @16397 Huellkurve 50ms:", [int(np.sqrt((r[i:i + 2205] ** 2).mean())) for i in range(0, len(r) - 2205, 2205)][:24])
