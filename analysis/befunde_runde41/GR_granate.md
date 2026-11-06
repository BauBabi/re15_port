# Spur GR — Handgranate zerfetzt Zombies (Runde 41, 2026-11-02)

Zweig `r41/granate`, Baum `.claude/worktrees/r41_granate`, Basis `r41/spinnengore` 2f886e91a.
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz).

## N3. NACHBESSERUNG 3 (2026-11-02) — Birkin-HP x 2, Riesenform 1000, Gator-Boss nach der Boss-Regel, ROOM3080 gemessen

> Nutzer auf F1-F5: "1. ja, 2. ja, 3. huh, wie jetzt das ist doch ein Boss fight. Warum soll die Granate nicht wirken ?...
> zu 4. Ja, nicht einfach mit einer Granate beenden."

Zuordnung (Orchestrator): 1 = F1 Birkin-HP x 2 (300 -> 600); 2 = F2 Riesenform/Bahnsteig-Form auf RE2-G4-Niveau; 3 = G-Form 0x36 in
ROOM3080 (im Dossier N2.7 als F4 gefuehrt); 4 = Gator-Boss 2090 (N2.7 F5) - eine Granate darf den Kampf nicht beenden.

### N3.1 Belege (selbst disassembliert, vor jeder Zeile Code)

Birkin-Wurzel 0x80116230 (STAGE3.BIN, `re15_disasm.py dis ... --bin STAGE3.BIN`):

| Stelle | Bytes / Instruktion | Bedeutung |
|---|---|---|
| @0x801164fc | `slti v0,v0,200` (nach `lh v0,154(v1)` @0x801164f4) | Verwundet-Tor: +0x1dd ^= 4 (DECIDE-Mutationstor) |
| @0x8011667c-688 | `slti v0,v0,100` / `beq` / `ori v0,zero,0x601` / `sw v0,4(v1)` | Mutation bei hp < 100 (nur Form 1) |
| @0x8011690c-910 | `ori v0,zero,0x12c` / `sh v0,154(v1)` | INIT HP 300, unbedingt |
| @0x80118094-098 | `ori v0,zero,0x96` / `sh v0,154(v1)` | HP in der Mutation 150 |
| @0x8011a530-534 | `ori v0,zero,0x32` / `sh v0,154(v1)` | Revive-Waechter: toedlicher Treffer in der Mutation -> 50 |
| @0x8011a134-138 | `addiu v0,v0,5` / `sh v0,154(v1)` | Fern-Rueckgabe +5 je Treffer ab Abstand 0xfa1 (bleibt, s. N3.2) |

STAGE5.BIN (Raeume 50xx) traegt dieselben Werte um +0x814 versetzt: `slti 200` @0x80116d10, `slti 100` @0x80116e90, `ori 0x12c`
@0x80117120, `ori 0x96` @0x801188a8, `ori 0x32` @0x8011ad44 (gelesen).

RE2-G4-Ctor (CDEMD0.EMS rec 1, Sektor 3238, 0xB2D4 Byte, gelinkt @0x80100000; Werkzeug `GR_belege/nb3/werkzeuge/g4dis.py`):
`andi v0,v0,0x40` @0x80100670, `beq v0,zero,0x8010069c` @0x80100674, Verzoegerung `addiu v0,zero,1000` @0x80100678,
`sh v0,342(s1)` @0x8010067c; `andi v0,v0,0x20` @0x80100688, `beq` @0x8010068c, `addiu v0,zero,900` @0x80100690 / `sh` @0x80100698;
Zweig ohne 0x40: `addiu v0,zero,1100` @0x8010069c, mit 0x20 `addiu v0,zero,1000` @0x801006b4. Bit 0x40 des RE2-Spielworts
0x800CFB74 ist im ausgelieferten RE2 immer gesetzt, 0x20 nie (Schreiber-Zensus, re15_damage.c "START-HP AUS DEN RE2-EM-MODULEN")
-> erreichbar ist genau **1000**.

RE2-Alligator (EM23_OVL_0000.BIN): `lh v0,342(s0)` @0x80101ebc, `slti v0,v0,25000` @0x80101ec4, `beq` @0x80101ec8,
`ori v0,v0,0x20` / `sh v0,536(s0)` @0x80101ed8-dc = Schwaeche bei hp < 25000 (loest im Port die Kanister-Szene aus).

Waffenzeile 0x36 (PSX.EXE `read 0x8006f360 22 --w 2 --stride 4`): **22 x 0** (alle Spielerwaffen). Explosionsarten
DAT_8006f418 (`read 0x8006f418 11 --w 2 --signed`): [10, 20, 1000, 1000, 1000, 50, 100, 200, 300, 1000, 0].

### N3.2 Regel (PORT-WAHL, Nutzerentscheide)

* **F1 Birkin x 2:** die GANZE HP-Skala der EM30-Wurzel mal `RE15_BIRKIN_HP_FAKTOR` 2 (`include/re15_birkin_formen.h`):
  INIT 600, Verwundet-Tor < 400, Mutations-Tor < 200, HP in der Mutation 300, Revive 100. Begruendung: der Nutzer bejahte
  "wie beim Gorilla x 2"; die Tore haengen an den HP, ohne Mitziehen kaeme die Mutation erst nach 500 von 600 Schaden (5/6 statt
  2/3) und die Form nach der Mutation haette nur ein Viertel der Start-HP. Mit dem Faktor bleiben alle Verhaeltnisse des Originals
  (Tor 1/3, Verwundet 2/3, Mutations-HP 1/2, Revive 1/3 der Mutations-HP). NICHT skaliert: die Fern-Rueckgabe +5 (@0x8011a134) -
  sie gehoert zum Schaden je Treffer, der unveraendert bleibt. Gilt auch fuer die Hangar-Form (birkin_formen.c setzt beim Aufbau
  denselben INIT-Wert) und fuer den 0x36 in ROOM3080 (gleiche Wurzel, dort waffen-immun und in einer Szene, ohne Wirkung).
* **F2 Riesenform 1000:** `RE15_BB_FORM_HP` 150 -> 1000 (G4-Ctor, N3.1). Nur die HP-Stelle im Eintritt von re15_bb_tick
  (birkin_bahnsteig.c) und die Sichtpruef-Zuweisung daneben; sonst nichts an der Form (Zweig r41/form arbeitet dort).
* **Granate an den Birkin-Formen:** unveraenderte Boss-Regel aus N2 (eine Granate aus voller Form-HP laesst R = 110, danach genau
  8 Pistolenschuesse): 600 -> 490, Form 1 600 - 200 - 110 = 290, Mutation/nach der Mutation 300 -> 190, Riesenform 1000 -> 890.
* **F4 Gator-Boss:** dieselbe Boss-Regel wie der Alligator (R = 50, N = 8): die "Form" des Gator-Bosses ist der Abstand Sockel ->
  Schwaeche, 25300 - 25000 = 300 - genau die HP des RE1.5-Alligators (@0x801175dc) bei demselben Pistolenschaden 7 (@0x8006ece4).
  Granate = 25300 - 25000 - 50 = **250**; eine Granate laesst 25050 stehen, danach toeten (schwaechen) genau 8 Pistolenschuesse.
  ABWEICHUNG vom Orchestrator-Vorschlag 100 ("1/3 der Luecke, 3 Granaten oder 43 Treffer"): 3 x 100 = 300 bringt 25300 nur auf
  25000, und die Schwelle ist `slti 25000` = hp < 25000 - es braeuchte 4 Granaten oder 3 Granaten + 1 Schuss. Vor allem waere
  der Gator-Boss dann der einzige Boss, an dem eine Granate nur 14 Pistolentreffer wert ist (100 / 7), waehrend sie ueberall sonst
  "Form-HP minus 8 Schuss" traegt; der Nutzer hatte in N2 "Gorilla, Handgranate und ein paar Schuss, und ja auch Birkin und
  Aligator" fuer die Regel bejaht. Wer 100 will: eine Zahl in `re15_granate_schaden` (re15_damage.c), Test `[6]` Fall 82.
* **F3 ROOM3080:** nicht geaendert - Befund N3.4, Nutzerfrage N3.9.

### N3.3 Boss-Tabelle (gemessen, `GR_belege/nb3/nb3_boss_tabelle_unit_r41_granate.txt`; "alt" = Stand 5670d7cab, N2.2)

Schaden ueber den echten Hitscan (re15_player_weapon_fire, Waffe 3 BROWNING HP) und die echte Explosion (re15_granate_r35_explosion,
Art 2), Form-HP aus dem echten INIT bzw. dem echten Eintritt (Riesenform: G4-Bank ueber den Plattform-Haken, erster Formen-Takt).

| Boss / Form | HP alt -> neu | Pistolenschaden | Granate alt -> neu | Schuesse nach 1 Granate | ohne Granate alt -> neu |
|---|---|---|---|---|---|
| Gorilla 0x27 (11C0/1141) | 360 (unveraendert) | 12 @0x8006ee44 | 320 | 4 | 31 |
| Alligator 0x23 RE1.5-KI (nur Messhilfe) | 300 | 7 @0x8006ece4 | 250 | 8 | 43 |
| Birkin 3070 / 5080 / Hangar-Sichel 50A0 / Labor 50E0+50F1 | 300 -> **600** | 15 @0x8006f15c | 190 -> **490** | 8 | 21 -> **41** |
| Birkin Form 1 bis zur Mutation (3071, 5090) | 300 bis < 100 -> **600 bis < 200** | 15 | 90 -> **290** | 8 (bis zur Mutation) | 14 -> **27** |
| Birkin waehrend der Mutation | 150 -> **300** (Revive 50 -> 100) | 15 | 40 -> **190** | — (stirbt in der Mutation nie) | — |
| Birkin nach der Mutation (3071) | 150 -> **300** | 15 | 40 -> **190** | 8 | 11 -> **21** |
| Bahnsteig-/Riesenform (5090) | 150 -> **1000** | 15 | 40 -> **890** | 8 | 11 -> **67** |
| G5 0x36 (5090) | 600 (unveraendert) | 15 | 80 (E16) | 35 | 41 |
| Gator-Boss 0x23 (2090), bis zur Schwaeche | 25300, Schwaeche < 25000 | 7 | 1000 -> **250** | **8** (vorher: 1 Granate = sofort geschwaecht) | 43 |

Ganze Kaempfe ohne Granate (Pruefung 91, Gorilla 31):

| Kampf | Pistolenschuesse alt -> neu |
|---|---|
| Leon 3070, 5080, Hangar/Sichel 50A0, Labor 50E0/50F1 | 21 -> **41** |
| Elza 3071 (Form 1 + nach der Mutation) | 14 + 11 = 25 -> 27 + 21 = **48** |
| Endkampf 5090 (Form 1 + Riesenform + G5) | 14 + 11 + 41 = 66 -> 27 + 67 + 41 = **135** |
| Gator-Boss bis zur Schwaeche | 43 (unveraendert) |

Mit je EINER Granate pro Form: Leon 3070 = 1 + 8; Elza 3071 = (1 + 8) + (1 + 8); 5090 = (1 + 8) + (1 + 8 Riesenform) + G5 (1 + 35);
Gator-Boss 1 + 8 bis zur Kanister-Szene. Der Gorilla bleibt mit 4 Schuss nach der Granate der schwaechste Boss (Pruefung 90).

### N3.4 Befund ROOM3080 (F3): eine Szene, kein Kampf - im Original wie im Port

Gemessen auf zwei Wegen, dazu die 40%-Disc gelesen.

1. **Raumprobe** (`unit_r41_granate [7] g3080`, `GR_belege/nb3/nb3_room3080_raumprobe.txt`): ROOM3080 geladen, SCD wie im Hauptlauf
   (Rahmen (1,27)/(2,7) -> player_mode 2), Spieler auf dem Tuerziel aus ROOM3070 (Door_aot_set @0x03302), Texte bestaetigt; unter
   RE2-KI (Vorgabe) und RE1.5-KI gleich:
   * sub00 setzt den 0x36 mit grid 0x24 (Sce_em_set @0x007D4) und startet sub02 (Evt_exec @0x007E8); sub02 setzt sofort Set(4,96)
     @0x007FA, **Set(2,7) @0x007FE und Set(1,27) @0x00802** (Szenenrahmen) und loescht beide bis zum Raumende nie.
   * Bild 1 Spawn (HP 600 nach NB3), Bild 367 Freigabe grid 0x24 -> 0x04 (Member_set(0x0c,4) @0x00934), danach der
     Szenenmodus Nibble 4 (BRAIN @0x80116d98 -> Tabelle @0x8011eea4[4] = 0x80116e48, DECIDE 0x80118cf8 = `jr ra`, ACT 0x80118d00)
     bis Phase 14 (@0x8011935c, steht), Bild 907 Raumwechsel ueber die Tuer (Aot_on @0x00A22, Door_aot_set @0x0072A -> ROOM4000).
   * **906 Bilder mit dem 0x36, in 0 davon hat der Spieler Kontrolle (player_mode 2), 0 Treffer am Spieler.**
   * Schaden am lebenden 0x36 (Bild 397, Zustand gesichert und zurueckgesetzt): **alle 21 Spielerwaffen treffen und machen 0**
     (Zeile @0x8006f360); Explosionsarten 2..9 = 1000/1000/1000/50/100/200/300/1000 (DAT_8006f418, die Explosion geht nicht ueber
     die Waffenzeile).
2. **Echte exe** (eigene Kopie, eigene PID; `RE15_DEBUG_JUMP=3080@250`, Handgranate ausgeruestet `RE15_GIVE=9:5 RE15_EQUIP=9`,
   Skript haelt Zielen + Feuer `MA4` x 12 ab Spielbild 260; Auswertung `GR_belege/nb3/nb3_room3080_echte_exe_auswertung.txt`, Werkzeug `GR_belege/nb3/werkzeuge/ana3080.py`):
   0x36 in 912 Bildern, grid 0x24 -> 0x04 im Bild 372, Phase bis 14; **in allen 912 Bildern pm = 2**, in 623 davon war die
   Feuer-Eingabe gedrueckt, der Zielclip blieb 6 (kein Zielen), **die 5 Granaten blieben im Inventar**, HP des 0x36 blieb 600,
   Spieler-HP 100; danach ROOM4000 (Ziel der Aufzugstuer; das weiterlaufende Skript fuehrte noch nach ROOM4010).
3. **40%-Disc** (`Version_40%/PSX/STAGE3/ROOM3080.RDT`, nur gelesen): derselbe Spawn 0x36 grid 0x24 (@0x0078C), Member_set(0x0c,4)
   @0x00AD6; sub02 setzt (2,7)/(1,27) @0x00804/@0x00808 und loescht sie erst UNMITTELBAR vor dem Aot_on (@0x00892/@0x00896/@0x0089A).
   Auch dort ist 3080 eine Zwischensequenz in der Aufzugsfahrt.

