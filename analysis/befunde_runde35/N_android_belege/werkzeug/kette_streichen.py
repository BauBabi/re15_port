# Runde 35 Spur N, Nachbesserung 1 (M2): "Streich-Messung" der bash-Urteilslogik in release/apk_pruefen.sh.
# Je Variante genau EINE gedachte kuenftige Aenderung (eine Pruefzeile gestrichen/entschaerft), als eigene Kopie von
# release/ unter <ziel>/<id>/repo/release. kette_streichen.sh legt die Junctions (re15_port, synchro: nur gelesen)
# dazu, faehrt die ECHTE Kette test_r35_android_pruefkette.sh je Variante und wertet aus: BEMERKT = Kette rot.
# Aufruf: python kette_streichen.py <repo-wurzel> <ziel>   (legt die Ordner an, druckt die Varianten-IDs)
import os, shutil, sys

DATEIEN = ["apk_asset_gate.py", "apk_asset_gate.sha256", "apk_pruefen.sh", "apk_signer.sha256", "gate_urteil.py",
           "gate_urteil.sha256", "python_finden.sh"]

# (id, Beschreibung, [(alt, neu), ...]) - jedes alt muss genau einmal in apk_pruefen.sh stehen
V = [
    ("B0", "keine Aenderung (Kontrolle der Kopie)", []),
    ("B1", "zweite Instanz in gate_laufen ganz aus",
     [("    if (( urteil == 0 )); then\n        if (( rc != 0 )); then", "    if false; then\n        if (( rc != 0 )); then")]),
    ("B2", "gate_urteil_selbsttest: (( rc == 0 )) || die ... gestrichen (Abnahme 0, M2)",
     [('    (( rc == 0 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile',
       '    true || die "Selbsttest des Gate-Urteils: OK-Schlusszeile')]),
    ("B3", "zweite Instanz: Zweig 'Gate gab Rueckgabe != 0' aus",
     [('        if (( rc != 0 )); then\n            echo "   Gate-Urteil ueberstimmt',
       '        if false; then\n            echo "   Gate-Urteil ueberstimmt')]),
    ("B4", "zweite Instanz: Zweig 'ohne Urteilszeile' aus",
     [('        elif ! grep -q "^   Gate-Urteil ($modus, Rueckgabe 0): " "$log.urteil"; then', '        elif false; then')]),
    ("B5", "Faelle: f1 == f2 gestrichen",
     [("(( f1 == f2 && f2 >= GATE_URTEIL_MIN_FAELLE ))", "(( f2 >= GATE_URTEIL_MIN_FAELLE ))")]),
    ("B6", "Faelle: Mindestzahl gestrichen",
     [("(( f1 == f2 && f2 >= GATE_URTEIL_MIN_FAELLE ))", "(( f1 == f2 ))")]),
    ("B7", "Mutanten: e + g == m gestrichen",
     [("(( e + g == m && e >= GATE_URTEIL_MIN_ERKANNT && g <= GATE_URTEIL_MAX_GLEICH ))",
       "(( e >= GATE_URTEIL_MIN_ERKANNT && g <= GATE_URTEIL_MAX_GLEICH ))")]),
    ("B8", "Mutanten: Mindestzahl erkannt gestrichen",
     [("(( e + g == m && e >= GATE_URTEIL_MIN_ERKANNT && g <= GATE_URTEIL_MAX_GLEICH ))",
       "(( e + g == m && g <= GATE_URTEIL_MAX_GLEICH ))")]),
    ("B9", "Mutanten: Hoechstzahl gleichwertig gestrichen",
     [("(( e + g == m && e >= GATE_URTEIL_MIN_ERKANNT && g <= GATE_URTEIL_MAX_GLEICH ))",
       "(( e + g == m && e >= GATE_URTEIL_MIN_ERKANNT ))")]),
    ("B10", "bash-Muster der Schlusszeile ohne Endanker $",
     [("begruendet\\ ==$ ]]; then", "begruendet\\ == ]]; then")]),
    ("B11", "bash-Muster der Schlusszeile ohne Anfangsanker ^",
     [('if [[ "$letzte" =~ ^==\\ URTEIL-SELBSTTEST-OK:', 'if [[ "$letzte" =~ ==\\ URTEIL-SELBSTTEST-OK:')]),
    ("B12", "Schlusszeile = letzte Zeile mit URTEIL-SELBSTTEST statt letzte Zeile",
     [("letzte=\"$(tr -d '\\r' < \"$log\" | grep -v '^[[:space:]]*$' | tail -1)\"",
       "letzte=\"$(tr -d '\\r' < \"$log\" | grep 'URTEIL-SELBSTTEST' | tail -1)\"")]),
    ("B13", "Urteils-Pin: Vergleich ist == soll entschaerft",
     [('    [[ "$ist" == "$soll" ]] || die "Gate-Urteil ist NICHT das festgehaltene',
       '    true || die "Gate-Urteil ist NICHT das festgehaltene')]),
    ("B14", "Gate-Pin: Vergleich ist == soll entschaerft",
     [('    [[ "$ist" == "$soll" ]] || die "Asset-Gate ist NICHT das festgehaltene',
       '    true || die "Asset-Gate ist NICHT das festgehaltene')]),
    ("B15", "Urteils-Pruefung vor jeder Nutzung (gate_laufen UND gate_urteil) gestrichen",
     [('    gate_urteil_selbsttest "$GATE_URTEIL_KOPIE"         # Runde 35: Pin des Urteils vor JEDEM Lauf',
       '    :         # (gestrichen)'),
      ('    gate_urteil_selbsttest "$GATE_URTEIL_KOPIE"\n    "$PY"', '    "$PY"')]),
    ("B16", "Gate-Pin vor jedem Lauf (gate_laufen) gestrichen",
     [('    shift 2\n    gate_pin_pruefen "$gate"\n', '    shift 2\n')]),
    ("B17", "kein Abbruch bei ungueltiger Schlusszeile (else-Zweig entschaerft)",
     [('        die "Selbsttest des Gate-Urteils ohne gueltige Schlusszeile', '        true "Selbsttest des Gate-Urteils ohne gueltige Schlusszeile')]),
]


def main():
    repo, ziel = sys.argv[1], sys.argv[2]
    quelle = open(os.path.join(repo, "release", "apk_pruefen.sh"), encoding="utf-8", newline="").read()
    for vid, besch, aend in V:
        rel = os.path.join(ziel, vid, "repo", "release")
        os.makedirs(rel, exist_ok=True)
        for d in DATEIEN:
            shutil.copyfile(os.path.join(repo, "release", d), os.path.join(rel, d))
        t = quelle
        for alt, neu in aend:
            n = t.count(alt)
            if n != 1:
                sys.exit("%s: Muster %d-mal: %r" % (vid, n, alt[:70]))
            t = t.replace(alt, neu)
        open(os.path.join(rel, "apk_pruefen.sh"), "w", encoding="utf-8", newline="").write(t)
        open(os.path.join(ziel, vid, "beschreibung.txt"), "w", encoding="utf-8").write(besch + "\n")
        print(vid)


main()
