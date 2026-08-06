# Kampagnen-Roadmap: Raumkette byte-true (ROOM 1120 · 1130 · 1140 · 1150 · 1170 · 1240)

*Angelegt 2026-08-02. Dies ist die OPERATIVE Roadmap der laufenden Kampagne; die
Master-Roadmap des Gesamtprojekts bleibt [PORTING_ROADMAP.md](PORTING_ROADMAP.md).
Diese Kampagne ist auf MEHRERE Sessions ausgelegt — jede Session liest zuerst §0,
dann §5 (Arbeitsstand), und arbeitet die oberste offene Zeile ab.*

---

## §0 DIE REGEL — vor allem anderen (aus `lesson.txt`, vom Nutzer erneut verlangt 2026-08-02)

> **„Verlange für jede Zahl/jedes Verhalten, das ich in den Port schreibe, die @0x…-Adresse."**

Konkret (Langform in [lesson.txt](lesson.txt) und CLAUDE.md STOP-GATE):

1. **Beleg zuerst, Code danach.** Disasm-Adresse + Instruktionen im Chat posten, BEVOR die
   erste Zeile Fix-Code entsteht. Jede Konstante trägt ihre `@0x…`-Adresse im Code-Kommentar
   UND in der Commit-Message.
2. **Der Nutzer hat ein Veto.** Auf jede Zahl darf er sagen „Zeig mir die Disasm-Adresse dafür."
   Kommt sie nicht → geraten → zurückweisen.
3. **„Das Symptom ist weg" ist kein Ergebnis.** Verifiziert wird gegen Original-Werte
   (Disasm, Savestate-RAM, Datei-Bytes), nicht gegen Höreindruck/Screenshot.
4. **Sackgasse ≠ Ende.** Weg wechseln (Xrefs, Caller/Callee, RE2-Vergleich, Savestate-RAM,
   PCSX-Redux-Watchpoint), nie die Lücke mit einer Gefühlszahl füllen.

---

## §1 Ziel und Scope

Die Kette **1240 → 1170 → (Hof) → 1130 → 1120/1140/1150** steht weitgehend und wirkt korrekt.
Ziel dieser Kampagne:

1. **Die bekannten Rest-Divergenzen beseitigen** (Nutzer-Report 2026-08-02):
   - **Rolltor-Sound** (1170 → Hof → 1130): Fahrgeräusch fehlt/falsch.
   - **Krähen-KI ROOM1170**: Verhalten weicht vom Original ab.
   - **Zombie-KI ROOM1140**: Hit-Reaktion falsch — konkret (a) Schuss auf einen Zombie
     WÄHREND der Grab-Release-/Wegstoß-Animation → merkwürdiges Verhalten; (b) Schuss auf
     einen am Boden liegenden (lebenden) Zombie → falsches Verhalten.
2. **Systematischer Sweep über die 6 Räume**: alles finden, was noch vom Original abweicht
   (SCD, AOT, Spawns, Sound, Events) — nicht nur die gemeldeten Symptome.
3. **Global statt lokal fixen:** Wenn ein Mechanismus im Original global ist (EXE-seitig, für
   alle Räume), wird er im Port global implementiert — nie als Raum-Sonderfall. Raum-Spezifika
   dürfen ausschließlich aus Raum-DATEN (RDT/SCD-Bytes) kommen, nie aus C-Code-Sonderfällen.
4. **Gesamt-Übersicht Adressen:** was vom Original ist bereits mit `@0x…`-Beleg portiert, was
   fehlt → [RE15_ADDRESS_COVERAGE.md](RE15_ADDRESS_COVERAGE.md) (generiert, reproduzierbar
   über `analysis/coverage/`).

---

## §2 Phasen

### W0 — Struktur & Inventar *(Session 2026-08-02 — KOMPLETT)*
- [x] Analyse-Workflows: Adress-Coverage-Zensus + 3 RE-Dossiers (Krähe/Zombie/Rolltor)
- [x] Artefakt-Cleanup (~1,3 GB shots-Scratch, Root-Logs, .sentry-native, __pycache__;
      .gitignore erweitert, damit es nicht wieder aufläuft)
- [x] Diese Roadmap; lesson.txt-Regel verankert (§0)
- [x] RE15_ADDRESS_COVERAGE.md erzeugt und geprüft (`fb42f86e`) — EXE 250/386 zitiert,
      STAGE1-Overlay 420/650; Lückenlisten als Anhang; reproduzierbar via `analysis/coverage/`
- [x] Dossiers adversarial verifiziert (ALLE Findings CONFIRMED) und in §5 eingetragen

### W1 — Die drei gemeldeten Symptome fixen (in dieser Reihenfolge)
Jeder Fix folgt §0: Beleg → Code (`@0x…` je Konstante) → Messung → ctest-Pin.

- [x] *(Code+ctest `355adbdd`, 2026-08-02 — Live-gdigrab-Verifikation offen → §5 Punkt 1)*
      **W1.1 Zombie-Hit-Reaktion 1140** — Dossier `analysis/zombie_hit_1140.md`.
      Akzeptanz: für Symptom (a) und (b) steht dokumentiert, was das Original in exakt der
      Situation tut (Adressen), der Port tut dasselbe, und ein room-probe-/AI-ctest pinnt
      beide Übergänge. Verifikation zusätzlich visuell per gdigrab (Skill
      `re15-port-visual-verify`), NICHT per Autoshot.
- [x] *(Code+ctest `2446139c`, 2026-08-02 — Rest-Divergenzen D7/D8/D9 + Live-Parity offen → §5 Punkt 3)*
      **W1.2 Krähen-KI 1170** — Dossier `analysis/crow_1170.md`.
      Akzeptanz: alle Divergenzen der State-Machine gefixt oder als byte-true widerlegt;
      ROOM1170-Spawn-Parameter (Sce_em_set-Operanden) stimmen mit RDT-Bytes überein;
      Flugverhalten im Live-Fenster gegen PSX-Capture verglichen.
- [x] *(Code+ctest `d10b7893`, 2026-08-02 — Hörprobe + PSX-Gegenprobe offen → §5 Punkte 1/5)*
      **W1.3 Rolltor-Sound** — Dossier `analysis/rolltor_sound.md`.
      Akzeptanz: der Original-Mechanismus ist mit Adresse belegt (welcher Code, welcher SE,
      welche Bank), der Port spielt denselben Cue am selben Trigger-Punkt. Falls statisch
      nicht auffindbar: PCSX-Redux-Lua-Watchpoint auf SPU-KeyOn beim Tor-Durchgang
      (Skill `re15-pcsx-watchpoint`) — der Weg dorthin ist in
      [HANDOVER_2026-08-01.md](HANDOVER_2026-08-01.md) §2b fertig beschrieben (Autopilot-Route;
      offen ist nur die Hindernis-Umfahrung im Hof bei x ≈ −17000).

