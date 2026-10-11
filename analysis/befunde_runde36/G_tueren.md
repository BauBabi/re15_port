# Runde 36 — Spur G "tueren" (Dossier)

Baum: `.claude/worktrees/r36_tueren`, Zweig `r36/tueren`, Basis master 78a4a748. Stand: alle drei Punkte gebaut,
gemessen, gepinnt (Abschnitt Tests). Belegbilder: `analysis/befunde_runde36/G_belege/`.

## Auftrag (Wortlaut des Nutzers, AUFTRAG.md)

1. "Einige Türen in den Türsequenzen sind noch nicht ganz korrekt. So zum Beispiel der Elevator ROOM 1080 und
   die große Tür in ROOM 2090. Ich glaube beide liegen auch korrekt in Resident Evil 2 vor."
2. "Die Tür Textur zwischen ROOM 10C0 und 10D0 bei der Türanimation ist herausstechend billig im Vergleich zum
   Rest."
3. "Türen die erfolgreich geöffnet wurden mit Karte oder sowas machen in Resident Evil 2 erst ein "piep" Sound
   und dann ein unlock Geräusch. Bei uns nicht."

Einordnung (Memory `reai-v2-beta-zu-retail`): RE1.5 hat KEINE Tuersequenzen (DOOR00.DO2 einziges Skript
`Evt_end`, Payload+12/+13 = 0 in 649/653 Door_aot_set) und ist beim Aufschliessen stumm -> RE2 Retail ist das
Ziel, Belege aus RE2 (`info/re2leon/PSX.EXE`, `info/re2leon/PL0/RDT`, `info/re2leon/COMMON/DOOR`).

## Werkzeuge dieser Spur (neu)

| Datei | Zweck |
|---|---|
| `platform/pc/src/tuer_r36_pc.c` + 2 Zeilen `platform/pc/main.c` | Pruefhaken `RE15_TUER_ARCHIV=NN:V[:B],Kkkk:V,...` (beliebige RE2-/Port-Archive durch den Spiel-Laeufer, Seite 901.., Bogen-Bilder + `archiv_liste.txt`); Anmeldung des PC-Spielers der Aufschliess-Toene |
| `tools/tueren/r36_kartenton_analyse.py` | Zeitprofil RE2-Raumbank-Wellen (20-ms-Fenster: Pegel, Spitzenfrequenz, spektrale Flachheit) |
| `tools/tueren/r36_re2_piep_zensus.py` | alle RE2-Skriptbloecke mit Piep-Welle und ihrer Se_on/Sleep-Folge |
| `tools/tueren/r36_textur_mass.py` | Mass fein/fleck/form/hell je Tuertextur gegen die 54 RE2-Texturen |
| `tools/tueren/r36_tueren_bauen.py` | plan.json-Wahl (archiv "RE2") + gezielte Nachfuehrung `gen/re15_tuer_eigen.inc` + `--p0cd` |
| `tools/tueren/r36_rezept_p0cd.py` | neues Textur-Rezept P0CD (registriert in `tuer_rezepte.REZEPTE`) |
| `tools/re2_karten_se_cut.py` | Mini-Bank `shared_assets/RE2/KARTSE.VBS` + `gen/re2_karten_bank.inc` |
| `tools/gen_r36_aufschliessen.py` | Stellen-Tabelle `gen/r36_aufschliessen.inc` (29 Stellen) |

Messlaeufe der exe: Hintergruende `build/r36_tueren/bg/` (probe_bg_dump STAGE1+2), Bogen-Bilder
`build/r36_tueren/bogen_*`, Messkopie `re15_pc_g36mess.exe`, Ton `RE15_AUDIO_CAP_SYNC` + `RE15_AUFSCHLIESS_LOG`.

---

## G1 — Fahrstuhl ROOM1080 und grosse Tuer ROOM2090

### Messung vorher (Stand 78a4a748; `G_belege/g1_fahrstuhl_vorher_nachher.jpg`, `g1_2090_vorher_nachher.jpg`)

`RE15_TUER_SEITE=S013,S040,S057,S163,S164,S165 RE15_TUER_BOGEN=...`:

| Seite | Raum -> Ziel | vorher | Bild |
|---|---|---|---|
| S013 / S040 / S057 | 1040 / 10C0 / 1120 -> ROOM1080 (Kabine) | Port-Archiv **P25G** (Basis DOOR25 "SHAFT TYPE-L") V0 | EIN Blatt faehrt nach OBEN (Hubtuer) |
| S164 | 2090 -> 20A0 | Port-Archiv **P26W** (Basis DOOR26) **V3** | EIN schmales Schott-Blatt |
| S165 | 20A0 -> 2090 | P26W **V2** | dito |

