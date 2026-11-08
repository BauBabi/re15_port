set -u
die(){ echo "DIE: $*"; exit 1; }
HERE="$1"; out="$2"; ASSETS="$3"
    for pf in EMD/EM30_HANGAR.EMD; do
        [[ -s "$out/shared_assets/PSX/$pf" ]] || die "fehlt"
        cmp -s "$ASSETS/$pf" "$out/shared_assets/PSX/$pf" || die "cmp"
    done
    em30_soll="$(awk 'NR==1{print $1}' "$HERE/em30_hangar.sha256")"
    em30_ist="$(sha256sum "$out/shared_assets/PSX/EMD/EM30_HANGAR.EMD" | awk '{print $1}')"
    [[ -n "$em30_soll" && "$em30_ist" == "$em30_soll" ]] || die "pin $em30_ist != $em30_soll"
    echo "TOR-OK $em30_ist"
