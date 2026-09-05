# VERIFY S6 — RE2-Zombie-Konformitaet (Skeptiker-Gegenpruefung)

Datum: 2026-09-05. Methode: EIGENES Disasm — `re2_disasm.py dis … --bin EMZ0.BIN` (EMZ0.BIN ==
EMOVL10_S0.BIN, md5 7f7a39e651064e07c95830db1bebdb89, 53068 B, RAW @0x80100000, geprueft), eigener
Voll-Disasm `scratchpad/my_emz0_full.asm` (13267 Instr., unabhaengig vom Diagnose-Dump erzeugt),
RE2-EXE-Disasm ohne --bin, RE2_Quellcode_V2/FUN_8004a808.c + FUN_80050858.c gegen eigenes Disasm
gehalten, Port-Code selbst gelesen. Keine Datei geaendert, kein Build.

## (a) IDLE EXEC[0] @0x801013F4-0x801016C4 — **CONFIRMED, mit 4 Praezisierungen**

Phasen-Dispatch @0x80101410-54 (`lbu +0x6`): P0 @0x80101458, P1 @0x80101494, P2 @0x801015A4,
P3 @0x801015E0, sonst Epilog @0x801016AC. Selbst nachgewiesen:

* **P0** @0x80101458-90: `jal rand(0x80015fe8)` → Clip-Wort `0xF0000 | (rand&0x1f)<<8`
  (`sw 332(s0)`; Layout clip|frame<<8|rate<<16 ⇒ Clip 0, ZUFALLSFRAME, Rate 0xF); `sb 1,+0x6`;
  `+0x158 = rand+150` (sh 344); `+0x15A = rand+300` (sh 346). Der RNG liefert **0..255**
  (eigener EXE-Disasm 0x80015fe8-0x80016024: `andi v0,v0,0xff` als Rueckgabe) ⇒ 150-405 / 300-555 ✓.
  ⚠ KORREKTUR 1: **P0 faellt in DEMSELBEN Tick in P1 durch** (nach @0x80101490 folgt 0x80101494,
  kein Sprung) — der Port (`re2z_exec_stand` Z. 1178 `return`) weicht hier ab; der Advance und
  der Timer-Lauf beginnen im Original schon auf dem Seed-Tick.
* **P1** @0x80101494-…: (1) `jal 0x8002959c(a3=256)` Advance. (2) `lhu +0x15A`; Dekrement wird
  IMMER gespeichert (Delay-Slot @0x801014B8; bei 0 landet erst 0xFFFF im Feld, dann Reseed);
  pre-dec==0 → Moan-Block: `+0x239`-Gate, `rand&1 != 0` → `jal 0x8005bd6c(11,e)`, `+0x239=150`;
  Reseed `rand+300`. (3) ⚠ KORREKTUR 2 (Dossier-Auslassung): @0x80101500-18 ein **von `+0x14E==0`
  gegateter `jal 0x80016200(e,0,1)`** in JEDEM P1-Tick. +0x14E ist das Rate-Byte des Clip-Worts
  (P0 schreibt dort 0xF) → im normalen Idle DORMANT, aber Teil der Byte-Struktur (0x80016200 =
  EXE-Matrix-/Part-Helfer, Konsument nicht identifiziert). (4) **WANDER-Wurf** @0x8010151C-40:
  `lh +0x15A` SIGNED **nach** dem Dekrement; ist er EXAKT 0 (= Tick VOR dem Moan-Tick) → Draw,
  `(rand&1)==0` → `+0x6=2` (50 %). Detail: gelingt der Wurf, verlaesst der Zombie P1 und der
  Moan-Tick des Zyklus ENTFAELLT. (5) **SELBST-WECKER** @0x80101544-7C: `+0x158==0` →
  `dist(+0x1F0) < 0x1d4c` (sltiu @0x8010155C) UND `+0x154&0x800` (@0x80101568-74) →
  `sw 257,+0x4` = **0x101** (@0x80101578-7C). Sonst @0x80101590-A0 Dekrement. Bei abgelaufenem
  Timer wird JEDEN Tick neu getestet (Timer bleibt 0).
