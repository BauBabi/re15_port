# ROOM1010 — kriechende Zombies: Rückversatz bei Treffer + fehlendes Totstellen (RE2-Flavor)

**Stand:** 2026-09-19 · **Phase:** Messung + RE, **KEINE Engine-Änderung** (nur Sonde + Dossier)
**Nutzer:** „ROOM 1010 – schieße ich die kriechenden Zombies einmal an, werden sie wieder ein Stück
zurückgesetzt (befund_1010_F233_marke10). … kriechen die im Original nicht direkt los, sondern
stellen sich erst einmal tot! … So eine Totstell-Aktion gibt es in RE2 auf jeden Fall auch!"
**Flavor:** RE2 (Nutzer-Default). Sonde: `re15_port/tests/unit/probe_r16_kriecher1010.c`
(registriert in `re15_port/tests/unit/probes/r16_kriecher-1010.cmake`, kein add_test).

---

## 0. Kurzfassung

1. **Rückversatz REPRODUZIERT und erklärt:** nach dem Kriecher-HURT (RE2, Clip 6) betritt der Port die Kriech-Lokomotion P0 mit zufälligem Startbild `rand&0xF` (=7), ohne die Wurzel-Momentaufnahme neu zu verankern; `re15_re2z_move_root` rechnet `dx = sx(Bild 7) − sx(Bild 34 vor dem Treffer) = −43 − 787 = −830` und versetzt den Kriecher **830 Einheiten in EINEM Bild** zurück (x 1598 → 768, Sonde Teil B, f38).
2. **Original RE2:** P0 der Lokomotion ruft `FUN_80015E7C` **zweimal bar** (@0x80103094 bei Bild 0, @0x801030B8 bei Bild `rand&0xF`) — das verankert die Momentaufnahme +0x20C auf das neue Bild; der P1-Aufruf @0x8010312C liefert dann Delta 0, `FUN_800152C8` @0x8010314C bewegt nichts. Kein Sprung.
3. **Totstellen fehlt im RE2-Flavor:** der Port setzt beide 0x81-Spawns per `re15_re2z_enter_crawler(e, NULL, 0)` sofort in die Lokomotion (Sonde Teil A: 1631 Einheiten in 150 Bildern ab Bild 1). Das RE2-Original setzt Deskriptor 1 auf **0x201 = Kriecher-WAIT** (`sw 513` @0x80100B74: liegt in Clip 23, EXEC[2] @0x80103B48) und weckt ihn NUR über DECIDE[2] @0x80103A70 (arc 512, dist < 0x514, LOS, gleiche Etage → 0x101 GRIFF) oder über den Treffer (HURT-P2 @0x80107A54-58 → Lokomotion). RE1.5-Original: Sub 5 wartet bis dist < 0xBB8 (@0x80104718), im RE1.5-Flavor des Ports bereits korrekt (0 Einheiten in 150 Bildern).
4. **Fix (Phase 2):** (a) in `re2z_crawl_exec_move` P0 die zwei baren e7c-Aufrufe nachbilden (Momentaufnahme neu verankern + `+0x144 = sx(r) − sx(0)`), (b) in `re2z_init` sel 1 → `enter_crawler(sub 2)` (WAIT), sel 3 → `sub 0` (Lokomotion), wie @0x80100B60-8C.
5. **Nebenbefund (separat, RE1.5-Flavor):** der RE1.5-Kriecher (Typ 0x10, Clip 0x1A) springt in ROOM1010 um Tausende Einheiten pro Bild (x −1982 → −7638 → −1982 …) und kriecht vom Spieler WEG — eigene Fehlerklasse (Hand-Lock/Wandklemme), hier nur gemessen, nicht RE't.

---

## 1. Reproduktion / Messung

**Sonde:** `probe_r16_kriecher1010.exe` (Build `re15_port/build_r16_kriecher1010`, eigener Ordner).
Echte `ROOM1010.RDT`, `scd_room_reenter(cut 4)` (= Südtür aus ROOM1020, spawnt die zwei
`Sce_em_set type=0x10 beh=0x81` @Datei 0x0A12/0x0A26, s. `analysis/nutzer_batch_2026-08-26/room1010-kriecher.md`),
`scd_vm_tick` + `re15_game_step`, RE2-Bank EM010 aus `shared_assets/RE2/CDEMD0.EMS`, Pistole
(Waffe 3) über Pad R1/SQUARE. Ausgabe im Scratchpad `probe_r16_kriecher1010*.txt`.

