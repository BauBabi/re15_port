# Runde 36 — Spur B "kleidung" (2026-10-10)

Zweig `r36/kleidung`, Basis master 5a9dbb7b (= v0.8.23 3ec5b1f8 + Auftrag), zuletzt master 78a4a748 eingemergt.
Dossier fortlaufend; Fortsetzung nach Sitzungslimit am 2026-10-10 14:20 (§0, §5.3, §6, §7 ergaenzt).

## Auftrag (Wortlaut, AUFTRAG.md Z.32)

> Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.

Spur-Zuteilung (VERTRAG.md §0): "Regression, bisecten, beheben, Pin". Keine Bank-9-Bits, keine
Nachrichten-IDs, keine Ereignis-Nummern fuer Spur B.

## 0. Wortlaut-Pruefung (kritisch, gegen den Satz des Nutzers)

Der Nutzer beschreibt eine KORRELATION ("wenn er auf Warning/Danger runter kommt"), keinen Mechanismus.
Im Original gibt es keine HP-gegatete Textur: FUN_8004ed6c (HP-Status >=0x50 -> 0, >=0x14 -> 1, sonst 2,
Flag&2 -> 3) steuert nur das Inventar-Portrait (@0x800479ec-80047a54, blood_decals.md §3.4); der
Wund-Akkumulator FUN_80037edc hat in der EXE KEINEN Aufrufer (Byte-Scan `jal 0x80037edc` = 0C00DFB7 ueber
PSX.EXE: 0 Treffer; STAGE1.BIN 22, STAGE2 10, STAGE3 17, STAGE4 21, STAGE5 21, STAGE6 0). Die Korrelation
entsteht, weil dieselben Griffe, die HP kosten, die Panels fuellen: RE2-Griff -30/-20/-30 HP, Stempel
+50 je Abwurf, Schwelle 120 -> der DRITTE ueberlebte Griff bringt Leon auf 20 HP (Caution/"Warning",
naechster Treffer Danger) UND setzt die erste Wund-Stufe. Original-Ground-Truth dazu:
stage_saves/mzd_blood_decals_hp30.sav (Hauptbaum, unversioniert; blood_decals.md §3.4) — hp 30 nach drei Griffen von vorn,
Panels 5/7 Stufe 1. Der Port-Spielweg (§5.3) liefert dieselbe Kopplung: hp 100 -> 70 -> 50 -> 20,
Panel 7 Stufe 1 beim dritten Abwurf, sichtbar auf der Weste (Bild). Damit ist der Wortlaut erfuellt:
Leons Kleidung zeigt beim Abstieg auf Warning/Danger wieder Blut/Risse — unter dem Standardgehirn
(RE2), auf dem echten Weg (Griffe, Spielstand vom Spiel geschrieben, CONTINUE), ohne Sprung.

Was der Satz NICHT abdeckt und offen bleibt (§7): Gegner, deren Original-Stempel der Port nie trug
(STAGE3-5 (0,0x32)-Stellen, Birkin) — dort kommt Leon auch heute ohne Decal auf Danger; das war vor
der Regression genauso (nie portiert), ist also keine Regression und nicht Teil dieser Spur.

## 1. Messung vorher

### 1.1 Wie der Port die "zerrissene/blutige Kleidung" realisiert (Bestand)
Blut-Decal-Wundsystem (analysis/blood_decals.md, 2026-08-02/03): Wund-Tabelle `s_wounds[8]`
{level, acc} in engine/src/re15_damage.c (= Tabelle @0x800b10ec), `re15_wound_add` = FUN_80037edc,
Anwendung als Rect-Kopie Damage-Bank -> Panel im Slot-0-Stash (render_pc.c
`re15_render_pc_wound_blit`, main.c "Blut-Decal-Sync" ~Z.9288). Es gibt KEINE Gesundheits-Stufen-
Textur (kein hp-gated Texturwechsel) — "Warning/Danger" ist nur die Beobachtung des Nutzers, der
Mechanismus ist der Treffer-/Griff-Akkumulator (Schwelle 120 je Panel).

Trigger im Port (Zensus `re15_wound_add`/`re15_wound_release_stamp`, identisch in 9362dd7e,
87cc8575, 3ec5b1f8):
* enemy_ai_common.c:2493 — RE1.5-Zombie-Griff [4] THROW-OFF (FUN_80102548): `release_stamp(dir)`
* enemy_ai_common.c:4273 — RE1.5-Kriecher-Griff [4] (0x80103f38): `release_stamp(sub+1)`
* enemy_ai_common.c:5925 — RE1.5-Kraehe Angriff (0x801161e8), :7487 RE1.5-Hund Release (@0x80111b64),
  :8072 RE1.5-Hund Biss (@0x8010f360), :1799 RE1.5-Hund-Finisher (@0x80111e78-90)
* affen_11c0.c:498 — Gorilla-Finisher (@0x8011c55c-74, nur beim Tod)
* enemy_ai_re2_zombie.c / enemy_ai_re2_dog.c / enemy_ai_re2_crow.c: KEIN einziger Aufruf.

