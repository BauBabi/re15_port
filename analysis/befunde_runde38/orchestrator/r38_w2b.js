export const meta = {
  name: 'r38-runde',
  description: 'Runde 38: 20 Bau-Spuren je Ermittlung+Bau -> unabhaengige Abnahme -> Nachbesserung, in eigenen Arbeitsbaeumen (Semaphor)',
  phases: [
    { title: 'Ermittlung+Bau', detail: 'Dossier, Messung/Bisect, RE-Beleg, Code, Tests, Suite im eigenen Baum' },
    { title: 'Abnahme', detail: 'unabhaengig gegen den Wortlaut des Nutzers am gebauten Stand gemessen' },
    { title: 'Nachbesserung', detail: 'Maengel der Abnahme beheben, erneut messen' },
  ],
}

const ROOT = 'C:/workspace/git/reAi_v2'
const BASIS = 'cb782bda'
const SLOTS = 4
const MIN_TESTS = 788

const SPUREN = [
  { id: "A", thema: "cursor", baum: "r38_cursor",
    punkte: [
      "Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird, hat immer noch so einen komischen Schatten. Den hat er im Original nicht...",
    ],
    hinweise: "'immer noch': der Schatten wurde schon einmal gemeldet — git log --all --oneline | grep -i -E 'cursor|zeiger|schatten' und die Dossiers analysis/befunde_runde34_nacht/B_hebetisch.md, analysis/befunde_runde36/G2_*.md lesen (was wurde damals gemacht, warum hielt es nicht). CODE: ROOM1150 Hebetisch-Cursor engine/src/hebetisch_cursor_1150.c + platform/pc/src/hebetisch_cursor_pc.c; ROOM11F0/1080/10D0/11E0/1230/... Panel-Cursor engine/src/panel_zeiger_common.c (Obj_model_set Typ 4, Band 0, obj+0 0x0101, Runde 36 G2), codepanel_r37.c; Zeichenpfade render_pc.c (Schatten-Layer, Skill re15-pc-render-order), Figurenschatten (memory reai-v2-char-shadow), Prop-Zeichner main.c. SCHRITT 1 MESSEN: Framedumps (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1) am Cursor in 1150 und 11F0 (und einem weiteren Panel-Raum), Schatten im Bild nachweisen und dem Zeichenpfad zuordnen (welcher Aufruf malt ihn: Prop-Schatten, Halbtransparenz, Unterlage, Doppelzeichnung?). Original: DuckStation-Bild desselben Cursors (Skill re15-room-capture, Savestates stage_saves/) — kein Schatten; warum zeichnet das Original keinen (Obj_model_set pc[6..7]-Flags, Installer LAB_80040914 @0x80040990-9a4, Schatten-Routine des Originals und ihr Gate). SCHRITT 2: Ursache beheben (alle Cursor-Raeume), Riegel je Raum (Pixelpruefung: keine Schattenpixel unter/neben dem Cursor, Gegenprobe alter Stand), Panel-/Hebetisch-Riegel gruen." },
  { id: "B", thema: "szenenschutz", baum: "r38_szenenschutz",
    punkte: [
      "Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal bewegen kann ist mist. So wie bei Leon zum Beispiel bei ROOM 1190 bei den Hunden oder 11D0 bei den Hunden. Es sei den es ist Teil der Cutscene wie bei Elza Room 1170.",
    ],
    hinweise: "Faelle: Leon ROOM1190 (Hunde-Szene), ROOM11D0 (Hunde). Gegenbeispiel, das NICHT veraendert werden darf: Elza ROOM1171 (sub07 @0x016C8: John + Kraehen 0x21 in der Szene, der Angriff ist Teil der Szene). Default-KI ist RE2 (enemy_ai_re2_dog.c u.a.); RE1.5-KI per RE15_AI_FLAVOR=re15. SCHRITT 1 MESSEN: an der exe beide Szenen (echter Weg oder Sprung + Szene ausloesen), Spieler-Sperre (Szene fuehrt Leon: Work_set SPIELER/Plc_dest/Plc_motion, Status-Bits (2,0x07)/(1,0x1B), die Szenen-Subs setzen sie z.B. ROOM1141 sub02 @0x00C26/@0x00C2A) und Treffer/HP-Verlust protokollieren (RE15_ENEMY_DBG, RE15_STATE_LOG). SCHRITT 2 ORIGINAL: wie verhindern RE1.5 und RE2, dass Gegner den Spieler waehrend eines Ereignisses angreifen (globale KI-Sperre: memory reai-v2-global-ai-freeze / reai-v2-npc-global-yield; Ereignis-Status-Bits; Gegner-Angriffs-Gate im Overlay/EXE; RE2: FUN_... Angriff nur bei freiem Spieler) — selbst disassemblieren, Adresse ins Dossier; und wie die Ausnahme 1171 im Original entsteht (dort wirkt die Sperre nicht: welches Bit/welcher Zustand). SCHRITT 3: Port an den Original-Mechanismus angleichen (fuer ALLE Gegner und Szenen, nicht nur Hunde), Riegel: 1190/11D0 kein Treffer waehrend der Szene, danach Angriff normal; 1171 unveraendert (Gegenprobe)." },
  { id: "C", thema: "werkstatt11b0", baum: "r38_werkstatt11b0",
    punkte: [
      "Ändere in ROOM 11B0 in der neuen Cutscene von Marvin seinen Text von \"Hold on...\" zu \"But, hold on...\"",
      "In ROOM 11B0 macht das Verschieben des COntainers mit Marvin zusammen in der Cutscene noch keinen Verschiebe Sound",
      "In ROOM 11B0 macht Marvin während/nach dem verschieben des Containers so eine komische Fall Animation, die er im original nicht macht.",
    ],
    hinweise: "Die Szene stammt aus Runde 36 Spur J: Generator tools/r36_j/szene11b0_bauen.py -> engine/src/gen/szl11b0_szene.inc (msg 20 'Hold on.' = NUTZER-VORGABE, Form ROOM11B0 msg 12 @0x1BD0; neuer Wortlaut NUTZER-VORGABE 'But, hold on...'), Dossier analysis/befunde_runde36/J_*.md; Container-Schieben gemeinsam mit Marvin (Szenen-Laeufer grep szl11b0 in engine/src). Sprachdatei: liegt zu msg 20 eine WAV in synchro/STAGE1/room11B0? (synchro/README.md) — Text und Stimme muessen zusammenpassen; im Dossier fuer den Nutzer vermerken, falls die Aufnahme den alten Wortlaut spricht. SCHIEBE-TON: wie toent das Schieben, wenn Leon allein eine Kiste schiebt (Raumbank-Satz des Schiebens, Runde 36 A klettern / objfall_r36.c, Original-Push-Routine mit Se_on) — derselbe Ton fuer das gemeinsame Schieben, Takt wie beim Original-Schieben (Adresse). FALL-ANIMATION: RE15_ANIM_TRACE an Marvin waehrend/nach dem Schieben — welcher Clip (Bank/Nummer) laeuft, warum (Szenen-Programm, Gravitation/Bodenhoehe, Fallzustand der NPC-Lokomotion?); Marvins Original-Clips (EMD) und die Original-Szene, an der sich die Form orientiert. Riegel: Text-Bytes, Ton-Mitschnitt (RE15_AUDIO_CAP_SYNC) beim Schieben, Clip-Folge Marvins ohne Fall-Clip." },
  { id: "D", thema: "zopf", baum: "r38_zopf",
    punkte: [
      "Bei Elza fehlt der Zopf - schaue in Resident Evil 2 wie es da gemacht ist mit Clairs Zopf.",
    ],
    hinweise: "SCHRITT 1 MESSEN: Elzas Modell im Port (Spieler-PLD Elza; Lader/Zeichner: grep -n -i 'pld' engine/src platform/pc/src; Runde 37 Spur N elzaems: Elza laedt Typen aus CDEMD1.EMS) — hat ihr Mesh-Satz einen Zopf/Haar-Teil, der nicht gezeichnet oder nicht animiert wird, oder fehlt er ganz? Framedump von Elza von hinten/seitlich. Gegenprobe: zeigt das ORIGINAL RE1.5 Elza mit Zopf (DuckStation, Skill re15-room-capture; info/Re1.5/PSX/PLD/)? SCHRITT 2 RE2: wie macht RE2 Claires Zopf — Claire-PLD (PL01) mit eigenem Objekt fuer den Zopf + Pendel-/Nachlauf-Logik im Spieler-Code (Leon hat keinen). Pruefe, ob die RE2-Leon-Disc (info/re2leon/) Claires PLD/Code enthaelt; sonst Referenzen unter info/Resident_Evil_und_Playstation_Information/ (BioClone/BioModels/bioclone-remake) und ghidra_re2_Leon.txt (Zopf-Routine: Teil-Matrix aus Geschwindigkeit/Drehung). Jede Konstante mit Adresse. SCHRITT 3 BAUEN: Elzas Zopf nach dem RE2-Mechanismus (Mesh: aus Elzas Original-Daten, wenn vorhanden; sonst Form aus RE2 Claire, als PORT-WAHL kennzeichnen), mit Bewegung beim Laufen/Drehen; beide Szenarien pruefen (Leon unveraendert). Riegel: Mesh vorhanden + gezeichnet (Pixel am Hinterkopf), Pendel reagiert auf Drehung (Winkelverlauf)." },
  { id: "E", thema: "elzaintro", baum: "r38_elzaintro",
    punkte: [
      "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon.",
      "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza",
    ],
    hinweise: "Elzas Vorspann: ROOM1241 (Text 'A bizarre incident...' msg 0-5; sub02 @0x0055A Set(3,0x6F), @0x0055E Set(3,0x70); Tuer Slot 0 -> 1031); Leon: ROOM1240. Leons Intro-Stimme: Runde 10 (memory reai-v2-runde10-arme-1090-stimme 'Intro-Stimme'), Sprachdateien synchro/STAGE1/room1240/ (Regel synchro/README.md: synchro/STAGE<n>/room<RAUM>/main<MSG>.wav) — gleiche Stimme fuer Elza: dieselben WAVs fuer ROOM1241, wenn Text und Message-Ids gleich sind (Bytes von 1240/1241 msg 0-5 vergleichen); sonst Zuordnung je Zeile. Pre-Intro: OPENING.BIN/Pre-Intro nach RE2 (memory reai-v2-preintro-re2, re15_gameflow.c, re15_montage_fx.c), Schwarz-/Fade-Layer render_pc.c (memory reai-v2-frontend-fade-leck 'Fade-Leck', Skill re15-pc-render-order). SCHRITT 1 MESSEN (mit Elza, RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, Neues Spiel): Ton-Mitschnitt (RE15_AUDIO_CAP_SYNC) Leon vs. Elza; Framedumps des Pre-Intro-Texts bei Elza (Raumhintergrund sichtbar? ab welchem Bild, welche Ebene) gegen Leon. SCHRITT 2: Ursache je Befund (warum laedt Elza keine Stimme; warum liegt bei Elza der Raum hinter dem Schwarz) — Fix, der den Befund erklaert. Riegel: Elza-Intro spielt die Stimmzeilen (Mitschnitt), Pre-Intro-Bilder bei Elza schwarz hinter dem Text (Pixelzaehlung), Leon unveraendert." },
  { id: "F", thema: "dining1141", baum: "r38_dining1141",
    punkte: [
      "Betreff des Logiklochs mit John in Room 1141, das klingt doch gut. Als Hinweis machen wir: „There's an empty key tag on his belt. / It reads \"Dining Room\".“ und das \"Help\" ist einfach Teil der Audio files, genau wie das klopfen. Das soll nicht als Text erscheinen.",
      "(Runde 37 Nachtrag 14, abgestimmt, woertlich in analysis/befunde_runde37/AUFTRAG.md) Der Raum 1141 ist vorher abgeschlossen, Roy hat den Schlüssel. Nachdem er zum Zombie geworden ist, hat er den Schlüssel an der Stelle gedropped, wo er vorher am Boden lag in ROOM 1011. Und wenn Elza wieder aus dem Raum raus kommt, ist John weg. — ohne das John, wait und das Sorry.... Einfach nur direkt - are you okay? what's happened? Und du musst einen für den Dining Room passenden Schlüssel finden. Und wir müssten irgendwie beim toten Roy irgendeine Art hinweis hinterlassen, das er den dining key hatte und der im 1011 gedropped gewesen sein könnte. Dafür müsste dann immer wieder ein panisches Klopfen/Hämmern und ein help ein regelmäßigen abständen, bis man den geheimen Schalter gefunden hat um die Tür zu öffnen, mit panischer Musik. Wenn Die Monkeys da sind, und Elza eine Tür anklickt, muss, so lange die Monkeys noch nicht tot sind kommen \"I cannot leave him alone with those monsters\".",
      "In Room 1140 hat das Rolltor keinen Sound beim hochfahren.",
    ],
    hinweise: "LIES ZUERST analysis/befunde_runde37/AUFTRAG.md Abschnitt 'Nachtrag 14' (Wortlaut + abgestimmter Ablauf 1-6) und analysis/story_leon_elza/ (ablauf_elza.md E13-E17, B_figuren.md 1.1/1.2, scd_annot/ROOM1021.txt, ROOM1141.txt, ROOM1171.txt, ROOM1011.txt; texte_elza.md). ABLAUF (nur Elza-RDTs, Leon unberuehrt): (1) ROOM1021: Roys Leiche (sub06 msg 9 'He's dead. Poor Roy...', Leiche main00 @0x01E40 bei (3,0x5F)) bekommt den Hinweis 'There's an empty key tag on his belt. / It reads \"Dining Room\".' (NUTZER-VORGABE, woertlich). (2) ROOM1011 (einzige Tuer -> 1021): der Schluessel liegt nach der 1021-Szene sichtbar an Roys Liegestelle (Roy-Spawn sub00 @0x009F6 (-2000,0,7450)); John ist zurueck in 1021 weg (schon Original: Spawn nur bei (4,0x60)==1, die Szene loescht das Flag @0x01FD8). (3) ROOM1131 -> 1141 (Tuer slot 0 @0x0088E) verschlossen bis zum Schluessel: Name 'Dining Room Key', Modell/Icon/Bild = RE2 Precinct Key 0x5B (Heart Key, mesh066; analysis/befunde_runde36/Q_belege/q_re2_schluessel_modelle.png 'Precinct 5B'; im Port unbenutzt), Tuertext nach RE2-Muster Text 0x13 @0x8009F2BB: 'It's locked. The door is marked \"Dining Room\".', Benutzen/Aufschliessen/Wegwerf-Ablauf wie schluessel_r36.c (Parking Lot Key, Muster Runde 36 Spur Q: Prop, Zone, Icon, Bild, CHECK-Text, RE2-Toene). (4) ROOM1141 nach dem Aufschliessen: John sitzt auf dem Dach fest (hochgekommen ueber die Flurtuer 1131 -> 1171, @0x008EE, immer offen). In regelmaessigen Abstaenden panisches Klopfen/Haemmern + 'Help' als TON (KEIN Text; Quellen: RE2_SOUND_LISTE.md, voice/Kendo (RE2-Kendo-Aufnahmen; John traegt Kendos Modell), RE2-Raumbaenke; Wahl im Dossier begruenden), dazu panische Musik (Original startet in 1141 die Gorilla-Kampfmusik main00 @0x00B4E Sce_bgm_control, Stopp sub01 @0x00C06; Titelwahl begruenden), bis Elza den GEHEIMEN SCHALTER findet: das Gemaelde 'A beautiful painting. There's a switch below.' (sub06 @0x00FE8, hebt den Notrolladen Prop obj 0 vor dem Dachgang, (3,0x6A)), danach der Riegel (sub05 @0x00FD2 'You've opened the lock.') -> John stuerzt herein. (5) Szene sub02 OHNE msg 0 'John, wait!' und msg 1 '...sorry, I'm glad to see you're still alive...': John stuerzt herein, Elza direkt 'Are you okay? What's happened?' (msg 2 ohne 'Same here...'), Rest wie Original (Dach-Warnung, Gorillas durch die Fenster, 'I'll try to distract them...'). (6) Solange die Gorillas leben ((4,0xF2)==1; sub01 loescht es erst, wenn (7,0xCB) und (7,0xCC) gesetzt sind, @0x00BF2-0x00C0C): jede Tuer in 1141 -> 'I cannot leave him alone with those monsters.' (NUTZER-VORGABE). Danach laeuft John sichtbar durch die Dachtuer voraus (1171 beginnt mit John oben, sub07 @0x016C8) — kein Sprung. ROLLTOR-TON ROOM1140 (Leon): derselbe Gemaelde-Schalter-Rolladen (Leons 1140-Sub selbst finden); das Original hat dort KEIN Se_on, ROOM1131 sub02 hebt seinen Rolladen MIT Se_on(2,0x0C) @0x00A48, Se_on(2,0x0A) @0x00A58, Se_on(2,0x0B) @0x00AB2 — dieselbe Ton-Form fuer 1140 und 1141 (Beta->Retail, Beleg 1131). RESSOURCEN (VERTRAG 1.1-1.3): Bank-9-Bits 64, 66, 67; Port-Ids 1011 45-49, 1021 40-44, 1131 40-44, 1141 40-49; freie Item-Id per Zensus. Texte ueber einen Text-Generator wie tools/r35_l/texte_bauen.py (Glyphen msg_common.c:175-199). Riegel: der ECHTE Weg 1021 -> 1011 -> 1021 -> 1131 -> 1141 -> 1171 mit Elza (RE15_WEGPLAN), Tuer zu ohne Schluessel, Haemmern/Help/Musik im Ton-Mitschnitt bis zum Schalter, Szene ohne msg 0/1, Tuer-Sperre bis die Gorillas tot sind, Uebergang nach 1171 lauffaehig; Leon 1140 Rolltor-Ton im Mitschnitt." },
  { id: "G", thema: "tagebuch", baum: "r38_tagebuch",
    punkte: [
      "Ich möchte das du mir in Elzas Szenario für Roy ein Tagebuch schreibst. Hintergrund könnte FILE16_title_paper.png sein. Muss nicht unbedingt ein Welt Model haben, das taucht einfach auf, wenn man Roy, der am Boden liegt noch einmal anklickt im Room 1011. Das Tagebuch soll kommen statt der Code Message. Und es soll folgendes beinhalten: Header: Roys Diary — Text: siehe AUFTRAG.md (woertlich, mit Absaetzen; September 22nd ... September 28th ... code: 0513 ... I just hope he's okay...). Den Code bitte wieder grün hervorheben, wie alle Codes in den Dokumenten",
      "Und danach, auf der Karte einmal den Communications Room markieren und einmal den Prison Room, so wie auch schon bei Leon im Communications Room gemacht.",
    ],
    hinweise: "TEXT: woertlich aus analysis/befunde_runde38/AUFTRAG.md (Header 'Roys Diary'; Absaetze, Apostrophe wie im Wortlaut — Glyphen pruefen, Typografie-Apostroph ’ ggf. auf das Spiel-Apostroph abbilden und im Dossier nennen). ORT: ROOM1011 Roy liegt verwundet (Spawn sub00 @0x009F6 (-2000,0,7450)); zweites Untersuchen = AOT Slot 5 -> sub07 msg 18 'He has passed out from his injuries.He is holding a slip.The numbers 0513 ...' (Code-Nachricht) — STATT dieser Nachricht oeffnet das Tagebuch (Runde 36 hat den Prison Key aus Roy herausgenommen: schluessel_r36.c, Prison Key in ROOM10F1; Zustand (4,0xFA)/(3,0x78) pruefen). DOKUMENT-SYSTEM: dokumente_r34.c (Runde 34 Nacht: 4 Dokumente), irons_tisch_1150.c (Irons Diary, Runde 30), Dokument-Hintergruende RE2 shared_assets/RE2/FILES/FILE16_title_paper.TIM (extracted_re2_dokumente/hintergruende/), Code-Hervorhebung GRUEN wie in den vorhandenen Dokumenten (grep -n -i 'gruen\\|green\\|farbe' in den Dokument-Quellen; z.B. 4312/5632-Zettel). Soll es in die Akten (FILES) aufgenommen werden wie in RE2 gelesene Dokumente? RE2-Verhalten belegen und so bauen. KARTE: Leon hat eine Markierung des Communications Room (grep -n -i 'communic\\|10F0\\|markier' in den Karten-Quellen, Runde 33/36 Karten-Arbeit, memory reai-v2-karte-*); fuer Elza Communications Room (ROOM10F1) UND Prison Room (Zellen B2: ROOM1211 Detention Area / Zelle, in der John sitzt) markieren, Zeitpunkt der Markierung = nach dem Lesen des Tagebuchs (Form wie Leons Markierung). RESSOURCEN: Bank-9-Bit 68 (falls ein Lese-/Aufnahme-Bit noetig), Port-Ids 1011 40-44, Dokument-Id per Zensus. Riegel: Klick auf den liegenden Roy (zweites Mal) -> Tagebuch statt msg 18, Text-Bytes woertlich, Code gruen (Pixel), Karte zeigt beide Markierungen bei Elza (FB_SHOT/Kartenbild), Leon unveraendert." },
  { id: "H", thema: "elzatoene", baum: "r38_elzatoene",
    punkte: [
      "In Elzas Szenario im ROOM 1040 bei *Gunshot* fehlt der Sound für einen Gunshot.",
      "Wenn Roy in der Cutscene in Room 1020 auf John zuläuft soll er schon irgendwie ein Zombie stöhnen machen.",
      "Wenn John dann in der Cutscene in Room 1020 auf Roy schießt, muss auch ein Schusssound kommen",
    ],
    hinweise: "Elza-Raeume: ROOM1041 und ROOM1021. 1041: sub11 @0x021EA..0x0223C (AOT Slot 8 auto bei Ck(4,0xF1)==0 & Ck(3,0xAD)==0): @0x02200 Message_on 3 = Platzhaltertext 'gunshot' (RE1.5 hat keinen Skript-Schuss-Ton), dann Plc_dest-Drehung + msg 2 'Elza: What was that!?'. Lesart (wie beim 'Help' der Spur F: Ton statt Text): der Platzhalter wird zum SCHUSS-TON, das Textfenster entfaellt; Ton-Wahl wie Runde 36 Spur L P4 (analysis/befunde_runde36/L_elza.md: RE2 ROOM40A0 sub04 Se_on(2,8) Skript-Pistolenschuss, EDT @0x09E00, VAG @0x17B80). 1021: engine/src/elza_r36.c baut seit Runde 36 P4 (Schuesse @0x020CE/@0x020D6, RE15_ELZA_AKT_SCHUSS) und P5 (Roys Sterbe-Ton) — der Nutzer HOERT aber keinen Schuss: ZUERST MESSEN, warum (Haken greift nicht mehr? Bytes durch Runde 37 K/N verschoben? Ton nicht geladen/stumm? Mitschnitt RE15_AUDIO_CAP_SYNC + RE15_SE_DEBUG am echten Weg mit Elza); Bisect gegen v0.8.24/v0.8.25, Ursache benennen. Roys Stoehnen beim Zugehen (sub02, Roy Plc_dest @0x020B2 -> (-4274,-6590)): Zombie-Toene = RE1.5-Raumbank snd1 (NUTZER-MANDAT 2026-08-23, enemy_ai_re2_zombie.c:766); Stoehn-Satz und Takt wie ein gehender Zombie im Original (Adresse). Riegel: Mitschnitt je Ton (Zeitpunkt zur Szene), Textfenster 'gunshot' entfaellt, Leon unveraendert." },
  { id: "I", thema: "heliport1171", baum: "r38_heliport1171",
    punkte: [
      "Elza stuck in ihrer Cutscene im ROOM 1170.",
    ],
    hinweise: "Elzas Heliport ROOM1171 (Leon: ROOM1170). Szene: sub07 @0x016C4 Ck(3,0xC2)==1 -> Spawn John 0x49 @0x016C8 + Kraehen 0x21 + @0x01704 Evt_exec sub02; sub02 @0x01398..: Set (3,0xC2)=0, (3,0x80)=1, (4,0xE9)=1, (3,0x82)=1, Pos_set, Plc_dest, While Ck(5,0x1C)==0 @0x013F4-0x013FE (wartet auf ein Bank-5-Bit, das das gemeinsame Schieben sub03/sub04 setzen muss), Messages 'Elza, hurry up!' / 'We need to get this thing off of here! Help me out!' / 'C'mon, give me a hand!' / 'Alright, let's get down from here!' (texte_elza.md ROOM1171). Neue Nutzer-WAVs synchro/STAGE1/room1171/main00-02.wav. SCHRITT 1 REPRODUZIEREN am ECHTEN Weg mit Elza (Spielstand/Wegplan bis 1141 -> Gorillas -> 1171; analysis/story_leon_elza/scd_annot/ROOM1171.txt), wo genau die Szene haengt (RE15_SCD_TRACE/RE15_EVT_TRACE: welcher Thread wartet auf welches Bit; Spieler-Eingabe frei?). SCHRITT 2: wer setzt im Original das erwartete Bit (Schiebe-Mechanik, AOT sce 12/13 Slots 6-9 @0x01310-0x0134C, Objekt-Schieben/Kollision, Runde 36 A klettern/Schieben), warum nicht im Port. Fix, der den Haenger erklaert; Riegel: Szene laeuft bis 'let's get down from here' und der Abstieg Slot 2 -> 10B1 (@0x0126C) wird frei; Leon 1170 unveraendert. Spur F aendert den Uebergang 1141 -> 1171 (John laeuft vorher aufs Dach) — miss am heutigen Ablauf, halte den Fix unabhaengig davon." },
  { id: "J", thema: "rekorder", baum: "r38_rekorder",
    punkte: [
      "Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner, und nachdem er fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound. Das zusammen mit der erfolgreich message auf den Panel.",
    ],
    hinweise: "Ton-Datei des Nutzers: synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav (liegt NICHT im main<ID>-Schema der Auto-Einbindung, synchro/README.md — eigene Einbindung noetig; Paket-Gate fuer neue Assets im Dossier fuer den Orchestrator nennen). Raetsel: Stimmschloss ROOM1100/1101 (Minidisc Player w/ Disc, Item 0x44; Original sub00 @0x00AC8 Ck(3,0x67)==0, sub02 @0x00C68 Set(4,0xE8)=1, @0x00C90 Message_on 4 'You've used the Minidisc Player w/ Disc.'; Runde 37 Spur Q kistenregel K6; Runde 37 Spur E codepanel: Panel-Licht blau/gruen, Piep + Aufschliessen, Kamera auf dem Panel — engine/src/codepanel_r37.c, include/re15_codepanel_r37.h, analysis/befunde_runde37/E_codepanel.md; aufschliessen_r36.c Folgen KARTE/KARTE_AUF/RIEGEL mit Saetzen Piep/Aufschliessen). SCHRITT 1 MESSEN: heutiger Ablauf beim Benutzen des Rekorders (Mitschnitt RE15_AUDIO_CAP_SYNC, Framedumps Panel). SCHRITT 2 BAUEN (NUTZER-VORGABE, Form aus E/aufschliessen): beim Reinsprechen die WAV abspielen; ERST nach ihrem Ende Piep (erfolgreiche Eingabe) + Tuer-oeffnen-Ton (dieselben Saetze wie die Kartenleser/Code-Panels, Satz-Nummern belegen) zusammen mit der Erfolgsmeldung und dem gruenen Panel-Licht. Riegel: Mitschnitt-Reihenfolge WAV -> Piep -> Tuer, Meldung/Licht im selben Takt, Tuer offen." },
  { id: "K", thema: "itemorte", baum: "r38_itemorte",
    punkte: [
      "Ich hätte den fire extinguisher gerne in room 1030 hängen an der stelle von fire_extinguisher.bmp, anstatt in room 1050.",
      "Ich möchte das du den Parking Lot key, und den Prison Key - je nach Szenario - gut sichtbar an die Location platzierst, die ich im Bild key_location.bmp markiert habe.",
    ],
    hinweise: "NUTZER-BILDER in deinem Baum: analysis/befunde_runde38/nutzer/fire_extinguisher.bmp und analysis/befunde_runde38/nutzer/key_location.bmp (320x240, rotes Rechteck = Ort). Erst Raum + Kamera-Schnitt bestimmen (Bild gegen die Hintergruende pruefen, z.B. build/blaetter/ROOMxxxx_cc.png im HAUPTBAUM C:/workspace/git/reAi_v2/build/blaetter/ nur lesen, oder BSS-Schnitte selbst rendern): fire_extinguisher = Lobby ROOM1030 (Rolltor im Hintergrund, Marke links an Wand/Saeule), key_location = Buero mit Pinnwand (Marke auf der Pinnwand; Raum belegen — vermutlich Communications Room 10F0/10F1, wo die Schluessel seit Runde 36 liegen). Position im Raum ueber Rueckprojektion aus dem Kamera-Schnitt (memory reai-v2-modellmass-aus-kamera, Lagemass nicht selbstbestaetigend: reai-v2-lagemass-selbstbestaetigend); 'haengen' = an der Wand/Pinnwand in der Hoehe der Marke, gut sichtbar, vom Boden aus erreichbar (Item-Zone, Band). CODE: Feuerloescher engine/src/feuerloescher_r37.c (Runde 37 Spur I: Modell, Fundort, Leer-am-Boden 1090/1091, Taken (9,0x86), Story (3,0x85); analysis/befunde_runde37/I_feuerloescher.md) + kistenregel_r37.c K1 (Bit (3,0x85) Port-Aufnahme); Schluessel engine/src/schluessel_r36.c (Runde 36 Spur Q, Prop + Zone in 10F0/10F1 (8350,2050), Bits (9,82)/(9,83), analysis/befunde_runde36/Q_schluessel.md). Elza: gilt der Loescher-Ort auch fuer ROOM1031 (Elzas Loescher-Weg 1091 pruefen)? Beide Szenarien messen. Riegel: Framedump je Ort mit sichtbarem Modell an der Marke (Pixel im Rechteck), Aufnahme am Ort, alte Orte leer, Loesch-Ablauf 1090/1091 und Schluessel-Ablaeufe unveraendert." },
  { id: "M", thema: "npckante", baum: "r38_npckante",
    punkte: [
      "Kendo springt am Ende von ROOM 10B0 nicht die Kante runter, wie im Original, sondern läuft in die Luft weiter",
      "Bei Room 1260 Springt Kendo auch nicht die Kanten runter. Machen Marvin und Ada in Leons Szenario übrigens auch immer nicht. Das ist falsch.",
    ],
    hinweise: "Kendo = John (Typ 0x49, Kendo-Modell) in Elzas Szenario: ROOM10B1 Ende (sub04 msg 0-5, @0x0195A..; John geht zum Kanaldeckel, Tuer Slot 1 -> 1261 @0x016F2), ROOM1261 Sewer Passage (John je Eintrittscut sub03 @0x01276/@0x01296/@0x012B2 bei (3,0x41)). Leon: Marvin und Ada in ROOM1260 und wo sie sonst Hoehenkanten queren. SCHRITT 1 MESSEN: an der exe NPC-Bahn + Hoehe (y) + Clip (RE15_ANIM_TRACE, RE15_STATE_LOG) an jeder Kante — laufen sie waagrecht in die Luft und 'fallen' dann? SCHRITT 2 ORIGINAL: wie steigt ein NPC im Original eine Kante hinunter (Sprung-/Fall-Clip im NPC-EMD, Bodenhoehen-Wechsel der SCA-Zelle, NPC-Lokomotion im Overlay/EXE — Skills re15-enemy-ai-re/re15-psx-disasm; Vergleich RE2: Ada/Ben/Sherry/Kendo in der Kanalisation; DuckStation-Messung an derselben Kante). CODE: engine/src/actor_locomotion.c, NPC-/Szenen-NPC-System (memory reai-v2-cutscene-npc-system, reai-v2-npc-ai, reai-v2-npc-global-yield, reai-v2-npc-crossfade-decay), Spieler-Abstieg zum Vergleich (climb_common.c). Riegel je Kante (10B1, 1261, 1260 Marvin/Ada): NPC-Hoehe folgt dem Boden mit Sprung-/Fall-Clip, kein Gehen in der Luft." },
  { id: "N", thema: "wasser", baum: "r38_wasser",
    punkte: [
      "In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im flachen Wasser bewegen.",
    ],
    hinweise: "Raeume: ROOM1260/1261 und die Kanalisation der Stage 2 mit flachem Wasser (Raumliste per SCA-Bodentyp-Zensus belegen). SCHRITT 1 ORIGINAL: wie erzeugt RE1.5 den Wasser-Effekt beim Gehen (Bodentyp/Floor-Attribut der SCA-Zelle oder Raumvariable -> ESP-Effekt (Spritzer/Ring) + Schritt-Ton 'Wasser'; Spieler-Schritt-Routine mit Bodentyp; Stage-2-Overlay) — selbst disassemblieren, DuckStation-Gegenprobe (Bild + Ton in 1260). Beta->Retail: wo RE1.5 nichts hat, RE2-Kanalisation (gleicher Mechanismus, RE2 ROOM20xx) als Ziel. CODE: engine/src/re15_esp.c (ESP-VM, memory reai-v2-esp-vm, reai-v2-esp-rowmachine), re2_fx.c, Schritt-Ton-/Bodentyp-Code (grep -n -i 'schritt\\|footstep\\|floor_se'), game_step_common.c. Fuer Leon UND Elza. Riegel: im Wasser je Schritt Effekt (FX-Log RE15_FX_LOG + Framedump) und Wasser-Ton (Mitschnitt), auf trockenem Boden nicht." },
  { id: "O", thema: "montage", baum: "r38_montage",
    punkte: [
      "Bei Leon - bei der Cutscene nach Irons Tod, wo die Zombies durchbrechen, sollen sich die Zombies jeweils auch Richtung Kamera bewegen, also so, wie es bei den Zombies hinter dem Rolltor schon passiert. Die anderen sollen das auch machen. Das wirkt auch bedrohlicher.",
    ],
    hinweise: "Montage nach Irons' Tod (Leon): engine/src/irons_tod_1150.c (Programme der Montage, Runde 35 L), Fenster ROOM1120 (fenster_1120.c, Runde 35 M), Lobby-Rolltor ROOM1030 (dort laufen die Zombies hinter dem Rolltor schon Richtung Kamera — MECHANISMUS dort messen und als Vorbild nehmen), Runde 37 Spur H zombieaktiv (zombieaktiv_r37.c, analysis/befunde_runde37/H_zombieaktiv.md: Montage-Zombies im RE2-Gang). NUTZER-VORGABE (RE1.5 hat diese Montage nicht): in JEDEM Montage-Schnitt, in dem Zombies durchbrechen, gehen sie auf die Kamera zu (Ziel = Kamerastandort des Schnitts auf den Boden projiziert, Form wie beim Rolltor; Kamera aus dem RDT-Schnitt belegen). SCHRITT 1: Zensus aller Montage-Schnitte mit Zombies (Raum, Cut, Zombies, heutige Richtung), Framedumps. SCHRITT 2 BAUEN. Riegel: je Schnitt Abstand Zombie->Kamera nimmt ueber die Schnittdauer ab (alle Zombies), Rolltor-Schnitt unveraendert." },
  { id: "Q", thema: "kleinkram", baum: "r38_kleinkram",
    punkte: [
      "Das öffnen des Player Inventories hat keinen Sounds, wenn Resident Evil 2 dort Sound hat, möchte ich auch Sound haben.",
      "In Elzas Szenario bei ROOM 1151 kommt - wenn man Irons anklickt erst - he already passed - und dann beugt sie sich hin und es kommt \"I'll be fine, just worry About yourself for now\". Ist natürlich quatsch … sie soll sich nur hinbeugen, und es soll stehen \"he already passed\"...",
      "In Room 30E0 soll beim aufnehmen des Flammenwerfers nicht mehr kommen - not available in the preview, sondern einfach der Flammenwerfer aufgenommen werden.",
    ],
    hinweise: "(1) INVENTAR-TON: RE2 Status-Bildschirm oeffnen — spielt RE2 dabei einen Satz (RE2-EXE: Oeffnen des Status-Schirms, Se-Aufruf; RE2_SOUND_LISTE.md CORE-Baenke; ghidra_re2_Leon.txt)? Wenn ja, denselben Ton beim Oeffnen (und, falls RE2, beim Schliessen) im Port (engine/src/re15_inv_screen.c, inventory_common.c, menu_common.c, audio_pc.c); wenn RE2 stumm ist, belegen und nichts bauen. (2) ROOM1151 Elza: Runde 36 Spur L 'Irons tot' (analysis/befunde_runde36/L_elza.md; Original sub01 @0x00E9E-0x00EAC Cmp(var00==2) & Ck(3,0x9D)==0 -> sub03 msg 2 'Irons: I'll be fine...'; AOT Slot 2 sce5 @0x00D92) — heute erscheint erst 'He has already passed...' und danach laeuft die Original-Szene mit msg 2. Soll: nur Hinbeugen + 'He has already passed...' (NUTZER-VORGABE), keine msg 2. (3) ROOM30E0 (und Elza 30E1): Flammenwerfer-Aufnahme zeigt den Preview-Text 'not available in this preview' (RE1.5-Sperre im SCD des Raums: Stelle selbst finden; Item-Id Flammenwerfer, 2-Feld-Waffe) — Soll: normale Aufnahme wie jede Waffe (Aufnahme-Modal, Inventar, Taken-Bit des Originals). Riegel je Punkt (Mitschnitt, Text-Folge, Inventar nach Aufnahme)." },
  { id: "R", thema: "spinnen", baum: "r38_spinnen",
    punkte: [
      "In Stage 2 die Spinnen die fliegen fast alle in der Luft und kommen teilweise nicht aus den Schächten raus wie im Original",
      "In Stage 2 die Spinnen verschießen kein Gift wie im Original, was dumm ist - denn das Gift ist das einzige das die Charaktere vergiften kann und damit der einzige Grund für das Gegengift Item",
      "In Stage 2 die Minispinnen, die rauskommen, wenn man eine Spinne getötet hat fliegen weitestgehend in der Luft und greifen einen nicht an wie im Original",
    ],
    hinweise: "Stage-2-Spinnen: Typen per Zensus der Stage-2-RDTs (Sce_em_set) belegen (memory reai-v2-spider-ai: '⛔ 0x26 = FEUER' — Typnummern NICHT raten), Minispinnen-Brut nach dem Tod. KI: engine/src/enemy_ai_re2_spider.c (RE2-KI, Default) + RE1.5-KI (RE15_AI_FLAVOR=re15); Original: Stage-2-Overlay RE_15_Quellcode_Overlays/STAGE2*/ (Handler ueber die Dispatch-Tabelle @0x80072bac), RE2-Spinne (RE2_Quellcode_Overlays, ENEMY11 'Spider sounds' in RE2_SOUND_LISTE.md). SCHRITT 1 MESSEN je Raum: y-Lage (Decke/Wand/Boden), Schacht-Austritt, Angriffe, Gift (Status-Bit Vergiftung des Spielers, Gegengift-Item) — Port vs. Original (DuckStation). SCHRITT 2: (a) Lage/Austritt wie Original (Wand-/Deckenlauf, Bodenhoehe aus der SCA), (b) Giftspucken (Projektil, Treffer -> Vergiftung wie Original/RE2, Gegengift heilt), (c) Minispinnen am Boden mit Angriff. Skill re15-enemy-ai-re. Riegel je Teilpunkt." },
  { id: "S", thema: "affen3", baum: "r38_affen3",
    punkte: [
      "In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts",
    ],
    hinweise: "Stage-3-'kleine Affen': Typ per Zensus der Stage-3-RDTs belegen (nicht die Gorillas 0x27 aus Stage 1). Vorarbeit: Runde 35 Spur J 'affen' (analysis/befunde_runde35/J_*.md, memory reai-v2-runde35-aufbau), affen_11c0.c ist die Gorilla-Szene. SCHRITT 1 MESSEN an der exe: Angriff laeuft (Clip), aber kein Treffer/HP-Verlust — wo bricht die Kette ab (Treffer-Box/Reichweite, Schaden-Eintrag FUN_80012d60 Spieler-Damage, Gate)? SCHRITT 2 ORIGINAL: Angriffs-/Treffer-Routine des Overlays (Adressen), RE2-KI-Variante falls Default. Riegel: Affe greift an -> Spieler verliert HP (Werte wie Original), beide KI-Varianten." },
  { id: "T", thema: "spiderman", baum: "r38_spiderman",
    punkte: [
      "In Stage 4 die Spiderman kommen nicht richtig aus den Schächten raus wie im Original, obwohl die Schächte korrekt hoch gehen",
      "In Stage 4 die Spiderman führen - mal wieder - dazu das Leon teilweise komplett von der Karte verschwindet, wenn sie einen Angreifen. Sowas darf GARNICHT passieren, bei keinen Gegner.",
    ],
    hinweise: "'Spiderman' = der Stage-4-Gegner, der aus Schaechten kommt (Typ per Zensus der Stage-4-RDTs belegen). 'mal wieder': fruehere Faelle, in denen der Spieler bei Gegnerangriffen von der Karte verschwand, im git log und in den Memory-Notizen suchen (z.B. memory reai-v2-runde16-zwoelf-befunde 'Teleport war SONDEN-Artefakt', Griff-/Wurf-Positionen) — was wurde damals gefixt, warum reicht es nicht. SCHRITT 1 REPRODUZIEREN: Stage 4 Schacht-Raeume, Spielerposition je Bild waehrend eines Angriffs (RE15_STATE_LOG), Moment des Verschwindens, schreibende Stelle. SCHRITT 2 ORIGINAL: Austritt aus dem Schacht (Animation/Hoehe/Schacht-Objekt) und Angriff (Griff setzt Spieler relativ zum Gegner: Original-Formel mit Adresse). SCHRITT 3 GENERELL: Zensus ALLER Stellen im Gegner-Code, die die Spielerposition schreiben (Griffe, Wuerfe, Ausschiebung, Rueckstoss) fuer alle Gegner; Invariante 'Spieler bleibt auf begehbarem Boden innerhalb des Raums' — an der Ursache korrigieren (Original-Formel), zusaetzlich ein Riegel (Sonde ueber alle Gegnertypen/Griffe). enemy_ai_common.c ist geteilt (Spur B): nur kleine Haken." },
  { id: "U", thema: "wasserzombie", baum: "r38_wasserzombie",
    punkte: [
      "Die Zombies in Stage2 die sich unten im Wasser befinden und zum Rand des Wassers laufen müssen sich an den Rand hochziehen können, wie im Original Resident Evil 1.5",
    ],
    hinweise: "Stage-2-Raeume mit Zombies im Wasser (Becken/Kanal): Zensus der Spawns mit Bodenhoehe unter dem Rand. SCHRITT 1 MESSEN: Port — laufen sie gegen die Kante, bleiben sie stehen? Original RE1.5 (DuckStation): Hochzieh-Animation (Clip im Zombie-EMD), Ausloeser (Kante/Hoehendifferenz, Spielernaehe) im Overlay — selbst disassemblieren. Nutzer verlangt RE1.5-Verhalten (nicht RE2). Default-KI ist RE2 (enemy_ai_re2_zombie.c) — die Datei teilt die Runde-37-Spur L: Logik in NEUER Datei engine/src/wasserzombie_r38.c, in enemy_ai_re2_zombie.c nur 1-5-Zeilen-Haken. Riegel: Zombie im Wasser erreicht den Rand und steht danach oben (Hoehe = Randhoehe, Clip-Folge), beide KI-Varianten." },
  { id: "V", thema: "releaseskript", baum: "r38_releaseskript",
    punkte: [
      "Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.",
    ],
    hinweise: "Heutiger Ablauf (Runde 37 UEBERGABE §4 und Eintrag 2026-10-14 10:40): bash release/build_win_cross.sh; bash release/build_linux_deck.sh (Linux-Container, volle Suite ~3 h; bei Rot Portabilitaet, zweiter Lauf RE15_CTEST_JOBS=3); bash release/build_android.sh --version vX; bash release/make_package.sh --version vX (baut NICHT, packt; Gates: Paket-Plattformen, APK-Asset-Gate, SHA256SUMS); Archiv C:/workspace/Re15Data/re15_packages_archiv/vX (Kopie aller Volumes + APK + SHA256SUMS*.txt, sha256sum -c), alte Version im Archiv entfernen; Docker Desktop muss laufen (start, warten bis docker info). FALLEN (memory reai-v2-releasebau-pipe-schluckt-fehler, reai-v2-lokalbau-ist-kein-release, reai-v2-sonde-luegt-paket-luegt, reai-v2-paket-plattform-kollateral 'nie --only win', reai-v2-android-glob-cache): Fehler nie verschlucken (set -o pipefail, Rueckgabe je Schritt pruefen, Artefakt-Zeitstempel/Groesse gegen den Laufbeginn), altes Artefakt darf nicht als neues durchgehen. BAUEN: release/release_alles.sh (Bash) + release/release_alles.cmd (Doppelklick unter Windows -> Git-Bash) — Version: Standard = letzter Tag vX.Y.Z -> Z+1 (Z=99 -> Y+1.0, wie v0.7.99 -> v0.8.0), oder explizit --version; Schritte: Vorbedingungen (sauberer Baum fuer re15_port/, Docker, Platz), Windows, Linux/Deck, Android, Paket, Archiv + Pruefung, alte Archivversion entfernen; je Schritt Protokoll + klarer Abbruch; --dry-run zeigt alles ohne zu bauen; Release-Commit/Tag/Push NICHT automatisch ohne --tag (Release-Notes schreibt der Mensch/Orchestrator). Tests: Versionslogik (Unit, Grenzfaelle), Trockenlauf, Fehlerpfade (simulierter Schritt-Fehler bricht ab). KEIN echter Release-Lauf durch die Spur (dauert Stunden, der Orchestrator faehrt v0.8.26 mit dem Skript). Kurze Anleitung in release/README oder im Kopf des Skripts. Diese Spur darf als einzige release/ aendern." },
]

