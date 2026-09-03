# Aufgabe F — "114 Cuts mit Masken, aber ohne SLD-Atlas"

**Ergebnis in einem Satz: Die Anomalie existiert NICHT.** Alle 114 gemeldeten Cuts
tragen einen vollstaendigen, byte-true dekodierbaren SLD-Vordergrundatlas im BSS.
`sweep.json` hat diese Cuts **nie gemessen** — der Sweep las den *vorgeschnittenen*
Baum `BSS/ROOM####/BG##.BSS`, und der bricht mitten in `ROOM5001` ab.

Datum: 2026-09-03 · Werkzeuge: `analysis/esp_masken_2026-09-03/f_sweep.py` (neu),
`analysis/esp_masken_2026-09-03/f_groundtruth.json` (neu, vollstaendige Neuvermessung
aller 206 RDTs gegen die **spiel-eigene** Tabelle).

---

## 0. Der Mechanismus, den das Original benutzt (vorher NICHT dokumentiert)

Der Sweep hat den SLD-Block per Heuristik gesucht (`find_sld`: "irgendein u32 in
[0x2000,0x40000], das sich zu einem TIM entpacken laesst"). Das Original **sucht nicht** —
es liest einen **Trailer am Ende der Cut-Daten**, dessen Position aus einer
**Tabelle im Stage-Overlay** kommt. Genau wie beim BGM-Container ist das Format
trailer-getrieben.

### 0.1 Der Lader: FUN_80021bbc (Kamera-Cut-Wechsel)

Disasm (`info/Re1.5/PSX.EXE`, `off = 0x800 + addr - 0x80010000`):

```
; A = *(u16*)( *(u32*)0x800b52c8 + room_idx*0x20 + cut_idx*2 )
80021d38: lui   s0,0x800b
80021d3c: addiu s0,s0,4066                  ; s0 = &DAT_800b0fe2   (room index)
80021d44: lh    v1,4068(v1)                 ; v1 = DAT_800b0fe4    (cut index)
80021d48: lh    v0,0(s0)                    ; v0 = room index
80021d4c: sll   v1,v1,1                     ; cut*2
80021d50: sll   v0,v0,5                     ; room*0x20
80021d54: addu  v0,v0,s1                    ; s1 = *(u32*)0x800b52c8  (@0x80021c40)
80021d58: addu  v1,v1,v0
80021d5c: lhu   v0,0(v1)                    ; <-- A  (Byte-Laenge der Cut-Daten)
80021d64: lui   at,0x8019                   ; 0x80190000 = BSS-Cut-Puffer
80021d68: addu  at,v0,at
80021d6c: lw    v0,-4(at)                   ; *(u32*)(0x80190000 + A - 4)  = PRESENT-FLAG
80021d74: beq   v0,zero,0x80021df8          ; ==0  -> dieser Cut hat KEINEN Atlas
...
80021d9c: lui   s1,0x8019
80021da0: addu  at,v0,s1
80021da4: lw    s0,-8(at)                   ; *(u32*)(0x80190000 + A - 8)  = SLD-OFFSET
80021da8: lui   a2,0x801a
80021dac: addiu a2,a2,22528                 ; a2 = 0x801a5800  (Ziel: entpacktes TIM)
80021db0: addu  a0,s1,s0                    ; a0 = 0x80190000 + sld_off
80021db4: lw    a1,0(a0)                    ; a1 = *(u32*)block = entpackte Groesse
80021db8: jal   0x800c47e8                  ; SLD-Dekompressor (= SldDecoder.java)
80021dbc: addiu a0,a0,4                     ; a0 = block+4 = Flag-Byte-Strom
...
80021de4: lhu   s1,0(s0)                    ; s0 = 0x800aca4c (VRAM-Slot-Cursor)
80021de8: ori   v0,zero,0x15                ; SLOT 0x15
80021dec: jal   0x8004ee78                  ; TIM -> VRAM
80021df0: sh    v0,0(s0)
80021df4: sh    s1,0(s0)                    ; Slot-Cursor wiederherstellen
```

Rohbytes (Gegenprobe):
`@0x80021d6c` = `fc ff 22 8c` = `lw v0,-4(at)`;
`@0x80021da4` = `f8 ff 30 8c` = `lw s0,-8(at)`.

Derselbe Code steht ein zweites Mal in `FUN_8001d600` (`lw a1,0x52c8(a1)` `@0x8001d9d8`,
Bytes `c8 52 a5 8c`) — der Raum-Erst-Ladepfad.

### 0.2 Die Tabelle DAT_800b52c8 — pro Stage im Overlay, alle sechs vorhanden

`DAT_800b52c8` ist im EXE-Dump nur **gelesen** (2 XREFs); geschrieben wird sie im
Stage-Overlay. Instruktionswort `ac2252c8` = `sw v0,0x52c8(at)`:

| Stage | `sw`-Stelle | geschriebener Wert (Tabellenbasis) |
|---|---|---|
| STAGE1 | `@0x8011e97c` | `0x8011EAE4` |
| STAGE2 | `@0x80116fd4` | `0x8011713C` |
| STAGE3 | `@0x8011cfa0` | `0x8011D0F8` |
| STAGE4 | `@0x80118458` | `0x80118590` |
| STAGE5 | `@0x8011ddc8` | **`0x8011DF18`** |
| STAGE6 | `@0x80101e0c` | `0x80101E3C` |

STAGE5-Beleg (`STAGE5.BIN`, `off = addr - 0x80100000`, KEIN 0x800-Header):

```
8011ddbc: lui   v0,0x8012
8011ddc0: addiu v0,v0,-8424          ; 0x80120000-0x20E8 = 0x8011DF18
8011ddc4: lui   at,0x800b
8011ddc8: sw    v0,21192(at)         ; -> DAT_800b52c8
```

Layout: `u16 size[16]` je Raum, Stride `0x20`, Index = Raumnummer (die beiden
Varianten `ROOMxxx0`/`ROOMxxx1` teilen sich Zeile UND BSS-Datei).

Zeile ROOM503 (`STAGE5.BIN` Datei-Offset `0x1DF78` = `@0x8011DF78`):

```
04 50 5c 76 78 9f a8 80 30 86 60 7c 48 76 9c 4b 70 62 88 73 90 4b 90 6c 08 00 08 00 08 00 08 00
= 5004 765c 9f78 80a8 8630 7c60 7648 4b9c 6270 7388 4b90 6c90 | 0008 x4 (ungenutzt)
```

Zeile ROOM117 (`STAGE1.BIN` Datei-Offset `0x1EDC4` = `@0x8011EDC4`), zur Eichung:

```
24 57 e4 5e d8 65 c4 60 34 78 a0 6b d0 50 9c 0a 3c 79 28 63 50 7b 28 79 5c 41 08 00 08 00 08 00
```

### 0.3 Der Trailer im BSS-Chunk (Datei-Byte-Offsets, verifiziert)

`ROOM503.BSS` Chunk 1 (= ROOM5030 Cut 1, Datei `0x10000..0x1FFFF`), Tabellenwert
`A = 0x765C`:

```
chunk+0x7650: f0 ff ff f4 | 84 5a 00 00 | 01 00 00 00 | 00 00 00 00
                            ^A-8=0x7654   ^A-4=0x7658    ^A=0x765c (Null-Padding)
              sld_off = 0x00005A84        present = 1
chunk+0x5A84: 20 02 01 00 | d0 10 00 00 00 09 ff c1 0c 02 ff b1
              size=0x10220=66080  | SLD-Flag-Byte-Strom
```

`ROOM117.BSS` Chunk 1 (STAGE1, Kontrolle), `A = 0x5EE4`:

```
chunk+0x5EDC: cc 4f 00 00 | 01 00 00 00 | 00 ...  -> sld_off=0x4FCC, present=1
chunk+0x4FCC: 20 02 01 00 | d0 10 00 00 00 09 ff c1 0c 02 ff b1   (BYTE-IDENTISCHER Header)
```

Ist kein Atlas da, liegt `A-8` im Null-Padding hinter dem VLC-Strom, also
`present == 0`, und `beq @0x80021d74` springt. (Beispiel ROOM117 Cut 0: `A=0x5724`,
`*(u32*)(0x5720)==0`.)

---

## 1. Teilaufgabe 1 — Unabhaengige Verifikation: liegt dort wirklich kein SLD-Block?

**Doch, er liegt dort.** Zwei voneinander unabhaengige Verfahren, beide positiv:

**(a) Spiel-Verfahren** (Tabelle `@0x8011DF18` + Trailer, s. 0.): fuer alle 57
gemeldeten Leon-Cuts (x2 Spiegel = 114) steht `present == 1` und ein gueltiger
SLD-Block. 86 Atlanten in STAGE5 insgesamt, **0 Fehlschlaege beim Entpacken**.

**(b) Der vorhandene Scanner** `analysis/esp_masken_2026-09-03/sld.py::find_sld`:
findet in **denselben** Chunks **denselben** Offset. Der Scanner war nie das Problem.

Gesamt-Kreuzprobe ueber **alle 1119 BSS-Chunks aller 6 Stages**:

| | Anzahl |
|---|---|
| Tabelle UND `find_sld` finden denselben Block | 359 |
| nur Tabelle | 0 |
| nur `find_sld` | 1 (ROOM305 Cut 14, s. 5.) |

Beispiel-Belege (Datei-Byte-Offsets, alle entpacken zu 66080 = `0x10220`):

| Raum/Cut | BSS-Datei | Chunk | `A` (Tabelle) | Trailer @A-8 | SLD-Offset | entpackt |
|---|---|---|---|---|---|---|
| ROOM5030 c1 | ROOM503.BSS | 1 | 0x765C | `84 5a 00 00 / 01 00 00 00` | 0x5A84 | 66080 |
| ROOM5030 c2 | ROOM503.BSS | 2 | 0x9F78 | — | 0x79D0 | 66080 |
| ROOM5040 c0 | ROOM504.BSS | 0 | 0xC28C | — | 0x76B0 | 66080 |
| ROOM5060 c4 | ROOM506.BSS | 4 | 0x9DCC | — | 0x4AC4 | 66080 |
| ROOM5120 c3 | ROOM512.BSS | 3 | 0xA7A8 | — | 0x5780 | 66080 |
| ROOM5140 c2 | ROOM514.BSS | 2 | 0xC6E8 | — | 0x77C0 | 66080 |

Die entpackten TIMs sind strukturell identisch zu STAGE1:

```
magic 0x00000010, flags 0x00000009 (8 bpp + CLUT)
CLUT-Block: bnum=524,   x=0, y=480, w=256, h=1     (256 Farben)
IMG-Block : bnum=65548, x=0, y=0,   w=128, h=256   (= 256 x 256 8-Bit-Pixel)
```

**Ein anderer Codec / eine andere Groesse war nicht noetig** — die Groesse ist exakt
dieselbe wie in STAGE1 (`0x10220`), und die vom Scanner geforderte TIM-Magic `0x10`
steht auch da. Die Annahme "Scannergrenzen zu eng" trifft nicht zu.

### Warum sweep.json dann `null` meldet

`sld: null` in `sweep.json` ist **nicht** "nichts gefunden" (das ist `[]`), sondern
"gar nicht gemessen". Belege:

- `sld: null` tritt auf in genau 51 Raeumen: `ROOM5001` (nur Cuts 5..8) und
  **alle** Raeume ab `ROOM5010` inklusive ganz STAGE6.
- `ROOM5000`: alle 9 Cuts gemessen. `ROOM5001`: Cuts 0..4 gemessen, 5..8 `null`.
- Verzeichnis `re15_port/shared_assets/PSX/BSS/ROOM5001/` enthaelt **exakt**
  `BG00.BSS BG01.BSS BG02.BSS BG03.BSS BG04.BSS` — fuenf Dateien, danach nichts.
  `BSS/ROOM5000/` hat BG00..BG08 (neun).
  `ls -d BSS/ROOM5*` / `BSS/ROOM6*` liefert **nur** `ROOM5000` und `ROOM5001`.

Der Sweep las also den vorgeschnittenen Baum, nicht die Originale
`STAGE5/ROOM50*.BSS` — und dieser Baum endet physisch bei `ROOM5001/BG04.BSS`.
Rechenprobe: alte Zahlen `366` (Maske+SLD) + `114` (die "Anomalie") = **480** = meine
Zahl der Cuts mit Masken. Und `130` + `110` (ungemessene STAGE5/6-Cuts mit Atlas)
minus `2` (ROOM3050/3051 c14, s. 5.) = **238** = meine Zahl "Atlas ohne Masken".
Beide Bilanzen gehen exakt auf.

Das ist derselbe Fehler wie in Memory `reai-v2-schiene-abdeckung`: die Messschiene
sah den gemeldeten Fall NIE.

---

## 2. Teilaufgabe 2 — Woraus sampelt das Original? (Antwort trotzdem belegt)

Da der Atlas existiert, ist die Frage "BG-Re-Blit statt Atlas-Blit?" hinfaellig — sie
ist aber ohnehin **negativ belegbar**, und zwar aus zwei hartcodierten Konstanten:

### TPage = 0x95, hartcodiert (FUN_80039590, der Zeichner)

```
80039630: ori  a3,zero,0x95        ; bytes 95 00 07 34
80039634: jal  0x80069858          ; SetDrawMode(p,1,1,0x95,NULL)
```

`tpage 0x95` = `tp=01` (8-Bit + CLUT), `x = (0x95 & 0x0F)*64 = 320`,
`y = ((0x95>>4)&1)*256 = 256`, also **VRAM (320,256), 8-Bit-indiziert**.

Das ist genau die Adresse, an die `FUN_8004ee78` den Atlas legt, wenn
`FUN_80021bbc` vorher `DAT_800aca4c = 0x15` setzt (`@0x80021de8`):

```
sVar4 = 0x15 * 0x40 = 0x540;  0x15 > 0x0F  ->  sVar4 -= 0x400  ->  x = 320
prect->y = (0x15 < 0x10 ^ 1) << 8            ->  y = 256
```

Der Hintergrund-Framebuffer ist 16-Bit-Direktfarbe und liegt woanders
(`StoreImage(&DAT_80072f2c, &DAT_80198000)` in derselben Funktion) — er ist mit einer
8-Bit-CLUT-TPage gar nicht sampelbar. **Es ist kein BG-Re-Blit.**

### CLUT = 0x7800, hartcodiert (FUN_800392d4, der Parser)

```
80039498: ori  v0,zero,0x7800      ; bytes 00 78 02 34
800394fc: sh   v0,0(s1)            ; bytes 00 00 22 a6   -> SPRT.clut (Puffer B)
80039500: sh   v0,0(s0)            ; bytes 00 00 02 a6   -> SPRT.clut (Puffer A)
```

`0x7800 = getClut(0,480)` = VRAM (0,480) — exakt das Ziel von
`pTVar1->crect->y = DAT_800aca4d + 0x1e0` in `FUN_8004ee78` und exakt der
CLUT-Rechteck-Eintrag im entpackten TIM (`x=0, y=480, w=256, h=1`).

### Nebenbefund: das Group-Feld +0x2 wird vom Original NICHT gelesen

`re15_pri.h` nennt `baseRaw` (Group `+0x2`) "likely TPage". Gemessen ist es die
**CLUT-Id** (`0x7800` = (0,480), `0x7C00` = (0,496)) — und `FUN_800392d4` liest es
nirgends; der Zeichner nimmt in allen Faellen die Konstante `0x7800`
(`@0x80039498`). ROOM5060 Cut 4 deklariert in den Daten `0x7C00`, gezeichnet wird
trotzdem mit `0x7800`. Der Port ignoriert das Feld ebenfalls, also **byte-true, kein
Handlungsbedarf**; der Kommentar in `re15_pri.h` sollte nur korrigiert werden.

### srcX/srcY-Bereiche der STAGE5-Masken (passen in den 256x256-Atlas)

| Stage | max srcX+w | max srcY+h |
|---|---|---|
| 1 | 256 | 240 |
| 2 | 232 | 144 |
| 3 | 256 | 256 |
| 4 | 256 | 128 |
| 5 | 256 | 240 |

Alle innerhalb 256x256. **Kein Wert zeigt in den BG-Bereich** (der waere 320 breit).

---

## 3. Teilaufgabe 3 — Sind die STAGE5-BSS unvollstaendig/anders?

**Nein.** Vollstaendigkeitspruefung ueber alle 120 BSS-Dateien:

- Jede STAGE5-BSS-Datei ist ein exaktes Vielfaches von `0x10000`.
- Jede hat `chunks >= max(nCut(ROOMxxx0), nCut(ROOMxxx1))`.
- Chunk-Header identisch (`id = 0x3800` an `+0x02` in jedem Chunk).
- SLD-Blockheader byte-identisch zu STAGE1 (s. 0.3).
- Die Stage-Tabelle `@0x8011DF18` ist vollstaendig gefuellt (21 Zeilen, ROOM500..ROOM514).

| Stage | BSS-Dateien | Cuts mit Atlas | Cuts mit Masken |
|---|---|---|---|
| 1 | 40 | 244 | 228 |
| 2 | 16 | 30 | 30 |
| 3 | 16 | 154 | 76 |
| 4 | 16 | 66 | 32 |
| 5 | 24 | **172** | **114** |
| 6 | 8 | 52 | 0 |

STAGE5 hat sogar **mehr** Atlanten als STAGE2 und STAGE4 zusammen. Von "weniger
fertig" keine Spur.

Die 17 4-Byte-Stubs (`ROOM127, ROOM20C-20F, ROOM30F, ROOM406, ROOM40C-40F,
ROOM515-517, ROOM605-607`) sind Platzhalter-Raeume und verteilen sich ueber **alle**
Stages (1/4/1/5/3/3) — kein STAGE5-Spezifikum.

---

## 4. Teilaufgabe 4 — Was ist wirklich kaputt, und was MUSS der Port tun

Die echte Anomalie ist eine andere: **302 Cuts haben einen Atlas im BSS, aber es
wurde nie eine `PRI##.TIM` extrahiert.** `re15_pri_load_cut_atlas`
(`re15_port/platform/pc/src/bg_pc.c:63`) liest ausschliesslich die vorextrahierte
Datei; fehlt sie, kommt `return 0`, damit `has_fg == 0`, damit ruft
`re15_port/platform/pc/main.c` (~Z.4450) `re15_render_pc_set_pri_rects(NULL,...,0)`
und der **Overdraw ist komplett aus**. Der Spieler laeuft dort VOR
Gelaendern/Kisten/Saeulen statt dahinter.

Davon tragen **188 Cuts echte Maskenrecords** (die restlichen 114 sind maskenlos und
harmlos):

| Raum | betroffene Cuts | Masken ges. | Atlasgroesse | max srcX+w | max srcY+h |
|---|---|---|---|---|---|
| ROOM1020 / ROOM1021 | 0,1,2,3,4,5,6,7,8,9,10,12 | 364 | 61984 | 256 | 160 |
| ROOM1030 / ROOM1031 | 0,1,2,3,4,5,6,7,9,10,11,12 | 371 | 61984 | 256 | 224 |
| ROOM1040 / ROOM1041 | 1,2,3,4,6 | 209 | 61984 | 256 | 184 |
| ROOM1070 / ROOM1071 | 0,1,2,3,4,5,6,7 | 267 | 61984 | 256 | 136 |
| ROOM5030 / ROOM5031 | 1,2,3,4 | 79 | 66080 | 248 | 72 |
| ROOM5040 / ROOM5041 | 0,1,2,3,4,6 | 324 | 66080 | 256 | 224 |
| ROOM5060 / ROOM5061 | 1,2,3,4,5,6,7,8,9,10,11,12 | 657 | 66080 | 256 | 240 |
| ROOM50A0 / ROOM50A1 | 1,2,3,4,5,6,7,8,10,11 | 221 | 66080 | 256 | 160 |
| ROOM50C0 / ROOM50C1 | 1,2,3,4 | 76 | 66080 | 208 | 104 |
| ROOM5110 / ROOM5111 | 1,2,3,4 | 105 | 66080 | 256 | 96 |
| ROOM5120 / ROOM5121 | 0,1,2,3,4,6 | 279 | 66080 | 256 | 224 |
| ROOM5140 / ROOM5141 | 1,2,3,4,5,6,7,8,9,10,12 | 313 | 66080 | 256 | 160 |
| **Summe** | **188 Cuts** | | | | |

Die STAGE5-Haelfte davon (57 Leon-Cuts x2 = **114**) ist exakt die Menge, die als
"Anomalie 2" gemeldet wurde:
ROOM5030 [1-4], ROOM5040 [0-4,6], ROOM5060 [1-12], ROOM50A0 [1-8,10,11],
ROOM50C0 [1-4], ROOM5110 [1-4], ROOM5120 [0-4,6], ROOM5140 [1-10,12] plus Spiegel.
Es fehlt dort **nicht der Atlas**, sondern nur dessen Extraktion.

Die STAGE1-Haelfte — ROOM1020/1030/1040/1070, 74 Cuts — ist die im Auftrag als
"Anomalie 3" gefuehrte 61984-Byte-Gruppe. Zur Klarstellung: `61984 = 0xF220` ist ein
**voellig gueltiger** TIM, nur `256x240` statt `256x256`
(`IMG-Block: bnum=61452, x=0, y=0, w=128, h=240`); die Masken dieser Raeume sampeln
maximal `srcY+h = 224` und passen. Auch hier fehlt nur die Extraktion.

Ausserdem 114 maskenlose Cuts mit Atlas in
`ROOM1220/1221, ROOM4040/4041, ROOM5020/5021, ROOM5050/5051, ROOM5090/5091,
ROOM50B0/50B1, ROOM5100/5101, ROOM6000/6001, ROOM6010/6011, ROOM6030/6031`
(Atlas wird geladen, aber nichts gezeichnet; das ist im Original genauso, s. 5.).

### Konkrete Pflicht-Massnahmen

**P1 (Kern, PC + PSX) — Atlas aus der Original-BSS lesen statt aus `PRI##.TIM`.**
Genau die Kur, die der BG-Lader schon hat: `bg_pc.c` faellt seit 2026-08-26 auf
`STAGE%u/ROOM%03X.BSS` + Scheibe `cut*0x10000` zurueck, weil der vorgeschnittene
Baum unvollstaendig ist (Kommentar `bg_pc.c:236-250`). `re15_pri_load_cut_atlas`
hat diesen Rueckfall **nicht**. Zu tun:

1. **SLD-Dekoder in C** in die Engine (`engine/src/`) — heute existiert er nur in Java
   (`src/main/java/de/re15/extractors/bss/SldDecoder.java` = `FUN_800c47e8`);
   `analysis/esp_masken_2026-09-03/sld.py` ist der lauffaehige Python-Port.
2. **Lokalisierung byte-true** wie das Original: Trailer bei `A-8`/`A-4` mit `A` aus
   der Stage-Tabelle. Die Tabellen liegen im Paket (`BIN/STAGE{1..6}.BIN`) an
   `0x8011EAE4 / 0x8011713C / 0x8011D0F8 / 0x80118590 / 0x8011DF18 / 0x80101E3C`
   (Datei-Offset = vaddr minus `0x80100000`), Zeile = Raumnummer, Stride `0x20`,
   Eintrag = Cut. Das ist der 1:1-Pfad und braucht keine Heuristik.
   (Fallback `find_sld` nur, falls die Tabelle nicht geladen ist.)
3. Reihenfolge im Lader: `BSS/ROOM%04X/PRI%02d.TIM` (falls da), sonst
   `STAGE%u/ROOM%03X.BSS` + Scheibe + Trailer + SLD.

**P2 (Alternative/Zwischenschritt) — die 302 fehlenden `PRI##.TIM` extrahieren.**
Der Java-Extraktor kann es bereits (`BssExtractor.processRoom` ruft
`SldDecoder.findBlock`); der Lauf war schlicht bei `ROOM5001` abgebrochen. Zielpfad
`shared_assets/PSX/BSS/ROOM%04X/PRI%02d.TIM`. Das behebt das Symptom, laesst den
Port aber weiter von einem Zwischenprodukt abhaengen.

**P3 (PSX-Backend, unabhaengiger Zweitfehler) — `RE15_PRI_VRAM_ROWS 128` reicht nicht.**
`re15_port/platform/psx/src/pri_psx.c:34` laedt nur 128 Zeilen des Atlas hoch
("die ROOM1170-Masken sampeln nur `srcY<=88`"). Gemessen ueber alle Raeume:
`max(srcY+h)` = **256** (ROOM3070 Cut 4), STAGE5 = **240**, STAGE1 = **240**.
Mit 128 Zeilen fehlt in ROOM5060/5040/5120/1030 die untere Atlashaelfte, also Muell
oder Loecher. Es muessen **alle** `t.height` Zeilen hoch (256 max).

**P4 (PSX-Backend) — VRAM-Platzierung dokumentieren/pruefen.**
Das Original legt den Atlas auf **(320,256)** mit CLUT **(0,480)** und zeichnet mit
festem `tpage 0x95` / `clut 0x7800` (`@0x80039630`, `@0x80039498`). Der PSX-Port
benutzt (512,256)/(0,502) und eigene Handles — das ist bewusst und OK, muss aber bei
einem 256-Zeilen-Upload gegen die Prop-Slots neu geprueft werden (der Kommentar in
`pri_psx.c:26-33` warnt selbst davor).

**P5 (Doku) — `re15_pri.h` korrigieren:** Group `+0x2` ist die **CLUT-Id**, nicht die
TPage, und wird vom Original nicht gelesen (s. 2.).

**P6 (Messhygiene) — Sweep-Werkzeuge muessen ihre Abdeckung ausgeben.**
`sweep.json` unterschied `[]` (gemessen, nichts gefunden) und `null` (nicht gemessen)
nur implizit; die Auswertung hat beides als "kein Atlas" gelesen. Jede kuenftige
Sammelschleife MUSS am Ende ausgeben, wie viele Einheiten sie tatsaechlich
angefasst hat, und bei Abweichung von der Erwartung abbrechen.
(Memory `reai-v2-schiene-abdeckung`.)

---

## 5. Zwei Nebenbefunde

**(a) ROOM3050/ROOM3051 Cut 14 — echte Daten-Inkonsistenz im Original.**
Tabellenzeile ROOM305 (`@0x8011D0F8 + 5*0x20`) enthaelt fuer Cut 14 den Wert
`0x77CC`. Im Chunk liegt bei **genau** `0x77CC` der SLD-Block (`find_sld`:
`0x77cc`, entpackt 66080) — der Wert ist also der Block-Offset, nicht die
Datenlaenge. Das Original liest folglich `*(u32*)(chunk+0x77C4)=0xEBFF6158` als
Offset und `*(u32*)(chunk+0x77C8)=0` als Flag, also **`present == 0`: das Original
laedt den Atlas nicht**. Cut 14 hat 0 Masken, ist also folgenlos. Unser
Java-Extraktor hat `PRI14.TIM` trotzdem geschrieben (`BSS/ROOM3050/PRI14.TIM`) — die
einzige der 720 Stellen, an der Extraktion und Spiel auseinanderlaufen. Der Port darf
sie laden, zeichnet aber ohnehin nichts.

**(b) 238 Cuts laden einen Atlas ohne jede Maske.** Auch das ist byte-true: der
Flag-Test `@0x80021d74` haengt nur am BSS-Trailer, der Maskentest `@0x8003932c`
(`pri_offset == 0xFFFFFFFF`) haengt am RDT. Beide sind unabhaengig. Der Atlas wird
also in VRAM geladen und nie gesampelt. Kein Fehler, kein Handlungsbedarf.

---

## 6. Reproduktion / erzeugte Dateien

Kreuzprobe Tabelle vs. `find_sld` ueber alle 1119 Chunks lieferte:
`chunks 1119  both 359  table-only 0  findsld-only 1` (ROOM305 c14).

- `analysis/esp_masken_2026-09-03/f_sweep.py` — RDT-Cut-/PRI-Parser + BSS-Trailer-/SLD-Leser
- `analysis/esp_masken_2026-09-03/f_groundtruth.json` — pro Raum: `nCut`, `hdr7`,
  Tabellenzeilen-Adresse, je Cut `masks`, `atlas=[offset,size]`, `tim` (PRI##.TIM vorhanden)

---

## Gegenpruefung (unabhaengiger Agent, 2026-09-03)

Auftrag: die 15 Behauptungen oben WIDERLEGEN. Jede zitierte Adresse wurde roh aus
`info/Re1.5/PSX.EXE` bzw. `info/Re1.5/PSX/BIN/STAGE*.BIN` disassembliert (eigener
MIPS-R3000-Dekoder, `off = 0x800 + addr - 0x80010000` fuer die EXE, `off = addr - 0x80100000`
fuer die Overlays), jede Datenbehauptung aus den Original-Bytes NEU erhoben —
`f_groundtruth.json` wurde NICHT gelesen.

Werkzeug: `analysis/esp_masken_2026-09-03/g_verify.py` (neu),
Ausgabe `analysis/esp_masken_2026-09-03/g_gegenpruefung.json` (neu).

### Ergebnis: 13 von 15 Behauptungen halten unveraendert stand, 2 sind in Teilen falsch

Meine unabhaengige Neuvermessung (240 RDTs, davon 34 4-Byte-Stubs = 206 echte, 2188 Cuts):

```
Cuts mit Masken:                        480
Cuts mit Masken OHNE gueltigen Atlas:     0     <-- Kernaussage BESTAETIGT
Cuts mit Atlas:                         718
Cuts mit Atlas ohne Masken:             238
Kreuzprobe Tabelle vs. find_sld:  chunks 1119  both 359  table-only 0  findsld-only 1 (ROOM305 c14)
Atlasgroessen:                    66080 x640, 61984 x76, 58400 x2 (ROOM4040/4041 cut 5)
Atlas je Stage:  S1=244 S2=30 S3=154 S4=66 S5=172 S6=52
Masken je Stage: S1=228 S2=30 S3=76  S4=32 S5=114
```

Alle diese Zahlen sind mit dem Bericht identisch.

### WIDERLEGT 1 — Behauptung 7: "+0x2 ist die CLUT-Id (0x7800/0x7C00)"

Die **erste Haelfte** der Behauptung ist belegt und bleibt stehen: das Feld wird vom Original
**nie gelesen**, der Zeichner setzt die Konstante `0x7800`.
Nachgeprueft: `@0x80039498 ori v0,zero,0x7800` (Bytes `00 78 02 34`), gespeichert
`@0x800394fc sh v0,0(s1)` / `@0x80039500 sh v0,0(s0)` (Bytes `00 00 22 a6` / `00 00 02 a6`)
auf Prim-Offset `+0x1a` = `SPRT.clut` bei Stride `0x20`. Im Decompile `FUN_800392d4` liest
`psVar13` vom Group-Record nur `+0`, `+4`, `+6` — `+2` kommt nirgends vor. Bestaetigt.

Die **zweite Haelfte** — die Identifikation als CLUT-Id — ist NICHT belegt und wird durch die
Daten widerlegt. Vollzaehlung aller 2750 Group-Records ueber alle 206 RDTs:

| Wert | Vorkommen | als getClut(x,y) |
|---|---|---|
| `0x7800` | 2712 | (0,480) — plausibel |
| `0x7C00` | 26 | (0,496) — plausibel |
| `0x7C10` | 8 | (256,496) — plausibel, im Bericht nicht erwaehnt |
| `0x8080` | 2 | **y = 0x8080>>6 = 514 — ausserhalb des VRAM (max y = 511)** |
| `0x8088` | 2 | **y = 514 — ausserhalb des VRAM** |

Fundstellen der beiden unmoeglichen Werte: `ROOM1210.RDT` / `ROOM1211.RDT` Cut 4,
Groups 32 und 33 (`base=0x8080` bzw. `0x8088`). Genau diese beiden Groups sind die
zwei, die ueber die deklarierte Maskenzahl hinausgehen (Cut 4 deklariert 75, die Groups
summieren zu 77 — siehe `re15_port/engine/src/pri_common.c`). Ein Feld, das in 4 von 2750
Faellen keine gueltige CLUT-Id sein kann, ist damit nicht als CLUT-Id nachgewiesen.

Konsequenz: Massnahme **P5** im Bericht ("`re15_pri.h` korrigieren: Group `+0x2` ist die
CLUT-Id") ersetzt eine unbelegte Vermutung ("likely TPage") durch eine andere. Byte-true
belegt ist nur: **das Feld wird vom Original nicht gelesen; seine Bedeutung ist NICHT
BELEGT.** So — und nur so — gehoert es in den Header-Kommentar.

### WIDERLEGT 2 — Behauptung 8: "Alle 24 STAGE5-BSS: Groesse exaktes Vielfaches von 0x10000"

Falsch. Gemessen (`os.path.getsize`):

```
info/Re1.5/PSX/STAGE5/ROOM515.BSS   4 Bytes
info/Re1.5/PSX/STAGE5/ROOM516.BSS   4 Bytes
info/Re1.5/PSX/STAGE5/ROOM517.BSS   4 Bytes
```

3 der 24 STAGE5-BSS sind 4-Byte-Stubs und damit **kein** Vielfaches von `0x10000`. Der Bericht
widerspricht sich hier selbst: zwei Absaetze spaeter fuehrt er `ROOM515-517` korrekt in seiner
eigenen Stub-Liste. Die uebrigen Teilaussagen von Behauptung 8 sind exakt reproduziert
(21/24 Dateien Vielfaches von `0x10000`; `chunks >= max(nCut)` in 120/120 Dateien, 0 Verstoesse;
Chunk-Header `id=0x3800` an `+0x02` in **allen** 1119 Chunks, 0 Abweichungen; Stub-Verteilung
1/4/1/5/3/3 = 17 Raum-Paare = 34 RDTs). Die Schlussfolgerung ("STAGE5 ist nicht anders
strukturiert") bleibt unberuehrt.

### Bestaetigt (mit eigener Gegenprobe)

- **1** — Disasm `@0x80021d30..0x80021df8` roh nachgeschlagen: `@0x80021d5c lhu v0,0(v1)`;
  `@0x80021d6c lw v0,-4(at)` = `fc ff 22 8c`; `@0x80021d74 beq v0,zero,0x80021df8`;
  `@0x80021da4 lw s0,-8(at)` = `f8 ff 30 8c`; `@0x80021db4 lw a1,0(a0)`;
  `@0x80021db8 jal 0x800c47e8`; `@0x80021de8 ori v0,zero,21` (=0x15). Tabellenzeiger
  `s1 = *(u32*)0x800b52c8` stammt aus `@0x80021c40 lw s1,21192(s1)` (`c8 52 31 8e`).
  Zweiter Leser `@0x8001d9d8 lw a1,0x52c8(a1)` (`c8 52 a5 8c`) mit identischer Indizierung
  (`sll v1,a2,1` / `sll v0,a0,5`). Alles wortgleich.
- **2** — Bytemuster `c8 52 22 ac` (`sw v0,0x52c8(at)`) kommt in STAGE1..6.BIN je **genau
  einmal** vor, an den behaupteten Adressen. Die davor stehenden `lui/addiu`-Paare ergeben
  0x8011EAE4 / 0x8011713C / 0x8011D0F8 / 0x80118590 / 0x8011DF18 / 0x80101E3C — alle sechs
  Werte nachgerechnet.
- **3** — Beide Byte-Dumps stimmen wortwoertlich (STAGE5 `0x1DF78`, STAGE1 `0x1EDC4`).
  Stride `0x20` = 16 u16 ist konsistent: `max(nCut)` ueber alle RDTs = **16** (ROOM1090).
  Zusaetzliche, im Bericht fehlende Gegenprobe zur Semantik von `A`: der SLD-Strom endet
  **exakt** bei `A-8` — ROOM503.BSS Chunk 1 dekomprimiert von `0x5A88` bis `0x7654`,
  `A-8 = 0x7654`, Delta **0**; ROOM117.BSS Chunk 1 endet `0x5EDA` gegen `A-8 = 0x5EDC`,
  Delta 2 (Padding). Damit ist `A` = Ende der Cut-Daten tatsaechlich belegt.
- **4** — Eigene Vermessung: 480 Cuts mit Masken, davon **0** ohne gueltigen Atlas. Der
  Blockheader `20 02 01 00 d0 10 00 00 00 09 ff c1 0c 02 ff b1` steht byte-identisch bei
  ROOM503.BSS `+0x15A84` und ROOM117.BSS `+0x14FCC`.
- **5** — Eigene TIM-Dekodierung: `magic=0x10, flags=0x9`, CLUT `bnum=524 x=0 y=480 w=256 h=1`,
  IMG `bnum=65548 x=0 y=0 w=128 h=256`, Summe 66080 — identisch fuer ROOM5030 c1 /
  ROOM5060 c4 / ROOM5120 c3 / ROOM1170 c1. Die Laengenfelder summieren exakt auf die im
  SLD-Block deklarierte Groesse (8+524+65548 = 66080), was den Dekoder unabhaengig stuetzt.
- **6** — `@0x80039630 ori a3,zero,149` (`95 00 07 34`) + `@0x80039634 jal 0x80069858`;
  Decompile `FUN_80039590.c:17` `SetDrawMode(p,1,1,0x95,(RECT *)0x0)`. tpage 0x95 =
  8bpp+CLUT, VRAM (320,256). Ziel-Uebereinstimmung nachgerechnet: `FUN_8004ee78` mit
  `DAT_800aca4c=0x15` -> `x = 0x15*0x40-0x400 = 320`, `y = ((0x15<0x10)^1)<<8 = 256`;
  `crect->y = DAT_800aca4d + 0x1e0 = 480`, weil der Halbwort-Store `sh v0,0(s0)` mit
  `v0=0x0015` das Byte `0x800aca4d` auf 0 setzt. **Zusatzbeleg gegen den BG-Re-Blit, den der
  Bericht nicht bringt:** die BG-RECT `DAT_80072f2c` enthaelt im EXE-Image
  `00 00 00 00 40 01 f0 00` = **RECT(0,0,320,240)** — der Framebuffer liegt also bei (0,0),
  nicht bei (320,256).
- **9** — `sweep.json` nachgezaehlt: `sld == null` in 510 Cuts; 50 Raeume komplett null
  (ROOM5010..ROOM5141 + ganz STAGE6), ROOM5001 teilweise (Cuts 5..8) = 51 Raeume mit Nulls.
  Klassen exakt reproduziert: `366 / 114 / 130 / 1578`, Summe 2188. **Alle 114** gemeldeten
  "Masken ohne Atlas"-Cuts haben `sld == null`, waren also nie gemessen. Ungemessene Cuts mit
  Atlas: 114 mit Masken + **110** ohne Masken — die Bilanz `130 + 110 - 2 = 238` geht auf.
  Verzeichnisbefund reproduziert (`BSS/ROOM5001/` = BG00..BG04, `BSS/ROOM5000/` = BG00..BG08,
  sonst nichts aus STAGE5/6).
- **10** — Kreuzprobe komplett neu gefahren: `chunks 1119, both 359, table-only 0,
  findsld-only 1` = ROOM305.BSS Chunk 14 (`A=0x77CC`, find_sld findet `0x77cc`).
  Detail bestaetigt: `*(u32*)(chunk+0x77C4) = 0xEBFF6158`, `*(u32*)(chunk+0x77C8) = 0`
  -> `present == 0`. `BSS/ROOM3050/PRI14.TIM` existiert im extrahierten Baum — die einzige
  Stelle, an der Extraktion und Spiel auseinanderlaufen.
- **11** — Eigene Zaehlung: `(masks>0, atlas, kein PRI##.TIM) = 188`,
  `(masks=0, atlas, kein PRI##.TIM) = 114`. Die Raum-/Cut-Listen stimmen Eintrag fuer
  Eintrag mit der Tabelle in §4 (u.a. ROOM1020 [0-10,12] = 12, ROOM5140 [1-10,12] = 11).
  Auch die maskenlose Raumliste (1220/4040/5020/5050/5090/50B0/5100/6000/6010/6030 je x2)
  ist identisch. Port-Code nachgelesen: `bg_pc.c:63` `re15_pri_load_cut_atlas`, `return 0`
  bei fehlender Datei; `main.c:4451/4455/4471` `has_fg && pri_n > 0` sonst
  `re15_render_pc_set_pri_rects(..., 0)`.
- **12** — `pri_psx.c:34 #define RE15_PRI_VRAM_ROWS 128`, `:59 rows = (t.height < ...)`.
  Eigene Maskenvermessung: global `max(srcY+h) = 256` bei **ROOM3070 Cut 4, srcY=184, h=72**;
  je Stage maxX/maxY = 256/240, 232/144, 256/256, 256/128, 256/240 — Zeile fuer Zeile wie
  im Bericht. **Zusatzpruefung, die der Bericht nicht macht:** kein einziger Cut sampelt ueber
  die Hoehe SEINES EIGENEN Atlas hinaus (0 Verstoesse, auch nicht in den 61984-B-Raeumen).
- **13** — `bg_pc.c` Rueckfall `STAGE%u/ROOM%03X.BSS` + Scheibe `cut_idx * 0x10000` vorhanden;
  `re15_pri_load_cut_atlas` kennt nur `BSS/ROOM%04X/PRI%02d.TIM` und den flachen Altpfad.
- **14** — `src/main/java/de/re15/extractors/bss/SldDecoder.java:6` traegt den Kommentar
  "EXACT port of the game routine FUN_800c47e8"; unter `re15_port/engine/src/` gibt es
  keinen SLD-Dekoder (Treffer nur in Kommentaren von `bg_pc.c`, `render_pc.c`, `pri_psx.c`).
- **15** — Beide Tests nachgeschlagen und voneinander unabhaengig: Atlas-Test haengt am
  BSS-Trailer (`@0x80021d6c/@0x80021d74`), Maskentest am RDT (`@0x8003932c bne v1,v0` gegen
  `addiu v0,zero,-1`, dann `@0x80039338 sb zero,0(a0)`). Eigene Zaehlung: **238** Cuts mit
  Atlas und 0 Masken.

### Fazit der Gegenpruefung

Die Kernaussage des Berichts — **"Masken ohne Atlas" existiert nicht, `sweep.json` hat
STAGE5/6 nie gemessen, der echte Defekt sind 302 fehlende `PRI##.TIM` (188 davon mit
Masken)** — haelt der Gegenpruefung vollstaendig stand und ist mit unabhaengig erhobenen
Zahlen reproduziert. Zu korrigieren sind nur zwei Nebenaussagen: die Deutung des
Group-Feldes `+0x2` als CLUT-Id (unbelegt, durch `0x8080`/`0x8088` in ROOM1210/1211 Cut 4
widerlegt) und die pauschale Aussage, alle 24 STAGE5-BSS seien ein Vielfaches von `0x10000`
(3 Stubs a 4 Bytes).
