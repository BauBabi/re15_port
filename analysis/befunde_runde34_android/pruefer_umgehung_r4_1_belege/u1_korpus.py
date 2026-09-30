# -*- coding: utf-8 -*-
"""Pruefer UMGEHUNG R4-1, H5: eigene Faelschungen gegen Format v2 - Geraete-Leser (asset_abgleich.c ueber
u1_liste_korpus.exe) und Gate (manifest_lesen aus einer Gate-Datei, als Modul geladen) auf DENSELBEN Bytes.
Spalte "Geraet soll": was das Format laut asset_abgleich.h verlangt (A = annehmen, V = verwerfen).
Aufruf: u1_korpus.py <korpus_exe> <ordner> <gate.py> [<gate2.py> ...]"""
import importlib.util
import os
import subprocess
import sys
import unicodedata

S1 = b"1" * 64
S2 = b"2" * 64
P1 = b"shared_assets/PSX/a.bin"
P2 = b"shared_assets/PSX/b.bin"


def liste(zeilen, n=None, b=None, nl=b"\n", kopf=None):
    if n is None:
        n = len(zeilen)
    if b is None:
        b = sum(int(z[0]) for z in zeilen)
    k = kopf if kopf is not None else b"# re15 assets v2 %d %d" % (n, b)
    return nl.join([k] + [z[0] + b"\t" + z[1] + b"\t" + z[2] for z in zeilen]) + nl


G = [(b"5", S1, P1), (b"5", S2, P2)]
K = "K".encode("utf-8")                                   # KELVIN SIGN (Faltung -> 'k')
NFC = unicodedata.normalize("NFC", "é").encode("utf-8")   # e-acute, 1 Codepunkt
NFD = unicodedata.normalize("NFD", "é").encode("utf-8")   # e + U+0301

KORPUS = [
    # (name, bytes, Geraet soll)
    ("gut", liste(G), "A"),
    ("summe_falsch_aber_hex", liste([(b"5", b"a" * 64, P1), G[1]]), "A"),   # Format gueltig; APK-Abgleich faengt es
    ("summe_leer", liste([(b"5", b"", P1), G[1]]), "V"),
    ("summe_fehlt_v1_zeile", b"# re15 assets v2 2 10\n5\t" + P1 + b"\n5\t" + S2 + b"\t" + P2 + b"\n", "V"),
    ("summe_gross_ganz", liste([(b"5", b"A" * 64, P1), G[1]]), "V"),
    ("summe_ein_grossbuchstabe", liste([(b"5", b"a" * 63 + b"F", P1), G[1]]), "V"),
    ("summe_leerzeichen_hinten", liste([(b"5", S1 + b" ", P1), G[1]]), "V"),
    ("summe_leerzeichen_vorn", liste([(b"5", b" " + S1, P1), G[1]]), "V"),
    ("summe_nbsp_hinten", liste([(b"5", S1 + b"\xc2\xa0", P1), G[1]]), "V"),
    ("summe_tab_doppelt", b"# re15 assets v2 2 10\n5\t\t" + S1 + b"\t" + P1 + b"\n5\t" + S2 + b"\t" + P2 + b"\n", "V"),
    ("summe_vollbreit_ziffer", liste([(b"5", "１".encode() + S1[3:], P1), G[1]]), "V"),
    ("summe_63_plus_g", liste([(b"5", S1[:63] + b"g", P1), G[1]]), "V"),
    ("zeilenende_nur_cr", liste(G, nl=b"\r"), "V"),
    ("zeilenende_lf_cr", liste(G, nl=b"\n\r"), "V"),
    ("zeilenende_crlf", liste(G, nl=b"\r\n"), "A"),
    ("zeilenende_cr_cr_lf", liste(G, nl=b"\r\r\n"), "A"),
    ("zeilenende_vt", liste(G, nl=b"\x0b"), "V"),
    ("zeilenende_ff", liste(G, nl=b"\x0c"), "V"),
    ("zeilenende_nel_utf8", liste(G, nl=b"\xc2\x85"), "V"),
    ("zeilenende_u2028", liste(G, nl=" ".encode()), "V"),
    ("zeile_nur_leerzeichen", liste(G)[:-1] + b"\n   \n", "V"),
    ("zeile_nur_tab", liste(G)[:-1] + b"\n\t\n", "V"),
    ("zeile_nur_cr", liste(G)[:-1] + b"\n\r\n", "A"),
    ("zeile_nur_nbsp", liste(G)[:-1] + b"\n\xc2\xa0\n", "V"),
    ("kopf_v1_zeilen_v2", liste(G, kopf=b"# re15 assets 2 10"), "V"),
    ("kopf_v2_zeilen_v1", b"# re15 assets v2 2 10\n5\t" + P1 + b"\n5\t" + P2 + b"\n", "V"),
    ("zwei_kopfzeilen_v2", liste(G).replace(b"\n", b"\n# re15 assets v2 2 10\n", 1), "V"),
    ("zweite_zeile_v1_kopf", liste(G).replace(b"\n", b"\n# re15 assets 2 10\n", 1), "V"),
    ("kopf_anzahl_null_vorn", liste(G, kopf=b"# re15 assets v2 0002 0010"), "A"),
    ("kopf_mit_cr_cr", liste(G, kopf=b"# re15 assets v2 2 10\r\r"), "A"),
    ("kopf_utf16", "# re15 assets v2 2 10\n".encode("utf-16-le") + liste(G).split(b"\n", 1)[1], "V"),
    ("pfad_doppelt", liste([G[0], (b"5", S2, P1)]), "V"),
    ("pfad_ascii_gross_klein", liste([G[0], (b"5", S2, P1.upper())]), "V"),
    ("pfad_kelvin_gegen_K", liste([(b"5", S1, b"shared_assets/PSX/K.bin"), (b"5", S2, b"shared_assets/PSX/" + K + b".bin")]), "V?"),
    ("pfad_kelvin_gegen_k", liste([(b"5", S1, b"shared_assets/PSX/k.bin"), (b"5", S2, b"shared_assets/PSX/" + K + b".bin")]), "V?"),
    ("pfad_AE_gegen_ae", liste([(b"5", S1, "shared_assets/PSX/Ä.bin".encode()), (b"5", S2, "shared_assets/PSX/ä.bin".encode())]), "V?"),
    ("pfad_nfc_gegen_nfd", liste([(b"5", S1, b"shared_assets/PSX/" + NFC + b".bin"), (b"5", S2, b"shared_assets/PSX/" + NFD + b".bin")]), "V?"),
    ("pfad_sz_gegen_ss", liste([(b"5", S1, "shared_assets/PSX/straße.bin".encode()), (b"5", S2, b"shared_assets/PSX/strasse.bin")]), "V?"),
    ("pfad_steuer_1f", liste([(b"5", S1, b"shared_assets/PSX/a\x1f.bin"), G[1]]), "V"),
    ("pfad_c1_nel", liste([(b"5", S1, "shared_assets/PSX/a\u0085.bin".encode()), G[1]]), "A"),
    ("pfad_leerzeichen_hinten", liste([(b"5", S1, P1 + b" "), G[1]]), "A"),
    ("pfad_punkt_hinten", liste([(b"5", S1, P1 + b"."), G[1]]), "A"),
    ("pfad_neu_mitte", liste([(b"5", S1, b"shared_assets/PSX/x.neu.bin"), G[1]]), "A"),
    ("pfad_neu_ordner", liste([(b"5", S1, b"shared_assets/.neu/x"), G[1]]), "A"),
    ("pfad_NEU_gross", liste([(b"5", S1, b"shared_assets/PSX/x.NEU"), G[1]]), "V"),
    ("pfad_neu_vollbreit", liste([(b"5", S1, "shared_assets/PSX/x.ｎｅｕ".encode()), G[1]]), "A"),
    ("pfad_512_mit_2byte_am_ende", liste([(b"5", S1, b"a/" + b"x" * 508 + "ä".encode()), G[1]]), "A"),
    ("pfad_513_mit_2byte_am_ende", liste([(b"5", S1, b"a/" + b"x" * 509 + "ä".encode()), G[1]]), "V"),
    ("pfad_datei_und_ordner", liste([(b"5", S1, b"shared_assets/PSX/a"), (b"5", S2, b"shared_assets/PSX/a/b")]), "A"),
    ("pfad_datei_und_ordner_gross", liste([(b"5", S1, b"shared_assets/PSX/a"), (b"5", S2, b"shared_assets/PSX/A/b")]), "A"),
    ("groesse_null_vorn", liste([(b"05", S1, P1), G[1]]), "A"),
    ("groesse_leerzeichen", liste([(b"5 ", S1, P1), G[1]], b=10), "V"),
    ("groesse_unterstrich", liste([(b"5_0", S1, P1), G[1]], b=55), "V"),
    ("bom", b"\xef\xbb\xbf" + liste(G), "V"),
    ("nul_am_ende", liste(G) + b"\0", "V"),
    ("nur_kopf_und_leer", b"# re15 assets v2 0 0\n\n", "V"),
]


