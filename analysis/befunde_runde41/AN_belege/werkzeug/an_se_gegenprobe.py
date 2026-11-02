"""an_se_gegenprobe.py - Runde 41 Spur AN, Nachbesserung 1: Gegenprobe der Mini-Bank ANNETTE_SE.VBS (Belege an_se.md).

Teile:
  ram      DuckStation-Savestates der echten Claire-Disc (Lauf 2): RAUM_A/B @0x800d07b0, ENEMSE-Bank-Nummer @0x800cbddf,
           EDT-/VH-Zeiger der Bank 3 (@0x800d3664 / @0x800cf094), RAM-EDT[0x0B], RAM-Bank gegen ENEMSE.VBS Sektor 2887.
  kreuz    die Wellen der INSTALLIERTEN Mini-Bank (shared_assets/RE2/ANNETTE_SE.VBS, Abspielrate = note2pitch2 der Ton-Bytes)
           gegen die Tonspur der echten Aufnahme re2c_lauf1_voll.mp4: normierte Kreuzkorrelation (Verfahren xa_einsatz.py),
           Nullmodell = alle anderen Wellen derselben RE2-Bank und die eigene Welle um +-1 Halbton verstimmt.
  steigen  ZUG_AN (ROOM7020 Satz 0x0C) und die anderen Toene von ROOM7020 sub04: staerkste Spektralspitze 100..4000 Hz je
           0,15 s - nur 0x0C hat einen steigenden Ton (Hochfahren).
Aufruf: python an_se_gegenprobe.py ram|kreuz|steigen   (numpy, scipy, zstandard; ffmpeg fuer kreuz)
"""
import os, struct, subprocess, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WT = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))                       # Baumwurzel
BANK = os.path.join(WT, "re15_port", "shared_assets", "RE2", "ANNETTE_SE.VBS")
SCR = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/an"
VID = SCR + r"/capture/lauf1/re2c_lauf1_voll.mp4"
CAP = SCR + r"/capture"
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg.exe"
sys.path.insert(0, os.path.join(WT, "re15_port", "tools", "r41_annette"))
import an_se_cut as cut                                                                   # Quellpfade, note2pitch2
FILT = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]


