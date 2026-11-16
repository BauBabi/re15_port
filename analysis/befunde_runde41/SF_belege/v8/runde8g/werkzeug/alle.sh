#!/usr/bin/env bash
H="$(cd "$(dirname "$0")" && pwd)"; E="$H/szene_d8ef12963.exe"
bash $H/lauf.sh $E $H/cd_8c $H/L/n_8c & bash $H/lauf.sh $E $H/cd_8d $H/L/n_8d & bash $H/lauf.sh $E $H/cd_8g $H/L/n_8g & wait
bash $H/lauf.sh $E $H/cd_8g $H/L/s RE15_SCHWANZ_MASKE=schwanz & bash $H/lauf.sh $E $H/cd_mk_klasse $H/L/mk_klasse & bash $H/lauf.sh $E $H/cd_mk_glied $H/L/mk_glied & wait
bash $H/lauf.sh $E $H/cd_naht_8c $H/L/naht_8c & bash $H/lauf.sh $E $H/cd_naht_8d $H/L/naht_8d & bash $H/lauf.sh $E $H/cd_naht_8g $H/L/naht_8g & wait
echo ALLE-FERTIG
