---
name: re15-pcsx-watchpoint
description: Lokalisiert die EXAKTE Instruktion, die eine PSX-RAM-Adresse schreibt (auch indirekte/DMA-Writes, die ein statischer Disasm-Scan verfehlt) — via PCSX-Redux + scriptbarem Lua-Memory-Write-Breakpoint, das Intro autonom über einen virtuellen Gamepad gefahren. Verwenden, wenn "wer setzt Adresse X / bit Y" statisch NICHT auffindbar ist (indirekter Store über ein Basis-Register, DMA, memcpy) und der literale PC-Beweis gebraucht wird. PCSX-Redux ist der EINZIGE lokale Emulator hier mit scriptbaren Watchpoints (DuckStation-Debugger ist GUI-only).
---

# RE1.5 PCSX-Redux Memory-Watchpoint (literale Setter-Instruktion finden)

**Zweck:** die byte-true Frage „**welche Instruktion schreibt Adresse X / setzt bit Y?**" beantworten,
wenn der statische Scan (alle sw/sh/sb, direkt + base-tracked, in EXE/DEBUG.BIN/STAGE*.BIN) sie NICHT
findet — weil der Store durch ein **Basis-Register** geht, das der Disassembler nicht auflöst (z.B. eine
Adresse aus einer Tabelle geladen), oder es ein **DMA**-Write ist. Ein adress-basierter Watchpoint
feuert für JEDEN Write auf die physische Adresse (CPU-Store UND DMA) und liefert den PC.

Bewährt 2026-07-21: der Fade-zu-schwarz-Setter `DAT_800aca3c & 0x10` war statisch **airtight
non-existent** (0 direkte Stores). Der Watchpoint fing ihn bei **`PC=0x8003fe74 sw v1,0(a3)`** — der
SET-Pfad des generischen Flag-Helfers `FUN_8003fdf8` (a3 = `bank_table[bank]` aus @0x80074664, deshalb
statisch unsichtbar). Ergebnis: aca3c = Flag-Bank 1, Fade = SCD-Opcode `Set(1, 27)`. Siehe Memory
[[reai-v2-global-ai-freeze]].

## Voraussetzungen (auf dieser Maschine)
- **PCSX-Redux**: `winget install -e --id GrumpyCoders.PCSX-Redux --accept-package-agreements --accept-source-agreements`
  → `%LOCALAPPDATA%\Microsoft\WinGet\Packages\GrumpyCoders.PCSX-Redux_*\pcsx-redux.exe`.
- **BIOS**: `SCPH1001.BIN` (aus DuckStation: `%LOCALAPPDATA%\DuckStation\bios\SCPH1001.BIN`) → nach `C:\tmp\scph1001.bin` kopieren.
- **Disc**: die MZD-`.bin` (`…\ePSXe2018\Biohazard 1.5 (MZD Mod)….bin`). ⚠️ **Pfad OHNE Leerzeichen/Klammern** — PCSX-Redux' Arg-Parser bricht sonst. Hardlink auf simplen Pfad:
  `New-Item -ItemType HardLink -Path C:\tmp\mzd.bin -Target "…\Biohazard 1.5 (MZD Mod)….bin"` (gleiches Laufwerk, kein 215MB-Copy) + eine `C:\tmp\mzd.cue` (`FILE "mzd.bin" BINARY / TRACK 01 MODE2/2352 / INDEX 01 00:00:00`).
- **vgamepad** (ViGEmBus) — wie in `re15-room-capture`.

## Ablauf (ein Kommando)
```bash
# Watch 0x800aca3c, flagge das Setzen von bit 0x10:
PCSX_WATCH_ADDR=0x800aca3c PCSX_WATCH_BITMASK=0x10 \
PCSX_WATCH_LOG=/c/workspace/git/reAi_v2/shots/pcsx_watch.log \
  python .claude/skills/re15-pcsx-watchpoint/scripts/pcsx_drive.py
# → shots/pcsx_watch.log:  "#N pc=8003fe74 … old=0 newval=0x10  <== SETS mask!"  +  ">>> SETTER: pc=8003fe74"
```
Der Treiber: erstellt den vgamepad → startet PCSX-Redux (`-interpreter -debugger -run -dofile watch_addr.lua`)
→ wartet auf Boot (~110s Interpreter) → fährt New Game (Up×3 + Cross ×3 + Advance-Crosses) → das Intro
läuft und schreibt die Adresse → das Lua loggt jeden Write mit PC. Läuft bis `>>> SETTER` oder `PCSX_MAX_SECS` (600).

