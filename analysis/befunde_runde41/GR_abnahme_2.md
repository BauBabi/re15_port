# Spur GR — Unabhaengige Abnahme 2 (nach Nachbesserung 2 + 3), Runde 41, 2026-11-02

Pruefer: unabhaengiger Abnahme-Agent (nicht der Autor). Gegenstand: Zweig `r41/granate`, Kopf 78ad377d2 (NB2 2e0d8caaa/5670d7cab,
NB3 2fb07b019/78ad377d2). Abnahme 1 war auf d8d847a93.
Bau: `bash re15_port/tools/local_build.sh configure` + `build` im Worktree-Baum, vorher alle in `d8d847a93..HEAD` geaenderten
Dateien per `touch` erneuert -> 393 Uebersetzungen, `re15_pc.exe` 22:00 (8 711 797 Byte). Messkopie `re15_pc_abn2gr.exe`
(eigene PID je Lauf), Gegenprobe `re15_pc_abn2gr_vorher.exe` = Kopie von `re15_pc_nb1_vorher.exe` (Stand d8d847a93).
Werkzeuge (eigene, `GR_belege/abnahme2/werkzeuge/`): `cap.ps1` (Start-Process, gdigrab `hwnd=` NUR am eigenen Fenster, beschleunigter
Renderer, ohne AUTOSHOT/SOFTWARE_RENDER), `hp.py` (HP-/Zustandswechsel aus `RE15_STATE_LOG`), `sheet.py`/`strip2.py` (Bildleisten,
**sequentiell dekodiert** - Seek per `CAP_PROP_POS_FRAMES` lieferte bei diesen mkv falsche Bilder, deshalb verworfen).
Alle Bildnummern `F...` = Spielbilder nach dem Debug-Sprung; HP-Werte aus `state.log`. Die Maschine lief durchgehend mit 100 %
CPU-Last (andere Spuren), das Spiel lief langsamer als Echtzeit; die Zuordnung Videobild -> Spielbild in den Leisten ist daher
angenaehert (Bildunterschriften), massgeblich sind die Logwerte (`GR_belege/abnahme2/abn2_logauszug.txt`).

## URTEIL: BESTANDEN (mit Maengeln 1-3, keiner blockierend)

Alle fuenf Nutzerentscheide sind im echten Fenster mit Logwerten bestaetigt: Spinne (RE2-KI) und Hund sterben an EINER echten
Handgranate (1000); der Gorilla ueberlebt sie (360 -> 40) und stirbt nach genau 4 Pistolentreffern; der Birkin ueberlebt sie in
jeder gemessenen Form und braucht danach genau 8 Pistolenschuesse (Form 1 600 -> 310 -> Mutation nach dem 8. Schuss, Riesenform
1000 -> 110 -> Rueckstoss-Szene nach dem 8. Schuss); beim Gator-Boss loest eine echte Granate die Kanister-Szene NICHT aus
(25300 -> 25050), die Gegenprobe mit d8d847a93 loest sie sofort aus (24300, Bild F301). Birkin-INIT 600 / Mutation 300 /
Riesenform 1000 laufen durch Mutation, Riesenform, Rueckstoss und G5 (Leon im Fenster, Leon + Elza im Integrationslauf).
Tests: alle gruen bis auf eine Lastueberschreitung (Mangel 3) und die bekannten df-Haenger (Umgebung).

## 1. Schadenslogik (Pruefauftrag 1) — Code gelesen, gegen die Tabelle

`re15_damage.c` `re15_granate_schaden` (Aufruf nur `if (type == 2u)` im Gegnerzweig von `re15_enemy_take_damage_at`, der
E16-Block fuer den G5 steht danach und ueberschreibt mit 80):

