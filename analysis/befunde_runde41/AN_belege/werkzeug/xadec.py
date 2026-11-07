"""XA-ADPCM aus RE2 VOICExx.XAS (2336-Byte-Sektoren: 8 Byte Subheader + 2324 Daten + 4 EDC).
xadec.py XAS KANAL BLOCK LAENGE OUT.wav  -> dekodiert die Kanal-Sektoren [BLOCK, BLOCK+LAENGE)."""
import sys, struct, wave
K0 = [0, 60, 115, 98]; K1 = [0, 0, -52, -55]
def sectors(path):
    d = open(path, "rb").read()
    n = len(d) // 2336
    for i in range(n):
        s = d[i*2336:(i+1)*2336]
        yield i, s
def decode_sector(s, hist):
    sub = s[0:8]; data = s[8:8+2324]
    coding = sub[3]
    stereo = coding & 1; rate = 18900 if coding & 4 else 37800; bits = 8 if coding & 0x10 else 4
    out = []
    for g in range(18):
        grp = data[g*128:(g+1)*128]
        hdr = grp[0:16]
        for blk in range(8):
            sh = hdr[4 + blk]
            shift = sh & 0x0F; flt = (sh >> 4) & 3
            for j in range(28):
                byte = grp[16 + j*4 + (blk >> 1)]
                nib = (byte >> 4) if (blk & 1) else (byte & 0x0F)
                if nib >= 8: nib -= 16
                smp = (nib << 12) >> shift
                smp = smp + ((hist[0]*K0[flt] + hist[1]*K1[flt] + 32) >> 6)
                smp = max(-32768, min(32767, smp))
                hist[1] = hist[0]; hist[0] = smp
                out.append(smp)
    return out, rate, stereo
def main():
    path, ch, blk, ln, outp = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
    hist = [0, 0]; samples = []; k = 0; rate = 37800; got = 0; info = None
    for i, s in sectors(path):
        sub = s[0:8]
        if sub[1] != ch: continue
        if not (sub[2] & 0x04): continue   # Audio-Submode
        if blk <= k < blk + ln:
            o, rate, st = decode_sector(s, hist); samples += o; got += 1
            if info is None: info = (i, sub.hex(" "))
        k += 1
    w = wave.open(outp, "wb"); w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
    w.writeframes(struct.pack("<%dh" % len(samples), *samples)); w.close()
    print("kanal %d sektoren gesamt %d, gelesen %d (erster Dateisektor %s), %d Samples @%d = %.2f s" % (ch, k, got, info, len(samples), rate, len(samples)/rate))
if __name__ == "__main__": main()
