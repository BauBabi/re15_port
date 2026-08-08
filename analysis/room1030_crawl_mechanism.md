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

---

## 11. WARUM DER DEBUG-SPRUNG DAS KRIECHEN NIE ZEIGT (gemessen 2026-08-08)

Drei aufeinander aufbauende Messungen am Original (PCSX-Redux, `tools/redux/crawl_watch.lua`,
ereignisgesteuert auf jede Aenderung von `+0x94` und `+0x0B`, Flag-Bank 5 in JEDEM Bild
ODER-akkumuliert):

| Schritt | Messung | Folgerung |
|---|---|---|
| Spieler steht still | 6 Zombies Typ 0x16 bei z≈-24838, `+0x0B` = **0x05** bei 5 von 6 | Zone-Geometrie + Glied 1 bestaetigt: sie stehen IN der HINWEG-Zone |
| dito, 8000 Bilder | `1C4lo` durchgehend `0000`, Clips nur 0x02/0x05/0x27 | Kette bricht zwischen „Stempel==5" und `Gosub 7` ab |
| Spieler in die Zone versetzt (z=-24300) | Zustaende 1/4/1 → 1/6/1 → 1/12/1, Clips 0x03/0x04/0x0a/0x29 | die KI lebt und greift an — es ist NICHT allgemeine Untaetigkeit |
| ueber ALLE Bilder | **Flag5 ODER W0 = `00000000`**, W1 ODER = `20000000` | `flag(5,0x21)` und `flag(5,0x22)` werden NIE gesetzt |

### Der eigentliche Grund

sub02 (Glied 6) beginnt mit
```
18 04              Gosub 4
06 00 2e 00        If (len 46)
21 05 14 01          Ck(bank5, bit 0x14, ==1)     <-- GATE des GANZEN Zyklus
```
`flag(5,0x14)` ist Bit 20 → Bank-5-**Wort 0**, Maske `0x00100000` (Adresse `0x800B1028`).
Gemessen war Wort 0 ueber den gesamten Lauf **durchgehend 0**. Der Zyklus, der Zone 4/6 umschaltet
und ueber `Gosub 7` das Freigabe-Bit setzt, **lief also kein einziges Mal**.

⛔ **Konsequenz fuer die Methodik:** Ein Debug-Menue-Sprung nach ROOM1030 reproduziert das
Kriechen grundsaetzlich NICHT — er setzt `flag(5,0x14)` nicht. Wer den Mechanismus am Original
sehen will, muss den Raum auf dem regulaeren Weg betreten (Story-Zustand), oder `flag(5,0x14)`
gezielt setzen. Das erklaert rueckwirkend, warum drei Messlaeufe „nichts passiert" ergaben,
obwohl Zone-Stempel und KI nachweislich funktionieren.

### Naechster Schritt (nicht geraten, sondern benannt)

**Wer setzt `flag(5,0x14)`?** Das ist genau die Frage, fuer die dieses Werkzeug gebaut ist: ein
SCHREIB-Haltepunkt auf `0x800B1028` mit Maske `0x00100000` liefert den literalen PC. Erst danach
ist entscheidbar, ob der Port diese Vorbedingung ueberhaupt herstellt — und ohne sie ist jeder
Einbau von Glied 6-8 blind.

---

## 12. DIE VOLLSTAENDIGE VORBEDINGUNGS-KETTE (statisch aufgeloest, 2026-08-08)

