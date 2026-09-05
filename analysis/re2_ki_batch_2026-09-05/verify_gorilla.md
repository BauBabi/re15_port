# SKEPTIKER-Verifikation PAKET S5 — Gorilla (0x27) haengt zwischen den Autos

Datum 2026-09-05. ALLE Belege unten sind EIGENE Herleitung dieser Session:
- Disasm: `python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr> <n> [--bin STAGE1.BIN]`
  (RE1.5 PSX.EXE bzw. STAGE1.BIN @0x80100000 ohne Header-Abzug). Voll-Dump des Maggot-Baums
  0x80116db8+5200 Instr. liegt in `scratchpad/maggot_full.txt`.
- SCA: eigener Python-Parser direkt auf `re15_port/shared_assets/PSX/STAGE1/ROOM11C0.RDT`
  (Offset aus Datei-Header 0x20, Struct-Layout gegen b93c-Disasm quergeprueft).
- Port: eigenes Code-Lesen (enemy_ai_common.c, re15_collision.c, scd_vm.c, aot_common.c).

---

## (a) FUN_8003b93c — SCA-Attribut-Zonen-Abfrage: **CONFIRMED**

Eigenes Disasm 0x8003b93c (120 Instr., PSX.EXE):
- Quadranten-Wahl `jal 0x8003b068` @0x8003b9a8, Partition = `[[0x800ac778]+0x20] + ret*4+4`,
  `lw s1,0(v0)` / `lw v1,4(v0)` @0x8003b9cc-d0 — **dieselbe Partitionierung wie der Resolver**
  (b0a4 @0x8003b188/0x8003b208 identisch).
- Zellen-Stride 12, s0 = Zelle+10. Match:
  - @0x8003b9f8 `lhu v1,0(s0)` (= u1 | floor<<8), @0x8003ba00-04 `sll 16 / sra 28` = floor>>4,
  - **@0x8003ba08 `bne a2,v0`** (a2 = Arg-Band = das uebergebene +0x82-Byte, `andi 0xff` @0x8003b9e4),
  - **@0x8003ba0c/`bne` @0x8003ba14**: `andi v1,0xff` = u1 muss EXAKT == a3 (Attr). ✓
  - **b93c prueft u0 NICHT** — Pads mit u0=0 sind matchbar (nur der Resolver filtert u0&mask). ✓
- Broadphase: @0x8003ba1c-34 `(pos.x − (zelle[+4] − a1)) <u (zelle[+0] + a1*2)`, Z analog
  @0x8003ba40-58 (zelle[+6]/zelle[+2]). Basis @+4/+6, Extents @+0/+2 — deckungsgleich mit dem
  Port-Struct re15_rdt.h:62-71 {width,density,x,z,type,u0,u1,floor}. **Das „+100" passiert im
  AUFRUFER** (@0x80117f00 / @0x80117f9c `addiu a1,a1,100`), b93c nutzt a1 roh — so stellt es
  auch das Dossier dar. ✓
- Treffer: @0x8003ba68 `jal 0x8001bf04(pos, sp+16, zelle)`, @0x8003ba70-88
  `(ret>>4)+8+(u1&3)`, **@0x8003ba8c `sb v0,144(a0)`** = entity+0x90 des GLOBAL-current-Entity
  ([0x800ac784]), @0x8003ba98 `ori v0,1` + **@0x8003ba9c `j exit`** = return 1 beim ERSTEN
  Treffer, Delay @0x8003baa0 `sw s1,436(v1)` = +0x1b4=&Zelle. Alle zitierten Adressen stimmen. ✓
- FUN_8001bf04 selbst disassembliert: X-Signpaar==3→0x000, ==0→0x800, innerhalb→Z-Paar
  ==0→0x400, ==3→0xc00, sonst 0xf00 (@0x8001bf6c-bfec) — identisch zum Port
  `coll_contact_dir` re15_collision.c:678-694. ✓

## (b) A[4]-Block + Produzent von +0x1e2: **CONFIRMED** (Produzent existiert beidseitig)

Eigenes Disasm 0x80117e40 (130 Instr., STAGE1.BIN):
- Reihenfolge exakt wie behauptet: Abort (`player.+0x93!=0 && +0x1d0&1` → CHASE @0x80117e50-74);
  HEAVY (`+0x93==0` @0x80117e88, `a804(0xfa0,0xc0)` @0x80117e90-9c, `+0x1dc==0` @0x80117eb0 →
  +0x5=6 @0x80117ec4); DANN Path A.
