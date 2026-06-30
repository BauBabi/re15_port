---
name: re15-psx-disasm
description: Disassembliert RE1.5 PSX.EXE- und STAGE*.BIN-Overlay-Funktionen DIREKT als MIPS R3000 (kein Ghidra-Setup nötig) und decodiert Pointer-/Dispatch-Tabellen — der "Weg C" für Code, der NICHT in den Decompile-Dumps (RE_15_Quellcode_V2 / RE_15_Quellcode_Overlays) liegt. Verwenden, wenn eine FUN_/LAB_-Adresse roh-Byte im Ghidra-Dump steht, eine Sprung-/Vtable byte-true aufgelöst werden muss, oder eine Funktion schnell zusammengefasst werden soll (State-Writes, Calls, Schwellen).
---

# RE1.5 PSX-Direkt-Disassembler (Weg C)

Die Decompile-Dumps sind primär (`ghidra-mapping`, `RE15_FUN_CATALOG.md`). **Aber:** nicht jede Funktion ist dort decompiliert — die generischen Humanoid-AI-Leaves (`0x8004f..`) z.B. lagen 2026-06-29 nur als **roh-Byte** in `ghidra1_V2.txt`. Dann ist der schnelle Weg, direkt aus dem Binary zu disassemblieren ("Weg C"), statt ein Ghidra-Headless aufzusetzen.

## Datei-Layout (Adresse → Binary → Datei-Offset)

| Adressbereich | Binary | Offset |
|---|---|---|
| `0x80010000`–`0x800effff` (EXE) | `info/Re1.5/PSX.EXE` | PS-X-EXE-Header: text @ Datei `0x800`, `t_addr` @Header `0x18` (meist `0x80010000`) → `off = 0x800 + addr − t_addr` |
| `0x80100000`–`0x80121fff` (Overlay) | `info/Re1.5/PSX/BIN/<stage>.BIN` | **RAW @`0x80100000`, KEIN `0x800`-Header** → `off = addr − 0x80100000` (Default-Stage `STAGE1.BIN`, sonst `--bin`) |

Das Script wählt das Binary automatisch nach Adressbereich.

## Befehle (`scripts/re15_disasm.py`)

```bash
S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $S dis   0x8004f100 60         # n Instruktionen disassemblieren (Globals/Calls annotiert)
python $S table 0x801217a0 24         # n Words als Pointer-Tabelle (taggt EXE/overlay/data)
python $S scan  0x8004f3a4            # Funktion zusammenfassen bis jr ra
python $S bytes 0x80121790 12         # roher Hex-Dump
python $S read  0x8006e5a0 22 --w 4   # TYPISIERTE Daten-Tabelle (22 u32 = Reach-Tabelle)
python $S read  0x8006e0d0 22 --w 2 --stride 4 --rows 0x17 --rowstride 0x58  # 2D-Tabelle
python $S dis   0x8011d6d4 30 --bin STAGE5.BIN
```