Default-KI seit e11962ec (2026-08-22, "RE2-KI ist jetzt der Standard") = RE15_AI_FLAVOR_RE2
(enemy_ai_re2_zombie.c:101); die Wahl im OPTIONS-Menue wird nicht gespeichert (pc_ai_mode_set ->
re15_ai_flavor_set, kein Config-File) — jeder Spielstart laeuft RE2.

### 1.2 Messung am gebauten Stand (master 5a9dbb7b = v0.8.23 + Auftrag)
Sonde `test_r36_kleidung griff <re2|re15>` (tests/unit/test_r36_kleidung.c): ROOM1140 ueber den
echten game_step-Weg (SCD-Spawns, re15_game_step, Pad-Kanten), Greifer = erster stehender Zombie
(Slot 2 Typ 0x10), Leon daneben, HP wird >=60 gehalten (kein Devour/One-Save), Knopf-Hammern alle 2
Takte im Griff, 4 ueberlebte Griffe (Opfer-Zustand 1 -> 3):

| Flavor | Abwurf 1 | 2 | 3 | 4 | Ergebnis |
|---|---|---|---|---|---|
| RE2 (DEFAULT) | alle 8 Panels L0 | L0 | L0 | L0 | **nie ein Stempel, gen bleibt 1** |
| RE1.5 | L0 | L0 | p4=L1 p7=L1 (gen 3) | unveraendert | Stempel beim 3. Griff (Helper 0x8010a208 von hinten: (0,10)(4,50)(7,50) -> 150 >= 120) |

Nutzer-Spielstaende (C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_card.mcr, 5 Slots,
alle v9): wounds[8][2] in ALLEN Slots 0/0 — auch Slot 4 (ROOM1070, hp=20 = Danger) und Slot 3
(Elza ROOM1071, hp=60). befund.log des Nutzers: HP-Verlauf 100 -> 80 -> 60 in ROOM1190 (Hunde,
RE2-Hund -20 je Biss @0x80104EB8), danach 65. Also: der Nutzer kam ueber RE2-Gegner auf
Warning/Danger, und die Wund-Tabelle blieb leer — exakt die Sonde.

## 2. RE-Belege (Adressen, Bytes)

### 2.1 Der Griff-Stempel sitzt auf der SPIELER-Seite (STAGE1.BIN roh @0x80100000, selbst disassembliert)
Opfer-FSM cmd 5 = 0x8010a2cc (Hook A LAB_8010a28c, Phasen ueber aca5a @0x8010017c;
analysis/player_hit_chain.md §3). Phase 4 = Handler 0x8010a4e8, laeuft genau einmal je ueberlebtem
Griff (der GREIFER schreibt aca5a := 4):
```
8010a4f0 ori v0,zero,0x5 / 8010a4f8 sb v0,-13734(at)   ; aca5a := 5 (Phase 4 -> 5)
8010a514 sb v1,-13592(at)                               ; acae8 := Seite*3+2 (Release-Clip)
8010a518 sltiu v0,v0,0x2 / 8010a51c bne -> 8010a550     ; aca59 < 2 (vorn/hinten) ?
8010a524 ori a0,zero,0x1500 ... 8010a54c addiu a2,a2,580  ; Kriecher: Blut an Spieler-Part 3 (+0x244)
8010a550 ori a0,zero,0x1500 ... 8010a570 addiu a2,a2,1440 ; vorn/hinten: Blut an Part 8 (+0x5A0)
8010a574 jal 0x80019700                                 ; Release-Blut, a1 = Spieler-Yaw +0x6a
8010a580 lbu v0,-13735(v0)      0x800aca59              ; dir = Griff-Richtung
8010a598 sll v0,v0,2 / 8010a5a0 addiu at,at,440  0x801201b8 / 8010a5a8 lw v0,0(at) / 8010a5b0 jalr v0
```
Tabelle 0x801201b8: [0] 0x8010a1cc [1] 0x8010a208 [2] 0x8010a244 [3] 0x8010a268 (table-Dump).
Helper (alle `jal 0x80037edc`): 0x8010a1cc: (0,0xa) @0x8010a1d4-dc, (5,0x32) @0x8010a1e0-e8,
(7,0x32) @0x8010a1ec-f4 | 0x8010a208: (0,0xa), (4,0x32), (7,0x32) @0x8010a210-30 | 0x8010a244:
(1,0x32) @0x8010a24c-54 | 0x8010a268: (2,0x32) @0x8010a270-78.
Konsequenz: der Stempel ist GEHIRN-UNABHAENGIG — jeder Greifer, der die generische Opfer-FSM
benutzt (Hook-A-Registrierung FUN_8011e864 @0x8011e9c0-ac8: 0x10,0x11,0x12,0x16,0x18,0x1c-0x1f;
0x13 ueber LAB_8010c16c), loest ihn beim Abwurf aus. Der Port hatte ihn in den RE1.5-ZOMBIE-Code
(FUN_80102548 [4] / Kriecher 0x80103f38 [4]) gelegt; der RE2-Abwurf (P4 @0x80102968) kam ohne.