def decode(w):
    out, s1, s2 = [], 0, 0
    for b in range(0, len(w) - 15, 16):
        sh, fi = w[b] & 15, min((w[b] >> 4) & 15, 4)
        f0, f1 = FILT[fi]
        for i in range(28):
            nib = (w[b + 2 + i // 2] >> (4 if i & 1 else 0)) & 15
            if nib & 8: nib -= 16
            s = ((nib << 12) >> sh) + ((s1 * f0 + s2 * f1 + 32) >> 6)
            s = max(-32768, min(32767, s)); s2, s1 = s1, s; out.append(s)
        if (w[b + 1] & 1) and not (w[b + 1] & 2): break
    return np.array(out, float)


class Vh:
    def __init__(self, d, vh, vb):
        self.d, self.vh, self.vb = d, vh, vb
        self.np = struct.unpack_from("<H", d, vh + 0x12)[0]; self.nv = struct.unpack_from("<H", d, vh + 0x16)[0]
        self.st = vh + 0x820 + self.np * 0x200
    def ton(self, prog, t): o = self.vh + 0x820 + prog * 0x200 + t * 0x20; return self.d[o:o + 32]
    def vag(self, i):
        pos = self.vb + sum(struct.unpack_from("<H", self.d, self.st + 2 * k)[0] << 3 for k in range(i))
        return pos, struct.unpack_from("<H", self.d, self.st + 2 * i)[0] << 3
    def welle(self, i): p, n = self.vag(i); return decode(self.d[p:p + n])


def rate(t): return 44100.0 * cut.note2pitch2(t[6], t[5], t[4], t[5]) / 4096.0


def mini_bank():
    d = open(BANK, "rb").read()
    vh_off = d.find(b"pBAV")
    nprog = struct.unpack_from("<H", d, vh_off + 0x12)[0]
    edt_size = vh_off + 0x20 + 0x800 + nprog * 0x200 + 0x200 + 8          # SE-Map + VH + Trailer
    assert struct.unpack_from("<I", d, edt_size - 8)[0] == vh_off, "Trailer-VH-Offset"
    v = Vh(d, vh_off, edt_size)
    saetze = []
    for s in range(4):
        e = d[s * 4:s * 4 + 4]; t = v.ton(0, e[2] >> 4); vi = struct.unpack_from("<H", t, 0x16)[0]
        saetze.append((e, t, v.welle(vi), rate(t)))
    return saetze


def teil_ram():
    sys.path.insert(0, os.path.join(WT, "analysis", "befunde_runde41", "AN_belege", "re2_claire", "werkzeug"))
    from re2c_ss import SS
    vbs = open(cut.VBS_C, "rb").read()
    for nm in ("s00", "s10", "s49", "s50"):
        s = SS(CAP + "/lauf2/" + nm + ".sav")
        e3, v3 = s.u32(0x800d3664), s.u32(0x800cf094)
        zeile = "%s: RAUM_A/B %s Bank %d | EDT*[3] 0x%08x VH*[3] 0x%08x" % (nm, s.bytes(0x800d07b0, 2).hex(" "),
                                                                           s.u8(0x800cbddf), e3, v3)
        if e3 == 0x801f8610:
            ram = s.bytes(e3, 0x90)
            zeile += " | RAM-EDT[0x0B] %s | RAM EDT+VH-Kopf == ENEMSE.VBS @0x5A3800: %s" % (
                s.bytes(e3 + 0x2C, 4).hex(" "), ram == vbs[0x5A3800:0x5A3800 + 0x90])
        print(zeile)


def teil_kreuz():
    from scipy.signal import fftconvolve
    R = 22050
    p = subprocess.run([FF, "-v", "error", "-i", VID, "-vn", "-ac", "1", "-ar", str(R), "-f", "s16le", "-"],
                       capture_output=True)
    a = np.frombuffer(p.stdout, np.int16).astype(float); a2 = np.r_[0.0, np.cumsum(a * a)]
    print("Tonspur %s: %.3f s" % (os.path.basename(VID), len(a) / R))

    def auf(x, r): n = int(len(x) * R / r); return np.interp(np.arange(n) * (r / R), np.arange(len(x)), x)

    def ncc(y):
        L = len(y); c = fftconvolve(a, y[::-1], mode="valid"); ew = a2[L:] - a2[:-L]
        return c / (np.sqrt(np.maximum(ew, 0.05 * ew.max())) * np.sqrt((y * y).sum()) + 1e-9)[:len(c)]

    def fenster(nc, e): lo, hi = int((e - 3) * R), int((e + 3) * R); k = lo + int(np.argmax(nc[lo:hi])); return k / R, nc[k]
    sz = mini_bank()
    vbs = open(cut.VBS_C, "rb").read(); r6 = open(cut.R6141, "rb").read()
    banken = {0: Vh(vbs, 0x5A3880, 0x5A4800), 1: Vh(r6, 0xA8F0, 0xB710), 2: Vh(r6, 0xA8F0, 0xB710)}
    ziel = {0: 6, 1: 10, 2: 11}
    erwartet = {0: 49.40, 1: 58.70, 2: 60.85}                          # zeitachse.md: Krach / Sturz-Ton / Ton steigt
    for s in (0, 1, 2):
        e, t, x, r = sz[s]
        nc = ncc(auf(x, r)); k = int(np.argmax(nc)); m = nc.copy(); m[max(0, k - R):k + R] = -9; k2 = int(np.argmax(m))
        tf, rf = fenster(nc, erwartet[s])
        print("Satz %d (%s): %.3f s @%.1f Hz | global %.3f s r=%.3f, zweitbeste %.3f s r=%.3f | Fenster %.2f+-3 s: %.3f s r=%.3f"
              % (s, ("KRACH", "STURZ", "BRUELL")[s], len(x) / r, r, k / R, nc[k], k2 / R, nc[k2], erwartet[s], tf, rf))
        b = banken[s]; null = []
        for i in range(b.nv):
            if i == ziel[s] or b.vag(i)[1] == 0: continue
            null.append((fenster(ncc(auf(b.welle(i), r)), erwartet[s])[1], i))
        null.sort(reverse=True)
        print("   Nullmodell andere Wellen der RE2-Bank: bestes r=%.3f (VAG %d), Median r=%.3f"
              % (null[0][0], null[0][1], float(np.median([n[0] for n in null]))))
        hb = [fenster(ncc(auf(x, r * f)), erwartet[s])[1] for f in (2 ** (-1 / 12), 2 ** (1 / 12))]
        print("   eigene Welle -1 / +1 Halbton: r=%.3f / r=%.3f" % tuple(hb))


def teil_steigen():
    d = open(cut.R7020, "rb").read(); v = Vh(d, 0x5D50, 0x6B70)
    for sid, wo in ((0x0A, "@0x0145A Stecker"), (0x0C, "@0x014AA nach Set(4,0x88)"), (0x13, "@0x014EC msg 1"),
                    (0x0B, "@0x01532 nach Cut_replace")):
        e = d[0x5C90 + sid * 4:0x5C90 + sid * 4 + 4]; t = v.ton(e[1] & 0x7F, e[2] >> 4)
        x = v.welle(struct.unpack_from("<H", t, 0x16)[0]); r = rate(t)
        win = 1024; hop = int(r * 0.15); spitzen = []
        for k in range(0, len(x) - win, hop):
            sp = np.abs(np.fft.rfft(x[k:k + win] * np.hanning(win))); f = np.fft.rfftfreq(win, 1.0 / r)
            m = (f > 100) & (f < 4000); spitzen.append((k / r, f[m][int(np.argmax(sp[m]))]))
        print("Satz 0x%02X %s: %.2f s @%.1f Hz, Spitze je 0,15 s: %s" % (sid, wo, len(x) / r, r,
              " ".join("%.2f:%d" % (tt, ff) for tt, ff in spitzen)))


if __name__ == "__main__":
    {"ram": teil_ram, "kreuz": teil_kreuz, "steigen": teil_steigen}[sys.argv[1] if len(sys.argv) > 1 else "kreuz"]()
