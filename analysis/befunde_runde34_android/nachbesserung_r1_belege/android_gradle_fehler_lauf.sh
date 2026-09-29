#!/usr/bin/env bash
# Nachbesserung R1 (echtlauf B4, genau der Fall der Gegenpruefung): Gradle SCHEITERT - bleibt eine alte APK unter
# dem Auslieferungsnamen liegen? Gradle-Fehler wie im Bauer-Dossier 3.6: alle RE15DOOR/*.DO2 kurz geparkt ->
# stageAssets doFirst wirft "Tuerarchive fehlen im Repo". Danach zurueck + sha256-Abgleich.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$BAUM" || exit 99
V=v0.8.19-nbgradle; ALT=release/re15_port_${V}_android.apk; PARK=build/r34a/nb/park_re15door
LOG=build/r34a/nb/android_gradlefehler.log; BEOB=build/r34a/nb/android_gradlefehler_beobachtung.txt
( cd re15_port/shared_assets/RE15DOOR && sha256sum *.DO2 ) > build/r34a/nb/re15door_vorher.sha256
rm -rf "$PARK"; mkdir -p "$PARK"
zurueck() { mv "$PARK"/*.DO2 re15_port/shared_assets/RE15DOOR/ 2>/dev/null; }
trap zurueck EXIT
mv re15_port/shared_assets/RE15DOOR/*.DO2 "$PARK"/
cp build/r34a/ref_v0.8.19.apk "$ALT" && touch -d '2026-09-29 10:00:00' "$ALT"
echo "vorher: $ALT $(stat -c '%s B %y' "$ALT"); RE15DOOR/*.DO2 im Quellbaum: $(ls re15_port/shared_assets/RE15DOOR/*.DO2 2>/dev/null | wc -l)" > "$BEOB"
rc=0; LOG="$LOG" bash analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh \
      bash release/build_android.sh --version "$V" --no-toolchain || rc=$?
zurueck; trap - EXIT
{
  echo "build_android.sh EXIT=$rc"
  grep -E "Tuerarchive fehlen|stageAssets FAILED|ABBRUCH|BUILD FAILED" "$LOG" | sed 's/^[0-9.]* /   /' | head -5
  echo "danach: $ALT $( [[ -e "$ALT" ]] && echo 'LIEGT NOCH DA' || echo fehlt ); .ungeprueft: $(ls release/*.ungeprueft 2>/dev/null | wc -l)"
  echo "RE15DOOR zurueck: $( cd re15_port/shared_assets/RE15DOOR && sha256sum -c --quiet "$BAUM/build/r34a/nb/re15door_vorher.sha256" && echo '30/30 sha256 gleich' )"
  echo "git status re15_port/: '$(git status --short re15_port/ | tr '\n' ' ')'"
} >> "$BEOB"
cat "$BEOB"
