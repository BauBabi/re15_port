#!/usr/bin/env bash
# pcsx_kill.sh — beendet ALLE PCSX-Redux-Prozesse und VERIFIZIERT es. Exit 1, wenn etwas überlebt.
#
# WARUM ES DAS GIBT: PCSX-Redux läuft unter ZWEI Prozessnamen —
#     pcsx-redux.exe    ~19 MB   nur der Starter
#     pcsx-redux.main  ~235 MB   der eigentliche Emulator
# Ich habe über viele Läufe hinweg nur `taskkill /IM pcsx-redux.exe` aufgerufen. Der Starter starb,
# der Emulator lief weiter, und mit jedem Lauf kam eine weitere Instanz dazu. Folgen: die Emulation
# kroch (der Nutzer hat es gemeldet, ich hatte es als "der Interpreter ist halt langsam" abgetan),
# und meine Prüfung "Instanzen: 0" war wertlos, weil sie denselben falschen Namen abfragte.
#
# Lehre, die hier festgeschrieben ist: eine Aufräum-Prüfung, die nur einen von zwei Prozessnamen
# kennt, bestätigt Sauberkeit, die es nicht gibt. Deshalb greift dieses Skript per Muster, nicht per
# Name, und meldet einen Fehlschlag statt stillschweigend weiterzumachen.
set -u

for _ in 1 2 3; do
  taskkill //F //IM pcsx-redux.exe  >/dev/null 2>&1
  taskkill //F //IM pcsx-redux.main >/dev/null 2>&1
  sleep 1
  if ! tasklist 2>/dev/null | grep -qi pcsx; then
    echo "[pcsx_kill] alle PCSX-Prozesse beendet"
    exit 0
  fi
done

echo "[pcsx_kill] FEHLGESCHLAGEN — es laufen noch Prozesse:" >&2
tasklist 2>/dev/null | grep -i pcsx >&2
exit 1