const REGELN = (s) => `
## Arbeitsbaum und Grenzen
- Dein Baum: ${ROOT}/.claude/worktrees/${s.baum} (Zweig r38/${s.thema}, Basis ${BASIS}). Zuerst: git -C "${ROOT}/.claude/worktrees/${s.baum}" status --porcelain (leer) und git log --oneline -1 (= ${BASIS} oder ein Merge von master in den Zweig; Fortsetzungs-Baeume tragen zusaetzlich die Vorgaenger-Commits).
- ALLE Lese-/Schreibzugriffe auf Projektdateien und ALLE git-Befehle NUR in diesem Baum (absolute Pfade mit dem Praefix ${ROOT}/.claude/worktrees/${s.baum}/ bzw. git -C "<baum>"; in Bash jeden Befehl mit cd "<baum>" beginnen). NIE Dateien unter ${ROOT}/ ausserhalb deines Baums aendern. Aus dem Hauptbaum nur LESEN, was der Baum nicht traegt: ${ROOT}/info/Resident_Evil_und_Playstation_Information/ und ${ROOT}/.agent_refs/.
- Pflichtlektuere ZUERST, vollstaendig: <baum>/analysis/befunde_runde38/AUFTRAG.md und VERTRAG.md (Zuteilung VERBINDLICH; nimm nur, was Spur ${s.id} zugeteilt ist). Vorrunde: <baum>/analysis/befunde_runde37/<SPUR>_*.md (Dossiers + Abnahmeberichte der Module, die du anfasst).
- Dossier <baum>/analysis/befunde_runde38/${s.id}_${s.thema}.md als ERSTES anlegen und FORTLAUFEND schreiben; nach JEDEM abgeschlossenen Schritt committen (wip(r38-${s.thema}): ... ausdruecklich erwuenscht, auch VOR Bau/Suite). Das Sitzungslimit beendet alle Agenten gleichzeitig — was nicht im Commit steht, ist verloren. Nur eigene Pfade adden (NIE -A), Rohbytes/Backticks per git commit -F <datei>.

## RE-Disziplin (CLAUDE.md STOP-GATE, Kurzfassung)
- Je Punkt: MESSEN/REPRODUZIEREN am gebauten Stand -> ORIGINAL disassemblieren (RE1.5; wo RE1.5 unfertig ist, RE2 Retail = Ziel, Beleg aus RE2; Sound ist RE2 — <baum>/RE2_SOUND_LISTE.md zuerst) -> Adresse/Bytes ins Dossier -> DANN Code -> VERIFIZIEREN durch erneutes Messen. Jede Konstante @0x... (oder Byte-Offset) im Code UND in der Commit-Message; Nutzer-Vorgaben als NUTZER-VORGABE / PORT-WAHL, ihre FORM belegt. Kein "deferred/tunable/interim/Env-Schalter" als Abschluss. Regressionen ZUERST bisecten (v0.8.23 3ec5b1f8 / v0.8.24 e8b3958f / v0.8.25 2077bb8e, dann die Runde-37-Merges), verursachenden Commit nennen; der Fix muss den Befund ERKLAEREN. "Sieht richtig aus" ist kein Beleg; Nutzer-Beobachtung schlaegt Code-Hypothese.
- Quellen im Baum: RE_15_Quellcode_Overlays/STAGE1_full/ (0x8010xxxx; STAGE1/ ohne _full ist +0x800 verschoben), RE_15_Quellcode_V2/, ghidra1_V2.txt, RE15_FUN_CATALOG.md, RE15_KNOWLEDGE.md, STAGE1_FLAG_MAP.md, RE15_SCD_OPCODES_REFERENCE.md, RE2_SOUND_LISTE.md; RE2: info/re2leon/, ghidra_re2_Leon.txt, RE2_Quellcode_V2/, RE2_Quellcode_Overlays/. Werkzeuge .claude/skills/re15-psx-disasm/scripts/re15_disasm.py / re2_disasm.py; Skills .claude/skills/<name>/SKILL.md (rdt-analysis, scd-disassembly, ghidra-mapping, re15-psx-disasm, re15-room-probe, re15-room-capture, re15-port-visual-verify, re15-enemy-ai-re, re15-weapon-render, re15-parity-verify, re15-pc-render-order, re15-savestate-ghidra, re15-pcsx-watchpoint) ZUERST lesen. DuckStation-GDB fuer RE2-/RE1.5-Messungen: Muster in analysis/befunde_runde36/H_raeume.md und J_affen.md (EnableGDBServer in %LOCALAPPDATA%/DuckStation/settings.ini, danach zurueck). Memory-Notizen (reai-v2-...) unter C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/<name>.md (nur lesen).

## Bauen, Messen, Testen
- NUR so bauen: cd "<baum>" && bash re15_port/tools/local_build.sh configure (einmal und nach jeder NEUEN .c-Datei), danach ... build (test/all = volle Suite: im Spurbaum VERBOTEN, s.u.). NIE cmake --build aus Git-Bash. exe <baum>/re15_port/build/platform/pc/re15_pc.exe; befund.log/debug.log daneben. ⛔ KEINE volle Suite (local_build.sh test/all) im Spurbaum — gemessen 2026-10-10: fuenf parallele Suiten brauchten je ~80 min und lieferten Dutzende Last-Timeouts der Fensterhaken (unbrauchbar). Stattdessen GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = eigene Tests (r38_${s.thema}) + die Testgruppen JEDES Moduls/Raums, das dein Diff beruehrt (git -C "<baum>" diff ${BASE(s)} --name-only lesen; vorhandene Testnamen: ctest --test-dir ... -N | grep -i <modul/raum>). Die ctest-Summenzeile ("100% tests passed, 0 tests failed out of N") woertlich mit dem -R-Ausdruck ins Dossier. Die volle Suite faehrt der Orchestrator EINMAL im Integrationsbaum. Fenster-Haken flattern unter Last (boot_bg_pin, dark_start_pin, relatch_pin, save_counter_pin, weste_load_pin, r30_sicherung_*, r30_titel_puls, r34n_e_dokumente_bild; exe-Starts brechen selten mit exit 1 nach 0,1 s ab): einzeln 2-3x nachfahren (ctest --test-dir "<baum>/re15_port/build" -R "^<name>$"), nicht als Regression werten. NIE taskkill /IM re15_pc.exe (nur eigene PID); Messkopie der exe unter eigenem Namen neben der exe (Muster tests/integration/test_r34n_b_cursor.cmake). Nie zwei Builds gleichzeitig im eigenen build/ (ein zweites Bauverzeichnis fuer Bisects: RE15_BUILD_DIR=<baum>/re15_port/build_bisect).
- Harness: getenv("RE15_...") in platform/pc/main.c + engine/src (RE15_DEBUG_JUMP, RE15_GOTO_ROOM, RE15_FORCE_EVENT, RE15_FORCE_CUT, RE15_PLAYER_POS, RE15_EQUIP, RE15_GIVE, RE15_PRESS, RE15_PAD_AT, RE15_INPUT_SCRIPT, RE15_SET_FLAG(_AT), RE15_FIRE_AOT, RE15_FORCE_EXPLOSION, RE15_GRANATE_LOG, RE15_STATE_LOG, RE15_EVT_TRACE, RE15_SCD_TRACE, RE15_MSG_LOG, RE15_ENEMY_DBG, RE15_ANIM_TRACE, RE15_NECK_LOG, RE15_FX_LOG, RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC (Ton-Mitschnitt ohne Audiogeraet), RE15_FRAMEDUMP, RE15_EXIT_AT, RE15_KILL_AT, RE15_CONTINUE_TEST + RE15_CARD_AUTO, RE15_PSELECT_AUTO(_SWITCH) fuer Elza, RE15_CLIMB_TRACE, RE15_TUER_TEST/_SEITE, RE15_MAP_SHOT_PAGE, RE15_PRI_LOG) — Semantik im Code nachlesen. Bilder NUR per RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1 (gdigrab liefert weisse Bilder; Framedump schreibt Fensterskalierung). Story-/Raum-Punkte ueber den ECHTEN Weg abnehmen (Tuer betreten, Spielstand + CONTINUE), nicht nur per Sprung; Elza-Punkte mit Elza (RE15_PSELECT_AUTO).
- Tests: NEUE Sonden/Pins in re15_port/tests/unit/probes/r38_${s.thema}.cmake (GLOB; Muster probes/r37_*.cmake) + tests/unit/test_r38_${s.thema}*.c / tests/integration/test_r38_${s.thema}*.cmake (registriert NUR aus deiner probes-Datei). NICHT tests/unit/CMakeLists.txt und NICHT tests/integration/CMakeLists.txt. Mindestens ein Test je Nutzer-Punkt, der die MECHANIK misst. Arbeitsordner eines Tests nie gleich dem Target-Namen (Linux ohne .exe).
- Dateien: neue Logik in NEUEN Dateien engine/src/<thema>_r38.c + include/re15_<thema>_r38.h; gemeinsame Dateien nur mit kleinen Haken (1-5 Zeilen) an klar benannten Stellen, Installer-Zeile am ENDE des Blocks in scd_room_setup.c mit Kommentar "Runde 38 Spur ${s.id}". KEIN Patch an shared_assets/PSX/*; neue Daten gen/*.inc oder shared_assets/RE2/ (im Dossier nennen). release/, platform/android/, RELEASE_NOTES.md: TABU.
- Token sparen: grosse Dateien nie ganz lesen (grep -n + Bereich), Logs gefiltert, Bau-/Suite-Ausgaben in Datei umleiten und nur Ende/Fehler lesen, keine Messung wiederholen, nur urteilstragende Bilder ansehen.

## Abschluss
- Dossier mit Abschnitten: Messung vorher (bei Regressionen: Bisect-Tabelle) / RE-Belege (Adressen, Bytes) / Umsetzung (Dateien, Konstanten) / Messung nachher / Tests (Namen, Ergebnis) / OFFEN / Fuer den Nutzer (neue Assets fuer das Paket-Gate, Sprachdateien, Bedienhinweise, Entscheidungsfragen). Letzter Commit fix/feat(r38-${s.thema}): ... Gezielte Tests gruen: ctest-Summenzeile + -R-Ausdruck woertlich zitieren (KEINE volle Suite). Antwort = StructuredOutput.
`

