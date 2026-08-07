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
