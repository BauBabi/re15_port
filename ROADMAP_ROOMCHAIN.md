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

Baseline nach der Session: **103/103 ctest grün.**

### Offene Punkte (oberste Zeile = nächster Schritt)

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
3. **Krähen-Rest** (Dossier crow_1170.md §6): D7 `0x800acc0c`-Konsument (FUN_80024c30 RE'en),
   D8 Root-Post-Pass für ALLE States (+ b544-Push-Loop), D9 Arrival-Frische (erst
   `0x8001a804`-Tail disassemblieren), `aca52`-Producer (EXE-Knockdown @0x800334e8-504/
   @0x800345c8 + Cmd-FSM @0x80073f90), Victim-Anim des Krähen-Grabs (Player-Cmd-5
   @0x80036834), Live-Parity-Savestate (JUMP 1170 mit z3-Bit-125).
4. **Zombie-Rest** (Dossier zombie_hit_1140.md §5): Wake-Trigger des Lyers (wer setzt +0x6=1),
   Waffen-IDs 0x12/0x13 (Prone-Flinch-B-Klasse), DEATH-Spalte 4 Producer (FUN_80107634),
   schwere Stagger-Handler FUN_80106290/80106624/80106048.
5. **Rolltor-Rest** (Dossier rolltor_sound.md §7): PSX-Gegenprobe per SpuVmKeyOnNow-Watchpoint
   @0x80055D10 (Skill `re15-pcsx-watchpoint`), Spielfluss-Nachweis ohne FORCE_EVENT
   (Hof-Hindernis bei x≈−17000, HANDOVER §2b), Positional-Pfad FUN_80045a64 (D3), ADSR (D4).
6. **W2-Sweep starten** (§2): ROOM1240 zuerst, Checkliste pro Raum.
