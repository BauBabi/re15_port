# BEFUND — Spinnen-Todeszyklus: „komische Dinge kommen raus, dann stellt sie sich wieder auf, dann legt sie sich wieder hin"

Nutzer 2026-09-13 (ROOM2090, Marker r7_befund_2090_F692_marke9.png / r7_befund_2090_F2356_marke10.png).
Statische Analyse; alle Zitate selbst gezogen aus `info/re2leon/COMMON/BIN/EMS25.BIN`
(Overlay RAW @0x80100000), `ghidra_re2_Leon.txt` (RE2-EXE FUN_80047030 / FUN_8002959c /
FUN_80029614, verbatim), `shared_assets/RE2/CDEMD0.EMS` (EMD kind 0x25 @Datei 0x3A7800,
Sektor 0x74F, 0xD0C8 B — EDD/EMR selbst gedumpt) und dem Port-Quelltext.

**Kurzfassung: drei getrennte Antworten.**
1. Die „komischen Dinge" sind die **byte-true Baby-Spinnen** (6–9 Stück, Typ 0x26) — original so.
2. Das „Aufstehen + Wieder-Hinlegen" ist ein **Port-Defekt ohne jede Nutzereingabe**: nach dem
   Clip-12-Wrap in CORPSE-Phase-0 steht `anim_frame = 0`, und der Port posiert **stateless aus
   (motion, anim_frame)** — er zeigt für die 30–61 Timer-Ticks der Phase 1 das ERSTE Bild des
   Todes-Clips (Keyframe 158 = aufgebäumt, Körper-Y −681) statt der gehaltenen Liege-Pose
   (Keyframe 207, Körper-Y −278). Das Original hält die zuletzt KOMPONIERTE Pose in den
   model_inst-Matrizen — der Frame-Zähler wrappt dort genauso auf 0, ist aber render-irrelevant.
3. Zusätzlich (zweiter, treffer-abhängiger Kanal): der Port lässt die sterbende Spinne
   (state 3, HP < 0) weiter als Schuss-Ziel zu — das Original schließt **HP < 0 kategorisch aus**
   (@0x80047148-50). Jeder Nachtreffer im Port stempelt DEATH neu und spielt Clip 12 von vorn.

---

## 0. Marker

* **marke9 (F692)**: die Adult liegt links als Leichen-Haufen (RE1.5-Hybrid-Geometrie unter
  RE2-Rig, gestreifte Beine) — die korrekte End-Pose des Todes-Clips. KEINE schwarzen Dreiecke
  mehr: der Runde-4-Fix (kein Hybrid für 0x26, main.c:866) ist wirksam; Babys sind auf dem
  Standbild nicht (mehr) im Bild.
* **marke10 (F2356)**: YOU-DIED-Einblendung (Gator-Tod); am unteren Rand eine Spinne in
  AUFGERICHTETER Pose von oben — konsistent mit der eingefrorenen kf158-Aufbäum-Pose aus §2
  (kann auf dem Standbild nicht sicher von einer zweiten, lebenden Spinne unterschieden werden).

---

## 1. RE2-Original: die komplette DEATH→CORPSE-Kette (KLÄRE 1)

Overlay = EMS25.BIN (kind 0x25, TOC-Records 0/1 = Overlay, 2 = TIM, 3 = EMD; Binder
FUN_8001aaa8 @0x8001ab4c-80).

### 1.1 DEATH endet am EDD-Frame-Flag (Runde 4, unverändert gültig)

* generische Zeile: `lw v0,376(s0) / lw v0,0(v0) / lui v1,0x8 / and / beq` @0x80103E10-24
  (Flag 0x80000, in Clip 12 ab f180) → `sw 7 → +0x4` @0x80103E9C; setzt bei +0x239==0
  **HP = 1** @0x80103EB0 und **+0x224 = 1** @0x80103EB8.
* Sonderzeile (Schrot w8→7, Magnum w7→5; Zeilen 5/6/7/8/17): dieselbe Leiter mit Maske
  **0x40000** @0x801045B8-D4 (in Clip 12 ab f12) → `sw 7` @0x80104644; setzt **weder HP=1
  noch +0x224** — die Schrot-Leiche behält ihr negatives HP.
