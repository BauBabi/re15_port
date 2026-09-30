#!/usr/bin/env bash
# Nachbesserung R1: echter Android-Bau, der am Asset-Gate scheitern MUSS (Punktdatei im Quellbaum: Gradle
# nimmt sie ins Manifest, AGP laesst sie beim Packen weg - Bauer-Dossier 3.7), dazu eine "alte" APK unter dem
# Auslieferungsnamen. Beobachtet: (1) ist die alte APK schon weg, wenn Gradle startet? (echtlauf B4)
# (2) bleibt nach dem Gate-Abbruch eine .ungeprueft liegen? (echtlauf B3) (3) SHA256SUMS_android.txt unveraendert?
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$BAUM" || exit 99
V=v0.8.19-nbneg
ALT=release/re15_port_${V}_android.apk
PROBE=re15_port/shared_assets/RE15DOOR/.r34a_nb_probe.bin
LOG=build/r34a/nb/android_neg.log
BEOB=build/r34a/nb/android_neg_beobachtung.txt
sums_vorher=$(sha256sum release/SHA256SUMS_android.txt | cut -d' ' -f1)
cp build/r34a/ref_v0.8.19.apk "$ALT" && touch -d '2026-09-29 10:00:00' "$ALT"
printf 'r34a-nb Negativprobe\n' > "$PROBE"
echo "vorher: $(ls -la --time-style=+%T "$ALT" | awk '{print $5, $6, $7}') | Probe $(wc -c < "$PROBE") B" > "$BEOB"
( LOG="$LOG" bash analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh \
      bash release/build_android.sh --version "$V" --no-toolchain ) &
pid=$!
gesehen=0
while kill -0 "$pid" 2>/dev/null; do
    if [[ $gesehen -eq 0 ]] && grep -q "== Gradle: fetchSdl2" "$LOG" 2>/dev/null; then
        if [[ -e "$ALT" ]]; then echo "bei '== Gradle:' ($(date +%T)): alte APK LIEGT NOCH da" >> "$BEOB"
        else echo "bei '== Gradle:' ($(date +%T)): alte APK schon entfernt" >> "$BEOB"; fi
        gesehen=1
    fi
    sleep 0.5
done
wait "$pid"; rc=$?
{
  echo "build_android.sh EXIT=$rc (Log $LOG: $(grep '^[0-9.]* EXIT=' "$LOG" | tail -1))"
  echo "danach: $ALT $( [[ -e "$ALT" ]] && echo VORHANDEN || echo fehlt ); $ALT.ungeprueft $( [[ -e "$ALT.ungeprueft" ]] && echo VORHANDEN || echo fehlt )"
  echo "release/*.ungeprueft: $(ls release/*.ungeprueft 2>/dev/null | wc -l)"
  echo "SHA256SUMS_android.txt: $( [[ "$(sha256sum release/SHA256SUMS_android.txt | cut -d' ' -f1)" == "$sums_vorher" ]] && echo unveraendert || echo GEAENDERT )"
  echo "Gate-Zeilen:"; grep -E "fehlt in der APK|Manifest nennt .*probe|ABBRUCH|Gate-Abbruch|re15_assets.txt:" "$LOG" | sed 's/^[0-9.]* /   /' | head -8
} >> "$BEOB"
rm -f "$PROBE"
echo "Probe entfernt; git status re15_port/: '$(git status --short re15_port/ | tr '\n' ' ')'" >> "$BEOB"
cat "$BEOB"
