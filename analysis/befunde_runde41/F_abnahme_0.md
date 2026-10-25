# Runde 41 — Spur F "gatorszene": Abnahme 0 (unabhaengig)

Stand: Zweig `r41/gatorszene` HEAD `dd2d59ae` (Basis r41/integration `9f10bdb0`), Baum
`.claude/worktrees/r41_gatorszene`. Pruefer: Abnahme 0 (2026-10-25). Ich glaube dem Bau-Agenten nichts, was ich nicht
selbst am gebauten Stand gemessen habe. Abgeschlossen 2026-10-25.

## 0. Bau und Lektuere

- Gelesen: AUFTRAG.md (bis Nachtrag 24b; master hat danach nur den Nachsatz "marvin" zu 24b, betrifft G), VERTRAG.md,
  Dossier F_gatorszene.md, `git log master..HEAD`, `git diff 9f10bdb0..HEAD` (F-eigener Code, 30 Dateien, +4699/-9) und
  `git diff master --stat` (105 Dateien inkl. Spur B; die 3 `synchro/.../main46.wav`-"Loeschungen" sind master-Zugaenge
  NACH der Basis (3da82407), nicht F).
- Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, `ninja: no work to do`
  (exe 2026-10-24 23:35, alle Engine-/Plattform-Quellen aelter; neuer sind nur Testdateien, deren Ziele ebenfalls aktuell).

Messordner (scratchpad, nicht im Repo): `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\F41a0`
(`runs/<lauf>/debug.log|state.log|gator_boss.log|fd_*.ppm`, Bildboegen `*.png`, Helfer `tools/lauf.sh` = eigene exe-Kopie
`re15_pc_f41a0_<lauf>.exe`, `timeout`, danach geloescht). Alle Laeufe mit `RE15_WINDOW_SCALE=1`, Bilder nur per `RE15_FRAMEDUMP`.

## 1. Gezielte Tests (Regex 129 Tests, `-j3 --timeout 600`)

Regex = Bau-Agent-Regex + alle Gruppen der beruehrten Module/Raeume (Spinnen-/2090-Riegel `unit_r38_spinnen`,
`integration_r38_spinnen_echtlauf`, `unit_re2_spider_*`, `unit_spider_ai`, Tuer-Riegel `unit_r30_tuer*`, `unit_r33_tueren_*`,
`unit_r36_tueren_*`, BGM `unit_rotor_bgm_pin`, `unit_r35_cut10f0_bgm`, `unit_r39_johnruf1041_musik`, Knall `unit_r35_cut1150_knallbank`,
`unit_r36_szene1150_knall_re2`, `integration_r39_elzazombies_knall`, Inventar/Item `unit_inv_*`, `unit_item_*`, `unit_r39_inventar_*`,
`unit_scd_*` ...). Ergebnis: **98% tests passed, 3 tests failed out of 129** (895 s). Einzeln nachgefahren (deterministische
Unit-Riegel, keine Fenster-Haken): wieder **0/3**:

| Riegel | Befund (Zeile aus der Ausgabe) | Ursache (F) |
|---|---|---|
| `unit_r36_tueren_schott` | `FEHLER: ROOM2090: keine begehbare Tuer nach ROOM20A0` | F widmet 2090 Slot 1 zum sce-3-Ereignis um (Tor zu, Nachtrag 21); der Riegel (Runde 36, Durchgang 2090 -> 20A0, DOOR26 V0) wurde nicht angepasst (z. B. Bit (9,113) setzen) |
| `unit_r38_spinnen` | `FAIL: RE1.5 ROOM2090 Cut 0: 0 Spinnen ...` / `FAIL: RE2 ROOM2090 Cut 0: 0 Spinnen ...` (`test_r38_spinnen: 2 FAIL`) | Spinnen verworfen (Nutzer-Punkt 2) - die Zensus-Zeile 2090 des Runde-38-Riegels erwartet noch Spinnen; kein belegter Querbezug (wie bei `probe_sca_wall_hit` C2 gemacht) |
| `unit_r30_tuer_verschlossen` | `FEHLER: ROOM20A0 Platz 0 msg 1 Art M: 2 Aufrufe, Satz 0 (soll 1 x 0)` (ebenso 20A1) und `Tabellen-Plaetze 54 ... statt 52` | **Verhaltensfehler**: `tor_druck` spielt in 20A0 den Verschlossen-Ton selbst (`re15_audio_re2_door_se(..ART_M)`, gatorszene_r41.c:937) UND zeigt Original-msg 1, die in `gen/lock_se_sites.inc:49` (`{ 0x20A0, 1, RE15_LOCK_ART_M, 1 }`) steht -> `re15_lock_se_notice` (scd_vm.c re15_scd_show_message) spielt denselben Ton ein zweites Mal im selben Bild |

