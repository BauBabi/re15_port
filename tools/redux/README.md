# PCSX-Redux als Messgerät — funktionierender Aufbau

Dynamisches RE gegen den **Auslieferungsstand**, vollständig skriptgesteuert: Speicher lesen und
schreiben, Bild für Bild, **ohne Fensterfokus und ohne Tastatur-Injektion**.

## Warum nicht DuckStation

DuckStation lässt sich hier nicht fernsteuern. Der Fensterfokus ließ sich zwar erzwingen
(`AttachThreadInput` an den Vordergrund-Thread — `SetForegroundWindow` allein meldet nur Erfolg),
aber **kein einziger injizierter Tastendruck kommt an**: weder `SendKeys` noch `SendInput` mit
Roh-Scancodes, auch nicht nach einem echten Mausklick ins Renderfenster. Die Sitzung läuft über
RDP (`RDP-Tcp#0`); Emulatoren lesen die Tastatur über Raw Input, das verhält sich dort anders.
Zusätzlich sieht `ffmpeg -f gdigrab` DuckStations Bild nicht (D3D-Oberfläche), nur die Qt-Leiste.

## Aufbau

Binärdatei (WinGet-Paket, **versteckt** — bei der Suche `-Force` bzw. keine Tiefenbegrenzung
verwenden, sonst findet man sie nicht):

```
C:\Users\<user>\AppData\Local\Microsoft\WinGet\Packages\
  GrumpyCoders.PCSX-Redux_Microsoft.Winget.Source_8wekyb3d8bbwe\pcsx-redux.exe
```

`pcsx-redux.exe` ist nur der Starter, `pcsx-redux.main` die eigentliche Binärdatei. **Aus dem
Paketverzeichnis heraus starten** (`cd` dorthin), sonst fehlen die DLLs.

```bash
cd "$PAKETVERZEICHNIS"
./pcsx-redux.exe -stdout -run -loadiso "<disc.cue>" -dofile "<skript.lua>"
```

- `-no-ui` **stürzt ab** (Segfault). Braucht man aber nicht: Lua treibt alles, das Fenster darf
  offen stehen und muss keinen Fokus haben.
- BIOS ist in `pcsx.json` konfiguriert (`C:/tmp/scph1001.bin`); `-bios` ist optional.

## Der Frame-Haken

**`function DrawImguiFrame()`** — die GUI ruft es pro gezeichnetem Bild auf. Beleg: das Beispiel
`src/mips/psyqo/examples/pcsxlua/pcsxlua.lua` im mitgelieferten Quellbaum benutzt genau diesen
Namen.

⚠ **`PCSX.nextTick()` feuert hier NICHT** (es hängt an `AfterPollingCleanup`). Das kostete einen
Fehlversuch: Die Ausgabedatei wurde angelegt, blieb aber leer.

## Speicherzugriff

`PCSX.getMemPtr()` liefert einen Zeiger auf das Haupt-RAM. Adresse → Offset mit
`bit.band(addr, 0x1fffff)`, danach byteweise lesen (Little Endian). Schreiben geht genauso.

Weitere nützliche Funktionen (aus `src/core/pcsxffi.lua`): `PCSX.pauseEmulator()`,
`PCSX.resumeEmulator()`, `PCSX.createSaveState()`, `PCSX.loadSaveState(obj)`,
`PCSX.getRegisters()`, `PCSX.getMemoryAsFile()`, `PCSX.quit(code)`.

Eine Pad-API gibt es in dieser Version **nicht** — Eingaben stattdessen direkt in die
Pad-Wörter des Spiels schreiben: `0x800ac768` (gehalten) und `0x800ac76c` (Flanke).

## Verifizierter Lauf

`probe_boot.lua` gegen `Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue`:

```
Bilder: 900
RAM@0x80010000 = 27bdffe0      (= addiu sp,sp,-32, erste EXE-Instruktion)
Stub@0x80026e4c = 03e00008     (= jr ra -> ORIGINAL-Auslieferungsstand)
```

