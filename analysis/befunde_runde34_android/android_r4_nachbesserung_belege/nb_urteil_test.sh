#!/usr/bin/env bash
# Nachbesserung R4-1: gate_urteil / gate_pin_pruefen / gate_laufen (release/apk_pruefen.sh) direkt pruefen.
#  1. echter Selbsttest-Log (OK) -> 0; daraus abgeleitete Faelschungen des Logs -> 2 (bzw. 1)
#  2. Pin: echte Kopie ok; G0 (leer), G1 (main() ohne sys.exit), altes Gate -> Abbruch
#  3. gate_laufen mit echtem Gate (selbsttest) -> 0; mit G1 unter UMGEPINNTEM Pin -> 2 (zweite Schicht)
# Aufruf: nb_urteil_test.sh <ok-selbsttest-log>
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/nb; T="$W/urteil_tmp"; rm -rf "$T"; mkdir -p "$T"
OKLOG="$1"
source release/python_finden.sh >/dev/null || exit 9
die() { echo "DIE: $*" >&2; exit 1; }
source release/apk_pruefen.sh
fehler=0
pruefe() {   # $1 = Titel, $2 = modus, $3 = log, $4 = rc, $5 = erwartetes Urteil
    local u=0 aus
    aus="$(gate_urteil "$2" "$3" "$4")" || u=$?
    if [[ "$u" == "$5" ]]; then echo "ok    $1 -> $u   $(echo "$aus" | cut -c1-150)"; else echo "FALSCH $1 -> $u (soll $5)   $aus"; fehler=$((fehler+1)); fi
}
tr -d '\r' < "$OKLOG" > "$T/ok.log"
pruefe "echter Selbsttest-Log, rc 0" selbsttest "$T/ok.log" 0 0
pruefe "echter Selbsttest-Log, rc 1" selbsttest "$T/ok.log" 1 2
pruefe "echter Selbsttest-Log, rc 2" selbsttest "$T/ok.log" 2 2
: > "$T/leer.log";                                       pruefe "leere Ausgabe (G0), rc 0" selbsttest "$T/leer.log" 0 2
printf '\n\n  \n' > "$T/leer2.log";                      pruefe "nur Leerzeilen, rc 0" selbsttest "$T/leer2.log" 0 2
sed '$d' "$T/ok.log" > "$T/ohne_schluss.log";           pruefe "ohne Schlusszeile, rc 0" selbsttest "$T/ohne_schluss.log" 0 2
sed 's/^== SELBSTTEST-OK: .*$/== SELBSTTEST-FEHLER: 234 von 248 Faellen falsch - das Gate ist NICHT verlaesslich ==/' "$T/ok.log" > "$T/g1.log"
pruefe "FEHLER-Schluss (G1), rc 0" selbsttest "$T/g1.log" 0 2
pruefe "FEHLER-Schluss, rc 1" selbsttest "$T/g1.log" 1 1
sed 's/^== SELBSTTEST-OK: \([0-9]*\)\/\([0-9]*\) /== SELBSTTEST-OK: 257\/\2 /' "$T/ok.log" > "$T/n1.log"; pruefe "OK 257/258" selbsttest "$T/n1.log" 0 2
sed 's/^== SELBSTTEST-OK: [0-9]*\/[0-9]* /== SELBSTTEST-OK: 202\/202 /' "$T/ok.log" > "$T/alt.log";   pruefe "OK 202/202 (altes Gate)" selbsttest "$T/alt.log" 0 2
awk '/^   \[ok\] 017 |^   \[ok\] 17 /{next}{print}' "$T/ok.log" > "$T/fall_weg.log";                  pruefe "Fallzeile 17 fehlt" selbsttest "$T/fall_weg.log" 0 2
awk '{ if (!d && $0 ~ /^   \[ok\] .* rc=1 \(soll 1\)$/) { sub(/rc=1 \(soll 1\)$/, "rc=0 (soll 1)"); d=1 } print }' "$T/ok.log" > "$T/rc.log"
pruefe "eine Fallzeile [ok] rc=0 (soll 1)" selbsttest "$T/rc.log" 0 2
awk '{ if (!d && $0 ~ /^   \[ok\] /) { sub(/^   \[ok\] /, "   [FEHLER] "); d=1 } print }' "$T/ok.log" > "$T/fehlerfall.log"
pruefe "eine Fallzeile [FEHLER]" selbsttest "$T/fehlerfall.log" 0 2
awk '{ if (!d && $0 ~ /^   \[ok\] 002 |^   \[ok\] 02 /) { print; d=1 } print }' "$T/ok.log" > "$T/doppelt.log";  pruefe "Fallzeile 2 doppelt" selbsttest "$T/doppelt.log" 0 2
sed 's/^   Innere Proben: \([0-9]*\)\/\([0-9]*\) /   Innere Proben: 131\/\2 /' "$T/ok.log" > "$T/innen.log"; pruefe "Innere Proben 131/132" selbsttest "$T/innen.log" 0 2
sed 's/^   Innere Proben: [0-9]*\/[0-9]* /   Innere Proben: 116\/116 /' "$T/ok.log" > "$T/innen2.log";   pruefe "Innere Proben 116/116" selbsttest "$T/innen2.log" 0 2
{ cat "$T/ok.log"; echo "Traceback (most recent call last):"; } > "$T/tb.log";   pruefe "Traceback NACH der Schlusszeile" selbsttest "$T/tb.log" 0 2
{ echo "ABBRUCH (Rueckgabe 2): x"; cat "$T/ok.log"; } > "$T/ab.log";            pruefe "ABBRUCH-Zeile im Lauf" selbsttest "$T/ab.log" 0 2
{ echo "== APK-ASSET-GATE-OK: 1 Dateien in 5 Baeumen bytegleich, Tuer-Soll erfuellt, Manifest stimmt, ZIP-Struktur wie Android sie liest =="; cat "$T/ok.log"; } > "$T/zwei.log"
pruefe "zwei Urteilszeilen" selbsttest "$T/zwei.log" 0 2
pruefe "Selbsttest-Log als APK-Urteil" apk "$T/ok.log" 0 2

