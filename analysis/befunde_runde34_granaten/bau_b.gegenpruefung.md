# Runde 34 (Granaten) — Gegenpruefung Spur B (Schaden und Gegnerreaktion)

Stand: 2026-09-30, Zweig `r34g/b-schaden` (HEAD 6d806abd), Arbeitsbaum `.claude/worktrees/r34g_b`,
Bauverzeichnis `re15_port/build_r34_b`. Eigene Ausgaben (unversioniert): `build/r34g_b_gp/`.
Pruefgegenstand: `git diff 8d8651e4..HEAD` (die 13 Commits der Spur B; 8d8651e4 = Basis r34g/c0-vertrag).
Der Gegenpruefer aendert KEINEN Code; Mutationsproben werden angewandt, gebaut, gefahren und aus einer
Sicherungskopie zurueckgesetzt (Nachweis `git diff` leer am Ende).

STATUS: ABGESCHLOSSEN — 1 kritisch (K1), 2 mittel (M1, M2), 8 Hinweise (H1-H8); Befundliste Abschnitt 5, Fazit Abschnitt 8.

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
| Je-Typ-Eingaben des Appliers | Hund EMD0G_MOD0 `80100284 addiu v0,zero,500` / `80100288 sh v0,148` (+0x94), `8010028c/94` -1000 -> +0x98, `80100298/9c` 1000 -> +0x9E, `80100290 addiu v1,zero,600` / `801002c4 sh v1,494`; Zombie EMZ0 `80100958-64` -1500/1500, `8010096c` 500 / `80100980 sh v1,494`, `80100990/94` +0x94/+0x96 = 0; Kraehe EMOVL21_S0 `801003b8/c4` -350, `801003c8/dc` 530, `801003f4/fc` 300 -> +0x1EE; Spinne EMS25 `80100414/20` 800 -> +0x1EE; Tentakel em37 (`re2_ems_cut.py` -> CDEMD0_EM37_ai1.BIN) `80100530 addiu v0,zero,-1` / `80100534 sh v0,342(s1)` | bestaetigt |
| RE2 Op 47 (GL-Explosion) Hitcode | RE2 PSX.EXE `80020d54 lui a3,0x1002` / `80020d58 ori a3,a3,0x9` / `80020d78 jal 0x800470c0` -> 0x10020009 = Klammer 1, Bit 0x20000, Zeile 9, Einzelmodus | Grundlage fuer K1 |

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

---

## 5. Maengel

### K1 (kritisch, Entscheidung vor der Integration) — Handgranate toetet keinen stehenden RE2-Zombie: er steht als Kriecher mit HP 10 wieder auf
* **Messung** (mess5, RE2-Flavor = Vorgabe, ROOM1140, echter `re15_game_step`): 24 von 24 Laeufen — Treffer
  HP 50 -> -150, Zustand 3, Spalte 0; Ausgang IMMER Zustand 1, HP 10, +0x10E 0x2001 (Kriecher), 0 Leichen.
  Erst eine zweite Granate toetet (Kriecherkasten -> Spalte 1 -> DEATH[9][1] 0x80108BEC, mess6).
* **Mechanik** (selbst gelesen): E6-Stempel mit Klammer 0 -> +0x1D2 = Zone 0 (Beine: Y + (-1500>>1) = -750 <
  P.y = -490) -> DEATH-Tabelle EMZ0 `read 0x8010CD68 9` = {0x80107438, 0x80108BEC, 0x80108BEC, 0x80108530 x6}
  -> Spalte 0 = 0x80107438 (Knockdown) mit dem `death`-Zweig: `8010778c jal 0x80015fe8` / `80107794 andi v0,v0,0x3`
  / `80107798 bne v0,zero,0x801077b0` (3/4 -> `801077b0 sh v0(=10),342(s2)`), sonst `801077a0 lb v0,363(s2)`
  (+0x16B) -> nur mit abgerissenem Arm Leiche. Port `re2z_hit_knockdown(death=1)` bildet das korrekt nach.
