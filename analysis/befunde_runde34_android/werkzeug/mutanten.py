# Mutanten des Gates erzeugen: jeder schaltet EINE Pruefung ab. Der Selbsttest muss jeden erkennen.
import os
import sys

src = open(sys.argv[1], encoding="utf-8").read()
out = sys.argv[2]
BS = chr(92)
mut = {
    "M1_sha_aus": ("            elif a_sha != q_sha:", "            elif False:"),
    "M2_zusatz_aus": ("            if name != man_name and name not in quellen:", "            if False:"),
    "M3_manifestluecke_aus": ("        if pfad not in eintraege:\n            befund(\"Manifest: Datei fehlt im Manifest\"",
                              "        if False:\n            befund(\"Manifest: Datei fehlt im Manifest\""),
    "M4_cr_nicht_abschneiden": ('        z = z.rstrip("' + BS + 'r")', "        z = z"),
    "M5_gradle_ignoriert": ("    ist = set(gradle_baeume(text))",
                            "    ist = set((b, q, z, tuple(sorted(i))) for b, q, z, i in BAEUME)"),
    "M6_abweichung_als_ok": ('        print("== APK-ASSET-GATE-ABWEICHUNG: %d Befunde ==" % n_befunde)\n        return RC_ABWEICHUNG',
                             '        print("== APK-ASSET-GATE-ABWEICHUNG: %d Befunde ==" % n_befunde)\n        return RC_GLEICH'),
    "M7_crc_als_gleich": ("            except (zipfile.BadZipFile, zlib.error, EOFError) as e:\n                befund(\"APK: beschaedigt (CRC)\", \"APK-Eintrag beschaedigt (CRC/Entpacken): %s: %s\" % (name, e))\n                continue",
                          "            except (zipfile.BadZipFile, zlib.error, EOFError) as e:\n                gleich[name] = info.file_size\n                continue"),
    "M8_pflicht_aus": ("        if n == 0:\n            befund(\"Quellbaum\", \"Pflichtinhalt fehlt",
                       "        if False:\n            befund(\"Quellbaum\", \"Pflichtinhalt fehlt"),
}
for k, (a, b) in mut.items():
    n = src.count(a)
    if n != 1:
        sys.exit("Muster fuer %s %d-mal gefunden" % (k, n))
    with open(os.path.join(out, k + ".py"), "w", encoding="utf-8", newline="\n") as f:
        f.write(src.replace(a, b))
print("Mutanten geschrieben:", len(mut))
