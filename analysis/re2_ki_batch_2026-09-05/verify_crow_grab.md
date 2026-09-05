# SKEPTIKER-VERIFIKATION — Paket S2 "Kraehe haengt ohne Animation am Spieler"

Alle Belege UNABHAENGIG neu hergeleitet: eigenes Disasm (`re2_disasm.py … --bin EMOVL21_S0.BIN`,
Overlay @0x80100000; RE2 PSX.EXE ohne `--bin`), eigener Voll-Overlay-Dump
(scratchpad/emovl21_full.asm, 4770 Instr.), eigener Byte-Parse von CDEMD0.EMS, eigenes
Code-Lesen der Port-Dateien. Nichts aus dem Diagnose-Dossier uebernommen.

---

## (a) Zerfallswege + Frame-Ordnung

### Zeilen/Semantik der vier Gates — alle CONFIRMED (selbst gelesen)

| Behauptung | Eigener Befund |
|---|---|
| run_all nullt `s_player_grabbed` jeden Frame | CONFIRMED: `enemy_ai_common.c:13164` (`s_player_grabbed = 0;` am Eintritt von `re15_enemy_ai_run_all`, EINMAL pro Pass, vor allen Slots) |
| exec14-Pin nur bei `claimed && state==1 && sub==14 && vs==1` | CONFIRMED: `enemy_ai_re2_crow.c:1157-1159` (`grab_out:`-Tail, ruft `re15_re2z_player_pin()`) |
| Auto-Release vs1→3 | CONFIRMED: `enemy_ai_common.c:1471-1486` (`g_player_victim==1 && !s_player_grabbed → g_player_victim=3`, Release-Clip gesetzt; 0x21-Sonderweg ohne +0x8f-Seed :1479-1484) |
| Bank-Hart-Reset vs=0 | CONFIRMED: `enemy_ai_common.c:1390` (`!vb || !vb->victim_ok → g_player_victim=0; return;`) |

### Frame-Ordnung (game_step_common.c, selbst gelesen)

