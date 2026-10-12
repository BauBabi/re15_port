# Runde 37 Spur O "szene11c0" — Ada/Marvin-Szene ROOM11C0 nach Irons' Tod

Baum: `.claude/worktrees/r37_szene11c0` (Zweig `r37/szene11c0`, Basis 07bb463a).
Auftrag: AUFTRAG.md Nachtrag 11 (woertlich):

> Und die Dialogszene mit Ada/Marvin nach Irons Tod ist auch noch nicht so geil von den Animationen....
>
> Erst schauen sie komplett in die falsche Richtung. Dann drehen sie sich einmal um 180 grad um. Dann dreht sich Ada noch mal um 90 grad um. Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada.... da musst du noch mal ran, das ist noch nicht so gut.

Zuteilung (VERTRAG.md §1.4): Spur O aendert Programm 4 (k_p_11c0) in irons_tod_1150.c an Ort und Stelle (Kommentar
"Runde 37 Spur O"); die uebrigen Programme (Spur H: Nachspawn 1130) und k_msg28 (Nachtrag 10) NICHT anfassen.

## Stand (fortlaufend)

- [x] Pflichtlektuere (AUFTRAG/VERTRAG; Vorrunde L_cut1150.md, K_szene1150.md K2, K_abnahme_0.md §3.2)
- [x] Messung vorher (echte exe, Kette wie integration_r35_cut1150 Lauf A, Bild fuer Bild Gier + Kopf)
- [x] RE-Belege (B1 Gier/Aufstellung ROOM10D0, B2 Raum-Vokabular, B3 Kopf-Klemme/INIT/Raten, B4 RE2-Zensus)
- [x] Umsetzung
- [x] Messung nachher (Lauf n3, Auswerter 0 Fehler, Vorher/Nachher-Streifen)
- [ ] Tests (probes/r37_szene11c0.cmake)

## Messung vorher

### Weg und Werkzeug
* Stand: Basis 07bb463a + NUR der Mess-Haken dieser Spur (engine/src/szene11c0_r37.c `re15_s11c0_mess_takt`, Aufruf
  aus `re15_irons_tod_tick`, env-gegatet `RE15_S11C0_LOG`, kein Spielverhalten). Messkopie `re15_pc_r37o_vor.exe`.
