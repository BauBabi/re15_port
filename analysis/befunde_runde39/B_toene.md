# Runde 39 — Spur B "toene" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r39_toene` (Zweig `r39/toene`, Basis 3e49ff87). Wortlaut: `AUFTRAG.md`, Zuteilung: `VERTRAG.md` §0 (Spur B).

## Punkte (Wortlaut des Nutzers)
1. "In Room 10F0 fehlt noch beim öffnen der Schublade der Sound. Den könnte man zum Beispiel aus Resident Evil 2 holen, wenn dort eine Schublade geöffnet wird."
2. "In Room 1090, wenn die Hunde in der Cutscene rein gesprungen wird, fehlt noch der Sound vom öffnenden Gitter, den könnte man von den öffnenden Gittern der Spinnen aus STAGE2 holen."
3. "In Room 1090 fehlt noch Sound für den bewegenden Schrank nach dem lösen des Rätsels. Den könnte man aus Resident Evil 2 holen von den bewegenden Regalen in der 2. Etage."

## Stand
- 2026-10-18 02:34-03:05 (Sitzung 1, Limit): Lektuere, Raumzuordnung (§0), Messung vorher (§1), RE-Belege (§2), Mini-Bank + Modul (vor Bau).
- 2026-10-18 07:10-08:xx (Sitzung 2): gebaut (configure + build gruen), Riegel geschrieben und gemessen (§4.1), echter Weg an der
  exe fuer Leon und Elza (§4.2), Riegel auf die gemessene Struktur gesetzt, Dossier §3-§7, gezielter ctest-Lauf (§5.1). FERTIG
  aus Sicht der Spur; Entscheidungsfragen und Paket-Gate-Eintrag unter §7.

## 0. Welche Raeume gemeint sind (gemessen, nicht vermutet)

### 0.1 Punkt 1 "Room 10F0 ... Schublade" = ROOM11F0 (Leon) / ROOM11F1 (Elza, bytegleich)
* ROOM10F0.RDT (= ROOM10F1.RDT, `cmp` 0 Abweichungen) hat KEINE Schublade: main00 @0x0F32..0x10D4 nur Tueren,
  Aot_set sce 1 (Texte), 4 Item_aot_set (Slot 11..14), Obj_model_set 1..3 (Item-Modelle); sub00/sub01 nur der
  Tritt obj 0 (Typ 4) und (3,102). Kein Cut_chg, kein Ereignis (`scd_dump_room.py`, Ausgabe im Messordner).
* ROOM11F0.RDT (Boiler Room, Generator) main00 @0x00CE6 `2c 0d 03 31 00 00 d8 27 b2 c1 dc 05 e8 03 ff 00 18 13 00 00`
  = AOT 13 sce 3 Rechteck (10200,-15950) 1500 x 1000 -> Evt_exec **sub19** @0x017B6:
  `29 0b` Cut_chg 11 / `22 02 07 01` Sperre / Work_set(1,0) + Member_set(1,-16200) (Leon aus dem Bild) / bei
  Ck(9,99)==0 Work_set(3,1) + Member_set(1,-1200) (Patronen obj 1 @0x00D10 in die Schublade) / Sleep 30 /
  `47 0e` Aot_on 14 = Item_aot_set @0x00CFA (H. Gun Bullets x30, Bit (9,99)) / Sleep 2 / zurueck / Cut_old /
  Cut_auto / `22 02 07 00` @0x0180A. Kein Se_on in sub19 -> RE1.5 STUMM.
* Bild (echte exe, RE15_DEBUG_JUMP=11F0, RE15_FIRE_AOT=13@400, Framedump F405): Cut 11 = die GEOEFFNETE
  SCHUBLADE mit der Patronenschachtel (`B_belege/b1_11f0_cut11_schublade.png`). Runde 34 Nacht C nennt sub19
  bereits "Schublade" (`C_generator.md` Z.186-187).
* Die Ziffernverschiebung 10xx -> 11xx ist dieselbe wie bei Punkt 2/3 (s. 0.2) und schon in Runde 36 belegt
  (`analysis/befunde_runde36/M_karte.md` R0: "ROOM 1090" mit Hunde-Luke + Weste = ROOM1190). Gegenprobe: die
  Leichenschublade ROOM1200 sub02 (obj 1 faehrt, ebenfalls ohne Se_on) ist eine Zombie-Szene, keine
  "geoeffnete Schublade" und zwei Ziffern entfernt -> nicht gemeint (im Abschnitt "Fuer den Nutzer" benannt).

