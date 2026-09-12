# BEFUND — ROOM1030 (Lobby): „Zombies fliegen ganz komisch nach dem Aufstehen auf die Beine"

Datum 2026-09-12, Runde 5. Marker: `befund_1030_F1140_marke1.bmp` (F1140, Kamera C9).
Nur statische Analyse: Port-Quellen, ROOM1030.RDT-Bytes, RE1.5-Overlay-Decompile,
befund.log (Marken-Block F1140). Kein Build, keine Quelldatei geändert.

---

## 1. Kernbefund (ein Satz)

Die Torkriecher der Lobby werden vom Skript-Rückweg (`Member16 = (m & 0x0FFF) | 0x2000`,
ROOM1030 sub05 @Datei 0x24B2) über die D15.3-Brücke in **EXEC[9] = den STOSS-/TAUMEL-Executor**
geschickt (`re15_ai_set_state_word(e, 0x901)`, enemy_ai_re2_zombie.c:7727) — der ist
**byte-belegt KEIN Aufstehen** (Port-eigene Disasm-Herleitung im D15.2-Block, :7285-7305):
er poppt den Liegenden hart in einen AUFRECHT startenden Clip (Tabelle {3,3,4,0x0D}
@0x801000D8), schiebt ihn mit **+0x144 = 400** (@0x80103F60-64) rückwärts und wirft ihn am
Schub-Ende mit **7/8 Wahrscheinlichkeit in den STURZ** `0x501` = EXEC[5]
(`s=(r1>>(r2&3))&7; s!=0 -> +0x4=0x501` @0x80104000-28). Der Sturz-Clip 1/2 startet
mitten im Flug (Startframe `side*5+10` @0x80103310-1C) — das ist der „verdreht in der
Luft"-Zombie; die Liegenden daneben sind dieselben Kriecher in den Boden-Phasen der
Sturzkette.

## 2. Messbeleg — der Marken-Block F1140 (befund.log:55972-55990)

```
 1  typ=0x16 st=1 ss= 5/1  clip=2   bild=53  hp=128  ... spd=0  grid=80 anim=2000
 2  typ=0x16 st=1 ss= 0/1  clip=5   bild=27  hp=82   ...        grid=00 anim=1004
 3  typ=0x16 st=1 ss= 5/7  clip=8   bild=40  hp=75   dist=1173  grid=80 anim=2000
 4  typ=0x16 st=1 ss= 5/3  clip=23  bild=9   hp=66   ...        grid=80 anim=2000
 5  typ=0x16 st=7 ss= 7/0  clip=23  bild=0   hp=-1   (Leiche)
 6  typ=0x16 st=1 ss= 5/7  clip=8   bild=13  hp=50   dist=1907  grid=80 anim=2000
```
- `anim=2000` = `e->anim_flags` == +0x1C4 == Member16 (actor_common.c:101/128/157; Logger
  main.c:5131-5139) → der Skript-RÜCKWEG lief (Bit 0x1000 weg, 0x2000 gesetzt — exakt die
  sub05-Form `AND 0x0FFF, OR 0x2000`, scd_vm.c:3889-3891; im RDT selbst gibt es KEINEN
  weiteren 0x2000-Schreiber: Byte-Scan `35 20 04`/`3d 04 20` = 0 Treffer, `35 10 04` nur
  @0x24C2/0x275E/0x2814).
- **Vier lebende Zombies gleichzeitig in ss=5/x = EXEC[5] (Sturzkette)** — Slot 1 mit
  **hp=128 (VOLL, nie beschossen)** in Phase 1 = der Fall-Clip 2 spielt = MITTEN IN DER LUFT.
  Slots 3/6 in P7 (Boden-Aufsteh-Clip 8), Slot 4 in P3 (Boden-Zucken, Prone-Clip 23 =
  `re2z_param_clips[2+side]` aus P2, :2090). `grid=80` = das Downed-Band, das EXEC[9]s
  Sturz-Commit setzt (:2712-2720). Ohne einen einzigen Schuss ist EXEC[5] von einem lebenden
  Zombie NUR über EXEC[9] erreichbar (Flinch-Weg braucht Treffer, s. HURT @0x801050A4).

## 3. Die Kette Ende-zu-Ende (Port, mit Zeilen)