D's Abnahme 0 hatte `unit_r38_spinnen` gruen (D_abnahme_0.md:143); die drei Riegel standen NICHT in der Regex des Bau-Agenten
(dessen "81/81" ist richtig gemessen, aber zu eng gewaehlt). Die uebrigen 126 sind gruen, darunter alle 8 `unit_r41_gatorszene_*`
und beide `integration_r41_gatorszene_{leon,elza}`, `integration_r38_spinnen_echtlauf`, `integration_r41_kleinkram_a_blink`.
`integration_r41_re2gator` (Spur B) selbst nachgefahren (`ctest -R "^integration_r41_re2gator$"`, 57 s): **rot**,
`r41_re2gator: RE2-Intro (Sub 0 @0x8010086c-948) nicht mit gewaehlter ...` (test_r41_re2gator.cmake:268) - Ursache wie Dossier §O1
(Debug-Sprung nach 2090 startet jetzt die Ankunftsszene; Tor 20A0 zu); bleibt Mangel (M4).

## 2. Punkt 2 — "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

- RE-Beleg selbst gelesen: ROOM2090.RDT und ROOM2091.RDT sind bytegleich (`cmp` ohne Ausgabe); `@0x00AB0 44 00 25 00 00 00 00 73
  9c ff 00 00 98 df` und `@0x00AC4 44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2` (xxd). Eigener SCD-Walker (main + subs, Opcode-
  Laengen aus scd_vm.c) ueber 2090: genau diese zwei Sce_em_set, sonst keine Spinne; 0x73/0x74 werden in keinem STAGE2-RDT
  per Ck/Set (Bank 7) gelesen -> das Verwerfen bricht kein Skript. Kill-Flag-Tor selbst disassembliert (re15_disasm.py):
  `80042120 lbu a1,7(a1)`, `80042124 ori v0,zero,0xff`, `80042128 beq a1,v0,...`, `80042130 jal 0x8004efe4`;
  `80042230 ori v0,zero,0x8000` / `80042238 sw v0,0(s0)` (Satz verworfen) = zitierte Form stimmt.
- exe-Messung (state.log, Zeilen mit ` t=25 `): L1 Leon Leiterweg 1289 Bilder **0**, E1 Elza 2081->2091 1059 Bilder **0**, K2/T0/P2
  (Kampf/Tod/Aufnahme) **0**; je 2090/2091-Besuch zwei Zeilen `[gatorszene] ROOM2090|2091: Spinne 0x25 bei (...) verworfen`.
- Nebenwirkung: `unit_r38_spinnen` rot (s. §1) - Riegel nicht nachgezogen.
- **Urteil Punkt 2: erfuellt** (im Spiel). Der rote Fremd-Riegel ist ein Vertrags-/Suite-Mangel (M4), kein Spielfehler.

## 3. Punkt 1 — Ankunftsszene 2090, Schluessel, Tod (Wortlaut + Nachtraege 6/17/20/21)

