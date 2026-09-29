#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Host-Teil der Container-Baus: Image bauen, Quellbaum als KOPIE in den
# Container bringen, Bau-Skript darin starten. Per "source" genutzt von
# release/build_linux_deck.sh und release/build_win_cross.sh.
# =============================================================================
# WARUM (2026-09-28, analysis/befunde_runde30/nachtrag-linux-bau.md): unter Docker
# Desktop (WSL2) ist ein Windows-Bindmount ein 9p-Mount (drvfs, msize=65536) mit
# ~10 ms je Dateioperation — gemessen 5000x stat 54,4 s statt 3,3 s, 2000 kleine
# Dateien schreiben 93,6 s statt 1,1 s, tar von re15_port 448 s statt 1,8 s nativ.
# Der Linux-Bau lief damit ueber eine Stunde, der kalte Windows-Cross-Bau hing
# 45 min in den SDL2-Pruefungen.
#
# SO: Dateiliste per git (versioniert + unversioniert-nicht-ignoriert) fuer die
# KOPIE-Pfade, auf dem Host (nativ, schnell) in ein tar packen, per stdin in den
# Container streamen und dort unter /src auspacken — /src ist damit Container-
# Dateisystem, und der Pfad bleibt derselbe wie beim frueheren Bindmount (die im
# Binary eingebauten Pfade aendern sich nicht). Das Repo haengt zusaetzlich unter
# /host; release/docker/rueckfall_links.sh legt im Container fuer ALLES Nicht-
# Kopierte einen Link /src/<pfad> -> /host/<pfad> an. Es fehlt also keine Datei,
# die es im Repo gibt; ein KOPIE-Eintrag entscheidet nur ueber die Geschwindigkeit.
# Das Ausgabe-Verzeichnis wird direkt eingehaengt.
#
# ⛔ Das tar MUSS aus einer Datei kommen, nicht aus einer Pipe: aus einer
# Git-Bash-Pipe schafft docker.exe 8 MB/s (200 MB in 24,7 s), aus einer Datei
# 200 MB in 6,4 s (gemessen 2026-09-28).
#
# Erwartet (vom Aufrufer gesetzt):
#   REPO RUNNER IMAGE MODE(kopie|mount) SKRIPT AUSGABE
#   KOPIE=( ... )  NIE=( ... )  ENV_WEITER=( NAME ... )   (Pfade relativ zu REPO)
# Bau-Umgebung, kein Spiel-Verhalten: keine Original-Adresse, keine Port-Konstante.
# =============================================================================

# ⛔ GIT-BASH SCHREIBT UNIX-PFADE IN ARGUMENTEN UM. Aus `-w /src` wird dort
# `C:/Program Files/Git/src`, und Docker bricht mit "working directory ... is invalid"
# ab (gemessen 2026-09-01). MSYS_NO_PATHCONV=1 schaltet das ab; zusaetzlich braucht jede
# Host-Pfad-Angabe (Volume-Quelle, Dockerfile, Kontext) unter Windows einen Windows-Pfad.
# git.exe bekommt deshalb keinen Pfad als Argument, sondern laeuft per cd im Repo.
# Auf Linux/Deck aendert beides nichts.
hp() { printf '%s' "$1"; }
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*)
        command -v cygpath >/dev/null 2>&1 && hp() { cygpath -w "$1"; }
        export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*'
        ;;
esac

T0=$(date +%s)

# bild_bauen <Dockerfile> <Tag>: Docker-Schichtcache — ohne Aenderung an Dockerfile
# und *_deps.sh dauert das Sekunden (gemessen 2 s).
bild_bauen() {
    echo "== Bau-Image $2 (${1#"$REPO"/}) =="
    "$RUNNER" build -q -t "$2" -f "$(hp "$1")" "$(hp "$(dirname -- "$1")")" >/dev/null
    echo "== Phase Image: $(( $(date +%s) - T0 )) s"
}

