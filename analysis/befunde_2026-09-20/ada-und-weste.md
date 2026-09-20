# Runde 17 — „ada-und-weste": Adas doppelte Animation + die verschwundene Weste

Nutzer-Befunde vom 2026-09-20:

> „dann im Raum der cutscene wo man ada aus dem feuer rettet, und dann mit ihr den Raum
> wechselt, spielt ihre Animation am Anfang doppelt ab."

> „dann sammelt man die weste ein speichert und lädt den Spielstand Ist die weste weg."

---

## 0. Kurzfassung

**Weste — GELÖST und gepinnt.** An der echten exe gemessen: nach dem Laden eines
Spielstands mit angelegter R.P.D.-Weste zieht der Port das Spielermodell nicht nach.
Der Save-Block kommt vollständig zurück (Flag(3,0x75), HP 105 mit Bonus), aber der
Modell-Lader hing ausschließlich an den beiden RAUM-Wegen — und der CONTINUE-Boot geht
durch keinen von beiden. Das Original zieht das Modell im Raumlader nach
(`FUN_800396fc` @0x80039760-8c → `FUN_800314b0`), und genau dieser Zug steht jetzt am
Lade-Weg des Ports. Riegel: `integration_weste_load_pin` (6,4 s, Gegenprobe rot).

**Zensus der übrigen Ausrüstungs-/Modellzustände — gemessen, kein weiterer Defekt.**
Blut-Decals und Waffen-Equip überleben das Laden; sie hängen an Pro-Bild-Vergleichen
(Wund-Generation / Waffen-Schlüssel), nicht an einem Raumwechsel-Haken.

**Ada — TEILERGEBNIS, ehrlich benannt.** Beide Kandidaten-Räume sind reproduziert und
Ada bildweise protokolliert. Dabei ist **ein** byte-true Defekt gefunden und behoben
worden, der genau zur Fehlerklasse „eine Animation läuft falsch/doppelt" gehört: der
Port ließ beim `Plc_dest` das RÜCKWÄRTS-Bit +0x1c4 stehen, das Original nullt das Wort
dort unbedingt (`sh zero,452(a1)` @0x80041c4c). **Nicht abgeschlossen** ist die Frage,
ob damit genau das gemeldete Symptom weg ist — dazu fehlt mir die Angabe, welcher der
beiden Momente gemeint ist (§5).

---

## 1. Reproduktion und Messung

Alle Läufe mit der echten exe aus diesem Arbeitsbaum
(`re15_port/build_r17/platform/pc/re15_pc.exe`), Softwarerenderer, `RE15_STATE_LOG`
für die bildweise Zustandsspur und `debug.log` für die Opcode-Spur.

### 1.1 Weste — der gemeldete Zustand, nachgestellt ohne Handspiel

Neues Messwerkzeug `probe_r17_weste_karte` (tests/unit): schreibt eine PSX-Karte mit
einem Spielstand, in dem Flag(3,0x75) = 1 (der save-persistente Träger, siehe
`scd_room_setup.c:131`) und der Raum 0x1190 ist. Damit lässt sich die exe per
`RE15_CONTINUE_TEST` + `RE15_CARD_AUTO` direkt in den Nutzer-Zustand laden.

    Karte re15_card.mcr geschrieben: Slot 0, Raum 0x1190, Flag(3,0x75)=1, work_vars[0x10]=1

Lauf **vor** dem Fix, `debug.log` vollständig durchsucht:

    43:[save] CONTINUE: resumed in room 1190 (hp=105)
    (keine einzige [pld]-Zeile im ganzen Log)

Der Save-Block ist also korrekt zurück — HP 105 ist der Westen-Bonus — aber das
Spielermodell wurde nie gewechselt: Leon steht in PL00, ohne Weste. Genau der
Nutzer-Befund.

Lauf **nach** dem Fix, derselbe Aufruf:

    43:[pld] Spielermodell -> PLD/PL01.PLD (Mesh 17 Teile, Textur 384x256)  [R.P.D.-Ruestung an]
    44:[save] CONTINUE: resumed in room 1190 (hp=105)

