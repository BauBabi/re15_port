# Positional-Audio (FUN_80045a64 / FUN_8005bec0)

All RE questions are answered. Compiling the dossier raw data now.

---

# POSITIONAL-AUDIO GAME-WEIT — RE-DOSSIER (Rohdaten)

## 1) MECHANISMUS

### 1.1 Treiber-Globals (RE1.5 PSX.EXE)

| Global | Bedeutung | Beleg |
|---|---|---|
| `DAT_800b2824` / `DAT_800b2826` | **voll / volr** (SPU-Key-On-Lautstärkepaar, u16, effektiv 0..0x7f) | Stack-Args 24(sp)/28(sp) des `jal 0x80059d3c` (=SsUtKeyOnV) `@0x8004521c-30`: `lh v0,10276 (0x800b2824)` → `sw v0,24(sp)`; `lh v1,10278 (0x800b2826)` → `sw v1,28(sp)`. O32: arg7=voll=24(sp), arg8=volr=28(sp) |
| `DAT_800ac778` | RDT-Blockpointer-Tabelle; `+0x24` = Kamera-Cut-Array (0x20/Eintrag), `+0x14` = snd1-EDT, `+0x8` = snd0-EDT | `@0x80045a68` `lw v1,-14472(v1)`; Katalog Z.107/148 |
| `DAT_800b0fe4` | aktiver Kamera-Cut-Index (lh) | `@0x80045a70` `lh v0,4068(v0)`; Katalog Z.217 („THE active cut index"), Schreiber Cut_chg `@0x800402a0` |
| `DAT_800ac784` | **CURRENT-ENTITY-Zeiger** (die gerade tickende Entity) | Setter: (a) Enemy-Loop `FUN_8001a50c`: `= &DAT_800acc2c`, dann `+= 0x7d` Worte (=0x1F4/Entity), Dispatch `(&DAT_80072bac)[type]()`; (b) Player-Dispatcher `FUN_80031c44`: `= &DAT_800aca54`; (c) AOT-Scan `FUN_80042bac`: `= param_1` (scannende Entity). Der Objekt-/Tür-Loop `FUN_8002bd44` setzt ihn NICHT → Tür-Handler laufen mit dem Player-Wert (Frame-Reihenfolge @0x8001ce04 Enemy → @0x8001ce0c Player → @0x8001ce14 Tür-Loop, Katalog Z.148) |
| Entity-Position RE1.5 | `entity+0x34/+0x38/+0x3C` = x/y/z (s32) | Se_on-Handler `@0x800416e0-e8` `lw a1,52(v1) / lw a2,56(v1) / lw v1,60(v1)`; Footstep `FUN_80045630`: `FUN_800437d4(*(ent+0x34), *(ent+0x3c))` |
| `DAT_800b2420` | 8 Voice-Records à **0x12 Bytes** (SE-Stimmen 16..23): +0 pending, +2 se_id, +4 vabId, +6 prog, +8 tone, +0xA note, +0xC fine, **+0xE voll, +0x10 volr** | Writes `@FUN_80045024` Zeilen 110-118 (`&DAT_800b2422+i*0x12` …), Pumpe s.u. |

**Kamera-Cut-Record (0x20 B):** `+4` cam_x, `+8` cam_y, `+0xC` cam_z, `+0x10` tgt_x, `+0x18` tgt_z (Reads `@0x80045aa0/ae0/ac0` bzw. `@0x80045b80-84`). Identisch zur Port-`re15_camera_cut_t`.

### 1.2 FUN_80045a64 — die komplette Formel (param = int* Emitter-Pos x/y/z)

**Referenz-Kamera = AKTIVER KAMERA-CUT, nicht der Spieler:** `s2 = *(DAT_800ac778+0x24) + DAT_800b0fe4*0x20` (`@0x80045a94-9c`).

**Distanz (byte-true, mit Original-Quirk):**
```
dx  = |cam_x - emit_x|                      @0x80045aa0-b8 (lw 4(s2); subu; bgez/negu)
dz  = |cam_z - emit_z|                      @0x80045ac0-d8
s1  = dx*dx + dz*dz                         @0x80045abc/adc mult, @0x80045ae8 addu
ady = |cam_y - emit_y|                      @0x80045ae0 lw s0,8(s2); @0x80045af4 subu s0,s0,v0; abs @0x80045af8-b00
rxz = SquareRoot0(s1); rxz2 = SquareRoot0(s1)   @0x80045b04 und @0x80045b10 (ZWEIMAL, BIOS-Approx @0x80065f60)
vy  = cam_y - ady                           @0x80045b18 lw v1,8(s2); @0x80045b20 subu v1,v1,s0   ← QUIRK: NICHT dy!
r   = SquareRoot0(vy*vy + rxz*rxz2)         @0x80045b24-38
d   = r / 250  (magic 0x10624dd3, mfhi, srl 4 @0x80045b3c-4c); clamp 0x7f @0x80045b50-5c
```
Der `cam_y - |cam_y-emit_y|`-Term ist im Roh-MIPS bestätigt (Emitter auf Kamerahöhe ⇒ vy=cam_y ⇒ eingebauter Mindestabstand ≈ Kamerahöhe; verhindert praktisch, dass d unter ~10 fällt und der u16-Wrap unten zuschlägt).

**Winkel (vier `FUN_80045d6c`-Aufrufe `@0x80045b60-bc0`):**
```
A = azimut(cam_x,cam_z → emit_x,emit_z)     @0x80045b70
B = azimut(cam_x,cam_z → tgt_x,tgt_z)       @0x80045b88  (Cut-Record +0x10/+0x18)
C = azimut(0,0 → cam_x,cam_z)  (2×)         @0x80045ba0 / @0x80045bb8
rel = ((s16)(A-C) < (s16)(B-C)) ? (A-C)+0x1000-(B-C) : (A-C)-(B-C)   @0x80045bc4-e8
```
**Pan (`@0x80045bf0-ce4`):**
- `(s16)rel ∈ {0, 0x800, 0x1000}` → voll = volr = **0x89** (`@0x80045c0c-1c`).
- sonst: `b1 = ((rel&0x800)==0)`, `b2 = !b1` (`@0x80045c28-3c`); `b3 = ((rel-0x401)&0xffff) < 0x7ff ? b1 : b2` (`@0x80045c40-6c`); `idx = (rel&0x3ff)>>3` (0..127); wenn b3: `idx = 0x89 - idx` (`@0x80045c70-7c`, **läuft bis 0x89 = ÜBER das Pan-LUT-Ende in die Distanz-LUT hinein**);
- `LUT = lbu (0x80074728 + idx)` (`@0x80045c88-94`);
- **`volr(0x800b2826) = b2*LUT + b1*0x89`** (`@0x80045c9c-cd4`), **`voll(0x800b2824) = b2*0x89 + b1*LUT`** (`@0x80045cbc-ce4`). D.h. rel∈(0,0x800) ⇒ links gedämpft ⇒ Klang RECHTS.

**Distanz-Abzug (`@0x80045ce8-d44`):** beidseitig `vol = (vol - lbu(0x800747a8 + d)) & 0x7f` (subu + `andi 0x7f` `@0x80045d08/d28` bzw. `@0x80045d30/d3c` — **Maske, KEIN Clamp**: 0x89−0 = 0x89 → &0x7f = 9).

**LUT-Bytes (PSX.EXE, gedumpt):**
- PAN `@0x80074728` (128 B): `7f×16, 7e×4, 7d×4, 7c×4, 7b 7b 7a 7a, 79 79 … (paarweise fallend) … 5b 5b, 5a 5a 59 58 57 … 3c` (Index 0x7F = 0x3C).
- ATTEN `@0x800747a8` (128 B): `00..19` (linear je 1), dann paarweise `1a 1a 1b 1b … 45 45`, dann `46..53` (Index 0x7F = 0x53 = −83).
- Überlauf-Indizes 0x80..0x89 des Pan-Pfades lesen die ersten 10 ATTEN-Bytes (`00..09`) — deshalb müssen beide Tabellen **eine zusammenhängende** 256-Byte-Tabelle bleiben (im Port bereits so: `rotor_common.c` SE_PAN_ATTEN).

### 1.3 FUN_80045d6c (Azimut) + catan

```
dx = a2-a0; dz = a3-a1
dx==0:  dz<=0 → 0x400, sonst 0xC00          @0x80045dd8-e0 (blez a0; delay ori 0x400 / ori 0xc00)
sonst:  q = (dz<<12)/dx  (signed div)        @0x80045d84-88
        a = catan(q)      jal 0x800658fc     @0x80045db4
        base = (dx>=0) ? 0x1000 : 0x800      @0x80045dc0-c8
        return (base - a) & 0xfff            @0x80045dcc-d4
```
`0x800658fc` = **PsyQ-BIOS `catan`**, 12-Iterationen-CORDIC, Tabelle `@0x80078c90` = {511,302,159,81,41,20,10,5,3,1,0,0} — im Port bereits byte-true als `re15_catan` (re15_math.c:258, inkl. catan(0)=1-Rundung).

### 1.4 SsUtKeyOnV — wie voll/volr in die SPU fließen (`RE_15_Quellcode_V2/SsUtKeyOnV.c`, @0x80059d3c)

Das Paar wird in (Key-Vol, Key-Pan) umgerechnet: `voll==volr` → vol=voll, pan=0x40; `volr<voll` → vol=voll, pan=(volr<<6)/voll; `voll<volr` → vol=volr, pan=0x7f−((voll<<6)/volr). Zusätzlich lädt der Treiber tone[+2]/[+3] (DAT_800b5321/22) — Tone-Attribute und Key-Paar wirken beide in der SpuVm-Stufe (das lineare vm-Modell des Ports, audio_pc.c:715-722, modelliert die Tone-Stufe bereits).

### 1.5 Aufrufer-Zensus — welche SE-Pfade sind positional

**Positional-Schalter in FUN_80045024:** `(param_1 & 0xff) != 0` → `FUN_80045a64(param_2)` (`@0x800451c0-cc`); sonst voll=volr=tone[+2] (`@0x800451dc-f0` `lbu v0,2(s1)`). Ergebnis wird in den Voice-Record (+0xE/+0x10) übernommen und von der Pumpe `FUN_800458d4` (Aufruf `@0x80021488`) unverändert gekeyt → **einmalige Berechnung beim SE-Start, KEIN Per-Frame-Update in RE1.5.**

| Pfad | positional? | Emitter | Aufrufer |
|---|---|---|---|
| **SCD Se_on 0x36** (`LAB_80041624`) | pro Opcode: Byte pc[3] (`a0=lh 2(s0)`, Packing `(bank<<24)\|(id<<16)\|(op>>8)` `@0x80041718-34`) | Klassen-Jumptable `@0x80010d9c`: 0→(0,0,0); 1→Player `0x800aca54`; 2→Enemy `0x800acc2c+idx*0x1F4`; 3→Objekt `0x800b3f98+idx*0x94`; 4/5→`0x800416e0` mit **stale v1** (Bug, ungenutzt). Pos = ent+0x34/38/3C **+ s16-Offsets** pc[6..11] (`@0x800416ec-171c`) | jeder Raum-SCD |
| **FUN_800453d0** (Raum-SE snd1) | **IMMER** (`FUN_80045a64(DAT_800ac784+0x34)` — uint*-Arith `+0xd` = Byte +0x34) | current entity | **335 Overlay-Dateien** (STAGE1-5_full, `func_0x800453d0(0..12)`) + EXE `FUN_80012d60(10)` (Player-Damage; wird der Treffer im Gegner-Tick ausgeteilt, ist der Emitter der GEGNER). id+=12 wenn `*ent & 0x2000` (`@0x80045404-18`) |
| **FUN_80045630** (Footstep) | **IMMER** (`FUN_80045a64(DAT_800ac784+0x34)`) | current entity (Player UND Gegner) | EXE `FUN_80036718` + 5 Overlay-Sites `func_0x80045630(2,0,0)` |
| **FUN_80045024 direkt** | je Wort | s. Wort | EXE: `FUN_80011f50`/`FUN_80035538` `(0x1080001,&DAT_800aca88)` = **GUNSHOT positional an Player-Pos** (0x800aca88 = 0x800aca54+0x34); `FUN_80017fa4` `(0x3030001,&local)` (Fall-Objekt-Landung); `FUN_80036718` `(0x4030001,&DAT_800aca88)`; `FUN_8002c444` `(0x2060000/0x2080000, ent+0x34)` **non-positional** (Flag 0, Pos ignoriert); `FUN_8004a0cc` `(0x40N0000, 0)` UI non-positional. Overlays (alle positional): 11× `(0x4030001, cur+0x34)`, 6× `(0x4010001, cur+0x34)`, 6× `(0x2070001, cur+0x34)`, 2× `(0x4030001, &DAT_800aca88)` |

### 1.6 RE2-EXE-Zwilling (ENEMSE — betrifft den RE2-AI-Modus des Ports)

- **FUN_8005bd6c** (ENEMSE-Player): schreibt in den 0x20-Byte-Voice-Record `DAT_800d4f18+chan*0x20`: +0xE/+0x10 tone[+2] (2×), **+0x12 = 1 (Positional-Flag), +0x14/+0x18/+0x1C = `param_2[0xe]/[0xf]/[0x10]` = entity+0x38/0x3C/0x40** (die „+56/60/64" aus P2; RE2-Entity-Pos, bestätigt durch RE2-Footstep `FUN_8005c040`: Material via `FUN_800528e8(*(ent+0x38), *(ent+0x40), …)`).
- **Pumpe FUN_8005c5e4** (RE2-Zwilling von FUN_800458d4): pro pending Record — Flag+0x12==0 → voll/volr aus Record; sonst **`FUN_8005c970(record+0x14)`** rechnet DAT_800d7598/759a aus der GESPEICHERTEN Emitter-Pos; vabId==1 → +10-Boost (clamp 0x7f); globaler SE-Master `DAT_800e8768` %; Key-On `FUN_8007fdc8(vab,prog,tone,note,fine,voll,volr)`. **Auch RE2: Berechnung einmalig beim Key-On (in der Pumpe), kein kontinuierliches Re-Panning** (Record-Flag wird danach gelöscht; Ausnahme vab==2-Stream, bis 16 Wiederholungen).
- **FUN_8005c970** (RE2-Zwilling von FUN_80045a64) — gleiche Architektur, drei RE1.5-Quirks GEFIXT: (1) vertikaler Term = echtes |dy| (kein cam_y−|dy|); (2) Basis **0x7f** statt 0x89, Pan-Index `rel&0x7ff`, Spiegel bei `&0x400` (0x800−rel), >>3, LUT `@0x800a7fb0` (RE2-EXE); (3) Distanz-Abzug `@0x800a8030[d]` **konditional** (`if (att < vol) vol -= att`) statt &0x7f-Wrap. Zusätzlich RE2-only: **Occlusion** — `FUN_80050858(camPos, emitPos(y−0x5dc), 0x8400, 1)` ≠ 0 → beide × 0x41/100 (65 %); dann clamp 0x7f. Kamera analog `*(DAT_800ce324+0x24) + DAT_800d4820*0x20`.

## 2) PORT-STAND

- **`re15_port/engine/src/rotor_common.c` — es existiert bereits ein FUN_80045a64/45d6c-Replikat** (`re15_rotor_compute_pan`, mit @-Zitaten inkl. cam_y−|dy|-Quirk (Audit wf_8cc15b53), kontiguierlicher SE_PAN_ATTEN[256]-Tabelle Byte-identisch zu meinem Dump, /250, &0x7f-Maske). Konsumenten (PC audio_pc.c:2713 / PSX audio_psx.c:755-775 Rotor-Update) sind stillgelegt — die Funktion ist frei für den echten SE-Pfad. **Zwei belegte Abweichungen darin:** (a) `rotor_azimuth` nutzt ein eigenes ATAN256-LUT statt der CORDIC — `re15_catan` (re15_math.c:258, byte-true @0x800658fc, catan(0)=1) weicht davon ab; (b) **L/R vertauscht**: rotor_common.c:138-139 `panL = b2 ? LUT : 0x89` — das Disasm sagt `voll(0x800b2824) = b2*0x89 + b1*LUT` (`@0x80045cbc-ce4` + Stack-Arg-Beweis `@0x8004521c-30`), also panL müsste bei b1 das LUT tragen.
- `re15_port/platform/pc/src/audio_pc.c`: SE-Maschine komplett (Gate FUN_80045a18, Records, Pumpe se_voice_pump :771, Direkt-Zweig :732) — nur vol/pan ist tone-basiert: se_play_layers :714-722; dokumentierte OFFEN-Blöcke :670-686 (Raum-SE), :943-947 (CORE), :961-969 (Weapon), :1109-1110 (RE2-ENEMSE); Event-Konsument :2846-2867 wirft flags/origin/pos weg.
- `re15_port/engine/src/scd_vm.c` op_se_on :1643-1669: dekodiert bank/id/FLAGS(pc[3])/Origin-Modus(pc[4])/Pool-Index(pc[5])/LE-s16-Offsets korrekt in evt (raw_w0/raw_w1/pos_x/y/z), berechnet aber nichts.
- `re15_port/include/re15_scd.h` :161-169 scd_audio_event_t (volume/pan-Felder u8 vorhanden).
- Engine-Callsites ohne Emitter-Kontext im Signatur: enemy_ai_common.c (136), player_common.c (8), game_step_common.c (7), re15_damage.c (6), enemy_ai_re2_zombie.c (5), menu_common.c (2), re15_itembox.c (1); API `re15_port/include/re15_audio.h` :107/:122/:126/:166/:170.
- Kamera-Cut: engine hat `re15_camera_cut_t` mit pos/target; game_step_common.c:1497-1508 zeigt das Muster (eye/tgt aus active_cut).

## 3) IMPLEMENTIERUNGS-PLAN

1. **`rotor_common.c` → generischer SE-Rechner:** `re15_se_compute_pan(cam_eye, cam_tgt, emit, &voll, &volr)` (Umbenennung/Alias von re15_rotor_compute_pan). Dabei (a) `rotor_catan` durch `re15_catan` ersetzen (Beleg: `jal 0x800658fc` @0x80045db4); (b) L/R gegen das Disasm richten: `voll = b2*0x89 + b1*LUT`, `volr = b2*LUT + b1*0x89` (@0x80045cbc-ce4) — vorher per PSX-Hardware-/DuckStation-Capture einer klaren Rechts-Quelle gegenprüfen (§4).
2. **Engine-Global „current emitter"** (Zwilling von DAT_800ac784): `re15_se_emitter_set(const int32_t pos[3])` — gesetzt (wie das Original) vom Enemy-AI-Dispatch (pro Actor), vom Player-Tick und vom AOT-Scan; Tür-/Objekt-Loop lässt den Player-Wert stehen. Damit bleiben alle 165 `re15_audio_room_se(id)`-Sites signaturstabil.
3. **Berechnung am Original-Ort = beim SE-Start** (nicht in der Pumpe, nicht per-frame — FUN_80045024/453d0/45630 rechnen sofort, Record speichert voll/volr): 
   - `re15_audio_room_se` / `_snd0` / `_weapon_se` / `_footstep`: IMMER positional mit current emitter (Raum-SE/Footstep) bzw. Player-Pos (Gunshot 0x1080001, EXE-Beleg oben); non-positionale EXE-Parity-Calls (UI FUN_8004a0cc, FUN_8002c444) behalten den Tone-Zweig — am saubersten über eine `_at(id, pos)`-Variante + Wrapper.
   - op_se_on (scd_vm.c): Origin auflösen (Klasse 0/1/2/3 → (0,0,0)/g_actors[0]/g_actors[idx]/Objekt-Pool; 4/5 wie Original ungenutzt → non-positional lassen und loggen), Offsets addieren, wenn pc[3]!=0 → voll/volr via re15_se_compute_pan mit dem AKTIVEN Cut (eye/tgt) berechnen und in evt.volume/evt.pan (bzw. zwei neuen u8-Feldern vl/vr) mitgeben; Konsument audio_pc.c:2861-2867 reicht sie an se_play_layers durch.
4. **se_play_layers:** optionales (voll,volr)-Paar; wenn gesetzt: `vl = voll*0x4000/127>>1`, `vr = volr*0x4000/127>>1` ERSETZT den tone[+2]-Zweig (das ist exakt der Original-Unterschied: Key-Paar statt tone[+2]/pan 0x40); Tone-Pan-Stufe wie bisher zusätzlich anwenden (SsUtKeyOnV lädt tone[+3] in beiden Fällen). Gleiche Werte in die s_se_pend-Records (Pumpe unverändert).
5. **RE2-Modus (re15_audio_re2_enemy_se):** Actor-Pos (entity+0x38/3C/40-Äquivalent) durchreichen; RE2-Formel als eigene Variante (Basis 0x7f, `rel&0x7ff`/`&0x400`-Spiegel, LUTs @0x800a7fb0/@0x800a8030 aus ghidra_re2_Leon.txt extrahieren, konditionaler Abzug, vab1-Boost+10, Master-%); Occlusion (FUN_80050858-Ray) zunächst als benannte Lücke.
6. **Verifikation:** (a) Unit-Test: Kamera/Emitter-Tripel aus einem ROOM1140-Savestate → voll/volr gegen von Hand aus dem Disasm gerechnete Werte; (b) dynamisch: DuckStation-Savestate mit laufendem Gegner-SE → `DAT_800b2824/26` (0x800b2824) direkt auslesen und mit dem Port-Rechner für dieselbe Kamera/Emitter-Lage vergleichen (Skill re15-savestate-ghidra); (c) hörbar: ROOM1140, Zombie links/rechts der Blickachse, Kanalvergleich per Capture.

## 4) OFFEN (ehrlich)

- **L/R-Zuordnung final:** Disasm ist eindeutig (voll=0x800b2824=b2·0x89+b1·LUT), aber rotor_common.c behauptet, ein früherer „Mirror-Bug" sei genau andersherum gefixt worden. Da beide Rotor-Konsumenten stillgelegt sind, gibt es keinen hörbaren Ist-Zustand als Referenz. **Nächster RE-Schritt:** DuckStation-Capture (Media-Capture, DIGITAL-Pad) eines Gunshots mit Emitter klar rechts der Blickachse → welcher Kanal trägt 0x89−att. Bis dahin gilt das Instruktions-Zitat.
- **SpuVm-Kombination Tone-Pan × Key-Pan:** SsUtKeyOnV lädt tone[+2]/[+3] (DAT_800b5321/22) UND das Key-Paar (DAT_800b5318/19); die exakte Mischformel liegt in der SpuVm-Stufe (VMANAGER_OBJ_441C/SpuVmKeyOnNow) und wurde hier nicht zu Ende disassembliert. Port-Annahme: zwei multiplikative lineare Stufen (bestehendes vm-Modell). Nächster Schritt: `SpuVmKeyOnNow`/`_svm`-Volumenpfad im PsyQ-Treiber-Decompile (echte Symbolnamen, grep nach „SpuVm" statt FUN_) lesen.
- **RE2-LUT-Bytes** @0x800a7fb0/@0x800a8030 noch nicht extrahiert (nur Existenz + erster Wert 0x7f belegt, ghidra_re2_Leon.txt:332923); Extraktion aus dem Dump ist mechanisch.
- **RE2-Occlusion FUN_80050858** (Ray-Test, Maske 0x8400) nicht disassembliert — nur Aufruf + Wirkung (×65 %) belegt.
- **Se_on-Klassen 4/5** springen auf `0x800416e0` mit nicht initialisiertem v1 (stale Register) — im Auslieferungs-SCD-Bestand vermutlich ungenutzt; ein Census über alle 240 RDT-SCDs (welche pc[4]-Werte vorkommen) steht aus.
- **STAGE6-Overlay** ist nicht decompiliert (0 Dateien) — der 453d0-Zensus deckt STAGE1-5 ab; STAGE6-Callsites müssten per jal-Scan auf STAGE6.BIN gezählt werden (`jal 0x800453d0` = Opcode `0x0C0114F4`).
