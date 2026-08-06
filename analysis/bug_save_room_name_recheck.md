# RE-CHECK: Ortsname im Speicherplatz-Screen — "war das im Vorprojekt nicht pro Raum anders?"

**Datum:** 2026-08-06
**Auftrag:** Der Nutzer widerspricht der These `analysis/bug_save_room_name.md` / Memory `reai-v2-save-room-name-stub`
("Ortsname ist im ORIGINAL konstant, weil `FUN_80026e4c` ein return-0-Stub ist"). Im Vorprojekt
`c:/workspace/git/reAi` habe pro Raum ein ANDERER Titel gestanden. Gesucht: der tatsaechliche Mechanismus —
(a) Original-Binary, (b) EXE-Modifikation des Vorprojekts, (c) Port-Eigenbau.
**Verfahren:** 5 unabhaengige Untersuchungs-Lanes + adversariale Gegenpruefung jeder Lane; danach eigene
Nachmessung aller tragenden Zitate (Bytes gelesen, MIPS selbst dekodiert, Savestates dekomprimiert, Disc de-sektoriert).
**Regel:** CLAUDE.md STOP-GATE — jede Behauptung mit `@0x…`-Adresse oder Datei-Byte-Offset. Kein Code geaendert.

---

## 0. Antworten (kurz)

| # | Frage | Antwort |
|---|-------|---------|
| 1 | Erzeugt das ORIGINAL-Binary pro Raum einen anderen Ortsnamen? | **NEIN.** `FUN_80026e4c` @`0x80026e4c` = `jr ra` / `addu v0,zero,zero` (Datei-Offset `0x1764c`: `08 00 e0 03 21 10 00 00`), genau 2 Aufrufer, null Datenzeiger. Beide Namenspfade bekommen konstant Index 0. |
| 2 | Woher kamen die unterschiedlichen Titel? | **EXE-Patch des Vorprojekts** (Code-Cave). `reAi/scripts/patch_save_final.py:681-732` ersetzt genau diesen Stub durch `j 0x80070890` und legt dort einen eigenen Resolver ab. Genau ZWEI Indizes (0 = ROOM1150 Schreibmaschine, 1 = ROOM1070 Telefon). Kein Port-Eigenbau, keine andere Namensliste, keine Verwechslung. |
| 3 | Beste Umsetzung fuer reAi_v2 | **Konstant lassen** — das ist byte-true und der Port macht es bereits richtig. Es gibt aber ZWEI echte, belegbare Port-Divergenzen daneben (BIOS-Kartentitel + Datenquelle des Strings), siehe §6. Jede Raum→Name-Zuordnung waere erfunden (Rate-Defekt). |
| 4 | War die alte Schlussfolgerung richtig? | **Teilweise richtig.** Das ERGEBNIS ("konstant, kein Port-Bug, nicht fixen") ist korrekt und bleibt. Die BEGRUENDUNG war falsch formuliert: eine Raum→Name-Zuordnung fehlt, aber die NAMENSTABELLEN existieren sehr wohl — sogar zwei. Und die Notiz kannte den Vorprojekt-Patch nicht, wodurch die Nutzer-Erinnerung wie ein Widerspruch aussah. |

**Beides ist gleichzeitig wahr:** Der Nutzer hat im Vorprojekt real unterschiedliche Titel gesehen — und die
Stub-Aussage fuer das unmodifizierte Binary ist korrekt.

---

## 1. Das ORIGINAL: der Resolver ist tot

### 1.1 Der Stub

```
c:/workspace/git/reAi_v2/info/Re1.5/PSX.EXE   (md5 b55fdaa5b9ec0f84bdd8a6b1107ea0b0, 718848 B, t_addr 0x80010000, t_size 0xaf000)
Datei-Offset 0x1764c = 08 00 e0 03  21 10 00 00
  80026e4c  03e00008  jr   ra
  80026e50  00001021  addu v0,zero,zero        -> RETURN 0, IMMER, a0 wird ignoriert
```
Offsetformel: `Datei = Adresse - 0x80010000 + 0x800`.

### 1.2 Genau zwei Aufrufer, kein indirekter Weg

Eigener erschoepfender Opcode-Scan (jal-Wort `0x0c009b93`) ueber `PSX.EXE` **und** `PSX/BIN/DEBUG.BIN`,
`TITLE.BIN`, `STAGE1-6.BIN`:

* `@0x80026810` — Slot-Zeile der In-Game-Speicherliste (`FUN_80026658`)
* `@0x80026efc` — BIOS-Kartentitel-Builder (`FUN_80026e54`)
* **0 Treffer in allen Overlays**, **0** 32-Bit-Datenworte mit dem Wert `0x80026e4c` (keine vtable/Dispatch),
  **0** `j`-Sprünge auf `0x80026e4c`/`0x80026e50` (kein alternativer Einstieg mit anderem Rueckgabewert).

### 1.3 Die Daten waeren da — sie werden nur nie gelesen

Der Save-Block (0x1230 Bytes ab `0x800b0fbc`) enthaelt Stage und Raum:

```
Writer  @0x800261b8-0x800261d8:  sll a0,s3,7 / addiu a0,a0,0x1430 / lui a1,0x800b /
                                 addiu a1,a1,0x0fbc / ori a2,zero,0x1230 / jal 0x8004ee38 (memcpy)
Feld    0x800b0fe0 = STAGE  (sh zero,0x0fe0(at) @0x8001d310, @0x8001d368)
Feld    0x800b0fe2 = RAUM   (sh v0,0x0fe2(at)   @0x8001d2b0, @0x8001d32c)
```
`FUN_80026e4c` bekommt genau diesen Record als `a0` — und liest ihn nicht an.

