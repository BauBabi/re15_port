# Pruefung: raum11d0-bluteffekte.md — Urteil: NICHT HALTBAR (Plan), Mechanismus im Kern belegt

Geprueft am 2026-09-26. Keine Engine-Datei geaendert, nicht gebaut.
Alle Adressen unten habe ich SELBST aus der Binaerdatei gelesen (re15_disasm.py bzw. Python
direkt auf die Datei), nicht aus einem Decompilat uebernommen.

## 0. Zusammenfassung

Der **Mechanismus** des Dossiers haelt: die Freigabe schreibt tatsaechlich 0x0C01, RE1.5-ACT[12]
ist tatsaechlich der effektfreie Aufsteh-Leaf, RE2-Sub[12] ist tatsaechlich der Post-Latch mit dem
Blut-Schwanz. 12 von 12 nachgepruefte Adressen stimmen.

Der **Umsetzungsplan faellt trotzdem**, an §5 Punkt 4. Die vorgeschlagene Weiche
`if (e->state == 1 && e->sub_state_1 == 0x0c) { re15_dog_release0c(e); return; }` vor
`enemy_ai_common.c:7583` kann die beiden Herkuenfte von Sub 12 nicht unterscheiden und wuerde den
ECHTEN RE2-Post-Latch mit abschneiden. Das Dossier behauptet woertlich das Gegenteil.

## 1. Selbst nachgelesene Adressen (12 Stueck, alle bestaetigt)

### 1.1 Freigabe @0x8011172c — STAGE1.BIN, @0x80100000, kein Header-Offset — STIMMT
```
8011170c: lbu v1,9(a0)                 ; grid
80111710: ori v0,zero,0x42
80111714: bne v1,v0,0x80111754
80111718: ori v0,zero,0xc01            ; Delay-Slot
8011171c: sb  zero,9(a0)
80111720: lui v1,0x800b
80111724: lw  v1,-14460(v1)            ; g_entity(cur)
8011172c: sw  v0,4(v1)                 ; entity+0x4 = 0x00000C01
```
Little-Endian: +0x4=0x01, +0x5=0x0C, +0x6=+0x7=0. Exakt wie im Dossier.
Ebenso bestaetigt: 0x80111688-0x801116fc (Phase 1, Clip 8, +0xbc/+0xbe += 300, `jal 0x8001f314`).

### 1.2 Tabellen — beide existieren, beide zeigen wie behauptet
* `STAGE1.BIN` ACT-Tabelle @0x80120FD4: Rohwort **[12] = 0x801101e4** ✔
* `STAGE1.BIN` DECIDE-Tabelle @0x80120F94: Rohwort **[12] = 0x801101dc** ✔;
  0x801101dc = `jr ra / nop` ✔ (leer)
* `EMD0G_MOD0.BIN` (RE2-Cerberus-Overlay, 0x56FA B, @0x80100000 → Disasm ist sauberes MIPS)
  Sub-Tabelle @0x80105464: Rohwort **[12] = 0x80102608** ✔; alle 17 Eintraege deckungsgleich
  mit der Dossier-Liste.
* 0x801101e4 selbst gelesen: Phasen-Automat auf +0x6, **einziger `jal` = 0x8001f314 (anim_set)
  @0x80110288**, Ausgang `sb v0,5(a0)` mit v0=2 @0x801102b0 + `sb zero,6(v0)` @0x801102c0.
  Null Effekte ✔.
* Member_set-Tabelle EXE @0x80010c8c: **[12] = 0x800411f4**; dort `j 0x80041230` /
  Delay-Slot `sb a2,9(a0)` ✔ (grid := 66).

### 1.3 Blut-Schwanz @0x80102804 / @0x80102844 — existiert, ist aber GEGATET
```
801027dc: lw  v0,376(s0)          ; +0x178
801027e4: lw  s1,0(v0)
801027e8: lui v0,0x3              ; 0x30000
801027ec: and v0,s1,v0
801027f0: beq v0,zero,0x8010284c  ; <-- KEIN Blut ohne diese Frame-Bits
801027fc: sb  s2,543(s0)          ; +0x21F = 1
80102804: jal 0x80105070          ; FX(4,0)
80102810: and v0,s1,v0            ; 0x20000
80102814: beq v0,zero,0x8010284c  ; <-- zweites Gate
80102828: bne v0,zero,0x8010284c  ; rng&3 != 0 -> raus  (1/4-Chance)
80102844: jal 0x80105070          ; FX(4, 1|2)
```
FX-Tabelle @0x801056AC (6 B/Eintrag) selbst gelesen:
`00 01 …` (fx0 = Art 0/Unterart 1) / `09 00` / `09 01` / `09 02` → die Dossier-Zuordnung stimmt.
0x80105070 prueft @0x80105090 `lbu v0,543(a3); beq v0,zero,…` — der Caller schaltet frei.

