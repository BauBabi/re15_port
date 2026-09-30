# -*- coding: utf-8 -*-
"""Gegenpruefung R3 (Linse UMGEHUNG): Mutanten des Gates AUSSERHALB der Klassen der Mutanten-Probe des
Bauers (A befund->pass, B raise->pass, C Namensregel, D Hand, E Vergleichsrichtung/Grenze, F and/or-Operand,
G Tupelpaar, H not, I Zahl +-1). Hier: Zeichenklassen, strip-Varianten, Zeilentrennung, Konstanten,
Methodenmenge - je GENAU EINE Textstelle (Treffer muss eindeutig sein).

Aufruf: r3_mutanten.py <gate.py> <zielordner>   -> <zielordner>/<name>/apk_asset_gate.py
Nur mit echtem Python starten (/c/Python310/python).
"""
import os
import sys

MUTANTEN = (
    # (Name, alt, neu, Beschreibung)
    ("MU0_sha_vergleich_aus",
     "            if a_sha != q_sha:\n                befund(\"Inhalt weicht ab (sha256)\"",
     "            if False:\n                befund(\"Inhalt weicht ab (sha256)\"",
     "Auftragsbeispiel: sha256-Vergleich Quelle/APK abgeschaltet (else zaehlt dann alles als gleich)"),
    ("MU1_manifest_rstrip_alles",
     "        z = z.rstrip(\"\\r\")                          # :200 nur angehaengte '\\r'",
     "        z = z.rstrip()                              # :200 nur angehaengte '\\r'",
     "Manifestzeile: rstrip() statt rstrip('\\r') - schneidet auch Leerzeichen/Tab/FF/VT ab (Geraet nur '\\r')"),
    ("MU2_groesse_unicode_ziffern",
     "        if not re.fullmatch(r\"[0-9]+\", groesse):",
     "        if not re.fullmatch(r\"\\d+\", groesse):",
     "Groessenfeld: \\d statt [0-9] - nimmt Unicode-Ziffern (int() rechnet sie um, atoll liest 0)"),
    ("MU3_splitlines",
     "    zeilen = text.split(\"\\n\")                       # android_glue.c:197 (strchr '\\n')",
     "    zeilen = text.splitlines()                       # android_glue.c:197 (strchr '\\n')",
     "Manifest: splitlines() statt split('\\n') - trennt auch an \\r, \\x0b, \\x0c, \\x1c-\\x1e, \\x85, U+2028/9"),
    ("MU4_manifest_max_64GiB",
     "MANIFEST_MAX = 64 << 20",
     "MANIFEST_MAX = 64 << 30",
     "Konstante: 64 GiB statt 64 MiB (Selbsttest prueft die Grenze nur ueber den Pruefhaken)"),
    ("MU5_methode_bis_8",
     "        if e.methode not in (0, 8):",
     "        if e.methode > 8:",
     "Methode: jede <= 8 lesbar (Geraet behandelt alles ausser 8 als Stored)"),
    ("MU6_groesse_isdigit",
     "        if not re.fullmatch(r\"[0-9]+\", groesse):",
     "        if not groesse.isdigit():",
     "Groessenfeld: str.isdigit() statt [0-9]+ (Unicode-Ziffern, hochgestellte Ziffern)"),
    ("MU7_kopf_strip",
     "    kopf = zeilen[0].rstrip(\"\\r\")                  # :163 sscanf am Pufferanfang",
     "    kopf = zeilen[0].strip()                        # :163 sscanf am Pufferanfang",
     "Kopfzeile: strip() statt rstrip('\\r')"),
    ("MU8_pfad_strip",
     "        groesse, pfad = z.split(\"\\t\", 1)",
     "        groesse, pfad = z.split(\"\\t\", 1)\n        pfad = pfad.strip()",
     "Manifestpfad zusaetzlich gestrippt (Leerzeichen am Pfadende/-anfang)"),
)


def main():
    gate, ziel = sys.argv[1], sys.argv[2]
    text = open(gate, "r", encoding="utf-8", newline="").read()
    for name, alt, neu, _besch in MUTANTEN:
        n = text.count(alt)
        if n != 1:
            print("FEHLER %s: Stelle %d-mal gefunden" % (name, n))
            continue
        d = os.path.join(ziel, name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, "apk_asset_gate.py"), "w", encoding="utf-8", newline="") as f:
            f.write(text.replace(alt, neu))
        print("%-30s geschrieben (%s)" % (name, _besch))
    return 0


if __name__ == "__main__":
    sys.exit(main())
