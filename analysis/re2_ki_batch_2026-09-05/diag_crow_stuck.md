# Diagnose-Dossier: "Die Kraehen bleiben manchmal einfach haengen" (RE2-Modus)

Datum 2026-09-05. Symptom (Nutzer, RE2-Modus): Kraehen frieren in irgendeinem Zustand ein /
bewegen sich nicht mehr sinnvoll.

Gepruefte Artefakte:
- Port: `re15_port/engine/src/enemy_ai_re2_crow.c` (1919 Z., vollstaendig gelesen),
  `enemy_ai_common.c` (Hook, body_push, LOS-Shim, steer), `re15_damage.c` (Writer/Hitbox),
  `enemy_ai_re2_dog.c` (Advancer, gflags), `scd_room_setup.c`, `actor_common.c`,
  `player_common.c` (Advancer-Liste).
- Original: `info/re2leon/COMMON/BIN/EMOVL21_S0.BIN` — EIGENE Disassemblierungen dieser Sitzung
  (re2_disasm.py) der Stellen 0x801041F0-3F4 (Arbiter), 0x80104094-180 (Wand-Prober),
  0x80100544-5F8 (ACTIVE-Prolog/Navigator-Aufruf), 0x801006C0-724 (dec0-Kopf/Nie-wecken),
  0x80100754-79C (Wake-Timer), 0x80101D30-84 (dec11), 0x80101EE8-F84 (exec11-Commit),
  0x801021EC-25C (dec13), 0x80102328-3AC (exec13-Steer), 0x80102410-46C (Hover-Stopp/Grab),
  0x8010125C-80 (exec4-Steer), 0x801044B0-524 (Post-Pass). RE2-EXE-Decompilate:
  `RE2_Quellcode_V2/FUN_800355c4.c`, `FUN_80034d0c.c`, `FUN_8004a808.c`, `FUN_80050858.c`.
- Tests: `tests/unit/test_re2_crow_ai.c`, `tests/unit/test_re2_room10c0_ab.c` (vollstaendig).

---

## 0. Kernbefund (Kurzfassung)

Der Zustandsautomat des Ports ist gegen das Original weitgehend byte-gematcht (alle in dieser
Sitzung nachdisassemblierten DEC/EXEC-Stellen stimmen instruktionsgenau). Die Haenger kommen
mit hoher Wahrscheinlichkeit NICHT aus einem Timer-/Mutex-/Phasen-Fehler, sondern aus dem
Zusammenspiel zweier DEKLARIERTER MAPPINGs mit echter Raumgeometrie:

1. **Navigator-MAPPING** (Steuerziel = Spieler statt Routen-Knoten `FUN_8004A808`/`0x8004AA50`)
   laesst Kraehen in die SCA-Kulisse beelinen — Wand-Flatter-Schleifen (Sub 4 ↔ 8/10) und
   Anpress-Situationen, die das Original per Routen-Wegpunkten strukturell vermeidet.
2. **LOS-Shim-MAPPING** (`re15_re2_los_clear` = RE1.5-Region-Ray statt RE2-`0x80050858`
   Mode 0x8400 = Kollisionslinien-Ray): Sub 11/13 haben als EINZIGE geometrische Exits
   Kegel + LOS-Bit. Meldet der Shim "frei", wo die SCA-Klemme den Flug blockt, steht die
   geclaimte Kraehe DAUERHAFT in der Luft — und haelt den Flock-Mutex 0xFBF4-Bit 0, wodurch
   ALLE anderen Kraehen nie mehr angreifen (= "die Kraehen haengen").

Die klassischen Verdaechtigen sind alle geprueft und ENTLASTET (Abschnitt 4): kein
Mutex-Leak, Pacify/Nie-wecken nur bei totem Spieler, Hoehenfenster von Sub 13 konvergiert
immer, Sub-11-Commit-Gate hat einen funktionierenden Produzenten, alle Timer-Stellen folgen
den belegten ALT==0/NEU==0-Mustern.

Beide Tests laufen OHNE `g_room_rdt_ok` — Wand/LOS/Kontakt am echten Raum sind komplett
ungetestet (Abschnitt 5); genau dort liegen die Kandidaten.

---

## 1. URSACHE A (likely): Navigator-MAPPING — Spieler-Beeline statt Routen-Steuerung

### Original (selbst belegt)