**Antwort auf die Frage „jedes Bild oder nur einmal?": WEDER noch.** Kein Einmal-Latch (das
Budget +0x21F wird gesetzt, nicht geprueft) — aber auch nicht bedingungslos jedes Bild, sondern
**jedes Bild, in dem das EDD-Frame-Wort Bit 16 oder 17 traegt**. Der zweite Spritzer zusaetzlich
nur mit 1/4-Wuerfel. Dass es in ROOM11D0 wie „jedes Bild" wirkt, liegt daran, dass RE2-Sub 12
Phase 0 (@0x801026B4-E4) den Schuettel-Clip 14 mit Zufalls-Startframe aufzieht, dessen Frames
diese Bits tragen.

## 2. Falsche / nicht tragende Behauptungen

| Stelle | Behauptet | Tatsaechlich |
|---|---|---|
| §5.4 | „`re2d_sub12_postlatch` bleibt unangetastet und **erreichbar** ueber den echten RE2-Latch-Weg (`re2d_sub7_latch` → sub 12), es geht nur der FALSCHE Eintritt … weg" | **Falsch.** Die Weiche aus §5.2 greift auf `state==1 && sub_state_1==0x0c` — ohne Herkunftsmerkmal. Der echte RE2-Weg erzeugt exakt denselben Zustand: `EMD0G_MOD0.BIN` @0x80102218 `addiu v0,zero,12` / @0x8010221c `sb v0,5(s1)` / @0x80102220 `sh zero,6(s1)`, im Port `enemy_ai_re2_dog.c:1221`. Die Weiche faengt ihn mit ab und macht sub 12 **unerreichbar**. |
| §4 / Titel | „und der **spritzt pro Bild** Blut (@0x80102804 / @0x80102844)" | Gegatet durch `and s1,0x30000` + `beq` @0x801027e8-f0; der zweite Aufruf zusaetzlich durch `beq` @0x80102814 und `bne` @0x80102828 (1/4). Nur EDD-flagged Frames. |
| §2b | Zustands-Tabelle @0x80120F74 habe zwoelf Eintraege `[8]=0x8010DDB8 [9]=0x8010DF94 [10]=0x8010E0C4 [11]=0x8010E568` | Diese vier Worte SIND die DECIDE-Eintraege [0..3] @0x80120F94 (= 0x80120F74+0x20). Die beiden im selben Absatz aufgefuehrten Tabellen ueberlappen — hoechstens eine der beiden Laengenangaben kann stimmen. Selbst gedumpt. |
| §5a | „ueber alle **206** ausgelieferten RDTs" | `find re15_port/shared_assets/PSX -iname '*.RDT' \| wc -l` = **240** (STAGE1 80, STAGE2 32, STAGE3 32, STAGE4 32, STAGE5 48, STAGE6 16). |
| §5a | ROOM2060/2061/20A0/20A1 fuehrten Typ 0x25 „ebenfalls mit grid 0x41" — im selben Absatz, der die 0x41-Spalte als 0 ausweist | Stimmt sachlich (`44 ?? 25 41` = 2/2/2/2 = 8 Treffer), ist aber im Zensus nicht sichtbar, weil dort nur nach Typ 0x20 gesucht wurde. Die Tabelle in §5a traegt diese Aussage nicht. |

## 3. Zensus — selbst nachgezaehlt (alle 240 RDTs)

`44 [00-1f] 20 41` (Sce_em_set, Typ 0x20, grid 0x41) und `34 0c 42 00` (Member_set(0x0C,66)):

```
ROOM11D0  em41=5  ms42=5      ROOM2060  em41=0  ms42=2
ROOM11D1  em41=5  ms42=5      ROOM2061  em41=0  ms42=2
ROOM3060  em41=3  ms42=3      ROOM20A0  em41=0  ms42=2
ROOM3061  em41=1  ms42=1      ROOM20A1  em41=0  ms42=2
                              ROOM4001  em41=0  ms42=2
                              ROOM5040/5041/5070/5071 je ms42=2
SUMME     em41=14             ms42=32
```
**Die Zahlen des Dossiers stimmen**: vier Raeume mit Typ-0x20-Hunden auf grid 0x41 (11D0/11D1/
3060/3061), zusammen **14 Hunde**, und in genau diesen vier Raeumen ebenso viele Freigaben.
Die fuenf `44 xx 20 41` in ROOM11D0 liegen bei 0x012BE/0x012D2/0x012E6/0x012FA/0x0130E, die fuenf
`34 0c 42 00` bei 0x0172C/0x01734/0x0173C/0x01744/0x0174C — Datei-Offsets wie angegeben.
Auch Kopf (`00 0f 08 00`), mainScd 0x11D8, subScd 0x1684, effect 0x2D04, espTim 0x125BC und der
ESP-Id-Header `07 ff ff ff ff ff ff ff` sind bitgenau so in der Datei.
(Die Gesamtzahl 206 ist die einzige falsche Zahl im Zensus.)