Die MZD-Disc trägt eine PSX.EXE, die **byte-identisch** zu `info/Re1.5/PSX.EXE` ist
(md5 `b55fdaa5b9ec0f84bdd8a6b1107ea0b0`) — also ein echter Auslieferungsstand, kein Save-Mod.
⚠ Nicht verwechseln mit `info/Re1.5/re15_save_final.bin`: das ist die **gepatchte** Disc.

## Nächster Schritt

Bis ROOM1030 kommen. Zwei Wege:
1. Pad-Wörter pro Bild schreiben und so durch Menüs/Räume steuern (Debug-Menü nutzen).
2. RAM + Register aus einem DuckStation-Savestate transplantieren
   (`.claude/skills/re15-savestate-ghidra/scripts/re15_ss.py` liest ihn, `PCSX.getRegisters()`
   setzt die CPU) — dann von einem beliebigen gespeicherten Punkt weiterfahren.

## Wege nach ROOM1030 — Stand der Erkundung

### Sackgasse: CPU-Transplantation aus einem DuckStation-Savestate
RAM allein reicht nicht — ohne PC/SP/RA landet die CPU im falschen Kontext. Die CPU-Sektion des
DuckStation-Savestates ist zwar auffindbar (längen-präfixierte ASCII-Marke `CPU` @Blob-Offset
0x30027, Nachbarn `System` 0x30000 / `Bus` 0x31a1b), aber das Registerfeld ließ sich darin nicht
identifizieren: eine Signatursuche über die ganze Sektion nach `r0 == 0` **und** plausiblem
Stapelzeiger in r29 (0x801f0000–0x80200000) **und** Rücksprungadresse in r31 (Codebereich) findet
**null** Treffer. Die Reihenfolge weicht also von der naheliegenden ab. Nicht weiterverfolgt —
Zustandsübertragung zwischen zwei Emulatoren ist ohnehin fragil (Verzögerungsschlitz, Cache).

### Das Debug-Menü des Originals (RAM-Adressen, aus debug_menu_common.c)
Menü-Funktion `@0x80014444`. **Kein einziger `jal` und kein 32-Bit-Zeiger** in PSX.EXE oder
DEBUG.BIN zeigt darauf (selbst gescannt) — es wird aus einem Overlay heraus erreicht. Die
Modus-Tabelle der Hauptschleife `@0x8001069c` (Leser `lbu 0x800b5359` @0x8001c994, Index−1)
enthält es ebenfalls nicht: [0..4] = 0x8001c9c8 / 0x8001ca98 / 0x8001cbb8 / 0x8001cc34 / 0x8001cc70.
Der Eintrittspunkt ist also noch offen.

Die Zustandsbytes sind dagegen bekannt und direkt beschreibbar:
| Adresse | Bedeutung |
|---|---|
| `0x800BBE5D` | Auswahlzeile 0..2 (1 = JUMP, 2 = MEMORY VIEWER) |
| `0x800BBE5E` | Stage-Index |
| `0x800BBE5F + stage` | Raumindex je Stage (Umbruch bei 0x31) |
| `0x800AC760` | das gelesene (remappte) Pad-Wort; 0x1000 hoch, 0x4000 runter, 0x2000 rechts, 0x40 zurück |

### Aussichtsreichster autonomer Weg (noch nicht gebaut)
Den Raumwechsel des Spiels selbst auslösen, statt zu navigieren: Der Tür-Handler `FUN_800430BC`
legt den Datensatzzeiger nach `0x800ac9a8` (`sw a0,-13912(at)` @0x800430c4) und setzt den
Modus `0x800b5359 = 1` (@0x800430d4). Baut man in freiem RAM einen Tür-Datensatz mit Ziel
ROOM1030, schreibt dessen Adresse nach `0x800ac9a8` und setzt das Modus-Byte, lädt die Engine den
Raum über ihren eigenen Lader. Dafür muss das Datensatz-Layout aus `op_door_aot_set` (scd_vm.c)
übernommen werden.

### Schnellster Weg insgesamt
Ein **PCSX-Redux-eigener Savestate** in ROOM1030. Den kann Lua direkt laden
(`PCSX.loadSaveState(obj)`, Datei über `Support.File`), und ab da läuft die Messung vollautomatisch.