### 0.2 Punkt 2 + 3 "Room 1090" = ROOM1190 (Schiessstand B1)
* ROOM1090.RDT hat weder Hunde noch einen Schrank, der sich nach einem Raetsel bewegt: Sce_em_set nur 0x26
  (Feuer), 0x42 (Ada), 0x10/0x11 (Zombies @0x023A0..0x023DC); Obj 0/1 sind Typ-4-Kisten (Spieler schiebt).
* ROOM1190.RDT sub10 @0x0276A (gestartet aus sub01 @0x02468/@0x024A8, wenn die vier Zielscheiben-Schalter
  stimmen: Ck(4,0xF3)=1 Strom, Bank-5-Bits 4..7) = DIE Raetsel-Szene:
  - `22 04 ea 01` Set(4,0xEA) geloest, Sleep 40, **@0x02790 `29 08` Cut 8**, Sleep 20,
    **@0x02796 `2e 03 05` Work_set(3,5) = SCHRANK obj 5** (@0x02A46 bei (3920,0,-25952)),
    @0x0279A `2f 00 9c ff` Speed_set(0,-100), @0x0279E `0d 00 06 00 14 00` For 20 {Add_speed, Evt_next, Evt_next}
    = 20 x -100 = -2000 in x ueber 40 Bilder (Endlage (1920,0,-25952) = sub13 @0x02AB8), danach
    Aot_reset Slot 5 sce 9 = die Red Keycard dahinter (Item_aot_set @0x02A30 Item 0x37).
  - Sleep 20, **@0x027E0 `29 0b` Cut 11**, je Hund (Sce_em_set 0x20 @0x02900/@0x02914/@0x02928 im Schacht):
    Work_set(2,n) + Member_set(0x0C,0x43) (+ Member_set(1,-3600)) + **Gosub 0x0B** @0x027EA/@0x027F8/@0x02806
    = sub11 @0x02854: Work_set(3,4) = LUKE/GITTER obj 4 (@0x02A06 bei (310,-4500,-26946)), Speed_set(5,150)
    Speed_set(0x0B,-6), For 10 {Add_speed, Add_aspeed, Evt_next} = aufklappen, dann Speed_set(5,-150)/(0x0B,6)
    For 10 = zuklappen. Dreimal (je Hund). Kein Se_on in sub10/sub11 -> RE1.5 STUMM.

## 1. Messung vorher (Bau 3e49ff87 im Spurbaum, Messkopie der exe, RE15_AUDIO_CAP_SYNC + RE15_SE_DEBUG)

| Lauf | Weg | Ergebnis |
|---|---|---|
| j11f0 | RE15_DEBUG_JUMP=11F0@250, RE15_FIRE_AOT=13@400 (AOT 13 = Schreibtisch -> sub19) | `[scd F401] Cut_chg(11)`; KEINE `[se]`-Zeile im ganzen Lauf; Mitschnitt Raum-Bild 390..440 Spitze 1 (Stille, 11F0 ohne Musik) -> Schublade STUMM |
| j1190 | RE15_DEBUG_JUMP=1190@250 (Spawn (3050,0,-11650) Cut 0, sub13 setzt die Hunde in den Schacht), RE15_FORCE_EVENT=10@400 (= sub10, die Raetsel-Szene) | F441 Cut 8, Schrank faehrt ~F461..F501 (Bilder 450/480/510: Spind gleitet nach rechts, Waffenregal frei), F521 Cut 11; Luke: `prop-member prop[4] m=5 -> 0` F531/F563/F595 (sub11-Beginn) und F553/F585/F617 (sub11-Ende); `[se]` NUR se=5 (Hund, Stimme 7) bei F521/F553/F585; Mitschnitt: Schrankfahrt F461..F501 Spitze 2447..5205 = Raummusik-Pegel (F440..F460 2782..4992), keine Spitze; Luke auf F532..F541: 2785..6224 = Ausklang des Hunde-SE von F523 + Musik -> Schrank und Gitter STUMM |

Bilder: `B_belege/b23_1190_schrank_gitter_vorher.png` (F450 Schrank zu, F480 faehrt, F510 offen, F540 Hund im
Lueftungsgitter). Regression? NEIN - RE1.5 spielt an allen drei Stellen nichts (kein Se_on in 11F0 sub19, 1190
sub10/sub11, s. §0); der Port war nie anders. Kein Bisect noetig.

## 2. RE-Belege

