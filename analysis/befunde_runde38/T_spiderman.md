# Runde 38 — Spur T "spiderman" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_spiderman`, Zweig `r38/spiderman`, Basis e2cc7673. Angelegt 2026-10-16 16:22.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen"
2. "In Stage 4 die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner."

## Stand
- 16:22 Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.

## 0. Identifikation "Spiderman" (Zensus Stage-4-RDTs)
- Typ **0x2B** (Port: `re15_tyrant_ai_tick`, enemy_ai_common.c "TYRANT"). Namensbeleg: RE15Editor
  `info/.../RE15Editor/src/main/java/baubabi/reeditor/core/rdt/scd/ScdUtilities.java:230`
  `enemyMapForSelection.put("2B", "2B - SPIDER-MAN - Stage 4,5")`; Zeile 258: Zustand 65 (=grid 0x41)
  "not there (SPIDER-MAN)".
- Zensus (Werkzeug `T_belege/t_emset_zensus.py`, Sce_em_set 0x44 aller RDTs): Typ 0x2B steht in STAGE4
  NUR in **ROOM4001** (Elza; Leon-ROOM4000 hat an denselben Slots 0x29 "MONKEY"), sub00:
  - @0x0122C `44 02 2b 41 ...` slot 2 grid 0x41 (-8280,0,-16110) [Ck(4,20)==1 && Ck(3,0x91)==0], sonst
    @0x0126A grid 0x00 (-13880,0,-12560);
  - @0x012BE `44 03 2b 41 ...` slot 3 grid 0x41 (-20160,0,-11070) [Ck(3,0xAF)==0], sonst @0x012FC grid 0x00.
  - Schachtdeckel = Obj_model_set obj 1 @0x01240 (-8280,-630,-16110) / obj 2 @0x012D2 (-20160,-630,-11070).
  - Ausloeser: Aot_set slot 4/5 sce 3 (@0x01218/@0x012AA) -> sub05/sub07: `Set(3,0x91/0xAF)=1`,
    `Work_set(2,2|3) Member_set 0C=0x42` (@0x015BA/@0x016F0: grid := 0x42), Deckel obj 1/2 fliegt hoch
    (Speed_set/Add_speed ab @0x015CE/@0x01704) = "die Schaechte gehen korrekt hoch".
  - STAGE5: 0x2B in 5040/5041 + 5070/5071 (grid 0x41 bzw. 0), 5110/5111 + 5130/5131 (grid 0) — Leon UND Elza.
    (Das Debug-Menue zaehlt Stages ab 0: "Stage 4" dort = Spiel-STAGE5, debug_jump_table.h:79.)
- Registrierung STAGE4.BIN (Werkzeug `T_belege/t_hook_scan.py`): `sw` @0x801183F8 -> 0x80072C58 = Typ 0x2B
  Root 0x801118D0; Spieler-Hook A (cmd 5) @0x801184E8 -> 0x800AC804 = **0x80115CC0**; Hook B (cmd 6)
  @0x80118574 -> 0x800AC904 = **0x801161DC**. STAGE5: Root 0x80111A50, Hook A 0x80115E40, Hook B 0x8011635C
  (Kopie +0x180).

## 1. Punkt 1 — Austritt aus dem Schacht
### Messung vorher
### RE-Belege
### Umsetzung
### Messung nachher

## 2. Punkt 2 — Spieler verschwindet von der Karte
### Messung vorher (+ Bisect)
### RE-Belege
### Zensus: alle Stellen, die die Spielerposition schreiben
### Umsetzung
### Messung nachher

## 3. Tests

## OFFEN

## Fuer den Nutzer
