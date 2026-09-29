#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — im Container: Rueckfall-Links /src/<pfad> -> /host/<pfad> anlegen
# =============================================================================
# Gegenstueck zu release/docker/kopie_lauf.sh (Host). Per "source" genutzt von
# release/docker_linux_build.sh und release/docker_win_build.sh; braucht REPO,
# phase() und die(). Ohne Linkliste (distrobox, --mount, direkter docker run) tut
# es nichts.
# Die Liste (NUL-getrennt, Pfade relativ zum Repo) enthaelt alles, was NICHT kopiert
# wurde: oberste Ebene, Geschwister teilweise kopierter Verzeichnisse und alle
# git-ignorierten Eintraege in kopierten Verzeichnissen. Damit gibt es im Container
# jede Datei, die es im Repo gibt — ein Test, der eine fehlende Datei mit
# "SKIP: ... fehlt" quittiert und 0 zurueckgibt, sieht dieselben Dateien wie frueher
# auf dem Bindmount (Dossier analysis/befunde_runde30/nachtrag-linux-bau.md).
# =============================================================================
STAGE_LIST="$REPO/.re15_stage/host_links.lst"
if [[ -f "$STAGE_LIST" ]]; then
    [[ -d /host ]] || die "Kopie-Modus ohne /host-Mount - Rueckfall-Links unmoeglich"
    n_links=0
    while IFS= read -r -d '' p; do
        [[ -n "$p" ]] || continue
        [[ -e "$REPO/$p" || -L "$REPO/$p" ]] && continue
        mkdir -p "$REPO/$(dirname -- "$p")"
        ln -s "/host/$p" "$REPO/$p"
        n_links=$(( n_links + 1 ))
    done < "$STAGE_LIST"
    rm -rf "$REPO/.re15_stage"
    echo "   Quellbaum: Container-Kopie ($(stat -f -c %T "$REPO")), $n_links Rueckfall-Links nach /host"
    phase Rueckfall-Links
fi
