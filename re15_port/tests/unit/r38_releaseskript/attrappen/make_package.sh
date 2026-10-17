#!/usr/bin/env bash
# Attrappe von release/make_package.sh (Test r38_releaseskript) - siehe attrappe_gemeinsam.sh.
# Tut fuer release_alles.sh sichtbar, was das echte tut: Split-Volumes je Plattform (.z01 + .zip), SHA256SUMS.txt,
# alte Volumes aus release/ UND aus dem git-Index, neue vorgemerkt (make_package.sh:979-1049), KEIN Commit.
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/attrappe_gemeinsam.sh"
attr_protokoll paket "$@"
V=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --version) V="$2"; shift 2 ;;
        *) echo "Attrappe paket: unbekannte Option $1" >&2; exit 2 ;;
    esac
done
[[ -n "$V" ]] || { echo "Attrappe paket: --version fehlt" >&2; exit 2; }
echo "== Attrappe paket $V =="
if [[ "${FAKE_FAIL:-}" == paket ]]; then echo "ABBRUCH: Attrappe paket simuliert einen Gate-Fehler" >&2; exit 7; fi
[[ -s "$ATTR_HERE/win_out/re15_pc.exe" ]] || { echo "ABBRUCH: win_out/re15_pc.exe fehlt" >&2; exit 1; }
[[ -s "$ATTR_HERE/linux_out/re15_pc" ]]   || { echo "ABBRUCH: linux_out/re15_pc fehlt" >&2; exit 1; }
NAME="re15_port_${V}"
PL=(linux_steamdeck_x64 win64)
if [[ -f "$ATTR_HERE/${NAME}_android.apk" && -z "${FAKE_OHNE_ANDROID:-}" ]]; then PL=(android "${PL[@]}")
else echo "   (kein Android-Paket: ${NAME}_android.apk fehlt - release/build_android.sh laeuft getrennt)"; fi
cd "$ATTR_HERE"
for p in "${PL[@]}"; do
    rm -f "${NAME}_${p}".z*
    printf 'volume 1 %s %s %s' "$V" "$p" "$(date +%s%N)" > "${NAME}_${p}.z01"
    printf 'volume 2 %s %s %s' "$V" "$p" "$(date +%s%N)" > "${NAME}_${p}.zip"
done
sha256sum "${NAME}"_*.z* > SHA256SUMS.txt
echo "== SHA256SUMS.txt geschrieben ($(grep -c '' SHA256SUMS.txt) Volumes) =="
if [[ -z "${FAKE_ALTE_BLEIBEN:-}" ]]; then
    while IFS= read -r f; do
        case "${f##*/}" in "${NAME}"_*) continue ;; esac
        git -C "$ATTR_REPO" rm -q --cached -- "$f"
        rm -f "$ATTR_REPO/$f"
    done < <(git -C "$ATTR_REPO" ls-files -- 'release/re15_port_v*')
fi
for f in "${NAME}"_*.z*; do git -C "$ATTR_REPO" add -- "release/$f"; done
echo "== Fertig =="