Nach dem Messbefund aus §11 („`flag(5,0x14)` wird nie gesetzt") habe ich die Setzer statisch
gesucht — Rohbytes im ausgelieferten RDT, kein Decompilat.

**`Set(bank5,0x14,1)` = `22 05 14 01`** — 2 Vorkommen in `ROOM1030.RDT` (@0x2008, @0x27da).
Der erste steht in einem `If`-Block mit genau einem Gatter:
```
06 00 1e 00     If (len 30)
21 04 0f 01       Ck(bank4, bit 0x0f, ==1)     <-- Gatter
4b 00 09 / 4b 03 0a / 4b 04 0b / 4b 06 0c
37 02 06 f7 / 37 03 06 f7
22 05 14 01       Set(bank5, 0x14, 1)
08 00           Endif
```

**`Set(bank4,0x0f,1)` = `22 04 0f 01`** — in GANZ STAGE1 nur **zwei** Vorkommen:
`ROOM1030.RDT @0x2198` und sein Spiegel `ROOM1031.RDT @0x2284`. Kein einziges `Set(bank4,0x0f,0)`.
Der Setzer liegt unmittelbar vor sub02 (@0x21a6) und ist doppelt gegatet:
```
0x2180  06 00 20 00      If (len 32)
        21 03 74 01        Ck(bank3, 0x74, ==1)     <-- STORY-Flag
        21 04 0f 00        Ck(bank4, 0x0f, ==0)     <-- Einmal-Schutz
        21 05 20 01        Ck(bank5, 0x20, ==1)
0x2190  06 00 0e 00        If (len 14)
        21 05 22 01          Ck(bank5, 0x22, ==1)   <-- Ein-Bild-Signal, AOT-Slot 5
        22 04 0f 01          Set(bank4, 0x0f, 1)
        04 ff 18 08          Evt_exec/Gosub 8
0x21a0  08 00 08 00      Endif Endif
        01 00            Evt_end
        18 04            Gosub 4                    <-- sub02 (Glied 6) beginnt hier
```

### Die Kette von oben nach unten

```
flag(3,0x74) [STORY]  UND  flag(5,0x20)  UND  flag(5,0x22) [Spieler in AOT-Zone 5]
      -> Set flag(4,0x0f)            @0x2198
      -> Set flag(5,0x14)            @0x2008   (gegatet auf flag(4,0x0f))
      -> sub02 laeuft                @0x21a6   (gegatet auf flag(5,0x14))
      -> Zone-4/6-Handshake, Gosub 7 -> +0x1C4 |= 0x1000
      -> Steer-Funktionen -> Zombie legt sich hin und kriecht (+0x94 0x12 -> 0x1A)
```

**Damit ist der Messbefund vollstaendig erklaert.** Gemessen war Bank-5-Wort 0 durchgehend `0`
und Wort 1 ODER nur `0x20000000` — also weder `flag(5,0x20)` (Bit 32 = Wort 1 Maske `0x1`) noch
`flag(5,0x22)` (Bit 34 = Maske `0x4`) je gesetzt, und `flag(5,0x14)` (Wort 0 Maske `0x00100000`)
folglich auch nicht. Ein Debug-Sprung bringt weder den Story-Zustand `flag(3,0x74)` mit noch
laeuft das Vorspiel, das `flag(5,0x20)` setzt.

### Was das fuer den Einbau bedeutet

Die Port-Seite muss **vier** Dinge koennen, bevor Glied 6-8 ueberhaupt sinnvoll sind:
1. `flag(3,0x74)` zum richtigen Zeitpunkt setzen (Story),
2. `flag(5,0x20)` setzen,
3. den AOT-sce-4-Handler von Slot 5 `flag(5,0x22)` als **Ein-Bild-Signal** setzen lassen,
4. den Aktor-Stempel `entity+0x0B` liefern (Glied 1 — im Port bis heute nicht vorhanden).

Punkt 4 war schon bekannt. Punkte 1-3 sind **neu** und waren in der bisherigen Fassung dieses
Dossiers nicht als Vorbedingung benannt — der Zyklus wurde dort ab sub02 beschrieben, als liefe er
von selbst.

---

## 13. ⛔ KORREKTUR zu §11/§12 — Flag-Bits sind MSB-ZUERST nummeriert

In §11 steht „weder `flag(5,0x21)` noch `flag(5,0x22)` wird je gesetzt". **Das ist falsch.** Ich
hatte die Bit-Nummer LSB-zuerst gerechnet. Der generische Flag-Helfer rechnet anders:

```
8003fdd4  lui   t0,0x8000        ; t0 = 0x80000000
8003fdd8  lbu   v1,0x1(v0)       ; bank
8003fddc  lbu   a1,0x2(v0)       ; bit
8003fdec  sll   v1,v1,0x2        ; bank*4 = Tabellenindex
8003fdf0  sra   v0,a1,0x5        ; Wort  = bit >> 5
8003fdf4  sll   v0,v0,0x2        ; *4
8003fdfc  addiu at,at,0x4664     ; PTR_DAT_80074664 = Bank-Basistabelle
8003fe04  lw    v1,0x0(at)       ; bank_base
8003fe0c  addu  a3,v0,v1         ; a3 = bank_base + (bit>>5)*4
8003fe18  andi  a1,a1,0x1f       ; bit & 31
8003fe68  srlv  v1,t0,a1         ; MASKE = 0x80000000 >> (bit & 31)   <-- MSB-ZUERST
8003fe74  sw    v1,0x0(a3)       ; SET-Pfad
```

**Regel:** `wort = bank_base + (bit>>5)*4`, `maske = 0x80000000 >> (bit & 31)`.
Bit 0 ist also `0x80000000`, Bit 31 ist `0x00000001`.

**Bank-Basistabelle @0x80074664** (selbst ausgelesen):

| Bank | Basis | | Bank | Basis |
|---|---|---|---|---|
| 0 | `0x800ACA38` | | 6 | `0x800B1030` |
| 1 | `0x800ACA3C` | | 7 | `0x800B1038` |
| 2 | `0x800ACA40` | | 8 | `0x800B1058` |
| 3 | `0x800B0FF8` | | 9 | `0x800B1078` |
| 4 | `0x800B1018` | | 10 | `0x800B1098` |
| 5 | `0x800B1028` | | 11 | `0x800ACA44` |

(Bank 5 = `0x800B1028` deckt sich mit der Messung: Wort 0 dort, Wort 1 bei `0x800B102C`.)

### Die Messung neu ausgewertet

| Flag | Bit | Wort | Maske | gemessen |
|---|---|---|---|---|
| `flag(5,0x14)` | 20 | 0 @`0x800B1028` | `0x00000800` | W0 ODER = 0 → **nicht gesetzt** |
| `flag(5,0x20)` | 32 | 1 @`0x800B102C` | `0x80000000` | **nicht gesetzt** |
| `flag(5,0x21)` | 33 | 1 | `0x40000000` | **nicht gesetzt** |
| `flag(5,0x22)` | 34 | 1 | `0x20000000` | W1 ODER = `0x20000000` → ✅ **GESETZT** |

**Damit ist §11 in einem wesentlichen Punkt zu korrigieren:** die AOT-Maschinerie funktioniert —
Slot 5 feuert seinen sce-4-Handler und setzt `flag(5,0x22)` als Ein-Bild-Signal, sobald der
Spieler in der Zone steht. Was fehlt, sind die BEIDEN AUSSEREN Bedingungen des Gatters @0x2180:
**`flag(3,0x74)`** (Story) und **`flag(5,0x20)`**.

`flag(4,0x0f)` = Bank 4 Wort 0 @`0x800B1018`, Maske `0x80000000 >> 15` = **`0x00010000`**.

---

## 14. Die ROOM1030-SCD-Struktur, statisch aufgeloest — und die Bestaetigung des Nutzers

**Nutzer (2026-08-08):** *„mit direktem Jump zu 1030 wirst du das Kriechen der Zombies nicht
sehen. das wird erst durch die Cutscene ausgeloest."* Das deckt sich exakt mit der Messung aus
§11/§13: die Vorbedingungen fehlen beim Direktsprung. Die dynamische Jagd ist damit beendet;
der Rest ist statisch aufgeloest.

### SCD-Basis und Sub-Tabelle

Bei `0x1fd0` steht die Sub-Offset-Tabelle (Offsets relativ zu `0x1fd0`):
```
0x1fd0  18 00 b0 01 d6 01 28 02 56 02 e2 04 f8 04 84 07
0x1fe0  94 07 10 08 6a 08 a8 08
```
→ sub00 `+0x018` = **0x1fe8** · sub01 `+0x1b0` = **0x2180** · sub02 `+0x1d6` = **0x21a6**
(sub02 deckt sich exakt mit der Dossier-Angabe „sub02 @0x21a6" — die Basis ist damit bestaetigt.)

| Sub | Adresse | Rolle |
|---|---|---|
| sub00 | `0x1fe8` | Raum-Init: `22 03 74 01` = **Set(bank3,0x74,1)** (Story, bedingungslos), danach `If Ck(bank4,0x0f,==1)` → `22 05 14 01` Set(bank5,0x14,1) |
| sub01 | `0x2180` | Der Poller (wird laut Per-Frame-Modell in JEDEM Gameplay-Bild neu geseedet): `Ck(bank3,0x74,==1) && Ck(bank4,0x0f,==0) && Ck(bank5,0x20,==1)` → `If Ck(bank5,0x22,==1)` → `Set(bank4,0x0f,1)` |
| sub02 | `0x21a6` | der Kriech-Zyklus (Glied 6) |

**Das schliesst den Kreis:** sub00 setzt die Story-Flagge beim Betreten; sub01 pollt und setzt
`flag(4,0x0f)`, sobald der Spieler in den Zonen steht; beim naechsten Raum-Init setzt sub00 dann
`flag(5,0x14)` und sub02 laeuft.

### `flag(5,0x20)` kommt aus einem AOT-Payload, NICHT aus einem Opcode

`Set(bank5,0x20,1)` = `22 05 20 01` hat **0 Vorkommen in ganz STAGE1**. Statt dessen tragen drei
aufeinanderfolgende Aot_set-Records (Stride 20) die Payload-Form `05 00 <bit> 00 01 00`:

| Payload | Offset in ROOM1030.RDT |
|---|---|
| `flag(5,0x20)` | `0x1cd8` |
| `flag(5,0x21)` | `0x1cec` |
| `flag(5,0x22)` | `0x1d00` |

(`flag(5,0x21)` kommt zusaetzlich bei `0x21de` vor — das ist die Payload im `Aot_reset` von sub02.)

Gemessen wurde `flag(5,0x22)` gesetzt, `flag(5,0x20)` nicht → der Spieler stand in der einen Zone,
aber nicht in der anderen. Beide werden in **derselben** Bild-Auswertung gebraucht (Wort 1 wird am
Bildende gewischt), die Zonen muessen sich also ueberlappen.

### Woran es im PORT wirklich haengt

Alles andere ist laut §? bereits vorhanden (Bank-5-Wort-1-Wisch `scd_vm.c:658`, `Aot_reset` als
FULL-RETYPE, `flag(5,0x22)` wird live korrekt gesetzt). **Es fehlt genau Glied 1: der Aktor-Stempel
`entity+0x0B`** (`@0x80042f5c` / `@0x80042fc4 sb v0,11(s1)` in `FUN_80042bac`). Ohne ihn ist
`member15 == 5` in sub06 nie wahr, und der Zyklus kann selbst bei gesetzten Flags nichts tun.

---

## 15. EINBAU-STAND (2026-08-08)

### Erledigt: der Vorbedingungs-Blocker

`member_0b` ist frei. Der Adult-Spider-LOS-Latch ist nach `aspider_los` umgezogen
(`re15_actor.h`, `enemy_ai_common.c:7062/7063` + `:7278/:7279`) — reiner Speicher-Umzug, keine
Verhaltensaenderung, 116/116 Tests gruen. Der Header-Kommentar hatte diese Reihenfolge selbst als
Vorbedingung benannt. Nebeneffekt: `Member_cmp(15)`/`Work_set` auf einen AKTOR las bisher den
Spider-Latch statt des AOT-Stempels — das war eine echte Divergenz und ist jetzt behoben.

### Als naechstes: Glied 1 — der Aktor-Stempel

**Der Port-Scan `re15_aot_scan(player_x, player_z, active_cut)` (aot_common.c:545) nimmt nur die
SPIELER-Position.** Das Original `FUN_80042bac` bekommt dagegen einen ENTITY-Zeiger
(`@0x80042bb4 addu s1,a0,zero`) und wird fuer JEDE Entitaet gefahren; dabei entsteht der Stempel
(`@0x80042f44 addiu v0,s2,255` → `sb v0,11(s1)` @0x80042f5c ACTION / @0x80042fc4 AUTO).

Zu beachten sind dabei Glied 2 (LAST-WINS auf AUTO, FIRST-WINS auf ACTION), Glied 3 (drei
verschiedene Clear-Regeln + ein vierter Mechanismus), Glied 4 (geschlossenes Intervall, unsigned)
und Glied 5 (weitere Stempel-Vorbedingungen).

### ✅ Ein PRUEFBARES Ziel gibt es jetzt — gemessen am Original

Die Messung liefert die Soll-Werte, gegen die der Einbau verifiziert werden kann:

| Situation im Original | Soll |
|---|---|
| 5 von 6 Zombies (Typ 0x16) stehen bei z≈-24838 | `entity+0x0B` = **0x05** |
| ein Zombie weiter hinten (z≈-25561) | `entity+0x0B` = **0x00** |
| Leerwert vor dem ersten Treffer | **0xFF** (Slot 0 zeigte das beim Raumeintritt) |

Damit ist Glied 1 nach dem Einbau **direkt gegen das Original pruefbar** — kein „sieht richtig aus".

### Weiterhin OFFEN vor Glied 7/8

Die Feldzuordnung `+0x1C4` ist ungeklaert (§10): gemessen stand dort pro Slot `0x000X0000`, also
der Slot-Index im OBEREN Halbwort, waehrend das Freigabe-Bit `0x1000` im unteren liegen muesste
(dort durchgehend 0). Entweder ist `+0x1C4` nicht das Wort aus Glied 7/8, oder es traegt
zusaetzlich eine Slot-ID. **Vor Glied 7/8 klaeren** — Glied 1 haengt nicht davon ab.

---

## 16. ⛔ KORREKTUR zu §10 — `+0x1C4` ist ein HALBWORT, die Feldzuordnung stimmt

In §10 stand als Warnung: „`+0x1C4` stand pro Slot auf `0x000X0000`, also der Slot-Index im OBEREN
Halbwort, waehrend das Freigabe-Bit `0x1000` im unteren liegen muesste. Vor Glied 7/8 klaeren."

**Geklaert — und der Fehler lag bei mir.** Alle drei Steer-Funktionen lesen identisch (STAGE1.BIN,
selbst disassembliert):

```
80101eb8: lui  v1,0x800b
80101ebc: lw   v1,-14460(v1)      ; aktueller Entity-Zeiger
80101ec4: lhu  v0,452(v1)         ; 452 = 0x1C4  ->  HALBWORT
80101ecc: andi v0,v0,0x1000
80101ed0: beq  v0,zero,0x80101edc
80101ed4: ori  v0,zero,0x1001
80101ed8: sw   v0,4(v1)           ; entity+0x04 = 0x1001
```
(identisch bei `@0x801021c0-e0` und `@0x80105784-a4` — die drei Steer-Funktionen aus Glied 8.)

Ich hatte ein **32-Bit-Wort** gelesen. Das obere Halbwort (`+0x1C6`) traegt eine Slot-ID, das ist
ein ANDERES Feld. Das untere Halbwort — das einzige, das das Original liest — war `0x0000`, also
Bit `0x1000` schlicht nicht gesetzt. Das passt exakt zum Rest der Messung (die Kette lief nie).

**Die Feldzuordnung `+0x1C4` in Glied 7/8 ist damit bestaetigt, nicht widerlegt.** Der Blocker vor
Glied 7/8 ist aufgehoben.

Bonus aus derselben Stelle: bei gesetztem Bit schreibt das Original `entity+0x04 = 0x1001`, also
`+0x04 = 0x01` und `+0x05 = 0x10` — genau der Sub-Modus 0x10, den Glied 9 als TOGGLE
(`FUN_80104f80`) beschreibt. Glied 8 → Glied 9 haengt damit instruktionsseitig zusammen.

---

## 17. EINBAU-STAND (2026-08-08, Session-Abschluss) — DIE KETTE IST IM PORT UND LAEUFT

Commits: `b0aeb5e6` (Stempel/Notch ans Frame-Ende + 0x0A-Wisch), `4c670931` (sca_mask),
`43a73748` (Grid-Wurzel 1 + Toggle + Kriech-Lokomotion + Hand-Lock), + Opcode 0x3D mit
End-to-End-Beweis. Alle Glieder 1-10 sind eingebaut; probe_gate_1030 Phase D spielt die
VOLLSTAENDIGE Kette ueber echten RDT-Bytecode (einzige gesetzte Vorbedingung: flag(4,0x0f)):
Stempel 5 -> sub07 (0x3D/0x26/0x35) anim_flags|=0x1000 -> 0x1001 -> Toggle -> Kriech-Commit
grid=0x81/sca=8 -> Clip 0x1A mit Hand-Lock-Vortrieb (dz=+2202 in 190 Ticks, durch den
Tor-Streifen) -> Zone 4 -> sub05 (0x2000) -> Aufstehen (grid=0, sca=4, Wort 0x201).
Drossel Cmp(wv7<4) verhaelt sich byte-true. ctest 117/117 (neu: unit_crawl_toggle).

### Blocker-Bilanz
- **BLOCKER A: ENTSCHIEDEN, SAFE.** Alle 17 grid_id-Bit-0x80-Lesestellen (16 aus §3 + die
  17. in re15_damage.c Aim-Band @0x80101614-20) haben byte-belegte Original-Gegenstellen am
  physisch selben Bit — das Original teilt "liegend"=="kriechend" bewusst. 0x81 wird geschrieben.
- **BLOCKER B: Algorithmus GESCHLOSSEN, Rundung offen.** FUN_80109470 ist ein HAND-Lock
  (5x CompMatrix, Kette 0->7->Arm, Hand-Bone 10/13; die "4x"-Angabe in §3 war die
  Schwester FUN_8010939c/Fuss). Implementiert mit exakter FUN_80022da0-Semantik (s16-Rot,
  >>12 floor, B.t s16). LSB-Genauigkeit gegen die GTE-Kette + Rotations-Staleness: erst per
  Savestate C messbar (§5) — Messung weiterhin erwuenscht, aber nicht mehr einbau-blockierend.
- **BLOCKER C.1 (0x3D-Zensus): ERLEDIGT.** Game-weit exakt 4 RDTs betroffen (1030/1031 +
  1040/1041); ROOM1040 nutzt Stempelwerte 6/7 + 20 Bank-5-Latches Bits 0x00-0x13 — dessen
  Kriechtor muss separat verifiziert werden (gleiche Maschine, andere Operanden).
- **BLOCKER C.2 (Objekt-Notch): war bereits erledigt** (1369b1aa), integration_keypad gruen.

### Offene Punkte (ehrlich OFFEN, dokumentiert im Code)
1. Playback-Richtung Toggle-Clip 0x12: Port folgt dem DATEN-Anker (py-Rampe -1744->-175 =
   Hinlegen = vorwaerts, B3 §6); die literale a2=(s8)+0x9F-Abbildung wuerde die Rampe
   invertieren. Dynamische Klaerung: Savestate B, +0x95-Folge waehrend des Hinlegens.
2. Grid-1-Zeilen 1-5/15 (u.a. 0x8010466C/0x80104808, [15] mit RNG-Zug @0x8010a188) — No-op
   mit Einmal-Log; im ROOM1030-Zyklus nicht erreicht.
3. Setzer von +0x1D8-Bits 0x20/0x40 (Hand-Lock-Unterdrueckung) und 0x80 (Aufsteh-Sperre)
   sowie +0x8C-Konsument: statisch nicht gefunden (B3 §8.4); Spawn=0 -> Verhalten identisch.
4. Cull-Randfall der Hand-Referenz (B3 §8.3): nicht nachgebaut, on-screen irrelevant.
5. LIVE-Verifikation im echten Spielfluss (Story-Weg ueber die Cutscene, gdigrab) steht aus —
   der Debug-Sprung zeigt das Kriechen NIE (§11); der Nutzer-Weg ist der Beweis.

---

## 18. HARDWARE-GROUND-TRUTH des vollen Zyklus (2026-08-08, PCSX-Redux, 2 deterministische Laeufe)

Messpunkt: Exec-BP @0x80030444 (Pad-Aufbereiter, 1x/Spiel-Frame VOR der VM — dort ist +0x0B der
stabile Vor-Scan-Stempel). Einziger Poke: `flag(5,0x20)` (0x800B102C |= 0x80000000); sub01 →
flag(4,0x0f) → sub08 → Tor-Zellen/flag(5,0x14) liefen ORGANISCH. Logs: tools/redux/
crawl_cycle_out.txt (Lauf 1) + crawl_cycle2_out.txt (Lauf 2); Sonden crawl_cycle*.lua + drive_1030.py.

### Der Zyklus (Lauf 2, Slot 4; Lauf 1 identisch ±1 gf)
- **HINLEGEN:** 0x1000→Steer→0x1001→Toggle im SELBEN Frame; 94=0x12, 95=rng&3(+1 Durchfall-Advance),
  9F=1 ⇒ Pose 97→0 RUECKWAERTS (f314 a2≠0: Slot = fc−1−95); 95 zaehlt +1/Frame; Position eingefroren;
  Trigger→Commit = 95 Frames; Commit = Wrap+1: 09=0x81, 1D7=8, Wort=1, 93 geloescht; +1 Frame: 94=0x1A, 8C=0x1E.
- **KRIECHEN:** 234 Frames, dz=+2837 (ø 12,1/Frame); **Savestate-C-Messung:** Vortrieb in 2-FRAME-PAAREN
  (Delta 0, dann ~20-60 — konsistent mit Half-Rate-Keyframes: Clip 0x1A = 99 Frames auf ~50 kf).
  8C bleibt konstant 0x1E (Konsument feuert nie — deckt B3 §8.4). 1C4 konstant 0x1000.
- **AUFSTEHEN:** Zone-4-Stempel → +1 Frame: 1C4 0x1000→0x2000 (exakt &0x0FFF|0x2000), DECIDE-0
  konsumiert → **Sub-Modus 6** (f920[6], dieselbe Toggle-Funktion), Clip 0x12, 95=rng&3, **9F=0 ⇒
  Pose VORWAERTS 0→97** = langsames Aufstehen ueber 95 Frames, kein Steh-Halt (danach sofort
  Clip 0x05 mit Blend 0x0E). Commit: 09=0, 1D7=4, Wort 0x201. **1C4 bleibt 0x2000 stehen (kein Clear).**
  ⇒ Der Richtungs-Fix (Commit dieser Session) ist damit HARDWARE-BESTAETIGT; B3-§6-Rampen-Lesung
  endgueltig widerlegt (Keyframes wie gespeichert: frame 0 = liegend py=−175, frame 97 = stehend −1744).

### sub08 = die "Cutscene" (Bytes @0x2764 dekodiert, Timing 2x gemessen)
t+0: `Set(1,0x1B)` (= flag(1,27) = das KINO-/Letterbox-Bit!), Aot_reset 17, 4x Rolltor-`4b`-Records,
Tor-Zellen 0xF7, wv5/wv7=0, Gosub 9 (SOLO-Trigger: genau EIN Zombie pro Aufruf, Cursor wv5
persistiert; weitere Aufrufe t+5/25/35); Kamera-Cuts `29 0c`@t+25, `29 0b`@t+250, `29 05`@t+450
(Sleeps 5+20+10+15+20+180+200); Kino aus + `Set(5,0x14)`@t+450; Gruppen-Latch @t+453 (sub02→Gosub 6
latcht ALLE gestempelten gleichzeitig). Drossel wv7<4 biss nie (wv7=1 beim Gruppenstart).

### NEU ENTDECKTES PORT-LOCH: Kriech-Verfolgung nahe am Spieler
Nach dem Tor schaltet der Kriecher bei ~1300 Einheiten auf **Sub-Modus 1** (Clip 0x1B 19f einmal →
Clip 0x1C-Loop 13f = Kriech-Lauern) und **Sub-Modus 3** (Clip 0x09 = Kriech-Attacke); dort ist DECIDE
ein Stub und **0x2000 wird IGNORIERT** — Aufstehen NUR aus Sub 0 (Lauf 1: Kriecher attackierte den in
Zone 4 stehenden Spieler statt aufzustehen). Die Port-Zeilen 1/3 waren OFFEN-No-ops — Einbau laeuft.

### Wartende Zombies
Kein Warte-Sonderclip: normale Verfolger-AI (Clips 0x02/0x05), die an der fuer Maske 4 soliden
Tor-Zelle (0xF7 & 4 ≠ 0) bei z≈−24838 auflaeuft — genau in Zone 5.