Ergebnis: ROOM3080 ist im ausgelieferten Stand, auf der 40%-Disc und im Port eine **Szene ohne Kampf** - der Spieler hat keine
Kontrolle, solange die G-Form da ist, die G-Form greift nicht an, keine Waffe des Spielers macht ihr Schaden (Original-Zeile 22 x 0).
Eine Granate "wirken lassen" hat dort keinen Ansatzpunkt: sie kann nicht geworfen werden. Nach dem Auftrag ("Falls die anderen
Waffen dort gar keinen Schaden machen ... NICHT eigenmaechtig aendern") ist nichts geaendert; Frage N3.9 F3.

### N3.5 Pistolen-Integrationslauf (integration_r41_birkin_pistole) mit Form-HP 1000

Auftrag: "Pruefe, ob die Form mit 1000 HP mit der Pistole im Pistolen-Integrationslauf noch besiegbar ist (Munition im
Raum/Inventar), sonst melden." Echte exe (eigene Kopie, eigene PID), Ablauf des Tests: Debug-Sprung 5090/5091, Zielen + Feuer
gehalten (Pistole alle 22 Bilder), keine Bewegung, keine Heilung. Drei Laeufe, Bildnummern aus state.log:

| Lauf | Form 1 (600) | Riesenform (1000) | Rueckstoss / G5 | Ergebnis |
|---|---|---|---|---|
| 1 ohne Haken, `RE15_GIVE=3:400` (Leon) | F406 - Mutation F1274, 33 Schuss | ab F1455, Griff F1608 (Spieler 80 -> 45), Hiebe/Biss | — | **Spieler tot F2100 bei Form-HP 700**, danach Game over / Neustart ROOM1240 (Auszug `GR_belege/nb3/nb3_pistolenlauf_auszug.txt`) |
| 2 Messhaken `RE15_R39_MESS_SPIELER_HP=100`, 3:400 (Leon + Elza) | wie 1 | F1455 - F3430, 67 Schuss in 1975 Bildern (66 s Dauerfeuer) | Rueckstoss F3431, G5 F3594 | **Waffe leer F4356 bei G5-HP 330** - `RE15_GIVE=3:400` lud nur 144 Schuss (Menge ist ein Byte: 400 & 0xff), der Skript-Schuetze laedt nicht nach  |
| 3 Haken + `RE15_GIVE=3:255` (Leon + Elza, bildgleich) | wie 1 | wie 2 | Rueckstoss F3431, G5 F3594, **G5 tot F4706** | gruen; 255 -> 89 Schuss = **166 Schuss** fuer den ganzen Endkampf  |

Antwort: Die Riesenform mit 1000 HP IST mit der Pistole besiegbar - 67 Treffer, gut eine Minute Dauerfeuer -, aber NICHT von
einem stehenden Schuetzen ohne Ausweichen und Heilen (Lauf 1). Munition: der ganze Endkampf mit der Pistole allein kostet 166
Schuss (Form 1 bis zur Mutation 33, Mutation 8, Riesenform 67, Rueckstoss-Szene 7, G5 51). Handfeuerwaffen-Patronen (0x15) liegen
im Repo-Baum an 38 Stellen, ALLE in STAGE1 (Leon 19 Stellen = 405 Schuss, Elza 19 Stellen = 405 Schuss; Zensus
`tools/discard_zensus.py`, Item_aot_set 0x50, `GR_belege/nb3/nb3_item_zensus_discard_zensus.json`); in STAGE2-5 und in 5090 liegt keine.
Kein Item_aot_set legt Handgranaten (0x09) aus. Ob ein Spieler mit 166 Pistolenpatronen in 5090 ankommt, haengt am Verbrauch
vorher - mit je einer Granate pro Form sind es 8 + 8 + 35 Pistolentreffer plus 3 Granaten (Tabelle N3.3, ohne Fehlschuesse
und ohne die Schuesse in Mutation und Rueckstoss-Szene).

Der Integrationstest (`integration_r41_birkin_pistole`) laeuft deshalb mit dem Messhaken der Runde 39 (Spieler-HP am Kopf jedes
KI-Bildes auf 100; negative HP hebt er nicht, das Fressen des G5 setzt -1 - Pruefung 5 aus Abnahme 2 bleibt scharf) und 255
Schuss; neu geprueft: Form 1 startet mit 600, die Riesenform mit 1000; Endbild 5400 (G5 tot F4706), Zeitlimit 600 s je Lauf,
ctest 1300 s.

### N3.6 Umsetzung

* `include/re15_birkin_formen.h`: `RE15_BIRKIN_HP_FAKTOR` 2 und `RE15_BIRKIN_HP_INIT/_VERWUNDET/_MUT_TOR/_MUTATION/_REVIVE`
  (je Adresse), `RE15_BB_FORM_HP` 1000 (G4-Beleg).
* `engine/src/enemy_ai_common.c` (re15_birkin_root): INIT @0x8011690c, Mutations-HP @0x80118094, Revive @0x8011a534, Tore
  @0x801164fc / @0x8011667c ueber die Makros.
* `engine/src/birkin_formen.c`: `re15_bf_granate_form` ueber die Makros (Kopfkommentar mit den neuen Zahlen), Hangar-Aufbau
  `RE15_BIRKIN_HP_INIT`.
* `engine/src/birkin_bahnsteig.c`: Eintritt `e->hp = RE15_BB_FORM_HP` (vorher 150) und die Sichtpruef-Zuweisung daneben.
* `engine/src/re15_damage.c` `re15_granate_schaden`: Gator-Boss 250 (RE15_GR2_HP_INIT - RE15_GR2_HP_SOCKEL - 50), Kopfblock
  (3)/(4) neu; Include `re15_gator_re2_r41.h`.

### N3.7 Tests

