# Runde 30 / Welle 2: die MÜNDUNGSHÖHE und das fünfte Tor — **gebaut**

Status: **FERTIG** (2026-09-27). `bash re15_port/tools/local_build.sh all` =
`=== LOCAL-BUILD-OK (all) — Tests 355/355`, alle vier GUI-Haken im ERSTEN Lauf grün.
Lauf-Protokoll: `analysis/befunde_2026-09-27/muendungshoehe-und-fuenftes-tor.log`.

Auftrag, wörtlich: *„na dann baue ein was fehlt im Port!"*
Nutzer-Befund seit Runde 26: *„Im Original Resident Evil 2 sind die Hunde erst dann wieder
verwundbar, sobald sie wieder stehen."*

> **Kurzurteil: der liegende Hund ist jetzt untreffbar, bis er steht — gemessen, nicht
> behauptet.** Unter Dauerfeuer: vorher **17 Treffer in 389 Bildern**, während er lag, und er
> kam in 400 Bildern **nie** wieder hoch. Jetzt **0 Treffer in 77 Bildern**, nach **78 Bildern**
> steht er wieder und ist wieder treffbar. Der stehende Hund wird unverändert getroffen.

---

## 1. Was gefehlt hat

Welle 1 hatte die beiden Hälften des Mechanismus gefunden, aber nur eine gebaut:

| | Welle 1 | Welle 2 |
|---|---|---|
| Hitbox-Stauchung `FUN_80104088` @0x80104090-D8 | ✅ gebaut | — |
| **Zielhöhe** `lw a0,4(s4)` @0x8004718C | ❌ fehlte | ✅ gebaut |
| **Fünftes Gate** @0x8004716C-A4 | ❌ nicht gebaut | ✅ gebaut |

Welle 1 hat den Einbau des Gates zu Recht abgelehnt: der einzige Port-Baustein für die
Zielhöhe (`re15_player_gunbone_world`) hängt am PC-Renderer und war headless **0 von 1200
Bildern** gültig. Ein Tor auf einem Feld, das nie gefüllt wird, sperrt dauerhaft — die Falle
aus Runde 14.

---

## 2. Die Zielhöhe — selbst disassembliert

`s4` ist das erste Argument von `FUN_800470C0`. Am Schuss-Pfad @0x80042F94 (Funktion
@0x80042C64, Rahmen −104), `info/re2leon/PSX.EXE`:

```
80042e14: addiu s0,s1,36        ; s0 = Spieler-MATRIX (+0x24)
80042e18: lw    s2,408(s1)      ; s2 = Part-/Posen-Block (+0x198)
80042e60: addiu s0,sp,32        ; ZIEL-MATRIX auf dem Stack
80042e64: jal 0x8002ce94   (a0 = Spieler-MATRIX, a1 = s2+24,   a2 = sp+32)
80042e74: jal 0x8002ce94   (a0 = sp+32,          a1 = s2+1572, a2 = sp+32)
80042e84: jal 0x8002ce94   (a0 = sp+32,          a1 = s2+1744, a2 = sp+32)
80042e94: jal 0x8002ce94   (a0 = sp+32,          a1 = s2+1916, a2 = sp+32)
80042f8c: addiu a0,sp,52        ; a0 = &MATRIX.t[0]  -> s4
80042f94: jal   0x800470c0
```

**`0x8002ce94` ist das PsyQ-Matrix-Compose**, ebenfalls selbst disassembliert
(@0x8002CE94-CF30) — kein Decompilat, sondern die Instruktionen:

```
8002ce94-ceb0: lw 0/4/8/12/16(a0) -> ctc2 cr0..cr4   ; Eltern-ROTATION in die GTE-Kontrollregs
8002cebc-ced0: lw 20/24/28(a0)    -> ctc2 cr5..cr7   ; Eltern-TRANSLATION (TRX/TRY/TRZ)
8002ced4-cee8: lhu 0/6/12(a1)     -> mtc2 r9..r11    ; Spalte 0 der KIND-Matrix (IR1..IR3)
8002cef4:      MVMVA                                  ; R * V (+ TR)
8002cf00-cf14: mfc2 r9..r11 -> sh 0/6/12(a2)          ; Spalte 0 der Ausgabe
8002cef8/cefc: addiu a0,a1,2 / addiu v1,a2,2          ; naechste Spalte
```