* **Widerspruch**: BAUPLAN §1.6 erwartet "Tod: HE DEATH[9][0] = 0x80107438 (Knockdown-Tod ...)". RE2s eigene
  GL-Explosion (Op 47) stempelt Hitcode **0x10020009** (`80020d54 lui a3,0x1002` / `80020d58 ori a3,a3,0x9`,
  re_gegner_re2_familie + Gegenpruefung §1.2) = Klammer 1 -> Spalte 3/4 -> DEATH[9][3] = **0x80108530**
  (Sturz-Tod -> Leiche). Die Kriecher-Wiederbelebung ist in RE2 die Reaktion auf den FLUG-Kontakt (0x00030009, K0),
  nicht auf die Explosion. Der Bauer hat das im Zensus als `t` gesehen und als "belegter Ablauf" eingeordnet, aber
  nicht als Abweichung vom BAUPLAN-Soll gemeldet. Die Abnahme B3 prueft nur den Stempel im Bild X, nicht den Ausgang.
* **Vorschlag**: Orchestrator-/Nutzer-Entscheidung vor der Integration: (a) Spalte wie Op 47 mit Klammer 1
  stempeln (+0x1D2 = Zone + 3), Schaden bleibt E4 (K0 = 200) -> Tod ueber 0x80108530 wie RE2-Retail (Achtung:
  global angewandt aendert Klammer 1 auch den Hund — Zeile 9 mit +0x1D2 >= 3 = nur Kern/Schrei statt "zerplatzt",
  @0x801046a8-d4 —, also ggf. nur fuer die Zombie-Familie); oder (b) den Kriecher-Ausgang ausdruecklich abnehmen.
  In beiden Faellen eine Sonde, die den AUSGANG (Leiche/Kriecher) nach N Bildern pinnt.

### M1 (mittel) — Bodenfeuer an RE1.5-KI-Zombies mit Import-Option: 15 statt 50 Schaden und RE2-Beinabriss
* **Messung**: mess1 M1 (Import AN = Vorgabe) -15 je Treffer, M1b (Import AUS) -50, M2 (RE2-KI) -5; mess3:
  Reserve +0x152 sinkt je Treffer um 2, **beim 7. Treffer Bein ab** (+0x21A |= 0x20, part_mesh[9] = 15).
* **Ursache**: (1) E4-Erweiterung auf `type >= 2u && type < 11u` (re15_damage.c:3001) greift auch fuer Art 5 ->
  `re15_enemy_dmg_row(e)[DAT_8006f430[5] = 14]` = `s_re2_wpn_dmg_zombie[14]` = 15 (@0x800A4258, RE2-Flammenwerfer-
  Zeile 16). BAUPLAN E4 nennt nur Art 2/3/4; das Dossier (bau_b.md §B4 "O-VB4") und Commit d89afba1 nennen fuer
  Art 5 "50 @0x8006f422". (2) Die Import-Bruecke `re15_re15_re2z_gore_hit(e, P, 1, 5)` stempelt ueber
  `re2z_row_from_atktype[5] = 9` die GL-EXPLOSIV-Zeile und zieht die Zonen-Reserve ab (w1 0x078F1E0A & 7 = 2);
  RE2 selbst zieht beim GL-Applier KEINE Reserve ab (Store-Liste @0x800471f8-0x800473d8, BAUPLAN K6 RESERVE).
* **Falsche Aussage im Dossier**: OFFEN 13 "Art 5 trifft RE2-KI-Typen im Spiel nicht ... nur im Zensus kuenstlich
  erreicht" — ueber O-VB4 erreicht Art 5 jeden RE1.5-KI-Zombie mit Import (Vorgabe im RE1.5-Flavor).
* Keine Sonde deckt den Fall ab (Pruefung 93 nimmt die Made 0x27 ohne Import). Wirksam, sobald C/D den Applier
  bindet (INTEGRATIONSWUNSCH 2).
