# Runde 36 — Spur G "tueren" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_tueren`, Zweig `r36/tueren`, Basis master 78a4a748.

## Auftrag (Wortlaut des Nutzers, AUFTRAG.md)

1. "Einige Türen in den Türsequenzen sind noch nicht ganz korrekt. So zum Beispiel der Elevator ROOM 1080 und
   die große Tür in ROOM 2090. Ich glaube beide liegen auch korrekt in Resident Evil 2 vor."
2. "Die Tür Textur zwischen ROOM 10C0 und 10D0 bei der Türanimation ist herausstechend billig im Vergleich zum
   Rest."
3. "Türen die erfolgreich geöffnet wurden mit Karte oder sowas machen in Resident Evil 2 erst ein "piep" Sound
   und dann ein unlock Geräusch. Bei uns nicht."

## Stand / Fortschritt

- [x] Pflichtlektuere (AUFTRAG/VERTRAG, Memories r31/r33/Tuermodelle/Tor-1170, r30 tuer-verschlossen.md)
- [x] G1 Messung vorher (Fahrstuhl + 2090), RE2-Belege
- [ ] G1 Umsetzung + Messung nachher
- [x] G2 Messung vorher
- [ ] G2 Umsetzung + Messung nachher
- [x] G3 RE2-Handler/Skripte + Wellen gemessen
- [ ] G3 Umsetzung + Messung nachher
- [ ] Tests (probes/r36_tueren.cmake)

## Werkzeuge dieser Spur (neu)

| Datei | Zweck |
|---|---|
| `re15_port/platform/pc/src/tuer_r36_pc.c` + 2 Zeilen `platform/pc/main.c` | Pruefhaken `RE15_TUER_ARCHIV=NN:V[,...]` / `Kkkk:V`: spielt BELIEBIGE RE2-/Port-Archive durch den Spiel-Laeufer (Seite 901, 902, ...; mit `RE15_TUER_BOGEN` Anfang/Mitte-Bilder + `archiv_liste.txt`) |
| `re15_port/tools/tueren/r36_kartenton_analyse.py` | Zeitprofil (20-ms-Fenster: Pegel, Spitzenfrequenz, spektrale Flachheit) der RE2-Raumbank-Wellen; `--wav <dir>` schreibt sie zum Anhoeren |
| `re15_port/tools/tueren/r36_re2_piep_zensus.py` | alle RE2-Skriptbloecke, deren Se_on eine TON-Welle (Piep) spielt, mit der Folge Se_on/Sleep/Message/Aot_reset |

Messbilder liegen unter `build/r36_tueren/` (unversioniert): Hintergruende `bg/` (probe_bg_dump, STAGE1+2),
Kontaktboegen `bogen_vorher_*.png`, `kand_*.png`, Seiten-Ausschnitte `build/r31_tueren/t1/re15_seiten/` (Runde-31-
Werkzeug `zensus_ausschnitte.py`, hier fuer 104/10C/112/10D/209/20A neu erzeugt).

---

## G1 — Fahrstuhl ROOM1080 und grosse Tuer ROOM2090

### Messung vorher (Stand 78a4a748)

`RE15_TUER_SEITE=S013,S040,S057,S163,S164,S165 RE15_TUER_BOGEN=...` (Kontaktbogen `bogen_vorher_aufzug.png`,
`bogen_vorher_2090.png`):

| Seite | Raum -> Ziel | heute | was man sieht |
|---|---|---|---|
| S013 / S040 / S057 | 1040 / 10C0 / 1120 -> ROOM1080 (Kabine) | Port-Archiv **P25G** (Basis DOOR25 = RE2 "SHAFT TYPE-L") V0 | EIN dunkles Blatt, das nach OBEN faehrt (Hubtuer) |
| S164 | 2090 -> 20A0 | Port-Archiv **P26W** (Basis DOOR26) **V3** | EIN schmales Schott-Blatt (einteilig), dunkelgruen umgefaerbt |
| S165 | 20A0 -> 2090 | P26W **V2** | dito von der anderen Seite |
| S163 | 2090 -> 2080 | DOOR16 V4 (Leiter) | unveraendert, nicht beanstandet |