def gate_laden(pfad, name):
    spec = importlib.util.spec_from_file_location(name, pfad)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def main():
    exe, ordner, gates = sys.argv[1], sys.argv[2], sys.argv[3:]
    os.makedirs(ordner, exist_ok=True)
    dateien = []
    for name, roh, _soll in KORPUS:
        p = os.path.join(ordner, name + ".txt")
        with open(p, "wb") as f:
            f.write(roh)
        dateien.append(p)
    aus = subprocess.run([exe] + dateien, stdout=subprocess.PIPE, check=True).stdout.decode("utf-8", "replace")
    geraet = {}
    for z in aus.splitlines():
        teile = z.split("\t")
        geraet[os.path.splitext(os.path.basename(teile[0]))[0]] = (teile[1], teile[4] if len(teile) > 4 else "")
    mods = [gate_laden(g, "gate%d" % i) for i, g in enumerate(gates)]
    kopf = "%-30s %-5s %-6s %-9s" % ("Fall", "soll", "Geraet", "Gate") + "".join(" %-9s" % ("Gate%d" % i) for i in range(1, len(mods)))
    print(kopf)
    abw_gate_geraet, abw_soll = [], []
    for name, roh, soll in KORPUS:
        rc, fehler = geraet[name]
        g_an = "A" if rc == "0" else ("V1" if rc == "-2" else "V")
        zeile = "%-30s %-5s %-6s" % (name, soll, g_an)
        for i, m in enumerate(mods):
            _e, _z, v1, f = m.manifest_lesen(roh)
            an = "A" if not f else ("V1" if v1 else "V")
            zeile += " %-9s" % an
            if i == 0 and (an == "A") != (g_an == "A"):
                abw_gate_geraet.append(name)
        if (g_an == "A") != (soll.startswith("A")):
            abw_soll.append(name)
        print(zeile + ("   | " + fehler[:70] if fehler and fehler != "-" else ""))
    print("# Gate (1. Datei) != Geraet: %d %s" % (len(abw_gate_geraet), abw_gate_geraet))
    print("# Geraet != 'soll' (Format-Regel bzw. case-insensitiver Speicher): %d %s" % (len(abw_soll), abw_soll))


if __name__ == "__main__":
    main()
