# -*- coding: utf-8 -*-
"""Mini-Faelle (Gegenpruefung Umgehung, Runde 1): Mini-Repo + Mini-APK ueber die Fixture des Gates
(_Fall aus release/apk_asset_gate.py), dann gefaelscht; jedes Gate (echt + Mutanten) als eigener Prozess.

Aufruf: minifaelle_umgehung.py <release/apk_asset_gate.py> <arbeitsordner> [<mutant.py> ...]
Ausgabe: je Fall und Gate die Rueckgabe + die erste Befundzeile.
"""
import importlib.util
import os
import shutil
import struct
import subprocess
import sys
import zipfile

gate_pfad = os.path.abspath(sys.argv[1])
wurzel = os.path.abspath(sys.argv[2])
mutanten = [os.path.abspath(m) for m in sys.argv[3:]]
hier = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, hier)
import umgehung_werkzeug as uw                       # noqa: E402  rohe ZIP-Patches

spec = importlib.util.spec_from_file_location("gate", gate_pfad)
g = importlib.util.module_from_spec(spec)
spec.loader.exec_module(g)
_Fall = g._Fall

P07 = "assets/shared_assets/RE15DOOR/P07G.DO2"
P2DS_REL = "shared_assets/RE15DOOR/P2DS.DO2"


def man(f, zeilen_extra_vorne=(), zeilen_extra_hinten=(), kopf=None, ersetzen=None):
    """Manifest im Schreiberformat, optional mit Zusatzzeilen/geaendertem Kopf."""
    z = sorted("%d\t%s" % (gr, p) for p, gr in f.manifest_aus_eintraegen())
    n, s = len(z), sum(gr for _p, gr in f.manifest_aus_eintraegen())
    if ersetzen:
        z = [ersetzen(x) for x in z]
    z = list(zeilen_extra_vorne) + z + list(zeilen_extra_hinten)
    k = kopf if kopf is not None else "# re15 assets %d %d" % (n, s)
    return (k + "\n" + "\n".join(z) + "\n").encode("utf-8")


def faelle():
    def geist(f):             # Manifest-Zeile fuer eine Datei, die es weder in der APK noch im Quellbaum gibt
        z = f.manifest_aus_eintraegen()
        n, s = len(z) + 1, sum(gr for _p, gr in z) + 5
        f.manifest_roh = man(f, zeilen_extra_hinten=["5\tshared_assets/RE15DOOR/GEIST.DO2"],
                             kopf="# re15 assets %d %d" % (n, s))

    def doppel(f):            # P2DS.DO2 zweimal: erste Zeile mit FALSCHER Groesse (Geraet: Entpack-Fehler)
        f.manifest_roh = man(f, zeilen_extra_vorne=["9999\t" + P2DS_REL])

    def verzeichnis(f):
        f.nach_schreiben = lambda: _anhaengen(f.apk, "assets/shared_assets/RE15DOOR/", b"")

    def kommentar_versteckt(f):   # Zeile auskommentiert: Geraet ueberspringt sie -> P2DS nie entpackt
        f.manifest_roh = man(f, ersetzen=lambda x: "#" + x if x.endswith("\t" + P2DS_REL) else x)

    def kommentar_harmlos(f):     # reine Notizzeile: Geraet ueberspringt sie, schadet nicht
        f.manifest_roh = man(f, zeilen_extra_hinten=["# Notiz"])

    def unterstrich(f):           # Groessenfeld '1_800': int() -> 1800, atoll() -> 1 (Geraet: Fehler)
        f.manifest_roh = man(f, ersetzen=lambda x: "1_800\t" + P2DS_REL if x.endswith("\t" + P2DS_REL) else x)

    def leerzeilen(f):            # Leerzeilen dazwischen: Geraet ueberspringt sie (harmlos)
        f.manifest_roh = man(f, ersetzen=lambda x: x + "\n\n")

    def crcrlf(f):                # '\r\r\n': Geraet schneidet ALLE angehaengten '\r' ab (harmlos)
        f.manifest_roh = man(f).replace(b"\n", b"\r\r\n")

    def tab_im_pfad(f):
        f.manifest_roh = man(f, ersetzen=lambda x: x.replace("RE15DOOR/P2DS", "RE15DOOR\tP2DS"))

    def leerzeichen_hinten(f):
        f.manifest_roh = man(f, ersetzen=lambda x: x + " " if x.endswith(P2DS_REL) else x)

    def nul_zeile(f):             # Geraet: strchr/Schleife enden am NUL -> alles danach nie entpackt
        f.manifest_roh = man(f, ersetzen=lambda x: x + "\n\x00" if x.endswith("\t" + P2DS_REL) else x)

    def backslash(f):
        f.nach_schreiben = lambda: _roh(f.apk, "backslash", P07)

    def nul_name(f):
        f.nach_schreiben = lambda: _roh(f.apk, "nul", P07)

    def lfh_crc(f):
        f.nach_schreiben = lambda: _roh(f.apk, "lfh_crc", P07)

    def verschluesselt(f):        # Bit 0 (verschluesselt) in LFH+CD eines Asset-Eintrags
        f.nach_schreiben = lambda: _flag(f.apk, P07, 0x1)

    def methode_99(f):            # unbekannte Kompressionsmethode im CD
        f.nach_schreiben = lambda: _methode(f.apk, P07, 99)

    def quelle_nur_unterordner(f):    # RE15DOOR enthaelt nur einen leeren Unterordner
        for rel in [r for r in f.quelle if r.startswith("re15_port/shared_assets/RE15DOOR/")]:
            del f.quelle[rel]
        f.eintraege = [e for e in f.eintraege if not e[0].startswith("assets/shared_assets/RE15DOOR/")]
        f.leere_ordner.append("re15_port/shared_assets/RE15DOOR/sub")

    def synchro_stage_datei(f):   # synchro/STAGEX.txt (Datei auf oberster Ebene, passt auf STAGE*/**)
        f.quelle["synchro/STAGEX.txt"] = b"notiz"

    def gradle_named(f):          # zusaetzlicher Baum AUSSERHALB des register-Blocks (tasks.named)
        f.gradle += ('\ntasks.named("stageAssets") {\n'
                     '    from(new File(portRoot, "shared_assets/NEU")) { into "shared_assets/NEU" }\n}\n')
        f.quelle["re15_port/shared_assets/NEU/X.BIN"] = b"neu"
        f.eintraege.append(["assets/shared_assets/NEU/X.BIN", b"neu", zipfile.ZIP_STORED])

    def gradle_named_leer(f):     # dito, aber der neue Baum liefert (noch) nichts
        f.gradle += ('\ntasks.named("stageAssets") {\n'
                     '    from(new File(portRoot, "shared_assets/NEU")) { into "shared_assets/NEU" }\n}\n')

    return [
        ("A1 Manifest: Geisterzeile (weder APK noch Quelle), Kopf angepasst", geist),
        ("A2 Manifest: Doppelzeile, erste mit falscher Groesse", doppel),
        ("A3 APK: Verzeichniseintrag assets/.../RE15DOOR/", verzeichnis),
        ("A4 Manifest: P2DS-Zeile auskommentiert", kommentar_versteckt),
        ("A5 Manifest: harmlose Notizzeile '# Notiz'", kommentar_harmlos),
        ("A6 Manifest: Groessenfeld '1_800'", unterstrich),
        ("A7 Manifest: Leerzeilen dazwischen", leerzeilen),
        ("A8 Manifest: Zeilenende \\r\\r\\n", crcrlf),
        ("A9 Manifest: Tab im Pfad", tab_im_pfad),
        ("A10 Manifest: Leerzeichen am Pfadende", leerzeichen_hinten),
        ("A11 Manifest: NUL-Zeile nach P2DS", nul_zeile),
        ("A12 APK: P07G.DO2 mit '\\' im Namen", backslash),
        ("A13 APK: P07G.DO2 mit NUL-Anhang im Namen", nul_name),
        ("A14 APK: P07G.DO2 LFH-CRC falsch (CD richtig)", lfh_crc),
        ("A15 APK: P07G.DO2 Verschluesselungsbit", verschluesselt),
        ("A16 APK: P07G.DO2 Methode 99 im CD", methode_99),
        ("A17 Quelle: RE15DOOR nur leerer Unterordner", quelle_nur_unterordner),
        ("A18 Quelle: synchro/STAGEX.txt (Quelle+APK)", synchro_stage_datei),
        ("A19 build.gradle: tasks.named-Baum mit Inhalt", gradle_named),
        ("A20 build.gradle: tasks.named-Baum ohne Inhalt", gradle_named_leer),
    ]


