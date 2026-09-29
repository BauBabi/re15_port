# Runde 34 / RE — Acid Grenade (0x0A) und Incendiary Grenade (0x0B)

Auftrag: `AUFTRAG.md` (Nutzer 2026-09-29: „Handgranate … komplett lauffähig … für alle 3 Granatenarten").
Thema dieses Dossiers: Was hat RE1.5 für 0x0A/0x0B, was ist das RE2-Retail-Ziel?
Werkzeuge + Textbelege: `re_saeure_brand_werkzeug/` (Skripte `*.py`, Ausgaben `*.txt`),
Laufzeit-Ausgaben `build/r34g_saeure_brand/` (untracked). Alle RE1.5-Adressen aus
`info/Re1.5/PSX.EXE` (t_addr 0x80010000, t_size 0xAF000) über `re15_disasm.py`, RE2-Adressen aus
`info/re2leon/PSX.EXE` bzw. `info/re2leon/COMMON/BIN/*.BIN` über `re2_disasm.py`.
Stand: abgeschlossen (RE-Phase, keine Port-Änderung, keine Git-Schreiboperation).

## Gliederung

1. RE1.5 — Einordnung fertig/unfertig (1.1 Entlade-Handler · 1.2 FSM-Gate · 1.3 Byte +8 ·
   1.4 Schadensspalten/Tester · 1.5 DAT_8006f418/f430 · 1.6 CORE00.ESP-Subs · 1.6a Routinen 29/30/31 ·
   1.7 Bänke · 1.8 Farben/Bilder/Texte · 1.9 Zwischenfazit)
2. RE2 — GL-Runden (2.1 Opcode-Tabelle · 2.2 Entstehung der Runde · 2.3 Flug · 2.4 Aufschlag je Art ·
   2.5 Bodenfeuer · 2.6 Töne · 2.7 Datenorte + Port-Zugriff)
3. Folgerung (3.1 Hypothesen-Prüfung · 3.2 Schaden · 3.3 RE2-Anteil der Explosion)
4. Übersichtstabelle
KONSTANTEN FUER DEN BAU · PORT-ABGLEICH · OFFEN

---

## 1. RE1.5 — was ist für 0x0A/0x0B da?

### 1.1 Entlade-Handler 0x80033B58 / 0x80033B78 — reine Munitions-Stubs (UNFERTIG)

`re15_disasm.py table 0x80074100 22`: [9] @0x80074124 → 0x80033B38, [10] @0x80074128 → **0x80033B58**,
[11] @0x8007412C → **0x80033B78**. Alle drei Körper sind bytegleich (8 Instruktionen):

```
80033b58: addiu sp,sp,-24      80033b78: addiu sp,sp,-24
80033b5c: sw ra,16(sp)         80033b7c: sw ra,16(sp)
80033b60: jal 0x8004eae4       80033b80: jal 0x8004eae4      ; Munition -1
80033b64: nop                  80033b84: nop
80033b68: lw ra,16(sp)         80033b88: lw ra,16(sp)
80033b6c: addiu sp,sp,24       80033b8c: addiu sp,sp,24
80033b70: jr ra                80033b90: jr ra
```

Kein Effekt-Spawn (`jal 0x80019700`), kein Treffer-Resolver (`jal 0x80011f50`/`0x80012d60`), kein SE.

### 1.2 FSM-Gate @0x8003368c — Projektil nur für Id 9 (UNFERTIG für 10/11)

Standard-Sub 2 FIRE 0x80033460 (`dis 0x80033460 220`), einziger Projektil-Spawn:

```
80033680: lui v1,0x800b
80033684: lbu v1,-13731(v1)        ; 0x800ACA5D ausgerüstete Waffe
80033688: ori v0,zero,0x9
8003368c: bne v1,v0,0x800337ac     ; != 9 -> Funktionsende, KEIN Spawn
80033690: ori v0,zero,0x13         ; Bild 19 (HOCH) / 0x16 = 22 (MITTE) @800336a4 / 0x18 = 24 (TIEF) @80033758
800336bc: lui a0,0x40d / 800336c0: ori a0,a0,0x1000   ; 0x040D1000 (auch @8003371c/20, @80033778/7c)
800336ec: jal 0x80019700           ; (auch @80033748, @800337a4)
```

**Zensus aller Leser von 0x800ACA5D** (`aca5d_zensus.py`, alle `lui 0x800b`+`lbu -13731` in PSX.EXE,
DEBUG.BIN @0x800C0000, STAGE1–6/TITLE.BIN; Ausgabe `aca5d_zensus.txt`): **46 Leser, keiner vergleicht
mit 10/11 (0x0A/0x0B) oder 16/17.** Der einzige Treffer mit Konstante 10 ist @0x80033e34 = Revolver-Speedloader
(`ori v0,zero,0x7` = Id 7, dann `acae9 == 0xa` = Bild 10). Es gibt also **keinen Code-Pfad, der Acid oder
Incendiary gesondert behandelt** — weder im Wurf noch sonstwo.

### 1.3 Byte +8 des Waffen-Records (@0x80074E14+8 usw.) — Farbe der Mengenziffern, KEIN Schadenstyp

Record-Tabelle @0x80074DA8, Stride 12 (`sll v0,v1,1; addu v0,v0,v1; sll v0,v0,2` = ×12 @0x80049af4-fc),
Felder `{u32 +0 Nachlade-Portion, u32 +4 Zeiger Munitionsliste, u8 +8, u8 +9 Anzahl Munitionsarten}`
(`bytes 0x80074da8 0x120`):

| Id | Record @ | +0 | +4 | +8 | +9 |
|---|---|---|---|---|---|
| 9 Hand Grenade | 0x80074E14 | 0xFA (250) | 0x80074C88 | **3** | 0 |
| 10 Acid Grenade | 0x80074E20 | 0xFA | 0x80074C88 | **1** | 0 |
| 11 Incendiary Grenade | 0x80074E2C | 0xFA | 0x80074C88 | **2** | 0 |
| 14 Flammenwerfer | 0x80074E50 | 0x64 | 0x80074CA8 | **2** | 1 |
| 15/16/17 GL-Klasse | 0x80074E5C/68/74 | 6 | 0x80074C88 | 3 / 1 / 2 | 0 |
| 3 Browning (Vergleich) | 0x80074DCC | 0x0F | 0x80074C8C | 3 | 1 |

**Leser von +8** (Ghidra-XREF DAT_80074db0, per Disasm geprüft): @0x80049b0c, @0x80049b6c (FUN_80049a5c),
@0x8004c8a4 (FUN_8004c830), @0x8004d9fc (FUN_8004d96c). **Alle vier** laden das Byte nach `a2` und rufen
`jal 0x80048f28` — den Ziffern-Zeichner der Inventar-Menge (Division /100 `0x51eb851f` @0x80048fd0-e8,
/10 `0xcccccccd` @0x80049018-1c). Dort (@0x80048f6c `sb a2,24(sp)`):

```
8004922c: lbu v0,24(sp)            ; Byte +8
80049234: sll v0,v0,1
80049238: lui at,0x800b / 8004923c: addiu at,at,9748   ; 0x800B2614
80049244: lhu v0,0(at)             ; CLUT-Wort = DAT_800B2614[+8]
8004924c: sh v0,0(s0)              ; in das Sprite-Primitiv
```

DAT_800B2614.. wird von FUN_800460b8 gefüllt: `0x800B2614 = GetClut(0x100, 0x1EA)` @0x80046174-84,
`…16 = GetClut(0x100,0x1EB)` @0x80046188-98, `…18 = (0x100,0x1EC)` @0x8004619c-ac, `…1A = (0x100,0x1ED)`
@0x800461b0-c0 (`jal 0x8006b3d8` = GetClut, a0 = 0x100, a1 in den Delay-Slots). **Byte +8 wählt also die
CLUT-Zeile 490+n (x 256) der Mengenziffern im Inventar**: 3 = Standard (wie alle Schusswaffen),
1 = eigene Farbe der Säure, 2 = dieselbe Farbe wie der Flammenwerfer. Das ist Präsentation, kein
Spielmechanismus; es belegt aber die Zuordnung 0x0A = Säure, 0x0B = Feuer (Flammenwerfer teilt die 2).
(Der Port-Kommentar `enemy_ai_re2_zombie.c:3786-3789` nennt es „Subtyp-Byte (3 = HE, 1 = Saeure,
2 = Brand)" — die Werte stimmen, die Semantik „Ziffernfarbe" fehlt dort.)

Byte +9 (DAT_80074DB1, einziger Leser @0x8004e9d8 in FUN_8004e900 = Kombinier-Prüfung) ist für 9/10/11
**0**: keine Munition kombinierbar.

### 1.4 Waffen-Schadensspalten 10/11 @0x8006E0D0 und Tester 0x800128A0 — DATEN VOLLSTÄNDIG, nie erreicht

Adressrechnung (FUN_80011f50, Direkt-Disasm): `800124b0: lbu v1,8(s1)` (Gegnertyp) · `800124b4-b8: lui a0,0x8007;
addiu a0,a0,-7984` (0x8006E0D0) · `800124c0-d0` ×0x58 · `800124d8-dc` Waffe×4. `dmg_spalten.py`
(Ausgabe `dmg_spalten.txt`) liest alle Typen 0x00..0x3F. Echte Gegnerzeilen sind 0x10..0x37 (davor
liegen andere Tabellen: 0x0D = Tester-Zeiger @0x8006E548, 0x0E/0x0F = Reichweite @0x8006E5A0; ab 0x38
DAT_8006F410 ff.). Auszug (u16):

| Typ (Zeile @) | w9 Hand | **w10 Acid** | **w11 Incend.** | w14 Flamme | w15/16/17 GL |
|---|---|---|---|---|---|
| 0x10..0x1A Zombies (@0x8006E650..) | 100 | **200** | **100** | 10 | 100/200/100 |
| 0x20 Hund (@0x8006EBD0) | 100 | **100** | **200** | 10 | 100/100/200 |
| 0x21 Krähe | 2 | 2 | 2 | 2 | 2/2/2 |
| 0x22 (@0x8006EC80) | 200 | 50 | 50 | 10 | 200/50/50 |
| 0x23 Alligator | 70 | 30 | 30 | 8 | 70/30/30 |
| 0x25 Spinne | 100 | **50** | **200** | 10 | 100/50/200 |
| 0x27/0x28 Gorilla-Made | 40 | **40** | **70** | 8 | 40/40/70 |
| 0x29/0x2A Schabe | 50 | **100** | **200** | 15 | 50/100/200 |
| 0x2B Tyrant | 50 | **200** | **100** | 10 | 50/200/100 |
| 0x30 Birkin | 40 | **70** | **40** | 8 | 40/70/40 |
| 0x31 | 50 | 50 | 80 | 8 | 50/50/80 |
| 0x32/0x33 | 90 | 60 | 60 | 10 | 90/60/60 |
| 0x34/0x35 | 60 | 60 | 90 | 8 | 60/60/90 |
| 0x1B–0x1F, 0x24, 0x26, 0x2C–0x2F, 0x36, 0x37 | 0 | 0 | 0 | 0 | 0 (ganze Zeile 0) |

* Säure und Brand sind je Gegnertyp **verschieden** gewichtet (Zombie/Tyrant: Säure 200 > Brand 100;
  Hund/Spinne/Schabe/Made: Brand > Säure). Die GL-Spalten 15/16/17 sind in JEDER echten Gegnerzeile
  gleich 9/10/11. Das ist ein fertig durchdachtes Elementar-Schadensmodell **als Daten**.
* Tester-Tabelle @0x8006E548 (`table 0x8006e548 22`): [9]/[10]/[11] @0x8006E56C/70/74 → **0x800128A0**
  (ebenso 14, 15–18, 20). Körper (`dis 0x800128a0`): `dist = SquareRoot0((t.x−p.x)²+(t.z−p.z)²)` (p = SVECTOR in a2, `lh v1,0(a2)` / `lh v1,4(a2)`)
  (@0x800128ac-ec), Treffer wenn `dist < t.hitbox(+0x78)->+6 + reach` (@0x800128dc-f8) und kleiner als
  der bisher nächste DAT_8008F5E0 (@0x80012904-20). Reichweite @0x8006E5A0 (`bytes 0x8006e5a0 0xb0`):
  [9]/[10]/[11] = `e8 03` = **1000** (@0x8006E5C4/C8/CC).
* **Aber:** FUN_80011f50 wird mit 9/10/11 **nie** gerufen — die drei Entlade-Handler (1.1) rufen es nicht,
  der Schrot-Zusatz ist hart `== 8` (@0x80033510-14). Die Spalten 9/10/11 und der Tester sind im
  Auslieferungsstand **tote Daten** (auch für die Hand Grenade: deren Schaden kommt aus 1.5).

### 1.5 DAT_8006F418 / DAT_8006F430 — Angriffsarten 3/4 = Säure-/Brand-Granate (DATEN, nie erzeugt)

FUN_80012d60(Radius a0, Punkt a1, Art a2) — `dis 0x80012d60 250`:

* Spieler-Zweig: `80012e38 andi a0,s7,0xff` · `80012e48-54 lhu v1,0(0x8006F418+2·Art)` · `80012e5c subu`
  HP (+0x9A) · Art < 2 → Blut/Gift-Würfel (@0x80012e58/0x80012e70-b4) · +0x04 = 2, +0x05 = Richtung+2
  (`jal 0x8001a7a8` @0x80012ec8), HP < 0 → +0x04 = 3 (@0x80012ef0).
* **Gegner-Zweig** (@0x80012f10-0x80013020): Gate A (Selbst @0x80012f4c), Gate B (+0x90&0x3000000
  @0x80012f60), Art < 2 → `jal 0x800453d0(0xa)` (@0x80012f74), dann
  ```
  80012fd4: sb zero,7(s1)
  80012fd8: sb v0,6(s1)              ; +0x06 = 1
  80012fdc: lui at,0x8007 / 80012fe0: addiu at,at,-3024 ; 0x8006F430
  80012fe4: addu at,at,s0            ; + Art
  80012fe8: lbu v0,0(at)
  80012ff0: sb v0,5(s1)              ; +0x05 (Reaktions-Waffe) = DAT_8006F430[Art]
  80012ff4: lhu a0,0(s3)             ; s3 = 0x8006F418 + 2·Art (@80012f14-20)
  80012ffc: subu v1,v1,a0 / 80013000: sh v1,154(s1)   ; HP -= DAT_8006F418[Art]  (FLACH, nicht @0x8006E0D0!)
  80013010: ori v0,zero,0x2 / 80013018: sb v0,4(s1)   ; +0x04 = 2, bei HP < 0 -> 3 (@8001301c-20)
  ```

`read 0x8006f418 12 --w 2 --signed` / `read 0x8006f430 12 --w 1`:

| Art | 0 | 1 | **2** | **3** | **4** | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Schaden DAT_8006F418 | 10 | 20 | **1000** | **1000** | **1000** | 50 | 100 | 200 | 300 | 1000 | 0 |
| +0x05 = DAT_8006F430 (Waffen-Id) | 3 | 3 | **9** | **10** | **11** | 14 | 15 | 16 | 17 | 18 | 20 |

Bytes: `8006f418: 0a 00 14 00 e8 03 e8 03 e8 03 32 00 64 00 c8 00 2c 01 e8 03 00 00` ·
`8006f430: 03 03 09 0a 0b 0e 0f 10 11 12 14 00`.

→ Art 2 = Hand Grenade (Waffe 9), **Art 3 = Acid Grenade (10), Art 4 = Incendiary Grenade (11)**, 5 = Flammenwerfer,
6/7/8 = GL Explosiv/Säure/Brand (15/16/17 mit 100/200/300), 9 = Rakete. Die Granaten-Explosion sollte also
jeden Gegner im Radius mit 1000 flach treffen und ihm als Reaktionswaffe 9/10/11 stempeln.

**Wer erzeugt Art 3/4?** `jal_xref.py 0x80012d60` über PSX.EXE + DEBUG.BIN + STAGE1–6 + TITLE.BIN
(`xref_80012d60.txt`): **genau 2 Aufrufe** — @0x80018008 (FUN_80017fa4, Gegnerangriff, `addu a2,zero,zero`
@0x80018004 = Art 0) und @0x800185b8 (Row-Routine 31, Granaten-Explosion, `ori a2,zero,0x2` @0x800185b4 =
Art 2, KONSTANT). Kein Datenwort 0x80012D60 in irgendeiner Binärdatei (kein `jalr`). **Die Arten 1, 3, 4,
5–10 werden im Auslieferungsstand nie erzeugt.** Art 3/4 sind vorbereitete, aber nicht verdrahtete Daten.

### 1.6 CORE00.ESP — alle Row-Subs von Effekt 3 und 4 (und 0/2/8)

Spawner-Semantik selbst disassembliert (FUN_80019700):
```
80019728: srl t8,a0,24             ; fx_id
8001970c: srl v0,a0,16 / 80019730: andi t7,v0,0xff   ; sub
80019734: andi v0,t7,0x7 / 80019738: sll v0,v0,1     ; (sub&7)*2 -> Sub-Offset-Tabelle
8001973c: srl v1,t7,3  / 80019754: sll s0,v1,6       ; (sub>>3)*0x40
80019744-50: lw t6, 0x800B22D4[fx_id]                ; Row-Block
8001975c: lhu v0,0(v0) / 80019770-74: t6 += off*4    ; Sub-Block
80019778: lhu t4,0(t6)                               ; Streams
8001987c: lhu v0,4(t5) / 80019884: addu v0,v0,s0 / 80019888: sh v0,50(t0)  ; CLUT = Kopf+4 + (sub>>3)*0x40
```
→ Es gibt je Effekt **höchstens 8 Row-Mengen (sub&7)**; `sub>>3` verschiebt NUR die CLUT um ganze VRAM-Zeilen
(+0x40 im CLUT-Wort = y+1). `esp_subs.py` (Ausgabe `core00_subs.txt`) listet alles; Ids der Datei: 3, 8, 0, 2, 4.

**Effekt 4** (Kopf @0x1728, CLUT-Wort 0x7AD1 = (272,491)), Sub-Tabelle @0x18C0 = `4 26 48 80 102 124 146 158`:

| sub&7 | Block | Zeilen (Routine A) | Bekannte Nutzung |
|---|---|---|---|
| 0 | @0x18D0 | 16 → 11 | Hülse Pistole (0x04000800, SPEC §2) |
| 1 | @0x1928 | 11, 11 | — |
| 2 | @0x1980 | 15, 15, 15 (p16 = 1024) | — |
| 3 | @0x1A00 | 38 → 11 | Schrothülse (0x04030920) |
| 4 | @0x1A58 | 10 → 27 | — |
| **5** | @0x1AB0 | **30**, 0 | **Granate 0x040D1000** (sub 0x0D = 5 + CLUT-Zeile +1) |
| 6 (sub 0x0E) | @0x1B08 | 39 | Routine 39 @0x80018918 = Mehrfach-Hülsen-Spawner (6 × `jal 0x800199d4` mit `lui s3,0x400` = 0x0400xxxx, @0x800189b4-0x80018c68), keine Explosion |
| 7 (sub 0x0F) | @0x1B38 | 16 → 27 | Routine 27 @0x8001810c = Zufalls-Streuung der Geschwindigkeit (`rng & 0xa/0x14`) + Routine B := 28, keine Explosion |

**Effekt 3** (Kopf @0x0008, CLUT-Wort 0x7811 = (272,480)), Sub-Tabelle @0x0384 = `4 26 48 80 102 124 146 0`:

| sub&7 | Block | Zeilen | Bekannte Nutzung |
|---|---|---|---|
| 0 | @0x0394 | A 10 (p16 96, gate 8), 0 | Rauch 0x0300xxxx |
| 1 | @0x03EC | A 10 (p16 0, gate 10), 0 | **Explosions-Kind 0x03195000** (sub 0x19 = 1, CLUT +3 Zeilen) |
| 2 | @0x0444 | 15 ×3 (p16 768) | — |
| 3 | @0x04C4 | A 10, acc (0,5,0), vel (0,−130,0), p16 64, gate 8 | **Explosions-Kind 0x030B5400/5800** (sub 0x0B = 3, CLUT +1) |
| 4 | @0x051C | A 10, vel (0,−80,0), p16 32 | — |
| 5 | @0x0574 | A 10, vel (0,−80,0), p16 96 | Flammenstrahl 0x031D1200 (sub 0x1D: &7 = 5, CLUT +3; SPEC §2 Id 14) |
| 6 | @0x05CC | A 10 (gate 25) | — |
| 7 | — | Offset 0 = kein Block | — |

Keine Row-Menge ruft eine eigene „Säure"- oder „Feuer-Explosions"-Routine; die Explosions-Kinder sind
Routine-10-Sprites (Animations-Abspieler). Farbvarianten wären in RE1.5 nur über `sub>>3` (CLUT-Zeile)
möglich; ein Aufrufer mit einer Säure-/Brand-Variante existiert nicht (1.5: Routine 31 ist der einzige
Explosions-Code, seine Kind-Codes sind Konstanten, s. §1.6a unten).

### 1.6a Row-Routinen 29/30/31 lesen die Variante NICHT

Routinentabelle @0x80071D40 (`table 0x80071d40 48`): [29] @0x80071DB4 → 0x80018320, [30] @0x80071DB8 →
0x8001843C, [31] @0x80071DBC → 0x8001854C (Datenwörter in PSX.EXE Datei 0x625B4/B8/BC, sonst nirgends).
Alle drei Körper selbst disassembliert (`dis 0x80018320 71`, `dis 0x8001843c 68`, `dis 0x8001854c 104`):
gelesen werden nur Slot +0x08/+0x10/+0x12/+0x14 (Beschl./Geschw.), +0x1E (Zünder), +0x26 (Abpraller),
+0x28/+0x2A/+0x2C (Lage), +0x2E (Gier), 0x800ACAEC (Zielhöhe) und `rng`. **Kein Lesen von +0x70 (fx_id),
+0x71 (sub) oder +0x72 (scale)**, keine Verzweigung auf eine Waffen-Id. Die Explosion (Routine 31) hat
nur Konstanten:

```
80018570: bne v1,v0,...  (v0 = 7)         ; Zünder == 7:
8001857c: sb v0,21336(at)                 ;   0x800B5358 := 1 (Lärm)
80018584: sb v0,108(a1)   (v0 = 0x61)     ;   Flags +0x6C
80018598: ori a0,zero,0x1f4               ;   Radius 500
800185a8: addiu v0,v0,-500                ;   Punkt y = +0x2A - 500
800185b4: ori a2,zero,0x2                 ;   Art 2 (fest)
800185b8: jal 0x80012d60
800185c0: lui a0,0x319 / 800185c4: ori a0,a0,0x5000 ; Kind 0x03195000
800185dc: jal 0x800199d4
800185e4: lui a0,0x408 / 800185e8: ori a0,a0,0x1    ; SE 0x04080001
800185ec: jal 0x80045024
80018604/08: Zünder == 2 -> 0x03195000 (@80018640) + 0x030B5400 (@80018648-60)
80018688-c8: Zünder == 0 -> 0x030B5800, sb zero,108(a1) (Platz frei)
```

→ Selbst wenn ein anderer Code eine Säure-/Brand-Granate mit anderem `sub` spawnen würde, liefe dieselbe
HE-Explosion mit Art 2 ab. **RE1.5 hat keinen Säure- oder Brand-Aufschlag.**

### 1.7 Waffenbänke — bytegleich

`md5sum` (re15_port/shared_assets/PSX/PLD): PL00W09 = PL00W0A = PL00W0B (`50cf41fd…`, je 26 792 B);
PL04W09 = PL04W0A = PL04W0B (`f8af5f07…`, je 28 132 B). Der Bank-Lader FUN_80036b68 wählt nur die Datei
(`lhu v0, 0x800741E8[aca5c]` @0x80036ba8-b4, `+ aca5d` @0x80036bc4, `jal 0x80013b60` @0x80036bc0) und
lädt die dir[3]-CLUT ohne Id-Abhängigkeit (`ori v0,zero,0xe0` @0x80036c88, `addiu v0,v0,481`
@0x80036c9c). **Wurfanimation, Hand-Granate und deren Farbe sind für 9/10/11 identisch** (Leon und Elza).
Zum Vergleich RE2: PL01W09/0A/0B (Claires Granatwerfer) sind ebenfalls bytegleich (`3eccb0d3…`),
Leons PL00W09/0A/0B sind 3 436-B-Stubs.

### 1.8 Farben, Item-Bilder, Icons, Texte — eigene Kunst je Sorte

* ITPS.ITP-Blöcke (Lader LAB_8001e404, `bilder.py` aus Runde 30, neu gefahren): 0x09 @0x1B000, **0x0A @0x1E000,
  0x0B @0x21000**, alle crect (0,489) prect (832,256) 56hw×72; Icons ITEMALL.PIX 0x09 @0x2A30, **0x0A @0x2EE0,
  0x0B @0x3390**, je 1200/1200 Byte belegt. Keiner bytegleich mit 0x09 oder RE2. Bild
  `re_saeure_brand_werkzeug/granaten_bilder_09_0a_0b.png`: **Hand = oliv, Acid = gelb, Incendiary = rot**
  (Bild und Icon).
* Mengenziffern: Byte +8 (1.3) → CLUT-Zeile 491 (Säure) / 492 (Brand, = Flammenwerfer) / 493 (Standard).
  Der Port setzt das schon um (`re15_inv_screen.c:1066-1069`, `clut = 2 + prop8`).
* CHECK-Texte (Desc-Bank @0x800C50DE in DEBUG.BIN, `texte.py`, Rohbytes geprüft @0x800C53CC/0x800C53FB):
  0x09 „Hand grenade designed to detonate after a set amount of time." ·
  **0x0A „Hand grenade containing a powerful toxic gas."** ·
  **0x0B „Hand grenade containing a flammable substance."** ·
  0x1A „Granade acid rounds. For the M79 Granade Launcher." · 0x1B „Granade flame rounds. …".
  Die „Acid Grenade" war im RE1.5-Entwurf also eine **Giftgas**-Granate; die GL-Runden heißen acid/flame
  wie in RE2.
* In der Hand bleibt die Granate bei allen drei Sorten oliv (1.7) — auch das ist ein Unfertig-Befund.

### 1.9 Zwischenfazit RE1.5 (Einordnung je Mechanismus)

| Mechanismus | 0x09 Hand | 0x0A Acid / 0x0B Incendiary | Beleg |
|---|---|---|---|
| Name, Bild, Icon, Text, Ziffernfarbe | fertig | **fertig** (eigene Kunst) | 1.3, 1.8 |
| Ausrüsten, Bank, Wurfanimation, Munition −1 | fertig | **fertig** (bytegleich W09, Stub-Handler zieht Munition ab) | 1.1, 1.7 |
| Projektil-Spawn 0x040D1000 | fertig | **fehlt** (Gate `== 9` @0x8003368c) | 1.2 |
| Flug/Abprall/Zünder (Routinen 30/29) | fertig | — (nie gespawnt) | 1.6a |
| Aufschlag-Effekt | HE-Explosion (Routine 31, Kinder 0x0319/0x030B, SE 0x04080001) | **fehlt** (keine Row-Menge, keine Routine, kein SE) | 1.6, 1.6a |
| Flächenschaden | Art 2 = 1000 flach, +0x05 = 9 | **nur Daten**: Art 3/4 = 1000, +0x05 = 10/11 (@0x8006F41E/20, @0x8006F433/34) — nie erzeugt | 1.5 |
| Elementar-Gewichtung je Gegner | tote Spalte 9 | **nur Daten**: Spalten 10/11 @0x8006E0D0 (Hitscan-Tabelle, für Granaten nie gelesen) | 1.4 |
| Gegner-Reaktion auf +0x05 = 10/11 | — | im Overlay je Typ zu prüfen (Nachbar-Dossier Gegner/RE2) | — |

**Einordnung: 0x0A/0x0B sind in RE1.5 NACHWEISLICH UNFERTIG** (Stub-Handler, hartes Id-9-Gate, kein Aufschlag,
Arten 3/4 ohne Erzeuger). Nach der Beta→Retail-Regel ist für Flug-Wirkung/Aufschlag/Effekt/Ton RE2 Retail das
Ziel. Vorhandene RE1.5-Teile (Kunst, Texte, Ziffernfarbe, Bank, Wurfanimation, Munitionsabzug, und als
Datenvorgabe die Arten 3/4 mit Schaden 1000 und Reaktionswaffe 10/11) bleiben RE1.5.

---

## 2. RE2 Retail — GL-Runden (9 Explosiv / 10 Brand / 11 Säure)

Alle Adressen `info/re2leon/PSX.EXE` (t_addr 0x80010000), selbst disassembliert mit `re2_disasm.py`
(⚠ dessen Annotationen sind RE1.5-Namen und in RE2 bedeutungslos). FX-System-Grundlagen (Pool 0x800D8CF0,
Stride 0x7C, 0x60 Slots, Spawner FUN_8001bf10, a0 = Bank<<24 | Sub<<16 | Skala, Sub&7 = Skript,
Sub>>3 = CLUT-Zeile, Opcode-Tabelle @0x8009D868) aus `analysis/konstruktion_2026-08-23/re2-fx-system.md`;
die hier gebrauchten Teile habe ich gegengeprüft. Treffer-Zustellung (Applier FUN_800470C0, Boxen,
Hitcodes, Schadensrecords, Sperre) steht ausführlich im Nachbar-Dossier `re_gegner_re2_familie.md` §1 —
hier nur, was die Munitionsart unterscheidet.

### 2.1 Opcode-Tabelle (Auszug, `table 0x8009d868 96`, Ausgabe `re2_fx_optab.txt`)

| Op | @Tabelle | Funktion | Rolle (belegt unten) |
|---|---|---|---|
| 15 | 0x8009D8A4 | 0x8001ED9C | Flug der Runde (als Op B) |
| 17 | 0x8009D8AC | 0x8001F198 | **Start der Runde** (als Op B im Skript) |
| 19 | 0x8009D8B4 | 0x8001F2C0 | Bodenflamme (ruft Op 40) |
| 22 | 0x8009D8C0 | 0x8001F634 | Op A der fliegenden Runde |
| 40 | 0x8009D908 | 0x80020758 | Nachbrenner-Treffer |
| 47 / 48 / 49 | 0x8009D924/28/2C | 0x80020C3C / 0x80020F3C / 0x800215C8 | Aufschlag Explosiv / Brand / Säure |
| 50 | 0x8009D930 | 0x80021970 | (von Op 40 gerufen, `jal` @0x800207cc) |

### 2.2 Wo die Runde entsteht — Waffen-Effekt-Handler + CORE00.ESP Bank 2 Skript 4 (NEU)

**Per-Waffe-Effekt-Tabelle** @0x800A6FDC (Index = Waffen-Id; gelesen `lhu v0,0x10e(s1)` / `andi 0xfff` /
`sll 2` / `lw v0,0x6fdc(at)` @0x800431ac-c4, `jalr` @0x800431cc; dieselbe Lesung @0x80043d00, 0x80047f28,
0x80048b1c, 0x80048d7c): [9] @0x800A7000 → **0x80044B44**, [10] @0x800A7004 → **0x80044F44**,
[11] @0x800A7008 → **0x80045090**. (Die Entlade-Tabelle @0x800A6F90 [9]/[10]/[11] = 0x80043FCC/0x80043FF4/
0x8004401C zieht wie in RE1.5 nur Munition ab: `jal 0x8006a0cc` @0x80043fdc/0x80044004/0x8004402c.)

Alle drei Handler spawnen im Schussbild `+0x14D == 1` (`lbu v1,333(s0)` / `addiu v0,zero,1` @0x80044b98-a0)
an der Waffenknochen-Matrix `*(+0x198) + 0x7AC` (@0x80044bb8/0x80044bdc) mit Versatz {120, 1200, 0}
(`addiu v0,zero,120` / `1200` @0x80044bc4-d4):

| Handler | Spawns im Bild 1 | Beleg |
|---|---|---|
| 9 Explosiv 0x80044B44 | 0x01002000 (Bank 1 Skr. 0, a1 = 0) · **5 × 0x020C0A00** (Bank 2 Skr. 4, CLUT+1, Skala 0x0A00), je danach Slot-Felder überschrieben (Tabelle unten) | @0x80044ba8-e4, @0x80044be8-0x80044e74 |
| 10 Brand 0x80044F44 | 0x01002000 · **1 × 0x020C1000** (Bank 2 Skr. 4, Skala 0x1000, a1 = Gier +0x76) · 0x03081200 (Bank 3 Skr. 0, CLUT+1) | @0x80044f68-0x80044fc8 |
| 11 Säure 0x80045090 | **bytegleich zu 10**: 0x01002000 · 0x020C1000 · 0x03081200 | @0x800450b4-0x80045114 |

Explosiv-Überschreibungen (Slot = Pool + Index·0x7C; Felder = Step-Cache): `sb acc.x,+0x08` (0x800D8CF8),
`sh vel.x,+0x0C` (Tabelle @0x80011030 = `00 00 88 ff 10 ff fa 00 c8 00 96 00` = {0, −120, −240, 250, 200, 150}
nach sp+24, Zeile `(0x800CFD4C >> 15)` = Bit 15 des Ziel-Worts Spieler +0x154, Spalte je Runde `lhu 0/2/4(v0)`),
`sh vel.y,+0x0E`, `sh vel.z,+0x10`:

| Runde | acc.x | vel.y | vel.z | vel.x (Bit 15 = 0 / 1) | Adressen |
|---|---|---|---|---|---|
| 1 | −10 | 600 | 0 | 0 / 250 | @0x80044c18-64 |
| 2 | −20 | 400 | 100 | −120 / 200 | @0x80044c98-e8 |
| 3 | −20 | 400 | −100 | −120 / 200 | @0x80044d1c-6c |
| 4 | −40 | 250 | 50 | −240 / 150 | @0x80044da0-f0 |
| 5 | −40 | 250 | −50 | −240 / 150 | @0x80044e24-74 |

**Das Skript der Runde** (`re2_esp_scripts.py`, Ausgabe `re2_core00_scripts.txt`): CORE00.ESP Bank 2 (@Datei
0x1784, Skripttabelle @0x1894), **Skript 4 @0x1924, 1 Part, 1 Step @0x192C**:
`00 11 2f 2f 00 10 00 10 f6 00 00 0f 00 00 80 02 00 00 00 00 00 00 00 00` =
Op A 0, **Op B 17**, step[2] = **47**, step[3] = **47**, Aspekte 4096/4096, acc = (−10, 0, 0),
step[0xB] = 15, vel = (0, 640, 0). Das ist der einzige Step mit Op 17 in CORE00.ESP (Op-B-Histogramm) und
in keinem der 212 Leon-Raum-ESP-Blöcke kommen Op 15/17/19/22/40/47/48/49 vor (`re2_rdt_esp_ops.py`,
Gegenprobe Op 1 → 3339 Treffer, Op 34 → 8). Die Runde ist also **rein datengetrieben aus Bank 2 Skript 4**.

**Start, Op 17 @0x8001F198** (als Op B, `dis 0x8001f198 66`):
```
8001f19c: lui a1,0x800d / 8001f1a0: addiu a1,a1,-762   ; 0x800CFD06 = Spieler +0x10E (Waffen-Id)
8001f1a8: lbu v0,0(a1)
8001f1b4: addiu v0,v0,-9 / 8001f1b8: sb v0,27(v1)      ; +0x1B := Id - 9  (0 Explosiv, 1 Brand, 2 Säure)
8001f1c4: addiu v0,zero,22 / 8001f1c8: sb v0,0(v1)     ; Op A := 22
8001f1d4: addiu a2,zero,15 / 8001f1d8: sb a2,1(v0)     ; Op B := 15 (Flug)
8001f1e4: ori v0,zero,0xb403 / 8001f1e8: sh v0,24(v1)  ; Status 0xB403
8001f1ec: addiu v0,zero,18 / 8001f1f0: sb v0,33(v1)    ; Anim-Index 18
8001f200: ori v0,v0,0x20 / 8001f204: sh v0,42(v1)      ; TPage |= 0x20
8001f224: lhu v0,0(a1) / 8001f228: addiu v1,zero,9 / 8001f22c: andi v0,v0,0x1f / 8001f230: bne v0,v1,...
8001f238: jal 0x80015fe8 ... 8001f240-60: v0 % 3 (0x55555556-Idiom) / 8001f26c: addiu v0,v0,10
8001f274: sb v0,11(v1)                                 ; Explosiv: Lebensdauer +0x0B := 10 + rng%3
8001f284: sb a2,11(v0)                                 ; Brand/Säure: Lebensdauer := 15
8001f288: jal 0x8001ed9c                               ; erster Flugschritt
```
Das ist die einzige Stelle der EXE, die die Waffen-Id in die Runde schreibt (Ghidra-XREF DAT_800CFD06:
Leser @0x8001f1a8/0x8001f224 in Op 17, sonst nur Schreiber/Inventar @0x80053098, 0x8005872c, 0x80068d8c ff.).

### 2.3 Flug (gemeinsam für alle drei Arten)

**Op A 22 @0x8001F634** (jedes Bild, Rauchspur): `FUN_8001cbe8(0x030B0000 | (Skala*150/100), 0, Identität
0x8009DB44, &Lage)` — Skala*150 über `lhu v0,58(a3)` / x5 / x15 / x2 @0x8001f648-60, /100 über
`0x51eb851f` >> 5 @0x8001f640-84, Code `lui v0,0x30b` @0x8001f688. Explosiv (Skala 0x0A00) -> 0x030B0F00,
Brand/Säure (0x1000) -> 0x030B1800.

**Op B 15 @0x8001ED9C** (`re2_op15_flug.dis`):

| Schritt | Instruktionen | Wert |
|---|---|---|
| Box des Flugkontakts | `lui a2,0x8001 / addiu a2,a2,2304` @0x8001edb8-bc, Bytes @0x80010900 `88 fa 00 00 5e 01 fa 00` | {-1400, 0, 350, 250} |
| Wasser | `jal 0x800527b4` (Wasserspiegel an x/z) @0x8001ede8; `slt` @0x8001ee14; darunter: y += 500 @0x8001ee34, `0x1A051C00` @0x8001ee1c-20 + `jal 0x8001cbe8` @0x8001ee38, dann Aufschlag | Spritzer Bank 0x1A |
| Lebensdauer | `lbu v0,11(v1) / addiu -1 / sb` @0x8001ee64-70 | -1 je Bild |
| Boden/Wand | `jal 0x8004fba0(&Lage, 2, 8192, 0)` @0x8001eea0 (setzt DAT_800DCBC8), Ergebnis - 10 -> `sw v0,20(v1)` @0x8001eebc | |
| Treffer Gegner | `ori s2,s2,0x9` (0x30009) @0x8001ee90-9c, `lb a3,27(v1)` + `addu a3,a3,s2` @0x8001eed0/dc, `jal 0x800470c0` bei y+1000 (@0x8001eec8/d8) und y-1000 (`addiu v0,v0,-2000` @0x8001eef8, jal @0x8001ef08) | Hitcode 0x30009 / 0x3000A / 0x3000B |
| Explodieren | Treffer (`bne s0,zero` @0x8001ef14) oder Lebensdauer 0 (`lbu v0,11 / bne` @0x8001ef28-30): Status \|= 0x80 @0x8001ef44-50, Lage -> DAT_800CFB88/8C/90, `lbu v0,2(v1)` = step[2] | Op[47 + Art] |
| Wand/Boden berührt | `lw v0,-13368(v0)` = DAT_800DCBC8 @0x8001ef84; != 0: Rückprall-Rechnung (0x55555556 = /3) @0x8001ef94-0x8001f0c8, dann `lbu v0,3(v1)` = step[3] @0x8001f0e0 | Op[47 + Art] (step[3] = 47) |
| Dispatch | `lb v1,27` / `addu` / `lw v0,-10136(at)` = 0x8009D868[step + Art] / `jalr` @0x8001f0e4-104 | |

Kein Abprallen wie bei der RE1.5-Handgranate: die GL-Runde explodiert bei der ersten Wand-/Bodenberührung,
bei Gegnerkontakt, im Wasser oder nach 10..12 (Explosiv) bzw. 15 (Brand/Säure) Bildern.

### 2.4 Aufschlag je Art

**Explosiv, Op 47 @0x80020C3C** (`re2_op47_explosiv.dis`), Phase = `lhu v1,18(v0)` (+0x12), Sprungtabelle
@0x80010928 = {0x80020CD0, 0x80020E7C, 0x80020E90, 0x80020EA4, 0x80020EB8}:
* Phase 0: Status 0x8400 (@0x80020d00-04), Op A := 0, Op B := 47 (@0x80020d14-18). Nur wenn Sub (+0x1E) == 12
  (`addiu v0,zero,12 / bne` @0x80020d34-38): **SE 0x01110001** (`lui a0,0x111 / ori 0x1` @0x80020d40-44,
  `jal 0x8005ba28` @0x80020d48), **Schaden** `jal 0x800470c0` mit Hitcode **0x10020009** (@0x80020d54-58) und
  Box @0x80010918 = {-2000, 0, 1000, 500} (`30 f8 00 00 e8 03 f4 01`) bei y (@0x80020d78) und y+900
  (`addiu v0,v0,900` @0x80020d98, jal @0x80020db0). Sonst nur SE 0x01140001 (@0x80020d3c/dc0-c4).
  Kinder (alle `jal 0x8001cbe8`, Identität 0x8009DB44, an der Lage): **0x031F1700** (@0x80020dfc-e1c),
  **0x040C1F00** (@0x80020e28-40), **0x041D1700** (@0x80020e48-60) (Skalen 5888/7936 = `addiu s2,s0,5888` /
  `addiu s0,s0,7936`, bei Sub 13 je +2560). Lebensdauer == 255 -> y -= 470 (@0x80020dd8-f4).
* Phase 1..3: nur Phase +1 (@0x80020e7c-a0) — 3 Bilder Pause.
* Phase 4 (@0x80020eb8): y -= 600 bei Lebensdauer 255, Kind **0x04152700** (@0x80020ee4-f00), dann Op A/B := 0,
  Status := 0 (@0x80020f14-1c) -> Platz frei. **Kein Nachbrennen.**

**Brand, Op 48 @0x80020F3C** (`re2_op48_brand.dis`), Phase 0 (`lhu v1,18(a3)` == 0 @0x80020fa0-a8):
* Matrix-Translation (+0x60/64/68) := Aufschlagort (@0x80021000/14/18), Status 0x8000, Op A := 0, Phase := 1,
  Op B := 48 (@0x80020ff4-101c). **SE 0x01120001** (`lui a0,0x112` @0x80020fd4, `ori 0x1` @0x80021028, jal @0x8002102c).
* Status-Bit 0x80 (vom Flug) = 0 (Wand/Boden/Wasser): Hitcode **0x0002000A** (@0x80021058-64, @0x8002106c-70) mit Box
  @0x8001093C = {-1200, 0, 600, 300} (`50 fb 00 00 58 02 2c 01`), zwei Aufrufe (@0x80021060, @0x8002108c; beide mit
  a0 = Slot+0x60 = Aufschlagort — das "+1800" @0x80021080 wirkt nur auf die gesicherte Kopie in sp+20);
  Kinder **0x040C2800** (@0x80021094-b8), **0x041D2700** (@0x800210c0-d8); dann **drei Bodenflammen 0x0505xxxx**:
  Skala `7168 + (rng%8)*768` (@0x800210e0-114), Gier = Gier + rng%40 (@0x80021120-164) / + rng%80 + 400
  (@0x80021270-2b8) / + rng%80 - 400 (@0x800213c8-410); je Flamme `vel.x += rng%25` (@0x80021184-1e0 usw.),
  `acc.y += rng%8` (@0x800211e4-22c usw.), **+0x4A := 1** (`sh s1,-29382(at)` @0x8002121c, `sh v0` @0x8002135c/b4).
* Bit 0x80 = 1 (Gegner getroffen oder Lebensdauer um): Box @0x80010944 = {-600, 0, 300, 150}, zwei Aufrufe
  0x0002000A (@0x800214f8/0x80021524), Kinder 0x040C2800 + 0x041D2700 (@0x8002152c-74), **keine Bodenflammen**.
* Phase 1 (@0x800215a4): Op A/B := 0, Status := 0 -> Platz frei.

**Säure, Op 49 @0x800215C8** (`re2_op49_saeure.dis`), Sprungtabelle @0x80010950 = {0x80021678, 0x800217D4,
0x80021854, 0x80021894, 0x800218F4}:
* Phase 0: Phase := 1, Status 0x8403, Op A := 0, Op B := 49 (@0x80021684-b0), **SE 0x01130001** (@0x80021678-7c,
  jal @0x800216ac). Box je Bit 0x80: {-1200,0,600,300} oder {-600,0,300,150} (@0x800216c8-dc); Hitcode
  **0x1002000B** (@0x800216e4-f0) bei y (@0x800216ec) und y+1800 (`addiu v0,v0,1800` @0x8002170c, jal @0x80021718).
  Danach fliegt der Spritzer weiter: vel.y := 240, vel.x := 0, acc.x := -23 (Lebensdauer 255: acc.x := 0, y -= 470)
  (@0x8002172c-7c), acc.y := 0. Kinder **0x030F2000** (@0x80021780-a0), **0x040C2000** (@0x800217a8-c0),
  **0x041D1800** (@0x800217c8-d0 -> jal @0x800218e4).
* Phase 1: **0x031F2000** (@0x800217f0-81c); Phase 2: **0x03142000** (@0x80021888-90); Phase 3: **0x040D2800**
  (@0x800218c8-cc); Phase 4: **0x030F2000** (@0x80021920-3c), dann Platz frei (@0x80021950-58).
  (Jeweils y -= 600 wenn Lebensdauer 255.) **Kein Nachwirken, kein Bodeneffekt.**

### 2.5 Brand über Zeit — die Bodenflammen (nur Brand-Runde, nur bei Wand/Boden-Aufschlag)

Die drei Flammen sind CORE00.ESP **Bank 5 Skript 5** (@0x07F8, Step @0x0800 `00 1b 2e 32 ...`: Op B 27, step[2] = 46,
step[3] = 50, acc (0,5,0), vel (96,0,0)):

| Op | Funktion | Wirkung (Adresse) |
|---|---|---|
| 27 (Start, Op B) | 0x8001FA9C | Status 0xB003, Anim rng%3, Boden -> +0x14, Op A := **58**, Op B := **28**, +0x1B := 2, Zähler +0x42 := **8 + rng%3** (@0x8001fabc-bb8) |
| 58 (Op A, in der Luft) | 0x80022254 | +0x1B 2: Zähler--, Aspekt x1.010 / x1.007 (1010/1000 @0x80022350-64, 1007/1000 @0x80022370-84); danach +0x1B := 1, Zähler := 2 + rng%2 (@0x800223b8-e4); +0x1B 1: x0.88/x0.80 (@0x800222c0-328); +0x1B 0: tot (@0x800222a0-a8) |
| 28 (Op B, in der Luft) | 0x8001FBD0 | Wasser -> tot (@0x8001fbec-24); Boden unter y-900 erreicht -> Op[step[2]] = **46** (@0x8001fc7c-cc); Wand (DAT_800DCBC8) -> Op 46 oder Op[step[3]] = 50 (@0x8001fcd8-34) |
| 46 (gelandet) | 0x80020B60 | Op A := **19**, Op B := 29, vel.x := 180, acc.x := -10 - rng%11, Zähler +0x42 := **38 + rng%8** (@0x80020b70-c24) |
| 19 (Op A, brennt am Boden) | 0x8001F2C0 | wenn +0x4A != 0: ab step[0x16] >= **16** (`sltiu v0,v0,0x10` @0x8001f2e8) und X-Aspekt > **0x1000** (`sltiu v0,v0,0x1001` @0x8001f2fc) -> `jal 0x80020758` (@0x8001f308); step[0x16]++ (@0x8001f31c-28). Zustand +0x1B 2: Zähler--, x1.009/x1.002 (@0x8001f400-478); dann +0x1B := 1, Zähler := **90 + rng%11** (@0x8001f47c-4c8); +0x1B 1: x0.99/x0.98 (@0x8001f398-3e4) bis 0; +0x1B 3: Zähler -> dann 2 mit 30 + rng%8 (@0x8001f4cc-51c); +0x1B 0: tot (@0x8001f37c-84) |
| 40 (Nachbrenner) | 0x80020758 | Box @0x80010910 = {-600, 0, 300, 150}, Prüfpunkt y - 100 (@0x800207a4), Hitcode **0x2002000A** (@0x80020794-a0); Treffer -> `jal 0x80021970` (Op 50: vel.x/acc.x := 0, Physik-Bit aus, @0x80021978-ac) — die Flamme **bleibt stehen und brennt weiter** (Op A 19 wird nicht geändert) |

Dauer einer gelandeten Flamme = (38..45) Wachsbilder + (90..100) Schrumpfbilder; Schaden je Bild möglich ab Bild 16
nach der Landung, solange der X-Aspekt > 0x1000 ist; die Gegner-Sperre +0x1D3 (= 15 Bilder, Nachbar-Dossier
`re_gegner_re2_familie.md` §1.2) begrenzt auf einen Treffer je 15 Bilder und Gegner. Einen "brennenden Gegner"
(Zustand am Gegner) setzt der GL-Pfad selbst NICHT — was der Gegner mit Zeile 10 macht, liegt in seiner
Treffer-Tabelle (Nachbar-Dossier §2). Hinweis an das Nachbar-Dossier: "Nachbrenner endet beim ersten Treffer"
stimmt nicht ganz — Op 50 stoppt nur das Gleiten der Flamme; Op A 19 läuft weiter und kann nach Ablauf der
Gegner-Sperre erneut treffen.

Ergänzung Bodenflammen (Op B der gelandeten Flamme):

| Op | Funktion | Wirkung (Adresse) |
|---|---|---|
| 29 (Op B, gelandet) | 0x8001FD5C | vel.x <= 0 -> vel.x/acc.x := 0 (@0x8001fd6c-84); solange vel.x >= **61** (`slti v0,v0,61` @0x8001fd78) und Zähler step[2] % **15** == 0 (0x88888889-Idiom @0x8001fd94-b8): **Folgeflamme** `0x05040000 \| (Skala*0.8)` (`lui v0,0x504` @0x8001fdf0, x4 x0x66666667 >> 1 @0x8001fdc8-ec, jal @0x8001fdf4) mit +0x4A := 1 (@0x8001fe20); step[2]++ (@0x8001fe30-3c); Wand bei y-100 -> Op[step[3]] (@0x8001fe70-b4) |
| 30 (Start Folgeflamme, Bank 5 Skr. 4, Op A) | 0x8001FECC | `jal 0x8001dd2c` (Op 2); Sub == 4 -> +0x1B := 2, Zähler := **2 + rng%8** (@0x8001fee8-f30); sonst +0x1B := 3, Zähler := 700 + (rng%6)*50 (@0x8001ff34-84, nicht GL) |

Die gleitende Flamme (vel.x 180, acc.x -10..-20) legt also mindestens beim Landebild (step[2] = 0 nach Op 46) eine
kleinere, ebenfalls schadende Folgeflamme ab (Bank 5 Skript 4 -> Op 30 -> Op A 19). Folgeflammen leben 2..9 Wachs-
+ 90..100 Schrumpfbilder.

### 2.6 Töne (RE2)

SE-Spieler **FUN_8005BA28** (`RE2_Quellcode_V2/FUN_8005ba28.c`, Kern: Bank = Code >> 24, Record = (Code >> 16) & 0xFF,
Deskriptor = `*(DAT_800DBB78[Bank] + Record*4)`, 0xFFFFFFFF -> stumm; Programm = Byte1 & 0x7F, Ton = Byte2 >> 4,
Tonattribut @ `DAT_800D75A0[Bank] + prog*0x200 + ton*0x20 + 0x820`).
**Bank 1 = ARMS-Datei der ausgerüsteten Waffe:** FUN_80059654 setzt `DAT_800DBB7C := 0x801FAA00` (@0x8005976c-80);
FUN_80059c74(Waffe) lädt dorthin die Datei `DAT_800A8118[Waffe]` (`lhu a0,-0x7ee8(at)` @0x80059cf0, Name
"ARMS EDT" @0x80011578, `jal 0x80012fb8` @0x80059cfc) und setzt den VAB-Kopf `DAT_800D75A4` (@0x80059d1c).
`read 0x800a8118 20 --w 2` = {289, 289, 291, …, 305 (Id 9), 307 (Id 10), 309 (Id 11), …} — Paarraster EDH/VB,
Id 9/10/11 -> ARMS09/0A/0B (Dateiname über das Raster + den Record-Befund unten zugeordnet; CD-TOC nicht gelesen).

`re2_arms_se.py` (Ausgabe `re2_arms_se.txt`) auf `info/re2leon/COMMON/SOUND/ARMS09/0A/0B.EDH` (Deskriptor-Tabelle
= erste 0x80 Byte, Länge = erstes Trailer-Byte 0x80; VAG-Größentabelle hinter den Tonattributen):

| SE | ARMS09 (Explosiv) | ARMS0A (Brand) | ARMS0B (Säure) | Sample |
|---|---|---|---|---|
| 0x01000001 Abschuss (Record 0) | `00 00 14 36` Ton 1 -> VAG 2 (4672 B) | gleich | gleich | `ARMSxx_00000.wav`, **in allen drei bytegleich** (md5 eecf4d0b…) |
| **0x01110001** Aufschlag Explosiv (Record 17) | `00 00 33 20` Ton 3 -> **VAG 3 (12800 B)** | 0xFFFFFFFF | 0xFFFFFFFF | `ARMS09_00001.wav` (md5 ad996c2e…) |
| **0x01120001** Aufschlag Brand (Record 18) | 0xFFFFFFFF | `00 00 33 20` -> **VAG 3 (11664 B)** | 0xFFFFFFFF | `ARMS0A_00001.wav` (md5 2b1376ac…) |
| **0x01130001** Aufschlag Säure (Record 19) | 0xFFFFFFFF | 0xFFFFFFFF | `00 00 33 20` -> **VAG 3 (10992 B)** | `ARMS0B_00001.wav` (md5 26d05bb3…) |
| 0x01140001 (Sub-13-Variante, Op 47) | 0xFFFFFFFF | 0xFFFFFFFF | 0xFFFFFFFF | stumm |
| 0x01010001 Leer-Klick (Record 1) | Ton 5 -> VAG 4 | gleich | gleich | `_00002.wav` |
| 0x01030001 (Record 3) | Ton 7 -> VAG 6 | gleich | gleich | `_00004.wav` |
| 0x010E0001 (Record 14) | Ton 6 -> VAG 5 | gleich | gleich | `_00003.wav` |

(Zuordnung VAG -> wav über die Größen: die extrahierten Dateien lassen die 48-Byte-Stumm-VAG 1 aus, wav-Samples·16/28
+ 16 = VAG-Bytes.) **Der Aufschlagton ist je Art ein eigenes Sample; der Abschusston ist für alle drei gleich.**
Der Knall-SE des Abschusses kommt wie in RE1.5 über das Animations-Ereignisbit 0x100000 (`lui v0,0x10 / and`
@0x80044efc-00, Code `0x01000001 | (Ereignis & 0xF0000)` @0x80044f08-18, `jal 0x8005ba28` @0x80044f1c).

### 2.7 Wo die RE2-Daten liegen — und was der Port davon schon kann

| Was | Datei (Repo) | Beleg |
|---|---|---|
| Skripte/Anims/UV der Runde, Explosion, Flammen | `info/re2leon/COMMON/DATA/CORE00.ESP` (8572 B): Bank 2 @0x1784 (Runde, Skr. 4), Bank 3 @0x0008, Bank 4 @0x1BCC, Bank 5 @0x05F0 | Ids `03 05 00 01 02 06 07 04`, Registrierung FUN_8001bca0 @0x8001bb8c (CORE-Boot) |
| Pixel + CLUTs der FX | `info/re2leon/COMMON/DATA/TEX.TIM` (CLUT (256,480) 32x19 = Zeilen 480..498, Bild 256hw x 256) | CLUT-Wörter der Bänke: 3/4 0x7811 (272,480), 5 0x7911 (272,484), 1 0x7B11 (272,492), 2 0x7B91 (272,494); tpage 0x1E/0x1F |
| Töne | `info/re2leon/COMMON/SOUND/ARMS09/0A/0B.EDH` + `.VB` (+ vorextrahierte `.wav`) | §2.6 |
| Code (Ops 15/17/19/22/27–30/40/46–50/58) | `info/re2leon/PSX.EXE` | §2.1–2.5 |

Port heute (nur gelesen):
* **Kein RE2-FX-System.** `re15_esp.c` ist die RE1.5-Row-Maschine (Routinen-Tabelle @0x80071D40); kein
  `re2_fx.c`, keine RE2-Step-Maschine, keine Ops 15/17/19/22/27–30/40/46–50/58. Der Bauplan steht in
  `analysis/konstruktion_2026-08-23/re2-fx-system.md` §3 (nicht umgesetzt; `grep re2fx` leer).
* `engine/src/re2_ems.c` = RE2-Gegnermodelle (EMS-Splitter, `re2_ems_*`, `re2_hybrid_*`) + ENEMSE-Satz-TOC
  (`re2_enemse_*`, :421-509); `engine/src/re15_to_re2.c` = SCD-Übersetzungsschicht. **Keins von beiden hat ESP/FX oder
  ARMS-Töne.**
* **RE2-Tonzugriff gibt es als Mini-Bank-Muster:** `shared_assets/RE2/{ENEMSE,ELEVSE,HINTSE,TUERSE,TORSE}.VBS`, geladen
  über `re15_pc_read_re2()` (`audio_pc.c:1074ff`, `:1131-1171`). Die ARMS09/0A/0B-Samples sind dort NICHT vorhanden.
* **RE2-Bilddaten:** `shared_assets/RE2/` hat kein CORE00.ESP und kein RE2-TEX.TIM; die RE1.5-Effektbögen kommen aus
  `shared_assets/extracted_fx/` bzw. RE1.5 `DATA/TEX.TIM` (README dort).
* **RE2-Reaktionszeilen** für 9/10/11 existieren schon (RE2-KI, `enemy_ai_re2_zombie.c:3787-3829`,
  `re2z_row_from_weapon` 9 -> 9, **10 -> 11**, **11 -> 10**; Zeilen 10/11 der Treffertabelle byte-identisch).

---

## 3. Folgerung — was belegt ist, was Port-Zuordnung ist

### 3.1 Prüfung der Arbeitshypothese

Hypothese aus dem Auftrag: „Wurf = RE1.5-Routinen 30/29 für alle drei; Explosion bei 0x0A/0x0B = RE2-Säure-/Brand-Aufschlag."

| Teil | Befund | Einordnung |
|---|---|---|
| Wurf (Spawn 0x040D1000, Routine 30 Startgeschwindigkeit je Zielhöhe, Routine 29 Abprall, Zünder 42 -> 7) | In RE1.5 existiert er NUR für Id 9 (Gate @0x8003368c; 46 Leser von 0x800ACA5D, keiner vergleicht mit 10/11, §1.2). RE2 hat **keinen** Handgranaten-Wurf: die GL-Runde ist ein Werfer-Geschoss (Bank 2 Skr. 4, fliegt gerade mit acc (-10,0,0), explodiert beim ersten Kontakt, §2.3). | **Belegt:** Routinen 30/29/31 sind der einzige Wurf-Mechanismus beider Spiele. **Port-Zuordnung:** denselben Wurf für 10/11 zu benutzen. Stütze: PL00W0A/0B und PL04W0A/0B sind bytegleich mit W09 (§1.7, gleicher Wurf-Clip 7), Entlade-Handler 0x80033B58/78 sind bytegleich mit 0x80033B38 (§1.1), Waffen-Records gleicher Klasse (+0 = 250, +9 = 0, §1.3), gleicher Tester 0x800128A0 und Reichweite 1000 (§1.4). |
| Explosions-ZEITPUNKT | RE1.5: Zünder (Routine 31, +0x1E = 42, Treffer bei 7). RE2: Kontakt/Lebensdauer. | **Port-Zuordnung:** der RE1.5-Zünder, weil nur er zu einer geworfenen Granate gehört. |
| Explosions-INHALT (Effekt, Ton, Bodenfeuer) | RE1.5 hat für 10/11 nichts: keine Row-Menge, keine Routine liest eine Variante, Routine 31 hat nur Konstanten (§1.6/1.6a). RE2 hat drei vollständige Aufschläge (Op 47/48/49, §2.4) und das Bodenfeuer (§2.5). | **Beta->Retail belegt** (RE1.5 unfertig) -> RE2. **Port-Zuordnung:** 0x0A Acid -> RE2-Art 2 (Op 49), 0x0B Incendiary -> RE2-Art 1 (Op 48). Stütze: Namen (FUN_80028840), Ziffernfarbe (+8: 0x0B teilt die 2 mit dem Flammenwerfer, §1.3), GL-Texte 0x1A "acid rounds"/0x1B "flame rounds" (§1.8), bestehende Port-Zuordnung `re2z_row_from_weapon` 10 -> 11, 11 -> 10. Gegenstimme: der RE1.5-Text zu 0x0A sagt "toxic gas" — RE2 hat keine Gas-Runde; die Säure-Runde ist die nächste. |
| SCHADEN der Explosion | RE1.5 hat mehr als vermutet: der Resolver FUN_80012d60 behandelt Art 3/4 vollständig (1000 flach, +0x05 = 10/11, @0x8006F41E/20, @0x8006F433/34), nur ohne Aufrufer (§1.5). RE2 hat eigene Hitcodes (0x1002000B Säure, 0x0002000A Brand, Boxen statt Radius). | Siehe 3.2. |
| Brand über Zeit | Nur RE2 (Op 48 -> Bank 5 Skr. 5 -> Op 46/19/40, Hitcode 0x2002000A). RE1.5: nichts (Art 5 = Flammenwerfer-Fläche ist ebenfalls ohne Aufrufer). | **Beta->Retail belegt** -> RE2. |

### 3.2 Schaden — Vorschlag mit Kennzeichnung

Die Regel „wo RE1.5 ein System vollständig hat, bleibt RE1.5 maßgeblich" trifft auf den Resolver zu: FUN_80012d60 samt
Art-3/4-Zeilen ist fertig, es fehlt nur der Aufruf. Der Aufruf für die Handgranate steht in Routine 31 @0x800185b8
(Radius 500, Punkt = Lage mit y-500, Art 2; Nachbar-Dossier `re_schaden_resolver.md` §1). Für 0x0A/0x0B ist daher der
belegte, kleinste Schritt: **derselbe Aufruf mit Art 3 (Acid) bzw. Art 4 (Incendiary)** — [BELEGT] Werte und
Reaktions-Id aus den RE1.5-Tabellen, [PORT-ZUORDNUNG] nur das a2 des Aufrufs (2 -> 3/4). Damit sind alle drei Granaten im
Schaden gleich (1000 flach, Radius 500) und unterscheiden sich in der Reaktions-Id (+0x05 = 9/10/11), im Effekt, im Ton und
(nur Brand) im Bodenfeuer. Die Reaktion auf 10/11 ist Sache der Gegner-Dossiers (für Reaktion 9 laut `re_schaden_resolver.md` §8.1/§9 in allen
Stages NULL -> RE2 Zeile 9; für 10/11 dort bzw. in `re_gegner_*` zu prüfen, Ziel dann RE2-Zeilen 11/10).

Der Port bildet Art 3/4 heute im RE2-Modus auf RE2-Zeile **17** ab (`re2z_row_from_atktype` = {1,1,17,17,17,9,9,10,11,17,1},
`enemy_ai_re2_zombie.c:3847`, gelesen `:3926`) — das ist für den Resolver-Weg eine ungeprüfte Port-Zuordnung, die bei Art 3/4 die
RE2-Zeilen 11/10 verfehlt (siehe PORT-ABGLEICH).

Das Bodenfeuer hat keine RE1.5-Quelle. Für die RE2-KI-Typen (Default-Flavor) ist Hitcode 0x2002000A (Zeile 10, Klammer 2)
byte-true verwendbar. Für Gegner, die im Port nicht in der RE2-KI laufen, gibt es keine Original-Zahl -> OFFEN.

### 3.3 Was die Explosion bei 0x0A/0x0B konkret aus RE2 übernimmt (Aufschlag im Zünder-Bild 7)

| Element | Säure (0x0A) = RE2 Op 49 | Brand (0x0B) = RE2 Op 48 |
|---|---|---|
| Ton | SE 0x01130001 = ARMS0B Record 19 -> VAG 3 (`ARMS0B_00001.wav`) | SE 0x01120001 = ARMS0A Record 18 -> VAG 3 (`ARMS0A_00001.wav`) |
| Sofort-Kinder | 0x030F2000, 0x040C2000, 0x041D1800 | 0x040C2800, 0x041D2700 |
| Folgebilder | Ph.1 0x031F2000, Ph.2 0x03142000, Ph.3 0x040D2800, Ph.4 0x030F2000 (4 Bilder) | Ph.1: Platz frei |
| Bodeneffekt | keiner | 3 Bodenflammen 0x0505xxxx (Skala 7168 + (rng%8)*768, Gier ±400), Folgeflammen 0x0504xxxx, Nachbrenner 0x2002000A ab Bild 16, Lebensdauer (38..45)+(90..100) Bilder |
| Lage | die Granaten-Lage (RE1.5 Slot +0x28/+0x2A/+0x2C); RE2-Kinder werden an der Aufschlaglage mit Identitätsmatrix gespawnt | wie links; Flammen an der Matrix-Kopie mit Translation = Aufschlaglage |

Die RE2-Sprites brauchen die RE2-FX-Maschine (Step-Maschine, nicht die RE1.5-Row-Maschine) und RE2-Daten
(CORE00.ESP Bänke 3/4/5, TEX.TIM-Seiten 0x1E/0x1F + CLUT-Zeilen 480..498). Ein Abbilden der RE2-Kinder auf RE1.5-
Effekt-Ids wäre eine erfundene Zuordnung und ist hier NICHT vorgeschlagen.

---

## 4. Übersicht

| Granate | Wurf (Quelle) | Aufschlag-Effekt (Quelle) | Ton | Schaden/Reaktion (Quelle) | Port heute | Lücke |
|---|---|---|---|---|---|---|
| 0x09 Hand (HE) | RE1.5 FSM 0x040D1000 @0x800336bc-a4, Routinen 30/29 @0x8001843C/0x80018320 | RE1.5 Routine 31 @0x8001854C: 0x03195000, 0x030B5400/5800 | RE1.5 SE 0x04080001 (Explosion), 0x010A0001 (Abprall) | RE1.5 FUN_80012d60(500, P, **2**) = 1000, +0x05 = 9 (@0x800185b8); Reaktion Zeile 9 -> RE2 (Nachbar-Dossiers) | Wurf-Anim + Munition ok; Projektil-Spawn Frame 19/22/24 ok (`game_step_common.c:1942-1961`); Routinen 29/30/31 fehlen in `re15_esp.c`; Schaden = Port-Brücke `ENT[9].resolve = 1` beim Abzug (`:1775`) | Flug, Abprall, Explosion, Fläche, Töne (Dossiers Wurf/Schaden) |
| 0x0A Acid | **Port-Zuordnung:** derselbe RE1.5-Wurf (Bank bytegleich W09) | **RE2 Op 49** @0x800215C8 (Beta->Retail) | **RE2** SE 0x01130001 = ARMS0B Rec. 19 (`ARMS0B_00001.wav`) | **RE1.5 Art 3** (1000, +0x05 = 10, @0x8006F41E/@0x8006F433) mit dem Routine-31-Aufruf [a2 = Port-Zuordnung]; RE2-Alternative Hitcode 0x1002000B | nur Munition -1 (`ENT[10] = {1,0,1,0}`, `game_step_common.c:1779`), Wurf-Anim läuft, **kein Projektil** (Gate `== 9`, `:1951`) | Projektil-Spawn für 10, RE2-FX-Maschine + Op 49, Ton, Resolver-Aufruf Art 3 |
| 0x0B Incendiary | **Port-Zuordnung:** derselbe RE1.5-Wurf | **RE2 Op 48** @0x80020F3C inkl. Bodenfeuer (Op 27/28/46/19/29/30/40/58) | **RE2** SE 0x01120001 = ARMS0A Rec. 18 (`ARMS0A_00001.wav`) | **RE1.5 Art 4** (1000, +0x05 = 11, @0x8006F420/@0x8006F434) [a2 = Port-Zuordnung]; Bodenfeuer RE2 0x2002000A (nur RE2-Typen) | wie 0x0A (`ENT[11]`, `game_step_common.c:1780`) | wie 0x0A + Bodenflammen, Nachbrenner-Treffer |

---

## KONSTANTEN FUER DEN BAU

RE1.5 = `info/Re1.5/PSX.EXE`, RE2 = `info/re2leon/PSX.EXE`, Dateien relativ zum Repo.

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| RE15_GRANATE_ART_SAEURE | 3 | RE1.5 Tabellenindex, Werte @0x8006F41E / @0x8006F433 | `8006f418: 0a 00 14 00 e8 03 [e8 03] e8 03 …` / `8006f430: 03 03 09 [0a] 0b …` | a2 für FUN_80012d60 bei 0x0A (Port-Zuordnung des Aufrufs, Werte belegt) |
| RE15_GRANATE_ART_BRAND | 4 | @0x8006F420 / @0x8006F434 | `… e8 03 [e8 03] 32 00 …` / `… 0a [0b] 0e …` | a2 für FUN_80012d60 bei 0x0B |
| Schaden Art 3 / 4 | 1000 / 1000 | @0x8006F41E / @0x8006F420 | `e8 03` / `e8 03`; Leser `lhu a0,0(s3)` @0x80012ff4, `subu`/`sh v1,154(s1)` @0x80012ffc-13000; Spieler `lhu v1,0(at)` @0x80012e54 | HP -= Wert |
| Reaktions-Id Art 3 / 4 | 10 / 11 | @0x8006F433 / @0x8006F434 | `lbu v0,0(at)` @0x80012fe8, `sb v0,5(s1)` @0x80012ff0 | Gegner +0x05 |
| Explosions-Aufruf (Vorlage Handgranate) | Radius 500, P.y = Lage.y − 500, Art 2 | RE1.5 @0x80018598-0x800185bc | `ori a0,zero,0x1f4` / `addiu v0,v0,-500` / `ori a2,zero,0x2` / `jal 0x80012d60` | für 0x0A/0x0B mit Art 3/4 (nur a2 ändert sich) |
| Waffen-Record +8 (Ziffern-CLUT) | 9→3, 10→1, 11→2 | @0x80074E1C / @0x80074E28 / @0x80074E34 | Leser `lbu a2,0(at)` @0x80049b0c/b6c, @0x8004c8a4, @0x8004d9fc → `jal 0x80048f28`; CLUT `lhu v0,0(at)` 0x800B2614[+8] @0x8004923c-44 = GetClut(256, 490+n) @0x80046174-c0 | Inventar-Ziffernfarbe (Port hat es: `re15_inv_screen.c:1066-1069`) |
| RE2 Runden-Skript | Bank 2, Skript 4 | CORE00.ESP (RE2) Datei 0x1924, Step @0x192C | `00 11 2f 2f 00 10 00 10 f6 00 00 0f 00 00 80 02 00…` | Op B 17, step[2]=step[3]=47, acc (−10,0,0), vel (0,640,0) |
| RE2 Runden-Spawn Brand/Säure | 0x020C1000, Versatz {120,1200,0} | RE2 @0x80044f9c-ac / @0x800450e8-f8; Versatz @0x80044f7c-8c | `lui a0,0x20c / ori a0,a0,0x1000`, `addiu v0,zero,120` / `1200` | Werfer-Geschoss (für den Port-Wurf NICHT gebraucht; Referenz) |
| RE2 Art-Byte | Id − 9 (0/1/2) | RE2 @0x8001f1a8-b8 | `lbu v0,0(a1)` (0x800CFD06) / `addiu v0,v0,-9` / `sb v0,27(v1)` | Aufschlag-Op = 47 + Art |
| RE2 Lebensdauer Runde | Explosiv 10 + rng%3, Brand/Säure 15 | RE2 @0x8001f26c-74 / @0x8001f1d4 + @0x8001f284 | `addiu v0,v0,10 / sb v0,11(v1)` ; `addiu a2,zero,15` / `sb a2,11(v0)` | Referenz (Port-Wurf nutzt RE1.5-Zünder) |
| RE2 Aufschlag-Op Säure | Op 49 = 0x800215C8 | Tabelle @0x8009D92C | `table 0x8009d868 96` [49] | Effektablauf 0x0A |
| RE2 Aufschlag-Op Brand | Op 48 = 0x80020F3C | @0x8009D928 | [48] | Effektablauf 0x0B |
| SE Aufschlag Säure | 0x01130001 | RE2 @0x80021678-7c, jal @0x800216ac | `lui a0,0x113 / ori a0,a0,0x1`; ARMS0B.EDH Record 19 = `00 00 33 20` (Ton 3 → VAG 3, 10992 B) | `ARMS0B_00001.wav` |
| SE Aufschlag Brand | 0x01120001 | RE2 @0x80020fd4 + @0x80021028, jal @0x8002102c | `lui a0,0x112` / `ori a0,a0,0x1`; ARMS0A.EDH Record 18 = `00 00 33 20` (VAG 3, 11664 B) | `ARMS0A_00001.wav` |
| Kinder Säure Bild 0 | 0x030F2000, 0x040C2000, 0x041D1800 | RE2 @0x80021780-84, @0x800217a8-ac, @0x800217c8/d0 | `lui a0,0x30f / ori 0x2000` · `lui a0,0x40c / ori 0x2000` · `lui a0,0x41d / ori 0x1800` + `jal 0x8001cbe8` | Säure-Spritzer |
| Kinder Säure Bild 1..4 | 0x031F2000, 0x03142000, 0x040D2800, 0x030F2000 | @0x800217f0/0x80021804, @0x80021888-90, @0x800218c8-cc, @0x80021920-24 | `lui a0,0x31f`/`0x314`/`0x40d`/`0x30f` + `ori` | ein Kind je Bild, dann frei (@0x80021950-58) |
| Säure-Bewegung nach Aufschlag | vel.y 240, vel.x 0, acc.x −23, acc.y 0 | @0x80021738-7c | `addiu v0,zero,-23 / sb v0,8(a0)`, `addiu v0,zero,240 / sh v0,14(a0)` | Spritzer fliegt weiter |
| Kinder Brand Bild 0 | 0x040C2800, 0x041D2700 | RE2 @0x80021094-98 / @0x800210c0-c4 | `lui a0,0x40c / ori 0x2800`, `lui a0,0x41d / ori 0x2700` | Feuerball |
| Bodenflammen | 3 × 0x0505xxxx, Skala 7168 + (rng%8)·768, Gier + rng%40 / + rng%80 + 400 / + rng%80 − 400 | RE2 @0x800210e0-0x80021410 | `addiu s0,s0,7168` / `lui v0,0x505`; `0x66666667` >> 4 / >> 5; `addiu a1,a1,400` / `-400` | nur bei Wand/Boden (Status-Bit 0x80 = 0, `andi v0,v0,0x80` @0x80021048) |
| Flammen-Zusatz je Spawn | vel.x += rng%25, acc.y += rng%8, +0x4A := 1 | @0x80021184-22c (und 2×) | `0x51eb851f` >> 3; `sh s1,-29382(at)` | Schadensberechtigung Op 19 |
| Bodenflamme Skript | Bank 5 Skript 5 | CORE00.ESP (RE2) @0x07F8, Step @0x0800 | `00 1b 2e 32 00 10 00 10 00 05 00 00 60 00 …` | Op B 27, step[2] 46, step[3] 50 |
| Luft-Wachsen | Zähler 8 + rng%3, ×1.010 / ×1.007 | RE2 @0x8001fbb4-b8, @0x80022350-84 | `addiu v0,v0,8 / sh v0,66(v1)` | Op 27/58 |
| Landung | Op A 19, Op B 29, vel.x 180, acc.x −10 − rng%11, Zähler 38 + rng%8 | RE2 @0x80020b84-c24 | `addiu v0,zero,19`/`29`/`180`, `addiu v1,zero,-10`, `addiu v0,v0,38` | Op 46 |
| Brennen | Schaden ab step[0x16] ≥ 16 und X-Aspekt > 0x1000; Wachsen ×1.009/×1.002; Schrumpfen 90 + rng%11 Bilder ×0.99/×0.98 | RE2 @0x8001f2e8, @0x8001f2fc, @0x8001f4c0 | `sltiu v0,v0,0x10`, `sltiu v0,v0,0x1001`, `addiu v0,v0,90` | Op 19 |
| Folgeflamme | 0x0504xxxx, Skala ×0.8, alle 15 Bilder solange vel.x ≥ 61 | RE2 @0x8001fd78, @0x8001fd94-b8, @0x8001fdf0 | `slti v0,v0,61`, `0x88888889` (/15), `lui v0,0x504` | Op 29 |
| Nachbrenner | Box {−600,0,300,150}, Punkt y − 100, Hitcode 0x2002000A | RE2 @0x80020768-bc; Box-Bytes @0x80010910 | `a8 fd 00 00 2c 01 96 00`, `addiu v0,v0,-100`, `lui a3,0x2002 / ori a3,a3,0xa` | nur RE2-KI-Typen belegbar |
| RE2-Hitcodes (Alternative zu Art 3/4) | Säure 0x1002000B, Brand 0x0002000A | RE2 @0x800216e4-f0 / @0x80021058-64 | `lui a3,0x1002 / ori a3,a3,0xb`, `lui a3,0x2 / ori a3,a3,0xa` | nur falls die Zustellung RE2 statt RE1.5 sein soll |
| RE2-Aufschlag-Boxen | Wand/Boden {−1200,0,600,300}, Gegner {−600,0,300,150} | @0x8001093C / @0x80010944 | `50 fb 00 00 58 02 2c 01` / `a8 fd 00 00 2c 01 96 00` | nur zur RE2-Zustellung |

## PORT-ABGLEICH

| Stelle | Heute | Lücke |
|---|---|---|
| `engine/src/game_step_common.c:1775` `ENT[9] = {1,1,1,0}` | Port-Brücke: Sofort-Treffer beim Abzug (`re15_player_weapon_fire(9)`) | Schaden soll aus der Explosion kommen (Nachbar-Dossiers Wurf/Schaden) |
| `game_step_common.c:1779-1780` `ENT[10]/[11] = {1,0,1,0}` | nur Munition −1, byte-true zum RE1.5-Stub | bleibt so (Handler sind Stubs); die Wirkung kommt über das Projektil |
| `game_step_common.c:1942-1961` | Projektil (4, 0x0D, 0x1000) nur bei `re15_player_equipped_weapon() == 9` (Spiegel von @0x8003368c) | Für 10/11 dieselbe Spawn-Regel (Port-Zuordnung, §3.1) und eine Art-Kennung am Slot, damit die Explosion Säure/Brand wählen kann (RE1.5 hat dafür kein Feld; Routine 31 liest weder +0x70/+0x71/+0x72) |
| `engine/src/re15_esp.c` (`esp_fx_dispatch` @:523, `esp_fx_dispatch_b` @:724) | kennt Routine 38 u. a., **nicht 29/30/31** (grep) | Wurf/Flug/Explosion fehlen für alle drei (Nachbar-Dossier Wurf) |
| RE2-FX-Maschine | fehlt ganz (`grep re2fx` leer; Plan `analysis/konstruktion_2026-08-23/re2-fx-system.md` §3 unumgesetzt) | für Op 48/49-Kinder, Bodenflammen (Ops 27/28/29/30/46/19/58/40) nötig; Daten: RE2 CORE00.ESP Bänke 3/4/5 + RE2 TEX.TIM |
| Töne | RE2-Mini-Bank-Muster vorhanden (`platform/pc/src/audio_pc.c:1074ff` ENEMSE, `:1131-1171` ELEVSE; `shared_assets/RE2/*.VBS`) | ARMS0A/0B-VAG 3 (Brand/Säure-Aufschlag) fehlen in `shared_assets/RE2/`; RE1.5-SE 0x04080001 der HE-Explosion ist Sache des Wurf-Dossiers |
| Schaden über Resolver | `re15_damage.c:41-58` `re15_damage_table`/`re15_react_table` bytegleich zu @0x8006F418/@0x8006F430 (Nachbar-Dossier Schaden §3) | Aufruf mit Art 3/4 fehlt (wie im Original) |
| RE2-Reaktion bei Resolver-Weg | `enemy_ai_re2_zombie.c:3847` `re2z_row_from_atktype[3]/[4] = 17/17` (gelesen `:3926`) | Art 3/4 landen auf RE2-Zeile 17 (Raketen-Klasse) statt 11/10 (Säure/Brand); `re2z_row_from_weapon` (`:3829`) hat 10 → 11, 11 → 10 richtig. Beim Bau Art 3 → 11, Art 4 → 10 setzen (Port-Zuordnung, gleiche Begründung wie `re2z_row_from_weapon`) |
| Inventar | Ziffernfarbe `prop8` umgesetzt (`engine/src/re15_inv_screen.c:1066-1069`); Bänke PL00W0A/0B werden geladen (bytegleich W09) | — |
| Wurf-Harness | `RE15_GIVE`/`RE15_EQUIP` in `platform/pc/main.c` (Runde 30) | Messläufe für 10/11 möglich |

## OFFEN

1. **Explosiv-GL in RE2 = 5 Teilgeschosse** (5 × 0x020C0A00 @0x80044be8-0x80044e74, alle mit Sub 12 → alle mit Schaden in
   Op 47). Byte-belegt, aber NICHT dynamisch gemessen: im Repo liegt kein RE2-Datenträgerabbild (nur `info/re2leon`-Dateien,
   `find *.cue/*.iso/*.chd` → nur RE1.5). Für 0x0A/0x0B ohne Belang (Brand/Säure = 1 Geschoss), für einen RE2-Vergleich der
   Handgranate aber ein Stolperstein. Nächster Weg: RE2-Abbild beschaffen, Savestate mit feuerndem GL, FX-Pool @0x800D8CF0 lesen.
2. **Aussehen der RE2-Kinder** (Bank 3 Skr. 3/4/7, Bank 4 Skr. 4/5, Bank 5 Skr. 4/5): nicht gerendert. Belegt ist nur die
   Farbseite: RE2-TEX.TIM (Lader `"TEX TIM"` @0x80010a08, `jal 0x80012fb8` @0x8002b8c0, Hochladen `jal 0x80076a40`
   @0x8002b8d8) trägt die CLUT (256,480) 32×19; die von den GL-Kindern benutzten Zeilen 480..484 (x 272..287: Bank 3/4 Basis
   480, +1/+2/+3 = 481/482/483, Bank 5 = 484) sind **bytegleich mit RE1.5 `DATA/TEX.TIM`** (Zeilenvergleich, ab 485 verschieden;
   Bild `re2_tex_tim_cluts.png`: 481 dunkelgrau, 482 braun, 483 weiß→gelb→rot, 484 weiß→flieder→grau). Die Pixelseiten sind
   NICHT bytegleich (RE2-Seite 2 ≈ RE1.5-Seite 3 zu 80 %). Weg: RE2-TEX.TIM-Seiten + Anim/UV der Bänke als Sprite-Katalog
   rendern (`re2_esp_scripts.py` liefert Anim-Startindizes); Blend (TPage |= 0x20 = additiv) mitrechnen.
3. ~~Op B 25 / Op 64~~ — **geklärt:** Op 25 @0x8001FA08 = Wassertest der Folgeflamme (`jal 0x800527b4` @0x8001fa20, unter
   der Oberfläche Op A := 0 / Status := 0 @0x8001fa54-58). Op 64 @0x80022728 (y := Boden, acc.y/vel.y/step[2]/Op B := 0,
   @0x80022734-70) wird für GL-Flammen nie angesprungen: Op 50 setzt zwar step[2] := 64, aber die gelandete Flamme hat Op B 29,
   das step[2] nur als Zähler benutzt; Op 28 (der einzige `Op[step[2]]`-Dispatch) läuft nur in der Luft, Op 40 nur am Boden.
4. **Schaden des Bodenfeuers für Gegner außerhalb der RE2-KI**: RE1.5 hat keine Quelle (Art 5 = Flammenwerfer-Fläche ohne
   Aufrufer; Spalte 14 @0x8006E0D0 ist Hitscan). Versucht: Aufrufer-Zensus FUN_80012d60 (2 Stellen), aca5d-Zensus (46 Leser).
   Ohne Nutzer-Entscheid keine Zahl.
5. **"toxic gas" vs. Säure**: RE1.5-Text zu 0x0A beschreibt eine Giftgas-Granate (@0x800C53CC). RE2 hat keine Gas-Runde; die
   Zuordnung auf die Säure-Runde (Op 49) ist Port-Zuordnung.
6. **ARMS-Dateiindex → Dateiname**: 305/307/309 (@0x800A8118) auf ARMS09/0A/0B über das EDH/VB-Paarraster und die
   Record-Belegung (17/18/19 je nur in einer Datei) zugeordnet; die CD-TOC-Tabelle von FUN_80012fb8 nicht gelesen.
7. **Spieler-Eigenschaden durch RE2-Hitcodes** (falls die RE2-Zustellung gewählt würde): Applier-Liste laut Nachbar-Dossier =
   Gegner-Zeigerliste 0x800CFE1C; ob der Spieler dort steht, nicht geprüft. Beim empfohlenen RE1.5-Resolver-Weg trifft die
   Explosion den Spieler wie die Handgranate (Spieler-Zweig @0x80012e18-f04, Art ≥ 2 → keine Blutung/Gift).
8. **Farben der Ziffern-CLUT-Zeilen 491/492/493** im Inventar-VRAM nicht gemessen (die Port-Umsetzung nimmt die CLUT aus dem
   geladenen Bild, gleiche Formel; ein Savestate mit offenem Inventar würde die RGB-Werte zeigen).