### 3.1 Szenenfolge an der exe (Leiterweg, Leon L1/L2/L3/L4, Elza E1)
Befehl (Leon): `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log
RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=700#2090 RE15_AUDIO_CAP_SYNC=audio.raw` (Leitertuer ROOM2080 Slot 1
@0x00BC2 -> 2090 (-8200,0,-25550), selbst im RDT gelesen). debug.log L1 (Auszug, Bildzaehler je Raum):
`Tor gesperrt` -> `Folge A F40` -> `P 1` -> `Cut_chg(1) F7` -> `T F98 wert=0/1` (Knall-Bank geladen + Satz 0) -> `P 2 ROOM2080` +
`M 8/2` -> `Cut_chg(2) F6` -> `P 3 ROOM2090` -> `Cut_chg(1) F7` -> `S F53` -> `Buehne ... S (-4624,-13590) Gier 1024` -> `G 1/7 F53`
-> `G 2/0 F176` -> `K 13 F189` -> `G 3 F190` -> `G 4 F310` -> `V F369` -> `G 5 F369` -> `G 6/0 F376` -> `G 7 F405` -> `B 1 F407` +
`[bgm] entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs, SEQ 100B`. Elza (E1, 2081/2091, `Spieler-Familie PL04`): identische Folge.
Bildboegen: `L2_ankunft.png` (Cut 1 Leiche, F4-F140), `L3_2080.png` (Cut 2, acht Zombies), `L4_gator1/2/3.png` (Durchbruch Cut 7,
Biss/Schuetteln/Kauen Cut 0, Wende/Schrei), `E1_szene.png` (Elza).

| Teil (Wortlaut) | Messung | Ergebnis |
|---|---|---|
| "sieht ... einen Zombie ... mit einem blinkenden Schluessel" | L2 Cut 1 F8-F140: Leiche gross im Bild; Schluessel = Ring aus 6-12 hellen Pixeln (Kasten ~10 x 8, `L2_key_zoom.png`, `L2_key_peak_trough.png`); hellstes Pixel (tools/region.py, Ausschnitt 116..140 x 120..142) pendelt 92 -> 112 mit Periode 60 (Tief F60/F120, Hoch F16-F48 und F76-F108 = **Plateau 112 ueber ~34 von 60 Bildern**) | sichtbar, aber Puls nur +22 % und abgeschnitten; Feuerloescher 1030 (D_kleinkram_a.md 2.4): 115 -> 164 = +42,6 %, Dreieck ohne Plateau. Ursache: Lage/Kamera wurden auf maximales Licht gewaehlt (Licht 210 von 255, leiche_lage.py), der Puls x0xBE/0x80 saettigt bei 255 -> 255/210 = 1,21 |
| "Es schwimmt bereits ein Zombie im Wasser, nimm ... diesen" | obj 2 (Obj_model_set @0x00A6C) an die Szenenlage (-5500,-1740,-23700) Gier 384 versetzt (Nachtrag 6 erlaubt das) | erfuellt |
| "Dann kommen oben in 2080 die Zombies rein" + N17 Knall + N20 oben | Knall: `Folge T wert=0/1`, audio.raw Ticks 573-593 Spitze 18920-19005 (sonst ~6000) = Satz 0 (`gen/knall_bank.inc` Kopf: RE2 ROOM1060 Se_on 0x0a @0x00F48, derselbe Satz wie die Irons-Montage `irons_tod_1150.c:155 OP_KNALL(RE15_IT_SIG_KNALL_TUER)`); 2080 Cut 2 150 Bilder, state.log 156 Bilder mit 8 x `t=16`, x -13900..-9900 (> -16650), alle 8 gelaufen (1251..5359 Einheiten) | erfuellt |
| "Dann erneut Kamera auf Zombie mit Schluessel" | L4 F10/F40 Cut 1 (Leiche + Ring) | erfuellt |
| "Aligator ... aus dem Wasser springen und den Zombie fressen inklusive dem Schluessel" | L4 F54-F176 Cut 7: Kopf taucht im Westkanal auf, kommt frontal mit offenem Maul; Cut 0 F178-F368: Biss, Leiche in der Luft/im Maul (`L4_biss_zoom.png`, `L4_schuetteln_zoom.png`), F369 `V` = Leiche + Schluessel weg | erfuellt (Inszenierung frei nach N6) |
| N6 "Macht danach Richtung Leon eine Schrei und Schuettel Animation" | Wende 1024 -> 1505 in 7 Bildern (75/Bild, state.log), Clip 10 F376-F404, SE 0, Ereignis 1 (Rumble + Schuetteln, Hintergrund-Versatz gemessen dy 0/-1, RE2-Tabelle @0x8009daf4: 0..1 px) | erfuellt (Form RE2-R4, Bild `L4_schrei_zoom.png`) |
| N6 "Danach geht der Kampf los, mit der Kampfmusik von RE2" | `B 1 F407`, MAIN3C-Platz -> `RE2/SOUND/MAIN1A.BGM` (md5 01835748... = info/re2leon, selbst geprueft) | Musik erfuellt; **aber Kamera s. 3.2** |
| "Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe seiner Leiche sein und blinken" | s. 3.3 | **nicht erfuellt (unsichtbar)** |
| N21 Leiter statt Tor, Tor zu bis Schluessel | s. 3.4 | erfuellt (bis auf Doppelton 20A0, M3) |

