# -*- coding: utf-8 -*-
"""Nachbesserung R1 (Befund B3 der Gegenpruefung): Mutanten-Probe ueber JEDE Pruefstelle des Gates.

Frage: faengt `apk_asset_gate.py --selbsttest` jede Abschwaechung? Die Probe des Bauers (M1-M8)
schaltete nur ganze Pruefungen ab; die Gegenpruefung fand vier Abschwaechungen (U1-U4), die den
Selbsttest mit 28/28 bestanden. Hier systematisch, aus dem Quelltext erzeugt (ast):
  A  jede Anweisung befund(...) einzeln durch 'pass' ersetzt,
  B  jede 'raise Bedienfehler(...)' / 'raise _Lesefehler(...)' einzeln durch 'pass' ersetzt,
  C  jedes 'return "<Grund>"' in _name_fehler einzeln durch 'pass' ersetzt (die Namensregeln),
  D  Hand-Mutanten fuer Semantik, die A-C nicht treffen (U3, U4, Offset aus dem Zentralverzeichnis,
     Namen normalisiert wie zipfile, EOCD vorn gesucht, ...).
Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/main). Je Mutant ein eigener Prozess
`<python> <mutant> --selbsttest`; er MUSS mit rc != 0 enden. Ausgabe je Mutant: rc + rote Faelle;
am Ende die Ueberlebenden (jeder muss im Dossier begruendet sein: unerreichbar oder redundant).

Aufruf: mutanten_voll.py <gate.py> <ausgabeordner> [--parallel N] [--python <exe>]
"""
import argparse
import ast
import concurrent.futures
import os
import re
import subprocess
import sys

PRUEFCODE = {"_zeichenkette_ende", "_ohne_kommentare", "_klammer_ende", "_anweisungen", "gradle_baeume",
             "gradle_baeume_pruefen", "_ant_passt", "_auslass_hinweis", "quelldateien", "zip_verzeichnis",
             "_name_fehler", "struktur_pruefen", "_eintrag_lesen", "_eintrag_pruefen", "_sha_datei",
             "manifest_pruefen", "pruefen"}

# D: (Name, [(alt, neu, erwartete Anzahl)])
HAND = [
    ("D1_U3_nur_erster_block", [
        ("            h.update(b)\n            n += len(b)\n",
         "            h.update(b)\n            n += len(b)\n            return h.hexdigest(), n\n", 1),
        ("            if d is None:\n                nimm(b)\n                continue\n",
         "            if d is None:\n                nimm(b)\n                break\n", 1)]),
    ("D2_U4_crc32_statt_sha256", [
        ("class Bedienfehler(Exception):",
         "class _Crc(object):\n    def __init__(self):\n        self.c = 0\n    def update(self, b):\n"
         "        self.c = zlib.crc32(b, self.c)\n    def hexdigest(self):\n        return '%08x' % self.c\n\n\n"
         "class Bedienfehler(Exception):", 1),
        ("    h, st = hashlib.sha256(), [0, 0]", "    h, st = _Crc(), [0, 0]", 1),
        ("    h, n = hashlib.sha256(), 0\n    with open(pfad", "    h, n = _Crc(), 0\n    with open(pfad", 1)]),
    ("D3_offset_aus_zentralverzeichnis", [
        ("        e.daten_off = e.lho + LFH_LEN + l_nlen + l_xlen\n",
         "        e.daten_off = e.lho + LFH_LEN + l_nlen\n", 1)]),
    ("D4_namen_wie_zipfile", [
        ("        grund = _name_fehler(e.name_roh)\n",
         "        e.name_roh = e.name_roh.split(b'\\x00')[0].replace(b'\\\\', b'/')\n"
         "        grund = _name_fehler(e.name_roh)\n", 1)]),
    ("D5_lfh_nur_name", [
        ("        if not (l_flags & FLAG_DATA_DESCRIPTOR) and (l_crc, l_csize, l_usize) != (e.crc, e.csize, e.usize):",
         "        if False:", 1)]),
    ("D6_manifest_cr_bleibt", [
        ("        z = z.rstrip(\"\\r\")", "        z = z", 1)]),
    ("D7_deflate_ende_egal", [
        ("            if not d.eof or d.unused_data:", "            if False:", 1)]),
    ("D8_nur_laenge_statt_crc", [
        ("    if crc != e.crc or n != e.usize:", "    if n != e.usize:", 1)]),
    ("D9_eocd_von_vorn", [
        ("        i = ende.rfind(EOCD_SIG)", "        i = ende.find(EOCD_SIG)", 1)]),
    ("D10_kommentarlaenge_egal", [
        ("        if eocd_pos + EOCD_LEN + kom != groesse:", "        if False:", 1)]),
    ("D11_zentralverzeichnis_rest_egal", [
        ("    if p != len(cd):\n", "    if False:\n", 1)]),
    ("D12_lage_egal", [
        ("        if e.daten_off + e.csize > cd_off:", "        if False:", 1)]),
    ("D13_doppelte_namen_egal", [
        ("        if len(je_name[name]) > 1:", "        if False:", 1)]),
    ("D14_nur_stored_gelesen", [
        ("    d = zlib.decompressobj(-15) if e.methode == 8 else None", "    d = None", 1)]),
    ("D15_nicht_assets_nicht_gelesen", [
        ("        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen), key=lambda x: x.lho):",
         "        for e in []:", 1)]),
    ("D16_sha_nur_quelle_ganz", [   # APK-Seite liest nur bis BLOCK, Quelle ganz
        ("            if d is None:\n                nimm(b)\n                continue\n",
         "            if d is None:\n                nimm(b)\n                if st[1] >= BLOCK:\n                    break\n"
         "                continue\n", 1)]),
    ("D17_manifest_nur_kopf", [
        ("    for pfad in sorted(eintraege):\n        if pfad not in apk_dateien:",
         "    for pfad in []:\n        if pfad not in apk_dateien:", 1)]),
    ("D18_zusatz_nur_ausserhalb_assets", [
        ("            if name != man_name and name not in quellen:",
         "            if name != man_name and name not in quellen and not name.startswith('assets/'):", 1)]),
]


