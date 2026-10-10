# Runde 36 — Spur B "kleidung" (2026-10-10)

Zweig `r36/kleidung`, Basis master 5a9dbb7b (= v0.8.23 3ec5b1f8 + Auftrag). Dossier fortlaufend.

## Auftrag (Wortlaut, AUFTRAG.md Z.32)

> Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.

Spur-Zuteilung (VERTRAG.md §0): "Regression, bisecten, beheben, Pin". Keine Bank-9-Bits, keine
Nachrichten-IDs, keine Ereignis-Nummern fuer Spur B.

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

### 5.3 Spielweg (echte exe, ROOM1140, Spielstand neben dem Zombie, CROSS-Hammern per Skript)
debug.log: `[save] CONTINUE: resumed in room 1140 (hp=100)` ... `[wound] sync: applied=1 ok=1 gen=2`
(F~228) ... `[wound] sync: applied=1 ok=1 gen=3` (F~366) — der Griff-Stempel kommt im echten Spiel
unter der RE2-KI an und der Renderer legt ihn auf die Textur (vorher in keinem Lauf eine solche Zeile).

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
Bestehende Riegel unveraendert gruen: unit_wound_accumulator, probe_save_injured, probe_r17_weste_karte,
Dog-/Kraehen-/Zombie-Griff-Tests (Suite-Zeile s.u.).
Vorbestehend ROT auf dem Basisstand 5a9dbb7b (nicht Spur B): unit_pri_hashes — die vom Orchestrator
eingecheckte Nutzer-Maske pri/STAGE1/10F0/02_02.png ist in der Hash-Liste ungelistet ("119 Dateien
im Baum, 118 gelistet, 1 ungelistet"); Liste mit tools/maske/pri_hashes.py neu erzeugen = Spur O (PRI).

## 7. OFFEN
* STAGE5 (Birkin): das Overlay traegt dieselben vier generischen Helper (FUN_80109b00/b3c/b78/b9c)
  und Birkin-eigene Stempel (FUN_8010c61c/8010cd88/8010d7a4: (0,0x32); FUN_801110c4/801110f8;
  FUN_80112294 tabellen-indiziert). Der Port-Birkin (enemy_ai_common.c :13719/:13757 Releases,
  Treffer) stempelt nicht — vorbestehende Luecke ausserhalb dieser Regression (STAGE1). Naechster
  Schritt: STAGE5-Hook-A-Registrierung (Zwilling von FUN_8011e864) lesen, Birkin-Release ->
  `re15_kleidung_abwurf`, Treffer-Sites -> (0,0x32).
* RE2-Kraehe: Dive/Strike-Angriffe der RE1.5-Kraehe (@0x80113b7c/@0x801144bc) haben in der RE2-Kraehe
  keinen eigenen Zwilling (sie kennt nur den Griff-Peck) — abgedeckt ist der Griff-Eintritt.

## 8. Fuer den Nutzer
* Keine neuen Assets, keine Sprachdateien, keine Paket-Gate-Eintraege.
* Bedienung unveraendert: Blut/Risse entstehen wie im Original durch Treffer — drei ueberlebte
  Zombie-Griffe auf dieselbe Koerperseite (je +50 auf zwei Brust- bzw. Rueckenpanels, Schwelle 120),
  Hundebisse (+50 je Biss auf ein Brust-/Rueckenpanel), Kraehengriffe; Heilung loescht sie nicht,
  Speichern/Laden erhaelt sie (v9-Spielstand). Unter "AI RE2" (Standard) und "AI RE1.5" gleich.
* Elza: die Decals liegen jetzt auf Elzas eigenen Panels (LUT-Zeile 4) — vorher sassen sie daneben.
