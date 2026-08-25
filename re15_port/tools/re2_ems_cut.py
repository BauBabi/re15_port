#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""re2_ems_cut.py — schneidet RE2-Gegner-Chunks byte-true aus CDEMD0/CDEMD1.EMS.

⛔ WARUM DIESES SKRIPT EXISTIERT
Die bereits im Repo liegenden Dateien `info/re2leon/PL0/PLD/CDEMD0/EM0xx.{EMD,TIM}` sind
ein SEKTORVERSCHOBENER Naiv-Split: ihre Groessen sind alle 2048-Vielfache, die echten
Records nicht. Gemessen: `EM036.TIM` liegt dort bei Sektor 3127, die TOC sagt 3468;
`EM036.EMD` bei 3192 gegen 3533, und enthaelt ab 0xB800 einen fremden Overlay-Chunk.
Wer aus diesen Dateien misst, misst NICHT den Typ, dessen Namen sie tragen.

AUTORITATIV ist allein die Gegner-TOC in der RE2-EXE:
    Basis 0x8009ADF4 (Datei-Offset 0x8B5F4), 8 Byte je Eintrag = [u32 Sektor][u32 Groesse],
    VIER Eintraege je Typ ab 0x10, Index = (typ - 0x10) * 4 + k:
        k=0  KI-Overlay, gegen 0x8010D000 gelinkt
        k=1  KI-Overlay, gegen 0x80100000 gelinkt
        k=2  TIM (Textur)
        k=3  EMD (Modell + Skelett + Animation)
    Zweite TOC 0x8009B880 fuer CDEMD1.EMS; Selektor 0x800CFC00, CD-Datei-Ids 469/470
    (`beq v1,14` @0x8001b970 -> `addiu v0,zero,470` @0x8001ba14).

BEWEIS fuer k=2 = TIM und k=3 = EMD (nicht umgekehrt):
    Typ 0x20 k=2 = 66592 B  == Groesse von COMMON/BIN/EM_TYPE20.TIM
    Typ 0x20 k=3 = 121144 B == Groesse von COMMON/BIN/EM_TYPE20.EMD
BEWEIS fuer die Sektor-Arithmetik: die fuenf lose in COMMON/BIN liegenden Dateien sind
md5-identisch mit den Chunks an ihren TOC-Sektoren:
    EMZ0.BIN=26/53068 · EMD0G_MOD0.BIN=1206/22266 · EMOVL21_S0.BIN=1320/19080
    EMS25.BIN=1825/26324 · EMS26.BIN=1901/4346

Aufruf:
    python re15_port/tools/re2_ems_cut.py                 # alle Typen, beide Archive
    python re15_port/tools/re2_ems_cut.py 0x36 0x37       # nur diese Typen
    python re15_port/tools/re2_ems_cut.py --list          # nur die TOC ausgeben
Ziel: build/extracted/re2_ems/  (Extraktions-Intermediates, nicht shared_assets)
"""
import os
import struct
import sys
import hashlib

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
ARCHIVES = [
    ("CDEMD0", os.path.join(REPO, "info", "re2leon", "PL0", "PLD", "CDEMD0.EMS"), 0x8009ADF4),
    ("CDEMD1", os.path.join(REPO, "info", "re2leon", "PL0", "PLD", "CDEMD1.EMS"), 0x8009B880),
]
OUT = os.path.join(REPO, "build", "extracted", "re2_ems")
KIND = {0: "ai0", 1: "ai1", 2: "tim", 3: "emd"}
EXT = {0: "BIN", 1: "BIN", 2: "TIM", 3: "EMD"}


def exe_reader():
    d = open(EXE, "rb").read()
    taddr = struct.unpack_from("<I", d, 0x18)[0]
    return d, (lambda a: 0x800 + (a - taddr))


def toc(d, fo, base, typ):
    """Die vier Records eines Typs. Rueckgabe: [(k, sektor, groesse), ...]"""
    out = []
    for k in range(4):
        idx = (typ - 0x10) * 4 + k
        sec, size = struct.unpack_from("<II", d, fo(base + idx * 8))
        out.append((k, sec, size))
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    only_list = "--list" in sys.argv
    types = [int(a, 0) for a in args] if args else list(range(0x10, 0x60))

    d, fo = exe_reader()
    if not only_list:
        if not os.path.isdir(OUT):
            os.makedirs(OUT)

    for name, path, base in ARCHIVES:
        if not os.path.isfile(path):
            print("%s fehlt: %s" % (name, path))
            continue
        blob = open(path, "rb").read()
        nsec = len(blob) // 2048
        print("== %s: %d Byte = %d Sektoren, TOC @0x%08X ==" % (name, len(blob), nsec, base))
        for typ in types:
            recs = toc(d, fo, base, typ)
            # Ein Typ gilt als in DIESEM Archiv vorhanden, wenn alle vier Records
            # vollstaendig innerhalb der Datei liegen und keine Groesse 0 ist.
            ok = all(size > 0 and sec * 2048 + size <= len(blob) for _, sec, size in recs)
            if not ok:
                continue
            for k, sec, size in recs:
                off = sec * 2048
                chunk = blob[off:off + size]
                tag = "%s_EM%02X_%s.%s" % (name, typ, KIND[k], EXT[k])
                md5 = hashlib.md5(chunk).hexdigest()
                print("   0x%02X k=%d  Sektor %5d  Offset 0x%08X  %8d B  md5 %s  -> %s"
                      % (typ, k, sec, off, size, md5[:12], tag))
                if not only_list:
                    open(os.path.join(OUT, tag), "wb").write(chunk)
    if not only_list:
        print("\nZiel: %s" % OUT)


if __name__ == "__main__":
    main()
