# SKEPTIKER-Verifikation S3 — "Kein Spieler-Tod bei Kraehen-Kill" (RE2-Modus)

**Datum:** 2026-09-05. Alle Original-Belege in diesem Dossier sind EIGENES Disasm dieser
Session (`re2_disasm.py` auf `info/re2leon/COMMON/BIN/EMOVL21_S0.BIN` bzw.
`info/re2leon/PSX.EXE`, `re15_disasm.py` auf `info/Re1.5/PSX.EXE`), Vollverlauf
`scratchpad/crow_full_dis.txt` (kompletter Overlay-Dump, 4770 Instr. = 19080 B Datei).
Port-Belege gegen den aktuellen Arbeitsbaum. NICHTS aus dem Diagnose-Dossier uebernommen.

---

## (a) Release-Trichter @0x80102848 — CONFIRMED

Eigener Voll-Dump des Overlays, grep nach `0x80102848`: **exakt 6 Referenzen**, alle in der
Grab-Maschine, keine weitere im ganzen Binary:

| Site | Instruktion | Weg |
|---|---|---|
| 0x80102518 | `bltz v0` nach `lh v0,0(s1)` mit s1=0x800cfd4e (PL-HP, @0x80102508-10) | P0-hp<0-Fruehausstieg |
| 0x80102670 | `beq v1,v0(=1)` nach `jal 0x800401d4` (a0=5, a1=+0x218 @0x8010265c-64) | ret 1 One-Save |
| 0x80102688 | `j` nach `jal 0x8010452c` a0=128 (@0x80102680-84) | ret 2 TOD (Broadcast 128) |
| 0x801026f8 | `bltz` nach `+0x219 -= 3*mash+1`, `sll v0,v0,24` (@0x801026e0-f4) | P2-Timeout |
| 0x80102774 | dito (@0x80102758-70) | P3-Timeout |
| 0x801027f0 | dito (@0x801027d0-ec) | P4-Timeout |

Trichter selbst (eigener Dump @0x80102848-98): rand&0x7f -> +0x220, jal 0x8004aa50 ->
+0x21F, **Sub := 4** (jal 0x80104088, a1=4 @0x8010285c-64), dann Claim-Gate
`lhu v0,554(s0); andi v0,4; beq v0,zero,0x8010289c` (@0x8010286c-78). **Unter dem Gate:**
Vol 350, Broadcast 16, `addiu v0,zero,3 / sb v0,-1026(at)` = **0x800cfbfe (PL+0x6) := 3
@0x80102890-98 — ohne jede weitere Bedingung** (kein HP-Check, kein Ist-Wert-Check).

Schreiber-Zensus PL+0x4/+0x6 im ganzen Overlay (grep ueber den annotierten Voll-Dump):
nur 0x80102898 (fbfe:=3), 0x80104820 (fbfc:=3), 0x80104830 (fbfe:=0). *Grenze der
Methode: Stores ueber berechnete Basisregister waeren nicht annotiert — dieselbe Grenze
hat der Diagnose-Zensus.*

**Scope-Praezisierung (kein Widerspruch):** Externe Abbrueche (Kraehe wird im Grab
getroffen/gegibbt) laufen NICHT durch den Trichter — der DEC-Listener 0x8010247c ->
0x80104400 schreibt nur Kraehen-Zustand (+0x22A &= 0xFFA7, Sub 4, +0x21C=30), NIE
Spieler-State. Spieler-gerichtete `0x80104078`-Aufrufe im ganzen Overlay: nur
@0x80102558 (PL(5,0,0,0) Grab-Claim; a0=s1-342=0x800cfbf8) und @0x8010487c
(PL(1,0,0,0), Victim-Hook Ph4). Alle 9 uebrigen Call-Sites uebergeben a0=s0=Kraehe
(eigene State-Writes, u.a. der scheinbare "PL(7)"-Fund @0x80102c7c ist in Wahrheit
`a0=s0` = Kraehen-State 7). Der Trichter ist also der gemeinsame Ausgang **aller
Release-Wege der Grab-Maschine** — die Formulierung des Dossiers ist in diesem Sinne
korrekt.

