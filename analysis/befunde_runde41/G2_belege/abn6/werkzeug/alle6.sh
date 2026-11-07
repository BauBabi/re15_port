#!/usr/bin/env bash
# alle6.sh <liste> [parallel]: je Zeile "<lauf> <neu|ohne> [alt|r1] [VAR=WERT ...]" ein batch6.sh-Lauf, hoechstens <parallel> gleichzeitig (eigene PIDs)
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn6"
W="$SP/werkzeug"; LISTE="$1"; PAR="${2:-2}"
mkdir -p "$SP/batch"
lauf_eins() {
    local zeile="$1"; local a=($zeile); local l="${a[0]}" m="${a[1]}" x="" ; local envs=()
    for t in "${a[@]:2}"; do if [ "$t" = "alt" ] || [ "$t" = "r1" ]; then x=$t; else envs+=("$t"); fi; done
    local tag="${x:+${x}_}${m}_${l}"; for t in "${envs[@]}"; do case "$t" in SUF=*) tag="$tag${t#SUF=}";; esac; done
    env "${envs[@]}" bash "$W/batch6.sh" "$l" "$m" $x > "$SP/batch/$tag.txt" 2>&1
    echo "$(date +%T) fertig $tag: $(grep -h 'exit=' "$SP"/runs/${tag}*/pid.txt 2>/dev/null | tr '\n' ' ')"
}
export -f lauf_eins; export SP W
grep -v '^#' "$LISTE" | grep -v '^\s*$' | xargs -d '\n' -P "$PAR" -I{} bash -c 'lauf_eins "$@"' _ {}
echo "ALLE_FERTIG $(date +%T)"
