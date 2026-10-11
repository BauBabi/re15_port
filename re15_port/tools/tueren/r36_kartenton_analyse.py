#!/usr/bin/env python3
"""r36_kartenton_analyse.py - Runde 36 Spur G (Punkt 3): Zeitprofil der RE2-Aufschliess-Wellen.

Frage des Nutzers: "Tueren die erfolgreich geoeffnet wurden mit Karte oder sowas machen in Resident Evil 2
erst ein 'piep' Sound und dann ein unlock Geraeusch." - Welche RE2-Welle(n) traegt das, und liegen Piep und
Aufschliessen in EINER Welle oder in zwei Saetzen?

Nur LESEND (info/re2leon/PL0/RDT). Je Welle: Rate nach dem Port-Weg (note2pitch2 wie
analysis/befunde_runde30/tools/r30_re2_wellen_wav.py), dann je 20-ms-Fenster
  pegel  = RMS in dB relativ zur Spitze der Welle,
  f_peak = Frequenz der staerksten FFT-Linie,
  flach  = spektrale Flachheit (geom./arith. Mittel des Leistungsspektrums; ~0 = Ton, ~1 = Rauschen).
Ein "Piep" ist ein stehender Ton (flach klein, f_peak ueber mehrere Fenster gleich), ein Schloss-Geraeusch ein
breitbandiger Anschlag (flach gross, kurz). Das sind MESSWERTE, kein Hoerbefund.

Aufruf: python r36_kartenton_analyse.py [--wav <dir>]   (schreibt optional die WAVs zum Anhoeren)
"""
import argparse
import os
import struct
import sys
import wave

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde30", "tools"))
from r30_re2_roombank import load_bank, resolve          # noqa: E402
from r30_re2_wellen_wav import note2pitch2, vag_decode   # noqa: E402

WELLEN = [
    # (raum, satz, wozu)
    ("60C0", 0x25, "EXE-Tuer-Handler @0x80051658 Se_on(2,0x25): Kartentuer key_type 0x61 -> ROOM6100"),
    ("6140", 0x25, "EXE-Tuer-Handler @0x80051658 Se_on(2,0x25): Kartentuer key_type 0x61 -> ROOM6130"),
    ("2110", 0x0c, "ROOM2110 sub08 Karte angenommen, 1. Se_on"),
    ("2110", 0x0d, "ROOM2110 sub08 Karte angenommen, 2. Se_on"),
    ("2110", 0x0b, "ROOM2110 sub08 Karte angenommen, 3. Se_on"),
    ("2110", 0x25, "ROOM2110 Satz 0x25 (Schluesseltuer aot 1 -> 2150, key 0x5c)"),
    ("1140", 0x25, "EXE @0x80051658: Schluessel benutzt (key 0x59)"),
    ("1050", 0x25, "EXE @0x80051658: Schluessel benutzt (key 0x58)"),
    ("2040", 0x25, "EXE @0x80051658: Schluessel benutzt (key 0x59)"),
    ("6030", 0x25, "EXE @0x80051658: Schluessel benutzt (key 0x5f)"),
    ("6060", 0x25, "EXE @0x80051658: Schluessel benutzt (key 0x62)"),
    ("60B0", 0x25, "Satz 0x25 (Tuer key_type 0xff)"),
    ("7000", 0x25, "EXE @0x80051658: Schluessel benutzt (key 0x63)"),
    ("20A0", 0x26, "EXE @0x800515f8: von dieser Seite entriegelt"),
]


def welle(raum, satz):
    p = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM%s.RDT" % raum)
    b = load_bank(p)
    r = resolve(b, satz)
    if r is None or r["empty"]:
        return None
    L = r["layers"][0]
    if "vag_off" not in L:          # VAG-Index ausserhalb der Bank (z.B. Satz zeigt ins Leere)
        return None
    raw = b["d"][L["vag_off"]:L["vag_off"] + L["vag_size"]]
    pcm = np.asarray(vag_decode(raw), np.float64)
    pitch = note2pitch2(L["mn"], L["shift"], L["center"], L["shift"])
    rate = 44100 * pitch // 4096
    return dict(pcm=pcm, rate=rate, L=L, r=r, lagen=len(r["layers"]))