echo "--- Pin"
mkdir -p "$T/g"; gate_festhalten "$T/g" > "$T/fest.txt" && echo "ok    gate_festhalten (echtes Gate): $(tail -1 "$T/fest.txt" | cut -c1-110)" || { echo "FALSCH gate_festhalten"; fehler=$((fehler+1)); }
: > "$T/G0.py"
( gate_pin_pruefen "$T/G0.py" ) > "$T/pin_g0.txt" 2>&1 && { echo "FALSCH Pin G0 angenommen"; fehler=$((fehler+1)); } || echo "ok    Pin lehnt G0 (0 B) ab: $(grep -m1 DIE "$T/pin_g0.txt" | cut -c1-120)"
/c/Python310/python build/r34a/nb/nb_mutanten.py release/apk_asset_gate.py "$T/mut" > /dev/null
sed 's/^    sys.exit(main())$/    main()/' release/apk_asset_gate.py > "$T/G1.py"
cmp -s release/apk_asset_gate.py "$T/G1.py" && { echo "G1 nicht erzeugt"; exit 9; }
( gate_pin_pruefen "$T/G1.py" ) > "$T/pin_g1.txt" 2>&1 && { echo "FALSCH Pin G1 angenommen"; fehler=$((fehler+1)); } || echo "ok    Pin lehnt G1 ab"
git show e777d796:release/apk_asset_gate.py > "$T/ALT.py"
( gate_pin_pruefen "$T/ALT.py" ) > "$T/pin_alt.txt" 2>&1 && { echo "FALSCH Pin altes Gate angenommen"; fehler=$((fehler+1)); } || echo "ok    Pin lehnt das Gate von HEAD vor R4-1 ab"

echo "--- gate_laufen"
u=0; gate_laufen selbsttest "$GATE_KOPIE" --selbsttest > "$T/lauf_echt.txt" 2>&1 || u=$?
[[ $u == 0 ]] && echo "ok    gate_laufen selbsttest (echt) -> 0: $(grep 'Gate-Urteil' "$T/lauf_echt.txt" | cut -c1-150)" || { echo "FALSCH gate_laufen echt -> $u"; tail -5 "$T/lauf_echt.txt"; fehler=$((fehler+1)); }
# zweite Schicht: G1 UMGEPINNT (eigene Pin-Datei), gate_laufen muss trotzdem 2 liefern
cp release/apk_asset_gate.sha256 "$T/pin_sicherung"
GATE_PIN_DATEI="$T/pin_g1"; printf '%s\n' "$(/c/Python310/python -c "import hashlib,sys; print(hashlib.sha256(open(sys.argv[1],'rb').read()).hexdigest())" "$T/G1.py")" > "$GATE_PIN_DATEI"
u=0; gate_laufen selbsttest "$T/G1.py" --selbsttest > "$T/lauf_g1.txt" 2>&1 || u=$?
[[ $u == 2 ]] && echo "ok    gate_laufen selbsttest G1 umgepinnt -> 2: $(grep -a "Gate-Urteil" "$T/lauf_g1.txt" | cut -c1-150)" || { echo "FALSCH gate_laufen G1 -> $u"; tail -3 "$T/lauf_g1.txt"; fehler=$((fehler+1)); }
: > "$T/G0.py"; printf 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\n' > "$GATE_PIN_DATEI"
u=0; gate_laufen selbsttest "$T/G0.py" --selbsttest > "$T/lauf_g0.txt" 2>&1 || u=$?
[[ $u == 2 ]] && echo "ok    gate_laufen selbsttest G0 umgepinnt -> 2: $(grep 'Gate-Urteil' "$T/lauf_g0.txt" | cut -c1-150)" || { echo "FALSCH gate_laufen G0 -> $u"; tail -3 "$T/lauf_g0.txt"; fehler=$((fehler+1)); }
cmp -s release/apk_asset_gate.sha256 "$T/pin_sicherung" || { echo "Pin-Datei veraendert!"; fehler=$((fehler+1)); }
echo "FEHLER=$fehler"
[[ $fehler == 0 ]]
