# Spur AN: Annette-Szene im hinteren Zugwagen (Runde 41, laufendes Dossier)

Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Basis `r41/form` 62f0be3ac. Wer nach einem Abbruch weitermacht,
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
