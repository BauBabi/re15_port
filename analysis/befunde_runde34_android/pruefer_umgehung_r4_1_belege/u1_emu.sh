#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1, Abschnitt 6: Emulator (Nutzer-AVD Medium_Phone_API_36, headless) - NUR wenn kein anderer
# Emulator laeuft (sonst Rueckgabe 3, nichts angefasst).
#   1. u1_fold.sh in /sdcard/Download: faltet der App-Speicher Unicode (Kelvin, Ae/ae, NFC/NFD, sz/ss)?
#   2. FK_sig (N2 + Kelvin-Paar K.bin "AAAAA" / U+212A.bin "BBBBB", Liste passend) frisch installieren, starten,
#      Abschlusszeile des Entpackers abwarten, beide Dateien auf dem Geraet lesen, u1_fold.sh im App-Speicher,
#      Neustart (schneller Weg?), deinstallieren.
#   3. Emulator beenden (adb emu kill), warten bis kein qemu mehr laeuft.
# Aufruf: u1_emu.sh
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
export MSYS_NO_PATHCONV=1
SDK=C:/Users/mjoedicke/AppData/Local/Android/Sdk
ADB="$SDK/platform-tools/adb.exe"; EMU="$SDK/emulator/emulator.exe"
W=build/r34a/pruefer_u1; L=$W/logs/emu; B=analysis/befunde_runde34_android/pruefer_umgehung_r4_1_belege
PKG=de.re15.port; FILES=/sdcard/Android/data/$PKG/files
mkdir -p "$L"
if "$ADB" devices | grep -q -E "device$|offline$"; then echo "ABBRUCH: fremdes Geraet/Emulator aktiv:"; "$ADB" devices; exit 3; fi
if tasklist 2>/dev/null | grep -i -q -E "qemu-system|emulator.exe"; then echo "ABBRUCH: qemu/emulator-Prozess laeuft"; exit 3; fi
echo "[$(date +%T)] Emulator starten (Medium_Phone_API_36, -no-window -no-snapshot)"
"$EMU" -avd Medium_Phone_API_36 -no-window -no-audio -gpu swiftshader_indirect -no-snapshot > "$L/emulator.log" 2>&1 &
"$ADB" wait-for-device
t0=$(date +%s)
until [[ "$("$ADB" shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" == 1 ]]; do
    (( $(date +%s) - t0 < 400 )) || { echo "ABBRUCH: kein boot_completed nach 400 s"; "$ADB" emu kill; exit 4; }
    sleep 3
done
echo "[$(date +%T)] gebootet nach $(( $(date +%s) - t0 )) s: $("$ADB" shell getprop ro.build.version.release | tr -d '\r') API $("$ADB" shell getprop ro.build.version.sdk | tr -d '\r'), $("$ADB" shell getprop ro.product.cpu.abi | tr -d '\r')"
"$ADB" shell df -h /data | tail -1
"$ADB" shell settings put secure immersive_mode_confirmations confirmed
# 1. Faltung im gemeinsamen Speicher
tr -d '\r' < $B/u1_fold.sh > $W/u1_fold_lf.sh
"$ADB" push $W/u1_fold_lf.sh /data/local/tmp/u1_fold.sh > /dev/null
echo "== 1. u1_fold.sh in /sdcard/Download/u1_fold"
"$ADB" shell sh /data/local/tmp/u1_fold.sh /sdcard/Download/u1_fold | tr -d '\r' | tee "$L/fold_download.txt"
# 2. FK_sig
if [[ -n "$("$ADB" shell pm list packages $PKG | tr -d '\r')" ]]; then
    echo "== 2. uebersprungen: $PKG ist auf dem Nutzer-AVD installiert (nicht meins - nicht anfassen)"