## (b) Victim-Hook Ph3 + Dispatch-Reihenfolge — CONFIRMED, Timing jetzt GEMESSEN

Hook-Install (einziger im Overlay): `lui v0,0x8010 / addiu v0,18240 (=0x80104740) /
sw v0,-7292(at)` = **0x800ce384** @0x801004b4-c0. grep Voll-Dump: **0 Referenzen auf
0x800ce484** (cmd-6-Slot) — kein Kollaps-Hook. EXE-Gegenprobe: cmd-5-Handler 0x8004006c
liest `lw v0,-6360(v1)` mit v1=0x800cfbd8+kind<<2 = Basis **0x800ce300** (@0x800400b0);
cmd-6-Handler analog `lw v0,-6104(v1)` = Basis **0x800ce400** (@0x80040114). 0x800ce300 +
(0x21<<2) = 0x800ce384. Beide Handler laden vor dem jalr a1/a2 = PL+0x188/+0x18C
(@0x800400a0-a4) = die VICTIM-Bank.

Hook 0x80104740 (eigener Dump): Dispatch auf PL+0x5 via Tabelle @0x80104a80
([0]=0x8010477c), Maschine 0x8010477c: `sltiu +0x6,6` + Tabelle @0x80100124 =
{0x801047b8, 0x801047c8, 0x801047f0, 0x80104804, 0x80104854, 0x80104884}. **Ph3
@0x80104804:** `lh v0,-690(v0)` = 0x800cfd4e (PL-HP); `bgez v0,0x80104844`;
TOT: `sb 3 -> 0x800cfbfc` (+0x4:=3) + `sb zero -> fbfd/fbfe/fbff` (+0x5/6/7:=0)
@0x80104818-38, dann `j exit` — **kein Clip-Write**. LEBEND @0x80104844-50: Clip-Wort
0x70002 -> +0x14C, +0x6:=4. Ph4 @0x80104854: Advance; fertig -> Clip-Wort 0x70001 +
`jal 0x80104078(0x800cfbf8, 1, 0)` = PL frei.

**Dispatch-Reihenfolge (neu gemessen, RE2-EXE-Hauptschleife):** Entity-Schleife
@0x80026568-0x80026604 (`lw s0,0(s1)` ueber die Pointer-Liste s3+0x3c30, pro Entity
Distanz zu s2=s3+0x3a10 + `lbu v1,8(s0); jalr [s3+15476+kind<<2]`) laeuft **VOR** dem
Spieler-Dispatcher `jal 0x8003bfac` @0x80026620 (a0 = s3+0x3a10; Ghidra loest s3+0x3a10
selbst als **DAT_800cfbf8** auf, z.B. @0x800360d0). FUN_8003bfac dispatcht
`PTR_LAB_800a4030[PL+0x4]` genau EINMAL pro Frame (Decompilat + Tabellen-Leser
@0x8003c19c).

**Folge:** Release-Write (Gegner-Schleife) und Ph3-Handoff PL(3,0,0,0) (Spieler-Dispatch,
cmd 5) liegen im **SELBEN Frame**; der cmd-3-Kollaps (Clip-Start + SE) beginnt erst im
**NAECHSTEN** Frame-Dispatch. Der geplante Port-Latch am Release-Tick (death_cmd3 in
run_all @game_step_common.c:1708, Tick @:1761 im selben Step) startet den Kollaps damit
**genau 1 Frame FRUEHER als RE2** — aber exakt in der Kadenz des RE1.5-Zwillings (RE1.5
schreibt aca58=3 direkt in der Entity-Schleife @0x80113f20, und RE1.5s Spieler-Dispatcher
@0x8001ce0c laeuft nach der Entity-Schleife @0x8001ce04 im selben Frame -> Clip startet
dort im selben Frame). Als dokumentierte 1-Frame-Divergenz in den Fix-Kommentar.

## (c) RE2-cmd-3 = FUN_8003fee4; Port-Aequivalent — CONFIRMED

