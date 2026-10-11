export const meta = {
  name: 'r36-runde',
  description: 'Runde 36: 16 Bau-Spuren je Ermittlung+Bau -> unabhaengige Abnahme -> Nachbesserung, in eigenen Arbeitsbaeumen (Semaphor)',
  phases: [
    { title: 'Ermittlung+Bau', detail: 'Dossier, Messung/Bisect, RE-Beleg, Code, Tests, Suite im eigenen Baum' },
    { title: 'Abnahme', detail: 'unabhaengig gegen den Wortlaut des Nutzers am gebauten Stand gemessen' },
    { title: 'Nachbesserung', detail: 'Maengel der Abnahme beheben, erneut messen' },
  ],
}

const ROOT = 'C:/workspace/git/reAi_v2'
const BASIS = '78a4a748'
const SLOTS = 5
const MIN_TESTS = 592

const SPUREN = [
  { id: 'A', thema: 'klettern', baum: 'r36_klettern',
    punkte: [
      'Ganz wichtig - aus irgendeinem Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B. ROOM 10F0. Das macht einen Fortschritt im Spiel unmoeglich',
    ],
    hinweise: 'BLOCKER, zuerst REPRODUZIEREN: exe aus dem eigenen Baum, Spielstand/RE15_DEBUG_JUMP=10F0 (oder der echte Weg), zum verschiebbaren Tritt gehen, Aktionstaste — was passiert (Log RE15_CLIMB_TRACE in engine/src/climb_common.c, RE15_STATE_LOG, RE15_PAD_AT/RE15_INPUT_SCRIPT, Framedump mit RE15_WINDOW_SCALE=1)? Dann BISECT: v0.8.21 (9362dd7e) vs v0.8.22 (87cc8575) vs v0.8.23 (3ec5b1f8), danach die Merges/Commits der Runde 35 (git log --oneline 9362dd7e..3ec5b1f8; Kandidaten: Spur E inventar1050 (Pad-Bit/pauseflags, Messer-Rueckfall, ammo), Spur J affen (game_step_common.c Opfer-Schub/Platzierung, re15_body_push_player), Spur H raeume (Sichtfilter, Opfer-Lage, climb_common.c:360 dz-Rundung erwaehnt), Spur I entladen (Caches), Spur C zgirl (Selbsttueren laden den Raum neu — ROOM10F0 hat Tueren in den eigenen Raum?), Spur F inhalt (Zombie-Abstand-Overrides), Spur K cut10f0 (Boot-Weg-Installer/Leihe in 10F0!)). Zum Bisect in einem ZWEITEN Bauverzeichnis bauen: git -C <baum> checkout <commit> -> RE15_BUILD_DIR=<baum>/re15_port/build_bisect bash re15_port/tools/local_build.sh configure/build (danach zurueck auf r36/klettern). Verursachenden Commit + Mechanismus (Original: Klettern = Treppen-/Tritt-Logik FUN_... in climb_common.c mit Adressen) belegen, beheben, Pin, der das Klettern in ROOM10F0 ueber den echten Weg misst (Tritt verschieben + hochklettern + oben ankommen) — in beiden Szenarien.' },

  { id: 'B', thema: 'kleidung', baum: 'r36_kleidung',
    punkte: [
      'Aus irgendeinem Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.',
    ],
    hinweise: 'REPRODUZIEREN: Leon auf Warning/Danger bringen (RE15_KILL_AT/Schaden-Haken oder Gegner), Framedump (RE15_WINDOW_SCALE=1) der Kleidung vorher/nachher; v0.8.21-exe als Gegenprobe (dort sichtbar?). Wie realisiert der Port die verletzte Kleidung (PLD-Texturseiten/Varianten je Gesundheitsstufe, grep in platform/pc + engine nach damage/verletzt/wound/blut/Warning/Danger beim Spieler-Modell; TIM-Slot 0 = Spieler-Puffer 0x801bd814) und wie das Original (PLD mit mehreren Texturvarianten? FUN_... der Health-Stufen-Umschaltung — Adresse)? BISECT ueber die Runde-35-Commits (Kandidaten: Spur I entladen — render_pc.c TIM-Slots/Freigabe, Spur E Messer/Equip -> Waffen-Render laedt Spieler-Textur neu?, Spur J/H Spieler-Pose/Neck). Beheben, Pin mit Bild-Messung (Pixel-Differenz der Uniform zwischen Fine und Danger), Elza ebenfalls pruefen.' },

  { id: 'C', thema: 'schuss', baum: 'r36_schuss',
    punkte: [
      'Grenze aus v0.8.22/23, vom Nutzer bestellt: Alle Schusswaffen treffen ueber den Konferenztisch und treffen fressende Zombies. Im Original geht beides nicht. -> beheben (Sichtlinie + Fresser-Sperre fuer ALLE Schusswaffen)',
      'Bei den Zombies habe ich manchmal das Gefuehl, dass das Blut nicht an der richtigen Stelle auftaucht, wenn sie getroffen werden',
      'Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in Resident Evil 2 auch so? (belegen; wenn RE2 anders ist: RE2-Verhalten bauen)',
    ],
    hinweise: 'Belege liegen in analysis/befunde_runde35/B_werfer.md (OFFEN 16/17): RE1.5-Schusspfad FUN_80011f50 @0x80012168-70 -> Sichtlinie FUN_8001b9b4 @0x8001ba1c-24 -> FUN_8003dcc4 @0x8003de5c-94 (sperrt an Klasse-3-Zellen, ROOM1140 Tisch Zelle 6 Wort 0x0300); RE2-Fresser-Gates +0x1D3 (0x80 gesetzt durch EXEC[8] @0x80103c04-14) und +0x10E im RE2-Schuss-Resolver @0x80041270-9c und im Applier @0x80047138-40/58-64; Spur B hat sie fuer die Werfer (re2_fx.c) gebaut — jetzt fuer den allgemeinen Resolver in engine/src/re15_damage.c (alle Schusswaffen inkl. Messer? Messer = Nahkampf, pruefen). Messen: ROOM1140 Python ueber den Tisch (vorher Treffer, nachher nicht), fressende Zombies aus 5000 Entfernung (vorher Treffer, nachher nicht; naeher als 0xFA0 wachen sie auf). Blut-Position: messen, wo der Port den Blut-Anker setzt (re15_damage.c / Gore / Spur J r35 Hinweis: Original nimmt die Part-8-/Trefferteil-Matrix des zuletzt GEZEICHNETEN Bildes FUN_80019700, der Port das laufende Bild) — Original-Mechanismus fuer den Trefferpunkt (Zombie-Treffer-Handler, welcher Part/Offset, @0x...) belegen und angleichen; Pin mit Lage-Differenz. Kraehe + Magnum: RE2-Kraehen-Handler (info/re2leon, ghidra_re2_Leon.txt; ENEMY07) — Todesreaktion auf Magnum-Kritklasse disassemblieren (Zerplatzen? Teile? verschwindet der Koerper?) und mit dem Port vergleichen (enemy_ai_re2_crow / crow-AI, RE15_RE2_DMG_MODEL); Antwort fuer den Nutzer ins Dossier, Abweichung beheben.' },

  { id: 'D', thema: 'granate', baum: 'r36_granate',
    punkte: [
      'Bei normalen Granaten brauche ich keinen Feuer-Effekt bei den Zombies, sondern nur das Zerplatzen der Koerperteile. Feuer wuerde ich nur bei den incendiary Granaten erwarten.',
      'Manchmal selten loest die Granate bei der Verwendung nicht aus/wird nicht geworfen.',
    ],
    hinweise: 'Stand: engine/src/granate_r35.c + re15_damage.c re15_re2_gl_kandidat (Explosiv-Treffer -> RE2 DEATH[9][1] @0x8010CD68+4 = Zerreissen) und die RE2-FX-Maschine re2_fx.c (Op 47/48/49; nach dem HE-Tod brennt die RE2-Zombie-Leiche mit zwei Flammen — RE2-Russ, Memory v0.8.21). Messen: welche Flammen/Brand-Effekte spawnen bei der Handgranate (RE15_FX_LOG, Framedump) und woher (Op 47 Sub-Explosion, Brand-Zweig re2z_death_rip Russ-Flammen); NUTZER-VORGABE: bei Art 2 (Handgranate/Explosiv) KEIN Feuer an Zombies, nur Zerreissen; bei Art 4 (Brand/incendiary) Feuer wie bisher; Art 3 (Saeure) unveraendert. Form belegen (welcher RE2-Op setzt die Flammen, @0x...), gezielt abschalten, Pin: Zaehlung Flammen-Slots nach HE = 0, nach Brand > 0. Wurf-Aussetzer: viele Wuerfe automatisiert (RE15_INPUT_SCRIPT, Positionen/Blickrichtungen/Zielhoehen, an Waenden, in Ecken, mit 1 Granate im Stapel, beim Nachladen/Wechsel, waehrend Ziel-Animation) mit RE15_GRANATE_LOG/RE15_WAFFEN_LOG; Kandidaten: Wurfbild-Regel der Spur A r35 (Zelle zwischen Werfer und Hand -> Sofortzuendung/kein Wurf?), Wand im Wurfbild, Dauerfeuer-FSM/Clip-Laenge (Memory reai-v2-dauerfeuer-fsm), Item-Stapel 1 -> 0, ESP-Slot voll (Routine 29/30/31 ohne freien Platz?). Ursache belegen (Original: Wurf-Handler @0x..., ESP-Platzvergabe), beheben, Pin mit N Wuerfen = N Granaten.' },

  { id: 'E', thema: 'gegnerzahl', baum: 'r36_gegnerzahl',
    punkte: [
      'Grenze aus v0.8.22/23, vom Nutzer bestellt: Der Port verwaltet hoechstens 15 Gegner je Raum, das Original 20. In ROOM1040 und 1030 fehlen deshalb die letzten fuenf Zombies. -> beheben',
    ],
    hinweise: 'Beleg (analysis/befunde_runde35/L_cut1150.md §10): Gegner-Feld des Originals hat 20 Plaetze (FUN_80037250 @0x800372b4 sltiu 0x14; Entity 0x800acc2c + Slot*0x1F4; Sce_em_set legt ohne Grenzpruefung ab @0x800420b0/@0x800420dc/@0x80042118; Limit-Gate @0x80042214-3c, Zaehler 0x800b0ff2). Port: RE15_ACTOR_MAX 16 (include/re15_actor.h:22, Spieler + 15) -> 21 (Spieler + 20). ZENSUS aller Nutzer: grep -rn RE15_ACTOR_MAX, feste 16/15, Bitmasken (1u << slot) mit uint16, Arrays je Slot in enemy_ai_*, affen, irons_tod (s_b1040_da), render/PRI/Schatten-Listen, Save-Format (re15_savepoint.c — wird der Aktor-Pool gespeichert? Spielstand-Kompatibilitaet messen: alter Spielstand laden), Tests mit festen 16 (anpassen, mit Beleg). Spur L hat die Auffuellung in ROOM1040 ueber ERSCHEINENDE Records gerechnet (irons_tod_1150.c re15_irons_tod_sichtbar_1040) — mit 20 Plaetzen muss sie weiter 5 ergeben (Riegel montage_1040_reihe). Messen: ROOM1040/1030 ohne Szene: Records 15..19 erscheinen (RE15_SPAWN_DIAG), Montage weiter 5/5; ALLE Unit-Tests (ctest -R "^unit_" -E "android_bash" --timeout 600; RE15_ACTOR_MAX beruehrt jedes Modul) + die integration-Tests der Spawn-/Montage-Raeume (1030/1040/1130/1150, ctest -N | grep) — die volle Suite faehrt der Orchestrator; Laufzeit (Zombies pro Bild) im Blick.' },

  { id: 'F', thema: 'zombieki', baum: 'r36_zombieki',
    punkte: [
      'Die Zombies verhalten sich anders als in Resident Evil 2 - sie bleiben hier ewig beteiligungslos stehen, obwohl ich ziemlich in ihrer Naehe vor ihnen bin, fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind. Das darf so nicht sein.',
      'Die Monkeys (ROOM11C0) scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern.',
      'Bei den Monkeys in ROOM11C0 laeuft das Blut nicht aus, wenn sie tot sind.',
    ],
    hinweise: 'Zombies: Standard-KI ist RE2 (enemy_ai_re2_zombie.c). REPRODUZIEREN in mehreren Raeumen (1010, 1140, 1220, 1030; Leon 500-1500 vor einem stehenden Zombie, 600 Bilder, RE15_ENEMY_DBG/RE15_STATE_LOG: Zustand/Sub/LOS/Fresser-Bit +0x1D3/+0x10E/Timer). Kandidaten aus Runde 35: Spur H raeume Sichtfilter (re15_los_ray_blocked / re15_re2_los_clear mit Aktor-Maske statt Original-Konstante 0x2000 @0x8004a868 — kann den Zombie blind machen), Spur B werfer Fresser-Sperre (+0x1D3 0x80 bleibt gesetzt?), Spur C zgirl Selbsttueren (Raum neu aufgesetzt -> Zustand), Spur F inhalt Zombie-Abstand (zombie_abstand_r35.c — Spawn-Overrides mit Sperr-Zustand?), Spur J Fusssperre/Pool-Pose in enemy_ai_common.c. BISECT ueber v0.8.21/22/23 und die Merges. RE2-Original: Weck-/Angriffsbedingungen des Zombie-Hirns (Abstand, Sicht FUN_80050858 mit Maske 0x2080, Timer, Zufall; ghidra_re2_Leon.txt, RE2_Quellcode_V2; DuckStation-GDB auf RE2-Savestates wie Spur H/J) belegen, Port angleichen, Pin: Zombie 800 vor Leon greift binnen N Bildern an (N aus RE2). Monkeys: affen_11c0.c (Spur J r35, Dossier J_affen.md mit GDB-Werkzeugen und Original-Spuren) — messen, WAS blockiert (Wand-Zonensprung = Original laut J; andere Faelle: Wagen, Pfeiler, Leon in Ecke?), gegen das Original (GDB) pruefen, nur echte Abweichungen beheben. Blutlache: Original-Todes-Handler des Gorillas (STAGE1 0x8011xxxx, Dispatch @0x80072bac Typ 0x27) — welche ESP-Routine/Decal setzt die Lache (bei Zombies im Port vorhanden: Blutlache r15), im Port fuer 0x27 nachziehen, Pin.' },

  { id: 'G', thema: 'tueren', baum: 'r36_tueren',
    punkte: [
      'Einige Tueren in den Tuersequenzen sind noch nicht ganz korrekt. So zum Beispiel der Elevator ROOM 1080 und die grosse Tuer in ROOM 2090. Ich glaube beide liegen auch korrekt in Resident Evil 2 vor.',
      'Die Tuer-Textur zwischen ROOM 10C0 und 10D0 bei der Tueranimation ist herausstechend billig im Vergleich zum Rest.',
      'Tueren, die erfolgreich geoeffnet wurden mit Karte oder sowas, machen in Resident Evil 2 erst ein "piep"-Sound und dann ein Unlock-Geraeusch. Bei uns nicht.',
    ],
    hinweise: 'Tuersequenzen: engine/src/door_seq_*.c + platform/pc/src/door_scene_pc.c, Zuordnung tools/tueren/ + shared_assets/RE2/DOOR/DOORxx.DO2 (27 RE2-Archive) und RE15DOOR (30 Port-Archive); Memories r31/r33 (83/144 Tuerseiten = RE2-Tuer, Variante = gemalter Griff). Messen (RE15_TUER_TEST/RE15_TUER_SEITE + Framedump): welches Modell spielt 1080 (Fahrstuhl) und 2090 (grosse Tuer) heute, welches RE2-Archiv passt (RE2-Fahrstuhl-/Aufzugtueren und das grosse Tor: alle DOORxx.DO2 der RE2-Disc info/re2leon durchsehen, Bilder je Archiv rendern, gegen das RE1.5-Hintergrundbild der Tuer vergleichen); ebenso 10C0<->10D0 (welches Archiv, warum die Textur grob: RE1.5-Tuer 45 Dreiecke statt RE2?). Zuordnung/Archiv belegen (DO2-Aufbau, Offsets), umstellen, neue DO2 nennen (Paket-Gate). Karten-Tueren: welche Tueren oeffnen per Karte/Keycard (AOT-Tuer mit Key-Item, z.B. Communication Room Staff Card) — RE2-Ablauf beim Oeffnen einer verschlossenen Tuer mit Schluessel: erst Piep (RE2 ROOM213 Electronic lock / ROOM616 card swipe) dann Unlock (ROOM114/115/204 Lock/Unlock) — RE2-Handler disassemblieren (welche SE-Codes in welcher Reihenfolge, @0x...), Toene aus info/re2leon schneiden (shared_assets/RE2/...), im Port an den Schluessel-Tuer-Pfad haengen (nur bei Karten-/elektronischen Tueren? oder allen Schluessel-Tueren — RE2 belegen), Pin ueber RE15_SE_DEBUG/Mitschnitt.' },

  { id: 'H', thema: 'ziel1190', baum: 'r36_ziel1190',
    punkte: [
      'Der ergaenzte Text in ROOM 1190 bei der Zielscheibe ist falsch. Die Texte "This target has a surprisingly large number of bullet holes." und "This target does not have many bullet holes" sollen sich hinten bei den Zielscheiben befinden, wenn man sie anklickt, nicht vorne bei den Knoepfen, die die Zielscheiben bewegen lassen. Bei Elza sind es genau die anderen zwei Zielscheiben, deshalb muessen die Texte da genau vertauscht sein.',
      'Frage: Was hat es mit der Kiste in ROOM 1090 auf sich? Hat die irgendeinen Verwendungszweck?',
    ],
    hinweise: 'Stand: engine/src/ziel_1190.c (Runde 35 Spur H) haengt die Texte an die 4 Knopf-AOTs (sce 5, Slots 0-3, x=-4400). Jetzt: die Zielscheiben HINTEN (Objekte/AOTs am Ende der Bahn: welche AOTs liegen dort — rdt-analysis/scd-disassembly auf ROOM1190.RDT und 1191.RDT, Kamera-Cuts der hinteren Scheiben, ggf. keine AOT -> eigener Text-AOT je Scheibe an der Scheiben-Position aus dem Objektmodell/Hintergrund), Zuordnung ganz links/3. von links per Kamera-Projektion messen (nicht Slot raten), Leon: Scheibe 1 und 3 = viele Loecher, 2 und 4 = wenige; Elza (ROOM1191): genau vertauscht (2 und 4 viele, 1 und 3 wenige). Die Knoepfe bekommen wieder ihr Original-Verhalten/ihren Original-Text (Umbau von r35 zuruecknehmen, Original-Bytes). Nachrichten-IDs 6..11. Pin je Szenario ueber den echten Weg (zu jeder Scheibe laufen, Aktion, Text pruefen; Framedump RE15_WINDOW_SCALE=1). Kiste ROOM1090: Objekt-/AOT-Zensus (Obj_model_set, Item-AOTs, Sce-Opcodes, Flags) in ROOM1090/1091 + Original-Overlay — hat die Kiste eine Funktion (Item, Rätsel, Blockade, Sichtschutz, nur Deko)? Antwort mit Belegen (Offsets) im Dossier.' },

  { id: 'I', thema: 'inventar', baum: 'r36_inventar',
    punkte: [
      'Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards oder Handgranaten 0, sollen sie aus dem Player-Inventar verschwinden',
      'Beim Debug-Menue, bei dem man zwischen den Raeumen springt, schickst du, wenn man mit Elza spielt, Elza in Leons Raeume. Wenn man mit Elza spielt, sind die Raeume mit "1" hinten korrekt, zum Beispiel ROOM 1071.',
    ],
    hinweise: 'Speichern: engine/src/re15_savepoint.c (Memory Card = Item, Hebetisch-Memory-Card r33/r34) — RE1.5 Original kann nicht speichern (Preview); Beta->Retail: RE2 Ink-Ribbon-Logik (Speichern verbraucht 1, bei 0 Slot leer: RE2 FUN_... Save-Flow, Item-Abzug + Slot-Loeschung FUN_8004947c-Zwilling) belegen und uebernehmen. Handgranaten: Stapel 0 nach dem letzten Wurf -> Slot entfernen (RE2: Item mit Menge 0 wird geloescht — Adresse), gleiches fuer alle Mengen-Items (Saeure-/Brandgranate, Munitionsschachteln bereits?). Pins: Speichern -> Zaehler -1; 1 -> 0 -> Slot weg; Wurf der letzten Granate -> Slot weg; Statusschirm zeigt nichts Leeres. Debug-Menue: engine/src/debug_menu_common.c Raumsprung — Zielraum = (stage, room, variante): Variante aus dem Spielercharakter/Szenario-Flag (wie waehlt der Raumlader FUN_800396fc die xxx1-RDT? Charakter-Flag, Adresse) — bei Elza immer xxx1; messen mit RE15_PSELECT_AUTO (Elza) + Debug-Menue-Sprung nach 1071, Log "loaded room1071".' },

  { id: 'J', thema: 'szenenleon', baum: 'r36_szenenleon',
    punkte: [
      'In der Cutscene in ROOM 11B0 mit Leon, Ada und Marvin klappt das mit der Animation und dem gemeinsamen Wegschieben des Containers mit Marvin noch nicht.',
      'ROOM 11B0, Cutscene (Ada, Marvin und Leon verlassen STAGE1 Richtung Sewer) am Ende erweitern — GEAENDERT durch den Nutzer 14:00 (AUFTRAG.md Nachtrag: "in Elzas Szenario ist fuer mich auch ok, wenn er nur die Werkstatt Tuer abschliesst, nicht die Parking Lot tuer"): Marvin: "Hold on" — Marvin rennt zur Innenseite der Tuer zur Werkstatt in ROOM 11B0 und fuehrt dort eine Zuschliess-Animation mit Zuschliess-Sound aus (aus Resident Evil 2 von Robert Kendo am Spielanfang); die Tuer bleibt dann geschlossen mit der gleichen Geschlossen-Meldung wie in Elzas Szenario — danach rennt Marvin zurueck zu Leon und Ada: Marvin: "I just want to make sure we don\'t have any unwanted followers." — geeignete Animationen aussuchen — die Werkstatt-Tuer danach beim Anklicken mit dem Text wie in Elzas Szenario (Tuer geschlossen). Die Parkplatz-Tuer 11B0 -> 11C0 wird NICHT abgeschlossen (Spur Q sperrt sie nur bis zum Parking-Lot-Schluessel).',
      'In der neuen Cutscene in ROOM 10F0 verfolgt die Kamera, wie Marvin und Ada den Raum verlassen; sie ploppen dann einfach weg. Bleibe mit der Kamera auf Leon fixiert, wenn Marvin und Ada den Raum verlassen, und mache an der Stelle nur einen Tuer-schliessen-Sound zum Schluss.',
      'Sobald Irons gegen Ende von Stage1 tot ist, soll er auch nicht mehr den Kopf hinter Leon her bewegen.',
    ],
    hinweise: 'ROOM11B0-Szene = ORIGINAL-Sub (sub06 Ada/Marvin/Leon, analysis/nutzer_batch_2026-08-30b/ada-marvin-vorszene.md; scd-disassembly): Container-Schieben — welche Opcodes bewegen das Objekt (Obj/Pos_set/Work_set/Member_set) und welche Clips spielen Marvin/Leon (Plc_motion/Em_set_motion) — am Original (DuckStation-Savestate der Szene: Skill re15-room-capture/re15-savestate-ghidra, stage_saves/) messen, was im Port fehlt (Objekt bewegt sich nicht? Clip falsch? Anker?), beheben. Erweiterung: Port-Programm (Vorbild cut_10f0.c/adaruf_1050.c: SCD aus Original-Opcodes, Ereignis 22/23 in 11B0 laut VERTRAG) NACH dem Original-Sub anhaengen (Haken am Evt_end des Subs): Zeilen (Nachrichten-IDs 20..27, Sprecher "Marvin:"), Plc_dest zu den Tueren, Zuschliess-Clip: RE2 Kendo-Szene (Leon A, Gun Shop room101: Kendo schliesst die Tuer ab — Sub disassemblieren, Clip-Nr./EMD des Kendo-NPC; fuer Marvin einen passenden Clip aus Marvins eigener Bank oder RE2-Marvin-Bank belegen), Zuschliess-Ton aus RE2 (ROOM101.SAP = Gun Shop; RE2_SOUND_LISTE.md) schneiden -> shared_assets/RE2/, danach NUR die Werkstatt-Tuer als geschlossen: Tuer-AOT auf Text umgetypt (Muster tuer1120_1130.c) mit den BYTES von Elzas Text ROOM11B1 msg 12 "The door is locked." (@0x179F), persistent ueber (9,84). Die Parkplatz-Tuer 11B0 -> 11C0 bleibt unberuehrt (Spur Q retypt sie allein, Schluessel-Sperre Bit 82) — kein Slot-Konflikt mehr. 10F0: cut_10f0.c Szenenende — Kamera bleibt auf Leon (Cut, in dem Leon steht), Marvin/Ada laufen hinaus (Plc_dest zur Tuer + verstecken erst NACH Verlassen des Bildes/Tuerposition), Tuer-zu-Ton (RE2 DOOR-Tonteil, cut10f0 hat schon die Tuerbank) am Ende; Pin: kein sichtbares Ploppen (Framedump-Serie, NPC-Sichtbarkeit nur ausserhalb des Bildes beendet). Irons: NPC-Hals-FSM (Plc_neck/npc look-at in enemy_ai_common/npc) fuer Irons in 1150 bei (9,73)=1 aus (Original-Form: tote NPCs haben keinen Neck-Tick — Beleg im Irons-Handler/NPC-Wurzel @0x...).' },

  { id: 'K', thema: 'szene1150', baum: 'r36_szene1150',
    punkte: [
      'Bei der finalen Cutscene mit Irons, wo er stirbt, ist das Tuerknallen noch falsch. Das ist kein Tuerknallen, das ist einfach nur der Sound von Tuer schliessen. Suche den Tuer-Knall-Sound von zum Beispiel Resident Evil 2 (Nutzer: bei "Door sounds" oder "Zombies breaking through wood" suchen, Liste RE2_SOUND_LISTE.md).',
      'In der Cutscene, wo Marvin und Ada in ROOM 11C0 stehen, ist der Dialog mit Marvin und Ada noch schrott: sie unterhalten sich nicht miteinander, sondern schauen irgendwo hin. Sie muessen sich schon miteinander unterhalten.',
      'Ebenfalls in der Cutscene kommt das Tuerknallen, dann die Kamera-Ansicht und dann die Zombies aus dem Nichts. Erst Tuerknallen, dann Zombies spawnen, dann erst der Kamera-Wechsel hin zu den Zombies.',
    ],
    hinweise: 'Eigentuemer: engine/src/irons_tod_1150.c (Spur L r35, Dossier L_cut1150.md §8-10; Knall = Signale (5,28)/(5,29) -> Knall-Bank engine/src/gen/knall_bank.inc, Herkunft dort nachlesen). Tuerknall: RE2-Toene aus RE2_SOUND_LISTE.md — Kandidaten ROOM205 (Zombies smashing through wood), ROOM112 (Library, Fenster-Einbruch), ROOM106 (Cabin, Zombies breaking in), ROOM207/209/20A (Door sounds), ROOM204 (Shutter): aus info/re2leon schneiden (Muster Spur M r35 glas1090_extrakt.py), anhoeren ist nicht moeglich -> Auswahl ueber Wellenform/Dauer/Nutzung im RE2-Skript belegen (welcher SE wird in RE2 bei "Zombies brechen durch die Tuer" gespielt: RE2-RDT-Sub des Raums mit Se_on-Code), Bank nach shared_assets/RE2/, im Dossier Alternativen nennen. Dialog 11C0 Cut 13: Ada und Marvin einander zuwenden (Plc_dir/Plc_neck auf den Partner, Blick waehrend der eigenen Zeile zum Gegenueber — Form aus Original-Dialogen, z.B. ROOM11C0 sub02 @0x0185E/@0x01886), Framedump-Belege. Reihenfolge je Montage-Schritt: Knall -> Spawn (Sce_em_set/Member_set) -> erst danach Cut_chg (Kamera); Pin ueber die Ereignisfolge im Log (Bildnummern: Knall < Spawn < Kamera) fuer 1130, 1040 und 1030.' },

  { id: 'L', thema: 'elza', baum: 'r36_elza',
    punkte: [
      'Bei Elzas Szenario soll die Scheibe in Room 1120, die bei Leons Szenario zum Schluss zerspringt in Stage1, bereits immer kaputt sein, und eine Kraehe mehr im Raum.',
      'Bei Elzas Szenario soll Irons immer bereits von Anfang an tot da liegen. Wenn sie ihn anklickt, soll da stehen "He has already passed...".',
      'In ROOM 1211 in Elzas Cutscene, wenn sie Kendo befreit, fehlt noch der Unlock-Sound bei der Tuer, wenn sie sie aufschliesst.',
      'In Elzas Cutscene in Room 1021, wo Kendo Roy erschiesst, fehlen die Schuss-Sounds.',
      'In Elzas Cutscene in Room 1021, wo Kendo Roy erschiesst, fehlt noch ein Sterbe-/Leid-/Hinfall-Sound von Roy, sobald er erschossen wurde.',
      'Ich habe fuer Elzas Szenario Synchron-Dateien hinzugefuegt. In Room 1101 - waehrend Roy bewusstlos wird - soll das Voiceover main 16.5 abgespielt werden, auch wenn es aktuell keinen Text dafuer gibt. Erstelle einen Untertitel dafuer, wie bei den anderen auch.',
    ],
    hinweise: 'Elza = RDT-Variante xxx1 (Charakter-Flag des Spielers; Beleg wie der Raumlader die Variante waehlt). Fenster 1121: engine/src/fenster_1120.c (Spur M r35) — fuer Elza: Schaden-Overlay immer sichtbar (re15_fenster1120_schaden_sichtbar: Szenario-Zweig), kein Ereignis; dazu EIN zusaetzlicher Kraehen-Spawn in 1121 (Typ 0x21, Position/Clip aus dem 1120-Fenster-Spawn ableiten — Form Sce_em_set, Kill-Flag aus freiem Zone-7-Bit laut Zensus, Beleg). Irons 1151: Original msg 2 = Irons lebt ("I\'ll be fine") — NUTZER-VORGABE ueberschreibt: Irons-NPC beim Raumaufbau in die Totenpose (Spur L r35: Clip 2 Bild 89 gehalten, irons_tod_1150.c Totenpose-Riegel) auf der Liege, der Original-Dialog-Sub wird nicht gestartet, Anklick-Text "He has already passed..." (ID 20, Form 04 02 ... 01 00 wie Text-AOTs), kein Kopf-Tracking. 1211 Kendo-Befreiung: Original-Sub disassemblieren (Aufschliess-Moment = welcher Opcode/Bild), Unlock-Ton (RE2 ROOM114/115/204 Unlock oder RE1.5-eigene Tuer-SE — belegen, Beta->Retail), Se_on-Zeile als Port-Haken an dieser Stelle. 1021 Kendo erschiesst Roy: Sub disassemblieren (Schuss-Bilder: Kendo-Clip/Muendungsfeuer), Schuss-SE (Kendos Waffe = Schrotflinte? RE2 WEAPON07 / RE1.5 ARMS-Bank), Roys Sterbe-/Hinfall-Ton (RE2 CORE-Baenke sind Spieler; fuer NPC-Roy: RE1.5-Stimmbank oder RE2 ENEMY28-artiger Schmerzlaut + Koerper-Fall-Ton ENEMY00 #5/#8 — Auswahl belegen und begruenden), an den Bildern des Subs einhaengen. 1101 main16.5: Datei liegt unter synchro/STAGE1/room1011/main16.5.wav (nicht 1101!) — pruefen, ob der Nutzer 1011 meint (Elza-Variante von ROOM1010 = 1011; "Room 1101" koennte ein Tippfehler sein: wo wird Roy bewusstlos — in welchem Raum laeuft diese Szene bei Elza? belegen) und die Datei dort einbinden, wo die Szene laeuft; Untertitel: neuer Port-Text (ID aus VERTRAG) mit Sprecher "Roy:" — den WORTLAUT kennen wir nicht (keine Transkription moeglich): Dauer der wav messen, Untertitelzeile "Roy: ..." fuer die Dauer anzeigen und im Dossier/Abschluss den Nutzer um den Wortlaut bitten; die Stimme ueber scd_queue_voice-Muster an das Bild "Roy sinkt zusammen" binden (Sub disassemblieren).' },

  { id: 'M', thema: 'karte', baum: 'r36_karte',
    punkte: [
      'Im Treppenhaus 10A0 auf Etage B1 fehlen die Treppen eingezeichnet.',
      'In ROOM 11D0 lande ich auf der Map direkt in der Mitte, sobald ich durch die Tuer gehe, statt am Tuereingang zu stehen.',
      'Bei ROOM 1090 auf der Map gibt es 2 Tueren, eine am Ende des Ganges, wo die Hunde durch die Luke kommen, und eine, wo man die Weste bekommt. Das muss als 2 separate Bereiche in der Map eingeblendet sein und sauber getrennt.',
      'Die Tuer von ROOM 11C0 ist auf der Map um 90 Grad verdreht.',
      'Die kleinen Tuerbereiche von ROOM 1110 werden schon als besucht angezeigt, obwohl ich dort noch gar nicht drin war.',
      'Die Fahrstuhl-Etagen werden als besucht angezeigt, obwohl ich auf den verschiedenen Etagen noch nicht im Fahrstuhl war.',
    ],
    hinweise: 'Karte: include/re15_map_zones.h + engine/src/re15_map_zones.c/re15_map_zeilen.h (Generator tools/**/gen_map_zones.py — Generator liest sich selbst, Memory), Tuermarken, Besucht-Latches (re15_map_owned.c, Zone-Besucht beim Raumaufbau — Spur K r35 Hinweis: JEDER Raumaufbau setzt das Zonen-Bit; Spur G r35 karte_fahrstuhl_1080.c), Messhaken RE15_MAP_SHOT_PAGE / RE15_INV_MAP_SHOT / RE15_KARTENZEILE / RE15_MAP_EBENE / FB_SHOT; Memories reai-v2-karte-*, Schablonen-Zonen, Massstabstabelle. Je Punkt: messen (Framedump der Karte RE15_WINDOW_SCALE=1, Spielerposition -> Zeile), RE2-Kartenlogik als Vorbild belegen (Treppen-Glyph je Blatt, Marker = Spielerposition auf dem Blatt, Besucht nur wo der Spieler wirklich war: RE2 setzt das Raum-Besucht-Bit beim Betreten des RAUMS — Teilbereiche/Fahrstuhl-Etagen: Latch an die Position/Etage binden), korrigieren; Pins je Punkt. Spur N (karten-items) arbeitet parallel an der Kartenfreischaltung (Etage kartiert) — nur kleine Haken in gemeinsamen Karten-Dateien, Konflikte im Dossier nennen.' },

  { id: 'N', thema: 'kartenitems', baum: 'r36_kartenitems',
    punkte: [
      'Extrahiere Karte01 oder Karte02 der RE2-Weltmodelle (extracted_re2_items/uebersicht.html, Lageplan-Karten) und haenge sie beispielhaft an die Stelle von map3F.bmp; schalte damit die Map fuer Etage 3F frei, wie das Kartensystem in Resident Evil 2 funktioniert. Auch mit dem Text.',
      'Bei Room 10D0 die Map fuer 2F da platzieren, wo map2F.bmp steht. Bei Room 1030 die Map fuer 1F da, wo map1F.bmp steht. Map B2 in Room 1200 bei mapB2.bmp. Map B1 in Room 11D0 bei mapB1.bmp.',
    ],
    hinweise: 'Marken: analysis/befunde_runde36/marken/map{3F,2F,1F,B2,B1}.png (+ Bbox der roten Marke in AUFTRAG.md: 3F = Pinnwand ROOM1150 rechts neben der Tuer; 2F = unterer Teil des WANTED-Aushangs ROOM10D0; 1F = Wand zwischen den Aushaengen ROOM1030 Wartebereich; B1 = Wand rechts der Metalltuer unter dem Schild ROOM11D0; B2 = Glasvitrine links ROOM1200). RE2-Modell: extracted_re2_items/modelle/karte01_99dae133 / karte02_5cdae635 (MD1 + Textur, Herkunft in extracted_re2_items/lageplaene.csv) -> als Port-Prop einbacken (Muster Spur F r35: engine/src/gen/r35_schrot_prop.inc + inhalt_r35.c Item-AOT/Pickup, Weltposition per Rueckprojektion aus der Kamera, Memory reai-v2-modellmass-aus-kamera), an der Wand/Vitrine haengend/stehend. RE2-Kartensystem: wie registriert RE2 eine aufgenommene Karte (Item-Typ "Map", FUN_... im Pickup, Karten-Bits; Kartenanzeige zeichnet kartierte Etage komplett, unbesuchte Raeume in anderer Farbe — RE2 FUN_8004b... Kartenzeichner, ghidra_re2_Leon.txt) belegen; Port: Bank-9-Bits 87..91 = Etage kartiert; Kartenanzeige (re15_inv_screen / map_*): kartierte Etage komplett zeichnen (unbesucht in RE2-Farbe), Text beim Aufnehmen (RE2-Wortlaut der Karten-Meldung / Item-Name "Map (3F)" — belegen), Leon UND Elza (beide Varianten). Pins: Aufnehmen -> Bit -> Karte zeigt alle Raeume der Etage; Framedump je Etage. Spur M arbeitet parallel an Zonen/Tuermarken/Besucht — nur kleine Haken in gemeinsamen Karten-Dateien.' },

  { id: 'O', thema: 'pri', baum: 'r36_pri',
    punkte: [
      'Neues PRI in ROOM 10F0 (02_02.png), weil mit der neuen Cutscene auffaellt, wie Marvin nicht ueberdeckt wird von der Absperrung.',
      'Einzelner Marker fuer ROOM 1100: hier wird Leon mit einer Halbtransparenz ueberdeckt mit Boden. Das ist falsch. (Marker-Datei nicht gefunden — selbst messen, alle Cuts)',
    ],
    hinweise: 'Masken-Pipeline: re15_port/tools/maske/ (maske_aus_png.py, pri_hashes.py — Hash-Liste der Nutzer-PRIs, abnahme.py, pruef_verdeckung.py, tiefenkalib.py), Nutzer-PRIs unter pri/STAGE1/<raum>/<cut>[_<n>].png, Ausgabe shared_assets/PSX/MASKS/ROOM....MSK/_PRIxx.TIM (Erzeugnis, kein Original-Patch — pruefen, wie r35/r19 die Masken erzeugt und eingebunden haben; Memories reai-v2-vordergrund-masken, reai-v2-tiefe-aus-kollision, reai-v2-runde19-pri-marken, ⛔ flache Maske verdeckt nichts, Bbox ist nicht die Kunst). 10F0 Cut 2: pri/STAGE1/10F0/02_02.png (Absperrung) durch die Pipeline, Tiefe aus der Kollision/Sehstrahl, Hash-Liste nachziehen, messen: Marvin an seiner Szenen-Position (cut_10f0.c, Cut 2) wird von der Absperrung verdeckt (Framedump RE15_WINDOW_SCALE=1, Pixel im Absperrungsbereich). ROOM1100: alle Cuts abfahren (RE15_FORCE_CUT / Positionen), Masken je Cut listen (RE15_PRI_LOG), die halbtransparente Bodenmaske finden (Alpha/ABE-Bit? Tiefe?), Ursache belegen (Masken-Datei + Eintrag), beheben (Tiefe/Transparenz), Pin.' },

  { id: 'Q', thema: 'schluessel', baum: 'r36_schluessel',
    punkte: [
      'Bei Leon soll die Tuer zum Parking Lot ROOM 11C0 verschlossen bleiben, bis ein Parking-Lot-Schluessel im Communication Room ROOM 10F0 gefunden wurde. Waehle einen Schluessel aus den Resident-Evil-2-Items, der am ehesten den optischen Eindruck eines Parking-Lot-Schluessels macht.',
      'Bei Elza soll der Prison Key nicht in Room 1011 sein wie aktuell, sondern da, wo auch der Parking-Lot-Schluessel in Leons Szenario ist. Auch hier ein gutes Modell aus Resident Evil 2 fuer einen Prison Key aussuchen.',
    ],
    hinweise: 'RE2-Item-Modelle: extracted_re2_items/ (87 Modelle, katalog.csv, kontaktbogen.png) — alle Schluessel-Modelle rendern/vergleichen, Wahl begruenden (Parkplatz: Autoschluessel-artig; Gefaengnis: Zellen-/Vorhaengeschloss-Schluessel-artig), als Port-Prop einbacken (Muster Spur F r35 inhalt_r35.c + gen/*.inc). Item-Id: RE1.5-Item-Tabelle (inventory_common.c Namen/Icons; gibt es freie Ids, Icons fuer Schluessel? Original-Schluessel-Items und ihr Tuer-Mechanismus: AOT-Tuer mit Key-Item-Feld und Lock-Bit — Opcode-Format RE15_SCD_OPCODES_REFERENCE.md/rdt-analysis, Beleg @0x... im Tuer-Handler FUN_8001d600/Door_aot_set), Prison Key existiert bereits (Elza, ROOM1011 — Id, Spawn-Offset in ROOM1011.RDT; dort entfernen = Port-Override ohne RDT-Patch, Muster zombie_abstand/room fixes). Position: EINE Stelle in ROOM10F0/10F1 (Kommunikationsraum; z.B. Tisch/Konsole — per Rueckprojektion aus einem Cut, Bild im Dossier), gleiche Koordinaten fuer beide Schluessel. Tuer 11B0 -> 11C0 (Leon): ohne (9,82) verschlossen mit Text (ID 28/29, Wortlaut z.B. "It\'s locked. I need a key for the parking lot." — PORT-WAHL kennzeichnen) oder ueber den Original-Key-Door-Mechanismus (Key-Item am AOT: "Use the Parking Lot Key?" — bevorzugt, wenn der Port ihn hat; belegen). Spur J schliesst seit 14:00 NUR noch die Werkstatt-Tuer ab (Bit 84), NICHT die Parkplatz-Tuer — Q retypt die Parkplatz-Tuer allein. Story-Hintergrund fuer das Dossier (AUFTRAG.md Nachtrag): Kendo/John sitzt wegen des sabotierten Funkraums in Untersuchungshaft, die Beamten entzogen Roy die Befugnis, deshalb liegt der Prison Key im Communications Room; KEIN leerer Haken/Text fuer Leon. Pins: Leon ohne Schluessel -> Text, mit Schluessel -> Tuer geht; Elza: Prison Key liegt in 10F1, nicht in 1011; Icons/Inventar/Statusschirm fuer beide.' },
  { id: 'P', thema: 'story', baum: 'r36_story',
    punkte: [
      'Ja, wenn es mehr sinn macht, das Roy erst da liegt, wenn der Strom eingeschaltet wurde, dann machen wir das so. (Leon ROOM1020: Roys Leiche + ihr Anklick-Text erst NACH dem Einschalten des Stroms)',
      'bei Leon ein Text an Elzas Zelle ("The cell door is open...") - die leere Zelle als Spur. - kannst du machen. (Leon ROOM1210: Anklick-Text an der Zelle, in der bei Elza (ROOM1211) Kendo/John sitzt)',
      'ROOM1090. Elzas 1091 ist bytegleich Leons Zustand nach der Rettung - Leon war zuerst dort. Hinnehmen und eine Spur ergaenzen ("burnt-out shed") << kannst du machen (Elza ROOM1091: Anklick-Text am ausgebrannten Schuppen; KEIN leerer Feuerloescher)',
      'Zum Textwiderspruch - ja, da hat der vorherige Modder auf jeden Fall quatsch gebaut. Die duerfen sich niemals begegnet sein, das heisst - alle Textstellen, die darauf hindeuten das sie sich begegnet waeren, muessen wir umschreiben. (Sweep ueber ALLE Texte beider Szenarien; bekannt: ROOM30C1 msg 2-6, ROOM6030 msg 1-5, ROOM6031 msg 0-2, ROOM3060 msg 1-8)',
      'Prison Key / Funkraum: "kann genauso gut sein, das sie es versucht hat, aber es eben nicht ging, weil die Kommunikationsanlage ja zerstoert war. In die Richtung wuerde ich auch gehen." (Pruefen, dass die Funkraum-Texte 10F0/10F1 das hergeben — msg 0 "It\'s out of commission" — und dass KEIN Text eine Reihenfolge Elza-vor-Leon behauptet; kein leerer Haken fuer Leon; nur Dossier, ggf. Textkorrektur)',
    ],
    hinweise: 'Vorarbeit: analysis/story_leon_elza/ (A_zensus.md, texte_leon.md, texte_elza.md = ALLE Texte beider Szenarien mit RDT-Offsets, zeitleiste.md, D_gegenpruefung.md: Roy-Leiche 1020 @0x01DEE = Elzas 1021 @0x01E40; Strom-Flag (4,0xF3)/(4,0x06) — belegen, welches Bit der Port beim Generator setzt (Generator-/Strom-Module r34: grep -rn in engine/src nach generator/strom), und wie der Port Spawn-/Objekt-Records je Flag unterdrueckt (Muster zombie_abstand_r35.c / room fixes / inhalt_r35.c: Override beim Raumaufbau ohne RDT-Patch). P1: Leichen-Record von Roy in ROOM1020.RDT identifizieren (Typ/Offset, rdt-analysis), beim Raumaufbau nur mit Strom-Bit setzen; msg 0 "It\'s the RPD officer, Roy..." haengt an einem AOT — ohne Leiche auch kein Text (AOT ebenfalls gaten oder Ersatztext PORT-WAHL). P2: ROOM1211 msg 10 ("It\'s locked. It seems that there is someone inside...") gehoert zu einem Text-AOT an der Zellentuer — Position/Groesse aus der 1211-AOT-Tabelle lesen, in Leons ROOM1210 (0 Nachrichten) denselben AOT als Text-AOT mit neuem Port-Text ID 20 installieren (Wortlaut PORT-WAHL, z.B. "The cell door is open. Whoever was locked up here is gone." — im Dossier als Vorschlag kennzeichnen). P3: Leons ROOM1090 Feuer-/Loesch-AOT (Spur M r35 glas1090 / Feuerloescher-Module, extracted_re2_feuerloescher) lokalisieren; in ROOM1091 (0 Nachrichten) an derselben Stelle Text-AOT ID 20 ("The shed is burnt out. Someone must have put the fire out." PORT-WAHL). P4 Sweep: texte_leon.md nach "Elza", texte_elza.md nach "Leon", beide nach "separated|met|each other|rookie|together|the other" und nach Szenen-Subs, in denen beide als Sprecher vorkommen; je Treffer entscheiden (Begegnung/Bekanntschaft impliziert?) und alt->neu-Tabelle schreiben; Umsetzung: re15_msg_install_text(id, bytes, len) + re15_msg_install_durations NACH re15_msg_load_room_block (Installer-Zeile je Raum am Ende des Blocks in scd_room_setup.c, Kommentar Runde 36 Spur P; Bytes im Original-Format: Sprecher-/Farbcodes 04 xx / 05 xx und Zeilenumbruch wie die Vorlage — msg_common.c re15_msg_decode_text lesen), Szenenfunktion erhalten (wer startet den Zug, wer wartet wo), Dritte (Marvin/Sherry/John) duerfen den anderen nur als Fremden erwaehnen. ROOM3060 steht im Leon-Zensus mit Elza/Sherry/John-Zeilen: belegen, welches Szenario 3060 laedt (gibt es 3061? Raumlader-Variante), bevor du umschreibst. Pins: Raum laden (Leon bzw. Elza, RE15_PSELECT_AUTO) + RE15_MSG_LOG/Message-Tabelle pruefen: neue Texte stehen, alte Wortlaute kommen nicht mehr vor (Suche ueber den dekodierten Text aller betroffenen Raeume); P1: 1020 ohne Strom-Bit -> kein Roy-Record/kein Text, mit Bit -> Record + msg 0; P2/P3 Text-AOT ausloest (RE15_FIRE_AOT oder RE15_PLAYER_POS + RE15_PRESS). Fuer den Nutzer: alt->neu-Tabelle aller umgeschriebenen Zeilen zur Freigabe.' },
  { id: 'R', thema: 'dachkiste', baum: 'r36_dachkiste', basis: '99398dc7',
    punkte: [
      'Befund der Spur H (analysis/befunde_runde36/H_ziel1190.md Abschnitte 2.2/2.4 und OFFEN): Die Kiste obj 1 auf dem DACH von ROOM1090 ist die einzige Stufe zum Tor nach ROOM1100 (Tuer-Record verlangt Band 6 = Kistenoberkante y -10800; aus ROOM1100 fuehrt keine Tuer zurueck). Seit Spur A (Objekt-Fall FUN_8002c444 in engine/src/objfall_r36.c, bereits integriert) kann die Kiste vom Dach fallen - dann waere das Tor unerreichbar (Softlock). Der Nutzer-Wortlaut der Spur A lautet "Das macht ein Fortschritt im Spiel unmoeglich" - ein vom Port erzeugter Softlock ist ausgeschlossen. Aufgabe: (1) An der exe messen, ob Leon (und Elza in ROOM1091) die Dach-Kiste im echten Spiel bis zum Fall-Ausloeser schieben kann (echter Weg: aus ROOM10F0 ueber den Tritt, Tuer nach 1090, Kiste in jede moegliche Richtung schieben; RE15_CLIMB_TRACE, Objekt-Log). (2) Am ORIGINAL messen (DuckStation, Savestate C:/workspace/git/reAi_v2/stage_saves/room1090_orig.sav aus dem HAUPTBAUM lesen; Skills re15-room-capture, re15-savestate-ghidra, GDB-Muster F_zombieki.md Nachbesserung 4 gdb_affen.py / H_raeume r35): laesst das Original die Kiste bis zur Absprungzelle schieben (Schubpruefung FUN_8003b558 je Schritt @0x8003b634-660, Objekt-Pass @0x8002c0fc-c14c), oder blockt es vorher? (3) Blockt das Original -> der Port muss an derselben Stelle und mit demselben Mechanismus blocken (Beleg @0x..., Fix in objfall_r36.c bzw. im Schub, Pin). Laesst das Original die Kiste fallen -> byte-true belassen, den moeglichen Softlock im Dossier und unter "Fuer den Nutzer" als Grenze benennen, Pin, der den Original-Weg festhaelt. (4) Dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6 (laut Spur A ebenfalls neu fallfaehig): haengt dort ein Fortschritt an der Kiste?',
    ],
    hinweise: 'Dein Baum basiert NICHT auf master, sondern auf r36/integration (Commit 99398dc7 = elf integrierte Spuren + G/H/M); alle Diff-/Log-Bezuege in deinen Befehlen gegen 99398dc7 (git -C <baum> diff 99398dc7, log 99398dc7..HEAD). Belege: A_klettern.md Abschnitt "Nachbesserung 3" (RE B1-B7 zu FUN_8002c444/FUN_8002d3d8/FUN_8002dacc, Objekt-Pass, Freiraum-Zensus FUN_8003b558 mit "8 erreichbare Absprungzellen" fuer ROOM1090 - nur in der Sonde, nie an der exe), objfall_r36.c + include/re15_objfall_r36.h + tests/unit/probe_r36_objfall.c (Sonde "pin", Lagen 5020/20A0/10F0), H_ziel1190.md Abschnitt 2 (Kiste 0 Hof ohne Zweck, Kiste 1 Dach = Stufe; Lauf k1090_tor3/tor5 mit RE15_FORCE_CUT=2, DOOR FIRE slot=1 -> ROOM1100). Original-Schubmechanik: climb_common.c / re15_prop_push_tick (Substate 8, FUN_8002bd44 Objekt-Pass), re15_collision_push_prop (@0x8002bed4). Savestates des Hauptbaums (C:/workspace/git/reAi_v2/stage_saves/*.sav) darfst du LESEN; DuckStation-Settings (EnableGDBServer) nach dem Lauf zuruecksetzen. Tests: tests/unit/probes/r36_dachkiste.cmake (GLOB) + eigene test_r36_dachkiste*.c / integration-Test; gezielt ctest -R "r36_dachkiste|r36_objfall|r36_klettern|1090|1100|prop|klettern|objfall" --timeout 900; keine volle Suite. Dossier analysis/befunde_runde36/R_dachkiste.md.' },
]