### Teil A — Totstellen: Spieler am Tür-Zielpunkt (3650,−3950), 150 Bilder

```
RE2   spawn slot3 type=0x10 grid=0x81 pos=(950,-1700) ry=0  st=0/0/0 mo=0x17 10E=0x0000
      f0   x=950  z=-1700  st=1/0/0 mo=0x17 fr=0   10E=0x0001 sca=8 dist=3510
      f1   x=950  z=-1700  st=1/0/1 mo=0x05 fr=15  rp_kf=15 rp_x=29           <- Lokomotion ab Bild 1
      f2   x=989           dx=39
      f30  x=1626 z=-1882  dx=676
      f149 x=2581 z=-2924  dx=1631 dz=-1224 dist=1492   (2. Kriecher: 2011 Einheiten)
RE15  spawn slot3 grid=0x81 mo=0x0C
      f0..f149  x=950 z=-1700  st=1/5/1 mo=0x0C fr=0  sca=4  dist=3510  -> 0 Einheiten (wartet)
```
→ **RE2-Flavor: kein Totstellen, sofortiges Kriechen.** RE1.5-Flavor wartet (Sub 5, Weck-Tor
dist < 0xBB8 = 3000 nicht erreicht) — das entspricht dem RE1.5-Original (Dossier 08-26 §2d).

### Teil B — Rückversatz: Kriecher kriecht auf den stehenden Spieler (2450,−1700) zu, EIN Pistolentreffer

Isolation: zweiter Kriecher nach (30000,30000) geparkt; Spieler-Riegel `+0x1D3 & 0x80` gesetzt,
damit der Kriecher nicht greift (DECIDE[0] kehrt @0x80102F4C-58 um) — der Treffer-Pfad bleibt
unverändert. Gerader Schuss traf den Kriecher NICHT (90 Bilder, 0 Treffer); mit D-Pad DOWN
Treffer im ersten Bild. Auszug (Spalten: x, dx = x − x(vorheriges Bild), st=+4/+5/+6, mo=Clip,
fr=Bild, rp_mo/rp_kf/rp_x = Port-Momentaufnahme `root_prev_motion/kf/x`, r144 = +0x144):

```
HIT! f21  x=1598 dx=0    st=2/3/1 mo=0x06 fr=4   rp_mo=5 rp_kf=34 rp_x=787 r144=5  hp 95->79
     f22..f36  x=1598 dx=0  st=2/3/1 mo=0x06 fr=5..19  rp_* UNVERAENDERT (HURT ruft move_root nie)
     f37  x=1598 dx=0    st=1/0/0 mo=0x06 fr=19  rp_kf=34 rp_x=787         <- HURT-P2: Wort 1
     f38  x= 768 dx=-830 st=1/0/1 mo=0x05 fr=7   rp_kf=7  rp_x=-43 r144=-830   <<< DER SPRUNG
     f39  x= 762 dx=-6   fr=8  rp_x=-49 r144=-6
     f40  x= 760 dx=-2   fr=9  rp_x=-51
     f41..f65  dx = 0,5,10,15,21,29,39,46,51,56,59,61,62,60,58,54,50,43,35,25,19,15,12,8,5  (= Tabelle C)
     f65  x=1598                          -> erst 27 Bilder spaeter wieder am Trefferpunkt
     Netto nach 90 Bildern: dx=+1
```
`dx(f38) = sx(Bild 7) − rp_x = −43 − 787 = −830` — exakt der gemessene Sprung.

### Teil C — Wurzel-Tabelle (RE2 EM010, Haupt-Bank, `re15_emd_get_keyframe_speed`)

Clip 5 (Kriechen, 50 Bilder), sx je Bild: `0 −1 −6 −12 −20 −28 −36 −43 −49 −51 −51 −46 −36 −21 0 29 68
114 165 221 280 341 403 463 521 575 625 668 703 728 747 762 774 782 787 790 790 788 785 781 775 768
762 755 748 742 736 732 729 728`; sz = 0 überall. Clip 6 (HURT, 20 Bilder) und Clip 23 (Liegen,
10 Bilder): sx = sy = sz = 0.
→ Jeder Wiedereintritt bei Bild `r ∈ 0..15` (sx ≤ 29) nach einem Treffer in der zweiten
Zyklushälfte (sx ≈ 700–790) ergibt im Port einen Rückversatz von 700–840 Einheiten; die Sonde
zeigt den Fall r=7 / alt=34.

