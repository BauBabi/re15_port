# Runde 34 (Granaten) — Gegenpruefung Spur B (Schaden und Gegnerreaktion)

Stand: 2026-09-30, Zweig `r34g/b-schaden` (HEAD 6d806abd), Arbeitsbaum `.claude/worktrees/r34g_b`,
Bauverzeichnis `re15_port/build_r34_b`. Eigene Ausgaben (unversioniert): `build/r34g_b_gp/`.
Pruefgegenstand: `git diff 8d8651e4..HEAD` (die 13 Commits der Spur B; 8d8651e4 = Basis r34g/c0-vertrag).
Der Gegenpruefer aendert KEINEN Code; Mutationsproben werden angewandt, gebaut, gefahren und aus einer
Sicherungskopie zurueckgesetzt (Nachweis `git diff` leer am Ende).

STATUS: IN ARBEIT (fortlaufend geschrieben)

---

## 0. Umfang und Vorgehen

| Punkt | Weg |
|---|---|
| (1) Konstanten + @0x | jede tragende Konstante selbst disassembliert bzw. Bytes gelesen (`re15_disasm.py` / `re2_disasm.py`, Mitschnitte `build/r34g_b_gp/*.dis`) |
| (2) Semantik | Zeitlinie/Einordnung BAUPLAN §1/§2 gegen Code und Disasm (Delay-Slots, Reihenfolge, Vorzeichen, s16/s32) |
| (3) Sonden | Quelltext gelesen (echte Engine? Erwartung aus Disasm? Negativ-Kontrollen?), eigene Mutationsproben |
| (4) Dateibesitz | `git diff --stat 8d8651e4..HEAD` gegen BAUPLAN §3.0 |
| (5) Tests | eigene Laeufe im Bauverzeichnis des Baums |
| (6) Regression | Pfade ausserhalb der Granate, die die Aenderungen mitnehmen |
| (7) Vollstaendigkeit | BAUPLAN §3.2 B1-B12 + Abnahme Spur B |

---

## 1. Selbst nachgepruefte Konstanten (Auszug der tragenden Mechanik)

### 1.1 RE1.5 PSX.EXE
| Behauptung | eigener Beleg | Urteil |
|---|---|---|
| Gate B `(+0x93 & 3) == 3`, Sprung HINTER den Zaehler | `80012f54 lw v0,144(s1)` / `80012f58 lui v1,0x300` / `80012f5c and` / `80012f60 beq v0,v1,0x8001302c`; `80013024 addiu s4,s4,1` liegt davor, Ziel 0x8001302c danach; Delay `80012f64 andi v0,s2,0xff` laeuft in beiden Faellen (Schleifenbedingung) | bestaetigt |
| Riegel Bit 0 -> `|= 2`, gezaehlt, kein Schaden | `80012fbc andi v0,v1,0x1` / `80012fc0 beq` / `80012fc4 ori v0,v1,0x2` (Delay) / `80012fc8 j 0x80013024` / `80012fcc sb v0,147(s1)` | bestaetigt (Port: Kopf von `re15_enemy_take_damage_at`) |
| Tabellen Art 0..10 | `read 0x8006f418 11 --w 2 --signed` = [10,20,1000,1000,1000,50,100,200,300,1000,0]; `read 0x8006f430 11` = [3,3,9,10,11,14,15,16,17,18,20] | bestaetigt |
| FUN_8002b498 Versatz | `8002b4bc lw s0,120(s1)` / `8002b4c4 lhu v0,106(s1)` / `jal 0x80068098` / VXY0 = (Kasten.x, Gier), VZ0 = Kasten.z (`lwc2` @0x800661e8/ec) / MVMVA `4a486012` @0x800661f4 / MAC1..3 -> `swc2` @0x800661f8-200 / `8002b504 sh v0,0(s2)` (MAC1 low16), `8002b510 sh` Kasten.y, `8002b51c sh` (MAC3 low16) | bestaetigt; Port `re15_hitbox_versatz` rechnet R[0]/R[2] bzw. R[6]/R[8] mit `(int16_t)(... >> 12)` = dieselbe Kuerzung |
| Kaesten (Bytes) | Hund `80120f64: 00 00 30 fd 00 00 84 03 d0 02 c2 01`, Zeiger `*(0x80120f70) = 0x80120F64`, INIT `8010da68 lw v0,3952(v0)` / `8010da70 sw v0,120(v1)`; Alligator STAGE2 `80118b98: e8 03 30 fd 00 00 98 08 d0 02 20 03`; Feuer `*(0x80121264) = 0x80121258`, `00 00 00 00 00 00 58 02 d0 02 58 02`; NPC 0x45 `*(0x80121734) = 0x80121728`, `00 00 60 fa 00 00 f4 01 a0 05 f4 01`; 0x4b `*(0x801218d4) = 0x801218C8`, `00 00 60 fa 00 00 2c 01 a0 05 2c 01`; Vorgabe `80072be0: 00 00 00 00 00 00 01 00 01 00 01 00`, Setzer `800422c8-d0` | alle bestaetigt |
| Treppe setzen/loeschen | `80038a3c lbu v0,-13593` / `80038a50 ori v0,v0,0x1` / `80038a58 sb v0,-13593(at)`; `80038c1c andi v0,v0,0xfe` / `80038c24 sb`; `80038cd0 lbu` / `80038cf8 ori 0x1` / `80038d00 sb`; `80038eb8 andi 0xfe` / `80038ec0 sb` | bestaetigt |