* Frame-Flags erneut selbst gemessen (CDEMD0.EMS kind 0x25, EDD-Paar dir[1]@0xC — das
  einzige nicht-leere Paar, Port-Bestwahl trifft es zwangsläufig: dir[3]/dir[5]-EDDs führen
  0 Clips): Clip 12 (206 f): f0-11 keine Flags, f12-179 0x40000, f180-205 0xC0000.
  Clip 13 (68 f): nur f67 trägt Flags (Wort 0xF00000CE = Bits 0xF0000000, kf 206).

### 1.2 CORPSE @0x80104CF8 — byte-für-byte verifiziert (Raw-Dump EMS25.BIN, s.u.)

```
80104d08 lbu v0,5(s0)            ; +0x5 Phase
80104d10 bne v0,zero,0x80104d40
80104d18 jal 0x8002959c          ; Advance (a3=512)
80104d20 beq v0,zero,+..         ; done-gegatet am RUECKGABEWERT
80104d28 jal rand / sb v0,5(s0)  ; Phase 1; Timer:
80104d30 andi 0x1f / addiu 30 / sb v0,0x16a(s0)   ; +0x16A = (rand&0x1F)+30
80104d40 lbu/addiu -1/sb +0x16A  ; Phase 1: Timer--
80104d54 bne .. (Timer!=0)
80104d58 lui 0x7 / ori 0xD / sw v0,0x14c(s0)      ; Clip 13, Rate 7
80104d64 sb zero,5(s0)           ; zurueck in Phase 0
```

Ablauf im Original: Phase 0 spielt den REST des Todes-Clips (ab f12 bzw. f180) bis zum
Wrap; dann Timer 30–61 Ticks; dann Clip 13; danach wieder Timer, wieder Clip 13 — endlos.

**Steht die Spinne je wieder auf? NEIN.** Selbst gemessen am EMR (kind 0x25, Header
{128,228,20,104}):
* Clip 12 nutzt Keyframes 158→207 (Kollaps); Root-Y: kf158 = **−681** (aufgebäumt, höher
  als der Normal-Stand kf0 = −400), kf200–207 = **−292…−278** (liegend; kf205–207 = −279/−278/−278).
* Clip 13 (der Zuck-Clip) nutzt AUSSCHLIESSLICH Keyframes **202–207** — Mikro-Zucken in der
  Liege-Pose; sein f0 ist kf207 = exakt die End-Pose von Clip 12.
Kein CORPSE-Pfad setzt je wieder einen Clip mit kf < 202. Die Original-Leiche bleibt liegen.

---

## 2. HAUPTDEFEKT (kein Input nötig): Wrap-Pose statt gehaltener End-Pose

### 2.1 Original: die Pose ist ein GEHALTENES Artefakt, nicht (clip, frame)-Lookup

FUN_8002959c / FUN_80029614 (RE2-EXE, verbatim aus ghidra_re2_Leon.txt):

* @0x800295D0-F8: `+0x178` = Frame-Wort-Zeiger aus dem AKTUELLEN `+0x14D`.
* @0x80029658-0x80029B24: die Pose DIESES Frames wird in die model_inst-Blöcke komponiert
  (`lw s0,0x198(s2)` @0x8002966C; Rotationsvektor → `RotMatrix(s0+0x68 → s0+0x18)`
  @0x80029B14; Stride 0xAC pro Bone).
* ERST DANACH @0x80029B28-4C: `lbu +0x14D / addiu 1 / sb`; `sltu v0,frame,s3`
  (s3 = Clip-Länge, `lhu s3` @0x80029680); am Ende `sb zero,+0x14D` + return 1.

Auf dem done-Tick steht also die Pose von **f205** in den Matrizen, der Zähler auf 0.
Während der Timer-Phase ruft NIEMAND 0x8002959c → der Renderer zeichnet weiter die
gespeicherten Matrizen = die Liege-Pose. Der genullte Zähler ist unsichtbar.

### 2.2 Port: der Renderer posiert jeden Frame NEU aus (motion, anim_frame)

* `re15_re2_advance_959c` (enemy_ai_re2_dog.c:197-207): am Wrap `anim_frame = 0` + done —
  numerisch original-gleich.