ACTIVE-Prolog ruft in JEDEM Tick den EXE-Navigator (eigene Disassemblierung @0x80100544-5F8):

```
80100584: lhu v0,554(s0)        ; +0x22A
8010058c: andi v0,v0,0x40       ; Bit 0x40 (Grab-Konvergenz) -> s1=0 (Modus 0)
80100598: lbu v0,544(s0)        ; +0x220 (rel220)
801005a0: beq v0,zero,...       ; rel220!=0 -> s1=1 (Modus 1 = ROUTE)
801005b4: lui a1,0x800d
801005b8: addiu a1,a1,-976      ; a1 = 0x800CFC30 = &Spieler-Position
801005bc: lbu a2,543(s0)        ; a2 = +0x21F = ROUTEN-KNOTEN-INDEX
801005c0: jal 0x8004a808        ; NAVIGATOR, a3=1 (routed)
801005c8: lbu v1,544(s0)        ; rel220 dec (Delay-Slot sb @0x801005d8)
801005e4-f4: ... jal 0x8004a808 a2=0 a3=0   ; sonst Modus 0 (Spieler-Folge)
```

`FUN_8004a808` (Decompilat `RE2_Quellcode_V2/FUN_8004a808.c`, Zeilen 28-58):
- `+0x10E & 0x1000` -> Steuerpunkt `+0x1C4/+0x1C6` = Zielposition direkt.
- Modus a3=1: **Steuerpunkt = MITTELPUNKT des Routen-Knotens `a2`**
  (`iVar3 = *(int*)(DAT_800ce324+0x38) + node*12; sVar6 = (n[4]+n[8])>>1 ...`).
- Zonen ungleich (`FUN_8004aab8(selfx,selfz) != zone(target)`) -> Pfadfinder
  `FUN_8004ab60(zone_from, zone_to, tx, tz)` — **hindernisbewusstes Routing ueber Zonen**.

Der EXEC von Sub 4 steuert auf diesen Punkt (eigene Disasm @0x8010125C-60):
```
8010125c: lh a1,452(s0)         ; +0x1C4  (NAVIGATOR-Punkt, NICHT Spieler)
80101260: lh a2,454(s0)         ; +0x1C6
80101264: jal 0x80015558        ; steer, rate 32
```
Gleiches Muster in Sub 5/6/7-P6/8/9. Dagegen steuern Sub 11 (@0x80101E54-60), Sub 13
(@0x8010233C-50, @0x80102374-80) und die Arc-Tests der DECs (@0x80101D3C-54,
@0x801021F8-210) im Original wirklich auf `0x800CFC30/38` = Spieler — dort ist der Port
byte-true.

### Port

`enemy_ai_re2_crow.c:25-27` (Datei-Kopf): "Navigator FUN_8004A808 (+0x1C4/+0x1C6) + Routen-
Helfer FUN_8004AA50 sind nicht portiert -> Steuerziel = Spielerposition, +0x21F bleibt 0".
Konkrete Stellen: `:616-617` (exec4 `re15_enemy_steer_point(e, pl->x, pl->z, 32)` mit
MAPPING-Kommentar), `:657` (exec5), `:684` (exec6), `:750` (exec7-P6), `:784` (exec8),
`:809` (exec9). ACTIVE-Prolog `:1760-1777` fuehrt nur die rel220/pac-Buchhaltung, ruft
keinen Navigator.

### Wirkung (Mechanik, aus Port-Konstanten abgeleitet)

- Steer-Rate 32/Tick (`re15_enemy_steer_point`, enemy_ai_common.c:3129-3140: ±slew/Tick)
  bei Speed bis 300 => minimaler Wenderadius ≈ 300/(32/4096·2π) ≈ **6100 Einheiten** —
  in RE1.5-Raeumen (Ausdehnung ~10000-20000) fliegt die Kraehe zwangslaeufig regelmaessig
  in Waende, wenn ihr Ziel permanent der Spieler ist. Das Original verbringt die
  Routen-Fenster (rel220 = rand&0x7f, staendig neu geseedet: ACTIVE-Kopf 1/32
  `:1762-1765`, Abort-Listener `:319`, Grab-Release `:1036`, Sub-11-Fail `:908`,
  Arbiter-Streak `:305`) auf Wegpunkt-Beinen quer durch den Raum.