`re15_enemy_ai_run_all` laeuft @`game_step_common.c:1708`, `re15_player_victim_tick`
@`:1755` — victim-tick NACH run_all im SELBEN Step. Der Clear (:13164) steht am
run_all-EINTRITT, der Claimer-Pin am Ende seines eigenen Slot-Ticks. **Solange die Kraehe
normal tickt (active, state 1, sub 14, claimed, vs==1), kann KEIN Frame ohne Pin
auftreten** — Slot-Reihenfolge ist egal (ein Clear pro Pass). Der RE2-Kraehen-Dispatch in
run_all (`enemy_ai_common.c:6332-6337`) gated NUR auf `s_ai_paused` — und dieser Hebel ist
laut dem Kommentar an :1705 ("NIE verdrahtete Hebel re15_enemy_ai_set_paused bleibt
unangetastet") nirgends gesetzt. `RE15_PAUSE_AI` ueberspringt run_all KOMPLETT (:1707) →
der Clear entfaellt mit → `s_player_grabbed` bleibt stehen → **Pause erzeugt KEINEN
Zerfall** (der victim-tick :1755 laeuft ungegated weiter, aber sieht den stehenden Pin).

### Welche realen Ablaeufe reissen die Kette WIRKLICH? — Ergebnis: KEINER herstellbar

1. **HURT 2/3 waehrend Grab**: braucht Schaden auf die Kraehe bei vs==1. Der Port-Writer
   `re15_enemy_take_damage` (re15_damage.c:1690-1706, schreibt +0x4=2/3, +0x5=React-Zeile)
   wird synchron aus dem Spieler-Angriff erreicht — und der gepinnte Spieler laeuft im
   grabbed-Branch (`game_step_common.c:1124-1140`, Gate `re15_player_is_grabbed()` =
   `s_player_grabbed || vs∈{1,2,3}`, enemy_ai_common.c:540-548), der `re15_player_tick`
   UEBERSPRINGT → kein Schuss, kein Messer. Kein anderer Schadensgeber existiert im
   Kraehenraum. Das einzige freie Fenster ist der EINE Frame commit+1 (Kraehe sub=14
   pending, vs noch 0): ein Treffer dort setzt state=2, BEVOR exec14-P0 je laeuft →
   kein Latch, kein Spieler-Write, Kraehe geht regulaer in HURT — benign.
   **HURT-mid-grab ist im Port genauso unerreichbar wie im Original** (dort aus dem
   identischen Grund). Deterministisch herstellbar nur per Werkzeug (Testschiene schreibt
   `e->state=2` bei vs==1) — nicht im Spiel.
2. **Brain-Tick-Ausfall**: kein Produzent (s_ai_paused nie gesetzt; Pause konserviert den
   Pin, s.o.; per-Entity-Freeze prueft der RE2-Kraehen-Pfad gar nicht).
3. **Latch feuert nie — victim_ok-Gate**: WIDERLEGT fuer eine gesunde Installation, s. (a6).
4. **Latch feuert nie — vs∉{0,3}-Gate**: braucht einen ZWEITEN, artfremden Greifer
   (Zombie/Hund) mit vs==1 im selben Raum — in Kraehenraeumen nicht vorhanden. (Der
   Feld-Overwrite VOR dem Gate, common.c:2354-2356, ist real — Cross-Greifer-Korruption
   nur in Mischraeumen, dort ein eigener Fund.)

**Fazit (a): die vier Gates existieren wie behauptet (CONFIRMED), aber die C1-Kausalkette
("Kette faellt EINEN Frame aus → Defektzustand") hat nach eigener Herleitung KEINEN im
normalen Spiel erreichbaren Ausloeser. Verdict fuer C1 als Ursache der Nutzer-Sitzung:
UNPROVEN** (das Diagnose-Dossier raeumt selbst ein: "Ausloeser … nicht gemessen"). Der
Defektzustand heilt zudem pro Grab selbst: der Peck-Zyklus P1→P2→P3→P4→P1 besucht P0 nie
wieder (P4-Ende schreibt phase:=1, `sb v0,6` @0x80102834-38, eigenes Disasm), das Timeout
(+0x219=100, −(3*mash+1) in JEDEM P2/P3/P4-Tick) beendet den Grab ≤100 Ticks, und der
NAECHSTE Grab-P0 latcht neu (vs 0→1). Ein PERSISTENTER "Spieler frei + Kraehe greift"-
Zyklus braeuchte einen Latch-Fehlschlag bei JEDEM Commit — dafuer bleibt nur victim_ok==0
(widerlegt) oder ein Dauer-Fremdgreifer (im Kraehenraum unmoeglich). §6.1-Messung
(Trace im Auto-Release + victim_begin-Fail) ist der richtige naechste Schritt.

### (a6) victim_ok-Gate als Ausloeser — REFUTED (eigener Byte-Parse)

TOC aus `gen/re2_ems_toc.inc` selbst dekodiert: EM021-EMD = Sektor 1347 (file 0x2a1800,
0x7e28 B) in `re15_port/shared_assets/RE2/CDEMD0.EMS`. dir @0x7e08, 8 Eintraege,
D = {8, 0xc, 0x3e4, 0x4604, 0x4608, 0x460c, 0x4734, 0x5d1c}. Paar 3 (D[5]=0x460c,
D[6]=0x4734): **3 Clips, Frames {14, 36, 20}** — Intro/Hold/Release, parsebar →
victim_ok=1. Haupt-Bank Paar 1 = 12 Clips (deckt den "12 clips"-Log). Der RE1.5-Fallback
(CDEMD0.EMS-RE1.5, Kraehen-Blob) hat die Victim-Bank ebenfalls (emd_common.c:561-604 inkl.
des dokumentierten PL00-Pool-Sonderfalls "crow_victim_anim.md F9"). Beide Ladewege liefern
victim_ok=1; der Hybrid (`pc_enemy_hybrid_re15_models`, main.c:578-633) fasst die
victim-Felder nicht an.

---

## (b) P1-Peck prueft nur Claim, nie Victim-State — CONFIRMED

* Port: `enemy_ai_re2_crow.c:1083-1099` — P1 prueft ausschliesslich `re2c_flags22a & 0x4`;
  im Claimed-Zweig `re15_re2_player_damage_mode(pl, 5, mode)` (:1092) = **−5 HP**
  (enemy_ai_re2_zombie.c:800-806: `pl->hp -= dmg`; mode 1 = kein Direkt-Tod, One-Save-Schwanz).
* Original identisch (eigenes Disasm): P1 @0x801025cc-d8 `lhu 554; andi 0x4; beq →
  0x80102690` (unclaimed = nur Feder 1/8); Claimed-Zweig `lbu a1,536(s0); addiu a0,zero,5;
  jal 0x800401d4` @0x8010265c-64. KEIN Lese-Zugriff auf PL+0x4/+0x5/+0x6 im ganzen P1.
* Praezisierung "unsichtbar": der Peck feuert hoerbar/sichtbar (Shake+Rumble
  @0x801025e0-f4, Blut-FX @0x801025f8-262c, SE 2 @0x80102650-58, alle im Port vorhanden) —
  unsichtbar ist der SCHADEN (kein Hit-React, kein Pin, keine HUD-Reaktion). Kadenz: ein
  Peck pro Zyklus ≈ 10-24 Ticks (P2: 5 Ticks via +0x21B=4; P3/P4: je rand&7+2) → ueber die
  ≤100-Tick-Grab-Dauer ≈ 5-8 Pecks = **25-40 HP pro defektem Grab**.
* Erreichbarkeit haengt an (a): NUR im Defektzustand (claimed + vs≠1). Im Original ist
  claimed-P1 immer mit cmd-5-Spieler gepaart (P0 schreibt PL→(5,0) bedingungslos bei
  Claim, @0x80102550-58) — der Check-Verzicht ist dort korrekt; der Port hat ihn byte-true
  uebernommen. **Ein Victim-State-Check im Peck waere eine Divergenz — die Schliessung
  gehoert ins Geschirr (Pin), nicht in den Peck.**

---

## (c) Sub 13 folgt / Sub 14 ohne XZ — CONFIRMED (eigenes Disasm)

* exec13 @0x80102260-45C: claimed → `jal 0x80015558 a3=96` @0x8010234c + speed+4 (Cap 300);
  unclaimed → `a3=384` @0x80102384 + speed+12 — **beide Zweige steuern auf den Spieler**
  (0x800cfc30/38 = PL-X/Z). Hoehenklemme ±60 @0x801023bc-f0. dist<0x28A(650): Hoehenfenster
  `subu; addiu +99; sltiu 0xc7` @0x80102424-38 → Commit `jal 0x80104088 a1=14`
  @0x80102444-48 — **OHNE Claim-Gate**; dist≥650 → `jal 0x80015350` (move3d) @0x80102454-5c;
  dist<650 ohne Hoehenfenster → steht und dreht nur.
* exec14 @0x8010249C-89C: einzige Positions-Stores sind `sw v0,60(s0)` (Y): y-Snap
  @0x80102588 (P0, beide Zweige), Lift `subu` @0x8010278c (P3), Drop `addu` @0x8010281c
  (P4). **Kein einziger Store auf +0x38/+0x40 (X/Z) in der ganzen Funktion** (Voll-Dump
  geprueft). Yaw-Snap nur in P0 (`jal 0x80015558 a3=2048` @0x8010259c). Speed 300
  @0x801025b8-bc wird gesetzt, aber nie in Bewegung umgesetzt. Clip 5 @0x801025a4-ac (P0)
  und @0x80102828-30 (P4→P1); Advance `jal 0x8002959c a3=512` @0x801026cc/0x80102748/
  0x801027c4 (P2/P3/P4) — loopt (EM021-Clip 5 = 8 Frames, Paar-1-EDD selbst geparst).

---

## (d) cmd-5-Spieler kann nicht laufen; Defektzustand original-unerreichbar — CONFIRMED (mit Detail-Korrektur)

* "LAB_8004006c" ist eine **Funktion** 0x8004006c (Prolog `addiu sp,sp,-24`), registriert
  an **Index 5 der Spieler-Kommando-Tabelle @0x800a4030** (selbst gefunden: Pointer-Scan +
  Dispatcher `lbu v0,4(s0); sll 2; lw @0x800a4030[cmd]; jalr` @0x8003c188-a4). Der
  Dispatch ist EXKLUSIV: pro Frame laeuft genau EIN Handler; cmd 1 = 0x8003c57c
  (Locomotion) laeuft bei cmd 5 nicht → **kein Laufen, kein Zielen/Feuern**.
* 0x8004006c selbst: setzt 0x800cfbd8|=0x40, cleart Word0-Bit 2, laedt a2=[PL+0x1B4]
  (Greifer), a1=[PL+0x188], a2=[PL+0x18C], dispatcht per **Greifer-TYP**
  (`lbu 8(a2); sll 2; lw @0x800CE300[type]; jalr` @0x8004009c-b8). Die Kraehe installiert
  dort 0x80104740 (Overlay-Init `addiu v0,…,18240; sw v0,-7292(at)` @0x801004b8-c0 →
  0x800CE384 = 0x800CE300+0x21*4).
* 0x80104740 dispatcht PL+0x5 (Tabelle @0x80104A80, [0]=0x8010477C); 0x8010477C dispatcht
  PL+0x6 (Tabelle @0x80100124, `sltiu 0x6`): P0 Clip 0x70000→Phase 1 @0x801047b8-c4;
  P1 Advance, done→0x70001 + Phase 2 @0x801047c8-ec; **P2 @0x801047f0 = NUR Advance —
  Endlos-Hold ohne eigenen Ausstieg**; P3 @0x80104804: HP<0→PL-cmd 3 + Nullen +0x5/6/7
  @0x80104818-38, sonst 0x70002 + Phase 4 @0x80104844-50; P4 @0x80104854: done→
  `jal 0x80104078(PL,1,0)` @0x80104870-80 = frei.
* ⇒ "Spieler laeuft frei, waehrend die Kraehe im Grab-Zyklus ist" ist im Original
  unerreichbar, weil der Claim-P0 den Spieler bedingungslos nach (5,0) schreibt und cmd 5
  bis zu einem expliziten Fremd-Write persistiert.

---

## (e) HURT-Exit, PL+0x6-Schreiber, Fix-Risiko

### Wer schreibt PL+0x6=3, wenn die Kraehe via HURT aus Sub 14 faellt? — NIEMAND (erschoepfend)

Voll-Overlay-Scan (4770 Instr., alle at-relativen Zugriffe + alle `s[bh] …,6(reg)`-Stores +
alle 11 `jal 0x80104078`-Aufrufe mit a0-Herkunft geprueft):
* Direkte 0x800cfbfe-Writer: **nur 2** — `sb 3` @0x80102898 (Release, gated allein auf
  Claim @0x8010286c-78) und `sb zero` @0x8010482c-30 (Victim-P3-Todeszweig).
* Indirekt via 0x80104078 (`sh zero,6(a0)` @0x80104084): nur die zwei PL-Aufrufe
  @0x80102558 (Grab-P0 → (5,0)) und @0x8010487c (Victim-P4 → (1,0)); alle 9 uebrigen
  Aufrufe laufen mit a0 = Kraehe selbst (Kontexte einzeln geprueft).
* Der Post-Pass 0x801044b0 released NUR Mutex 0x800cfbf4&=~1 + Claim &=~4 (wenn +0x5∉11..14,
  `addiu −11; sltiu 0x4` @0x801044cc-d0) — **kein Spieler-Write**. Der HURT-Dispatch
  schreibt den Spieler nie.
⇒ Im Original bliebe der Spieler nach einem Mid-Grab-Treffer auf die Kraehe im
Endlos-P2-Hold haengen (bis zu einem Re-Grab) — die Dossier-Zeile "strukturell gleich,
aber unerreichbar" ist CONFIRMED, und die Unerreichbarkeit gilt im PORT aus demselben
Grund (gepinnter Spieler feuert nicht).

### Anker-These (C3 / "Anker-Divergenz als Renderthema") — REFUTED in der Deutung

PL+0x188/+0x18C sind **Animations-Bank-Zeiger, keine Render-Anker**: 0x8004006c reicht sie
als a1/a2 an den Victim-Hook durch; die FSM-Advances rufen `0x8002959c(PL, a1, a2, 512)`,
und FUN_8002959c (Decompile selbst gelesen) indiziert **param_3 als Clip-Tabelle**
(`*(param_3 + clip*4)` → Keyframe-Zeiger nach +0x178). Die Herkunft ist der EMD-Binder:
dir[5]→Entity+0x18C, dir[6]→Entity+0x188 (@0x8001abe0/abf0 — im Port-Kommentar re2_ems.c
selbst dokumentiert); der Grab-P0 kopiert die KRAEHEN-Victim-Bank auf den SPIELER
(@0x80102568-7c), damit Leon die Clips 0x70000-2 aus der Kraehen-EMD spielt. PL+0x1B4 ist
der Greifer-Zeiger fuer den TYP-Dispatch (`lbu 8(a2)` @0x8004009c). EXE-weiter Scan aller
Loads/Stores auf 0x800cfdac/0x800cfd80/0x800cfd84: genau EIN weiterer Treffer
(`lw @0x80065b90`, ein Fallback-Read in einer Suchschleife) — **kein Renderer konsumiert
diese Worte als Position**. Das Original rendert die Kraehe an ihrer eigenen
Entity-Position; der "haengt AN mir"-Look ist reine Geometrie (Commit bei dist<650,
y-Snap, Paar steht, weil der Spieler gepinnt ist). Der Port braucht hier KEIN
Render-Nachziehen — mit funktionierendem Pin sieht das Paar aus wie im Original. (Der
Port-Kommentar "EXE-Render-Zeiger, doc" crow.c:1068-1072 ist seinerseits ein Fehl-Label.)

### Fix-Varianten

* **Variante (2) (persistenter Pin) ist die original-nahe**: das Original fuehrt den Halt
  als persistenten cmd-State (Tabelle @0x800a4030[5]; Aufloesung NUR durch explizite
  Writer: Release @0x80102898, Victim-P4 @0x8010487c, Todeszweig @0x80104820-38, oder ein
  neuer Grab-P0). Es gibt im Original WEDER eine Per-Frame-Neuherleitung NOCH ein
  Auto-Release-Netz.
* **Variante (1) (symmetrisches Per-Frame-Geschirr, victim-tick loest auch die
  Kraehen-Seite)** invertiert die Original-Abhaengigkeit (dort schreibt IMMER der Gegner
  den Spieler, nie umgekehrt) und beruehrt geteilten Code — Risiken s. unten.
* Egal welche Variante: der Peck bleibt claim-only (byte-true), die Schliessung erfolgt am
  Geschirr.

### Konkrete Regressions-Kandidaten (fuer Variante (1))

1. **Kraehe in HURT**: nach einem Treffer ist +0x5 die HURT-Zeile (Port-Writer
   re15_damage.c:1704-1706 setzt sub_state_1=React-Zeile). Ein victim-seitiger
   "Claimer-Release" der `re2c_grab_release`-Form schriebe Sub 4 in einen state-2/3-Aktor
   → falscher HURT-Dispatch / Post-Pass-Claim-Logik (crow.c:1866-1870) korrumpiert.
   Jeder Fremd-Write auf e->sub_state_1 MUSS auf state==1 gaten.
2. **Zombie-Wurf [4..7]**: der Pin trackt nur den HOLD [0..3] (common.c:2074); die
   Wurf-Phasen rufen re15_player_victim_throwoff selbst (:12642/:12680 etc.). Ein
   zusaetzlicher Release-Write aus dem victim-tick kann doppelt-releasen bzw. die
   cmd5/6-Maschine (P9-Wort 0x1) verschieben → test_re2_zombie_ai, RE1.5-Grab/Mash-Pins
   (room1140-Fixtures), test_re2_room1140_ab.
3. **Hund Sub-7-Latch**: ein erzwungener Kraehen-Release-Pfad im geteilten victim-tick
   darf den Dog-Latch nicht beruehren → test_re2_dog_ai, test_re2_room1190_ab.
4. **Peck-Tod (r==2)**: hp<0 laesst vs==1 stehen (crow.c:1042-Gate), der Auto-Release
   feuert dann auf einem TOTEN Spieler — jede Umbau-Variante muss den hp<0-Handoff auf die
   Port-Death-FSM erhalten (Vorrang is_dead vor is_grabbed, game_step_common.c:1099/1124).
5. Fuer Variante (2): vor dem Umbau ALLE Grab-Enden ohne throwoff-Aufruf inventarisieren
   (Zombie/Hund/Gorilla/Birkin — 12 Pin-Setzer in enemy_ai_common.c) — jeder solche Pfad
   wuerde mit persistentem Pin zum Dauer-Hang.

---

## Verdikt-Tabelle

| # | Behauptung | Verdict |
|---|---|---|
| a1-a4 | Vier Gates (13164 / 1157-1159 / 1471-1485 / 1390), Zeilen+Semantik | CONFIRMED |
| a5 | Ordnung: victim-tick nach run_all; Frame ohne Pin bei normal tickender Kraehe | CONFIRMED (Ordnung) / **kein pinfreier Frame moeglich** |
| a6 | Kette reisst in realen Ablaeufen (HURT mid-grab etc.) | **REFUTED als spielbar-deterministisch**; Sitzungs-Ausloeser UNPROVEN |
| a7 | victim_ok-Latch-Fehlschlag als Ursache | REFUTED (EM021-Paar-3 parsebar: 3 Clips {14,36,20}, Bytes selbst gelesen) |
| b | P1-Peck nur Claim-Check, −5 HP auf freien Spieler | CONFIRMED (Port = Original; "unsichtbar" nur bzgl. Hit-React/Pin — SE+FX feuern) |
| c | Sub 13 folgt / Sub 14 ohne XZ (nur y/Yaw) | CONFIRMED (eigenes Disasm, kein X/Z-Store in exec14) |
| d | cmd-5-Dispatch ersetzt Locomotion; Defektzustand original-unerreichbar | CONFIRMED (0x8004006c = Funktion @Tabelle 0x800a4030[5]; exklusiver jalr) |
| e1 | HURT-Exit ohne Spieler-Release; PL+0x6=3-Schreiber | CONFIRMED: NIEMAND schreibt — Original liesse den Spieler im Endlos-P2-Hold |
| e2 | Anker PL+0x1B4/188/18C = Render-Verankerung der Kraehe am Spieler | **REFUTED**: Anim-Bank-Zeiger + Typ-Dispatch; kein Render-Konsument (EXE-Scan) |
| e3 | Variante (2) naeher am Original | CONFIRMED (persistenter cmd-State ist der Original-Mechanismus) |