### 2.2 Akkumulator FUN_80037edc (PSX.EXE)
```
80037efc lbu v0,1(t0) / 80037f04 addu v0,v0,a1 / 80037f08 sb v0,1(t0)   ; acc += amount
80037f0c lbu v0,1(t0)                                                   ; NEU gelesen = 8-Bit!
80037f14 sltiu v0,v0,0x78 / 80037f18 bne -> 80037fc8                     ; < 120 -> nur Akku
80037f24 sb zero,1(t0) / 80037f2c sb v0,0(t0)                           ; acc := 0, level++
80037f40 sltiu v1,v1,0x3 / 80037f4c ori v0,zero,0x2 / 80037f50 sb v0,0(t0) ; Clamp 2
```
Der Port verglich das untrunkierte Register (der Kommentar behauptete das Gegenteil) — korrigiert
auf `(sum & 0xff) < 0x78`; mit den ausgelieferten Betraegen (<= 0x32) ohne sichtbare Folge (max 169).

### 2.3 Hund (RE1.5 Biss-Connect FUN_8010f15c) — Form fuer den RE2-Boden-Biss
```
8010f2dc bne (+0x1e4) / 8010f2e4 bltz hp -> Griff        ; sonst Knockdown + Stempel:
8010f30c sb v0,-13736(at)  aca58 := 2                     ; Spieler-cmd HIT
8010f310 jal 0x8001a780 / 8010f31c addiu v0,v0,2 / 8010f324 sb aca59   ; aca59 = a780(Spieler)+2
8010f334 jal 0x8001af20                                   ; rng (Engine-Strom)
8010f33c-58 lui/addiu a0 0x80121010; lbu aca59; andi v0,1; sll v1,1; addu; addu
8010f35c lbu a0,0(v1) / 8010f360 jal 0x80037edc / 8010f364 ori a1,zero,0x32
bytes 0x80121010: e8 08 11 80 05 07 04 06   -> aca59=2 (Spieler schaut zum Hund): 05/07 (Brust);
                                               aca59=3 (gleichgerichtet, Ruecken): 04/06
```
RE2-Hund (enemy_ai_re2_dog.c re2d_contact): Boden-Biss @0x80104F7C-88 -> 0x801051BC(0,dir) = PL
HURT; dort wird die RE1.5-Form angehaengt (PORT-MAPPING, RE2 kennt keine Wunden). Der RE2-
Kehlbiss (Rueckgabe 2) ist toedlich -> kein Release-Stempel noetig (RE1.5-Hund-Release @0x80111b64
stempelt nur beim UEBERLEBTEN Hunde-Griff; der RE2-Hund hat keinen ueberlebbaren Griff).

### 2.4 Kraehe (RE1.5 0x801161e8) — Form fuer den RE2-Griff
```
801161f8 lh v1,492(v0)            ; +0x1ec = Spieler.y - Kraehe.y (@0x801124cc / @0x80114f40)
80116200 slti v0,v1,1500 -> (1,0xa) @0x8011620c, (2,0xa) @0x80116214-1c/70
80116220 slti v0,v1,3000 -> (3,0xa) @0x8011622c, (4,0x28) @0x80116238, (6,0x28) @0x80116244, (5,0x28) @0x80116250-70
80116254 slti v0,v1,3600 -> (0,0x8) @0x80116260, (7,0x28) @0x80116268-70 ; sonst nichts (beq @0x80116258)
```
RE1.5-Aufrufer: Dive @0x80113b7c, Griff-Eintritt @0x80113ef4, Strike @0x801144bc. RE2-Kraehe:
Griff P0 geclaimt (@0x80102550-58 -> Opfer-FSM, enemy_ai_re2_crow.c:1121) -> dieselbe Form mit
vert = Spieler.y - Kraehe.y LIVE (PORT-MAPPING).

### 2.5 Panel-Ziel-LUT @0x80074208 (PSX.EXE Datei-Offset 0x64a08) — Elza-Zeile
Builder FUN_80037c1c: `lbu v0,-120(s3)` @0x80037c8c (s3 = 0x800acad4 -> 0x800aca5c = Charakter-Byte),
`sll v0,v0,4` @0x80037c94 -> Zeile = Charakter (Leon 0, Elza 4; lying_devour.md: einziger Writer
@0x8001d4f4, Titel 0/4). Die Tabelle hat ACHT Zeilen (bytes 0x80074208 128):
```
Zeile 0 (PL00 Leon): (67,35)(43,44)(31,192)(66,175)(22,116)(16,26)(6,102)(0,0)
Zeile 1 (PL01):      (67,35)(43,44)(31,192)(66,175)(23,99)(7,34)(7,115)(24,6)
Zeile 4 (PL04 Elza): (70,38)(40,119)(48,47)(66,162)(9,79)(11,15)(27,190)(43,192)   <- 46 26 28 77 30 2f 42 a2 09 4f 0b 0f 1b be 2b c0
```
Der Port (main.c WLUTX/WLUTY, 2 Zeilen) nahm fuer Elza Zeile 1 = eine LEON-Variante. Gemessen
(§5.3): mit Zeile 1 blaue Saeume neben den UV-Inseln von Elzas Jacke/Hose, mit Zeile 4 sitzen die
Decals auf Shirt, Aermel und Hose. PL04.TIM traegt eine eigene Damage-Bank in Seite 2 (x256..383,
alle vier Baender voll belegt; Render-Sichtpruefung: zerrissene/blutige Jackenteile).

