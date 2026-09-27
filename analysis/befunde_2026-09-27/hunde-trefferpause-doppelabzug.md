# Runde 29: die Trefferpause +0x1D3 des Hundes wurde doppelt abgezogen — und was das am Nutzer-Befund NICHT erklärt

Status: FERTIG (2026-09-27). Ein echter, gemessener Defekt behoben; `local_build.sh all`
353/353 grün. **Die Nutzer-Beobachtung erklärt der Fix nur zum Teil** — §4 sagt, was fehlt,
mit Adressen.

Nutzer-Befund seit Runde 26: *„Im Original Resident Evil 2 sind die Hunde erst dann wieder
verwundbar, sobald sie wieder stehen."*

---

## 1. Der Defekt — gemessen, nicht modelliert

Messschiene `re15_port/tests/unit/probe_r29_1d3_doppelabzug.c`, echter Weg
(`re15_game_step` + Pad, echte RDTs, RE2-KI, RE2-Bank aus `shared_assets/RE2/CDEMD0.EMS`).
`+0x1D3` von außen auf 40 gesetzt, danach nur zielen; Delta je Bild protokolliert.

| Typ | Raum | ALT | NEU |
|---|---|---|---|
| ZOMBIE 0x10 | ROOM1140 | Delta 1 in 40/40, 0 nach 40 Bildern | unverändert |
| **HUND 0x20** | ROOM1190 | **Delta 2 in 8 von 32**, 0 nach **32** Bildern | Delta 1 in 40/40, 0 nach 40 |
| KRÄHE 0x21 | ROOM10C0 | Delta 1 in 40/40 | unverändert |
| BABY 0x26 | ROOM1090 | Delta 1 in 40/40 | unverändert |
| SPINNE 0x25 | ROOM2000 | Delta 1 in 40/40 | unverändert |

Abdeckung: 5 × bis zu 200 Bilder je Durchgang, plus zwei Schuss-Läufe.

Echter Pistolentreffer (Waffe 3 Browning HP):

| Typ | ALT | NEU | SOLL |
|---|---|---|---|
| ZOMBIE 0x10 | 5 Bilder | 5 Bilder | **5** |
| HUND 0x20 | **8 Bilder** | **15 Bilder** | **15** |

⛔ **Die Vermutung „Hund UND Krähe zählen doppelt" ist zur Hälfte widerlegt.** Nur der Hund.
Die Sammelzeile `enemy_ai_common.c:5650` sitzt in `re15_enemy_ai_live_tick`; Krähe, Spinne
und Hund werden von `re15_enemy_ai_run_all` typweise an **eigene** Ticks verteilt, die
`live_tick` gar nicht betreten. Der Hund hatte seine eigene, zweite Zeile in
`re15_dog_ai_tick`.

## 2. Die Belege (selbst nachgelesen, `re2_disasm.py`)

**Genau EIN Dekrement je Bild, je Overlay-Root** (jedes Overlay lädt roh @0x80100000):

| Overlay | Adressen |
|---|---|
| Zombie `EMZ0.BIN` | `lbu v1,467(s0)` @0x80100484 / `andi v0,v1,0x7f` @0x8010048c / `beq` @0x80100490 / `addiu v0,v1,-1` @0x80100494 / `sb v0,467(s0)` @0x80100498 |
| **Hund `EMD0G_MOD0.BIN`** | `lbu v1,467(s0)` @0x80100028 / `andi v0,v1,0x7f` @0x80100030 / `beq` @0x80100034 / `addiu v0,v1,-1` @0x80100038 / `sb v0,467(s0)` @0x8010003c |
| Krähe `EMOVL21_S0.BIN` | `lbu` @0x80100160 / `andi 0x7f` @0x80100168 / `beq` @0x8010016c / `addiu -1` @0x80100170 / `sb` @0x80100174 |
| 0x25 `EMS25.BIN` | `andi 0x7f` @0x801000f4 / `addiu -1` @0x801000fc / `sb` @0x80100100 |
| 0x26 `EMS26.BIN` | `andi 0x7f` @0x80100048 / `addiu -1` @0x80100050 / `sb` @0x80100054 |

Gegenprobe der Hunde-Ladeadresse: `lui at,0x8010` @0x80100064 + `lw v0,21560(at)`
@0x8010006c ⇒ Zustandstabelle @0x80105438 mit lauter gültigen Overlay-Zeigern.