const bauPrompt = (s) => `Du bist Spur ${s.id} "${s.thema}" der Runde 38 (2026-10-16) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Du ermittelst, belegst, baust und testest ALLE Punkte dieser Spur im eigenen Arbeitsbaum. Ein anderer Agent nimmt danach unabhaengig gegen den Wortlaut des Nutzers ab — was du nicht gemessen und belegt hast, faellt dort durch.
${STAND[s.id] ? `
## ⛔ FORTSETZUNG — dein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST das Dossier, git -C "<baum>" log --stat ${BASE(s)}..HEAD und den Code-Diff gegen master. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg. Stelle je Nutzer-Punkt fest, was noch fehlt, und schliesse genau das ab; pruefe kritisch, ob der Stand den WORTLAUT erfuellt.
` : ''}
## Deine Punkte (Wortlaut des Nutzers, AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Einstiegspunkte (Stand 2026-10-16 — Zeilennummern und Behauptungen SELBST verifizieren)
${s.hinweise}
${REGELN(s)}
Arbeite jeden Punkt vollstaendig ab (Dossier je Punkt: Messung vorher / Beleg / Umsetzung / Messung nachher). Sackgasse: Weg wechseln (Xrefs, Caller/Callee, RE2, Savestate-RAM, GDB, Laufzeit-Log), nicht aufgeben, nicht raten. Am Ende: gezielte Tests gruen (keine volle Suite), alles committet, Dossier vollstaendig.`