* **Vorschlag**: Entscheidung dokumentieren und pinnen: E4 auf Art 2..4 begrenzen (dann 50 laut O-VB4) oder fuer
  Import-Zombies den RE2-Wert des RE2-Pfads (Z10 K2 = 5 @0x800A41E0); die Import-Bruecke fuer Art 5 nicht ueber die
  Explosiv-Zeile 9 und nicht mit Reserve-Abzug laufen lassen. Sonde: Import-Zombie + Bodenfeuer -> Schaden,
  +0x152, +0x21A.

### M2 (mittel) — tragende E6-Pruefung ohne Wirkung: die GL-Wache in re2z_hurt ist durch keine Sonde gepinnt
* Mutation **G06** (`if (e->re2_gl_stamp) ...` -> `if (0) ...`, enemy_ai_re2_zombie.c:7079 = zweiter Stempel mit
  Spielerpeilung und Reserve-Abzug) laesst beide Sonden gruen. Die Pruefungen 22/31/101/111 lesen den Stempel im
  Bild X, der Konsument re2z_hurt laeuft erst in X+1.
* Am Stand HEAD wirkt die Wache (mess4: Reserve 13/13/13 und 1D0 0x0021 aus P bleiben in X+1..X+3; mit Spielerpeilung
  waere es 0x01). Das Verhalten stimmt — es ist nur ungeschuetzt. Dasselbe fuer G03 (+0x1D0 &= 0xFF00 je Kandidat,
  re15_damage.c:4186, @0x8004716c-84).
* **Vorschlag**: in `unit_r34_reaktion` Teil zombie nach dem Treffer ein Bild fahren und Reserve + Richtungsbits
  pruefen (Negativ-Kontrolle: Schuss-Pfad zieht ab); fuer G03 einen Kandidaten mit vorbelegtem 1D0-Low-Byte.

### H1 (hinweis) — Gator-Boss ROOM2090 bekommt den Alligator-Versatz 1000 ungeskaliert
* mess1 M3: `re15_enemy_apply_hitbox(a, 0x23)` -> hit_offset_x = 1000 (re15_damage.c:3734) fuer JEDEN 0x23, also auch
  den Boss; dessen Modul skaliert Radien/Hoehe auf 2/3 (enemy_ai_boss_gator.c:1057-1061), den Versatz nicht. Wirkung:
  Granaten-/Bodenfeuer-Kasten des Bosses liegt 1000 vor der Figur. BAUPLAN §1.6/E16: Gator-Boss "unveraendert".
  Im Dossier nicht erwaehnt. **Vorschlag**: im Boss-Modul den Versatz mitskalieren oder bewusst 0 setzen
  (Nutzer-Design) und benennen.

### H2 (hinweis) — Hund: Immunitaet +0x1D3 Bit 0x80 nach HURT 10/16 wirkt nur gegen den GL-Applier
* `re15_re2_pause_filter_apply` (re15_damage.c:3843) gibt +0x93 Bit 0 frei, sobald `(+0x1D3 & 0x7F) == 0` — Bit 0x80
  bleibt unbeachtet. RE2 prueft in BEIDEN Appliern das ganze Byte (Schuss FUN_800410CC `80041270 lbu v0,467(s2)` /
  `80041278 bne`; GL `80047138-40`). Folge im Port: Granate/Schuss treffen den Hund 15 Bilder nach HURT 10 wieder,
  RE2 erst nach `&= 0x7F`. Vorbestehend (Filter aelter als Runde 34); fuer die Granate folgenlos (Hund stirbt an 300),
  Sonde 166 misst nur den GL-Applier. **Vorschlag**: im Dossier benennen; den Filter nur mit gemessener Freigabe aller
  `&= 0x7F`-Stellen auf das ganze Byte umstellen (sonst Dauersperre).

### H3 (hinweis) — Kraehe: OFFEN 3 ("+0x98 hat kein Port-Feld") ist falsch
* Der Port fuehrt +0x98 der Kraehe je Bild (`e->re2_hit_b98 = (ai_dist < 0x384) ? -350 : 0`,
  enemy_ai_re2_crow.c:1824, Beleg @0x801001ec-208), setzt nur `re2_hit_box_set` bewusst nicht (Messer-Tor).
  `re2_gl_typ` (Fall 0x21) nimmt deshalb den INIT-Wert -350 statt des gefuehrten Feldes. Praktisch klein.
  **Vorschlag**: fuer 0x21 das Feld direkt lesen oder OFFEN 3 berichtigen.

