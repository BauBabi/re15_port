#!/usr/bin/env bash
# =============================================================================
# Test r38_releaseskript (Runde 38, Spur V): release/release_alles.sh, release_alles_lib.sh, release_alles.cmd
# =============================================================================
# Nutzer 2026-10-16: "... die Ports für alle Systeme zu bauen - der neuen Version eine neue Versionsnummer - minor -
# zukommen zu lassen und die alte ins Archiv zu bringen."  Dossier analysis/befunde_runde38/V_releaseskript.md.
#
# Aufruf: bash test_r38_releaseskript.sh <repo> <arbeitsordner> <teil>
#   version    Versionslogik (Unit): Folgeversion, Uebertrag bei 99 (versionCode build.gradle:43-49 streng steigend),
#              hoechster Tag (numerisch, Fremd-Tags), Gueltigkeit, Vergleich
#   bausteine  SHA256SUMS mit CRLF, Archiv anlegen (erst .unvollstaendig, dann fertig), Frische-Pruefung
#   trocken    Trockenlauf im Wegwerf-Repo mit ECHTEM release_alles.sh: zeigt Version + jeden Schritt, schreibt
#              NICHTS (Baum, Index, Archiv bytegleich), ruft keinen Bau-Schritt; Versionswahl ueber Tags
#   ablauf     ganzer Lauf mit Attrappen der Schritt-Skripte: Reihenfolge, Version an Android und Paket, alte
#              Version ins Archiv, neue ins Archiv (sha256sum -c), alte APK aus release/, kein Commit ohne --tag;
#              dann --ab abschluss --tag: Commit + annotierter Tag + Push in ein lokales origin
#   fehler_bau     jeder simulierte Schritt-Fehler bricht ab, spaetere Schritte laufen nicht (auch durch die
#                  tee-Pipe): Rueckgabe != 0, altes Artefakt, kein Artefakt, fehlende Marke, fehlender Android-Satz,
#                  alte Volumes geblieben, Paket rot; Wiederaufnahme mit --ab
#   fehler_umfeld  Archiv-Konflikt (vor dem Bauen und im Schritt), kaputtes/anderes Altarchiv, schmutziger Baum,
#                  Werkzeug, unversionierte Datei, Sperre, --tag-Voraussetzungen, Docker, Platz, Aufruffehler,
#                  Teillauf --bis, --tag nach Code-Commit verweigert, --kein-push
#   cmd        (Windows) release_alles.cmd: Git-Bash gefunden, Argumente + Rueckgabe durchgereicht,
#              Doppelklick-Weg (ohne Argumente): Trockenlauf, Rueckfrage "N" -> nichts gebaut
# Die Attrappen (attrappen/*.sh) ersetzen NUR die Schritt-Skripte; release_alles.sh und die Bausteine sind echt.
# Rueckgabe 0 = alles gruen.
# =============================================================================
set -uo pipefail
REPO_ECHT="$(cd "$1" && pwd)"
ARBEIT="$2"
TEIL="$3"
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p "$ARBEIT"
ARBEIT="$(cd "$ARBEIT" && pwd)"

FEHLER=0
N=0
ok()   { N=$((N + 1)); echo "  ok   $*"; }
fehl() { N=$((N + 1)); FEHLER=$((FEHLER + 1)); echo "  FEHL $*"; }
pruef() { local t="$1"; shift; if "$@"; then ok "$t"; else fehl "$t"; fi; }
hat()       { grep -qF -- "$2" "$1"; }
hat_nicht() { ! grep -qF -- "$2" "$1"; }
gleich()    { [[ "$1" == "$2" ]]; }
zeige()     { echo "  ---- $1 (Ende) ----"; tail -n "${3:-25}" "$2" | sed 's/^/  | /'; }

IST_WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) IST_WIN=1 ;; esac

# --- Wegwerf-Repo ----------------------------------------------------------------------
# fx_bauen <ordner> <tags...>: repo/ (master, ein Commit, Tags), origin.git/ (bare, Push-Ziel), archiv/ (leer).
# release/ traegt die Vorversion v0.8.25 wie nach einem Release: 6 Volumes (getrackt), SHA256SUMS.txt und
# SHA256SUMS_android.txt mit CRLF (so liegen sie in einem frisch ausgecheckten Baum, git ls-files --eol w/crlf)
# und die APK (gitignoriert).
fx_bauen() {
    local fx="$1"; shift
    rm -rf "$fx"
    mkdir -p "$fx/archiv"
    R="$fx/repo"; A="$fx/archiv"; P="$fx/aufrufe.txt"
    git init -q --bare "$fx/origin.git"
    git init -q "$R"
    git -C "$R" symbolic-ref HEAD refs/heads/master
    git -C "$R" config user.name r38test
    git -C "$R" config user.email r38test@localhost
    git -C "$R" config core.autocrlf false
    git -C "$R" config commit.gpgsign false
    git -C "$R" config tag.gpgsign false
    mkdir -p "$R/re15_port/engine/src" "$R/synchro/STAGE1/room1170" "$R/release"
    echo 'int r38_attrappe;' > "$R/re15_port/engine/src/a.c"
    printf 'RIFF' > "$R/synchro/STAGE1/room1170/main00.wav"
    cp "$REPO_ECHT/release/release_alles.sh" "$REPO_ECHT/release/release_alles_lib.sh" "$R/release/"
    cp "$REPO_ECHT/release/release_alles.cmd" "$R/release/"
    cp "$HIER/attrappen/"*.sh "$R/release/"
    printf '# v0.8.25 - Testfassung\n\nalt\n' > "$R/release/RELEASE_NOTES.md"
    printf 'release/*_android.apk\nrelease/logs/\nrelease/win_out/\nrelease/linux_out/\n' > "$R/.gitignore"
    (
        cd "$R/release" || exit 1
        for p in android linux_steamdeck_x64 win64; do
            printf 'alt z01 %s' "$p" > "re15_port_v0.8.25_$p.z01"
            printf 'alt zip %s' "$p" > "re15_port_v0.8.25_$p.zip"
        done
        sha256sum re15_port_v0.8.25_*.z* | sed 's/$/\r/' > SHA256SUMS.txt
        printf 'alte apk v0.8.25' > re15_port_v0.8.25_android.apk
        sha256sum -b re15_port_v0.8.25_android.apk | sed 's/$/\r/' > SHA256SUMS_android.txt
    )
    git -C "$R" add -A
    git -C "$R" commit -q -m "fixture"
    local t
    for t in "$@"; do git -C "$R" tag "$t"; done
    git -C "$R" remote add origin "$fx/origin.git"
    git -C "$R" push -q origin master --tags 2>/dev/null
    export FAKE_PROTOKOLL="$P"
    : > "$P"
}
# ra <ausgabe> <argumente...>: release_alles.sh im Wegwerf-Repo, Rueckgabe in RC
ra() {
    local out="$1"; shift
    bash "$R/release/release_alles.sh" --archiv "$A" "$@" > "$out" 2>&1
    RC=$?
}
schnappschuss() {   # Inhalt von Baum (ohne .git), Index und Archiv als eine Pruefsumme
    {
        git -C "$R" status --porcelain=v1 --untracked-files=all --ignored
        git -C "$R" ls-files -s
        git -C "$R" rev-parse HEAD
        git -C "$R" tag --list
        ( cd "$R" && find . -path ./.git -prune -o -type f -print | LC_ALL=C sort | while IFS= read -r f; do sha256sum "$f"; done )
        ( cd "$A" && find . -print | LC_ALL=C sort )
    } | sha256sum | cut -d' ' -f1
}
aufrufe() { grep -vE '^docker' "$P" 2>/dev/null | tr '\n' ';'; }

