# Runde 7b: ein Spiellauf (Overlay-Wurzel, Spiel-Asset unberuehrt). Wartet, bis kein re15*-Prozess laeuft (kein paralleler Lauf).
# lauf.sh <laufordner> <emd> [FD] [ENDE]
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
while tasklist 2>/dev/null | grep -qi "^re15"; do sleep 5; done
(time FD=${3:-200-1100/1} bash $L "$1" "$2" ${4:-1110}) > "$1.txt" 2>&1
echo LAUF-FERTIG >> "$1.txt"
