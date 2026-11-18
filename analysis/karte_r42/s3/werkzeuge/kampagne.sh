#!/usr/bin/env bash
export R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3
lauf() { set -- $1; t=$1; r=$2; x=$3; z=$4; shift 7; [ -f "$R/schuss/$t/karte.bmp" ] && exit 0; bash $R/schuss1.sh "$t" "$r" "$x" "$z" 96 "$@"; }
export -f lauf
cat $R/kampagne.txt | xargs -P 5 -I{} bash -c 'lauf "{}"' > $R/kampagne.log 2>&1
echo FERTIG >> $R/kampagne.log