## 3. Bisect

Dieselbe Sonde (identischer Quelltext) gegen die Engine von v0.8.21 (9362dd7e, Quellbaum per
`git archive` in den Scratchpad, Assets per Junction, eigener Build):

| Stand | RE2 (Default) | RE1.5 |
|---|---|---|
| v0.8.21 9362dd7e | 4 Abwuerfe (F98/F198/F492/F848), alle Panels L0 | Stempel beim 3. Abwurf F416: p4=L1 p7=L1 |
| master 5a9dbb7b (= v0.8.23) | 4 Abwuerfe (F98/F198/F492/F848), alle Panels L0 | Stempel beim 3. Abwurf F416: p4=L1 p7=L1 |

Bild-fuer-Bild identisch. Zwischen v0.8.21 und v0.8.23 hat sich am Wundsystem NICHTS geaendert
(git grep der Aufrufstellen an allen drei Tags identisch; render_pc.c-Diff der Runde 35 beruehrt nur
Raum-Slots 1..18/24..49, Slot 0 = Spieler ist explizit ausgenommen, re15_render_pc_tim_slot_raum).
Der verursachende Commit ist **e11962ec (2026-08-22) "feat: RE2-KI ist jetzt der Standard"**: seit
dem Default RE2 laufen alle Griffe/Bisse ueber die RE2-Gehirne, und deren Port-Fassung ruft den
Wund-Akkumulator nirgends — die Stempel haengen im Port am RE1.5-ZOMBIE-Code (Fehlplatzierung,
s. §2: das Original stempelt auf der SPIELER-Seite, gehirn-unabhaengig). Der Nutzer hat die
blutige Kleidung zuletzt unter der RE1.5-KI gesehen (bis 2026-08-22) und den Verlust jetzt bemerkt.

## 4. Umsetzung

Neu: `engine/src/kleidung_r36.c` + `include/re15_kleidung_r36.h`
* `re15_kleidung_generischer_griff(typ)`: Hook-A-Typen {0x10,0x11,0x12,0x13,0x16,0x18,0x1c-0x1f}
  + 0x1A (RE2-Gitterarm ROOM1210, Opfer-Spender 0x10 — PORT-WAHL, im RE1.5-Overlay ohne Griff).
* `re15_kleidung_abwurf(typ, dir)`: Phase 4 @0x8010a4e8 = Release-Blut (0x1500, Part 8 / Part 3)
  + `re15_wound_release_stamp(dir)` (Helper-Tabelle 0x801201b8).
* `re15_kleidung_hundebiss(hund, spieler)`: @0x8010f33c-64 (05 07 04 06, +0x32, Engine-rng).
* `re15_kleidung_kraehe(kraehe, spieler)`: 0x801161e8-Baender, vert live.
* `re15_kleidung_panel_ziel(char, panel, &x, &y)`: die 8-Zeilen-LUT @0x80074208.

Haken (je 1-2 Zeilen):
* enemy_ai_common.c :2493 RE1.5-Zombie [4] und :4273 Kriecher [4]: `re15_kleidung_abwurf(...)`
  ersetzt die bisherigen Inline-Bloecke (Stempel + Blut) — bildgleich (Sonde RE1.5 unveraendert
  F152/F284/F416). :12949 RE2-Arm P4 (EM01A) ruft sie neu. Accessor `re15_player_victim_variant()`.
* enemy_ai_re2_zombie.c P4 @0x80102968 (:1865): `re15_kleidung_abwurf(e->type, variant)` NEU.
* enemy_ai_re2_dog.c Boden-Biss (:623): `re15_kleidung_hundebiss(e, pl)` NEU.
* enemy_ai_re2_crow.c Griff P0 (:1123): `re15_kleidung_kraehe(e, pl)` NEU.
* platform/pc/main.c Blut-Decal-Sync: `re15_kleidung_panel_ziel(g_gameflow.character, p, ...)`
  statt WLUTX/WLUTY[(character&4)?1:0].
* engine/src/re15_damage.c re15_wound_add: `(sum & 0xff) < 0x78` (@0x80037f0c).

Nicht angefasst (byte-true): der RE2-Abbruch des Griffs durch Treffer (re2z_grab_abort -> PL cmd 1
@0x80104F68, kein Phase-4-Durchlauf) und der Port-eigene Hund-stirbt-Abwurf (enemy_ai_re2_dog.c:2240)
stempeln NICHT — das Original stempelt nur ueber Phase 4 (aca5a := 4 durch den Mash-Release).

## 5. Messung nachher

### 5.1 Mechanik (Sonde griff, derselbe Lauf wie §1.2)
| Flavor | Abwurf 1 | 2 | 3 | 4 |
|---|---|---|---|---|
| RE2 (DEFAULT) | L0 (F98) | L0 (F198) | **p4=L1 p7=L1, gen 3 (F492)** | unveraendert (F848) |
| RE1.5 | L0 (F152) | L0 (F284) | p4=L1 p7=L1, gen 3 (F416) | unveraendert (F548) |
RE2 stempelt jetzt wie RE1.5 (Griff von hinten -> Helper 0x8010a208: 3x50 auf Panel 4 und 7);
RE1.5 ist bildgleich zu vorher (kein Doppelstempel durch die Umhaengung).