## 4. Was die neue Weiche sonst noch abfaengt

Unter dem ausgelieferten RE2-Flavor fuer Typ 0x20 gibt es **zwei** Produzenten von
`state==1 && sub_state_1==0x0c`, und die Weiche sieht beide gleich:

1. `enemy_ai_common.c:7482` — die RE1.5-Kaefig-Freigabe (@0x8011172c). Das gewollte Ziel.
2. `enemy_ai_re2_dog.c:1221` — der **echte** RE2-Abwurf am Ende von `re2d_sub7_latch`
   (@0x8010221C). Der Hund hat den Spieler gerade abgeschuettelt und soll in den
   Schuettel-/Blut-Clip 14. Mit der Weiche bekaeme er stattdessen RE1.5-Clip 8 und ginge nach
   Sub 2 — nach JEDEM echten Biss, in JEDEM Raum mit Hunden. Das ist eine Regression, kein Fix.

Nicht betroffen (geprueft, kein zusaetzlicher Weg unter RE2):
* `enemy_ai_common.c:8128` (RE1.5-HURT-Recovery @0x80110d60-78) liegt in `case 2` der
  RE1.5-Maschine — unter RE2 uebernimmt `re2d_hurt` @0x801032A8, also unerreichbar.
* `enemy_ai_common.c:7404` `re15_dog_sub(e,0xc)` sitzt in `re15_dog_grabhold`, das nur aus
  `case 9/10` der RE1.5-Maschine (Zeile 7953) gerufen wird — unter RE2 unerreichbar.
* `enemy_ai_common.c:416` (0xc01) und `:2059` gehoeren zum Zombie, nicht zum Hund.

Zusaetzliche kleinere Risiken derselben Weiche:
* Sie kehrt vor `re15_re2dog_tick` zurueck und ueberspringt damit den RE2-ACTIVE-Tail
  (`re2d_frame_word & 0x40000 → SE 8`, `enemy_ai_re2_dog.c:2304`) — im RE1.5-Original haengt an
  Zustand 1 ein eigener Tail (`FUN_8010DBCC` @0x8010dd00-ddb0), den der Port hier ebenfalls
  nicht faehrt. Das Dossier nennt das in §6 selbst als offen; es ist damit kein „byte-true"-Fix,
  sondern ein dritter, in keiner der beiden Engines existierender Pfad.
* `re15_dog_sub(e,2)` (`enemy_ai_common.c:6940`) setzt nur +0x5 und +0x6; +0x7 bleibt stehen.
  RE2-Sub 2 (`re2d_sub2_run` @0x80100B20) startet damit auf einem Fremdwert.

## 5. Methodische Anmerkung zur Reproduktion (§1c)

Der Nachweis lief mit `RE15_DEBUG_JUMP=11D0@120` + `RE15_SUBSTART=2@60#11D0`, also Raumsprung +
erzwungenem Sub-Start — nicht ueber den vom Nutzer beschriebenen Weg (M93R aufnehmen → sub03 →
`Evt_exec` sub02). Fuer die Isolation des Mechanismus reicht das; als Beleg dafuer, dass genau
DAS der Nutzer-Befund ist, ist es schwaecher als es klingt. Ein Original-Vergleich
(DuckStation) fehlt und wird in §6 auch eingeraeumt.

## 6. Urteil

Mechanismus: belegt. Plan: nicht haltbar in der vorgeschlagenen Form, weil die Weiche den echten
RE2-Post-Latch (@0x8010221C → @0x80102608) mitnimmt und §5.4 das Gegenteil behauptet. Eine
Weiche, die nur den RE1.5-Eintritt trifft, braucht ein Herkunftsmerkmal (z.B. eine eigene
Markierung, die die Freigabe in `enemy_ai_common.c:7482` setzt und die der Tick sofort
verbraucht) — nicht `state==1 && sub_state_1==0x0c` allein.
