# Rolltor-Sound ROOM1170 → ROOM1130 — RE-Dossier (2026-08-02)

## 0. Symptom und Ergebnis in einem Satz

**Symptom (Nutzer):** Beim Rolltor Richtung ROOM1130 fehlt das Fahrgeräusch im Port bzw. ist nicht korrekt.

**Ergebnis:** Das Original spielt beim **Benutzen der Türen** (ROOM1170 AOT 0 *und* AOT 4) **keinen einzigen Ton**
— die gesamte Tür-Transitions-Kette ist belegt sound-frei, das Türsequenz-Skript in `DOOR00.DO2` ist ein
leerer Stub. Das Rolltor-**Fahrgeräusch existiert im Original ausschließlich als Teil des SCHALTER-Events in
ROOM1130** (`subScd sub02`): `Se_on(2,12)` → `Se_on(2,10)` → Tor-Hochfahr-Animation → `Se_on(2,11)`, alle aus
der **snd0-Bank der ROOM1130.RDT**. Der Port führt diese Kette nachweislich aus (live gemessen), spielt die
drei VAGs aber **ohne den VAB-Tone-Pitch** ab — **+24 bis +29 Halbtöne zu hoch und 4–5,3× zu kurz**. Aus dem
2,5-Sekunden-Motor-Rumpeln des Originals wird im Port ein 0,47-Sekunden-Blip. Das ist die Hauptdivergenz (D1).

---

## 1. Original-Mechanismus, Teil A — die Tür-Transition spielt keinen Sound (W1)

### 1.1 Die Kette

`0x800B5359=1` → State-1-Handler `@0x8001C9C8` → `FUN_8001d600`. Tür-Zweig (`DAT_800ac9a8 != 0`),
Decompile `RE_15_Quellcode_V2/FUN_8001d600.c`:

```c
FUN_80021634(2,0);
FUN_800171f4();                        // "DOOR TEXTURE" laden (CD-File)
FUN_80029a98(1,&LAB_80016188);         // Türsequenz als Lade-Task registrieren
do { FUN_80029ac8(1); } while ((DAT_800aca38 & 0x10000) != 0);
...                                    // Zielposition/Stage/Raum/Cut aus dem Tür-Record
FUN_800396fc();                        // Raum-Teardown/-Install
...
DAT_800b5457 = 1;
FUN_800444b0();                        // BGM hörbar machen — KEIN SE
```

Kein `FUN_80045024`-, `SsUtKeyOn*`- oder `SpuVmKeyOn*`-Aufruf in dieser Funktion.

### 1.2 Der Tür-Record ist der rohe AOT-Payload

sce-2-Handler `LAB_800430bc` (ghidra1_V2.txt:154098-154111):

```
800430bc ori   v0,zero,0x1
800430c4 sw    a0,-0x3658(at)=>DAT_800ac9a8   ; Record-POINTER = der AOT-Payload selbst
800430d4 sb    v0,offset DAT_800b5359(at)     ; Ladeflag
800430d8-e4 DAT_800aca40 |= 0xff000000
```

Kein Kopieren, kein Ton. `DAT_800ac9a8` zeigt direkt in die RDT-Bytes des `Door_aot_set`-Records
(Payload = ab `spawn_x`).

### 1.3 Die Türsequenz-Task-Kette (Ladebildschirm) — vollständig sound-frei

`LAB_80016188` (ghidra1_V2.txt:82393-82414) iteriert eine Funktionstabelle `@0x80071d30` bis zum
NULL-Eintrag: `[0]=FUN_800161e0`, `[1]=FUN_800164c8`, `[2]=0x80016664` (Raw-Bytes im Dump, unten
disassembliert).

**`FUN_800161e0` (Init)** — parst den DOOR-TEXTURE-Puffer `@0x801a1000`:
- `FUN_800170e0(DAT_801a1008 - DAT_801a1004)` — lädt die **SE-Bank 0** aus dem Container (siehe §3).
- bindet das Türmodell (`FUN_80022150(2, ptr[0], 0, 0)`), lädt das TIM (`FUN_8004ee78(ptr[2])`),
- `DAT_800b3f70 = ptr[1]` = **SCD-Basis der Türsequenz**, `FUN_8003edec(&DAT_800b39ac, 0)` startet
  SCD-**Thread-Slot 10** (Pool `0x800b2b4c` + 10·0x170 = `0x800b39ac`).
