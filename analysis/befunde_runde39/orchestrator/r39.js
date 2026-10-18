export const meta = {
  name: 'r39-runde',
  description: 'Runde 39: Bau-Spuren je Ermittlung+Bau -> unabhaengige Abnahme -> Nachbesserung, in eigenen Arbeitsbaeumen, Welle fuer Welle',
  phases: [
    { title: 'Ermittlung+Bau', detail: 'Dossier, Messung/Bisect, RE-Beleg, Code, gezielte Tests im eigenen Baum' },
    { title: 'Abnahme', detail: 'unabhaengig gegen den Wortlaut des Nutzers am gebauten Stand gemessen' },
    { title: 'Nachbesserung', detail: 'Maengel der Abnahme beheben, erneut messen' },
  ],
}

const ROOT = 'C:/workspace/git/reAi_v2'
const BASIS = (args && args.basis) || 'v0.8.26'
const SLOTS = 4
const MIN_TESTS = 960

const SPUREN = [
  { id: "A", thema: "stimmen", baum: "r39_stimmen",
    punkte: [
      "In Room 1050 wird das Voiceover nicht abgespielt von vor dem Retten von Ada - das ich unter main09-main11 abgelegt habe. Also die Voiceovers für unsere neue Cutscene",
      "Die Voiceovers für die letzte Cutscene in ROOM 1150, wenn Irons stirbt sind nicht drin....",
      "In ROOM 11B0 bei der finalen Cutscene mit Ada und Marvin, fehlt das neue Voiceover von But hold on und I want to make sure we don't have any unexpected Visitors. Was dir eigentlich vorliegt.",
      "Bei \"Alright, let's get down from here\" fehlt aus irgendeinen Grund mal wieder das Voiceover.",
    ],
    hinweise: "REGEL synchro/README.md: synchro/STAGE<n>/room<RAUM>/main<MSG>.wav, Message-Id = Datei, allein die Datei entscheidet. LADER platform/pc/src/audio_pc.c re15_voice_load_clip (~Z.2240: erst room%04X, dann room%04x; Sonderdateien re15_elza_stimme_datei (Runde 36 L), re15_rekorder_r38_stimme (Runde 38 J), re15_elzaintro_r38_stimme_pfad (Runde 38 E)); ausgeloest ueber Message_on (scd_vm.c) — Port-Szenen mit eigenen Programmen (gen/*.inc, Szenen-Laeufer) muessen denselben Weg nehmen. BESTAND 2026-10-18: room1050 main08-11; room1150 main02, 04-10, 12-21; room11B0 UND room11b0 (ZWEI Ordner, nur Gross/Klein verschieden — unter Linux/Deck verschiedene Ordner, im Paket?) je main00-14; room1171 main00-02 (Texte 'Elza, hurry up!' / 'We need to get this thing off of here! Help me out!' / 'C'mon, give me a hand!' / 'Alright, let's get down from here!' — vier Zeilen, drei Dateien: Zuordnung Message-Id -> Datei belegen). Nutzer-Dateien NIE umbenennen/aendern (VERTRAG 1.2) — Zuordnung im Code. SCHRITT 1 MESSEN je Punkt am ECHTEN Weg (Spielstand + CONTINUE/Tuer, Elza fuer 1171): RE15_MSG_LOG + '[voice]'-Zeilen im debug.log + RE15_AUDIO_CAP_SYNC — welche Message-Id (Original oder Port-Id) zeigt die Szene, welche Datei wird gesucht, warum nicht gefunden/gespielt. (1) ROOM1050 neue Ada-Rettungs-Szene (grep -rn 1050 engine/src: adaruf_1050.c, rolltor_1050.c, Runde 18 Ada-Eintritt, Runde 34/37 Szenen) main09-11 (main08?). (2) ROOM1150 Szene nach Irons' Tod (engine/src/irons_tod_1150.c, Runde 35 L): welche Zeilen der letzten Szene ohne Ton, welche Dateien passen. (3) ROOM11B0 Marvin/Ada-Szene (tools/r36_j/szene11b0_bauen.py -> engine/src/gen/szl11b0_szene.inc; Runde 38 C machte msg 20 'But, hold on...'): 'Was dir eigentlich vorliegt' -> die Aufnahme liegt im Bestand; Zuordnung per Dateizeit (git log --format=%ad -- synchro/STAGE1/room11B0 room11b0), Laenge, Reihenfolge der Szene belegen (beide Ordner vergleichen: md5 je Datei). (4) ROOM1171 'mal wieder': git log -S und Runde 36/38 (I heliport1171, Dossier analysis/befunde_runde38/I_heliport1171.md) — warum fehlt die Zeile erneut. Fix muss die Ursache ERKLAEREN. Riegel je Punkt: '[voice] clip loaded'-Zeile + Mitschnitt-RMS im Zeilenfenster am echten Weg." },
  { id: "B", thema: "toene", baum: "r39_toene",
    punkte: [
      "In Room 10F0 fehlt noch beim öffnen der Schublade der Sound. Den könnte man zum Beispiel aus Resident Evil 2 holen, wenn dort eine Schublade geöffnet wird.",
      "In Room 1090, wenn die Hunde in der Cutscene rein gesprungen wird, fehlt noch der Sound vom öffnenden Gitter, den könnte man von den öffnenden Gittern der Spinnen aus STAGE2 holen.",
      "In Room 1090 fehlt noch Sound für den bewegenden Schrank nach dem lösen des Rätsels. Den könnte man aus Resident Evil 2 holen von den bewegenden Regalen in der 2. Etage.",
    ],
    hinweise: "QUELLEN: RE2_SOUND_LISTE.md ZUERST (SAP-Liste CORE/ENEMY/ROOM/WEAPON), RE2-Raumbaenke info/re2leon/ (gleiche Nummern), ghidra_re2_Leon.txt, RE2_Quellcode_Overlays/; RE1.5 Stage-2-RDTs (shared_assets/PSX/STAGE2/*.RDT, nur lesen). Muster fuer RE2-Toene im Port: shared_assets/RE2/*.VBS + audio_pc.c Ladepfade ('... fehlt -> ... stumm', z.B. GLAS1090, TUERSE, KARTSE, ELZASE ~Z.1000-1700/4000-4200), Extraktions-Werkzeuge der Runden 31/36/38 (grep -rln 'VBS' tools/). (1) ROOM10F0 (Leon; Elza 10F1 gegenpruefen): wo oeffnet die Schublade (SCD-Sub oder Port-Haken, grep 10f0/10F0 in engine/src: cut_10f0.c, schluessel_r36.c), RE2-Schubladen-Ton: ein RE2-Raum mit Schublade + Se_on im SCD (Raum, Satz, Offset belegen). (2) ROOM1090 Hunde-Szene: das Gitter, durch das die Hunde springen (Szene/Prop belegen) — Ton der sich oeffnenden Schachtgitter der Spinnen in Stage 2 (Runde 38 R/T Schachtspinnen ROOM2060/20A0: Se_on-Satz im Stage-2-RDT/Overlay belegen), im Takt des Gitter-Oeffnens in 1090. (3) ROOM1090 Schrank nach dem Raetsel (Port: probe_1090_push*, test_1090_push_pin, test_1090_npc_prop_push; Schrank-Bewegung grep 1090 engine/src) — RE2 2. Etage bewegliche Regale (RE2-Raum + Se_on belegen), Ton ueber die Bewegungsdauer (Schleife/Wiederholung wie RE2). Riegel je Punkt: RE15_AUDIO_CAP_SYNC (Ton im Zeitfenster, Kreuzkorrelation wie probe_r38_elzatoene_kreuz) + Herkunft (Datei, Offset) im Dossier. Neue Dateien unter shared_assets/RE2/ fuer das Paket-Gate im Dossier nennen." },
  { id: "C", thema: "inventar", baum: "r39_inventar",
    punkte: [
      "In Room 1000 möchte ich im Spind, wo vorher der Feuerlöscher drin war jetzt einmal Handfeuerwaffen Munition drin haben. Erst danach den text \"Nothing useful\", den du jetzt schon hast.",
      "Memory Cards sind noch nicht Stapelbar, das ist falsch, alles mit einer Zahl dran an amounts was gleich ist muss stapelbar sein.",
      "Elza hat im Player Inventory noch das Bild und die Karte von Leon oben drin.",
      "Ich habe die gelbe Karte bei Elza noch, obwohl ich gesagt habe \"discard\".",
    ],
    hinweise: "(1) ROOM1000 Spind: heute 'Nothing useful' (Port-Text oder Original-Nachricht belegen); vorher lag dort der Feuerloescher (Runde 37 I feuerloescher_r37.c, Runde 38 K verlegte ihn nach 1030: analysis/befunde_runde38/K_itemorte.md). NUTZER-VORGABE: einmal Handgun-Munition (Item-Id + Menge belegen: Original-Munitionsfunde derselben Stage / RE2-Standardmenge), danach 'Nothing useful'. Aufnahme-Ablauf wie die anderen Port-Funde (inhalt_r35.c, leiche_1110_1230.c: Modal, Inventar voll). Taken-Bit Bank 9 Bit 69 (VERTRAG 1.1, in k_port[] eintragen). (2) Memory Cards stapeln: Stapelregel im Port (inventory_common.c: Mengen-/Stapel-Logik, Item-Klassen) und Original (Item-Tabelle mit Max-Menge/Stapel-Flag @0x..., RE15_KNOWLEDGE.md Item-Abschnitt) — NUTZER-VORGABE 'alles mit einer Zahl dran ... was gleich ist muss stapelbar sein': welche Items zeigen eine Menge, welche davon stapeln nicht (Zensus aller Item-Ids), Memory Card zuerst; RE2: Ink Ribbon stapelt (Beleg). Aufnahme, Kombinieren, Kiste (Item-Box) mitpruefen. (3) Elza-Inventar oben Leons Bild und Karte: Status-Schirm (re15_inv_screen.c, Portrait/Kopf-Kacheln) waehlt nach Spieler (PL04 = Elza)? Elza-Portrait/-Karte: Quelle belegen (RE1.5-Elza-Assets, ITEM/ oder Status-TIM; sonst RE2-Claire-Form als PORT-WAHL). (4) gelbe Karte bei Elza nach 'discard' noch da: Wegwerf-Ablauf (Runde 25 discard/Code-Sperre, aufschliessen_r36.c, schluessel_r36.c) — welche Karte (Item-Id), wo fragt das Spiel, warum wird sie bei Elza nicht entfernt (Leon gegenpruefen). Riegel je Punkt (Inventar-Zustand im State-Log, Framedump Status-Schirm)." },
  { id: "L", thema: "optik", baum: "r39_optik",
    punkte: [
      "Ich habe noch mal einen Marker mit F9 gemacht. Leon ist immer noch vom Boden überdeckt an den einen Streifen.",
      "Der Schlüssel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran hängt, nicht so riesig.",
    ],
    hinweise: "(1) F9-MARKE des Nutzers 2026-10-17 21:49: analysis/befunde_runde39/nutzer/befund_1100_F741_marke1.bmp + befund_marken_2026-10-17.txt (Auszug befund.log): ROOM1100 Cut 1, Leon pos (-16007,0,-23658) rot 3023, Kasten x158..180 y108..146, Masken 42 (178,59) 7x109 t=124 z=8007 DECKT und 43 (176,60) 6x104 t=128 DECKT (schmale senkrechte Streifen). 'immer noch': Runde 37 Spur A pri1100 (analysis/befunde_runde37/A_*.md; Riegel test_r37_pri1100_marke, test_r37_pri1100_elza) und fruehere Marken (2026-10-05 F240, 2026-10-12 F892 in ROOM1100) — was wurde gemacht, warum deckt der Streifen weiter. Masken-Regeln: memory reai-v2-nutzer-misst-selbst, reai-v2-flache-maske-verdeckt-nichts, reai-v2-masken-nur-wo-man-hinkommt, reai-v2-pri-hashliste-mitcommitten, reai-v2-re2-pri-vorbild (RE2 als Vorbild), reai-v2-sprite-pri-occlusion. Zweite, juengere Marke 23:31 ROOM1090 Cut 0 (befund_1090_F1357_marke1.bmp, 'keine Maske am Koerper') liegt mit bei — ansehen; gehoert sie zu diesem Befund, mitbehandeln, sonst im Dossier fuer den Nutzer beschreiben. (2) Schluessel an der Pinnwand ROOM10F0/10F1 (Parking Lot Key Leon / Prison Key Elza; Runde 38 K itemorte, engine/src/schluessel_r36.c Prop + Lage): Masstab des Prop-Modells gegen die Groesse vergleichbarer Original-/RE2-Schluessel-Modelle (Weltmasse belegen, memory reai-v2-modellmass-aus-kamera) — kleiner/dezenter (NUTZER-VORGABE), weiter sichtbar und aufnehmbar. Riegel: Masken-Log/Pixel am Markenort (Leon nicht gedeckt, Gegenprobe alter Stand), Schluessel-Kasten im Framedump kleiner, Aufnahme unveraendert." },
  { id: "D", thema: "treffer", baum: "r39_treffer",
    punkte: [
      "Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original Resident Evil 2 auf jeden Fall nicht.",
      "Zombies sind über den Tisch hinweg nicht treffbar. Das darf so nicht sein",
      "Das Auto Aim darf nicht gelten für Gegner die der Spieler sonst garnicht sehen könnte, zum Beispiel wenn sie um die Ecke sind und nicht mal ansatzweise in Leons Blickfeld mit Wand davor.",
    ],
    hinweise: "Default-KI ist RE2 (engine/src/enemy_ai_re2_zombie.c), RE1.5 per RE15_AI_FLAVOR=re15 — beide pruefen. (1) Aufstehen: Treffbarkeit im Aufsteh-Clip (befund.log-Spalte 'treffbar=', +0x93-Latch memory reai-v2-hit-latch-93, Band/Grid-Tore Runde 38 c939ce5d @0x8003710c / @0x800120ec, Runde 16 aufstehen-schuss, Runde 37 re2zombie) — RE2 Retail: Treffer-Gate je Zombie-Zustand im RE2-Overlay belegen. (2) ueber den Tisch: welcher Raum/Tisch (Nutzer nennt keinen — Kandidaten mit Tisch + Zombie belegen und messen), Schussweg/Zielhilfe gegen niedrige Hindernisse (Kollisionszellen-Hoehe, Original: Kugel ueber niedrige Hindernisse @0x..., RE2 gleich?). (3) Auto-Aim nur sichtbare Gegner: Zielauswahl des Ports (grep -rn 'aim' engine/src: Zielwahl/Lock-on) gegen RE2 (Zielwahl: Sichtwinkel/Entfernung/Sichtlinie @0x... in RE2-EXE) und RE1.5 — Sehstrahl gegen Wandzellen (memory reai-v2-tiefe-aus-kollision). NUTZER-VORGABE greift nur, soweit RE2 nicht schon belegt. Riegel je Punkt: Treffer/HP im State-Log (RE15_ENEMY_DBG), Zielwahl-Log, Gegenprobe alter Stand." },
  { id: "E", thema: "gegner", baum: "r39_gegner",
    punkte: [
      "Die Gorillas beim Parking Lot können ruhig doppelt so viel Energie haben. Außerdem sollten sie erst die flinch Animation machen, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen wurden, nicht bei jeden Schuss.",
      "Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch von anderen Hunden angegriffen/gebissen werden können, wenn ich mich nicht wehren kann, so zum Beispiel bei Room 1090.",
      "Die Zombies in ROOM 1220/1221 und in ROOM 1010 die am Boden liegen kriechen im Original. Das sollen sie bei uns auch.",
      "Die Gorillas haben beim springen direkt ihren Schatten unter dem beim.",
    ],
    hinweise: "(1) Parking Lot (Raum belegen; Gorilla Typ 0x27, memory reai-v2-maggot-ai, Runde 35 J / 36 affen): HP x2 gegen Original-HP (@0x... Spawn/Init) als NUTZER-VORGABE; Flinch erst, wenn der seit dem letzten Flinch akkumulierte Schaden >= 3 x Handgun-Schaden ist (Handgun-Schaden belegen, NUTZER-VORGABE) — andere Waffen entsprechend ihrem Schaden. Nur die Parking-Lot-Gorillas (nicht 1141). (2) Hundebiss (enemy_ai_re2_dog.c, memory reai-v2-dog-ai): waehrend ein Hund den Spieler beisst/greift, kein zweiter Biss — RE2: Griff-/Damage-Sperre des Spielers (Spieler-Zustand 'gegriffen' @0x... in RE2-EXE, FUN_80012d60 Spieler-Damage RE1.5) belegen und fuer alle Hunde gelten lassen; 1090 messen (zwei Hunde). (3) liegende Zombies kriechen (ROOM1220/1221 Elza, 1010): Runde 16 kriecher-1010 / liegende-zombies (analysis/befunde_runde16*), Spawn-Bytes (Sce_em_set Zustand/Variante) im RDT, Kriech-Zustand der KI (RE2 Kriechzombie, RE1.5 Overlay) — warum liegen sie still. (4) Gorilla-Schatten beim Sprung (Nachtrag 2026-10-18, AUFTRAG.md): der Schatten-Blob haengt beim Sprung unter dem Koerper mit in der Luft statt auf dem BODEN zu liegen. MESSEN (Framedump je Sprungbild + Gegner-y/Boden-y im Log), ORIGINAL: Schatten-Routine des Gegners (Boden-y der SCA-Zelle statt Koerper-y, Adresse; RE2 gleich), Port: Schatten-Quads render_pc.c/Prop-Schatten (memory reai-v2-char-shadow, Runde 36 O pri_schatten), fuer ALLE springenden Gegner (Gorillas 1141/Parking Lot, Hunde, Spinnen) pruefen. Riegel je Punkt (RE15_ENEMY_DBG, State-Log; Schatten: Schatten-y = Boden-y waehrend des Sprungs)." },
  { id: "F", thema: "zustand", baum: "r39_zustand",
    punkte: [
      "Wenn der Spieler verletzt wurde, hat dann kaputte Klamotten und heilt sich aber wieder vollständig - zum Beispiel mit Heilspray - dann muss er auch wieder die ganzen Klamotten haben.",
      "Der \"Poison\" Zustand ist zwar jetzt da, aber er zieht keine Energie ab Stück für Stück, so wie im Originalen Resident Evil 2. Zumindest zeigt er es vom EKG her nicht so an. Da ist noch ein Fehler.",
    ],
    hinweise: "(1) Kleidung: Runde 36 kleidung (Tests test_r36_kleidung_bild/_hundeweg/_spielweg; grep -rln kleidung engine/src) — Regel des Ports (welche HP-Schwelle setzt die kaputte Kleidung) und RE2-Beleg (RE2: Spieler-Modell/Texturwechsel nach Zustand? @0x...); NUTZER-VORGABE: volle Heilung -> ganze Kleidung. Beide Szenarien (Leon, Elza). (2) Gift: Runde 38 R (Spinnen-Gift setzt den Status; analysis/befunde_runde38/R_spinnen.md): RE2-Gift-Tick (HP-Abzug je N Bilder, Untergrenze, EKG-Farbe/-Kurve 'POISON' im Status-Schirm) @0x... in der RE2-EXE belegen; Port: Tick fehlt oder wirkt nicht / EKG zeigt es nicht. Gegengift heilt (Runde 38). Riegel: HP-Verlauf unter Gift (State-Log), EKG im Framedump des Status-Schirms, Kleidung nach Heilung (Modell-/Textur-Zustand)." },
  { id: "H", thema: "elza", baum: "r39_elza",
    punkte: [
      "Bei Elza, direkt in ROOM 1031 in der 1. Cutscene direkt am Anfang hebt sie die Hand aus irgendeinen Grund 2x, also lässt sie einmal fallen zwischendurch und hebt sie nochmal. Im Original hebt sie die Hand nur einmal und senkt sie dann wieder.",
      "Bei Elza, nach aufnehmen von Roys Diary muss direkt die Karte kommen, mit den Markern wo man hin muss, noch bevor das Spiel zum bewegen wieder zurückgeben wird nach dem schließen des Tagebuchs.",
      "Elza kann in ROOM 1090 bei dem Feuerlöscher, bei dem ausgebrannten Fahrzeug hochklettern warum auch immer... das ist falsch...",
    ],
    hinweise: "Elza = PL04, Raeume ROOMxxx1. (1) ROOM1031 erste Szene: Clip-Folge Elzas (RE15_ANIM_TRACE) gegen das Original-SCD (Plc_motion/Plc_ret/Ereignis-Anims @0x... im ROOM1031-SCD) und Elzas Clip-Tabelle (Runde 37 elzaems, Elza laedt aus CDEMD1.EMS) — doppelter Clip, Schleife oder falsche Rueckkehr; DuckStation-Gegenprobe des Originals. (2) nach Roys Tagebuch (Runde 38 G: engine/src/tagebuch_r38.c, ROOM1011) sofort die Karte mit den Markern (Communications Room + Prison Room, Runde 38 G) bevor die Steuerung zurueckkommt — Form wie RE2 Karten-Anzeige nach einem Hinweis (memory reai-v2-runde30: 'RE2 merkt sich nach dem Karten-Hinweis nichts'), NUTZER-VORGABE. (3) ROOM1091 (Elza) Feuerloescher am ausgebrannten Auto: Elza kann hochklettern — Kletter-Ausloeser (Runde 36 A klettern, climb_common.c, AOT/SCA-Zelle), Runde 37 I (Loescher leer am Boden 1090/1091) und Runde 38 K (Loescher-Ort) — welche Zone erlaubt das Klettern, Original-Beleg; Leon 1090 gegenpruefen. Riegel je Punkt." },
  { id: "G", thema: "karten", baum: "r39_karten",
    punkte: [
      "Bei den Maps des Police Departement passt es noch nicht so ganz. Also, ich habe dir Dateien hinterlegt: map3F_correct.png, map2F_correct.png, map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich aussehen sollte. Keine Doppelwände, Tür immer genau auf Wand platziert, treppen nicht in der Wand platziert usw. Und natürlich muss der Spielermarker da entsprechend passen. Der Room B1 den habe ich nicht gemacht. Da muss du generell noch einmal schauen, da passt nichts so richtig zusammen mit dem wie man sich bewegt im Vergleich zu der Karte. Die Karte von B1 ist noch ziemlicher schrott.",
      "In ROOM 1090 ist Leons Positionsmarker der Map außerhalb des aktuellen Raums.",
    ],
    hinweise: "NUTZER-VORLAGEN: analysis/befunde_runde39/nutzer/map1F_correct.png, map2F_correct.png, map3F_correct.png, mapB2_correct.png (je links Port, rechts Soll). B1 ohne Vorlage: aus Kollisionsdaten/Raumgeometrie und Tuerverbindungen neu ableiten. Karten-Code + Memory: reai-v2-karte-grundriss, reai-v2-karte-schleier-und-zeichenreihenfolge, reai-v2-karte-massstabstabelle, reai-v2-karte-schablonen-zonen, reai-v2-karte-1010-zeilen-spiegel (OFFEN), reai-v2-kollisionszellen-sind-waende, reai-v2-hindernis-ist-keine-wand, reai-v2-karten-sichtlauf (FB_SHOT), reai-v2-runde27-28-karten-messer, reai-v2-runde33 (Karte 2F), Runde 36 Karten (analysis/befunde_runde36/*karte*). (2) ROOM1090 Marker ausserhalb: Abbildung Raumkoordinate -> Kartenpixel fuer 1090 (Masstab/Versatz/Spiegel/Etage) belegen. Riegel: je Etage Kartenbild gegen die Soll-Haelfte (Wandlinien/Tueren/Treppen an den Soll-Pixeln), Marker-Lage je Raum (Stichproben inkl. 1090, Leon und Elza) im Raumrechteck." },
  { id: "I", thema: "funk10f1", baum: "r39_funk10f1",
    punkte: [
      "In Room 10F1 möchte ich, das wenn Elza ihn betritt eine Cutscene läuft wo sie Rechts zum Kommunikationssystem geht und von der Animation her drauf drückt und sagt \"Hello, anyone, can you hear me? We Need help!\"",
      "Dann kommt \"...\"",
      "Dann \"Uhh, this Thing is broken!\".",
      "Suche die dir passenden Animationen dafür heraus.",
    ],
    hinweise: "NEUE Szene (NUTZER-VORGABE) in ROOM10F1 (Elza, Communications Room), einmalig beim ersten Betreten (Bank 9 Bit 70, VERTRAG 1.1; Port-Ids 10F1 40-44, VERTRAG 1.2). Elza geht nach RECHTS zum Kommunikationssystem (Lage im Raum aus Hintergrund/Kamera-Schnitten/Kollision belegen), Druecken-Animation (Elza-Clips: Panel-/Schalter-Bedienung, RE2-Claire-Form; Clip-Wahl im Dossier begruenden), drei Zeilen als Nachrichten mit Stimme (synchro/STAGE1/room10F1/main<ID>.wav, falls vorhanden; sonst die Ids fuer den Nutzer im Dossier nennen). Form wie fruehere Port-Szenen: Runde 36 J szene11b0 (Generator tools/r36_j/szene11b0_bauen.py -> gen/*.inc), Runde 38 F/G (Szenen-Programme gen/dining_r38_programme.inc, tagebuch_r38.c), Spieler-Sperre waehrend der Szene (Runde 38 B szenenschutz). Andere 10F1-Inhalte (Prison Key an der Pinnwand Runde 38 K, Spur L dieser Runde aendert seine Groesse) unberuehrt. Leon 10F0 unveraendert. Riegel: echter Weg mit Elza in 10F1 -> Szene (Pfad, Clip, drei Zeilen in Reihenfolge, Stimme falls Datei), zweites Betreten ohne Szene." },
  { id: "J", thema: "dining1141", baum: "r39_dining1141",
    punkte: [
      "Mit Room 1141 bin ich noch überhaupt nicht zufrieden: 1. kommen komische Zombie Geräusche im Hintergrund, die will ich nicht. Es gibt ja keine Zombies.",
      "2. Finde ich es doch besser, wenn John durch die Tür kommt, statt durch das Tor. Es macht keinen Sinn das er dazwischen rein kam. Kannst du nicht während des Tür öffnens die Kamera vorne auf Elza irgendwie machen, dann Tür schließen Sound und dann Kamera auf Tür, wo John bereits da ist?",
      "3. Du hast irgendeinen komischen shout voiceover für John eingebaut. Denn will ich nicht.",
      "4. Ich möchte das du immer im Mix mit einigen Abständen die Voiceovers 1-5 stattdessen abspielst. Dazwischen möchte ich das du immer wieder ein mehrfachen Klopf gegen Tür Sound abspielst, den du aktuell noch garnicht hier drin hast.",
      "5. Ich möchte das Elza darauf reagiert, das sie das das erste mal hört wenn du den Raum betrittst. Sie soll sowas sagen wie \"Oh, no, this is Roy! He is in Trouble, I have to help him!\".",
      "6. Vor Elzas \"What was that?\" Möchte ich das du einen Scheiben zerspringen Sound abspielst.",
      "7. Bei Johns Text \"It's some Kind of Gorilla....\" schaut er ab da nicht mehr vernünftig zu Elza. Das musst du fixen.",
      "8. Nachdem die Monkeys tot sind, rennt John einfach raus. Das macht keinen Sinn. Da muss schon noch etwas kommen wie: \"John: Puuuh, that was close! I thought we end up as Gorilla meal….\". \"John: Elza, follow me, I think I might have found a way out of here!\". Alles natürlich mit passenden Animationen.",
      "Wenn man ROOM 1141 danach verlässt zu room 1171 kommt am anfang noch kurz die Panik Musik. Warum auch immer...",
    ],
    hinweise: "BAUT AUF Runde 38 F (analysis/befunde_runde38/F_dining1141.md + F_abnahme_*.md; engine/src/dining_r38.c, gen/dining_r38_programme.inc, Riegel test_r38_dining1141*). Nutzer-Aufnahmen synchro/STAGE1/room1141/help01..05.wav (2026-10-17 13:17-13:20) + main00-11, main40, main42. (1) Zombie-Geraeusche: Quelle messen (RE15_SE_DEBUG: Raumbank-Zombie-Saetze? Ambient-Haken?) und entfernen (es gibt keine Zombies in 1141). (2) John kommt durch die TUER: waehrend des Tueroeffnens Kamera vorne auf Elza (Kamera-Schnitte 1141 belegen), Tuer-zu-Ton, dann Kamera auf die Tuer mit John schon im Raum (NUTZER-VORGABE). (3) Johns 'shout' (Port-Ton aus Runde 38 F) entfernen. (4) statt dessen help01-05 in gemischter Folge mit Abstaenden, dazwischen mehrfaches Klopfen gegen eine Tuer (RE2-Klopf-/Haemmer-Ton belegen, RE2_SOUND_LISTE.md) — bis der Schalter gefunden ist (Ablauf Runde 38 F). (5) Elzas Reaktion beim ersten Hoeren nach dem Betreten (Port-Ids 1141 50-59, Bank 9 Bit 94/95): Wortlaut NUTZER-VORGABE, vom Nutzer am 2026-10-18 praezisiert (AUFTRAG.md Nachtrag): 'Oh, no, this is John! He is in Trouble, I have to help him!' — JOHN, nicht Roy ('Roy ist nur unten in den ROOM 1021 aktiv'). (6) Glasbruch-Ton vor 'What was that?' (RE2-Glasbruch, audio_pc.c GLAS1090.* vorhanden — Satz belegen). (7) John schaut ab 'It's some Kind of Gorilla....' nicht mehr zu Elza: Blick-/Koerperausrichtung (Plc_neck, memory reai-v2-plc-neck-verify, NPC-Ausrichtung) je Zeile. (8) nach dem Tod der Gorillas zwei neue Zeilen Johns (NUTZER-VORGABE, woertlich, mit passenden Animationen: Erleichterung, Heranwinken/Zeigen), dann geht John zur Dachtuer (Ablauf Runde 38 F) statt einfach rauszurennen. (9) beim Verlassen 1141 -> 1171 kurz Panik-Musik: BGM-Stopp/-Wechsel beim Raumwechsel (Sce_bgm_control, Musik aus Runde 38 F) — wo laeuft sie weiter/neu an, Original-Regel belegen. Riegel je Unterpunkt am echten Weg mit Elza." },
  { id: "M", thema: "schluessel2", baum: "r39_schluessel2",
    punkte: [
      "Und die Schlüssel im Communicationsroom sind immer noch zu groß, die sollten maximal halb so groß sein.",
    ],
    hinweise: "NUTZER-NACHTRAG 5 (AUFTRAG.md 2026-10-18 19:29). Vorstand = Spur L Punkt 2 (Welle 1, in master; Dossier analysis/befunde_runde39/L_optik.md Punkt 2 + L_abnahme_0.md): Parking Lot Key (Leon ROOM10F0) und Prison Key (Elza ROOM10F1) an der Pinnwand haengen auf RE2-Bildgroesse = oberes Quartil RE2_QUANTIL = 0.75 der RE2-Bildlaengen L_nat*H/vz desselben Modells an seinen RE2-Platzierungen (tools/r38_itemorte/itemorte_export.py Z.179/436/593 -> engine/src/gen/itemorte_r38_lage.inc RE15_R39L_* + itemorte_r38_md1.inc; Riegel unit_r39_optik_schluessel, integration_r39_optik_schluessel, test_r38_itemorte Schwellen). Gemessen in Abnahme L0 bei 960x720: Leon 69 Pixel / Huelle 14 x 8, Elza 63 Pixel / 16 x 10. NUTZER-VORGABE jetzt: 'maximal halb so gross' = lineare Groesse hoechstens 0,5 x dieses Stands (Flaeche ~1/4) fuer BEIDE Schluessel - als ausdrueckliche Nutzer-Zahl kennzeichnen (Konstante mit Herkunft 'NUTZER-VORGABE Nachtrag 5, 0,5 x RE2-Quartil-Groesse'), nicht als RE-Wert. SCHRITT 1 MESSEN vorher (Framedump RE15_WINDOW_SCALE=1 und Skala 3 an der Pinnwand, beide Szenarien, Pixelzahl + Huelle wie Abnahme L0). SCHRITT 2 Faktor einbauen (Generator + gen/*.inc neu erzeugen, nichts von Hand in gen/), Lage/Drehung/Aufhebe-Zone unveraendert. SCHRITT 3 nachher messen: Huelle linear <= 0,5 x vorher (beide), Schluessel weiter sichtbar (mindestens einige Pixel, Kontrast probe_r38_itemorte_kontrast) und aufnehmbar (Aufnahme-Ablauf Leon + Elza). Riegel nachziehen (unit_r39_optik_schluessel, integration_r39_optik_schluessel, r38_itemorte-Schwellen) mit Begruendung; vor dem Abschluss ALLE unit_/prop_/probe_ (Memory reai-v2-gezielte-integration-querbezug)." },
  { id: "O", thema: "parkplatz11c0", baum: "r39_parkplatz11c0",
    punkte: [
      "Ah, eine Sache fehlt mir noch - bei room11C0, während der Kampf gegen die Gorillas noch läuft, soll bei Leon der Text kommen \"I cannot leave Ada alone here with those monsters!\". Wenn Leon versucht durch die Tür aus dem Raum raus zu gehen.",
    ],
    hinweise: "NUTZER-NACHTRAG 9 (AUFTRAG.md). BAUM r39_parkplatz11c0 = master 1f403109 (enthaelt Runde 39 E: Gorilla-Regel HP 360 / Flinch ab 36 fuer ALLE Typ 0x27 inkl. 11C0, Querungs-Wache gegen Wand-Durchstoss). ROOM11C0 = Parkplatz im Leon-Szenario: RDT msg_sec 0x1C84 (re15_port/tools/rdt_msgdump.py) msg 5 'Leon: Ada, you hide inside that patrol car.', msg 6 'Ada: O-okay, I'll do that...', dann der Gorilla-Kampf, msg 7 'Ada: Okay, it's over now.'; Nutzer-Aufnahmen synchro/STAGE1/room11C0/main00-08. VORBILD Runde 38 F ROOM1141: Tueren gesperrt, solange die Gorillas leben, mit Text 'I cannot leave him alone with those monsters.' (include/re15_dining_r38.h RE15_R38F_MSG_1141_SPERRE = 41, tools/r38_dining/programme_bauen.py Z.351/473, gen/dining_r38_programme.inc Z.703, Riegel test_r38_dining1141*; Dossier analysis/befunde_runde38/F_dining1141.md). AUFGABE (1) MESSEN vorher am echten Weg (Leon in den Parkplatz, Szene msg 5/6, Kampf; waehrend des Kampfes jede Tuer von 11C0 benutzen - was passiert heute?), Tuer-AOTs des Raums (Door_aot_set-Slots) und die SCD-Stellen, die den Kampf beginnen und beenden (Flags/Bedingungen vor msg 7), aus dem RDT mit Offsets belegen; pruefen, ob die Elza-Variante ROOM11C1 Gorillas/Begleiter hat (der Nutzer spricht nur von Leon - Elza unveraendert lassen, wenn dort kein Kampf mit Begleiter ist). (2) FORM: dieselbe Sperr-Mechanik wie 1141 wiederverwenden (nicht neu erfinden); Bedingung = Kampf laeuft, Beginn/Ende aus den ORIGINAL-Flags/-Bedingungen des Raums (kein Port-Zaehler, wenn ein Original-Flag existiert); nach dem Kampf gehen die Tueren normal. NUTZER-VORGABE Text woertlich 'I cannot leave Ada alone here with those monsters!' - Textbox-Form wie 1141 msg 41 (Sprecher-Kopf 'Leon:' oder Erzaehltext: Form von 1141 uebernehmen und im Dossier begruenden; der Nutzer schreibt 'soll bei Leon der Text kommen'); Port-Message-Id 11C0 = 40 (VERTRAG 1.2), Sprachdatei des Nutzers kuenftig synchro/STAGE1/room11C0/main40.wav (Ordnerregel - im Dossier 'Fuer den Nutzer' GENAU diesen Pfad nennen). RE2-Beleg (Beta -> Retail): wie RE2 Tueren waehrend eines Pflichtkampfes bzw. mit wartendem Begleiter sperrt (RE2_Quellcode_Overlays / info/re2leon RDT-SCD) - Form belegen. Bank-9-Bit nur, wenn kein Original-Flag reicht: dann 96 (VERTRAG 1.1, in k_port[] von tests/unit/test_r36_kartenbits.c eintragen). (3) MESSUNG nachher am echten Weg: waehrend des Kampfes an JEDER Tuer des Raums -> Text, kein Raumwechsel, Leon bleibt steuerbar; nach dem Kampf (msg 7) gehen die Tueren; Kampf, Ada-Szene und Gorilla-Verhalten sonst unveraendert (Log/Framedump); Elza-Szenario unberuehrt. (4) Riegel unit + integration (Muster test_r38_dining1141*; parallel-/Linux-fest, eigene exe-Kopie mit Praefix). Vor dem Abschluss ALLE unit_/prop_/probe_ (Memory reai-v2-gezielte-integration-querbezug)." },
  { id: "P", thema: "johnruf1041", baum: "r39_johnruf1041",
    punkte: [
      "Eine letzte Änderung - Ich möchte das die Hilferufe von John erstmalig bereits erstmalig leise im ROOM 1040 kommen, nachdem John verschwunden ist. Also die Hilferufe sollen leise sein. dann soll bereits von Elza kommen: Oh, no, this is John! He is in Trouble, I have to help him! - nicht erst in ROOM 1041. Kopiere mir auch das voicefile stattdessen dort hin. Dann soll die Karte aufgehen und den ROOM 1141 markieren. Danach soll wieder die gleiche Panische finale Musik kommen wie bei Leon zum Ende - wenn Irons stirbt und er zu den gorillas muss. Wenn wir uns den Flur 1131 näheren, könnte das please help von John auch noch mal kommen, aber schon etwas lauter. Im ROOM 1141 soll alles so bleiben wie bisher, nur eben ohne die \"Oh, no, this is John! He is in Trouble, I have to help him!\" Cutscene.",
      "Die panische Musik hört auf, sobald man John gerettet hat. Der Rest bleibt normal.",
    ],
    hinweise: "NUTZER-NACHTRAG 10 (analysis/befunde_runde39/AUFTRAG.md; dort Lesart + Ablauf des Orchestrators, VERBINDLICH): Elza-Raeume = ROOMxxx1 ('ROOM 1040' = ROOM1041, 'nicht erst in ROOM 1041' meint 1141). Ablauf: John verschwindet in 1041 -> leise Hilferufe -> Elza msg 40 -> Karte mit 1141-Marker -> Panikphase (Bank-9-Bit 97 + Leons Schlussmusik) -> Zombie-Durchbrueche (Spur Q) -> 1121 Hund (Spur R) -> Flur 1131 lauterer Ruf -> 1141 ohne Reaktion (Spur J) -> John gerettet -> Bit 97 aus, Musik aus. Bit 97 setzt NUR Spur P; Q und R lesen es (in ihren Baeumen per Debug-/Flag-Haken setzen und das im Dossier zeigen). BAUM r39_johnruf1041 = Zweig r39/dining1141 (Spur J mitten in Nachbesserung 2; enthaelt Johns Hilferuf-Mechanik: engine/src/dining_r39.c Phase H, Se_on(0x3E,0x31..0x35) -> Sprach-Id 53..57 -> synchro/STAGE1/room1141/help01..05.wav ueber den Stamm-Haken in platform/pc/src/audio_pc.c re15_voice_load_clip, include/re15_dining_r39.h RE15_R39J_HILFE_ID0 / RE15_R39J_SATZ_HILFE0) + master (Stimme synchro/STAGE1/room1041/main40.wav = Kopie von room1141/main50.wav). Spur J arbeitet parallel weiter an 1141 (Johns Schuesse; 1141 OHNE Elzas Reaktion) - P aendert in 1141 NICHTS ausser dem Ende der Panikmusik bei Johns Rettung (raumunabhaengig am Original-Ende des Kampfes: beide Gorillas tot -> sub01 loescht (4,0xF2), Runde 38 F; am echten Weg belegen). Gemeinsame Stellen klein, Kommentar 'Runde 39 Spur P'. AUFGABE (1) MESSEN/BELEGEN: ROOM1041 (Elza) - wo/wann verschwindet John (RDT-SCD 1041; elzatoene_r38.c P1 Schuss @0x02200 'gunshot' + msg 2 'Elza: What was that!?'; Dossiers analysis/befunde_runde37 + runde38 zu 1041/1031/1021) - Ausloeser direkt danach. Leons Panikmusik: welche BGM nach Irons' Tod (irons_tod_1150.c Runde 35 L, montage_r38.c Runde 38 O, szene11c0_r37.c) und wie sie ueber Raumwechsel stehen bleibt - Form uebernehmen. Karte mit Marker: Form der Spur H 'nach Roys Tagebuch sofort die Karte mit Markern' (analysis/befunde_runde39/H_elza.md, engine/src/elza_r39.c), Marker ROOM1141. (2) UMSETZEN: 1041 nach Johns Verschwinden leise Hilferufe (help01..05; 'leise' = NUTZER-VORGABE, Faktor messen und begruenden), dann msg 40 'Elza: Oh, no, this is John! / He is in trouble, I have to help him!' (Textform wie Spur J msg 50 tools/r39_dining/programme_bauen.py:524, Stimme room1041/main40.wav per Ordnerregel), dann Karte mit 1141-Marker, beim Schliessen Panikphase: Bit 97 setzen (VERTRAG 1.1, k_port[] in tests/unit/test_r36_kartenbits.c), Panikmusik an. Flur 1131 in der Panikphase betreten: Johns 'please help' einmal, lauter als in 1041. Musik bleibt ueber alle Raeume bis zur Rettung, dann Bit 97 aus, Musik aus, normale Raum-BGM. (3) Nebenwirkungen: Leon unveraendert; Wiederbetreten 1041 ohne Wiederholung; Laden mitten in der Phase setzt die Musik fort (Bit 97 im Spielstand). (4) MESSUNG am echten Weg mit Elza (RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1): Mitschnitt (Rufe leise, main40, 1131 lauter), Karte per FB_SHOT, Musik bis zur Rettung. (5) Riegel unit + integration, parallel-/Linux-fest. Vor dem Abschluss ALLE unit_/prop_/probe_ + integration_r39_dining1141_d + integration_r38_dining1141*." },
  { id: "Q", thema: "elzazombies", baum: "r39_elzazombies",
    punkte: [
      "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM 1061 die Etage von unten über die Treppe erreicht aber möglichst weit weg an der Seite stehen. Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die Zombies nicht besiegt wurden und im ROOM 1031 über die Absperrung/Das Rollo, so wie bei der cutscene.",
    ],
    hinweise: "NUTZER-NACHTRAG 10 (analysis/befunde_runde39/AUFTRAG.md; dort Lesart + Ablauf des Orchestrators, VERBINDLICH): Elza-Raeume = ROOMxxx1 ('ROOM 1040' = ROOM1041, 'nicht erst in ROOM 1041' meint 1141). Ablauf: John verschwindet in 1041 -> leise Hilferufe -> Elza msg 40 -> Karte mit 1141-Marker -> Panikphase (Bank-9-Bit 97 + Leons Schlussmusik) -> Zombie-Durchbrueche (Spur Q) -> 1121 Hund (Spur R) -> Flur 1131 lauterer Ruf -> 1141 ohne Reaktion (Spur J) -> John gerettet -> Bit 97 aus, Musik aus. Bit 97 setzt NUR Spur P; Q und R lesen es (in ihren Baeumen per Debug-/Flag-Haken setzen und das im Dossier zeigen). BAUM r39_elzazombies = master. VORBILD Leon nach Irons' Tod: Montage mit Zombie-Durchbruechen (Runde 35 L engine/src/irons_tod_1150.c, Runde 38 O engine/src/montage_r38.c + include/re15_montage_r38.h 'in JEDEM Montage-Schnitt ... Rolltor ROOM1040 Cut 1', Runde 37 H zombieaktiv_r37.c; Dossiers analysis/befunde_runde35, 37, 38). AUFGABE (1) Leons Form messen/belegen: Raeume/Schnitte, Typen/Zahl, Aufstellung, wann sie in den Raeumen stehen, Bedingung 'nicht besiegt'. (2) Elza bei steigender Flanke von Bit 97: Durchbruch in den Flur von 10C1 aus, Zahl = so viele Zombies, wie in ROOM10E1 (medical room) am Leben gelassen wurden (Original-Gegner-Todesflags des Raums; Zensus Sce_em_set 10E1); ROOM1061 bei Ankunft von unten ueber die Treppe: sie stehen moeglichst weit weg an der Seite (Lage aus Kollision/Raumgeometrie belegen); ROOM1071 dasselbe wie bei Leon, falls die Zombies nicht besiegt wurden; ROOM1031 ueber die Absperrung/das Rollo wie in der Cutscene (welche Cutscene: belegen). Montage-Form wie Leon, Gegner-Verhalten RE2-KI wie bisher. Bank-9-Bit 95 nur, wenn kein Original-/Port-Flag reicht (VERTRAG 1.1, k_port[]). (3) Nebenwirkungen: Leon unveraendert; ohne Bit 97 keine Durchbrueche; Speichern/Laden. (4) Messung am echten Weg mit Elza (Framedump RE15_WINDOW_SCALE=1, Log), Riegel unit + integration. Vor dem Abschluss ALLE unit_/prop_/probe_." },
  { id: "R", thema: "hund1121", baum: "r39_hund1121",
    punkte: [
      "Und dann möchte ich, das in ROOM 1121, dort wo bei Leon die Krähe durch das Fenster fliegt, beim Durchlauf ein Hunde durch das Fenster rein springt.",
    ],
    hinweise: "NUTZER-NACHTRAG 10 (analysis/befunde_runde39/AUFTRAG.md; dort Lesart + Ablauf des Orchestrators, VERBINDLICH): Elza-Raeume = ROOMxxx1 ('ROOM 1040' = ROOM1041, 'nicht erst in ROOM 1041' meint 1141). Ablauf: John verschwindet in 1041 -> leise Hilferufe -> Elza msg 40 -> Karte mit 1141-Marker -> Panikphase (Bank-9-Bit 97 + Leons Schlussmusik) -> Zombie-Durchbrueche (Spur Q) -> 1121 Hund (Spur R) -> Flur 1131 lauterer Ruf -> 1141 ohne Reaktion (Spur J) -> John gerettet -> Bit 97 aus, Musik aus. Bit 97 setzt NUR Spur P; Q und R lesen es (in ihren Baeumen per Debug-/Flag-Haken setzen und das im Dossier zeigen). BAUM r39_hund1121 = master. VORBILD Leon ROOM1120: die Kraehe fliegt durchs Fenster (Fenster-Ereignis; Riegel integration_r35_fenster / integration_r36_elza_fenster, Kraehe engine/src/enemy_ai_re2_crow.c; Dossiers analysis/befunde_runde35/36). Hund = Typ 0x20 (engine/src/enemy_ai_re2_dog.c, Memory reai-v2-dog-ai); RE2-Form eines Fenstersprungs belegen (Beta -> Retail). AUSLOESER: Durchlauf ROOM1121 in der Elza-Panikphase (Bit 97), einmal; Ort = Fenster von Leons Kraehen-Ereignis in 1120 (Geometrie in 1121 belegen). AUFGABE (1) Leons Kraehen-Ereignis messen/belegen (Ausloeser, Fenster-Lage, Glas-Ton, Kamera). (2) Elza 1121: Hund springt durchs Fenster (Glas-Ton, Sprung-Animation aus der Hunde-EMD - Modell im Raum verfuegbar machen wie die EMD-/RBJ-Leihen anderer Spuren), danach normaler Hunde-Gegner. (3) Leon unveraendert, ohne Bit 97 kein Hund, nur einmal. (4) Messung am echten Weg mit Elza, Riegel unit + integration. Vor dem Abschluss ALLE unit_/prop_/probe_." },
  { id: "K", thema: "heliport1171", baum: "r39_heliport1171",
    punkte: [
      "Wenn John in Room 1171 sagt - \"Elza hurry up\" macht er so eine komische Kletter Animation statt der ran wink Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen.",
    ],
    hinweise: "ROOM1171 (Elza), Runde 38 I heliport1171 (analysis/befunde_runde38/I_heliport1171.md, engine/src/heliport1171_r38.c) und M npckante (Kendo-/John-Kanten). Szene sub07 @0x016C4..: John (Typ 0x49, Kendo-Modell) + Kraehen; sub02 @0x01398.. (Messages 'Elza, hurry up!' / 'We need to get this thing off of here! Help me out!' / 'C'mon, give me a hand!' / 'Alright, let's get down from here!'). Johns Clips je Szenenschritt (RE15_ANIM_TRACE) gegen die Original-Bytes (Plc_motion/Obj-Anim-Set/Work_set im 1171-SCD, Clip-Nummern im EMD des John-Modells; RE2-Kendo-EMD als Vergleich) — wo steht im Original der Heran-Winken-Clip, warum spielt der Port einen Kletter-Clip. 'Auch sonst': jede Szenen-Animation in 1171 pruefen und Abweichungen beheben. Spur A (Welle 1) hat die Stimme 'Alright, let's get down from here' bearbeitet — nicht anfassen. Riegel: Clip-Folge Johns je Zeile = Original, Gegenprobe Leon 1170 unveraendert." },
]

