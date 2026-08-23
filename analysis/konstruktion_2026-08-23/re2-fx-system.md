# RE2-FX-System 0x8001bf10

Alle RE-Ziele sind erreicht. Hier das Dossier-Rohmaterial.

---

# RE-DOSSIER: RE2-FX-SYSTEM `0x8001bf10` (RE2-Leon-EXE) — Effekt-Kinds, Dispatcher, Per-Frame-Modell, Sprite-Quellen

Quellen dieses Dossiers: `RE2_Quellcode_V2/FUN_8001{bba4,bca0,bd38,bf10,c224,c56c,c8c4,cbe8,cefc,cfbc,d07c,d0ac,d300,d68c,d894,dc28,dc30,dd2c,ed9c}.c`, `FUN_80021a48.c`, `FUN_80077924.c`, `FUN_80077ed0.c`, `ghidra_re2_Leon.txt` (Zeilen 94876–95394, 291535–291643, 517568–517906, 610667), eigene MIPS-Disasm über `info/re2leon/PSX.EXE` (t_addr `0x80010000`, +0x800 Header) und `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` (Base `0x80100000`), eigene Parser über `info/re2leon/COMMON/DATA/CORE00.ESP` und alle 250 `info/re2leon/PL0/RDT/ROOM*.RDT`. Scratch-Skripte: `<scratchpad>/mips_dis.py`, `esp_census.py`, `esp_dump.py`, `rdt_esp_dump.py`.

## 1) MECHANISMUS

### 1a. a0-Wort-Dekodierung (Spawner `FUN_8001bf10`)

Disasm-Beweis (ghidra_re2_Leon.txt:95350–95368):
```
8001bf14  srl  t6,a0,0x18          ; BANK   = a0 >> 24
8001bf1c  srl  v0,a0,0x10
8001bf20  andi t5,v0,0xff          ; SUB    = (a0 >> 16) & 0xFF
8001bf24  andi v0,t5,0x7           ; SCRIPT = SUB & 7
8001bf28  sll  v0,v0,0x1
8001bf34  lw   t4,DAT_800d4e18(at) ; Script-Tabelle der Bank
8001bf40  lhu  v0,0x0(v0+t4)       ; ushort-Offset (in WORTEN)
8001bf4c  lw   t1,DAT_800d4cd8(at) ; Sprite-Bank-Daten
8001bf50  sll  v0,v0,0x2           ; Offset * 4 = Byte-Offset
```

**a0 = `BANK<<24 | SUB<<16 | PARAM`** mit:
- **Bits 24–31 = BANK (ESP-Id, 0–63)**: Index in zwei EXE-Zeigertabellen `DAT_800d4cd8` (Sprite-Bank) und `DAT_800d4e18` (Script-Tabelle der Bank), je 64 Einträge (Invalidierungs-Schleife `addiu v1,zero,0x40` @0x8001bb08, eigener Disasm).
- **Bits 16–23 = SUB**, zweigeteilt:
  - `SUB & 7` = Script-Index 0–7 in der Bank (@0x8001bf24),
  - `SUB >> 3` = **CLUT-Zeilen-Offset**: `clut = bank_hdr[2] + (SUB>>3)*0x40` (FUN_8001bf10.c:62 — `*(&UNK_800d8d22+i) = uVar3 + (short)(uVar15>>3)*0x40`; +0x40 = eine VRAM-CLUT-Zeile tiefer). **Das ist der Farbvarianten-Kanal**: Zombie-Blutfarbe = `(entity_flags>>0x13)&0x18` als SUB (FUN_80046304.c:290 → CLUT-Zeile 0–3), Sabber = `SUB 0x15` = Script 5 + CLUT-Zeile 2 (grün statt rot), Säure-Spray `0x040F0FA0` = Bank 4, Script 7, CLUT-Zeile 1.
- **Bits 0–15 = PARAM = Q12-GRÖSSEN-SKALIERUNG des Sprites.** Beleg: Spawn schreibt `a0<<16` nach Slot+0x38 (FUN_8001bf10.c:56, PARAM landet als u16 in **+0x3a**), und der Packet-Builder rechnet `size = anim_size * *(u16*)(slot+0x3a) * zoom` (**FUN_80077ed0.c:32**: `param_5 = param_3 * *(ushort*)(param_2+0x3a) * param_5`), danach perspektivische Division durch GTE-`sz` (FUN_80077ed0.c:52-53). 0x1000 = 4096 = 1.0; die Caller-Werte 8096/6096/3024+rng*8/0x2800/0x2c8 sind schlicht ~1.98×/1.49×/0.74×/2.5×/0.17×.

