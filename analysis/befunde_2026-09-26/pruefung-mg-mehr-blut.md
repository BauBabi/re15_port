# Pruefung: analysis/befunde_2026-09-26/mg-mehr-blut.md

**Urteil: NICHT HALTBAR** (als Umsetzungsplan). Der RE-Kern ist ueberraschend sauber —
17 von 17 zitierten Einzeladressen im Handler stimmen woertlich —, aber der Plan schiebt eine
**erfundene Zahl** in den Auslieferungswert, eine **Adresse aus der falschen Binaerdatei** in die
Begruendung, und die Symptom-Kette haengt an zwei ungeprueften Gliedern.

Selbst nachgelesen: **38 Adressen/Fundstellen** in 4 Binaerdateien
(`info/re2leon/COMMON/BIN/EMZ0.BIN`, `info/re2leon/PSX.EXE`, `info/Re1.5/PSX/BIN/STAGE1.BIN`,
`info/re2leon/COMMON/BIN/STAGE1.BIN`) + 5 Port-Dateien.

---

## 0. Ist es ueberhaupt die richtige Datei?

**JA.** `info/re2leon/COMMON/BIN/EMZ0.BIN` (53068 B = 0xCF4C) laedt roh @0x80100000, ohne Header.
Beweis, nicht Annahme: **437** Woerter in der Datei liegen im Bereich 0x8010xxxx (Zensus), und die
so adressierten Offsets tragen gueltige Funktions-Prologe
(`80105438: addiu sp,sp,-96`, `80107ef0: addiu sp,sp,-56`). Zum Vergleich:
`re2leon/COMMON/BIN/STAGE1.BIN` ist 0x6A8 Byte gross und enthaelt **null** solcher Pointer — es ist
kein Code-Overlay. Die EXE waere falsch gewesen (dort liegt 0x8010xxxx gar nicht im Text).

---

## 1. Die Tabelle @0x8010C940 — BESTAETIGT, und mehr als das Dossier zeigt

Eigener Dump (Stride 36, ab 0x8010C940, Datei-Offset = Adresse − 0x80100000):

```
row  0 @8010c940 8032008c 803e0096 80480096 8046006e 804a0078 802200b4 8034006e 804200c8 804200b4
row 15 @8010cb5c 80107ef0 80107ef0 00000000 80107ef0 80107ef0 00000000 80107ef0 80107ef0 00000000
row 18 @8010cbc8 80107ef0 80107ef0 00000000 80107ef0 80107ef0 00000000 80107ef0 80107ef0 00000000
```
Alle uebrigen Zeilen wie im Dossier (1-4/13/14/16 = 5438, 5/6/17 = NULL, 7/8 = 7438/66FC,
9-11 = GL, 12 = 703C, 19 = Ueberlappung mit der 1D-Kriechertabelle @0x8010CBE8).
**Nur 15 und 18 zeigen auf 0x80107EF0.** ✔

**Zeile 0 ist keine Pointer-Zeile** (`8032008c …`) — die Tabelle ist 1-basiert. Das Dossier sagt
„19 Zeilen ab @0x8010C940"; korrekt waere „Basis @0x8010C940, erste benutzte Zeile 1 @0x8010C964".

**Nachgeliefert, was das Dossier BEHAUPTET, aber NICHT belegt:** der Satz „Der normale Pfad
dispatcht … ueber die 2D-Tabelle 0x8010C940 + Zeile*36 + Spalte*4" steht dort ohne eine einzige
Instruktion. Ich habe die Stelle gesucht und gefunden — sie stimmt:
```
801053e0: lbu   v1,5(a0)            ; ZEILE = +0x5
801053e4: lui   a2,0x8011
801053e8: addiu a2,a2,-14016        ; a2 = 0x8010C940
801053ec: sll   v0,v1,3
801053f0: addu  v0,v0,v1
801053f4: sll   v0,v0,2             ; ((id*8)+id)*4 = id*36
801053f8: lbu   v1,466(a0)          ; SPALTE = +0x1D2
801053fc: addu  v0,v0,a2
80105400: sll   v1,v1,2
80105404: addu  v1,v1,v0
80105408: lw    v0,0(v1)
80105410: jalr  v0
```

