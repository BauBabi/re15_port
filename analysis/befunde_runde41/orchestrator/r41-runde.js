export const meta = {
  name: 'r41-runde',
  description: 'Runde 41: Spuren je Ermittlung+Bau -> unabhaengige Abnahme -> Nachbesserung, in eigenen Arbeitsbaeumen (Semaphor, hoechstens 4 gleichzeitig)',
  phases: [
    { title: 'Ermittlung+Bau', detail: 'Dossier, Messung, RE-Beleg (RE1.5/RE2), Code, Tests, gezielte Laeufe im eigenen Baum' },
    { title: 'Abnahme', detail: 'unabhaengig gegen den Wortlaut des Nutzers am gebauten Stand gemessen' },
    { title: 'Nachbesserung', detail: 'Maengel der Abnahme beheben, erneut messen' },
  ],
}

const ROOT = 'C:/workspace/git/reAi_v2'
const CFG = args || {}
const BASIS = CFG.basis || 'master'
const SLOTS = 4

const SPUREN = [
  { id: 'A', thema: 'wasser20b0', baum: 'r41_wasser20b0',
    punkte: [
      'Runde 41 startet mit dem Wasserschwall in 20B0 im Original. (vereinbart: "in 20B0 laeuft ja das Wasser mit grossen Druck aus der Tuer heraus. Uebrigens ist das in unseren Port noch kaputt!" -> im Original messen, im Port reparieren)',
      'In STAGE2 fehlt noch der Wassereffekt vom rein springen von der Kante in das Wasser. Auch die NPCs teilen die gleichen Effekte wenn sie durch das Wasser gehen.',
    ],
    hinweise: 'ROOM20B0/20B1 = Variante von ROOM2000 (Auswasch-Szene; 35 Sce_em_set Typ 0x24 EM024, Flag (3,0xEB), Subs sub02/sub05 — scd-disassembly). Port: enemy_ai_common.c ab ca. Zeile 13563 "AMBIENT FX-EMITTER (type 0x24, EM024)": Wurzel 0x8010ee9c (STAGE2.BIN), +0x5-Dispatch @0x80118d58 [0]=0x8010f130 [1]=0x8010f3fc -> jal 0x80019700 (ESP-Spawn) stehen als OFFEN (nicht gebaut) — genau das ist der fehlende Schwall. ORIGINAL MESSEN: DuckStation (Skills re15-room-capture: Debug-Menue JUMP in 20B0 — Raum ist eine Variante, ggf. Flag (3,0xEB) setzen; re15-savestate-ghidra; psx_dev/re15_reborn/psxtest.sh), Framedump-Serie des Schwalls aus der Tuer + ESP-Ids/Phasen/Positionen aus RAM und Disasm (re15_disasm.py --bin STAGE2.BIN 0x8010f130 / 0x8010f3fc: a0-Aufbau (phase<<8)|(id<<24)|(kind<<16), Spawnlage, Takt); Port: RE15_FX_LOG + Framedump (RE15_WINDOW_SCALE=1), RE15_DEBUG_JUMP=20B0. Umsetzung in neuer Datei wasser20b0_r41.c (Haken im 0x24-Zweig), jede Konstante @0x.... Sprung ins Wasser: wo springt der Spieler in STAGE2 von einer Kante ins Wasser (2050 Steg -> Becken, 2090 Rampe, 20A0?) — Original: RE1.5 Wasser-Effekt beim Landen (Spieler-Wasser-Flag, ESP-Routine Spritzer; Port wasser_r38.c = Wat-Effekte, wasserzombie_r38.c, re15_esp.c; suche im Player-Landehandler nach dem Wasser-Zweig @0x...); fehlt er in RE1.5, gilt RE2 (Kanal-Wasser: Landen/Watten-Spritzer des RE2-Spielers, ghidra_re2_Leon.txt / RE2_Quellcode_V2) als Beleg. NPC-Effekte: dieselbe Spritz-/Wellen-Routine fuer NPC-Aktoren (Ada/Marvin/John/Sherry, Typen 0x40..0x4b) im Wasser — wo ruft der Port den Spieler-Effekt (grep -n wasser/water in game_step_common.c, actor_locomotion.c, wasser_r38.c) und wie entscheidet das Original (Aktor-Flag "im Wasser" je Entity, nicht nur Spieler?). Pins: FX-Zaehlung je Punkt (RE15_FX_LOG), Framedump-Differenz, Riegel unit_r41_wasser20b0_*.' },

  { id: 'B', thema: 're2gator', baum: 'r41_re2gator',
    punkte: [
      'Der Aligator in ROOM 2090 hat bereits einen finisher in Resident Evil 2. Wir haetten garkeinen eigenen bauen muessen... Ich will das du den Finisher und fehlende Animationen aus Resident Evil 2 extrahierst!',
      'Generell: Du kannst fast alles vom Aligator Fight von Resident Evil 2 uebernehmen. Ich moechte das der Aligator beim hinterher laufen das gleiche Verhalten bei Animation und schreien macht. An gleichen Stellen beschleunigt usw. Ich moechte das auch die Waende ab und an wackeln wie im Original und Putz runter faellt. Das einzige was wir natuerlich nach wie vor brauchen, ist das der Aligator weiterhin ueber die Platform hinten klettern kann.',
      'Kanister-Mechanik (Teil des G-Punkts, hier die Alligator-Seite): Wenn der Aligator dran vorbei kommt, soll er ihn aufnehmen, GENAUSO wie im original Resident Evil 2. Sobald er den Kanister im Maul hat, soll man auf ihn schiessen koennen, und die Sterbe Animation mit geplatzen Kiefer etc. vom Aligator soll genauso kommen wie im Original. (Mechanik mit Testhaken; die Szene, die den Kanister wirft, baut Spur G in Welle 2)',
    ],
    hinweise: 'Eigentuemer: engine/src/enemy_ai_boss_gator.c (2393 Zeilen, Nutzer-Design v0.7.35-79: Phasen LURK/CHASE/LUNGE/FLINCH/CROSS/DIE/GUARD + FRESSEN-Finisher; Memory C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/reai-v2-gator-bosskampf.md ZUERST ganz lesen — dort stehen Klemm-/Telemetrie-/Massstabs-Lehren, Testhaken RE15_GB_TEST, gator_boss.log) und analysis/befunde_2026-09-19/gator-finisher-sounds.md (RE2-EM23: SE 4 Bruell-Ruf @0x80100d84, Lunge Clip 2->3, Biss-Fenster Clip 3 f>=114, Opfer-Anker-Skalierung Leon im Maul; Sonden probe_r16_gator_*). RE2-Daten: info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN (Alligator-Gegner-Overlay, .claude/skills/re15-psx-disasm/scripts/re2_disasm.py), RE2-Modell EM23 (Bank, die pc_enemy_load_ex fuer 0x23 laedt — grep EM23/em23 in platform/pc/src), RE2-Kampfraum: Sce_em_set-Scan (Typ 0x23) ueber info/re2leon/PL0/RDT/ROOM5*.RDT -> dessen SCD (Zonen/Ausloeser fuer Beschleunigung + Schreie, Kanister-Knopf/Objekt, Wand-Wackeln = Kamera-Shake-Opcode/ESP Putz-Partikel — alles mit Offsets), Overlay-Zustandstabelle (Zustaende, Clip-Nummern, Geschwindigkeiten je Zustand, SE-Rufe, Kanister-Phase: Objekt in Reichweite -> Aufnahme-Clip -> Maul-Objekt -> Schuss-Treffer-Gate -> Kiefer-Platz-Tod (Clip + ESP + SE), Finisher-Zustand). DuckStation mit RE2-Disc nur, wenn ein RE2-Image vorliegt (pruefen: info/re2leon ist der Dateibaum, kein .bin/.cue) — sonst statisch belegen und am Port messen. UMSETZUNG: RE2-Verhalten in neuer Datei gator_re2_r41.c (+ include), enemy_ai_boss_gator.c nur Haken/Umschaltung; Nutzer-Vorgabe: Klettern ueber die Rampe/Plattform (gb_cross_*, GUARD) BLEIBT; Finisher = RE2-Finisher (Clips/Anker aus EM23, nicht der eigene Halbkoerper-Wirbel); Verfolgung = RE2-Zustaende/Clips/Speed/SE; Waende wackeln + Putz = Kamera-Shake (re15_camera/render) + ESP-Partikel wie RE2 (Ids/Phasen belegen; RE2-ESP-Kind ohne RE1.5-Pendant: Textur/Partikel aus RE2 nach shared_assets/RE2/, im Dossier nennen); Kanister: Port-Prop (RE2-Objekt-Modell des Kanisters aus dem RE2-RDT/ITEM nach shared_assets/RE2/), Lage per Testhaken RE15_GATOR_KANISTER=x,z (Spur G setzt ihn spaeter ueber die Szene), Aufnahme + Tod GENAU wie RE2 (Clips, Frames, SE, ESP, Schadensfenster). SE: RE2_SOUND_LISTE.md (ENEMY23-Bank) + audio_pc.c ENEMSE-Bank je Gegner (Memory: Ein-Latch-Falle). Pins: Zustandsfolge, Clip-Folge, SE-Folge, Kanister-Tod (unit_r41_re2gator_*), bestehende unit_gator_*/unit_alligator_ai gruen oder mit Beleg angepasst.' },

  { id: 'C', thema: 'ventilkette', baum: 'r41_ventilkette',
    punkte: [
      'ROOM 2000: die Leiter zu ROOM 2050 mit herunterlaufendem Wasserstrom blockieren, Message "I cannot climb up the ladder because of the water pressure" — bis die Flut gelaufen ist.',
      'ROOM 2030: die Bruecke (Mittelplattform) ist zunaechst OBEN; gesenkt wird sie ueber das Pult auf der linken Seite (Kamera 2, Nordwand) — "Bruecken-Pult: Irgendein kleines Raetsel waere gut" -> "Ventil 13 als Bruecken-Raetsel: Einverstanden": Pult meldet zu wenig Druck / Zulaufventil 13 geschlossen -> Ventil 13 drehen -> Pult "Lower the bridge?" -> Bruecke faehrt herunter. Nicht die Kurbel.',
      'Ventilraetsel 2030 als Platzhalter ("Ventilraetsel: einverstanden"): nach dem Loesen Warnung, dass das Wasser in einer Minute steigt und man den Raum verlassen soll; 60 s; "Wasser in 2030: Ueber die Ringebene"; verlaesst man den Raum (Ost-Tuer), fuehrt das zu ROOM 20B0 (Auswasch-Szene des Originals); "Zeit abgelaufen und noch im Raum: Tot mit ertrink Effekt."',
      'Nach dem Steigen des Wassers: 20B0 statt 2000 (spaeter wieder frei ohne Wasser), die Leiter ist frei (kein Wasser laeuft mehr herunter).',
      'ROOM 2050: das Schalter-Pult (DANGER High voltage, Skala 0-90, 80 rot) wird kombiniert: das Wasserlevel in 2050 laesst sich nicht steigen lassen (Hebel), bevor das Pult-Raetsel geloest ist (Ziel 80 V wie 11F0 Notstrom).',
      'ROOM 2080: der Rechner, der das Wasser in 2080 senkt, entleert danach auch 2030 (Raum wieder frei ohne Wasser).',
    ],
    hinweise: 'Befund-Grundlage: analysis/befunde_runde40/UEBERGABE.md "Befund STAGE2" (2030: obj 0 Bruecke main00 @0x01B6C Lage (-1075,-1800,-275), nie bewegt; Seitenlifte am Generator (3,0x7E); Cut 14/15 nie geschaltet; RE2-Vorbild ROOM4080 sub05 @0x00FC Fahrt-Signatur 22 01 1c 01 09 0a 08 00 ..., Zustand (4,0x1E); 2050: Hebel sub04 (3,0x68), Kisten (5,0..2)->sub02 (3,0x6D), Pult Cut 15 2D + Cursor obj 8 + sub05-08/sub10 fertig, Ausloeser Platz 8 (4000,-8450,1800,800) Band 3 -> sub09 nur bei (5,7)=1, keine Schalterlogik; 2080: (3,0xF5) Raum-Merkflag sub02 @0x00C94). 20B0/20B1 = 2000-Variante: Tueren 20B0 -> 2010/2030/2050 (engine_tueren.txt), Auswasch-Szene sub02/sub05 (scd-disassembly). Ebenen 2030: Ring Band 3 (y -5400), Mittelsteg Band 1 (y -1800), Buero Band 6, Tuerebene Band 9 (auswahl.json ROOM2030 "ebene"-Belege). Muster im Port: untersuchen_r40.c (Text-Plaetze, Satzform @0x00CF8), nahaufnahmen_r40.c (Kamera-Programme aus Original-Opcodes, Ereignis 31, Set(2,7,1)/Cut_chg/Message_on/Evt_next/Cut_old/Cut_auto), Raum-Umleitung (wie waehlt der Port die RDT-Variante: Raumlader/room fixes, 20B0 statt 2000 ueber das Original-Flag (3,0xEB)? pruefen, ob das Original 2000 -> 20B0 selbst ueber (3,0xEB) schaltet — dann NUR das Flag setzen), Objekt-Fahrt (Work_set/Member_set/Pos_set Muster hebetisch/rolltor_1050.c, karte_fahrstuhl), Timer (Original-Zaehler-Opcodes oder Port-Bildzaehler, 60 s = 1800 Bilder @30 — Form belegen), Wasser steigt = Wasser-Objekt (2080 obj 0 "Wasser sinkt" als Modell-/Mechanik-Vorbild; 2030 hat kein Wasserobjekt -> Port-Prop Wasserflaeche aus 2080/2050-Modell, Ebene ueber Band 3, PORT-WAHL belegt), Tod mit Ertrink-Effekt (RE1.5/RE2 haben kein Ertrinken: PORT-WAHL — Form: Spieler-Tod (hp 0, Todesanimation) + Wasser-ESP/Blau-Ueberblendung, belegt und benannt), 2050-Pult: Vorbilder 2060 Generator (4 Schalter, (5,6..9)=1,0,1,0 -> (3,0x7E)) + 11F0 Notstrom (Ziel 80; notstrom-Modul im Port), Cursor obj 8 + sub05-08/10 wiederverwenden; Texte als Port-Ids laut VERTRAG (Wortlaute PORT-WAHL, englisch wie das Spiel, im Dossier zur Freigabe listen), Bits laut VERTRAG (100-103,106,107), beide Szenarien (2001/2031/2051/2081/20B1). Pins je Glied (Flag-Folge, Objektlage, Timer-Ende -> Tod, Tuer -> 20B0, Leiter-Text vor/nach), Riegel unit_r41_ventilkette_* + ein integration-Lauf ueber den echten Weg (2000 -> 2030 ... -> 20B0).' },

  { id: 'D', thema: 'kleinkram_a', baum: 'r41_kleinkram_a',
    punkte: [
      'Mir fehlt in den Optionen eine Lautstaerke Option bei der ich alles lauter oder leiser machen kann.',
      'Kannst du den Feuerloescher in room 1030 einen leichten Blink Effekt geben, das man sieht, das der aufnehmbar ist? Ich glaube so etwas existiert bereits in Resident Evil 2, uebernehme den.',
      'Der Spieler kann nicht sterben, wenn er noch mal mit Gift von den Spinnen angeschossen wird, aber bereits auf Danger ist...',
    ],
    hinweise: 'Lautstaerke: Options-Screen platform/pc/main.c pc_run_config (CFG_TOP/PICKER/SOUND/EDIT/AI; SOUND-Screen = STEREO/MONO-Boxen CONFIG.TIM uv 0,232 88x18, Memory reai-v2-options-config-screen.md) — RE1.5/RE2-PSX haben keine Lautstaerke -> PORT-WAHL, Form aus dem SOUND-Screen (dritte Zeile "VOLUME" mit Stufen 0..10 oder Balken aus CONFIG.TIM-Kacheln, UP/DOWN waehlt, LEFT/RIGHT aendert — belegen, welche Kacheln/Glyphen es gibt: Game-Font pc_font_code), persistent dort, wo der Port den Sound-Modus speichert (s_sound_mode / Pendant DAT_800b0fce -> Port-Config-Datei?); Wirkung auf ALLES (BGM, SE, Stimmen, FMV): EINE Endstufe in audio_pc.c (Mixer-Ausgang / SS_BASE_MVOL / Master-Q15) statt je Pfad; Pin: Mitschnitt RE15_AUDIO_CAP_SYNC mit 100 % vs 50 % -> RMS halbiert, Stimmen + BGM + SE. Blinken: Feuerloescher-Prop ROOM1030 (feuerloescher_r37.c / Item-Props inhalt_r35.c-Muster) — RE2: gibt es ein Blinken aufnehmbarer Gegenstaende? Pruefen (RE2-Item-Objekte im Raum: Render-Pfad der Item-AOT-Modelle in ghidra_re2_Leon.txt / RE2_Quellcode_V2 — Helligkeits-/Farbpuls, Periode, @0x...); hat RE2 KEIN Blinken (ehrlich belegen), dann PORT-WAHL mit belegter Form (periodisches Aufhellen des Prop-Modells ueber die Vertex-Farbe/Beleuchtung, Periode aus einem RE2-Blink-Gatter wie dem Cursor @0x80028600), als wiederverwendbare API re15_blink_prop(slot/handle, an) (Spur F nutzt sie fuer den Schluessel in 2090) in neuer Datei blink_r41.c; nur der Feuerloescher in 1030 (nicht 1031? Elza: AUFTRAG sagt room 1030 — beide Varianten pruefen, ob der Loescher dort liegt); Pin: Framedump-Serie zeigt Helligkeitsmodulation. Spinnengift: spinnen_r38.c (Spuck-Treffer), zustand_r39.c (Gift-Takt @0x80020138-40), re15_damage.c — messen: Leon auf Danger, Spinnen-Spucke trifft -> hp? (Klemme auf 1? Schadensart ohne hp-Abzug?); Original RE2 (Spinne = RE2 EM-Typ der Riesenspinne; Spuck-Treffer: Schaden + Gift @0x... in ghidra_re2_Leon.txt / RE2_Quellcode_V2) — Nutzer-Wortlaut: erneuter Gift-Treffer bei Danger = Tod; belegen und angleichen; Pin (hp Danger + Treffer -> Tod, Todesanimation).' },

  { id: 'E', thema: 'kleinkram_b', baum: 'r41_kleinkram_b',
    punkte: [
      'Leon hat eine komische herumfliegende Animation im ROOM 2020 am Anfang wenn er die Kurbel nutzt.',
      'Die Map B1 ist jetzt beinahe perfekt, aber bevor man in den vorderen Teil der Waffen Armory - room 1090 - geht - also die vordere Tuer - macht der Spieler auf der Map einen Sprung zum hinteren Teil, obwohl man garnicht da ist.',
      'Die Tuer der Werkstatt in ROOM 11B1 bei Elza, wenn sie ueber ROOM 2000 rein kommt und die Cutscene abspielt wo sie Elza sucht, ist immer noch offen. Ich wollte das sie dauerhaft geschlossen bleibt in Elzas Szenario.',
    ],
    hinweise: 'Kurbel 2020: ROOM2020/2021 Kurbel-AOT + Sub (scd-disassembly: Plc_motion/Em_set_motion/Plc_dest-Folge, Spieler-Clip aus welcher Bank — Zusatzclips des Spielers fuer Kurbel/Ventil = eigene PLD-Clip-Bank? wie laedt das Original sie (Raum-Overlay, FUN_... Clip-Bank-Wechsel), hat der Port diese Bank (RE15_ANIM_TRACE: Clip-Nr./Bank beim Kurbel-Start, Framedump-Serie zeigt das "Fliegen" = falscher Clip, falscher Anker, falsche Wurzelbewegung?); Original per DuckStation-Savestate der Kurbel-Szene (re15-room-capture, re15-parity-verify) messen; beide Szenarien. Karte B1: karte_r39.c / re15_map_zones.c / re15_map_zeilen.h (Runde 36 M: ROOM1090 = 2 getrennte Bereiche; Runde 39 G: B1 neu) — messen: Spieler vor der VORDERTUER von 1090 (Weg ueber den Vorraum/Gang; welcher Raum ist das, RE15_MAP_SHOT_PAGE + Positionen je Bild), Marker springt in den hinteren 1090-Teil -> Zonen-Zuordnung (Zone/Zeile des Vorraums falsch an den hinteren Bereich gebunden? Tuermarke?), Ursache belegen, Pin. 11B1: Elza kommt aus ROOM2000 nach 11B1 (Szene "sucht" — Nutzer-Wortlaut sinngemaess: die Szene, in der nach Elza gesucht wird / John sie sucht; Original-Sub in ROOM11B1 beim Eintritt aus 2000 disassemblieren) — die Werkstatt-Tuer (11B1 -> Werkstatt 11A1? Zuordnung gen/tuer_zuordnung.inc, engine_tueren.txt) steht danach offen; Nutzer: in Elzas Szenario DAUERHAFT geschlossen (Runde 36 J schliesst sie bei Leon ueber Marvin ab, Bit RE15_R36Q_TUER?/84 + Text "The door is locked." msg 12 @0x179F ROOM11B1; Runde 39 Nachtrag 16: Elzas Parkplatz-Tuer 11B1 Slot 1 -> 11C1 "immer zu" — nicht verwechseln: hier die WERKSTATT-Tuer). Messen ueber den echten Weg mit Elza (RE15_PSELECT_AUTO) 2000 -> 11B1 -> Szene -> Tuer anklicken; Form: Tuer-AOT auf Text umgetypt (Muster tuer1120_1130.c / Runde 36 J) ab Spielstart in Elzas Szenario; Pin.' },

  { id: 'M', thema: 'masken36', baum: 'r41_masken36',
    punkte: [
      'Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher aufweisen. -> "danach kannst du deine Masken schliessen in der naechsten Version."',
      'Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden! (sind in master 5924939d — hier: Sichtpruefung im Spiel, Abweichungen melden)',
    ],
    hinweise: 'Messungen in analysis/befunde_runde40/UEBERGABE.md (2026-10-23): STAGE3-6-Masken haben keine eigenen _PRIcc.TIM, die Rechtecke des Generators (Commit b8db7b2b, analysis/esp_masken_2026-09-03/gen_masken.py) lesen den Original-SLD-Atlas, CLUT[0]=0x0000, 17,3 % der Rechteck-Texel = Index 0 (3050 C1: 51 %); Helligkeit trennt NICHT (Index-0-Stellen < 24: 50 %, deckende 19 %), eingeschlossene Loecher 18423 ohne Groessenluecke. Werkzeuge re15_port/tools/maske/ (raum.py, bau_p2.py, atlas.py build_rechtecke = eigener Atlas ohne Index 0, maskenbild.py, original.py atlas(), abnahme.py; Uebersichtsbilder pri/STAGE3-6/<raum>/<cc>_port.png = heutige Masken). Ansatz: je Cut eigener Atlas aus dem Hintergrundbild (wie bau_p2: Texel = Hintergrund an der Zielstelle), Silhouette = heutige Deckung MIT geschlossenen Loechern, aber echte Zwischenraeume (Gelaender, Gitter) offen lassen — Klassifikation belegen (z.B. Loch beruehrt den Silhouettenrand/ist laenglich entlang der Kunstkante = echter Zwischenraum; Pixelrauschen innen = schliessen) und JE CUT Vorher/Nachher-Bild im Dossier-Ordner M_bilder/ (Soll/Deckung/Loch wie abnahme.pruefbild); Tiefen/Rechtecke unveraendert; Ausgabe shared_assets/PSX/MASKS (R15M-Container + _PRIcc.TIM; Ausnahme im VERTRAG: M darf dort schreiben); bestehende Riegel unit_pri_*/unit_r37_masken36_*/integration_r37_masken36_* anpassen nur mit Beleg. Sichtpruefung der 53 neuen Masken (ROOM11A0, 2010, 2020, 2030, 2040, 2050, 2080, 20A0; Pruefbilder build/p2/masks_r40_pruefbilder im Hauptbaum NUR lesen): je Cut Framedump (RE15_WINDOW_SCALE=1, RE15_DEBUG_JUMP + RE15_PLAYER_POS vor/hinter dem Objekt, RE15_FORCE_CUT) — verdeckt die Maske die Figur dahinter und nicht davor? Abweichungen mit Bild melden (Dossier), Nutzer-PNGs unter pri/ NIE aendern; Masken-Fixes nur in auswahl.json + MASKS mit Beleg.' },

  { id: 'F', thema: 'gatorszene', baum: 'r41_gatorszene',
    punkte: [
      'Ich moechte eine neue Cutscene: Wenn man Room 2090 betritt, soll der Charakter einen Zombie am Boden liegen sehen mit einen blinkenden Schluessel - Blinkend so - wie bei der anderen Implementierung weiter oben der Feuerloescher. Es schwimmt bereits ein Zombie im Wasser, nimm vielleicht diesen. Der Aligator soll dann aus dem Wasser springen und den Zombie fressen inklusive dem Schluessel. Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe seine Leiche sein und blinken.',
      'Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr.',
    ],
    hinweise: 'Baut auf dem integrierten Stand der Welle 1 auf (Spur B gator_re2_r41.c: RE2-Fress-/Finisher-Clips und Sprung aus dem Wasser; Spur D blink_r41.c: re15_blink_prop). Zombie: ROOM2090/2091 Spawn-Zensus (Sce_em_set; der Wasser-Zombie: wasserzombie_r38.c) — den liegenden Zombie an eine vom Eingang sichtbare Stelle (Kamera beim Eintritt: Tuer-Spawns von 2080 (-8200,-25550) / von 20A0 (6300,-16250), RVD-Zone -> Cut; Lage per Rueckprojektion aus dem Cut-Bild), Schluessel-Prop daneben (blinkend); Szene (Port-Programm wie nahaufnahmen_r40.c/cut_10f0.c: Set(2,7,1), Kamera, Alligator springt aus dem Wasser (B-Clip), frisst Zombie (FRESSEN auf einen Zombie-Aktor statt Leon — Anker wie gator-finisher-sounds.md §2) samt Schluessel (Prop verschwindet), Kamera zurueck, Set(2,7,0)), nur beim ersten Betreten (Bit 110), beide Szenarien; nach dem Tod (GBP_DEAD/Leiche) Schluessel-Item-AOT neben der Leiche (Bit 111 liegt, 112 genommen), blinkend bis zur Aufnahme; Schluessel-Item: Zensus der STAGE2-Schluesseltueren (welche Tuer braucht einen Schluessel, der nirgends liegt? rdt-analysis Door_aot_set Key-Feld ueber STAGE2) -> passenden RE1.5-Schluessel (Runde 38 F Muster, nie platzierte Ids) waehlen, Modell aus extracted_re2_items (Schluessel), im Dossier VORSCHLAG + Rueckfrage an den Nutzer, welche Tuer er oeffnet. Spinnen: Spawn-Override fuer Typ 0x25 in 2090/2091 (Muster zombie_abstand_r35.c, kein RDT-Patch); Riegel unit_re2_spider_water_2090 / unit_sca_wall_hit (laedt 2090-Spinnen!) mit Beleg anpassen; der TEMPORAERE Spinnen-Freeze in enemy_ai_common.c (Suchanker "TEMPORAER") darf dabei fallen. Nachrichten 2090/2091 Ids 42-46; Pins: Szene-Folge (Log), Schluessel-Item nach dem Tod, keine Spinnen.' },

  { id: 'G', thema: 'kanister2080', baum: 'r41_kanister2080',
    punkte: [
      'Nach dem Start der Cutscene, wo der Aligator den Zombie mit dem Schluessel frisst, soll oben im ROOM 2080 im Hauptbereich Erst ein Sound kommen der schliessendem Metall Tuer. Dann sollen oben im Hauptbereich 8 Zombies stehen die gespawned sind.',
      'Ich moechte das im ROOM 2080 am Rand der Reling der Gaskanister steht, der in Resident Evil 2 dafuer sorgt, das man den Aligator besiegt. Siehe Gas_Can.bmp.',
      'Hat man den Aligator oft genug angeschossen, soll eine Cutscene Triggern. Es gibt erneut die Geraeusche eine schliessenden Tuer. Marvin (Bei Leons Szenario), John (Bei Elzas Szenario) ist im ROOM 2080. Toetet alle 8 Zombies mit Kopfschuss - ein Schuss - Kopf platzt - tot. Rennt zu den Gas Kanister nimmt ihn auf in beide Haende und sagt so etwas wie - "Hey, Leon, try with this!" Dann schmeisst er den Gaskanister die Reling runter. Der liegt dann in 2090 an der Stelle im Wasser. Wenn der Aligator dran vorbei kommt, soll er ihn aufnehmen, GENAUSO wie im original Resident Evil 2. Sobald er den Kanister im Maul hat, soll man auf ihn schiessen koennen, und die Sterbe Animation mit geplatzen Kiefer etc. vom Aligator soll genauso kommen wie im Original.',
    ],
    hinweise: 'Baut auf Welle 1 (B: Kanister-Aufnahme + Tod ueber RE15_GATOR_KANISTER-Pfad; Kanister-Modell in shared_assets/RE2/) und Spur F (Bit 110 = 2090-Szene gesehen) auf. (a) 2080 nach Bit 110, beim Betreten: Metalltuer-Schliess-SE (RE2_SOUND_LISTE.md: Door sounds / metal; Schnitt wie Runde 38/39 Toene nach shared_assets/RE2/) + 8 Zombies im oberen Hauptbereich (Band 8, y -14400; Positionen aus Kollision/Kameras, Form Sce_em_set-Record wie Port-Spawns (Muster elzazombies_r39.c / affen), Kill-Bits aus freier Zone-7-/Bank-9-Zuteilung 114-119), beide Szenarien (2081). (b) Kanister-Prop an der Reling: Gas_Can.bmp (Repo-Wurzel, 320x240) per Bildvergleich gegen die 2080-Kameras (load_bg / bg_alle-Abzuege) dem Cut zuordnen, rote Marke -> Rueckprojektion auf das Reling-Band (Memory reai-v2-modellmass-aus-kamera), Prop stehend; (c) Szene nach "oft genug angeschossen": Schwelle als NUTZER-VORGABE belegt (Vorschlag: HP-Anteil, bei dem RE2 den Kanister ermoeglicht / Flinch-Schwelle der Spur B; im Dossier nennen), Spieler in 2090 eingefroren (Set(2,7,1)), Kamera auf 2080-Cuts (Kamerasaetze des NACHBARRAUMS in einem anderen Raum zeigen: Raum-Doppel-Laden? Port-Muster pruefen — alternativ: Szene laeuft beim naechsten Betreten von 2080 / oder 2090-Kamera auf die Reling von unten; Form entscheiden und belegen, im Dossier Alternativen), Tuer-SE, Marvin (Leon) / John (Elza) als NPC in 2080: 8 Kopfschuesse (NPC-Schuss-Clip + Zombie-Kopfschuss-Tod: re15_damage / re2 zombie head-burst-Pfad, ein Treffer = Kopf platzt, SE), Lauf zum Kanister (Plc_dest), Aufnahme in beide Haende (NPC-Trage-Clip: Zensus Marvin-/John-Banken + RE2-NPC-Clips; sonst passendster Clip mit Beleg), Zeile Id 42 "Hey, Leon, try with this!" / 2081 "Hey, Elza, try with this!" (Stimmen synchro/STAGE2/room2080/main42.wav + room2081/main42.wav — fuer den Nutzer nennen), Wurf (Objektbahn ueber die Reling, Landepunkt in 2090 = Position unter der Reling), danach Kanister in 2090 ueber den B-Pfad setzen (persistent Bit), Alligator nimmt ihn auf -> Schuss -> RE2-Tod (B). NUTZER-VORGABE Nachtrag 5 (AUFTRAG.md): Kamera und Ablauf des Wurfs GENAU wie die RE2-Szene, in der Ada den Raketenwerfer von oben zu Leon hinunterwirft (Endkampf Tyrant; RE2-Raum per Zensus: Item_aot_set Raketenwerfer / Ada-NPC + Wurf-Clip im SCD, info/re2leon/PL0/RDT; Schnitte, Zeitpunkte, Werfer-Clip, Flugbahn mit Offsets belegen, auf 2080/2090-Kameras abbilden). Nachtrag 4: RE2-Kameraschnitte des Alligator-Kampfes (Spur B) fuer Aufnahme/Explosion mitnutzen. Pins je Teilschritt (Log-Folge, Spawnzahl 8, Kopfschuss-Toede, Prop-Lage, Kanister in 2090, Schnittfolge des Wurfs).' },

  { id: 'H', thema: 'kollision', baum: 'r41_kollision',
    punkte: [
      'Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest.',
      'In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch',
    ],
    hinweise: 'NPC<->NPC: Original-Body-Push @0x8002b1f0 (Port re15_enemy_body_push_tail / re15_body_push_player in game_step_common.c; Memory gator-bosskampf v0.7.40 Y-Band-Falle) — welche Aktor-Paare pusht das Original (NPC-Typen 0x40..0x4b untereinander? Flags +0x188&1, Dispatch-Reihenfolge, Push nur Spieler<->Gegner?) und RE2 (Beta->Retail: RE2 NPC-Wurzel, FUN_8002xxxx Body-Push mit NPC-Maske in ghidra_re2_Leon.txt) — messen in den Szenen mit Ada+Marvin (11C0 Cut 13 Dialog, 10F0, 11B0; RE15_STATE_LOG Positionen, Framedump); Form belegen, angleichen (nur Haken in game_step_common.c, Logik in kollision_r41.c), Pin: zwei NPCs auf Kollisionskurs bleiben getrennt (Abstand >= Radien-Summe). Kisten 2050: kiste2feld_r37.c / zellschub_r36.c / aot_common.c re15_prop_push_tick (Runde 36 R: FUN_8002bd44 Ruhe-Zweig, FUN_8003b558 Zellen-Test) — Objekt<->Objekt-Test: wie blockt das Original eine Kiste an einer anderen (Objekt-Kollisionsliste FUN_8003b558 ueber alle Objekte? oder RE2-Kisten-Puzzle (RE2 ROOM?? Kisten im Kanal) @0x...), messen (Kiste gegen Kiste schieben, RE15_PUSH-Log), Pin: SCHUB-GESPERRT an der zweiten Kiste; das Kisten-Raetsel (5,0..2) bleibt loesbar (integration_r37_kiste* gruen).' },

  { id: 'I', thema: 'zombiefresser', baum: 'r41_zombiefresser',
    punkte: [
      'In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und fressen, anstatt aufzustehen wenn man ihnen naeher kommt. gleiches trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010 etc. es ist durchgehend falsch.',
    ],
    hinweise: 'Zensus aller Fresser-Spawns (Sce_em_set-Variante/Grid der "eating"-Zombies) in STAGE2/3 (2000/2001, 3000, 3010, ...; rdt-analysis); RE2-Hirn enemy_ai_re2_zombie.c (Fresser-Bit +0x1D3 0x80, Weck-Distanz 0xFA0 @0x..., Runde 36 F zombieki_r36.c Wecker, Runde 37 L re2zombie) — messen: Leon/Elza 500-1500 vor dem fressenden Zombie, 600 Bilder, RE15_ENEMY_DBG/RE15_STATE_LOG/RE15_ANIM_TRACE: Zustand/Sub/LOS/Fresser-Bit/Clip; warum wachen sie in STAGE2/3 nicht (Stage-/Raum-Gate im Port, Spawn-Overrides nur STAGE1 (zombie_abstand_r35.c), grid-Byte der STAGE2/3-Records anders als STAGE1, fehlender Aufsteh-Clip in der STAGE2/3-EMD-Bank (Clip-Zensus EMD/EM010/EM016 je Stage), Fresser-Sperre haengt?); Original RE2: Fresser-Weckbedingungen (Distanz/Sicht/Timer @0x... ghidra_re2_Leon.txt; DuckStation-GDB an RE2 nur mit Image); Original RE1.5 (STAGE2.BIN Zombie-Root fuer die Fresser-Variante) als Gegenprobe; beheben (Logik in zombiefresser_r41.c, Haken in enemy_ai_re2_zombie.c), Pin je Raum (2000, 2001, 3000, 3010): Zombie steht binnen N Bildern auf und greift an.' },

  { id: 'J', thema: 'stage3szenen_a', baum: 'r41_stage3szenen_a',
    punkte: [
      'Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist falsch in ROOM 3060.',
      'In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von dieser Cutscene hat Elza auch erneut eine falsche Animation.',
    ],
    hinweise: 'Original-Subs disassemblieren (scd-disassembly ROOM3060 / ROOM3061: Plc_motion/Em_set_motion/Plc_dest/Plc_ret-Folge je Zeile, Clip-Nummern + Banken), Port: RE15_ANIM_TRACE + RE15_SCD_TRACE + Framedump-Serie (RE15_WINDOW_SCALE=1) ueber die ganze Szene (Marvin-Abschnitt: wie kommt man hin — Spielstand/CONTINUE oder RE15_DEBUG_JUMP + Flags; Memory reai-v2-cutscene-npc-system.md, reai-v2-npc-crossfade-decay.md, reai-v2-plc-neck-verify.md), Original per DuckStation-Savestate der Szene (stage_saves/, re15-room-capture, re15-parity-verify) Bild fuer Bild vergleichen; typische Ursachen aus frueheren Runden: Clip-Bank-Zuordnung je Stage (anim_select_common.c), fehlender Plc_ret (Runde 39 Nachtrag ENTRIEGELN39), Sherry-Modell/Clips (CDEMD1-Sektor, test_r37_elzaems elza3091 Typ 0x42; Spur K klaert das Modell — hier NUR die Schacht-Bewegung/Clip-Folge, Ueberschneidung im Dossier nennen), Elzas Ende-Clip; Pins: Clip-Folge je Akteur (Log) = Original-Sub.' },

  { id: 'K', thema: 'stage3szenen_b', baum: 'r41_stage3szenen_b',
    punkte: [
      'Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist voellig falsch und kaputt. Ausserdem sollte sie keine Waffe haben.',
      'Die Cutscene im ROOM 3091 hat STAENDIG falsche Animationen....',
    ],
    hinweise: 'Sherry: welches EMD/welche Bank laedt der Port fuer den Sherry-Typ in 30C1/30D1/.../3091 (Sce_em_set-Zensus Typ in ROOM30C1..3091: enemy_ai_common.c:9640 nennt 0x45 Sherry, test_r37_elzaems elza3091 nennt Typ 0x42 aus CDEMD1 Sektor 1505 — klaeren), pc_enemy_load_ex / roy_pc.c (CDEMD1.EMS-Weg) — Original-Modell (RE1.5 CD-Baum EMD/, CDEMD1.EMS-TOC @0x80073178, welcher Sektor fuer Sherry, Original-Texturseite), Framedump Port vs Original (DuckStation-Savestate der Szene) — "voellig falsch und kaputt" = falsches Modell? falsche Textur/CLUT? falsche Bone-Zuordnung (Skelett-Zahl)? Waffe = PLW-Anhang/Waffen-Mesh, weil Sherry als Spielertyp/mit Hand-Mesh geladen wird? -> ohne Waffe (Beleg: Original-Sherry hat kein Waffen-Mesh); 3091-Szene (Zeilen 9-30 John/Elza/Sherry nach der Rettung, Nutzer-Aufnahmen synchro/STAGE3/room3091/main09-30 liegen und bleiben): Original-Subs (sub02 @0x01596 ff., Set(9,21,1) sub06 @0x016C2) Plc_motion/Em_set_motion je Zeile vs RE15_ANIM_TRACE, Bild-fuer-Bild gegen den DuckStation-Savestate; Ursachen belegen (Clip-Bank, Modell, Plc_ret, Neck), beheben, Pins: Clip-Folge je Akteur = Original, Sherry-Modell-Hash/Mesh-Zahl, kein Waffen-Mesh.' },

  { id: 'L', thema: 'spinnenanim', baum: 'r41_spinnenanim',
    punkte: [
      'Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch nicht ganz.',
    ],
    hinweise: 'Adult-Spinne 0x25 (enemy_ai_re2_spider.c, spinnen_r38.c Schachtspinnen: Gitter-Tore grid&0x60 @0x8003710c / Band-Tor @0x800120ec; Memory reai-v2-stage2-enemies.md: 9-Phasen-Ambush state 4 @0x80114ec8, Ceiling-Drop grid 2-0x1f y -10800, cling-pose Clip 0x10, pos_advance): Original-Choreografie MESSEN (DuckStation-Savestate ROOM2030/2010/3020 mit Gitter-Spinne, re15-room-capture; Bild-fuer-Bild: Clip-Nr., Wurzel-Lage, Drehung, Hoehe je Bild) vs Port (RE15_ANIM_TRACE, RE15_ENEMY_DBG, Framedump-Serie), Abweichung je Phase belegen (Clip, Takt, Wurzelbahn, Yaw/Pitch beim Wandklettern — pitch-Kanal/Rotation um X beim Herunterklettern?), angleichen (Logik in spinnenanim_r41.c, Haken), Pins: Clip-/Positions-Folge = Original (Toleranz belegt).' },
  { id: 'N', thema: 'nachtrag30', baum: 'r41_nachtrag30',
    punkte: [
      "Wasser 2010 (Nachtrag 30e, Wortlaut): \"und eine Sache fehlt mir noch - in room 2010 soll - bis zum ablassen der Wassers, auch ein Wasser Effekt die Leiter runter kommen und ueber den Weg bis zum Wasser Pool in der Mitte laufen.\" Nutzer-Wahl: \"Bis zur Flut in 2030\" (wie der Wasserstrom an der Leiter in 2000).",
      "Schrank 2010 (Nachtrag 30, Wortlaut): \"In ROOM 2010, beim Anklicken des Schranks, mache das er aufklappt und dann so aussieht wie in 2060 der Fuse Kasten - siehe fuse_box.bmp. Ab da soll auch der Mechanismus gleich - das Einsetzen der Sicherung etc.\"",
      "Panel 2030 (Nachtrag 30 + 30b, Wortlaut): \"Dann - in ROOM 2030 am Kasten, damit die Platform runter faehrt, erstelle ein Raetsel, das so aussieht wie das Panel in Nahansicht, anstatt das komische - dreh an Valve 13.\" + \"Zum 2030er Raetsel ist noch wichtig das die Pressure irgendwie 80 betragen muss, wenn du das schon so hin schreibst.\" Nutzer-Wahl: \"C entwirft, ich gebe frei\" -> 2-3 Entwuerfe mit Bildern, Ziel Druck 80; GEBAUT WIRD ERST NACH DER FREIGABE (eigener Nachtrag in AUFTRAG.md). Solange keine Freigabe dort steht, bewertet die Abnahme die Entwuerfe (Bilder, Stil des Spiels, Druck 80, Bedienung), nicht ein gebautes Raetsel.",
      "Notiztext (Nachtrag 30, Wortlaut): \"Dann - beim Text soll nur drin stehen das gruen und rot verwechselt wurde, nicht black, das soll raus. Bei der verwechselten Nummer soll drin stehen das sie ausserdem die Nummern verwechselt haben von \\\"Schwarz\\\" und \\\"Gruen\\\". Hier dienen die Anfuehrungszeichen als Ironie fuer die falschen Farben auf den Poster. Natuerlich auf englisch.\" + Nachtrag 30d (ersetzt 30b/30c): Plakat NEU 1. GREEN 01 - 2. RED 02 - 3. BLACK 09 - 4. RED 08 (Regeln der Notiz: Gruen<->Rot vertauscht, Nummern der Zeilen GREEN und BLACK vertauscht) -> Loesung 09 -> 02 -> 01 -> 08; Notiz-Satz siehe AUFTRAG.md Nachtrag 30b.",
      "RE2-Ton der Plattform (Nachtrag 30, Wortlaut): \"Und wenn die Plattform runter faehrt, fehlt mir noch der Original Sound von Resident Evil 2.\"",
      "RE2-Alarmton (Nachtrag 30, Wortlaut): \"Und waehrend des Alerts und der Alert message fehlt mir noch zusaetzlich durchweg der Alert sound aus Resident Evil 2.\"",
      "Lautstaerke (Nachtrag 30 + 30b, Wortlaut): \"Und bei der reduzierten Volume sollen ALLE Volumes reduziert werden, nicht nur ein paar.\" + \"Die Tuersequenzen und Sounds der Reinsprung SOunds ins wasser etc, scheint davon nicht betroffen.\"",
    ],
    hinweise: "Basis: Spur C (ventilkette) und D (Lautstaerke) sind in master integriert; ihre Dossiers C_ventilkette.md (§0f Sicherungskasten 2010 B5-1, §7) und D_kleinkram_a.md (§1 Endstufe) zuerst lesen. Module: engine/src/ventilkette_r41.c + include/re15_ventilkette_r41.h, gen/ventilkette_r41_texte.inc + gen/ventilkette_r41_programme.inc (Generator tools/gen_r41_ventilkette.py, tools/r41_ventilkette/*.py), lautstaerke_r41.c, audio_pc.c (Endstufe am Ende von audio_callback, C-Alarm re15_audio_r41_alarm). (1) Schrank 2010: Platz 8 (x 4900..5900, z -3470..-2070, Cut 3), Original-Kasten ROOM2060 Slot 9 @0x010A6 + sub19 @0x0169E ((3,144), msg 3 \"One of the fuses is missing...\"); fuse_box.bmp (im Baum: analysis/befunde_runde41/N_belege/nutzer/fuse_box.bmp, 320x240, bytegleich zur Repo-Wurzel des Hauptbaums) per Bildvergleich dem ROOM2060-Cut zuordnen; \"aufklappen\" in einer belegten Form (RE1.5/RE2-Tuer-/Klappenmodelle, Memory reai-v2-tuermodelle-polygone.md / reai-v2-tor-1170-tuermodell.md, oder Nahaufnahme-Standbild wie engine/src/nahaufnahmen_r40.c) - im Dossier begruenden; danach der 2060-Ablauf (Fragen, Sicherung rein/raus) mit Bit (9,125). (2) Panel 2030: schaltschrank.png (im Baum: analysis/befunde_runde41/N_belege/nutzer/schaltschrank.png, links mit roter Marke); Nahansicht aus dem 2030-Hintergrund; Vorbilder: RE1.5 11F0 Notstrom-Pult (80 V), Port 2050 Pult/Hebel (Nachtrag 18: Lampen unten, Hebel gehen beim Umlegen runter); Ziel Druck 80 (Port-Text 2030 Id 43); Entwuerfe als Bilder unter analysis/befunde_runde41/N_belege/ und in fragen_an_nutzer; bis zur Freigabe bleibt Ventil 13 (Bit 101) wie gebaut. (3) Notiztext: Dokument 6 = Item 0x4E (re15_files.c, Seiten shared_assets/RE2/FILES/FILE31_p0x_page.TIM aus dem Generator); Loesung RE15_R41C_LOESUNG {5,3,1,0} -> {5,1,0,4} (= 09,02,01,08 ueber RE15_R41C_VENTIL_NUMMERN {1,2,3,7,8,9}, Nachtrag 30d) + Plakat-Bild neu (tools/r41_ventilkette/plakat.py); Riegel unit_r41_ventilkette, integration_r41_ventilkette_* nachziehen. (4) RE2-Plattformton: RE2_SOUND_LISTE.md (Nutzer-SAP-Liste, gleiche Nummern wie info/re2leon) + SCD-Zensus der RE2-Raeume mit Plattform/Hebebuehne (z. B. ROOM6xx \"Elevator/Switch\"), Byte-Beleg; Port: Brueckenfahrt 2030 (Lift-Form sub03, 30 x 120). (5) RE2-Alarmton: zusaetzlich zu SUB_25 der RE2-Alarm-SE der Selbstzerstoerung (RE2 ROOM6040/Zug; Sound-Liste), durchgehend vom Warntext 46 bis Flut/Verlassen, Muster C-E4/C-E5. (6) Lautstaerke: Mitschnitt RE15_AUDIO_CAP bei volume=100 und 50 (RE15_OPTIONS_FILE) je Tonart: Tuersequenz, Wassersprung/Spritzer, SE, Stimmen, BGM, FMV, Ambience, Alarm; Ursache belegen (eigener Pfad, Ladezeitpunkt re15_lautstaerke_r41_laden, Saettigung ...), beheben, Riegel je Tonart. Neue RE2-Dateien nach shared_assets/RE2/ (Paket-Gate nennen)." },
]