| Fall | Code | Erwartung N3.3 | gemessen (Abschnitt 2) |
|---|---|---|---|
| normaler Gegner (default) | `re15_damage_table[2]` = 1000 (@0x8006f41c) | 1000 | Spinne 110 -> -890, Hund 96 -> -904 |
| Gorilla 0x27 | 180 x `RE15_R39_AFFEN_HP_FAKTOR` (Regel an) - 40 | 320 | 360 -> 40 |
| Gator-Boss 0x23 in 2090/2091 | 25300 - 25000 - 50 | 250 | 25300 -> 25050 |
| Alligator 0x23 sonst | 300 - 50 | 250 | (nur unit 71) |
| Birkin Form 1 (Mutation offen) | 600 - 200 - 110 | 290 | 600 -> 310 |
| Birkin Riesenform | 1000 - 0 - 110 | 890 | 1000 -> 110 |
| Birkin Mensch (grid 0x10) / nach Mutation / waehrend Mutation | 490 / 190 / 190 | dito | unit 72-75, 78, 79 |
| G5 0x36 in 5090/5091 | E16 80 | 80 | unit 81 |

Randfaelle (gelesen):
* **Mutation:** `re15_bf_granate_form` prueft in dieser Reihenfolge Bahnsteig-Bank (1000) -> `birkin_flags & 0x8` (Mutation laeuft,
  300) -> Merker "nach der Mutation" (300, nur solange `hp <= 300`) -> Form-1-Tor (`!(grid & 0x10)` oder 5090 vor dem
  Mutationsstart: Ende 200) -> sonst 600/0. Das Mutationsende (sub 6 Phase 2 @0x8011888c) setzt `grid |= 0x10` und loescht Bit 8;
  ohne den Merker kaeme ein Birkin nach der Mutation in 3071 auf 490 Schaden und stuerbe an einer Granate - der Merker deckt
  genau das. Ueberschiessender Form-1-Schaden fuehrt (Wurzel-Tor `hp < 200` -> 0x601) in die Mutation, nicht in den Tod.
* **Revive:** `RE15_BIRKIN_HP_REVIVE` = 100 (0x32 @0x8011a530 x 2); eine Granate waehrend der Mutation (190 von 300) ist nie
  toedlich. Gemessen: 8 Pistolenschuesse in der Mutation 300 -> 180, kein Revive ausgeloest (Leon 5090, F1146-F1321).
* **Raumwechsel-Merker:** `s_bf_nach_mutation` wird in `re15_bf_reset` geloescht; Aufruf bei JEDEM Raumwechsel
  (`enemy_common.c:171`, Bankabbau), nicht nur beim Hangar. In Ordnung.
* **Elzas Birkin 3071:** Form 1 ueber grid 0x01 (Member_set(0x0C,1) @0x03645) -> Tor `!(grid & 0x10)`; nach der Mutation Merker.
  Nur Unit-Beleg (Faelle 76, 78, 79 gruen), kein Fensterlauf (Mangel 2).
* **Granatwerfer, Saeure/Brand unveraendert:** der neue Block greift nur bei Art 2; Art 3/4 (E4-Zeilen) und 6..9 laufen wie vorher
  (unit 3: 0x11 Saeure/Brand je 200; unit 102/105: Werfer-Arten 100/200/300/1000).
* **`RE15_KEIN_80PROZENT_GORE`:** `re15_granate_schaden` liest den Schalter nicht (Absicht, Nutzerentscheid); unit 61 "Schalter aus:
  Schaden bleibt 1000" gruen.
* **HP-Skala vollstaendig:** im 0x30-Tick (`enemy_ai_common.c` 12100-12800) stehen alle HP-Zuweisungen/-Schwellen auf
  `RE15_BIRKIN_HP_*` (INIT, Mutation, Revive, Verwundet-Tor 400, Mutations-Tor 200); einzige Literal-Zahl ist die Fern-Rueckgabe
  `+5` @0x8011a138 (Absicht, N3.2). Hangar-Aufbau setzt `RE15_BIRKIN_HP_INIT`. Gemessen: INIT in 3070 = 600 (vorher-exe: 300).