**Sichtprüfung** (echter Renderpfad, Vollbild-Readback vor `SDL_RenderPresent`, kein
`AUTOSHOT`, kein Softwarerenderer; gleiche Karte, gleiche Stelle, Kamerawinkel 13):

* `ada-und-weste/weste_vor_fix.png` — Leon in der dunkelblauen Uniform (PL00).
* `ada-und-weste/weste_nach_fix.png` — Leon in der roten R.P.D.-Weste (PL01).

Beide Bilder selbst angesehen; der Unterschied ist der Oberkörper.

### 1.2 Zensus: Wunden, Waffe, Kamera

Dieselbe Karte, zusätzlich mit Waffe 3 und allen acht Wund-Panels auf Level 2
(`probe_r17_weste_karte re15_card.mcr 1190 3 2`). CONTINUE-Lauf:

    32:[wpn] loaded 21/21 PL00W** in-hand meshes (textured from body-skin slot 0)
    60:[wound] sync: applied=1 ok=1 gen=1 slotgen=10

* **Blut-Decals: in Ordnung.** Die Stempel werden nach dem Laden aufgebracht, und zwar
  auch noch, nachdem der neue Modellwechsel den Textur-Slot 0 neu hochgeladen hat
  (`slotgen=10`) — der Abgleich hängt an `re15_wound_generation()` **und** an der
  Slot-0-Generation und läuft in jedem Bild (`main.c:7754`).
* **Waffen-Equip: in Ordnung.** `weapon_id` kommt aus dem Save (`re15_savedata.c`), die
  Waffen-SE-Bank wird am CONTINUE-Block ausdrücklich neu gesetzt, und das
  Hand-Mesh/der Slot-0-Verbund werden über einen Pro-Bild-Schlüsselvergleich gebaut
  (`eqw > 0 && (key != s_wpn_key || gen != s_wpn_gen)`, `main.c:7795`) — kein
  Einmal-Haken am Raumwechsel.
* **Kamerawinkel: in Ordnung.** `s_resume_cut` wird nach der Raum-Init gesetzt.

Der Weste-Fall war also die einzige Stelle, die an einem Raumwechsel-Haken hing.

### 1.3 Ada — die beiden Kandidaten-Räume

Wer „Ada" ist, ist im Repo belegt: Typ **0x42**, Wurzel 0x8011cb70, CDEMD0.EMS Blob 19
(`analysis/nutzer_batch_2026-08-28/ada-animation.md` A1). Das Skript nennt sie „Woman".

Eigener Zensus über die Raumskripte (`re15_scd_walk.py` über die ausgelieferten RDTs):

* **ROOM1090** — die Rettung aus dem Feuer. `sub00` @0x22A2: `Ck(3,0x84)==1` →
  `Obj_model_set 2` + `Sce_em_set` Typ 0x42 @0x22CC + `Evt_exec sub03`.
  `sub03` @0x24CE setzt zuerst `Set(3,0x6e,1)` — das Tor für den Folgeraum.
* **ROOM1050** — der Raum danach. `sub00` @0x0C86: `Ck(3,0x6e)==1` → `Sce_em_set`
  Typ 0x42 @0x0C8E + `Evt_exec sub03`; `sub03` @0x0D88 ist die Szene „Ada läuft davon".

Beide wurden gefahren.

**ROOM1090** (`RE15_DEBUG_JUMP=1090@30 RE15_SET_FLAG=3:0x84`, Dialoge per
`RE15_INPUT_SCRIPT` durchgetickt). Ada = Aktor-Slot 1; Spalten aus dem Zustandslog
(`st` = +0x4, `ss1` = +0x5, `ss2` = +0x6 Phase, `mo` = +0x94 Clip, `af` = +0x95):

