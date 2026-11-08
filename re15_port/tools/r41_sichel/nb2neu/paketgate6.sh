#!/usr/bin/env bash
# NB2neu Schritt 5: Gate-Block von release/make_package.sh (von "for pf in EMD/EM30_HANGAR.EMD" bis "check_tree: EMD/EM30_HANGAR.EMD
# = Pin", unveraendert per sed) gegen Probe-Pakete. Kein ganzes Paket. paketgate6.sh <scratch>
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel
T=$1; mkdir -p "$T"
BLOCK="$(sed -n '/for pf in EMD\/EM30_HANGAR.EMD; do/,/check_tree: EMD\/EM30_HANGAR.EMD = Pin/p' $B/release/make_package.sh)"
echo "Block: $(echo "$BLOCK" | wc -l) Zeilen"
probe_lauf(){ ( HERE=$B/release; ASSETS=$2; out=$1; die(){ echo "  DIE: $*"; exit 1; }
                eval "probe(){ $BLOCK
}"; probe && echo "  GATE OK" ) }
mk(){ rm -rf "$T/$1"; mkdir -p "$T/$1/pak/shared_assets/PSX/EMD" "$T/$1/src/EMD"
      cp "$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD" "$T/$1/src/EMD/"
      cp "$T/$1/src/EMD/"*.EMD "$T/$1/pak/shared_assets/PSX/EMD/"; }
V=6a1e9430d   # NB2 (vor dem Neuaufbau)
echo "== 1 Endstand (Baum = Paket)"; mk a; probe_lauf "$T/a/pak" "$T/a/src"
echo "== 2 EM30 von NB2 in Paket UND Quellbaum"; mk b
git -C "$B" show $V:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD > "$T/b/src/EMD/EM30_HANGAR.EMD"; cp "$T/b/src/EMD/EM30_HANGAR.EMD" "$T/b/pak/shared_assets/PSX/EMD/"; probe_lauf "$T/b/pak" "$T/b/src"
echo "== 3 EM30 fehlt im Paket"; mk c; rm "$T/c/pak/shared_assets/PSX/EMD/EM30_HANGAR.EMD"; probe_lauf "$T/c/pak" "$T/c/src"
echo "== 4 EM30 im Paket alt, Quellbaum neu"; mk d
git -C "$B" show $V:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD > "$T/d/pak/shared_assets/PSX/EMD/EM30_HANGAR.EMD"; probe_lauf "$T/d/pak" "$T/d/src"
echo "== 5 EM30 1 Byte veraendert in Paket UND Quellbaum"; mk e
python -c "import sys;p=sys.argv[1];b=bytearray(open(p,'rb').read());b[5000]^=1;open(p,'wb').write(b)" "$T/e/src/EMD/EM30_HANGAR.EMD"; cp "$T/e/src/EMD/EM30_HANGAR.EMD" "$T/e/pak/shared_assets/PSX/EMD/"; probe_lauf "$T/e/pak" "$T/e/src"
