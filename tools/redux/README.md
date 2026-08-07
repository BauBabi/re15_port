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

## DER SCHLÜSSEL: so springt das Original in einen Raum (byte-true, selbst disassembliert)

Der Debug-JUMP baut **keinen** Tür-Datensatz — er setzt drei Werte und überlässt den Rest dem
normalen Raumlader:

```
@0x80014a44/48   sb 1     -> 0x800b5359    Modus 1 = Raumwechsel
@0x80014a4c/50   sw zero  -> 0x800ac9a8    Datensatz-Zeiger = NULL
@0x80014a54/58   sb zero  -> 0x800bbe5c
```

Das Ziel steht in den Raum-Globals, die der JUMP-Ausführer direkt beschreibt:

| Adresse | Bedeutung |
|---|---|
| `0x800B0FE6` | Stage |
| `0x800B0FE2` | Index |

**Raumnummer = `(stage+1) << 8 | idx`** — Port-Raum-ID ist dieser Wert × 16.
Beleg: BRIEFING ROOM = Index 0x14 → 0x114 → Port-ROOM1140.
Damit ist **ROOM1030 = Stage 0, Index 0x03**.

Der Zugriff `lw 0x800ac9a8` @0x8001d874 dereferenziert den Zeiger anschließend (`lh 0(a0)` für die
Spielerposition) — bei NULL wird also von Adresse 0 gelesen. Ob der Lader das vorher abfängt, ist
noch **nicht** geklärt; das ist die erste Frage beim nächsten Versuch.

## Offen: jump_1030.lua lädt noch nicht

Das Skript erzeugt **keine** Ausgabe — nicht einmal die Zeile, die es beim ersten Frame-Aufruf
schreiben soll. Da `probe_boot.lua` mit identischem Aufruf einwandfrei läuft, liegt es am Inhalt,
nicht am Pfad (beide Ablageorte getestet) und nicht am Startbefehl. `DrawImguiFrame` wird also gar
nicht definiert ⇒ Ladefehler im Skript.

Bereits ausgeschlossen: fehlendes BOM, Ablageort, `io`-Zugriff auf oberster Ebene (wurde in den
Frame-Aufruf verlegt — half nicht).

**Nächster Schritt:** von `probe_boot.lua` ausgehend in kleinen Schritten erweitern, bis die
Ausgabe abbricht — dann ist die fehlerhafte Zeile eingekreist. Ein Lua-Ladefehler geht hier
kommentarlos verloren (kein Eintrag in stdout), deshalb ist Bisektion der einzige verlässliche Weg.

## Gelöst: der stille Lua-Ladefehler

`jump_1030.lua` erzeugte kommentarlos keine Ausgabe. Per Bisektion eingekreist
(`bisect_a.lua` = Helfer + Lesezugriffe, `bisect_b.lua` = zusätzlich Zustandsausgabe und ein
Schreibzugriff — **beide laufen**). Der Fehler lag in der Ablaufsteuerung der alten Fassung
(Phasen-Variable); die neue Fassung ist aus `bisect_b.lua` aufgebaut und läuft.

**Merksatz:** Ein Lua-Ladefehler geht hier *völlig still* verloren — kein Eintrag in stdout, keine
Datei, nichts. Bei „keine Ausgabe" also nicht am Aufruf zweifeln, sondern bisektieren.

Bestätigt außerdem: Lesen **und Schreiben** über `PCSX.getMemPtr()` funktioniert (`m[o] = v`).

## Nächste Hürde: der Sprung braucht Spielbetrieb

Erster Lauf des Sprungs bei Bild 900:

```
VOR:   Stage=0 Index=0x00   Modus=0   Spieler=(0,0,0)   aktive Gegner: 0
       -> Ziel gesetzt, Modus 1 ausgelöst
NACH:  Stage=0 Index=0x00   Modus=1   Spieler=(0,0,0)   aktive Gegner: 0
```

Spieler auf (0,0,0) und null Gegner heißt: Das Spiel steht noch im **Titel/Vorspann**. Der
Modus-1-Handler läuft dort nicht — mein Wert 1 steht unverbraucht noch im Byte, und die
Raum-Globals wurden zurückgesetzt.