**Der Sollwert je (Typ, Waffe)** — `info/re2leon/PSX.EXE`:
Zeile = `0x800A6A88[entity+0x8] + 0x14*(id-1)`
(`lbu v0,8(s1)` @0x80047218 / `lw a1,27272(at)` @0x8004722c / `sll/addu/sll` @0x80047230-38 /
`addiu v0,v0,-20` @0x8004723c), Stun = `(Zeile[+4] >> 9) & 0x7F`
(`lw v0,4(a1)` @0x80047338 / `srl v0,v0,9` @0x80047340 / `andi v0,v0,0x7f` @0x80047344 /
`or a0,a0,v0` @0x80047348 / `sb a0,467(s1)` @0x8004734c).
* Hund: `0x800A6A88[0x20] = 0x800A4424`, Zeile 3 = 0x800A444C, Wort1 `0x078F1E0A` ⇒ **15**
* Zombie: `0x800A6A88[0x10] = 0x800A412C`, Zeile 3 = 0x800A4154, Wort1 `0x02850A14` ⇒ **5**

**Kandidatenfilter** `lbu v0,467(s0)` @0x80047138 / `bne v0,zero,0x8004740c` @0x80047140
(unmaskiert, Bit 0x80 sperrt mit).

## 3. Der Fix — ein Riegel, der die Dispatch-Weiche NICHT nachbaut

`enemy_ai_common.c`: die Runde-14-Zeile ist weg; an ihrer Stelle merkt sich der RE2-Zweig von
`re15_dog_ai_tick` den Wert **vor** dem Dispatch und ruft nach **jedem** der drei Ausgänge
(Zustand 4/5/6 → `re15_dog_state456`, Käfig-Freigabe 0x0C → `re15_dog_release0c`, sonst
→ `re15_re2dog_tick`) `re15_dog_pause_nachziehen(e, vor)` auf. Diese zieht nur ab, wenn
niemand das Byte angefasst hat.

* **nie zu kurz:** hat der Root abgezogen, ist der Wert anders → hier passiert nichts.
* **nie eingefroren:** hat niemand abgezogen, zieht diese Zeile nach.
  ⛔ Genau das war die Runde-14-Falle: der wartende ROOM1190-Zwingerhund (grid 0x40,
  state 4/0/0) blieb auf +0x1D3 = 14 stehen und war **dauerhaft unverwundbar**.
* Die Latch-Freigabe `+0x93 &= ~1` läuft mit (Resolver `FUN_80011f50`, Maske 0x03000000
  @0x800120c0, Test @0x800120f4-0x80012100) — sonst hinge der Skript-Hund im Resolver fest.

**Riegel** `r29_1d3_doppelabzug` (neuer ctest, `probes/r29_1d3-doppelabzug.cmake`):
je Typ „Delta je Bild == 1" UND „0 wird erreicht"; Gegen-Riegel gegen Überkorrektur
„Pause == Sollwert der Stun-Zeile".
* am **ALTEN** Stand nachgefahren: **ROT** (`Delta je Bild max 2`, `Pause 8 Bilder (SOLL 15)`)
* am neuen Stand: **GRÜN**, 7 von 7 Urteilen.

## 4. ⛔ Erklärt der Fix den Nutzer-Befund? NEIN — nur den ersten Schritt

Dritter Durchgang derselben Sonde, mit **echter** EM020-Bank (ohne Bank hat jeder Clip
Länge 0 und die Kette misst sich selbst kaputt — der erste Lauf tappte genau da hinein):

| Clip | Rolle (Adresse im Hunde-Overlay) | Bilder |
|---|---|---|
| 17 | P0 Treffer-Zucken, Wort 0x00070011 @0x80103460-7C | 13 |
| 18 | P2.0 Hinfallen, `addiu v1,v1,18` @0x801036B0-CC | 9 |
| 7 | P2.1 Aufstehen, Wort 0x000F0007 @0x801036D0-DC | **50** |
| 22 | P3 weiche Landung, Wort 0x00030F16 @0x8010378C-A0 | 20 |

