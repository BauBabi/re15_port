#!/usr/bin/env bash
# adb NUR fuer die Emulator-Instanz dieses Pruefers (Port 5586); ohne MSYS-Pfadumschreibung.
export MSYS_NO_PATHCONV=1
exec C:/Users/mjoedicke/AppData/Local/Android/Sdk/platform-tools/adb.exe -s emulator-5586 "$@"