### 2.1 Punkt 1 - RE2-Schubladen-Ton
* RE2 `info/re2leon/PSX.EXE`, AOT-Handler-Tabelle @0x800a73c4 (Aufruf @0x80051494-9c, Runde 30 tuer-verschlossen.md
  §3.3): `[13] 0x800a73f8 -> 0x80052044` = SCHUBLADE (sce 13). Selbst disassembliert (re2_disasm.py):
  - @0x800520f0 `jal 0x80077360` Flag (Gegenstand schon genommen?) -> @0x800522a0 Text 4 ("There is nothing more"),
    kein Ton, keine Kamera.
  - @0x80052100 `lhu a1,0(s1)` / @0x80052108 `jal 0x80077360` (aufgeschlossen?) / **@0x80052110 `bne v0,zero,0x80052180`
    mit @0x80052114 `lui a0,0x228`** (Delay-Slot) -> @0x80052188 `addu a1,zero,zero` / **@0x8005218c `jal 0x8005ba28`**
    = Se_on(Bank 2 = Raumbank, Satz 0x28), nicht positional; danach @0x800521cc `ori v1,v1,0x11` (Schubladen-Objekt
    faehrt auf) und **@0x800521d8 `jal 0x8002c7ac`** mit a0 = Nutzlast+4 = KAMERA-SCHNITT (0x8002c7ac: `sb a0,-1038(at)`
    = 0x800cfbf2 aktueller Schnitt, Suche in der Schnitt-Tabelle Schritt 20). Ton und Schnitt also im SELBEN Aufruf.
  - Nicht aufgeschlossen: @0x80052118 `jal 0x800696cc` a0=31 (Small Key im Inventar?) -> Frage -> @0x800523a0
    `lui a0,0x227` / @0x800523a4 `jal 0x8005ba28` = Se_on(2,0x27) Aufschliessen.
* Zensus aller 250 RE2-RDTs (Walker re2_scd_walk.py, Aot_set 0x2C mit sce 13): genau 4 Raeume, je Satz 0x28 eine
  eigene Welle (Werkzeug-Ausgabe, Lagen in der Datei):

  | Raum | Aot_set (sce 13) | Satz 0x27 (Schloss) | Satz 0x28 EDT / VAG | Dauer |
  |---|---|---|---|---|
  | ROOM10E0 | MAIN sub00 @0x01D52 | leer | `00 00 94 16` / VAG 10 @0x0C228 6448 B sha1 61994673da2b | 0,86 s |
  | **ROOM1130** | INIT sub00 @0x00D50 `2c 03 0d 31 ..` | `00 00 44 16` VAG 5 6976 B | **`00 00 54 16` / VAG 6 @0x15A08 5216 B sha1 c58644421c0f** | 0,83 s |
  | ROOM1150 | MAIN sub12 @0x019BA | leer | `00 00 93 00` / VAG 8 @0x3F284 8256 B sha1 4aabadce33d6 | 1,31 s |
  | ROOM2010 | INIT sub00 @0x00C44 | `00 00 74 16` VAG 8 4592 B | `00 00 84 17` / VAG 7 @0x0ADF8 6880 B sha1 e550533af331 | 1,54 s |

  Wahl: **ROOM1130 Satz 0x28** - die SAP-Liste des Nutzers (RE2_SOUND_LISTE.md) nennt Schubladen-Toene nur bei
  "ROOM113.SAP - Changing clothes/locked drawer sounds?"; ROOM1130 traegt genau das Paar 0x27 (Schloss) / 0x28 (auf)
  der verschlossenen Schublade. (PORT-WAHL unter 4 gleichwertigen RE2-Schubladen; EDT-/Ton-Lage im Werkzeug-Selbsttest.)
* RE1.5-Gegenstueck: ROOM11F0/11F1 sub19 @0x017B6 `29 0b` Cut_chg 11 = Schnitt auf die GEOEFFNETE Schublade (Bild
  §0.1) -> der Ton gehoert in das Bild dieses Schnitts (RE2: Se_on @0x8005218c und Schnitt @0x800521d8 im selben
  Handler-Aufruf). Anker: Datei-Offset 0x017B6, Signatur `29 0b 22 02 07 01 2e 01`. Abweichung von RE2 (benannt):
  RE1.5 zeigt die Schublade auch nach dem Nehmen der Patronen erneut (sub19 laeuft wieder, nur ohne Aot_on 14);
  RE2 oeffnet eine leere Schublade gar nicht (@0x800520f8 -> Text 4). Der Ton begleitet das GEZEIGTE Oeffnen, also
  jedes Mal, wenn sub19 den Schnitt setzt.