Weitere Argumente: **a1 (u16) = Y-Rotationswinkel 0–4095** der Partikel-Lokalbewegung (Slot+0x22; `RotMatrixY((int)*(short*)(i+0x22),…)` FUN_8001d894.c:49). **a2 = MATRIX\*** (Eltern-Transform; 32 B kopiert nach Slot+0x4c, Zeiger nach +0x6c). **a3 = SVECTOR\* Offset** im Elternraum oder NULL (Slot+0x2c).

Spawner-Familie (alle identischer Kern, Pool-Scan rückwärts 0x60 Slots):
| Funktion | Unterschied | Beleg |
|---|---|---|
| `FUN_8001bf10(a0,yaw,mtx,ofs)` | Standard, Status `0xa003` | :39 |
| `FUN_8001c8c4(a0,(id<<0?/yaw-Pack),mtx,ofs)` | zusätzlich **Owner-Id-Byte** → Slot+0x1d (`(param_2&0xff)*0x100` :41), yaw = `param_2>>16` | für Kill-by-Owner `FUN_8001cfbc` |
| `FUN_8001c224(a0,yaw,mtx,ofs,extra)` | +6-Byte-Extra `*extra`→+0x44, `extra[1]`(u16)→+0x48 | :88–90 |
| `FUN_8001c56c` | = c8c4 + extra | :42–45, 89–91 |
| `FUN_8001cbe8` | Status **`0x4000`** statt 0xa003 = „deferred": wird im nächsten Pump auf 0xa003 promoted und tickt dann (FUN_8001d300.c:27–32) | :38 |

Rückgabe: Slot-Index; `0xff` = Pool voll, `0xfe` = voll beim Ketten-Part. **Mehrteilige Scripts**: `script_hdr[0]` = Part-Anzahl; Parts 1..n−1 bekommen eigene Slots (Kopie der 0x60 Bytes ab +0x18 + Script-PC des jeweiligen Parts), Eltern-Slot-Index in Kind+0x1a; stirbt der Elternteil, killt der Pump die Kinder (FUN_8001d300.c:15–20).

### 1b. Bank-Quellen, Registrierung, „Kind"-Namensraum

**Es gibt KEINE kind→Routine-Tabelle in der EXE.** Ein „Kind" (= Bank = ESP-Id) ist reine **DATEN**-Adresse; Verhalten kommt aus dem Script der Bank (s. 1d). Die EXE-Tabelle `@0x8009d868` ist die **Script-OPCODE-Tabelle** (96 Einträge `0x00–0x5F`, ghidra_re2_Leon.txt:291548–291643, Ende bei `DAT_8009d9e8`).

**Registrierung** — einziger Schreiber beider Tabellen ist `FUN_8001bca0(idlist, off_end, base, slot)` (XREF[8]-Liste 517568 ist vollständig):
- liest bis zu 8 Id-Bytes ab `idlist` (0xFF = Ende), trägt sie ins Registry `DAT_800eae48[16]` ab `slot` ein,
- Offsets werden **RÜCKWÄRTS** ab `off_end` gelesen (`param_2 = param_2 + -1`, FUN_8001bca0.c:18),
- `DAT_800d4cd8[id] = base+off`; `DAT_800d4e18[id] = bank + (n1*2 + n2 + 2) Worte` (FUN_8001bca0.c:23–24).

