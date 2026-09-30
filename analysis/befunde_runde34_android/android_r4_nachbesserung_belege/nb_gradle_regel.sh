#!/usr/bin/env bash
# Nachbesserung R4-1: Gradle-Regel (writeAssetManifest) negativ pruefen - eine Datei mit Kelvin-Zeichen im Namen im
# Quellbaum (re15_port/shared_assets/PSX/, NTFS haelt sie neben K.bin) muss den Bau abbrechen. Danach wird die Datei
# entfernt und writeAssetManifest erneut gefahren (Staging wieder = Quellbaum). Der Name entsteht nur in Python (die
# Konsole kann U+212A nicht ausgeben - erster Anlauf pflanzte deshalb nichts).
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PROJ=re15_port/platform/android
L=build/r34a/nb/logs; mkdir -p "$L"
export JAVA_HOME="C:/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot"
SDK="$(cygpath -m "$LOCALAPPDATA")/Android/Sdk"
export ANDROID_HOME="$SDK" ANDROID_SDK_ROOT="$SDK"
nicht_ascii() { /c/Python310/python -c "import os; print(sum(1 for n in os.listdir('re15_port/shared_assets/PSX') if any(ord(c) > 0x7e for c in n)))"; }
/c/Python310/python -c "import os; open(os.path.join('re15_port','shared_assets','PSX', chr(0x212A) + '.bin'),'wb').write(b'BBBBB')" || exit 1
echo "gepflanzt: $(nicht_ascii) Name(n) ausserhalb ASCII in re15_port/shared_assets/PSX/"
[[ "$(nicht_ascii)" == 1 ]] || { echo "Pflanzen fehlgeschlagen"; exit 1; }
rc=0
( cd "$PROJ" && bash ./gradlew --no-daemon --console=plain -Pre15Version=v0.8.20-nb1 writeAssetManifest ) > "$L/gradle_kelvin.log" 2>&1 || rc=$?
echo "Gradle writeAssetManifest mit Kelvin-Datei: EXIT=$rc"
grep -a -E "Asset-Pfad verletzt|BUILD (FAILED|SUCCESSFUL)|What went wrong" "$L/gradle_kelvin.log" | head -5 | cut -c1-200
/c/Python310/python -c "import os; os.remove(os.path.join('re15_port','shared_assets','PSX', chr(0x212A) + '.bin'))" || exit 1
echo "entfernt: $(nicht_ascii) Name(n) ausserhalb ASCII; git status shared_assets: $(git status --short re15_port/shared_assets | wc -l) Aenderungen"
rc2=0
( cd "$PROJ" && bash ./gradlew --no-daemon --console=plain -Pre15Version=v0.8.20-nb1 writeAssetManifest ) > "$L/gradle_danach.log" 2>&1 || rc2=$?
echo "Gradle writeAssetManifest danach: EXIT=$rc2"
grep -a -E "re15_assets.txt \(v2\)|BUILD (FAILED|SUCCESSFUL)" "$L/gradle_danach.log" | head -3
