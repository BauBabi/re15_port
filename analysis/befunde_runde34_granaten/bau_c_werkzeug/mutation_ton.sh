#!/usr/bin/env bash
# Mutationsprobe: $1 Datei, $2 sed-Ausdruck, $3 Marke. Baut nur probe_r34_plattform, laeuft, stellt zurueck.
set -u
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c
cd "$WT"
f="$1"; expr="$2"; tag="$3"
cp "$f" "/tmp/mut_backup_$$" 2>/dev/null || cp "$f" "$WT/build/r34g_c/mut_backup.tmp"
sed -i "$expr" "$f"
if git diff --quiet -- "$f"; then echo "[$tag] MUTATION GRIFF NICHT"; exit 3; fi
export PATH=/c/msys64/mingw64/bin:$PATH
ninja -C re15_port/build_r34_c probe_r34_plattform_ton > "build/r34g_c/mut_$tag.build.log" 2>&1
b=$?
if [ $b -ne 0 ]; then echo "[$tag] BAU FEHLGESCHLAGEN"; git checkout -- "$f"; exit 4; fi
(cd re15_port/build_r34_c/tests/unit && ./probe_r34_plattform_ton.exe) > "build/r34g_c/mut_$tag.log" 2>&1
rc=$?
fails=$(grep -c "FAIL" "build/r34g_c/mut_$tag.log")
first=$(grep -m3 "FAIL" "build/r34g_c/mut_$tag.log" | cut -c1-110 | tr '\n' ';')
git checkout -- "$f"
echo "[$tag] rc=$rc FAILs=$fails :: $first"
