#!/usr/bin/env bash
export R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r4b
lauf() { set -- $1; t=$1; r=$2; x=$3; y=$4; z=$5; yaw=$6; g=$7; shift 8; [ -f "${SCHUSS:-$R/schuss}/$t/karte.bmp" ] && exit 0; o=""; case "$*" in *exit:*) o=$(echo "$*" | sed 's/.*exit:\([0-9]*\).*/\1/');; esac; rest=$(echo "$*" | sed 's/exit:[0-9]*//'); EXIT0=${o:-206} bash $R/schuss.sh "$t" "$r" "$x" "$y" "$z" "$yaw" "$g" $rest; }
export -f lauf
cat ${LISTE:-$R/kampagne.txt} | xargs -P ${PAR:-6} -I{} bash -c 'lauf "{}"' > ${LOG:-$R/kampagne.log} 2>&1
echo FERTIG >> ${LOG:-$R/kampagne.log}
