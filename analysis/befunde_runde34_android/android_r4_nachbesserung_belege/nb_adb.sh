#!/usr/bin/env bash
# adb fuer DIE Emulator-Instanz dieser Nachbesserung (Port 5584), ohne MSYS-Pfadumschreibung
export MSYS_NO_PATHCONV=1
exec C:/Users/mjoedicke/AppData/Local/Android/Sdk/platform-tools/adb.exe -s emulator-5584 "$@"
