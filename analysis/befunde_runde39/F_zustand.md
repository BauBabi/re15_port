# Runde 39 — Spur F "zustand" (2026-10-18)

Baum `.claude/worktrees/r39_zustand`, Zweig `r39/zustand`, Basis c71fa144 (master nach Integration Welle 1).

## Auftrag (Wortlaut, AUFTRAG.md)
1. "Wenn der Spieler verletzt wurde, hat dann kaputte Klamotten und heilt sich aber wieder vollständig - zum Beispiel
   mit Heilspray - dann muss er auch wieder die ganzen Klamotten haben."
2. "Der "Poison" Zustand ist zwar jetzt da, aber er zieht keine Energie ab Stück für Stück, so wie im Originalen
   Resident Evil 2. Zumindest zeigt er es vom EKG her nicht so an. Da ist noch ein Fehler."

## Fortschritt (fortlaufend, neueste unten)
- 12:xx Baum geprueft (status leer, HEAD c71fa144), AUFTRAG/VERTRAG gelesen, Vorrunde R_spinnen.md (Gift) und
  Runde-36 B_kleidung.md (Kleidung) gelesen. Bau configure+build gruen (F_belege/build_0.log).
- Bestand gelesen: Gift-Takt = RE1.5-Form (spinnen_r38.c re15_spinnen_r38_gift_tick, -2 HP alle 121 Bilder,
  @0x80031e44-ecc), gerufen NUR im Kommando-1-Prolog (game_step_common.c:1712). EKG: RE1.5-Farbtabelle
  @0x8007622c, Eintrag 3 (Gift) = 20 ff 20 = GRUEN wie FINE. Kleidung = Blut-Decal-Wundtabelle (re15_damage.c
  s_wounds[8]) + VRAM-Blit in die Slot-0-Basis (render_pc.c re15_render_pc_wound_blit schreibt in
  s_slot0_base_rgba, also KEINE unversehrte Kopie mehr); Heilung loescht im Original nichts (blood_decals.md).

## 1. Punkt 1 — volle Heilung = ganze Kleidung
### 1.1 Messung vorher
(folgt)
### 1.2 RE-Belege
(folgt)
### 1.3 Umsetzung
(folgt)
### 1.4 Messung nachher
(folgt)

## 2. Punkt 2 — Gift zieht Energie wie RE2, EKG zeigt es
### 2.1 Messung vorher
(folgt)
### 2.2 RE-Belege
(folgt)
### 2.3 Umsetzung
(folgt)
### 2.4 Messung nachher
(folgt)

## 3. Tests
(folgt)

## 4. OFFEN
(folgt)

## 5. Fuer den Nutzer
(folgt)