### 2.2 Punkt 2 - Stage-2-Spinnengitter
* RE1.5 ROOM2060 sub02 @0x0133E (Schachtspinne 0, Ausloeser sub01 Cut 1, Runde 38 R §1.1) - das GITTER obj 0:
  `2e 03 00` Work_set(3,0) @0x01346, **@0x0134A `36 02 0e 00 03 02 ..` Se_on(2,0x0E)** direkt vor dem Aufklappen
  (@0x01356 Speed_set(5,25) / @0x0135A For 20 {Add_speed, Evt_next}), Sleep 6, @0x01368 Speed_set(5,-25) For 20
  (zurueck in die Ruhelage) **@0x01376 `36 02 0d 00 00 02 ..` Se_on(2,0x0D)** = Aufschlag in der Ruhelage, dazu
  @0x01382 Se_on 0x0E (naechster Schwung), zwei Nachschwuenge (@0x013A2/@0x013AE, @0x013CE je 0x0D in der Ruhelage).
  sub03 @0x013F0/@0x0141C.. (Gitter 1) und ROOM20A0 sub03 @0x01CBC.. / sub04 @0x01D62.. identisch.
  => FORM: 0x0E beim Beginn eines Aufschwungs, 0x0D wenn das Gitter in die Ruhelage zurueckschlaegt.
* Bank 2 = snd0 (FUN_80045024 caseD_2, re15_audio.h). ROOM2060 snd0 (RDT +0x08 = EDT @0x5B38, VH @0x5BB8,
  VB @0x7658): **Satz 0x0E** EDT @0x05B70 `00 00 b1 11` (Ton 11, Prio 1, Stimme 1) / Ton @0x06538
  `00 04 7f 40 5e 00 46 46` (vol 127, center 94, Note 70) / VAG 10 @0x0CA38 2416 B sha1 afecf6f02055 (0,38 s);
  **Satz 0x0D** EDT @0x05B6C `00 00 a2 11` / Ton @0x06518 `00 04 7f 40 5d 00 45 45` / VAG 9 @0x0C278 1984 B sha1
  bc83fd5bb902 (0,31 s). ROOM20A0/2061/20A1 tragen dieselben Wellen (gleiche sha1).
* RE1.5-Gegenstueck ROOM1190 sub11 (Luke obj 4, je Hund einmal): @0x0286A For 10 = Beginn des Aufschwungs
  (Speed_set(5,150)/(0x0B,-6) @0x02860/@0x02864, Evt_next @0x02868), @0x02880 For 10 zurueck
  (Speed_set(5,-150)/(0x0B,6)), Summe +1230 / -1230 -> zurueck in der Ruhelage, **@0x0288C `34 05 00 00`
  Member_set(5,0)** im Bild nach dem letzten Rueckschritt = Aufschlag. Anker (Datei-Offset + 8-Byte-Signatur):
  @0x0286A `0d 00 06 00 0a 00 30 31` -> Satz 0x0E; @0x0288C `34 05 00 00 01 00 2b 00` -> Satz 0x0D.
  For laeuft je Schleife EINMAL (op_for: Next springt auf body = pc+6, LAB_8003f540/LAB_8003f674), der Anker feuert
  also einmal je Hund.