Die Kabinen-Ausgaenge ROOM1080 Slot 0-2 sind Null-Rechteck-Skriptuebergaenge (`ROOM1080.RDT` main00 @0x00482/
@0x004A2/@0x004C2 `3b 0? 02 31 00 00 00 00 00 00 00 00 ..`) und haben keine Sequenz (Runde 31, tueren_02_re2.md 6.3).

### Was RE1.5 malt (Hintergrund = Soll der Gestalt)

* **Fahrstuhl:** `ROOM10800` (Kabine von oben, `aufzug_innen_zoom.png`): die Kabinentuer ist eine **ZWEIFLUEGELIGE,
  MITTIG OEFFNENDE Schiebetuer** aus gebuerstetem Stahl (senkrechte Mittelfuge), darueber die Etagenanzeige
  "▼①②③▲". Von aussen (`ROOM11200`, `ROOM10C00`, `aufzug_aussen_zoom.png`): glatte graue Stahltuer in tiefer
  Nische, Ruftaster rechts. -> Bewegung "ein Blatt faehrt hoch" (DOOR25) ist FALSCH.
* **2090:** Die Tuer 2090 <-> 20A0 (T084) ist **breit**: Door_aot_set ROOM2090.RDT @0x9B6 Flaeche hw 500 / hh 1750
  = 3500 Einheiten breit (Standardtuer 2000; `proj2090.png`: das 3500er Rechteck deckt den gemalten Rahmen), auf der
  20A0-Seite (`ROOM20A13`) endet der Gang in einer ueber ganze Breite schwarzen, fast quadratischen Oeffnung.
  Gemalt: Stahlrahmen, gelb-schwarze Warnstreifen, rotes Handrad links (`bg_2090_tuer_zoom.png`). Ein Blatt
  (DOOR26 V2/V3 = Mesh 0, z 0..2990 im Tuerraum = ~1600 Raumeinheiten) ist halb so breit wie die gemalte Tuer.

### RE2-Belege (selbst gelesen, Walker `analysis/befunde_runde30/tools/r30_re2_scd.py`)

**Fahrstuhltuer in RE2 = DOOR27** (zweiteilig, mittig auseinander schiebend):
* RE2 `ROOM6010.RDT` = "A-2 ELEVATOR" (Kabine; Hintergrund `ROOM60104` "A-2 ELEVATOR"): main00 @0x004C2 / @0x004E2 /
  @0x00502 `Door_aot_set aot 0/1/2 -> ROOM6020 / ROOM6060 / ROOM7000`, alle **DOOR27 V0**, dasselbe Rechteck
  (-20870,-25250,1800,3900) = EINE Tuer, Ziel je Etage. Gegenseiten ROOM6020 @0x00872, ROOM6060 @0x00E7C -> 6010
  **DOOR27 V0**; ROOM6070 @0x00B5E -> 6010 DOOR27 V0 (Null-Rechteck, Skriptuebergang).
* RE1.5s eigene A-2-Kabine ROOM4020 (Debug-Name "A-2 ELEVATOR") ist im Port seit Runde 31 GLEICH DOOR27 V0
  (S215/S260/S290, `gen/tuer_zuordnung.inc`; Kontaktbogen `bogen_door27.png`).
* Bewegung DOOR27 V0 `[Laeufer-Bild]` (`kand_schiebe.png` S901): zwei Fluegel fahren aus der Mitte nach links und
  rechts auseinander = die RE1.5-Kabinentuer (`ROOM10800`). Ton: eigener Tonteil (VAG 2 be1141 2,39 s, Se_on Bild
  70; Door_exit VAG 3 805d2a 2,57 s; tueren_02_re2.md Abschnitt 4).
* Gegenkandidaten gemessen: DOOR25 (heute, faehrt hoch), DOOR29 "SHAFT TYPE-M" (Drehtuer, S908), DOOR2A V2/V3
  (zwei Standardblaetter, mittig, Bahnsteig 7000/7030/7040 — keine Fahrstuhltuer in RE2), DOOR10 (Scherengitter
  der Irons-Kabine RE2 ROOM21B0, passt nicht zur gemalten Stahltuer).