# =============================================================================
teil_version() {
    # shellcheck source=/dev/null
    source "$REPO_ECHT/release/release_alles_lib.sh"
    echo "== Versionslogik (release/release_alles_lib.sh) =="
    local v soll ist
    while read -r v soll; do
        ist="$(ra_naechste_version "$v" 2>/dev/null)" || ist=FEHLER
        pruef "Folgeversion ${v:-<leer>} -> $soll (ist $ist)" gleich "$ist" "$soll"
    done <<'EOF'
v0.8.25 v0.8.26
v0.8.9 v0.8.10
v0.8.98 v0.8.99
v0.7.99 v0.8.0
v0.3.99 v0.4.0
v0.8.0 v0.8.1
v1.2.3 v1.2.4
v0.99.98 v0.99.99
v0.99.99 FEHLER
v0.8 FEHLER
v0.8.25-3-gabc FEHLER
EOF
    while read -r v soll; do
        ist="$(ra_version_normal "$v" 2>/dev/null)" || ist=FEHLER
        pruef "Gueltig $v -> $soll (ist $ist)" gleich "$ist" "$soll"
    done <<'EOF'
v0.8.26 v0.8.26
0.8.26 v0.8.26
v999.99.99 v999.99.99
v0.08.1 FEHLER
v0.8.100 FEHLER
v0.100.0 FEHLER
v1000.0.0 FEHLER
v0.8.26-rc1 FEHLER
V0.8.26 FEHLER
v0.8 FEHLER
EOF
    while read -r a b soll; do
        ist="$(ra_version_vergleich "$a" "$b")"
        pruef "Vergleich $a ? $b = $soll (ist $ist)" gleich "$ist" "$soll"
    done <<'EOF'
v0.8.10 v0.8.9 1
v0.9.0 v0.8.99 1
v0.8.25 v0.8.25 0
v0.8.25 v0.8.26 -1
v1.0.0 v0.99.99 1
EOF
    ist="$(printf 'v0.1\nv0.2\nv0.1.1\nv0.8.9\nv0.8.25\nv0.8.10\nbackup-before-pull-ed20f6d3\nitembox-hybrid-v1\nv0.7.99\n' | ra_hoechste_version)"
    pruef "hoechster Tag aus der Liste mit Fremd-Tags (v0.1, v0.2, backup-..., itembox-...) = v0.8.25 (ist $ist)" gleich "$ist" v0.8.25
    ist="$(printf 'v0.8.9\nv0.8.10\n' | ra_hoechste_version)"
    pruef "hoechster Tag numerisch, nicht lexikalisch: v0.8.10 > v0.8.9 (ist $ist)" gleich "$ist" v0.8.10
    ist="$(printf 'v0.8.9\r\nv0.8.11\r\n' | ra_hoechste_version)"
    pruef "Tag-Liste mit CRLF (ist $ist)" gleich "$ist" v0.8.11
    if printf 'v0.1\nv0.2\nfoo\n' | ra_hoechste_version >/dev/null 2>&1; then fehl "ohne vX.Y.Z-Tag muss Rueckgabe 1 kommen"
    else ok "ohne vX.Y.Z-Tag: Rueckgabe 1 (Version muss dann per --version kommen)"; fi

    # Der Grund fuer den Uebertrag bei 99: Androids versionCode (build.gradle:43-49) muss mit JEDER Folgeversion
    # streng steigen, sonst lehnt das Geraet das Update ab (build_android.sh:195-199). 300 Folgeversionen ab v0.7.90
    # (ueber v0.7.99 -> v0.8.0, v0.8.99 -> v0.9.0, v0.9.99 -> v0.10.0).
    local cur="v0.7.90" code prev=-1 i mono=1 t
    for (( i = 0; i < 300; i++ )); do
        t=($(ra_version_teile "$cur"))
        code=$(( t[0] * 1000000 + t[1] * 10000 + t[2] * 100 ))
        if (( code <= prev )); then mono=0; echo "    versionCode faellt/steht bei $cur: $code <= $prev"; fi
        prev=$code
        cur="$(ra_naechste_version "$cur")" || { mono=0; echo "    keine Folgeversion nach $cur"; break; }
    done
    pruef "versionCode streng steigend ueber 300 Folgeversionen (zuletzt $cur)" gleich "$mono" 1
    t=($(ra_version_teile v0.8.100))
    pruef "Gegenprobe: ohne Uebertrag ergaebe v0.8.100 denselben versionCode wie v0.9.0 (90000)" \
        gleich "$(( t[0] * 1000000 + t[1] * 10000 + t[2] * 100 ))" 90000

    # Die echten Tags dieses Repos (wenn lesbar): unabhaengig mit sort -V nachgerechnet
    local echt; echt="$(git -C "$REPO_ECHT" tag --list 'v*' 2>/dev/null || true)"
    if [[ -n "$echt" ]]; then
        local soll_echt ist_echt
        soll_echt="$(printf '%s\n' "$echt" | grep -E '^v[0-9]+\.[0-9]+\.[0-9]+$' | sort -V | tail -1)"
        ist_echt="$(printf '%s\n' "$echt" | ra_hoechste_version)"
        pruef "echte Repo-Tags: hoechster = $soll_echt (sort -V), ra_hoechste_version = $ist_echt" gleich "$ist_echt" "$soll_echt"
    else
        echo "  (echte Repo-Tags nicht lesbar - Zusatzpruefung entfaellt, die Liste oben bleibt Pflicht)"
    fi

    while read -r f soll; do
        ist="$(ra_volumen_version "$f" 2>/dev/null)" || ist=FEHLER
        pruef "Volume-Name $f -> $soll" gleich "$ist" "$soll"
    done <<'EOF'
re15_port_v0.8.25_win64.z01 v0.8.25
re15_port_v0.8.25_linux_steamdeck_x64.zip v0.8.25
release/re15_port_v0.8.26_android.z02 v0.8.26
re15_port_v0.8.25_android.apk FEHLER
SHA256SUMS.txt FEHLER
re15_port_v0.8.25_macos.zip FEHLER
EOF
}