* Weg wie integration_r35_cut1150 Lauf A (Werkzeug `re15_port/tools/r36_k/kette_lauf.sh`): Karte
  `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene`, CONTINUE in ROOM1130, Aktionstaste an der Tuer ->
  ROOM1150 Szene -> Montage 1130 -> 1040 -> 1030 -> ROOM11C0 bis `RE15_EXIT_AT=498#11C0` (das Programm schneidet nach
  ~500 Bildern zurueck nach 1150; ein EXIT_AT dahinter wird nie erreicht und die Bildnummern-Dumps wuerden von der
  Rueckkehr nach 1150 ueberschrieben — erster Versuch mit 520#11C0 so verworfen).
* Je Bild: `RE15_FRAMEDUMP=0-498/2:f_` (RE15_WINDOW_SCALE=1, 320x240), `RE15_S11C0_LOG=s11c0.log` (Untertitel, je Figur
  Ort/Gier/Clip/Kopf-Flags +0x1b8/Kopf-Zielpunkt +0x160,+0x164/Kopf-Akku/Gier zum Partner), `RE15_NECK_TRACE`
  (skeleton_common.c: geklemmtes lokales Kopfziel tgt, Akku acc, Kopf-Keyframe kf je gezeichnetem Bild).
* Auswertung `re15_port/tools/r37_o/szene_mess.py <lauf>`: Drehabschnitte je Figur (Summe der Gier-Schritte), je
  Untertitel Koerper- und Kopfblick-Abweichung zum Partner. Kopf-Blick (Welt) = Gier + Kopf-Keyframe + Akku (die
  Kopf-FSM addiert den Akku auf den Keyframe-Euler vy des Kopfteils, skeleton_common.c `ay + a->neck_yaw`).
* Ergebnis: `O_belege/o_vor_mess.txt` (Zahlen), `O_belege/o_vor_bilder.txt` (jedes 4. Bild), Kontaktbogen
  `O_belege/o_vor_11c0.png` (F20..F470).

### Zahlen (o_vor_mess.txt)
```
Drehungen Ada:    F45..F64  Gier  475 -> 2763  -158.9 Grad | F84..F94 2763 -> 3679 +80.5 Grad | F384..F394 3679 -> 2763 -80.5 Grad
Drehungen Marvin: F35..F55  Gier  475 -> 2745  -160.5 Grad | F184..F196 -97.9 | F251..F263 +97.9 | F311..F323 -97.9 | F374..F389 +98.3 (Losrennen)
Untertitel: 10 F95..F193, 11 F197..F251, 12 F252..F323, 13 F324..F394, 14 F395..F493
msg 10 Ada  "What was this noise?":  Ada Koerper 0.1 Grad zu Marvin, Kopf auf Marvin | Marvin (Zuhoerer) Koerper 98 Grad weg,
                                     Kopf-Ziel Ada, aber Kopf an der Klemme in 93/99 Bildern -> Blick 35.4 Grad an Ada vorbei
msg 11 Marvin "Yes!....":            Marvin Koerper 0.1 Grad, Blick max 6.8 Grad (Nicken)        | Ada sieht ihn an
msg 12 Marvin "Oh, no, Leon!":       Marvin Koerper 98 Grad weg (2/72 im Kegel), Kopf-Ziel TUER 60/72, Blick 97.9 Grad an Ada vorbei
msg 13 Marvin "I have to help him":  Marvin Koerper bis 160.5 Grad weg (51/71), Kopf frei 20/71 — er dreht sich und rennt los,
                                     waehrend die Zeile noch steht (F374..F394)
msg 14 Ada "Marvin!...":             Ada Koerper zur Tuer gedreht (80.5 Grad), Kopf zur Tuer
```

### Gegen den Wortlaut des Nutzers (Punkt fuer Punkt)
| Nutzer | gemessen |
|---|---|
| "Erst schauen sie komplett in die falsche Richtung." | F6..F35: beide Gier 475 = Ruecken zur Kamera (Kamera-Blick Cut 13 f = (0,597,-0,802); Gier 475 zeigt (0,746,-0,666), Skalarprodukt 0,98). Ada 78 Grad an Marvin vorbei, Marvin 102 Grad an Ada vorbei. Herkunft: Ada = Original-Record ROOM11C0 sub00 @0x01770 `44 00 42 40 .. fb dc 00 00 f5 c7 00 00 db 01` (rot 0x01db = 475: ihre Warte-Haltung fuer Leons Ankunft im Original), Marvin = r35 hat dieselbe 475 uebernommen (OP_EM rot 475). |
| "Dann drehen sie sich einmal um 180 grad um." | Ada F45..F64 -158.9 Grad, Marvin F35..F55 -160.5 Grad (Schritt (1) "beide zum Gebaeude": die Tuer Slot 0 @0x01712 liegt HINTER der Kamera). |
| "Dann dreht sich Ada noch mal um 90 grad um." | Ada F84..F94 +80.5 Grad (Schritt (2) Ada zu Marvin). |
| "Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada" | msg 12 "Oh, no, Leon!": Marvins Koerper 98 Grad weg, Kopf zur Tuer, Blick 97.9 Grad an Ada vorbei (Schritt (4)). Dazu: bei msg 10 (Ada fragt) steht Marvins Kopf an der NPC-Klemme ±0x2c8 (= 62,6 Grad, @0x8011c7a0) — der Koerper zeigt 98 Grad weg, der Kopf erreicht Ada nicht (35 Grad daneben). Marvin dreht sich in 11C0 insgesamt fuenfmal um ~98 Grad hin und her. |

Plc_neck wirkt bei Marvin (Typ 0x40) sichtbar: neck_trace slot=4 hat Bilder mit fl=84 (Modus 1), der Akku laeuft
auf das Ziel (msg 11: Blick 6,8 Grad zu Ada; msg 10: an der Klemme -707/-712); im Bild F130/F190 (o_vor_11c0.png)
ist Marvins Kopf sichtbar zu Ada gedreht, der Koerper zur Kamera. Die Kopfdrehung allein reicht nicht, wenn der
Koerper mehr als 62,6 Grad vom Partner weg zeigt (Klemme).

### Regression?
Nein. Alle vier Beobachtungen sind die gebaute Choreografie der Runde 36 Spur K (K2, Commit 2eb6c38e "11C0-Dialog
einander zugewandt", in v0.8.24 ausgeliefert; Schritte (1) beide zum Gebaeude, (2) Ada zu Marvin, (4) "Oh, no, Leon!"
zum Gebaeude) auf der Grundlage der r35-Fassung (Spur L: beide zum Gebaeude, Anfangs-Gier 475). Der Nutzer schreibt
"noch nicht so geil" (= noch nicht gut), nicht "wieder falsch". Kein Bisect noetig.

## RE-Belege

### B1 Gier = Richtung zum Gegenueber (Form des Ports, Original-Peilung)
* Das Drehen der Engine schwenkt auf `bearing = FUN_8001a6d4(self, ziel)` ein (FUN_8001aac4 = Port
  `re15_enemy_steer_point`, enemy_ai_common.c: `bearing = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff`;
  actor_locomotion.c `atan2_q12` = FUN_8001a6d4 mit der PsyQ-`catan` @0x800658fc, "+Z-zero-Konvention = Rohwert + 0x400").
  Rohwert 0 = +X, 1024 = -Z. Neue Rechenhilfe `re15_s11c0_gier()` (engine/src/szene11c0_r37.c) = genau diese Form.
* Orte: Ada = ROOM11C0 sub00 @0x01770 `44 00 42 40 00 00 00 ff fb dc 00 00 f5 c7 00 00 db 01 00 00` (-8965,-14347),
  Marvin = (-8163,-13748) (r35 M5, PORT-WAHL mit Herleitung, unveraendert). Peilungen (gemessen im Lauf, gp-Feld):
  Ada -> Marvin **3679**, Marvin -> Ada **1631**; Ada -> Tuer 2763, Marvin -> Tuer 2745 (Tuer = Mitte Slot 0 @0x01712).
* Original-Vorbild fuer "Gier = Peilung auf den Partner" und fuer das AUFSTELLEN per Dir_set: der Marvin/Leon-Dialog
  ROOM10D0 @0x01ABA..@0x01B00 (`scd_dump_room.py`):
  ```
  0x01ABA Cut_chg 0e
  0x01ABC Pos_set  32 00 fb 1d 00 00 91 50   Marvin (Work_set(2,0) @0x01A96) auf (7675,20625)
  0x01AC4 Dir_set  33 00 00 00 ea 06 00 00   Gier 0x06ea = 1770 (Peilung auf Leon 1761: +9)
  0x01ACC Plc_neck 41 01 57 17 00 00 71 4d 64 00   Kopf auf Leon (5975,19825)
  0x01AE2 Work_set 2e 01 00
  0x01AE6 Pos_set  32 00 57 17 00 00 71 4d   Leon auf (5975,19825)
  0x01AEE Dir_set  33 00 00 00 f2 0e 00 00   Gier 0x0ef2 = 3826 (Peilung auf Marvin 3809: +17)
  0x01AF6 Plc_neck 41 01 fb 1d 00 00 91 50 64 00   Kopf auf Marvin
  ```
  = das Original stellt zwei Gespraechspartner beim Schnitt sofort einander zugewandt auf (Dir_set = Peilung) und
  setzt beide Koepfe auf den Partner. Zensus Dir_set in allen 240 RE1.5-RDTs (`O_belege/re15_dir_set_zensus.txt`):
  114 Saetze, 32 davon auf Figuren (Work-Art 1/2), auf NPCs (Work-Art 2): ROOM10D0 @0x01AC4, ROOM3091 @0x01726/@0x0173E,
  ROOM4000 @0x0157A/@0x01666/@0x01684/@0x0172C (derselbe Dialog, der die langsame Kopf-Rate 0x3c traegt, B3), ROOM4010
  @0x00E44; der Rest Requisiten (Work-Art 3).
* Dir_set-Handler LAB_80041080 (selbst disassembliert): `lw a1,340(a0)` (Work +0x154) @0x80041084, `lhu v0,2(v1)` /
  `sh v0,104(a1)` @0x80041088/@0x80041090 (rot_x), `lhu v0,4(v1)` / `sh v0,106(a1)` @0x80041094/@0x8004109c (**rot_y
  +0x6a**), `lhu v0,6(v1)` / `sh v0,108(a1)` @0x800410a0/@0x800410a8, pc += 8.
* Spawn-Gier im Sce_em_set (0x44, FUN @0x800420a0, s2 = pc+2): `lh v0,6(s2)` -> `sw v0,52(s0)` (x, Satz +8),
  `lh v0,10(s2)` -> +0x3c (z, Satz +12), `lhu v0,14(s2)` @0x800421bc -> `sh v0,106(s0)` @0x800421c4 (**rot_y = Satz +16**),
  `sh zero,104(s0)` / `sh zero,108(s0)`. Adas Record traegt dort 0x01db = 475 (ihre Warte-Haltung fuer Leons Ankunft im
  Original-Dialog sub02, der sie erst per Plc_dest m9 @0x018CA zu Leon dreht) — DAS ist die "falsche Richtung" zu Beginn.

### B2 Original-Vokabular des Raums (unveraendert aus r36, K_szene1150.md K2.2)
ROOM11C0 sub02: Plc_neck m1 `41 01 x 0 z 64 00` @0x01886 (Leon->Ada) / @0x018C0 (Ada->Leon); Plc_dest m9 @0x01890 /
@0x018CA; beide Koepfe nacheinander auf denselben Punkt @0x01940 / Sleep 10 @0x0194A / @0x01952; Adas Sprech-Geste
Clip 19 vor/zurueck @0x018D8..@0x018E8; sub03 Nicken @0x01BB2 (`41 00 .. 64 00` Kopf frei) / Sleep 5 / @0x01BC0
(`41 03 01 00 00 00 00 00 00 3c`). Ankunfts-Warten sub05/sub06 @0x01C3E/@0x01C50 wird nicht mehr gebraucht (keine
Koerperdrehung im Gespraech).

### B3 Kopf: Klemme, Raten, INIT
* NPC-Kopf-INIT im STAGE1-Overlay (selbst disassembliert, `re15_disasm.py --bin STAGE1.BIN`): `ori v0,zero,0x40` /
  `sh v0,1528(v1)` @0x8011c78c/@0x8011c790 (Schritt Gier 64), `ori v0,zero,0x30` @0x8011c794 (Neigung 48),
  **`ori v0,zero,0x2c8` / `sh v0,1532(v1)` @0x8011c79c/@0x8011c7a0 (Gier-Klemme +-712 = 62,6 Grad)**, `ori v0,zero,0x138`
  @0x8011c7a4 / `sh v0,1534(v1)` @0x8011c7b0 (Neigungs-Klemme). Der INIT (FUN_8011c6dc, erster KI-Takt des NPC) schreibt
  +0x1b8 (@0x8011c768, SPL-Fall 0x12 @0x8004260c-18) — ein Plc_neck VOR dem INIT wird ueberschrieben (gemessen, s.u.).
* Plc_neck-Handler @0x80041e98 (selbst disassembliert): `ori v0,zero,0x80` / `sb v0,440(v1)` @0x80041ea4/@0x80041ea8,
  `sltiu v0,a1,0x5` @0x80041eac -> Sprungtabelle @0x80010dfc fuer Modus 0..4 (0x12 / 0x04 / 0x08 / 0x2a / 0x58,
  @0x80041ed8..@0x80041f10); Ziel `lhu`+`sh` +0x160/+0x162/+0x164 @0x80041f20..@0x80041f50; Raten `lh a1,8(v0)` ->
  `sb a1,158(v1)` (Gier) / `sb v0,159(v1)` (Neigung) @0x80041f5c..@0x80041f6c. RE1.5 hat KEINEN Entity-Folge-Modus
  (RE2 Modus 5): Modus >= 5 laesst nur 0x80 stehen.
* Zensus aller 200 Plc_neck Modus 1 in den RE1.5-RDTs (`O_belege/re15_plc_neck_m1_zensus.txt`): Gier-Rate 0x64 172x,
  0x60 12x, **0x3c 15x (ROOM4000 @0x015B6 `41 01 9d f2 00 00 15 f6 3c 00`, Dialog mit Message_on @0x015C4)**, 0x78 1x.

### B4 RE2 als Vorbild (Beta -> Retail)
* **Geraeusch-Reaktion = Kopf zuerst.** `tools/r37_o/re2_reaktion_zensus.py` (alle 495 RE2-Leon-RDTs, Walker r30):
  erste Figuren-Reaktion (Work-Art 1/2/3) nach einem Se_on: **Kopf (Plc_neck) 52x, Koerper (Plc_dest m9 / Plc_rot /
  Dir_set) 22x** (`O_belege/re2_reaktion_zensus.txt`). Beispiele: ROOM2000 sub05 Se_on @0x029BA -> Sleep 10 -> Plc_neck m1
  @0x029CA (Rate 0x20) -> Sleep 10 -> erst dann Plc_dest m9 @0x029D8; ROOM1080 sub08 Se_on @0x021BC -> Plc_neck m2 @0x021FE
  -> Sleep 180 -> m0 @0x0220C (nur Kopf, dann zurueck); ROOM11C0 sub02 Se_on @0x00ABC -> Plc_neck m2 @0x00AE0 / @0x00AEE /
  @0x00AFC (Kopf sieht sich um); ROOM5000 sub02 Se_on @0x018BA -> Plc_neck m1 @0x018CE.
* **Aufstellung einander zugewandt.** `tools/r37_o/re2_aufstellung_zensus.py`: 16 Stellen, an denen RE2 vor einer Zeile
  zwei Figuren per Pos_set + Dir_set aufstellt; die meisten zeigen auf den Partner (ROOM3040 sub14 @0x017B2/@0x017C6:
  +30/+4 Einheiten; ROOM6020 sub11 @0x010DC/@0x010F0: +1/-55; ROOM3040 sub12: +65/+295) (`O_belege/re2_aufstellung_zensus.txt`).
* **Der Sprecher sieht den Partner an.** RE2 ROOM3010 sub04 (Dialog Leon <-> NPC, Xa_on + Message_on je Zeile): je Zeile
  der Kopf des Sprechers auf den Partner — Leon `41 01 14 a1 f8 f8 80 a8 3c 1e` @0x0288E (Weltpunkt = Kopf des NPC,
  Rate 0x3c/0x1e), der NPC `41 05 01 00 ..` @0x028B0 / @0x028D4 / @0x02940 (RE2-Modus 5 = auf Work(1,0) = Leon), kurze
  Blicke weg und zurueck nur mit dem Kopf @0x0290E/@0x0291C.

## Umsetzung

### Dateien
* `re15_port/engine/src/irons_tod_1150.c` — Programm 4 `k_p_11c0` an Ort und Stelle neu (Kommentarblock "RUNDE 37 SPUR O"
  mit allen Belegen; der Ablauf der Runde 36 ist dort als ERSETZT markiert). Neue Makros `OP_DIR` (Dir_set, ROOM10D0
  @0x01AC4), `OP_NECK1_LANGSAM` (ROOM4000 @0x015B6), `IT_GIER_ADA_MARVIN 3679` / `IT_GIER_MARVIN_ADA 1631`
  (= re15_s11c0_gier, vom Riegel unit_r37_szene11c0_aufstellung nachgerechnet). Dazu 2 Zeilen Mess-Haken (Include +
  Aufruf `re15_s11c0_mess_takt()` in `re15_irons_tod_tick`, nur in 11C0 waehrend Programm 4, env-gegatet). Andere
  Programme (Spur H) und `k_msg28` (Nachtrag 10) unberuehrt.
* NEU `re15_port/engine/src/szene11c0_r37.c` + `re15_port/include/re15_szene11c0_r37.h`: `re15_s11c0_gier` (Peilung
  in der Form des Ports), `re15_s11c0_winkel_diff`, Mess-Haken `RE15_S11C0_LOG`.
* Werkzeuge `re15_port/tools/r37_o/`: `szene_mess.py` (Auswertung Bild fuer Bild), `re2_reaktion_zensus.py`,
  `re2_aufstellung_zensus.py`.
* Keine Assets, keine Sprachdateien, keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse (Ereignis 21 wie r35).

### Ablauf Programm 4 (Takte = PORT-WAHL; jede Form mit Vorbild)
| Schritt | Opcodes | Vorbild |
|---|---|---|
| (0) Aufstellung im Raumaufbau | Sce_em_set Marvin mit Gier 1631; Work_set(2,0) + Dir_set 3679 (Ada); Work_set(1,0) | Spawn-Gier Satz +16 -> +0x6a @0x800421bc/@0x800421c4; Dir_set ROOM10D0 @0x01AC4 (LAB_80041080); Work_set(1,0) ROOM10D0 @0x01AE2 |
| Koepfe auf den Partner (erster Takt nach dem NPC-INIT, Bild 7) | Sleep 3; Plc_neck m1 Marvin->Ada, Ada->Marvin | ROOM10D0 @0x01ACC/@0x01AF6; INIT schreibt +0x1b8 @0x8011c768 |
| (1) Knall | Plc_neck m1 zur Tuer: Marvin, Sleep 6, Ada; Sleep 36 | RE2 Kopf zuerst (ROOM2000 @0x029CA u.a.); 11C0 sub02 @0x01940..@0x01952 |
| (2) zurueck zueinander, Ada fragt | Plc_neck m1 Ada->Marvin, Sleep 4, Marvin->Ada, Sleep 8; msg 10 + Clip 19 vor/zurueck (40/50) | 11C0 sub02 @0x018D8..@0x018E8 |
| (3) "Yes!...." | Sleep 8; msg 11; Kopf frei, Sleep 5, Nicken, Sleep 20; Kopf auf Ada, Sleep 30 | sub03 @0x01BB2..@0x01BCA |
| (4) "Oh, no, Leon!" | msg 12, Sleep 60 (Marvin sieht Ada an) | Nachtrag 11 |
| (5) "I have to help him, sorry!" | msg 13; Clip 15 vor, 25 + 20 halten, zurueck 26; Sleep 30 (= 101 >= 99 Haltezeit `01 63`) | Arm wie r35; RE2 ROOM3010 sub04 je Zeile Kopf auf den Partner |
| (6) Losrennen | Kopf frei; Plc_dest m5 zur Tuer-Mitte; Sleep 15 | 11B0 sub06 @0x015EA |
| (7) Ada sieht nach | Plc_neck m1 zur Tuer Rate 0x3c; Sleep 4; msg 14 + Clip 15; Sleep 60; Tuer zurueck nach 1150 Cut 7 | ROOM4000 @0x015B6 |

### Zwei Befunde beim Bauen (gemessen, behoben)
* **Plc_neck vor dem NPC-INIT wird geloescht.** Erste Fassung (Lauf n1) setzte die Koepfe schon im Raumaufbau:
  s11c0.log `F17 ... A(... nf=84 nt=(-8163,-13748))` -> `F18 ... A(... s=6 nf=12)` — der INIT (FUN_8011c6dc, enemy_ai_common.c
  state 0: `e->neck_flags = (grid_id & 0x40) ? 0x12 : 0x00`, Original @0x8011c768 / SPL @0x8004260c-18) ueberschreibt +0x1b8.
  Jetzt: Koepfe erst im 3. Takt nach den Riegeln (INIT beider in Bild 7, gemessen n2/n3: `mo 2 -> 1` in Bild 8).
* **Work_set auf Ada haelt ihren INIT auf.** In n1 blieb Ada bis Bild 17 ohne INIT in einer haengenden Greif-Pose
  (`O_belege/o_n1_init_pose.png`, Bilder F10/F16 der Zwischenfassung n1): `re15_scd_slot_event_controlled(slot)` (enemy_ai_common.c) laesst
  einen NPC, den ein SCD-Faden per Work_set haelt, vor seinem INIT nicht ticken. Jetzt gibt `Work_set(1,0)` am Ende des
  Raumaufbaus Ada frei (Form ROOM10D0 @0x01AE2); INIT beider in Bild 7 wie vor der Aenderung (Lauf vor: `F7 s=6`).

## Messung nachher

Lauf n3 (Stand 95ff173f, Messkopie `re15_pc_r37o_n3.exe`, derselbe Weg wie "vorher", `RE15_EXIT_AT=509#11C0`):
`O_belege/o_nach_mess.txt`, `o_nach_bilder.txt`, Kontaktbogen `o_nach_11c0.png`, Kopf-Ausschnitte `o_nach_kopf_zoom.png`,
**Vorher/Nachher-Streifen `o_vor_nach_streifen.png`** (je Beat oben vorher, unten nachher), Auswerter
`test_r37_szene11c0 auswerten` -> `o_nach_auswerten.txt` (0 Fehler) gegen `o_vor_auswerten.txt` (alter Stand: 9 Fehler).
```
Drehungen Ada:    keine
Drehungen Marvin: F385..F400 Gier 1631 -> 2749 +98.3 Grad (Losrennen, nach dem Ende von msg 13 in F383); danach nur das
                  Nachsteuern im Lauf (+4.7 / -8.6 Grad)
Untertitel: 10 F72..F169, 11 F170..F224, 12 F225..F284, 13 F285..F383, 14 F405..F503
msg 10 Ada:    Sprecher Koerper 0.1 Grad, Kopf auf Marvin 98/98 (Blick 0.0 Grad) | Zuhoerer Marvin Koerper 0.1, Kopf auf Ada 98/98
msg 11 Marvin: Koerper 0.1 Grad, Kopf auf Ada 30/55 + frei/Nicken 25/55, Blick max 6.2 Grad (Nicken) | Ada Kopf auf Marvin 55/55
msg 12 Marvin: Koerper 0.1 Grad, Kopf auf Ada 60/60, Blick 0.0 Grad | Ada Kopf auf Marvin 60/60
msg 13 Marvin: Koerper 0.1 Grad, Kopf auf Ada 99/99, Blick 0.0 Grad | Ada Kopf auf Marvin 99/99
msg 14 Ada:    Koerper bleibt (Marvins alter Platz), Kopf zur Tuer an der Klemme 99/99, Blick zum laufenden Marvin 7..32 Grad
               (der Koerper 61..87 Grad) — "sieht ihm nach", soweit die Kopf-Klemme 0x2c8 reicht
Knall (vor msg 10): beide Koepfe zur Tuer (Marvin ab Bild 18, Ada ab Bild 24), Koerper unveraendert
```
Gegen den Wortlaut des Nutzers:
| Nutzer | nachher |
|---|---|
| "Erst schauen sie komplett in die falsche Richtung." | Im ersten Bild (F6) Ada Gier 3679 = Peilung auf Marvin, Marvin 1631 = Peilung auf Ada: einander zugewandt (Profil zur Kamera), Bild F8/F14. |
| "Dann drehen sie sich einmal um 180 grad um." | Keine Koerperdrehung beim Knall; nur die Koepfe drehen zur Tuer (Kopf an der Klemme = Gesicht zur Kamera, Bild F32). Groesste Drehung Ada 0, Marvin 98,3 Grad nur beim Losrennen. |
| "Dann dreht sich Ada noch mal um 90 grad um." | Ada dreht ihren Koerper in der ganzen Szene nicht (0 Abschnitte); beim Nachsehen nur der Kopf. |
| "Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada" | Bei allen drei Zeilen (msg 11/12/13) Koerper 0,1 Grad und gezeichneter Kopf-Blick <= 6,2 Grad auf Ada; er rennt erst los, wenn seine letzte Zeile abgelaufen ist. |

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
