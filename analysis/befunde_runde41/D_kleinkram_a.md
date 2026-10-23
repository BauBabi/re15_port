# Runde 41 — Spur D "kleinkram_a" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kleinkram_a` (Zweig `r41/kleinkram_a`, Basis master 116e8b0f). Begonnen 2026-10-23.
Wortlaut (AUFTRAG.md): (1) Lautstaerke-Option "alles lauter oder leiser"; (2) Feuerloescher ROOM1030 leichter
Blink-Effekt "existiert bereits in RE2, uebernehme den"; (3) Spieler stirbt nicht bei erneutem Spinnengift auf Danger.

## 0. Stand / Protokoll (neueste zuerst)
* 2026-10-23: Baum sauber (116e8b0f), AUFTRAG/VERTRAG/UEBERGABE-40 gelesen, Einstiegspunkte lokalisiert:
  Options-FSM `platform/pc/main.c` pc_run_config (CFG_TOP/PICKER/SOUND/EDIT/AI, Zeilen ~2543-2905), Audio
  `platform/pc/src/audio_pc.c` (audio_callback :416, SS_BASE_MVOL :384, s_audio_mono :282), Feuerloescher
  `engine/src/feuerloescher_r37.c`, Spinnen `engine/src/spinnen_r38.c`, Zustand `engine/src/zustand_r39.c`,
  Schaden `engine/src/re15_damage.c`.

## 1. Punkt 1 — Lautstaerke-Option
### 1.1 Messung vorher
(folgt)
### 1.2 RE-Belege
(folgt)
### 1.3 Umsetzung
(folgt)
### 1.4 Messung nachher
(folgt)

## 2. Punkt 2 — Feuerloescher 1030 Blink-Effekt
### 2.1 Messung vorher
(folgt)
### 2.2 RE-Belege (RE2)
(folgt)
### 2.3 Umsetzung
(folgt)
### 2.4 Messung nachher
(folgt)

## 3. Punkt 3 — Spinnengift bei Danger toedlich
### 3.1 Messung vorher
(folgt)
### 3.2 RE-Belege
(folgt)
### 3.3 Umsetzung
(folgt)
### 3.4 Messung nachher
(folgt)

## 4. Tests
(folgt)

## 5. OFFEN
(folgt)

## 6. Fuer den Nutzer
(folgt)