const REGELN = (s) => `
## Arbeitsbaum und Grenzen
- Dein Baum: ${ROOT}/.claude/worktrees/${s.baum} (Zweig r41/${s.thema}, Basis master ${BASIS}). Zuerst: git -C "${ROOT}/.claude/worktrees/${s.baum}" status --porcelain (leer) und git log --oneline -1 (= ${BASIS} oder ein Merge von master in den Zweig; Fortsetzungs-Baeume tragen zusaetzlich die Vorgaenger-Commits). Der Baum ist ein sparse-Checkout (re15_port, analysis, Quellcode-Dumps, info/Re1.5, info/re2leon, pri, synchro, stage_saves, tools, release, memory) — fehlt ein Pfad, aus dem Hauptbaum ${ROOT}/ NUR LESEN.
- ALLE Lese-/Schreibzugriffe auf Projektdateien und ALLE git-Befehle NUR in diesem Baum (absolute Pfade mit dem Praefix ${ROOT}/.claude/worktrees/${s.baum}/ bzw. git -C "<baum>"; in Bash jeden Befehl mit cd "<baum>" beginnen). NIE Dateien unter ${ROOT}/ ausserhalb deines Baums aendern (der Hauptbaum ist der Spielbaum des Nutzers). Aus dem Hauptbaum nur LESEN (info/Resident_Evil_und_Playstation_Information/, ghidra*.txt, build/p2/, extracted_re2_*).
- Pflichtlektuere ZUERST, vollstaendig: <baum>/analysis/befunde_runde41/AUFTRAG.md und VERTRAG.md (Zuteilung VERBINDLICH; nimm nur, was Spur ${s.id} zugeteilt ist; Bits/Nachrichten-Ids nur die zugeteilten). Vorrunden: <baum>/analysis/befunde_runde40/UEBERGABE.md (Befund STAGE2), befunde_runde39/39/38-Dossiers der Module, die du anfasst.
- Dossier <baum>/analysis/befunde_runde41/${s.id}_${s.thema}.md als ERSTES anlegen und FORTLAUFEND schreiben; nach JEDEM abgeschlossenen Schritt committen (wip(r41-${s.thema}): ... ausdruecklich erwuenscht, auch VOR Bau/Tests). Das Sitzungslimit beendet alle Agenten gleichzeitig — was nicht im Commit steht, ist verloren. Nur eigene Pfade adden (NIE -A), Rohbytes/Backticks per git commit -F <datei>; jede Commit-Message endet mit der Zeile "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>".
- Nie SendMessage an andere Agenten oder an dich selbst (startet eine zweite Instanz im selben Baum); einen vermuteten parallelen Schreiber nur im Dossier vermerken.

## RE-Disziplin (CLAUDE.md STOP-GATE, Kurzfassung)
- Je Punkt: MESSEN/REPRODUZIEREN am gebauten Stand -> ORIGINAL disassemblieren (RE1.5; wo RE1.5 unfertig ist, RE2 Retail = Ziel, Beleg aus RE2; Sound ist RE2 — <baum>/RE2_SOUND_LISTE.md zuerst) -> Adresse/Bytes ins Dossier -> DANN Code -> VERIFIZIEREN durch erneutes Messen. Jede Konstante @0x... (oder Byte-Offset) im Code UND in der Commit-Message; Nutzer-Vorgaben als NUTZER-VORGABE / PORT-WAHL, ihre FORM belegt (Vorbild im Original/RE2 nennen). Kein "deferred/tunable/interim/Env-Schalter" als Abschluss. "Sieht richtig aus" ist kein Beleg; Nutzer-Beobachtung schlaegt Code-Hypothese. Nicht raten, nicht aufgeben: Sackgasse = Weg wechseln (Xrefs, Caller/Callee, RE2, Savestate-RAM, GDB, Laufzeit-Log). Was nach Messung nicht belegbar ist, als OFFEN mit Adresse dokumentieren, nie mit einer Zahl nach Gefuehl fuellen.
- Quellen im Baum: RE_15_Quellcode_Overlays/STAGE{1..6}_full/ (0x8010xxxx), RE_15_Quellcode_V2/, ghidra1_V2.txt (Hauptbaum), RE15_FUN_CATALOG.md, RE15_KNOWLEDGE.md, RE15_SCD_OPCODES_REFERENCE.md, RE2_SOUND_LISTE.md; RE2: info/re2leon/ (PL0/RDT, COMMON/BIN mit EM23_OVL_0000.BIN), ghidra_re2_Leon.txt (Hauptbaum), RE2_Quellcode_V2/, RE2_Quellcode_Overlays/. Werkzeuge .claude/skills/re15-psx-disasm/scripts/re15_disasm.py / re2_disasm.py (STAGE*.BIN ohne Header? Memory reai-v2-re-pitfalls: pruefen), re15_port/tools/rdt_msgdump.py, tools/maske/; Skills .claude/skills/<name>/SKILL.md (rdt-analysis, scd-disassembly, ghidra-mapping, re15-psx-disasm, re15-room-probe, re15-room-capture, re15-port-visual-verify, re15-enemy-ai-re, re15-weapon-render, re15-parity-verify, re15-pc-render-order, re15-savestate-ghidra, re15-pcsx-watchpoint) ZUERST lesen. Memory-Notizen (reai-v2-...) unter C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/<name>.md (nur lesen; MEMORY.md ist der Index).

## Bauen, Messen, Testen
- NUR so bauen: cd "<baum>" && bash re15_port/tools/local_build.sh configure (einmal und nach jeder NEUEN .c-Datei), danach ... build. NIE cmake --build aus Git-Bash (PATH-Shadowing, stiller gcc-Tod). exe <baum>/re15_port/build/platform/pc/re15_pc.exe; befund.log/debug.log daneben. ⛔ KEINE volle Suite (local_build.sh test/all) im Spurbaum. Stattdessen GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = eigene Tests (r41_${s.thema}) + die Testgruppen JEDES Moduls/Raums, das dein Diff beruehrt (git -C "<baum>" diff master --name-only; je Datei: git -C "<baum>" grep -l <datei-stamm> re15_port/tests; ctest -N | grep -i <modul/raum>). Die ctest-Summenzeile woertlich mit dem -R-Ausdruck ins Dossier. Fenster-Haken flattern unter Last: einzeln 2-3x nachfahren, nicht als Regression werten. NIE taskkill /IM re15_pc.exe (nur eigene PID); Messkopie der exe unter eigenem Namen neben der exe (Muster tests/integration/test_r34n_b_cursor.cmake). Nie zwei Builds gleichzeitig im eigenen build/. Bau und exe-Messlauf NIE in EINEM Bash-Aufruf; exe-Laeufe immer mit timeout und unter 8 min (sonst run_in_background + Waechter).
- Harness: getenv("RE15_...") in platform/pc/main.c + engine/src (RE15_DEBUG_JUMP, RE15_FORCE_EVENT, RE15_FORCE_CUT, RE15_PLAYER_POS, RE15_EQUIP, RE15_GIVE, RE15_PRESS, RE15_PAD_AT, RE15_INPUT_SCRIPT, RE15_SET_FLAG(_AT), RE15_FIRE_AOT, RE15_STATE_LOG, RE15_EVT_TRACE, RE15_SCD_TRACE, RE15_MSG_LOG, RE15_ENEMY_DBG, RE15_ANIM_TRACE, RE15_NECK_LOG, RE15_FX_LOG, RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC, RE15_FRAMEDUMP, RE15_EXIT_AT, RE15_KILL_AT, RE15_CONTINUE_TEST + RE15_CARD_AUTO, RE15_PSELECT_AUTO(_SWITCH) fuer Elza, RE15_GB_TEST, RE15_KEIN_GATOR, RE15_MAP_SHOT_PAGE, RE15_PRI_LOG, RE15_WINDOW_SCALE=1) — Semantik im Code nachlesen. Bilder NUR per RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1. Story-/Raum-Punkte ueber den ECHTEN Weg abnehmen (Tuer betreten, Spielstand + CONTINUE), nicht nur per Sprung; Elza-Punkte mit Elza.
- Original-Messung: DuckStation (psx_dev/re15_reborn/psxtest.sh, Skills re15-room-capture / re15-savestate-ghidra; EnableGDBServer in %LOCALAPPDATA%/DuckStation/settings.ini danach zurueck auf false; stage_saves/ sind Savestates des Auslieferungsstands — NICHT die PATCHED-*.sav). RE2: nur statisch (info/re2leon ist ein Dateibaum), ausser es liegt ein RE2-Image vor.
- Tests: NEUE Sonden/Pins in re15_port/tests/unit/probes/r41_${s.thema}.cmake (GLOB; Muster probes/r39_*.cmake) + tests/unit/test_r41_${s.thema}*.c / tests/integration/test_r41_${s.thema}*.cmake (registriert NUR aus deiner probes-Datei). NICHT tests/unit/CMakeLists.txt und NICHT tests/integration/CMakeLists.txt. Mindestens ein Test je Nutzer-Punkt, der die MECHANIK misst. Linux-fest (VERTRAG §2), parallel-fest (eigene Laufordner, exe-Kopie je Test).
- Dateien: neue Logik in NEUEN Dateien engine/src/<thema>_r41.c + include/re15_<thema>_r41.h; gemeinsame Dateien nur mit kleinen Haken (1-5 Zeilen) an klar benannten Stellen, Installer-Zeile am ENDE des Blocks in scd_room_setup.c mit Kommentar "Runde 41 Spur ${s.id}". KEIN Patch an shared_assets/PSX/* (Ausnahme Spur M: MASKS/), pri/ und synchro/ TABU; neue Daten gen/*.inc oder shared_assets/RE2/ (im Dossier nennen). release/, platform/android/, RELEASE_NOTES.md: TABU.
- Token sparen: grosse Dateien nie ganz lesen (grep -n + Bereich), Logs gefiltert, Bau-/Test-Ausgaben in Datei umleiten und nur Ende/Fehler lesen, keine Messung wiederholen, nur urteilstragende Bilder ansehen.

## Abschluss
- Dossier mit Abschnitten: Messung vorher / RE-Belege (Adressen, Bytes) / Umsetzung (Dateien, Konstanten) / Messung nachher / Tests (Namen, Ergebnis) / OFFEN / Fuer den Nutzer (neue Assets fuer das Paket-Gate unter shared_assets/RE2, Sprachdateien mit Pfad, Bedienhinweise, Entscheidungsfragen). Letzter Commit fix/feat(r41-${s.thema}): ... Gezielte Tests gruen: ctest-Summenzeile + -R-Ausdruck woertlich zitieren (KEINE volle Suite). Antwort = StructuredOutput.
`