### 1.2 RE1.5 STAGE-Overlays
| Behauptung | eigener Beleg | Urteil |
|---|---|---|
| Spurtabellen 0x27 (STAGE1) | `read 0x801214a8 22` -> 0x8011B018 x7, B1EC x2, B400 x3, B018, B1EC, B018, B400 x4, B018 x2, B1EC; `read 0x80121500 22` -> B7B8 x7, B998 x2, BB9C x3, B7B8, B998, B7B8, BB9C x4, B7B8 x3 | = `s_spur_hurt` / `s_spur_death` |
| Spurtabellen 0x29 (STAGE3) | `read 0x8011ed84 22` (484C/4A40/4CB8), `read 0x8011eddc 22` (5070/5280/54B4), gleiche Belegung | bestaetigt |
| 0x29 auch STAGE4/5 (vom Bauer nicht gelesen) | eigene Suche nach den Explosions-Handlern: STAGE4 HURT `0x80119fb4`, DEATH `0x8011a00c`; STAGE5 HURT `0x8011fb1c`, DEATH `0x8011fb74` — je 22 Worte, dieselbe Belegung | bestaetigt (Port-Tabelle gilt fuer alle drei Stages) |
| 0x29 HURT-Zucken faellt von Phase 0 in Phase 1 | `8011496c ori a0,zero,0x2` (Delay von `jal 0x800453d0`) -> `80114970` = Phase-1-Kopf (anim_set @0x80114984), kein Sprung dazwischen | bestaetigt |
| 0x23 Tod 0x8010ea30 (STAGE2) | Phase 0 `8010ea84-88` \|= 2, `8010eaa4 ori v0,zero,0xd`, `8010eac4/c8` Crossfade 7, `8010ead8 sh zero,140` (+0x8c), `8010eae8 sh zero,156` (+0x9c), `8010eaf8-b18` +0x1ba = -(+0x82*1800) (7*32+1 = 225, *8 = 1800), faellt nach `8010eb58`; Phase 1 `8010eb9c lbu 480` (+0x1e0) / `8010ebb4 bne v1,v0(=0x14)` / `8010ebbc sb 2,7`; `8010ebd4 sltiu 0x15` -> `jal 0x8001c1a4` mit a2 = -80 | bestaetigt |

