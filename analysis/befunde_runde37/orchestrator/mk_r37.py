"""Erzeugt analysis/befunde_runde37/orchestrator/r37.js aus dem Runde-36-Orchestrator (gleiche Maschine:
Bau -> unabhaengige Abnahme -> Nachbesserung, Modi ueber args), mit den Spuren der Runde 37."""
import io, json, re

SRC = r"C:\workspace\git\reAi_v2\analysis\befunde_runde36\orchestrator\r36-rest3.js"
DST = r"C:\workspace\git\reAi_v2\analysis\befunde_runde37\orchestrator\r37_i.js"
BASIS = "64ba31e0"
HAUPT = "C:/workspace/git/reAi_v2"

SPUREN = [
 dict(id="A", thema="pri1100", baum="r37_pri1100",
  punkte=["Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker. Die Marker Datei liegt da wo immer die Dateien liegen mit F9."],
  hinweise=("Die F9-Marken des Nutzers liegen im HAUPTBAUM (nicht in deinem Baum): " + HAUPT + "/re15_port/build/platform/pc/befund_1100_F892_marke1.bmp "
   "(2026-10-12 10:07, v0.8.24-Stand; 960x720 BMP, mit PowerShell System.Drawing in PNG wandeln) und befund_1100_F240_marke1.bmp (2026-10-05, vor Runde 36). "
   "Die Zahlen dazu stehen in " + HAUPT + "/re15_port/build/platform/pc/befund.log, Block '=========== MARKE 1 (F9) ===========' (grep -n 'MARKE' ; ab Zeile ~15850): "
   "F892 R1100 C1, Leon pos=(-14914,0,-25645) rot=4488 vz=12487/12358/12228, alle 105 Masken dieses Winkels mit Bildlage/Tiefe, Gegner: Zombie 2 typ 0x10 LIEGEND "
   "pos=(-14269,0,-25091) (dist 849), Zombie 1 (-16157,0,-23876). Im Bild F892 liegt Zombie 2 quer in der Tuer am Ende des Gangs, sein Kopf liegt an/ueber der "
   "rechten Tuerrahmen-Kante (etwa x 168..178 / y 140..148 im 320x240-Bild) und wird ueber die davorliegende rechte Wand gezeichnet; F240 zeigt dasselbe Muster "
   "(nur der Kopf eines liegenden Zombies ueber der rechten Wand sichtbar). ⛔ Spur O der Runde 36 hat die Marke nie gesehen (falsch gesucht) und in Abschnitt 2 "
   "(analysis/befunde_runde36/O_pri.md, '2.2 e)' und '2.5': 'Kopf des kriechenden Zombies wieder sichtbar' an der Suedecke (-15500,-23700)) moeglicherweise genau "
   "die falsche Richtung gebaut — PRUEFEN. Daten: shared_assets/PSX/MASKS/ROOM1100.MSK + ROOM1100_PRI01/02.TIM (+ .PBM/.STAND/.BELEG), Werkzeuge tools/maske/, "
   "render_pc.c (Masken-Tiefentest, Schatten-Bucket der Spur O), Lehren im Memory: Maske = Tiefen-PROFIL je Zeile, 'deckt ab' != 'verdeckt', flache Maske "
   "verdeckt nichts. Vorgehen: Szene exakt nachstellen (Leon + liegender Zombie an derselben Stelle, RE15_PLAYER_POS/Gegner-Haken oder Kartenwerkzeug), "
   "Framedump (RE15_WINDOW_SCALE=1), je Pixel des Kopfes: Figur-Tiefe gegen Masken-Tiefe; geometrisch belegen, ob der Kopf HINTER der Wandebene liegt (Welt- "
   "und Kameradaten, SCA-Zellen der Wand); dann die Maske/Tiefe korrigieren und mit BEIDEN Marken (F892, F240) und den Faellen der Spur O (Schatten am Wandfuss "
   "Cut 1/2, Zombie hinter der Nordecke) gegenpruefen, dass nichts anderes kippt. Original-Vergleich, wo moeglich (stage_saves/ mit ROOM1100, sauber = nicht "
   "PATCHED-EXE_*).")),
 dict(id="C", thema="fahrstuhl", baum="r37_fahrstuhl",
  punkte=["Die Fahrstuhl Tür in der Tür Animation ist noch nicht korrekt. Da musst du noch mal schauen ... wenn es keine passende Fahrstuhl Tür gibt, müssen wir die wieder bauen....",
          "Beim Aussteigen aus dem Fahrstuhl fehlt noch die Türsequenz. << stimmt."],
  hinweise=("Stand v0.8.24 (Runde 36 Spur G, analysis/befunde_runde36/G_tueren.md + G_abnahme_0.md H1/H3/X1): der Einstieg in die Kabine ROOM1080 spielt RE2 DOOR27 "
   "(RE2-LABOR-Fahrstuhl, oliv-grau mit Bedienfeld, zwei Fluegel aus der Mitte) — der Nutzer sagt: nicht korrekt. Die Runde-33-Port-Archive (P25G = Hub, Farbe "
   "passend) hatten die falsche Mechanik. Schritt 1: den GEMALTEN RE1.5-Fahrstuhl in den Hintergruenden belegen (Kabine 1080, die Etagen-Raeume mit der "
   "Fahrstuhltuer, Cuts; Farbe, Fluegelzahl, Oeffnungsrichtung, Rahmen, Bedienfeld) und die RE1.5-eigene Tuer-Zuordnung (welches DO2 spielte das Original fuer "
   "1080? info/Re1.5/PSX/DOOR + Tuer-Tabelle der EXE). Schritt 2: ALLE RE2-Tuerarchive (shared_assets/RE2/DOOR/*.DO2, info/re2leon/) als Bogen rendern "
   "(RE15_TUER_SEITE, G_belege/) und eine wirklich passende Fahrstuhl-Tuer suchen. Gibt es keine: eigenes Port-Archiv bauen (Rezept-Weg wie Runde 33/36: "
   "tools/tueren/, r36_rezept_p0cd.py, shared_assets/RE15DOOR/, FNV in gen/re15_tuer_eigen.inc) mit Fahrstuhl-Mechanik und der Optik des gemalten RE1.5-"
   "Fahrstuhls; Bildvergleich gegen die Hintergruende. Schritt 3 Ausstieg: ROOM1080/4020 verlassen die Kabine per Skript (Null-Rechteck-Tueren ROOM1080.RDT "
   "@0x00482/@0x004A2/@0x004C2, Aot_on sub08 @0x0080E) ohne Sequenz; RE2 spielt beim Verlassen der Kabine DOOR27 V0 (ROOM6010 @0x004C2/@0x004E2/@0x00502; "
   "Skript-Tueren mit Archiv: ROOM21B0 sub03 @0x026EE). Ausstieg mit derselben (korrigierten) Tuer, Richtung/Variante belegen. Code: door_scene_pc.c, "
   "door_seq_*.c, gen/tuer_zuordnung.inc, gen/re15_tuer_eigen.inc; Ton RE2 (RE2_SOUND_LISTE.md, ELEVSE.VBS liegt schon unter shared_assets/RE2/). Auch "
   "ROOM4020 (Labor-Fahrstuhl) und Elzas Varianten pruefen. Bilder: Bogen/Framedump je Tuer im Dossier, Vergleich zum Hintergrund.")),
 dict(id="D", thema="itemband", baum="r37_itemband",
  punkte=["Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden."],
  hinweise=("Befund: analysis/befunde_runde36/R_abnahme_3.md M1/M2/H4 und NACHLAUF.md #73/#74. (M1) Der AOT-Scan FUN_80042bac prueft im Original fuer JEDEN Eintrag "
   "das Band: @0x80042cac-ccc rec[2] == Spieler+0x82 (sonst naechster Eintrag); der Port wendet das Band-Gate nicht auf ITEM an (aot_common.c). (M2) Member_set "
   "0x12 (FUN_8004116c, Tabelle 0x80010c8c[18] -> 0x80041228 sb a2,130(a0) = DAT_800acad6 = Band der Schubkette @0x8002bf38 und des AOT-Scans @0x80042cc0) setzt "
   "im Port nur g_actors.floor (actor_common.c re15_actor_set_member case 18), nicht das Kollisionsband (s_coll_band in re15_collision.c) -> ROOM3050-Lift "
   "(sub04 @0x023D0, (3,82)=1) laesst Leon auf Band 4. NUR GEMEINSAM beheben (sonst waere die Super Redhawk unerreichbar). Zensus aller Item_aot_set mit floor != 0 "
   "(12: 2030/2031, 2040/2041, 2090/2091, 3010/3011, 3050/3051 — selbst nachzaehlen, scd_dump_room.py) und aller Member_set 0x12 in allen RDTs; je Item an der "
   "exe belegen, dass es auf dem VORGESEHENEN Weg erreichbar bleibt (3050: Kiste schieben -> Fall -> Lift -> Band 0 -> Kerbe slot 7 @0x01F06 -> Latch (3,93) "
   "sub01 @0x02184-2196 -> Aufstieg -> Super Redhawk (9,92)) und vom falschen Band NICHT aufnehmbar ist; Original-Messung am Original wie in Spur R "
   "(R_belege/orig_*.py, DuckStation-GDB, settings.ini EnableGDBServer danach wieder false). Riegel: Zensus + Verhaltens-Pins + Gegenprobe. Achtung: das Band "
   "steuert auch Treppen/Klettern/Schub — Spur-R/A-Riegel (unit_r36_dachkiste_*, unit_r36_klettern*, unit_r36_objfall*, integration_r36_dachkiste) gruen "
   "halten.")),
 dict(id="E", thema="codepanel", baum="r37_codepanel",
  punkte=["bei den Code panels möchte ich das, so lange der Code noch falsch eingegeben ist unten das blaue Licht leuchtet. Ist er dann richtig eingegeben das grüne Licht.",
          "Die Kamera darf noch auf den Panels bleiben wenn der Piep und unlock sound kommt.", "Nachtrag 3 (Korrektur): Nein, beim Panel in korridor 2 ist falsch - dort soll natürlich rot leuchten - richtig eingegeben - dann blau.... / rot = geschlossen", "Nachtrag 3b: Es muss schon in der richtigen Farbe leuchten das Panel... hintergrund rot - rot leuchten - hintergrund blau - blau leuchten..."],
  hinweise=("Code-Panel-Raeume (Runde 36 Spur G, analysis/befunde_runde36/G_tueren.md Z.238ff): Ziffernfeld + Karte in 10D0/10D1 (Code 4312, Blue Keycard), "
   "11E0/11E1 ('Prisons', Yellow Keycard + 0513), 1230/1231 (Zettel 5632); dazu das Minidisc-Schloss 1100/1101 (Cursor-Raetsel) — vollstaendigen Zensus aller "
   "Ziffern-/Code-Panels aller Stages selbst machen (Ck(5,0x0D..0x10)-Muster, Cursor-Raster-Raeume). Die Folge Piep/Aufschliessen (0/15/30 Bilder) baute Spur G "
   "(engine/src/aufschliessen_r36.c, gen/r36_aufschliessen.inc, shared_assets/RE2/KARTSE.VBS). Lampen: im Panel-Bild unten blau und gruen (Hintergrund-Cut des "
   "Panels ansehen und belegen, wo die Lampen gemalt sind, ob als an/aus); Vorbild fuer gezeichnete Lampen im Port: Generator ROOM11F0 (panel_lampen_pc.c + "
   "shared_assets/RE2/LAMPE2130.TIM aus RE2 ROOM2130). RE2 als Vorbild pruefen (Beta -> Retail): gibt es in RE2 Code-/Kartenpanels mit Lampe rot/blau/gruen "
   "(RDT-ESP/TIM, Skript)? Dann deren Mechanik/Texturen; sonst NUTZER-VORGABE mit belegter Lampenlage. Kamera: heute schaltet die Kamera vor dem Piep/"
   "Aufschliessen zurueck (Cut_old/Cut_auto des Panel-Skripts) — die Ruecknahme bis nach dem Aufschliess-Ton verschieben (Bildnummern im Log: Piep, "
   "Aufschliessen, Kamerawechsel), ohne die Tuer-/Skriptlogik zu aendern. Messen mit RE15_SE_DEBUG/RE15_AUDIO_CAP_S, RE15_EVT_TRACE, Framedumps je Zustand "
   "(falsch/teilweise/richtig).")),
 dict(id="F", thema="sperre1050", baum="r37_sperre1050",
  punkte=["bei Leon, bevor dem Ende wo er noch zu Irons muss habe ich eine Lücke gefunden - man kann über die Luke bis nach F1 und kann den \"Ich muss zuerst irons holen\" Blocker bei der Tür überspringen. Ich schlage vor - du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene triggern, wenn wir Ada noch nicht gerettet haben.",
          "Ja, neuen Texte sind super. Bezug zu ada bei elza kann raus."],
  hinweise=("Sperre heute: include/re15_tuer1060.h + engine/src/tuer1060*.c (ROOM1060 Slot 2 -> ROOM1040, Text 'I have to get the Chief first...', Bedingung "
   "(9,73)=0 UND ((9,71)=1 ODER (3,94)=0)). Ada-Tuer: include/re15_adaruf.h + engine/src/adaruf_1050.c (ROOM1050 Slot 4 -> ROOM10A0; vor der Ada-Rettung "
   "(3,0xBB)=0: Szene Ereignis 13 bzw. Sperrtext msg 25; danach Original-Tuer). Schritt 1: die Luecke des Nutzers REPRODUZIEREN (Leon im Endspiel-Zustand: "
   "10F0-Szene gesehen (9,71)=1, Irons noch nicht geholt (9,73)=0) — welche 'Luke' fuehrt nach F1 an der 1060-Sperre vorbei, Weg Ende-zu-Ende an der exe; "
   "Zensus aller Wege von dort zum Ende von Stage 1 (11B0/11C0), damit die neue Sperre die Luecke wirklich schliesst. Schritt 2: an ROOM1050 Slot 4 nach der "
   "Ada-Rettung denselben Text wie 1060 zeigen, solange Leon den Chief noch holen muss (Bedingung im Dossier begruenden: Fruehspiel darf NICHT gesperrt werden, "
   "dort gilt die Ada-Logik; Elza (ROOM1051) bleibt offen). Nachrichten-Id laut VERTRAG.md 1.2 (26..28), Form wie re15_tuer1060.h (Text-Platz sce 1). "
   "Schritt 3 Text Elza ROOM50B1 msg 0 'It's Ada. She's dead.It looks like one of those / gorillas killed her.' ohne Ada-Bezug (NUTZER-VORGABE), Weg der "
   "Spur P (tools/r36_p/texte_bauen.py, gen/story_r36_texte.inc, engine/src/story_r36.c), Wortlaut PORT-WAHL in RE2-Form (z.B. 'It's a woman. She's dead.'), "
   "Zeilenumbrueche wie das Original; Sweep, ob Elza an weiteren Stellen 'Ada' sagt/liest.")),
 dict(id="G", thema="beretta", baum="r37_beretta",
  punkte=["bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden. Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung."],
  hinweise=("Vorarbeit: analysis/waffen_fsm_2026-09-12/ (SPEC.md Z.273/320: Waffen 5/6 = M93R/Glock 18 3-Burst, Entlade-Handler 0x800338A8 ueber Tabelle "
   "@0x80074114/18, ammo_per_shot 3 @0x80033974-84; entlade-granaten-burst.md §2: drei FUN_80019700-Spawns = Muendungs-Effekte). Runde 36 Spur C (analysis/"
   "befunde_runde36/C_schuss.md, engine/src/schuss_r36.c): Schusspfad FUN_80011f50 @0x80012168-70, Sichtlinie FUN_8001b9b4, RE2-Treffer-Resolver @0x80041270-9c. "
   "Schritt 1 MESSEN: an der exe (Waffe 5, RE15_EQUIP/RE15_GIVE) einen Zombie (RE1.5-KI und RE2-KI) mit EINEM Feuerstoss treffen — wie viele Treffer, wie viel "
   "Schaden (HP vorher/nachher, RE15_ENEMY_DBG), wie viele Treffer-Reaktionen/Blut, wie viel Munition; dasselbe fuer Waffe 6 (Glock 18) und zum Vergleich "
   "Browning (Einzelschuss). Schritt 2 ORIGINAL: im RE1.5-Original (DuckStation-GDB wie Spur R, stage_saves/ sauber; Waffe per RAM/Debug-Menue) messen, ob "
   "der Feuerstoss DREI Treffer-Aufloesungen macht (Haltepunkt im Treffer-Pfad je Schuss) und welchen Schaden; disassemblieren, wo der Burst den Schuss-Pfad "
   "dreimal oder einmal ruft. RE1.5 dreimal -> Port byte-true nachziehen. RE1.5 einmal -> NUTZER-VORGABE: jeder der drei Schuesse loest einen eigenen "
   "Treffer aus (Schaden je Schuss wie der Einzelschuss der Waffe aus der Schadenstabelle, Reaktion/Blut je Treffer), Zeitpunkte je Schuss aus dem Clip der "
   "Waffe belegen. RE2/RE3-Vorbild fuer Feuerstoesse pruefen (Beta -> Retail). Riegel: Treffer je Feuerstoss = 3 (Leon und Elza), Munition -3, Gegenprobe; "
   "Dauerfeuer-Waffen (Ingram/MC51) und Einzelschuss-Waffen unveraendert (Runde-36-C-Riegel gruen halten).")),
 dict(id="H", thema="zombieaktiv", baum="r37_zombieaktiv",
  punkte=["das soll nicht nur da gelten, das darf generell für die Zombies gelten — schaue einfach nach wie Resident Evil 2 das macht (Nutzer-Ergaenzung: gilt fuer ALLE Zombies, Vorlage RE2)", "die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang."],
  hinweise=("UMFANG (Nutzer): GENERELL alle Zombies — RE1.5-Typen (0x10/0x11/0x16/... im Port mit RE2-KI als Standard, enemy_ai_re2_zombie.c) in JEDEM Raum und jeder Spawn-Art; Vorlage ist die RE2-Retail-Zombie-KI: wie sie im Gang wandern, wann sie loslaufen, auf Leon zukommen (Zustandsautomat, Weck-/Sichtbedingungen, Wander-Ziele, Tempo) — aus RE2 belegen (RE2_Quellcode_Overlays/, ghidra_re2_Leon.txt, info/re2leon/ RDTs: Sce_em_set-Bytes der RE2-Gang-Zombies) und am RE2-Original messen, wo moeglich (DuckStation mit RE2, Memory reai-v2-re2-trace-datei). Erst feststellen, was der Port von der RE2-KI schon hat und was fehlt (z.B. Wander-Zustand, Weckradius, Annaeherung ohne Sicht, Spawn-Variante), dann das Fehlende byte-true nachbauen. Beispiel des Nutzers: Montage nach Irons' Tod:  include/re15_irons_tod.h + engine/src/irons_tod_1150.c (Runde 35 Spur L, Runde 36 Spur K: analysis/befunde_runde36/"
   "K_szene1150.md; Kette 1150 -> [1130 Cut 0, wenn in 1140 noch Zombies leben] -> 1040 Cut 1 -> 1030 (Cut 7, wenn in 1070 noch Zombies leben; Cut 6) -> 11C0 "
   "Cut 13 -> 1150; Spawn-Bits (9,74)/(9,75)/(9,76) fuer 1130/1040/1030, Records 15..19 seit Runde 36 Spur E (20 Gegner)). Schritt 1 MESSEN am ECHTEN Weg: "
   "Leon nach der Montage durch 1130/1040/1030/1070 (Spielstand + CONTINUE oder Kartenwerkzeug wie Spur E probe_r36_gegnerzahl_karte), je Zombie ueber "
   "mehrere Sekunden: Zustand/Unterzustand, Sicht/Weck-Status, Geschwindigkeit, Abstand zu Leon (RE15_ENEMY_DBG, RE15_STATE_LOG, befund.log-Gegnerliste), "
   "Framedumps. Wer steht warum? (Weck-Radius, Sicht, Spawn-Byte des Montage-Satzes (Sce_em_set beh / pc[6], Memory: pc[6] NICHT ohne Messung anfassen), "
   "AI-Freeze-Gate (DAT_800aca40 & 0x20000000 / entity+0x9 & 0x20) nach der Szene nicht geloescht, Szenen-Flags, Tick-Budget bei vielen Gegnern?). "
   "Schritt 2 ORIGINAL: was tun die Zombies dort im RE1.5-Original (Montage gibt es dort so nicht — PORT-Szene; Vergleich mit den Zombies derselben Raeume "
   "im Original-Normalspiel und mit RE2: RE2-Zombie-KI enemy_ai_re2_zombie.c, RE2 EM10/EMZ0 in RE2_Quellcode_Overlays/ und ghidra_re2_Leon.txt: "
   "Wander-/Annaeherungszustaende im Gang, Weckbedingungen). Ziel Beta -> Retail: wie RE2 (teilweise aktiv auf Leon zu), mit Beleg. Runde 36 Spur F "
   "(analysis/befunde_runde36/F_zombieki.md, Riegel unit_r36_zombieki_*, Eintritts-Zensus test_r36_zombieki_eintritt) gruen halten; Spur-E-Riegel "
   "(integration_r36_gegnerzahl) und die Montage-Riegel (integration_r35_cut1150, unit_r36_szene1150*) gruen halten. Riegel: nach der Montage bewegt sich "
   "in 1130/1040 ein Anteil der Zombies binnen N Bildern auf Leon zu (N aus RE2 belegt), Gegenprobe Stand v0.8.24.")),
 dict(id="I", thema="feuerloescher", baum="r37_feuerloescher",
  punkte=["Bitte erstelle mir zusätzlich auch ein Modell des Feuerlöschers. Ich finde deine Idee, bei elza einen Leeren Feuerlöscher am Boden zu haben gut. Der müsste dann bei Leon nach der Nutzung erstmalig auftauchen.",
          "Außerdem finde ich den FEuerlöscher im Spind bei Leon noch unglücklich und würde ihn gerne woanders platzieren....", "Nachtrag 4b: ah, it can only lay there - if leon when he is asked to discard it select \"yes\"."],
  hinweise=("Vorarbeit: extracted_re2_feuerloescher/README.md + analysis/befunde_2026-09-21/re2-feuerloescher-modell.md: es gibt KEIN Feuerloescher-Modell in RE2 "
   "oder RE1.5; vorhanden sind das RE1.5-Item-Bild (info/Re1.5/PSX/ITEM/ITPS.ITP file@0x093000, 112x72 8bpp, Item 0x31: roter Loescher, Chromgriff, weisses Etikett; "
   "Decoder engine/src/itps_common.c) und vier Basis-Netze (extracted_re2_feuerloescher/basis_kandidaten/: RE1.5 ROOM1010 m00 Spraydose @0xAF4/@0x22D28 und "
   "ROOM4050 m03 rote Spraydose @0x3788/@0x2DDA8 = beste Basis, RE2 ROOM1010 m03 / 60D0 m02). MODELL BAUEN (PORT-WAHL, im Dossier begruenden): Netz eines "
   "Feuerloeschers (Zylinderkoerper, Schulter, Ventil mit Griff/Hebel, Schlauch/Duese) in PSX-Massstab (Hoehe aus der realen Groesse und der Leon-Groesse "
   "abgeleitet, Memory reai-v2-modellmass-aus-kamera), Textur aus dem ITPS-Bild (rot, Chrom, weisses Etikett) als 4/8-bpp-TIM, eingebettet wie die Lageplaene "
   "der Spur N (engine/src/gen/kartenitems_r36.inc: MD1 + TIM als gen/*.inc, Prop wie Obj_model_set, Licht flach/NCCT wie das Original fuer solche Objekte "
   "belegen). Sichtpruefung mit Framedumps aus mehreren Kameras + Vergleich mit dem Item-Bild. EINSATZ: (a) Elza ROOM1091 (Schuppen, Text der Spur P "
   "'Someone must have put out the fire.' msg 20): leerer Loescher liegt von Anfang an am Boden (liegend, plausibler Ort im Schuppen, sichtbar in den Cuts; "
   "optional Untersuchen-Text als PORT-WAHL zur Freigabe). (b) Leon ROOM1090: derselbe leere Loescher liegt erst NACH dem Loeschen ((3,0x84) = 1, ROOM1090 "
   "sub06 @0x02722; vorher nicht), danach dauerhaft (Wiederbetreten, Spielstand). (c) Leons Feuerloescher (heute ROOM1000 Bath-Lockers Spind: sub00 @0x00C24 "
   "Item 0x31 Taken (9,0x86), sub01 @0x00D0C Set(3,0x85)) an einen besseren Ort verlegen: sichtbar mit dem neuen Modell (z.B. Wandhalterung in einem Gang auf "
   "Leons Weg zu 1090 nach dem Hilferuf in 1050), Taken-Bit (9,0x86) und Set(3,0x85) unveraendert (Spielstaende!), Original-Spind-AOT in 1000 auf Leons Seite "
   "stilllegen bzw. umwidmen (Elzas 1001 bleibt mit 30 Patronen), Ort mit Bildern begruenden und in 'Zu entscheiden' fuer den Nutzer nennen. Elza-Raeume: "
   "1091 hat keinen Loescher-Gegenstand. Riegel: Prop vorhanden/nicht vorhanden je Zustand (1091 immer, 1090 erst nach (3,0x84)), Aufnahme am neuen Ort setzt "
   "(9,0x86)+(3,0x85) wie das Original, Feuer 1090 laesst sich damit loeschen (Weg Ende-zu-Ende an der exe), Paket-Gate-Eintraege fuer neue Assets nennen.")),
]