## Lua-Watchpoint-API (das Wesentliche)
```lua
-- PCSX-Redux = LuaJIT (Lua 5.1): KEINE 5.3-Bit-Operatoren (& | >> <<) -> bit.band/bit.rshift/bit.bor!
local bp = PCSX.addBreakpoint(0x800aca3c, 'Write', 4, 'name', function(address, width, cause)
  local pc  = PCSX.getRegisters().pc                 -- der schreibende PC
  local rt  = bit.band(bit.rshift(instr,16),0x1f)    -- Store-Quell-Reg (sw/sh/sb: rt)
  local val = PCSX.getRegisters().GPR.r[rt]          -- geschriebener Wert (sw)
  local mem = PCSX.getMemPtr()                       -- uint8_t* auf 2MB RAM; Index = addr & 0x1fffff
end)
_G.__keep = bp   -- MUSS am Leben gehalten werden (GC entfernt sonst den Breakpoint!)
```
- Breakpoint braucht **`-interpreter -debugger`** (Dynarec hat keine akkuraten Breakpoints; Interpreter ~5x langsamer).
- `cause` unterscheidet CPU (`Lua Breakpoint`) vs `DMA channel N write` — DMA-Writes triggern den Breakpoint ebenfalls.
- Lua-Pad-Input (Alternative zum vgamepad, deterministisch): `PCSX.SIO0.slots[1].pads[1].setOverride(PCSX.CONSTS.PAD.BUTTON.CROSS)` (drücken) / `clearOverride(...)` (loslassen). Buttons: `UP=4, RIGHT=5, DOWN=6, LEFT=7, CROSS=14, CIRCLE=13, TRIANGLE=12, SQUARE=15, START=3, SELECT=0`.

## Fallen (teuer bezahlt)
1. **LuaJIT ≠ Lua 5.3**: `&`/`>>` = Parse-Error → das ganze Chunk lädt nicht → io.open läuft nie (kein Log). Immer `bit.band` etc.
2. **Disc-Pfad mit Leerzeichen/Klammern** bricht `-iso` still → Hardlink auf `C:\tmp\mzd.bin`.
3. **Alte PCSX-Redux-Instanz** hält die Log-Datei offen → `os.remove` crasht; einfach mit Mode `"w"` truncaten und vorher `taskkill //F //IM pcsx-redux.exe`.
4. **vgamepad NACH PCSX-Redux erstellt** → GLFW sieht ihn nicht. Immer zuerst den Pad, dann den Emulator.
5. **Adresse→RAM-Offset**: `addr & 0x1fffff` (KSEG0/KUSEG-Mirror), nicht `addr`.
6. Statik ist trotzdem ZUERST dran (billiger): erst alle Stores/Base-Tracking scannen; den Watchpoint nur, wenn statisch nichts (indirekt/DMA).

## In einen BELIEBIGEN Raum springen (2026-08-07 erarbeitet, funktioniert)

Damit ist jede dynamische Frage messbar, nicht nur Watchpoints. `drive_to_room.py` + `room_jump.lua`
fahren das Spiel autonom bis ins Debug-Menue und springen von dort in einen Raum.

### Die zwei Dinge, an denen ich stundenlang gescheitert bin

1. **SELECT oeffnet das Debug-Menue NUR im INTRO, nach der Spielerauswahl.** Vorher gedrueckt
   stoert es die Auswahl — der Spieler entsteht dann gar nicht. Also: warten, bis der Spieler
   existiert (Position != 0,0), DANN SELECT.
2. **„Neues Spiel" muss WIEDERHOLT werden**, nicht einmal gedrueckt. Die Zeitpunkte sind nicht
   verlaesslich; `pcsx_drive.py` macht es alle ~18 s genauso. Einmaliges Druecken schlaegt oft fehl
   und man wartet dann ewig auf einen Spieler, der nie kommt.

### Nachweis statt Vermutung
Ein **Exec-Haltepunkt auf `0x80014444`** (die Menuefunktion) beweist, dass das Menue offen ist.
Ohne den raet man — die Menue-Bytes zeigen sonst nur die eigenen Schreibwerte zurueck.