**Zwei Quellen:**
1. **CORE-Bänke 0–7 (Boot, Registry-Slots 0–7)** — von Ghidra NICHT decompiliert; eigener Disasm der Roh-Region (Fund über EXE-Byte-Scan nach `jal 0x8001bca0` = `28 6f 00 0c`, zweiter Treffer file 0xc38c):
   ```
   8001babc  (FX-Boot-Init)
   8001bac4-e4  Pool-Clear: 0x60 Slots, Status @0x800d8d08+i*0x7c := 0
   8001bae8-b04 Registry DAT_800eae48[0..15] := 0xFF
   8001bb08-34  BEIDE Tabellen komplett (64 Einträge) := -1  (0x800d4cd8-4dd4, 0x800d4e18-4f14)
   8001bb3c-50  jal 0x80012fb8(a0=0x90, a1=0x801fc400, a2=1, a3=0x800018cc)  ; Datei-Load
   8001bb54-88  a1 := 0x801fc400 + align4(size) - 4   ; = LETZTES WORT der Datei
   8001bb8c     jal 0x8001bca0(0x801fc400, a1, 0x801fc400, 0)   ; slot=0!
   ```
   Die geladene Datei ist **`COMMON/DATA/CORE00.ESP`** (8572 B): beginnt mit exakt 8 Id-Bytes `03 05 00 01 02 06 07 04` (nur diese Datei im ganzen RE2-Baum enthält die Sequenz), und der Rückwärts-Walk ab dem letzten Wort ergibt die konsistenten Bank-Offsets: id 3→+0x8, 5→+0x5f0, 0→+0x88c, 1→+0x11b0, 2→+0x1784, 6→+0x19c0, 7→+0x1b58, 4→+0x1bcc. CLUT/TPage der CORE-Bänke sind **vorgebacken** (z. B. Bank 0: clut 0x7a11 = VRAM(272,488), tpage 0x1f; Pixel: `CORE01.TIM`, 64 KB), es gibt hier keinen `FUN_8001bd38`-Aufruf.
2. **Raum-Bänke (Room-Load, Registry-Slots 8–15)** — `FUN_8001bba4` (aus Room-Setup `FUN_80049e48`:126, `DAT_800ce324` = RDT-Basis): invalidiert nur Slots 8–15, registriert dann `*(RDT+0x50)` (ESP-Block, beginnt mit 8 Id-Bytes) mit Offset-Array-Ende `*(RDT+0x54)`, und lädt die Sprite-TIMs via **`FUN_8001bd38(*(RDT+0x5c), *(RDT+0x58))`**: 8 TIMs rückwärts, VRAM-Stapelung ab TPage-Spalte x=15*64 abwärts, CLUTs ab (0x120·16? — `GetClut(0x120,y)` mit y ab 0x1e0), patcht `bank_hdr[2]=GetClut`, `bank_hdr[3].lo=GetTPage` und **rebasiert das V-Byte jedes UV-Rects** (`pcVar4 = bank + n1*8 + 9; *pcVar4 += stack_y; pcVar4 += 4` FUN_8001bd38.c:59–64).

**Bank-Binärformat** (Byte-Offsets, verifiziert an CORE00.ESP + ROOM1090.RDT):
```
+0  u16 n1        Anim-Einträge
+2  u16 n2        UV-Rects
+4  u16 clut      (CORE vorgebacken / Raum beim Load gepatcht)
+6  u16 tpage     (dito; nur Low-Byte)
+8  n1 × 8 B      Anim-Eintrag {cell u8, nprim u8, dauer u8, size u8, 4×0}
                  dauer 0 & cell 0 = ENDE(kill); dauer 0xFF = LOOP zu cell (FUN_8001d68c.c:38–50)
+8+8n1  n2 × 4 B  UV-Rect {u u8, v u8, cx s8, cy s8}  (cx/cy = signierte Zentrums-Offsets,
                  FUN_80077ed0.c:81/86 `(char)pbVar12[2/3]`; Breite in Pixeln = anim.size)
+8+8n1+4n2        Script-Tabelle: 8 × u16 Offset (in WORTEN ab Tabellenstart) → Scripts
Script:  {u16 nparts, u16 ?}, je Part {u16 nsteps, u16 ?}, je Step 24 B (s. 1d)
```

**Kind-Census (250 Leon-RDTs, eigener Scan)** — Raum-Ids 0x08–0x30; die häufigsten: 0x09 (123 Räume), 0x08 (94), 0x0A (94), 0x16 (72), 0x0B (55), 0x1D (35), 0x1A (31, Wasser — die EXE-Caller `FUN_8005c040` Wat-Splash / `FUN_80046304` Tropfen nutzen 0x1a und sie steht auf der Cutscene-Whitelist), 0x1C (24), 0x15 (23), 0x19 (22), 0x0C **Feder** (8 Räume, u. a. ROOM1090), Rest ≤ 12 Räume.

