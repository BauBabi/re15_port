# -*- coding: utf-8 -*-
"""Gegenpruefung R2 (Linse UMGEHUNG): TEILWEISE Abschwaechungen des Gates, je genau EINE Textstelle.

Die Mutanten-Probe des Bauers (mutanten_voll.py) schaltet ganze Pruefungen ab (befund -> pass,
raise -> pass, Namensregel weg, 18 Hand-Mutanten). Hier dagegen Pruefungen, die noch LAUFEN, aber
nur eine Richtung/einen Teil pruefen - der typische Tippfehler/Denkfehler. Jede Kopie liegt unter
<zielordner>/<name>.py; der Selbsttest der Kopie prueft dann die Kopie selbst (__file__).

Aufruf: r2_mutanten.py <release/apk_asset_gate.py> <zielordner>
"""
import os
import sys

MUTANTEN = (
    # (Name, alt, neu, Beschreibung)
    ("M1_manifest_nur_zu_gross",
     "        elif apk_dateien[pfad] != eintraege[pfad]:\n",
     "        elif apk_dateien[pfad] < eintraege[pfad]:\n",
     "Manifest-Groesse: nur 'Manifest groesser als APK' wird gemeldet, 'kleiner' nicht"),
    ("M9_laenge_nicht_gegen_cd",
     "    if crc != e.crc or n != e.usize:\n",
     "    if crc != e.crc:\n",
     "Eintrag lesen: nur CRC32 gegen das Zentralverzeichnis, die entpackte Laenge nicht"),
    ("M5_eocd_nur_muell_hinten",
     "        if eocd_pos + EOCD_LEN + kom != groesse:\n",
     "        if eocd_pos + EOCD_LEN + kom < groesse:\n",
     "EOCD: nur Bytes HINTER dem Kommentar gemeldet, zu grosse Kommentarlaenge nicht"),
    ("M14_dd_bit_aus_cd",
     "        if not (l_flags & FLAG_DATA_DESCRIPTOR) and (l_crc, l_csize, l_usize) != (e.crc, e.csize, e.usize):\n",
     "        if not (e.flags & FLAG_DATA_DESCRIPTOR) and (l_crc, l_csize, l_usize) != (e.crc, e.csize, e.usize):\n",
     "Data-Descriptor-Bit aus dem Zentralverzeichnis statt aus dem Local Header (libziparchive nimmt LFH)"),
    ("M16_nicht_assets_nur_stored",
     "        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen), key=lambda x: x.lho):\n",
     "        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen and x.methode == 0), key=lambda x: x.lho):\n",
     "uebrige Eintraege (classes.dex, lib/, ...): CRC nur fuer Stored-Eintraege, Deflate uebersprungen"),
)


def main():
    quelle, ziel = sys.argv[1], sys.argv[2]
    text = open(quelle, "r", encoding="utf-8", newline="").read()
    os.makedirs(ziel, exist_ok=True)
    for name, alt, neu, text_ in MUTANTEN:
        # Zeilenenden der Quelle respektieren
        if "\r\n" in text:
            alt_, neu_ = alt.replace("\n", "\r\n"), neu.replace("\n", "\r\n")
        else:
            alt_, neu_ = alt, neu
        n = text.count(alt_)
        if n != 1:
            raise SystemExit("%s: Textstelle %d-mal gefunden (erwartet 1)" % (name, n))
        with open(os.path.join(ziel, name + ".py"), "w", encoding="utf-8", newline="") as f:
            f.write(text.replace(alt_, neu_, 1))
        print("%-30s %s" % (name, text_))
    return 0


if __name__ == "__main__":
    sys.exit(main())