* ABER platform/pc/main.c:8149-8155: `ncur = anim_frame` (bzw. `>>1`) →
  `re15_compute_actor_kf(...)` → `re15_skel_compute_pose(...)` — **stateless pro
  Render-Frame**. Es gibt keine gehaltene Matrix.
* re2s_corpse (enemy_ai_re2_spider.c:2418-2437): Phase 0 `if (re2s_advance(e,512)) {
  sub_state_1=1; Timer }` — nach dem done steht `anim_frame = 0` und bleibt es die ganze
  Timer-Phase.

**Sichtbare Folge (jeder Spinnen-Tod, ohne weiteren Treffer):**
1. Kollaps läuft (DEATH ab Flag in CORPSE weiter) — Spinne legt sich hin. ✓
2. Wrap-Tick: anim_frame=0 → Renderer zeigt Clip-12-Bild-0 = **kf158, Körper-Y −681 =
   AUFGEBÄUMT** — und hält das 30–61 Ticks (1–2 s): „**dann stellt sie sich wieder auf**".
   (Der Crossfade-Decay glättet den Sprung — es sieht nach echtem Aufstehen aus.)
3. Timer abgelaufen → `re2s_clip(0x0007000D)` (Clip 13, frac 7): Crossfade zurück in die
   Liege-Pose: „**dann legt sie sich wieder hin**".
4. Ab jetzt unauffällig: Clip-13-Wraps landen auf f0 = kf207 = liegend — nur EIN
   Aufsteh-Zyklus pro Tod. Exakt der gemeldete Ablauf.

### 2.3 Antwort auf KLÄRE 3 (mehrfaches Feuern des Flag-Übergangs)

Nein. Nach `re2s_word(e,7)` dispatcht die Wurzel (enemy_ai_re2_spider.c:2981, `case 7`) auf
`re2s_corpse`; die DEATH-Zelle mit den Flag-Gates wird ohne neuen Stempel nie wieder
betreten. Ein Pendeln 3→7→3 über das Flag selbst ist unmöglich. Der Pendel-Eindruck
entsteht aus §2.2 — plus dem Treffer-Kanal in §3.

---

## 3. NEBENDEFEKT (treffer-abhängig): HP<0 ist im Original KEIN gültiges Ziel

RE2-EXE, Kandidaten-Schleife des Spieler-Angriff-Appliers (FUN_80047030, LAB_8004711C;
verbatim) — genau VIER Gates, KEIN Zustands-Gate:

```
8004712c andi v0,word0,0x1     ; aktiv?
80047138 lbu  v0,0x1d3(s0)     ; Treffer-Cooldown != 0 -> skip
80047148 lh   v0,0x156(s0)     ; HP
80047150 bltz v0,0x8004740c    ; HP < 0 -> KEIN ZIEL
80047158 lhu  v0,0x10e(s0) / andi 0xc000 -> skip
```

Stempel danach @0x8004727C-90: `lh +0x156 / bgez → sw 2,+0x4 / sonst sw 3,+0x4`
(das Wort-`sw` nullt +0x5/6/7 mit).

**Original-Konsequenz:**
* Während des GESAMTEN Sterbens (HP < 0) ist die Spinne unbeschießbar → die
  DEATH-P0 (Clip-12-Neustart + 6–9 Babys @0x80104590-A4) kann NIE erneut zünden.
* Die generische Leiche (HP = 1 @0x80103EB0, +0x224 = 1 @0x80103EB8) IST beschießbar:
  neuer Treffer → HP < 0 → state 3 → `+0x224 != 0` → ABSCHLUSS-AUSBRUCH @0x80104440:
  Clip 13 (@0x80104460), **1–4 Babys** (@0x80104470-78), Gore, zurück in 7 — die Leiche
  zuckt und platzt, steht aber nie auf. Danach HP < 0 → endgültig unbeschießbar.
* Die Schrot-/Magnum-Leiche (Sonderzeile, HP bleibt < 0) ist ab dem Todesschuss endgültig
  unbeschießbar.

**Port-IST (re15_damage.c):**
* Gun-Pfad :1221 `if (e->state == 7) continue;` — Port-Erfindung: blockiert den
  byte-true Leichen-Ausbruch der generischen Zeile.