# =============================================================================
teil_bausteine() {
    # shellcheck source=/dev/null
    source "$REPO_ECHT/release/release_alles_lib.sh"
    echo "== Bausteine: SHA256SUMS, Archiv, Frische =="
    local d="$ARBEIT/bausteine" erg rc
    rm -rf "$d"; mkdir -p "$d/quelle" "$d/archiv"
    ( cd "$d/quelle" && printf 'eins' > re15_port_v0.8.26_win64.z01 && printf 'zwei' > re15_port_v0.8.26_win64.zip \
        && sha256sum re15_port_v0.8.26_win64.z* | sed 's/$/\r/' > SHA256SUMS.txt \
        && printf 'apk' > re15_port_v0.8.26_android.apk && sha256sum -b re15_port_v0.8.26_android.apk | sed 's/$/\r/' > SHA256SUMS_android.txt )
    # Gegenprobe des Befunds: sha256sum -c direkt auf der CRLF-Datei scheitert
    if ( cd "$d/quelle" && sha256sum -c SHA256SUMS.txt >/dev/null 2>&1 ); then
        echo "  (Hinweis: dieses sha256sum liest CRLF selbst - die Gegenprobe greift hier nicht)"
    else
        ok "Gegenprobe: sha256sum -c auf der CRLF-Datei scheitert (Befund 2026-10-16)"
    fi
    erg="$(ra_sums_pruefen "$d/quelle" SHA256SUMS.txt 2>&1)"; rc=$?
    pruef "ra_sums_pruefen liest CRLF: '$erg'" gleich "$rc:$erg" "0:2/2 OK"
    ist="$(ra_sums_namen "$d/quelle/SHA256SUMS_android.txt")"
    pruef "ra_sums_namen ohne '*' und ohne CR: '$ist'" gleich "$ist" re15_port_v0.8.26_android.apk
    erg="$(ra_archiv_anlegen "$d/quelle" "$d/archiv" v0.8.26 "$d/quelle/SHA256SUMS.txt" "$d/quelle/SHA256SUMS_android.txt" re15_port_v0.8.26_android.apk 2>&1)"; rc=$?
    pruef "ra_archiv_anlegen: rc 0, beide SUMS geprueft ('$erg')" gleich "$rc:$erg" "0:SHA256SUMS.txt 2/2 OK; SHA256SUMS_android.txt 1/1 OK"
    pruef "Archivordner fertig, kein .unvollstaendig" test -d "$d/archiv/v0.8.26" -a ! -e "$d/archiv/.v0.8.26.unvollstaendig"
    pruef "SUMS im Archiv ohne CR" bash -c "! grep -q $'\r' '$d/archiv/v0.8.26/SHA256SUMS.txt'"
    pruef "Archivkopie bytegleich (APK)" cmp -s "$d/quelle/re15_port_v0.8.26_android.apk" "$d/archiv/v0.8.26/re15_port_v0.8.26_android.apk"
    erg="$(ra_archiv_anlegen "$d/quelle" "$d/archiv" v0.8.26 "$d/quelle/SHA256SUMS.txt" 2>&1)"; rc=$?
    pruef "vorhandener Archivordner wird nicht ueberschrieben (rc $rc)" gleich "$rc" 1
    # kaputte Quelle: die Kopie wird gepruefte, der Ordner NIE fertig
    printf 'falsch' > "$d/quelle/re15_port_v0.8.26_win64.zip"
    erg="$(ra_archiv_anlegen "$d/quelle" "$d/archiv" v0.8.27 "$d/quelle/SHA256SUMS.txt" 2>&1)"; rc=$?
    pruef "veraenderte Quelle: rc 1 und KEIN fertiger Ordner v0.8.27" test "$rc" = 1 -a ! -e "$d/archiv/v0.8.27"
    pruef "veraenderte Quelle: Meldung nennt den Fehler" hat <(printf '%s\n' "$erg") "FAILED"
    erg="$(ra_archiv_pruefen "$d/archiv/v0.8.26" 2>&1)"; rc=$?
    pruef "ra_archiv_pruefen auf dem guten Ordner rc 0" gleich "$rc" 0
    printf 'x' >> "$d/archiv/v0.8.26/re15_port_v0.8.26_win64.z01"
    erg="$(ra_archiv_pruefen "$d/archiv/v0.8.26" 2>&1)"; rc=$?
    pruef "ra_archiv_pruefen findet ein veraendertes Volume (rc $rc)" gleich "$rc" 1
    : > "$d/leer.txt"
    pruef "leere SUMS-Datei gilt nicht als gruen" bash -c "source '$REPO_ECHT/release/release_alles_lib.sh'; ! ra_sums_pruefen '$d' leer.txt 2>/dev/null"
    # Frische
    local t0; t0=$(date +%s)
    printf 'neu' > "$d/neu.bin"
    pruef "frisches Artefakt gilt" ra_artefakt_frisch "$d/neu.bin" "$t0"
    printf 'alt' > "$d/alt.bin"; touch -d '2020-01-01 00:00:00' "$d/alt.bin"
    erg="$(ra_artefakt_frisch "$d/alt.bin" "$t0" 2>&1)"; rc=$?
    pruef "altes Artefakt (2020) faellt durch: rc $rc" test "$rc" = 1
    pruef "Meldung 'AELTER als der Schrittbeginn'" hat <(printf '%s\n' "$erg") "AELTER als der Schrittbeginn"
    : > "$d/leer.bin"
    pruef "leeres Artefakt faellt durch" bash -c "source '$REPO_ECHT/release/release_alles_lib.sh'; ! ra_artefakt_frisch '$d/leer.bin' 0 2>/dev/null"
    pruef "fehlendes Artefakt faellt durch" bash -c "source '$REPO_ECHT/release/release_alles_lib.sh'; ! ra_artefakt_frisch '$d/gibtsnicht' 0 2>/dev/null"
}

