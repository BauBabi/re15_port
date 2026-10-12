export const meta = {
  name: 'r37-runde-k',
  description: 'Runde 37 Spur K: Bau-Spur je Ermittlung+Bau -> unabhaengige Abnahme -> Nachbesserung, in eigenen Arbeitsbaeumen (Semaphor)',
  phases: [
    { title: 'Ermittlung+Bau', detail: 'Dossier, Messung/Bisect, RE-Beleg, Code, Tests, Suite im eigenen Baum' },
    { title: 'Abnahme', detail: 'unabhaengig gegen den Wortlaut des Nutzers am gebauten Stand gemessen' },
    { title: 'Nachbesserung', detail: 'Maengel der Abnahme beheben, erneut messen' },
  ],
}

const ROOT = 'C:/workspace/git/reAi_v2'
const BASIS = '3feb3a78'
const SLOTS = 4
const MIN_TESTS = 698

const SPUREN = [
  { id: "A", thema: "pri1100", baum: "r37_pri1100",
    punkte: [
      "Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker. Die Marker Datei liegt da wo immer die Dateien liegen mit F9.",
    ],
    hinweise: "Die F9-Marken des Nutzers liegen im HAUPTBAUM (nicht in deinem Baum): C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_1100_F892_marke1.bmp (2026-10-12 10:07, v0.8.24-Stand; 960x720 BMP, mit PowerShell System.Drawing in PNG wandeln) und befund_1100_F240_marke1.bmp (2026-10-05, vor Runde 36). Die Zahlen dazu stehen in C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund.log, Block '=========== MARKE 1 (F9) ===========' (grep -n 'MARKE' ; ab Zeile ~15850): F892 R1100 C1, Leon pos=(-14914,0,-25645) rot=4488 vz=12487/12358/12228, alle 105 Masken dieses Winkels mit Bildlage/Tiefe, Gegner: Zombie 2 typ 0x10 LIEGEND pos=(-14269,0,-25091) (dist 849), Zombie 1 (-16157,0,-23876). Im Bild F892 liegt Zombie 2 quer in der Tuer am Ende des Gangs, sein Kopf liegt an/ueber der rechten Tuerrahmen-Kante (etwa x 168..178 / y 140..148 im 320x240-Bild) und wird ueber die davorliegende rechte Wand gezeichnet; F240 zeigt dasselbe Muster (nur der Kopf eines liegenden Zombies ueber der rechten Wand sichtbar). ⛔ Spur O der Runde 36 hat die Marke nie gesehen (falsch gesucht) und in Abschnitt 2 (analysis/befunde_runde36/O_pri.md, '2.2 e)' und '2.5': 'Kopf des kriechenden Zombies wieder sichtbar' an der Suedecke (-15500,-23700)) moeglicherweise genau die falsche Richtung gebaut — PRUEFEN. Daten: shared_assets/PSX/MASKS/ROOM1100.MSK + ROOM1100_PRI01/02.TIM (+ .PBM/.STAND/.BELEG), Werkzeuge tools/maske/, render_pc.c (Masken-Tiefentest, Schatten-Bucket der Spur O), Lehren im Memory: Maske = Tiefen-PROFIL je Zeile, 'deckt ab' != 'verdeckt', flache Maske verdeckt nichts. Vorgehen: Szene exakt nachstellen (Leon + liegender Zombie an derselben Stelle, RE15_PLAYER_POS/Gegner-Haken oder Kartenwerkzeug), Framedump (RE15_WINDOW_SCALE=1), je Pixel des Kopfes: Figur-Tiefe gegen Masken-Tiefe; geometrisch belegen, ob der Kopf HINTER der Wandebene liegt (Welt- und Kameradaten, SCA-Zellen der Wand); dann die Maske/Tiefe korrigieren und mit BEIDEN Marken (F892, F240) und den Faellen der Spur O (Schatten am Wandfuss Cut 1/2, Zombie hinter der Nordecke) gegenpruefen, dass nichts anderes kippt. Original-Vergleich, wo moeglich (stage_saves/ mit ROOM1100, sauber = nicht PATCHED-EXE_*)." },
  { id: "C", thema: "fahrstuhl", baum: "r37_fahrstuhl",
    punkte: [
      "Die Fahrstuhl Tür in der Tür Animation ist noch nicht korrekt. Da musst du noch mal schauen ... wenn es keine passende Fahrstuhl Tür gibt, müssen wir die wieder bauen....",
      "Beim Aussteigen aus dem Fahrstuhl fehlt noch die Türsequenz. << stimmt.",
      "Nachtrag 5: ja, dann mache den noch (Ausstieg Lastenaufzug ROOM3080 -> 4000). Mir ist auch aufgefallen in den Stage2 kleineres aufzügen fehlt noch das sound. Den sollte man auch aus Resident Evil 2 extrahieren.",
    ],
    hinweise: "Stand v0.8.24 (Runde 36 Spur G, analysis/befunde_runde36/G_tueren.md + G_abnahme_0.md H1/H3/X1): der Einstieg in die Kabine ROOM1080 spielt RE2 DOOR27 (RE2-LABOR-Fahrstuhl, oliv-grau mit Bedienfeld, zwei Fluegel aus der Mitte) — der Nutzer sagt: nicht korrekt. Die Runde-33-Port-Archive (P25G = Hub, Farbe passend) hatten die falsche Mechanik. Schritt 1: den GEMALTEN RE1.5-Fahrstuhl in den Hintergruenden belegen (Kabine 1080, die Etagen-Raeume mit der Fahrstuhltuer, Cuts; Farbe, Fluegelzahl, Oeffnungsrichtung, Rahmen, Bedienfeld) und die RE1.5-eigene Tuer-Zuordnung (welches DO2 spielte das Original fuer 1080? info/Re1.5/PSX/DOOR + Tuer-Tabelle der EXE). Schritt 2: ALLE RE2-Tuerarchive (shared_assets/RE2/DOOR/*.DO2, info/re2leon/) als Bogen rendern (RE15_TUER_SEITE, G_belege/) und eine wirklich passende Fahrstuhl-Tuer suchen. Gibt es keine: eigenes Port-Archiv bauen (Rezept-Weg wie Runde 33/36: tools/tueren/, r36_rezept_p0cd.py, shared_assets/RE15DOOR/, FNV in gen/re15_tuer_eigen.inc) mit Fahrstuhl-Mechanik und der Optik des gemalten RE1.5-Fahrstuhls; Bildvergleich gegen die Hintergruende. Schritt 3 Ausstieg: ROOM1080/4020 verlassen die Kabine per Skript (Null-Rechteck-Tueren ROOM1080.RDT @0x00482/@0x004A2/@0x004C2, Aot_on sub08 @0x0080E) ohne Sequenz; RE2 spielt beim Verlassen der Kabine DOOR27 V0 (ROOM6010 @0x004C2/@0x004E2/@0x00502; Skript-Tueren mit Archiv: ROOM21B0 sub03 @0x026EE). Ausstieg mit derselben (korrigierten) Tuer, Richtung/Variante belegen. Code: door_scene_pc.c, door_seq_*.c, gen/tuer_zuordnung.inc, gen/re15_tuer_eigen.inc; Ton RE2 (RE2_SOUND_LISTE.md, ELEVSE.VBS liegt schon unter shared_assets/RE2/). Auch ROOM4020 (Labor-Fahrstuhl) und Elzas Varianten pruefen. Bilder: Bogen/Framedump je Tuer im Dossier, Vergleich zum Hintergrund." },
  { id: "D", thema: "itemband", baum: "r37_itemband",
    punkte: [
      "Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden.",
    ],
    hinweise: "Befund: analysis/befunde_runde36/R_abnahme_3.md M1/M2/H4 und NACHLAUF.md #73/#74. (M1) Der AOT-Scan FUN_80042bac prueft im Original fuer JEDEN Eintrag das Band: @0x80042cac-ccc rec[2] == Spieler+0x82 (sonst naechster Eintrag); der Port wendet das Band-Gate nicht auf ITEM an (aot_common.c). (M2) Member_set 0x12 (FUN_8004116c, Tabelle 0x80010c8c[18] -> 0x80041228 sb a2,130(a0) = DAT_800acad6 = Band der Schubkette @0x8002bf38 und des AOT-Scans @0x80042cc0) setzt im Port nur g_actors.floor (actor_common.c re15_actor_set_member case 18), nicht das Kollisionsband (s_coll_band in re15_collision.c) -> ROOM3050-Lift (sub04 @0x023D0, (3,82)=1) laesst Leon auf Band 4. NUR GEMEINSAM beheben (sonst waere die Super Redhawk unerreichbar). Zensus aller Item_aot_set mit floor != 0 (12: 2030/2031, 2040/2041, 2090/2091, 3010/3011, 3050/3051 — selbst nachzaehlen, scd_dump_room.py) und aller Member_set 0x12 in allen RDTs; je Item an der exe belegen, dass es auf dem VORGESEHENEN Weg erreichbar bleibt (3050: Kiste schieben -> Fall -> Lift -> Band 0 -> Kerbe slot 7 @0x01F06 -> Latch (3,93) sub01 @0x02184-2196 -> Aufstieg -> Super Redhawk (9,92)) und vom falschen Band NICHT aufnehmbar ist; Original-Messung am Original wie in Spur R (R_belege/orig_*.py, DuckStation-GDB, settings.ini EnableGDBServer danach wieder false). Riegel: Zensus + Verhaltens-Pins + Gegenprobe. Achtung: das Band steuert auch Treppen/Klettern/Schub — Spur-R/A-Riegel (unit_r36_dachkiste_*, unit_r36_klettern*, unit_r36_objfall*, integration_r36_dachkiste) gruen halten." },
  { id: "E", thema: "codepanel", baum: "r37_codepanel",
    punkte: [
      "bei den Code panels möchte ich das, so lange der Code noch falsch eingegeben ist unten das blaue Licht leuchtet. Ist er dann richtig eingegeben das grüne Licht.",
      "Die Kamera darf noch auf den Panels bleiben wenn der Piep und unlock sound kommt.",
      "Nachtrag 3 (Korrektur): Nein, beim Panel in korridor 2 ist falsch - dort soll natürlich rot leuchten - richtig eingegeben - dann blau.... / rot = geschlossen",
      "Nachtrag 3b: Es muss schon in der richtigen Farbe leuchten das Panel... hintergrund rot - rot leuchten - hintergrund blau - blau leuchten...",
    ],
    hinweise: "Code-Panel-Raeume (Runde 36 Spur G, analysis/befunde_runde36/G_tueren.md Z.238ff): Ziffernfeld + Karte in 10D0/10D1 (Code 4312, Blue Keycard), 11E0/11E1 ('Prisons', Yellow Keycard + 0513), 1230/1231 (Zettel 5632); dazu das Minidisc-Schloss 1100/1101 (Cursor-Raetsel) — vollstaendigen Zensus aller Ziffern-/Code-Panels aller Stages selbst machen (Ck(5,0x0D..0x10)-Muster, Cursor-Raster-Raeume). Die Folge Piep/Aufschliessen (0/15/30 Bilder) baute Spur G (engine/src/aufschliessen_r36.c, gen/r36_aufschliessen.inc, shared_assets/RE2/KARTSE.VBS). Lampen: im Panel-Bild unten blau und gruen (Hintergrund-Cut des Panels ansehen und belegen, wo die Lampen gemalt sind, ob als an/aus); Vorbild fuer gezeichnete Lampen im Port: Generator ROOM11F0 (panel_lampen_pc.c + shared_assets/RE2/LAMPE2130.TIM aus RE2 ROOM2130). RE2 als Vorbild pruefen (Beta -> Retail): gibt es in RE2 Code-/Kartenpanels mit Lampe rot/blau/gruen (RDT-ESP/TIM, Skript)? Dann deren Mechanik/Texturen; sonst NUTZER-VORGABE mit belegter Lampenlage. Kamera: heute schaltet die Kamera vor dem Piep/Aufschliessen zurueck (Cut_old/Cut_auto des Panel-Skripts) — die Ruecknahme bis nach dem Aufschliess-Ton verschieben (Bildnummern im Log: Piep, Aufschliessen, Kamerawechsel), ohne die Tuer-/Skriptlogik zu aendern. Messen mit RE15_SE_DEBUG/RE15_AUDIO_CAP_S, RE15_EVT_TRACE, Framedumps je Zustand (falsch/teilweise/richtig)." },
  { id: "F", thema: "sperre1050", baum: "r37_sperre1050",
    punkte: [
      "bei Leon, bevor dem Ende wo er noch zu Irons muss habe ich eine Lücke gefunden - man kann über die Luke bis nach F1 und kann den \"Ich muss zuerst irons holen\" Blocker bei der Tür überspringen. Ich schlage vor - du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene triggern, wenn wir Ada noch nicht gerettet haben.",
      "Ja, neuen Texte sind super. Bezug zu ada bei elza kann raus.",
    ],
    hinweise: "Sperre heute: include/re15_tuer1060.h + engine/src/tuer1060*.c (ROOM1060 Slot 2 -> ROOM1040, Text 'I have to get the Chief first...', Bedingung (9,73)=0 UND ((9,71)=1 ODER (3,94)=0)). Ada-Tuer: include/re15_adaruf.h + engine/src/adaruf_1050.c (ROOM1050 Slot 4 -> ROOM10A0; vor der Ada-Rettung (3,0xBB)=0: Szene Ereignis 13 bzw. Sperrtext msg 25; danach Original-Tuer). Schritt 1: die Luecke des Nutzers REPRODUZIEREN (Leon im Endspiel-Zustand: 10F0-Szene gesehen (9,71)=1, Irons noch nicht geholt (9,73)=0) — welche 'Luke' fuehrt nach F1 an der 1060-Sperre vorbei, Weg Ende-zu-Ende an der exe; Zensus aller Wege von dort zum Ende von Stage 1 (11B0/11C0), damit die neue Sperre die Luecke wirklich schliesst. Schritt 2: an ROOM1050 Slot 4 nach der Ada-Rettung denselben Text wie 1060 zeigen, solange Leon den Chief noch holen muss (Bedingung im Dossier begruenden: Fruehspiel darf NICHT gesperrt werden, dort gilt die Ada-Logik; Elza (ROOM1051) bleibt offen). Nachrichten-Id laut VERTRAG.md 1.2 (26..28), Form wie re15_tuer1060.h (Text-Platz sce 1). Schritt 3 Text Elza ROOM50B1 msg 0 'It's Ada. She's dead.It looks like one of those / gorillas killed her.' ohne Ada-Bezug (NUTZER-VORGABE), Weg der Spur P (tools/r36_p/texte_bauen.py, gen/story_r36_texte.inc, engine/src/story_r36.c), Wortlaut PORT-WAHL in RE2-Form (z.B. 'It's a woman. She's dead.'), Zeilenumbrueche wie das Original; Sweep, ob Elza an weiteren Stellen 'Ada' sagt/liest." },
  { id: "G", thema: "beretta", baum: "r37_beretta",
    punkte: [
      "bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden. Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.",
    ],
    hinweise: "Vorarbeit: analysis/waffen_fsm_2026-09-12/ (SPEC.md Z.273/320: Waffen 5/6 = M93R/Glock 18 3-Burst, Entlade-Handler 0x800338A8 ueber Tabelle @0x80074114/18, ammo_per_shot 3 @0x80033974-84; entlade-granaten-burst.md §2: drei FUN_80019700-Spawns = Muendungs-Effekte). Runde 36 Spur C (analysis/befunde_runde36/C_schuss.md, engine/src/schuss_r36.c): Schusspfad FUN_80011f50 @0x80012168-70, Sichtlinie FUN_8001b9b4, RE2-Treffer-Resolver @0x80041270-9c. Schritt 1 MESSEN: an der exe (Waffe 5, RE15_EQUIP/RE15_GIVE) einen Zombie (RE1.5-KI und RE2-KI) mit EINEM Feuerstoss treffen — wie viele Treffer, wie viel Schaden (HP vorher/nachher, RE15_ENEMY_DBG), wie viele Treffer-Reaktionen/Blut, wie viel Munition; dasselbe fuer Waffe 6 (Glock 18) und zum Vergleich Browning (Einzelschuss). Schritt 2 ORIGINAL: im RE1.5-Original (DuckStation-GDB wie Spur R, stage_saves/ sauber; Waffe per RAM/Debug-Menue) messen, ob der Feuerstoss DREI Treffer-Aufloesungen macht (Haltepunkt im Treffer-Pfad je Schuss) und welchen Schaden; disassemblieren, wo der Burst den Schuss-Pfad dreimal oder einmal ruft. RE1.5 dreimal -> Port byte-true nachziehen. RE1.5 einmal -> NUTZER-VORGABE: jeder der drei Schuesse loest einen eigenen Treffer aus (Schaden je Schuss wie der Einzelschuss der Waffe aus der Schadenstabelle, Reaktion/Blut je Treffer), Zeitpunkte je Schuss aus dem Clip der Waffe belegen. RE2/RE3-Vorbild fuer Feuerstoesse pruefen (Beta -> Retail). Riegel: Treffer je Feuerstoss = 3 (Leon und Elza), Munition -3, Gegenprobe; Dauerfeuer-Waffen (Ingram/MC51) und Einzelschuss-Waffen unveraendert (Runde-36-C-Riegel gruen halten)." },
  { id: "H", thema: "zombieaktiv", baum: "r37_zombieaktiv",
    punkte: [
      "das soll nicht nur da gelten, das darf generell für die Zombies gelten — schaue einfach nach wie Resident Evil 2 das macht (Nutzer-Ergaenzung: gilt fuer ALLE Zombies, Vorlage RE2)",
      "die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang.",
    ],
    hinweise: "UMFANG (Nutzer): GENERELL alle Zombies — RE1.5-Typen (0x10/0x11/0x16/... im Port mit RE2-KI als Standard, enemy_ai_re2_zombie.c) in JEDEM Raum und jeder Spawn-Art; Vorlage ist die RE2-Retail-Zombie-KI: wie sie im Gang wandern, wann sie loslaufen, auf Leon zukommen (Zustandsautomat, Weck-/Sichtbedingungen, Wander-Ziele, Tempo) — aus RE2 belegen (RE2_Quellcode_Overlays/, ghidra_re2_Leon.txt, info/re2leon/ RDTs: Sce_em_set-Bytes der RE2-Gang-Zombies) und am RE2-Original messen, wo moeglich (DuckStation mit RE2, Memory reai-v2-re2-trace-datei). Erst feststellen, was der Port von der RE2-KI schon hat und was fehlt (z.B. Wander-Zustand, Weckradius, Annaeherung ohne Sicht, Spawn-Variante), dann das Fehlende byte-true nachbauen. Beispiel des Nutzers: Montage nach Irons' Tod:  include/re15_irons_tod.h + engine/src/irons_tod_1150.c (Runde 35 Spur L, Runde 36 Spur K: analysis/befunde_runde36/K_szene1150.md; Kette 1150 -> [1130 Cut 0, wenn in 1140 noch Zombies leben] -> 1040 Cut 1 -> 1030 (Cut 7, wenn in 1070 noch Zombies leben; Cut 6) -> 11C0 Cut 13 -> 1150; Spawn-Bits (9,74)/(9,75)/(9,76) fuer 1130/1040/1030, Records 15..19 seit Runde 36 Spur E (20 Gegner)). Schritt 1 MESSEN am ECHTEN Weg: Leon nach der Montage durch 1130/1040/1030/1070 (Spielstand + CONTINUE oder Kartenwerkzeug wie Spur E probe_r36_gegnerzahl_karte), je Zombie ueber mehrere Sekunden: Zustand/Unterzustand, Sicht/Weck-Status, Geschwindigkeit, Abstand zu Leon (RE15_ENEMY_DBG, RE15_STATE_LOG, befund.log-Gegnerliste), Framedumps. Wer steht warum? (Weck-Radius, Sicht, Spawn-Byte des Montage-Satzes (Sce_em_set beh / pc[6], Memory: pc[6] NICHT ohne Messung anfassen), AI-Freeze-Gate (DAT_800aca40 & 0x20000000 / entity+0x9 & 0x20) nach der Szene nicht geloescht, Szenen-Flags, Tick-Budget bei vielen Gegnern?). Schritt 2 ORIGINAL: was tun die Zombies dort im RE1.5-Original (Montage gibt es dort so nicht — PORT-Szene; Vergleich mit den Zombies derselben Raeume im Original-Normalspiel und mit RE2: RE2-Zombie-KI enemy_ai_re2_zombie.c, RE2 EM10/EMZ0 in RE2_Quellcode_Overlays/ und ghidra_re2_Leon.txt: Wander-/Annaeherungszustaende im Gang, Weckbedingungen). Ziel Beta -> Retail: wie RE2 (teilweise aktiv auf Leon zu), mit Beleg. Runde 36 Spur F (analysis/befunde_runde36/F_zombieki.md, Riegel unit_r36_zombieki_*, Eintritts-Zensus test_r36_zombieki_eintritt) gruen halten; Spur-E-Riegel (integration_r36_gegnerzahl) und die Montage-Riegel (integration_r35_cut1150, unit_r36_szene1150*) gruen halten. Riegel: nach der Montage bewegt sich in 1130/1040 ein Anteil der Zombies binnen N Bildern auf Leon zu (N aus RE2 belegt), Gegenprobe Stand v0.8.24." },
  { id: "I", thema: "feuerloescher", baum: "r37_feuerloescher",
    punkte: [
      "Bitte erstelle mir zusätzlich auch ein Modell des Feuerlöschers. Ich finde deine Idee, bei elza einen Leeren Feuerlöscher am Boden zu haben gut. Der müsste dann bei Leon nach der Nutzung erstmalig auftauchen.",
      "Außerdem finde ich den FEuerlöscher im Spind bei Leon noch unglücklich und würde ihn gerne woanders platzieren....",
      "Nachtrag 4b: ah, it can only lay there - if leon when he is asked to discard it select \"yes\".",
      "Nachtrag 4c: If he doesn't he has the text empty fire extinguisher in the player inventory.",
    ],
    hinweise: "Vorarbeit: extracted_re2_feuerloescher/README.md + analysis/befunde_2026-09-21/re2-feuerloescher-modell.md: es gibt KEIN Feuerloescher-Modell in RE2 oder RE1.5; vorhanden sind das RE1.5-Item-Bild (info/Re1.5/PSX/ITEM/ITPS.ITP file@0x093000, 112x72 8bpp, Item 0x31: roter Loescher, Chromgriff, weisses Etikett; Decoder engine/src/itps_common.c) und vier Basis-Netze (extracted_re2_feuerloescher/basis_kandidaten/: RE1.5 ROOM1010 m00 Spraydose @0xAF4/@0x22D28 und ROOM4050 m03 rote Spraydose @0x3788/@0x2DDA8 = beste Basis, RE2 ROOM1010 m03 / 60D0 m02). MODELL BAUEN (PORT-WAHL, im Dossier begruenden): Netz eines Feuerloeschers (Zylinderkoerper, Schulter, Ventil mit Griff/Hebel, Schlauch/Duese) in PSX-Massstab (Hoehe aus der realen Groesse und der Leon-Groesse abgeleitet, Memory reai-v2-modellmass-aus-kamera), Textur aus dem ITPS-Bild (rot, Chrom, weisses Etikett) als 4/8-bpp-TIM, eingebettet wie die Lageplaene der Spur N (engine/src/gen/kartenitems_r36.inc: MD1 + TIM als gen/*.inc, Prop wie Obj_model_set, Licht flach/NCCT wie das Original fuer solche Objekte belegen). Sichtpruefung mit Framedumps aus mehreren Kameras + Vergleich mit dem Item-Bild. EINSATZ: (a) Elza ROOM1091 (Schuppen, Text der Spur P 'Someone must have put out the fire.' msg 20): leerer Loescher liegt von Anfang an am Boden (liegend, plausibler Ort im Schuppen, sichtbar in den Cuts; optional Untersuchen-Text als PORT-WAHL zur Freigabe). (b) Leon ROOM1090: derselbe leere Loescher liegt erst NACH dem Loeschen ((3,0x84) = 1, ROOM1090 sub06 @0x02722; vorher nicht), danach dauerhaft (Wiederbetreten, Spielstand). (c) Leons Feuerloescher (heute ROOM1000 Bath-Lockers Spind: sub00 @0x00C24 Item 0x31 Taken (9,0x86), sub01 @0x00D0C Set(3,0x85)) an einen besseren Ort verlegen: sichtbar mit dem neuen Modell (z.B. Wandhalterung in einem Gang auf Leons Weg zu 1090 nach dem Hilferuf in 1050), Taken-Bit (9,0x86) und Set(3,0x85) unveraendert (Spielstaende!), Original-Spind-AOT in 1000 auf Leons Seite stilllegen bzw. umwidmen (Elzas 1001 bleibt mit 30 Patronen), Ort mit Bildern begruenden und in 'Zu entscheiden' fuer den Nutzer nennen. Elza-Raeume: 1091 hat keinen Loescher-Gegenstand. Riegel: Prop vorhanden/nicht vorhanden je Zustand (1091 immer, 1090 erst nach (3,0x84)), Aufnahme am neuen Ort setzt (9,0x86)+(3,0x85) wie das Original, Feuer 1090 laesst sich damit loeschen (Weg Ende-zu-Ende an der exe), Paket-Gate-Eintraege fuer neue Assets nennen." },
  { id: "J", thema: "bissarm", baum: "r37_bissarm",
    punkte: [
      "Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird.",
    ],
    hinweise: "Keine F9-Marke dazu — beide Lesarten messen: (1) der Zombie-Arm, der beim Biss/Griff an Leon haengt (Griff von vorn, von hinten, Kriecher am Bein; Leon UND Elza), fehlt in manchen Bildern/Faellen; (2) ein angeschossener, noch dranhaengender (baumelnder) Arm des Zombies fehlt waehrend des Bisses. Port: enemy_ai_re2_zombie.c (Griff-FSM: grab_kill_ctr/grab_choreo in include/re15_actor.h, Griff [0] kopiert den Zombie-Anker auf den Spieler, Opfer-FSM des Spielers, Abriss-/Drift-Logik der Arme ~Z.6693ff 'Drift-Felder eines abgerissenen ARMS (Flagwort 0x4A)', Kinder abgerissener Teile ~Z.5584), Teile-Sichtbarkeit/Gore (md1_gore, Memory reai-v2-gore-vollausbau), emd_common.c. Schritt 1: alle Biss-Varianten an der exe reproduzieren (RE15_ENEMY_DBG, RE15_ANIM_TRACE, Framedumps RE15_WINDOW_SCALE=1, je Bild welche Teile des Zombies gezeichnet werden), auch mit vorher angeschossenem Arm (Schaden an Teil 3/6 o.ae.). Schritt 2: RE2-Original (RE2_Quellcode_Overlays/, ghidra_re2_Leon.txt, EM10-Griff/Biss, Teile-Flags +0x?? fuer abgerissene/baumelnde Arme) belegen, welche Teile RE2 waehrend des Bisses zeigt und wie der baumelnde Arm mitgeht; wo moeglich RE2 an DuckStation messen. Schritt 3: Fehlendes byte-true nachbauen. Riegel je Variante (Teile-Sichtbarkeit je Bild des Bisses), Runde-36-Riegel (Kleidung/Griff-Blut B, Schuss C) gruen halten." },
  { id: "K", thema: "roy", baum: "r37_roy",
    punkte: [
      "Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich seine Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben.....",
    ],
    hinweise: "Roy = NPC-Typ 0x47 (analysis/story_leon_elza/B_figuren.md §1.1: dasselbe Typ-Byte wie Elliot in Leons ROOM1170 sub15 @0x017D2): ROOM1011 sub00 @0x009F6 Roy (-2000,0,7450) verwundet, Dialog sub02 msg 0-17 (Ihre Aufnahme main16.5 laeuft, wenn er bewusstlos wird), danach sub00 @0x00A3E Roy bewusstlos (AOT Slot 5 -> sub06 Prison Key/Zettel); ROOM1021 main00 @0x01E0A Roy als Zombie (-4614,0,-16919), Plc_dest @0x020B2 auf John zu, John schiesst (Runde 36 Spur L: zwei Schuesse + Todeslaut), Roy Clip 1/7 @0x020DE = Fallen. Runde 36 L-Abnahme: Roy sackt in 1021 nicht sichtbar zusammen, in 1011 richtet er sich F1946..F1958 auf und bleibt aufrecht (NACHLAUF #21/#67) — derselbe Fehler. MODELL: die Modelle liegen in shared_assets/PSX/EMD/CDEMD0.EMS und CDEMD1.EMS (vermutlich je Spieler-Familie — PRUEFEN, welches EMD/EMS das Original fuer Typ 0x47 in Elzas Raeumen laedt (Lader in der EXE, Spieler-Familie PL04, Index), und was der Port laedt (emd/ems-Lader, enemy_common.c / npc-Code)). ANIMATIONEN: Clip-Nummern der Skripte (Plc_motion/Plc_flg/Plc_dest in 1011/1021) gegen die Bank des richtigen Modells; Dauerschleife = dieselbe Fehlerklasse wie frueher bei Leons NPCs — die Lehren nachlesen (Memory: reai-v2-platzhalter-als-verhalten 'Ada Clip 0 = Laufzyklus', reai-v2-npc-crossfade-decay, reai-v2-npc-global-yield, reai-v2-runde18-ada-eintritt; Dossiers analysis/befunde_runde18*, Plc_motion-Ende/Halten des letzten Bilds). Am Original messen (DuckStation-Savestate mit Elza in 1011/1021 — stage_saves/ sauber, sonst per Debug-Menue JUMP mit Elza; GDB wie Spur R), Framedumps Original gegen Port je Szenenabschnitt. Riegel: Modell-Kennung je Raum, Clip je Szenenabschnitt, kein Clip laeuft ueber sein Ende hinaus in Schleife (wo das Original haelt), Elliot bei Leon (1170) unveraendert." },
]

