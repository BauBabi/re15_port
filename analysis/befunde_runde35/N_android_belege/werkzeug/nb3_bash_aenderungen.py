# Runde 35 Spur N, Nachbesserung 3 (Abnahme 2, M1 + Hinweise H1/H2): faehrt simulierte Aenderungen am bash-Urteil
# release/apk_pruefen.sh gegen urteil_kontrollen.sh (ALLE Kontrollen, ohne --schnell). J-Reihe = Abnahme 2 3.4 (so
# woertlich wie die Tabelle sie angibt), Y-Reihe = eigene Aenderungen an derselben Schnittstelle (Uebergabe an Urteil
# und Gate). Je Variante genau EINE Textersetzung in einer Kopie von release/ (die anderen release-Dateien daneben, damit
# GATE_QUELLE/GATE_URTEIL_QUELLE der Kopie auf echte Dateien zeigen - Messhinweis der Abnahme 2 zu J14/J15).
# BEMERKT = mindestens eine Kontrolle FALSCH.
# Aufruf: python nb3_bash_aenderungen.py <repo-wurzel> <arbeitsordner> <bash> [parallel=6]
import concurrent.futures as cf
import os
import re
import shutil
import subprocess
import sys

DATEIEN = ["apk_asset_gate.py", "apk_asset_gate.sha256", "apk_pruefen.sh", "apk_signer.sha256", "gate_urteil.py",
           "gate_urteil.sha256", "python_finden.sh"]

J = [
    ("J0", "keine Aenderung (Kontrolle)", None, None),
    ("J6", "Urteilszeile mit festem Modus (selbsttest)", 'grep -q "^   Gate-Urteil ($modus, Rueckgabe 0): "',
     'grep -q "^   Gate-Urteil (selbsttest, Rueckgabe 0): "'),
    ("J9", "Gate-Pin nur 16 Hexziffern", '[[ "$ist" == "$soll" ]] || die "Asset-Gate ist NICHT',
     '[[ "$ist" == "${soll:0:16}"* ]] || die "Asset-Gate ist NICHT'),
    ("J10", "Urteils-Pin nur 16 Hexziffern", '[[ "$ist" == "$soll" ]] || die "Gate-Urteil ist NICHT',
     '[[ "$ist" == "${soll:0:16}"* ]] || die "Gate-Urteil ist NICHT'),
    ("J11", "gate_urteil uebergibt 0 0 statt der Mindestzahlen", '"$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN"',
     '0 0'),
    ("J12", "MIN_INNEN doppelt (min_faelle 148 statt 261)", '"$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN"',
     '"$GATE_SELBSTTEST_MIN_INNEN" "$GATE_SELBSTTEST_MIN_INNEN"'),
    ("J13", "unzip-Zaehlung nicht mehr uebergeben", '"${GATE_APK_EINTRAEGE:-}"', '""'),
    # ---- eigene (Nachbesserung 3): dieselbe Schnittstelle, andere Stellen
    ("Y1", "gate_laufen gibt dem Urteil Rueckgabe 0 statt $rc", 'gate_urteil "$modus" "$log" "$rc"', 'gate_urteil "$modus" "$log" 0'),
    ("Y2", "Gate bekommt seine Argumente nicht", '"$PY" "$(apk_nativ "$gate")" "$@" >', '"$PY" "$(apk_nativ "$gate")" >'),
    ("Y3", "gate_laufen: Modus fest selbsttest", 'local modus="$1" gate="$2"', 'local modus="selbsttest" gate="$2"'),
    ("Y4", "gate_urteil gibt Rueckgabe 0 statt $3 weiter", '"$1" "$2" "$3" "$GATE_SELBSTTEST_MIN_FAELLE"',
     '"$1" "$2" 0 "$GATE_SELBSTTEST_MIN_FAELLE"'),
    ("Y5", "Urteil liest $log.urteil statt der Gate-Ausgabe", 'gate_urteil "$modus" "$log" "$rc"',
     'gate_urteil "$modus" "$log.urteil" "$rc"'),
    ("Y6", "MIN_FAELLE und MIN_INNEN vertauscht", '"$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN"',
     '"$GATE_SELBSTTEST_MIN_INNEN" "$GATE_SELBSTTEST_MIN_FAELLE"'),
    ("Y7", "unzip-Zaehlung aus APK_ASSET_EINTRAEGE (falscher Name)", '"${GATE_APK_EINTRAEGE:-}"', '"${APK_ASSET_EINTRAEGE:-}"'),
    ("Y8", "gate_urteil: Modus fest apk", '"$1" "$2" "$3" "$GATE_SELBSTTEST_MIN_FAELLE"',
     'apk "$2" "$3" "$GATE_SELBSTTEST_MIN_FAELLE"'),
]


def main():
    repo, ziel, bash = sys.argv[1], sys.argv[2], sys.argv[3]
    par = int(sys.argv[4]) if len(sys.argv) > 4 else 6
    kontrollen = os.path.join(repo, "re15_port/tests/unit/r35_android/urteil_kontrollen.sh").replace("\\", "/")
    ap = open(os.path.join(repo, "release", "apk_pruefen.sh"), encoding="utf-8", newline="").read()
    if os.path.isdir(ziel):
        shutil.rmtree(ziel)
    os.makedirs(ziel)
    att = (ziel + "/attrappen").replace("\\", "/")
    p = subprocess.run([bash, kontrollen, "anlegen", os.path.join(repo, "release", "apk_pruefen.sh").replace("\\", "/"), att],
                       capture_output=True, text=True)
    print(p.stdout.strip().splitlines()[-1] if p.stdout.strip() else p.stderr[-300:])

    def lauf(jid, alt, neu):
        d = os.path.join(ziel, jid, "release")
        os.makedirs(d)
        for f in DATEIEN:
            shutil.copy2(os.path.join(repo, "release", f), d)
        if alt is not None:
            if ap.count(alt) != 1:
                return None, "NICHT ANWENDBAR (%d Treffer)" % ap.count(alt), []
            open(os.path.join(d, "apk_pruefen.sh"), "w", encoding="utf-8", newline="").write(ap.replace(alt, neu))
        q = subprocess.run([bash, kontrollen, "pruefen", (d + "/apk_pruefen.sh").replace("\\", "/"), att,
                            (os.path.join(ziel, jid, "w")).replace("\\", "/")], capture_output=True, text=True)
        z = q.stdout.strip().splitlines()
        return q.returncode, (z[-1] if z else ""), [x[:150] for x in z if x.startswith("FALSCH")]

    with cf.ThreadPoolExecutor(par) as ex:
        futs = [(jid, besch, alt, ex.submit(lauf, jid, alt, neu)) for jid, besch, alt, neu in J]
        n = bemerkt = 0
        for jid, besch, alt, fut in futs:
            rc, schluss, falsch = fut.result()
            if alt is None:
                print("%-4s %s: %s" % (jid, "KONTROLLE-OK" if rc == 0 else "KONTROLLE-FALSCH", schluss))
                continue
            n += 1
            b = rc not in (0, None)
            bemerkt += b
            print("%-4s %-8s %s   [%s]" % (jid, "BEMERKT" if b else "NICHT", besch, schluss))
            for x in falsch[:4]:
                print("       " + x)
    print("SUMME: %d von %d Aenderungen bemerkt" % (bemerkt, n))


if __name__ == "__main__":
    main()