- `DAT_800b0fde = *(byte*)(door_record + 0xd)` (`sh v0,DAT_800b0fde` @0x800164b4) — **einziger Writer,
  null Leser im gesamten EXE-Dump** (XREF-Liste ghidra1_V2.txt:492950) und **null Treffer in
  `RE_15_Quellcode_Overlays/`** (grep `800b0fde` leer). Der Handover-Befund „toter Store" ist bestätigt.

**`FUN_800164c8` (Anim-Loop)** — `while (DAT_800b39ad != 0) { FUN_80016518(); FUN_800166c4(); FUN_80029ac8(1); }`.
`FUN_80016518` tickt die SCD-Slots 10..13 über **dieselbe Opcode-Tabelle `@0x800744a8`** wie die
Haupt-SCD-VM (`lw v0,0x0(v0)=>PTR_LAB_800744a8` @0x8001659c == @0x8003f104). Die Türsequenz ist also
ein normales SCD-Skript — inklusive der Möglichkeit von `Se_on` (0x36). `FUN_800166c4` ist reiner
GTE-Renderer (RotMatrix/gte_rt, kein Call außer `FUN_80016b54`).

**Task 3 `@0x80016664`** (aus `info/Re1.5/PSX.EXE` disassembliert, RAM-Base 0x80010000, Header 0x800;
Sanity: `@0x800171f4` = `27bdffe8` ✓):

```
80016664 addiu sp,sp,-0x18
8001666c jal   0x8002178c        ; warten (a0=0), Schleife über FUN_80029ac8(1)
80016674 bne   v0,zero,0x8001668c
8001668c ori   a1,a1,0xffff      ; a1 = 0xfdffffff
80016690-806166b0 DAT_800aca38 &= ~0x02000000 ; Türsequenz-aktiv-Bit löschen
800166a4 sb    v1,0x5456(at)     ; DAT_800b5456 = 2
800166ac jal   0x80029afc
```

Kein Sound. **Die komplette Tür-Transition (EXE-Seite) enthält keinen einzigen SE-Aufruf.**
Der einzige Ort, an dem die Türsequenz einen Ton spielen KÖNNTE, ist ihr eigenes SCD — und das ist leer (§3).

### 1.4 DOOR-TEXTURE-Dateiauswahl

`FUN_800171f4` (Disasm ghidra1_V2.txt:83680-83696):

```
8001720c lbu  v0,0xc(v0)                 ; door_record BYTE +0xC = Dtex-Index
80017214 sll  v0,v0,0x1
80017224 lhu  a0,0x0(at)=>DAT_80071d2c   ; u16-Tabelle @0x80071d2c → CD-File-ID
8001722c "DOOR TEXTURE" → FUN_80013b60(id, 0x801a1000, 1, ...)
```

Tabelle `@0x80071d2c` = `25 00 00 00` (ghidra1_V2.txt:233458-233461): Eintrag 0 = **CD-File-ID 0x25**,
Eintrag 1 = 0. Im Asset-Baum existiert **genau eine** Türdatei: `DOOR/DOOR00.DO2` (57016 B). Die
Struktur passt exakt auf das `FUN_800161e0`-Parsing (§3) — Dtex 0 ⇒ DOOR00.DO2.
(Die File-ID→Name-Bindung ist wie bei ARMS/CORE nur strukturell belegbar; die EXE hält LBAs
`@0x8006f43c`, keine Namen — siehe RE15_COMBAT_SE_SUBSYSTEM.md §2.)

---

## 2. Original-Mechanismus, Teil B — die Door-Records beider Türen sind sound-identisch (W2)

`ROOM1170.RDT` (u32@0x40 mainScd=0x11f4), main00, strukturtreues Disassembly
(Skill `scd-disassembly`, Offset-Tabellen-geführt, kein Byte-Scan):

```
0x0012 Door_aot_set 00 02 31 04 00 | dc 05 40 38 34 08 a4 06 | 42 d2 e0 e3 7c 98 8d 0b | 00 17 0b 04 00 00 | ...
0x0104 Door_aot_set 04 02 31 00 00 | 9e a8 34 b8 e8 08 8e 03 | 5a 0a 00 00 be 3c 00 0c | 00 13 07 00 00 00 | ...
```

Decodiert (Record-Layout, verifiziert gegen die `FUN_8001d600`-Leser und den AOT-Dump des Ports):

