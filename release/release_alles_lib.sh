#!/usr/bin/env bash
# =============================================================================
# release/release_alles_lib.sh - Bausteine von release/release_alles.sh (Runde 38, Spur V)
# =============================================================================
# Per "source" geladen von release/release_alles.sh und von den Tests
# re15_port/tests/unit/r38_releaseskript/ (Versionslogik, Volumen-Namen, Frische, Archiv).
# Nur Funktionen, keine Seiteneffekte beim Laden. Laeuft unter "set -euo pipefail".
#
# Bau-Umgebung, KEIN Spielverhalten: es gibt keine Original-Adresse. Jede Regel nennt ihre
# Quelle (Datei:Zeile oder Messung) - Dossier analysis/befunde_runde38/V_releaseskript.md.
# =============================================================================

# --- Versionen -----------------------------------------------------------------
# Form vX.Y.Z. Y und Z hoechstens 99, X hoechstens 999:
#   re15_port/platform/android/app/build.gradle:43-49 rechnet den Android-versionCode als
#   X*1000000 + Y*10000 + Z*100. Z = 100 ergaebe denselben Code wie Y+1 (0.8.100 -> 90000 = 0.9.0),
#   Y = 100 denselben wie X+1 - das Update auf dem Geraet wuerde als gleich/aelter abgelehnt
#   (release/build_android.sh:195-199). X <= 999 haelt den Code unter 2^31 (999*1000000 + 99*10000 + 99*100).
# Ohne "v" ("0.8.26") wird "v" ergaenzt; fuehrende Nullen ("v0.08.1") sind ungueltig, weil git-Tag,
# Paketname und versionName sonst zwei Schreibweisen derselben Version haetten.
RA_VERSION_MUSTER='^v?(0|[1-9][0-9]{0,2})\.(0|[1-9][0-9]?)\.(0|[1-9][0-9]?)$'

# ra_version_normal <text> -> druckt vX.Y.Z; Rueckgabe 1, wenn ungueltig
ra_version_normal() {
    local t="${1:-}"
    [[ "$t" =~ $RA_VERSION_MUSTER ]] || return 1
    printf 'v%s.%s.%s\n' "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}" "${BASH_REMATCH[3]}"
}

# ra_version_teile <vX.Y.Z> -> druckt "X Y Z" (dezimal); nimmt jede Ziffernfolge (auch Tags, die die
# strenge Form verletzen - die zaehlen fuer "hoechster Tag" mit, damit nie eine KLEINERE Version entsteht)
ra_version_teile() {
    [[ "${1:-}" =~ ^v([0-9]+)\.([0-9]+)\.([0-9]+)$ ]] || return 1
    printf '%d %d %d\n' "$((10#${BASH_REMATCH[1]}))" "$((10#${BASH_REMATCH[2]}))" "$((10#${BASH_REMATCH[3]}))"
}

# ra_version_vergleich <a> <b> -> druckt -1 / 0 / 1 (numerisch je Stelle: v0.8.10 > v0.8.9)
ra_version_vergleich() {
    local a b
    a=($(ra_version_teile "$1")) || return 1
    b=($(ra_version_teile "$2")) || return 1
    local i
    for i in 0 1 2; do
        if (( a[i] < b[i] )); then echo -1; return 0; fi
        if (( a[i] > b[i] )); then echo 1; return 0; fi
    done
    echo 0
}

# ra_hoechste_version: liest Tag-Namen (je Zeile) von stdin, druckt den hoechsten vX.Y.Z.
# Andere Tags (v0.1, v0.2, backup-before-pull-ed20f6d3, itembox-hybrid-v1 - alle im Repo vorhanden)
# zaehlen nicht. Rueckgabe 1, wenn keiner passt.
ra_hoechste_version() {
    local t best=""
    while IFS= read -r t; do
        t="${t%$'\r'}"
        ra_version_teile "$t" >/dev/null 2>&1 || continue
        if [[ -z "$best" || "$(ra_version_vergleich "$t" "$best")" == 1 ]]; then best="$t"; fi
    done
    [[ -n "$best" ]] || return 1
    echo "$best"
}

# ra_naechste_version <vX.Y.Z> -> druckt die Folgeversion: Z+1; bei Z = 99 Y+1 und Z = 0
# (Tags v0.3.99 -> v0.4.0 und v0.7.99 -> v0.8.0; Grund build.gradle:43-49, siehe oben).
# Y = 99 UND Z = 99: Rueckgabe 1 - der Sprung in die erste Stelle ist keine "minor"-Erhoehung
# (Wortlaut 2026-10-16) und kommt in keinem Tag vor; dann --version angeben.
ra_naechste_version() {
    local t x y z
    t=($(ra_version_teile "${1:-}")) || return 1
    x=${t[0]}; y=${t[1]}; z=${t[2]}
    (( y <= 99 && z <= 99 )) || return 1
    if (( z < 99 )); then
        z=$((z + 1))
    elif (( y < 99 )); then
        y=$((y + 1)); z=0
    else
        return 1
    fi
    ra_version_normal "v$x.$y.$z"
}

