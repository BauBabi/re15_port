`C:\workspace\git\reAi_v2\.claude\worktrees\wf_5ebaf1dc-c6e-28\analysis\befunde_2026-09-21\pri-runde19\SYNTHESE.md`

# SYNTHESE — PRI-Runde 19 (2026-09-21)

Sechs Messlaeufe, sechs Skeptiker, 11 bestaetigte und 16 widerlegte Befunde.
Dieses Dokument ordnet sie nach dem, was der Nutzer sieht, fasst gleiche Ursachen zusammen
und gibt die Reihenfolge, in der gebaut wird.

**Stand des Auslieferungszweigs, selbst nachgesehen:** `master` = `4b41c995` enthaelt
**keine** der vier Aenderungen. `auswahl.json` fuehrt fuer ROOM10D0 C1 weiter
`"aufrecht": true` an der Tischplatte, ROOM1050 C1 hat keinen `zelle`-Eintrag,
`geom.py`/`geometrie.py` haben `zellen_solid_test` nicht, und `re15_port/tests/unit/probes/`
hat keinen `r19`-Riegel. Die Arbeit liegt auf vier Worktree-Zweigen:

| Zweig | Commit | Inhalt | Urteil |
|---|---|---|---|
| `worktree-wf_5ebaf1dc-c6e-2` | `e7b4063c` | Marke 1, ROOM10D0 C1 Platte `ebene:-1525` | **mergen, Riegel vorher verengen** |
| `worktree-wf_5ebaf1dc-c6e-3` | `43f09335` | Marke 2, ROOM10E0 C7 Diagonalzelle als Dreieck | **mergen** |
| `worktree-wf_5ebaf1dc-c6e-4` | `9e37e39d` | Marke 3, ROOM1050 C1 Tiefe aus der Zelle | **mergen, Schranke korrigieren** |
| `worktree-wf_5ebaf1dc-c6e-1` | `1902bd10` | Marke 4, ROOM10D0 C7 eigene Huelle + `minus_png` | **NICHT mergen** (§6.1) |

---

## 1. Die vier Marken des Nutzers — je ein Satz

| Marke | Raum / Cut | Antwort in einem Satz | Status |
|---|---|---|---|
| **1** (F162, 14:30:07) | ROOM10D0 C1 | Die Tischplatte und das Gestell **desselben** Klapptischs bekamen Tiefen, die 5732 Welteinheiten auseinanderliegen, weil `"aufrecht": true` die Vorderkante der Platte auf den Fussboden projizierte — der Spieler lag genau dazwischen, 133 seiner Beinpunkte lagen sichtbar auf der Platte. | **beantwortet und behoben** |
| **2** (F185, 14:30:17) | ROOM10E0 C7 | Die Tiefe der Liege kam aus dem **Huellrechteck** einer Typ-5-Diagonalzelle und war damit 1739 Einheiten zu nah; der Spieler stand in der freien Haelfte und wurde in zwei Stuecke zerrissen (43 Bildzeilen ohne einen sichtbaren Figurpunkt). | **beantwortet und behoben** |
| **3** (F239, 14:30:43) | ROOM1050 C1 | Die Spaltenregel las die **eigene Kante der Schreibtischplatte** als Bodenkontakt und schob 21 Maskenspalten bis 3699 Einheiten hinter den Tisch, den sie darstellen — Leons Bein wurde ueber Platte und Akten gemalt. | **beantwortet und behoben** |
| **4** (F3843, 14:36:29) | ROOM10D0 C7 | ⛔ **Unbeantwortet.** Belegt ist nur, dass die Freistellung des Nutzers unangetastet ist (2677 von 2677 Punkten in der `.PBM`, Blob seit `7b24600f` unveraendert) und dass 18 Punkte des blaugrauen Kastens am Tischende die Tiefe des nahen Stuhls (53..55 statt 109..114) tragen. Das erklaert einen hellen Splitter, **nicht** das Loch, das der Nutzer sieht: am gelieferten Abzug sind 240 der 273 Punkte dieses Gegenstands von der Figur uebermalt, und der vorgeschlagene Fix haette die 18 sichtbaren entfernt und 0 hinzugefuegt. | **offen, §5.1** |

Zu Marke 4 gehoert eine Zahl, die der Nutzer wissen soll: sein Verdacht war richtig, dass an
seinem Stuhl etwas fehlt — aber nicht in seiner Datei. 33 % der Deckung dieses Winkels
(1794 von 5435 Punkten) ist **kein** Lasso von ihm, sondern mein eigener Quader
`"Holztisch mit Klappstuhl"` mit einem Rechteck-Abzug `minus 58,0,320,240`. Das ist die
einzige messbare Fremdform in diesem Bild.

---

## 2. Gleiche Ursachen — 11 Befunde, 5 Ursachen

### U1 — Eine waagerechte Flaeche, deren eigene Kante als Bodenkontakt gelesen wird
Marke 1 (Variante `aufrecht`) und Marke 3 (Variante Spaltenregel) sind **derselbe**
Mechanismus: `geometrie.standpunkte` nimmt die unterste opake Zeile und projiziert sie mit
`welt_am_boden(..., y0=0)`; bei einer Platte ist diese Zeile ihre eigene Vorderkante auf
Plattenhoehe, der Sehstrahl laeuft darunter weiter und trifft den Boden weit dahinter.
Aktenkundig ist die Klasse seit 2026-09-04 (ROOM1140 C0, `ebene:-700`) und 2026-09-07
(ROOM10C0 02_02, ROOM10D0 07_02/08/09_02 zurueck auf die Spaltenregel). Marke 1 stand nur
nicht auf der Liste. **Ein Fix-Prinzip, zwei Daten-Eintraege** (`ebene` bzw. `zelle`).
Verdaechtige derselben Klasse, noch nicht adjudiziert: ROOM10D0 C6 `"aufrecht": 170`
(Spiegelbild des Fehlers, Fuss 67 Zeilen **ueber** der Silhouette), ROOM10D0 C8
„Tischkante" und ROOM10C0 C3 (nur mit dem liegenden Koerper sichtbar, Defizit-Median 13
bzw. 51 Buckets).

### U2 — Eine Kollisionszelle als Huellrechteck geraycastet
Marke 2. Behoben generisch fuer Typ 3..7 mit Adresse je Bedingung
(`push_circle` @FUN_8003d6a8, `push_diag4` @LAB_8003beb0, `push_diag5` @LAB_8003c734,
`push_diag6` @LAB_8003cb9c, `push_diag7` @LAB_8003c2cc). Typ 1 bleibt bitgleich (von 16 neu
gebauten Cuts weichen genau die 4 mit Nicht-Typ-1-Zelle ab, alle PBM bitgleich).
⛔ Die uebrigen Typ-3-Faelle (ROOM10F0 C1/C2/C3) sind **kein** Befund und werden **nicht**
neu gebaut — §6.2.

### U3 — Eine Freistellung, die zwei Entfernungen umfasst, bekommt EIN Tiefenmodell
Marke 1 (`01.png` = Platte + Gestell, 5732 Einheiten auseinander; vom Nutzer nicht getrennt,
per Farbregel in zwei Tiefen-Objekte geteilt) und Marke 4 (`07_01.png` = naher Stuhlrahmen +
ferner Kasten, eine einzige 8er-Zusammenhangskomponente ueber 2677 Punkte, Standpunkte 3985
gegen 7337 Kamera-z). Marke 2 ist der gelungene Gegenfall: dort hat **der Nutzer selbst**
`07_02.png` in Wand (`07_03`) und Liege (`07_04`) geteilt — und genau diese Teilung kostet
14 Antialias-Saumpunkte (§3, Schritt 8). Fuer Marke 4 ist die Trennung **offen** und gehoert
dem Nutzer, nicht einer Farbregel (§5.1).