### Nebenbefund RE1.5-Flavor (Teil B, RE15, nicht der Nutzer-Fall)

```
f0   x=950   st=1/5/0 mo=0x0C            (Spieler bei dist 1498 < 3000 -> Wecker feuert)
f25  x=-1182 z=-2083 st=1/0/1 mo=0x1A fr=25 sca=8   (kriecht nach -X, vom Spieler WEG)
f75  x=-7638 z=-3521   f100 x=-1982   f150 x=-5136   f175 x=-8552   f199 x=-4818
f214 dx=+2836 in einem Bild; f218 dz=+548; f226 dz=-518; sonst dx=dz=0
```
Der RE1.5-Kriecher (Typ 0x10 / EM010-Bank, Hand-Lock `re15_enemy_handlock_step`) teleportiert
um Tausende Einheiten und bewegt sich zwischen den Sprüngen gar nicht. In ROOM1030 (Typ 0x16)
war dieselbe Kette gemessen in Ordnung (`probe_1030_crawl_live`). **Eigene Fehlerklasse, hier
nicht RE't** (§5).

---

## 2. Original-Mechanismus (Adressen + Instruktionen)

### 2.1 RE2 EMZ0.BIN — Kriech-Lokomotion EXEC[0] `FUN_80103024` P0/P1 (selbst disassembliert)

```
80103064: lui v0,0xf / ori v0,v0,0x5 / sw v0,332(s0)   ; +0x14C = 0x000F0005 = Clip 5, Bild 0, Rate 15
80103070: addiu v0,zero,1 ; jal RNG(0x80015fe8) ; sb v0,6(s0)   ; +0x6 = 1
8010308c: andi v0,v0,0x7 / addiu v0,v0,7
80103094: jal 0x80015e7c            ; ERSTER barer e7c — +0x14D ist 0 (aus dem sw oben)
80103098: sh v0,344(s0)             ;   Delay-Slot: +0x158 = (rand&7)+7
8010309c: jal RNG
801030b4: andi v0,v0,0xf
801030b8: jal 0x80015e7c            ; ZWEITER barer e7c — sieht +0x14D = rand&0xF, weil …
801030bc: sb v0,333(s0)             ;   … der Delay-Slot VOR dem Sprungziel ausgefuehrt wird
;; kein Sprung: P0 faellt in P1
801030c0: lh v0,324(s0) / slti v0,v0,21 / bne -> 0x80103124   ; +0x144 < 21 -> Steuerblock ueberspringen
801030d4-801031 1c: FUN_80015558(+0x1C4/+0x1C6, 24), +0x158-Countdown (alter Wert getestet)
8010312c: jal 0x80015e7c            ; DRITTER e7c (a3 = 0)
80103140: jal 0x8002959c (a3=256)   ; Bild-Vorschub, Rueckgabe NICHT gelesen
8010314c: jal 0x800152c8 (a1=0)     ; Delta +0x144/+0x148 yaw-rotiert auf +0x38/+0x40 anwenden
```

### 2.2 RE2 PSX.EXE — `FUN_80015E7C` (Wurzel-Delta gegen Momentaufnahme, selbst disassembliert)

