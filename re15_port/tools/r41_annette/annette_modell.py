#!/usr/bin/env python3
"""Runde 41 Spur AN: Modell der Frau im Kittel (Typ 0x4D, Leon 6000/6030/5090) mit dem Aussehen der RE2-Annette.

    python re15_port/tools/r41_annette/annette_modell.py          # schreibt shared_assets/RE2/ANNETTE_4D*.EMD
    python re15_port/tools/r41_annette/annette_modell.py --pruef  # nur pruefen, ob die Dateien im Repo bytegleich sind

PORT-WAHL (Dossier analysis/befunde_runde41/AN_annette.md Abschnitt 4): Gerippe und Clips bleiben die des RE1.5-Typs 0x4D
(RE1.5 CDEMD0.EMS, Blob von 0x4D, dir[1..6]); Netz (MD1, dir[7]) und Textur (TIM, dir[8]) kommen von der RE2-Annette:

    RE2 CDEMD0.EMS (TOC @0x8009adf4, engine/src/gen/re2_ems_toc.inc)
      kind 0x47 rec 3 (EMD) Sektor 4525, 51252 B  -> MD1 = EMD[dir[7]:dir[8]] (dir @EMD+0, 8 Eintraege)
      kind 0x47 rec 2 (TIM) Sektor 4476, 99872 B  -> ANNETTE_4D.EMD
      kind 0x49 rec 2 (TIM) Sektor 4616, 99872 B  -> ANNETTE_4D_BLUT.EMD (dasselbe EMD byte-gleich, Kittel mit Blut)

Belegt, dass das passt (AN_belege/rig_4d_gegen_re2_47.txt): beide Gerippe 15 Knochen, gleiche Eltern, gleiche Versaetze bis auf
die Unterarme/Haende (z -60/-61 gegen -55/-46). Netz i haengt an Knochen i wie im Port ueblich.

Container wie tools/birkin_formen.py baue(): [u32 dir_off][u32 n=9][Sektionen dir[0..8]][Verzeichnis n*4].
Deterministisch (kein Zeitstempel, keine Zufallszahl).
"""
import argparse, hashlib, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
sys.path.insert(0, os.path.join(PORT, 'tools'))
import emd_ansichtsblatt as EA            # noqa: E402  (EMS-Kette und EMS_ORDER wie re15_ems.c)

RE2_EMS = os.path.join(PORT, 'shared_assets', 'RE2', 'CDEMD0.EMS')
AUS = os.path.join(PORT, 'shared_assets', 'RE2')
TYP = 0x4D
RE2 = {  # kind: (TIM Sektor, Groesse), (EMD Sektor, Groesse)   gen/re2_ems_toc.inc
    0x47: ((4476, 99872), (4525, 51252)),
    0x49: ((4616, 99872), (4665, 51252)),
}
DATEIEN = (('ANNETTE_4D.EMD', 0x47), ('ANNETTE_4D_BLUT.EMD', 0x49))

u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]


def re2_teile(kind):
    ems = open(RE2_EMS, 'rb').read()
    (ts, tl), (es, el) = RE2[kind]
    emd, tim = ems[es * 2048:es * 2048 + el], ems[ts * 2048:ts * 2048 + tl]
    assert len(emd) == el and len(tim) == tl
    do, dc = u32(emd, 0), u32(emd, 4)
    assert dc == 8, 'RE2-EMD traegt 8 Verzeichniseintraege (re2_ems.h)'
    d = list(struct.unpack_from('<%dI' % dc, emd, do)) + [do]
    md1 = emd[d[7]:d[8]]
    assert tim[:4] == b'\x10\x00\x00\x00', 'TIM-Kopf'
    return md1, tim


def baue(kind):
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(TYP))
    blob = ems[off:off + ln]
    d = EA.emd_dir(blob)
    dir_off, n = u32(blob, 0), u32(blob, 4)
    assert dir_off + n * 4 == len(blob) and d[0] == 8 and n == 9
    md1, tim = re2_teile(kind)
    while len(md1) % 4:
        md1 += b'\0'
    vor = blob[8:d[7]]                                   # dir[0..6] unveraendert (EDD/EMR beider Baenke = 0x4D)
    neu_dir = d[:8] + [d[7] + len(md1)]
    neu = vor + md1 + tim
    while (8 + len(neu)) % 4:
        neu += b'\0'
    kopf = struct.pack('<II', 8 + len(neu), n)
    return kopf + neu + struct.pack('<%dI' % n, *neu_dir)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--pruef', action='store_true')
    a = ap.parse_args()
    fehler = 0
    for name, kind in DATEIEN:
        b = baue(kind)
        ziel = os.path.join(AUS, name)
        md5 = hashlib.md5(b).hexdigest()
        if a.pruef:
            alt = open(ziel, 'rb').read() if os.path.exists(ziel) else b''
            ok = alt == b
            fehler += 0 if ok else 1
            print('%-22s %s %s' % (name, md5, 'gleich' if ok else 'ABWEICHEND'))
        else:
            open(ziel, 'wb').write(b)
            print('%-22s %7d B  md5 %s' % (name, len(b), md5))
    return fehler


if __name__ == '__main__':
    sys.exit(main())