Die vier Kind-Offsets **24 / 1572 / 1744 / 1916** liegen **172 (0xAC)** auseinander
(1572 = 24 + 9·172, dann +172, +172) — das ist der Part-Stride des Modell-Pools. Die Kette ist
also **Bone 0 → 9 → 10 → 11**, und das Ende ist **Bone 11**, derselbe Knochen, den der Port
schon als Hand-/Waffen-Bone führt (`kine+0x7b8` = 11·0xAC + 0x40 + 0x14).

PsyQ-`MATRIX` = 3×3 `short` (18 B) + 2 Pad + `t[3]`; bei einer Matrix @sp+32 liegt `t[0]`
@sp+52, `t[1]` @sp+56, `t[2]` @sp+60 — genau die Slots, die die Funktion danach als Vektor
weiterreicht (`lw 52/56/60(sp)` @0x80042FCC-EC).

⇒ **`lw a0,4(s4)` @0x8004718C liest die WELT-Y-KOORDINATE DES WAFFEN-BONE.** Nicht die
Fußhöhe, nicht die Kamera, nicht den Zielpunkt.

**Port-Gegenstück, neu:** `re15_player_muzzle_world` (`re15_damage.c`) — posiert Leons
PL00-Bank als QUERY (Crossfade unangetastet, wie `re15_enemy_bone_world_pos`) und dreht Bone 11
über `re15_skel_bone_to_world` in die Welt. **Engine-seitig**, also headless *und* auf der PSX
verfügbar. Rückgabe 0, wenn die Bank fehlt — dann darf niemand gaten.

