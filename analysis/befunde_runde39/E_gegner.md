# Runde 39 — Spur E "gegner" (Parking-Lot-Gorillas, Hundebiss, liegende Zombies, Gorilla-Schatten)

Baum: `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`, Basis master c71fa144. Beginn 2026-10-18.

## Auftrag (woertlich, AUFTRAG.md)
1. "Die Gorillas beim Parking Lot können ruhig doppelt so viel Energie haben. Außerdem sollten sie erst die flinch
   Animation machen, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen wurden, nicht bei jeden Schuss."
2. "Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch von anderen Hunden
   angegriffen/gebissen werden können, wenn ich mich nicht wehren kann, so zum Beispiel bei Room 1090."
3. "Die Zombies in ROOM 1220/1221 und in ROOM 1010 die am Boden liegen kriechen im Original. Das sollen sie bei uns auch."
4. "Die Gorillas haben beim springen direkt ihren Schatten unter dem beim." (Nachtrag 2026-10-18; Orchestrator: der
   Schatten haengt beim Sprung mit in der Luft unter dem Koerper statt auf dem Boden zu bleiben.)

Zuteilung VERTRAG.md §0: Spur E = genau diese vier Punkte. Keine Bank-9-Bits, keine Nachrichten-Ids noetig.

## Protokoll (fortlaufend)

### 2026-10-18 Start
- Baum geprueft: `git status --porcelain` leer, HEAD c71fa144, Zweig r39/gegner.
- AUFTRAG.md + VERTRAG.md vollstaendig gelesen; Vorrunden: Runde 35 J_affen.md, Runde 36 O_pri.md/F_zombieki,
  Runde 38 S_affen3.md, B_toene.md (Runde 39), Runde-16-Dossiers analysis/befunde_2026-09-19/kriecher-1010.md,
  liegende-zombies.md.
- Baseline-Build (local_build.sh configure + build) in diesem Baum: OK (exe 10:44).

### Schritt 0 — Zensus: welche Gegner, welche Raeume (Sce_em_set, `scd_dump_room.py`)
- **ROOM11C0 ("PARKING LOT", debug_jump_table.h)**: zwei Gorillas Typ **0x27** (`44 01 27 30 ..` / `44 02 27 30 ..`
  Szenen-Variante grid 0x30 @0x01784/0x01798; Kampf-Variante grid 0x10 @0x017E8/0x017FC). ROOM1141 traegt ebenfalls
  0x27 (grid 0x30/0x10) — darum Raum-Tor auf 0x11C0 (Nutzer: "beim Parking Lot", Einstieg: "nicht 1141").
- **"Room 1090" mit Hunden = ROOM1190** (Schiessstand B1, Hunde-Luke): ROOM1090.RDT hat keine Hunde (nur 0x26 Feuer,
  0x42 NPC, Zombies 0x10/0x11); ROOM1190.RDT: drei Hunde 0x20 (`44 00 20 40 ..` @0x02900/0x02914/0x02928 Szene,
  `44 00 20 00 ..` @0x02940/0x02954/0x02968). Dieselbe Zuordnung traf Spur B (B_toene.md §0.2) fuer den Gitter-Ton.
- **ROOM1220/1221**: zwei Zombies **0x16 mit grid 0x81** (liegend, @0x00F2A/@0x00F3E) + acht 0x16 grid 0x02.
  **ROOM1010/1011**: zwei Zombies **0x10 mit grid 0x81** (@0x00A12/@0x00A26 bzw. @0x00ACE/@0x00AE2).
- Flavor des Nutzers: RE2-KI (Standard seit 2026-08-22). Hund = RE2-Hundemodul (enemy_ai_re2_dog.c), Zombies =
  RE2-Zombiemodul (enemy_ai_re2_zombie.c); der Gorilla 0x27 hat in beiden Flavors das RE1.5-Gehirn
  (enemy_ai_common.c re15_maggot_ai_tick, Wurzel STAGE1.BIN 0x80116db8).

### 2026-10-18 12:05 — Fortsetzung (zweiter Agent nach dem Sitzungslimit, Stand 9ddf0bbc)
- Vorgefunden: exe 11:00, test_r39_gegner.exe 11:05, ctest-Lauf 11:08 des Vorgaengers mit `LastTestsFailed.log` =
  unit_r35_affen_sprung / _wagen / _schrot (rot); integration_r39_gegner_hunde1190 war nie gruen (s.u.).
- Eigener Lauf 12:05 `ctest -R "r39_gegner|r35_affen"`: 7 Unit-Teile gruen, integration_r39_gegner_schatten gruen (58,6 s),
  **integration_r39_gegner_hunde1190 Timeout (900 s)**, **unit_r35_affen_sprung/wagen/schrot rot**.
- Zensus ROOM11C1 (Elza-Variante des Parking Lot, `scd_dump_room.py`): **keine 0x27**, sondern fuenf Hunde 0x20
  (`44 00 20 00 ..` @0x016F8, @0x0170C, @0x01720, @0x01734, @0x01748, grid 0x00). Das Raum-Tor 0x11C0 der Punkt-1-Haken
  ist damit vollstaendig (Elza trifft im Parking Lot auf Hunde — fuer Punkt 2 unten als zweite Messung genutzt).

#### Befund A — die drei roten Runde-35-Riegel (Regression dieser Spur, Ursache belegt)
- Ausgabe (12:05): sprung `Treffer 1: Flinch-Exit -> Sub 0 (erwartet 3)`, `Treffer 4: -> Sub 7 (erwartet 3)` (15 Fehler);
  wagen `HP 360 / 360 = 180 (Original t=6.11: 180/180)`; schrot `3. Treffer Spur 1: Exit F251 -> Sub 3 (Original F276 -> 7)`,
  `Landung F-1 bei (0,0)`.
- Ursache: alle drei Riegel booten ROOM11C0 (test_r35_affen.c room_boot, Kampf-Layout Flag 4/0x40) und messen den
  ORIGINAL-Mechanismus (HP 180 @0x8011f514, Flinch bei jedem Treffer, Vergeltungs-Sprung beim 3.) gegen DuckStation-
  Savestates (t=6.11, F276). Genau diesen Mechanismus aendert die NUTZER-VORGABE fuer den Parking Lot absichtlich.
- Fix: test_r35_affen.c room_boot `re15_r39_gegner_enable(0)` (+ include, 2 Zeilen, Kommentar "Runde 39 Spur E") — der
  Original-Riegel misst die unveraenderte Basis (sie gilt weiter in ROOM1141), die Vorgabe misst test_r39_gegner.
  Danach 20/20 r35_affen gruen (s. Tests).

#### Befund B — der Schatten-Haken des Vorgaengers zeichnete Phantom-/Schwebe-Schatten (Root-Skip fehlte) — ⛔ WIDERLEGT (Abnahme 0 M1, s. "Nachbesserung 1")
- Gemessen (gegner_r39.log des gruenen Schatten-Laufs, Zeilen mit Koerper-Y != Boden-Y je Slot/Zustand):
  `29x slot=2 st=1/0 koerper=-20000 boden=-3600`, `29x slot=3 ... -20000/-3600`, `35x slot=2 st=1/0 koerper=-2500 boden=-3600`,
  `311x slot=3 st=1/0 koerper=-2500 boden=-3600`, dazu 24x slot=3 st=1/7 (der Sprung, boden=0). Die Szenen-Gorillas stehen
  laut RDT bei y=-20000 (`44 01 27 30 02 00 00 60 3c fb e0 b1 c0 ab ..` @0x01784: `e0 b1` = -20000, pc[4]=2 -> +0x1ba=-3600)
  und per `Member_set 01=-2500` @0x0198E/@0x01996 im Wagen (J_affen.md: Original t=34.89 sitzt der Gorilla sichtbar in der
  Heckklappe). Mit dem ersten Haken lag ihr Quad auf -3600: 1,1 m UEBER dem Wagen-Gorilla bzw. auf dem Dach unter dem
  geparkten — zwei neue sichtbare Fehler.
- Damalige Deutung (FALSCH, Abnahme 0 §4.2, in Nachbesserung 1 selbst nachdisassembliert): "`andi v0,v0,0x20` @0x80116df4 /
  `bne v0,zero,0x80116e88` @0x80116df8 ueberspringt die GANZE Wurzel samt Schatten-Schwanz". Das Sprungziel 0x80116e88 liegt
  IN der Funktion vor dem Schwanz @0x80116f20-38, der auf jedem Pfad laeuft (STAGE3 0x29: `bne` @0x80110b40 -> 0x80110bd0,
  Schwanz @0x80110c68-80). Die Logzeilen oben waren keine sichtbaren Fehler: in Cut 5 liegen die Quads links ausserhalb des
  Bildes (x = -692..-22), in Cut 12 im schwarzen Innenraum der Klappenoeffnung (Band 120..214 x 107..130) — Details,
  Original-RAM-Tabelle und Bildvergleich in "Nachbesserung 1".
- Der daraus gebaute Root-Skip-Haken `re15_r39_schatten_aus` (Stand c3823fa4) ist in Nachbesserung 1 wieder entfernt;
  `re15_r39_schatten_y` setzt den Quad fuer 0x27/0x29 immer auf +0x1ba (Original-Lage). Logzeilen mit Bildnummer bleiben.

#### Befund C — integration_r39_gegner_hunde1190 lief in den Timeout: Leon stirbt
- Beide Laeufe (11:08 und 12:16) bildgleich: Bisse F685 (-20), F720, F753, F833, **F1042 (hp 0), F1074 (hp -20 = Tod)**,
  danach CONTINUE -> der raumgebundene `RE15_EXIT_AT=2600#1190` kam nie, 2 x 480 s. Fix: `RE15_EXIT_AT=1000` (ohne Raum),
  Timeout 300 s, Pruefung (d) "Leon lebt am Ende"; der Lauf misst die vier Bisse F685/720/753/833.

### 2026-10-18 13:20 — Nachbesserung 1 (dritter Agent, nach Abnahme 0 = E_abnahme_0.md, Mangel M1)
- Stand vorgefunden: HEAD 8338a13e (c3823fa4 + Abnahme-Bericht), Baum sauber, exe 12:44. Abnahme 0: Punkte 1-4 erfuellt,
  M1 = die Root-Skip-Behauptung von Befund B ist am Byte falsch. Bearbeitung: Abschnitt "Nachbesserung 1" am Ende.

## Punkt 1 — Parking-Lot-Gorillas: doppelte Energie, Flinch erst nach 3 Handfeuerwaffen-Treffern (Energie)

> **Stand Nachbesserung 2 (Nutzer-Nachtrag 3 "generell")**: die Regel gilt fuer JEDEN Gorilla Typ 0x27 (ROOM11C0 Leon und
> ROOM1141 Elza = alle 0x27-Raeume des Zensus), es gibt kein Raum-Tor 0x11C0 mehr (`re15_r39_affen_regel`, Konstante
> `RE15_R39_AFFEN_TYP 0x27u`). Die Abschnitte unten beschreiben den ersten Bau (Raum-Tor); Ersatz s. "Nachbesserung 2".

### RE-Belege (Original, selbst gelesen)
- **HP-Saat** (STAGE1.BIN ohne Kopf, Datei-Offset = Adresse - 0x80100000): INIT FUN_80116f50
  `jal 0x8001af20` (rng) @0x801170cc, `lui a0,0x8012 / addiu a0,a0,-4044` = 0x8011f034 @0x801170d4-d8,
  `andi v0,v0,0xf` @0x801170e4, `lbu v1,8(a1)` (Typ) @0x801170e8, `sll v0,v0,1 / sll v1,v1,5 / addu / addu`
  @0x801170ec-f8, `lhu v0,0(v0)` @0x801170fc, **`sh v0,154(a1)` = +0x9a (HP) @0x80117104**.
  Zeile Typ 0x27 = 0x8011f034 + 0x27*0x20 = **@0x8011f514**: 16 x **180** (Datei 0x1f514, alle 16 Spalten gleich).
  Port: enemy_ai_common.c INIT `if (e->hp <= 0) e->hp = 180;` (konstant, weil alle 16 Spalten 180 sind).
- **Handfeuerwaffen-Schaden am Gorilla** (PSX.EXE t_addr 0x80010000, Datei-Offset = Adresse - 0x80010000 + 0x800):
  Schadenstabelle @0x8006e0d0 + Typ*0x58 + Waffe*4 (u16); Zeile 0x27 @**0x8006ee38** =
  `{0,7,30,12,12,28,28,50,38,40,40,70,8,50,8,40,40,70,400,20,0,100}`; **Waffe 3 (Pistole, RE1.5-Waffen-Id
  DAT_8006f430[0]=3 @0x8006f430) = 12 @0x8006ee44**. Port: re15_damage.c s_wpn_dmg_maggot[3] = 12,
  `dmg = dmg_row[weapon_id]` im Schusspfad re15_player_weapon_fire.
- **Treffer-Pfad (EXE, FUN_80011f50 / Resolver)**: +0x5 = Waffen-Id (@0x800124bc), hp -= dmg, +0x93 |= 1
  (Einmal-Treffer-Riegel), +0x7 = 0 (@0x80012428), **+0x4 = 2 (HURT) bzw. 3 (@0x80012520)**.
- **Gorilla-HURT Spur 0 (Boden-Flinch) FUN_8011b018** (Spur = Waffen-Id ueber Tabelle @0x801214a8; Pistole 3 -> 0):
  Phase 0: `lbu v0,147(a0) / ori v0,v0,0x2 / sb v0,147(a0)` @0x8011b064-70 (+0x93 |= 2), `ori v1,zero,0x7 /
  sb v1,148(v0)` @0x8011b08c-90 (**Flinch-Clip 7**), Blut @0x8011b0bc-130, `jal 0x800453d0 / ori a0,zero,0x3`
  @0x8011b134-38 (Se 3). Exit Phase 2: `sb zero,147(a0)` @0x8011b178 (+0x93 = 0), `sb v0,4(v1)` @0x8011b188
  (+0x4 = 1), `ori v0,zero,0x7 / sb v0,5(v1)` @0x8011b194-98 (+0x5 = 7 Vergeltungs-Sprung; 9 nur bei +0x1e3
  @0x8011b1c8-d8). Runde 35 Spur J: Sprung erst beim 3. Treffer (mag_hit_ctr), sonst +0x5 = 3 (Jagd).
- Das Original flincht also bei JEDEM Treffer (der Resolver schreibt +0x4 = 2 bedingungslos; es gibt keine
  Schadens-Schwelle). Beides — doppelte HP und die Flinch-Schwelle — sind **NUTZER-VORGABEN** (Form unten).

### Port-Ist (vor der Aenderung, Code gelesen)
- enemy_ai_common.c:8890 INIT hp = 180; :9423 `case 2` HURT-Wurzel; Spur 0 spielt Clip 7 bei jedem Eintritt.
- re15_damage.c re15_player_weapon_fire: `e->state = (e->hp >= 0) ? 2 : 3;` nach `hp -= dmg` — jeder Treffer ->
  HURT. Der Schuss faellt im Spieler-Schritt VOR `re15_enemy_ai_run_all` (game_step_common.c:2414), der Gorilla-Tick
  sieht den Treffer also im selben Bild.