# =============================================================================
teil_trocken() {
    echo "== Trockenlauf =="
    local fx="$ARBEIT/fx_trocken" o vorher nachher
    fx_bauen "$fx" v0.1 v0.2 v0.8.9 v0.8.24 v0.8.25 backup-before-pull-ed20f6d3
    o="$fx/out1.txt"
    vorher="$(schnappschuss)"
    ra "$o" --dry-run --min-frei-gb 1
    pruef "Trockenlauf rc 0 (ist $RC)" gleich "$RC" 0
    pruef "Version v0.8.26 aus hoechstem Tag v0.8.25" hat "$o" "Version: v0.8.26  (hoechster Tag v0.8.25 + 1)"
    pruef "Schritt alt: Vorversion v0.8.25 kommt ins Archiv" hat "$o" "Vorversion v0.8.25 fehlt im Archiv - Schritt 'alt' legt"
    pruef "Schritt windows mit Befehl" hat "$o" "bash release/build_win_cross.sh -> release/win_out/re15_pc.exe"
    pruef "Schritt android mit --version v0.8.26" hat "$o" "bash release/build_android.sh --version v0.8.26"
    pruef "Schritt linux mit Befehl" hat "$o" "bash release/build_linux_deck.sh -> release/linux_out/re15_pc"
    pruef "Schritt paket mit --version v0.8.26" hat "$o" "bash release/make_package.sh --version v0.8.26"
    pruef "Schritt archiv: im Archiv wird nichts geloescht" hat "$o" "im Archiv wird nichts geloescht"
    pruef "ohne --tag kein Commit/Tag/Push" hat "$o" "ohne --tag: kein Release-Commit, kein Tag, kein Push"
    pruef "Schlusszeile TROCKENLAUF-OK v0.8.26" hat "$o" "TROCKENLAUF-OK v0.8.26"
    nachher="$(schnappschuss)"
    pruef "Trockenlauf schreibt nichts (Baum, Index, Tags, Archiv unveraendert)" gleich "$vorher" "$nachher"
    pruef "Trockenlauf ruft keinen Bau-/Paket-Schritt (Aufrufe: '$(aufrufe)')" gleich "$(aufrufe)" ""
    pruef "Trockenlauf prueft Docker nur (--pruefen)" bash -c "grep -qx 'docker --pruefen' '$P' && ! grep -qx 'docker ' '$P'"
    pruef "kein Protokollordner release/logs" test ! -e "$R/release/logs"
    [[ $FEHLER -eq 0 ]] || zeige "out1" "$o" 40

    o="$fx/out2.txt"; ra "$o" --dry-run --min-frei-gb 1 --tag
    pruef "--tag ohne Notes-Abschnitt: rc 1 (ist $RC)" gleich "$RC" 1
    pruef "--tag ohne Notes: [XX]-Zeile nennt RELEASE_NOTES" hat "$o" "[XX] --tag: release/RELEASE_NOTES.md beginnt nicht mit '# v0.8.26 ...'"
    pruef "--tag: Schritt abschluss im Plan" hat "$o" "git commit -F (release: v0.8.26), git tag -a v0.8.26, git push origin master v0.8.26"
    o="$fx/out3.txt"; ra "$o" --dry-run --min-frei-gb 1 --version 0.9.0
    pruef "--version 0.9.0 -> v0.9.0" hat "$o" "Version: v0.9.0  (per --version; hoechster Tag v0.8.25)"
    o="$fx/out4.txt"; ra "$o" --dry-run --min-frei-gb 1 --version v0.8.25
    pruef "--version v0.8.25 (= hoechster Tag): rc 2 (ist $RC)" gleich "$RC" 2
    pruef "Meldung: nicht groesser (Downgrade)" hat "$o" "ist nicht groesser als der hoechste Tag v0.8.25"
    o="$fx/out5.txt"; ra "$o" --dry-run --min-frei-gb 1 --version v0.8.100
    pruef "--version v0.8.100: rc 2 (ist $RC)" gleich "$RC" 2
    o="$fx/out6.txt"; ra "$o" --dry-run --min-frei-gb 1 --ab paket
    pruef "--ab paket: Plan nur paket archiv" hat "$o" "Plan:    paket archiv"
    pruef "--ab paket ohne Bau-Ergebnisse: rc 1 (ist $RC)" gleich "$RC" 1
    pruef "--ab paket: fehlendes Windows-Ergebnis als [XX]" hat "$o" "[XX] windows uebersprungen, aber"
    o="$fx/out6b.txt"; ra "$o" --dry-run --min-frei-gb 1 --ab windows --bis windows
    pruef "--ab windows --bis windows: Plan nur windows, rc 0 (ist $RC)" bash -c "[[ $RC == 0 ]] && grep -qxF 'Plan:    windows' '$o'"
    o="$fx/out6c.txt"; ra "$o" --dry-run --min-frei-gb 1 --ab linux --bis windows
    pruef "--bis vor --ab: rc 2 (ist $RC)" gleich "$RC" 2
    o="$fx/out7.txt"; FAKE_DOCKER=nein ra "$o" --dry-run --min-frei-gb 1
    pruef "Docker nicht bereit und nicht startbar: rc 1 (ist $RC)" gleich "$RC" 1
    printf '\n// geaendert\n' >> "$R/re15_port/engine/src/a.c"
    o="$fx/out8.txt"; ra "$o" --dry-run --min-frei-gb 1
    pruef "schmutziges re15_port/: rc 1 (ist $RC)" gleich "$RC" 1
    pruef "schmutziges re15_port/: [XX] mit Dateiliste" hat "$o" "re15_port/engine/src/a.c"
    git -C "$R" checkout -q -- re15_port

    fx_bauen "$fx" v0.7.98 v0.7.99
    o="$fx/out9.txt"; ra "$o" --dry-run --min-frei-gb 1
    pruef "Tags bis v0.7.99 -> v0.8.0 (wie die Geschichte)" hat "$o" "Version: v0.8.0  (hoechster Tag v0.7.99 + 1)"
    fx_bauen "$fx" v0.99.99
    o="$fx/out10.txt"; ra "$o" --dry-run --min-frei-gb 1
    pruef "v0.99.99: keine automatische Folgeversion, rc 2 (ist $RC)" gleich "$RC" 2
    pruef "v0.99.99: Meldung verlangt --version" hat "$o" "mit --version angeben"
}

