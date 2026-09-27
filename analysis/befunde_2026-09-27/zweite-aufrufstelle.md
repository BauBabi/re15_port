# Runde 33: die ZWEITE Aufrufstelle des Kandidatenfilters — der Widerspruch ist aufgelöst

`bash re15_port/tools/local_build.sh all` → **`=== LOCAL-BUILD-OK (all) — Tests 357/357`**
(356 alte + die neue `r33_aufrufstelle`), alle vier GUI-Haken im **ersten** Lauf grün.
Messprotokoll: `analysis/befunde_2026-09-27/zweite-aufrufstelle.log`.

---

## 0. Kurzurteil

| | |
|---|---|
| **Teil+0x5C** | Die **Translation `t[0..2]` der WELT-Matrix von Teil 20** (Teil 17 bei Entity-Typ 14) im Teilepool `+0x198` des Spielers. Teil 20/22/24 sind **Projektil-Arbeitsplätze**, keine Mündung. `+0x60` = `t[1]` = die **Welt-Y des fliegenden Geschosses**, und sie wandert. |
| **Zuordnung** | **(A) @0x80042F94 = WAFFE 1 (Messer).** **(B) @0x800467C0 = WAFFE 12** (drei Bolzen gleichzeitig). **Keine Schusswaffe** betritt `FUN_800470C0` — sie laufen über `FUN_800410CC` (@0x80043AFC), das `+0x98/+0x9E` **nie** liest. |
| **+0x14D** | Aufgelöst: die **BILDNUMMER im Clip**, kein Waffenfilter. `(+0x14D)−7 ∈ [0,4]` = die Bilder 7…11 des Messerschlags. |
| **Widerspruch** | **Aufgelöst, ohne eine einzige geratene Zahl.** Der stehende Hund fällt bei EBEN (2500) nur dann aus dem Fenster `[−100, 2100)`, wenn man die Zielpose in ein Tor speist, das RE2 **für Schüsse überhaupt nicht durchläuft**. |
| **Gebaut** | Die **Haltungsklasse `(word0>>26)&7`** des RE2-Hundes aus `FUN_80104088` @0x80104090-D8 — die Hälfte, die Runde 30 ausdrücklich als „nicht gebaut" notiert hatte. Liegend **1**, stehend **3**, gemessen 0 Abweichungen in 400 Bildern. |
| **NICHT gebaut** | Der Mechanismus-Wechsel (Tor 5 raus vom Schusspfad, `FUN_800410CC`-Zwilling für den Hund rein). Grund mit Zahl in §6 — nicht „später", sondern benannt. |

---

## 1. Was Teil+0x5C ist — Vollscan und Instruktionen

### 1.1 Wo der Zeiger herkommt

`info/re2leon/PSX.EXE`, `FUN_80046304` (Rahmen −176), selbst disassembliert:

```
80046368: lw    a0,408(fp)    ; a0 = Teilepool des Spielers (+0x198)
80046374: lbu   v1,8(fp)      ; Entity-Typ
8004637c: bne   v1,14 -> 80046388
80046380: addiu s3,a0,3440    ; DELAY, immer:  Teil 20   (3440 = 20*172)
80046384: addiu s3,a0,2924    ; nur Typ 14:    Teil 17   (2924 = 17*172)
80046394: addiu s2,s3,154     ; s2 = Teil + 0x9A
80046398: addiu s5,s3,92      ; s5 = Teil + 0x5C   <-- erstes Argument von FUN_800470C0
8004639c: addiu s4,s3,72      ; s4 = Teil + 0x48
```

`s4 = Teil+0x48` ist eine **PsyQ-MATRIX** (3×3 `short` = 18 B, 2 Pad, `long t[3]`). Deren
`t[0]` liegt bei `+0x48 + 20 = +0x5C` — **genau `s5`**. Das ist kein Zufallstreffer, sondern
die Ablage, die der allgemeine Skelett-Komponierer benutzt: er legt die **Welt**-Matrix je
Teil nach `+72 + 172·k` (@0x80019100-64, Zitat im Runde-32-Dossier §1.1).

