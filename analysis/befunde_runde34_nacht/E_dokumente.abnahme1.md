# Spur E — Abnahme 1 (unabhaengig, nach dem Bau)

Pruefer: Abnahme-Agent, hat den Code NICHT geschrieben. Auftrag: versuchen zu WIDERLEGEN, dass
Spur E fertig ist. Gepruefter Stand: `3030e61c` (Zweig `r34n/dokumente`), exe unveraendert seit
`54abec03` (danach nur Dossier-Commits; mein Bau: `ninja: no work to do`).

## 0. Urteil

**ABGENOMMEN** — kein Blocker, kein wesentlicher Mangel gefunden. Alle Nutzerpunkte der vier
Dokument-Abschnitte (AUFTRAG.md Z. 20..58) sind am Artefakt erfuellt; die Widerlegungsversuche
(eigener Satz-Leser, echter Speicher-/Lade-Durchgang, Gehen statt Teleport, Druck ins Leere nach der
Aufnahme, Wiederbetreten, Stand am Aufhebeort in den Klemmen-Cuts, Disassembly der zitierten
Scan-Adressen) haben nichts Tragendes umgeworfen. Es bleiben fuenf KOSMETISCHE Punkte (Abschnitt 6),
der wichtigste ist die Helligkeit (vorhersehbarer Nutzerbefund, Entscheidung beim Nutzer).

## 1. Gelesener Stand (git log, Dossier, Gegenpruefung)

* Bau-Commits: `16f19248` Dok 1, `34838629` Dok 2, `bbdd45b5` Dok 3, `2d7ede7b` Dok 4, `5e50910c` /
  `54abec03` Bild-Riegel + zwei angepasste Fremd-Riegel, `8e78e13d` Bau-Fix (cherry-pick).
* Dossier `E_dokumente.md` 0..9.10 gelesen. `E_dokumente.gegenpruefung.md` gibt es NICHT (die
  Gegenpruefung starb am Limit; der Bau hat stattdessen `selbstpruefung.py` gefahren, 56/57 PASS).
  Diese Abnahme ersetzt die fehlende Gegenpruefung nicht vollstaendig (keine RE-Neuherleitung aller
  Lagewerte), prueft aber jede tragende Zahl, die sich billig nachmessen liess (Abschnitt 5).

## 2. Eigener Bau + Suite-Zeile

