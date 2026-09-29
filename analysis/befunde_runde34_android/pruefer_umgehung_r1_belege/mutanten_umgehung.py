# -*- coding: utf-8 -*-
"""Weitere Mutanten des Gates (Gegenpruefung Umgehung, Runde 1) - je EINE Pruefung abgeschwaecht.

Frage: faengt `--selbsttest` jede Abschwaechung, deren Pruefung NICHT durch eine andere ersetzt wird?
Aufruf: mutanten_umgehung.py <release/apk_asset_gate.py> <ausgabeordner>
"""
import os
import sys

src = open(sys.argv[1], encoding="utf-8").read().replace("\r\n", "\n")
out = sys.argv[2]
os.makedirs(out, exist_ok=True)

CRC_KLASSE = '''
class _Crc:                                     # MUTANT U4: CRC32 statt sha256 (schneller, kollisionsschwach)
    def __init__(self):
        self.c = 0
    def update(self, b):
        self.c = zlib.crc32(b, self.c)
    def hexdigest(self):
        return "%08x" % self.c

'''

mut = {
    # U1: Manifest-Zeile ohne APK-Eintrag wird nicht mehr gemeldet ("Geisterzeile")
    "U1_geisterzeile_aus": [(
        '            befund("Manifest: Zeile ohne APK-Eintrag",\n'
        '                   "Manifest nennt %s (%d B), die APK hat keinen Eintrag assets/%s" % (pfad, eintraege[pfad], pfad))',
        '            pass')],
    # U2: doppelte Manifest-Zeile: spaetere ueberschreibt still die fruehere
    "U2_doppelzeile_aus": [(
        '        if pfad in eintraege:\n            befund("Manifest", "Manifest nennt %s mehrfach',
        '        if False:\n            befund("Manifest", "Manifest nennt %s mehrfach')],
    # U3: Inhalt nur ueber den ersten 1-MiB-Block verglichen (beide Seiten)
    "U3_nur_erster_block": [(
        '            h.update(b)\n            n += len(b)\n',
        '            h.update(b)\n            n += len(b)\n            if n >= BLOCK:\n                return h.hexdigest(), n\n')],
    # U4: CRC32 statt sha256 (APK-Eintrag wird weiter gelesen -> zipfile-CRC-Pruefung bleibt)
    "U4_crc_statt_sha256": [
        ("class Bedienfehler(Exception):", CRC_KLASSE.lstrip("\n") + "class Bedienfehler(Exception):"),
        ("    h, n = hashlib.sha256(), 0\n", "    h, n = _Crc(), 0\n")],
    # U5: Verzeichniseintraege nicht mehr gesondert gemeldet
    "U5_verzeichnis_aus": [(
        '            if info.is_dir():\n                befund("APK: doppelt/Verzeichnis", "Verzeichniseintrag in der APK: %s" % name)\n                continue\n',
        '')],
    # U6: Kommentarzeilen ab Zeile 2 still uebersprungen
    "U6_kommentarzeile_aus": [(
        '            if nr != 1:\n                befund("Manifest", "Manifest-Zeile %d: unerwartete Kommentarzeile',
        '            if False:\n                befund("Manifest", "Manifest-Zeile %d: unerwartete Kommentarzeile')],
    # U7: Groessenfeld nicht mehr auf reine Ziffern geprueft (int() nimmt ' 12', '+12', '1_2')
    "U7_groessenfeld_frei": [(
        '        if not re.fullmatch(r"[0-9]+", groesse):',
        '        if False:')],
    # U8: Manifest-Hoechstgroesse nicht geprueft
    "U8_64mib_aus": [(
        '    if len(roh) > MANIFEST_MAX:', '    if False:')],
}

for k, ersetzungen in mut.items():
    t = src
    for a, b in ersetzungen:
        n = t.count(a)
        erwartet = 2 if k in ("U3_nur_erster_block", "U4_crc_statt_sha256") and a.startswith(("            h.update", "    h, n")) else 1
        if n != erwartet:
            sys.exit("Muster fuer %s %d-mal gefunden (erwartet %d)" % (k, n, erwartet))
        t = t.replace(a, b)
    with open(os.path.join(out, k + ".py"), "w", encoding="utf-8", newline="\n") as f:
        f.write(t)
print("Mutanten geschrieben: %d -> %s" % (len(mut), out))