def profil(pcm, rate, fenster_ms=20):
    n = max(64, int(rate * fenster_ms / 1000))
    spitze = np.max(np.abs(pcm)) or 1.0
    zeilen = []
    for i in range(0, len(pcm) - n + 1, n):
        x = pcm[i:i + n] * np.hanning(n)
        rms = np.sqrt(np.mean(pcm[i:i + n] ** 2))
        db = 20 * np.log10(max(rms, 1e-9) / spitze)
        P = np.abs(np.fft.rfft(x)) ** 2 + 1e-12
        f = np.fft.rfftfreq(n, 1.0 / rate)
        k = int(np.argmax(P[1:]) + 1)
        flach = float(np.exp(np.mean(np.log(P[1:]))) / np.mean(P[1:]))
        zeilen.append((i / rate, db, f[k], flach))
    return zeilen


def segmente(zeilen, schwelle_db=-30.0):
    """Zusammenhaengende Abschnitte ueber der Schwelle; je Abschnitt Mittel der Flachheit und die haeufigste
    Spitzenfrequenz (Ton = stehende Linie)."""
    seg, akt = [], []
    for z in zeilen:
        if z[1] >= schwelle_db:
            akt.append(z)
        elif akt:
            seg.append(akt); akt = []
    if akt:
        seg.append(akt)
    aus = []
    for s in seg:
        fl = float(np.mean([z[3] for z in s]))
        fs = [round(z[2] / 50) * 50 for z in s]
        werte, zahl = np.unique(fs, return_counts=True)
        aus.append(dict(t0=s[0][0], t1=s[-1][0] + 0.02, flach=fl, f=float(werte[np.argmax(zahl)]),
                        f_anteil=float(np.max(zahl)) / len(s), spitze_db=max(z[1] for z in s)))
    return aus


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--wav", default="")
    ap.add_argument("--profil", default="", help="RAUM:SATZ (hex) - volles 20-ms-Profil ausgeben")
    a = ap.parse_args()
    if a.wav:
        os.makedirs(a.wav, exist_ok=True)
    for raum, satz, wozu in WELLEN:
        w = welle(raum, satz)
        if w is None:
            print("ROOM%s 0x%02x LEER  | %s" % (raum, satz, wozu))
            continue
        L = w["L"]
        dauer = len(w["pcm"]) / w["rate"]
        print("ROOM%s 0x%02x sha1 %s %5d B rate %5d Hz %.3f s vol %3d Lagen %d | %s" % (
            raum, satz, L["sha1"][:8], L["vag_size"], w["rate"], dauer, L["vol"], w["lagen"], wozu))
        for s in segmente(profil(w["pcm"], w["rate"])):
            art = "TON " if s["flach"] < 0.08 and s["f_anteil"] >= 0.6 else "BREIT"
            print("    %.3f..%.3f s  %s  flach %.3f  f_peak %5.0f Hz (%.0f %% der Fenster)  spitze %5.1f dB" % (
                s["t0"], s["t1"], art, s["flach"], s["f"], 100 * s["f_anteil"], s["spitze_db"]))
        if a.wav:
            fn = os.path.join(a.wav, "re2_%s_%02x_%s.wav" % (raum, satz, L["sha1"][:8]))
            with wave.open(fn, "wb") as f:
                f.setnchannels(1); f.setsampwidth(2); f.setframerate(max(1000, w["rate"]))
                f.writeframes(struct.pack("<%dh" % len(w["pcm"]), *[int(v) for v in w["pcm"]]))
        if a.profil and a.profil.upper() == "%s:%02X" % (raum, satz):
            for t, db, fp, fl in profil(w["pcm"], w["rate"]):
                print("      t %.3f  %6.1f dB  f %5.0f  flach %.3f" % (t, db, fp, fl))


if __name__ == "__main__":
    main()