Gegenprobe aus derselben Funktion — `s4` wird als **MATRIX** weitergereicht:
`80046728: jal 0x80047858 (a0 = Trefferergebnis, a1 = s4, a2 = 0x800A6B88)` @0x800467DC.

### 1.2 Wer +0x5C…+0x64 schreibt

**Schreiber 1 — die Komposition (Spawn).** `FUN_80045E78` (Aufrufer: `FUN_80044044`
@0x80044068/74/80) setzt den Platz auf:

```
80045ee4: sw s3,0(s1)            ; Teil-Flags = 1
80045ee8: sw v0,-104(s0)         ; Teil+0x2C = 200
80045ef4: sw v0,-100(s0)         ; Teil+0x30 = 912
80045efc/f04/f18: sw …,-96(s0)   ; Teil+0x34 = 0 / +100 / -100   (a1 = 0/1/2)
80045f24-34: sw 4096/0/4096/0/4096 nach Teil+0x18..+0x28   ; LOKALE Matrix = Identität
80045f1c: lw a0,-32(s0)          ; Teil+0x74 = Zeiger auf die ELTERN-Matrix
80045f20: addiu a2,s1,72         ; Ziel = Teil+0x48  (die WELT-Matrix)
80045f34: jal 0x8002ce94         ; Compose(Eltern, lokal, Welt)
80045f5c: sw v1,-32(s0)          ; Teil+0x74 = Pool+1964 = Teil 11 + 0x48  (Waffenhand)
```

Das heißt: **die Startlage des Geschosses ist die Welt-Matrix der Waffenhand (Teil 11)**,
über eine Identitäts-Lokalmatrix hineinkomponiert. Danach gehört sie dem Geschoss.

**Schreiber 2 — die Integration je Bild** (`FUN_80046304`, Zustand 0):

```
80046524-3c: ApplyMatrixLV(s4, sp+16 = SVECTOR(0, 800, 0), sp+24)
80046544-54: lh 24(sp) ; lw -62(s2)  (= Teil+0x5C) ; addu ; sw -62(s2)
80046558-68: lh 26(sp) ; lw -58(s2)  (= Teil+0x60) ; addu ; sw -58(s2)
8004656c-7c: lh 28(sp) ; lw -54(s2)  (= Teil+0x64) ; addu ; sw -54(s2)
80046590-c8: bei Kollision (sp+120 == 0) wird derselbe Schritt wieder SUBTRAHIERT
```

Also **Position += 800 Einheiten entlang der eigenen lokalen Y-Achse, je Bild.**

**Schreiber/Leser 3 — Boden und Grenzen**, dieselben drei Langworte:

```
80046734-50: lw -58(s2)  <  *(0x800d5be4)            -> Zustand 2   (BODEN)
80046754-70: (lw -62(s2) + 30000) >u 60000           -> Zustand 2   (X ausserhalb ±30000)
80046774-90: (lw -58(s2) + 30000) >u 60000           -> Zustand 2   (Y)
80046794-b0: (lw -54(s2) + 30000) >u 60000           -> Zustand 2   (Z)
800467b4-c4: a0 = s5, a1 = lh -28(s2), a2 = sp+96, a3 = 12 -> jal FUN_800470C0
800467e8-f0: lw -62(s2), lw -54(s2) -> jal 0x800527b4        (X/Z, Richtung)
```

> **Ergebnis: `Teil+0x5C/+0x60/+0x64` ist ein VEKTOR aus DREI Langworten — die Welt-Position
> eines fliegenden Geschosses.** `+0x60` trägt zur Laufzeit dessen **Y**, und die kommt
> zuerst aus der Waffenhand (Teil 11) und wandert dann mit 800/Bild davon. Es ist **keine
> Mündungshöhe** und schon gar keine Konstante.

Drei Plätze gleichzeitig: `addiu s0,s0,344 / addiu s1,s1,344` @0x80045F6C-70 (344 = 2·172),
Index bei Teil 11 +132 mit Umlauf bei 3 (@0x80045F80-A4) ⇒ **Teile 20, 22, 24**.
`FUN_80044044` feuert alle drei in einem Bild (`a1 = 0/1/2`, Seitenversatz `+0x34` = 0/+100/−100)
und spielt den Waffen-SE dreimal.