else
    echo "== 2. FK_sig frisch installieren"
    t1=$(date +%s); "$ADB" install "$W/apk/FK_sig.apk" | tr -d '\r' | tail -1; echo "   install $(( $(date +%s) - t1 )) s"
    "$ADB" logcat -c
    "$ADB" shell am start -W -n $PKG/.RE15Activity | tr -d '\r' | grep -E "Status|TotalTime"
    timeout 600 "$ADB" logcat -v time -s re15:V -e "Entpacken fertig|Assets aktuell|FEHLER|ungueltig" -m 1 > /dev/null; echo "   warten rc=$?"
    "$ADB" logcat -d -v time -s re15:V | tr -d '\r' | grep -v "^------" > "$L/fk_erststart.logcat"; cat "$L/fk_erststart.logcat" | cut -c1-230
    cat > $W/u1_fk_lesen.sh <<'DEV'
D=/sdcard/Android/data/de.re15.port/files
P=$D/shared_assets/PSX
k=$(printf 'K.bin'); kel=$(printf '\342\204\252.bin'); kl=$(printf 'k.bin')
echo "Eintraege in PSX mit Namen <= 7 Zeichen + .bin:"; ls -la "$P" | grep -a -E ' .{1,7}\.bin$'
echo "cat K.bin     : $(cat "$P/$k" 2>&1)"
echo "cat U+212A.bin: $(cat "$P/$kel" 2>&1)"
echo "cat k.bin     : $(cat "$P/$kl" 2>&1)"
sha256sum "$P/$k" "$P/$kel" 2>&1
echo "Liste zuletzt entpackt, letzte 2 Zeilen:"; tail -2 "$D/re15_assets_entpackt.txt"
echo "Kopf: $(head -1 "$D/re15_assets_entpackt.txt")"
DEV
    "$ADB" push $W/u1_fk_lesen.sh /data/local/tmp/u1_fk_lesen.sh > /dev/null
    echo "   -- auf dem Geraet nach dem Erststart:"
    "$ADB" shell sh /data/local/tmp/u1_fk_lesen.sh | tr -d '\r' | tee "$L/fk_geraet_nach_erststart.txt" | sed 's/^/   /' | cut -c1-200
    echo "   -- u1_fold.sh im App-Speicher ($FILES/u1_fold):"
    "$ADB" shell sh /data/local/tmp/u1_fold.sh $FILES/u1_fold | tr -d '\r' | tee "$L/fold_appspeicher.txt" | sed 's/^/   /'
    echo "== 2b. Neustart (force-stop + start)"
    "$ADB" shell am force-stop $PKG; "$ADB" logcat -c
    "$ADB" shell am start -W -n $PKG/.RE15Activity | tr -d '\r' | grep -E "Status|TotalTime"
    timeout 600 "$ADB" logcat -v time -s re15:V -e "Entpacken fertig|Assets aktuell|FEHLER|ungueltig" -m 1 > /dev/null; echo "   warten rc=$?"
    "$ADB" logcat -d -v time -s re15:V | tr -d '\r' | grep -v "^------" > "$L/fk_neustart.logcat"; cut -c1-230 "$L/fk_neustart.logcat"
    "$ADB" shell sh /data/local/tmp/u1_fk_lesen.sh | tr -d '\r' | tee "$L/fk_geraet_nach_neustart.txt" | sed 's/^/   /' | cut -c1-200
    "$ADB" shell am force-stop $PKG
    echo "== 2c. deinstallieren"
    "$ADB" uninstall $PKG | tr -d '\r'
    echo "   Android/data mit re15: $("$ADB" shell ls /sdcard/Android/data 2>/dev/null | tr -d '\r' | grep -c re15)"
fi
"$ADB" shell rm -f /data/local/tmp/u1_fold.sh /data/local/tmp/u1_fk_lesen.sh
"$ADB" shell df -h /data | tail -1
echo "== 3. Emulator beenden"
"$ADB" emu kill
t2=$(date +%s)
while tasklist 2>/dev/null | grep -i -q -E "qemu-system|emulator.exe"; do (( $(date +%s) - t2 < 120 )) || { echo "qemu laeuft nach 120 s noch"; break; }; sleep 2; done
echo "[$(date +%T)] beendet; adb devices: $("$ADB" devices | tail -n +2 | tr -d '\r' | tr '\n' ' ')"
echo FERTIG