- **Path A**: @0x80117ed4 `lbu v0,482(v1)` (+0x1e2), ==0 → 0x80118028 (Path B). Query 1:
  a3=0x10 @0x80117ee0, **a2 = `lbu a2,130(v1)` = +0x82** @0x80117ee4, a1 = `lhu 6([+0x78])`+100
  @0x80117ef4/f00, `jal 0x8003b93c` @0x80117efc. Treffer → @0x80117f18 `lbu a1,144(a0)`
  (+0x90, frisch von b93c), Yaw-Fenster `((a1&0xf0)<<4 − rot_y + 512)&0xfff < 1024`
  @0x80117f20-38, dann **+0x9f=+0x90 @0x80117f40, +0x5=7 @0x80117f50, +0x6=0 @0x80117f60,
  +0x7=1 @0x80117f74**. ✓
- **Attr-0x20-Zweig** @0x80117f78-0x80118024: zweiter b93c-Call (a2 wieder +0x82 @0x80117f94,
  a3=0x20 aus dem Delay-Slot @0x80117f08); Treffer + **`+0x1d0&1` GESETZT → `bne` @0x80117fc0
  nach 0x80118048** = exakt der +0x1d6-Check von Path B (LOS-Gate @0x80118034-40 wird
  UEBERSPRUNGEN); LOS frei → gleiches Yaw-Fenster @0x80117fd0-e4 → +0x9f/+0x5=7/+0x6=0/
  **+0x7=3** @0x80117ff0-8011 8024. ✓
- Path B @0x80118028-8100: LOS → **+0x1d6==0 @0x80118054-5c** → +0x1d4>=0x1771 @0x8011806c →
  a9cc(0x20)==0 @0x80118084-8c → aca58==0x701||rng&1 @0x80118090-b4 → a780==0 @0x801180bc-c4 →
  +0x5=7/+0x6=0/+0x7=0. ✓ (Port :8411-8419 byte-gleich.)
- **PRODUZENT +0x1e2**: grep ueber den 5200-Instr.-Dump: EINZIGER Write im Baum =
  **INIT @0x80117098-9c `ori v0,zero,0x4` / `sb v0,482(v1)`** — unconditional, Wert 4.
  Alle anderen Fundstellen (0x801171a0, 0x80117ed4, 0x80118a64, 0x80118c00, 0x80118f90,
  0x801194ac, 0x80119900, 0x8011afb0, 0x8011b750) sind `lbu`-LESER. +0x1e2 ist zugleich der
  **Kollisions-Box-Index**: INIT @0x801171a0-c0 laedt `+0x78 = [0x80121368 + 4*+0x1e2]`;
  Tabelle selbst gelesen: [4] = 0x80121350, Box = {0,−1440,0,**1600**,1440,1600} (Bytes
  dekodiert). Spawn nullt die Struct, INIT laeuft im ersten unfrozen Tick (Kampf-Grid 0x10,
  Freeze-Bit 0x20 nicht gesetzt) ⇒ +0x1e2 = 4 fuer die gesamte Lebenszeit. ✓
  **Port-Produzent existiert ebenfalls**: enemy_ai_common.c:8279 `e->mag_boost = 4` im INIT —
  der portierte Path-A-Block wuerde also NICHT an einem fehlenden Produzenten scheitern.
- **Eine Korrektur am Diagnose-Dossier §1.1**: „Kein +0x1d6-Lese im CHASE-Decide" ist als
  Absolutaussage falsch — A[3] LIEST +0x1d6 @0x80117b18-20 (REAR-UP-Gate; das eigene §4 des
  Dossiers listet es korrekt). Richtig bleibt: kein AUSWEICH-Konsument in A[3].

## (c) +0x82-Writes + Root-Klemm-Aufruf: **CONFIRMED** (+ 1 Ergaenzung)

- Launch B[7] @0x80118aa4-b10 (eigenes Disasm): `ori a0,zero,0x1` @0x80118aac →
  `sb a0,480` (+0x1e0=1 airborne) @0x80118ab0 → **`beq +0x7,zero,0x80118afc` @0x80118ac8**
  (Gate!) → Yaw-Snap `rot_y=(+0x9f&0xf0)<<4` @0x80118ad0-ae0 → **`sb a0,130(v0)` = +0x82=1
  @0x80118af0** — also NUR bei +0x7!=0, Wert 1. ✓  +0x93|=1 @0x80118b00-10 laeuft fuer beide. ✓