const REGELN = (s) => `
## Arbeitsbaum und Grenzen
- Dein Baum: ${ROOT}/.claude/worktrees/${s.baum} (Zweig r37/${s.thema}, Basis master ${BASIS}). Zuerst: git -C "${ROOT}/.claude/worktrees/${s.baum}" status --porcelain (leer) und git log --oneline -1 (= ${BASIS} oder ein Merge von master in den Zweig; Fortsetzungs-Baeume tragen zusaetzlich die Vorgaenger-Commits).
- ALLE Lese-/Schreibzugriffe auf Projektdateien und ALLE git-Befehle NUR in diesem Baum (absolute Pfade mit dem Praefix ${ROOT}/.claude/worktrees/${s.baum}/ bzw. git -C "<baum>"; in Bash jeden Befehl mit cd "<baum>" beginnen). NIE Dateien unter ${ROOT}/ ausserhalb deines Baums aendern. Aus dem Hauptbaum nur LESEN, was der Baum nicht traegt: ${ROOT}/info/Resident_Evil_und_Playstation_Information/ und ${ROOT}/.agent_refs/.
- Pflichtlektuere ZUERST, vollstaendig: <baum>/analysis/befunde_runde37/AUFTRAG.md und VERTRAG.md (Zuteilung VERBINDLICH; nimm nur, was Spur ${s.id} zugeteilt ist). Vorrunde: <baum>/analysis/befunde_runde36/<SPUR>_*.md (Dossiers + Abnahmeberichte der Module, die du anfasst).
- Dossier <baum>/analysis/befunde_runde37/${s.id}_${s.thema}.md als ERSTES anlegen und FORTLAUFEND schreiben; nach JEDEM abgeschlossenen Schritt committen (wip(r37-${s.thema}): ... ausdruecklich erwuenscht, auch VOR Bau/Suite). Das Sitzungslimit beendet alle Agenten gleichzeitig — was nicht im Commit steht, ist verloren. Nur eigene Pfade adden (NIE -A), Rohbytes/Backticks per git commit -F <datei>.

## RE-Disziplin (CLAUDE.md STOP-GATE, Kurzfassung)
- Je Punkt: MESSEN/REPRODUZIEREN am gebauten Stand -> ORIGINAL disassemblieren (RE1.5; wo RE1.5 unfertig ist, RE2 Retail = Ziel, Beleg aus RE2; Sound ist RE2 — <baum>/RE2_SOUND_LISTE.md zuerst) -> Adresse/Bytes ins Dossier -> DANN Code -> VERIFIZIEREN durch erneutes Messen. Jede Konstante @0x... (oder Byte-Offset) im Code UND in der Commit-Message; Nutzer-Vorgaben als NUTZER-VORGABE / PORT-WAHL, ihre FORM belegt. Kein "deferred/tunable/interim/Env-Schalter" als Abschluss. Regressionen ZUERST bisecten (v0.8.21 9362dd7e / v0.8.22 87cc8575 / v0.8.23 3ec5b1f8, dann die Runde-35-Merges), verursachenden Commit nennen; der Fix muss den Befund ERKLAEREN. "Sieht richtig aus" ist kein Beleg; Nutzer-Beobachtung schlaegt Code-Hypothese.
- Quellen im Baum: RE_15_Quellcode_Overlays/STAGE1_full/ (0x8010xxxx; STAGE1/ ohne _full ist +0x800 verschoben), RE_15_Quellcode_V2/, ghidra1_V2.txt, RE15_FUN_CATALOG.md, RE15_KNOWLEDGE.md, STAGE1_FLAG_MAP.md, RE15_SCD_OPCODES_REFERENCE.md, RE2_SOUND_LISTE.md; RE2: info/re2leon/, ghidra_re2_Leon.txt, RE2_Quellcode_V2/, RE2_Quellcode_Overlays/. Werkzeuge .claude/skills/re15-psx-disasm/scripts/re15_disasm.py / re2_disasm.py; Skills .claude/skills/<name>/SKILL.md (rdt-analysis, scd-disassembly, ghidra-mapping, re15-psx-disasm, re15-room-probe, re15-room-capture, re15-port-visual-verify, re15-enemy-ai-re, re15-weapon-render, re15-parity-verify, re15-pc-render-order, re15-savestate-ghidra, re15-pcsx-watchpoint) ZUERST lesen. DuckStation-GDB fuer RE2-/RE1.5-Messungen: Muster in analysis/befunde_runde36/H_raeume.md und J_affen.md (EnableGDBServer in %LOCALAPPDATA%/DuckStation/settings.ini, danach zurueck). Memory-Notizen (reai-v2-...) unter C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/<name>.md (nur lesen).

## Bauen, Messen, Testen
- NUR so bauen: cd "<baum>" && bash re15_port/tools/local_build.sh configure (einmal und nach jeder NEUEN .c-Datei), danach ... build (test/all = volle Suite: im Spurbaum VERBOTEN, s.u.). NIE cmake --build aus Git-Bash. exe <baum>/re15_port/build/platform/pc/re15_pc.exe; befund.log/debug.log daneben. ⛔ KEINE volle Suite (local_build.sh test/all) im Spurbaum — gemessen 2026-10-10: fuenf parallele Suiten brauchten je ~80 min und lieferten Dutzende Last-Timeouts der Fensterhaken (unbrauchbar). Stattdessen GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = eigene Tests (r37_${s.thema}) + die Testgruppen JEDES Moduls/Raums, das dein Diff beruehrt (git -C "<baum>" diff ${BASE(s)} --name-only lesen; vorhandene Testnamen: ctest --test-dir ... -N | grep -i <modul/raum>). Die ctest-Summenzeile ("100% tests passed, 0 tests failed out of N") woertlich mit dem -R-Ausdruck ins Dossier. Die volle Suite faehrt der Orchestrator EINMAL im Integrationsbaum. Fenster-Haken flattern unter Last (boot_bg_pin, dark_start_pin, relatch_pin, save_counter_pin, weste_load_pin, r30_sicherung_*, r30_titel_puls, r34n_e_dokumente_bild; exe-Starts brechen selten mit exit 1 nach 0,1 s ab): einzeln 2-3x nachfahren (ctest --test-dir "<baum>/re15_port/build" -R "^<name>$"), nicht als Regression werten. NIE taskkill /IM re15_pc.exe (nur eigene PID); Messkopie der exe unter eigenem Namen neben der exe (Muster tests/integration/test_r34n_b_cursor.cmake). Nie zwei Builds gleichzeitig im eigenen build/ (ein zweites Bauverzeichnis fuer Bisects: RE15_BUILD_DIR=<baum>/re15_port/build_bisect).
- Harness: getenv("RE15_...") in platform/pc/main.c + engine/src (RE15_DEBUG_JUMP, RE15_GOTO_ROOM, RE15_FORCE_EVENT, RE15_FORCE_CUT, RE15_PLAYER_POS, RE15_EQUIP, RE15_GIVE, RE15_PRESS, RE15_PAD_AT, RE15_INPUT_SCRIPT, RE15_SET_FLAG(_AT), RE15_FIRE_AOT, RE15_FORCE_EXPLOSION, RE15_GRANATE_LOG, RE15_STATE_LOG, RE15_EVT_TRACE, RE15_SCD_TRACE, RE15_MSG_LOG, RE15_ENEMY_DBG, RE15_ANIM_TRACE, RE15_NECK_LOG, RE15_FX_LOG, RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC (Ton-Mitschnitt ohne Audiogeraet), RE15_FRAMEDUMP, RE15_EXIT_AT, RE15_KILL_AT, RE15_CONTINUE_TEST + RE15_CARD_AUTO, RE15_PSELECT_AUTO(_SWITCH) fuer Elza, RE15_CLIMB_TRACE, RE15_TUER_TEST/_SEITE, RE15_MAP_SHOT_PAGE, RE15_PRI_LOG) — Semantik im Code nachlesen. Bilder NUR per RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1 (gdigrab liefert weisse Bilder; Framedump schreibt Fensterskalierung). Story-/Raum-Punkte ueber den ECHTEN Weg abnehmen (Tuer betreten, Spielstand + CONTINUE), nicht nur per Sprung; Elza-Punkte mit Elza (RE15_PSELECT_AUTO).
- Tests: NEUE Sonden/Pins in re15_port/tests/unit/probes/r37_${s.thema}.cmake (GLOB; Muster probes/r37_*.cmake) + tests/unit/test_r37_${s.thema}*.c / tests/integration/test_r37_${s.thema}*.cmake (registriert NUR aus deiner probes-Datei). NICHT tests/unit/CMakeLists.txt und NICHT tests/integration/CMakeLists.txt. Mindestens ein Test je Nutzer-Punkt, der die MECHANIK misst. Arbeitsordner eines Tests nie gleich dem Target-Namen (Linux ohne .exe).
- Dateien: neue Logik in NEUEN Dateien engine/src/<thema>_r37.c + include/re15_<thema>_r37.h; gemeinsame Dateien nur mit kleinen Haken (1-5 Zeilen) an klar benannten Stellen, Installer-Zeile am ENDE des Blocks in scd_room_setup.c mit Kommentar "Runde 37 Spur ${s.id}". KEIN Patch an shared_assets/PSX/*; neue Daten gen/*.inc oder shared_assets/RE2/ (im Dossier nennen). release/, platform/android/, RELEASE_NOTES.md: TABU.
- Token sparen: grosse Dateien nie ganz lesen (grep -n + Bereich), Logs gefiltert, Bau-/Suite-Ausgaben in Datei umleiten und nur Ende/Fehler lesen, keine Messung wiederholen, nur urteilstragende Bilder ansehen.

## Abschluss
- Dossier mit Abschnitten: Messung vorher (bei Regressionen: Bisect-Tabelle) / RE-Belege (Adressen, Bytes) / Umsetzung (Dateien, Konstanten) / Messung nachher / Tests (Namen, Ergebnis) / OFFEN / Fuer den Nutzer (neue Assets fuer das Paket-Gate, Sprachdateien, Bedienhinweise, Entscheidungsfragen). Letzter Commit fix/feat(r37-${s.thema}): ... Gezielte Tests gruen: ctest-Summenzeile + -R-Ausdruck woertlich zitieren (KEINE volle Suite). Antwort = StructuredOutput.
`

