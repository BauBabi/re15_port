# Attract-/Demo-Mode (FE-3.2)

All evidence is in. Compiling the dossier raw data.

**FE-3.2 ATTRACT-/DEMO-MODE — RE-ERGEBNIS: Der Preview-Build (MZD) HAT KEINEN Attract-Mode. Punkt ist SCHLIESSBAR (Schliessungs-Beleg unten vollständig).**

---

## 1) MECHANISMUS (vollständig disassembliert)

### 1.1 Wie der Titel überhaupt betreten wird — Modul-Tabelle + drei Eintrittspfade

Modul-Records (12 Bytes: file_id, task_entry, load_addr_ptr) in der EXE, Modul 0 = TITLE:
- `@0x80073bdc` = file_id **6** (TITLE.BIN), `@0x80073be0` = Task-Entry **0x80101f7c**, `@0x80073be4` → ptr `0x80011360` → Ladeadresse **0x80100000** (verifiziert: `read 0x80011360` = `0x80100000`).
- Lader: `FUN_800299a4(slot, modul)` @0x800299a4 (`FUN_80013b60(file_id, load_addr, 0)` @0x800299e8 + Thread-Registrierung `FUN_80029a98` @0x80029a04); `FUN_80029a28(modul)` @0x80029a28 = laden + **REPLACE** des laufenden Threads via `FUN_80029ba4` @0x80029a7c (CloseTh/ChangeTh, `RE_15_Quellcode_V2/FUN_80029ba4.c`).

Alle Aufrufstellen (vollständig, per `grep jal` über ghidra1_V2.txt — es gibt genau 4):
1. **Boot→Title**: Boot-Task `0x80021138` (Disclaimer-Stills, Datei 0x21/0x20 → 0x80100000 @0x800212ac-e8) endet mit `jal 0x80029a28` / `a0=0` @**0x80021318-1c**.
2. **Game→Title** (Game-Over/Quit): im Game-Task-Rumpf nach dem Frame-Yield: `lw 0x800aca38` @0x8001d1e8, `and` mit 0x40000000 @0x8001d1f0-f4; Bit GELÖSCHT → `FUN_80021eb4` @0x8001d200 + `jal 0x80029a28`/`a0=0` @**0x8001d208-0c**.
3. **Soft-Reset** in `main()`: `lhu 0x800ac760` (Pad-1 RAW held) @0x80020c90, `andi v0,0x90c` / `bne v0,v1(=0x90c)` @**0x80020c94-9c** — Kombination **L1(0x4)+R1(0x8)+SELECT(0x100)+START(0x800)** (libetc.h psyq-4.7-converted-full: PADn=1<<2, PADl=1<<3, PADk=1<<8, PADh=1<<11). Dann CloseTh Slots 0/1/2 (`FUN_80029b48`) @0x80020cf0-cfc, `FUN_800299a4(0,0)` @**0x80020d5c**.
4. `FUN_80039a30` @0x80039a44: `FUN_800299a4(2, stage+1)` = STAGE-Overlays (Slot 2) — nicht Title.

**Jeder** dieser Pfade startet den Title-Task frisch bei State 0.

### 1.2 Title-Task-FSM (TITLE.BIN @0x80100000, Task-Entry 0x80101f7c) — komplett

Haupt-Loop @0x80101f7c: `sb zero → 0x801026c4` (Main-State=0) @**0x80101f94**; dann Endlos-Schleife @0x80101f98-0x80101fc8: Dispatch `Tabelle[0x8010269c + state*4]` @0x80101fa4-b4, Flip `FUN_80029ac8(1)` @0x80101fbc-c0, `j 0x80101f98` @0x80101fc4. **Kein Zähler, kein Timer, kein anderer Ausgang.**

Main-State-Tabelle @**0x8010269c** (per `table`-Decode): `[0]=0x80101fe0, [1]=0x80102038, [2]=0x80102100, [3]=0x80102140`.

