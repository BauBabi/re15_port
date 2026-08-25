# FINDING 3 — „Item aufgenommen → das Welt-Modell muss verschwinden" (ROOM1020 u. a.)

**Datum:** 2026-08-24 · **Status:** URSACHE GEFUNDEN + GEMESSEN, Fix steht aus (diese Phase: nur Dossier)
**Urteil:** Das ORIGINAL entfernt das Modell **sofort beim Bestätigen** der Aufnahme. Der Port tut das
**nicht** — er räumt es erst beim **Wiederbetreten** des Raums weg. Das ist eine echte Divergenz,
kein „macht das Original auch nicht".

---

## 1. Der Mechanismus im Original — vollständig

Das Welt-Modell eines liegenden Items ist ein ganz normales **Obj_model_set-Objekt** aus dem
Objekt-Pool `DAT_800b3f98`, Stride `0x94`. Der Zeichen-Gate ist Bit 0 des Flag-Worts an `+0x00`:

```
FUN_8002c18c (Objekt-Draw-Loop, RE_15_Quellcode_V2/FUN_8002c18c.c):
    if ((*(uint *)((int)&DAT_800b3f98 + iVar6) & 1) != 0) { ... RotMatrix/Draw ... }
    iVar6 += 0x94;
```

`Item_aot_set` (SCD-Opcode 0x50) verknüpft AOT-Zone, taken-Bit und Objekt-Index:

| Feld | Byte im Opcode | Record-Offset (rec = pc+2) | Beleg |
|---|---|---|---|
| item type | pc[14] / pc[22] (long) | rec+12 / rec+20 | `lhu 0(s1)` @0x8001e0a8 |
| amount    | pc[16] / pc[24]        | rec+14 / rec+22 | `lhu 2(s1)` @0x8001e0c0 |
| **taken-Bit** | pc[18] / pc[26]    | rec+16 / rec+24 | Installer `lhu a1,18(a2)` @0x80040680 (long: `lhu a1,26(a2)` @0x80040670) |
| **Objekt-Index (Welt-Modell)** | pc[20] / pc[28] | rec+18 / rec+26 | Installer `lbu s1,20(a2)` @0x80040684 (long: `lbu s1,28(a2)` @0x80040674) |

### 1a. Persistenz über Raumwechsel — der Installer (Raum-Eintritt)

`Item_aot_set`-Handler, taken-Zweig:

```
80040674: lbu s1,28(a2)      ; long form:  Objekt-Index = pc[28]
80040684: lbu s1,20(a2)      ; short form: Objekt-Index = pc[20]
800406d4: lui a0,0x8007
800406d8: lw  a0,18056(a0)          ; a0 = *(0x80074688) = Flag-Zone 9 (= DAT_800b1078)
800406dc: jal 0x8004efe4            ; Flag-GET(zone9, taken-Bit)
800406e4: beq v0,zero,0x8004071c    ; nicht genommen -> normal installieren
800406e8: lui v1,0x8000             ; v1 = 0x80000000
800406ec: lw  v0,0(s0)              ; v0 = AOT-Record-Zeiger
800406f4: sb  zero,0(v0)            ; rec[0] = sce 0  -> Zone TOT
800406f8: sll v0,s1,3 ; addu ; sll ; addu ; sll   ; v0 = idx * 0x94
8004070c: lui at,0x800b
80040710: addiu at,at,16280         ; 0x800b3f98
80040714: addu at,at,v0
80040718: sw  v1,0(at)              ; obj[idx].flags = 0x80000000  -> Bit0 clear = NICHT gezeichnet
```

`0x80040718` ist **der einzige Schreiber von 0x80000000 in den Objekt-Pool im gesamten Spiel**
(Ghidra-XREF-Liste von `DAT_800b3f98`, 24 Xrefs; nur diese eine Stelle schreibt den Wert).
→ Das ist der Persistenz-Pfad, und **der ist im Port bereits korrekt implementiert**
(`scd_vm.c:3489-3490`).

### 1b. Der SOFORT-Pfad beim Aufnehmen — bisher im Port komplett fehlend

Der zweite Weg ist **kein** Teil des Item-Modals, sondern ein **Message-Script-Callback**.

**Das Aufnahme-Prompt trägt den Aufruf im Text-Bytecode.** Das Skript liegt im Bank-Format
`&DAT_800c4fc6 + u16[param_3]` (`FUN_80027e68`, Typ 0x100 — genau der Typ, mit dem das Modal
öffnet, `@0x8001df6c`/`@0x8001df88`). Der Port hat die Bytes bereits byte-true extrahiert:

```
re15_port/engine/src/gen/item_prompt_data.inc — re15_item_prompt_script_take[31]:
  'W','i','l','l',' ','y','o','u',' ','t','a','k','e',' ','t','h','e'
  08            newline
  05 01         Farbe
  06 00         Item-Namen einfügen
  05 00         Farbe
  1b            '.'
  03            <- Yes/No-Halt
  02            <-   Skip-Weite (Bytes, die bei „No" übersprungen werden)
  f9 00         <-   ESCAPE 0xF9, Callback-Index 0
  01            END
```

**Die Ausführung** — `FUN_8002877c`, gerufen aus dem Message-VM-Confirm
(`@0x800285a8`, direkt nachdem der virtuelle Confirm 0x4000 das Box-Bit `DAT_800b8520 & 0x80`
gelöscht hat):

```
8002877c: addiu sp,sp,-24
80028784: addiu a1,a1,-31444        ; a1 = &DAT_800b852c  (Message-Zeiger)
8002878c: lw  v1,0(a1)              ; v1 = ptr  (zeigt auf das 0x03)
80028794: addiu v1,v1,1             ; ptr+1     (auf die Skip-Weite)
80028798: sw  v1,0(a1)
800287a0: lw  a0,-31444(a0)
800287a8: lbu v0,-31456(v0)         ; v0 = DAT_800b8520
800287ac: lbu a0,0(a0)              ; a0 = Skip-Weite (= 0x02)
800287b0: andi v0,v0,0x1            ; Bit0 = „No" gewählt
800287b4: mult a0,v0
800287bc: addu v0,v0,v1             ; ptr += Skip * No-Flag
800287c0: addiu v1,v0,1
800287c4: sw  v1,0(a1)
800287d4: lbu v1,0(v1)              ; Byte an der neuen Position
800287e4/ec/f4: != 0xf9  -> return   ; „No" landet auf 0x01 (END) -> KEIN Callback
80028808: lbu v0,0(ptr)             ; Callback-Index
80028810: sll v0,v0,2
80028818: addiu at,at,12084         ; Tabelle @0x80072f34
80028820: lw  v0,0(at)              ; [0] = 0x80021f6c
80028828: jalr v0
```

Rechnung für `... 03 02 f9 00 01`:
* **YES** (Bit0 = 0): ptr → 0x02 → +0 → +1 → zeigt auf `f9` ✓ → Index `00` → **Aufruf `LAB_80021f6c`**
* **NO** (Bit0 = 1): ptr → 0x02 → +2 → +1 → zeigt auf `01` (END) → kein Aufruf ✓

**Der Callback `LAB_80021f6c` — DAS ist die gesuchte Modell-Entfernung:**

```
80021f6c: lui v1,0x800b
80021f70: lw  v1,-13776(v1)         ; v1 = DAT_800aca30 = AOT-Record der Item-Zone
                                    ;   (gesetzt vom sce-9-Handler @0x80043364)
80021f80: lbu v0,1(v1)              ; rec[1] = Flags
80021f88: andi v0,v0,0x80           ; long-form-Bit
80021f8c: beq  v0,zero,0x80021f9c
80021f90: sb  zero,0(v1)            ; rec[0] = 0  -> AOT-Zone TOT (unbedingt, Delay-Slot)
80021f98: addiu s0,v1,20            ;   long : Item-Feldbasis = rec+20
80021f9c: addiu s0,v1,12            ;   short: Item-Feldbasis = rec+12
80021fa0: lbu v0,6(s0)              ; Objekt-Index (= pc[20]/pc[28])
80021fa8: sll v1,v0,3 ; addu v1,v1,v0 ; sll v1,v1,2 ; addu v1,v1,v0 ; sll v1,v1,2   ; idx*0x94
80021fbc: lui at,0x800b
80021fc0: addiu at,at,16280         ; 0x800b3f98
80021fc4: addu at,at,v1
80021fc8: sw  zero,0(at)            ; ***** obj[idx].flags = 0  -> MODELL WEG *****
80021fcc: lhu a1,4(s0)              ; taken-Bit
80021fd0: lui a0,0x800b
80021fd4: addiu a0,a0,4216          ; 0x800b1078 = Flag-Zone 9
80021fd8: jal 0x8004ef90            ; Flag-SET(zone9, taken-Bit)  -> Persistenz
80021fe0: lhu v0,0(s0)              ; item type
80021fec: sh  v0,4054(at)           ; DAT_800b0fd6
80021ffc: jr  ra
```