Hund (Sonde hund, ROOM11D0 RE2, 5 Hunde frei): Bisse F30/F78/F111/F116 (hp -20 je Biss) -> je
genau ein Panel aus {4,5,6,7} +50 (0/0 -> 0/50), Panels 0..3 unberuehrt.
Kraehe (Sonde kraehe, ROOM10C0 RE2): Griffe F348/F1278 -> je +130 Wundpunkte (= Band 1500..2999:
10+40+40+40), die Band-Grenzen 1499/1500/2999/3000/3599/3600 liefern exakt die Tabellen von 0x801161e8.

### 5.2 Bild (echte exe, CONTINUE mit Spielstand, Framedump Bild 150, RE15_WINDOW_SCALE=1)
ROOM1130/1131 Cut 2, Figur bei (-1024,0,-3336) rot 856, hp 20 (Danger-Haltung Clip 23):
| Vergleich | Pixel verschieden | Kasten |
|---|---|---|
| Leon ohne / ohne (zwei Laeufe) | 0 | — (deterministisch) |
| Leon ohne / alle 8 Panels Stufe 2 | 737 | (192,96)-(231,179) = Brust, Weste, Schulter, Oberschenkel |
| Leon ohne / Panel 5+7 Stufe 1 (= 3 ueberlebte Griffe von vorn) | 250 | (204,103)-(230,139) = Brust/Weste |
| Elza ohne / alle 8 Panels Stufe 2, LUT-Zeile 1 (ALT) | 765 | blaue Saeume an Jacke/Hose (falsche Panels) |
| Elza ohne / alle 8 Panels Stufe 2, LUT-Zeile 4 (NEU) | 458 | Blut auf Shirt, Aermel, Hose |
Alle Abweichungen liegen im Figurenkasten x177..247 / y81..205; die Kulisse ist bitgleich.
Belegbilder (Scratchpad, nicht versioniert): AB1130.png (Leon Fine/Stufe 2), ABCelza.png.

### 5.3 Spielweg (echte exe, RE2-Standardgehirn, kein Raumsprung; Bilder im Dossier-Ordner)
Messhaken RE15_KLEIDUNG_LOG=1 (env-gegatet, kleidung_r36.c `kleid_log`; Accessor re15_wound_acc).
Lauf 1: CONTINUE aus Spielstand ROOM1140 neben dem Briefing-Zombie (Leon (-1400,0,-19600) rot 0,
hp 100, Zombies Slot 2 Typ 0x10 bei (-1800,0,-19600) u.a.), CROSS-Hammern ab Spielbild 60 (1 Bild
Taste / 1 Bild Pause), danach R1.2 (Drehung zur Kamera von Cut 1) und Save-Haken RE15_SAVE_TEST_AGAIN=300
(das SPIEL schreibt den Spielstand, re15_savedata_capture, v5 wounds[8][2]). debug.log (drei Laeufe
zeilengleich = deterministisch):
```
[save] CONTINUE: resumed in room 1140 (hp=100)
[kleidung] F97  abwurf typ=0x11 dir=0 hp=70 gen=1 panels=0/10,0/0,0/0,0/0,0/0,0/50,0/0,0/50
[kleidung] F163 abwurf typ=0x10 dir=1 hp=50 gen=1 panels=0/20,0/0,0/0,0/0,0/50,0/50,0/0,0/100
[kleidung] F229 abwurf typ=0x11 dir=0 hp=20 gen=2 panels=0/30,0/0,0/0,0/0,0/50,0/100,0/0,1/0
[wound] sync: applied=1 ok=1 gen=2 slotgen=2
[save] saved (room 1140) slot n=2 -> next=3
```
Griff 1 von vorn (Helper 0x8010a1cc: 0+10, 5+50, 7+50), Griff 2 von hinten (0x8010a208: 0+10, 4+50,
7+50), Griff 3 von vorn: Panel 7 = 150 >= 0x78 -> Stufe 1 (Akku 0, @0x80037f24/2c), Panel 5 = 100,
Panel 4 = 50, Panel 0 = 30. HP 100 -> 70 -> 50 -> 20 = Caution ("Warning"); ohne Flucht toetet der
vierte Griff bei F366 (hp 0) — genau deshalb war das Bild des Vorgaengers (griff_exe_4.png) unbrauchbar:
Leon starb, CONTINUE_TEST lud neu, die Serie lief endlos.

Werkzeug `test_r36_kleidung nullen <karte> [slot]`: liest den vom Spiel geschriebenen Spielstand, meldet
die Wund-Tabelle (`Raum 1140 char 0 pos (-621,0,-19766) rot 1855 hp 20 cut 1 wunden 0/30 0/0 0/0 0/0
0/50 0/100 0/0 1/0`), nullt NUR wounds[8][2], Pruefsumme neu, Rest bytegleich (66 Byte Unterschied
inkl. Titel) — die Referenz "derselbe Spielstand ohne Wunden".
Laeufe 2/3: CONTINUE aus beiden Spielstaenden, Framedump Bild 30/60/90/120 (RE15_WINDOW_SCALE=1,
beschleunigter Renderer):

