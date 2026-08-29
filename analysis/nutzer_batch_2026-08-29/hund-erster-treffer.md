# Hund: „Der erste Schusstreffer ist sehr verzoegert" (RE1.5-Modus)

Recon-Dossier 2026-08-29. Alle Adressen selbst nachdisassembliert
(`re15_disasm.py`, STAGE1.BIN laedt @0x80100000, Datei-Offset = Adresse−0x80100000; EXE-Adressen aus PSX.EXE).

## Kurzfassung (Verdict)

Die REAKTIONS-Kette des Ports ist **nicht** verzoegert — Registrierung und Flinch laufen im
**selben game_step-Frame** wie der Abzug (Original: +1 Frame, weil dessen Entity-Schleife VOR dem
Spieler-FSM laeuft). Der einzige gefundene, hund-spezifische Mechanismus, der Treffer **verschluckt**,
ist das **nicht portierte Hoehenband des Hundes** (im Port ausdruecklich als OPEN markiert):

- **Original** (Hund-ACTIVE-Tail, jeden state-1-Frame, unkonditional nach den beiden Dispatches):
  `word0 &= 0x1fffffff` → wenn Clip ∉ {1, 0x13}: `word0 |= 0x40000000` (LEVEL) →
  `jal 0x80012974(0xfa0)`: Distanz < 4000 → `word0 |= 0x20000000` (DOWN).
  ⇒ Ein Hund in Reichweite < 4000 ist **auch mit nach UNTEN gezieltem Schuss** treffbar; ein
  IDLE-Hund (Clip 1) oder der Low-HP-Hop (Clip 0x13) ist **nur** mit Down-Aim treffbar.
