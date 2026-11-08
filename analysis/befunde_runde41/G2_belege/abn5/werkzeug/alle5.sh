#!/usr/bin/env bash
# alle5.sh <liste> [parallel]: je Zeile "<lauf> <neu|ohne> [alt] [VAR=WERT ...]" ein batch5.sh-Lauf, hoechstens <parallel> gleichzeitig (eigene PIDs)
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn5r2"
W="$SP/werkzeug"; LISTE="$1"; PAR="${2:-3}"
mkdir -p "$SP/batch"
lauf_eins() {
    local zeile="$1"; local a=($zeile); local l="${a[0]}" m="${a[1]}" x="" ; local envs=()
    for t in "${a[@]:2}"; do if [ "$t" = "alt" ]; then x=alt; else envs+=("$t"); fi; done
    local tag="${x:+alt_}${m}_${l}"
    env "${envs[@]}" bash "$W/batch5.sh" "$l" "$m" $x > "$SP/batch/$tag.txt" 2>&1
    echo "$(date +%T) fertig $tag: $(grep -h 'exit=' "$SP/runs/$tag/pid.txt" 2>/dev/null | tr '\n' ' ')"
}
export -f lauf_eins; export SP W
grep -v '^#' "$LISTE" | grep -v '^\s*$' | xargs -d '\n' -P "$PAR" -I{} bash -c 'lauf_eins "$@"' _ {}
echo "ALLE_FERTIG $(date +%T)"
