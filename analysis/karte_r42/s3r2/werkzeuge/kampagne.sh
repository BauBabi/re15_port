#!/usr/bin/env bash
export R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r2
lauf() { set -- $1; t=$1; r=$2; x=$3; y=$4; z=$5; yaw=$6; g=$7; shift 8; [ -f "$R/schuss/$t/karte.bmp" ] && exit 0; bash $R/schuss2.sh "$t" "$r" "$x" "$y" "$z" "$yaw" "$g" "$@"; }
export -f lauf
cat ${LISTE:-$R/kampagne.txt} | xargs -P ${PAR:-6} -I{} bash -c 'lauf "{}"' > $R/kampagne.log 2>&1
echo FERTIG >> $R/kampagne.log