* **P2 (WANDER-SETUP)** @0x801015A4-DC: `+0x6=3`; Clip-Wort `0xF0000 + walkclip(+0x218)`
  (Frame 0, KEIN Zufallsframe); `+0x158 = rand+150`; **`jal 0x8004aa50` @0x801015C4** →
  Rueckgabe `sb v0,+0x235`; `+0x21A |= 0x400` @0x801015D4-D8. ⚠ KORREKTUR 3: auch **P2 faellt im
  selben Tick in P3 durch** (@0x801015DC ist schon der a0-Setup von P3).
  **0x8004AA50 selbst disassembliert** (EXE @0x8004aa50-aab4): Zaehlwort = erstes u16 der
  Knoten-Tabelle `[room(0x800ce324)+0x38]`; `count==0 → return 255 OHNE Draw`; sonst
  `rand % count` (divu/mfhi), `& 0xff` → **ZONEN-INDEX** (0-basiert). In +0x235 landet also der
  rohe Index-Byte (bzw. 255). Derselbe RNG 0x80015fe8 ✓.
* **P3 (WANDER-GANG)** @0x801015E0-16A8: `jal 0x80015558(+0x1C4/+0x1C6, a3=16)` Steer Rate 16;
  Bewegungs-Trio `0x80015e7c` / `0x8002959c(256)` / `0x800152c8`. Exits: (i) `+0x158--`
  UNBEDINGT; ==0 → `+0x6=0` + `+0x21A &= ~0x400` (@0x80101624-4C) — **kein Return, die zwei
  Wake-Pruefungen laufen im selben Tick noch**; (ii) `dist<0xbb8` → 0x101 + Clear (OHNE LOS);
  (iii) `dist<0x1d4c && +0x154&0x800` → 0x101 + Clear (@0x80101674-A8). ✓ wie im Dossier.
* Port-Stand gegengeprueft (enemy_ai_re2_zombie.c:1170-1185): nur P0+Moan, `return` nach
  Dekrement — B1 des Dossiers (Statue) stimmt.

