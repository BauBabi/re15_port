# RE1.5 Room-Correctness Backlog — falsch „byte-true" portierte Mechaniken

Vom Nutzer beim GUI-Test gefunden (2026-07-02). Regel: **jeder Fix byte-true belegt** (Overlay/Ghidra/
Savestate-Adresse oder Datei-Offset), Mechanismus beweisen — „sieht richtig aus" zählt nicht. Einer zu
100% fertig, bevor der nächste dran ist. Status: ⬜ offen / 🔎 in RE / 🔧 fix / ✅ verifiziert.

---

## 1. ROOM 1240 — Intro-Text: Volltext statt Typewriter + Balken über Text  ✅ (1A [FL-observed], 1B [BT])
- **Bug A (KORRIGIERT — Richtung war verkehrt):** Das Original zeigt das 1240-Pre-Intro als **VOLLTEXT**
  (kompletter Block), der Port **typte** es. Ursache: der Narrator (sub02, 6× `Message_on(0x2B)` msg 0..5)
  lief durch den Typewriter-FSM, weil `re15_room_has_voice` nur 0x1170 kannte → 1240 in den else-Zweig.
  **Fix (`scd_vm.c` op_message_on):** neues `re15_room_full_text() = {0x1170,0x1240}` → Volltext-Pfad
  (`msg_show`/legacy `message_display_frames`); Voice bleibt auf {0x1170} (1240 hat keine VOICE##.VAG).
  **[FL-observed]:** der statisch sichtbare EXE-Printer (0x2B→LAB_800404f4 hardcodet 0x300→FUN_80027e68
  setzt DAT_800b8521=0 bedingungslos→FUN_80028134) ist **byte-true Typewriter-only** — Volltext kommt aus
  einem nicht in ghidra1_V2.txt sichtbaren Weg (**DEBUG.BIN @0x800C0000**, savestate-only). Basiert auf der
  Nutzer-Beobachtung. **RESIDUAL zum byte-true-Schließen:** 1240-Pre-Intro-Savestate ziehen, DAT_800b8521/
  8524/852c über Frames lesen (springt das Fenster [start==full] in 1 Frame → echter Volltext-Printer →
  DEBUG.BIN-Overlay disassemblieren; sonst schneller Typewriter). Workflow: wf_f427c5e9-e8c.
- **Bug B [BT]:** Letterbox-**Balken lagen ÜBER dem Text**. **Fix (`render_pc.c` end_frame):** Balken jetzt
  VOR dem Text-Overlay → Untertitel sitzt auf dem schwarzen Balken (Text über Balken), byte-true zum
  Original (Letterbox POLY_F4-Quads `FUN_80020f8c` im OT hinter den Text-Primitiven).

## 2. ROOM 1240/Pre-Intro — Hintergrund  ✅ (verifiziert gegen Original, KEIN Code-Change nötig)
- **Ursprüngliche Beobachtung:** Port zeigt ROOM1170-Kisten während des Pre-Intro-Textes statt des Intros.
- **Byte-true RE (statisch):** ROOM1240s Narrator = `sub[2]` (subScdStart @RDT 0x53c, off 0x1e) macht
  **9× Cut_chg (cut 0→8) + 6× Message_on**, cycled also durch ROOM1240s EIGENE Backgrounds
  **BG00–BG08** (exakt 9 Dateien vorhanden). SCD-Disasm bestätigt (scd_dis.py). Cut 0=schwarz,
  1=Zombie, 2–4=T-Virus-Zellen, 5–7=S.T.A.R.S.-Logo, 8=Heli/Umbrella.
- **Ursache der Kisten:** Der Boot-BG-Load (`re15_bg_load_cut(0)` @main.c:427) läuft VOR
  `g_current_room_id = boot_room` (@main.c:457) → nutzt den Default `0x1170` (room_common.c:31).
  VOR dem Boot-Fix (a883a93f) lud ROOM1240s RDT nicht → sub[2] lief nie → kein Cut_chg → der
  ROOM1170-Boot-BG blieb stehen = die Kisten. Der Boot-Fix hat ROOM1240s SCD zum Laufen gebracht
  → sub[2]s Cut_chg(0) lädt sofort ROOM1240 BG00 → #2 als Seiteneffekt mitgefixt.
- **Dynamische Verifikation (DuckStation, selbst gecaptured 2026-07-02):** Original-Pre-Intro
  Frame-für-Frame gegen den Port verglichen (`shots/COMPARE_port_vs_original.png`,
  `shots/contact_ORIGINAL.png`): **Port == Original** — beide zeigen schwarz→Zombie→T-Virus→
  S.T.A.R.S.→Heli→Umbrella-Logo mit dem Text DARÜBER, alle 6 Captions, danach Handoff zu ROOM1170.
  Die „pur schwarz"-Beobachtung = der lange Schwarz-Vorlauf (in BEIDEN da). Die Kisten erscheinen
  im Port jetzt korrekt ERST NACH dem Intro (Helipad-Gameplay f3180+).
- **Rest:** main.c:424 Kommentar („boot directly into ROOM1170 cut 0 BG") war irreführend → korrigiert.

## 3. Waffe/Inventar — Modell + Anheben + Schuss-Anim + Effekte falsch  ⬜
- Beim Waffen-Auswählen wird das **Waffenmodell an Leon (außerhalb des Inventars) nicht geladen**.
- **Waffe anheben** spielt die **falsche Animation**.
- **Schießen** spielt die **falsche Animation**.
- **Effekte fehlen** (Mündungsfeuer/Treffer?) — evtl. mit den obigen Punkten gekoppelt.
- (Item-Menü-Submenüs sind separat unvollständig — siehe der laufende Submenü-Map-Workflow.)

## 4. ROOM 1140 — Zombie-Trigger + falsche Anim + Hänger  ✅ (byte-true gefixt, 2026-07-03; live-GUI-Verify offen)

### FIX (Commits 473282cd Anim-FSM-Clock + e9782897 Death-Continue)
- **WURZEL = „THE FSM CLOCK":** jeder Gegner-ANIMATE-Sub-FSM rückt seinen +0x6/+0x5-Sub-Step nur bei
  **Clip-Ende** vor (`+0x6 += func_0x8001f314(...)`, FUN_8001f3bc/f8b4-Return `+0x95++; count<=+0x95 →
  wrap→return 1`). Der Port machte sofortige Ein-Frame-Advances → Snap durch Stand-up/Grab.
  6-Angle-Workflow (STAGE1.BIN-Disasm) + Savestate-Cross-Check (`+0x94`: Engage 0x02∈{2,3,4,5}, Search
  0x01, Walk 0x0a, Death 0x1f).
- **`re15_enemy_clip_done(e)`** = der f314-Clip-End-Gate (aus Death-Handler faktorisiert; Guard→done bei
  fehlender Bank, kein neuer Hang). **Feeding-Stand-up** (case 2) + **Grab-Sub-Steps** (1/3/7) gaten darauf.
- **„falsche getroffen-Anim" GEFIXT:** der Wake snappte in die Engage-Idle-Clip {2,3,4,5}; jetzt spielt
  der Zombie erst seinen Feeding-Clip zu Ende (steht auf), DANN Engage. + Wake-Timer `ai_timer=rand&0xf`
  (0-15 Frames) staffelt die 5 Zombies → kein simultaner Instant-Snap („DIREKT alle stehen").
- **Szenen-Freeze GEFIXT:** der NPC-Anim-Advance lag in `re15_player_tick` (in grab/dead-Branch
  übersprungen) → alles fror beim Grab ein. Verschoben in `re15_actors_anim_advance()`, **unbedingt in
  ALLEN game_step-Branches** (byte-true: FUN_8001a50c per-Typ-Handler, unabhängig von der Player-FSM
  @0x80073f90). Corpse (state 7) hält seine Pose (skip + LOOP-Flag gelöscht).
- **HANG (permanenter Death-Pin) GEFIXT:** HP<0 pinnte den Player ewig (kein Game-Over im Port). Nach dem
  byte-true 0x78=120-Frame-Death-Timer → **Continue-Reload des aktuellen Raums** (`re15_player_continue_
  reload` → re15_room_request_change → re15_actor_init HP=100 → is_dead klärt sich). **[FL / Nutzer-gewählt]**
  — die exakte Continue-vs-Title-Routing (Reader von DAT_800aca59==2) ist ein fehlendes EXE-Subsystem;
  Fade + „YOU DIED"-Screen bleiben deferred.
- **„alle wachen auf einmal" = FAITHFUL** (kein Bug): Wake ist per-Zombie (`FUN_80103980`, eigene
  Distanz<4000), Original + Port identisch, Savestate-bestätigt gestaffelt. Cluster-Geometrie (5 Zombies
  am Tisch) lässt sie zusammen wachen — wie im Original.
- Tests: test_room1140_combat (13b Stand-up-Gate, 6 Grab, 13 Death-Clip, **20 Death→Continue**), 35/35.
  ROOM1140 rendert clean (Smoke). **OFFEN: live-Combat-GUI-Verify** (Wake-Stand-up spielt / Grab spielt /
  Death→Reload) — braucht Spieler-Input, am besten vom Nutzer in der GUI.

### Ursprüngliche Symptome (Nutzer-Beobachtung, alle oben adressiert)
- Zombies starten **korrekt** (fressend am Boden). ✅
- Trigger-Punkt → **SOFORT ALLE** stehen + **falsche „getroffen"-Anim** → jetzt: Stand-up-Anim spielt +
  Wake-Timer staffelt (kein Snap); „getroffen" war die übersprungene Stand-up→Engage-Idle {2,3,4,5}.
- **Biss falsche Anim** → Grab-Clips gaten jetzt auf Clip-Ende.
- **Hang** → Szenen-Freeze + permanenter Death-Pin, beide gefixt.

### RE-Historie (Befunde während der Untersuchung)
- **Wake-Trigger IST per-Zombie** (kein globaler Trigger): `FUN_80103980` prüft die EIGENE Distanz
  des Entities (`_DAT_800ac784 + 0x1d0 < 4000`) → der Port (`re15_enemy_ai_live_feeding`,
  `e->ai_dist < 4000`) macht das gleiche. Also „alle wachen auf einmal auf" ist NICHT der Trigger —
  entweder sind die 5 Zombies geclustert (dann korrekt) ODER `e->ai_dist` wird im Port nicht
  per-Zombie berechnet. **TODO: empirisch prüfen** (Port ROOM1140 laden, ai_dist je Slot lesen).
- **WURZEL der 3 Anim-Symptome = die deferrte Enemy-Anim-Hälfte.** Der Port hat die Zombie-Clips
  bei den Übergängen NICHT gesetzt (deferred): Wake-Stand-up (`re15_enemy_ai_live_feeding` case 2 =
  „faithful stand-in", kein Clip), Grab/Biss („port has no zombie grab anim"), Hurt (`hurt_clip`
  nur geseedet). → Zombie bleibt im falschen Default/Stale-Clip stehen = die „falsche getroffen-Anim".
- **Original-Dispatch-Kette:** `FUN_801018f8` (feeding-Handler) ruft ZWEI Tabellen indexiert per
  entity+0x5: `PTR_FUN_8011f9d0[+5]` (Logik) + `PTR_FUN_8011f9d4[+5]` (**ANIMATE** — die deferrte
  Hälfte). Der Stand-up-anim_set sitzt @0x80103b08 (+0x170/+0x174). **TODO: die zwei Tabellen aus
  STAGE1.BIN auflösen (re15-psx-disasm) → Stand-up/Hurt/Biss-Clip-Indizes byte-true bestimmen.**
- **Hang-Verdacht:** die Death/Grab-Lock-Kette ohne Game-Over-Infra — Biss → HP<0 → Death-State 7 →
  game_step friert Spieler ein (Death-Timer 0x78), aber kein Game-Over/Fade → Spieler permanent
  eingefroren = der „Hang". ODER der player-grabbed-Lock (`s_player_grabbed`) löst nie aus.
  **TODO: empirisch belegen wo genau es freezt.**
- Quellen: `FUN_80103980/801018f8/80102548/80105a8c/80100688.c` (STAGE1_full), Dispatch
  `@0x8011f7b4`/`@0x8011f80c`/`@0x8011f890`, Tabellen `PTR_FUN_8011f9d0/d4`.

## 5. ~~ROOM 1070~~ ROOM 1150 — Irons-Office-Cutscene  ✅ (2026-07-06, RE-verifiziert; funktioniert)
- **RAUM-NUMMER KORRIGIERT:** Die Irons-Cutscene ist **ROOM1150 = "CHIEF OFFICE" (JUMP 0x115)**, NICHT
  ROOM1070. ROOM1070 = "LOBBY OFFICE" (0x107), ein 5-Zombie-Kampfraum ohne NPC/Cutscene (Doc-Vertipper
  "LOBBY OFFICE" ↔ "CHIEF OFFICE"). Beleg: `information74.txt:108` (0x115 = SHOCHO/Chief/Irons);
  der Cutscene-End-Byte-Block existiert NUR in ROOM1150.RDT (@0xf6a), in ROOM1070.RDT keine Übereinstimmung.
  Irons nutzt Leons Modell als Platzhalter (`information74.txt:251`) → die Figur wird von PLAYER-Opcodes
  animiert (Plc_motion/Plc_neck/Plc_ret), kein NPC-Handler. Workflow wf_c5742c63-dc4.
- **Byte-true RE der Cutscene (ROOM1150 sub[8], getriggert per AOT 6):** Cut_chg 5 → drei `Plc_dest`
  Geh-zu-Ziel + `do{Evt_next}edwhile`-Ankunftsschleifen → Kniefall (Plc_motion 0x0b) + Plc_neck +
  Message_on-Dialog → Cut_chg 7 → Tail `Cut_old` (Kamera zurück + Auto-Cam an, byte-true op_cut_old) →
  `Plc_motion→routine 4` → `Plc_ret` (routine 1 = Gameplay, byte-true LAB_80041f88) → `Set(2,7,0)/
  Set(1,27,0)` (Cine-Flags löschen) → Evt_end. **Keine blockierenden Opcodes** — die Cutscene MUSS enden.
- **VERIFIZIERT (headless, RE15_FORCE_EVENT=8 aus dem AOT-6-Bereich):** Walk-in kommt am Ziel an (F52) →
  Kniefall/Dialog (mo 11/10/12) → Ende bei F1421 (mo→200 Idle) → **Plc_ret-Handoff, player_mode→0** →
  Post-Cutscene-Input (pad=UP) **bewegt Leon wieder** (F1519 mo=105, x läuft). Letterbox + Kamera-Cut
  korrekt (Screenshot `shots/irons_kneel.png`: Leon kniet über dem liegenden Chief, Balken oben/unten).
- **Der ursprüngliche „Hänger" reproduziert NICHT mehr** — durch die umfangreiche SCD-Clock-/Cutscene-Arbeit
  seit dem 02.07. behoben (Do/Edwhile-Ankunfts-Poll + Plc_dest-Walker + Sleep-Yield laufen byte-true).
  Ein „Hänger" ist nur bei FALSCHER Startposition erzeugbar (Test-Artefakt: startet man Leon am Spawn statt
  am AOT-6-Trigger, blockt eine Wand bei z=-15446 die Straight-Line-Walk → der Walk-to-dest-Poll kommt nie
  an; der echte Trigger AOT 6 ≈(-21550,-22800) liegt aber neben dem Ziel = kein Wall im Weg).
- **Bekannte byte-true DIVERGENZ (aktuell benign, Refactor deferred):** Der Port keyt player_mode=2 +
  Letterbox auf die Cine-FLAGS (1,27)/(2,7) [main.c:1236]; das Original gated den Input auf das Player-
  ROUTINE-Byte DAT_800aca58 (Plc_motion→4 scripted / Plc_ret→1 gameplay). Für JEDE Cutscene die der Port
  fährt sind beide observably identisch (die Cutscene setzt UND löscht beide + Plc_ret). Nicht refactort:
  der Flag-Gate ist load-bearing für 1170/1240-Intro; ein Umbau riskiert dort Regressionen für einen
  nicht-reproduzierenden Bug. Recovery-Spec im Arbiter-Verdict (wf_c5742c63-dc4).

---

### Reihenfolge / Notizen
- Präsentation (1,2) vs. Combat-AI (4) vs. Cutscene-Ende (5) vs. Waffen-Pipeline (3).
- **Hänger (#4, #5) sind funktional am schwersten** (Freeze) — ggf. vorziehen, falls der Nutzer will.
- #1+#2 teilen sich evtl. das Intro-Präsentations-Subsystem; #3 koppelt an Combat/Effekte (#4).