### 3.2 Kampfbeginn: Kamera zeigt die Spielfigur nicht (Leon UND Elza)
`gr2_kam_frei` kehrt am Szenenende auf den Cut VOR dem Programm zurueck (gator_boss.log `RE2 KAMERA SZENE zurueck -> Cut 1 F405`),
das ist P3s eigener Leichen-Cut 1. state.log L1: `F405 cam=1`, `F420 pm=0` (Steuerung frei), Cut 1 bleibt bis F699. Kamera Cut 1
pos (-7740,-3132,-25956) Ziel (5724,-18,-21654) (debug.log `[F60] cut=1`); Leon (-8392,0,-25550): Skalarprodukt (Leon - Kamera)
. (Ziel - Kamera) = -2 915 208 < 0 -> **hinter der Kamera**. Bilder `L4_kampfstart.png` (F406/F430/F460: nur Gator-Ruecken im
Vordergrund, kein Leon) und `E1_szene.png` F420/F468 (Elza: das Maul fuellt das Bild, Elza unsichtbar). Erster Treffer auf Leon
F466 (hp 100 -> 50), 46 Bilder nach Steuerungsfreigabe, bei unsichtbarer Figur. Die Leiter-Ankunft des Originals setzt Cut 0
(ROOM2080 Slot 1 @0x00BC2 Ziel-Cut `00`). -> Mangel M1.

### 3.3 Schluessel nach dem Tod — Lage ja, SICHTBAR nein
Zwei Todeswege gemessen, je Differenzmessung "mit Schluessel" gegen "ohne Schluessel" (gleicher Lauf, nur `RE15_SET_FLAG` + `9:112`
= Taken-Bit vorab, dann legt `key_bei_leiche` nichts hin; sonst bitgleiche Eingaben), Bild fuer Bild exakt verglichen
(tools/diff0.py, jede Abweichung > 0 zaehlt):

1. **Echter RE2-Kanistertod R1** (Rezept K aus B_abnahme_0.md: `RE15_SET_FLAG=9:110 RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=5
   RE15_PLAYER_POS=6500,-25000,3072 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3` + Schuesse): gator_boss.log
   `RE2 KANISTER angehaengt F18`, `RE2 R1 start`, `RE2 R1 ende (Leiche) pos=(3413,-25001)`; debug.log `[gatorszene] ROOM2090: Water
   Key (Item 0x34 ...) bei der Leiche (2795,-1620,-26820), Zone Slot 15`, `Folge L F109`, `Folge B 0/0` (Musik aus);
   `[prop-render] pi=4 oid=0x04 pos=(2795,-1620,-26820) rot=(0,0,0)`. Kamera per `RE15_FORCE_CUT` = 0..12, Bild F302 (und F332,
   halbe Blinkperiode spaeter): **in allen 13 Kameras bitgleich** mit dem Lauf ohne Schluessel (`KC*`/`KD*`, Bogen `KC_cuts.png`).
2. **Todesrolle GB_TEST=3** (wie Dossier-Lauf tod1; `RE15_R39_MESS_SPIELER_HP=100`): Schluessel (-7277,-1620,-25029), Gator-Leiche
   (-5416,-22578), laufende Kamera = Cut 0 (state.log F470-F519 `cam=0`). Cut 0 und Cut 1, F480/F510: **bitgleich**.