### Umsetzung
- Neue Datei engine/src/gegner_r39.c + include/re15_gegner_r39.h (Konstanten: RE15_R39_PARKPLATZ_RAUM 0x11C0 — seit
  Nachbesserung 2 ersetzt durch RE15_R39_AFFEN_TYP 0x27u,
  RE15_R39_AFFEN_HP_TABELLE 180 @0x8011f514, RE15_R39_AFFEN_HP_FAKTOR 2 = NUTZER-VORGABE, RE15_R39_PISTOLE_SCHADEN 12
  @0x8006ee44, RE15_R39_ZUCK_TREFFER 3 = NUTZER-VORGABE, Schwelle 36).
- Haken enemy_ai_common.c (je 1 Zeile, Kommentar "Runde 39 Spur E"): INIT `e->hp = re15_r39_affen_hp_init(e, 180)`;
  HURT-Wurzel `case 2` zuerst `if (re15_r39_affen_zucken_unterdruecken(e)) break;`; Tick-Ende vor dem ROOT-Post-Pass
  `re15_r39_affen_merken(e)` (merkt +0x5/+0x6/+0x7 des Gehirn-Ticks); Blut-Zwilling `re15_r39_affen_blut` neben
  re15_maggot_gore (Blut + Se 3 wie der Flinch-Eintritt @0x8011b0bc-138).
- Akku = hp_ref - hp (hp_ref = HP beim INIT bzw. beim letzten Flinch). Treffer unter 36 (nicht Krit +0x93&0x40, nicht
  toedlich): Zustand 1 mit den gemerkten Unterzustaenden, +0x93 = 0 (@0x8011b178), Blut, Runde-35-Zaehler +1
  (jeder Treffer zaehlt -> der 3. Treffer flincht UND springt, wie Runde 35 "3 Treffer -> Sprung"). Treffer >= 36:
  Flinch wie bisher (Spur nach Waffen-Zeile @0x801214a8), hp_ref neu.
- Der Schuss faellt im Spieler-Schritt vor run_all (game_step_common.c:2414): der Gorilla-Tick sieht Zustand 2 im
  selben Bild, der Flinch-Clip 7 wird nie gesetzt (gemessen: Clip 7 = 0, Zustand 1 nach 0 Bildern).

## Punkt 2 — Kein paralleler Hundebiss (ROOM1190, RE2-Hundemodul)

### RE-Belege (RE2 Retail, EMD0G_MOD0.BIN RAW @0x80100000 + PSX.EXE Leon, selbst disassembliert)
- **Boden-Biss-Spielerhaken 0x801051BC** (a0 = 0: Spieler 0x800cfbf8, sonst Partner 0x800cfe18; a1 = Richtung):
  `lbu v1,467(a0)` @0x801051dc (PL+0x1D3), `addiu v0,zero,2 / sb v0,4(a0)` @0x801051e0-e4 (**PL +0x4 = 2 HURT**),
  `addiu v0,a1,2 / sb v0,5(a0)` @0x801051e8-ec (+0x5 = dir+2), `sb zero,6(a0) / sb zero,7(a0)` @0x801051f0-f4,
  **`ori v1,v1,0x80` @0x801051f8 / `sb v1,467(a0)` @0x80105200 = PL+0x1D3 |= 0x80 (EIN-ANGREIFER-RIEGEL)**.
  Aufruf aus dem Kontakt-Handler 0x80104DF0 nach FUN_800401D4(20,0) (Biss-Schaden 20 @0x80104EB8-C0), lebender
  Spieler (@0x80104F7C-88 -> 0x801051BC); toter Spieler -> 0x80105204 (PL +0x4 = 3).
- **Kontakt-Tor desselben Moduls**: `lbu v0,467(s1)` @0x80104e5c / `andi v0,v0,0x80` @0x80104e64 /
  `bne v0,zero,0x80104fcc` @0x80104e68 -> **kein Kontakt, solange der Spieler beansprucht ist**; ebenso das
  Flug-/Latch-Tor `lbu v0,467(s1)` @0x80104d84 / `andi 0x80` @0x80104d8c / `bne -> -4` @0x80104d90-94.
- **Freigabe durch den SPIELER** (PSX.EXE): Spieler-Wurzel FUN_8003bfac dispatcht PTR_LAB_800a4030[+0x4];
  Zustand 2 (HURT) = 0x8003f600, Unterdispatch @0x800a40e4[+0x5]: [0]/[1] 0x8003f658, **[2]/[3] 0x8003f778**
  (dir+2 des Hundebisses), [4] 0x8003f8e4, [5] 0x8003fc3c, [6] 0x8003ff3c.
  0x8003f778 Phase 0: Clip 3/4 (`addu v0,v0,a2` 0x30003 @0x8003f810-14), **`lbu v1,467(s0)` @0x8003f830 /
  `ori v1,v1,0x80` @0x8003f83c / `sb v1,467(s0)` @0x8003f844** (der Spieler beansprucht sich selbst erneut);
  Phase 1: Anim (0x8002959c @0x8003f88c); **Phase 2: `sw a0,4(s0)` (a0 = 1 -> Zustand 1) @0x8003f8b0 /
  `sb zero,467(s0)` @0x8003f8b4 = PL+0x1D3 := 0** (+0x1C0 &= 0xED @0x8003f8b8-c0, +0x154 |= 2 @0x8003f8bc-c4).
  Gleiches Muster [0]/[1]: `ori 0x80` @0x8003f6d4-d8, Exit `sb zero,467(s0)` @0x8003f748; [4]: @0x8003fa4c;
  [5]: @0x8003fe68 / @0x8003fec4. Die unteren 7 Bits von +0x1D3 zaehlt die Spieler-Wurzel jedes Bild herunter
  (`lbu v1,0x1d3(s0) / andi v0,v1,0x7f / beq -> / addiu v0,v1,-1 / sb v0,0x1d3(s0)` @0x8003bff4-c008).
  **Mechanismus: der Riegel steht genau so lange, wie der Spieler in seiner Treffer-Reaktion (Zustand 2) ist.**
- FUN_800401D4 (Spieler-Schaden) setzt den Riegel selbst nur im Todesfall (`lbu v1,0x1d3(a2) / ori 0x80 / sb`
  @0x800402c0-d0, Rueckgabe 2); der Port fuehrt das schon (re2z_player_damage).

### Port-Ist (Code gelesen)
- enemy_ai_re2_dog.c re2d_contact: Tor nur `re15_player_is_grabbed()` (Griff-Pin), **liest PL+0x1D3 (Port-Feld
  pl->re2z_self1d3) NICHT**; Boden-Biss setzt nur `pl->hit_react |= 1` + Kleidungsstempel — **kein Riegel**
  (Kommentar dort: "der Claim ueber den Grab-Pin unterbleibt (kein Latch)"). Folge: zweiter/dritter Hund beisst
  parallel waehrend der 22 Bilder Flinch (cmd 2, game_step_common.c s_hit_flinch) — der Nutzer-Befund.
- Der RE2-Zombie fuehrt denselben Riegel korrekt (Setzer @0x80102754-60, Loescher HURT @0x80104FA0 / DEATH
  @0x801082E8, enemy_ai_re2_zombie.c:1950/4349), die Zellenarme ebenso (enemy_ai_re2_zellenarm.c:690-696/872).
- Spieler-Seite im Port: cmd-2-Flinch 22 Bilder (PL00-Clip 8/9), Exit `pl->state = 1` (game_step_common.c:1599-1611),
  der NORMAL-Prolog loescht +0x93 (`pl->hit_react = 0` @0x80031964, :1626) im Folgebild; Knockdown-Exit kd_ende.

### Umsetzung
- enemy_ai_re2_dog.c (je 1 Zeile): Boden-Biss `re15_r39_hund_riegel_setzen(pl)` = `pl->re2z_self1d3 |= 0x80`
  (@0x801051f8-0x80105200) + Besitzer-Merker; Angriffs-Tor re2d_attack_decide `|| re15_r39_hund_riegel(pl)` (-4,
  @0x80104d84-94); Kontakt-Tor re2d_contact `|| re15_r39_hund_riegel(pl)` (@0x80104e5c-68).
- Freigabe `re15_r39_hund_riegel_tick()` je Bild am Kopf von re15_enemy_ai_run_all (enemy_ai_common.c, 1 Zeile):
  Port-Zwilling von @0x8003f8b4 (Zustand 1 + PL+0x1D3 := 0) = der NORMAL-Prolog hat +0x93 Bit 1 geloescht
  (@0x80031964) und weder Flinch (cmd 2, 22 Bilder) noch Niederschlag (cmd 4/5) laeuft; nur fuer den vom Hund
  gesetzten Riegel (Zombie-Griff @0x80102754-60 / Arme loeschen ihren selbst).
- Toedlicher Biss unveraendert (Latch + Opfer-FSM, re15_player_is_grabbed). RE1.5-KI-Hund: unveraendert, prueft im
  Original +0x93 == 0 (@0x8010f294).

## Punkt 3 — Liegende Zombies (0x81) kriechen (ROOM1010/1011, 1220/1221)

### RE-Belege
- **RE1.5 (STAGE1.BIN) DECIDE[5] FUN_8010466C** (Dossier 2026-08-26 / Port enemy_ai_common.c:4794-4825, selbst
  disassembliert): Block B @0x80104704: `sltiu 0xbb8` @0x80104718 (dist < 3000), arc 0x200 @0x8010472C-34,
  **`ori v0,1` @0x80104738 / `sw` @0x80104748 = Wort 1 = Kriech-Lokomotion**. Liegende 0x81-Zombies des Originals
  kriechen also los, sobald der Spieler in Sicht (+-0x200) und naeher als 3000 ist.
- **RE2 (EMZ0.BIN) INIT Kriecher-Zweig** @0x80100B60-8C: Deskriptor 1 -> `sw 0x201` @0x80100B74 = KRIECHER-WAIT
  (EXEC[2] @0x80103B48: Clip 23, Rate 0, **kein Ausgang**); DECIDE[2] @0x80103A70 weckt NUR in den Griff (512er-Sektor,
  dist < 0x514, Riegel frei, Sicht, Etage -> 0x101 @0x80103B10-14). Einen Distanz-Wecker in die Lokomotion hat RE2
  nicht (kriecher-1010.md §2.4). Deskriptor 3 -> `sw a1(=1)` @0x80100B8C = kriecht sofort.
- Port (Runde 16 F2, enemy_ai_re2_zombie.c:8708): RE1.5-sel 1 unter 0x80 -> RE2-WAIT (Totstellen, damals
  Nutzerwunsch "stellen sich erst einmal tot"), sel 3 -> Lokomotion. Folge heute: die 0x81-Zombies in 1010/1220
  liegen, bis der Spieler auf < 1300 im Sektor steht (Griff) oder sie anschiesst — sie **kriechen nie los**.
  Der Nutzer vergleicht mit dem RE1.5-Original (DuckStation): dort kriechen sie ab 3000.

### Umsetzung
- NUTZER-VORGABE "kriechen wie im Original" in RE1.5-Form: enemy_ai_re2_zombie.c re2z_crawl (Kriecher-Wurzel
  0x80101210, decide-dann-exec) zwischen DECIDE und EXEC (2 Zeilen): `if (e->sub_state_1 == 2 &&
  re15_r39_kriecher_weckt(e, pl)) re15_ai_set_state_word(e, 1u);` — erst der byte-true RE2-Griff-Wecker DECIDE[2]
  (0x101), dann, wenn der Kriecher noch wartet, der RE1.5-Wecker Block B (`re15_r39_kriecher_weckt`: ai_dist <
  0xbb8 @0x80104718, arc 0x200 @0x8010472C-34 -> Wort 1 @0x80104738/48). EXEC liest +0x5 frisch (@0x80101254) und
  betritt die Lokomotion P0 (@0x80103064) im selben Tick. Das Totstellen bleibt, bis der Spieler in Sicht und
  naeher als 3000 ist; Treffer (HURT-P2) und Griff wie bisher.

## Punkt 4 — Gorilla-Schatten beim Sprung auf dem Boden

### RE-Belege
- **RE1.5 Gorilla-Wurzel FUN_80116db8 (STAGE1.BIN), Schwanz jedes Bild**: `lw v0,-14460(v0)` (g_entity) @0x80116f24,
  **`lh a1,442(v0)` @0x80116f30 = a1 = +0x1ba (Boden-Referenz)**, `jal 0x8001b064` @0x80116f34, `addiu a0,a0,176`
  @0x80116f38 (a0 = +0xb0 Schatten-Record). FUN_8001b064 (EXE, Decompile RE_15_Quellcode_V2): `local_88.t[1] =
  (long)param_2` = Quad-Hoehe = a1; Ecken aus rec+0xc/+0xe (+0xbc/+0xbe), Versatz rec+8/+10, RotMatrixY(+0x6a).
  Dasselbe Muster: Hund @0x8010d91c (Runde 35 Spur H), kleiner Affe 0x29 STAGE3.BIN @0x80110c78 (`lh a1,442(v0)` /
  `jal 0x8001b064` @0x80110c7c / `addiu a0,a0,176` @0x80110c80).
- Sprung: +0x1ba bleibt der Boden (Ballistik c1a4 landet auf +0x1ba; Sce_em_set saet +0x1ba = -(pc[4]*1800)
  @0x800421d4-0x80042210, Port scd_vm.c:3929 dog_floor_y).
- RE2 (Gegenprobe, PSX.EXE Leon): Entity-Schleife `jal 0x8004fba0` @0x8002686c / `sh v0,0x1c2(s0)` @0x80026874,
  `lh a1,450(s0)` @0x800268c8 -> 0x800168b4 -> Quad-Y = +0x1C2 (Boden-Sonde) fuer JEDE Entity.

### Port-Ist (Code gelesen)
- platform/pc/main.c:9859 `int32_t nsh_y = npc->y;` — Quad-Hoehe = KOERPER-Y; Ausnahmen nur Hund (Runde 35 H),
  NPC-Kante (Runde 38 M), Wasserzombie (Runde 38 U), Kraehe. Der Gorilla (0x27) und der kleine Affe (0x29) fallen
  auf npc->y -> beim Sprung (y steigt bis zu +0x1ba - 1600 im Rear-Up / Ballistik) haengt der Blob unter dem Koerper.

### Umsetzung
- Haken (1 Zeile) platform/pc/main.c hinter `re15_wasserzombie_schatten_y`: `re15_r39_schatten_y(npc, &nsh_y)` ->
  fuer 0x27 und 0x29 Quad-Y = dog_floor_y (+0x1ba, @0x80116f30 / @0x80110c78); Hund (Runde 35 H), Kraehe, NPC-Kante,
  Wasserzombie behalten ihre Wege. Mess-Schiene RE15_R39_LOG=1 -> gegner_r39.log: je gezeichnetem Bild mit
  Koerper-Y != Boden-Y eine Zeile `schatten slot=.. koerper_y=.. boden_y=.. quad_y=..`.
- Geprueft, nicht geaendert: Hund 0x20 (Runde 35 H, +0x1ba @0x8010d91c / RE2 +0x1C2 @0x800268c8), RE2-Spinne 0x25
  (Sprung Sub 7 P5 @0x80100E50 ist waagerecht: Thrust + Vorschub, kein +0x38-Schreiber; Decken-/Wasserwege fuehren
  +0x1C2 selbst, Runde 38 R) — kein Luft-Schatten-Fall.