Was RE1.5 malt (= Soll der Gestalt):
* Fahrstuhl: `ROOM10800` (Kabine von oben): **zweifluegelige, mittig oeffnende Schiebetuer** (senkrechte Mittelfuge,
  Etagenanzeige "▼①②③▲" darueber); aussen (`ROOM11200`, `ROOM10C00`) glatte graue Stahltuer in tiefer Nische.
  -> "ein Blatt faehrt hoch" ist falsch.
* 2090: Door_aot_set `ROOM2090.RDT` @0x9B6 Flaeche hw 500 / hh 1750 = **3500 breit** (Standardtuer 2000); das
  3500er-Rechteck deckt den gemalten Rahmen in Cut 1/2/3 (`build/r36_tueren/proj2090.png`), auf der 20A0-Seite
  (`ROOM20A13`) endet der Gang in einer fast quadratischen, ganz breiten Oeffnung. Gemalt: Stahlrahmen,
  gelb-schwarze Warnstreifen, rotes Handrad links. Ein Fluegel (DOOR26 V2/V3 = Mesh 0, z 0..2990 im Tuerraum
  ~1600 Raumeinheiten) ist halb so breit.

### RE2-Belege (Walker `analysis/befunde_runde30/tools/r30_re2_scd.py`, Bytes selbst gelesen)

**Fahrstuhltuer in RE2 = DOOR27** (zwei Fluegel, aus der Mitte auseinander):
* RE2 `ROOM6010.RDT` = A-2-Kabine (Hintergrund `ROOM60104` "A-2 ELEVATOR"): main00 @0x004C2 / @0x004E2 / @0x00502
  `Door_aot_set aot 0/1/2 -> ROOM6020 / 6060 / 7000`, alle **DOOR27 V0**, dasselbe Rechteck = eine Tuer mit Ziel je
  Etage; Gegenseiten ROOM6020 @0x00872, ROOM6060 @0x00E7C, ROOM6070 @0x00B5E -> 6010 **DOOR27 V0**.
* RE1.5s eigene A-2-Kabine ROOM4020 ("A-2 ELEVATOR") laeuft im Port seit Runde 31 GLEICH DOOR27 V0 (S215/S260/S290).
* Maschine DOOR27 V0 (gemessen `unit_r36_tueren_fahrstuhl`): obj 0 z-Weg 2240 Richtung -, obj 1 z-Weg 2240 Richtung +,
  Drehung 0, y-Weg 0 = Schiebetuer mittig. Gegenprobe DOOR25 V0: ein Objekt, Hub nach oben.
* Verworfen: DOOR25 (hebt), DOOR29 "SHAFT TYPE-M" (Drehtuer), DOOR2A V2/V3 (zwei Standardblaetter, Bahnsteig 7000-7040,
  keine Fahrstuhltuer), DOOR10 (Scherengitter der Irons-Kabine RE2 ROOM21B0 — passt nicht zur gemalten Stahltuer).

**Grosse Tuer in RE2 = DOOR26 V0 (zweiteilig)**:
* DOOR26 Skript 3 (Variante 0) @Datei 0x05914: `4d 00 .. mesh 0` bei z -3000 UND @0x0592A `4d 01 .. mesh 1` bei
  z +3000 = zwei Fluegel (je 2990 breit), Rad (mesh 3) am Fluegel 0, Schild (mesh 4) am Fluegel 1; Skript 1/2
  (V3/V2) nur obj 0.
* RE2 nutzt DOOR26 als Gross-Schott beidseitig **V0**: ROOM6020 @0x008AC <-> ROOM6030 @0x029CA, 6030 @0x029EA <->
  6040 @0x01FB0, 60C0 @0x00F7A <-> 60F0 @0x00716, 6140 @0x00BD2 <-> 6150 @0x02B96, 6160 @0x01D2A <-> 6170 @0x03582
  (Flaechen 3200..5900); einteilig nur 3030 @0x015B8 <-> 3040 @0x01004 (2560..3020).
* 2 x 2990 / 1,86 (Massstab Raum -> Tuerszene) = ~3220 ~ RE1.5-Flaeche 3500; Rad links = V0.

### Umsetzung

* `analysis/befunde_runde33/tueren_rest/plan.json`: T011/T017/T018 `archiv "RE2", basis "DOOR27"`, V0; T084
  `archiv "RE2", basis "DOOR26"`, S164 V0 / S165 V0 (Herkunft je Seite mit den Adressen oben).
* `tools/tueren/tuer_archiv_bauen.py zeilen_bauen`: "RE2" wie "OBJEKTLOS" (RE2-Datei, eigen 0).
* `engine/src/gen/re15_tuer_eigen.inc`: die 10 Zeilen (5 Seiten x xxx0/xxx1) gezielt nachgefuehrt
  (`tools/tueren/r36_tueren_bauen.py`; ein voller Generator-Lauf braucht die Runde-31/33-Ausschnitte des Hauptbaums):
  `0x27, 0, ..., eigen 0` bzw. `0x26, 0, ..., eigen 0`. Die RE2-Dateien DOOR27.DO2/DOOR26.DO2 liegen schon in
  `shared_assets/RE2/DOOR` (Runde 31). P25G/P26W bleiben als Dateien/Tabelleneintraege (Indizes der uebrigen Archive
  unveraendert), haben aber keine Zeile mehr.
