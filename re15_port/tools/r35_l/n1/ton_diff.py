# ton_diff.py <mit.raw> <ohne.raw> [hz_tuer hz_1030]
# Differenz zweier RE15_AUDIO_CAP_SYNC-Mitschnitte (s16 stereo 44100, 1470 Stereo-Frames je Spielbild):
# Ereignisse (Bursts) im Differenzsignal, je Ereignis Onset-Bild, Dauer, RMS/Spitze, und die normierte
# Korrelation mit den zwei Referenzwellen (PSX-ADPCM selbst dekodiert, auf die Abspielrate umgerechnet).
import sys, struct
import numpy as np

REPO = "C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/shared_assets/"
TICK = 1470


def adpcm(b):
    f = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]
    out = []; s1 = s2 = 0
    for o in range(0, len(b) - 15, 16):
        sh = b[o] & 0xF; fi = (b[o] >> 4) & 0x7
        if fi > 4: fi = 0
        for i in range(28):
            byte = b[o + 2 + i // 2]
            n = (byte >> ((i & 1) * 4)) & 0xF
            if n & 8: n -= 16
            v = (n << 12) >> sh
            v += (s1 * f[fi][0] + s2 * f[fi][1] + 32) >> 6
            v = max(-32768, min(32767, v))
            out.append(v); s2, s1 = s1, v
    return np.array(out, dtype=np.float64)


def ref_tuer():
    d = open(REPO + "RE2/DOOR/DOOR04.DO2", "rb").read()
    # VH @0x10 ("pBAV"), VB @0xC38; VAG-Groessen-Tabelle hinter Kopf+Programme+Tones (wie knall_cut.py)
    vh = 0x10
    nprog = struct.unpack_from("<H", d, vh + 0x12)[0]
    sz = vh + 32 + 128 * 16 + nprog * 16 * 32
    sizes = [struct.unpack_from("<H", d, sz + 2 * i)[0] * 8 for i in range(256)]
    off = sum(sizes[1:3]); n = sizes[3]
    return adpcm(d[0xC38 + off: 0xC38 + off + n])


def ref_1030():
    a = open(REPO + "PSX/STAGE1/ROOM1030.RDT", "rb").read()
    return adpcm(a[0x8BA0: 0x8BA0 + 8080])


def resample(x, hz):
    if hz <= 0: return x
    n = int(len(x) * 44100.0 / hz)
    t = np.arange(n) * (hz / 44100.0)
    return np.interp(t, np.arange(len(x)), x)


def ncorr(sig, ref):
    """max. normierte Kreuzkorrelation (ref ganz im Fenster), Rueckgabe (r, versatz)."""
    if len(sig) < len(ref): return 0.0, 0
    best = (0.0, 0)
    rn = ref - ref.mean(); rr = np.sqrt((rn * rn).sum()) or 1
    c = np.correlate(sig, rn, mode="valid")
    s2 = np.convolve(sig * sig, np.ones(len(ref)), mode="valid")
    s1 = np.convolve(sig, np.ones(len(ref)), mode="valid")
    den = np.sqrt(np.maximum(s2 - s1 * s1 / len(ref), 1e-9)) * rr
    r = c / den
    i = int(np.argmax(r))
    return float(r[i]), i


mit = np.fromfile(sys.argv[1], dtype=np.int16).astype(np.float64).reshape(-1, 2)
ohne = np.fromfile(sys.argv[2], dtype=np.int16).astype(np.float64).reshape(-1, 2)
n = min(len(mit), len(ohne))
mit, ohne = mit[:n], ohne[:n]
diff = (mit - ohne).mean(axis=1)
nt = n // TICK
rms_d = np.sqrt((diff[:nt * TICK].reshape(nt, TICK) ** 2).mean(axis=1))
rms_m = np.sqrt((mit[:nt * TICK].mean(axis=1).reshape(nt, TICK) ** 2).mean(axis=1))
print("Bilder %d (%.1f s); Differenz identisch (0) in %d Bildern" % (nt, nt / 30.0, int((rms_d == 0).sum())))
hz_t = float(sys.argv[3]) if len(sys.argv) > 3 else 0
hz_a = float(sys.argv[4]) if len(sys.argv) > 4 else 0
rt = resample(ref_tuer(), hz_t); ra = resample(ref_1030(), hz_a)
print("Referenzen: Tuer %d Abtastwerte (%.2f s @%s Hz), Knall1030 %d (%.2f s @%s Hz)" % (len(rt), len(rt) / 44100, hz_t, len(ra), len(ra) / 44100, hz_a))
ev = []; i = 0
while i < nt:
    if rms_d[i] > 200:
        j = i
        while j < nt and rms_d[j:j + 5].max() > 50: j += 1
        ev.append((i, j)); i = j + 1
    else:
        i += 1
for (a, b) in ev:
    seg = diff[max(0, (a - 5) * TICK): (b + 5) * TICK]
    rT, oT = ncorr(seg, rt[: min(len(rt), len(seg))])
    rA, oA = ncorr(seg, ra[: min(len(ra), len(seg))])
    print("Ereignis Bild %5d..%5d (%.2f s): Diff-RMS max %6.0f, Spitze %6.0f, Gesamt-RMS max %6.0f | Korrelation Tuer %.3f  Knall1030 %.3f"
          % (a, b, (b - a) / 30.0, rms_d[a:b + 1].max(), np.abs(diff[a * TICK:(b + 1) * TICK]).max(), rms_m[a:b + 1].max(), rT, rA))
