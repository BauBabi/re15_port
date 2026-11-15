#!/usr/bin/env bash
# bash cdroot.sh <asset.emd> <ziel>  -> <ziel>/PSX/EMD/EM30_HANGAR.EMD (Kopie) + <ziel>/RE2/* (die 18 RE2-Dateien, die nur r41/sichel_szene hat, aus git)
# Das Spiel sucht jede Datei der Reihe nach in RE15_CD_ROOT (= <ziel>/PSX), RE15_ASSET_ROOT (Arbeitsbaum r41_sichel_v8) und dem Compile-Default
# (asset_root_pc.c rr_read_list); hier liegt nur das Schwanz-Asset, alles andere kommt aus dem Arbeitsbaum.
set -eu
H="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$2/PSX/EMD" "$2/RE2"
cp -f "$1" "$2/PSX/EMD/EM30_HANGAR.EMD"
cp -f "$H"/re2_szene/* "$2/RE2/"
sha256sum "$2/PSX/EMD/EM30_HANGAR.EMD" | cut -c1-16