### 1.4 Es gibt nirgends eine Raum-ID → Namensindex-Tabelle

Negativ-Scans ueber `PSX.EXE` + `DEBUG.BIN` + `TITLE.BIN` + `STAGE1-6.BIN` (zweifach unabhaengig gefahren):

* exakte u16-Sequenz der 8 Save-Raeume (`0x1070,0x1120,0x1150,0x2010,0x30A0,0x30B0,0x4010,0x5010`,
  Quelle `re15_port/engine/src/re15_savepoint.c:16-26`), LE **und** BE → **0 Treffer**
* Byte-Sequenz der Low-Nibbles `07 12 15 01 0a 0b 01 01` → **0 Treffer**
* Cluster-Suche (≥4 verschiedene Save-Raum-IDs als u16 in einem 80-Byte-Fenster) → **0 Treffer**

Unabhaengig davon ist die Frage entschieden: selbst eine gefundene Tabelle waere unbenutzt, weil der
einzige Index-Lieferant der Stub ist.

---

## 2. Die Namenslisten — sie EXISTIEREN, beide

### 2.1 Pfad A — In-Game-Slot-Zeile (der Screen beim Speichern/Laden)

```
80026810  0c009b93  jal 0x80026e4c
80026814  02002021  addu a0,s0,zero        (Delay-Slot: a0 = Slot-Record = buf+0x1430+n*0x80)
80026818  3404001a  ori  a0,zero,0x1a      <-- BASIS-INDEX
8002681c  0c030039  jal 0x800c00e4         (sysmes-Getter in DEBUG.BIN)
80026820  00822021  addu a0,a0,v0          <-- DELAY-SLOT: Index = 0x1a + Resolver
8002682c  0c013b8e  jal 0x8004ee38         (memcpy)
80026830  34060010  ori  a2,zero,0x10      <-- nur 0x10 Bytes!
```
Getter `FUN_800c00e4`, selbst aus `PSX/BIN/DEBUG.BIN` (laedt RAW @`0x800c0000`, Datei-Offset 0xe4) dekodiert:
`lui t0,0x800c / addiu t0,t0,0x5f96 / sll a0,a0,1 / addu t1,t0,a0 / lhu t1,0(t1) / jr ra / addu v0,t0,t1`
→ `ptr = 0x800c5f96 + u16[0x800c5f96 + idx*2]`.

Strings (Zeichensatz: Byte+0x24 = ASCII, `0x00` = Space, `0x01` = Terminator; `0x3a` = Apostroph,
`0x3b` = Bindestrich laut Port-Fonttabelle `re15_port/platform/pc/src/render_pc.c:2396-2409`):

| idx | DEBUG.BIN-Offset | String |
|-----|------------------|--------|
| 0x1a | 0x6197 | `Irons' Office` |
| 0x1b | 0x61a6 | `Medical Room` |
| 0x1c | 0x61b4 | `West Staircase 1F` |
| 0x1d | 0x61c7 | `Sewer Maintainance` |
| 0x1e | 0x61db | `Sewer Control Room` |
| 0x1f | 0x61ef | `Factory - Office` |
| 0x20 | 0x6201 | `Security Room` |
| 0x21 | 0x6210 | `Monitor Room` |

Nachbarn: 0x16 `Fail in save`, 0x17 `Fail in load`, 0x18/0x19 = Leon-/Elza-Template mit Steuerbyte-Praefix.
Mit Stub = 0 wird **immer** 0x1a gezogen.

**Signal, dass das Feature nie fertig verdrahtet wurde:** der Kopierer nimmt `0x10` Bytes, aber
0x1c/0x1d/0x1e sind 18/19/19 Bytes und 0x1f 17 Bytes (inkl. Terminator) — **vier von acht Namen passen
nicht in den Zielpuffer**.

### 2.2 Pfad B — BIOS-Memory-Card-Blocktitel

Selbst disassembliert (`FUN_80026e54`, Original-EXE):
```
80026e54  lui  v0,0x800b / 80026e58 lbu v0,0x0fbe(v0)      (Charakter)
80026e80  addiu a1,a1,0x07cc   |  80026e94 addiu a1,a1,0x07f8   (Elza- / Leon-Template)
80026e98  jal 0x8004ee38 / 80026e9c ori a2,zero,0x2a          (0x2a Byte Template kopieren)
80026ea8  addiu a0,a0,0x0fbd ... (Save-Zaehler-Ziffern via multu 0xcccccccd / srl 3)
80026efc  jal 0x80026e4c                                        <-- derselbe Resolver
80026f04  addiu a0,s0,0x2a                                      (Ziel = Template-Ende)
80026f08  andi a1... 80026f0c-80026f18: a1 = 19*v0 (= 0x13*v0)
80026f1c  lui v0,0x8007 / 80026f20 addiu v0,v0,0x3628           (Tabelle 0x80073628)
80026f28  jal 0x8004ee38 / 80026f2c ori a2,zero,0x13            (0x13 Byte Ortsname anhaengen)
```
Tabelle `@0x80073628` = Datei-Offset `0x63e28`, Stride `0x13`, **7 Eintraege, Shift-JIS**, inhaltlich
**Resident-Evil-1-Orte** (RE1-Erbmasse):