| Feld | AOT 0 (Rolltor in den Hof, Selbst-Tür) | AOT 4 (Tür nach ROOM1130) |
|---|---|---|
| slot / sce / sat / band | 0 / **2** / 0x31 / 4 | 4 / **2** / 0x31 / 0 |
| rect x,z,w,h | 1500,14400,2100,1700 → Mitte **(2550,15250)** ✓ | −22370,−18380,2280,910 → Mitte **(−21230,−17925)** ✓ |
| Payload+0..+7 spawn x,y,z,rot | −11710,−7200,−26500, 2957 ✓ | 2650,0,15550, 3072 |
| +8 stage / +9 room | 0 / 0x17 (= ROOM1170, Selbst-Tür) | 0 / 0x13 (= ROOM1130) |
| +0xA cut | **0x0b (11)** ✓ | **0x07** ✓ |
| +0xB band | 4 | 0 |
| **+0xC Dtex** | **0** | **0** |
| **+0xD door_type** | **0** | **0** |

**Beide Türen sind in den sound-relevanten Bytes (+0xC/+0xD) identisch 0** — beide laden dieselbe
DOOR00.DO2, und +0xD landet im toten Store `DAT_800b0fde`. Das Original unterscheidet „Rolltor" von
„normaler Tür" auf dem Tür-Weg **überhaupt nicht**.

---

## 3. DOOR00.DO2 — Türsequenz-Container: leeres Skript, aber eigene SE-Bank

Datei `re15_port/shared_assets/PSX/DOOR/DOOR00.DO2` (57016 = 0xdeb8 B), geparst exakt nach
`FUN_800161e0`:

| Was | Datei-Offset | Inhalt |
|---|---|---|
| Header u32[0..2] | 0x0 | `0xc, 0x8bc8, 0x97f8` |
| Ptr-Tabelle (3 × u32, rel. base+0xc) | 0xc | `0xc, 0x998, 0x99c` |
| ptr[0] Modell | 0x18 | Türmodell (an `FUN_80022150`) |
| **ptr[1] SCD** | **0x9a4** | **`02 00 01 00`** — Entry-Offset 2 → Opcode `0x01` = **Evt_end. LEER.** |
| ptr[2] TIM | 0x9a8 | `10 00 00 00 09 00 00 00` = TIM, 8bpp+CLUT |
| u32[1] SE-Bank VH | 0x8bc8 | 8-B-EDT-Präfix `00 00 13 16` `00 00 23 17` + `pBAV`-VAB @0x8bd0 (ver 7, fsize 21216, ps=1, ts=2, **vs=2**) |
| u32[2] SE-Bank VB | 0x97f8 | 18112 B ADPCM (2 VAGs) |

`FUN_8003edec` (Decompile): Thread-PC = `DAT_800b3f70 + u16[DAT_800b3f70 + idx*2]` → für DOOR00.DO2:
PC = SCD-Basis+2 → `0x01` = sofortiges Thread-Ende. **Die Türsequenz des MZD-Builds tut nichts und
spielt nichts** — der RE2-artige Türsequenz-Rahmen (Modell+TIM+SCD+SE-Bank pro Türtyp) ist vorhanden,
aber inhaltlich gestubbt.

**Bank-0-Load** `FUN_800170e0` (Decompile, einziger Caller `FUN_800161e0` @0x800162dc — XREF-Liste
ghidra1_V2.txt:83591):

```c
memcpy(&DAT_801fdd00, DAT_801a1004, size);                      // VH-Blob (EDT+VAB) → Bank-0-Puffer
DAT_800b2598 = &DAT_801fdd00 + *(int*)(&DAT_801fdcf8 + size);   // VH = Basis + u32[blob_size-8] (= 8, EDT-Präfix)
SsVabOpenHeadSticky(DAT_800b2598, 0, 0x1020);                   // Bank 0
SsVabTransBody(DAT_801a1008, handle);                            // VB
```