`bash re15_port/tools/local_build.sh` (eigener Baum, 10:10–10:30, parallel liefen Agenten anderer
Baeume): configure OK, build OK, ctest **429/430**, einziger Roter `integration_r30_titel_puls`
(Periodenmessung des Titel-Pulses, „Periode … AUSSERHALB" — Zeitmessung unter Last).
Einzeln nachgefahren: **2 von 2 gruen** (26 s / 28 s). Kein Bezug zu Spur E (Titel-Schleife, kein
Dokument-Code) → **Suite 430/430** (memory reai-v2-gui-tests-flattern-bei-parallelen-agenten).
`unit_r34n_e_dokumente` PASS, `integration_r34n_e_dokumente_bild` PASS (173 s).

## 3. Nutzerpunkte aus AUFTRAG.md an der echten exe

Werkzeug `E_belege/abnahme1/abn1_lauf.sh`: eigene exe-Kopie `re15_pc_abn1.exe` in
`build/r34n_e/abn1/mess`, CONTINUE (Nutzerkarte Runde 30), `RE15_FRAMEDUMP` bei
`RE15_WINDOW_SCALE=3` (Readback vor SDL_RenderPresent, kein AUTOSHOT/SOFTWARE_RENDER), jedes Bild
angesehen. Laeufe (unversioniert) `build/r34n_e/abn1/<name>/`.

### 3.0 Texte, Titel, Satz (alle vier)

* Eigene Extraktion aus `AUFTRAG.md` (CRLF, Zitatpraefix, Tabs) gegen `E_texte/dok*.txt`:
  **4/4 Texte und 4/4 Titel zeichengleich**; die Nutzerzeilen der vier Texte sind reines ASCII.
* Die AUSGELIEFERTEN Seiten `shared_assets/RE2/FILES/FILE26..29_*.TIM` mit eigenem 4bpp-TIM-Leser
  dekodiert und Zeile fuer Zeile gelesen (`a1_seiten_FILE26_29_dekodiert.png`): alle Woerter,
  `...`, `'`, `:`, `!`, `,`, **4312**, **5632** stimmen; Absaetze wie beim Nutzer (Dok 2 vier
  Leerzeilen, Dok 3 eine — faellt auf den Kopf von p02 und entfaellt, Irons-Regel); Titelseiten in
  Versalien („POLICE OFFICER'S / FINAL DIARY ENTRY" zweizeilig), Listennamen gemischt; Seiten
  p00..p03 / p00..p04 / p00..p02 / p00..p02 = max_page 3/4/2/2, kein p<max+1>. Keine
  konstruierte Glyphe sichtbar (Apostroph = RE2-Glyphe wie im Irons Diary).

### 3.1 Dok 1 — Police Officer's Final Diary Entry (ROOM1050, sitzende Leiche vor dem Rolltor)

* **Lauf `a_dok1`** (Gehen statt Teleport an den Aufhebeort): Sprung 1050, Stand (14300,-6750) Blick
  Ost, **1 s GEHEN**, die Kollision haelt bei x 15232 (`[walk] F180 pos=(15232,0,-6750)`), VIERECK
  (F195) → Leser Satz 26 (`st=7 page=0/4`, F207), 4x RECHTS → 1/4..4/4 → EXIT, KREUZ → „The Police
  Officer's Final Diary Entry has been filed." (Schreibmaschine), VIERECK → Welt
  (`a1_dok1_aufnahme_bogen.png`). Danach **echter Speicher-Durchgang** (`RE15_SAVE_TEST_AGAIN=930`,
  Karten-Autofahrt Platz 4): `[save] saved (room 1050)`, Kartenblock 5 geaendert.
* **Lauf `c_dok1_laden`** (CONTINUE von GENAU dieser Karte, Platz 4): `resumed in room 1050`, KEINE
  Zeile `[dokumente] Boot-Weg` (Bit 57 geladen) → Schoss leer; danach Sprung 1050 (Tuer-Weg
  `scd_room_reenter`) → Schoss leer; FILE-Liste Zeile 0 = Dokument 1 (`liste0=1`)
  (`a1_dok1_schoss_vorher_laden_wieder.png`: links Bau-Bild mit Buch, Mitte nach dem Laden, rechts
  nach dem Wiederbetreten — die gemalte Hand ist wieder zu sehen).
* **Lauf `e_dok1_liste`** (nach dem Laden am alten Aufhebeort, Blick Ost): VIERECK → **nichts**
  (kein Leser, keine Meldung — ROOM1050 hat keinen Leichen-Satz); dann START, R1, VIERECK, VIERECK →
  **Leser aus der Liste** 1/4..3/4 (`a1_dok1_nach_laden_liste_leser.png`).
* Lage: Projektion (16474,-347,-6592) Cut 3 (136,05;142,55) / Cut 9 (157,82;167,07) = die gemalte Hand
  auf dem Schenkel (Dossier 2.4) — mit der Sonde nachgerechnet. Kamera: Zone 8 (Cut 3) enthaelt den
  Aufhebeort (Sonde `zonen`).
* Vorrang/Konflikt (Auftragspunkt 3): ROOM1050 hat keinen Untersuchen-Satz der Leiche (Bau 2.4;
  Slot-Liste nach dem Raumstart `build/r34n_e/aots_1050.txt`: Nachrichten-Slots 8/9/10 = msg 1/3/4,
  msg 5 an keinen Slot gebunden; Slot 10 = Tafel msg 4 x[15750..16750] z[-5850..-2650] grenzt an,
  schneidet das Dok-Rechteck z[-7250..-6250] nicht); Rolltor-Schalter Slot 7 x[16800..17600] z[-8950..-8150]
  schneidet nicht → kein Konflikt bei einem Druck. ROOM1051: Leichen-Satz Slot 11 gewinnt, bis
  sub01 @0x00CB2 ihn nach Bit (9,165) stilllegt (Unit-Riegel). Scan-Regel selbst disassembliert (5.).

### 3.2 Dok 2 — Elliot's Diary (ROOM1000, Bank, elliot.bmp)

* Marke selbst gemessen: 462 px (237,28,36), x 182..203 y 162..182 → Mitte (193,0;172,5);
  Buch-Auflagepunkt projiziert Cut 0 (192,98;172,49) (Sonde nachgerechnet). Im Framedump liegt das
  Buch auf der rechten Bank in der Marke (Bau-Bild `bau_d2_bank_vorher_nachher.png`, eigener Zoom).
* Aufnahme/Leser 1/5..5/5/Meldung/Bank leer: Bau-Lauf `d2_aufnahme` (Log `st=7 page=0/5 satz=27`,
  Bilder angesehen). Laden: Lade-Riegel (Karte mit Bit 58) im Suite-Lauf PASS.

### 3.3 Dok 3 — Marvin's Notes (ROOM1020, Marvins Schreibtisch, marvin.bmp)

* Marke 99 px x 151..161 y 101..109 → (156,5;105,5); Blattmitte projiziert Cut 6 (156,49;105,48).
* **Stand am Aufhebeort in den Klemmen-Cuts** (`d3_stand_c3`, `d3_stand_c6`): Leon verdeckt das Blatt
  in Cut 6 und Cut 3 korrekt; die Tiefen-Klemme (Cut 3, Maske 258) zieht das Blatt NICHT vor die
  Figur (`a1_d3_d4_spieler_am_aufhebeort.png` oben).
* Nachricht-Umzug/Leser/„It's Lieutenant Branagh's desk." danach: Bau-Lauf `d3_aufnahme` (Bilder
  angesehen) + Unit-Riegel (Umzug, Waechter, mit Bit 59 Original-Slot, Nordseite → Nachricht).

### 3.4 Dok 4 — Armory Notice (ROOM1010, Verhoertisch, interrogation.bmp)

* Marke 528 px x 208..231 y 166..187 → (220,0;177,0); darunter steht im Spiel das Original-Spray
  (Bau 2.7, im Framedump bestaetigt), das Blatt liegt daneben, Ursprung Cut 0 (210,71;170,49) in der
  Marke. Nutzer: „Ort in etwa" — Lesart vertretbar und im Dossier begruendet.
* **Stand am Aufhebeort** (700,6400) in Cut 0 und Cut 1 (Klemmen 40/53): Blatt neben der Dose ganz
  sichtbar, keine Figur wird vom Blatt uebermalt (`a1_d3_d4_spieler_am_aufhebeort.png` unten; ein
  Zombie des Raums steht am Sprung-Stand — Original-Besetzung, wie im Dossier 9.4).
* Spray-Vorrang (kleinerer Slot 2) und Blatt-Zone: Unit-Riegel + Bau-Lauf `d4_aufnahme`.

### 3.5 Codes 4312 / 5632 (Auftragspunkt 5)

Nutzertexte passen zu den Schloessern. Nachgeprueft an `E_belege/codes.txt`: die Bau-Herleitung
arbeitet mit je Schloss VERSCHIEDENEN Rad-Versaetzen (v=2 fuer ROOM10D0, v=6 fuer ROOM1230), was
zirkulaer aussieht. Tragfaehig ist es trotzdem, und zwar ueber die SUB-Nummern: ROOM10D0 verlangt
die Subs 9,8,6,7 (Bits 13..16), ROOM1230 die Subs 10,11,8,7 — mit **derselben** Regel Ziffer =
Sub − 5 ergibt das 4,3,1,2 und 5,6,3,2 = genau die Original-Zettel (ROOM1110 msg 0 @0x0D68 „4312",
ROOM1230 msg 10 @0x16F4 „5632"). Die Rad-Tabellen der zwei Raeume sind nur um 4 verschoben
(sub6→Rad 3 bzw. 7). Port: die Schloss-Skripte laufen unveraendert, `test_keypad.c` faehrt ROOM1230
„5632" ueber die Ziffern-Subs; ROOM10D0 „4312" ist weder im Port-Test noch an der exe gefahren
(auch nicht in dieser Abnahme) — Beleg dort nur Skript + Zettel. Kopplung mit Spur F (Zettel fallen
weg) bleibt: E und F zusammen ausliefern.

## 4. Gegenproben (Zusammenfassung)

| Gegenprobe | Ergebnis | Beleg |
|---|---|---|
| Gehen statt Teleport an den Aufhebeort (Kollision haelt) | Aufnahme feuert (Pruefpunkt 15852 im Rechteck) | Lauf `a_dok1`, `[walk] F180` |
| Druck ins Leere nach der Aufnahme (gleicher Stand, gleicher Blick) | nichts | Lauf `e_dok1_liste` F100..F180 |
| Speichern nach Aufnahme → Laden (Boot-Weg) | kein Prop, Liste traegt Dok 1 | `c_dok1_laden`, `a1_dok1_schoss_vorher_laden_wieder.png` |
| Wiederbetreten (Tuer-Weg `scd_room_reenter` per Sprung) | kein Prop | dito rechts |
| Leser aus der FILE-Liste nach echtem Laden | 1/4..3/4 | `a1_dok1_nach_laden_liste_leser.png` |
| Figur am Aufhebeort in Klemmen-Cuts (1020 C3, 1010 C0/C1) | 1020 C3: Leon verdeckt das Blatt, die Klemme zieht es nicht vor ihn; 1010 C0/C1: am Stand keine Ueberdeckung, Blatt ganz sichtbar | `a1_d3_d4_spieler_am_aufhebeort.png` |
| Nachbarverhalten: Tafel-Satz 1050, Tisch-Nachricht 1020, Spray 1010 | unveraendert bzw. nur im Dokument-Rechteck verdraengt (Original-Regel, Zensus 10/10) | Dossier 3.4 + Unit-Riegel |
| Variante x1 | nur Unit-Riegel (ROOM1001/1011/1021/1051); kein Elza-Spielstand an der exe | — |

## 5. RE-Gate-Pruefung des Codes

`re15_dokumente.h`, `dokumente_r34.c`, die Haken in `re15_files.c` / `scd_room_setup.c` /
`main.c` gelesen: jede Konstante traegt entweder eine Adresse/einen Datei-Offset oder ist als
NUTZER-VORGABE / PORT-WAHL / PORT-ZUSATZ mit Messgrundlage gekennzeichnet; Commit-Messages der vier
Bau-Commits zitieren dieselben Adressen. Kein „interim/plausibel/TODO" (grep leer).
Selbst disassembliert (`re15_disasm.py`, `info/Re1.5/PSX.EXE`) — die tragenden Zitate stimmen:
* @0x80042c50/54 `lw s0,0(s4)` / `addiu s4,s4,4` (Scan aufsteigend), @0x80042f8c `jalr v0` +
  @0x80042f94 `j 0x80043028` (ein Satz je Druck) → kleinster Slot gewinnt.
* @0x80042c84..90 `lbu v0,1(s0)` / `and v0,v0,a3` / `beq v0,zero` (sat 0 wird uebersprungen),
  @0x80042f48..50 `lbu v0,0(s0)` / `beq v0,zero` (sce 0), @0x80042ca0 `andi v0,v0,0x10` (Aktionsbit),
  @0x80042bd0 `ori v0,zero,0x26c` (620 voraus).
Nachgerechnet mit der Engine-Sonde: alle vier Projektionen (3.1..3.4), Marken aus den Nutzerbildern
selbst gezaehlt. Parallel-Felder des Slot-Umzugs: `re15_aot_state_t` hat genau die sechs Felder, die
`tisch_nachricht_umziehen` mitnimmt.
Unkritische Port-Wahl-Parameter: Dok-4-Lageregel „Rand 30" (`lage_1010.py`), Drehung 0 fuer Dok 1..3
— beide als PORT-WAHL gekennzeichnet, optische Nutzer-Abnahme offen (Dossier 9.10 §1).

## 6. Maengelliste

| # | Schwere | Titel | Beleg |
|---|---|---|---|
| 1 | kosmetisch | Dokumente deutlich dunkler als ihre Ablage: Mittel-Y Dok 1 26,9 (Untergrund 69,1), Dok 2 27,0 (57,0), Dok 3 33,3 (93,8), Dok 4 22,9 (58,2); Marvins Blatt liest sich als dunkelgraues Objekt auf hellem Tisch. Byte-true Licht des aktiven Cuts, gleich hell wie die Original-Items (Spray gemessen); in 1000/1010/1050 gibt es keinen anderen Lichtsatz zur Wahl. Vorhersehbarer Nutzerbefund: Runde 30 Nachschliff „tischlicht" (Irons Diary Y 15..17 → 37..44), Runde 32 „Tor ROOM1170 zu dunkel". Nutzerentscheid noetig, keine Aenderung ohne Vorgabe. | `a1_helligkeit_dokumente.png`; Messung aus den Framedumps `integration_r34n_e_dokumente_bild` (mit/ohne Bit) |
| 2 | kosmetisch | Listenname „Police Officer's Final Diary Entry" (228 px) ragt beim Anwaehlen ~75 px ueber die Hervorhebungs-Kachel (x 43..196) hinaus. Wortlaut = Nutzervorgabe, im Dossier als Risiko 7.6 gefuehrt. | Bau-Lauf `liste` F1140 (Kachel endet bei „Diary") |
| 3 | kosmetisch | ROOM1051 (Elza): die Nahaufnahme Cut 9 beim Untersuchen der Leiche (sub03 `Cut_chg 9`) zeigt den Schoss OHNE Buch (Regions-Test, Anker Cut 9 ausserhalb des Flurs); in Cut 3 davor/danach liegt es da. Byte-true Culling-Regel, nicht an der exe gerendert (kein Elza-Stand). | Dossier 2.4 + 9.9 §2 |
| 4 | kosmetisch | Meldung „The Elliot's Diary has been filed." / „The Marvin's Notes has been filed." — die RE1.5-Vorlage „The <Name>" (Skript [5] @0x800c506f) mit possessiven Nutzernamen ergibt holpriges Englisch. Vorlage byte-true, Namen = Nutzerwortlaut → nur Hinweis. | Bau-Laeufe d2/d3 (Meldungsbild) |
| 5 | kosmetisch | Nachweisluecken ohne sichtbare Wirkung: Schosshoehe Dok 1 nur EIN Verfahren (Triangulation Cut 3 × Cut 9), Auftrag verlangte zwei; Code 4312 am Port-Schloss ROOM10D0 nie gefahren; Rad-Versatz-Herleitung in `codes.py` je Schloss verschieden (tragfaehig erst ueber die Sub-Nummern, 3.5). | Dossier 3.5 / 3.7, `codes.txt` |

Nicht als Mangel gewertet: Dok 4 neben statt unter der Marke (Original-Spray steht dort, „in etwa");
PSX-Prop-Lader und Android-GLOB (offene Punkte 9.10 §4/§5, betreffen nicht das PC-Artefakt);
`integration_r30_titel_puls` (flattert unter Last, einzeln 2/2 gruen).

## 7. Belege (`E_belege/abnahme1/`)

| Datei | Inhalt |
|---|---|
| `abn1_lauf.sh` | Laufwerkzeug dieser Abnahme (eigene exe-Kopie, CONTINUE, Framedump) |
| `a1_seiten_FILE26_29_dekodiert.png` | die ausgelieferten TIM-Seiten, mit eigenem Leser dekodiert |
| `a1_dok1_aufnahme_bogen.png` | Lauf `a_dok1`: nach dem Gehen, Leser 1/4..4/4, EXIT, Meldung, Welt |
| `a1_dok1_schoss_vorher_laden_wieder.png` | Schoss: Bau-Bild mit Buch / nach echtem Laden / nach Wiederbetreten |
| `a1_dok1_nach_laden_liste_leser.png` | Druck ins Leere, FILE-Liste, Leser aus der Liste nach echtem Laden |
| `a1_d3_d4_spieler_am_aufhebeort.png` | Figur am Aufhebeort in 1020 Cut 3/6 und 1010 Cut 0/1 |
| `a1_helligkeit_dokumente.png` | Ausschnitte der vier Dokumente im Nutzer-Cut (+ 1010 Cut 1) |