| Bild | st | ss1 | ss2 | mo | Bedeutung |
|---|---|---|---|---|---|
| 1   | 0 | 0 | 0 | 0  | INIT |
| 6   | 4 | 6 | 0 | 2  | INIT-Ende: grid_id 0x40 → State 4 / Sub 6 (@0x8011ce1c-40) |
| 54  | 4 | 6 | 1 | 1  | Sub-6-Phase 0 → **Clip 1** |
| 69  | 4 | 6 | 2 | 1  | Clip 1 zu Ende |
| 70  | 4 | 6 | 3 | 2  | Clip 2 (Ruhe) |
| 82  | 4 | 6 | 1 | 1  | **Clip 1 ZUM ZWEITEN MAL** — ausgelöst vom `Plc_dest` @ROOM1090.RDT 0x2564 |
| 84  | 4 | 1 | 1 | 23 | `Plc_motion(1,23)` + `Plc_flg(0,4)` |
| 190 | 4 | 1 | 1 | 11 | `Plc_motion(1,11)` |
| 214 | 4 | 1 | 2 | 11 | Halt auf Bild 24 |
| 350 | 4 | 1 | 1 | 11 | `Plc_motion(1,11)` **erneut** + `Plc_flg(0,0x80)` |
| 382 | 4 | 9 | 1 | 5  | `Plc_dest(…,9,…)` = Gehen |
| 399 | 4 | 1 | 1 | 23 | `Plc_motion(1,23)` + `Plc_flg(0,4)` |
| 672 | 1 | 0 | 0 | 23 | `Plc_ret` → Eskorte |

**ROOM1050** (`RE15_DEBUG_JUMP=1050@30 RE15_SET_FLAG=3:0x6e`):

| Bild | ss1 | ss2 | mo | |
|---|---|---|---|---|
| 1  | 9 | 0 | 0 | `Plc_dest(…,9,…)` aus sub03 @0x0DAA |
| 9  | 6 | 1 | 1 | Clip 1 |
| 25 | 6 | 3 | 2 | Clip 2 |
| 35 | 5 | 2 | 0 | Gehen (sub04 @0x0E04) |
| 64 | 6 | 0 | 0 | Ankunft |
| 65 | 5 | 2 | 0 | zweites Gehen (sub04 @0x0E18) |
| 79 | 6 | 1 | 1 | **Clip 1 ein zweites Mal** |
| 95 | 6 | 3 | 2 | Clip 2 — bleibt bis Bild 853 (Endlage 850,-14200 = `Member_set` @0x0E2C/0x0E30) |

---

## 2. Original-Mechanismus (@0x)

Alles hier selbst aus `info/Re1.5/PSX.EXE` disassembliert
(`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`).

### 2.1 Der Lade-Weg (Weste)

    80026294  addiu a0,a0,3516      ; a0 = 0x800b0dbc  (Save-Block)
    8002629c  jal   0x8004ee38      ; memcpy
    800262a0  ori   a2,zero,0x1430  ; 0x1430 Bytes

`DAT_800b0ff0` (= `work_vars[0x10]`, die Spielermodell-Variante) liegt bei Block+0x234,
wird also vom LOAD mit zurückgeschrieben. Danach läuft der Raumlader und zieht das
Modell nach:

    80039760  lbu  a0,0x0(s0)=>DAT_800aca5c   ; Modell/Waffen-Byte
    80039768  lh   v1,DAT_800b0ff0            ; work_vars[0x10]
    8003976c  andi v0,a0,0xf                  ; untere Nibble = Modell-Index
    80039770  beq  v0,v1,LAB_80039790         ; unverändert -> nichts tun
    80039774  _andi v0,a0,0xf0
    80039788  jal  FUN_800314b0               ; sonst Spielermodell NEU LADEN

`FUN_800314b0` @0x800314d4-1c liest die Datei-Id aus der Tabelle @0x80073f70
(16 u16: 0x3C…0x4B) → Datei-Id = 0x3C + Index; **Index 1 = PL01.PLD = die
R.P.D.-Rüstung** (Mesh/Textur-Vergleich PL00/PL01 in `main.c:1040ff`).

### 2.2 Die drei Animations-Opcodes

**`Plc_motion` (0x3F), Handler @0x80041b90** (Tabellen-Eintrag
0x800744a8 + 0x3F·4 = 0x800745a4):

    80041b98  lbu a2,1(v0)        ; pc[1] = Sub
    80041b9c  lhu a1,2(v0)        ; pc[2..3]
    80041ba8  sb  a1,148(v0)      ; +0x94 = Clip
    80041bb0  sb  v1,4(v0)        ; +0x4  = 4  (State-4-Executor)
    80041bb4  sb  zero,6(v0)      ; +0x6  = 0  (Phase zurück)
    80041bb8  sb  zero,7(v0)      ; +0x7  = 0
    80041bc4  sb  a2,5(v0)        ; +0x5  = pc[1]
    80041bc8  sh  a1,452(v0)      ; +0x1c4 = pc[3]  (Flags, ÜBERSCHREIBEND)