### H4 (hinweis) — Sektor-Kaesten: Vier-Quadranten-Pruefung (BAUPLAN O8 / Abnahme B2) fehlt
* mess2: bei Gier 512/1536 liegt die lange Achse (Hund 900) QUER zur Blickrichtung. Grund (byte-true): FUN_8002b5d0
  nimmt `ratan2(dz,dx) - +0x6a` (`8002b648 jal 0x80065de0` / `8002b650 lhu v1,106(s3)` / `8002b658 subu`); ratan2 =
  +atan2 (0x80065de0, Port `re15_ratan2`), die Blickrichtung ist (cos g, -sin g) (FUN_800245d8, Port
  `re15_dog_advance`). Bei 0/1024/2048/3072 fallen beide zusammen, bei Diagonalen nicht — Original-Eigenheit, vom Port
  treu nachgebildet (rsin 0x800683e8 / rcos 0x80068348 wie im Original zugeordnet). Sonde 43 prueft nur Gier 0/1024
  und kann das nicht unterscheiden. Die Radien-Grenzen sind mit +-50 statt +-1 (Abnahme B2) geprueft.
  **Vorschlag**: Pruefung bei Gier 512 (laengs 0 / quer 1) als Pin; O8 im Dossier schliessen.

### H5 (hinweis) — ungepinnte Details (eigene Mutationen gruen), Code gegen Disasm korrekt
* G02 (S1*S2-Term FUN_80041EF8), G05 (DoT-Gier-Zucken @0x80101e10-50 / @0x80101ea8-c0), G08/G09 (0x29 Luft-Spur:
  SE 1 bei Bild 0x13 @0x80115460-74, Ausgang HP < 50 @0x80114c58), G10 (0x2b Crossfade 0 @0x80114d80), G11 (0x23
  +0x1ba = -(+0x82*1800) @0x8010eaf8-b18), G15 (Hund Flag-Tor 0x4A @0x8010473c-40; Sonde 123 akzeptiert FX7 0 UND 1).
  **Vorschlag**: je eine gezielte Pruefung (bei G15 den Zufallsteil ueber den Strom festlegen, FX7 exakt pruefen).

### H6 (hinweis) — Treppen-Sonde umgeht re15_game_step
* Pruefung 201 tickt `re15_stair_tick` direkt. Dass der Normal-Zweig mit `pl->hit_react = 0` (game_step_common.c:1520,
  @0x80031964) waehrend der Treppe NICHT laeuft, belegt erst die Code-Lesung (Treppen-Zweig :1383-1391 ist ein eigener
  `else if`; `re15_stair_try_start` :2051 laeuft NACH dem Nullen im selben Bild). Verhalten korrekt, aber ungepinnt.
  **Vorschlag**: Pruefung 201 ueber `re15_game_step` fahren.

### H7 (hinweis) — Kakerlaken-Spurtabellen STAGE4/5 nachtragen
* Das Dossier belegt nur STAGE3. Eigene Lesung: STAGE4 HURT @0x80119fb4 / DEATH @0x8011a00c, STAGE5 HURT @0x8011fb1c /
  DEATH @0x8011fb74 — dieselbe Belegung; die Port-Tabelle gilt fuer alle drei Stages. **Vorschlag**: Adressen in den
  Kommentar bei `s_spur_hurt` / `s_spur_death` und ins Dossier.