```
80015f04: lbu v0,333(t0)            ; +0x14D Bild
80015f0c: bne v0,zero,0x80015fa8    ; Bild != 0 -> direkt zur Differenz
80015f14: sw zero,524(t0)           ; Bild 0: Momentaufnahme +0x20C/+0x20E := 0
80015f1c: sh zero,528(t0)           ;         +0x210 := 0
80015fb4: lhu v1,524(t0)            ; alte Momentaufnahme X
80015fbc: lhu a1,528(t0)            ; alte Momentaufnahme Z
80015fc4: sw v0,524(t0)             ; Momentaufnahme := Wurzel(kf_jetzt) X/Y
80015fc8: sh t3,528(t0)             ;                                     Z
80015fcc: subu v1,t1,v1             ; dx = Wurzel(kf_jetzt) - Momentaufnahme  (BEDINGUNGSLOS)
80015fd4: subu a1,t3,a1             ; dz
80015fd8: sh v1,324(t0)             ; +0x144 = dx
80015fe4: sh a1,328(t0)             ; +0x148 = dz
```
Durchgerechnet für den Wiedereintritt nach dem Treffer (Bank EM010, Clip 5, r = 7):
1. e7c @0x80103094, Bild 0 → Momentaufnahme := 0, dann := sx(0) = 0; `+0x144 = 0`.
2. e7c @0x801030B8, Bild 7 → `+0x144 = sx(7) − sx(0) = −43`; Momentaufnahme := −43.
3. P1: `lh +0x144` = −43 < 21 → Steuerblock übersprungen.
4. e7c @0x8010312C, Bild 7 → `+0x144 = −43 − (−43) = 0`; 959c → Bild 8; 152c8 wendet (0,0) an.
5. nächster Tick: `sx(8) − sx(7) = −6` usw.
→ **Im Original gibt es keinen Sprung; die Momentaufnahme wird in P0 durch die beiden baren
e7c-Aufrufe neu verankert.** (`FUN_800152C8` @0x80015310-34: `lh 16(sp)`/`lh 20(sp)` → `+0x38 +=`,
`+0x40 +=` — sie addiert nur, was e7c abgelegt hat.)

### 2.3 RE2 EMZ0.BIN — Kriecher-HURT `FUN_80107888` (selbst disassembliert, jal-Zensus)

Aufrufe im ganzen Handler: `0x80015fe8` (RNG @0x801078F4), `0x8001bf10` (Blut @0x80107938),
`0x8005bd6c` (SE @0x80107950), `0x80106128/0x80106310/0x80106510` (Gore-Zeilen), `0x8002959c`
(Bild-Vorschub @0x80107A38, a3 = 1024). **Kein `0x80015e7c`, kein `0x800152c8`** — während des
HURT bewegt sich der Kriecher nicht und die Momentaufnahme bleibt stehen (wie im Port).
P2 @0x80107A54 `sh v0,270(s0)` (+0x10E = 1) / @0x80107A58 `sw a0,4(s0)` (+0x4 = 1) → Zustand 1 /
Sub 0 / Kriecher = Lokomotion P0 (§2.1).

### 2.4 RE2 EMZ0.BIN — INIT, Kriecher-Zweig (Totstellen) — selbst disassembliert

```
801009b8: addiu a1,zero,1            ; a1 = 1 (einziger a1-Schreiber bis 0x80100B90)
80100ae0: lhu v0,270(s2) / andi v0,v0,0x1 / beq v0,zero,0x80100b60   ; +0x10E & 1 = Kriecher?
80100af8: addiu v0,zero,23 / sw v0,332(s2)   ; +0x14C = Clip 23 (Liegen)
80100b00-20: sh 200,154 / sh 200,156 / sh 200,144 / sh 200,146 / sh -350,152 / sh 350,158  ; Trefferbox flach
80100b24-34: lhu v0,270 / ori v0,v0,0x2000 / sh v0,270            ; +0x10E |= 0x2000
80100b28-44: lw a0,0 / and a0,a0,0xF3FFFFFF / or a0,a0,0x04000000 / sw a0,0   ; word0-Flags
80100b3c-5c: lhu v1,342 / sll,sra 16 / srl 31 / addu / sra 1 / sh v0,342     ; HP := HP/2 (gerundet)
80100b60: lhu v0,270(s2) / andi v0,v0,0x3f
80100b6c: bne v0,a1,0x80100b78        ; (+0x10E & 0x3F) == 1 ?
80100b70: addiu v0,zero,513           ;   Delay-Slot
80100b74: sw v0,4(s2)                 ;   -> +0x4 = 0x201 = Zustand 1 / Sub 2 = KRIECHER-WAIT
80100b78-84: lhu v0,270 / addiu v1,zero,3 / andi 0x3f / bne v0,v1,0x80100b90
80100b8c: sw a1,4(s2)                 ; (+0x10E & 0x3F) == 3 -> +0x4 = 1 = Lokomotion sofort
```
**RE2-Deskriptor 1 = liegender Kriecher, der sich totstellt; Deskriptor 3 = kriecht sofort.**