const REGELN = (s) => `
## Arbeitsbaum und Grenzen
- Dein Baum: ${ROOT}/.claude/worktrees/${s.baum} (Zweig r39/${s.thema}, Basis ${BASIS}). Zuerst: git -C "${ROOT}/.claude/worktrees/${s.baum}" status --porcelain (leer) und git log --oneline -1 (= ${BASIS} oder ein Merge von master in den Zweig; Fortsetzungs-Baeume tragen zusaetzlich die Vorgaenger-Commits).
- ALLE Lese-/Schreibzugriffe auf Projektdateien und ALLE git-Befehle NUR in diesem Baum (absolute Pfade mit dem Praefix ${ROOT}/.claude/worktrees/${s.baum}/ bzw. git -C "<baum>"; in Bash jeden Befehl mit cd "<baum>" beginnen). NIE Dateien unter ${ROOT}/ ausserhalb deines Baums aendern. Aus dem Hauptbaum nur LESEN, was der Baum nicht traegt: ${ROOT}/info/Resident_Evil_und_Playstation_Information/ und ${ROOT}/.agent_refs/.
- Pflichtlektuere ZUERST, vollstaendig: <baum>/analysis/befunde_runde39/AUFTRAG.md und VERTRAG.md (Zuteilung VERBINDLICH; nimm nur, was Spur ${s.id} zugeteilt ist). Vorrunde: <baum>/analysis/befunde_runde38/<SPUR>_*.md (Dossiers + Abnahmeberichte der Module, die du anfasst).
- Dossier <baum>/analysis/befunde_runde39/${s.id}_${s.thema}.md als ERSTES anlegen und FORTLAUFEND schreiben; nach JEDEM abgeschlossenen Schritt committen (wip(r39-${s.thema}): ... ausdruecklich erwuenscht, auch VOR Bau/Suite). Das Sitzungslimit beendet alle Agenten gleichzeitig — was nicht im Commit steht, ist verloren. Nur eigene Pfade adden (NIE -A), Rohbytes/Backticks per git commit -F <datei>.

## RE-Disziplin (CLAUDE.md STOP-GATE, Kurzfassung)
- Je Punkt: MESSEN/REPRODUZIEREN am gebauten Stand -> ORIGINAL disassemblieren (RE1.5; wo RE1.5 unfertig ist, RE2 Retail = Ziel, Beleg aus RE2; Sound ist RE2 — <baum>/RE2_SOUND_LISTE.md zuerst) -> Adresse/Bytes ins Dossier -> DANN Code -> VERIFIZIEREN durch erneutes Messen. Jede Konstante @0x... (oder Byte-Offset) im Code UND in der Commit-Message; Nutzer-Vorgaben als NUTZER-VORGABE / PORT-WAHL, ihre FORM belegt. Kein "deferred/tunable/interim/Env-Schalter" als Abschluss. Regressionen ZUERST bisecten (v0.8.24 e8b3958f / v0.8.25 2077bb8e / v0.8.26 (Tag v0.8.26), dann die Runde-38-Merges), verursachenden Commit nennen; der Fix muss den Befund ERKLAEREN. "Sieht richtig aus" ist kein Beleg; Nutzer-Beobachtung schlaegt Code-Hypothese.
- Quellen im Baum: RE_15_Quellcode_Overlays/STAGE1_full/ (0x8010xxxx; STAGE1/ ohne _full ist +0x800 verschoben), RE_15_Quellcode_V2/, ghidra1_V2.txt, RE15_FUN_CATALOG.md, RE15_KNOWLEDGE.md, STAGE1_FLAG_MAP.md, RE15_SCD_OPCODES_REFERENCE.md, RE2_SOUND_LISTE.md; RE2: info/re2leon/, ghidra_re2_Leon.txt, RE2_Quellcode_V2/, RE2_Quellcode_Overlays/. Werkzeuge .claude/skills/re15-psx-disasm/scripts/re15_disasm.py / re2_disasm.py; Skills .claude/skills/<name>/SKILL.md (rdt-analysis, scd-disassembly, ghidra-mapping, re15-psx-disasm, re15-room-probe, re15-room-capture, re15-port-visual-verify, re15-enemy-ai-re, re15-weapon-render, re15-parity-verify, re15-pc-render-order, re15-savestate-ghidra, re15-pcsx-watchpoint) ZUERST lesen. DuckStation-GDB fuer RE2-/RE1.5-Messungen: Muster in analysis/befunde_runde36/H_raeume.md und J_affen.md (EnableGDBServer in %LOCALAPPDATA%/DuckStation/settings.ini, danach zurueck). Memory-Notizen (reai-v2-...) unter C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/<name>.md (nur lesen).

## Bauen, Messen, Testen
- NUR so bauen: cd "<baum>" && bash re15_port/tools/local_build.sh configure (einmal und nach jeder NEUEN .c-Datei), danach ... build (test/all = volle Suite: im Spurbaum VERBOTEN, s.u.). NIE cmake --build aus Git-Bash. exe <baum>/re15_port/build/platform/pc/re15_pc.exe; befund.log/debug.log daneben. ⛔ KEINE volle Suite (local_build.sh test/all) im Spurbaum — gemessen 2026-10-10: fuenf parallele Suiten brauchten je ~80 min und lieferten Dutzende Last-Timeouts der Fensterhaken (unbrauchbar). Stattdessen GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = eigene Tests (r39_${s.thema}) + die Testgruppen JEDES Moduls/Raums, das dein Diff beruehrt (git -C "<baum>" diff ${BASE(s)} --name-only lesen; vorhandene Testnamen: ctest --test-dir ... -N | grep -i <modul/raum>). Die ctest-Summenzeile ("100% tests passed, 0 tests failed out of N") woertlich mit dem -R-Ausdruck ins Dossier. Die volle Suite faehrt der Orchestrator EINMAL im Integrationsbaum. Fenster-Haken flattern unter Last (boot_bg_pin, dark_start_pin, relatch_pin, save_counter_pin, weste_load_pin, r30_sicherung_*, r30_titel_puls, r34n_e_dokumente_bild; exe-Starts brechen selten mit exit 1 nach 0,1 s ab): einzeln 2-3x nachfahren (ctest --test-dir "<baum>/re15_port/build" -R "^<name>$"), nicht als Regression werten. NIE taskkill /IM re15_pc.exe (nur eigene PID); Messkopie der exe unter eigenem Namen neben der exe (Muster tests/integration/test_r34n_b_cursor.cmake). Nie zwei Builds gleichzeitig im eigenen build/ (ein zweites Bauverzeichnis fuer Bisects: RE15_BUILD_DIR=<baum>/re15_port/build_bisect).
- Harness: getenv("RE15_...") in platform/pc/main.c + engine/src (RE15_DEBUG_JUMP, RE15_GOTO_ROOM, RE15_FORCE_EVENT, RE15_FORCE_CUT, RE15_PLAYER_POS, RE15_EQUIP, RE15_GIVE, RE15_PRESS, RE15_PAD_AT, RE15_INPUT_SCRIPT, RE15_SET_FLAG(_AT), RE15_FIRE_AOT, RE15_FORCE_EXPLOSION, RE15_GRANATE_LOG, RE15_STATE_LOG, RE15_EVT_TRACE, RE15_SCD_TRACE, RE15_MSG_LOG, RE15_ENEMY_DBG, RE15_ANIM_TRACE, RE15_NECK_LOG, RE15_FX_LOG, RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC (Ton-Mitschnitt ohne Audiogeraet), RE15_FRAMEDUMP, RE15_EXIT_AT, RE15_KILL_AT, RE15_CONTINUE_TEST + RE15_CARD_AUTO, RE15_PSELECT_AUTO(_SWITCH) fuer Elza, RE15_CLIMB_TRACE, RE15_TUER_TEST/_SEITE, RE15_MAP_SHOT_PAGE, RE15_PRI_LOG) — Semantik im Code nachlesen. Bilder NUR per RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1 (gdigrab liefert weisse Bilder; Framedump schreibt Fensterskalierung). Story-/Raum-Punkte ueber den ECHTEN Weg abnehmen (Tuer betreten, Spielstand + CONTINUE), nicht nur per Sprung; Elza-Punkte mit Elza (RE15_PSELECT_AUTO).
- Tests: NEUE Sonden/Pins in re15_port/tests/unit/probes/r39_${s.thema}.cmake (GLOB; Muster probes/r38_*.cmake) + tests/unit/test_r39_${s.thema}*.c / tests/integration/test_r39_${s.thema}*.cmake (registriert NUR aus deiner probes-Datei). NICHT tests/unit/CMakeLists.txt und NICHT tests/integration/CMakeLists.txt. Mindestens ein Test je Nutzer-Punkt, der die MECHANIK misst. Arbeitsordner eines Tests nie gleich dem Target-Namen (Linux ohne .exe). Tests parallel- und Linux-fest (VERTRAG §2, Lehren des Release v0.8.26): exe-Kopie mit Praefix JE TEST (nie ein Glob, der Kopien eines Geschwister-Tests loescht), eigener Laufordner je Lauf, keine Umgebungsvariablen des Aufrufers erben; 'm' linken bei sqrt/sin/cos/atan2/pow; cmake_minimum_required(VERSION 3.16) in jedem -P-Skript; POSIX-Funktionen mit _DEFAULT_SOURCE.
- Dateien: neue Logik in NEUEN Dateien engine/src/<thema>_r39.c + include/re15_<thema>_r39.h; gemeinsame Dateien nur mit kleinen Haken (1-5 Zeilen) an klar benannten Stellen, Installer-Zeile am ENDE des Blocks in scd_room_setup.c mit Kommentar "Runde 39 Spur ${s.id}". KEIN Patch an shared_assets/PSX/*; neue Daten gen/*.inc oder shared_assets/RE2/ (im Dossier nennen). release/, platform/android/, RELEASE_NOTES.md: TABU.
- Token sparen: grosse Dateien nie ganz lesen (grep -n + Bereich), Logs gefiltert, Bau-/Suite-Ausgaben in Datei umleiten und nur Ende/Fehler lesen, keine Messung wiederholen, nur urteilstragende Bilder ansehen.

## Abschluss
- Dossier mit Abschnitten: Messung vorher (bei Regressionen: Bisect-Tabelle) / RE-Belege (Adressen, Bytes) / Umsetzung (Dateien, Konstanten) / Messung nachher / Tests (Namen, Ergebnis) / OFFEN / Fuer den Nutzer (neue Assets fuer das Paket-Gate, Sprachdateien, Bedienhinweise, Entscheidungsfragen). Letzter Commit fix/feat(r39-${s.thema}): ... Gezielte Tests gruen: ctest-Summenzeile + -R-Ausdruck woertlich zitieren (KEINE volle Suite). Antwort = StructuredOutput.
`