const bauPrompt = (s) => `Du bist Spur ${s.id} "${s.thema}" der Runde 41 (2026-10-23) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Du ermittelst, belegst, baust und testest ALLE Punkte dieser Spur im eigenen Arbeitsbaum. Ein anderer Agent nimmt danach unabhaengig gegen den Wortlaut des Nutzers ab — was du nicht gemessen und belegt hast, faellt dort durch.
${STAND[s.id] ? `
## ⛔ FORTSETZUNG — dein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST das Dossier, git -C "<baum>" log --stat master..HEAD und den Code-Diff gegen master. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg. Stelle je Nutzer-Punkt fest, was noch fehlt, und schliesse genau das ab; pruefe kritisch, ob der Stand den WORTLAUT erfuellt.
` : ''}
## Deine Punkte (Wortlaut des Nutzers, AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Einstiegspunkte (Stand 2026-10-23 — Zeilennummern und Behauptungen SELBST verifizieren)
${s.hinweise}
${REGELN(s)}
Arbeite jeden Punkt vollstaendig ab (Dossier je Punkt: Messung vorher / Beleg / Umsetzung / Messung nachher). Am Ende: gezielte Tests gruen (keine volle Suite), alles committet, Dossier vollstaendig.`

const nachPrompt = (s, abn, runde, vorher) => `Du bist Spur ${s.id} "${s.thema}" der Runde 41 (2026-10-23) im Projekt reAi_v2 — NACHBESSERUNG ${runde} nach einer unabhaengigen Abnahme. Der Bau-Agent vor dir hat im selben Baum gearbeitet; sein Stand ist committet (lies git -C "<baum>" log --oneline master..HEAD, das Dossier und den Abnahmebericht <baum>/analysis/befunde_runde41/${s.id}_abnahme_${runde - 1}.md).

## Deine Punkte (Wortlaut des Nutzers)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

${STAND[s.id] ? `
## ⛔ FORTSETZUNG — ein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST git -C "<baum>" log --stat master..HEAD, das Dossier und den Abnahmebericht. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg.
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
Punkte, die die Abnahme als erfuellt bewertet hat, NICHT verschlechtern (ihre Riegel gruen halten). Schreibe im Dossier einen Abschnitt "Nachbesserung ${runde}" (je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher). Baue neu, gezielte Tests gruen (keine volle Suite), committe (fix(r41-${s.thema}): Nachbesserung ${runde} ...).`

const abnahmePrompt = (s, bau, runde) => `Du bist die UNABHAENGIGE ABNAHME ${runde} fuer Spur ${s.id} "${s.thema}" der Runde 41 (2026-10-23) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Ein Bau-Agent behauptet, die Punkte des Nutzers umgesetzt zu haben. Du glaubst ihm NICHTS, was du nicht selbst am gebauten Stand gemessen hast. Der Nutzer weist jede Aenderung ohne Disasm-Beleg zurueck — du bist seine Instanz.
${STAND[s.id] && MODUS[s.id] === 'abnahme' && runde === (RUNDE0[s.id] || 0) ? `
## ⛔ FORTSETZUNG EINER ABGEBROCHENEN ABNAHME
${STAND[s.id]}
` : ''}
## Die Punkte des Nutzers (Wortlaut; AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Behauptung des Bau-Agenten
Status: ${bau ? bau.status : 'KEIN ERGEBNIS (Agent abgebrochen) — pruefe Baum und Dossier trotzdem'}
${bau ? bau.zusammenfassung : ''}
Commits: ${bau && bau.commits ? bau.commits.join(', ') : '?'}
Suite laut Bau-Agent: ${bau ? bau.suite : '?'}
Offen laut Bau-Agent: ${bau && bau.offene_punkte && bau.offene_punkte.length ? bau.offene_punkte.join(' | ') : '(nichts)'}