* :1234 HP<0-Gate nur für Typ 0x26 — die STERBENDE Adult (state 3, HP<0, bis f12 bzw.
  f180) bleibt Ziel: jeder Nachtreffer (Dauerfeuer!) stempelt `state=3, sub_state_2=0`
  (Stempel-Kommentar :1872-1878, @0x80047288/90) → P0 zündet neu: Clip 12 von f0
  (= Aufbäumen + Kollaps) und bei Schrot-Zeilen **6–9 WEITERE Babys**.
* Messer-/Hitbox-Pfad `re15_resolve_attack` → `re15_enemy_take_damage` (:1893-1918) hat
  überhaupt kein HP-/state-Gate (GATE B inert, :2237-2243).

---

## 4. Die „komischen Dinge" (KLÄRE 2): original so

* Der Schrot-/Magnum-Tod stößt byte-true **6–9 Baby-Spinnen** aus:
  `FUN_80105D38(self, 0x2002, (rand&3)+6)` @0x80104590-A4, Kappe 18 lebende
  (`sltiu 0x12` @0x80105DB0); Port `re2s_spawn_babies` (enemy_ai_re2_spider.c:1722-1760)
  deckungsgleich. Die Abschluss-Ausbrüche spawnen weitere 1–4 (@0x80104470-78) bzw. 6–9
  (@0x80104828-30).
* Seit Runde 4 laufen die Babys auf dem ECHTEN RE2-EM026-Modell + eigener TIM
  (Hybrid-Ausschluss main.c:864-867; TOC kind 0x26: EMD Sektor 1921, TIM 1904) — die
  „schwarzen Dreiecke" sind Geschichte (Marker marke9 bestätigt: keine Dreiecke).
  → **Dokumentieren als „original so"**: kleine krabbelnde Spinnen aus der sterbenden
  Adult sind RE2-Retail-Verhalten.
* Todes-Gore: Original `FUN_801056DC(self,19,1)` @0x80104580 + Spray-Bündel
  `FUN_80104890` @0x80104588; der Port nähert beide als deklarierte RE1.5-ESP-Blut-Quads
  (`re2s_gore` → `re15_esp_fx_spawn_ex(..., 0x1500, ...)`) — bekannte, deklarierte
  Näherung, kein neuer Defekt.
* ABER: durch §3 spawnt der Port bei jedem DEATH-Neustempel einer Schrot-Zeile 6–9
  ZUSÄTZLICHE Babys (bis Kappe 18) — der Nutzer sieht also u.U. MEHR „komische Dinge"
  als das Original je zeigen würde. Fix von §3 behebt das mit.

---

## 5. PATCH-PLAN

### 5.1 CORPSE-Halte-Pose (Hauptdefekt) — enemy_ai_re2_spider.c:2421-2424

In `re2s_corpse`, Phase-0-done-Zweig:

```c
if (re2s_advance(e, 512)) {                        /* @0x80104D18-20 */
    /* HALTE-POSE: 0x8002959c komponiert die Pose des LETZTEN Bildes in die
     * model_inst-Matrizen (+0x198, RotMatrix @0x80029B14), BEVOR +0x14D
     * inkrementiert/genullt wird (@0x80029B28-4C). Waehrend des +0x16A-Timers
     * laeuft kein Advance -> das Original zeigt die GEHALTENE Liege-Pose
     * (Clip 12 f205 = kf207, Root-Y -278). Der Port posiert stateless aus
     * (motion, anim_frame) (main.c:8149-8155); anim_frame=0 zeigte kf158
     * (Root-Y -681, aufgebaeumt) fuer 30-61 Ticks = das gemeldete
     * "stellt sich wieder auf". Frame deshalb auf dem letzten Bild halten. */
    int fc = re15_actor_clip_len(e);
    if (fc > 0) e->anim_frame = fc - 1;
    e->sub_state_1 = 1;                            /* @0x80104D2C */
    e->re2z_dir16a = (uint8_t)((re2s_rand() & 0x1fu) + 30u);   /* @0x80104D30-3C */
}
```