### U4 — Die Messschienen messen am falschen Ort oder die falsche Eigenschaft
Vier Einzelbefunde, eine Wurzel und eine Folgewurzel:

* **U4a, der Ort.** `abnahme.floor_aus_dump` (`abnahme.py:181-185`) liest die `F`-Zeilen der
  Sonde `probe_r16_pri_masken_audit`, und die kommen aus `re15_collision_on_floor`
  (`probe_r16_pri_masken_audit.c:87`) — dem Containment-Scan FUN_8003b7f0, also den
  Zell-INNENRAEUMEN. Die SCA-Zellen sind die Hindernisse; der Spieler laeuft im Komplement.
  `raum.py:606` setzt diesen Dump als Default (`build/p2/dump_vorher.txt`), und der Docstring
  von `floor_aus_dump` nennt die Punkte weiter „begehbar". Gemessen: 20081 von 20081
  Dumppunkten in ROOM10D0, 3350/3350 in ROOM1010, 12915/12915 in ROOM1140, 6636/6636 in
  ROOM10E0 liegen INNERHALB einer soliden bandgleichen Zelle. Die Ersatzsonde existiert seit
  2026-09-19 und ist ausdruecklich formatkompatibel gebaut
  (`probe_p2_floor_dump.c:1-18`, `re15_collision_constrain`, PR=450) — sie wird nur nicht
  benutzt. **Folge:** jede „VORn/VORverd/HINTn/HINTfrei"-Zahl aus `bau_p2`/`raum.py` in
  allen sechs Dossiers beschreibt unerreichbare Orte. Mindestens drei Widerlegungen dieser
  Runde (ROOM2090 C2/C3/C9, ROOM10D0 C1 „20 HINTfrei") haengen allein daran.
* **U4b, die Eigenschaft.** `bau_p2.py:209` schreibt bei
  `ok = fehlt==0 and zuviel==0 and VORverd==0`. Es gibt **kein** Kriterium fuer FEHLENDE
  Verdeckung. Gemessen an ROOM1050 C1: auch mit einem voellig unabhaengigen Bezug (SCA-Quader
  statt Spaltenregel) meldet die Schiene GRUEN (VORverd 0, HINTfrei 113) — die Selbstreferenz
  war nicht der Grund, warum der Fehler durchkam.
* **U4c, das Mass.** `unit_pri_eingemessen` zaehlt **Spalten-Praesenz** („traegt diese Spalte
  irgendeine wirksame Maske"), nicht Punktdeckung; das Gestell liefert in allen 21 Spalten
  eine, also blieb das Mass satt, waehrend 133 Figurpunkte sichtbar auf der Platte lagen —
  an seiner eigenen eingemessenen Marke F423 im selben Raum und Cut.
* **U4d, die Abdeckung.** Im kaputten Zustand waren sechs PRI-Riegel gruen
  (`unit_pri_eingemessen`, `unit_pri_silhouette`, `unit_pri_hashes`, `unit_pri_kopfschnitt`,
  `test_pri_inventar_riegel`, `integration_pri_masken`). `unit_pri_silhouette` sieht 77 von
  323 Sektionen (23,8 %) und genau die vier Kasten-Cuts nicht, weil `bau_p2` dort nie lief
  und sie deshalb keine `.PBM` haben.

### U5 — Silhouetten-Transport und Bestand
* `bau_p2` wertet `nur_kunst` nicht aus (`grep`: der Schluessel existiert nur in
  `raum.py:413`). Latent, aber scharf: ein Neubau von ROOM10F0 C4/C5 wuerde die rohen
  Quader schreiben — 31757 statt 12068 (Faktor 2,6) und 21469 statt 9285 (2,31) — und die
  Abnahme laesst das mit `ok=True` durch. Einzige Schranke heute ist
  `raum.P2_UNANGETASTET` (`raum.py:55-60`).
* `ALPHA_SCHWELLE = 110` (`bau_p2.py:32`, im Code als Heuristik gekennzeichnet) wirkt global.
  Ueber 89 Cuts 21979 von 1076165 Punkten = 2,04 %, aber ROOM1060 C7 23,35 %, C4 22,87 %,
  C0 22,25 %, ROOM10D0 C1 19,48 %. Maximale Dicke der verworfenen Menge = 1 und keine
  abgesetzte Komponente >= 8 Punkte: ein Saum, kein verlorenes Teil — bei einem 2-3 Punkte
  breiten Gelaenderrohr aber bis zwei Drittel seiner Breite.
* Bestand: 224 von 323 Sektionen haben keinen Atlas und zeichnen nichts (kein Bildfehler,
  `main.c:4976` verhindert Overdraw); ROOM2091 hat keinen `auswahl.json`-Eintrag;
  ROOM1000 C1, ROOM1000 C3 und ROOM11F0 C1 haben bewusst leere Objektlisten.

---

## 3. Fix-Plan in Reihenfolge

Reihenfolge = was der Nutzer sieht, dann was das Messen ueberhaupt erst belastbar macht,
dann Bestand. Schritt 4 ist absichtlich VOR allen weiteren Neubauten: solange die Abnahme
an unbegehbaren Orten misst, ist jede neue Zahl wertlos.

### Schritt 1 — Marke 2 mergen (ROOM10E0 C7)
* **Aenderung:** `43f09335` — `geom.zellen_solid_test`/`zellen_schnittflaechen` +
  `geometrie.quader_auf_band(typ=…)`; neu gebaut `ROOM10E0.MSK` Sektion 7,
  `ROOM10E0_PRI07.TIM/STAND`. `PBM` unveraendert.
* **Beleg:** SCA-Eintrag 21 @`ROOM10E0.RDT` Datei-Offset `0x758`, 12 Bytes
  `22 0b 74 0e 8c f1 88 fa 05 ff 00 03` (w=2850, d=3700, x=-3700, z=-1400, Typ 5, solide,
  Band 0); Solidbedingung `push_diag5` @LAB_8003c734 (`ghidra1_V2.txt:144830-145138`).
  An px=-2360 beginnt die solide Flaeche bei z=+340, die Huellkante bei z=-1400 =
  **1739 Einheiten zu nah**; Spieler bei z=-164, also 503 Einheiten davor.
* **Riegel:** `unit_r19_zerreiss_10e0c7` (Zerreiss-Schiene, Abdeckung 112 Figurspalten ueber
  4 Standorte) mit Gegenprobe: alter Satz 26/26/18/25 zerrissene Spalten, neuer Satz 0.
* **Erkennungsmass:** verdeckte Koerperkastenpunkte an F185 **1661 -> 21**; die 43
  aufeinanderfolgenden Bildzeilen y128..y170 mit NULL sichtbaren Figurpunkten verschwinden.
  ⛔ Die im Dossier tragende Zahl „1140 geloeschte Figurpunkte" ist **gerechnet**, nicht
  gemessen — vor dem Zitieren durch 1661/21 ersetzen.
* **Vor dem Merge in das Dossier nachtragen:** (a) die Restverdeckung an F523 kommt vom
  **Pult** (feste Nutzertiefe 50), nicht von der Liege; (b) die Tabellenzeile „t=90 ferner
  als die Figur, richtig frei" ist falsch — t=90 ist naeher als die Figurachse, das echte
  Mesh liegt 1..2 Stufen naeher als die Senkrechte, der Riegel ist dadurch strenger als die
  Wirklichkeit und bleibt gueltig; (c) `HINTfrei 274 -> 385` ist nicht die „notwendige
  Kehrseite", sondern harmlos, weil die ferne Seite der Typ-5-Zelle durch die Typ-1-Zelle
  x-5000..-600 / z2300..3100 geschlossen ist: dort ist keine Standflaeche.
* **Nebenbefund, eigener Posten:** das Original addiert vor dem Diagonaltest einen
  Offset-Vektor auf die Position (@8003c790 `lw v0,-0x387c(v0)`, @8003c7a0 `lw a0,0x7c(v0)`,
  @8003c7a8 `lh t0,0x0(a0)` -> `addu t9,t0,s5`; @8003c814 `lh a0,0x4(a0)` ->
  `addu t7,a0,s1`). Der Port laesst ihn weg (`re15_collision.c:529`,
  `int32_t PX = *lx, PZ = *lz;`). Ohne Wirkung auf die Zellform, aber eine unbenannte
  Abweichung in genau der Funktion, die als Beweis zitiert wird — gehoert in das
  Kollisions-Backlog, nicht in diesen Fix.

### Schritt 2 — Marke 3 mergen (ROOM1050 C1)
* **Aenderung:** `9e37e39d` — `"zelle": [16100,1500,1100,4400]` fuer ROOM1050 C1 Objekt 01;
  Sektion Cut 1, TIM, STAND neu. `PBM` bitgleich (XOR 0).
* **Beleg:** SCA-Eintrag 11, Typ 1, x16100..17200 / z1500..5900, Band 0
  (Datei-Byte-Offset `0x5EC` in `ROOM1050.RDT`) — das ist der **byte-true** Teil.
  Leon bei z=6350 steht 450 Einheiten hinter dem Tischende, groesseres z ist gemessen weiter
  weg (Spalte 198 -> z6358 vz5644; Spalte 202 -> z5993 vz5292).
  ⛔ Die Quaderhoehe **-1575 traegt keine Adresse**: sie ist an die Freistellung gefittet
  (IoU-Maximum 0,809, Nullmodell 0,10@-100 -> 0,80@-1600 -> 0,62@-2000, echtes Maximum).
  Sie muss im Eintrag als „an der Kunst gemessen" beschriftet werden. Der Befund braucht sie
  nicht: die Schranke folgt allein aus x/z und dem Kamerasatz.
* **Riegel:** `unit_r19_marke3_1050_c1_tiefe` — keine Maskentiefe darf hinter der fernsten
  Ecke der eigenen SCA-Zelle liegen. Gegenprobe: altes MSK 31 von 100 Rechtecken ueber der
  Grenze, groesste Tiefe 136 -> EXIT 1; neues 0 von 92, groesste 79 -> EXIT 0.
  **Vor dem Merge korrigieren:** die fernste Ecke (17200,0,5900) hat Kamera-z **5246**, also
  Eimer **81**, nicht 5261/82 — die Schranke ist derzeit einen Eimer lockerer als beweisbar.
* **Erkennungsmass, ohne freien Parameter:** die Marke nennt die Dreieck-Spanne selbst
  (Kamera-z 4704..5665). Eine Maske verdeckt alle Dreiecke bei Tiefe <= 72 und keines bei
  >= 88. Spalten 184..194 verdeckten vorher **beweisbar kein einziges** Dreieck, nachher
  liegt jede Spalte ueber dem Koerper bei <= 72. Die Punktzahlen (263 / 9 / 64 / 190) sind
  schwellenabhaengig (ein unabhaengiger Lauf mit Diff-Schwelle 18 gibt 364 / 42 / 98 / 224)
  und muessen mit ihrer Schwelle zitiert werden.
* **Streichen:** der offene Punkt „17 HINTfrei auf der linken Tischkante". Alle 17 liegen bei
  x16100..16300 / z3250..5250, also INNERHALB der byte-verifizierten Sperrzelle — unerreichbare
  Lagen, kein Restposten. Ebenso die Tabellenzeile „Tischplatte ferne Kante -> 4953..5062":
  (16100,-1575,5900) gibt 4750, (17200,-1575,5900) gibt 4859; 4953 gehoert zu einer
  Deckelhoehe von etwa -750.
* **Kosten, bewusst akzeptiert:** Atlasflaeche 28007 -> 43715 Punkte (Fuellrate 31,0 % ->
  19,9 %), Grenzen gehalten (92 <= 105 Rechtecke, 43715 <= 65536). Wer diesem Cut spaeter
  ein Objekt hinzufuegt, prueft zuerst die Atlasflaeche.

### Schritt 3 — Marke 1 mergen (ROOM10D0 C1), Riegel vorher verengen
* **Aenderung:** `e7b4063c` — `"aufrecht": true` -> `"ebene": -1525` + `"zelle": false` an
  der Platte; Sektion Cut 1, TIM, STAND neu. `PBM` byte-identisch.
* **Beleg:** Nahtprobe — 70 Plattenpunkte grenzen an 55 Gestellpunkte; Tiefensprung ueber die
  eigene Naht Median **85 Stufen** (`aufrecht`) gegen **2 Stufen** (`ebene:-1525`), waehrend
  die Kuenstler innerhalb EINES Rechtecks median 9 Buckets streuen. Plattenebene dreifach
  gemessen: Grundriss-Deckung mit dem eigenen Gestell (Minimum bei -1525, Median |dz| 132),
  28 Spalten einzeln (Median Y -1515, sd 65, Verfahren am Spieler geeicht: Log-vz 12057 ->
  Y=+34), Vorwaertsprojektion der SCA-Deckflaeche x2900..4200 / z-4100..3800.
  Bildzensus im Koerperkasten x150..170 / y82..117: 133 von 255 gezeichneten Figurpunkten
  (52 %) liegen sichtbar auf der Platte, 0 von 162 Gestellpunkten zeigen Figur.
  ⛔ Die 133 haengen an der Diff-Schwelle (>16: 133, >24: 82, >32: 31, >40: 7) — Schwelle
  mitzitieren.
* **Zwei Pflicht-Aenderungen vor dem Merge:**
  1. **Riegel verengen.** `test_r19_marke1_10d0_c1.c` Teil 1 laeuft heute ueber **alle** Masken
     des Cuts und verbietet jeder eine Tiefe >= 181 (Kopf des Spielers an EINER Marke).
     Am fernen Korridorende dieses Winkels stehen Schreibtisch und Pflanze bei Kamera-z um
     13000; bekommt einer davon spaeter eine Freistellung, faellt der Riegel zu Recht gebaute
     Masken. Richtig ist die Form des Marke-3-Riegels: Schranke aus der **eigenen** SCA-Zelle
     der Tischreihe (x2900..4200 / z-4100..3800, fernste Ecke aus dem RDT) und nur fuer die
     Masken, die die Plattensilhouette decken.
  2. **Ebene mit Unsicherheit und Referenz eintragen:** `-1525 +- 75`, Referenz benannt.
     Das Nullmodell „nur 2 von 97 Hoehen innerhalb einer Tiefenstufe" wird auf der
     1-D-Statistik Median|dz| gemessen, die steiler auf h reagiert als die Tiefe selbst;
     ein unabhaengiger Fussabdruck-IoU legt das Optimum gegen die Gestell-Fusslinie auf
     -1600 (Plateau -1550..-1625) und gegen die SCA-Zelle auf -1525 (Plateau -1475..-1600).
     100 Einheiten Hoehe sind in den betroffenen Zeilen 8..11 Tiefenstufen. Fuer den Befund
     ohne Belang (jede Hoehe des Bandes verdeckt die Beine), fuer den Eintrag nicht.
* **Ehrlich zum gewaehlten Modell:** die Spaltenregel auf der Ebene -1525 liegt systematisch
  zu nah. Exakter Sehstrahl-Schnitt mit der Ebene gibt 105..173, gebaut ist 102..162,
  Differenz Median -18 Stufen (Mittel -20,8, Max -71), 447 von 493 Punkten weichen um mehr
  als eine Tiefenstufe ab. Ueber alle Standplaetze verdeckt das gebaute Modell an 18 von 711
  beruehrten Plaetzen **mehr** als die Physik (219 Punkte), nie weniger. Das ist die sichere
  Richtung und bleibt so, gehoert aber in den Eintrag; die Naht ist ein Konsistenzmass, kein
  Lagemass.
* **Erkennungsmass:** engine-seitig `unit_pri_kopfschnitt` (Phase 2, Klemmpfad — verifiziert,
  s. §4 W1): ROOM10D0 C1 HINTfrei **91 -> 1**, spielweit **1011 -> 921**. Dieser eine Cut
  trug 90 der 1011 spielweiten Fehlstellen.
* **Als Beleg streichen:** die Generator-Zahlen (5796 Standplaetze / 809 beruehrt,
  VORn 325 -> 104, HINTfrei 289 -> 20). Sie stammen aus dem `on_floor`-Dump (U4a); fuer
  ROOM10D0 liegen 20081 von 20081 dieser Punkte in soliden Zellen. Der Fix selbst steht auf
  Naht, Hoehenmessung und Bildzensus und ist davon unberuehrt.

### Schritt 4 — Die Abnahme auf den Klemmpfad umstellen (Werkzeug, blockiert alles Weitere)
* **Aenderung:** `raum.py:606` `--dump`-Default auf den Klemmpfad-Dump
  (`probe_p2_floor_dump`) umstellen, Docstring von `abnahme.floor_aus_dump`
  (`abnahme.py:181-185`) korrigieren — er nennt `on_floor`-Punkte weiter „begehbar" —, und
  die Protokollbefehle in `phase3_pri-kreativ/protokoll/bau_alle.sh` nachziehen.
* **Beleg:** `probe_r16_pri_masken_audit.c:87` `re15_collision_on_floor` = Containment-Scan
  FUN_8003b7f0 gegen `probe_p2_floor_dump.c:75` `re15_collision_constrain` = FUN_8003b0a4,
  PR=450. Die Ersatzsonde ist ausdruecklich formatgleich gebaut („Ausgabe wie die alte Sonde
  (ROOM / B / F / BN), damit tools/maske/abnahme.floor_aus_dump sie liest"), also ein
  Ein-Zeilen-Wechsel. `test_pri_kopfschnitt.c` hat denselben Wechsel 2026-09-19 fuer seine
  Phase 2 schon vollzogen und dokumentiert ihn im Kopf.
* **Riegel:** neuer ctest — jeder `F`-Punkt des Dumps, den die Abnahme benutzt, muss
  ausserhalb jeder soliden bandgleichen Zelle liegen; Abdeckung = Zahl der geprueften Raeume
  und Punkte im Testausdruck.
* **Erkennungsmass:** Anteil der Dumppunkte in einer soliden bandgleichen Zelle
  **100 % -> 0 %** (heute ROOM10D0 20081/20081, ROOM1010 3350/3350, ROOM1140 12915/12915,
  ROOM10E0 6636/6636). Zweitens: Abstand des naechsten Satzpunkts zur echten Nutzerposition
  (418,0,26497) **518 -> 135** Einheiten.
* **Folge, die mitgeschrieben werden muss:** alle „Standplatz"-Zahlen der sechs Dossiers
  sind danach neu zu erheben. Ohne diesen Schritt ist jede weitere VOR/HINTER-Zahl wertlos.

### Schritt 5 — `nur_kunst` in `bau_p2` nachziehen (Werkzeug, latent scharf)
* **Aenderung:** Silhouette eines `quader`-Objekts mit `nur_kunst` =
  `Quader ∩ ((kunst_label == eigene Zelle) | (dunkel_label == eigene Zelle))`, wie
  `raum.py:413-429`. **Bis dahin** in `bau_p2.bau_cut` ablehnen — mit
  `return {"fehler": 'quader "%s" traegt nur_kunst — in bau_p2 nicht ausgewertet', ...}` wie
  die anderen Ablehnungen (`bau_p2.py:148/171/176`), **nicht** `SystemExit`:
  `raum.py:775` ruft `bau_cut` ohne `try` und ein `SystemExit` reisst den Bau des ganzen
  Raums ab.
* **Beleg:** `grep -n nur_kunst re15_port/tools/maske/*.py` -> nur `raum.py:413`
  (selbst nachgeprueft). Rohbau gegen Auslieferung: ROOM10F0 C4 31757/12068 (Faktor 2,6),
  C5 21469/9285 (2,31); ROOM1100 C1/C2 bitgleich (28160/28160, 25211/25211), also von diesem
  Punkt nicht betroffen.
* **Warum es scharf ist:** hinter `P2_UNANGETASTET` steht **keine** zweite Schranke. Der
  Rohbau erreicht `fehlt=0, zuviel=0, VORverd=0, VORteil=0, ok=True` und wuerde
  **geschrieben** — bei 733 bzw. 905 messbar schlechter gewordenen Standplaetzen. Er wuerde
  ausserdem die falsche Soll-Silhouette als `.PBM` einzementieren.
* **Erkennungsmass:** ein Rohbau mit `nur_kunst`-Quader endet mit `fehler` statt `ok=True`;
  nach dem Port ist der Neubau von C4/C5 bitgleich zur Auslieferung (12068 / 9285).
* ⛔ **Kein Fix fuer Befund 1 der Kasten-Runde:** der `dunkel_label`-Zweig
  (Kanalsumme < 45, `raum.py:327`) ist genau die Regel, die dort als sichtbarer Defekt
  gemeldet wurde — und die ist widerlegt (§6.3). Der Port stellt nur den Auslieferungsstand
  her.

### Schritt 6 — `unit_pri_eingemessen` auf ein Punktmass umstellen
* **Aenderung:** nicht auf „Figurpunkte ohne Maske" (der Vorschlag des Dossiers ist so nicht
  umsetzbar: ein Unit-Test hat keine gezeichnete Figur, nur den Kasten `sx +- 450`; als
  Kastenpunkte gelesen bleiben 211 (F423), 304 (F162) und 151 (ROOM1130 C3) Punkte ohne
  jede Maske — auch im reparierten Zustand FAIL, und ROOM1130 C3 wuerde zu Unrecht rot).
  Umsetzbar und trennend ist: **Maskentexel im Koerperkasten eines Objekts, hinter dem der
  Spieler steht, die NICHT wirksam sind** — gemessen 290 (F162), 202 (F423), **0**
  (ROOM1130 C3).
* **Abdeckung, offen im Testausdruck:** `MESS[]` in `test_pri_eingemessen.c` enthaelt
  **2 Marken in 2 Raeumen**, nicht 103 Raeume — das ist 2 von 439 Winkeln mit Masken =
  **0,46 %**. Die Breite taugt nicht als Grund, die Umstellung zu verschieben.
* **Erkennungsmass:** Gegenprobe mit zurueckgedrehtem Marke-1-Modell: der Riegel muss
  EXIT 1 geben (heute EXIT 0 bei 133 sichtbaren Figurpunkten auf der Platte).

### Schritt 7 — 9 beitragslose Kaesten entfernen (niedrig, risikolos)
* **Aenderung:** aus `auswahl.json` entfernen: ROOM10F0 C4 Buerostuhl x-1600 z12200 /
  x-1800 z8200 / x-1900 z4300; C5 x1600 z5700 / x1600 z1900 / x1500 z-2000;
  ROOM1100 C1 Suedblock + Suedwestwand; ROOM1100 C2 Nordwand.
* **Beleg:** je Objekt ALLEIN 0 und „liefert die naechste Tiefe an 0 Punkten". Die sechs
  Stuhl-Kaesten sind bitgleiche Doppel der `tiefe:"szene"`-Lassos (`raum.py:552-560` gibt
  jeder Lasso-Komponente bereits den Quader ihrer Zelle); Suedblock/Suedwestwand/Nordwand
  liegen vollstaendig in der Westwand-Silhouette, und die Westwand ist an jedem gemeinsamen
  Punkt naeher (28160/28160 bzw. 25211/25211).
* **Erkennungsmass:** Deckung bitgleich, Rechtecke ROOM1100 C1 102 -> 83 und C2 105 -> 76,
  Atlaspunkte 56542 -> 31430 bzw. 42820 -> 28480, Kachelkanten ROOM10F0 C4 16/24 -> 12/12
  (49 -> 54 Tiefenstufen), C5 20/16/20 -> 16/12/12 (41 -> 47).
* ⛔ **Ehrlich:** das Fehlurteil aendert sich kaum (C4 3837 -> 3828, C5 2423 -> 2276,
  ROOM1100 0 -> 0). Ballast, kein sichtbarer Fehler. Deshalb Rang 7 und nicht frueher.

### Schritt 8 — `ALPHA_SCHWELLE` je Objekt (niedrig, nach Schritt 3)
* **Aenderung:** `ALPHA_SCHWELLE` je Objekt zulassen und fuer die duennen Gegenstaende
  (ROOM1060 C0/C1/C2/C4/C7, ROOM10D0 C1/C8) auf 1 setzen. Im gleichen Zug die Sonderregel
  fuer **geteilte** Freistellungen: stammen mehrere Objekte aus EINER Datei, die Alphawerte
  addieren und erst dann schwellen (die 14 Saumpunkte an der `07_02`-Naht in ROOM10E0 C7
  haben in `07_02` Alpha 111..200, in jeder Teildatei einzeln 20..107).
* **Beleg:** ueber 89 Cuts 21979 von 1076165 Punkten verworfen; ROOM1060 C7 23,35 %;
  maximale Dicke der verworfenen Menge 1, keine abgesetzte Komponente >= 8 Punkte.
  `07_01.png` (Marke 4) hat binaeres Alpha, dort verwirft die Schwelle 0 Punkte — sie ist
  fuer Marke 4 nicht die Ursache.
* **Kopplung, die vor dem Bau geprueft werden muss:** fuer ROOM10D0 C1 aendert das die
  Soll-Silhouette (1021 Punkte) und damit `ROOM10D0_PRI01.PBM`, den Marke-1-Riegel
  (`SOLL_PUNKTE 1021`), `unit_pri_silhouette` und die Hash-Liste. Deshalb strikt nach
  Schritt 3 und mit neu erzeugter `pri_stage1_hashes.inc`.
* **Erkennungsmass:** Deckung der betroffenen Cuts == Freistellung bei Alpha > 0,
  Differenz 0; Rohrbreite im Bild der Freistellung gleich (heute 1 Punkt schmaler).

### Schritt 9 — Bestand ehrlich machen (kein Bildfehler)
`.PBM` bei jedem Bau mitschreiben (damit die vier Kasten-Cuts in `unit_pri_silhouette`
fallen, heutige Abdeckung 77 von 323 = 23,8 %, 246 Container-Cuts ohne PBM);
die 224 toten Sektionen entweder bauen oder die Container von ROOM1221 und den 32
STAGE3-6-Raeumen entfernen; ROOM2091-Eintrag anlegen (Spiegel von ROOM2090, `.RDT` und
`_PRI03.TIM` sind md5-identisch mit ROOM2090).

---

## 4. Widersprueche — wer traegt und warum

**W1 — Ist `unit_pri_kopfschnitt` auf dem falschen Standplatz-Satz?**
Die Skepsis zu Marke 1 sagt, auch die Engine-Schiene („91 -> 1", „1011 -> 921") stamme aus
dem invertierten Satz. Die Skepsis zu ROOM10F0 sagt, Phase 2 dieses Tests benutze den
Klemmpfad. **Selbst nachgesehen:** `test_pri_kopfschnitt.c` Phase 2 ruft
`re15_collision_constrain(&rdt, gx, gz, &x2, &z2)` und verwirft jeden Punkt, den der
Klemmpfad verschiebt; der Kopfkommentar (Zeilen ~95-105) verbietet `on_floor` ausdruecklich
und laesst nur Phase 1 darauf stehen, benannt als Verhaeltnis-Pin.
**-> Die ROOM10F0-Skepsis traegt.** Die Engine-Zahlen von Marke 1 sind gueltig, die
Generator-Zahlen (aus `abnahme.floor_aus_dump`) nicht. Das ist gemessen, nicht argumentiert.

**W2 — Verliert `raum.py` beim Neubau die Sektionen ROOM10F0 C4/C5?**
Das Marke-2-Dossier begruendet damit, ROOM10F0 nicht einzubauen. **Widerlegt:** ein echter
Lauf gegen eine Kopie des Auslieferungsverzeichnisses liefert eine **byte-identische**
10008-Byte-Datei mit allen sieben Sektionen (86/36/27/35/105/105/102); der gemeldete
6340-Byte-Container entsteht nur, wenn man mit `--out` in ein **leeres** Verzeichnis baut —
genau das hat der Lauf getan. **-> Die Widerlegung traegt.** Der richtige Grund, ROOM10F0
C4/C5 nicht neu zu bauen, ist die `nur_kunst`-Luecke in `bau_p2` (Schritt 5), nicht die
`alt`-Erkennung.

**W3 — „Die ausgelieferten 10F0-Sektionen stammen aus einem anderen Bau-Stand (36/27/35
gegen 41/34/50)."** Widerlegt: der heutige Werkzeugstand reproduziert 36/27/35 und alle
neun Dateien (TIM/PBM/STAND) **bitgleich** (md5). Die 41/34/50 sind die Wirkung der
Zellform-Aenderung selbst. **-> Kein zusaetzliches Einbaurisiko.**

**W4 — Sieht der Nutzer an Marke 4 den 18-Punkt-Splitter oder ein Loch?**
Das Marke-4-Dossier erklaert die 18 Punkte zum Befund. Die Gegenmessung am **gelieferten
Abzug** zaehlt: von 273 Punkten des Gegenstands sind 240 im Render uebermalt, 33 sichtbar,
davon 18 gedeckt. Der Fix entfernt genau diese 18 und fuegt 0 hinzu — der Gegenstand ist
danach zu 94,5 % statt 87,9 % unsichtbar. **-> Die Gegenmessung traegt** (sie misst das
Artefakt, nicht das Modell). Folge: Marke 4 ist **nicht** beantwortet, und die Aenderung
darf nicht in den Auslieferungsstand.

**W5 — Hat eine „selbstbestaetigende Metrik" den Fehler von Marke 3 durchgelassen?**
Nein. Die Schiene laeuft mit einem voellig unabhaengigen Bezug (SCA-Quader, nichts aus der
Spaltenregel) ebenfalls auf GRUEN: VORverd 0, HINTfrei 113 — und HINTfrei ist kein Tor
(`bau_p2.py:209`, `test_pri_kopfschnitt.c:308`). **-> Die Widerlegung traegt.** Wichtige
Konsequenz fuer den Plan: den Bezug unabhaengig zu machen faengt diese Fehlerklasse
**nicht**. Gebraucht wird ein Kriterium fuer FEHLENDE Verdeckung gegen eine benannte Zelle —
also der Marke-3-Riegel, verallgemeinert (Schritt 6 und §5.6).

**W6 — Ist `HINTfrei 2 -> 36` an ROOM10D0 C7 ein Fehler der Kennzahl?**
Die Begruendung traegt nicht: die zitierte „Standlinie Kasten 7337" kommt in `stand[]`
nirgends vor (Maximum ueber alle 320 Spalten 4496, in den strittigen Spalten 46..72 sind es
7042..7331 fuer den Kasten und 3714..3983 fuer den Stuhl); die zwoelf Beispiele sind die
Druck-Trunkierung `hint_rest[:12]`, zwei davon standen vorher schon in der Liste; das erste
Beispiel (-1500,24950) traegt 25 unverdeckte Punkte des **Stuhls**, der Kasten steuert null
bei; objektweise ausgewertet bleiben 4 statt 0 Faelle, und die VOR-Pruefungen vervielfachen
sich (dieser Cut 21 -> 70, ROOM1100 C1 170 -> 868). **-> Die Widerlegung traegt.**
`abnahme.py` wird in dieser Runde **nicht** auf objektweise umgebaut.

**W7 — Nicht adjudiziert, aber nicht vergessen:** die Befunde B5 (ROOM10D0 C6 „Pflanze
links", `"aufrecht": 170`, benoetigte Ebene +1963..+2576 unter dem Boden, drei unabhaengige
Masse und die einzige VORverd-Meldung in STAGE1), B6 (224 tote Sektionen), B7 (ROOM2091 ohne
Eintrag) und B9 (Kasten statt Freistellung) hat **kein** Skeptiker geprueft. B5 ist der
staerkste verbleibende STAGE1-Kandidat und gehoert vor dem naechsten Bau vor einen
Gegenlauf — er ist derselbe Mechanismus wie U1, nur spiegelbildlich.

---

## 5. Was OFFEN bleibt — und was es braucht

### 5.1 Marke 4, ROOM10D0 Cut 7 (der Punkt, der den Nutzer am meisten stoert)
Die Freistellung `07_01.png` ist **eine** Zusammenhangskomponente ueber **zwei**
Entfernungen: naher Stuhlrahmen (Standpunkt Kamera-z 3985, Tiefe 53..62) und ferner
blaugrauer Kasten am Tischende (eigener Bodenkontakt in den Spalten 58/62/67 auf den
Bildzeilen 158/155/154 -> Kamera-z 7456/7706/7793, Tiefe 116..121; Nullmodell: ein
Gegenstand am Spielerfuss haette seinen Sockel auf Bildzeile 179). Ein Tiefenmodell kann
das nicht trennen, und meine automatische Trennung ist nicht zulaessig: die Farbregel
`b-r >= 0 UND r+g+b >= 120` markiert innerhalb der Nutzer-Freistellung 291 von 2677 Punkten
in 23 Komponenten und ist dieselbe Regel, die die Huelle erzeugt hat — sie bestaetigt sich
selbst. Der dunkle Fuss des Kastens unterhalb Bildzeile 159 ist farblich nicht vom Boden zu
trennen.
**Gebraucht:** eine Freistellung des Nutzers, die im Bereich x46..72 / y126..159 den fernen
Kasten vom nahen Stuhlrahmen trennt (zwei PNG statt einem). Bis dahin bleibt ROOM10D0 C7
**wie ausgeliefert**; die 18 zu nahen Punkte sind ein Splitter, kein Loch, und ihre
Entfernung wuerde das gemeldete Bild verschlechtern.
**Zweiter offener Posten im selben Cut:** mein Quader „Holztisch mit Klappstuhl"
(1794 Punkte = 33 % der Deckung) ist keine Kunst. Auch dafuer waere eine Freistellung die
Vorgabe.
**Weitergabe aus dem Kasten-Zensus:** an Band 0 (-900, 25750) urteilt die ausgelieferte
Maske 209 Punkte zu VERDECKT gegenueber der exakten Punkttiefe derselben Objekte. ⛔ Dieser
Standplatz stammt aus dem `on_floor`-Satz und muss nach Schritt 4 gegen den Klemmpfad
nachgemessen werden, bevor er zaehlt.

### 5.2 Fehlende Freistellungen, die nichts ersetzen kann
* **ROOM1000 Cut 3** und **ROOM11F0 Cut 1** — wie seit Runde 18 offen. Ihre
  Auswahl-Eintraege tragen bewusst leere Objektlisten, weil die vorher verwendeten
  Freistellungen nachweislich zu einem anderen Cut gehoerten. Keine Ersatzmaske bauen: eine
  falsch verortete Maske ist schlechter als keine.
* **ROOM10F0 C4/C5** — die 12 Buerostuhl-Kaesten sind ohne Lasso fuer diese zwei Winkel
  nicht ersetzbar; der morphologische Ersatz ist widerlegt (§6.3).
* **ROOM2090 C8/C9** — hier braucht es eine **Entscheidung**, keine Zeichnung: die zwei
  Lassos gehoeren nachweislich zu anderen Cuts (`08.png` Gipfel-IoU 1.000 in Cut 9 bei
  Massstab 1 mit nur 8 gleich guten Lagen; `09.png` 1.000 in Cut 11 mit 2 gleich guten
  Lagen; der `_warum`-Block in `auswahl.json` sagt es woertlich), und **beide enthalten die
  begehbare Deckflaeche des Betonpodests**, deren Maskierung der Nutzer am 2026-09-12
  abgelehnt hat. Empfehlung: wie ROOM1000 C1/C3 behandeln — leere Objektliste, Sektion aus
  `ROOM2090.MSK` entfernen, `_PRI08.TIM`/`_PRI09.TIM` loeschen. Ein Umhaengen auf Cut 9/11
  ist ohne Rueckfrage nicht erlaubt.

### 5.3 Nicht mehr offen, ausdruecklich zurueckgezogen
Die **Rueckfrage nach einer Freistellung des VORDEREN Klapptischs in ROOM10D0 C1** ist
**gestrichen**. Sie stuetzte sich auf 20 HINTfrei-Plaetze, die alle in der Tischzelle
(2900,-4100,1300,7900) liegen, und auf eine Farbregel, deren Belegzahl mit 5 Einheiten
Schwellenverschiebung um 97 % faellt (th+5: 3947, th+10: 2254, th+15: 64, th+20: 4).
Am Bild: im gesamten Vollbild liegen **0** gezeichnete Figurpunkte auf der maskenlosen
Vorderplatte, und die Vereinigung aller 15 gemessenen Koerperkaesten schneidet sie in
36 von 1968 Punkten (1,8 %) — alle leer. Eine Maske dort waere zudem riskant: an der
gemessenen Position (2322,0,3771) steht der Spieler klar VOR dem vorderen Tisch.

### 5.4 Der Ebenen-Riegel aus Zensus B
Er braucht keine Toleranz und keine Zielmenge (`vz` ist linear in `y0`; am untersten
Maskenpunkt jeder Spalte wird gefragt, welche Ebene die geschriebene Tiefe erklaert), und
seine Selbstpruefung trifft in 6 von 8 Faellen die in `auswahl.json` **deklarierte** Ebene
auf unter 140 Einheiten. Heute fallen 3 von 99 Cuts: ROOM10D0 C6 (unadjudiziert, §4 W7),
ROOM2090 C3 und ROOM2091 C3 (die geometrische Anomalie steht, ihre Wirkung ist widerlegt —
§6.4). Er kann deshalb **nicht** als Tor registriert werden, sondern als Messung mit
benannter Ausnahmeliste; Tor wird er, wenn B5 entschieden und STAGE2 geklaert ist.

### 5.5 Byte-true-Luecke ausserhalb PRI
Der fehlende Offset-Summand in `push_diag5` (§3 Schritt 1) — eigener Posten im
Kollisions-Backlog.

### 5.6 Das Kriterium, das allen sechs Laeufen gefehlt hat
Keine Schiene fragt: **„deckt die Objektmenge den gemalten Gegenstand, und sitzt jede Tiefe
an seinem eigenen Bodenkontakt?"** Die Abnahme prueft nur Deckung gegen die eigene
Objektmenge (selbstbezueglich) und Falsch-Positive (`VORverd`). Beide Marken 1 und 3 waeren
mit dem verallgemeinerten Marke-3-Riegel gefallen: fuer jedes Objekt mit IoU >= 0,6 gegen
eine SCA-Zelle darf keine Maskentiefe hinter der fernsten Ecke dieser Zelle liegen — Zelle,
Kamerasatz und Tiefen aus RDT/MSK/TIM, nichts aus dem Bauwerkzeug. `hoehe_messen` fuer alle
146 Objekte laufen zu lassen und die Treffer einzutragen ist der naechste Schritt nach
Schritt 4; in dieser Runde nicht gemessen und deshalb nur benannt.

---

## 6. Was NICHT geaendert werden soll — mit Messung

### 6.1 ROOM10D0 C7: die Marke-4-Aenderung nicht mergen
Commit `1902bd10` (neue Huelle `pri/STAGE1/10D0/07_03.png`, `minus_png` in `bau_p2`,
zwei Riegel, neue Hash-Liste) bleibt draussen.
* Am Artefakt gemessen: 240 der 273 Huellenpunkte sind im Render uebermalt, 33 sichtbar,
  davon 18 gedeckt. Wirkung des Fixes: **-18, +0**.
* Der Nutzen ist modellseitig: fuer keinen der 163 „besser verdeckten" Standplaetze existiert
  ein Screenshot, und das Vorzeigebeispiel (1700,27950) ist gar kein Standplatz — ROOM10D0
  hat 43147 begehbare Punkte, der naechste liegt 600 Einheiten entfernt, im beanspruchten
  Fussbereich x1550..2050 / z27800..28050 liegen 0. Der Punkt projiziert auf Bild (60,157),
  also in den Sockel des Gegenstands.
* Die Abnahme zerfaellt gleichzeitig um Faktor 18 (HINTfrei 2 -> 36) und die noetige
  Behebung ist nach eigener Angabe nicht gemacht — die Aenderung ist nur unter der Annahme
  richtig, dass die Abnahme falsch ist, und diese Annahme ist widerlegt (§4 W6).
* Verwertbar bleibt: die 18 Punkte tragen messbar 53..55 statt 109..114 (56 Buckets, weit
  ueber der Zeichnerauflösung 64,0625), und die Datenaenderung ist kollateralfrei (von 5417
  gedeckten Punkten ausserhalb der Huelle aendert keiner seine Tiefe). Das gehoert in die
  Rueckfrage an den Nutzer (§5.1), nicht in die Auslieferung.
* Ebenfalls nicht mergen: `probe_r19_koerperkasten` als **Riegel**. Der Koerperkasten
  (1700 in `main.c:5262`, `KOPF = 1500` in `abnahme.py:30`) hat keine `@0x`-Grundlage, das
  Original entscheidet je Dreieck (`otz>>4` @0x8002565c, ZSF3=341 @0x80066c70/74).
  In `befund.log` ist er nur der **Listenfilter** (`main.c:5307-5308`), das Urteil kommt aus
  `re15_pri_mask_occludes(rd[q], pvz)` mit der Fuss-Tiefe; an Marke 4 fuegt ein 3000er
  Kasten **0** Masken hinzu (kleinstes Masken-y 123, Kastenoberkante 118), an Marke 1 und 3
  ebenfalls 0, nur an Marke 2 15. Ein hoeherer Kasten senkt ausserdem `q = verd/box` und
  macht damit das einzige harte Tor `VORverd == 0` **leichter** erfuellbar, und in 207 von
  448 betroffenen Log-Zeilen wuerde die 12er-Kappe heute gezeigte Masken verdraengen.

### 6.2 ROOM10F0 C1/C2/C3 nicht auf die Kreisform neu bauen
Die Typ-3-Form ist byte-true belegt (@0x0720/0x072c/0x0750 in `ROOM10F0.RDT`,
`dc 05 dc 05 … 03 ff 00 03`, w=d=1500, Mittelpunkt (x+w/2, z+w/2), Radius w/2 laut
`push_circle`) — und trotzdem kein Grund, diese drei Cuts anzufassen:
* Kein Symptom: `grep -c "R10F0" befund.log` = **0**; der Nutzer war 2026-09-21 nie in
  ROOM10F0.
* Keine Wirkung: auf dem Klemmpfad-Satz hat der ausgelieferte Quader-Stand
  **0 zerrissene Spalten** von 4318 / 3619 / 2037 Figurspalten mit Maske, VORverd 0,
  VORteil 0.
* Geometrisch unerreichbar, wo Quadrat und Kreis auseinanderlaufen: halbe Diagonale des
  Huellquadrats 750·√2 = **1060,66** < Ausdrueckradius 750+450 = **1199**; 0 von 4441
  begehbaren Band-0-Punkten liegen im Huellquadrat, der naechste begehbare Punkt zu den drei
  Kreismitten liegt 1202/1202/1204 Einheiten entfernt. Auch fuer den Zombie (r=400) gilt
  1150 > 1061.
* Der Umbau wirkt in der **Gegenrichtung** des behaupteten Fehlers: im Endartefakt werden
  bei C2 534 von 877 und bei C3 1549 von 1900 Punkten **naeher** (bis -6 bzw. -7 Stufen =
  384/449 Einheiten), weil das Hoehen-Optimum mitwandert (C2 -950 -> -1050, C3 -875 ->
  -1025).
* Beide Modelle beschreiben den Gegenstand ohnehin nicht: IoU 0,29..0,38 gegen das im
  Werkzeug selbst dokumentierte Gueltigkeitsband 0,6..0,92. Der offene Punkt dort ist die
  **Einzelzellen-Zuordnung**, nicht die Zellform.
Die Formmodellierung in `geom.py`/`geometrie.py` bleibt trotzdem drin (Schritt 1): sie ist
fuer Typ 1 bitgleich und fuer kuenftige Baeume richtig.

### 6.3 ROOM10F0 C4/C5 nicht morphologisch oeffnen
* 95,3 % der als Wirkung gezaehlten Verdeckung ist durch die Kollisionsgeometrie des Raums
  gedeckt: von 73224 verdeckenden Punkten bleiben ohne solides Sperrfeld auf dem Sehstrahl
  **3470 (4,74 %)** an 74 von 604 Standplaetzen. Am zitierten schlimmsten Platz (4600,7400)
  steht der Spieler hinter einer soliden Typ-1-Zelle x2300..4100 / z-5400..8200.
* Die zwei zitierten Spitzenwerte liegen ausserdem **ausserhalb** der RVD-Zone, in der der
  Cut ueberhaupt aktiv ist (Cut-5-Anker x-7900..3600 / z-5500..8000; Cut 4 x-2400..3600 —
  x=4600 ist in beiden draussen).
* Das Duennen-Mass ist monoton im Fix-Parameter (Oeffnen mit k=3/5/7/9 gibt
  24,4/20,3/18,4/16,5 % bei 1856/2779/3720/4326 verlorenen Maskenpunkten) und nicht
  trennend: das Nullmodell der **reinen Nutzer-Lassos** reicht bis 69,3 % (ROOM1060 C7,
  Treppengelaender, eine Zusammenhangskomponente, abgenommen). Die Schranke „so duenn wie
  die Handarbeit des Nutzers" ist damit nicht 26 %, sondern bis 69 %.
* Die Kosten gehen in die Richtung der Nutzerbeschwerde: 341 (C4) bzw. 210 (C5)
  Standplaetze verlieren jeden Maskenkontakt, HINTn 1141 -> 800, die relative Unterdeckung
  in C5 39,6 % -> 45,9 %; 107 von 673 Punkten des Kunst-Saums (der Zweck, fuer den die
  Tiefschwarz-Regel nach F1039/F1195/F1321/F1451 eingefuehrt wurde) und 18 Punkte eigener
  Nutzerkunst fallen weg, 13 neue Punkte landen auf projizierter begehbarer Standflaeche.
* Kein Bildbeleg existiert: ROOM10F0 kommt in 11470 protokollierten Bildern nicht vor, und
  die „Figur" in den Belegbildern ist der rechteckige Koerperkasten, nicht die Silhouette.
Richtige Abhilfe bleibt eine Freistellung (§5.2).

### 6.4 ROOM2090 / ROOM2091 nicht anfassen (C2, C3, C8, C9)
* **C2/C3:** die gesamte gemeldete Wirkung liegt in soliden Zellen. C3: 703 stehende und
  803 liegende Plaetze, **alle** in Klaertank-Sockel, Ostplattform, Ostwand oder
  Band-1-Randstreifen; C2: 484 von 484 ebenso. Von 7584 Dumppunkten des Raums liegen
  7584 in bandgleichen SCA-Zellen. Zusaetzlich ist das Objekt in C3 **kein** durchgehender
  Sockel: x>=223 ist der Klaertank-Sockel (deklarierte Zelle, Raycast trifft 18660/18660),
  x<223 die Ostplattform (Zelle #5, 2093/8207) — der „Bruch bei Spalte 223" ist die echte
  Grenze zweier Koerper. `DEPTH_FACTOR 0.90` wird auf diesem Pfad ausdruecklich nicht
  angewandt (gemessen: x=224 geschrieben 95, Bucket des Raycasts 95).
* **C9:** die geschriebene Tiefe 104 ist **richtig**. Der Kamerasatz deklariert seine
  Blickhoehe selbst: `ROOM2090.RDT` Cut 9 @Datei-Offset `0x194` = `a2 f7 ff ff` = **-2142**
  (Gegenprobe im selben Satz: Cut 0 @0x074 = -18, Cut 7 @0x154 = -18, Cut 8 @0x174 = -3042).
  Die Ebene, die 104 erklaert, ist -2192 — **50 Einheiten** entfernt, weniger als ein Bucket.
  Auf der wirklich begehbaren Menge (7488 Band-0- + 468 Band-1-Plaetze, 513 mit Kasten auf
  der Maske) hat der Auslieferungsstand **0 VOR-verdeckt und 0 HINTER-frei**; der
  Fixvorschlag 173 erzeugt **295** HINTER-frei, der Vorschlag 117 erzeugt 66. Der Gegenstand
  ist ausserdem kein Gitter an der Rueckwand, sondern die Nische hinter dem erhoehten
  Betonsims, und `Band 1 = -1800` ist die Bandobergrenze (`band = -(Y/0x708)`), keine
  Flaechenhoehe.
* **C8/C9 Silhouette:** der `oben=4/grow=1`-Mechanismus ist bitgenau bewiesen
  (`A'\C == C\A' == 0`, `raum.py:92/447-454/622`), heilt aber nur 15,3 % (C9) bzw. 0 % (C8)
  des messbaren Schadens — die Ursache ist die Cut-Zuordnung (§5.2). In C8 verdeckt der
  gemeldete Saum fuer den Spieler ueberhaupt nichts (Figursaeule trifft Saum 0).

### 6.5 ROOM1100 C1 und C2: der Kasten bleibt
* Fehlurteil **0 in beiden Richtungen** ueber 1685 bzw. 9496 Standplaetze mit Maskensicht,
  2,7 % bzw. 3,0 % duenne Struktur — fuer eine Wand ist ein Kasten die richtige Form, und
  hier ist es belegt.
* Der vorgeschlagene Fussleisten-Beschnitt an C2 ist gemessen **schaedlich**: ausgefuehrt
  (25211 -> 24513 Punkte) gewinnt er 232 Punkte Ueberdeckung und erzeugt **1323** Punkte
  LECK an 40 Standplaetzen — der Spieler wird durch die Wand sichtbar, genau die Fehlerklasse
  der Nutzer-Marke F595 („die Zombies um die Ecke sind alle durch die Wand sichtbar"), fuer
  die dieser Kasten gebaut wurde.
* Der Detektor misst seine eigenen zwei Parameter: `binary_closing(ones(5))` ueberbrueckt
  das 2-4 Zeilen dunkle Kontaktband und hebt die „Fussleiste" um Median 6, maximal 53 Zeilen;
  ohne closing faellt der Median-Ueberstand von +7 auf +3 und der Streifen von 698 auf 250
  Punkte. Ueber Schwelle x closing spannt der Wert in C2 von -42 bis +9 px. Und C1 und C2
  implizieren mit demselben Detektor die gemalte Wandflaeche 249 Einheiten auseinander
  (x=-15668 gegen x=-15917) — eine Wand aendert ihre Weltlage nicht mit der Kamera.
* Die 65 „gebissenen" Punkte sind ausserdem falsch beschrieben: bei (-15700,-11500) liegt die
  Spielermitte 100 Einheiten westlich der Zell-Ostflaeche, verdeckt sind 234 von 476
  Kastenpunkten und davon 224 geometrisch zu Recht (Strahltest gegen das Zellvolumen).

### 6.6 `abnahme.py` nicht auf objektweise VOR/HINTER umbauen (jetzt)
§4 W6: die Begruendung traegt nicht, objektweise bleiben 4 statt 0 Faelle, und die
VOR-Pruefungen vervielfachen sich (ROOM1100 C1 170 -> 868) ohne messbaren Gewinn (VORverd
bleibt in allen 7 geprueften Cuts 0). Was `abnahme.py` braucht, ist Schritt 4 (richtiger
Ort) und §5.6 (fehlendes Kriterium) — nicht eine feinere Aufteilung desselben Masses.

---

## 7. Zahlen, die dieser Text selbst nachgeprueft hat

| Aussage | Quelle, selbst gelesen |
|---|---|
| `unit_pri_kopfschnitt` Phase 2 nimmt den Klemmpfad | `test_pri_kopfschnitt.c` Kopf ~Z.95-105 und `re15_collision_constrain` im Gitterlauf |
| `abnahme.floor_aus_dump` nimmt `on_floor` | `abnahme.py:181-185` + `probe_r16_pri_masken_audit.c:87` |
| `raum.py` liefert den falschen Dump als Default | `raum.py:606` (`build/p2/dump_vorher.txt`) |
| die richtige Sonde ist formatgleich vorhanden | `probe_p2_floor_dump.c:1-18`, `:75` |
| `nur_kunst` fehlt in `bau_p2` | `grep -n nur_kunst tools/maske/*.py` -> nur `raum.py:413` |
| `ALPHA_SCHWELLE` ist als Heuristik markiert | `bau_p2.py:32` |
| der Marke-1-Riegel ist cut-global | `test_r19_marke1_10d0_c1.c`, Teil 1 laeuft ueber alle `m` Masken |
| master hat keinen der vier Fixes | `auswahl.json` (10D0 C1 weiter `aufrecht:true`, 1050 C1 ohne `zelle`), `geom.py` ohne `zellen_solid_test`, `probes/` ohne `r19` |