### Menue-Zustand (aus debug_menu_common.c, alle @0x-belegt)
| Adresse | Bedeutung |
|---|---|
| `0x800BBE5D` | Auswahlzeile (1 = JUMP) |
| `0x800BBE5E` | Stage |
| `0x800BBE5F + stage` | Raumindex je Stage |
| Halbwort `0x800AC762`, Bit `0x80` | Bestaetigen (`lhu` @0x80014a30, `andi 0x80` @0x80014a38) |

**Raumnummer = `(stage+1) << 8 | idx`**, Port-Raum-ID = dieser Wert x 16.
Beleg: BRIEFING ROOM = Index 0x14 -> 0x114 -> Port-ROOM1140. Liste: `re15_port/include/debug_jump_table.h`.

Der JUMP baut KEINEN Tuer-Datensatz: `sb 1 -> 0x800b5359` (Modus 1 = Raumwechsel) @0x80014a44/48,
`sw zero -> 0x800ac9a8` (Datensatz-Zeiger NULL) @0x80014a4c/50. Stage/Index wandern nach
`0x800B0FE6` / `0x800B0FE2` (@0x8001D644 / @0x8001D660).

### Weitere Fallen (jede kostete einen Fehlversuch)
7. **Der Frame-Haken heisst `function DrawImguiFrame()`** — `PCSX.nextTick()` feuert NICHT
   (haengt an AfterPollingCleanup).
8. **Pad-Woerter aus `DrawImguiFrame` zu schreiben kommt ZU SPAET** — das Spiel ueberschreibt sie
   jedes Bild vor der Nutzung. Immer per Schreib-Haltepunkt auf `0x800AC76C` einspeisen (das
   letzte der vier Woerter, die FUN_80030444 schreibt).
9. **`-no-ui` stuerzt ab** (Segfault). Braucht man nicht — Lua treibt alles, das Fenster darf
   offen stehen und muss KEINEN Fokus haben.
10. **Ein Lua-Ladefehler geht VOELLIG STILL verloren** — kein stdout, keine Datei, nichts. Bei
    „keine Ausgabe" nicht am Aufruf zweifeln, sondern das Skript bisektieren.
11. **`PCSX.GPU.takeScreenShot()`**: width/height/bpp sind CDATA -> `tonumber()`; `data.size` ist
    ein Feld, keine Methode.
12. **Kein `io`-Zugriff auf oberster Ebene** — Datei erst im Frame-Aufruf oeffnen.

### Entity-Tabelle im RAM (zum Mitschreiben)
Spieler `0x800aca54`; Gegner ab `0x800acc2c`, Schrittweite `0x1F4`
(`s0 = 0x800acc2c + slot*0x1F4` @0x800420f8-118). Felder: `+0x4/5/6` Zustandsbytes,
`+0x8` Typ, `+0x9` grid, `+0x34/0x38/0x3c` Position, `+0x94` Clip, `+0x95` Clip-Bild.
Belegt heisst: Wort `+0x0` ohne Bit 0x8000.

### Werkzeuge
- `drive_to_room.py` — bootet, wiederholt „Neues Spiel" bis der Spieler da ist, drueckt dann
  SELECT bis das Menue nachweislich offen ist.
- `room_jump.lua` — erkennt das Menue per Exec-Haltepunkt, traegt Stage/Index ein, bestaetigt per
  Pad-Haltepunkt und protokolliert danach die Entity-Tabelle Bild fuer Bild.

## Werkzeuge (`scripts/`)
- `watch_addr.lua` — der Watchpoint (env: `PCSX_WATCH_ADDR`/`_LOG`/`_BITMASK`).
- `pcsx_drive.py` — vgamepad + PCSX-Redux-Start + New-Game-Navigation + Log-Überwachung.

Schwester-Skills: `re15-room-capture` (DuckStation-Savestates), `re15-savestate-ghidra` (RAM aus Savestate),
`re15-psx-disasm` (der statische Scan, der ZUERST läuft). Memory [[reai-v2-global-ai-freeze]].

### Raumauswahl beim JUMP — die Kette (2026-08-07 disassembliert + live gemessen)