Das Warten selbst — EXEC[2] `@0x80103B48` (9 Instruktionen): `lbu +0x6; bne -> raus; sb 1,+0x6;
addiu v0,zero,23; sw v0,332` → Clip 23, Bild 0, Rate 0, **kein Ausgang**.
Der Wecker — DECIDE[2] `@0x80103A70`:
```
80103ab4: jal 0x80015614(PL.x,PL.z,a3=512) / sll v0,v0,16 / bne v0,zero,raus   ; Spieler im 512er-Sektor?
80103ac4: sltiu v0,s1,0x514 / beq zero,raus       ; +0x1F0 (Dist) < 1300
80103ad4: lbu 0x800cfdcb / andi 0x80 / bne raus   ; Spieler+0x1D3 Bit 0x80 (Ein-Angreifer-Riegel) frei
80103ae8: lhu 340(s0) / andi 0x800 / beq raus     ; +0x154 & 0x800 = Sichtlinie
80103afc: lbu 262(s0) / lbu 0x800cfcfe / bne raus ; gleiche Etage
80103b10: addiu v0,zero,257 / sw v0,4(s0)         ; +0x4 = 0x101 = GRIFF (Kriecher-EXEC[1] = 0x801025EC)
80103b18-2c: 0x800cfdcb |= 0x80                   ; Riegel setzen
```
Zweiter Ausgang aus dem Warten: der Treffer (§2.3, HURT-P2 → Lokomotion). Es gibt keinen
Distanz-Wecker in die Lokomotion — der RE2-Totsteller steht nur zum Griff oder nach Beschuss auf.

### 2.5 RE1.5 STAGE1.BIN — zur Einordnung (aus Dossier 08-26, hier nur die Anker)

INIT `sel∈{1,3}` unter `+0x9&0x80`: `+0x94 = 0x0C` @0x80100D54-58, `+0x5 = 5` @0x80100D64-68.
DECIDE[5] `FUN_8010466C`: wirksam `dist(+0x1D0) < 0xBB8` @0x80104718 + `arc 0x200` @0x8010472C → `+0x4 = 1` @0x80104748 = Lokomotion.
HURT-Router @0x80105AA4 `andi 0x80` → FUN_801068a0/FUN_80106a38; deren Phase 2 (selbst gelesen):
```
801069e0 / 80106b50: sw a1(=1),4(a0)             ; Wort 1
801069f8 / 80106b68: andi v0,v0,0x1f
801069fc / 80106b6c: bne v0,v1(=3),weiter        ; nur sel == 3 …
80106a04 / 80106b74: sb 5,5(a0)                  ; … zurueck in Sub 5 (weiter liegen)
80106a1c / 80106b8c: andi 0xfe / sb 147          ; +0x93 &= ~1
```
→ RE1.5: 0x81 nach Treffer → Lokomotion (Sub 0); 0x83 → weiter liegen (Sub 5). Port
`enemy_ai_common.c` (downed-HURT Phase 2) bildet das bereits so ab.

---

## 3. Port-Ist