- **State 0 @0x80101fe0 — FMV QUEUEN**: `FUN_80029c60(0, 1, 0x80198000, 1)` @0x80101fe4-ff8 (Movie-Id 0 → `sb a0, 0x800afbb9` @0x80029cc4); **`DAT_800aca38 |= 0x8000`** @0x8010201c-20; State++ @0x80102024.
- **State 1 @0x80102038 — Title-Screen-Setup + Fade-in**: `jal 0x801026cc` @0x80102040 (Setup inkl. CORE-SE-Bank 0x11 `FUN_800440c4(0x11)` @0x80102704-08, s. analysis/title_fade_voice.md); Fade-in `FUN_800217b0(0x200,0xfc00,7,3)` @0x80102054-64; Reset 0x801026c6..cb=0; **`0x801026c5 = 1`** @0x801020c4-cc (Sub-State startet bei 1 = MENÜ SOFORT); State++ @0x801020e8-ec.
- **State 2 @0x80102100 — Sub-Dispatch**: `Tabelle[0x801026ac + sub*4]` @0x80102110-28. Sub-Tabelle @**0x801026ac**: `[0]=0x80102a8c, [1]=0x80102b00, [2]=0x801023d0, [3]=0x80102594, [4]=0x801025f0`.
- **State 3 @0x80102140 — Spielstart**: `FUN_80021634(2,0)` @0x8010214c + REPLACE mit Game-Task `FUN_80029ba4(0x8001c958)` @0x80102154-5c (0x8001c958 = In-Game-Haupttask, State-Byte 0x800b5359, Tabelle @0x8001069c).

