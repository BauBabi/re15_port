#!/usr/bin/env bash
# zeige.sh <laufdir> <frames...> : Spieler + Gegner je Bild (nach dem Sprung, ab der zweiten F1-Zeile)
d=$1; shift
awk 'BEGIN{n=0} /^F250 /{n=1} n' $d/state.log > $d/state_post.log
for f in "$@"; do l=$(grep -m1 "^F$f " $d/state_post.log); echo "F$f $(echo "$l" | grep -o 'PL([^)]*)') $(echo "$l" | grep -o '\[[0-9] t=[^]]*\] hp=[-0-9]*' | sed 's/ss3=0 //;s/stun=[0-9]* //;s/g=[0-9a-f]* //' | tr '\n' ' ')" | cut -c1-460; done
grep 'EV' $d/gr.log | grep -v abprall