Tabelle @0x800a4028 selbst gedumpt: Basis 0x800a4030, [0]=0x8003c210, [1]=0x8003c57c,
[2]=0x8003f600, **[3]=0x8003fee4**, [4]=0x80065c88 (im Diagnose-Dossier ausgelassen,
harmlos), [5]=0x8004006c, [6]=0x800400d0, [7]=0x80040134.

FUN_8003fee4 (eigener Dump 0x8003fee4-0x80040048): 0x800cfbd8|=0x40; **a1/a2 =
PL+0x108/+0x17C** (Prolog @0x8003ff08-0c = die NORMALE Spielerbank — Gegenprobe: cmd 5/6
laden stattdessen +0x188/+0x18C); Sub-Dispatch @0x800a40fc[PL+0x5]; Maschine 0x8003ff3c:
* Ph0 @0x8003ff9c: **Clip-Wort 0x70002 -> +0x14C** (eigene Bank, Clip 2, Rate 7);
  +0x144:=0 / +0x148:=0; +0x1D3|=0x80; +0x1C0:=1; **Se_on(0x04030001)** (a0 = lui 0x403 /
  ori 1 @0x8003ffa0-a4, jal 0x8005ba28 @0x8003ffc8); Shake/Rumble 0x8003947c(3,0) /
  0x80039514(8,200,0) / 0x800395b8(90,200,0,8); +0x6:=1.
* Ph1 @0x80040004: Advance 0x8002959c(a3=512), +0x6 += done.
* Ph2 @0x8004002c: `sw v0,4(s1)` mit v0=7 (Delay-Slot @0x8003ff90) = **PL-State-Wort := 7
  (cmd 7)**.

RE1.5-Praezedenz selbst nachgezogen: Tabelle @0x80073f90 gedumpt, **[3]=0x800366bc**;
dessen Ph0 @0x80036764: **Clip 7** (`sb 7 -> 0x800acae8` @0x80036780), Rate-Byte
0x800acae3:=7, Frame 0x800acae9:=0, +0x93|=1, **Se_on(0x04030001)** (a0 @0x80036744/64,
jal 0x80045024 @0x800367a8). Slots 3/5/6/7 beider Engines sind funktionsgleich belegt.

Port (`game_step_common.c:169-243`, gelesen): Ph0 motion=7/frame=0/frac=7/blend=0x200 +
`re15_audio_core_se(3)` (=Se_on 0x04030001); Ph1 zaehlt gegen
`c->pl00_anim->clips[7].frame_count` (**113 aus PL00.EDD**, nicht hartkodiert;
`tests/unit/probe_crow_kill.c:16` dokumentiert die Clip-Tabelle (113,1032)); Ph2
`pl->state = 7`. Das IST der strukturgleiche Kollaps. **Clip-Unterschied begruendet:**
der Port faehrt in ALLEN AI-Flavors das RE1.5-PL00-Rig (RE2-Modi tauschen nur
Gegner-Anim/-Modelle); RE2s "Clip 2 der eigenen Bank" hat kein Port-Bank-Gegenstueck,
RE1.5-cmd-3 (gleicher Tabellen-Slot, gleicher SE) ist das korrekte Analog.
**Dokumentation:** die RE1.5-Adressen stehen im Code; ein RE2-seitiger Mapping-Kommentar
(FUN_8003fee4/0x70002/Se_on/cmd 7) existiert an der Stelle noch NICHT — der im Fix
geplante Beleg-Kommentar ist die richtige Ergaenzung, kein Widerspruch.

## (d) victim_reset aus dem Kraehen-Release — CONFIRMED, kein halbzerlegter Zustand

* `re15_player_death_cmd3()` (game_step_common.c:169-179): idempotent (`s_death3_on`-Gate
  :171), ruft `re15_player_victim_reset()` (:177).
* `re15_player_victim_reset` (enemy_ai_common.c:1761-65) nullt ALLE Victim-Statics:
  g_player_victim, _type, _variant, s_victim_phase, _fresh, _standup, _groan_done,
  **g_player_victim_zombie=-1**, s_grab_mercy_timer.
