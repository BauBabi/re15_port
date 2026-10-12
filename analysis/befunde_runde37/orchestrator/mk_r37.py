"""Erzeugt analysis/befunde_runde37/orchestrator/r37.js aus dem Runde-36-Orchestrator (gleiche Maschine:
Bau -> unabhaengige Abnahme -> Nachbesserung, Modi ueber args), mit den Spuren der Runde 37."""
import io, json, re

SRC = r"C:\workspace\git\reAi_v2\analysis\befunde_runde36\orchestrator\r36-rest3.js"
DST = r"C:\workspace\git\reAi_v2\analysis\befunde_runde37\orchestrator\r37_p.js"
BASIS = "bddd6b31"
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
          "Beim Aussteigen aus dem Fahrstuhl fehlt noch die Türsequenz. << stimmt.", "Nachtrag 5: ja, dann mache den noch (Ausstieg Lastenaufzug ROOM3080 -> 4000). Mir ist auch aufgefallen in den Stage2 kleineres aufzügen fehlt noch das sound. Den sollte man auch aus Resident Evil 2 extrahieren."],
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
          "Ja, neuen Texte sind super. Bezug zu ada bei elza kann raus.",
          "Nachtrag 8: Elza kann theoretisch ada begegnen in ihren durchlauf. Ich kenne zumindest keine Aussage der Entwickler die sagt, das sie sich nicht begegnen können. Nur das Elza und Leon sich niemals begegnen. Aber wenn Elza nur eine Leiche von Ada findet, kennt sie ada nicht. Dann ist es halt nur - a woman."],
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
          "Außerdem finde ich den FEuerlöscher im Spind bei Leon noch unglücklich und würde ihn gerne woanders platzieren....", "Nachtrag 4b: ah, it can only lay there - if leon when he is asked to discard it select \"yes\".", "Nachtrag 4c: If he doesn't he has the text empty fire extinguisher in the player inventory."],
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
 dict(id="J", thema="bissarm", baum="r37_bissarm",
  punkte=["Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird."],
  hinweise=("Keine F9-Marke dazu — beide Lesarten messen: (1) der Zombie-Arm, der beim Biss/Griff an Leon haengt (Griff von vorn, von hinten, "
   "Kriecher am Bein; Leon UND Elza), fehlt in manchen Bildern/Faellen; (2) ein angeschossener, noch dranhaengender (baumelnder) Arm des Zombies "
   "fehlt waehrend des Bisses. Port: enemy_ai_re2_zombie.c (Griff-FSM: grab_kill_ctr/grab_choreo in include/re15_actor.h, Griff [0] kopiert den "
   "Zombie-Anker auf den Spieler, Opfer-FSM des Spielers, Abriss-/Drift-Logik der Arme ~Z.6693ff 'Drift-Felder eines abgerissenen ARMS (Flagwort "
   "0x4A)', Kinder abgerissener Teile ~Z.5584), Teile-Sichtbarkeit/Gore (md1_gore, Memory reai-v2-gore-vollausbau), emd_common.c. Schritt 1: alle "
   "Biss-Varianten an der exe reproduzieren (RE15_ENEMY_DBG, RE15_ANIM_TRACE, Framedumps RE15_WINDOW_SCALE=1, je Bild welche Teile des Zombies "
   "gezeichnet werden), auch mit vorher angeschossenem Arm (Schaden an Teil 3/6 o.ae.). Schritt 2: RE2-Original (RE2_Quellcode_Overlays/, "
   "ghidra_re2_Leon.txt, EM10-Griff/Biss, Teile-Flags +0x?? fuer abgerissene/baumelnde Arme) belegen, welche Teile RE2 waehrend des Bisses zeigt "
   "und wie der baumelnde Arm mitgeht; wo moeglich RE2 an DuckStation messen. Schritt 3: Fehlendes byte-true nachbauen. Riegel je Variante "
   "(Teile-Sichtbarkeit je Bild des Bisses), Runde-36-Riegel (Kleidung/Griff-Blut B, Schuss C) gruen halten.")),
 dict(id="K", thema="roy", baum="r37_roy",
  punkte=["Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich seine Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben....."],
  hinweise=("Roy = NPC-Typ 0x47 (analysis/story_leon_elza/B_figuren.md §1.1: dasselbe Typ-Byte wie Elliot in Leons ROOM1170 sub15 @0x017D2): "
   "ROOM1011 sub00 @0x009F6 Roy (-2000,0,7450) verwundet, Dialog sub02 msg 0-17 (Ihre Aufnahme main16.5 laeuft, wenn er bewusstlos wird), danach "
   "sub00 @0x00A3E Roy bewusstlos (AOT Slot 5 -> sub06 Prison Key/Zettel); ROOM1021 main00 @0x01E0A Roy als Zombie (-4614,0,-16919), Plc_dest "
   "@0x020B2 auf John zu, John schiesst (Runde 36 Spur L: zwei Schuesse + Todeslaut), Roy Clip 1/7 @0x020DE = Fallen. Runde 36 L-Abnahme: Roy "
   "sackt in 1021 nicht sichtbar zusammen, in 1011 richtet er sich F1946..F1958 auf und bleibt aufrecht (NACHLAUF #21/#67) — derselbe Fehler. "
   "MODELL: die Modelle liegen in shared_assets/PSX/EMD/CDEMD0.EMS und CDEMD1.EMS (vermutlich je Spieler-Familie — PRUEFEN, welches EMD/EMS das "
   "Original fuer Typ 0x47 in Elzas Raeumen laedt (Lader in der EXE, Spieler-Familie PL04, Index), und was der Port laedt (emd/ems-Lader, "
   "enemy_common.c / npc-Code)). ANIMATIONEN: Clip-Nummern der Skripte (Plc_motion/Plc_flg/Plc_dest in 1011/1021) gegen die Bank des richtigen "
   "Modells; Dauerschleife = dieselbe Fehlerklasse wie frueher bei Leons NPCs — die Lehren nachlesen (Memory: reai-v2-platzhalter-als-verhalten "
   "'Ada Clip 0 = Laufzyklus', reai-v2-npc-crossfade-decay, reai-v2-npc-global-yield, reai-v2-runde18-ada-eintritt; Dossiers analysis/befunde_runde18*, "
   "Plc_motion-Ende/Halten des letzten Bilds). Am Original messen (DuckStation-Savestate mit Elza in 1011/1021 — stage_saves/ sauber, sonst per "
   "Debug-Menue JUMP mit Elza; GDB wie Spur R), Framedumps Original gegen Port je Szenenabschnitt. Riegel: Modell-Kennung je Raum, Clip je "
   "Szenenabschnitt, kein Clip laeuft ueber sein Ende hinaus in Schleife (wo das Original haelt), Elliot bei Leon (1170) unveraendert.")),

 dict(id="L", thema="re2zombie", baum="r37_re2zombie", basis="e399ee3f",
  punkte=["Ein weiteres Problem das ich immernoch habe - die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang.",
          "das soll nicht nur da gelten, das darf generell für die Zombies gelten",
          "schaue einfach nach wie Resident Evil 2 das macht",
          "Orchestrator-Auftrag L (AUFTRAG.md): die drei vorbestehenden Abweichungen der RE2-Zombie-KI vom RE2-Original schliessen — (a) die drei Produzenten des Arme-hoch-Gangs +0x4 = 0x201 (EXEC[2]) im Gang-Executor @0x80101D68-DBC (Timer), @0x80101EC4-F10 (Nahbereich), @0x80101F14-F60 (Spieler rennt) byte-true scharf schalten, NACHDEM der Nahkampf-Zusammenbruch (test_re2_zombie_abc Positiv-Kontrolle 3523 -> 0 mit (1)+(2)) aufgeklaert und an der Ursache behoben ist; (b) DECISION[0] Block 3 committet bei (rand & 1) == 1 wie @0x801013c8; (c) EXEC[0] P1: Selbstwecker im selben Takt, P2 erst im naechsten wie @0x80101540-a0 / @0x8010159c. Ergebnis an der exe: die Zombies heben wie in RE2 die Arme und kommen auf Leon zu, greifen und beissen wie in RE2; der Nahkampf wird nicht schlechter."],
  hinweise=("BASIS: dein Baum steht auf e399ee3f = abgenommener Stand der Spur H (9c2a825d: RE1.5 grid 0x00 -> RE2-Deskriptor 6 'Gang ab Spawn', engine/src/zombieaktiv_r37.c) "
   "+ AUFTRAG/VERTRAG vom master-Stand (Orchestrator-Auftrag L, VERTRAG Ausnahme fuer enemy_ai_re2_zombie.c). Lies in deinem Baum analysis/befunde_runde37/H_zombieaktiv.md (Abschnitt OFFEN, RE2-Messung) "
   "und H_abnahme_0.md (H1, N1..N5). CODE: re15_port/engine/src/enemy_ai_re2_zombie.c — OFFEN-Block 'DIE DREI PRODUZENTEN VON +0x4 = 0x201' (~Z.1633-1695: Disassembly je Zeile "
   "zitiert, Bisektion nur (1) -> 3756, nur (2) -> 3589, (1)+(2) -> 0; Verdacht dort: der Anmarschweg endet ausserhalb des +-0x400-Spielerkegels, weil EXEC[2] nur mit +-16 steuert "
   "statt mit der Gang-Rate 8+16 — PRUEFEN, nicht uebernehmen), EXEC[2] @0x80102260 ('DER ZWEITE GANG (Arme oben)'; Nutzer-Report 2026-08-22: fetter Zombie 0x11 ROOM1140 laeuft "
   "im Arme-hoch-Gang auf der Stelle), re2z_decide_stand (DECISION[0] Block 3), EXEC[0] P1 Wander-Wurf (sub_state_2, Wecker-Sperre). RE2-QUELLE: EMZ0.BIN (RE2-Zombie-Overlay; "
   "info/re2leon/, RE2_Quellcode_Overlays/), ghidra_re2_Leon.txt — SELBST disassemblieren, jede Konstante mit @0x, richtige Binaerdatei. TEST: test_re2_zombie_abc (Positiv-Kontrolle "
   "Nahkampf; Pistole ~1700) — zuerst klaeren, WAS er zaehlt. SCHRITT 1 MESSEN: den Zusammenbruch mit (1)+(2) reproduzieren und Bild fuer Bild die Ursache finden (Zustandswort +0x4, "
   "Steuerwinkel, Abstand, Kegeltest, Griff-Einstieg; RE15_ENEMY_DBG); am RE2-Original gegenmessen wie Spur H (wie oft/wann wechselt ein RE2-Zombie in 0x201, wie steuert er, wann "
   "greift er). SCHRITT 2: Ursache byte-true beheben, dann die drei Produzenten + (b) + (c) scharf. SCHRITT 3: Zensus der uebrigen OFFEN-Marken der RE2-Zombie-KI, die Bewegung/Angriff "
   "betreffen (u.a. Kriecher-Test P1 ~Z.3120, Kriecher +0x5 > 2 ~Z.3603): je Marke Adresse + 'wirkt auf die Aktivitaet ja/nein'; was wirkt, mitschliessen oder mit Adresse begruenden. "
   "STROM: re2z_rand ist EIN gemeinsamer Zufallsstrom — jede Aenderung verschiebt deterministische Riegel, auch fremder Spuren (r37_zombieaktiv der Spur H, unit_r36_zombieki_*, "
   "integration_r36_gegnerzahl, test_re2_zombie_*, Biss-/Griff-Riegel); Strom vorher/nachher messen (Wuerfe je Takt), JEDEN verschobenen Riegel mit altem/neuem Wert + Begruendung im "
   "Dossier auflisten und nur mit Beleg anpassen. Griff-/Biss-Code (Spur J bissarm arbeitet parallel daran) und re2z_init (Haken der Spur H) nur lesen. AN DER EXE (RE2-KI = Standard, "
   "Leon UND Elza): Gang 1130 nach Irons' Tod, 1030/1040, 1010/1100 — Zombies kommen mit erhobenen Armen, greifen; an Zombies vorbeirennen (Produzent 3); Messer/Nahkampf geht. "
   "RIEGEL ueber probes/r37_re2zombie.cmake: je Produzent ein Pin (Wechsel nach 0x201 unter seiner Bedingung, Gegenprobe ausserhalb), Nahkampf-Positiv-Kontrolle > 0 (gemessener Wert), "
   "(b)/(c) gegen das Original-Muster (Takt-Folge).")),

 dict(id="M", thema="kiste", baum="r37_kiste",
  punkte=["In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden.",
          "Außerdem ist mir aufgefallen, das beim scrollen/bewegen in der Kiste währendessen die Items oberhalb des Kastens liegen. Das darf nicht sein, der Kasten muss layertechnisch über den Items liegen."],
  hinweise=("KISTE = Item-Box-Bildschirm nach RE2 (Nutzer 2026-08-30/31: 'GENAUSO wie in Resident Evil 2'). CODE: re15_port/engine/src/re15_inv_screen.c Block "
   "'7b. DIE BOX ALS LISTE' (~Z.1690-1800: LIST_X0 12, LIST_X1 204, NAME_X 16, ICON_X 158 'RE2: x 155', BAR_X 208, ROW0_Y 41+RE15_BOX_PANEL_DY, ROW_DY "
   "RE15_BOX_ROW_PX, Gleiten ry += st->box_pixoff, Symbol box_cell_icon_sized RE15_BOX_ICON_W/H, Ziffern box_digits) und der Block danach 'DAS ORIGINALE "
   "RE2-BOX-PANEL, 1:1'; Zeichner platform/pc/src/inv_render_pc.c. ⛔ REIHENFOLGE: die Op-Liste wird von HINTEN nach VORNE gerastert (ein frueher Eintrag liegt "
   "OBEN; Memory reai-v2-zeichenreihenfolge-invers) — 'Rahmen ueber den Items' heisst: Rahmen-Ops vor die Zeilen-Ops ODER die Zeilen auf das Fenster "
   "abschneiden — so, wie RE2 es macht (belegen). RE2-BELEGE: analysis/itembox_re2/re2-box-screen.md, re2-box-panel-geometrie.md, referenz/ (Bilder aus dem "
   "laufenden RE2), Zeichenschleife FUN_800710dc (y-Start 0x7f, Schritt -0x14), Rahmen-Sprites @0x800a9c38 (x 7..218 / y 14..163), ghidra_re2_Leon.txt / "
   "RE2_Quellcode_V2 — selbst disassemblieren, jede Konstante mit @0x. SCHRITT 1 MESSEN an der exe (Kiste ROOM1150 und weitere Kisten-Raeume, Leon UND Elza; "
   "Framedumps RE15_WINDOW_SCALE=1): (a) Zensus ALLER Item-Namen (Pixelbreite im Port-Zeichensatz, emit_name_at) gegen ICON_X — welche Namen reichen in das "
   "Symbol; (b) Scrollen Bild fuer Bild (box_pixoff, nach oben und unten, Halten der Taste) — wo liegen Zeilen ueber dem Rahmen bzw. ausserhalb des Panels. "
   "SCHRITT 2 RE2: wie legt RE2 Symbol und Namen (Symbol-x, lange Namen, Mengen-Ziffern), und wie verdeckt bzw. schneidet RE2 die gleitenden Zeilen am Rahmen "
   "(OT-Reihenfolge, DR_AREA/Clip, Maske)? Am RE2-Original messen (referenz/ oder DuckStation wie fruehere RE2-Messungen). SCHRITT 3 BAUEN: Symbol so weit "
   "rechts wie moeglich (zwischen Namen und Scrollbalken, Mengen-Ziffern lesbar; Lage begruenden), gleitende Zeilen vom Rahmen verdeckt/abgeschnitten; "
   "Auswahl-Band, Ziffern, Scrollbalken, Cursor, Inventar-Gitter rechts unveraendert. RIEGEL ueber probes/r37_kiste.cmake: Namen-Symbol-Ueberlappung = 0 "
   "fuer ALLE Item-Ids, kein Zeilen-Pixel ueber dem Rahmen bzw. ausserhalb des Fensters in jedem Gleit-Bild, Rahmen-Pixel unveraendert; test_itembox_screen, "
   "test_itembox_transfer, test_room1150_itembox gruen halten. Paket: keine neuen Assets erwartet.")),

 dict(id="O", thema="szene11c0", baum="r37_szene11c0",
  punkte=["Und die Dialogszene mit Ada/Marvin nach Irons Tod ist auch noch nicht so geil von den Animationen....",
          "Erst schauen sie komplett in die falsche Richtung. Dann drehen sie sich einmal um 180 grad um. Dann dreht sich Ada noch mal um 90 grad um. Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada.... da musst du noch mal ran, das ist noch nicht so gut.",
          "Nachtrag 12: zum schluss dialog ada/marvin - wenn Ada sagt: \"Marvin!\" kann sie sich gerne den weglaufenden Marvin nach drehen."],
  hinweise=("SZENE = Programm 4 der Irons-Szene: re15_port/engine/src/irons_tod_1150.c k_p_11c0 (~Z.391-470; Herleitung im Kommentar: Runde 35 Spur L "
   "analysis/befunde_runde35/L_cut1150.md, Runde 36 Spur K2 analysis/befunde_runde36/K_*.md + K_belege/k2_*). Heutiger Ablauf: (1) beide drehen sich "
   "zum Gebaeude (Tuer Slot 0 @0x01712, Mitte (-25100,17250) — liegt HINTER der Kamera Cut 13 = RID ROOM11C0 @0x200), (2) Ada dreht sich zu Marvin, "
   "Marvin nur mit dem Kopf (OP_NECK1), (3) Marvin dreht sich zu Ada + Nicken, (4) 'Oh, no, Leon!' wieder zum Gebaeude gedreht, (5) zu Ada, Arm, "
   "(6) rennt (Modus 5), (7) Ada sieht nach + 'Marvin!...'. Orte: Ada (-8965,-14347) (ROOM11C0 sub00 @0x01770), Marvin (-8163,-13748) Spawn-Gier 475 "
   "(OP_EM). Vokabular aus dem Original-Dialog dieses Raums (ROOM11C0 sub02 Leon<->Ada: Plc_neck Modus 1 `41 01 x 0 z 64 00` @0x01886/@0x018C0, "
   "Plc_dest Modus 9 Drehen @0x01890/@0x018CA, Ankunfts-Warten sub05/sub06 @0x01C3E/@0x01C50, Nicken sub03 @0x01BB2-BCA). SCHRITT 1 MESSEN an der "
   "exe (echter Weg wie integration_r35_cut1150 Lauf A: probe_r35_cut1150_karte 1130 'nach10f0;ersteszene', CONTINUE, Tuer, Szene; RE15_FRAMEDUMP "
   "Serie mit RE15_WINDOW_SCALE=1 im 11C0-Abschnitt; dazu je Bild Gierwinkel + Kopfdrehung (Plc_neck-Zustand) von Ada und Marvin loggen): jede Drehung "
   "mit Bild, Start-/Endwinkel, Dauer — die Beschreibung des Nutzers Punkt fuer Punkt belegen (falsche Anfangsrichtung, 180-Grad-Drehung, 90-Grad-"
   "Drehung Adas, Marvin spricht ohne Ada anzusehen). Pruefen, ob Plc_neck bei Marvin (Typ 0x40, NPC-Bibliothek) sichtbar wirkt (Kopfknochen dreht sich "
   "im Bild?) und wie die Anfangs-Gier von Ada entsteht. SCHRITT 2 VORBILD: RE2-Dialogszenen zweier NPCs/Spieler (RE2_Quellcode_Overlays/, info/re2leon "
   "RDT-SCD, z.B. Leon/Ada- oder Marvin-Szenen): wie halten sie sich zugewandt, wie reagieren sie auf ein Geraeusch (Kopf vs. Koerper), wie lange "
   "drehen sie. SCHRITT 3 BAUEN: Anfangs-Gier beider so, dass sie sich zugewandt sind (Winkel aus den Positionen, atan2-Form des Ports zitieren); "
   "Knall-Reaktion nur Kopf/kurze Koerperdrehung, keine 180 Grad; wer spricht, sieht den anderen an (Kopf + Koerper), auch Marvin bei allen drei "
   "Zeilen bis zum Losrennen; Ada sieht ihm nach. Beats, Texte, Gesten, Laufziel und Schnitt zurueck nach 1150 bleiben; Takte PORT-WAHL. NICHT "
   "anfassen: andere Programme in irons_tod_1150.c (Spur H: Nachspawn 1130), k_msg28 (Nachtrag 10). RIEGEL ueber probes/r37_szene11c0.cmake: je "
   "Sprech-Abschnitt Gier/Kopf des Sprechers zeigt auf den anderen (Toleranz begruenden), groesste Einzeldrehung je Figur unter 90 Grad bzw. "
   "begruendet, keine Drehung um ~180 Grad; integration_r35_cut1150 und integration_r36_szene1150 (Dialog-11C0-Riegel unit_r36_szene1150_dialog11c0) "
   "gruen halten bzw. mit Beleg anpassen. Bildbeweis: Vorher/Nachher-Streifen im Dossier.")),

 dict(id="N", thema="elzaems", baum="r37_elzaems", basis="99b4b13f",
  punkte=["Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich seine Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben..... (Nachtrag 7 — fuer Roy durch Spur K erledigt; Spur N schliesst dieselbe Fehlerklasse fuer ALLE uebrigen Typen)",
          "Orchestrator-Auftrag N (AUFTRAG.md): in Elzas Szenario laedt der Port jeden Gegner-/NPC-Typ wie das Original aus CDEMD1.EMS (FUN_80022300: Elza-Bit bltz @0x80022334 -> Datei 0x27 @0x800223ac, TOC @0x80073178), nicht aus Leons CDEMD0-Daten; sichtbar u.a. 0x30 Birkin (anderes Modell in ROOM3071/5081/5091/50F1) und 0x42 (anderes Modell in ROOM11B1 @0x01058, ROOM3091 @0x014F4); Aktions-/Opfer-Banken der Zombies 0x10-0x18, Kraehe, 0x27, 0x29, 0x2b. Leon bleibt bytegleich."],
  hinweise=("BASIS: dein Baum steht auf 99b4b13f = abgenommener Stand der Spur K (238bb610) + AUFTRAG/VERTRAG vom master-Stand. Lies in deinem Baum "
   "analysis/befunde_runde37/K_roy.md (v.a. §6.1 OFFEN 1, Zensus der Typ-Unterschiede, K_belege/emstab.py, emsdiff.py, k6_*) und K_abnahme_0.md, "
   "dazu J_bissarm.md (OFFEN 2: RE1.5-Flavor mit Elza, Hand 229..284 statt 20..65 ueber der Schulter mit der Leon-Bank). CODE der Spur K: "
   "engine/src/roy_r37.c + include/re15_roy_r37.h (Weiche nach dem Elza-Bit), engine/src/gen/ems_toc_r37.inc (BEIDE TOCs), "
   "platform/pc/src/roy_pc.c (Lader aus CDEMD1), Haken platform/pc/main.c pc_enemy_read_re15_emd (~Z.851), enemy_common.c, anim_select_common.c, "
   "enemy_ai_common.c. SCHRITT 1 ZENSUS: je Typ (alle Typen beider TOCs) Byte-Vergleich CDEMD0 gegen CDEMD1 je dir-Eintrag (Mesh, EDD/EMR-Banken, "
   "TIM): was unterscheidet sich, wo kommt der Typ in Elzas Raeumen vor (alle Elza-RDTs, Sce_em_set/Typ-Zensus), welche Port-Pfade lesen welchen "
   "dir-Eintrag (RE1.5-KI-Flavor, RE2-KI-Hybrid re2_hybrid_apply, NPC-Bibliothek, Opfer-Banken des Spielers). SCHRITT 2 ORIGINAL: am Original "
   "messen, was es fuer Elza laedt (DuckStation-Spielstaende mit Elza wie K — Elza-Bit, Banken im RAM gegen CDEMD1; sauber = nicht "
   "PATCHED-EXE_*), mindestens fuer 0x30 Birkin und 0x42 und einen Zombie-Typ; RE-Belege fuer den Lader (FUN_80022300 ganz lesen: gilt die Weiche "
   "fuer JEDEN Typ? Sonderfaelle?). SCHRITT 3 BAUEN: die K-Weiche auf alle Typen verallgemeinern (ein Lade-Weg: Elza -> CDEMD1-Blob des Typs, "
   "Leon -> wie bisher), Speicher/Entladen (Runde 35 Spur I entladen_pc.c) beachten; Roy (K) und Elliot (Leon 1170) bytegleich halten. "
   "Beruehrt H (Zombie-Spawn), J (Griff-Arm: Opfer-Bezug Elza in bissarm_r37.c rechnet heute mit Leons Bank + Schulter-Differenz — mit der "
   "CDEMD1-Opfer-Bank pruefen, ob das bleibt oder wegfaellt; im RE2-KI-Standard nur messen) und L (RE2-KI) — deren Code nur lesen, Wechselwirkung "
   "im Dossier nennen. AN DER EXE (Elza, echte Wege bzw. Spielstand+CONTINUE): Birkin 0x30 in 3071/5081/5091/50F1 (Modell wie das Original), 0x42 in "
   "11B1/3091, ein Zombie-Griff an Elza mit RE1.5-KI (Hand auf IHRER Schulter, J-Messung 20..65), Kraehen-/0x27-Opfer; Framedumps Original gegen Port. "
   "RIEGEL ueber probes/r37_elzaems.cmake: je Typ Quelle (CDEMD0/CDEMD1) nach Szenario, Bytegleichheit der geladenen Banken mit dem Original-Blob, "
   "Leon unveraendert (Gegenprobe), Roy-Riegel der Spur K gruen. Neue Assets: keine (CDEMD1.EMS liegt im Paket).")),

 dict(id="P", thema="masken36", baum="r37_masken36", basis="bddd6b31",
  punkte=["Orchestrator-Auftrag P (AUFTRAG.md, Regression): die ergaenzten Vordergrund-Masken der Stages 3..6 (30 MSK-Dateien ROOM3040..ROOM6031, erzeugt b8db7b2b 2026-09-03 fuer Cuts mit Vordergrund-Grafik im BSS, aber ohne Masken-Records im RDT) werden seit 53741fe5 (2026-09-08) fuer NIEMANDEN gezeichnet, weil der Lader fuer R15M-Masken einen eigenen Atlas verlangt, den nur die Raeume mit den Freistellungen des Nutzers haben. Ziel: diese Masken verdecken die Figuren wieder wie am 2026-09-03 gebaut (BSS-Atlas), ohne die Raeume mit eigenem Atlas (Stage 1/2, Spur A) zu veraendern; Figuren VOR einem Objekt werden nicht verdeckt.",
          "Bezug zum Nutzer: Vordergrund-Verdeckung ist seit Runde 19 ein Dauerthema des Nutzers (F9-Marken, Freistellungen); in den Stages 3..6 fehlt sie seit v0.6.x unbemerkt."],
  hinweise=("BASIS: dein Baum steht auf bddd6b31 = abgenommener Stand der Spur A (06ac2b56: Masken je Tiefenstufe ROOM1100, Zwillings-Rueckfall "
   "Elza -> Leon re15_pri_masken_raum, Zensus tools/maske/zwilling_zensus.py) + AUFTRAG/VERTRAG vom master-Stand. Lies A_pri1100.md und "
   "A_abnahme_1.md (M2: ROOM3040 Cut 0..7 fg_atlas=0, Cut 2 mit/ohne Masken 0 Punkte) in deinem Baum. GESCHICHTE: git show b8db7b2b (Generator "
   "analysis/esp_masken_2026-09-03/gen_masken.py + pruef_gen.py: Atlas-Bloecke per NCC im Hintergrund wiedergefunden, Masken-Byte = src, "
   "Transparenz Palettenindex 0, Tiefe konservativ x1.10; Container R15M, Original-Sektion hat IMMER Vorrang, Nachschlag nur bei NULL-Sektion "
   "FF FF FF FF) und git show 53741fe5 (re15_pri_load_cut_atlas_ex: 'nachgezeichnete Rechtecke tragen srcX/srcY fuer IHREN Atlas', Merker aus "
   "dem R15M-Zweig) — danach die Freistellungs-Commits 9474f9bb/692b4f10/7b24600f (eigene ROOMxxxx_PRI*.TIM). CODE: platform/pc/src/bg_pc.c "
   "(re15_pri_load_cut_atlas_ex, beide Atlas-Pfade, A-Haken), entladen_pc.c re15_entladen_msk, render_pc.c Masken-Tiefentest, "
   "engine re15_pri_msk_section_offset. SCHRITT 1 MESSEN: Zensus aller MASKS/*.MSK (alle Stages): je Raum/Cut eigene Atlas-Datei ja/nein, "
   "R15M-Sektion ja/nein, Original-RDT-Sektion NULL ja/nein, fg_atlas an der exe; welche Cuts sind seit 53741fe5 tot (Erwartung laut A: alle "
   "30 Stage-3..6-Dateien; pruefe auch Stage 1/2 auf Raeume/Cuts ohne eigenen Atlas). Pro totem Cut: auf welchen Atlas zeigen die Masken "
   "(BSS-SLD-Atlas des Cuts? Bytes gegen gen_masken.py-Ausgabe pruefen). SCHRITT 2 BAUEN: R15M-Cut OHNE eigenen Atlas -> BSS-Atlas des "
   "Cuts (Verhalten vor 53741fe5), R15M-Cut MIT eigenem Atlas -> unveraendert; Elza-Zwilling (A) unberuehrt. SCHRITT 3 PRUEFEN: Blit-Pruefung "
   "je Cut (Masken durch den Atlas auf den Hintergrund: Bild bleibt praktisch gleich, gemessene Farbabweichung angeben); an der exe in "
   "mehreren Raeumen jeder Stage 3..6 Leon (und Elza) HINTER einem Vordergrund-Objekt (verdeckt) und DAVOR (nicht verdeckt), Framedumps "
   "RE15_WINDOW_SCALE=1 + Punktzaehlung wie A; Gegenprobe Stand vor P. Memory-Lehren: Maske = Tiefen-Profil je Zeile, 'deckt ab' != "
   "'verdeckt', flache Maske verdeckt nichts, Masken nur wo man hinkommt, ⛔ Nutzer-PRI ohne Hash-Liste = Basis rot (pri_hashes.py im selben "
   "Commit, falls Hash-Listen betroffen). RIEGEL ueber probes/r37_masken36.cmake: Zensus (je Datei/Cut Atlas-Quelle, kein toter Cut), "
   "Blit-Pruefung, Verdeckungs-Pins je Stage mit Gegenprobe; Spur-A-Riegel (r37_pri1100*, pri_masken, r36_pri*) gruen.")),
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
    js.append("  { id: %s, thema: %s, baum: %s,%s" % (json.dumps(s_["id"]), json.dumps(s_["thema"]), json.dumps(s_["baum"]),
              (" basis: %s," % json.dumps(s_["basis"])) if s_.get("basis") else ""))
    js.append("    punkte: [")
    for p_ in s_["punkte"]:
        js.append("      %s," % json.dumps(p_, ensure_ascii=False))
    js.append("    ],")
    js.append("    hinweise: %s }," % json.dumps(s_["hinweise"], ensure_ascii=False))
js.append("]")
t = t.replace(PLATZ, chr(10).join(js))
t = t.replace("Basis master ${BASIS}", "Basis ${BASIS}")
io.open(DST, "w", encoding="utf-8", newline=chr(10)).write(t)
print("geschrieben", len(t))
vorlage = t[:t.index("const SPUREN = [")] + t[t.index("const REGELN"):]
print("r36 in Vorlage:", vorlage.count("r36"), "| Runde 36:", vorlage.count("Runde 36"), "| runde36:", vorlage.count("runde36"))