const bauPrompt = (s) => `Du bist Spur ${s.id} "${s.thema}" der Runde 39 (2026-10-18) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Du ermittelst, belegst, baust und testest ALLE Punkte dieser Spur im eigenen Arbeitsbaum. Ein anderer Agent nimmt danach unabhaengig gegen den Wortlaut des Nutzers ab — was du nicht gemessen und belegt hast, faellt dort durch.
${STAND[s.id] ? `
## ⛔ FORTSETZUNG — dein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST das Dossier, git -C "<baum>" log --stat ${BASE(s)}..HEAD und den Code-Diff gegen master. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg. Stelle je Nutzer-Punkt fest, was noch fehlt, und schliesse genau das ab; pruefe kritisch, ob der Stand den WORTLAUT erfuellt.
` : ''}
## Deine Punkte (Wortlaut des Nutzers, AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Einstiegspunkte (Stand 2026-10-18 — Zeilennummern und Behauptungen SELBST verifizieren)
${s.hinweise}
${REGELN(s)}
Arbeite jeden Punkt vollstaendig ab (Dossier je Punkt: Messung vorher / Beleg / Umsetzung / Messung nachher). Sackgasse: Weg wechseln (Xrefs, Caller/Callee, RE2, Savestate-RAM, GDB, Laufzeit-Log), nicht aufgeben, nicht raten. Am Ende: gezielte Tests gruen (keine volle Suite), alles committet, Dossier vollstaendig.`