⚠️ Korrektur zu `RE15_COMBAT_SE_SUBSYSTEM.md` §2 („bank 0 resident … called: boot"): **Bank 0 wird
ausschließlich beim Tür-Übergang geladen** — es ist die Tür-SE-Bank aus DOOR00.DO2 (2 Records:
prog0/tone1 und prog0/tone2 → die 2 VAGs; mutmaßlich Tür-öffnen/-schließen).

**Wird Bank 0 je abgespielt? Nein:**
- Se_on-Zensus über **alle** `STAGE*/ROOM*.RDT` (main+sub, strukturtreuer Walker): **284 Se_on, alle
  284 mit bank=2**. Kein Skript ruft Bank 0 (oder 1/3/4/5) über Se_on.
- EXE: alle ~30 `jal FUN_80045024`-Callsites geprüft — statische Konstanten sind 0x101/0x102/0x103/
  0x108/0x109/0x10a (bank 1), 0x206/0x207/0x208 (bank 2), 0x303 (bank 3), 0x404/0x405/0x406/0x408/
  0x409 (bank 4). Die einzigen dynamischen: der Se_on-Opcode-Handler (`@0x80041730`, §4.2) und der
  Kampf-Action-Treiber `@0x80018424` (`lhu a0,0x26(t0); sll a0,a0,8` — kann bank 0 erzeugen, ist aber
  der Attacker-Action-SE aus `DAT_800b52c4+0x26`, kein Tür-Pfad).

---

## 4. Original-Mechanismus, Teil C — das Rolltor-Fahrgeräusch ist das ROOM1130-Schalter-Event (W4)

### 4.1 ROOM1170: null Sound

Komplettes Disassembly (main00 + alle 16 Subs): **kein einziges `Se_on`/`Se_vol`/`Xa_on`** im ganzen
Raum. Der cut-11-Pfad der Selbst-Tür (sub00 case 11 → `Evt_exec ff 18 0e` → sub14) ist reiner Dialog
(`Message_on`/`Plc_motion`/`Plc_neck`), Flags 3/62, 1/27, 2/7. Deckt sich mit der Runtime-Messung des
Handover („Intro-Pfad führt null Se_on aus").

### 4.2 ROOM1130 sub02 — der Rolltor-Mechanismus (der Kern-Beleg)

`ROOM1130.RDT`: mainScd u32@0x40=0x88c, subScd u32@0x44=0x9c8, 3 Subs (`0x6, 0x54, 0x60`).

**main00** installiert das Tor + den Schalter, Flag-gated:

```
0x0082 Ifel_ck / 0x0086 Ck z=3 bit=107 (Tor-offen-Flag)
  [Flag AUS]  0x008a Aot_set  04 03 31 .. rect(-950,14350,800,800) payload: ff 00 18 02
              0x009e Obj_model_set 00 ... pos=(-1240,0,16300) rot_y=0x0c00   ; Tor UNTEN
  [Flag AN]   0x00c4 Obj_model_set 00 ... z=0x3f64 ... 00 04                 ; Tor OBEN
```

Schalter-AOT slot 4 = **sce 3 EVENT**; Handler `LAB_800430f0` (ghidra1_V2.txt:154112-154124):

```
800430fc lhu a0,0x0(v0)      ; cond = 0x00ff
80043100 lbu a1,0x3(v0)      ; sub  = 0x02
80043104 jal FUN_8003ee3c    ; → startet subScd[2] = sub02
```

**sub02** (Datei-Offset 0xa28 in ROOM1130.RDT; Offsets = sect-relativ, Datei = 0x9c8+off):

```
0x0060 Message_on 00 80 ff        ; Yes/No-Prompt ("Will you push it?"-Klasse)
0x0064 Evt_next
0x0066 Ifel_ck skip=208
0x006a Ck  z=12 bit=31            ; Message-ANTWORT-Flag (Yes)
0x006e Set z=3  bit=107           ; Tor-offen-Flag
0x0072 Aot_reset 04 00 ...        ; Schalter deaktivieren
0x007c Set 2/7   0x0080 Set 1/27  ; Cutscene-Latches
0x0084 Cut_chg 09                 ; Kamera aufs Tor
0x0086 Se_on 02 0c 00 01 00 ...   ; == Datei-Offset 0xa4e:  Se_on(bank2, SE 12)   Schalter-Klack
0x0092 Sleep 15
0x0096 Se_on 02 0a 00 03 00 ...   ; == Datei-Offset 0xa5e:  Se_on(bank2, SE 10)   FAHRGERÄUSCH
0x00a6 Work_set 03 00             ; Objekt 0 = das Tor
0x00aa-0x00ee Speed_set/For/Add_speed-Rampen  ; das Tor fährt hoch (Y-Anim mit Ruckel-Zyklen)
0x00f0 Se_on 02 0b 00 03 00 ...   ; == Datei-Offset 0xab8:  Se_on(bank2, SE 11)   Anschlag/Stopp
0x0100-0x0124 Sca_id_set/Sca_floor_set 0..4   ; Durchgang kollisionstechnisch öffnen
0x0128 Set 2/7=0, 1/27=0, Cut_auto, Plc_ret, Evt_end
```

Das ist **schalter-getriggert, nicht tür-getriggert** — der alte Audit-Satz in `aot_common.c`
(„die Se_on(2,12)/(2,10) sind die eigenen Cues des Raumskripts") ist damit präzisiert. Der frühere
Fehlbefund „sub02-Se_on waren Nachrichtentext" (HANDOVER §2b) ist **widerlegt**: das strukturtreue
Disassembly ab der Sub-Offset-Tabelle zeigt echte, balancierte Opcodes (For/Next paaren, Endif/Evt_end
landen exakt am Sektionsende 0x13a).

### 4.3 Se_on-Handler — Operanden-Layout (Disasm `LAB_80041624`, ghidra1_V2.txt:151737-151815)

```
lbu a3,0x1(pc)          ; bank
lh  a0,0x2(pc)          ; u16: low = SE-Index, high = FLAGS
lh  a1,0x4(pc)          ; u16: low = Ursprungs-Modus, high = Pool-Index
  mode 0: Ursprung (0,0,0)   mode 1: Spielerblock 0x800aca54 (+0x34/38/3c)
  mode 2: Entity-Pool 0x800acc2c + idx*0x1f4     mode 3: Objekt-Pool 0x800b3f98 + idx*0x94
pos = Ursprung + u16[pc+6]/[pc+8]/[pc+0xa]
a0_packed = (bank<<24) | (id<<16) | flags     ; @0x80041718-80041734
jal FUN_80045024(a0_packed, &pos)             ; pc += 0xc
```

Für die drei Rolltor-SEs ist das FLAGS-Byte (pc[3]) jeweils **0x00** ⇒ `FUN_80045024` läuft
**non-positional** (`andi a0,a0,0xff` @0x80045080; ==0 ⇒ `voll = volr = tone[+2]`). Das pc[4]-Byte
(1 bzw. 3) ist nur der (ungenutzte) Positions-Ursprung. **Kein Pan bei diesen SEs.**

### 4.4 Bank 2 → RDT snd0, SE-Record → Tone → VAG (die vollständige Auflösung)

`FUN_80045024` Bank-Selector (RE15_COMBAT_SE_SUBSYSTEM.md §1, disasm-zitiert): **bank 2 → case 5**
(`j caseD_5` @0x800450f0) = **RDT snd0** (`lw a0,0x8(v0)` @0x80045138, `*(DAT_800ac778+8)`).

`ROOM1130.RDT` snd0: EDT @0x1454 (32 Records), VH @0x1c=**pBAV** @0x14d4 (ps=1, ts=12, **vs=9**),
VB @0x2f74 (VAG-Summe 36432 B). Record → Tone-Entry (`VH+0x820+prog*0x200+tone*0x20`) → VAG:

| SE | EDT-Record (Datei-Offs.) | prog/tone | Tone-Entry: vol, center, shift, note(=+6), fine(=+5) | VAG (1-based) | VAG-Größe | **Original-Rate** | **Original-Dauer** |
|---|---|---|---|---|---|---|---|
| **12** (Klack) | `00 00 95 11` @0x1484 | 0/9 | vol=110, c=104, s=0, note=68, fine=0 → **Δ −36 HT** | 8 | 336 B = 588 smp | 44100·2^(−36/12) = **5512 Hz** | 0,107 s |
| **10** (Fahren) | `00 00 76 16` @0x147c | 0/7 | vol=127, c=107, s=105, note=66, fine=105 → **Δ −41 HT** | 4 | 5920 B = 10360 smp | 44100·2^(−41/12) ≈ **4131 Hz** | **2,51 s** |
| **11** (Stopp) | `00 00 87 16` @0x1480 | 0/8 | vol=113, c=107, s=0, note=67, fine=0 → **Δ −40 HT** | 5 | 2240 B = 3920 smp | ≈ **4375 Hz** | 0,90 s |

Key-On-Kette: `FUN_80045024` → `SsUtKeyOnV(voice, vabId, prog, tone, note=tone[+6], fine=tone[+5],
voll, volr)` @0x8004522c → `note2pitch2(note, fine)` → `SpuVmKeyOnNow(1, pitch)`
(Decompile `RE_15_Quellcode_V2/SsUtKeyOnV.c` Zeilen 83-84; `note2pitch2.c` / `SpuVmKeyOnNow.c` liegen
im Repo unter echten Symbolnamen). Effektive Rate = 44100 · 2^(((note−center) + (fine−shift)/128)/12);
bei SE 10 ist fine==shift (105/105), also exakt −41 Halbtöne.

**Das „Fahrgeräusch" ist by design ein stark heruntergepitchtes VAG**: die −36…−41-Halbton-Keys machen
aus kurzen Samples ein langes, tiefes Motor-Rumpeln. Ohne diesen Pitch ist der Klang nicht als
Rolltor erkennbar.

### 4.5 W3 (RE2-Referenz) — nicht mehr benötigt

Der Mechanismus ist ohne RE2-Umweg direkt belegt. Strukturbefund am Rande: der DO2-Container
(Modell + TIM + eigenes SCD + eigene SE-Bank, Task-Kette @0x80071d30) **ist** das RE1.5-Pendant des
RE2-„Door disp" — im MZD-Build mit leerem Skript ausgeliefert. RE1.5 MZD hat darum, anders als RE2
retail, **keinen türtyp-abhängigen Tür-Sound-Mechanismus in Funktion**.

---

## 5. Port-Ist-Zustand

### 5.1 Tür-Durchgang: still — konform

`aot_common.c:aot_fire_door` / `room_common.c` (Kette Zeilen 152-217) spielen keinen SE; in
`audio_pc.c` existiert kein Tür-Sound. **Deckungsgleich mit dem Original** (§1). Der Port bildet die
(leere) Türsequenz nicht ab — verhaltensgleich, solange DOOR00.DO2 leer ist.

### 5.2 Schalter-Kette: läuft end-to-end — live gemessen

Messlauf (Binary vom 2026-08-02, master):
`RE15_NO_INTRO=1 RE15_GOTO_ROOM=1130 RE15_FORCE_EVENT=2 RE15_SE_DEBUG=1 RE15_MSG_LOG=1
RE15_INPUT_SCRIPT="W6 A1 W1 A1 …"` →

```
[foot] Bank snd0 geladen: 9 VAGs, vh=3104B vb=36432B edt=128B     ; == ROOM1130 snd0 (VAG-Summe exakt)
[scd]  main00: Aot_set 04 (Schalter) + Obj_model_set 0 pos=(-1240,0,16300)  ; Tor UNTEN installiert
[force-event] scd_event_fire(2)
[msg]  room=1130 id=0 … (Yes/No-Choice-FSM parkt den Thread)
[se] SCD Se_on: bank=2 id=12 → SND0
[se] SCD Se_on: bank=2 id=10 → SND0
[se] SCD Se_on: bank=2 id=11 → SND0                                ; Cut 9 aktiv (Cut_chg 09 ✓)
```

Routing `audio_pc.c:2146-2151` + `re15_audio.h:141-151`: bank 2 → `re15_audio_room_se_snd0` ✓
(Original: case 2 → case 5 → snd0 ✓). Bank-0-Skip ist derzeit verhaltensgleich (kein Abrufer, §3).

### 5.3 Die Wiedergabe-Divergenz (der eigentliche Defekt)

`audio_pc.c`:
- `se_play_layers` (541-568): setzt nur `pcm/pos/subpos/volume_q15` — **flat vol 100, „Per-tone
  volume/pan stay the documented faithful-line deferral"** (Kommentar Z. 544).
- Mixer (305-336): **fixes 2×-Downsample** („PSX VAGs are typically 22050 Hz … Phase 4.6.4+ will pull
  real per-sample rates from the VAB tone table for accurate pitch" — Kommentar Z. 310-314). Jede
  Voice läuft mit 22050 Hz, ohne Tone-Pitch.

Folge für die drei Rolltor-SEs (Port 22050 Hz vs. Original §4.4):

| SE | Original | Port | Fehler |
|---|---|---|---|
| 12 | 5512 Hz, 0,107 s, vol 110 | 22050 Hz, 0,027 s, vol 100 | **+24 HT (4×), ¼ Dauer** |
| 10 | 4131 Hz, **2,51 s**, vol 127 | 22050 Hz, **0,47 s**, vol 100 | **+29 HT (5,34×), 1/5 Dauer** |
| 11 | 4375 Hz, 0,90 s, vol 113 | 22050 Hz, 0,18 s, vol 100 | **+28 HT (5,04×), 1/5 Dauer** |

Das aus dem Original bekannte tiefe, sekundenlange Fahrgeräusch wird im Port zu einem kurzen, hellen
Blip — **konsistent mit „fehlt bzw. ist nicht korrekt"**, selbst wenn die Kette feuert.

### 5.4 Nebenbefunde aus der Messung

- `scd_vm.c:1455-1457` + `op_se_on`: deutet die Se_on-Operanden als „+3 volume, +4 pan". Laut
  `LAB_80041624` (§4.3) ist pc[2..3] = id|flags und pc[4..5] = Ursprungs-Modus|Pool-Index. Fürs
  Playback derzeit folgenlos (der Port nutzt beide Bytes nicht), aber das `RE15_SE_DEBUG`-Log
  („vol=0 pan=1") ist irreführend, und die Positional-Semantik (flags-Byte) fehlt dem Port ganz.
- `re15_audio.h:148`: `case 5 → SND1` — Original case 5 = **snd0** (`lw a0,0x8(v0)` @0x80045138).
  Kein Se_on nutzt Bank 5 (Zensus §3), aber die Zeile ist falsch, falls je ein EXE-Pfad mit
  bank 5 portiert wird.
- Harness: Der Yes/No-Confirm der Choice-FSM hängt am ACTION-Button (**Square**; Cross-only-Hämmern
  bestätigte nicht). Und: `RE15_DEBUG_JUMP` + `RE15_AUTOPILOT` interferieren — das Debug-Menü
  konsumiert das Pad byte-true nicht, die Autopilot-Lenk-Bits verstellen den JUMP-Cursor
  (gemessen: Wunsch 1130, Ausführung ROOM1120). Für Messläufe `RE15_GOTO_ROOM` statt DEBUG_JUMP nutzen.

---

## 6. Divergenzen und Fix-Vorschläge

**D1 — Tone-Pitch fehlt im SE-Playback (Hauptdivergenz).**
Original: `SsUtKeyOnV(..., note=tone[+6], fine=tone[+5], ...)` @0x8004522c → `note2pitch2` →
`SpuVmKeyOnNow` (SsUtKeyOnV.c:83-84). Port: fixe 22050 Hz (audio_pc.c:305-336).
*Fix:* `active_sample_t` um einen Q16-Sample-Step erweitern; `se_play_layers` (bzw.
`re15_edt_resolve_layers`-Rückgabe) um note/fine/center/shift des aufgelösten Tones ergänzen;
`step = 65536 * 2^(((note−center) + (fine−shift)/128)/12)` relativ zur 44100-Basis (Mixer läuft
44100; heutiges Verhalten = konstant step 0.5). Exakte Rundung gegen `note2pitch2.c`/`_svm_ptable`
verifizieren (Decompiles im Repo). Betrifft ALLE snd0/snd1/ARMS/CORE-SEs — Footsteps u.a. klingen
heute nur richtig, wo note≈center−12 (halbe Rate) zufällig passt; ein Sweep über alle EDTs nach
|Δ|≠12 lohnt.

**D2 — per-Tone-Volume fehlt.**
Original non-positional: `voll = volr = tone[+2]` (110/127/113 hier); Port: flat 100
(se_play_layers Z. 551). *Fix:* `volume_q15 = tone_vol * 0x4000 / 127` (>>1 wie bisher), zusammen mit D1.

**D3 — Positional-Flags/Pan-Pfad fehlt (hier nicht hörbar).**
flags-Byte (pc[3]) ≠ 0 ⇒ `FUN_80045a64(pos)`-Panning (@0x80045080/@0x800454ec). Die Rolltor-SEs sind
flags=0 — für sie irrelevant; als eigenes Arbeitspaket führen. Dazu Se_on-Operanden-Deutung in
`op_se_on` gemäß §4.3 korrigieren (id|flags, Ursprungs-Modus, Offset-Addition).

**D4 — ADSR nicht emuliert (niedrige Priorität).**
Tone adsr1=0x80ff/adsr2=0x1fc0, mode=0 (kein Loop) — Attack instant; hörbare Wirkung hier gering,
erst nach D1/D2 messen.

**D5 — Doku-Korrektur:** `RE15_COMBAT_SE_SUBSYSTEM.md` §2 „bank 0 … boot" → Bank 0 = DOOR00.DO2-
Tür-SE-Bank, geladen ausschließlich in `FUN_800161e0` @0x800162dc; im MZD-Build von niemandem
abgerufen (Zensus 284/284 bank 2; EXE-Callsites §3).

**D6 — `re15_audio_se_bank_kind` case 5** → SND1 statt snd0 (Original @0x80045138). Heute ohne
Nutzer; korrigieren, bevor EXE-Pfade mit bank 5/2-Konstanten (0x206/0x207/0x208) portiert werden —
die 0x02-Callsites (z.B. `FUN_80035538` case 5 = 0x0207xxxx) gehören auf **snd0**, nicht snd1.

---

## 7. Offen — mit nächstem konkreten Schritt

1. **PSX-Gegenprobe „Tür-Durchgang ist still"** (statisch vollständig belegt, dynamisch noch nie
   gemessen — HANDOVER §2b: „Der Ton ist bis heute NICHT gemessen"). *Schritt:* PCSX-Redux-Lua
   (Skill `re15-pcsx-watchpoint`): Exec-Breakpoint auf `SpuVmKeyOnNow` @0x80055D10 (+ Log von
   `DAT_800b5316` note / `DAT_800b532c` vag), Tor AOT 0 und Schalter-Event durchspielen; erwartet:
   0 Key-Ons am Tor, 3 Key-Ons (VAG 8/4/5, notes 68/66/67) am Schalter.
2. **Echter Spielfluss-Nachweis im Port** (die Messung nutzte `RE15_FORCE_EVENT`): Schalter-AOT per
   ACTION am Tableau feuern (Autopilot von der 1170-Seite kommend; Hof-Hindernis aus HANDOVER §2b
   zuerst lösen) und die Se_on-Kette im selben Log sehen. *Schritt:* `xza:`-Zwischenpunkte in der
   Hof-Route ergänzen, dann `xza` auf (−550,14750) in 1130.
3. **Yes/No-Confirm-Taste byte-true?** Im Port bestätigt Square die Choice-FSM; das MSG-Audit nennt
   CROSS als Dialog-Confirm. *Schritt:* `FUN_80028134`-Yes/No-Zweig (Pad-Maske) disassemblieren und
   gegen `re15_dialog`/`re15_msg_tick` prüfen.
4. **Wozu die 2 Bank-0-SEs in DOOR00.DO2 gedacht waren** (prog0/tone1+2): vermutlich die nie
   aktivierte Türsequenz-Vertonung. *Schritt (niedrig):* Tone-Entries der DO2-Bank decodieren und
   die 2 VAGs anhören/als WAV exportieren — reine Neugier, kein Port-Verhalten.
5. **Pitch-Sweep über alle Banken** nach D1-Fix: Skript, das für jede EDT (snd0/snd1 aller 240 RDTs,
   ARMS, CORE) note−center dumpt und die Port-Abspielrate gegenprüft. *Schritt:* Erweiterung von
   `re15_port/tools/bgm_container_probe.py`-Machart auf die SE-Banken.

---

## 8. Beleg-Inventar (Kurzliste)

- `RE_15_Quellcode_V2/FUN_8001d600.c`, `FUN_800171f4.c`, `FUN_800161e0.c`, `FUN_800164c8.c`,
  `FUN_80016518.c`, `FUN_800166c4.c`, `FUN_800170e0.c`, `FUN_8003edec.c`, `SsUtKeyOnV.c`,
  `note2pitch2.c`, `SpuVmKeyOnNow.c`
- ghidra1_V2.txt: LAB_80016188 (82393-82414), @0x800164b4 + XREF DAT_800b0fde (492950),
  @0x8001720c-80017224 (Dtex-Lookup), DAT_80071d2c (233458), PTR_FUN_80071d30 (233462-233466),
  LAB_800430bc (154098-154111), LAB_800430f0 (154112-154124), LAB_80041624 (151737-151815),
  FUN_80045024-Callsite-Sweep (alle `jal FUN_80045024`), @0x80045080/@0x800450f0/@0x80045138
  (RE15_COMBAT_SE_SUBSYSTEM.md §1, disasm-zitiert)
- PSX.EXE @0x80016664-0x800166c0 (Task 3, selbst disassembliert, Sanity `@0x800171f4=27bdffe8`)
- Datei-Bytes: DOOR/DOOR00.DO2 (Offsets 0x0/0xc/0x18/0x9a4/0x9a8/0x8bc8/0x97f8),
  ROOM1170.RDT (mainScd 0x11f4: Door_aot_set @+0x12/+0x104), ROOM1130.RDT (subScd 0x9c8: sub02 @0xa28,
  Se_on @0xa4e/0xa5e/0xab8; snd0 EDT @0x1454, VH @0x14d4, VB @0x2f74, Tone-Entries @0x1dd4/0x1df4/0x1e14)
- Port: `re15_port/engine/src/aot_common.c`, `room_common.c:152-217`, `scd_vm.c:1260-1352,1455-1476`,
  `re15_port/platform/pc/src/audio_pc.c:305-336,541-586,2131-2152`, `re15_port/include/re15_audio.h:141-151`
- Live-Messungen: debug.log-Läufe 2026-08-02 (GOTO_ROOM=1130 + FORCE_EVENT=2 + SE_DEBUG/SCD_TRACE/MSG_LOG)
