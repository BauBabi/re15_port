# Runde 34 (Granaten) — Gegnerreaktion auf Explosion / Säure / Brand: RE2-KI-Familie

Stand: 2026-09-29, RE-Phase abgeschlossen (nur ermittelt, nichts gebaut, keine Messläufe gegen den Port).
Auftrag: `AUFTRAG.md`. Werkzeuge: `re_gegner_re2_werkzeug/`. Laufzeit-Ausgaben: `build/r34g_gegner/`.
Alle Adressen ohne Zusatz = `info/re2leon/PSX.EXE` (RE2 Retail Leon, t_addr 0x80010000,
`re2_disasm.py`). Overlay-Adressen tragen ihre Datei (`--bin …`). Nachbar-Dossiers derselben Runde:
`re_saeure_brand.md` (Flug/Aufschlag/FX/SE der RE2-Runden), `re_schaden_resolver.md` (RE1.5-Resolver),
`re_wurf_flug_explosion.md` (RE1.5-Routinen 29/30/31).

## Gliederung

0. Einordnung (RE1.5 fertig/unfertig je Mechanismus, Beta->Retail)
1. RE2: Treffer-Zustellung einer GL-Runde (Explosiv 9 / Brand 10 / Säure 11)
2. RE2: Reaktion JE TYP (Zombie-Familie, Zombie Girl, Hund, Krähe, Spinne, Zellenarm, weitere)
3. Port-Abgleich je Typ
4. Tabelle Typ | Port-KI | RE2 HE | Säure | Brand | Schaden | Clip(s) | Port heute | Lücke
5. KONSTANTEN FUER DEN BAU
6. PORT-ABGLEICH
7. OFFEN

## Kurzfassung

* **Zustellung RE2:** alle Treffer einer GL-Runde laufen über den Projektil-Applier FUN_800470C0
  (Gegnerliste 0x800CFE1C, vier Gates + Höhenband, gedrehter Boxtest FUN_80041EF8). Je Rundenart
  mehrere Ereignisse mit eigenem Hitcode: Flug-Kontakt (K0, alle Gegner in der Box), Explosion
  (Zeile 9 K1), Brand-Stoß (Zeile 10 K0, 2× am selben Punkt), Säure-Stoß (Zeile 11 K1), Bodenflammen
  (Zeile 10 K2 je Bild). Einzige Einmal-Sperre: +0x1D3 = 15 Bilder (aus Record-Wort 1). Das
  Explosiv-Geschoss wird fünffach gespawnt (Nachbar-Dossier), jedes mit eigenem Flug und Explosion.
* **Schaden** je Typ aus 0x800A6A88[Typ] + (Zeile−1)·20, Wort 0 = drei 10-Bit-Klammern. K0 tötet
  Zombie (200), Hund (300), Krähe (60) sicher; Brad (250) überlebt jeden Einzeltreffer, die Spinne
  den Explosiv- und Säure-Flug (60/90), kaum aber den Brand-Flug (130 gegen HP 99..132).
* **Reaktion:** Zombie Explosiv-Tod = Zerreißen/Wegschleudern (Spalte 1) bzw. Knockdown-Tod (Spalte 0),
  Brand = Verkohlung + DoT, Säure = Ätzung + Bein wegätzen + DoT; Hund zerplatzt / verkohlt / wird
  geätzt; Krähe GIB; Spinne Zerplatzen+Babys / Farben; Zellenarm unbeeindruckt (Rückzug).
