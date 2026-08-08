# ROOM1030 — Kriechtor-Mechanismus: Einbau-Sicherheits-Dossier

**Stand:** 2026-08-07 · **Ziel dieser Runde:** NICHT ein Fix, sondern EINBAU-SICHERHEIT.
**Methode:** 6 Analyse-Lanes + adversariale Gegenprüfung, danach eine dritte, unabhängige
Verifikationsrunde durch mich selbst (eigene Disassembly aus `PSX.EXE` / `STAGE1.BIN` /
`STAGE2.BIN`, eigener RDT-Parse, eigene Port-Greps). Alles, was unten `BYTE_BELEGT` heißt, habe
ich in dieser Runde selbst gelesen — nicht aus einem Lane-Report übernommen.

> **Legende Sicherheit**
> `BYTE_BELEGT` = Instruktionen/Bytes selbst gelesen, Basisregister aufgelöst.
> `HERGELEITET` = aus belegten Teilen geschlossen, aber kein direkter Beleg.
> `OFFEN` = weder belegt noch widerlegt.

---

## 0. Executive Summary

Die Kette ist **end-to-end byte-belegt**. Gegenüber der letzten Runde haben sich **vier**
angebliche Blocker als **falsch** herausgestellt (sie sind erledigt), und es sind **zwei neue,
echte** Blocker dazugekommen, die vorher niemand gesehen hat.

**Vier Blocker der letzten Runde sind TOT** (ich habe sie selbst widerlegt):

| Alter Blocker | Status | Beleg |
|---|---|---|
| „Der 0x1000-Trigger ist statisch nicht auffindbar, braucht Savestate" | **TOT** | sub07 = `3d 04 10 / 26 00 05 04 00 10 / 35 10 04` — Opcode 0x26 (Calc) Operator 5 = OR mit Immediate 0x1000 |
| „Typ→Root nicht statisch auflösbar, evtl. läuft ROOM1030 auf `FUN_8011d6d4`" | **TOT** | `@0x8011e8a0-a4 sw v0,11268(at)` = `0x80072c04` = Typ **0x16** → `0x80100424`. ROOM1030 spawnt 20× Typ 0x16. |
| „Rect-Test halboffen → Port 1 Einheit daneben" | **TOT** | `@0x80042b78 sltu a0,a0,v1` + `bne` = **geschlossenes** Intervall; alle 8 ROOM1030-Extents gerade → Port exakt äquivalent |
| „Pool-Kapazität widersprüchlich (20/24/35)" | **TOT** | Kategorienverwechslung: 20 = Sce_em_set-Records, 35 = SCA-Zellen, AOT-Pool = 32 |

**Zwei neue, echte Befunde:**

1. **`FUN_8003ec28` @0x8003f194** — am Ende JEDES VM-Laufs wird `+0x0A/+0x0B` per `sh 0xFFFF`
   überschrieben (Spieler unbedingt, aktive Gegner/Objekte gegated). Das kannte keine Lane.
2. **`grid_id` Bit 0x80 ist im Port bereits als „DOWNED" belegt** (16 Lesestellen) — und im
   Original ist es **physisch dasselbe Bit**, das der Kriech-Commit setzt. Das ist jetzt der
   größte verbleibende Risikopunkt.

**Einbau-Urteil: NEIN** (Details §3). Der Mechanismus ist sicher, aber drei Dinge sind es nicht.

---

## 1. DIE KETTE, Glied für Glied

Die Kette ist **kein Einbahn-Schalter, sondern ein Zyklus** mit einem Hin- und einem Rückweg,
getaktet über einen 3-Frame-Handshake. Das ist die wichtigste inhaltliche Korrektur gegenüber
der ursprünglichen Skizze.

### Frame-Reihenfolge (die Grundlage von allem)

```
@0x8001cdec  jal 0x8003f038   SCD-VM          <- Skript liest Stempel von Frame N-1
@0x8003f18c  jal 0x8003ebf4     work_vars[0..3] = -1, 0x800B102C = 0
@0x8003f194  jal 0x8003ec28     +0x0A/+0x0B = 0xFFFF          <-- NEU GEFUNDEN
@0x8001ce04  jal 0x8001a50c   Gegner-AI
@0x8001ce0c  jal 0x80031c44   Spieler
@0x8001ce1c  jal 0x800436a8   AOT-Scan        <- Clear + Stempel für Frame N
```
`BYTE_BELEGT` (selbst gelesen). **Konsequenz:** Das Skript liest IMMER den Stempel des
**Vorframes**. Ein Port, der den Stempel vor dem VM-Tick setzt, verschiebt die ganze Kette um
einen Frame.

---

### Glied 1 — AOT-Stempel auf `entity+0x0B`

**Original.** `FUN_80042bac` stempelt den 0-basierten Aot_set-Slot-Index in `entity+0x0B`.

```
@0x80042bb4  addu s1,a0,zero        ; s1 = Entity-Pointer (a0)
@0x80042bcc  addu s2,zero,zero      ; Laufindex = 0
@0x80042c64  addiu s2,s2,1          ; Delay-Slot: +1 pro Array-Position
@0x80042f44  addiu v0,s2,255        ; Delay-Slot: low byte = s2-1
@0x80042f5c  sb v0,11(s1)           ; ACTION-Pfad
@0x80042fc4  sb v0,11(s1)           ; AUTO-Pfad
```
Dass der Wert die **Aot_set-ID** ist (nicht nur eine Array-Position), folgt aus dem Installer:
`@0x80040544 addiu v1,v1,-13904` = `0x800AC9B0`, direkt mit `pc[1]` indiziert, ohne Allokator.
Der Scan läuft über dasselbe Array (`@0x80042c48 addiu fp,a0,556` = `0x800AC9B0`).

**Sicherheit:** `BYTE_BELEGT`.

**Port.** Es gibt **keinen** Aktor-Stempel. Der einzige `member_0b`-Produzent ist
`re15_object_notch_update` (`aot_common.c:318-326`) — er läuft nur über `g_scd.props`
(Prop-Pool), filtert auf `type == RE15_AOT_TYPE_EXAMINE_WORKVAR` (sce=5) und schreibt
`g_scd.props[i].member_0b`, nie `g_actors[].member_0b`.

---

### Glied 2 — LAST-WINS (AUTO) vs FIRST-WINS (ACTION)

**Original.**
```
@0x80042f40  beq s6,zero,0x80042fc4   ; s6 = a2; a2==0 -> AUTO
@0x80042f48  lbu v0,0(s0)             ; ACTION-Pfad: sce==0 wird verworfen
@0x80042fbc  j 0x80043028             ; ACTION: EARLY RETURN  -> FIRST-WINS
@0x80043010  jalr v0                  ; AUTO: Handler
@0x8004301c  bne v0,zero,0x80042c50   ; AUTO: SCHLEIFE LÄUFT WEITER -> LAST-WINS
```
Alle drei Pool-Aufrufe übergeben `a2 = 0` (`@0x800436c0`, `@0x80043728`, `@0x80043790`) — der
Stempel-Pfad ist also **immer** LAST-WINS: **höchster Slot-Index unter allen Treffern gewinnt.**
Wichtig: der AUTO-Pfad hat **keinen** `sce != 0`-Filter (den hat nur ACTION).