---

## 2. @0x80107FC8-0x801080C4 — Instruktion fuer Instruktion: ALLE 17 Adressen stimmen

Eigener Disasm (`re2_disasm.py dis 0x80107f80 88 --bin EMZ0.BIN`), Delay-Slots aufgeloest:

| zitiert | gelesen | ✔ |
|---|---|---|
| @0x80107FCC-FF0 Magic-Multiply | `lui v1,0xaaaa / ori 0xaaab / multu / mfhi t0 / srl 1 / sll 1 / addu / subu / andi 0xff` = `+0x1D2 % 3` | ✔ |
| @0x80107FD8 | `sh v0,344(s2)` — v0 = Rueckgabe von `jal 0x800154ac` @0x80107FC0 | ✔ |
| @0x80107FF4 | `beq a0,zero,0x80108068` | ✔ |
| @0x80108004/08/0C | `addiu s0,zero,256 / sll v0,v0,1 / subu v0,s0,v0` → X = 256−2·rand | ✔ |
| @0x80108018/1C | `sll v0,v0,2 / addiu v1,zero,312` (+ `subu` @20) → Y = 312−4·rand | ✔ |
| @0x8010802C/30 | `sll v0,v0,1 / subu s0,s0,v0` → Z = 256−2·rand | ✔ |
| @0x80108034 | `jal RNG` (Wert landet @0x80108040 `addu s0,v0,zero`) | ✔ |
| @0x8010803C | `jal RNG` (Wert → scale) | ✔ |
| @0x80108050 | `addiu v0,v0,7000` | ✔ |
| @0x80108064 | `addiu a2,s1,72` (Part 0) | ✔ |
| @0x80108070/74 | `addiu s0,zero,127 / subu v0,s0,v0` | ✔ |
| @0x80108080/84 | `addiu v1,zero,927 / subu v1,v1,v0` | ✔ |
| @0x80108090 | `subu s0,s0,v0` | ✔ |
| @0x80108094 | `jal RNG` (sub) | ✔ |
| @0x8010809C | `jal RNG` (scale) | ✔ |
| @0x801080B0 | `addiu v0,v0,6096` | ✔ |
| @0x801080C0 | `addiu a2,s1,244` (Part 1) | ✔ |
| @0x8010805C / @0x801080BC | `lh a1,344(s2)` | ✔ |

**ANZAHL und REIHENFOLGE der RNG-Zuege: bestaetigt.** Fuenf Zuege pro Stoss,
X (@0x80108000) → Y (@0x80108010) → Z (@0x80108024) → sub (@0x80108034) → scale (@0x8010803C).
Die Delay-Slot-Aufloesung ist korrekt gemacht: `addu s0,v0,zero` @0x80108040 liegt im Delay-Slot
des jal @0x8010803C und faengt deshalb den VORHERIGEN Wurf.

RNG `FUN_80015FE8` selbst gelesen: `andi v0,v0,0xff` @0x80016004, `jr ra` @0x80016020 → 0..255. ✔

**⛔ Aber: Phase 0 hat einen SECHSTEN Wurf, den das Dossier nicht mitzaehlt.**
@0x80107F80 `jal RNG` / @0x80107F88 `andi v0,v0,0x7` — die Clip-Wahl. §5 schreibt
„es muessen genau **fuenf** Zuege pro Stoss bleiben"; fuer den Spawn stimmt das, fuer die
RNG-Sequenz der Phase sind es sechs. (Der Port hat den sechsten schon:
`enemy_ai_re2_zombie.c:4936 re2z_rand() & 7u`.)

---

## 3. ⛔ Der Faktor „0x2000 / 6096" — ERFINDUNG, und die Begruendung zitiert die falsche Datei