* `tests/unit/probe_r33_tueren.c`: "zuordnung" laesst die fuenf Runde-36-RE2-Zeilen (eigen 0, nicht DOOR36) zu;
  "archive" zaehlt 43 statt 46 Varianten (P25G V0, P26W V2/V3 ohne Zeile).

### Messung nachher (`G_belege/g1_*`)

* Bogen S013/S040/S057: zwei Fluegel schieben aus der Mitte auseinander (DOOR27 V0, RE2-Ton des Archivs).
* Bogen S164/S165: Gross-Schott zweiteilig, Fluegel mit Rad geht weg, Fluegel "TYPE-P" steht (DOOR26 V0).
* Echter Spielschritt (`unit_r36_tueren_fahrstuhl` / `_schott`): ROOM10C0 Slot 1 -> ROOM1080 (mit Strom-Flag (4,0xF3),
  ROOM10C0 main00 @0x00E66 / @0x00E82) stellt GENAU eine Anfrage DOOR27 V0 eigen 0 S040; ROOM2090 Slot 1 -> ROOM20A0
  DOOR26 V0 eigen 0 S164.
* exe (`integration_r36_tueren` C): `[tuer-seite] S040 ROOM10C0 DOOR27 V0`, `[tuer] Sequenz Archiv 2 DOOR27 Variante 0
  ... Seite S040`, `S164 ROOM2090 DOOR26 V0`.

---

## G2 — Tuer-Textur ROOM10C0 <-> ROOM10D0 (T025, Port-Archiv P0CD)

### Messung vorher

Seiten S041 (10C0 -> 10D0, V1) und S044 (10D0 -> 10C0, V0) spielen P0CD (Basis DOOR0C, beide Fluegel). Rezept
Runde 33 (`tuer_rezepte.rezept_P0CD`, jetzt `rezept_P0CD_r33`): Flaechenfarbe EINE Zahl, "Maserung" = DOOR0C-Feld
68x80 Texel **bikubisch auf 128x256 hochgezogen** (`resize((128, 256), Image.BICUBIC)`), Fenster/Gitter/Stange als
gefuellte Rechtecke. Gemessen (`tools/tueren/r36_textur_mass.py`, Blatt v 0..217, L Rec.601, fein = std(L - G1)/mittel,
fleck = std(G1 - G4)/mittel, form = std(G4)/mittel; gegen die RE2-Texturen; die alte Datei liegt als
`build/r36_tueren/P0CD_alt.DO2`):

| | fein | fleck | form | hell | Faser gy/gx (C) |
|---|---|---|---|---|---|
| P0CD vorher (Python) | 0.0818 | 0.0917 | 0.1817 | 114.5 | 0.44 (senkrechte Flecken) |
| RE2-Texturen Minimum / Median | 0.0788 / 0.1682 | 0.0655 / 0.1825 | 0.0969 / 0.3955 | (RE2-Holz 25..70) | |
| RE1.5 gemalt (ROOM10C04, 5fach `G_belege/g2_*`) | | | | | waagerechte Faser |

= Feinzeichnung knapp ueber dem RE2-Minimum, Grossform halb so stark wie der RE2-Median (kein Licht, kein Relief),
ueberhell, Faser in falscher Richtung.

### Beleg der Gestalt (RE1.5-Hintergrund, Port-MDEC-Dekoder)

`ROOM10C04` (frontal), linker Fluegel x 111..164, Blatt y 69..164 (95 Zeilen -> v 0..217):
* Holz-Zeilenprofil (x 115..134, Median je 4 Zeilen): y70 (99,84,68) ... y102 (78,56,29) ... y146 (41,30,10.5) —
  Deckenlicht, unten 0,4fach.
* Glas x 142..148 y 80..119: Median (34,36,32); helle Schraegdraehte.
* Trittblech ab y 152 (87 %), Median (26,29,26).
* Lagen Fenster u 26..57 v 17..137, Stange v 147..153 u 14..115 aus Runde 33 (S041 c03, Fuge = u klein).

### Umsetzung (`tools/tueren/r36_rezept_p0cd.py`, PORT-WAHL Textur, Belege je Zahl im Kopf)

* Faser = relatives Feinkorn der drei DOOR0D-Fuellungen (RE2 helles Holz; u 27..98, v 20..58/91..129/162..200,
  dort gx > gy = senkrecht), um 90 Grad gedreht, NUR laengs gestreckt (quer 1 Texel = 1 Texel), 4 Stuecke mit
  4 Zeilen Ueberblendung (Fugen laufen mit der Faser; gemessen keine Fugenstufe), Kontrast 1,3.
