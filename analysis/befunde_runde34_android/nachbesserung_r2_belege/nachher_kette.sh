#!/usr/bin/env bash
# Nachbesserung R2 - volle Kette (release/build_android.sh --gate-only, schreibt nichts) NACH den Aenderungen:
# Referenz positiv, dann je Befund die Faelschung, die vorher durchkam (B3 Tausch, B4 fremder Schluessel,
# B5 unausgerichtet, B1/M1 signiert, B2 29/30 und 0 B im Schattenbaum, B8 Praefix/Kopf signiert).
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
PY=/c/Python310/python
W=build/r34a/nb2
F=analysis/befunde_runde34_android/pruefer_umgehung_r2_belege/r2_faelschen.py
BT="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0"
JB="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin"
KS="$(cygpath -m "$USERPROFILE/.android/debug.keystore")"
cd "$B"
REF=build/r34a/ref_v0.8.19.apk
signieren() {   # $1 unsigniert -> $2: wie AGP ausgerichtet (-P 16 4), dann mit dem Debug-Schluessel signiert
    "$BT/zipalign.exe" -f -P 16 4 "$(cygpath -m "$1")" "$(cygpath -m "$2.tmp")" > /dev/null || return 1
    "$JB/java" -jar "$(cygpath -m $BT/lib/apksigner.jar)" sign --ks "$KS" --ks-pass pass:android --ks-key-alias androiddebugkey \
        --out "$(cygpath -m "$2")" "$(cygpath -m "$2.tmp")" > /dev/null 2>&1; local rc=$?; rm -f "$2.tmp" "$2.tmp.idsig" "$2.idsig"; return $rc
}
kette() {       # $1 Name, $2 APK, [$3 Baum] -> EXIT + erste ABBRUCH-Zeile
    local baum="${3:-$B}" log="$W/logs/nachher_kette_$1.log" rc t0 t1
    t0=$(date +%s)
    bash "$baum/release/build_android.sh" --gate-only "$2" --version v0.8.19 > "$log" 2>&1; rc=$?
    t1=$(date +%s)
    printf '%-28s EXIT=%d (%3d s) %s\n' "$1" "$rc" "$((t1 - t0))" "$(grep -m1 -E '^ABBRUCH|ANDROID-GATES-OK' "$log" | cut -c1-170)"
}
kette ref "$REF"
kette K0_unsigniert "$W/apk/K0.apk"
kette K0_fremder_schluessel "$W/apk/K0_fremd.apk"
kette K0_unausgerichtet "$W/apk/K0_unausgerichtet.apk"
kette F_M1_signiert "$W/apk/F_M1_sig.apk"
signieren "$W/apk/F_praefix.apk" "$W/apk/F_praefix_sig.apk" && kette F_praefix_signiert "$W/apk/F_praefix_sig.apk" || echo "F_praefix: Signieren scheitert ($?)"
signieren "$W/apk/F_kopf_arabisch.apk" "$W/apk/F_kopf_arabisch_sig.apk" && kette F_kopf_arabisch_signiert "$W/apk/F_kopf_arabisch_sig.apk"
# --- B3: Tausch gegen K0 waehrend des Selbsttests (wie toctou_lauf.sh der Gegenpruefung)
cp -f "$REF" $W/apk/toctou.apk; cp -f $W/apk/K0.apk $W/apk/toctou_tausch.apk
LOG=$W/logs/nachher_kette_toctou.log
( bash release/build_android.sh --gate-only $W/apk/toctou.apk --version v0.8.19 > $LOG 2>&1; echo "EXIT=$?" >> $LOG ) &
pid=$!
for i in $(seq 1 900); do
  if grep -q "Volle Asset-Pruefung 1/2" $LOG 2>/dev/null; then mv -f $W/apk/toctou_tausch.apk $W/apk/toctou.apk; echo "B3 $(date +%T): gegen K0 getauscht (waehrend des Selbsttests)"; break; fi
  sleep 0.2