### H8 (hinweis) — Spieler-Unterzustand 13: i-Frame-Setzer ohne Port-Zuordnung
* BAUPLAN §2 ordnet "Treppen-/Kletter-i-Frames" den Modus-1-Unterzustaenden 9..13 zu; gebaut sind 11/12 (B10), 9/10
  bestanden schon (climb_common.c:406/421/471/534). Unterzustand 13 (Dispatch 0x80073fb0[13] = 0x80038EF4, Tick
  0x80073ff0[13] = 0x80038EFC; Clip 0x13 `80038f90 ori v0,zero,0x13` / `80038f98 sb v0,-13592(at)`) setzt Bit 0
  `80038fc4 ori v0,v0,0x1` / `80038fcc sb v0,-13593(at)` und loescht es @0x80039078 — im Port finde ich keinen
  Zwilling (kein Treffer auf 80038fcc/80039078). **Vorschlag**: Aktion bestimmen; falls der Port sie fuehrt, i-Frame
  nachziehen, sonst im Dossier als nicht portierte Aktion benennen.

---

## 6. Volle Suite / Integrationstests

* Eigener Lauf aller 31 Integrationstests (`ctest -R integration -j 1`, 04:15-04:26): 23/31 gruen, rot
  `r30_cut_blitz`, `r30_granate_laden`, `r30_irons_tisch_bild`, `r30_irons_tisch_licht`, `r30_sicherung_laden`,
  `r30_titel_puls`, `r33_speichern`, `relatch_pin` — dieselbe Klasse wie beim Bauer: die exe endet mit exit 1
  an wechselnden Stellen (z.B. `r30_irons_tisch_bild`: debug.log endet nach `[pad] kein Controller gefunden`;
  `r30_cut_blitz` Lauf 78409a4a_b endet bei F90, Lauf c6297273_c nach 5 Zeilen, Laeufe a/b desselben Tests
  erreichen `EXIT_AT`).
* **Ursache (nicht Spur B)**: exit 1 ohne Absturzcode ist die Signatur von `taskkill /F`. `tools/local_build.sh`
  setzt `CLEAN_PATH` ohne `WindowsPowerShell` (local_build.sh:147), `command -v powershell` schlaegt fehl und der
  Rueckfall `taskkill //F //IM re15_pc.exe` (local_build.sh:298-299) beendet bei JEDEM `build` irgendeiner Sitzung
  alle re15_pc.exe der Maschine. Spur C hat das bereits gemessen (Kommentar in
  `r34g_c/re15_port/tests/unit/probes/r34_plattform.cmake:84-89`, bau_c.md NACHBESSERUNG M2). Waehrend meiner Laeufe
  fuhren die Baeume r34g_a und r34g_c ihre vollen Suiten. **Nebenwirkung meiner Pruefung**: meine zwei
  `local_build.sh build` (~03:54 und nach den Mutationsproben ~04:14) haben ueber denselben Rueckfall ebenfalls
  alle re15_pc.exe der Maschine beendet — parallele exe-Laeufe anderer Baeume koennen dadurch rot geworden sein.
* Wiederholung der 8 roten Tests einzeln (`ctest --repeat until-pass:4 -j 1`, 04:27-04:40): gruen `r30_granate_laden`
  (1. Wiederholung), `r30_irons_tisch_bild`, `r33_speichern` (3. Versuch), `relatch_pin`; nach 4 Versuchen weiter rot
  `r30_cut_blitz`, `r30_irons_tisch_licht`, `r30_sicherung_laden`, `r30_titel_puls`. `Testing/Temporary/LastTest.log`
  des Wiederholungslaufs: 18x `exit=1`, 4x `exit=0`, KEIN Absturzcode (0xc0000005); die debug.log der roten Laeufe
  enden ohne Fehlermeldung (titel_puls nach 5 Zeilen, sicherung lauf_76025d40_a bei F150).
* Keiner der roten Tests beruehrt Schaden/Gegnerreaktion; alle 399 Nicht-Integrationstests sind gruen. Eine
  `LOCAL-BUILD-OK`-Zeile ist unter dieser Last nicht zu erreichen — wie beim Bauer. INTEGRATIONSWUNSCH (Integration,
  `tools/local_build.sh:147`): `/c/Windows/System32/WindowsPowerShell/v1.0` in `CLEAN_PATH` aufnehmen, damit der
  gezielte Kill greift; ohne das ist keine GUI-Suite unter Parallel-Last aussagekraeftig.

---