const nachPrompt = (s, abn, runde, vorher) => `Du bist Spur ${s.id} "${s.thema}" der Runde 38 (2026-10-16) im Projekt reAi_v2 — NACHBESSERUNG ${runde} nach einer unabhaengigen Abnahme. Der Bau-Agent vor dir hat im selben Baum gearbeitet; sein Stand ist committet (lies git -C "<baum>" log --oneline ${BASE(s)}..HEAD, das Dossier und den Abnahmebericht <baum>/analysis/befunde_runde38/${s.id}_abnahme_${runde - 1}.md).

## Deine Punkte (Wortlaut des Nutzers)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

${STAND[s.id] ? `
## ⛔ FORTSETZUNG — ein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST git -C "<baum>" log --stat ${BASE(s)}..HEAD, das Dossier und den Abnahmebericht. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg.
` : ''}
## Zusammenfassung des Bau-Agenten
${vorher ? vorher.zusammenfassung : '(keine)'}
Offen laut Bau-Agent: ${vorher && vorher.offene_punkte && vorher.offene_punkte.length ? vorher.offene_punkte.join(' | ') : '(nichts)'}

## Maengel der Abnahme (JEDEN beheben oder mit Messung + Adresse widerlegen)
${abn.maengel.map((m, i) => `${i + 1}. ${m}`).join('\n')}

## Urteile der Abnahme je Punkt
${abn.urteile.map(u => `- ${u.punkt}: ${u.urteil} — ${u.beleg}`).join('\n')}

## Einstiegspunkte
${s.hinweise}
${REGELN(s)}
Punkte, die die Abnahme als erfuellt bewertet hat, NICHT verschlechtern (ihre Riegel gruen halten). Schreibe im Dossier einen Abschnitt "Nachbesserung ${runde}" (je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher). Baue neu, gezielte Tests gruen (keine volle Suite), committe (fix(r38-${s.thema}): Nachbesserung ${runde} ...).`

