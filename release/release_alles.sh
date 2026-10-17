#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port - EIN Aufruf fuer das ganze Release (Runde 38, Spur V)
# =============================================================================
# Nutzer 2026-10-16: "Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle
# Systeme zu bauen - der neuen Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte
# ins Archiv zu bringen."
#
# KURZANLEITUNG (ausfuehrlicher: release/README.md)
#   Doppelklick auf release\release_alles.cmd  -> Trockenlauf, dann Wahl [1] bauen [2] + Commit/Tag/Push
#                                                 [3] nur Commit/Tag/Push [4] nichts
#   bash release/release_alles.sh --dry-run    zeigt Version, Pruefungen und jeden Schritt; baut und schreibt nichts
#   bash release/release_alles.sh              naechste Version, bauen, packen, Archiv (ohne Commit/Tag/Push)
#   bash release/release_alles.sh --tag        dazu am Ende Release-Commit + Tag + Push
#                                              (die Release-Notes fuer die neue Version muessen dann oben in
#                                              release/RELEASE_NOTES.md stehen - die schreibt der Mensch)
#   bash release/release_alles.sh --ab linux   nach einem Abbruch ab diesem Schritt weitermachen
#   bash release/release_alles.sh --ab abschluss --tag
#                                              Commit + Tag + Push einer schon gebauten und archivierten Version;
#                                              stehen Commit und Tag schon (Push scheiterte, --kein-push), wird nur
#                                              noch gepusht (Doppelklick: Wahl [3])
#
# OPTIONEN
#   --version vX.Y.Z   statt der automatischen Version (muss groesser sein als jeder Tag vX.Y.Z)
#   --dry-run          Trockenlauf (auch --trockenlauf)
#   --tag              Release-Commit + Tag + Push am Ende (Schritt "abschluss")
#   --kein-push        mit --tag: Commit + Tag, aber nicht pushen
#   --ab SCHRITT       erst ab SCHRITT (alt windows android linux paket archiv abschluss)
#   --bis SCHRITT      nur bis einschliesslich SCHRITT (z.B. --ab windows --bis windows: nur Windows neu bauen)
#   --linux-jobs N     ctest im Linux-Container mit N Jobs (RE15_CTEST_JOBS; Standard seriell)
#   --archiv ORDNER    Archiv-Wurzel (Standard: $RE15_ARCHIV, sonst C:/workspace/Re15Data/re15_packages_archiv)
#   --min-frei-gb N    verlangter freier Platz auf Repo- und Archiv-Laufwerk (Standard 10)
#
# VERSION: hoechster Tag vX.Y.Z -> vX.Y.(Z+1); bei Z = 99 -> vX.(Y+1).0 (wie v0.7.99 -> v0.8.0).
#   Grund fuer die 99: release_alles_lib.sh (Android-versionCode, build.gradle:43-49).
#
# SCHRITTE (jeder mit eigenem Protokoll release/logs/<version>_<zeit>/NN_<schritt>.log, jeder bricht
# bei Fehler den Lauf ab; Rueckgabe UND Erfolgsmarke UND frisches Artefakt werden geprueft):
#   vorbedingungen  Version, Tag frei, re15_port/ + Werkzeug sauber, Docker (startet Docker Desktop),
#                   Platz, Werkzeuge, Archiv, Sperre gegen einen zweiten Lauf
#   alt       die bisherige Version aus release/ ins Archiv bringen (oder dort pruefen), VOR dem Bauen
#   windows   release/build_win_cross.sh            -> release/win_out/re15_pc.exe
#   android   release/build_android.sh --version V  -> release/re15_port_V_android.apk
#   linux     release/build_linux_deck.sh           -> release/linux_out/re15_pc (volle Suite im Container)
#   paket     release/make_package.sh --version V   -> Volumes aller DREI Plattformen + SHA256SUMS.txt;
#             die alten Volumes nimmt make_package.sh aus release/ und aus dem git-Index
#   archiv    neue Version ins Archiv (sha256sum -c), die alte APK aus release/ (liegt geprueft im Archiv);
#             im Archiv wird NICHTS geloescht - die alte Version bleibt dort
#   abschluss (nur --tag) Release-Commit, Tag, Push; was davon schon steht, wird nicht neu angelegt; der Push gilt
#             erst, wenn origin selbst master und Tag traegt (git ls-remote)
# Reihenfolge Windows -> Android -> Linux: die kurzen Baue zuerst, der 3-Stunden-Linux-Lauf zuletzt.
#
# Bau-Umgebung, kein Spielverhalten: keine Original-Adresse. Belege: analysis/befunde_runde38/V_releaseskript.md
# =============================================================================
set -euo pipefail

# Ein vom Aufrufer geerbtes MSYS_NO_PATHCONV=1 schaltet unter Git-Bash die Pfad-Umsetzung fuer git.exe & Co. ab:
# "git -C /tmp/..." findet das Repo dann nicht (gemessen 2026-10-16, unit_r38_releaseskript_cmd: "kein git-Repo:
# /tmp/claude/..."). Die Einzelskripte setzen es selbst dort, wo sie es brauchen (release/docker/kopie_lauf.sh:44).
unset MSYS_NO_PATHCONV MSYS2_ARG_CONV_EXCL

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
# shellcheck source=release_alles_lib.sh
source "$HERE/release_alles_lib.sh"

SCHRITTE=(alt windows android linux paket archiv abschluss)

VERSION_ARG=""
DRY=0
TAG=0
PUSH=1
AB=""
BIS=""
LINUX_JOBS=""
ARCHIV="${RE15_ARCHIV:-}"          # Umgebung RE15_ARCHIV = Standard fuer --archiv (andere Maschine, Tests)
# PORT-WAHL (Bau-Umgebung): gemessen ~6,2 GB je Lauf auf dem Host (Dossier Abschnitt 1, M10) plus die Docker-VM.
MIN_FREI_GB=10
# Optionen dieses Aufrufs, die jede gemeldete Wiederaufnahme-Zeile mitnimmt: ohne --archiv pruefte die Zeile ein
# ANDERES Archiv und bliebe stehen (Abnahme 0, M1: die gemeldete Zeile muss woertlich zum Ziel fuehren).
WEITER_OPT=()

benutzung() { awk 'NR == 1 { next } /^set -euo pipefail/ { exit } { print }' "${BASH_SOURCE[0]}"; }

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)       [[ -n "${2:-}" ]] || { echo "--version braucht vX.Y.Z" >&2; exit 2; }
                         VERSION_ARG="$2"; shift 2 ;;
        --dry-run|--trockenlauf) DRY=1; shift ;;
        --tag)           TAG=1; shift ;;
        --kein-push)     PUSH=0; WEITER_OPT+=("$1"); shift ;;
        --ab)            [[ -n "${2:-}" ]] || { echo "--ab braucht einen Schritt: ${SCHRITTE[*]}" >&2; exit 2; }
                         AB="$2"; shift 2 ;;
        --bis)           [[ -n "${2:-}" ]] || { echo "--bis braucht einen Schritt: ${SCHRITTE[*]}" >&2; exit 2; }
                         BIS="$2"; shift 2 ;;
        --linux-jobs)    [[ "${2:-}" =~ ^[1-9][0-9]*$ ]] || { echo "--linux-jobs braucht eine Zahl >= 1" >&2; exit 2; }
                         LINUX_JOBS="$2"; WEITER_OPT+=("$1" "$2"); shift 2 ;;
        --archiv)        [[ -n "${2:-}" ]] || { echo "--archiv braucht einen Ordner" >&2; exit 2; }
                         ARCHIV="$2"; WEITER_OPT+=("$1" "$2"); shift 2 ;;
        --min-frei-gb)   [[ "${2:-}" =~ ^[0-9]+$ ]] || { echo "--min-frei-gb braucht eine Zahl" >&2; exit 2; }
                         MIN_FREI_GB="$2"; WEITER_OPT+=("$1" "$2"); shift 2 ;;
        -h|--help)       benutzung; exit 0 ;;
        *) echo "unbekannte Option: $1 (Hilfe: --help)" >&2; exit 2 ;;
    esac