Der `DAT_800aca30`-Record wird beim Armieren der Aufnahme gesetzt (sce-9-Handler):

```
80043328: lbu v0,11579(v0)          ; Modal-State DAT_80072d3b, nur wenn 0
80043344: lw  a0,-16996(a0)         ; a0 = *(0x800bbd9c) = aktueller AOT-Record
8004334c: sb  v1,11579(at)          ; DAT_80072d3b = 1 (Modal starten)
8004335c: sb  v0,-1098(at)          ; DAT_800afbb6 = item id
80043364: sw  a0,-13776(at)         ; DAT_800aca30 = Record   <- den liest LAB_80021f6c
```

### 1c. Was das Modal (FUN_8001db28) selbst tut — und was NICHT

State 7 (`@0x8001e048`), der Zweig „Inventar hat Platz und YES":

```
8001e074: lw  v1,-13776(v1)         ; derselbe Record
8001e090: sb  zero,0(v1)            ; rec[0] = 0            (nochmal; idempotent)
8001e09c: addiu s1,v1,12            ; Feldbasis
8001e0c4: jal 0x8004dc4c            ; INVENTAR-INSERT (type, amount, Icon)
8001e0d0: jal 0x8004ef90            ; Flag-SET(zone9, taken-Bit)  (nochmal; idempotent)
8001e0d8: sh  zero,6(s1)            ; Objekt-Index-FELD im Record := 0
8001e0e0: sb  zero,11579(at)        ; Modal-State = 0 = DONE
```

→ **Das Modal räumt das Modell NICHT weg.** Es schreibt in `rec+18` sogar nur eine 0
(`sh zero,6(s1)`). Die Modell-Entfernung passiert ausschließlich im Message-Callback aus 1b,
und der läuft **vor** State 7 (Confirm des Yes/No → `FUN_8002877c` → Callback; das Modal sieht
die geschlossene Box erst danach).

**Reihenfolge im Original, gesichert:**
`SQUARE an der Zone` → sce-9-Handler `@0x80043328` (Record merken, Modal an)
→ Modal State 1..5 (Zoom/Flip, Prompt öffnen) → State 6 wartet
→ Spieler bestätigt **YES** → `@0x800285a8 jal FUN_8002877c` → `0xf9 00` → **`LAB_80021f6c`:
Zone tot + `obj[idx].flags = 0` (Modell weg) + taken-Flag**
→ Modal State 7: Inventar-Insert (+ idempotente Wiederholung von Zone/Flag).

Bei **NO** überspringt `FUN_8002877c` das `f9 00`, bei **vollem Inventar** wird gar nicht das
Take-Skript, sondern `re15_item_prompt_script_full` benutzt (`... 57 01 00`, **kein** `03 02 f9`)
→ in beiden Fällen bleibt das Modell korrekt liegen. Der Schrumpf-Abgang State 8 (`@0x8001e10c`)
passt dazu.

### 1d. Rand-Fall prop = 0xFF

50 von 968 ausgelieferten `Item_aot_set`-Records tragen `prop = 255` (Zensus über alle
`STAGE*/ROOM*.RDT`, Walker = `re15_port/tools/aot_sce_census.py`-Opcode-Tabelle). `LAB_80021f6c`
prüft **nicht** auf 255: 255*0x94 = 0x936C → Schreibziel `0x800BD304`. Das ist im Ghidra-Dump
unreferenzierter Null-BSS (keine Xref, keine Symbolisierung) → im Original folgenlos.
Diese Items haben schlicht kein Welt-Modell (z. B. Items in Schubladen/Hintergrund).
Der Port muss den Index deshalb weiterhin gegen `prop_count` klemmen — das ist eine
Pool-Größen-Anpassung, keine Verhaltens-Abweichung.

---

## 2. Die Zuordnung in ROOM1020 (belegt, nicht vermutet)

Aus `re15_port/shared_assets/PSX/STAGE1/ROOM1020.RDT`, main00 (Roh-Bytes mitgeschrieben):

```
main00 ITEM @0x1d6a slot=3 sce=9 fl=0x31 rect=(-24900,-10700,2700,1700)
                    type=0x15 amt=15 tk=2   prop=4
       raw= 50 03 09 31 0000 bc9e 34d6 8c0a a406 1500 0f00 0200 0400
main00 ITEM @0x1d80 slot=4 sce=9 fl=0x31 rect=(-24900,-10700,2700,1700)
                    type=0x15 amt=15 tk=227 prop=5
       raw= 50 04 09 31 0000 bc9e 34d6 8c0a a406 1500 0f00 e300 0500
main00 OBJ  @0x1d96 id=4 type=0 pos=(-23700,-1500,-10000)
main00 OBJ  @0x1db8 id=5 type=0 pos=(-23350,-1500,-10000)
```