**`Plc_flg` (0x43), Handler @0x80041fb8** — schreibt ausschließlich +0x1c4:

    80041fec/ffc/8020   Modus 0: +0x1c4 |= wert      (OR)
    80042000/8008       Modus 1: +0x1c4  = wert      (SET)
    8004200c/801c/8020  Modus 2: +0x1c4 ^= wert      (XOR)

**`Plc_dest` (0x40), Handler @0x80041be4:**

    80041bec  lhu  v0,452(a1)     ; +0x1c4
    80041bf8  andi v0,v0,0x4
    80041bfc  beq  v0,zero,0x80041c14   ; Bit 4 aus -> immer schreiben
    80041c04  lbu  v0,5(a1)             ; +0x5
    80041c0c  beq  v0,a2,0x80041c24     ; gleicher Sub -> Re-Init ÜBERSPRINGEN
    80041c14  sb   v0,4(a1)             ; +0x4 = 4
    80041c18  sb   a2,5(a1)             ; +0x5 = Sub
    80041c1c  sb   zero,6(a1)           ; +0x6 = 0
    80041c20  sb   zero,7(a1)           ; +0x7 = 0
    80041c24  sb   v1,451(a1)           ; +0x1c3 = pc[3]      <- Ziel des Guard-Sprungs
    80041c4c  sh   zero,452(a1)         ; +0x1c4 = 0          <- UNBEDINGT, hinter dem Guard
    80041c50  sltiu v0,v0,0x10          ; Typ < 0x10 ? (Spieler/Gegner-Zweig)

### 2.3 Wo die Abspielrichtung herkommt

Motion-Sub der State-4-Maschine, @0x80050cb8:

    80050cec  sb  v0,6(a0)        ; Phase 0 -> 1
    80050cfc  sb  v0,143(v1)      ; +0x8f = 7   (Überblendung)
    80050d0c  sb  zero,149(v0)    ; +0x95 = 0   (Clip von vorn)
    80050d24  andi v0,v0,0x40     ; Bit 0x40 = kein Überblenden
    80050d44  lbu a2,452(v0)      ; +0x1c4
    80050d50  srl a2,a2,7         ; a2 = BIT 0x80 = RICHTUNG
    80050d4c  jal 0x8001f314      ; anim_set(bank, bank, richtung, 0x200)
    80050da4  sb  v0,6(v1)        ; Clip zu Ende -> Phase 2 (halten)
    80050dc8  sb  v0,6(v1)        ; ... aber mit Bit 0x04 zurück auf Phase 1 (Schleife)

`anim_set` @0x8001f314 spiegelt daraufhin den Bildindex:

    8001f338  beq   t2,zero,0x8001f358   ; Richtung 0 -> vorwärts
    8001f344  lbu   v0,149(t0)           ; sonst v0 = +0x95
    8001f34c  subu  v0,t1,v0             ; t1 = Clip-Länge
    8001f354  addiu v0,v0,-1             ; Index = Länge - Bild - 1

Das heißt: **die Richtung ist im Original kein Aktor-Zustand, sondern das dritte
Argument jedes einzelnen `anim_set`-Aufrufs**, und die Quelle dafür ist +0x1c4, das
`Plc_dest` und `Plc_motion` jedes Mal überschreiben.

---

## 3. Port-Ist

1. `re15_port/platform/pc/main.c` (CONTINUE-Restore): `re15_savedata_restore`
   rekonstruiert `work_vars[0x10]` aus Flag(3,0x75) (`re15_savedata.c:206`), aber
   `pc_sync_player_model` (main.c:1108) wird nur aufgerufen
   * aus dem Raumwechsel (main.c:6630) und
   * über den Rückruf aus `scd_room_reenter` (`re15_scd.h:602`).
   Der CONTINUE-Boot startet die SCD-Threads direkt und geht durch keinen von beiden.
   → **Weste fehlt nach dem Laden.**

