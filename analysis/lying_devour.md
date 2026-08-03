# Der „Liegend-Devour"-Einstieg — FUN_80103b94/FUN_80104548: wer committet die Fress-States, und auf wen

**Datum:** 2026-08-03 · **Status:** Original-Seite **byte-belegt** (Roh-Disasm + Tabellen-Dumps STAGE1.BIN), Port-IST code-verifiziert
**Quellen:** `info/Re1.5/PSX/BIN/STAGE1.BIN` (@0x80100000 roh, off = addr−0x80100000), `info/Re1.5/PSX.EXE` (t_addr 0x80010000, off = 0x800+addr−0x80010000), `RE_15_Quellcode_Overlays/STAGE1_full/`, `RE_15_Quellcode_V2/`, `re15_port/engine/src/enemy_ai_common.c` (Audit wf_827f186d Zombie-Girl), Schwester-Dossiers `analysis/player_hit_chain.md`, `analysis/bite_blood_fx.md`, `analysis/blood_decals.md`, `analysis/player_knockdown.md`.
**Werkzeug:** `re15_disasm.py` (dis/table), vollständiger Instruktions-Zensus STAGE1.BIN (alle `sb →+0x5` / `sw/sh →+0x4`-Sites 0x80100000–0x8011f000, 218 Sites, plus `li`-Zensus aller State-Wörter).

---

## 0. Executive Answer

**Die Prämisse „Zombies fressen den am Boden liegenden lebenden Spieler" hat in MZD KEINEN erreichbaren Code-Pfad.** Die Aufklärung der Kette ergibt drei Befunde:

1. **FUN_80103b94 (Latch/Biss) + FUN_80104548 (Kill-Biss) sind die Fress-am-Lebenden-FSM der ZOMBIE-GIRL-Familie (Typ 0x13) im Grid-Submode 1** — eine **kauernde Ambush-Girl** (Spawn-Behavior-Nibble 1/3, Hock-Clip 0xc), die den **STEHENDEN** Spieler anfällt: Commit `+0x4 = (facing+1)<<8|1` → +0x5 = **1/2** → Spieler-cmd `5` mit **aca59 = 2/3** (die „Krabbler"-Richtungen der Opfer-FSM 0x8010a2cc: Opfer-Clips 8/11, Release-Blut am **Knie-Bone 3**, Wund-Helper [2]/[3] = Panels 1/2). Kill → cmd `6` aca59 2/3 → generischer Devour-Kill 0x8010a6f8 (HP:=−1 @Frame 0x23). Die Maschine ist **code-komplett und lauffähig — aber es existiert game-weit KEIN Spawn mit Behavior-Nibble 1/3** (SCD-Zensus Audit wf_827f186d: STAGE1 = 0 Girl-Spawns, STAGE4 ROOM4050/4051 = Behavior 0x00 → Mode 0). **Dormant per Content.**
2. **Die f890-Instanziierung als Zombie-States 0x24–0x28 (aca59 38/39) ist toter, gebrochener Code:** Es existiert **kein Committer** (vollständiger Binär-Zensus: kein `li 0x2401/0x2501/0x2601/0x1501…0x1801`, kein `sb 0x24..0x28 → +0x5`; einziger Producer = das interne `+0x5 += 2` @0x80103f18-24), die Steer-Tabelle (20 Einträge) läuft für +0x5 ≥ 20 in die Move-Tabelle über (State 0x25 bekäme den Knockdown-MOVE 0x8010512c als „Decide"), und der Opfer-Dispatch für aca59 = 38/39 endet in **`jalr 0` (Crash-Lane)**: Hook A `0x801201a8[38]` = 0x8010b8f8 (ein Girl-ROOT-Handler, auf dem SPIELER ausgeführt!) → `0x80120374[38]` = **0x00000000**. Hook B `0x801201d8[38/39]` = 0x80102540 = `jr ra` (No-Op).
3. **Ein „liegender lebender Spieler" als Angriffsfenster existiert nicht:** Der echte Knockdown (cmd-2-Substates [4]/[5], `analysis/player_knockdown.md`) hält den Spieler durchgehend **unverwundbar** (`+0x93 |= 1` bis zum Exit `aca58:=1`) und steht von selbst wieder auf — und **alle** Fress-/Grab-Decides der Zombie-Familie gaten auf `DAT_800acae7 == 0` (player+0x93). Der einzige persistente Liege-Zustand (cmd-4-Plc-Mode 6, Krähen-Wurf) wird von keinem Devour-Decide gesondert behandelt.

---

## 1. Die Tabellen-Architektur (Fundament; alles Tabellen-Dump-belegt)

### 1.1 Standard-Zombie (Typen 0x10–0x12, Root LAB_80100424, Registrierung `_DAT_80072bec/f0/f4` @FUN_8011e864)

```
Root-State-Tabelle @0x8011f7b4[+0x4]: [0]=INIT 0x80100688, [1]=0x80101224, [2]=HURT 0x80105a8c,
                                      [3]=DEATH 0x80106ba4, [4]=0x8010919c, [7]=CORPSE 0x80109554
State 1 (FUN_80101224): Dispatch @0x8010165c-70:  jalr 0x8011f80c[grid(+0x9) & 0xf]   ; GRID-SUBMODE
Submode 0 = 0x8010168c:  jalr 0x8011f840[+0x5]  (STEER/Decide, @0x801016a8-bc)
                          + jalr 0x8011f890[+0x5]  (MOVE/Animate, @0x801016dc-f0)
```

⚠ **Steer-Tabelle 0x8011f840 hat exakt 20 Einträge** (0x8011f840–0x8011f88c; @0x8011f890 beginnt die Move-Tabelle). Für +0x5 ≥ 20 liest der ungeprüfte Dispatch die Move-Tabelle als Steer: State 0x24 → „Steer" = f890[16] = 0x80104f80, 0x25 → f890[17] = 0x8010512c (Knockdown-MOVE!), 0x26 → 0x801054f4, 0x27 → 0x801057bc, 0x28 → 0x801035f8.

Move-Tabelle f890 (48 Einträge gedumpt): [36] = **FUN_801036dc**, [37]/[38] = **FUN_80103b94**, [39]/[40] = **FUN_80104548**, [41] = 0x80104808, [42] = 0x80104f80; [21]/[22] = 0x80103b8c und [23]/[24] = 0x80104540 sind **bare `jr ra`** (roh verifiziert; ebenso 0x80102540, 0x80109e44).

### 1.2 Zombie-Girl (Typ 0x13, Root FUN_8010a8c8; Registrierung `_DAT_80072bf8` @_ALL:12267 = 0x80072bac[0x13])

```
Root-State-Tabelle @0x80120208[+0x4] (Dispatch @0x8010aa0c-2c):
  [0]=INIT 0x8010ab2c, [1]=ACTIVE 0x8010b274, [2]=HURT 0x8010bf80, [3]=DEATH 0x8010c014,
  [4]=0x8010919c, [7]=CORPSE 0x80109554 (setzt em-status-Kill-Flag via 0x8004ef90(0x800b1038/58,+0x1c6))
ACTIVE: Mode-Dispatch @0x8010b694-b6b8:  jalr 0x80120230[grid & 0xf]
  Mode 0 = 0x8010b6d4: Steer @0x80120264[+0x5] (@0x8010b724-38) + Move @0x801202a8[+0x5] (@0x8010b758-6c)
           — beides wortidentisch zu 0x8011f840/0x8011f890 [0..0x10] bis auf [0xa] (Girl-Floor-Drop
           0x8010bbe0/0x8010bbe8); vom Port word-diff-verifiziert (enemy_ai_common.c:6308-6315)
  Mode 1 = 0x8010b784: Steer @0x801202ec[+0x5] (@0x8010b7a0-b4) + Move @0x80120308[+0x5] (@0x8010b7d4-e8)
           ← DIE FRESS-MASCHINE (§2/§3)
  Mode 2 = 0x8010b800 (Steer @0x80120324 + Move @0x801202a8), Mode 3 = 0x8010b87c (nur Move @0x80120308),
  Modes 4-6 = 0x8010b8f8/0x8010b974 (Wake-Paar 0x80103980/0x80103a58), 7-12 = 0x8010b9f0/0x8010ba6c/…
```

**Mode-1-Untertabellen** (Dump @0x801202ec / @0x80120308):

| +0x5 | Steer @0x801202ec | Move @0x80120308 | Bedeutung |
|---|---|---|---|
| 0 | **0x801035f8** (Decide) | **0x8010be50** (Kriech-Annäherung, Clip 0x1a) | Anpirschen |
| 1/2 | 0x80103b8c (`jr ra`) | **FUN_80103b94** | Latch + Biss-Loop (vorn/hinten) |
| 3/4 | 0x80104540 (`jr ra`) | **FUN_80104548** | Kill-Biss (Clips 9/10) |
| 5 | **0x8010466c** (Re-Attack-Decide) | 0x80104808 (Hock-Clip 0xc; `+0x1d8 &= ~0x10` @_ALL:1633) | Kauern/Lauern (**Spawn-State**) |
| 6 | 0x80104f78 (`jr ra`) | 0x80104f80 (Clip 0x12 Auf-/Ablege-Übergang, Exit 0x201/0x10001) | Script-Übergang |

### 1.3 Erratum an player_hit_chain.md §2.2: „Zombie-Girl 0x13, Action-Tabelle 0x80120fd4" ist der HUND (0x20)

Die dort der Girl zugeschriebene FSM-Familie (0x8010f15c Steh-Biss, 0x8010f80c Front-Grab, 0x8010fc60 Rück-Grab, 0x801100b4 Devour-Start, Dispatch @0x8010dce0-f0) hängt an **Root 0x8010d7f8 = `0x80072bac[0x20]` = DOG**: 0x8010d7f8 dispatcht `@0x80120f74[+0x4]` (@0x8010d84c-60, roh), dessen [1] = 0x8010dbcc den Steer/Move-Doppel-Dispatch `@0x80120f94[+0x5]` + `@0x80120fd4[+0x5]` enthält (@0x8010dc90-dcf4, roh). Deckungsgleich mit dem Port (Dog-Block zitiert 0x8010f80c/0x801100b4/0x8010fa18) und komplementär zur Knockdown-Dossier-Korrektur (0x80118xxx = 0x27-Boss). Die Girl-spezifischen Victim-Trampoline LAB_8010c16c/LAB_8010c1ac (Slots 0x800ac7a4/0x800ac8a4 = Typ 0x13) bleiben korrekt Girl.

---

## 2. (A) Wer committet die Fress-States — der vollständige Zensus

### 2.1 Negativ-Befund für die f890-Route (Zombie-States 0x24–0x28 / 0x15–0x18)

- **Kein Literal:** Instruktions-Scan über ganz STAGE1.BIN: keine einzige `ori/addiu rX,zero,imm`-Materialisierung von 0x2401/0x2501/0x2601/0x2701/0x2801/0x1501–0x1801. Auch die EXE ist leer (einziger Treffer `unpack_packet.c` = Datenkonstante).
- **Kein Byte-Store:** Zensus aller 218 `sb →+0x5`/`sw/sh →+0x4`-Sites (0x80100000–0x8011f000, je 7 Instruktionen Kontext): kein Site schreibt 0x24–0x28 oder 0x15–0x18. Die einzigen nicht-literalen +0x5-Quellen sind `+0x9 & 0x7f` (Spider-INIT @0x801166ac/0x801169f0/0x80116b4c) und Inkremente.
- **Einziger Producer überhaupt:** das FSM-interne `+0x5 += 2` @**0x80103f18-24** (`lbu v1,5(v0); addiu v1,v1,2; sb v1,5(v0)`) — 0x25→0x27 setzt aber voraus, dass 0x25 je erreicht wird.
- **FUN_801036dc (animate[36] = State 0x24) advanct sich NIE selbst** (voller Roh-Disasm 0x801036dc–0x80103850): Setup Clip 0x1a/Speed 0x1e/Hold 0xf/+0x1d7=8 (@0x801036f8-374c), Turn zu +0x1bc/+0x1be bei Anim-Flag 0x10000 (@0x80103764-84), SE 2 an Frames 0x23/0x50 (@0x801037b4-cc), Positions-Snap FUN_80109470 (Part-Matrix-Delta-Ausrichtung, kein State-Write) — **kein einziger Store auf +0x4/+0x5**.
- **Latenter em_set-Pfad scheidet aus:** die pc[18]≠0-Variante (@0x800425a4-e8) schriebe `+0x4=4` (Root-State 4), nicht State-1-Animate.

→ **Die States 0x24–0x28 der Standard-Zombie-Tabelle sind unerreichbarer Tabellen-Ballast** (RE2-Erbe-Layout), zusätzlich strukturell gebrochen (§1.1-Steer-Überlauf, §3.4-Crash-Lanes).

### 2.2 Die LEBENDE Route: Girl Mode 1 — drei Commit-Sites, ein Spawn-Einstieg

**Spawn-Einstieg (Girl-INIT 0x8010ab2c, Spawn-Pose-Decoder, roh):**
```
8010b0d8: lbu v0,9(a0)          ; grid = Sce_em_set-Behavior (pc[3] → +0x9)
8010b0e0: andi v1,v0,0xf
8010b0e8/f0: nibble==1 || nibble==3 ->
8010b0f8/fc:   Clip +0x94 = 0xc      ; Hock-Pose
8010b108/10c:  +0x5 = 5              ; Mode-1/3-State 5 = Kauern/Lauern
```
(Nibbles 4/7/9 → Clip 0xc→0x12, 5/8/0xa → Clip 0xe→0x14 = die Liege-Posen des Mode-0-Spektrums.)

**Commit-Site 1 — 0x801035f8 (Mode-1-Steer[0], läuft während der Kriech-Annäherung; identisch als Mode-0-Overflow-Row [0x11]):**
```
Gates:  DAT_800acae7 == 0            ; player+0x93 hit_react   (@0x8010360c-14)
        +0x1d0 < 0x4b0 (1200)        ; Distanz-Cache           (@0x80103628-34)
        arc_test(&playerX,0x200)==0  ; ±Kegel                  (@0x80103640-48)
        DAT_800acad6 == +0x82        ; gleiches Stockwerk      (@0x80103650-68)
Commit: 80103678: addiu v0,v0,0x1    ; v0 = facing_aligned(0x8001a780) ∈ {0,1}
        8010367c: sll v0,v0,8
        80103688: ori v0,v0,0x1
        8010368c: sw v0,4(v1)        ; +0x4 = 0x101/0x201  → Mode-1-States 1/2 = FUN_80103b94
Alt:    (+0x1c4 & 0x2000) && !(+0x1d8 & 0x80) -> +0x4 = 0x601 (@_ALL:1127-1129)
```
**Commit-Site 2 — 0x8010466c (Mode-1-Steer[5], das Lauern):** identische vier Gates → `sw (facing+1)<<8|1` @**0x80104700**; zusätzlich `dist < 3000 && arc==0 → +0x4 = 1` (zurück in State 0 = Anpirschen; @_ALL:1591-1593).
**Commit-Site 3 — 0x80104760 (Mode-2-Steer[19]):** nur der Attack-Commit @**0x801047f0** (gleiche Gates).

**Spieler-Bedingung: KEIN Knockdown, keine aca5a-Phase, kein cmd-4.** Der Commit verlangt exakt: Spieler NICHT in Hit-Reaktion (`acae7==0`), nah (<1200), im Kegel, gleiche Etage — den **stehenden, normal steuerbaren** Spieler. `facing_aligned` (FUN_8001a780) wählt vorn/hinten.

### 2.3 Kill-Advance und Handoffs (FUN_80103b94/FUN_80104548, Girl-Kontext +0x5 = 1..4)

| Site | Write | Wirkung |
|---|---|---|
| @0x80103cc0 | `sw ((+0x5)+1)<<8 \| 5 → aca58` | Latch: Spieler cmd 5, **aca59 = 2/3** |
| @0x80103ed4 | `sb 4 → aca5a` (Budget < 0) | Mash-Escape → Opfer-Phase 4 (Release + Wund-Stempel) |
| @0x80103f18-24 | `+0x5 += 2` (+0x9e==0 ODER HP<0) | 1/2 → **3/4** = Kill-Biss |
| @0x801045dc | `sw ((+0x5)-1)<<8 \| 6 → aca58` | Kill: Spieler cmd 6, **aca59 = 2/3** |

(In der toten f890-Instanziierung ergäben dieselben Formeln aca59 = 38/39 → §3.4-Crash-Lanes — der schärfste Beleg, dass diese Instanziierung nie lief.)

---

## 3. (B) Die FSMs im Detail

### 3.1 Zombie-Seite: FUN_80103b94 (States 1/2 — Latch + Biss-Loop)

| Phase (+0x6) | Tut (jede Konstante mit Adresse) |
|---|---|
| 0 | Clip **0x1b** (+0x94, _ALL:1339-Region), +0x95=0, +0x8f=7; `acbfc := self` (Grabber-Link); `FUN_8001ac38(&player)`; `+0x93 \|= 1`; word0 \|= 0x1000 (self+Spieler); `acbcc/acbd0 := +0x178/+0x17c` (Victim-Bank C); **`aca58 = ((+0x5)+1)<<8\|5` @0x80103cc0**; `DAT_800acae7 \|= 1`; Yaw-Snap `0x8001a8f8(&playerX,0x800)`; `+0x1d8 \|= 1` (Busy-Latch); SE 4 |
| 1 | `ad68` + `f314(+0x170,+0x174,0,0x200)` → Phase += wrap (Latch-Clip einmal) |
| 2 | Clip **0x1c** (Biss-Loop); **Budget +0x9c = 100** (`_DAT_800aca50 & 1` → **10**); Phase 3; **Devour-Zähler +0x9e = 0x5a (90)**; **Spieler-HP −5** |
| 3 | pro Biss-Clip-Wrap: **HP −1** + Blut `0x1500` @Zombie-Bone 10 / **Girl (Typ 0x13/0x14) Bone 11** (@0x80103e70/e80, Bone-Select `+0x6b8`/`+0x764`); Mash: `Budget −= 1 + 5·FUN_80037024()`; **Budget<0 → Phase 4 + `aca5a := 4` @0x80103ed4**; `+0x9e−−; ==0 \|\| HP<0 → +0x5 += 2` @0x80103f18-24 |
| 4 | Wurf-ab-Clip **0x1d**; +0x1d5 = 0x5a (Mercy); **`aca50 \|= 1`** (@0x80103f88-fa0 — macht ALLE Folge-Grabs/Devours game-weit kurz: Budget 10 statt 100 bzw. 5 statt 0x6e); SE 7 |
| 5 | anim → Phase += wrap; **Frame 0x19-Event mit `DAT_800aca5c & 4`-Gate** (§4) |
| 6 | **Selbst-Tod:** `word0 \|= 2` @0x80104134-40; `+0x9a := −1` @0x8010414c-50; `+0x4(u16) := 7` @0x8010415c-60 → Root-State 7 = 0x80109554 (Corpse + **em-status-Kill-Flag** → despawnt permanent). **Die Ambush-Girl ist ein One-Shot: Mash-Escape = ihr Tod.** |

### 3.2 Zombie-Seite: FUN_80104548 (States 3/4 — Kill-Biss) und die Nachbarn

- Phase 0: Clip = `+0x5 + 6` = **9/10** (dieselben Devour-Clips wie FUN_80102bd8 `+0x5+4`); `aca58 = ((+0x5)-1)<<8|6` @**0x801045dc**; SE 3 @Frame 0x28.
- Phase 1: anim → Phase 2. **Phase ≥ 2: `return` für immer** (Fress-Halte-Pose auf der Leiche — Game-Over läuft über cmd 7).
- 0x8010be50 (State 0 Anpirschen): Klon von FUN_801036dc mit **Speed 0x14 statt 0x1e** (@0x8010be6c-70) und ohne Frame-SE; Turn-Rate 0x10 zu +0x1bc/+0x1be (@0x8010bed0-dc), Positions-Snap FUN_80109470 (Part-Matrix-Deltas Bone 10/13, gated `+0x1d8 & 0x20/0x40` + Anim-Flag 0x2000).

### 3.3 Opfer-Seite (aca59 = 2/3): die bestehende generische Maschine, Krabbler-Richtungen

- **cmd 5** → Hook A (`0x800ac758[Typ]`): Standard-Zombies → LAB_8010a28c → `0x801201a8[2/3]` = **0x8010a2cc**; Girl → LAB_8010c16c → `0x801208fc[2/3]` = **0x8010a2cc** (Dump §1). Für aca59 ≥ 2: Opfer-Clip-Basis **8/11** (@0x8010a35c-380), Phase 4 (vom Zombie via `aca5a:=4` geschoben): Blut `0x1500` am **Spieler-Bone 3 = Knie** (`a2 = acbdc+0x244` @0x8010a54c), Wund-Helper `jalr 0x801201b8[aca59]`: [2] = 0x8010a244 (Panel 1, +50), [3] = 0x8010a268 (Panel 2, +50); Phase 5 → `aca58 := 1` @0x8010a664.
- **cmd 6** → Hook B (`0x800ac858[Typ]`): → `0x801201d8[2/3]` bzw. `0x8012090c[2/3]` = **0x8010a6f8**: Death-Bite-Clips 6/7, Blut-Paar 0x1500/0x2000 @Bone 8, **HP := −1 @Frame 0x23** (@0x8010a80c-14), → cmd 7 (@0x8010a8ac/b4).
- Damit ist geklärt, **wofür die aca59≥2-Richtungen der Opfer-FSM in STAGE1 existieren**: für die kniehohe Girl-Attacke (STAGE3/5 nutzen dieselben Richtungen für ihre `0x205`-Krabbler-Grabs, hit_chain §2.3).

### 3.4 Die „hohen Einträge" — Crash-/No-Op-Lanes (Aufgabe B, ungeprüfter Index)

Der cmd-6-Dispatcher 0x8010a6b8 indexiert `0x801201d8[aca59]` **ohne Bounds-Check** (roh @0x8010a6c8-e0); Hook A 0x8010a28c analog. Was dort für hohe aca59 liegt (Dump @0x80120190-0x80120480):

| aca59 | Hook A `0x801201a8[n]` | Hook B `0x801201d8[n]` |
|---|---|---|
| 0–3 | 0x8010a2cc (Grab-FSM) | 0x8010a6f8 (Devour-Kill) |
| 4–7 | 0x8010a1cc/a208/a244/a268 (Wund-Helper — als Victim-Handler falsch, aber harmlos) | **Daten-Vektoren 0x801201e8ff (0xfa600000…) → `jalr` in Daten = Crash-Lane** |
| 12–15 | 0x8010a6f8 ×4 | 0x80120208-Region (Girl-ROOT-Pointer als „Handler") |
| **38/39** | **0x8010b8f8/0x8010b974** = Girl-Mode-4/5-Root-Handler, auf dem SPIELER ausgeführt → deren Inner-Dispatch `0x80120374[38]` = @0x8012040c = **0x00000000 → `jalr 0` (Absturz)** | **0x80120270/74 = 0x80102540 = `jr ra` (No-Op)** |

Girl-Trampoline: `0x801208fc[38]` = @0x80120994 = 0x8010ce54 (Sprung mitten in Dog-Code), `0x8012090c[39]` = @0x801209a8 = **0x00000000**. → Die aca59-38/39-Bahnen sind in keiner Tabelle versorgt; die „Liegend-Opfer-FSM" für 38/39 **existiert nicht** — es gibt nur die 2/3-Maschine (§3.3).

---

## 4. (C) Das `DAT_800aca5c & 4`-Gate in Phase 5 (Frame 0x19 des Wurf-ab-Clips 0x1d)

**`DAT_800aca5c` = player+0x8 = CHARAKTER-INDEX (0..15).** Belege: einziger Writer EXE-weit @**0x8001d4f4** (`sb` aus dem Game-State-Byte `DAT_800b0fbe`; Rückrichtung `FUN_80026f48`); Konsumenten-LUTs `DAT_80073f70[char]` = Player-Model-IDs **0x3c..0x4b = PL00..PL0F** (16 Charaktere) und `DAT_800741e8[char]` = PLW-Datei-Basis = **{0x4c,0x4c,0x4c,0x4c, 0x61,0x61,0x61,0x61}** → **Bit 2 trennt den Leon-Block (Chars 0–3, PLW 0x4c) vom Elza-Block (Chars 4–7, PLW 0x61)** (deckungsgleich mit dem Weapon-Render-Befund und blood_decals-LUT `0x80074208[char*0x10]`, 4 Zeilen pro Block). FUN_80031c44 ruft für den &4-Block zusätzlich pro Frame FUN_80024c30 (@Decompile Z.25-27); re15_damage.c:328 fand die &4-Alternativ-Reach-Row byte-identisch.

Das Gate im Escape-Pfad (roh):
```
80103ffc-4008: lbu 0x800aca5c; andi v0,v0,4
8010400c:      bne v0,zero,0x801040c4          ; Elza-Block ->
; LEON-Block (aca5c&4 == 0), Frame +0x95 == 0x19 (@0x80104020-28):
80104030:      sb 1,440(a0)                    ; +0x1b8 := 1
8010403c-80:   a0=0x2000; puVar = pool + u8[0x8011f784+typ]*0xAC (Gore-Bone 14/Girl 8)
               jal 0x80019700 @0x80104080      ; grosses Blut am eigenen Gore-Bone
               puVar[0x1a]=0x6f; +0x94/96/9a/9e-Felder des Parts = 0; *puVar |= 0x4a
               ; = Fleisch-Riss-Detach ("Brocken im Maul"), SE 9
; ELZA-Block (@0x801040c4ff), Frame == 0x19:
801040e0-f0:   +0x1d8 |= 2 ; SE 9 (a0 @0x801040dc)
801040f4-11c:  rng&1 -> SE 5 (@0x8010410c) sonst SE 8 (@0x8010411c)  ; kein Gore-Detach
```
**Semantik: charakterabhängige Escape-Inszenierung** — beim Leon-Block reißt die Girl beim Abschütteln sichtbar Fleisch heraus (eigener Gore-Bone wird geflaggt + 0x2000-Blut), beim Elza-Block nur Sound-Paar + ein Girl-lokales Flag (+0x1d8 Bit 1; Leser OFFEN, §8). Danach in beiden Fällen Phase 6 = Selbst-Tod (§3.1).

---

## 5. Wechselwirkung mit dem Knockdown (Abhängigkeit `analysis/player_knockdown.md`)

- Der echte Knockdown ist **cmd 2 Substate 4/5** (nicht cmd 4), läuft Fall→Aufstehen in EINER FSM, Spieler dabei **unverwundbar** (`+0x93 |= 1` @0x80036178/0x800364e4 bis Exit) — **`DAT_800acae7 != 0` blockiert währenddessen JEDEN Fress-/Grab-Commit** (§2.2-Gate 1). Es gibt kein „liegt und wartet"-Fenster.
- cmd 4 ist die Plc-Script-Klasse; einziger persistenter Liege-Zustand = Mode 6 (Krähen-Wurf). Kein Devour-Decide prüft cmd 4/Mode 6 — ein „Zombie frisst den Geworfenen"-Pfad existiert nicht (die Decides sähen höchstens den normalen Steh-Grab 3/4, wenn `acae7` dort 0 ist — Zustand von +0x93 im Mode-6-Halt = OFFEN im Knockdown-Dossier).
- Damit ist die Aufgaben-Hypothese „Knockdown [4]/[5] → liegend → 0x25/0x26 → Devour" **widerlegt**: weder existiert ein Committer (§2.1), noch ein verwundbares Liege-Fenster, noch eine lauffähige 38/39-Opferbahn (§3.4).

---

## 6. (D) PORT-IST und minimale byte-true Kette

### 6.1 IST (code-verifiziert, `re15_port/engine/src/enemy_ai_common.c`)

| Original | Port | IST |
|---|---|---|
| Girl-Root/Mode-0 (Tabellen §1.2) | `re15_zgirl_ai_tick` (:6390ff, Audit wf_827f186d) | ✓ byte-true inkl. Overflow-Row [0x11] (= Commit-Site 1 als Mode-0-Instanz, `re15_zgirl_overflow_row11` :6327-6341) und Spawn-Pose-Decoder inkl. `sel==1||3 → motion 0xc, +0x5=5` (:6434) |
| **Girl Mode 1 (Fress-Maschine §2.2/§3)** | `if ((e->grid_id & 0x0f) != 0) break;` (:6480) | **bewusst NICHT geroutet** — Port-Kommentar: „mode 1 … has ZERO spawns game-wide"; SCD-Zensus im Audit-Header (:6296-6298) |
| f890-States 0x24–0x28 | — | nicht portiert — **korrekt** (kein Committer im Original, §2.1) |
| Opfer-Richtungen aca59 2/3 (Clips 8/11, Knie-Blut, Helper [2]/[3]) | Victim-FSM kennt nur variant 0/1 (`g_player_victim_variant`, :535) | **FEHLT** (bereits als hit_chain-F2-Bestandteil gelistet: Helper [2]/[3] = dir 2/3) |
| Knockdown-Klasse (Voraussetzungs-Dossier) | — | FEHLT (player_knockdown F1) — für DIESES Dossier ohne Folgen (kein Original-Pfad koppelt Knockdown→Devour) |

### 6.2 Byte-true Konsequenz für den Port

1. **Nichts erfinden:** Es gibt KEINEN zu portierenden „Liegend-Devour-Einstieg". Ein Port-Committer für 0x24/0x25/0x26 oder ein „Zombie frisst den Umgeworfenen"-Feature wäre ein Rate-Defekt (Negativ-Zensus §2.1 ist der Beleg dagegen).
2. **Falls die dormante Girl-Ambush je aktiviert werden soll** (Modding/Parity-Lab, Spawn mit Behavior 1/3), ist die minimale byte-true Kette exakt: Mode-Dispatch `0x80120230[grid&0xf]` (:6480-Erweiterung) → Steer/Move-Paar §1.2-Tabelle → Commits §2.2 (vier Gates + `(facing+1)<<8|1` @0x8010368c/@0x80104700) → FUN_80103b94-Phasen §3.1 (Budget 100/10, +0x9e=0x5a, HP −5/−1, Blut @Girl-Bone 11, `aca5a:=4` @0x80103ed4, `+0x5+=2` @0x80103f18-24) → FUN_80104548 §3.2 (`((+0x5)-1)<<8|6` @0x801045dc) → Opfer-Seite: Victim-Variant **2/3** in der bestehenden Maschine ergänzen (Clips 8/11 @0x8010a35c-380, Knie-Blut Bone 3 @0x8010a54c, Wund-Helper [2]/[3] @0x8010a244/a268, Kill über den VORHANDENEN 0x8010a6f8-Port-Pfad) → Escape: §4-Gate (`aca5c&4`) + Selbst-Tod §3.1 Phase 6 (word0|=2, HP=−1, Root 7 + em-status-Flag).
3. **Unabhängig sinnvoll (echte Lücke):** die Opfer-Richtungen 2/3 werden auch von den STAGE3/5-Krabbler-Grabs (`0x205`-Writer, hit_chain §2.3) gebraucht — dieses Dossier liefert dafür die Clip-/Blut-/Helper-Konstanten (§3.3).

---

## 7. Errata an Schwester-Dossiers

1. **player_hit_chain.md §2.2 „Zombie-Girl 0x13"-Block** (0x8010f15c/0x8010f80c/0x8010fc60/0x801100b4, „Action-Tabelle 0x80120fd4", „Dispatch @0x8010dce0-f0"): gehört dem **DOG (0x20)** (Beleg §1.3). Die dortigen Inhalte (Sites/Werte) bleiben korrekt, nur der Besitzer-Typ ist zu ändern.
2. **bite_blood_fx.md §2.4**: FUN_80103b94 ist nicht „Zombie frisst den LIEGENDEN Spieler", sondern die **Girl-Mode-1-Ambush am stehenden Spieler** (Knie-Biss, aca59 2/3); die 0x25/0x26→„aca59 38/39"-Lesart beschreibt die tote f890-Instanziierung. Deren Offen-Punkte 1 („Einstieg 0x25/0x26") und 6 („aca5c&4") sind hiermit geschlossen; Offen-Punkt 3 aus hit_chain §6 („wer nutzt aca59≥2") ebenfalls (Girl + STAGE3/5-Krabbler).
3. **bite_blood_fx.md §2.4/Addendum**: „0x801201a8[38/39] = 0x8010b8f8/0x8010b974 — beide OHNE eigene Spawns" ist zu schwach formuliert: die Einträge sind **keine Opfer-Handler**, sondern fehlgeroutete Girl-Root-Handler mit `jalr 0`-Folge (§3.4).

---

## 8. Offene Punkte (ehrlich OFFEN)

1. **Visuelle Verifikation der Girl-Clips** 0xc (Hocken), 0x1a (Kriechen/Anpirschen), 0x1b/0x1c/0x1d (Latch/Biss/Wurf-ab) und der Opfer-Clips 8/11 (PL-Victim-Bank): Sequenzlogik byte-belegt, Sichtprüfung (Render-Katalog EM13 / DuckStation-Forced-Spawn) steht aus. Nächster Weg: `re15_ss_patch.py` — em_set-Behavior eines Test-Spawns auf 0x01 patchen und live fahren.
2. **Leser von Girl `+0x1d8` Bit 1** (Elza-Zweig §4) und Bit 0x10 (State-5-Clear @_ALL:1633): nicht identifiziert.
3. **`_DAT_800aca50` Bit 0** („einmal entkommen → alle Folge-Budgets kurz", §3.1 Phase 4 + FUN_80102548 case 2): Reset-Zeitpunkt (Raumwechsel? Spieler-Load?) nicht kartiert.
4. **Girl-Modes 2/3 und 7–12** (0x8010b800/0x8010b87c/0x8010b9f0/0x8010ba6c/0x8010bae8/0x8010bb64): Tabellen gedumpt, Semantik nur für die Fress-relevanten Pfade verfolgt.
5. **Game-weiter Spawn-Zensus über STAGE2–6-SCDs** für Behavior-Nibble 1/3 bei Typ 0x13/0x14: STAGE1+4-Aussage stammt aus Audit wf_827f186d (SCD-Walk); ein eigener Voll-Walk aller 240 RDTs mit dem SCD-Parser (statt Byte-Heuristik — die produziert Falsch-Treffer in Nicht-SCD-Sektionen) wäre der saubere Abschluss.
6. **STAGE2-6-Pendants** der Mode-1-Maschine (Shared-Library liegt in jedem Overlay, z.B. STAGE2 @0x80103b54): Tabellen-Layouts nicht einzeln gedumpt.
7. **Zustand von player `+0x93` im cmd-4-Mode-6-Halt** (Krähen-Wurf): entscheidet, ob dort theoretisch der Steh-Grab committen kann — hängt am Knockdown-Dossier-Offen-Punkt 1.