- **Szenen-Gorillas (grid 0x30) — Original-Lage (Nachbesserung 1, ersetzt den Root-Skip des Zwischenstands c3823fa4)**:
  kein Sonderweg. Der Schatten-Schwanz laeuft auf jedem Pfad der Wurzel (@0x80116f20-38; Root-Skip @0x80116df4-f8 springt
  nur ueber Gehirn-Dispatch + Push-out auf 0x80116e88), der Quad liegt auf +0x1ba = -3600 mit Ecken 1700/1800 (INIT
  @0x801171e8/f0 im Spawn-Bild: Sce_em_set loescht Bit 0x20 @0x8004256c-70 vor dem `jalr` @0x8004259c, zurueck
  @0x80042604-08); ob er gezeichnet wird, entscheidet der Region-Cull des Cuts in FUN_8001b064 (`lw a1,DAT_800ac790`
  @0x8001b180 / `jal 0x80014368` @0x8001b224 / `beq` @0x8001b22c), den der Port schon traegt (main.c npc_region_culled).
  Entfernt: `re15_r39_schatten_aus` (gegner_r39.c, re15_gegner_r39.h) und die Hakenzeile in main.c.

## Messungen (Riegel test_r39_gegner, jeder Teil misst "vorher" = Haken aus und "nachher" im selben Lauf)

### Punkt 1 (unit_r39_gegner_affen_hp / affen_zucken / affen_schuss, ROOM11C0 Kampf-Layout, Gorilla geparkt Sub 0)
- vorher: HP 180/180 (Tabelle); jeder Pistolentreffer (Resolver-Schreibsatz, 12) -> Clip 7 (12 Bilder), Exit Sub 3/3/7:
  `hp 180->168 Clip7=1 Sub 3 | 168->156 Clip7=1 Sub 3 | 156->144 Clip7=1 Sub 7`. Echter Schuss (Pad R1 + Kreuz,
  re15_player_weapon_fire): dieselben drei Flinches, je -12.
- nachher: HP 360/360 (ROOM1141: 180). Treffer 1/2: KEIN Clip 7, Zustand 1 / Sub 0 bleibt (Merker), je ein
  Blut-Effekt (fx 0->1->2), +0x93 = 0; Treffer 3 (36 Energie): Clip 7, 12 Bilder, Exit Sub 7 (Sprung; Runde 35:
  3 Treffer -> Sprung); HP 360 -> 324. Echter Schuss: `360->348 Clip7=0 | 348->336 Clip7=0 | 336->324 Clip7=1`.
  Schrotflinte (Zeile 8 = 38 >= 36): Reaktion sofort (Spur 1, Clip 8/9), 312 -> 274.
- **Echte exe (Fortsetzung 12:40), ROOM11C0 Kampf-Layout**: `RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240
  RE15_PLAYER_POS=-6729,-12800,1486 RE15_EQUIP=3 RE15_INPUT_SCRIPT=M1.5,MA0.1,M0.6,... (R1 halten, Quadrat-Flanken alle
  0,7 s) RE15_STATE_LOG RE15_R39_LOG`. gegner_r39.log: `affen init slot=2/3 raum=11c0 hp=360 (tabelle 180 @0x8011f514)`;
  `affen treffer slot=2 hp=348 akku=12 -> OHNE ZUCKEN (zurueck in 1/5/1/0)` / `hp=336 akku=24 -> OHNE ZUCKEN (1/3/1/0)` /
  `hp=324 akku=36 -> FLINCH (Schwelle 36)` / `hp=312 akku=12 -> OHNE ZUCKEN (1/7/1/0)`. state.log Gorilla Platz 2:
  F335 hp 348 st=1/5 mo=18, F379 hp 336 st=1/3 mo=5, **F440 hp 324 st=2/3 mo=7 (Clip 7 F440-F452 = 13 Bilder)**, F462 hp
  312 st=1/7 mo=20 (Vergeltungs-Sprung nach dem Flinch, danach wieder ohne Zucken). Schuesse (pad 8800 = R1+Quadrat)
  F335/356/377/398/419/440/461/482, Treffer bei F335/F379/F440/F462 (die uebrigen gingen am anlaufenden Gorilla vorbei).

### Punkt 2 (unit_r39_gegner_hund_riegel; zwei RE2-Hunde im Flug am Spieler, echte EM020-Bank, Kiefer 600 vor dem Spieler)
- vorher: Bild 1: `hp 100 -> 80 (Hund 1) -> 60 (Hund 2)` — beide beissen IM SELBEN BILD, PL+0x1D3 = 0x00 (kein Riegel)
  = der Nutzer-Befund.
- nachher: `hp 100 -> 80 (Hund 1) -> 80 (Hund 2)`, PL+0x1D3 = 0x80, Hund 2 ohne Kontakt (+0x21E = 0); waehrend
  +0x93&1 bleibt der Riegel; nach dem Loeschen von +0x93 (NORMAL-Prolog @0x80031964 = RE2 @0x8003f8b4) ist er frei
  und ein Hund beisst wieder (-20); ein fremder Riegel (Zombie-Griff) wird vom Hunde-Tick nicht geloescht.
- Echte exe ROOM1190 (Basislauf c71fa144, RE15_DEBUG_JUMP=1190@250 + RE15_FORCE_EVENT=10@400, Leon steht am Spawn):
  6 Bisse F685/720/749/829/942/1000 (je -20), Abstaende 35/29/80/113/58 Bilder — in DIESEM Lauf kein Paar innerhalb
  der 22 Flinch-Bilder; der parallele Fall entsteht, wenn zwei Hunde im selben Flug treffen (Unit-Riegel oben).
  Der RE1.5-Flavor-Hund prueft schon im Original +0x93 == 0 vor dem Biss (`acae7==0` @0x8010f294) — dort gab es den
  Doppelbiss nie; der Nutzer spielt RE2-KI.
- **RE1.5-Flavor gemessen (unit_r39_gegner_hund_re15, Fortsetzung)**: zwei RE1.5-Hunde in BITE Sub 8 am Spieler, Haken aus
  und an: `erster Abzug in Bild 12, hp=90, acae7=0x01, Hund 1 Sub 8/5 (Recoil), Hund 2 Sub 8/1` — nur EIN Abzug (-10
  @0x8010f2c4-d0), Niederschlag setzt acae7 |= 1 (@0x8010f338-44), 5 weitere Bilder kein zweiter Biss (`lbu acae7 / bne`
  @0x8010f294). Port enemy_ai_common.c case 8 (`pl->hit_react == 0 && re15_dog_arc(...)`), kein Haken dieser Spur beteiligt.
- **Echte exe ROOM1190 (integration_r39_gegner_hunde1190, gruen 12:27)**: Bisse F685/F720/F753/F833 (je -20), Abstaende
  35/33/80 >= 24; gegner_r39.log `hund riegel GESETZT hp=80 / FREI hp=80 / GESETZT 60 / FREI 60 / GESETZT 40 / FREI 40 /
  GESETZT 20 / FREI 20` (4 Paare = 4 Bisse); die Hunde greifen nacheinander an (F685 Hund 1 Sub 3, F710 Hund 2 Sub 3).
  Gegen den Basislauf (F685/720/749/829/942/1000) verschieben sich der 3./4. Biss um 4 Bilder und der 5. um 100 Bilder:
  der jeweils naechste Hund kam waehrend der Reaktion nicht mehr an Leon heran.
- **Echte exe ROOM11C1 mit Elza (Fortsetzung 12:47; Parking Lot der Elza-Variante hat fuenf Hunde 0x20)**: Spielstand per
  probe_r36_elza_karte (Figur 4, Lage (-22604,14455)), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`: Bisse
  F779/F862/F906/F947/F1008/F1040 (je -20), Abstaende 83/44/41/61/32 >= 24; `hund riegel GESETZT/FREI` je Biss (hp 80, 60, 40,
  20, 0). Elza stirbt mit dem 6. Biss (F1040, hp -20) — fuenf Hunde ohne Gegenwehr; der Riegel laesst waehrend jeder
  Reaktion keinen zweiten Biss zu, er macht den Spieler nicht unsterblich.

### Punkt 3 (unit_r39_gegner_kriecher_1010 / _1220, RE2-KI, echte RDTs, RE2-Bank EM010/EM016)
- ROOM1010 Cut 4 (Suedtuer, Spawns @0x0A12/@0x0A26): Kriecher (950,-1700). Spieler fern am Tuer-Ziel (dist 3510,
  im Sichtkegel): liegt (Wort 0x201, Clip 23). Spieler 2500 davor (arc 0): **vorher** 120 Bilder lang Sub 2 / Clip 23,
  Weg 0 (RE2-WAIT, nur der Griff-Wecker < 1300 — der Nutzer-Befund); **nachher** im ersten Bild Wort 1 (RE1.5 Block B),
  Kriech-Lokomotion Clip 5, Weg 1653 Einheiten in 120 Bildern bis auf 849 an den Spieler (dann RE2-Griff Sub 1 =
  byte-true Kriecher-DECIDE[0]).
- ROOM1220 Cut 0 (sub00 `Switch 0x0a` Case 0 @0x00F24, Spawns @0x00F2A/@0x00F3E, Typ 0x16): Kriecher (-23500,-9700)
  (der erste liegt vom Spawn-Schub bei (-25398,-6739)); dieselbe Messung: vorher Weg 0, nachher Weg 1657.
  ROOM1011/1221 (Elza) tragen dieselben Spawn-Zeilen (@0x00ACE/@0x00AE2 bzw. identische sub00) — derselbe Code-Pfad
  (re2z_crawl, typ-unabhaengig).
- **RE1.5-Flavor gemessen (unit_r39_gegner_kriecher_1010_re15 / _1220_re15, Fortsetzung)**: fern `st=1/5 mo=0x0c` (DECIDE[5]
  FUN_8010466C, Sub 5); nah (dist 2495, arc 0) `Wort 1 nach 0 Bildern, Kriech-Clip 0x1A, Weg 1360 bzw. 1373` in 120 Bildern,
  vorher wie nachher (Block B ist dort nativ, Haken unbeteiligt).
- **Echte exe ROOM1010 ueber den echten Weg (Fortsetzung 12:46)**: Debug-Sprung nach 1010 traegt Eintritts-Case 0 (sub00
  `Switch 0x0a` @0x009D8: Case 0 @0x009DC spawnt zwei stehende 0x10, die 0x81-Kriecher liegen in **Case 4** @0x00A0C =
  Suedtuer). Deshalb: `RE15_DEBUG_JUMP=1020@240 RE15_FIRE_AOT=0@60#1020` (ROOM1020 Door_aot_set 0 @0x01C82 `.. 00 01 04 ..`
  -> ROOM1010 Cut 4, Ziel (3650,-3950) rot 2048) + `RE15_INPUT_SCRIPT=R0.25,U2` ab Bild 420. state.log: nach der Tuer beide
  Kriecher `st=1 ss1=2 g=81 mo=23` bei (950,-1700) r0 und (-50,-3800) r3584 (liegend, RE2-WAIT); Leon laeuft (F430
  rot 2816), **F440: `kriecher weckt slot=3 typ=10 dist=2996 -> Lokomotion`**, Kriecher A `ss1=0 mo=5` kriecht
  (950,-1700) -> (1195,-1732) F440 -> (1630,-1889) F450 -> (2161,-1951) F460 = Griff `ss1=1 mo=14/15/16`, Leon hp
  100 -> 95 (F480) -> 90 (F500) -> 85 (F540), F600 Kriecher A `st=7` (Kopf zertreten, RE2-Griffausgang). Kriecher B
  (dist > 3000, ausserhalb des 0x200-Kegels) liegt bis zum Ende — wie Block B es vorsieht.
- **Echte exe ROOM1220 mit Leon (Debug-Sprung 1220@240, Case 0 = die Kriecher-Spawns, `RE15_PLAYER_POS=-23500,-7700`)**:
  `kriecher weckt slot=1 typ=16 dist=2126` und `slot=2 typ=16 dist=1999` (beide < 0xbb8, im Kegel), Leon hp 100 -> 95
  (F35) -> 90 -> 85 -> 80 -> 75 (Kriecher-Griff), F260 Kriecher 2 `st=7` (zertreten), Kriecher 1 `ss1=1` (Griff).
- **Echte exe ROOM1221 mit ELZA (Spielstand probe_r36_elza_karte 1221 (-23500,-7700) rot 2048, CONTINUE)**: F0 beide
  `st=1 ss1=2 mo=23` (liegend) bei (-25398,-6739) / (-23500,-9700); **F5 beide `ss1=0 mo=5`** (`kriecher weckt`
  dist=2126 / 1999), Kriecher 2 kriecht (-23500,-9700) -> (-23500,-9374) F10 -> (-23494,-8890) F20 = Griff `ss1=1`,
  Elza hp 100 -> 95 (F40) -> 90 -> 85 -> 80 -> 70; F180 Kriecher 2 `st=7`, Kriecher 1 greift (F180 `ss1=1`), F300 `st=7`.
- **Echte exe ROOM1011 mit ELZA ueber den echten Weg (Fortsetzung 12:58)**: Spielstand probe_r36_elza_karte 1021
  (-2750,-7250) rot 0 + `RE15_FIRE_AOT=0@60#1021` (ROOM1021 Door_aot_set 0 @0x01C82 -> 1011 Cut 4) + `RE15_INPUT_SCRIPT=
  R0.25,U2` ab Bild 300: nach der Tuer beide 0x81 `st=1 ss1=2 mo=23` (liegend) bei (950,-1700) / (-50,-3800); **F320
  `kriecher weckt slot=4 typ=10 dist=2996`**, Kriecher A `ss1=0 mo=5` (1195,-1732) F320 -> (1630,-1889) F330 -> Griff
  F340 (`ss1=1 mo=14`), Elza hp 100 -> 95 (F360) -> 90 (F400) -> 85 (F450), F500 Kriecher A `st=7`; Kriecher B liegt.
  Bildgleich mit Leons 1010-Lauf (gleiche Spawn-Zeilen @0x00ACE/@0x00AE2).

### Punkt 4 (Basislauf c71fa144, ROOM11C0 Szene, RE15_FRAMEDUMP 1340-1400/5)
- state.log: Gorilla Platz 3 in Sub 7 (Fernsprung) F1352 fuer 39 Bilder. Framedumps F1355/F1365/F1375/F1385
  (E_gegner_bilder/vorher_sprung_F*.png): der Schatten-Quad des springenden Gorillas haengt in der Luft unter dem
  Koerper (F1365: Quad links oben in Koerperhoehe, der stehende Gorilla in der Mitte hat seinen auf dem Boden).
- unit_r39_gegner_schatten: 0x27 mit Koerper -5200 ueber Boden -3600: vorher Quad-Y = -5200, nachher -3600; 0x29
  ebenso; Hund/Zombie unveraendert. ROOM11C0 Kampf-Variante (`44 01 27 10 00 ..` pc[4] = 0): +0x1ba = 0 = Quad-Y.
  Nachbesserung 1: grid 0x30 (Wagen y=-2500 / geparkt y=-20000) -> Quad auf +0x1ba = -3600 wie im Original-RAM; ROOM11C0-
  Szene: Zustand 1 (INIT im Spawn-Bild); Region-Viereck des RDT je Cut 0/1/3/4/5/12/13 == DAT_800ac790 der Savestates,
  Punkt-im-Viereck fuer (-1220,-21568)/(-554,-25423) = Original-RAM (Cut 0/1/3/4/13: 0/0, Cut 12: 1/0, Cut 5: 1/1), die
  beiden Port-Fassungen des Tests (re15_aot_point_in_quad / _fun80014368) gleich; Kampf-Layout grid 0x10 -> Schatten auf 0.
