# Spur AN: unabhaengige Abnahme 2 (Runde 41)

Gepruefter Stand: Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Kopf 10fed9d19 (Code afadac63f, Merge
r41/form 7d79bcfae = b0685df4f). Bau: `bash re15_port/tools/local_build.sh build` -> `LOCAL-BUILD-OK`, "ninja: no work to do"
(exe 2026-11-03 03:28, juenger als afadac63f; 10fed9d19 aendert nur Doku). Ich bin nicht der Autor. Alle Werte unten stammen
aus eigenen Laeufen im echten Fenster (eigene exe-Kopie `re15_pc_abn2_*`, Renderer des Nutzers, kein AUTOSHOT, kein
SOFTWARE_RENDER), Bilder per `RE15_FRAMEDUMP` (jedes Bild, komponiert vor dem Present), Ton per `RE15_AUDIO_CAP_SYNC`
(bildgenau, 1470 Stereo-Samples je Spielbild). Werkzeug: `AN_belege/abnahme2/werkzeug/`.

## 0. Urteil

**NICHT BESTANDEN.**

Die Punkte der Abnahme 1 sind bis auf M3 (Stimmen, Sache des Nutzers) behoben, und die Behebungen halten im eigenen Fensterlauf:
Annette steht beim Satz frei im Bild, der Abgang in 6030 hat kein Verschwinden im Bild, sie sackt im Trefferbild zusammen, das
Blut erreicht die RE2-Flaeche, Untertitel und Hieb-Takt stimmen, die Leiche liegt beim Wiedereintritt, der Kampfstart zeigt
die ganze Form. Ablauf und Reihenfolge-Sicherheit sind in Ordnung, alle Tests sind gruen.

Durchgefallen ist die Abnahme an zwei Punkten:

- **N1 (schwer, Ursache in der Form-Spur):** Im echten Spielweg (6030-Tuer -> Wagen A -> B -> C) betritt die Riesenform
  Wagen C mit **HP 0** statt 150. Der erste Pistolentreffer nach "Leon: Annette!" wirft sie sofort in die Rueckstoss-Szene
  (HP 1). Der Riesenform-Kampf direkt nach der Annette-Szene faellt damit weg. Die Integrationstests sehen das nicht, weil sie
  per Debug-Sprung in Wagen B starten; dort hat die Form HP 150. Der Fehler steckt schon in r41/form und lag auch im Lauf w8
  der Abnahme 1 vor.
- **N2 (mittel, Spur AN):** Die Leiche sieht aus wie eine Frau, die zurueckgelehnt auf dem Boden sitzt (Clip 6 Bild 12, 60 Grad
  gekippt, Knie hoch, Kopf aufrecht). Seit dem Kampfstart in Cut 11 ist sie waehrend der ganzen ersten Kampfphase gross vorn
  rechts im Bild.

## 1. Maengel

