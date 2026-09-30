#!/usr/bin/env bash
# u2_urteil_nach.sh <mut-ordner> - gate_urteil (release/apk_pruefen.sh, unveraendert) auf jedes <mutant>.voll.log
# (Selbsttest Rueckgabe 0) nachrechnen; der erste Lauf k1 rief versehentlich den WSL-Starter statt Git-Bash
cd "$1" || exit 2
for f in *.voll.log; do
  [[ -e "$f" ]] || continue
  u=$(bash "C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_u2/u2_urteil.sh" selbsttest "$(cygpath -m "$PWD/$f")" 0 2>&1); rc=$?
  printf '%s\turteil=%d\t%s\n' "${f%.voll.log}" "$rc" "$(echo "$u" | tail -1 | cut -c1-160)"
done