Beide Enden existieren — **aber in ZWEI VERSCHIEDENEN SPIELEN**:

* `@0x8010567C` in **RE2 `EMZ0.BIN`**: `addiu a0,zero,6096` ✔ (gelesen)
* `@0x80105C54` in **RE2 `EMZ0.BIN`**: `lhu v0,464(s4)` — **das ist NICHT 0x2000**.
  `@0x80105C54` ist erst in **RE1.5 `STAGE1.BIN`** `ori a0,zero,0x2000` (ebenso `@0x80105CBC`).

Das Dossier stellt in §3.2 und §5 beide Adressen nebeneinander — „`0x2000 / 6096`, **beide Werte
belegt: @0x80105C54 und @0x8010567C**" — **ohne zu sagen, dass sie aus verschiedenen Binaerdateien
kommen**. In dem Adressraum, in dem der ganze Rest des Dossiers arbeitet (EMZ0.BIN), sagt
@0x80105C54 etwas voellig anderes. Das ist wortwoertlich der Fehler aus
Memory `reai-v2-zitierte-adresse-ist-kein-beleg`.

Und der Quotient selbst: **1,3438 steht in keiner der beiden Dateien.** Er ist das Verhaeltnis
zweier Pistolen-Skalierungen aus zwei Engines mit — laut dem Dossier selbst — **nicht
vergleichbaren ESP-Baenken** („die RE1.5-Bank hat kein Ein-Sprite-Aequivalent zur RE2-Bank",
§3.2). Daraus „9406…12146" fuer den MG-Stoss abzuleiten erzeugt eine Konstante **ohne jede
`@0x…`-Adresse** — genau das, was das STOP-GATE verbietet. Das Etikett „[PORT-MAPPING]" aendert
daran nichts; es ist eine Rate-Zahl mit zwei echten Enden.

---

## 4. `e->rot_y` statt +0x158 — die „Port-Naeherung" ist eine ANDERE GROESSE, keine Naeherung

@0x80107FC0 `jal 0x800154ac`, Argumente `lw a0,56(s2)` / `lw a1,64(s2)` (Gegner X/Z) gegen
`lw a2,-976(0x800d…)` = 0x800CFC30 / `lw a3,-968(…)` = 0x800CFC38 (Spieler X/Z).
`0x800154AC` selbst gelesen (RE2-EXE): `subu s0,a2,a0` (dx), `subu a0,a3,a1` (dz),
`sll a0,a0,12`, `div a0,s0`, `jal 0x8008d190`, Quadranten-Fixups 1024/2048/3072/4096 —
**ein atan2, der Winkel VOM ZOMBIE ZUM SPIELER**, bei jedem Treffer neu. @0x80107FD8 legt ihn
nach +0x158, @0x8010805C/@0x801080BC liest ihn als `a1`.

`e->rot_y` ist die **Eigen-Blickrichtung des Zombies**. Beide sind nur dann gleich, wenn der
Zombie den Spieler exakt anschaut. Die Abweichung ist der gesamte Peilfehler, bis **±180°** —
z.B. bei jedem Treffer von hinten oder von der Seite genau entgegengesetzt. Bei einer Waffe, die
pro Sekunde mehrere Stoesse setzt, ist das kein Rundungsfehler, sondern die Ausrichtung des
Sprites/Geschwindigkeitsvektors. Das Dossier nennt es „Port-Naeherung" und setzt die Zeile
trotzdem in einen Block, dessen Kopfkommentar `byte-true FUN_80107EF0 @0x80107FC8-0x801080C4`
lautet — **fuer diese Zeile ist der Kopf falsch**.

---

## 5. Die Kette „einmal pro Patrone" — Glied 1 haelt (mit Zusatz), Glied 2 und 3 NICHT

