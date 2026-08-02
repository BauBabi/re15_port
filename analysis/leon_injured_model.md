# Verletzungs-Optik des Spielers (RE1.5 MZD) — PLD-Slots, Index-Mechanismus, Port-Ist

**Datum:** 2026-08-02 · **Quellen:** `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Datei-Offset = addr−0x80010000+0x800), `info/Re1.5/PSX/BIN/{TITLE,DEBUG,STAGE*}.BIN` (@0x80100000 bzw. DEBUG @0x800c0000), `info/Re1.5/re15_save_final.bin` (MZD-Disc-Image MODE2/2352), `stage_saves/*.sav` (DuckStation-RAM), `re15_port/`.

---

## 0. Executive Answer (Modell vs. Textur)

**Beides — aber nicht so, wie der Report vermutet.** Die „verletzten Versionen" sind **komplette separate PLD-Dateien** (eigenes MD1-**Mesh** + eigene TIM-**Textur**; EDD-Anims + EMR-Skelett pro Charakter geteilt). Der Lade-Index ist ein **Slot-Byte `DAT_800aca5c`**, das eine 16-Einträge-File-ID-Tabelle **`@0x80073f70`** indiziert (PL00..PL0F). Der Reload-Mechanismus beim Raumwechsel **existiert und ist byte-belegt** — aber im MZD-Build gibt es **keinen einzigen Code-Pfad, der den Slot aus der Condition (HP) berechnet**. Der Slot-Writer-Zensus (alle Binaries, alle Opcodes) ist geschlossen: Slot wird nur vom Title-Screen (0=Leon/4=Elza) und vom Save-Record-Roundtrip geschrieben. **Der HP-gesteuerte Modell-/Textur-Wechsel ist in diesem Build dormant.** Zusätzlich trägt **jede** PLD-Textur eine dritte, blutige Texture-Page + dritte CLUT-Zeile, die vom Basis-Mesh **nicht referenziert** wird (Altlast eines Damage-Systems, kein Nutzer im Code gefunden). Was bei Caution/Danger im MZD-Build **wirklich** sichtbar umschaltet, ist (a) die Injured-Idle-**Animation** (PL00.EDD Clip 22/23, Schwellen 0x32/0x1e) und (b) die 2D-Condition-Anzeige im Inventar (Selector `FUN_8004ed6c`, Schwellen 0x50/0x14) — beides ist im Port bereits vorhanden.

---

## 1. (B) Die Index-Formel — byte-belegt

### 1.1 Zentraler CD-Loader per File-ID: `FUN_80013b60`

```
80013b60 addiu sp,sp,-0x18
80013b64 ori   v0,zero,0x1
80013b6c sh    a0, DAT_800be572      ; file-id merken
80013b70 sll   a0,a0,0x3             ; id*8
...
80013ba0 lw    v0, 0x8006f43c(a0)    ; Tabelle @0x8006f43c, Stride 8: +0 = Byte-Größe
80013bbc lbu   v0, 0x8006f442(a0)    ; +6 = LBA-Highbyte
80013bcc lhu   v1, 0x8006f440(a0)    ; +4 = LBA-Lowhalf
80013bd0 sll   v0,v0,0x10 / addu → 24-bit-LBA → DAT_800be57c
```

**File-Tabelle `@0x8006f43c`** (EXE-Datei-Offset 0x5FC3C, Stride 8: `{u32 size, u16 lba_lo, u8 lba_hi, u8 pad}`), vollständig gegen die Disc gematcht (Größe exakt + LBA-Reihenfolge):

| id | Datei | id | Datei |
|---|---|---|---|
| 0x00–0x07 | STAGE1..6.BIN, TITLE.BIN, DEBUG.BIN | 0x2c–0x3b | **STPIC_00..STPIC_0F.TIM** (16 Status-Portraits, je 12492 B) |
| 0x25 | DOOR00.DO2 | **0x3c–0x4b** | **PL00.PLD..PL0F.PLD** (16 Player-Slots) |
| 0x26/0x27 | CDEMD0/1.EMS | 0x4c–0x60 | PL00W00..W14.PLW (Leon-Waffen) |
| 0x2b | CAPCOM.STR | 0x61–0x75 | PL04W00..W14.PLW (Elza-Waffen); 0x76 = PL0FW00 |

### 1.2 In-Game-Player-Load: `FUN_800314b0`

```
800314d4 lbu  v0, DAT_800aca5c       ; SLOT-Byte (0=Leon-Basis, 4=Elza-Basis)
800314f8 sll  v0,v0,0x1              ; Halfword-Index
80031500 addiu at,at,0x3f70          ; PLD-SLOT-TABELLE @0x80073f70
80031508 lhu  a0, 0x80073f70[slot]   ; → file-id
8003151c jal  FUN_80013b60           ; lädt in Buffer 0x801bd814 (resident PLD)
```

**Slot-Tabelle `@0x80073f70`** (16 u16, EXE == Savestate-RAM, unverändert): `[0x3c,0x3d,0x3e,0x3f, 0x40,0x41,0x42,0x43, 0x44..0x4b]` = **PL00+slot**. Formel: **`pld_file_id = 0x3c + DAT_800aca5c`**.

### 1.3 Waffen-Bank-Tabelle bestätigt die Slot-Semantik

`FUN_80036b68` (Waffen-PLW-Load): `file_id = DAT_800741e8[slot] + weapon_id` (`@0x80036b84 lbu DAT_800aca5c` … `@0x80036bb4 lhu DAT_800741e8[slot*2]` … `@0x80036bc0 jal FUN_80013b60`, add im Delay-Slot). **`@0x800741e8` = `[0x4c,0x4c,0x4c,0x4c, 0x61,0x61,0x61,0x61]`** (Savestate-verifiziert): Slots 0–3 → Leon-Waffenbank, Slots 4–7 → Elza-Waffenbank. **Vier identische Einträge pro Charakter = die 4 Slots je Charakter sind Varianten desselben Charakters** (Design-Beweis: gleiche Waffen, anderes Modell).

### 1.4 Geht die CONDITION in den Index ein? — NEIN (Zensus geschlossen)

Vollständiger Binär-Scan (EXE + alle 8 Overlays) nach **allen** Instruktionen mit imm16 `0xca5c` (Adress-Lowhalf von `DAT_800aca5c`), plus `0x0ff0`/`0x0fbe` (s.u.): **Slot-Writer sind exakt:**

| Adresse | Binary | Code | Bedeutung |
|---|---|---|---|
| `@0x801016ac` | TITLE.BIN | `sb v0,DAT_800aca5c` | Player-Select-Commit: `char<<2` (0/4) |
| `@0x801024c0/24d4` | TITLE.BIN | `sb zero` / `sb v0(=4)` | zweiter Title-Pfad (0 oder 4) |
| `@0x8001d4f4` | EXE | `sb v0,DAT_800aca5c` | Record-Apply: `v0 = lbu DAT_800b0fbe` (Save/NewGame-Record +2) |
| `@0x8003978c` | EXE | `sb v0,0(s0=aca5c)` | Room-Load-Merge: `(slot&0xf0)\|DAT_800b0ff0` (§2) |

Die beiden Quell-Globals `DAT_800b0ff0` (Pending-Variante) und `DAT_800b0fbe` (Record-Slot) haben **je genau einen Writer**, und beide schreiben nur den aktuellen Slot zurück (`@0x8001d558 sh a0(=slot),DAT_800b0ff0`; `@0x80026f6c sb v0(=slot),DAT_800b0fbe`). **Geschlossener Kreis ohne Condition-Input.** Auch `PATCH.BIN` (MZD-Fan-Patch, LBA 73388, „PTCH") enthält keine Referenz auf Slot/Tabelle/Pending. Die HP-Schwellen-Xrefs außerhalb HUD/Idle-FSM: Die Schwellen-Tabelle `DAT_800112b4` (=0x50/0x14/0x40/0x10) wird **nur** von `FUN_8004ed6c` gelesen (XREF-genau), und `FUN_8004ed6c` hat **genau 3 Call-Sites**, alle 2D-Inventar (`FUN_80047648` ×2, `FUN_80049a5c`). Kein 3D-/Load-Pfad liest die Condition.

---

## 2. (C) Trigger — wann wird der Index ausgewertet?

**Ausschließlich beim Raumwechsel/Load.** `FUN_800314b0` hat **einen einzigen Caller**: `@0x80039788` in `FUN_800396fc` (Room-Load-Orchestrator, vom Raum-Lader `FUN_8001d600`):

```
80039760 lbu  a0, DAT_800aca5c        ; aktueller Slot
80039768 lh   v1, DAT_800b0ff0        ; ANGEFORDERTE Variante (Record +0x34)
8003976c andi v0,a0,0xf               ; slot & 0xf = PLD-Index
80039770 beq  v0,v1 → skip            ; identisch → KEIN Reload
80039774 _andi v0,a0,0xf0
80039784 or   v0,v0,v1                ; neuer Slot = (alt&0xf0)|angefordert
80039788 jal  FUN_800314b0            ; PLD NEU LADEN (CD → 0x801bd814 → VRAM)
8003978c _sb  v0, DAT_800aca5c
```

- **Kein Live-Swap im Raum:** kein anderer Caller, kein VRAM-Upload-Pfad bei Schwellen-Unterschreitung. Bei Schaden ändert sich nur HP (`DAT_800acaee`, Writer u.a. `@0x80012e64` EXE-Damage + Stage-Overlays) — nie der Slot.
- Der Vergleichswert `DAT_800b0ff0` ist Teil des **persistenten Game-State-Records `@0x800b0fbc`** (Länge 0x1230; Save-Copy `@0x800261d4 jal FUN_8004ee38` mit `a1=0x800b0fbc, a2=0x1230`; Layout: +0 Stage `DAT_800b0fbc`, +1 Raum, **+2 Slot `DAT_800b0fbe`**, +4/+6/+8 X/Yaw/Z `@0x8001d4a0-d4c8`, **+0x34 Pending-Variante `DAT_800b0ff0`**). Ein verletzter Look **würde** also Save/Load überleben — wenn ihn je jemand setzte.
- Savestate-Beweis (alle 90 `stage_saves/*.sav`): `DAT_800b0ff0 == 0` und `DAT_800aca5c == 0` überall, auch bei HP=75 (< Caution-Schwelle 0x50) in `mzd_stage1_hit_effect.sav`.

---

## 3. (A) Daten — die 16 PLD-Slots

PLD-Container (Header: `{u32 dir_offset, u32 count=4}`, dir = `[EDD, EMR, MD1, TIM]`; Parser-Referenz `PldExtractor.java:136-176`):

| Slot | Datei | file-id | Größe | EDD (md5-12) | EMR | MD1-Meshes | Textur (Page 0/1, CLUT-Zeile 0/1) | Optik (TIM dekodiert) |
|---|---|---|---|---|---|---|---|---|
| 0 | PL00 | 0x3c | 189108 | `038c9653a1ae` | `b467407eb887` | 17 | 8bpp 384×256, 3 CLUTs @y480 | **Leon R.P.D.-Uniform** |
| 1 | **PL01** | 0x3d | 190900 | == PL00 | == PL00 | 17 | dito | **Leon + „POLICE"-Schutzweste** |
| 2 | PL02 | 0x3e | 191052 | == PL00 | == PL00 | 17 | dito | **Leon U.S.S./Umbrella-Uniform** |
| 3 | PL03 | 0x3f | 190068 | == PL00 | eigen | 17 | dito | Leon R.P.D.-Variante (anderes Haar/Kopf) |
| 4 | PL04 | 0x40 | 190056 | `de98dbdd4c13` | `145d796429b1` | 21 | dito | **Elza „RACCOON"-Bikersuit** |
| 5 | PL05 | 0x41 | 190768 | == PL04 | == PL04 | 21 | dito | Elza R.P.D.-Outfit |
| 6 | PL06 | 0x42 | 191464 | == PL04 | == PL04 | 21 | dito | Elza U.S.S.-Uniform |
| 7 | PL07 | 0x43 | 182240 | == PL04 | == PL04 | 21 | dito | Elza Haut/Casual (Teil-Page leer = unfertig) |
| 8 | PL08 | 0x44 | 182928 | == PL04 | eigen | 15 | dito | weiblicher NPC (Elza-Anims, eigenes Skelett) |
| 9 | PL09 | 0x45 | 182752 | == PL04 | eigen | 19 | dito | weiblicher NPC |
| A–C | PL0A–PL0C | 0x46–0x48 | ~181–184k | `9e16e9c68831` (3×) | je eigen | 15 | dito | dritter Charakter, 3 Varianten |
| D–E | PL0D–PL0E | 0x49/0x4a | 179284/182220 | `338e989382ba` (2×) | je eigen | 15 | dito | vierter Charakter, 2 Varianten |
| F | PL0F | 0x4b | 126892 | eigen | eigen | 15 | 8bpp **256**×256, **2** CLUTs | kleiner Charakter (Sherry-artig; eigener PLW PL0FW00) |

**Kern-Deltas innerhalb einer Charakter-Gruppe:** EDD identisch, EMR identisch (Ausnahmen PL03/PL08/PL09+: eigene Skelett-Maße), **MD1 immer eigen** (Mesh; z.B. PL00 Torso 44/24/30 vs. PL01 53/40/31 tV/tF/qF — Weste ist Geometrie!) und **TIM immer eigen** (Pixel-Diff PL00↔PL02 97.9 %, PL04↔PL07 99.7 %). → **Slot-Bedeutung = Charakter × KOSTÜM/AUSRÜSTUNG** (nicht „Zustand"): je 4 Slots pro Hauptcharakter mit identischer Waffenbank (§1.3).

### 3.1 „PL01 fehlt" — aufgeklärt und geborgen

PL01.PLD steht in der File-Tabelle (id 0x3d, **LBA 9224**, 190900 B) und **liegt physisch auf der Disc** — es fehlt nur der ISO-Directory-Eintrag, weshalb Datei-Extraktionen es nicht sehen. Aus `re15_save_final.bin` per LBA extrahiert (Methode gegen PL00 bit-exakt validiert): EDD **und** EMR == PL00, eigenes MD1 (30708 B) + TIM. Kopie: Scratchpad `PL01.PLD`. → Empfehlung: nach `re15_port/shared_assets/PSX/PLD/PL01.PLD` übernehmen.

### 3.2 Die „verletzten" Assets IN jeder PLD — unreferenziert

Jede Spieler-TIM = 8bpp, 384×256 = **3 Texture-Pages**, mit **3 CLUT-Zeilen** @VRAM (0,480..482). MD1-UV-Zensus (korrekter Stride: tri-UV 12 B, quad-UV 16 B; `MD1File.java:334-380`): **alle** Polys aller geprüften Slots nutzen ausschließlich `page 0x80 + clut 0x7800` (Zeile y=480) oder `page 0x81 + clut 0x7840` (y=481). **Page 2 (x 256–383, enthält blutige Torso-/Arm-Duplikate — in PL00/PL02/PL04/PL05/PL06 klar sichtbar) und CLUT-Zeile 2 (0x7880) werden von keinem Poly referenziert.** Kein Code addiert +0x40 auf CLUT-IDs oder +1 auf Page (imm-Scan `0x7880` negativ). Die Extra-Meshes (PL00: #15/#16 mit 6/8 Verts; PL04: #15–#20 mit 4–8 Verts) sind Kleinteile (Holster/Taschen), keine Blut-Duplikate. → **Damage-Texturen sind vorbereitete, tote Assets** — konsistent mit dem toten Slot-Mechanismus.

### 3.3 STPIC = Status-Portrait pro Slot (16 ↔ 16)

`ITEM/STPIC_00..0F.TIM` (16×, je 12492 B, 8bpp 116×103, CLUT @ (0,485)) sind die Inventar-ID-Card-Portraits, **eines pro PLD-Slot**: Loader `FUN_800c01c4` (DEBUG.BIN-resident): `lbu DAT_800aca5c` → `lhu @0x80073ad8[slot]` → `jal FUN_80013b60` (Tabelle `@0x80073ad8` = `[0x2c..0x3b]`, EXE-verifiziert). Damit ist der frühere Port-Blocker „Q2 selector @0x800C01C4" gelöst: **STPIC-Index == PLD-Slot.**

### 3.4 Zwei getrennte Condition-Schwellen-Sets (beide byte-belegt)

| Nutzer | Schwellen | Beleg |
|---|---|---|
| Idle-Anim-FSM (Injured-Idle Clip 22/23) | HP < **0x32**/**0x1e** | `slti imm 0x32 @0x800321a0`, `slti imm 0x1e @0x800322a4` (je nach `lh DAT_800acaee`) |
| 2D-Status-Selector `FUN_8004ed6c` (Inventar: Portrait-CLUT, Condition-Panel `V += status*16` in `FUN_80047648 @0x80047a00`, CLUT-Array `@0x800b2610`) | Fine ≥ **0x50**, Caution ≥ **0x14**, Danger < 0x14, Poison = `DAT_800acaec` Bit 1 → 3 | `lb DAT_800112b4(=0x50) @0x8004ed74`, `lb DAT_800112b5(=0x14) @0x8004ed7c`, `andi v0,v0,0x2 @0x8004eda8`, `lh DAT_800acaee @0x8004edb8` |