- **Port** (`re15_damage.c:1355-1358`, Kommentar `1349-1354` „OFFEN"): Hund-eband = **immer LEVEL**
  (`0x40000000`), nie DOWN. Zielt der Spieler mit **Dpad-DOWN** (naheliegend beim niedrigen Hund),
  ist `(pband & eband) == 0` → der Hund ist **nie Kandidat** → der Schuss trifft **gar nicht**
  (Muendungsfeuer + Knall spielen, kein Jaulen, kein Flinch, kein Schaden).
  Erst ein LEVEL-Schuss registriert ⇒ „erster Treffer stark verzoegert".

## 1) Port-Stand — Kette Schuss → Resolver → Hund-Reaktion

1. **Abzug**: `game_step_common.c:1337-1344` — SQUARE gehalten in HOLD (`re15_player_aim_ready()`)
   → `re15_player_fire_start()` + **sofort im selben Frame** `re15_player_weapon_fire(eq_item)`
   (byte-true zum Original-Discharge @0x800337bc, Resolver FUN_80011f50).
2. **Resolver** `re15_damage.c:1176ff` (`re15_player_weapon_fire`):
   - Kandidaten-Gates: Hitbox vorhanden (`:1209`), kein Corpse (`:1210`),
     `(hit_react & 3) == 3` → skip (`:1224`), **Hoehenband** `(pband & eband) == 0` → skip (`:1361`),
     Gun-Wedge (`:1385-1387`).
   - Treffer-Anwendung **synchron**: `+0x5 = weapon_id` (`:1413`, @0x800124bc), `hp -= dmg` (`:1428`),
     `+0x93 |= 1` (`:1429`, @0x800124f8), `sub_state_3 = 0` (`:1445`, @0x80012428),
     `state = hp>=0 ? 2 : 3` (`:1446`, @0x80012520).
3. **Hund-Reaktion**: `re15_enemy_ai_run_all` laeuft **am ENDE desselben Steps**
   (`game_step_common.c:1673-1674`). Dog HURT-Dispatcher (`enemy_ai_common.c:7579ff`,
   FUN_801108f0, Zeilen-Tabelle @0x80121018 selbst gedumpt: Rows 7-11/13-18/21 = airborne, Rest
   grounded — deckungsgleich mit `re15_dog_row_airborne` `:7077-7080`):
   - **Grounded-Flinch Phase 0** (`:7584-7590`): sofort Clip 6 (@0x80110a3c-70) + **Se(1)**
     (@0x80110a7c) + Blut Bone 1 (@0x80110a84-aa4). **Kein Warten auf Anim-Ende, keine
     State-Prioritaet** — take_damage ueberschreibt jeden State (auch 4/5/6).
   - **Frames nach Treffer bis Flinch: 0** (gleicher game_step). Original: Entity-Schleife
     @0x8001ce04 laeuft VOR dem Spieler-FSM (Kommentar `game_step_common.c:1677-1679`) ⇒ dort +1 Frame.
     Der Port ist also eher 1 Frame SCHNELLER, nie langsamer.
4. **Gates beim ERSTEN Treffer**: keine. `+0x93` Bit 0 ist vor dem ersten Treffer 0
   (Resolver setzt es erst `:1429`); der Latch verschluckt nur **Folge-Schuesse waehrend des
   laufenden Flinches** (`:1406-1409` Rekursion, @0x8001240c-18) — byte-true, der Hund cleart
   +0x93 am Flinch-Ende (`:7603` @0x80110b70 bzw. `:7637` @0x80110d90).
   Ausnahme ROOM1190-Skript-Sprung: `+0x93 |= 3` @0x801113f8 (Port `:6966`) — waehrend des
   SCD-Drops unverwundbar, Clear @0x80111610 (Port `:7005`) — **in beiden Seiten identisch**.

## 2) Original — Damage-Reaktion des Hundes (Root 0x8010d7f8)

- FUN_80011f50 schreibt `+0x4 = 2` synchron im Spieler-Action-Frame (@0x80012520).
- Hund-Root dispatcht state 2 → FUN_801108f0 → Fall-Physik (@0x80110900-4c, `+0x7==0` erzwingt
  Row 7 wenn y ueber `+0x1ba`), dann `table@0x80121018[+0x5]`:
  - Grounded FUN_801109e0 Phase 0: Clip 6 (@0x80110a3c-70), Se(1) (@0x80110a7c), Blut. **Sofort im
    naechsten Entity-Tick**, kein Anim-Ende-Gate.
- **ACTIVE-Tail = der Band-Stempel** (FUN_8010dbcc, selbst disassembliert, laeuft unkonditional
  nach DECISION-Tabelle @0x80120f94 und ACT-Tabelle @0x80120fd4 — `jalr` @0x8010dcbc / @0x8010dcf0):
  ```
  8010dd00: lui v1,0x1fff / lw v0,0(a0) / ori v1,v1,0xffff / and / sw   ; word0 &= 0x1fffffff
  8010dd20: lbu v1,148(a0)            ; +0x94 aktueller Clip
  8010dd24: ori v0,zero,0x1
  8010dd28: beq v1,v0,0x8010dd48      ; Clip 1  (IDLE)  -> LEVEL ueberspringen
  8010dd2c: ori v0,zero,0x13
  8010dd30: beq v1,v0,0x8010dd48      ; Clip 0x13 (Low-HP-Hop) -> LEVEL ueberspringen
  8010dd38: lw v0,0(a0) / lui v1,0x4000 / or / sw    ; word0 |= 0x40000000 (LEVEL)
  8010dd48: jal 0x80012974 / ori a0,zero,0xfa0       ; Distanz-Helfer, R=4000
  ```
- **FUN_80012974** (EXE, selbst disassembliert): `dist = SquareRoot0((plX−ex)^2+(plZ−ez)^2)`
  (Spieler @0x800aca88/90 vs. entity+0x34/+0x3c);
  `@0x800129cc: sltu s0,a1,s0` (dist < R) → `@0x800129d4-f0: word0 |= 0x20000000` (DOWN), kein Clear.
- **Resolver-Band-Gate** (EXE, selbst disassembliert):
  ```
  800120d0: lw v0,0(s0)            ; Gegner word0
  800120d8: lw v1,-13740(v1)       ; Spieler word0 (0x800aca54)
  800120e0: and v0,v0,v1
  800120e4: lui v1,0xe000 / and
  800120ec: beq v0,zero,0x80012128 ; kein Ueberlapp -> Kandidat VERWORFEN
  800120f4: lw v0,144(s0) / and 0x3000000 / beq==  ; (+0x93 Bits 0+1 beide) -> verworfen
  ```
⇒ **Original-Timing des ersten Treffers: Registrierung sofort, Flinch im naechsten Entity-Tick
(+1 Frame). KEIN „erst nach Anim-Ende".** Ein Delay im Original existiert nicht — und im Port
ebensowenig, SOBALD der Schuss ueberhaupt als Treffer registriert.

## 3) Schadens-Registrierung vs. Feedback (Sound)

- **Knall**: nicht im Discharge-Pfad, sondern ROW-VM-Routine 9 des Muendungs-Effekts am 2. Tick des
  Slots (`game_step_common.c:1397-1404`, `re15_esp.c:446`; Original @0x80017654 →
  FUN_80045024(0x1000001)). Spielt auch bei Whiff — deshalb klingt ein verschluckter Schuss
  „normal", nur der Hund reagiert nicht.
- **Treffer-Jaulen**: Se(1) am Flinch-Eintritt — Port `enemy_ai_common.c:7587` (grounded) /
  `:7617` (airborne); Original @0x80110a7c / @0x80110c68. Gleicher Frame wie die Registrierung
  (Pump-Latenz `audio_pc.c:767-784` ≤ 1 Frame). SE 3 („Treffer/Latch" im Probe-Kopf) ist der
  GRAB-Latch @0x8010f84c, nicht der Schusstreffer.
- ⇒ Ein per Band verworfener Schuss erzeugt **weder Jaulen noch Flinch noch Blut** — exakt das
  Symptom „Treffer kommt (viel) spaeter".

## 4) probe_hund_bellen.c (untracked)

`re15_port/tests/unit/probe_hund_bellen.c` ist eine reine SE-ZAEHL-Sonde zum aelteren Befund
„Hunde bellen nicht" (Bell-Tor @0x8010e194-1c8, kein add_test). Fuer den Treffer-Verzug ohne
Befund, aber der SE-Spion (`g_test_room_se_log`) ist die richtige Infrastruktur fuer die Messung
unten.

## 5) Wahrscheinlichste Ursache + Fix-Vorschlag

**Ursache (Divergenz PROVEN, Kausalitaet fuer den Nutzer-Report LIKELY):** Der Port stempelt dem
Hund nie das Original-Hoehenband. Wer beim Hund (natuerlich) **nach unten zielt**, trifft im Port
**nie**, im Original innerhalb 4000 Einheiten immer. Gegenrichtung ebenfalls falsch: der Port
trifft IDLE-Hund (Clip 1) und Hop (Clip 0x13) mit LEVEL-Schuessen, das Original nicht.
Die Kraehe hatte exakt dieselbe Fehlerklasse und ist bereits ueber `e->aim_band` gefixt
(crow_shot_attack.md F5; `re15_damage.c:1238-1246`) — der Hund ist der liegengebliebene Zwilling
(OPEN-Marker `re15_damage.c:1349-1354` und `enemy_ai_common.c:7568`).

**Fix (byte-true, jede Konstante mit Adresse):**
1. Im Hund-ACTIVE-Tail (`enemy_ai_common.c` nach dem ACT-Dispatch, bei `:7567-7571`) den Stempel
   nachziehen (z. B. in `e->aim_band` wie bei der Kraehe):
   - `band = 0` — @0x8010dd00-10 (`word0 &= 0x1fffffff`)
   - `if (e->motion != 1 && e->motion != 0x13) band |= LEVEL` — @0x8010dd20-44
   - `dist(Spieler-XZ, Hund-XZ) < 0xfa0 → band |= DOWN` — @0x8010dd48-4c + @0x800129cc-f0
   (Nur der state-1-Tail schreibt/cleart; in HURT/DEATH bleibt der letzte Stempel stehen —
   genau wie im Original, wo ausserhalb des Tails kein Clear existiert.)
2. Resolver `re15_damage.c:1355-1358`: fuer Typ 0x20 den Stempel lesen statt der
   LEVEL-Konstante (Muster Kraehe `:1244-1246`).
3. Wache/Pin: Probe „Hund dist 2000, `re15_player_set_aim_elevation_for_test(-1)`,
   `re15_player_weapon_fire(3)`" — heute Rueckgabe 0 (Whiff), nach Fix Slot+1; und
   „IDLE-Hund (Clip 1), Elevation 0" — heute Treffer, nach Fix 0.

## 6) Offen (nur dynamisch entscheidbar)

- **Kausalitaets-Bestaetigung**: ob der Nutzer beim Hund tatsaechlich Dpad-DOWN haelt. Messung:
  (a) Port-Probe wie oben (beweist den Whiff deterministisch), (b) temporaerer Zaehler im
  Band-Skip (`re15_damage.c:1361`) waehrend eines echten Hunde-Gefechts — inkrementiert er beim
  Nutzer-Input, ist die Ursache bestaetigt; (c) Gegenprobe DuckStation ROOM1190: Hund < 4000,
  R1 + Dpad-DOWN, Schuss → Flinch (und Savestate: Hund-word0 Bit 0x20000000 gesetzt).
- **Spawn-Wert des Bandes**: ob Sce_em_set (FUN_800420a0) word0-Topbits wirklich 0 laesst (dann
  ist ein noch nie ACTIVE gewesener Skript-Hund im Original unbeschiessbar; der Port ist da heute
  permissiver). Vor dem Fix per Savestate-RAM verifizieren.
