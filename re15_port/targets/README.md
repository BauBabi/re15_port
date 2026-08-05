# `targets/` — zusätzliche Bauziele

Dieses Verzeichnis enthält **eigenständige Build-Definitionen** neben dem bestehenden
PC-Build. Sie sind bewusst additiv: `re15_port/CMakeLists.txt` und alles darunter bleibt
unverändert, damit der funktionierende Windows-Build nicht angefasst wird.

| Ziel | Stand | Einstieg |
|---|---|---|
| [`steamdeck/`](steamdeck/README.md) | **läuft** — baut, startet, rendert | `docker build -t re15-deck re15_port/targets/steamdeck` |
| [`psx/`](psx/README.md) | **baut + linkt**, passt aber noch nicht in den Konsolen-RAM (262 KB zu viel) | `re15_port\targets\psx\build.cmd` |

## Warum getrennte Projekte

Beide Ziele brauchen ein anderes Build-Modell als der PC-Zweig:

- **PSX**: PSn00bSDK 0.24 bringt seine eigene CMake-Integration mit (Toolchain-Datei plus
  `psn00bsdk_add_executable`/`add_cd_image`). Das im Repo liegende `cmake/FindPSn00bSDK.cmake`
  erwartet das ältere flache Layout und findet 0.24 nicht. Statt das Find-Modul umzubauen,
  benutzt `targets/psx/` direkt das offizielle SDK-Modell.
- **Steam Deck**: Hier baut die **unveränderte** Basis — nötig ist nur die richtige
  *Umgebung* (Steam-Runtime-Container), damit das Binary zur Bibliotheksbasis von SteamOS
  passt. `targets/steamdeck/` liefert diese Umgebung plus Paketierung und Startskript.

## Änderungen an der Basis

Für das PSX-Ziel war **eine** Zeile in der Basis unvermeidbar
(`engine/src/game_step_common.c`: `int32_t` → `int` in einer lokalen Doppel-Deklaration der
Trig-Funktionen, passend zum Header). Auf dem PC sind beide Typen identisch, die Änderung ist
dort nachweislich folgenlos — PC-Build neu gebaut, 110/110 Tests grün. Begründung und Beleg:
[`psx/README.md`](psx/README.md) §3.2.

Alles Übrige liegt in diesem Verzeichnis:

```
targets/
├── psx/
│   ├── CMakeLists.txt        eigenständiges PSX-Projekt (SDK-0.24-Modell)
│   ├── build.cmd             Ein-Kommando-Build inkl. CD-Image + RAM-Prüfung
│   ├── compat/               Hosted-C-Ersatz (getenv/fopen/fprintf) für PSX
│   ├── assets/               4-Byte-Platzhalter für die noch offene SFX-Bank
│   ├── iso.xml, system.cnf   CD-Image-Beschreibung + Boot-Konfiguration
│   └── README.md             Stand, Blocker, Begründungen
└── steamdeck/
    ├── Dockerfile            Steam-Runtime-SDK + SDL2-Bauabhängigkeiten
    ├── build.sh              Build (Container oder direkt auf dem Deck)
    ├── package.sh            Verteilordner: Binary + Assets + Starter
    ├── re15-deck.sh          Startskript — DAS wird in Steam eingetragen
    └── README.md             Bauen, Paketieren, In-Steam-Eintragen, Steuerung
```

Die Bauordner (`*/build/`, `steamdeck/dist/`) sind Wegwerf-Artefakte und gehören nicht ins
Repository.