> **Kein geratener Zahlenwert.** Die Mündungshöhe ist keine Konstante, sondern das Ergebnis der
> Pose. Der offene Punkt aus Welle 1 („Leons echter Mündungs-Y in RE2 ist nicht gemessen") ist
> damit gegenstandslos geworden: es wird nichts gegen eine Zahl geprüft, sondern die Rechnung
> nachgebaut. Dass das Ergebnis (Hgun ≈ 1665) in das aus dem Nutzer-Befund folgende Intervall
> [1100, 2100) fällt, ist eine **Gegenprobe**, keine Eingabe.

---

## 3. Das fünfte Gate — selbst nachgelesen

`python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x80047118 50`:

```
8004712c: andi v0,v0,0x1        -> Gate 1 (aktiv)
80047138: lbu v0,467(s0)        -> Gate 2 (Trefferpause +0x1D3)
80047148: lh  v0,342(s0)        -> Gate 3 (hp < 0)
80047158: lhu v0,270(s0)        -> Gate 4 (+0x10E & 0xC000)
8004716c: lhu v1,464(s0)        ; +0x1D0 (nur Seiteneffekt)
80047170: lh  a0,152(s0)        ; b = +0x98 SIGNED
80047174: lw  v0,60(s0)         ; eY = MATRIX.t[1] des Gegners (+0x3C)
80047178: andi v1,v1,0xff00
8004717c: addu v0,v0,a0
80047180: addiu v0,v0,100
80047184: sh  v1,464(s0)
80047188: lhu v1,158(s0)        ; h = +0x9E UNSIGNED
8004718c: lw  a0,4(s4)          ; ZIELHOEHE = Muendungs-Y
80047190: addu v0,v0,v1
80047194: subu v0,v0,a0
80047198: addiu v1,v1,100
8004719c: sll v1,v1,1
800471a0: sltu v0,v0,v1         ; UNSIGNED
800471a4: beq v0,zero,0x8004740c
```

`0x8004740c: addiu s2,s2,4` / `bne s2,v0,0x8004711c` @0x80047418 — **dieselbe** Kandidaten-
schleife wie die vier bekannten Gates. **Es sind fünf Gates, nicht vier.**

Als Formel, `Hgun := eY − Mündungs-Y`:

> **DURCH ⟺ −(b+h+100) ≤ Hgun < (h − b + 100)**

Für symmetrische Boxen (b = −h) ist das **[−100, 2h+100)**: die Mündung muss senkrecht in der
Trefferzone des Gegners liegen (±100 Schlupf).

---

## 4. Gemessen — der Befund selbst

`probe_r30b_muendung`, echter Weg (`re15_game_step` + Pad, echte RDTs, RE2-KI-Geschmack,
RE2-Bank `shared_assets/RE2/CDEMD0.EMS` geladen — ohne sie hätte jeder Clip Länge 0,
die Falle aus Runde 29).

### 4.1 Die Mündungshöhe ist da

| Typ | Raum | Mündung gültig | MündungY | Hgun |
|---|---|---|---|---|
| ZOMBIE 0x10 | ROOM1140 | **240/240** | −1671…−1665 | 1665…1671 |
| HUND 0x20 | ROOM1190 | **240/240** | −5349…−2197 | 1649…2085 |
| KRÄHE 0x21 | ROOM10C0 | **240/240** | −1921…−1665 | 1420…1671 |
| BABY 0x26 | ROOM1090 | **240/240** | −3471…−3465 | 1665…1671 |

Vorher (Welle 1, `re15_player_gunbone_world`): **0/1200**.

### 4.2 Der Nutzer-Befund, unter Dauerfeuer

„ALT" = `re2_hit_box_set` jedes Bild auf 0 zurückgesetzt, das Gate ist inert — also exakt der
Code-Pfad vor dieser Runde. Kein Env-Schalter im Spielcode.

| | **ALT** | **NEU** |
|---|---|---|
| Treffer am **stehenden** Hund | 1 | **1** (unverändert) |
| Treffer, **während er liegt** | **17 in 389 Bildern** | **0 in 77 Bildern** |
| wieder auf den Beinen nach | **nie** (400 Bilder Budget) | **78 Bilder** |
| Treffer **nach dem Aufstehen** | 0 / 0 (kam nie hoch) | **1 in 60 Bildern** |

Der ALT-Lauf zeigt nebenbei, **warum es dem Nutzer auffiel**: unter Dauerfeuer wurde der Hund
im Port immer wieder neu getroffen und kam nie mehr hoch.

### 4.3 Die Kette Bild für Bild (Tor gegen die echte Mündung)

| Bild | Zustand | Box | Hgun | Tor |
|---|---|---|---|---|
| Treffer | HURT-P0, Clip 17 | −500/500 | 1670 | **SPERRT** |
| +1 … +11 | Clip 17 | −500/500 | ~1670 | SPERRT |
| +12 … +20 | Clip 18 (Hinfallen) | −500/500 | ~1669 | SPERRT |
| +21 … +70 | Clip 7 (Aufstehen) | −500/500 | 1665 | SPERRT |
| **+71** | state 1/6, ACTIVE | **−1000/1000** | 1665 | **DURCH** |

Die Trefferpause +0x1D3 endet schon bei **+14** — sie war nie die Erklärung. Die restlichen
**57 Bilder** deckt jetzt das Tor ab.

---

## 5. ⛔ Die Runde-14-Sicherung: `re2_hit_box_set`

Das Gate wertet +0x98/+0x9E **nur** aus, wo der Port sie byte-true führt. Mit b/h = 0/0 wäre
das Fenster **[−100, 100)** und **jeder** Gegner für immer untreffbar — das ist die
Hitbox-Variante genau der Dauersperre aus Runde 13/14.

Heute schreibt die Werte ausschließlich der **HUND 0x20** (`re2d_init` @0x8010028C-9C,
`re2d_hitbox` @0x80104090-D8). `re15_re2dog_tick` läuft nur unter
`re15_ai_re2_for_type(0x20)`, also bleibt der RE1.5-Pfad unberührt; Aktor-Slots werden bei
`re15_actor_alloc` genullt, eine Altmarke kann nicht überleben.

Zweite Sicherung: liefert `re15_player_muzzle_world` 0, wird **nicht** gegatet.

### Gegenprobe — kein Typ dauerhaft untreffbar (200 Bilder Dauerfeuer je Typ)

| Typ | Raum | Treffer | `re2_hit_box_set` |
|---|---|---|---|
| ZOMBIE 0x10 | ROOM1140 | 19 | 0 |
| HUND 0x20 | ROOM1190 | 3 | **1** |
| KRÄHE 0x21 | ROOM10C0 | 10 | 0 |
| SPINNE 0x25 | ROOM1090 | 19 | 0 |
| BABY 0x26 | ROOM1090 | 10 | 0 |

---

## 6. NICHT gebaut — mit Zahl, nicht mit Bauchgefühl

* **Das Gate für KRÄHE 0x21.** Fenster [−280, 980). Gemessen: Hgun 1420…1671 ⇒ **0/240 durch**.
  Sie wäre dauerhaft untreffbar. (Krähen im Original fliegen höher; der Port hat dafür keine
  gemessene Grundlage.)
* **Das Gate für BABY 0x26.** Fenster nur [−100, 120). Gemessen Hgun 1665…1671 ⇒ **0/240
  durch**, und das Overlay hat **genau einen** Schreiber auf +0x98/+0x9E (@0x80100168-78) — es
  gibt keinen Zustand, der den Wert höbe.
* **Die +0x98/+0x9E-Werte für 0x10 / 0x21 / 0x25 / 0x26 überhaupt.** Belegt sind sie
  (Welle-1-Dossier §3.1), aber ihre Setz-Stellen sind je Overlay ein Dutzend Zustände (Zombie
  allein 14) — eigene Runde, eigene Messung. Ohne Setz-Stellen kein `re2_hit_box_set`, ohne
  `re2_hit_box_set` kein Gate.
* **Die word0-Bits von `FUN_80104088`** (Maske 0xE7FFFFFF @0x80104090-B4 + 0x04000000
  @0x801040AC; 0x0C000000 ohne Maskierung @0x801040CC). Der Port führt sie nicht — als
  Fehlstelle vermerkt, **nicht** auf ein fremdes Feld gebogen.
* **Der Seiteneffekt des Tores** (`andi v1,v1,0xff00` @0x80047178 + `sh v1,464(s0)`
  @0x80047184 löscht das untere Byte von +0x1D0 bei JEDEM Kandidaten, auch bei einem, den es
  gleich darauf verwirft). Der Port führt +0x1D0 nicht.

---

## 7. Berichtigt (waren falsch im Code)

1. `re15_damage.c` — *„RE2s EIGENER Kandidatenfilter hat überhaupt kein Höhen-Band …
   @0x8004716c geht es direkt zur Trefferprüfung"*. Falsch; Welle 1 hatte es widerrufen,
   Welle 2 ergänzt: das Gate ist jetzt gebaut.
2. `re15_damage.c` — *„RE2s Kandidatenfilter hat KEIN Höhen-Band (FUN_800470C0, genau vier
   Gates @0x8004712c/38/48/60)"*. Es sind **fünf**. Widerrufen an Ort und Stelle, mit dem
   Hinweis, dass das fünfte Gate **kein** Band-Ersatz ist (es prüft die Mündungshöhe gegen
   +0x98/+0x9E, während die Höhen-Selektivität dort unten ein dy-Fenster je Waffe ist).
3. `enemy_ai_re2_dog.c` — `re2d_hitbox` als *„dokumentierter NOP"*. Seit Welle 1 byte-true,
   seit Welle 2 auch gelesen.

---

## 8. Offen

1. **Kein Bildbeleg.** Die Sitzung läuft in einer REMOTEDESKTOP-Sitzung; gemessen wurde
   ausschließlich über `re15_game_step` + Pad. Nicht auf `RE15_AUTOSHOT` oder den
   Softwarerenderer ausgewichen.
2. **Welchen Ursprung die NAHKAMPF-Aufrufer von `FUN_800470C0` übergeben, ist nicht geprüft.**
   Ausgewertet ist der Schuss-Pfad @0x80042F94 (1 von 17 Aufrufern). Das Gate sitzt im Port in
   der gemeinsamen Kandidatenschleife, wertet für das Messer also dieselbe Bone-11-Höhe. Der
   Port ankert seinen Klingenpunkt ohnehin an Bone 11 (`s_hand_world`), der Knochen stimmt
   also — der *Offset* innerhalb des Knochens ist ungeprüft.
3. **Die Vorschubrate der HURT-Clips** (+0x15A: 512 @0x80103518 in P1, 256 @0x801036E0 in P2)
   ist weiter nicht gegen den Port geprüft; der Port advanciert gemessen 1 Bild je Tick. Das
   betrifft die Kettenlänge (71 bzw. 78 Bilder), nicht das Tor.
4. **Maßstabs-Abgleich RE2 ↔ Port** bleibt offen (RE2 gibt dem Spieler −1530/1530
   @0x8005742C-3C). Er wird für dieses Tor aber nicht mehr gebraucht: gerechnet wird mit der
   *im Port gemessenen* Bone-11-Höhe, nicht mit einer importierten RE2-Zahl.
5. **0x25 hat zwei `sh zero,152`-Stellen** (@0x8010049C auf s2, @0x8010276C auf s0); welche im
   Spiel läuft, ist weiter nicht auseinandergehalten. Folgenlos, solange 0x25 kein
   `re2_hit_box_set` trägt.