done
if [[ $PUSH -eq 0 && $TAG -eq 0 ]]; then echo "--kein-push gilt nur zusammen mit --tag" >&2; exit 2; fi

IST_WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) IST_WIN=1 ;; esac
if [[ -z "$ARCHIV" ]]; then
    if [[ $IST_WIN -eq 1 ]]; then ARCHIV="/c/workspace/Re15Data/re15_packages_archiv"
    else ARCHIV="$HOME/Re15Data/re15_packages_archiv"; fi
fi

# --- Plan: welche Schritte laufen ---------------------------------------------------
PLAN=()
for opt in "--ab:$AB" "--bis:$BIS"; do
    name="${opt%%:*}"; wert="${opt#*:}"
    [[ -n "$wert" ]] || continue
    gefunden=0
    for s in "${SCHRITTE[@]}"; do if [[ "$s" == "$wert" ]]; then gefunden=1; fi; done
    [[ $gefunden -eq 1 ]] || { echo "$name: unbekannter Schritt '$wert' (${SCHRITTE[*]})" >&2; exit 2; }
done
an=0; [[ -z "$AB" ]] && an=1
bis_gesehen=0
for s in "${SCHRITTE[@]}"; do
    [[ "$s" == "$AB" ]] && an=1
    [[ $an -eq 1 ]] || continue
    if [[ "$s" != abschluss || $TAG -eq 1 ]]; then PLAN+=("$s"); fi
    if [[ "$s" == "$BIS" ]]; then bis_gesehen=1; break; fi
done
if [[ -n "$BIS" && $bis_gesehen -eq 0 ]]; then echo "--bis $BIS liegt vor --ab $AB" >&2; exit 2; fi
if [[ "$AB" == abschluss && $TAG -eq 0 ]]; then echo "--ab abschluss braucht --tag" >&2; exit 2; fi
(( ${#PLAN[@]} > 0 )) || { echo "leerer Plan (--ab $AB --bis $BIS)" >&2; exit 2; }
im_plan() { local s; for s in "${PLAN[@]}"; do [[ "$s" == "$1" ]] && return 0; done; return 1; }

# --- Ausgabe + Protokoll -------------------------------------------------------------
ABLAUF_LOG=""
PUFFER=()
meld() {
    printf '%s\n' "$*"
    if [[ -n "$ABLAUF_LOG" ]]; then printf '%s\n' "$*" >> "$ABLAUF_LOG"; else PUFFER+=("$*"); fi
}
BLOCKER=0
WARNUNGEN=0
pz() {   # Pruefzeile: $1 = ok|warn|stop|info, Rest = Text
    local art="$1"; shift
    case "$art" in
        ok)   meld "  [ok] $*" ;;
        info) meld "  [--] $*" ;;
        warn) meld "  [!!] $*"; WARNUNGEN=$((WARNUNGEN + 1)) ;;
        stop) meld "  [XX] $*"; BLOCKER=$((BLOCKER + 1)) ;;
    esac
}
AKTUELL="vorbedingungen"
# opt_text [ohne-kein-push]: die Optionen aus WEITER_OPT als Befehlszeilen-Text (" --archiv X ..."), shell-sicher
opt_text() {
    local o t=""
    for o in "${WEITER_OPT[@]}"; do
        if [[ "${1:-}" == ohne-kein-push && "$o" == --kein-push ]]; then continue; fi
        t+=" $(printf '%q' "$o")"
    done
    printf '%s' "$t"
}
# weiter_zeile <schritt> [--tag]: die Zeile, mit der ein Lauf ab <schritt> weitergeht (Version + Optionen des Aufrufs)
weiter_zeile() {
    local z="bash release/release_alles.sh --version $NEU --ab $1"
    if [[ $TAG -eq 1 || "${2:-}" == --tag ]]; then z+=" --tag"; fi
    printf '%s%s\n' "$z" "$(opt_text)"
}
WEITER_VOR=()    # Zusatzzeilen der Abbruchmeldung des laufenden Schritts: vor bzw. nach der Wiederaufnahme-Zeile
WEITER_NACH=()
die() {
    local z
    meld ""
    meld "!!! ABBRUCH in Schritt '$AKTUELL': $*"
    [[ -n "${SCHRITT_LOG:-}" ]] && meld "    Protokoll: $SCHRITT_LOG"
    if [[ -n "${NEU:-}" && "$AKTUELL" != vorbedingungen ]]; then
        for z in "${WEITER_VOR[@]}"; do meld "    $z"; done
        meld "    Nach der Behebung weiter mit: $(weiter_zeile "$AKTUELL")"
        for z in "${WEITER_NACH[@]}"; do meld "    $z"; done
    fi
    meld "RELEASE-ALLES-ABBRUCH ($AKTUELL)"
    exit 1
}

git_r() { git -C "$REPO" "$@"; }

# ist_release_commit <vX.Y.Z>: HEAD ist der Release-Commit dieser Version - Betreff "release: vX.Y.Z ..." (Form aller
# Release-Commits: 2077bb8e "release: v0.8.25 Zwischenversion (...)", e8b3958f "release: v0.8.24 (...)", s_abschluss)
ist_release_commit() {
    local s; s="$(git_r log -1 --format=%s HEAD 2>/dev/null || true)"
    [[ "$s" == "release: $1" || "$s" == "release: $1 "* ]]
}
# tag_auf_head <tag>: der Tag existiert und zeigt (aufgeloest) auf HEAD
tag_auf_head() {
    local t; t="$(git_r rev-parse -q --verify "refs/tags/$1^{commit}" 2>/dev/null || true)"
    [[ -n "$t" && "$t" == "$(git_r rev-parse HEAD 2>/dev/null || true)" ]]
}

# Unix-Pfad fuer Meldungen als Windows-Pfad (nur Anzeige)
zeige_pfad() { if [[ $IST_WIN -eq 1 ]] && command -v cygpath >/dev/null 2>&1; then cygpath -m "$1"; else echo "$1"; fi; }

