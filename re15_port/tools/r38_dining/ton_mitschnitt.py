#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 38 Spur F: Ton-Mitschnitt (RE15_AUDIO_CAP_SYNC) gegen die Se_on-Zeilen des debug.log auswerten.

  python re15_port/tools/r38_dining/ton_mitschnitt.py <arbeitsordner>

Der Mitschnitt schreibt je Spielbild 1470 Stereo-Frames int16 (audio_pc.c s_cap_ticks; 5880 Byte je Takt). Jede
Port-Ton-Zeile `[se] Se_on bank=0x3B/0x3C/0x3D id=N ... tick=T` (platform/pc/src/dining_r38_pc.c) traegt den Takt T, an
dem die Stimme gestartet wurde. Gemessen wird der Effektivwert (RMS) der 15 Takte ab T gegen die 15 Takte davor —
liegt der Ton im Mix, steigt der Pegel. Ausgabe je Zeile + Zusammenfassung je Bank.
"""
import math
import os
import re
import struct
import sys

TAKT = 1470 * 4   # Byte je Spielbild (2 Kanaele * int16)


def rms(buf):
    n = len(buf) // 2
    if n == 0:
        return 0.0
    v = struct.unpack("<%dh" % n, buf[:n * 2])
    return math.sqrt(sum(x * x for x in v) / n)


def main():
    wd = sys.argv[1]
    cap = open(os.path.join(wd, "cap.raw"), "rb").read()
    log = open(os.path.join(wd, "debug.log"), encoding="utf-8", errors="replace").read()
    zeilen = re.findall(r"\[se\] Se_on bank=0x(3[BCD]) id=(\d+) -> Runde 38 Spur F \(([^)]*)\) F(\d+) tick=(\d+)", log)
    bgm = re.findall(r"\[bgm\] Sce_bgm_control slot=(\d+) op=(\d+) .*capTick=(\d+)", log)
    print("Mitschnitt %d Byte = %d Takte" % (len(cap), len(cap) // TAKT))
    je_bank = {}
    for bank, satz, name, bild, tick in zeilen:
        t = int(tick)
        if (t + 15) * TAKT > len(cap) or t < 15:
            continue
        vor = rms(cap[(t - 15) * TAKT:t * TAKT])
        nach = rms(cap[t * TAKT:(t + 15) * TAKT])
        je_bank.setdefault(bank, []).append((vor, nach))
        print("  Takt %6d Bank 0x%s Satz %2s (%s): RMS vorher %7.1f  nachher %7.1f  x%.2f" %
              (t, bank, satz, name, vor, nach, (nach / vor) if vor > 0 else float("inf")))
    for slot, op, tick in bgm:
        t = int(tick)
        if (t + 60) * TAKT <= len(cap) and t >= 60:
            print("  Takt %6d BGM slot %s op %s: RMS 60 Takte vorher %7.1f  nachher %7.1f" %
                  (t, slot, op, rms(cap[(t - 60) * TAKT:t * TAKT]), rms(cap[t * TAKT:(t + 60) * TAKT])))
    for bank, w in sorted(je_bank.items()):
        lauter = sum(1 for v, n in w if n > v)
        print("Bank 0x%s: %d Toene, %d mit hoeherem Pegel danach, Mittel vorher %.1f nachher %.1f" %
              (bank, len(w), lauter, sum(v for v, _ in w) / len(w), sum(n for _, n in w) / len(w)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