### W2 — Systematischer Raumketten-Sweep (pro Raum, Checkliste)
Für jeden der 6 Räume, gegen Original-Daten (RDT-Bytes) und Original-Code (Disasm):

| Prüfpunkt | Methode |
|---|---|
| SCD-Opcode-Census (main00 + ALLE subs) | `RE15_SCD_TRACE=1` Runtime-Lauf (Offline-Walker desynct — verboten als Beleg) |
| AOT-/Door-Census inkl. aller Operanden | RDT-Bytes parsen vs. `RE15_AOT_DUMP=1` |
| Enemy-Spawns (Typ/Pos/Rot/Parameter) | Sce_em_set-Bytes vs. Savestate-RAM (`0x800acc2c`, Stride 0x1f4) |
| BGM-Track + SE-Bänke | Tabellen `@0x8007498c`/`@0x80074a0c` vs. `debug.log` |
| Events/Cutscenes | sub-Threads einzeln (`RE15_FORCE_EVENT=N`) gegen DuckStation |
| Kamera-Cuts (RVD) | bestehende ctests + Stichprobe |

- [ ] W2.1 ROOM1240 · [ ] W2.2 ROOM1170 · [ ] W2.3 ROOM1130 · [ ] W2.4 ROOM1120 ·
  [ ] W2.5 ROOM1140 · [ ] W2.6 ROOM1150
- Jede gefundene Divergenz: erst prüfen, ob der Mechanismus GLOBAL ist (EXE) → dann global
  fixen und in ALLEN 6 Räumen gegenmessen.

### W3 — Coverage-getriebene Lückenschließung
- [ ] Aus RE15_ADDRESS_COVERAGE.md die nicht-zitierten STAGE1-Overlay-Funktionen triagieren:
      {gameplay-relevant → RE-Ticket} / {toter Code/anderer Raum → dokumentiert offen}.
- [ ] Dasselbe für die EXE-Lücken je Subsystem (Katalog-Kreuzung liegt im Dokument).
- [ ] Coverage-Zensus nach jeder Fix-Welle regenerieren (`analysis/coverage/`-Skripte).

### W4 — Regressionsnetz
- [ ] Jeden W1/W2-Fix als ctest pinnen (room-probe / AI-probe), damit die Kette nicht
      still regressiert. Ziel: `ctest` deckt die komplette 6-Raum-Kette ab.

---

## §3 Verifikations-Harness (Kurzreferenz)

Vollständig in [HANDOVER_2026-08-01.md](HANDOVER_2026-08-01.md) §1/§2b. Kernpunkte:
- **Kein `RE15_START_ROOM` mehr** (entfernt `79e8504f`). Sprung: `RE15_DEBUG_JUMP="<hexraum>@gp"`;
  Laufen: `RE15_INPUT_SCRIPT`; Zielpunkt+Aktion: `RE15_AUTOPILOT="xza:x,z;…"`.
- **Vor jedem Build die Exe killen** — sonst scheitert das Linken still.
- `--headless` hängt in dieser Umgebung — Messen über Fenster-Lauf + `debug.log`.
- Visuelle Verifikation NUR per ffmpeg-gdigrab (`re15-port-visual-verify`) — Autoshot maskiert.
- Diagnose-Schalter: `RE15_SCD_TRACE` · `RE15_AOT_DUMP` · `RE15_SE_DEBUG` · `RE15_FLAG_TRACE` ·
  `RE15_PAD_DEBUG`.

---

## §4 Artefakt-Hygiene (Stand nach Cleanup 2026-08-02)

- Gelöscht: ~1,3 GB ungetrackte `shots/`-Scratches, Root-Logs (`*.log`, `err.txt`,
  `build_err.txt`, `grep.exe.stackdump`, Datei `2`), `.sentry-native/` (66 MB Crash-DB),
  `scripts/__pycache__/`. `.gitignore` erweitert (Root-Logs, `/shots/`, `.sentry-native/`,
  `__pycache__/`, `stage_saves/*.sav`).
- **Behalten:** `stage_saves/` (Ground-Truth-Savestates, 122 MB, bleiben LOKAL; neue Saves
  sind jetzt gitignored — ein goldener Save, der ins Repo soll, braucht `git add -f`).
  Getrackte Referenzen unter `shots/` (44 Einträge) bleiben.
- Neue getrackte RE2-Referenz-BINs: `info/re2leon/COMMON/BIN/{EMOVL10_S0,EMOVL10_S1,EMZ0,EMZ0_d1}.BIN`.

---

## §5 Arbeitsstand (lebende Sektion — JEDE Session hier fortschreiben)

| Datum | Was | Ergebnis / Commit |
|---|---|---|
| 2026-08-02 | Kampagne aufgesetzt: Coverage-Zensus, 3 RE-Dossiers (alle Findings adversarial CONFIRMED), Cleanup, Roadmap | `fb42f86e` (Coverage), `590d6146` (Cleanup+Roadmap) |
| 2026-08-02 | **W1.1 Zombie-Hit 1140**: beide Nutzer-Symptome gefixt — (a) Reverse-Leak (Reverse ist f314-ARGUMENT @0x80102aec, kein Zustand; Clear am Damage-Hijack), (b) Scripted-Lyer unschießbar (Wake-Phase-0 `+0x93\|=1`/Tick @0x80103aac-ab8) + Wake-Maschine FUN_80103a58 portiert; dazu D3 SE-Rolls, D4 Y-Korrekturen, D5 Choreo-Latch, D6 Grab-Bit, D7 anim-Wrap (@0x8001F610-3C, jetzt mit Bank-Regel). ctest `unit_zombie_hit_fixes` | `355adbdd` |
| 2026-08-02 | **W1.2 Krähen-KI 1170**: Kernbefund — Original entwaffnet per `grid&0x10` (@0x801123cc-e8), ROOM1170 = 5/7 Krähen PERMANENT passiv (Port griff mit allen 7 an); + Weave in Launch/Arc, Anim-Tick in Approach, ungegateter Grab/Strike-Schaden, Timer-Wrap, Death-Flock-Writes, INIT-f314, Doku-Erratum D10. ctest `unit_crow_fixes` | `2446139c` |
| 2026-08-02 | **W1.3 Rolltor-Sound**: Mechanismus BELEGT — Tür ist im Original STUMM; das Fahrgeräusch ist ROOM1130 sub02 `Se_on(2,12/10/11)` auf snd0. Port-Defekt war fehlender Tone-Pitch/-Volume: `note2pitch2` EXAKT portiert (fine+shift ADDIEREN, Tabelle DAT_80077520), Mixer per-Voice Q16-Step, Tone-Volume; Se_on-Operanden korrigiert (id\|FLAGS); bank 5→snd0. ctest `unit_se_pitch` (Fixpunkte 512/421/406 = 5512/4533/4371 Hz) | `d10b7893` |