| idx | Adresse | SJIS |
|-----|---------|------|
| 0 | 0x80073628 | 館　小部屋　１Ｆ」 (Herrenhaus, kleiner Raum 1F) |
| 1 | 0x8007363b | 館　ホール　１Ｆ」 |
| 2 | 0x8007364e | 中庭　部屋　Ｂ１」 |
| 3 | 0x80073661 | 　寄宿舎　　１Ｆ」 |
| 4 | 0x80073674 | 　研究所　　Ｂ３」 |
| 5 | 0x80073687 | 館　　物置　１Ｆ」 |
| 6 | 0x8007369a | 中庭　通路　Ｂ１」 |

Templates: `DAT_800107f8` (Datei 0x00ff8) `ＢＩＯ　ＨＡＺＡＲＤ　「Ｌｅｏｎ　／００／`,
`DAT_800107cc` (Datei 0x00fcc) `…「Ｅｌｚａ　／００／`, je 0x2a Bytes.
Dateiname auf der Karte: `BISLPS-00222*` (= SLPS-00222 = Biohazard 1 JP) — ebenfalls unersetztes RE1-Erbe.
Mit Stub = 0 wird **immer** Eintrag 0 angehaengt.

### 2.3 ⚠ NEU / WICHTIG: Build-Provenienz — welches "Original" eigentlich?

Im Workspace liegen **zwei verschiedene RE1.5-PSX.EXE-Revisionen**:

| Datei | md5 | Groesse | Kennzeichen |
|-------|-----|---------|-------------|
| `reAi_v2/info/Re1.5/PSX.EXE` (= Projekt-Referenz, identisch `reAi/info/Re1.5/PSX.EXE`) | `b55fdaa5b9ec0f84bdd8a6b1107ea0b0` | 718848 | **MZD-Fan-Build**; `info/Re1.5/README.TXT` = „-=Resident Evil 1.5 (Magic Zombie Door) Update 25-01-2025=- … Team IGAS build modification made by MartinBiohazard"; hat `PSX/BIN/DEBUG.BIN` (md5 `c2c11aab8a0ae4c0c7fdb56fb33c3a74`) |
| `reAi/already_extracted/PSX.EXE` | `b653c80fa01aa77a4ad5b646dd40e2db` | 718848 | enthaelt den Build-Datumsstring `1995/11/10` (in der MZD-EXE nicht mehr vorhanden), Memcard-Texte **japanisch**, `PSX/BIN` **ohne DEBUG.BIN**, **null** `jal 0x800c00e4` im gesamten .text |

**Der Slot-Zeilen-Ortsname wird in den beiden Builds aus VERSCHIEDENEN Traegern gezogen** — selbst
disassembliert aus `already_extracted/PSX.EXE`:

```
80026894  0c009b93  jal 0x80026e4c
80026898  02002021  addu a0,s0,zero
800268a0  304200ff  andi v0,v0,0xff
800268a4  00021040  sll  v0,v0,1
800268a8  3c018007  lui  at,0x8007
800268ac  24213610  addiu at,at,0x3610      <-- &offtbl[0x1a]  (offtbl-Basis 0x800735dc)
800268b0  00220821  addu at,at,v0
800268b4  94250000  lhu  a1,0(at)
800268b8  34060010  ori  a2,zero,0x10
800268bc  0c013b8e  jal 0x8004ee38
800268c0  00b42821  addu a1,a1,s4           (s4 = 0x80073490, gesetzt @0x80026820/24)
```
Also: **EXE-interne** Message-Tabelle, Strings ab `0x80073490`, u16-Offsets ab `0x800735dc`.
Tabelle komplett gedumpt — sie hat **27 Eintraege (0x00..0x1a)**, und **Index 0x1a ist der LETZTE**:

```
idx 0x10 @0x80073556  "NO DATA"                    (belegt Zeichensatz + Zweck der Tabelle)
idx 0x18 @0x800735c2  "Leon /00/"
idx 0x19 @0x800735ca  "Elza /00/"
idx 0x1a @0x800735d2  e4 a8 ab 0e 6f e2 3b b6 01   <-- EINZIGER Ortsname, 8 Glyphen, japanisch
idx 0x1b @0x80073490  (Alias auf Message 0)  |  idx 0x1c+ = Fremddaten (Zeiger 0x801ff404)
```

**Konsequenz:** Die acht ENGLISCHEN Ortsnamen aus §2.1 sind **nicht in beiden Builds vorhanden** — sie
gehoeren zum MZD-Build (dessen englische Uebersetzung nach `DEBUG.BIN` ausgelagert ist). Im aelteren Build
existiert genau EIN Ortsname (japanisch). In **beiden** Builds ist `0x80026e4c` byte-identisch derselbe
return-0-Stub, und in beiden gibt es genau die zwei Aufrufer (`0x80026894`/`0x80026efc` bzw.
`0x80026810`/`0x80026efc`). Die SJIS-Kartentitel-Tabelle `@0x80073628` ist in beiden identisch vorhanden.