**Sicherheit:** `BYTE_BELEGT`.

**Port.** Kein Stempel. Die vorhandene Gegner-Schleife `aot_common.c:905-914` bricht mit
`&& !flag_pool_inside` beim **ersten** Treffer ab und ist als Vorlage **falsch**.

> **ROOM1030-Quantifizierung** (Rechtecke selbst aus der RDT geparst):
> Zone 4 `x[-14800,-2800] z[-22500,-20300]` liegt **vollständig** in Zone 6
> `x[-27300,3300] z[-24500,-700]`. Zone 5 `x[-12900,-3700] z[-25300,-24200]` überlappt Zone 6
> nur in `z[-24500,-24200]` (300 Einheiten).
> **ABER:** sub02 schaltet Zone 4 und Zone 6 **streng wechselseitig** (siehe Glied 6) — die
> Total-Überdeckung 4⊂6 wird deshalb nie wirksam. Real relevant ist allein: im 300er-Streifen
> gewinnt Slot 6, dort feuert `Member_cmp(15,==,5)` **nicht**. Nur `z[-25300,-24500]` liefert 5.

---

### Glied 3 — Clear-Semantik (drei verschiedene Regeln + ein vierter Mechanismus)

**Original.** `FUN_800436a8`:
```
@0x80043704  andi v0,v0,0x1           ; Gegner: nur wenn word0 & 1 (aktiv)
@0x80043708  beq v0,zero,0x80043730
@0x8004371c  sb zero,0(at)            ; at = 0x800ACC37 = enemy+0x0B  -> 0
@0x80043788  sb zero,0(at)            ; at = 0x800B3FA3 = obj+0x0B    -> 0, UNBEDINGT
             (Spieler-Pool @0x800436b4-d0: KEIN Clear)
```

**NEU — `FUN_8003ec28`, unbedingt am VM-Ende (`@0x8003f194`):**
```
@0x8003ec30  addiu v1,v1,-13730       ; 0x800ACA5E = player+0x0A
@0x8003ec44  ori v0,zero,0xffff
@0x8003ec4c  sh v0,0(v1)              ; Delay-Slot -> läuft IMMER; player+0x0A/+0x0B = 0xFF
@0x8003ec68  sh v1,10(a0)             ; jeder AKTIVE Gegner, Stride 500
@0x8003eccc  sh a1,10(a0)             ; jedes AKTIVE Objekt, Stride 148
```

**Netto-Wirkung** (weil der VM-Tail VOR dem Scan läuft):

| Pool | 0xFF-Wisch | Scan-Clear | Wert außerhalb aller Zonen |
|---|---|---|---|
| Spieler | ja (unbedingt) | **nein** | **0xFF** — nicht „klebrig", wie eine Lane behauptete |
| Gegner (aktiv) | ja | ja → 0 | **0** |
| Gegner (inaktiv) | nein | nein | **stale** (Alt-Wert bleibt stehen) |
| Objekt (aktiv) | ja | ja → 0 | **0** |

**Sicherheit:** `BYTE_BELEGT`.