* Farbe/Licht = gemaltes Zeilenprofil, Texel = gemalt * 128 / 73 (Blattflaeche c = 73, tuer_archiv_bauen C_BLATT);
  waagerechte Woelbung -12 % an den Kanten, normiert auf die Messstelle (u ~94).
* Fenster: erhabener Metallrahmen, Glas mit Spiegelverlauf, Drahtrauten 1 Texel (Abstand 6); Glas-Basis 0,88 so,
  dass der Fenster-MEDIAN gezeigt (34,37,33) = gemalt (34,36,32).
* Panikstange als Zylinder (Glanz oben, Schatten unten, Wurfschatten), Halter an beiden Enden; Trittblech ab v 190
  gebuerstet (Feinkorn DOOR07), Schrauben; dunkle Blattkanten.
* `shared_assets/RE15DOOR/P0CD.DO2` neu gebaut (Tonteil/SCD/MD1 bytegleich DOOR0C, nur TIM; 61468 B, Palette 245,
  FNV in `gen/re15_tuer_eigen.inc`); Kennung P0CD bleibt -> Spiegel-Korrektur Runde 35 (`tuer_spiegel_r35.c`) gilt weiter.

### Messung nachher (`G_belege/g2_p0cd_vorher_nachher.jpg`)

| | fein | fleck | form | hell | Faser gy/gx | Glas/Holz |
|---|---|---|---|---|---|---|
| P0CD vorher (C-Rechnung `probe_r36_tueren textur P0CD_alt.DO2`) | 0.0814 | 0.0915 | 0.1824 | 114.5 | 0.44 | 0.48 |
| P0CD nachher (C-Rechnung `unit_r36_tueren_textur`) | 0.1039 | 0.1069 | 0.3544 | 86.1 | 4.43 | 0.64 |
| P0CD nachher (Python `r36_textur_mass.py`) | 0.1081 | 0.1037 | 0.3569 | 86.1 | | |

Grossform jetzt nahe am RE2-Median (0.3569 gegen 0.3955), Feinzeichnung +32 % (unter dem RE2-Median 0.1682 — die
gemalte RE1.5-Tuer ist selbst glatt lackiertes Holz), Helligkeit im Bereich der uebrigen Holztueren.

Gezeigt (Texel x 73/128, Median u 85..103, je 5 Zeilen) gegen gemalt (ROOM10C04, linker Fluegel):

| Bildzeile | v | gezeigt | gemalt |
|---|---|---|---|
| y70 | 6 | (99,84,66) | (99,84,68) |
| y86 | 42 | (94,70,47) | (89,67,43) |
| y102 | 79 | (80,56,33) | (78,56,29) |
| y118 | 116 | (66,47,23) | (69,49,24) |
| y138 | 162 | (51,33,14) | (45,31,11) |

Groesste Abweichung 6 je Kanal. Sequenz S041/S044 im Laeufer (`G_belege/g2_p0cd_vorher_nachher.jpg`, untere zwei
Reihen): Holz mit waagerechter Faser und Licht von oben, Drahtglas mit Rahmen, Panikstangen mit Haltern, Trittblech.

---

## G3 — Karten-Tueren: erst Piep, dann Aufschliessen

### Messung vorher

* RE1.5-Original stumm: kein Se_on in einem der Aufschliess-Bloecke (Zensus `tools/gen_r36_aufschliessen.py`,
  Spalte Bedingung C), z.B. ROOM10D0 sub19 @Datei 0x01988 Aot_reset Platz 2 sce 2 + @0x0199E Message_on 5
  "You've opened the lock."; Runde 30 (`analysis/befunde_runde30/tuer-verschlossen.md` 3.1): einziges Schloss-Se_on
  des Spiels ist ROOM4000 sub02 @0x0142E (Piep VOR der Kartenabfrage, bleibt RE1.5).
* Port vorher: dieselben Wege ohne Ton (Ton-Spione der Runde 30 zaehlten nichts; `RE15_SE_DEBUG` ohne Zeile).

### RE2-Belege

Kartenleser-Tuer `ROOM2110.RDT` (RE2 Leon A, Waffenkammer):
```
sub06 @0x01AE6 Set(4,0x0e)=1  @0x01AEA Sce_Item_lost 0x34  @0x01AEC Evt_exec -> sub08
sub08 @0x01B2C Cut_chg 12   @0x01B2E Aot_reset 46 08 01 31 ..   (Platz 8 wird Tuer = aufgeschlossen)
      @0x01B4C 36 02 0c 00 ..  Se_on(2,0x0C)
      @0x01B58 09 0a 0f 00     Sleep 15
      @0x01B5C 36 02 0d 00 ..  Se_on(2,0x0D)
      @0x01B68..7C             Sce_espr_kill2/on2 (Lampe am Lesegeraet wechselt)
      @0x01B8C 09 0a 0f 00     Sleep 15
      @0x01B90 36 02 0b 00 ..  Se_on(2,0x0B)
```
Se_on 0x36 @0x80056428 -> `jal 0x8005ba28` @0x80056530, a0 = Bank<<24 | Satz<<16 (@0x80056518..24); Bank 2 =
Raumbank (FUN_80059e54 @0x80059f44 0x800dbb80 = [RDT+8]). Wellen (`tools/tueren/r36_kartenton_analyse.py`):