Objekt 4 und 5 liegen beide **innerhalb** des Item-Rechtecks (x −24900…−22200, z −10700…−9000)
— die zwei Pistolen-Magazine (0x15 ×15) auf dem Boden. `prop` verweist also nachweislich auf
das Welt-Modell des jeweiligen Items.

---

## 3. MESSUNG am Port

Die GUI-Route (gdigrab am echten Fenster) war in dieser Sitzung **nicht** verfügbar —
`re15_pc.exe` bricht in der RDP-Session vor `main()` ab
(`Assertion failure at WIN_AddDisplay (SDL_windowsmodes.c:380) 'index == *display_index'`,
davor `[audio] SDL_OpenAudioDevice failed: WASAPI can't find requested audio endpoint`).
Gemessen wurde deshalb headless über die Engine-Bibliothek selbst
(`libre15_engine.a` + `libre15_test_support.a`, Sonde im Scratchpad, **nicht** im Repo):
echte `ROOM*.RDT` laden → main00 fahren → `re15_item_modal_start(...)` mit den Record-Werten
→ Modal mit virtuellem Confirm (0x4000) bis DONE → `g_scd.props[prop].active` lesen.

```
ROOM1020 s3  aot3 type0x15 x15 tk=2   prop=4 | VOR : prop[4].active=1 aot.active=1
ROOM1020 s3                                  | NACH: prop[4].active=1 aot.active=0  (inv qty=15, zone9-Bit2=1)
ROOM1020 s4  aot4 type0x15 x15 tk=227 prop=5 | VOR : prop[5].active=1 aot.active=1
ROOM1020 s4                                  | NACH: prop[5].active=1 aot.active=0  (inv qty=15, zone9-Bit227=1)
ROOM1070 s13 aot13 type0x15 x15 tk=144 prop=0| VOR : prop[0].active=1 aot.active=1
ROOM1070 s13                                 | NACH: prop[0].active=1 aot.active=0  (inv qty=15, zone9-Bit144=1)
```

Und derselbe Raum über einen Raumwechsel hinweg (main00 erneut gefahren, Flags behalten):

```
A) nach Raum-Init        props: [4].active=1  [5].active=1 | aot[3] ITEM aktiv
B) NACH dem Pickup       props: [4].active=1  [5].active=1 | aot[3] aktiv=0   <-- Modell BLEIBT
C) nach Wiederbetreten   props: [4].active=0  [5].active=1 | aot[3] type=NONE <-- erst JETZT weg
```

**Befund:** Aufnahme funktioniert (Inventar +15, taken-Flag gesetzt, Zone deaktiviert) — aber der
Modell-Slot bleibt aktiv. Der Draw-Gate im Port ist `re15_port/platform/pc/main.c:7196`
(`if (!g_scd.props[pi].active) continue;`), das Gegenstück zu `flags & 1` @0x8002c18c.
Also: **Das Magazin liegt nach der Aufnahme weiter sichtbar da, bis der Spieler den Raum
verlässt und neu betritt.** Genau die Nutzer-Beobachtung.

---

## 4. Die genaue Port-Stelle, die das Aufräumen unterlässt

1. **`re15_port/engine/src/item_modal_common.c:276-278`** (Modal State 7, der Confirm-Zweig):
   ```c
   if (s_taken) re15_game_flag_set(9, s_taken, 1);   /* FUN_8004ef90 (zone-9 taken flag) */
   if (s_aot_slot >= 0 && s_aot_slot < RE15_AOT_MAX)
       g_aot.slots[s_aot_slot].active = 0;            /* the item leaves the world */
   ```
   Der Kommentar „the item leaves the world" stimmt nur für die **Zone**, nicht für das
   **Modell**. Der Gegenpart zu `sw zero,0(at)` @0x80021fc8 fehlt ersatzlos.

2. **`re15_port/engine/src/scd_vm.c:3465` + `:3489-3490`** — `op_item_aot_set` liest `tk_prop`
   (`pc[20]`/`pc[28]`) korrekt, benutzt ihn aber **nur** für das Install-Time-Hide und wirft ihn
   dann weg (lokale Variable).

3. **`re15_port/include/re15_aot.h:160-167`** — `re15_aot_item_params_t` hat `item_type`,
   `amount`, `taken_bit`, aber **kein** Prop-Feld. Deshalb kann die Aufnahme den Index gar nicht
   kennen.

