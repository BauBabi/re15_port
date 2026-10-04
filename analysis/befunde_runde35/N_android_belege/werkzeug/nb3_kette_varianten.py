# Runde 35 Spur N, Nachbesserung 3 (Abnahme 2, M1-M3): die Ketten-Varianten der Abnahme 2 (3.5, K-/G-Reihe) an der
# ECHTEN Kette nachfahren. Je Variante genau EINE Aenderung (G4b: Aenderung + das vom Skript verlangte Nachziehen der
# Mindestzahl), als eigene Kopie von release/ unter <ziel>/<id>/repo/release; nb3_kette_varianten.sh legt die Junctions
# (re15_port, synchro: nur gelesen) dazu, faehrt test_r35_android_pruefkette.sh je Variante und wertet aus: BEMERKT = rot.
# K*: Textaenderung in apk_pruefen.sh. G*: das Urteil gate_urteil.py durch eine Variante ersetzt UND neu gepinnt (wie es ein
# Entwickler taete). Ersetzungen woertlich wie N_abnahme_2.md 3.3/3.4 bzw. nb3_urteil_aenderungen.py / nb3_bash_aenderungen.py.
# Aufruf: python nb3_kette_varianten.py <repo-wurzel> <ziel>   (legt die Ordner an, druckt die Varianten-IDs)
import hashlib
import os
import shutil
import sys

DATEIEN = ["apk_asset_gate.py", "apk_asset_gate.sha256", "apk_pruefen.sh", "apk_signer.sha256", "gate_urteil.py",
           "gate_urteil.sha256", "python_finden.sh"]
MIN = '"$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN"'
H17_ALT = "any(qq != gg or qq == 0 for qq, gg in baum)"
H17_NEU = "any(qq == 0 for qq, gg in baum) or sum(qq for qq, _g in baum) != sum(gg for _q, gg in baum)"

# (id, Beschreibung, Ersetzungen in apk_pruefen.sh, Ersetzungen in gate_urteil.py)
V = [
    ("K0", "keine Aenderung (Kontrolle der Kopie)", [], []),
    ("K2", "J11: gate_urteil uebergibt 0 0 statt der Mindestzahlen", [(MIN, "0 0")], []),
    ("K3", "J12: MIN_INNEN doppelt", [(MIN, '"$GATE_SELBSTTEST_MIN_INNEN" "$GATE_SELBSTTEST_MIN_INNEN"')], []),
    ("K4", "J13: unzip-Zaehlung nicht uebergeben", [('"${GATE_APK_EINTRAEGE:-}"', '""')], []),
    ("K6", "Y6: MIN_FAELLE/MIN_INNEN vertauscht", [(MIN, '"$GATE_SELBSTTEST_MIN_INNEN" "$GATE_SELBSTTEST_MIN_FAELLE"')], []),
    ("G3", "H10 (range(1, len(faelle)+1)), neu gepinnt", [],
     [("!= list(range(1, n + 1))", "!= list(range(1, len(faelle) + 1))")]),
    ("G4b", "H17 (Quelle = gleich nur als Summe), neu gepinnt, GATE_URTEIL_MIN_ERKANNT=886 nachgezogen",
     [("GATE_URTEIL_MIN_ERKANNT=887\n", "GATE_URTEIL_MIN_ERKANNT=886\n")], [(H17_ALT, H17_NEU)]),
    ("G5", "H18 (Zusatzspalte in der pk-Baumzeile), neu gepinnt", [],
     [('r"   \\S+\\s+(\\d+)\\s+(\\d+)"', 'r"   \\S+\\s+(\\d+)\\s+(?:\\S*\\s+)?(\\d+)"')]),
]


def ersetzen(t, aend, vid, datei):
    for alt, neu in aend:
        n = t.count(alt)
        if n != 1:
            sys.exit("%s: Muster in %s %d-mal: %r" % (vid, datei, n, alt[:70]))
        t = t.replace(alt, neu)
    return t


def main():
    repo, ziel = sys.argv[1], sys.argv[2]
    rel_q = os.path.join(repo, "release")
    ap = open(os.path.join(rel_q, "apk_pruefen.sh"), encoding="utf-8", newline="").read()
    ur = open(os.path.join(rel_q, "gate_urteil.py"), encoding="utf-8", newline="").read()
    for vid, besch, a_ap, a_ur in V:
        rel = os.path.join(ziel, vid, "repo", "release")
        os.makedirs(rel, exist_ok=True)
        for d in DATEIEN:
            shutil.copyfile(os.path.join(rel_q, d), os.path.join(rel, d))
        if a_ap:
            open(os.path.join(rel, "apk_pruefen.sh"), "w", encoding="utf-8", newline="").write(ersetzen(ap, a_ap, vid, "apk_pruefen.sh"))
        if a_ur:
            u = ersetzen(ur, a_ur, vid, "gate_urteil.py")
            open(os.path.join(rel, "gate_urteil.py"), "w", encoding="utf-8", newline="").write(u)
            open(os.path.join(rel, "gate_urteil.sha256"), "w", encoding="utf-8", newline="").write(
                hashlib.sha256(u.encode("utf-8")).hexdigest() + "\n")
        open(os.path.join(ziel, vid, "beschreibung.txt"), "w", encoding="utf-8").write(besch + "\n")
        print(vid)


main()