const REGELN = (s) => `
## Arbeitsbaum und Grenzen
- Dein Baum: ${ROOT}/.claude/worktrees/${s.baum} (Zweig r36/${s.thema}, Basis master ${BASIS}). Zuerst: git -C "${ROOT}/.claude/worktrees/${s.baum}" status --porcelain (leer) und git log --oneline -1 (= ${BASIS} oder ein Merge von master in den Zweig; Fortsetzungs-Baeume tragen zusaetzlich die Vorgaenger-Commits). AUFTRAG.md traegt seit ${BASIS} einen Nachtrag 14:00 mit Story-Entscheidungen des Nutzers — lesen.
- ALLE Lese-/Schreibzugriffe auf Projektdateien und ALLE git-Befehle NUR in diesem Baum (absolute Pfade mit dem Praefix ${ROOT}/.claude/worktrees/${s.baum}/ bzw. git -C "<baum>"; in Bash jeden Befehl mit cd "<baum>" beginnen). NIE Dateien unter ${ROOT}/ ausserhalb deines Baums aendern. Aus dem Hauptbaum nur LESEN, was der Baum nicht traegt: ${ROOT}/info/Resident_Evil_und_Playstation_Information/ und ${ROOT}/.agent_refs/.
- Pflichtlektuere ZUERST, vollstaendig: <baum>/analysis/befunde_runde36/AUFTRAG.md und VERTRAG.md (Zuteilung VERBINDLICH; nimm nur, was Spur ${s.id} zugeteilt ist). Vorrunde: <baum>/analysis/befunde_runde35/<SPUR>_*.md (Dossiers + Abnahmeberichte der Module, die du anfasst).
- Dossier <baum>/analysis/befunde_runde36/${s.id}_${s.thema}.md als ERSTES anlegen und FORTLAUFEND schreiben; nach JEDEM abgeschlossenen Schritt committen (wip(r36-${s.thema}): ... ausdruecklich erwuenscht, auch VOR Bau/Suite). Das Sitzungslimit beendet alle Agenten gleichzeitig — was nicht im Commit steht, ist verloren. Nur eigene Pfade adden (NIE -A), Rohbytes/Backticks per git commit -F <datei>.

## RE-Disziplin (CLAUDE.md STOP-GATE, Kurzfassung)
- Je Punkt: MESSEN/REPRODUZIEREN am gebauten Stand -> ORIGINAL disassemblieren (RE1.5; wo RE1.5 unfertig ist, RE2 Retail = Ziel, Beleg aus RE2; Sound ist RE2 — <baum>/RE2_SOUND_LISTE.md zuerst) -> Adresse/Bytes ins Dossier -> DANN Code -> VERIFIZIEREN durch erneutes Messen. Jede Konstante @0x... (oder Byte-Offset) im Code UND in der Commit-Message; Nutzer-Vorgaben als NUTZER-VORGABE / PORT-WAHL, ihre FORM belegt. Kein "deferred/tunable/interim/Env-Schalter" als Abschluss. Regressionen ZUERST bisecten (v0.8.21 9362dd7e / v0.8.22 87cc8575 / v0.8.23 3ec5b1f8, dann die Runde-35-Merges), verursachenden Commit nennen; der Fix muss den Befund ERKLAEREN. "Sieht richtig aus" ist kein Beleg; Nutzer-Beobachtung schlaegt Code-Hypothese.
- Quellen im Baum: RE_15_Quellcode_Overlays/STAGE1_full/ (0x8010xxxx; STAGE1/ ohne _full ist +0x800 verschoben), RE_15_Quellcode_V2/, ghidra1_V2.txt, RE15_FUN_CATALOG.md, RE15_KNOWLEDGE.md, STAGE1_FLAG_MAP.md, RE15_SCD_OPCODES_REFERENCE.md, RE2_SOUND_LISTE.md; RE2: info/re2leon/, ghidra_re2_Leon.txt, RE2_Quellcode_V2/, RE2_Quellcode_Overlays/. Werkzeuge .claude/skills/re15-psx-disasm/scripts/re15_disasm.py / re2_disasm.py; Skills .claude/skills/<name>/SKILL.md (rdt-analysis, scd-disassembly, ghidra-mapping, re15-psx-disasm, re15-room-probe, re15-room-capture, re15-port-visual-verify, re15-enemy-ai-re, re15-weapon-render, re15-parity-verify, re15-pc-render-order, re15-savestate-ghidra, re15-pcsx-watchpoint) ZUERST lesen. DuckStation-GDB fuer RE2-/RE1.5-Messungen: Muster in analysis/befunde_runde35/H_raeume.md und J_affen.md (EnableGDBServer in %LOCALAPPDATA%/DuckStation/settings.ini, danach zurueck). Memory-Notizen (reai-v2-...) unter C:/Users/mjoedicke/.claude/projects/c--workspace-git-reAi-v2/memory/<name>.md (nur lesen).

## Bauen, Messen, Testen
- NUR so bauen: cd "<baum>" && bash re15_port/tools/local_build.sh configure (einmal und nach jeder NEUEN .c-Datei), danach ... build (test/all = volle Suite: im Spurbaum VERBOTEN, s.u.). NIE cmake --build aus Git-Bash. exe <baum>/re15_port/build/platform/pc/re15_pc.exe; befund.log/debug.log daneben. ⛔ KEINE volle Suite (local_build.sh test/all) im Spurbaum — gemessen 2026-10-10: fuenf parallele Suiten brauchten je ~80 min und lieferten Dutzende Last-Timeouts der Fensterhaken (unbrauchbar). Stattdessen GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = eigene Tests (r36_${s.thema}) + die Testgruppen JEDES Moduls/Raums, das dein Diff beruehrt (git -C "<baum>" diff ${BASE(s)} --name-only lesen; vorhandene Testnamen: ctest --test-dir ... -N | grep -i <modul/raum>). Die ctest-Summenzeile ("100% tests passed, 0 tests failed out of N") woertlich mit dem -R-Ausdruck ins Dossier. Die volle Suite faehrt der Orchestrator EINMAL im Integrationsbaum. Fenster-Haken flattern unter Last (boot_bg_pin, dark_start_pin, relatch_pin, save_counter_pin, weste_load_pin, r30_sicherung_*, r30_titel_puls, r34n_e_dokumente_bild; exe-Starts brechen selten mit exit 1 nach 0,1 s ab): einzeln 2-3x nachfahren (ctest --test-dir "<baum>/re15_port/build" -R "^<name>$"), nicht als Regression werten. NIE taskkill /IM re15_pc.exe (nur eigene PID); Messkopie der exe unter eigenem Namen neben der exe (Muster tests/integration/test_r34n_b_cursor.cmake). Nie zwei Builds gleichzeitig im eigenen build/ (ein zweites Bauverzeichnis fuer Bisects: RE15_BUILD_DIR=<baum>/re15_port/build_bisect).
- Harness: getenv("RE15_...") in platform/pc/main.c + engine/src (RE15_DEBUG_JUMP, RE15_GOTO_ROOM, RE15_FORCE_EVENT, RE15_FORCE_CUT, RE15_PLAYER_POS, RE15_EQUIP, RE15_GIVE, RE15_PRESS, RE15_PAD_AT, RE15_INPUT_SCRIPT, RE15_SET_FLAG(_AT), RE15_FIRE_AOT, RE15_FORCE_EXPLOSION, RE15_GRANATE_LOG, RE15_STATE_LOG, RE15_EVT_TRACE, RE15_SCD_TRACE, RE15_MSG_LOG, RE15_ENEMY_DBG, RE15_ANIM_TRACE, RE15_NECK_LOG, RE15_FX_LOG, RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC (Ton-Mitschnitt ohne Audiogeraet), RE15_FRAMEDUMP, RE15_EXIT_AT, RE15_KILL_AT, RE15_CONTINUE_TEST + RE15_CARD_AUTO, RE15_PSELECT_AUTO(_SWITCH) fuer Elza, RE15_CLIMB_TRACE, RE15_TUER_TEST/_SEITE, RE15_MAP_SHOT_PAGE, RE15_PRI_LOG) — Semantik im Code nachlesen. Bilder NUR per RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1 (gdigrab liefert weisse Bilder; Framedump schreibt Fensterskalierung). Story-/Raum-Punkte ueber den ECHTEN Weg abnehmen (Tuer betreten, Spielstand + CONTINUE), nicht nur per Sprung; Elza-Punkte mit Elza (RE15_PSELECT_AUTO).
- Tests: NEUE Sonden/Pins in re15_port/tests/unit/probes/r36_${s.thema}.cmake (GLOB; Muster probes/r35_*.cmake) + tests/unit/test_r36_${s.thema}*.c / tests/integration/test_r36_${s.thema}*.cmake (registriert NUR aus deiner probes-Datei). NICHT tests/unit/CMakeLists.txt und NICHT tests/integration/CMakeLists.txt. Mindestens ein Test je Nutzer-Punkt, der die MECHANIK misst. Arbeitsordner eines Tests nie gleich dem Target-Namen (Linux ohne .exe).
- Dateien: neue Logik in NEUEN Dateien engine/src/<thema>_r36.c + include/re15_<thema>_r36.h; gemeinsame Dateien nur mit kleinen Haken (1-5 Zeilen) an klar benannten Stellen, Installer-Zeile am ENDE des Blocks in scd_room_setup.c mit Kommentar "Runde 36 Spur ${s.id}". KEIN Patch an shared_assets/PSX/*; neue Daten gen/*.inc oder shared_assets/RE2/ (im Dossier nennen). release/, platform/android/, RELEASE_NOTES.md: TABU.
- Token sparen: grosse Dateien nie ganz lesen (grep -n + Bereich), Logs gefiltert, Bau-/Suite-Ausgaben in Datei umleiten und nur Ende/Fehler lesen, keine Messung wiederholen, nur urteilstragende Bilder ansehen.

## Abschluss
- Dossier mit Abschnitten: Messung vorher (bei Regressionen: Bisect-Tabelle) / RE-Belege (Adressen, Bytes) / Umsetzung (Dateien, Konstanten) / Messung nachher / Tests (Namen, Ergebnis) / OFFEN / Fuer den Nutzer (neue Assets fuer das Paket-Gate, Sprachdateien, Bedienhinweise, Entscheidungsfragen). Letzter Commit fix/feat(r36-${s.thema}): ... Gezielte Tests gruen: ctest-Summenzeile + -R-Ausdruck woertlich zitieren (KEINE volle Suite). Antwort = StructuredOutput.
`