done
wait $pid
printf '%-28s %s | %s\n' "B3_tausch" "$(grep -E '^EXIT=' $LOG)" "$(grep -m1 -A2 -E '^ABBRUCH|ANDROID-GATES-OK' $LOG | tr '\n' ' ' | cut -c1-260)"
# --- B3-Gegenprobe: hin- UND zuruecktauschen (ABA) - geprueft wurde die Kopie, unter dem Pfad liegt am Ende dieselbe Datei
cp -f "$REF" $W/apk/aba.apk; cp -f $W/apk/K0.apk $W/apk/aba_k0.apk
LOG=$W/logs/nachher_kette_aba.log
( bash release/build_android.sh --gate-only $W/apk/aba.apk --version v0.8.19 > $LOG 2>&1; echo "EXIT=$?" >> $LOG ) &
pid=$!
getauscht=0
for i in $(seq 1 1500); do
  if [[ $getauscht == 0 ]] && grep -q "Volle Asset-Pruefung 1/2" $LOG 2>/dev/null; then
      mv -f $W/apk/aba.apk $W/apk/aba_ref.apk; cp -f $W/apk/aba_k0.apk $W/apk/aba.apk; getauscht=1; echo "ABA $(date +%T): hin (K0)"
  fi
  if [[ $getauscht == 1 ]] && grep -q "Volle Asset-Pruefung 2/2" $LOG 2>/dev/null; then
      mv -f $W/apk/aba_ref.apk $W/apk/aba.apk; getauscht=2; echo "ABA $(date +%T): zurueck (Referenz)"; break
  fi
  sleep 0.2
done
wait $pid
printf '%-28s %s | %s\n' "B3_ABA" "$(grep -E '^EXIT=' $LOG)" "$(grep -m1 -E '^ABBRUCH|ANDROID-GATES-OK' $LOG | cut -c1-200)"
# --- B2 im Schattenbaum (vorher_b2_b6_b8.sh) mit den NEUEN Skripten + Engine-Tabellen
S=$W/schatten/repo
mkdir -p $S/release $S/re15_port/include $S/re15_port/engine/src/gen
cp -f release/build_android.sh release/apk_pruefen.sh release/python_finden.sh release/apk_asset_gate.py release/apk_signer.sha256 $S/release/
cp -f re15_port/include/re15_door_seq.h $S/re15_port/include/
cp -f re15_port/engine/src/gen/re15_tuer_eigen.inc re15_port/engine/src/gen/re2_tuer_tabelle.inc re15_port/engine/src/gen/tuer_zuordnung.inc $S/re15_port/engine/src/gen/
kette schatten_ref "$B/$REF" "$B/$S"
mv $S/re15_port/shared_assets/RE15DOOR/P2DS.DO2 $W/schatten/P2DS.geparkt
signieren $W/apk/F_ohne_P2DS.apk $W/apk/F_ohne_P2DS_sig.apk && kette schatten_ohne_P2DS "$B/$W/apk/F_ohne_P2DS_sig.apk" "$B/$S"
grep -E "RE15DOOR:|Port-Tuerarchiv fehlt" $W/logs/nachher_kette_schatten_ohne_P2DS.log | head -3 | sed 's/^/      /'
mv $W/schatten/P2DS.geparkt $S/re15_port/shared_assets/RE15DOOR/P2DS.DO2
cp -p $S/re15_port/shared_assets/RE15DOOR/P07G.DO2 $W/schatten/P07G.geparkt; : > $S/re15_port/shared_assets/RE15DOOR/P07G.DO2
signieren $W/apk/F_P07G_leer.apk $W/apk/F_P07G_leer_sig.apk && kette schatten_P07G_leer "$B/$W/apk/F_P07G_leer_sig.apk" "$B/$S"
grep -E "passt nicht zur Engine-Tabelle" $W/logs/nachher_kette_schatten_P07G_leer.log | head -2 | sed 's/^/      /'
cp -p $W/schatten/P07G.geparkt $S/re15_port/shared_assets/RE15DOOR/P07G.DO2
mkdir -p $S/re15_port/shared_assets/RE15NEU; echo neu > $S/re15_port/shared_assets/RE15NEU/NEU.DAT
kette schatten_neuer_baum "$B/$REF" "$B/$S"
grep -E "in keinem Asset-Baum" $W/logs/nachher_kette_schatten_neuer_baum.log | head -1 | sed 's/^/      /'
rm -rf $S/re15_port/shared_assets/RE15NEU
ls -d /tmp/re15_apk_pruefen.* 2>/dev/null | wc -l | sed 's/^/uebrige Pruefkopien in \/tmp: /'
