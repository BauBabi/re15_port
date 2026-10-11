#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""knall_zensus.py — Runde 36 Spur K (K1): Zensus der RE2-Raumtoene, die als TUERKNALL / EINBRUCH in
Frage kommen (Nutzer-Liste RE2_SOUND_LISTE.md Z.7-8: "Door sounds" ROOM207/209/20A, "Zombies smashing
through wood" ROOM205, Library-Einbruch ROOM112, Cabin "Zombies breaking in" ROOM106, Shutter ROOM204).

Je Raum (info/re2leon/PL0/RDT/ROOM<nnnn>.RDT, RE2 Retail Leon):
  * Raumbank snd0 = RDT-Kopfworte +0x08/+0x0C/+0x10 (EDT/VH/VB; glas1090_extrakt.py, FUN_80059e54),
  * jeder nicht leere EDT-Satz: Programm/Basis-Tone/Extra-Lagen (re15_edt_decode-Felder, FUN_800453d0),
    VAG je Lage, Laenge, Abspielrate (Ton-Satz center/min note, 0x1000 = 44100 Hz), Dauer, Spitze, RMS,
    Huellkurve 50 ms, Anstieg (Bild des Huellkurven-Maximums), Schwerpunkt-Frequenz,
  * jede Stelle im SCD, die den Satz mit Se_on (Bank 2) spielt (r30_re2_scd.py-Walker).
Aufruf: RE15_INFO_DIR=<Hauptbaum>/info/Resident_Evil_und_Playstation_Information \
        python knall_zensus.py [raum ...] [--wav <dir>]   (Default: 2050 1060 1120 2070 2090 20A0 2040)
"""
import os, struct, sys, wave
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDTDIR = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde30", "tools"))
_cwd = os.getcwd(); os.chdir(REPO)
import r30_re2_scd as SCD   # noqa: E402  (Walker; braucht RE15_INFO_DIR im schlanken Baum)
os.chdir(_cwd)

VAB_HDR, VAB_PROG_TABLE, VAB_TONE_SEG = 32, 128 * 16, 16 * 32
F0 = [0, 60, 115, 98, 122]
F1 = [0, 0, -52, -55, -60]


def adpcm(b):
    """PSX-SPU-ADPCM (psx-spx 'SPU ADPCM'): 16-B-Block = Shift/Filter, Flags, 14 B = 28 Abtastwerte.
    Endflag (Flags & 1) beendet die Welle (wie re15_vag_adpcm_decode)."""
    out = []; s1 = s2 = 0
    for o in range(0, len(b) - 15, 16):
        sf, fl = b[o], b[o + 1]
        sh, fi = sf & 0x0f, min((sf >> 4) & 0x0f, 4)
        for k in range(28):
            nib = (b[o + 2 + k // 2] >> (4 * (k & 1))) & 0x0f
            v = (nib << 12) & 0xffff
            if v & 0x8000: v -= 0x10000
            v >>= sh
            v += (s1 * F0[fi] + s2 * F1[fi] + 32) >> 6
            v = max(-32768, min(32767, v))
            out.append(v); s2, s1 = s1, v
        if fl & 1 and o > 0: break
    return np.array(out, dtype=np.float64)


def vab(d, vh):
    if d[vh:vh + 4] != b"pBAV": raise ValueError("kein pBAV @0x%X" % vh)
    nprog = struct.unpack_from("<H", d, vh + 18)[0]
    tones = {}
    for p in range(128):
        pe = vh + VAB_HDR + p * 16
        nt = d[pe]
        if nt == 0: continue
        for t in range(16):
            ro = vh + VAB_HDR + VAB_PROG_TABLE + (p * 16 + t) * 32
            if ro + 32 > len(d): break
            r = d[ro:ro + 32]
            tones[(p, t)] = dict(vol=r[2], pan=r[3], center=r[4], fine=r[5], mn=r[6], mx=r[7],
                                 vag=struct.unpack_from("<H", r, 22)[0], ro=ro)
    szo = vh + VAB_HDR + VAB_PROG_TABLE + nprog * VAB_TONE_SEG
    sizes = [struct.unpack_from("<H", d, szo + 2 * i)[0] * 8 for i in range(256)]
    return nprog, tones, sizes


def kennzahlen(pcm, rate):
    n = len(pcm)
    if n == 0: return {}
    win = max(1, int(rate * 0.05))
    env = [float(np.sqrt((pcm[i:i + win] ** 2).mean())) for i in range(0, n, win)]
    spec = np.abs(np.fft.rfft(pcm * np.hanning(n)))
    fr = np.fft.rfftfreq(n, 1.0 / rate)
    cent = float((spec * fr).sum() / max(spec.sum(), 1e-9))
    lo = float((spec[fr < 300] ** 2).sum() / max((spec ** 2).sum(), 1e-9))
    imax = int(np.argmax(env))
    # Abklingen: erstes 50-ms-Fenster nach dem Maximum unter 10 % des Maximums
    abkl = next((i for i in range(imax, len(env)) if env[i] < 0.1 * env[imax]), len(env))
    return dict(dauer=n / rate, spitze=int(np.abs(pcm).max()), rms=float(np.sqrt((pcm ** 2).mean())),
                env=[int(e) for e in env], anstieg_ms=imax * 50, abkl_ms=(abkl - imax) * 50,
                schwerpunkt=cent, anteil_unter_300=lo)


def se_on_stellen(d):
    out = {}
    for which, nm in ((16, "main"), (17, "sub")):
        for i, s, e in SCD.blocks(d, which):
            recs, st = SCD.walk(d, s, e)
            for off, op, r in recs:
                if op == 0x36 and r[1] == 2:
                    out.setdefault(r[2], []).append("%s%02d@0x%05X" % (nm, i, off))
    return out


def raum(nr, wavdir=None):
    p = os.path.join(RDTDIR, "ROOM%s.RDT" % nr)
    d = open(p, "rb").read()
    edt, vh, vb = struct.unpack_from("<III", d, 8)
    # EDT-Laenge = Abstand zur VH (je Satz 4 B)
    n_edt = (vh - edt) // 4 if vh > edt else 32
    nprog, tones, sizes = vab(d, vh)
    stellen = se_on_stellen(d)
    print("== ROOM%s  EDT @0x%05X (%d Saetze)  VH @0x%05X (%d Programme)  VB @0x%05X" % (nr, edt, n_edt, vh, nprog, vb))
    for sid in range(n_edt):
        e = d[edt + 4 * sid:edt + 4 * sid + 4]
        if e[2] == 0 and e[3] == 0: continue
        prog, tone, extra = e[1] & 0x7f, e[2] >> 4, e[3] >> 5
        voice = (e[3] & 0x1f) - 0x10
        lagen = []
        for k in range(extra + 1):
            t = tones.get((prog, tone + k))
            if not t or t["vag"] <= 0: break
            v = t["vag"]
            off = sum(sizes[1:v]); sz = sizes[v]
            w = d[vb + off:vb + off + sz]
            pcm = adpcm(w)
            rate = 44100.0 * 2 ** ((t["mn"] - t["center"]) / 12.0)
            kz = kennzahlen(pcm, rate)
            lagen.append((tone + k, v, vb + off, sz, t, rate, kz, pcm))
        use = ",".join(stellen.get(sid, [])) or "-"
        print("  Satz 0x%02x  EDT @0x%05X `%s`  Prog %d Tone %d +%d Lage(n) Stimme %d  Se_on: %s" % (
            sid, edt + 4 * sid, e.hex(" "), prog, tone, extra, voice, use))
        for (tn, v, fo, sz, t, rate, kz, pcm) in lagen:
            print("     Tone %d -> VAG %d @Datei 0x%05X %5d B  vol %3d pan %3d center %3d min %3d -> %5.0f Hz  "
                  "%.2f s  Spitze %5d RMS %5.0f  Anstieg %d ms  Abklingen %d ms  Schwerpunkt %4.0f Hz  <300Hz %.2f" % (
                      tn, v, fo, sz, t["vol"], t["pan"], t["center"], t["mn"], rate, kz["dauer"], kz["spitze"],
                      kz["rms"], kz["anstieg_ms"], kz["abkl_ms"], kz["schwerpunkt"], kz["anteil_unter_300"]))
            print("        Huellkurve 50 ms: %s" % " ".join(str(x) for x in kz["env"][:30]))
            if wavdir:
                os.makedirs(wavdir, exist_ok=True)
                fn = os.path.join(wavdir, "ROOM%s_se%02x_t%d_vag%d.wav" % (nr, sid, tn, v))
                with wave.open(fn, "wb") as wf:
                    wf.setnchannels(1); wf.setsampwidth(2); wf.setframerate(int(round(rate)))
                    wf.writeframes(np.clip(pcm, -32768, 32767).astype("<i2").tobytes())


if __name__ == "__main__":
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    wavdir = None
    if "--wav" in sys.argv:
        wavdir = sys.argv[sys.argv.index("--wav") + 1]
        args = [a for a in args if a != wavdir]
    for nr in (args or ["2050", "1060", "1120", "2070", "2090", "20A0", "2040"]):
        raum(nr, wavdir)