### 1.3 RE2 PSX.EXE
| Behauptung | eigener Beleg (`re2_disasm.py dis 0x800470c0 420`, `build/r34g_b_gp/re2_800470c0.dis`) | Urteil |
|---|---|---|
| Gates 1-4, Band, Radius-Puffer, Ruecknahme | `8004712c-30`, `80047138-40` (`lbu 467` ganzes Byte), `80047148-50`, `80047158-64`; Band `8004716c-a4`; Radius `800471bc-ec`; Ruecknahme `800473dc-408` nur ueber `800471f0 beq v0,zero,0x800473dc` | bestaetigt |
| Modus-Bit 0x10000 | `80047208-10`: Bit 0 -> `beq -> 0x80047434` = Einzelzweig NACH dem ersten Treffer (Schleife verlassen); Bit gesetzt -> Anwendung in der Schleife (`80047218..800473d8`) | bestaetigt (Port: Anwendung + `break`) |
| Record, Schaden, Zustandswort | `8004722c lw a1,27272(at)`, `(Zeile*5*4 - 20)`, `srlv` um 10K, `andi 0x3ff`, `sw 2/3` @0x80047288/90 | bestaetigt |
| Zone | `80047294-98` Zone 1; `8004729c-a4` Bit 0x20000 sonst -> `80047314` (Kopf- UND Beinregel uebersprungen); `800472b4-b8` a0 = (s16)+0x98 >> 1; `800472c8 slt` Y+a0 < P.y -> 0; `800472d8-e4` Wort0 & 0x10000000; `800472e8` (Delay) v1 = 2a0, `800472fc` Y+3a0, `80047300 slt P.y < Y+3a0` -> 2 | bestaetigt |
| +0x5, Sperre, Richtung | `80047324 sb s5,5(s1)`; `8004731c/2c/34` alt & 0x80, `80047338-4c` (w1>>9)&0x7F; `80047350-54` a0/a1 = P.x/P.z, a2/a3 = +0x38/+0x40 (`80047314`/`8004733c`), `80047360 lh v1,118(s1)`; drei Tests `8004736c-3d8` | bestaetigt |
| Einzelzweig Bit 0x40000 | `80047534-60`: `lw v0,-7376(0x800d)` (= *(0x800ce330)), `lbu +0x106` Gegner vs. dieses Objekt, gleich -> Zone 0 | bestaetigt (fuer GL-Hitcodes ohne Wirkung) |
| FUN_80041EF8 inkl. S1/S2 | lokal.t `80041f28-78`; C = 0x8002ce94 (cv=TR); C/4 `80041fbc-c8`; e1 = R*(p2, hi16, 0) (`80041fcc-dc`), e2 = R*(0,0,lo16(2p3)) (`80041fe8-ffc`); **`80042040 sw t1,64(sp)` (t1 = `lh 24(sp)` = e1.x) -> m0[0][1] = Vorzeichen(e1.x)**; **`800420d4 sw v1,100(sp)` (v1 = `lh 56(sp)` = s16 d2.x) -> m1[1][0] = Vorzeichen(d2.x)**; m0[2] = [0,0,4096] aus der Einheitsmatrix @sp+64 (`80041f4c-5c`); Test `800420ec-118` | bestaetigt — die S1/S2-Rekonstruktion des Bauers stimmt Wort fuer Wort |
| FUN_800154AC | `800154b4-d8` s16-Kuerzung, dx == 0 -> 1024 / 3072 (dz > 0), sonst `(dx >= 0 ? 4096 : 2048) - catan` & 0xfff | = Port `atan2_q12` (actor_locomotion.c:128) minus 0x400: bestaetigt |
| FUN_80036E30 | `80036e34-3c` Gier & 0x400 -> +0xA0/+0xA2 = +0x96/+0x94 getauscht; +0x84 = +0x38 + (s16)+0xA0, +0x8C = +0x40 + (s16)+0xA2 | bestaetigt |
| Records Z9/Z10/Z11 | selbst gelesen ueber *(0x800A6A88 + Typ*4): Zombie 00A0C8C8/0050C8C8/00A0C8C8, 0x16 00A0C850/0050C850/00A0C8C8, Hund 00A0C92C/0050C92C/00A0C92C, Kraehe 00F0F03C x3, Spinne 0140F03C/00520882/00A0F05A (w1 078F1FB4/078F1F68/078F1F68), Arm 0140F03C/00A0F03C/00A0F03C, G5 05014050/00511846/00A11846; alle uebrigen w1 = 078F1E0A | alle = `s_re2gl_rec_*` |