# --- Paket-Volumes -------------------------------------------------------------
# Namen wie make_package.sh sie schreibt (NAME="re15_port_${VERSION}" :97, Plattformen linux_steamdeck_x64,
# win64, android; zip -s 90m (:74) -> .z01 .. .zNN + .zip, :856/:875/:912).
RA_PLATTFORMEN=(win64 linux_steamdeck_x64 android)
RA_VOLUMEN_MUSTER='^re15_port_(v[0-9]+\.[0-9]+\.[0-9]+)_(win64|linux_steamdeck_x64|android)\.(z[0-9][0-9]|zip)$'

# ra_volumen_version <dateiname> -> druckt vX.Y.Z; Rueckgabe 1, wenn kein Paket-Volume
ra_volumen_version() {
    [[ "${1##*/}" =~ $RA_VOLUMEN_MUSTER ]] || return 1
    echo "${BASH_REMATCH[1]}"
}
# ra_volumen_plattform <dateiname> -> druckt die Plattform
ra_volumen_plattform() {
    [[ "${1##*/}" =~ $RA_VOLUMEN_MUSTER ]] || return 1
    echo "${BASH_REMATCH[2]}"
}

# ra_volumen_in <ordner> -> Dateinamen der Paket-Volumes im Ordner (je Zeile, sortiert)
ra_volumen_in() {
    local f
    for f in "$1"/re15_port_v*; do
        [[ -f "$f" ]] || continue
        ra_volumen_version "$f" >/dev/null && echo "${f##*/}"
    done | LC_ALL=C sort
}

# ra_versionen_in <ordner> -> die Versionen dieser Volumes (je Zeile, eindeutig)
ra_versionen_in() {
    local f
    ra_volumen_in "$1" | while IFS= read -r f; do ra_volumen_version "$f"; done | LC_ALL=C sort -u
}

# --- SHA256SUMS ----------------------------------------------------------------
# ⛔ Gemessen 2026-10-16: in einem frisch ausgecheckten Baum (core.autocrlf=true, keine eol-Regel) sind
# release/SHA256SUMS*.txt CRLF (git ls-files --eol: i/lf w/crlf), und sha256sum -c meldet dann
# "'a.bin'$'\r': No such file or directory". Deshalb wird JEDE SUMS-Datei ohne \r gelesen.

# ra_sums_namen <sums-datei> -> die Dateinamen darin (ohne "*"-Kennung, je Zeile)
ra_sums_namen() {
    tr -d '\r' < "$1" | sed -n 's/^[0-9a-fA-F]\{64\} [ *]\(.*\)$/\1/p'
}

# ra_sums_pruefen <ordner> <sums-name> -> sha256sum -c im Ordner, ohne \r; druckt "n/n OK";
# Rueckgabe 1 bei jedem Fehler oder wenn die Datei keine Zeile hat
ra_sums_pruefen() {
    local dir="$1" sums="$2" n ok out rc=0
    [[ -f "$dir/$sums" ]] || { echo "fehlt: $dir/$sums" >&2; return 1; }
    n="$(tr -d '\r' < "$dir/$sums" | grep -c '[0-9a-fA-F]\{64\}' || true)"
    (( n > 0 )) || { echo "leer: $dir/$sums" >&2; return 1; }
    out="$(cd "$dir" && tr -d '\r' < "$sums" | sha256sum -c - 2>&1)" || rc=$?
    ok="$(printf '%s\n' "$out" | grep -c ': OK$' || true)"
    if (( rc != 0 || ok != n )); then
        printf '%s\n' "$out" | grep -v ': OK$' >&2 || true
        echo "sha256sum -c $sums in $dir: $ok/$n OK (rc=$rc)" >&2
        return 1
    fi
    echo "$ok/$n OK"
}

# ra_sha256 <datei> -> druckt die Pruefsumme
ra_sha256() {
    local s
    s="$(sha256sum "$1")" || return 1
    echo "${s%% *}"
}

# --- Frische ---------------------------------------------------------------------
# ra_mtime <datei> -> Sekunden seit 1970 (GNU stat, sonst BSD stat)
ra_mtime() {
    stat -c %Y "$1" 2>/dev/null || stat -f %m "$1" 2>/dev/null
}
# ra_groesse <datei> -> Bytes
ra_groesse() {
    stat -c %s "$1" 2>/dev/null || stat -f %z "$1" 2>/dev/null
}
# ra_zeit <sekunden> -> lesbare Ortszeit
ra_zeit() {
    date -d "@$1" '+%F %T' 2>/dev/null || date -r "$1" '+%F %T' 2>/dev/null || echo "@$1"
}

