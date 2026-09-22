# RE2: „This key is useless now. / Discard?" — vollstaendiger Mechanismus

Stand 2026-09-21. Quelle: `info/re2leon/PSX.EXE` (= `SLUS_007.48`, 987136 Bytes),
`info/re2leon/PL0/RDT/*.RDT` (250 Leon-RDTs), `ghidra_re2_Leon.txt`,
`RE2_Quellcode_V2/FUN_*.c`. Beide Baeume NUR gelesen.

Adressumrechnung (selbst nachgemessen, nicht uebernommen):
EXE-Header `PS-X EXE`, `t_addr = 0x80010000`, `t_size = 0x000F0800`, Header 0x800,
Dateigroesse 0x800 + 0xF0800 = 987136 → **RAM = 0x80010000 + Datei-Offset − 0x800**.

---

## 0. Der Wortlaut — korrigiert gegenueber der Aufgabenstellung

Roh ab Datei-Offset **0x8F960** (= RAM **0x8009F160**), 19+16 Bytes, komplette Nachricht:

```
30 44 45 4f 00 47 41 55 00 45 4f 00 51 4f 41 48 41 4f 4f 00 4a 4b 53   "This key is useless now"
01                                                                      Glyph 0x01 = '.'
fc                                                                      Zeilenwechsel
20 45 4f 3f 3d 4e 40                                                    "Discard"
1b                                                                      Glyph 0x1b = '?'
fb 00                                                                   Auswahlbox, Layout 0
fe 00                                                                   Seitenende / warten
```

> **„This key is useless now."**
> **„Discard?"**   →   **Yes** / **No**

**Wichtige Korrektur:** `0x01` ist hier **kein** Steuerbyte, sondern der **Punkt-Glyph**.
Beleg aus einem zweiten, unabhaengigen Datensatz — der Item-Namenstabelle:
`0x8009E569` = `1f 01 00 24 3d 4a 40 00 23 51 4a` = `C` `.` ` ` `Hand Gun` → „C. Hand Gun";
`0x8009E5C5` = `1f 4b 48 50 00 2f 01 1d 01 1d 01` → „Colt S.A.A.";
`0x8009E66A` = `29 01 23 01 00 ...` → „M.G. Bullets".
Ebenso enden alle Nachbarnachrichten auf `01` vor dem `fe`: Msg 10 @0x8009F185
= `fc "You unlocked it" 01 fe 00` → „You unlocked it." Der Satzpunkt gehoert also
**in den Text**. Die FSM behandelt `0x01` als druckbaren Glyph, weil sie nur
`*p < 0xEE` prueft (`FUN_8003027c`, `LAB_800305b4`).

Es heisst also **nicht** „This item is not used anymore" und **nicht** „drop it" —
RE2 sagt **„key"** (nicht „item") und **„Discard?"** (nicht „Drop it?"), mit Punkt.

