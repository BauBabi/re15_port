---
name: re15-room-capture
description: Lädt im echten RE1.5 (MZD-Disc) über DuckStation autonom einen beliebigen Raum per Debug-Menü (Stage + Room) und sichert den Zustand als Savestate — gesteuert über einen virtuellen Gamepad (vgamepad/ViGEm), da Tastatur-Injektion vom Spiel gefiltert wird. Verwenden, wenn ein echter Spiel-/Raum-Zustand (RAM/VRAM) für byte-true RE oder zum Vergleich mit dem Port gebraucht wird, ohne dass der Nutzer selbst spielt.
---

# RE1.5 Raum-Capture (DuckStation + virtueller Gamepad)

Fährt das **Original-RE1.5** (MZD-Mod, identische Original-`PSX.EXE` → alle `0x80xxxxxx`-Adressen gültig), navigiert autonom ins Debug-Menü, lädt einen Raum aus einer wählbaren Stage und schreibt einen Savestate. Verifiziert 2026-06-28 (2 Räume aus Stage 1 + Stage 2 live geladen). Schwester-Skill: **re15-savestate-ghidra** (extrahiert Werte aus dem Savestate). Siehe auch Memory `reai-v2-duckstation-dynamic-re`.

## Voraussetzungen (auf dieser Maschine eingerichtet)

- **Disc:** `C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue`
- **DuckStation:** `C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe`
- **settings.ini** (`%LOCALAPPDATA%\DuckStation\settings.ini`): `[Main] SaveStateOnExit=true`, `ConfirmPowerOff=false`, `StartFullscreen=false`. `[Pad1]` zusätzlich an `SDL-0` gebunden: Cross=`SDL-0/A`, Square=`SDL-0/X`, Triangle=`SDL-0/Y`, Circle=`SDL-0/B`, Select=`SDL-0/Back`, Start=`SDL-0/Start`, DPad=`SDL-0/DPad*`.
- **Virtueller Gamepad:** Python `vgamepad` (ViGEmBus). Tastatur-Injektion ans Spiel wird gefiltert → Game-Input MUSS über den virtuellen Pad (SDL-0) laufen. DuckStation-**Hotkeys** (F10 Screenshot, F2 Savestate) sind separat, aber hier nicht nötig.
- **Python 3.10** mit `vgamepad` + `zstandard`.

## Werkzeuge (`scripts/`)

- **`re15_capture.py`** — EMPFOHLEN: Capture **+** Auto-Verifikation in einem. Fährt den Treiber, lädt danach sofort den Savestate, prüft Globals + **erkennt schwarze (zu früh gespeicherte) Frames** und sagt dir, `--postload` zu erhöhen. Exit≠0 bei Schwarzbild.
- **`re15_mzd_load_room.py`** — der reine vgamepad-Treiber (erstellt den Pad, startet DuckStation `-batch <cue>`, Boot→Debug-Menü→JUMP, graziöser Close → `SaveStateOnExit` → Kopie nach `--out`).
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

`stage_saves/mzd_demo/` enthält die beiden live erzeugten, verifizierten Captures: `mzd_stage1_room.sav/.png` (Stage-1-Gang, Warnschild/Gully) und `mzd_stage2_room.sav/.png` (Stage-2-Tunnel). Stage-Wechsel belegt: Overlay-Region @0x80100000 differiert zu 97 % zwischen den beiden (gleiche Stage über Räume = 0 %).

## Troubleshooting

- **Kein Input im Spiel:** Pad vor DuckStation erzeugen; ViGEmBus installiert? `python -c "import vgamepad"` muss laufen.
- **Schwarzer/halb geladener Frame:** `--postload` erhöhen (Cross-Stage 16–20 s).
- **Falscher Raum:** `--right`/`--triangle` anpassen; die echte Stage per re15-savestate-ghidra (Overlay-Fingerprint) prüfen, nicht raten.
- **ms-gamebar-Popup beim Pad-Anstecken:** `HKCU\Software\Classes\ms-gamebar*` shell\open\command = `systray.exe` (einmalig).