def _anhaengen(apk, name, daten):
    with zipfile.ZipFile(apk, "a") as zf:
        zf.writestr(zipfile.ZipInfo(name), daten, zipfile.ZIP_STORED)


def _roh(apk, art, eintrag):
    tmp = apk + ".tmp"
    shutil.move(apk, tmp)
    uw.forge(tmp, apk, art, eintrag)
    os.remove(tmp)


def _flag(apk, eintrag, bit):
    z = uw.cd_lesen(apk)
    e = uw.finde(z, eintrag)
    uw.patch(apk, e["lho"] + 6, struct.pack("<H", e["lfh_flags"] | bit))
    uw.patch(apk, e["cd_pos"] + 8, struct.pack("<H", e["flags"] | bit))


def _methode(apk, eintrag, m):
    z = uw.cd_lesen(apk)
    e = uw.finde(z, eintrag)
    uw.patch(apk, e["cd_pos"] + 10, struct.pack("<H", m))
    uw.patch(apk, e["lho"] + 8, struct.pack("<H", m))


def main():
    if os.path.isdir(wurzel):
        shutil.rmtree(wurzel)
    os.makedirs(wurzel)
    gates = [("ECHT", gate_pfad)] + [(os.path.basename(m)[:-3], m) for m in mutanten]
    for nr, (titel, faelschen) in enumerate(faelle(), 1):
        w = os.path.join(wurzel, "a%02d" % nr)
        os.makedirs(w)
        f = _Fall(w)
        f.nach_schreiben = None
        faelschen(f)
        f.schreiben()
        if f.nach_schreiben:
            f.nach_schreiben()
        teile = []
        for label, gp in gates:
            r = subprocess.run([sys.executable, gp, "--repo", f.repo, f.apk], stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL, timeout=120)
            aus = r.stdout.decode("utf-8", "replace")
            erste = ""
            for z in aus.splitlines():
                if z.startswith("      ") or z.startswith("ABBRUCH") or "Error" in z:
                    erste = z.strip()[:110]
                    break
            teile.append("%s rc=%d%s" % (label, r.returncode, ("  [" + erste + "]") if label == "ECHT" and erste else ""))
            with open(os.path.join(w, "gate_%s.log" % label), "w", encoding="utf-8") as h:
                h.write(aus)
        print("%-62s %s" % (titel, " | ".join(teile)))


main()