| # | Schwere | Was | Beleg |
|---|---|---|---|
| N1 | **schwer** (Ursache Form-Spur, wirkt im AN-Ablauf) | **Riesenform kommt im echten Weg mit HP 0 in Wagen C.** Die HP der Form (Slot 2, t=30) laut state.log: Wagen A F1 300, Wagen B F498 150, **Wagen C F946 0**, beim Kampfstart F1777 weiter 0. Pistole ab F1823: Rueckstoss-Szene (st 3/2, Clip 13, HP 1) bei F1915, Sturz F2011, G5 F2112. Leon wurde nie getroffen (hp 100). Kontrollen: (a) dieselbe Wagen-Kette **ohne** Annette, nur (3,89): Wagen C F946 HP 0, erster Schuss F1887, Rueckstoss F1897. (b) Dieselbe Kette auf der **r41/form-exe** (`.claude/worktrees/r41_form`, Bau 02:52): Wagen C F946 HP 0. (c) Debug-Sprung in Wagen B (Weg aller Integrationstests): Wagen C F100 HP 150, der Schaden faellt in 15er-Schritten. (d) Abnahme-1-Lauf w8 (`build/abn1/w8/state.log`): Wagen C F946 HP 0, der Fehler bestand also schon vor dem Merge. Die Form-Spur wollte HP 150 ausdruecklich (birkin_bahnsteig.c:791-796, "ohne das ... tot nach 49 Bildern, kein einziger Angriff"). Ansatz, nicht bewiesen: der Eintritt `if (s_bb.slot != slot)` (birkin_bahnsteig.c:783) laeuft beim Wagenwechsel B -> C nicht erneut, oder der Neuaufbau setzt die HP danach. | `kampf_abn2.jpg`; Laeufe `build/abn2/kampf`, `kette_ohne`, `kette_an`, `form_kette_ohne`, `torb` (state.log); Werkzeug `zustand.py` |
| N2 | **mittel** | **Leichenpose.** Clip 6 Bild 12 mit -60 Grad Kippung (AN_KIPP_WINKEL, annette_r41.c) wirkt wie Sitzen mit zurueckgelehntem Oberkoerper: Knie hoch, Kopf aufrecht und zur Kamera gewandt, die Beine reichen bis an den Fuss des Regals. Das Bild in Cut 11 entsteht jetzt beim Kampfstart (F1779 ff.), waehrend der Rueckstoss-Szene und beim G5-Auftritt (F1950 bis F2200) sowie beim Wiedereintritt. In Abnahme 1 war die Leiche nie im Bild. RE2 legt Annette flach hin (Bewegung 0x18, zeitachse.md). | `kampf_abn2.jpg` Felder F1980 bis F2200; Ausschnitt in Abschnitt 3.4 |
| N3 | leicht | **Leiche nimmt einen Treffer.** F2751: Annette hp -1 -> -6, st 0 -> 3, ss1 3, ss2 1. Im selben Bild faellt die Munition 106 -> 105 (Leon schiesst auf die G5, Abstand ca. 12600), es kommen 5 Effekte dazu (fx 18 -> 23), die G5 bleibt bei HP 345. Die Pose bleibt (leiche_halten). Das widerspricht dem Kommentar "hp -1: kein Treffer-Kandidat" (annette_r41.c:722). Welcher Schadenspfad das war, habe ich nicht bestimmt; die Kandidatenschleifen in re15_damage.c (:1108, :1148, :3640, :4450) schliessen hp < 0 aus. | state.log `kampf` F2750/F2751 |
| N4 | leicht | **6030: Abgang endet vor Leon.** Annette geht in Cut 0 zum Tuerziel (-3873,-22550). Im letzten Bild mit ihr (F1490) steht sie vor Leon und ueberdeckt ihn halb. Ab F1491 zeigt Cut 1 sie nicht mehr. Ein Verschwinden im Bild gibt es nicht (M2 behoben). Die Tuer bewegt sich aber nicht, und es gibt keinen Tuerton, sodass es nicht wie ein Abgang durch die Tuer aussieht. | `zugstart6030_abn2.jpg` Felder F1474 bis F1492 |
| N5 | leicht/Hinweis | **Annettes Gesicht ist in der Szene nie zu sehen.** Bei "William." steht sie in Cut 12 klein und weit hinten (RE2 Cut 7/8/9: Nahaufnahme). Beim Satz sieht man in Cut 11 ihren Ruecken (RE2 0x0B zeigt das Gesicht von Williams Seite). Die Form steht in Cut 11 mit dem Unterkoerper hinter Bodenfeuer. Das ist als PORT-WAHL belegt und vom Nutzer gedeckt ("Annette rechts ok"); fuer die Bildwirkung siehe Abschnitt 2.3. | `vergleich_re2_port_abn2.jpg` Zeilen 1, 2, 7 |
| H1 | Info, nicht Spur AN | Feuer in Kameranaehe wird als grobe Bloecke gezeichnet (Cut 15 F1340, Cut 11 beim Kampfstart F1806 bis F1834): ESP-Bodenfeuer aus main00, vergroessert. | `szene5090_abn2.jpg` |
| H2 | Info, ausserhalb, offen | **Vom Torschalter komme ich im Port nicht zur Zugtuer.** Die Treppe an der Lok (6000, Zellen x 16350..18800) haelt Leon bei z -20368 fest (Zelle #8, z -20400..-19900). Das gilt fuer den Tastenlauf mit Gier 3040 und 3072. Die Westtuer (Slot 2, (3,51)) fuehrt nach 6010; dort trennt der Gleisblock (Zelle #4, z -18830..-10045, x -32000..1513) beide Tueren. Der Autopilot bleibt in 6000 haengen. Gegen das Original habe ich das nicht geprueft. Die Erreichbarkeit nach dem Tor sollte die Kollisions- oder Raum-Spur klaeren. | Laeufe `t2`, `t3`, `weg1`, `weg2`, `weg3` |
| H3 | Info | Dossier N3.5 ist zu korrigieren: RE2 zeigt das Blut nicht 1,1 s lang. Ab 71,49 s steht das Bild bis zum Schnitt still (Ladestandbild); die mittlere Pixeldifferenz zum Vorbild betraegt 0,00 bis 0,04 (71,49 bis 72,13 s). Das bewegte RE2-Blut dauert 0,40 s. | Abschnitt 2.4 |
| H4 | Info | Laut state.log kommt beim Kampfstart F1777 ein Bild lang Cut 15. Gezeichnet wird er nicht: F1777 und F1778 zeigen noch Cut 13, ab F1779 kommt Cut 11. | Abschnitt 2.2 |
| M3 | wie Abnahme 1 | Die Stimmen fehlen weiterhin: Log `[voice] ... keine Aufnahme` fuer 5090 msg 40-43 und 6030 msg 27-30. | debug.log `s5090` |

## 2. Abnahme-1-Punkte M1 bis M9, eigene Messung

### 2.1 Laeufe

| Lauf | Weg | Zweck |
|---|---|---|
| `tor` | 6000 Debug-Sprung an den Torschalter (18200,-22404), Flags Bosssieg/Generator/6000-Szenen/Fuehrerstand (3,48)(3,49)(3,100)(3,239)(4,23), Aktion, "Yes" | Tor oeffnet sich. Laut `RE15_FLAG_CENSUS` schreibt das Spiel selbst `Set (3,89)` und `Set (3,22)`. (4,23) stand schon. |
| `v0`, `s5090`, `z6030`, `kampf` | Ab der 6000-Zugtuer mit genau diesem Torstand (Basisflags + (3,89),(3,22)), durchgehend: Wagen, msg 10, Fuehrerstand, **Zugstart**, Tuer nach 5090, Wagen A -> B -> C, **Szene**, Kampf. Eingabe = Tastenskript w8 der Abnahme 1; `kampf` zusaetzlich mit Pistole und Feuer. | Kernlaeufe. Bilder s5090 F1150-F1960 jedes Bild, z6030 F700-F1545 jedes Bild, kampf F1800-F5400 jedes 10. Bild. Alle vier Laeufe liefen bildgleich (gleiche [annette]-Zeilen). |
| `leiche`, `ohnezug`, `elza`, `zug_wieder`, `torb`, `kette_*`, `form_kette_ohne` | Reihenfolge und Kontrollen (Abschnitt 4, N1) | |

Den Weg vom Torschalter bis zur Zugtuer konnte ich im Port nicht laufen (H2). Der Zustand an der Uebergabe ist durch den Census
belegt: genau die Flags, die sub07 schreibt.

### 2.2 5090: Ablauf und Zeit (Lauf s5090, Bilder ab Eintritt in 5090, 30 je Sekunde)

| Schritt | Port-Bild | s ab Krach | RE2 (s ab Krach) | Beleg |
|---|---|---|---|---|
| Krach, Cut 12 | F1168 | 0 | 0 | Ton 0 im Mitschnitt bei 151,433 s = F1168 (r 0,853) |
| "William." (msg 43) | F1264..F1299 (35 Bilder) | 3,20 | 3,53 | state.log msg-Spalte |
| Schnitt Cut 15 (sie geht) | F1330 | 5,40 | 8,07 | |
| Sturz, Cut 11 | F1350 | 6,07 | 9,30 | Ton 1 bei 157,533 s (r 0,895) |
| gelandet | F1365 | 6,57 | 10,21 | |
| Bruellen | F1407 | 7,97 | 11,43 (Ton) | Ton 2 bei 159,433 s (r 0,681) |
| Satz (msg 40) | F1480..F1598 (118 Bilder) | 10,40 | 14,80 | |
| Hieb-Signal / Ausholen | F1598 / F1611 | 14,33 / 14,77 | 20,37 / 20,82 | |
| Treffer, Blut | F1640 | 15,73 | 21,69 | `[annette] F1640 Treffer`, 30 Aufrufe, Pool 96/96 |
| Schnitt weg, liegt (Cut 13) | F1673/F1674 | 16,83 | 22,83 | |
| "Leon: Annette!" (msg 42) | F1716..F1776 (60 Bilder) | | | |
| Kampfstart sub04 | F1776; Bild Cut 11 ab F1779 | 20,27 | | Bild F1777/F1778 noch Cut 13 (H4) |
| Steuerung frei | F1823 | 21,83 | 24,39 | |

- **M6 behoben:** Die Zeilen stehen genau 35/118/60 Bilder (RE2 Xa 0x30 = 1,17 s, Xa 0x31 = 3,95 s). Keine laeuft ueber einen
  Schnitt: 43 endet bei F1299, der Schnitt folgt bei F1330; 40 endet mit dem Hieb-Signal; 42 endet mit dem Kampfstart.
- **M7 (Info) gilt als behoben:** Vom Hieb-Signal bis zum Treffer vergehen 42 Bilder = 1,40 s (RE2 1,32 s), das Ausholen ist
  sichtbar (Felder F1612 bis F1627). Vom Treffer bis zum Schnitt vergehen 1,13 s (RE2 1,14 s, davon 0,74 s Ladestandbild, H3).
  Gesamt: Port 21,8 s bis zur Steuerung, RE2 24,4 s. RE2 enthaelt Ladestandbilder an den Schnitten (eines habe ich selbst
  gemessen, H3), dazu kommt der kuerzere Weg.
- Toene: Krach, Sturz und Bruellen liegen bildgenau auf den protokollierten Bildern (Mitschnitt im mp4 bei 0,600 s, 6,700 s und
  8,600 s ab F1150). Zug an und Krach in 6030 liegen 7,000 s auseinander (F800 -> F1010).

### 2.3 M1: Satz und Landung (Cut 11)

Gegenprobe zum Bildanteil: Ich habe den ID-Puffer der Autoren (`werkzeug/n3/szmass.py`) mit Lagen, Gier und Clip aus **meinem**
state.log gerechnet und als Umriss auf **mein** Fensterbild gelegt (`werkzeug/deckung.py`, Bild `deckung_satz_abn2.jpg`).
Die Umrisse decken sich mit den gezeichneten Figuren. Abweichungen gibt es nur in der Pose: das Werkzeug setzt fuer Annettes
Leerlauf Clip 1 Bild 0 ein, und den ausgestreckten linken Arm der Form bei F1366 bildet es nicht nach.

| Bild | Schnitt | Form im Band | Annette frei (gegen die Form) | Annette ganz zwischen den Balken |
|---|---|---|---|---|
| F1366 gelandet | 11 | 9,8 % | 100 % | ja (Fuesse ueber y 432) |
| F1420 Bruellen | 11 | 8,7 % | 100 % | ja |
| F1500 Satz | 11 | 9,9 % | 100 % | ja |
| F1590 Satzende | 11 | 9,8 % | 100 % | ja |

**M1 ist behoben.** Die Form verdeckt Annette nicht mehr, und die Landung ist kein schwarzes Bild mehr: Die Form faellt bei F1352
oben ins Bild und steht ab F1366 links in der Tuer.

Bewertung der Bildwirkung (PORT-WAHL Cut 11): Die Szene ist gut lesbar. Annette steht als Sprecherin rechts vorn, die Form
dahinter in der Tuer ihr zugewandt, der Untertitel steht unten. Die Lage "Annette rechts" (Nutzerentscheid) ist erfuellt. Was
gegenueber RE2 fehlt, ist der Gegenschuss: Waehrend "W-William! You're alive!" sieht man 4 s lang ihren Ruecken. Die Form ist
klein und dunkel vor der dunklen Tueroeffnung, ihr Unterkoerper verschwindet hinter dem Bodenfeuer. Der Satz wirkt deshalb
schwaecher als in RE2, verfehlt die Szene aber nicht. Eine bessere Wahl gibt es mit den Disc-Schnitten von 5090 nicht; das
haben die Autoren mit suche3 belegt, und ich habe es nicht erneut gerechnet. Deshalb nur Hinweis N5.

### 2.4 M4/M5: Trefferreaktion und Blut

- **M4 behoben:** Ab dem Trefferbild F1640 spielt Clip 6. Bei F1652 ist sie zusammengesackt, bei F1663 bis F1672 liegt sie
  sichtbar am Boden vor der Form (`szene5090_abn2.jpg` Felder F1652, F1666).
- **M5 behoben.** Masszahl wie die Autoren (`blutflaeche.py`: rein rote Pixel im Band, ohne die schon vorher roten Pixel), aber fuer
  jedes Bild (`werkzeug/blut2.py`). RE2-Bilder habe ich selbst aus `re2c_szene_320.mp4` gezogen (Clipzeit = Aufnahmezeit - 1,00 s).

| ab Treffer | Port | RE2 |
|---|---|---|
| 0,00 s | 2,5 % | 3,7 % |
| 0,10 s | 11,9 % | 4,4 % (0,08 s) |
| 0,20 s | **17,2 %** (Spitze) | 3,8 % (0,16 s) |
| 0,30 s | 11,3 % | 7,4 % (0,24 s) |
| 0,40 s | 7,8 % | **16,4 %** (0,40 s), danach Standbild |
| 0,50 / 0,60 s | 8,5 % / 9,2 % | 16,4 % (Standbild) |
| 0,70 s | 7,2 % | 16,4 % (Standbild) |
| 0,83 s | 2,1 % | 16,4 % (Standbild) |
| 1,10 s | 1,2 % | 16,4 % (Standbild bis zum Schnitt 1,12 s) |

  Groesse: Die Spitze im Port (17,2 %) entspricht der RE2-Spitze (16,4 %). Dauer: Im Port liegt das Blut von 0,10 bis 0,70 s
  ueber 7 %, in RE2 ist es nur 0,40 s lang bewegt zu sehen. Danach friert RE2 das Bild ein (H3). Farbe: Das Port-Blut ist dunkler rot
  und grob blockig, RE2 leuchtend rot (`vergleich_re2_port_abn2.jpg` Zeile 10).

### 2.5 M2: Abgang in 6030 (Lauf z6030, jedes Bild)

Zugstart F704 (Cut 0 -> 1). Cut 3 F720, Cut 4 und Ton 3 F800, Cut 1 F830, msg 27 F890..989, Krach F1010, beide drehen sich zur
Wagentuer (F1050 bis F1092), msg 28 F1100..1199, msg 29 F1220..1319, msg 30 F1340..1439, Cut 0 F1460. Danach geht sie zur Tuer.
Das letzte Bild mit ihr ist F1490 (Cut 0), ab F1491 kommt Cut 1 ohne sie. Steuerung frei F1534. **Kein Bild zeigt Cut 0 ohne
sie, M2 ist behoben.** Das bleibt bildgenau so, obwohl der Schnitt im state.log schon bei F1490 steht: das Bild wird mit dem
Stand vor dem VM-Schritt gezeichnet. Rest: N4.

### 2.6 M8/M9: Leiche, Kampfstart

- **M8 behoben (Mechanik):** Lauf `leiche` mit (3,89),(9,133),(3,42) per Debug-Sprung in Wagen B, dann Tuer nach C. Log
  `Wiedereintritt nach der Szene: Leiche liegt`, Phase 8. Die Leiche liegt bei (4856,-22048) mit hp -1, Clip 6 Bild 12, und
  erscheint in Cut 12 hinten rechts neben Leon. Die Form erscheint nicht, eine Szene gibt es nicht. Im Kettenlauf bleibt sie von
  F1674 bis F5400 bei (4931,-22086), Clip 6 Bild 12 (bis auf N3). Pose: N2.
- **M9 behoben:** Der Kampfstart zeigt ab F1779 in Cut 11 die ganze Form (Kopf, Kiefer, Arme). Ein Bild, das nur Haut zeigt,
  gibt es nicht (`szene5090_abn2.jpg` Felder F1779, F1806, F1834).

## 3. Merge r41/form: Riesenform in 5090

| Pruefung | Ergebnis |
|---|---|
| Sturz in der Szene | richtig: y -5600 -> 0 in 14 Schritten (F1350 -> F1365), Ton 1, Landung bei L (-1378,-23395), danach Drohen-Clip 10 |
| Kampfstart, Cut 11, ganze Form | richtig (M9) |
| Form-Modell, Textur | `Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips`; in den Fensterbildern F1366 bis F1834 faellt auf der Form kein Mosaik auf (Kopf und Kiefer mit Zaehnen, Haut dunkel mit Glanzflecken). Eine Texturpruefung im Einzelnen war das nicht. |
| Rueckstoss (Taumeln Clip 13, HP 1) -> Sturz aus dem Wagen (Clip 12) -> G5 | Ablauf vorhanden: F1915 -> F2011 -> G5 (t=36, HP 600) F2112. Die G5 faellt unter der Pistole (HP -1 bei F3257), Leon lebt (hp 100) |
| **HP der Riesenform im echten Weg** | **0 statt 150 (N1)**: Die Rueckstoss-Szene kommt beim ersten Treffer, die Riesenform greift im Kampf praktisch nicht an |
| Leiche im Kampf | bleibt liegen, wird nicht verschoben. Sie nimmt einmal Schaden (N3); Pose N2 |

### 3.4 Leichenpose (N2), Ausschnitt

`kampf_abn2.jpg` Feld F2110 (Cut 11, Form gerade aus dem Wagen gefallen). Annette liegt rechts vor dem Regal, der Oberkoerper
ist etwa 60 Grad aufgerichtet, die Knie sind hoch, der Kopf ist aufrecht. Das sieht eher nach Sitzen aus als nach einer Leiche.

## 4. Reihenfolge-Sicherheit

| Fall | Ergebnis | Beleg |
|---|---|---|
| Tuer nach 5090 vor dem Zugstart | msg 10 "I have to get this thing moving first!" (6030 F192..F286), danach fuehrt der Weg in den Fuehrerstand | v0 state.log |
| nach dem Zugstart | die Tuer nach 5090 oeffnet sich (6030 F2073 -> 5090 F1) | v0 |
| 5090 nur mit (3,89) (Zugstart nie gelaufen) | `0x4D verworfen: (3,89)=1 (9,133)=0 (3,42)=0`, Original-Kampf | `ohnezug` |
| 5090 nach der Szene (Neuladen/Wiedereintritt) | Leiche, keine Form, keine zweite Szene | `leiche` |
| 5090 mit (3,89),(9,133) per Sprung (Laden vor der Szene) | Szene laeuft wie im Kettenlauf; die Form hat hier HP 150 (N1 nur im Wagenweg) | `torb` |
| Elza 5091 mit (3,89),(9,133) | keine 0x4D-Zeile im state.log, keine `[annette]`-Zeile, Kampf wie bisher (Form HP 150) | `elza` |
| 6030-Fuehrerstand nach dem Zugstart | keine 0x4D, kein Szenenmodus (pm 0 durchgehend), kein zweiter Zugstart | `zug_wieder` |

## 5. Tests (eigene Laeufe in diesem Arbeitsbaum)

| Test | Ergebnis |
|---|---|
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|android\|r37_masken36_ausschluss\|paket\|titelversion`, `-j 4` | **1040/1040 gruen** (225 s). Darin: unit_r41_annette, unit_r41_annette_se_bank/_toene/_lagen, unit_r36_kartenbits, unit_birkin_bahnsteig/_formen/_szenen/_opfer/_ai, unit_r41_form_emd, unit_n2_griff_start, unit_r41_birkin_wand_rein/_g5/_griff, probe_p2_birkin_g5, probe_p3_birkin_rest |
| integration_r35_zgirl_5090 | gruen (147 s) |
| integration_r41_annette | gruen (248 s) |
| integration_r41_birkin_pistole | gruen (242 s) |

Die ausgeschlossenen Skripttests haengen an `df.exe` (Umgebungsproblem). Laut Auftrag habe ich sie nicht wiederholt. Die
Tests decken N1 nicht ab, weil sie alle per Debug-Sprung in Wagen B starten, und N2 nicht, weil sie das Bild nicht pruefen.

## 6. Belege

`analysis/befunde_runde41/AN_belege/abnahme2/`:

| Datei | Inhalt |
|---|---|
| `szene5090_abn2.jpg` | 20 Felder 5090 (Lauf s5090, Krach bis Kampfstart) |
| `szene5090_abn2_mit_ton.mp4` | F1150..F1960 jedes Bild, 30 Bilder/s, Ton bildgenau (`RE15_AUDIO_CAP_SYNC`, Krach bei 0,600 s) |
| `vergleich_re2_port_abn2.jpg` | 13 Zeilen RE2 (eigene Bilder aus `re2c_szene_320.mp4`) und Port |
| `deckung_satz_abn2.jpg` | ID-Puffer-Umriss (cyan Form, gruen Annette) auf den Fensterbildern F1366/F1420/F1500/F1590 |
| `zugstart6030_abn2.jpg`, `zugstart6030_abn2_mit_ton.mp4` | 6030 Zugstart und Abgang (F700..F1545 jedes Bild, Ton bildgenau) |
| `kampf_abn2.jpg` | Kampf nach der Szene: Form, Rueckstoss, Sturz, G5, Leiche (N1, N2) |
| `tor_abn2.jpg` | Torschalter, Frage, Tor oeffnet sich, Cut 8 |
| `reihenfolge_abn2.jpg` | Leiche (3,42), ohne Zugstart, Elza 5091, 6030 nach dem Zugstart, Torfrage |
| `werkzeug/` | `abn2_lauf.sh` (Lauf), `blatt2.py` (Bildleisten), `deckung.py` (ID-Puffer-Gegenprobe), `blut2.py` (Blut je Bild), `zustand.py` (Zustandswechsel aus dem state.log) |

Rohdaten: `re15_port/build/abn2/<lauf>/` (nicht versioniert). Keine Datei unter `pri/`, `synchro/`, `Version_80%/` oder `info/`
veraendert, Code nicht geaendert. Alle exe-Kopien `re15_pc_abn2_*` liefen unter `timeout` auf die eigene PID und sind
geloescht. Die Kopie im r41_form-Baum (`re15_pc_abn2_formkontrolle_*`) ist ebenfalls entfernt. Die dort liegende
`re15_pc_abn2.exe` stammt nicht von mir.
