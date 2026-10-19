#!/usr/bin/env bash
# =============================================================================
# Riegel Runde 39 Spur T "titelversion": die Release-Version erreicht den Bau (Titelbild "Ver.X.Y.Z").
# Dossier analysis/befunde_runde39/T_titelversion.md. Nutzer (Nachtrag 14): "bei dieser und zukuenftig neuen
# Versionen passe mir bitte die Version im Titelscreen an."
#
# Aufruf: bash test_r39_titelversion_release.sh <repo> <arbeitsordner> <teil>
#   huelle  ECHTE release/build_win_cross.sh + build_linux_deck.sh (Kopie in einem Wegwerf-Baum) mit einer
#           docker-Attrappe im PATH (--mount, --image): --version V, Umgebung RE15_VERSION, ohne beides der letzte
#           Tag + " dev" aus dem Host-git, ohne git gar nichts -> "docker run ... -e RE15_VERSION=..." (der Weg in den
#           Container; dort liest re15_port/cmake/re15_version_r39.cmake die Umgebung)
#   ablauf  ECHTES release/release_alles.sh im Wegwerf-Repo (Schritt-Skripte = Attrappen aus r38_releaseskript, hier
#           umhuellt, damit sie RE15_VERSION mitschreiben): Trockenlauf zeigt RE15_VERSION=<neu> an windows/linux;
#           Lauf bis linux gibt windows und linux RE15_VERSION=<neu>, android --version <neu>; die Aufruflisten der
#           r38-Attrappen bleiben dieselben (deren Riegel pruefen sie woertlich)
# Keine Umgebung des Aufrufers erben (RE15_* werden geloescht). Rueckgabe 0 = alles gruen.
# =============================================================================
set -uo pipefail
for _v in $(env | sed -n 's/^\(RE15_[A-Za-z0-9_]*\)=.*/\1/p'); do unset "$_v"; done
unset FAKE_PROTOKOLL GIT_CEILING_DIRECTORIES
REPO_ECHT="$(cd "$1" && pwd)"
ARBEIT="$2"
TEIL="$3"
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
R38="$HIER/../r38_releaseskript"
mkdir -p "$ARBEIT"
ARBEIT="$(cd "$ARBEIT" && pwd)"

FEHLER=0
N=0
ok()   { N=$((N + 1)); echo "  ok   $*"; }
fehl() { N=$((N + 1)); FEHLER=$((FEHLER + 1)); echo "  FEHL $*"; }
pruef() { local t="$1"; shift; if "$@"; then ok "$t"; else fehl "$t"; fi; }
hat()   { grep -qF -- "$2" "$1"; }
gleich() { [[ "$1" == "$2" ]]; }
git_q() { git -c user.name=r39t -c user.email=r39t@localhost -c commit.gpgsign=false -c tag.gpgsign=false \
              -c core.autocrlf=false "$@"; }

# --- huelle --------------------------------------------------------------------------------------------------------
# docker-Attrappe: info/build ok; run schreibt die Argumente (eines je Zeile) nach $DOCKER_PROTOKOLL
attrappe_docker() {
    mkdir -p "$1"
    printf '%s\n' '#!/usr/bin/env bash' \
        'case "${1:-}" in' \
        '  run) shift; printf "%s\n" "$@" > "$DOCKER_PROTOKOLL"; cat > /dev/null; exit 0 ;;' \
        'esac' \
        'exit 0' > "$1/docker"
    chmod +x "$1/docker"
}
# e_wert <protokoll>: der Wert hinter "-e RE15_VERSION=" (leer, wenn keiner)
e_wert() {
    awk 'p == 1 && /^RE15_VERSION=/ { sub(/^RE15_VERSION=/, ""); print; exit } { p = ($0 == "-e") }' "$1"
}