**Grosse Tuer in RE2 = DOOR26 V0 (zweiteilig)**:
* DOOR26 Skript 3 (Variante 0, `tuerkatalog.py --dis 26`, Datei @0x05914): `Door_model_set obj 0 mesh 0` bei
  z -3000 UND `obj 1 mesh 1` bei z +3000 = **zwei Fluegel** (je 2990 breit), Rad (mesh 3) am Fluegel 0, Schild
  (mesh 4) am Fluegel 1. V2/V3 (Skript 2/1) stellen nur obj 0 (einteilig) auf.
* RE2 benutzt DOOR26 als Gross-Schott beidseitig mit **V0**: ROOM6020 @0x008AC <-> ROOM6030 @0x029CA, ROOM6030
  @0x029EA <-> ROOM6040 @0x01FB0, ROOM60C0 @0x00F7A <-> ROOM60F0 @0x00716, ROOM6140 @0x00BD2 <-> ROOM6150 @0x02B96,
  ROOM6160 @0x01D2A <-> ROOM6170 @0x03582 — Flaechen 3200..5900 breit. Einteilig (V2/V3) nur ROOM3030 @0x015B8 <->
  ROOM3040 @0x01004 (2560..3020).
* Breite: 2 x 2990 Tuerraum-Einheiten / 1,86 (Massstab Raum -> Tuerszene, analysis/tor_1170) = ~3220 ~ RE1.5-Flaeche
  3500. Rad links = V0 ("zweiteilig, linker Teil geht weg, Rad links", tueren_02_re2.md 1.1) = gemaltes Rad links.

### Texturen (Messung `texturen.png`)

P26W hat nur die Haelfte des Fluegels 0 (u 0..63) umgefaerbt — die rechte TIM-Haelfte (Fluegel 1 mit "TYPE-P") ist
das RE2-Original. Mit V0 (zwei Fluegel) zeigte P26W einen gruenen und einen rotbraunen Fluegel. P25G (DOOR25 ohne
Schild) traegt die Bewegung "hoch".

(Entscheidung + Umsetzung folgen.)

---

## G2 — Tuer-Textur ROOM10C0 <-> ROOM10D0 (T025)

### Messung vorher

Seiten S041 (10C0 -> 10D0, V1) und S044 (10D0 -> 10C0, V0) spielen das Port-Archiv **P0CD** (Basis DOOR0C, beide
Fluegel; `bogen_vorher_10c0.png`). Die Textur (`door0d_zoom.png` rechts) entsteht in
`tools/tueren/tuer_rezepte.py rezept_P0CD` (Runde 33) aus: Flaechenfarbe aus `ROOM10C03.bmp`, "Maserung" = DOOR0C
unteres Feld (68x80 Texel) BIKUBISCH auf 128x256 hochgezogen (`resize((128, 256), Image.BICUBIC)`), mit 0,6
aufgetragen, Fenster/Gitter/Stange als gefuellte Rechtecke. Ergebnis: verschwommene Flecken ohne Faser, flaches Gelb,
Karo-Gitter, flache Stange — gegen jede RE2-Tuertextur (scharfe Faser 1:1, Kantenschatten, Rahmenrelief, Glanz).

(Messgroesse fuer "billig" + Umsetzung folgen.)

---

## G3 — Karten-Tueren: erst Piep, dann Aufschliessen

### RE1.5 (Original) ist stumm

* Kartenleser-/Pincode-Tueren in RE1.5 (Texte, `analysis/story_leon_elza/texte_leon.md`): ROOM10D0/10D1 Funkraum
  (Blue Keycard + 4 Ziffern), ROOM11E0/11E1 Gefaengnis (Yellow Keycard + 4 Ziffern), ROOM1230/1231 Waffenkammer
  (Red Keycard + 4 Ziffern), ROOM3010/3011 (Green Keycard), ROOM4000/4001 (Blue Master Keycard), ROOM40A0
  (Research Center ID Card).
* ROOM10D0 (SCD `analysis/story_leon_elza/scd_annot/ROOM10D0.txt`): sub20 @0x019AE msg 7 "...Will you operate the
  card reader?" -> Ck(9,0x34) -> @0x019C4 msg 9 "You've used the Blue Keycard." -> Evt_exec sub17 (Ziffernfeld,
  Cut 11); die vier richtigen Ziffern setzen (5,0x0D..0x10); sub01 @0x01526 Evt_exec sub19 + Set(3,0x32);
  **sub19 @0x01988 Aot_reset Platz 2 sce 2 (Tuer scharf) + @0x0199E msg 5 "You've opened the lock."** — in keinem
  dieser Bloecke ein Se_on. Runde 30 (`analysis/befunde_runde30/tuer-verschlossen.md` §3.1) hat das fuer alle RE1.5-
  Schlossstellen gezaehlt: einziges Schloss-Se_on ist ROOM4000 sub02 @Datei 0x0142E (Piep VOR der Kartenabfrage).

