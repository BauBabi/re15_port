"""Wasser-Ton-Nachweis: welcher VAG der Raum-Bank snd0 klingt in den Schritt-Fenstern des Mitschnitts?
Aufruf: python vag_korr.py <RDT> <cap.raw> <tick>:<etikett> ...  (tick = Mitschnitt-Bild des Schritt-Tons)"""
import sys, struct, numpy as np
rdt = open(sys.argv[1], 'rb').read()
u32 = lambda o: struct.unpack_from('<I', rdt, o)[0]
edt, vh, vb = u32(0x08), u32(0x0c), u32(0x10)
ps, ts, vs = struct.unpack_from('<HHH', rdt, vh + 0x12)
tone_base = vh + 0x820
vagsz_base = tone_base + ps * 512
sizes = [struct.unpack_from('<H', rdt, vagsz_base + 2*i)[0] * 8 for i in range(vs + 1)]
F = [(0,0),(60,0),(115,-52),(98,-55),(122,-60)]
def decode(k1):      # 1-basiert
    off = vb + sum(sizes[1:k1]); n = sizes[k1]
    out = []; s1 = s2 = 0
    for b in range(0, n, 16):
        blk = rdt[off+b: off+b+16]
        if len(blk) < 16: break
        sh, fl = blk[0] & 0xf, (blk[0] >> 4) & 0xf
        f0, f1 = F[fl] if fl < 5 else (0,0)
        for i in range(28):
            nib = (blk[2 + i//2] >> (4 * (i & 1))) & 0xf
            if nib >= 8: nib -= 16
            s = (nib << 12) >> sh
            s = s + ((s1 * f0 + s2 * f1 + 32) >> 6)
            s = max(-32768, min(32767, s))
            out.append(s); s2, s1 = s1, s
    return np.array(out, float)
def edt_vag(idx):
    e = rdt[edt + 4*idx: edt + 4*idx + 4]
    prog, tone = e[1] & 0x7f, e[2] >> 4
    t = tone_base + (prog * 16 + tone) * 32
    center, shift, mn = rdt[t+4], rdt[t+5], rdt[t+6]
    vag = struct.unpack_from('<H', rdt, t + 22)[0]
    return vag, center, shift, mn
cap = np.fromfile(sys.argv[2], dtype='<i2').reshape(-1, 2).astype(float).mean(1)
kand = {}
for idx in (0x17, 0x1a, 0x1d, 0x18, 0x1e):
    vag, c, sh, mn = edt_vag(idx)
    if vag <= 0 or vag > vs: continue
    rate = 2 ** ((mn - c) / 12.0)       # Abspiel-Rate relativ zu 44100 (Note min_note gegen Mitte)
    pcm = decode(vag)
    kand[idx] = (vag, rate, pcm)
    print("EDT %#04x -> VAG %d (0-basiert %d) Mitte %d Note %d Faktor %.3f, %d Abtastwerte" % (idx, vag, vag-1, c, mn, rate, len(pcm)))
for arg in sys.argv[3:]:
    tick, et = arg.split(':'); tick = int(tick)
    best = []
    for idx, (vag, rate, pcm) in kand.items():
        # auf 44100 umrechnen
        n = int(len(pcm) / rate)
        ref = np.interp(np.arange(n) * rate, np.arange(len(pcm)), pcm)
        bestc = 0
        for lag in range(-1470, 1470 * 2, 49):
            st = tick * 1470 + lag
            w = cap[st: st + n]
            if len(w) < n or n < 100: continue
            r = ref[:len(w)]
            c = float(np.dot(w - w.mean(), r - r.mean()) / (np.linalg.norm(w - w.mean()) * np.linalg.norm(r - r.mean()) + 1e-9))
            bestc = max(bestc, c)
        best.append((bestc, idx, vag))
    best.sort(reverse=True)
    print("Fenster Tick %d (%s): " % (tick, et) + "  ".join("EDT %#04x/VAG%d r=%.2f" % (b[1], b[2] - 1, b[0]) for b in best))
