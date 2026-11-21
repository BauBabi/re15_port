#!/usr/bin/env python3
"""Strang riegel (21.11.2026): erzeugt tests/unit/daten/psxspx_spu_gauss_auszug.md fuer den Riegel unit_r42_ton_gauss.

Warum: der Riegel las die Gauss-Tabelle aus info/Resident_Evil_und_Playstation_Information/psx-spx.github.io-master/docs/
soundprocessingunitspu.md - ausserhalb von re15_port. In einem Arbeitsbaum ohne diesen Ordner (Sparse-Checkout, z. B. der
Integrationsbaum r41_sichel) war er deshalb rot ("nicht lesbar"), ohne dass am Spiel etwas falsch war.
Der Auszug ist WOERTLICH der Abschnitt der Hardware-Beschreibung von "The Gauss table contains the following values" bis
einschliesslich der Zeile "The PSX table is a bit different ..." (psx-spx Z. 225-292), davor ein Kopf mit Quelle und
Pruefsummen. Der Riegel parst ihn mit demselben Leser wie vorher (512 Werte), unabhaengig vom Port-Kopf re15_spu_gauss.h.

Aufruf (Repo-Wurzel):  python re15_port/tools/r42_ton/gauss_auszug_psxspx.py           -> schreibt den Auszug
                       python re15_port/tools/r42_ton/gauss_auszug_psxspx.py --pruefen  -> vergleicht nur (exit 1 bei Abweichung)
"""
import hashlib
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HIER, '..', '..', '..'))
QUELLE_REL = 'info/Resident_Evil_und_Playstation_Information/psx-spx.github.io-master/docs/soundprocessingunitspu.md'
ZIEL = os.path.join(REPO, 're15_port', 'tests', 'unit', 'daten', 'psxspx_spu_gauss_auszug.md')
ANFANG = 'The Gauss table contains the following values'
ENDE = 'The PSX table is a bit different'


def auszug_bauen():
    roh = open(os.path.join(REPO, QUELLE_REL), 'rb').read()
    zeilen = roh.split(b'\n')
    a = next(i for i, z in enumerate(zeilen) if ANFANG.encode() in z)
    e = next(i for i, z in enumerate(zeilen) if i > a and ENDE.encode() in z)
    koerper = b'\n'.join(zeilen[a:e + 1]) + b'\n'
    kopf = (
        '<!-- ERZEUGT von re15_port/tools/r42_ton/gauss_auszug_psxspx.py - nicht von Hand aendern.\n'
        f'     Quelle: {QUELLE_REL}\n'
        f'     Zeilen {a + 1}-{e + 1} woertlich (psx-spx "4-Point Gaussian Interpolation", Gauss-Tabelle 512 Werte)\n'
        f'     SHA-256 der ganzen Quelldatei: {hashlib.sha256(roh).hexdigest()}\n'
        f'     SHA-256 des Auszugs (ab der naechsten Zeile): {hashlib.sha256(koerper).hexdigest()}\n'
        '     Leser: re15_port/tests/unit/test_r42_ton_gauss.c tabelle_lesen() -->\n'
    ).encode()
    return kopf + koerper


def main():
    neu = auszug_bauen()
    if '--pruefen' in sys.argv:
        alt = open(ZIEL, 'rb').read() if os.path.exists(ZIEL) else b''
        print('gleich' if alt == neu else 'ABWEICHUNG')
        return 0 if alt == neu else 1
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    open(ZIEL, 'wb').write(neu)
    print(f'geschrieben: {ZIEL} ({len(neu)} Bytes)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