- **Echte exe (integration_r39_gegner_schatten, gruen 12:27, Lauf lauf_5613f23f)**: Sprung-Bilder F1362-F1385 (24 Zeilen
  `schatten F.. slot=3 grid=10 st=1/7 koerper_y=-720..-4680 boden_y=0 quad_y=0`), alle Quad-Y == Boden-Y. (Die 595
  `schatten_aus`-Zeilen dieses Laufs gehoerten zum entfernten Root-Skip-Haken; Stand nach Nachbesserung 1 s. dort.) Framedump
  `E_gegner_bilder/nachher_sprung_F1370.png` (Gorilla in der Luft ueber dem Auto, der Schatten-Blob liegt auf dem
  Asphalt) gegen `vorher_sprung_F1365.png` (Blob in Koerperhoehe).

## Tests (Namen, Ergebnis)
- Nachbesserung 2: test_r39_gegner_1141.c (unit_r39_gegner_1141_hp1141 / _zucken1141 / _ablauf1141 / _affe29),
  integration_r39_gegner_1141 (test_r39_gegner_1141.cmake), test_maggot_ai.c mit Haken aus — Laeufe/Summenzeilen in
  "Tests (Nachbesserung 2, ...)".
- tests/unit/probes/r39_gegner.cmake -> test_r39_gegner.c, Teile: unit_r39_gegner_affen_hp, _affen_zucken, _affen_schuss,
  _hund_riegel, _hund_re15, _kriecher_1010, _kriecher_1220, _kriecher_1010_re15, _kriecher_1220_re15, _schatten;
  Integration: integration_r39_gegner_schatten (echte exe ROOM11C0, Sprung + Original-Lage der Szenen-Gorillas, Nachbesserung 1),
  integration_r39_gegner_hunde1190
  (echte exe ROOM1190, vier Bisse, Riegel-Protokoll, Leon lebt).
- Beruehrte Testgruppen des Diffs: r35_affen (Gorilla-Original-Riegel, test_r35_affen.c mit `re15_r39_gegner_enable(0)`).
- Zwischenstand 12:34 (`ctest -R "unit_r39_gegner|unit_r35_affen"`): `100% tests passed, 0 tests failed out of 30`;
  12:27 (`ctest -R "integration_r39_gegner"`): `100% tests passed, 0 tests failed out of 2`.
- **Nachbesserung 1, Abschlusslauf 13:55** (`ctest --test-dir re15_port/build -R "r39_gegner|r35_affen|schatten|affen3" --timeout 900
  --output-on-failure`): **`100% tests passed, 0 tests failed out of 49`**; davor `-R "^unit_r39_gegner_schatten$"`:
  `100% tests passed, 0 tests failed out of 1`. Details im Abschnitt "Nachbesserung 1".
- **Abschlusslauf 13:02 (Stand c3823fa4)** (`ctest --test-dir re15_port/build -R "r39_gegner|r35_affen" --timeout 900 --output-on-failure`):
  **`100% tests passed, 0 tests failed out of 32`** (20 x unit_r35_affen_*, 10 x unit_r39_gegner_*, integration_r39_gegner_
  schatten 58,8 s, integration_r39_gegner_hunde1190 42,3 s). Riegel-Protokoll des Abschlusslaufs 1190 (mit Bildnummern):
  `GESETZT F685 / FREI F708`, `GESETZT F720 / FREI F743`, `GESETZT F753 / FREI F776`, `GESETZT F833 / FREI F856` — der
  Riegel steht je Biss genau 23 Bilder (22 Bilder Flinch cmd 2 + das Exit-Bild, Freigabe mit dem NORMAL-Prolog
  @0x80031964 = RE2 @0x8003f8b4).

## OFFEN
- Nachbesserung 2: Befund E (Gorilla/Elza durch 2000 dicke Waende, Original-Mechanik), RE15_R39_LOG-Namensgleichheit mit
  Spur F, Abhaengigkeit des 1141-Exe-Riegels vom Runde-38-F-Ablauf — Details "OFFEN (Nachbesserung 2)".
- (Nachbesserung 1, Hinweis, kein Punkt dieser Spur) DAT_800ac790 ist beim Raumeintritt bis zum ersten Cut-Apply stale
  (@0x80021c0c schreibt es, der Eintritt resolviert nur DAT_800ac794); das Original cullt in diesem Fenster Schatten und
  NPC-Zeichnung mit dem alten Viereck (bei 11C0 s000/s001: Garbage-Viereck, Spieler aussen). Der Port nimmt am Eintritt das
  echte Cut-0-Viereck. An den Savestates nicht messbar (Einblendung); fuer die Gorillas ergebnisgleich.
- (Nachbesserung 1) Kleiner Affe 0x29 (STAGE3/4) mit grid&0x20: der Quad liegt jetzt wie im Original auf +0x1ba (STAGE3-
  Schwanz @0x80110c68-80 auf jedem Pfad); ob in einer 0x29-Szene mit geparkten Affen ein Quad im Bild liegt, das die Masken
  des Ports anders verdecken als das Original, ist nicht an der exe gemessen (kein Nutzer-Raum dieser Spur).
- Der "parallele" Biss in ROOM1190 liess sich am skriptierten Basislauf (Leon steht am Spawn) nicht reproduzieren — die
  Hunde kommen dort nacheinander (Abstaende >= 29 Bilder) durch die Luke. Vorher/nachher am ECHTEN Weg unterscheiden sich
  nur in der Verschiebung der Bisse 3-5 (749->753, 829->833, 942->1042); den Doppelbiss im selben Bild misst der Unit-Riegel
  (zwei Hunde im Flug: vorher -40, nachher -20). Der Nutzer-Fall (zwei Hunde treffen in Leons Flinch) ist damit mechanisch
  abgedeckt (Kontakt-Tor @0x80104e5c-68 + Flug-Tor @0x80104d84-94 am Byte PL+0x1D3), nicht als Spielszene nachgestellt.
- RE1.5-KI-Flavor nur im Riegel gemessen (unit hund_re15, kriecher_*_re15 mit echten RDTs/Baenken), die Exe-Laeufe im
  RE2-Flavor (Standard des Nutzers). Gorilla (RE1.5-Gehirn in beiden Flavors) und Schatten sind flavor-unabhaengig.
- Im Debug-Sprung-Lauf 1220 (Leon) fehlten dem state.log in den ersten 240 Bildern die Gegner-Spalten (pm=2 aus der
  Startraum-Szene); die Wecker-Zeilen und Leons HP-Verlauf sind vorhanden, der Elza-Lauf 1221 per CONTINUE ist vollstaendig.
- Treffer-Schwelle fuer andere Waffen folgt dem Schaden (Orchestrator-Vorgabe "entsprechend ihrem Schaden"): Messer (7 je
  Treffer, @0x8006ee3c) flincht erst beim 6. Treffer (42 >= 36); Schrotflinte 38/50, Magnum 40: sofort. Krit-Treffer
  (+0x93 & 0x40) flinchen immer (>= 38).

## Fuer den Nutzer
- Neue Assets / Sprachdateien / Paket-Gate: keine (nur Code + Tests; shared_assets unveraendert).
- Bedienung (Stand Nachbesserung 2): ALLE Gorillas (Leons Parking Lot ROOM11C0 und Elzas Briefing Room ROOM1141 — die
  einzigen Raeume mit Gorillas; Elzas Parking Lot 11C1 hat Hunde) haben 360 statt 180 Energie und zucken erst beim 3.
  Handfeuerwaffen-Treffer (36 Energie seit dem letzten Zucken; Leons Pistole und Elzas SIG P228 je 12); jeder 3. Treffer ist
  damit zugleich der Vergeltungs-Sprung des Originals (3-Treffer-Zaehler @0x8011b194-98). Dazwischen: Blut + Treffer-Ton,
  der Gorilla laeuft weiter. Schrotflinte und Magnum zucken sofort (>= 36), Messer erst beim 6. Treffer. Ein Gorilla braucht
  31 statt 16 Handfeuerwaffen-Treffer. Die kleinen Affen (Stage 3-5) sind unveraendert.
- ROOM1141 (Elza): der Gorilla-Kampf dauert damit gut doppelt so lange (62 P228-Treffer fuer beide). Munition liegt auf
  Elzas Weg reichlich (H.GUN BULLETS zusammen 405 Schuss, dazu Remington, Beretta, Ingram, MC51); der Ausweg ueber das
  Dachschloss (Original) bleibt. Hinweis: in langen Kaempfen kann ein Gorilla (oder Elza im Pin) durch eine Wand gedrueckt
  werden (Befund E, Original-Mechanik, wird weiter untersucht).
- Hunde (RE2-KI): waehrend der eigenen Treffer-Reaktion (22 Bilder Flinch, Niederschlag) beisst kein zweiter Hund; danach
  sofort wieder. Gilt in allen Hunde-Raeumen (1190, 11C1, ...). Der toedliche Biss und das Fressen sind unveraendert.
- Liegende Zombies (1010/1011, 1220/1221): sie liegen still, bis der Spieler naeher als 3000 im 90-Grad-Sichtkegel vor ihnen
  steht (RE1.5-Original Block B), dann kriechen sie heran; naeher als 1300 greifen sie wie im RE2-Original.
- Entscheidungsfrage (erledigt, Nutzer-Nachtrag 3 2026-10-18: "Zu 2. ja, wir machen das Gorilla Verhalten generell so.")
  -> Nachbesserung 2: Regel fuer alle 0x27.

## Nachbesserung 1 (2026-10-18, nach Abnahme 0 — Mangel M1)

### M1 — der Root-Skip-Haken trug eine am Byte falsche Mechanismus-Behauptung
- **Ursache**: Befund B deutete `lbu v0,9(v1)` @0x80116dec / `andi v0,v0,0x20` @0x80116df4 / `bne v0,zero,0x80116e88`
  @0x80116df8 als Sprung ueber die ganze Wurzel samt Schatten-Schwanz. Selbst nachdisassembliert (`re15_disasm.py dis
  0x80116db8 104`, STAGE1.BIN): das Sprungziel 0x80116e88 (`lui a1,0x800b` / `lw a1,g_entity` @0x80116e8c) liegt IN der
  Funktion; von dort prueft @0x80116e94-ea0 grid&0x40 (-> 0x80116f20) bzw. rechnet @0x80116ea8-f1c die +0x1d8-Formel, und
  der Schwanz @0x80116f20-38 (`lw v0,g_entity` @0x80116f24 / `lh a1,442(v0)` @0x80116f30 / `jal 0x8001b064` @0x80116f34 /
  `addiu a0,a0,176` @0x80116f38) laeuft auf JEDEM Pfad — auch ueber das Pause-Bit `bne` @0x80116dd8. Uebersprungen werden
  nur der Gehirn-Dispatch (`jalr` @0x80116e20, Tabelle 0x801213c8) und Push-out/Kollision (@0x80116e28-e84: 0x8002b498,
  0x8002aec4, 0x8002b544, 0x8003b0a4). STAGE3 0x29 ebenso: `bne` @0x80110b40 -> 0x80110bd0, Schwanz @0x80110c68-80
  (`lh a1,442(v0)` @0x80110c78 / `jal 0x8001b064` @0x80110c7c). Die Abnahme hat recht; die falsche Aussage stand in
  gegner_r39.c, re15_gegner_r39.h (4), main.c-Haken, Dossier Befund B / Punkt 4 und Riegel (d).
- **INIT im Spawn-Bild** (warum die Szenen-Gorillas trotz grid 0x30 in Zustand 1 stehen): Sce_em_set schreibt das grid-Byte
  zuerst OHNE Bit 0x20 (`andi v0,v1,0xdf` @0x8004256c / `sb v0,9(s0)` @0x80042570), ruft die Wurzel (`jalr` @0x8004259c,
  s4 = v1&0x20 @0x800425a0) und setzt das Bit danach zurueck (`or v0,v0,s4` @0x80042604 / `sb v0,9(s0)` @0x80042608).
  Der Port macht es genauso (enemy_ai_common.c re15_enemy_spawn_root, Runde 35 J); die Ecken 1700/1800 schreibt der INIT
  (@0x801171e8/f0, Port re15_r36_affen_lache_init).
- **Original-RAM (Runde-35-Savestates orig_scene/r3, Pool 0x800acc2c Stride 0x1f4, Cut 0x800b0fe4, Zone DAT_800ac790 ->
  20 Byte mit Ecken @+4..+0x12; eigene Lesung scratchpad e_nb1/zone_read.py mit re15_ss.Ram)**:

  | Savestate | Cut | Gorilla 1 (slot1) | Gorilla 2 (slot2) | +0x1ba | +0xbc/+0xbe | Zone (x,z) x4 | FUN_80014368 G1 / G2 |
  |---|---|---|---|---|---|---|---|
  | s001 t=6.11 | 0 | 1/0/0/0 grid 30 (-1220,-20000,-21568) | 1/0/0/0 grid 30 (-554,-20000,-25423) | -3600 | 1700/1800 | (1530,0)(17586,0)(10768,-32750)(0,-32590) | 0 / 0 |
  | s002 t=7.64 | 1 | dito | dito | -3600 | 1700/1800 | (-30301,4702)(-28200,27002)(1800,29703)(2603,4502) | 0 / 0 |
  | s004 t=10.69 | 3 | dito | dito | -3600 | 1700/1800 | (-29901,-20799)(-29300,19505)(-2098,19505)(-3693,-20298) | 0 / 0 |
  | s005 t=12.19 | 4 | dito | dito | -3600 | 1700/1800 | (-30500,-28901)(-29700,19803)(-2897,20704)(-2899,-29900) | 0 / 0 |
  | s007-s019 | 13 | dito | dito | -3600 | 1700/1800 | (-24200,-18102)(-24502,5700)(12901,5200)(13004,-17901) | 0 / 0 |
  | s020 t=34.89 | 12 | (-1220,-2500,-21568) im Wagen | (-554,-2500,-25423) | -3600 | 1700/1800 | (-23700,-21102)(-24502,-1300)(5401,-1300)(5504,-25401) | **1** / 0 |
  | s021 t=36.41 | 12 | (-3617,0,-17798) grid 10 | (-1220,-2500,-21568) im Wagen | 0 / -3600 | 1700/1800 | dito | 1 / **1** |
  | s022-s026 | 5 | (-3617,0,-17798) | (-1220,-2500,-21568) im Wagen | 0 / -3600 | 1700/1800 | (-21767,-31201)(-18463,-3298)(18004,-3300)(20301,-32700) | 1 / **1** |

  Der Mechanismus, der im Original ueber den Quad entscheidet, ist der Region-Cull in FUN_8001b064 selbst: `lw a1,DAT_800ac790`
  @0x8001b180, `jal 0x80014368` @0x8001b224, `beq v0,zero,0x8001b364` @0x8001b22c — Punkt-im-Viereck ueber (x,z) der Entity
  (FUN_80014368 liest `lw v0,8(a0)` = z @0x80014370 und `lw v0,0(a0)` = x @0x80014384, die Hoehe nie; Zonenecken `lh 4/6/8/10/
  12/14/16/18(a1)`). Geparkt (Cuts 0/1/3/4/13) also kein Quad; im Wagen (Cut 12, Cut 5) ein Quad auf -3600 mit Ecken 1700/1800.
  (DAT_800ac790 schreibt nur der Cut-Apply @0x80021c0c aus FUN_80014324 — Port: re15_rdt_get_region_quad.)
- **Messung vorher (Port, Stand c3823fa4)**: Root-Skip-Haken aktiv — 595 `schatten_aus`-Zeilen (kein Quad fuer grid 0x30),
  darunter die Cut-12-/Cut-5-Bilder, in denen das Original den Quad ausgibt (Abweichung vom Original, wenn auch unsichtbar).