### RE2: die Kartenleser-Tuer ROOM2110 (einzige in Leon A)

`ROOM2110.RDT` (RE2 Leon), selbst gelaufen:
```
sub06 @0x01AC2  Ck(4,0x0e)==0 / Cmp(0,0,9) / Cmp(0,2,0x34) / Ck(4,0x3c)==1 (Strom an)
      @0x01AE6  Set(4,0x0e)=1   @0x01AEA Sce_Item_lost 0x34   @0x01AEC Evt_exec -> sub08
sub08 @0x01B2C  Cut_chg 12 (Kamera auf das Lesegeraet)
      @0x01B2E  Aot_reset 46 08 01 31 ...        ; Platz 8 wird TUER (sce 1) = aufgeschlossen
      @0x01B4C  Se_on 36 02 0c 00 ...            ; Satz 0x0C
      @0x01B58  Sleep 15 (0a 0f 00)
      @0x01B5C  Se_on 36 02 0d 00 ...            ; Satz 0x0D
      @0x01B68..7C Sce_espr_kill2/on2 x2         ; Lampe am Lesegeraet wechselt
      @0x01B8C  Sleep 15
      @0x01B90  Se_on 36 02 0b 00 ...            ; Satz 0x0B
      @0x01B9C  Cut_chg 9 / Cut_auto
```
Se_on 0x36 @0x80056428 -> `jal 0x8005ba28` @0x80056530 mit a0 = Bank<<24 | Satz<<16 (@0x80056518..24);
Bank 2 = Raumbank (FUN_80059e54 @0x80059f44: 0x800dbb80 = [RDT+8]).

Die drei Wellen (Raumbank ROOM2110: EDT @0x2EA4, VH @0x2F64, VB @0x3B84; `r36_kartenton_analyse.py`):

| Satz | EDT | Welle (sha1) | Rate / Dauer | Profil | Bedeutung |
|---|---|---|---|---|---|
| 0x0C | `00 00 64 16` | 9064a95e, VAG @0x064B4 2976 B | 16397 Hz / 0,32 s | breitbandig, 65 % Energie 2..4 kHz | Karte einziehen (Klack) |
| 0x0D | `00 00 43 01` | b03b6dff, VAG @0x09874 4640 B | 16397 Hz / 0,49 s | **stehender Ton 1450 Hz**, Flachheit 0,005 | **PIEP** |
| 0x0B | `00 00 53 00` | da805fa3, VAG @0x0AA94 11264 B | 16397 Hz / 1,20 s | breitbandig tief (87 % unter 250 Hz) | **Aufschliessen** (Riegel/Motor) |

Gegenprobe: dieselbe Piep-Welle b03b6dff liegt in RE2 ROOM2130 Satz 0x0F (Nutzerliste "ROOM213 Electronic lock
sounds", sub04 @0x01798) und ROOM4070 Satz 0x0C (sub08 @0x00DD4, direkt danach Aot_reset 3/7).
RE2 EXE-Weg fuer Kartenschloesser (Lab Card Key 0x61, ROOM60C0 @0x00F52 / ROOM6140 @0x00BB2): Tuer-Handler
@0x80051654/58 `lui a0,0x225 / jal 0x8005ba28` = Raumbank-Satz 0x25 = Welle 45d7a753 (1,07 s): 0..0,30 s
Klack (Spektrum = 2110 0x0C, L1-Abstand 0,085) + 0,42..1,06 s Ton 1450 Hz (= Piep). Mechanische Schluessel
(Satz 0x25 in 1050/1140/2040/6030/6060/7000) haben KEINEN Ton-Anteil (Flachheit 0,15..0,43): **der Piep gehoert nur
zu Karten**, das Aufschliess-Geraeusch zu allen.

(Umsetzung folgen.)
