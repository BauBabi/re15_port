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
- Messung vorher beider Punkte (Abschnitte 1.1 / 2.1): Kleidung bleibt nach Vollheilung blutig (787/603 Pixel),
  Gift = RE1.5-Takt -2/121, EKG-Spur gruen wie FINE.
- Bestand gelesen: Gift-Takt = RE1.5-Form (spinnen_r38.c re15_spinnen_r38_gift_tick, -2 HP alle 121 Bilder,
  @0x80031e44-ecc), gerufen NUR im Kommando-1-Prolog (game_step_common.c:1712). EKG: RE1.5-Farbtabelle
  @0x8007622c, Eintrag 3 (Gift) = 20 ff 20 = GRUEN wie FINE. Kleidung = Blut-Decal-Wundtabelle (re15_damage.c
  s_wounds[8]) + VRAM-Blit in die Slot-0-Basis (render_pc.c re15_render_pc_wound_blit schreibt in
  s_slot0_base_rgba, also KEINE unversehrte Kopie mehr); Heilung loescht im Original nichts (blood_decals.md).

## 1. Punkt 1 — volle Heilung = ganze Kleidung
### 1.1 Messung vorher (gebauter Stand c71fa144 + Messschiene, Messkopie re15_pc_r39f_stand1.exe)
Aufbau (F_belege/lauf2.sh): Spielstand per Werkzeug `test_r39_zustand karte` (ROOM1130 Leon / ROOM1131 Elza, Lage
(-1024,0,-3336) rot 856 Cut 2 wie test_r36_kleidung_bild, HP 20, Inventar Slot 0 = First Aid Spray 0x22), CONTINUE,
Heilspray ueber den echten Statusschirm (RE15_PAD_AT=200:S,260:A,300:A,340:A,500:S: START oeffnet, SQUARE = ITEM,
SQUARE = Kommandokranz, SQUARE = USE, START schliesst; Menue-Zustaende aus RE15_INV_LOG: st 1 -> 3 (F300) -> 4 (F308)
-> 5 (F340) -> 6 (F375) -> 1 (F383), Schirm zu F507), Framedumps Bild 150 (vorher) und 650 (nachher), je einmal mit
allen 8 Wund-Panels Stufe 2 und einmal ohne Wunden; Mass = Pixel-Differenz im Figurenkasten x177..247 / y81..205
(`test_r39_zustand bild`, wie r36).

| Lauf | HP (state.log) | Pixel-Differenz mit/ohne Wunden Bild 150 | Bild 650 (nach dem Spray, HP 100) |
|---|---|---|---|
| Leon k_v_leon_w / k_v_leon_o | 20 -> 100 in F375 | 737 (Kasten 192,96-231,179) | **787** (183,89-227,178) |
| Elza k_v_elza_w / k_v_elza_o | 20 -> 100 in F375 | 462 (193,110-223,180) | **603** (191,101-226,180) |

Befund: die Heilung setzt die HP auf 100, die Blut-Decals bleiben auf der Kleidung (Bild 650 unterscheidet sich weiter
von der unverwundeten Figur) — der Nutzerpunkt ist reproduziert. Bilder: F_belege/laeufe/k_v_*_{w,o}/fd_000650.png.
### 1.2 RE-Belege
(folgt)
### 1.3 Umsetzung
(folgt)
### 1.4 Messung nachher
(folgt)

## 2. Punkt 2 — Gift zieht Energie wie RE2, EKG zeigt es
### 2.1 Messung vorher (gebauter Stand + Messschiene RE15_R39_GIFT_AT/RE15_R39_LOG, Messkopie re15_pc_r39f_stand1.exe)
Aufbau (F_belege/lauf.sh g_vorher / g_gesund): Leon ROOM1130, HP 100, Gift-Bit 0x2 (0x800acaec = status_flags) im Bild
300 gesetzt, HP je Bild protokolliert, Statusschirm mit START im Bild 1500, Framedumps 1540..1600.

| | Takte (Bild: HP) |
|---|---|
| g_vorher | F301: 98, F422: 96, F543: 94, F664: 92, F785: 90, F906: 88, F1027: 86, F1148: 84, F1269: 82, F1390: 80 |
| g_gesund | kein Abzug (HP 100 bis F1500) |

= die RE1.5-Form (-2 HP alle 121 Bilder, erster Takt sofort @0x80031e74, Zaehler 0x78 @0x80031e78), nicht die
RE2-Form (-1 HP alle 31 Bilder, s. 2.2). Statusschirm (F_belege/laeufe/g_vorher/fd_001600.png vs g_gesund): die
EKG-Spur ist in BEIDEN Laeufen gleich gruen (Pixelfarben der Spur (16,112,24)/(16,80,24)/..., Farbtabelle @0x8007622c
Eintrag 3 = 20 ff 20 wie Eintrag 0); Bild-Differenz vergiftet/gesund liegt NUR im Wort (x54..95, y143..152: "Fine" gruen
vs "Poison" magenta, 304 Pixel). Vom EKG her zeigt der Port das Gift also nicht — der Nutzerpunkt ist reproduziert.
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