- **Messung: Port ohne Root-Skip, Quad auf +0x1ba (Messvarianten mess1/mess2, nicht committet; Aufbau wie
  integration_r39_gegner_schatten, in mess2 dazu eine temporaere main.c-Logzeile der Bildschirmecken nsx/nsy)**: 404 Quad-
  Zeilen der grid-30-Gorillas, nach Cut (state.log `cam` je Bild): cam 5 direkt nach dem Debug-Sprung F185-F212 beide geparkt
  (28+28 — ein Debug-Sprung-Artefakt: im Original ist vor Cut 13 nie Cut 5 aktiv, Reihe 0 -> 1 -> 3 -> 4 -> 13), cam 13 nur das
  Cut_chg-Bild F213 (altes Viereck, wie das Original im Anforderungsbild), cam 12 F764-F798 Gorilla 1 im Wagen (35) und
  F800-F879 Gorilla 2 im Wagen (80; davor bei (-554,-25423) aussen — wie s020), cam 5 F880-F1110 Gorilla 2 im Wagen (231).
  Der Port-Cull (re15_rdt_get_region_quad / re15_aot_point_in_quad) trifft also je Cut dieselbe Entscheidung wie FUN_80014368.
  Bildschirmecken (320x240, cx/cy = 160/120): Cut 12 Band sx 120..214 / sy 107..130 = Klappenoeffnung (schwarzer Innenraum,
  Gorilla-Koerper, davor die Heckklappe Objekt 0); Cut 5 sx -173..-22 (Wagen-Position) bzw. -441..-128 / -692..-299
  (geparkt) = links ausserhalb des Bildes. Framedumps F785/F860 (Cut 12) und F935/F1010 (Cut 5) gegen die Originalbilder
  s020/s021 und s024/s026 (3x vergroessert, scratchpad e_nb1/): kein Blob auf dem weissen Klappenrahmen, keine dunkle Scheibe
  ueber den Wagen — Port und Original gleich. Die "zwei neuen sichtbaren Fehler" aus Befund B waren Logzeilen, keine Pixel.
- **Aenderung**: `re15_r39_schatten_aus` gestrichen (gegner_r39.c, re15_gegner_r39.h, main.c-Hakenzeile weg; main.c-Diff
  dieser Spur damit nur noch der +0x1ba-Haken); `re15_r39_schatten_y` setzt den Quad fuer 0x27/0x29 IMMER auf +0x1ba (auch
  grid&0x20) = die Original-Lage; Kommentare tragen die wirklichen Bytes (Schwanz auf jedem Pfad, Region-Cull
  @0x8001b180/b224/b22c, INIT im Spawn-Bild @0x8004256c-70/@0x8004259c/@0x80042604-08, Ecken 1700/1800 @0x801171e8/f0).
  Riegel: unit_r39_gegner_schatten prueft die Original-Lage (grid 0x30 -> -3600; ROOM11C0-Szene: Zustand 1 + Quad -3600;
  Region-Viereck des RDT je Cut 0/1/3/4/5/12/13 == DAT_800ac790 der Savestates; Punkt-im-Viereck fuer beide Szenen-
  Positionen == Original-RAM, beide Port-Fassungen gleich); integration_r39_gegner_schatten (d1-d3) misst an der exe:
  grid-30-Quads auf +0x1ba (>= 10 auf -3600), Cut-13-Fenster leer, Cut-12-Fenster nur der Wagen-Gorilla.
- **Nebenbefund beim Riegelbau (Cut 0, Raumeintritt)**: der erste Entwurf des Unit-Riegels nahm fuer Cut 0 das Viereck aus
  DAT_800ac790 des Savestates s001 ([1530,0; 17586,0; 10768,-32750; 0,-32590], Zeiger 0x80121b80) — der Port-Loader liefert
  fuer Cut 0 aber (-30500,4500)(-28600,28692)(-10999,28800)(-2997,4200). Nachgelesen: FUN_80014324 (@0x80014324-64, erster
  20-Byte-Satz ab RDT+0x28 mit Byte+2 == Cut) liefert fuer Cut 0 den Eintrag @0x80121bd8 (`8f ff 00 00`, genau das Port-
  Viereck; DAT_800ac794 zeigt bei s000/s001 dorthin). DAT_800ac790 schreibt in der ganzen EXE nur der Cut-Apply FUN_80021bbc
  (`jal 0x80014324` @0x80021c00 / `sw v0,DAT_800ac790` @0x80021c0c), und der ist beim Raumeintritt vor dem ersten Cut_chg
  noch nicht gelaufen: 0x80121b80 ist der STALE Zeiger des vorigen Zustands, misaligned in die 11C0-Tabelle (die Anker
  0x80121c00/c8c/cdc/d18/ed0/ef8 liegen auf dem 20-Byte-Raster, 0x80121b80 = 0x80121c00 - 128 nicht). Fuer die Gorillas ist
  das Ergebnis gleich (beide Vierecke: 0/0); der Riegel traegt die echte Cut-0-Zone (@0x80121bd8) mit diesem Vermerk. Ob das
  Original in diesem Fenster (Raumeintritt bis zum ersten Cut_chg, s000/s001) auch den Spieler-Schatten mit dem stalen
  Viereck cullt (Spieler (-22604,14455) dort = 0), war an den Savestates nicht zu sehen — s000/s001 sind Einblend-Bilder
  (VRAM schwarz). Kein Punkt dieser Spur; als Hinweis fuer den Orchestrator unter OFFEN.
- **Messung nachher (gebaut 13:48, exe re15_pc.exe)**:
  - `ctest -R "^unit_r39_gegner_schatten$"`: `100% tests passed, 0 tests failed out of 1` — alle Pruefungen ok, darunter
    grid 0x30 im Wagen -> -3600; 0x29 grid&0x20 -> -3600; ROOM11C0-Szene: beide Gorillas grid 0x30, y=-20000, Quad -3600/-3600,
    Zustand 1/1; Region-Viereck Cut 0/1/3/4/13/12/5 == Original-RAM (Cut 0 = Anker @0x80121bd8), Punkt-im-Viereck
    Gorilla 1/2 = 0/0, 0/0, 0/0, 0/0, 0/0, 1/0, 1/1 in beiden Port-Fassungen; Kampf-Layout grid 0x10, Quad auf 0.
  - integration_r39_gegner_schatten (Lauf lauf_59e277d7, 13:52): 404 Quad-Zeilen der grid-30-Gorillas, alle quad_y == boden_y
    == -3600: cam 5 nach dem Debug-Sprung F185-F212 (28+28), Cut_chg-Bild F213 (1+1, altes Viereck), Cut 12 F764-F798
    Gorilla 1 im Wagen (35) + F800-F879 Gorilla 2 im Wagen (80), Cut 5 F880-F1110 Gorilla 2 im Wagen (231); Cut-13-Fenster
    F214-F762 leer; Cut_chg(13) F213 / (12) F763 / (5) F880; 24 Sprung-Zeilen grid 10 st 1/7 mit quad_y == boden_y == 0
    (Punkt 4 unveraendert). Keine `schatten_aus`-Zeile mehr (Haken entfernt).
  - `ctest --test-dir re15_port/build -R "r39_gegner|r35_affen|schatten|affen3" --timeout 900 --output-on-failure`:
    **`100% tests passed, 0 tests failed out of 49`** (20 x unit_r35_affen_*, unit_r35_raeume_hundeschatten,
    unit_r36_pri_schatten + integration_r36_pri_schatten, 11 x unit_r38_affen3_* + 3 integration_r38_affen3*,
    10 x unit_r39_gegner_*, integration_r39_gegner_schatten, integration_r39_gegner_hunde1190).
  - Messkopien: eigene `re15_pc_e_mess.exe` (Messvarianten) nach der Messung geloescht; die Messvarianten selbst sind nicht
    committet (Diff: nur die beiden Zeilen in gegner_r39.c bzw. die Logzeile in main.c, beide aus dem .orig restauriert).

## Nachbesserung 2 (2026-10-18, nach Abnahme 1 — Nutzer-Nachtrag 3)

Auftrag (Orchestrator, Mangel 1 der Abnahme 1): AUFTRAG.md Nachtrag 3 (2026-10-18 14:25) woertlich "Zu 2. ja, wir machen
das Gorilla Verhalten generell so." -> HP x2 und Flinch erst nach 3 Handfeuerwaffen-Treffern (Akku-Schwelle 36) fuer ALLE
Gorillas Typ 0x27 in allen Raeumen und beiden Szenarien; Raum-Tor `re15_r39_affen_parkplatz` auf den Typ allein; Zensus
aller 0x27-Raeume; ROOM1141 (Elza) Gorilla-Szene muss lauffaehig bleiben; kleine Affen 0x29 unveraendert.

### Protokoll
- 2026-10-18 Start: Baum sauber, HEAD 2486a028 (Merge master dc330054 in r39/gegner), `git diff master --name-only` =
  nur Pfade dieser Spur. Pflichtlektuere AUFTRAG.md (inkl. Nachtrag 3), VERTRAG.md, E_abnahme_1.md, dieses Dossier gelesen.

### Mangel 1 — die Gorilla-Regel galt nur in ROOM11C0 (Raum-Tor)

**Ursache**: `re15_r39_affen_parkplatz(e)` = `type 0x27 && g_current_room_id == 0x11C0` (Stand 2486a028) — nach dem
Wortlaut "beim Parking Lot" gebaut; Nutzer-Nachtrag 3 (2026-10-18 14:25) entscheidet "generell".

**Zensus Sce_em_set Typ 0x27, alle 240 RDTs beider Szenarien** (`scd_dump_room.py` je RDT, Feld 3 = Typ; Skript
scratchpad e_nb2/zensus27.sh): Treffer nur in
- ROOM11C0 (Leon, Parking Lot): @0x01784 `44 01 27 30 02 ..` / @0x01798 `44 02 27 30 02 ..` (Szene, grid 0x30),
  @0x017E8 `44 01 27 10 00 ..` / @0x017FC `44 02 27 10 00 ..` (Kampf, grid 0x10, Flag 4/0x40).
- ROOM1141 (Elza, Briefing Room): sub00 @0x00B1A `44 01 27 30 00 00 00 cb ..` / @0x00B2E `44 02 27 30 .. cc ..` (Szene,
  Zweig Ck(4,0xF0) @0x00AFE), @0x00B54 `44 01 27 10 .. cb 38 e6 .. b8 d9` = (-6600,-9800) / @0x00B68 `44 02 27 10 .. cc 52 ea
  .. da b2` = (-5550,-19750) (Kampf, Zweig Ck(4,0xF2) @0x00B4A). Der Port-Ablauf BEFREIUNG (Runde 38 F,
  gen/dining_r38_programme.inc Zeile 607-609) setzt dieselben beiden Szenen-Saetze.
- Gegenprobe 0x29 (kleine Affen): ROOM3040/3041, 4000, 4040/4041, 4080/4081, 5000/5001, 5031, 50A0/50A1, 50B0, 5100/5101.
- Vollstaendigkeit: der Walker brach nur in 34 RDTs ab — alle 34 sind Stummel zu 4 Byte (ROOM1270/1271, 20C0-20F1, 30F0/1,
  4060/1, 40C0-40F1, 5150-5171, 6050-6071). Roh-Byte-Gegenprobe (`44 ?? 27 <grid>` ueber alle Dateibytes): ausser den 8
  Saetzen oben nur Treffer in Nicht-SCD-Daten (ROOM11B0 @0xdefe `44 00 27 00 17 00 27 00 ..` = u16-Tabelle, ROOM1260/2000/
  2010/2020 @0x2xxxx `44 03 27 00 10 d0 0b ff ..` = Bilddaten), keiner in einer SCD-Region.
- Dispatch: Typ 0x27 -> Wurzel 0x80116db8 nur ueber die STAGE1-Registrierung `lui v0,0x8011 / addiu v0,v0,28088`
  @0x8011e900-04, `sw v0,11336(at)` = 0x80072c48 = 0x80072bac + 0x27*4 @0x8011e90c (re15_disasm.py dis 0x8011e900; im
  PSX.EXE-Abbild ist der Platz 0 = Laufzeit-Registrierung).
Damit sind "alle Gorillas" = die zwei in 11C0 (Leon) und die zwei in 1141 (Elza).

**RE-Belege, selbst gelesen (re15_disasm.py)**:
- HP-Zeile `read 0x8011f514 16 --w 2` = 16 x 180; INIT FUN_80116f50 `dis 0x80116f50 30` + `dis 0x801170b8 24`: Saat
  `lhu v0,0(v0)` @0x801170fc / `sh v0,154(a1)` @0x80117104, OHNE hp-Bedingung (einziger Zweig im INIT ist grid&1 `beq`
  @0x80116f80 -> Wort 0x101 @0x80116f88-8c). Der Port-INIT fuehrt `if (e->hp <= 0)`; Sce_em_set setzt hp = 0 (scd_vm.c
  op_sce_em_set "Leave hp at the struct-cleared 0"), im Spiel laeuft der Haken also bei jedem Spawn.
- Schaden Zeile 0x27 `read 0x8006ee38 22 --w 2` = [0,0,7,0,30,0,12,0,12,0,28,..] = Schritt 4 Byte je Waffe: Messer 7
  @0x8006ee3c, Pistole (Waffe 3) 12 @0x8006ee44, **Elzas SIG P228 (Waffe 4, Item 0x04 aus ROOM1051) 12 @0x8006ee48**.
  Die Schwelle 36 = 3 x 12 gilt damit fuer Leons und Elzas Handfeuerwaffe gleich.
- Spur-Tabelle `read 0x801214a8 22 --w 4`: Waffen 0-6, 12, 14, 19, 20 -> 0x8011b018 (Boden-Flinch Clip 7), 7/8/13/21 ->
  0x8011b1ec, 9-11/15-18 -> 0x8011b400 — die P228 flincht wie die Pistole auf Spur 0.
- Resolver `dis 0x80012500 12`: `lh v1,154(s1)` @0x80012520 / `ori v0,zero,0x2` @0x80012524 / `bgez v1,0x80012420`
  @0x80012528 / `sb v0,4(s1)` @0x8001252c — hp >= 0 bleibt HURT (2), erst hp < 0 ist der Tod. Ein Gorilla braucht mit
  12er-Treffern 180/12 + 1 = 16 (Original) bzw. 360/12 + 1 = 31 Treffer (Nutzer-Vorgabe).

**Messung vorher (exe Stand 2486a028, neu gebaut; Elza, echter Weg ROOM1131 -> Tuer -> ROOM1141)**: Aufbau wie
integration_r38_dining1141_d bis zum Kampf (RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78, RE15_GIVE=0x43:1,0x04:15,
0x15:120, RE15_EQUIP=4, RE15_DEBUG_JUMP=1131@gp, RE15_PLAYER_POS=-1300,-13900,0, RE15_WEGPLAN 1131 Schluessel/Tuer -> 1141
Phase H -> Gemaelde; danach RE15_INPUT_SCRIPT `M1.5` + 40 x `MA0.1,M0.6` ab Spielbild 2760, RE15_PSELECT_AUTO(_SWITCH)=1,
RE15_R39_LOG=1, RE15_EXIT_AT=3700; Lauf scratchpad e_nb2/v1141_vorher, Skript e_nb2/lauf.sh, Auswertung e_nb2/auswert.py):
- debug.log: `[messer] Startinventar Charakter 4`, `[give] item 4 x15`, `[equip] RE15_EQUIP -> item 4 (slot 1)`, `[dining]
  F283 ROOM1141 Phase H`, `F854 Gemaelde -> Programm BEFREIUNG`, `[elzaems] Typ 0x27 aus EMD/CDEMD1.EMS Sektor 893`.
