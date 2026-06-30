---
name: re15-room-capture
description: Lädt im echten RE1.5 (MZD-Disc) über DuckStation autonom einen beliebigen Raum per Debug-Menü (Stage + Room) und sichert den Zustand als Savestate — gesteuert über einen virtuellen Gamepad (vgamepad/ViGEm), da Tastatur-Injektion vom Spiel gefiltert wird. Verwenden, wenn ein echter Spiel-/Raum-Zustand (RAM/VRAM) für byte-true RE oder zum Vergleich mit dem Port gebraucht wird, ohne dass der Nutzer selbst spielt.
---

# RE1.5 Raum-Capture (DuckStation + virtueller Gamepad)

Fährt das **Original-RE1.5** (MZD-Mod, identische Original-`PSX.EXE` → alle `0x80xxxxxx`-Adressen gültig), navigiert autonom ins Debug-Menü, lädt einen Raum aus einer wählbaren Stage und schreibt einen Savestate. **Verifiziert lauffähig:** 2026-06-28 (Boot-Treiber, Stage 1+2) + 2026-06-30 (Quickload-Navigation end-to-end — Räume steppen + Square-laden in Stage 1/2/3, Cross-Stage per Overlay-Fingerprint bewiesen, ~27-36 s/Lauf). Schwester-Skill: **re15-savestate-ghidra** (extrahiert Werte aus dem Savestate). Siehe auch Memory `reai-v2-duckstation-dynamic-re`.

## Voraussetzungen (auf dieser Maschine eingerichtet)

- **Disc:** `C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue`
- **DuckStation:** `C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe`
- **settings.ini** (`%LOCALAPPDATA%\DuckStation\settings.ini`): `[Main] SaveStateOnExit=true`, `ConfirmPowerOff=false`, `StartFullscreen=false`. `[Pad1]` zusätzlich an `SDL-0` gebunden: Cross=`SDL-0/A`, Square=`SDL-0/X`, Triangle=`SDL-0/Y`, Circle=`SDL-0/B`, Select=`SDL-0/Back`, Start=`SDL-0/Start`, DPad=`SDL-0/DPad*`.
- **Virtueller Gamepad:** Python `vgamepad` (ViGEmBus). Tastatur-Injektion ans Spiel wird gefiltert → Game-Input MUSS über den virtuellen Pad (SDL-0) laufen. DuckStation-**Hotkeys** (F10 Screenshot, F2 Savestate) sind separat, aber hier nicht nötig.
- **Python 3.10** mit `vgamepad` + `zstandard`.

## Werkzeuge (`scripts/`)

- **`re15_capture.py`** — EMPFOHLEN: Capture **+** Auto-Verifikation in einem. Fährt den Treiber, lädt danach sofort den Savestate, prüft Globals + **erkennt schwarze (zu früh gespeicherte) Frames** und sagt dir, `--postload` zu erhöhen. Exit≠0 bei Schwarzbild.
- **`re15_mzd_load_room.py`** — der reine vgamepad-Treiber (erstellt den Pad, startet DuckStation `-batch <cue>`, Boot→Debug-Menü→JUMP, graziöser Close → `SaveStateOnExit` → Kopie nach `--out`).
- **`re15_quickload.py`** — QUICK-LOAD eines Savestates via DuckStation `-statefile` (überspringt den ~64 s-Boot → ~20-40 s/Lauf). Navigiert das JUMP-Menü + Menü-Shot/Laden/Provoke. Für die Gegner-/AI-Capture-Iteration (s.u.).
- **`re15_ss.py`** — Decoder für manuelle Verifikation (geteilt mit re15-savestate-ghidra).