1. **HINWEG**: ROOM1030 sub07 @0x2754 `Member16 |= 0x1000` → Brücke
   enemy_ai_re2_zombie.c:7728-7734 → `re15_re2z_enter_crawler(e,NULL,0)` (:2863-2874,
   `+0x10E=(…&~0x3F)|1` @0x80104590-98) → Zombies kriechen unter dem Rolltor durch (korrekt).
2. **RÜCKWEG**: sub05 @0x24B2 setzt `anim_flags = 0x2000` → Brücke :7724-7727:
   `f10e &= ~1; sca_mask=4; set_state_word(0x901)`.
3. **EXEC[9]** `re2z_exec_getup` (:2606-2718): P0 committet Clip `{3,3,4,0x0D}[row*2+dir]`
   mit frac 0 (HARTER Schnitt von Bauchlage auf Stehend), `speed_h=400`; P1 schiebt
   (`re2z_thrust_yaw`, Abbau −30/Frame @0x80103FC4-CC); im Auslauf-Frame 7/8 → `0x501`,
   Fall-Clip 1/2 ab Frame 10/15, `grid|=0x80`.
4. **EXEC[5]** (:1984-2196): Flugphase P1 → Aufschlag P2 (Prone-Clip 22/23 + Liegezeit) →
   Zucken P3-P5 → P6/P7 Boden-Aufsteher 8/9 → P8 `0x101`. Sichtbar: aufstehen → rückwärts
   durch die Luft fliegen → liegen → nochmal aufstehen. Bei 4-6 Zombies gleichzeitig am
   selben Torpunkt = der Stapel des Markers.

Hypothesen (a) Pose-Bank-Wechsel/root_y_fix, (b) Clip-Index-Kollision, (c) anim_frame auf
falscher Bank: **alle drei entlastet für 1030** — dort gibt es keinen sel-0x0E-Sitzer
(alle 20 Records `type=0x16, behavior=0x0D`, Diagnose re2_ki_batch_2026-09-05/
diag_zombie_lying_spawn.md §1), `re2z_re15_pose` bleibt 0; alle Marker-Clips (2/8/23/5)
sind in-Bank-RE2-Clips; der Sitz-Übergabe-Tick ist seit exec_only-Fix clip-sauber
(enemy_ai_common.c:5434-5455). Bekannt-offen, aber hier nicht Täter: die Bank-Asymmetrie
`re15_enemy_hold_last_frame` vs. `re15_actor_clip_len` (enemy_ai_common.c:1999-2006).

## 4. Was die Originale tun

- **RE1.5 (der Anforderer der Sequenz)**: der Rückweg `+0x1C4 & 0x2000` @0x8010369C-A4
  (Gate `!(+0x1D8 & 0x80)` @0x801036B0-BC) → Wort 0x601 → Toggle **FUN_80104f80**
  (RE_15_Quellcode_Overlays/STAGE1_full/FUN_80104f80.c): Phase 0 lädt **Clip 0x12**,
  Startframe rand&3, +0x8F=0xF; Richtung `+0x9F = (+0x9 & 0x80) ? 0 : 1` — der Kriecher
  (+0x9=0x81) spielt den Clip **VORWÄRTS** (f314 a2=0); Phase 2 committet `+0x4=0x201`
  ENGAGE, `+0x1D7=4`. **Ein glatter Ein-Clip-Aufsteher aus der Bauchlage. Kein Schub,
  kein Sturz, kein RNG.**