- gegner_r39.log: `affen init slot=2/3 raum=1141 hp=180` (Raumaufbau, danach nimmt Phase H sie heraus) und erneut slot 2/3
  hp=180 (BEFREIUNG-Spawn).
- state.txt: Steuerung zurueck ~F2632, Gorillas jagen ab F2800; Schuesse F2805 ... (Magazin 15 -> 0, Nachladen); Treffer
  **F3163 G2 180 -> 168 Clip 7**, F3498 168 -> 156 Clip 7, F3520 156 -> 144 Clip 7, F3542 144 -> 132 Clip 7, F3603 G3
  180 -> 168 Clip 7, F3625 168 -> 156 Clip 7 — **jeder P228-Treffer flincht, HP 180** (ROOM1141 hatte die Regel nicht).
  Elza 100 -> 88 (F3241, schwerer Biss -12) -> 82 -> 76 -> 70.

**Aenderung (Code)**:
- engine/src/gegner_r39.c: `re15_r39_affen_parkplatz` -> `re15_r39_affen_regel(e)` = Haken an && `e->type ==
  RE15_R39_AFFEN_TYP` (0x27); kein Raum mehr. Logzeilen tragen Raum + Bild (`affen init ... raum= typ= hp= regel=`,
  `affen treffer F.. slot= raum= hp= akku= -> FLINCH|OHNE ZUCKEN`).
- Akku-Bezug (PORT-WAHL der Buchfuehrung, jetzt fuer jeden 0x27 in jedem Riegel wirksam): `re15_r39_affen_merken` setzt den
  Bezug am Tick-Ende neu, wenn noch keiner gesetzt ist oder die HP ueber ihm liegen (ein Gorilla verliert HP nur — HP ueber
  dem Bezug heisst: der Platz traegt einen Aktor, der den INIT dieses Hakens nicht durchlief, z. B. eine Sonde mit gesetzter
  HP). Im Spiel setzt der INIT den Bezug (Sce_em_set -> hp 0 -> INIT im Spawn-Bild).
- include/re15_gegner_r39.h: Kopf (1) neu (Zensus, Nachtrag 3, Dispatch-Beleg, P228 @0x8006ee48), `RE15_R39_PARKPLATZ_RAUM`
  ersetzt durch `RE15_R39_AFFEN_TYP 0x27u`.
- enemy_ai_common.c / test_r35_affen.c: nur die Kommentare der vorhandenen Haken-Zeilen ("ROOM11C0" -> "jeder 0x27").

### Fortsetzung (2026-10-18 17:05, vierter Agent nach dem Sitzungslimit; Stand 5ecd8f07 = 0705d967 + Merge master)
- Vorgefunden: Baum sauber; 0705d967 WAR gebaut (exe 14:55, test_r39_gegner_1141.exe 14:56) und gemessen: ctest-Lauf 14:57
  des Vorgaengers (113 Unit-Tests, Regex scratchpad e_nb2/regex_unit.txt) -> 112 gruen, **unit_maggot_ai rot**
  (`FAIL(1): HP must seed to 180, got 360`); die vier unit_r39_gegner_1141_* gruen (ablauf1141: vorher 16 Treffer / 15
  Flinches je Gorilla, nachher 31 / 10, Kill-Flags, (4,0xF2) = 0, Johns Abgang, Dachtuer -> 0x17 in beiden Laeufen).
- Neu gebaut nach dem Merge (local_build.sh configure + build, 795 Schritte, LOCAL-BUILD-OK 17:2x).
- `ctest -j4 -R "^unit_.*(maggot|affen|1141|11c0|dining1141|elzaems|griff_gorilla|r39_gegner|zombieki_affen)"`:
  `99% tests passed, 1 tests failed out of 72` — wieder nur unit_maggot_ai.

#### Befund D — unit_maggot_ai (Original-Riegel des Gorilla-INIT) rot durch die Typ-Regel
- Ursache: test_maggot_ai.c setzt keinen Raum (g_current_room_id bleibt 0) und prueft (1) die Original-Saat
  `m->hp != 180` (@0x8011f514, `sh v0,154(a1)` @0x80117104). Mit dem Raum-Tor 0x11C0 war der Haken dort durchsichtig;
  seit der Typ-Regel verdoppelt er auch hier (360). Teil (2f) blieb gruen, weil der Riegel hp = 180 von Hand setzt
  (Akku 360 - 180 >= 36 -> Flinch) — also zufaellig, nicht weil er die Original-Antwort misst.