| Bild | Pixel verschieden | Kasten | ausserhalb der Figur |
|---|---|---|---|
| 30 / 60 / 90 | 121 | (176,103)-(190,125) = Weste/Brust, Leons linke Schulterseite | 0 |
| 120 | 118 | (183,105)-(194,127) (Idle-Atmung) | 0 |
| Software-Renderer (Riegel) Bild 60 | 131 | (176,103)-(190,125) | 0 |

Bilder: `B_kleidung_spielweg_vorher_nachher.png` (Ausschnitt x150..230/y85..205, 4x; links genullt,
rechts der Spielweg-Spielstand: Blutfleck auf der Weste oberhalb von "R.P.D.", Leon in der
Caution-Haltung mit hp 20) und `B_kleidung_spielweg_bild60.png` (ganzes Bild 60, Cut 1).
Gegenprobe an derselben Position/Blickrichtung mit dem Kartenwerkzeug: Panel 7 allein Stufe 1 in der
SEITENANSICHT (rot 3583) = 3 Pixel, Panel 5 allein = 0, alle Panels Stufe 2 = 267 — die Griff-Panels
5/7 liegen auf der Brust (Torso-Insel MD1-Mesh 0, blood_decals.md §4), die Figur muss der Kamera
zugewandt stehen; darum die Drehung R1.2 vor dem Spielstand.

## 6. Tests (tests/unit/probes/r36_kleidung.cmake, alle gruen)
* unit_r36_kleidung_griff_re2 / _re15 — ROOM1140, game_step, 4 ueberlebte Griffe: Abwurf 1/2 ohne
  Stempel, Abwurf 3: Panel 7 Stufe 1 UND genau eines von 4/5, Panel 0/1/2/3/6 = 0, Generation springt.
* unit_r36_kleidung_hund — ROOM11D0 RE2-Hund: >= 3 Boden-Bisse, je genau ein Panel aus 05 07 04 06 um
  0x32, Panels 0..3 unberuehrt.
* unit_r36_kleidung_kraehe — 0x801161e8-Baender an den Grenzen + Live-Griff ROOM10C0 (jeder Griff stempelt).
* unit_r36_kleidung_lut — LUT @0x80074208: Leon Zeile 0, Elza Zeile 4, Zeile 8 ungueltig, Rechtecke
  innerhalb der Body-Seiten.
* integration_r36_kleidung_bild — 6 exe-Laeufe (Leon A/A2/B/C, Elza A/B), Pixel-Differenz: 0 /
  >= 350 / >= 120 / >= 220, alles im Figurenkasten (Schwellen = halbe Messwerte).
* integration_r36_kleidung_spielweg (NEU, Fortsetzung) — der Weg des Nutzers mit der exe unter dem
  RE2-Standardgehirn: 3 ueberlebte Griffe (Log-Zeilen [kleidung] F97/F163/F229, hp 70/50/20, 3. Abwurf
  gen=2), Spielstand vom Spiel geschrieben, `nullen` meldet die Wunde (Exit 3 = keine), CONTINUE
  vorher/nachher: >= 60 Pixel, alle in x150..240/y80..210 (gemessen 131). ~35 s.
Bestehende Riegel unveraendert gruen: unit_wound_accumulator, probe_save_injured, probe_r17_weste_karte,
Dog-/Kraehen-/Zombie-Griff-Tests (Suite-Zeile s.u.).
unit_pri_hashes war auf dem Basisstand 5a9dbb7b rot (Nutzer-Maske 10F0/02_02.png ungelistet) — auf
master mit 78a4a748 behoben und in diesen Zweig gemergt (37ed0867); hier gruen.

Volle Suite im Baum (2026-10-10, configure/build/test je ueber local_build.sh, Basis master 78a4a748,
parallel liefen die Suiten der Spuren schuss/granate/klettern/zombieki, 5033 s):
```
100% tests passed, 0 tests failed out of 599
=== LOCAL-BUILD-OK (test) — Tests 599/599
```
(Ein zweiter Durchlauf als `local_build.sh all` wurde danach gestartet; seine Zeile steht, sobald er
durch ist, in einem Folge-Commit — die Flatter-Kandidaten boot_bg_pin, dark_start_pin, relatch_pin,
save_counter_pin, weste_load_pin, r30_sicherung_*, r30_titel_puls, r34n_e_dokumente_bild waren im
ersten Durchlauf alle gruen.)

## 7. OFFEN (ausserhalb der Regression; beruehrt den Wortlaut nur fuer Gegner, die nie stempelten)

Zensus aller `jal 0x80037edc`-Stellen je Overlay (Byte-Scan, STAGE*.BIN ohne Header, Funktionen aus
RE_15_Quellcode_Overlays/STAGE{N}_full):
* Die vier generischen Phase-4-Helper sind in ALLEN fuenf Stages wertgleich — STAGE2 FUN_8010a060/
  a09c/a0d8/a0fc, STAGE3 FUN_8010a2b8/a2f4/a330/a354, STAGE4 FUN_8010a180/a1bc/a1f8/a21c, STAGE5
  FUN_8010a300/a33c/a378/a39c: (0,10)(5,0x32)(7,0x32) / (0,10)(4,0x32)(7,0x32) / (1,0x32) / (2,0x32).
  `re15_kleidung_abwurf` ist typ-, nicht stage-gebunden und deckt damit den generischen Griff ueberall,
  sofern der Greifer-Typ in `re15_kleidung_generischer_griff` steht (STAGE1-Registrierung FUN_8011e864;
  die Zwillinge der anderen Stages sind NICHT gegengelesen — naechster Schritt fuer STAGE2-5).