---

## 2. Welche Aufrufstelle für welche Waffe — die Dispatch-Kette

Selbst gedumpt (`scan_ptr.py`, `re2_disasm.py table`), Schritt für Schritt:

```
80043e20: lhu v0,270(s0)        ; +0x10E
80043e30: andi v0,v0,0xfff      ; = die WAFFEN-ID
80043e34: sll v0,v0,2
80043e40: lw  v0,0x6f38(at)     ; Tabelle @0x800A6F38, indiziert mit der Waffen-Id
80043e48: jalr v0
```

| Waffe | Verteiler | Zeile | Feuer-Handler (Zeile[2]) | ruft |
|---|---|---|---|---|
| **1** | 0x80042C28 | @0x800A702C | **0x80042C64** | **`jal FUN_800470C0` @0x80042F94** |
| 2, 3, 5…13, 17, 19, 20 | 0x80043230 | @0x800A7048 | 0x80043908 | `jal FUN_800410CC` @0x80043AFC |
| 4 | 0x8004879C | @0x800A7098 | 0x800487D8 | `jal FUN_800410CC` @0x80048A04 |
| 14 | 0x800482D8 | @0x800A7080 | 0x80048314 | `jal FUN_800410CC` @0x8004845C |
| 15, 16 | 0x8004799C | @0x800A7068 | 0x80047C6C | `jal FUN_800410CC` @0x80047EF4 |
| 18 | 0x80048B50 | @0x800A70B0 | 0x80048B8C | `jal FUN_800410CC` @0x80048D48 |

Der Verteiler selbst wählt über `lbu v0,6(a0)` @0x80042C30 den Eintrag der Zeile;
`FUN_80042C64` steht auf **Index 2** (@0x800A7034).

**(B)** hängt an keiner Zeile: `FUN_80046304` wird direkt aus dem Spieler-Root
`FUN_8003BFAC` aufgerufen, und nur dann:

```
8003c1c8: lhu v0,270(s0)
8003c1d0: andi v0,v0,0xfff
8003c1d4: bne v0,12 -> 8003c1e4
8003c1dc: jal 0x80046304
```

### 2.1 Waffe 1 **ist** das Messer — vier unabhängige Belege

1. **Schadenszeile** @0x800A412C + (id−1)·20: Zeile 1 beginnt mit **3**, jede andere Zeile mit
   ≥ 11 (Zeile 2 = `0x00E03C10` = 16/15/14, Zeile 7 = Schrot 200/60/40). Selbst gelesen,
   `--w 2 --signed`, 20 Zeilen.
2. **Feuer-SE** @0x800A6F90[1] = `0x80043ECC` = **`jr ra`** — kein Schussgeräusch. Alle
   Schusswaffen-Einträge rufen `jal 0x8006A0CC(weaponId, 0)`.
3. **Hitcode** an (A): `a3 = 0x00020001`, mit Zielband-Bit 0x2000 `a3 = 0x00060001`
   (@0x80042F64-88) — low byte **1**; der Zensus der Runde-D (im Kopf von `re15_damage.c`)
   liest daraus schon „Zeile 1".
4. **Geometrie**: der Keil an (A) ist `{−200, 0, 250, 125}` (@0x8001101C, nach `sp+72`
   kopiert @0x80042CC8-E4) — Reichweite 250, Halbbreite 125. Eine Kugel braucht Tausende.

### 2.2 Waffe 12 ist eine **Bolzen-Salve**

Drei Geschosse in einem Bild, Seitenversatz 0/+100/−100, 800 Einheiten/Bild, Bodenkontakt,
Grenzen ±30000, eigener 7-Zustands-Automat über `Teil+0x98` (Sprungtabelle @0x80011044).
Hitcode 12 (@0x800467C4).

---

## 3. +0x14D ist die BILDNUMMER, nicht die Waffe

Der Punkt, an dem Runde 32 hängen blieb. Selbst disassembliert:

```
80029b28: lbu   v0,333(s2)      ; +0x14D
80029b30: addiu v0,v0,1
80029b34: sb    v0,333(s2)      ; JE BILD +1, direkt hinter der Teile-Schleife
80029b3c: sltu  v0,v0,s3        ; und gegen die Clip-Laenge geprueft
--
80015ddc: lbu   v0,333(t1)
80015dfc: sll   v0,v0,2
80015e00: addu  t0,v1,v0        ; KEYFRAME-Index
--
80042d14: lui   a0,0x7
80042d24: addu  v0,v0,a0
80042d28: sw    v0,332(s1)      ; +0x14C = Clip | 0x00070000  -> +0x14D = 0 (Reset)
--
80042da4: lbu   v0,28026(at)    ; Tabelle @0x800A6D7A
80042dac: andi  v0,v0,0x7f      ; Clip-LAENGE
80042db0: sltu  v0,v0,v1        ; Laenge < +0x14D  ->  +0x06 = 1 (Angriff zu Ende)
--
80042df4: sltiu v0,v0,0x15      ; +0x14D < 21
80042e3c: addiu v0,v0,-5
80042e40/44: sll v1,v0,4 / subu v1,v1,v0    ; 15*(+0x14D - 5)
80042e48/4c: addiu v0,zero,300 / subu v0,v0,v1   ; z = 300 - 15*(Bild-5)
```

Also: **`+0x14C` = Clip, `+0x14D` = Bild im Clip, `+0x14E` = Bank (7).**
`(+0x14D)−7 ∈ [0,4]` = **die Bilder 7…11 des Messerschlags** — das aktive Fenster, in dem
allein die `−200`-Absenkung UND `jal FUN_800470C0` stattfinden
(`beq v0,zero,0x80042FBC` @0x80042F58 überspringt beides).

Das passt zum RE2-Muster des Messers, das der Port schon gedumpt hat
(@0x800A6608/6434/656C: `ff/6 00/1 01/1 02/1 03/1 04/1 00/255` = sechs Bilder Ausholen,
dann fünf Treffbilder).

> Die Runde-32-Notiz „ein Schreiber legt dort `7·x` ab (@0x8003D790-B4)" bleibt richtig —
> sie steht nur in einem **Gegner**-Kontext (dort wird das Byte @0x8003D748-84 durch 7
> geteilt). Dasselbe Byte, anderes Objekt. Genau die Falle „zwölf von 48 Stores gingen auf
> ein Modellteil statt auf den Gegner".

---

## 4. Der Widerspruch — mit Zahlen entschieden

**Frage:** welche Zielhöhe gilt für den stehenden Hund wirklich, und fällt er aus seinem
Fenster?

**Antwort:** für einen **Schuss** gilt **gar keine** — das fünfte Tor liegt nicht im Pfad.

Beleg, Vollscan `lh/lhu rt,152|158(rs)` über `info/re2leon/PSX.EXE`; im gesamten Bereich
`0x8004xxxx` gibt es **sechs** Leser von `+0x98/+0x9E`, und alle sechs liegen in zwei
Funktionen:

```
80047170 / 80047188   FUN_800470C0  — Tor 5
800472ac / 800474d0   FUN_800470C0  — die Zonen-Rechnung
80047730 / 80047744   FUN_80047664  — der AoE-Zwilling (Aufrufer alle in 0x8002xxxx:
                                      @0x800200E0/21BA0/21E48/224E4/23BC4/2442C = Explosionen)
```

`FUN_800410CC`, durch das jede Schusswaffe geht, liest sie **nicht**. Es entscheidet über

```
800413c4: lw   v0,0(s2)        ; word0 des Kandidaten
800413cc: srl  v0,v0,26
800413d4: andi s6,v0,0x7       ; HALTUNGSKLASSE
800413d8: beq  s6,zero,0x80041774     ; 0 -> Kandidat faellt raus
800413e0-18: Hoehenfenster ueber FUN_80041B20 (dy = Gegner+0x3C − Spieler+0x3C)
8004142c-54: Hoehen-Stufe s0 = 6 / 3 / 0
80041488/a8/c8: `and v0,v1,s6` gegen die Maskentabelle @0x800A6DB4
```

