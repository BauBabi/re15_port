#!/usr/bin/env python3
"""Runde 34 / Granate: SE-Bank-Satz (EDT) und die VAB-Toene dahinter ausgeben.

Aufbau .EDH (wie re15_port/platform/pc/src/audio_pc.c, load_weapon_se_vab_pc):
  [EDT: 4-Byte-Saetze @0 .. pBAV_off) ["pBAV" VH @pBAV_off] [8-Byte-Trailer]
  pBAV_off = u32 @edh[size-8].
Satzfelder so, wie FUN_80045024 sie liest (PSX.EXE):
  rec[0] bit7 -> fp = rec[0]&0x7f (VAB-Handle-Override)       @0x80045148-5c
  rec[1]&0x7f -> Programm                                      @0x80045174/80
  rec[2]>>4  -> Ton-Index,  rec[2]&0xf -> s5                   @0x80045160-68/@0x800451a0
  rec[3]>>5  -> Zusatz-Lagen (Anzahl), rec[3]&0x1f -> Stimme   @0x8004516c/@0x8004517c
VAB (pBAV): Kopf 0x20, 128 Programm-Saetze a 16 B, je Programm 16 Ton-Saetze a 32 B
(ab 0x20+0x800), VAG-Tabelle danach. Ton-Satz +0x16 = VAG-Nummer (1-basiert).

Aufruf: python edh_dump.py <EDH> [satz ...]
"""
import struct, sys

def main():
    p = sys.argv[1]
    b = open(p, "rb").read()
    pbav = struct.unpack_from("<I", b, len(b) - 8)[0]
    n = pbav // 4
    want = [int(x, 0) for x in sys.argv[2:]] or list(range(n))
    vh = b[pbav:]
    assert vh[:4] == b"pBAV", vh[:4]
    ps = struct.unpack_from("<H", vh, 0x12)[0]      # Programme
    ts = struct.unpack_from("<H", vh, 0x14)[0]      # Toene gesamt
    vs = struct.unpack_from("<H", vh, 0x16)[0]      # VAGs
    print("%s: %d B, pBAV @0x%X, EDT-Saetze %d, VAB prog=%d tones=%d vags=%d"
          % (p, len(b), pbav, n, ps, ts, vs))
    prog_base = 0x20
    tone_base = 0x20 + 128 * 16
    for i in want:
        if i >= n:
            print("  Satz 0x%02X: ausserhalb (n=%d)" % (i, n)); continue
        r = b[4 * i: 4 * i + 4]
        prog = r[1] & 0x7f; tone = r[2] >> 4; s5 = r[2] & 0xf
        extra = r[3] >> 5; voice = r[3] & 0x1f
        leer = (r == b"\0\0\0\0")
        line = ("  Satz 0x%02X @Datei 0x%04X: %s  prog=%d ton=%d s5=%d zusatz=%d stimme=%d%s"
                % (i, 4 * i, r.hex(" "), prog, tone, s5, extra, voice,
                   "  (LEER)" if leer else ""))
        print(line)
        if leer:
            continue
        # Programm-Satz: +0 tones (Anzahl Toene)
        pr = vh[prog_base + 16 * prog: prog_base + 16 * prog + 16]
        ntone = pr[0]
        for k in range(extra + 1):
            t = tone + k
            if t >= 16:
                break
            to = tone_base + (prog * 16 + t) * 32
            tr = vh[to: to + 32]
            vag = struct.unpack_from("<H", tr, 0x16)[0]
            print("     Lage %d: prog %d ton %d (prog.tones=%d) -> VAG %d, vol=%d pan=%d center=%d shift=%d min=%d max=%d"
                  % (k, prog, t, ntone, vag, tr[2], tr[3], tr[4], tr[5], tr[6], tr[7]))

if __name__ == "__main__":
    main()