### 1.4 RE2-Overlays
| Behauptung | eigener Beleg | Urteil |
|---|---|---|
| Zombie-DoT EXEC[1] | EMZ0 `80101dc0-e90`; Rohwort `80101e24 0x00508007` = SRAV rd=s0 rt=s0 rs=v0; Gier-`sh` im Delay `80101e50` (immer); Tod `80101e58-90`, `j 0x80101f64`; Saeure-Zucken `80101e94-ec0`; Platz: direkt hinter dem +0x15A-Block (`80101d68-dbc`) | bestaetigt |
| Zombie-DoT EXEC[2] | EMZ0 `80102474-54c` hinter `jal 0x800152c8` @0x8010246c, vor der Ausstiegsleiter `80102550`; ohne Wuerfe | bestaetigt |
| Zaehler +0x236 | EMZ0 `801004f8-508` nach `jalr` @0x801004e8 und `jal 0x8010c2a4` @0x801004f0, unbedingt; Port `re15_re2z_tick` ohne fruehen Ausstieg zwischen Kopf und Zaehler | bestaetigt |
| Leichen-Ausblender | EMZ0 `8010a80c-868` (a1 = neues +0x15A, `andi a1&3`, Part-0-Byte 16, 15 Parts, `+0xFFFEFEFF`) | bestaetigt |
| Hund Tabellen | `table 0x801055cc 22` / `table 0x80105618 22` / `table 0x80105688 4`, `bytes 0x80105680` = `02 03 04 07 08 09 0a` | bestaetigt |
| Hund P0 0x80104694 | `801046a8-d4` (Zeile 9 && 1D2 >= 3 -> nur Kern, `j 0x8010475c`); `801046e4 jal Kern` + Delay `801046e8 sb s1,561`; `801046ec jal 0x80104440`; `801046f4 lbu v1,5` (Zeile NACH dem Kern neu gelesen — der Kern/HURT-P0 schreibt +0x5 nicht, nur `sh ...,6` @0x80103430/50); `80104708` Budget 1 im Delay; `8010473c andi 0x4a`; `80104758 sb 18` | bestaetigt |
| Hund Teile-Wurf, Brand, Saeure, HURT 9/10/11 | `80104440-4d8`; `80104774-800`; `8010481c-89c`; `80103ce4-d78`, `80103d9c-e3c`, `80103e60-edc` (FX-8-Schleife `sltu s0 < +0x21F` NACH dem Spawner-Abzug; `+0x5 := 1` @0x80103d68 / @0x80103e30 / @0x80103edc; `ori 0x80` @0x80103e34-3c) | bestaetigt |
| Spinne | EMS25 `8010609c-c4` (20 Parts), `80104a5c-b58`, `80104b88-c2c` (Part 19 = +3268, +0x9E 90, +0x98/+0x9A 0, +0x9C/+0x9D 100/100, Flags \|= 0x10), `80104c5c-c98` | bestaetigt |
| G5 (em36, `CDEMD0_EM36_ai1.BIN`) | Byte-Tabelle `801056b0: 01 0d 00 00 05 05 05 05 0e 14 0e 14 0e 0e 0e 05 05 14 01 01 14 01 00 00` -> Zeile 0..20 = 0,5,5,5,5,14,20,14,20,14,14,14,5,5,20,1,1,20,1,0,0; Main `801000ec-15c` (Zaehler `addiu v0,v1,255` + `sb` im Delay, bei 0: Akku-1 und `sb 15` im Delay @0x8010014c, Akku 0 -> Fenster zu); Treffer `801029b0-a8c`; Ctor `801003fc` 600 (bei Spielwort-Bit 0x20: 400 @0x80100414-18), `80100534-38`, `80100578-8c`, `80100580`. ai0 ist dieselbe Routine mit anderer Ladeadresse (Diff nur Sprungziele) | bestaetigt |

---

## 2. Eigene Messungen (Messprogramme gegen `libre15_engine.a` des Baums, `build/r34g_b_gp/mess*.c`, unversioniert)

| Nr | Aufbau | Ergebnis |
|---|---|---|
| mess1 M1 | RE1.5-Flavor, Import AN (Vorgabe), Zombie 0x10 HP 80, `re15_re2_gl_apply(P,0,{-600,0,300,150},0x2002000A)` | **-15**, +0x5 = 14, Zustand 2 |
| mess1 M1b | dasselbe, Import AUS | -50 (O-VB4 wie dokumentiert) |
| mess1 M2 | RE2-Flavor, RE2-Zombie 0x10 | -5 (Z10 K2 = 0x0050C8C8 >> 20), +0x5 = 10 |
| mess1 M3 | `re15_enemy_apply_hitbox(a, 0x23)` | hit_offset_x = **1000** — gilt auch fuer den Gator-Boss 2090 (dessen Radien skaliert das Modul auf 2/3, den Versatz nicht) |
| mess2 | Hund-Sektorkasten (Resolver), Punkt 1350 laengs / quer zur Blickrichtung (RotY-Konvention lokal +x -> (cos g, -sin g)) | Gier 0 / 1024: laengs 1, quer 0; **Gier 512 / 1536: laengs 0, quer 1** |
| mess3 | wie M1, 12 Bodenfeuer-Treffer seitlich (P.z +-300), Riegel zwischen den Treffern frei | je Treffer -15, +0x152 13 -> 11 -> ... -> -1; **Treffer 7: +0x21A \|= 0x20, part_mesh[9] = 15 (Bein ab, RE2-Zerleger)** |
| mess4 | RE2-Flavor ROOM1140, Brad 0x11, Explosion Art 4 (P 300 in +x, Spieler 6000 in -x) | Bild X: Reserve 13/13/13, 1D0 0x0021, gl 1; **Bild X+1..X+3: Reserve 13/13/13, 1D0 0x0021, gl 0** -> die Wache in re2z_hurt wirkt |
| mess5 | RE2-Flavor ROOM1140, RE2-Zombie 0x10 (HP 50) allein, Handgranate Art 2 (P 300 daneben, gleicher Boden), RE2-Zufallsstrom je Lauf um k = 0..23 Zuege verschoben | Treffer: HP 50 -> -150, Zustand 3, Spalte 0; **Ausgang 24/24: Zustand 1, HP 10, +0x10E 0x2001 (Kriecher), 0 Leichen** |
| mess6 | wie mess5, danach 2. Granate auf den Kriecher | Spalte 1 (Kriecherkasten -350) -> DEATH[9][1] 0x80108BEC -> Leiche |