Die Maskentabelle, selbst gedumpt (27 B ab 0x800A6DB4):
`4,2,1 | 2,1,4 | 1,2,4 || 4,2,0 | 2,0,0 | 1,2,0 || 0,0,0,0,0,0,0,0,0`.
Trifft **keiner** der drei Maskenbytes die Haltungsklasse, wird der Treffer nicht gezählt.

**Damit lösen sich beide Hälften des Widerspruchs auf:**

| | |
|---|---|
| „Zielpose EBEN 2500 fällt aus `[−100, 2100)`" | Richtig gerechnet, aber **gegenstandslos**: mit der Pistole durchläuft der Hund dieses Tor nicht. Die Zielpose ist die Pose der **Waffe**, das Tor gehört dem **Messer** und dem **Bolzen**. |
| „byte-true + byte-true = falsch, also fehlt ein Dritter" | Der Dritte ist **`FUN_800410CC` mit der Haltungsklasse**. Er ist gefunden, und seine Eingangsgröße ist ab dieser Runde im Port. |

Und für die beiden Stellen, an denen RE2 das Tor **wirklich** fährt:

* **(A) Messer:** die Zielhöhe ist die verkettete Hand-/Klingenmatrix `t[1]`, **plus 200**
  (@0x80042F68, PSX-Y zeigt nach unten ⇒ 200 tiefer), und nur in den Bildern 7…11.
* **(B) Bolzen:** die Zielhöhe ist `Teil+0x60` — die **Welt-Y des Bolzens selbst**. Sie
  startet an der Waffenhand und sinkt/steigt mit dem Flug. Ein Bolzen, der auf Hundehöhe
  ankommt, steht **in** dessen Fenster; genau dafür ist das Tor da.

⇒ **Der stehende Hund fällt nirgends aus seinem Fenster.** Nicht beim Schuss (kein Tor),
nicht beim Bolzen (der Bolzen ist auf seiner Höhe), und beim Messer nur dann, wenn die
Klinge senkrecht danebenliegt — was genau die Absicht ist.

---

## 5. Was gebaut wurde

### 5.1 Die Haltungsklasse des RE2-Hundes (neu)

`FUN_80104088` in `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN`, **alle 22 Instruktionen selbst
gelesen**:

```
80104088: bne   a1,zero,0x801040b8
80104090: lui   a0,0xe7ff
80104094: ori   a0,a0,0xffff
80104098: addiu v0,zero,-500      / 8010409c: sh v0,152(a2)     ; +0x98 = -500
801040a4: addiu v1,zero,500       / 801040a8: sh v1,158(a2)     ; +0x9E =  500
801040ac: lui   v1,0x400
801040b4: and   v0,v0,a0          ; word0 &= 0xE7FFFFFF   (loescht Feldbits 1|2)
801040b8: addiu v0,zero,-1000     / 801040bc: sh v0,152(a2)
801040c0: addiu v0,zero,1000      / 801040c4: sh v0,158(a2)
801040cc: lui   v1,0xc00          ; OHNE Maskierung
801040d0: or    v0,v0,v1
801040d8: sw    v0,0(a2)
```

Feld = `(word0>>26)&7`, also Bit0 = `0x04000000`, Bit1 = `0x08000000`, Bit2 = `0x10000000`:

* `a1 == 0` (gestauchte Box): Feld **= 1**
* `a1 != 0` (volle Box): Feld **|= 3**

Im Port: `re2d_hitbox()` (`enemy_ai_re2_dog.c`) schreibt jetzt `e->re2z_parts` — dasselbe
Feld, das `re15_re2_gun_probe` (der Port-Zwilling von `FUN_800410CC`) schon liest.

**Gemessen** (`probe_r33_aufrufstelle` Teil 1, 400 Bilder Dauerbeschuss in ROOM1190):
gestauchte Box in **360** Bildern → Klasse genau **1**; volle Box in **40** Bildern → Klasse
trägt **3**; **0 Abweichungen**.

