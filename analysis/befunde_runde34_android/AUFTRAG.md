# Runde 34 (Android-Gate, Sitzung reai-v2-4b, 2026-09-29) — Auftrag des Nutzers, wörtlich

> Gehe parallel den noch offenen Punkt an.

Der offene Punkt (Schlussmeldung v0.8.19): "Der Android-Bau prüft bisher nur eine der 30 neuen
Türdateien automatisch. Diesmal habe ich alle 30 in der APK von Hand nachgemessen; die volle
Prüfung baue ich beim nächsten Paket ein."

Parallel arbeitet die Sitzung reai-v2-b5 an den Granaten (analysis/befunde_runde34_granaten/,
master a358fd5d) — informiert; dieser Zweig fasst nur Release-Werkzeuge an.

## Bestand (gelesen, master a358fd5d)

- `re15_port/platform/android/app/build.gradle` stageAssets (:100-120): spiegelt shared_assets/PSX,
  extracted_fx, RE2, RE15DOOR und synchro/STAGE*/** nach app/build/re15_assets/; prüft im Quellbaum
  nur Stichproben (u.a. `shared_assets/RE15DOOR/P07G.DO2`). writeAssetManifest (:125-142) schreibt
  re15_assets.txt ("<bytes>\t<pfad>"), nach dem die App auf dem Gerät entpackt.
- `release/build_android.sh` Gates (:211-240): 10 Stichproben-Einträge, Zahl der Asset-Einträge,
  Warnung bei komprimierten Assets, aapt badging. KEIN Vergleich Datei für Datei.
- `release/make_package.sh` (:195-219): RE15DOOR vollständig (vorhanden, nicht leer, cmp);
  RE2/DOOR nur vorhanden + nicht leer (kein cmp); TORSE.VBS nur vorhanden. :413 ruft `python3`
  (unter Git-Bash = WindowsApps-Alias -> v0.8.17 installierte ungefragt Python 3.14).
