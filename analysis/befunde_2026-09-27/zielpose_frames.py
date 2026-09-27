#!/usr/bin/env python3
"""MESSUNG (Welle 2): Muendungshoehe Bone 11 BILD FUER BILD je Clip.

Gleiche Vorwaertskinematik wie zielpose_fk.py (dieselben Quellen, dieselben Zahlen);
hier nur eine andere Ausgabe: je Clip die Hoehe jedes Bildes, damit die HALTE-Pose
(letztes Bild) von der Bewegung unterschieden werden kann.

usage: zielpose_frames.py <PLD-Verzeichnis> <W-Bank> <clip,clip,...>
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import zielpose_fk as Z  # noqa: E402  (nutzt dessen Parser + FK unveraendert)


def main():
    pld = sys.argv[1]
    bank = sys.argv[2]
    skel = Z.parse_emr(os.path.join(pld, 'PL00.EMR'))
    wemr = Z.parse_emr(os.path.join(pld, bank + '.EMR'))
    clips, frames = Z.parse_edd(os.path.join(pld, bank + '.EDD'))
    want = ([int(x) for x in sys.argv[3].split(',')] if len(sys.argv) > 3
            else list(range(len(clips))))
    print("Bank %s: %d Clips" % (bank, len(clips)))
    for ci in want:
        if ci >= len(clips):
            print("Clip %d: fehlt" % ci)
            continue
        first, n = clips[ci]
        hs = []
        for f in range(n):
            kfi = frames[first + f] & 0xfff
            if kfi >= wemr['kf_count']:
                hs.append(None)
                continue
            y, _py = Z.bone11_y(skel, wemr['kf'], wemr['kf_sz'], kfi)
            hs.append(-y)
        print("Clip %-3d n=%-3d erstes=%s letztes=%s  alle=%s"
              % (ci, n, hs[0] if hs else '-', hs[-1] if hs else '-',
                 ','.join(str(h) for h in hs)))


main()