* `re15_re2z_victim_begin` (enemy_ai_common.c:2350-2424) setzt am SPIELER nur
  Anim-Felder + Aim-Interrupt; **kein player->state, kein Flag-Bit**; der
  Yaw/t158-Block ist fuer 0x21 per `owns_type`-Gate uebersprungen. Es bleibt nichts
  zurueck, was reset nicht deckt.
* Pin: `s_player_grabbed` wird am run_all-Kopf JEDEN Frame geloescht (enemy_ai_common.c
  ~:13160) und vom Kraehen-grab_out nur bei `sub_state_1==14 && vs==1` neu gesetzt
  (enemy_ai_re2_crow.c:1157-59) — nach `re2c_sub(e,4)` und vs=0 nie wieder.
* Kraehe selbst: Sub 4 = ihr regulaeres Kreisen (Trichter @0x8010285c-64) — kein
  Aufraeumbedarf.
* Renderer: PL00-Bank-Wahl haengt an `re15_player_death_cmd3_active()`
  (anim_select_common.c:356-378) — der Bank-Wechsel weg von der Victim-Bank ist gedeckt;
  death_cmd3_tick laeuft im SELBEN game_step nach run_all (:1761), Ph0 ueberschreibt
  motion/frame/frac/blend vor dem naechsten Render.
* Der Selbst-Reset in cmd3_tick (:188-193, vs!=0 -> Abbruch) feuert nicht, weil
  victim_reset vs bereits genullt hat.

## (e) Fix-Risiken — Einzelbewertung

1. **"Unbedingter throwoff bricht vs==3":** ALS RISIKO WIDERLEGT.
   `re15_player_victim_throwoff` gatet SELBST `if (g_player_victim != 1) return;`
   (enemy_ai_common.c:1142) — das Call-Site-Gate in enemy_ai_re2_crow.c:1042 ist
   redundant, sein Entfall verhaltensneutral. (Rest-Divergenz nur akademisch: das
   Original wuerde +0x6=3 auch bei bereits laufendem Release erneut schreiben; im Port
   no-op. Unerreichbar, weil grab_release pro Grab genau einmal laeuft — jeder
   Trichter-Weg setzt sofort Sub 4 — und vs==3 einen frueheren Release voraussetzt.)
2. **"cmd3-Latch bricht Game-Over-FSM/Death-Cam":** WIDERLEGT. Game-Over-FSM keyt auf
   hp<0 (game_step_common.c:1035-36, Top-Level, nicht am Zweig), Death-Zweig :1099-1123
   keyt auf `re15_player_is_dead()`; die RE2-Pacing-Ausnahme (re15_damage.c:284-290)
   klammert nur Zombie-Familie+0x20 und ist nach victim_reset (vs==0) ohnehin inaktiv.
   Der RE1.5-Kraehen-Pfad faehrt EXAKT diese Kombination heute schon (drei
   death_cmd3-Sites enemy_ai_common.c:5880/5972/6062; probe_crow_kill pinnt motion=7 x
   113 F neben derselben FSM). Todes-Kamera unberuehrt (Gameover-FSM + RVD-Scan im
   Death-Zweig).
3. **RE1.5-Kraehe:** vom Fix unberuehrt (Sites liegen in enemy_ai_common.c;
   re15_player_death_cmd3 idempotent).
4. **Fix-Teil 2 (`pl->re2z_self1d3 |= 0x80` im re2z_player_damage-Todeszweig):**
   Original-Store selbst verifiziert @0x800402c0-d0 (lbu/ori 0x80/sb auf a2+467) +
   Todesbit 0x800cfb74|=0x04000000 @0x800402d4-e4; gilt fuer ALLE Caller
   (Zombie/Hund/Kraehe) — der Port-Nachzug ist damit auch fuer Zombie-/Hund-Kills
   byte-gedeckt. Port-Konsument EXISTIERT: Writher-Grab-Gate
   `!(pl->re2z_self1d3 & 0x80)` (enemy_ai_common.c:11773) — der Nachzug macht dieses
   Gate im Todesfall erst funktional (ohne ihn koennte nach dem Kraehen-Fix — vs==0,
   is_grabbed()==0 — ein Writher theoretisch die Leiche greifen). Der einzige
   Port-Clear (:1716, `&=0x7F` beim Release-Finish) laeuft nur fuer Zombie-Familie nach
   UEBERLEBTEM Grab — kein Leck ins lebende Spiel, da nur der Todeszweig 0x80 setzt.
   Kein Port-Code liest Spieler-1d3 als Cooldown (grep: nur :1716/:11773; alle anderen
   Treffer sind e->-Felder der Gegner).
   Todesbit-Konsument: Decompile-weiter grep nach `0x4000000` findet als Writer nur
   FUN_800401d4.c:35; der Leser @0x800266c8-0x80026734 (Task-2-Scheduler + Clear) ist
   selbst disassembliert. "Einziger Konsument" gedeckt (gleiche grep-Grenze wie oben).