3. Gegenprobe Ersatzlage (Bits 110+111 vorab, `RE15_KEIN_GATOR=1`, Schluessel (-4595,-1620,-24305)), Cut 1: 7 Pixel Unterschied,
   Kasten (122,158)-(129,161), hellstes Pixel 96 (F60) / 112 (F90) -> der Zeichner kann den flachen Schluessel zeigen; an den
   beiden gemessenen Todeslagen tut er es nicht (flach auf y -1620 + Lage/Licht der Kopf-Stelle; das Dossier selbst: "in Cut 0 bei
   Lauf tod1 nicht erkennbar", §O3 - als OFFEN statt als Mangel gefuehrt).
Aufnahme selbst funktioniert (P2: Teleport an die Ersatzlage, Quadrat F60/F100 -> `[inv-r39] F130 ... inv=03:15,15:50,34:1`), der
Name ist die Original-Namenszeile 0x45 "Water Key" (Bytes `33 3d 50 41 4e 00 27 41 55` = gen/inv_name_bank.inc Eintrag 0x45,
selbst dekodiert), Modell/Icon/Bild `schluessel_export.py --pruefen` -> `bytegleich`. Wortlaut "soll der Schluessel in der Naehe
seiner Leiche sein und blinken": Lage ja (Kopf-Stelle, 1900 bzw. 3080 Einheiten von der Wurzel), Blinken als API ja, **gesehen
werden kann er in beiden gemessenen Todesfaellen nicht** -> Mangel M2.

### 3.4 Tor 2090 <-> 20A0 (Nachtrag 21) und Leiterweg
- RDT selbst gelesen: 2090 Slot 1 @0x009B6 `3b 01 02 31 01 00 5e 1a 4c b9 e8 03 ac 0d 56 13 f0 f1 04 10 00 04 01 0a 0d` (-> 20A0
  (4950,-3600,4100) Cut 13), 20A0 Slot 0 @0x01746 (-> 2090 (6300,-1800,-16250) Cut 2), Leiter 2080 Slot 1 @0x00BC2 -> 2090
  (-8200,0,-25550) Cut 0; 2090 hat nur diese zwei Tueren, 20A0 nur Slot 0 nach 2090 (eigener SCD-Walker) -> keine Umgehung.
  Texte ROOM20A0 @0x2008: Offsettabelle `06 00 34 00 44 00`, msg 1 @0x203C `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 01 00`,
  msg 2 @0x204C `04 02 35 4b 51 3a 52 41 00 4b 4c ...` = k_tor_msg_zu/auf bytegleich.
- exe G1 (Leon, ohne Schluessel, `RE15_PLAYER_POS=6630,-16350,0,1`, Quadrat F60/F160): `[msgtext] room=2090 id=42 "It's locked."`,
  `Folge Z`, kein Raumwechsel (Bild `G1_tor_zu.png`). GE (Elza 2091): ebenso msg 42.
- exe G2 (Leon, `RE15_GIVE=0x34:1`): `id=43 "You've opened the lock."`, `Folge U`, Wegwerf-Abfrage (`wegwerf=1 fsm=4..6`), Schluessel
  weg (`inv=... 00:0` F220), danach `[tuer] Sequenz ... DOOR26 Variante 0` -> `R20A0`. GE2 (Elza): dieselbe Folge -> `R20A1`.
- exe G3 (20A0 ohne Bit 113): `id=1 "It's locked."`, `Folge Z`, tuerse.log `F60 raum=20A0 nachricht=1 art=M weg=AOT satz=0(ZU_A)`
  (Tabellen-Ton) + der eigene Ton in `tor_druck` (gatorszene_r41.c:937) -> **zwei Toene** (Riegel `unit_r30_tuer_verschlossen`
  zaehlt `2 Aufrufe`) -> Mangel M3.

## 4. RE-Gate (Stichproben selbst disassembliert/gelesen)

| Zitat im Diff | selbst gelesen | Ergebnis |
|---|---|---|
| Kill-Flag-Tor FUN_800420a0 @0x80042120-38, `ori v0,zero,0x8000`/`sw v0,0(s0)` @0x80042230-38 (RE1.5 PSX.EXE) | `lbu a1,7(a1)` / `ori v0,zero,0xff` / `beq` / `jal 0x8004efe4`; `ori v0,zero,0x8000` / `j 0x8004261c` / `sw v0,0(s0)` | stimmt |
| RE2 EM23 Clip 4 @0x801010ec-f4, Clip 5 @0x80101180-88, Clip 6 @0x801008ac-b8, Clip 10 Blend 3 @0x80102c40-48, SE 0 @0x80102c0c/10, Wende 75 @0x801025f8-608, Blend 32 @0x80100c38, Biss-Sperre 110 @0x8010091c-20, Stoss 0x28 @0x801003f0-40c, Koppelbild 13 @0x80101100-04 | re2_disasm.py `--bin EM23_OVL_0000.BIN`: jeweils `addiu a1,zero,<clip>` + `jal 0x8001a330`, `addiu a2,zero,3`, `addu a0,zero,zero` + `jal 0x8005bd6c`, `addiu a2,zero,75` + `jal 0x8001569c`, `addiu a2,zero,32`, `addiu v0,zero,110` / `sb v0,568(s0)`, `lui v1,0x2800` / `sb v0,551(s0)`, `addiu v0,zero,13` / `bne v1,v0` | stimmt. Ungenauigkeit: Clip 11 @0x801011e0 ruft `jal 0x8001a240` (Clip-Start OHNE den FUN_80015e7c-Wurzelschritt), nicht FUN_8001a330 wie im Dossier ("FUN_8001a330-Rufe"); der Port (ga_anker_lage = FUN_80015cb8-Form) verhaelt sich trotzdem wie RE2 |
| RE2-Tuerhandler LAB_80051514 (@0x80051628 `jal FUN_800696cc`, `bltz` @0x80051634, Text @0x8005164c, Se_on(2,0x25) @0x80051654/58, Wegwerf @0x80051670, Schlossbit @0x80051690, kein Durchgang @0x80051698, Verschlossen-Ton @0x800516a0/a4, Text @0x800516b8) | alle Instruktionen so vorhanden (`lui a0,0x225` / `jal 0x8005ba28`; `lui a0,0x216` / `jal 0x8005ba28`) | stimmt. RE2 spielt im Nicht-Treffer-Zweig EINEN Ton; 20A0 im Port zwei (M3) |
| RE2-Musik INIT_TBL @0x2DA `5a ff` (+ @0x19A2), ROOM40A0 SCD-main +0x71F `51 00 01 00 78 40`, +0x762/+0x78A `51 00 05 ..` | xxd: `149a 5aff` an 0x2DA und 0x19A2; ROOM40A0.RDT SCD-main @0x25C0, Muster bei Datei 0x2CDF/0x2D22/0x2D4A = +0x71F/+0x762/+0x78A | stimmt; MAIN1A.BGM md5 = Quelle |
| RDT-Bytes 2090 @0x00AB0/0x00AC4/0x00996/0x009B6, 20A0 @0x01746/@0x203C/@0x204C, Namenszeile 0x45, Modell ROOM7040 @0x354C | xxd + Dekodierung + `schluessel_export.py --pruefen` -> `bytegleich` | stimmt |

Konstanten-Pruefung gatorszene_r41.c: Szenen-Lagen/-Zeiten stehen im Block "PORT-WAHL der Inszenierung (Nachtrag 6)", Knall 45
und Montage 150 mit Port-Vorbild (irons_tod_1150.c), RE2-Werte mit Adresse. Ohne Beleg und ohne Kennzeichnung: `if (fertig ||
s_ga.t > 60)` (Z. 702, Notbremse der Wende: 60 Bilder); `GSZ_LEICHE2_BILDER 45` ohne Vorbild (nur im PORT-WAHL-Block). Suche im
Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO/vorerst: 0 Treffer; kein getenv im F-Code. Erklaert der Fix
den Befund? Spinnen: ja (Satz-Waechter genau auf die zwei RDT-Saetze). Szene: Mechanik belegt; die zwei sichtbaren Fehler (M1
Kamera, M2 Schluessel) hat der Bau-Agent nicht gemessen bzw. nur als OFFEN gefuehrt.

## 5. Vertrag

- Bits: nur (9,110..113) (VERTRAG 1.1: F = 110-113), k_port[] in test_r36_kartenbits.c eingetragen (4 Zeilen). Raum-Scratch
  (5,28/29/30) nur in 2090. Nachrichten: 2090/2091 Ids 42/43 (zugeteilt 42-46). AOT-Plaetze 15/20 und Ereignisse 18/19/20 frei
  (Zensus des Dossiers, nicht beanstandet).
- Pfade: `git diff 9f10bdb0..HEAD --name-only` ohne release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  Test-CMakeLists (neue probes/r41_gatorszene.cmake = uebliches Muster); neue RE2-Daten unter shared_assets/RE2/SOUND/ (im Dossier
  genannt). Die drei `synchro/...main46.wav` in `git diff master` sind master-Zugaenge nach der Basis (3da82407), nicht F.
- Gemeinsame Dateien: kleine markierte Haken ("Runde 41 Spur F") in scd_vm.c (4), scd_room_setup.c (2), enemy_ai_boss_gator.c
  (2 Haken, einer 6 Zeilen), main.c (4 Stellen, Prop-Lader-Block 6 Zeilen), audio_pc.c (2 x 2), Item-/Inventar-Dateien (1-4).
- Riegel je Punkt vorhanden und messend (8 Unit-Teile, 2 exe-Laeufe, gruen). Nebenwirkungen NICHT abgesichert: 3 bestehende
  Riegel rot + `integration_r41_re2gator` (Spur B) rot -> M4. Linux: `if(NOT WIN32) m`, cmake_minimum_required vorhanden; die
  exe-Kopie traegt fest ".exe" (wie der Riegel von Spur B; VERTRAG "keine .exe-Pfade" - nur Hinweis).

## 6. Maengel (nummeriert, nachpruefbar)

- **M1 Kampfbeginn ohne Sicht auf die Spielfigur (Leon UND Elza).** Szenenende `gr2_kam_frei` -> "zurueck -> Cut 1 F405"
  (gator_boss.log), Cut 1 bleibt im ganzen Kampfanfang (state.log L1 F405-F699 `cam=1`); Leon (-8392,0,-25550) liegt hinter der
  Kamera Cut 1 (Skalarprodukt -2 915 208), Bilder L4 F406-F460 und E1 F420-F468 ohne Spielfigur; erster Treffer F466 (hp 100 -> 50)
  46 Bilder nach der Steuerungsfreigabe (F420 `pm=0`). Soll: die Szene uebergibt die Kamera der Spielerlage (Leiter-Ankunft =
  Cut 0, ROOM2080 Slot 1 @0x00BC2 Ziel-Cut 0). Messung danach: im ersten freien Bild ist die Spielfigur im Bild, Leon und Elza.
- **M2 Water Key nach dem Tod unsichtbar.** Echter R1-Kanistertod: Schluessel (2795,-1620,-26820), in allen 13 Kameras von 2090
  bitgleich zum Lauf ohne Schluessel (F302/F332); GB_TEST=3-Tod: (-7277,-1620,-25029), laufende Kamera Cut 0 und Cut 1 bitgleich
  (F480/F510). "soll der Schluessel in der Naehe seiner Leiche sein und blinken" ist fuer den Spieler nicht erfuellt; das Dossier
  fuehrt es als OFFEN 3 ("nicht erkennbar") statt es zu loesen. Soll: Lage/Ausrichtung/Licht so, dass der Schluessel in der
  Zonenkamera an der Todesstelle sichtbar ist und pulsiert (Messung: Differenzbild > 0 und Puls ueber 60 Bilder, beide
  Todeswege, Leon und Elza).
- **M3 Doppelter Verschlossen-Ton am Tor in 20A0/20A1.** `tor_druck` (gatorszene_r41.c:937) spielt Art M selbst, und die
  Original-msg 1 von 20A0 steht in gen/lock_se_sites.inc:49, also spielt `re15_lock_se_notice` denselben Ton ein zweites Mal
  (exe G3 tuerse.log + Riegel `unit_r30_tuer_verschlossen`: "2 Aufrufe, Satz 0 (soll 1 x 0)"). RE2 LAB_80051514 spielt im
  Nicht-Treffer-Zweig genau einen Ton (@0x800516a0/a4).
- **M4 Rote Riegel durch F, nicht nachgezogen bzw. nicht gefahren:** `unit_r36_tueren_schott` (2090 -> 20A0 ohne Bit 113 nicht
  begehbar), `unit_r38_spinnen` (2 FAIL: der Zensus erwartet Spinnen in 2090), `unit_r30_tuer_verschlossen` (Tabellenzahl 52 -> 54
  und M3), `integration_r41_re2gator` (Spur B; selbst nachgefahren: rot "RE2-Intro (Sub 0 @0x8010086c-948) nicht mit gewaehlter
  ..."). Jeder braucht einen belegten Querbezug (wie `probe_sca_wall_hit` C2) bzw. einen Orchestrator-Entscheid; "Riegel gruen"
  galt nur fuer die zu enge Regex des Bau-Agenten.
- **M5 Blinken in der Ankunftseinstellung gesaettigt.** Hellstes Schluessel-Pixel 92 -> 112 (+22 %) mit Plateau 112 ueber ~34 von
  60 Bildern (L2 F8-F140) statt des Dreieckspulses des Feuerloeschers (+42,6 %, 115 -> 164, D_kleinkram_a.md 2.4). Ursache: Lage
  nach maximalem Licht gewaehlt (210 von 255), der Puls (x 0xBE/0x80) laeuft in die 255-Grenze. Wortlaut "Blinkend so wie ...
  der Feuerloescher": nur teilweise.
- **M6 RE-Gate (klein):** `s_ga.t > 60` (gatorszene_r41.c:702) ohne Adresse und ohne PORT-WAHL-Kennzeichnung; Dossier-Zitat
  Clip 11 "FUN_8001a330" - tatsaechlich `jal 0x8001a240` @0x801011e0.

Hinweise (keine Maengel): H1 nach dem Tod spielt wieder MAIN2E (RE2 blendet aus, `51 00 05` @+0x762) - Nutzerfrage. H2 die
Musikweiche haengt an "Gator nicht untergetaucht"; mit Spur G (Gator nach der Kanister-Szene untergetaucht bis zum Annaehern)
wechselt die Musik hin und her - bei der Integration messen. H3 2080-Lagen von G kopiert (synchron halten). H4 Schluessel in
der Ankunftseinstellung ~10 Pixel grosser Ring aus 6-12 hellen Punkten (Groesse wie der Feuerloescher, als Schluessel kaum
erkennbar). H5 PSX-Bau ungeprueft.

## 7. Urteile

| Punkt | Urteil | Beleg |
|---|---|---|
| 1a Ankunftsszene (Leiche + blinkender Schluessel, 2080-Zombies oben + Knall, erneut Leiche, Gator springt und frisst, Schrei/Schuetteln, RE2-Kampfmusik; Leon + Elza) | teilweise | Folge und Bilder 3.1 vollstaendig; M1 (Kampfbeginn ohne Spielfigur), M5 (Puls gesaettigt) |
| 1b Schluessel nach dem Tod bei der Leiche, blinkend | nicht_erfuellt | 3.3: in beiden Todeswegen unsichtbar (bitgleiche Differenzbilder); die Aufnahme selbst geht |
| Nachtrag 21 Leiter statt Tor, Tor mit dem Schluessel | erfuellt (Ton-Mangel M3) | 3.4 G1/G2/G3/GE/GE2 |
| 2 Spinnen aus 2090 entfernen | erfuellt | §2: 0 Spinnen-Bilder in allen Laeufen, Leon + Elza |
| RE-Gate | teilweise | §4 Stichproben stimmen; M6 |
| Vertrag/Riegel | nicht erfuellt | §5, M4 |

**Ergebnis: NICHT bestanden** (M1-M6).
