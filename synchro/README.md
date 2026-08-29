# Sprachaufnahmen (`synchro/`)

RE1.5 hat keine englische Sprachausgabe — alles hier ist eigene Produktion des Projekts.
Der Port bindet die Dateien **automatisch** ein: was hier nach der Regel unten liegt, spielt.
Es gibt **keine Liste, in die etwas eingetragen werden müsste.**

## Die Regel

```
synchro/STAGE<n>/room<RAUM-ID>/main<MESSAGE-ID>.wav
```

| Teil | Bedeutung | Format |
|---|---|---|
| `<n>` | Stage-Ordner, wie auf der CD (`STAGE1` … `STAGE6`) | Ziffer |
| `<RAUM-ID>` | die gepackte Raum-Id | **4 Stellen HEX**, z. B. `1150` |
| `<MESSAGE-ID>` | die Message, zu der die Zeile gehört | **2 Stellen DEZIMAL**, z. B. `07` |

Beispiel: die achte Zeile (Message-ID 7) in ROOM1150 →
`synchro/STAGE1/room1150/main07.wav`

Die Raum-Id ist dieselbe wie im Dateinamen des Raums: `ROOM1150.RDT` → `room1150`.
Der Stage-Ordner muss zur Raum-Id passen (`0x1150 >> 12 = 1` → `STAGE1`); ein `room2xxx`
gehört nach `STAGE2`. Groß-Hex wird zuerst gesucht, Klein-Hex als Rückfall — auf Linux/Deck
(case-sensitiv) ist beides in Ordnung, solange es innerhalb eines Ordners einheitlich ist.

**Message-IDs 0 … 63.** Höhere Nummern lädt der Port nicht (`VOICE_MAX_MSG`).

## Das Dateiformat

Angenommen werden **WAV** mit:

* PCM 8 / 16 / 24 / 32 bit, oder IEEE-Float 32 bit
* auch `WAVE_FORMAT_EXTENSIBLE` (das ist, was die meisten Schnittprogramme ausgeben)
* Mono oder Stereo (Stereo wird gemischt), beliebige Samplerate (wird auf 44100 Hz gerechnet)

Andere Formate (`.mp3`, ADPCM …) werden **nicht** geladen. Der Port schreibt in dem Fall
eine Zeile auf `stderr` — er verwirft sie nicht mehr stillschweigend.

Zu leise Aufnahmen hebt der Port beim Laden auf einen gemeinsamen Pegel an (Ziel ~45 %
Vollaussteuerung, Verstärkung höchstens 4×). Wer schon auf Pegel liefert, wird nicht angefasst.

## Welche Zeile ist welche Message?

Die Arbeitsliste erzeugt der Port selbst:

```bash
cmake --build re15_port/build --target probe_synchro_status
re15_port/build/tests/unit/probe_synchro_status.exe --texte
```

Sie zeigt je Raum, wie viele Messages er hat, welche schon vertont sind und welche fehlen —
mit `--texte` im Wortlaut, damit klar ist, was zu sprechen ist. `--alle` listet zusätzlich
die Räume ohne jede Aufnahme.

Stand 2026-08-29: **1229 Messages in 170 Räumen mit Text, 84 vertont** (ROOM11C0 und ROOM1240 vollständig).

## `unused/`

Wird vom Port **nicht** gelesen und **nicht** ins Paket kopiert — Ablage für Aufnahmen, die
(noch) nicht zugeordnet sind.

## Wachen

* `unit_synchro_stimme` (ctest) prüft die Regel gegen den Bestand: Namen, Stage-Zuordnung,
  Format — und dass eine Message in einem beliebigen Raum die Sprachausgabe auslöst.
  Eine Datei, die `main…` heißt und die Regel trotzdem verfehlt, lässt den Test **rot**
  werden, statt im Spiel stumm zu bleiben.
  Ein noch **nicht zugeordneter** Roh-Export (z. B. `MiniMax_2026-08-27_23_50_03_Ada.wav`)
  wird nur **gemeldet**, nicht als Fehler gewertet — Arbeit im Fluss soll den Paketbau
  nicht blockieren. Er spielt aber auch nicht: erst das Umbenennen auf `main<NN>.wav`
  bindet ihn ein.
* `release/make_package.sh` kopiert **alle** `STAGE*`-Ordner ins Paket und bricht ab, wenn
  die Anzahl der WAVs im Paket nicht der im Repo entspricht.

## Historie

Bis 2026-08-26 hing die Wiedergabe an zwei hartkodierten Raumlisten
(`re15_room_has_voice = { 0x1170 }`). Von 37 aufgenommenen Dateien waren dadurch nur 11
hörbar — room10D0, room1150 und room1240 waren stumm, obwohl korrekt abgelegt. Seitdem
entscheidet allein, ob die Datei da ist.