| Datei:Zeile | Was er tut | Warum falsch |
|---|---|---|
| `enemy_ai_re2_zombie.c:3165-3195` `re2z_crawl_exec_move` P0 | `re2z_clip(5,0,…)`, `re2z_root144 = 0`, `anim_frame = rand&0xF`; **kein** Re-Anker der Momentaufnahme; Kommentar: „der zweite Delta ist zwangsläufig 0 (prev == current)" | Der Kommentar stammt aus dem alten Bildnummern-Modell. Seit Runde 12 (`re15_clip_root_motion_delta`, `enemy_ai_common.c:691-760`) gilt die **Momentaufnahme** `root_prev_x/z`; die zwei baren e7c @0x80103094/@0x801030B8 verankern sie im Original neu — der Port lässt sie aus. |
| `enemy_ai_common.c:2692-2722` `re15_re2z_move_root` | `prev = (root_prev_motion == motion) ? root_prev_kf : -1` | Nach HURT (Clip 6 → zurück Clip 5) ist `root_prev_motion == 5` noch wahr (HURT ruft move_root nie, `re2z_crawl_hurt` :6508-6551), also `prev = 34`, `dx = sx(7) − 787 = −830`. Nur der Clip-WECHSEL re-ankert, der Wieder-EINTRITT in denselben Clip nicht. |
| `enemy_ai_re2_zombie.c:3189` `re2z_root144 = 0` | P1-Torwächter liest 0 | Original: `+0x144 = sx(r) − sx(0)` (zweiter e7c). Für r = 15 (sx = 29 ≥ 21) nimmt das Original den Steuerblock im Wiedereintritts-Tick, der Port nicht. Kleiner, aber derselbe Baustein. |
| `enemy_ai_re2_zombie.c:7684-7700` `re2z_init`, `(sel==1||sel==3) && beh&0x80` | `re15_re2z_enter_crawler(e, NULL, 0)` → Wort 1 = **Lokomotion sofort**, für sel 1 UND 3 | RE2-INIT @0x80100B60-8C: Deskriptor 1 → **0x201 WAIT** (Totstellen), 3 → 1. Der Port nimmt für beide den 3er-Zweig. |
| `enemy_ai_re2_zombie.c:2863-2874` `re15_re2z_enter_crawler(e, pl, sub)` | Wort `1 | sub<<8`, `+0x10E = (…&~0x3F)|1`, `sca_mask = 8` | Korrekt und wiederverwendbar; `sub = 2` ist genau der WAIT-Eintritt. |
| `enemy_ai_re2_zombie.c:3229-3258` `re2z_crawl_decide_wait` / `re2z_crawl_exec_wait` | DECIDE[2]/EXEC[2] byte-true portiert (arc 512, 0x514, Riegel, LOS, Etage → 0x101; Clip 0x17 Rate 0) | Vorhanden, aber ohne Produzenten für Sub 2 beim Spawn — der Zweig läuft heute nie. |
| `enemy_ai_re2_zombie.c:7527` HP-Seed | `(rand&0x1f)+50`, nicht halbiert | RE2-INIT halbiert die Kriecher-HP @0x80100B3C-5C (gemessen hp 95/75 statt 47/37). Nebenpunkt; kein Teil des Nutzer-Reports. |
| `enemy_ai_re2_zombie.c:3284` `sub_state_1 > 2 → re2z_active` | Fallback auf die AUFRECHT-Tabelle | Dokumentiert OFFEN (Griff-Ausgang 0x501 @0x80102D24-2C). Mit dem WAIT→GRIFF-Eintritt (§2.4) wird dieser Pfad häufiger erreicht — Risiko in §4. |

---

## 4. Fix-Plan (Phase 2)

**Reihenfolge:** F1 (Rückversatz) zuerst — isoliert, gepinnt; dann F2 (Totstellen); F3 optional.

### F1 — Momentaufnahme im Kriech-P0 neu verankern (`enemy_ai_re2_zombie.c` `re2z_crawl_exec_move`, `enemy_ai_common.c`)

1. **Neuer Helfer** `void re15_re2z_root_probe(re15_actor_t *e)` in `enemy_ai_common.c` neben
   `re15_re2z_move_root` = „barer `FUN_80015E7C`": dieselbe Bankwahl, ruft
   `re15_clip_root_motion_delta` mit `prev = (root_prev_motion == motion) ? root_prev_kf : -1`,
   **legt nur ab** (`root_prev_x/z`, `re2z_root144`, `root_prev_kf/motion`), **bewegt nicht**
   (@0x80015FC4/C8 Momentaufnahme, @0x80015FD8 `+0x144`; die Anwendung ist erst `FUN_800152C8`
   @0x80015314-34). Umsetzung: `re15_clip_root_motion_delta` bekommt einen Parameter `apply`
   (0 = Position nicht anfassen), sonst unverändert.
2. In P0 exakt an den Originalstellen:
   ```
   re2z_clip(e, 5, 0, 0xF, 0x100, 1);         /* +0x14C = 0x000F0005 @0x80103064-6C */
   e->sub_state_2 = 1;                         /* @0x80103070-78 */
   e->re2z_t158 = (rand&7)+7;                  /* @0x8010308C-98 */
   re15_re2z_root_probe(e);                    /* e7c #1 @0x80103094, Bild 0 -> Momentaufnahme := sx(0) */
   e->anim_frame = rand & 0xF;                 /* sb 333 @0x801030BC (Delay-Slot, VOR dem Sprungziel) */
   re15_re2z_root_probe(e);                    /* e7c #2 @0x801030B8 -> +0x144 = sx(r)-sx(0), Momentaufnahme := sx(r) */
   ```
   und die Zeile `e->re2z_root144 = 0;` (:3189) samt dem falschen Kommentar entfernen.
   Damit liefert der P1-`move_root` (@0x8010312C) im Wiedereintritts-Tick Delta 0 (prev = r,
   Momentaufnahme = sx(r)) — identisch zum Original (§2.2 Schritt 4).