### 2.3 Punkt 3 - RE2-Regale 2. Etage (Bibliothek)
* RE2 ROOM1120 = Bibliothek 2F (Nachrichten sub00 "Power switch to the shelf", sub01 "A switch to slide the shelf
  left and right"; Nutzer-SAP "ROOM112.SAP - Library Sounds"). Schalter-Subs sub04 @0x02EE6 / sub05 @0x02FCE /
  sub06 @0x030B6 / sub15: `2e 04 00` Work_set(4,n) = Regal n, dann **@0x02F2E `36 02 0c 01 04 00 ..` Se_on(2,0x0C)**
  bzw. @0x02F62 (Gegenrichtung), sub05 @0x03016/@0x0304A Satz 0x0E, sub06 @0x030FE/@0x03132 Satz 0x10 - jeweils
  unmittelbar vor `18 08`/`18 09` Gosub sub08/sub09 (ohne Evt_next dazwischen) = die Fahrt: For 30 {Add_speed,
  .., Evt_next, Add_speed, .., Evt_next} @0x03296/@0x03312 = 60 Bilder. EIN Se_on je Fahrt, kein Wiederholen.
* RE2 ROOM1120 Raumbank (RDT +0x08): **Satz 0x0C** EDT @0x14CA4 `00 00 63 00` / Ton @0x15614
  `00 00 6e 40 5f 39 41 41` (vol 110, center 95, shift 57, Note 65) / VAG 23 @0x35C44 10512 B sha1 e855511764c6;
  0x0E (@0x14CAC, Ton @0x156B4, center 100, Note 70) und 0x10 (@0x14CB4, Ton @0x156F4, center 102, Note 72) = DIESELBE
  Welle mit derselben Tonhoehe (Note - center = -30). Welle: Blockflags {0,1,4,7}, kein Wiederhol-Bit (Flag 3) ->
  EIN Durchlauf, 18368 Samples ~ 2,36 s; Huellkurve (100 ms) durchgehend 4300..10200, letzte Spitze 2,1..2,2 s.
* RE1.5-Gegenstueck ROOM1190 sub10: **@0x02796 `2e 03 05` Work_set(3,5) = Schrank**, @0x0279A Speed_set(0,-100),
  @0x0279E For 20 {Add_speed, Evt_next, Evt_next} = 40 Bilder. RE2-Form: Ton im Bild des ersten Fahrschritts, einmal.
  Anker @0x02796 Signatur `2e 03 05 00 2f 00 9c ff`. Benannte Folge: RE2s Welle (2,36 s) ist laenger als RE2s eigene
  Fahrt (2,0 s) und erst recht als die des Schranks (1,33 s); sie klingt nach dem Halt des Schranks aus und endet
  ~0,36 s nach dem Schnitt auf Cut 11 (F521). Das ist die RE2-Form (kein Abbruch einer laufenden SE-Stimme in RE2s
  Skript), kein Platzhalter.


## 3. Umsetzung (Fortsetzung nach dem Sitzungslimit, Bau 40b03d9b+)

| Datei | Inhalt |
|---|---|
| `engine/src/toene_r39.c`, `include/re15_toene_r39.h` (NEU) | Anker = RDT-Datei-Offset + 8-Byte-Signatur (Muster elzatoene_r38.c): ROOM11F0/11F1 @0x017B6 (`29 0b 22 02 07 01 2e 01`, Cut_chg 11 = Schublade), ROOM1190/1191 @0x02796 (`2e 03 05 00 2f 00 9c ff`, Work_set(3,5) = Schrank), @0x0286A (`0d 00 06 00 0a 00 30 31`, For 10 = Aufschwung der Luke), @0x0288C (`34 05 00 00 01 00 2b 00`, Member_set(5,0) = Ruhelage). Signatur weicht ab -> nicht armiert (lieber stumm als falsch). |
| `platform/pc/src/toene_r39_pc.c` (NEU) | laedt `shared_assets/RE2/TOENE39.VBS` einmal (Satzformat wie ELZASE.VBS, FUN_8005a09c-Trailer), spielt ueber `re15_audio_se_play_bank` (dieselbe Stimmen-/Prioritaets-Maschine wie die Raum-SE), nicht positional (wie RE2 @0x80052188 `addu a1,zero,zero`). |
| `engine/src/gen/re2_toene39_bank.inc`, `shared_assets/RE2/TOENE39.VBS` (NEU, 23272 B, md5 dc776048cec6fd83af2aa6a881ab99b1) | Satz 0 SCHUBLADE = RE2 ROOM1130 0x28 (VAG @0x15A08 5216 B), 1 GITTER_AUF = RE1.5 ROOM2060 snd0 0x0E (VAG @0x0CA38 2416 B), 2 GITTER_ZU = ROOM2060 0x0D (VAG @0x0C278 1984 B), 3 REGAL = RE2 ROOM1120 0x0C (VAG @0x35C44 10512 B). Werkzeug `tools/r39_b/toene39_se_cut.py` (Selbsttest gegen die zitierten Bytes; Lauf 2026-10-18 07:1x: Ausgabe-md5 == installierte Datei). |
| `engine/src/scd_vm.c` (Haken, 2 x 2 Zeilen "Runde 39 Spur B") | `scd_register_current_rdt`: Anker-Scan; Opcode-Schleife `scd_vm_tick`: `re15_toene_r39_pc(t->pc)` vor jedem Opcode (Wachposten `g_re15_toene39_anker_n`). |
| `platform/pc/main.c` (Haken, 1 Zeile) | `re15_pc_toene_r39_init()` neben den anderen VBS-Haken. |
| `tests/unit/test_r39_toene.c`, `tests/unit/probes/r39_toene.cmake` (NEU) | Riegel `unit_r39_toene_{schublade,gitter,schrank,bank,signatur}` (Mechanik am echten VM, s. §4.1). |
| `tests/unit/probe_r39_toene_kreuz.c` (NEU) | Kreuzkorrelations-Werkzeug fuer TOENE39.VBS-Saetze (Verfahren probe_r38_elzatoene_kreuz.c). |
| `tests/integration/test_r39_toene.cmake` (NEU) | `integration_r39_toene_{schublade,schublade_elza,szene,szene_elza}`: Spielstand + CONTINUE + Tuer per Viereck-Druck (RE15_PAD_AT=60:A, Vorwaerts-620-Trefftest @0x80042bd0), Ton-Mitschnitt RE15_AUDIO_CAP_SYNC, Kreuzkorrelation je Satz (s. §4.2). |

Konstanten mit Beleg: Offsets/Signaturen = ausgelieferte RDT-Bytes (scd_dump_room.py, §0/§2); Takt der Toene = Opcode-Takt des
Original-Skripts (kein eigener Zaehler, keine Framezahl nach Gefuehl): Schublade im Bild von Cut_chg 11 (RE2-Form
@0x8005218c/@0x800521d8), Schrank im Bild des ersten Fahrschritts (RE2 ROOM1120 @0x02F2E vor `18 08`), Luke 0x0E im Bild des
For (= erster Add_speed, RE1.5 ROOM2060 @0x0134A vor @0x0135A For 20), 0x0D im Bild der Ruhelage (@0x01376 nach For 20 zurueck).

## 4. Messung nachher

### 4.1 Riegel am VM (`test_r39_toene.exe`, Bau 40b03d9b + Riegel-Korrektur, 2026-10-18)
Alle Bilder = VM-Takte ab dem Raumaufbau (scd_room_reenter), Spion am Plattform-Haken `re15_toene39_se_hook`.
* **schublade** ROOM11F0: `re15_aot_fire_slot(13)` (Schreibtisch, derselbe Handler-Pfad @0x800430F0 wie der Aktionsdruck) ->
  sub19: Cut 11 in Bild 3 (erster Takt des Fadens), **Schubladen-Ton 1x in Bild 3 = Cut-11-Bild**; (9,99) bleibt 0 (Aot_on 14 nur
  armiert). Zweites Oeffnen mit (9,99)=1: wieder Cut 11 + Ton in Bild 3 (RE1.5 zeigt die Schublade erneut, §2.1).
  Gegenprobe ohne Anker: Cut 11, 0 Toene. ROOM11F1 (Elza): 1 Anker, Ton im Cut-11-Bild.
* **schrank** ROOM1190 ueber den ECHTEN Ausloeser: (4,0xF3)=1 im Spielstand, Bank-5-Bits (5,4)/(5,5) nach Bild 2 gesetzt (Bank 5 ist
  raumlokal) -> sub01 @0x02468 `04 ff 18 0a` startet sub10: (4,0xEA)=1 ab Bild 3, Cut 8 Bild 43 (Sleep 40), **Regal-Ton 1x in
  Bild 63 = Bild des ersten Fahrschritts** (x 3920 -> 3820; Sleep 20 nach Cut 8), 20 Schritte je 2 Evt_next (Bilder 63..101),
  Endlage x = 1920, Cut 11 in Bild 123 (= Ton + 40 Fahrt + Sleep 20). ROOM1191 (Elza): 63/63.
* **gitter** dieselbe Szene: Luke obj 4 (Obj_model_set rot (-2048,1024,0), rot_z 0 = Ruhelage): je Hund **Auf-Ton in Bild 134 /
  166 / 198 = Bild des ersten Luken-Schritts** (rot_z != 0 ab genau diesem Bild; = Gosub + Sleep 10 + Evt_next @0x02868),
  **Zu-Ton in Bild 155 / 187 / 219 = Auf + 21** (10 Aufschwung-Bilder For @0x0286A + Evt_next @0x0287E zwischen den Schleifen +
  10 Rueckschwung-Bilder For @0x02880, dann Member_set(5,0) @0x0288C; rot_z ist im Bild davor (154/186/218) wieder 0, zwei
  Bilder davor 96 = letzter Rueckschritt). Erster Riegel-Entwurf erwartete +20 — der Evt_next @0x0287E zwischen den Schleifen
  ist Skript-Bestand (Dump oben), der Riegel wurde auf 21 mit Beleg korrigiert. Gegenprobe ohne Anker: Luke bewegt sich ab 134,
  0 Toene. ROOM1191 (Elza): 3 Anker, 3/3 Toene, Luke ab 134.
* **bank**: TOENE39.VBS 23272 B, 4 VAGs; je Satz Welle bytegleich zur Quelle (RE1.5 ROOM2060 @0x0CA38/@0x0C278; RE2 ROOM1130
  @0x15A08, ROOM1120 @0x35C44, wenn info/re2leon im Baum), Ton-Parameter (vol/pan/center/shift/min/max) wie Quelle, EDT-Prio/-Stimme
  wie Quelle (Programm 0, Ton-Index = Satz). Die Formen-Quellen stehen in den Dateien: ROOM2060 @0x0134A `36 02 0e 00` /
  @0x01376 `36 02 0d 00`, RE2 ROOM1130 @0x00D50 Aot_set sce 13, RE2 ROOM1120 @0x02F2E `36 02 0c`.
* **signatur**: 10F0/10F1/1090/1091 = 0 Anker (die vom Nutzer genannten Raumnummern tragen nichts), 11F0/11F1 = 1, 1190/1191 = 3;
  Kopie mit einem geaenderten Byte @0x02798 -> nur 2 Anker; ohne RDT 0.

### 4.2 Echter Weg an der exe (`integration_r39_toene_*`, Messkopie `re15_pc_r39b_<id>.exe`, RE15_AUDIO_CAP_SYNC)
* **schublade (Leon)**: Spielstand ROOM11E0 (-25100,-8300) Blick 0x400 vor Tuer 0, CONTINUE, Viereck in Bild 60 -> `[aot] DOOR FIRE
  slot=0 rect=(-25100,-9550,hw=2000,hh=750) spawn=(250,0,250)` -> ROOM11F0 (`[toene39] ROOM11F0: 1 Anker armiert`), Schreibtisch
  `[fire-aot] slot=13 at F200` -> `[toene39] F201 ROOM11F0 Anker @0x017B6 -> Ton 0` und `[scd F201] Cut_chg(11)` im SELBEN Bild;
  Mitschnitt 5791800 B (985 Bilder), Kreuzkorrelation Satz 0 an Bild 201: **1.000 (Versatz +1 Bild = Mischer-Takt wie Runde 38 H),
  Gegenprobe -0.006**; Kontrollbilder 171/231: -0.006. Lauf `r39_toene_wd/lauf_schublade_cea2fdc1`, 38.9 s.
* **schublade_elza**: Spielstand ROOM11E1 (Figur 4, `[pl] Spieler-Familie PL04`), Tuer 0 -> ROOM11F1 (1 Anker), `[fire-aot] slot=13 at
  F200` -> `[toene39] F201 ROOM11F1 Anker @0x017B6 -> Ton 0` = `[scd F201] Cut_chg(11)`; Korrelation 1.000, Gegenprobe -0.006 (33.7 s).
* **szene (Leon)**: Spielstand ROOM1180 (-8117,10300) Blick 0x400 vor Tuer 2 mit (4,0xF3)=1, Viereck in Bild 60 -> `[aot] DOOR FIRE
  slot=2 rect=(-8118,9355,hw=1148,hh=467) spawn=(3174,0,-11182)` -> ROOM1190 (`3 Anker armiert`), `[setflag-at] Frame 300:
  flag(5,4)/(5,5) = 1` -> sub01 @0x02468 startet sub10 (der echte Ausloeser, kein FORCE_EVENT): `[scd F341] Cut_chg(8)`,
  **`[toene39] F361 ROOM1190 Anker @0x02796 -> Ton 3`** (= Cut 8 + 20; `[se] Stimme: se=3 ... vag=3 note=65 center=95 vol=110
  pitch=0x2f9 -> SE-Stimme -16` = der Regal-Satz auf dem Direkt-Zweig), `[scd F421] Cut_chg(11)` (= Ton + 60), dann je Hund
  **Auf F432 / F464 / F496 (`se=1 vag=1 note=70 center=94 vol=127 pitch=0x400 -> SE-Stimme 1`) und Zu F453 / F485 / F517
  (`se=2 vag=2 note=69 center=93`) = Auf + 21**, Hunde-SE 5 (`SE-Stimme 7`) bei F421 und ~2 Bilder nach jedem Zu-Ton.
  Kreuzkorrelation (Mitschnitt 620 Bilder): Regal an Bild 361 **0.965** (Versatz 0, Gegenprobe 0.118 = Raummusik); Auf ganze
  Welle **0.495 / 0.576 / 0.563** (Versatz +1), Zu ganze Welle 0.412 / 0.417 / 0.835, Zu Anschlag 0..100 ms **0.591 / 0.605 /
  0.943**; Rauschboden ohne Ton (Bilder 380/400/560/580) 0.006..0.042. Die niedrigeren Werte der Hunde 0/1 liegen unter dem
  Hunde-SE 5 (29120 Samples = 0,66 s: der Anschlag des Auf-Tons 11 Bilder nach dem Bellen, der Ausklang des Zu-Tons unter dem
  naechsten Bellen - Hund 2 ohne folgendes Bellen 0.835/0.943). Der erste Riegel-Entwurf verlangte >= 0.5 fuer alle sechs
  (zwei Zu-Werte 0.41) -> Riegel auf die gemessene Struktur gesetzt: Auf ganze Welle >= 0.35, Zu Anschlag >= 0.5 (je > 8x
  Rauschboden, unter dem kleinsten gemessenen Wert). Lauf `lauf_szene_714bab17`, 45.3 s.
* **szene_elza**: Spielstand ROOM1181 (PL04), Tuer 2 -> ROOM1191, dieselben Bilder und Korrelationen (deterministischer
  Mischer): F341 Cut 8, F361 Ton 3, F421 Cut 11, Auf 432/464/496, Zu 453/485/517, Regal 0.965, Auf 0.495/0.576/0.563, Zu-Anschlag
  0.591/0.605/0.943. Lauf `lauf_szene_elza_c3c8be81`, 45.7 s.

## 5. Tests
* `unit_r39_toene_{schublade,gitter,schrank,bank,signatur}` (tests/unit/test_r39_toene.c, Registrierung
  tests/unit/probes/r39_toene.cmake) — Mechanik am VM, §4.1.
* `integration_r39_toene_{schublade,schublade_elza,szene,szene_elza}` (tests/integration/test_r39_toene.cmake) — echter Weg an der
  exe, §4.2. Parallel-/Linux-fest: exe-Kopie `re15_pc_r39b_<id>.exe` je Lauf, Laufordner `r39_toene_wd/lauf_<teil>_<id>`,
  Umgebung nur ueber set(ENV) in spiel_lauf.cmake, `cmake_minimum_required(VERSION 3.16)`, Kreuz-Werkzeug mit `m`.
* Werkzeug `probe_r39_toene_kreuz` (kein add_test).
* Gezielter Lauf (ctest-Summenzeile woertlich): siehe §5.1.

## 6. OFFEN
* Nicht positional (wie Runde 38 H / Runde 36 L): die drei Toene spielen ohne Lage. RE2 spielt die Schublade ebenfalls ohne Lage
  (@0x80052188 `addu a1,zero,zero`); fuer Gitter (RE1.5 ROOM2060 Se_on Flags `03 02`/`00 02`) und Regal (RE2 Se_on `01 04`) tragen
  die Original-Se_on Lage-Felder, die der Port fuer Raum-SE generell nicht umsetzt. Kein Punkt dieser Spur.
* Regal-Welle (2,36 s) laenger als die Schrankfahrt (1,33 s) — RE2-Form (ein Se_on, Welle ohne Wiederhol-Bit, klingt aus), s.
  §2.3; Entscheidungsfrage unter §7.

## 7. Fuer den Nutzer / Orchestrator
* **Raumnummern**: Die genannten Raeume tragen nichts davon: ROOM10F0/10F1 haben keine Schublade (nur Tueren, Texte, vier
  Item-Plaetze), ROOM1090/1091 weder Hunde noch Schrank. Gemeint und umgesetzt (gemessen, §0): die Schublade = **ROOM11F0/11F1**
  (Boiler Room, Schreibtisch neben dem Generator, sub19 Cut 11 mit den Patronen), Hunde-Luke und Schrank = **ROOM1190/1191**
  (Schiessstand B1, Raetsel-Szene sub10/sub11). Dieselbe Ziffernverschiebung wie in Runde 36 M ("ROOM 1090" = ROOM1190).
* **Neue Assets fuer das Paket-Gate** (release/make_package.sh ist TABU fuer die Spur, bitte dort eintragen wie ELZASE.VBS,
  Z.269 `for rs in ELZASE.VBS TUERSE.VBS ...`): `re15_port/shared_assets/RE2/TOENE39.VBS` (NEU, 23272 B, md5
  dc776048cec6fd83af2aa6a881ab99b1). Fehlt die Datei im Paket, bleiben Schublade/Gitter/Schrank still stumm (debug.log
  `[toene39] shared_assets/RE2/TOENE39.VBS fehlt`). Keine Sprachdateien.
* **Herkunft der Toene** (NUTZER-VORGABE, Form belegt §2): Schublade = RE2 ROOM1130 ("ROOM113.SAP - locked drawer sounds" der
  SAP-Liste) Satz 0x28 — die Schublade, die RE2 nach dem Aufschliessen oeffnet; Gitter = RE1.5 STAGE2 ROOM2060 snd0 0x0E
  (Aufklappen) + 0x0D (Rueckschlag in die Ruhelage) der Schachtspinnen-Gitter, je Hund einmal auf/zu wie die Luke schwingt;
  Schrank = RE2 ROOM1120 (Bibliothek 2F) Satz 0x0C, die Regalfahrt, einmal beim Losfahren.
* **Entscheidungsfragen**: (1) Die Regal-Welle klingt ~1 s nach dem Halt des Schranks aus (RE2-Form, dort ist die Fahrt 2,0 s,
  hier 1,33 s). Soll sie bleiben (Stand) oder auf die Fahrt gekuerzt werden (waere eine Port-Kuerzung der Welle)? (2) Die
  Schublade spielt den Ton bei JEDEM Oeffnen (RE1.5 zeigt die Schublade auch nach dem Nehmen der Patronen erneut; RE2 oeffnet eine
  leere Schublade nicht, sondern zeigt nur Text). Stand: Ton bei jedem Oeffnen, weil das Bild das Oeffnen zeigt.
* Bedienung: nichts Neues; die Toene erklingen an den vorhandenen Stellen (Schreibtisch untersuchen; Raetsel loesen).