const abnahmePrompt = (s, bau, runde) => `Du bist die UNABHAENGIGE ABNAHME ${runde} fuer Spur ${s.id} "${s.thema}" der Runde 38 (2026-10-16) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Ein Bau-Agent behauptet, die Punkte des Nutzers umgesetzt zu haben. Du glaubst ihm NICHTS, was du nicht selbst am gebauten Stand gemessen hast. Der Nutzer weist jede Aenderung ohne Disasm-Beleg zurueck — du bist seine Instanz.

## Die Punkte des Nutzers (Wortlaut; AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Behauptung des Bau-Agenten
Status: ${bau ? bau.status : 'KEIN ERGEBNIS (Agent abgebrochen) — pruefe Baum und Dossier trotzdem'}
${bau ? bau.zusammenfassung : ''}
Commits: ${bau && bau.commits ? bau.commits.join(', ') : '?'}
Suite laut Bau-Agent: ${bau ? bau.suite : '?'}
Offen laut Bau-Agent: ${bau && bau.offene_punkte && bau.offene_punkte.length ? bau.offene_punkte.join(' | ') : '(nichts)'}

## Baum
${ROOT}/.claude/worktrees/${s.baum} (Zweig r38/${s.thema}, Basis ${BASIS}). ALLE Zugriffe und git-Befehle nur dort (absolute Pfade, git -C "<baum>", Bash mit cd "<baum>"). Nichts ausserhalb des Baums aendern. Aus dem Hauptbaum ${ROOT}/ nur lesen (info/Resident_Evil_und_Playstation_Information/, .agent_refs/).

## Vorgehen
1. Lies <baum>/analysis/befunde_runde38/AUFTRAG.md, VERTRAG.md, das Dossier ${s.id}_${s.thema}.md und: git -C "<baum>" log --oneline ${BASE(s)}..HEAD; git -C "<baum>" diff ${BASE(s)} --stat; git -C "<baum>" diff ${BASE(s)} -- re15_port (den Code-Diff WIRKLICH lesen).
2. Baue: cd "<baum>" && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build (nie cmake --build aus Git-Bash). KEINE volle Suite (test/all) im Spurbaum — die faehrt der Orchestrator einmal im Integrationsbaum. Fahre GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = Tests der Spur (r38_${s.thema}) + die Testgruppen aller Module/Raeume, die der Diff beruehrt (git diff ${BASE(s)} --name-only; ctest -N | grep -i). Rote Fenster-Haken unter Last 2-3x einzeln nachfahren, bevor du sie wertest. NIE taskkill /IM re15_pc.exe; Messkopie der exe unter eigenem Namen (Muster tests/integration/test_r34n_b_cursor.cmake).
3. MISS jeden Nutzer-Punkt selbst an der exe (<baum>/re15_port/build/platform/pc/re15_pc.exe; Haken getenv("RE15_...") — Semantik im Code nachlesen; Spielstand + CONTINUE, RE15_DEBUG_JUMP, RE15_FORCE_EVENT, RE15_PLAYER_POS, RE15_PRESS/RE15_PAD_AT/RE15_INPUT_SCRIPT, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, Logs RE15_STATE_LOG/RE15_EVT_TRACE/RE15_ENEMY_DBG/RE15_FX_LOG/RE15_SE_DEBUG/RE15_AUDIO_CAP_SYNC usw.; Elza-Punkte mit RE15_PSELECT_AUTO). Story-/Raum-Punkte ueber den ECHTEN Weg (Tuer betreten), nicht nur per Sprung. Bei Regressionen: Gegenprobe mit der v0.8.25-exe, ob der genannte verursachende Commit den Befund wirklich erklaert. Urteil je Punkt gegen den WORTLAUT: erfuellt / teilweise / nicht_erfuellt / nicht_pruefbar (mit Grund) — mit Messprotokoll (Befehl, Log-Zeilen, Bild-Pfad).
4. RE-Gate: Jede verhaltensrelevante Konstante im Diff hat @0x... (oder Byte-Offset) ODER ist als NUTZER-VORGABE / PORT-WAHL gekennzeichnet; stichprobenartig 3 Adressen SELBST disassemblieren (re15_disasm.py / re2_disasm.py / ghidra*.txt) und pruefen, dass die zitierte Stelle das Behauptete enthaelt (Sprungziel disassemblieren, richtige Binaerdatei). Suche im Diff nach "deferred", "tunable", "interim", "for now", "faithful", "plausibel", "TODO", Env-Schaltern als Abschluss. Pruefe, ob der Fix den gemessenen Befund ERKLAERT.
5. Vertrag: nur zugeteilte Bank-9-Bits/Nachrichten-IDs/Slots/Ereignisse (VERTRAG.md); erlaubte Pfade (git diff ${BASE(s)} --name-only: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists); gemeinsame Dateien nur kleine Haken; Tests je Punkt vorhanden und messend (ctest --test-dir "<baum>/re15_port/build" -R "r38_${s.thema}" laufen lassen). Pruefe auch Nebenwirkungen auf das, was die Spur beruehrt (z.B. andere Raeume, beide Szenarien).
6. Schreibe den Bericht <baum>/analysis/befunde_runde38/${s.id}_abnahme_${runde}.md (je Punkt Messprotokoll, Urteil, Beleg; Gates; Maengel nummeriert, konkret, nachpruefbar) und committe NUR diese Datei: git -C "<baum>" add analysis/befunde_runde38/${s.id}_abnahme_${runde}.md && git -C "<baum>" commit -m "doc(r38-${s.thema}): Abnahme ${runde}". Du aenderst KEINEN Code. Schreibe den Bericht FORTLAUFEND (nach jedem gemessenen Punkt speichern) — das Sitzungslimit kann dich jederzeit beenden.
7. bestanden = true NUR, wenn alle Punkte "erfuellt" sind und alle Gates halten. Sonst bestanden = false mit Maengelliste. Antwort = StructuredOutput.`