`re2s_clip(0x0007000D)` in Phase 1 (:2427-2429) resettet anim_frame über das Frame-Byte
des Clip-Worts auf 0 (:166-173) — der gehaltene Wert leckt nicht in Clip 13. Der Fix
deckt auch die Clip-13-Wraps ab (f67 = kf206 statt f0 = kf207 — beide liegend, jetzt
konsistent „letztes Bild gehalten").

### 5.2 Byte-true Ziel-Gates (Nebendefekt) — re15_damage.c

* **:1221** (Gun-Pfad): die state-7-Pauschale für die RE2-Adult öffnen —
  `if (e->state == 7 && !(e->type == 0x25u && re15_ai_re2_for_type(e->type))) continue;`
* **:1234** auf die Adult ausweiten —
  `if ((e->type == 0x25u || e->type == 0x26u) && re15_ai_re2_for_type(e->type) && e->hp < 0) continue;`
  Beleg beider Zeilen: die vier Kandidaten-Gates @0x8004712C-30 / @0x80047138-40 /
  **@0x80047148-50 (HP<0)** / @0x80047158-64 — kein state-Gate im Original.
  Wirkung: (a) sterbende Spinne unbeschießbar → kein DEATH-Neustart, keine Zusatz-Babys;
  (b) generische Leiche (HP=1) wieder beschießbar → Abschluss-Ausbruch @0x80104440
  (Clip 13 + 1–4 Babys) wird original-treu erreichbar.
* **:1895** (`re15_enemy_take_damage`, Messer-/Hitbox-Pfad): gleicher Guard am Eintritt:
  `if ((e->type == 0x25u || e->type == 0x26u) && re15_ai_re2_for_type(e->type) && e->hp < 0) return 0;`
  (RE2 führt Messer-Treffer durch denselben Applier mit denselben Gates.)

### 5.3 Pins (Muster tests/unit/test_adult_spider_ai.c, Bank geladen)

1. Tod (Zeile 7) → state 7; CORPSE ticken bis Phase-0-done: `anim_frame == clip_len-1`
   (nicht 0) und `motion == 12`, solange `sub_state_1 == 1`; nach Timer `motion == 13`.
2. Gun-Resolver: Kandidat state 3 / HP<0 (Typ 0x25, RE2-owned) wird ÜBERSPRUNGEN.
3. Generische Leiche (state 7, HP=1, done224=1) + Treffer → `re2s_death_done_generic`:
   state 7, motion 13, 1–4 neue 0x26-Aktoren.

### 5.4 Bewusst NICHT anfassen

* Baby-Spawns, Kappe 18, EM026-Modellweg — original so (§4).
* Der Flag-Übergang der Runde 4 (funktioniert; EDD-Paar-Wahl verifiziert: dir[1]@0xC ist
  das einzige Paar mit Clips, `frames[]` führt die rohen 32-bit-Wörter, emd_common.c:92-93).
* `re2s_gore`-ESP-Näherung (deklariert) und die Fall-Modi 1/2/3 der Todeszeilen.

---

## FAZIT (Kurzform)

1. Die „komischen Dinge" sind die byte-true 6–9 Baby-Spinnen des Schrot-/Magnum-Todes
   (@0x80104590-A4, Kappe 18 @0x80105DB0) — seit dem Runde-4-Hybrid-Ausschluss
   (main.c:866) mit echtem RE2-EM026-Modell. Original so, keine Änderung.
2. Das „Aufstehen + Wieder-Hinlegen" hat ZWEI Ursachen, beide belegt:
   a) HAUPTKANAL, ohne Input: CORPSE-Phase-0 endet am Clip-12-Wrap mit `anim_frame = 0`;
      der Port posiert stateless aus (motion, anim_frame) (main.c:8149-55) und zeigt
      30–61 Ticks lang Clip-12-Bild-0 = kf158, Root-Y −681 = AUFGEBÄUMT, ehe Clip 13
      (kf202-207, liegend) es wieder hinlegt. Das Original hält die zuletzt komponierte
      f205-Pose in den model_inst-Matrizen (+0x198, RotMatrix @0x80029B14) — der
      Frame-Wrap @0x80029B48-4C ist dort render-irrelevant.
   b) NEBENKANAL, treffer-abhängig: das Original schließt HP<0-Entities als Ziel aus
      (@0x80047148-50); der Port nicht (nur 0x26) — Nachtreffer während des Sterbens
      stempeln DEATH neu (Clip 12 von vorn + 6–9 Zusatz-Babys je Schrot-Stempel).