const bauPrompt = (s) => `Du bist Spur ${s.id} "${s.thema}" der Runde 36 (2026-10-10) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Du ermittelst, belegst, baust und testest ALLE Punkte dieser Spur im eigenen Arbeitsbaum. Ein anderer Agent nimmt danach unabhaengig gegen den Wortlaut des Nutzers ab — was du nicht gemessen und belegt hast, faellt dort durch.
${STAND[s.id] ? `
## ⛔ FORTSETZUNG — dein Vorgaenger wurde vom Sitzungslimit beendet
${STAND[s.id]}
Lies ZUERST das Dossier, git -C "<baum>" log --stat ${BASE(s)}..HEAD und den Code-Diff gegen master. Wiederhole KEINE vorliegende Messung und KEINEN vorliegenden Beleg. Stelle je Nutzer-Punkt fest, was noch fehlt, und schliesse genau das ab; pruefe kritisch, ob der Stand den WORTLAUT erfuellt.
` : ''}
## Deine Punkte (Wortlaut des Nutzers, AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Einstiegspunkte (Stand 2026-10-10 — Zeilennummern und Behauptungen SELBST verifizieren)
${s.hinweise}
${REGELN(s)}
Arbeite jeden Punkt vollstaendig ab (Dossier je Punkt: Messung vorher / Beleg / Umsetzung / Messung nachher). Sackgasse: Weg wechseln (Xrefs, Caller/Callee, RE2, Savestate-RAM, GDB, Laufzeit-Log), nicht aufgeben, nicht raten. Am Ende: gezielte Tests gruen (keine volle Suite), alles committet, Dossier vollstaendig.`