4. **`re15_port/engine/src/aot_common.c:620` und `:1275`** — beide Modal-Starts
   (`Aot_on`-fire-now und der Scan) reichen `(item_type, amount, taken_bit, slot)` durch; der
   Prop-Index kommt nie an.

5. **`re15_port/engine/src/item_prompt_common.c:28`** — der Prompt-Walker bricht bei `0x03` ab
   (`if (b == 0x01 || b == 0x03) break;`) und erreicht das `02 f9 00`-Ende nie. Einen Dispatcher
   für das Message-Escape `0xF9` (Tabelle `@0x80072f34`) gibt es im Port **überhaupt nicht** —
   das Modal ersetzt den Message-VM-Zweig durch eigene Yes/No-Logik (`s_choice`, `s_msg_no`).
   Deshalb ist der Original-Callback komplett unportiert, nicht nur „an falscher Stelle".

**Gegenprobe:** im gesamten Port gibt es genau zwei Schreiber von `props[].active`:
`scd_vm.c:3559` (Obj_model_set → 1) und `scd_vm.c:3490` (Install-Time-Hide → 0).
Keiner davon liegt im Aufnahme-Pfad.

---

## 5. Was der Fix (nächste Phase) tun muss

* `re15_aot_item_params_t` um `taken_prop` (u8, Default 0xFF) erweitern; in
  `op_item_aot_set` (`scd_vm.c:3465`) mitgeben, über `re15_aot_set_item_tk` speichern,
  in beiden `re15_item_modal_start`-Aufrufern (`aot_common.c:620`, `:1275`) durchreichen.
* Byte-true Ort der Entfernung ist der **YES-Confirm des Prompts** (`FUN_8002877c` →
  `0xf9 00` → `LAB_80021f6c` @0x80021fc8), also im Port die Stelle in
  `item_modal_common.c` State 6, an der `pad_edge & 0x4000` mit `s_choice == 0` bestätigt
  wird — **nicht** hinter der Inventar-Voll-Prüfung. Praktisch deckungsgleich mit State 7
  (weil bei vollem Inventar Prompt 2 ohne `03 02 f9`-Ende läuft), aber der Confirm ist der
  Ort des Originals.
  Gesetzt wird `g_scd.props[prop].active = 0` (Gegenstück zu `sw zero,0(at)` @0x80021fc8;
  das Original schreibt hier **0**, der Installer dagegen **0x80000000** — beide löschen Bit 0,
  der Port modelliert nur Bit 0 als `active`).
* Zusätzlich byte-true: `rec+18 := 0` nach dem Insert (`sh zero,6(s1)` @0x8001e0d8) und
  `rec[0] := 0` bereits im Callback (@0x80021f90). Der Port hat für den Record kein
  Rückschreib-Modell — als OPEN vermerken, wenn es nicht sauber abbildbar ist; verhaltens-
  relevant wird es nur, wenn ein Raum denselben Item-Slot im selben Besuch erneut installiert.
* `prop >= g_scd.prop_count` (insbesondere 0xFF) muss folgenlos bleiben — siehe §1d.

---

## 6. Offene Punkte (klar als offen markiert, nicht geraten)

* **`0xf9`-Tabelle hat nur einen Eintrag.** `0x80072f34[0] = 0x80021f6c`; `0x80072f38` ist im
  Ghidra-Dump als `DAT_80072f38` **Datenvariable** geführt (Xref: `FUN_80022300:80022378`) und
  ist 0. `re15_item_prompt_script_use` endet aber mit `02 f9 01 01 00`, also Index 1. Das heißt
  entweder, dass das „Will you use the X?"-Prompt im RE1.5-Stand nie über `FUN_8002877c` läuft
  (anderer Message-Typ/Bank), oder dass dieser Pfad im Prototyp tot ist. **Nicht** verifiziert;
  für FINDING 3 irrelevant, aber vor einem generischen `0xF9`-Dispatcher im Port zu klären.
* Die visuelle Endabnahme per gdigrab konnte hier nicht laufen (SDL-Display-Assert in der
  RDP-Session, §3). Nach dem Fix muss sie in einer echten Desktop-Session nachgeholt werden:
  ROOM1020 betreten, Magazin aufnehmen, Bild **ohne** Raumwechsel prüfen.
* Der Zensus-Walker desynct in einigen `sub`-Skripten (z. B. ROOM1050 sub00) — die
  ausgewerteten Zahlen (968 Records, 50× prop=255) stammen aus dem sauberen Teil des Laufs
  und sind für die main00-Item-Records belastbar, für Sub-Skript-Items nicht vollständig.