3. **Erwartung (Sonde Teil B):** f38 `dx ∈ {0}` statt −830; danach die Tabellenfolge
   `sx(8)−sx(7) = −6`, `−2`, `0`, `+5` … (ist im Port ab f39 bereits richtig).
4. **Pin:** neuer ctest `unit_re2z_crawl_hurt_reentry` (aus der Sonde abgeleitet, ROOM1010 Cut 4,
   Riegel gesetzt, Pistole DOWN, Treffer): `|dx| ≤ 60` in jedem Bild zwischen HURT-P2 und +5
   Bildern (60 = größter |Δsx| der Tabelle C ist 62; Schranke exakt aus der Bank ableiten, nicht
   raten: `max_f |sx(f+1)−sx(f)|`). Zusätzlich Gegenprobe `probe_1030_crawl_live` unverändert.
5. **Risiko:** `re15_re2z_move_root`-Nutzer außerhalb des Kriechers (Hund/Krähe/Gang) bleiben
   unberührt, weil nur P0 des Kriechers den neuen Helfer ruft. Derselbe Fehler-Typ kann in
   JEDEM Executor stecken, der einen Clip **wieder** betritt, ohne dass ein Clip-Wechsel dazwischen
   ein `root_prev_kf = -1` setzt (Muster `enemy_ai_re2_zombie.c:1299/1393/1569`); nach F1 einen
   grep-Zensus über alle `re2z_clip(…, loop=1)`-Eintritte mit anschließendem `move_root` machen.

### F2 — Totstellen: RE2-Deskriptor 1 → WAIT (`enemy_ai_re2_zombie.c` `re2z_init` :7684-7700)

```
if ((sel == 1 || sel == 3) && (beh & 0x80u)) {
    /* RE2-INIT @0x80100B60-8C: (+0x10E&0x3F)==1 -> sw 0x201 @0x80100B74 (Kriecher-WAIT, EXEC[2]
     * @0x80103B48 Clip 23 Rate 0; Wecker NUR DECIDE[2] @0x80103A70 -> 0x101 GRIFF, oder HURT-P2
     * @0x80107A54-58 -> Lokomotion); ==3 -> sw a1(=1) @0x80100B8C (Lokomotion sofort).
     * PORT-MAPPING: RE1.5-sel {1,3} == RE2-Deskriptor {1,3}. */
    re15_re2z_enter_crawler(e, NULL, (sel == 1) ? 2u : 0u);
    e->re2z_f10e |= 0x2000u;                /* ori 0x2000 @0x80100B2C-34 */
}
```
- `re15_re2z_spawn_pose_seed` sät bereits Clip 23 (@0x80100AF8-FC, `:7471-7472`) — bleibt.
- **Verhalten danach (byte-true RE2):** ROOM1010-Südtür: beide Kriecher liegen still (Clip 23),
  bis der Spieler in den 512er-Sektor vor ihnen kommt und näher als 1300 ist (→ Beingriff), oder
  bis er sie anschießt (→ Kriechen). Das ist die Totstell-Aktion, die der Nutzer aus RE2 kennt.