## 7. Dateibesitz, Vertraege, Vollstaendigkeit

* **Dateibesitz eingehalten**: `git diff --stat 8d8651e4..6d806abd` = 14 Dateien, alle in der B-Liste von BAUPLAN
  §3.0 (re15_damage.c, re15_actor.h, re15_ai_flavor.h, enemy_ai_re2_zombie/dog/spider.c, enemy_ai_common.c,
  enemy_ai_boss_g5.c, stair_common.c, RE15_FUN_CATALOG.md, probe_r34_schaden.c, probe_r34_reaktion.c,
  probes/r34_schaden.cmake, bau_b.md). `re15_damage.h` (V2b-Signatur) unveraendert, `local_build.sh`/RE15_MIN_TESTS
  unveraendert. Keine fremde Datei beruehrt.
* **Commit-Messages**: alle 13 mit `Co-Authored-By`; die elf Bau-Commits tragen die @0x-Adressen ihrer Konstanten
  (3..37 je Commit); die zwei Doku-Commits haben keine Konstanten.
* **Konstanten ohne Adresse im Code**: keine tragende gefunden. Rueckfallwert `fc = 40` in `re15_birkin_fc`
  (enemy_ai_common.c) ist die bestehende Port-Sicherung von `re15_birkin_anim` (nur ohne geladene Bank).
* **BAUPLAN §3.2 B1-B12**: alle gebaut. Offen/abweichend: die Abnahme-Punkte "+-1 an R" (B2) und O8 (vier
  Quadranten) — s. H4; die Gegnerreaktion der RE2-Zombies auf die Handgranate weicht vom erwarteten "Tod" ab — s. K1.
* **Auftrag** ("Gegner Reaktion, korrekter Animation, Schaden etc. mit allen"): Schaden und Reaktion sind fuer alle
  Typen verdrahtet und haengerfrei (Zensus 352 Laeufe; Pruefungen 210/211 in jedem meiner Sondenlaeufe gruen,
  Quelltext des Zensus gelesen). Sichtbar fehlen
  weiter die Part-Farben/-Fluege (O-VB3/O7, Spur C/D) und die Applier-Bindung (INTEGRATIONSWUNSCH 2 des Bauers).
  Eine Sichtpruefung am echten Programm ist fuer B allein nicht moeglich (Granate fliegt erst mit A, zeichnet erst
  mit C) und wurde nicht gemacht.

---

## 8. Fazit

Der Bau ist handwerklich sauber und in den tragenden Konstanten korrekt: jede von mir nachgelesene Adresse und jedes
Byte stimmt (Gate B, FUN_8002b498, alle Kaesten, FUN_800470C0 samt FUN_80041EF8 inkl. der S1/S2-Stapelreste,
FUN_800154AC/FUN_80036E30, alle GL-Records, Zombie-DoT/Ausblender, Hund, Spinne, 0x27/0x29/0x2b/0x23-Spuren und
-Todesablaeufe, G5-Tabelle/Zerfall/Treffer, Treppe). Dateibesitz eingehalten, 399/399 Unit-Tests gruen, die
roten GUI-Tests sind extern verursacht (local_build.sh-Rueckfall-Kill). ABER: (K1) die Handgranate toetet im
Vorgabe-Flavor RE2 keinen stehenden Zombie, er steht in 24/24 Messlaeufen als Kriecher mit HP 10 wieder auf —
Folge der BAUPLAN-Wahl Klammer 0 fuer die Spalte (E6), abweichend vom erwarteten "Tod" und von RE2s eigener
GL-Explosion (Klammer 1 -> 0x80108530); (M1) das Bodenfeuer trifft RE1.5-Import-Zombies mit 15 statt 50 und reisst
nach 7 Treffern ein Bein ab (O-VB4 x E4 x Import-Bruecke, Dossier-Aussage OFFEN 13 falsch); (M2) die E6-Wache gegen
den zweiten Stempel ist ungepinnt. Vor der Integration braucht K1 eine Entscheidung, M1 eine Korrektur oder
dokumentierte Entscheidung mit Sonde.
