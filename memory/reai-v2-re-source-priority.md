---
name: reai-v2-re-source-priority
description: Welche RE-Decompile-Quellen in reAi_v2 primär/fallback sind und wie Lücken zu schließen sind
metadata: 
  node_type: memory
  type: feedback
  originSessionId: 073aec74-f763-4c50-aa34-afe1225f60d0
---

Verbindliche RE-Quellen-Priorität in reAi_v2 (Nutzer-Vorgabe 2026-06-27):

- **PRIMÄR: Overlay-Decompilate** `RE_15_Quellcode_Overlays/` (RE1.5; `STAGE{1..6}_overlay.c`, `STAGE{N}/FUN_*.c`, `STAGE{N}_full/FUN_*.c`) bzw. `RE2_Quellcode_Overlays/` (RE2-Referenz). Hier ZUERST nachschauen — sie enthalten die zu portierende Spiel-/Raum-/AI-Logik (Overlay-Adressen `0x80100000+`).
- **FALLBACK: EXE-Decompilate** `RE_15_Quellcode_V2/` / `RE2_Quellcode_V2/` (Engine-Infrastruktur, `~0x8001xxxx`). Nur heranziehen, wenn die Logik nicht in den Overlays liegt.
- **Fehlt etwas in beiden → selbstständig ermitteln, niemals raten:** Ghidra headless auf `info/Re1.5/PSX/BIN/STAGE{1..6}.BIN` (@`0x80100000`) + `stage_saves/` (Laufzeit-/RAM-Verifikation).

**Warum:** Die per-Stage/per-Raum-Logik des Ports lebt in den Overlays; die EXE-Funktionen sind geteilte Infrastruktur. **Wie angewandt:** Diese Priorität steht jetzt in CLAUDE.md (RE-Quellen), RE15_FUN_CATALOG.md (Evidence-Header + Save-Sektion), design.md §8 (RE-Quellen-Priorität) und requirements.md (Glossar Ghidra_Overlay).

Wichtig (Adress→Dir-Mapping, nicht verwechseln): Overlay-Funktionen `0x801xxxx` werden aus `_Overlays` zitiert, EXE-Funktionen `0x8001xxxx` aus `_V2` — beide Citation-Stile sind korrekt und koexistieren (so machen es auch die `.agent_refs`-Berichte, die deshalb unangetastet blieben). Siehe [[reai-v2-build-recipe]], [[reai-v2-open-gaps]].