# ra_artefakt_frisch <datei> <t0> -> 0, wenn die Datei da ist, nicht leer, und NICHT aelter als t0
# (Laufbeginn des Schritts). Memory reai-v2-releasebau-pipe-schluckt-fehler: nach einem gescheiterten
# Bau lag das Binary des VORIGEN Release noch in linux_out - ein Artefakt ohne frisches Datum ist keins.
# Meldung auf stderr.
ra_artefakt_frisch() {
    local f="$1" t0="$2" m g
    [[ -f "$f" ]] || { echo "Artefakt fehlt: $f" >&2; return 1; }
    g="$(ra_groesse "$f")"
    [[ -n "$g" && "$g" -gt 0 ]] || { echo "Artefakt ist leer: $f" >&2; return 1; }
    m="$(ra_mtime "$f")"
    [[ -n "$m" ]] || { echo "Zeitstempel nicht lesbar: $f" >&2; return 1; }
    if (( m < t0 )); then
        echo "Artefakt ist AELTER als der Schrittbeginn: $f ($(ra_zeit "$m") < $(ra_zeit "$t0")) - altes Artefakt, kein neues" >&2
        return 1
    fi
    return 0
}

# --- Archiv ------------------------------------------------------------------------
# ra_archiv_pruefen <ordner> -> sha256sum -c SHA256SUMS.txt (+ SHA256SUMS_android.txt, falls da);
# druckt "SHA256SUMS.txt n/n OK; SHA256SUMS_android.txt 1/1 OK"; Rueckgabe 1 bei jedem Fehler
ra_archiv_pruefen() {
    local dir="$1" a b=""
    a="$(ra_sums_pruefen "$dir" SHA256SUMS.txt)" || return 1
    if [[ -f "$dir/SHA256SUMS_android.txt" ]]; then
        b="$(ra_sums_pruefen "$dir" SHA256SUMS_android.txt)" || return 1
        echo "SHA256SUMS.txt $a; SHA256SUMS_android.txt $b"
    else
        echo "SHA256SUMS.txt $a"
    fi
}

# ra_sums_gleich <datei-a> <datei-b> -> 0, wenn beide (ohne \r, ohne "*"-Kennung, sortiert) dieselben Zeilen tragen
ra_sums_gleich() {
    [[ -f "$1" && -f "$2" ]] || return 1
    diff <(tr -d '\r' < "$1" | sed 's/^\([0-9a-fA-F]\{64\}\) [ *]/\1  /' | LC_ALL=C sort) \
         <(tr -d '\r' < "$2" | sed 's/^\([0-9a-fA-F]\{64\}\) [ *]/\1  /' | LC_ALL=C sort) >/dev/null
}

# ra_archiv_anlegen <quelle> <archiv-wurzel> <version> <sums-pc> [<sums-android> <apk>]
#   Kopiert die in <sums-pc> genannten Volumes (+ APK aus <sums-android>, falls angegeben) aus <quelle>
#   erst nach <wurzel>/.<version>.unvollstaendig, schreibt die SUMS ohne \r dazu, prueft mit sha256sum -c
#   und benennt erst DANN in <wurzel>/<version> um: ein abgebrochener Lauf hinterlaesst nie einen
#   Ordner, der wie eine fertige Version aussieht. Ein vorhandener fertiger Ordner wird NICHT angefasst
#   (Aufrufer entscheidet vorher). Druckt das Pruefergebnis.
ra_archiv_anlegen() {
    local quelle="$1" wurzel="$2" version="$3" sums_pc="$4" sums_android="${5:-}" apk="${6:-}"
    local ziel="$wurzel/$version" tmp="$wurzel/.$version.unvollstaendig" f n
    [[ ! -e "$ziel" ]] || { echo "Archivordner existiert schon: $ziel" >&2; return 1; }
    mkdir -p "$wurzel" || return 1
    rm -rf "$tmp"
    mkdir -p "$tmp" || return 1
    while IFS= read -r n; do
        [[ -n "$n" ]] || continue
        [[ -f "$quelle/$n" ]] || { echo "Volume fehlt in $quelle: $n" >&2; return 1; }
        cp -f "$quelle/$n" "$tmp/$n" || return 1
    done < <(ra_sums_namen "$sums_pc")
    tr -d '\r' < "$sums_pc" > "$tmp/SHA256SUMS.txt" || return 1
    if [[ -n "$sums_android" ]]; then
        [[ -f "$quelle/$apk" ]] || { echo "APK fehlt in $quelle: $apk" >&2; return 1; }
        cp -f "$quelle/$apk" "$tmp/$apk" || return 1
        tr -d '\r' < "$sums_android" > "$tmp/SHA256SUMS_android.txt" || return 1
    fi
    local erg
    erg="$(ra_archiv_pruefen "$tmp")" || { echo "Pruefung der Archivkopie fehlgeschlagen - $tmp bleibt zur Ansicht liegen" >&2; return 1; }
    mv "$tmp" "$ziel" || return 1
    echo "$erg"
}