Fuer reAi_v2 ist das nicht schaedlich, aber es muss benannt werden: **Der Port portiert den MZD-Build**
(`CLAUDE.md` nennt `info/Re1.5/` „RE1.5-Original-CD-Daten" — faktisch ist es der MZD-Fan-Build).
Der korrekte konstante Name fuer diesen Port ist daher `Irons' Office` (sysmes 0x1a).

---

## 3. Der Mechanismus des Vorprojekts: Code-Cave-Patch (Ausgang **b**)

Artefakt: `c:/workspace/git/reAi/info/Re1.5_PATCHED/re15/PSX.EXE`
(md5 `17d19f4896de70a01381e11f53e05490`, 727040 B = +8192, `t_size` 0xaf000 → 0xb1000).

### 3.1 Stub → Sprung in die Cave

```
Datei 0x1764c   ORIGINAL: 08 00 e0 03 21 10 00 00   (jr ra / addu v0,zero,zero)
Datei 0x1764c   PATCHED : 24 c2 01 08 00 00 00 00   -> 0x0801c224 = j 0x80070890 ; nop
```
Quelle: `reAi/scripts/patch_save_final.py:716` `FUN_80026E4C_FILE = 0x1764C`,
`:718` `0x24,0xC2,0x01,0x08  # j 0x80070890`, angewandt `:2128`.
Duplikate: `reAi/scripts/patch_save_final_backup.py:452,477`, `reAi/lastMaster.py:400,425`.

### 3.2 Die eingeschleuste Funktion (Original dort: Nullen)

`SAVE_LOC_FUNC` @`0x80070890` (Datei `0x61090`), selbst disassembliert:
```
80070890  lui  v1,0x8002
80070894  ori  v1,v1,0x6f04
80070898  beq  ra,v1,0x800708ac
8007089c  nop
800708a0  lbu  v0,3(a0)          <-- Aufrufer 1 (Slot-Liste, ra=0x80026818): per_slot[3]
800708a4  jr   ra
800708ac  lui  v0,0x800b
800708b0  lbu  v0,0x0fbf(v0)     <-- Aufrufer 2 (Kartentitel, ra=0x80026f04): game_state[3]
800708b4  jr   ra
```
Im ORIGINAL ist der Bereich `0x80070840`–`0x80070900` **vollstaendig 0x00** (Nulllauf
`0x80070562`–`0x80070927`, 966 B; `reAi/code_cave_analysis.txt:43` „Cave 1: 0x80070564 - 0x80070938").
Die ra-Konstanten sind aus dem Original verifiziert: `jal @0x80026810` + Delay-Slot → ra `0x80026818`;
`jal @0x80026efc` + Delay-Slot → ra `0x80026f04`.

### 3.3 Wer den Index setzt — genau zwei Werte

```
AOT_TYPE1_HOOK  @0x8007084c :  lhu v0,0(a0) / ori v1,zero,0xfe / bne v0,v1,0x80070888
                    0x80070870 ori v0,zero,0x1
                    0x8007087c sb  v0,0x0fbf(at)      -> game_state[3] = 1
                    0x80070888 j   0x80043084          (Fallthrough: Original-Handler)
SCD_SAVE_RET    @0x800708bc :  lui at,0x800b
                    0x800708c0 sb  zero,0x0fbf(at)    -> game_state[3] = 0
```
Skript: `patch_save_final.py:643` (`AOT_TYPE1_HOOK_RAM = 0x8007084C`), `:663-665`, `:724` (`SCD_SAVE_RET_RAM = 0x800708BC`), `:727-732`.
Dispatch umgebogen: Datei `0x64ea0` (RAM `0x800746a0`) `84 30 04 80` (=0x80043084) → `4c 08 07 80` (=0x8007084c);
Nachbareintraege unveraendert (genau EIN Tabelleneintrag).

### 3.4 Welcher Raum welchen Index bekommt — 3 RDT-Sentinel-Bytes

Byte-Diff der ausgelieferten RDTs (Original vs. `Re1.5_PATCHED`):

| RDT | Offset | orig → patched | Pfad |
|-----|--------|----------------|------|
| `STAGE1/ROOM1150.RDT` | `0x10ED` | `0x01 → 0xFE` | SCD-Save (Schreibmaschine) → Index **0** |
| `STAGE1/ROOM1151.RDT` | `0x10CB` | `0x01 → 0xFE` | dito |
| `STAGE1/ROOM1070.RDT` | `0x1568` | `0x14 → 0xFE` | AOT-Typ-1 (Telefon) → Index **1** |

(`ROOM1070.RDT` hat genau **ein** abweichendes Byte; `ROOM1150/1151` je 5 — die vier weiteren gehoeren zum
separaten Item-Box-Patch, `patch_save_final.py:800`.) Skript: `:149-151`, `:674-677`, `:4581`.
Stage-weiter Asset-Diff: **nur diese 3 RDTs weltweit**, STAGE2-6 unveraendert.

### 3.5 Transportweg auf die Karte

`0x800b0fbf` liegt als **+3** im ab `0x800b0fbc` gesicherten 0x1230-Block (§1.3), landet also automatisch als
Kartenblock-Offset `0x203`. Ruecklesepfad aus dem Original-Disasm (nicht aus Decompilaten):
`FUN_80026ca8` @`0x80026d04-0x80026d68`: `addiu s4,s5,0x200` / `ori s2,zero,0x1430` / `ori a2,zero,0x80` /
`jal 0x80027368` mit `ori a3,zero,0x200` / `jal 0x8004ee38` → `buf+0x1430+n*0x80`.
Genau dieser Zeiger ist das `a0`, aus dem der Cave `lbu v0,3(a0)` liest.

### 3.6 Der Patch erfindet KEINE Namen

* `0x800b0fbf` hat im ORIGINAL **0 Referenzen** (Effektivadress-Scan mit lui-Registerverfolgung ueber
  `PSX.EXE` + alle 8 BINs); in der gepatchten EXE genau **3** (`sb` @0x8007087c, `lbu` @0x800708b0, `sb` @0x800708c0).
  Die Nachbarbytes `0x0fbc/0x0fbd/0x0fbe` werden im Original durchgaengig angesprochen — `0x0fbf` ist das einzige Loch.
* `DEBUG.BIN` ist im Patch-Build **byte-identisch** (md5 `c2c11aab8a0ae4c0c7fdb56fb33c3a74`), die SJIS-Tabelle
  `0x63e28` liegt in **keiner** Diff-Region.
* Grep im Patcher nach `Irons|Medical|Hall|Small Room|0x73628|0x63E` → **0 Treffer**.

Der Patch liefert also ausschliesslich die Zahlen **0** und **1**.

### 3.7 Eigenaussage des Vorprojekts

`patch_save_final.py:681` `# [Z] SAVE_LOC_FUNC: Per-slot location name index for memory card title`
`:686` `Original is a 2-instruction stub returning 0 (always shows location 0)`
`:688` `Fix: Store location index in game_state[3] (0x800B0FBF, unused byte)`
`:4954` `Room 1070 phone: card title shows Hall 1F for THAT slot.`
`:4955` `Room 1150 typewriter: card title shows Small Room 1F for THAT slot.`
Inventarisiert auch in `reAi/bug_v124att10_agent_AA14.md:17,45` (`| 23 | 0x1764C | 0x80026E4C | 6 B | -> j 0x80070890 |`).
Die eigenen Decompilate des Vorprojekts beschreiben den Original-Stub korrekt:
`reAi/RE_15_modified_V2/room_get_index_for_save.c:13-19` (`return 0`),
`reAi/RE_15_modified/memcard_get_area_id.c:15-20` (`/* STUB: Gibt immer 0 zurueck */`).

---

## 4. Was der Nutzer gesehen hat — und was ausgeschlossen ist

* **Ausgang (c) PORT_EIGENBAU ist ausgeschlossen.** Grep nach `Irons.{0,4}Office|Medical Room|sysmes|slot_title|card_title`
  ueber ALLE `*.c/*.h/*.java/*.py` in `c:/workspace/git/reAi` (inkl. `psx_dev/re15_reborn`, `re15_reborn_pc`, `re2_port`,
  18050 Dateien) → **0 Treffer**.
* **Kein Screenshot/Video existiert.** Das Video `reAi/"re15_save_final 2026-06-23-11-49-09.mp4"` (42.77 s, 640x480,
  59.82 fps, 2559 Frames) zeigt **durchgehend Gameplay** (Briefing-Room mit Leon + Zombies): eigener 3-fps-Kontaktbogen
  (128 Kacheln), `signalstats` ueber alle 2559 Frames YAVG 53.510..54.597 (Spannweite 1.09),
  `select='gt(scene,0.10)'` → 0 Schnitte. Der Dateiname stammt aus DuckStations Spieltitel = Name des gepatchten ISO.
  `reAi/.agent_refs/ss_savestate_*.png` sind 1024x512-VRAM-Dumps, kein Menue.
* **Sachprobe statt Screenshot:** die echte Karte `C:/Users/mjoedicke/AppData/Local/DuckStation/memcards/re15_save_final_1.mcd`
  (128 KB, Magic `MC`) enthaelt 2 Saves `BISLPS-002220/1`, Titel byte-genau
  `ＢＩＯ　ＨＡＺＡＲＤ　「Ｌｅｏｎ　／０１／館　小部屋　１Ｆ」` (= SJIS-Eintrag **0**, angehaengt exakt ab
  Blockoffset 0x2e = 4 + 0x2a, passgenau zum `memcpy(dst+0x2a, …, 0x13)`), und Blockoffset `0x203` = `0x00` in
  **beiden** Bloecken.
* **Wahrscheinlichster Ort der Erinnerung:** der BIOS-Kartentitel (`館　小部屋　１Ｆ` vs. `館　ホール　１Ｆ`) bzw. die
  Slot-Zeile `Irons' Office` vs. `Medical Room`. Die Zuordnung des Vorprojekts ist semantisch schief
  (ROOM1070 heisst im Debug-Menue `LOBBY OFFICE`, bekommt aber sysmes 0x1b = `Medical Room`) — es wurde
  einfach „irgendein anderer" Index vergeben.
* **Verwechslung mit dem Debug-Menue ausgeschlossen:** die Raum-Namen der Debug-/Spawn-Tabelle
  (`DEBUG.BIN @0x263a`, 6x49 Saetze a 26 B, `off = 1274*stage + 26*room`, Name @+8, z.B. `CHIEF OFFICE`,
  `MED. EXAM ROOM`, `CONTROL ROOM`, `MONITOR ROOM`) werden **ausschliesslich** vom Debug-Menue-Renderer
  gelesen (`@0x80014ba8` `jal 0x800279c8` mit `addiu a3,a3,8`). Der Save-/Memcard-Pfad fasst das Namensfeld
  nirgends an; nur die Positionsfelder +2/+4/+6 werden vom normalen Spawn-Pfad
  (`@0x8001d3cc-0x8001d454`, zweiter Block `@0x8001d6a0/6dc/718/754/790`) gelesen. Alle 294 Saetze haben
  `+7 == 0x00` und `+22..+25 == 0` — **kein Area-Index-Feld**.
* **Mehr als zwei Titel sind mit diesem Patch nicht erklaerbar.** Geprueft wurden Haupt-Build
  (`17d19f48…`) und eine zweite Revision (`c71c0ab9270227409d143058f99eda0b`, in
  `reAi/.claude/worktrees/agent-a795e02c/`): identischer Redirect, identische Cave-Funktion, identisches
  3-Referenz-Muster auf `0x800b0fbf`. Beide kennen nur 0 und 1.

---

## 5. Quergegenprobe RE2-Retail: so haette es fertig ausgesehen

RE2 hat **dieselbe Architektur, nur ausimplementiert** — und beweist damit, dass RE1.5 hier eine unfertige
Baustelle ist, keine fehlende Absicht:

* Slot-Titel liegt im eigenen Overlay `info/re2leon/COMMON/BIN/MEM_CARD.BIN` (0x5d7c B, Ladebasis
  `0x801bfa18` — dreifach gestuetzt: Stringbasis `0x801c3e1c` = Datei 0x4404, Offsettabelle `0x801c4260`
  = Datei 0x4848, Prolog `0x801bff10` = Datei 0x4f8).
* Ortsnamen = Message-Index `0x2b..0x39`, 15 Eintraege (`Waiting Room`, `Hall`, `Dark Room`, `Storeroom L/R`,
  `Control Room1/2`, `Water Pool`, `Factory`, `Pump Room`, `Security Room`, `Monitor Room`, `Gun Shop`,
  `Cabin`, `Train`; Datei 0x4697..0x473c).
* **Der Index kommt aus den SAVE-DATEN:** `MEM_CARD.BIN` Datei `0x26a0` (= `0x801c20b8`) `lbu v0,11(s3)`
  → `msg = 0x2b + saveblock[0x0b]` (`s1 = 0x801c42b4` = &offtbl[0x2a], gesetzt Datei 0x2620) — genau der
  Record, den RE1.5s Stub bekommt und ignoriert. (Charakter liegt daneben: `lbu v1,10(s3)` @0x801c1fc8.)
* **Gefuellt wird das Byte vom RAUMSKRIPT**, nicht aus einer Raum-ID-Tabelle:
  RE2-`PSX.EXE` `@0x80051ab0` (Datei 0x422b0) `lhu a0,12(v1)` → `sb a0,0x46af(at)` (`lui at,0x800d` →
  `0x800d46af`); genau EIN Datenzeiger darauf, `@0x800a73e8` (Eintrag 9 einer 15-Eintrag-Zeigertabelle
  `@0x800a73c4`).
* **Korrektur einer Zwischenbehauptung:** RE2s BIOS-Kartentitel enthaelt **doch** einen Ortsnamen — der
  Titelbauer laeuft bis `jr ra @0x801c3220` und haengt an: `0x801c31b4 addiu a0,s0,38` /
  `0x801c31e4 lbu v0,-31545(at)` (= **0x800d46af**) / `0x801c31e8 addiu a2,zero,27` / 29*idx /
  `0x801c3200 addiu v0,v0,17184` → Tabelle `0x801c4320` (Datei 0x4908) mit denselben 15 Orten in
  Vollbreiten-SJIS. **Mechanisch identisch zu RE1.5** (Template + Ortsname am Template-Ende).

**Fazit der Quergegenprobe:** Auch RE2 hat keine Raum-ID→Name-Tabelle im Binary. Der Index wird pro
Save-Punkt im Skript autoriert. Genau dieser Skript-Operand fehlt RE1.5 — deshalb ist die Suche nach einer
„verlorenen Tabelle" im RE1.5-Binary strukturell aussichtslos.

---

## 6. Frage 3: Was ist die beste Umsetzung fuer reAi_v2?

### 6.1 Byte-true = konstant. Der Port macht das bereits richtig.

`re15_port/platform/pc/main.c:540-544` baut die Slot-Zeile mit hartem
`room[] = {0x25,0x4e,0x4b,0x4a,0x4f,0x3a,0x00,0x2b,0x42,0x42,0x45,0x3f,0x41}` = `Irons' Office`, mit
korrekter Begruendung im Kommentar `main.c:518-534`. Diagnose-Probe existiert:
`re15_port/tests/unit/probe_save_room_name.c`. **Hier ist nichts zu fixen — jede Aenderung waere eine
Regression.**

### 6.2 Zwei ECHTE, belegbare Divergenzen daneben (die man byte-true fixen KANN)

| # | Divergenz | Beleg | byte-true Ziel |
|---|-----------|-------|----------------|
| **P1** | Der Ortsname ist im Port als Byte-Literal hartkodiert statt aus den Assets gelesen. | `main.c:544` vs. Original-Getter `FUN_800c00e4` (DEBUG.BIN Tabelle @0x5f96) | `sysmes(0x1a + 0)` datengetrieben aus `shared_assets/PSX/BIN/DEBUG.BIN` lesen und die ersten `0x10` Bytes kopieren — **identisches Ergebnis**, aber Datenquelle statt Literal, und die 0x10-Byte-Kopie wird mit-portiert. Kein Verhaltensrisiko. |
| **P2** | Der BIOS-Kartentitel ist ASCII-`snprintf` statt der SJIS-Byte-Kette des Originals. | Port: `main.c:510-512` `"BIO HAZARD %s /%02d/"`. Original: `FUN_80026e54` — `memcpy(dst, 0x800107f8, 0x2a)` (@0x80026e98/9c) + Ziffern-Patch + `memcpy(dst+0x2a, 0x80073628 + 0x13*0, 0x13)` (@0x80026f28/2c). Ist-Zustand auf echter Hardware nachweisbar: `re15_save_final_1.mcd` Titel `…／０１／館　小部屋　１Ｆ」` | Template `DAT_800107f8`/`DAT_800107cc` (Datei 0x00ff8 / 0x00fcc, je 0x2a B) + SJIS-Eintrag 0 (0x13 B) = 0x3d Bytes, Zaehlerziffern an Template-Position 29/… . Achtung: `RE15_MC_TITLE_LEN` = 32 < 0x3d (`re15_port/include/re15_memcard.h`). |

Beide sind mit Zitaten belegbar und damit STOP-GATE-konform.

### 6.3 Was NICHT geht (und warum genau)

| Option | Was fehlt zur byte-true-Fähigkeit |
|--------|-----------------------------------|
| **(a) Resolver ausimplementieren** (Raum aus dem Save-Block `+0x26` = `0x800b0fe2` lesen und auf 0..7 mappen) | Der Funktionskoerper existiert nicht — es gibt **null Bytes**, die die Abbildung Raum→Index definieren (§1.4). Die Zahlenwerte muessten erfunden werden = Rate-Defekt. Zusatzbeweis gegen die Absicht: der Kopierer nimmt 0x10 Bytes, 4 der 8 Namen sind 17-19 Bytes lang (§2.1). Das Vorprojekt selbst hatte auch keine Tabelle, sondern zwei hartkodierte Konstanten. |
| **(b) Debug-Raumnamen-Tabelle benutzen** (`DEBUG.BIN @0x263a`, Name @+8, z.B. `CHIEF OFFICE`) | Das ist eine echte Raum→Name-Tabelle in den Referenzdaten — aber (i) sie wird im Original **nur** vom Debug-Menue gelesen (`@0x80014ba8`), nie vom Save-Screen; (ii) andere Stringquelle, andere Schreibweise (Debug-Grossbuchstaben-Labels, nicht die 8 sysmes-Namen); (iii) 18-Byte-Feld gegen 0x10-Byte-Kopie. Ergebnis waere eine **sichtbare Divergenz zur PSX** (Parity-Oracle-Verstoss). |
| **(c) Optionaler QoL-Schalter** | Nur akzeptabel, wenn Default **AUS** und explizit als „nicht original" gekennzeichnet — sonst Verstoss gegen 100%-Mandat. Empfehlung: **nicht** bauen, solange Kernsysteme offen sind. |

### 6.4 Empfohlene Formulierung fuer die Memory-Notiz `reai-v2-save-room-name-stub`

> „Immer Irons' Office" ist ORIGINAL. Es fehlt **nicht** die Namenstabelle — es gibt sogar zwei
> (8 sysmes-Eintraege ab `DEBUG.BIN 0x6197` fuer die Slot-Zeile, 7 SJIS-Eintraege ab `PSX.EXE 0x80073628`
> fuer den BIOS-Kartentitel), und die Index-Maschinerie ist vollstaendig (`0x1a + FUN_80026e4c()` @0x80026818/20;
> `0x13*idx + 0x80073628` @0x80026f0c-28). Es fehlt **ausschliesslich** der Raum→Index-Resolver
> `FUN_80026e4c` @0x80026e4c (return-0-Stub, Datei 0x1764c). Die abweichende Erinnerung aus dem Vorprojekt
> stammt aus dessen Code-Cave-Patch (`reAi/scripts/patch_save_final.py` Block `[Z]`), der genau diesen Stub
> ersetzt und exakt zwei Indizes (0/1) kennt.

---

## 7. Frage 4: Bewertung der urspruenglichen Schlussfolgerung

**Ergebnis: TEILWEISE RICHTIG — operativ richtig, in der Begruendung unpraezise.**

**Richtig und bestaetigt:**
* „`FUN_80026e4c` ist ein return-0-Stub" — byte-belegt, in **beiden** vorhandenen Build-Revisionen.
* „genau 2 Aufrufer, null Datenzeiger" — durch eigenen erschoepfenden Scan bestaetigt (inkl. Overlays).
* „Der Ortsname ist im Original konstant; das ist kein Port-Bug; nicht fixen." — korrekt, gilt weiter.

**Falsch bzw. irrefuehrend formuliert:**
* „Eine Raum→Name-Zuordnung existiert im Binary nicht." Das liest sich als „es gibt keine Ortsnamen". Es gibt
  **zwei vollstaendige Namenslisten** und die komplette Index-Maschinerie; tot ist nur der Resolver.
* Die Notiz kannte den Vorprojekt-Patch nicht. Dadurch stand die Nutzer-Erinnerung scheinbar gegen den Befund —
  tatsaechlich haben **beide** recht. Genau diese Luecke hat den Widerspruch erzeugt.
* Implizit unvollstaendig: die Notiz behandelt nur die In-Game-Slot-Zeile; der zweite betroffene Pfad
  (BIOS-Kartentitel, `FUN_80026e54`) haengt am selben Stub und ist im Port zusaetzlich divergent (§6.2 P2).

**Nicht widerlegt, aber praezisiert:** die 8 englischen Namen sind Bestandteil des MZD-Build; im aelteren
Build (`b653c80f`) hat die Tabelle genau einen (japanischen) Eintrag. Fuer den Port (MZD-Assets) bleibt
`Irons' Office` der korrekte konstante Wert.

---

## 8. Hygiene-Befunde (unabhaengig vom Ortsnamen, aber messrelevant)

1. **`reAi_v2/info/Re1.5/re15_save_final.bin` ist die GEPATCHTE Disc, nicht die Referenz-Disc.**
   Selbst de-sektoriert (MODE2/2352, 91494 Sektoren, Userdata-Offset 24/2048): `PS-X EXE` bei Userdata
   `0x8e9e000`, extrahierte EXE `t_size 0xb1000`, md5 `17d19f4896de70a01381e11f53e05490`, `@0x1764c =
   24 c2 01 08` — identisch zum Vorprojekt-Patch-Build. `analysis/leon_injured_model.md:3` nennt sie
   „MZD-Disc-Image" und `:114` extrahiert `PL01.PLD` daraus. **Wer daraus misst, misst den Mod.**
   Dieselbe Datei liegt auch unter `re15_port/cmake/build/extracted/shared_assets_extracted/re15_save_final.bin`.
2. **6 von 85 Savestates in `reAi_v2/stage_saves/` laufen auf der GEPATCHTEN EXE.** Selbst nachgemessen
   (`.claude/skills/re15-savestate-ghidra/scripts/re15_ss.py`):
   `HASH-881C08B8082E53B6_1.sav` … `_6.sav` → `[0x80026e4c] = 24 c2 01 08`, `[0x80070890] = 02 80 03 3c …`
   (Cave-Code). Gegenprobe `doorA_square.sav` / `mzd_stage1_briefing.sav` → `08 00 e0 03 …`, Cave = Nullen.
   `boot_16.sav` ist nicht lesbar (PSX.EXE-Signatur nicht gefunden).
3. **`CLAUDE.md` bezeichnet `info/Re1.5/` als „RE1.5-Original-CD-Daten".** Faktisch ist es der MZD-Fan-Build
   (`info/Re1.5/README.TXT`). Das ist fuer den Port in Ordnung (er portiert diesen Build), sollte aber so
   benannt werden, damit „original" nicht mit „Capcom-Prototyp" verwechselt wird.

---

## 9. Offene Punkte

* **Kein Emulator-Nachweis.** Der Beweis ist statisch (Bytes, Disasm, echte Memory Card). Wer es sehen will:
  `reAi/info/Re1.5_PATCHED/re15_save_final.cue` in DuckStation, in ROOM1150 speichern (→ `Irons' Office` /
  `館　小部屋　１Ｆ`), in ROOM1070 speichern (→ `Medical Room` / `館　ホール　１Ｆ`).
* **Erinnert der Nutzer MEHR als zwei verschiedene Titel?** Dann passt kein bekanntes Patch-Artefakt: beide
  gefundenen Patch-Revisionen und alle drei Generator-Skripte kennen nur 0/1, und stage-weit sind nur 3 RDTs
  gepatcht. Naechster Weg waere `reAi/history/` bzw. `scripts/patch_save_v90.py`,
  `create_save_patch_complete.py`, `patch_save_system.py`, `enable_save_patch.py` (ungeprueft).
* **Ist `DEBUG.BIN` im normalen Spielfluss resident?** Der sysmes-Getter liegt im Overlay `@0x800c0000`.
  Nicht belegt — relevant nur, falls P1 (§6.2) datengetrieben umgesetzt wird.
* **Glyph 0x3b** in `Factory - Office`: der Port mappt ihn auf `-` (`render_pc.c:2396-2409`); gegen die
  Font-TIM nicht gegengeprueft. Kommt game-weit nur an dieser einen Stelle vor.
* **Save-Slot-Stride**: Writer kopiert 0x1230 Bytes nach `card+0x1430+slot*0x80`, die Slot-Liste liest mit
  Stride 0x80 — nominelle Ueberlappung. Fuer den Ortsnamen irrelevant (alle relevanten Felder < 0x80), fuer
  eine byte-true Save-Portierung noch zu klaeren.
* **RE2-Schreibpfad** von `0x800d46af` in den Save-Payload ist nicht disassembliert (nur der Lesepfad).

---

## 10. Reproduktionsrezepte (alles ohne Ghidra)

```bash
# Stub im Original
python -c "d=open(r'c:/workspace/git/reAi_v2/info/Re1.5/PSX.EXE','rb').read(); print(d[0x1764c:0x17654].hex(' '))"
# -> 08 00 e0 03 21 10 00 00

# Patch im Vorprojekt
python -c "d=open(r'c:/workspace/git/reAi/info/Re1.5_PATCHED/re15/PSX.EXE','rb').read(); print(d[0x1764c:0x17654].hex(' '))"
# -> 24 c2 01 08 00 00 00 00   (= j 0x80070890 / nop)

# Disasm
python c:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80026e4c --count 2
python c:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80026800 --count 16

# Savestate-Kontamination pruefen
python - <<'EOF'
import sys; sys.path.insert(0,'c:/workspace/git/reAi_v2/.claude/skills/re15-savestate-ghidra/scripts')
import re15_ss, glob
for p in sorted(glob.glob('c:/workspace/git/reAi_v2/stage_saves/*.sav')):
    try: print(p, re15_ss.Ram(p).bytes(0x80026e4c,4).hex(' '))
    except Exception as e: print(p,'ERR',e)
EOF
```