- **RE2 (der Ausführer)**: hat **keinen Kriech→Stand-Übergang** — alle 36 Zugriffe auf
  +0x1C4/+0x1C6 im Zombie-Overlay sind das Steer-Ziel-Paar, die acht `andi 0x1000` lesen
  +0x21A (Byte-Scan im Brücken-Kommentar :7704-7712). Der RE2-Aufsteher „vom Boden in den
  Stand" ist die **EXEC[7]-P2-Kette: Boden-Aufstehclip `re2z_param_clips[4+back]` = 8/9**
  (@0x80103840-80), P3 spielt aus, P4 committet 0x101 + `1D3&=0x7F` + grid-Clear
  (@0x80103900-18) — deren glatter Verlauf ist gemessen (D15.2-Block :7266-7268:
  „f44 Clear → f45 P2/P3 Clip 8 → f112 P4 → f113 0x101"). Der 10D0-Situp
  (probe_10d0_situp_re2.c) ist der andere, hier NICHT beteiligte Weg: RE1.5-Pose-Bank
  0x2A→0x29 vorwärts (@0x80104aa8, a2=0 @0x80104af0) → 0x101 mit Same-Tick-exec_only.
- Präzedenzfall im Port selbst: der D15.2-Wecker benutzte bis 2026-08-21 GENAU DASSELBE
  falsche Ziel 0x901 und wurde nach Messung („128 von 128 Aufsteher in EINEM Frame")
  auf den Latch-Clear/EXEC[7]-Weg umgebaut (:7266-7305). Der D15.3-Rückweg blieb dabei
  ungefixt stehen — sein Kommentar (:7718-7720 „dasselbe Ziel wie der D15.2-Wecker")
  beschreibt seither einen Zustand, den es nicht mehr gibt.

## 5. Patch-Plan (konkret)

**enemy_ai_re2_zombie.c:7727** — das Brücken-Ziel des Rückwegs von EXEC[9] auf die
EXEC[7]-Boden-Aufstehkette umstellen (Direkteinstieg in P2, Präzedenz: der Kriecher-Abwurf
betritt EXEC[5] direkt in P2 via `0x00020501` @0x801045D4):

```c
if ((e->anim_flags & 0x2000u) && (e->re2z_f10e & 1u)) {
    e->re2z_f10e &= (uint16_t)~1u;                 /* Kriecher-Bit aus (unverändert) */
    e->sca_mask  = 4;                              /* +0x1D7 = 4 @0x801050B4 (unverändert) */
    re15_ai_set_state_word(e, 0x20701);            /* EXEC[7] DIREKT in P2: Boden-Aufsteher
                                                    * Clip 8/9 @0x80103840-80 -> P3 -> P4
                                                    * 0x101 @0x80103900-0C — statt EXEC[9]
                                                    * (Stoss+7/8-Sturz @0x80104000-28) */
}
```
- Pose-kontinuierlich: der Kriecher liegt (Clip 23-Familie), P2 blendet mit 0xF/0x100
  (Produkt 0xF00 < 0x1000, keine Klemme) in den 67-Frame-Aufsteher; P4 räumt selbst auf
  (`0x101`, `1D3&=0x7F`, `grid_id=0`).
- Kommentar :7718-7720 („ZURÜCK … sw 0x901 … dasselbe Ziel wie D15.2") mit korrigieren —
  er zitiert den vor 2026-08-21 zurückgebauten D15.2-Stand.
- Optional, falls Folge-Runde es verlangt: das RE1.5-Rückweg-Gate `!(+0x1D8 & 0x80)`
  @0x801036B0-BC hat im Port keinen Zwilling (Brücke feuert auch mitten im Griff-Sub);
  erst messen, ob ein 1030-Kriecher je im Griff steckt, dann ggf. `sub_state_1 != 1` gaten.

## 6. Messschiene (EINE env-gegatete Zeile)

**enemy_ai_re2_zombie.c:7727** (in den 0x2000-Zweig, vor dem Commit), unter dem
existierenden `RE15_RE2_TRACE`-Kanal (Datei-Log `re2_ki.log` via `re15_re2_trace_out()`,
:89/:1223-1224 — GUI-exe hat kein stderr):

```c
if (getenv("RE15_RE2_TRACE"))
    fprintf(re15_re2_trace_out() ? re15_re2_trace_out() : stderr,
            "[z-stand1030] slot=%d ss=%d/%d clip=%d f10e=%04x 21a=%04x grid=%02x\n",
            (int)(e - g_actors), e->sub_state_1, e->sub_state_2,
            (int)e->motion, e->re2z_f10e, e->re2z_flags21a, e->grid_id);
```
Bestätigung im Spiel: VOR dem Fix folgt auf jede `[z-stand1030]`-Zeile in den vorhandenen
`[re2z]`-Zustandsspuren/nächsten F9-Marke `ss=9/x` und dann `ss=5/x` mit Fall-Clip 1/2
bei vollem HP; NACH dem Fix `ss=7/2 → 7/3 → 7/4 → ss=1` ohne jedes 5/x und ohne Clip 1/2.

## 7. Nebenbefund (dokumentiert, kein Fix hier)

Slot 2 der Marke (`ss=0/1 clip=5 anim=1004 grid=00`, dist 6014) trägt das HINWEG-Bit noch
(sub05 lief für ihn nicht oder später); er ist weit weg und nicht Teil des Symptoms. Ob
`clip=5` im Kriech-Sub-0 korrekt ist, wäre ein eigener Messlauf — hier nicht Täter.