**Glied 1 — `sw v0,4(s1)` @0x80047288 nullt die Phase: HAELT.** Gelesen:
```
8004727c: lh    v1,342(s1)      ; HP
80047280: addiu v0,zero,2
80047284: bgez  v1,0x80047294
80047288: sw    v0,4(s1)        ; Delay-Slot: laeuft IMMER
8004728c: addiu v0,zero,3
80047290: sw    v0,4(s1)
80047294: addiu v0,zero,1
80047298: sb    v0,466(s1)
```
Wort-Store, Little-Endian → +0x4=2, **+0x5=0, +0x6=0, +0x7=0**. +0x6 ist die Phase (7EF0 liest sie
@0x80107F40 `lbu v1,6(s2)` und setzt sie @0x80107F98 auf 1) → ja, jeder Treffer startet Phase 0 neu.
**Zusatz, den das Dossier verschweigt:** derselbe Store nullt auch **+0x5**, also die WAFFENZEILE.
Der Zeilen-Stempel @0x80041AB4 muss danach laufen, sonst dispatcht der naechste Treffer auf
Zeile 0 (= die Nicht-Pointer-Zeile aus §1). Diese Reihenfolge belegt das Dossier nirgends; der
Port setzt sie voraus (`re15_damage.c:2371` dann `:2373`).

**Glied 2 — „alle 8 Frames eine Patrone": NICHT BELEGT.** Gelesen @0x8006A0CC:
```
8006a124: addiu v0,zero,15
8006a128: beq   s1,v0,0x8006a138     ; Id 15
8006a12c: addiu v0,zero,18
8006a130: bne   s1,v0,0x8006a184     ; Id 18
8006a13c: lhu   v0,23580(v0)         ; 0x800D5C1C
8006a144: addiu v0,v0,1
8006a158: slti  v0,v0,8
8006a15c: bne   v0,zero,0x8006a218
8006a168: sh    zero,23580(at)
8006a178: jal   0x800694b8
```
Das ist **eine Patrone pro 8 AUFRUFEN**, nicht pro 8 Frames. Dass `FUN_8006A0CC` genau einmal je
Frame laeuft, steht nirgends im Dossier und habe ich nicht belegt gefunden. Die Zahl „3,75
Blutstoesse/s" in §2.4 haengt vollstaendig an dieser ungeprueften Annahme.