* `unit_r41_granate` (`tests/unit/test_r41_granate.c`):
  * `[6] boss` neu gefasst: Form-HP je Fall gegen die Makros (600 / 300 / Riesenform 1000 / Gator 25300), die Riesenform ueber den
    ECHTEN Eintritt von re15_bb_tick (G4-Bank wie der Plattform-Haken, `g4_haken`, Muster test_birkin_bahnsteig.c form_bank) statt
    des Mutationsrests, der Gator-Boss bis zur Schwaeche (Ende 25000, 1 Granate + 8 Schuss, Pruefung 82; vorher "unveraendert
    1000"), neue Pruefung 91 "ohne Granate braucht jeder Birkin-Kampf und der Gator-Boss mehr Pistolenschuesse als der Gorilla
    (31)" ueber die Kampf-Summen (N3.3) - 7/7. Ergebnis 70-82, 90, 91 gruen (`GR_belege/nb3/nb3_boss_tabelle_unit_r41_granate.txt`).
  * `[7] g3080` neu: ROOM3080 unter RE2-KI und RE1.5-KI (Pruefungen 100-105, N3.4) - gruen.
* HP-Pins ueber die Makros statt fester Zahlen: `test_birkin_ai.c` (INIT 600, Mutation 300, Revive 100, Form-5 600),
  `test_birkin_szenen.c` (nach der Mutation 300, Form 1000), `test_birkin_bahnsteig.c` M3 (Form kommt mit 1000, Vorbedingung
  < 300). `tests/integration/test_r35_granate.cmake` Lauf gator: -250 statt -1000.
* Gezielt (`ctest -R "birkin|granate|gator|bahnsteig|r34_schaden|r34_wurf|r34_reaktion|g5|5090|3070|3071|50a0|50e0|hangar|werfer|form"
  -E ^integration_`): **80/80 gruen**.
* Volle Unit-/Prop-/Probe-Suite (`ctest -E "^integration_|releaseskript|titelversion_ablauf" -j 4 --timeout 1200`, Stand
  2fb07b019): **1069/1069 gruen** (1185 s). Ausgenommen die 8 Release-Skript-Tests (unit_r38_releaseskript_* x 7,
  unit_r39_titelversion_ablauf): das Skript prueft den freien Platz mit `df -Pk`, und `df.exe` haengt auf dieser Maschine
  weiter (N2.4; ein eigenes `timeout 10 df -Pk /c/workspace/git/reAi_v2` kam auch jetzt nicht zurueck, die haengenden
  df-Prozesse dieses Baums aus N2 stehen noch). Die Tests beruehren keine Engine-Datei. Gesondert mit 150 s
  Zeitlimit: unit_r38_releaseskript_bausteine (4,7 s) und _version (30,4 s) gruen; _trocken, _ablauf, _fehler_bau,
  _fehler_umfeld, _cmd und unit_r39_titelversion_ablauf haengen an `df.exe -Pk .../fx_*/repo` (Prozessliste), ctest setzt das
  Zeitlimit nicht durch, weil df nicht beendbar ist; Lauf abgebrochen. BEKANNT (N2.4), keine Regression dieser Aenderung.
* Integration (`-j 3`): `integration_r34_granaten` 446 s, `integration_r35_granate` 366 s, `integration_r36_granate` 204 s -
  **3/3 gruen**. Im r35-Lauf gator (echte exe, `RE15_FORCE_EXPLOSION=2@60:35`): F59 HP 25300 -> F60 **25050** (Zustand 2),
  F61 Zustand 1 - nach einer Granate liegt er mit 25050 ueber der Schwaeche-Schwelle (hp < 25000 @0x80101ec4).
* `integration_r41_birkin_pistole` (neue Fassung, ctest, Zeitlimit 1300): **gruen** (463 s) - Leon und Elza: Mutation F1274,
  Form 1 HP 600, Riesenform 1000 ab F1455, Rueckstoss F3431, G5 F3594, G5 tot F4706, 166 Schuss.
* VERSEHEN beim Abbrechen der haengenden Release-Tests: der Filter `ctest.exe` mit "releaseskript" in der Befehlszeile traf
  ausser dem eigenen Lauf (PID 81412) auch einen FREMDEN ctest (PID 111104, `--test-dir re15_port/build -R
  ^(unit_r35_android_bash_mutanten|unit_r38_releaseskript_*|unit_r39_titelversion_ablauf)$ -j 3`, Baum unbekannt); er ist
  beendet und muss von seinem Besitzer wiederholt werden.

### N3.8 Integrationshinweise fuer Zweig r41/form (Spur Form, Worktree r41_form)

r41/form (Kopf d8eab158b, Basis aa88a1b6a vor GR-N2) baut die Riesenform um (Stufe 3: keine Mutation mehr in 5090, Riesenform ab
der Raumladung, auch 5080/5081 ueber `re15_bf_riesenform_raum`, Asset `RE2/BAHNSTEIG_FORM.EMD` statt `BAHNSTEIG_G4.EMD`). Beim
Zusammenfuehren:

1. `engine/src/birkin_bahnsteig.c`: r41/form hat `e->hp = 150;` im Eintritt (dort Zeile ~646) und in der Sichtpruefung (~721)
   unveraendert - beide auf `RE15_BB_FORM_HP` (= 1000, re15_birkin_formen.h) setzen; die neuen `e->hp = 1;` der Rueckstoss-/
   Sturz-Szene bleiben.
2. `include/re15_birkin_formen.h`: beide Zweige aendern den Kopf; `RE15_BB_FORM_HP 1000` mit G4-Beleg und die
   `RE15_BIRKIN_HP_*`-Makros uebernehmen, `re15_bf_granate_form` (aus GR-N2) behalten.
3. `tests/integration/test_r41_birkin_pistole.cmake`: r41/form erwartet `_form_hp EQUAL 150` ("Form kommt mit HP 150 und
   laeuft", "Form-HP 150 bis 0") -> **1000**; die GR-Pruefung "Form 1 hat hoechstens HP 600" entfaellt dort (keine Form 1 mehr in
   5090; dann stattdessen pruefen, dass die Riesenform ab der Raumladung 1000 traegt). Endbild/Zeitlimit, Messhaken
   `RE15_R39_MESS_SPIELER_HP=100` und `RE15_GIVE=3:255` aus GR uebernehmen (N3.5) - mit 1000 HP stirbt der stehende Schuetze sonst.
4. `tests/unit/test_birkin_bahnsteig.c` (r41/form +270 Zeilen) und `tests/unit/test_birkin_szenen.c` (r41/form +39): die
   Pruefungen "Form kommt mit HP 150", "b->hp < 150" (M3) und "nach Mutation hp 150" laufen bei GR ueber `RE15_BB_FORM_HP` /
   `RE15_BIRKIN_HP_MUTATION`; in der r41/form-Fassung dieselben Stellen umstellen (M3 entfaellt dort ggf. mit der Mutation).
5. `tests/unit/test_r41_granate.c [6]`: die Faelle "Birkin 0x30 5080", "Form 1 5090" und "Bahnsteig-/Riesenform 5090" setzen den
   alten Ablauf voraus (5080 = EM30 600, 5090 Form 1 -> Mutation -> G4-Bank aus `BAHNSTEIG_G4.EMD` ueber `g4_haken`). Mit Stufe 3:
   5080 und 5090 tragen die Riesenform (1000, Granate 890, 8 Schuss), `g4_dateien` auf `BAHNSTEIG_FORM.EMD` umstellen, der
   Kampf "Endkampf 5090" in Pruefung 91 wird Riesenform + G5 = 67 + 41 = 108 Schuss (> 31 Gorilla, bleibt gruen).
6. Tests aus GR, die r41/form nicht kennt: `test_birkin_ai.c` (INIT/Mutation/Revive ueber die Makros) - konfliktfrei.

### N3.9 Offene Nutzerfragen

* **F3 ROOM3080:** 3080 ist im ausgelieferten Stand und auf der 40%-Disc eine Zwischensequenz in der Aufzugsfahrt (N3.4): Spieler
  gesperrt, die G-Form greift nicht an, alle Spielerwaffen 0 Schaden (Original-Zeile @0x8006f360 = 22 x 0). Soll der Port dort einen KAMPF bekommen (dann: Spieler freigeben, G-Form mit
  Angriffen, eigene HP/Waffenzeile - ein eigener Bauauftrag, kein Granaten-Schalter), oder bleibt 3080 die Szene?
* **F4 Gator-Boss:** umgesetzt ist die Boss-Regel 250 (1 Granate + 8 Schuss bis zur Kanister-Szene), nicht der Vorschlag 100
  (N3.2). Recht so?
* **Endkampf 5090 mit der Pistole:** ohne Granate jetzt 135 Treffer (N3.3); ein stehender Schuetze ueberlebt die Riesenform nicht
  (N3.5). Handfeuerwaffen-Patronen liegen im Repo-Baum nur in STAGE1 (N3.5). So gewollt (Granate/andere Waffen noetig), oder
  Riesenform-HP niedriger?

---

## N2. NACHBESSERUNG 2 (2026-11-02) — Schaden der Handgranate: normale Gegner 1000, Bosse nach Form (Nutzerentscheide)

> "Naja, die Granate hat halt einen Schaden, sind die Gegner damit tot, sind sie tot. Der Gorilla ist ein Endgegner, wenn
> der mit einer Granate stirbt ist das etwas schwach."
> Auf den Vorschlag (1000 fuer alle normalen Gegner unabhaengig von der KI, Bosse weniger): "Ja, ist gut. Gorilla,
> Handgranate und ein paar Schuss, und ja auch Birkin und Aligator, wobei die an sich schon staerker sein muessen als der
> Gorilla."

### N2.1 Messung VORHER (Stand d8d847a93, echter Hitscan + echte Explosion)

Werkzeug: neuer Abschnitt `[6] boss` in `tests/unit/test_r41_granate.c`, gegen die Engine VOR der Aenderung gebaut (Engine-
Dateien per `git stash` zurueckgenommen, Test unveraendert), Ausgabe `scratchpad/GR3/boss_vorher.txt`. Je Boss frisch
aufgestellt, INIT ueber die echte KI (Spawn-Wurzelaufruf `re15_enemy_spawn_root` = Sce_em_set @0x8004259c bzw. ein KI-Takt);
Schuss ueber `re15_player_weapon_fire` (Waffe 3 = BROWNING HP, Spieler 2000 davor), Granate ueber `re15_granate_r35_explosion`
(Art 2, Liegestelle 300 daneben). Formende: Tod (`hp < 0`) bzw. bei Form 1 `hp < 100` + ein KI-Takt -> Mutation (sub 6).

| Boss / Form | Form-HP (Beleg) | Pistole | Schrot | Granate vorher | Schuesse ohne Granate |
|---|---|---|---|---|---|
| Gorilla 0x27 (11C0/1141) | 360 = 180 (STAGE1 @0x8011f514, 16 x 180; INIT `sh v0,154(a1)` @0x80117104) x 2 (Runde 39 Spur E) | 12 (`0c 00` @0x8006ee44) | 38 (`26 00` @0x8006ee58) | 1000 -> tot | 31 |
| Alligator 0x23, RE1.5-KI | 300 (STAGE2 @0x801175dc, 16 x 300; `sh v0,154(a1)` @0x8010c6d4) | 7 (`07 00` @0x8006ece4) | 30 (`1e 00` @0x8006ecf8) | 1000 -> tot | 43 |
| Birkin 0x30, Form-2-Bit vom Spawn (3070 Leon grid 0x10, 5080 grid 0x13, Hangar/Sichel 50A0, Labor 50E0/50F1) | 300 (`ori v0,zero,0x12c` @0x8011690c), Tod bei hp < 0 | 15 (`0f 00` @0x8006f15c) | 40 (`28 00` @0x8006f170) | 1000 -> tot | 21 |
| Birkin 0x30 Form 1 (3071 Elza grid 0x21 -> 0x01 per Member_set(0x0C,1) @0x03645; 5090 vor der Mutation) | 300 bis hp < 100 (`slti v0,v0,100` @0x8011667c -> `ori v0,zero,0x601` @0x80116684) | 15 | 40 | 1000 -> Mutation sofort | 14 |
| Birkin waehrend der Mutation | 150 (`ori v0,zero,0x96` @0x80118094); toedlicher Treffer -> HP 50 (`ori v0,zero,0x32` @0x8011a530) | 15 | 40 | 1000 -> HP 50 (Revive-Waechter) | — |
| Birkin nach der Mutation (3071) | 150 (@0x80118094) | 15 | 40 | 1000 -> tot | 11 |
| Bahnsteig-/Riesenform (5090 nach der Mutation) | 150 (Eintritt re15_bb_tick, birkin_bahnsteig.c; Port-Mass = die 0x96 der Mutation) | 15 | 40 | 1000 -> tot | 11 |
| G5 0x36 (5090/5091) | 600 (Ctor @0x801003fc) | 15 (0x30-Zeile im Endkampf) | 40 | 80 (E16, `0x05014050` @0x800A5F7C) | 41 |
| Gator-Boss 0x23 (2090) | 25300 (`RE15_GR2_HP_INIT`, @0x80100518-580) | 7 | 30 | 1000 -> 24300 | — (nur Kanister toedlich) |

Normale Gegner unter dem RE2-Modell (vorher, gemessen in `[1]`): Hund 300 (RE2 Zeile 9 K0 `0x00a0c92c` @0x800A44C4), Kraehe 60
(`0x00f0f03c` @0x800A4640), Spinne und RE2-Babyspinne 60 (`0x0140f03c` @0x800A4C30) — RE2-Granatwerfer-Zeile, obwohl RE2 keine
Handgranate hat. Unter RE1.5-KI und MIXED (ausser Hund) schon 1000.

### N2.2 Regel (PORT-WAHL, Nutzerentscheid) und Boss-Tabelle NACHHER

* **Normale Gegner:** die Handgranate (Art 2) traegt an JEDEM normalen Gegner unter JEDER KI ihre eigene Zahl 1000
  (`e8 03` @0x8006f41c, Aufruf FUN_80012d60(500,&P,2) @0x80018598-bc). Dieselbe Begruendung wie fuer die Zombie-Familie (Spur GR
  §4): die RE2-Zeile 9 war eine Zuordnung, die 1000 ist der Wert des Gegenstands. Werfer-Explosivrunde (Art 6, Zeile 9),
  Saeure/Brand (Art 3/4) unveraendert.
* **Bosse:** eine Granate aus VOLLER Form-HP laesst einen Rest R stehen, danach toeten genau N Pistolenschuesse:
  Schaden = Form-HP - Formende - R. Gorilla N = 4 (Nutzer "ein paar Schuss"): R = 40 (40 - 3 x 12 = 4 >= 0, 40 - 4 x 12 < 0).
  Birkin und Alligator "an sich staerker als der Gorilla" -> N = 8 (Faktor 2): Birkin R = 110 (110 - 7 x 15 = 5,
  110 - 8 x 15 < 0), Alligator R = 50 (50 - 7 x 7 = 1, 50 - 8 x 7 < 0). Die HP selbst sind NICHT angefasst.
* **Unveraendert:** G5 (80), Gator-Boss 2090 (1000; Treffer nur ueber den Kanister toedlich), 0x36 ausserhalb 5090 (Szene
  ROOM3080, waffen-immun `00 00` @0x8006f36c; siehe Frage F4).

| Boss | HP (Form) | Pistolenschaden | Granatenschaden neu | Schuesse nach 1 Granate | Schuesse ohne Granate |
|---|---|---|---|---|---|
| Gorilla 0x27 | 360 | 12 @0x8006ee44 | **320** (vorher 1000) | **4** | 31 |
| Alligator 0x23 (RE1.5-KI) | 300 | 7 @0x8006ece4 | **250** (vorher 1000) | **8** | 43 |
| Birkin 300-Form (3070, 5080, Hangar/Sichel 50A0, Labor 50E0/50F1) | 300 | 15 @0x8006f15c | **190** (vorher 1000) | **8** | 21 |
| Birkin Form 1 bis zur Mutation (3071, 5090) | 300 -> Mutation bei < 100 | 15 | **90** (vorher 1000 = sofort Mutation) | **8** bis zur Mutation | 14 |
| Birkin waehrend der Mutation | 150 (Revive-Waechter) | 15 | **40** (vorher 1000 -> HP 50) | — (stirbt in der Mutation nie) | — |
| Birkin nach der Mutation (3071) | 150 | 15 | **40** (vorher 1000) | **8** | 11 |
| Bahnsteig-/Riesenform (5090) | 150 | 15 | **40** (vorher 1000) | **8** | 11 |
| G5 0x36 (5090) | 600 | 15 | 80 (unveraendert) | 35 | 41 |
| Gator-Boss 0x23 (2090) | 25300 | 7 | 1000 (unveraendert) | — (Kanister) | — |

Alle Zahlen gemessen (`scratchpad/GR3/boss_nachher.txt`), nicht gerechnet. Ganze Kaempfe mit je EINER Granate pro Form:
Leon 3070 = 1 Granate + 8 Schuss; Elza 3071 = (1 Granate + 8 Schuss bis zur Mutation) + (1 Granate + 8 Schuss); 5090 =
(1 + 8 bis zur Mutation) + (1 + 8 Bahnsteig-Form) + G5 (1 + 35).

### N2.3 Umsetzung

* `re15_damage.c re15_granate_schaden(e)` (Block "RUNDE 41 SPUR GR, NACHBESSERUNG 2", jede Konstante mit Adresse): 1000 fuer
  jeden Typ ausser Gorilla (Form-HP 180 x Faktor der Runde-39-Regel - 40), Alligator ohne Boss-Modul (300 - 50), Birkin
  (Form-HP - Ende - 110). Aufruf im Gegnerzweig an der Stelle der frueheren Zombie-Regel (`if (type == 2u) dmg = ...`); der
  E16-Block fuer den G5 (80) steht unveraendert danach. Haengt nicht am Gore-Schalter.
* `birkin_formen.c re15_bf_granate_form(e, &form_hp, &ende)`: Bahnsteig-Form (G4-Bank oder 5090 nach der Mutation) 150/0;
  Mutation laeuft (+0x1dd & 8) 150/0; nach der Mutation (Merker je Slot, gesetzt in re15_bf_mutation_fertig = sub 6 Phase 2
  @0x8011888c, geloescht beim Raumwechsel; gilt nur bei hp <= 150) 150/0; Form 1 (Wurzel-Tor `andi v0,v0,0x10` @0x801164e8
  bzw. 5090 vor dem Mutationsstart) 300/100; sonst 300/0. `RE15_BB_FORM_HP` (re15_birkin_formen.h) = 150 der Bahnsteig-Form.
* Zerfetzen unveraendert wie N1 (nur Tod durch Explosivgranate an Zombie/Hund; Bosse werden nicht zerfetzt).

### N2.4 Tests

* `unit_r41_granate` erweitert: `[1]` Handgranate an ALLEN normalen Typen (0x10, 0x11, 0x16, Hund 0x20, Kraehe 0x21, Spinne 0x25,
  RE2-Babyspinne 0x26, kleiner Affe 0x29, Tyrant 0x2B, Zellenarm 0x1A) unter RE2-KI, RE1.5-KI und MIXED = 1000: **30/30**
  (vorher 25/30 — Hund/Kraehe/Spinne/Baby unter RE2-KI 300/60/60/60, Hund unter MIXED 300). Die Explosionshoehe P.y wird je Typ
  durchprobiert, bis der Kandidatentest (Band @0x8004716c-a4) greift (Kraehe RE1.5, Babyspinne, G5 lagen bei -500 daneben);
  geprueft ist der Schaden, nicht die Trefferflaeche. NEU `[6] boss`: 13 Boss-/Formfaelle (Tabelle N2.2) ueber echten Hitscan
  und echte Explosion; je Form "eine Granate beendet die Form nicht" + "danach genau N Pistolenschuesse" (Gorilla 4, sonst 8),
  bei Form 1 entscheidet ein echter KI-Takt (Wurzel-Tor) ueber die Mutation; Gorilla N < min(Birkin, Alligator) (Pruefung 90);
  G5 80 und Gator-Boss 1000 unveraendert. Vorher-Lauf (Engine d8d847a93, gleicher Test): 12 Fehler; nachher: alle gruen.
* Verhaltensbedingt angepasst (alte Pins "Granate toetet den Boss"): `probe_r34_schaden` (13: Gorilla-Dummy 180 - 320 = -140,
  neu 13b: normaler Gegner 0x29 = 1000 @0x8006f41c), `probe_r34_wurf` (24: Dummy 0x27 -140), `test_r35_granate` (112: 0x27
  -140), `probe_r34_reaktion` + `_ohne80` (187 Alligator, 197 Birkin: angeschlagen auf HP 100, damit die Granate der toedliche
  Treffer bleibt — Thema der Pruefungen ist der Todesablauf nach Zeile 9).
* Volle Unit-/Prop-/Probe-Suite (`ctest -E ^integration_ -j 5`, 2400 s): 1066 gruen; rot waren die 5 obigen Pins (angepasst,
  danach 7/7 gruen inkl. unit_r41_granate/_aus) und 6 Bash-/Release-Tests mit Timeout 1200 s unter Last (unit_r38_releaseskript_*
  x5, unit_r39_titelversion_ablauf). Ursache gemessen, NICHT diese Aenderung: das Release-Skript prueft den freien Platz mit
  `df -Pk <repo>` (`--min-frei-gb`), und `df.exe` haengt auf dieser Maschine zur Zeit ganz — auch ein eigenes
  `timeout 15 df -Pk /c/workspace/git/reAi_v2` kam nach 120 s nicht zurueck, die df-Prozesse lassen sich nicht beenden;
  dieselben Tests hingen gleichzeitig in den Baeumen r41_moosgang und r41_story80. Die Wiederholung (19:17) haengte
  wieder an `df.exe -Pk .../fx_ablauf/repo` und wurde abgebrochen. Die sechs Tests beruehren keine Engine-Datei.
* Integration `integration_r34_granaten` / `integration_r35_granate` / `integration_r36_granate` (`-j 3`, nach den Pin-Anpassungen): **3/3 gruen** (415 / 323 / 190 s); darin unveraendert G5 HP -80 (E16) und Gator-Boss -1000.

### N2.5 Sichtlaeufe im echten Fenster (gdigrab am eigenen Fenster, eigene PID, beschleunigter Renderer, echter Titel + Spielerwahl)

Werkzeug `scratchpad/GR3/cap.ps1` (Kopie des Abnahme-Werkzeugs), Bildleisten `stripc.py` (Ausschnitt, Bildsuche ueber
POS_FRAMES), Logs `nb2_sichtlaeufe_log_auszug.txt`. Je Lauf dieselbe Eingabe mit der exe vorher (d8d847a93, Kopie
`re15_pc_nb1_vorher.exe`) und nachher.

1. **Spinne ROOM2030, RE2-KI (Vorgabe), ECHTER Wurf** (Aufstellung wie Abnahme 1 s3: `RE15_PLAYER_POS=6550,2000,3072,3`,
   Handgranate, Wurf ab Spielbild 288): erste Explosion T=558 `resolver art=2 ... eingriffe=1` in beiden Laeufen.
   VORHER Raumbild F312 HP 110 -> 50 (Zustand 2, lebt; erst der zweite Wurf F471 -> -10). NACHHER F312 HP 110 -> **-890**
   (Zustand 3, +0x5 = 9), im Folgebild der RE2-Tod (Clip 12) mit den Babyspinnen; im Bild fliegen die gelb-braunen Splitter ab
   +0,1 s, ab +0,8 s steht am Ort keine Spinne mehr, nach 4 s liegen nur Reste.
   Bild: `GR_belege/nb2/nb2_spinne2030_re2ki_eine_handgranate_vorher_oben_neu_unten.jpg` (oben vorher: die Spinne steht nach dem
   Feuerball weiter, unten nachher: sie platzt).
2. **Gorilla ROOM11C0 (Leon, Pistole)**: `RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486
   RE15_FORCE_CUT=5` (Kamera, die den Kampfplatz zeigt; ohne den Haken blieb Cut 0 = leere Gasse), Granate ueber den
   Mess-Haken `RE15_FORCE_GRANATE=300:2` (echte Explosion der Routine 31, Wurf auf einen springenden Gorilla ist nicht
   wiederholbar; danach braucht Leon die Pistole, ein Waffenwechsel ist im Skript nicht moeglich), dann echte Pistolenschuesse
   `M1.0` + 16 x `MA0.1,M0.6` ab Spielbild 390, `RE15_R39_MESS_SPIELER_HP=100`.
   VORHER: F300 HP 360 -> **-640**, Zustand 3, Leiche ab F341 — der Gorilla stirbt an der Granate.
   NACHHER (gegner_r39.log): `F301 hp=40 akku=320 -> FLINCH`, `F390 hp=28`, `F453 hp=16`, `F516 hp=4 -> FLINCH`, F538 HP **-8**
   Zustand 3 (vierter Schuss), Leiche ab F608 — er ueberlebt die Granate, wird umgeworfen, steht wieder auf und stirbt nach
   vier Pistolentreffern.
   Bild: `GR_belege/nb2/nb2_gorilla11c0_granate_dann_pistole_vorher_oben_neu_unten.jpg` (Zeit relativ zum Feuerball:
   vorher liegt er ab +1 s tot, ab +8 s kommt der zweite Gorilla; nachher wird er umgeworfen (+1 / +3 s), steht ab +5,1 s
   wieder an Leon, der Todes-Commit liegt bei +7,93 s (F538), ab +8,3 s faellt er).

### N2.6 Berichtigungen aus Abnahme 1

1. **Kanal ROOM2000 (Mangel 1):** die Explosion WIRD gezeichnet. Berichtigt in N1.5 (Lauf ROOM2000), in der Bildunterschrift von
   `n1/n1_vergleich_video_cd2_gegen_port_2000_kanal_0x16.jpg` und in N1.8 Punkt 3: im Haken-Lauf (Cut 6,
   `RE15_FORCE_GRANATE=100:4`) lag der Feuerball ausserhalb des Bildausschnitts (Mitte sx=2 sy=341 bei 320x240,
   `GR_belege/abnahme1/a1_kanal2000_fxlog_projektion.txt`); beim echten Wurf ist er in allen drei exe-Staenden zu sehen.
2. **Spinne unter RE2-KI (Mangel 2):** durch N2 entschieden — 1000 an jedem normalen Gegner, eine Granate toetet sie (N2.5).
3. **Schalter `RE15_KEIN_80PROZENT_GORE=1` (Mangel 3), vollstaendig:**
   * AUS damit (Stand vor Spur GR): das Zerfetzen der Zombie-Familie (RE2-Gehirn `re2z_granate_zerfetzen`, Latch zg_9f 0x08;
     RE1.5-Gehirn `z_granate`, w9/w15) — es laeuft wieder RE2 DEATH[9][1] = 0x80108BEC bzw. der Standard-Tod; der Teile-Wurf
     des Hundes (`re15_re2d_granate_teile`, RE2-Zeile 9 und RE1.5 `case 3`) — RE2 wieder 0x4A-Drift, +0x1D2 >= 3 nur der Kern;
     der w15-Platzer der RE1.5-Spinne (`re15_spinnengore_an`, Schalter der Spur S). Mit dem Zerfetzen entfallen auch alles, was
     nur von dort ausgeloest wird: die liegenden Stuecke (Maske `re2z_part_gr`, Boden-/Wandtest, kein Kaskaden-Ausloeser), die
     Spritzer 2 x 2 und der Rauch-Nachlauf (`re15_esp_granate_rauch_nachlauf`).
   * BLEIBT AN (haengt nicht am Schalter): der Schaden der Handgranate (1000 bzw. die Bosswerte aus N2 — Nutzerentscheid, kein
     80%-Gore); der Vorrang der Explosionskinder vor Blutteilchen (`esp_granate_kind`, re15_esp.c: bei vollem Pool, 96 Plaetze
     `sltiu v0,t3,0x60` @0x8001978c, raeumt ein Blutteilchen fuer die Kinder 0x03195000/0x030B5400/0x030B5800 der Routine 31
     den Platz — dieselbe PORT-WAHL wie fuer den Wurfkoerper, Runde 36 Spur D Punkt 2b); der Mess-Haken `RE15_FORCE_GRANATE`
     (kein Spielverhalten, nur mit Env).
4. **`GR_RAUCH_MAX` 8 und Abstand 1000 (Mangel 3), begruendet — beides GESTALTUNG, keine Original-Werte:**
   * `GR_RAUCH_MAX 8` = Zahl gleichzeitiger Rauch-Nachlaeufe. Die meisten Leichen EINER gemessenen Explosion sind 4 (ROOM1140,
     echter TIEF-Wurf, vier `[gr] ZERFETZT`); 8 ist das Doppelte, damit auch zwei Wuerfe kurz hintereinander jede Stelle bedienen.
     Ein Nachlauf setzt je 8 Takte EIN Kind; bei 8 Nachlaeufen sind das hoechstens 8 Plaetze je Rastertakt von 96 im ESP-Pool
     (@0x8001978c) — der Pool bleibt fuer Blut und Explosion frei. Ist das Feld voll, bekommt eine weitere Stelle nur den Rauch
     der Routine 31 (kein Absturz, kein Speicher-Wachstum).
   * Abstand 1000 = "dieselbe Stelle": zwei Leichen, deren Koerper sich beruehren, liegen bis 2 x 400 = 800 auseinander
     (Trefferradius des Zombies hbdata+6 = 400, re15_damage.c Kegeltest FUN_800127FC); im Video steigt an der Stelle EINES
     Zombies EINE Saeule auf (CD2 #590-#598). 1000 = 800 + Spielraum: ein Haufen nah beieinander liegender Leichen bekommt
     eine Saeule statt einer doppelt so dichten.
   * Die Zufallsmasken des Teile-Wurfs (`rand & 0x3f`, `rand * 2`, `(rand & 0xff) - 128`) sind Streuung der GESTALTUNG ohne
     Original-Vorbild (Video CD2 #577-#592: Teile streuen bis an beide Waende) — nur so benannt, nicht als Beleg verkauft.

### N2.7 Offene Nutzerfragen

* **F1 Birkin "an sich staerker als der Gorilla" gilt fuer die HP nicht:** ohne Granate braucht die 300-Form (3070 Leon, 5080,
  Hangar/Sichel 50A0, Labor 50E0/50F1) **21** Pistolenschuesse, der Gorilla **31** (360 HP = 180 x 2 aus Runde 39; Pistole 15
  gegen 12). HP anheben (Vorschlag: wie beim Gorilla x 2 = 600 -> 41 Schuss, Granate dann 490 fuer 8 Schuss)? Nicht geaendert.
* **F2 Bahnsteig-/Riesenform zu schwach:** 150 HP = **11** Pistolenschuesse ohne Granate. 150 ist ein Port-Mass (die 0x96 der
  Mutation @0x80118094); der RE2-G4, dessen Ablaeufe die Form traegt, hat 1000/900/1100 (Ctor @0x8010067c-b8). Anheben?
* **F3 Elzas Birkin 3071 (Form 1 + Form 2):** 14 Schuss bis zur Mutation + 11 danach = 25 < 31 Gorilla. Mit F1/F2 zu entscheiden.
* **F4 G-Form 0x36 in ROOM3080** (Szene, Nibble 4, waffen-immun `00 00` @0x8006f36c): die Handgranate traegt dort unveraendert
  1000 und wuerde sie toeten (Todesmaschine, Fortschritts-Flag z5:0x1C @0x8011a6b8). Soll eine Granate in der Szene ueberhaupt
  wirken? Nicht angefasst.
* **F5 Gator-Boss 2090:** unveraendert 1000 je Handgranate — eine Granate senkt 25300 auf 24300 < 25000 und loest damit sofort
  die RE2-Schwaeche (+0x218 & 0x20, @0x80101ebc-dc) und die Kanister-Szene aus (mit der Pistole 7 je Schuss: 43 Treffer). So
  lassen?
* **F6 RE1.5-Alligator (HP 300, Granate jetzt 250):** laeuft im Port nur mit `RE15_GATOR_ORIGINAL=1` (Messhilfe; Nutzer-Entscheid
  2026-11-01: der RE1.5-Alligator wird nicht ausgebaut) — der spielbare Alligator ist der Gator-Boss (F5).

---

## N1. NACHBESSERUNG 1 (2026-11-02) — Berichtigung nach Nutzer-Korrektur

> 1. "Das stimmt nicht, bei complete disc video 2 bei 15 Sekunden wirft Leon eine Granate. Und auch die Hunde und andere
>    Gegner die zerfetzen koennen sollen natuerlich entsprechend zerfetzen von einer Granate."
> 2. "Nein, nicht die Spinne ist das was zersprengt wird durch die Granate, sondern die Zombies im Sewer"
> 3. (Nachtrag) "Bei der Granate das es garkeine Koerperteile mehr hinterher gibt die uebrig bleiben ist unwahrscheinlich.
>    Wahrscheinlich wuerden die einzelnen Koerperteile nach der Explosion dann da liegen. Man sieht es nur in dem Video
>    Ausschnitt nicht so richtig."

### N1.0 ⛔ BERICHTIGT — was in der ersten Fassung falsch war

| Stelle erste Fassung | Falsche Aussage | Richtig (Einzelbild, unten belegt) |
|---|---|---|
| §2.1 Ueberschrift + Text, §8 Punkt 2 | "Es gibt im 80%-Material keinen Handgranaten-Wurf" | Es gibt GENAU EINEN: Leon wirft in CD2 16,95-17,9 s eine Handgranate, Detonation 19,253 s am Khaki-Zombie im Kanal (N1.1). Dieselbe Szene ist B 5:20.35-5:24.0. |
| §2.1 Zeile "B 5:18-5:23.2 (Tunnel)", Spur S Z14 | "Schuetze nicht im Bild, Waffe offen" | Handgranate, Werfer Leon (N1.1) |
| Auftrag/Spur S Z14 | "Wiederholung um B 7:06.9" | dort keine Explosion, nur eine Trefferwolke (Beleg video2/x01) |
| §2.2, §5 letzter Punkt, §9.4 | "der Rumpf bleibt", "Rumpf-Platzen waere Erfindung" | Am Ort des Zombies bleibt NICHTS stehen (CD2 ab 19,65 s). Wo die Teile landen, zeigt das Video nicht; Nutzer-Entscheid: die Teile bleiben verstreut liegen (N1.4). |
| Spinne CD2 14,6 s | (Nutzer-Hinweis) | Die Spinne bei CD2 14,65 s platzt nach einem SCHUSS (Leon, Handfeuerwaffe, Muendungsblitze 11,8/13,2/14,4 s), nicht durch eine Granate. |
| Elza-Granatwerfer B 3:30 / 3:36 / 8:05 / 8:42 | — | bestaetigt (Muendungsblitz an der Huefte, Einschlag 0,07-0,33 s spaeter, nie ein Ausholen), Beleg video2/g01-g04 |

### N1.1 Was das Video zeigt (CD2, Bild fuer Bild)

Datei `Version_80%/Resident Evil 1.5 Complete Disc Video 2 (480p Upscale) (480p).mp4`, 640x480, 29,97 B/s; Bild i = i/29,97 s
(exakt per cv2). Werkzeuge `scratchpad/GR2/sheet.py` (Boegen), Messreihe `GR_belege/n1/n1_cd2_messreihe_566_639.txt`.

| Zeit (Bild) | Was | Beleg |
|---|---|---|
| 14,65 s | Spinne platzt nach Schuessen (Leon, blaue Uniform, Maschinenraum mit rotem Rad) — keine Granate | video2/x02 |
| 15,62 s (#468) | Schnitt: Abwasser-Tunnel mit oranger Tuer. Links auf dem Steg 3 Khaki-Zombies, rechts hinten die Spielerfigur (Leon; im Kanallicht gruenlich, B-Ortsliste `video_b.md` §4: "Leon rechts hinten, Zombies in Khaki"). Unten in der Kanalrinne glimmt es: orange-gelbe Pixel (R>180, G>90, B<90) 76 im ersten Bild 15,62 s, flackernd abklingend auf 0 ab 16,42 s (Nachflackern 16,55 s), rote Pixel 792 -> 52 bis 17,35 s — wie ein ausgehender Brand. Es ist schon im ERSTEN Bild der Einstellung da; eine fruehere Explosion ist im Material NICHT zu sehen (davor B 5:14.5-5:19 der Spinnenraum), Ursache offen | nutzer_cd2/cd2_15s5_17s6_kanal.jpg |
| 16,95-17,15 s (#508-514) | Leon hebt einen Arm ueber den Kopf (Ausholen) | n1/n1_cd2_1655_1755_wurf_ausholen.jpg |
| 17,18-17,9 s (#515-536) | Vorlage mit weit nach hinten gestrecktem Bein = Wurf-Nachschwung, ueber den Schnitt 17,58 s hinweg; ab ~18,2 s steht er wieder | n1/n1_cd2_1758_1859_wurf_nachschwung.jpg |
| 17,6-19,2 s | Nahe Kamera: der Khaki-Zombie haengt/kriecht ueber der Stegkante, Oberkoerper auf dem Steg, Beine in der Rinne. Granate im Flug nicht zu erkennen | nutzer_cd2/cd2_17s5_19s2_kanal.jpg |
| **19,253 s (#577)** | **erstes Detonationsbild**: rote Wolke am Zombie, 3-4 khakifarbene Brocken (Glied-Laenge, 30-50 px) steigen ueber den Koerper; #576 (19,219 s) ist der Zombie noch ganz | n1/n1_cd2_576_579_vorher_detonation_raster.jpg |
| 19,29-19,52 s | rote Wolke ueber dem ganzen Koerper und groesser (x 290-560, y 255-470 bei #582), roter Bildanteil 1,7 % (#577) -> 2,6 % (#579/580) -> 0,4 % (#585); rote Brocken fliegen nach vorn links | Messreihe, n1_cd2_1925_1965_* |
| 19,29-19,62 s | weiss-gruenlicher, grob gepixelter Blitz/Qualm im Vordergrund unten; heller Bildanteil 0,4 % -> 7,75 % (#585, 19,52 s) -> 0,2 % (#588) = ~0,3 s | Messreihe |
| 19,32-19,69 s | die Brocken steigen bis an die Tunneldecke (Hoehe der Deckenleuchten, y 85-160), Scheitel 19,55-19,69 s = 9-13 Bilder nach der Detonation; Streuung nach links bis an die linke Wand und nach rechts neben Leon | n1_cd2_1925_1965_*, nutzer_cd2/cd2_19s0_21s0_explosion.jpg |
| 19,686 s (#590) -> 19,953 s (#598) | schwarze Rauchsaeule steigt an der Stelle des Zombies auf (x 230-470, y 120-480), Dunkelanteil der Bildmitte 41 % (Grundwert, Tunnelende) -> 74 % | n1/n1_cd2_1969_2102_rauch_teile_flecken.jpg, Messreihe |
| 19,95-21,09 s (#598-632) | der Rauch steht unveraendert (70-76 %) bis zum Schnitt 21,12 s (#633) — Dauer >= 1,4 s, Ende nicht im Bild | Messreihe |
| ab 19,75 s | dunkle Flecken an linker Wand und Decke (mittlere Luma der linken Wand 56 -> 31-45), bleiben bis zum Schnitt | n1_cd2_592_598_rauch_raster.jpg |
| 19,82-20,02 s | ein Brocken faellt neben Leon herunter und kommt ~20,02 s (#600) auf dem Steg an (23 Bilder nach der Detonation) | n1_cd2_1969_2102_* |
| 20,15 s | rote Spritzer auf beiden Stegkanten | n1_cd2_1969_2102_* |
| 20,55-20,62 s (#616-618) | ein grauer Brocken faellt noch durchs Bild (39-41 Bilder nach der Detonation) | n1_cd2_1969_2102_* |
| 19,65-21,1 s | am Ort des Zombies ist NICHTS mehr — kein Rumpf, keine Beine. Wo die Teile liegen bleiben, zeigt das Video NICHT (Rauch, Bildausschnitt, Schnitt nach 1,8 s) | — |
| Wurf -> Detonation | Ausholen 17,1 s -> Detonation 19,25 s = 2,0-2,2 s | — |

**Welcher Zombie, welcher Raum:** Der Tunnel mit oranger Tuer ist NICHT auf der 40%-Disc (Bildvergleich `video_b.md` §10.3: kein
Treffer 0,42; aehnlich nur der Fan-Raum 202 der Mod, ohne Tuer). Die Zombies der Kanal-Raeume sind auf Disc und Mod
**Typ 0x16** (eigener `Sce_em_set`-Zensus, `scratchpad/GR2/tools/emset.py`): 40%-Disc ROOM2030 3 x 0x16; Mod ROOM2000 2 x 0x16
(liegend, g=84), ROOM2020 4 x 0x16. Typ 0x16 = Port-Zombie-Familie (re15_re2z_owns_type), RE1.5-Modell aus `EMD/CDEMD0.EMS`
Bank 0x16; unter RE2-KI + RE2-Modellen das RE2-EM16. Der Zombie im Video ist khakifarben, kahl, gefleckt, gross — die
Kleidung passt zu "Zombies in Braun = Arbeitskleidung" (BEFUND 5.1, B 4:06-5:12); dass es genau 0x16 ist, folgt aus dem Ort
(Kanal) und dem Zensus, nicht aus dem Bild selbst.

### N1.2 Alle 80%-Videos erneut durchgesehen (Handgranaten und Explosions-Tode)

Durchsicht B 0:00-14:19 lueckenlos mit 2 B/s + Hilfsmessung an allen 25 774 Einzelbildern, CD1-4 mit 3 B/s, Final Build alle
174 Standbilder; jede gemeldete Stelle am Einzelbild bei voller Bildrate. Belege `GR_belege/video2/`. Zeitzuordnung (Helligkeits-
Korrelation): CD2 + 303,6 s = B, CD3 + 197,9 s = B, CD4 + 738,1 s = B.

| ID | Datei, Zeit | Ort | Werfer | Waffe, Merkmal | Ziel | Ausloesen -> Detonation | Ausgang | Beleg |
|---|---|---|---|---|---|---|---|---|
| **W1** | CD2 0:16.75-0:20.5 = B 5:20.35-5:24.0 | Tunnel mit oranger Tuer | Leon | **HANDGRANATE**: Ausholen, Vorlage mit Bein, kein Muendungsblitz | Khaki-Zombie (0x16) ueber der Stegkante | 17,1 -> 19,25 s (2,0-2,2 s) | zerfetzt, am Ort nichts mehr | video2/w01, w02 |
| G1a-c | B 3:30.91-3:34.42 (CD3 13,0-16,5) | 3000 | Elza | Granatwerfer, Muendungslicht an der Huefte | 3 Zombies | 0,07-0,33 s | 1. zerfetzt wahrscheinlich, 2./3. offen | video2/g01 |
| G2 | B 3:35.95 | 3010 | Elza | Granatwerfer | RPD-Zombie | 0,1 s | rote Saeule, weg, Brocken + Haufen in der Lache | video2/g02 |
| G3a-e | B 8:05.47-8:17.4 | 1070 Cut 3 | Elza | Granatwerfer, 5 Schuesse | >= 2 Zombies | 0,07-0,2 s | einer bleibt ganz und richtet sich auf (Z23) | video2/g03 |
| G4 | B 8:42.0 | 1070 Cut 7 | Elza | Granatwerfer, rosa Kugel sichtbar | Zombie in der Lache | 0,13 s | nur roter Fleischhaufen | video2/g04 |
| G5 | B 7:58.07 | 1030 Cut 1 blau | offen (Figur am Rand) | Muendungsblitz | Zombie | <= 0,07 s | Funkenexplosion, faellt; Rest offen | video2/g05 |
| G6 | CD4 25,86 = B 12:43.96 | 4080 | Elza | Granatwerfer wahrscheinlich | **Affenwesen** | 0,27 s | Feuerball, bleibt ganz | video2/o02 |
| O1 | B 7:50.73 | 1030 Cut 11 | offen | Waffe offen | Zombie | — | faellt, orange Reste | video2/o01 |
| O2 | CD4 23,7 = B 12:41.8 | 4080 | offen | Ausbruch am Ruecken | Affenwesen | — | faellt, steht wieder auf | video2/o02 |
| O3 | Final Build A005/A006/A142/A146/A158 | Gaenge | — | Standbilder | offen | — | offen | video2/o03 |

**Nicht-Zombies an einer Explosion:** nur das Affenwesen in 4080 (G6, O2) — es UEBERLEBT beide Treffer und zerfaellt nicht.
Hunde (1190), Kraehe (1170), Spinnen (Maschinenraum, 20B0, 2070), Birkin, Brandzombies: keine Explosion im Material. Damit gibt es
fuer Nicht-Zombies keinen Video-Beleg des Granaten-Zerfetzens; die Quelle fuer Hund/Kraehe/Spinne ist RE2/RE1.5 (N1.3).

### N1.3 Inventur aller Gegnertypen (Code gelesen, Datei:Zeile im Explore-Bericht; HG = Handgranate w9/Art 2, WE = Werfer-Explosivrunde w15/RE2-Zeile 9; "K0/K1" = Direkttreffer/Explosion)

| Typ | Name | HP RE2-KI / RE1.5-KI | Schaden HG / WE (K0 / K1) | stirbt an 1 Granate? | Gore-Quelle (Beleg) | Explosionstod VORHER | NACHHER |
|---|---|---|---|---|---|---|---|
| 0x10/0x12/0x13/0x18 | Zombie | 50..143 / dto. (Import an; ohne Import 0x10 61-101 ...) | 1000 / 200 / 50 | HG ja; WE direkt ja, Explosion nur bei HP < 50 | Video CD2 19,25 s (HG) + B 3:31/3:36/8:42 (WE); RE2 8BEC/9610/92C4; RE1.5 0x80107ee0/0x80108abc | RE2: Kopf/Oberarme/Oberschenkel fliegen, Rest platzt, Rumpf + Becken bleiben; RE1.5: w9 dto., w15 Standardtod | beide KIs, w9 UND w15: 7 Stuecke fliegen bis ~2400-2900 ueber den Boden und BLEIBEN verstreut liegen, am Ort nichts, Rauch >= 1,6 s, Lache |
| 0x11 | dicker Zombie (Brad) | 250 / dto. (ohne Import 71-107) | 1000 / 200 / 50 | HG ja; **WE nein (Rest 50 bzw. 200)** | wie oben | wie oben | wie oben |
| 0x16 | Zombie (Kanal, Video-Typ) | 50..143 / dto. | 1000 / 80 / 50 | HG ja; **WE meist nein (Rest bis 63 / 93)** | wie oben | wie oben | wie oben |
| 0x20 | Hund | 69..132 / 65..111 | RE2 300 / 300 / 50; RE1.5 1000 / 100 | HG ja; WE direkt ja, **Explosion RE2 nein (Rest 19-82)**; RE1.5 WE bei HP < 100 | RE2 Teile-Wurf 0x80104440 (7 Parts @0x80105680) | RE2: 7 Parts driften/platzen (0x4A) und sind nach 29 Bildern weg, Rumpf bleibt; WE-Explosion (1D2 >= 3) nur Schrei; RE1.5: nur Blut (Luft-Tod) | beide KIs, Zeile 9 jeder Spalte: dieselben 7 Parts fliegen (Wurf 0x1062) und BLEIBEN liegen, Rumpf bleibt (RE2), Rauch |
| 0x21 | Kraehe | 10 / 0 | 60 / 60 / 60 (RE1.5 1000/100) | ja | RE2 GIB 0x80102CA0 + Federn 0x80104678/0x8010459C; RE1.5 GIB @0x801149c4 | GIB unter beiden KIs (w9/w15 schon drin) | unveraendert (Teile nach Original-Ablauf) — offen, s. N1.8 |
| 0x25 | Spinne | 99..132 / 76..136 | RE2 60 / 60 / 60; RE1.5 1000 / 100 | **RE2: nein (Rest 39-72)**; RE1.5: HG ja, WE bei HP < 100 | RE1.5 Platzer 0x80113ff8; RE2 FUN_80104890; Video B 5:18.3 (Schuss) | RE2: platzt erst beim Tod; RE1.5: w9 platzt, w15 nur Blut | RE1.5: auch w15 platzt; RE2 unveraendert |
| 0x26 | RE2-Babyspinne / RE1.5-Feueremitter | 1 / — | 60 | Baby ja | keine | Baby-Tod | unveraendert, keine Quelle |
| 0x27 | Gorilla | 360 / 360 | 1000 / 100 | HG ja; **WE nein (Rest 260)** | keine (nur ESP-Blut) | Absturz-Tod | unveraendert, keine Quelle |
| 0x29 | kleiner Affe ("Kakerlake" im alten Test) | 81..121 | 1000 / 100 | HG ja; WE nur HP < 100 | keine; Video 4080 (G6/O2): ueberlebt, zerfaellt nicht | Explosionstod Clip | unveraendert, keine Quelle |
| 0x23 | Alligator | 300 | 1000 / 100 | HG ja; **WE nein (Rest 200)** | keine (Kiefer-Platzen nur Boss) | 0x8010ea30 | unveraendert, keine Quelle |
| 0x23 (2090) | Gator-Boss | 25300 | 1000 / 100 | **nein** | Kiefer platzt nur beim Kanister-Tod | Treffer absorbiert | unveraendert (Boss) |
| 0x2B | Tyrant | 86..126 | 1000 / 100 | HG ja; WE nur HP < 100 | keine | 0x80114cb0 | unveraendert, keine Quelle |
| 0x30/0x36 | Birkin G1/G-Form | 300 | 1000 / 100 | HG ja (ausser Mutation); **WE nein (Rest 200)** | keine | 0x8011a3f0 | unveraendert (Boss) |
| 0x36 (5090) | G5 | 600 | 80 / 80 | **nein (Rest 520)** | keine | — | unveraendert (Boss) |
| 0x1A | Zellenarm | 250 | 1000 / 60 | HG: Tod-Zustand, setzt HP auf 250 zurueck; **WE nein** | RE1.5 "Hand ab" 0x8010D5D0 unerreichbar | nur Blut | unveraendert |
| 0x37, 0x40-0x4D | Tentakel, NPCs | -1 | — | kein Kandidat | — | — | — |
| 0x24, 0x2D, 0x22 | FX-Emitter, Efeu, Stub | — | nicht geprueft | — | — | — | — |

**⛔ Durch Nachbesserung 2 (N2) entschieden: Handgranate = 1000 an jedem normalen Gegner unter jeder KI (auch Spinne/Hund/Kraehe unter RE2-KI), Bosse nach Form. Werfer-Explosivrunde unveraendert.** Stand N1: **Typen, die an EINER Granate nicht sterben (Schaden NICHT geaendert, zur Entscheidung):** Werfer-Explosivrunde an 0x11 (Rest 50
bzw. 200), 0x16 (bis 63 bzw. 93), allen Zombies bei reiner Explosion K1 = 50 (stirbt nur, wer weniger als 50 HP hat), Hund
(RE2, Explosion K1 50: Rest 19-82), Gorilla 0x27, Alligator 0x23, Birkin; **die Spinne unter RE2-KI an der Handgranate UND am
Werfer (60 gegen 99..132)**; G5, Gator-Boss, Zellenarm (setzt HP zurueck).

### N1.4 Umsetzung (GESTALTUNG / PORT-WAHL; jeder Video-Wert mit Datei + Zeit im Code-Kommentar)

**Zombie (RE2-Gehirn `re2z_granate_zerfetzen`, RE1.5-Gehirn `z_granate` -> `re15_re2z_granate_teile_r15`; enemy_ai_re2_zombie.c,
zombiegore_r15.c).** Ausloeser Zeile 9 (w9 Handgranate, w15 Werfer-Explosivrunde) unter beiden KIs, einmal je Tod.
- **Sieben Stuecke fliegen**: Rumpf 0, Becken 1, Kopf 8, Oberarme 9/12 (mit Unterarm + Hand), Oberschenkel 2/5 (mit Unterschenkel +
  Fuss) — RE2-Wurf mit Aufschlag FUN_80028AD8, Flagwort `ori 0x1062` (@0x80108E5C / @0x8010537C-80), Felder wie @0x80108E6C-98.
  Bit 0 von Part 0 bleibt stehen (re15_re2z_gore_active liest es als INIT-Seed). Keine RE1.5-Scherben mehr am Zombie (die
  erste Messung mit Rumpf-Scherben FUN_80108d04 ergab riesige rote Dreiecke bis zur Decke — verworfen).
- **Hoehe/Zeit (PORT-WAHL, Video):** Schwerkraft GR_G = 40 je Bild, Zielhoehe des Scheitels GR_DECKE = 2400 + 0..510 ueber dem
  Boden (CD2 19,55-19,69 s Teile an der Tunneldecke), Steig-Geschwindigkeit sqrt(2 g Steighoehe) (PsyQ SquareRoot0) aus der
  WIRKLICHEN Lage im ersten Wurfbild (Zielhoehe in +0x9A abgelegt, das Drift-Feld liest ein 0x1062-Teil nie), mindestens 600;
  waagrecht 30 + (rand & 0x3f) je Bild (CD2 #577-#592 linke Teile bis an die Wand), Richtungen 4096/7 je Stueck + Zufall.
- **Liegen bleiben (Nutzer-Entscheid):** beim zweiten Aufschlag entfaellt fuer diese Teile der Kaskaden-Ausloeser
  `and v0,v0,-2` @0x80028CA0; st86 = 1 haelt die Physik an (`slti v0,v0,2` @0x80028B60) -> das Stueck liegt bis zum
  Raumwechsel (der Aktor bleibt als Leiche Zustand 7).
- **Boden am Teil + Wandtest** (bisher [PORT (1)]/[PORT (2)] offen, nur fuer diese Teile, Maske re2z_part_gr): Boden =
  RE1.5-Bodensonde FUN_8001c6e8 mit den Argumenten der RE1.5-Brocken-Routine 36 (`0, 8, 0x100` @0x800187dc-f8), Rueckfall
  Aktor-y, wenn die Sonde den eigenen Standboden nicht erklaert; Wand = Original-Zweig @0x80028CAC-D20 (Radius 100,
  `vx = -(vx >> 2)`, `vz = -(vz >> 2)`), RE2-Kollision FUN_8004C1BC -> re15_collision_box_blocked (feste Klasse 4, Band des
  Bodens unter dem Teil).
- Blut wie die erste Fassung (FUN_80109610 P0 Wolken + Fleischbrocken, 0x0800-Brocken + Wolke 8000, Fontaene 0x00021F40,
  Spreng-Russ FUN_8010640C) + Blut 2000 an jedem Teil beim Aufschlag (@0x80028C34-4C) + Spritzer am Boden **2 x 2 Ausloeser**
  (wie der RE1.5-Gore-Aufbau "0x2000 zweimal"; die erste Fassung nahm 2 x 16 = 96 Plaetze = der ganze ESP-Pool).
- **Rauch-Nachlauf (re15_esp.c):** das Original-Rauchkind 0x030B5800 (@0x80018698-a8) erneut an P = Leiche (y - 500), erstmals 13
  Takte nach dem Zerfetzen (CD2 #577 -> #590), dann alle 8 Takte, 6 Mal (Rauch im Video >= 1,4 s; Routine 31 allein zeigt
  ~0,2-0,3 s). Ein Nachlauf je Stelle (naeher als 1000 = derselbe).
- **Explosionskinder vor Blutteilchen:** gemessen (gr.log ROOM2000): die Kinder der Routine 31 im Zuender-2-/0-Bild
  (0x03195000/0x030B5400 @0x80018640/60, 0x030B5800 @0x800186c8) bekamen bei vollem Pool (96, `sltiu 0x60` @0x8001978c)
  keinen Platz (n = 0). Jetzt raeumt — wie fuer den Wurfkoerper (Runde 36 Spur D Punkt 2b) — ein Blutteilchen den Platz.
- RE1.5-Gehirn ohne RE2-Modellblock (RE15_RE15_RE2Z_IMPORT=0, nicht die Vorgabe): Glieder platzen (FUN_80108d04), Wurzel + Rumpf
  weg (Teil-Wort 0 @0x801081d0) — dort keine liegenden Teile (OFFEN, N1.8).

**Hund (enemy_ai_re2_dog.c `re15_re2d_granate_teile`, Aufruf im RE2-Tod-Router und im RE1.5-Tod `case 3`).** Dieselben 7 Parts
des RE2-Teile-Wurfs 0x80104440 (Tabelle @0x80105680, Farbe 0x00101040 @0x801044c8), aber mit dem Wurf 0x1062 statt Drift/
Scherben (0x4A @0x801044a4) — sie fliegen wie die Zombie-Stuecke und bleiben liegen; der Rumpf bleibt wie im RE2-Original. RE2-KI:
Zeile 9 zerfetzt mit 80%-Gore in jeder Spalte (Original bei +0x1D2 >= 3 nur der Kern @0x801046A8-D4). RE1.5-KI: +0x5 = 9 oder 15;
die Part-Bruecke (Spur S) ist fuer den RE1.5-Hund nach dem Wurf aktiv (re15_re2z_gore_active). Rauch-Nachlauf wie beim Zombie.

**Spinne (RE1.5-Gehirn, enemy_ai_common.c Spinnen-Tod):** +0x5 = 15 platzt wie die Handgranate (Tabelle @0x80118f38[9] =
0x80113ff8; Original [15] = 0x8011461c), danach +0x5 = 9 (Muster @0x80114b1c-20).

Alles haengt an `RE15_KEIN_80PROZENT_GORE=1` (aus = Stand vor Spur GR, Schaden bleibt 1000). ⛔ VOLLSTAENDIG in N2.7: der Schalter nimmt Zerfetzen, Teile-Wurf, Liegenbleiben, Rauch-Nachlauf und den w15-Platzer der RE1.5-Spinne weg; Schaden und der Vorrang der Explosionskinder (`esp_granate_kind`) bleiben.

**Mess-Haken (kein Spielverhalten) `RE15_FORCE_GRANATE="<bild>:<slot>[,...]"`** (fx_plattform_pc.c): eine liegende Handgranate
zuendet am Gegner mit der Zuender-Folge der Routine 31 (Licht-Latch, Resolver, Kind 0x03195000, SE; 5 Takte spaeter
0x03195000 + 0x030B5400; 7 Takte spaeter 0x030B5800) — fuer Sichtlaeufe auf engen Kanalstegen, wo ein echter Wurf in der
Wandzelle zuendet (gemessen ROOM2000/2020: `EV wand ... explosion sofort`).

### N1.5 Messung im Port (echte exe, gdigrab am eigenen Fenster, ohne AUTOSHOT/SOFTWARE_RENDER)

Werkzeug `scratchpad/GR2/cap.ps1` (eigene PID, Fenster-Handle), Fluglog `RE15_GR_LOG=1` (`[gr-teil]` je Bild und Teil,
Auswertung `scratchpad/GR2/teil_check.py`).

- **ROOM1140, echter TIEF-Wurf** (Aufstellung integration_r35_granate, RE2-KI, RE1.5-Modelle): vier `[gr]`-Zeilen (0x10, 0x10,
  0x11, 0x11). Ablauf im Bild: Blitz (+0,0 s), Feuerball + Blutwolke (+0,1..+0,4 s), die Stuecke (blaue RPD-Teile) fliegen bis
  an den oberen Bildrand, schwarzer Rauch +0,5..+2,0 s (vorher ~0,3 s), danach liegen die Stuecke mit Blut am Boden, am Ort der
  Zombies steht nichts mehr. Fluglog: alle 28 Stuecke kommen nach 21-32 Bildern am Boden zur Ruhe (st 2 -> 1, y = Boden 0).
- **ROOM2000 (Kanal, Typ 0x16, liegender Zombie wie im Video)**, Kamera Cut 6 (`RE15_FORCE_CUT=6`), `RE15_FORCE_GRANATE=100:4`:
  zwei `[gr] ... Typ 0x16 ... ZERFETZT`, die Stuecke fliegen, Blutwolke; gr.log: Explosionskinder jetzt alle mit Platz
  (vorher n=0), Rauch-Nachlauf 6 Kinder je Leiche. ⛔ BERICHTIGT (Nachbesserung 2, Abnahme 1 §3): ~~In ROOM2000 werden die
  RE1.5-Explosionseffekte NICHT gezeichnet~~ — sie WERDEN gezeichnet. Beim echten Wurf in ROOM2000 zeigen 8492d3594,
  2f886e91a und die master-exe denselben Feuerball (weiss -> orange) und Rauch ~0,6 s. In diesem Haken-Lauf lag die
  Explosionsmitte nur ausserhalb des erzwungenen Bildausschnitts: Kind `id=3 sub=25` besteht den Regionstest und laeuft
  durch den Zeichner, projiziert aber auf sx=2 sy=341 bei 320x240 (unten links ausserhalb; Cut 6 sieht den liegenden
  Zombie von ganz nah, P.y = Spieler-y - 500 liegt unter der Bildkante). Gleiches Bild mit RE15_KEIN_80PROZENT_GORE=1.
  Beleg `GR_belege/abnahme1/a1_kanal2000_*` und `a1_kanal2000_fxlog_projektion.txt`.
- **ROOM1230, Hund (RE2-KI)**, Cut 6, `RE15_FORCE_GRANATE=45:1`: `[gr] Hund Slot 1 Waffe 9: ZERFETZT (Teile-Maske 0x079c)`;
  Blitz, Feuerball, ein Bein fliegt hoch, Rauch +0,5..+1,7 s, danach rote Teile und Lache am Boden, nach 12 s unveraendert.
  Fluglog: 7 Parts, Scheitel 2349..2672 ueber dem Boden, Ruhe nach 27-30 Bildern. Bild
  `GR_belege/n1/n1_port_hund_1230_explosivgranate.jpg`.

**Vergleichsbilder (Bildleiste Video oben / Port unten, Zeit relativ zur Detonation, gdigrab 30 B/s am echten Fenster):**
- `GR_belege/n1/n1_vergleich_video_cd2_gegen_port_1140.jpg` — ROOM1140, echter Wurf. Gleich: Blutwolke ueber den Koerpern
  ab +0,03 s, Stuecke steigen bis zum oberen Bildrand +0,1..+0,43 s, schwarzer Rauch ab +0,43 s bis ueber +1,77 s, am Ort kein
  Koerper mehr. Anders (benannt): der Blitz ist im Port die RE1.5-Explosion (weiss -> ORANGE Feuerball, Original-Kind
  0x03195000 @0x800185dc), im Video weiss-gruenlicher Qualm ohne Orange; die Stuecke sind im Port blaue RPD-Teile (Typ
  0x10/0x11), im Video khaki (0x16); die dunklen Wandflecken fehlen (N1.8). Letzte Kachel +8 s: die Stuecke liegen.
- `GR_belege/n1/n1_port_1140_vorher_nachher_liegen.jpg` — vorher vier Zombies, +11 s nur noch verstreute Stuecke in Blut.
- `GR_belege/n1/n1_vergleich_video_cd2_gegen_port_2000_kanal_0x16.jpg` — derselbe Typ 0x16 wie im Video, liegend, im Kanal.
  Das RE1.5-Modell 0x16 der Mod ist weiss-grau (nicht khaki). Die Stuecke steigen ueber den Bildausschnitt der hohen Kamera und
  landen im dunklen Kanal. ⛔ BERICHTIGT (Nachbesserung 2): ~~Explosions- und Rauch-Sprites fehlen in diesem Raum~~ — der
  Feuerball liegt in diesem Haken-Lauf (Cut 6, `RE15_FORCE_GRANATE=100:4`) ausserhalb des Bildausschnitts (Mitte sx=2 sy=341
  bei 320x240, Abnahme 1 §3); beim echten Wurf in ROOM2000 ist er zu sehen (`GR_belege/abnahme1/a1_kanal2000_echterwurf_neu_vorher_master.jpg`).
- `GR_belege/n1/n1_port_hund_1230_explosivgranate.jpg` — Hund RE2-KI (kein Video-Vorbild).
- `GR_belege/n1/n1_port_hund_1230_re15ki.jpg` — Hund RE1.5-KI (`RE15_AI_FLAVOR=re15`): `[gr] Hund Slot 1 Waffe 9: ZERFETZT`,
  Fluglog 7 Parts, Scheitel 2229..2690, Ruhe nach 25-28 Bildern; Explosion, Rauch, danach liegen die Teile in Blut (+7,8 s).
- Pruefpunkte aus dem Auftrag: kein heiler Koerper bleibt stehen (alle drei Laeufe), nichts haengt in der Luft (Fluglog: jedes
  der 7 Parts je Gegner endet mit st 2 -> 1 auf y = Boden; Unit [2]/[3]/[5]), keine Teile durch Waende (Wandtest; im Bild keine
  Teile in Wand-/Moebel-Flaechen), kein Flackern (Folgebilder 30 B/s ohne Springen; die gezeichneten Stuecke bleiben ueber 11 s
  unveraendert).

### N1.6 Tests

- `unit_r41_granate` neu: [2] RE2-Gehirn, sechs Typen (0x10/0x11/0x16 aus ROOM1140, umgetypt 0x12/0x13/0x18): Zerfetzung
  einmal, die sieben Stuecke tragen 0x1062 + Liegen-Maske 0x1327, Kinder bleiben dran, keine Scherben, Sturz 0x80108530,
  Leiche, Zeichengang 150 Bilder: alle Stuecke liegen am Boden, Scheitel gegen die Zielhoehe (max(Ziel, Start + 600)
  -350..+50, mindestens 1800), Ruhe <= 45 Bilder, >= 5/7 gestreut, alle 15 Teile weiter gezeichnet, Rauch-Nachlauf 6;
  [3] RE1.5-Gehirn dieselben Stuecke, liegen, Leiche, kein Kriecher; [5] Hund RE2-KI und RE1.5-KI (ROOM11D0): 7 Parts
  0x1062 + Farbe 0x101040, Maske 0x079C, Bruecke aktiv, liegen. [1]/[4] unveraendert (Schaden, Schalter aus).
- `unit_zombie_gore` Granate: w9 UND w15 (RE1.5-Gehirn), RE2-Block bzw. Teilschicht je nach Import-Option.
- `unit_r34_reaktion` 121/122/152/220/221 schalterabhaengig; NEU `unit_r34_reaktion_ohne80` (RE15_KEIN_80PROZENT_GORE=1)
  pinnt das RE2-Original (Hund Zeile 9: 0x4A-Drift, 1D2 >= 3 nur Kern; Zombie 8BEC/9610, Spalte 0 Kriecher 0x80107438).
- Gelaufen (Stand f95dd987a): volle Unit-/Prop-/Probe-Suite `ctest -E ^integration_ -j 6` **1077 Tests, 1074 im ersten Lauf
  gruen**; die drei uebrigen (unit_r39_titelversion_cmake "git fehlt", unit_r38_releaseskript_fehler_umfeld Timeout 1200 s,
  unit_r35_android_bash_mutanten) lagen an meinem Mess-PATH ohne git bzw. an der Last und sind im Wiederholungslauf mit git im
  PATH **3/3 gruen** -> **1077/1077**. Granaten-Integration integration_r34_granaten / integration_r35_granate /
  integration_r36_granate **3/3 gruen** (359 / 212 / 186 s).

### N1.7 Fuer den Nutzer (Kurzfassung N1)

1. **Video:** Ja — CD2 16,95-17,9 s holt Leon aus und wirft, Detonation 19,253 s am Khaki-Zombie, der ueber der Stegkante
   liegt (Kanal, Typ 0x16). Rote Wolke, khakifarbene Teile fliegen bis an die Tunneldecke (Scheitel nach 0,3-0,4 s, ein Teil
   landet nach 0,77 s, ein letztes faellt noch nach 1,3 s), weiss-gruenlicher Blitz 0,3 s, schwarze Rauchsaeule ab 0,43 s bis
   zum Schnitt (>= 1,4 s), dunkle Flecken an der Wand, rote Spritzer auf den Stegen. Am Ort des Zombies bleibt nichts stehen; wo
   die Teile landen, zeigt das Video nicht. Die Spinne bei 14,6 s platzt nach Schuessen. Es ist der einzige Handgranatenwurf im
   ganzen 80%-Material; die anderen Explosionen sind Elzas Granatwerfer.
2. **Port jetzt:** Explosivgranate (Handgranate UND Werfer-Explosivrunde, RE2-KI UND RE1.5-KI) — der Zombie zerfliegt in
   sieben Stuecke (Rumpf, Becken, Kopf, beide Arme, beide Beine), die bis ~2,4-2,9 m hoch fliegen und dann verstreut am Boden in
   Blut LIEGEN BLEIBEN (bis zum Raumwechsel); schwarzer Rauch steht jetzt ~1,6 s; Teile prallen an Waenden ab und fallen auf den
   Boden unter sich (auch in den Kanal). Der Hund wirft bei Granaten-Tod seine 7 RE2-Teile, die ebenfalls liegen bleiben. Die
   RE1.5-Spinne platzt auch am Werfer.
3. **Nicht geaendert (zur Entscheidung):** Wer an EINER Granate nicht stirbt (N1.3): Werfer-Explosivrunde an dickem Zombie
   0x11 und 0x16, reine Werfer-Explosion (50) an allen Zombies mit mehr als 50 HP, RE2-Hund an der Werfer-Explosion, die
   Spinne unter RE2-KI (60 Schaden gegen 99-132 HP, Handgranate und Werfer), Gorilla, Alligator, Birkin, G5, Gator-Boss,
   Zellenarm.

### N1.8 Offen

1. Dunkle Flecken an Wand/Decke (CD2 19,75-21,1 s): der Port hat keine Wand-Abziehbilder (Spur S Z28) — nicht gebaut.
2. Der Blitz: Port = RE1.5-Explosion weiss -> orange (Original-Kind der Disc), Video weiss-gruenlich ohne Orange — nicht
   angepasst (Original-Effekt; die 80%-Fassung hat offenbar einen anderen Explosions-Effekt).
3. ⛔ BERICHTIGT (Nachbesserung 2, Abnahme 1 §3) — KEIN Fehler: die Explosions- und Rauch-Sprites werden in ROOM2000 gezeichnet
   (echter Wurf, drei exe-Staende gleich). Im Haken-Lauf mit Cut 6 lag die Explosionsmitte ausserhalb des Bildes (sx=2 sy=341
   bei 320x240). Die fruehere Aussage "nicht gezeichnet, vermutlich Textur/Bank" war falsch.
4. RE1.5-Gehirn OHNE RE2-Modellblock (RE15_RE15_RE2Z_IMPORT=0): Glieder platzen in Scherben, keine liegenden Teile (die RE1.5-
   Flugphysik FUN_8001f024 laesst ein ruhendes Teil bei y == 0 verschwinden, @0x8001f104-134).
5. Kraehe: der GIB laeuft wie bisher (RE2 0x80102CA0 / RE1.5 @0x801149c4), ihre Teile verschwinden nach dem Original-Ablauf —
   nicht auf "liegen bleiben" umgestellt (Federn/Fetzen; Entscheidung des Nutzers offen).
6. ~~Spinne unter RE2-KI stirbt an keiner Granate~~ — erledigt in Nachbesserung 2 (N2): 1000 an jedem normalen Gegner.
7. Das rote/orange Glimmen in der Kanalrinne CD2 15,6-17,4 s: Ursache vor dem Schnitt, nicht im Material.
8. Elza: figurunabhaengig (kein Figurbezug im Pfad, wie §6.4) — kein eigener Sichtlauf in N1.

---

# ERSTE FASSUNG (Stand 1cfbcb074) — Videobefunde in §2.1/§2.2, §5 "Rumpf bleibt", §8.2, §9.4 sind durch N1 BERICHTIGT

## 0. Auftrag (Nutzer, woertlich, 2026-11-02)

> Und die Granate sollte laut den Videos der 80% Version die Zombies zerfetzen, also die normale, richtige
> Handgranate. Ausserdem ist sie noch zu schwach, die sollte auch die dicken Zombies zerfetzen

Gore-Regel (bindend): Gore = Maximum aus RE1.5-Disc, RE2-Retail und 80%-Videos. Gestaltung = PORT-WAHL,
jeder Einzelmechanismus mit @0x-Beleg.

## 1. Welches Item

* "normale, richtige Handgranate" = **Item 0x09 Hand Grenade** (Namensleser FUN_80028840, Runde 30;
  BAUPLAN Runde 34 E1). Die anderen beiden Granaten sind 0x0A Acid / 0x0B Incendiary. Der Granatwerfer ist
  die Waffe 0x0F (RE2-Explosivrunde, `SOUND/ARMS0F`).
* Explosion: ESP-Routine 31 @0x8001854c, Zuender 7 @0x8001856c, Resolver `FUN_80012d60(500, &P, 2)`
  (`ori a0,zero,0x1f4` @0x80018598, `addiu v0,v0,-500` @0x800185a8, a2 = 2 @0x800185b4).
  Art 2 -> Schaden DAT_8006f418[2] = 1000 (@0x8006f41c), Waffen-Id DAT_8006f430[2] = 9 (@0x8006f432).
* Port: Reichweite/Kandidaten RE2 Op 47 (`granate_r35.c re15_granate_r35_explosion`), Anwendung RE1.5-Gegnerzweig
  (`re15_resolver_gegnerzweig`, re15_damage.c), Stempel `re2_gl_explosion_stempel` (Zombie-Familie: Zeile 9,
  K0, Zone 1 = Spalte 1 -> DEATH[9][1] = 0x80108BEC Zerreissen, Runde 35 Spur A §3.5).

## 2. Video-Befund (80%-Material)

Material: B = `Version_80%/Resident Evil 1.5 (All Footage).mp4` (320x240, 30 B/s), A = `Version_80%/Biohazard 1.5
Final Build.mp4` (Diashow), CD1-4 = `documentation/Biohazard 1.5 Complete Disc Video N of 4 (360p).mp4`
(laut Spur S §1 ausser Vorspann/Abspann vollstaendig in B enthalten). Ausgangspunkt: die Gore-Sichtung der
Spur S (`S_video_gore.md` §2.1, Z08-Z11, Z14, Z21-Z24), hier jede Explosionsstelle neu am Einzelbild auf die
WAFFE geprueft (Werkzeug `scratchpad/GR/strip.py`, 5-30 B/s, Ausschnitte 2-2,5-fach).

### 2.1 ⛔ BERICHTIGT (N1.0): ~~Es gibt im 80%-Material keinen Handgranaten-Wurf~~ — es gibt einen (CD2 / B 5:20, Leon). Die Elza-Stellen sind Granatwerfer (bestaetigt)

| Stelle | Was man sieht | Waffe | Beleg |
|---|---|---|---|
| B 3:30.8-3:31.8 (3000 Cut 2) | Elza steht hinten, Waffe an der Huefte; **Muendungsblitz 3:30.9-3:31.0** an ihrem Rumpf; 0,3 s spaeter (3:31.3-3:31.4) platzt der Vordergrund-Zombie (weisses Hemd) zur roten Wolke mit Feuer, ab 3:31.6 ist er weg, am Boden Brocken/Blut | Granatwerfer (Schuss, kein Wurf, kein Bogen) | `GR_belege/video/v01_B0330_elza_muendungsblitz_dann_zombie_zerplatzt_300.jpg` |
| B 3:35.5-3:37.2 (3010 Draufsicht) | 3:35.5 Elza mit langer Waffe im Anschlag; 3:36.0 Blitz bei Elza, der RPD-Zombie vor ihr wird getroffen; 3:36.1-3:36.2 rote Saeule/Wolke ueber dem ganzen Koerper; ab 3:36.3 **ist der Zombie weg**, es bleiben eine grosse rote Lache und ein grauer Brocken | Granatwerfer | `v02_B0335_3010_elza_schuss_zombie_weg_lache.jpg` |
| B 8:05.2-8:08.0 (1070 Cut 3) | Muendungsblitz an Elzas Huefte 8:05.4 und 8:06.6, jeweils 0,2 s spaeter Explosion am Zombie vor der Kamera | Granatwerfer | `v03_B0805_1070_elza_muendungsblitz_explosion.jpg` |
| B 8:40.5-8:44 (1070 Cut 7 Draufsicht) | Elza zielt waagrecht mit langer Waffe, weisser Blitz an der Muendung 8:42.1; Explosion am Zombie in der Lache 8:42.1-8:42.3; danach **nur noch ein roter Fleischhaufen in einer grossen runden Lache** | Granatwerfer | `v04_B0840_1070_elza_schuss_haufen_in_lache.jpg` |
| B 5:18-5:23.2 (Tunnel, nicht auf der Disc) | Der Khaki-Zombie haengt ueber der Bruestung, 5:23.0 rote Wolke + weisse Wolke + helle Brocken, danach ist er weg | Schuetze nicht im Bild, Waffe offen | `v05_B0518_tunnel_zombie_zerfetzt_schuetze_unsichtbar.jpg`, Spur S g08 |
| B 7:50.8-7:51.6 (1030 blau) | orange Funken/Brocken am Zombie | offen (kein Schuetze im Bild) | Spur S g30 |
| A 0:00-2:07 | Diashow-Standbilder, keine Wurfpose, keine Granate in der Hand | – | `scratchpad/GR/a_*.jpg` (nicht im Repo) |

Kennzeichen eines Handgranaten-Wurfs (Port/RE1.5: Wurfclip W09 Clip 7/9/11, 35 Bilder, Flug mit Taumel-Sprite,
Abpraller, Explosion erst 36 Bilder nach dem Liegen = >= 1,2 s nach dem Aufschlag; BAUPLAN Runde 34 §1.1) kommt
an keiner Stelle vor. Alle Explosionen mit sichtbarem Schuetzen haben Muendungsblitz an der Huefte und
Einschlag 0,2-0,5 s spaeter ohne Bogen = Granatwerfer.

**Folgerung:** Das Video belegt, was eine **Explosivgranate** mit einem Zombie macht (Koerper verschwindet in
roter Saeule/Wolke, Lache + Brocken/Fleischhaufen bleiben). Dass die **Handgranate** dasselbe tut, ist die
Uebertragung nach der Gore-Regel des Nutzers (selbe Sprengladung, Gore = Maximum) — **PORT-WAHL**, nicht Video.
Ein dicker Zombie (Typ 0x11/EM11) wird im Video an keiner Explosionsstelle getroffen (3000/3010/1070: RPD- und
Hemd-Zombies). "Auch die dicken zerfetzen" ist damit Nutzer-Vorgabe, kein Videobefund.

### 2.2 Was das Video beim Zerfetzen zeigt (Soll-Bild)

1. Einschlag: rote Wolke/Saeule ueber dem GANZEN Koerper (3:31.4, 3:36.1-2, 8:42.3), mit Feuer/oranger Wolke.
2. Der Koerper ist danach weg (3:31.6, 3:36.3, 5:23.2) bzw. nur noch ein roter Haufen (8:42.5ff).
3. Es bleiben: grosse rote Lache (3:36.3ff, 8:42.5ff), ein grauer Brocken (3:36.4), Blut/Reste am Boden (3:31.8).
4. Einzelne fliegende Teile sind bei 320x240 als helle Brocken zu sehen (5:23.0-5:23.2), Gliedmassen als
   solche nicht unterscheidbar.

## 3. Ist-Stand VORHER (gemessen, echte exe, Messkopie `re15_pc_r41GR_vorher.exe` = Basis 2f886e91a)

Lauf `v1_leon_vorher` (Werkzeug `scratchpad/GR/cap.ps1`: eigene PID, gdigrab am Fenster-Handle, beschleunigter Renderer
direct3d, ohne AUTOSHOT/SOFTWARE_RENDER, 10 Bilder/s, `RE15_WINDOW_SCALE=2`). Echter Titel + echte Spielerwahl
(`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`), dann `RE15_DEBUG_JUMP=1140@250`, Leon (-1676,-18070) Blick 1076,
Standard-KI (`RE15_AI_FLAVOR=re2`), Modelle wie in der Nutzer-Sitzung (Hybrid, `RE15_AI_MODELS=re15`, ZA-Dossier §6),
Handgranate (`RE15_GIVE=9:5`, `RE15_EQUIP=9`), TIEF-Wurf `MD0.6,MDA0.2,MD2.5,W5` (Aufstellung integration_r35_granate).

`state.log` (Bild nach dem Sprung), Explosion F119 (gr.log T=365, `resolver art=2 ... r=2000 eingriffe=4`):

| Slot | Typ | HP vor | HP nach | Zustand | Ausgang |
|---|---|---|---|---|---|
| 2 | 0x10 | 50 | -150 | 3 (Tod) | 0x80108BEC Zerreissen bzw. 0x80109610 Wegschleudern, Leiche F180 |
| 3 | 0x10 | 80 | -120 | 3 (Tod) | dito, Leiche bis F400 |
| 4 | **0x11 dick** | 250 | **50** | **2 (HURT)** | **ueberlebt**, F400 noch HP 50, greift weiter an |
| 5 | **0x11 dick** | 250 | **50** | **2 (HURT)** | **ueberlebt** (stirbt spaeter an anderem) |

Schaden = 200 = RE2-Granatwerfer-Zeile 9 Klammer 0 (`0x00a0c8c8` @0x800A41CC), seit Runde 34 E4 als Ersatz fuer die
RE1.5-Zahl. Brad/dicker Zombie 0x11 hat RE2-HP 250 (`addiu v1,zero,250` @0x801008C8) -> ueberlebt jeden Wurf mit 50.
Typ 0x16 bekommt sogar nur 80 (@0x800A4348) bei HP bis 143 -> ueberlebt meist ebenfalls.
Bild: `GR_belege/port/p01_leon_1140_wurf_vorher_nachher.jpg` (obere Zeile) — die zwei dicken Zombies (weisse Hemden) stehen nach der Explosion
weiter und gehen auf Leon los; die zwei normalen fallen (Sturz bzw. Zerreissen, Rumpf ganz).

## 4. Staerke — Herleitung (PORT-WAHL)

Quellen (alle selbst gelesen):
* RE1.5-Handgranate: `FUN_80012d60(500, &P, 2)` (`ori a0,zero,0x1f4` @0x80018598, `ori a2,zero,0x2` @0x800185b4),
  Art 2 -> `DAT_8006f418[2] = 1000` (`e8 03` @0x8006f41c). Gleich in `info/Re1.5/PSX.EXE` und `Version_40%/PSX.EXE`
  (eigener Vergleich: Tabelle `10,20,1000,1000,1000,50,100,200,300,1000,0`, Reaktionstabelle @0x8006f430 `3,3,9,10,...`).
* RE2: keine Handgranate (BAUPLAN Runde 34 E1, Musterscan). Die Zeile 9 ist die Granatwerfer-Explosivrunde
  (Zombie K0 200 / K1 50 / K2 10 @0x800A41CC; 0x16: 80 @0x800A4348). Staerkster RE2-Explosivwert am Zombie: Rakete
  Zeile 17 = 900 (`0x384E1384` @0x800A426C).
* HP: RE2-Zombie max 128 (@0x8010C670) + 15 (`sltiu ..,0x4` @0x801007FC / @0x80100810) = 143; **dicker Zombie 0x11 =
  Brad 250** (@0x801008C8). RE1.5-HP (`0x8011f034 + Typ*0x20`, STAGE1.BIN, Disc = Mod): 0x10 max 101, **0x11 max 107**,
  0x12 max 111, 0x13 max 88, 0x16 max 99, **0x18 einmal 1058** (Index 3 @0x8011f33a, sonst max 109).
* Video: ein Treffer = Zombie zerfetzt (Abschnitt 2).

**PORT-WAHL:** Die Handgranate (Art 2) traegt an der Zombie-Familie (re15_re2z_owns_type 0x10/0x11/0x12/0x13/0x16/0x18)
wieder ihre eigene Zahl **1000** (@0x8006f41c) statt der RE2-Granatwerfer-Zeile. Begruendung: Die Zeile 9 war eine
Zuordnung (RE2 kennt keine Handgranate), die 1000 ist der Wert des Gegenstands im Original; sie liegt ueber jeder
RE2-HP (max 250) und jeder RE1.5-HP ausser dem Einzelwert 1058 von 0x18 (nur ohne RE2-Modell, d. h.
`RE15_RE15_RE2Z_IMPORT=0` + RE1.5-KI; dort ueberlebt dieser eine Zombie mit 58, wie im Original). Die Regel
"HP und Schaden nur zusammen" (E4) wird damit fuer genau diesen Fall bewusst durchbrochen (Nutzer-Auftrag "zu schwach").
Nicht geaendert: Saeure/Brand (Art 3/4, E4 = 200), Werfer-Arten 6..9, alle Nicht-Zombies (Hund 300, Kraehe 60,
Spinne 60, G5 80 nach E16, Gorilla/Kakerlake/Alligator/Birkin 1000 RE1.5, Gator-Boss Modul).
Code: `re15_damage.c re15_enemy_take_damage_at` (Block "RUNDE 41 SPUR GR").

## 5. Zerfetzen — Umsetzung (GESTALTUNG / PORT-WAHL, Einzelmechanismen mit @0x)

Keine der drei Quellen nimmt dem Zombie alles auf einmal (RE2 Zeile 9 DEATH[9][1..2] = 0x80108BEC: Kopf + Arm(e),
Rumpf bleibt; RE1.5 0x80107ee0 ZERFETZEN: Kopf + Arme; RE1.5 0x80108abc BEINE WEG: Beine). Das Video zeigt aber den
ganzen Koerper weg und Reste in der Lache. Gebaut ist die Vereinigung, je Teil ein belegter Mechanismus (Code-Block
"RUNDE 41 SPUR GR" ueber `re2z_granate_zerfetzen`, enemy_ai_re2_zombie.c; RE2-Slots nach den Parser-Bindlaengen):

| Teil (RE2-Slot) | Was | Mechanismus / Beleg |
|---|---|---|
| Kopf 8 | fliegt weg, Halsfontaene | Drift 0x4A, Felder 0x80108BEC @0x80108DD4-E3C, Effekt 3000 @0x80108E18; Fontaene 0x00021F40 @0x80109428-38 |
| Oberarme 9/12 | fliegen (beide) | Wurf 0x1062, Felder @0x80108E54-E98 / @0x80108F10-F70 (RE2 Arm B nur 50 % @0x80108F04-08 -> hier immer) |
| Unterarme 10/13 | platzen in Scherben | RE1.5 FUN_80108d04 (@0x80108d04-6c: Blut 0x2000, +0x68 = 0x8f, Wort |= 0x4a), wie ZERFETZEN @0x8010818c/@0x80108224 |
| Haende 11/14 | weg | RE1.5 Teilwort 0 @0x801081d0 / @0x8010826c |
| Oberschenkel 2/5 | fliegen | Wurf 0x1062 mit den RE2-Beinwurf-Feldern @0x80105384-C0, Effekt 7000 @0x801053CC |
| Unterschenkel/Fuesse 3/4/6/7 | platzen | RE1.5 BEINE WEG @0x80108ba0 / @0x80108bb8 / @0x80108bd0 / @0x80108be8 (RE1.5-Teile 2/3/5/6 -> RE2 3/4/6/7, re2_hybrid_perm) |
| Rumpf 0 + Becken 1 | bleiben liegen | Spreng-Russ FUN_8010640C (Zeile 9, @0x80108CEC; Flammen weiter aus, Runde 36 D), Stumpf-Marker Part 0 @0x80108F98-B4, Rumpf-Stumpf Record 16 @0x80108FB8-FDC (nur RE2-MD1) |
| Blut/Brocken | Wolken, Fleischbrocken, Lache | FUN_80109610 P0 (8096/6096, n = (rand&3)+1 Brocken 0x0902xxxx @0x801096E8-0x801097B8), Brocken 0x0800xxxx + Wolke 8000 @0x80108FF0-0x80109054, grosse Lache (2 x 16 Spritzer, wie der fruehere Zeile-9-Zusatz) |
| danach | Sturz zur Leiche | 0x80108530 (`re2z_death_main`, wie re2z_80_zerfetzen) -> Zustand 7 |

* Ausloeser: DEATH-Wurzel mit Zeile 9 (Handgranate w9 und Granatwerfer-Explosivrunde w15, `re2z_row_from_weapon`), in
  jedem Zweig (stehend, liegend, Kriecher), genau einmal (Latch `zg_9f` Bit 0x08). Der Kriecher-Wiederbeleber
  0x80107438 (Spalte 0) wird damit fuer Zeile 9 nicht mehr erreicht — die Handgranate stempelt ohnehin Spalte 1.
* Farbwort der platzenden Teile: RE1.5 +0x68 = 0x8f; bei aktiver RE2-Bruecke nimmt der Zeichner das RE2-Farbwort
  +0x70 (main.c `gore_on`), deshalb steht dort derselbe Wert (Port-Mapping, beide multiplizieren das NCCT-Ergebnis).
* RE1.5-Gehirn (Flavor re15, `zombiegore_r15.c z_granate`): w9 -> ZERFETZEN (@0x8010800c-0x8010826c, ohne den
  Muenzwurf ZERFETZEN/OBERKOERPER @0x801080c8-d4) + BEINE WEG (@0x80108ba0-c50), Leiche statt Kriecher. Saeure/Brand
  (w10/w11) behalten die fruehere Gestaltung (Oberkoerper ab, kriecht weiter) — nicht Teil dieses Auftrags.
* Schalter wie alle 80%-Gestaltungen: `RE15_KEIN_80PROZENT_GORE=1` -> RE2 0x80108BEC wie vorher. Die Staerke (1000)
  haengt NICHT am Schalter.
* Nicht gebaut / nicht erfunden: das Verschwinden des Rumpfes (keine Quelle hat es; der Rumpf bleibt als Brocken in der
  Lache — Video 8:42.5 zeigt genau so einen Haufen). Die fliegenden RE2-Teile verschwinden nach dem zweiten
  Bodenkontakt (Kaskade @0x80028CA0), die RE1.5-Scherben nach 0x1d Bildern (@0x8001f208-214) — beides Original.

## 6. Messung NACHHER (echte exe, Messkopie `re15_pc_r41GR_nachher.exe` = Stand dieses Commits)

### 6.1 Leon, ROOM1140, echter TIEF-Wurf (gleiche Aufstellung wie §3, Lauf `v1_leon_nachher`)

`state.log` nach dem Sprung, Explosion F119 (gr.log `resolver art=2 ... eingriffe=4`), `debug.log` (RE15_GR_LOG=1):

| Slot | Typ | HP vor -> nach | F120 | F180 | debug.log |
|---|---|---|---|---|---|
| 2 | 0x10 | 50 -> -950 | Zustand 3, Sturz (mo 2) | 7 Leiche | `[gr] RE2 Slot 2 Typ 0x10 Zeile 9: ZERFETZT (HP -950)` |
| 3 | 0x10 | 80 -> -920 | 3, Sturz (mo 1) | 7 | `... Slot 3 Typ 0x10 ... ZERFETZT (HP -920)` |
| 4 | **0x11 dick** | **250 -> -750** | 3, Sturz | **7 Leiche** | `... Slot 4 Typ 0x11 ... ZERFETZT (HP -750)` |
| 5 | **0x11 dick** | **250 -> -750** | 3, Sturz | **7 Leiche** | `... Slot 5 Typ 0x11 ... ZERFETZT (HP -750)` |

Slot 1 (0x16, liegt seit Raumstart in seiner Lache, g=88) liegt ausserhalb von Band/Tor und bleibt unberuehrt —
wie vorher. Bild (gdigrab, eigene PID): `GR_belege/port/p01_leon_1140_wurf_vorher_nachher.jpg` untere Zeile —
Explosion, rote Scherben fliegen strahlenfoermig, Arme/Beine/Kopf fliegen weg, ab ~1 s liegen nur noch Rumpfe in
grossen Lachen; die dicken Zombies stehen nicht mehr auf.

### 6.2 Explosion direkt am dicken Zombie (Messhaken `RE15_FORCE_EXPLOSION=2@100:4`, Slot 4 = 0x11)

`[harness] RE15_FORCE_EXPLOSION F100 art=2 slot=4 t=11 P=(-48,-500,-20878) Treffer=4`.
Vorher (`v2_dick_vorher`): beide 0x11 bleiben stehen und packen Leon (Bilder #303-#324). Nachher (`v2_dick_nachher`):
alle vier zerfetzt (vier `[gr]`-Zeilen, HP -950/-920/-750/-750). Bild: `GR_belege/port/p02_leon_1140_dicke_zombies_vorher_nachher.jpg`.

### 6.3 Video gegen Port

`GR_belege/vergleich_video_80_gegen_port_nachher.jpg`: oben 80%-Video B 3:35.9-3:36.8 (3010) und 8:42.1-8:43.5 (1070),
unten Port nachher. Gleich: Einschlag als rote Wolke ueber dem ganzen Koerper, der Koerper ist danach nicht mehr als
Figur da, es bleibt ein Rest in einer grossen Lache. Anders (benannt): im Port fliegen sichtbare Glieder und rote
Scherben (RE2-Wurf + RE1.5-Scherben), das Video zeigt bei 320x240 nur Wolke und Brocken; der Rumpf bleibt im Port als
erkennbarer Torso liegen (RE2-Stumpf-Mesh 16 nur mit RE2-Modellen; mit den RE1.5-Modellen der Nutzer-Sitzung der
Original-Torso, Grenze (1) im Code-Kopf von 0x80108BEC).

### 6.4 Elza

* Gleicher Weg wie Leon: Waffen-Id 9 und Art 2, kein Figurenbezug im Schadens- und Todespfad (re15_damage.c,
  enemy_ai_re2_zombie.c, zombiegore_r15.c lesen keine Figur); die Wurfbank PL04W09 hat dieselben Cliplaengen
  (Runde 36 D §6.8).
* Gemessen (echte exe, echte Spielerwahl Elza `RE15_PSELECT_AUTO_SWITCH`, `RE15_DEBUG_JUMP=1041@250`, Elza
  (-23500,-1000) Blick 3259, Wurf ab Bild 60): `resolver art=2 P=(-24194,-729,-1190) r=2000 eingriffe=2`,
  `[gr] RE2 Slot 1 Typ 0x16 ... ZERFETZT (HP -921)`, `[gr] RE2 Slot 2 Typ 0x16 ... ZERFETZT (HP -950)`, beide ab
  Bild 180 Leiche (Zustand 7). Vorher derselbe Lauf: beide sterben (HP 79/50 - 80 = -1/-30) ohne Zerfetzung.
  Die Granate zuendet hier ueber Elza selbst (Wandzelle im Wurfbild, Runde 35 N1.2 Wurfbild-Regel), kein Spielerzweig.
  Bild `GR_belege/port/p03_elza_1041_wurf_vorher_nachher.jpg` + `p03_elza_1041_log_auszug.txt`: die Kamera dieses
  Schnitts zeigt nur Elza am Rand und die roten Scherben (#294-#306), die Zombies liegen ausserhalb des Bildes —
  als Bildbeleg schwach, als Logbeleg vollstaendig.
* Dicker Zombie mit Elza: in keinem der geprueften Elza-Raeume ist beim Eintritt ein 0x11 erreichbar (Zensus
  1001/1011/1021/1031/1041/1061/1071/1081/1091/10D1/10E1/1101/11E1/1201; 0x11 nur in 1021/1071/1091, dort im
  Ruhezustand ohne Treffer, `RE15_FORCE_EXPLOSION ... Treffer=0`). Der dicke Zombie ist deshalb mit Leon belegt (§6.1,
  §6.2) und fuer beide Figuren im Riegel (unit_r41_granate [1]/[2], figurunabhaengig).

## 7. Tests

* NEU `unit_r41_granate` (tests/unit/test_r41_granate.c, probes/r41_granate.cmake): [1] Schaden 1000 an
  0x10/0x11/0x12/0x13/0x16/0x18 (dick 0x11 mit 250 -> -750, Tod), RE1.5-KI-Dicker (107 -> -893), Saeure/Brand bleiben
  200, Nicht-Zombies unveraendert (Hund 300, Kraehe 60, Spinne 60, Gorilla/Kakerlake/Alligator 1000); [2] RE2-Gehirn,
  sechs Typen (0x10/0x11/0x16 aus ROOM1140, 0x12/0x13/0x18 umgetypt): Zerfetzung genau einmal, Kopf/Oberarme/
  Oberschenkel fliegen, Unterarme/Unterschenkel/Fuesse platzen, Haende weg, Sturz 0x80108530, Leiche, kein Kriecher,
  mit nachgefahrenem Zeichengang (Teilstufe + Part-Matrix wie main.c) nach 150 Bildern nur Rumpf + Becken gezeichnet;
  [3] RE1.5-Gehirn z_granate, Leiche ohne Kriecher.
* NEU `unit_r41_granate_aus` (eigener Prozess, `RE15_KEIN_80PROZENT_GORE=1`): keine Zerfetzung (RE2 0x80108BEC wie
  vorher), Schaden bleibt 1000.
* Verhaltensbedingt angepasst (alte Pins der schwaecheren Granate / des Zerreissens): probe_r34_schaden 31/34/35/38/99,
  probe_r34_reaktion 220/221/222, test_r35_granate 120-130/173/234, test_r36_granate 105, test_re2_gore PIN 8
  (Latch vorbelegt, misst weiter den Russ allein), test_zombie_gore (w9 zerfetzt, w10 behaelt Oberkoerper/Kriecher),
  integration_r35_granate und integration_r36_granate (Zerfetzung je getroffenem Slot ueber RE15_GR_LOG statt
  "weggeschleudert > 600"; 0x11 mitgezaehlt).
* Gelaufen: Zielauswahl 101 Tests (`granate|gore|zgore|re2z|ztear|teardeath|r34_schaden|r34_reaktion|zombie_death|
  corpse|deathgetup|werfer|weapon_rows|dog|hund|crow|kraehe|spider|spinne`, ohne integration) 101/101 gruen;
  integration_r34_granaten, integration_r35_granate, integration_r36_granate 3/3 gruen (528 s).
* Volle Unit-/Prop-/Probe-Suite im Baum (`ctest -E ^integration_ -j 8`, 2026-11-02): **1076/1076 gruen** (1023 s).
* Gegenprobe RE2-Modelle (Menue "RE2 AI + Model", ohne `RE15_AI_MODELS`): derselbe Wurf, vier `[gr]`-Zeilen, Bild
  `GR_belege/port/p04_leon_1140_re2modelle_nachher.jpg` (Rumpf traegt dort das RE2-Stumpf-Mesh 16).

## 8. Fuer den Nutzer (Kurzfassung)

1. **Welche Granate:** die normale Handgranate = Item 0x09 (nicht Saeure 0x0A / Brand 0x0B, nicht der Granatwerfer).
2. **Video:** In den 80%-Videos wird keine Handgranate geworfen. Die Zombie-Explosionen (B 3:31, 3:36, 8:05, 8:42) kommen
   aus Elzas Granatwerfer (Muendungsblitz an der Huefte, Einschlag 0,2-0,5 s spaeter). Dort zerplatzt der Zombie in einer
   roten Saeule, uebrig bleiben Lache und Fleischhaufen. Genau das macht jetzt die Handgranate (und die
   Granatwerfer-Explosivrunde, die dieselbe RE2-Zeile 9 nimmt).
3. **Staerke:** Die Handgranate macht an Zombies wieder ihre eigenen 1000 Schaden aus dem RE1.5-Original statt der 200 der
   RE2-Granatwerfer-Zeile. Die dicken Zombies (Typ 0x11, 250 HP) ueberlebten bisher mit 50 HP; jetzt sterben alle Zombies
   im Wirkradius. Hund, Kraehe, Spinne, Gorilla und Bosse: unveraendert.
4. **Zerfetzen:** Kopf, Oberarme und Oberschenkel fliegen weg, Unterarme, Unterschenkel und Fuesse platzen in rote
   Scherben, die Haende sind weg, Rumpf und Becken bleiben verrusst in einer grossen Lache liegen. Alles aus vorhandenen
   RE2-/RE1.5-Mechanismen zusammengesetzt, die Zusammenstellung ist Gestaltung nach dem Video.

## 9. Offen

1. Elza am dicken Zombie im echten Fenster: kein Elza-Raum mit beim Eintritt erreichbarem 0x11 gefunden (§6.4). Logik
   figurunabhaengig und im Riegel; ein Bild Elza + 0x11 braucht einen Spielstand mitten im Elza-Ablauf.
2. Saeure-/Brandgranate (w10/w11) im RE1.5-Gehirn behalten die alte Gestaltung "Oberkoerper ab, kriecht weiter", deren
   Videobeleg (BEFUND 5.2, 8:10-8:22) laut Spur S nicht traegt. Nicht Teil dieses Auftrags.
3. 0x18 mit dem Einzel-HP-Wert 1058 (@0x8011f33a) ueberlebt die 1000 nur ohne RE2-Modell (Import-Option aus + RE1.5-KI,
   nicht die Vorgabe) — wie im Original.
4. Der Rumpf verschwindet nicht ganz (Video 3:31.6/3:36.3: Koerper fast weg). Keine Quelle hat ein Rumpf-Platzen fuer
   Zombies; der Haufen in der Lache entspricht 8:42.5. Ein Rumpf-Platzen waere reine Erfindung.