* Hund-Zwillinge (tabellen-indizierter Biss `*(byte*)(aca59*2+...)`: STAGE2 FUN_801120b8, STAGE3
  FUN_8010dc3c, STAGE4 FUN_8011275c, STAGE5 FUN_801128dc; Release (0,10) STAGE2 FUN_801127f0; Finisher
  (0,10)(5,0x32)(7,0x32) STAGE3 FUN_801160a4, STAGE4 FUN_80111744, STAGE5 FUN_801118c4) — im Port
  stage-unabhaengig durch den RE1.5-Hund (enemy_ai_common.c) bzw. den RE2-Hund-Haken abgedeckt.
* NICHT portiert (vorbestehend, kein Teil der Regression): die (0,0x32)-Stellen STAGE3 FUN_80112784,
  STAGE4 FUN_8010de24, STAGE5 FUN_8010dfa4 (je ein Gegner-Handler, Gegnertyp nicht bestimmt) sowie
  die Birkin-eigenen Stempel in STAGE5 (FUN_8010c61c/8010cd88/8010d7a4 (0,0x32); FUN_801110c4/801110f8;
  FUN_80112294 tabellen-indiziert; Port-Birkin enemy_ai_common.c :13719/:13757 stempelt nicht).
  Naechster Schritt: Hook-A-Registrierung je Stage lesen (Zwilling von FUN_8011e864), Treffer-Sites ->
  `re15_wound_add(0,0x32)`, Birkin-Release -> `re15_kleidung_abwurf`.
* RE2-Kraehe: Dive/Strike der RE1.5-Kraehe (@0x80113b7c/@0x801144bc) haben in der RE2-Kraehe keinen
  Zwilling (sie kennt nur den Griff-Peck); abgedeckt ist der Griff-Eintritt (Form 0x801161e8).
* Der Port verliert die Stempel NICHT beim Laden (v5-Spielstand traegt wounds[8][2]); das Original
  nullt beim Spieler-Load (FUN_80037c1c @0x800316c8/0x800318cc, blood_decals.md §5). Vorbestehende
  PORT-WAHL (analysis/save_injured_state.md SI-1: der Original-Save-Blob enthaelt die Tabelle, der
  Original-LOAD-Restore @0x80026290-a0 wird vom Builder ueberschrieben) — nicht Teil dieser Spur.

## 8. Fuer den Nutzer
* Keine neuen Assets, keine Sprachdateien, keine Paket-Gate-Eintraege.
* Bedienung unveraendert: Blut/Risse entstehen wie im Original durch Treffer — drei ueberlebte
  Zombie-Griffe auf dieselbe Koerperseite (je +50 auf zwei Brust- bzw. Rueckenpanels, Schwelle 120),
  Hundebisse (+50 je Biss auf ein Brust-/Rueckenpanel), Kraehengriffe; Heilung loescht sie nicht,
  Speichern/Laden erhaelt sie (v9-Spielstand). Unter "AI RE2" (Standard) und "AI RE1.5" gleich.
* Elza: die Decals liegen jetzt auf Elzas eigenen Panels (LUT-Zeile 4) — vorher sassen sie daneben.
* Messhaken (nur fuer Messungen, kein Spielverhalten): RE15_KLEIDUNG_LOG=1 schreibt je Stempel eine
  Zeile `[kleidung] F<bild> <quelle> typ dir hp gen panels=level/akku,...` ins debug.log.

## Nachbesserung 1 (2026-10-10, nach Abnahme 0 — B_abnahme_0.md)

Maengel: M1 (Nutzer-Weg Hunde ROOM1190: fuenf RE2-Bisse 100 -> 0, 250 Wundpunkte, kein Decal) und
M2 (Hinweis: Abnahme 0 Teilstand; Messumgebung RDP -> `SDL_ASSERT=always_ignore`).

### N1.1 M1 — Messung vorher (selbst wiederholt, HEAD a096a1a5 = gebauter Stand 866af8e3)

Messkopie `re15_pc_r36b_nb1_vorher.exe` (= re15_pc.exe 16:13), Arbeitsordner Scratchpad, Befehl der
Abnahme (Bild-Fenster verkuerzt):
```
SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_KLEIDUNG_LOG=1 RE15_DEBUG_JUMP=1190@240 RE15_DEBUG_SUB=13@450
RE15_DEBUG_DOGWAKE=500 RE15_FRAMEDUMP=560-760/20:h_ RE15_EXIT_AT=1000   -> RC=0, 43 s
[kleidung] F582 hundebiss typ=0x20 dir=2 hp=80 gen=1 panels=0/0,0/0,0/0,0/0,0/50,0/0,0/0,0/0
[kleidung] F585 hundebiss typ=0x20 dir=2 hp=60 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/0,0/0,0/0
[kleidung] F587 hundebiss typ=0x20 dir=1 hp=40 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/0,0/0,0/50
[kleidung] F673 hundebiss typ=0x20 dir=0 hp=20 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/50,0/0,0/50
[kleidung] F708 hundebiss typ=0x20 dir=0 hp=0 gen=1 panels=0/0,0/0,0/0,0/0,0/100,0/100,0/0,0/50
```
Zeilengleich mit B_abnahme_0.md §5.1 (deterministisch). Fuenf Bisse x 20 HP, je EIN Stempel +0x32 ->
250 Punkte, kein Panel erreicht 0x78, gen bleibt 1: kein Decal bis in den Tod. Der Mangel ist echt.