Gegenprobe: im gesamten RE2-Baum gibt es genau einen weiteren „useless"-Treffer
(Datei-Offset 0x91FF4 = RAM 0x800A17F4, „It's useless by itself", C4-Zuender);
„anymore", „needed", „unnecessary", „No longer", „Dispose", „Throw" kommen in dieser
Kodierung **nicht** vor (0 Treffer, Suche ueber die gesamte Datei).

---

## 1. Wer zeigt die Abfrage? — die Nachrichten-Id

Die Strings liegen **nicht** als Pointer-Tabelle (Suche ueber alle 32-Bit-Worte der
Datei nach Zeigern in 0x8009E000–0x800A3000: **0 Treffer**), sondern als
**u16-Offsettabelle + Basis**. Der Aufloeser ist `FUN_8002FE38`.

### `FUN_8002FE38(a0, a1, a2, a3)` — Nachricht anzeigen

* `a1 & 0x300` waehlt die Nachrichten-**Gruppe**, `a2` ist der **Index**, `a3` geht nach
  0x800E8760 (Freeze-Flags).
* `a1 = 0x100` → Sprung bei `@0x8002FEC8`, dann `@0x8002FF20`:

```
8002ff14: addiu v0,zero,64          ; 0x40
8002ff20: andi  v1,v1,0x840         ; v1 = [0x800CFB74]
8002ff24: bne   v1,v0,0x8002ff58    ; != 0x40  -> Block B
8002ff28: sll   v0,a2,1
; (x & 0x840) == 0x40  -> Block A  (englisch):
8002ff3c: lui   at,0x800a
8002ff40: addu  at,at,v0
8002ff44: lhu   v0,-3224(at)        ; Tabelle @0x8009F368 + a2*2
8002ff48: lui   v1,0x800a
8002ff4c: addiu v1,v1,-4148         ; Basis   @0x8009EFCC
8002ff54: addu  v0,v0,v1
8002fffc: lui   at,0x800f
80030000: sw    v0,-30908(at)       ; [0x800E8744] = Nachrichten-Zeiger
```

* **Block A** (englisch): Tabelle **RAM 0x8009F368** (Datei 0x8FB68),
  Basis **RAM 0x8009EFCC** (Datei 0x8F7CC), **24 Eintraege**
  ((0x8009F398−0x8009F368)/2 = 24).
* Block B (andere Fassung): Tabelle 0x8009EF9C / Basis 0x8009ECC4, ebenfalls 24.

Tabelleneintrag **9** = `0x0194` → `0x8009EFCC + 0x194` = **0x8009F160**. Das ist
punktgenau unsere Zeichenkette. **Die Nachrichten-Id ist also `(Gruppe 0x100, Index 9)`.**

Block A vollstaendig (englisch), zur Einordnung:

| Idx | RAM | Text |
|----|------|------|
| 0/1 | 0x8009EFCC / 0x8009F01F | It's an old typewriter. |
| 2/3 | 0x8009F084 / 0x8009F09A | The desk is locked. |
| 4 | 0x8009F0CD | There's nothing more. |
| 5 | 0x8009F0E5 | You have used the ‹Name›. |
| 6 | 0x8009F100 | Doors can be unlocked / by a card key. |
| 7 | 0x8009F126 | Will you use the ‹Name›? **+ Auswahlbox** |
| **9** | **0x8009F160** | **This key is useless now. / Discard? + Auswahlbox** |
| 10 | 0x8009F185 | You unlocked it. |
| 11 | 0x8009F198 | It's locked from inside. |
| 12–23 | 0x8009F1B3 … 0x8009F358 | „It's locked." / „The door is opened with a card key." |

### Der einzige Aufrufer mit (0x100, 9)

Alle 22 `jal 0x8002FE38` (Suchwort `0x0C00BF8E`) wurden abgeklopft; genau **einer**
uebergibt `a1 = 0x100, a2 = 9`:

```
80051820: andi v0,v0,0xff
80051824: bne  v0,zero,LAB_8005185c
80051828: clear a0
8005182c: li   a1,0x100
80051830: li   a2,0x9
80051834: jal  FUN_8002fe38          <-- HIER
80051838: lui  a3,0xff00
```

Die Funktion ist `LAB_80051718`. Sie hat **0 jal-, 0 j-, 0 Branch-Xrefs**; Ghidra
zeigt genau eine Referenz: `80051670(*)`. Sie wird als **Fortsetzungs-Zeiger** in
`DAT_800D4498` eingetragen und danach pro Frame aufgerufen.

---

## 2. Was ist `0xFB`? — die Ja/Nein-Auswahl (belegt, nicht vermutet)

Die Nachrichten-FSM ist `FUN_8003027C` (Zustand in `0x800E873D`, 7 Zustaende,
Sprungtabelle @**0x80010A24**: 0x800302D0 / 0x80030328 / 0x80030618 / 0x800306EC /
**0x80030728** / 0x8003079C / 0x800307C4). Im Zustand 1:

```c
if (bVar3 == 0xfb) {
    *(undefined1 *)(param_1 + 0x5c8d) = 4;                 /* [0x800E873D] = Zustand 4 */
    *(char *)(param_1 + 0x5caa) = (char)(8 << bVar1);      /* [0x800E875A] = Eingabesperre */
    break;
}
```

im Maschinencode:

```
800304b8: addiu v0,zero,4
800304bc: sb    v0,23693(s1)      ; [0x800E873D] = 4  (Zustand 4 = Auswahlbox)
800304c0: addiu v0,zero,8
800304c4: sllv  v0,v0,s2          ; s2 = (DAT_800dfc1a == 0)   @0x800302A8: sltiu s2,v0,0x1
800304cc: sb    v0,23722(s1)      ; [0x800E875A] = 8 << s2  = Eingabesperre in Frames
```
(`param_1 = s1 = 0x800E2AB0`.)

Zustand 4 (`@0x80030728`):

```c
uVar5 = pbVar6[1];                                   /* das Byte NACH 0xFB, hier 0x00 */
if ((DAT_800cfb74 & 0x840) == 0x40)                  /* dieselbe Weiche wie oben */
     FUN_80030844(param_1, &UNK_8009df10 + (uVar5 & 0xf)*6, uVar5, 1);
else FUN_80030844(param_1, &UNK_8009dee4 + (uVar5 & 0xf)*6, uVar5, 0);
```

**`0xFB` = „Auswahlbox oeffnen", das Folgebyte waehlt das Layout** (6 Byte je Eintrag).
Layouttabelle englisch **0x8009DF10** (Datei 0x8E710), Eintrag 0 (unser Fall):

```
02 d2 32 00 00 00
 |  |  |     +---- u16 Label-Startindex = 0
 |  |  +--------- Schrittweite = 50 px
 |  +------------ x = 210
 +--------------- Anzahl Eintraege = 2
```

### Die Texte „Yes" / „No"

`FUN_80030844` holt jedes Label als
`&UNK_800A1F64 + *(u16*)(&DAT_800A2040 + (labelidx*2 + 2*k + lang)*2)`.
Mit `labelidx = 0`, `lang = 1` (englisch):

| k | Tabellenindex | Offset | RAM | Text |
|---|---|---|---|---|
| 0 | 1 | 0x0005 | **0x800A1F69** | **„Yes"** |
| 1 | 3 | 0x000E | **0x800A1F72** | **„No"** |

(Die geradzahligen Indizes 0/2 zeigen auf die zweite Fassung @0x800A1F64 / 0x800A1F6E.)

### Bedienung und Ergebnis

In `FUN_80030844`:

| Vorgang | Code |
|---|---|
| naechster Eintrag | `DAT_800ce310 & 0x800` → `uVar11++` (Umlauf bei `choicedef[0]−1`), SFX `FUN_8005ba28(0x4040000,0)` |
| voriger Eintrag | `DAT_800ce310 & 0x400` → `uVar11--` (Umlauf) |
| bestaetigen | `DAT_800ce310 & 0x1000` **und** `[0x800E875A] == 0` |
| beim Bestaetigen | `DAT_800ce30c = 0xffff;` `DAT_800cfbdc = [0x800E875C];` `[0x800E873C] &= 0x7f;` |
| Auswahl merken | `[0x800E873C] = ([0x800E873C] & 0xf0) \| uVar11` |

**Das Ergebnis der Abfrage ist das untere Nibble von `[0x800E873C]`: 0 = „Yes", 1 = „No".**
Bit 0x80 derselben Zelle heisst „Nachricht/Abfrage laeuft noch".

**Vorbelegung = „Yes":** `FUN_8002FE38` setzt beim Start jeder Nachricht
`@0x8002FE80: addiu t2,zero,128` / `@0x8002FE88: sb t2,[0x800E873C]` — also exakt `0x80`,
unteres Nibble = 0. Der Cursor steht beim Oeffnen auf **Yes**.

**Eingabesperre:** `[0x800E875A] = 8 << (DAT_800dfc1a == 0)`, heruntergezaehlt in
`FUN_80030844`; vorher wird `& 0x1000` ignoriert. Standard (`DAT_800dfc1a == 0`)
= **16 Frames**.

`DAT_800ce310` ist ein **logisches** (umgelegtes) Tastenwort. Die Zuordnung
logisch→physisch steht in der konfigurierbaren Tabelle **0x800A26A0**
(16 × u16 je Konfiguration, 32 Byte Schritt, Konfigurationsindex in `[0x800D46B2]`),
aufgebaut in der Schleife `@0x80039178–0x800391F0`
(`sllv v1,t2,a0` → logisches Bit = `1 << i`, physische Maske = `Tabelle[i]`).
Die *physische* Taste habe ich bewusst **nicht** behauptet — `0x800CE310` wird von
einer zweiten Schleife mit einer anderen Tabelle (`0x800A2700`, siehe `@0x80039218`)
gefuellt, die ich nicht zu Ende verfolgt habe. Fuer den Port ist nur die Semantik
noetig: bestaetigen / vor / zurueck.

---

## 3. Woran erkennt RE2, dass ein Schluessel verbraucht ist?

**Antwort: an einem Verbrauchszaehler im Inventarplatz — es gibt keine „ist ein
Schluessel"-Tabelle und keine Id-Liste.**

### Das Inventar

`FUN_800696CC(item_type)` (decompiliert, `RE2_Quellcode_V2/FUN_800696cc.c`):

```c
uVar1 = 0; iVar2 = 0;
do {
    if ((byte)(&DAT_800d4a3c)[iVar2] == param_1) return uVar1;   /* Platz gefunden */
    uVar1++; iVar2 += 4;
} while (uVar1 < DAT_800d46ac);                                  /* Platzanzahl */
return 0xffffffff;
```

Daraus und aus `FUN_8006947C(slot, type, count, flags)` und `FUN_80069714()`
(Inventar-Kompaktierung, kopiert `+0x8854/+0x8855/+0x8856`):

| Adresse | Bedeutung |
|---|---|
| **0x800D4A34** | 8-Byte-Bitfeld „Tuer bereits aufgeschlossen" (Flags 0…63) |
| **0x800D4A3C + 4·slot** | `+0` = **Item-Id** |
| **0x800D4A3D + 4·slot** | `+1` = **Anzahl / verbleibende Verwendungen** |
| **0x800D4A3E + 4·slot** | `+2` = Flags (1/2 = linke/rechte Haelfte eines Doppelplatzes) |
| 0x800D46AC | Anzahl belegbarer Plaetze |

Dass 0x800D4A34 genau 8 Byte vor dem Inventar liegt, passt exakt zu `a1 & 0x3f`
(64 Flags = 64 Bit = 8 Byte) in `FUN_80077360`/`FUN_8007730C`.

### Der Tuer-Handler `LAB_80051514` (AOT-Dispatch-Tabelle `0x800A73C8`)

```
800515a8: lbu   a1,0xf(s2)          ; door_aot_set-Byte 29  ("locked")
800515b0: andi  v0,a1,0x80
800515b4: beq   v0,zero,LAB_800516c8 ; Bit 0x80 nicht gesetzt -> gar nicht verschlossen
800515b8: ori   a0,zero,0x884c
800515bc: addu  a0,s3,a0            ; a0 = 0x800D4A34  (s3 = 0x800CC1E8)
800515c0: jal   FUN_80077360        ; Tuer-Flag (a1 & 0x3f) schon gesetzt?
800515c4: andi  a1,a1,0x3f
800515c8: bne   v0,zero,LAB_800516cc ; ja -> einfach oeffnen
800515d0: lbu   s0,0x10(s2)         ; door_aot_set-Byte 30  = Schluessel-Id
800515d4: li    v0,0xfe
800515d8: bne   s0,v0,LAB_80051608  ; 0xFE -> Msg (0x100,10) "You unlocked it."
800515dc: li    v0,0xff             ; 0xFF -> Msg (0x100,11) "It's locked from inside."
80051628: jal   FUN_800696cc        ; sonst: Platz des Schluessels suchen
8005162c: move  a0,s0
80051630: move  s1,v0
80051634: bltz  s1,LAB_800516a0     ; nicht dabei -> Msg (0x100, key_id-0x4C) "It's locked."
80051638: clear a0
8005163c: li    a1,0x100
80051640: li    a2,0x5              ; Msg 5 "You have used the <Name>."
80051648: sb    s0,[0x800E873F]     ; Item-Id fuer die 0xF8-Namenseinsetzung
8005164c: jal   FUN_8002fe38
80051654: lui   a0,0x225
80051658: jal   FUN_8005ba28        ; Aufschliess-SFX 0x02250000
80051660: lui   v0,0x8005
80051664: addiu v0,v0,0x1718
80051670: sw    v0,[0x800D4498]     ; <== Fortsetzung LAB_80051718 einhaengen
80051674: addiu v0,s1,0x1
80051680: sb    v0,[0x800D4249]     ; = Platz + 1  (nicht-null = „Abfrage steht an")
80051690: jal   FUN_8007730c        ; Tuer-Flag setzen = dauerhaft offen
```

### `LAB_80051718` — der Kern (pro Frame, solange `[0x800D4498]` gesetzt ist)

```
80051718: lui   v1,0x800d
8005171c: lbu   v1,[0x800D4249]
8005172c: addiu s0,s0,-0x3e18        ; s0 = 0x800CC1E8
80051730: bne   v1,zero,LAB_800517dc ; Abfrage-Pfad

; --- Antwort-Pfad ([0x800D4249] == 0) ---
8005173c: lbu   v1,[0x800E873C]
80051744: andi  v0,v1,0x80
80051748: bne   v0,zero,LAB_80051870 ; Abfrage laeuft noch -> warten
8005174c: andi  v0,v1,0x1            ; unteres Nibble Bit 0 = Auswahl
80051750: bne   v0,zero,LAB_800517a8 ; == 1 ("No")
; „Yes" (Auswahl 0):
80051774: sb    zero,[0x800D4A3C + 4*idx]   ; Item-Id  := 0
80051794: sb    zero,[0x800D4A3E + 4*idx]   ; Flags    := 0
80051798: jal   FUN_80069714                ; Inventar kompaktieren
; „No" (Auswahl 1):
800517c4: sb    v1,[0x800D4A3D + 4*idx]     ; Anzahl := 1  (v1 = 1 aus 0x80051754)
800517d0: sw    zero,[0x800D4498]           ; fertig

; --- Abfrage-Pfad ---
800517dc: lbu   v0,[0x800E873C]
800517e4: addiu v1,v1,-1
800517ec: sw    v1,[0x800C3B98]      ; idx := [0x800D4249] - 1 = Inventarplatz
800517f0: andi  v0,v0,0x80
800517f4: bne   v0,zero,LAB_80051870 ; Msg 5 laeuft noch -> warten
80051808: lbu   v0,[0x800D4A3D + 4*idx]
80051810: addiu v0,v0,-1             ; *** ANZAHL HERUNTERZAEHLEN ***
8005181c: sb    v0,[0x800D4A3D + 4*idx]
80051820: andi  v0,v0,0xff
80051824: bne   v0,zero,LAB_8005185c ; > 0 -> keine Abfrage, fertig
80051830: li    a2,0x9               ; == 0 -> FRAGEN
80051834: jal   FUN_8002fe38
80051848: or    v0,v0,0xff000000
80051850: sw    v0,[0x800CFBDC]      ; Spieler bleibt eingefroren
8005186c: sb    zero,[0x800D4249]
```

**Die Bedingung ist also exakt: „die Anzahl im Inventarplatz erreicht durch diese
Tuer-Benutzung 0".** Kein Bit je Item-Id, kein Save-Flag, keine Id-Liste.

### Gegenprobe an den Spieldaten — die Anzahl IST die Anzahl der Tueren

Aus den 250 Leon-RDTs, `item_aot_set` (Opcode 0x4E, 22 Byte, Typ @Byte 14–15,
Anzahl @Byte 16–17) gegen `door_aot_set` (Opcode 0x3B, 32 Byte, „locked" @Byte 29
Bit 0x80, Schluessel-Id @Byte 30):

| Id | Name (identifiziert) | Anzahl beim Aufnehmen | gefundene verschlossene Tueren |
|----|----|----|----|
| 0x59 | Precinct Key → **Spade Key** | 2 (bzw. 1 in der zweiten Fassung) | 1 |
| 0x5A | Precinct Key → **Diamond Key** | **2** | **2** ✔ |
| 0x5B | Precinct Key → **Heart Key** | **1** | **1** ✔ |
| 0x5C | Precinct Key → **Club Key** | **3** | **3** ✔ |
| 0x5F | **P. Room Key** | **1** | **1** ✔ |
| 0x62 | **Master Key** | 2 | 1 |
| 0x63 | **Platform Key** | **1** | **1** ✔ |

Beispielsatz (ROOM30B0.RDT @0xD1E):
`4e 04 02 31 00 00 a4 c8 19 ea 38 04 60 04 5c 00 03 00 7c 00 00 a1`
→ Typ `0x005C`, Anzahl `0x0003`.
Beispiel-Tuer (ROOM2040.RDT @0x191E):
`3b 02 01 31 00 00 d2 c8 71 da 40 06 52 08 b5 98 00 00 ac aa 00 00 01 03 00 00 08 01 00 85 59 00`
→ Byte 29 = `0x85` (verschlossen, Flag 5), Byte 30 = `0x59`.

Damit ist der Mechanismus **gemessen** bestaetigt: der Club Key wird mit 3 aufgenommen,
oeffnet 3 Tueren, und erst nach der dritten fragt das Spiel. Genau das ist das
RE2-Spielgefuehl.

### Sollzahl und betroffene Items

* **Item-Id-Raum:** die englische Namenstabelle liegt @**0x8009EBAC**, Basis
  @**0x8009E550** (belegt in `FUN_80030B9C`). Die Haupttabelle endet, wo die
  Alternativnamen-Tabelle @**0x8009ECAE** beginnt → `(0x8009ECAE − 0x8009EBAC)/2` =
  **129 Ids = 0x00…0x80**. Die zweite Sprachfassung (Tabelle 0x8009E438 / Alt
  0x8009E53A, Basis 0x8009DF3C) ergibt dieselben 129 — zwei unabhaengige Zaehlungen.
* **Tragbare Gegenstaende:** die 8-Byte-Definitionstabelle @**0x800A9E1C**
  (`+1 & 0x0f` = Alternativname, `+3` = Anzahl Kombi-Rezepte, `+4..7` = Rezeptzeiger)
  ist bis einschliesslich Id **0x64** gefuellt und endet bei 0x800AA144 (ab dort
  zeigen die „Zeiger" ins Nirgendwo und die naechste Tabelle wird per
  `lhu@0x8006D0B0` angesprochen) → **0x00…0x63 = 100 echte Gegenstaende**;
  0x64…0x67 = „no item", 0x68…0x80 = 25 Akten/Dokumente.
* **Vom Motor erwartetes Schluessel-Id-Fenster:** der „du hast ihn nicht"-Zweig
  rechnet `a2 = key_id − 0x4C` (`@0x800516B4: addiu a2,s0,-0x4c`) in einen Block mit
  **24** Eintraegen → **0x4C…0x63, genau 24 Ids** — und 0x63 ist zugleich die hoechste
  echte Item-Id. Die Ids **0x58…0x63 (12 Stueck)** sind die, deren Index (12…23)
  tatsaechlich auf „It's locked." / „The door is opened with a card key." zeigt.
* **In den ausgelieferten Leon-Raumdaten tatsaechlich als Tuerschluessel verwendet:**
  **7 Ids** — 0x59, 0x5A, 0x5B, 0x5C, 0x5F, 0x62, 0x63 (10 verschlossene Tueren),
  dazu die Sondermarken 0xFE (3×) und 0xFF (4×), die der Handler
  @0x800515D4/0x800515DC abfaengt und die **nie** zur Abfrage fuehren.

**Abgrenzung / was die Zahl 7 nicht ist:** der Scan ist eine **untere Schranke**.
Er erfasst `door_aot_set` (0x3B) mit der Signatur `b1 ≤ 0x1F, b2 = 0x01, b3 = 0x31,
b28 = 0, b31 = 0`, Zielraum muss als Datei existieren, Breite/Tiefe plausibel
(328 Saetze, davon 17 verschlossen). Der 4-Punkt-Zwilling `door_aot_set_4p` (0x68,
40 Byte) lieferte unter der analogen Signatur 18 Saetze, **keinen** davon verschlossen.
Skriptgesteuerte Tueren (`sce_key_ck` u. a.) und die Card-Key-Tueren laufen ueber
einen anderen Pfad und wurden **nicht** ausgezaehlt — sie koennen die Abfrage aber
ohnehin nicht ausloesen (siehe Abschnitt 5).

Alternativnamen (`DAT_800A9E1D[id*8] & 0x0f` → Namensplatz `0x81 + alt`, Beleg
`FUN_800693D0`: `param_4 = uVar2 + 0x81`):

| Id | Grundname | alt | identifiziert als |
|---|---|---|---|
| 0x33 | Red Jewel | 6 | Virgin Heart |
| 0x43 | Crank | 7 | Square Crank |
| 0x53 | Special Key | 10 | Locker Key |
| 0x59 | Precinct Key | 1 | Spade Key |
| 0x5A | Precinct Key | 2 | Diamond Key |
| 0x5B | Precinct Key | 4 | Heart Key |
| 0x5C | Precinct Key | 5 | Club Key |
| 0x5D | C. Panel Key | 8 | Down Key |
| 0x5E | C. Panel Key | 9 | Up Key |

---

## 4. Was passiert bei „Yes"?

```
80051758..80051774: [0x800D4A3C + 4*idx] = 0     ; Item-Id loeschen
80051778..80051794: [0x800D4A3E + 4*idx] = 0     ; Flags loeschen
80051798:           jal FUN_80069714             ; Inventar nachruecken
```

`FUN_80069714` (decompiliert) schiebt alle nachfolgenden Plaetze um eins nach vorn
(`(&DAT_800d4a3c)[i] = puVar2[0x8854]` usw.), korrigiert die Cursorpositionen
`DAT_800d5bf8` / `DAT_800d69f4` und nullt den letzten Platz.

**Der Gegenstand wird nur aus dem Inventar entfernt.** Es gibt in diesem Pfad keinen
einzigen weiteren `jal` — kein Ablegen in die Welt, kein `item_aot_set`, keine
Raum-Instanz. Das ist der komplette Zweig.

Bei „No" (`@0x800517A8`): `[0x800D4A3D + 4*idx] = 1`. Die Anzahl wird von 0 auf 1
zurueckgesetzt — sonst wuerde sie bei der naechsten Benutzung auf 0xFF unterlaufen.
Der Schluessel bleibt mit Anzahl 1 liegen und die Abfrage kommt bei der naechsten
Benutzung erneut.

In beiden Faellen danach: `[0x800D4498] = 0` bzw. `[0x800D4249] = 0` → Fortsetzung
ausgehaengt.

---

## 5. Wann wird gefragt?

**Unmittelbar nach dem Aufschliessen der Tuer, nicht im Inventar und nicht beim
Anwaehlen.** Die Kette:

1. Spieler laeuft in die Tuer-AOT → `LAB_80051514` (AOT-Dispatch @0x800A73C8).
2. Tuer verschlossen, Schluessel vorhanden → Msg 5 **„You have used the ‹Name›."**,
   SFX 0x02250000, Tuer-Flag gesetzt, `[0x800D4498] = &LAB_80051718`,
   `[0x800D4249] = Platz + 1`.
3. Ab dem naechsten Frame laeuft `LAB_80051718`. Solange `[0x800E873C] & 0x80`
   gesetzt ist (Msg 5 steht noch), tut sie nichts.
4. Sobald Msg 5 weg ist: Anzahl −1. **> 0 → Schluss.** **== 0 → Msg 9 + Auswahlbox.**
5. Ab jetzt `[0x800D4249] == 0`; `LAB_80051718` wartet, bis `[0x800E873C] & 0x80`
   faellt (der Spieler hat Yes/No bestaetigt), und wertet das untere Nibble aus.

Waehrend der Abfrage bleibt der Spieler eingefroren: `[0x800CFBDC] |= 0xFF000000`
(@0x80051848) bzw. beim Nachrichtenstart `DAT_800cfbdc |= [0x800E8760]` (a3 = 0xFF000000
aller Aufrufer); zurueckgesetzt wird es aus `[0x800E875C]` beim Schliessen der Box.

**Die Abfrage haengt an genau einer Stelle** (`@0x80051670`, einzige Referenz auf
`LAB_80051718` im ganzen Programm). Sie kann folglich **nur** durch eine
`door_aot_set`-Tuer ausgeloest werden, nie durch „Use" im Inventar, nie durch ein
Skript-Ereignis.

---

## 6. Was der Port dafuer braucht (Umsetzungsnotiz, keine Aenderung in diesem Lauf)

* Inventarplatz braucht ein **Anzahl-Byte**, das bei der Schluesselvergabe mit der
  Zahl der Tueren belegt wird, die der Schluessel oeffnet (RE2: `item_aot_set`
  Byte 16–17).
* Nach erfolgreichem Aufschliessen: Anzahl −1; bei 0 die Abfrage
  `„This key is useless now."` / `„Discard?"` mit **Yes (vorbelegt) / No**.
* Yes → Platz leeren (Id 0, Anzahl 0, Flags 0) und Inventar nachruecken.
  No → Anzahl auf **1** zuruecksetzen.
* Eingabesperre **16 Frames** nach dem Oeffnen der Box, bevor Bestaetigen greift
  (`8 << (DAT_800dfc1a == 0)` @0x800304C0–0x800304CC → `[0x800E875A]`).
* Freeze waehrend der Abfrage.

Alle Konstanten mit Adresse:
`"This key is useless now."` @0x8009F160 · `"Discard?"` @0x8009F179 ·
`"Yes"` @0x800A1F69 · `"No"` @0x800A1F72 · Layout {2, x=210, Schritt=50} @0x8009DF10 ·
Nachrichten-Id (0x100, 9), Tabelle @0x8009F368[9] = 0x0194, Basis @0x8009EFCC ·
Zaehler `[0x800D4A3D + 4·slot]` · Dekrement @0x80051810 · Abfrage @0x80051834 ·
Yes-Zweig @0x80051774/0x80051794/0x80051798 · No-Zweig @0x800517C4 ·
Auswahl-Nibble `[0x800E873C] & 0x0F` (0 = Yes) @0x80030844 · Vorbelegung 0x80 @0x8002FE88 ·
Eingabesperre `[0x800E875A]` @0x800304CC · 0xFB→Zustand 4 @0x800304BC.

---

## 7. Offen / bewusst nicht behauptet

* Die **physische** Taste hinter `DAT_800ce310 & 0x1000` / `0x800` / `0x400`. Die
  Umlegetabelle 0x800A26A0 und die Aufbauschleife @0x80039178 sind gefunden, aber
  `0x800CE310` wird von einer zweiten Schleife mit der Tabelle 0x800A2700 gefuellt,
  die ich nicht zu Ende verfolgt habe. Nicht geraten.
* Die Bedeutung der zweiten Zeichenkettenfassung (Block B @0x8009ECC4, Glyphen
  0x58–0xEB) — dieselbe `(x & 0x840) == 0x40`-Weiche schaltet Namen, Nachrichten und
  Auswahl-Layouts gemeinsam um. Fuer den englischen Auslieferungsstand irrelevant.
* Card-Key-Tueren („The door is opened with a card key.", Block-A-Index 20/21) laufen
  ueber einen anderen Handler; nicht verfolgt, weil sie die Abfrage nicht ausloesen
  koennen (einzige Einhaengestelle, siehe Abschnitt 5).