- An der Wand: SCA-Klemme (Root-Tail `:1879-1888`, Radius 200) friert die Bewegung ein;
  der Wand-Prober `:254-265` liefert dann die byte-true Folge Streak>=11 -> Sub 8 Trudeln
  (127/128) -> Kegel/Timer -> Sub 4 -> wieder Wand — eine sichtbare
  **Wand-Flatter-Schleife** ("Kraehe haengt an der Wand"). Frontal zusaetzlich Sub 10
  Wand-Crash -> Boden -> (1,9) -> Sitzen -> Wake -> Abheben -> wieder Wand.
- Das ist kein einzelner toter Zustand, sondern ein geometrischer Attraktor, den das
  Original ueber die Routen-Knoten nicht besitzt.

**Einschraenkung/Confidence:** Das MAPPING selbst ist CONFIRMED (Code + eigene Disasm).
Dass es die gemeldeten Haenger erzeugt, ist LIKELY — der Nachweis braucht die Repro aus
Abschnitt 6 (echter Raum, Positions-Trace).

---

## 2. URSACHE B (likely, der eigentliche "Freeze"): Sub-11/13-Stall an SCA-Kante + LOS-Shim haelt Bit 2 — Mutex bleibt gehalten, Flock wird passiv

### Exit-Struktur (byte-gematcht, eigene Disasm)

Sub 11 DEC (@0x80101D30-84) und Sub 13 DEC (@0x801021EC-25C) haben GENAU zwei geometrische
Ausgaenge: Kegel (96 bzw. 192, auf den Spieler) und LOS-Bit `+0x22A&0x2`. Sub 13 EXEC
(@0x80102410-2C, eigene Disasm): `dist(+0x1F0) < 0x28A` -> nur noch Hoehenfenster-Pruefung,
KEIN `0x80015350`-Move; sonst Move. Der Wand-Prober `0x80104094` laeuft NUR im
flight_dec_tail von Sub 4/5/6 (Port `:334-339` == Original @0x80101150-98) — **in Sub 11/13
gibt es keinen Wand-Ausweg.** Das ist in Original und Port identisch.

### Der Unterschied: was das LOS-Bit sieht

- Original: `0x80050858(selfpos, PLpos, 0x8400, 0)` jeden Tick (Root @0x80100178-1E8).
  Decompilat `RE2_Quellcode_V2/FUN_80050858.c`: iteriert die Raum-KOLLISIONSLINIEN
  (`[DAT_800ce324+0x20]`, Records a 16 Bytes) und testet 2D-Segmentkreuzung selfpos->PLpos
  gegen jeden Record mit `record[0xC] & 0x8400` — d. h. der LOS-Ray laeuft ueber DIESELBE
  Kulissen-Familie, die auch die Bewegung stoppt. **Ein Hindernis, das den Flug blockt,
  bricht im Original typischerweise auch die Sicht** -> `+0x22A&2` faellt -> DEC 11/13
  exiten nach Sub 4 -> dort greift der Wand-Prober.
- Port: `re15_re2_los_clear` (enemy_ai_common.c:2657-2664) = RE1.5-Sichtstrahl
  `re15_los_ray_blocked` ueber die RE1.5-Boden-REGIONEN, alle 4 pro Tick. Datei-Kopf
  `enemy_ai_re2_crow.c:28-32`: "Mode 0x8400 selbst nicht RE'd (OFFEN)". Die SCA-Zellen, die
  `re15_collision_constrain_enemy` (Root-Tail `:1879-1888`) der Kraehe in den Weg stellt
  (Kisten/Truck in ROOM10C0), sind NICHT dieselbe Menge wie die Regionen des
  RE1.5-Sichtstrahls.

### Der Stall

Geclaimte Kraehe in Sub 13 (oder 11), Spieler hinter/neben einer SCA-Kante, die der
RE1.5-Sichtstrahl nicht blockt:
1. exec13 steuert auf den Spieler (byte-true), `re2c_move3d` wird von der SCA-Klemme
   neutralisiert -> `ai_dist` bleibt > 650 -> Hover-Stopp/Grab nie erreicht.
2. DEC13: Kraehe blickt zum Spieler (steer 96 haelt sie ausgerichtet) -> Kegel-192-Exit
   feuert nie; LOS-Bit bleibt gesetzt (Shim "frei") -> KEIN Exit. Abort-Listener feuert
   nur auf Broadcast 16 (Grab-Ende eines ANDEREN Claimers — es gibt aber nur einen Claim).
