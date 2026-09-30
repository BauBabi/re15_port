# -*- coding: utf-8 -*-
"""Pruefer UMGEHUNG R4-1: eigene Ein-Zeilen-Mutanten des Gates (nur Kopien unter build/r34a/pruefer_u1/mut/).
Jeder Mutant: (name, beschreibung, [(alt, neu, erwartete_anzahl)]) - die Textstelle muss GENAU so oft vorkommen.
Sonderfaelle: G0 leere Datei, G4 abgeschnitten vor dem main()-Block (Anweisungsgrenze).
Aufruf: u1_mutanten.py <gate.py> <zielordner> [name ...]   (ohne Namen: alle)"""
import os
import sys

BS = "\\"          # ein Backslash (Heredocs/Werkzeuge verschluckten ihn im ersten Anlauf)

M = [
    # --- H1: Ausgangspfad (die R2-Kampagne mutierte main/Selbsttest ausdruecklich NICHT)
    ("G1_kein_sys_exit", "sys.exit(main()) -> main()  (Exit-Code immer 0)",
     [('if __name__ == "__main__":\n    sys.exit(main())', 'if __name__ == "__main__":\n    main()', 1)]),
    ("G2_rc_abweichung_0", "RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 1, 2 -> 0, 0, 2",
     [("RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 1, 2", "RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 0, 2", 1)]),
    ("G3_main_return_0", "return rc if rc in (RC_GLEICH, RC_ABWEICHUNG) else RC_FEHLER -> return RC_GLEICH",
     [("        return rc if rc in (RC_GLEICH, RC_ABWEICHUNG) else RC_FEHLER", "        return RC_GLEICH", 1)]),
    # --- H4: v2-Teil von manifest_lesen / _pfad_fehler
    ("V1_leerzeile_strip", "manifest_lesen: 'if not z:' -> 'if not z.strip():' (Zeile nur aus Leerraum gilt als leer)",
     [('        z = z.rstrip(b"' + BS + 'r")\n        if not z:\n',
       '        z = z.rstrip(b"' + BS + 'r")\n        if not z.strip():\n', 1)]),
    ("V2_steuer_1f", "_pfad_fehler: 'c < 0x20' -> 'c < 0x1f' (0x1f nicht mehr Steuerzeichen)",
     [("    if any(c < 0x20 or c == 0x7f for c in p):", "    if any(c < 0x1f or c == 0x7f for c in p):", 1)]),
]


def main():
    gate, ziel = sys.argv[1], sys.argv[2]
    nur = set(sys.argv[3:])
    text = open(gate, "rb").read().decode("utf-8")
    assert "\r\n" not in text, "Gate hat CRLF"
    os.makedirs(ziel, exist_ok=True)

    def schreiben(name, t):
        d = os.path.join(ziel, name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, "apk_asset_gate.py"), "wb") as f:
            f.write(t.encode("utf-8"))
        print("%-28s %8d B" % (name, len(t.encode("utf-8"))))

    if not nur or "G0_leer" in nur:
        schreiben("G0_leer", "")
    if not nur or "G4_abgeschnitten_vor_main" in nur:
        i = text.index("\n# =============================================================================================\ndef main(")
        schreiben("G4_abgeschnitten_vor_main", text[:i + 1])
    for name, beschr, ersetzungen in M:
        if nur and name not in nur:
            continue
        t = text
        for alt, neu, n in ersetzungen:
            k = t.count(alt)
            if k != n:
                sys.exit("%s: Stelle %r kommt %d-mal vor, erwartet %d" % (name, alt[:60], k, n))
            t = t.replace(alt, neu)
        schreiben(name, t)
        print("    " + beschr)


if __name__ == "__main__":
    main()
