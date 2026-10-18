# Runde 39 — Spur F "zustand" (2026-10-18)

Baum `.claude/worktrees/r39_zustand`, Zweig `r39/zustand`, Basis c71fa144 (master nach Integration Welle 1).

## Auftrag (Wortlaut, AUFTRAG.md)
1. "Wenn der Spieler verletzt wurde, hat dann kaputte Klamotten und heilt sich aber wieder vollständig - zum Beispiel
   mit Heilspray - dann muss er auch wieder die ganzen Klamotten haben."
2. "Der "Poison" Zustand ist zwar jetzt da, aber er zieht keine Energie ab Stück für Stück, so wie im Originalen
   Resident Evil 2. Zumindest zeigt er es vom EKG her nicht so an. Da ist noch ein Fehler."

## Fortschritt (fortlaufend, neueste unten)
- 10:2x Baum geprueft (status leer, HEAD c71fa144), AUFTRAG/VERTRAG gelesen, Vorrunde R_spinnen.md (Gift) und
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
### 1.2 RE-Belege (Adressen, Bytes)
- **Original RE1.5 loescht bei Heilung nichts** (analysis/blood_decals.md §5, Writer-Zensus auf die Wund-Tabelle
  @0x800b10ec: nur der Builder FUN_80037c1c (`addiu s1,s1,4332` @0x80037c30 = 0x800b10ec, Level+Akku := 0) und der
  Akkumulator FUN_80037edc (`addiu a3,a3,4332` @0x80037ef4, Schwelle 0x78 @0x80037f14, Clamp 2 @0x80037f50). Der
  Heil-Applier FUN_8004adcc @0x8004adcc-0x8004b070 (Tabelle @0x80010fbc, s0 = 100 @0x8004ae14, Gift-Loeschung
  `andi 0xfffd` @0x8004af8c) enthaelt KEINEN Store auf 0x800b10ec. RE2 Retail kennt keine Blut-Decals — dort gibt
  es nichts, was zurueckgesetzt werden muesste. => Punkt 1 ist **NUTZER-VORGABE** (PORT-WAHL), kein Original-Verhalten.
- **FORM der Vorgabe, belegt:** "vollstaendig geheilt" = die HP erreichen den Wert, den das Heilspray schreibt:
  `sh s0` mit s0 = 100 (`addiu s0,zero,100` @0x8004ae14; absolute Heilung der Ids 0x22/0x2a/0x2d, Port
  item_use_common.c s_heal[].set) + der Westen-Bonus des Ports (re15_vest_hp_bonus, scd_room_setup.c). Additive
  Heilungen (0x23/0x24/0x27..0x2c/0x2e, ohne Kappung @0x8004afa8-b0) zaehlen, sobald HP >= dieser Wert. Die Nuss 0x2f
  (absolut 0x4d = 77 @0x8004b028) ist nie voll.
- **Wiederherstellung = der Spieler-Lade-Pfad des Originals:** FUN_800314b0 baut die Wund-Tabelle neu (Builder
  FUN_80037c1c @0x800316c8/@0x800318cc) und laedt die Spielerhaut (FUN_8004ee78, Slot 0x17) frisch aus der PLD —
  genau so verliert ein geladener Spielstand im Original sein Blut (blood_decals.md §5.6). Der Port tut nach der
  vollen Heilung dasselbe: Tabelle nullen (re15_wound_reset) + Slot-0-Upload der PLD-TIM (main.c
  pc_player_skin_reload, Familie PL%02X aus DAT_800aca5c: Leon 0, Elza 4 — Tabelle @0x80073f70, Leser
  FUN_800314b0 @0x800314d4). Der Waffen-Composite legt sich ueber die Slot-0-Generation wieder auf
  (render_pc.c s_slot0_generation, main.c s_wpn_gen).
- Warum nicht nur die Tabelle nullen: render_pc.c re15_render_pc_wound_blit kopiert die Damage-Bank IN die
  Slot-0-Basis (s_slot0_base_rgba, in place) — die Basis ist danach nicht mehr unversehrt; nur ein frischer
  Upload der PLD-TIM stellt die Kleidung her (gemessen 1.1: nach dem Spray blieben 787/603 Pixel Blut).

### 1.3 Umsetzung
- `engine/src/zustand_r39.c` re15_zustand_r39_heilung(id): nach dem Applier (Haken am Ende von
  re15_item_use_apply, item_use_common.c, 2 Zeilen): HP >= re15_zustand_r39_hp_voll() (= 100 @0x8004ae14 +
  Weste) -> re15_wound_reset() + Anforderung "Haut neu".
- `platform/pc/main.c`: pc_player_skin_reload() (PLD-TIM der aktuellen Familie/des Modellwechsel-Index ueber
  pc_read_pl_part -> re15_render_pc_upload_tim_slot(&tim, 0)); Abholung im Blut-Decal-Sync-Block (2 Zeilen) vor
  der Generationspruefung. Log: `[r39-zustand] Heilung 0x22: HP 100 >= 100 voll -> Wund-Tabelle genullt (hatte
  Wunden), Spielerhaut neu` / `[r39-zustand] Spielerhaut PL00.TIM neu geladen (...) - Kleidung ganz`.
- Beide Szenarien: die Mechanik ist figur-unabhaengig (Elza = Familie PL04, Panels aus LUT-Zeile 4 nur beim Blit).
### 1.4 Messung nachher (Stand 87180698 gebaut = build_4.log, Messkopie re15_pc_r39f_stand3.exe, F_belege/lauf3.sh)
Derselbe Aufbau wie 1.1 (Spielstand HP 20 + Heilspray im Platz 0, CONTINUE, Spray ueber den echten Statusschirm
START/SQUARE/SQUARE/SQUARE/START, Heilung in Bild 375, Framedumps 150/650), je Figur einmal mit allen 8 Panels Stufe 2
(`_w`) und einmal ohne Wunden (`_o`), Mass = `test_r39_zustand bild` im Figurenkasten x177..247 / y81..205:

| Lauf | Bild 150 (vor dem Spray) w vs o | Bild 650 (nach dem Spray) w vs o | debug.log |
|---|---|---|---|
| Leon k_n_leon_w / k_n_leon_o | 737 Pixel (Kasten 192,96-231,179), ausserhalb 0 | **0 Pixel** (vorher 787) | `Heilung 0x22: HP 100 >= 100 voll -> Wund-Tabelle genullt (hatte Wunden), Spielerhaut neu` / `Spielerhaut PL00.TIM neu geladen (384x256) - Kleidung ganz` |
| Elza k_n_elza_w / k_n_elza_o | 462 Pixel (193,110-223,180), ausserhalb 0 | **0 Pixel** (vorher 603) | dito, `Spielerhaut PL04.TIM neu geladen` |

Gegenprobe im Spray-Lauf selbst (Leon w, Bild 150 vs 650): 4526 Pixel verschieden (Pose + Kleidung) — die Kleidung
nach dem Spray ist pixelgleich mit der nie verwundeten Figur. Vor der Heilung sind die Wunden weiterhin sichtbar
(737/462 wie in 1.1), d.h. der Wund-Pfad der Runde 36 ist unveraendert. Bilder: F_belege/laeufe/k_n_*/fd_000{150,650}.ppm.
Waffen-Composite: der Slot-0-Upload bumpt s_slot0_generation (render_pc.c:2562), der Composite-Block in main.c
(`gen != s_wpn_gen`) baut die Waffe danach neu auf — in den Laeufen ohne Ausruestung (eq=128) nicht sichtbar, Mechanik
im Code gelesen (render_pc.c:2549-2562, main.c Composite-Watch).

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
### 2.2 RE-Belege (Adressen, Bytes)
**RE1.5 (Beta, Stand Runde 38):** Gift-Wirkung im Kommando-1-Handler 0x80031de8 @0x80031e44-ecc: `lhu 0x800acaec /
andi 0x7 / beq` (kein Gift -> Ende), Zaehler 0x800acc27 -1, war 0 -> Zaehler 0x78 (8 bei Bit 0x4), HP -2, HP < 0 ->
1. Periode 121 Bilder, -2 HP; laeuft nur im Kommando 1 (nicht in Griff/Flinch/Treppe). Der Gift-Wuerfel des
Spielerzweigs ist tot (a0 = 0x0A an beiden `jal 0x8001af20` @0x80012e78/e80, R_spinnen.md 1.4) — RE1.5 hat das
Gift nie fertig gebaut. **Beta -> Retail: RE2 ist die Zielform** (Nutzer: "so wie im Originalen Resident Evil 2").

**RE2 Retail (PSX.EXE Leon, ghidra_re2_Leon.txt, selbst disassembliert):**
- Spieler-Takt FUN_8003bfac, jedes Bild VOR dem Zustands-Dispatch (`lbu v0,0x4(s0)` @0x8003c188 /
  `jalr` ueber PTR_LAB_800a4030), Pause-Tor `DAT_800cfbdc < 0 -> return` @0x8003bfac-c0 (= Port RE15_PAUSE_PLAYER /
  bltz @0x80031c78):
  ```
  8003c12c 0d 80 02 3c  lui   v0,0x800d
  8003c130 10 fe 42 94  lhu   v0,-0x1f0(v0)   = DAT_800cfe10 (Gift-Wort)
  8003c138 12 00 40 10  beq   v0,zero,LAB_8003c184          kein Gift -> Ende
  8003c144 12 fe 63 90  lbu   v1,-0x1ee(v1)   = DAT_800cfe12 (Takt-Zaehler)
  8003c14c ff 00 62 24  addiu v0,v1,0xff      ; 8003c154 sb v0,-0x1ee(at)   Zaehler - 1 (u8)
  8003c158 0a 00 60 14  bne   v1,zero,LAB_8003c184 ; 8003c15c 1e 00 02 24 _li v0,0x1e   lief noch -> Ende
  8003c164 12 fe 22 a0  sb    v0,-0x1ee(at)   Zaehler := 30
  8003c168 56 01 02 86  lh    v0,0x156(s0)    HP
  8003c174 02 00 42 28  slti  v0,v0,0x2 ; 8003c178 bne v0,zero,LAB_8003c184   HP < 2 -> Ende (Untergrenze 1)
  8003c17c ff ff 62 24  _addiu v0,v1,-0x1 ; 8003c180 56 01 02 a6 sh v0,0x156(s0)   HP := HP - 1
  ```
  Periode 31 Bilder (30 .. 0, Abzug bei 0), -1 HP, nie unter 1, keine weiteren Stores (kein Kommando-/Zustandswort,
  kein SE) = der Takt ist kein Treffer. Laeuft in JEDEM Spielerzustand (Griff, Flinch, Zielen, Treppe), nur die Pause
  haelt ihn an.
- Vergiftung = Op 32 der Raum-ESP (FUN_8001f530, Giftkugel der Spinne, Spinnen-sub `andi 0x8` @0x80020118):
  `li v0,0x1518` @0x80020120 / `sh v0,-0x1f0(at)` @0x80020130 (0x800cfe10 := 5400 = "Gift an"; der Wert wird
  nirgends heruntergezaehlt, alle 18 Leser pruefen nur != 0), **`li v1,0x1e` @0x80020138 / `sb v1,-0x1ee(at)`
  @0x80020140 (0x800cfe12 := 30)** -> erster Abzug 31 Bilder nach dem Treffer. Weitere Giftquellen 0x80021e98 /
  0x80023bf0 (FUN_80021a48, andere ESP-Ops) gibt es in RE1.5 nicht.
- Heilung: Inventar FUN_8006affc `sh zero` 0x800cfe10 @0x8006b1e8 (Blaues Kraut/Mischungen, EKG-Wischmodus 3
  @0x8006b1e0); Skript Poison_clr @0x800591a8/@0x80059334 (+ 0x800cfe12 := 0 @0x800591b0/@0x8005933c),
  Poison_ck @0x8005930c (`sltu v0,zero,v0`). Port: Gegengift 0x23 / Blau 0x26 / Mischungen loeschen das
  RE1.5-Bit @0x8004af8c (vorhanden, item_use_common.c).
- Statusschirm-Klassifikator FUN_80069538 @0x80069538-80069584: `lhu 0x800cfe10` @0x80069544, != 0 -> **4 = POISON
  vor allem anderen**; sonst HP (0x800cfd4e) > max/2 (0x800cfd5a >> 1) -> 0 FINE, >= 0x29 -> 1, >= 0x15 -> 2, sonst 3.
  (RE1.5 FUN_8004ed6c ebenso: Bit 1 -> 3 vor den Schwellen 80/20 @0x800112b4/b5.) Auch in RE2 verraet der Schirm
  waehrend des Gifts die HP nicht — er zeigt POISON.
- EKG-Farben RE2 FUN_800754ac (Aufruf `jal FUN_80069538` @0x8006a6d8 -> `move a0,v0` -> `jal FUN_800754ac`
  @0x8006a6e0): Linie i (iVar6 = 32 .. 1, Kopf hell) Farbe = `(&DAT_800ab160)[cond*6 + c] - (&DAT_800ab163)[cond*6 + c]
  * i`. Tabelle @0x800ab160 (selbst gelesen):
  | cond | Bytes @ | r g b / dr dg db | |
  |---|---|---|---|
  | 0 FINE | 800ab160 | 20 ff 20 / 01 08 01 | gruen |
  | 1 CAUTION | 800ab166 | ff ff 20 / 08 08 01 | gelb |
  | 2 CAUTION | 800ab16c | ff 80 20 / 08 04 01 | orange |
  | 3 DANGER | 800ab172 | ff 20 20 / 08 01 01 | rot |
  | **4 POISON** | **800ab178** | **ff 20 ff / 08 01 08** | **magenta** |
  Wellenformen @0x800ab180 (5 Zeiger, 0x1d001000-Stil {y,h}-Bytepaare).
- RE1.5-EKG (Port-Statusschirm, FUN_80048a44): Farbtabelle @0x8007622c, 4 Bytes {r,g,b,Fade-Maske} je Zustand
  (selbst gelesen): [0] 20 ff 20 02, [1] 80 80 20 07, [2] 20 ff 20 07, **[3] 20 ff 20 02** (Gift = dasselbe Gruen wie
  FINE), [4] 20 20 20 07, [5] 20 20 20 07; Wellenform-Zeiger @0x80076214 (6 Eintraege). Gemessen (2.1): die Spur ist
  vergiftet und gesund pixelgleich.

### 2.3 Umsetzung
- `engine/src/zustand_r39.c` re15_zustand_r39_spieler_takt(pl, aktiv) = die RE2-Instruktionsfolge
  @0x8003c130-180 1:1 (Gift-Wort = RE1.5-Bit 0x2 von status_flags; Zaehler s_takt = 0x800cfe12; -1 HP bei 0,
  dann 30; `slti 2` Untergrenze). Haken in game_step_common.c neben dem Zopf-Haken (hinter dem Pause-bltz
  @0x80031c78, vor der Zweigkette), aktiv = rdt_ok && !RE15_PAUSE_PLAYER; der Abzug wird aus dem Treffer-Detektor
  s_prev_hp herausgenommen (wie Runde 38 N1). Der alte RE1.5-Aufruf im Kommando-1-Prolog ist entfernt
  (re15_spinnen_r38_gift_tick bleibt als dokumentierte Beta-Form im Code, ungenutzt).
- re15_zustand_r39_gift_setzen() (= `sb 0x1e` @0x80020140): Haken am Giftklumpen-Treffer in spinnen_r38.c
  (1 Zeile hinter `status_flags |= 0x2`). Kommando 0 nullt den RE2-Zaehler nicht (RE2 hat keinen solchen Schreiber;
  der RE1.5-Zaehler-Reset @0x8003196c betrifft nur die Beta-Form).
- EKG: re15_zustand_r39_ekg_farbe(cond, i): fuer cond 3 (Gift) die RE2-POISON-Farbe @0x800ab178-7d
  (r = 0xff - 8 i, g = 0x20 - i, b = 0xff - 8 i); Haken in re15_inv_screen.c hinter den drei Farbzeilen der
  32 LineF2 (2 Zeilen). FINE/CAUTION/DANGER bleiben die RE1.5-Tabelle (s. OFFEN).
- Messschiene (env, kein Verhalten): RE15_R39_GIFT_AT=<bild> (Gift-Bit + Zaehler 30 wie Op 32), RE15_R39_LOG=<datei>
  (je Bild hp/status/Zaehler/cond). Messschiene RE15_SPINNEN_LOG (Spur R) traegt in `gz=` jetzt den RE2-Zaehler.
- Tests der Runde 38 (unit_r38_spinnen B/B3/B4, pruef_log der integration_r38_spinnen_echtlauf) auf die RE2-Form
  umgestellt (Periode 31, -1 HP, Zaehler 30 am Treffer, kein Kommando-0-Reset).
### 2.4 Messung nachher (Stand 87180698 gebaut, Messkopie re15_pc_r39f_stand3.exe, F_belege/lauf3.sh)
Aufbau wie 2.1 (Leon ROOM1130, Gift per Messhaken im Bild 300, RE15_R39_LOG=gift.log, START im Bild 1500, Framedumps
1540..1600) + Lauf g_unter mit HP 5 (Untergrenze):

| Lauf | `test_r39_zustand hp gift.log` |
|---|---|
| g_nachher (HP 100) | 38 Takte, Summe 38, **Abzug min 1 max 1**, erster Takt **F330**, **Abstaende 31: 37 von 37**, HP-Minimum 62 (F1477), Giftbilder 1200, letztes Bild F1499 |
| g_unter (HP 5) | 4 Takte (F330 5->4, F361 4->3, F392 3->2, **F423 2->1**), HP-Minimum **1**, danach bis F900 kein Abzug (`slti v0,v0,0x2 / bne` @0x8003c174-78) |

= die RE2-Form @0x8003c12c-180 (vorher: -2 alle 121, erster Takt sofort). Erster Takt im Bild 330 = 30 Bilder nach
dem Messhaken, weil der Haken den Zaehler im selben Spielerschritt VOR der Abfrage setzt (30 -> 29 im Bild 300, 0 im
Bild 330); am Spinnen-Treffer setzt Op 32 den Zaehler im ESP-Takt NACH dem Spielerschritt (@0x8001ce0c < @0x8001ce2c),
dort also 31 Bilder (unit_r38_spinnen B3/B5 lassen 0 oder 1 Takt in 60 Bildern zu). Im Statusschirm (ab F1500) gibt es
keine Logzeile und keinen Takt: re15_game_step kehrt bei re15_menu_gameplay_frozen vor dem Spieler-Dispatcher um
(game_step_common.c:1343, unveraendertes Port-Verhalten; der RE2-Takt gehoert zum Spielertask FUN_8003bfac, der im
RE2-Inventar nicht laeuft — nicht separat disassembliert, Spielwelt steht dort erkennbar).
EKG (`test_r39_zustand ekg`, Kasten x26..103 / y118..140 = die Linien x = sweep-i-6 @0x80048c34-44, y = 82+39..55):

| Bild | g_nachher | g_vorher / g_gesund (2.1) |
|---|---|---|
| 1540 | hell 23, **magenta 23**, gruen 0 | — |
| 1560 | hell 33, **magenta 27**, gruen 0, sonst 6 (dunkle Schwanzlinien) | — |
| 1580 | hell 19, **magenta 19**, gruen 0 | — |
| 1600 | hell 5, **magenta 5**, gruen 0 | hell 1, gruen 1, magenta 0 (beide) |

Zoom F_belege/laeufe/g_nachher/ekg_zoom_1560.png (magenta Spur + "Poison") gegen g_vorher/ekg_zoom_1560.png (gruen):
die Spur traegt jetzt die RE2-POISON-Farbe @0x800ab178 (Kopf hell, Schwanz abgedunkelt 08 01 08 je Linie).
**Wellenform (gemessen, F_belege/ekg_wellen.txt):** RE1.5 hat nur ZWEI Wellen — Tabelle 0 (FINE @0x80075774) und
eine gemeinsame fuer alle anderen Zustaende (Tabellen 1..5 @0x8007592c/ae4/cec/ea4/0x8007605c sind bytegleich; Gift =
Tabelle 3); RE2 hat je Zustand eine eigene (5 Zeiger @0x800ab180, POISON @0x800aad58: 80 Spalten {y,h} u8, mehrere
unregelmaessige Zacken, Hub 4..36 bei Grundlinie 15). Die RE2-Welle passt nicht ohne erfundene Skalierung in den
RE1.5-Rahmen (77 Spalten, Hub 39..55 bei Grundlinie 47, das Wort "Poison" liegt im Fenster): NICHT uebernommen,
die Gift-Spur zeigt die RE1.5-Welle (Tabelle 3) in der RE2-Farbe. Siehe 5 (Entscheidungsfrage).

## 3. Tests
(folgt)

## 4. OFFEN
(folgt)

## 5. Fuer den Nutzer
(folgt)

## Fortsetzung (Sitzung 2, ab ~12:10 — der Vorgaenger wurde ~11:08 vom Sitzungslimit beendet)
- 87180698 (vom Orchestrator gesicherte Reste) Zeile fuer Zeile gegen die Belege gelesen: Haken game_step_common.c
  (Spieler-Takt hinter dem Pause-bltz, Abzug aus s_prev_hp), item_use_common.c (Heil-Applier -> re15_zustand_r39_heilung
  im `if (h)`-Zweig), re15_inv_screen.c (EKG-Farbe hinter den drei Farbzeilen, i = 0 Kopf wie RE2 s1 = 31..0 nach
  `addiu s1,s1,-0x1` @0x800755a4: Farbe = base - step * s1 @0x800755c0-5e0, Kopf = base), spinnen_r38.c (Zaehler 30
  am Giftklumpen), main.c (pc_player_skin_reload: PL%02X aus s_player_model_idx/character wie pc_sync_player_model,
  pc_read_pl_part -> Slot 0; Abholung vor der Generationspruefung). RE2-Tabellenbytes @0x800ab160-7d und die
  Wellen-Zeiger @0x800ab180 / @0x80076214 aus den EXE-Dateien nachgelesen (F_belege/ekg_wellen.txt).
- Bau: build_4.log (nichts neu zu bauen, exe = 11:06 = der Stand 87180698). Messung nachher 1.4 / 2.4.
