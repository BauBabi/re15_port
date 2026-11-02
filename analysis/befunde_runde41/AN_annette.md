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
| Phase 2: Bau | offen |

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
- **F6 (Schrei):** Untertitel fuer den Schrei ja oder nein; sonst nur Ton.