| Satz | EDT @Datei | Welle | Profil | Rolle |
|---|---|---|---|---|
| 0x0C | 0x02ED4 `00 00 64 16` | 9064a95e, VAG @0x064B4 2976 B, 16397 Hz 0,32 s | breitbandig, 65 % 2..4 kHz | Karte ein (Klack) |
| 0x0D | 0x02ED8 `00 00 43 01` | b03b6dff, VAG @0x09874 4640 B, 0,49 s | **stehender Ton 1450 Hz**, Flachheit 0,005 | **PIEP** |
| 0x0B | 0x02ED0 `00 00 53 00` | da805fa3, VAG @0x0AA94 11264 B, 1,20 s | tief, 87 % unter 250 Hz | **Aufschliessen** |

Gegenproben: dieselbe Piep-Welle b03b6dff in RE2 ROOM2130 Satz 0x0F (Nutzerliste "ROOM213 Electronic lock sounds",
sub04 @0x01798) und ROOM4070 Satz 0x0C (sub08 @0x00DD4). EXE-Kartenschloss (Lab Card Key 0x61, ROOM60C0 @0x00F52,
ROOM6140 @0x00BB2): Tuer-Handler @0x80051654/@0x80051658 `lui a0,0x225 / jal 0x8005ba28` = Satz 0x25 = Welle
45d7a753: 0..0,30 s Klack (Spektrum wie 2110 0x0C, L1-Abstand 0,085) + 0,42..1,06 s Ton 1450 Hz. Mechanische
Schluessel (Satz 0x25 in 1050/1140/2040/6030/6060/7000) ohne Ton-Anteil -> **Piep nur bei Karten**. Riegel von dieser
Seite (key_type 0xFE): @0x800515ec `jal 0x8002fe38` (Text "You unlocked it.") + @0x800515f4/@0x800515f8 `lui a0,0x226
/ jal 0x8005ba28` = Satz 0x26 (Welle d1d99d65, ROOM20A0 VAG @0x07D8C) im selben Bild.

### Umsetzung

* Mini-Bank `shared_assets/RE2/KARTSE.VBS` (`tools/re2_karten_se_cut.py`, Format TUERSE/ELEVSE, Wellen
  bytegleich + sha1-geprueft): 0 KARTE_EIN (2110 0x0C), 1 PIEP (2110 0x0D), 2 AUF_E (2110 0x0B), 3 AUF_HIER
  (20A0 0x26); TOC `gen/re2_karten_bank.inc`.
* Stellen `gen/r36_aufschliessen.inc` (`tools/gen_r36_aufschliessen.py`, Wortlaut = PORT-WAHL, Folge = RE2):
  * KARTE_EIN (6): "You've used the Blue/Yellow/Red Keycard." in 10D0/10D1/11E0/11E1/1230/1231 (Raeume mit Ziffernfeld)
    -> Karte ein (0).
  * KARTE (3): "You've used the Green Keycard." 3010/3011, "You've used the Blue Master Keycard." 4000 -> Klack 0 /
    Piep 15 / Aufschliessen 30 (= 2110 sub08).
  * KARTE_AUF (8): "You've opened the lock." in den Ziffernfeld-Raeumen und am Minidisc-Schloss 1100/1101 -> Piep 0 /
    Aufschliessen 15.
  * RIEGEL (12): "You've opened the lock." 1140 (msg 0), 1141 (msg 12), 2020/2021, 2070/2071, 20A0/20A1, 3040/3041,
    6010/6011 -> Satz 0x26 (0).
  * verworfen: 30C0/30C1 msg 0 und 4001 msg 2 (kein Message_on im Raum).
* `engine/src/aufschliessen_r36.c` + `include/re15_aufschliessen_r36.h`: Folge in Spielbildern (Sleep 15 = 15 Bilder,
  `RE2_KARTE_SLEEP`; das Nachrichtenbild ist Bild 0 — gemessen vorher +14/+29), Spieler per Anmeldung
  (PC: `re15_audio_re2_karte_se`, PSX/Tests: Spion bzw. stumm), Log `RE15_AUFSCHLIESS_LOG`.