- Aenderung: test_maggot_ai.c `re15_r39_gegner_enable(0)` am Anfang von main (+ include, 2 Zeilen, Kommentar "Runde 39
  Spur E") — wie test_r35_affen.c room_boot: der Original-Riegel misst den Original-INIT und den Original-Flinch je
  Treffer; die NUTZER-VORGABE misst test_r39_gegner / test_r39_gegner_1141 (je vorher/nachher im selben Lauf).
  Kein Gorilla wird in test_maggot_ai ueber HP getoetet ((3) setzt hp = -1 direkt) — nichts an der Energie nachzuziehen.
- Nachher: `ctest -R "^unit_maggot_ai$"` -> `100% tests passed, 0 tests failed out of 1`.

#### Messung nachher an der exe (Stand db069f14, gebaut 17:2x)
- **ROOM11C0 (Leon), Gegenprobe der Typ-Regel**: Aufbau wie E_abnahme_1.md §1 (Messkopie `re15_pc_e_nb2c_11c0.exe`, Arbeitsordner
  scratchpad e_nb2c/n11c0; `RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486 RE15_EQUIP=3`,
  `RE15_INPUT_SCRIPT=M1.5` + 14 x `,MA0.1,M0.6` ab Spielbild 290, `RE15_EXIT_AT=900`) -> exit 0. gegner_r39.log:
  `affen init slot=2/3 raum=11c0 typ=27 hp=360 (tabelle 180 @0x8011f514, regel 1)`; `affen treffer F335 slot=2 raum=11c0 hp=348
  akku=12 -> OHNE ZUCKEN` / `F379 hp=336 akku=24 -> OHNE ZUCKEN` / `F440 hp=324 akku=36 -> FLINCH (Schwelle 36)` / `F462 hp=312
  akku=12 -> OHNE ZUCKEN` — bildgleich mit Abnahme 1 (F335/F379/F440/F462): der Wechsel Raum-Tor -> Typ-Regel aendert 11C0 nicht.
- **ROOM1141 (Elza), Versuch 1 (scratchpad e_nb2c/n1141_a, Skript `W10` + 140 x `W0.1,M0.4,MA0.1,M0.3`)**: Raumaufbau und
  BEFREIUNG-Spawn `affen init slot=2/3 raum=1141 typ=27 hp=360 (... regel 1)` (je zweimal: Raumaufbau, danach Phase H heraus,
  dann BEFREIUNG). Treffer F3075 G2 360->348 OHNE ZUCKEN, F3561 336 OHNE, F3615/F3669 G3 348/336 OHNE, **F3723 G3 324 akku 36
  FLINCH** (Clip 7 F3723-F3735), **F3858 G2 324 akku 36 FLINCH**, F4047/F4101 G3 312/300 OHNE, F4344/F4398 G2 312/300 OHNE.
  Das Skript taugte nicht als Kampf: nach jedem Loslassen von R1 dreht Elza ~12 Bilder auf das Ziel (ac=6), das Quadrat
  nach 12 Bildern faellt vor die Anschlag-Bereitschaft (ac=8 ab Bild ~16) und wird verworfen — 10 Schuesse in 1300 Bildern,
  Elza 100 -> 28 (F4360). Lauf abgebrochen (eigene PID 74648 der Messkopie, Stop-Process).
- **ROOM1141, Versuch 2 (n1141_b, Skript `W10` + 45 x `W0.1,M0.7,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M0.6` = R1 gehalten, alle drei
  Schuesse neu zielen)**: 30 Schuesse, 28 Treffer (27 auf G2, 1 auf G3), Auswertung scratchpad e_nb2c/auswert2.py:
  `treffer-zeilen 28 FLINCH 9 OHNE 19 regelwidrig 0` — G2 F3084 348 OHNE / F3106 336 OHNE / **F3128 324 FLINCH akku 36** /
  F3193 312 OHNE / F3432 300 OHNE / **F3474 288 FLINCH** / ... / **F4585 36 FLINCH** (jeder dritte Treffer, 9 x). Elza
  (steht) 100 -> 88 (F3212) -> 82 -> 70 -> 64 -> 58 -> 46 -> 34 -> 28 -> 22 -> 16 -> 4 -> **-2 (F4756, tot)** — 12 Gorilla-
  Angriffe (Biss -6 `addiu v0,v0,-6` @0x80118468, Heavy -12 `addiu v0,v0,-12` @0x801187bc). G2 bleibt bei 36, G3 bei 348.
  Der Lauf haengt danach im Tod (frame_count steht bei F5017, EXIT_AT nie) -> eigene PID 70712 beendet.
- **ROOM1141, Versuch 3 mit Elzas Schrotflinte (n1141_s, `RE15_GIVE=0x43:1,0x08:7,0x16:63 RE15_EQUIP=8`)**: Schaden 38 (Zeile
  0x27 Waffe 8, re15_damage.c s_wpn_dmg_maggot[8]) >= 36 -> jeder Treffer FLINCH (F3084 G2 322 akku 38, F3087 G3 322, F3117,
  F3124, F3150, F3273 — "andere Waffen entsprechend ihrem Schaden" an der exe bestaetigt). 7 Schuss bis F3734 (mg 7 -> 0),
  danach kein Nachladen mehr: Elza wird ab F3528 alle 37-60 Bilder gebissen (88 -> ... -> -2 F4378, 16 Angriffe) und kommt
  aus der Treffer-Reaktion nicht zum Nachladen (die Statusspalte bx zaehlt nur 0x15, -1 = keine H.GUN BULLETS — kein Befund).
  Lauf haengt danach im Tod -> PID 8160 beendet. Nicht weiter verfolgt.
- **Folgerung**: mit einem STEHENDEN Skript-Schuetzen ist der Kampf gegen zwei Gorillas mit je 360 Energie an der exe nicht
  bis zum Ende zu fahren (ein Spieler weicht aus; Munition fuer 62 Treffer liegt auf Elzas Weg reichlich: Zensus
  analysis/story_leon_elza/census.json, H.GUN BULLETS 0x15 in 1001/1011/1021/1051/1071/10E1/10F1/1191/11D1/11F1/1211 —
  zusammen 405 Schuss, dazu Remington 0x08 (10F1), Beretta 0x05 (11D1), Ingram 0x0C (1191), MC51 0x13 (1111)). Fuer die
  Pflicht-Messung "bis zum Tod beider Gorillas und Johns Abgang" darum ein MESSHAKEN (nur mit Env, kein Spielverhalten):
  `RE15_R39_MESS_SPIELER_HP=<n>` hebt am Kopf jedes KI-Bildes die Spieler-HP auf mindestens n (gegner_r39.c
  r39_mess_spieler_hp, aufgerufen aus re15_r39_hund_riegel_tick = Kopf von re15_enemy_ai_run_all, KEINE neue Zeile in einer
  gemeinsamen Datei); jeder Ausgleich steht mit Bild in gegner_r39.log. n = 100 haelt auch den Luft-Finisher (-600) fern:
  dessen Commit verlangt player.hp < 50 (selbst disassembliert: `lbu v1,149(v0)` == 0x13 @0x80118cd4-cdc, `lh v0,0(a2)`
  player.hp @0x80118cec / `slti v0,v0,50` @0x80118cf4 / `beq v0,zero` @0x80118cf8, hit_react == 0 @0x80118d04-0c,
  a804(0x1388,0xc0) @0x80118d18, +0x1dc == 0 @0x80118d34-3c, `sb v0,5(v1)` = Sub 8 @0x80118d44, Se 7 @0x80118d64).
- **ROOM1141, Versuch 4 mit Messhaken (n1141_h, RE15_R39_MESS_SPIELER_HP=100, sonst wie Versuch 2)**: bis F4585 bildgleich mit
  Versuch 2 (gleiche Treffer/Flinch-Bilder), Elza bleibt bei >= 88; danach ist das Magazin leer (mg 0, Reserve 105) und das
  Nachladen (Anschlag-Zustand ac=13 ab F4656) wird von jedem Biss abgebrochen: G2 beisst ab F4757 alle 50-58 Bilder
  (`messhaken F.. spieler hp 94 -> 100`, 58 Ausgleiche bis F7914) — eine Biss-Schleife gegen den stehenden Schuetzen.
  Kein weiterer Schuss -> abgebrochen (eigene PID 74492).
- **ROOM1141, Versuch 5 (n1141_m: Messhaken + `RE15_GIVE=0x43:1,0x04:99,0x15:60` = 99 Schuss im Magazin, Skript `W10` + 80 x
  `W0.1,M0.6` + 6 x `MA0.1,M0.3`, EXIT_AT 11200 -> erreicht)**: 36 Treffer-Zeilen, alle regelkonform; **G2 stirbt**: F5406
  hp 0 akku 36 FLINCH (hp >= 0 bleibt HURT, `bgez v1` @0x80012528), der 31. Treffer F5496 -> `st=7` hp -12. **G3 bleibt bei
  288 und ist ab F4624 AUSSERHALB des Raums** (x = -12218, Westwand-Zelle 0 = x -10600..-8600), wo er bis F11200 in Sub 4
  an der Aussenkante entlangschiebt (Elza trifft ihn nie wieder; Kampf endet nie, John wartet) -> Raum nicht abschliessbar.

#### Befund E — Gorilla durch die 2000 dicke Westwand gedrueckt (Versuch 5, F4623 -> F4624), Mechanismus belegt
- Bildfolge (state.txt n1141_m, Gorilla G3 = Platz 3 im Vergeltungs-Sprung Sub 7/Phase 2, Hoehe aus gegner_r39.log
  `schatten F..`): F4622 (-7368,-9243) y -3420; F4623 (-6982,-9649) y -3000; **F4624 (-12218,-9837)**. G2 (Platz 3 tickt
  nach Platz 2) steht F4624 bei (-6611,-9865). ROOM1141 SCA (Kopf @0x570, 5 x 19 Zellen): Zelle 0 = Westwand x -10600..-8600,
  z -25550..1850, Typ 1, u0 0xff (2000 dick).
- Kette, Schritt fuer Schritt nachgerechnet (alle Funktionen sind Byte-Ports, Original selbst disassembliert):
  1. Gorilla-Kasten @0x80121350 = {0,-1440,0,1600,1440,1600} (Radius 1600, Halbhoehe 1440, Versatz -1440; `read 0x80121350 6`).
  2. Koerper-Stoss Gorilla gegen Gorilla (FUN_8002aec4 aus der Wurzel @0x80116e40-54; Port re15_enemy_body_push_tail):
     dist = SquareRoot0 `jal 0x80065f60` @0x8002b1d8, pen = R - dist @0x8002b1e4 (`blez s0` @0x8002b1e8), Hoehenband
     `lhu a0,8(s5)` / `lhu v1,8(s6)` / `slt` @0x8002b208-228 (|dy| < 1440 + 1440 = 2880): F4623 dy = -3000 -> KEIN Stoss,
     F4624 dy ~ -2520 -> Stoss. Versatz x = dx*pen/(dist+1) (`mult s1,s0` / `div v0,a2` @0x8002b22c-260): dx = -6982 -
     (-6611) = -371, dz = 2, dist = 371, R = 3200, pen = 2829 -> **x -2821** -> G3 bei x = -9803 (jenseits der Wandmitte -9600).
     Der Querungs-Zweig fuer "Eintritt von oben" (Vorbild-Y +0x42 `lh v0,66(s2)` @0x8002b268, Achsentests alte X +0x40
     `lh a1,64(s2)` @0x8002b2b8 / alte Z +0x44 `lhu t2,68(s2)` @0x8002b2cc) greift NUR, wenn G3 in diesem Bild die X- bzw.
     Z-Koordinate von G2 ueberquert hat — hier nicht (x -6982 -> -6982, z -9649 -> -9863 gegen -9865) -> normaler Stoss wie
     im Port (der Zweig ist im Port nicht portiert, aendert dieses Bild aber nicht).
  3. Wandklemme FUN_8003b0a4 -> Rechteck-Ausstoss FUN_8003bca8 (Port re15_collision.c push_rect): t1 = (w + x + r) - pos + 18
     @0x8003bcc4-ce0, a0 = (x - r) - pos - 18 @0x8003bcd4-ce8, `slt v0,-a0,t1` / `beq` / `addu t1,a0,zero` @0x8003bcec-cfc =
     die NAEHERE erweiterte Kante gewinnt: Ost -8600 + 1600 + 18 = -6982 (Abstand 2821), West -10600 - 1600 - 18 = -12218
     (Abstand 2415) -> **x = -12218** (genau der gemessene Wert; -6982 ist die Ostkante, an die G3 in F4623 geschoben wurde).
- Folge: G3 steht hinter der Westwand ausserhalb des begehbaren Raums, seine Wandklemme haelt ihn an der Aussenkante. Das
  ist der Original-Mechanismus (Stoss 2821 > halbe Wanddicke 1000 + Radius-Rest), KEIN Fehler dieser Spur; die doppelte
  Energie verlaengert den Kampf (31 statt 16 Treffer je Gorilla, doppelt so viele Vergeltungs-Spruenge = Stoss-Gelegenheiten)
  und macht ihn wahrscheinlicher. Im Original-Ablauf (E16: "John: Elza, I'll try to distract them, while you try to get out
  of here") ist der Kampf nicht noetig — Schloss Slot 2 -> sub05 @0x00FD2 Set(4,0xC3)=1 + Aot_reset Slot 1 @0x00FDC macht
  die Dachtuer scharf; ob der Gang dorthin waehrend des Kampfes frei ist (Rolladen-Zelle 13: x -9000..-5300, z -7200..-6700),
  entscheidet der Runde-38-F-Ablauf (kein Punkt dieser Spur). -> OFFEN, Entscheidungsfrage an den Nutzer.
- Gegenprobe "kommt das auch mit 180 vor?": Stoss-Spruenge > 1000 je Bild gibt es in JEDEM Lauf, auch im Vorher-Lauf
  v1141_vorher (F3536 G3 (-6982,-9418) -> (-7368,-6254)); dort blieben sie im Raum.

- **ROOM1141, Versuch 6 (n1141_sued: wie Versuch 5, Wegplan danach `GEHE -6200 -14000 / -6200 -20000 / -1000 -20500`, also
  Elza in die freie Suedflaeche)**: 3 Treffer (G2 F3431 OHNE 12 / F3453 OHNE 24 / F3522 FLINCH 36), dann wirft der Gorilla
  Elza (mo=1) gegen die Suedwand und **Elza landet ausserhalb des Raums**: F3555 z = -23132 (= Innenkante -23600 + 450 + 18,
  Spieler-Radius 450), F3556 (-2715,-23132) -> **(-2144,-26068)** = Aussenkante -25600 - 450 - 18 (Zelle 1 = Suedwand
  z -25600..-23600, 2000 dick) — die Endlage ist der Rechteck-Ausstoss FUN_8003bca8 zur naeheren Kante (@0x8003bcec-cfc),
  diesmal fuer die Spielerin. Lage davor: G3 in Sub 15 (Rear-up-Pin, Elza im Opfer-Clip mo=1), F3555 G3 (-2551,-21646) /
  Elza (-2715,-23132) Abstand 1495, F3556 G3 (-2359,-21777). Der reine Koerper-Stoss (R = 450 + 1600 = 2050, pen ~680)
  reicht fuer die Wandmitte -24600 NICHT — welcher Schreiber Elza in diesem Bild ueber die Mitte setzt (Pin-/Wurf-Platzierung
  der Runde-35-Haken oder Opfer-Wurzelbewegung), ist NICHT ermittelt (OFFEN). Danach 159 Schuss ins Leere, abgebrochen
  (eigene PID 59416).

- **ROOM1141, Versuch 7 = drei Varianten von Versuch 5 (nur die Anfangs-Wartezeit des Skripts W9 / W11 / W12, sonst
  gleich; Laufordner n1141_v9/_v11/_v12, EXIT_AT 11500 -> erreicht, exit 0)** — der Kampf ist chaotisch, die Varianten
  streuen ihn, ohne die Regel zu beruehren:
  | Lauf | Treffer-Zeilen (FLINCH/OHNE) | regelwidrig | Tod (state.txt Zustand 3/7) | Johns Abgang (Platz 1 Typ 0x49) |
  |---|---|---|---|---|
  | v9 | 45 (15/30) | 0 | G3 F6027; G2 lebt (Munition 99 + 60 verschossen) | — |
  | **v11** | **60 (20/40)** | 0 | **G3 F5739, G2 F6576** | **F6678 los (Szene pm=2), Gang x -7351 z -21363 -> -975, F6815 geparkt (-30000,-30000)** |
  | **v12** | **60 (20/40)** | 0 | **G2 F5304, G3 F7536** | **F7638 los, F7775 geparkt (-30000,-30000)** |
  Je Gorilla 30 Treffer-Zeilen = 30 nicht-toedliche P228-Treffer (360 -> 0), davon 10 x FLINCH (genau jeder dritte, akku 36),
  der 31. Treffer toetet (hp -12 < 0, `bgez v1` @0x80012528) — wie unit_r39_gegner_1141_ablauf1141 (31 Treffer, 10 Flinches).
  Vom zweiten Tod bis zu Johns Aufbruch je 102 Bilder (v11 6576 -> 6678, v12 7536 -> 7638), in beiden Laeufen gleich.
  Die Zwischenglieder (Kill-Flags (7,0xCB)/(7,0xCC) vom Todes-Commit, sub01 @0x00BEE-@0x00C14 `Ck(4,0xF2)` / `Ck(7,0xCB)` /
  `Ck(7,0xCC)` -> `Set 22 04 f2 00` @0x00C0C) zeigt das Zustands-Log der exe nicht; gemessen sind sie im selben Engine-Weg
  von unit_r39_gegner_1141_ablauf1141 (Tod Bild 1787 -> Flags 1858 -> (4,0xF2) = 0 Bild 1859 -> John geparkt 2026). Johns
  Weg fuehrt durch den Gang hinter der Rolladen-Zelle 13 (z -7200..-6700), die nach dem Gemaelde abgeschaltet ist (sub00
  `Ifel_ck` Ck(3,0x6A) @0x00B80-84, Else @0x00BBE: `Sca_id_set 37 0n 0d 00` @0x00BC2-@0x00BD2 + `Sca_floor_set`
  @0x00BD6-@0x00BE6, alle 5 Baender). **Damit laeuft der Ablauf
  "Gorillas brechen ein -> Kampf -> Tod beider Gorillas -> sub01 -> John geht" an der exe mit 2 x 360 Energie durch.**
  Elza (Messhaken) blieb bei 100 (v11: Ausgleiche im Log, jeder <= 12).

#### Neuer Exe-Riegel integration_r39_gegner_1141 (gruen)
- Aufbau = Versuch 5 bis Spielbild 3900 (bildgleich: F3081/F3105/F3129 Platz 2, F3513/F3756/F3780 Platz 3). Lauf 17:57-18:01
  (erste Fassung, EXIT_AT 3900; die Endfassung laeuft bis 4500 und verlangt das Muster fuer mindestens EINEN Gorilla, s. u.):
  `ctest --test-dir re15_port/build -R "^integration_r39_gegner_1141$" --timeout 1200 --output-on-failure` ->
  `100% tests passed, 0 tests failed out of 1` (259,95 s). gegner_r39.log des Laufs: Messhaken aktiv, 4 x `affen init
  ... raum=1141 typ=27 hp=360 (... regel 1)`, Treffer-Zeilen wie Versuch 5; cap.raw und die Messkopie loescht der Test.
- **Endfassung** (EXIT_AT 4500, Muster fuer mindestens einen Gorilla, Kopf mit Integrations-Hinweis), Lauf 18:2x:
  `ctest --test-dir re15_port/build -R "^integration_r39_gegner_1141$" --timeout 1200 --output-on-failure -V` ->
  `r39_gegner_1141: OK — INIT 360 x 4, 20 Treffer-Zeilen (FLINCH 6 nur bei akku >= 36, OHNE ZUCKEN 14), Flinch-Beginne im
  state.txt 6` und `100% tests passed, 0 tests failed out of 1` (274,16 s) — genau die aus Versuch 5 bis F4500 erwarteten Werte.

#### Riegel, die Gorillas toeten — gegen die neue Energie geprueft
- integration_r38_dining1141_{w,z,d,l,s,r,a} toeten die Gorillas NICHT ueber HP, sondern ueber den Runde-38-Messhaken
  RE15_R38F_GORILLAS_TOT (dining_r38.c: nach n freien Kampfbildern "Gorillas heraus", Flags (7,0xCB)/(7,0xCC) = 1) und
  frieren sie mit RE15_R38F_GORILLAS_STARR ein (grid|0x20 = Root-Skip) — energieunabhaengig, nichts nachzuziehen.
  Lauf 17:41-17:56 (`ctest -j2 -R "^integration_r38_dining1141_"`): `100% tests passed, 0 tests failed out of 7` (871,8 s;
  _z 114 s, _w 187 s, _l 45 s, _d 276 s, _r 191 s, _s 548 s, _a 290 s).
- unit_r38_dining1141_* (10 Teile) gruen im Unit-Lauf (s. Tests); unit_r38_dining1141_abgang toetet ueber Kill-Flags, nicht HP.
- Der einzige Riegel, der die Gorillas ueber HP-Treffer bis zum Tod fuehrt, ist unit_r39_gegner_1141_ablauf1141 (neu,
  vorher 16 / nachher 31 Treffer je Gorilla, zugeschnitten auf 360 = 2 x 180 @0x8011f514 und `bgez` @0x80012528).
- unit_maggot_ai (Original-Riegel, Befund D) setzt hp = -1 direkt fuer den Tod.

#### Warum der Voll-Ablauf an der exe KEIN Riegel ist
- Versuch 7 zeigt den Voll-Ablauf (Tod beider Gorillas -> Johns Abgang) in zwei von drei Varianten; welche Variante
  durchlaeuft, entscheidet der chaotische Kampf (v9: Munition 159 Schuss alle, G2 lebt; Versuch 5: G3 hinter der Westwand,
  Befund E). Ein Riegel darauf faellt bei jeder Aenderung an Kampf-/Szenen-Timing um (Spur J baut ROOM1141 in Welle 3 um)
  — darum misst integration_r39_gegner_1141 die REGEL an der exe (INIT 360, Schwelle je Treffer, Flinch nur nach FLINCH-
  Eintrag; Bild 4500 mit 600 Bildern Spielraum) und unit_r39_gegner_1141_ablauf1141 den Voll-Ablauf ueber dieselben
  Engine-Wege (SCD-VM, BEFREIUNG, Resolver-Schreibsatz, Todes-Commit, sub01, Abgang) deterministisch.
- Nachstellen von Versuch 7 v11: scratchpad e_nb2c/lauf2.sh mit `E_GIVE="0x43:1,0x04:99,0x15:60"
  E_EXTRA="RE15_R39_MESS_SPIELER_HP=100"`, Skript `W11` + 80 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`) ab Spielbild 2760,
  EXIT_AT 11500; Auswertung e_nb2c/auswert2.py (Treffer/Flinch/Tod/John).

### Tests (Nachbesserung 2, Spurbaum, gezielt — keine volle Suite)
- Unit, Endstand (Messhaken + Typ-Regel + Original-Riegel-Haken), 18:0x:
  `ctest --test-dir re15_port/build -j4 -R "^(unit_(r39_gegner|r35_affen|r38_affen3|r38_dining1141|maggot|r36_zombieki_affen|
  r37_beretta|r37_elzaems|r38_griff|r38_szenenschutz_alle|r34_reaktion|r36_tueren|r37_fahrstuhl|1090_fire_pin|adult_spider_ai|
  ai_flavor_mixed|cockroach_ai|collision_layers|enemy_attack_reach|enemy_body_push|irons_neck_1150|item_use|member|r35_werfer|
  r37_sperre1050|re15_re2z_gore_se|re2_hybrid_rig|re2_spider_ai|re2_zombie_ai|writher_kill_flag|r35_granate|r37_szene11c0|
  r36_karte_tuer11c0|r36_szene1150_dialog11c0|r35_cut1150_montage_11c0)|r31_trefferboxen)" --timeout 600 --output-on-failure`
  (Ausdruck = scratchpad e_nb2c/regex_unit.txt, eine Zeile) -> **`100% tests passed, 0 tests failed out of 119`** (16,9 s).
  Nach der letzten Kopf-Kommentarzeile (re15_gegner_r39.h, Messhaken-Hinweis) neu gebaut (LOCAL-BUILD-OK) und derselbe
  Ausdruck erneut: **`100% tests passed, 0 tests failed out of 119`** (18,0 s).
  Darin: unit_r39_gegner_* (10), unit_r39_gegner_1141_{hp1141,zucken1141,ablauf1141,affe29}, unit_r35_affen_* (20),
  unit_r38_affen3_* (11), unit_r38_dining1141_* (10), unit_maggot_ai/_bone_square/_zone_leap, unit_r37_elzaems_*, unit_r38_griff_*.
- Integration, Endstand, 18:03-18:11: `ctest --test-dir re15_port/build -j1 -R "^integration_(r39_gegner_schatten|
  r39_gegner_hunde1190|r37_elzaems_elza1141|r37_szene11c0|r38_affen3|r38_affen3_decke|r38_affen3_tor)$" --timeout 1200
  --output-on-failure` -> **`100% tests passed, 0 tests failed out of 7`** (495,5 s).
- Integration dining1141, 17:41-17:56: `ctest --test-dir re15_port/build -j2 -R "^integration_r38_dining1141_" --timeout 3600
  --output-on-failure` -> **`100% tests passed, 0 tests failed out of 7`** (871,8 s; Engine-Stand = Endstand, danach nur
  Test-Registrierung neu konfiguriert).
- Integration neu: `ctest --test-dir re15_port/build -R "^integration_r39_gegner_1141$" --timeout 1200 --output-on-failure -V`
  -> **`100% tests passed, 0 tests failed out of 1`** (274,2 s).
- Zusammen 119 Unit + 15 Integration gezielt gruen; die volle Suite faehrt der Orchestrator im Integrationsbaum.