const BAU_SCHEMA = {
  type: 'object',
  properties: {
    status: { type: 'string', enum: ['fertig', 'teilweise', 'blockiert'] },
    zusammenfassung: { type: 'string', description: 'max 12 Zeilen: was gemessen, was belegt (Adressen), was gebaut' },
    commits: { type: 'array', items: { type: 'string' } },
    suite: { type: 'string', description: 'woertliche ctest-Summenzeile der gezielten Laeufe + -R-Ausdruck (keine volle Suite), sonst der Fehler' },
    offene_punkte: { type: 'array', items: { type: 'string' } },
    neue_assets: { type: 'array', items: { type: 'string' } },
    sprachdateien: { type: 'array', items: { type: 'string' } },
    gemeinsame_dateien: { type: 'array', items: { type: 'string' } },
  },
  required: ['status', 'zusammenfassung', 'commits', 'suite', 'offene_punkte'],
}

const ABN_SCHEMA = {
  type: 'object',
  properties: {
    bestanden: { type: 'boolean' },
    urteile: { type: 'array', items: { type: 'object', properties: {
      punkt: { type: 'string' },
      urteil: { type: 'string', enum: ['erfuellt', 'teilweise', 'nicht_erfuellt', 'nicht_pruefbar'] },
      beleg: { type: 'string' } }, required: ['punkt', 'urteil', 'beleg'] } },
    maengel: { type: 'array', items: { type: 'string' } },
    suite: { type: 'string' },
    bericht: { type: 'string' },
  },
  required: ['bestanden', 'urteile', 'maengel', 'bericht'],
}