- **Abweichung zum RE1.5-Original, bewusst:** RE1.5 weckt bei dist < 3000 in die Lokomotion;
  RE2 kennt diesen Wecker nicht. Unter RE2-Flavor gilt RE2 (Nutzer: „So eine Totstell-Aktion
  gibt es in RE2 auf jeden Fall auch"). Wer RE1.5-Verhalten will, hat es im RE1.5-Flavor.
- **Pin:** `unit_re2z_crawler_spawn_wait`: ROOM1010 Cut 4, RE2, Spieler am Tür-Ziel, 300 Bilder →
  `sub_state_1 == 2`, `motion == 0x17`, Position unverändert; dann Spieler vor den Kriecher
  (dist < 0x514, im Sektor) → `state word == 0x101` im nächsten Tick. Bestehende Pins
  `probe_1010_kriecher` (Messsonde), `test_re2_zombie_abc`, `probe_re2z_crawl_attack` müssen grün
  bleiben (ROOM1030 kommt über die Skript-Brücke mit `sub 0`, nicht über `re2z_init` — unberührt).
- **Risiko 1 — OFFENER Griff-Ausgang:** nach dem Beingriff schreibt Kriecher-EXEC[1] Phase 8
  `0x501` (@0x80102D24-2C); `re2z_crawl` (:3284) fällt bei `+0x5 > 2` auf die AUFRECHT-Tabelle
  zurück (dokumentiert OFFEN). Mit F2 wird dieser Pfad zum Regelfall (WAIT → GRIFF → 0x501).
  **Vor F2 klären**, was das Original nach dem Kriecher-Griff tut (Weg: Phasenkette von
  `FUN_801025EC` unter `s5 = 1` bis zum Ausgang durchrechnen; Kandidat: ein `+0x10E`-gegateter
  Store, der für den Kriecher nicht 0x501 sondern 1 schreibt). Bis dahin F2 hinter einen
  Messlauf stellen, der den Griff-Ausgang protokolliert (Sonde: Riegel NICHT setzen).
- **Risiko 2 — gerader Schuss trifft den Liegenden nicht** (Sonde: 0 Treffer ohne DOWN). Im
  RE2-Original ist die Kriecher-Trefferbox flach (@0x80100B00-20: 200/200/−350/350). Ob RE1.5-
  Original einen geraden Pistolenschuss auf einen Liegenden zulässt, ist hier nicht gemessen;
  kein Teil dieses Reports, aber der Nutzer wird nach F2 öfter auf Liegende schießen.

### F3 (optional, byte-true RE2-INIT-Rest)
- HP-Halbierung @0x80100B3C-5C (`hp = (hp + (hp>>15&1)) >> 1`), Trefferbox-Halbworte
  @0x80100B00-20, word0-Flags @0x80100B28-44. Nur mit eigener Messung der Konsumenten
  (Trefferbox-Felder des Ports für RE2-Zombies) einbauen — nicht blind.

### RE1.5-Flavor-Nebenbefund (eigene Runde)
Der RE1.5-Kriecher Typ 0x10 in ROOM1010 teleportiert (§1). Erster Weg: `re15_enemy_handlock_step`
mit EM010-Bank gegen EM016 (ROOM1030) vergleichen — Hand-Bones 8-13 / Lock-Bone 10/13
(@0x801094B8-C8) könnten in EM010 andere Indizes haben; zweiter Weg: `s_zcrawl_ref`-Cache vs.
Wandklemme (die wiederkehrende x = −1982 riecht nach Zellenkante). Nicht in diesem Dossier gelöst.

---

## 5. Offen / nicht belegt

- **Hardware-Gegenprobe fehlt** für beide Punkte: kein DuckStation-/PCSX-Lauf, der (a) den RE2-
  Kriecher nach einem Treffer bildweise zeigt oder (b) ROOM1010 im RE1.5-Original über die Südtür
  betritt. Die RE2-Seite ist statisch lückenlos (§2.1-2.4); die Konstante „−830" ist aus dem Port
  + der Bank nachgerechnet, nicht am Original gemessen (das Original hat den Sprung laut §2.2 nicht).
- **Kriecher-Griff-Ausgang 0x501** (Risiko 1) — im Port OFFEN, vor F2 zu klären.
- **RE2-INIT-Rest** (HP-Halbierung, Trefferbox, word0-Flags 0x04000000/&0xF3FFFFFF): Konsumenten
  nicht verfolgt.
- **`+0x10E |= 0x2000` am Spawn** (@0x80100B2C-34): Bedeutung im Port als Gate `!(pz->re2z_f10e &
  0x2000)` (:1937, Lunge-Nachbar-Test) und Löscher @0x8010373C-4C bekannt; ob das Setzen am Spawn
  Nebenwirkungen auf die Nachbar-Zombies hat, nicht gemessen.
- **RE1.5-Flavor-Teleport** (§1 Nebenbefund): gemessen, Ursache nicht RE't.
- **Gerader Schuss vs. Liegender** (Risiko 2): welches Tor blockt (Elevations-Band / Trefferbox-
  Höhe), nicht protokolliert — die Sonde druckt die Gate-Kette nicht.