3. Post-Pass (@0x801044C4-D4, Port `:1866-1870`): Sub == 13 in [11..14] -> **Mutex bleibt
   gehalten**, byte-true.
4. Folge: die eine Kraehe steht bewegungslos flatternd in der Luft ("haengt einfach"),
   alle anderen bestehen den Arbiter nie (`!(g_re2_room_gflags & 1)` @0x80104270-7C,
   Port `:284`) und kreisen nur noch passiv — genau das Nutzer-Bild "bewegen sich nicht
   mehr sinnvoll". Der Zustand loest sich erst, wenn der Spieler die Kegel-/Sichtlage
   aendert oder den Raum verlaesst (Raum-Reload cleart das Wort, scd_room_setup.c:286 /
   actor_common.c:33 -> re15_re2dog_room_reset, enemy_ai_re2_dog.c:154).

Sub 11 analog (Kegel nur 96 -> noch enger; exec11 bewegt bei `!crow_contact` per move3d,
geklemmt -> Stall; Commit-Fenster braucht Kontakt, der ohne Annaeherung nie kommt).

**Confidence: LIKELY.** Die Bausteine sind alle CONFIRMED (Exit-Struktur byte-gematcht,
Shim-Divergenz dokumentiert-OFFEN, Mutex-Haltung byte-true); der konkrete Raum-Beleg
(welche 10C0-Kante genau) fehlt noch -> Repro Abschnitt 6.

---

## 3. URSACHE C (moeglich, klein): Wand-Radius und Boden-Hoehe als weitere Geometrie-MAPPINGs

- Original koppelt den Wand-Radius an `+0x90` (die Vol-Werte 350/100/50/10 —
  `0x8010472C` schreibt +0x90, `FUN_8003567C` liest ihn als Radius; Port-Kommentar
  `:183-186` und `:1876-1878`). Der Port klemmt fix mit 200. Eine hovernde Kraehe
  (Original: Radius 100) kommt im Original naeher an Kulissen heran als im Port (200) —
  verstaerkt Ursache B an engen Stellen.
- `dog_floor_y = (int16_t)e->y` beim INIT (`:1704`) ersetzt die EXE-Boden-Probe
  `0x8004FBA0(pos,250,1024,0)` (Decompilat vorhanden: RE2_Quellcode_V2/FUN_8004fba0.c).
  Spawnt eine Kraehe erhoeht, "landet" sie dauerhaft auf Spawn-Hoehe−250 in der Luft —
  optisch ein Haenger. Fuer ROOM10C0 nur relevant, wenn die drei Sce_em_set-Spawns
  (@RDT 0xF8C/0xFA0/0xFB4) nicht auf Bodenhoehe liegen (nicht geprueft — OPEN).

---

## 4. ENTLASTET — systematisch geprueft, KEIN Befund

### 4.1 Mutex-Leak (Auftrag: "Kraehe stirbt/GIBt im Claim")
- Post-Pass eigene Disasm @0x801044B0-2C4: `lbu v0,5; addiu −11; sltiu 4` — kein
  State-Gate; Release sobald `+0x5 ∉ [11..14]`. Port `:1866-1870` identisch.
- Treffer im Claim: BEIDE Port-Writer ueberschreiben `+0x5` (Gun-Pfad `+0x5 = RE1.5-
  Waffen-Id` @0x800124bc-Analog; Melee-Pfad `re15_enemy_take_damage` re15_damage.c:1705
  `sub_state_1 = react_table[...]`). RE1.5-Waffen-Ids 11..14 halten den Mutex nur fuer die
  HURT-Dauer (byte-true: RE2-Projektil-Ids 11..14 taten dasselbe, Kommentar `:1855-1865`);
  jeder HURT-Ausgang schreibt `re2c_state(1,·)` oder `(7,·)` -> Release im selben Tick.
- GIB -> (7,1), Boden -> (7,·): Post-Pass laeuft in JEDEM Root-Tick (nach dem switch),
  auch fuer state 2/3/7 -> Release. Kein Pfad deaktiviert den Aktor mitten im Raum.
- Raum-(Re-)Load cleart das Wort: actor_common.c:33 + scd_room_setup.c:286 ->
  re15_re2z_rng_reset -> re15_re2dog_room_reset (enemy_ai_re2_dog.c:154), Beleg-Kette
  FUN_80052f3c `sh zero` @0x80052fe4 im Kommentar scd_room_setup.c:278-285.
