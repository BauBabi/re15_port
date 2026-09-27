# Runde 30 / Welle 1: das fünfte Tor von `FUN_800470C0` — MESSUNG vor dem Bau

Status: **MESSUNG FERTIG** (2026-09-27). **Kein Verhalten geändert.** Neu sind nur eine Sonde
(`re15_port/tests/unit/probe_r30_zielfenster.c`, kein ctest) und drei Kommentar-Berichtigungen.
Lauf-Protokoll: `analysis/befunde_2026-09-27/zielfenster-messung.log`.

Auftrag: *„na dann baue ein was fehlt im Port!"* — Nutzer-Befund seit Runde 26: *„Im Original
Resident Evil 2 sind die Hunde erst dann wieder verwundbar, sobald sie wieder stehen."*

> **Kurzurteil: das fünfte Tor allein ist NICHT baubar.** Es prüft die Mündungshöhe gegen
> +0x98/+0x9E. Der Port führt **keines der beiden Felder** und **keine Mündungshöhe** — sein
> heutiger senkrechter Eingang ist `pl->y`, die **Fußhöhe**. Damit steht Hgun auf **0** und das
> Tor ließe **alle fünf RE2-Typen in allen 240 gemessenen Bildern durch** — ein Einbau ohne die
> Mündungshöhe ist ein wirkungsloser Platzhalter, kein Fix. Was zuerst gebaut werden muss, steht
> in §6.

---

## 1. Das Tor, selbst nachgelesen

`info/re2leon/PSX.EXE`, `python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x800470c0 100`:

```
8004716c: lhu v1,464(s0)     ; +0x1D0
80047170: lh  a0,152(s0)     ; +0x98  b   (SIGNED halfword)
80047174: lw  v0,60(s0)      ; +0x3C  eY  (die MATRIX liegt @+0x24, t[1] = +0x3C)
80047178: andi v1,v1,0xff00
8004717c: addu v0,v0,a0
80047180: addiu v0,v0,100
80047184: sh  v1,464(s0)
80047188: lhu v1,158(s0)     ; +0x9E  h   (UNSIGNED halfword)
8004718c: lw  a0,4(s4)       ; ZIELHOEHE
80047190: addu v0,v0,v1
80047194: subu v0,v0,a0
80047198: addiu v1,v1,100
8004719c: sll v1,v1,1
800471a0: sltu v0,v0,v1      ; UNSIGNED
800471a4: beq v0,zero,0x8004740c
```

`0x8004740c: addiu s2,s2,4` / `bne s2,v0,0x8004711c` @0x80047418 — dieselbe Kandidatenschleife
wie die vier bekannten Gates (@0x8004712C aktiv, @0x80047138 Trefferpause +0x1D3,
@0x80047148 hp<0, @0x80047158 +0x10E & 0xC000). **Es sind fünf Gates, nicht vier.**

Als Formel, mit `Hgun := eY − Zielhöhe` (positiv = Ziel liegt ÜBER dem Gegner, PSX-Y zeigt nach unten):

> **DURCH ⟺ −(b+h+100) ≤ Hgun < (h − b + 100)**

