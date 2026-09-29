#!/usr/bin/env python3
"""tuer_g12_ton.py - Runde 33 / Thema T, G12 (Durchgang ohne Tuerblatt): Ton der objektlosen
RE2-Tuerarchive DOOR20/21/32/34/36 dekodieren und vermessen (Huellkurve, Einsaetze), damit die Wahl
des Archivs fuer die vier G12-Uebergaenge belegt statt geraten ist.

    python re15_port/tools/tueren/tuer_g12_ton.py        -> build/r33_tueren/g12/*.wav, g12_ton.json, g12_huelle.png

Quellen: Tonteil wie re2_tuer_ton.py (Kopf 16 B, VH @Nachspann 0xC30, VB @0xC38; Tonsatz VH+0x820+prog*0x200+
ton*0x20, VAG-Nummer Satz+22, Rate aus note2pitch2 @0x80083010 / Tabelle @0x800aba40). PSX-ADPCM wie
re15_port/engine/src/vab_common.c re15_vag_adpcm_decode (psx-spx "SPU ADPCM": 16-B-Block, Byte 0 =
Filter<<4 | Shift, Byte 1 = Flags, 28 Nibbles; Filter f0 = 0,60,115,98,122 / f1 = 0,0,-52,-55,-60, /64).
Se_on-Bild je Archiv aus dem Katalog-Simulator (tuerkatalog.analyse_variant), Bildtakt 59,826/s.
"""
import json
import os
import struct
import sys
import wave

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
sys.path.insert(0, HIER)
import tuerkatalog as tk      # noqa: E402
import re2_tuer_ton as rt     # noqa: E402

AUS = os.path.join(REPO, "build", "r33_tueren", "g12")
F0 = (0, 60, 115, 98, 122)
F1 = (0, 0, -52, -55, -60)


def vag_dekodieren(b):
    out, s1, s2 = [], 0, 0
    for blk in range(len(b) // 16):
        k = b[blk * 16:blk * 16 + 16]
        sh, fi, fl = k[0] & 15, (k[0] >> 4) & 7, k[1]
        fi = min(fi, 4)
        for i in range(28):
            nib = (k[2 + i // 2] >> (4 * (i & 1))) & 15
            if nib >= 8:
                nib -= 16
            v = (nib << 12) >> sh
            v += (s1 * F0[fi] + s2 * F1[fi] + 32) >> 6
            v = max(-32768, min(32767, v))
            out.append(v)
            s2, s1 = s1, v
        if fl & 1:
            break
    return np.array(out, np.int16)


def ton0(idx, lut):
    door = tk.Door(idx)
    d = door.data
    ent = tk.exe_rd(tk.DOOR_TABLE + idx * 12, 12)
    ton_gr = struct.unpack_from("<H", ent, 0)[0]
    ton = d[:ton_gr]
    nach = struct.unpack_from("<I", ton, 0xC30)[0]
    vh = ton[nach:0xC30]
    vb = ton[0xC38:]
    nprog = struct.unpack_from("<H", vh, 18)[0]
    groessen = [struct.unpack_from("<H", vh, 2080 + nprog * 512 + 2 * i)[0] * 8 for i in range(256)]
    off, pos = {}, 0
    for i, g in enumerate(groessen):
        if g:
            off[i] = (pos, g)
            pos += g
    e = ton[0:4]
    prog, tone = e[1] & 0x7F, e[2] >> 4
    t = vh[2080 + prog * 512 + tone * 32:2080 + prog * 512 + tone * 32 + 32]
    center, shift, note = t[4], t[5], t[6]
    vag = struct.unpack_from("<h", t, 22)[0]
    pitch = rt.note2pitch2(lut, note, shift, center, shift)
    rate = 44100.0 * pitch / 4096.0
    vo, g = off[vag]
    return vag_dekodieren(vb[vo:vo + g]), rate, dict(vol=t[2], note=note, center=center, vag=vag)


def huelle(x, rate, fen=0.02):
    n = max(1, int(rate * fen))
    k = len(x) // n
    r = np.sqrt((x[:k * n].astype(np.float64).reshape(k, n) ** 2).mean(1))
    return r


def einsaetze(r, fen=0.02):
    """Einsaetze = lokale Anstiege: r[i] > 2 * Mittel(r[i-5..i-1]) und r[i] > 10 % des Maximums,
    mindestens 0,15 s Abstand."""
    m = r.max()
    out, letzte = [], -99
    for i in range(5, len(r)):
        vor = r[i - 5:i].mean()
        if r[i] > 0.1 * m and r[i] > 2.0 * max(vor, 1e-6) and (i - letzte) * fen >= 0.15:
            out.append(round(i * fen, 2))
            letzte = i
    return out


def main():
    os.makedirs(AUS, exist_ok=True)
    lut = list(struct.unpack("<192H", tk.exe_rd(0x800ABA40, 384)))
    res = {}
    kurven = {}
    for idx in (0x20, 0x21, 0x32, 0x34, 0x36):
        x, rate, info = ton0(idx, lut)
        vm, _ = tk.analyse_variant(tk.Door(idx), 0)
        se = [s["tick"] for s in vm.sounds]
        r = huelle(x, rate)
        kurven[idx] = r / max(r.max(), 1)
        ein = einsaetze(r)
        # Spektralschwerpunkt je Einsatz-Fenster (0,1 s)
        spek = np.abs(np.fft.rfft(x.astype(np.float64)))
        fr = np.fft.rfftfreq(len(x), 1 / rate)
        schwer = float((spek * fr).sum() / max(spek.sum(), 1))
        name = "DOOR%02X" % idx
        with wave.open(os.path.join(AUS, name + "_ton0.wav"), "wb") as w:
            w.setnchannels(1); w.setsampwidth(2); w.setframerate(int(round(rate))); w.writeframes(x.tobytes())
        res[name] = dict(rate_hz=round(rate, 1), dauer_s=round(len(x) / rate, 3), se_on_bilder=se,
                         bilder=len(vm.frames), einsaetze_s=ein, n_einsaetze=len(ein),
                         schwerpunkt_hz=round(schwer), pegel_spitze=int(np.abs(x).max()), **info,
                         huelle_20ms=[round(float(v), 3) for v in kurven[idx][::5]])
        print(name, {k: v for k, v in res[name].items() if k != "huelle_20ms"})
    json.dump(res, open(os.path.join(AUS, "g12_ton.json"), "w"), indent=1)
    try:
        from PIL import Image, ImageDraw
        W, H = 1000, 110
        im = Image.new("RGB", (W, H * 5), (16, 16, 16))
        dr = ImageDraw.Draw(im)
        for j, (idx, k) in enumerate(kurven.items()):
            y0 = j * H
            dr.text((4, y0 + 2), "DOOR%02X Ton 0 (je 20 ms, 0..5 s)" % idx, fill=(255, 255, 0))
            for i, v in enumerate(k):
                x = int(i * 0.02 / 5.0 * W)
                dr.line([(x, y0 + H - 4), (x, y0 + H - 4 - int(v * (H - 20)))], fill=(120, 220, 120))
        im.save(os.path.join(AUS, "g12_huelle.png"))
    except ImportError:
        pass
    return 0


if __name__ == "__main__":
    sys.exit(main())