- **Ergebnis: kein permanentes Leak im Port.**

### 4.2 Pacify/Nie-wecken bei lebendem Spieler
Eigene Disasm dec0-Kopf @0x801006C0-F4: `lh hp(0x800CFD4E); bgez -> 0x801006fc`
(lebend -> normale Wake-Kette); `lb pac(545); bgtz -> 0x801006fc`; nur tot+pac<=0+
`dist<0x708` -> return. Port `:347-349` identisch (`pl->hp < 0 && pac221 <= 0 &&
ai_dist < 0x708`). Einziger Pacify-Produzent ist Broadcast 128 und der feuert nur bei
`re15_re2_player_damage_mode`-ret 2 = Spieler-Tod (`:1095-1097`). ACTIVE-Prolog dekrementiert
pac nur bei totem Spieler (eigene Disasm @0x80100550-580; Port `:1768-1773`).
**Bei lebendem Spieler unerreichbar.**

### 4.3 Sub 13 Hoehenfenster (Auftrag: "was, wenn nie erreicht?")
Eigene Disasm @0x801023BC-F0 (±60-Klemme) + @0x80102424-48 (±99-Fenster): Die Klemme zieht
`y` pro Tick um 60 auf `+0x224` zu, bis `d ∈ [−60,60]` ⊂ Fenster [−99,99]. Das Fenster wird
also IMMER erreicht, sobald `dist < 650` faellt. Port `:1016-1028` byte-gleich. Der Stall
entsteht davor (dist faellt nie unter 650 — Ursache B), nicht im Hoehenfenster.

### 4.4 Sub 11 Commit-Gate (Auftrag: "hat der Port einen Produzenten?")
JA: Root-Tail (4) `:1889-1892` — `crow_contact = re15_body_push(pl, 450, e, 96)`
(Vorframe-Stempel, wie beim Hund). Original-Produzent selbst gelesen:
`FUN_800355c4` setzt `+0xD=0xFF`/Frame und `FUN_80034d0c` stempelt beim Part-Kontakt
`*(param_2+0xd) = (char)param_1[3]` (Pusher-Index, Spieler = 0) inkl. Y-Band
`(-hsum < dy < dy < hsum)` — strukturgleich zum Port-Band (enemy_ai_common.c:4047-4051).
Rechnung am Port: Spieler `hit_offset_y = −1530`, `hit_height = 1530` (re15_damage.c:
2153-2164, On-Disc-Beleg 0x80073e94/Datei 0x64694), Kraehe h=512 (`:1696`) ->
dy(Commit-Hoehe 2700) = 1530−2700 = −1170, Band ±2042 -> **Kontakt in Angriffshoehe
moeglich** (der 2026-08-02-Fix; Sonde probe_crow_1170). exec11-Commit-Tail byte-gematcht
(eigene Disasm @0x80101EE8-F84 gegen Port `:901-913`).

### 4.5 Timer-Muster (Auftrag: ALT==0 vs NEU==0)
- dec0-Wake: eigene Disasm @0x80100768-77C — `bne v1(ALT),zero` mit `sh v0(ALT−1)` im
  Delay-Slot = Dec IMMER, Wake bei ALT==0. Port `:355-359` exakt so.