- Landung B[7] @0x80118c5c-cc4: c1a4!=0 → +0x6=3, +0x8c=0x64, Se, +0x1e0=0 @0x80118c94,
  **+0x82=0 @0x80118ca4**, +0x7=0 @0x80118cb4, +0x93=0 @0x80118cc4. ✓
- **ERGAENZUNG** (im Diagnose-Dossier nicht genannt, aendert nichts): dritter Write
  **@0x80119034 `sb zero,130`** = die B[8]-Finisher-Landung (gleiches Muster
  @0x80118fec-9054); der Port spiegelt sie bereits (:8598 `e->floor = 0`). Vollstaendige
  Write-Liste im Baum: 0x80118af0 (=1), 0x80118ca4 (=0), 0x80119034 (=0); Reads: die zwei
  b93c-Callsites @0x80117ee4/@0x80117f94. Sonst NICHTS (grep `,130(` ueber den Dump).
- **Root-Klemm-Aufruf @0x80116e58-84, Register-Herleitung**: a0 = [0x800ac784]+0x34 (Delay-Slot
  @0x80116e74), a1 = `lhu 6([+0x78])` = 1600, a2 = **`ori a2,zero,0x4`** = MASKE (@0x80116e68),
  a3 = UNGESETZT. **Der Root uebergibt KEIN Band.** FUN_8003b0a4 (eigenes Disasm, 200 Instr.)
  liest das Band SELBST: @0x8003b228-2c `lw a3,[0x800ac784]`, **@0x8003b234 `lbu v1,130(a3)`
  = entity+0x82**, @0x8003b238 `srl v0,28` = floor>>4 der Zelle, **@0x8003b23c `bne v1,v0`** =
  striktes Band-Gate. Maske: @0x8003b128 `sb a2,24(sp)`, @0x8003b244-58 `lhu −2(s2)`
  (Halbwort @Zelle+8) `sra 24` = u0, `and` mit Maske → 0 = nicht solide → (bei u1&0x80==0)
  skip. **Pads (u0=0, u1=0x10/0x20) klemmen also NIE** — reine Marker. ✓
  ⇒ Die Original-Band-Quelle des Klemmpfads ist byte-genau **+0x82**, kein Y-Derivat und kein
  Argument. Die Port-Divergenz (re15_collision.c:797/:808 `re15_collision_band_from_y(enemy_y)`
  = `-(y/0x708)`, :186) ist real; der Port-Kommentar :787-788 raeumt sie selbst ein.

## (d) ROOM11C0-SCA selbst geparst: **CONFIRMED**

Eigener Parser: SCA-Offset aus Datei 0x20 = **0x1410**, Header {ceiling 61605/4531,
Partitionen **11/13/19/16/0** = 59 Zellen}, Eintraege 12 B {w,d,x,z,typ,u0,u1,fl}:
- **Pads**: idx **36** t1 u0=00 **u1=0x10** fl=02 x[4900..9500] z[−6080..−2180];
  idx **47** t1 u0=00 **u1=0x10** fl=02 x[−17397..−12096] z[−6590..−1690];
  idx **4/31** t1 u0=00 **u1=0x20** fl=02 x[7500..10700] z[−200..8201];
  idx **19/55** t1 u0=00 **u1=0x20** fl=02 x[−11900..−9300] z[200..9900]. Alle Band 0
  (fl>>4 = 0), alle u0=0. Byte-identisch zum Diagnose-Dossier. ✓
- **Ost-Spalt**: Zelle 7 (Zentralblock Ost, u0=ff) x_max = **4800**; Auto-Reihe 28/29 (u0=ff,
  t4/t6) x_min = **7511** ⇒ Spalt **2711**. Push-Algebra mit r=1600 (Box @0x80121350 selbst
  dekodiert): Zentrum ≥ 4800+1600 = 6400 UND ≤ 7511−1600 = 5911 → Widerspruch →
  **am Boden unpassierbar**. Pad 36 deckt exakt diesen Spalt. ✓
- **Kampf-Spawn #2**: Sce_em_set-Bytes selbst dekodiert @Datei **0x17fc**
  `44 02 27 10 00 ...` → x=**9434**, z=**2189** (0x24da/0x088d) → liegt IN Pad 4/31. ✓
  (Zusatz: x=9434 liegt auch im Band-1-Streifen 9/39 x[9300..19600] — betrifft nur den
  fliegenden Zonen-Leaper.)
- Band-1-Zellen (fl=0x13): 16/51, 8/22, 38/56, 9/39, 40/57 ✓; Gegner-Spezialzellen 30/48
  u0=0x04 ✓.