Sub-States (alle Übergänge NUR pad-getriggert, Edge-Wort `DAT_800ac762`):
- **Sub 0 @0x80102a8c** („Press Button"): `pad&0x8f0` @0x80102aa0 → sub++ + SE `FUN_80045024(0x04060000,0)` @0x80102ac4-c8. **TOT**: Sub startet bei 1 (@0x801020cc), kein Writer setzt je 0 (alle `0x801026c5`-Writer: 0x801020cc(=1), 0x801021a0(++), 0x801022dc(=cursor+2), 0x80102ac0(++), 0x80102c44(=cursor+2) — vollständig gegrept).
- **Sub 1 @0x80102b00 — DAS MENÜ** (NEW GAME/LOAD GAME/OPTION): DOWN `andi 0x4000` @0x80102b10, UP `andi 0x1000` @0x80102b60 (Cursor 0x801026ca, je + Cursor-SE `FUN_80045024(0x04040000,0)`); Confirm `andi 0x8f0` @0x80102c14 (△○✕□+START) → Announcer-SE @0x80102c20-24, `0x801026c5 = cursor+2` @0x80102c3c-44, Blocking-Fade `jal 0x80102ccc` @0x80102c48; wenn ==2 (NEW GAME): REPLACE Player-Select `FUN_80029ba4(0x80101094)` @0x80102c94-9c. **Sonst KEIN weiterer Code-Pfad — kein Idle-Zweig, kein Zähler.**
- **Sub 2 @0x801023d0**: rudimentäres 2-Sprite-Charakter-Select (`DAT_800aca5c`=0/4 @0x801024c0/0x801024d0-d4) — **TOT** (NEW-GAME-Confirm ersetzt den Task, bevor Sub 2 je dispatcht; Pointer-Ref nur Tabelle[2], per Byte-Scan verifiziert).
- **Sub 3 @0x80102594 — LOAD GAME**: `FUN_80025c00(1,0x8018xxxx,0,0)` @0x801025a8; Rückgabe ≠0 → Main-State=1 @0x801025b4-bc (zurück zum Menü); ==0 → Main-State++ @0x801025d8-dc (→3 = Spielstart mit Ladedaten).
- **Sub 4 @0x801025f0 — OPTION**: `FUN_80029a98(1, 0x8002dde4)` @0x801025f4-604 (Config-Task in Slot 1) + Main-State=1 @0x80102608-10.
- Zusätzlich TOT (0 Referenzen, per Byte-Scan auf Pointer + `jal`-grep): 0x80102174 und 0x801021ac-0x801023c8 (ältere Inline-Variante von Confirm/Menü).

### 1.3 Was statt eines Attracts passiert: FMV bei JEDEM Title-Eintritt, danach Menü FÜR IMMER

`main()` (Entry ~0x80020ba8, Decompile `RE_15_Quellcode_V2/main.c`) prüft am Schleifenkopf: `lw 0x800aca38` @0x80020c28, `andi 0x8000` @0x80020c30, Bit löschen + **`jal FUN_80029cd8`** @0x80020c44 = STR-Player. Der liest die Movie-Id aus **0x800afbb9** @0x80029da0-a8, Movie-Descriptor-Tabelle @**0x80073c2c** (Stride 12; nur Eintrag 0 ist real; Einträge 1-3 Platzhalter-Wiederholungen), Stream-Loop mit **START-Skip**: `lhu DAT_800ac762` @0x80029e1c, `andi 0x800` @0x80029e24 → Flag, Exit am geraden Field @0x80029e34. Movie spielt **genau einmal** (Ende wenn `FUN_8002a630`≠0), keine Loop, kein Warten auf Input am Ende. Auf der Disc existiert **nur ein einziges STR**: `MOVIE/CAPCOM.STR` (info/Re1.5/PSX/MOVIE/ = 1 Datei) — es gibt kein Intro-/Demo-FMV, das ein Attract abspielen KÖNNTE, und keine Demo-Play-Daten.

Der **einzige** Setzer von `DAT_800aca38|0x8000` im gesamten Spiel ist Title-State 0 @0x8010201c (EXE-Decompiles + alle Overlay-Decompiles gegrept: 0 weitere Treffer). Ablauf sichtbar: Title-Task startet → 1 Frame State 0 → CAPCOM.STR (skippable) → Fade-in → Menü.

### 1.4 Negativ-Beleg „kein Timer" (der eigentliche Schliessungs-Beweis)

1. Kompletter TITLE.BIN (11832 B = 2958 Instruktionen) disassembliert; **alle** 4 Main-States + **alle** 5 Sub-States oben einzeln zitiert — jede Transition ist ein Pad-Edge-Test, keine einzige ist zeitgesteuert.
2. Scan über alle `slti/sltiu` des Overlays: keine Schwellwert-Vergleiche außer Geometrie/Draw (0x80-Winkel im Player-Select) und `sltiu v1,0x1f` @0x801028fc — das ist der **einzige** Per-Frame-Zähler des Titels: ein kosmetischer Helligkeits-Pulser (Zähler @0x80102946 zählt 0..0x3b, Brightness @0x80102944 ±2/Frame, Reset auf 0x80 bei 0x3c @0x80102918-38) — zyklisch, ohne jeden Seiteneffekt.
3. Title-Zustandsbytes 0x801026c4..cb: alle Reader/Writer enumeriert (grep über den Voll-Disasm) — kein Frame-Zähler darunter.
4. Residenter Frame-Code der EXE während des Titels: `main()`-Loop (Decompile vollständig gelesen) enthält nur Soft-Reset-Kombi, Pause-Debug (`0x800aca38&0x800000`), Fades (`FUN_80021a0c/80021880/8002137c`) und Thread-Scheduler `FUN_800298b0` (Sleep-Dekrement pro Task, kein Idle-Zähler). Pad-Reader `FUN_80030444` (Decompile): kein Idle-Zähler, nur Rapid-Fire-Block (DAT_80073e2c).
5. Während des Menüs läuft **nur Thread-Slot 0** (Title-Task); Slot 1 erst ab OPTION, Slot 2 erst in-game (`FUN_800297c8` initialisiert 3 Slots; Belegungen s.o.).

**Fazit: Nach Idle im Title-Menü passiert im Original NICHTS — für immer. Kein FMV-Loop, kein Demo-Play, kein Rücksprung. Der klassische „Attract" existiert in diesem Preview-Build nicht; das FMV läuft stattdessen VOR dem Menü, bei jedem Title-Eintritt (Boot, Game-Over→Title, Soft-Reset).**

---

## 2) PORT-STAND

- `re15_port/platform/pc/main.c:2186-2272` — CAPCOM.STR-Playback einmalig beim Boot (`g_gameflow.boot_movie`, One-Shot-Clear Zeile 2188, START-Skip vorhanden); `re15_port/include/re15_gameflow.h:33`, `engine/src/re15_gameflow.c:33`.
- `main.c:2272` (`re_title:`) — Death→Title springt HINTER den FMV-Block direkt ins Menü.
- `main.c:2345-2470` — Title-Menü-Loop: pad-getrieben, korrekt OHNE Idle-Timer. `re15_mc_title.c` ist Memory-Card-Blocktitel, für FE-3.2 irrelevant.
- Soft-Reset-Kombi (L1+R1+SELECT+START, Maske 0x90c): im Port **nicht vorhanden** (grep 0x90c über engine/include/platform = 0 Treffer).

## 3) PLAN (Schliessung + zwei belegte Nebenbefunde)

1. **FE-3.2 SCHLIESSEN**: „Kein Attract-Mode im Original" mit obigen Belegen dokumentieren. Im Port ist NICHTS zu implementieren — der Ist-Zustand (Menü wartet unbegrenzt) ist byte-true.
2. Nebenbefund A (Divergenz, eigener Punkt, nicht FE-3.2): Das Original spielt CAPCOM.STR bei **jedem** Title-Task-Eintritt neu (Beleg §1.1/1.3: Game→Title @0x8001d208 und Soft-Reset @0x80020d5c starten den Task bei State 0, der das FMV unbedingt queued). Der Port überspringt es bei Death→Title (`re_title` liegt hinter dem FMV-Block). Fix: FMV-Block hinter das `re_title:`-Label ziehen bzw. `boot_movie` bei `re15_gameflow_to_title()` wieder auf 1 setzen. Verifikation: DuckStation, sterben → Title: CAPCOM-Logo muss erneut kommen (Capture).
3. Nebenbefund B (fehlendes Feature, eigener Punkt): Soft-Reset L1+R1+SELECT+START → Title (@0x80020c90-9c, RAW-Held-Wort 0x800ac760) fehlt im Port komplett.

## 4) OFFEN (ehrlich)

- **Dynamische Idle-Soak nicht gefahren** (z.B. 15 min Title-Menü in DuckStation stehen lassen). Der statische Beweis ist vollständig (jeder Code-Pfad enumeriert), aber der finale Ritterschlag wäre der Soak-Test; nächster Schritt: `re15-room-capture`-Harness, Boot → Menü → 15 min warten → Capture, erwartet: Menü steht unverändert (nur der ±2-Pulser @0x80102944).
- Player-Select-Szene (0x80101094, TITLE.BIN 0x80100000-0x80101f74) wurde für Idle NICHT vollständig einzeln auditiert; der `slti/sltiu`-Scan über das Gesamt-Overlay fand dort aber ebenfalls keine Timer-Schwelle (nur 0x80-Winkel-/Draw-Vergleiche @0x801006a8/0x80100820/0x801012ac).
- Semantik von `FUN_80029c60`-Arg a1=1 (sh → 0x800afbba, Startsektor-Offset?) und der Movie-Tabellen-Felder @0x80073c2c im Detail nicht aufgelöst (für die Attract-Frage nicht nötig).
- Nebenbefund A (FMV-Replay nach Game-Over) ist statisch belegt, aber noch nicht am echten PSX-Flow gemessen (Death→Title-Capture steht aus).

Belegdateien: Voll-Disasm `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\e15d7177-43f5-413e-8f99-d5e5f369c973\scratchpad\title_full.asm`; Quellen `info/Re1.5/PSX/BIN/TITLE.BIN` (lädt @0x80100000, off=addr−0x80100000), `info/Re1.5/PSX.EXE`, `RE_15_Quellcode_V2/{main.c,FUN_80029ba4.c,FUN_80029cd8.c,FUN_80030444.c,FUN_800298b0.c}`, `analysis/title_fade_voice.md`, `info/Resident_Evil_und_Playstation_Information/psyq-4.7-converted-full/include/libetc.h:14-35`.