frei_gb() {   # freier Platz (GiB, abgerundet) des Laufwerks, auf dem $1 (oder sein naechster Vorfahr) liegt
    local p="$1"
    while [[ ! -e "$p" && "$p" == */* ]]; do p="${p%/*}"; done
    [[ -n "$p" ]] || p=/
    df -Pk "$p" 2>/dev/null | awk 'NR == 2 { printf "%d\n", $4 / 1048576 }'
}

# Andere Bau-/Testprozesse: Release-Gates unter Last flattern (Memory reai-v2-releasebau-pipe-schluckt-fehler,
# reai-v2-gui-tests-flattern-bei-parallelen-agenten). Nur Hinweis.
fremde_last() {
    local liste=""
    if [[ $IST_WIN -eq 1 ]] && command -v tasklist.exe >/dev/null 2>&1; then
        liste="$(MSYS_NO_PATHCONV=1 tasklist.exe /FO CSV /NH 2>/dev/null | cut -d, -f1 | tr -d '"\r' | tr 'A-Z' 'a-z' || true)"
    elif command -v ps >/dev/null 2>&1; then
        liste="$(ps -eo comm= 2>/dev/null || true)"
    fi
    printf '%s\n' "$liste" | grep -E '^(ninja|ctest|cc1|cmake|re15_pc|gradle)(\.exe)?$' | sed 's/\.exe$//' \
        | LC_ALL=C sort | uniq -c | awk '{ printf "%s%s %s", (NR > 1 ? ", " : ""), $2, $1 }' || true
}

# =============================================================================
# 0. Vorbedingungen
# =============================================================================
meld "== RE1.5 Port - Release fuer alle Systeme$( [[ $DRY -eq 1 ]] && echo ' (TROCKENLAUF: es wird nichts gebaut und nichts geschrieben)') =="
git_r rev-parse --git-dir >/dev/null 2>&1 || { echo "kein git-Repo: $REPO" >&2; exit 1; }
ZWEIG="$(git_r rev-parse --abbrev-ref HEAD 2>/dev/null || echo '?')"
HEAD_KURZ="$(git_r rev-parse --short HEAD)"
meld "Repo:    $(zeige_pfad "$REPO")  (Zweig $ZWEIG, HEAD $HEAD_KURZ)"

# --- Version
VORIG_TAG="$(git_r tag --list 'v*' | ra_hoechste_version || true)"
# Wiederaufnahme NACH Commit + Tag (Abnahme 0, M1: der Push scheiterte, oder --kein-push): HEAD ist der Release-Commit
# des hoechsten Tags und release/ traegt genau dessen Volumes -> diese Version; Commit und Tag bleiben, s_abschluss
# pusht nur noch. Nur fuer den Schritt abschluss allein (--ab abschluss --tag = Doppelklick-Wahl [3],
# release_alles.cmd). Sonst gaelte der eigene Tag als "hoechster" und die Version liefe an ihm vorbei: mit --version
# "nicht groesser als der hoechste Tag" (unten), ohne --version die Folgeversion.
NACHHOLEN=0
if [[ "$AB" == abschluss && $TAG -eq 1 && -n "$VORIG_TAG" ]] && tag_auf_head "$VORIG_TAG" \
   && ist_release_commit "$VORIG_TAG" && [[ "$(ra_versionen_in "$HERE")" == "$VORIG_TAG" ]]; then
    if [[ -z "$VERSION_ARG" || "$(ra_version_normal "$VERSION_ARG" 2>/dev/null || true)" == "$VORIG_TAG" ]]; then
        NACHHOLEN=1
    fi
fi
if [[ $NACHHOLEN -eq 1 ]]; then
    NEU="$VORIG_TAG"
    HERKUNFT="Tag $VORIG_TAG steht schon auf HEAD = Release-Commit: Commit und Tag bleiben, nur der Push wird nachgeholt"
elif [[ -n "$VERSION_ARG" ]]; then
    NEU="$(ra_version_normal "$VERSION_ARG")" \
        || { echo "--version '$VERSION_ARG' ist keine gueltige Version vX.Y.Z (Y, Z hoechstens 99, keine fuehrenden Nullen)" >&2; exit 2; }
    if [[ -n "$VORIG_TAG" && "$(ra_version_vergleich "$NEU" "$VORIG_TAG")" != 1 ]]; then
        echo "--version $NEU ist nicht groesser als der hoechste Tag $VORIG_TAG - die APK waere ein Downgrade (build_android.sh:195-199)" >&2
        exit 2
    fi
    HERKUNFT="per --version; hoechster Tag ${VORIG_TAG:-keiner}"
else
    [[ -n "$VORIG_TAG" ]] || { echo "kein Tag vX.Y.Z im Repo - Version mit --version angeben" >&2; exit 2; }
    NEU="$(ra_naechste_version "$VORIG_TAG")" \
        || { echo "Folgeversion von $VORIG_TAG ist nicht festgelegt (Uebertrag in die erste Stelle ist keine 'minor'-Erhoehung) - mit --version angeben" >&2; exit 2; }
    HERKUNFT="hoechster Tag $VORIG_TAG + 1"
fi
meld "Version: $NEU  ($HERKUNFT)"
meld "Archiv:  $(zeige_pfad "$ARCHIV")"
meld "Plan:    ${PLAN[*]}"
meld ""
meld "Vorbedingungen:"

if [[ $NACHHOLEN -eq 1 ]]; then
    pz ok "Tag $NEU steht auf HEAD = Release-Commit $HEAD_KURZ - Commit und Tag bleiben$( [[ $PUSH -eq 1 ]] && echo ', nur der Push wird nachgeholt')"
elif git_r rev-parse -q --verify "refs/tags/$NEU" >/dev/null 2>&1; then
    if im_plan abschluss; then pz stop "Tag $NEU existiert schon - diese Version ist veroeffentlicht"
    else pz stop "Tag $NEU existiert schon - naechste freie Version mit --version waehlen"; fi
else
    pz ok "Tag $NEU ist frei"
fi
# Haengt der vorige Release noch lokal (Commit + Tag, aber nicht gepusht)? Lokal gemessen, ohne Netz:
# refs/remotes/origin/master ist der Stand des letzten erfolgreichen Push/Fetch (git setzt ihn beim Push;
# remote.origin.fetch +refs/heads/*:refs/remotes/origin/*). Ist der Commit des hoechsten Tags nicht darin, fehlt
# dessen Push. Nur Hinweis - der Doppelklick zeigt diesen Trockenlauf zuerst; steht der Release-Commit noch auf HEAD,
# pusht Wahl [3] nur, sonst (weitere Commits danach) der Push von Hand.
if [[ $NACHHOLEN -eq 0 && -n "$VORIG_TAG" ]]; then
    om="$(git_r rev-parse -q --verify refs/remotes/origin/master 2>/dev/null || true)"
    tc="$(git_r rev-parse -q --verify "refs/tags/$VORIG_TAG^{commit}" 2>/dev/null || true)"
    anc=0
    if [[ -n "$om" && -n "$tc" ]]; then git_r merge-base --is-ancestor "$tc" "$om" 2>/dev/null || anc=$?; fi
    if (( anc == 1 )); then     # 1 = nicht enthalten (0 = enthalten, sonst Fehler -> kein Hinweis)
        if tag_auf_head "$VORIG_TAG" && ist_release_commit "$VORIG_TAG"; then
            pz warn "$VORIG_TAG: Release-Commit $HEAD_KURZ und Tag stehen lokal, origin/master steht auf $(git_r rev-parse --short "$om") - der Push fehlt.
         Nur pushen: bash release/release_alles.sh --ab abschluss --tag$(opt_text ohne-kein-push) (Doppelklick: Wahl [3])
         oder von Hand: git push origin master $VORIG_TAG"
        else
            pz warn "$VORIG_TAG: der Commit des Tags ($(git_r rev-parse --short "$tc")) fehlt in origin/master ($(git_r rev-parse --short "$om")) - der Push fehlt.
         von Hand: git push origin master $VORIG_TAG"
        fi
    fi
fi

# --- Saubere Baeume: der Code im Binary = der Code im Commit
schmutz() { git_r status --porcelain --untracked-files=all -- "$@" 2>/dev/null || true; }
s="$(schmutz re15_port)"
if [[ -z "$s" ]]; then pz ok "re15_port/ sauber (keine Aenderung, keine unversionierte Datei)"
else
    pz stop "re15_port/ hat $(printf '%s\n' "$s" | grep -c '') nicht committete Aenderung(en) - erst committen (Binary = Commit):"
    while IFS= read -r z; do meld "         $z"; done < <(printf '%s\n' "$s" | head -10)
fi
WERKZEUG=(release/release_alles.sh release/release_alles_lib.sh release/docker_bereit.sh release/release_alles.cmd
          release/build_win_cross.sh release/build_linux_deck.sh release/build_android.sh release/make_package.sh
          release/docker_win_build.sh release/docker_linux_build.sh release/apk_pruefen.sh release/python_finden.sh
          release/apk_asset_gate.py release/gate_urteil.py release/zip_exec_bit.py release/apk_asset_gate.sha256
          release/gate_urteil.sha256 release/apk_signer.sha256 release/docker release/pkg_files)
s="$(schmutz "${WERKZEUG[@]}")"
if [[ -z "$s" ]]; then pz ok "Release-Werkzeug in release/ sauber"
else
    pz stop "Release-Werkzeug in release/ nicht committet - erst committen:"
    while IFS= read -r z; do meld "         $z"; done < <(printf '%s\n' "$s" | head -10)
fi
s="$(schmutz synchro)"
if [[ -n "$s" ]]; then
    n_sync="$(printf '%s\n' "$s" | grep -c '' || true)"
    n_paket="$(printf '%s\n' "$s" | grep -c ' synchro/STAGE' || true)"
    if (( n_paket > 0 )); then
        pz warn "synchro/: $n_paket nicht committete Eintraege unter synchro/STAGE* kommen ins Paket, aber nicht in den Commit:"
        while IFS= read -r z; do meld "         $z"; done < <(printf '%s\n' "$s" | grep ' synchro/STAGE' | head -5)
    else
        pz info "synchro/: $n_sync nicht committete Eintraege, alle unter synchro/unused/ (nicht im Paket)"
    fi
fi
PORT_BAUM="$(git_r rev-parse HEAD:re15_port 2>/dev/null || echo '')"

# --- Release-Notes / Commit-Voraussetzungen
NOTES="$HERE/RELEASE_NOTES.md"
if [[ $NACHHOLEN -eq 1 ]]; then
    # Commit + Tag stehen schon: es zaehlen die Notes IM Release-Commit (die Datei darf schon die naechste Version beginnen)
    NOTES_KOPF="$(git_r show HEAD:release/RELEASE_NOTES.md 2>/dev/null | tr -d '\r' | grep -m1 '^# ' || true)"
else
    NOTES_KOPF="$( [[ -f "$NOTES" ]] && tr -d '\r' < "$NOTES" | grep -m1 '^# ' || true)"
fi
if [[ "$NOTES_KOPF" == "# $NEU" || "$NOTES_KOPF" == "# $NEU "* ]]; then
    pz ok "release/RELEASE_NOTES.md beginnt mit dem Abschnitt $NEU$( [[ $NACHHOLEN -eq 1 ]] && echo ' (im Release-Commit)')"
elif [[ $NACHHOLEN -eq 1 ]]; then
    pz warn "die Release-Notes im Release-Commit beginnen nicht mit '# $NEU ...' (oben: '${NOTES_KOPF:-nichts}') - gepusht wird der Commit, wie er ist"
elif im_plan abschluss; then
    pz stop "--tag: release/RELEASE_NOTES.md beginnt nicht mit '# $NEU ...' (oben steht: '${NOTES_KOPF:-nichts}') - Notes zuerst schreiben"
else
    pz info "release/RELEASE_NOTES.md hat noch keinen Abschnitt $NEU (oben: '${NOTES_KOPF:-nichts}') - fuer --tag noetig"
fi
if im_plan abschluss; then
    [[ "$ZWEIG" == master ]] && pz ok "Zweig master (Release-Commit + Tag gehoeren auf master)" \
        || pz stop "--tag nur auf master (jetzt: $ZWEIG)"
    if [[ $PUSH -eq 1 ]]; then
        git_r remote get-url origin >/dev/null 2>&1 && pz ok "Push-Ziel origin: $(git_r remote get-url origin)" \
            || pz stop "--tag: kein Remote 'origin' (oder --kein-push)"
    else
        pz info "--kein-push: Commit + Tag, Push von Hand"
    fi
else
    pz info "ohne --tag: kein Release-Commit, kein Tag, kein Push (macht der Mensch/Orchestrator)"
fi

# --- Docker
if im_plan windows || im_plan linux; then
    if [[ $DRY -eq 1 ]]; then
        drc=0; d="$(bash "$HERE/docker_bereit.sh" --pruefen 2>&1)" || drc=$?
        case "$drc" in
            0) pz ok "$d" ;;
            1) pz info "$d" ;;          # Docker Desktop ist da, der echte Lauf startet es
            *) pz stop "$d" ;;
        esac
    else
        pz info "Docker: pruefen/starten (release/docker_bereit.sh) ..."
        if d="$(bash "$HERE/docker_bereit.sh" 2>&1)"; then pz ok "$(printf '%s\n' "$d" | tail -1)"
        else pz stop "Docker nicht bereit: $(printf '%s\n' "$d" | tail -1)"; fi
    fi
else
    pz info "Docker nicht noetig (windows/linux nicht im Plan)"
fi

# --- Platz
f_repo="$(frei_gb "$REPO")"; f_arch="$(frei_gb "$ARCHIV")"
for paar in "Repo:$f_repo" "Archiv:$f_arch"; do
    wo="${paar%%:*}"; gb="${paar#*:}"
    if [[ -z "$gb" ]]; then pz warn "freier Platz ($wo) nicht lesbar (df)"
    elif (( gb >= MIN_FREI_GB )); then pz ok "Platz $wo: $gb GiB frei (verlangt $MIN_FREI_GB)"
    else pz stop "Platz $wo: nur $gb GiB frei, verlangt $MIN_FREI_GB (je Lauf ~6,2 GB + Docker-VM)"; fi
done

# --- Werkzeuge
for w in git sha256sum tar; do
    command -v "$w" >/dev/null 2>&1 && pz ok "$w: $(command -v "$w")" || pz stop "$w fehlt"
done
ZIP_PFAD_ZUSATZ=""
if im_plan paket; then
    if command -v zip >/dev/null 2>&1; then pz ok "zip: $(command -v zip)"
    elif [[ -x /c/msys64/usr/bin/zip ]]; then
        ZIP_PFAD_ZUSATZ=/c/msys64/usr/bin
        pz ok "zip: /c/msys64/usr/bin/zip (wird HINTEN an PATH gehaengt, make_package.sh:22-28)"
    else pz stop "zip fehlt (auch nicht in /c/msys64/usr/bin)"; fi
    command -v unzip >/dev/null 2>&1 && pz ok "unzip: $(command -v unzip)" || pz stop "unzip fehlt"
fi
if im_plan android || im_plan paket; then
    if p="$(bash "$HERE/python_finden.sh" 2>&1)"; then
        pz ok "Python: $(printf '%s\n' "$p" | sed -n 's/^PY=//p') ($(printf '%s\n' "$p" | sed -n 's/^PY_VERSION=//p'))"
    else pz stop "kein echtes Python >= 3.8 (release/python_finden.sh): $(printf '%s\n' "$p" | tail -1)"; fi
fi

# --- Vorversion in release/ (die "alte", die ins Archiv kommt)
mapfile -t ALT_VERSIONEN < <(ra_versionen_in "$HERE")
ALT=""
if (( ${#ALT_VERSIONEN[@]} == 0 )); then
    pz info "release/ traegt keine Paket-Volumes - keine Vorversion ins Archiv zu bringen"
elif (( ${#ALT_VERSIONEN[@]} > 1 )); then
    pz stop "release/ traegt Volumes mehrerer Versionen (${ALT_VERSIONEN[*]}) - Rest eines abgebrochenen Paketlaufs? von Hand klaeren"
else
    ALT="${ALT_VERSIONEN[0]}"
    case "$(ra_version_vergleich "$ALT" "$NEU")" in
        0)  pz info "release/ traegt schon $NEU (frueherer Lauf) - die Vorversion wurde damals behandelt" ;;
        1)  pz stop "release/ traegt $ALT, NEUER als $NEU - Version pruefen" ;;
        -1) if [[ -d "$ARCHIV/$ALT" ]]; then pz ok "Vorversion $ALT liegt im Archiv - wird im Schritt 'alt' mit sha256sum -c geprueft"
            else pz ok "Vorversion $ALT fehlt im Archiv - Schritt 'alt' legt $(zeige_pfad "$ARCHIV/$ALT") an"; fi ;;
    esac
fi
if [[ -d "$ARCHIV/$NEU" ]] && im_plan archiv; then
    if im_plan paket; then
        # Ein neuer Paketbau ergibt andere Bytes (Zeitstempel im Zip) - der Archiv-Schritt wuerde nach Stunden am
        # Vergleich scheitern. Deshalb hier, vor dem Bauen; geloescht wird im Archiv nichts.
        pz stop "Archivordner $NEU existiert schon ($(ra_zeit "$(ra_mtime "$ARCHIV/$NEU")")) - $NEU ist schon gebaut und archiviert.
         Nur veroeffentlichen: bash release/release_alles.sh --ab abschluss --tag$(opt_text)
         Neu bauen: den Ordner $(zeige_pfad "$ARCHIV/$NEU") vorher umbenennen oder entfernen"
    else
        pz warn "Archivordner $NEU existiert schon - Schritt 'archiv' vergleicht: gleich = ok, abweichend = Abbruch"
    fi
fi

# --- Eingaben uebersprungener Bauschritte (nach --ab)
if im_plan paket; then
    for paar in "windows:$HERE/win_out/re15_pc.exe" "android:$HERE/re15_port_${NEU}_android.apk" "linux:$HERE/linux_out/re15_pc"; do
        st="${paar%%:*}"; f="${paar#*:}"
        im_plan "$st" && continue
        if [[ -s "$f" ]]; then pz ok "$st uebersprungen: $(zeige_pfad "$f") von $(ra_zeit "$(ra_mtime "$f")") (Frische prueft make_package.sh)"
        else pz stop "$st uebersprungen, aber $(zeige_pfad "$f") fehlt"; fi
    done
fi

# --- Sperre: nie zwei Laeufe im selben Baum (gemeinsame win_out/linux_out/Volumes)
SPERRE="$HERE/logs/.release_alles.sperre"
if [[ -d "$SPERRE" ]]; then
    pid="$(cat "$SPERRE/pid" 2>/dev/null || true)"
    if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then pz stop "ein anderer Lauf ist aktiv (PID $pid, $(zeige_pfad "$SPERRE"))"
    else pz warn "verwaiste Sperre eines frueheren Laufs (PID ${pid:-?}) - wird uebernommen"; fi
fi

last="$(fremde_last)"
[[ -n "$last" ]] && pz warn "andere Bau-/Testprozesse laufen ($last) - Gates unter Last koennen flattern"

# --- Plan anzeigen
meld ""
meld "Schritte:"
schritt_text() {
    case "$1" in
        alt)       if [[ -n "$ALT" && "$ALT" != "$NEU" ]]; then echo "Vorversion $ALT ins Archiv bringen bzw. dort pruefen (sha256sum -c)"
                   else echo "keine Vorversion zu archivieren"; fi ;;
        windows)   echo "bash release/build_win_cross.sh -> release/win_out/re15_pc.exe (zuletzt 78 s, Runde 37)" ;;
        android)   echo "bash release/build_android.sh --version $NEU -> release/re15_port_${NEU}_android.apk (zuletzt ~3 min, Runde 32)" ;;
        linux)     echo "$( [[ -n "$LINUX_JOBS" ]] && echo "RE15_CTEST_JOBS=$LINUX_JOBS ")bash release/build_linux_deck.sh -> release/linux_out/re15_pc (volle Suite; zuletzt ctest 10827 s seriell, Runde 37)" ;;
        paket)     echo "bash release/make_package.sh --version $NEU -> Volumes win64 + linux_steamdeck_x64 + android, SHA256SUMS.txt" ;;
        archiv)    echo "$(zeige_pfad "$ARCHIV/$NEU") anlegen + sha256sum -c; alte APK aus release/ (liegt geprueft im Archiv); im Archiv wird nichts geloescht" ;;
        abschluss) if [[ $NACHHOLEN -eq 1 ]]; then
                       echo "Commit + Tag $NEU stehen schon (HEAD)$( [[ $PUSH -eq 1 ]] && echo " - nur git push origin master $NEU, Kontrolle per git ls-remote origin" || echo " - nichts zu tun (--kein-push)")"
                   else
                       echo "git commit -F (release: $NEU), git tag -a $NEU$( [[ $PUSH -eq 1 ]] && echo ", git push origin master $NEU")"
                   fi ;;
    esac
}
i=0
for s in "${PLAN[@]}"; do
    i=$((i + 1))
    meld "  $i/${#PLAN[@]} $(printf '%-9s' "$s") $(schritt_text "$s")"
done
LAUF_ID="${NEU}_$(date +%Y%m%d-%H%M%S)"
LOGDIR="$HERE/logs/$LAUF_ID"
meld "Protokolle: $(zeige_pfad "$LOGDIR")/NN_<schritt>.log"
meld ""

if [[ $DRY -eq 1 ]]; then
    if (( BLOCKER > 0 )); then
        meld "TROCKENLAUF: ein echter Lauf wuerde in den Vorbedingungen ABBRECHEN ($BLOCKER Punkt(e) [XX] oben)"
        exit 1
    fi
    meld "TROCKENLAUF-OK $NEU ($WARNUNGEN Hinweis(e) [!!]) - nichts gebaut, nichts geschrieben"
    exit 0
fi
(( BLOCKER == 0 )) || die "$BLOCKER Vorbedingung(en) nicht erfuellt (Zeilen [XX] oben)"

# =============================================================================
# Ab hier wird geschrieben: Sperre, Protokolle
# =============================================================================
mkdir -p "$HERE/logs"
if [[ -d "$SPERRE" ]]; then rm -rf "$SPERRE"; fi     # verwaist (die Vorbedingung fand keinen lebenden Lauf)
mkdir "$SPERRE" 2>/dev/null || die "Sperre $SPERRE nicht anlegbar (laeuft gerade ein zweiter Lauf an?)"
echo "$$" > "$SPERRE/pid"
SPERRE_MEIN=1
aufraeumen() {
    local rc=$?
    if [[ "${SPERRE_MEIN:-0}" == 1 ]]; then rm -rf "$SPERRE"; fi
    return $rc
}
trap aufraeumen EXIT
trap 'meld ""; meld "!!! unterbrochen in Schritt $AKTUELL"; meld "RELEASE-ALLES-ABBRUCH ($AKTUELL, unterbrochen)"; exit 130' INT TERM

mkdir -p "$LOGDIR"
ABLAUF_LOG="$LOGDIR/00_ablauf.log"
printf '%s\n' "${PUFFER[@]}" > "$ABLAUF_LOG"
T_LAUF=$(date +%s)
ERGEBNIS=()

dauer() { local s=$1; printf '%dh%02dm%02ds' $((s / 3600)) $((s % 3600 / 60)) $((s % 60)); }

stand_pruefen() {   # der Code darf sich zwischen den Bau-Schritten nicht aendern
    local s now
    s="$(schmutz re15_port)"
    [[ -z "$s" ]] || die "re15_port/ wurde waehrend des Laufs geaendert - die Binaries stammten aus verschiedenen Staenden:
$(printf '%s\n' "$s" | head -5)"
    now="$(git_r rev-parse HEAD:re15_port 2>/dev/null || echo '')"
    [[ "$now" == "$PORT_BAUM" ]] || die "re15_port/ hat seit dem Laufbeginn einen anderen Commit-Stand ($PORT_BAUM -> $now)"
}

NR=0
SCHRITT_LOG=""
# lauf <name> <anzeige> <befehl...>: Ausgabe auf den Schirm UND ins Schritt-Protokoll; die Rueckgabe des BEFEHLS
# zaehlt (PIPESTATUS[0]), nicht die von tee (Memory reai-v2-releasebau-pipe-schluckt-fehler: '| tail' meldete exit 0).
lauf() {
    local name="$1" anzeige="$2"; shift 2
    local st rc
    meld "    \$ $anzeige"
    set +e
    "$@" 2>&1 | tee -a "$SCHRITT_LOG"
    st=("${PIPESTATUS[@]}")
    set -e
    rc="${st[0]}"
    [[ "$rc" == 0 ]] || die "$name endete mit Rueckgabe $rc"
    [[ "${st[1]}" == 0 ]] || die "Protokoll $SCHRITT_LOG nicht schreibbar (tee ${st[1]})"
}
marke() {   # $1 = Text, der im Schritt-Protokoll stehen muss
    grep -aqF -- "$1" "$SCHRITT_LOG" || die "Erfolgsmarke '$1' fehlt im Protokoll - der Schritt meldete Erfolg, ohne fertig zu sein"
}
schritt_beginn() {
    AKTUELL="$1"
    NR=$((NR + 1))
    SCHRITT_LOG="$LOGDIR/$(printf '%02d' "$NR")_$1.log"
    : > "$SCHRITT_LOG"
    T0=$(date +%s)
    meld ""
    meld "=== [$NR/${#PLAN[@]}] $1 - $(schritt_text "$1")"
    meld "    Beginn $(ra_zeit "$T0"), Protokoll $(zeige_pfad "$SCHRITT_LOG")"
}
schritt_ende() {   # $1 = Zusammenfassung
    local d=$(( $(date +%s) - T0 ))
    meld "=== [$NR/${#PLAN[@]}] $AKTUELL OK in $(dauer $d): $1"
    ERGEBNIS+=("$(printf '%-9s %-10s %s' "$AKTUELL" "$(dauer $d)" "$1")")
}
artefakt_text() { echo "$(zeige_pfad "$1") $(ra_groesse "$1") B, $(ra_zeit "$(ra_mtime "$1")"), sha256 $(ra_sha256 "$1" | cut -c1-16)..."; }
frisch_oder_abbruch() { local m; m="$(ra_artefakt_frisch "$1" "$T0" 2>&1)" || die "$m"; }

# --- Schritt alt -------------------------------------------------------------------
s_alt() {
    schritt_beginn alt
    if [[ -z "$ALT" || "$ALT" == "$NEU" ]]; then
        schritt_ende "nichts zu tun (release/ traegt ${ALT:-keine Volumes})"
        return
    fi
    local ziel="$ARCHIV/$ALT" apk="re15_port_${ALT}_android.apk" erg n_rel n_sums
    # Die Volumes in release/ muessen genau die aus release/SHA256SUMS.txt sein und stimmen.
    [[ -f "$HERE/SHA256SUMS.txt" ]] || die "release/SHA256SUMS.txt fehlt - die Vorversion $ALT ist nicht pruefbar"
    n_rel="$(ra_volumen_in "$HERE" | grep -c '' || true)"
    n_sums="$(ra_sums_namen "$HERE/SHA256SUMS.txt" | grep -c '' || true)"
    diff <(ra_volumen_in "$HERE") <(ra_sums_namen "$HERE/SHA256SUMS.txt" | LC_ALL=C sort) > /dev/null \
        || die "release/SHA256SUMS.txt ($n_sums Namen) passt nicht zu den Volumes in release/ ($n_rel)"
    erg="$(ra_sums_pruefen "$HERE" SHA256SUMS.txt 2>&1 | tee -a "$SCHRITT_LOG")" \
        || die "Volumes der Vorversion $ALT in release/ stimmen nicht mit release/SHA256SUMS.txt: $erg"
    meld "    release/ $ALT: $n_rel Volumes, sha256sum -c $erg"
    local mit_apk=0
    if [[ -f "$HERE/$apk" && -f "$HERE/SHA256SUMS_android.txt" ]] && ra_sums_namen "$HERE/SHA256SUMS_android.txt" | grep -qxF "$apk"; then
        ra_sums_pruefen "$HERE" SHA256SUMS_android.txt >> "$SCHRITT_LOG" 2>&1 \
            || die "release/$apk stimmt nicht mit release/SHA256SUMS_android.txt"
        mit_apk=1
    fi
    if [[ -d "$ziel" ]]; then
        ra_sums_gleich "$ziel/SHA256SUMS.txt" "$HERE/SHA256SUMS.txt" \
            || die "im Archiv liegt ein ANDERES $ALT als in release/ ($(zeige_pfad "$ziel/SHA256SUMS.txt") weicht ab) - von Hand klaeren"
        erg="$(ra_archiv_pruefen "$ziel" 2>&1 | tee -a "$SCHRITT_LOG")" \
            || die "Archiv $(zeige_pfad "$ziel") ist FEHLERHAFT: $erg"
        if [[ $mit_apk -eq 1 && ! -f "$ziel/$apk" ]]; then
            meld "    Hinweis: $apk fehlt im Archiv, liegt aber in release/ - bleibt dort liegen"
        fi
        schritt_ende "Vorversion $ALT war schon im Archiv, geprueft: $erg"
    else
        if [[ $mit_apk -eq 1 ]]; then
            erg="$(ra_archiv_anlegen "$HERE" "$ARCHIV" "$ALT" "$HERE/SHA256SUMS.txt" "$HERE/SHA256SUMS_android.txt" "$apk" 2>&1 | tee -a "$SCHRITT_LOG")" \
                || die "Vorversion $ALT nicht archivierbar: $erg"
        else
            meld "    Hinweis: release/$apk fehlt (oder steht nicht in SHA256SUMS_android.txt) - archiviert werden die Volumes (der Android-Satz enthaelt die APK)"
            erg="$(ra_archiv_anlegen "$HERE" "$ARCHIV" "$ALT" "$HERE/SHA256SUMS.txt" 2>&1 | tee -a "$SCHRITT_LOG")" \
                || die "Vorversion $ALT nicht archivierbar: $erg"
        fi
        schritt_ende "Vorversion $ALT ins Archiv gebracht ($(zeige_pfad "$ziel")): $erg"
    fi
}

# --- Bau-Schritte ------------------------------------------------------------------
s_windows() {
    schritt_beginn windows
    stand_pruefen
    local exe="$HERE/win_out/re15_pc.exe"
    rm -f "$exe"            # ein altes Binary darf nicht als neues durchgehen
    lauf "build_win_cross.sh" "bash release/build_win_cross.sh" bash "$HERE/build_win_cross.sh"
    marke "WIN-CROSS-BUILD-OK"
    frisch_oder_abbruch "$exe"
    stand_pruefen
    schritt_ende "$(artefakt_text "$exe")"
}
s_android() {
    schritt_beginn android
    stand_pruefen
    local apk="$HERE/re15_port_${NEU}_android.apk"
    rm -f "$apk" "$apk.ungeprueft"
    lauf "build_android.sh" "bash release/build_android.sh --version $NEU" bash "$HERE/build_android.sh" --version "$NEU"
    marke "ANDROID-BUILD-OK"
    frisch_oder_abbruch "$apk"
    frisch_oder_abbruch "$HERE/SHA256SUMS_android.txt"
    ra_sums_namen "$HERE/SHA256SUMS_android.txt" | grep -qxF "re15_port_${NEU}_android.apk" \
        || die "SHA256SUMS_android.txt nennt nicht re15_port_${NEU}_android.apk"
    ra_sums_pruefen "$HERE" SHA256SUMS_android.txt >> "$SCHRITT_LOG" 2>&1 \
        || die "APK stimmt nicht mit SHA256SUMS_android.txt"
    stand_pruefen
    schritt_ende "$(artefakt_text "$apk")"
}
LINUX_TESTS=""
s_linux() {
    schritt_beginn linux
    stand_pruefen
    local bin="$HERE/linux_out/re15_pc"
    rm -f "$bin"
    if [[ -n "$LINUX_JOBS" ]]; then
        lauf "build_linux_deck.sh" "RE15_CTEST_JOBS=$LINUX_JOBS bash release/build_linux_deck.sh" \
            env RE15_CTEST_JOBS="$LINUX_JOBS" bash "$HERE/build_linux_deck.sh"
    else
        lauf "build_linux_deck.sh" "bash release/build_linux_deck.sh" bash "$HERE/build_linux_deck.sh"
    fi
    marke "LINUX-BUILD-OK"
    LINUX_TESTS="$(grep -aoE 'Tests: [0-9]+ gelaufen = [0-9]+ registriert' "$SCHRITT_LOG" | tail -1 || true)"
    [[ -n "$LINUX_TESTS" ]] || die "Zaehlzeile 'Tests: N gelaufen = N registriert' fehlt im Protokoll (docker_linux_build.sh:131)"
    frisch_oder_abbruch "$bin"
    stand_pruefen
    schritt_ende "$(artefakt_text "$bin"); $LINUX_TESTS"
}

# --- Paket -------------------------------------------------------------------------
s_paket() {
    schritt_beginn paket
    stand_pruefen
    if [[ -n "$ZIP_PFAD_ZUSATZ" ]]; then
        lauf "make_package.sh" "PATH=\"\$PATH:$ZIP_PFAD_ZUSATZ\" bash release/make_package.sh --version $NEU" \
            env PATH="$PATH:$ZIP_PFAD_ZUSATZ" bash "$HERE/make_package.sh" --version "$NEU"
    else
        lauf "make_package.sh" "bash release/make_package.sh --version $NEU" bash "$HERE/make_package.sh" --version "$NEU"
    fi
    marke "== Fertig =="
    # ALLE Systeme: make_package.sh liefert ohne APK still nur die PC-Saetze ("kein Android-Paket", :931)
    local p f n_neu n_sums
    for p in "${RA_PLATTFORMEN[@]}"; do
        f="$HERE/re15_port_${NEU}_$p.zip"
        [[ -f "$f" ]] || die "Satz $p fehlt: $(zeige_pfad "$f") - nicht alle Systeme gepackt"
    done
    while IFS= read -r f; do
        [[ "$(ra_volumen_version "$f")" == "$NEU" ]] || die "in release/ liegt noch ein Volume einer anderen Version: $f"
        frisch_oder_abbruch "$HERE/$f"
    done < <(ra_volumen_in "$HERE")
    frisch_oder_abbruch "$HERE/SHA256SUMS.txt"
    diff <(ra_volumen_in "$HERE") <(ra_sums_namen "$HERE/SHA256SUMS.txt" | LC_ALL=C sort) > /dev/null \
        || die "SHA256SUMS.txt nennt nicht genau die Volumes in release/"
    n_neu="$(ra_volumen_in "$HERE" | grep -c '' || true)"
    n_sums="$(ra_sums_pruefen "$HERE" SHA256SUMS.txt 2>&1 | tee -a "$SCHRITT_LOG")" || die "sha256sum -c SHA256SUMS.txt: $n_sums"
    # git: neue Volumes vorgemerkt, alte aus dem Index (make_package.sh:979-1049)
    while IFS= read -r f; do
        git_r ls-files --error-unmatch -- "release/$f" >/dev/null 2>&1 || die "release/$f ist nicht im git-Index vorgemerkt"
    done < <(ra_volumen_in "$HERE")
    while IFS= read -r f; do
        [[ "$(ra_volumen_version "$f")" == "$NEU" ]] || die "git-Index fuehrt noch $f"
    done < <(git_r ls-files -- 'release/re15_port_v*' | sed 's#^release/##')
    stand_pruefen
    schritt_ende "$n_neu Volumes (${RA_PLATTFORMEN[*]}), sha256sum -c $n_sums, im git-Index vorgemerkt"
}

# --- Archiv ------------------------------------------------------------------------
s_archiv() {
    schritt_beginn archiv
    local ziel="$ARCHIV/$NEU" apk="re15_port_${NEU}_android.apk" erg f v
    [[ -f "$HERE/$apk" ]] || die "release/$apk fehlt"
    ra_sums_namen "$HERE/SHA256SUMS_android.txt" | grep -qxF "$apk" || die "SHA256SUMS_android.txt nennt $apk nicht"
    if [[ -d "$ziel" ]]; then
        { ra_sums_gleich "$ziel/SHA256SUMS.txt" "$HERE/SHA256SUMS.txt" && ra_sums_gleich "$ziel/SHA256SUMS_android.txt" "$HERE/SHA256SUMS_android.txt"; } \
            || die "Archivordner $(zeige_pfad "$ziel") existiert mit ANDEREM Inhalt - nicht ueberschrieben; von Hand pruefen und ggf. entfernen"
        erg="$(ra_archiv_pruefen "$ziel" 2>&1 | tee -a "$SCHRITT_LOG")" || die "Archiv $(zeige_pfad "$ziel") ist FEHLERHAFT: $erg"
        meld "    $NEU lag schon gleich im Archiv: $erg"
    else
        erg="$(ra_archiv_anlegen "$HERE" "$ARCHIV" "$NEU" "$HERE/SHA256SUMS.txt" "$HERE/SHA256SUMS_android.txt" "$apk" 2>&1 | tee -a "$SCHRITT_LOG")" \
            || die "Archiv $NEU nicht angelegt: $erg"
        meld "    Archiv $(zeige_pfad "$ziel"): $erg"
    fi
    # Alte APKs aus release/ - nur wenn eine BYTEGLEICHE Kopie im Archiv liegt ("die alte ins Archiv bringen").
    local weg=0 bleibt=0
    for f in "$HERE"/re15_port_v*_android.apk; do
        [[ -f "$f" ]] || continue
        v="${f##*/re15_port_}"; v="${v%_android.apk}"
        [[ "$v" == "$NEU" ]] && continue
        if [[ -f "$ARCHIV/$v/${f##*/}" && "$(ra_sha256 "$f")" == "$(ra_sha256 "$ARCHIV/$v/${f##*/}")" ]]; then
            rm -f "$f"
            meld "    release/${f##*/} entfernt - liegt bytegleich in $(zeige_pfad "$ARCHIV/$v")"
            weg=$((weg + 1))
        else
            meld "    release/${f##*/} bleibt liegen - keine bytegleiche Kopie im Archiv"
            bleibt=$((bleibt + 1))
        fi
    done
    schritt_ende "$(zeige_pfad "$ziel"): $erg; alte APK aus release/: $weg entfernt, $bleibt geblieben; im Archiv nichts geloescht"
}

# --- Abschluss (--tag) ------------------------------------------------------------------
s_abschluss() {
    schritt_beginn abschluss
    local f b erlaubt msg m s z kurz
    ra_archiv_pruefen "$ARCHIV/$NEU" >> "$SCHRITT_LOG" 2>&1 || die "Archiv $NEU fehlt oder ist fehlerhaft - erst Schritt 'archiv'"
    [[ "$(ra_versionen_in "$HERE")" == "$NEU" ]] || die "release/ traegt nicht genau die Volumes von $NEU"
    ra_sums_pruefen "$HERE" SHA256SUMS.txt >> "$SCHRITT_LOG" 2>&1 || die "release/SHA256SUMS.txt stimmt nicht"
    # Der Tag muss den Stand tragen, aus dem gepackt wurde: kein Commit an Paket-Inhalt NACH dem Paketbau
    # (Code + Assets + Aufnahmen + Paketdateien; Regel wie make_package.sh check_binary_fresh, :160-191).
    local inhalt_t aeltestes=""
    inhalt_t="$(git_r log -1 --format=%ct -- re15_port/engine re15_port/include re15_port/platform \
                re15_port/shared_assets synchro release/pkg_files 2>/dev/null || true)"
    while IFS= read -r f; do
        m="$(ra_mtime "$HERE/$f")"
        if [[ -z "$aeltestes" ]] || (( m < aeltestes )); then aeltestes="$m"; fi
    done < <(ra_volumen_in "$HERE")
    if [[ -n "$inhalt_t" && -n "$aeltestes" ]] && (( inhalt_t > aeltestes )); then
        die "nach dem Paketbau ($(ra_zeit "$aeltestes")) wurde Paket-Inhalt committet ($(ra_zeit "$inhalt_t"),
    $(git_r log -1 --format='%h %s' -- re15_port/engine re15_port/include re15_port/platform re15_port/shared_assets synchro release/pkg_files)) -
    der Tag zeigte auf einen Stand, den die Pakete nicht enthalten. Neu bauen (ohne --ab)."
    fi
    # Wiederholbar (Abnahme 0, M1): was ein frueherer Lauf schon angelegt hat (Commit, Tag), wird geprueft und nicht
    # neu angelegt - ein Abbruch nach dem Commit (Tag oder Push scheiterte) endet sonst in "nichts vorgemerkt" bzw.
    # einem zweiten Release-Commit.
    if ist_release_commit "$NEU"; then
        s="$(paket_abweichung || true)"
        [[ -z "$s" ]] || die "HEAD ist schon der Release-Commit $NEU, aber die Paketdateien in release/ weichen davon ab - von Hand klaeren:
$(printf '%s\n' "$s" | head -5)"
        meld "    Release-Commit steht schon: $(git_r log -1 --format='%h %s') - wird nicht neu angelegt"
    else
        git_r add -- release/SHA256SUMS.txt release/SHA256SUMS_android.txt
        if [[ -n "$(git_r status --porcelain -- release/RELEASE_NOTES.md)" ]]; then git_r add -- release/RELEASE_NOTES.md; fi
        # Nur Paketdateien gehoeren in den Release-Commit (Form wie 2077bb8e: 6 Volumes umbenannt + 2 SUMS).
        while IFS= read -r f; do
            [[ -n "$f" ]] || continue
            b="${f#release/}"
            erlaubt=0
            [[ "$f" == release/* ]] && ra_volumen_version "$b" >/dev/null 2>&1 && erlaubt=1
            case "$f" in release/SHA256SUMS.txt|release/SHA256SUMS_android.txt|release/RELEASE_NOTES.md) erlaubt=1 ;; esac
            [[ $erlaubt -eq 1 ]] || die "fremde vorgemerkte Aenderung im Index: $f - gehoert nicht in den Release-Commit"
        done < <(git_r diff --cached --no-renames --name-only)
        [[ -n "$(git_r diff --cached --name-only)" ]] || die "nichts vorgemerkt - ist $NEU schon committet?"
        msg="$LOGDIR/commit_msg.log"
        {
            echo "release: $NEU (Windows + Linux/Steam Deck + Android)"
            echo
            echo "Gebaut und gepackt mit release/release_alles.sh (Lauf $LAUF_ID)."
            for z in "${ERGEBNIS[@]}"; do echo "  $z"; done
            echo
            echo "Archiv: $(zeige_pfad "$ARCHIV/$NEU") ($(ra_archiv_pruefen "$ARCHIV/$NEU" 2>&1))"
        } > "$msg"
        git_r commit -q -F "$msg" || die "git commit fehlgeschlagen"
    fi
    kurz="$(git_r rev-parse --short HEAD)"
    if git_r rev-parse -q --verify "refs/tags/$NEU" >/dev/null 2>&1; then
        tag_auf_head "$NEU" \
            || die "Tag $NEU zeigt auf $(git_r rev-parse --short "$NEU^{commit}"), nicht auf den Release-Commit $kurz - von Hand klaeren"
        meld "    Tag $NEU steht schon auf dem Release-Commit - wird nicht neu angelegt"
    else
        git_r tag -a "$NEU" -m "$NEU" || die "git tag $NEU fehlgeschlagen"
    fi
    meld "    Commit $kurz, Tag $NEU"
    if [[ $PUSH -eq 1 ]]; then
        # Scheitert der Push, stehen Commit und Tag schon: die Meldung sagt das, und ihre Zeile pusht nur noch
        WEITER_VOR=("Commit $kurz und Tag $NEU stehen lokal - nur der Push fehlt.")
        WEITER_NACH=("(pusht nur, Commit und Tag bleiben; Doppelklick: Wahl [3])" "oder von Hand: git push origin master $NEU")
        lauf "git push" "git push origin master $NEU" git -C "$REPO" push origin master "$NEU"
        push_pruefen
        schritt_ende "Commit $kurz, Tag $NEU, gepusht (origin master + $NEU, von origin bestaetigt)"
    else
        schritt_ende "Commit $kurz, Tag $NEU (nicht gepusht: --kein-push)"
        meld "    Pushen spaeter: bash release/release_alles.sh --version $NEU --ab abschluss --tag$(opt_text ohne-kein-push) (Doppelklick: Wahl [3])"
        meld "    oder von Hand: git push origin master $NEU"
    fi
}
# paket_abweichung: Paketdateien in release/ (Volumes, SHA256SUMS*.txt), die vom Commit HEAD abweichen
# (geaendert, vorgemerkt oder unversioniert) - je Zeile wie git status --porcelain
paket_abweichung() {
    local z p
    git_r status --porcelain --untracked-files=all -- release | while IFS= read -r z; do
        p="${z:3}"; p="${p##* -> }"; p="${p#release/}"
        if ra_volumen_version "$p" >/dev/null 2>&1 || [[ "$p" == SHA256SUMS.txt || "$p" == SHA256SUMS_android.txt ]]; then
            printf '%s\n' "$z"
        fi
    done
}
# push_pruefen: Erfolgsmarke des Pushs - origin SELBST traegt master = HEAD und den Tag $NEU (dasselbe Tag-Objekt);
# Rueckgabe 0 von git push allein ist es nicht (gleiche Regel wie Marke + Artefakt der Bau-Schritte)
push_pruefen() {
    local r h t
    r="$(git_r ls-remote origin refs/heads/master "refs/tags/$NEU" 2>>"$SCHRITT_LOG")" \
        || die "git ls-remote origin fehlgeschlagen - der Push ist nicht nachpruefbar"
    printf '%s\n' "$r" >> "$SCHRITT_LOG"
    h="$(printf '%s\n' "$r" | awk '$2 == "refs/heads/master" { print $1 }')"
    t="$(printf '%s\n' "$r" | awk -v r="refs/tags/$NEU" '$2 == r { print $1 }')"
    [[ "$h" == "$(git_r rev-parse HEAD)" ]] \
        || die "origin master steht nach dem Push auf '${h:-nichts}', nicht auf dem Release-Commit $(git_r rev-parse --short HEAD)"
    [[ "$t" == "$(git_r rev-parse "refs/tags/$NEU")" ]] \
        || die "origin traegt nach dem Push nicht den Tag $NEU dieses Repos ('${t:-fehlt}')"
    meld "    origin bestaetigt (git ls-remote): master = $(git_r rev-parse --short HEAD), Tag $NEU = ${t:0:12}"
}

# =============================================================================
# Lauf
# =============================================================================
for s in "${PLAN[@]}"; do
    "s_$s"
done

AKTUELL=fertig
meld ""
meld "== Zusammenfassung $NEU (gesamt $(dauer $(( $(date +%s) - T_LAUF )))) =="
for z in "${ERGEBNIS[@]}"; do meld "  $z"; done
letzter="${PLAN[${#PLAN[@]}-1]}"
if [[ "$letzter" != archiv && "$letzter" != abschluss ]]; then
    # --bis: nur ein Teil der Kette lief - kein "fertig"
    naechster=""
    for (( i = 0; i < ${#SCHRITTE[@]} - 1; i++ )); do
        if [[ "${SCHRITTE[$i]}" == "$letzter" ]]; then naechster="${SCHRITTE[$((i + 1))]}"; fi
    done
    meld ""
    meld "Teillauf bis '$letzter' fertig. Weiter mit: $(weiter_zeile "$naechster")"
    meld "RELEASE-ALLES-TEILLAUF-OK $NEU (${PLAN[*]})"
    exit 0
fi
if ! im_plan abschluss; then
    meld ""
    meld "Noch offen (macht der Mensch): Release-Notes '# $NEU ...' oben in release/RELEASE_NOTES.md, dann"
    meld "  $(weiter_zeile abschluss --tag)     (Commit + Tag + Push; Doppelklick: Wahl [3])"
fi
meld "RELEASE-ALLES-OK $NEU"