| 2026-08-02 | **W1.2-NACHTRAG (Nutzer: „Krähen haben NOCH NIE angegriffen")**: Sonde `probe_crow_1170` fand die Wurzel — Krähen-INIT installierte die Hitbox nie (`+0x78 = DAT_80121108` @FUN_80111a4c, Box @0x801210fc {0,0,0,200,180,200}, ofs_y=0-Ausnahme) UND Spieler-Box-ofs_y war 0 statt −1530 (PSX.EXE file 0x64694 byte-verifiziert) → aec4-Y-Band wies den Grapple-Hover immer ab. Nach Fix: Sonde hp 100→76, Grab-Zyklus 12→13→14 läuft; Dossier-Nachtrag D12 | *(dieser Commit)* |

| 2026-08-02 | **Verletzungs-Optik-RE (Nutzer-Request)**: byte-belegt — der Modell-Mechanismus ist ein KOSTÜM-Slot-System (`DAT_800aca5c` → Tabelle @0x80073f70, Reload nur beim Raumwechsel @0x80039750-8c); **kein HP/Condition-Input, im MZD-Build dormant** (Writer-Zensus geschlossen, 92 Savestates slot=0). Blutige TIM-Assets liegen unreferenziert. Sichtbares „Verletzt" = Injured-Anims + Inventar-Condition (beides im Port). NEUE echte Divergenzen: Slot-Mechanismus fehlt / Elza spielt in-game mit Leon-Modell / STPIC hardcoded (Q2-Blocker gelöst @0x800c01c4). Dossier `analysis/leon_injured_model.md`; PL01.PLD ergänzt | `02ed15eb` |

| 2026-08-02 | **Verletzungs-Optik DYNAMISCH bewiesen** (HP-25-Patch + DuckStation-Capture): HP100 vs HP25 = NULL VRAM-/MD1-Differenz; Motion 2→**23 = Injured-Idle** — der „verletzte Leon" IST das Animations-Set, kein Modell/Textur-Wechsel. Scheinbefund Tile/CLUT war Waffen-Confounder. OFFEN: verletzter WALK (DS-Pad-Config, dann HP25-Geh-Capture) + Port-Live-Check der Injured-Idles (Sentinel 213); Elza-Bycatch: FUN_80024c30 gated auf `aca5c&4` = vermutlich Elza-Haar (Krähen-D7-Hinweis) | `c1f0064d` |

| 2026-08-02 | **BLUT-DECAL-SYSTEM gefunden + portiert** (Nutzer-Save als Ground Truth): DR_MOVE-Blits aus der Damage-Bank (TIM-Seite 2) auf 8 Panels, Akkumulator FUN_80037edc (Schwelle 120, Clamp 2), Trigger = Overlay-Hurt-Dispatcher; KEINE HP-Schwelle. Port: Wund-Tabelle + Trigger + Slot-0-Blit + Re-Apply; ctest `unit_wound_accumulator`. OFFEN: Substate-0/1-Writer, STAGE2-5-Helper-Args, Gegner-Wunden @0x800b267c, Live-gdigrab | *(dieser Commit)* |

| 2026-08-03 | **Wund-Trigger korrigiert + Biss-Blut komplett** (`b26fe943`): Dispatcher @0x8010a580 = Phase 4 der Grab-OPFER-FSM → Stempel NUR beim überlebten Grab (Mash-Release); Falsch-Stempel aus dem Hurt-Entry entfernt. Alle Biss-/Release-/Devour-/Krähen-Blut-Spawns mit Original-Scales (0x1500/0x2000/ctr<<11) + Bone-Ankern portiert; 7 Alt-Sites von halber Scale auf 0x2000 gehoben. OFFEN: Steh-Biss-Direktstempel @0x8010f360, Dog-Knockdown-Klasse 4/5, hitchain-VERIFY ausstehend, Live-gdigrab Biss-Sequenz | `b26fe943` |

| 2026-08-03 | **HIT-Verify komplett** (6×CONFIRMED, 2×PLAUSIBLE): Dog-Sprung-Biss stempelte im Port Zufalls-SEs statt Wund-Panels (Bytes 05 07 04 06 @0x80121014 = PANEL-Indizes, als SE-Tabelle fehlgedeutet) → gefixt `1e7f4338`. OFFEN: Dog-Heavy-Biss-Knockdown-Klasse facing+4 (@0x801187e8/f0; EXE-Handler 0x800360e8/0x8003644c), Liegend-Devour-Einstieg, cmd-4-Familie, PCSX-Watchpoint für indirekte aca58/59-Writer | `1e7f4338` |

| 2026-08-03 | **Krähen-LIVE-Fix + Knockdown-Klasse** (`c9a19140`/`8c3ece3e`): LOS-Ray byte-true (FUN_8003dcc4 mit GEGNER-Höhenband-Filter — Sonde: 0→620 Attack-Ticks live), Elevation-Band-Stempel (hohe Krähe braucht UP-Aim), Death-Lane-Map, +0x9a-Latch; Spieler-Knockdown [4]/[5] komplett (0x800360e8/0x8003644c, Slam-Zweig, 0xb-rev-Aufstehen, i-Frames), Boss-0x27-Trigger. Liegend-Devour WIDERLEGT (dormante Girl-Ambush, toter Code — nichts erfunden). OFFEN: Krähen-Wurf-Exit (Mode 6, Watchpoint), aca3c-0x40/0x80-Leser, aca52-Bit-0-Semantik, Waffen-14-Lane, visuelle Clip-Prüfung 0xb-0x10 per gdigrab | `8c3ece3e` |

| 2026-08-03 | **ROOM10A0-Treppen-Richtung** (Nutzer: „hintere Treppe steigt hoch statt runter"): Original RE'd — KEINE Zonen-Paarung; sce-12/13-Handler LAB_80043500/LAB_800435cc entscheiden PRO Record aus Rect-Hälfte × side-Flag rec+0xC (`ASCEND ⇔ low_half==(side==0)`), Ziel = band ± count&7 (rec+0xE, Gait @0x800389cc-dc), Facing = Achsen-Kardinale (acabe-Settle @0x8003891c-9ac). Port paarte über event_id = das side-Flag → in 10A0 (2× side-0 + 2× side-1) falsche Partner. Fix: per-Record-Entscheidung + side/count/axis/corner/extent-Registrierung + Kardinal-Snap + Probe 800→620 (@0x80042bd0). Proben: `probe_stair_10a0` 8/8 == Original, `probe_stair_1170` (neu, ctest) 4/4. Dossier `analysis/stair_10a0.md` (5×CONFIRMED, O2 Clip-5-Settle offen) | *(dieser Commit)* |

| 2026-08-03 | **Leons Angegriffen-Anim bei Krähen** (Nutzer-Report): Hook A [0x21] = LAB_8011597c komplett RE'd (Front-FSM 0x801159bc: EM021-victim Clips 0→1-Loop→2 aus Paar C, Blend 7, hit_react-Latch; Rear = nur 1171-Szene; Hook B = toter Code). Port: Victim-Latch am Krähen-Grab (@0x80113e48/e30), Clip-Map 0x21, kein Root-Motion/Yaw-Flip/Release-Blend (byte-Fakten der Krähen-FSM); EM021-Bank-Geometrie aus dem dir[6]-Pool-Header (15/80) statt dir[2] (13/72); Flinch-Aim-Gate ENTFERNT (Original ungated @0x80113b00, cmd-Dispatch @0x80031c88 ersetzt Aim → `re15_player_aim_interrupt`), Flinch-Richtung auf Yaw-Vergleich a780 (@0x8001a788-a4, vorne→Clip 8) umgestellt (Port war invertiert); `re15_crow_screech` als Fehldeutung des Wund-Stemplers 0x801161e8 ersetzt (vollständige vert-Bänder an Dive/Grab/Strike). Sonde `probe_crow_flinch`: Grabs spielen 0→1→2 (14/36/20f), Dive-Flinch auch unter R1, kf_size 80. Dossier `analysis/crow_victim_anim.md` (8×CONFIRMED, 1×PLAUSIBLE). OFFEN: 1171-Wurf-Szene (Exit jetzt RE'd: Script-Plc_dest), visuelle gdigrab-Prüfung | *(dieser Commit)* |

| 2026-08-03 | **ROOM10D0 Marvin-Cutscene → VIER GLOBALE Cutscene-/NPC-Fixes** (Nutzer erlaubte global; Dossiers `analysis/marvin_10d0.md` + `analysis/cutscene_headlook.md`, 12/13 Findings CONFIRMED): (1) **Plc_neck → Work-Entity** (@0x80041e9c) statt Player-Hardcode + komplette Neck-FSM FUN_80037358 global: Klemmen (Spieler ±0x200/±0x138 @0x800319b0-c4, NPC ±0x2c8/±0x138 @0x8011c7a0/b0), Default-Steps 96/96 bzw. 64/48, Modi 2/3/4 (relativ/Sweep, Abschluss→0x12 @0x80037698), Entity-Tracking (Flags 0, Kopf-Part-Welt-Cache), NPC-Neck-Init (@0x8011c738-b0, grid&0x40→0x12 @0x8004260c-18), Gameplay-Auto-Look (State-1-Prolog @0x80031e04-40, konservative 3703c-Portierung); Messung: max-Yaw exakt 512 = Klemme (vorher 901), Marvins Necks auf Marvin. (2) **RBJ-Marker-Binder** (FUN_8001b3f8, Ziel +0x180/+0x184) datengetrieben statt Handtabelle — 10D0 REC1→Marvin bindet (Probe: 20/30/30/20/30/25/35/50/24/20 == REC1-EDD); Executor-Sub→Kanal-Map (0=RBJ, 1=Loco, 2/6/Walk=eigene, 3=Victim) in Länge UND Render; Elliot-1170-Ausnahme dokumentiert (O3, vor Umstellung messen). (3) **Plc_dest = State-4-Dispatch** (@0x80041c14-18): Modi 0-3/6 sind Pose-/Event-Subs, KEIN Walk — Mode 6 = Event-Reach @0x800517f0 (Marvin war 1700 Ticks eingefroren + weggedreht Richtung (0,0)); Spieler-Mode-6 = PLW-Clips 1→2 (Render-Override). (4) **Executor-Subs 1-3 = Play-once+Hold** (0x80050ddc/f00/1024, Double-Step 0x8, No-Blend 0x40) statt Endlos-Loop; NPC-Walk-Clip 5 (@0x800511dc) statt Player-Sentinels. Proben: probe_marvin_10d0 (alle 4 Hypothesen behoben, Dir_set-Facing 1770 hält), probe_headlook_10d0 (Klemme exakt). 104/104 ctest | *(dieser Commit)* |

| 2026-08-03 | **Nutzer-Runde 2 (5 Punkte)**: (1+2) NPC-Gleiten gefixt (`3f5310cc`) — Elliot-Regression zurückgenommen (PLD-Bank-Ausnahme), Walker advanced NPC-Walk-Clips selbst (Original: f314 jeden Frame im Walk-Sub); (3) **ROOM10D0-Liegend-Zombie** steht jetzt auf: behavior sel 0x0E ist KEIN pose-only — Original-INIT @0x80100f64-fd4 startet die Sleeping-Submaschine (Word +0x4=0x1201, grid=0); Decoder-Zweig + Advancer-Schlaf-Halt (Phasen 0/1 ohne f314: case 0 @0x80105534, case 1 → Epilog @0x8010560c) nachgezogen — Probe: schlafen fr=0 → Annäherung <0xBB8 → Clip 0x2A 28f → Standup 0x29 59f → Engage (`analysis/zombie_lyer_10d0.md`, 3×CONFIRMED); (4) **Save-Zähler byte-true**: DAT_800b0fbd = LIVE-Zustand (frisch=0 @EXE 0xa17bd, PRE-Inkrement-Titel /NN/ @0x80026eac-f0, ++ nur nach Erfolg @0x80026488-9c, Load restauriert @0x80026290-a0) — Karten-Max-Seed gestrichen, New-Game-Reset, /00/-Semantik (`analysis/save_counter.md`, 3×CONFIRMED); (5) **Wund-Persistenz v5**: Original-Save-Blob ENTHÄLT die Wund-Tabelle (GSB+0x130 @0x800b10ec im 0x1230-memcpy @0x800261c4-d8) → Save v5 speichert/restauriert wounds[8][2] + Generation-Bump (Wound-Sync re-stempelt); behebt zugleich Stale-Blut nach CONTINUE (SI-3). HP/Injured-Anims waren bereits korrekt (SI-2). (`analysis/save_injured_state.md`, 4×CONFIRMED+1 PLAUSIBLE) | *(dieser Commit)* |

| 2026-08-03 | **Nutzer-Runde 3 (6 Punkte, Teil 1)**: (5) **Charakterauswahl-Textkästen** + (6) **Fallback-Font entfernt** (`bf92ef5b`): PSX-Transparenzregel (Farbwert 0x0000 = voll transparent, nur STP-Bit 0x8000 = opakes Schwarz; psx-spx) in SELECTH3-Atlas + tmoji_strip; 6x8-Tabelle komplett gelöscht, Debug-Menü/Untertitel auf TEX.TIM-Spielschrift (8-px-Raster @0x80014AB4-C08), Item-Prompt vs. Message-Doppelbeschriftung (34,180) entschärft. (2) **Tote Krähen byte-true** (`eab426af`, Dossier `analysis/crow_death_pool.md`, Kern-Zitate disasm-selbst-reproduziert): State 7 CORPSE 0x801157e8 hat KEINEN f314 → Freeze Frame 12 des Land-Clips 0x0a (letzter Tick = FINISH-f314 @0x8011493c); (3) **Blutlache = Schatten-Grower** Handler [0] 0x80115830 (+0xbc/+0xbe += 10/Tick × 51, Farben 0x00ffff38 @0x80115880-c8) + ACTIVE-Tail-Basis 0x80115f70 (Größe ((y−floor)>>4)+400 min 100, Tint >>5+128 min 32); GIB → sub 1 = Pool-WIPE (a0=1 @0x80114b7c) + 13-Bone-Scatter (@0x80114a50-aa4, Kill @0x80114b78 → crow_hide); GIB-Step-Router lief auf +0x6 statt +0x7 (`lbu 7(a0)` @0x801149d4) → Step 0 (Se(3)/Splatter/Timer/Flags) lief NIE; Land rot_z=0 @0x801148f8; word0|=0x2|0x40 an allen Lane-Enden. Regression `unit_crow_death_corpse`. (4) **ROOM1170-Treppentür verschlossen** (`57eb84e4`, Dossier `analysis/door_lock_1170.md`, Bytes selbst-verifiziert): Lock = sce1/Door-ZWILLING auf `Ck(4,0xc3)` (Aot_set msg 0x0c „It's locked from the other side." @scd 0x012c vs. Door @0x0144); Root-Cause war PORT-EIGEN — Boot-Pre-Stage `flag_set(4,195,1)` (pc+psx main.c) aus einer Fehllesung („sonst Dead-End"; tatsächlich umfasst der Ifel/Else NUR Slot 5, `Else_ck 07 00 26 00` @0x0140 → 0x0166 = slot6 dahinter) — gestrichen; Unlock byte-true nur ROOM1140 sub02 `Set(4,0xc3,1)` @0x0190; game-weit 36 Zwillingspaare, alle über den generischen Pfad = Fix global. Regression `unit_door_1170_lock`. (1) Marvin-Spawn/Start-Anim: RE-Workflow lief in einen Runtime-Stall (DuckStation-Capture) → neu aufgesetzt (`wf_a76ac28e`), OFFEN | `eab426af`/`57eb84e4` |

| 2026-08-03 | **Marvin-Spawn/Start/Lauf GELÖST (Runde 3 Punkt 1)** (`2194c41f`, Dossier `analysis/marvin_spawn_anim.md`, 5/5 CONFIRMED; Ground-Truth = DuckStation-Savestate r10d0_walk1 des abgestürzten Workflow-Vorlaufs): Die State-Maschine des Ports war byte-true — aber der **Executor-Kanal +0x170/+0x174 ist die BANK 1 (dir[4]/dir[3])**, positions-fest gemappt vom Kanal-Loader FUN_80022300 (@0x800224b8/c8), während der Port die largest-Bank (dir[1], 24 Clips) spielte → Clip-Wraps 32/50 statt 16/52 = falsche Posen für Spawn-Idle, Gesten UND Szenen-Walk (Clip 5 = 30f). Verify-KORREKTUR am Fix-Scope: eigene Bank spielen NUR Subs {2,4,5,6,9} (Dispatch @0x80076ca0); Subs 7/8 = +0x84/+0x16c (@0x80051a20/24, @0x80051c18/1c). Port: `re15_emd_parse_own_bank` (dir[3]/dir[4], own_ok=0 bei leerem dir[3] schützt Dog/Crow/Gorilla), Kanal-Map + Walker-Länge + Render-Override, Bank-1-Fallback-Tabelle, INIT-f314-Schritt; `test_npc_scd_pose`-Fixture auf Bank-1-Clip 5. Regression `unit_marvin_spawn_bank`. Betroffen alle 7 NPC-Typen 0x40-0x4d | `2194c41f` |

| 2026-08-03 | **Nutzer-Runde 4 (2 Punkte)**: (1) **Krähen-Lache schwebte im Körper** (`f6bdfef0`) — Nutzer-Verdacht berechtigt: FUN_8001b064 setzt `t[1] = param_2` (Y = Argument), der Krähen-Post-Pass übergibt a1 = +0x1ba FLOOR (@0x8011221c-234); Port zeichnete auf npc->y = floor−400 (Leiche @0x801148e4) → Quad auf Körperhöhe. Typ 0x21 jetzt crow_floor als Quad-Y. gdigrab-Bestätigung der Kill-Szene OFFEN (Harness erreichte den Abschuss nicht: CONTINUE-Save trägt Messer; RE15_EQUIP wird vom Restore überschrieben). (2) **10D0-Re-Entry-Zombie = Statue** (`0887b138`, Dossier `analysis/zombie_10d0_reentry.md`, F1-F4 CONFIRMED): Nach der Cutscene (Set(4,247) NUR in sub21) spawnt der ELSE-Zweig denselben Zombie STEHEND (beh 0x02) — Port hatte für Grid-Nibble 2 kein Dispatch (`default:break`), der byte-true decide FUN_80101c7c (@0x80101c8c-cf4, f960[0]; Rest == f840) war als `re15_ai_decide_approach` portiert aber tot. case 2 an case-0 gekoppelt; Scope 91 beh-0x02-Records / 23 Räume (inkl. 1140-Re-Entry, vorher 5 Statuen). Szenario A (ohne Cutscene) war byte-true (gemessen). Regressionen `unit_zombie_10d0_reentry_a/_b` | `f6bdfef0`/`0887b138` |

| 2026-08-04 | **Nutzer-Runde 5 (3 Punkte, alle Findings adversarial CONFIRMED)**: (1) **Marvin-Gleiten am Cutscene-Ende GELÖST** (`feafc409`, `analysis/marvin_glide_end.md` D1-D4): Der Abgang ist Plc_dest **Mode 5 = RUN-Sub @0x80051484** — Align-Abschluss setzt `+0x94=0` (@0x8005157c) = **RUN-Gait Clip 0** (Bank 1, 22f); der Port lief den 30f-Walk-Gait bei 200/Tick + fror bei Ankunft als Statue ein (Original: Sub 6 Geste+Idle). Fix global: NPC-Plc_dest (0x40-0x4d außer Elliot) läuft jetzt byte-true in der State-4-Sub-VM (`state=4/+0x5=mode` @0x80041c14-18), `re15_npc_sub_walk` = volle 3-Phasen-FSM mit Per-Sub-Speed-Tabellen @0x80076c00/20/60/80, Phase-2-Slew 48 @0x80076c01, Arrival→Sub-6. Probe-Finale == Original-Sequenz. (2) **Title-Confirm** (`c4094a25`, `analysis/title_fade_voice.md`): Fade-out war 0,53 s statt **~5,45 s** (3 Phasen @0x80102ccc: Weiß-Blitz additiv 16 T → subtraktiv +1.75/T 147 T → Schwarz halten; Tick = 2 Vsyncs gemessen; alte 32×(+8)-Schleife = Rate-Defekt der Card-Fade-Klasse) + **„Biohazard 2!"-Stimme** = CORE11-SE 0 (Stereo VAG5/6 44,1 kHz, FUN_80045024(0x04000000,0) @0x80102c20 VOR dem Fade, jeder Menüpunkt) + Cursor-SE 4; `re15_audio_prime_core` (Title 0x11 @0x80102704, Game-Start Charakter-Bank @0x800316d8: Leon CORE00/Elza CORE04), Tone-Pan im Mixer (SsUtKeyOnV.c). Fade-in ebenfalls byte-true (−8/T, 32 T @0x80102054). (3) **Charakterauswahl-Infoblock** (`383fb0d8`, `analysis/pselect_info_bg.md`): Idle war schon pixel-identisch (Badges AUCH im Original opak, Savestate-FB-Beweis) — die schwarzen Kästen entstanden in der **Confirm-Phase**: Original schaltet auf das ADDITIV-Prim (Code 0x66 @0x80100e68, ABR1) mit Rampe Counter<<2 → Badges unsichtbar; Port stempelte schwarz-modulierte opake Kästen. Fix: FLAG/Counter gespiegelt, ADD-Blend + Rampe, Namens-td-Fade entfernt (kein Group-B-RGB-Write im Original, Voll-Store-Scan). Readback-Verifikation: Title + Player-Select rendern korrekt (gdigrab in dieser RDP-Session nicht komponiert). OFFEN: Live-Hörprobe der Stimme (RDP ohne Audio-Endpoint), gdigrab beim Nutzer | `feafc409`/`c4094a25`/`383fb0d8` |

| 2026-08-04 | **Marvin-Report #5 verifiziert (kein Defekt gefunden)** (`6dc6bea0`): Verifikations-Kette bis in die GEPOSTE Ausgabe geschlossen — probe_em40_gait (Bank-1-Clips 5/0 sind echte Zyklen, 46-61/80 KF-Bytes/Frame) + `unit_marvin_render_pose` (Render-Pfad-Posen: WALK 25,6 units/Frame/Bone max 185, RUN 106,8 max 406, Wurzel-Y konstant auf Hüfthöhe). Neue Debug-Hooks `RE15_SUBSTART="N@F"` + `RE15_FORCE_EVENT="N@F"`. GELÖST: Nutzer bestätigt 2026-08-04 „Marvin scheint erst mal ok zu sein" (Report #5 war der veraltete Build ohne `feafc409`). Rest-OFFEN nur noch: Live-gdigrab der Szene fürs Archiv (GOTO-Spawn wirft Auto-Prompt „shutter switch", blockiert das Harness). **Schreibmaschinen-Sounds** (`52c0b521`, `analysis/typewriter_sounds.md`, TW-1..4 CONFIRMED): Original ist beim Text-Tippen UND an den Save-Punkten (Telefon/Computer, 7 Räume byte-gedumpt) KOMPLETT stumm — Port korrekt; die eine reale Lücke war der Save-Screen als einziges Menü ohne Bank-4-Beeps: Cursor/Confirm/Cancel (0x0404/6/5) nach dem RE2-MEM_CARD-Vorbild (24 Sites) verdrahtet, SE-Log-verifiziert | `6dc6bea0`/`52c0b521` |

| 2026-08-04 | **Adress-Coverage-Zensus regeneriert** (auf Nutzer-Wunsch; `analysis/coverage/*.py`): **5431 unique zitierte Original-Adressen** (Top-Datei enemy_ai_common.c mit 2745); EXE **257/386 (66,6 %, +7 seit 2026-08-02)**, STAGE1 **431/650 (+11)**, STAGE2-6 329/456 · 362/526 · 343/459 · 357/535 · 50/73, 190 DAT-Globals. Status-Snapshot-Block im Kopf von `PORTING_ROADMAP.md` ergänzt (funktional spielbarer Umfang + Offen-Liste) | *(dieser Commit)* |

| 2026-08-04 | **Confirm/Cancel-Belegung (Nutzer-Report Load-Screen)** (`analysis/confirm_cancel_mapping.md`, CC-1..6 alle CONFIRMED): Vollzensus der Pad-Bits pro Menü — RE1.5 ist **game-weit □-Confirm/✕-Cancel** über das VIRTUAL-Wort DAT_800ac76c (Type-A @0x80073dbc[14]=0x0080=□, [15]=0x0040=✕); Card-FSM FUN_80025c00 bestätigt mit v0x4000|RAW-START (@0x80025e84/e98), bricht mit v0x8000 ab (@0x80025ea0) — **Port-Bits waren byte-true**, das RE2-MEM_CARD-Vorbild (✕-Confirm @0x801c08a8) wird zu Recht NICHT übernommen. Der Nutzer-Eindruck ist original-echt: Title nimmt JEDEN Face-Button (0x8f0 @0x80102c14), einen Screen später ist ✕ Cancel. CC-3 (Card-Screen umgeht die OPTIONS-Button-Config) war als Fix eingebaut (`vp` via `pc_pad_config`), ist aber **am 2026-08-05 auf Nutzer-Wunsch zurückgenommen** — der auslösende Report entpuppte sich als RDP-Eingabeproblem, nicht als Port-Fehler; der Card-Screen liest wieder das rohe Pad. Die RE-Belege bleiben im Dossier gültig, CC-3 ist damit wieder ein OFFENER (belegter) Punkt. Beifang, der bleibt: alte „MSG-Confirm=CROSS"-Etikettierung byte-widerlegt (Memory korrigiert), save_mode-Doku-Inversion gefixt | *(dieser Commit)* |

| 2026-08-06 | **Fehlerliste des Nutzers (5 Punkte) abgearbeitet** — Workflow `bugstack-5-re` (5 RE-Agenten + 5 adversariale Prüfer, alle Findings CONFIRMED oder als PLAUSIBLE/REFUTED korrigiert). Dossiers `analysis/bug_{load_after_death,room1030_cutscene,room1040_switch,room1070_wake,save_room_name}.md`. **(1) Laden nach dem Tod:** der Devour-Kommandozustand überlebte Tod→Title→LOAD (Teleport an den Weltursprung + zweiter Tod nach 34 Ticks); Raum-Init-Reset in den Boot-/Lade-Pfad eingesetzt (`re15_enemy_reset` + neue `re15_player_cmd_reset`), Port-Gegenstück zu `sw zero,0x800aca58` @0x80031518 / `sb zero,0x800aca58` @0x8001cbdc. **(2)+(3) ROOM1030/1040:** gemeinsamer Kern — der Port fuhr sub01 nur EINMAL beim Raum-Eintritt statt in jedem Gameplay-Frame (FUN_8003f038 @0x8003f064-84) und wischte Flag-Bank-5-Wort-1 nie (FUN_8003ebf4 @0x8003ec1c aus FUN_8003f0a0 @0x8003f18c); dadurch tat ROOM1040s Schalter beim Drücken nichts und detonierte stattdessen beim nächsten Betreten, ROOM1030s Cutscene feuerte nie live. Dazu `flag(2,7)` beim Raum-(Re)Load löschen (einziger Leser ist der `cine_active`-Proxy = Port-Gegenstück zum Routinenregister @0x8001CBDC) → kein permanenter Kontrollverlust mehr. **(5) ROOM1070:** Grid-Nibble 9/10 (der Skript-Wake, @0x8011f80c[9]/[10]=0x801019f0) war gar nicht dispatcht + der globale Anim-Advancer pinnte den Aufsteh-Clip auf Frame 0 — beides gefixt, gilt für 12 Räume. **(4) Save-Ortsname: KEIN Port-Fehler** — `FUN_80026e4c` ist in der ausgelieferten EXE ein `return 0`-Stub (@0x80026e4c `jr ra` / `addu v0,zero,zero`, zwei Aufrufer, null Datenzeiger — selbst nachverifiziert), das Original zeigt ebenfalls immer „Irons' Office"; nur der Code-Kommentar trägt jetzt den Beleg | *(dieser Commit)* |

| 2026-08-06 | **Kamera-Reports (ROOM1030 Wiederbetreten + Treppenhaus) — beide vom Nutzer bestätigt** (`analysis/bug_camera_1030_stairs.md`, `analysis/stair_camera_1060.md`). **(a) ROOM1030:** `op_cut_replace` (0x4B) machte aus dem ZEIGER-Fixup `LAB_80040414` einen echten Kamerawechsel und verdrängte über `cam_change_pending` den Tür-Eintritts-Cut → beim Wiederbetreten zeigte die Kamera die Zombie-Halle (Cut 9 ≡ Cut 0) statt den Spieler. Byte-Beleg: der Schwanz liest @0x800404ac-b8 das `cam_from` des Ankers `0x800ac794` NACH der Tausch-Schleife (@0x8004044c-a8, Stride 20) und ruft `FUN_800142f4(b)` nur, wenn es JETZT `a` trägt — schreibt also den angezeigten Wert zurück (`sb a0,0x800afbb5` + `sw v0,0x800ac794` @0x80014300/10). Block gestrichen; `test_cut_replace_live_switch` pinnte den Defekt und ist zu `test_cut_replace_no_camera_change` umgeschrieben. Der Defekt ist alt (eb4322ec), war aber unerreichbar — e53fb043 hat ihn über den sub01-Reseed scharf geschaltet. **(b) Treppenhaus ROOM1060:** der Zonen-Scan `FUN_80014230` ist eine ZUSTANDSMASCHINE (`lbu v1,22(a0)` @0x80014254) mit Punkt-Test pro Frame (@0x80014298) — ein verpasster Übergang ist endgültig und blockiert die Folgezone. Der Gait trug 3761 statt der nötigen 3820/3920 Einheiten, weil der Pose-Cursor VOR statt NACH der Abtastung inkrementiert wurde (byte-true: `lbu v0,149(t0)` @0x8001f35c liest unverändert, `addiu v0,v0,1` @0x8001f618 erst danach). Nach der Korrektur 4134/4355. Port-unabhängiger Beleg: der alte Gait überspannte sein eigenes Auslöse-Rechteck (4300/4400 laut RDT) nicht. Zusätzlich Treppen-Trigger byte-true gegatet (Positions-Test nur bei sce_flags 0x40 @0x80042ea8, Sonde bei 0x20 @0x80042ef8). **Widerlegt:** der Finalize-Eject war NICHT die Ursache (zwei Trial-Patches, identisches Ausfallfenster), und der vermeintliche Clamp in `FUN_8003da78` ist ein Achsen-Selektor. **Messtechnik-Lehre:** zwei Sonden maßen unrepräsentativ (Treppe rückwärts aus dem Raum / Sweep in der Wand) und erzeugten die falsche Aussage „die Kette bricht immer" — es war ein Ausfallfenster (Ost 23 %, West 63 %), was das „teilweise" des Reports erklärt | *(dieser Commit)* |

Baseline nach der Session: **114/114 ctest grün** (neu: `unit_cam_1030_reentry` — der Tür-Cut muss sich gegen alle 13 Rest-Cuts des Vorraums durchsetzen).

Baseline davor: **113/113 ctest grün** (neu: `unit_load_after_death`, `unit_room1040_switch`, `unit_room1070_wake`). Live-Boot gegen einen HEAD-Baseline-Build frame-für-frame verglichen (`[walk]`-Spur F0..F300 + alle `[scd] thread start` identisch).

Baseline der Vorsession: **110/110 ctest grün** (neu u.a.: `unit_crow_death_corpse`, `unit_door_1170_lock`, `unit_marvin_spawn_bank`, `unit_zombie_10d0_reentry_a/_b`, `unit_marvin_render_pose`).

### Offene Punkte (oberste Zeile = nächster Schritt)

0. **Live-Verifikation der Fehlerlisten-Fixes vom 2026-08-06** — die ctest-Gates beweisen die
   Logik, nicht das Bild: ROOM1040 (Schalter drücken → Frage; Raum neu betreten → keine Frage),
   ROOM1030 (Cutscene feuert beim Zombie-Durchlauf, Kontrolle kommt zurück), ROOM1070 (fünf
   Zombies stehen am Trigger-Punkt auf) und ein Tod→Load-Zyklus, jeweils im echten Fenster per
   gdigrab (`re15-port-visual-verify`). Diese Session hatte keinen Audio-Endpoint (RDP).
0b. **Zwei bewusst zurückgestellte, belegte Divergenzen aus derselben Runde** (nicht Teil der
   gemeldeten Fehler, deshalb nicht als Beifang eingebaut):
   (a) `work_vars[0..3] = -1` am Ende jedes VM-Laufs (dieselbe FUN_8003ebf4 @0x8003ebfc-ec14) —
   macht die Skript-Scratch-Variablen zu Ein-Frame-Werten und lässt 6 Assertions in
   `unit_scd_opcodes` fallen; braucht einen eigenen Durchgang.
   (b) Save-Slot-Zeile ~28 px zu breit (`analysis/bug_save_room_name.md` F2/F4): das `05 00` des
   Templates ist Farb-Op MIT Argument, und `re15_render_pc_game_codes` rückt für Code 0x00 hart
   8 px vor statt der 4 px aus der Breitentabelle @0x800c4416.
1. **Live-/Hör-Verifikation der drei W1-Fixes** — die Unit-Pins beweisen die Logik, nicht das
   Bild/den Klang: (a) Rolltor-Hörprobe (diese Session hatte KEINEN Audio-Endpoint — RDP);
   (b) Zombie (a)/(b) und Krähen-Verhalten im echten Fenster per gdigrab
   (`re15-port-visual-verify`), Route/Aufruf steht in HANDOVER_2026-08-01.md §2b.
   Für die Rolltor-Messung: `RE15_NO_INTRO=1 RE15_GOTO_ROOM=1130 RE15_FORCE_EVENT=2
   RE15_SE_DEBUG=1` → `debug.log` muss `[se] voice: se=12/10/11 … pitch=0x200/0x1a5/0x196`
   zeigen (die neue Voice-Log-Zeile).
2. **SE-Pitch-Sweep** (Folge von W1.3): der Tone-Pitch gilt jetzt für ALLE SEs — Footsteps
   u.a. klangen bisher nur richtig, wo note≈center−12 zufällig passte. Sweep über alle
   snd0/snd1/ARMS/CORE-EDTs nach |Δ|≠12 und Stichproben hören (Dossier rolltor_sound.md §6 D1).
3. **Krähen-Rest** (Dossiers crow_1170.md §6 + crow_victim_anim.md §7 + crow_death_pool.md §4):
   D7 `0x800acc0c`-Konsument (FUN_80024c30 RE'en), D8 Root-Post-Pass für ALLE States
   (+ b544-Push-Loop), D9 Arrival-Frische (erst `0x8001a804`-Tail disassemblieren),
   `aca52`-Producer (EXE-Knockdown @0x800334e8-504/@0x800345c8 + Cmd-FSM @0x80073f90),
   ROOM1171-Wurf-SZENE bauen (Rear-Grab + Mode-6-Halt; Exit = Script-`Plc_dest` —
   crow_victim_anim.md §3; vorher Plc_dest mode 0x13 @0x80073e30[19] disassemblieren),
   visuelle Identität der EM021-victim Clips 0/1/2 + PLW 1/2 per gdigrab, Live-Parity-
   Savestate (JUMP 1170 mit z3-Bit-125); NEU aus crow_death_pool.md §4: Part-Scatter-
   Mover/-Renderer der 13 GIB-Bone-Parts (Kandidat Skelett-Draw), fx-Identität ESP
   0x08032000 (Waffen-14-Lane), word0-Bit-0x2/0x40-Konsumenten-Census, PSX-Live-Capture
   der Lachen-Optik (Subtraktiv-Beweis stützt sich auf den Spieler-Pool), gdigrab der
   liegenden Krähe + Lache. *(Victim-Anim des Front-Grabs + Flinch-Gate/-Richtung +
   Corpse-Freeze/Lache/GIB-Wipe: ERLEDIGT 2026-08-03.)*
4. **Zombie-Rest** (Dossier zombie_hit_1140.md §5): Wake-Trigger des Lyers (wer setzt +0x6=1),
   Waffen-IDs 0x12/0x13 (Prone-Flinch-B-Klasse), DEATH-Spalte 4 Producer (FUN_80107634),
   schwere Stagger-Handler FUN_80106290/80106624/80106048.
5. **Rolltor-Rest** (Dossier rolltor_sound.md §7): PSX-Gegenprobe per SpuVmKeyOnNow-Watchpoint
   @0x80055D10 (Skill `re15-pcsx-watchpoint`), Spielfluss-Nachweis ohne FORCE_EVENT
   (Hof-Hindernis bei x≈−17000, HANDOVER §2b), Positional-Pfad FUN_80045a64 (D3), ADSR (D4).
6. **W2-Sweep starten** (§2): ROOM1240 zuerst, Checkliste pro Raum.
7. **Cutscene-/NPC-Rest** (marvin_10d0.md §6 + cutscene_headlook.md §6 + marvin_spawn_anim.md
   open_questions): (a) ROOM10D0 visuelle gdigrab-Prüfung + DuckStation-Gegenprobe (Bank-1-
   Posen vs r10d0_walk1-VRAM); (b) ROOM1170/Elliot-Marker-Messung (Original bindet REC1)
   vor Umstellung der Elliot-Ausnahme; (c) `Plc_dest` mode 0x13 (@0x80073e30[19])
   disassemblieren; (d) FUN_8003703c-Kategorien verhaltens-verifizieren; (e) Sweep-Modi 3/4
   Live-PSX-Vergleich (ROOM11B1 sub02); (f) 0x1910 Extra-Bytes nach dem 10D0-RBJ-Trailer;
   (g) Bank-0-Kanal +0x84/+0x16c game-weit kartieren (Plc_motion(1,…)-Fundstellen; Subs 7/8
   ungenutzt in STAGE1?); (h) State-1-Idle-Kanal der NPCs messen (Port: eigene Bank —
   INIT-f314 läuft auf +0x84/+0x16c, der State-1-Spielkanal ist nicht disasm-belegt).
