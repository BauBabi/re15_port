#!/usr/bin/env python3
"""r30_karte3010_hint_se.py - der RE2-KARTENHINWEIS-Ton: Satz 0x2B der RAUMBANK.

RE2 FUN_8006F1C4 @0x8006F234-38:  lui a0,0x22b / jal 0x8005ba28  = Se_on(0x022B0000,0)
  Bank 2 = Raumbank: FUN_80059E54 laedt EDT = RDT+0x08 (@0x80059F1C lw a1,8(a2)
  -> sw 0x800DBB80 @0x80059F44), VH = RDT+0x0C (@0x80059F5C -> sw 0x800D75A8 @0x80059F70),
  VB = RDT+0x10 (@0x80059FF0).
  Satz 0x2B -> EDT[0x2B] (4 Byte): b1&0x7f = Programm, b2>>4 = Ton, b2&0xf = Prioritaet,
  b3&0x1f = Stimme, b3>>5 = Zusatzlagen (FUN_8005BA28, RE2_Quellcode_V2/FUN_8005ba28.c).

Das Werkzeug loest fuer die vier Hinweis-Raeume (Zensus r30_karte3010_hint_zensus.py)
den Satz bis zur Welle auf, meldet jeden Datei-Offset und schreibt die rohe VAG-Welle
+ eine dekodierte WAV nach build/r30_karte-3010/.
"""
import struct, os, sys, hashlib, wave
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
OUT = os.path.join(REPO, "build", "r30_karte-3010")
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
ROOMS = ["ROOM3010", "ROOM3040", "ROOM30B0", "ROOM6030"]
SE = 0x2B

K0 = [0.0, 60 / 64.0, 115 / 64.0, 98 / 64.0, 122 / 64.0]
K1 = [0.0, 0.0, -52 / 64.0, -55 / 64.0, -60 / 64.0]

def vag_decode(b):
    out = []; s1 = s2 = 0.0
    for i in range(0, len(b) - 15, 16):
        pf = b[i]; fl = b[i + 1]
        sh = pf & 15; f = (pf >> 4) & 15
        if f > 4: f = 0
        for j in range(28):
            byte = b[i + 2 + j // 2]
            nib = (byte >> 4) if (j & 1) else (byte & 15)
            v = nib << 12
            if v & 0x8000: v -= 0x10000
            s = (v >> sh) + s1 * K0[f] + s2 * K1[f]
            s2 = s1; s1 = s
            out.append(max(-32768, min(32767, int(round(s)))))
        if fl & 1: break
    return out

def main():
    os.makedirs(OUT, exist_ok=True)
    shas = {}
    for r in ROOMS:
        p = os.path.join(RDT, r + ".RDT")
        d = open(p, "rb").read()
        edt, vh, vb = struct.unpack_from("<III", d, 8)
        n_edt = (vh - edt) // 4
        rec_off = edt + SE * 4
        rec = d[rec_off:rec_off + 4]
        assert d[vh:vh + 4] == b"pBAV", "kein pBAV"
        ver, vid, fsize = struct.unpack_from("<III", d, vh + 4)
        nprog, ntone, nvag = struct.unpack_from("<HHH", d, vh + 18)
        prog = rec[1] & 0x7F; tone = rec[2] >> 4; prio = rec[2] & 0xF
        voice = rec[3] & 0x1F; extra = rec[3] >> 5
        # Tone-Segmente liegen in der Reihenfolge der BELEGTEN Programme
        slot = 0
        for i in range(prog):
            if d[vh + 32 + i * 16] != 0: slot += 1
        tone_off = vh + 32 + 128 * 16 + slot * 512 + tone * 32
        t = d[tone_off:tone_off + 32]
        vagidx = struct.unpack_from("<H", t, 0x16)[0]
        sz_tab = vh + 32 + 128 * 16 + nprog * 512
        off = 0
        for i in range(1, vagidx):
            off += struct.unpack_from("<H", d, sz_tab + i * 2)[0] * 8
        size = struct.unpack_from("<H", d, sz_tab + vagidx * 2)[0] * 8
        total_vb = sum(struct.unpack_from("<H", d, sz_tab + i * 2)[0] * 8 for i in range(1, nvag + 1))
        w = d[vb + off:vb + off + size]
        sha = hashlib.sha1(w).hexdigest()
        shas[r] = sha
        print("%s  EDT @0x%05X (%d Saetze)  VH @0x%05X (nprog=%d ntone=%d nvag=%d, VH-Laenge %d)  VB @0x%05X (%d B)" % (
            r, edt, n_edt, vh, nprog, ntone, nvag, sz_tab + 512 - vh, vb, total_vb))
        print("   EDT[0x%02X] @0x%05X = %s -> Programm %d Ton %d Prio %d Stimme %d Zusatzlagen %d" % (
            SE, rec_off, rec.hex(" "), prog, tone, prio, voice, extra))
        print("   Tone @0x%05X: %s" % (tone_off, t.hex(" ")))
        print("     prior=%d mode=%d vol=%d pan=%d center=%d shift=%d min=%d max=%d  ADSR1=0x%04X ADSR2=0x%04X  VAG=%d" % (
            t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7],
            struct.unpack_from("<H", t, 0x10)[0], struct.unpack_from("<H", t, 0x12)[0], vagidx))
        print("   VAG %d @0x%05X  %d B  sha1 %s" % (vagidx, vb + off, size, sha))
        open(os.path.join(OUT, "hint_se_%s.vag" % r), "wb").write(w)
        pcm = vag_decode(w)
        # Tonhoehe: note = min (SsUtKeyOnV @0x8004522c-Aequivalent, note=tone[+6]); Halbtonabstand
        semis = (t[6] - t[4]) - t[5] / 128.0
        rate = int(round(44100 * 2 ** (semis / 12.0)))
        wv = wave.open(os.path.join(OUT, "hint_se_%s.wav" % r), "wb")
        wv.setnchannels(1); wv.setsampwidth(2); wv.setframerate(max(4000, min(96000, rate)))
        wv.writeframes(struct.pack("<%dh" % len(pcm), *pcm)); wv.close()
        print("   dekodiert: %d Samples, Abspielrate ~%d Hz (%.3f s)" % (len(pcm), rate, len(pcm) / float(rate)))
    print("--- Wellen bitgleich ueber alle vier Raeume: %s" % ("JA" if len(set(shas.values())) == 1 else "NEIN %s" % shas))

if __name__ == "__main__":
    main()