# =============================================================================
teil_ablauf() {
    echo "== Ganzer Lauf mit Attrappen =="
    local fx="$ARBEIT/fx_ablauf" o head0 logdir f
    fx_bauen "$fx" v0.8.24 v0.8.25
    head0="$(git -C "$R" rev-parse HEAD)"
    o="$fx/out1.txt"
    ra "$o" --min-frei-gb 1 --linux-jobs 3
    pruef "Lauf rc 0 (ist $RC)" gleich "$RC" 0
    pruef "Schlusszeile RELEASE-ALLES-OK v0.8.26" hat "$o" "RELEASE-ALLES-OK v0.8.26"
    pruef "Reihenfolge + Version: windows, android --version v0.8.26, linux (3 Jobs), paket --version v0.8.26" \
        gleich "$(aufrufe)" "windows  jobs=;android --version v0.8.26 jobs=;linux  jobs=3;paket --version v0.8.26 jobs=;"
    pruef "Docker wurde im echten Lauf bereitgestellt (ohne --pruefen)" grep -qx 'docker ' "$P"
    # die alte Version im Archiv
    pruef "Archiv v0.8.25 (die alte) angelegt" test -d "$A/v0.8.25"
    pruef "Archiv v0.8.25: 6 Volumes + APK + 2 SUMS" gleich "$(ls "$A/v0.8.25" | grep -c '')" 9
    pruef "Archiv v0.8.25: sha256sum -c beider SUMS gruen" bash -c "cd '$A/v0.8.25' && sha256sum -c SHA256SUMS.txt && sha256sum -c SHA256SUMS_android.txt"
    pruef "Archiv v0.8.25: SUMS ohne CR (Quelle war CRLF)" bash -c "! grep -q $'\r' '$A/v0.8.25/SHA256SUMS.txt'"
    pruef "Archiv v0.8.25: alte APK bytegleich" bash -c "printf 'alte apk v0.8.25' | cmp -s - '$A/v0.8.25/re15_port_v0.8.25_android.apk'"
    # die neue Version im Archiv
    pruef "Archiv v0.8.26 angelegt" test -d "$A/v0.8.26"
    pruef "Archiv v0.8.26: 3 Plattformen je .z01 + .zip" bash -c "ls '$A/v0.8.26' | grep -cE '^re15_port_v0\.8\.26_(win64|linux_steamdeck_x64|android)\.(z01|zip)$' | grep -qx 6"
    pruef "Archiv v0.8.26: sha256sum -c beider SUMS gruen" bash -c "cd '$A/v0.8.26' && sha256sum -c SHA256SUMS.txt && sha256sum -c SHA256SUMS_android.txt"
    for f in re15_port_v0.8.26_win64.zip re15_port_v0.8.26_android.apk; do
        pruef "Archiv v0.8.26: $f bytegleich mit release/" cmp -s "$R/release/$f" "$A/v0.8.26/$f"
    done
    pruef "kein .unvollstaendig im Archiv" bash -c "! ls -a '$A' | grep -q unvollstaendig"
    # release/
    pruef "release/: nur noch Volumes von v0.8.26 (6)" gleich "$(cd "$R/release" && LC_ALL=C ls re15_port_v*.z* | tr '\n' ' ')" \
        "re15_port_v0.8.26_android.z01 re15_port_v0.8.26_android.zip re15_port_v0.8.26_linux_steamdeck_x64.z01 re15_port_v0.8.26_linux_steamdeck_x64.zip re15_port_v0.8.26_win64.z01 re15_port_v0.8.26_win64.zip "
    pruef "release/: alte APK weg (liegt im Archiv)" test ! -e "$R/release/re15_port_v0.8.25_android.apk"
    pruef "release/: neue APK da" test -s "$R/release/re15_port_v0.8.26_android.apk"
    pruef "Meldung: alte APK entfernt, liegt bytegleich im Archiv" hat "$o" "release/re15_port_v0.8.25_android.apk entfernt - liegt bytegleich in"
    # git
    pruef "ohne --tag kein Commit" gleich "$(git -C "$R" rev-parse HEAD)" "$head0"
    pruef "ohne --tag kein Tag v0.8.26" bash -c "! git -C '$R' rev-parse -q --verify refs/tags/v0.8.26 >/dev/null"
    pruef "Index: neue Volumes vorgemerkt, alte entfernt" \
        gleich "$(git -C "$R" diff --cached --no-renames --name-status | grep -c '^[AD]	release/re15_port_v0\.8\.2[56]_')" 12
    pruef "Hinweis auf --ab abschluss --tag" hat "$o" "bash release/release_alles.sh --version v0.8.26 --ab abschluss --tag"
    # Protokolle
    logdir="$(ls -d "$R"/release/logs/v0.8.26_* 2>/dev/null | head -1)"
    pruef "Protokollordner release/logs/v0.8.26_<zeit>" test -n "$logdir"
    for f in 00_ablauf 01_alt 02_windows 03_android 04_linux 05_paket 06_archiv; do
        pruef "Protokoll $f.log" test -s "$logdir/$f.log"
    done
    pruef "Windows-Protokoll traegt die Bau-Ausgabe" hat "$logdir/02_windows.log" "WIN-CROSS-BUILD-OK"
    pruef "Linux-Zaehlzeile in der Zusammenfassung" hat "$o" "Tests: 5 gelaufen = 5 registriert"
    pruef "Sperre nach dem Lauf entfernt" test ! -e "$R/release/logs/.release_alles.sperre"
    pruef "Protokolle, Sperre, Bau-Ausgaben, APK erzeugen keine unversionierte Datei" \
        gleich "$(git -C "$R" status --porcelain --untracked-files=all | grep '^??' | tr '\n' ';')" ""
    [[ $FEHLER -eq 0 ]] || zeige "out1" "$o" 60

    # --- Abschluss: Notes schreibt der Mensch, dann --ab abschluss --tag
    printf '# v0.8.26 - Testfassung\n\nneu\n\n# v0.8.25 - Testfassung\n\nalt\n' > "$R/release/RELEASE_NOTES.md"
    : > "$P"
    o="$fx/out2.txt"
    ra "$o" --min-frei-gb 1 --version v0.8.26 --ab abschluss --tag
    pruef "abschluss --tag rc 0 (ist $RC)" gleich "$RC" 0
    pruef "abschluss ruft keinen Bau-Schritt" gleich "$(aufrufe)" ""
    pruef "Release-Commit 'release: v0.8.26 (Windows + Linux/Steam Deck + Android)'" \
        gleich "$(git -C "$R" log -1 --format=%s)" "release: v0.8.26 (Windows + Linux/Steam Deck + Android)"
    pruef "Commit: 6 neue + 6 alte Volumes, 2 SUMS, Notes (15 Pfade)" \
        gleich "$(git -C "$R" show --no-renames --name-only --format= HEAD | grep -c '')" 15
    pruef "Commit enthaelt release/SHA256SUMS_android.txt" bash -c "git -C '$R' show --name-only --format= HEAD | grep -qx release/SHA256SUMS_android.txt"
    pruef "Tag v0.8.26 annotiert" gleich "$(git -C "$R" cat-file -t v0.8.26)" tag
    pruef "Tag v0.8.26 zeigt auf den Release-Commit" gleich "$(git -C "$R" rev-parse 'v0.8.26^{commit}')" "$(git -C "$R" rev-parse HEAD)"
    pruef "Push: origin master = Release-Commit" gleich "$(git -C "$fx/origin.git" rev-parse master)" "$(git -C "$R" rev-parse HEAD)"
    pruef "Push: origin kennt Tag v0.8.26" git -C "$fx/origin.git" rev-parse -q --verify refs/tags/v0.8.26
    pruef "Baum danach sauber" gleich "$(git -C "$R" status --porcelain | tr '\n' ';')" ""
    [[ $FEHLER -eq 0 ]] || zeige "out2" "$o" 40
    # --- naechster Lauf: v0.8.27, die nun alte v0.8.26 liegt schon im Archiv
    o="$fx/out3.txt"; ra "$o" --dry-run --min-frei-gb 1
    pruef "danach: naechste Version v0.8.27" hat "$o" "Version: v0.8.27  (hoechster Tag v0.8.26 + 1)"
    pruef "danach: Vorversion v0.8.26 liegt im Archiv (wird geprueft)" hat "$o" "Vorversion v0.8.26 liegt im Archiv"
}