**Port.** Kein Clear, kein Wisch. Zusätzlich ist der Kommentar bei `aot_common.c:314-317`
(„an object over no cell keeps its last notch") als **Gesamt**verhalten **falsch** — das Objekt
wird jeden Frame zweifach genullt. Er zitiert außerdem `@0x80042f5c` (ACTION), obwohl alle Pools
über den AUTO-Pfad `@0x80042fc4` laufen. **Verdachtsstelle.**

---

### Glied 4 — Der Rechteck-Test

**Original.** `FUN_80042b64`:
```
@0x80042b6c  lw v1,0(a2)      ; Position X (32 bit)
@0x80042b70  lhu a0,4(a1)     ; Extent   (UNSIGNED 16 bit!)
@0x80042b74  subu v1,v1,v0    ; delta = pos - corner (corner via lh, signed)
@0x80042b78  sltu a0,a0,v1    ; a0 = (extent < delta)
@0x80042b7c  bne a0,zero,miss ; MISS wenn extent < delta  ->  HIT iff delta <= extent
```
Das ist ein **GESCHLOSSENES** Intervall `[corner, corner+extent]`, unsigned. Die
Lane-A-Behauptung „halboffen" ist **widerlegt**.

**Sicherheit:** `BYTE_BELEGT` (Rohbytes `2b 20 83 00 / 09 00 80 14` selbst dekodiert).

**Port.** `scd_vm.c:2303-2306` speichert Mitte+Halbextent, `aot_common.c:308` testet
`abs(px-cx) <= hw`. Für **gerade** Extents ist das **exakt äquivalent**. Alle acht
ROOM1030-Extents (3700/1800/12000/2200/9200/1100/30600/23800) sind gerade → **hier kein Fehler,
nichts zu tun.**
*(Latenter, game-weiter Nebenbefund außerhalb dieses Einbaus: das Original liest den Extent mit
`lhu`, der Port als `int16_t` mit `abs()`. Records mit s16-negativem Extent divergieren. Nicht
ROOM1030.)*

---

### Glied 5 — Weitere Stempel-Vorbedingungen

Reihenfolge in `FUN_80042bac` pro Record:
```
@0x80042c8c  and v0,v0,a3      ; rec[1] & Pool-Maske (1 Spieler / 2 Gegner / 4 Objekt)
@0x80042ca4  bne v0,s6,...     ; rec[1] & 0x10 muss == a2 sein (AUTO/ACTION)
@0x80042cb4  andi v0,v0,0x80   ; rec[2] Bit 0x80 -> Floor-Prüfung entfällt
@0x80042ccc  bne v1,v0,...     ; sonst rec[2] == entity+0x82
@0x80042ea0  lbu v1,1(s0)      ; Bit 0x40 = CENTRE-Test / Bit 0x20 = FORWARD-Test (620 Einheiten voraus)
```
Ohne Bit 0x40 **und** ohne Bit 0x20 stempelt ein Record **nie**. ROOM1030: Slot 3 = 0x41,
Slots 4/5/6 = 0x42 (alle CENTRE, alle AUTO). Zusätzlich schreibt jeder Treffer den Index auch in
`work_vars[1]` (`@0x80042ee4` → `0x800B0FD2`, CENTRE) bzw. `work_vars[0]` (`@0x80042f3c` →
`0x800B0FD0`, FORWARD).

**Sicherheit:** `BYTE_BELEGT`.
**Port.** Alle Bausteine existieren einzeln, aber nirgends als **ein** Pass. Ein Stempel-Pass
darf **nicht** über `a->type` filtern (Slot 6 hat sce=0 → `RE15_AOT_TYPE_NONE`), sondern muss
wie das Original über `sce_flags & Pool-Maske` gehen.

---

### Glied 6 — Der Trigger ist ein 3-Frame-Handshake mit Drossel

**Original.** sub02 (`@0x21a6..0x21f7`, 82 Byte, selbst dekodiert — geht exakt auf):
```
18 04              Gosub 4                       ; RÜCKWEG-Poller (member15 == 4)
06 00 2e 00        If (len 46)
21 05 14 01          Ck(bank5, bit 0x14, ==1)
46 04 00 00 …          Aot_reset(4, sce=0, flags=0)      ; Zone 4 AUS
46 06 05 42 …          Aot_reset(6, sce=5, flags=0x42)   ; Zone 6 AN
02 00                  Evt_next                          ; YIELD
18 03                  Gosub 3                           ; zählt member15==6 -> work_vars[7]
06 00 0c 00            If (len 12)
23 00 07 03 04 00        Cmp(work_vars[7], op3 '<', 4)   ; DROSSEL
02 00                    Evt_next                        ; YIELD
18 06                    Gosub 6                         ; HINWEG-Poller (member15 == 5)
08 00 08 00        Endif Endif
46 04 04 42 …      Aot_reset(4, sce=4, flags=0x42, payload 5/0x21/1)  ; Zone 4 AN
46 06 00 00 …      Aot_reset(6, sce=0, flags=0)                       ; Zone 6 AUS
02 00              Evt_next
17 ff ff 00 b6 ff  Goto -74 -> 0x21A6
```

**Der Zyklus, mit dem 1-Frame-Stempel-Versatz durchgerechnet:**

| Frame | VM tut | liest Stempel aus Scan von | AOT-Zustand damals |
|---|---|---|---|
| N | Gosub 4 (`==4`), dann Zone4 AUS / Zone6 AN, YIELD | N-1 | Zone4 AN, Zone6 AUS ✓ |
| N+1 | Gosub 3 (zählt `==6`), YIELD | N | Zone4 AUS, Zone6 AN ✓ |
| N+2 | Gosub 6 (`==5`), dann Zone4 AN / Zone6 AUS, YIELD | N+1 | Zone4 AUS, Zone6 AN ✓ |

Das ist **in sich konsistent** und erklärt, warum die 4⊂6-Überdeckung nie beißt.

**Gate-Bits.** Die AOT-sce-4-Handler setzen sie selbst: Slot 4 → `flag(5,0x21)`, Slot 5 →
`flag(5,0x22)` (Payloads `05 00 21 00 01 00` / `05 00 22 00 01 00`). Bank-5-**Wort 1** wird am
Ende jedes VM-Laufs gewischt (`@0x8003ec1c sw zero,4140(at)` = `0x800B102C`) → **Ein-Frame-
Signale**. Bank-5-**Wort 0** Bits 0..19 sind dagegen **persistente Pro-Gegner-Latches**.

**sub06 / sub04 sind exakte Spiegelbilder** (je 20 ausgerollte 32-Byte-Blöcke, selbst dekodiert):

| | sub06 (HINWEG, hinlegen) | sub04 (RÜCKWEG, aufstehen) |
|---|---|---|
| Gate | `21 05 22 01` (Zone 5) | `21 05 21 01` (Zone 4) |
| Pro Slot | `21 05 i 00` (Latch **frei**) | `21 05 i 01` (Latch **gesetzt**) |
| Bind | `2e 02 i` Work_set(kind 2, Slot i) | dito |
| Test | `3e 00 0f 00 05 00` member15 **== 5** | `3e 00 0f 00 04 00` member15 **== 4** |
| Aktion | `18 07` Gosub 7 | `18 05` Gosub 5 |
| Latch | `22 05 i 01` setzen | `22 05 i 00` löschen |

**Sicherheit:** `BYTE_BELEGT`.
**Port.** Der Zyklus ist **fast vollständig da**: Bank-5-Wort-1-Wisch (`scd_vm.c:658`),
Aot_reset als FULL-RETYPE, `flag(5,34)` wird live korrekt gesetzt. Es fehlt **nur der Stempel**.

---

### Glied 7 — sub07 setzt `+0x1C4 |= 0x1000` (und sub05 den Rückweg)

**Original.** ROOM1030 `@0x2754`, 16 Byte, restlos:
```
3d 04 10              Member_get(work_vars[4] <- Member 16)   ; @0x80041238, pc+=3
00                    nop
26 00 05 04 00 10     Calc(op 5 = OR, var 4, 0x1000)          ; @0x8004008c, Op-Tab 0x80010c5c[5]=OR
35 10 04              Member_set2(Member 16 <- work_vars[4])  ; @0x80041108, pc+=3
00 01 00              nop, Evt_end
```
sub05 `@0x24b2` (22 Byte) ist der Rückweg: `wv4 &= 0x0FFF` (Op 6 = AND), dann `wv4 |= 0x2000`.
Member 16 → `entity+0x1C4` (`@0x80041218 sh a2,452(a0)` / `@0x80041444 lhu v0,452(a0)`).

**Sicherheit:** `BYTE_BELEGT` (RDT-Bytes selbst gedumpt, Handler selbst gelesen).
Damit ist der alte Blocker „Trigger nur per Savestate entscheidbar" **erledigt**.

**Port.** Opcode **0x3D ist NICHT registriert** (`grep -c "s_op_table\[0x3D\]"` = **0**; 43 von
~60 Opcodes registriert). Die Längentabelle hat `[0x3D] = 3` — der PC-Vorschub stimmt also, aber
`work_vars[4]` behält seinen **Alt**wert. Konkret: sub03 lässt `work_vars[4]` auf **20** stehen
(seine 20-Iterationen-Schleife inkrementiert ihn). sub07 schriebe dann
`anim_flags = 20 | 0x1000 = 0x1014` — Bit 0x1000 richtig, aber **Bit 0x04 (LOOP) und 0x10
zusätzlich gesetzt**. Das ist Zustandskorruption, kein harmloses No-op.

---

### Glied 8 — Wer `0x1000` konsumiert: drei Steer-Funktionen, **aber nicht gleichrangig**

**Original.** Genau drei Leser in STAGE1, alle identisch gebaut:
```
@0x80101ecc / @0x801021d4 / @0x80105798   andi v0,v0,0x1000
                                          ori v0,zero,0x1001
                                          sw v0,4(v1)        ; +0x4 = 0x1001 -> Sub-Modus 0x10
```
**KORREKTUR einer Lane-Behauptung** („der Test ist überall die letzte und damit höchstpriore
Prüfung"): Das gilt **nur für `FUN_80101de4`**. In `FUN_80102058` springt der Kontakt-Pfad am
Gate **vorbei**:
```
@0x801020ac  j 0x801021e4         ; Ziel = EPILOG
@0x801020b0  sw v0,4(a1)          ; Delay-Slot: Kontakt-Wort 0x901/0xA01
```
`FUN_8010561c` macht dasselbe (`@0x80105670 j 0x801057a8`). **Die Kontakt-Reaktion schlägt also
in Sub-Modus 2 und 19 das Hinlegen.**

Der **Rückweg** ist dagegen wirklich höchstprior: in `FUN_801035f8` fällt der Grab-Write
`@0x8010368c sw v0,4(v1)` **durch** in das 0x2000-Gate `@0x80103690-c4`.

**Sicherheit:** `BYTE_BELEGT` (beide Sprünge selbst gelesen).
**Port.** `enemy_ai_common.c:407` modelliert das bereits **korrekt** (Test im Nicht-Kontakt-Zweig,
0x901/0xa01 im else). **Die Lane-Empfehlung „Test ans Ende verschieben" wäre eine Regression —
nicht befolgen.**

---

### Glied 9 — Sub-Modus 0x10 = der TOGGLE (`FUN_80104f80`)

`f840[0x10] = 0x80104F78` (`jr ra`-Stub, keine Decide-Logik) · `f890[0x10] = 0x80104F80`
· **und `f920[6] = 0x80104F80`** — Hin- und Rückweg teilen sich **eine** Funktion
(Tabellen selbst gedumpt).

```
@0x80104f90  lbu v1,6(a0)              ; Dispatch auf +0x06

PHASE 0 (@0x80104fcc):
  @0x80104fd4  ori v0,v0,0x1 / sb 147   ; +0x93 |= 1
  @0x80104fe4  ori v0,zero,0x12
  @0x80104fec  sb v0,148(v1)            ; +0x94 = 0x12   (Delay-Slot von jal rng)
  @0x80104ffc  sb v0,149(v1)            ; +0x95 = rng & 3   <- ZUFÄLLIGER Startframe
  @0x8010500c  sb a0,6(v0)              ; +0x06 = 1  (a0 = 1)
  @0x8010501c  sb v0,143(v1)            ; +0x8F = 0x0F
  @0x8010502c  sb zero,159(v0)          ; +0x9F = 0
  @0x80105044  andi v0,v0,0x80          ; (+0x09 & 0x80)?
  @0x80105048  bne v0,zero,0x80105054   ; -> beide Wege landen auf 0x80105054
  @0x80105050  sb a0,159(v1)            ; +0x9F = 1 NUR wenn Bit 0x80 CLEAR
  ;; ---- KEIN RETURN: FÄLLT DURCH IN PHASE 1 ----

PHASE 1 (@0x80105054):
  @0x8010506c  jal 0x8001f314           ; anim_set(+0x170, +0x174, (s8)+0x9F, 0x100)
  @0x80105088  addu v1,v1,v0 / sb 6     ; +0x06 += Rückgabe (1 im Wrap-Frame)
  @0x8010508c  j 0x80105114             ; Phase 1 fällt NICHT in Phase 2

PHASE 2 (@0x80105094):
  @0x80105094  sb zero,9(a0)            ; +0x09 = 0
  @0x801050a4  sw v0,4(v1)              ; +0x04 = 0x201   (v0 aus Delay-Slot @0x80104fc0)
  @0x801050b4  sb v0,471(v1)            ; +0x1D7 = 4
  @0x801050cc  beq v0,zero,0x80105100   ; +0x9F == 0 ? -> fertig (AUFSTEHEN)
  @0x801050d4  sb v0,9(v1)              ; +0x09 = 0x81    (v0 aus Delay-Slot @0x801050d0)
  @0x801050e4  sw a1,4(v0)              ; +0x04 = 1       (a1 aus @0x80104f94)
  @0x801050f4  sb v0,471(v1)            ; +0x1D7 = 8      (KRIECHEN)
  @0x80105110  sb v0,147(v1)            ; +0x93 &= 0xFE
```

**Sicherheit:** `BYTE_BELEGT` (komplette Funktion selbst gelesen, Register-Liveness von
`a1`/`v0` über beide Pfade geprüft).

**Zwei Fallen für den Nachbau:**
1. **Phase 0 fällt durch** — Setup UND erster `anim_set` im **selben** Tick, `+0x06` wird
   `1 + Rückgabe`, nicht 1. Wer drei getrennte Phasen baut, verliert einen Frame.
2. Die `sw` auf `+0x04` sind **32-Bit**-Stores: `0x201` setzt `+0x5=2, +0x6=0, +0x7=0`;
   `1` setzt `+0x5=0, +0x6=0, +0x7=0`. Alle vier Bytes müssen geschrieben werden.

**Port.** Die Animate-Kaskade deckt `+0x5` = 0,1,2,…,0x13 ab — **0x10 fehlt**. Die Decide-Seite
(`0x1000 → 0x1001`) ist bei `:333-334` / `:407-408` schon da. **Das heißt: der Port schreibt
heute schon `0x1001`, hat aber keinen Handler dafür — ein Skript, das Bit 0x1000 setzt, friert
den Zombie im AKTUELLEN Build ein.** (Latent, weil 0x3D fehlt.)

---

### Glied 10 — Grid-Wurzel 1 (die Kriech-Maschine)

```
@0x8010164c  lbu v0,9(v0) / andi v0,v0,0xf     ; Grid-Nibble
@0x80101660  addiu at,at,-2036                 ; Tabelle 0x8011f80c
             0x8011f80c[1] = 0x80101708
FUN_80101708: DOPPEL-Dispatch, +0x05 wird ZWEIMAL frisch gelesen
  @0x80101728  addiu at,at,-1824  = 0x8011F8E0  (DECIDE)  jalr
  @0x8010174c  lbu v0,5(v0)                     <- FRISCH: Decide darf +0x05 ändern
  @0x8010175c  addiu at,at,-1760  = 0x8011F920  (ANIMATE) jalr
```

**Tabellen (selbst gedumpt):**

| Index | `0x8011F8E0` DECIDE | `0x8011F920` ANIMATE |
|---|---|---|
| 0 | `0x801035F8` | `0x801036DC` ← Kriech-Lokomotion |
| 1,2 | `0x80103B8C` (Stub) | `0x80103B94` |
| 3,4 | `0x80104540` (Stub) | `0x80104548` |
| 5 | `0x8010466C` | `0x80104808` |
| 6 | `0x80104F78` (Stub) | `0x80104F80` ← **derselbe Toggle** |
| **7..14** | **`0x00000000`** | **`0x00000000`** |
| 15 | `0x80109E44` | `0x80109E4C` |

**HARTE INVARIANTE:** Indizes 7..14 sind **NULL-Pointer** = `jalr 0` = Absturz. Ein Port-`case 1`
muss garantieren, dass `+0x05` in Grid 1 nie 7..14 wird.

**Kriech-Lokomotion `FUN_801036dc`, Erstframe (`+0x06 == 0`):**
```
@0x801036fc  sh v0,140(v1)   ; +0x8C = 0x1E   (HALBWORT!)
@0x8010370c  sb v0,6(v1)     ; +0x06 = 1
@0x8010371c  sb v0,148(v1)   ; +0x94 = 0x1A   Kriech-Clip
@0x8010372c  sb zero,149(v0) ; +0x95 = 0
@0x8010373c  sb v0,143(v1)   ; +0x8F = 0x0F
@0x8010374c  sb v0,471(v1)   ; +0x1D7 = 8     Maske erneut
```

**Sicherheit:** `BYTE_BELEGT`.
**Port.** `enemy_ai_common.c:2549` hat Cases 2,0,5,6,7,8,9,10 und `default: break` (`:2669`) für
1,3,4,11..15. **Ein committeter Kriecher friert ein.** *(Nebenbefund: `0x8011f80c` hat nur 13
eigene Einträge; Nibble 13..15 aliasen physisch in `0x8011f840[0..2]`.)*

---

### Glied 11 — Die SCA-Sperre

**Original.** Maske pro Entity:
```
@0x80100620  lw v0,120(a0)     ; entity+0x78 = Hitbox
@0x80100624  lbu a2,471(a0)    ; entity+0x1D7 = MASKE
@0x80100628  lhu a1,6(v0)      ; Radius (u16-Index 3)
@0x8010062c  jal 0x8003b0a4
@0x80100630  addiu a0,a0,52    ; entity+0x34 = Position
```
Test in `FUN_8003b0a4`:
```
@0x8003b244  lhu v0,-2(s2)     ; u16 bei Zelle+8
@0x8003b248  lbu v1,24(sp)     ; die gesicherte Maske
@0x8003b250  sra v0,v0,24      ; = vorzeichenerweitertes Byte Zelle+9 (u0)
@0x8003b254  and v1,v1,v0
@0x8003b258  bne v1,zero,0x8003b2e0   ; != 0  ->  SOLID / Push-out
```
`Sca_id_set` (0x37) schreibt genau dieses Byte:
```
@0x80041774  addiu v1,v1,1     ; Regions-Index +1 (!)
@0x80041780  sll v0,a1,1 / addu / sll 2   ; Stride 12
@0x8004179c  sb a1,9(v0)       ; Zelle+9 = pc[3]
```
ROOM1030 `@0x2000/0x2004` (sub00) und `@0x278e/0x2792` (sub08): `37 02 06 f7` / `37 03 06 f7`.
`0xF7 = 0xFF & ~0x08` → Maske 1 (Spieler) **solid**, Maske 4 (aufrecht) **solid**,
Maske 8 (kriechend) → **0 = frei**.

**Sicherheit:** `BYTE_BELEGT`.
**Vokabular game-weit:** Spieler = 1 (`@0x80031d74`, `@0x800384c8` hart), Standardgegner = 4
(hart), und **genau zwei** Aufrufer lesen `+0x1D7`: `@0x80100624` (Zombie-Root) und
`@0x8010aac8` (Zombie-Girl-Root).

**Port.** `re15_collision.c:617` übergibt hart `4u`. Der Test selbst (`:567
(mask & e->u0)`) und `op_sca_id_set` (`scd_vm.c:3707`) sind byte-true. **Nur die Maskenquelle
fehlt.** Solange `4u` steht, ist der Zwischenzustand **sicher** (optisch falsch, aber kein
Durch-die-Wand-Bug) — deshalb darf dieser Baustein früh rein.

---

### Glied 12 — Typ → Root (der Ring schließt sich)

```
@0x8011e86c  addiu v0,v0,1060      ; v0 = 0x80100424
@0x8011e8a4  sw v0,11268(at)       ; 0x80072c04 = Tabelle 0x80072bac + 4*0x16
```
ROOM1030 spawnt **20× Typ 0x16, Behavior 0x0D** (selbst enumeriert, `@0x1de6..0x1f62`, alle
Seed-Halbworte 0). Spawn-Cap `24 12 06 00` @0x1de2 = `work_vars[0x12] = 6`.

→ **Typ 0x16 läuft nachweislich auf `FUN_80100424`**, dem Root, der `+0x1D7` liest.
Die konkurrierende Root-Hypothese (`FUN_8011d6d4`) ist damit **erledigt**.

**Sicherheit:** `BYTE_BELEGT`.

---

## 2. FELD-BELEGUNGS-TABELLE

| Offset | Original-Bedeutung (Beleg) | Port-Feld | Belegt? | Ausweg |
|---|---|---|---|---|
| `+0x04` (Wort) | State-Wort; 32-Bit-Store nullt +0x5/+0x6/+0x7 (`@0x801050a4`, `@0x801050e4`) | `state` + 3 | frei | `re15_ai_set_state_word()` existiert (`:56`) |
| `+0x05` | Sub-Modus (Dispatch-Index) | `sub_state_1` | frei | — |
| `+0x06` | Toggle-Phase 0/1/2 (`@0x80104f90`) | `sub_state_2` | frei | — |
| `+0x09` | Grid-Nibble (&0x0F) + **Bit 0x80 = Kriecher-Latch** (`@0x801050d4` = 0x81) | `grid_id` | ⚠️ **Bit 0x80 = „DOWNED", 16 Lesestellen** | **§3 BLOCKER A** — Audit nötig, kein trivialer Ausweg |
| `+0x0A` | Member 14; per-Frame `0xFF` (`@0x8003ec4c`) | `member_0a` | Kommentar „no consumer yet" **falsch** (`:8441` schreibt 0x19) | Kommentar korrigieren |
| `+0x0B` | Member 15 = **AOT-Stempel** (`@0x80041438` lbu / `@0x80041210` sb) | `member_0b` | ⚠️ **Adult-Spider-LOS-Latch** (`:7062/63`, `:7278/79`) | **Einseitig:** Member 15 ⇔ +0x0B ist byte-belegt → **die Spinne muss umziehen** |
| `+0x78` | Hitbox-Zeiger; Radius = u16-Index 3 (`@0x80100628`) | `hit_radius_min` (=400) | korrekt | — |
| `+0x82` | Floor/Band (Stempel-Gate `@0x80042cb4`) | Band aus Y abgeleitet | ok | — |
| `+0x8C` | Kriech-Timer 0x1E, **Halbwort** (`@0x801036fc sh`) | `ai_timer` | frei | Breite beachten |
| `+0x8F` | Blend-Countdown 0x0F, sättigend (`@0x8001f5b4`) | `anim_frac` | frei | — |
| `+0x93` | Bit 0 Busy-Gate (`@0x80104fd4` / `@0x8010510c`) | `hit_react` | frei | **Nicht** als No-op behandeln: Konsumenten existieren in der EXE (u.a. `@0x80012c30`, `@0x80043380`) |
| `+0x94` | Clip: 0x12 Übergang / 0x1A Kriechen | `motion` | frei | — |
| `+0x95` | Anim-Frame; Start = `rng & 3` | `anim_frame` | frei | Zufalls-Start **nicht** auf 0 vereinfachen |
| `+0x9F` | Richtungs-Latch des Toggles (`@0x80105050` / `@0x801050c4`) | — | ⚠️ `dog_aux9f` + `neck_speed`-Highbyte | **Neues eigenes Feld** (`xfer_dir`). **Nicht** über `anim_flags` Bit 0x80 routen |
| `+0x1C4` | Member 16 = anim_flags; 0x1000 rein / 0x2000 raus (`@0x80041218`) | `anim_flags` | Bits 0x1000/0x2000 **frei** | ⚠️ Port-Bit **0x80** (reverse) ist **port-erfunden** — im Original wird 0x80 auf +0x1C4 **nirgends** getestet |
| `+0x1D0` | **typ-überladen**: Zombie = u32-Distanz (`@0x801004c0 sw`); Hund/Spinne = Halbwort-Bitfeld, Bit 0 = LOS (`@0x8010dc18`, `@0x80110e9c`) | `ai_dist` | Zombie korrekt | Spinne/Hund brauchen ein **eigenes** Bitfeld-Feld |
| `+0x1D7` | SCA-Kollisionsmaske: 4 aufrecht / 8 kriechend (`@0x801050b4` / `@0x801050f4`) | — | ⚠️ `crow_bank` (Krähe, aktiv) | **Neues eigenes Feld** (`sca_mask`), Default 4 |
| `+0x1D8` | Bit 0x80 = permanenter Kriecher (sperrt Aufstehen, `@0x801036b8`) | LOS/Sensor-Bits | Bit 0x80 frei | Im Aufsteh-Gate mitführen |

### Wichtige Richtigstellung zur Port-Doku

`enemy_ai_common.c:7062` kommentiert `„+0x1d0 bit0 LATCHED on verdict tick @0x80110e70-bc"`.
Eine Prüfer-Stimme hat das als „Zitat enthält die Instruktion gar nicht" verworfen — **das war
ein Fehler des Prüfers** (er hat `STAGE1.BIN` statt `STAGE2.BIN` disassembliert; beide Overlays
laden auf `0x80100000`). Ich habe nachgesehen:

```
STAGE2.BIN @0x80110e90  lhu v0,464(v1)     ; +0x1D0
           @0x80110e98  andi v0,v0,0xfffe
           @0x80110e9c  sh v0,464(v1)      ; Bit 0 löschen  -> das Zitat stimmt
STAGE1.BIN @0x80110e78  lbu v0,5(v0)       ; ganz andere Funktion
```

**→ Der Kommentar ist inhaltlich RICHTIG. Der Defekt ist nur, dass der Port den Wert in
`member_0b` statt in einem `+0x1d0`-Feld ablegt. Kommentar nicht „reparieren".**

---

## 3. BIN ICH EINBAU-SICHER?

# **NEIN.**

Der **Mechanismus** ist zu 100 % byte-belegt — jedes Kettenglied, jede Konstante, jede
Feldzuordnung. Was **nicht** sicher ist, sind drei Dinge, die alle **nicht** den Mechanismus
betreffen, sondern die **Einbettung in den bestehenden Port**:

### BLOCKER A — `grid_id` Bit 0x80 ist doppelt belegt *(der gefährlichste)*

Im Original setzt der Kriech-Commit `+0x09 = 0x81` — Nibble 1 **plus Bit 0x80**. Der Port
benutzt Bit 0x80 bereits als **„DOWNED"** mit **16 Lesestellen**
(`enemy_ai_common.c:1035, 1408, 1602, 1659, 2683, 2769, 3029, 3092, 3114, 3126, 3145, 3487,
5439, 5575, 6866, 6903`).

Im Original ist das **physisch dasselbe Bit** — d. h. entweder teilt das Original die Semantik
bewusst („liegend" == „kriechend"), oder eine der 16 Port-Lesestellen wird falsch, sobald das
Bit auch „kriecht" bedeutet. **Das ist nicht geklärt.** Es ist eine **Audit**-Aufgabe (16
Stellen gegen ihre Original-Gegenstücke prüfen), keine Messung — und sie ist **nicht gemacht**.

**Solange das offen ist, darf nichts `grid_id = 0x81` schreiben.**

### BLOCKER B — Die Kriech-Geschwindigkeit ist nicht byte-true

`FUN_801036dc` bewegt den Kriecher über Root-Motion (`FUN_80109470`), nicht über eine
Geschwindigkeit:
```
@0x80109504  lw v1,36(sp)      ; MATRIX.t[0]  (Matrix aus 4 verketteten jal 0x80022da0)
@0x80109508  lw a0,84(s0)      ; s0 = *(entity+0x188); Referenz pool[+0x54]
@0x8010950c  lw v0,52(a1)      ; entity+0x34
@0x80109510  subu v1,v1,a0
@0x80109514  subu v0,v0,v1
@0x80109518  sw v0,52(a1)      ; entity+0x34 -= (MATRIX.t[0] - pool[+0x54])
             ... analog +0x3C  mit MATRIX.t[2] / pool[+0x5C]
```
Der Port hat `re15_enemy_footlock_step` (`enemy_ai_common.c:2195`), aber das ist ein **anderer
Algorithmus**: Pose-Query eines einzelnen Fuß-Bones, Delta gegen einen **selbst gecachten**
Vorframe-Wert (`s_zfoot_ref`), Auswahl über EDD-Bit `0x2000`. Das Original nutzt eine
**Bone-Ketten-Matrix** gegen eine im Modell-Pool gespeicherte Referenz.

Die beiden sind **nicht** byte-verglichen. Der Kriecher würde die (bereits ausgelieferte,
aber unverifizierte) Ungenauigkeit des Geh-Footlocks erben. Das ist kein *neuer* Rateschritt —
aber es ist auch **kein byte-true**.

Dazu: dass `entity+0x174` physisch die 43-Clip-Bank ist, in der Clip 0x12 (98 Frames) und
Clip 0x1A (99 Frames) liegen, ist ein **Datenargument**, kein Instruktionsbeleg — der
EMD→Entity-Binder wurde von keiner Lane gefunden. `HERGELEITET`.

### BLOCKER C — Zwei Regressionsflächen sind unvermessen

1. **Opcode 0x3D registrieren** ändert Verhalten überall, wo 0x3D instruktionsausgerichtet
   vorkommt. **Kein Census gefahren.** (Warnung: Offline-SCD-Walker desynchronisieren in
   Datenregionen — der Census darf nur über die Sub-Pointer-Tabelle laufen.)
2. **Der Objekt-Notch-Clear.** Das Original nullt `obj+0x0B` **jeden Frame unbedingt**
   (`@0x80043788`) und wischt vorher `0xFF` drüber (`@0x8003eccc`). Der Port verlässt sich
   ausdrücklich auf **Klebrigkeit** (`aot_common.c:314-317`, `if (notch >= 0)`), und
   `integration_keypad` (`test_keypad.c:153-159`) gatet genau darauf. Baut man den Clear
   byte-true ein, kann die Keypad-Bestätigung brechen. **Nicht gemessen.**

### Was NICHT mehr blockiert *(alle in dieser Runde erledigt)*

| Erledigt | Warum |
|---|---|
| Trigger-Herkunft `0x1000` | sub07 Opcode 0x26 OR 0x1000 — statisch, kein Savestate |
| Typ→Root-Bindung | `@0x8011e8a4` → Typ 0x16 → `0x80100424` |
| Rect-Test-Offenheit | geschlossenes Intervall; ROOM1030-Extents gerade → Port äquivalent |
| Pool-Kapazität 20/24/35 | Kategorienverwechslung; AOT-Pool = 32 (`@0x8003edd0 sltiu v0,a0,0x20`) |
| „`member_0b` ist im ganzen Spiel belegt" | Nur **zwei** Stellen, beide Adult-Spider |
| „Port-Kommentar `@0x80110e70-bc` ist erfunden" | Prüfer-Irrtum (falsches Overlay) — Kommentar ist korrekt |

---

## 4. EINBAU-REIHENFOLGE

Grundregel: **Schritte 1–3 sind per Konstruktion verhaltensneutral** — sie sind unerreichbar,
solange nichts `+0x05 = 0x10` bzw. `grid&0xf = 1` setzt. Der Zustand wird erst mit Schritt 5
lebendig. Regressions-Gate nach **jedem** Schritt: `ctest --test-dir re15_port/build --timeout 30`
→ **116/116 grün**.

### Schritt 0 — Vorarbeit (kein Code-Verhalten)
* `re15_actor.h:49-51` Kommentare korrigieren (`member_0a`/`member_0b` „no consumer yet" ist
  nachweislich falsch).
* `aot_common.c:314-317` Kommentar korrigieren (Objekt behält den Notch **nicht**; falsches
  Zitat `@0x80042f5c` → richtig ist der AUTO-Pfad `@0x80042fc4` + Clear `@0x80043788`).
* `enemy_ai_common.c:7062` Kommentar **nicht** anfassen (er ist richtig).
* **Gate:** 116/116, keine Verhaltensänderung.

### Schritt 1 — SCA-Maske per Entity
Neues Feld `uint8_t sca_mask; /* +0x1d7 @0x801050b4/@0x801050f4 */`. **Nicht** `crow_bank`.
Default 4 beim Spawn (`@0x80100828`). `re15_collision_constrain_enemy` um einen Masken-Parameter
erweitern; **nur** die zwei Aufrufer `enemy_ai_common.c:8975` (Zombie) und `:9120` (ZGirl)
übergeben `e->sca_mask`, alle anderen weiterhin `4u` (byte-true: 11 der 13 STAGE1-Aufrufer sind
hart 4).
* **Zwischenstand:** alle Gegner behalten Maske 4 → identisches Verhalten.
* **Gate:** 116/116, insbesondere `unit_collision_layers`.

### Schritt 2 — `member_0b` entkoppeln
Adult-Spider-LOS in ein **eigenes** Feld (`uint16_t aspider_los;` als `+0x1D0`-Bitfeld, plus
optional `+0x1E0` für den Rohwert). `enemy_ai_common.c:7062/63` und `:7278/79` darauf umbiegen.
`member_0b` bleibt danach **exklusiv** für Member 15 / `+0x0B` reserviert.
* **Zwischenstand:** Spinnen-Sichtlinie unverändert, `member_0b` frei.
* **Gate:** 116/116, insbesondere `unit_adult_spider_ai`.

### Schritt 3 — Grid-Wurzel 1 + Sub-Modus 0x10 *(nur nach BLOCKER-A-Audit)*
`case 1:` mit Doppel-Dispatch (DECIDE `0x8011F8E0[+0x05]`, dann `+0x05` **frisch** lesen,
ANIMATE `0x8011F920[+0x05]`), mindestens Index 0 (`0x801035F8` / `0x801036DC`) und Index 6
(Stub / `0x80104F80`). Plus `f890[0x10]`-Animate = dieselbe Funktion. **Assert**, dass `+0x05`
in Grid 1 nie 7..14 wird (NULL-Pointer). **Hin- UND Rückweg zusammen** — nur der Hinweg lässt
den Zombie dauerhaft mit Maske 8 laufen (= durch **jede** Wand mit u0-Bit 3).
* **Zwischenstand:** unerreichbar → Verhalten identisch.
* **Gate:** 116/116 + neue Unit-Sonde, die einen Aktor künstlich auf `grid_id=0x81` setzt und
  60 Ticks fährt (erwartet `motion==0x1A`, `sca_mask==8`, `ai_timer==0x1E`).

### Schritt 4 — `+0x93`, `+0x9F`, `+0x1D8` Bit 0x80
Eigenes `xfer_dir`-Feld für `+0x9F`. `+0x93` Bit 0 byte-true setzen/löschen. `+0x1D8` Bit 0x80
als Aufsteh-Sperre im 0x2000-Gate.
* **Gate:** 116/116.

### Schritt 5 — Opcode 0x3D **+** AOT-Stempel-Pass *(müssen ZUSAMMEN rein)*
0x3D allein ist folgenlos; der Stempel allein ruft sub07 mit einem **stale** `work_vars[4]` (=20)
auf und schriebe `0x1014`. Der Stempel-Pass muss:
1. Spieler: **kein** Clear · Gegner: Clear nur bei `word0 & 1` · Objekte: Clear **unbedingt**
2. vorher (VM-Ende) `+0x0A/+0x0B = 0xFF` — Spieler unbedingt, aktive Gegner/Objekte gegated
3. Slots **aufsteigend**, **kein** `break` → LAST-WINS
4. Filter: `sce_flags & Pool-Maske`, `sce_flags & 0x10 == kind`, Floor `rec[2]` vs `+0x82`,
   Geometrie `0x40` (CENTRE) / `0x20` (FORWARD, 620 voraus) — **nicht** über `a->type`
5. nur Slots **0..31** (RVD-Kamerazonen ab Slot 63 ausschließen)
6. **als LETZTER** Zustands-Tick des Frames
* **Gate:** 116/116 + `integration_keypad` **muss** grün bleiben (BLOCKER C.2) + `probe_gate_1030`
  zeigt `m0b=5` für den Gegner in Zone 5.

---

## 5. MESS-BITTE AN DEN NUTZER

Der **Mechanismus** braucht keine Messung mehr. Was eine Messung braucht, ist die **Einbettung**.
Vier ROOM1030-Savestates plus einer für die Keypad-Frage.

⚠️ `stage_saves/PATCHED-EXE_*.sav` sind als Referenz **unzulässig**.

**Adressen** (alle byte-belegt hergeleitet):

| Größe | Adresse |
|---|---|
| Gegner N | `0x800ACC2C + 500*N` |
| AOT-Zeiger-Array | `0x800AC9B0 + 4*id` (32 Slots) |
| AOT-Anzahl | `0x800AFBB4` |
| work_vars[k] | `0x800B0FD0 + 2*k` |
| Flag-Bank 5 | Wort 0 `0x800B1028`, Wort 1 `0x800B102C` |
| SCA-Zelle | `*(0x800AC778) + 0x20` → Eintrag `[region+1]` → `+ 12*index` → Byte `+9` |

### Savestate A — Zombie **vor** dem Tor (steht, kriecht noch nicht)
Lies für N=0..19 den Hexdump `0x00..0x1E0`.
**Entscheidet:** den Stempelwert. Erwartung: der Gegner in `x[-12900,-3700] z[-25300,-24500]`
hat `+0x0B == 5`; im 300er-Überlappungsstreifen `z[-24500,-24200]` hat er `+0x0B == 6`;
außerhalb `0`. → beweist LAST-WINS und den Stempel in einem.

### Savestate B — Zombie **mitten** in der Kriech-Animation
Felder: `+0x04, +0x05, +0x06, +0x09, +0x8C, +0x8F, +0x93, +0x94, +0x95, +0x9F, +0x1C4, +0x1D7,
+0x1D8`.
**Entscheidet BLOCKER A:** Ist `+0x09 == 0x81`? Dann teilt das Original „liegend" und „kriechend"
tatsächlich dasselbe Bit — und die 16 Port-Lesestellen müssen dagegen auditiert werden.
Erwartung sonst: `+0x94 == 0x1A`, `+0x1D7 == 8`, `+0x8C == 0x1E`.

### Savestate C — Zwei aufeinanderfolgende Frames **während** des Kriechens
Nur `+0x34` (X), `+0x3C` (Z), `+0x94`, `+0x95` — beide Frames.
**Entscheidet BLOCKER B:** das Positions-Delta pro Frame bei bekanntem Anim-Frame ist die einzige
byte-true Referenz für die Kriech-Geschwindigkeit. **Ohne diese zwei Frames bleibt die
Geschwindigkeit geraten.** Ideal: 5–10 aufeinanderfolgende Frames.

### Savestate D — Zombie **durch** und wieder aufrecht
Erwartung: `+0x1C4 == (alt & 0x0FFF) | 0x2000`, `+0x05 == 6` → dann `+0x09` ohne Bit 0x80,
`+0x1D7 == 4`. **Entscheidet den Rückweg.**

### Savestate E — Keypad-Raum **ROOM1230**, Cursor **zwischen** zwei Ziffernzellen
Lies `0x800B3F98 + 148*N`, Byte `+0x0A` und `+0x0B`.
**Entscheidet BLOCKER C.2:** Ist `+0x0B == 0` (bzw. `+0x0A == 0xFF`), dann ist der byte-true
Clear korrekt und der Port-Test `integration_keypad` gatet auf ein **falsches** Verhalten —
dann muss der Test angepasst werden, nicht der Clear weggelassen.

### Bonus, kostenlos in A/B mitzunehmen
* SCA-Zelle Region 2 **und** 3, Index 6, Byte `+9` → erwartet `0xF7` (Tor offen) bzw. `0xFF`.
* `0x800B1028` Bits 0..19 (Pro-Gegner-Latches) und `0x800B102C` Bit `0x20000000`.
* AOT-Zeiger-Array `0x800AC9B0[0..17]` in **B** → macht die sub02-Wechselschaltung von Slot 4/6
  sichtbar statt hergeleitet.

---

## 6. Offene Punkte (bewusst nicht geschlossen)

* Wer setzt `+0x1D8` Bit 0x80 in ROOM1030? Nicht gefunden → das Aufsteh-Gate ist im Port als
  „immer offen" zu behandeln, und **das ist eine ungeprüfte Annahme**.
* Erreichbarkeit von Grid-1 Sub-State 15 (`0x80109E4C`). Kein literaler `+0x05 = 0x0F`-Schreiber
  in STAGE1. Die dortige RNG-Ziehung `@0x8010a188` würde den deterministischen RNG-Strom
  verschieben → **nicht einbauen**, solange offen.
* Grid-Wurzel 3 (`0x80101800`) teilt die ANIMATE-Tabelle mit Wurzel 1, hat aber eine eigene
  DECIDE-Tabelle mit nur 6 eigenen Worten (Index 6 aliast in Wurzel 4). **Nicht** aus einer
  gemeinsamen Implementierung bauen.
* `Work_set(kind 2)` bindet im Original **bedingungslos** (`@0x80040f00`), der Port gatet auf
  `active` (`scd_vm.c:2530-2538`). Da der Clear ebenfalls `active`-gegated ist, kann das Original
  an einem **stale** `+0x0B` eines nie gespawnten Slots hängenbleiben (ROOM1030: Slots 6..19).
  Vor Schritt 5 harmlos, danach nicht mehr automatisch.
* Kein einziger Wert der Kette ist bisher an einem **Original**-Savestate gemessen. Alle
  Feldzuordnungen sind instruktions-hergeleitet.

---

## 10. HARDWARE-VERIFIKATION am ORIGINAL (2026-08-08, PCSX-Redux)

Der letzte Punkt oben — „kein einziger Wert der Kette ist bisher an einem **Original**-Savestate
gemessen" — ist damit teilweise erledigt. Zugang: Debug-Menue-Sprung Stage 0 / Index 0x03,
autonom gefahren (`tools/redux/jump_menu.lua`). Zwei Werkzeug-Defekte mussten dafuer erst weg:
`return false` im Haltepunkt-Rueckruf LOESCHT den Haltepunkt (gemessen 1 vs. 2075 Treffer), und
bestaetigt wird im Debug-Menue mit **QUADRAT** (`@0x80014a38 andi v0,v0,0x80`), nicht mit KREUZ.

### Bestaetigt

| Behauptung im Dossier | Messung am Original | Status |
|---|---|---|
| Spawn-Cap `24 12 06 00` @0x1de2 = `work_vars[0x12] = 6` | **genau 6** belegte Gegner-Slots, durchgaengig ueber ~10000 Bilder | ✅ |
| ROOM1030 spawnt Typ **0x16** (`@0x8011e8a0-a4 sw v0,11268(at)` → `0x80072c04`) | alle 6 Slots tragen `+0x08 = 0x16` | ✅ |
| Entity-Tabelle `0x800acc2c + slot*0x1F4` | Slots 0..5 zusammenhaengend belegt, Felder plausibel | ✅ |

Damit ist die Nutzer-Beobachtung „im Original sind nur 6 Zombies sichtbar" **hardware-belegt** —
und der bereits eingebaute Cap trifft die richtige Zahl.

### Noch NICHT bestaetigt — und eine Selbstkorrektur

Bei stillstehendem Spieler (nur Raumsprung, keine Bewegung) traten ueber ~10000 Bilder **nur**
die Clips `0x02`, `0x05` und `0x27` auf. **Weder `0x12` (Uebergang) noch `0x1A` (Kriechen)**.
Die Trigger-Kette lief also gar nicht an — erwartbar, denn der Zonen-Handshake (Glied 6) braucht
einen Spieler, der sich bewegt.

⚠️ Zwei Messfehler meinerseits, die hier festgehalten gehoeren:
1. Ich habe zuerst nur nach `+0x94 == 0x12` gesucht. Laut Tabelle in §? ist `0x12` aber nur der
   **Uebergang**, das eigentliche Kriechen ist **`0x1A`**.
2. Abtastung alle 300 Bilder ist zu grob: Slot 0 sprang zwischen zwei Stichproben von
   `z=-27878` auf `z=-25004` — genau die Strecke unter dem Tor. Der Vorgang lag ZWISCHEN den
   Abtastpunkten. Messung muss ereignisgesteuert sein (`tools/redux/crawl_watch.lua` schreibt bei
   jeder Aenderung von `+0x94`).

### Beobachtung, die eine Feldzuordnung in Frage stellt

`+0x1C4` stand pro Slot auf `0x00000000, 0x00010000, 0x00020000, … 0x00050000` — also
**Slot-Index im oberen Halbwort**. Das Freigabe-Bit `0x1000` aus Glied 7/8 muesste im UNTEREN
Halbwort liegen; dort stand durchgaengig 0. Entweder ist `+0x1C4` nicht das Wort aus Glied 7/8,
oder es traegt zusaetzlich eine Slot-ID. **Vor dem Einbau von Glied 7/8 klaeren** — eine
Feldzuordnung, die im Original nachweislich anders belegt ist, darf nicht in den Port.
