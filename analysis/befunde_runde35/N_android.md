# Runde 35 Spur N "android" — Dossier

Baum: `.claude/worktrees/r35_android` (Zweig r35/android, Basis master 154a73c1).
Auftrag (AUFTRAG.md Z. 97-104, woertlich):

> - Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten.
> - Wenn ein Update eine Datei durch einen Ordner gleichen Namens ersetzt oder umgekehrt, bricht das
>   Entpacken sauber ab; erst der zweite Start stellt den Stand her.
> - Künftige Änderungen am Prüfskript müssen dessen Urteilslogik selbst sorgfältig mittesten.

Hinweis RE-Gate: Die drei Punkte betreffen ausschliesslich Port-Infrastruktur (Android-Startbild,
Geraete-Entpacker, Release-Pruefskript). Es gibt dafuer KEINE Original-Funktion in RE1.5/RE2 (PSX
hat weder Entpacker noch APK). Belege sind hier Messungen (Geometrie-Rechnung, Temp-Verzeichnis-Lauf,
Mutanten-Lauf des Gates) und Quelltext-Zeilen des Ports, keine @0x-Adressen. Jede Zahl im Code wird
mit ihrer Herleitung (Messung/Zeile) kommentiert.

Status: IN ARBEIT

## Punkt 1 — Fortschrittsanzeige auf sehr breiten Displays

### Messung vorher
(folgt)

## Punkt 2 — Datei<->Ordner-Konflikt im Update

### Messung vorher
(folgt)

## Punkt 3 — Urteilslogik des Pruefskripts selbst mittesten

### Messung vorher
(folgt)

## OFFEN
(folgt)