### OFFEN (Nachbesserung 2)
- **Befund E (Hinweis, nicht diese Regel)**: in ROOM1141 kann der Original-Koerper-Stoss (Gorilla/Gorilla R 3200, Kasten
  @0x80121350) einen Gorilla ueber die Mitte der 2000 dicken Westwand schieben; die Original-Wandklemme FUN_8003bca8 nimmt die
  naehere Kante (@0x8003bcec-cfc) und stellt ihn AUSSERHALB ab (Versuch 5: G3 bei x -12218, Kampf endet nie). Ebenso landete
  Elza im Rear-up-Pin hinter der Suedwand (Versuch 6, z -26068; der Schreiber, der sie ueber die Wandmitte setzt, ist NICHT
  ermittelt). Mechanik des Originals (beide Funktionen Byte-Ports, Zahlen nachgerechnet); die doppelte Energie verlaengert den
  Kampf (31 statt 16 Treffer je Gorilla) und macht es wahrscheinlicher. Elza kann den Raum dabei weiter ueber das Dachschloss
  verlassen (Original sub05 @0x00FC8-@0x00FDC, Gang offen nach Sca_id_set @0x00BC2-@0x00BD2) — ausser sie steht selbst
  ausserhalb. Naechster RE-Weg: das RE2-Gegenstueck (Koerper-Stoss + Wandklemme in ghidra_re2_Leon.txt) lesen, ob Retail den
  Durchstoss verhindert (Beta -> Retail), und den Elza-Fall (Pin-Platzierung Runde 35 J) mit RE15_ENEMY_DBG bildgenau messen.
- Spur F und Spur E lesen beide `RE15_R39_LOG`: E als Schalter (Datei gegner_r39.log), F (zustand_r39.c:58) als DATEINAME —
  mit `RE15_R39_LOG=1` legt F eine Datei "1" an (harmlos, in jedem Laufordner sichtbar). Bei der Integration einen der beiden
  Namen trennen (Hinweis an den Orchestrator; keine Aenderung in fremden Dateien).
- integration_r39_gegner_1141 haengt am Runde-38-F-Ablauf (Wegplan bis zum Gemaelde, Kampfbeginn ~F2632-F3060): wenn Spur J
  (Welle 3) die 1141-Szene umbaut, ist der Wegplan/EXIT_AT bei der Integration nachzuziehen (600 Bilder Spielraum eingebaut).

### Ergebnis Nachbesserung 2 (Mangel 1 der Abnahme 1)
- **Ursache**: Raum-Tor `re15_r39_affen_parkplatz` = `type 0x27 && g_current_room_id == 0x11C0` (Stand 2486a028) nach dem
  Wortlaut "beim Parking Lot"; Nutzer-Nachtrag 3 (2026-10-18 14:25) entscheidet "generell".
- **Messung vorher** (exe 2486a028, Elza ROOM1141 ueber 1131 -> Tuer, v1141_vorher): `affen init ... raum=1141 hp=180`, jeder
  P228-Treffer flincht (F3163/F3498/F3520/F3542 G2, F3603/F3625 G3, je Clip 7). Unit vorher-Teile: 180 / Clip 7 je Schuss /
  16 Treffer bis zum Tod.
- **Beleg**: HP-Saat `sh v0,154(a1)` @0x80117104 aus Zeile 0x27 @0x8011f514 = 16 x 180 (ohne hp-Bedingung); Pistole 12
  @0x8006ee44, P228 12 @0x8006ee48; Spur-Tabelle @0x801214a8 (Waffe 4 -> Boden-Flinch 0x8011b018); Resolver `bgez v1`
  @0x80012528; Dispatch 0x27 -> 0x80116db8 nur ueber @0x8011e900-0c; Zensus Sce_em_set 0x27 = ROOM11C0 + ROOM1141 (beide
  Szenarien), 0x29 in 3040/3041/4000/4040/4041/4080/4081/5000/5001/5031/50A0/50A1/50B0/5100/5101.
- **Aenderung**: `re15_r39_affen_regel(e)` = Haken an && Typ 0x27 (RE15_R39_AFFEN_TYP), kein Raum; Akku-Bezug am Tick-Ende
  fuer Aktoren ohne INIT (PORT-WAHL der Buchfuehrung); Logzeilen mit Raum/Bild; Kommentare/Kopf/Dossier ohne Parkplatz-
  Sonderregel; Original-Riegel test_maggot_ai.c `re15_r39_gegner_enable(0)` (Befund D); Messhaken RE15_R39_MESS_SPIELER_HP
  (nur Messlauf). Neue Riegel: test_r39_gegner_1141.c (hp1141 / zucken1141 / ablauf1141 / affe29),
  test_r39_gegner_1141.cmake (integration_r39_gegner_1141). 0x29 unveraendert (affe29 Bild fuer Bild gleich, affen3 gruen).
- **Messung nachher**: ROOM11C0 bildgleich mit Abnahme 1 (F335/F379 OHNE, F440 FLINCH akku 36); ROOM1141 Elza: INIT
  hp=360 (beide Gorillas, Raumaufbau + BEFREIUNG), jede Treffer-Zeile regelkonform (Versuche 2/5/7: 28/36/45/60/60 Zeilen,
  0 regelwidrig), je Gorilla 30 x 12 bis 0, 10 x FLINCH (jeder dritte), Tod mit dem 31.; Voll-Ablauf an der exe in v11/v12:
  Tod beider Gorillas -> John bricht 102 Bilder spaeter auf und parkt (F6815 / F7775); unit ablauf1141: Flags, (4,0xF2) = 0,
  Abgang, Dachtuer -> 0x17 mit 360.

## Nachbesserung 3 (2026-10-18, nach Abnahme 2 — Befund E: Elza/Gorilla durch Waende ROOM1141)
Auftrag (Orchestrator, args_e_nach3.json): Mangel 1 = Befund E (b) Elza ausserhalb des Raums nach dem Gorilla-Pin/Wurf
(Versuch 6, F3555 -> F3556, Suedwand); Mangel 2 = Befund E (a) Gorilla hinter der Westwand (Versuch 5, F4623 -> F4624,
Koerper-Stoss Gorilla gegen Gorilla) -> Softlock, weil der Kampf in 1141 im Port Pflicht ist (Runde 38 F). Je Mangel:
Schreiber finden, Original + RE2 lesen, Ursache korrigieren bzw. Invariante (Runde 38 T) belegt durchsetzen, Riegel.
Punkte 1-4 (Abnahme 2: erfuellt) nicht verschlechtern.

### Protokoll
- 19:07 Start (fuenfter Agent). Baum sauber, HEAD 98822214. Pflichtlektuere AUFTRAG/VERTRAG/E_abnahme_2/Dossier-Befund E,
  Runde 38 T (T_spiderman.md: Wandwaechter re15_r38_griff_durch_wand, Riegel unit_r38_griff_*).
- Cherry-pick 3fe9e358 aus r39/integration (Logschalter RE15_R39_LOG -> RE15_R39_GEGNER_LOG, Namenskollision mit Spur F) ->
  0a9076e9, damit neue Riegel denselben Schalternamen tragen wie der Integrationsstand (gleiche Aenderung auf beiden Seiten =
  konfliktfreier Merge).
- 19:09 gebaut (Stand 0a9076e9), 19:2x Mess-Schiene "Stationen" (gegner_r39.c r39_mess_stationen, nur mit
  RE15_R39_GEGNER_LOG=1 + RE15_R39_MESS_STATIONEN=1; liest die Runde-27-Stationen OPFER/SCHUB/KLEMME/ENDE des Vorbildes
  am Kopf von re15_enemy_ai_run_all; KEIN Spielverhalten), gebaut, Versuch 6 nachgefahren.

### Mangel 1 (Befund E (b)) — Messung vorher (Versuch 6 bildgleich nachgefahren)
- Lauf scratchpad e_nb3/v6_vorher (lauf.sh = e_nb2c/lauf3.sh mit RE15_R39_GEGNER_LOG; Wegplan e_nb2c/wegplan_sued.txt,
  `RE15_GIVE=0x43:1,0x04:99,0x15:60`, Messhaken `RE15_R39_MESS_SPIELER_HP=100`, Skript e_nb2c/schuss_sued.txt ab Spielbild
  3300, `RE15_EXIT_AT=3600`) -> exit 0, state.txt bildgleich mit n1141_sued: F3555 PL(-2715,-23132), **F3556 PL(-2144,-26068)**.
- Stationen (Zeile "stationen F<n+1> (Vorbild)" = Bild n), Wurf = Opfer-Clip 1 (mo=1), Anker (1060,-24101), Blick 389:

| Bild | Clip-Bild | OPFER (Platzierung) | SCHUB | KLEMME = Bildende |
|---|---|---|---|---|
| 3552 | 31 | (-3087,-23504) | = | (-3087,-23132) |
| 3554 | 33 | (-2715,-24266) | = | (-2715,-23132) |
| **3556** | **35** | **(-2144,-24973)** | **=** | **(-2144,-26068)** |
| 3557 | 36 | (-613,-25309) | = | (-613,-26068) |
| 3558.. | 37.. (>= 0x25: keine Platzierung) | (-613,-26068) | = | = |

- **Schreiber, Bild 3556**: (1) die Opfer-Platzierung des Gorilla-Wurfs (re15_player_victim_tick -> re15_victim_place,
  Clip 1 Bild 35 < 0x25) setzt Elza absolut auf z -24973 = 373 hinter die Mitte (-24600) der Suedwand (Zelle 1, z -25600..
  -23600); (2) der Koerper-Schub bewegt nichts (SCHUB = OPFER); (3) die Spieler-Wandklemme im Schwanz (re15_player_body_
  and_walls -> re15_collision_constrain = FUN_8003b0a4 -> push_rect = FUN_8003bca8) nimmt die naehere aufgeblasene Kante:
  innen -23600 + 450 + 18 = -23132 (Abstand 1841), aussen -25600 - 450 - 18 = -26068 (Abstand 1095) -> **-26068**. Schon der
  Anker (1060,-24101) liegt IN der Wandzelle (501 tief); Bild 3552/3554 (Platzierung 96 bzw. 334 vor der Mitte) klemmt die
  Innenkante, Bild 3556 kippt.
- Der Runde-38-T-Waechter (re15_r38_griff_durch_wand) laeuft in re15_victim_place NICHT fuer 0x27 (`vg_ty != 0x27u`,
  enemy_ai_common.c:1576 "0x27 klemmt im Spieler-Schwanz @0x80031d70") — der Gorilla-Wurf ist der einzige Griff ohne
  Querungs-Pruefung (0x29 ebenso: eigener Haken re15_affen3_opfer_tick, Schwanz-Klemme).

### Mangel 1 — RE-Belege (selbst disassembliert, re15_disasm.py / re2_disasm.py)
- RE1.5 Gorilla-Opfer-Haken 0x8011c118 P2 @0x8011c268: `lbu v0,0(a0)` (Spieler-Clip-Bild 0x800acae9) / `sltiu v0,v0,0x25`
  @0x8011c278 / `beq v0,zero` @0x8011c27c / `lw a1,-13364(a1)` (0x800acbcc) / `lw a2,-13360(a2)` (0x800acbd0) / `jal 0x8001ad68`
  @0x8011c294 mit `addiu a0,a0,-149` = 0x800aca54 (Spieler) -> danach anim_set @0x8011c2b0.
- FUN_8001ad68: Wurzel FUN_8001ae38 @0x8001ad84, RotMatrixY(+0x6a) @0x8001addc, ApplyMatrix @0x8001adec, x = (s16)+0xa0 +
  off.x (`lh v0,160(s1)` / `sw v0,52(s1)` @0x8001adf4-ae04), z = (s16)+0xa2 + off.z (@0x8001ae08-18) — absolut, ohne Klemme.
- Anker: Pin-Latch @0x8011ac00-18 `addiu s1,s0,-424` = 0x800aca54, `jal 0x8001ac38` @0x8011ac18; FUN_8001ac38 = Lage des
  Greifers minus RotY(+0x6a) * Wurzel(Bank +0x84/+0x16c @0x8001ac64-68) (`sh v0,160(a0)` @0x8001acfc), Kopie auf den Spieler
  — Port re15_affen_pin_anker (affen_11c0.c (5)) = dieselbe Rechnung.
- Spieler-Schwanz FUN_80031c44: Koerper-Schub `jal 0x8002b544` @0x80031cbc, Wandklemme `jal 0x8003b0a4` @0x80031d70 (Maske
  `ori a2,zero,0x1` @0x80031d74, Radius `lhu a1,6(v0)` @0x80031d6c = DAT_80073e94[+6] = 0x01c2 = 450, PSX.EXE Datei 0x6469a),
  Bezug = Spiegel +0x40/+0x44 (game_step_common.c Kopf des Schwanzes); Port game_step_common.c:2508 (wurf-Pfad) = dieselbe
  Reihenfolge Platzierung -> Schub -> Klemme.
- Ergebnis: **Original-Mechanismus**, kein Port-Fehler — mit demselben Zustand (Gorilla pinnt mit dem Anker in der Wand)
  setzt das Original Elza genauso an die Aussenkante (die Zahlen sind die der Byte-Ports; 1095 < 1841 -> Aussenkante).
- RE2 RETAIL (Beta -> Retail): RE2 hat keinen Gorilla; seine Wandklemme fuer Spieler UND Gegner ist FUN_8003567c ->
  FUN_8004c1bc (`jal 0x8004c1bc` @0x800356d0, Maske a1|0x1000000 @0x800356c4), die Rechteck-Routine waehlt sie ueber die
  Tabelle @0x800a737c (`lhu v0,8(s0)` / `andi 0xf` / `lw v0,29564(at)` @0x8004c418-34, `jalr v0` @0x8004c448): [0] =
  FUN_8004c900. Diese entscheidet die Seite NICHT nach der naeheren Kante, sondern nach der Vorbild-Lage +0x118/+0x11a
  (`lh v0,280(a3)` @0x8004c930, `lh v0,282(a3)` @0x8004c94c), die FUN_8003567c nach JEDER Klemme schreibt (`lw v0,132(s0)` /
  `lw v1,140(s0)` / `sh v0,280(s0)` / `sh v1,282(s0)` @0x800356ec-f8):
  t2 = 2r + w (@0x8004c91c-20), a0 = Vorbild.x + r - cell.x (@0x8004c930-3c), Code-Bit 2 = (a0 >= t2) (`sltu`/`xori`/`sll`
  @0x8004c940-48); Z ebenso Bit 1 (@0x8004c94c-64); Randfaelle a0 == -1 / t2+1 -> 2, a1 == -1 / t1+1 -> 1 (@0x8004c968-a8).
  Code 0 (Vorbild in BEIDEN Achsen innerhalb) -> FUN_8004c858 (@0x8004c9b8) = die RE1.5-Regel (naehere Kante, Code-Bits
  FUN_8004ca68). Bit 2: x = cell.x + w/2 (@0x8004c9f8) -/+ (r + w/2 + 1) (@0x8004c9e0-f0), Minus wenn x - Vorbild.x >= 0
  (`bltz a1` @0x8004c9f4, `subu` @0x8004ca00 / `addu` @0x8004ca04, `sw v0,132(a3)` @0x8004ca08); Bit 1 ebenso fuer z
  (@0x8004ca18-50). RE2 stellt den Aktor also immer auf die Seite, VON DER ER KAM.
- Fuer Bild 3556 rechnet RE2: Vorbild z -23132, a1 = -23132 + 450 + 25600 = 2918 >= t1 = 900 + 2000 = 2900 -> Bit 1;
  z - Vorbild = -24973 + 23132 < 0 -> z = -24600 + (450 + 1000 + 1) = **-23149** (innen). x: Zelle 1 spannt die Raumbreite,
  Vorbild innerhalb -> unveraendert. Elza bliebe im Raum.