* Adressen stichprobenhaft gegen das Dossier gelesen (Kommentare/Makros): 0x12c @0x8011690c, slti 200 @0x801164fc, slti 100
  @0x8011667c, 0x96 @0x80118094, 0x32 @0x8011a530, G4 `addiu v0,zero,1000` @0x80100678, `slti v0,v0,25000` @0x80101ec4 -
  konsistent zwischen Header, Damage-Block und Dossier.

## 2. Sichtlaeufe im echten Fenster (Pruefauftrag 2) — eigene Messungen

| Lauf | Aufstellung | Ergebnis (state.log / gr.log / debug.log) | Bild |
|---|---|---|---|
| Spinne 0x25, ROOM2030, **RE2-KI**, **echter Wurf** | `RE15_PLAYER_POS=6550,2000,3072,3`, Granate, Skript ab F270 | `EV resolver art=2 ... eingriffe=1` T=558; Slot 1 **110 -> -890** F312 (Zustand 3, Waffe 9), F313 sechs Babyspinnen 0x26, F324 Zustand 7; im Bild gelbe Splitter, ab +0,6 s keine Spinne mehr | `abn2_spinne2030_re2ki_echter_wurf.jpg` |
| Hund 0x20, ROOM1230, RE2-KI, **echter Wurf** | `RE15_PLAYER_POS=-172,9800,3072`, Cut 6 | `[gr] Hund Slot 1 Waffe 9: ZERFETZT (Teile-Maske 0x079c, HP -904)`; **96 -> -904** F42, Zustand 7 F64 | `abn2_hund1230_re2ki_echter_wurf.jpg` |
| Gorilla 0x27, ROOM11C0, Leon | `RE15_SET_FLAG=4:0x40`, Pos (-6729,-12800), Cut 5, **Granate per Haken `RE15_FORCE_GRANATE=300:2`** (echte Explosion Art 2), danach **echte Pistolenschuesse** | **360 -> 40** F300 (`akku=320 -> FLINCH`); Schuss 1-4: F382 28, F404 16, F444 4 (FLINCH), F466 **-8** Zustand 3; Leiche F536 | `abn2_gorilla11c0_granate_dann_4_pistolentreffer.jpg` |
| Birkin Form 1, ROOM5090, Leon, **echter TIEF-Wurf**, Waffenwechsel ueber den Statusschirm, echte Pistole | Pos (500,-9175), Skript `A0.1,W1,U8.5,W9,MD...` | **600 -> 310** F954 (Zustand 2, Waffe 9); 8 Schuesse 295/280/265/250/235/220/205/**190** -> F1145 Mutation (sub 6), F1146 HP **300** | `abn2_birkin5090_leon_form1_mutation_riesenform_echte_wuerfe.jpg` |
| Birkin waehrend der Mutation | Pistole weiter | 300 -> 180 nach 8 Schuessen, kein Revive; F1326 Riesenform **1000** (Clip 10) | dito |
| Riesenform, ROOM5090, **echter Wurf** (Wechsel auf Granate und zurueck ueber den Statusschirm) | dito, Lauf b7 | **1000 -> 110** F1703 (im Griff, Zustand 2 -> Griff fortgesetzt); 8 Schuesse 95/80/65/50/35/20/5 -> 8. Schuss F2192 Zustand 3 (Rueckstoss-Szene, HP 1), F2349 G5 0x36 **600** | dito |
| Gator-Boss 0x23, ROOM2090, Leon, **echter Wurf** + Pistole | `RE15_SET_FLAG=9:110,3:245`, Pos (-6000,-17500,3072) wie `integration_r41_kanister2080_kampf` | **25300 -> 25050** F299, KEIN Ausloeser; Schuesse je 7: 25043 ... 25001, **8. Schuss F1057 24994** (Schwaeche), Ausloeser `[k2080] F1230 AUSLOESER geschwaecht ... Treffer 11` nach dem 10. Schuss (Mangel 1) -> ROOM2080 | `abn2_gator2090_neu_oben_vorher_unten.jpg` (oben) |
| Gegenprobe Gator, **d8d847a93** | gleiche Eingabe | **25300 -> 24300** F299, `F301 AUSLOESER ... Treffer 1` - Kanister-Szene sofort nach der Granate | dito (unten) |

Echte Wuerfe: Spinne, Hund, Birkin (beide Formen), Gator. Beim Gorilla war kein echter Wurf moeglich (4 Versuche g1-g4): in 11C0
trifft der Gorilla Leon in diesem Aufbau alle 57-58 Bilder (F289, F347, F404, ... je 6 HP, Rueckstoss mo 8 bis ~21 Bilder);
Zielen bis Wurf-Freigabe braucht ~40 Bilder (gemessen in 1230: Zielen F1, Freigabe F41), das Fenster zwischen zwei Treffern ist
~34 Bilder. Deshalb der Haken wie beim Autor (echte Explosionsroutine, derselbe Schadensweg Art 2), die Pistolentreffer echt.

## 3. Birkin-HP x 2 — Mutation, Revive, Tore, Leon und Elza (Pruefauftrag 3)

* **Fenster, Leon 5090 (Lauf b7, oben):** Form 1 -> Mutation am Tor `hp < 200` (190 nach dem 8. Schuss), Mutations-HP 300,
  Riesenform-Eintritt mit 1000, Rueckstoss-Szene, Uebergabe an den G5 (600). Ablauf vollstaendig, keine Haenger durch die HP.
* **Fenster, Leon 5090 ohne Granate (Lauf b2):** Form 1 nur mit der Pistole 600 -> 195 = **27 Schuesse** bis zur Mutation
  (Tabelle N3.3: 27), Mutation F1459 mit HP 300.
* **Integration (eigener Lauf, `integration_r41_birkin_pistole`, Leon UND Elza 5090/5091, bildgleich):** Form 1 ab 600, Mutation
  F1274 (HP 195), Riesenform 1000 F1455, G5 600 F3594, G5 tot F4706 (Magazin 222 -> 89). Elza laeuft dieselbe Folge.
* **3070 INIT:** Leon-Birkin grid 0x10 (Sce_em_set @0x0340C nach `RE15_SET_FLAG=4:215`) startet mit **600** (vorher-exe: 300).
* **3071 (Elza, Form 1 + nach der Mutation):** nur Unit (76/78/79 gruen) - siehe Mangel 2.
* **Kampfdauer:** mit je einer Granate je Form dauert der 5090-Kampf (Form 1 + Mutation + Riesenform) im Fenster F954 -> F2192 =
  1238 Spielbilder (~41 s inkl. drei Waffenwechsel ueber den Statusschirm); nur Pistole (Integration) F397 -> F4706 (~144 s
  Dauerfeuer, 166 Schuss). Plausibel und wie im Dossier N3.3/N3.5.

## 4. Nebenwirkungen (Pruefauftrag 4)

* Andere Waffen gegen Bosse: nur ueber die HP veraendert (Birkin x 2; Gorilla/Alligator/Gator/G5-HP unangetastet). Gemessen: die
  Pistole macht am Birkin weiter 15 (Fern-Rueckgabe +5 bleibt: Integration F397 600 -> 590), am Gorilla 12, am Gator 7.
* Normale Gegner gegen andere Waffen: der neue Code greift ausschliesslich bei Art 2 (`if (type == 2u)`); der Rest von
  `re15_enemy_take_damage_at` ist gegenueber d8d847a93 unveraendert (diff gelesen). Saeure/Brand/Werfer-Werte per Unit belegt.
* Riegel-Anpassungen des Autors (probe_r34_schaden 13/13b, probe_r34_wurf 24, test_r35_granate 112, probe_r34_reaktion 187/197,
  test_r35_granate.cmake gator -250) folgen dem Nutzerentscheid; geprueft, dass sie das neue Verhalten pinnen, nicht aufweichen.

## 5. Tests (Pruefauftrag 5)

* `unit_r41_granate` alle Abschnitte ([1]-[7]: 1-4, 21-33, 41-43, 51-53, 70-82, 90, 91, 100-105) + `unit_r41_granate_aus` (61, 62):
  **2/2 gruen**.
* Integration `integration_r34_granaten` 306 s, `integration_r35_granate` 232 s, `integration_r36_granate` 148 s,
  `integration_r41_birkin_pistole` 425 s (`-j 4`): **4/4 gruen**.
* Volle Unit-/Prop-/Probe-Suite (`ctest -E "^integration_|<df-Tests>" -j 4 --timeout 1200`, 1070 Tests, 1121 s):
  **1069/1070 gruen**; rot nur `unit_r41_birkin_wand_griff` **Timeout 360 s** (eigenes ctest-TIMEOUT) - einzeln wiederholt:
  wieder Timeout; direkt ohne ctest-Zeitlimit: **OK** (alle sechs Raeume 5090/5091/5080/5081/3070/3071, 8 min 12 s bei 100 %
  CPU-Last). Mangel 3.
* Ausgenommen (Umgebung): `unit_r38_releaseskript_{trocken,ablauf,fehler_bau,fehler_umfeld,cmd}`, `unit_r39_titelversion_ablauf`,
  `unit_r35_android_bash_mutanten` - das Release-Skript ruft `df -Pk`, und `df.exe` haengt maschinenweit seit 17:38 (Prozessliste:
  79 haengende `df.exe`, darunter Laeufe in r41_moosgang/story80/gatorfix/sichel/g5auftritt/form und in diesem Baum). Gesondert
  gruen: `unit_r38_releaseskript_bausteine`, `unit_r38_releaseskript_version` (2/2). Kein Engine-Bezug.
* Es wurden nur eigene Prozesse (eigene PIDs) beendet bzw. abgewartet; kein fremder ctest beruehrt.

## 6. Maengel

1. **Gator-Boss: "1 Granate + 8 Schuss bis zur Kanister-Szene" (N3.2/N3.3) ist ungenau formuliert.** Gemessen: die SCHWAECHE
   (`hp < 25000`, @0x80101ebc-dc) kommt genau mit dem 8. Pistolenschuss (F1057, 24994); die SZENE startet aber erst, wenn der
   Ausloeser den Alligator "frei" findet (`kanister2080_r41.c` `gator_frei`: verfolgt, nicht getaucht, nicht beim Fressen) -
   im Lauf nach dem 10. Schuss (`Treffer 11`, F1230). Das ist das bestehende Tor der Spur G, kein Fehler von GR; die Tabelle
   sollte "8 Schuss bis zur Schwaeche, die Szene folgt beim naechsten freien Moment" sagen. Schwere: gering (Dokumentation).
2. **Kein Fensterbeleg fuer 3070/3071.** Nach `RE15_DEBUG_JUMP=3070` (mit `RE15_SET_FLAG=4:215` = Kampfaufstellung Sce_em_set
   @0x0340C) nimmt Leon keine Eingabe an (Zielen/Feuer/START wirkungslos, mo 200 ueber 900 Bilder) - **identisch mit der
   vorher-exe** (d8d847a93), also ein Messweg-Problem, nicht GR. Elzas Form 1 -> Mutation -> nach der Mutation in 3071 ist deshalb
   nur ueber unit 76/78/79 belegt; der Ablauf Form 1 -> Mutation ist im Fenster in 5090 belegt (dieselbe Wurzel und dieselben Tore).
   Schwere: gering (Abdeckung).
3. **`unit_r41_birkin_wand_griff` reisst unter Last sein Zeitlimit 360 s** (inhaltlich OK, 8 min 12 s bei 100 % CPU). Die GR-
   Aenderung beruehrt den Lauf nicht (der Test setzt `e->hp = 400`: weder das neue Verwundet-Tor `< 400` noch das Mutations-Tor
   `< 200` greift - wie vorher bei `< 200`/`< 100`). Auf einer ruhigen Maschine wiederholen; ggf. Zeitlimit des Riegels pruefen
   (Spur R). Schwere: gering (Umgebung).

## 7. Nebenbefunde (nicht von GR verursacht, gemessen)

1. **Softlock: START (Statusschirm) waehrend des Riesenform-Griffs.** In 5090 oeffnet START den Statusschirm auch dann, wenn die
   Riesenform Leon gepackt hat (`gr=1`, Form sub 15 Clip 27), und erlaubt dort sogar den Waffenwechsel. Danach bleibt die Form
   auf Clip 27 Bild 94 stehen, Leon auf Clip 11 - **beide ueber 600+ Bilder eingefroren**, Schuesse wirkungslos (Laeufe b4, b5).
   Gegenproben: derselbe Ablauf OHNE Granate friert ebenso ein (b5); derselbe Griff OHNE START-Druck endet normal (b6: F1750
   wieder Gehen). Ursache also Statusschirm-im-Griff, unabhaengig von Granate und HP. Mit der Riesenform-HP 1000 dauert der Kampf
   laenger und der Griff kommt oefter - die Lage wird dadurch haeufiger erreicht. Fuer Spur Form/R (Griff) melden; ob das
   Original den Statusschirm im Griff sperrt, habe ich nicht disassembliert. Log: `abn2_logauszug.txt` Abschnitt b5/b6.
2. 11C0: der Gorilla trifft Leon in der Aufstellung des Autors alle 57-58 Bilder aus ~2100 Einheiten Abstand (Bild F347:
   Gorilla (-8489,-12939), Leon (-6342,-12772)); nicht untersucht, nur als Grund fuer den Granaten-Haken genannt.

## 8. Boss-Tabelle (eigene Messwerte, echtes Fenster)

| Boss / Form | Weg | HP vorher | nach 1 Granate | Pistolenschuesse danach bis Formende | Formende |
|---|---|---|---|---|---|
| Spinne 0x25 (RE2-KI) | echter Wurf | 110 | **-890** | — | tot (Zustand 3/7), platzt, 6 Babys |
| Hund 0x20 (RE2-KI) | echter Wurf | 96 | **-904** | — | zerfetzt |
| Gorilla 0x27 11C0 | Haken + echte Pistole | 360 | **40** | **4** (28/16/4/-8) | tot F466 |
| Birkin Form 1 5090 | echter Wurf + Pistole | 600 | **310** | **8** (bis 190) | Mutation, HP 300 |
| Birkin waehrend Mutation | Pistole | 300 | — | 8 Schuss -> 180, kein Revive | Riesenform |
| Riesenform 5090 | echter Wurf + Pistole | 1000 | **110** | **8** (bis 5, 8. -> Rueckstoss) | Rueckstoss F2192, G5 600 |
| Gator-Boss 2090 (neu) | echter Wurf + Pistole | 25300 | **25050** (kein Ausloeser) | **8** bis Schwaeche (24994), Szene nach dem 10. | Kanister-Szene F1230 |
| Gator-Boss 2090 (d8d847a93) | gleich | 25300 | 24300 | 0 | Kanister-Szene sofort F301 |
| Birkin Form 1 nur Pistole | Pistole | 600 | — | 27 bis Mutation | Mutation, HP 300 |

## 9. Bildleisten (GR_belege/abnahme2/)

`abn2_spinne2030_re2ki_echter_wurf.jpg`, `abn2_hund1230_re2ki_echter_wurf.jpg`, `abn2_gorilla11c0_granate_dann_4_pistolentreffer.jpg`,
`abn2_birkin5090_leon_form1_mutation_riesenform_echte_wuerfe.jpg`, `abn2_gator2090_neu_oben_vorher_unten.jpg`,
`abn2_logauszug.txt` (Env/Skript, Resolver-, Ausloeser-, Gorilla-Zeilen und HP-Folgen aller Laeufe, Suite-Ergebnis, Griff-Gegenproben),
`werkzeuge/` (cap.ps1, hp.py, sheet.py, strip2.py).