* Haken: `scd_vm.c` 2 Zeilen neben `re15_lock_se_notice` (Text-AOT + Message_on, hinter der Stimmen-Warte-Schranke),
  `game_step_common.c` 1 Zeile Tick (vor dem Item-Modal-Return, laeuft auch bei offener Nachricht),
  `audio_pc.c` Slot KARTSE (Kopie Slot 5c), `main.c` 1 Zeile Anmeldung.

### Messung nachher (echte exe, `RE15_AUFSCHLIESS_LOG` + `RE15_SE_DEBUG` + `RE15_AUDIO_CAP_SYNC`)

Laeufe `build/r36_tueren/mess_*` (Messkopie `re15_pc_g36mess.exe`, `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AUDIO_CAP_SYNC=cap.raw RE15_AUFSCHLIESS_LOG=auf.log`):
```
ROOM3010 (RE15_SET_FLAG=3:59 RE15_DEBUG_JUMP=3010@120 RE15_FIRE_AOT=6@100#3010 -> sub02 Message_on 1, RE15_SE_DEBUG=1):
F101 raum=3010 nachricht=1 art=KARTE satz=0(KARTE_EIN) bild=+0 nr=1
F116 raum=3010 nachricht=1 art=KARTE satz=1(PIEP) bild=+15 nr=2
F131 raum=3010 nachricht=1 art=KARTE satz=2(AUF_E) bild=+30 nr=3
[se] Stimme: se=0 layer=0 vag=0 note=65 fine=57 center=83 vol=80 pitch=0x5f3 (16397 Hz) -> SE-Stimme 6 (SPU 22)
[se] Stimme: se=1 layer=0 vag=1 note=63 fine=57 center=81 vol=80 pitch=0x5f3 (16397 Hz) -> SE-Stimme -15 (SPU 1)
[se] Stimme: se=2 layer=0 vag=2 note=64 fine=57 center=82 vol=115 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16 (SPU 0)
ROOM10D0 (RE15_DEBUG_JUMP=10D0@120, Ziffern-Bits RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@200 -> sub19 Message_on 5):
F201 raum=10D0 nachricht=5 art=KARTE_AUF satz=1(PIEP) bild=+0 nr=1
F216 raum=10D0 nachricht=5 art=KARTE_AUF satz=2(AUF_E) bild=+15 nr=2
ROOM1140 (RE15_DEBUG_JUMP=1140@120 RE15_FIRE_AOT=2@100#1140 -> sub02 @0x00C94 Message_on 0, RE15_SE_DEBUG=1):
F101 raum=1140 nachricht=0 art=RIEGEL satz=3(AUF_HIER) bild=+0 nr=1
[se] Stimme: se=3 layer=0 vag=3 note=65 fine=57 center=83 vol=127 pitch=0x5f3 (16397 Hz) -> SE-Stimme 6 (SPU 22)
```
"SE-Stimme -15/-16" = Satz mit Stimmen-Byte < 0x10 = Direkt-Zweig (audio_pc.c se_play_layers, FUN_80045024) auf
einem freien Mischer-Platz; so stehen die Saetze in der RE2-Raumbank (Bytes kopiert), keine BGM-Stimme wird belegt.

Mitschnitt (`tools/tueren/r36_cap_ticks.py`, 1470 Stereo-Frames je Audio-Tick, Tick = Raumbild F + 124 bzw. + 123):

| Lauf | Klack | Piep (1450 Hz) | Aufschliessen (< 250 Hz) |
|---|---|---|---|
| ROOM3010 | Tick 225 (= F101): Pegel 785 -> 1287, 2..4 kHz 0,24 | Ticks 240..254 (= F116), Anteil 0,31..0,82, 15 Ticks = 0,5 s | ab Tick 255 (= F131) Anteil 0,87..0,97, Spitze 2313 bei Tick 260, abklingend bis ~285 |
| ROOM10D0 | — | Ticks 324..338 (= F201), Anteil 0,24..0,65 | ab Tick 339 (= F216), Spitze 2987 bei Tick 345 |
| ROOM1140 | — | — | Riegel Ticks 225..248 (= F101; Welle 13244 Abtastwerte / 16397 Hz = 0,81 s = 24 Ticks) |

Gegenlauf ROOM1140 OHNE Nachricht (`mess_ohne1140`, gleiche Umgebung ohne RE15_FIRE_AOT): beide Mitschnitte bitgleich
bis Tick 224, **erster abweichender Tick 225 = F101** (der Riegel-Ton). Ab Tick 234 fehlt im Nachrichtenlauf der
wiederkehrende Raum-Ton (Raumbank se=3, pitch 0x51c: 17 Anschlaege ohne Nachricht, 7 mit — keiner nach F100): die
offene Nachricht haelt dessen Quelle an, das ist kein Effekt des Riegel-Tons (dessen Mischer-Platz endet mit der Welle
bei Tick 248; ab Tick 249 ist der Mitschnitt still).

---

## Tests