**Wirkung heute: keine** — und das steht als Zahl da, nicht als Hoffnung.
`re15_re2_gun_probe` läuft nur für `re15_re2z_owns_type` (0x10/0x11/0x12/0x13/0x16/0x18,
`enemy_ai_re2_zombie.c:186`); der Hund 0x20 ist nicht dabei. Die Trefferzahlen aller Typen
sind vor und nach dem Einbau identisch (§5.3).

### 5.2 Drei Berichtigungen im Code

1. **`enemy_ai_re2_dog.c`** — „der **einzige** Leser steht im Zombie-Overlay EMZ0.BIN … für
   den HUND gibt es überhaupt keinen Leser, dort wäre es ein Store ins Leere." Falsch. Der
   Vollscan suchte `lui reg,0x400|0x800|0xc00` + `and` und konnte den allgemeinen Leser
   nicht finden, weil der **schiebt**: `srl v0,v0,26` @0x800413CC.
2. **`re15_damage.c`** — „am **SCHUSS**-Pfad @0x80042F94". Es ist der **Messer**-Pfad; keine
   der beiden Spieler-Aufrufstellen gehört zu einer Schusswaffe.
3. **`re15_damage.c`** — „+0x14D ist nicht aufgelöst, jede Portierung der [7,11]-Klammer wäre
   geraten." Aufgelöst (§3): Bildnummer im Clip.

### 5.3 Was **nicht** gebaut wurde — mit Zahl

* **Die −200-Absenkung** (@0x80042F60-6C / @0x80042FAC-B8). Sie gehört zum **Messer**-Tor.
  Das Messer fährt im Port über `re15_re2_gun_probe` / `s_re2z_geo1`, nicht über dieses Tor;
  die Absenkung dorthin zu schreiben hieße, sie an die falsche Stelle zu hängen. Wirkung auf
  die sieben gemessenen Fälle: **keine** — 200 Einheiten drehen keinen um (Runde 32 §5 hat
  das mit derselben Arithmetik durchgerechnet: 2500 − 200 = 2300 liegt immer noch über der
  Hunde-Oberkante 2100).
* **Das Tor für Krähe 0x21, Spinne 0x25, Baby 0x26** bleibt aus (unverändert seit Runde 30).
  Gemessen bleiben sie treffbar: 40 / 82 / 41 Treffer in 900 Bildern.
* **Die Kriecher-Box −350/350 als `re2_hit_box_set`.** Gemessen (§5.4, Zeile 2): mit ihr ist
  der Zombie über das fünfte Tor **0 mal in 900 Bildern** treffbar (Fenster `[−100, 800)`
  gegen Mündung ~1665). Wer sie scharf schaltet, baut die Runde-13/14-Dauersperre. Als
  Riegel festgenagelt, damit es niemand versehentlich tut.

### 5.4 Der Riegel — `probe_r33_aufrufstelle riegel` → **PROBE-OK**

```
TEIL 1  Hund 0x20: gestauchte Box in 360 Bildern (Klasse 1), volle Box in 40 (Klasse 3),
        Abweichungen 0

TEIL 2  900 Bilder Dauerbeschuss je Typ
  ZOMBIE 0x10 stehend        Treffer   82 / 900   box_set=1
  ZOMBIE 0x10 Kriecherbox    Treffer    0 / 900   box_set=1   (erzwungen -350/350)
  HUND   0x20                Treffer   11 / 900   box_set=1
  KRAEHE 0x21                Treffer   40 / 900   box_set=0
  SPINNE 0x25                Treffer   82 / 900   box_set=1
  BABY   0x26                Treffer   41 / 900   box_set=0

TEIL 3  KONTROLLE mit kuenstlich genullter Box (Fenster [-100,100))
  ZOMBIE 0x10 stehend        Treffer    0 / 300
  HUND   0x20                Treffer    0 / 300
```

Die **Kontrolle** ist der Punkt, an dem diese Messung sich selbst prüft: fiele sie nicht auf
0, läge das fünfte Tor gar nicht im gemessenen Pfad und die Spalte „Treffer" darüber sagte
nichts. Sie fällt auf 0 — beide Male.