```
8001d630: addiu a1,a1,-16802     ; a1 = 0x800bbe5e  (Menue-STAGE)
8001d638: lhu   v0,0x800b0fe2    ; alter Raumindex
8001d63c: lbu   v1,0(a1)         ; Menue-Stage
8001d644: sh    v0,0x800b0fe6    ; <- bekommt den ALTEN Index = VORHERIGER Raum (nicht die Stage!)
8001d64c: addiu at,at,-16801     ; 0x800bbe5f
8001d650: addu  at,at,v1         ; + stage
8001d654: lbu   v0,0(at)         ; Menue-INDEX fuer diese Stage
8001d660: sh    v0,0x800b0fe2    ; <- neuer Raum
```

⚠ **`0x800B0FE6` ist der VORHERIGE Raum, nicht die Stage** — die Bezeichnung in
`debug_menu_common.c` ist an dieser Stelle irrefuehrend.

**Offen (Stand 2026-08-07):** Das Ziel liess sich noch nicht setzen. Weder aus `DrawImguiFrame`
noch aus dem Pad-Schreib-Haltepunkt geschrieben ueberlebt `0x800bbe5f + stage` bis zum Lesen —
beim JUMP stand dort reproduzierbar `0x17` (HELIPORT) statt der geschriebenen `0x03` (LOBBY).
Der Sprung selbst funktioniert (Modus 1 feuert, Raumwechsel messbar an Spielerposition und
Entity-Tabelle), er nimmt nur ein anderes Ziel.

**Naechster Ansatz:** einen **Exec-Haltepunkt auf `0x8001d654`** setzen (die Lese-Instruktion
selbst) und dort `0x800bbe5f + stage` unmittelbar vor dem `lbu` beschreiben — dann kann nichts
mehr dazwischenfunken. Alternativ direkt das Zielregister nach dem `lbu` setzen
(`PCSX.getRegisters().GPR.r[2]` = v0) oder gleich `0x800b0fe2` per Exec-Haltepunkt auf
`0x8001d664` ueberschreiben.

### ⛔ Erfolg messen, nicht vermuten: `pathcount.lua`

Ich hatte „Menue offen" gemeldet, weil ein Exec-Haltepunkt auf `0x80014444` **einmal** gefeuert
hatte. Das war falsch: Ein OFFENES Menue ruft seine Funktion **jedes Bild** auf, also hunderte
Male. Ein einzelner Treffer ist ein beilaeufiger Aufruf, kein offenes Menue.

`pathcount.lua` setzt Zaehler auf alle Kandidaten und zeigt, was wirklich laeuft. Messung vom
2026-08-07 (SELECT gedrueckt, nachdem der Spieler existierte):

| Haltepunkt | Treffer |
|---|---|
| `0x80014444` Menuefunktion | **1** |
| `0x80014a44` JUMP bestaetigt | 0 |
| `0x80014a50` JUMP Zeiger NULL | 0 |
| `0x8001d630` Raumwahl-Kette | 0 |
| `0x8001d660` neuer Raum | 0 |
| `0x800396fc` Raumlader | 1 |

Heisst: **Das Menue oeffnete sich nie**, und die beobachteten Raumwechsel waren der Intro-Verlauf.

**Abnahmekriterium fuer den naechsten Versuch:** Haltepunkt auf `0x80014444` muss im
DREISTELLIGEN Bereich zaehlen. Erst dann ist das Menue wirklich offen.

**Offene Frage:** wann genau SELECT wirkt. Der Nutzer sagt „im Intro nach der Spielerauswahl" —
das Intro hat aber mehrere Abschnitte (Vorspann-Film, ROOM1240-Pre-Intro, Helipad). Naechster
Schritt: SELECT ueber einen langen Zeitraum in kurzen Abstaenden druecken und mit `pathcount.lua`
mitzaehlen, ab welchem Moment die Menuefunktion pro Bild laeuft.

### Pad-API dieser Version (mit `padapi.lua` ausgelesen, 2026-08-07)

Alles vorhanden — die API ist NICHT das Problem:

```
PCSX.CONSTS.PAD.BUTTON = { SELECT=0, START=3, UP=4, RIGHT=5, DOWN=6, LEFT=7,
                           L2=8, R2=9, L1=10, R1=11, TRIANGLE=12, CIRCLE=13,
                           CROSS=14, SQUARE=15 }
PCSX.SIO0.slots[1].pads[1] = { setOverride, clearOverride, getButton, map, setAnalogMode }
```

