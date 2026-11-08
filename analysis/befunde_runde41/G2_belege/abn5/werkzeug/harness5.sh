#!/usr/bin/env bash
# G2 Abnahme 5: eigene Harness-Lagen (test_r41_gatorfix.exe nb3_oben / nb3_haengt, Messwerkzeuge des Messers), neu und alter Stand (G2_NB3_ALT=1 = a661eb8aa),
# je Lauf eigenes Arbeitsverzeichnis (gator_boss.log), Spur je Bild (G2_NB3_SPUR) fuer den Bild-fuer-Bild-Vergleich alt/neu.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn5r2"
T="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/tests/unit/test_r41_gatorfix.exe"
export PATH="/c/msys64/mingw64/bin:$PATH" SDL_ASSERT=always_ignore
H="$SP/harness"; mkdir -p "$H"
# Rampe (x 1850..7200, z -18100..-14500): eigenes Raster, dichter als das 18er-Raster der Riegel, auch Raender
RAMPE=""; for z in -18000 -17300 -16700 -16300 -15900 -15300 -14600; do for x in 1950 2318 2600 3000 3400 3800 4800 6000 7100; do RAMPE="$RAMPE $x,$z"; done; done
# Plattform (x -1700..1850, z -20000..-12450)
PLATT=""; for z in -19800 -18500 -17000 -15500 -14000 -12600; do for x in -1550 -500 500 1700; do PLATT="$PLATT $x,$z"; done; done
# Wasser im Band (bis 1800 vor einer Kante): Rampen-Sued, Plattform-Sued, Plattform-West, Rampen-Nord, Plattform-Nord
BAND=""
for x in 2000 3000 4000 5000 6000 7000; do for d in 400 1000 1600; do BAND="$BAND $x,$((-18100 - d)),2"; done; done
for x in -1500 -500 500 1500; do for d in 400 1000 1600; do BAND="$BAND $x,$((-20000 - d)),0"; done; done
for z in -19000 -16000 -13000; do for d in 400 1000 1600; do BAND="$BAND $((-1700 - d)),$z,0"; done; done
for x in 2500 4500 6500; do for d in 400 1000 1600; do BAND="$BAND $x,$((-14500 + d)),4"; done; done
for x in -1000 1000; do for d in 400 1000 1600; do BAND="$BAND $x,$((-12450 + d)),4"; done; done
# Wasser AUSSERHALB der Baender (> 1800 von jeder Blockkante, nicht oben): hier muss neu = alt Bild fuer Bild gelten
FERN="-6500,-22000,0 -6000,-16000,0 -7000,-10000,0 -4000,-7000,0 2000,-7500,4 5500,-8000,4 5500,-11500,4 -1000,-25000,0 5500,-25000,0 4000,-21000,0 6500,-21500,0 -4500,-13500,0 -4200,-19500,0"
lauf() {   # lauf <name> <werkzeug> <wer> <stand> <alt0/1> <lagen...>
    local name="$1" wz="$2" wer="$3" st="$4" alt="$5"; shift 5
    local d="$H/$name"; rm -rf "$d"; mkdir -p "$d"; cd "$d"
    if [ "$alt" = "1" ]; then export G2_NB3_ALT=1; else unset G2_NB3_ALT; fi
    G2_NB3_SPUR="$d/spur.txt" "$T" "$wz" "$wer" "$st" 3600 "$@" > "$d/aus.txt" 2>&1; echo "exit=$?" >> "$d/aus.txt"
    unset G2_NB3_ALT
    echo "$(date +%T) $name: $(grep -E 'SUMME' "$d/aus.txt" | head -2 | tr '\n' ' ' | cut -c1-300)"
}
case "${1:-alle}" in
oben)  for st in ohne neu; do lauf oben_rampe_leon_$st nb3_oben leon $st 0 $RAMPE; lauf oben_platt_leon_$st nb3_oben leon $st 0 $PLATT; done
       lauf alt_oben_rampe_leon_ohne nb3_oben leon ohne 1 $RAMPE; lauf alt_oben_platt_leon_ohne nb3_oben leon ohne 1 $PLATT
       lauf oben_rampe_elza_ohne nb3_oben elza ohne 0 $RAMPE ;;
band)  for st in ohne neu; do lauf band_leon_$st nb3_haengt leon $st 0 $BAND; done
       lauf alt_band_leon_ohne nb3_haengt leon ohne 1 $BAND; lauf band_elza_ohne nb3_haengt elza ohne 0 $BAND ;;
fern)  for st in ohne neu; do lauf fern_leon_$st nb3_haengt leon $st 0 $FERN; lauf alt_fern_leon_$st nb3_haengt leon $st 1 $FERN; done
       lauf fern_elza_ohne nb3_haengt elza ohne 0 $FERN; lauf alt_fern_elza_ohne nb3_haengt elza ohne 1 $FERN ;;
esac
echo "HARNESS_FERTIG ${1:-} $(date +%T)"