2. `re15_port/engine/src/scd_vm.c`, `op_plc_dest`: das Nullen von `anim_flags` (+0x1c4)
   stand INNERHALB des Re-Init-Zweigs und zusätzlich nur im Nicht-Spieler-Zweig. Im
   Original steht der Store hinter dem Zusammenfluss (@0x80041c4c) und gilt für jeden
   Slot. Messbar in der ROOM1090-Szene:

       [scd F139] Plc_flg(subop=0, mask=0x0080) -> slot=0 anim_flags=0x0080
       [scd]      Plc_dest(slot=0 mode=0x06 ...) -> state4/sub6

   Der Spieler betritt den Mode-6-Event-Reach mit stehengebliebenem Rückwärts-Bit; im
   Port ist dieses Bit Aktor-Zustand (`anim_select_common.c:52`), die Folge-Clips liefen
   also rückwärts. Der Kommentar an derselben Stelle warnt genau davor („ein
   stehengebliebenes Bit spielt den nächsten Clip rückwärts — im Original unmöglich").

---

## 4. Umsetzung

**Weste** (`main.c`, CONTINUE-Restore): `pc_player_model_sync_cb()` direkt nach dem
Restore, vor dem ersten Spiel-Tick — der Analogpunkt zu @0x80039760-8c. Der
`beq`-Vergleich steckt in `pc_sync_player_model` selbst, ein neues Spiel
(`work_vars[0x10] == 0`) ist damit ein No-op.
Riegel `integration_weste_load_pin` (`tests/integration/test_weste_load_pin.cmake`,
registriert aus `tests/unit/probes/r17_ada-und-weste.cmake`): schreibt eine Karte mit
angelegter Weste, fährt die echte exe per CONTINUE hinein und verlangt die
PL01-Zeile im `debug.log`. 6,4 s. **Gegenprobe gefahren**: mit auskommentiertem
Nachzieher fällt der Riegel rot.

**Rückwärts-Bit** (`scd_vm.c`, `op_plc_dest`): `a->anim_flags = 0;` steht jetzt
unbedingt und für jeden Slot an der Stelle von @0x80041c4c.

---

## 5. Offen

1. **Welcher Moment ist mit „ihre Animation am Anfang" gemeint?** Ich habe beide
   Kandidaten gefahren und in BEIDEN eine Stelle gemessen, an der Adas Clip 1 ein
   zweites Mal startet (ROOM1090 Bild 82, ROOM1050 Bild 79). Beide Male ist der Auslöser
   ein `Plc_dest` des Raumskripts, und beide Male ist der Zustands-Reset nach
   @0x80041c14-20 byte-true, weil der Idempotenz-Guard (@0x80041bf8-c0c) nur mit
   gesetztem Bit 0x04 greift und Ada dort +0x1c4 = 0 trägt. **Ich kann diese beiden
   Wiederholungen also NICHT als Port-Defekt belegen** und habe sie deshalb auch nicht
   „gefixt". Was ich bräuchte: ob der Nutzer die Szene in ROOM1090 (Rettung) oder die
   in ROOM1050 (Ada läuft davon) meint, und ob die Doppelung vor oder nach dem
   Raumwechsel auftritt.
2. **State-4-Sub 6 ist nicht zu Ende zurückgebaut.** Der Port spielt dort Clip 1 → Clip 2
   (`re15_npc_sub_event_reach`, @0x800517f0). Die Eskorten-Variante @0x8004ffc0 dagegen
   schreibt +0x94 = 6 (@0x8005001c) und danach +0x94 = 7 (@0x800500a8) aus BANK 1 — und
   BANK 1 von Typ 0x42 hat nur sechs Clips {22,16,52,1,105,30}. Diese Unstimmigkeit steht
   schon seit 2026-08-28 als offener Punkt 3 im Dossier `ada-animation.md` und ist hier
   nicht geschlossen worden.
3. **Sichtprüfung der Ada-Szene fehlt.** Ich habe die Zustandsspur gemessen, aber keine
   Bilderfolge der Szene angesehen; die Zustandsspur allein kann Vorwärts von Rückwärts
   nicht unterscheiden (`+0x95` zählt in beide Richtungen hoch). Für die Weste ist die
   Sichtprüfung gefahren (§1.1), für Ada nicht.