```bash
# Raum aus Stage 1 (2 Räume weiter als der Boot-Raum), mit Auto-Verify + PNG:
python scripts/re15_capture.py --right 2 --triangle 0 --out stage1.sav --png stage1.png
# Raum aus Stage 2 (eine Stage weiter); Cross-Stage lädt neues Overlay von CD
# → längere Settle-Zeit (sonst meldet das Tool "display is black -> raise --postload"):
python scripts/re15_capture.py --triangle 1 --postload 16 --out stage2.sav --png stage2.png
```

Parameter: `--right N` (N Räume innerhalb der Stage weiter), `--triangle M` (M Stages weiter), `--postload S` (Sekunden warten nach „Square/Laden"; **Cross-Stage-Jumps brauchen ≥16 s**, sonst Savestate mitten im Laden = schwarzer Frame), `--boot S` (Boot-Wartezeit, Default 64), `--out` (Zielpfad).

## Navigations-Sequenz (Timings verifiziert, vom Treiber gesteuert)

1. **Boot ~64 s** (Pad muss vor DuckStation existieren → Enumeration als SDL-0).
2. **Titel:** `DPad-Up ×3` (Cursor nach oben) + `Cross` (= New Game) → ~12 s.
3. `Cross` (Leon, Default) → ~3 s → `Cross` → ~18 s → in **ROOM1240** (Boot-Raum, Stage 1).
4. **`Select`(Back)** → „– DEBUG MENU –" (~1,5 s).
5. **`DPad-Down`** → JUMP-Zeile.
6. **`DPad-Right` ×N** = Raum innerhalb der Stage; **`Triangle`(Y) ×M** = Stage wechseln.
7. **`Square`(X)** = Raum **laden**.
8. Warten (`--postload`), dann **graziös schließen**: `taskkill /IM duckstation-qt-x64-ReleaseLTCG.exe` **OHNE `/F`** → WM_CLOSE → `SaveStateOnExit` schreibt `…\savestates\HASH-957757946319438E_resume.sav` (957757946319438E = MZD-Disc-Hash). Der Treiber wartet auf frische mtime und kopiert nach `--out`.

`/F` (Force) verliert den Savestate — niemals `/F` verwenden.

## Verifikation (PFLICHT — kein „sieht richtig aus")

`re15_capture.py` macht das automatisch. Manuell prüfen mit dem Decoder (`scripts/re15_ss.py`, geteilt mit re15-savestate-ghidra):

```bash
python scripts/re15_ss.py stage2.sav stage2.png   # Globals + Display-%-non-black + Framebuffer-PNG
```

Erwartung für einen sauber geladenen Gameplay-Raum: `blob ≈ 4 MB`, RAM-Base `0x31a62`, Player-HP plausibel (z.B. 100), `state=1` (idle), `fade=0`, und das PNG zeigt einen erkennbaren 3D-Raum (nicht Titel/schwarz). **Schwarzer Frame = zu früh gespeichert → `--postload` erhöhen und neu aufnehmen.**

## Demo-Artefakte

`stage_saves/mzd_demo/` enthält die beiden live erzeugten, verifizierten Captures: `mzd_stage1_room.sav/.png` (Stage-1-Gang, Warnschild/Gully) und `mzd_stage2_room.sav/.png` (Stage-2-Tunnel). Stage-Wechsel belegt: Overlay-Region @0x80100000 differiert zu 97 % zwischen den beiden (gleiche Stage über Räume = 0 %). `stage_saves/mzd_stage1_combat_death.sav/.png` = ein **Live-Combat-Frame** (7 aktive Zombies + getöteter Spieler), erzeugt über den Provoke-Modus.

## Gegner-/AI-Capture (Provoke, Menü-Shot, Quick-Load) — 2026-06-29

Für byte-true Gegner-KI braucht man einen Savestate mit **aktiven** Gegnern (idle ≠ approach/attack). Zusätzliche Treiber-Modi (verstehe die Felder mit `re15-savestate-ghidra/scripts/re15_enemy_state.py`):

- **`--jump`** (re15_mzd_load_room.py): erzwingt den Debug-JUMP auch bei `--right 0` (lädt den Default-JUMP-Raum).
- **`--menushot`**: stoppt IM JUMP-Menü OHNE zu laden → der Savestate fängt das Menü; dump das VRAM-PNG (`re15_ss.py`) und lies die angezeigte `JUMP <raum> <name>`-Zeile, um `--right`→Raum zu mappen. **Die JUMP-Liste enthält ALLE Räume.** **`--right` = nächster Listen-Eintrag = Raum-Nummer +1; `--left` = −1** (verifiziert 2026-06-30). ⚠️ Einzeldrücke kurz halten — **ein langer Hold löst Auto-Repeat aus und überspringt ans Listenende** (`STEP_HOLD` ≤0.08 s); jeden Schritt mit `--menushot` verifizieren.
- **`--provoke S`**: läuft nach dem Laden S Sekunden vorwärts (DPad-Up + Rotations-Sweeps), bis ein Gegner den Spieler erkennt und in seinen approach/attack-State geht (für Live-Combat-/Mid-Lunge-Frames).
- **`--advance`** (mit `--provoke`): tappt zusätzlich **Cross** je Schritt (Dialog skippen / eine angeschaute Tür öffnen / interagieren) — für die New-Game-Progression durch Räume. ⚠️ Random-Walk+Cross navigiert NICHT zuverlässig mehrere Räume tief (siehe Befund unten).

**Quick-Load-Workflow** (`re15_quickload.py`, ~27-36 s statt ~110 s — spart den 64 s-Boot). Die **dauerhafte Debug-Menü-Base ist eingecheckt** (`stage_saves/mzd_debugmenu.sav`) und ist der **Default** von `--state` → keine Neu-Erzeugung nötig. **VERIFIZIERT lauffähig 2026-06-30 (end-to-end: steppt Räume + lädt sie).**
```bash
# Default --state = stage_saves/mzd_debugmenu.sav (committet). Räume steppen + Menü lesen:
python scripts/re15_quickload.py --right N --menushot --out probe.sav      # N Räume rechts, nur Menü
python scripts/re15_quickload.py --left  N --menushot --out probe.sav      # N Räume links
python scripts/re15_quickload.py --triangle M --right N --menushot --out probe.sav   # Stage M weiter
# Raum WIRKLICH laden (Square) + optional provozieren:
python scripts/re15_quickload.py --right N --postload 10 --out room.sav
python scripts/re15_quickload.py --right N --provoke 14 --out room.sav
# Base neu erzeugen (falls je nötig, einmaliger 110 s-Boot):
python scripts/re15_mzd_load_room.py --jump --menushot --out stage_saves/mzd_debugmenu.sav
```
**Die Debug-Menü-Sequenz (Nutzer-bestätigt + getestet — der Treiber macht das):** man landet im DEBUG-Menü **ganz oben** → **Down** zu „JUMP" → **Links/Rechts** = vereinzelte KURZE Drücke wechseln den Raum (Raum-Nummer ±1) → **Square** = Raum laden. **Dreieck** wechselt im JUMP die **Stage**. ⚠️ **KRITISCH nach `-statefile`-Quickload:** ein blankes Rechts wird IGNORIERT — die Menü-Eingabe muss erst durch eine Navigations-Taste RE-AKTIVIERT werden. `re15_quickload.py` macht das automatisch (Up×2 → zur Spitze, Down → JUMP, *dann* steppen) = die frische Nav-Sequenz des Boot-Treibers.

**Befunde (verifiziert 2026-06-30):**
- Stepping läuft: r0=**124 OPENING** (=ROOM1240, der Boot-Raum = die Base), r1=125 LOBBY ER4-1, r2=126 SEWER PASSAGE, `--left 10`=114 SEWER EXIT, `--right 3`=100 BATH-LOCKERS. `--right`=Raum +1, `--left`=Raum −1, `--triangle`=Stage.
- Square-Load bestätigt (Stage 1): `--right 3` + Square lädt 100 BATH-LOCKERS (display 96 % non-black, Gameplay-State, freeze=0x0).
- **Cross-Stage-Load bewiesen:** je ein Raum in Stage 1/2/3 geladen (`--right 2` / `--triangle 1` / `--triangle 2`, alle display 85-98 % non-black). Overlay-Region @0x80100000 (0x20000 B) byte-Diff: Stage1↔Stage2 = **87.9 %**, Stage1↔Stage3 = 85.5 %, Stage2↔Stage3 = 88.2 % (= verschiedene Stage-Overlays); Stage1↔Base = **0.0 %** (gleiche Stage = Methoden-Sanity). Cross-Stage braucht `--postload ≥16 s` (Overlay lädt von CD; sonst Frame mitten im Laden — Stage-3-Test war noch state 4/fade 0xf0 bei postload 20, ggf. 24 s).
- Overlay-Diff selbst messen: `re15_ss.Ram(sav).bytes(0x80100000, 0x20000)` aus beiden Saves byte-vergleichen.
- ⚠️ **Die Debug-JUMP-Nummer ≠ der Port-ROOM####-Code** (114 = „SEWER EXIT", NICHT ROOM1140). Die Liste ist sequenziell mit RE-Raum-Namen.
- **Debug-JUMP spawnt sehr wohl Gegner (korrigiert 2026-06-30):** ein JUMP zu `114 SEWER EXIT` (= ROOM1140, Debug-Nr = Port-ROOM####/10: 124→ROOM1240, 114→ROOM1140) lud einen Gegner (1× Typ **0x40**, Spieler lebt). 124 OPENING ist nur leer (Start-Raum). **ABER:** der JUMP lädt eine **ANDERE SCD-Szenario-Variante** als die Port-First-Visit — ROOM1140 via JUMP = 1× 0x40, NICHT die 5 Briefing-Zombies (0x10/0x11/0x16) aus `test_room1140_spawn`/sub00-First-Visit.
- **Die 5-Zombie-Briefing ist die geskriptete New-Game-FIRST-VISIT-Sequenz** (≠ JUMP-Szenario): New Game → Intro-Cinematic (~30-50s, `aca3c & 0x100` Freeze, Input ignoriert) → **Elliot-Briefing-Dialog** (ROOM1170, Typ **0x47**, Hitbox 450/1530 live-bestätigt) → Spieler-Kontrolle → die Briefing-Zombies kommen **hinter Türen tiefer in der Progression** (Event-Spawn). `stage_saves/mzd_stage1_combat_death.sav` (7 Zombies, post-death) ist die einzige Ground-Truth dieser Szene. ⚠️ Random-`--provoke`(+`--advance`) erreicht sie NICHT (kommt nur bis Elliot/ROOM1170; die Türen-Navigation mehrere Räume tief ist nicht zuverlässig automatisierbar). Für einen player-alive 5-Zombie-Frame: entweder deterministische Routen-Navigation bauen, oder der Nutzer spielt bis zur Briefing.
- **HURT-Capture braucht Feuern:** `--provoke` drückt kein Square → bezeugt nie den HURT-State. Für ein HURT-Frame `--provoke`/`--advance` um einen Square-Tap (+ R1-aim) erweitern.

## Troubleshooting

- **Kein Input im Spiel:** Pad vor DuckStation erzeugen; ViGEmBus installiert? `python -c "import vgamepad"` muss laufen.
- **Schwarzer/halb geladener Frame:** `--postload` erhöhen (Cross-Stage 16–20 s).
- **Falscher Raum:** `--right`/`--triangle` anpassen; die echte Stage per re15-savestate-ghidra (Overlay-Fingerprint) prüfen, nicht raten.
- **ms-gamebar-Popup beim Pad-Anstecken:** `HKCU\Software\Classes\ms-gamebar*` shell\open\command = `systray.exe` (einmalig).