teil_huelle() {
    echo "== Huelle: build_win_cross.sh / build_linux_deck.sh -> docker run -e RE15_VERSION =="
    local T="$ARBEIT/huelle" s out prot
    rm -rf "$T"
    mkdir -p "$T/release/docker"
    cp "$REPO_ECHT/release/build_win_cross.sh" "$REPO_ECHT/release/build_linux_deck.sh" "$T/release/"
    cp "$REPO_ECHT/release/docker/kopie_lauf.sh" "$T/release/docker/"
    attrappe_docker "$T/bin"
    prot="$T/docker_run.txt"
    for s in build_win_cross.sh build_linux_deck.sh; do
        out="$T/${s%.sh}"
        # 1) --version
        rm -f "$prot"
        PATH="$T/bin:$PATH" DOCKER_PROTOKOLL="$prot" bash "$T/release/$s" --image r39t-attrappe --mount --version v9.9.9 \
            > "$out.1.txt" 2>&1
        RC=$?
        pruef "$s --version v9.9.9: rc 0 (ist $RC), docker run -e RE15_VERSION=v9.9.9 (ist '$(e_wert "$prot" 2>/dev/null)')" \
            gleich "$RC:$(e_wert "$prot" 2>/dev/null)" "0:v9.9.9"
        pruef "$s --version: meldet die Version" hat "$out.1.txt" "== RE15_VERSION (Titelbild): 'v9.9.9' =="
        # 2) Umgebung
        rm -f "$prot"
        PATH="$T/bin:$PATH" DOCKER_PROTOKOLL="$prot" RE15_VERSION=v8.8.8 bash "$T/release/$s" --image r39t-attrappe --mount \
            > "$out.2.txt" 2>&1
        pruef "$s mit Umgebung RE15_VERSION=v8.8.8 -> -e RE15_VERSION=v8.8.8 (ist '$(e_wert "$prot" 2>/dev/null)')" \
            gleich "$(e_wert "$prot" 2>/dev/null)" "v8.8.8"
        # 3) --version schlaegt die Umgebung
        rm -f "$prot"
        PATH="$T/bin:$PATH" DOCKER_PROTOKOLL="$prot" RE15_VERSION=v8.8.8 bash "$T/release/$s" --image r39t-attrappe --mount \
            --version v7.7.7 > "$out.3.txt" 2>&1
        pruef "$s --version v7.7.7 gegen Umgebung v8.8.8 -> v7.7.7 (ist '$(e_wert "$prot" 2>/dev/null)')" \
            gleich "$(e_wert "$prot" 2>/dev/null)" "v7.7.7"
    done
    # 4) ohne Angabe: letzter Tag vX.Y.Z des Host-git + " dev" (Fremd-Tag danach zaehlt nicht)
    git_q init -q "$T"
    git_q -C "$T" add -A >/dev/null
    git_q -C "$T" commit -q -m "r39t fixture"
    git_q -C "$T" tag v1.2.3
    git_q -C "$T" commit -q --allow-empty -m "r39t danach"
    git_q -C "$T" tag fremd-tag
    for s in build_win_cross.sh build_linux_deck.sh; do
        rm -f "$prot"
        PATH="$T/bin:$PATH" DOCKER_PROTOKOLL="$prot" bash "$T/release/$s" --image r39t-attrappe --mount > "$T/${s%.sh}.4.txt" 2>&1
        pruef "$s ohne Angabe im git-Baum (Tag v1.2.3, danach fremd-tag) -> 'v1.2.3 dev' (ist '$(e_wert "$prot" 2>/dev/null)')" \
            gleich "$(e_wert "$prot" 2>/dev/null)" "v1.2.3 dev"
    done
    # 5) ohne Angabe und ohne git: keine Weitergabe (cmake faellt dann auf "dev")
    rm -rf "$T/.git"
    for s in build_win_cross.sh build_linux_deck.sh; do
        rm -f "$prot"
        PATH="$T/bin:$PATH" DOCKER_PROTOKOLL="$prot" GIT_CEILING_DIRECTORIES="$ARBEIT" bash "$T/release/$s" \
            --image r39t-attrappe --mount > "$T/${s%.sh}.5.txt" 2>&1
        pruef "$s ohne Angabe und ohne git: rc 0, kein -e RE15_VERSION" \
            bash -c "[[ -s '$prot' ]] && ! grep -q '^RE15_VERSION=' '$prot'"
    done
    [[ $FEHLER -eq 0 ]] || { for f in "$T"/*.txt; do echo "  ---- $f"; tail -5 "$f" | sed 's/^/  | /'; done; }
}

# --- ablauf --------------------------------------------------------------------------------------------------------
# Wegwerf-Repo wie test_r38_releaseskript.sh fx_bauen (Vorversion v0.8.25 in release/, Tags v0.8.24 v0.8.25).
fx_bauen() {
    local fx="$1" t
    rm -rf "$fx"
    mkdir -p "$fx/archiv"
    R="$fx/repo"; A="$fx/archiv"
    git_q init -q --bare "$fx/origin.git"
    git_q init -q "$R"
    git_q -C "$R" symbolic-ref HEAD refs/heads/master
    git -C "$R" config user.name r39t
    git -C "$R" config user.email r39t@localhost
    git -C "$R" config core.autocrlf false
    git -C "$R" config commit.gpgsign false
    git -C "$R" config tag.gpgsign false
    mkdir -p "$R/re15_port/engine/src" "$R/synchro/STAGE1/room1170" "$R/release"
    echo 'int r39t_attrappe;' > "$R/re15_port/engine/src/a.c"
    printf 'RIFF' > "$R/synchro/STAGE1/room1170/main00.wav"
    cp "$REPO_ECHT/release/release_alles.sh" "$REPO_ECHT/release/release_alles_lib.sh" "$R/release/"
    cp "$R38/attrappen/"*.sh "$R/release/"
    # die drei Bau-Attrappen umhuellen: erst RE15_VERSION + Argumente mitschreiben, dann die r38-Attrappe
    for t in build_win_cross build_linux_deck build_android; do
        mv "$R/release/$t.sh" "$R/release/r38_$t.sh"
        printf '%s\n' '#!/usr/bin/env bash' \
            "echo \"$t RE15_VERSION=\${RE15_VERSION:-<leer>} \$*\" >> \"\$R39T_PROTOKOLL\"" \
            "exec bash \"\$(cd \"\$(dirname \"\${BASH_SOURCE[0]}\")\" && pwd)/r38_$t.sh\" \"\$@\"" > "$R/release/$t.sh"
    done
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
    git -C "$R" tag v0.8.24
    git -C "$R" tag v0.8.25
    git -C "$R" remote add origin "$fx/origin.git"
    git -C "$R" push -q origin master --tags 2>/dev/null
    export FAKE_PROTOKOLL="$fx/aufrufe.txt" R39T_PROTOKOLL="$fx/versionen.txt"
    : > "$FAKE_PROTOKOLL"; : > "$R39T_PROTOKOLL"
    WERKZEUG_PFAD=""
    local w
    for w in zip unzip; do
        if command -v "$w" >/dev/null 2>&1; then continue; fi
        if [[ "$w" == zip && -x /c/msys64/usr/bin/zip ]]; then continue; fi
        mkdir -p "$fx/werkzeug"
        printf '#!/bin/sh\necho "Platzhalter %s (Test r39_titelversion)"\n' "$w" > "$fx/werkzeug/$w"
        chmod +x "$fx/werkzeug/$w"
        WERKZEUG_PFAD="$fx/werkzeug:"
    done
}
ra() {
    local out="$1"; shift
    PATH="${WERKZEUG_PFAD}$PATH" bash "$R/release/release_alles.sh" --archiv "$A" "$@" > "$out" 2>&1
    RC=$?
}

teil_ablauf() {
    echo "== release_alles.sh reicht die Version an windows / android / linux =="
    local fx="$ARBEIT/fx_ablauf" o
    fx_bauen "$fx"
    o="$fx/trocken.txt"
    ra "$o" --dry-run --min-frei-gb 1
    pruef "Trockenlauf rc 0 (ist $RC)" gleich "$RC" 0
    pruef "Trockenlauf: Version v0.8.26" hat "$o" "v0.8.26"
    pruef "Trockenlauf: windows mit RE15_VERSION=v0.8.26" hat "$o" "RE15_VERSION=v0.8.26 bash release/build_win_cross.sh -> release/win_out/re15_pc.exe"
    pruef "Trockenlauf: linux mit RE15_VERSION=v0.8.26" hat "$o" "RE15_VERSION=v0.8.26 bash release/build_linux_deck.sh -> release/linux_out/re15_pc"
    pruef "Trockenlauf: android mit --version v0.8.26" hat "$o" "bash release/build_android.sh --version v0.8.26"
    pruef "Trockenlauf baut nichts" gleich "$(cat "$R39T_PROTOKOLL")" ""
    o="$fx/lauf.txt"
    ra "$o" --min-frei-gb 1 --linux-jobs 3 --bis linux
    pruef "Lauf bis linux rc 0 (ist $RC)" gleich "$RC" 0
    pruef "windows bekam RE15_VERSION=v0.8.26 ohne neue Argumente" bash -c "grep -qxF 'build_win_cross RE15_VERSION=v0.8.26 ' '$R39T_PROTOKOLL'"
    pruef "android bekam --version v0.8.26 (Titelbild ueber -Pre15Version -> -DRE15_VERSION)" \
        bash -c "grep -qE '^build_android RE15_VERSION=.* --version v0.8.26$' '$R39T_PROTOKOLL'"
    pruef "linux bekam RE15_VERSION=v0.8.26 ohne neue Argumente" bash -c "grep -qxF 'build_linux_deck RE15_VERSION=v0.8.26 ' '$R39T_PROTOKOLL'"
    pruef "Aufrufliste der r38-Attrappen unveraendert (windows/android/linux)" \
        gleich "$(grep -vE '^docker' "$FAKE_PROTOKOLL" | tr '\n' ';')" "windows  jobs=;android --version v0.8.26 jobs=;linux  jobs=3;"
    pruef "Schritt-Protokoll zeigt den Befehl mit RE15_VERSION" bash -c "grep -rqF 'RE15_VERSION=v0.8.26 bash release/build_win_cross.sh' '$o'"
    [[ $FEHLER -eq 0 ]] || { echo "  ---- versionen.txt"; sed 's/^/  | /' "$R39T_PROTOKOLL"; echo "  ---- lauf (Ende)"; tail -30 "$o" | sed 's/^/  | /'; }
}

case "$TEIL" in
    huelle) teil_huelle ;;
    ablauf) teil_ablauf ;;
    *) echo "unbekannter Teil: $TEIL" >&2; exit 2 ;;
esac
echo "== r39_titelversion release $TEIL: $((N - FEHLER))/$N Pruefungen gruen =="
(( N > 0 && FEHLER == 0 )) || exit 1
exit 0