## (e) Original-Beobachtung des Zonen-Leaps: **UNPROVEN** (keine Beobachtung, aber auch kein Gegenbeleg)

- `RE15_MAGGOT_AI.md` (Wave 2c, :113-131): LIVE-verifiziert wurde NUR der **Fern-Leap +0x7=0**,
  und zwar per **erzwungenem** +0x5=7 (Savestate-Patch), nicht als Spontanverhalten. Path A
  steht dort als „[Path A Zone-Leaps (+0x1e2!=0 → 0x8003b93c …, +0x7=1/3) brauchen
  BLK-Kollisions-Zonen.]" — erwaehnt, nie gesehen.
- `RE15_ENEMY_AI_AUDIT.md`:36-37: „maggot: Path-A ZONE leaps (#11) … B[7] launch variants
  implemented but **dormant**" — als OPEN gefuehrt, keine Beobachtung.
- `analysis/gorilla_11c0/verhalten.md` D6 nennt „Zone-Leaps" die **Lanes 9-14 (+0x1e3)** —
  das ist ein ANDERER Mechanismus (Sonderleaps, kein STAGE1-Writer von +0x1e3) als der hier
  fixrelevante A[4]-Path-A→sub-7-Leap (+0x7=1/3). Nicht verwechseln.
- **Nirgendwo steht „nie beobachtet" als Verdikt**; es gibt schlicht keinen dynamischen Lauf.
  Statisch ist die Erreichbarkeit sauber: +0x1e2 immer 4 (b), Pads Band-0-matchbar (d),
  SELECTOR (sub 4) live belegt (Heavy-Eskalations-Savestate 3→4→6), und Leap-/Hurt-Zyklen
  muenden erneut in SELECTOR (Landungs-Tail @0x80118d74-dc0 → sub 4; Hurt-Exit → sub 7 →
  Landung → sub 4). ⇒ Der Fix ist NICHT als wirkungslos belegbar — aber der im Diagnose-Dossier
  §5 vorgeschlagene PCSX-Watchpoint auf den +0x82-Write @0x80118af0 ist der fehlende
  Original-Beweis und sollte VOR der Erfolgsmeldung laufen.

## (f) FIX-RISIKO: teils bestaetigt, **zwei Korrekturen am Fix-Plan**

1. **„INIT seedet floor aus Spawn-Y" ist der FALSCHE Mechanismus.** Das Original seedet +0x82
   aus dem **Spawn-Byte pc[4] von Sce_em_set** (Port bereits byte-true: scd_vm.c:3369
   `a->floor = t->pc[4]`, zitiert @0x800421c8/@0x800421d0). ROOM11C0-Kampf-Gorillas:
   pc[4]=**0x00** (Bytes @0x17e8/0x17fc) → floor=0 korrekt VORHANDEN. Die Cutscene-Gorillas
   haben pc[4]=**0x02** bei y=−20000 — ein Y-Seed ergaebe 11 statt 2 und waere eine erfundene
   Konstante. ⇒ Fix-Schritt: KEIN neues Seeding schreiben, das existierende nutzen.
2. **Wer schreibt e->floor fuer 0x27 heute**: scd_vm.c:3369 (Spawn), enemy_ai_common.c:8529
   (=1, Zonen-Launch), :8553/:8598 (=0, beide Landungen). „Kein Konsument" stimmt NUR fuer den
   Klemmpfad: **aot_common.c:755** gatet Band-AOT-Stempel fuer ALLE Aktoren (Enemy-Pool-Bit
   0x02) auf `e->floor != a->band` — e->floor hat also heute schon einen Leser; floor=1 im
   Zonenflug entspricht dem Original (+0x82=1), keine Regression zu erwarten.
3. **Shared-Funktion**: `re15_enemy_sca_clamp` (enemy_ai_common.c:12938) wird von SIEBEN
   Zweigen gerufen (Zombie :13268, Hund :13311, Adult-Spider :13334, **Cockroach :13344
   (FLIEGT!)**, Birkin :13358, Maggot :13386, Zombie-Girl :13437). Eine globale Umstellung auf
   e->floor aendert sofort das Flug-Klemmverhalten des Cockroach und jedes Typs, dessen Brain
   floor nicht pflegt (Hund pflegt: :6863/:6913; ZGirl pflegt: :10020/:10282). Fuer „NUR 0x27"
   braucht es einen Parameter/Wrapper — die Original-Architektur (b0a4 liest +0x82 fuer ALLE)
   spricht langfristig fuer global, aber nur mit Typ-fuer-Typ-Floor-Audit.
4. **Klemmt der Port den Leap-FLUG?** Ja, Code-Pfad: run_all :13383-13386 → maggot_tick
   (Ballistik `e->y -= s1`, Apex −4680: vimp 720, grav −60, Summe 12·(720+60)/2) →
   `re15_enemy_sca_clamp` UNBEDINGT jeden Frame (kein Airborne-Gate — byte-gleich zum Root:
   b0a4 @0x80116e64 laeuft VOR dem +0x9&0x40-Gate @0x80116e9c, das nur den Schatten-OTZ-Block
   bis 0x80116f20 ueberspringt). ABER die Band-Quelle band_from_y macht die Klemme im Flug
   wirkungslos: y −720/−1380 (Band 0, ~2 Frames) → Band 1 (~2) → **Band 2 = KEINE Zellen
   (~19 Frames)** → zurueck. Freiflug ≈ 19 × (232..271) ≈ 4400-5100 Einheiten > Auto-Reihe 28
   (Breite 4320) ⇒ **der heutige +0x7=0-Leap (u.a. der Retaliation-Leap nach JEDEM Flinch,
   :8755-8760) kann Autos ueberfliegen** — der zweite „haengt"-Pfad des Dossiers ist bestaetigt.
   Das Original klemmt den +0x7=0-Flug durchgehend an Band 0 (+0x82 bleibt 0).
5. **Folge-Risiken des Fixes**:
   - Nach der Band-Umstellung prallen Retaliation-Leaps sichtbar an Auto-Reihen ab
     (byte-true, aber eine deutliche Verhaltensaenderung; 0x27 existiert nur in ROOM11C0 —
     Blast-Radius klein).
   - **Schritte (1)+(2) ohne Schritt (3) sind inkonsistent**: der Zonen-Leap setzt floor=1,
     die Klemme ignoriert es; in den Landeframes (Band 0) steht der Gorilla ggf. IN einer
     Auto-Zelle und wird zu einer zufaelligen Seite ausgestossen — nichtdeterministische
     Landung. Zusammen landen.
   - Path A feuert NUR in sub 4 (SELECTOR). Eintritt: `+0x93==0 && dist>=0x1771 && LOS`
     (@0x80117c2c-54, selbst disassembliert), Grab-Cmd 0x701 (A[0]), oder Leap-/Hurt-Recovery.
     Ein Gorilla, der NAH (dist<6001) hinter dem Auto klemmt, zonen-leapt erst nach
     Beschuss-/Distanz-Zyklen — auch im Original. Der Fix beseitigt das Symptom also ueber
     die Zyklen (Flinch→Leap→Landung→SELECTOR→Path A), nicht im Nahstand-Standbild; fuer den
     Nahkampf-Eindruck bleiben D2 (aec4-Push) und das REAR-UP-Entsperren die Treiber.
   - Port-Zutaten vorhanden: `e->ai_contact`/`e->coll_cell_attr` werden geschrieben
     (re15_collision.c:766-769 via :12942-12944), `dog_aux9f` existiert (re15_actor.h:808),
     `re15_dog_sub` setzt +0x5/+0x6=0 (:6570) — die Dossier-Korrektur, dass der alte
     OPEN-Kommentar (:8408 „+0x90-Writer fehlt") veraltet ist, stimmt.

## Kurz-Verdikte

| Behauptung | Verdikt |
|---|---|
| (a) b93c-Semantik + Adressen | CONFIRMED |
| (b) A[4]-Gates, +0x1e2-Produzent INIT=4 (einziger Writer), 0x20-Zweig/LOS-Latch | CONFIRMED (Nebensatz „kein +0x1d6-Lese in A[3]" falsch — @0x80117b18) |
| (c) +0x82-Writes @0x80118af0/@0x80118ca4; b0a4-Band = +0x82 intern @0x8003b234, kein Argument | CONFIRMED (+ dritter Write @0x80119034, Port hat ihn) |
| (d) Pads idx36/47=0x10, idx4/31/19/55=0x20, u0=0; Ost-Spalt 2711; Spawn#2 im Pad | CONFIRMED |
| (e) Original-Zonen-Leap je beobachtet | UNPROVEN (kein Lauf, kein Gegenbeleg; statisch erreichbar; Watchpoint @0x80118af0 = Beweisweg) |
| (f) Fix wie skizziert gefahrlos | TEILS REFUTED: Spawn-Y-Seed erfunden (pc[4]-Seed existiert), sca_clamp shared (7 Typen), Schritt-Kopplung (1+2 ohne 3 = instabile Landung), Nahstand bleibt |