**Nächster Schritt:** erst per Pad-Schreibzugriff ins Spiel (NEU SPIEL bestätigen), dann springen.
Kandidaten für das Pad-Wort: `0x800AC758` (roh, wird von `FUN_80030444` gelesen), daraus
`0x800AC760` (remappt, vom Debug-Menü gelesen) sowie `0x800AC768` (gehalten) / `0x800AC76C`
(Flanke). Der Titelbildschirm nimmt laut Portwissen jeden Face-Button (Maske 0x8f0 @0x80102c14).
Offen ist, ob `DrawImguiFrame` vor oder nach dem Pad-Einlesen des Spiels läuft — deshalb im ersten
Versuch alle vier Wörter über mehrere Bilder hinweg setzen und am Spielerzustand ablesen, ob es
gegriffen hat.

## Gelöst: Pad-Eingabe braucht einen Haltepunkt, keinen Frame-Haken

`pad_drive.lua` zeigt: Schreiben in die Pad-Wörter aus `DrawImguiFrame()` **greift** (die Werte
stehen drin), kommt aber **zu spät** — das Spiel überschreibt sie jedes Bild, bevor es sie benutzt;
nach der Treibphase standen sie wieder auf 0.

Richtig ist ein **Schreib-Haltepunkt**:
`PCSX.addBreakpoint(0x800AC76C, 'Write', 4, 'name', function() ... return false end)`
— der Rückgabewert `false` lässt den Emulator weiterlaufen. `pad_bp.lua` setzt ihn auf das
Flanken-Wort, das letzte der vier, die `FUN_80030444` schreibt (@0x8003057c, nach gehalten
@0x8003051c und remappt @0x80030564).

## ⛔ Aktueller Blocker: das Spiel führt Code aus, kommt aber nicht voran

**Wichtig, weil irreführend:** Der Emulator *sieht* aus, als liefe alles — und das Spiel führt
tatsächlich Code aus. Es macht nur keinen Fortschritt. Zwei Messungen, die zusammen erst das
richtige Bild ergeben:

*Lebendigkeitstest* (`alive.lua`, 1800 Bilder): 102 **verschiedene** Programmzähler-Werte. Klingt
nach normalem Betrieb — stammt aber fast vollständig aus der Boot-Phase bis Bild ~1500.

*Langzeittest* (`wait_title.lua`, 16.500 Bilder): ab Bild 1500 liegt **jede einzelne** der 33
Stichproben in derselben Zwei-Adressen-Schleife (`0x80062130` / `0x8006217c`), und das
Pad-Flankenwort wird **null mal** geschrieben. Kein Spieler, Modus bleibt 0.

Das ist eine **Verklemmung**: Die Schleife läuft, erreicht ihr Ziel aber nie.

```
80062118: lw   v0,0x800787dc     ; Zustandsvariable
80062120: slt  v0,v0,a0
80062124: beq  v0,zero,0x80062194 ; Ausgang
8006212c: addiu v1,zero,-1
80062130: lw   v0,16(sp)          ; Zähler auf dem Stapel
80062138: addiu v0,v0,-1
80062148: bne  v0,v1,0x8006217c   ; Wiederholung
```

Eingegrenzt: Die Funktion beginnt bei `0x80062108` und wird nur aus `0x80062050` und `0x80062074`
gerufen; `0x800787dc` wird ausschließlich von neun Instruktionen zwischen `0x80061ebc` und
`0x80062180` angefasst — ein geschlossenes Warte-Subsystem im PsyQ-Bibliotheksbereich (CD/libcd).

**Nicht das Disc-Image:** Mit `re15_save_final.cue` hängt es genauso, nur einen Schritt weiter in
derselben Schleife. `-loadiso` und `-iso` verhalten sich identisch. Und **DuckStation spielt beide
Discs problemlos** — es ist also spezifisch für PCSX-Redux' CD-Emulation bei diesem Spiel.

**Nächste Ansätze:** CD-Einstellungen in `AppData/Roaming/pcsx-redux/pcsx.json` prüfen (die Datei
stammt aus einer früheren Sitzung), `-fastboot` gegen `-no-fastboot`, ein anderes BIOS, und die
Aufrufer `0x80062050`/`0x80062074` samt `0x800787dc` auswerten, um zu sehen, *welche* Bedingung
nie eintritt.
