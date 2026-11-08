# Spur AN: Annette-Szene im hinteren Zugwagen (Runde 41, laufendes Dossier)

Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Basis `r41/form` 62f0be3ac (ab Nachbesserung 3: r41/form 7d79bcfae gemergt). Wer nach einem Abbruch weitermacht,
liest zuerst Abschnitt 0.

Auftrag, Wortlaut Nutzer: "Sobald man den Boss aussen besiegt hat, den Generator eingeschaltet und das Tor geoeffnet, ist Anette nicht
mehr im Fahrer Raum. Sie ist hinten im Zugabteil. In Resident Evil 2 gibt es eine Szene wo Anette zu ihren Mutierten William spricht,
und er sie dann umbringt. Sowas wuerde ich dann im hinteren Zugabteil auch machen inklusive dem Dialog. Betreff der G5 Birkin Form am
Ende - das ist eine Entscheidung die ich gerne so drin lassen moechte - also erst die Tierform von Birkin noch mal die wir erstellt
haben und dann die G5 Form."

## 0. Stand und naechster Schritt

| Schritt | Stand |
|---|---|
| Phase 1: RE2-Szene suchen | erledigt (Abschnitt 1). Ergebnis: die Szene liegt NICHT auf der Leon-Disc, sondern auf der Claire-Disc; davon hat das Repo nur die Sprachspuren. Raum, Kameras und Bildzahlen sind damit nicht lesbar (Frage F1). |
| Phase 1: Port-Zustaende | erledigt (Abschnitt 2). Die drei Bedingungen sind zusammen das Flag (3,89); die Disc setzt Annette ab da schon aus 603 heraus. |
| Phase 1: Plan | Abschnitte 3 bis 6, offene Fragen Abschnitt 7 |
| Phase 2: Bau | erledigt (Abschnitt 8), Tests gruen (Abschnitt 9), Sichtbelege (Abschnitt 10). Der Bau weicht vom Plan in Abschnitt 3/4 an drei Stellen ab (8.2). Offen: Abschnitt 11. |
| Nachbesserung 1 (2026-11-02) | erledigt (Abschnitt N1): RE2-Szene auf der Claire-Disc gefunden und im Emulator aufgenommen, 5090-Szene danach umgebaut, Zugstart-Szene 6030, Toene (ANNETTE_SE.VBS), F7-Recherche. Offen: N1.10. Abschnitte 1.2, 3, 5, 7, 8.2 sind dadurch teils ueberholt (N1 gilt). |
| Nachbesserung 2 (2026-11-02/03) | erledigt (Abschnitt N2): Bit 133 eingetragen, Ausgang 6030 vor dem Zugstart = Tuer (msg 20 erst danach), Musikwechsel als Original belegt, Blut = RE2-Emitter FUN_80104C24, Sherry vermerkt. Offen: N2.8. |
| Nachbesserung 3 (2026-11-03) | erledigt (Abschnitt N3): r41/form gemergt; nach Abnahme 1 Satz/Landung in Cut 11 mit neuen Lagen und RE2-Vorstoss (M1), Abgang 6030 ueber Cut 0 (M2), Zusammensacken im Trefferbild (M4), Blut in RE2-Bildgroesse (M5), Untertitel = RE2-Stimmlaenge (M6), PAL-Takt + Ausholen nach der Aufnahme (M7), Leiche beim Wiedereintritt (M8), Kampfstart Cut 11 geprueft (M9). Offen: N3.12. |
| Nachbesserung 4 (2026-11-03) | erledigt (Abschnitt N4): nach Abnahme 2 liegt die Leiche flach mit dem RE2-Liegeclip 0x18 (ROOM6141-RBJ, N2), sie ist kein Schuss-Kandidat mehr, und der Waffen-Resolver hat die RE1.5-Typpruefung @0x80012178-84 (N3). Abgang 6030: Leon tritt zur Seite, Tuerton nach RE2 ROOM4081, keine Tuerbewegung, weil es kein Modell gibt (N4). Gesicht beim Satz: mit den Disc-Kameras nicht moeglich, begruendet (N5). N3.5 berichtigt (Ladestandbild). |
| Nachbesserung 5 (2026-11-03) | erledigt (Abschnitt N5): Belege A1/H1 berichtigt, Schuss geht durch Begleiter (RE2-Regel hp < 0, Nutzerentscheid), Test Schusslinie 10D0; Leichen-Kollision wie RE2. Der Kasten-Teil (RE2-Segment fuer jede 0x4D) ist durch N6 eingeschraenkt. |
| Nachbesserung 6 (2026-11-07) | erledigt (Abschnitt N6): nach Abnahme 4 M1 traegt die 0x4D in 6000/6030 und in 5090 vor der Szene wieder ihren RE1.5-Kasten 450/1530 (STAGE6 @0x80101958 -> 0x80102784, STAGE5 @0x8011d88c -> 0x801207d0); das RE2-Segment 350/1350 setzt annette_r41.c erst beim Szenenstart und fuer die Leiche. Pult 6030: Leon haelt bei 901 statt 802. |

Belege dieser Spur: `analysis/befunde_runde41/AN_belege/` (Werkzeuge unter `werkzeug/`).

## 1. RE2-Vorbild

### 1.1 Auf der Leon-Disc gibt es die Szene nicht

Gesucht in `info/re2leon/` (RE2 DualShock, Leon-Disc SLUS-00748, Szenarien Leon A und Leon B):

- Gegnertypen aller 250 RDTs (Sce_em_set 0x44, Typ = Byte +3, Walker `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py`,
  `AN_belege/werkzeug/emscan.py`, Ergebnis `AN_belege/re2_leon_npc_typen.txt`): Annette (Typ 0x47) steht nur in ROOM4040, 4050,
  4090, 6020, 60C0 und 6110. In keinem dieser Raeume steht eine Birkin-Form (0x30 bis 0x37); ROOM6020 hat neben ihr nur 0x2A (Mr. X),
  ROOM6110 nur 0x3F und Ada 0x43 (`AN_belege/re2_leon_typen_stage5_7.txt`).