container_lauf() {
    local envargs=() n
    for n in "${ENV_WEITER[@]}"; do
        [[ -n "${!n:-}" ]] && envargs+=( -e "$n=${!n}" )
    done
    mkdir -p "$REPO/$AUSGABE"

    if [[ "$MODE" == mount ]]; then
        echo "== Bauen in $RUNNER ($IMAGE), Repo direkt als /src eingehaengt (--mount) =="
        local rc=0
        "$RUNNER" run --rm -v "$(hp "$REPO"):/src" -w /src "${envargs[@]}" "$IMAGE" \
            bash "/src/$SKRIPT" || rc=$?
        echo "== Wrapper gesamt: $(( $(date +%s) - T0 )) s (rc=$rc)"
        return "$rc"
    fi

    command -v git >/dev/null 2>&1 && ( cd "$REPO" && git rev-parse --git-dir >/dev/null 2>&1 ) || {
        echo "Kopie-Modus braucht ein git-Repo (Dateiliste). Ohne: --mount" >&2; return 1; }

    local t_pack; t_pack=$(date +%s)
    STAGE="$(mktemp -d "${TMPDIR:-/tmp}/re15_stage.XXXXXX")"
    trap 'rm -rf "$STAGE"' EXIT
    mkdir -p "$STAGE/.re15_stage"

    # 1) Dateiliste: versioniert + unversioniert-nicht-ignoriert. Versionierte, aber im
    #    Arbeitsbaum geloeschte Dateien fallen heraus (die gibt es auf dem Host auch nicht).
    : > "$STAGE/files.lst"
    local f
    while IFS= read -r -d '' f; do
        [[ -e "$REPO/$f" ]] && printf '%s\0' "$f" >> "$STAGE/files.lst"
    done < <(cd "$REPO" && git ls-files -z --cached --others --exclude-standard -- "${KOPIE[@]}")

    # 2) Rueckfall-Links (NUL-getrennt): a) oberste Ebene, b) Geschwister teilweise
    #    kopierter Verzeichnisse, c) git-ignorierte Eintraege in kopierten Verzeichnissen.
    local LINKS="$STAGE/.re15_stage/host_links.lst" k d e rel base pre
    : > "$LINKS"
    declare -A eltern=([.]=1)
    for k in "${KOPIE[@]}"; do
        d="$k"
        while [[ "$d" == */* ]]; do d="${d%/*}"; eltern[$d]=1; done
    done
    shopt -s dotglob nullglob
    for d in "${!eltern[@]}"; do
        if [[ "$d" == . ]]; then base="$REPO"; pre=""; else base="$REPO/$d"; pre="$d/"; fi
        for e in "$base"/*; do
            rel="$pre${e##*/}"
            _auf_kopiepfad "$rel" || _link "$rel" "$LINKS"
        done
    done
    shopt -u dotglob nullglob
    while IFS= read -r -d '' f; do
        _link "${f%/}" "$LINKS"
    done < <(cd "$REPO" && git ls-files -z --others --ignored --exclude-standard --directory -- "${KOPIE[@]}")

    local n_dateien n_links
    n_dateien=$(tr -cd '\0' < "$STAGE/files.lst" | wc -c)
    n_links=$(tr -cd '\0' < "$LINKS" | wc -c)
    tar -cf "$STAGE/src.tar" -C "$STAGE" .re15_stage -C "$REPO" --null -T "$STAGE/files.lst"
    echo "== Phase Quellbaum packen: $(( $(date +%s) - t_pack )) s, gesamt $(( $(date +%s) - T0 )) s  ($n_dateien Dateien, $(( $(wc -c < "$STAGE/src.tar") / 1048576 )) MB, $n_links Rueckfall-Links)"

    echo "== Bauen in $RUNNER ($IMAGE), Quellbaum als Kopie im Container =="
    local rc=0
    "$RUNNER" run --rm -i \
        -v "$(hp "$REPO"):/host" \
        -v "$(hp "$REPO/$AUSGABE"):/src/$AUSGABE" \
        -w /src "${envargs[@]}" "$IMAGE" \
        bash -c 't=$(date +%s); tar -C /src --no-same-owner -xf - && echo "== Phase Quellbaum auspacken: $(( $(date +%s) - t )) s" && exec bash "/src/$0"' "$SKRIPT" \
        < "$STAGE/src.tar" || rc=$?
    echo "== Wrapper gesamt: $(( $(date +%s) - T0 )) s (rc=$rc)"
    return "$rc"
}

_auf_kopiepfad() {   # 0, wenn $1 ein KOPIE-Pfad oder ein Vorfahr eines KOPIE-Pfads ist
    local k
    for k in "${KOPIE[@]}"; do
        [[ "$k" == "$1" || "$k" == "$1/"* ]] && return 0
    done
    return 1
}
_link() {            # $1 in die Linkliste $2, ausser er liegt unter einem NIE-Pfad
    local n
    for n in "${NIE[@]}"; do
        [[ "$1" == "$n" || "$1" == "$n/"* ]] && return 0
    done
    printf '%s\0' "$1" >> "$2"
}
