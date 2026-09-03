#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Ausfuehrungsbit im Linux-ZIP setzen und nachweisen.

WARUM DAS NOETIG IST (gemessen 2026-09-03 am Paket v0.3.97):
Im ausgelieferten Linux-Archiv lag `re15_pc` mit Modus 0644. Wer das Paket auf dem Deck
entpackt und das Binary direkt startet - oder es direkt als Nicht-Steam-Spiel eintraegt,
statt ueber run.sh zu gehen - bekommt "Permission denied" und sonst nichts.

Die Ursache liegt NICHT im Paketbauer, sondern im Bauhost: das Arbeitsverzeichnis liegt
auf einem Windows-Dateisystem, das MSYS ohne ACLs einbindet. Dort ist `chmod` wirkungslos,
und MSYS leitet das Ausfuehrungsbit stattdessen aus Shebang bzw. Dateiendung ab. `run.sh`
kommt deshalb zufaellig richtig heraus (`#!`), ein ELF-Binary ohne Endung nie. Ein
`install -m 755` im Paketbauer laeuft damit ins Leere, ohne zu scheitern - der Fehler ist
unsichtbar, solange niemand IN das Archiv schaut.

Der Modus einer Datei steht im ZIP nicht im lokalen Kopf, sondern in den oberen 16 Bit
des Feldes `external_attr` im ZENTRALVERZEICHNIS. Bei Split-Archiven liegt das
Zentralverzeichnis komplett im LETZTEN Volume - das ist die `.zip` (die `.z01` ist das
erste Volume). Genau dort wird gesetzt, nichts anderes wird angefasst; Dateidaten, CRCs
und Offsets bleiben unberuehrt.

Aufruf:
    zip_exec_bit.py setzen <archiv.zip> <pfad-im-archiv> [...]
    zip_exec_bit.py pruefen <archiv.zip> <pfad-im-archiv> [...]

`pruefen` ist das Gate: es liest den Modus aus demselben Feld zurueck und faellt mit
Exit 1, wenn eine der genannten Dateien nicht ausfuehrbar ist. Ohne dieses Gate kehrt der
Fehler beim naechsten Bau still zurueck.
"""
import struct
import sys

ZENTRAL = b'PK\x01\x02'
UNIX = 3                      # "version made by": 3 = Unix, sonst gilt der Modus nicht


def eintraege(roh):
    """Alle Zentralverzeichnis-Records als (offset, name) liefern."""
    aus = []
    p = 0
    while True:
        p = roh.find(ZENTRAL, p)
        if p < 0:
            return aus
        n_name, n_extra, n_kom = struct.unpack_from('<HHH', roh, p + 28)
        name = roh[p + 46:p + 46 + n_name].decode('utf-8', 'replace')
        aus.append((p, name))
        p += 46 + n_name + n_extra + n_kom


def treffer(name, muster):
    """Ein Muster passt auf den vollen Pfad oder auf den Dateinamen am Ende."""
    return name == muster or name.endswith('/' + muster)


def main(argv):
    if len(argv) < 4 or argv[1] not in ('setzen', 'pruefen'):
        sys.stderr.write(__doc__)
        return 2
    modus, archiv, muster = argv[1], argv[2], argv[3:]

    with open(archiv, 'rb') as f:
        roh = bytearray(f.read())

    gefunden = {m: False for m in muster}
    schlecht = []
    geaendert = 0

    for off, name in eintraege(roh):
        passend = [m for m in muster if treffer(name, m)]
        if not passend:
            continue
        gefunden[passend[0]] = True
        made_by, = struct.unpack_from('<H', roh, off + 4)
        attr, = struct.unpack_from('<I', roh, off + 38)
        rechte = (attr >> 16) & 0xFFFF

        if modus == 'setzen':
            neu = (rechte | 0o111) if rechte else 0o100755
            if (made_by >> 8) != UNIX:
                # Ohne Unix-Herkunft ignoriert unzip den Modus - also mitsetzen.
                struct.pack_into('<H', roh, off + 4, (UNIX << 8) | (made_by & 0xFF))
            if neu != rechte:
                struct.pack_into('<I', roh, off + 38,
                                 (neu << 16) | (attr & 0xFFFF))
                geaendert += 1
            print("   x-Bit: %s  %04o -> %04o" % (name, rechte, neu))
        else:
            if (made_by >> 8) != UNIX or not (rechte & 0o111):
                schlecht.append("%s (made_by=%d, Modus %04o)"
                                % (name, made_by >> 8, rechte))
            else:
                print("   x-Bit ok: %s  %04o" % (name, rechte))

    fehlt = [m for m, ok in gefunden.items() if not ok]
    if fehlt:
        sys.stderr.write("   FEHLER: im Archiv nicht gefunden: %s\n" % ", ".join(fehlt))
        return 1
    if schlecht:
        sys.stderr.write("   FEHLER: ohne Ausfuehrungsbit im Archiv: %s\n"
                         % "; ".join(schlecht))
        return 1
    if modus == 'setzen':
        if geaendert:
            with open(archiv, 'wb') as f:
                f.write(roh)
        print("   %d Eintrag/Eintraege im Zentralverzeichnis angepasst" % geaendert)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