⛔ **Berichtigt:** in `re15_damage.c` stand *„RE2s EIGENER Kandidatenfilter hat überhaupt kein
Höhen-Band … @0x8004716c geht es direkt zur Trefferprüfung"*. Das ist falsch und ist jetzt an
Ort und Stelle widerrufen (auch `enemy_ai_re2_dog.c:2244` „vier Gates").

---

## 2. Die Zielhöhe hinter `lw a0,4(s4)` — aufgelöst

`s4 = a0` (erstes Argument). 17 Aufrufer in der EXE; ausgewertet der Schuss-Pfad
@0x80042F94 in der Funktion @0x80042C64 (Rahmen −104):

```
80042e14: addiu s0,s1,36        ; Spieler-MATRIX (+0x24)
80042e18: lw    s2,408(s1)      ; Pose-/Bone-Block
80042e60: addiu s0,sp,32        ; ZIEL-MATRIX auf dem Stack
80042e64: jal 0x8002ce94  (a1 = s2+24)      \
80042e74: jal 0x8002ce94  (a1 = s2+1572)     |  vier Verkettungen ueber die
80042e84: jal 0x8002ce94  (a1 = s2+1744)     |  Arm-/Waffen-Bone-Kette
80042e94: jal 0x8002ce94  (a1 = s2+1916)    /
80042f8c: addiu a0,sp,52        ; a0 = &MATRIX.t[0]
80042f90: lh    a1,118(s1)      ; Spieler-Yaw (+0x76)
80042f94: jal   0x800470c0
80042f98: addiu a2,sp,72
```

PsyQ-`MATRIX` = 3×3 `short` (18 B) + 2 Pad + `t[3]`. Bei einer Matrix @sp+32 liegt `t[0]`
@sp+52, `t[1]` @sp+56, `t[2]` @sp+60 — **genau die drei Slots, die die Funktion danach als
Vektor weiterreicht** (`lw 52/56/60(sp)` @0x80042FCC-EC, plus `lh 64/68(sp)` als XZ-Schrittweite).

⇒ **`s4` ist der Schuss-Ursprung, `s4+4` = `t[1]` = die WELT-Y-KOORDINATE DER MÜNDUNG.**
Nicht die Fußhöhe des Spielers, nicht die Kamera, nicht der Zielpunkt.

Zwei Nebenbefunde derselben Stelle:
* Für Waffen-Id `(+0x14D) − 7 ∈ [0,4]` wird `t[1]` **vor** dem Aufruf um +200 gesenkt
  (`lw 56(sp)` / `addiu v0,v0,200` / `sw` @0x80042F60-6C) und danach zurückgestellt
  (@0x80042FAC-B8).
* Das Tor hat einen **Seiteneffekt**: `andi v1,v1,0xff00` @0x80047178 + `sh v1,464(s0)`
  @0x80047184 löscht das untere Byte von +0x1D0 bei **jedem Kandidaten**, auch bei dem, den es
  gleich darauf verwirft.

**Port-Gegenstück: es gibt keines.** Siehe §3.

---

## 3. Was tragen +0x98 / +0x9E — Original und Port

### 3.1 Original: Vollzählung aller `sh rt,152(rs)` / `sh rt,158(rs)` je Overlay

| Typ | Datei (roh @0x80100000) | b = +0x98 | h = +0x9E | Adressen |
|---|---|---|---|---|
| 0x10 Zombie stehend | `EMZ0.BIN` | −1500 | 1500 | INIT @0x8010095C/64; zurück @0x80103710-20, @0x80104A1C-28, @0x80107E8C-98 |
| 0x10 Zombie Kriecher | `EMZ0.BIN` | −350 | 350 | @0x80100B14-20, @0x80100BC8-D4, @0x80103460-6C, @0x80106B28-34, @0x801077F8-818, @0x80108998-9A4 |
| **0x20 Hund stehend** | `EMD0G_MOD0.BIN` | **−1000** | **1000** | INIT @0x8010028C-9C; `FUN_80104088(…,1)` @0x801040B8-C4 |
| **0x20 Hund liegend** | `EMD0G_MOD0.BIN` | **−500** | **500** | `FUN_80104088(…,0)` @0x80104098-A8 |
| 0x21 Krähe | `EMOVL21_S0.BIN` | −350 | 530 | @0x801003B8 / @0x801003C4 / @0x801003C8 / @0x801003DC |
| 0x25 Spinne | `EMS25.BIN` | **0** | **1400** | Block-Kopie aus der Tabelle @0x801063A0 (+20 = `0x03E8FA88` → −1400/1000, +24 = `0x057803E8` → 1000/1400) nach +0x84…+0xA0 @0x80102720-5C, danach `sh zero,152(s0)` @0x8010276C |
| 0x26 Baby | `EMS26.BIN` | −10 | 10 | @0x80100168-78 — der **einzige** Schreiber im ganzen Overlay |

Ladeadressen gegengeprüft (Hund): `lui at,0x8010` @0x80100064 + `lw v0,21560(at)` @0x8010006C
⇒ Zustandstabelle @0x80105438 mit lauter gültigen Overlay-Zeigern.

⛔ **Objekt-Prüfung je Store** (die Falle aus Runde 26): alle oben genannten Stores gehen auf
den **Gegner selbst** — `s0`/`s1`/`s2` = `a0` des jeweiligen Roots bzw. `a2 = a0` in
`FUN_80104088` (`addu a2,a0,zero` @0x8010408C). Zwei Stores, die **nicht** mitgezählt sind, weil
sie auf ein **anderes Objekt** gehen: `sh t0,158(v1)` @0x801044C0 / `sh v0,152(v1)` @0x801044CC
(Hund) und `sh t2,158(v1)` @0x80105CD8 / `sh v0,152(v1)` @0x80105CE8 (0x25) — dort ist `v1` ein
aus `lhu v0,118(a0)` adressiertes Fremd-Objekt, nicht `self`.

Zum Vergleich: **RE1.5 kennt dieses Feldpaar praktisch nicht** — `info/Re1.5/PSX.EXE` hat im
ganzen Binary **2** Stores auf +0x98/+0x9E (@0x80041220, @0x800421EC) gegen Dutzende in RE2.
Das Tor ist eine RE2-Neuerung.

### 3.2 Port: die Felder gibt es nicht

`re15_actor_t` (`re15_port/include/re15_actor.h`) trägt **weder +0x98 noch +0x9E** als
Trefferzonen-Felder. Was es gibt:

| Offset | Port-Feld | Bedeutung | Bemerkung |
|---|---|---|---|
| +0x98 | `status_flags` (`re15_actor.h:92`) | RE1.5-Statusbits (Blut/Gift) | **anderes Feld, gleicher Offset** — kein Kandidat |
| +0x9A | `re2z_rad9a` (`re15_actor.h:299`) | XZ-Halbbreite, nur Zombie-Familie | der direkte **Nachbar** ist bereits modelliert |
| +0x9E | — | — | fehlt |

Der heutige senkrechte Eingang des RE2-Trefferpfads ist `re15_re2_gun_probe`
(`re15_damage.c:1520`): `int32_t dy = e->y - pl->y;` gegen die Waffen-Fenster. Die „Zielhöhe"
ist dort die **Fußhöhe des Spielers**.

---

## 4. Die Messung auf dem echten Weg

`probe_r30_zielfenster`, `re15_game_step` + Pad, echte RDTs, RE2-KI-Geschmack, RE2-Bank
`shared_assets/RE2/CDEMD0.EMS` geladen, Gegner am Leben gehalten, ab Bild 20 wird geschossen.
**5 Typen × 240 Bilder = 1200 Bilder Abdeckung**, dazu der Hunde-Lauf (§5).

| Typ | Raum | eY | pl->y | **Hgun** | Mündung verfügbar |
|---|---|---|---|---|---|
| ZOMBIE 0x10 | ROOM1140 | 0 | 0 | **0** | 0/240 |
| HUND 0x20 | ROOM1190 | −3680…−530 | −3680…−530 | **−20…420** | 0/240 |
| KRÄHE 0x21 | ROOM10C0 | −250…0 | −250…0 | **−250…0** | 0/240 |
| SPINNE 0x25 | ROOM2000 | −5400 | −5400 | **0** | 0/240 |
| BABY 0x26 | ROOM1090 | −1800 | −1800 | **0** | 0/240 |

*(Der Hunde-Streubereich −20…420 ist die Eigenbewegung des springenden Hundes innerhalb eines
Bildes; die Sonde setzt `pl->y = e->y` am Bildanfang.)*

⛔ **Erster Lauf war ein Sondenfehler und ist verworfen:** ohne `pl->y = e->y` blieb der Spieler
auf y=0 stehen, während die Gegner je nach Raum auf −530…−5400 sitzen — gemessen wurde dann der
Teleport-Versatz der Sonde, nicht die Spielgröße (Hgun −5400). Der Fehler steht als Kommentar in
`track()`.

### Die drei Zielhöhen-Kandidaten des Ports

| Kandidat | Hgun | Quelle | Ergebnis |
|---|---|---|---|
| **A** `pl->y` (heutiger Stand) | **0** | `re15_re2_gun_probe`, `re15_damage.c:1520` | Tor lässt **alle sieben** Boxen durch |
| **B** `pl->y − 2083` | 2083 | `game_step_common.c:1762`, Quelle `shots/pose_aim.txt.leon` b13 y=−2083 | Zombie stehend + Hund stehend ja; Kriecher, Hund liegend, Krähe, 0x25, 0x26 **NEIN** |
| **C** `re15_player_gunbone_world` | — | `re15_damage.c:1134` | **0 von 1200 Bildern gültig** — braucht `s_hand_world`/`s_hand_rot` vom PC-Renderer, headless nie gesetzt |

---

## 5. Das Tor durchgerechnet — wer bleibt treffbar

Fenster je Box (`−(b+h+100) ≤ Hgun < h−b+100`) und Urteil je Mündungshöhe:

| Box | b | h | Fenster Hgun | 0 | 400 | 800 | 1000 | 1200 | 1600 | 2000 | 2083 | 2200 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| ZOMBIE 0x10 stehend | −1500 | 1500 | [−100, 3100) | ja | ja | ja | ja | ja | ja | ja | **ja** | ja |
| ZOMBIE 0x10 Kriecher | −350 | 350 | [−100, **800**) | ja | ja | NEIN | NEIN | NEIN | NEIN | NEIN | **NEIN** | NEIN |
| **HUND 0x20 stehend** | −1000 | 1000 | [−100, **2100**) | ja | ja | ja | ja | ja | ja | ja | **ja** | NEIN |
| **HUND 0x20 liegend** | −500 | 500 | [−100, **1100**) | ja | ja | ja | ja | NEIN | NEIN | NEIN | **NEIN** | NEIN |
| KRÄHE 0x21 | −350 | 530 | [−280, **980**) | ja | ja | ja | NEIN | NEIN | NEIN | NEIN | **NEIN** | NEIN |
| SPINNE 0x25 | 0 | 1400 | [−1500, **1500**) | ja | ja | ja | ja | ja | NEIN | NEIN | **NEIN** | NEIN |
| BABY 0x26 | −10 | 10 | [−100, **120**) | ja | NEIN | NEIN | NEIN | NEIN | NEIN | NEIN | **NEIN** | NEIN |

**Die entscheidenden Zahlen:**

1. **Bei Hgun = 0 (= dem heutigen Port-Stand) hält das Tor für NIEMANDEN.** Gemessen 1200/1200
   Bilder „DURCH", für alle sieben Boxen. Ein Einbau gegen `pl->y` ändert **nichts** am
   Nutzer-Befund.
2. **Das Original fällt in ein schmales Fenster.** Damit der stehende Hund treffbar und der
   liegende nicht treffbar ist, muss die Mündung **1100 ≤ Hgun < 2100** über dem Hund liegen.
   Das ist die einzige Aussage, die aus dem Nutzer-Befund und dem Tor gemeinsam folgt — und sie
   passt zur Größenordnung „Pistole in Brusthöhe".
3. **Dauerhaft untreffbar würden, sobald Hgun in jenem Fenster liegt:**
   * **BABY 0x26** — Fenster nur [−100, 120). Ab Hgun ≥ 120 nie wieder treffbar. Das Overlay hat
     **genau einen** Schreiber auf +0x98/+0x9E (@0x80100168-78) — es gibt keinen Zustand, in dem
     der Wert größer würde.
   * **KRÄHE 0x21** — Fenster [−280, 980). Bei Hgun ≥ 980 nie treffbar, solange sie **auf der
     Ebene des Spielers** ist. Gemessen fliegt sie im Port bis 250 Einheiten über dem Boden
     (eY −250…0) — das reicht nicht, das Tor ginge weiterhin zu. Krähen im Original sitzen/fliegen
     höher; das ist im Port nicht gemessen und bleibt offen.
   * **ZOMBIE-KRIECHER** — Fenster [−100, 800). Das ist im Original plausibel gewollt
     (Zielen nach unten), aber der Port hat dafür keinen Mechanismus, der die Mündung senkt.
   * **SPINNE 0x25** — Fenster [−1500, 1500). Bei Hgun 1100…1500 noch treffbar, darüber nicht.
     Also **randständig**: dieselbe Mündungshöhe, die den liegenden Hund ausschließt, kann je
     nach Wert auch die Spinne ausschließen.
   * **ZOMBIE stehend** — [−100, 3100), bleibt in jedem Fall treffbar.

---

## 6. Die Hunde-Kette, mit geladener Bank

`shared_assets/RE2/CDEMD0.EMS`, EM020-Bank: **27 Clips**, Längen gemessen (nicht 0 — die Falle
aus Runde 29 ist ausgeschlossen):

| Clip | Rolle | Adresse | Bilder |
|---|---|---|---|
| 17 | HURT-P0 Treffer-Zucken | Wort `0x00070011` @0x80103460-7C | **13** |
| 18 | HURT-P2, +0x7=0, Hinfallen | `addiu v1,v1,18` @0x801036B0-CC | **9** |
| 7 | HURT-P2, +0x7=1, Aufstehen | Wort `0x000F0007` @0x801036D0-DC | **50** |
| 22 | HURT-P3 weiche Landung | Wort `0x00030F16` @0x8010378C-A0 | 20 |

Gemessener Lauf im Port (`pass_dog_chain`): Treffer bei f0 mit +0x1D3 = 14 (Stempel 15, im
Treffer-Bild schon einmal abgezogen), Kette 17 → 18 → 7, Rückkehr nach ACTIVE bei **+71**.

| | Bilder |
|---|---|
| Trefferpause +0x1D3 endet | **14** |
| HURT-Kette endet (state 2 → 1) | **71** |
| ⇒ treffbar, während er liegt | **57** |

**Wo das Original wieder öffnet** — `FUN_80104088` hat im ganzen Hunde-Overlay genau **vier**
Aufrufer (`jal`-Vollscan von `EMD0G_MOD0.BIN`):

| Aufruf | a1 | Stelle |
|---|---|---|
| @0x80103458 | 0 (`addu a1,zero,zero` @0x80103448) | HURT-P0, im selben Block wie `sh 2,6(s1)` @0x80103450 (Phase 2) |
| @0x8010352C | 0 (`addu a1,zero,zero` @0x80103514) | HURT-P1, direkt nach `sw a2,60(s0)` (Y setzen) und `sb zero,537` |
| **@0x801036F0** | **1** (`addiu a1,zero,1` @0x801036F4) | HURT-P2, Zweig `+0x7 == 2` (`addiu v0,zero,2` @0x80103694 / `beq v1,v0,0x801036f0` @0x80103698) — und **nur**, wenn der Clip-Vorschub `jal 0x8002959c` @0x8010365C ≠ 0 lieferte, also **im Bild nach dem Ende des Aufsteh-Clips 7** |
| **@0x801037C0** | **1** (`addiu a1,zero,1` @0x801037C4) | HURT-P3, nach `jal 0x8002959c` @0x801037B0 ≠ 0, gefolgt von `addiu v0,zero,513` / `sw v0,4(s0)` @0x801037C8-D0 = zurück nach ACTIVE, Sub 2 |

Das ist wörtlich „wieder treffbar, sobald er wieder steht": gestaucht beim Treffer, geöffnet am
**Ende** der Aufsteh-Kette.

`FUN_80104088` im Wortlaut — der a1≠0-Zweig **maskiert die Flags NICHT**, er ODERt nur:
```
80104088: bne  a1,zero,0x801040b8
8010408c: addu a2,a0,zero          ; a2 = SELF (Objekt-Prüfung: alle Stores gehen auf a2)
80104090: lui  a0,0xe7ff
80104094: ori  a0,a0,0xffff        ; Maske 0xE7FFFFFF
80104098: addiu v0,zero,-500
8010409c: sh   v0,152(a2)          ; +0x98 = -500
801040a4: addiu v1,zero,500
801040a8: sh   v1,158(a2)          ; +0x9E =  500
801040ac: lui  v1,0x400            ; |= 0x04000000
801040b4: and  v0,v0,a0            ; &= 0xE7FFFFFF
801040b8: addiu v0,zero,-1000      ; a1 != 0
801040bc: sh   v0,152(a2)          ; +0x98 = -1000
801040c0: addiu v0,zero,1000
801040c4: sh   v0,158(a2)          ; +0x9E =  1000
801040cc: lui  v1,0xc00            ; |= 0x0C000000   (KEINE Maskierung!)
801040d0: or   v0,v0,v1
801040d8: sw   v0,0(a2)
```

---

## 7. Was danach gebaut werden KANN — und was NICHT

**NICHT baubar in dieser Form (und darum in dieser Welle nicht gebaut):**
* Das fünfte Gate allein. Gegen `pl->y` ist es gemessen **wirkungslos** (1200/1200 durch).
* `re2d_hitbox` allein. Es beschriebe Felder, die niemand liest.
* Das Gate mit irgendeiner **geschätzten** Mündungshöhe. 2083 ist ein Pose-Messwert des Ports
  (`shots/pose_aim.txt.leon` b13), keine RE2-Größe; er liegt 17 Einheiten unter der
  Hund-stehend-Schranke 2100. Eine Konstante, die so knapp an einer Kippe sitzt, ist ein
  Rate-Defekt.

**Baubar, in dieser Reihenfolge:**
1. **`+0x98`/`+0x9E` als echte Aktor-Felder** (neben dem schon vorhandenen `re2z_rad9a` = +0x9A),
   je Typ aus den INIT-Werten von §3.1 gesetzt, und `re2d_hitbox` byte-true nach @0x80104090-D8
   (inkl. der Flagbits 0x04000000 / 0x0C000000 und der Maske 0xE7FFFFFF nur im a1==0-Zweig).
   Das ist für sich **verhaltensneutral**, solange niemand die Felder liest — also ein sicherer
   erster Schritt mit eigener Messung.
2. **Die Mündungshöhe**: `t[1]` der verketteten Waffen-Bone-Matrix (@0x80042E60-94). Der Port hat
   den Baustein (`re15_player_gunbone_world`, `re15_damage.c:1134`), aber er hängt am
   PC-Renderer und ist **headless nie gültig** (0/1200). Vor dem Gate muss diese Größe
   **engine-seitig** verfügbar und deterministisch sein — sonst verhält sich der Port unter Test
   anders als im Fenster, und der PSX-Zweig hätte sie gar nicht.
3. **Erst dann** das fünfte Gate @0x8004716C-A4 in `re15_re2_pause_filter_apply` bzw. im
   RE2-Kandidatenpfad, **mit einem Riegel je Typ**, der die Zahlen aus §5 festnagelt: stehender
   Hund durch, liegender Hund gesperrt, Zombie stehend durch — und einer **Gegenprobe**, dass
   0x26 und 0x21 nicht dauerhaft untreffbar werden.

⛔ **Die Runde-13/14-Falle wörtlich:** ein Tor auf ein Feld, das der Port nie füllt, sperrt
dauerhaft. Hier ist es umgekehrt gelagert und genauso wertlos — ein Tor auf eine Zielhöhe, die
immer 0 ist, sperrt **nie**. Beide Male ist das Ergebnis ein Fix, der den Befund nicht erklärt.

---

## 8. Offen (ehrlich benannt)

* **Leons echte Mündungshöhe in RE2 ist nicht gemessen.** Aus dem Nutzer-Befund folgt nur das
  Intervall **[1100, 2100)**. Der nächste Weg dafür: DuckStation-Savestate von RE2 (Leon zielt
  auf einen Hund) und `sp+56` von @0x80042F94 bzw. die Bone-Matrix direkt lesen —
  oder PCSX-Redux-Watchpoint auf den Store.
* **Die Größenordnung des Ports und die von RE2 sind nicht abgeglichen.** RE2 gibt dem Spieler
  −1530/1530 (@0x8005742C-3C, @0x8003BDE0-EC), der Port misst die Ziel-Hand-Bone auf 2083 über
  dem Boden. Eines von beidem passt nicht; das muss geklärt sein, bevor eine RE2-Konstante gegen
  eine Port-Höhe geprüft wird.
* **Kein Bildbeleg.** Die Sitzung läuft in einer REMOTEDESKTOP-Sitzung; gemessen wurde
  ausschließlich über `re15_game_step` + Pad.
* **Die Vorschubrate der HURT-Clips** (+0x15A: 512 in P1 @0x80103518, 256 in P2 @0x801036E0)
  wurde in dieser Runde nicht gegen den Port geprüft; der Port advanciert gemessen 1 Bild je Tick.
  Berührt die Kettenlänge, nicht das Tor.
* **0x25 hat zwei `sh zero,152`-Stellen** (@0x8010049C auf `s2`, @0x8010276C auf `s0`). Welche
  davon im Spiel läuft, ist nicht auseinandergehalten; beide setzen +0x98 auf 0, das Ergebnis der
  Rechnung ändert sich dadurch nicht.
