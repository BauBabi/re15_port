---
name: reai-v2-duckstation-dynamic-re
description: "Dynamisches RE: DuckStation+virtueller Gamepad+Debug-Menü -> beliebigen Raum laden, Savestate ziehen, RAM/VRAM lesen, mit Ghidra fehlende Werte byte-true ermitteln"
metadata:
  type: reference
  originSessionId: 073aec74-f763-4c50-aa34-afe1225f60d0
---

Vollständige Pipeline (verifiziert 2026-06-28), um im echten RE1.5 jeden Raum zu laden, den Zustand zu speichern und damit + Ghidra ([[reai-v2-ghidra-pipeline]]) fehlende byte-true Werte zu ermitteln. Ergänzt [[reai-v2-byte-true-audit]].

## 0. Voraussetzungen (einmalig eingerichtet, MÜSSEN aktiv bleiben)
- **Disc**: `C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue` — fährt die **identische Original-RE1.5-PSX.EXE** (per Signatur belegt), also sind alle EXE-Adressen 0x80xxxxxx gültig.
- **DuckStation**: `C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe`. Start headless: `-batch <cue>`.
- **settings.ini** (`C:\Users\mjoedicke\AppData\Local\DuckStation\settings.ini`): `[Main] SaveStateOnExit=true`; `[Pad1]` Buttons zusätzlich auf `SDL-0` gebunden (Tastatur bleibt via ` & `): Cross=`SDL-0/A`, Square=`SDL-0/X`, Triangle=`SDL-0/Y`, Circle=`SDL-0/B`, Select=`SDL-0/Back`, Start=`SDL-0/Start`, DPad=`SDL-0/DPad*`.
- **Virtueller Gamepad**: Python `vgamepad` (ViGEmBus). `vg.VX360Gamepad()` lebendig halten -> DuckStation sieht ihn als `SDL-0` (umgeht den Filter gegen injizierte Tastatur). Buttons: `gp.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_{A,X,Y,B,BACK,START,DPAD_UP/DOWN/LEFT/RIGHT})`, `gp.update()`.
- **ms-gamebar-Popup** unterdrückt (sonst poppt es beim Pad-Anstecken auf Windows N): `HKCU\Software\Classes\ms-gamebar`/`ms-gamebarservices`/`ms-gamingoverlay` shell\open\command = `systray.exe`; `HKCU\Software\Microsoft\GameBar\UseNexusForGameBarEnabled=0`.

## 1. Zum gewünschten Raum navigieren (Debug-Menü)
Boot-Sequenz bis ROOM1240 (Pad lebendig halten, dann taps mit ~0.7s Gap):
`boot 62s` -> Titel: `DPad-Up x3` + `A` (=New Game) -> `11s` -> `A` (=Leon, default) -> `2.5s` -> `A` -> `16s` -> in ROOM1240.
**Debug-Menü** (Nutzer-bestätigt): aus ROOM1240 **`Select`** (=Back) drücken -> „– DEBUG MENU – / UTILITY MENU / JUMP <room> <scenario> / MEMORY VIEWER". Dann: **`Down`** -> JUMP; **`Links/Rechts`** = Raum innerhalb der Stage wechseln; **`Dreieck`(Y)** = Stage wechseln; **`Viereck`(X)** = Raum LADEN. (MEMORY VIEWER = Live-RAM-Inspektion im Spiel.)
**Werkzeug**: `scratchpad/load_room.py --right N --triangle M` macht boot->Leon->1240->Select->JUMP->Wechsel->laden->graceful close automatisch.