const bauPrompt = (s) => `Du bist Spur ${s.id} "${s.thema}" der Runde 37 (2026-10-12) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Du ermittelst, belegst, baust und testest ALLE Punkte dieser Spur im eigenen Arbeitsbaum. Ein anderer Agent nimmt danach unabhaengig gegen den Wortlaut des Nutzers ab — was du nicht gemessen und belegt hast, faellt dort durch.
${STAND[s.id] ? `
## ⛔ FORTSETZUNG — dein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST das Dossier, git -C "<baum>" log --stat ${BASE(s)}..HEAD und den Code-Diff gegen master. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg. Stelle je Nutzer-Punkt fest, was noch fehlt, und schliesse genau das ab; pruefe kritisch, ob der Stand den WORTLAUT erfuellt.
` : ''}
## Deine Punkte (Wortlaut des Nutzers, AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Einstiegspunkte (Stand 2026-10-12 — Zeilennummern und Behauptungen SELBST verifizieren)
${s.hinweise}
${REGELN(s)}
Arbeite jeden Punkt vollstaendig ab (Dossier je Punkt: Messung vorher / Beleg / Umsetzung / Messung nachher). Sackgasse: Weg wechseln (Xrefs, Caller/Callee, RE2, Savestate-RAM, GDB, Laufzeit-Log), nicht aufgeben, nicht raten. Am Ende: gezielte Tests gruen (keine volle Suite), alles committet, Dossier vollstaendig.`