### 1c. Dispatcher: Overlay-Tabelle `@0x801056AC` vs. EXE-Tabelle

Die Tabelle liegt im **HUNDE-Modul** `EMD0G_MOD0.BIN` (Base 0x80100000; im Auftrag stand „Zombie-Overlay" — das Zombie-Modul `EMZ0.BIN`≡`EMOVL10_S*.BIN` hat KEINE Tabelle, sondern 88 inline `jal 0x8001bf10`-Stellen, eigener Byte-Scan). Format 6 B/Eintrag `{kind u8, sub u8, spread u16, param u16}` — Bytes @file 0x56ac stimmen 1:1 mit `re2d_fx_tbl` in `enemy_ai_re2_dog.c:312`.

Overlay-Spawner `0x80105070` (kompletter eigener Disasm):
```
80105090  lbu v0,543(a3)       ; +0x21F Budget-Gate
801050bc  0x801056ac + fx*6    ; Tabelleneintrag
801050c4-d0  fx==4 → Ziel = SPIELER 0x800cfbf8, sonst der Hund
801050fc  a3 = *(ziel+0x198) + (part&0x7f)*0xAC   ; Part-Datensatz
80105100  lhu a1,2(s1)         ; spread
80105104  lh  a0,118(s0)       ; Ziel-Yaw +0x76
8010510c-20  a1 = yaw + (rand & (2*spread-1)) - spread
8010511c/24/28  kind = tbl[0];  a0 = kind&0x7f;  flag = kind&0x80
flag GESETZT (0x85/0x84 = Gore/Säure):
  80105130  a2 = part+0x48            ; DIE PART-MATRIX SELBST → Effekt FOLGT dem Körperteil
  8010513c-48  a3 = {0,0,0}
flag CLEAR (Blut/Spritzer):
  80105150-54  a2 = 0x8009db44        ; IDENTITÄTS-MATRIX
  8010514c-70  a3 = Translation der Part-Matrix (+0x5c/+0x60/+0x64) ; frei an Part-Position
80105174-88  a0 = (kind&0x7f)<<24 | tbl[1]<<16 | tbl[4..5];  jal 0x8001bf10
8010518c-98  Budget +0x21F--
```
**Das 0x80-Flag = „an Part-Matrix ANHEFTEN" vs. „frei an Part-Weltposition"** — nicht Spawner-Variante. (Korrektur zum Port-Kommentar in enemy_ai_re2_dog.c:301, der das Flag nicht auflöste.)

Die Krähe (EMOVL21) ruft `0x0C000800` (Feder, a1 = rand<<4) @0x801046EC-F4; der Zombie baut a0 inline (z. B. `6096` @0x8010567C, `8096` @0x801096E8/0x80109b40/0x8010866C, `0x040F0FA0` @0x80108740-44, `3024+rng*8` @0x8010B650-5C — alle bereits im Port dokumentiert).

**EXE-Caller-Beispiele** (RE2_Quellcode_V2): `FUN_80046304`:164/290/308 Waffen-Treffer (Bank 0x1a Wasser + Bank 0 Blut mit Farb-SUB aus Entity-Flags, Matrix = `entity[0x66]+part*0xAC+0x48`), `FUN_80040350`:79-118 Funken 6× `0x070002c8`, `FUN_80042aac` Bank 0x1a `0x818+rand*4`, `FUN_8005c040` Wat-Splash Bank 0x1a mit Yaw±0x400/0xC00, `FUN_80053394` Bank 5 `(uVar2*8+7)<<16|0x5002800`, `FUN_80028ad8` Bank 0 `sub=(flags>>0x13&0x18)`, param 2000.

### 1d. Per-Frame-Modell (was eine FX-Instanz tut)

**Pool**: Basis `0x800d8cf0`, **Stride 0x7c**, **0x60 Slots** (Ende 0x800dbb70). Slot-Layout (alle Offsets aus den Spawnern + FUN_8001d68c/d894/77924/77ed0 kreuzverifiziert):
```
+0x00 24 B   AKTUELLER SCRIPT-STEP (Cache):
   [0] u8  Opcode A (Update-Phase)        [1] u8  Opcode B (Draw-Phase)
   [2] u8  Anim-Startindex                [3] u8  op-spezifisch
   [4] u16 X-Aspekt Q12, [6] u16 Y-Aspekt Q12 (FUN_80077ed0.c:60/65)
   [8][9][A] s8 Beschleunigung x/y/z      [B] s8  Step-Delta (Transition)
   [C][E][10] s16 Geschwindigkeit x/y/z
   [12] u16 STATUS des Steps  [14] u16 TPage-OR (Blend)  [16] u16 Random-Range (Op 2)
+0x18 u16 Status   +0x1a u8 Eltern-Slot   +0x1c u32 Id {bank, owner, sub, 0}
     (+0x1f = laufender Step-INDEX, FUN_8001dc30.c:24)
+0x20 u8 Anim-Countdown  +0x21 u8 Anim-Index  +0x22 u16 Yaw(a1)
+0x24/26/28 s16 Lokalposition   +0x2a u16 TPage   +0x2c SVECTOR Offset(a3)
+0x32 u16 CLUT   +0x34/36/38 s16 Weltposition   +0x3a u16 PARAM(a0-low16)
+0x3c/3e/40 s16 Weltposition Vorframe   +0x42 u16 =1 (Spawn-Init 0x10000)
+0x44 u32 / +0x48 u16  Extra (c224/c56c)   +0x4c MATRIX-Kopie  +0x6c MATRIX*
+0x70 Anim-Einträge*  +0x74 UV-Rects*  +0x78 Script-Part-Basis*
```
**Status-Bits** (belegt): `0x8000` lebendig (Pump-Gate d300:14), `0x4000` deferred (cbe8; Promotion d300:27–32 `[0x18]=3,[0x19]=0xa0`), `0x2000` sichtbar (Draw-Gate `(s&0xa000)==0xa000` FUN_80077924.c:47), `0x1000` semitransparent (Prim-Code 0x2c→0x2e :53–56), `0x0800` Matrix jeden Frame neu kopieren = folgen (d894:18), `0x0400` Offset im Elternraum statt Welt (d894:70), `0x0200` Alternativ-Draw-Pfad (77924), `0x0002` Physik, `0x0001` Anim.

**Frame-Pump `FUN_8001d300`** (XREF: einziger Caller 0x80026980 = Hauptschleife). Global-Gate `DAT_800cfbdc & 0x10000000` (== der RE15-Pause-Flag-Wert): wenn gesetzt, ticken NUR Bänke {1, 0x15, 0x16, 0x1a, 0x1c, 0x28} (d300:45–46/61) — Umgebungs-FX laufen in Cutscenes weiter.
1. **Update-Pass**: je Slot mit Bit 0x8000: Waisen-Kill (Eltern-Status==0), dann `PTR_FUN_8009d868[step[0]]()` — **Opcode A**.
2. **Draw-Pass**: 0x4000-Promotion + Opcode A einmalig; dann je lebendigem Slot `FUN_8001d68c(0)`:
   - `FUN_8001d894`: Welt-Transform. Normalmodus: `welt = M.t + M.rot·offset + RotY(a1)·lokalpos`; 0x400-Modus: `welt = M.rot·(basis·lokalpos + offset) + M.t`. Vorframe-Position wird nach +0x3c gerettet.
   - `PTR_FUN_8009d868[step[1]]()` — **Opcode B** (Spezial-Render/Zusatzlogik).
   - **Physik** (Bit 2): `pos += vel; vel += acc` (s16/s8, d68c:19–32).
   - **Anim** (Bit 1): Countdown; bei 0 nächster Anim-Eintrag; `dauer==0 && cell==0` → **KILL**; `dauer==0xFF` → Loop zu `cell` (d68c:33–53).
3. **Standard-Billboard-Draw** läuft separat im Render-Frame: **`FUN_80077924`** iteriert den Pool rückwärts, Sichtbarkeitstest `FUN_8002c820`, holt Anim-Eintrag (`cell`, `nprim`, `size`), Prim-Code 0x2c/0x2e, 40-B-POLY_FT4-Pakete via **`FUN_80077ed0`**: GTE-`rtps` auf Weltpos, OT-Einsortierung bei Bucket `sz>>5` (:42), Bildschirm-Halbgröße = `size·PARAM·zoom / (sz<<4)` (:32/52-53), UV aus Rect `cell..cell+nprim-1` (nprim>1 = mehrere Sprites pro Instanz, z. B. Funken-Bank 7 nprim=4), CLUT aus +0x32, TPage aus +0x2a. Spiegel-Boden-Modus (`DAT_800cfbd8 & 0x10000`) zeichnet einen zweiten reflektierten Satz (77924:74–100).

**Script-Steps sind PHASEN, keine klassischen Befehle**: Opcode A läuft JEDEN Frame, bis er selbst weiterschaltet. Kern-Transitionen: **Op 1 `FUN_8001dc30`** = Status←step[0x12], Anim-Start←step[2], TPage|=step[0x14], Step-Index += step[0xB], 24 B nachladen (LWL/LWR-Kopie); **Op 2 `FUN_8001dd2c`** = wie Op 1, aber Anim-Start = step[2] + rand%(step[0x16]+1). **Op 0 = NOP** (FUN_8001dc28 — reiner Ballistik-Frame). Opcodes können vollwertige Spiellogik sein: **Op 0x0F `FUN_8001ed9c`** = Granaten-Projektil (Bodenhöhe `FUN_800527b4`, Kollision `FUN_8004fba0`, **teilt Schaden aus**: `FUN_800470c0(pos, yaw+0x22, …, step[0x1b]+0x30009)`); **Op 0x36 `FUN_80021a48`** = Wassertropfen-Aufprall (SE `FUN_8005ba28(0x3040001)`, spawnt Kind-Effekt `FUN_8001cbe8(0x030F2800)` = Bank 3 Ring, patcht eigenen Step um). Kill-APIs: `FUN_8001cefc(bank,sub[,mtx])`, `FUN_8001cfbc(owner)`, `FUN_8001d07c()` alle, `FUN_8001d0ac` = Step-Byte[0] pokes (Loop-Release).

### 1e. TOP-Kinds (Sprites + Scripts, aus den Daten gedumpt)

| Kind | Quelle/Offset | Inhalt (verifiziert) |
|---|---|---|
| **0 Blut-Tropfen/-Puff** | CORE00.ESP+0x88c; clut 0x7a11, tpage 0x1f; 36 Anim, 32 Rects | sub1 (Hund `0x00011000`): 3 Parts ballistisch, vel (50,−90,0)/(50,−70,±14), Gravitation acc.y=+10, Status 0xb803; sub0 (Zombie 6096/8096): wie sub1 mit Vorschalt-Step; sub2/3: 9-Part-Burst mit Boden-Splat (Op 4→Op 6, Anim 20); sub5 = Sabber-Script (Status 0xb003, Op-B 0x10), via SUB 0x15 mit CLUT-Zeile+2 |
| **9 Blut-Spritzer** | Raum-ESP (123 Räume; z. B. ROOM1000) | Hunde-Kinds 1–3 = `0x0900_0800|sub<<16`, spread 0x200 |
| **4 Gore-/Säure-Spray** | CORE00.ESP+0x1bcc; 37 Anim, **163 Rects**, tpage 0x1e | `0x040F0FA0/0x040F1770`: Script 7, CLUT-Zeile+1; Scripts nutzen Op 0x0b/0x0c/0x49 (Taumel-Ops), Blend 0x20/0x40/0x60 |
| **5 Gore-Brocken** | CORE00.ESP+0x5f0 | Zombie `0x0503_2710/1388/13E8` (Script 3), Hund `0x85→Bank5` sub3/4 **an Part-Matrix angeheftet** |
| **7 Funken** | CORE00.ESP+0x1b58; 2 Anim, 4 Rects (32×32 @v=208) | Anim `{cell0, nprim=4, dauer 6, size 0x20}` = 4 Sprites/Instanz; Explosion `FUN_80040350` spawnt 6 Instanzen `0x070002c8` |
| **0x0C Feder** | Raum-ESP (ROOM1090+0x62b4); 29 Anim, 16 Rects 40×40 | Script 0: EIN Step, Op A **0x22** = Feder-Flatter-Physik (op-spezifische Params), Op B 0x10; Krähe: a1 = rand<<4 |
| 1 Blut-Lachen/-Splats | CORE00.ESP+0x11b0 | Op-B 7 (Boden-Quad), Op-B 9 (wachsende Lache, op-spezifische Params), Op 8, Op-B 0x14 |
| 3 Wasser-Ring | CORE00.ESP+0x8 | von Op 0x36 als Kind gespawnt |
| 2/6 | CORE00.ESP+0x1784/+0x19c0 | vorhanden, nicht vertieft |

## 2) PORT-STAND

- **Stand-in-Trichter**: `re15_port/engine/src/enemy_ai_re2_zombie.c:915–926` — `re2z_blood_fx_at` → `re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500, x,y,z, yaw)`; `re2z_gore_fx` (:3391) wirft `packed_id` KOMPLETT weg und ruft dieselbe Zeile. `enemy_ai_re2_dog.c:312–339` — Tabelle + Budget + Yaw-Spread byte-true, aber Kinds 6/9/≥10 stumm (:329), alles andere → RE1.5-Raum-Blut. `enemy_ai_re2_crow.c:189` — Feder dokumentiert, kein Effekt. `RE15_RE2_AI.md:1001` + `OPEN_POINTS_AUDIT.md:95` führen das als Infrastruktur-Blocker #1.
- **RE1.5-ESP-Seite (dasselbe Datenformat-Erbe!)**: `re15_esp.c` parst Bänke mit IDENTISCHER Struktur (8-B-Eff-Header, count_a×8 B Anim, count_b×4 B Coords — :243–263), RE1.5 hat ein **eigenes `CORE00.ESP`** (Ids `03 08 00 02 04`, :288) als Global-Bank (:282–297) und eine `scale16`-Q12-Spur (:332 „@entry+0x72") — d. h. RE1.5 ist ein früherer Stand DERSELBEN Familie; der RE1.5-Treiber ist die Row-Maschine, nicht die RE2-Step-Maschine.
- **Pool/Tick/Draw existieren**: `s_esp_fx[RE15_ESP_FX_MAX]` (re15_esp.c:276), Tick 30 Hz `main.c:3757` (mit demselben Pause-Gate-Wert 0x10000000, main.c:3748), Draw plattformseitig `main.c:208` / `room_pc.c`.
- **Asset-Konvention für RE2-Flavor steht**: `shared_assets/RE2/` (CDEMD0.EMS, ENEMSE.VBS), Loader-Muster `main.c:474–507` (`pc_read_shared`-Fallback-Kette), Splitter `re2_ems.c`.

## 3) IMPLEMENTIERUNGS-PLAN (minimaler RE2-FX-Layer)

**Grundsatzentscheidung**: Nicht auf RE1.5-Kinds mappen (verliert Mehr-Part-Scripts, Matrix-Anheftung, Gore-Taumeln, Feder-Flatter, CLUT-Farbzeilen), sondern die RE2-Maschine daten-getreu nachbauen — sie ist klein (Slot 0x7c B, Pump + 2 Transitions-Ops + Billboard-Draw decken ~90 % der real gerufenen Scripts ab).

1. **Assets** (Kopien, keine Konvertierung): `CORE00.ESP` + `CORE01.TIM` aus `info/re2leon/COMMON/DATA/` nach `shared_assets/RE2/`; dazu ein einmalig extrahiertes Raum-Bank-Paket (`rdt_esp_dump.py`-Logik) für die im RE2-Modus gebrauchten Raum-Kinds: **9** (Spritzer, aus ROOM1000), **0x0A**, **0x0C** (Feder, aus ROOM1090) inkl. der zugehörigen TIMs aus `*(RDT+0x58/0x5c)` → z. B. `shared_assets/RE2/FX/ESP09.BIN`+`ESP09.TIM` …
2. **Neue Datei `engine/src/re2_fx.c`** (+`include/re2_fx.h`):
   - `re2fx_register(bank_id, blob)` = FUN_8001bca0-Äquivalent (Id-Liste + Rückwärts-Offsets), CORE beim RE2-Flavor-Boot (neben dem CDEMD0-Load, main.c:474ff), Raum-Kinds beim Room-Load;
   - `re2fx_spawn(a0, yaw, mtx, ofs[, owner, extra])` — exakt die 1a-Dekodierung; Slot-Struktur 1:1 (0x7c-Layout aus 1d);
   - Pump = Übersetzung von FUN_8001d300/d68c/d894 (Gate an `RE15_PAUSE_ACTION` koppeln, Bank-Whitelist {1,0x15,0x16,0x1a,0x1c,0x28} @FUN_8001d300.c:45/61);
   - Opcode-Tabelle 96 Einträge, initial belegt: 0 (NOP), 1/2 (Transition, FUN_8001dc30/dd2c), Rest als benannter Stub mit Zähl-Log — dann NACH BEDARF der real getriggerten Scripts nachziehen (Blut sub0/1 braucht NUR Op 0/1; Feder braucht Op 0x22; Gore Op 0x0b/0x0c/0x49; Lachen Op-B 7/9);
   - Draw: FUN_80077ed0-Übersetzung in den bestehenden Sprite-Pfad (OT-Bucket `sz>>5`, Code 0x2c/0x2e, PARAM-Q12-Größe, CLUT-Zeilen-Offset) — Einhängen wie der ESP-Fx-Draw in `main.c:208`/`room_pc.c`, Render-Order-Skill `re15-pc-render-order` beachten.
3. **Call-Sites umstellen** (nur RE2-Flavor-Pfad): `re2z_gore_fx`/`re2z_blood_fx_at` (enemy_ai_re2_zombie.c:915/3391), `re2d_fx` (enemy_ai_re2_dog.c:324 — inkl. 0x80-Flag = Part-Matrix-Anheftung, Korrektur zu heute), Krähen-Feder (enemy_ai_re2_crow.c:189), jeweils `packed_id` unverändert durchreichen. RE1.5-Flavor bleibt unangetastet.
4. **Verifikation**: (a) Unit-Probe je Kind: spawnen, N Frames ticken, Slot-Felder (+0x24 Lokalpos, +0x20/21 Anim, Status) gegen die Step-Daten aus 1e prüfen (Muster `probe_dog_hit.c`); (b) Zombie-Beschuss im RE2-Modus per `re15-room-probe`, FX-Zähler + Positionen; (c) visuell NUR per `re15-port-visual-verify` (gdigrab), da Autoshot Textur-Bugs maskiert.

## 4) OFFEN (ehrlich)

- **CORE01.TIM-Upload-Stelle** in der RE2-EXE nicht lokalisiert (die vorgebackenen clut/tpage-Werte 0x7a11/0x7811/0x7b11/0x7c91, tpage 0x1e/0x1f implizieren festes VRAM @(896/960,256)+, CLUTs (272,480+)); für den Port irrelevant (Placement frei wählbar), fürs Byte-true-Protokoll: nächster Schritt = Scan nach `LoadImage`-Callern mit RECT-Konstanten x≥896 bzw. PCSX-Redux-VRAM-Write-Watch.
- **Datei-Index 0x90 = CORE00.ESP** ist über den perfekten Struktur-Match belegt (Id-Liste an a1-Basis, Rückwärts-Walk trifft alle 8 Bänke), nicht über die CD-TOC; TOC-Nachweis stünde noch aus (FUN_80012fb8-Tabelle).
- **91 der 96 Opcodes** nicht einzeln RE'd (nur 0/1/2/0x0F/0x36 + Draw-Pass; die LAB_-Handler 0x03–0x0E usw. liegen als Roh-Disasm in ghidra_re2_Leon.txt). Op-genauer Bedarf ergibt sich aus den real getriggerten Scripts — Stub-Zähler im Port (Plan 2) macht die Lücke messbar.
- **Extra-Param der c224/c56c-Varianten** (+0x44/+0x48): Konsumstelle nicht verifiziert (vermutlich Farb-/Ziel-Override einzelner Op-B-Renderer).
- **Status-Bit 0x200-Zweig** in FUN_80077924 (Alternativ-Draw, tritt bei Lachen-/Splat-Steps 0xb203 auf) nicht zu Ende gelesen; ebenso der Rest von FUN_80077924 (Zeilen >100).
- Anim-Eintrag Bytes 4–7: in allen Dumps 0 — Bedeutung unbekannt (möglicherweise ungenutzt).
- Semantik der Raum-Kinds 0x08/0x0A/0x16/0x0B usw. nur über Caller-Kontext teilbenannt (0x0A = Zombie-FX `0x0A001000` @0x80104DE0-F4, 0x1A = Wasser); vollständige Benennung = Script-Dump je Kind (Werkzeug `rdt_esp_dump.py` liegt bereit).