Alle bestehenden Riegel bleiben grün, insbesondere `r30b_muendungshoehe` (liegender Hund
untreffbar, stehender treffbar, nach dem Aufstehen wieder treffbar) und `r31_boxen`.
**357/357.**

---

## 6. ⛔ OFFEN — der Mechanismus-Wechsel, benannt statt halb gemacht

Der Port fährt das fünfte Tor heute **für jede Waffe** (in der gemeinsamen
Kandidatenschleife, `re15_damage.c`). RE2 fährt es nur für Messer und Bolzen. Byte-true
wäre also: Tor 5 vom Schusspfad nehmen und den Hund stattdessen an den
`FUN_800410CC`-Zwilling hängen.

**Das ist in dieser Runde bewusst NICHT passiert, und hier ist die Zahl dazu:**
`re15_re2_gun_probe` besitzt heute keinen Hund. Nimmt man dem Hund das Tor weg, ohne ihn
vorher dort anzuschließen, ist er im Liegen sofort wieder beschießbar — Runde 30 hat den
Zustand vor dem Tor gemessen: **17 Treffer in 389 Bildern, während er lag, und er kam in
400 Bildern nie wieder hoch.** Das ist genau der Nutzer-Befund aus Runde 26
(„im Original sind die Hunde erst wieder verwundbar, sobald sie stehen"), und den wieder
aufzureißen wäre ein Rückschritt, kein Fortschritt.

Was für den Wechsel **namentlich** fehlt:

1. `re15_re2_gun_probe` muss den Hund 0x20 annehmen (heute `re15_re2z_owns_type`-gegated).
2. Der Hund braucht sein `+0x9A` (Breitenzuschlag; `re2z_rad9a`) byte-true aus
   `EMD0G_MOD0.BIN` — Vollscan der `sh rt,154(rs)` dort steht aus.
3. Die Höhen-Stufe s0 hängt an `FUN_80041B20` mit den **Fenster-Zeilen des Hundes**;
   der Port hat `s_re2z_fen` nur für die Zombie-Familie belegt.
4. Belegt werden muss, dass die Maskentabelle @0x800A6DB4 den liegenden Hund (Klasse 1)
   bei der Pistolen-Stufe tatsächlich verwirft — sonst wäre der Wechsel eine Verschlechterung.

**Weitere offene Punkte dieser Runde:**

* **Kein lebender Kriecher im Testbestand.** In 1010/1220/1050/10F0/2000/2010 steht beim
  Laden kein 0x10 mit `grid & 0x80`. Die Kriecher-Zeile ist deshalb eine **erzwungene Box**
  und misst das Tor, nicht die Kriech-KI. Steht so in der Sonde.
* **Das Messer-Tor selbst.** RE2 fährt für Waffe 1 `FUN_800470C0` mit dem Keil
  `{−200, 0, 250, 125}`; der Port fährt das Messer über `s_re2z_geo1` (die Records
  @0x800A657C/63A8/64E0 aus der Tabelle @0x800A68E8). Beide Datensätze existieren im
  Original; welcher im Spiel die Messertreffer erzeugt, ist **nicht** entschieden — die
  Records @0x800A68E8+1·24 haben in der EXE keinen von mir gefundenen Aufrufer für Waffe 1.
* **Kein Bildbeleg.** Diese Runde hat ausschließlich gemessen (`re15_game_step` + Pad) und
  disassembliert. Kein `AUTOSHOT`, kein Softwarerenderer, aber auch kein Screenshot —
  es gibt in dieser Runde keine sichtbare Änderung, die man fotografieren könnte.

---

## 7. Werkzeuge dieser Runde

* `analysis/befunde_2026-09-27/scan_ptr.py` — alle 32-Bit-Worte, die auf eine Zieladresse
  zeigen (Dispatch-Tabellen, die kein `jal` haben).
* `analysis/befunde_2026-09-27/scan_immrange.py` — alle Load/Store mit Immediate in `[lo,hi]`.
* `re15_port/tests/unit/probe_r33_aufrufstelle.c` + `probes/r33_aufrufstelle.cmake`.