const nachPrompt = (s, abn, runde, vorher) => `Du bist Spur ${s.id} "${s.thema}" der Runde 36 (2026-10-10) im Projekt reAi_v2 — NACHBESSERUNG ${runde} nach einer unabhaengigen Abnahme. Der Bau-Agent vor dir hat im selben Baum gearbeitet; sein Stand ist committet (lies git -C "<baum>" log --oneline ${BASE(s)}..HEAD, das Dossier und den Abnahmebericht <baum>/analysis/befunde_runde36/${s.id}_abnahme_${runde - 1}.md).

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
Punkte, die die Abnahme als erfuellt bewertet hat, NICHT verschlechtern (ihre Riegel gruen halten). Schreibe im Dossier einen Abschnitt "Nachbesserung ${runde}" (je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher). Baue neu, gezielte Tests gruen (keine volle Suite), committe (fix(r36-${s.thema}): Nachbesserung ${runde} ...).`

const abnahmePrompt = (s, bau, runde) => `Du bist die UNABHAENGIGE ABNAHME ${runde} fuer Spur ${s.id} "${s.thema}" der Runde 36 (2026-10-10) im Projekt reAi_v2 (RE1.5-PSX -> PC-Port). Ein Bau-Agent behauptet, die Punkte des Nutzers umgesetzt zu haben. Du glaubst ihm NICHTS, was du nicht selbst am gebauten Stand gemessen hast. Der Nutzer weist jede Aenderung ohne Disasm-Beleg zurueck — du bist seine Instanz.

## Die Punkte des Nutzers (Wortlaut; AUFTRAG.md ist massgeblich)
${s.punkte.map((p, i) => `${i + 1}. ${p}`).join('\n')}

## Behauptung des Bau-Agenten
Status: ${bau ? bau.status : 'KEIN ERGEBNIS (Agent abgebrochen) — pruefe Baum und Dossier trotzdem'}
${bau ? bau.zusammenfassung : ''}
Commits: ${bau && bau.commits ? bau.commits.join(', ') : '?'}
Suite laut Bau-Agent: ${bau ? bau.suite : '?'}
Offen laut Bau-Agent: ${bau && bau.offene_punkte && bau.offene_punkte.length ? bau.offene_punkte.join(' | ') : '(nichts)'}

## Baum
${ROOT}/.claude/worktrees/${s.baum} (Zweig r36/${s.thema}, Basis master ${BASIS}). ALLE Zugriffe und git-Befehle nur dort (absolute Pfade, git -C "<baum>", Bash mit cd "<baum>"). Nichts ausserhalb des Baums aendern. Aus dem Hauptbaum ${ROOT}/ nur lesen (info/Resident_Evil_und_Playstation_Information/, .agent_refs/).

## Vorgehen
1. Lies <baum>/analysis/befunde_runde36/AUFTRAG.md, VERTRAG.md, das Dossier ${s.id}_${s.thema}.md und: git -C "<baum>" log --oneline ${BASE(s)}..HEAD; git -C "<baum>" diff ${BASE(s)} --stat; git -C "<baum>" diff ${BASE(s)} -- re15_port (den Code-Diff WIRKLICH lesen).
2. Baue: cd "<baum>" && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build (nie cmake --build aus Git-Bash). KEINE volle Suite (test/all) im Spurbaum — die faehrt der Orchestrator einmal im Integrationsbaum. Fahre GEZIELT: ctest --test-dir "<baum>/re15_port/build" -R "<regex>" --timeout 600 --output-on-failure, Regex = Tests der Spur (r36_${s.thema}) + die Testgruppen aller Module/Raeume, die der Diff beruehrt (git diff ${BASE(s)} --name-only; ctest -N | grep -i). Rote Fenster-Haken unter Last 2-3x einzeln nachfahren, bevor du sie wertest. NIE taskkill /IM re15_pc.exe; Messkopie der exe unter eigenem Namen (Muster tests/integration/test_r34n_b_cursor.cmake).
3. MISS jeden Nutzer-Punkt selbst an der exe (<baum>/re15_port/build/platform/pc/re15_pc.exe; Haken getenv("RE15_...") — Semantik im Code nachlesen; Spielstand + CONTINUE, RE15_DEBUG_JUMP, RE15_FORCE_EVENT, RE15_PLAYER_POS, RE15_PRESS/RE15_PAD_AT/RE15_INPUT_SCRIPT, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, Logs RE15_STATE_LOG/RE15_EVT_TRACE/RE15_ENEMY_DBG/RE15_FX_LOG/RE15_SE_DEBUG/RE15_AUDIO_CAP_SYNC usw.; Elza-Punkte mit RE15_PSELECT_AUTO). Story-/Raum-Punkte ueber den ECHTEN Weg (Tuer betreten), nicht nur per Sprung. Bei Regressionen: Gegenprobe mit der v0.8.21-exe, ob der genannte verursachende Commit den Befund wirklich erklaert. Urteil je Punkt gegen den WORTLAUT: erfuellt / teilweise / nicht_erfuellt / nicht_pruefbar (mit Grund) — mit Messprotokoll (Befehl, Log-Zeilen, Bild-Pfad).
4. RE-Gate: Jede verhaltensrelevante Konstante im Diff hat @0x... (oder Byte-Offset) ODER ist als NUTZER-VORGABE / PORT-WAHL gekennzeichnet; stichprobenartig 3 Adressen SELBST disassemblieren (re15_disasm.py / re2_disasm.py / ghidra*.txt) und pruefen, dass die zitierte Stelle das Behauptete enthaelt (Sprungziel disassemblieren, richtige Binaerdatei). Suche im Diff nach "deferred", "tunable", "interim", "for now", "faithful", "plausibel", "TODO", Env-Schaltern als Abschluss. Pruefe, ob der Fix den gemessenen Befund ERKLAERT.
5. Vertrag: nur zugeteilte Bank-9-Bits/Nachrichten-IDs/Slots/Ereignisse (VERTRAG.md); erlaubte Pfade (git diff ${BASE(s)} --name-only: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists); gemeinsame Dateien nur kleine Haken; Tests je Punkt vorhanden und messend (ctest --test-dir "<baum>/re15_port/build" -R "r36_${s.thema}" laufen lassen). Pruefe auch Nebenwirkungen auf das, was die Spur beruehrt (z.B. andere Raeume, beide Szenarien).
6. Schreibe den Bericht <baum>/analysis/befunde_runde36/${s.id}_abnahme_${runde}.md (je Punkt Messprotokoll, Urteil, Beleg; Gates; Maengel nummeriert, konkret, nachpruefbar) und committe NUR diese Datei: git -C "<baum>" add analysis/befunde_runde36/${s.id}_abnahme_${runde}.md && git -C "<baum>" commit -m "doc(r36-${s.thema}): Abnahme ${runde}". Du aenderst KEINEN Code. Schreibe den Bericht FORTLAUFEND (nach jedem gemessenen Punkt speichern) — das Sitzungslimit kann dich jederzeit beenden.
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