### ⛔ UNGELOEST: SELECT oeffnet das Debug-Menue nicht

Der Nutzer sagt: **ab dem Zeitpunkt nach der Spielerauswahl kann man JEDERZEIT SELECT druecken.**
Der Zeitpunkt ist also nicht das Problem. Trotzdem:

| Versuch | Ergebnis |
|---|---|
| vgamepad-Knopf BACK, 12x gedrueckt | Menue-Aufrufe: 1 |
| Lua `setOverride(SELECT)`, nur bei Zustandswechsel, 182 Zyklen | Menue-Aufrufe: 1 |
| Lua `setOverride(SELECT)`, **jedes Bild** waehrend der Druckphase, 182 Zyklen | Menue-Aufrufe: 1 |

Immer genau **ein** Aufruf von `0x80014444` — und der kommt schon vor dem ersten SELECT.

**Zu pruefen beim naechsten Mal, in dieser Reihenfolge:**
1. **Kommt die Uebersteuerung ueberhaupt an?** Pad-Woerter mitlesen waehrend `setOverride` aktiv
   ist: `0x800AC758` (roh), `0x800AC760` (remappt), `0x800AC768` (gehalten), `0x800AC76C` (Flanke).
   SELECT ist Bit `0x100` im RAW-Wort. Zeigt keines der Woerter das Bit, erreicht die
   Uebersteuerung das Spiel nicht — dann ist der vgamepad-Weg der richtige, aber mit korrekt
   gemapptem Knopf (PCSX-Redux-Pad-Konfiguration pruefen!).
2. **Ist `0x80014444` ueberhaupt die richtige Adresse?** Sie hat weder `jal` noch Zeiger in
   PSX.EXE/DEBUG.BIN (selbst gescannt) — sie wird aus einem Overlay erreicht. Moeglicherweise ist
   die Menueschleife eine ANDERE Funktion, und 0x80014444 wird nur einmal zum Aufbau gerufen.
   Gegenprobe: Exec-Haltepunkte auf mehrere Adressen im Bereich 0x80014444-0x80014cb0 legen und
   sehen, welche pro Bild zaehlt.

### Pad-Eingabe: gemessen, was ankommt (`menu_probe.lua`)

**H1 beantwortet — die Uebersteuerung KOMMT AN.** Bei dauerhaft gehaltenem SELECT:
`roh=0100 remap=0100 held=0000 edge=0000`. Bit 0x100 = SELECT steht also im Rohwort `0x800AC758`
UND im remappten `0x800AC760`.

⚠ **Die FLANKE `0x800AC76C` bleibt bei Dauerdruck 0** — Menues reagieren aber auf die Flanke.
Deshalb TAKTEN: 8 Bilder druecken, 24 loslassen, und `setOverride`/`clearOverride` **in jedem
Bild** aufrufen (nur beim Zustandswechsel gesetzt wirkt gar nicht — das war ein Fehlversuch).

**Wirkung des Taktens, gemessen:**

| Eingabe | Adressen im Menue-Bereich, die feuern |
|---|---|
| Dauerdruck | 2 (`0x80014444`, `0x80014cb0`) |
| getaktet | **6** (`0x80014444`, `0x80014500`, `0x800145f4`, `0x80014698`, `0x80014b10`, `0x80014cb0`) |

Das Menue laeuft mit Takten also deutlich tiefer — `0x800145f4` ist die Hoch-Taste,
`0x80014698` die JUMP-Zeile, `0x80014b10` die Textausgabe.

**Weiterhin offen:** jede Adresse feuert genau EINMAL und danach nie wieder. Das Menue macht
also einen einzigen Durchlauf. Zu pruefen: (a) ob es sofort wieder verlassen wird — Austritt ist
Pad-Bit `0x40` @0x8001466C, das im remappten Wort evtl. mitgesetzt ist; (b) ob die FLANKE fuer
SELECT ueberhaupt je gesetzt wird (`edge` blieb in allen Messungen 0 — moeglicherweise filtert
der Remap SELECT aus dem Flankenwort heraus, dann muss man `0x800AC76C` per Haltepunkt selbst
setzen).