* **Port:** die Reaktionsseite ist weitgehend portiert. Es fehlen die Zustellung (heute Hitscan beim
  Abzug, nur Id 9), Klammer 1/2, Spalte aus dem Prüfpunkt, Richtung aus dem Prüfpunkt, der
  **Zombie-DoT** (im Port als „Kanten-Sturz" fehlgedeutet), die Hund-Tode Zeile 10/11 und der
  Hund-Teile-Wurf. Der geplante RE1.5-Weg (Art 2) liefert der Zombie-Familie Zeile 17 statt der
  belegten Zeile 9 (DAT_8006F430[2] = 9).

---

## 0. Einordnung

| Mechanismus | RE1.5 (Auslieferung) | Ziel |
|---|---|---|
| Treffer-Zustellung Explosiv (Id 9) | vorhanden: Routine 31 @0x8001854c ruft `jal 0x80012d60` @0x800185b8 (Dossier Runde 30 §2.2) | RE1.5 für die Zustellung. Die **Reaktion** läuft aber im Port für die hier behandelten Typen in der RE2-KI (Default-Flavor RE2), und die RE2-KI liest nur RE2-Felder (+0x5 Zeile, +0x1D2 Spalte, +0x1D3 Sperre). Die RE2-KI braucht deshalb den RE2-Stempel, s. §3. |
| Säure (0x0A) / Brand (0x0B) | unfertig: Entlade-Handler 0x80033B58/78 = reine Munitions-Stubs, Spawn hart auf Id 9 (@0x8003368c, Runde 30 §1) | **RE2 Retail** (Beta->Retail-Regel). Belege unten aus RE2. |
| Gegnerreaktion der RE2-KI-Typen | im Port eine bewusste Port-Option (Default RE2, `enemy_ai_re2_zombie.c:70-79`, Nutzer 2026-08-22) | RE2-Verhalten, byte-true zu den RE2-Overlays |

## 1. RE2: Treffer-Zustellung der GL-Runden

### 1.1 Kette (alles RE2-EXE, selbst disassembliert)

Die GL-Runde ist ein Slot des RE2-FX-Systems (Pool, Zeiger auf den laufenden Slot `DAT_800dcbd0`,
Opcode-Tabelle `@0x8009d868`, 96 Einträge; Aufbau: `analysis/konstruktion_2026-08-23/re2-fx-system.md`).
Die drei Rundenarten teilen EINEN Code; die Art steht im Slot-Byte **+0x1B** (`lb a3,27(v1)`):

| +0x1B | Runde | Flug-Hitcode | Explosions-Op (Dispatch `table[step[2] + (+0x1B)]`) |
|---|---|---|---|
| 0 | Explosiv | 0x00030009 | Op 47 @0x80020c3c (setzt `step[1] := 47` @0x80020d14-18) |
| 1 | Brand | 0x0003000A | Op 48 @0x80020f3c (setzt `step[1] := 48` @0x8002100c/1c) |
| 2 | Säure | 0x0003000B | Op 49 @0x800215c8 (setzt `step[1] := 49` @0x800216a8/b0) |

(Die Zuordnung +0x1B → Op 47/48/49 folgt aus dem Dispatch `lbu v0,2(v1)` / `lb v1,27(v1)` /
`addu v0,v0,v1` / `lw v0,-10136(at)` = `0x8009d868[step[2]+(+0x1B)]` @0x8001ef74 + @0x8001f0e4-104.
Das Skript der Runde ist CORE00.ESP Bank 2 Skript 4, Step @Datei 0x192C =
`00 11 2f 2f 00 10 00 10 f6 00 00 0f 00 00 80 02 …` → step[2] = step[3] = 0x2F = **47** (selbst
nachgelesen; gefunden vom Nachbar-Dossier `re_saeure_brand.md` §2.2). Die Art schreibt Op 17 beim
Start: `lbu v0,0(a1)` (0x800CFD06 = Spieler-Item-Id) / `addiu v0,v0,-9` / `sb v0,27(v1)`
@0x8001F1A8-B8. Gespawnt wird die Runde als 0x020C0A00 (Explosiv, 5×, @0x80044BE8-EC ff.) bzw.
0x020C1000 (Brand/Säure, @0x80044F9C-A0) → Unter-Id +0x1E = 0x0C = 12 (Gate von Op 47, s. u.).)

**Flug, Op 0x0F = FUN_8001ed9c** (Tabellenplatz `@0x8009d8a4`):

```
8001ee90  lui  s2,0x3
8001ee9c  ori  s2,s2,0x9            ; s2 = 0x00030009
8001eec8  addiu v0,v0,1000          ; Prüfhöhe 1: y + 1000
8001eed0  lb   a3,27(v1)            ; +0x1B = Rundenart
8001eed4  lh   a1,34(v1)            ; Yaw +0x22
8001eed8  jal  0x800470c0           ; Applier
8001eedc  addu a3,a3,s2             ; Hitcode = 0x30009 + Art
8001eef8  addiu v0,v0,-2000         ; Prüfhöhe 2: y - 1000
8001ef08  jal  0x800470c0           ; derselbe Hitcode
8001ef10  addu s0,s0,v0
8001ef14  bne  s0,zero,0x8001ef38   ; irgendein Treffer -> Explosion
8001ef28  lbu  v0,11(t2)            ; sonst Lebensdauer +0xB (je Bild -1 @0x8001ee64-70)
8001ef30  bne  v0,zero,0x8001ef80   ;   > 0 -> weiterfliegen / abprallen
8001ef44..50  lhu/ori 0x80/sh +0x18 ; Explosion: Status |= 0x80
8001ef58/64/70  sw DAT_800cfb88/8c/90 = Lage (Leser nur im RE2-0x36-Overlay, §7 O5)
8001ef74  lbu v0,2(v1)              ; -> table[step[2] + (+0x1B)]
```

Box des Flugs = `a2` = sp+32, 8 Byte kopiert aus **@0x80010900 = {-1400, 0, 350, 250}**
(`lui a2,0x8001 / addiu a2,a2,2304` @0x8001edb8-bc, `88 fa 00 00 5e 01 fa 00`).

**Explosion Explosiv, Op 47 @0x80020c3c, Phase 0 (`+0x12 == 0`, Sprungtabelle @0x80010928[0] = 0x80020cd0):**

```
80020d30  lbu  v1,30(v1)            ; +0x1E (Unter-Id des Slots)
80020d34  addiu v0,zero,12
80020d38  bne  v1,v0,0x80020dc0     ; != 12 -> NUR SE 0x01140001, KEIN Schaden
80020d40  lui  a0,0x111 / ori 0x1   ; SE 0x01110001
80020d54  lui  a3,0x1002 / ori 0x9  ; Hitcode 0x10020009
80020d68..74  lbu a2,30 / sll 3 / addiu -96 ; Box = sp+32 + (+0x1E*8 - 96) = Box[0]
80020d78  jal  0x800470c0           ; Prüfhöhe y
80020d98  addiu v0,v0,900           ; Prüfhöhe y + 900
80020db0  jal  0x800470c0
```

Boxen @0x80010918 (16 Byte nach sp+32 kopiert @0x80020c58-ca0): **Box[0] = {-2000, 0, 1000, 500}**
(`30 f8 00 00 e8 03 f4 01`), Box[1] = {-1000, 0, 500, 250} (würde bei +0x1E = 13 gewählt, dort läuft aber kein Schadensaufruf).

**Brand, Op 48 @0x80020f3c, Phase 0 (`+0x12 == 0` @0x80020fa0-a8):** SE 0x01120001 (@0x80020fd4/1028).
Boxen @0x8001093c (16 Byte nach sp+32): **{-1200, 0, 600, 300}** und **{-600, 0, 300, 150}**.

```
80021000/14/18  sw pos.x/y/z -> +0x60/+0x64/+0x68   ; Aufschlagpunkt in die Slot-Matrix
80021040  lhu v0,24(v1) / andi 0x80  ; Status-Bit 0x80 (vom Flug gesetzt)
8002104c  bne v0,zero,0x800214e8
80021050  addiu a0,v1,96            ; (Delay-Slot, beide Wege) a0 = Slot+0x60 = Aufschlagpunkt
; Bit 0x80 = 0 (Wand/Boden getroffen):
80021058  lui a3,0x2 / ori a3,0xa   ; Hitcode 0x0002000A, Box sp+32 = {-1200,0,600,300}
80021060  jal 0x800470c0            ; Aufruf 1 am Aufschlagpunkt
80021074..84  lw/addiu 1800/sw 20(sp) ; +1800 NUR auf die gesicherte Kopie sp+20
8002108c  jal 0x800470c0            ; Aufruf 2, a0 = Slot+0x60 (@0x80021090) = DERSELBE Punkt
; Bit 0x80 = 1 (Gegner getroffen oder Lebensdauer um):
800214e8  addiu s0,sp,40            ; Box {-600,0,300,150}
800214f8  jal 0x800470c0            ; Hitcode 0x0002000A, Aufschlagpunkt
80021518  addiu v0,v0,-1800         ; wieder nur sp+20
80021524  jal 0x800470c0            ; a0 = Slot+0x60 (@0x80021528) = derselbe Punkt
80021578..a0  sp+16/20/24 -> +0x60/64/68 ; die veränderte Kopie wandert erst am Ende zurück
```

Der Brand-Stoß prüft also zweimal am SELBEN Punkt. Weil jeder Aufruf nur den ersten ungesperrten
Gegner trifft und ihn sperrt, trifft er bis zu ZWEI verschiedene Gegner (Hinweis des Nachbar-Dossiers
`re_saeure_brand.md` §2.4, selbst nachgelesen).

**Säure, Op 49 @0x800215c8, Phase 0 (Sprungtabelle @0x80010950[0] = 0x80021678):** SE 0x01130001
(@0x80021678-7c / jal @0x800216ac). Dieselben Boxen @0x8001093c.

```
800216c8  andi v0,v0,0x80           ; Status-Bit 0x80
800216cc  bne  v0,zero,0x800216dc   ; 1 -> s0 = sp+40 = {-600,0,300,150}
800216d8  addiu s0,sp,32            ; 0 -> {-1200,0,600,300}
800216e4  lui  a3,0x1002 / ori 0xb  ; Hitcode 0x1002000B
800216ec  jal  0x800470c0           ; Prüfhöhe y
8002170c  addiu v0,v0,1800          ; y + 1800 (in BEIDEN Fällen)
80021718  jal  0x800470c0
```

**Brand-Nachbrenner, Op 40 = FUN_80020758** (Tabellenplatz `@0x8009d908`), läuft JEDES Bild als Opcode A:

```
8002076c  addiu a2,a2,2320          ; Box @0x80010910 = {-600, 0, 300, 150}
80020794  lui a3,0x2002 / ori 0xa   ; Hitcode 0x2002000A
800207a4  addiu v0,v0,-100          ; Prüfhöhe y - 100
800207bc  jal 0x800470c0
800207c4  beq v0,zero,...           ; Treffer -> jal 0x80021970 (Op 50)
```

Op 50 (@0x80021970-B0): `step[2] := 64`, acc.x := 0, `step[3] := 0`, vel.x := 0, Status &= ~2
(Physik aus). Die Flamme bleibt liegen; Op A bleibt 19 und ruft Op 40 weiter. Op 40 läuft nicht
selbst als Opcode, sondern aus **Op 19 = 0x8001F2C0** (Bodenflamme): `lh v0,74(v1)` (+0x4A != 0)
@0x8001F2D0-D8, `lhu v0,22(v1)` / `sltiu v0,v0,0x10` (step[0x16] ≥ 16) @0x8001F2E0-EC,
`lhu v0,4(v1)` / `sltiu v0,v0,0x1001` (X-Aspekt > 0x1000) @0x8001F2F4-300 → `jal 0x80020758`
@0x8001F308; step[0x16]++ @0x8001F31C-28. Die Bodenflammen sind CORE00.ESP Bank 5 Skript 5 (Spawn
0x0505xxxx in Op 48 @0x80021110-6C, nur im Wand/Boden-Zweig) und deren Folgeflammen Bank 5 Skript 4
(Op 30 → Op A 19); Lebensdauer und Wachs-/Schrumpf-Zähler stehen im Nachbar-Dossier
`re_saeure_brand.md` §2.5.

### 1.2 Der Applier FUN_800470C0 (Volldisasm `re_gegner_re2_werkzeug/re2_800470c0.dis`)

Signatur: `a0` = Punkt {x,y,z} (s32), `a1` = Yaw, `a2` = Box {p0,p1,p2,p3} (s16), `a3` = Hitcode.
Hitcode-Felder: `& 0xFF` = Zeile (Attacken-Id) -> `+0x5`; `>>28` = Klammer (Spalte des
Schadenswortes, `srl s6,s5,28` @0x80047114); Bit 0x10000 = ALLE Gegner in der Box treffen
(sonst nur den ersten); Bit 0x20000 = Zonen-Rechnung Beine/Kopf; Bit 0x40000 = Sonderfall
Zone 0 bei gleicher Etage (nur im Einzelpfad @0x80047538-60; bei keiner GL-Runde gesetzt).

| Hitcode | Rolle | Zeile | Klammer | Alle/erster | Zonen |
|---|---|---|---|---|---|
| 0x00030009 / 0A / 0B | Flug-Kontakt | 9 / 10 / 11 | 0 | **alle** (0x10000) | ja (0x20000) |
| 0x10020009 | Explosion (Op 47) | 9 | **1** | erster | ja |
| 0x0002000A | Brand-Stoß (Op 48), 2× am selben Punkt | 10 | 0 | erster | ja |
| 0x1002000B | Säure-Stoß (Op 49) | 11 | **1** | erster | ja |
| 0x2002000A | Brand-Nachbrenner (Op 40, je Bild aus Op 19) | 10 | **2** | erster | ja |

**Gegnerliste und Filter** (vier Gates + Höhenband, in dieser Reihenfolge):

```
800470fc  addiu s7,s7,-15896        ; s7 = 0x800CC1E8
80047118  addiu s2,s7,15412         ; s2 = 0x800CFE1C = Zeigerliste der Gegner
8004711c  lw   s0,0(s2)
80047124..30  lw v0,0(s0) / andi 0x1 / beq -> weiter   ; Gate 1: aktiv (Wort0 Bit 0)
80047138..40  lbu v0,467(s0) / bne -> weiter           ; Gate 2: +0x1D3 != 0 (GANZES Byte)
80047148..50  lh v0,342(s0) / bltz -> weiter           ; Gate 3: HP (+0x156) < 0
80047158..64  lhu v0,270(s0) / andi 0xc000 / bne       ; Gate 4: +0x10E & 0xC000
8004716c..84  lhu v1,464 / andi 0xff00 / sh 464        ; +0x1D0 &= 0xFF00 (Richtungsbits löschen)
80047170..a4  Höhenband: (Y+0x3C + h(+0x98,s16) + 100 + d(+0x9E,u16) - P.y) <u 2*(d+100)
80047410  lw   v0,8524(s7)          ; Listenende = *(0x800CE334)
```

Höhenband: der Prüfpunkt P.y muss in `(Y + h - d - 100, Y + h + d + 100]` liegen.

**Boxtest FUN_80041EF8** (Volldisasm `re2_80041ef8.dis`): vorher wird die Box um den Radius des
Gegners erweitert, `p2 += (s16)(+0x1EE) >> 2` und `p3 += (s16)(+0x1EE) >> 2`
(`lhu v1,494(s0)` / `sll 16` / `sra 18` @0x800471bc-ec). Rechnung in FUN_80041EF8:
Ecke = P + R(Yaw)·(p0, 0, -p1-4·p3) (`sw v0,84(sp)` = p0 @0x80041f34, `subu v1,zero,v1` /
`sll a3,a3,2` / `subu v1,v1,a3` @0x80041f68-70), Kanten R·(p2,0,0) und R·(0,0,2·p3)
(@0x80041fcc-dc und @0x80041fe8-ffc) im Viertel-Raster (Ecke und Ziel `>>2` @0x80041fbc-c0,
@0x80042054-bc). In Weltmaß heißt das, im Yaw-Rahmen des Prüfpunkts:

| Box | Bytes | x-Bereich | z-Bereich |
|---|---|---|---|
| Flug @0x80010900 | `88 fa 00 00 5e 01 fa 00` | [-1400, 0] | [-1000, +1000] |
| Explosion @0x80010918 | `30 f8 00 00 e8 03 f4 01` | [-2000, +2000] | [-2000, +2000] |
| Brand/Säure Wand @0x8001093c | `50 fb 00 00 58 02 2c 01` | [-1200, +1200] | [-1200, +1200] |
| Brand/Säure Gegner @0x80010944 | `a8 fd 00 00 2c 01 96 00` | [-600, +600] | [-600, +600] |
| Nachbrenner @0x80010910 | `a8 fd 00 00 2c 01 96 00` | [-600, +600] | [-600, +600] |

Mit Gegnerradius r = +0x1EE wird daraus x ∈ [p0, p0 + 4·p2 + r], z ∈ [-4·p3 - r, 4·p3 + r].
Es gibt also KEINEN Kreisradius, sondern ein gedrehtes Rechteck. ⚠ Nach einem Treffer im
Alle-Modus wird die Erweiterung NICHT zurückgenommen (Rücknahme nur im Nicht-Treffer-Zweig
@0x800473dc-408). Der nächste Gegner in derselben Liste prüft gegen die schon erweiterte Box.

**Was ein Treffer schreibt** (Alle-Zweig @0x800471f8-3d8, Einzelzweig @0x80047434-628, gleich bis auf
zwei Stellen):

```
80047200  ori  v0,v0,0x1 / sh 464   ; +0x1D0 |= 1 (getroffen)
8004722c  lw   a1,27272(at)         ; Record-Basis 0x800A6A88[Typ(+0x8)]
8004723c  addiu v0,v0,-20           ; + (Zeile-1)*20
8004724c..5c  w0 >> (10*Klammer) & 0x3FF
80047260..68  HP(+0x156) -= Schaden
8004726c..78  +0x1FC := altes Zustandswort, außer es war 0xC02 (Einzelzweig: 0xC02/0xC03, @0x8004748c-9c)
80047288  sw   v0(=2),4(s1)         ; +0x4 WORT = 2 (HURT) — nullt +0x5/+0x6/+0x7
80047290  sw   v0(=3),4(s1)         ; bei HP < 0: 3 (DEATH)
80047298  sb   1,466(s1)            ; +0x1D2 = 1 (Rumpf)
800472a4..d4  Bit 0x20000 && Y + h/2 < P.y      -> +0x1D2 = 0 (Beine)
800472d8..30c Wort0 & 0x10000000 && P.y < Y + 3·h/2 -> +0x1D2 = 2 (Kopf)
80047324  sb   s5,5(s1)             ; +0x5 = Hitcode & 0xFF = ZEILE 9/10/11
80047330  sb   v0,466(s1)           ; +0x1D2 += 3*Klammer
8004732c..4c  +0x1D3 = (+0x1D3 & 0x80) | ((w1 >> 9) & 0x7F)   ; TREFFERSPERRE
80047358  jal  0x800154ac           ; Winkel P -> Gegner
80047360..3d8 a = Winkel - Yaw(+0x76):
              (a+0x400)&0xFFF < 0x800 -> +0x1D0 |= 0x20
              (a+0x600)&0xFFF < 0x400 -> +0x1D0 |= 0x40
              (a-0x200)&0xFFF < 0x400 -> +0x1D0 |= 0x80
```

**Einmal-Sperre.** Das einzige Gate gegen Doppeltreffer ist +0x1D3 (Gate 2). Der Applier setzt die
unteren 7 Bit auf `(w1 >> 9) & 0x7F` der Zeile; für ALLE Gegnertypen und alle drei GL-Zeilen ist
das **15** (w1 = 0x078F1E0A bzw. 0x078F1FB4/1F68 bei der Spinne, s. §1.3). Der Gegner-Root zählt
die unteren 7 Bit je Bild herunter, selbst nachgelesen (`lbu v1,467` / `andi v0,v1,0x7f` / `beq` /
`addiu v0,v1,-1` / `sb v0,467`): Zombie EMZ0.BIN @0x80100484-98, Hund EMD0G_MOD0.BIN @0x80100028-3C,
Krähe EMOVL21_S0.BIN @0x80100160-74, Spinne EMS25.BIN @0x801000EC-100. Der Zellenarm hat KEIN
Dekrement (nur `sb zero,467` im INIT @0x80100130 und `ori 0x80` in HURT/DEATH) — er bleibt nach einem
Treffer gesperrt, bis ein Zustand ihn freigibt (Port `enemy_ai_re2_zellenarm.c:318`).
Folgen:
* Die zwei Prüfhöhen eines Aufrufs (y±1000 usw.) treffen denselben Gegner nie doppelt.
* Der Gegner, den der FLUG getroffen hat, trägt im selben Bild 15 und ist für die Explosion (Op 47
  läuft im selben Bild über den Dispatch @0x8001f104) KEIN Kandidat. Die Explosion trifft den
  ersten ANDEREN ungesperrten Gegner in ihrer Box, je Prüfhöhe höchstens einen.
* Der Nachbrenner (Op 40 aus Op 19) kann einen Gegner höchstens alle 15 Bilder treffen. Ein Treffer
  hält nur das Gleiten der Flamme an (Op 50); die Flamme schadet weiter, solange Op 19 läuft.
* Der Brand-Stoß (Op 48) ruft zweimal am selben Punkt: bis zu zwei verschiedene Gegner.
* Das Explosiv-Geschoss wird fünfmal gespawnt (5 × 0x020C0A00, Nachbar-Dossier §2.2); jede der fünf
  Runden hat ihren eigenen Flug-Kontakt und ihre eigene Explosion. Mehrfachschaden an EINEM Gegner
  verhindert nur die 15-Bild-Sperre.

### 1.3 Schaden je Typ (Records `0x800A6A88[Typ] + (Zeile-1)*20`)

Werkzeug `re_gegner_re2_werkzeug/re2_gl_records.py`, Ausgabe `build/r34g_gegner/re2_gl_records.txt`.
w0 = drei 10-Bit-Schäden (Klammer 0/1/2), w1 bit 9..15 = Sperre.

| Typ(en) | Basis (Zeiger @) | Zeile 9 Explosiv K0/K1/K2 | Zeile 10 Brand | Zeile 11 Säure | Sperre |
|---|---|---|---|---|---|
| 0x10 0x11 0x12 0x13 0x14 0x18..0x1F 0x2C | 0x800A412C (@0x800A6AC8 ff.) | 200/50/10 `c8 c8 a0 00` @0x800A41CC | 200/50/5 `c8 c8 50 00` @0x800A41E0 | 200/50/10 `c8 c8 a0 00` @0x800A41F4 | 15 |
| 0x15 0x16 0x17 | 0x800A42A8 (@0x800A6AE0) | 80/50/10 `50 c8 a0 00` @0x800A4348 | 80/50/5 @0x800A435C | 200/50/10 @0x800A4370 | 15 |
| 0x20 Hund | 0x800A4424 (@0x800A6B08) | 300/50/10 `2c c9 a0 00` @0x800A44C4 | 300/50/5 @0x800A44D8 | 300/50/10 @0x800A44EC | 15 |
| 0x21 Krähe | 0x800A45A0 (@0x800A6B0C) | 60/60/15 `3c f0 f0 00` @0x800A4640 | 60/60/15 @0x800A4654 | 60/60/15 @0x800A4668 | 15 |
| 0x25 0x26 Spinnen | 0x800A4B90 (@0x800A6B1C/20) | 60/60/20 `3c f0 40 01` @0x800A4C30 | 130/130/5 `82 08 52 00` @0x800A4C44 | 90/60/10 `5a f0 a0 00` @0x800A4C58 | 15 |
| 0x2D Zellenarm | 0x800A5180 (@0x800A6B3C) | 60/60/20 @0x800A5220 | 60/60/10 @0x800A5234 | 60/60/10 @0x800A5248 | 15 |

Die übrigen RE2-Typen (Licker, Alligator, G, Mr.X, Motte …) stehen vollständig in der Ausgabedatei.
Schaden je Ereignis = Zeile × Klammer aus §1.2: Flug 9/10/11-K0, Explosion 9-K1, Brand-Stoß 10-K0,
Säure-Stoß 11-K1, Nachbrenner 10-K2. Beispiel Zombie 0x10: Flug 200, Explosion 50, Brand-Stoß 200,
Säure-Stoß 50, Nachbrenner 5.

### 1.4 Überleben oder Tod — die Zeile allein entscheidet nicht

Der Applier schreibt `+0x4 = 3` bei HP < 0, sonst 2 (@0x80047288-90). RE2-Start-HP (Port-Tabellen mit
Adresse, `re15_damage.c:893-898`): Zombie 50..128 (+15 bei < 4 Gegnern, @0x8010C670 EMOVL10_S0.BIN),
Brad 0x11 fest 250 (@0x801008C8), Hund 69..129 + (rand&3) (@0x801053B0), Krähe 10 (@0x80100324),
Spinne 99..129 + (rand&3) (@0x80106334). Daraus (Klammer-0-Treffer tötet, Klammer 1/2 meist nicht):

| Ereignis | Zombie 0x10-0x13/0x18 | Brad 0x11 | Zombie 0x16 | Hund | Krähe | Spinne 0x25 |
|---|---|---|---|---|---|---|
| Flug Explosiv K0 | 200 = Tod | 250-200 = HURT | 80 = HURT oder Tod | 300 = Tod | 60 = Tod | 60 = HURT |
| Flug Brand K0 | 200 = Tod | HURT | 80 = HURT oder Tod | 300 = Tod | Tod | 130 = fast immer Tod |
| Flug Säure K0 | 200 = Tod | HURT | 200 = Tod | 300 = Tod | Tod | 90 = HURT |
| Explosion 9-K1 | 50 = HURT (HP ≤ 49: Tod) | HURT | 50 | 50 = HURT | 60 = Tod | 60 = HURT |
| Brand-Stoß 10-K0 | 200 = Tod | HURT | 80 | 300 = Tod | Tod | 130 |
| Säure-Stoß 11-K1 | 50 | HURT | 50 | 50 = HURT | 60 = Tod | 60 = HURT |
| Nachbrenner 10-K2 | 5 | 5 | 5 | 5 | 15 = Tod | 5 |

## 2. RE2: Reaktion je Typ auf die Zeilen 9 / 10 / 11

### 2.1 Zombie-Familie (EMZ0.BIN == EMOVL10_S0.BIN, geladen @0x80100000)

**Dispatch** (selbst gedumpt, `re2_disasm.py table … --bin EMZ0.BIN`):

HURT-Wurzel FUN_80104F40 → 2D-Tabelle `0x8010C940 + Zeile*36 + Spalte*4` (Dispatch @0x801053E0-410,
`lbu v1,5(a0)` / `lbu v1,466(a0)` / `jalr v0` @0x80105410). DEATH-Wurzel FUN_80108250 →
`0x8010CC24 + Zeile*36 + Spalte*4` (@0x801084E0-518). Spalte = +0x1D2 = Zone + 3·Klammer.

| Zeile | Tabelle | Sp.0 Beine K0 | Sp.1 Rumpf K0 | Sp.2 Kopf K0 | Sp.3-5 (K1) | Sp.6-8 (K2) |
|---|---|---|---|---|---|---|
| 9 HURT | @0x8010CA84 | 0x80107438 | 0x80105BC0 | 0x80105BC0 | 0x80105438 | 0x80105438 |
| 10 HURT | @0x8010CAA8 | 0x80105BC0 | 0x80105BC0 | 0x80105BC0 | 0x80105438 | 0x80105438 |
| 11 HURT | @0x8010CACC | 0x80105BC0 | 0x80105BC0 | 0x80105BC0 | 0x80105438 | 0x80105438 |
| 9 DEATH | @0x8010CD68 | 0x80107438 | **0x80108BEC** | **0x80108BEC** | 0x80108530 | 0x80108530 |
| 10 DEATH | @0x8010CD8C | 0x80108530 | 0x80108530 | 0x80108530 | 0x80108530 | 0x80108530 |
| 11 DEATH | @0x8010CDB0 | 0x80108530 | 0x80108530 | 0x80108530 | 0x80108530 | 0x80108530 |

⚠ Zwei Namensfallen: (1) Die DEATH-Tabelle beginnt @0x8010CC24 (`lui a2,0x8011 / addiu a2,a2,-13276`
@0x801084E8-EC); @0x8010CD20 ist ihre Zeile 7 (Schrot), nicht die Basis. (2) Das Kürzel „BURN" neben
0x80107EF0 in `enemy_ai_re2_zombie.c:3791/3807` ist KEIN Brand-Handler — 0x80107EF0 ist der leichte
Taumel-Treffer der Dauerfeuer-Zeilen 15/18 (`:5016`). Brand (Zeile 10) = STAGGER bzw. Sturz-Tod plus
Verkohlung FUN_80106128.

Handler-Namen wie im Port (`enemy_ai_re2_zombie.c:3634-3667`, `:7183-7200`): 0x80105438 = MAIN
(Zucken), 0x80105BC0 = STAGGER (Taumeln), 0x80107438 = Knockdown (Bein-Treffer, im DEATH mit
`+0x4==3`-Zweig), 0x80108530 = normaler Sturz-Tod (Clip 1/2), **0x80108BEC = ZERREISSEN** (Kopf ab +
Bein(e) ab, Phasentabelle @0x8010012C). Die Kopf-Spalte 2 ist beim stehenden Zombie original
unerreichbar (INIT setzt nur Beine+Rumpf in Wort0, `enemy_ai_re2_zombie.c` Block „KORRIGIERT
2026-09-12"). Ergebnis Explosiv-Flugtreffer in den Rumpf = **Zombie wird zerrissen**; in die Beine
= Knockdown-Handler mit Todeszweig. Brand/Säure-Tod = normaler Sturz.

Clips (Aktionsbank Paar 2 des Zombies; Port-Zitate mit Original-Adresse):
* Sturz-Tod 0x80108530: Clip 1 ab Bild 0 (rückwärts) bzw. Clip 2 ab Bild 10 (vorwärts), Rate 15
  (Wort `((+0x16A*5)<<9) + 0xF0000 + {1,2}[+0x16A]`, @0x8010855C ff., Port `:7489-7525`).
* Zerreißen 0x80108BEC: P0 Clip 4 (Front) / 3 (Rücken), Rate 3, Blend 1024 (@0x80108D74-A4, Port `:5751`);
  Kopf (Part 8) ab @0x80108DD4 ff.; später Clip {2,1}[Rücken] (sp+32/33 @0x80108C24-30, Port `:5686`) ab Bild Rücken·15+10, Rate 7
  (@0x8010918C-A8, Port `:5911`). Ausstieg 2 (nur Zeile 9, 3 von 4 Würfen) → Wegschleudern 0x80109610.
* Wegschleudern 0x80109610: Clip 1/2 ab Bild 15, Rate 15 (`lui a0,0xf / ori a0,a0,0xf00` @0x80109840-64,
  Port `:5478`).
* Knockdown 0x80107438 (Todeszweig): Clip 1, Rate 15 (Wort 0xF0001 @0x801074C8, Port `:6522`).
* Taumeln 0x80105BC0: Clip 4 (Rücken-Treffer 3), Rate 3 (@0x80105C38-74).

**Element-Leiter je Zeile** (Scan `re_gegner_re2_werkzeug/zeilen_vergleich_scan.py`, Ausgabe
`build/r34g_gegner/scan_emz0.txt`; jede Fundstelle einzeln nachgelesen). Die Leiter steht NICHT in
jedem Handler:
* Zeilen 9 + 10 + 11: HURT-Liegend-Zweig @0x80105188-208, DEATH-Liegend @0x80108444-4B8, DEATH-MAIN
  0x80108530 @0x801086E4-7C0, Kriecher-Tod @0x80108B00-B88, Kriecher-Tod 2 @0x80109B90-C54.
* nur Zeile 9 (Ruß, gemeinsam mit 17): ZERREISSEN 0x80108BEC @0x80108CC0-CEC.
* nur Zeilen 10 + 11: STAGGER 0x80105BC0 P0 @0x80105DC4-F18, Kriecher-Treffer 0x80107888 @0x80107960-9B0,
  Aufsteh-Treffer 0x80107A78 @0x80107BE0-CE0.
* KEINE 9/10/11-Leiter: MAIN 0x80105438 (dort nur Zeile 16 @0x80105510-78 und 14 @0x80105724) und
  Knockdown 0x80107438 (nur Zeile 12 @0x801074FC). Ein Zombie, der eine Explosion oder einen
  Säure-Stoß (Klammer 1 → Spalten 3..5 → MAIN) überlebt, bekommt also weder Ruß noch Ätzung noch DoT.

Beispiel HURT-Liegend-Zweig:

```
80105188  lbu v1,5(s1) / addiu v0,zero,10 / bne       ; Zeile 10 (Brand)
8010519c..a8  lhu 270 / andi 0x80 / bne               ;   nur wenn +0x10E Bit 0x80 noch frei
801051b0  jal 0x80106128                              ;   VERKOHLUNG (setzt +0x10E |= 0x80 @0x8010613C-48)
801051b8..c4  lhu 538 / ori 0x800 / sh                ;   +0x21A |= 0x800 (Brand brennt)
801051c8  lbu v1,5(s1) / addiu v0,zero,11 / bne       ; Zeile 11 (Säure)
801051d8  jal 0x80106310                              ;   ÄTZUNG (setzt +0x21A |= 0x1800 @0x8010632C-38)
801051e0  lbu v1,5(s1) / addiu v0,zero,9 / bne        ; Zeile 9 (Explosiv)
801051f0..fc  lhu 270 / andi 0x80 / bne               ;   nur wenn +0x10E Bit 0x80 frei
80105204  jal 0x8010640c                              ;   SPRENG-RUSS
```

Die drei Funktionen (Port hat sie mit allen Adressen: `re2z_gore_burn` `:4668-4703`,
`re2z_gore_acid` `:4705-4720`, `re2z_gore_soot` `:4722-4738`):
* FUN_80106128 Verkohlung: `+0x10E |= 0x80`, 3 Feuer-Emitter (RE2-FX 0x05032710/0x05031388/0x050313E8
  an Part 0/8/3 bzw. 0/3/6, Münzwurf @0x80106160), 11 Part-Farbwörter 0x404040..0x707070 (@0x8010627C-2F4).
* FUN_80106310 Ätzung: `+0x21A |= 0x1800`, 7 Part-Farbwörter 0x304040..0x506060 (@0x8010633C-384),
  Säure-Spray 0x040F1770/0x040F0FA0 an Part 0/12/3.
* FUN_8010640C Ruß: 2 Emitter 0x05032710/0x050313E8, 10 Part-Farbwörter (@0x8010648C-4F8).

**Brand/Säure über Zeit (DoT)** — NEU belegt, im Port FEHLEND (s. §3). Im Gang-Executor und im
Anrempel-Executor steht derselbe Block (EMZ0.BIN):

```
80101dc0  lhu v0,270(s1) / andi 0x80 / bne -> 80101de8   ; verkohlt (+0x10E Bit 0x80)
80101dd4  lhu v0,538(s1) / andi 0x1000 / beq -> 80101ec4 ; ODER geätzt (+0x21A Bit 0x1000)
80101de8  lhu v0,566(s1) / andi 0x7 / bne -> 80101e94    ; nur jedes 8. Bild (+0x236 & 7 == 0)
80101dfc  lhu v0,538(s1) / andi 0x800 / beq -> 80101e94  ; und +0x21A Bit 0x800 (brennt/ätzt)
80101e10  jal rand ; 80101e18 jal rand ; andi 3 ; srav ; andi 1 ; sll 7   ; j = ((r1 >> (r2&3)) & 1) * 128
80101e30..50  +0x76 (Yaw) = Yaw + 64 - j                 ; Zucken ±64
80101e34..44  lhu 342 / addiu -1 / sh 342                 ; HP -= 1
80101e4c  bgez -> 80101e94                               ; HP < 0: Tod durch Brand/Säure
80101e58/64  addiu v1,zero,2563 / sw v1,4(s1)             ; +0x4-Wort = 0x0A03 (DEATH, Zeile 10)
80101e5c..6c  +0x21A & 0x1000 -> sw 2819,4(s1)            ; = 0x0B03 (DEATH, Zeile 11) bei Säure
80101e70/74  addiu v0,zero,4 / sb v0,466(s1)              ; +0x1D2 = 4 (Rumpf, Klammer 1)
80101e78..90  +0x1D3 |= 0x80 ; +0x21A |= 0x2000           ; Latch „am Brand gestorben"
80101e94..c0  +0x21A & 0x1000: Yaw += 64 - (rand >> 1)    ; Säure: Zucken JEDES Bild
```

Anrempel-Executor gleichartig @0x80102474-54C (Brand-Tick @0x8010249C-518 ohne das ±64-Zucken,
Säure-Zucken @0x80102520-4C). Zähler +0x236 zählt der Root jedes Bild hoch (`lhu v0,566(s0) /
addiu v0,v0,1 / sh v0,566(s0)` @0x801004F8-508), INIT nullt ihn (`sh zero,566` @0x801008AC).
Also: **1 HP je 8 Bilder, solange der Zombie geht oder anrempelt**, nach Brand-Treffer (HURT setzt
0x800) oder Säure-Treffer (0x1800 setzt beide Bits). Stirbt er daran: DEATH-Zelle
`0x8010CC24 + 10*36 + 4*4` = 0x80108530 (Sturz). `+0x21A & 0x2000` liest der Sturz-Tod
(@0x801085AC-C0: Seite `+0x16A` wird dann 0 statt Münzwurf).

Weitere Leser des Brand-Bits: halbe Liegezeit im Knockdown (@0x80103494-D8, im Port `:2172-2177`),
Fett-Takt im Gang/Anrempeln (`+0x10E&0x80 || +0x21A&0x8000` @0x80101CD8-D5C / @0x801023F8-468, im Port
`re2z_fat_cadence_tick` `:1412`), Leichen-Farbausblender (@0x8010A810-868, jedes 4. Bild alle
Part-Farben -0x010101; im Port OFFEN `:7712`).

### 2.2 Hund 0x20 (EMD0G_MOD0.BIN)

HP 69..132 < 300 → jeder Klammer-0-Treffer tötet. DEATH-Wurzel 0x801040DC → `0x801055CC[+0x5]`
(`lbu v0,5(a0)` / `lw v0,21964(at)` @0x801040E4-F8). Zeile 9 → 0x80104610 (eigener Phasen-Dispatch
@0x80105688, P0 = 0x80104694), Zeilen 10/11 → 0x80104118 (Router: `+0x6 == 0` → `0x80105618[+0x5]`,
sonst `0x80105668[+0x6]`).

| Zeile | P0-Handler | Was er tut (selbst disassembliert, `emd0g_death_80104610.dis`) |
|---|---|---|
| 9 Explosiv | 0x80104694 | `+0x1D2 >= 3` (Klammer ≥ 1): nur Kern 0x80104178 (Todesschrei SE 7) @0x801046A8-CC. Sonst (Klammer 0): `+0x231 := 1` (Schrei stumm) @0x801046E8, Kern, **Teile-Wurf 0x80104440** (7 Parts aus @0x80105680 = {2,3,4,7,8,9,10}: Flags `|= 0x4A`, +0x9C=800, +0x9A=-150, +0x9E=10, +0xA4=-100, Farbe 0x00101040 @0x80104440-4D8), FX 7 an einem Zufalls-Part (nur wenn dessen Flags & 0x4A == 0) @0x80104704-50, dann `+0x21F := 18`. Jedes Bild danach: FX an Part 3 und 2 (`jal 0x80105070` @0x8010464C/5C/78). = **Hund zerplatzt**. |
| 10 Brand | 0x80104774 | Kern; bei `+0x1D2 < 3` (oder Zeile 16): `+0x21F := 6`, 6× FX 7 an Zufalls-Parts (@0x801047B0-D4), alle 17 Part-Farben `+0x70 := 0x00202020` (@0x801047DC-800) = **verkohlt** |
| 11 Säure | 0x8010481C | Kern; bei `+0x1D2 < 3`: alle 17 Part-Farben `:= 0x00003F2F` (`addiu a1,zero,16175` @0x80104844-64), `+0x21F := 2`, FX 9 an Part (rand&7), FX 10 an Part ((rand&7)|8) @0x80104868-9C = **geätzt** |

FX-Tabelle @0x801056AC (6 Byte je Eintrag, `bytes 0x801056ac 84`): FX 7 = {0x85, 3, 0, 0x1000},
FX 8 = {0x85, 4, 0, 0x1000}, FX 9 = {0x84, 0x0F, 0, 0x1400}, FX 10 = {0x84, 0x0F, 0, 0x0C00}.
Bit 0x80 der Art = am Part angeheftet (`andi 0x80` @0x80105128, a2 = Part+72).

HURT (nur Explosion 9-K1 / Säure 11-K1 / Nachbrenner 10-K2 lassen ihn leben), Tabelle @0x80105538,
`emd0g_hurt_rows_80103ce4.dis`:
* Zeile 9 → 0x80103CE4: `+0x21F = 2 - +0x1D2/3` (@0x80103D00-28), so oft FX 8; `+0x5 := 1`; generischer HURT 0x80103308.
* Zeile 10 → 0x80103D9C: `+0x21F = 4 - +0x1D2/3`, FX 8; `+0x5 := 1`; **`+0x1D3 |= 0x80`** (@0x80103E28-3C); 0x80103308.
* Zeile 11 → 0x80103E60: EIN Zufalls-Part (rand&0xF) bekommt Farbe 0x00003F2F (@0x80103EB0-C0), `+0x21F := 1`, FX 9 dort; `+0x5 := 1`; 0x80103308.

### 2.3 Krähe 0x21 (EMOVL21_S0.BIN)

Zustandstabelle @0x80104908: `[2] = [3] = 0x801028BC` — HURT und DEATH teilen die Wurzel. Zeilen-Dispatch
`0x80104A18[+0x5]` (`lbu v0,5(a0)` / `lw v0,18968(at)` @0x801028F4-908): Zeilen 9/10/11 (und 5/6/17)
→ **0x80102CA0 = GIB** (@0x80104A3C/40/44). HP 10, jeder GL-Schaden ≥ 15 tötet. Port: `re2c_hurt`
leitet 9/10/11 bereits auf `re2c_hurt_gib` (`enemy_ai_re2_crow.c:1486-1500`, Handler `:1317-1339`).

### 2.4 Spinne 0x25 / Baby 0x26 (EMS25.BIN / EMS26.BIN)

Tabellen selbst gedumpt (`table … --bin EMS25.BIN`): HURT `0x80106518[+0x5]` (Dispatch @0x80102CD8),
DEATH `0x801065A8[+0x5]` (`lbu v0,5(s0)` / `lw v0,26024(at)` / `jalr` @0x80103CE0-FC; Eintrag [0] sind
die Datenbytes {0,1,2,0x13}).

| Zeile | HURT | DEATH |
|---|---|---|
| 9 | 0x80103800: `+0x5 := 7` (@0x8010380C), Sonderzeile 0x80102FDC, dann `2 - (+0x1D2/3)` Blut-Emitter an Zufalls-Beinen (@0x80103820-C0) | 0x8010493C: 1 + 3×2 Gore-Salven, dann Zerplatz-Zeile 0x801044D0 (Clip 12, **6..9 Babys** `FUN_80105D38(self,0x2002,(rand&3)+6)` @0x80104590-A4) |
| 10 | 0x801038E4: 5 Blut-Emitter (@0x80103914-68), `+0x5 := 2`, generische Zeile | 0x80104A5C: 1 + 4×2 Gore-Salven, **alle 20 Part-Farben 0x00202F2F** (`lui a1,0x20 / ori 0x2f2f / jal 0x8010609c` @0x80104B44-4C), generischer Tod |
| 11 | 0x801039AC: **Bein abschießen** (k = rand&7, `+0x220 |= 1<<k`, `+0x221--` @0x801039CC-A3C), FX (2k+3, 7), `+0x5 := 2`, generische Zeile | 0x80104B88: **alle 20 Part-Farben 0x00101F3F** (`lui a1,0x10 / ori 0x1f3f` @0x80104BC0-C4, `jal 0x8010609c` @0x80104BF0), Part 19 fliegt (Flags |= 0x10, +0x9C..9E = 100/100/90 @0x80104BC8-F4), FX (19,7) (0,6) (1,6), `+0x239 := 1` |

FUN_8010609C (@0x8010609C-C4) schreibt `a1` in `+0x70` aller 20 Parts (Stride 172). Port: alle Zeilen
verdrahtet (`enemy_ai_re2_spider.c:1957-2003`, `:2258-2296`), die Mesh-Farben/Part-Flug sind dort als
OPEN markiert. ⚠ Der Port-Kommentar `:2285` nennt Zeile 11 „verkohlt"; Zeile 11 ist nach der
Item-Tabelle RE2-**Säure** (Zeile 10 = Brand), nur die Beschriftung ist vertauscht.

### 2.5 Zellenarm (RE2-Typ 0x2D, `build/extracted/re2_ems/CDEMD0_EM2D_ai1.BIN`, `RE_OVERLAY_DIR`)

HURT 0x80100F50 und DEATH 0x80100FE0 dispatchen NICHT über die Zeile, sondern über die Spalte
(`lbu v0,466(a0)` @0x80100F58 → `0x801014CC[+0x1D2]`; DEATH → `0x801014F0[+0x1D2]`). Alle erreichbaren
Spalten zeigen auf denselben Zweig (0x80100F8C bzw. 0x8010101C); Spalte 2/5/8 = NULL, aber der
INIT setzt nur Wort0 |= 0x0C000000 (`lui v1,0xc00` / `or` / `sw v0,0(s0)` @0x80100350-374) — Kopf-Zone
unmöglich. +0x1EE = 800 (`sh v1,494(s0)` @0x8010034C). HP 250, DEATH füllt wieder auf 250 und zieht
zurück (0x501) — der Arm ist unsterblich (Port `enemy_ai_re2_zellenarm.c:508-529`). **GL-Zeilen 9/10/11
ändern am Arm nichts**: jeder Treffer = Rückzug + SE 2/3. Schaden je GL-Ereignis 60/60/20 (Zeile 9),
60/60/10 (10, 11) aus @0x800A5220/34/48 — nur fürs 250er-Konto.

### 2.6 Weitere Typen im Port auf RE2-KI

`re15_re2_owns_type` (`enemy_ai_re2_dog.c:75-97`) = Zombie-Familie (0x10/0x11/0x12/0x13/0x16/0x18,
`enemy_ai_re2_zombie.c:167-171`) + 0x20 + 0x21 + 0x25 + 0x26 (nur echte RE2-Babys,
`re15_re2spider_baby_owns`). Dazu der Zellenarm 0x1A über `re15_re2arm_owns` (`enemy_ai_re2_zellenarm.c:204-207`,
Aufruf `enemy_ai_common.c:14496`). Keine weiteren. 0x13 (Zombie Girl) nutzt im Port das RE2-Zombie-Gehirn
und in RE2 dieselbe Schadenszeile 0x800A412C (@0x800A6AD4).

### 2.7 Nebenbefund zum RE1.5-Zustellweg (für §3 nötig)

RE1.5 FUN_80012D60, Gegner-Zweig (`re15_disasm.py dis 0x80012f10 72`):

```
80012fd4  sb zero,7(s1)                 ; +0x7 = 0
80012fd8  sb v0(=1),6(s1)               ; +0x6 = 1
80012fdc  lui at,0x8007 / addiu at,-3024 ; 0x8006F430
80012fe4  addu at,at,s0                 ; + Art
80012fe8  lbu v0,0(at) / 80012ff0 sb v0,5(s1)  ; +0x5 = DAT_8006F430[Art]
80012ff4  lhu a0,0(s3)                  ; s3 = 0x8006F418 + 2*Art (@0x80012f14-20)
80012ffc  subu / 80013000 sh v1,154(s1) ; HP(+0x9A) -= DAT_8006F418[Art]
80013008  ori 0x1 / sb 147              ; +0x93 |= 1
80013010..20  +0x4 = 2, bei HP < 0: 3
```

Bytes `8006f418: 0a 00 14 00 e8 03 …` (Art 2 = 1000) und `8006f430: 03 03 09 0a 0b 0e 0f 10 11 12 14`.
**+0x5 ist hier eine RE1.5-WAFFEN-Id** (dieselbe Id, die der Hitscan @0x800124BC schreibt): Art 2 → 9
(Hand Grenade), Art 3 → 10, Art 4 → 11. Das deckt sich mit `re_saeure_brand.md` §1.5.

## 3. Port-Abgleich je Typ

### 3.1 Die beiden Wege, auf denen heute eine Granate einen Gegner erreichen kann

**Weg B (heute live): Port-Brücke `ENT[9].resolve = 1`** (`engine/src/game_step_common.c:1775`,
Aufruf `:1808-1809`) → `re15_player_weapon_fire(9)` = RE1.5-Hitscan-Zwilling (`re15_damage.c:1922`).
Wirkung: EIN Ziel (Auto-Aim, nächstes in Reichweite 1000 = `s_player_wpn_reach[9]` `re15_damage.c:1005-1008`),
SOFORT beim Abzug (Runde 30 §2.1: 22 Bilder vor dem Verlassen der Hand). Für RE2-eigene Typen:
* Sperre `+0x1D3 = (…&0x80) | stun` (`:2648-2649`), stun[9] = 15 für Zombie/Hund/Krähe/Spinne (= RE2 ✓).
* Schaden `dmg_row[9]` = Klammer-0-Wert der RE2-Zeile 9 (Zombie 200, 0x16 80, Hund 300, Krähe 60,
  Spinne 60; `:581-640`) ✓ für den FLUG-Kontakt, ✗ für Explosion/Säure-Stoß (K1) und Nachbrenner (K2).
* `+0x5 = 9` (`:2667`), `+0x93 |= 1` (`:2683`), `+0x4 = 2/3` (`:2700`).
* Stempel `re15_re2_stamp_hit(e, 0, 9)` (`:2733` → `:2869-2957`): `+0x1D2 = (liegt || Zielen-tief) ? 0 : 1`
  (`:2939`) — Quelle ist die ZIELHÖHE DES SPIELERS, nicht der Aufschlagpunkt; Klammer immer 0;
  `+0x6 = 0`; Zombie-Familie: Zeile = `re2z_row_from_weapon[9]` = 9 (`enemy_ai_re2_zombie.c:3829-3835`).
* Waffen 10/11: `ENT[10]/[11].resolve = 0` (`:1779-1780`) → **kein Schaden, keine Reaktion**.

**Weg A (Plan Runde 30 §2.3 Punkt 4): `re15_resolve_attack(r=500, Punkt, Art 2)`** (`re15_damage.c:3294-3355`)
→ `re15_enemy_take_damage(e, 2)` (`:2959-2992`) für jeden Gegner, dessen RE1.5-Trefferkasten den Punkt
überdeckt (`re15_hitbox_test` `:3273`), Spieler eingeschlossen (`:3325-3331`). Für RE2-eigene Typen:
* Schaden `re15_damage_table[2]` = **1000 flach** (`:2973`), unabhängig vom RE2-Schadensmodell → jeder
  RE2-Typ stirbt, auch Brad (250), der in RE2 den Flug-Kontakt (200) überlebt.
* `+0x5 = re15_react_table[2] = 9` (`:2976`) — Werte = DAT_8006F430, also RE1.5-Waffen-Id (§2.7);
  der Port-Kommentar nennt es „reaction clip", das ist die falsche Lesart.
* KEINE RE2-Sperre +0x1D3 (nur `+0x93`-Riegel `:2968`).
* Stempel `re15_re2_stamp_hit(e, 1, 2)` (`:2981`): Zombie-Familie bekommt
  `re15_re2z_row_for_atktype(2)` = `re2z_row_from_atktype[2]` = **17 (Rakete)**
  (`enemy_ai_re2_zombie.c:3847`, Port-Zuordnung „Schadensklasse"). Belegt ist aber
  `+0x5 = DAT_8006F430[2] = 9` (@0x80012fdc-f0) → über `re2z_row_from_weapon` = **Zeile 9**.
  Hund/Krähe/Spinne lesen `+0x5 = 9` bereits als Waffe 9 → Zeile 9 (richtig).
* `+0x1D2` wieder aus der Zielhöhe des Spielers.

### 3.2 Je Typ: was kommt heute an, was fehlt

**Zombie-Familie 0x10/0x11/0x12/0x13/0x18, 0x16** (`enemy_ai_re2_zombie.c`):
* Reaktionsseite ist komplett portiert: HURT-/DEATH-Tabellen Zeilen 9/10/11 (`:3876-3899`, `:7183-7200`),
  Element-Leiter in allen Handlern, die sie im Original tragen (§2.1; `re2z_dismember_row` `:4772-4792`, Stagger `:4964-4974`,
  Kriecher `:6776-6818`, `:6927-6943`, DEATH-MAIN `:7454-7477`, Kriecher-Tod `:7345-7349`, ZERREISSEN
  `:5722`), Verkohlung/Ätzung/Ruß (`:4668-4738`), Säure-Bein-Wegätzen (`:4884-4910`), halbe Liegezeit
  (`:2172-2177`), Fett-Takt für Verkohlte (`:1397-1432`).
* **FEHLT 1 — Brand/Säure-DoT** (§2.1). Der Port hält den Block für einen „WALK edge-fall death"
  (`:1493-1494`) bzw. „Kanten-Sturz-/Jitter-Zweig" (`:1603-1606`). Das ist falsch gelesen: es ist 1 HP je
  8 Bilder (+0x236 & 7), Gier-Zucken ±64 bzw. Säure-Zucken, Tod mit 0x0A03/0x0B03 und +0x1D2 = 4. Der Port
  hat kein Feld +0x236 (`:8062-8066`).
* **FEHLT 2 — Leichen-Farbausblender** für Verkohlte (@0x8010A810-868; `:7712` OFFEN).
* **FEHLT 3 — Zustellung**: RE2-Spalte (Zone aus Aufschlag-Y statt Zielhöhe), Klammer 1/2 (Explosion,
  Säure-Stoß, Nachbrenner), Mehrfachziel (Flug) bzw. Erst-ungesperrtes-Ziel (Stöße), Richtungsbits
  +0x1D0 aus dem Aufschlagpunkt (heute `re2z_stamp_hit_row` mit Spieler-Peilung `:6611-6621`).
* **FEHLT 4 — Zonen-Reserven**: der Port zieht bei JEDEM RE2-Treffer die Reserve ab (`re2z_stamp_hit` `:7004`
  → `:6623-6632`, Kosten `(w1 >> 3*Klammer) & 7`). Das tut nur der Hitscan-Applier FUN_800410CC
  (@0x80041954-88); FUN_800470C0 (GL) schreibt keine Reserve (vollständige Store-Liste §1.2). Für
  GL-Treffer ist der Abzug eine Abweichung (Zeile 9/10/11 w1 & 7 = 2 → Bein-Verlust wird früher erreichbar).
* Weg A: Zeile 17 statt 9. Bei Klammer 0 gleich (DEATH-Spalte 0 = 0x80107438, Spalte 1 = 0x80108BEC in
  beiden Zeilen); ab Klammer 1 verschieden: Zeile 17 Spalten 3-5 = {0x80107438, 0x80109610, NULL},
  6-8 = {0x80107438, 0x80108530, NULL} (Dump @0x8010CE88, Port-Tabelle `:7204`) gegen Zeile 9 = 0x80108530.
* Nebenbefund: `re2z_pool_cost_w1` (`:6598-6604`) weicht oberhalb Bit 9 von der EXE ab (z. B. Zeile 9
  Port 0x078EFC0A, EXE `0a 1e 8f 07` = 0x078F1E0A @0x800A41D0). Die genutzten Bits 0..8 sind gleich —
  folgenlos, aber der Kommentar „selbst gedumpt" stimmt für die oberen Bits nicht.

**Hund 0x20** (`enemy_ai_re2_dog.c`):
* HURT Zeilen 9/10/11 portiert (`:2037-2070`), Zeile 11 ohne die Part-Farbe 0x3F2F auf EINEM Part
  (@0x80103EB0-C0; Port `:2070-2073` nur FX 9, stumm).
* DEATH: Zeile 9 → Gore-Zweig portiert, aber der **Teile-Wurf 0x80104440** (7 Parts, Hund zerplatzt) ist
  Render-OPEN (`:2159-2161`), und die Pro-Bild-Blut-FX an Part 3/2 in 0x80104610 (@0x80104644-7C) fehlen.
* **FEHLT**: DEATH Zeile 10 (0x80104774: 17 Parts 0x00202020 + 6× FX 7) und Zeile 11 (0x8010481C: 17 Parts
  0x00003F2F + FX 9/10) — im Port „auf den Kern gefallen" (`:2152-2156`).

**Krähe 0x21** (`enemy_ai_re2_crow.c`): Zeilen 9/10/11 → GIB portiert (`:1486-1500`, `:1317-1339`).
Weg A/B liefern beide +0x5 = 9 → GIB. Lücke nur die Zustellung (§3.1).

**Spinne 0x25 / Baby 0x26** (`enemy_ai_re2_spider.c`): HURT/DEATH-Zeilen 9/10/11 portiert
(`:1957-2003`, `:2258-2296`); OFFEN sind die Part-Farben 0x00202F2F/0x00101F3F (`:2280`, `:2289`),
Part 19 Flug und das Mesh-Gate der Blut-Emitter.

**Zellenarm 0x1A → RE2 0x2D** (`enemy_ai_re2_zellenarm.c`): Reaktion zeilenunabhängig (§2.5), Port
`arm_hurt`/`arm_death` (`:511-529`). Der RE2-Stempel läuft für 0x1A NICHT (`re15_re2_owns_type` enthält
0x1A nicht, `re15_damage.c:2881-2882`); folgenlos, weil der Port-Arm weder +0x5 noch +0x1D2 liest und
im Original alle erreichbaren Spalten denselben Zweig haben (§2.5).
Weg B trifft ihn mit dem RE1.5-Zombie-Schaden (`s_player_wpn_dmg_zombie[9]` = 100, `re15_damage.c:394-396`,
Standardzweig `:751`) gegen HP 250.

### 3.3 Welche Spalte trifft der Flug? (für den Bau wichtig)

Stehender RE2-Zombie: `+0x98 = -1500`, `+0x9E = 1500` (Steh-Box, Port `enemy_ai_re2_zombie.c:2306`, `:8013`),
`+0x1EE = 500` (@0x8010096C, Port `re15_damage.c:1539`); Hund `+0x1EE = 600` (@0x80100290). Höhenband
§1.2: P.y ∈ (Y − 3100, Y + 100]. Zonen-Regel (Bit 0x20000 gesetzt): Zone 0, wenn P.y > Y − 750
(`sra a0,v0,17` = −1500/2 @0x800472B8, `slt` @0x800472C8). Der Flug prüft ERST bei y+1000, dann bei y−1000,
und die Sperre +0x1D3 = 15 aus dem ersten Treffer blockt den zweiten. Daraus (reine Rechnung aus den
belegten Konstanten, y = Granaten-Y, Y = Fuß-Y des Zombies, PSX-y wächst nach unten):
* Granate höher als 1750 über dem Fuß → 1. Prüfung trifft, Zone 1 → Spalte 1 → DEATH 0x80108BEC (Zerreißen).
* Granate 900..1750 über dem Fuß → 1. Prüfung trifft, Zone 0 → Spalte 0 → DEATH 0x80107438 (Knockdown mit Todeszweig).
* Granate tiefer als 900 → 1. Prüfung liegt unter dem Band, 2. Prüfung (y−1000) trifft, Zone 1 → Zerreißen.

Die Spalte hängt also an der FLUGHÖHE, nicht an der Zielhöhe des Spielers (heute `re15_damage.c:2939`).

## 4. Übersicht je Typ

| Typ | Port-KI | RE2 Explosiv (Zeile 9) | RE2 Säure (Zeile 11) | RE2 Brand (Zeile 10) | Schaden K0/K1/K2 (9 · 10 · 11) | Clip(s) | Port heute | Lücke |
|---|---|---|---|---|---|---|---|---|
| Zombie 0x10/0x11/0x12/0x13/0x18 | RE2 (`enemy_ai_re2_zombie.c`) | Tod: Sp.1 → 0x80108BEC Zerreißen (+Ruß, 3/4 → Wegschleudern 0x80109610), Sp.0 → 0x80107438; Überleben (Brad, K1): 0x80105BC0 bzw. 0x80105438 | Tod: 0x80108530 Sturz + Ätzung; Überleben K0: Taumeln + Bein wegätzen + Ätzung + DoT | Tod: 0x80108530 + Verkohlung; Überleben: Taumeln + Verkohlung + DoT 1 HP/8 Bilder | 200/50/10 · 200/50/5 · 200/50/10 (@0x800A41CC/E0/F4); Brad HP 250 | Sturz Clip 1 ab Bild 0 / Clip 2 ab Bild 10 (@0x8010855C, Port `:7489-7525`); Taumeln Clip 4 (Rücken 3), Rate 3 (@0x80105C38-74) | Reaktion komplett; Zustellung = Hitscan beim Abzug (Weg B), nur Id 9 | DoT, Leichen-Ausblender, Zustellung (Mehrfach/Box/Höhe/Klammer/Zone/+0x1D0), kein Reserve-Abzug für GL, Weg A Zeile 17 statt 9 |
| Zombie 0x16 | RE2 (Zombie-Gehirn) | wie oben | wie oben | wie oben | 80/50/10 · 80/50/5 · 200/50/10 (@0x800A4348/5C/70) — überlebt Explosiv-/Brand-Flug je nach HP | wie oben | wie oben (`s_re2_wpn_dmg_zombie16` ✓) | wie oben |
| Hund 0x20 | RE2 (`enemy_ai_re2_dog.c`) | Tod K0: zerplatzt (0x80104694: stumm, Teile-Wurf 0x80104440, FX 7, Blut je Bild); K≥1: Tod mit Schrei | Tod: 0x8010481C alle Parts 0x3F2F + FX 9/10; HURT: 1 Part 0x3F2F + FX 9 | Tod: 0x80104774 alle Parts 0x202020 + 6× FX 7; HURT: FX 8 ×(4−K), +0x1D3 \|= 0x80 | 300/50/10 · 300/50/5 · 300/50/10 (@0x800A44C4/D8/EC) | Kern-Clip 17 (Port `:2118`), Rutschen Clip 18 (@0x80104274-8C, Port `:2192`) | HURT 9/10/11 ✓ (11 ohne Farbe), DEATH 9 ✓ ohne Teile-Wurf | DEATH 10/11 fehlen, Teile-Wurf + Blut je Bild, Zustellung |
| Krähe 0x21 | RE2 (`enemy_ai_re2_crow.c`) | GIB 0x80102CA0 | GIB | GIB | 60/60/15 alle drei (@0x800A4640/54/68), HP 10 | keiner (versteckt, Corpse Sub 1) | ✓ | nur Zustellung |
| Spinne 0x25 | RE2 (`enemy_ai_re2_spider.c`) | HURT (60 < HP): Knock 0x80102FDC + Blut; Tod: Zerplatzen + 6..9 Babys | HURT: Bein ab; Tod: Parts 0x101F3F, Part 19 fliegt | HURT: 5 Blut-Emitter; Tod: Parts 0x202F2F | 60/60/20 · 130/130/5 · 90/60/10 (@0x800A4C30/44/58) | Zerplatz-Clip 12, Rate 7 (@0x80104578) | ✓ (Farben/Part-Flug OPEN) | Farben, Zustellung |
| Baby 0x26 | RE2 (EMS26) | HP 1 → jeder Treffer tötet (Port `re15_damage.c:434-437`) | dto. | dto. | Zeile wie 0x25 | — | ✓ | Zustellung |
| Zellenarm 0x1A (RE2 0x2D) | RE2 (`enemy_ai_re2_zellenarm.c`) | Rückzug 0x501 (zeilenunabhängig) | dto. | dto. | 60/60/20 · 60/60/10 · 60/60/10 (@0x800A5220/34/48), HP 250, unsterblich | Rückzug | ✓ (Weg B mit RE1.5-Schaden 100) | keine Reaktionslücke |

(„K" = Klammer des Schadenswortes; Flug-Kontakt und Brand-Stoß = K0, Explosion und Säure-Stoß = K1,
Nachbrenner = K2.)

## KONSTANTEN FUER DEN BAU

Alle RE2-Adressen `info/re2leon/PSX.EXE` (t_addr 0x80010000) bzw. das genannte Overlay (roh @0x80100000).

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| GL_HITCODE_FLUG | 0x00030009 + Art (0 HE, 1 Brand, 2 Säure) | @0x8001EE90-EDC | `lui s2,0x3` / `ori s2,s2,0x9` / `lb a3,27(v1)` / `addu a3,a3,s2` | Flug-Kontakt: Zeile 9/10/11, K0, alle Gegner (0x10000), Zonen (0x20000) |
| GL_FLUG_Y1 / Y2 | y+1000, dann y−1000 | @0x8001EEC8, @0x8001EEF8 | `addiu v0,v0,1000` / `addiu v0,v0,-2000` | zwei Prüfpunkte je Bild |
| GL_FLUG_BOX | {-1400, 0, 350, 250} → x∈[-1400,0], z∈[-1000,1000] | @0x80010900 | `88 fa 00 00 5e 01 fa 00` | Boxtest FUN_80041EF8 |
| GL_HITCODE_EXPLOSION | 0x10020009 | @0x80020D54-58 | `lui a3,0x1002` / `ori a3,a3,0x9` | Op 47: Zeile 9, K1, erster ungesperrter Gegner |
| GL_EXPL_GATE | Slot +0x1E == 12 | @0x80020D30-38 | `lbu v1,30(v1)` / `addiu v0,zero,12` / `bne` | nur Unter-Id 12 teilt Schaden aus |
| GL_EXPL_Y | y, dann y+900 | @0x80020D98 | `addiu v0,v0,900` | Prüfpunkte Explosion |
| GL_EXPL_BOX | {-2000, 0, 1000, 500} → ±2000 × ±2000 | @0x80010918 | `30 f8 00 00 e8 03 f4 01` | Explosion |
| GL_HITCODE_BRAND | 0x0002000A | @0x80021058-64, @0x800214F0-FC | `lui a3,0x2` / `ori a3,a3,0xa` | Op 48: Zeile 10, K0, erster Gegner |
| GL_BRAND_PUNKT | beide Aufrufe am Aufschlagpunkt (Slot+0x60); die ±1800 treffen nur die Kopie sp+20 | @0x80021050, @0x80021090, @0x80021528 (a0); @0x80021080/@0x80021518 (Kopie) | `addiu a0,v1,96` / `addiu a0,a0,96`; `addiu v0,v0,1800` `sw v0,20(sp)` | Brand-Stoß: 2 Aufrufe, gleicher Punkt |
| GL_HITCODE_SAEURE | 0x1002000B | @0x800216E4-F0, @0x800216FC / @0x8002171C | `lui a3,0x1002` / `ori a3,a3,0xb` | Op 49: Zeile 11, K1 |
| GL_SAEURE_Y | y, y+1800 (beide Fälle) | @0x8002170C | `addiu v0,v0,1800` | Säure-Stoß |
| GL_BOX_WAND | {-1200, 0, 600, 300} → ±1200 | @0x8001093C | `50 fb 00 00 58 02 2c 01` | Brand/Säure bei Status-Bit 0x80 = 0 (@0x80021040-4C, @0x800216C8-D8) |
| GL_BOX_GEGNER | {-600, 0, 300, 150} → ±600 | @0x80010944 | `a8 fd 00 00 2c 01 96 00` | Brand/Säure bei Status-Bit 0x80 = 1 |
| GL_HITCODE_NACHBRENNER | 0x2002000A | @0x80020794-A0 | `lui a3,0x2002` / `ori a3,a3,0xa` | Op 40: Zeile 10, K2, je Bild aus Op 19 (ein Treffer stoppt nur das Gleiten, nicht den Schaden) |
| GL_NACHBR_BOX / Y | {-600,0,300,150}; y−100 | @0x80010910, @0x800207A4 | `a8 fd 00 00 2c 01 96 00`; `addiu v0,v0,-100` | Treffer → Op 50 `jal 0x80021970` @0x800207CC (Gleiten aus: step[2] := 64, acc.x/vel.x := 0, Status &= ~2 @0x80021978-B0); aufgerufen aus Op 19 (@0x8001F2D0-308: +0x4A != 0, step[0x16] ≥ 16, X-Aspekt > 0x1000) |
| SE_EXPLOSION / BRAND / SAEURE | 0x01110001 / 0x01120001 / 0x01130001 | @0x80020D40-48 / @0x80020FD4+@0x80021028 / @0x80021678-7C | `lui a0,0x111` bzw. `0x112`/`0x113`, `ori a0,a0,0x1`, `jal 0x8005ba28` | Aufschlag-SE (Unter-Id 13: 0x01140001 @0x80020D3C/@0x80020DC0) |
| APPLIER_LISTE | 0x800CFE1C .. *(0x800CE334) | @0x800470FC, @0x80047118, @0x80047410 | `addiu s7,s7,-15896` / `addiu s2,s7,15412` / `lw v0,8524(s7)` | Gegner-Zeigerliste |
| APPLIER_GATES | Wort0&1; +0x1D3 == 0 (ganzes Byte); HP ≥ 0; +0x10E&0xC000 == 0 | @0x8004712C-64 | `andi 0x1` / `lbu v0,467` `bne` / `lh v0,342` `bltz` / `andi 0xc000` `bne` | Kandidatenfilter |
| APPLIER_BAND | (Y + h98 + 100 + d9e − P.y) <u 2·(d9e+100) | @0x8004716C-A4 | `lh a0,152` / `addiu v0,v0,100` / `lhu v1,158` / `sll v1,v1,1` / `sltu` | Höhenband |
| APPLIER_RADIUS | p2 += (s16)+0x1EE>>2, p3 += dto. | @0x800471BC-EC | `lhu v1,494` / `sll 16` / `sra 18` / `sh 6(s3)`, `sh 4(s3)` | Box um Gegnerradius erweitern; im Alle-Modus nach Treffer NICHT zurückgenommen (Rücknahme nur @0x800473DC-408) |
| APPLIER_SCHADEN | (w0 >> 10·K) & 0x3FF | @0x8004722C-68 | `lw a1,27272(at)` / `addiu v0,v0,-20` / `srlv` / `andi 0x3ff` / `sh v0,342` | HP −= |
| APPLIER_ZUSTAND | +0x4-Wort = 2, bei HP<0 = 3; +0x1FC = alt (außer 0xC02; Einzelzweig 0xC02/0xC03) | @0x8004726C-90, @0x8004748C-B4 | `sw v0,4(s1)` | HURT/DEATH (nullt +0x5/+0x6/+0x7) |
| APPLIER_ZONE | 1; Bit 0x20000 && Y+h/2 < P.y → 0; Wort0&0x10000000 && P.y < Y+3h/2 → 2; danach += 3·K | @0x80047294-330 | `sb v0(=1),466` / `sb zero,466` / `sb v0(=2),466` / `sll v1,s6,1` `addu` | +0x1D2 |
| APPLIER_ZEILE | +0x5 = Hitcode & 0xFF | @0x80047324 | `sb s5,5(s1)` | Zeile 9/10/11 |
| APPLIER_SPERRE | +0x1D3 = (alt&0x80) \| ((w1>>9)&0x7F) = 15 | @0x8004732C-4C | `andi a0,a0,0x80` / `srl v0,v0,9` / `andi v0,v0,0x7f` / `or` / `sb a0,467` | Einmal-Sperre 15 Bilder (Root zählt low-7 herunter: Zombie @0x80100484-98, Hund @0x80100028-3C, Spinne @0x801000EC-100) |
| APPLIER_RICHTUNG | a = Peilung(P→G) − Yaw(+0x76): (a+0x400)&0xFFF<0x800 → 0x20; (a+0x600)&0xFFF<0x400 → 0x40; (a−0x200)&0xFFF<0x400 → 0x80 | @0x80047358-3D8 | `jal 0x800154ac` / `lh v1,118` / `slti 2048` / `slti 1024` / `ori 0x20/0x40/0x80` | +0x1D0 (vorher `&= 0xFF00` @0x8004716C-84) |
| SCHADEN_ZOMBIE_9/10/11 | 200/50/10 · 200/50/5 · 200/50/10 | @0x800A41CC / @0x800A41E0 / @0x800A41F4 | `c8 c8 a0 00` / `c8 c8 50 00` / `c8 c8 a0 00` | Typen 0x10-0x14, 0x18-0x1F, 0x2C |
| SCHADEN_ZOMBIE16_9/10/11 | 80/50/10 · 80/50/5 · 200/50/10 | @0x800A4348 / @0x800A435C / @0x800A4370 | `50 c8 a0 00` / `50 c8 50 00` / `c8 c8 a0 00` | Typen 0x15-0x17 |
| SCHADEN_HUND_9/10/11 | 300/50/10 · 300/50/5 · 300/50/10 | @0x800A44C4 / @0x800A44D8 / @0x800A44EC | `2c c9 a0 00` / `2c c9 50 00` / `2c c9 a0 00` | 0x20 |
| SCHADEN_KRAEHE_9/10/11 | 60/60/15 (alle) | @0x800A4640 / @0x800A4654 / @0x800A4668 | `3c f0 f0 00` | 0x21 |
| SCHADEN_SPINNE_9/10/11 | 60/60/20 · 130/130/5 · 90/60/10 | @0x800A4C30 / @0x800A4C44 / @0x800A4C58 | `3c f0 40 01` / `82 08 52 00` / `5a f0 a0 00` | 0x25/0x26 |
| SCHADEN_ARM_9/10/11 | 60/60/20 · 60/60/10 · 60/60/10 | @0x800A5220 / @0x800A5234 / @0x800A5248 | `3c f0 40 01` / `3c f0 a0 00` / `3c f0 a0 00` | RE2 0x2D |
| ZOMBIE_HURT_TAB | Zeile 9: {7438,5BC0,5BC0, 5438×6}; 10/11: {5BC0×3, 5438×6} | @0x8010CA84 / @0x8010CAA8 / @0x8010CACC (EMZ0.BIN) | Basis `lui a2,0x8011` / `addiu a2,a2,-14016` @0x801053E4-E8 | HURT-Zelle [+0x5][+0x1D2] |
| ZOMBIE_DEATH_TAB | Zeile 9: {7438,8BEC,8BEC, 8530×6}; 10/11: 8530×9 | @0x8010CD68 / @0x8010CD8C / @0x8010CDB0 (EMZ0.BIN) | Basis `lui a2,0x8011` / `addiu a2,a2,-13276` @0x801084E8-EC | DEATH-Zelle (⚠ @0x8010CD20 ist Zeile 7, nicht die Basis) |
| ZOMBIE_DOT_TAKT | HP −1, wenn (+0x236 & 7) == 0 und +0x21A & 0x800 und (+0x10E & 0x80 oder +0x21A & 0x1000) | Gang @0x80101DC0-E4C, Anrempeln @0x80102474-4D8 (EMZ0.BIN) | `lhu v0,566` / `andi v0,v0,0x7` / `lhu v0,538` / `andi v0,v0,0x800` / `addiu v0,v0,-1` / `sh v0,342` | Brand/Säure über Zeit |
| ZOMBIE_DOT_ZAEHLER | +0x236 += 1 je Bild; INIT 0 | @0x801004F8-508, @0x801008AC | `lhu v0,566` / `addiu v0,v0,1` / `sh v0,566`; `sh zero,566` | Takt |
| ZOMBIE_DOT_TOD | +0x4-Wort 0x0A03 (Brand) / 0x0B03 (Säure); +0x1D2 = 4; +0x1D3 \|= 0x80; +0x21A \|= 0x2000 | @0x80101E54-90 | `addiu v1,zero,2563` / `addiu v0,zero,2819` / `addiu v0,zero,4` `sb v0,466` / `ori v0,v0,0x80` / `ori v1,v1,0x2000` | Tod am Brand; Sturz-Tod liest 0x2000 → Seite 0 (@0x801085AC-C0) |
| ZOMBIE_DOT_ZUCKEN | Brand-Tick: Yaw += 64 − ((r1>>(r2&3))&1)·128; Säure: Yaw += 64 − (rand>>1) je Bild | @0x80101E10-50, @0x80101EA8-C0 | `srav` (Rohwort 0x00508007) / `andi s0,s0,0x1` / `sll s0,s0,7` / `addiu v1,v1,64`; `sra v0,v0,1` | Gier-Zucken |
| ZOMBIE_ELEMENT | Zeile 10 → FUN_80106128, 11 → FUN_80106310, 9 → FUN_8010640C | HURT @0x80105188-208 (+ neun weitere Stellen §2.1) | `addiu v0,zero,10/11/9` + `jal` | Port vorhanden (`enemy_ai_re2_zombie.c:4668-4738`) |
| HUND_TOD_BRAND | alle 17 Parts +0x70 = 0x00202020; 6× FX 7; +0x21F = 6 | @0x80104774-800 (EMD0G_MOD0.BIN) | `lui a0,0x20` / `ori a0,a0,0x2020` / `sw a0,112(v0)` / `sltiu v0,s0,0x11`; `sltiu v0,s0,0x6` | Gate +0x1D2 < 3 oder Zeile 16 (@0x8010478C-A8) |
| HUND_TOD_SAEURE | alle 17 Parts 0x00003F2F; +0x21F = 2; FX 9 an (rand&7), FX 10 an ((rand&7)\|8) | @0x8010481C-89C | `addiu a1,zero,16175` / `sb v0(=2),543` / `ori a1,v0,0x8` | Gate +0x1D2 < 3 (@0x80104830-3C) |
| HUND_TOD_HE | stumm (+0x231 = 1), Teile-Wurf 7 Parts {2,3,4,7,8,9,10} (Flags \|= 0x4A, +0x9C 800, +0x9A −150, +0x9E 10, +0xA4 −100, Farbe 0x00101040), FX 7, +0x21F = 18 | @0x80104694-758, @0x80104440-4D8, Tabelle @0x80105680 | `sltiu v0,v0,0x3` / `ori v0,v0,0x4a` / `lui a2,0x10` `ori a2,a2,0x1040` / `addiu v0,zero,18` | Gate +0x1D2 < 3 (@0x801046B8-C4) |
| HUND_HURT_SAEURE | 1 Zufalls-Part (rand&0xF) Farbe 0x00003F2F; +0x21F = 1; FX 9 | @0x80103E80-EC | `addiu v1,zero,16175` / `sw v1,112(v0)` / `sb s0(=1),543` | HURT Zeile 11 |
| HUND_FX_TAB | FX7 {0x85,3,0,0x1000} FX8 {0x85,4,0,0x1000} FX9 {0x84,0x0F,0,0x1400} FX10 {0x84,0x0F,0,0x0C00} | @0x801056D6 / @0x801056DC / @0x801056E2 / @0x801056E8 | `85 03 00 00 00 10` / `85 04 00 00 00 10` / `84 0f 00 00 00 14` / `84 0f 00 00 00 0c` | FUN_80105070 |
| SPINNE_TOD_FARBE | Zeile 10: 0x00202F2F; Zeile 11: 0x00101F3F (20 Parts) | @0x80104B44-4C, @0x80104BC0-C4 (EMS25.BIN) | `lui a1,0x20` / `ori a1,a1,0x2f2f`; `lui a1,0x10` / `ori a1,a1,0x1f3f`; FUN_8010609C @0x8010609C-C4 | Mesh-Farbe |
| RE15_ART_ZU_WAFFE | +0x5 = DAT_8006F430[Art] = {3,3,9,10,11,14,15,16,17,18,20} | @0x80012FDC-F0 (RE1.5 PSX.EXE) | `lui at,0x8007` / `addiu at,at,-3024` / `lbu v0,0(at)` / `sb v0,5(s1)`; Bytes `03 03 09 0a 0b 0e 0f 10 11 12 14` | Weg A: Art 2 → Waffe 9 → RE2-Zeile 9 (nicht 17) |

## PORT-ABGLEICH

Kurzfassung von §3 (Einzelheiten und Belege dort). „Weg B" = heutige Brücke, „Weg A" = geplanter
RE1.5-Flächenschaden über `re15_resolve_attack`.

| # | Was | Port heute (Datei:Zeile) | Lücke |
|---|---|---|---|
| P1 | Zustellung Id 9 | `game_step_common.c:1775` `ENT[9].resolve = 1` → `re15_player_weapon_fire(9)` (`re15_damage.c:1922`), EIN Ziel, beim Abzug | kein Flug, keine Box, kein Mehrfachziel, keine Explosion; RE2-Ereignisse §1.1 fehlen |
| P2 | Zustellung Id 10/11 | `game_step_common.c:1779-1780` `resolve = 0` | gar keine Wirkung; RE2-Ziel = Säure-/Brand-Runde §1.1 |
| P3 | Klammer | immer 0 (`re15_damage.c:2653-2666`, Sonde nur für Geometrie-Ids ≠ 9/10/11 `:1917-1920`) | Explosion/Säure-Stoß brauchen K1, Nachbrenner K2 |
| P4 | Spalte +0x1D2 | Zielhöhe des Spielers `re15_damage.c:2934-2940` | aus dem Prüfpunkt (Zone-Regel §1.2, Rechnung §3.3) |
| P5 | Sperre +0x1D3 | Weg B stempelt 15 ✓ (`:2648-2649`); Weg A stempelt NICHT (`:2959-2992`) | Weg A braucht sie für Mehrfach-Ereignisse im selben Bild |
| P6 | Zeile Zombie | Weg B: 9/11/10 ✓ (`enemy_ai_re2_zombie.c:3829-3835`); Weg A: `re2z_row_from_atktype[2]` = 17 (`:3847`) | Weg A: Zeile = `re2z_row_from_weapon[DAT_8006F430[Art]]` (Art 2 → 9, 3 → 11, 4 → 10) |
| P7 | Schaden Weg A | `re15_damage_table[2]` = 1000 flach (`re15_damage.c:2973`) | RE1.5-Original für Id 9 (§2.7); RE2-Werte §1.3 — siehe Bauhinweis |
| P8 | Richtungsbits +0x1D0 | Peilung SPIELER→Gegner (`enemy_ai_re2_zombie.c:6611-6621`) | Peilung Prüfpunkt→Gegner |
| P9 | Zonen-Reserven | Abzug bei jedem Treffer (`enemy_ai_re2_zombie.c:7004`, `:6623-6632`) | FUN_800470C0 zieht nichts ab → für GL-Treffer weglassen |
| P10 | Zombie-DoT | fehlt; als „edge-fall"/„Jitter" fehlgedeutet (`:1493-1494`, `:1603-1606`), kein Feld +0x236 (`:8062-8066`) | Block @0x80101DC0-EC0 / @0x80102474-54C nachbauen |
| P11 | Zombie-Leichen-Ausblender | OFFEN (`:7712`, `:7863`) | @0x8010A810-868 |
| P12 | Hund DEATH 10/11 | auf den Kern gefallen (`enemy_ai_re2_dog.c:2152-2156`) | 0x80104774 / 0x8010481C |
| P13 | Hund HE-Tod | Teile-Wurf Render-OPEN (`:2159-2160`), Blut je Bild aus 0x80104610 fehlt | 0x80104440, @0x80104644-7C |
| P14 | Hund HURT 11 | Part-Farbe fehlt (`:2070-2073`) | @0x80103EB0-C0 |
| P15 | Spinne | Farben/Part 19 OPEN (`enemy_ai_re2_spider.c:2280`, `:2289`); Kommentar `:2285` nennt Zeile 11 „verkohlt" (ist Säure) | Mesh-Farbe FUN_8010609C |
| P16 | Krähe, Baby-Spinne, Zellenarm | Reaktion vollständig | nur Zustellung (P1-P9) |
| P17 | `re2z_pool_cost_w1` | obere Bits ≠ EXE (`enemy_ai_re2_zombie.c:6598-6604`) | folgenlos (nur Bits 0..8 genutzt) — Kommentar korrigieren |

Nicht gebaut (Auftrag RE-Phase). Keine Messläufe gegen den Port gefahren: alle Port-Aussagen oben sind
Quelltext-Lesungen mit Zeile; eine Laufzeit-Messung (probe) ist für den Bau nachzuholen.

### Bauhinweise (aus den Belegen oben, keine neuen Zahlen)

1. **Applier-Zwilling FUN_800470C0** für RE2-eigene Typen statt Hitscan: Gates 1-4 + Höhenband, gedrehtes
   Rechteck nach FUN_80041EF8 inkl. Radius-Erweiterung (+0x1EE>>2 auf p2/p3, im Alle-Modus nach Treffer
   nicht zurücknehmen), Bit 0x10000 = alle / sonst erster, Schaden = Record[Zeile] >> 10·K, +0x4-Wort 2/3
   (nullt +0x5/+0x6/+0x7), +0x1FC, +0x1D2 = Zone + 3·K (Zone aus Prüfpunkt-Y), +0x5 = Zeile,
   +0x1D3 = (alt&0x80) | 15, +0x1D0 &= 0xFF00 je Kandidat vor dem Band + Richtungsbits aus dem Prüfpunkt.
   KEIN Zonen-Reserve-Abzug (P9). Gate 2 (+0x1D3 ganz) selbst prüfen, nicht nur über +0x93.
2. **Ereignisse je Art** exakt nach §1.1/§1.2: Flug zwei Prüfpunkte (y+1000, y−1000) je Bild, Explosion
   HE Op 47 Phase 0 (y, y+900, Box ±2000, K1), Brand-Stoß 2× am Aufschlagpunkt (Box nach Status-Bit 0x80),
   Säure-Stoß (y, y+1800, K1), Bodenflammen Op 19 → Op 40 je Bild (Bedingungen @0x8001F2D0-308).
3. **Weg A** (RE1.5-Art 2 über `re15_resolve_attack`) für RE2-Typen: Zeile = `re2z_row_from_weapon[DAT_8006F430[Art]]`
   (Art 2 → 9), nicht `re2z_row_from_atktype`. Die Schadenszahl (RE1.5 1000 flach, Original für Id 9;
   oder RE2-Record) hängt an der Reichweite der bestehenden Port-Option „RE2-Schadensmodell", die heute
   nur den Hitscan abdeckt (`re15_damage.c:699-760`).
4. **Zombie-DoT** nachbauen: Zähler +0x236 im Root (+1 je Bild, INIT 0), Block im Gang (@0x80101DC0-EC0)
   und Anrempeln (@0x80102474-54C), Tod 0x0A03/0x0B03 + +0x1D2 = 4 + +0x1D3 |= 0x80 + +0x21A |= 0x2000;
   Sturz-Tod liest 0x2000 (@0x801085AC-C0). Port-Kommentare `:1493-1494`/`:1603-1606` korrigieren.
5. **Hund**: DEATH 10/11 (Farben, FX, +0x21F), Zeile-9-Teile-Wurf 0x80104440 und Blut je Bild aus 0x80104610,
   HURT 11 Part-Farbe.
6. **Kommentar-Korrekturen**: `re15_react_table` = RE1.5-Waffen-Id (DAT_8006F430); Spinne `:2285` Zeile 11 =
   Säure; `re2z_pool_cost_w1` obere Bits; „BURN" bei 0x80107EF0.
7. **Mess-Sonde** vor und nach dem Bau je Typ: Zelle (+0x5/+0x1D2), HP, Clip, +0x1D3, +0x1D0, +0x21A/+0x10E.

## OFFEN

| # | Offen | Versuchte Wege | Nächster Weg |
|---|---|---|---|
| O1 | ~~Welcher `step[2]`-Wert im Granaten-Skript steht~~ → **GESCHLOSSEN**: CORE00.ESP Bank 2 Skript 4, Step @Datei 0x192C `00 11 2f 2f …` → step[2] = step[3] = 47 (selbst nachgelesen, Fund Nachbar-Dossier §2.2) | — | — |
| O2 | ~~Unter-Id (+0x1E) des GL-Slots~~ → **GESCHLOSSEN**: Spawn 0x020C0A00 (Explosiv, `lui a0,0x20c / ori a0,a0,0xa00` @0x80044BE8-EC) bzw. 0x020C1000 (Brand/Säure @0x80044F9C-A0) → Unter-Id 12 → Op-47-Gate erfüllt | alle 79 `jal 0x8001cbe8` der EXE aufgelistet (keiner spawnt die Runde); Spawner ist FUN_8001BF10 im Waffen-Effekt-Handler (Nachbar §2.2) | — |
| O3 | ~~Welche Skripte Op 40 nutzen~~ → **GESCHLOSSEN**: Op 40 wird aus Op 19 gerufen (@0x8001F308, selbst gelesen); Op 19 läuft in Bank 5 Skript 5 (Bodenflamme nach Op 46) und Skript 4 (Folgeflamme via Op 30) — Nachbar-Dossier §2.5 | — | — |
| O4 | Lage der Flug-Box zur Flugrichtung. Belegt: Box x ∈ [−1400, 0] im Rahmen RotY(+0x22), +0x22 = Spieler-Yaw (`lh a1,118(s0)` @0x80044BF4); RE2-Einheiten laufen lokal +x vorwärts (FUN_800152C8, Stagger-Schub −450 = zurück, Port `enemy_ai_re2_zombie.c:4944-4958`); die Runde fliegt im 0x400-Rahmen des Waffenknochens entlang +y (Status 0xB403 @0x8001F1E4-E8, vel.y 600..250 @0x80044C40-4C). **Folgerung, nicht gemessen:** die Box liegt HINTER dem Prüfpunkt auf der schon durchflogenen Strecke (Sweep gegen Durchtunneln) | Disasm FUN_80041EF8 (Ecke/Kanten), Spawn-Code des Nachbarn | Laufzeit: RE2-Savestate mit fliegender Runde (Slot-Pool 0x800D8CF0) oder Port-Sonde nach dem Bau |
| O5 | ~~Leser des Explosionspunkts DAT_800CFB88/8C/90~~ → **GESCHLOSSEN**: Scan `re_gegner_re2_werkzeug/global_leser_scan.py` über PSX.EXE, EMZ0, EMD0G_MOD0, EMS25, EMS26, EMOVL21_S0 und alle `CDEMD0_EM*_ai0/ai1.BIN`: Schreiber nur in der EXE (@0x8001EF58-70 GL, @0x8001F7E0-F8 Rakete, @0x80023414-30 Flammenwerfer); Leser NUR in CDEMD0_EM36 (@0x80101DC0-F0, @0x80102010-40, @0x8010223C-6C, ai0 = ai1). Kein Leser in der hier behandelten Familie → deren Reaktion hängt nur an +0x5/+0x1D2/+0x1D0. (Hinweis für das Boss-Dossier: RE2-Typ 0x36 liest den Punkt.) | — | — |
| O6 | Hund/Spinne/Krähe: Brand/Säure über Zeit → **belegt NEGATIV**. Voll-Scan aller `sh …,342(…)` (HP-Schreiber): Hund @0x80100234 (INIT) und @0x8010387C (HP-Neuwurf aus Tabelle @0x80105340, dazu +0x151..0x153 := 13 @0x80103880-8C), Spinne @0x80100308 (INIT) / @0x80103EB0 / @0x801041AC (Leiche HP := 1), Krähe @0x80100348 (INIT) / @0x8010302C / @0x801032F4 (Tötung) — kein periodischer Abzug. Brand über Zeit gibt es bei diesen Typen nur über die Bodenflammen (Op 19/40) | — | — |
| O7 | Laufzeit-Beleg im Port (welche Zelle, welche HP, welcher Clip nach einem GL-Treffer) | nicht gefahren (RE-Phase) | Sonde `probe_re2_*` mit `re15_player_weapon_fire(9)` bzw. `re15_enemy_take_damage(e,2)` je Typ |