---

## 3. Eigene Mutationsproben (`build/r34g_b_gp/mut_run.py`, Ergebnis `mut_run.log`)

Je Mutation: Sicherung, Ersetzung auf genau einer Zeile, gezielter Bau der zwei Sonden, beide Sonden, Datei
aus der Sicherung zurueck. Danach `git diff` leer, `local_build.sh build` -> LOCAL-BUILD-OK, beide Sonden gruen.

| Id | Mutation (Datei:Zeile) | Ergebnis | Urteil |
|---|---|---|---|
| G01 | Gate B zaehlt (`return 1`) re15_damage.c:3425 | 11/18/16/17/94 rot | gepinnt |
| G02 | S1*S2-Term der Box weg re15_damage.c:4055 | **gruen** | ungepinnt (Wirkung nur an Grenzfaellen +-1/4096) |
| G03 | +0x1D0 &= 0xFF00 je Kandidat weg re15_damage.c:4186 | **gruen** | ungepinnt (Sonden starten mit 1D0 = 0) |
| G04 | Explosionsstempel Klammer 1 re15_damage.c:4137 | 31/34/101-104 rot | gepinnt |
| G05 | DoT-Gier-Zucken weg enemy_ai_re2_zombie.c:1474 | **gruen** | ungepinnt |
| G06 | GL-Wache in re2z_hurt weg (zweiter Stempel) enemy_ai_re2_zombie.c:7079 | **gruen** | **ungepinnt** (s. Mangel M2) |
| G07 | Spinne Part 19 +0x9E 91 enemy_ai_re2_spider.c:2315 | 173 rot | gepinnt |
| G08 | 0x29 Luft-Tod SE 1 bei Bild 0x14 enemy_ai_common.c:11506 | **gruen** | Luft-Spur ungepinnt |
| G09 | 0x29 HURT-Luft Ausgang HP < 0 statt < 50 enemy_ai_common.c:11431 | **gruen** | Luft-Spur ungepinnt |
| G10 | 0x2b Ph.0 Crossfade 7 statt 0 enemy_ai_common.c:13772 | **gruen** | ungepinnt |
| G11 | 0x23 +0x1ba mit 1700 enemy_ai_common.c:13336 | **gruen** | ungepinnt |
| G12 | G5 Akku-Zerfall weg enemy_ai_boss_g5.c:1528 | 195 rot | gepinnt |
| G13 | Drehsinn FUN_8002b498 umgekehrt re15_damage.c:3352 | 41 rot | gepinnt |
| G14 | E4 erst ab Art 3 re15_damage.c:3001 | 31/34/35/38 rot | gepinnt |
| G15 | Hund Flag-Tor 0x4A vor FX 7 weg enemy_ai_re2_dog.c:2280 | **gruen** | ungepinnt (Sonde 123 prueft nur FX7 <= 1) |
| G16 | NPC-Kennung HP <= 0 re15_damage.c:3471 | 15 rot | gepinnt |
| G17 | Hund HURT 11 ohne +0x5 := 1 enemy_ai_re2_dog.c:2119 | 164 rot | gepinnt |
| G18 | Ausblender je 8 statt 4 Bilder enemy_ai_re2_zombie.c:7950 | 105 rot | gepinnt |
| G19 | E16 G5 HE 70 statt 80 re15_damage.c:3012 | 190/194 rot | gepinnt |

11 von 19 rot. Die 8 gruenen betreffen Details, deren Code ich gegen den Disasm korrekt fand — sie sind
nur nicht durch eine Sonde geschuetzt (Regressionsschutz fehlt), mit Ausnahme von G06 (tragend, s. M2).

---

## 4. Tests im Bauverzeichnis des Baums

* `unit_r34_schaden` + `unit_r34_reaktion`: gruen (vor und nach den Mutationsproben).
* Alle 399 Nicht-Integrationstests (`ctest -E integration -j 4`): **399/399 gruen** (45 s).
* Integrationstests (31, `-j 1`, waehrend die Baeume r34g_a / r34g_c / r34n_generator parallel ihre
  exe-Tests fuhren): siehe Abschnitt 6.