## 2. Savestate ziehen + extrahieren
- **Save**: GRAZILE Schließung (`taskkill /IM duckstation-qt-x64-ReleaseLTCG.exe` OHNE `/F` -> WM_CLOSE) -> SaveStateOnExit schreibt `…\DuckStation\savestates\HASH-957757946319438E_resume.sav`. (`/F` = Force verliert den Save.)
- **Format**: Header `DUCCS\0\0\0` + Name + zstd-Frame (Magic `0x28b52ffd`). Decompress -> ~4 MB.
- **RAM-Base im Decompress**: per Signatur `PSX.EXE[0x800:0x830]` im decomprimierten Blob suchen -> Fundstelle = RAM `0x80010000`, also `base = pos - 0x10000` (praktisch `0x31a62`). **Beliebiges RAM-Global lesen**: `raw[base + (addr - 0x80000000)]` (LE).
- **Framebuffer/VRAM**: bei `base + 0x200000`, 1024x512, PSX-16bpp: `R=(px&0x1f)<<3; G=((px>>5)&0x1f)<<3; B=((px>>10)&0x1f)<<3`. Der Display-Bereich des Spiels liegt im VRAM (Titel-Shots: x≈384..704).
- Python-Extraktor (zstandard): siehe wiederholt genutzte Snippets in `scratchpad/` (debugmenu.png, jumped_room.png, room1240b.png) — decompress -> Signatur -> u8/u16/u32 + PNG-Dump.

## 3. Mit Ghidra fehlende Werte byte-true ermitteln (der Kern)
Ghidra liefert den **statischen Mechanismus**, der Savestate den **dynamischen Wert**:
1. **Ghidra** ([[reai-v2-ghidra-pipeline]]): finde das DAT_/FUN_, das einen Wert liest/schreibt (Dispatch-Tabelle, Xrefs, Handler-Disasm). Bsp: `DAT_800aca3c` Bit 0x100 = BG-Renderer-Skip; gesetzt von FUN_800402a0 (Opcode 0x29).
2. **Raum laden** (Schritt 1) bis zum interessierenden Moment.
3. **Savestate ziehen** (Schritt 2) und das DAT_ LESEN — was wird der Wert WIRKLICH? Bsp 1240-Briefing: `DAT_800aca3c=0x110`, `DAT_800b0fe4=0` (=Cut 0 schwarz), `DAT_800b5568=0xf0` -> bestätigte die Freeze-Mechanik byte-true.
4. **Byte-true im Port** umsetzen, mit zitierter Adresse + dynamischem Beleg.
Damit lassen sich die offenen Audit-Lücken schließen, die rein statisch unklar sind (Laufzeit-Werte, STAGE2-6-Räume, riskante Fixes #6/#13/#14/#15) und die der Nutzer NICHT manuell bedienen will.

## Operationalisiert als /slash-Skills (2026-06-28, beide live verifiziert)
Diese Pipeline ist jetzt als zwei Skills im Repo (`.claude/skills/`) mit getesteten Scripts:
- **`re15-room-capture`** — `scripts/re15_mzd_load_room.py` (vgamepad-Treiber: Boot→Debug-Menü→JUMP `--right N --triangle M --postload S`→graceful close→Savestate). WICHTIG: Cross-Stage-Jump braucht `--postload ≥16` (sonst schwarzer Frame, mitten im CD-Overlay-Load gespeichert).
- **`re15-savestate-ghidra`** — `scripts/re15_ss.py` (Decoder: zstd→RAM-Base via PSX.EXE-Sig 0x31a62→Reads+VRAM-PNG), `re15_savestate_inspect.py` (labelt Stage/HP/State/Cam-Cut/Flags), `decomp_globals.py` (Ghidra-Postskript, benennt eine Adresse). Stage-ID = **Overlay-Fingerprint** CRC32 @0x80100000 (per-Stage resident, über Räume stabil; STAGE{n}.BIN-Match scheitert, Disc-Overlays komprimiert). Demo-Captures: `stage_saves/mzd_demo/` (Stage1 fp 0x96290818, Stage2 fp 0x1c0394d0).
- **Session-Lehre:** Diff-Kandidat `0x800b532c/532e` (1 in Stage1-Raum, 2 in Stage2) sah wie Stage-ID aus, war per Ghidra aber der **Kamera-Cut-Index** (`DAT_800b5330 = DAT_800b532e<<3`, indiziert Cam-Tabellen @0x8008f82c). → Diff-Wert nie ohne Ghidra-Beleg glauben.