const CFG = args || {}
const WELLE = (CFG.welle || SPUREN.map(s => s.id)).map(id => SPUREN.find(s => s.id === id)).filter(Boolean)
const MODUS = CFG.modus || {}
const STAND = CFG.stand || {}
const BAU_VORHER = CFG.bau || {}
const RUNDE0 = CFG.runde0 || {}
const ABN_VORHER = CFG.abn || {}
const MAX_NACH = CFG.max_nach === undefined ? 1 : CFG.max_nach
const MODELL = CFG.model ? { model: CFG.model } : {}
const BASE = (s) => s.basis || 'master'

let frei = CFG.slots || SLOTS
const warteschlange = []
const nimm = () => new Promise(res => { if (frei > 0) { frei--; res() } else warteschlange.push(res) })
const gib = () => { const n = warteschlange.shift(); if (n) n(); else frei++ }

const laufSpur = async (s) => {
  await nimm()
  const kopf = { spur: s.id, thema: s.thema, baum: s.baum }
  try {
    let bau
    if (MODUS[s.id] === 'abnahme' || MODUS[s.id] === 'nachbesserung') {
      bau = BAU_VORHER[s.id] || null
      log(`Spur ${s.id} ${s.thema}: Bau liegt vor, direkt zur ${MODUS[s.id] === 'abnahme' ? 'Abnahme' : 'Nachbesserung'}`)
    } else {
      log(`Spur ${s.id} ${s.thema}: ${STAND[s.id] ? 'Fortsetzung' : 'Ermittlung+Bau'} startet`)
      bau = await agent(bauPrompt(s), { label: `bau:${s.id}-${s.thema}${STAND[s.id] ? '-forts' : ''}`, phase: 'Ermittlung+Bau', schema: BAU_SCHEMA, ...MODELL })
      if (!bau) { log(`Spur ${s.id}: Bau OHNE Ergebnis (abgebrochen) — Spur haelt an`); return { ...kopf, bestanden: false, abgebrochen: 'bau', nachbesserungen: 0, bau: null, abnahme: null } }
      log(`Spur ${s.id} Bau: ${bau.status}, Suite: ${bau.suite}`)
    }
    let runde = RUNDE0[s.id] || 0
    let nachbesserungen = 0
    let abn = null
    if (MODUS[s.id] === 'nachbesserung') {
      abn = ABN_VORHER[s.id]
      runde++
      nachbesserungen++
      const nach0 = await agent(nachPrompt(s, abn, runde, bau), { label: `nachbesserung:${s.id}-${s.thema}#${runde}`, phase: 'Nachbesserung', schema: BAU_SCHEMA, ...MODELL })
      if (!nach0) { log(`Spur ${s.id}: Nachbesserung ${runde} OHNE Ergebnis (abgebrochen)`); return { ...kopf, bestanden: false, abgebrochen: 'nachbesserung', nachbesserungen, bau, abnahme: abn } }
      log(`Spur ${s.id} Nachbesserung ${runde}: ${nach0.status}`)
      bau = nach0
    }
    while (true) {
      abn = await agent(abnahmePrompt(s, bau, runde), { label: `abnahme:${s.id}-${s.thema}#${runde}`, phase: 'Abnahme', schema: ABN_SCHEMA, ...MODELL })
      if (!abn) { log(`Spur ${s.id}: Abnahme ${runde} OHNE Ergebnis (abgebrochen)`); return { ...kopf, bestanden: false, abgebrochen: 'abnahme', nachbesserungen, bau, abnahme: null } }
      log(`Spur ${s.id} Abnahme ${runde}: ${abn.bestanden ? 'BESTANDEN' : 'Maengel ' + abn.maengel.length}`)
      if (abn.bestanden || nachbesserungen >= MAX_NACH) break
      runde++
      nachbesserungen++
      const nach = await agent(nachPrompt(s, abn, runde, bau), { label: `nachbesserung:${s.id}-${s.thema}#${runde}`, phase: 'Nachbesserung', schema: BAU_SCHEMA, ...MODELL })
      if (!nach) { log(`Spur ${s.id}: Nachbesserung ${runde} OHNE Ergebnis (abgebrochen)`); return { ...kopf, bestanden: false, abgebrochen: 'nachbesserung', nachbesserungen, bau, abnahme: abn } }
      bau = nach
    }
    return { ...kopf, bestanden: !!abn.bestanden, nachbesserungen, bau, abnahme: abn }
  } finally {
    gib()
    log(`Spur ${s.id} ${s.thema}: Ende`)
  }
}

const ergebnisse = (await parallel(WELLE.map(s => () => laufSpur(s)))).filter(Boolean)
const fehlend = WELLE.filter(s => !ergebnisse.find(e => e.spur === s.id)).map(s => s.id)
if (fehlend.length) log(`OHNE Ergebnis: ${fehlend.join(', ')}`)
return { ergebnisse, fehlend }