### N1.2 M1 — RE-Belege (selbst disassembliert)

RE1.5-Hund, Angriffszustand 8 = FUN_8010f15c (Zustandstabelle @0x80120fd4, Eintrag 8 = 0x80120ff4 ->
0x8010f15c; Phasen-Sprungtabelle @0x801001ec: [0] 0x8010f1a0 [1] 0x8010f204 [2] 0x8010f370
[3] 0x8010f3e0 ...), STAGE1.BIN:
```
Fenster 1 (Sprung, Phase 1, +0x95 > 0x0c: `sltiu v0,v0,0xd` @0x8010f254):
8010f2bc lhu v0,-13586(v0)   player.hp
8010f2c8 addiu a0,v0,-10                     ; BISS KOSTET 10 HP
8010f2d0 sh a0,-13586(at)    player.hp
8010f2dc bne +0x1e4 / 8010f2e4 bltz hp       ; Griff-Eskalation 0x8010f468
8010f334 jal 0x8001af20 (rng) / 8010f34c andi v0,v0,0x1
8010f35c lbu a0,0(v1)  (0x80121010 + aca59*2 + rng&1 -> Bytes @0x80121014 = 05 07 04 06)
8010f360 jal 0x80037edc / 8010f364 ori a1,zero,0x32   ; EIN Stempel je 10-HP-Biss
Fenster 2 (Landung, Phase 3, nur nach Fehlsprung):
8010f438 lhu player.hp / 8010f444 addiu a0,v0,-10 / 8010f44c sh   ; ebenfalls 10 HP
8010f460 bgez -> 8010f4b8: Knockdown OHNE jal 0x80037edc (bis 8010f504 j 0x8010f598)
```
Das RE1.5-Verhaeltnis des Knockdown-Bisses (Fenster 1) ist damit **0x32 Wundpunkte je 10 HP, ein
Stempel (mit eigenem rng-Zug @0x8010f334) je Biss**. Der Lande-Biss (Fenster 2) ist ein Nachschnappen
nach einem Fehlsprung und stempelt nie.

RE2-Hund (EMD0G_MOD0.BIN, re2_disasm.py):
```
80104eb8 addiu a0,zero,20 / 80104ebc jal 0x800401d4 / 80104ec0 addu a1,zero,zero   ; BISS KOSTET 20 HP
80104f7c sh v0(=3),6(s0) / 80104f84 sb 1,6(s3) / 80104f90 bltz hp / 80104fa4 jal 0x801051bc ; Boden-Biss
```
Der RE2-Biss entspricht dem RE1.5-Fenster-1-Biss (Kontakt im Sprung -> Spieler HURT, kein
Nachschnappen-Fenster), kostet aber das DOPPELTE. Die Spur hatte ihm EINEN RE1.5-Stempel gegeben ->
halbes Verhaeltnis; auf dem Hundeweg reichen die HP nicht, um ein Panel ueber 0x78 zu bringen.

**Ursache M1:** PORT-MAPPING mit falschem Massstab — ein RE2-Biss (20 HP) wurde als EIN RE1.5-Biss
(10 HP) gestempelt. Richtig ist: je 10 HP Biss-Schaden ein RE1.5-Biss-Stempel, also beim RE2-Biss
20/10 = **zwei** Stempel der RE1.5-Form (je eigener rng-Zug, je +0x32, Panel-Paar nach Blickrichtung).

### N1.3 Nebenbefund (nicht Spur B, unter OFFEN): RE2-Hund-Claim fehlt
Die drei Bisse F582/F585/F587 fallen in FUENF Bildern (100 -> 40). RE2 0x801051BC (EMD0G_MOD0.BIN)
schreibt beim Boden-Biss `sb 2,4(a0)` @0x801051e4 (PL HURT), `sb dir+2,5(a0)` @0x801051ec und
`ori v1,v1,0x80` / `sb v1,467(a0)` @0x801051f8/0x80105200 = PL+0x1D3 |= 0x80 (Claim); jeder weitere
Hund prueft diesen Claim (`re15_player_is_grabbed`, @0x80104E5C-68) und beisst nicht, solange er
steht. Der Port laesst den Claim aus (enemy_ai_re2_dog.c re2d_contact, Kommentar "der Claim ueber den
Grab-Pin unterbleibt") -> mehrere Hunde beissen im selben Augenblick. Das veraendert die HP-Kurve,
nicht das Verhaeltnis Wundpunkte:HP — keine Spur-B-Zuteilung (Hunde-KI), Adresse + Messweg unter OFFEN.