## Baum
${ROOT}/.claude/worktrees/${s.baum} (Zweig r41/${s.thema}, Basis master ${BASIS}). ALLE Zugriffe und git-Befehle nur dort (absolute Pfade, git -C "<baum>", Bash mit cd "<baum>"). Nichts ausserhalb des Baums aendern. Aus dem Hauptbaum ${ROOT}/ nur lesen.

## Vorgehen
1. Lies <baum>/analysis/befunde_runde41/AUFTRAG.md, VERTRAG.md, das Dossier ${s.id}_${s.thema}.md und: git -C "<baum>" log --oneline master..HEAD; git -C "<baum>" diff master --stat; git -C "<baum>" diff master -- re15_port (den Code-Diff WIRKLICH lesen).
2. Baue: cd "<baum>" && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build (nie cmake --build aus Git-Bash). KEINE volle Suite (test/all) im Spurbaum. Fahre GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = Tests der Spur (r41_${s.thema}) + die Testgruppen aller Module/Raeume, die der Diff beruehrt (git diff master --name-only; git grep -l; ctest -N | grep -i). Rote Fenster-Haken unter Last 2-3x einzeln nachfahren, bevor du sie wertest. NIE taskkill /IM re15_pc.exe; Messkopie der exe unter eigenem Namen. Bau und exe-Lauf nie in einem Aufruf; exe mit timeout.
3. MISS jeden Nutzer-Punkt selbst an der exe (<baum>/re15_port/build/platform/pc/re15_pc.exe; Haken getenv("RE15_...") — Semantik im Code nachlesen; Spielstand + CONTINUE, RE15_DEBUG_JUMP, RE15_FORCE_EVENT, RE15_PLAYER_POS, RE15_PRESS/RE15_PAD_AT/RE15_INPUT_SCRIPT, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, Logs; Elza-Punkte mit RE15_PSELECT_AUTO). Story-/Raum-Punkte ueber den ECHTEN Weg (Tuer betreten), nicht nur per Sprung. Urteil je Punkt gegen den WORTLAUT: erfuellt / teilweise / nicht_erfuellt / nicht_pruefbar (mit Grund) — mit Messprotokoll (Befehl, Log-Zeilen, Bild-Pfad). Bei RE2-Uebernahmen: stichprobenartig die zitierten RE2-Stellen (EM23_OVL / ghidra_re2_Leon.txt / RE2-RDT-Offsets) selbst disassemblieren bzw. lesen.
4. RE-Gate: Jede verhaltensrelevante Konstante im Diff hat @0x... (oder Byte-Offset) ODER ist als NUTZER-VORGABE / PORT-WAHL gekennzeichnet; stichprobenartig 3 Adressen SELBST disassemblieren (re15_disasm.py / re2_disasm.py / ghidra*.txt) und pruefen, dass die zitierte Stelle das Behauptete enthaelt (Sprungziel disassemblieren, richtige Binaerdatei). Suche im Diff nach "deferred", "tunable", "interim", "for now", "faithful", "plausibel", "TODO", Env-Schaltern als Abschluss. Pruefe, ob der Fix den gemessenen Befund ERKLAERT.
5. Vertrag: nur zugeteilte Bank-9-Bits/Nachrichten-IDs/Slots/Ereignisse (VERTRAG.md); erlaubte Pfade (git diff master --name-only: keine release/, platform/android/, shared_assets/PSX/ (ausser M: MASKS/), pri/, synchro/, keine Test-CMakeLists); gemeinsame Dateien nur kleine Haken; Tests je Punkt vorhanden und messend (ctest -R "r41_${s.thema}" laufen lassen). Nebenwirkungen auf beide Szenarien und Nachbarraeume pruefen.
6. Schreibe den Bericht <baum>/analysis/befunde_runde41/${s.id}_abnahme_${runde}.md (je Punkt Messprotokoll, Urteil, Beleg; Gates; Maengel nummeriert, konkret, nachpruefbar) und committe NUR diese Datei: git -C "<baum>" add analysis/befunde_runde41/${s.id}_abnahme_${runde}.md && git -C "<baum>" commit -F <msgdatei> (Text: "doc(r41-${s.thema}): Abnahme ${runde}" + Leerzeile + "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"). Du aenderst KEINEN Code. Schreibe den Bericht FORTLAUFEND (nach jedem gemessenen Punkt speichern) — das Sitzungslimit kann dich jederzeit beenden. Kein SendMessage an andere Agenten.
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
    fragen_an_nutzer: { type: 'array', items: { type: 'string' } },
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

const WELLE = (CFG.welle || SPUREN.map(s => s.id)).map(id => SPUREN.find(s => s.id === id)).filter(Boolean)
const MODUS = CFG.modus || {}
const STAND = CFG.stand || {}
const BAU_VORHER = CFG.bau || {}
const RUNDE0 = CFG.runde0 || {}
const ABN_VORHER = CFG.abn || {}
const MAX_NACH = CFG.max_nach === undefined ? 2 : CFG.max_nach
const MODELL = CFG.model ? { model: CFG.model } : {}

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