src = io.open(SRC, encoding="utf-8").read()
a = src.index("const SPUREN = [")
b = src.index("]" + chr(10) + chr(10) + "const REGELN")
PLATZ = "/*@@SPUREN@@*/"
t = src[:a] + PLATZ + src[b + 1:]
t = t.replace("befunde_runde36", "befunde_runde37").replace("befunde_runde35/", "befunde_runde36/")
t = t.replace("probes/r35_*.cmake", "probes/r36_*.cmake")
t = t.replace("r36", "r37")
t = t.replace("Runde 36", "Runde 37").replace("(2026-10-10)", "(2026-10-12)").replace("Stand 2026-10-10", "Stand 2026-10-12")
t = t.replace("const BASIS = '78a4a748'", "const BASIS = '%s'" % BASIS).replace("const MIN_TESTS = 592", "const MIN_TESTS = 698").replace("const SLOTS = 5", "const SLOTS = 4")
t = t.replace(" AUFTRAG.md traegt seit ${BASIS} einen Nachtrag 14:00 mit Story-Entscheidungen des Nutzers — lesen.", "")
t = t.replace("description: 'Runde 37: 16 Bau-Spuren", "description: 'Runde 37: 5 Bau-Spuren")
js = ["const SPUREN = ["]
for s_ in SPUREN:
    js.append("  { id: %s, thema: %s, baum: %s," % (json.dumps(s_["id"]), json.dumps(s_["thema"]), json.dumps(s_["baum"])))
    js.append("    punkte: [")
    for p_ in s_["punkte"]:
        js.append("      %s," % json.dumps(p_, ensure_ascii=False))
    js.append("    ],")
    js.append("    hinweise: %s }," % json.dumps(s_["hinweise"], ensure_ascii=False))
js.append("]")
t = t.replace(PLATZ, chr(10).join(js))
io.open(DST, "w", encoding="utf-8", newline=chr(10)).write(t)
print("geschrieben", len(t))
vorlage = t[:t.index("const SPUREN = [")] + t[t.index("const REGELN"):]
print("r36 in Vorlage:", vorlage.count("r36"), "| Runde 36:", vorlage.count("Runde 36"), "| runde36:", vorlage.count("runde36"))