# =============================================================================
teil_fehler_bau() {
    echo "== Fehlerpfade: Bau- und Paket-Schritte =="
    local fx="$ARBEIT/fx_fehler" o
    # 1. Linux rot (Rueckgabe 7 durch die tee-Pipe) -> Abbruch, paket/archiv laufen nicht
    fx_bauen "$fx" v0.8.25
    o="$fx/linux_rot.txt"; FAKE_FAIL=linux ra "$o" --min-frei-gb 1
    pruef "linux rot: rc 1 (ist $RC)" gleich "$RC" 1
    pruef "linux rot: Abbruch nennt Schritt und Rueckgabe 7" hat "$o" "!!! ABBRUCH in Schritt 'linux': build_linux_deck.sh endete mit Rueckgabe 7"
    pruef "linux rot: Schlusszeile RELEASE-ALLES-ABBRUCH (linux)" hat "$o" "RELEASE-ALLES-ABBRUCH (linux)"
    pruef "linux rot: paket NICHT aufgerufen (Aufrufe '$(aufrufe)')" gleich "$(aufrufe)" "windows  jobs=;android --version v0.8.26 jobs=;linux  jobs=;"
    pruef "linux rot: kein Archiv v0.8.26" test ! -e "$A/v0.8.26"
    pruef "linux rot: release/ traegt weiter v0.8.25" test -f "$R/release/re15_port_v0.8.25_win64.zip"
    pruef "linux rot: Fehlerausgabe (stderr) im Schritt-Protokoll" bash -c "grep -q 'simulierter Fehlschlag' '$R'/release/logs/v0.8.26_*/04_linux.log"
    pruef "linux rot: Wiederaufnahme-Hinweis --ab linux" hat "$o" "bash release/release_alles.sh --version v0.8.26 --ab linux"
    pruef "linux rot: Sperre entfernt" test ! -e "$R/release/logs/.release_alles.sperre"
    pruef "linux rot: die alte Version kam trotzdem schon ins Archiv (Schritt alt vor dem Bauen)" test -d "$A/v0.8.25"
    #    Wiederaufnahme: nur linux + paket + archiv laufen
    : > "$P"; o="$fx/linux_weiter.txt"; ra "$o" --min-frei-gb 1 --version v0.8.26 --ab linux
    pruef "Wiederaufnahme --ab linux: rc 0 (ist $RC)" gleich "$RC" 0
    pruef "Wiederaufnahme: nur linux und paket gebaut ('$(aufrufe)')" gleich "$(aufrufe)" "linux  jobs=;paket --version v0.8.26 jobs=;"
    pruef "Wiederaufnahme: Archiv v0.8.26 geprueft" bash -c "cd '$A/v0.8.26' && sha256sum -c SHA256SUMS.txt >/dev/null"

    # 2. Windows rot -> nicht einmal android
    fx_bauen "$fx" v0.8.25
    o="$fx/win_rot.txt"; FAKE_FAIL=windows ra "$o" --min-frei-gb 1
    pruef "windows rot: rc 1, nur windows aufgerufen ('$(aufrufe)')" gleich "$RC:$(aufrufe)" "1:windows  jobs=;"
    # 3. altes Artefakt (Zeitstempel 2020) darf nicht als neues durchgehen
    fx_bauen "$fx" v0.8.25
    o="$fx/win_alt.txt"; FAKE_ALT=windows ra "$o" --min-frei-gb 1
    pruef "altes Windows-Artefakt: rc 1 (ist $RC)" gleich "$RC" 1
    pruef "altes Windows-Artefakt: Meldung AELTER als der Schrittbeginn" hat "$o" "AELTER als der Schrittbeginn"
    pruef "altes Windows-Artefakt: android nicht aufgerufen" gleich "$(aufrufe)" "windows  jobs=;"
    # 4. Rueckgabe 0 ohne Artefakt
    fx_bauen "$fx" v0.8.25
    o="$fx/and_kein.txt"; FAKE_KEIN=android ra "$o" --min-frei-gb 1
    pruef "android ohne APK trotz rc 0: Abbruch 'Artefakt fehlt'" bash -c "[[ $RC == 1 ]] && grep -qF 'Artefakt fehlt' '$o'"
    pruef "android ohne APK: linux nicht aufgerufen" gleich "$(aufrufe)" "windows  jobs=;android --version v0.8.26 jobs=;"
    # 5. Rueckgabe 0 + Artefakt, aber ohne Erfolgsmarke
    fx_bauen "$fx" v0.8.25
    o="$fx/lin_marke.txt"; FAKE_OHNE_MARKE=linux ra "$o" --min-frei-gb 1
    pruef "linux ohne LINUX-BUILD-OK: Abbruch Erfolgsmarke" bash -c "[[ $RC == 1 ]] && grep -qF \"Erfolgsmarke 'LINUX-BUILD-OK' fehlt\" '$o'"
    pruef "linux ohne Marke: paket nicht aufgerufen" hat_nicht "$P" "paket"
    # 6. make_package ohne Android-Satz (das echte tut das still, wenn die APK fehlt)
    fx_bauen "$fx" v0.8.25
    o="$fx/ohne_android.txt"; FAKE_OHNE_ANDROID=1 ra "$o" --min-frei-gb 1
    pruef "Paket ohne Android-Satz: Abbruch 'nicht alle Systeme'" bash -c "[[ $RC == 1 ]] && grep -qF 'Satz android fehlt' '$o'"
    pruef "Paket ohne Android-Satz: kein Archiv v0.8.26" test ! -e "$A/v0.8.26"
    # 7. make_package laesst alte Volumes liegen
    fx_bauen "$fx" v0.8.25
    o="$fx/alte_bleiben.txt"; FAKE_ALTE_BLEIBEN=1 ra "$o" --min-frei-gb 1
    pruef "alte Volumes in release/ geblieben: Abbruch" bash -c "[[ $RC == 1 ]] && grep -qF 'noch ein Volume einer anderen Version' '$o'"
    # 8. make_package rot
    fx_bauen "$fx" v0.8.25
    o="$fx/paket_rot.txt"; FAKE_FAIL=paket ra "$o" --min-frei-gb 1
    pruef "paket rot: Abbruch in Schritt paket, kein Archiv v0.8.26" bash -c "[[ $RC == 1 ]] && grep -qF \"ABBRUCH in Schritt 'paket'\" '$o' && [[ ! -e '$A/v0.8.26' ]]"
}