def mutanten(quelle):
    baum = ast.parse(quelle)
    zeilen = quelle.split("\n")
    aus = []
    for fn in ast.walk(baum):
        if not isinstance(fn, ast.FunctionDef) or fn.name not in PRUEFCODE:
            continue
        for k in ast.walk(fn):
            art = None
            if (isinstance(k, ast.Expr) and isinstance(k.value, ast.Call) and isinstance(k.value.func, ast.Name)
                    and k.value.func.id == "befund"):
                art = "A"
            elif (isinstance(k, ast.Raise) and isinstance(k.exc, ast.Call) and isinstance(k.exc.func, ast.Name)
                  and k.exc.func.id in ("Bedienfehler", "_Lesefehler")):
                art = "B"
            elif (fn.name == "_name_fehler" and isinstance(k, ast.Return) and isinstance(k.value, ast.Constant)
                  and isinstance(k.value.value, str) and k.value.value):
                art = "C"
            if not art:
                continue
            a, e = k.lineno - 1, k.end_lineno            # Zeilen a .. e-1
            einzug = re.match(r"\s*", zeilen[a]).group(0)
            text = " ".join(" ".join(zeilen[a:e]).split())[:110]
            neu = zeilen[:a] + [einzug + "pass  # MUTANT"] + zeilen[e:]
            aus.append(("%s_%s_Z%d" % (art, fn.name, k.lineno), text, "\n".join(neu)))
    for name, ersetzungen in HAND:
        t = quelle
        for alt, neu, n_soll in ersetzungen:
            n = t.count(alt)
            if n != n_soll:
                raise SystemExit("Hand-Mutant %s: Muster %d-mal gefunden (erwartet %d): %r" % (name, n, n_soll, alt[:60]))
            t = t.replace(alt, neu)
        aus.append((name, "Hand-Mutant", t))
    return aus


def laufen(py, pfad):
    r = subprocess.run([py, pfad, "--selbsttest"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       stdin=subprocess.DEVNULL, timeout=900)
    aus = r.stdout.decode("utf-8", "replace")
    rot = re.findall(r"\[FEHLER\] (\d+) ", aus)
    return r.returncode, rot, aus


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("gate")
    ap.add_argument("ordner")
    ap.add_argument("--parallel", type=int, default=2)
    ap.add_argument("--python", default=sys.executable)
    a = ap.parse_args()
    quelle = open(a.gate, encoding="utf-8").read().replace("\r\n", "\n")
    os.makedirs(a.ordner, exist_ok=True)
    liste = mutanten(quelle)
    pfade = []
    for name, text, t in liste:
        p = os.path.join(a.ordner, name + ".py")
        with open(p, "w", encoding="utf-8", newline="\n") as f:
            f.write(t)
        compile(t, p, "exec")                      # jeder Mutant muss uebersetzbar sein
        pfade.append((name, text, p))
    print("# %d Mutanten (A befund, B raise, C Namensregel, D Hand) aus %s, Python %s"
          % (len(pfade), a.gate, a.python))
    ergebnis = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=a.parallel) as pool:
        laeufe = {name: pool.submit(laufen, a.python, p) for name, _t, p in pfade}
        for name, lauf in laeufe.items():
            ergebnis[name] = lauf.result()
    ueberlebt = []
    for name, text, p in pfade:
        rc, rot, aus = ergebnis[name]
        with open(os.path.join(a.ordner, name + ".log"), "w", encoding="utf-8") as f:
            f.write(aus)
        status = "ERKANNT" if rc != 0 else "UEBERLEBT"
        if rc == 0:
            ueberlebt.append((name, text))
        print("%-9s rc=%d rot=%-24s %-44s %s" % (status, rc, ",".join(rot) if rot else "-", name, text))
    print("# erkannt: %d / %d, ueberlebt: %d" % (len(pfade) - len(ueberlebt), len(pfade), len(ueberlebt)))
    for name, text in ueberlebt:
        print("#   UEBERLEBT %s: %s" % (name, text))
    return 0


if __name__ == "__main__":
    sys.exit(main())