- exec2-Tail `:520-521` und exec7-P5 `:737-738` sind NEU==0 — laut Welle-D-Review-Sweep
  byte-true (`addiu −1; sb; andi; bne`), alle uebrigen Stellen ALT==0 mit Dec-immer
  (Review-Fixes #2/#6/#10/#11, RE15_RE2_AI.md "Muster-Sweep"). Beim Nachlesen aller 20+
  Timer-Stellen im Port-File: kein Verstoss gegen das jeweils zitierte Muster gefunden;
  jede Stelle traegt ihr @0x-Zitat. **Kein Befund.**
- Wichtig fuers Haenger-Bild: JEDER Timer-Zustand (Sub 1/8/9, Idle-Phasen, Liegen)
  hat einen RNG- oder Wrap-Exit ≤ 256 Ticks; kein Zustand kann rein ueber Timer
  dauerhaft stehen bleiben.

### 4.6 Weitere gepruefte Nicht-Ursachen
- **State 4 Skript-Perch**: Port-Stub leitet nach (1,0) um (`:1831-1835`); unerreichbar,
  weil `re2z_f10e` keinen RE1.5-Produzenten hat (INIT-Kommentar `:1716-1721`). Kein Stall.
- **Sub 8 Trudeln**: Kegel-96-Exit ODER ALT==0-Timer (rand&0x3f ≤ 63 Ticks) -> Sub 4
  garantiert (`:789-799`, Review-Fix #6). Kein Stall.
- **Sub 7 Landen**: Flare-Gate `y − (floor−250) >= −299` wird durch vy=120-Fall monoton
  erreicht; P5-Tail hat den adv-Exit; P6 endet bei lebendem Spieler IMMER in Sub 2
  (Lane-K-Korrektur `:754-760`). Kein Stall.
- **Doppel-Advance**: Typ 0x21 steht in `re15_type_self_advances_anim`
  (player_common.c:957) -> globaler Advancer ueberspringt die Kraehe; die
  P13-Frame-Gleichheit (`:443-447`) kann nicht uebersprungen werden.
- **Arbiter byte-gematcht** (eigene Disasm @0x801041F0-3F4): Cooldown exitet auch im
  Original den GANZEN Arbiter (`j 0x801043ec` @0x8010424C); Fenster `+0x1F0−901 <
  0x189B`; Join-Maske 0xA0000; Direkt-Strike-Kaskade inkl. `sltiu 0xb`-Zweig — Port
  `:269-311` deckungsgleich.
- **RNG**: geteilter LCG (`re15_re2_rand`), Draw-Zahlen im Review gepinnt; ein
  "geometrischer" Alarm-Exit (p=1/8 pro Tick) kann nicht praktisch unendlich werden.

---

## 5. Test-Luecken (beide Tests decken die Kandidaten NICHT ab)

`test_re2_crow_ai.c`:
- ruft `re15_re2crow_tick` direkt, ohne Raum: `g_room_rdt_ok == 0` -> SCA-Klemme aus
  (`crow_wall` immer 0, `:1879-1888`), LOS-Shim liefert immer "frei"
  (enemy_ai_common.c:2659). Wand-Prober, Sub 10, LOS-Exits: NIE ausgefuehrt.
- `crow_contact` wird fuer den Direkt-Strike von Hand gesetzt (`:251`); der organische
  Sub-11-Commit (Body-Push am fliegenden Aktor) wird nirgends getestet — kein einziger
  Test setzt `sub_state_1 = 11`.
- `test_no_freeze` (600 F) teleportiert den Spieler (700↔2500) und prueft nur
  State-Signaturen-Wechsel (>=3) — ein Mid-Air-Stall ist ohne Waende nicht herstellbar,
  und POSITIONS-Fortschritt wird nicht gemessen.

`test_re2_room10c0_ab.c`:
- parst das RDT nur LOKAL (`re15_rdt_parse` in eine Stack-Variable) und laedt NIE
  `g_room_rdt` -> auch hier `g_room_rdt_ok == 0`: der "echte Raum" liefert nur Spawns
  und SCD, aber keine Kollision/LOS. Grep-Beleg: kein Treffer fuer `g_room_rdt` in beiden
  Testdateien.
- No-Freeze-Block (6) verlangt nur `live_moving >= 1` — zwei von drei haengenden Kraehen
  wuerden bestehen; der Mutex-Zustand am Ende wird nicht gegen "gehalten & Position
  eingefroren" geprueft.

**Fehlende Stuck-Szenarien:** (a) geclaimte Kraehe gegen SCA-Kante in Sub 11/13 mit
LOS-frei; (b) Wand-Flatter-Schleife Sub 4↔8/10 an einer echten Wand; (c) Claim-Starvation
bei dauerhaft dist<901; (d) toter Spieler (Lander/Nie-wecken); (e) organischer
Body-Push-Commit; (f) Positions-Fortschritts-Metrik pro Kraehe.

---

## 6. Repro/Messung fuer den Hauptagenten

1. **Live-Trace existiert schon**: `RE15_RE2_TRACE=1` (Port `:1904-1917`) loggt je
   Signatur-Wechsel `state/sub/clip/hp/dist/spd/pos/fl` nach stderr. Lauf:
   `RE15_AI_FLAVOR=RE2` + `RE15_DEBUG_JUMP=10c0` + Boot-Input-Skript; im Raum an
   Truck/Kisten-Kanten stehen bleiben. Haenger-Signatur: eine Kraehe bleibt mit
   `fl & 0x0004` (Claim) in `sub=13` (oder 11) OHNE weitere Trace-Zeilen (keine
   Signatur-Wechsel), waehrend `pos` sich nicht mehr aendert; alle anderen Kraehen
   zeigen nie sub 11..14. Da der Trace nur bei Signatur-WECHSEL druckt, zusaetzlich
   periodisch `g_re2_room_gflags`+Positionen dumpen (Mini-Sonde).
2. **Deterministische Sonde** (room-probe-Muster): ROOM10C0 wirklich in `g_room_rdt`
   laden (`g_room_rdt_ok=1`), Spieler an eine Position setzen, deren Gerade zur Kraehe
   eine SCA-Zelle schneidet, die `re15_los_ray_blocked` NICHT blockt (Kandidaten per
   Scan: alle SCA-Zellen von 10C0 gegen die Sicht-Regionen differenzieren); 3000 Ticks;
   Assertions: (i) keine Kraehe haelt den Claim > N Ticks bei eingefrorener Position,
   (ii) jede lebende Kraehe bewegt sich in jedem 300er-Fenster > M Einheiten.
3. **A/B gegen das Original** ist fuer die Kraehe schwer (RE2-Retail-Raum 1090 vs.
   RE1.5-Raum) — der Beweis der Ursachen laeuft daher ueber die Mechanik-Belege
   (Abschnitt 1/2) + Port-Repro, nicht ueber Frame-Vergleich.

## 7. Fix-Skizzen (NICHT umgesetzt — nur Analyse)

- **B zuerst (kleinster byte-naher Eingriff):** `0x80050858` ist decompiliert vorhanden
  (RE2_Quellcode_V2/FUN_80050858.c) und klein: 2D-Segmentkreuzung gegen Kollisionslinien
  mit `flags & 0x8400`. Portieren gegen die RE1.5-SCA-Linien (dieselbe Kulisse, die
  `re15_collision_constrain_enemy` nutzt) statt des Region-Sichtstrahls — dann bricht
  LOS exakt dort, wo der Flug blockt, und Sub 11/13 exiten wie im Original nach Sub 4
  (dort faengt der Wand-Prober). Die Attribut-Maske 0x8400 muss dabei auf die
  RE1.5-SCA-Typen abgebildet werden (OFFEN: welche RE1.5-Zell-Typen der 0x8400-Menge
  entsprechen — naechster RE-Schritt: RE2-RDT-SCA-Record-Layout `[+0x20]`-Tabelle mit
  Feld +0xC gegen RE15_KNOWLEDGE.md §1.1 legen).
- **A danach (groesser):** `FUN_8004a808`/`FUN_8004aab8`/`FUN_8004ab60`/`0x8004AA50`
  liegen als Decompilate/EXE vor; die Zonen-Quelle im RE1.5-Raum waere zu waehlen
  (RDT-Kollisionsrechtecke als Zonen). Bis dahin mildert ein deklarierter
  Zwischenschritt (im Routen-Fenster rel220!=0 ein raumzentriertes Wegpunkt-Ziel statt
  des Spielers) die Wand-Attraktoren — das waere aber ein NEUES MAPPING und gehoert
  als solches markiert, nicht als byte-true verkauft.
- **Tests:** AB-Test auf `g_room_rdt` umstellen + Stuck-Assertions aus Abschnitt 6.

## 8. OPEN (nicht erfunden, naechste RE-Wege)

- `0x80050858`-Rueckgabewert-Semantik im Detail (Rest des Decompilats lesen: return-Wert
  bei Schnitt; DAT_800c3b80-Seitenkanal 2/3) — noetig fuer die byte-true Portierung.
- Zuordnung RE2-Kollisionsrecord-Feld `+0xC`-Bits (0x8400) ↔ RE1.5-SCA-Zelltypen.
- `FUN_8004aab8`-Zonenquelle (`DAT_800ce324`-Struktur +0x38: 12-Byte-Knoten) ↔
  RE1.5-RDT-Aequivalent.
- ROOM10C0-Spawn-Hoehen der drei Kraehen (RDT 0xF8C/0xFA0/0xFB4) fuer Ursache C.
