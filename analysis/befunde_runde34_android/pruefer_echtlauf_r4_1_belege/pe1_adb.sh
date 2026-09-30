#!/usr/bin/env bash
# adb fuer DIE Emulator-Instanz dieses Pruefers (Port 5580), ohne MSYS-Pfadumschreibung (/sdcard bliebe sonst nicht /sdcard)
export MSYS_NO_PATHCONV=1
exec C:/Users/mjoedicke/AppData/Local/Android/Sdk/platform-tools/adb.exe -s emulator-5580 "$@"
