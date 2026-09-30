# Faelschungen der Referenz-APK (Kopie) mit zipfile umschreiben - Negativ-Kontrollen Runde 34a.
# Aufruf: faelschen.py <quelle.apk> <ziel.apk> <art>
import sys
import zipfile

src, dst, art = sys.argv[1], sys.argv[2], sys.argv[3]
MAN = "assets/re15_assets.txt"
P07G = "assets/shared_assets/RE15DOOR/P07G.DO2"
DOOR04 = "assets/shared_assets/RE2/DOOR/DOOR04.DO2"
PSX = "assets/shared_assets/PSX/SOUND/SUB_00.BGM"
P2DS_REL = "shared_assets/RE15DOOR/P2DS.DO2"
WAV_REL = "synchro/STAGE1/room1170/main00.wav"

geaendert = 0
with zipfile.ZipFile(src) as zi, zipfile.ZipFile(dst, "w") as zo:
    for info in zi.infolist():
        name = info.filename
        daten = zi.read(info)
        if art == "K0_identitaet":
            pass
        elif art == "N1_re15door_entfernt" and name == P07G:
            geaendert += 1
            continue
        elif art == "N2_re2door_ein_byte" and name == DOOR04:
            b = bytearray(daten)
            b[1000] ^= 0x01
            daten = bytes(b)
            geaendert += 1
        elif art == "N3_psx_entfernt" and name == PSX:
            geaendert += 1
            continue
        elif art in ("N5_manifest_groesse", "N6_manifest_zeile_fehlt") and name == MAN:
            zeilen = daten.decode("utf-8").split("\n")
            neu = []
            for z in zeilen:
                if "\t" in z:
                    g, p = z.split("\t", 1)
                    if art == "N5_manifest_groesse" and p == P2DS_REL:
                        z = "%d\t%s" % (int(g) + 1, p)
                        geaendert += 1
                    elif art == "N6_manifest_zeile_fehlt" and p == WAV_REL:
                        geaendert += 1
                        continue
                neu.append(z)
            daten = "\n".join(neu).encode("utf-8")
        zo.writestr(info, daten)
    if art == "N4_zusatz_asset":
        zo.writestr(zipfile.ZipInfo("assets/shared_assets/PSX/ZUSATZ.BIN"), b"nicht aus dem Quellbaum\n",
                    zipfile.ZIP_STORED)
        geaendert += 1
if art != "K0_identitaet" and geaendert != 1:
    sys.exit("Faelschung %s: %d Stellen geaendert statt 1" % (art, geaendert))
print("%s: %d Stelle(n) geaendert" % (art, geaendert))