Neue Riegel (`tests/unit/probes/r36_tueren.cmake`, `tests/unit/probe_r36_tueren.c`,
`tests/integration/test_r36_tueren.cmake`):

| Test | misst |
|---|---|
| `unit_r36_tueren_fahrstuhl` | Zeilen S013/S040/S057 = DOOR27 V0 RE2-Datei; Maschine DOOR27 V0 zwei Fluegel gegenlaeufig in z (2240), keine Drehung/kein Hub; Gegenprobe DOOR25 V0 = ein Blatt hebt; echter Durchgang ROOM10C0 -> ROOM1080 -> Anfrage DOOR27 V0 |
| `unit_r36_tueren_schott` | Zeilen S164/S165 = DOOR26 V0; Maschine V0 zwei Fluegel, V3 einer; echter Durchgang ROOM2090 -> ROOM20A0 -> Anfrage DOOR26 V0 |
| `unit_r36_tueren_textur` | P0CD: fein >= 0.095, form >= 0.30, hell <= 100, Faser gy > 2 gx, Glas < 0,8 x Holz, gezeigt = gemalt (+-10 je Kanal) an 5 Profilzeilen; dieselbe Rechnung auf die alte Datei (`probe_r36_tueren textur P0CD_alt.DO2`) ist ROT (fein 0.0814, form 0.1824) |
| `unit_r36_tueren_aufschliessen` | RE2-Bytes ROOM2110 @0x01B4C/0x01B58/0x01B5C/0x01B8C/0x01B90, RE2 PSX.EXE @0x800515f4 `3c040226` / @0x800515f8 `0c016e8a`; KARTSE-Wellen = RE2-Bytes; Zensus 6/3/8/12; Folgen; echter Skriptweg ROOM10D0 still ohne Code / Code -> Piep +0 Aufschliessen +15, ROOM10D0 Kartenleser Ja -> Karte ein, ROOM3010 -> +0/+15/+30, ROOM1140 -> Riegel +0 |
| `integration_r36_tueren` | echte exe: ROOM3010 drei Toene mit 16397 Hz (KARTSE geladen), ROOM10D0 Piep/+15, Sequenzen S040 DOOR27 / S164 DOOR26 / S041 P0CD |

Angepasst: `unit_r33_tueren_zuordnung` (Runde-36-RE2-Zeilen zulaessig), `unit_r33_tueren_archive` (43 Varianten).

Gezielte Laeufe (KEINE volle Suite), Regex = eigene Tests + Testgruppen der beruehrten Module (Tueren, Fahrstuhl,
Schloss-/Nachrichtenwege, SCD-AOT, Kopflauf, SE-/Audio-Kette wegen des neuen audio_pc.c-Slots):
```
ctest --test-dir "<baum>/re15_port/build" --timeout 600 --output-on-failure -j 3 -R "r36_tueren|r33_tuer|unit_r31_|r32_unterteilung|unit_door_seq|r35_inhalt_tueren|fahrstuhl|elevator_1080|unit_elev_se|r30_tuer|door_|keydoor|unit_msg_dialog|unit_action_msg_gate|unit_scd_key_ck|unit_aot_|unit_scd_aot_opcodes|unit_tor_1170|r35_cut10f0_tuerton|r30_elza_selbsttuer|r35_zgirl_tuer|r35_zgirl_selbsttueren|integration_headless_run|unit_r32_tor_hell|unit_audio_|unit_se_|unit_r34_plattform_ton"
```
Ergebnis (Stand 2026-10-11, Baum auf d03b8dd2 + Dossier/Werkzeug, Protokoll `build/r36_tueren/ctest_gezielt_final.log`):
```
100% tests passed, 0 tests failed out of 59
```
Darin u. a. `integration_r36_tueren` (34 s), `integration_r35_karte_fahrstuhl` (185 s), `integration_keydoor`,
`integration_door_lock`, `integration_door_traversal`, `unit_r33_tueren_*`, `unit_door_seq`, `unit_r30_tuerse_bank`.

---

## OFFEN

* Fahrstuhl-AUSSTIEG: ROOM1080/4020 verlassen die Kabine per Skript (Null-Rechteck-Tueren ROOM1080.RDT @0x00482/
  @0x004A2/@0x004C2) und haben wie seit Runde 31 keine Sequenz. RE2 spielt bei Skript-Tueren mit Archiv eine Sequenz
  (ROOM21B0 sub03 @0x026EE `Aot_on 2` -> Door_aot_set @0x024B6 DOOR10 V0; ROOM6070 @0x00B5E -> 6010 DOOR27 V0) — eine
  Ausstiegssequenz waere RE2-belegt, ist aber nicht beauftragt (der Nutzer nannte die Tuersequenz des Einstiegs).
