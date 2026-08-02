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

### W0 — Struktur & Inventar *(Session 2026-08-02)*
- [x] Analyse-Workflows: Adress-Coverage-Zensus + 3 RE-Dossiers (Krähe/Zombie/Rolltor)
- [x] Artefakt-Cleanup (~1,3 GB shots-Scratch, Root-Logs, .sentry-native, __pycache__;
      .gitignore erweitert, damit es nicht wieder aufläuft)
- [x] Diese Roadmap; lesson.txt-Regel verankert (§0)
- [ ] RE15_ADDRESS_COVERAGE.md erzeugt und geprüft
- [ ] Dossiers geprüft (adversarial verifiziert) und in §5 eingetragen

### W1 — Die drei gemeldeten Symptome fixen (in dieser Reihenfolge)
Jeder Fix folgt §0: Beleg → Code (`@0x…` je Konstante) → Messung → ctest-Pin.

- [ ] **W1.1 Zombie-Hit-Reaktion 1140** — Dossier `analysis/zombie_hit_1140.md`.
      Akzeptanz: für Symptom (a) und (b) steht dokumentiert, was das Original in exakt der
      Situation tut (Adressen), der Port tut dasselbe, und ein room-probe-/AI-ctest pinnt
      beide Übergänge. Verifikation zusätzlich visuell per gdigrab (Skill
      `re15-port-visual-verify`), NICHT per Autoshot.
- [ ] **W1.2 Krähen-KI 1170** — Dossier `analysis/crow_1170.md`.
      Akzeptanz: alle Divergenzen der State-Machine gefixt oder als byte-true widerlegt;
      ROOM1170-Spawn-Parameter (Sce_em_set-Operanden) stimmen mit RDT-Bytes überein;
      Flugverhalten im Live-Fenster gegen PSX-Capture verglichen.
- [ ] **W1.3 Rolltor-Sound** — Dossier `analysis/rolltor_sound.md`.
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
| 2026-08-02 | Kampagne aufgesetzt: Workflows (Coverage-Zensus + 3 RE-Dossiers), Cleanup, diese Roadmap | *(diese Session — Ergebnisse folgen unten)* |

### Offene Punkte (oberste Zeile = nächster Schritt)
1. *(wird nach Workflow-Abschluss dieser Session gefüllt)*