- **`dis`** — vollständiger R3000-Disassembler (lui/lw/sb/sh/sltiu/beq/bne/jal/jr/…), annotiert bekannte Globals (`g_entity 0x800ac784`, `playerX/Z`, `player.hit_react 0x800acae7`, `attack_workstruct 0x800b52c4`, …) und Call-Ziele (`arc_test`, `atan2_q12`, `resolve_attack`, `anim_set`, …). Die Maps `GLOBALS`/`CALLS` im Script erweitern, wenn neue Adressen bestätigt sind.
- **`table`** — liest Words als Funktionspointer; markiert `overlay` (`0x8010xxxx`) vs `EXE` (`0x800xxxxx`) vs `data/?`. Das Werkzeug für AI-Dispatch-/Action-Tabellen.
- **`scan`** — symbolischer Schnell-Überblick einer Funktion: `sb`-Writes auf `+0x4/+0x5/+0x6/+0x7` (FSM-State-Transitions), `jal`-Ziele, `sltiu`-Schwellen (z.B. Distanz-Gates), bis `jr ra`. Ideal, um den Transition-Graph mehrerer Leaves zu kartieren, ohne jede Zeile zu lesen.
- **`read`** — TYPISIERTES Daten-Array / Tabellen-Lesen (`--w 1|2|4`, `--signed`, `--stride N` = Byte-Schritt, `--rows R --rowstride S` = 2D-Tabelle `base + type*S + i*stride`). Der byte-true Dumper für **Daten-Tabellen** (Damage/Reach/Reaktions-Clip/Hitbox-Structs), wo `bytes` nur Roh-Hex gibt und `table` Pointer annimmt. **⚠️ WICHTIG (die Lehre, die das Tool erzwang, Phase 8.10): NIE Daten-Tabellen ad-hoc mit selbst-gerechnetem File-Offset dumpen** — die EXE-Abbildung ist `0x800 + addr − t_addr` (t_addr aus dem PS-X-EXE-Header @0x18, NICHT pauschal 0x80010000 annehmen). Ein falsches Mapping landet in der BIOS-Stub-Region und liefert **Garbage** (ein Workflow-Agent ist daran gescheitert + hat „Tabelle nicht lesbar" gemeldet, obwohl sie sauber da war). `read` nutzt denselben `load()`-Helfer wie `dis`/`table` → immer korrekt. Beispiele: `read 0x8006e0d0 22 --w 2 --stride 4 --rows 0x17 --rowstride 0x58` (Player-Waffen-Damage `u16[type*0x58 + weapon*4]`), `read 0x8006f418 11 --w 2 --signed` (Enemy-Attack-Damage `s16[11]`).

## ⚠️ DECOMPILE-MISSTRAUEN — die wichtigste Lehre (2026-06-29)

**Ein Overlay-Decompilat (`RE_15_Quellcode_Overlays/STAGE*/FUN_*.c`) kann komplett FALSCH/fehl-analysiert sein.** Konkret: `STAGE1/FUN_80100424.c` zeigte einen Pose-Setter-Body — der echte `FUN_80100424` (Direkt-Disasm aus STAGE1.BIN) ist ein **Per-Frame-AI-Tick** (dist-cache + `@0x8011f7b4[+0x4]`-Dispatch). Der Decompiler hatte den Body einer ANDEREN Funktion (`FUN_8010ab2c`) drangehängt. Eine ganze Phase (8.3) wurde auf dieser Fiktion gebaut → revertiert.

**Regel:** Bevor du Code aus einem Overlay-`.c` portierst, der AI-/Dispatch-/State-Logik trägt, **disasm-verifiziere mindestens den Prolog + die Schlüssel-Konstanten** gegen die rohen Bytes (`dis <addr>`). Stimmt `dis <addr> 4` nicht mit dem `.c`-Anfang überein (z.B. `.c` liest sofort `+0x94`, aber `dis` zeigt dist-cache `SquareRoot0`+`@tabelle[+0x4]`) → dem `.c` NICHT trauen, aus den Bytes arbeiten. Live-RAM aus einem Savestate (`re15-savestate-ghidra`) ist der finale Schiedsrichter (Bytes @addr == on-disc?).

## ⚠ NIE DEN FILE-OFFSET SELBST RECHNEN (Overlay = KEIN 0x800-Header)

Die PSX.EXE hat einen **0x800-Byte-Header** (`off = 0x800 + addr − t_addr`). Die **STAGE*.BIN-Overlays haben KEINEN** (`off = addr − 0x80100000`). Wer ad-hoc `addr − 0x80100000 + 0x800` rechnet (den EXE-Header fälschlich annimmt), liest **0x800 Bytes zu weit = Garbage/Nullen** — und schließt dann fälschlich „die Tabelle ist leer / runtime-gepatcht".
**Belegt (2026-06-30):** die AI-Dispatch-Tabellen `@0x8011f840`(decide)/`@0x8011f890`(animate) wurden ad-hoc mit `+0x800` als „null" gelesen → falscher „runtime-patched"-Schluss. Mit korrektem Offset (`table 0x8011f840`) stehen sie **vollständig in STAGE1.BIN** und sind **40/40 identisch zum Savestate-RAM**: `0x80101b64`(search) `0x80102058`(engage) `0x80102548`(grab) `0x80102bd0`(walk) `0x80102dc8`(turn) `0x801048a8`(feeding) `0x8010561c`(0x13=engage+fw-precheck) = statische Overlay-Daten, NICHT gepatcht.

**Regel:** den Offset NIE selbst rechnen — `re15_disasm.py` (dis/table/read, nutzt `load()` = immer korrekt) bzw. `re15_ss.Ram` verwenden. `table <addr>` liefert viele `0x00000000` → ERST Adresse/`--bin`/Offset verdächtigen (das Tool warnt jetzt selbst), NICHT „runtime-patched" annehmen. **Cross-Check statisch↔RAM** (falls eine Tabelle WIRKLICH zur Laufzeit gebaut wird): `../re15-savestate-ghidra/scripts/re15_runtime_table.py <sav> 0x8011f840 16 --pair 0x8011f890` — Match ⇒ statisch, Diff ⇒ runtime-gebaut. Siehe [[reai-v2-re-pitfalls]].

## Dispatch-Tabellen-Familien decodieren (`table`)

Die STAGE1-Zombie-AI ist ein Geflecht paralleler Tabellen — pro Gegner-Typ eine eigene. So aufgelöst (alle byte-true 2026-06-29):
- **Per-Frame-Entity-Loop** = `FUN_8001a50c` (@0x8001ce04 im Main-Update), iteriert `DAT_800acc2c` (stride 0x1f4) und ruft `@0x80072bac[entity+0x8 type]` (die Per-Frame-Tick-Tabelle).
- **Typ 0x10/0x11/0x16** (Live-Briefing/Combat-Zombies): `@0x80072bac[type]=FUN_80100424` → `@0x8011f7b4[+0x4]` (state) → active `FUN_80101224`; dessen un-armed Tail → `@0x8011f80c[+0x9&0xf]` (sub-mode), und **`@0x8011f840 == &@0x8011f80c[13]`** (das +0x5-Decision-Brain `FUN_80101b64/de4/2058`).
- **Typ 0x47** (anderer Gegner): `@0x80072bac[0x47]=FUN_8011d6d4` → `@0x801217a0[+0x4]` → `FUN_8011d9f4`/`da48`. **PARALLEL, NICHT der Live-Pfad** — leicht zu verwechseln (gleiche Struktur, andere Tabelle).

Rezept: `table <base> 16` für jede vermutete Vtable; den Per-Frame-Caller über die XREFs der Tabelle im `ghidra1_V2.txt` finden (`grep 80072bac ghidra1_V2.txt` → `XREF` = die Loop); NIE annehmen, dass zwei Typen dieselbe Tabelle teilen, ohne das Savestate-`+0x8`-Typbyte + den `@0x80072bac[type]`-Eintrag zu lesen.

## Großes interconnected Subsystem RE'en (Workflow-Fanout) — bewährt 8.10

Für ein großes, verschachteltes Subsystem (z.B. die Player-Fire-Pipeline: Aim/Fire-FSM + Fire→Damage-Pfad + Port-Seite; oder die Forward-Walk-Kette) lohnt ein **Workflow mit parallelen RE-Agenten**, je ein Strang pro unabhängigem Teil (eine FSM / ein Daten-Pfad / die Port-Integrationsseite), die strukturierte Findings (cited address + confidence) zurückgeben. ABER (hart): **JEDEN Agenten-Befund selbst gegen die Bytes verifizieren** (`dis`/`read`/`table`) + gegen einen **Savestate** (`re15-savestate-ghidra`), bevor du portierst. Die Agenten dieser Session lagen oft richtig, aber: einer behauptete „Forward-Walk nur in Variante m1" (savestate-widerlegt: live=m0), einer scheiterte am EXE-File-Mapping und meldete eine Damage-Tabelle als „nicht lesbar" (sie war sauber — `read` löste es). → Workflow = schnelle Breite; die byte-true Verifikation ist NICHT optional. Siehe [[disasm-verify-decompiles]].

## Verifikation (so wurde es belegt)

- Pointer-Tabelle gegen die **Live-RAM** gegenprüfen (ist sie zur Laufzeit gepatcht?): mit `re15-savestate-ghidra` (`re15_ss.Ram`) dieselben Adressen aus einem Savestate lesen und mit der `table`-Ausgabe vergleichen.
- Prolog-Spot-Check für die Load-Adresse: `dis <fn> 4` muss ein plausibles `addiu sp,sp,-N; sw ra,..` zeigen.

## Verwandte Skills / Quellen

- `ghidra-mapping` + `RE15_FUN_CATALOG.md` — IMMER zuerst (vielleicht ist die Adresse schon kartiert).
- `re15-savestate-ghidra` — dynamische RAM-Werte + Tabellen-Patch-Check (Schwester-Methode).
- `scd-disassembly` — für SCD-Bytecode (nicht MIPS), eigene Opcode-Tabelle.
- Memory `reai-v2-foundation-combat` — die Zombie-AI-RE, die dieses Tooling erzwang.
