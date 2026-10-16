#!/usr/bin/env bash
# =============================================================================
# Attrappen der Release-Einzelschritte fuer test_r38_releaseskript.sh (Runde 38, Spur V)
# =============================================================================
# Die Tests kopieren release/release_alles.sh + release_alles_lib.sh (ECHT) in ein Wegwerf-Repo und legen
# daneben diese Attrappen unter den Namen der echten Schritt-Skripte (build_win_cross.sh, build_android.sh,
# build_linux_deck.sh, make_package.sh, docker_bereit.sh, python_finden.sh). Sie tun fuer release_alles.sh
# sichtbar dasselbe wie die echten (Artefakt, Erfolgsmarke, Volumes, git-Index), in Sekunden statt Stunden,
# und lassen sich ueber die Umgebung gezielt kaputt machen:
#   FAKE_PROTOKOLL=<datei>   jeder Aufruf haengt "<schritt> <argumente> jobs=<RE15_CTEST_JOBS>" an
#   FAKE_FAIL=<schritt>      Rueckgabe 7 (nach etwas Ausgabe) - auch durch die tee-Pipe von release_alles.sh
#   FAKE_KEIN=<schritt>      Rueckgabe 0 MIT Erfolgsmarke, aber OHNE Artefakt
#   FAKE_ALT=<schritt>       Artefakt mit Zeitstempel 2020-01-01 (altes Artefakt, das als neues durchgehen will)
#   FAKE_OHNE_MARKE=<schritt> Rueckgabe 0 und Artefakt, aber ohne Erfolgsmarke
#   FAKE_OHNE_ANDROID=1      make_package packt keinen Android-Satz (wie das echte ohne APK, make_package.sh:929)
#   FAKE_ALTE_BLEIBEN=1      make_package laesst die Volumes der alten Version liegen
#   FAKE_DOCKER=nein         docker_bereit.sh meldet "nicht bereit"
# Schritt-Namen: windows android linux paket.
# =============================================================================

ATTR_HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ATTR_REPO="$(cd "$ATTR_HERE/.." && pwd)"

attr_protokoll() {   # $1 = Schritt, Rest = Argumente
    local s="$1"; shift
    if [[ -n "${FAKE_PROTOKOLL:-}" ]]; then
        echo "$s $* jobs=${RE15_CTEST_JOBS:-}" >> "$FAKE_PROTOKOLL"
    fi
}

# attr_artefakt <schritt> <datei> <inhalt> <marke>: das Verhalten eines Bau-Schritts
attr_artefakt() {
    local s="$1" f="$2" inhalt="$3" marke="$4"
    echo "== Attrappe $s: baue $(basename "$f") =="
    echo "   (Zeile 1 der Bau-Ausgabe)"
    if [[ "${FAKE_FAIL:-}" == "$s" ]]; then
        echo "!!! Attrappe $s: simulierter Fehlschlag" >&2
        exit 7
    fi
    mkdir -p "$(dirname "$f")"
    if [[ "${FAKE_KEIN:-}" != "$s" ]]; then
        printf '%s' "$inhalt" > "$f"
        if [[ "${FAKE_ALT:-}" == "$s" ]]; then touch -d '2020-01-01 00:00:00' "$f"; fi
    fi
    if [[ "${FAKE_OHNE_MARKE:-}" != "$s" ]]; then
        echo "$marke"
    fi
}