**Glied 3 — „jede Patrone loest einen Treffer auf": NICHT GEPRUEFT.** Das raeumt das Dossier in
§6.5 selbst ein („ist **nicht** nachgelesen") — steht in §0 aber als Tatsache
(„laeuft Phase 0 … **pro Patrone** erneut"). Damit traegt die Ueberschrift des Dossiers mehr, als
die Belege hergeben.

---

## 6. Was ich ZUSAETZLICH bestaetigt habe (damit klar ist, was NICHT strittig ist)

* Poise-Tabelle @0x8010CC33, eigener Dump `8010cc30: 88 78 10 80 09 0f 0f 23 00×8 14 00…` —
  Zeile 1=9, 2=15, 3=15, 4=35, 13=20, **15=0, 18=0** ✔ (bit-genau wie im Dossier).
  Geladen @0x801055D8 `lbu v1,-13261(at)`, indiziert mit `lbu v0,5(s1)`, abgezogen von +0x223.
* +0x93-Schloss: eigener Voll-Scan nach Load/Store mit imm=147 →
  **EMZ0.BIN 0, re2leon/PSX.EXE 0, RE1.5 STAGE1.BIN 189** ✔
* Spawn-Zensus `jal 0x8001bf10` / `jal 0x80019700`: **EMZ0 88, RE2-EXE 146, RE1.5-STAGE1 98** ✔
  Handler-Zaehlungen 7EF0 = 1, 5438 = 1, 4F40 = 4 ✔
* Packung `FUN_8001BF10`: `srl t6,a0,24` (id), `srl v0,a0,16 / andi 0xff / andi 0x7` (sub, **&7**
  fuer den Tabellenindex), `addiu t2,zero,96` (96 Slots) ✔
* Zeilenstempel @0x80041A9C `sb v1,466(t0)` und @0x80041AB4 `sb v1,5(t0)` ✔
  (Das Dossier laesst in seinem Zitat @0x80041AAC `lw t0,104(sp)` still weg — der `sb` schreibt in
  die Entity, nicht in den Treffercode. Kosmetisch, aber es verdeckt einen Registerwechsel.)
* RE1.5-Tabelle @0x8011FB90, Stride 0x20: Zeile 12 (Ingram) **komplett 0** ✔,
  Zeile 5/6 = 0x80106290, Zeile 19 = 0x80106048 ✔
* Port: `re2z_blood_fx_at → re2z_blood_fx_scaled(…, 0x1500, 0)` (`enemy_ai_re2_zombie.c:1016-1019`)
  und `re2z_blood_fx_dir → (…, 0x2000, 8)` (`:1121-1124`) ✔;
  `re15_esp_fx_spawn_ex` ist EIN Slot (`re15_esp.h:206-210`) — „1 Sprite gegen 9" stimmt ✔
* **§3.4 stimmt, und der Port-Kommentar dort ist nachweislich falsch.** `:4942` schreibt
  „Anker `addiu a2,s1,244` @0x801080C0 = Part 1 (Brust), **OHNE Zonen-Verzweigung**" — die
  Verzweigung existiert sehr wohl (@0x80107FF4), und fuer den gestempelten `+0x1D2 = 1`
  (`re15_damage.c:2366`) gilt `1 % 3 = 1` → Zweig A → **Part 0** @0x80108064.

## 7. Ueberzogene Saetze und kleinere Fehler

* **„nur SMG (15) und Gatling (18)"** — die **Zeilen** 15/18 sind byte-belegt, die **Waffennamen**
  sind es NICHT. Fuer „Zeile 15 = SMG/Ingram", „Zeile 18 = Gatling", „Zeile 5,6 = Magnum
  (900 Schaden)", „Zeile 17 = Rakete" nennt das Dossier keine Quelle. Die einzige Stuetze ist,
  dass 15/18 sich @0x8006A124-30 den schnellsten Auto-Takt teilen — und selbst das setzt voraus,
  dass `a0` von `FUN_8006A0CC` derselbe Id-Raum ist wie `(Treffercode>>16)+1` @0x80041AB4. Nirgends
  gezeigt. Plausibel, unbewiesen.
* §0 „Streuung ~±256 X/Z und **1020 Y**": Y = 312 − 4·rand laeuft von +312 bis **−708**, ist also
  nicht um 0 zentriert. „±" ist fuer Y falsch.
* §2.5 „Poise-Bruch unerreichbar, **weil** die Kosten-Tabelle Zeile 15/18 = 0" — **falscher
  Mechanismus**. Die Tabelle wird @0x801055D8 gelesen, also **innerhalb 0x80105438 (MAIN)**, wohin
  Zeile 15/18 nie kommt. Der tragende Grund ist ein anderer: im gesamten Bereich
  0x80107EF0-0x80108184 gibt es **keinen einzigen Zugriff auf +0x223** (eigener Zensus aller
  `547(`-Zugriffe in EMZ0.BIN: 0x8010089C, 0x8010094C, 0x80102D48, 0x80104034, 0x801045E8,
  0x80104A18, 0x8010506C, 0x801050C8, 0x801055C4, 0x801055EC, 0x80105620, 0x80106010, 0x80106030 —
  keiner davon im 7EF0-Block). Ergebnis richtig, Begruendung falsch —
  Memory `reai-v2-fix-erklaert-befund-nicht`.
* §2.6 „0x80105BC0 STAGGER … 5 Spawns": meine Bereichszaehlung [5BC0,66FC) ergibt 22. Das Dossier
  nennt „Funktionsgrenzen ueber `addiu sp,sp,-N`", zeigt sie aber nicht — als geschrieben nicht
  nachpruefbar.
* §5 Schritt 2: `re15_esp_fx_spawn_rows` wird „genau der Zwilling von FUN_8001BF10" genannt;
  `re15_esp.h:238` dokumentiert es gegen **FUN_80019700** (den RE1.5-Spawner). Andere Funktion.
* §5 baut die Zonen-Verzweigung von Hand nach, obwohl der Port sie **schon hat**:
  `re2z_blood_anchor()` `enemy_ai_re2_zombie.c:1119` = `((hits1d2 % 3) == 0) ? 1 : 0` — exakt die
  Regel aus @0x80107FF4/@0x80108064/@0x801080C0. Unnoetige Zweitimplementierung.

---

## 8. Risiko des Umsetzungsplans

1. **`enemy_ai_re2_zombie.c:4942`, Anker von fix `1` auf die Zone:** der Port fuettert
   `re2z_hits1d2` nur ueber eine Naeherung (`re15_damage.c:2360-2368`: Sonde, sonst
   `elev < 0 ? 0 : 1`). Ohne gueltige Sonde ist die Zone **immer 1** → immer Zweig A → Zweig B
   (6096, Part 1) wird **toter Code**, der als „byte-true" im Baum steht und nie laeuft.
2. **`enemy_ai_re2_zombie.c:4942`, fuenf neue `re2z_rand()`-Zuege:** derselbe RNG-Strom speist
   die Clip-Wahl `:4936` und `re2z_thrust(… re2z_rand() & 0x3f)` `:4950` und alles danach im
   Frame. Mit dem Port-Takt `player_common.c:423` (`*schuss = ((f & 4) == 0)` → **4 Schuesse je
   8 Frames** statt RE2's 1 je 8) sind das **4× so viele Zuege wie im Original** — jeder
   RNG-Determinismus-Pin (Memory `reai-v2-rng-determinism`) verschiebt sich.
3. **Umstieg auf `re2z_gore_fx_ex` (`:1041`):** das ruft `re15_esp_fx_spawn_rows`, also **N Slots
   statt 1**. Gegen einen 96-Slot-Pool (`addiu t2,zero,96` @0x8001BF10) und bei 4 Schuessen je
   8 Frames ist Pool-Erschoepfung realistisch — dann fallen **andere** Effekte still aus
   (Muendungsfeuer, Huelsen), und das Symptom waere „weniger Blut", nicht mehr.
4. **Schritt 2 fasst den PISTOLEN-Pfad an** (`re2z_blood_fx_dir`s `splatter_n = 8`,
   `enemy_ai_re2_zombie.c:1124`). Das ist die bereits vom Nutzer abgenommene Darstellung —
   Regressionsrisiko an akzeptiertem Verhalten, ohne Auftrag.
5. **Der Faktor aus §3:** landet ohne Adresse im Auslieferungswert.

## 9. Fehlende Messung (die den Plan entscheidbar machen wuerde)

**Es hat nie jemand Sprites gezaehlt.** Gebraucht wird ein Zaehlvergleich pro Treffer:
im Original ueber einen DuckStation-Savestate der RE2-Leon-Disc die Slot-Belegung hinter
`FUN_8001BF10` (Pools ueber 0x800D4E18 / 0x800D4CD8, 96 Slots) je einmal fuer einen
Pistolentreffer und einen Ingram-Treffer — insbesondere `uVar1` = die **Zeilenzahl der
Sub-Records 0 UND 1** der RE2-Blutbank, denn genau darauf stuetzt sich §2.3s Behauptung
„zwei Sprite-Saetze mit je eigener Sprite-Zahl", ohne dass die beiden Zahlen jemals gelesen
wurden; im Port dieselbe Groesse ueber `re15_esp_fx_count()` vor/nach einem Treffer.
Ohne diese eine Messung ist „MG = viel mehr Blut" nur ein Skalenwert, und Schritt 2 des Plans
(„die Sprite-Zahl kommt aus der Bank") ist eine Annahme ueber ungelesene Daten.

Zweitens, billiger und ebenso entscheidend: ein Aufruf-Zaehler auf `FUN_8006A0CC` (ein Frame vs.
ein Aufruf), der Glied 2 der Kette in §5 entweder traegt oder kippt.