Gemessen auf dem echten Weg: die **HURT-Kette dauert 71 Bilder**, die **korrekte Pause 15**.
Der Hund ist also auch nach dem Fix **57 Bilder lang treffbar, während er noch liegt**.
Der doppelte Abzug war die halbe Antwort — die andere Hälfte fehlt.

### Der Mechanismus, der noch fehlt — mit Adressen

`FUN_800470C0` hat **fünf** Gates, nicht vier. Das fünfte ist ein **senkrechtes Zielfenster**
und wurde bisher als „Trefferprüfung" gelesen (so steht es heute in
`re15_damage.c` ~:1727: *„RE2s EIGENER Kandidatenfilter hat überhaupt kein Höhen-Band"* —
**das ist falsch**, selbst disassembliert):

```
8004716c: lhu v1,464(s0)     ; +0x1D0
80047170: lh  a0,152(s0)     ; +0x98   Unterkante
80047174: lw  v0,60(s0)      ; +0x3C   y
8004717c: addu v0,v0,a0
80047180: addiu v0,v0,100
80047188: lhu v1,158(s0)     ; +0x9E   Halbhoehe
8004718c: lw  a0,4(s4)       ; Schuss-/Zielhoehe
80047190: addu v0,v0,v1
80047194: subu v0,v0,a0
80047198: addiu v1,v1,100
8004719c: sll v1,v1,1
800471a0: sltu v0,v0,v1
800471a4: beq v0,zero,0x8004740c    ; <- Schleifen-Fortschaltung, wie die vier anderen Gates
```
(`0x8004740c: addiu s2,s2,4` / `bne s2,v0,0x8004711c` @0x80047418 = die Kandidatenschleife.)

Und **genau diese beiden Felder halbiert der Hund, solange er liegt** —
`FUN_80104088(self, a1)` im Hunde-Overlay:
```
80104098: addiu v0,zero,-500     ; a1 == 0  (LIEGEND)
8010409c: sh v0,152(a2)          ; +0x98 = -500
801040a4: addiu v1,zero,500
801040a8: sh v1,158(a2)          ; +0x9E =  500
801040b4: and v0,v0,a0           ; flags &= 0xE7FFFFFF  (a0 = 0xE7FFFFFF @0x80104090-94)
801040ac: lui v1,0x400           ; flags |= 0x04000000
801040b8: addiu v0,zero,-1000    ; a1 != 0  (STEHEND)
801040bc: sh v0,152(a2)          ; +0x98 = -1000
801040c0: addiu v0,zero,1000
801040c4: sh v0,158(a2)          ; +0x9E =  1000
801040cc: lui v1,0xc00           ; flags |= 0x0C000000
801040d0: or v0,v0,v1  /  801040d8: sw v0,0(a2)
```
Gerufen wird es mit **0 beim Treffer** (`jal 0x80104088(0)` @0x80103458 in Hurt-P0,
@0x8010352C in P1) und mit **1 erst am ENDE der Aufsteh-Kette** (@0x801036F0 in P2,
@0x801037C0 in P3) — also exakt „wieder treffbar, sobald er wieder steht".
Das Zielfenster ist liegend nur noch halb so hoch (±500 statt ±1000 plus die 100er-Reserve),
der liegende Hund fällt bei waagerechtem Ziel aus der Kandidatenliste.

**Im Port ist das ein dokumentierter NOP:** `enemy_ai_re2_dog.c:664`
`static void re2d_hitbox(re15_actor_t *e, int restore) { (void)e; (void)restore; }`,
und der portierte Kandidatenfilter hat das fünfte Gate gar nicht.

**Nächster Weg** (nicht mehr in dieser Runde gebaut — er berührt den Filter aller
RE2-Typen und die Zielhöhe des Spielers und braucht eine eigene Messrunde):
1. `+0x98`/`+0x9E` als echte Felder führen und `re2d_hitbox` byte-true machen
   (@0x80104090-D8), inklusive der Flags-Bits 0x04000000/0x08000000.
2. Das fünfte Gate @0x8004716C-A4 in `re15_re2_pause_filter_apply` nachziehen und die
   Zielhöhe `lw a0,4(s4)` im Port benennen.
3. Vorher messen, welche Werte `+0x98/+0x9E` bei ALLEN RE2-Typen im Port tragen — ein
   Gate auf ein nie gesetztes Feld sperrt sonst wieder dauerhaft (Runde-14-Falle).