* Wortlaut-Zuordnung der Aufschliess-Stellen ist PORT-WAHL (RE1.5 hat kein key_type-Feld); die Folge je Art ist RE2.
  Zensus aller RE1.5-Nachrichten mit "used the" / "unlock" / "opened the" / "the lock" (alle STAGE*-RDTs):
  aufgenommen sind nur Tuer-Stellen. Nicht aufgenommen, weil keine Tuer: "You've used the Red Master Keycard."
  ROOM3050/3051 msg 5 (Computer, danach msg 6 "The system has been successfully restored."), Fire Extinguisher 1090,
  Pliers 11E0/11E1, Fuse 2060/2061. Mechanischer Schluessel: "You've used the Latch Key." ROOM3091 msg 32 = sub07
  @0x016CA Message_on 0x20, danach Zwischensequenz mit eigenen Se_on (@0x01A62/@0x01A72/@0x01A82/@0x01A92) -> nicht
  angefasst (RE2 spielt beim mechanischen Schluessel Satz 0x25 OHNE Piep, @0x80051654/@0x80051658; der Nutzer
  verlangt den Piep fuer Karten). ROOM1211 (Gefaengnisschluessel, Spur L) und die Parkplatz-Tuer (Spur Q) gehoeren
  anderen Spuren.
* Tuer-Optik: DOOR27 (RE2-Labor-Fahrstuhl, oliv-grau mit Bedienfeld) und DOOR26 (RE2-Schott rostrot "TYPE-P") laufen
  als unveraenderte RE2-Dateien, weil der Nutzer beide Tueren ausdruecklich "korrekt in Resident Evil 2" verortet hat.
  Die gemalten RE1.5-Tueren sind anders gefaerbt (Fahrstuhl glatt grau, 2090 dunkel mit Warnstreifen und rotem Rad,
  `G_belege/g1_*`). Die Runde-33-Archive P25G/P26W hatten diese Farben, aber die falsche Mechanik (Hub bzw. ein
  Fluegel; P26W faerbt nur Mesh 0 + Rad um, Mesh 1 des zweiten Fluegels waere rostrot geblieben). Eine Umfaerbung beider
  Fluegel waere ein neues Port-Archiv (Rezept-Weg wie P26W) — nicht gebaut.

## Fuer den Nutzer

* **Neue/geaenderte Assets (Paket-Gate):** NEU `re15_port/shared_assets/RE2/KARTSE.VBS` (29592 B, sha1 b4dcdc24...);
  GEAENDERT `re15_port/shared_assets/RE15DOOR/P0CD.DO2` (gleiche Groesse, FNV in `gen/re15_tuer_eigen.inc`). Beide
  werden von `make_package.sh` (`cp -r RE2`, `cp -r RE15DOOR`) und `build.gradle` (ganze Ordner) mitgenommen; ein
  eigener Gate-Eintrag fuer KARTSE.VBS fehlt noch (release/ ist Orchestrator-Sache). DOOR27.DO2/DOOR26.DO2 liegen
  seit Runde 31 im Paket.
* **Sprachdateien:** keine.
* **Was man sieht/hoert:** Fahrstuhl (1F/2F/3F -> Kabine): die RE2-Fahrstuhltuer der A-2-Kabine (zwei Fluegel schieben
  auseinander, Labor-Optik mit Bedienfeld — RE2 hat keinen Revier-Fahrstuhl dieser Art). Tuer 2090 <-> 20A0: das
  grosse zweiteilige RE2-Schott (rostrot "TYPE-P"). Tuer 10C0 <-> 10D0: helles Holz mit Faser, Drahtglas, Panikstange,
  Trittblech. Karten-/Ziffernfeld-Tueren: beim Erfolg Piep, eine halbe Sekunde spaeter das Aufschliessen (Karte
  einstecken klackt schon bei "You've used the ... Keycard."); Riegel von dieser Seite ("You've opened the lock."):
  das RE2-Entriegeln.
* **Bedienhinweise (Messhaken):** `RE15_TUER_ARCHIV="27:0,26:0,P0CD:1"` + `RE15_TUER_BOGEN=<dir>` spielt beliebige
  Tuerarchive; `RE15_AUFSCHLIESS_LOG=<datei>` protokolliert jeden Aufschliess-Ton mit Spielbild.

## Endstand

* G1 Fahrstuhl (S013/S040/S057 -> DOOR27 V0) und grosse Tuer 2090 (S164/S165 -> DOOR26 V0): gebaut, gemessen, gepinnt.
* G2 Tuer 10C0 <-> 10D0 (P0CD): neue Textur, gemessen gegen RE2-Texturen und gegen die gemalte Tuer, gepinnt.
* G3 Aufschliessen: Karte ein / Piep / Aufschliessen nach RE2 ROOM2110 sub08, Riegel nach EXE @0x800515f8; 29
  Stellen; in der exe gehoert (Mitschnitt) und gepinnt.
* Gezielte Tests: `100% tests passed, 0 tests failed out of 59` (Regex oben).