const nachPrompt = (s, abn, runde, vorher) => `Du bist Spur ${s.id} "${s.thema}" der Runde 37 (2026-10-12) im Projekt reAi_v2 — NACHBESSERUNG ${runde} nach einer unabhaengigen Abnahme. Der Bau-Agent vor dir hat im selben Baum gearbeitet; sein Stand ist committet (lies git -C "<baum>" log --oneline ${BASE(s)}..HEAD, das Dossier und den Abnahmebericht <baum>/analysis/befunde_runde37/${s.id}_abnahme_${runde - 1}.md).

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
Punkte, die die Abnahme als erfuellt bewertet hat, NICHT verschlechtern (ihre Riegel gruen halten). Schreibe im Dossier einen Abschnitt "Nachbesserung ${runde}" (je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher). Baue neu, gezielte Tests gruen (keine volle Suite), committe (fix(r37-${s.thema}): Nachbesserung ${runde} ...).`

const abnahmePrompt = (s, bau, runde) => `Du bist die UNABHAENGIGE ABNAHME ${runde} fuer Spur ${s.id} "${s.thema}" der Runde 37 (2026-10-12) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Ein Bau-Agent behauptet, die Punkte des Nutzers umgesetzt zu haben. Du glaubst ihm NICHTS, was du nicht selbst am gebauten Stand gemessen hast. Der Nutzer weist jede Aenderung ohne Disasm-Beleg zurueck — du bist seine Instanz.

## Die Punkte des Nutzers (Wortlaut; AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Behauptung des Bau-Agenten
Status: ${bau ? bau.status : 'KEIN ERGEBNIS (Agent abgebrochen) — pruefe Baum und Dossier trotzdem'}
${bau ? bau.zusammenfassung : ''}
Commits: ${bau && bau.commits ? bau.commits.join(', ') : '?'}
Suite laut Bau-Agent: ${bau ? bau.suite : '?'}
Offen laut Bau-Agent: ${bau && bau.offene_punkte && bau.offene_punkte.length ? bau.offene_punkte.join(' | ') : '(nichts)'}

## Baum
${ROOT}/.claude/worktrees/${s.baum} (Zweig r37/${s.thema}, Basis master ${BASIS}). ALLE Zugriffe und git-Befehle nur dort (absolute Pfade, git -C "<baum>", Bash mit cd "<baum>"). Nichts ausserhalb des Baums aendern. Aus dem Hauptbaum ${ROOT}/ nur lesen (info/Resident_Evil_und_Playstation_Information/, .agent_refs/).

## Vorgehen
1. Lies <baum>/analysis/befunde_runde37/AUFTRAG.md, VERTRAG.md, das Dossier ${s.id}_${s.thema}.md und: git -C "<baum>" log --oneline ${BASE(s)}..HEAD; git -C "<baum>" diff ${BASE(s)} --stat; git -C "<baum>" diff ${BASE(s)} -- re15_port (den Code-Diff WIRKLICH lesen).
2. Baue: cd "<baum>" && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build (nie cmake --build aus Git-Bash). KEINE volle Suite (test/all) im Spurbaum — die faehrt der Orchestrator einmal im Integrationsbaum. Fahre GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = Tests der Spur (r37_${s.thema}) + die Testgruppen aller Module/Raeume, die der Diff beruehrt (git diff ${BASE(s)} --name-only; ctest -N | grep -i). Rote Fenster-Haken unter Last 2-3x einzeln nachfahren, bevor du sie wertest. NIE taskkill /IM re15_pc.exe; Messkopie der exe unter eigenem Namen (Muster tests/integration/test_r34n_b_cursor.cmake).
3. MISS jeden Nutzer-Punkt selbst an der exe (<baum>/re15_port/build/platform/pc/re15_pc.exe; Haken getenv("RE15_...") — Semantik im Code nachlesen; Spielstand + CONTINUE, RE15_DEBUG_JUMP, RE15_FORCE_EVENT, RE15_PLAYER_POS, RE15_PRESS/RE15_PAD_AT/RE15_INPUT_SCRIPT, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, Logs RE15_STATE_LOG/RE15_EVT_TRACE/RE15_ENEMY_DBG/RE15_FX_LOG/RE15_SE_DEBUG/RE15_AUDIO_CAP_SYNC usw.; Elza-Punkte mit RE15_PSELECT_AUTO). Story-/Raum-Punkte ueber den ECHTEN Weg (Tuer betreten), nicht nur per Sprung. Bei Regressionen: Gegenprobe mit der v0.8.21-exe, ob der genannte verursachende Commit den Befund wirklich erklaert. Urteil je Punkt gegen den WORTLAUT: erfuellt / teilweise / nicht_erfuellt / nicht_pruefbar (mit Grund) — mit Messprotokoll (Befehl, Log-Zeilen, Bild-Pfad).
4. RE-Gate: Jede verhaltensrelevante Konstante im Diff hat @0x... (oder Byte-Offset) ODER ist als NUTZER-VORGABE / PORT-WAHL gekennzeichnet; stichprobenartig 3 Adressen SELBST disassemblieren (re15_disasm.py / re2_disasm.py / ghidra*.txt) und pruefen, dass die zitierte Stelle das Behauptete enthaelt (Sprungziel disassemblieren, richtige Binaerdatei). Suche im Diff nach "deferred", "tunable", "interim", "for now", "faithful", "plausibel", "TODO", Env-Schaltern als Abschluss. Pruefe, ob der Fix den gemessenen Befund ERKLAERT.
5. Vertrag: nur zugeteilte Bank-9-Bits/Nachrichten-IDs/Slots/Ereignisse (VERTRAG.md); erlaubte Pfade (git diff ${BASE(s)} --name-only: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists); gemeinsame Dateien nur kleine Haken; Tests je Punkt vorhanden und messend (ctest --test-dir "<baum>/re15_port/build" -R "r37_${s.thema}" laufen lassen). Pruefe auch Nebenwirkungen auf das, was die Spur beruehrt (z.B. andere Raeume, beide Szenarien).
6. Schreibe den Bericht <baum>/analysis/befunde_runde37/${s.id}_abnahme_${runde}.md (je Punkt Messprotokoll, Urteil, Beleg; Gates; Maengel nummeriert, konkret, nachpruefbar) und committe NUR diese Datei: git -C "<baum>" add analysis/befunde_runde37/${s.id}_abnahme_${runde}.md && git -C "<baum>" commit -m "doc(r37-${s.thema}): Abnahme ${runde}". Du aenderst KEINEN Code. Schreibe den Bericht FORTLAUFEND (nach jedem gemessenen Punkt speichern) — das Sitzungslimit kann dich jederzeit beenden.
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