teil_fehler_umfeld() {
    echo "== Fehlerpfade: Archiv, Baum, Sperre, --tag, Docker, Platz, Aufruf, Teillauf =="
    local fx="$ARBEIT/fx_fehler" o
    # 9. Archivordner der neuen Version existiert schon -> ein Neubau wird VOR dem Bauen abgelehnt (die neuen Zips
    #    haetten andere Bytes, der Vergleich scheiterte sonst erst nach Stunden); der Ordner bleibt unangetastet
    fx_bauen "$fx" v0.8.25
    mkdir -p "$A/v0.8.26"; printf 'fremd' > "$A/v0.8.26/SHA256SUMS.txt"
    o="$fx/archiv_da.txt"; ra "$o" --min-frei-gb 1
    pruef "Archiv v0.8.26 schon da: Abbruch in vorbedingungen, nichts gebaut" \
        bash -c "[[ $RC == 1 ]] && grep -qF \"ABBRUCH in Schritt 'vorbedingungen'\" '$o' && [[ -z \"\$(grep -v '^docker' '$P')\" ]]"
    pruef "Archiv v0.8.26 schon da: Hinweis auf --ab abschluss --tag" hat "$o" "Nur veroeffentlichen: bash release/release_alles.sh --ab abschluss --tag"
    pruef "Archiv v0.8.26 schon da: fremder Ordner unangetastet" bash -c "[[ \$(cat '$A/v0.8.26/SHA256SUMS.txt') == fremd && \$(ls '$A/v0.8.26' | grep -c '') == 1 ]]"
    #    ohne Neubau (--ab archiv) vergleicht der Schritt: gleich = ok (wiederholbar), abweichend = Abbruch
    fx_bauen "$fx" v0.8.25
    o="$fx/voll_archiv.txt"; ra "$o" --min-frei-gb 1
    o="$fx/archiv_nochmal.txt"; ra "$o" --min-frei-gb 1 --version v0.8.26 --ab archiv
    pruef "--ab archiv mit gleichem Archiv: rc 0, 'lag schon gleich im Archiv' (ist $RC)" \
        bash -c "[[ $RC == 0 ]] && grep -qF 'v0.8.26 lag schon gleich im Archiv' '$o'"
    printf 'x' >> "$A/v0.8.26/SHA256SUMS.txt"
    o="$fx/archiv_konflikt.txt"; ra "$o" --min-frei-gb 1 --version v0.8.26 --ab archiv
    pruef "--ab archiv mit ANDEREM Archiv: Abbruch in Schritt archiv" \
        bash -c "[[ $RC == 1 ]] && grep -qF \"ABBRUCH in Schritt 'archiv'\" '$o' && grep -qF 'mit ANDEREM Inhalt' '$o'"
    # 10. das Archiv der ALTEN Version ist kaputt -> Abbruch VOR dem Bauen
    fx_bauen "$fx" v0.8.25
    mkdir -p "$A/v0.8.25"
    ( cd "$R/release" && cp re15_port_v0.8.25_* "$A/v0.8.25/" && tr -d '\r' < SHA256SUMS.txt > "$A/v0.8.25/SHA256SUMS.txt" )
    printf 'kaputt' >> "$A/v0.8.25/re15_port_v0.8.25_linux_steamdeck_x64.z01"
    o="$fx/altarchiv_kaputt.txt"; ra "$o" --min-frei-gb 1
    pruef "kaputtes Altarchiv: Abbruch in Schritt alt" bash -c "[[ $RC == 1 ]] && grep -qF \"ABBRUCH in Schritt 'alt'\" '$o' && grep -qF 'FEHLERHAFT' '$o'"
    pruef "kaputtes Altarchiv: nichts gebaut" gleich "$(aufrufe)" ""
    # 11. im Archiv liegt ein ANDERES v0.8.25 (anderer Bau derselben Version)
    fx_bauen "$fx" v0.8.25
    mkdir -p "$A/v0.8.25"; ( cd "$A/v0.8.25" && printf 'x' > a.zip && sha256sum a.zip > SHA256SUMS.txt )
    o="$fx/altarchiv_anders.txt"; ra "$o" --min-frei-gb 1
    pruef "anderes v0.8.25 im Archiv: Abbruch 'ANDERES'" bash -c "[[ $RC == 1 ]] && grep -qF 'im Archiv liegt ein ANDERES v0.8.25' '$o'"
    # 12. schmutziger Baum / Werkzeug -> Abbruch in den Vorbedingungen, nichts geschrieben
    fx_bauen "$fx" v0.8.25
    printf '\n' >> "$R/re15_port/engine/src/a.c"
    o="$fx/schmutzig.txt"; ra "$o" --min-frei-gb 1
    pruef "schmutziges re15_port/: Abbruch in vorbedingungen, kein Schritt, keine Protokolle" \
        bash -c "[[ $RC == 1 ]] && grep -qF \"ABBRUCH in Schritt 'vorbedingungen'\" '$o' && [[ ! -e '$R/release/logs' ]]"
    pruef "schmutziges re15_port/: nichts gebaut, Archiv leer" bash -c "[[ -z \"\$(grep -v '^docker' '$P')\" && -z \"\$(ls -A '$A')\" ]]"
    fx_bauen "$fx" v0.8.25
    printf '\n# geaendert\n' >> "$R/release/build_win_cross.sh"
    o="$fx/werkzeug.txt"; ra "$o" --min-frei-gb 1
    pruef "nicht committetes Release-Werkzeug: Abbruch" bash -c "[[ $RC == 1 ]] && grep -qF 'Release-Werkzeug in release/ nicht committet' '$o'"
    fx_bauen "$fx" v0.8.25
    mkdir -p "$R/re15_port/engine/src/neu"; printf 'int y;' > "$R/re15_port/engine/src/neu/b.c"
    o="$fx/unversioniert.txt"; ra "$o" --min-frei-gb 1
    pruef "unversionierte Datei in re15_port/ (GLOB nimmt sie mit): Abbruch" bash -c "[[ $RC == 1 ]] && grep -qF 're15_port/engine/src/neu/b.c' '$o'"
    # 13. Sperre: ein lebender Lauf (diese Test-Shell) -> Abbruch, fremde Sperre bleibt
    fx_bauen "$fx" v0.8.25
    mkdir -p "$R/release/logs/.release_alles.sperre"; echo "$$" > "$R/release/logs/.release_alles.sperre/pid"
    o="$fx/sperre.txt"; ra "$o" --min-frei-gb 1
    pruef "Sperre eines lebenden Laufs: Abbruch" bash -c "[[ $RC == 1 ]] && grep -qF 'ein anderer Lauf ist aktiv' '$o'"
    pruef "Sperre eines lebenden Laufs: bleibt stehen" test -f "$R/release/logs/.release_alles.sperre/pid"
    #     verwaiste Sperre (PID nicht mehr da) -> Lauf geht durch
    echo 999999 > "$R/release/logs/.release_alles.sperre/pid"
    o="$fx/sperre_alt.txt"; ra "$o" --min-frei-gb 1
    pruef "verwaiste Sperre: Lauf geht durch (rc $RC) mit Hinweis" bash -c "[[ $RC == 0 ]] && grep -qF 'verwaiste Sperre' '$o'"
    # 14. --tag: Notes fehlen / nicht master -> Abbruch VOR dem Bauen
    fx_bauen "$fx" v0.8.25
    o="$fx/tag_notes.txt"; ra "$o" --min-frei-gb 1 --tag
    pruef "--tag ohne Notes: Abbruch vor dem Bauen" bash -c "[[ $RC == 1 ]] && [[ -z \"\$(grep -v '^docker' '$P')\" ]]"
    fx_bauen "$fx" v0.8.25
    printf '# v0.8.26 - x\n' > "$R/release/RELEASE_NOTES.md"; git -C "$R" commit -q -am notes; git -C "$R" checkout -q -b zweig
    o="$fx/tag_zweig.txt"; ra "$o" --min-frei-gb 1 --tag
    pruef "--tag auf einem anderen Zweig: Abbruch" bash -c "[[ $RC == 1 ]] && grep -qF -- '--tag nur auf master (jetzt: zweig)' '$o'"
    # 15. Docker nicht bereit, zu wenig Platz
    fx_bauen "$fx" v0.8.25
    o="$fx/docker.txt"; FAKE_DOCKER=nein ra "$o" --min-frei-gb 1
    pruef "Docker nicht bereit: Abbruch in vorbedingungen" bash -c "[[ $RC == 1 ]] && grep -qF 'Docker nicht bereit' '$o'"
    o="$fx/platz.txt"; ra "$o" --min-frei-gb 999999
    pruef "zu wenig Platz: Abbruch" bash -c "[[ $RC == 1 ]] && grep -qF 'verlangt 999999' '$o'"
    # 16. Aufruffehler
    o="$fx/aufruf.txt"; ra "$o" --ab gibtsnicht
    pruef "--ab mit unbekanntem Schritt: rc 2" gleich "$RC" 2
    o="$fx/aufruf2.txt"; ra "$o" --kein-push
    pruef "--kein-push ohne --tag: rc 2" gleich "$RC" 2
    # 17. Teillauf --bis android: danach weiter mit --ab linux
    fx_bauen "$fx" v0.8.25
    o="$fx/bis.txt"; ra "$o" --min-frei-gb 1 --bis android
    pruef "--bis android: rc 0, Teillauf-Marke, kein 'fertig'" bash -c "[[ $RC == 0 ]] && grep -qF 'RELEASE-ALLES-TEILLAUF-OK v0.8.26 (alt windows android)' '$o' && ! grep -qF 'RELEASE-ALLES-OK' '$o'"
    pruef "--bis android: nur windows + android gebaut" gleich "$(aufrufe)" "windows  jobs=;android --version v0.8.26 jobs=;"
    pruef "--bis android: Hinweis 'weiter mit --ab linux'" hat "$o" "Weiter mit: bash release/release_alles.sh --version v0.8.26 --ab linux"
    # 18. --tag verweigert, wenn nach dem Paketbau Port-Code committet wurde (der Tag traege Code, den kein Paket hat)
    fx_bauen "$fx" v0.8.25
    o="$fx/voll.txt"; ra "$o" --min-frei-gb 1
    pruef "Vorlauf fuer 18: rc 0 (ist $RC)" gleich "$RC" 0
    printf '\n// spaeter\n' >> "$R/re15_port/engine/src/a.c"
    GIT_COMMITTER_DATE="$(( $(date +%s) + 120 )) +0000" git -C "$R" commit -q -m "code nach dem Paketbau" -- re15_port/engine/src/a.c
    printf '# v0.8.26 - x\n\n# v0.8.25 - Testfassung\n' > "$R/release/RELEASE_NOTES.md"
    o="$fx/tag_nach_code.txt"; ra "$o" --min-frei-gb 1 --version v0.8.26 --ab abschluss --tag
    pruef "Port-Code nach dem Paketbau: --tag bricht ab" bash -c "[[ $RC == 1 ]] && grep -qF 'nach dem Paketbau' '$o' && grep -qF 'code nach dem Paketbau' '$o'"
    pruef "Port-Code nach dem Paketbau: kein Tag v0.8.26" bash -c "! git -C '$R' rev-parse -q --verify refs/tags/v0.8.26 >/dev/null"
    # 19. --tag --kein-push: Commit + Tag lokal, origin unveraendert
    fx_bauen "$fx" v0.8.25
    o="$fx/voll2.txt"; ra "$o" --min-frei-gb 1
    printf '# v0.8.26 - x\n\n# v0.8.25 - Testfassung\n' > "$R/release/RELEASE_NOTES.md"
    local origin_vorher; origin_vorher="$(git -C "$fx/origin.git" rev-parse master)"
    o="$fx/kein_push.txt"; ra "$o" --min-frei-gb 1 --version v0.8.26 --ab abschluss --tag --kein-push
    pruef "--kein-push: rc 0, Tag lokal" bash -c "[[ $RC == 0 ]] && [[ \$(git -C '$R' cat-file -t v0.8.26) == tag ]]"
    pruef "--kein-push: origin unveraendert, ohne Tag" bash -c "[[ \$(git -C '$fx/origin.git' rev-parse master) == '$origin_vorher' ]] && ! git -C '$fx/origin.git' rev-parse -q --verify refs/tags/v0.8.26 >/dev/null"
}