- Sprachspuren: Xa_on (0x59) in allen RDTs, aufgeloest ueber die Tabelle, deren Zeiger jede STAGEn.BIN beim Laden nach 0x800ce544
  schreibt (`sw v0,-0x1abc(at)` = Bytes `44 e5 22 ac`; STAGE6.BIN @0x11b0, Tabelle 0x8011bad0 = Datei @0x1AD0, je Eintrag
  {Kanal, Block, Laenge} als u16; Zensus `AN_belege/re2_leon_xa_zensus.txt`, Werkzeug `werkzeug/xascan_all.py`).
  Alle 66 Eintraege von VOICE06.XAS (Stage 6) dekodiert (`werkzeug/xadec.py`, XA-ADPCM 4 bit, 37800 Hz, 4032 Samples je Kanalsektor)
  und per Spracherkennung gelesen (`werkzeug/asr.py`, Whisper medium.en; `AN_belege/re2_leon_stage6_xa_transkript.txt`).
  Annette spricht auf der Leon-Disc nur in ROOM6020/60C0 (Xa 01 bis 09: "You murdered my husband ...", "Now where's that spy you were
  working with earlier?", "... You're about to die anyway."). Das ist ihre Konfrontation mit Leon, nicht mit William. William ist auf
  der Leon-Disc nur Gebruell (ROOM5090 Xa 13/14 VOICE05, ROOM6170 Xa 40/41 VOICE06).
- Die Videos `Downloads/Resident Evil 2 Cutscenes Claire|Leon Version PS1 4K` enthalten nur die FMVs (Spracherkennung der Tonspur:
  Vorspann, Birkin wird angeschossen, G-Mutation, Abspann). Die gesuchte Szene ist Spielgrafik und kommt dort nicht vor.

### 1.2 Die Szene liegt auf der Claire-Disc

Das Repo enthaelt unter `info/re2leon/PL0/VOICE/` auch Dekodierungen der Claire-Disc (VOICE1x.XAS, in Sprecher-Ordner sortiert;
die XAS-Dateien selbst und die Claire-RDTs fehlen). Darin:

| Datei (unter `info/re2leon/PL0/VOICE/`) | Lage im Kanal (`done/VOICE16.XAS[n].wav`, `AN_belege/voice16_abschnitte.txt`) | Sprecher | Inhalt (zwei Erkenner) |
|---|---|---|---|
| `Anette/VOICE16.XAS[9]_3.wav` | Kanal 9, Sample 299249-447299 = 7,917-11,833 s = Kanalsektor 74,2-110,9 | Annette | medium.en: "W-William! You're alive!"; large-v3-turbo: "W-William, you're alive." |
| `Anette/VOICE16.XAS[12]_2.wav` | Kanal 12, Sample 45674-102374 = 1,208-2,708 s = Kanalsektor 11,3-25,4 | Annette | Schrei, 1,50 s |
| `Clair/VOICE16.XAS[12]_5.wav` | Kanal 12, 8,500-10,542 s | Claire | "What happened? Annette!" |
| `Anette/VOICE16.XAS[12]_4.wav` | Kanal 12, 4,000-8,500 s | Annette | unsicher ("Which room? Tell me, William ... Where's Sherry?"), nicht verwendet |

Zuordnung Abschnitt -> Kanalort per Kreuzkorrelation der 48-kHz-Abschnitte gegen die 37,8-kHz-Kanaldekodierung (r = 1,000 fuer alle
genannten, `werkzeug/seg2.py`). Ein Kanal traegt mehrere Xa-Eintraege hintereinander (vgl. VOICE06 Kanal 6: Xa 41 Block 0-107, Xa 16
Block 107-191, Xa 01 Block 191-204); die Grenzen der Claire-Eintraege sind ohne die Claire-Stage-Tabelle nicht byte-genau angebbar,
die Kanalsektoren oben sind gemessen.

**Was belegt ist:** Annettes Satz an William woertlich ("W-William! You're alive!", RE2 zeigt in der US-Fassung keine Untertitel, der
Wortlaut ist gehoert, von beiden Erkennern gleich), ihr Schrei, Claires Ruf "What happened? Annette!" auf demselben Kanal.
**Was nicht belegt ist:** Raum, Szenario, Ausloeser, Kameraschnitte, Bildzahlen, Clips von Annette und William, Gebruell-Toene. Das
steht nur in den Claire-RDTs (SCD, RID) und der Claire-STAGE6.BIN (Xa-Tabelle). Gesucht auf dem Rechner (Repo, `Downloads/`,
`Documents/`, `Desktop/`, ePSXe- und DuckStation-Ordner, Laufwerk D:): einziges RE2-Abbild ist `Downloads/ePSXe2018/re2leon.cue`.
Eine Emulator-Aufnahme der echten Szene (Werkzeug der Spur G5A) ist deshalb nicht moeglich. Frage F1.

### 1.3 RE2-Modell der Annette (Leon-Disc, vollstaendig)

RE2 `CDEMD0.EMS` (`re15_port/shared_assets/RE2/CDEMD0.EMS`, TOC @0x8009adf4, `engine/src/gen/re2_ems_toc.inc`):

| Typ | TIM (rec 2) | EMD (rec 3) | Inhalt |
|---|---|---|---|
| 0x47 | Sektor 4476, 99872 B | Sektor 4525, 51252 B | Annette: helles Haar, weisser Kittel, schwarzes Oberteil, lange Hose (`AN_belege/re2_em47_annette.jpg`) |
| 0x49 | Sektor 4616, 99872 B | Sektor 4665, 51252 B | dasselbe EMD byte-gleich (md5 88ca733b...), TIM anders (65590 Byte): Kittel mit Blut, verletzte Annette (`re2_em49_annette.jpg`) |

Das ist das Aussehen der Frau im Kittel aus #145 bis #147 (helles Haar, weisser Kittel, schwarzes Oberteil, lange dunkle Hose).

**Gerippe gleich dem RE1.5-Typ 0x4D** (`AN_belege/rig_4d_gegen_re2_47.txt`, `werkzeug/rigcmp.py`): beide 15 Knochen, gleiche Eltern,
gleiche Versaetze bis auf die Unterarme/Haende (Knochen 10/11/13/14 z -60/-61 gegen -55/-46). Netz je Knochen gleich viele Punkte ausser
Knochen 0 (95 gegen 72). RE2-Netz und -Textur passen damit auf das RE1.5-0x4D-Gerippe samt seinen 24 Clips.

## 2. Port heute (gemessen in den RDT-Skripten, `re15_port/tools/scd_dump_room.py`)

### 2.1 Die drei Bedingungen

| Ereignis | Raum | Stelle | Flag |
|---|---|---|---|
| Generator eingeschaltet | 5080 | sub02 @0x007AE Message_on 0 (Frage), @0x007B8 Ck(12,31)=0 (Ja), @0x007BC msg 1, @0x007EC Evt_exec sub03 (Sturz der Form) | kein eigenes Flag; der Bahnsteig-Kampf beginnt erst danach |
| Boss am Bahnsteig besiegt (Port: Flucht der Riesenform) | 5080 | Port setzt (5,28) am Ende der Flucht (`birkin_bahnsteig.c` bb_flucht, wie der Birkin-Tod @0x8011aecc STAGE5); sub01 @0x0078A Ck(5,28)=1 -> @0x00792 Set(3,48), @0x00796 Set(3,49), @0x0084C msg 3 "The power already has been supplied to the train!" | (3,48), (3,49) |
| Tor geoeffnet | 6000 | (3,49)=0: @0x00E02 Aot Slot 5 -> msg 7 "The power is not supplied."; (3,49)=1 und (3,89)=0: @0x00E66 Aot Slot 5 sce 3 -> sub07 @0x011A6 msg 5 "A device to open the tunnel gate. Will you turn on the switch?", @0x011B0 Ck(12,31)=0, **@0x011B4 Set(3,89,1)** | **(3,89)** |

(3,89) setzt (3,49) voraus, (3,49) den Bosssieg, der Bosssieg den Generator. **Die Bedingung ist also allein (3,89) = 1.**

### 2.2 Die Frau im Kittel (Typ 0x4D) heute

| Raum | Stelle | Bedingung | Lage | Szene |
|---|---|---|---|---|
| 6000 Bahnsteig | sub00 @0x00F44 | (3,100)=0 | (3150,-3600,1600) | sub06 @0x0106A: Set(3,100), msg 0-3 ("You must be that cop ..."; Port-Text Runde 36) |
| 6000 | sub00 @0x00F68 | (3,100)=1, (3,239)=0 | dieselbe | steht; Aot Slot 4 -> sub02 @0x00F84: Set(3,239) @0x00F8E, msg 4 "Let's see if we can get this thing moving.", sie geht in den Zug |
| 6030 Fuehrerstand (Eintritt Cut 0) | sub07 @0x010D4 | (4,23)=0 | (-1790,0,-22272) | sub08: msg 11-19 ("Where's that biker girl?" ... "Not enough power!" ... "I'll be ready to start the train when you do!") |
| 6030 | sub07 @0x010F8 | (4,23)=1, (3,89)=0 | dieselbe | sub09, steht am Pult |
| 6030 | sub07 | **(3,89)=1** | **kein Spawn** | - |
| 6030 Wagen (Eintritt Cut 6/8/12) | sub06 @0x0106A / @0x01098 | (3,89)=0 / 1 | - | (3,89)=0: Tuer Slot 3 nach 5090 = msg 10 "I have to get this thing moving first!"; (3,89)=1: Ausgang Slot 2 = msg 20 "You can't get off the train right now." |
| 5090 Wagen C | sub00 @0x0122A `44 00 4d 40 .. 58 02 00 00 2c 9d` | **unbedingt** | (600,0,-25300) | keine; steht |
| 5090 | sub00 @0x01278 | (3,89)=1 | - | Evt_exec sub03: Schleife Set(1,28) an/aus (Fahrt) |
| 5091, 6031 (Elza) | - | - | - | keine 0x4D; am Pult in 6031 steht Sherry (0x4B, sub07 @0x00EFA) |

**Die Disc traegt den Nutzerablauf also schon:** nach dem Tor (3,89) spawnt der Fuehrerstand sie nicht mehr, 5090 ist erst ab (3,89)
erreichbar (6030 Slot 3), und dort steht sie in Wagen C. Abweichung im Port nur: 5090 spawnt sie unbedingt, also auch bei
einem Debug-Sprung vor dem Tor.

**Modell heute:** EM4D aus RE1.5 `CDEMD0.EMS` (Elzas Kunst auf Adas Rig, `analysis/story_begegnungen/BERICHT.md` Z. 151; debug.log
`[enemy] EM4D loaded: 15 meshes, 15 bones, 24 clips`, `herkunft/birkin_modelle/port_messung/r5090/debug.log`). Also NICHT das
Aussehen aus #145 bis #147.

### 2.3 Was 5090 heute beim Betreten zeigt

- Einstieg aus 6030 Slot 3 auf (27200,0,4900) Cut 4 (Wagen A). Tueren A -> B -> C: Slot 0 (rect (-1000,3150)) -> (24700,-9250) Cut 9,
  Slot 2 (rect (-1000,-11150)) -> (24800,-23550) Cut 14.
- Feuer: main00 @0x0110E bis @0x0120E, 17 Sce_espr_on.
- Riesenform (Typ 0x30, Asset RE2/BAHNSTEIG_FORM) ab Raumladung, sub00 @0x0124A auf (-14700,0,-23350), grid 0x33 friert die Wurzel,
  sie spielt Stehen-Clip 2 (`re15_bb_wartet`).
- Kampfstart: sub01 @0x01290 Cmp(0x0A = Cut, 12) -> @0x01296 Evt_exec sub04, @0x0129A Set(3,42). sub04: @0x012F2/@0x012F6 Sperre und
  Balken, @0x012FE Pos_set Form (1200,0,-23350), @0x0130A Member_set(0x0C,0x13) = Freigabe, @0x0130E Cut 15, Musik, @0x01320 Leon dreht
  sich nach (300,-23400), @0x01334 Aot_reset Slot 4 (Tuer nach B zu), @0x01342 Cut 12, Sperre frei.
- Kameras Wagen C (RID @0x60): Cut 11 @0x1C0 (7626,-3256,-25360)->(-1504,-986,-21970) = Blick aufs Westende = **#145**; Cut 12 @0x1E0
  Kampf; Cut 13 @0x200; Cut 14 @0x220; Cut 15 @0x240 (flag 1, Szenenkamera aus (46,-1236,-24302)).

### 2.4 Abgleich mit ihren bestehenden Zeilen (Hinweis Orchestrator, Story-Inventur r41/story80 ABGLEICH.md B10/B16, f.1)

Reihenfolge im Spiel: 6000 Treffen (msg 0-3) -> 6000 sie geht in den Zug (msg 4, (3,239)) -> 6030 Pult (msg 11-19, (4,23)) -> 5080
Generator und Bahnsteig-Kampf ((3,48)/(3,49)) -> 6000 Tor ((3,89)) -> 5090 Wagen C, Szene, Tod. Alle ihre Zeilen in 6000 und 6030 liegen
vor (3,89); nach (3,89) spawnt keiner der beiden Raeume sie mehr, und der Zug ist dann nicht mehr zu verlassen. **Keine Doppelung im
Ablauf.** Inhaltliche Reibungen, die ich nicht selbst aufloese (Fragen F3, F4):

1. 6030 msg 18 "I'll be ready to start the train when you do!" - nach dem Tor faehrt der Zug (5090 sub03), sie steht aber hinten.
   Wer faehrt, und warum geht sie nach hinten?
2. Ihr Satz in 5090 setzt voraus, dass sie William fuer tot hielt ("You're alive!"). In 6030 sagt sie dazu nichts. Mod-Text 50B0 msg 6
   (Ada): "He's dead. Umbrella killed him." passt dazu.
3. Der Name "Annette" ist Mod-Name ohne Capcom-Beleg (ABGLEICH B16); der neue Text nennt sie wie die bestehenden Zeilen.

## 3. Plan Bild fuer Bild (Leon, 5090 Wagen C)

Die RE2-Szene ist in Kamera und Zeit nicht lesbar (1.2). Belegt sind: ihr Satz und ihr Schrei (RE2), die Konstellation Frau neben dem
aufgerichteten Wesen in Cut 11 (80 % #145 = Bild 2 der Form: Wesen bei (776,0,-23713) Gier 3917 in Pose Bild 2,
`form/FORMBLATT2.md` Z. 223; Frau nahe (1166,0,-25211), `form/kamera.md` Abschnitt 3), Kameras und Raum (RE1.5). Alles Uebrige
ist PORT-WAHL und so markiert.

Ausloeser: der Kampfstart selbst. 5090 sub01 @0x01296 Evt_exec sub04 wird beim ersten Cut 12 in Wagen C umgeleitet auf das Programm
der Szene, solange Annette lebt und dasteht ((3,89)=1, (3,42)=0, Typ 0x4D aktiv). Das Programm endet mit Evt_exec sub04: danach laeuft
der Kampfstart unveraendert (Pos_set, Cut 15, Freigabe, Cut 12). Einmalig durch (3,42), das sub01 @0x0129A direkt nach dem Aufruf setzt.

| Bild | Kamera | Annette | Riesenform | Leon | Ton/Text |
|---|---|---|---|---|---|
| 0 | Cut 11 (#145) | steht bei (1166,0,-25211), Blick zur Form | auf (776,0,-23713), Gier 3917 (Bild-2-Lage), Drohen-Clip 10 ab Bild 0 | gesperrt (Set(2,7,1), Set(1,27,1) wie sub04 @0x012F2/@0x012F6), bleibt wo er ist, dreht sich zur Form | Clip-Toene der Form (Clip 10: Bild 30, 51 ...) |
| ~45 | Cut 11 | | Pose Bild 2 (Clip 10 Bild 45-52) = #145 | | |
| ~20 bis ~138 | Cut 11 | spricht | | | msg 40 "Annette: W-William! You're alive!" (Dauer nach RE2-Aufnahme 3,92 s = 118 Bilder) |
| ~140 | Cut 11 | wird gepackt (Griff Bild 8), haengt in der Klaue | Griff-Clip 27 (Packen Bild 8, Schleudern, Bruellen) | | msg 41 Schrei (RE2 VOICE16 Kanal 12, 1,50 s), Blut am Griffpunkt |
| Wurfbild | Cut 11 | fliegt, schlaegt auf, bleibt liegen (Blut-Textur EM49, Todespose) | bruellt | | Blut beim Aufschlag |
| +30 | Cut 11 | liegt | Clip 27 laeuft aus | | msg 42 "Leon: Annette!" (Vorbild: Claire "What happened? Annette!", RE2 VOICE16 Kanal 12) |
| Ende | - | Leiche bleibt bis Raumwechsel | Evt_exec sub04: Pos_set (1200,-23350), Cut 15, Freigabe, Kampf wie bisher | | Kampfmusik (sub04) |

Bildzahlen ausser der Satzdauer: PORT-WAHL (Takt wie die Szenen in 1150/2090). Elza (5091): keine Szene (Disc: keine 0x4D in
5091/6031), Frage F2.

## 4. Modell, Clips, Gore

- **Aussehen (PORT-WAHL, Frage F5):** RE2-EM047-Netz und -Textur auf das RE1.5-0x4D-Gerippe (Gerippe gleich, 1.3), nur in Leons
  Raeumen 6000/6030/5090. Bewegung und Clips der 0x4D bleiben die RE1.5-Clips. Nach dem Tod die Textur von EM049 (Kittel mit Blut).
  Weg im Port: wie `EM29_ROT.EMD`/`EM30_HANGAR.EMD` ein Ersatzcontainer `EMD/EM4D_ANNETTE.EMD` (Generator in tools/, Asset im Repo), den
  `re15_bf_emd_datei` bzw. ein eigener Haken je Raum waehlt.
- **Tod:** die Form packt sie mit Griff-Clip 27 (G4-Clip 27, Packen Bild 8 @0x80105408, Stufe 3), Annette folgt der Klaue (Muster
  Hangar-Szene `birkin_formen.c`, Spider-Man an Knochen 15), Wurf, Fall, Liegen. Todespose aus den 0x4D-Clips (Kandidaten Clip 3, 20
  Bilder, endet am Boden; Pruefung im Bild in Phase 2). Keine neuen Clips der Form noetig; falls doch, nur im Generator stufe3/gen/.
- **Gore (Maximum aus RE1.5, RE2, 80 %):** Blut am Griff und beim Aufschlag mit den Blutspritzern der Form (`bb_blut_spawn`, ESP wie
  @0x80107438-f8), Blut-Textur EM049 fuer die Leiche. Mehr belegt das Material nicht.

## 5. Dialog und Stimmen

| Id | Sprecher | Text (Untertitel) | Herkunft | Stimme |
|---|---|---|---|---|
| 40 | Annette | W-William! You're alive! | RE2 woertlich (VOICE16 Kanal 9) | `synchro/STAGE5/room5090/main40.wav` |
| 41 | Annette | (Schrei) | RE2 (VOICE16 Kanal 12); Untertitel PORT-WAHL, Frage F6 | `synchro/STAGE5/room5090/main41.wav` |
| 42 | Leon | Annette! | PORT-WAHL nach Claires Ruf (VOICE16 Kanal 12) | `synchro/STAGE5/room5090/main42.wav` |

Freie Ids: 5090 hat im RDT msg 0 und 1 (`rdt_msgdump.py`), `synchro/STAGE5/room5090/` gibt es nicht, `stimmen_r39.c` hat keinen
5090-Eintrag, keine andere Spur belegt 5090-Ids (grep ueber alle Arbeitsbaeume). Ids 40-42 nach VERTRAG 1.2 ("Port-Ids liegen hinter den
Original-Nachrichten eines Raums (ab 40)"), unter VOICE_MAX_MSG 64. Die Aufnahmen liegen beim Nutzer; als Vorlage dienen die RE2-Abschnitte
aus 1.2.

## 6. Bits und Flags

Kein neues Bank-9-Bit: Annette in 5090 genau dann, wenn (3,89)=1 und (3,42)=0 (Szene gesehen = Kampf begonnen, (3,42) setzt sub01
@0x0129A). PORT-WAHL gegen das Original (@0x0122A unbedingt), weil der Nutzer sie erst nach dem Tor in 5090 haben will.

## 7. Offene Fragen an den Nutzer

- **F1 (Vorbild):** Die Szene ist auf der RE2-Claire-Disc. Ich habe gesucht: Leon-Disc vollstaendig (Typen, alle Xa-Eintraege), die
  Cutscene-Videos (nur FMV), den Rechner nach Abbildern (nur re2leon). Belegt sind nur Annettes Satz und Schrei aus den Claire-Sprachspuren
  im Repo. Fuer Raum, Kameraschnitte und Bildzahlen brauche ich ein Abbild der Claire-Disc (z. B. `re2claire.bin/.cue`). Bis dahin baue
  ich Kamera und Zeiten als PORT-WAHL nach #145.
- **F2 (Elza):** Auf der Disc steht in Elzas Zug keine Frau im Kittel (5091/6031). Vorschlag: Szene nur bei Leon.
- **F3 (Zug):** Wer faehrt nach dem Tor den Zug, wenn Annette hinten ist (6030 msg 18 "I'll be ready to start the train when you do!")?
  Und warum geht sie nach hinten? Ein Satz dazu waere neuer Text; ich schreibe keinen ohne Ihre Vorgabe.
- **F4 (Text):** Ihr RE2-Satz "W-William! You're alive!" setzt voraus, dass sie ihn fuer tot hielt. Passt das zu Ihrer Story?
- **F5 (Modell):** RE2-Annette (EM047) statt Elzas Kunst auf 0x4D in 6000/6030/5090? Sie entspricht #145 bis #147.
- **F6 (Schrei):** Untertitel fuer den Schrei ja oder nein; sonst nur Ton. (Gebaut: "Annette: Aaaah!".)
- **F7 (Bau):** Hieb links statt Griff und Annette rechts der Form statt links wie in #145 (Grund 8.2) - so recht?

## 8. Phase 2: gebaut (Commit 8e8a9fa01 und Nachtraege)

### 8.1 Dateien

| Datei | Inhalt |
|---|---|
| `re15_port/engine/src/annette_r41.c`, `re15_port/include/re15_annette_r41.h` | Spawn-Tor, Umleitung des Kampfstarts, Programm, Takt der Figuren, Modellwahl |
| `re15_port/engine/src/gen/annette_r41_texte.inc` | Texte 40-42 (Generator `re15_port/tools/r41_annette/texte_bauen.py`) |
| `re15_port/shared_assets/RE2/ANNETTE_4D.EMD`, `ANNETTE_4D_BLUT.EMD` | 0x4D mit RE2-Annette (Generator `re15_port/tools/r41_annette/annette_modell.py`, `--pruef` vergleicht bytegleich) |
| `re15_port/tools/r41_annette/an_lauf.sh`, `an_blatt.py` | Lauf im echten Fenster (gdigrab hwnd=, eigene exe-Kopie, nur eigene PID), Bildleisten |
| `re15_port/tests/unit/test_r41_annette.c`, `tests/integration/test_r41_annette.cmake`, `tests/unit/probes/r41_annette.cmake` | Tests (Abschnitt 9) |

Haken in gemeinsamen Dateien (je eine Zeile, Kommentar "Runde 41 Spur AN"): `scd_vm.c` op_sce_em_set (Spawn-Tor) und op_evt_exec
(Umleitung), `scd_room_setup.c` (install: Texte, Zustand je Raum), `enemy_ai_common.c` run_all (Takt, Besitz der Slots),
`platform/pc/main.c` (Modelldatei aus RE2/, Modell- und Audio-Haken, volle Drehung fuer die gekippte Leiche), `release/make_package.sh`
(Paket-Gate ANNETTE_4D.EMD/ANNETTE_4D_BLUT.EMD bytegleich). `tests/unit/test_r35_zgirl.c` T9: der Spawn-Zaehler in 5090 zaehlt die 0x4D
nur noch, wenn sie nach dem neuen Tor spawnt (Beleg: Annette-Tor, Abschnitt 6).

Birkin-/Form-/G5-Dateien sind nicht angefasst (birkin_formen.c, birkin_bahnsteig.c, g5auftritt, Uebergabepunkte). Die Szene liegt
VOR dem Kampfstart; Rueckstoss, Sturz und G5 laufen danach unveraendert.

### 8.2 Ablauf wie gebaut (Leon, gemessen im echten Lauf `re15_port/build/an_lauf/leon/debug.log`)

| Spielbild | Schritt | Beleg / Kennzeichnung |
|---|---|---|
| F280 | Leon erreicht Cut 12 in Wagen C: sub01 @0x01290/@0x01296 Evt_exec sub04 -> Programm der Szene. Sperre + Balken (Set(2,7,1)/(1,27,1) wie sub04 @0x012F2/@0x012F6), Cut 11. Form auf (776,0,-23713) Gier 3917, Drohen-Clip 10; Annette auf (4018,0,-22533), Handgeste Clip 22, Blick zur Form; Leon dreht sich zur Form (Plc_dest Modus 9 wie sub04 @0x01320) | Ausloeser: Original. Lagen: PORT-WAHL (Form = Bild-2-Lage FORMBLATT2.md Z. 223) |
| +20 | msg 40 "Annette: W-William! You're alive!" | RE2 woertlich |
| ~+45..52 | Pose Bild 2 (Clip 10 Bild 45-52) im Satz = #145 | STUFE3.md Abschnitt 3 |
| F420 | Signal (5,0x10): sie weicht zurueck (Clip 4), die Form dreht sich zu ihr (0x40 je Bild) | PORT-WAHL |
| F437 | Hieb links, Clip 4; Gier so gewaehlt, dass die Hand (Knochen 13) im Trefferbild ihr am naechsten ist (am Gerippe gemessen) | Clip 4 = G4-Clip 4 |
| F452 | Treffer in Clip-4-Bild 14 (EDD-Bit 16 Bild 14-17, G4-Clip 4, STUFE3.md); 4 Blutspritzer (Effekt 0 wie bb_blut_spawn @0x80107438-f8), Blut-Textur (ANNETTE_4D_BLUT = RE2 EM049-Textur) nachgeladen, msg 41 Schrei; sie wird herumgerissen und weggeschleudert | Trefferbild belegt, Rest PORT-WAHL |
| F464 | Aufschlag (Clip 6 Bild 0-12), 2 Blutspritzer | PORT-WAHL |
| F472 | liegt: Clip 6 Bild 12, 60 Grad nach hinten gekippt (rot_z -683), um 480 angehoben (am Gerippe gemessen: Pose reicht sonst bis y +482) | PORT-WAHL |
| +20 | Cut 12, msg 42 "Leon: Annette!" | PORT-WAHL |
| F553 | Programm ruft Evt_exec sub04: Kampfstart unveraendert (Pos_set (1200,-23350) @0x012FE, Member_set(0x0C,0x13) @0x0130A, Cut 15, Musik, Cut 12); die Form ist frei, die Leiche bleibt | Original |

Abweichungen vom Plan (Abschnitt 3/4) mit Grund:

1. **Hieb statt Griff.** Alle Angriffs-Clips der Form treffen erst rund 3700 vor dem Ursprung (gemessen aus BAHNSTEIG_FORM.EMD: Griff Clip 27
   Haende bei Bild 8 lokal x 4318 und z +-1580, also seitlich weit auseinander; Biss Clip 26 Kiefer ab Bild 14 bei x 3747; Hieb links Clip 4
   Hand im Trefferfenster bei x 3685-3864). Der Griff haette Annette nicht zwischen den Haenden gefasst; der Hieb trifft sauber.
2. **Annette rechts statt links wie in #145.** Links der Form (Bild-2-Reading (1166,-25211), Spawn (600,-25300)) steht sie nur rund 1600 vor
   ihr: zu nah fuer jeden Clip, und hinter ihr liegt die Suedwand. Rechts (Linie 20 Grad von der Form, Start 3450, Ende 3760) steht sie in
   Cut 11 frei im Bild (Projektion durch Cut 11 @0x1C0 mit `tools/form_kamera.py`: Kopf um (500,190), Fuss (473,468) von 640x480).
3. **Cut 12 fuer Leons Ruf.** In Cut 11 ist Leon nicht im Bild; Cut 12 ist die Kampfkamera (sub04 @0x01342).

Die Riesenform behaelt Form, Textur und Clips (keine neuen Clips).

### 8.3 Aussehen

0x4D (nur Leon 5090/6000/6030, Zensus aller RDTs) laedt `RE2/ANNETTE_4D.EMD`: Gerippe und 24 Clips des RE1.5-0x4D, Netz und Textur der
RE2-Annette (1.3). Ihr Verhalten in 6000/6030 bleibt das alte (gleiche Clips). Nach dem Treffer `ANNETTE_4D_BLUT.EMD`.
Kontaktblatt der Clips: Clip 0/1 = Gehen, 2 = Klettern, 4 = Zurueckweichen mit erhobenen Armen, 6 = Sturz zu Boden (Bild 12 sitzt sie),
11 = Zusammensinken, 17/18 = Kauern, 22/23 = Handgesten.

### 8.4 Elza

Keine Szene (Disc: keine 0x4D in 5091/6031); der Elza-Lauf zeigt den Kampfstart wie bisher (Abschnitt 10). Frage F2 bleibt.

## 9. Tests

| Test | Inhalt | Ergebnis |
|---|---|---|
| `unit_r41_annette` (neu) | Spawn 5090 je (3,89)/(3,42) mit dem echten RDT-Skript; 6030 Fuehrerstand vor/nach dem Tor (Original sub07 @0x010F8); 5091 ohne 0x4D; Programm (Kopf, Cut 11, Texte 40-42 woertlich, Ende Evt_exec sub04); Asset (dir[0..6] bytegleich EM4D, MD1/TIM = RE2 0x47/0x49, Parser 15 Knochen 24 Clips); ganzer Ablauf ueber VM + KI-Takt: Treffer in Clip-4-Bild 14, Hand 169 vom Ziel, 6 Blutspritzer, Leiche (hp -1, rot_z -683, y -480), Kampfstart sub04 (grid 0x13), einmalig | gruen |
| `integration_r41_annette` (neu) | echte exe, drei Laeufe: Leon mit Tor (alle Schritte in Reihenfolge, beide Modelle geladen, Form danach g=13, Annette am Ende hp -1), Leon ohne Tor (0x4D verworfen, keine Szene), Elza 5091 (keine Szene, keine 0x4D) | gruen (164 s) |
| `unit_birkin_bahnsteig`, `unit_birkin_szenen`, `unit_birkin_formen`, `unit_birkin_ai` | Form-/Birkin-Bestand | gruen |
| `integration_r41_birkin_pistole` | Kampf Leon/Elza bis G5 tot (ohne Tor, also ohne Szene) | gruen (249 s) |
| `integration_r35_zgirl_5090` | Endkampf 5090 nach Selbst-Tuer | gruen |
| Suite `^unit_|^prop_|^probe_` (1062 Tests, `-j 6`) | Bestand | 1061 gruen, 1 rot: `unit_r35_zgirl_wiedereintritt` zaehlte in 5090 zwei Spawns (0x4D + Boss); mit dem Annette-Tor ist es ohne (3,89) einer. Test angepasst (Sollwert aus `re15_annette_r41_spawn_weg`), danach gruen |

## 10. Sichtbelege (echtes Fenster)

Lauf `re15_port/tools/r41_annette/an_lauf.sh leon 5090 12 38 10 "A0.1,W1,U8.5,W60" RE15_SET_FLAG=3:89` (Debug-Sprung in Wagen B wie
integration_r41_birkin_pistole, Tuer 2, laufen bis Cut 12; Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER, eigene exe-Kopie,
gdigrab hwnd=, nur die eigene PID beendet). RE15_SET_FLAG=3:89 stellt "Tor offen" nach (das Spiel setzt das Flag selbst in ROOM6000 sub07).

| Datei | Inhalt |
|---|---|
| `AN_belege/spiel/leon_5090_szene.jpg` | Bildleiste: Szenenstart Cut 11, Satz, Pose Bild 2, Weichen, Hieb, Treffer mit Blut und Schrei, Sturz, liegt, Cut 12 "Leon: Annette!", Kampfstart sub04 (Cut 15), Kampf |
| `AN_belege/spiel/leon_5090_szene.mp4` | ganzer Lauf als Video (10 Bilder/s) |
| `AN_belege/spiel/vergleich_145_port.jpg` | 80 % #145 neben dem Port-Bild in Cut 11 waehrend des Satzes |
| `AN_belege/spiel/elza_5091.jpg` | Elza 5091: keine Frau im Kittel, Kampfstart wie bisher |

**RE2-Vorbild nebeneinander: nicht moeglich.** Die Szene liegt auf der Claire-Disc, die auf dem Rechner fehlt (1.2, F1); es gibt kein
RE2-Bild der Szene. Statt dessen #145 (80 %) neben dem Port.

Beobachtet: Der Kampfstart nach der Szene schneidet wie bisher auf Cut 15 (sub04 @0x0130E); die Riesenform steht dann bei (1200,-23350)
direkt vor dieser Kamera (Auge (46,-1236,-24302)) und fuellt das Bild ganz aus. Das ist der bestehende Kampfstart der Form-Spur, nicht
diese Szene (Hinweis an die Form-Spur, Abschnitt 11).

## 11. Offen

- F1-F7 (Abschnitt 7).
- Stimmen: `synchro/STAGE5/room5090/main40.wav`, `main41.wav`, `main42.wav` fehlen (Nutzer). Vorlagen: RE2 Claire-Disc
  `info/re2leon/PL0/VOICE/Anette/VOICE16.XAS[9]_3.wav` (Satz), `Anette/VOICE16.XAS[12]_2.wav` (Schrei).
- Hinweis an die Form-Spur: Kampfstart sub04 Cut 15 mit der Riesenform zeigt nur ihre Haut (Abschnitt 10).

## N1. Nachbesserung 1 (2026-11-02)

Auftrag: RE2-Szene belegen und die 5090-Szene angleichen; Zugstart-Szene 6030 (Nutzer-Idee, Story-Nachtrag "Sherry ist im
Zug", Ids 27-30 nach Absprache mit ST80-T); F7 recherchieren (nur Dossier). Nutzerantworten: F1 Claire-Daten ergaenzt
(`info/re2clair/`, dazu das Abbild `Downloads/ePSXe2018/Resident Evil 2 Claire.cue`), F2/F8 ok, F9 Schrei nur Ton.

### N1.1 Die RE2-Szene (Claire-Disc)

**Disc:** `Resident Evil 2 Claire.cue` = SLES-10972 (SYSTEM.CNF `BOOT = cdrom:SLES_109.72;1`), europaeische PAL-Fassung, nicht
DualShock. Werkzeuge `AN_belege/werkzeug/n1/` (iso.py liest das Abbild nur, c_scd.py SCD-Dump der Claire-RDTs, c_xa.py
Xa-Tabelle, c_msg.py Nachrichten).

**Raum:** `PL1/RDT/ROOM6141.RDT` (Stage 6, Raum 0x14; Typ-Zensus aller 125 Claire-RDTs: nur 6141 hat 0x49 und eine
Birkin-Form 0x33). Skript MAIN sub02 @0x00E32..@0x01192:

- Ausloeser: MAIN sub00 @0x00C58 Ck(1,1)=0, @0x00C60 Ck(0x22,0x2B)=1, @0x00C68 Ck(3,0x42)=0 -> @0x00C6C Sce_em_set Slot 0
  Typ 0x49 (Annette) auf (-22710,0,-22970), @0x00C82 Slot 1 Typ 0x33 (Birkin) geparkt auf (-6600,-9000,-14800), @0x00D0E
  Evt_exec sub02: die Szene beginnt beim Raumeintritt. Ende @0x01186 Set(5,0x0C), @0x0118A Set(3,0x42), @0x0118E gosub
  sub08 (Plc_ret, Sperren auf, Cut_auto). sub03 ist das spaetere Sterbegespraech mit Claire.
- Stimmen: Xa-Tabelle der Claire-STAGE6.BIN (Zeiger nach 0x800c611c, STAGE6.BIN @0x34..@0x3C `0c 80 01 3c 1c 61 22 ac`;
  Tabelle 0x8011A4D4 = Datei 0x4D4, je Eintrag {Kanal, Block, Laenge}); VOICE16.XAS von der Disc dekodiert und erkannt
  (Whisper medium.en): Xa 0x30 Kanal 10 Block 138 Laenge 11 = 1,17 s "William."; Xa 0x31 Kanal 8 Block 37 Laenge 37 = 3,95 s
  "W-William! You're alive!"; Xa 0x32 Kanal 10 Block 102 Laenge 13 = 1,39 s Schrei. (Die frueher genannten Kanal 9/12 aus
  `info/re2leon/PL0/VOICE` sind eine andere Fassung.)
- Untertitel: die Nachrichten von ROOM6141 (Block @0x1620) sind leer (`fa 00 fa 02` und Pausen `fd nn`): RE2 zeigt in dieser
  Szene keinen Text. Der Port behaelt Untertitel fuer gesprochene Zeilen (Port-Regel), der Schrei hat keinen (F9).
- Kameras (RID @0x64, 32 Byte je Schnitt): Cut 7 weit auf beide, Cut 8/9 Schuss/Gegenschuss Annette/Claire, Cut 2 Gang von
  hinter Williams Landestelle, Cut 0x0A tief von Annettes Seite auf William, Cut 0x0B hoch von Williams Seite auf Annette,
  Cut 6 weg von der Tat (Claire vor dem Labor).

**Echte Aufnahme** (Unteragent, DuckStation, neues Spiel Claire, Einstieg per Tuer-Patch im RAM; Weg, PAL-Adressen,
Zeitachse und 97 Savestates: `AN_belege/re2_claire/zeitachse.md`, `ram_reihe_voll.md`, Bildleisten `re2c_*.jpg`, Clip
`re2c_szene_320.mp4` mit Ton). Rate: PAL 25 Spielbilder/s (VSync-Teiler 2 @0x8002ade8 `90847662`); die Sleeps im Skript
sind Spielbilder, der Port rechnet sie mit 30/s (Takt des Ports).

| Schritt | RE2-Skript | Aufnahme (Lauf 1) |
|---|---|---|
| Cut 7, Musik, Krach | @0x01024, @0x0103A, Se_on(3,0x0B) @0x01040 | Krach 49,40 s |
| 25 Bilder, beide sehen hin, 36+1 Bilder | @0x0104C, Plc_neck @0x0105A/@0x01068, @0x01072/@0x01076 | |
| "William." | Xa 0x30 @0x0107A, Wsleep @0x01084 | 52,93 s |
| sie geht (3 Strecken), Cut 2 nach Strecke 1 | Plc_dest @0x010A2/@0x010B8/@0x010DE, Cut 2 @0x010B2 | Cut 2 57,47 s |
| Birkin faellt herein, Se_on(2,0x0A) | Member_set 0x17=1 @0x010CA, @0x010CE | 58,7-59,6 s (y -9000 -> 0) |
| sie bleibt stehen, 30 Bilder | Plc_motion 0x0C @0x010F9, @0x010FE | Annette (-11097,-15001), Birkin (-6600,-14800) |
| Cut 0x0A, Se_on(2,0x0C), 60 Bilder | @0x01102, @0x01104, @0x01110 | 61,17 s |
| Cut 0x0B, "W-William! You're alive!", 30 + Xa-Ende | @0x01114, Xa 0x31 @0x0111A, @0x01132, @0x01136 | 64,20-68,14 s |
| Annette Clip 2, Cut 0x0A, Hieb, 45 Bilder | @0x01138, @0x0113C, Member_set 0x17=2 @0x01142, @0x01146 | Hieb 70,2-71,1 s, Blut ab 71,09 s, Vorstoss 1397 |
| Cut 6 (Claire), Schrei aus dem Off, Annette liegend gesetzt | @0x0114A, Xa 0x32 @0x01150, Pos_set (-12300,900,-15300) @0x01162 | 72,23 s; liegend nie im Bild |

### N1.2 Port 5090 nach RE2 (angeglichen)

Ablauf jetzt (Programm `k_prog`, `annette_r41.c`; jede Bildzahl mit RE2-Adresse im Code):

| RE2 | Port | Kamera Port (RE1.5-Schnitte 5090) | Beleg / PORT-WAHL |
|---|---|---|---|
| Cut 7 + Krach | Ausloeser wie bisher (erster Cut 12, sub01 @0x01296), Krach = ANNETTE_SE Satz 0 | Cut 12 (weit, Leon vorn) | Ton RE2 belegt (an_se.md); Kamera PORT-WAHL |
| 25 / 36+1 Bilder, beide sehen hin | Sleep 25; Leon Plc_dest Modus 9 (Form sub04 @0x01320), Annette Modus 9 Bit 0x21 (Form 6030 sub08 @0x01160); Sleep 36+1 | Cut 12 | RE2-Bildzahlen |
| "William." | msg 43 (neu, RE2 woertlich), Sleep 35 (= Xa-Laenge 11 Sektoren) | Cut 12 | RE2 |
| 3 Strecken, Cut 2 nach Strecke 1 | Plc_dest Modus 4 (Form 6000 sub02 @0x00FB0) ueber W1/W2 nach S, Teilung 47 %/76 % wie RE2 | Cut 15 nach W1 | Kamera PORT-WAHL (Cut 15 = Auge hinter der Landestelle, wie RE2 Cut 2) |
| Birkin faellt | Riesenform faellt mit dem ORIGINAL-Sturz von 5080 sub03 (Pos_set y -5600 @0x00822, 1 Bild halten @0x0082A, 14 x 400 @0x0082E/@0x00832), Satz 1 | Cut 15 | RE1.5-Werte statt RE2 (andere Figur); im Takt, siehe unten |
| 30 Bilder, Cut 0x0A + Bruellen + 60 | Sleep 30, Cut 11 + Satz 2, Drohen-Clip 10, Sleep 60 | Cut 11 (von Annettes Seite) | RE2 |
| Cut 0x0B + Satz | Cut 15, Sleep 1, msg 40, Sleep 118 (30 + Xa 0x31) | Cut 15 (Form links, Annette rechts) | RE2 |
| Clip 2 + Hieb + 45 | Annette 0x4D-Clip 4 (Arme hoch) an Ort und Stelle, Form Hieb links Clip 4 (Treffer Bild 14), Blut 4 + 2 + 2 | Cut 11 | Clip-Wahl PORT-WAHL (naechster 0x4D-Clip zu RE2 M02) |
| Cut 6 + Schrei + liegend | Cut 13 (nur Leon), Schrei als Stimme `synchro/STAGE5/room5090/main41.wav` ohne Untertitel, Annette 1203 von der Form weg / 299 seitlich (RE2-Versatz e07 -> e23) liegend | Cut 13 | RE2 |
| (Steuerung zurueck) | msg 42 "Leon: Annette!" (F8), Sleep 60, dann sub04 wie bisher | Cut 13, dann sub04 | PORT-WAHL (vom Nutzer freigegeben) |

Entfallen gegen Phase 2: Zurueckweichen, Flug, Aufschlag und Kippen im Bild (RE2 zeigt Annette liegend nie), Untertitel des
Schreis.

Lagen (PORT-WAHL, gemessen; Werkzeuge `werkzeug/n1/proj5090.py`, `lage2.py`, `lagesuche.py`):
- Landestelle L (500,-22600), Haltepunkt S (4131,-23573), Abstand 3759 (Trefferfenster des Hiebs lokal x 3685..3864, 8.2).
  In Cut 15 liegt die Form links (Bild-x bis 155), Annette rechts (259..302). Der erste Fensterlauf mit L (1000,-22800) zeigte
  beim Satz nur die Haut der Form und Annette zwischen ihren Beinen.
- Start A0 (8000,-24250): in Cut 12 bei Bild-x 117 neben Leon (156; mit A0 (9500,-22900) stand sie genau hinter ihm),
  1614 vom naechsten Bodenfeuer (main00 @0x0110E..@0x0120E, (9900,-24500)), Weg A0 -> S mindestens 1035 von allen Feuern.
- Sturz im Takt statt im Programm: Bank 5 Wort 1 (Bits 0x20..0x3F) wird jedes Bild gewischt (scd_vm.c, FUN_8003ebf4), die
  Ankunft (5,0x21) ist also wie im Original ein Ein-Bild-Puls; eine For-Schleife im Programm blockierte das Warten und
  verpasste ihn (Test: Puls F175, Schleife bis F176, Szene hing). Jetzt wartet das Programm wie RE2 (gosub sub10) waehrend
  des Sturzes.

### N1.3 Zugstart 6030 (Nutzer-Idee, Wortlaut = Vorschlag)

Ausloeser: ROOM6030 sub00 @0x00D28 Switch(Cut) Case 0 -> sub07; bei (3,89)=1 spawnt sub07 sie nicht mehr und ruft @0x01132
Evt_exec sub05 (Licht-Schleife). Der Haken (`re15_annette_r41_evt_exec`, scd_vm.c op_evt_exec) leitet genau diesen Aufruf
um, wenn (9,133)=0 und keine 0x4D im Raum steht (sonst lief beim ersten Besuch nach dem Tor sub08; dann kommt der Zugstart
beim naechsten Eintritt). Wagen-Eintritt (Cut 6/8/12, sub06 @0x010C4 Evt_exec sub05) vor dem Zugstart: die Tuer nach 5090
bleibt beim Original-Text msg 10 "I have to get this thing moving first!" (Aot_reset-Bytes sub06 @0x0106A), die
Licht-Schleife startet erst danach. So steht Annette in 5090 erst nach dem Zugstart (Spawn-Tor 5090 = (3,89) und (9,133) und
nicht (3,42)).

Programm (38 Segmente, 290 Byte Original aus ROOM6030, 68 Byte Port-Befehle in Original-Form; der Riegel vergleicht jedes
Original-Segment mit der RDT):

| Schritt | Bytes |
|---|---|
| Set(9,133,1) | Port (Set-Form) |
| Sperre + Balken | @0x0113E |
| Annette am Pult | Sce_em_set @0x010F8 |
| Sleep 1, Cut 1, Leon dreht sich zu ihr, Sleep 10 | @0x01146..@0x0115B |
| Work_set Annette, Cut 3, Pult-Clip 0x12, Sleep 80 | @0x011CC, @0x011F6 |
| Zug-an-Ton (Satz 3) | Port (Set(5,0x10)) |
| Pultlicht Set(1,28), Cut 4, 2 x Sleep 15, Set(1,28,0) | @0x01210, @0x01216, @0x01220 (ohne Cut 5 = rotes WARNING des gescheiterten Starts, im Fensterlauf gesehen; PORT-WAHL) |
| Cut 1, Sleep 60; Licht-Schleife (Fahrt) | @0x01254, Evt_exec sub05 @0x01132 |
| msg 27 + Gesten Annette | Port Message_on, Gestenblock @0x0129A (nach msg 16) |
| Krach (Satz 0), Sleep 25 | Port; RE2 @0x0104C |
| beide drehen sich zur Wagentuer, Sleep 36 | @0x0125A + Plc_dest (Form @0x01150) + @0x01358; @0x012E2 + Plc_dest (Form @0x01160) + @0x0128A; RE2 @0x01072 |
| msg 28 + Gesten Annette | Gestenblock @0x01178 (nach msg 11) |
| msg 29 + Gesten Leon | @0x0125A, Gestenblock @0x01262 (nach msg 15) |
| msg 30 + Gesten Annette | @0x012E2, Gestenblock @0x012EA (nach msg 18) |
| sie geht zur Tuer, versteckt | Plc_dest Modus 4 (Form 6000 sub02 @0x00FFA) auf (-3873,-22550), Warten @0x0128A, Pos_set-Bytes 6000 sub02 @0x0100A |
| Sleep 30, Sperren auf, Cut_auto, Plc_ret, Evt_end | @0x0125A, @0x01364, @0x01374 |

Tuer-Ziel (-3873,-22550): die Wagentuer Slot 0 (rect x -5100..-4200) liegt in der Wand SCA #13 (x -5941..-4341, ROOM6030
SCA @0x708); erreichbar ist die Wandflaeche x -3873 (= Ziel der Tuer Slot 1 @0x00C52). Erster Fensterlauf mit x -4800: sie
blieb an der Wand stehen und kam nie an.

Zeilen (Vorschlag, PORT-WAHL; der Nutzer kann den Wortlaut aendern, Generator `tools/r41_annette/texte_bauen.py`):

| Id | Zeile | Stimme |
|---|---|---|
| 27 | Annette: There! The train's moving. | `synchro/STAGE6/room6030/main27.wav` |
| 28 | Annette: What was that? / ...Sherry is back there! | `synchro/STAGE6/room6030/main28.wav` |
| 29 | Leon: I'll check it out. | `synchro/STAGE6/room6030/main29.wav` |
| 30 | Annette: No. She's my daughter. / Stay here and keep an eye on / the controls. | `synchro/STAGE6/room6030/main30.wav` |

5090 dazu: Id 43 "Annette: William." -> `synchro/STAGE5/room5090/main43.wav`; 40 / 41 (nur Ton) / 42 wie bisher.

Toene (`AN_belege/an_se.md`, Unteragent): Satz 0 KRACH = RE2 Se_on(3,0x0B) ROOM6141 @0x01040 = ENEMSE-Bank 64 Satz 0x0B
(Lader und RAM belegt, Gegenprobe gegen die Aufnahme r = 0,988); Satz 1 STURZ = Se_on(2,0x0A) @0x010CE; Satz 2 BRUELL =
Se_on(2,0x0C) @0x01104; Satz 3 ZUG_AN = RE2 ROOM7020 sub04 @0x014AA Se_on(2,0x0C) (Hochfahren nach Set(4,0x88) "Strom fuer den
Zug"). Im Zugstart ist das Krachen aus dem hinteren Wagen derselbe Satz 0 (PORT-WAHL: der RE2-Ton der Szene, auf die es
hinauslaeuft).

**Flag:** Bank 9 Bit 133 "Zugstart gesehen". Zensus `tools/r36_kartenbits/bank9_zensus.py` (240 RDTs, 0 Desync): frei im
Original; in keinem r41-Zweig vergeben; VERTRAG 1.1 "125-133 vergibt der Orchestrator" -> **bitte bestaetigen** (noch nicht in
`test_r36_kartenbits.c` k_port[] eingetragen, weil die Zuteilung beim Orchestrator liegt).

**Integration mit ST80-T** (`.claude/worktrees/r41_story80/analysis/story80/TEXTE_UMSETZUNG.md` Abschnitt 5): ST80-T leitet in
6030 sub02 (von sub01 @0x00D70) und sub08 (von sub07 @0x010E8) um, diese Spur nur sub05 (von sub07 @0x01132 und sub06
@0x010C4). Beide Haken pruefen nur ihre eigene Sub-Nummer; Ids 21-26 (ST80-T) / 27-30 (AN). Keine Kollision; beim
Zusammenfuehren beide Haken-Zeilen in op_evt_exec behalten.

### N1.4 Bestehende 6030-Zeilen gegen den neuen Ablauf (nicht umgeschrieben, zur Entscheidung)

1. msg 20 "You can't get off the train right now." (Ausgang, sub06 @0x01098 ab (3,89)=1) erscheint jetzt auch vor dem
   Zugstart, wenn der Zug noch steht.
2. sub06 @0x01092 wechselt die Musik schon bei (3,89)=1, also vor dem Zugstart.
3. ST80-T: Sherry sitzt im Wagen von 6030 (sub06 @0x01044 Sce_em_set 0x4B, unbedingt). Annette laeuft nach msg 30 nach hinten
   und muesste auf dem Weg nach 5090 an Sherry vorbei; ihre Sorge "Sherry is back there!" und dass sie dann in Wagen C (5090)
   steht, setzt voraus, dass sie Sherry dort nicht findet oder weitergeht. Ein Satz dazu waere neuer Text.
4. msg 18 "I'll be ready to start the train when you do!" und msg 10 "I have to get this thing moving first!" passen zum neuen
   Ablauf (kein Widerspruch).
5. RE2-Satz "W-William! You're alive!" setzt weiter voraus, dass sie ihn fuer tot hielt (F4 aus Abschnitt 7, unbeantwortet).

### N1.5 F7 (fuer spaeter, nicht gebaut): die Szene "das Monster ist William"

- **Leon-Disc** (SLUS-00748): `PL0/RDT/ROOM4090.RDT` MAIN sub02 (Kanalisation, Annette und Ada; Stimmen VOICE04 vom Abbild
  `re2leon.bin` dekodiert, Whisper medium.en, nicht von Hand gegengehoert): @0x0271E Annette "Annette Birkin. My husband is the
  man responsible for the creation of the T-virus. William Birkin." @0x0278C "It's capable of creating the ultimate
  bioweapon. Its potential is even greater than that of the T-Virus." @0x027A6 Ada "Then that must mean the creature in the
  police department is..." **@0x027BA Annette "Precisely, my husband, William."** @0x027D4 "And it's all Umbrella's fault. None
  of this would have happened if they hadn't tried to steal his research away from him." **@0x02818 `6f 0b` Movie_on**, danach
  @0x0283C "So those rats were the carriers of the virus." @0x0284C "As a result of his virus-induced transmutation, William
  should have lost any prior memories he had as a human by now." @0x02866 "Even worse, every G-virus bioweapon, including
  William, has the ability to implant embryos into other creatures."
- **Claire-Disc**: `PL1/RDT/ROOM4081.RDT` MAIN sub03 (Annette und Claire, VOICE14): Xa 0x11 @0x02D50 "That's impossible. I told
  her to go to the police station. What's she doing here? William will be after her." Xa 0x12 Claire "William?" **Xa 0x13
  @0x02D92 "That's right. The creature that's been causing all this destruction is none other than my own husband."** Xa 0x15
  "As a result of his virus induced transmutation, William should have lost any prior memories he had as a human by now." Xa
  0x16 "I tried to save him, but I'm afraid it's already too late."; Movie_on 0x0B dort in sub08 @0x036A2.
- **FMV:** Movie_on 0x0B in beiden Raeumen; Datei je Szenario nach Raumnamen: Leon `PL0/ZMOVIE/R409.STR` (50 142 240 Byte,
  4293 Bilder MDEC 0x3800 Version 2, 320x160, XA Stereo 37,8 kHz coding 0x01, 143,1 s bei 30 Bildern/s), Claire
  `PL1/ZMOVIE/R408.STR` (44 011 520 Byte). Inhalt R409 (Stichbilder alle 10 s aus `info/re2leon/PL0/ZMOVIE/R409.STR[0].avi`):
  USS-Soldaten, William im Labor, Annette, der angeschossene William, die Mutation, HUNK. Die Zuordnung Index 0x0B -> Datei
  steht nicht als Text in den EXEs (kein "R409"/".STR" in SLUS_007.48 / SLES_109.72); belegt sind nur Raum-Aufruf und Name.
- **Port:** re15_str.c erkennt Videosektoren nur an der Magic 0x0160 (re15_str.c:38; R409 hat Submode 0x42 statt 0x48 wie
  CAPCOM.STR, das stoert nicht), dekodiert MDEC 0x3800 Version 2 ueber die BSS-Kette (wie CAPCOM.STR) und XA Stereo 37,8 kHz
  (re15_xa.c, wie CAPCOM.STR). Fehlen wuerden: Ausgabe 320x160 mittig in 320x240 (der Spieler zeigt 1:1 Vollbild), ein
  Ausloeser (Movie_on 0x6F ist in scd_vm ein Durchlauf) und die Datei unter shared_assets. Abspielbar also mit kleinem Umbau.

### N1.6 Tests

| Test | Inhalt | Ergebnis |
|---|---|---|
| `unit_r41_annette` | Spawn 5090 je (3,89)/(9,133)/(3,42); 6030 vor dem Tor Original; Programm 5090: Schnittfolge 15/11/15/11/13, RE2-Sleeps, Message_on 43/40/42, kein Text fuer 41, kein Pos_set (Sturzwerte gegen ROOM5080-Bytes); Zugstart: jedes Original-Segment bytegleich mit ROOM6030, Ids 27-30 mit Wortlaut, Tuer-Programm = @0x0106A + @0x010CA; Szene 5090 ueber VM + KI-Takt (Sturz 1+14 Bilder ab y -5600, Schnitte 12 15 11 15 11 13, Annette steht bei S, Treffer Clip 4 Bild 14 Hand 87, Blut 8, Toene je 1, Schrei 1, Leiche RE2-Versatz, sub04 frei, einmalig); Zugstart 6030 (Tuer msg 10 vorher, Tuer danach, Programm einmal, (9,133), Toene Zug an vor Krach, Annette versteckt, Wiedereintritt ohne Szene) | gruen |
| `unit_r41_annette_se_*` (3, Unteragent) | ANNETTE_SE.VBS Bank/Toene/Lagen | gruen |
| `integration_r41_annette` (4 Laeufe, echte exe) | leon_tor (Schritte und Toene in Reihenfolge, beide Modelle, Form g=13, hp -1), leon_ohne_zug (0x4D verworfen), elza 5091, zug6030 (Zugstart, Toene 3 vor 0, Annette versteckt) | gruen (222 s) |
| Suite unit/prop/probe, `integration_r41_birkin_pistole`, `integration_r35_zgirl_5090` | Bestand | siehe N1.9 |

### N1.7 Sichtbelege (echtes Fenster, gdigrab hwnd=, eigene exe-Kopie, Renderer des Nutzers)

Laeufe `tools/r41_annette/an_lauf.sh leon 5090 12 36 10 "A0.1,W1,U8.5,W60" RE15_SET_FLAG=3:89,9:133` und
`an_lauf.sh zug6030 6030 8 34 10 W1 RE15_SET_FLAG=3:89,4:23 RE15_PLAYER_POS=-3873,-23487,0,0` (Debug-Sprung = Eintritt Cut 0).
Ablage `AN_belege/spiel_n1/`:

| Datei | Inhalt |
|---|---|
| `vergleich_re2_port_5090_1.jpg`, `_2.jpg` | neun Schritte RE2 (echte Aufnahme) neben dem Port |
| `leon_5090_szene_n1.jpg`, `.mp4` | Port 5090 Bildleiste und Video (10 Bilder/s, ohne Ton) |
| `leon_6030_zugstart.jpg`, `.mp4` | Port 6030 Zugstart |

RE2-Seite mit Ton: `AN_belege/re2_claire/re2c_szene_320.mp4` und Bildleisten `re2c_*.jpg`.

Gesehen: Cut 12 zeigt Annette links neben Leon beim Krach und bei "William."; Cut 15 sie auf die Kamera zu, dann faellt die
Form links ins Bild (wie RE2 Cut 2); Cut 11 Anstarren; Cut 15 Satz mit Annette rechts frei im Bild; Cut 11 Hieb, Blut und
Kittel mit Blut ab Bild 266; Cut 13 nur Leon, Schrei ohne Untertitel, "Leon: Annette!". Unterschied zu RE2: RE2s Blutwolke
ist deutlich groesser als die acht Port-Spritzer. Zugstart: Pult (Cut 3), Pult ohne WARNING (Cut 4), Dialog 27-30 in Cut 1,
Annette geht zur Wagentuer, Steuerung zurueck.

### N1.8 Dateien

Neu/geaendert: `engine/src/annette_r41.c`, `include/re15_annette_r41.h`, `engine/src/gen/annette_r41_texte.inc`,
`tools/r41_annette/texte_bauen.py`, `tests/unit/test_r41_annette.c`, `tests/integration/test_r41_annette.cmake`,
`tests/unit/probes/r41_annette.cmake`, Haken-Kommentare `scd_vm.c`/`scd_room_setup.c`, `platform/pc/main.c` (2 Zeilen:
ANNETTE_SE.VBS, Stimme ohne Text). Unteragent Toene: `tools/r41_annette/an_se_cut.py`, `shared_assets/RE2/ANNETTE_SE.VBS`,
`engine/src/gen/re2_annette_se_bank.inc`, `platform/pc/src/annette_se_pc.c`, `tests/unit/probe_r41_annette_se.c`,
`tests/unit/probes/r41_annette_se.cmake`, `release/make_package.sh` (Paket-Gate), `AN_belege/an_se.md`. Belege
`AN_belege/re2_claire/` (Aufnahme), `AN_belege/spiel_n1/`, `AN_belege/werkzeug/n1/`.

### N1.9 Testlage und Commits

- Bau `bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK.
- `unit_r41_annette`, `unit_r41_annette_se_bank/_toene/_lagen`: gruen. `integration_r41_annette` (4 Laeufe): gruen, 222 s.
- Suite `^unit_|^prop_|^probe_` ohne die Umgebungs-Skripttests (`-E "releaseskript|android|r37_masken36_ausschluss|paket"`;
  sie hingen im ersten Lauf je bis zum Zeitlimit 900 s, wie von ST80-T gemeldet "Skript-Riegel haengen in df -Pk"), `-j 4`:
  1046/1048 gruen. Rot nur Zeitlimits: `unit_r41_birkin_wand_griff` (360 s unter Last; einzeln wiederholt gruen, 138 s) und
  `unit_r39_titelversion_ablauf` (1200 s; startet das echte `release_alles.sh`, dieselbe Skript-Familie, nicht diese Spur).
- `integration_r41_birkin_pistole` gruen (247 s), `integration_r35_zgirl_5090` gruen (147 s).
- Commits: 9c3654e6a (Code, Toene, Tests), Doku-Commit mit diesem Abschnitt und den Belegen.

### N1.10 Offen

- Bank-9-Bit 133 vom Orchestrator bestaetigen und in `test_r36_kartenbits.c` k_port[] eintragen.
- Stimmen (Nutzer): `synchro/STAGE5/room5090/main40..43.wav`, `synchro/STAGE6/room6030/main27..30.wav`.
- N1.4 Punkte 1-3 (Text/Story), F4.
- ZUG_AN-Alternative laut an_se.md: das eigentliche Anfahren des RE2-Zuges liegt in ROOM7030 sub12 (Hebel- und Schlagtoene,
  ohne Hochlaufen); gebaut ist ROOM7020 0x0C.
- RE2-Bildzahlen sind Spielbilder der PAL-Fassung (25/s); ob eine NTSC-Claire-Disc dieselben Sleeps hat, ist ohne diese Disc
  nicht pruefbar.
- Kampfstart sub04 Cut 15 zeigt weiter nur die Haut der Form (Hinweis an die Form-Spur, Abschnitt 10).
- F7 nach Nutzerwunsch spaeter (N1.5).

## N2. Nachbesserung 2 (2026-11-02/03)

Auftrag: fuenf Entscheide des Orchestrators zu N1.4 / N1.10. Je Punkt Beleg, Bau, Nachweis.

### N2.1 Bank 9 Bit 133 "Zugstart gesehen" (Entscheid 1: bestaetigt)

- `tests/unit/test_r36_kartenbits.c`: Include `re15_annette_r41.h`, k_port[]-Zeile `RE15_AN_ZUG_BANK/RE15_AN_ZUG_BIT`.
- `analysis/befunde_runde41/VERTRAG.md` 1.1: Zeile **AN** (133 = Zugstart-Szene 6030 gesehen).
- Kopf `include/re15_annette_r41.h`: "bestaetigt 2026-11-02".
- `unit_r36_kartenbits`: `ok: Bit 133 RE15_AN_ZUG_BIT ... im Original frei`, `ok: 61 Port-Bits gegen 85 Original-Bits: 0
  Kollision(en)`. Hinweis fuer die Integration: teil_p sammelt die Bits in `unsigned alle[64]`; mit 56 k_port-Zeilen + 5
  Lageplan-Bits sind 61 belegt, 3 frei. Kommen beim Zusammenfuehren weitere Spur-Bits dazu, muss das Feld groesser werden.

### N2.2 msg 20 erst, wenn der Zug faehrt (Entscheid 2)

Was das Original im Wagen tut (ROOM6030 sub06 = Eintritt mit Cut 6/8/12, sub00 @0x00D38):

| Zweig | Bytes | Wirkung |
|---|---|---|
| (3,89)=0 @0x0105C..@0x01088 | @0x01064 `54 01 01 00 00 00`, @0x0106A `46 03 01 31 0a 00 ff ff 00 00`, @0x01074..@0x01084 Sca_id_set 0..4 | Slot 3 (nach 5090) = msg 10; **Slot 2 bleibt die Tuer aus main00 @0x00C72** |
| (3,89)=1 @0x0108A..@0x010C8 | @0x01092 `54 00 01 00 00 00`, @0x01098 `46 02 01 31 14 00 ff ff 00 00`, @0x010A2 Obj_model_set, @0x010C4 Evt_exec sub05 | Slot 2 = msg 20 "You can't get off the train right now.", Slot 3 bleibt die Tuer nach 5090 |

main00 @0x00C72 (Slot 2, Ausgang): `3b 02 02 31 00 00 f4 b6 c0 ae dc 05 b0 04 9d 06 f0 f1 ba d8 00 0c 05 00 03 02 00 00 00 00
00 00` = Rechteck (-18700,-20800, 1500 x 1200), Ziel Stage-Byte 5 Raum 0 = ROOM6000, (1693,-3600,-10054), Richtung 0x0C00, Cut 3.

Port bis NB1: bei (3,89)=1 ohne Zugstart lief @0x01098 (Slot 2 = msg 20), danach leitete der Haken sub05 auf das Tuer-Programm um
(Slot 3 = msg 10) - beide Wagentueren zu, obwohl der Zug noch steht.

Jetzt (`annette_r41.c` k_tuer, 44 Byte, nur Original-Bytes): main00 @0x00C72 (Slot 2 wieder die Tuer nach 6000) + sub06 @0x0106A
(Slot 3 msg 10) + Evt_end @0x010CA. Vor dem Zugstart verhaelt sich der Ausgang also wie im Original vor dem Start; kein neuer
Text. Nach dem Zugstart ((9,133)=1) leitet der Haken nicht mehr um, sub06 laeuft unveraendert, @0x01098 gilt: msg 20.

Warum das Zugstart-Programm selbst kein Aot_reset braucht: Fuehrerstand (Cut 0, sub07) und Wagen (sub06) trennt die Wand SCA #13
(N1.3); hinueber geht es nur durch die Tuer Slot 0 (main00 @0x00C32, Ziel `05 03 08` = ROOM6030 Cut 8), die den Raum neu laedt
und damit sub06 laufen laesst. Der Ausgang (x -18700..-17200) liegt im Wagen.

Riegel `unit_r41_annette`: Tuer-Programm bytegleich mit @0x00C72 / @0x0106A / @0x010CA; Wagen vor dem Zugstart: Slot 2 Tuer,
Ziel 5/0, Slot 3 msg 10; nach dem Zugstart: Slot 2 msg 20 (Ereignis 20), Slot 3 Tuer.

Fenster (gdigrab hwnd=, eigene exe-Kopie): `spiel_n2/tuer_6030_ausgang_vor_nach.jpg`, `tuer_6030_vor.mp4`, `tuer_6030_nach.mp4`.
Weg: `an_lauf.sh tuer_vor 6000 6 46 4 "W8,A1,W2,R0.5,W1,A1,W12" RE15_SET_FLAG=3:89,4:23,3:100,3:239
RE15_PLAYER_POS=1763,-9700,1024,2` (nachher dasselbe mit `,9:133`). (3,100)/(3,239) markieren die beiden 6000-Szenen
(sub00 @0x00F40 / @0x00F22) als gelaufen, sonst haelt die Annette-Szene in 6000 den Spieler fest (erster Versuch). Leon geht durch
6000 Tuer Slot 3 (main00 @0x00DDA) in den Wagen (Cut 6, (-17970,-20380)), dreht (Gier 1100 -> 2540), Aktion am Ausgang:
- vorher: `[aot] DOOR FIRE slot=2 rect=(-17950,-20200,hw=750,hh=600) target_cut=3 spawn=(1693,-3600,-10054)`, Tuerfahrt,
  ROOM6000;
- nachher: kein DOOR FIRE, "You can't get off the train right now." ab Bild 142, Leon bleibt im Wagen.

### N2.3 Musikwechsel beim Tor (Entscheid 3): kommt aus dem Original-Skript, unveraendert

- ROOM6030 sub06: @0x0105C Ifel / @0x01060 Ck(3,89)=0 `21 03 59 00` -> @0x01064 `54 01 01 00 00 00`; @0x0108A Ifel / @0x0108E
  Ck(3,89)=1 `21 03 59 01` -> @0x01092 `54 00 01 00 00 00`. Dasselbe in sub07 (Fuehrerstand): @0x01118 Ck(3,89)=0 -> @0x0111C
  `54 01 01 ...`, @0x01128 Ck(3,89)=1 -> @0x0112C `54 00 01 ...`.
- Sce_bgm_control (scd_vm.c op_sce_bgm_control, Handler FUN_80044da4): Byte 1 = Sequenz-Slot (0 MAIN, 1 SUB), Byte 2 = 1 =
  SsSeqSetVol + SsSeqPlay. Ab (3,89)=1 spielt beim Eintritt die MAIN- statt der SUB-Sequenz.
- (3,89) setzt ROOM6000 sub07 @0x011B4 `22 03 59 01` (Tor).
- Der Port setzt keine Musik: keines der drei Port-Programme (5090-Szene, Zugstart, Tuer) enthaelt Opcode 0x54. Riegel in
  `unit_r41_annette` (Opcode-Lauf ueber alle drei, dazu die Bytes @0x0108E/@0x01092/@0x01128/@0x0112C gegen die RDT).
- Damit nach Entscheid 3 unveraendert.

### N2.4 Blut bei Annettes Tod = der Emitter der RE2-Szene (Entscheid 4)

**Wo RE2 das Blut erzeugt** (Claire PAL SLES-10972, RAM-Abzuege der Aufnahme lauf3 e19..e24; Disasm und Platz-Abzug
`AN_belege/re2_claire/blut_emitter_ram.txt`, Werkzeuge `AN_belege/werkzeug/n2/`):

1. FX-Spawn: RE2 Leon FUN_8001BF10 = Claire PAL **FUN_8001B910** (maskierter Codevergleich `exe_map.py find 8001bf10 24`, Kopf
   `240a0060 addiu t2,zero,0x60` / `00047602 srl t6,a0,0x18`). Pool **0x800D07D0**, 0x7C je Platz (@0x8001B99C-A4), +0x18 = 0xA003
   belegt, +0x1C = (sub<<16)|Effekt, +0x38 = a0<<16 (Skala), +0x2C/+0x30 = *a3, +0x6C = Anker a2; Pool voll -> `j 0x8001bc1c` /
   `addiu v0,zero,0xff` @0x8001B994-98 (Aufruf verloren).
2. RAM e22 (71,54 s, 0,45 s nach dem ersten Blut): **60 Plaetze Effekt 0 / sub 1 / Skala 0x2000**, Anker 0x80095F68 =
   Einheitsmatrix (T = 0), also Lage = a3; 10 Gruppen zu 6 = 2 Lagen x 3 Zeilen; +0x20 je Gruppe 1280 .. 3328 in 256er-Schritten.
   e23 (6 Spielbilder spaeter, Birkin M07/45 -> /51): nur noch die 4 juengsten Gruppen; e24: keine. Je Gruppe ~10 Spielbilder
   Leben, Wolke ~15 + 10 = 25 Spielbilder = 1,0 s PAL (Aufnahme: 71,09 s bis ueber den Schnitt 72,23 s).
3. Wer ruft: 100 `jal 0x8001b910` im Overlay (`jal.py`). Die beiden Lagen einer Gruppe liegen 393 auseinander = Abstand der
   Birkin-Teile 14 und 16 (RAM e21: 393, e22: 393; Teile 14/15/16 in einer Reihe je ~195, je ~600 von Teil 13 = die drei Klauen
   der Hand 13). Emitter **FUN_801058BC**(e, Teil, Index): Beschreiber 0x801065B8 + 6 * Index, **Index 1 = `00 01 00 00 00 20`** =
   Effekt 0, sub 1, Skala 0x2000; Bit 0x80 = 0 -> Anker 0x80095F68, a3 = Weltlage des Teils (`lw` +0x5C/+0x60/+0x64
   @0x80105950-78); a1 = FUN_80014E4C(e.x, e.z, Spieler.x, Spieler.z) @0x801058F0-908 (= Leon FUN_800154AC, Peilung zum
   Spieler). Index-1-Aufrufer mit den Teilen 14/16: **FUN_80104C24** @0x80104D38/@0x80104D4C und @0x801031E8/F8 (Angriff auf den
   Spieler: Zaehler 3, dazu Blut am Spieler Teil 8 mit Index 0).
4. FUN_80104C24 ist Routine 2 der Birkin-Tabelle @0x80106584 (`80104a18 80104a7c 80104c24 80104d88`); Routine 1 (FUN_80104A7C)
   wechselt nach 2, wenn +0x1D4 (Member 0x17) = 2 (@0x80104BE4 `lh v0,0x1d4(s0)` / `bne v0,s3` / `sb v0,5(s0)`) = RE2 ROOM6141
   @0x01142 Member_set 0x17=2. RAM: e20/e21 +5=2 +7=0, e22/e23 +7=1 und +0x16B=0 (15 Bilder vorbei), e24 Routine 3.

FUN_80104C24, nach dem Bildvorschub (@0x80104C9C-A8 `jal 0x800289e8`):

```
80104cd4: 8e020178  lw   v0,0x178(s0)        ; Bildwort
80104ce0: 3c030001  lui  v1,1
80104ce4: 00431024  and  v0,v0,v1            ; EDD-Trefferbit 0x10000
80104cf0: 92020007  lbu  v0,7(s0)            ; nur einmal
80104d08: 0c015da4  jal  0x80057690          ; a0 = 3
80104d0c: a2020007  sb   v0,7(s0)
80104d10: 2402000f  addiu v0,zero,0xf        ; 15 Bilder
80104d14: a202016b  sb   v0,0x16b(s0)
80104d18: 3c118010  lui  s1,0x8010
80104d1c: 86316330  lh   s1,0x6330(s1)       ; = 13
80104d2c: 26250001  addiu a1,s1,1            ; Teil 14
80104d38: 0c04162f  jal  0x801058bc          ; a2 = 1
80104d44: 26250003  addiu a1,s1,3            ; Teil 16
80104d4c: 0c04162f  jal  0x801058bc          ; a2 = 1
80104d5c: 2442ffff  addiu v0,v0,-1
80104d60: a202016b  sb   v0,0x16b(s0)
```

**Port** (`annette_r41.c` blut_bild, Konstanten AN_BLUT_* mit diesen Adressen):
- Trefferbild (Clip 4 Bild 14, EDD-Bit 16 wie bisher) setzt den Zaehler auf 15; je Bild, nach dem Bildvorschub, zwei
  `re15_esp_fx_spawn_rows(Raumbank, 0, 1, 0x2000, ...)` an der Weltlage der Knochen **14 und 28** der Riesenform, Boden = Aktor-y,
  Winkel = Peilung Form -> Spieler; dann Zaehler - 1. Laeuft ueber den Schnitt weg (Routine der Form, nicht des Skripts).
- RE2-Effekt 0 -> RE1.5-Blut 0 ist der vorhandene Dekoder der RE2-Gehirne (`enemy_ai_re2_zombie.c` re2z_gore_fx_core; Runde 4
  gore-vollausbau.md: "0 -> 0 Blut; die Subs 0-3 tragen dieselben Klassen").
- PORT-WAHL der Teile: die Riesenform (BAHNSTEIG_FORM.EMD) ist kein RE2-Modell. Ihre linke Hand 13 (Hieb links) traegt die Klauen
  14 (729,30,-228), 15 (782,30,83), 28 (684,30,258) und den Daumen 30; RE2 nimmt die aeusseren Klauen -> 14 und 28.
- Entfallen: die 8 Spritzer aus NB1 (4 im Trefferbild an Annettes Brust und an der Hand, je 2 nach 6 und 12 Bildern); Lage und
  Zeitpunkt hatten keinen Beleg. Die Blut-Textur des Kittels (RE2 EM049) bleibt.

**Gemessen** (`unit_r41_annette` mit Raumbank ROOM5090 + CORE00.ESP, Takt wie im Spiel):
- 30 Aufrufe in F400..F414, Treffer F400 (RE2: 15 Bilder x 2).
- RE1.5-Blut 0/1 hat 4 Zeilen je Aufruf (RE2: 3); 99 von 120 Plaetzen belegt: der 96er-Pool ist voll, 25 Plaetze halten die
  Bodenfeuer von 5090 schon vorher; der Rest geht wie im Original verloren.
- hoechstens 79 Blutplaetze gleichzeitig (RE2 e22: 60), Wolke weg 27 Bilder nach dem ersten Spawn (RE2 ~25 Spielbilder).
- im ersten Bild 8/8 Plaetze genau an den Klauen.
- echter Lauf (`build/an_lauf/leon_nb2/debug.log`): `F681 Treffer`, `F681 Blut 1/15: 2 Aufrufe, 8 Plaetze, Pool 25/96`,
  `F695 Blut 15/15: 30 Aufrufe, 99 Plaetze, Pool 96/96`.

**Sicht**: `spiel_n2/vergleich_re2_port_blut.jpg` (RE2 | Port, Zeilen im gleichen Abstand zum ersten Blut, -0,18 .. +1,2 s),
`spiel_n2/leon_5090_blut_n2.mp4` (Port 10 Bilder/s, Bild 236..275). Port: rote Wolke ab Bild 254 vor Annettes Beinen (die Klaue
steht zwischen Kamera und Annette), 0,9 s sichtbar, deutlich groesser als die 8 Spritzer aus NB1 (`spiel_n1/vergleich_re2_port_5090_2.jpg`
Zeile 3).

Unterschiede, die bleiben (offen, N2.8):
- RE2 zeigt die Wolke viel groesser im Bild: RE2 Schnitt 0x0A steht dicht vor der Klaue, Port-Schnitt 11 (PORT-WAHL N1.2, Auge (7626,-3256,-25360)) rund 3800
  von der Klaue (4742,-1123,-24049) entfernt. Zahl, Skala, Dauer und Lage sind die von RE2; eine groessere Bildwirkung ginge
  nur ueber die Kamera.
- RE2s Plaetze wachsen an Ort und Stelle (+0x20 +256 je Bild bis ~0xD00), die RE1.5-Zeilen 0/1 fallen und zerspritzen am Boden.
  Die RE2-FX-Zeilen selbst sind im Port nicht portiert (enemy_ai_re2_dog.c:51).

### N2.5 Sherry im 6030-Wagen (Entscheid 5): nichts geaendert

Vermerk: Sherry sitzt im Wagen von 6030 (Original sub06 @0x01044 Sce_em_set Typ 0x4B, unbedingt; von ST80-T genutzt). Nach
Entscheid gilt: Annette geht nach msg 30 nach hinten zu Sherry und weiter zum Krach. Der Port zeigt diesen Weg nicht; sie
verschwindet durch die Wagentuer (6000 sub02 @0x0100A-Bytes) und steht beim naechsten 5090-Eintritt in Wagen C. N1.4 Punkt 3 ist
damit entschieden, ohne neuen Text.

### N2.6 Tests

Bau `bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK.

| Test | Inhalt (neu in NB2) | Ergebnis |
|---|---|---|
| `unit_r41_annette` | Tuer-Programm = @0x00C72 + @0x0106A + @0x010CA; Wagen vor dem Zugstart Slot 2 Tuer (5/0) + Slot 3 msg 10, danach Slot 2 msg 20 + Slot 3 Tuer; kein 0x54 in den Port-Programmen, Musik-Bytes @0x0108E/@0x01092/@0x01128/@0x0112C; Blut mit Raumbank 5090 + CORE00.ESP: 30 Aufrufe F400..F414 ab dem Trefferbild, 4 Zeilen je Aufruf, 99 Plaetze (Pool 96/96), Wolke max 79 (>= 60 wie RE2 e22), weg 27 Bilder nach dem ersten Spawn, 8/8 Plaetze an den Klauen | gruen |
| `unit_r36_kartenbits` | Bit 133 im Original frei, 61 Port-Bits, 0 Kollisionen | gruen |
| `unit_r41_annette_se_bank/_toene/_lagen` | unveraendert | gruen |
| `integration_r41_annette` (echte exe, 4 Laeufe) | unveraendert | gruen (212 s) |
| `integration_r41_birkin_pistole` | Bestand | gruen (259 s) |
| `integration_r35_zgirl_5090` | Bestand | gruen (147 s) |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|android\|r37_masken36_ausschluss\|paket\|titelversion`, `-j 4` | Bestand | **1039/1039 gruen** (283 s) |

Ausgeschlossen (26 Tests, Umgebungsproblem wie in N1.9 und von ST80-T gemeldet: die Skript-Riegel haengen in `df -Pk`, der
Titelversion-Ablauf startet das echte `release_alles.sh`): probe_r16_objekte_paket, unit_r30_android_* (3), unit_r35_android_* (5),
unit_r37_masken36_ausschluss, unit_r38_releaseskript_* (7), unit_r39_titelversion_* (9). Nicht von dieser Spur beruehrt.
Fensterlaeufe nur mit eigener exe-Kopie, beendet per eigener PID (an_lauf.sh); danach kein `re15_pc_an*` mehr aktiv.

### N2.7 Dateien und Commits

- Code: `re15_port/engine/src/annette_r41.c` (blut_bild, AN_BLUT_*, k_tuer), `re15_port/include/re15_annette_r41.h` (Kopf,
  re15_annette_r41_blut_plaetze), `re15_port/tests/unit/test_r41_annette.c` (Tuer vor/nach, Musik-Riegel, Blut-Kette),
  `re15_port/tests/unit/test_r36_kartenbits.c` (Bit 133).
- Doku: `analysis/befunde_runde41/VERTRAG.md` 1.1, dieses Dossier.
- Belege: `AN_belege/spiel_n2/` (vergleich_re2_port_blut.jpg, leon_5090_blut_n2.mp4, tuer_6030_ausgang_vor_nach.jpg,
  tuer_6030_vor.mp4, tuer_6030_nach.mp4), `AN_belege/re2_claire/blut_emitter_ram.txt`, `AN_belege/werkzeug/n2/`.
- Commits: e2c85f4ab (Code, Tests, VERTRAG), Doku-Commit mit diesem Abschnitt und den Belegen.

### N2.8 Offen

- Stimmen (Nutzer): `synchro/STAGE5/room5090/main40..43.wav`, `synchro/STAGE6/room6030/main27..30.wav` (unveraendert aus N1.10).
- Blut: Bildwirkung kleiner als RE2 wegen der Kamera (N2.4); RE2-FX-Zeilen (Wachsen statt Fallen) nicht portiert.
- RE2 spielt im Trefferbild zusaetzlich FUN_80057690(3, e) (@0x80104D08); der Port spielt die Bildtoene der Form wie bisher
  (vor(), Bit 27 des Bildworts). Ob 0x80057690 ein Ton ist, ist nicht belegt.
- F4 (Annette hielt William fuer tot), F7 (spaeter), ZUG_AN-Alternative 7030, NTSC-Bildzahlen, Haut der Form beim Kampfstart:
  wie N1.10.
- `alle[64]` in test_r36_kartenbits.c teil_p: 61/64 belegt (N2.1).

## N3. Nachbesserung 3 (2026-11-03)

Auftrag: Abnahme 1 (`AN_abnahme_1.md`, NICHT BESTANDEN) - M1, M2, M4 bis M8 beheben, M9 nach dem Merge pruefen, M3 (Aufnahmen)
bleibt beim Nutzer. Vorher r41/form in r41/annette mergen und alles auf dem neuen Formstand messen.

### N3.0 Merge r41/form

- `git merge r41/form` (Kopf 7d79bcfae, neuer als 8c7cc6754: dazu Textur-Werkzeug E22 und START im Griff) -> Merge-Commit
  b0685df4f. Textuell ohne Konflikt; ueberlappend nur `engine/src/enemy_ai_common.c` (Annette-Haken in run_all + Koerper-Schub der
  Form) und `platform/pc/main.c` (Annette-Modell/Ton + Form-Schatten/Messspalten), beide Seiten erhalten. Die Annette-Szene laeuft
  weiter vor dem Kampfstart (Evt_exec sub04 -> Programm -> am Ende sub04).
- Tests direkt nach dem Merge: unit_r41_annette (+ _se_bank/_toene/_lagen), unit_r36_kartenbits, unit_birkin_bahnsteig,
  unit_birkin_formen, unit_birkin_szenen, unit_r41_form_emd, unit_n2_griff_start, unit_r41_birkin_wand_rein/_g5, gruen;
  integration_r41_annette (278 s), integration_r41_birkin_pistole (322 s), integration_r35_zgirl_5090 (182 s) gruen.

### N3.1 Messweg

- Alle Bilder dieses Abschnitts aus dem ECHTEN Fenster (eigene exe-Kopie `re15_pc_n3_*`, Renderer des Nutzers, kein AUTOSHOT, kein
  SOFTWARE_RENDER), Bild fuer Bild per `RE15_FRAMEDUMP` (komponiertes Bild direkt vor dem Present, kein Bildausfall):
  `AN_belege/werkzeug/n3/n3_lauf.sh`. Bildleisten `blatt.py`, RE2/Port-Blatt `vergleich.py` (RE2-Bilder aus
  `re2_claire/re2c_szene_320.mp4`, Clipzeit = Videozeit - 1,00 s).
- Bildanteil: `szmass.py` baut die Port-Zeichenkette fuer Form (BAHNSTEIG_FORM.EMD/TIM) und Annette (ANNETTE_4D.EMD) nach
  (form_buehne-Kette, RID 5090 @0x60 + 32 * Cut), zeichnet einen ID-Puffer und zaehlt im Band zwischen den Kinobalken (render_pc.c
  FUN_80021a0c: Balken (0,0)-(320,24) und (0,216)-(320,240)). `satzmass.py` nimmt Lage/Gier/Clip/Bild je Spielbild aus dem state.log
  des Fensterlaufs. Gegenprobe: alter Stand (L (500,-22600)) Cut 15 Form 71-97 %, Annette 0-32 % frei = Abnahme M1 ("70-80 %");
  NB3-Erstlauf F600 Cut 15 offline 74 % = Fensterbild. Fehler auf dem Weg (behoben): die Gier rechnete zuerst mit atan2(dz, dx);
  `re15_atan2_q12(dz, dx)` ist atan2(dx, dz) (gemessen: L -> S Gier 3957 im Spiel, 3959 im Werkzeug).

### N3.2 M1 (schwer): Satz und Landung, Form verdeckt Annette

Befund gemessen (`suche3.py`, `suche3_3760.json`, `suche3_5157.json`): Cut 15 (Auge (46,-1236,-24302)) sieht vom Wagen nur x 0..6000;
mit 3760 Abstand (Trefferfenster Clip 4) oder 3760 + 1397 (RE2-Vorstoss) gibt es KEINE Lage im Gang, bei der Annette >= 95 % frei
ist und die Form <= 40 % des Bildes deckt (beide Listen leer). Der erste NB3-Lauf mit einer anderen Lage zeigte es im Fenster:
Cut 15 beim Satz 74 % Form, Annette nur unten rechts. RE2 0x0B (hoch von Williams Seite) hat in 5090 keine Entsprechung.

Behebung (PORT-WAHL, `annette_r41.c` Lagen-Kopf und k_prog):
- Satz in Cut 11 (von Annettes Seite, wie Abnahme-Zeile "Bruellen, Anstarren: gut" und wie Foto #145), Lagen per `suche4.py`:
  S (3700,-22500), L (-1378,-23395), Abstand 5157 = 3760 + 1397. W1 (5979,-23427), W2 (4732,-22920) wie bisher 47 %/76 %.
- Sturz/Landung ebenfalls in Cut 11 (Schnitt mit dem Sturz-Signal, RE2 @0x010CA): die Form faellt vor der Tuer ganz ins Bild.
- Schnittfolge jetzt 12 / 15 / 11 / 13 (RE2 7 / 2 / 0x0A+0x0B+0x0A / 6).
- Vorstoss beim Hieb: RE2 Birkin x -6600 (e20) -> -7997 (e22) = 1397 (AN_VORSTOSS); die Gier des Hiebs wird an der Lage nach dem
  Vorstoss gewaehlt.

Messwerte (Fensterlauf `n3_leon`, Lagen aus dem state.log, `satzmass.py`):

| Bild | Schnitt | Form im Bild | Annette frei | Annette ganz zwischen den Balken |
|---|---|---|---|---|
| F479 gelandet | 11 | 9,9 % | 100 % | ja (y 177..402 von 48..432) |
| F520 Bruellen | 11 | 8,7 % | 100 % | ja |
| F596 Satz | 11 | 10,1 % | 100 % | ja (y 187..424) |
| F650 Satz | 11 | 9,7 % | 100 % | ja |
| F700 Satzende | 11 | 9,8 % | 100 % | ja |
| vorher (Abnahme w8) Satz | 15 | 70-80 % (Werkzeug 71-97 %) | 0-32 % | nein |

Bilder: `AN_belege/n3/szene5090_n3.jpg` (20 Felder), `vergleich_re2_port_n3.jpg` (13 Zeilen RE2 | Port), `szene5090_n3.mp4`
(30 Bilder/s, F270..F1250).

### N3.3 M2 (mittel): Abgang in 6030

Befund: das Auge von Cut 1 (RID 6030 Cut 1 (-5886,-3132,-23058)) steht in der Tuer selbst (Tuerrechteck main00 @0x00C32 x
-5100..-4200); wer zur Tuer geht, laeuft in die Kamera, am Ziel (-3873,-22550) blieben Kopf und Rumpf im Bild (Projektion x 94..105,
Fuesse unter dem Rand) - daher "gross im Vordergrund, dann weg".

Behebung (PORT-WAHL, Original-Bytes): vor dem Gehen Cut 0 (sub08 @0x01336 `29 00`, Eintrittskamera, Tuer rechts im Bild), sie geht
zur Tuer; im Ankunftsbild Schnitt auf Cut 1 (sub08 @0x0114A `29 01`) und im selben VM-Schritt versteckt (6000 @0x0100A).
Gemessen (Fensterlauf `n3_zug`): Cut 0 F757..F786, sie steht in F786 an der Tuer neben Leon, F787 Cut 1 ohne sie. Es gibt kein Bild,
in dem sie innerhalb eines Schnitts verschwindet. Bilder `AN_belege/n3/zugstart6030_n3.jpg`, `zugstart6030_n3.mp4`.

### N3.4 M4 (mittel/leicht): Reaktion auf den Treffer

Im Trefferbild sackt Annette mit 0x4D-Clip 6 sofort zusammen (Bild 0..12, ab Bild 12 am Boden, gehalten); bis dahin Clip 4. Gemessen:
Treffer F752 Clip 6/0, F764 am Boden; vorher (Abnahme) 1,0 s aufrecht mit erhobenen Armen. RE2 zeigt sie in diesem Moment nicht.
Beim Schnitt weg (F785) wird sie wie bisher an die Lage aus @0x01162 gelegt.

### N3.5 M5 (leicht): Blut sichtbar

Kette unveraendert RE2 (FUN_80104C24: 15 Bilder x 2 Klauen, Effekt 0/sub 1). Neu ist nur die Skala: gleiche Bildgroesse wie RE2.
RE2 Schnitt 0x0A: ROOM6141 RID @0x64 + 0x0A * 32 = fov 32946 (H 257), Auge (-10656,-684,-16056); erster Blutplatz RAM e22 Slot 6
(-8064,-1865,-16471) = 2878 vom Auge. Port: Skala = 0x2000 * (257 / H) * (Abstand / 2878) mit H und Auge des angezeigten Schnitts
im Trefferbild (`blut_skala`). Fensterlauf: Cut 11 H 208, Klaue 14 4492 vom Auge -> Skala 0x3DB6.

Blut-Flaeche (Anteil rein roter Pixel im Band, abzueglich des Bildes vor dem Treffer, `blutflaeche.py`):

- RE2 (Aufnahme ab 71,09 s): 3,7 % / +0,21 s 5,5 % / +0,34 s 12,0 % / +0,51 s 16,4 % / bis +1,04 s 16,4 % (bis zum Schnitt).
  **Berichtigt in Nachbesserung 4 (Abnahme 2 H3):** die 16,4 % ab +0,40 s sind kein stehendes Blut, sondern ein
  Ladestandbild. Ab 71,49 s steht das RE2-Bild bis zum Schnitt still (mittlere Pixeldifferenz zum Vorbild 0,00 bis 0,04 von
  71,49 bis 72,13 s, Abnahme 2 Abschnitt 2.4). Bewegt ist das RE2-Blut nur 0,40 s (71,09 bis 71,49 s).
- Port NB3 (ab Treffer F752): 2,1 % / +0,13 s 14,6 % / +0,27 s 12,1 % / +0,40 s 7,5 % / +0,53 s 9,3 % / +0,67 s 8,4 % /
  +0,80 s 1,3 % / +1,07 s 1,1 %.
- Vorher (Abnahme M5): "kleiner dunkelroter Fleck am Boden, ~0,9 s".

Rest (berichtigt NB4: "stehen bis zum Schnitt" = RE2-Ladestandbild ab 71,49 s, bewegt 0,40 s): RE2-Zeilen wachsen am Ort, die RE1.5-Zeilen 0/1 fallen und zerspritzen (N2.4) - im Port ist die
Wolke so gross wie in RE2, aber nach 0,7 s weg statt 1,1 s. Pool weiter 96/96 waehrend der 15 Bilder.

### N3.6 M6 (leicht): Untertiteldauer

Der Port zeigt eine Zeile genau ihren `01 nn`-Halt lang (msg_common.c re15_msg_compute_duration, FUN_80028134 Zustand 6). Halt jetzt
je Zeile (`tools/r41_annette/texte_bauen.py`): 43 "William." 35 Bilder (Xa 0x30 11 Sektoren = 1,173 s), 40 "W-William! You're
alive!" 118 (Xa 0x31 37 Sektoren = 3,947 s), 42 "Leon: Annette!" 60 (= Sleep bis zum Kampfstart); 6030 bleibt 99.
Gemessen (state.log, Spalte msg): 43 F376..F411 (Schnitt F442), 40 F592..F710 (endet mit dem Hieb-Signal), 42 F828..F888
(Kampfstart F888/F889). Vorher 99 Bilder ueber den Schnitt bzw. ueber den Kampfstart.

### N3.7 M7 (Info): Tempo und Ausholen

- Die RE2-Sleeps zaehlen PAL-Spielbilder (25/s, VSync(2) @0x8002ade4-ec); jetzt PAL(n) = n * 6 / 5 Port-Bilder (30/s): 25 -> 30,
  36 -> 43, 30 -> 36, 60 -> 72, 1 -> 1. Die Xa-Laengen waren schon Sekunden.
- Hieb nach der RE2-Aufnahme (re2c_12b_hieb_dicht.jpg): Signal/Schnitt 69,77 s, Birkin steht noch 70,05 s, Ausholen ab 70,22 s,
  Blut 71,09 s, Schnitt 6 72,23 s -> Port: 13 Bilder Vorlauf (Form steht), Clip 4 Bild 0..14 mit 2 Takten je Bild (28 Bilder), dabei
  der Vorstoss 1397, Treffer, 34 Bilder bis zum Schnitt (zusammen 75 = Sleep im Programm).

| Schritt (s ab Krach) | RE2 | Port vorher (Abnahme w8) | Port NB3 (n3_leon) |
|---|---|---|---|
| "William." | 3,53 | 2,80 | 3,20 |
| Schnitt Gang | 8,07 | 4,83 | 5,40 |
| Sturz | 9,30 | 5,43 | 6,07 |
| Landung | 10,21 | 5,93 | 6,57 |
| Bruellen | 11,77 | 6,90 | 7,97 |
| Satz | 14,80 | 8,93 | 10,40 |
| Hieb-Signal | 20,37 | 12,87 | 14,33 |
| Ausholen sichtbar | 20,82 | (keins) | 14,77 |
| Treffer/Blut | 21,69 | 13,37 | 15,73 |
| Schnitt weg | 22,83 | 14,40 | 16,83 |
| Steuerung / Kampfstart | 24,39 | 17,8 | 20,27 |

Hieb-Signal -> Treffer: RE2 1,32 s, Port 1,40 s (vorher 0,5 s); Treffer -> Schnitt weg: RE2 1,14 s, Port 1,10 s. Rest der
Gesamtdifferenz (4,1 s): Annettes Weg ist im Port 4640 lang, in RE2 17000 (drei Plc_dest-Strecken), und RE2 laedt an jedem Schnitt
von der CD (0,3-0,7 s Standbild je Schnitt, zeitachse.md) - beides nicht nachgebaut.

### N3.8 M8 (Info): Leiche beim Wiedereintritt

RE2 legt Annette bei spaeteren Besuchen liegend hin (Claire ROOM6141 MAIN sub00 else-Zweig @0x00D16, gleiche Lage wie Pos_set
@0x01162). Kein neues Bit: (3,42) schreiben nur ROOM5090 sub01 @0x0129A und ROOM5091 @0x0127E (Byte-Suche `22 03 2a` ueber alle
RDTs); in 5090 heisst (3,89) und (9,133) und (3,42) also "Szene gelaufen". Port: die 0x4D aus sub00 @0x0122A wird dann nicht mehr
verworfen, der erste Takt legt sie (Phase RE15_AN_RUHE) an die Lage aus @0x01162 (Stand S), Clip 6 Bild 12 gekippt, hp -1,
ANNETTE_4D_BLUT.EMD. Auch beim Neuaufbau desselben Raums (Tuer Wagen B -> C ist ein Wiedereintritt in 5090) wird sie neu gelegt.
Liegen-Zerlegung korrigiert: RE2-Versatz in Williams Blickrichtung = 1215 weg, +245 quer (NB1/NB2 hatten die Achsenwerte 1203/-299,
Seite gespiegelt) -> Leiche (4856,-22048), 348 vor dem Regal SCA #28.
Fensterlauf `n3_leiche` ((3,89),(9,133),(3,42) per Flag, Weg aus Wagen B): Leiche in Cut 12 hinten rechts, in Cut 11 vorn rechts;
kein Formauftritt, keine Szene. Bild `AN_belege/n3/leiche_wiedereintritt_n3.jpg`.

### N3.9 M9 (Form-Spur): Kampfstart

Nach dem Merge tritt die Form beim Kampfstart in Cut 11 auf (form/STUFE3.md N4a). Fensterlauf: "Leon: Annette!" bis F888 in Cut 13,
F889/F890 noch Cut 13, ab F891 Cut 11 mit der ganzen Form, F949 Cut 12. Ein Bild "nur Haut" in Cut 15 gibt es nicht mehr
(`AN_belege/n3/kampfstart_n3.jpg`). Leon steht beim Satz nicht mehr im Bild (Cut 11 schaut von ihm weg).

### N3.10 M3

Unveraendert: `synchro/STAGE5/room5090/main40..43.wav` und `synchro/STAGE6/room6030/main27..30.wav` fehlen (Sache des Nutzers).
Die Untertitel-Halte (N3.6) sind die Laengen, die die Aufnahmen haben sollten.

### N3.11 Tests

Bau `bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK.

| Test | Inhalt (neu in NB3) | Ergebnis |
|---|---|---|
| `unit_r41_annette` | Schnittfolge 15/11/13 im Programm, 12/15/11/13 im Takt; Sleeps 30/43/1/35/36/72/1/118/75/1/42/60; Annette steht bei S (3700,-22500); Hieb-Signal -> Treffer 42 Bilder, Treffer -> Schnitt weg 33..36; Annette Clip 6 im Trefferbild; Untertitel-Halt 35/118/60; Leiche 1215 von L / 245 quer; Wiedereintritt (3,42)=1: Leiche (4856,-22048) Clip 6/12, hp -1, Phase RUHE, Blut-Kittel; 6030: vor dem Verstecken Cut 0, beim Verstecken Cut 1 | gruen |
| `unit_r41_annette_se_bank/_toene/_lagen` | unveraendert | gruen |
| `integration_r41_annette` (5 Laeufe, echte exe) | Schritte jetzt mit "Ausholen (RE2" und "Blut-Skala"; neuer Lauf `leon_leiche` ((3,89),(9,133),(3,42)): Leiche gelegt, Blut-Kittel, keine zweite Szene | gruen (254 s) |
| `integration_r41_birkin_pistole`, `integration_r35_zgirl_5090` | Bestand | gruen (248 s / 147 s) |
| Form-/Birkin-Tests nach dem Merge: unit_birkin_bahnsteig, _szenen, _formen, _opfer, _ai, unit_r41_form_emd, unit_n2_griff_start, unit_r41_birkin_wand_rein/_g5/_griff, unit_r36_kartenbits | Bestand | gruen |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|android\|r37_masken36_ausschluss\|paket\|titelversion`, `-j 4` | Bestand | **1040/1040 gruen** (295 s) |

Ausgeschlossen wie in N2.6 (Umgebungsproblem df.exe / echtes release_alles.sh). Fensterlaeufe nur mit eigener exe-Kopie
`re15_pc_n3_*` (per `timeout` auf die eigene PID begrenzt), danach geloescht.

### N3.12 Offen

- RE2 0x0B (hoch von Williams Seite) bleibt ohne 5090-Gegenstueck; der Satz laeuft von Annettes Seite (N3.2). Wer das Gesicht im
  Satz will, braucht eine neue Kamera (kein RID-Schnitt der Disc) - nicht gebaut.
- Blut: RE2-Wolke ist 0,40 s bewegt zu sehen, danach Ladestandbild bis zum Schnitt (berichtigt NB4, N3.5); die RE1.5-Zeilen
  sind nach 0,7 s weg; RE2-FX-Zeilen nicht portiert.
- Tempo: 20,3 s gegen RE2 24,4 s; Rest = kuerzerer Weg und keine CD-Ladepausen (N3.7).
- Die Form steht nach der Landung bei L (-1378,-23395) zwischen den Bodenfeuern am Wagenende (naechstes 500 entfernt).
- Stimmen (M3), F4, F7, ZUG_AN-Alternative, NTSC-Bildzahlen: wie N2.8.

### N3.13 Dateien und Commits

- Merge b0685df4f (r41/form 7d79bcfae).
- Code: `re15_port/engine/src/annette_r41.c`, `re15_port/include/re15_annette_r41.h`, `re15_port/tools/r41_annette/texte_bauen.py`,
  `re15_port/engine/src/gen/annette_r41_texte.inc`, `re15_port/tests/unit/test_r41_annette.c`,
  `re15_port/tests/integration/test_r41_annette.cmake`, `re15_port/tests/unit/probes/r41_annette.cmake`.
- Belege `AN_belege/n3/`: `szene5090_n3.jpg`/`.mp4`, `vergleich_re2_port_n3.jpg`, `zugstart6030_n3.jpg`/`.mp4`,
  `leiche_wiedereintritt_n3.jpg`, `kampfstart_n3.jpg`. Werkzeug `AN_belege/werkzeug/n3/` (n3_lauf.sh, szmass.py, satzmass.py,
  suche1-4.py mit Ergebnissen suche3_3760.json / suche3_5157.json (leer) / suche4.json, ansicht.py, blatt.py, vergleich.py,
  blutflaeche.py). Rohdaten `re15_port/build/an_lauf/n3_*` (nicht versioniert).
- Commits: b0685df4f (Merge), afadac63f (Code, Tests), Doku-Commit mit diesem Abschnitt und den Belegen.

## N4. Nachbesserung 4 (2026-11-03)

Auftrag: Abnahme 2 (`AN_abnahme_2.md`, NICHT BESTANDEN), Punkte N2 bis N5 und die Dossier-Korrektur H3 (N3.5). N1 (Riesenform mit
HP 0 im Wagenweg) gehoert der Form-Spur und ist hier nicht angefasst.

### N4.1 Messweg

Alle Fensterbilder stammen aus dem echten Fenster: eigene exe-Kopie `re15_pc_n4_*`, Renderer des Nutzers, kein AUTOSHOT, kein
SOFTWARE_RENDER. Gezogen per `RE15_FRAMEDUMP`, Ton per `RE15_AUDIO_CAP_SYNC`, beendet ueber `timeout` auf die eigene PID. Werkzeug:
`AN_belege/werkzeug/n4/` (`n4_lauf.sh`, Eingaben `eingabe_*.txt`). Rohdaten liegen unter `re15_port/build/an_lauf/n4/` und sind
nicht versioniert.

| Lauf | Weg | Zweck |
|---|---|---|
| `kampf` | Debug-Sprung in Wagen B mit (3,89),(9,133), Tuer nach C, Szene, dann Zielen und Feuer gehalten (Pistole), F250 bis F3300 jedes 5. Bild | N2 (Leiche im Kampfbild), N3 (Schuesse an der Leiche vorbei) |
| `leiche` | wie `kampf` mit (3,89),(9,133),(3,42) (Wiedereintritt) | N2 (Leiche beim Wiedereintritt) |
| `z6030` | Kette ab der 6000-Zugtuer wie Abnahme 2 `z6030` (Eingabe w8), F1340 bis F1700 jedes Bild, Ton bildgenau | N4 |

### N4.2 N2 (mittel): Leiche liegt flach wie RE2

**RE2-Beleg.** Claire PL1/RDT/ROOM6141.RDT, MAIN sub02. Die drei Befehle laufen, waehrend die Kamera weg ist (Cut 6):

- Pos_set @0x01162 `32 00 f4 cf 84 03 3c c4` = (-12300, **900**, -15300)
- Dir_set @0x0116A `33 00 00 00 e4 0c` = 3300
- Plc_motion @0x01172 `3f 00 18 04` = Clip **0x18**

Beim spaeteren Besuch setzt MAIN sub00 @0x00D16 `44 00 00 49 00 09 00 00 00 ff f4 cf 84 03 3c c4 e4 0c 18 01 40 00` dieselbe
Lage, dieselbe Richtung und Bewegung 0x18.

Clip 0x18 steht nicht im Annette-EMD: EM049 hat in Paar 1 nur 8 Clips, in Paar 2 17 und in Paar 3 keinen. Er steht im
Raum-Animationsblock. ROOM6141 RDT+0x60 zeigt auf @0x2E70 `b8 79 00 00 01 00 00 00`. Record 0 traegt Marker 5, 15 Knochen,
80 Byte je Keyframe und 26 Clips; Clip 24 = 0x18 hat 40 Bilder (Keyframes 239 bis 278).

RAM-Beleg aus der eigenen Aufnahme (Savestates lauf3 e23, e24, e25, e30), gelesen mit `werkzeug/n4/liege.py` und `lage.py`:

| Feld | Wert |
|---|---|
| Annette +0x14C | 0x18 |
| Bildzaehler +0x14D | 6, 26, 6, 22 (Schleife ueber 40 Bilder) |
| +0x156 (hp) | -1 |
| Teile (+0x198, je 0xAC, Lage +0x5C/+0x60/+0x64) | y -433 bis -24, sie liegt also flach am Boden |

Aus dem RBJ nachgerechnet liegen Teil 0, 4 und 8 bei (-12300,-154,-15300), (-11682,-108,-13977) und (-12611,-158,-15842).
Der RAM-Stand e23 sagt (-12300,-171,-15300), (-11680,-73,-13980) und (-12611,-188,-15840); der Rest erklaert sich aus Bild 6
gegen Bild 0.

**Bau:**

- **Modell:** Paar 1 von `ANNETTE_4D*.EMD` bekommt hinten Clip 24 = RE2-Clip 0x18 (`tools/r41_annette/annette_modell.py`).
  - Der Auszug liegt als `tools/r41_annette/re2_6141_liegen.bin` (40 Bildeintraege + 40 Keyframes, md5 bd1ce367...), weil
    `info/re2clair` nicht versioniert ist; `--auszug <ROOM6141.RDT> --pruef` vergleicht ihn mit der Quelle.
  - Clips 0..23, Keyframes 0..357 und Paar 2/3 bleiben bytegleich.
  - Das u32 hinter der Bildliste (EM4D @EDD+0x5F8 = Listenende) wandert mit.
  - Gegen das RE2-Gerippe weichen die Gelenke um hoechstens 19 ab (Unterarme, AN_belege/rig_4d_gegen_re2_47.txt).
- **Pose:** `annette_r41.c` AN_FA_LIEGEN 24, Bild 0 gehalten, y = AN_LIEGEN_Y 900 (@0x01162), nicht mehr gekippt
  (AN_KIPP_* entfallen).
  - PORT-WAHL: RE2 laesst den Clip als Schleife laufen, denn dort atmet die Sterbende noch und spricht spaeter mit Claire
    (sub03). Im Port ist sie tot, deshalb haelt der Port Bild 0. Das ist das Startbild, das Plc_motion setzt: RE2-Handler
    0x80056b8c (Opcode-Tabelle 0x800a74c8[0x3F] @0x800a75c4), darin `sb zero,333(v1)` = `a060014d` @0x80056ba0 (berichtigt in
    NB5, N5.1; vorher stand hier 0x80052ef0, dort steht `subu v0,v0,v1`).
- **Lage:** Die Lage ist fest, denn RE2 setzt absolute Werte. Sie wird aus dem Haltepunkt S abgeleitet: 1215 weg und 245 quer
  von L wie bisher, das ergibt (4854,-22047).
  - **Gier:** relativ zu William wie in RE2. RE2 3300 minus Peilung((-6600,-14800) -> (-12300,-15300)) ergibt rel 1309; im
    Port kommt die Peilung L -> Liegestelle dazu, zusammen 1170.
  - **Wandfreiheit (PORT-WAHL, gemessen):** An dieser Lage ragen Kopf und Schulter bis z -21043 in das Regal SCA #28 (z -21700,
    294 Netzpunkte darin). Keine Drehung am selben Ort ist frei (`leiche_lage.py` / `suche_liegen.py`). Die kleinste
    Verschiebung im 50er-Raster ohne Netzpunkt in einer soliden SCA-Zelle ist 700 nach Sueden (AN_LIEGEN_SUED). Die Leiche
    liegt damit bei **(4856,-22748), Gier 1170**, 43 vom Regal und 401 vom naechsten Bodenfeuer (4000,-24600).

**Bildanteil** (ID-Puffer `bildmass.py`, Netz der Liegepose):

| Schnitt | Vorher | Jetzt |
|---|---|---|
| Cut 11 (Kampfstart, Rueckstoss, G5) | Abnahme 2: "gross vorn rechts" | 0,00 % im Band zwischen den Kinobalken; bei offenem Bild 2,95 % als Streifen am unteren Rand (Bild-y 433..479) |
| Cut 12 | | 0,30 % (Bild-x 217..306, y 209..226), klein hinten |

**Fenster (Lauf `kampf`):**

- Ab F786 (Cut 13, Schnitt weg) liegt sie mit Clip 24 Bild 0 bei (4856,900,-22748) r1170, hp -1.
- In allen 2515 state.log-Zeilen bis F3300 bleibt das so: eine Lage, ein Bild, st 0, hp -1.
- Bilder: `n4/leiche_kampf_n4.jpg` (16 Felder Szene bis Kampfende) und `n4/leiche_vorher_nachher_n4.jpg` (Abnahme 2 gegen NB4 in
  Cut 11).
- Im offenen Cut 11 (F1300) sieht man sie am unteren Rand flach auf dem Ruecken liegen, Gesicht nach oben; vorher sass sie
  gekippt rechts im Bild.
- Wiedereintritt (Lauf `leiche`): `Wiedereintritt nach der Szene: Leiche liegt` mit derselben Lage und Pose; in Cut 12 liegt
  sie flach neben dem Weg (`n4/leiche_wiedereintritt_n4.jpg`).

### N4.3 N3 (leicht): Leiche nimmt keinen Treffer mehr

**Schadensweg (gemessen):** Abnahme 2 F2751 zeigt Annette mit hp -1 -> -6, st 3, ss1 3 (= Waffe 3) und ss2 1 (= EBEN). Das ist
die Schreibfolge des Waffen-Resolvers `re15_player_weapon_fire` (re15_damage.c): +0x5 = Waffe, +0x6 = senkrechte Trefferrichtung,
+0x4 = 2/3. Dessen Kandidatenschleife prueft nur Trefferkasten, Band und Keil. Ein NPC traegt einen Kasten (0x4D: 450/1530) und
Band LEVEL; das zeigt der RE1.5-RAM `stage_saves/mzd_stage1_npc.sav`: Typ 0x40/0x42/0x4B haben Wort 0 = 0x40000001, hp -1
(`werkzeug/n4/npc_band.py`). Die Leiche lag naeher als die G5 und war deshalb das Ziel. Die Schleifen, die die Abnahme genannt
hat (:1108, :1148, :3640, :4450), gehoeren zu anderen Wegen.

**Original:** FUN_80011f50 prueft nach der Wahl die Sichtlinie (@0x80012168-74) und danach den Typ des Gewinners:

```
80012178: lbu   v0,8(s1)
80012180: sltiu v0,v0,0x40
80012184: beq   v0,zero,0x80012540     ; v0 = 0: kein Treffer
```

Ein NPC als naechstes Ziel laesst den Schuss in RE1.5 also ins Leere gehen. Diese Pruefung fehlte im Port. RE2 verwirft
Kandidaten mit hp < 0 schon in der Schleife (Leon PSX.EXE und SLUS_007.48 gleich), fuer die Pistole in FUN_800410CC
(`lh v0,342(s2)` @0x80041280, `bltz v0,0x80041300` @0x80041288; ergaenzt in NB5, N5.1), fuer Messer und Bolzen in FUN_800470C0
(`lh v0,0x156(s0)` @0x80047148, `bltz v0,0x8004740c` @0x80047150). Die liegende RE2-Annette traegt hp -1 (RAM e24 +0x156).

**Bau:**

- `re15_damage.c`, nach der Sichtlinie: `if (g_actors[best].type >= 0x40u) return 0;` (@0x80012178-84). RE1.5 hat die
  Pruefung vollstaendig, also gilt RE1.5 fuer jedes NPC.
- In der Kandidatenschleife ist die Leiche kein Kandidat: Haken `re15_annette_r41_kein_ziel` (nach dem Treffer oder in Phase
  RUHE und hp < 0). Die Leiche ist RE2-Inhalt (RE1.5 hat keine), deshalb gilt hier die RE2-Regel (Pistole @0x80041280-88,
  Messer/Bolzen @0x80047148-50). Der Schuss geht
  dann an ihr vorbei auf die G5.

**Gemessen:**

- `unit_r41_annette` test_schuss, Lage wie Abnahme 2: Leon (14378,-23510) Gier 2049, G5 (1800,-23400).
  - (a) Leiche: Rueckgabe = G5-Slot, G5 345 -> 330, Leiche hp -1 st 0.
  - (b) dieselbe 0x4D ohne Leichen-Zustand (lebendes NPC): Rueckgabe 0, NPC und G5 unveraendert.
- Fensterlauf `kampf`: Leon steht bei (13775,-23550) mit Blick nach Westen, die Leiche seitlich 802 vor ihm (in Abnahme 2
  wurde sie seitlich 1424 getroffen).
  - 108 Schuss (Munition 144 -> 36).
  - Jeder Treffer im Kampf liegt im Feuertakt 22 Bilder: Riesenform 150 -> 0 in 10 Treffern F930..F1128, G5 600 -> -1 in 40
    Treffern F1612..F2470.
  - Die Leiche hat in allen Zeilen hp -1 und st 0.

### N4.4 N4 (leicht): Abgang in 6030 durch die Tuer

**Befund (gemessen, ID-Puffer Cut 0 `werkzeug/n4/zug6030.py`):** Am Tuerziel (-3873,-22550) steht Annette zwischen dem Auge von
Cut 0 und Leon (-3873,-23487). Leon ist dort nur zu 46 % frei, das entspricht Abnahme 2 F1490.

**Muster (Zensus aller 240 RE1.5-RDTs, Unteragent, Rohdaten im Scratchpad):**

- RE1.5 hat keinen NPC-Abgang mit Tuerton oder Tuerbewegung. Die fuenf Abgaenge (10D0 sub21, 2000 sub02/03, 4010 sub02, 6000
  sub02) sind stumm, zum Beispiel ROOM4010 sub02 @0x00D1A..@0x00D2A: gehen, warten, Cut 1, Sleep 20, Pos_set weg.
- RE2 hat genau einen: Claire ROOM4081 MAIN sub05 @0x03624..@0x03658, Annette laeuft in die Tuer.
  - Se_on Tuerbank Satz 0 (oeffnen) @0x03634 `36 00 00 01 03 00 ..`
  - Sleep 30 @0x03640
  - Satz 1 (zufallen) @0x03644
  - Pos_set weg
- Tuerbewegung: ROOM6030 hat kein Tuermodell. nOmodel ist 2; Objekt 0 sitzt bei main00 @0x00CEE (-17800,-25900), Objekt 1 bei
  sub06 @0x010A2 (-26230,-20148). Das Tuernetz gibt es nur in der Tuersequenz (DOOR2A.DO2). Weder RE1.5 noch RE2 bewegen bei
  einem NPC-Abgang eine Tuer im Raum. **Eine Tuerbewegung waere erfunden und ist nicht gebaut.**

**Bau (`annette_r41.c` k_zug_seg nach msg 30):**

1. Cut 0.
2. Leon tritt zur Seite: Work_set(1,0) + Plc_dest Modus 4, Bit 0x20 auf (-3000,-23700), Form ROOM6031 @0x00D86
   `2e 01 00 00 40 00 04 20`; dann Warten @0x01358.
3. Leon dreht sich zur Tuer (Form @0x01150).
4. Erst dann geht sie zur Tuer und wird erwartet (@0x0128A). Nacheinander, weil die Ankunftsbits Ein-Bild-Pulse sind.
5. Im Ankunftsbild: Tuer auf `36 0e 00 01 02 00 ..` (Port-Bank 0x0E = RE2-Tuerbank, Ursprung RE1.5-Modus 2 = Entity-Pool
   Index 0, Handler @0x80041624/@0x80041698-b0), Cut 1 (@0x0114A), versteckt (6000 @0x0100A).
6. Sleep 30 (@0x01364 = RE2 @0x03640), dann Tuer zu `36 0e 01 01 02 00 ..`.
7. Rest von sub08 @0x01368.

- Die Tuerbank laedt `re15_pc_annette_tuerbank` (annette_se_pc.c, Haken in main.c) aus DOOR2A.DO2, Tonteil [0..0x4028) laut
  Tabelle @0x8009A718. DOOR2A ist die Tuer aller Zugtueren 5090/6030 (gen/tuer_zuordnung.inc S320/S321).
- PORT-WAHL Z_LEON (-3000,-23700): Am Ende sind Leon und Annette in Cut 0 je zu 100 % frei. Sie steht 1444 von ihm entfernt und
  ist mindestens 1100 von ihrem Weg entfernt.

**Fenster (Lauf `z6030`, jedes Bild, Ton):**

| Bild | Was | Beleg |
|---|---|---|
| F1460 | Cut 0 | |
| F1470..F1480 | Leon tritt nach (-3080,-23674) | |
| F1482..F1497 | Leon dreht sich zur Tuer | |
| F1482..F1510 | Annette geht zur Tuer (-3820,-22540) | |
| F1511 | Cut 1, sie ist versteckt, `Se_on: bank=14 id=0` | |
| F1541 | `Se_on: bank=14 id=1` | |
| F1555 | Steuerung frei | |
| ab F1504 | Annette rechts an der Tuer, Leon frei in der Mitte | `n4/zugstart6030_abgang_n4.jpg` Felder F1504 bis F1510 |

- Ton: Der Pegel je Spielbild steigt 4 Bilder nach jedem Se_on an, bei F1516 auf 1796 (davor rund 900) und bei F1546 auf 2157
  (davor rund 1000). Beide Anstiege liegen wie die Befehle 30 Bilder auseinander.
- Video mit Ton: `n4/zugstart6030_abgang_n4_mit_ton.mp4` (F1340..F1700).
- Kein Bild zeigt ein Verschwinden innerhalb eines Schnitts. Die Steuerung kommt 21 Bilder spaeter als in Abnahme 2.

### N4.5 N5 (Hinweis): Gesicht beim Satz - mit den 5090-Kameras nicht moeglich

Die Kameras von Wagen C (RID @0x60) liegen so:

| Schnitt | Auge | Blickrichtung |
|---|---|---|
| 11 | (7626,-3256,-25360) | nach Westen |
| 12 | (18526,-2656,-24660) | nach Westen |
| 13 | (11050,-3110,-24812) | nach Osten, nur x > 11050 |
| 14 | (23870,-7450,-29130) | nach Norden, Ostende |
| 15 | (46,-1236,-24302) | nach Osten |

Beim Satz sieht Annette zur Form im Westen.

**Heute** (ID-Puffer, `n4/n5_satz_kameras.png`):

- Cut 11: Der Winkel zwischen ihrer Blickrichtung und der Richtung zum Auge betraegt 134 Grad, man sieht also ihren Ruecken.
- Cut 15: 16 Grad, das Gesicht waere zu sehen. Dort steht aber der Leib der Form vor dem Auge (70 % des Bildes, Annette 25 %
  frei), obwohl ihre Wurzel L hinter dem Auge liegt, denn der Leib reicht 4340 nach vorn.

**Suche** (`werkzeug/n4/suche5.py`, Ergebnis `suche5.json`):

- Geprueft: alle fuenf Schnitte, Annette im Raster 500/300 ueber den Gang, die Form in 24 Richtungen im Abstand 5157 (Hieb
  3760 + Vorstoss 1397), beide begehbar.
- Bedingungen: Gesicht sichtbar (Winkel hoechstens 60 Grad), Form hoechstens 40 %, Annette ganz im Band und mindestens 95 %
  frei.
- Ergebnis: 1693 Lagen geprueft, 126 Treffer.

| Schnitt | Treffer | Lage | Annette im Bild | Folge |
|---|---|---|---|---|
| 14 | 123 | Annette x 15500..20000, Form x 20657..25157 | 37..74 x 91..99 Pixel | Die Form stuende an Leons Eintrittstuer (24800,-23550) |
| 11 | 3 | Annette am Westende (-3500), Form zwischen ihr und der Kamera | 35 x 107 Pixel | Gesicht etwa 10 Pixel |

Beide Treffer drehen die Szene um: Die Form faellt dann oestlich von Annette, zwischen sie und Leon. Sturz, Weg, Hieb, Blut und
Kampfstart muessten neu gelegt werden, und das Gesicht bliebe trotzdem winzig, kleiner als heute ihr ganzer Koerper in Cut 11
(79 x 235 Pixel).

Eine Drehung Annettes in Cut 11 muesste mindestens 74 Grad von William weg gehen (fuer 60 Grad zur Kamera). Dann spraeche sie
nicht mehr zu ihm. RE2 0x0B zeigt sie William zugewandt (Plc_neck @0x01128 auf William).

**Ergebnis:** Mit den Disc-Kameras von 5090 gibt es keine Einstellung, die ihr Gesicht beim Satz zeigt und die Szene so laesst.
Nicht gebaut. Moeglich waere es nur mit einer neuen Kamera; das ist kein RID-Schnitt der Disc und nicht beauftragt.

### N4.6 Tests

Bau `bash re15_port/tools/local_build.sh build`: LOCAL-BUILD-OK.

| Test | Inhalt (neu in NB4) | Ergebnis |
|---|---|---|
| `unit_r41_annette` | Modell: 25 Clips, Clips 0..23/Keyframes/Paar 2-3 gleich EM4D, Clip 24 = Auszug; Leiche (Wiedereintritt und nach der Szene) (4856,-22748) Gier 1170 +-24, y 900, ungekippt, Clip 24 Bild 0, hp -1, kein Schuss-Kandidat; 6030: Leon zur Seite, Tuerton Bank 0x0E Satz 0 im Bild des Verschwindens, Satz 1 30 Bilder spaeter, Segmentfolge; test_schuss (a)/(b) | gruen |
| `integration_r41_annette` (5 Laeufe) | leon_tor: Annette am Ende Clip 24 Bild 0; leon_leiche: Clip 24 Bild 0 bei (4856,-22748) r1170; zug6030: Plc_dest (-3000,-23700), Se_on bank=14 id=0 vor id=1 | gruen (248 s) |
| `unit_r41_annette_se_bank/_toene/_lagen` | unveraendert | gruen (in der Suite) |
| `integration_r41_birkin_pistole` | Bestand | gruen (242 s) |
| `integration_r35_zgirl_5090` | Bestand | gruen (147 s) |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|android\|r37_masken36_ausschluss\|paket\|titelversion`, `-j 4` | Bestand, darin die Resolver-Pruefung fuer alle Typen | **1040/1040 gruen** (249 s) |

Ausgeschlossen wie in N2.6/N3.11: Umgebungsproblem df.exe und das echte release_alles.sh. Fensterlaeufe nur mit eigener exe-Kopie
`re15_pc_n4_*` (timeout auf die eigene PID), danach geloescht; ctest-Laeufe mit ihren eigenen Kopien.

### N4.7 Dateien

- Code:
  - `re15_port/engine/src/annette_r41.c` (Liegepose, Lage, kein_ziel, Abgang 6030, Tuerbank-Haken)
  - `re15_port/include/re15_annette_r41.h`
  - `re15_port/engine/src/re15_damage.c` (2 Stellen)
  - `re15_port/platform/pc/src/annette_se_pc.c` (re15_pc_annette_tuerbank)
  - `re15_port/platform/pc/main.c` (1 Zeile Haken)
- Assets und Generator: `re15_port/shared_assets/RE2/ANNETTE_4D.EMD` / `ANNETTE_4D_BLUT.EMD`,
  `re15_port/tools/r41_annette/annette_modell.py`, `re15_port/tools/r41_annette/re2_6141_liegen.bin`.
- Tests: `re15_port/tests/unit/test_r41_annette.c`, `re15_port/tests/unit/probes/r41_annette.cmake` (RE15_AN_AUSZUG),
  `re15_port/tests/integration/test_r41_annette.cmake`.
- Belege `AN_belege/n4/`:
  - `leiche_kampf_n4.jpg`, `leiche_vorher_nachher_n4.jpg`, `leiche_wiedereintritt_n4.jpg`
  - `zugstart6030_abgang_n4.jpg`, `zugstart6030_abgang_n4_mit_ton.mp4`
  - `n5_satz_kameras.png`
- Werkzeug `AN_belege/werkzeug/n4/`:
  - Laeufe: `n4_lauf.sh`, `eingabe_*.txt` (darin `eingabe_w8_nb4.txt` = w8 fuer den neuen Abgang)
  - RE2-Liegepose und Lage: `re2emd.py`, `rbj2.py`, `liege.py`, `lage.py`, `suche_liegen.py`, `leiche_lage.py`, `bildmass.py`
  - N5: `suche5.py`, `suche5.json`, `suche5.log`, `n5_blatt.py`
  - 6030: `zug6030.py`
  - NPC-Band aus dem RE1.5-RAM: `npc_band.py`

### N4.8 Offen und Commits

- N1 (Form-Spur): Die Riesenform kommt im Wagenweg mit HP 0; hier nicht angefasst.
- Die RE1.5-Typpruefung @0x80012178-84 gilt jetzt fuer jedes NPC (Typ >= 0x40). Ein lebendes NPC, das im Schusskeil naeher steht
  als ein Gegner, schluckt den Schuss wie im Original, statt Schaden zu nehmen. Fuer die Integration ist das eine
  Verhaltensaenderung in re15_damage.c fuer alle Spuren.
- Abnahme-Ketten mit festen Tastenskripten durch 6030: Die Szene endet 21 Bilder spaeter, und Leon steht danach bei
  (-3080,-23674) statt an der Tuer. Mit w8 bleibt die Kette deshalb in 6030 stecken (Lauf `kette`).
  - Angepasste Eingabe `werkzeug/n4/eingabe_w8_nb4.txt`: `W29,L0.367,W0.2,` wird zu `W29.7,L0.22,U0.4,`, der Rest bleibt
    gleich. Ein Sprungtest `t1` zeigt: Leon dreht auf 1999, geht bis x -3873, und die Tuer loest aus.
  - Lauf `kette2` (ganze Kette): Die Kette erreicht 5090, und die Szene laeuft bildgleich zu Abnahme 2 (Treffer F1640,
    Kampfstart F1776).
- Stimmen (M3), F4, F7, ZUG_AN-Alternative, NTSC-Bildzahlen: wie N3.12.
- Commits: Code/Tests/Assets, dann Doku-Commit mit diesem Abschnitt und den Belegen.

## N5. Nachbesserung 5 (2026-11-03)

Auftrag: kleine Nachtraege aus der bestandenen Abnahme 3 (`AN_abnahme_3.md`): A1 (falscher Beleg), H1 (Pistolen-Adresse), A2 (Test
"Begleiter in der Schusslinie"), H2 (Leiche schiebt Leon). Waehrend der Arbeit kam ein Nutzerentscheid zu A2/E1 (N5.2), der
Vorrang vor dem Orchestrator-Entscheid im Auftrag hat.

### N5.1 A1 und H1: Belege berichtigt (am Verhalten aendert sich nichts)

- **Plc_motion setzt Bild 0.** An 0x80052ef0 steht in beiden RE2-Programmen (`info/re2leon/PSX.EXE`, `SLUS_007.48`) das Wort
  `0x00431023` = `subu v0,v0,v1` (auch `ghidra_re2_Leon.txt:171650`). Selbst nachgelesen:
  - Die SCD-Opcode-Tabelle beginnt bei 0x800a74c8. Der Dispatcher liest sie mit `lbu v0,0(a1)` / `sll v0,v0,2` / `lui at,0x800a` /
    `lw v0,0x74c8(at)` / `jalr v0` @0x80053f70-8c.
  - Eintrag [0x3F] @0x800a75c4 = 0x80056b8c (Plc_motion), Eintrag [0] = 0x800537e4 (Nop: `pc += 1`).
  - Handler 0x80056b8c: `lw v0,28(a0)` (pc), `lw v1,340(a0)` (Arbeits-Entity), `lbu a1,2(v0)` (Clip), dann
    **`sb zero,333(v1)` @0x80056ba0 = `a060014d`** (Bildzaehler +0x14D = 0) und `sb a1,332(v1)` @0x80056bb8 (Clip +0x14C).
  - Wortsuche `a060014d` im ganzen EXE: nur 0x80056ba0, in beiden Programmen gleich.
  - Berichtigt in `annette_r41.c` (Kopf AN_FA_LIEGEN) und hier in N4.2.
- **hp < 0 ist kein Ziel, Pistole.** Die Schusswaffen laufen ueber FUN_800410CC (nicht FUN_800470C0): `lw s2,0(fp)`
  @0x80041254 (Kandidat), `lh v0,342(s2)` @0x80041280, `bltz v0,0x80041300` @0x80041288, 0x80041300 = `j 0x800417d0` /
  `addiu fp,fp,4` (naechster Kandidat). Ergaenzt in `annette_r41.c`, `re15_annette_r41.h`, `re15_damage.c`, `test_r41_annette.c`
  und hier in N4.3.
- Belege: `AN_belege/n5/re2_plc_motion.txt`, `AN_belege/n5/re2_kandidaten_tore.txt`.

### N5.2 A2 und E1: Begleiter in der Schusslinie, jetzt RE2-Regel (Nutzerentscheid)

**Nutzerentscheid (2026-11-03, woertlich):** "Mache die RE 2 Variante, das der Schuss durch begleiter durch geht."

Das ersetzt die NB4-Typpruefung (RE1.5 FUN_80011f50 @0x80012178-84) fuer die Begleiter. Vorher schluckte ein Begleiter vorn im
Keil jeden Schuss (Abnahme 3, E1).

**RE2, selbst disassembliert (`info/re2leon/PSX.EXE`, `SLUS_007.48` gleich):** Die Regel laeuft ueber hp, nicht ueber den Typ.

- Schusswaffen FUN_800410CC, Kandidatenliste ab 0x800cfe1c (`lui fp,0x800d` / `addiu fp,fp,-484` @0x80041208-0c), Ende
  `[0x800ce334]` (@0x800417d0-dc). Je Kandidat genau vier Tore, dann die Sichtlinie `jal 0x80050858` @0x800412f0:

  | Tor | Befehle |
  |---|---|
  | aktiv | `lw v0,0(s2)` / `andi v0,v0,0x1` / `beq` @0x8004125c-68 |
  | Trefferpause +0x1D3 | `lbu v0,467(s2)` / `bne` @0x80041270-78 |
  | **hp < 0** | **`lh v0,342(s2)` @0x80041280 / `bltz v0,0x80041300` @0x80041288** |
  | +0x10E & 0xC000 | @0x80041290-9c |

  Eine Typabfrage gibt es nicht.
- Messer, Bolzen und Explosion FUN_800470C0: dieselbe hp-Abfrage `lh v0,0x156(s0)` @0x80047148 / `bltz v0,0x8004740c` @0x80047150.
- Begleiter stehen in derselben Liste wie die Gegner (Claire-RAM ROOM6141: Annette em[0] @0x800c79b0) und tragen hp -1:
  Claire-RAM lauf3 e01..e22 (lebend) und e23..e30 (liegend) +0x156 = -1.
- RE1.5-NPCs tragen dieselbe Kennung aus ihrem INIT (+0x9a = -1 @0x8011c744). Im Port bestaetigt der Lauf unten Marvin mit hp -1.

**Bau (`re15_damage.c`, re15_player_weapon_fire):**

- In der Kandidatenschleife: `if (e->type >= 0x40u && e->type <= 0x4Du && e->hp < 0) continue;` mit der Kennzeichnung
  "RE2-Retail-Regel statt RE1.5 @0x80012178-84, Nutzerentscheid 2026-11-03".
  - PORT-ZUORDNUNG wie Runde 34 B1 in re15_resolve_attack: die RE2-hp-Bedingung fuer die NPC-Familie 0x40..0x4D. Das ist dieselbe
    Menge, die RE1.5 mit `sltiu v0,v0,0x40` @0x80012180 aussortiert hat.
  - Fuer Gegner < 0x40 bleibt die RE1.5-Semantik von hp < 0 unveraendert (Resolver liest nur das Vorzeichen, @0x80012520-2c).
- Die Typpruefung des Gewinners (`if (g_actors[best].type >= 0x40u) return 0;`) ist entfernt.
- Der Annette-Haken `re15_annette_r41_kein_ziel` bleibt. Er ist jetzt ein Teilfall (Typ 0x4D, hp -1) und dient der Szene und
  `test_r41_annette` als Zustandsabfrage.

**Alle Waffen, Wege im Port geprueft:**

| Weg | Waffen | Begleiter-Tor |
|---|---|---|
| re15_player_weapon_fire | Pistole, Schrot (w8), Magnum, MG, Messer/Rohr, Kanister-Explosion (`re15_player_weapon_fire(7)`, kanister2080_r41.c) | **neu** (NB5) |
| re15_re2_gl_kandidat | Granaten-Explosion und Flugkontakt (granate_r35.c) | schon da: Gate 3 `e->hp < 0` (@0x80047148-50) |
| re15_re2_gl_apply | RE2-Op 40, Bodenfeuer/Werfer (fx_plattform_pc.c `re2fx_applier`) | schon da: Gate 3 |
| re15_resolve_attack | Angriffskasten FUN_80012d60 | schon da: Runde 34 B1, 0x40..0x4D und hp < 0 |
| dining_r39.c | nur Gorilla-Typ | nicht betroffen |

**Was NB4 sonst brauchte, weiter erfuellt:**

- Leiche: Typ 0x4D, hp -1 -> kein Kandidat (allgemeine Regel und Haken).
- `unit_r41_annette` test_schuss (a): Leiche hp -1 st 0, G5 345 -> 330.
- test_schuss (b) ist umgestellt: das lebende NPC (hp -1) ist kein Kandidat, der Schuss geht durch auf die G5 (Rueckgabe = G5-Slot,
  G5 345 -> 330, NPC hp -1 st 0). Vorher erwartete (b) "ins Leere".

**Raum fuer den Integrationstest: ROOM10D0, nicht 2000 oder 3080.**

- ROOM2000 sub00 (selbst disassembliert): Ada 0x42 und Marvin 0x40 stehen im If-Zweig von Ck(4,244)=1 (`06 00 34 00` @0x01794).
  Die beiden 0x16 stehen im **Else-Zweig** (`07 00 2e 00` @0x017C8, Sce_em_set @0x017CC/@0x017E0). Sie sind also nie gleichzeitig im
  Raum. Das berichtigt Abnahme 3, Abschnitt 3.3.
- ROOM3080: Ada, Marvin und Birkin 0x36 stehen unter Ck(4,96)=0, aber sub00 startet sofort die Szene (Evt_exec sub02 @0x007E8).
  Es gibt dort kein freies Spiel.
- ROOM10D0 sub00 Ck(4,247)=0 (neuer Spielstand):
  - Marvin: Sce_em_set @0x01256 `44 00 40 40 .. 8f 16 .. 49 5c .. 76 04` = Typ 0x40 bei (5775,23625).
  - Zombie: Sce_em_set @0x0126A `44 01 10 0e ..` = Typ 0x10, Deskriptor 0x0E (schlafend liegend, wacht bei Spielerabstand
    < 0xBB8 auf).
  - Erkundungslauf: Marvin bleibt in st 4 / ss1 6 an seiner Lage, hp -1; der Zombie wacht auf.

**Test `integration_r41_begleiter_schusslinie`** (`tests/integration/test_r41_begleiter_schusslinie.cmake`, eingetragen in
`tests/unit/probes/r41_annette.cmake`):

- Aufbau: echte exe, Debug-Sprung 10D0, Pistole (RE15_EQUIP=3, RE15_GIVE=3:100), Zielen + Feuer 6 s.
- Der Zombie (Aktor 2) wird ueber den Messhaken RE15_GEGNER_POS 2950 vor Leon festgehalten: wach, aber ausser Griffweite.
  Marvin bleibt unberuehrt.
- Pistolen-Keil: Halbbreite Radius + 1000 (`s_player_wpn_reach[3]`).

| Lauf | Lage | Erwartet | Ergebnis |
|---|---|---|---|
| (a) `davor` | Leon (3800,23625) Gier 0, Zombie (6750,23625); Marvin 1975 vor Leon genau auf der Linie | Zombie >= 2 Treffer, Marvin st/hp unveraendert | 8 Schuesse, Zombie 98 -> 34 (4 Treffer, erster F289), Marvin st 4 hp -1 |
| (b) `seitlich` | Leon (3800,25500), Zombie (6750,25500); Marvin 1875 seitlich (> 450 + 1000) | Zombie >= 2 Treffer, Marvin unveraendert | 8 Schuesse, Zombie 98 -> 34 (4 Treffer), Marvin st 4 hp -1 |
| (c) Gegenprobe 1: re15_damage.c = NB4 (HEAD 37f565a3c) | wie (a) | rot | **rot:** "davor: Zombie hinter Marvin nur 0x getroffen" (8 Schuesse, Zombie 98 -> 98) |
| (d) Gegenprobe 2: re15_damage.c aus b646c72a6^ (vor NB4) | wie (a) | rot | **rot:** "davor: Begleiter veraendert ... F289: Marvin st 4 -> 3, hp -1 -> -6" |

- In (c) und (d) war der Lauf `seitlich` jeweils gruen (4 Treffer).
- Die Gegenproben liefen mit eigenen exe-Kopien. Die Sonde `npc_schuss.c` der Abnahme ist eine Bibliotheks-Sonde ohne Raum; fuer
  die exe-Gegenprobe passte sie nicht.
- Belege: `AN_belege/n5/gegenprobe_nb4.txt`, `gegenprobe_vor_nb4.txt`. Werkzeug: `AN_belege/werkzeug/n5/lauf.sh` (Erkundung).

### N5.3 H2: Leiche schiebt Leon - RE2 hat die Kollision auch, bleibt

**Frage:** Hat die tote Annette in RE2 (Claire ROOM6141) eine Kollision gegen den Spieler?

**Antwort: ja.** Der Port bleibt, wie er ist.

**Mechanismus (Leon-EXE selbst disassembliert, im Claire-EXE SLES_109.72 per Muster wiedergefunden und gegengelesen):**

1. Spieler-Tick: `jal 0x800355c4` @0x80026628 (Claire `jal 0x8003319c` @0x80025b68). Vorher prueft er nur das globale
   Pausenbit (`lhu` / `andi v0,v0,0x8000` / `bne` @0x80025b4c-58).
2. FUN_800355C4 (Claire 0x8003319C) laeuft ueber die Entity-Liste: Leon 0x800cfe14 bis `[0x800ce334]`, Claire 0x800c79a8 bis
   `[0x800c5f0c]`.
   - Sich selbst ueberspringt sie: `beq s1,a0` (Claire @0x800331ec).
   - Jede aktive Entity (`andi v0,v0,0x1` @0x800331fc) geht an `jal 0x80032904` @0x80033208 (Leon `jal 0x80034d0c` @0x80035630).
3. FUN_80034D0C (Claire 0x80032904) schiebt den Spieler aus den Segmenten der Entity. Die Tore:
   - `(e.w0 | p.w0) & 2` und `(e.w0 & p.w0) & 0x1000` und `p.w0 & 4` -> kein Schub (Claire @0x80032940-64, Leon @0x80034d48-6c).
   - Segmentzahl `lw v0,488(s6)` (+0x1E8) @0x8003296c, -1, `bltz` @0x8003299c (Leon @0x80034d74 / @0x80034da4). 0 Segmente
     heisst kein Schub.
   - Segmente ab +0x84 (`addiu s5,s6,132`): x/y/z +0x0/+0x4/+0x8, Radius +0x16 (`lhu s7,22(s5)`), Halbhoehe +0x1A.
   - XZ-Ueberdeckung: R = Summe der Radien.
   - Hoehen-Tor `-(hA+hB) < dy < hA+hB` @0x80032a48-70 (Leon @0x80034e50-78).
   - Bit 0x100000 der Entity begrenzt den Schub auf +-100 (Leon @0x80035048-50).

**RAM (Claire lauf3 e23, e24, e25, e30; Annette liegt seit e23):**

| Feld | Annette (@0x80125d4c, id 0x49) | Claire |
|---|---|---|
| w0 | 0x00002001 (Bit 0x2 = 0, Bit 0x1000 = 0, Bit 0x100000 = 0) | 0x00000001 |
| in der Liste | 0x800c79b0 (Ende 0x800c79b8) | 0x800c79a8 |
| +0x1E8 (Segmente) | **1** | 1 |
| Segment | Mitte (-12300, -450, -15300) = Lage (y 900) + 0x14 (-1350); **Radius 350**, Halbhoehe 1350 | y -1350, Radius 350, Halbhoehe 1350 |

- dy = 900 < 2700, also greift das Hoehen-Tor nicht.
- Alle Tore lassen den Schub durch: Die liegende Annette schiebt Claire aus einem Kreis von 350 + 350 = 700 um ihre Wurzel.
- Das ist derselbe Mechanismus wie beim Port-Befund "um 100 seitlich versetzt".

**Radius (Entscheid nach N5.3: die Szene folgt RE2; durch N6 eingeschraenkt auf die RE2-Szene ab Szenenstart und die Leiche, in 6000/6030 und in 5090 vor der Szene gilt wieder der RE1.5-Kasten 450/1530):** Annette (0x4D) traegt jetzt das RE2-Segment r 350 / Halbhoehe 1350 /
Mitte Lage - 1350 statt des NPC-Familienkastens 450/1530 (`re15_enemy_apply_hitbox`). Beleg: FUN_80034D0C +0x1E8 @0x80034d74,
+0x16 @0x80034dac, +0x1A @0x80034e5c. Claire-RAM lauf3 zeigt lebend (e01..e22) und liegend (e23..e30) dieselben Werte, deshalb gilt
er fuer die lebende Annette und die Leiche, ohne getrennte Fuehrung. Die Leiche schiebt den Port-Spieler (r 450) jetzt ab 800 statt
900: `unit_r41_annette` test_leiche_radius ist gruen, die Gegenprobe mit 450 ist rot (3 Fehler,
`AN_belege/n5/gegenprobe_radius450.txt`).

Belege: `AN_belege/n5/leiche_kollision_re2.txt`, `claire_schub_code.txt`, `leon_schub_code.txt`. Werkzeug:
`AN_belege/werkzeug/n5/leiche_kollision.py`, `claire_code.py`, `cdis.py`. Die Savestates lauf3 und SLES_109.72 liegen wie
bisher unversioniert im Scratch der Sitzung (Pfade in den Skripten).

### N5.4 Tests

Bau nur ueber `bash re15_port/tools/local_build.sh build` (LOCAL-BUILD-OK). Laeufe in diesem Arbeitsbaum:

| Test | Ergebnis |
|---|---|
| unit_r41_annette (+ `_se_bank`, `_se_toene`, `_se_lagen`) | gruen; test_schuss (a) Leiche hp -1 st 0, G5 330; (b) lebendes NPC hp -1 st 0, G5 330 |
| integration_r41_annette | gruen (271 s) |
| integration_r41_begleiter_schusslinie (neu) | gruen (62 s); Gegenproben (c) und (d) rot wie in N5.2 |
| integration_r41_birkin_pistole | gruen (323 s) |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|titelversion`, `-j 4` | **1050/1050 gruen** (1190 s) |

Ausgenommen sind die 7 `unit_r38_releaseskript_*` und die 9 `unit_r39_titelversion_*` (df-Haenger der Maschine, laut Auftrag).
Die Integrationstests liefen nacheinander und nicht parallel zur Suite.

### N5.5 Dateien, Commits und Offen

- Code: `re15_port/engine/src/re15_damage.c` (RE2-Regel, Zitate), `re15_port/engine/src/annette_r41.c`,
  `re15_port/include/re15_annette_r41.h` (Zitate/Kommentare).
- Tests: `re15_port/tests/unit/test_r41_annette.c` (test_schuss (b)), `re15_port/tests/integration/test_r41_begleiter_schusslinie.cmake`
  (neu), `re15_port/tests/unit/probes/r41_annette.cmake` (Eintrag).
- Belege: `AN_belege/n5/`, `AN_belege/werkzeug/n5/`.
- Offen:
  - Leichen-Radius: entschieden und gebaut (RE2 350/1350, N5.3).
  - Die RE2-Regel gilt im Port fuer die NPC-Familie 0x40..0x4D mit hp < 0. Ein NPC mit hp >= 0 waere wie in RE2 treffbar; im Port
    setzt der NPC-INIT hp -1 (@0x8011c744), im Lauf 10D0 ab Bild 6 (Bild 1..5 traegt Marvin noch hp 0).
  - Release Notes: Der E1-Punkt der Abnahme 3 ist durch den Nutzerentscheid erledigt.

## N6. Nachbesserung 6 (Kasten 0x4D je Raum/Phase) (2026-11-07)

Auftrag: Abnahme 4 (`AN_abnahme_4.md`) M1, blockierend. cefd57402 gab in `re15_enemy_apply_hitbox` jeder 0x4D das RE2-Segment
r 350 / Halbhoehe 1350, also auch der lebenden Annette in 6000/6030 und in 5090 vor der Szene. Dort ist sie RE1.5-Inhalt. Gemessen
(Abnahme 4 `an6030_pult`): Leon hielt am Pult bei 802 statt 900. Das widersprach auch 8.3 ("Ihr Verhalten in 6000/6030 bleibt das
alte").

**Entscheid des Orchestrators (nach "Original oder nicht", keine Wahlfrage): Variante NB6.** In RE1.5-Raeumen und in 5090 vor der
RE2-Szene gilt der RE1.5-Kasten 450/1530. Das RE2-Segment 350/1350 gilt nur ab Beginn der RE2-Szene und fuer die Leiche. Die
Nachbesserung 5 (Schuss geht durch Begleiter) bleibt unveraendert.

### N6.1 Der RE1.5-Kasten der 0x4D (selbst nachgelesen)

Bytes in `info/Re1.5/PSX/BIN` (= `re15_port/shared_assets/PSX/BIN`), Overlay ohne Kopf, geladen @0x80100000 (Datei-Offset =
Adresse - 0x80100000):

| Stelle | STAGE6.BIN (6000/6030) | STAGE5.BIN (5090) |
|---|---|---|
| Registrierung Typ 0x4D in 0x80072bac + 0x4D*4 = 0x80072ce0 | `lui v0,0x8010` / `addiu v0,v0,6048` @0x80101df0-f4 (Woerter `3c021080` `244217a0`), `sw v0,11488(at)` @0x80101dfc (`ac222ce0`) -> Wurzel 0x801017a0 | `addiu v0,v0,-10540` @0x8011ddb0 (`2442d6d4`), `sw v0,11488(at)` @0x8011ddb8 (`ac222ce0`) -> Wurzel 0x8011d6d4 |
| INIT: +0x78 = Kasten | `lw v0,10128(v0)` @0x80101958 (Wort `8c422790`; [0x80102790] = `84 27 10 80` = 0x80102784), `sw v0,120(v1)` @0x80101960 (`ac620078`) | `lw v0,2012(v0)` @0x8011d88c (`8c4207dc`; [0x801207dc] = `d0 07 12 80` = 0x801207d0), `sw v0,120(v1)` @0x8011d894 |
| Kasten-Bytes | @0x80102784 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` | @0x801207d0 `00 00 06 fa 00 00 c2 01 fa 05 c2 01` |
| = {ofs_x, ofs_y, ofs_z, r_min, h, r_max} | {0,-1530,0,450,1530,450} | {0,-1530,0,450,1530,450} |

Leser (Abnahme 4, 4.2): Wand-Klemme der 0x4D-Wurzel `lw v0,120(a0)` / `lhu a1,6(v0)` / `jal 0x8003b0a4` @0x801018c4-d0, Bodenprobe
FUN_8001bd60 @0x8001bd94-a8, Schub FUN_8002aec4 `lhu t4,6(s6)` / `lhu t4,6(s5)` @0x8002af40/@0x8002af4c.

Der alte Port-Wert 450/1530 war fuer 0x4D also byte-genau. Der Kommentar nannte bisher nur den STAGE1-Kasten @0x80121658; jetzt
stehen die eigenen 0x4D-Belege im Code.

### N6.2 Umschaltpunkt RE1.5 -> RE2

| Zustand | Kasten | Wer setzt ihn | Beleg |
|---|---|---|---|
| 6000 lebend (sub00 @0x00F44/@0x00F68) | RE1.5 450/1530, Versatz -1530 | `re15_enemy_apply_hitbox` beim Sce_em_set (scd_vm.c) | STAGE6 @0x80101958 -> 0x80102784 |
| 6030 am Pult (sub07 @0x010D4/@0x010F8) und in der Port-Zugstart-Szene NB1 | RE1.5 450/1530 | dto. | STAGE6 @0x80101958 -> 0x80102784. Die Zugstart-Szene laeuft in einem RE1.5-Raum mit RE1.5-Skriptsegmenten (N1.3); sie ist keine RE2-Szene |
| 5090 vor der Szene (sub00 @0x0122A) | RE1.5 450/1530 | dto. | STAGE5 @0x8011d88c -> 0x801207d0 |
| 5090 ab Szenenstart | **RE2 r 350, Halbhoehe 1350, Versatz -1350** | `annette_r41.c` `re2_segment` in `re15_annette_r41_evt_exec`, im selben Bild, in dem Evt_exec sub04 @0x01296 in das Programm nach Claire ROOM6141 MAIN sub02 (ab @0x01024) umgelenkt wird | RE2 FUN_80034D0C: Segmentzahl `lw v0,488(s6)` (+0x1E8) @0x80034d74, Segmente ab `addiu s5,s6,132` @0x80034d78, Radius `lhu s7,22(s5)` (+0x16) @0x80034dac, Halbhoehe `lhu v0,26(s5)` (+0x1A) @0x80034e5c. Claire-RAM lauf3: schon im ersten Abzug der Szene **e01 (Skript @0x010B0 in sub02**, `re2_claire/ram_reihe_voll.md`) traegt Annette @0x80125d4c +0x1E8 = 1, +0x14 = -1350, +0x16 = 350, +0x1A = 1350 (`abn4/claire_annette_ram.txt`) |
| 5090 Leiche in derselben Szene (Phase LEICHE/FREI) | RE2 | bleibt vom Szenenstart | Claire-RAM lauf3 e23..e30 liegend gleich |
| 5090 Wiedereintritt, Leiche (Phase RUHE) | RE2 | `re2_segment` in `ruhe_setzen` (nach dem RE1.5-Spawn @0x0122A, der zuerst 450/1530 gibt) | RE2 MAIN sub00 @0x00D16 setzt die liegende RE2-Annette; Segment wie oben |

Der Port fuehrt sonst nichts nach: Der NPC-Takt (`re15_npc_ai_tick`) setzt den Kasten nicht neu, `re15_enemy_ai_init` laeuft fuer die
NPC-Familie nicht. Faellt der Szenenstart aus (keine Riesenform-Bank, Protokoll "keine Szene"), bleibt der RE1.5-Kasten.

### N6.3 Code

- `re15_port/engine/src/re15_damage.c` `re15_enemy_apply_hitbox`: `case 0x4d: r = 450; h = 1530;` mit den STAGE6/STAGE5-Belegen
  aus N6.1 und dem Hinweis, dass das RE2-Segment nur `annette_r41.c` setzt.
- `re15_port/engine/src/annette_r41.c`: `re2_segment` (AN_RE2_SEG_R 350, AN_RE2_SEG_HH 1350, AN_RE2_SEG_DY -1350, je mit Adresse),
  Aufruf beim Szenenstart und in `ruhe_setzen`.
- Die Schussregel aus N5.2 ist unveraendert.

### N6.4 Riegel und Gegenproben

`unit_r41_annette`, `test_leiche_radius` (erweitert) und `test_szene` (zwei neue Pruefungen):

| Fall | Soll | Ergebnis NB6 |
|---|---|---|
| 6000 lebend (Ck(3,100)=0, @0x00F44) | 450/450/1530/-1530 | 450/450/1530 Versatz (0,-1530,0) |
| 6030 Pult ((4,23)=1, (3,89)=0, @0x010F8, Lage (-1790,-22272)) | 450/450/1530/-1530 | 450/450/1530 Versatz (0,-1530,0) |
| Pult-Riegel: Leon westlich bei 850 / 810 / 950, ein Schub-Durchlauf `re15_body_push_player` | 900 / 900 / 950 | **901 / 901 / 950** |
| 5090 vor der Szene ((3,89)=1, (9,133)=1, (3,42)=0) | 450/450/1530/-1530 | 450/450/1530 Versatz (0,-1530,0) |
| 5090 ab Szenenstart (`re15_annette_r41_evt_exec(0x5090, 4)`, Phase WARTEN) | 350/350/1350/-1350 | 350/350/1350 Versatz (0,-1350,0) |
| `test_szene`: vor der Szene / nach der ganzen Szene (Leiche) | 450/1530 / 350/1350 | gruen |
| Leiche beim Wiedereintritt ((3,42)=1): beim Spawn, nach `ruhe_setzen` | 450/1530, dann 350/1350 | 450/1530, dann 350/350/1350 Versatz (0,-1350,0) |
| Leiche: Leon bei 850 / 750 | 850 (kein Schub) / 800 | **850 / 801** |

Gegenproben, je mit eigener Bau- und Testrunde im Arbeitsbaum (danach zurueckgestellt und neu gebaut):

| Gegenprobe | Ergebnis |
|---|---|
| 1: `re15_damage.c` aus cefd57402 (case 0x4d 350/1350) | **rot, 7 Fehler**: 6000, 6030, 5090 vor der Szene und Wiedereintritt-Spawn 350 statt 450; Pult-Riegel "Leon bei 850 nicht auf 900 geschoben (Abstand 850)", "bei 810 ... (Abstand 810)" (`AN_belege/n6/gegenprobe_cefd57402.txt`) |
| 2: `annette_r41.c` ohne `re2_segment`-Aufrufe | **rot, 5 Fehler**: ab Szenenstart und als Leiche 450/1530; Leiche schiebt bei 850 und 750 auf 901 (`AN_belege/n6/gegenprobe_ohne_umschaltung.txt`) |

### N6.5 Fensterlauf an6030_pult (wie Abnahme 4 5.3)

Eigene exe-Kopie des Arbeitsbaums nach 99bbe97ba (`re15_pc_annb6_*`, nur eigene PID per `timeout`), echtes Fenster, Renderer des
Nutzers, ohne AUTOSHOT und SOFTWARE_RENDER. Dieselbe Umgebung wie Abnahme 4 `alle4b.sh`: `RE15_DEBUG_JUMP=6030@250`,
`RE15_PLAYER_POS=-2600,-22272,0,0`, `RE15_SET_FLAG=3:48,3:49,3:100,3:239,4:23`, Eingabe `U2,W4` ab Bild 100,
`RE15_EXIT_AT=400#6030`, `RE15_FRAMEDUMP=90-400/10:f`. Werkzeug `AN_belege/werkzeug/n6/lauf6.sh`, Auswertung `abstand6.py`.

| Bild | Leon | Annette (0x4D) | Abstand |
|---|---|---|---|
| F1..F5 (vor dem NPC-INIT, hp 0) | (-2600,-22272) | (-1790,-22272) | 810 |
| F6 (INIT, hp -1, erster Schub) | (-2691,-22272) | (-1790,-22272) | 901 |
| F100..F400 (Leon geht 2 s auf sie zu) | -2691..-2693 | (-1790,-22272) | **901..903** |

Abnahme 4 mass mit cefd57402 an derselben Stelle 802 (Minimum 801 bei F102). Jetzt haelt der RE1.5-Kasten Leon bei 900. Bildleiste
`AN_belege/n6/an6030_pult_nb6.jpg` (F90..F400, Leon hinter Annette am Pult, mit sichtbarem Abstand).

### N6.6 Tests

Bau nur ueber `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build` (LOCAL-BUILD-OK). ctest aus
Git-Bash mit `PATH=/c/msys64/mingw64/bin:$PATH`, `SDL_ASSERT=always_ignore`, `--test-dir <Baum>/re15_port/build`. Summenzeilen
woertlich (`AN_belege/n6/ctest_nb6.txt`):

| Lauf | Summenzeile |
|---|---|
| unit_r41_annette, _se_bank, _se_toene, _se_lagen | `100% tests passed, 0 tests failed out of 4` |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|titelversion\|android\|paket\|r37_masken36_ausschluss`, `-j 2` (darin alle Unit-Tests aus `git grep` auf 0x4d/0x4D/Annette in `re15_port/tests`, z.B. unit_r41_kollision_npc, unit_1090_npc_prop_push, unit_r36_kartenbits, unit_r35_affen_npcband) | `100% tests passed, 0 tests failed out of 1040` (627 s) |
| integration_r41_annette, _r41_begleiter_schusslinie, _r36_story_leon, _r36_story_elza, _r41_kollision_npc3070, _r41_kollision_npc3001, _r35_zgirl_5090, _r41_birkin_pistole, `-j 1` | `100% tests passed, 0 tests failed out of 8` (1643 s) |
| uebrige Integrationstests aus `git grep` (r32_tor_hell, r35_zgirl, r36_elza_* 5, r37_elzaems_* 12, r37_sperre1050, r37_szene11c0, r38_elzatoene_* 2, r38_tagebuch_* 3, r41_wasser20b0, r41_wasser20b0_npc), `-j 1` | `100% tests passed, 0 tests failed out of 28` (1851 s) |

Die Integrationslaeufe liefen nacheinander; die Unit-Suite lief parallel zur ersten Integrationsgruppe. Kein roter Fenster-Riegel war
nachzufahren.

### N6.7 Dateien, Commits und Offen

- Code: `re15_port/engine/src/re15_damage.c`, `re15_port/engine/src/annette_r41.c`.
- Test: `re15_port/tests/unit/test_r41_annette.c` (`test_leiche_radius`, `test_szene`).
- Belege: `AN_belege/n6/` (`unit_r41_annette_nb6.txt`, `gegenprobe_cefd57402.txt`, `gegenprobe_ohne_umschaltung.txt`,
  `an6030_pult_abstand.txt`, `an6030_pult_nb6.jpg`, `ctest_nb6.txt`), Werkzeug `AN_belege/werkzeug/n6/`.
- Commits: 99bbe97ba (Code + Riegel), dieser Dossier-Commit.
- N5.3 "Radius" und N5.5 "Leichen-Radius" gelten jetzt nur noch fuer die RE2-Szene und die Leiche (siehe oben).
- Offen: nichts Neues. Die Stimmen (Abnahme 4 M3) fehlen weiter.

## Integration A2 (2026-11-08): Querbezug AN x ST80 im Zug 6030 und kombinierter 5090-Lauf

Auftrag: Gegenpruefung Welle 2 Befund 2 (Querbezug ROOM6030 AN sub05 x ST80 sub02/08/09 ungemessen, durchgehender Lauf
"erster Besuch 6030 -> Tor -> zweiter Besuch" fehlt) und Befund 3 (5090 mit den AN-Bits wie im echten Weg). Integrationsbaum
`r41/integration`, Leon. Elza: entfaellt fuer den Zug, ROOM6031 hat keine 0x4D (Abschnitt 2.2) und keine ST80-Umleitung
(alle `k_s80`-Eintraege tragen 0x30C1/0x6030); `integration_r36_story_elza` ist gruen (A2.7).

### A2.1 Mechanismus (scd_dump_room.py, Datei-Offsets)

| Stelle | Bytes | Wirkung |
|---|---|---|
| ROOM6000 sub07 @0x011B4 | `22 03 59 01` | Torschalter: Set(3,89)=1 |
| ROOM6000 sub07 @0x011BC / @0x011C0 | `21 03 16 00` / `22 03 16 01` | Wagen-Szene als gelaufen markiert, falls nicht gelaufen |
| ROOM6000 sub07 @0x011CA / @0x011CE | `21 04 17 00` / `22 04 17 01` | Fuehrerstand-Szene als gelaufen markiert, falls nicht gelaufen |
| ROOM6030 sub07 @0x010D0 / @0x010E8 | `21 04 17 00` / `04 ff 18 08` | (4,23)=0 -> sub08 (ST80 vorne/voll) |
| ROOM6030 sub07 @0x010F4 / @0x0110C | `21 03 59 00` / `04 ff 18 09` | sonst nur VOR dem Tor -> sub09 (ST80-Nachtrag) |
| ROOM6030 sub07 @0x01128 / @0x01132 | `21 03 59 01` / `04 ff 18 05` | nach dem Tor -> sub05 (AN-Zugstart) |
| ROOM6030 sub06 @0x0108E / @0x010C4 | `21 03 59 01` / `04 ff 18 05` | Wagen nach dem Tor -> sub05 (AN k_tuer) |

Folge: nach dem Tor laufen weder die Wagen-Szene (sub01 @0x00D62 Ck(3,22)=0) noch sub08/sub09 je wieder; der einzige
Fuehrerstand-Haken ist sub05 @0x01132. Nebenbefund: der AN-Zweig "erster Besuch erst nach dem Tor: sub08 laeuft, Zugstart beim
naechsten Eintritt" (annette_r41.c zug_evt_exec) ist im echten Spiel nicht erreichbar, weil der Torschalter @0x011CE (4,23)
setzt; er greift nur mit Debug-Flags. Unveraendert gelassen.

### A2.2 Messweg

Messhaken (Commit 8f09f8fe0, env-gegated): Eingabeskript mit einem Abschnitt je Raum (`|`, input_pc.c), RE15_FIRE_AOT als
Liste, RE15_FLAG_WATCH, RE15_EXIT_AT `!k`, Karte `vor6030 tor_bereit` ((3,48)/(3,49) ROOM5080 sub01 @0x00792/@0x00796,
(3,239) ROOM6000 sub02 @0x00F8E). Start ROOM6000 vor der Zugtuer, nur normale Eingaben im Zug (Wege der Riegel P4j/P4k),
Ausgang Slot 2 / Torschalter Slot 5 (msg 5, Ja) / Tuer Slot 3 per FIRE_AOT. Software-Renderer, SDL_AUDIODRIVER=dummy, eigene
exe-Kopien, nur eigene PIDs beendet. Werkzeug und Logauszuege: `INT_belege/a2_zug6030/` (`werkzeug/lauf.sh`, `wege.sh`,
`b3.sh`; je Lauf `<lauf>.txt`).

### A2.3 Ergebnis je Weg (Folge der angezeigten Ids ROOM6030; Bits)

| Weg | Stand | Folge | Bits | Urteil |
|---|---|---|---|---|
| F vorne -> Wagen -> ohne Rueckkehr -> Tor -> vorne | dabac8d22 (`weg_F_vor`) | 11 31 13..19 / 0 1 21 2 3 22 4 5 6 7 23 24 8 / **27 28 29 30** | (9,128) bleibt 1, (9,133) 0 -> 1 | **Widerspruch**: msg 28 "...Sherry is back there!", obwohl Annette nichts von Sherry weiss (msg 25 nie); der Nachtrag 40/25 kommt nie |
| F | nach Fix (`weg_F_nach`) | 11 31 13..19 / 0 1 21 .. 8 / **40 25** (F638, 1. Betreten nach dem Tor) / **27 28 29 30** (F2356, naechstes Betreten) | (9,128) 1 -> 0 F638, (9,133) 0 -> 1 F2357 | widerspruchsfrei |
| W Wagen -> vorne -> Tor -> vorne | beide Staende | 0 1 21 .. 8 / 11 12 25 26 13..19 / 27 28 29 30 | (9,128) nie, (9,133) F638 | widerspruchsfrei, Zugstart sofort |
| R vorne -> Wagen -> zurueck -> Tor -> vorne | beide Staende | 11 31 13..19 / 0 1 21 .. 8 / 40 25 / 27 28 29 30 | (9,128) 1 -> 0 vor dem Tor, (9,133) F638 | widerspruchsfrei, Zugstart sofort |

Jede Zeile hoechstens einmal (28 Zeilen). Der Wagen-Eintritt nach dem Tor zeigt in allen Wegen `[annette] ROOM6030 Wagen (Cut 6):
Zugstart nicht gelaufen -> Ausgang = Tuer` und keine Wagen-Szene: ST80 sub02 und AN k_tuer treffen im echten Weg nie zusammen,
weil der Torschalter @0x011C0 (3,22) setzt. Tor jeweils F284 (ROOM6000 (3,89) 0 -> 1).

### A2.4 Fix (Commit 3b2eec448)

Kleinster belegter Fix, keine neue Zeile, kein neues Bit: der offene ST80-Nachtrag laeuft an der einzigen nach dem Tor noch
erreichbaren Fuehrerstand-Stelle, sub07 @0x01132 (Regel ST80 NB1 "naechstes Betreten des Fuehrerstands nach dem Wagen").
- `k_s80_6030_sub05_nachtrag_tor` (262 B, szenen_bauen.py, reproduzierbar): Set(9,128)=0 (Form @0x0136C), Sce_em_set Annette =
  @0x010F8..0x0110C (sub07 setzt sie ab (3,89)=1 nicht mehr), Evt_exec sub05 = @0x01132..0x01136 (Licht-Schleife wie im
  Original), dann der Nachtrag-Rumpf des bisherigen Programms (@0x0113E.., msg 40, msg 25, Schluss @0x0131E..0x01376).
  Gate (9,128)=1, (3,89)=1, Cut 0 (sub00 @0x00D2C Case 0; im Wagen Cut 6/8/12 nicht).
- annette_r41.c zug_evt_exec: der Zugstart wartet, solange (9,128)=1. Haken-Reihenfolge op_evt_exec (scd_vm.c): story80 vor
  annette; ohne die Wartebedingung ueberschriebe der AN-Haken die Umleitung. Der Zugstart folgt beim naechsten Betreten (wie
  nach sub08: 0x4D steht -> Original-sub05).

### A2.5 Weitere Wege (gemessen; kein Beleg entscheidet sie -> Nutzerfragen)

| Weg | Lauf | Folge | Befund |
|---|---|---|---|
| F0 vorne -> ohne Wagen -> Tor -> vorne | `weg_F0` | 11 31 13..19 / (Tor) / 40 25 / 27 28 29 30 | Die Wagen-Szene laeuft nie (Torschalter @0x011C0 setzt (3,22)); Leon sagt msg 40 "There's a little girl in the back... Sherry." ohne Johns Vorstellung (msg 21). Vor dem Fix: msg 28 ohne Wissen Annettes. Die Bits unterscheiden F0 nicht von F ((3,22)=1 in beiden). |
| N Zug erst nach dem Tor betreten | `weg_N` | (Tor) / 27 28 29 30 | Weder Wagen- noch Fuehrerstand-Szene (Torschalter @0x011C0/@0x011CE), Zugstart sofort mit "...Sherry is back there!". |

Die Disc ueberspringt nach dem Tor beide Zug-Szenen; welcher Satz in F0/N an welcher Stelle stehen soll, entscheidet kein Beleg.

### A2.6 Befund 3: 5090 mit (3,89) und (9,133)

Lauf `b3_leon` (Debug-Sprung 5090 Wagen B wie integration_r41_g5_tod, RE15_SET_FLAG 3:89,9:133, Rakete 18:200 erst nach dem
Kampfstart, Renderer direct3d - nicht der Software-Renderer, der mit K5c abstuerzt, Befund 1). Zeitachse:
F280 Szenenstart statt sub04 (Krach) -> F462 Sturz -> F477 gelandet -> F710 Hieb -> F752 Treffer (Blut 15/15, ANNETTE_4D_BLUT.EMD)
-> F786 liegt, Schrei -> F888 Kampfstart (sub04), Form frei (g 0x13) -> Raketen F949 699, F980 398, F1012 97, F1044 -204 (wie
g5_tod ohne AN) -> F1045/F1046 Rueckstoss (st 2, Clip 13, HP 1) -> F1107 Clip 8 -> F1135 Sturz (Clip 12) -> F1192 G5-Vorlauf
(Typ 0x36 HP 600) -> F1247 Armierung -> F2409 Szenenende (Spieler x 13873, Cut 12). Spieler-HP 100 bis F2409 (F2844 80: G5-Biss
nach dem Szenenende, der Lauf steht ohne Rueckzug). Annette am Ende Clip 24 Bild 0 @(4856,-22748) Gier 1170, hp -1
(Begleiter-Regel NB5); Riesenform-Asset "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips". exit 0, kein Absturz -> Riegel
`integration_r41_annette_g5` (gruen, dieselben Bilder). Beleg `INT_belege/a2_zug6030/b3_leon.txt`.

### A2.7 Tests (Windows, 320x240)

- `unit_r41_story80_torweg6030` (neu): Weg F `11 31 13 14 15 16 17 18 19 0 1 21 2 3 22 4 5 6 7 23 24 8 40 25 27 28 29 30`, 28 Zeilen,
  keine doppelt, 40/25 vor 27/28; Weg W Zugstart sofort ohne Nachtrag -> `100% tests passed, 0 tests failed out of 1`.
- `integration_r36_story_torweg_f/_w/_r` (neu, durchgehend, -j 3) -> `100% tests passed, 0 tests failed out of 3`,
  `Total Test time (real) = 464.86 sec`.
- `integration_r41_annette_g5` (neu) -> `100% tests passed, 0 tests failed out of 1`, `Total Test time (real) = 113.15 sec`.
- Gruppe `-R "annette|story|kartenbits|g5auftritt|r41_story80|zug" -E "torweg|annette_g5" -j 4`: `100% tests passed, 0 tests failed out of 25`, `Total Test time (real) = 519.00 sec` (darin integration_r36_story_leon 518,92 s, _zug 449,79 s, _elza 270,23 s, integration_r41_annette, unit_r41_annette*, unit_r41_g5auftritt, unit_r36_kartenbits).
- `-R "^(unit_|prop_|probe_)" -j 6`: `100% tests passed, 0 tests failed out of 1083`, `Total Test time (real) = 601.63 sec`.
- PSX-Syntax story80_r41.c / annette_r41.c (mipsel-none-elf-gcc PSn00bSDK 0.24, -fsyntax-only) rc 0.
- Offen: Linux-Lauf der neuen Riegel; Elza-Zug unberuehrt (s. oben).
