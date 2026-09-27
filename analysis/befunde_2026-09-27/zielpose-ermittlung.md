# Runde 32 / Welle 1: DIE SENKRECHTE ZIELPOSE — Ermittlung

Status: **ERMITTLUNG FERTIG, kein Verhaltens-Code geändert** (Auftrag: „DU AENDERST IN DIESER
WELLE KEIN SPIELVERHALTEN"). Neu sind nur Mess-/Scan-Werkzeuge in diesem Verzeichnis
(`scan_off.py`, `ctx_stores.py`, `scan_word.py`, `emr_hier.py`,
`zielpose_fk.py`; die vorhandenen `scan_imm.py`/`scan_jal.py` sind UNVERAENDERT und wurden mit
ihrer eigenen Aufrufform benutzt). `local_build.sh all` grün, Abschlusszeile unten in §7.

Nutzer-Auftrag: *„Zielpose bauen, damit wir diesen Aspekt sauber abgeschlossen haben."* —
vorher: *„Das soll alles sauber Resident Evil 2 entsprechen."*

---

> ## ⛔ Kurzurteil — die Annahme dieser Runde ist WIDERLEGT
>
> 1. **Die Kette stimmt und ist belegt.** Bone 0 → 9 → 10 → 11 ist wörtlich die Ahnenkette des
>    Waffen-Knochens: aus den POSE-DATEN, nicht aus einem Namen — `PL00.EMR` liefert
>    `Eltern{11:10, 10:9, 9:0}`, und der Spiegelast 12→13→14 trägt dieselben Beträge mit
>    umgekehrtem Z (±369/±58/±37) = rechter/linker Arm (§1). Gilt für RE1.5 **und** RE2
>    (RE2s `PL00.emr`: identische Hierarchie, ±369/±80/±45).
> 2. **Die senkrechte Zielrichtung ist im Original die SPIELEREINGABE** — kein Auto-Ziel, keine
>    Gegnerhöhe. Drei diskrete Bänder in `+0x154` Bit 15/14/13 (UP/EBEN/DOWN), gesetzt aus den
>    Pad-Bits **0x10 (hoch)** und **0x20 (runter)** (@0x8004371C-B4, @0x80047AB0-B4C). Jedes Band
>    wählt einen ANDEREN Schuss-Clip aus der Tabelle @0x80011010 `{0,14,10,0,12}` (§2).
>    Ein auto-zielendes Band nach Gegnerhöhe existiert in RE2 auch — aber **nicht für Leon**,
>    sondern in der Handler-Tabelle @0x800A8930 eines NPC-Schützen (§2.3).
> 3. **Der Port hat diese Eingabe längst** (`s_aim_elev`, `player_common.c:1034`) und die drei
>    Clips (8/10/12 halten, 7/9/11 Rückstoß). Was fehlt, ist EINE Zeile Zuordnung:
>    `re15_player_muzzle_world` posiert die **Basis-Bank PL00** statt der **W-Bank** — gemessen
>    liefert sie exakt die **Bindpose 1666** (§4), und genau 1665…1671 hat Runde 30/31 gemessen.
> 4. ⛔ **Aber: die Zielpose HEBT die Mündung, sie senkt sie nicht.** Vorwärtskinematik über
>    RE1.5s echte Bank `PL00W03` (§3): **EBEN 2500 · HOCH 2751 · TIEF 1988**. Damit wird
>    **kein einziger** der offenen Fälle treffbar (Kriecher braucht ≤ 799, sitzende Krähe ≤ 979,
>    Spinne ≤ 1499, Baby ≤ 119) — und der **stehende Hund (≤ 2099) fiele bei EBENEM Zielen
>    NEU heraus**. Der Bank-Fix allein macht es also **schlechter**.
> 5. ⇒ **Krähe und Baby werden durch die Zielpose NICHT freigeschaltet.** Wer das baut, löst den
>    Befund nicht. Der nächste Weg steht in §8.

---

## 1. Die Knochenkette — was `FUN_8002CE94` tut, und wer 0/9/10/11 sind

### 1.1 `FUN_8002CE94(a0 = Eltern-MATRIX, a1 = Kind-MATRIX, a2 = Ausgabe-MATRIX)`

Selbst disassembliert (`info/re2leon/PSX.EXE`, `re2_disasm.py dis 0x8002ce94 86`). Es ist reine
GTE-Arbeit; der Disassembler zeigt die COP2-Wörter roh, hier decodiert:

```
8002ce94-cec  lw 0/4/8/12/16(a0)    -> ctc2 $0..$4     ; Eltern-ROTATION in die GTE
8002cebc-cd0  lw 20/24/28(a0)       -> ctc2 $5,$6,$7   ; Eltern-TRANSLATION (TRX/TRY/TRZ)
                                                        ; = MATRIX-Layout 3x3 short + 2 Pad + t[3]
8002ced4-ce8  lhu 0/6/12(a1)        -> mtc2 $9,$10,$11 ; SPALTE 0 der Kind-Rotation nach IR1..3
8002cef4      0x4a49e012 = MVMVA sf=1 mx=Rot v=IR cv=0 ; IR = (R_eltern * Spalte) >> 12
8002cf00-14   mfc2 $9,$10,$11       -> sh 0/6/12(a2)   ; Ergebnis-Spalte 0 nach a2
8002cf18-58   dasselbe fuer Spalte 1 (a1+2 -> a2+2)
8002cf5c-98   dasselbe fuer Spalte 2 (a1+4 -> a2+4)
8002cf9c-b0   lhu 0/4(a1+20) gepackt -> mtc2 $0 ; 0xc8a10008 = lwc2 $1,8(a1+20)
8002cfbc      0x4a480012 = MVMVA sf=1 mx=Rot v=V0 cv=TR ; t' = R_eltern * t_kind + t_eltern
8002cfc4-cc   swc2 $25,$26,$27 -> 0/4/8(a2+20)          ; Ergebnis-Translation
8002cfd0      jr ra
```

⇒ Das ist **`CompMatrixLV`**: `out.R = parent.R · child.R`, `out.t = parent.R · child.t + parent.t`.
Die vier Aufrufe @0x80042E64/74/84/94 laufen alle mit `a0 = a2 = sp+32`, also **in-place
akkumulierend** auf derselben Stack-MATRIX. Nach dem vierten Aufruf trägt
`MATRIX.t[0..2]` @sp+52/56/60 die **Weltposition des Waffen-Knochens**; `t[1]` @sp+56 ist die
Mündungshöhe, die @0x8004718C (`lw a0,4(s4)`) ins fünfte Tor geht.

Die Kind-Zeiger sind `s2 + 24 / 1572 / 1744 / 1916` mit `s2 = lw 408(s1)` (= Spieler+0x198,
der Teile-Pool). Stride 172 = 0xAC ⇒ Teile **0, 9, 10, 11**, jeweils `+24` = die lokale MATRIX
des Teils. (Die Stride-172-Rechnung ist dieselbe, die im Trefferboxen-Zensus §1.1 die zwölf
Fremd-Stores entlarvt hat.)

### 1.2 Sind 0/9/10/11 Wurzel/Schulter/Arm/Waffe? — aus den Pose-Daten

`emr_hier.py` liest die EMR nach dem Layout des Port-Parsers
(`re15_port/engine/src/emd_common.c:143-183`).

`re15_port/shared_assets/PSX/PLD/PL00.EMR` (RE1.5 Leon, 15 Bones):

| Bone | Kinder | Bind-Offset (x,y,z) rel. Eltern |
|---|---|---|
| 0 | 1, 8, 9, 12 | (0, **−1804**, 0) |
| 8 | — | (−98, −704, 0) |
| **9** | 10 | (−59, **−692**, **−369**) |
| **10** | 11 | (−11, **+422**, −58) |
| **11** | — | (4, **+408**, −37) |
| 12 | 13 | (−59, −692, **+369**) |
| 13 | 14 | (−11, +422, **+58**) |
| 14 | — | (4, +408, **+37**) |

`Eltern: {1:0, 2:1, 3:2, 4:3, 5:1, 6:5, 7:6, 8:0, 9:0, 10:9, 11:10, 12:0, 13:12, 14:13}`
⇒ **Kette zu Bone 11: 11 ← 10 ← 9 ← 0.** Genau die vier, die RE2 verkettet — und *nur* die
vier, weil 9 ein DIREKTES Kind der Wurzel ist. Die vier `jal`s sind also die vollständige
Ahnenkette, kein Auszug.

**Der Beweis der Rolle steckt im Spiegel:** 9/10/11 und 12/13/14 tragen paarweise dieselben
Beträge mit **umgekehrtem Z-Vorzeichen** (±369 / ±58 / ±37). Zwei spiegelbildliche Dreierketten,
die 692 über der Wurzel ansetzen und von dort 422 + 408 nach unten hängen = **rechter und linker
Arm**, Ansatz = Schulter (9/12), Mitte = Ellenbogen (10/13), Ende = Hand (11/14). Bone 8
(−98, −704, 0, kinderlos, mittig) ist der Kopf. Namen braucht es dafür nicht.

**Gegenprobe an RE2s eigenem Leon** (`info/re2leon/PL0/PLD/PL00/PL00.emr`): identische
15-Bone-Hierarchie, identische Kette `11 ← 10 ← 9 ← 0`, Wurzel (0,−1810,0), Schulter
(−59,−692,∓369), Arm (−11,+485,∓80), Hand (4,+454,∓45). Dieselbe Anatomie, leicht längerer Arm.

**Bindpose-Höhe der Mündung über den Füßen** (PSX-Y zeigt nach unten, also `−Σ y`):
* RE1.5: 1804 + 692 − 422 − 408 = **1666**
* RE2:   1810 + 692 − 485 − 454 = **1563**

---

## 2. ⛔ DIE KERNFRAGE: was bestimmt die senkrechte Zielrichtung?

**Antwort: die Spielereingabe.** Drei diskrete Bänder, kein stetiges Neigen, kein Auto-Ziel.

### 2.1 Das Band lebt in `+0x154` Bit 15/14/13

Vollscan aller Stores auf +0x154 in `info/re2leon/PSX.EXE`
(`scan_off.py … 340 sh,sb,sw` → **48 Treffer**, mit `ctx_stores.py` nach den Bit-Operationen
davor sortiert). Die Band-Schreiber sind daran zu erkennen, dass sie erst `andi 0x1fff`
(alle drei Bandbits weg) und dann `ori 0x2000 | 0x4000 | 0x8000` machen:

| Adresse | Band | Auslöser |
|---|---|---|
| @0x800432EC-FC | `0x4000` EBEN | **Eintritt** ins Zielen (Phase 0, zusammen mit `+0x14C = 0x00070009`) |
| @0x8004359C-A8 | `0x2000` TIEF | Pad **0x20** (@0x80043588), wenn Tabelle @0x800A6D78[3·(state−1)] & 0x7f < Waffen-Id |
| @0x800435F4-600 | `0x8000` HOCH | Pad **0x10** (@0x800435E0), Tabelle @0x800A6D79 |
| @0x80043740-4C | `0x8000` HOCH | Pad **0x10** (@0x80043718), Halte-Schleife |
| @0x80043774-80 | `0x2000` TIEF | Pad **0x20** (@0x80043720/34) |
| @0x800437A8-B4 | `0x4000` EBEN | **weder noch** (`andi v0,s1,0x30` @0x80043754 → @0x80043784) |
| @0x80047AD8-E4 / @0x80047B0C-18 / @0x80047B40-4C | 0x8000 / 0x2000 / 0x4000 | zweite Waffen-FSM, Bauplan identisch |

Die Zustandsmaschine im Wortlaut (@0x8004371C-B4):

```
8004371c: beq (pad & 0x10)==0 -> 0x80043750     ; HOCH nicht gedrueckt
80043724-30: +0x154 & 0x8000 != 0 -> 0x80043750 ; schon HOCH, nichts tun
8004373c: sb zero,7(s0)                          ; Phase 0 = Clip neu einsteigen
80043740-4c: +0x154 = (&0x1fff) | 0x8000         ; HOCH
80043750: beq (pad & 0x20)==0 -> 0x80043784      ; TIEF nicht gedrueckt
80043758-70: +0x154 & 0x2000 != 0 -> 0x80043784
80043774-80: +0x154 = (&0x1fff) | 0x2000         ; TIEF
80043784: bne (pad & 0x30)!=0 -> 0x800437d0      ; eins von beiden haelt -> Band bleibt
8004378c-98: +0x154 & 0x4000 != 0 -> 0x800437d0
800437a8-b4: +0x154 = (&0x1fff) | 0x4000         ; EBEN
```

**Dass 0x10/0x20 wirklich HOCH/RUNTER sind**, steht im selben Register `s1`/`s2` (= `a3`,
das vierte Argument): @0x80043508 `pad & 0x8` → Yaw **−32**, @0x80043524 `pad & 0x2` → Yaw
**+32** (links/rechts drehen), @0x800436E4 `pad & 0x100` → gehalten = weiterzielen, sonst
Sub 3 = Waffe senken (R1), @0x800437E4 `pad & 0x40` = Feuer. Das ist Zeile für Zeile die
**virtuelle Pad-Wortbelegung**, die der Port schon byte-true führt
(`re15_port/engine/src/pad_common.c:28-36`, FUN_80030444 @0x800304B8-E4, Preset 0 @0x80073DBC):
Bit 0 HOCH · 1 RECHTS · 2 RUNTER · 3 LINKS · **4 HOCH** · **5 RUNTER** · 6 SQUARE · 8 R1.
0x10 = Bit 4 = HOCH, 0x20 = Bit 5 = RUNTER. ✔

### 2.2 Was das Band bewirkt — ZWEI Verbraucher, und nur einer zählt

**(a) Es wählt den Schuss-Clip.** Im Schuss-Pfad `FUN_80042C64`:

```
80042c90-cbc  12 B von DAT_80011010 -> sp+16 (LWL/LWR/SWL/SWR) + lh 8(a1) -> sh 24(sp)
80042cf8      lhu v0,340(s1)          ; +0x154
80042d08      srl v0,v0,13            ; die drei Bandbits als INDEX
80042d0c-10   sll 1 ; addu sp+16
80042d1c      lh v0,0(v0)             ; Tabelle[index]
80042d14/24   lui a0,0x7 ; addu       ; | 0x00070000  (Bank 7)
80042d28      sw v0,332(s1)           ; +0x14C = Bank 7, Clip
```

Tabelle @0x80011010, selbst gelesen (`re2_disasm.py read 0x80011010 12 --w 2 --signed`):
`{0, 14, 10, 0, 12}` — als **Maske** gelesen (nicht als Zähler):

| +0x154>>13 | Bandbit | Richtung | Schuss-Clip |
|---|---|---|---|
| 1 | 0x2000 | **TIEF** | **14** |
| 2 | 0x4000 | **EBEN** | **10** |
| 4 | 0x8000 | **HOCH** | **12** |
| 0 / 3 | — | ungültig | 0 |

**(b) Es wird nach word0 Bit 31/30/29 gestempelt** — und dort **niemals gelesen**:

```
80042d44/50  lui a0,0x1fff ; ori 0xffff      ; Maske 0x1FFFFFFF
80042d4c/64  lhu v0,340(s1) ; andi v0,0xe000
80042d68     sll v0,v0,16
80042d6c-74  word0 = (word0 & 0x1FFFFFFF) | (band << 16)
```
Das ist wörtlich RE1.5s Elevations-Band (UP/LEVEL/DOWN, Bit 31/30/29), das der Port in
`re15_band_stamp_aa4` (`re15_damage.c:1228ff`, FUN_80012AA4) führt. **In RE2 hat es keinen
Leser**: `python analysis/befunde_2026-09-27/scan_imm.py e000 info/re2leon/PSX.EXE` liefert **keine einzige
`lui`-Zeile**, also
existiert die Konstante `0xE0000000` in der ganzen RE2-EXE nicht, und damit auch nicht RE1.5s
`enemy.word0 & player.word0 & 0xE0000000`-Tor (@0x800120D0-EC). ⇒ **In RE2 wirkt das Band
AUSSCHLIESSLICH über den Clip — also über die Pose — und die Pose wirkt über die Mündungshöhe
im fünften Tor.** Das ist die Mechanik, die Runde 30/31 gesucht hat.

### 2.3 Ein AUTO-Band nach Gegnerhöhe existiert — aber nicht für Leon

@0x80060EE4 (und drei Klone @0x80060DAC / @0x80061030 / @0x800615EC) setzt das Band aus der
HÖHENDIFFERENZ zum erfassten Ziel:

```
80060f4c  lw v0,60(a2)   ; Ziel-Y  (+0x3C)
80060f50  lw v1,60(s0)   ; Selbst-Y
80060f60  subu a1,v0,v1  ; dY = ZielY - SelbstY
80060f6c-74  (dY+4999) & 0xffff < 0x1b57 (6999)  -> dY in [-4999,+1999] : Ziel erfasst,
             Zustand 0x501, +0x1B4 = Ziel, Band = 0x4000 EBEN   (@0x80060f84-9c)
80060f94  (dY+4999) < 0xbb7 (2999)   -> dY in [-4999,-2001] : Band = 0x8000 HOCH  (@0x80060fa8)
80060fac  (dY+499)  < 0x9c3 (2499)   -> dY in [ -499,+1999] : Band = 0x2000 TIEF  (@0x80060fd0)
```

Wem das gehört: `scan_word.py … 0x80060ee4` (und `scan_jal.py 80060ee4 info/re2leon/PSX.EXE` = 0 direkte `jal`) findet den Zeiger **genau einmal**, @0x800A8948, in
einer Handler-Tabelle ab 0x800A8930, deren Einträge alle in 0x80060xxx–0x80061xxx liegen. Leons
Handler stehen woanders — `scan_word.py … 0x8004326c` (die Aim-FSM aus §2.1) findet sechs
Einträge @0x800A702C/7048/7068/7080/7098/70B0, eine andere Tabelle. ⇒ Das Höhen-Auto-Band gehört
einem **NPC-Schützen**, nicht dem Spieler. Für Leon gibt es **kein** Auto-Ziel in der Senkrechten.

### 2.4 Zwei Nebenbefunde derselben Stelle

* **Waffenlauf-Versatz je Waffe** @0x80042E2C-54: SVECTOR `(0, 0, 300 − 15·(WaffenId−5))`
  (`lbu 333(s1)` = +0x14D; `sll 4`/`subu` = ·15) durch `FUN_8008DBA4` mit der Spieler-MATRIX.
  Rein in Z (vorwärts), **nicht** in der Höhe.
* **Mündung 200 tiefer für Waffen-Id 7…11** @0x80042F60-6C (`lw 56(sp)` / `addiu v0,v0,200` /
  `sw`), danach zurückgestellt @0x80042FAC-B8. PSX-Y wächst nach unten ⇒ +200 = 200 **tiefer**.
  Nur für den Tor-Aufruf, nicht für den Rest.

---

## 3. WELCHE MÜNDUNGSHÖHEN SIND NÖTIG — und was die Pose liefert

### 3.1 Die Forderung, aus der Fenstertabelle ausgerechnet

Tor (selbst gelesen @0x8004716C-A4, Herleitung in `zielfenster-messung.md` §1):
`MündungY ∈ [eY + b − h − 99, eY + b + h + 100]`. Steht der Gegner auf derselben Ebene
(eY = Bodenhöhe) und ist **M** die Mündungshöhe ÜBER dem Boden, dann ist `Hgun = M`:

| Ziel | b | h | erlaubtes **M** | Deckel |
|---|---|---|---|---|
| Zombie stehend | −1500 | 1500 | [−100, **3099**] | 3099 |
| Zombie Kriecher | −350 | 350 | [−100, **799**] | **799** |
| Hund stehend | −1000 | 1000 | [−100, **2099**] | **2099** |
| Hund liegend | −500 | 500 | [−100, **1099**] | 1099 |
| Krähe sitzend (+0x1F0 < 900) | −350 | 530 | [−280, **979**] | 979 |
| Spinne 0x25 | 0 | 1400 | [−1500, **1499**] | 1499 |
| Baby 0x26 | −10 | 10 | [−100, **119**] | **119** |
| Krähe fliegend (+0x1F0 ≥ 900, Flughöhe H) | 0 | 530 | H ∈ (M−630, M+630] | — |

**Gibt es EINE Höhe für alle?** Rechnerisch ja: der Schnitt aller Boden-Fenster ist
**[−100, 119]** — 220 Einheiten breit, auf **Knöchelhöhe**. Ohne das Baby: **[−100, 799]**,
also Wadenhöhe. Beides liegt 1547 bzw. 867 Einheiten unter der gemessenen Bindpose 1666 und
weit unter jeder stehenden Schützenhaltung. ⇒ **Die Zahl existiert, die Pose nicht.**
Die Mündung MUSS wandern — und zwar **nach UNTEN**, um mindestens **867** (Kriecher) bis
**1547** (Baby) Einheiten gegenüber der heutigen Höhe.

### 3.2 Was die echte Zielpose liefert — GEMESSEN

`zielpose_fk.py` rechnet dieselbe Vorwärtskinematik wie der Port, mit denselben Zahlen:
Hierarchie + Bind aus `PL00.EMR`, Keyframes aus der W-Bank (`PL00W03.EMR` hat
`bones_table = 0` und ist ein reiner Keyframe-Strom — genau die Komposition „PL00-Knochen +
W-Pool", die der Renderer baut, `platform/pc/main.c:7423-7431`), 12-Bit-Euler 36 Bit/Bone ab
Keyframe-Byte +12 (`emd_common.c:219-283`), `mat3_from_euler` = RE1.5-`RotMatrix` @0x80068130
(`skeleton_common.c:134-160`), sin/cos aus `re15_trig_lut.c` (DAT_800794C4), Verkettung
`parent_rot·bind + parent_trans` (`skeleton_common.c:717-729`). Höhe = `−trans[1]`.

`python analysis/befunde_2026-09-27/zielpose_fk.py re15_port/shared_assets/PSX/PLD PL00W03`:

```
Bank PL00W03: 14 Clips, 248 Keyframes (Groesse 80 B)
PL00-Bindpose Bone 11 ueber den Fuessen: 1666
Clip  Bilder Muend.min Muend.max Wurzel-py
6     10     1684      2500      1761..1802     RAISE   (Waffe heben)
8     1      2500      2500      1761..1761     HALTEN  EBEN
10    1      2751      2751      1761..1761     HALTEN  HOCH
12    1      1988      1988      1761..1761     HALTEN  TIEF
7     23     2430      2550      1758..1761     RUECKSTOSS EBEN
9     24     2694      2751      1695..1761     RUECKSTOSS HOCH
11    24     1963      2123      1760..1769     RUECKSTOSS TIEF
```
(Clip-Rollen aus dem Port: `player_common.c:1029/1038/1089` — halten 8/10/12, Rückstoß 7/9/11.)

### 3.3 Das Urteil je Ziel

| Ziel | Deckel M | Bindpose 1666 (heute) | TIEF 1988 | EBEN 2500 | HOCH 2751 |
|---|---|---|---|---|---|
| Zombie stehend | 3099 | ✔ | ✔ | ✔ | ✔ |
| **Hund stehend** | **2099** | ✔ | ✔ | ⛔ **NEU RAUS** | ⛔ **NEU RAUS** |
| Hund liegend | 1099 | ✖ | ✖ | ✖ | ✖ |
| Zombie Kriecher | 799 | ✖ | ✖ | ✖ | ✖ |
| Krähe sitzend | 979 | ✖ | ✖ | ✖ | ✖ |
| Spinne 0x25 | 1499 | ✖ | ✖ | ✖ | ✖ |
| Baby 0x26 | 119 | ✖ | ✖ | ✖ | ✖ |
| Krähe fliegend | H-Band | (1036, 2296] | (1358, 2618] | (1870, 3130] | (2121, 3381] |

⛔ **Zwei harte Folgerungen:**
1. **Die Zielpose öffnet Krähe und Baby NICHT.** Selbst das TIEF-Band lässt die Mündung auf
   1988 stehen — 1189 zu hoch für den Kriecher, 1009 zu hoch für die sitzende Krähe, 489 zu
   hoch für die Spinne, 1869 zu hoch für das Baby. Die Annahme des Auftrags („schließt Krähe
   und Baby-Spinne mit ab") ist damit **gemessen widerlegt**.
2. **Der Bank-Fix allein wäre eine Verschlechterung.** Der stehende Hund kommt heute (1666)
   durch und fiele bei EBENEM Zielen (2500) heraus — gegen den Nutzer-Befund aus Runde 26
   („im Original sind die Hunde wieder verwundbar, sobald sie stehen").
   ⇒ **RE1.5s Posenhöhen und RE2s Fenster passen nicht zusammen.** Die Fenster stammen aus RE2,
   die Pose aus RE1.5. Solange RE2s EIGENE Mündungshöhe nicht gemessen ist, darf keins von
   beidem gegen das andere geriegelt werden.

Die einzig treffbare Öffnung, die die Pose WIRKLICH bringt: die **fliegende Krähe**. Ihr
RE2-Flugband ist 901…7199 (`zielfenster`/`trefferboxen-zensus` §5.3), und die drei Posen decken
davon 1358…3381 ab. Der Port hält seine Krähe aber auf 0…250 über dem Boden — das ist der
Zustand, der ihr fehlt, nicht die Pose.

---

## 4. WAS DER PORT HEUTE HAT

| Baustein | Ort | Zustand |
|---|---|---|
| **Senkrechte Eingabe** | `re15_port/engine/src/player_common.c:1034` und `:1053` — `int elev = (pad_bits & RE15_PAD_BIT_UP) ? 1 : (pad_bits & RE15_PAD_BIT_DOWN) ? -1 : 0;` | **fertig, byte-true zu @0x8004371C-B4** |
| Zugriff darauf | `player_common.c:290` `re15_player_aim_elevation()` → −1/0/+1 | fertig |
| **Drei Halte-Clips** | `player_common.c:1038-1041` (`8 + 2·hoch + 4·tief` bzw. auto `9 + 3/6`), Rückstoß `:405/407/1056` (7/9/11 bzw. 7/10/13) | **fertig** — dieselbe Dreiteilung wie RE2s `{14,10,12}` |
| Clip-Ausgabe | `player_common.c:289` `re15_player_aim_clip()` | fertig |
| **Pose mit der Zielbank** | `re15_port/platform/pc/main.c:7423-7431`: `p_skel = wact_skel; p_anim = wact_anim; p_clip_override = re15_player_aim_clip();` | **fertig — aber NUR im Renderer** |
| Bone 11 → Welt | `re15_damage.c:1134` `re15_player_gunbone_world` (aus `s_hand_world`/`s_hand_rot` des Renderers) | vorhanden, **headless 0/1200 gültig** (Runde 30 §4) |
| **Mündungshöhe Engine-seitig** | `re15_damage.c:1190` `re15_player_muzzle_world` | ⛔ **posiert die FALSCHE BANK** |
| Fünftes Tor | `re15_damage.c:1764` (Hauptbaum, noch nicht committet), scharf für 0x10/0x20/0x25 | gebaut |
| RE1.5-Höhenband | `re15_damage.c:1228ff` `re15_band_stamp_aa4` (FUN_80012AA4) | fertig, **anderer Mechanismus** (Gegner-seitig, Ring + Höhenindex/1800) |
| Elevations-Geometrie im RE2-Applier | `re15_damage.c:1520` `re15_re2_gun_probe(rid, elev, …)`, `grp = elev<0?0:elev>0?2:1` | fertig |

**Der Defekt, in einer Zeile:** `re15_player_muzzle_world` (`re15_damage.c:1193-1207`) ruft
`re15_player_pl00_skel()` / `re15_player_pl00_anim()` — die **Basis-Bank** — und posiert sie mit
`re15_compute_actor_kf(an, sk, pl, -1, pl->anim_frame)`, also mit dem Clip, in dem der Spieler
*sonst* wäre. Beim Zielen steht `pl->motion` aber auf dem Sentinel `RE15_MOTION_AIM_W`, den nur
der Renderer über die W-Bank auflöst. Ergebnis, gemessen und nachgerechnet:

> `probe_r30b_muendung` maß **MündungY 1665…1671** über alle vier Gegnertypen und 240 Bilder,
> obwohl die Sonde **HOCH/RUNTER tatsächlich drückt** (`elev_pad_for`, Zeile 128-137).
> Die PL00-Bindpose rechnet sich zu **1666** (§1.2). Die beiden Zahlen sind dieselbe:
> der Port liest die **Bindpose**, nicht die Zielpose. Die Streuung von 6 Einheiten ist der
> Rest-Keyframe der Leerlauf-Animation.

---

## 5. RISIKO — die Mündungshöhe geht in JEDEN Schuss

`grep -rln "muendung|muzzle_world|gunbone_world|aim_elev|re15_player_aim_clip|Hgun"
re15_port/tests` (Hauptbaum) → 40 Dateien. Nach Wirkung sortiert:

**Riegel, die sich ÄNDERN WERDEN (und deren Erwartung mitgezogen werden muss):**
* `probe_r30b_muendung` — misst die Mündung direkt (MündungY/Hgun/Tor-Urteil je Typ).
* `probe_r30_zielfenster`, `probe_r31_boxen` — dieselbe Messstrecke.

**Riegel, die NICHT kippen dürfen — sie hängen am Hund und am stehenden Zombie, also genau an
den beiden Fenstern, die eine höhere Mündung sprengt:**
* `test_dog_aim_band`, `probe_dog_hit`, `probe_hund_ersttreffer`, `probe_hund_ersttreffer2`,
  `probe_hund_trefferluecke`, `probe_gegen_hundsperre`, `probe_gegen_hundsperre2`,
  `probe_verify_hundsperre`, `probe_verify_hund_abzugzaehlung`
  → Hund stehend, Deckel **2099**. EBEN 2500 bricht sie **alle**.
* `test_1140_straight_shot`, `test_room1140_combat`, `probe_1140_feeder_shot`,
  `probe_r16_trefferhoehe`, `probe_r16_sk_trefferhoehe`, `probe_r16_aufstehen_schuss`,
  `test_r16_aufstehen_schuss`, `test_r16_liegende_unschiessbar`, `probe_r16_liegende_zombies`
  (+ die beiden Skeptiker-Zwillinge), `probe_10d0_situp_re2`, `probe_re2_hitpath`,
  `probe_re2z_bandlock`, `probe_schrot_bauch_y`, `test_writher_hit_feedback`
  → Zombie/Kriecher; der stehende Zombie (3099) hält, der **Kriecher** hängt an der
  RE1.5-Liegeregel und am RE2-Applier, nicht am fünften Tor — der muss unangetastet bleiben.
* `probe_re2_hit_crow_spider`, `probe_re2_crow_shadow` — Krähe/Spinne; heute ungegatet.
* `test_autofire_pin`, `probe_m93r_nachladen`, `probe_r28_messer_dauerschlag` — hängen an
  `re15_player_aim_clip`, nicht an der Höhe; bleiben unberührt, solange die Clip-Wahl bleibt.

⛔ **Die Runde-13/14-Falle in ihrer dritten Gestalt:** Runde 13 = Tor auf ein Feld, das der Port
nie füllt (sperrt dauerhaft). Runde 30 = Tor auf eine Höhe, die immer 0 ist (sperrt nie). Hier
wäre es: Tor auf eine Höhe, die aus der **falschen Engine** stammt (sperrt den Falschen).

---

## 6. Offen / ehrlich benannt

1. **RE2s eigene Mündungshöhe ist weiterhin nicht gemessen.** Die Aim-Keyframes liegen nicht in
   den Bänken, die wir haben: `info/re2leon/PL0/PLD/PL00W03.PLW` ist **3436 B** und enthält nur
   Mesh + TIM (`PL00W03.edd` 12 B, `.emr` 4 B) — RE1.5s Gegenstück hat **19848 B** EMR mit
   14 Clips. RE2s `PL00.edd` hat nur **10** Clips (gemessen: 30/34/113/22/22/20/25/10/25/45),
   `PL00CH` nur 3. Die Clips 9/10/12/14 kommen aus **Pool 0x0007** (`lui a0,0x7` @0x80042D14 /
   @0x800432E4 → `+0x14C = 0x00070000 | clip`); welcher Container das ist, ist nicht aufgelöst.
2. **Die „1668 = 54,5 % der Körperhöhe"-Begründung aus dem Zensus (§5.1) hält nicht.** Die Zahl
   ist die **RE1.5-Bindpose 1666**, kein RE2-Maß; RE2s eigene Bindpose ist 1563. Die Deckung
   mit „Brusthöhe" ist Zufall. Der Zensus-Satz „die Zahl ist also richtig" ist damit
   zurückgezogen — sie ist nur zufällig innerhalb des Zombie- und Hund-Fensters.
3. **Der Sub-Zustand `+0x7` und die Fähigkeits-Tabelle @0x800A6D78** sind nur so weit gelesen,
   wie das Band es braucht: `tbl[3·((+0x10E & 0xFFF)−1) + 0/1] & 0x7f < Waffen-Id` erlaubt
   TIEF/HOCH. Was das gesetzte Bit 0x80 der Tabelleneinträge (133/135/138/158 …) bedeutet und
   wofür das dritte Byte (@0x800A6D7A, gelesen @0x80042DA4) steht, ist **nicht** ermittelt.
4. **Die vier NPC-Auto-Band-Klone** (@0x80060DAC/@0x80060EE4/@0x80061030/@0x800615EC) sind nur
   an einer Stelle vollständig gelesen; dass alle vier dieselben Schwellen tragen, ist aus den
   `ctx_stores.py`-Kontexten (`0x501`, `andi 0x1fff`, `ori 0x2000/0x4000/0x8000`) erschlossen,
   nicht instruktionsweise nachgeprüft.
5. **Kein Bildbeleg.** Diese Welle hat nichts Sichtbares geändert; es gibt kein Vorher/Nachher.
6. **`re15_player_aim_clip()` kann 15 liefern** (`player_common.c:933/1038`, auto-Maschine
   `9 + 6`), aber `PL00W03` hat nur 14 Clips (0…13). Berührt diese Runde nicht, gehört aber
   notiert.

---

## 7. Bau-Nachweis

```
bash re15_port/tools/local_build.sh all
```
Abschlusszeile siehe `analysis/befunde_2026-09-27/zielpose-ermittlung.log`.
In dieser Welle wurde **kein** C-Code angefasst; die Suite ist die unveränderte des
Worktree-Standes.

---

## 8. EMPFEHLUNG für Welle 2 — in dieser Reihenfolge, nicht anders

1. ⛔ **Zuerst RE2s eigene Mündungshöhe messen.** Ohne sie ist jede Riegelzahl geraten. Zwei
   Wege, beide konkret:
   * **statisch** — Pool 0x0007 auflösen: wer füllt `Spieler+0x198` (der Teile-Pool, `lw 408`)
     und woher kommt die Bank zu `+0x14C >> 16 == 7`? Danach `zielpose_fk.py` unverändert auf
     RE2s `PL00.emr`-Hierarchie + diese Keyframes loslassen (das Skript kann das schon, es
     braucht nur die Datei).
   * **dynamisch** — DuckStation-Savestate von RE2 (Leon zielt), `sp+56` am Aufruf @0x80042F94
     lesen, je einmal für EBEN/HOCH/TIEF. Gibt drei Zahlen und beendet die Frage.
2. **Dann erst** `re15_player_muzzle_world` auf die Zielbank umstellen (dieselbe
   Bank+Clip-Wahl wie `platform/pc/main.c:7423-7431`, aber im Engine-Teil, damit sie headless
   UND auf der PSX gilt) — mit einem Riegel, der die drei Höhen festnagelt.
3. **Krähe und Baby NICHT über die Zielpose erwarten.** Gemessen (§3.3):
   * **Krähe** — ihr fehlt die **Flughöhe** (+0x1F0-Band 901…7199, Soll-Höhe +0x224 mit
     ±60-Totzone, @0x801023BC-D0 / @0x80101C1C-3C). Mit ihr ist sie schon bei der heutigen
     Mündung treffbar (H ∈ (1036, 2296]).
   * **Baby 0x26** — Deckel 119. Keine stehende Pose erreicht das. Die Spur aus dem Zensus §7.5
     (radialer Pfad @0x800477CC, nur XZ, kein Y) ist die nächste zu prüfende — dort ist zu
     klären, **wer ihn ruft**.
4. **Nichts davon scharf schalten**, solange der Hund-Riegel (Deckel 2099) nicht mitgerechnet
   ist. Die Reihenfolge ist: messen → Bank umstellen → Riegel je Typ → erst dann das Tor für
   weitere Typen öffnen.