3. Der Flag-Übergang DEATH→CORPSE selbst kann NICHT mehrfach feuern (Wurzel-Dispatch
   verlässt die DEATH-Zelle endgültig); die Runde-4-Umstellung ist intakt und die
   EDD-Paar-Wahl trifft das Flag-tragende Paar (dir[1]@0xC, einziges mit Clips).
4. Fixes: Halte-Frame in re2s_corpse (enemy_ai_re2_spider.c:2421-2424), HP<0-Gate statt
   state-7-Pauschale für 0x25/0x26 (re15_damage.c:1221/:1234/:1895) — dadurch wird
   nebenbei der byte-true Leichen-Ausbruch @0x80104440 (Clip 13 + 1–4 Babys) erreichbar.
5. Die Original-Leiche steht NIE wieder auf: CORPSE ist ein endloser Timer(30–61)/
   Clip-13-Zyklus, und Clip 13 lebt komplett in den Liege-Keyframes 202–207 (gemessen).

---

## NACHTRAG — unabhängige Zweitverifikation (2026-09-13, statisch)

Alle tragenden Zahlen dieses Dossiers wurden in einem zweiten, unabhängigen Messlauf
bestätigt (eigene Python-Parses auf CDEMD0.EMS/EMS25.BIN/PSX.EXE, verbatim-Greps im
ghidra_re2_Leon.txt):

* TOC kind 0x25 (RE2-EXE @Datei 0x8B5F4): EMD Sektor 0x74F (1871), 0xD0C8 B; dir@0xD0A8,
  8 Einträge; NUR Paar dir[1]@0xC führt Clips (16, Längen {16,8,16,31,21,6,10,17,5,29,
  55,42,206,68,32,32}) — dir[3]/dir[5]-EDDs: 0 Clips.
* Clip 12 (206 f): Flag-Runs exakt f0–11 keine, f12–179 0x40000, f180–205 0xC0000;
  Keyframes f0 = kf158 … f205 = kf207. Clip 13 (68 f): kf-Bereich 202–207, f0 = kf207,
  f67 = kf206 (Wort 0xF00000CE).
* EMR {128,228,20,104}: Root-Y kf0 = −400, kf158 = **−681** (aufgebäumt), kf200–207 =
  −292…−278 (liegend).
* CORPSE @0x80104CF8 raw (EMS25.BIN): 80104d18 `jal 0x8002959c`/a3=512, 80104d20
  `beqz v0` (done-gegatet), 80104d2c `sb 1,+0x5`, 80104d30-3C Timer (rand&0x1F)+30,
  80104d5c-60 `sw 0x0007000D,+0x14C`, 80104d64 `sb zero,+0x5` — wortgleich mit §1.2.
* FUN_8002959c: Pose-Komposition (RotMatrix @0x80029B14, Bone-Stride 0xAC @0x80029B24)
  VOR dem Zähler (`lbu +0x14D` @0x80029B28, Wrap `sb zero` @0x80029B48 + return 1) —
  die Halte-Pose-These aus §2.1 steht.
* Applier-Gates FUN_80047030: @0x8004712C andi 0x1, @0x80047138 lbu +0x1D3,
  **@0x80047148-50 `lh +0x156` / `bltz` → skip**, @0x80047158-64 andi 0xC000; Stempel
  @0x8004727C-90 `bgez → sw 2` / `sw 3` in +0x4. Kein state-Gate — §3 steht.

STATUS DER FIXES: der Patch-Plan wurde parallel bereits umgesetzt —
* §5.1 Halte-Frame: enemy_ai_re2_spider.c:2424-2436 (Kommentar zitiert dieses Dossier);
* §5.2 Ziel-Gates: re15_damage.c:1233 (state-7-Pauschale für RE2-0x25 geöffnet),
  :1247-1248 (HP<0-Gate auf 0x25||0x26), :1910-1911 (take_damage-Eintritts-Guard).
Die Zeilenangaben im Plan (:2421-2424, :1221/:1234/:1895) beschreiben den Vor-Fix-Stand.
Drei Zahlen des Erstlaufs wurden präzisiert (Clip-13-Flagwort 0xF00000CE statt „0xF0000";
kf200–207-Y = −292…−278; Dispatch `case 7` bei :2981).