# =============================================================================
teil_cmd() {
    echo "== release_alles.cmd (Windows) =="
    if [[ $IST_WIN -ne 1 ]]; then fehl "Teil cmd nur unter Windows registrieren"; return; fi
    local fx="$ARBEIT/fx_cmd" o cmdw
    fx_bauen "$fx" v0.8.24 v0.8.25
    cmdw="$(cygpath -w "$R/release/release_alles.cmd")"
    pruef "release_alles.cmd hat CRLF-Zeilenenden" bash -c "[[ \$(tr -cd '\r' < '$R/release/release_alles.cmd' | wc -c) -eq \$(wc -l < '$R/release/release_alles.cmd') ]]"
    o="$fx/cmd1.txt"
    MSYS_NO_PATHCONV=1 cmd.exe /c "$cmdw" --dry-run --archiv "$(cygpath -m "$A")" --min-frei-gb 1 > "$o" 2>&1 < /dev/null; RC=$?
    pruef "cmd --dry-run: rc 0 (ist $RC)" gleich "$RC" 0
    pruef "cmd --dry-run: Trockenlauf aus Git-Bash" hat "$o" "TROCKENLAUF-OK v0.8.26"
    pruef "cmd --dry-run: keine Pause/Rueckfrage mit Argumenten" hat_nicht "$o" "Release jetzt bauen"
    o="$fx/cmd2.txt"
    MSYS_NO_PATHCONV=1 cmd.exe /c "$cmdw" --dry-run --version v0.8.25 --archiv "$(cygpath -m "$A")" > "$o" 2>&1 < /dev/null; RC=$?
    pruef "cmd reicht die Rueckgabe durch (--version v0.8.25 -> 2, ist $RC)" gleich "$RC" 2
    # Doppelklick-Weg (ohne Argumente): Trockenlauf + Wahl [1-4]. Das Archiv kommt aus RE15_ARCHIV (Wegwerf-Ordner) -
    # ohne Argumente nimmt das Skript sonst das ECHTE Archiv. choice liest die Wahl aus der Pipe.
    local AW; AW="$(cygpath -m "$A")"
    o="$fx/cmd3.txt"
    printf '4\n' | RE15_ARCHIV="$AW" MSYS_NO_PATHCONV=1 cmd.exe /c "$cmdw" > "$o" 2>&1; RC=$?
    pruef "Doppelklick 4: Trockenlauf gezeigt (TROCKENLAUF-OK v0.8.26)" hat "$o" "TROCKENLAUF-OK v0.8.26"
    pruef "Doppelklick 4: Wahl angeboten" hat "$o" "Auswahl [1-4]"
    pruef "Doppelklick 4: rc 0 (ist $RC)" gleich "$RC" 0
    pruef "Doppelklick 4: nichts gebaut ('$(aufrufe)')" gleich "$(aufrufe)" ""
    pruef "Doppelklick 4: keine Protokolle, Archiv leer" bash -c "[[ ! -e '$R/release/logs' && -z \"\$(ls -A '$A')\" ]]"
    #   1: der ganze Lauf ohne Commit/Tag/Push
    o="$fx/cmd4.txt"
    printf '1\n' | RE15_ARCHIV="$AW" MSYS_NO_PATHCONV=1 cmd.exe /c "$cmdw" > "$o" 2>&1; RC=$?
    pruef "Doppelklick 1: rc 0 (ist $RC)" gleich "$RC" 0
    pruef "Doppelklick 1: RELEASE-ALLES-OK v0.8.26" hat "$o" "RELEASE-ALLES-OK v0.8.26"
    pruef "Doppelklick 1: alle Schritte ('$(aufrufe)')" gleich "$(aufrufe)" \
        "windows  jobs=;android --version v0.8.26 jobs=;linux  jobs=;paket --version v0.8.26 jobs=;"
    pruef "Doppelklick 1: alte (v0.8.25) und neue (v0.8.26) Version im Archiv" test -d "$A/v0.8.25" -a -d "$A/v0.8.26"
    pruef "Doppelklick 1: ohne --tag kein Tag" bash -c "! git -C '$R' rev-parse -q --verify refs/tags/v0.8.26 >/dev/null"
    #   2 danach: v0.8.26 liegt schon im Archiv -> ein zweiter Bau wird VOR dem Bauen abgelehnt (sonst Archiv-Konflikt nach Stunden)
    : > "$P"
    o="$fx/cmd5.txt"
    printf '2\n' | RE15_ARCHIV="$AW" MSYS_NO_PATHCONV=1 cmd.exe /c "$cmdw" > "$o" 2>&1; RC=$?
    pruef "Doppelklick 2 nach dem Bau: rc 1, nichts gebaut (ist $RC, '$(aufrufe)')" gleich "$RC:$(aufrufe)" "1:"
    pruef "Doppelklick 2 nach dem Bau: Hinweis 'Nur veroeffentlichen: ... --ab abschluss --tag'" hat "$o" "Nur veroeffentlichen: bash release/release_alles.sh --ab abschluss --tag"
    #   3: nur Commit + Tag + Push (die Notes stehen jetzt)
    printf '# v0.8.26 - x\n\n# v0.8.25 - Testfassung\n' > "$R/release/RELEASE_NOTES.md"
    o="$fx/cmd6.txt"
    printf '3\n' | RE15_ARCHIV="$AW" MSYS_NO_PATHCONV=1 cmd.exe /c "$cmdw" > "$o" 2>&1; RC=$?
    pruef "Doppelklick 3: rc 0 (ist $RC), nichts gebaut" gleich "$RC:$(aufrufe)" "0:"
    pruef "Doppelklick 3: Release-Commit + Tag v0.8.26 + Push" bash -c "[[ \$(git -C '$R' log -1 --format=%s) == 'release: v0.8.26 (Windows + Linux/Steam Deck + Android)' && \$(git -C '$R' cat-file -t v0.8.26) == tag ]] && git -C '$fx/origin.git' rev-parse -q --verify refs/tags/v0.8.26 >/dev/null"
    [[ $FEHLER -eq 0 ]] || { zeige cmd4 "$fx/cmd4.txt" 30; zeige cmd5 "$fx/cmd5.txt" 30; zeige cmd6 "$o" 40; }
}

case "$TEIL" in
    version)   teil_version ;;
    bausteine) teil_bausteine ;;
    trocken)   teil_trocken ;;
    ablauf)    teil_ablauf ;;
    fehler_bau)    teil_fehler_bau ;;
    fehler_umfeld) teil_fehler_umfeld ;;
    cmd)       teil_cmd ;;
    *) echo "unbekannter Teil: $TEIL" >&2; exit 2 ;;
esac
echo "== r38_releaseskript $TEIL: $((N - FEHLER))/$N Pruefungen gruen =="
(( N > 0 && FEHLER == 0 )) || exit 1
exit 0
