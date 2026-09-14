#!/usr/bin/env python3
"""Laengen ALLER synchro-Aufnahmen, gerechnet WIE DER PORT.

wav_find_data (re15_port/platform/pc/src/audio_pc.c:1463): chunk-walk ab 12; die
data-Groesse wird UNSIGNED gegen die tatsaechlich vorhandenen Bytes geklemmt
(avail = sz - offset(body)).  Dann re15_voice_load_clip (audio_pc.c:1590ff):
   16-bit PCM  -> src_n = (dbytes/2)/ch
   sonst       -> frames = dbytes/(bits/8 * ch)
   out_n       = src_n * 44100 / rate   (int64)
Dauer (s) = out_n / 44100 ; Bilder = Dauer * 30.
"""
import os, struct, sys, glob


def find_data(b):
    sz = len(b)
    rate, ch, bits, dbytes, fmt = 22050, 1, 16, 0, 1
    raw_csz = None
    if sz < 44 or b[0:4] != b'RIFF' or b[8:12] != b'WAVE':
        return None
    p = 12
    data_off = None
    while p + 8 <= sz:
        csz = struct.unpack_from('<I', b, p + 4)[0]
        body = p + 8
        if b[p:p + 4] == b'fmt ' and body + 16 <= sz:
            fmt = struct.unpack_from('<H', b, body + 0)[0]
            ch = struct.unpack_from('<H', b, body + 2)[0]
            rate = struct.unpack_from('<I', b, body + 4)[0]
            bits = struct.unpack_from('<H', b, body + 14)[0]
            if fmt == 0xFFFE and csz >= 40 and body + 26 <= sz:
                fmt = struct.unpack_from('<H', b, body + 24)[0]
        elif b[p:p + 4] == b'data':
            data_off = body
            raw_csz = csz
            avail = sz - body
            dbytes = avail if csz > avail else csz
            break
        p += 8 + csz + (csz & 1)
    if data_off is None:
        return None
    return dict(rate=rate, ch=ch, bits=bits, dbytes=dbytes, fmt=fmt,
                raw_csz=raw_csz, avail=sz - data_off, filesize=sz)


def scan():
    rows = []
    for path in sorted(glob.glob('synchro/STAGE*/room*/main*.wav')):
        b = open(path, 'rb').read()
        info = find_data(b)
        parts = os.path.normpath(path).split(os.sep)
        stage = parts[1]
        room = parts[2][4:]
        msg = int(parts[3][4:6])
        if info is None:
            rows.append((stage, room, msg, path, None,
                         'KEIN RIFF/WAVE oder kein data-Chunk'))
            continue
        rate, ch, bits, dbytes, fmt = (info['rate'], info['ch'], info['bits'],
                                       info['dbytes'], info['fmt'])
        if fmt == 1 and bits == 16:
            src_n = (dbytes // 2) // ch
            ok = True
        else:
            bps = bits // 8
            src_n = dbytes // (bps * ch) if bps and ch else 0
            ok = src_n > 0 and ((fmt == 1 and bits in (8, 24, 32)) or
                                (fmt == 3 and bits == 32))
        out_n = src_n * 44100 // rate if rate else 0
        sec = out_n / 44100.0
        frames30 = sec * 30.0
        defekt = ''
        if info['raw_csz'] is not None and info['raw_csz'] > info['avail']:
            defekt = 'data-Groesse 0x%08X > vorhanden %d B (geklemmt)' % (
                info['raw_csz'], info['avail'])
        if not ok:
            defekt += ' FORMAT-ABLEHNUNG (fmt=%d bits=%d ch=%d)' % (fmt, bits, ch)
        rows.append((stage, room, msg, path,
                     dict(rate=rate, ch=ch, bits=bits, fmt=fmt, dbytes=dbytes,
                          out_n=out_n, sec=sec, frames=frames30), defekt))
    return rows


if __name__ == '__main__':
    rows = scan()
    print("%-7s %-6s %3s %9s %2s %2s %3s %10s %8s %7s  %s" %
          ("STAGE", "ROOM", "MSG", "rate", "ch", "bi", "fmt", "data-B", "Sek",
           "Bilder", "Defekt"))
    for stage, room, msg, path, i, d in rows:
        if i is None:
            print("%-7s %-6s %3d  %s" % (stage, room, msg, d))
            continue
        print("%-7s %-6s %3d %9d %2d %2d %3d %10d %8.3f %7.1f  %s" %
              (stage, room, msg, i['rate'], i['ch'], i['bits'], i['fmt'],
               i['dbytes'], i['sec'], i['frames'], d))
    print("\nDateien: %d" % len(rows))