const nachPrompt = (s, abn, runde, vorher) => `Du bist Spur ${s.id} "${s.thema}" der Runde 39 (2026-10-18) im Projekt reAi_v2 — NACHBESSERUNG ${runde} nach einer unabhaengigen Abnahme. Der Bau-Agent vor dir hat im selben Baum gearbeitet; sein Stand ist committet (lies git -C "<baum>" log --oneline ${BASE(s)}..HEAD, das Dossier und den Abnahmebericht <baum>/analysis/befunde_runde39/${s.id}_abnahme_${runde - 1}.md).

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
Punkte, die die Abnahme als erfuellt bewertet hat, NICHT verschlechtern (ihre Riegel gruen halten). Schreibe im Dossier einen Abschnitt "Nachbesserung ${runde}" (je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher). Baue neu, gezielte Tests gruen (keine volle Suite), committe (fix(r39-${s.thema}): Nachbesserung ${runde} ...).`

const abnahmePrompt = (s, bau, runde) => `Du bist die UNABHAENGIGE ABNAHME ${runde} fuer Spur ${s.id} "${s.thema}" der Runde 39 (2026-10-18) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Ein Bau-Agent behauptet, die Punkte des Nutzers umgesetzt zu haben. Du glaubst ihm NICHTS, was du nicht selbst am gebauten Stand gemessen hast. Der Nutzer weist jede Aenderung ohne Disasm-Beleg zurueck — du bist seine Instanz.

## Die Punkte des Nutzers (Wortlaut; AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Behauptung des Bau-Agenten
Status: ${bau ? bau.status : 'KEIN ERGEBNIS (Agent abgebrochen) — pruefe Baum und Dossier trotzdem'}
${bau ? bau.zusammenfassung : ''}
Commits: ${bau && bau.commits ? bau.commits.join(', ') : '?'}
Suite laut Bau-Agent: ${bau ? bau.suite : '?'}
Offen laut Bau-Agent: ${bau && bau.offene_punkte && bau.offene_punkte.length ? bau.offene_punkte.join(' | ') : '(nichts)'}

## Baum
${ROOT}/.claude/worktrees/${s.baum} (Zweig r39/${s.thema}, Basis ${BASIS}). ALLE Zugriffe und git-Befehle nur dort (absolute Pfade, git -C "<baum>", Bash mit cd "<baum>"). Nichts ausserhalb des Baums aendern. Aus dem Hauptbaum ${ROOT}/ nur lesen (info/Resident_Evil_und_Playstation_Information/, .agent_refs/).

## Vorgehen
1. Lies <baum>/analysis/befunde_runde39/AUFTRAG.md, VERTRAG.md, das Dossier ${s.id}_${s.thema}.md und: git -C "<baum>" log --oneline ${BASE(s)}..HEAD; git -C "<baum>" diff ${BASE(s)} --stat; git -C "<baum>" diff ${BASE(s)} -- re15_port (den Code-Diff WIRKLICH lesen).
2. Baue: cd "<baum>" && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build (nie cmake --build aus Git-Bash). KEINE volle Suite (test/all) im Spurbaum — die faehrt der Orchestrator einmal im Integrationsbaum. Fahre GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = Tests der Spur (r39_${s.thema}) + die Testgruppen aller Module/Raeume, die der Diff beruehrt (git diff ${BASE(s)} --name-only; ctest -N | grep -i). Rote Fenster-Haken unter Last 2-3x einzeln nachfahren, bevor du sie wertest. NIE taskkill /IM re15_pc.exe; Messkopie der exe unter eigenem Namen (Muster tests/integration/test_r34n_b_cursor.cmake).
3. MISS jeden Nutzer-Punkt selbst an der exe (<baum>/re15_port/build/platform/pc/re15_pc.exe; Haken getenv("RE15_...") — Semantik im Code nachlesen; Spielstand + CONTINUE, RE15_DEBUG_JUMP, RE15_FORCE_EVENT, RE15_PLAYER_POS, RE15_PRESS/RE15_PAD_AT/RE15_INPUT_SCRIPT, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, Logs RE15_STATE_LOG/RE15_EVT_TRACE/RE15_ENEMY_DBG/RE15_FX_LOG/RE15_SE_DEBUG/RE15_AUDIO_CAP_SYNC usw.; Elza-Punkte mit RE15_PSELECT_AUTO). Story-/Raum-Punkte ueber den ECHTEN Weg (Tuer betreten), nicht nur per Sprung. Bei Regressionen: Gegenprobe mit der v0.8.26-exe, ob der genannte verursachende Commit den Befund wirklich erklaert. Urteil je Punkt gegen den WORTLAUT: erfuellt / teilweise / nicht_erfuellt / nicht_pruefbar (mit Grund) — mit Messprotokoll (Befehl, Log-Zeilen, Bild-Pfad).
4. RE-Gate: Jede verhaltensrelevante Konstante im Diff hat @0x... (oder Byte-Offset) ODER ist als NUTZER-VORGABE / PORT-WAHL gekennzeichnet; stichprobenartig 3 Adressen SELBST disassemblieren (re15_disasm.py / re2_disasm.py / ghidra*.txt) und pruefen, dass die zitierte Stelle das Behauptete enthaelt (Sprungziel disassemblieren, richtige Binaerdatei). Suche im Diff nach "deferred", "tunable", "interim", "for now", "faithful", "plausibel", "TODO", Env-Schaltern als Abschluss. Pruefe, ob der Fix den gemessenen Befund ERKLAERT.
5. Vertrag: nur zugeteilte Bank-9-Bits/Nachrichten-IDs/Slots/Ereignisse (VERTRAG.md); erlaubte Pfade (git diff ${BASE(s)} --name-only: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists); gemeinsame Dateien nur kleine Haken; Tests je Punkt vorhanden und messend (ctest --test-dir "<baum>/re15_port/build" -R "r39_${s.thema}" laufen lassen). Pruefe auch Nebenwirkungen auf das, was die Spur beruehrt (z.B. andere Raeume, beide Szenarien).
6. Schreibe den Bericht <baum>/analysis/befunde_runde39/${s.id}_abnahme_${runde}.md (je Punkt Messprotokoll, Urteil, Beleg; Gates; Maengel nummeriert, konkret, nachpruefbar) und committe NUR diese Datei: git -C "<baum>" add analysis/befunde_runde39/${s.id}_abnahme_${runde}.md && git -C "<baum>" commit -m "doc(r39-${s.thema}): Abnahme ${runde}". Du aenderst KEINEN Code. Schreibe den Bericht FORTLAUFEND (nach jedem gemessenen Punkt speichern) — das Sitzungslimit kann dich jederzeit beenden.
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