5. **Timing:** Kollaps startet mit dem Fix 1 Frame frueher als RE2 (Messung s. (b)),
   identisch zur RE1.5-Zwillings-Kadenz -> als Divergenz-Kommentar dokumentieren.
6. **Platzierung im Claimed-Gate:** korrekt — auch das Original schreibt +0x6=3 nur
   unter +0x22A&4; der P0-hp<0-Exit mit Claim, aber ohne je begonnene Victim-FSM
   (Spieler stirbt anderweitig direkt vor Grab-P0) endet im idempotenten death_cmd3 —
   unkritisch.
7. **Alternative Fix-Stelle (Victim-FSM-Transition enemy_ai_common.c:1472):** deckt
   dieselben Faelle plus den hypothetischen "Kraehe verlaesst Sub 14 ohne grab_release
   waehrend Spieler tot" (Auto-Release ueber den Pin-Verlust — im Original gibt es
   diesen Weg NICHT, PL+0x6=3 hat nur den einen Producer). Praktisch unerreichbar, weil
   der Kill-Peck grab_release im selben Tick ruft. Beide Stellen ok; Release-Site ist
   naeher am Original-Producer.

## Sekundaerbehauptungen des Diagnose-Dossiers (Stichproben)

* §3.1/3.2 Port-Kill-Zweig + Release ohne Todes-Latch: bestaetigt
  (enemy_ai_re2_crow.c:1092-98, 1032-47 gelesen; 6 grab_release-Sites im Port spiegeln
  die 6 Original-Trichter-Refs).
* §3.3 Victim-Tick spielt dem Toten Release-Clip, dann Idle 200: bestaetigt
  (enemy_ai_common.c:1471-85 ohne HP-Check; :1699-1751 -> vs=0, motion=200).
* §3.4 Kanten-Detektor gegatet + One-Shot: bestaetigt (game_step_common.c:1005-08;
  is_grabbed zaehlt vs 1/2/3, enemy_ai_common.c:540-49; s_prev_hp-Update :1008).
  Kein death_cmd3-Aufruf im RE2-Kraehen-Code (grep).
* §3.5 Praesentation laeuft (Kraehe nicht in der Pacing-Ausnahme): bestaetigt
  (re15_damage.c:266-294).
* §4 STRIKE ohne HP-Verlust: bestaetigt (enemy_ai_re2_crow.c:921-40: nur
  hit_react|=1; einziger player_damage-Call der Datei ist :1092 = Grab-Peck).
  Spinnen-Randnotiz bestaetigt (enemy_ai_re2_spider.c:338 verwirft Rueckgabe; ihr
  Gate :337 mappt uebrigens Original-`+0x1D3&0x80` auf is_grabbed — nach Fix-Teil 2
  koennte dort das echte Bit stehen, optional).

## Gesamturteil

Alle fuenf Pruef-Punkte CONFIRMED; kein Befund widerlegt die Diagnose. Zwei
Praezisierungen: (1) Trichter-Aussage gilt fuer die Grab-Maschine (externe
Kraehen-Abbrueche schreiben gar keinen Spieler-State — deckt die Diagnose implizit);
(2) Timing des Fixes = exakt 1 Frame frueher als RE2, gleich dem RE1.5-Zwilling
(jetzt gemessen statt "±1 nicht gemessen").
