# -*- coding: utf-8 -*-
"""Nachbesserung R4-1: Ein-Zeilen-Mutanten der NEUEN Regeln (H5/U3) und der Pruefer-Mutanten V1/V2 gegen das
Gate des Arbeitsbaums. Kopien unter build/r34a/nb/mut2/<name>/apk_asset_gate.py; je Mutant muss die Textstelle
genau so oft vorkommen wie angegeben. Aufruf: nb_mutanten.py <gate.py> <zielordner>"""
import os
import sys

BS = chr(92)
M = [
    ("V1_leerzeile_strip", [('        z = z.rstrip(b"' + BS + 'r")\n        if not z:\n',
                             '        z = z.rstrip(b"' + BS + 'r")\n        if not z.strip():\n', 1)]),
    ("V2_steuer_1f", [("    if any(c < 0x20 or c == 0x7f for c in p):", "    if any(c < 0x1f or c == 0x7f for c in p):", 1)]),
    ("V3_del_7e", [("    if any(c < 0x20 or c == 0x7f for c in p):", "    if any(c < 0x20 or c == 0x7e for c in p):", 1)]),
    ("N1_ascii_grenze_80", [("    if any(c > 0x7f for c in p):", "    if any(c > 0x80 for c in p):", 1)]),
    ("N2_ascii_aus", [("    if any(c > 0x7f for c in p):", "    if False:", 1)]),
    ("N3_segment_252", [("    if lang > SEGMENT_MAX:", "    if lang > SEGMENT_MAX + 1:", 1)]),
    ("N4_segment_aus", [("    if lang > SEGMENT_MAX:", "    if False:", 1)]),
    ("N5_segment_konstante", [("SEGMENT_MAX = 251 ", "SEGMENT_MAX = 255 ", 1)]),
    ("N6_quellpfad_aus", [("        grund = _pfad_fehler(roh)\n        if grund:\n            befund(\"Quellbaum: Pfad\"",
                           "        grund = None\n        if grund:\n            befund(\"Quellbaum: Pfad\"", 1)]),
    ("N7_quelldublette_aus", [("    for gruppe in sorted(v for v in klein.values() if len(v) > 1):\n        befund(\"Quellbaum: Pfad\"",
                               "    for gruppe in sorted(v for v in klein.values() if len(v) > 2):\n        befund(\"Quellbaum: Pfad\"", 1)]),
    ("N8_quellpfad_nicht_gerufen", [("    quellpfade_pruefen(dateien, befund)\n", "    pass\n", 1)]),
]


def main():
    gate, ziel = sys.argv[1], sys.argv[2]
    text = open(gate, "rb").read().decode("utf-8")
    assert "\r\n" not in text, "Gate hat CRLF"
    for name, ersetzungen in M:
        t = text
        for alt, neu, n in ersetzungen:
            k = t.count(alt)
            if k != n:
                sys.exit("%s: Stelle %r kommt %d-mal vor, erwartet %d" % (name, alt[:70], k, n))
            t = t.replace(alt, neu)
        d = os.path.join(ziel, name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, "apk_asset_gate.py"), "wb") as f:
            f.write(t.encode("utf-8"))
        print("%-28s %8d B" % (name, len(t.encode("utf-8"))))


if __name__ == "__main__":
    main()