Wecker-Wortlaut der Diagnose („Wake nach ≤405 Ticks") praezisiert: der Schutz-Timer +0x158
laeuft in P1; nach Ablauf wacht der Zombie im ERSTEN Tick mit dist<7500 UND freier Sichtlinie —
ohne LOS-Produzent (F2) bliebe er weiter stehen bzw. wachte (Mapping=1) durch Waende.

## (b) Selbst-Wecker OHNE Kegel — **CONFIRMED, Gegenprobe negativ**

Vollstaendige Call-Liste von EXEC[0] (awk ueber eigenen Voll-Disasm, 15 jal):
6× rand, 0x8002959c, 0x8005bd6c, 0x80016200, 0x8004aa50, 0x80015558, 0x80015e7c, 0x8002959c,
0x800152c8. **Kein `jal 0x80015614` und kein `jal 0x80015758`** (die beiden Kegel-/Sektor-Tests)
im gesamten Executor. Die einzigen Gates vor `+0x4=0x101` sind +0x158==0, dist<0x1d4c,
+0x154&0x800. Der Zombie weckt sich also auch mit dem Ruecken zum Spieler, sofern der LOS-Ray
frei ist. (Die Kegel-Tests in DECISION[0] @0x801012C4/E4 sind ein ANDERER, parallel laufender
Pfad mit eigener 5000er-Schwelle.)

## (c) LOS-Produzent + Ray-Mode — **CONFIRMED (0x2000, nicht 0x8400); sechs Leser exakt**

Eigenes Disasm FUN_8004a808 @0x8004a834-8f4: Repath-Byte +0x150 (low7==0→|=7; immer −1);
`(neu & 0x80)==0` → `+0x154 &= 0xF7FF` (@0x8004a87c-880), **`a2 = 8192 (0x2000)`
@0x8004a868, a3=1**, a0 = `[+0x198] + (+0x1C1)·0xAC + 0x5C` (Part-Matrix-Translation),
a1 = `[0x800cfd90] + [0x800cfdb9]·0xAC + 0x5C` (Spieler-Part), `jal 0x80050858`; **Rueckgabe 0 →
`+0x154 |= 0x800`** (Sicht FREI). Da Bit 7 von +0x150 im Auslieferungsspiel nie gesetzt wird,
schiesst der Ray praktisch JEDEN Tick. Decompilat FUN_8004a808.c:19-26 deckungsgleich ✓.
Root-Aufruf pro Tick belegt: @0x80100354 `jal 0x8004a808` mit `lbu a2,565(s0)` (+0x235) und
`lhu a3,538(s0); andi a3,0x400` (@0x8010034C-58).

**Welche Linien matcht Mode 0x2000** (eigenes Disasm 0x80050858 + Decompilat): Iteration ueber
die Raum-Kollisionstabelle `[room+0x20]` (Zaehlwort bei +4, Eintraege ab +16, **Stride 16 Byte**);
Eintrag blockiert nur, wenn `attr_u16(Eintrag+8) & 0x2000` UND `DAT_800a73b4[attr&0xf]==0`.
Die Skip-Tabelle (16 Bytes @0x800a73b4, selbst gelesen) = [0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0] —
Form-Nibbles 1-5 und 13 blockieren NIE. a3=1 aktiviert zusaetzlich den Hoehenband-Test
(Y-Extents aus Eintrag+10/+12). Rueckgabe 0 = kein blockierender Eintrag. 0x2000 ist also ein
ATTRIBUT-BIT der SCA-Eintraege, kein Kombi-Mode 0x8400.

**Die sechs Leser** (erschoepfender Scan ALLER `,340(`-Zugriffe + aller `andi …,0x800` des
Overlays): +0x154&0x800 wird gelesen @0x80101308, @0x8010134C, @0x80101390 (DECISION[0], alle
drei Bloecke — eigenes Disasm des ganzen DECISION[0] bestaetigt), @0x80101568 (Idle-Selbst-
Wecker), @0x80101688 (Wander-Wecker), @0x80103AE8 (Kriecher-Warte-Entscheid). Die zwei weiteren
andi-0x800-Stellen (@0x80101E04, @0x801024B8) lesen **+0x21A**, nicht +0x154; die zwei
`lhu 340(s2)` @0x801003E8/@0x80100420 lesen das SPIELER-Wort und testen 0x8000/0x4000. Es sind
exakt sechs. Port: 4 davon als „gemappt 1" (3× decide_stand Z. 1140-1145, Kriecher Z. 3004-3007),
2 existieren nicht (Teil von F1). Wichtig fuer F2: in DECISION[0] steht der LOS-Test VOR dem
`rand&1`-Draw der Bloecke 2/3 — LOS=0 aendert dort auch die WURFZAHL.

## (d) WALK-Rumpf — **CONFIRMED; Draw-Zaehlung Port praezisiert**

Eigenes Disasm @0x80101A40-0x80101D6C:

* **P0 = exakt 5 Draws** (Reihenfolge): (1) Zeile `+0x16B=(rand&0xf)<<1` @0x80101A88-9C;
  (2) Zeilen-Timer `+0x158 = (tbl0x8010C924[row]&0x7fff) + rand` — **VOLLES Byte** @0x80101AC0-C8;
  (3) `+0x16A=(rand&0x1f)+30` @0x80101AC4-E8; (4) `+0x14D=rand&0x1f` @0x80101AEC-B0C, zwischen
  den zwei 0x80015e7c-Re-Ankern @0x80101AE4/@0x80101B08 ✓; (5) `+0x15A=(rand>>3)+100`
  @0x80101B10-20. Danach Durchfall in P1 im selben Tick (frisch geseedete Timer → keine
  weiteren Draws auf dem P0-Tick).
* **Zeilenwechsel** (pre-dec +0x158==0) @0x80101B38-A8: row++ mit Wrap 32 (Zeilen werden ab da
  auch UNGERADE); Reload-Draw `tbl[row]&0x7fff + (rand&0x1f)` (**Maske 0x1f**, Init/Reload-
  Asymmetrie bestaetigt); **2. Draw = Jitter**: `rand&1` → `yaw(+0x76) = yaw − 16 + ((r&1)<<5)`
  @0x80101B8C-A8 ✓ = ±16 je Zeilengrenze, im Port fehlend (re2z_gait_tick Z. 384-390: nur 1 Draw).
* **Turn-Gate** ✓ (sltiu 0x1389 → 8; sonst tbl>>15 → ±8; Steer @0x80101C00; Nah-Zusatz
  sltiu 0xbb8 → Extra-Steer 16 @0x80101CA8-AC) — deckungsgleich mit dem Port.
* **+0x16A-Puls** @0x80101C08-40: Dekrement jeden Tick; NUR pre-dec==0: Reload-Draw
  `(rand&0xf)+30`, **Extra-Steer Rate 16** @0x80101C3C, und ERST DANN der **Moan-Block**
  @0x80101C44-88 (Gate +0x239; Draw `&0x1f==0`→SE 10, sonst 2. Draw `&0x1f==0`→SE 11; cd 150).
  Nicht-Ablauf-Ticks ueberspringen Steer UND Moan ✓. Port: `re2z_walk_moan` laeuft JEDEN Tick
  (Z. 1199; Funktion Z. 916-924) und VOR dem Turn — auch die **Draw-REIHENFOLGE** weicht ab
  (Original: Zeile/Turn → Puls → Moan; Port: Moan → Gait).
* **Draw-Bilanz-Korrektur**: Dossier sagt „P0: Original 5, Port 2". Praeziser: Port-P0-Tick =
  2 (Gait-Init) **+ 0-2 Moan-Draws** (re2z_walk_moan laeuft auch auf dem Entry-Tick; nach
  INIT ist +0x239==0 → es WIRD gewuerfelt). „Port 2" gilt nur bei laufendem Moan-Cooldown.
  Steady-State stimmt die Dossier-Aussage (Port 1-2 Draws jeden cd-freien Tick, Original nur
  am Puls-Tick).

## (e) Fettzombie-Block + INIT — **CONFIRMED im Original; Dossier-B5 zur Haelfte REFUTED (Port hat den SE-Teil laengst)**

* Gate @0x80101CD8-CF8: `(+0x10E&0x80) || (+0x21A&0x8000)` ✓ (bne @0x80101CE4 springt bei
  0x80 direkt in den Block; sonst beq @0x80101CF8 vorbei).
* `+0x14D % 3 == 2` ✓ (multu 0xAAAAAAAB-Magie @0x80101D04-24), gelesen NACH dem regulaeren
  Advance 0x8002959c(256) @0x80101CD0.
* Dann `jal 0x801016c8` (selbst disassembliert: liest das Frame-Wort ueber +0x178; Bit
  0x08000000 && (Wort>>28)<2 → `jal 0x8005bd6c(Wort>>28, e)` = Schritt-SE 0/1) + **Steer
  Rate 8** @0x80101D48 + `jal 0x8002a9c8` @0x80101D54.
* **0x8002A9C8** (Task nannte 0x8002A9CC = zweite Instruktion derselben Funktion) selbst
  disassembliert @0x8002a9c8-aa24: `+0x14D++`, Wrap gegen das Frame-Count-Halbwort des
  Clip-Deskriptors, `+0x178` = neuer Frame-Datenzeiger — **purer Frame-Advance**, kein Blend,
  keine Wurzelbewegung ✓.
* **INIT** @0x801008BC-D4: `lbu v1,+0x8` (Typ) @0x80100894; `v0=17`; Typ==0x11 →
  `+0x156(HP)=250` UND `+0x21A|=0x8000` ✓ (+0x21A wurde @0x8010087C zuvor genullt). Zweiter
  Produzent @0x801008D8-950: Global `0x800cfb74&0x40` → `(rand >> (rand&3)) % 3 == 0` →
  `|=0x8000` **plus Reseed `+0x223=(rand&0xf)+32`** (Dossier-Detail fehlte; im Port ohne
  Produzenten des Globals irrelevant).
* ⚠ **KORREKTUR A (gegen Dossier B5 und Fix-Plan F4):** Der SE-Takt-Teil des Blocks ist im
  Port BEREITS SCHARF — `enemy_ai_common.c` ~12984-13075 implementiert exakt das Gate
  `(re2z_f10e&0x80)||(re2z_flags21a&0x8000)`, `slot%3==2` und die 0x801016c8-Probe, und
  `unit_re2_sfx_cadence` (test_re2_sfx_cadence.c PIN A/B/C) pinnt es. Es fehlen NUR
  (i) der Extra-Advance 0x8002a9c8 und (ii) der Steer-8 der WALK-Stelle. F4 darf die SE-Probe
  NICHT ein zweites Mal einbauen (Doppel-SE), sondern muss sie mit dem bestehenden
  Common-Block zusammenfuehren.
* ⚠ **KORREKTUR B:** Der EXEC[2]-Zwilling @0x801023F8-468 hat **KEINEN Steer-8** (eigenes
  Disasm: jal 0x801016c8 @0x80102454 → direkt jal 0x8002a9c8 @0x80102460). „Extra-Advance +
  Steer 8 + SE-Probe an 2 Stellen" ist fuer EXEC[2] falsch — dort nur Probe + Advance.
* ⚠ **KORREKTUR C (Kadenz):** „≈4/3-fache Gang-Kadenz" ist falsch gerechnet. Der %3-Test
  laeuft auf dem POST-Advance-Frame; der Extra-Advance springt ueber die Residue-2-Frames,
  wodurch Residue 2 alle ZWEI Ticks wiederkehrt: Frames 1,3,4,6,7,9,… = **3 Frames je 2 Ticks
  = 1,5-fache Kadenz**, SE-Probe alle 2 Ticks. Wichtig fuer die Neuberechnung von
  test_re2_sfx_cadence PIN C1/C2 (deren expected_hits-Modell nimmt 1 Frame/Tick an).

## (f) FIX-RISIKO — Draw-Strom, Tests, Navigator-Anschluss

**Ist der Draw-Strom ein Byte-Kriterium? JA, dreifach belegt:**
* RE15_RE2_AI.md:236: „**Die Zahl der RNG-Ziehungen ist selbst Verhalten:** 0-2 pro Tick
  (D @0x80101888, E @0x801018D0), …"
* enemy_ai_re2_zombie.c:329-331: „Die WURFZAHL ist Verhalten (jeder `jal 0x80015FE8` zaehlt);
  die Gore-Tests pinnen sie differenziell" + Zaehler `re15_re2_rand_draws()`.
* test_re2_zombie_ai.c prueft `d.rng_draws == 0` fuer die stumme W2-Leiter.

**Tests, die nach F1-F4 angefasst/re-validiert werden muessen** (ctest-registriert, selbst aus
tests/unit/CMakeLists.txt gezogen):
1. **unit_re2_sfx_cadence** — MUSS umgerechnet werden bei F4: PIN C1/C2 modellieren
   1 Frame/Tick; mit Extra-Advance 1,5× und SE alle 2 Ticks. PIN A/B bleiben.
2. **unit_re2_zombie_abc** (Seed-Sweep, Schwellen hits>500/deaths>20/down_n>50/blocked==0/
   standup==0/down_min-max) — F1+F3 verschieben den gesamten Strom; Praezedenz: die
   B3-Produzenten allein kollabierten den Nahkampf 3523→0. Sweep neu fahren, ggf. neu eichen.
3. **unit_re2_room1140_ab** — deterministischer Raumlauf mit Ereignis-Pins (Wake, Angriff,
   Grab, Biss=20 HP, Knockdown, Death-Kette, kein Freeze) in festen Tick-Budgets; jede
   Draw-Verschiebung kann Ereignisse verschieben/verlieren.
4. **unit_re2_zombie_ai** — die Leiter-Pins (rng_draws) bleiben unberuehrt (F1-F4 aendern die
   W2-Leiter nicht), aber die Live-Sektionen nach `re15_re2z_rng_reset()` (D/E-Commit-Schleife
   ueber bis 300 Walk-Ticks; Kommentare wie „first draw 0x7D") laufen auf verschobenen
   Stroemen; ausserdem koennen lange Stand-Schleifen jetzt WANDERN (F1). Re-run + ggf. Sektionen
   nachziehen.
5. **unit_re2_livepath**, **unit_re2_lyer_1140**, **unit_re2_zombie_teardeath**,
   **unit_re2_zombie_death_fall**, **unit_re2_stagger_knockdown**, **unit_re2_hit_repeat(_crow_
   spider)**, **unit_re2_zombie_hitfilter**, **probe_re2z_stomp**, **unit_re2z_bandlock_pin**,
   **unit_re2z_*-Familie** (bump_walk, grab_core_se, pushoff_crawl, rise_hittable, finisher_bite,
   devour_anchor, corpse_pool, death_getup) — alle ticken das Brain ueber laengere Fenster;
   re-validieren. test_re2z_grab_core_se (E) prueft explizit „RE2-Strom unberuehrt" ueber
   Shim-Aufrufe — bleibt ok, solange F1-F4 nur in Brain-TICKS ziehen.
6. Gore-Differenzialpins (test_re2_gore 429-461, test_re15_re2z_import 277-284,
   test_re15_re2z_gore_se, probe_re15_gore_import, probe_re2_ztear) messen Draws um EINZELNE
   Hit-Aufrufe ohne Brain-Ticks dazwischen — von F1-F4 NICHT betroffen.
7. Nicht registrierte Dev-Probes (probe_re2z_abc, probe_re2_fatz_walkspot, …) nur bei Gebrauch.

**Kann F1 ohne den Navigator-WP-Anschluss korrekt laufen? NEIN.**
* Ohne WP-Modus steuert P3 mit Rate 16 auf `e->steer_x/z`, das der gemeinsame Prolog
  (enemy_ai_common.c:5267) JEDEN Tick in Richtung SPIELER fuellt — aus „Wandern zu einer
  Zufallszone" wuerde ein langsamer Anmarsch, und der `dist<0xbb8`-Exit macht daraus einen
  Selbst-Wecker durch Anlaufen. Der Zonen-Draw muss trotzdem gezogen werden (Strom-Paritaet),
  inkl. der Edge: `nav_count==0 → 255 OHNE Draw` (eigener 0x8004aa50-Beleg).
* Der Port-WP-Pfad EXISTIERT (nav_zone_common.c:204-240: wp_mode → Zielzone := wp_node,
  Zielpunkt = Boxmitte — strukturgleich zu FUN_8004a808s WP-Zweig, eigenes Decompilat-Lesen
  Zeilen 38-48). ABER der Aufrufer uebergibt `e->ai_flags & 8` als EIN-SCHUSS (Clear direkt
  danach, Z. 5271, RE1.5-Semantik @0x8010a9f8), waehrend RE2 ein PERSISTENTES `+0x21A&0x400`
  uebergibt. F1 braucht die Bruecke: entweder pro P3-Tick `ai_flags|=8` + `ai_wp_node=+0x235`
  aus dem Brain setzen (wirkt wegen Prolog-vor-Brain erst im Folgetick — dieselbe 1-Tick-
  Pipeline wie im Original: P2 setzt das Bit, der Navigator liest es im naechsten Tick) oder
  den Prolog-Aufruf flavor-abhaengig `re2z_flags21a&0x400` lesen lassen.
* ⛔ NICHT als ZWEITEN `re15_nav_update_steer`-Aufruf im Brain loesen: der dekrementiert den
  geteilten Repath-Timer (+0x91-Zwilling `repath_timer`) DOPPELT pro Tick und verschiebt die
  DFS-Kadenz.

**Weitere konkrete Regressions-Kandidaten:**
* F3 saet +0x15A im Walk-P0 (Draw 5) — der KONSUMENT ist aber der bewusst deaktivierte
  B3-Timer-Block (@0x80101D68-DBC: Reseed-Draw + `&3`-Draw + dist<0x1388 → 0x201). Nur saeen
  ohne den Block zu ticken erzeugt ab dem ersten Ablauf (~101-132 Ticks) wieder Draw-Divergenz.
  Byte-sauber: den ganzen Timer-Block MIT seinen 2 Ablauf-Draws laufen lassen und nur den
  finalen 0x201-Commit gedrosselt lassen (B3-Politik), sonst ist die F3-Paritaet Kosmetik.
* F2 senkt in DECISION[0] die Wurfzahl, wenn LOS=0 (Bloecke 2/3 ziehen dann NICHT) — gewollt
  byte-naeher, aber es verschiebt bestehende Laufe; ausserdem Default definieren: synthetische
  Tests ohne Kollisionsgeometrie muessen LOS=frei sehen, sonst schlafen alle Test-Zombies.
* F1 macht Sub 0 zeitbegrenzt: Tests/Szenen, die einen geparkten Zombie als ruhend annehmen
  (Screenshots, lange Fenster), sehen nach 150-405 Ticks Bewegung/Wake.
* F4-Platzierung: Gate liest +0x14D NACH 0x8002959c und VOR 0x800152c8 — im Port also zwischen
  Advance und Apply in `re15_re2z_move_root`, sonst verschiebt sich der %3-Takt um einen Tick;
  und die SE-Emission muss an EINER Stelle bleiben (Common-Block vs. Brain), sonst Doppel-SE.
* F1-Moan-Detail: erfolgreicher Wander-Wurf ueberspringt den Moan-Tick desselben Zyklus
  (Phase verlaesst P1) — wer den Port-Moan unveraendert vor dem Wander prueft, moant zu oft.

## Urteilsuebersicht

| # | Behauptung (Auftrag) | Urteil |
|---|---|---|
| a | IDLE-EXEC[0]-Struktur inkl. Timer/Wander/Zonen-Draw/0x400/Exits | **CONFIRMED** + 4 Praezisierungen (P0→P1- und P2→P3-Durchfall im selben Tick; +0x14E-gegateter jal 0x80016200 in P1; 0x8004aa50 = Index `rand%count`, 255-Sentinel ohne Draw) |
| b | Selbst-Wecker dist<0x1d4c + Bit 0x800 OHNE Kegel | **CONFIRMED** (Call-Liste: kein 0x80015614/0x80015758 im ganzen EXEC[0]) |
| c | LOS-Produzent FUN_8004a808, Mode 0x2000, sechs Leser | **CONFIRMED** (0x2000 @0x8004a868; matcht SCA-Eintraege attr&0x2000 mit Form-Nibble ∉{1..5,13}; exakt 6 Leser, erschoepfend gescannt) |
| d | WALK: Startframe/Jitter/Puls/Moan-nur-am-Ablauf/5 P0-Draws | **CONFIRMED**; Port-P0-Zahl korrigiert (2 + 0-2 Moan-Draws, nicht fix 2); Draw-REIHENFOLGE weicht zusaetzlich ab |
| e | Fettzombie-Gate/0x8002a9c8/INIT 0x11 | **CONFIRMED** im Original; Dossier-B5 „Block im Port leer" **halb REFUTED** (SE-Takt laengst portiert + gepinnt; nur Extra-Advance + WALK-Steer-8 fehlen); EXEC[2]-Zwilling OHNE Steer-8; Kadenz 1,5×, nicht 4/3 |
| f | Draw-Strom-Risiko + Tests + Navigator-Frage | s. oben; F1 ohne WP-Anschluss NICHT korrekt |