Korrektur zur Wissensbasis: `RE15_KNOWLEDGE.md §5.7` („4 CLUT palettes per PLD, selected per-frame from HP status … Polygon renderer 0x80047648") ist **falsch zugeordnet** — die PLD-TIM hat **3** CLUTs, `FUN_80047648` ist ein **SPRT/2D-Inventar-Renderer** (Prim-Layout x@−2/y@0/u@+2/v@+3/clut@+4), und der Selector speist **nur** die 2D-UI.

### 3.5 Title-Screen-Sonderfall (MZD-Patch)

Player-Select (`FUN_80101720`, TITLE.BIN) lädt per eigener Tabelle `@0x80102690`: **on-disc `{0x2c,0x44}`, zur Laufzeit gepatcht auf `{0x3c,0x40}`** (= PL00/PL04; Savestate `mzd_title.sav`-RAM-Beweis). Die On-Disc-Werte sind Altstand einer früheren File-Tabellen-Nummerierung (die stale 9-PLD-Namensliste in DEBUG.BIN @0xbbd: PL00,PL01,PL02,PL04,PL05,PL06,PL0D,PL0E,PL0F — der historische Ursprung des `%9`-Ankers).

---

## 4. (D) Port-Ist

- **In-Game:** `re15_port/platform/pc/main.c:1956/1972/2158-2159` lädt **hart** `PLD/PL00.TIM/MD1/EDD/EMR` (vor-gesplittete Dateien). Kein Slot-Byte, keine Tabelle, kein Room-Load-Compare/Reload. `g_gameflow.character` (re15_gameflow.c:32) wird gesetzt, beeinflusst das In-Game-Modell aber nicht.
- **Player-Select:** `main.c:1055/1061` lädt PL00.* (Leon) / PL04.* (Elza) — **stimmt mit der Laufzeit-Wahrheit überein** (Kommentar L1040 begründet mit dem stale On-Disc-Wert 0x44=PL08, Ergebnis trotzdem korrekt, da Laufzeit 0x40=PL04).
- **Inventar:** Condition-Classifier byte-true (re15_inv_screen.c:189-192, Schwellen 80/20 + Poison-Bit); STPIC hardcoded `STPIC_00` (inv_render_pc.c:213-214, „blocker Q2" — durch §3.3 jetzt auflösbar).
- **Waffen-Compositing:** eigener dir[3]-TIM-Upload nach Slot0 @(200,364) pro Frame (Memory `reai-v2-weapon-render`) — orthogonal zum Slot-Mechanismus.
- **Injured-Idle-Anims:** vorhanden (player_common.c:97-98, Motion 213/214 → PL00.EDD Clip 22/23, HP<50/<30).

**Hook-Punkt für den Slot-Index im Port:** die Stelle, an der `room_apply_pending`/Room-Load das Spieler-Modell bereitstellt — dort das Original-Muster nachbauen: `slot`-Global (Analog `DAT_800aca5c`, von Player-Select 0/4), `pending`-Feld im Save-Record (Analog +0x34), Compare `(slot&0xf) != pending` → PLD `PL[0x3c+slot−0x3c]` neu laden (Tabellen-Analog `@0x80073f70`).

---

## 5. Divergenzen D1..D5

| # | Divergenz | Original-Beleg | Port-Ist | Fix-Vorschlag |
|---|---|---|---|---|
| **D1** | Slot-/Reload-Mechanismus fehlt komplett | §1.2/§2 (`@0x80073f70`, `@0x800396fc:80039760-8c`, `@0x800314b0`) | PL00 hardcoded | Slot-Byte + 16er-Tabelle + Room-Load-Compare/Reload implementieren (auch wenn im MZD-Build funktional dormant — er ist die Infrastruktur für D2/D4 und byte-true) |
| **D2** | Elza spielt in-game mit Leon-Modell | Original lädt `0x80073f70[4]`=PL04 nach Select-Commit `@0x801016ac` (slot=4) | main.c lädt immer PL00.* | In-Game-Modell-Load über D1-Slot statt hardcoded PL00 |
| **D3** | Nutzer-Report „Modell/Textur wechselt bei Caution/Danger" | **Im MZD-Build NICHT belegt**: kein Condition-Writer auf Slot/Pending (§1.4, Zensus vollständig); Savestates bestätigen slot=0 bei HP<80 | Port zeigt (korrekt) immer unverletzt; Injured-IDLE-Anim vorhanden | KEIN Fix ohne dynamischen Gegenbeweis (offene Frage 1). Das sichtbare „Verletzt" des Originals = Injured-Idle-Anim + Inventar-Condition — beides im Port |
| **D4** | STPIC-Portrait nicht slot-indiziert | `FUN_800c01c4`: `@0x80073ad8[slot]` (§3.3) | `STPIC_00` hardcoded | Slot-Index (aus D1) → `STPIC_%02X` laden; löst „blocker Q2" |
| **D5** | PL01.PLD fehlt in `shared_assets` | LBA 9224/190900 B in File-Tabelle `@0x8006f43c[0x3d]`; aus Image extrahiert, EDD/EMR == PL00 | Datei fehlt → Slot 1 unladbar | Geborgenes PL01.PLD (Scratchpad) nach `shared_assets/PSX/PLD/` übernehmen |

---

## 6. Offene Fragen (mit nächstem konkreten RE-Schritt)

1. **Dynamischer Gegen-Check des Nutzer-Reports:** Im DuckStation/PCSX-Redux-Lauf Schaden bis Caution/Danger nehmen, Tür durchqueren, dann `DAT_800aca5c`/`DAT_800b0ff0`/VRAM-Page-2-Bereich lesen (Skills `re15-room-capture` + `re15-pcsx-watchpoint`: Write-Watch auf 0x800aca5c und 0x800b0ff0). Erwartung nach Statik: keine Änderung. Falls doch → Writer ist ein indirekter Store, den der imm16-Zensus nicht sieht (Register-Basis ≠ lui 0x800b, z.B. `sh v,0x34(rec_ptr)`) — dann Watchpoint-PC = der fehlende Beweis.
2. **Wer nutzt TIM-Page 2 / CLUT-Zeile 2?** VRAM-Watch auf den Page-2-Bereich des Spieler-TIM-Slots beim Schaden/Tod. Kandidat: gar niemand (Altlast). Alternativ RE der „Blood-decal"-Spur aus `RE15_KNOWLEDGE.md §5.6` (Partikel 0x800A73B8, Sprite-State 0x800B25CC, „Apply-function not located") — das ist der wahrscheinlichere Träger des Live-Verletzt-Looks.
3. **Slot-Bedeutung 8..15 absichern:** PL08/PL09/PL0A-0F-Nutzer finden (Story-NPC-Spawns? Debug-Menü „PL No."?). Nächster Schritt: DEBUG.BIN-Menü-Code nach Writes auf `DAT_800aca5c` jenseits der 4 bekannten scannen (DEBUG.BIN liest den Slot @0x800c00ac/0x800c01d0/0x800c4618/0x800c4730 — Reads; ein Debug-Menü-Write würde den 16-Slot-Raum erklären).
4. **PL03/PL08/PL09-EMR-Deltas quantifizieren** (gleiche Clips, andere Bone-Längen?) — für byte-true NPC-Rendering, sobald diese Slots gebraucht werden.


---

## NACHTRAG 2026-08-02 (Session 2) — DYNAMISCHER GEGENBEWEIS: Es ist die ANIMATION

Nutzer bestand darauf, den Wechsel im Original zu sehen -> Beweislage selbst erzeugt
(re15_ss_patch.py: mzd_stage1_briefing.sav auf HP=25 gepatcht, DuckStation gefahren,
Capture; Sonde/Ablauf im Scratchpad der Session, Ergebnisse hier):

1. **HP 100 vs HP 25, gleiche Waffe (Messer W01), gleicher Raum — NULL VRAM-Differenz:**
   Textur-Streifen (448..639, 256..511) identisch, alle 3 CLUT-Zeilen (480/481/482)
   identisch, residente MD1 identisch (gleiche 255 `7840->7880`-Patches). Bei Danger
   wechselt mid-room weder Modell noch Textur noch Palette.
2. **Was wechselt: das Motion-Byte.** HP 100: motion=2. HP 25: **motion=23 = der
   Injured-Idle-Clip** (PL00-Bank; Schwellen @0x800321a0 `slti 0x32` / @0x800322a4
   `slti 0x1e`). Der sichtbare "verletzte Leon" des MZD-Builds IST die Verletzt-Pose/
   -Animation (gebueckt, Arm) — was als "anderes Modell" wahrgenommen wird.
3. **Frueherer Scheinbefund aufgeklaert:** Die in Session 2 zunaechst gemessenen
   VRAM-Diffs (Face-/Hand-Tile px 200..255 + CLUT-Block 224..239) zwischen
   `mzd_stage1_briefing` und `mzd_stage1_hit_effect` waren ein CONFOUNDER — die Saves
   haben VERSCHIEDENE Waffen equipped (aca5d 0x01 vs 0x03); das Rect ist das bekannte
   Waffen-Composite (PLW resident @0x801d7700; Datei-Offset-Beweis 0x5094/0x5348).
   Nebenbefund: PL00W00/W01/W02 und W03/W04 sind BYTE-IDENTISCHE Disc-Duplikate
   (file_id == weapon_id, Waffen teilen Art) — keine Zustands-Varianten.
4. **Nebenbefund fuer die Kraehen-Kampagne:** FUN_80031c44 gated `FUN_80024c30` auf
   `DAT_800aca5c & 4` (= Elza-Slots!) — der offene acc0c-Konsument D7 ist mit hoher
   Wahrscheinlichkeit Elzas Haar-/Sekundaerphysik. (Noch zu verifizieren.)

### Offen danach
- **Verletzter WALK**: Bei den DuckStation-Fahrten registrierte der D-Pad-Hold nicht
  (alle Captures zeigen Idle; DuckStation-Pad-Config pruefen — [[reai-v2-duckstation-config-update]]).
  Naechster Schritt: Pad fixen, HP25 GEHEND capturen, Motion-Byte vs HP100-Walk diffen ->
  belegt/widerlegt ein verletztes GEH-Set (Humpeln). Der Skript-Walker LAB_80030af0 hat
  KEINEN Condition-Fork (Clip 5 fix); der Pad-Locomotion-Pfad (cmd-0 LAB_800318f8) ist
  noch nicht auf Condition-Clips geprueft.
- **Port-Gegenprobe**: feuert der Injured-Idle (Sentinels 213/214 -> PL00 22/23) im Port
  LIVE und rendert er die richtige Pose? (Alte Sentinel-Kollision AIM_W==213 beachten,
  [[reai-v2-aim-lower-land-hunch]].) Visuell per gdigrab bei RE15-Lauf mit HP<30.
