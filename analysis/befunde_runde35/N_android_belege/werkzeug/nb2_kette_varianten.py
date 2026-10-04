# Runde 35 Spur N, Nachbesserung 2 (Abnahme 1, M3): die Aenderungen F1-F13 und G1/G2 der Abnahme 1 (N_abnahme_1.md 3.3,
# woertlich) an der ECHTEN Kette nachfahren. Je Variante genau EINE Aenderung, als eigene Kopie von release/ unter
# <ziel>/<id>/repo/release; nb2_kette_varianten.sh legt die Junctions (re15_port, synchro: nur gelesen) dazu, faehrt
# test_r35_android_pruefkette.sh je Variante und wertet aus: BEMERKT = Kette rot.
# F*: eine Textaenderung in apk_pruefen.sh. G*: das Urteil gate_urteil.py durch eine Variante ersetzt UND neu gepinnt
# (wie es ein Entwickler taete): G1 = E24 (quellbaum ohne Tuer-Soll-Regel; seit NB2 = Anweisung tuer_soll() gestrichen),
# G2 = E4 (tuer_soll != -> <).
# F9 hat seit NB2 die Form "| tail -1 || true)" (Fix: pipefail, Dossier) - die Aenderung selbst ist dieselbe.
# Aufruf: python nb2_kette_varianten.py <repo-wurzel> <ziel>   (legt die Ordner an, druckt die Varianten-IDs)
import hashlib
import os
import re
import shutil
import sys

DATEIEN = ["apk_asset_gate.py", "apk_asset_gate.sha256", "apk_pruefen.sh", "apk_signer.sha256", "gate_urteil.py",
           "gate_urteil.sha256", "python_finden.sh"]

F = [
    ("F0", "keine Aenderung (Kontrolle der Kopie)", []),
    ("F1", ":352 zweite Instanz if (( rc != 0 )) -> if (( rc == 1 ))",
     [('        if (( rc != 0 )); then\n            echo "   Gate-Urteil ueberstimmt',
       '        if (( rc == 1 )); then\n            echo "   Gate-Urteil ueberstimmt')]),
    ("F2", ":352 rc != 0 -> rc == 2",
     [('        if (( rc != 0 )); then\n            echo "   Gate-Urteil ueberstimmt',
       '        if (( rc == 2 )); then\n            echo "   Gate-Urteil ueberstimmt')]),
    ("F3", ":296 (( f1 == f2 && -> (( f1 >= f2 &&", [("(( f1 == f2 && ", "(( f1 >= f2 && ")]),
    ("F4", ":298 (( e + g == m && -> (( e + g >= m &&", [("(( e + g == m && ", "(( e + g >= m && ")]),
    ("F5", ":295 (( rc == 0 )) || die -> (( rc <= 1 )) || die",
     [('    (( rc == 0 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile',
       '    (( rc <= 1 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile')]),
    ("F6", ":295 (( rc == 0 )) || die -> (( rc != 1 )) || die",
     [('    (( rc == 0 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile',
       '    (( rc != 1 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile')]),
    ("F7", ":345 || rc=$? -> || true (Gate-Rueckgabe verschluckt)",
     [('"$@" > "$log" 2>&1 || rc=$?', '"$@" > "$log" 2>&1 || true')]),
    ("F8", ":290 Mutanten\\ erkannt,\\ ([0-9]+)\\ als -> ([0-9]*)",
     [("Mutanten\\ erkannt,\\ ([0-9]+)\\ als", "Mutanten\\ erkannt,\\ ([0-9]*)\\ als")]),
    ("F9", ":288 tail -1 -> tail -2 | head -1", [("| tail -1 || true)\"", "| tail -2 | head -1 || true)\"")]),
    ("F10", ":355 Urteilszeile in $log statt $log.urteil", [('" "$log.urteil"; then', '" "$log"; then')]),
    ("F11", ":355 \"^   Gate-Urteil ($modus, Rueckgabe 0): \" -> \"^   Gate-Urteil (\"",
     [('"^   Gate-Urteil ($modus, Rueckgabe 0): "', '"^   Gate-Urteil ("')]),
    ("F12", ":296 f2 >= MIN -> f2 >= MIN - 1",
     [("f2 >= GATE_URTEIL_MIN_FAELLE ))", "f2 >= GATE_URTEIL_MIN_FAELLE - 1 ))")]),
    ("F13", ":298 g <= MAX -> g <= MAX + 1",
     [("g <= GATE_URTEIL_MAX_GLEICH ))", "g <= GATE_URTEIL_MAX_GLEICH + 1 ))")]),
]
G = [
    ("G1", "Urteil = E24 (quellbaum ohne Tuer-Soll-Regel), neu gepinnt",
     (r"\n        tuer_soll\(\)[^\n]*(\n        ende\(0, \"APK-ASSET-GATE-QUELLBAUM-OK)", r"\1")),
    ("G2", "Urteil = E4 (tuer_soll int(g1) != int(g2) -> <), neu gepinnt",
     (re.escape("int(m.group(1)) != int(m.group(2))"), "int(m.group(1)) < int(m.group(2))")),
]


def main():
    repo, ziel = sys.argv[1], sys.argv[2]
    rel_q = os.path.join(repo, "release")
    ap = open(os.path.join(rel_q, "apk_pruefen.sh"), encoding="utf-8", newline="").read()
    ur = open(os.path.join(rel_q, "gate_urteil.py"), encoding="utf-8", newline="").read()
    for vid, besch, aend in F + [(g[0], g[1], None) for g in G]:
        rel = os.path.join(ziel, vid, "repo", "release")
        os.makedirs(rel, exist_ok=True)
        for d in DATEIEN:
            shutil.copyfile(os.path.join(rel_q, d), os.path.join(rel, d))
        if aend is not None:
            t = ap
            for alt, neu in aend:
                n = t.count(alt)
                if n != 1:
                    sys.exit("%s: Muster %d-mal: %r" % (vid, n, alt[:70]))
                t = t.replace(alt, neu)
            open(os.path.join(rel, "apk_pruefen.sh"), "w", encoding="utf-8", newline="").write(t)
        else:
            muster, neu = next(g[2] for g in G if g[0] == vid)
            if len(re.findall(muster, ur)) != 1:
                sys.exit("%s: Urteils-Muster nicht genau einmal" % vid)
            u = re.sub(muster, neu, ur)
            open(os.path.join(rel, "gate_urteil.py"), "w", encoding="utf-8", newline="").write(u)
            sha = hashlib.sha256(u.encode("utf-8")).hexdigest()
            open(os.path.join(rel, "gate_urteil.sha256"), "w", encoding="utf-8", newline="").write(sha + "\n")
        open(os.path.join(ziel, vid, "beschreibung.txt"), "w", encoding="utf-8").write(besch + "\n")
        print(vid)


main()
