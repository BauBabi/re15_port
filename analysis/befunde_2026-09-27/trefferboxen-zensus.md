# Runde 31 / Welle 1: VOLLZENSUS der Trefferboxen + Fahrstuhl-Zensus

Status: **MESSUNG FERTIG**, kein Verhaltens-Code geändert (Auftrag: „DU AENDERST IN DIESER
WELLE KEIN SPIELVERHALTEN"). Einzige Code-Änderung ist eine **Mess**-Korrektur an
`probe_r30b_muendung` (die 0x25-Zeile fehlte, weil sie im falschen Raum suchte).

Nutzer-Auftrag, wörtlich: *„Ja, na klar, fahrstuhl sound muss ueberall bei fahrstuehlen rein.
Der fehlt weil REsident Evil 1.5 eine 40% Beta ist und unvollstaendig. Und die Trefferboxen
wenn die fehlen muessen natuerlich auch ermttelt und uebernommen werden. Das soll alles sauber
Resident Evil 2 entsprechen."*

Werkzeuge dieser Runde (alle in `analysis/befunde_2026-09-27/`): `scan_sh.py` (Vollscan
`sh rt,152/158(rs)`), `whereis.py`, `dumpmany.py`, `scan_op.py`, `scan_lift_all.py`,
`scan_lift_re2.py`, `count_lift.py`.

---

> ## Kurzurteil in vier Sätzen
>
> 1. **48 Schreibstellen** auf +0x98/+0x9E in den fünf Overlays — 26 davon kannte das
>    Welle-1-Dossier aus Runde 30 nicht, und **eine Fundstelle dort war falsch** (die Krähe hat
>    ihren INIT nicht nur @0x801003B8-DC, sondern zusätzlich einen **Neu-Berechner pro Bild**
>    @0x801001EC-208).
> 2. **12 der 48 Stores gehen NICHT auf den Gegner**, sondern auf ein **Modell-Teil**
>    (`lw …,408(base)` = +0x198, Index × **172 = 0xAC**). Dort bedeutet +0x98 einen **Winkel**,
>    nicht eine Box. Runde 30 hatte zwei davon richtig ausgeschlossen; es sind zwölf.
> 3. **Kein Klein-Zustand ist eine Sackgasse.** Der Zombie hat als Gegenstelle eine **Rampe**
>    @0x8010366C-94, die die Box je Bild um 10 aufzieht, gedeckelt bei −1500 (`slti v0,v0,-1499`)
>    ⇒ **115 Bilder** von −350/350 auf −1500/1500. Der Hund öffnet über `FUN_80104088(…,1)`.
> 4. **Aber drei Typen kämen mit ihren ECHTEN Boxen trotzdem 0/240 durch** (Krähe, Spinne,
>    Baby — die Spinne ist **neu gemessen**). Die Ursache ist **NICHT die Mündungshöhe** (die ist
>    maßstabsgetreu belegt, §5.1), sondern **ein fehlender Zustand** — und zwar zweierlei:
>    beim Spieler die **senkrechte Zielpose** (der Port hat sie gar nicht, §5.2), bei der Krähe
>    zusätzlich die **Flughöhe** (RE2-Band **901…7199**, §5.3).
>
> **Fahrstuhl:** RE1.5 hat **drei** Fahrstühle (6 RDT-Dateien), nicht zwei. Der Ton spielt in
> vier von sechs Dateien. **Es fehlt die WAREHOUSE LIFT ROOM3080/3081** (STAGE3) — sie fährt mit
> einem **anderen** Skript und fällt deshalb durch die 32-Byte-Signatur (§6).

---

## 1. Methode und Belegquellen

Alle fünf Gegner-Overlays liegen unter `info/re2leon/COMMON/BIN/` und laden **roh @0x80100000**
(kein 0x800-Header). Der Vollscan sucht das Instruktionswort direkt:
`op = 0x29 (sh)` und `imm ∈ {152, 158}`, also `(w >> 26) == 0x29 && (w & 0xFFFF) ∈ {0x98, 0x9E}`.
Das findet **jede** Setz-Stelle, auch die, die kein Decompilat zeigt.

| Overlay | Datei | Größe | Treffer |
|---|---|---|---|
| 0x10 Zombie | `EMZ0.BIN` | 53068 B | **26** |
| 0x20 Hund | `EMD0G_MOD0.BIN` | 22266 B | **10** |
| 0x21 Krähe | `EMOVL21_S0.BIN` | 19080 B | **6** |
| 0x25 Spinne | `EMS25.BIN` | 26324 B | **4** |
| 0x26 Baby | `EMS26.BIN` | 4346 B | **2** |
| | | | **48** |

Ladeadresse des Hundes gegengeprüft wie beauftragt: `lui at,0x8010` @0x80100064 +
`lw v0,21560(at)` @0x8010006C ⇒ Zustandstabelle @0x80105438.

### 1.1 Die Objekt-Prüfung — zwölf Fremd-Stores, nicht zwei

Runde 26 und Runde 30 sind je an dieser Falle hängengeblieben. Der Test ist mechanisch: geht
die Basis auf `self` (= `a0` der Wurzel) oder auf ein anderes Objekt?

**Zwölf Stores gehen auf ein MODELL-TEIL.** Der Beweis ist in allen vier Fällen derselbe
Zwei-Schritt: `lw <reg>,408(base)` (= **+0x198**, der Teile-Pool) und danach ein Index, der mit
**172 = 0xAC** multipliziert wird. Die Multiplikation steht als Schiebekette im Code, z.B.
Hund @0x80104478-94: `sll v0,v1,1 / addu v0,v0,v1` (3v1) `/ sll v0,v0,2` (12v1) `/ subu v0,v0,v1`
(11v1) `/ sll v0,v0,2` (44v1) `/ subu v0,v0,v1` (43v1) `/ sll v0,v0,2` = **172·v1**.

| Overlay | Stores | Teile-Nachweis | Was dort wirklich geschrieben wird |
|---|---|---|---|
| Hund | @0x801002DC, @0x801002E8 | `lw v0,408(s0)` @0x801002D0, `addiu a1,a1,172` @0x801002FC, 4 Durchläufe | +0x98/+0x9A/+0x9E = 0 |
| Hund | @0x801044C0, @0x801044CC | `lw v1,408(a0)` @0x80104490, ×172 @0x80104478-94, `sltiu v0,a1,7` = 7 Durchläufe | +0x98 = `lhu(a0+118)` = **YAW**, +0x9E = 10 |
| Spinne | @0x80105CD8, @0x80105CE8 | `lw v1,408(s0)` @0x80105CAC, ×172 @0x80105C90-A8 | +0x98 = **YAW + 2048**, +0x9E = 10 |
| Krähe | @0x80102F0C, @0x80102F14 | Funktionsanfang @0x80102EF4 (direkt nach `jr ra` @0x80102EEC), `a0` = Teile-Zeiger des Aufrufers | +0x9E = 10, +0x98 = `a1` |
| Zombie | @0x80105EAC, @0x80105EB4, @0x80105EE8, @0x80105EF0 | `lw v1,408(s4)` @0x80105E98, Versatz `s0·516 + 1720` = Teil `3·s0 + 10` (516 = 3·172, 1720 = 10·172) | +0x98/+0x9A = 0, +0x9E = `s1` = −32718 |

Dass es **dieselbe** Sache ist, zeigen die gemeinsamen Konstanten: alle vier schreiben
zusätzlich `sw …,112(v1)` (+0x70) und ODERn `word0 |= 0x4A` (Zombie: `0x10`), und drei von vier
setzen wörtlich dieselben Zahlen +0x9C = 800/600, +0x9E = 10, +0xA4 = −100, +0x70 = **0x101040**
(Hund @0x80104444-5C: `t2=800, t1=-150, t0=10, a3=-100, a2=0x101040`; Krähe @0x80102EF4-24
gleiche Werte). **In der TEIL-Struktur ist +0x98 ein Winkel und +0x9E ein Radius** — ein ganz
anderes Feld-Layout als in der Gegner-Struktur. Diese zwölf gehören **nicht** in den Zensus der
Trefferbox und dürfen im Port **nicht** auf `re2_hit_box_set` gebogen werden.

Bleiben **36 echte SELF-Stores** = 18 Paare.

---

## 2. Der Zensus je Typ — jede Stelle, ihr Wert, ihr Objekt, ihr Zustand

Lesart des Feldpaars (selbst hergeleitet aus dem Tor, §4): Die Box ist um **eY + b** zentriert
und **h** hoch (halb). Sie reicht also von `eY+b−h` bis `eY+b+h`.
Gegenprobe am Spieler: RE2 gibt ihm **−1530/1530** (`addiu v0,zero,-1530` @0x8005742C /
`sh v0,152(s0)` @0x80057434 / `addiu v0,zero,1530` @0x80057438 / `sh v0,158(s0)` @0x8005743C,
XZ 450 @0x80057444/48) ⇒ Spanne eY−3060…eY, ein **3060 Einheiten hoher Mensch**. Das ist der
Maßstab, gegen den alles andere zu lesen ist.

### 2.1 ZOMBIE 0x10 — `EMZ0.BIN`, 26 Stores = 11 SELF-Paare + 4 Teil-Stores

| # | Adresse (b / h) | Wert | Objekt | Zustand / Auslöser | Gegenstelle |
|---|---|---|---|---|---|
| 1 | @0x8010095C / @0x80100964 | **−1500 / 1500** | `s2` SELF | **INIT** (direkt nach `sw 1,488(s2)` @0x80100954) | — (ist der Normalwert) |
| 2 | @0x80100B18 / @0x80100B20 | **−350 / 350** | `s2` SELF | Niederschlag-Zweig; davor `sw 23,332(s2)` (+0x14C = Clip 23) und +0x9A/+0x9C/+0x90/+0x92 = **200** @0x80100B00-10 | Rampe #5; danach `sw 513,4(s2)` @0x80100B74 = ACTIVE/Sub2 |
| 3 | @0x80100BCC / @0x80100BD4 | **−350 / 350** | `s2` SELF | Zweig `(+0x10E & 0x3F) == 10` (`bne v0,v1,…` @0x80100BAC); setzt danach **`sw 0x0F01,4(s2)`** @0x80100BDC = Zustand 1 / Sub 15 | Rampe #5 |
| 4 | @0x80103464 / @0x8010346C | **−350 / 350** | `s2` SELF | Kriecher-Tick; XZ auf **200** @0x80103458/5C, +0x9A/+0x9C = 0 @0x80103478/7C | Rampe #5 (steht 2 Instruktionen später im selben Block) |
| 5 | **@0x8010368C / @0x80103694** | **RAMPE: b−10 / h+10 je Bild** | `s2` SELF | Aufricht-Rampe, **die zentrale Gegenstelle** — siehe §3 | deckelt sich selbst bei b = −1500 |
| 6 | @0x80103714 / @0x80103720 | **−1500 / 1500** | `s2` SELF | Ende der Aufricht-Kette; danach `andi +0x1D3,0x7F` @0x80103724 und `word0 |= 0x0C000000` @0x80103730 | — |
| 7 | @0x80104A20 / @0x80104A28 | **−1500 / 1500** | `s1` SELF | Wiedereinstieg; XZ auf `v1` @0x80104A04-0C, danach `sw s0,332(s1)` (Clip) @0x80104A7C | — |
| 8 | @0x80106B2C / @0x80106B34 | **−350 / 350** | `a0` SELF | Niederschlag; danach `sw t2,4(a0)` @0x80106B3C und `sw t0,332(a0)` | Rampe #5 |
| 9 | @0x80107810 / @0x80107818 | **−350 / 350** | `s2` SELF | setzt `sh 8193,270(s2)` (+0x10E = 0x2001) @0x80107824; Rückkehr **`sw 513,4(s2)`** @0x8010784C | Rampe #5 |
| 10 | @0x80107E90 / @0x80107E98 | **−1500 / 1500** | `s1` SELF | Aufstehen; `+0x21A &= 0xFFED` @0x80107EA4, `word0 |= 0x0C000000` @0x80107EA8, `+0x6 += 1` @0x80107ECC | — |
| 11 | @0x8010899C / @0x801089A4 | **−350 / 350** | `s1` SELF | wie #9: `+0x10E = 8193` @0x801089B0, Rückkehr **`sw 513,4(s1)`** @0x801089D0-DC | Rampe #5 |
| — | @0x80105EAC/B4, @0x80105EE8/F0 | 0 / −32718 | **TEIL** | Modell-Teil `3·s0+10`, s. §1.1 | n/z |

**Bilanz Zombie:** 4 Paare setzen die stehende Box (−1500/1500), **6 Paare** die Kriecher-Box
(−350/350), **1 Paar** ist die Rampe zurück.

### 2.2 HUND 0x20 — `EMD0G_MOD0.BIN`, 10 Stores = 3 SELF-Paare + 4 Teil-Stores

| # | Adresse (b / h) | Wert | Objekt | Zustand | Gegenstelle |
|---|---|---|---|---|---|
| 1 | @0x80100294 / @0x8010029C | **−1000 / 1000** | `s0` SELF | **INIT**; davor +0x94 = 500 @0x80100288 | — |
| 2 | @0x8010409C / @0x801040A8 | **−500 / 500** | `a2` SELF (`addu a2,a0,zero` @0x8010408C) | `FUN_80104088(…, a1 = 0)` — gerufen @0x80103458 (HURT-P0) und @0x8010352C (HURT-P1) | #3 |
| 3 | @0x801040BC / @0x801040C4 | **−1000 / 1000** | `a2` SELF | `FUN_80104088(…, a1 = 1)` — gerufen @0x801036F0 (HURT-P2, nach Clip-Ende) und @0x801037C0 (HURT-P3, direkt vor `sw 513,4(s0)` = ACTIVE) | — |
| — | @0x801002DC/E8, @0x801044C0/CC | 0 bzw. Yaw | **TEIL** | s. §1.1 | n/z |

Das ist der bereits gebaute und verriegelte Stand (`re2d_hitbox`, ctest
`r30_hund_hitbox_stauchung`). **Unverändert korrekt** — der Vollscan bestätigt, dass es keine
weitere Setz-Stelle gibt.

### 2.3 KRÄHE 0x21 — `EMOVL21_S0.BIN`, 6 Stores = 2 SELF-Paare + 2 Teil-Stores

⛔ **Hier hatte Runde 30 eine Lücke.** Dort stand nur „−350 / 530 @0x801003B8-DC". Tatsächlich
gibt es einen **zweiten, PRO BILD laufenden Schreiber**:

```
801001ec: lw    v0,496(s0)        ; +0x1F0
801001f4: sltiu v0,v0,0x384       ; < 900   (UNSIGNED)
801001f8: beq   v0,zero,0x80100208
801001fc: addiu v0,zero,-350      ; Delay-Slot
80100200: j     0x8010020c
80100204: sh    v0,152(s0)        ; Delay-Slot:  +0x98 = -350
80100208: sh    zero,152(s0)      ;              +0x98 =    0
8010020c: lw    a1,264(s0)
80100210: lbu   v0,4(s0)          ; Zustands-Byte
80100224: lw    v0,18696(at)      ; Verteiler-Tabelle @0x80104908
8010022c: jalr  v0
```

Das sitzt **vor** dem Zustands-Verteiler, läuft also in **jedem** Bild und in **jedem**
Zustand. Die Krähe hat damit **zwei** Boxen, und die Umschaltung hängt an **+0x1F0**.

| # | Adresse (b / h) | Wert | Objekt | Zustand | Gegenstelle |
|---|---|---|---|---|---|
| 1 | @0x801003C4 / @0x801003DC | **−350 / 530** | `s1` SELF | INIT (`sh zero,496(v1)`/`498(v1)` @0x801003BC/C0 setzt +0x1F0 = 0) | — |
| 2 | **@0x80100204** | **−350** | `s0` SELF | Wurzel, jedes Bild, **wenn +0x1F0 < 900** | @0x80100208 |
| 3 | **@0x80100208** | **0** | `s0` SELF | Wurzel, jedes Bild, **wenn +0x1F0 ≥ 900** | @0x80100204 |
| — | @0x80102F0C/14 | 10 bzw. `a1` | **TEIL** | s. §1.1 | n/z |

`h` = 530 wird **nie** wieder geschrieben. **+0x1F0 ist die Flughöhe** — Beleg in §5.3.

### 2.4 SPINNE 0x25 — `EMS25.BIN`, 4 Stores = 2 SELF + 2 Teil-Stores

Die Box kommt als **Blockkopie** aus einer Tabelle, nicht aus Immediates:

```
80102718: lui   a1,0x8010
8010271c: addiu a1,a1,25504     ; = 0x801063A0
80102720..8010275C:  acht  lw/sw  -> +0x84 … +0xA0
8010274c: sw a0,152(s0)         ; +0x98/+0x9A = Tabelle[+0x14]
80102758: sw v0,156(s0)         ; +0x9C/+0x9E = Tabelle[+0x18]
8010276c: sh zero,152(s0)       ; +0x98 := 0     (ueberschreibt die Kopie)
```

Tabelle selbst gelesen (`bytes 0x801063a0 32`, EMS25.BIN):
```
801063a0: 00 00 00 00 00 00 00 00 00 00 00 00 20 03 20 03
801063b0: 00 00 00 00 88 fa e8 03 e8 03 78 05 00 00 00 00
```
⇒ `+0x14 = 88 fa e8 03` → +0x98 = 0xFA88 = **−1400**, +0x9A = 0x03E8 = 1000
⇒ `+0x18 = e8 03 78 05` → +0x9C = 0x03E8 = 1000, +0x9E = 0x0578 = **1400**
⇒ `+0x0C = 20 03 20 03` → +0x90/+0x92 = 800/800 (XZ)

| # | Adresse | Wert | Objekt | Zustand | Gegenstelle |
|---|---|---|---|---|---|
| 1 | @0x8010276C | **+0x98 = 0** | `s0` SELF | direkt nach der Blockkopie, danach `sw 1,4(s0)` = Zustand 1 | — |
| 2 | @0x8010049C | **+0x98 = 0** | `s2` SELF | anderer Zweig; davor `sh 2048,120(s2)`, danach `word0 |= 0x10000000` @0x801004A0 und `sw v1,20(s0)` | — |
| — | @0x80105CD8/E8 | Yaw+2048 / 10 | **TEIL** | s. §1.1 | n/z |

**Ergebnis: b = 0, h = 1400.** Beide SELF-Stellen setzen denselben Wert; die offene Frage aus
Runde 30 („welche der zwei läuft") ist damit **folgenlos** — sie war es schon, und der Vollscan
bestätigt, dass es keine dritte gibt.

### 2.5 BABY 0x26 — `EMS26.BIN`, 2 Stores = 1 SELF-Paar

```
80100168: addiu v0,zero,-10
8010016c: sh v0,152(s0)     ; +0x98 = -10
80100170: addiu v0,zero,10
80100174: sh v0,154(s0)     ; +0x9A =  10
80100178: sh v0,158(s0)     ; +0x9E =  10
8010017c: sh v0,156(s0)     ; +0x9C =  10
80100180: sh v0,144(s0)     ; +0x90 =  10
80100184: sh v0,146(s0)     ; +0x92 =  10
80100194: sh zero,148(s0)   ; +0x94 =   0
80100198: sh zero,150(s0)   ; +0x96 =   0
```

**Ein einziges Paar, im INIT, ohne jede Gegenstelle** — bestätigt gegen den Vollscan des ganzen
Overlays (2 von 2 Treffern liegen in diesem Block). Alle sechs Box-Maße sind **10**. Das ist ein
**20 Einheiten großer Würfel** bei einem 3060 Einheiten hohen Spieler — 0,65 % der Körperhöhe.
Nebenbefunde derselben Stelle: hp = 1 (`sh v0,342(s0)` @0x801000F8), +0x1D3 = 6 @0x801001B0,
`word0 |= 0x04C00000` @0x80100130-44.

---

## 3. ⛔ Die wichtigste Frage: macht ein Klein-Zustand dauerhaft untreffbar?

Das ist die Runde-13/14-Falle in Hitbox-Form. Antwort **je Klein-Zustand**:

| Typ | Klein-Zustand | Wo macht er wieder auf? | Im Spiel erreichbar? |
|---|---|---|---|
| **0x10** | −350/350 (6 Setz-Stellen, §2.1 #2/#3/#4/#8/#9/#11) | **Die Rampe @0x8010366C-94** — siehe unten. Zusätzlich vier harte Rücksetzer auf −1500/1500 (#1 INIT, #6, #7, #10) | **JA** — die Rampe steht im selben Tick-Block wie der Kriecher-Tick (#4) und läuft, solange der Zombie nicht schon groß ist |
| **0x20** | −500/500 (`FUN_80104088(…,0)`) | `FUN_80104088(…,1)` @0x801040B8-C4, gerufen @0x801036F0 und @0x801037C0 | **JA** — gemessen: Bild +71 zurück auf −1000/1000 (ctest `r30_hund_hitbox_stauchung`) |
| **0x21** | b = 0 (@0x80100208) | @0x80100204 im **nächsten Bild**, sobald +0x1F0 < 900 | **JA** — beide Zweige laufen jedes Bild |
| **0x25** | — | es gibt keinen Klein-Zustand; b = 0 / h = 1400 ist der Normalwert | n/z |
| **0x26** | — | es gibt nur **einen** Zustand (±10) | n/z — **das ist der Normalwert, keine Stauchung** |

### Die Zombie-Rampe, selbst disassembliert

```
80103640: lhu v0,156(s2)          ; +0x9C
80103648: addiu v0,v0,10
8010364c: sh  v0,156(s2)          ; +0x9C += 10   (je Bild, ungedeckelt)
80103644: lhu v1,144(s2)          ; +0x90
80103650: sltiu v0,v1,0x1f4       ; < 500
80103654: beq v0,zero,0x8010366c  ; sonst XZ-Wachstum ueberspringen
80103658: addiu v1,v1,10
80103660: sh  v1,144(s2)          ; +0x90 += 10, Deckel 500
80103668: sh  v0,146(s2)          ; +0x92 += 10
8010366c: lh  v0,152(s2)          ; b (SIGNED)
80103674: addu v1,v0,zero
80103678: slti v0,v0,-1499        ; b < -1499  ?
8010367c: bne v0,zero,0x80103698  ; ja -> fertig, nicht weiter aufziehen
80103684: lhu v0,158(s2)          ; h
80103688: addiu v1,v1,-10
8010368c: sh  v1,152(s2)          ; b -= 10
80103690: addiu v0,v0,10
80103694: sh  v0,158(s2)          ; h += 10
801036a0: jal 0x8002959c          ; Clip-Vorschub
```

**Rechnung:** von b = −350 bis zum Deckel b = −1500 sind 1150 Einheiten, in 10er-Schritten je
Bild ⇒ **115 Bilder** (≈ 3,8 s @30 Hz). Parallel wächst h von 350 auf 1500 und XZ von 200 auf
den Deckel 500 (`sltiu v1,0x1f4` @0x80103650).

⇒ **Kein Typ hat einen Klein-Zustand ohne erreichbare Gegenstelle.** Die Gefahr liegt
woanders — sie liegt beim **Normalwert** kleiner Gegner (§5).

---

## 4. Das Tor, noch einmal selbst gelesen — und was danach passiert

```
8004716c: lhu v1,464(s0)     ; +0x1D0 (nur Seiteneffekt)
80047170: lh  a0,152(s0)     ; b  SIGNED
80047174: lw  v0,60(s0)      ; eY = MATRIX.t[1] (+0x3C)
8004717c: addu v0,v0,a0
80047180: addiu v0,v0,100
80047188: lhu v1,158(s0)     ; h  UNSIGNED
8004718c: lw  a0,4(s4)       ; Muendungs-Y
80047190: addu v0,v0,v1
80047194: subu v0,v0,a0
80047198: addiu v1,v1,100
8004719c: sll v1,v1,1        ; 2h + 200
800471a0: sltu v0,v0,v1      ; UNSIGNED
800471a4: beq v0,zero,0x8004740c
```
⇒ `DURCH ⟺ −(b+h+100) ≤ Hgun < (h−b+100)`, mit `Hgun = eY − Mündungs-Y`.
Gleichwertig, und anschaulicher: **die Mündung muss senkrecht in `[eY+b−h, eY+b+h]` liegen,
±100 Schlupf.**

**Neu in dieser Runde — was hinter dem Tor liegt** (Auftrag: das Sprungziel lesen, nicht die
Aufrufstelle):

```
800471a8: addu a0,s4,zero        ; Schuss-Ursprung
800471b4: addiu a2,s0,132        ; = +0x84, der BOX-BLOCK
800471e8: jal 0x80041ef8         ; die eigentliche Trefferpruefung
800471f0: beq v0,zero,0x800473dc ; kein Treffer -> weiter
80047200: ori v0,v0,0x1 / sh v0,464(s1)
80047258: lhu v0,342(s1)         ; hp
80047260: subu v0,v0,v1
80047268: sh  v0,342(s1)         ; hp -= Schaden
80047284: bgez v1,… / sw v0,4(s1) ; Zustand 2 (Schaden) bzw. 3 (Tod)
```

⇒ **`FUN_800470C0` ist der Schadens-Pfad selbst, nicht bloß eine Vorauswahl.** Ein dauerhaft
geschlossenes fünftes Tor bedeutet wörtlich **unverwundbar**. Die Warnung des Auftrags ist
damit belegt, nicht angenommen.

**Ein Ausweg existiert aber** (und das relativiert die Gefahr, ohne sie aufzuheben): der
Vollscan aller `sh rt,342(rs)` in `info/re2leon/PSX.EXE` findet 17 hp-Schreiber; drei liegen im
Schuss-Komplex. @0x80047488 ist derselbe Gewinner wie @0x80047268 (anderer Zweig, dasselbe
Tor). **@0x800477CC dagegen sitzt in einem eigenen Pfad ohne jedes Höhen-Tor**:

```
80047764: lw v0,0(s5) / lw v1,56(s0)   ; dX  (+0x38)
80047778: lw v0,8(s5) / lw v1,64(s0)   ; dZ  (+0x40)
80047794: jal 0x8008d2f4               ; Wurzel(dX^2 + dZ^2)
800477a4: sltu v0,v0,a3                ; < Radius
800477cc: sh v0,342(s3)                ; hp -= s6
```
Nur XZ-Abstand, **kein Y**. Das ist die Flächen-/Radius-Schadensquelle.

---

## 5. Die Tabelle: Fenster je Typ und Zustand gegen die GEMESSENE Mündungshöhe

Gemessen mit `probe_r30b_muendung` (echter Weg: `re15_game_step` + Pad, echte RDTs,
RE2-KI-Geschmack, RE2-Bank `shared_assets/RE2/CDEMD0.EMS` geladen — sonst hätte jeder Clip
Länge 0, die Falle aus Runde 29). **Die 0x25-Zeile ist NEU** und in dieser Runde erst möglich
geworden (§5.4).

| Typ / Zustand | b | h | Fenster Hgun | Box-Spanne über eY | **gemessen Hgun** | Raum | **DURCH** |
|---|---|---|---|---|---|---|---|
| 0x10 Zombie stehend | −1500 | 1500 | [−100, **3100**) | 3000 | 1665…1671 | ROOM1140 | **240/240** |
| 0x10 Zombie Kriecher | −350 | 350 | [−100, **800**) | 700 | (dieselbe Pose) | — | **0** (rechnerisch) |
| 0x20 Hund stehend | −1000 | 1000 | [−100, **2100**) | 2000 | 1649…2085 | ROOM1190 | **240/240** |
| 0x20 Hund liegend | −500 | 500 | [−100, **1100**) | 1000 | (dieselbe Pose) | — | **0** (gewollt, das ist der Befund) |
| 0x21 Krähe, +0x1F0 < 900 | −350 | 530 | [−280, **980**) | 880 | 1420…1671 | ROOM10C0 | **0/240** |
| 0x21 Krähe, +0x1F0 ≥ 900 | 0 | 530 | [−630, **630**) | 530 | 1420…1671 | ROOM10C0 | **0/240** |
| **0x25 Spinne** | **0** | **1400** | **[−1500, 1500)** | **1400** | **1665…1671** | **ROOM2050** | **0/240** |
| 0x26 Baby | −10 | 10 | [−100, **120**) | 20 | 1665…1671 | ROOM1090 | **0/240** |

Die Mündungshöhe selbst, gemessen 240/240 gültig je Typ:
`MündungY −1671…−1665` (Zombie), `−5349…−2197` (Hund), `−1921…−1665` (Krähe),
`−7071…−7065` (Spinne), `−3471…−3465` (Baby).

**Das Muster ist eindeutig:** durch kommt genau, wessen Box bis mindestens **~1668** über eY
reicht. Zombie (3000) und Hund (2000) tun das. Spinne (1400), Krähe (880) und Baby (20) nicht.
Die Spinne verfehlt es um **168 Einheiten**.

### 5.1 ⛔ ZURÜCKGEZOGEN (Runde 32): „1668 = 54,5 % der Körperhöhe" war kein Beleg

Hier stand: *„RE2 gibt dem Spieler die Box −1530/1530 (@0x8005742C / @0x80057434 /
@0x80057438 / @0x8005743C) ⇒ Körperhöhe 3060; der Port misst die Mündung 1668; 1668/3060 =
54,5 % = Brust-/Schulterhöhe, die Zahl ist also richtig."*

**Das ist ein Prozentsatz, kein Mechanismus.** Berichtigt in zwei Punkten, beide gemessen:

1. **1666/1668 ist die BINDPOSE von PL00**, nicht eine Zielhöhe: 1804 + 692 − 422 − 408 = 1666
   (Bind-Offsets Bone 0/9/10/11, `analysis/befunde_2026-09-27/zielpose-ermittlung.md` §1.2).
   Der Port las sie, weil `re15_player_muzzle_world` die Basis-Bank posierte. Die Deckung mit
   „Brusthöhe" ist Zufall — RE2s eigene Bindpose ist **1563**, nicht 1666.
2. **Die echte Zielpose liegt viel höher** (Runde 32, `probe_r30b_muendung` Teil 0, Waffe 3):
   **HOCH 2751 · EBEN 2500 · TIEF 1988**; RE2s eigene Bank PL00W02 liefert **2805 / 2504 /
   1921**. Der Anschlag sitzt also auf **Schulterhöhe** (RE1.5 Schulter = 1804 + 692 = 2496),
   nicht auf 54,5 %.

Folge für die Tabelle oben: die Spalte „gemessen Hgun" ist die Höhe der **Basis-Bank**. Mit
der Zielpose kommt von den sieben Fällen **nur der stehende Zombie** bei allen drei Bändern
durch, der stehende Hund **nur bei TIEF** — deshalb füttert das fünfte Tor die Zielpose
bewusst **nicht** (Begründung mit Zahl im Kopf von `re15_player_muzzle_world`).

### 5.2 Was fehlt, ist die senkrechte ZIELPOSE des Spielers

Der Port hat sie **gar nicht**. Beleg:

* `grep -rn "aim_pitch\|aim_up\|aim_down\|AIM_UP\|AIM_DOWN"` über `re15_port/engine/src/*.c`
  und `re15_port/include/*.h` ⇒ **null Treffer**.
* `re15_player_muzzle_world` (`re15_damage.c:1190-1209`) posiert die PL00-Bank mit
  `re15_compute_actor_kf(an, sk, pl, -1, pl->anim_frame)` — also mit **dem Clip, in dem der
  Spieler ohnehin ist**, ohne jede Ziel-Abhängigkeit. Deshalb ist der gemessene Wert über alle
  vier Typen hinweg praktisch **konstant** (1665…1671, eine Streuung von 6 Einheiten), obwohl
  die Gegner von 20 bis 3000 Einheiten hoch sind.
* In RE2 kommt die Mündung dagegen aus der **verketteten Pose** Bone 0 → 9 → 10 → 11
  (@0x80042E64/74/84/94, Kind-Versätze 24/1572/1744/1916, Abstand je 172). Eine Pose, die sich
  mit der Zielrichtung ändert, bewegt Bone 11 zwangsläufig mit.

**Dass RE1.5 dasselbe Problem kennt und anders löst, steht schon im Port**
(`re15_damage.c:1228ff`, `re15_band_stamp_aa4`): RE1.5 führt ein **Elevations-Band** in word0
Bit 31/30/29 (UP/LEVEL/DOWN), Höhenindex `(playerY − enemyY) / 1800` (`FUN_80012AA4`
@0x80012AD8/B0C-B1C), und der Resolver `FUN_80011F50` lässt einen Kandidaten nur durch, wenn
`enemy.word0 & player.word0 & 0xE0000000` gesetzt ist (@0x800120D0-EC).

> **Das ist DASSELBE Problem in zwei Bauformen.** RE1.5: ein diskretes Band mit Schwelle 1800.
> RE2: stetig, Mündung gegen die Box. Der Port führt das RE1.5-Band (mit Spieler-Anteil) und
> seit Runde 30 das RE2-Tor — aber **nur RE2s Hälfte verlangt, dass sich die Mündung bewegt**,
> und diese Bewegung hat der Port nicht.

⇒ **Urteil: nicht die Mündungshöhe ist falsch, sondern es fehlt ein Zustand — die senkrechte
Zielpose des Spielers.** Sie fehlt für Krähe, Spinne, Baby **und** den Zombie-Kriecher, also für
jeden Gegner, der niedriger ist als Brusthöhe.

### 5.3 Bei der Krähe fehlt ZUSÄTZLICH die Flughöhe

`+0x1F0` schaltet die Box der Krähe (§2.3). Der Vollscan über das Overlay zeigt: **+0x1F0 wird
9× gelesen und nach dem INIT nie geschrieben** — es kommt von der Engine. Die Schwellen, gegen
die die Krähe es prüft, sagen, was es ist:

| Stelle | Prüfung |
|---|---|
| @0x801001F4 | `sltiu v0,v0,0x384` — **< 900** (Box-Umschaltung) |
| @0x801006F0 | `sltiu v0,v0,0x708` — **< 1800** |
| @0x80100704 | `sltiu v0,v0,0x709` — **< 1801** |
| @0x801010C4 | `sltiu v0,v0,0x1c21` — **< 7201** |
| @0x801023FC | `sltiu v0,v0,0x384` — **< 900** |
| @0x80102418 | `sltiu v0,v0,0x28a` — **< 650** |
| @0x8010425C-60 | `addiu v0,v0,-901` / `sltiu v0,v0,0x189b` — **Band 901 … 7199** |
| @0x8010448C | `sltiu v0,v0,0x708` — **< 1800** |

Dazu die Höhenregelung selbst: `+0x224` ist die **Soll-Höhe** (`lh v0,548(s0)` @0x801023BC),
sie wird gegen die Ist-Höhe `+0x3C` mit **±60 Totzone** ausgeregelt (`subu v1,v0,a0` /
`addiu v0,v1,60` / `sltiu v0,v0,121` @0x801023C8-D0), und @0x80101C1C-3C wächst `+0x224` um
**20 je Bild** und wird auf `+0x3C` addiert.

⇒ **+0x1F0 ist die Höhe über dem Boden, und RE2s Krähe arbeitet im Band 901…7199.** Fliegt sie
dort, liegt ihre Box (530 halb, um eY zentriert bei +0x1F0 ≥ 900) genau auf Mündungshöhe 1668 —
das Tor ginge auf. **Der Port hält seine Krähe auf 0…250 über dem Boden** (Runde 30, eY −250…0
über 240 Bilder). Das ist der fehlende Zustand.

### 5.4 Was an der 0x25-Zeile geändert wurde (Mess-Korrektur, keine Logik)

`probe_r30b_muendung` Teil 1 suchte die Spinne in `STAGE2/ROOM2000` — dort hat der Port **keine
lebende 0x25**, die Sonde meldete nur „FEHLLAUF … sagt NICHTS", und darum **fehlte die Zeile im
Welle-2-Dossier**. Teil 4 fuhr längst eine Raumliste ab. Beide teilen jetzt dieselbe Liste
(`SPINNENRAUM`), und Teil 1 nennt den gefundenen Raum:

```
(0x25 gefunden in STAGE2/ROOM2050.RDT)
[SPINNE 0x25 ] 240 Bilder | Muendung gueltig 240/240 | MuendungY -7071..-7065
               | Hgun 1665..1671 | Tor(b=0,h=1400) DURCH 0/240
```

---

## 6. AUFTRAG B — der Fahrstuhl-Zensus

### 6.1 Was der Port heute hat

`scd_elev_se.c` hängt den Ton an einer **32-Byte-Signatur** im geladenen RDT-Puffer auf
(`22 01 1c 01 / 09 0a 08 00 / 22 01 1c 00 / 09 0a 5a 00 / 22 01 1c 01 / 09 0a 08 00 /
22 01 1c 00 / 09 0a 14 00`). Über alle 240 RDTs: **4 Dateien, je 3 Treffer** —
ROOM1080/1081 @0x746/0x7D8/0x86A und ROOM4020/4021 @0x95A/0xA00/0xA96. Bestätigt
(`scan_lift_all.py`).

### 6.2 Die Suche UNABHÄNGIG von der Signatur

Gesucht wurde die **Gestalt** einer Fahrt statt der festen Bytefolge, über alle 240 RDTs:
jedes `Set bank1 bit0x1C/0x1D`-Paar mit Schlafzeiten dazwischen. Ergebnis — **14 Räume** tragen
diese Bits überhaupt:

| Raum | 1C=1 | 1C=0 | 1D=1 | 1D=0 | SIG | Urteil |
|---|---|---|---|---|---|---|
| ROOM1080 / 1081 | 6 | 6 | 0 | 0 | **3** | **FAHRSTUHL** (STAGE1 „ELEVATOR"), 3 Etagen |
| **ROOM3080 / 3081** | **4** | **2** | **2** | **2** | **0** | **FAHRSTUHL** (STAGE3 „WAREHOUSE LIFT"), 2 Fahrten — **anderes Skript** |
| ROOM4020 / 4021 | 6 | 6 | 0 | 0 | **3** | **FAHRSTUHL** (STAGE4 „A-2 ELEVATOR"), 3 Etagen |
| ROOM2040 / 2041 | 1 | 1 | 0 | 0 | 0 | kein Fahrtmuster (Einzelpuls) |
| ROOM20B0 / 20B1 | 1 | **0** | 0 | 0 | 0 | kein Fahrtmuster (nur Einschalten) |
| ROOM5090 / 5091 | 1 | 1 | 0 | 0 | 0 | kein Fahrtmuster |
| ROOM6030 / 6031 | 3–4 | 3 | 1–2 | 1 | 0 | **Lampen-Flackern**, kein Fahrstuhl — die Pulse sind mit `Cut_chg 4/5` verschränkt (@0x01214-0x01250) und @0x01004 ist eine Blink-Schleife mit `Goto` |

Das erklärt die Bits nebenbei: **bank1 bit 0x1C/0x1D ist eine LAMPE**, kein „Fahrstuhl fährt".
Die 32-Byte-Signatur ist also der **Flacker-Rhythmus** der Fahrstuhlbeleuchtung — ein Anker,
der zufällig eindeutig ist, aber die Warehouse Lift nicht kennt.

### 6.3 Die WAREHOUSE LIFT — das fehlende Stück

Name aus dem Original-Debug-Menü: `re15_dbg_jump_name[2][8] = "WAREHOUSE LIFT"`
(STAGE3, Slot 8 ⇒ ROOM3080). Das Fahrskript, selbst disassembliert
(`scd_dump_room.py ROOM3080.RDT`):

```
0x009FE  Set        22 01 1c 01   bank=1 bit=28 = 1
0x00A02  Sleep      09 0a 3c 00   60
0x00A06  Set        22 01 1c 00   bit=28 = 0
0x00A0A  Set        22 01 1d 01   bit=29 = 1
0x00A0E  Sleep      09 0a 3c 00   60
0x00A12  Set        22 01 1d 00   bit=29 = 0
0x00A16  Set        22 01 1c 01   bit=28 = 1
0x00A1A  Message_on 2b 01 00 00
0x00A1E  Sleep      09 0a 64 00   100
0x00A22  Aot_on     47 00
0x00A24  Evt_end    01 00
```
Die zweite Fahrt steht @0x00BDA-0x00BFC, **bitgleich** (ohne das `Message_on`).
In ROOM3081 dieselben zwei Fahrten @0x007FE und @0x00842.

**Unterschied zur Signatur:** zwei Bits statt einem, Schlafzeiten **60 / 60 / 100** statt
**8 / 90 / 8 / 20**. Die Fahrt dauert 60+60+100 = **220 Bilder ≈ 7,3 s** (der Fahrstuhl
ROOM1080 fährt 90 Bilder = 3 s je Etage).

**Ton:** ROOM3080 hat zwar `Se_on` im Skript (`36 02 0a 00 02 …` = Bank 2, Id 0x0A,
@0x0095C / @0x00970 / @0x00984), aber **nicht in der Fahrt** — die drei liegen ~0xA0 Byte
davor in einem anderen Abschnitt. **Die Fahrt selbst ist stumm**, genau wie bei den anderen
vier. Und weil die 32-Byte-Signatur nicht greift, spielt der Port dort auch nichts.

### 6.4 Das RE2-Gegenstück

`scan_lift_re2.py` über alle 250 RE2-RDTs: RE2s Fahrstuhl ist **ROOM21B0 / ROOMB1B0** — je
2 Pulse auf bit0x1C und **je 2 `Se_on` unmittelbar daneben** (@0x2762/@0x2790 bzw.
@0x28B8/@0x28E6). Das sind genau die beiden, die der Port schon importiert hat
(Bank 2, Id 0x11 und 0x12). Kein weiterer RE2-Raum zeigt die Fahrt-Gestalt mit
danebenliegenden `Se_on`.

### 6.5 Antwort auf die gestellte Frage

> *„Sag klar: wieviele Fahrstuehle hat RE1.5, in wievielen spielt der Ton, und welche fehlen."*

* **RE1.5 hat 3 Fahrstühle** = **6 RDT-Dateien** (je eine Leon-/Elza-Fassung).
* **Der Ton spielt in 2 von 3** (4 von 6 Dateien): ROOM1080/1081 und ROOM4020/4021.
* **Es fehlt 1 Fahrstuhl** (2 Dateien): **ROOM3080 / ROOM3081, „WAREHOUSE LIFT" (STAGE3)**,
  je **2** Fahrten, Byte-Offsets **ROOM3080 @0x009FE und @0x00BDA**, **ROOM3081 @0x007FE und
  @0x00842**.
* Damit der Ton auch dort kommt, muss der Anker die **zweite Fahrt-Gestalt** kennen
  (`22 01 1c 01 / 09 0a 3c 00 / 22 01 1c 00 / 22 01 1d 01 / 09 0a 3c 00 / 22 01 1d 00 /
  22 01 1c 01`, 28 Byte, in genau diesen 2 Dateien × 2 Fundstellen, 0 Fehltreffer über die
  240 RDTs). Das bleibt Welle 2 — diese Welle ändert kein Verhalten.

---

## 7. Was daraus für den Bau folgt (Empfehlung, nicht gebaut)

1. **`re2_hit_box_set` für 0x10 verdrahten** ist gefahrlos und der nächste sinnvolle Schritt:
   der Zombie hat mit −1500/1500 ein Fenster [−100, 3100), kommt gemessen 240/240 durch, und
   seine Kriecher-Stauchung hat mit der Rampe @0x8010366C-94 eine **erreichbare, gemessene**
   Gegenstelle (115 Bilder). Damit wird der Kriecher gegen Brusthöhe-Schüsse gesperrt — was RE2
   so will, **aber erst, wenn Punkt 3 steht**, sonst ist er unspielbar.
2. **0x25 verdrahten ist grenzwertig** — 168 Einheiten fehlen. Nicht ohne Punkt 3.
3. **⛔ Zuerst die senkrechte Zielpose des Spielers.** Ohne sie sperrt das Tor jeden Gegner
   unter Brusthöhe dauerhaft aus: Krähe, Spinne, Baby und den Zombie-Kriecher. Das ist der
   einzige Schritt, der die anderen freischaltet.
4. **0x21 braucht zusätzlich die Flughöhe** (+0x1F0-Band 901…7199, Soll-Höhe +0x224 mit
   ±60-Totzone).
5. **0x26 bleibt auch danach offen.** Mit ±10 ist das Fenster [−100, 120); selbst eine voll
   nach unten gerichtete Mündung träfe das nur knapp. Vermutung — **nicht** belegt, daher als
   Frage notiert: RE2 tötet Baby-Spinnen über den **radialen** Pfad @0x800477CC (nur XZ, kein
   Y, §4). Das ist die nächste zu prüfende Spur, keine Feststellung.

---

## 8. Offen / ehrlich benannt

1. **Die Zustandsnamen im Zensus sind Orte, nicht überall Namen.** Für alle 18 SELF-Paare stehen
   Adresse, Wert, Objekt und der umgebende Block; bei den Zombie-Paaren #2/#3/#8 ist der
   auslösende Zustand über die Nachbarschreiber (+0x4, +0x14C, +0x10E) eingegrenzt, aber nicht
   bis zum auslösenden Ereignis zurückverfolgt. Für die Frage dieser Runde (Gegenstelle
   erreichbar?) reicht es, weil die Rampe **zustandsunabhängig** im Kriecher-Tick sitzt.
2. **Kein Bildbeleg.** Gemessen wurde ausschließlich über `re15_game_step` + Pad. Der Auftrag
   erlaubt Sichtprüfung wieder, aber diese Welle hat nichts Sichtbares geändert — es gibt kein
   Vorher/Nachher zu zeigen.
3. **Die +0x9A/+0x9C/+0x90/+0x92-Maße (XZ) sind nicht vollständig erfasst.** Der Auftrag galt
   +0x98/+0x9E; die XZ-Werte stehen nur dort, wo sie im selben Block liegen.
4. **Ob die 0x26-Vermutung aus §7.5 stimmt, ist ungeprüft.** Der radiale Pfad @0x80047764-CC ist
   disassembliert und hat kein Y-Tor; **wer ihn ruft**, ist nicht aufgelöst.
5. **Die Teil-Struktur (+0x98 = Winkel) ist aus dem Gebrauch erschlossen**, nicht aus einer
   Layout-Definition: `lhu(self+118)` (Yaw) bzw. Yaw+2048 landen dort, und der Index läuft über
   den Teile-Pool +0x198 mit Stride 172. Das reicht, um die zwölf Stores auszuschließen; es ist
   kein vollständiges Feld-Layout des Teils.
