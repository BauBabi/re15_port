# -*- coding: utf-8 -*-
"""Die kaputte data-Groesse der Sprachaufnahmen heilen - verlustfrei und geprueft.

68 der Aufnahmen unter synchro/STAGE*/ tragen data-Groesse 0xFFFFFFFF (und RIFF-Groesse
ebenso): ein ffmpeg-Streaming-Artefakt. Der Loader faengt das heute ab, indem er gegen die
tatsaechlich vorhandenen Bytes klemmt (wav_find_data, audio_pc.c) - aber die Klemme zaehlt
ALLES hinter `data` als Audio. Kaeme je ein Export mit nachlaufendem Chunk dazu, waere die
Laenge zu gross, und der neue Sprach-Riegel wuerde entsprechend zu lange warten.

Geheilt wird mit `ffmpeg -c copy` (Datei-Muxer statt Stream-Muxer schreibt echte Groessen).

JEDE Datei wird einzeln geprueft, BEVOR sie ersetzt wird:
  - Format identisch (Kanaele, Rate, Bit-Tiefe)
  - PCM-Rumpf BYTEGLEICH zum bisher geklemmten Bereich
  - die neue data-Groesse == der bisher geklemmte Wert
Schlaegt eine Pruefung fehl, bleibt die Datei unangetastet und wird gemeldet.
"""
import io
import os
import struct
import subprocess
import sys
import glob
import shutil

FFMPEG = r'C:/ProgramData/chocolatey/bin/ffmpeg.exe'
TMP = os.environ.get('WAVTMP', '.')


def lies(p):
    d = io.open(p, 'rb').read()
    if d[:4] != b'RIFF' or d[8:12] != b'WAVE':
        return None
    i = d.find(b'fmt ')
    j = d.find(b'data')
    if i < 0 or j < 0:
        return None
    ch, = struct.unpack_from('<H', d, i + 10)
    rate, = struct.unpack_from('<I', d, i + 12)
    bits, = struct.unpack_from('<H', d, i + 22)
    dsz, = struct.unpack_from('<I', d, j + 4)
    riff, = struct.unpack_from('<I', d, 4)
    avail = len(d) - (j + 8)
    n = min(dsz, avail)
    return {'pcm': d[j + 8:j + 8 + n], 'dsz': dsz, 'riff': riff, 'avail': avail,
            'ch': ch, 'rate': rate, 'bits': bits, 'gesamt': len(d)}


def main():
    dateien = sorted(glob.glob('synchro/STAGE*/room*/*.wav'))
    defekt = []
    for f in dateien:
        a = lies(f)
        if a is None:
            print('  UNLESBAR (uebersprungen): %s' % f)
            continue
        if a['dsz'] == 0xFFFFFFFF or a['riff'] == 0xFFFFFFFF:
            defekt.append(f)
    print('%d Aufnahmen unter synchro/STAGE*/, davon %d mit kaputter Groesse'
          % (len(dateien), len(defekt)))
    if not defekt:
        return 0

    geheilt = probleme = 0
    for f in defekt:
        alt = lies(f)
        tmp = os.path.join(TMP, 'heil_' + os.path.basename(f))
        r = subprocess.run([FFMPEG, '-hide_banner', '-loglevel', 'error', '-y',
                            '-i', f, '-c', 'copy', tmp],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if r.returncode != 0 or not os.path.exists(tmp):
            print('  FFMPEG-FEHLER  %s: %s' % (f, r.stderr.decode('utf-8', 'replace')[:120]))
            probleme += 1
            continue
        neu = lies(tmp)
        grund = None
        if neu is None:
            grund = 'Ergebnis nicht lesbar'
        elif (neu['ch'], neu['rate'], neu['bits']) != (alt['ch'], alt['rate'], alt['bits']):
            grund = ('Format geaendert: %dkan/%dHz/%dbit -> %dkan/%dHz/%dbit'
                     % (alt['ch'], alt['rate'], alt['bits'], neu['ch'], neu['rate'], neu['bits']))
        elif neu['pcm'] != alt['pcm']:
            grund = ('PCM NICHT bytegleich (%d gegen %d Bytes)'
                     % (len(alt['pcm']), len(neu['pcm'])))
        elif neu['dsz'] != len(alt['pcm']):
            grund = ('neue data-Groesse %d passt nicht zum geklemmten Wert %d'
                     % (neu['dsz'], len(alt['pcm'])))
        if grund:
            print('  NICHT ERSETZT  %-44s %s' % (f, grund))
            probleme += 1
            os.remove(tmp)
            continue
        shutil.move(tmp, f)
        geheilt += 1

    print('\n%d geheilt, %d unangetastet (Problem)' % (geheilt, probleme))
    return 1 if probleme else 0


if __name__ == '__main__':
    sys.exit(main())
