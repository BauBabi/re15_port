# Runde 34 (Granaten) — Bau B: Schaden und Gegnerreaktion aller Typen

Stand: 2026-09-29, Zweig `r34g/b-schaden`, Arbeitsbaum `.claude/worktrees/r34g_b`, Basis `r34g/c0-vertrag` (8d8651e4).
Auftrag: BAUPLAN §3.2 B1-B12, E4-E7, E13, E16, K4-K7, P14-P27; O-VB4 vom Orchestrator entschieden
(Bodenfeuer an Gegnern OHNE RE2-KI = RE1.5-Angriffsart 5 "Flaechenfeuer", 50 @0x8006f422 / Reaktion 14 @0x8006f435).
Bauverzeichnis `re15_port/build_r34_b`; Laufzeit-Ausgaben `build/r34g_b/` (unversioniert).

STATUS: B1-B12 GEBAUT + NACHBESSERUNG K1/M1/M2 (Abschnitt am Dateiende; unit_r34_schaden 47, unit_r34_reaktion 69
Pruefungen, Mutationsproben MN1-MN13; volle Suite `=== LOCAL-BUILD-OK (all) — Tests 430/430`, Stand 16a8d90e). Stand vor der Nachbesserung: alle Sonden gruen (unit_r34_schaden 39, unit_r34_reaktion 63 Pruefungen, Mutationsproben M1-M51);
volle Suite: 5 Laeufe je 426-429/430, rot nur GUI-Integrationen mit exe-Abbruch unter Parallel-Last, alle einzeln gruen
(Abschnitt "Volle Suite").

---

## B1 — Resolver-Tore (`engine/src/re15_damage.c` `re15_resolve_attack`)

### Gebaut
| Datei:Zeile | Inhalt | Beleg |
|---|---|---|
| `re15_damage.c` Kandidatenschleife (Pass 1) | NPC 0x40..0x4D mit HP < 0 = kein Kandidat (E7) | RE2 Gate 3 `lh v0,342(s0)` / `bltz v0,0x8004740c` @0x80047148-50; RE2-NPC HP -1 @0x8005d7b4-b8; RE1.5-NPC-INIT HP -1 `addiu v0,zero,-1` @0x8011d320 / `sh v0,154(v1)` @0x8011d324 (STAGE1, selbst nachgelesen) |
| `re15_damage.c` Anwendungsschleife | Gate B `(hit_react & 3) == 3 -> continue` VOR `&= 1`, nicht gezaehlt, +0x93 unberuehrt | `lw v0,144(s1)` @0x80012f54 / `lui v1,0x300` @0x80012f58 / `and` @0x80012f5c / `beq v0,v1,0x8001302c` @0x80012f60; Sprungziel liegt hinter `addiu s4,s4,1` @0x80013024 (selbst nachgelesen, `re15_disasm.py dis 0x80012f24 68`) |
| `re15_damage.c` `re15_enemy_take_damage_at` (neu, static) | Angriffspunkt P aus dem Resolver an den Gegnerzweig (fuer den RE2-Stempel B3); `re15_enemy_take_damage(e,t)` bleibt die oeffentliche Schnittstelle mit P = NULL | FUN_80012d60 a1 = s5 (`lw a1,0(s5)` / `lw a2,8(s5)` @0x80012f8c-90) |
| Kommentar "GATE B ... inert/OMITTED" | ersetzt durch die Belegkette | P14 |

### Sonde `unit_r34_schaden` Teil `tore` (Pruefungen 11-18)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 11 | Typ 0x27 mit +0x93 = 0x83, P 300 daneben: HP 180 bleibt, +0x93 bleibt 0x83, Rueckgabe 0 | gruen |
| 18 | +0x93 = 0x03 (Bit 0x80 frei, P hinter der Figur): bleibt 0x03 (Gate B laesst Bit 0x80 stehen) | gruen |
| 12 | NEGATIV: nur Bit 0 -> Kandidat, `|= 2`, kein Schaden, gezaehlt (1) | gruen |
| 13 | NEGATIV: frei -> HP 180 - 1000, Zustand 3, +0x5 = 9, +0x6 = 1 | gruen |
| 14 | NPC 0x45 HP -1 300 neben P: nichts aendert sich, Rueckgabe 0 | gruen |
| 15 | NEGATIV: NPC 0x45 mit HP 0 wird getroffen (Zustand 3) — die HP-Kennung schliesst aus, nicht der Kasten | gruen |
| 16 | Ivy 0x2d (+0x93 = 3): immun | gruen |
| 17 | FX-Emitter 0x24 (+0x93 = 1): 1. Treffer nur `|= 2`, 2. Treffer Gate B | ROT bis B2 (der Port gibt 0x24 heute gar keinen Kasten; das Original hat die Spawn-Voreinstellung {0,0,0,1,1,1} @0x80072be0 -> mit Hoehe 0 liegt P.y = -500 genau auf der strengen Bandgrenze) |

### Mutationsproben
| Mutation | Erwartung | Ergebnis |
|---|---|---|
| Gate-B-Zeile durch `if (0) continue;` ersetzt | 11 rot | 11 rot (`n=1` statt 0; +0x93 zufaellig wieder 0x83, deshalb Pruefung 18 dazu), zurueckgesetzt |
| NPC-Zeile durch `if (0) continue;` ersetzt | 14 rot | 14 rot (`hp=-1001 state=3`), zurueckgesetzt |

---

## B4 — `re15_re2_gl_apply` = FUN_800470C0-Zwilling (Vertrag V2b) und B3 — Explosions-Stempel

Beide Pakete teilen sich den Stempel und sind deshalb EIN Commit.

### Selbst disassembliert (in dieser Sitzung)
* FUN_800470C0 komplett, 420 Instruktionen (`re2_disasm.py dis 0x800470c0 420`, Mitschnitt `build/r34g_b/re2_800470c0.dis`).
  Alle Adressen im Code-Kopf `re15_damage.c` "Runde 34 B3/B4 — DER RE2-GL-APPLIER".
* FUN_80041EF8 (Kastentest) komplett + die GTE-Helfer RotMatrix 0x8008e1f4 (Port-Zwilling `re15_door_rotmatrix`),
  0x8002ce94 (Rotation je Spalte MVMVA `4a49e012`, Translation MVMVA sf=1 cv=TR `4a480012` @0x8002cfbc, Ablage MAC
  `swc2` @0x8002cfc4-cc), 0x8008dba4 (ApplyMatrixSV, MVMVA `4a486012` lm=0 @0x8008dbd8), 0x8008d6c4 (MulMatrix0, je
  Spalte MVMVA). **Neu gefunden:** zwei Stapel-Reste fliessen in die Matrizen: m0[0][1] = Oberhalbwort von
  `sw t1,64(sp)` @0x80042040 (= e1.x < 0 ? -1 : 0), m1[1][0] = Oberhalbwort von `sw v1,100(sp)` @0x800420d4 (= d2.x < 0
  ? -1 : 0) -> m2[0][0] bekommt `+ S1*S2` (0 oder 1) vor dem `>> 12`. Im Port nachgebaut (`re2gl_box_test`).
* FUN_80036E30 (+0x84/+0x8C): `+0x84 = +0x38 + (s16)+0xA0`, `+0x8C = +0x40 + (s16)+0xA2`, +0xA0/+0xA2 = +0x94/+0x96,
  bei `Gier & 0x400` VERTAUSCHT (@0x80036e34-80); Aufrufer (roher `jal`-Wort-Scan) EXE @0x80036030, @0x8003631c,
  @0x8003ee70, @0x8004fc98, @0x800554d8.
* FUN_800154AC (Peilung): s16-Kuerzung aller vier Argumente (@0x800154b4-d8), dx == 0 -> 1024/3072, sonst
  `(dx >= 0 ? 4096 : 2048) - catan(...)`. Port-Zwilling `re15_atan2_q12(dz,dx) - 1024`; catan RE2 @0x8008d190 und RE1.5
  @0x800658fc sind dieselbe CORDIC-Routine (Registerbelegung verschieden), Tabellen bytegleich (RE2 @0x800adb4c =
  RE1.5 @0x80078c90 = {511,302,159,81,41,20,10,5,3,1,0,0}).
* Records (w0, w1) der Zeilen 9/10/11 fuer Zombie/0x16/Hund/Kraehe/Spinne/Baby/Arm/G5 per Skript direkt aus
  `info/re2leon/PSX.EXE` gelesen (Zeiger *(0x800A6A88 + Typ*4)); Werte = K6 (Klammer 0/1/2 und Sperre 15).
* Je-Typ-Eingaben (+0x1EE, +0x94/+0x96, +0x98/+0x9E) per Store-Scan `build/r34g_b/feldscan.py` ueber EMZ0,
  EMD0G_MOD0, EMOVL21_S0, EMS25, EMS26 und CDEMD0_EM2D_ai1 (Tabelle mit Adressen im Code-Kommentar).
* +0x1FC: Lade-Scan (Immediate 508, `build/r34g_b/ladescan.py`) ueber EMZ0/EMD0G_MOD0/EMOVL21_S0/EMS25/EMS26: 0 Leser
  -> nicht gefuehrt (verhaltensneutral).

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `re15_damage.c` Block "Runde 34 B3/B4" (Dateiende) | Records, Je-Typ-Tabelle `re2_gl_typ`, Kastentest `re2gl_box_test`, Peilung, Richtungsbits, gemeinsamer Stempel `re2_gl_stempel`, Explosions-Stempel `re2_gl_explosion_stempel` (E6), Applier `re15_re2_gl_apply` |
| `re15_damage.c` `re15_resolver_gegnerzweig` (neu, static) | Gegnerzweig von FUN_80012d60 ab Gate B, geteilt von Resolver und Applier (O-VB4) |
| `re15_damage.c` `re15_enemy_take_damage_at` | E4 (Modellwert `re15_enemy_dmg_row(e)[DAT_8006f430[Art]]` fuer Art >= 2 bei Modell-/Import-Typen), E16 (G5 5090/5091: 80/70/70), E6-Stempel mit P, Zerleger-Peilquelle = P |
| `re15_damage.c` `re15_re2_stamp_hit` | loescht `re2_gl_stamp` (Nicht-GL-Stempel) |
| `include/re15_actor.h` | Port-Feld `re2_gl_stamp` |
| `enemy_ai_re2_zombie.c` `re2z_hurt` | kein zweiter Stempel (Spielerpeilung + Reserve) nach einem GL-Treffer |
| `enemy_ai_re2_zombie.c` `re2z_row_from_atktype` | [2..4] = 9/11/10 statt 17 (P18) |

### Port-Zuordnungen (gekennzeichnet)
* **[NACHBESSERUNG M1: Typen unter dem RE2-Schadens-/HP-Modell (Import-Zombies) bekommen statt 50 den RE2-Record-Wert
  des Hitcodes (Zombie Z10 K2 = 5 @0x800A41E0); die E4-Modellwahl gilt nur Art 2..4. Vorher gab der Code 15 — s. Dateiende.]**
* **O-VB4 (Orchestrator):** RE1.5-KI-Kandidaten des Appliers laufen durch den RE1.5-Gegnerzweig mit Art 5
  (50 @0x8006f422 / Reaktion 14 @0x8006f435); ihr Band/Radius/Mittelpunkt ist der eigene RE1.5-Kasten (Mitte =
  Lage + Versatz, Halbhoehe = Kastenhoehe, Radius = `re15_ellipse_radius` zum Punkt). Gate B -> kein Treffer.
* **E6 +0x5 fuer Hund/Kraehe/Spinne/Baby:** die Port-Gehirne lesen +0x5 als RE1.5-Waffen-Id und uebersetzen selbst
  (`s_re2d_row_von_waffe`, `s_re2c_row_from_weapon`, `s_re2s_row_from_weapon`: w10 -> 11, w11 -> 10). Der Stempel
  schreibt deshalb fuer sie die RE1.5-Waffen-Id (Zeile 10 -> 11, 11 -> 10); deren Uebersetzung ergibt GENAU die
  RE2-Zeile des Originals. Die Zombie-Familie traegt die RE2-Zeile direkt. (Wortlaut E6 "+0x5 = RE2-Zeile" ->
  gleiche Zelle, gleiches Verhalten.)
* Kopf-Bit (Wort0 & 0x10000000) fuer Hund/Kraehe/Spinne/Baby = 0: deren Port-Gehirne lesen +0x1D2 nur als /3 ->
  verhaltensneutral (OFFEN: nicht gescannt).
* Kraehe: +0x98/+0x9E = INIT-Werte -350/530 (der Port fuehrt das Feld fuer die Kraehe nicht; der Flug schaltet
  +0x98 auf 0/-350 @0x80100204/08) — OFFEN.
* Zeilen ausser 9/10/11 sind nicht hinterlegt (einziger Aufrufer Op 40 = Zeile 10): Treffer ohne Anwendung.
* Trefferpause -> `hit_react |= 1` im Applier (Port-Buchfuehrung wie im Schusspfad; die Filter
  `re15_re2_pause_filter_apply` / `re15_re2z_hit_filter_apply` geben Bit 0 frei, sobald +0x1D3 & 0x7F == 0).

### Sonde `unit_r34_schaden` Teile `applier` (21-29, 91-94) und `stempel` (31-39)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 21 | Zombie Zeile 10 Kl. 2: HP 80 -> 75, Zustand 2, +0x5 10, +0x1D2 6 (Zone 0), +0x1D3 15, +0x1D0 Bit 0, +0x6 0, Rueckgabe Slot+1 | gruen |
| 22 | Zonen-Reserven 13/13/13 unberuehrt, `re2_gl_stamp` = 1 | gruen |
| 23 | nur der ERSTE Gegner (zwei Zombies im Kasten) | gruen |
| 24 | Gate 2 (+0x1D3 = 5) -> uebersprungen, der naechste getroffen | gruen |
| 25 | Gate 3 (HP -1) und Gate 4 (+0x10E 0x4000) | gruen |
| 26 | Leerliste -> 0 | gruen |
| 27 | Band-Grenzen P.y 100/101/-3099/-3100 -> 1/0/1/0 | gruen |
| 28 | Kasten-Grenzen x 1099/1100/-600/-601 und z 1099/1100/-1100/-1101 -> 1/0/1/0 | gruen |
| 29 | Gier 1024 dreht den Kasten (+ Kontrolle Gier 0) | gruen |
| 91 | Radius-Erweiterung bleibt im Puffer: Modus ALLE trifft B bei x 1500 nur nach A; ohne A nicht; Einzelmodus prueft B nie | gruen |
| 92 | Richtung aus P: 0x61 (P in -z) / 0x81 (P in +z) | gruen |
| 93 | RE1.5-KI (Made 0x27): Art 5 -> HP 180 -> 130, +0x5 14, +0x6 1, +0x93 Bit 0 | gruen |
| 94 | Gate B im Applier: Made +0x93 = 3 -> kein Treffer, der Zombie dahinter wird getroffen | gruen |
| 31 | Explosion RE2-Zombie HP 80: -120, Zustand 3, +0x5 9, +0x6 0, +0x1D2 ~~0~~ **3** (NACHBESSERUNG K1), +0x1D3 15, gl_stamp, Reserve 13 | gruen |
| 32 | Richtung aus P (0x21 / 0x01) | gruen |
| 33 | Saeure -> Zeile 11, Brand -> Zeile 10 | gruen |
| 34 | Brad 0x11 HP 250 -> 50, HURT, Zeile 9, Spalte ~~0~~ **3** (NACHBESSERUNG K1) | gruen |
| 35 | 0x16: HE 80 / Saeure 200 / Brand 80 | gruen |
| 36 | Hund RE2 Saeure: 300, +0x5 = 10 (Waffen-Id), +0x1D3 15, +0x6 0 | gruen |
| 39 | Direktaufruf ohne Punkt: atktype 2/3/4 -> Zeile 9/11/10 | gruen |
| 37 | NEGATIV RE1.5 ohne Import: 1000 flach, kein RE2-Stempel | gruen |
| 38 | NEGATIV RE1.5 mit Import: 200 (E4), kein RE2-Stempel | gruen |

### Mutationsproben (`build/r34g_b/mut.sh`: anwenden, bauen, laufen, zuruecksetzen, neu bauen; MUTATION-Reste 0)
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M1 Band `<` -> `<=` | 27 | 27 rot (-3100 -> 1) |
| M2 Radius-Erweiterung auch nach Treffer zurueck | 91 | 91 rot |
| M3 Gate 2 weg | 24 | 24 rot |
| M4 Klammer ignoriert (`10*k` -> 0) | 21 | 21/23/24/91/94 rot |
| M5 Zonen-Vergleich umgedreht | 21 | 21 rot (+0x1D2 7) |
| M6 Richtungsbit 0x20 -> 0x80 | 92 | 92 rot |
| M7 Explosionszeile ohne 10/11-Tausch | 33 | 33/36 rot |
| M8 E4-Modellschaden weg | 31 | 31/34/35/36/38 rot |
| M9 O-VB4 Art 5 -> Art 2 | 93 | 93 rot |
| M10 `re2z_row_from_atktype[2]` zurueck auf 17 | 39 | 39 rot (8 = Rueckfallzeile) |

---

## B2 — Kasten-Versatz FUN_8002b498 + Kaesten je Typ

### Selbst disassembliert / gelesen
* FUN_8002b498 (`re15_disasm.py dis 0x8002b498 60`): SVECTOR (0, +0x6a, 0) -> RotMatrix 0x80068098 (@0x8002b4d4) ->
  ApplyMatrix 0x800661c0 (MVMVA sf=1 `4a486012` @0x800661f4, MAC -> VECTOR) auf (Kasten.x, [Gier], Kasten.z) ->
  +0x7c[0]/[2] = gedrehtes x/z (@0x8002b504/@0x8002b51c), +0x7c[1] = Kasten.y ungedreht (@0x8002b508-10).
* RotMatrix-Tabelle RE1.5 @0x800794c4 (`lw t9,-27452(t9)` @0x800680c0) mit RE2 rcossin @0x800adeac verglichen:
  16384 Byte bytegleich -> der vorhandene RE2-Zwilling `re15_door_rotmatrix` ist auch der RE1.5-RotMatrix-Zwilling.
* Gier-Konvention: Port-`rot_y` der RE1.5-Gegner = rohes +0x6a (z.B. `e->rot_y = atan2_q12(...) - 0x400`
  enemy_ai_common.c:3445 = FUN_8001a6d4-Rohwert; `hit_from_front` nutzt dieselbe Konvention).
* Kaesten, Bytes selbst gelesen: Hund @0x80120f64 `00 00 30 fd 00 00 84 03 d0 02 c2 01` (INIT `lw v0,3952(v0)`
  @0x8010da68 / `sw v0,120(v1)` @0x8010da70); Alligator @0x80118b98 `e8 03 30 fd 00 00 98 08 d0 02 20 03` (STAGE2);
  Feuer 0x26 @0x80121258 `00 00 00 00 00 00 58 02 d0 02 58 02`; NPC 0x45 @0x80121728 `00 00 60 fa 00 00 f4 01 a0 05 f4 01`;
  NPC 0x4b @0x801218c8 `00 00 60 fa 00 00 2c 01 a0 05 2c 01`; Voreinstellung @0x80072be0 `00 00 00 00 00 00 01 00 01 00
  01 00`; Tentakel em37 HP -1 `addiu v0,zero,-1` @0x80100530 / `sh v0,342(s1)` @0x80100534.

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `re15_damage.c` `re15_hitbox_versatz` (neu) | FUN_8002b498-Zwilling: gedrehter x/z-Versatz aus dem lokalen Kasten (hit_offset_x/z) |
| `re15_damage.c` `re15_hitbox_test` | Mitte ueber `re15_hitbox_versatz`; Spawn-Voreinstellung {0,0,0,1,1,1} fuer kastenlose Typen NUR im Resolver-Test (Koerper-Schub/Zielhilfe bleiben bei "kein Kasten", Audit wf_efd92a2c ivy #77) |
| `re15_damage.c` `re15_resolver_kasten` (neu) | Kasten des FUN_8002b5d0-Zwillings: Aktor-Kasten gedreht nach FUN_8002b498; fuer den HUND der byte-gelesene Sektor-Kasten {0,-720,0,900,720,450} @0x80120f64 |
| `re15_damage.c` `re15_enemy_apply_hitbox` | Alligator Versatz x 1000, Feuer 0x26 Versatz y 0, NPC 0x45 500/1440, NPC 0x4b 300/1440 (Hund bleibt dort 500/600, s. unten) |
| `re15_damage.c` Resolver Pass 1 | G5-Tentakel 0x37 mit HP < 0 kein Kandidat (RE2 Gate 3, sonst traefe ihn die Voreinstellung) |
| `re15_damage.c` `re15_re2_gl_apply` | RE1.5-Kandidat: derselbe Resolver-Kasten (`re15_resolver_kasten` + Voreinstellung) |

**Messung, die den Hunde-Kasten auf den Resolver beschraenkt:** Mit dem Original-Kasten 900/720/450 auf dem Aktor
(also auch im Koerper-Schub FUN_8002aec4 und im Schusspfad) wurde `unit_r27_hund_wandtrieb` rot: **469 Bilder** mit
dem Spieler in einer soliden Zelle (ROOM11D0, 8 Plaetze, 71 Bisse; 2 Plaetze betroffen). Der Koerper-Schub ist nicht
Gegenstand dieser Runde -> der Hund behaelt dort 500/600, der Resolver nimmt den byte-gelesenen Kasten (PORT-ZUORDNUNG,
benannt; OFFEN: Koerper-Schub des Hundes mit dem Original-Kasten, eigene Runde). Danach `unit_r27_hund_wandtrieb`
wieder gruen.

### Sonde `unit_r34_schaden` Teil `kasten` (41-49)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 41 | Alligator Gier 0: laengs +X 3650 trifft / 3750 nicht; Gier 1024: laengs -Z -3650 / -3750; der ungedrehte Punkt (3650,0) trifft bei Gier 1024 nicht | gruen |
| 42 | Alligator-Hoehenband exakt 499/500/-1939/-1940 -> 1/0/1/0 | gruen |
| 43 | Hund-Sektor: laengs 1350/1450, quer 900/1000, Gier 1024 tauscht die Achsen | gruen |
| 44 | Hund-Band 499/500 | gruen |
| 45 | Feuer 0x26 Band um y 0: P.y 1000 trifft, -1300 nicht | gruen |
| 46 | NPC 0x45 950/1050, 0x4b 750/850 (mit HP >= 0) | gruen |
| 47 | Voreinstellung (FX 0x24): 400 trifft, 600 nicht | gruen |
| 48 | Tentakel 0x37 HP -1: kein Kandidat | gruen |
| 49 | NEGATIV/Regression: Zombie 850/950 und Gier 1024 ohne Wirkung | gruen |
| 17 | (aus B1) FX 0x24: 1. Treffer `|= 2`, 2. Treffer Gate B | jetzt gruen |

Sqrt-Grenzen mit Abstand 50 (SquareRoot0 ist eine Tabellen-Naeherung, `re15_squareroot0`), Hoehenband exakt.

### Mutationsproben
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M11 Versatz ungedreht (`re15_hitbox_versatz` kehrt sofort zurueck) | 41 | 41 rot |
| M12 Alligator ohne x-Versatz | 41 | 41 rot |
| M13 Hund-Resolver-Kasten zurueck auf 500/600 (`re15_resolver_kasten`) | 43 | 43 rot |
| M14 Feuer 0x26 wieder -720 | 45 | 45 rot |
| M15 Voreinstellung weg | 47 | 47 rot |
| M16 Tentakel-Ausschluss weg | 48 | 48 rot |

### Folge fuer andere Pfade (benannt)
NPC 0x45/0x4b: der Kasten wirkt auch auf deren Wandklemme (`lhu a1,6(v0)` @0x8011cc60 liest +0x78) und Koerper-Schub
— das ist das RE1.5-Original; `unit_npc_wall_clamp`, `unit_npc_back_walk`, `unit_1090_npc_prop_push` gruen. Alligator-
Versatz und Feuer-y wirken nur im Resolver bzw. im y-Band des Koerper-Schubs (Feuer). Regressionslauf aller
Schaden-/Hund-/Alligator-/NPC-/Treffer-Tests (44 Tests, `ctest -R ...`): gruen nach der Hunde-Beschraenkung.

---

## B5 — Zombie: Brand-/Saeure-DoT und Leichen-Ausblender (`enemy_ai_re2_zombie.c`)

### Selbst disassembliert (EMZ0.BIN)
* EXEC[1] @0x80101DC0-EC0 (66 Instr.), EXEC[2] @0x80102474-54C (58 Instr.), Wurzel @0x80100470-52c (Zaehler +0x236 nach
  dem Dispatch `jalr v0` @0x801004e8 und `jal 0x8010c2a4` @0x801004f0: `lhu v0,566` @0x801004F8 / `addiu 1` @0x80100504 /
  `sh v0,566` @0x80100508), Epiloge EXEC[1] @0x80101f64 / EXEC[2] @0x801025d0, Leichen-Schwanz @0x8010a80c-868.
  `srav s0,s0,v0` @0x80101e24 steht als Rohwort 0x00508007 (von Hand dekodiert: rd = s0, rt = s0, rs = v0).

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `include/re15_actor.h` | Feld `re2z_c236` (+0x236) |
| `enemy_ai_re2_zombie.c` `re2z_dot_tick` (neu) | DoT-Block: Gate verkohlt/geaetzt, 8er-Takt, zwei Wuerfe + Gier-Zucken (nur EXEC[1]), HP -1, Tod 0x0A03/0x0B03 + +0x1D2 = 4 + +0x1D3 \|= 0x80 + +0x21A \|= 0x2000, Saeure-Zucken jedes Bild |
| `re2z_exec_walk` (EXEC[1]) | Aufruf nach dem +0x15A-Block (dort stand die Fehldeutung "WALK edge-fall ... OPEN") |
| `re2z_exec_bump` (EXEC[2]) | Aufruf nach der Bewegung, vor der Ausstiegs-Leiter; Kopfkommentar "Kanten-Sturz/Jitter" korrigiert |
| `re15_re2z_tick` | `re2z_c236++` nach dem Zustands-Dispatch |
| `re2z_init` | `re2z_c236 = 0` (@0x801008AC); INIT-Kommentar korrigiert |
| `re2z_corpse` Schwanz | Leichen-Farbausblender (nur verkohlt, jedes 4. Bild 15 Parts -0x010101, Stopp bei Part-0-R 16) |

### Sonde `unit_r34_reaktion` Teil `zombie` (ROOM1140, RE2-Flavor, echter `re15_game_step`, RE2-Baenke geladen)
Harness-Hilfe: Brad 0x11 wird aus der Fress-Pose in den Gang gesetzt (Zustand 0x101); Treffer, KI, Filter, HP-Stempel
und Anim laufen echt.
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 101/111 | Brand/Saeure-Explosion 300 neben Brad: HP 250 -> 50, Zustand 2, +0x5 10/11, Spalte 0 | gruen |
| 102/112 | Stagger-P0 setzt die Element-Bits: Brand +0x10E 0x80 + +0x21A 0x800; Saeure +0x21A 0x1800 | gruen (0x0080/0x8800; 0x9820) |
| 103/113 | DoT: 51 HP-Stufen, jede genau im 8er-Takt ((+0x236 & 7) == 1 nach dem Bild), Abstand ueber durchgehendes Gehen immer 8 | gruen (415/404 Gang-Bilder) |
| 104/114 | Tod am Element: +0x5 0x0A bzw. 0x0B, +0x1D2 4, +0x1D3 0x80, +0x21A 0x2000 | gruen (Bild 1289 / 1401) |
| 105 | Leichen-Ausblender: R von 64 auf 16 in 48 Stufen, je 4 Bilder | gruen |
| 106 | Stopp bei R = 16 | gruen |
| 107 | NEGATIV: gehender Brad ohne Element-Bits verliert in 400 Bildern keine HP | gruen |

### Mutationsproben
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M17 DoT im Gang aus | 103/104 | 103/104/105/106/113/114 rot |
| M18 Takt `& 7` -> `& 3` | 103 | 103/113 rot (Takt-Fehler 25/26) |
| M19 Ausblender aus | 105 | 105/106 rot |
| M20 Saeure-Todeswort 0x0B03 -> 0x0A03 | 114 | 114 rot |

### Hinweis (Wirkung ueber die Granate hinaus)
Der DoT gilt fuer JEDE Quelle der Element-Bits: auch der Flammenwerfer (RE1.5-Waffe 14 -> RE2-Zeile 16, MAIN-P0 setzt
`+0x21A |= 0x800` @0x80105560) laesst verkohlte Zombies jetzt im Gang ausbluten — das ist RE2-Retail.

---

## B6 — Hund RE2 (`enemy_ai_re2_dog.c`, EMD0G_MOD0.BIN)

### Selbst disassembliert
* Todes-Wurzel 0x801040DC: `lbu v0,5(a0)` @0x801040E4, Tabelle @0x801055CC (22 Worte selbst gelesen):
  [0] 0x801037E8, [1..4]/[10..13]/[15]/[16]/[18] 0x80104118, [5]/[6]/[9]/[17]/[19] 0x80104610, [7]/[8] 0x801042B0,
  [14] 0x801048B4, [20]/[21] 0x80104178. Unter-Router 0x80104118: +0x6 == 0 -> P0-Tabelle @0x80105618[+0x5]
  ([10]/[16] 0x80104774, [11] 0x8010481C, sonst Kern), sonst Phasen @0x80105668. Router 0x80104610: Phasen @0x80105688
  = {0x80104694, 0x801034C8, 0x80104200, 0x801037E8}, danach IMMER FX(3,0), FX(2,0), FX(2,1+(rand&1)) @0x80104644-7C.
  (Der alte Port-Kommentar ordnete Zeile 0 dem Gore-Router zu — die Wurzel schickt sie nach 0x801037E8; die Port-
  Uebersetzung bildet Zeile 0 nie.)
* P0 0x80104694 (Zeile 9 >= 3 -> nur Kern, `j 0x8010475c` ueberspringt das Budget), Teile-Wurf 0x80104440 (Tabelle
  @0x80105680 = `02 03 04 07 08 09 0a`), Brand 0x80104774, Saeure 0x8010481C, HURT 9/10/11 0x80103CE4/0x80103D9C/
  0x80103E60, Kern 0x80104178, HURT-P0 0x80103344 (`sb s0(=1),7(s2)` @0x801034a4 mit s2 = self+0x218 -> Budget 1, FX(0,0)),
  Spawner 0x80105070 (Budget-Tor @0x80105090-98, Abzug @0x8010518c-98). Effekt-Tabelle @0x801056AC hat 13 Eintraege
  (nicht 10): [10] `84 0f 00 00 00 0c` @0x801056E8, [11] `86 00 00 00 00 10`, [12] `84 0c 00 00 00 10`.

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `include/re15_actor.h` | `re2z_part_flags/_tint/_yaw98/_w9a/_w9c/_w9e` 16 -> 20 (Hund 17 Parts `sltiu 0x11` @0x801047f8, Spinne 20 @0x801060b4) |
| `re2d_death` P0 | Reihenfolge des Originals: Router-9-Zeilen {5,6,9,17,19} (@0x801055CC): +0x231 := 1, Kern, Teile-Wurf, Zeile 9: Budget 1 + FX 7 hinter dem Flag-Tor 0x4A (@0x8010473C-50), dann Budget 18 (@0x80104758). Zeilen 10/16: Kern, bei +0x1D2 < 3 oder Zeile 16 Budget 6, 6x FX 7, 17 Parts 0x00202020. Zeile 11: Kern, bei +0x1D2 < 3 17 Parts 0x00003F2F, Budget 2, FX 9 an rand&7, FX 10 an (rand&7)\|8 |
| `re2d_death` Phasen | Blut je Bild der Router-9-Zeilen nach der Phase (Zeile vor der Phase gelesen) |
| `re2d_kern` / `re2d_teile_wurf` / `re2d_router9_blut` (neu) | Kern ausgelagert; Teile-Wurf mit allen Feldern (+0x00 \|= 0x4A, +0xA0 0, +0x9C 800, +0x9A -150, +0x9E 10, +0xA4 -100, +0x70 0x00101040, +0x98 Gier) |
| `re2d_hurt` Zeile 9/10 | FX-8-Schleife `re2d_fx8_schleife` = ceil(n/2) (Zaehler waechst, Budget schrumpft, @0x80103d38-5c), +0x5 := 1 (@0x80103d64-68 / @0x80103e30) |
| `re2d_hurt` Zeile 11 | EIN Part rand&0xF +0x70 := 0x00003F2F (@0x80103ebc-c0), FX 9 an diesem Part, +0x5 := 1 (@0x80103edc) |
| `re2d_fx` | Testhaken `re15_re2dog_fx_zaehler` (Zaehlung hinter dem Budget-Tor, kein Verhalten); Kommentar zur Tabelle korrigiert |

Vorher-Defekt (belegt): der Port-Kern lief NACH dem Gore-Block; sein HURT-P0 setzt Budget 1 und verbraucht es mit FX(0,0)
— das Budget 18 war damit weg, das Blut je Bild fiel aus. Mutation M22 bildet genau das nach (rot).

### Sonde `unit_r34_reaktion` Teil `hund` (ROOM11D0 + Flag 3:152, RE2-Flavor, Bank EM020 VOR dem Spawn, echter `re15_game_step`)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 120 | HE-Explosion (Art 2) 300 neben dem Hund: Tod, +0x5 9, +0x1D2 < 3, +0x1D3 15 | gruen (HP 83 -> -217) |
| 121/122 | Teile-Flags 0x4A genau an {2,3,4,7,8,9,10}; Felder 800/-150/10/-100/0, Farbe 0x00101040, +0x98 = Gier | gruen |
| 123 | P0-Bild: Budget 15 (= 18 - 3 Router-Effekte), FX0 3, FX1/2 1, FX7 <= 1, KEIN SE 7 | gruen (FX7 1) |
| 124 | Blut je Bild: 15 -> 0 in 3er-Stufen, FX0 13, FX1/2 6 | gruen |
| 130-133 | Brand (Art 4 -> Waffe 11 -> Zeile 10): 17 Parts 0x00202020, 17..19 unberuehrt, FX7 6, Budget 0, SE 7 1x, kein Blut je Bild | gruen |
| 140-142 | Saeure (Art 3 -> Waffe 10 -> Zeile 11): 17 Parts 0x00003F2F, FX9 1, FX10 1, SE 7 1x | gruen |
| 150/151 | NEGATIV GL Zeile 10 Klammer 1 (+0x1D2 = 3): keine Farbe, kein FX 7, Schrei | gruen |
| 152 | NEGATIV GL Zeile 9 Klammer 1: nur Kern (Schrei), kein Teile-Wurf, kein Blut | gruen |
| 153 | Zeile 16 (Waffe 14) bei +0x1D2 = 3 faerbt trotzdem (`bne v1,16` @0x801047a0-a8) | gruen |
| 160-163 | HURT FX-8-Zahl n = 1/2/3/4 -> 1/1/2/2, FX0 1 (HURT-P0), +0x5 = 1, Bit 0x80 nur Zeile 10 | gruen |
| 164 | HURT 11: genau 1 Part 0x00003F2F, FX9 1, +0x5 1 | gruen |
| 165 | nach HURT 9: wieder treffbar in Bild 14 (Sperre 15 abgelaufen) | gruen |
| 166 | nach HURT 10: erst in Bild 71 wieder treffbar, Bit 0x80 stand bis dahin | gruen |

### Mutationsproben (alle mit `build/r34g_b/mut.sh`, MUTATION-Reste 0)
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M21 Budget 18 weg | 123/124 | 123/124 rot |
| M22 alte Reihenfolge (Kern-P0 nach Budget 18) | 123/124 | 123/124 rot |
| M23 Brand-Farbschleife 16 statt 17 | 131 | 131/153 rot |
| M24 FX-8-Schleife bis Budget 0 (= n Wuerfe, alter Port) | 161-163 | 161/162/163 rot |
| M25 HURT 9 ohne +0x5 := 1 | 160/161 | 160/161 rot |
| M26 Teile-Wurf ohne Flags | 121 | 121 rot |
| M27 Klammer-Tor der Brand-Zeile weg | 151 | 151 rot |
| M28 `\|\| Zeile 16` weg | 153 | 153 rot |
| M29 Blut je Bild weg | 124 | 124 rot |
| M30 HURT 10 ohne Bit 0x80 | 162/163/166 | 162/163/166 rot (erster Lauf mit Kommentar-Baufehler, wiederholt) |
| M31 HURT 11 ohne Part-Farbe | 164 | 164 rot |
| M32 Saeure-Farbe 0x3F2E | 141 | 141 rot |

Regression: alle 20 Hund-/Dog-/r34-Tests gruen (`ctest -R "dog|hund|re2doc|r34"`).

---

## B7 — Spinne RE2 (`enemy_ai_re2_spider.c`, EMS25.BIN)

### Selbst disassembliert
* FUN_8010609C @0x8010609C-C4: 20 Parts (`sltiu v0,a2,0x14` @0x801060b4), Stride 172, `sw a1,112(v0)` = Part +0x70.
* Zeile 10/16 0x80104A5C: Tor +0x6 == 0 und +0x224 == 0 (@0x80104a84-9c), Farbe 0x00202F2F @0x80104B44-4C.
* Zeile 11 0x80104B88: Tor wie oben (@0x80104ba4-bc); Part 19 (Record +3268): `sh 90,3426` (+0x9E) @0x80104bd0,
  `sh zero,3420/3422` (+0x98/+0x9A) @0x80104bdc/e0, `sb 100,3424/3425` (+0x9C/+0x9D) @0x80104be4/e8, Flags `ori 0x10`
  @0x80104bec / `sw` @0x80104bf4; Farbe 0x00101F3F (`lui a1,0x10` / `ori 0x1f3f` @0x80104bc0-c4, `jal` @0x80104bf0);
  FX(19,7), FX(0,6), FX(1,6), +0x239 := 1 (@0x80104c2c).
* Zeile 14 0x80104C5C: Farbe 0x003F3F3F @0x80104C88-90 (im Port unerreichbar, der Vollstaendigkeit halber gesetzt).
* Leser des Halbworts +0x9C: FUN_80028DAC `lhu v0,156(s0)` @0x80028e28 (RE2-PSX.EXE) -> die zwei Byte-Stores ergeben
  0x6464. Die Flug-Physik der Spinnen-Parts hat der Port nicht (Render OFFEN).

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `re2s_faerben` (neu) | FUN_8010609C-Zwilling (20 Parts) |
| `re2s_death_row1016` | Farbe 0x00202F2F statt OPEN-Kommentar; Kopf "BRAND" |
| `re2s_death_row11` | Part-19-Zustand + Farbe 0x00101F3F; Kopf korrigiert (Zeile 11 = SAEURE, nicht "verkohlt") |
| `re2s_death_row14` | Farbe 0x003F3F3F |

### Sonde `unit_r34_reaktion` Teil `spinne` (ROOM1140-Kontext, eine Spinne 0x25, Bank EM025 vor dem INIT)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 170 | Brand-Explosion (Art 4 -> Waffe 11 -> Zeile 10), HP 50: Tod, 20 Parts 0x00202F2F | gruen |
| 171 | Brand: Part 19 fliegt NICHT, +0x239 bleibt 0 | gruen |
| 172 | Saeure-Explosion (Art 3 -> Waffe 10 -> Zeile 11): Tod, 20 Parts 0x00101F3F | gruen |
| 173 | Part 19: Flags \|= 0x10, +0x9E 90, +0x98 0, +0x9A 0, +0x9C 0x6464, +0x239 1 | gruen |
| 174 | NEGATIV HE (Zeile 9): keine Farbe | gruen |

### Mutationsproben
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M33 FUN_8010609C 19 statt 20 Parts | 170/172 | 170/172 rot |
| M34 Part 19 ohne Flag 0x10 | 173 | 173 rot |
| M35 Brand-Farbe weg | 170 | 170 rot |

Regression `ctest -R "spider|spinne|r34|gore|re2_hit"`: 16/16 gruen.

---

## B8 — RE1.5-KI-Typen (`enemy_ai_common.c`)

### Selbst disassembliert / gelesen (re15_disasm.py)
* Spur-Tabellen (22 Worte je `read`): 0x27 HURT @0x801214a8 / DEATH @0x80121500 (STAGE1), 0x29 HURT @0x8011ed84 / DEATH
  @0x8011eddc (STAGE3) — beide Typen dieselbe Belegung: 0..6/12/14/19/20 Boden, 7/8/13 Luft, 9..11/15..18 Explosion, [21]
  HURT Luft / DEATH Boden. Dispatcher `lbu v0,5(v0)` @0x8011afe0/@0x8011b780 (0x27), @0x80114814/@0x80115038 (0x29).
* 0x29 (Datei `build/r34g_b/roach_hurt_death.dis`, 0x80114790..0x80115734): HURT-Zucken 0x8011484c (Clip 7, +0x93 \|= 2,
  SE 2, Ausgang Sub 7/5/9), HURT-Luft 0x80114a40 (Clip 8/9 nach 0x80, Ausgang Sub 3, bei HP < 50 Sub 7/5/9 @0x80114c58),
  HURT-Explosion 0x80114cb8 (Phasen `table 0x8010034c`: Clip 10/11, +0x8c = (rng&31)+80, +0x9c = 0, SE 2, Rueckstoss
  0x800 + bei 0x80 zusaetzlich 0, Aufstehen Clip 0x10/0x11, Ausgang Sub 4), DEATH-Boden 0x80115070 (Clip 0xe, SE 7, SE 1
  bei +0x95 == 0x3d), DEATH-Luft 0x80115280 (Clip 0xa/0xb, SE 7, SE 1 bei 0x13), DEATH-Explosion 0x801154b4 (Clip 10/11,
  rng, SE 7, Rueckstoss, Leiche). CORPSE 0x80115a6c-b60: KEIN `jal` (kein anim_set).
* 0x23 DEATH 0x8010e9e8/0x8010ea30 (STAGE2): alle 22 Zeilen -> 0x8010ea30; Clip 13, +0x93 \|= 2, Boden-Y
  -(+0x82*1800), Bild 20 -> Leiche (nur +0x1e0 == 0), ab Bild 21 0x8001c1a4(+0x8c, 0, -80, +0x1ba). CORPSE 0x8010eca4-ed98
  ohne `jal`. EM023 steht in KEINEM RE1.5-EMS (`re15_ems.c` s_ems_order) -> Clip-13-Laenge unbekannt (OFFEN, s.u.).
* 0x2b DEATH 0x80114cb0 (STAGE4, Phasen `table 0x80100344`): Ph.0 +0x93 \|= 2 @0x80114d04, Clip 8/9, Crossfade **0**
  (`sb zero,143` @0x80114d80), SE 2; Ph.1 bei +0x95 == 24 SE 7 UND Phase 2 (@0x80114ed4-efc); Ph.2 Clip 0xa/0xb nach
  **0x80** (@0x80114f34), Crossfade 7; Ph.4 Leiche. CORPSE 0x80115af0 ohne `jal`.
* Resolver-Riegel Bit 0 (fuer den Fresser): `andi v0,v1,0x1` @0x80012fbc / `ori v0,v1,0x2` @0x80012fc4 / `j 0x80013024`
  @0x80012fc8 / `sb v0,147(s1)` @0x80012fcc.

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `s_spur_hurt` / `s_spur_death` / `re15_spur` (neu, vor `re15_maggot_ballistic`) | Spur-Tabellen; Werte >= 22 -> Spur 0 (PORT-SICHERUNG) |
| 0x27 HURT/DEATH | Spurwahl ueber die Tabellen statt `< 7 / < 9 / sonst` (12/13/14/19/20/21 lagen in der Explosionsspur) |
| 0x29 `case 2` | drei HURT-Spuren komplett (Zucken, Luft, Explosion mit Rueckstoss + Aufstehen); Phase 0 faellt wie im Original in Phase 1 |
| 0x29 `case 3` | drei DEATH-Spuren (Boden/Luft/Explosion); SE-1-Bild NACH dem Vorschub; letztes Bild gehalten |
| 0x29 `case 7` | Leiche ohne Anim-Vorschub (vorher lief der Todesclip 90 Bilder lang neu) |
| 0x23 `case 3` | Todesablauf 0x8010ea30 statt sofortiger Leiche; `case 7` ohne Bildvorschub |
| 0x2b `case 3` | Ph.0 \|= 2, Crossfade 0, Bild-24-Uebergang, Ph.2-Clip nach 0x80, Ph.3/Ph.4 getrennt; `re15_birkin_fc` (neu) haelt das letzte Bild; `case 7` ohne Anim-Vorschub |

Der RE1.5-Fresser 0x16 haelt +0x93 = 1 in Ruhe bereits im Port (Sonde 189) -> keine Aenderung noetig.

### Sonde `unit_r34_reaktion` Teil `re15` (Arena ohne Raum, `re15_enemy_ai_run_all`; Fresser in ROOM1140, RE15-Flavor)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 180/182 | 0x29 Explosion vorn/hinten: Tod, +0x5 9, Clip 10/11 nach 0x80, +0x8c 80, SE 7 1x, Leiche | gruen (hr 0x01 / 0x81) |
| 181/183 | Rueckstoss im ersten Bild: vorn (-80,0), hinten netto (0,0); letztes Bild in der Leiche gehalten | gruen |
| 184 | 0x29 Flaechenfeuer (Art 5): HURT, +0x5 14, HP -50, Clip 7 (Zucken-Spur) | gruen |
| 185/186 | 0x2b vorn/hinten: Ph.0 Clip 8/9 + Bit 2, Ph.2 Clip 0xa/0xb nach 0x80, Leiche | gruen |
| 187 | 0x23 Land: Clip 13, Leiche nach Bild 20, Bit 2 | gruen |
| 188 | 0x27: Zeile 9 Crash-Clip 0xa; Art 5 (Zeile 14) jetzt Boden-Tod Clip 0xe | gruen |
| 189 | 0x16 liegend (RE1.5-KI, ROOM1140): +0x93 Bit 0 in Ruhe, Explosion -> kein Schaden, \|= 2 | gruen (0x01 -> 0x83, HP 65) |

Harness-Befund (Sonde, nicht Engine): nach dem Hunde-Teil blieb ROOM11D0 als `scd_register_current_rdt` registriert; der
VM-Tick saet Slot 1 je Bild mit sub01 des registrierten Raums (`scd_vm.c:674`) -> ROOM1140 spawnte im Folgeteil nichts.
`bringup` setzt jetzt Flags, `g_room_rdt_ok` und die Registrierung auf den Stand eines frischen Prozesses zurueck.

### Mutationsproben
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M36 0x29 DEATH immer Boden-Spur (alter Port) | 180/182 | 180/181/182 rot |
| M37 Spur-Tabelle HURT [14] = Explosion | 184 | 184 rot |
| M38 zweiter Rueckstoss (a0 = 0) weg | 183 | 183 rot |
| M39 0x2b Ph.2 nach Bit 2 (alter Port) | 185 | 185 rot |
| M40 0x23 Bild-20-Regel weg | 187 | 187 rot |
| M41 Spur-Tabelle DEATH [14] = Explosion (alter Port) | 188 | 188 rot |
| M42 0x29-Leiche mit Anim-Vorschub (alter Port) | 181/183 | 181/183 rot |

Regression `ctest -R "maggot|gorilla|roach|cockroach|tyrant|alligator|gator|birkin|kakerlake|made|r34|enemy|stage2|stage3|stage4"`:
21/22 gruen, rot nur `unit_r34_reaktion` (der Harness-Befund oben, danach gruen).

---

## B9 — Endkampf-G5 (`enemy_ai_boss_g5.c`, `re15_damage.c`)

### Selbst disassembliert
em36-KI aus dem RE2-Archiv geschnitten (`python re15_port/tools/re2_ems_cut.py 0x36` -> `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN`
im Arbeitsbaum), dann `re2_disasm.py --bin <Pfad>`:
* Treffer (0x80102940..0x80102acc, `build/r34g_b/em36_treffer.dis`): Zeilen-Partikel 9/17 -> 0x8010221c, 10 -> 0x80101d9c,
  11 -> 0x80101ff0, 14 -> 0x801022d0; Byte[Zeile] `lbu v0,22195(at)` @0x801029bc (0x801056B3 + Zeile); `>= 11` -> +0x225 := 7
  (@0x801029c4-d0); Umgehung +0x226 & 2 (@0x801029dc-e0); Akku +0x222 += Byte (@0x80102a18-34); erster Akku-Treffer: Fenster
  +0x226 \|= 1, +0x221 = 15 (@0x80102a38-4c); `sltiu v0,v0,0xf` @0x80102a58 -> STAGGER 0x80102AD0 mit Akku/Zaehler/Fenster = 0
  (@0x80102a84-90).
* Byte-Tabelle `bytes 0x801056b0 32`: Zeile 0..20 = 0,5,5,5,5,14,20,14,20,14,14,14,5,5,20,1,1,20,1,0,0.
* Main-Kopf @0x801000ec-15c: +0x1D3 -1 je Bild (Bit 0x80 bleibt); bei Fenster: +0x221 -1, bei 0 Akku -1 und +0x221 = 15, Akku 0
  -> Fenster zu. => Akku -1 je 16 Bilder (Port vorher: je 15, ohne Fenster).
* Ctor (0x801003f0..0x801005ac): +0x1EE 5700 (@0x80100578/8c), +0x94 -2000 (@0x80100534-38), +0x96 0 (@0x8010057c), +0x98 -1500
  (@0x80100470 / @0x80100520), +0x9E 1500 (@0x80100448 / @0x80100580), Wort0 \|= 0x0C000000 (@0x801005a0-ac, kein Kopf-Bit).
* Records (RE2-PSX.EXE, Zeiger *(0x800A6A88 + 0x36*4) = 0x800A5EDC): Z9 @0x800A5F7C `0x05014050`/`0x078f1e0a`,
  Z10 @0x800A5F90 `0x00511846`/`0x078f1e0a`, Z11 @0x800A5FA4 `0x00a11846`/`0x078f1e0a` -> 80/80/80, 70/70/5, 70/70/10, Sperre 15.

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `re15_damage.c` `s_re2gl_rec_g5` + `re2_gl_typ` Fall 0x36 | G5 (ROOM5090/5091) ist RE2-Kandidat des Appliers (Bodenfeuer Op 40) UND bekommt den Explosions-Stempel (Sperre 15, Zone, +0x5, Richtung) |
| `enemy_ai_boss_g5.c` Main-Kopf | Sperre -1 je Bild; Akku-Zerfall im Fenster je 16 Bilder |
| `enemy_ai_boss_g5.c` Treffer | Zuschlag = Byte[Zeile] (+0x5 ueber re2z_row_from_weapon) statt der ausgeruesteten Waffe; Fenster armieren; STAGGER setzt Akku/Zaehler/Fenster zurueck; +0x93 Bit 0 bleibt bis zum Ende der Sperre (`treffer_offen`, PORT) |
| `enemy_ai_boss_g5.c` `re15_g5_flinch_zustand` | Testhaken (nur Messung) |

**Wirkung ueber die Granate hinaus (benannt):** der Zuschlag haengt jetzt fuer ALLE Waffen an der Treffer-Zeile. Nach der
Uebersetzungstabelle (Belege je Waffe in `enemy_ai_re2_zombie.c` re2z_row_from_weapon) aendern sich u.a.: w9/w15 (Granate/GL
HE) 20 -> 14, w10/w11/w16/w17 (Saeure/Brand) 5 -> 14, w5/w6 20 -> 5, w12 (MP) 14 -> 1, w13 (SPAS) 14 -> 20, w0/w1/w2 1/5 -> 5.
Die fruehere Zuordnung war laut Kommentar eine "dokumentierte Port-Entscheidung aus §7"; das Original waehlt nach +0x5 (@0x801029b0).

### Sonde `unit_r34_reaktion` Teil `g5` (ROOM5090-Kontext, Bank EM036, Kampfstart grid 0x13; je Arena neuer Slot)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 190 | Granate: HP 600 -> 520, +0x1D3 15, +0x93 Bit 0, +0x5 9 | gruen |
| 191 | nach dem Bild: Akku 14, Fenster offen, Zaehler 15, kein STAGGER, Sperre 14 | gruen |
| 192 | zweite Explosion waehrend der Sperre: kein Schaden, +0x93 = 3 | gruen |
| 193 | Sperre frei genau in Bild 15, Akku noch 14 | gruen |
| 194 | zweite Granate innerhalb des Zerfalls -> STAGGER, Akku/Fenster 0, HP 440 | gruen |
| 195 | NEGATIV Einzeltreffer: nie STAGGER, Akku 13 in Bild 17, 0 in Bild 225, Fenster zu | gruen |
| 196 | Bodenfeuer (Hitcode 0x2002000A) am G5: HP -5, Sperre 15, zweiter Aufruf durch Gate 2 gesperrt, Akku 14 | gruen |

### Mutationsproben
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M43 Byte[9] = 20 (alter Port-Wert) | 191 | 191/193/195 rot |
| M44 Fenster startet mit 14 (15er-Takt) | 195 | 191/195 rot |
| M45 Bit 0 sofort frei (alter Port) | 192/193 | 192/193/194 rot |
| M46 G5 kein RE2-Kandidat | 190/196 | 190-194/196 rot |
| M47 Schaden 1000 (alter Port) | 190 | 190/191/193/194/195 rot |

Regression `ctest -R "g5|5090|birkin|r34|tentakel|damage|schaden"`: 14/14 gruen.

---

## B10 — Treppen-Unverwundbarkeit (`stair_common.c`, E13)

### Selbst disassembliert (PSX.EXE)
* hoch: Gang-Aufbau @0x80038a00 (aca5b = 3, Clip 0x14 @0x80038a0c-14), `lbu v0,-13593` @0x80038a38 / `ori v0,v0,0x1` @0x80038a50 /
  `sb v0,-13593(at)` @0x80038a58 (player+0x93 \|= 1); Abschluss @0x80038bb0 (Standby-Clip 2, aca59/5a/5b := 0), `andi v0,v0,0xfe`
  @0x80038c1c / `sb` @0x80038c24.
* runter: Clip 0x15 @0x80038cb4-bc, `lbu` @0x80038cd0 / `ori 0x1` @0x80038cf8 / `sb` @0x80038d00; Abschluss @0x80038e50,
  `andi 0xfe` @0x80038eb8 / `sb` @0x80038ec0.
* Riegel des Spielerzweigs: `if (p->hit_react & 1) return 0` (re15_damage.c, @0x80012e24-30).

### Gebaut
| Datei:Stelle | Inhalt |
|---|---|
| `re15_stair_try_start` (Erfolgsweg) | `p->hit_react \|= 1` — der Port startet den Gang-Clip direkt (kein Dreh-Vorspann), also an dieser Stelle |
| `re15_stair_tick` Abschluss (`s_finalize`) | `p->hit_react &= ~1` |

### Sonde `unit_r34_reaktion` Teil `treppe` (ROOM1060 Slot 9, Band 2 -> 0, Aufbau wie probe_adv_stairband_1060.c)
| Nr | Pruefung | Ergebnis |
|---|---|---|
| 200 | Treppe gestartet, +0x93 Bit 0 gesetzt | gruen |
| 201 | Explosion (Art 2, 300 neben dem Spieler) waehrend der Treppe: ueberlappt (Rueckgabe 1), HP bleibt 100 | gruen |
| 202 | Treppe beendet (58 Takte), Bit 0 frei | gruen |
| 203 | NEGATIV-Gegenstueck nach der Treppe: dieselbe Explosion -> HP -900, Zustand 3, cmd 3 -> Clip 7 | gruen |

### Mutationsproben
| Mutation | erwartet rot | Ergebnis |
|---|---|---|
| M48 kein Setzen beim Start | 200/201 | 200/201 rot (HP -900 auf der Treppe) |
| M49 kein Loeschen im Abschluss | 202 | 202 rot (203 bleibt gruen: der naechste Spieler-Tick gibt Bit 0 ohnehin frei) |

---

## B11 — Katalog (`RE15_FUN_CATALOG.md`)
* FUN_80012d60: Gate B = `lw v0,144(s1)` / `lui v1,0x300` / `and` / `beq` @0x80012f54-60 = (+0x93 & 3) == 3, nicht gezaehlt (Ziel hinter
  `addiu s4,s4,1` @0x80013024); die alte Lesart "Tod/Despawn-Flags, inert" als falsch markiert; Ablauf danach (&= 1, Seitenbit, Riegel
  Bit 0 @0x80012fbc-cc) ergaenzt.
* FUN_8001a7a8: Rueckgabe 1 = Punkt HINTEN (Instruktionen @0x8001a7c4-ec), Spieler +0x5 = 2 vorn / 3 hinten mit den Schub-Richtungen
  der Handler (@0x80035f18/1c, @0x8003609c/a0).
* FUN_8002b498 neu: Kasten-Versatz je Bild (B2).

---

## B12 — Zensus "KEIN HAENGER-NACHBAU" (`unit_r34_reaktion` Teil `zensus`)

Fuer 22 Typ-Eintraege (alle KI-Typen des `re15_enemy_ai_run_all`-Dispatchs, dazu G5@5090, Gator-Boss@2090, Birkin 0x36@3080,
NPC 0x45) x beide Flavors x Art 2/3/4/5 (+0x5 = 9/10/11/14, DAT_8006f430 @0x8006f432-35) x {INIT-HP, HP 1} = 352 Laeufe:
Treffer ueber den echten Resolver, danach je Bild ein Schritt wie `re15_game_step` (Anim-Vorschub, `run_all`, RE2-Filter,
HP-Stempel; game_step_common.c:2234-2303). Bestanden = der Aktor verlaesst Zustand 2 bzw. 3 binnen 1500 Bildern (G5: Modul-
Routinen). Ergebnis (H = HURT verlassen, T = Tod -> Leiche, t = Tod verlassen ohne Leiche, - = immun/ausgeschlossen):

| Typ | RE15 | RE2 |
|---|---|---|
| Zombie 0x10 / 0x12 / 0x13 / 0x18 | TT TT TT HT | ~~tt~~ **TT** TT TT HT (NACHBESSERUNG K1) |
| Zombie 0x11 (Brad, HP 250) | HT HT HT HT | ~~Ht~~ **HT** HT HT HT (K1) |
| Zombie 0x16 | HT TT HT HT | ~~Ht~~ **HT** TT HT HT (K1) |
| Writher/Arm 0x1a (wach) | TT TT TT HT | tt tt tt Ht |
| Hund 0x20 | TT TT TT HT | TT TT TT HT |
| Kraehe 0x21 | TT TT TT TT | TT TT TT TT |
| Alligator 0x23 | TT TT TT HT | TT TT TT HT |
| Gator-Boss 0x23@2090 | HT HT HT HT | HT HT HT HT |
| FX 0x24 / Ivy 0x2d / NPC 0x45 | -- (immun bzw. E7) | -- |
| Spinne 0x25 | TT TT TT HT | HT HT TT HT |
| Feuer 0x26 | tt tt tt Ht | tt tt tt Ht |
| Made 0x27 / Kakerlake 0x29 / Tyrant 0x2b | TT TT TT HT | TT TT TT HT |
| Birkin 0x30 / 0x36@3080 | tt tt tt Ht (Mutations-Schutz, E7) | tt tt tt Ht |
| G5 0x36@5090 | HT HT HT HT (Modul) | HT HT HT HT |

**[NACHBESSERUNG K1: die RE2-Zombie-`t` der Art 2 waren eine Abweichung vom §1.6-Soll "Tod" — jetzt `T`, s. Dateiende.]**
**Kein Haenger.** Die `t`-Faelle sind belegte Ablaeufe: RE2-Zombie Zeile 9 = Knockdown-Handler 0x80107438 mit `+0x4 == 3`,
Kriecher-Ausgang mit HP 10 (`sh v0(=10),342` @0x801077B0, 3/4 bzw. gaitrow == 0); RE2-Arm = `arm_death` HP 250 + Rueckzug
(@0x80101090-A0, unsterblich); Feuer 0x26 = Flammen-Minderung (kein Leichenzustand); Birkin = Port-Todeshandler mit
Mutations-Schutz (E7).

**Befunde des Zensus (behoben/benannt):**
1. **Feuer-Emitter 0x26 im RE2-Flavor bekam 0 Schaden** (`HH` statt `tt`): E4 (B3) nahm fuer den Typ 0x26 die RE2-Baby-Zeile,
   weil `re15_re2_owns_type` typ- statt herkunftsgenau ist. Behoben in `re15_enemy_take_damage_at`: 0x26 nur mit
   `re15_re2spider_baby_owns(e)` unter dem RE2-Modell (derselbe Herkunfts-Test wie `re15_re2_stamp_hit`). Sonde 212, Mutation M50.
2. Schlafender RE2-Arm (grid & 0x1F != 1): der Port-Tick kehrt vor dem Zustands-Dispatch zurueck; ein Explosionstreffer
   bleibt bis zum Aufwachen in Zustand 3 stehen und laeuft dann (arm_death). Der RE2-Applier liesse ihn ueber Gate 4
   (+0x10E & 0xC000 @0x80047158-64) aus; der RE1.5-Resolver (Traeger der Granate) kennt Gate 4 nicht. OFFEN (kein Haenger,
   verzoegerte Reaktion; ROOM1210 vor Member_set(12,1)).
3. RE1.5-Kraehe HURT = `jr ra` (Tabelle @0x8012111c [2] = 0x80114e4c) — mit INIT-HP 0 unerreichbar (jede Art toetet).
4. Gator-Boss@2090: Modul hebt den Kasten auf Bodenhoehe (+0x7c y = +1200); eine Explosion auf Hoehe des Wassers trifft nicht,
   eine auf dem Steg (y 0) schon — der Zensus wirft dort.

| Nr | Pruefung | Ergebnis |
|---|---|---|
| 210 | 352 Laeufe, 0 Haenger | gruen |
| 211 | 0 unerwartete Immunitaeten/Treffer | gruen |
| 212 | Feuer 0x26 im RE2-Flavor: 1000 Schaden, Zustand 3 | gruen |

Mutationsproben: M50 Herkunfts-Tor 0x26 weg -> 212 rot; M51 RE1.5-Zombie-Tod kehrt bei +0x5 == 9 sofort zurueck (simulierte
NULL-Zeile) -> 210 rot (10 Haenger).

---

## Volle Suite (`local_build.sh`, Bauverzeichnis `re15_port/build_r34_b`)
* **Lauf 1** (all, 2026-09-30 01:19-01:40, parallel zu den Suiten der Arbeitsbaeume r34g_a/_c/_d mit ihren GUI-Laeufen):
  `99% tests passed, 4 tests failed out of 430` — rot nur die GUI-/exe-Integrationen `integration_r30_irons_tisch_licht`
  (kein Framedump, exe exit 1), `integration_r30_titel_puls` (eine Pulsperiode am Bild 8936 us daneben bei 106 ms
  Bilddauer), `integration_r33_speichern` (exe exit 1 beim Hochfahren, vor dem CONTINUE), `integration_dark_start_pin`
  (exe exit 1). Einzeln wiederholt: alle vier **gruen** (dark_start_pin 19.9 s, irons_tisch_licht 60.6 s, titel_puls 25.8 s,
  r33_speichern 89.5 s beim dritten Einzellauf; die ersten zwei Einzellaeufe fielen mit exe exit 1 an wechselnden Stellen
  des Hochfahrens, waehrend Arbeitsbaum r34g_c GENAU DIESE Tests ebenfalls einzeln nachfuhr). Der r33-Ablauf mit
  denselben Umgebungsvariablen von Hand: exit 0, `CONTINUE: resumed`, Hinweis ohne Karte. Keiner der vier Tests beruehrt
  Schaden/Gegnerreaktion; RE15_MIN_TESTS unveraendert.
* **Lauf 2** (test, 02:14-02:32, dieselben Binaerdateien): `99% tests passed, 1 tests failed out of 430` — die vier aus Lauf 1
  gruen, rot nur `integration_r30_cut_blitz` (in Lauf 1 gruen). Einzeln waehrend der GUI-Suiten von r34g_a, r34g_c,
  r34g_d und r34n_rolltor fuenfmal rot, jedes Mal mit exe **exit 1 an einer anderen Stelle** (vor dem ersten Bild,
  Bild 30, 60, 150, 240 — `debug.log` bricht mitten im Lauf ab); kein Absturzcode (0xC0000005), sondern Rueckgabe 1 wie
  bei einem von aussen beendeten Prozess oder dem dokumentierten SDL-Abbau-Flake (`platform/pc/main.c:2749-2762`).
  Um 02:48 einzeln **gruen** (88.6 s). Damit ist jeder der 430 Tests auf demselben Stand mindestens einmal gruen gelaufen.
* **Lauf 3** (test, 02:52-03:10): `3 tests failed out of 430` — `integration_elza_vollstart`, `integration_r30_granate_laden`
  (Lauf "abgerissen", exit 1), `integration_r30_irons_tisch_laden` (exit 1 vor Bild 120) — alle drei in Lauf 1 und 2 gruen,
  einzeln sofort gruen (101.1 s / 96.2 s / 35.8 s). Wieder nur GUI-Integrationen, wieder wechselnde.
* **Lauf 4** (test, 03:14-03:27): `5 tests failed out of 430` — `integration_r30_irons_tisch_bild`, `..._licht`,
  `integration_r33_speichern`, `integration_relatch_pin`, `integration_save_counter_pin` (alle: exe exit 1 frueh bzw.
  mitten im Lauf). Einzeln: relatch_pin und save_counter_pin sofort gruen, irons_tisch_licht im 2., r33_speichern im 2.,
  irons_tisch_bild im 4. Einzellauf gruen. Zur selben Zeit fuhr Arbeitsbaum r34g_c GENAU diese GUI-Tests
  (cut_blitz, elza_vollstart, granate_laden, irons_tisch_*) einzeln nach — die Ausfaelle sind baumuebergreifend.
* **Lauf 5** (test, 03:34-03:47): `99% tests passed, 1 tests failed out of 430` — nur `integration_r33_speichern`
  (Lauf a: `debug.log` endet mitten im Lauf bei Bild 240, exit 1; die Laeufe b und c desselben Tests erreichen
  `EXIT_AT`). Einzeln sofort gruen (81.9 s).
* **Ergebnis:** in fuenf vollen Laeufen waren ALLE Unit-/Sonden-Tests (darunter unit_r34_schaden, unit_r34_reaktion)
  jedes Mal gruen; rot waren ausschliesslich GUI-Integrationen der exe mit Abbruch `exit 1` an wechselnden Stellen, jeder
  davon einzeln gruen. Eine Zeile `LOCAL-BUILD-OK` kam unter der Parallel-Last der anderen Arbeitsbaeume nicht zustande
  (letzte Zeile: `!!! [local_build] FEHLER: ctest fehlgeschlagen (exit=8), Log: re15_port/build_r34_b/local_build_ctest.log`).
  Empfehlung: die volle Suite nach dem Merge auf einer ruhigen Maschine einmal ohne Parallel-Laeufe wiederholen.

## INTEGRATIONSWUNSCH
1. **Part-Farben/-Flags des Hundes und der Spinne zeichnen** (Spur C/D, `platform/pc/main.c` ~9717): `re15_re2z_gore_resolve`
   bedient nur die Zombie-Familie (`re15_re2z_owns_type`). Hund (17 Parts) und Spinne (20 Parts) tragen jetzt die
   Original-Farbworte in `re2z_part_tint[]` (0 = nicht gesetzt, KEIN Neutralwert 0x00808080) und die Wurf-Flags in
   `re2z_part_flags[]`. Wunsch: fuer Typ 0x20/0x25 (RE2-Flavor) das Farbwort als NCCT-Faktor wie beim Zombie
   (`ldrgb` @0x80027C2C / NCCT @0x80027D10), Wert 0 -> neutral.
2. **Applier binden** (Spur C/D, Bindungsstelle der RE2-FX-Haken in `platform/pc/main.c`, dazu PSX/Android an derselben
   Stelle): `re2fx_applier = re15_re2_gl_apply;` (`#include "re15_damage.h"`). Vertrag V2b (`include/re2_fx.h:50-54`:
   "Vorgabe NULL ... die Plattform bindet ihn an re15_re2_gl_apply, sobald Spur B gemergt ist"). Ohne die Bindung
   macht das Bodenfeuer (Op 40) keinen Schaden.
3. **Hinweis an alle Leser von `re2z_part_*`** (keine Aenderung noetig): `re2z_part_flags/_tint/_yaw98/_w9a/_w9c/_w9e` haben
   jetzt 20 Eintraege (`include/re15_actor.h`); der Zombie-Renderer liest weiter 0..15.

## OFFEN
1. **Hunde-Koerper-Schub mit dem Original-Kasten** (900/720/450 @0x80120f64): nur der Resolver nimmt ihn (B2); auf dem
   Aktor wurde `unit_r27_hund_wandtrieb` rot (469 Bilder in der Wand). Eigene Runde.
2. Kopf-Bit (Wort0 & 0x10000000) fuer Hund/Kraehe/Spinne/Baby nicht gescannt (verhaltensneutral, die Gehirne lesen +0x1D2 /3).
3. Kraehe: +0x98 im Flug (0/-350 @0x80100204/08) hat kein Port-Feld; der Applier nimmt den INIT-Wert -350/530.
4. Applier-Zeilen ausser 9/10/11 nicht hinterlegt (einziger Aufrufer Op 40 = Zeile 10).
5. Hund: +0x1C0 \|= 1 im Teile-Wurf (@0x80104458-64) ohne Port-Feld; Flug der geworfenen Parts, FX-Arten 0x84/0x86 (FX 9-12)
   und die Part-Farben ohne Zeichnung (-> INTEGRATIONSWUNSCH 1); Todes-Zeilen {7,8} (0x801042B0, aus w8/w13) und 14
   (0x801048B4, von der Port-Uebersetzung nie gebildet) weiter auf dem Kern.
6. Spinne: Part-19-Flug (FUN_80028DAC, +0x9C = 0x6464) nicht gezeichnet; die Gore-Salven der Zeilen 10/16 feuern ohne das
   Part-Flag-Tor `(flags & 0x4B) == 1` (@0x80104b00-0c), weil der Port fuer die Spinne keine Part-Flags seedet (bestehend).
7. Kakerlake 0x29: Luft-Landung im HURT/DEATH-Kopf (+0x1e0 -> 0x8001c1a4(0,0,-50,+0x1ba), Kasten @0x8011ec44[+0x1e4], SE 1
   @0x801147a0-804 / @0x80114fc4-5028) und Lokator FUN_80115b68 nicht portiert; Blut 0x80019700 = Render.
8. Alligator 0x23 (nicht 2090): EM023 in keinem RE1.5-EMS -> Clip-13-Laenge unbekannt; im Wasser (+0x1e0 != 0) endet der Tod
   am Clip-Ende nur mit geladener Bank, ohne Bank greift die Bild-20-Regel (PORT-SICHERUNG).
9. Tyrant 0x2b: Fussanker FUN_80115bec (Lokator) nicht portiert.
10. G5: Ruettler +0x225 := 7 (@0x801029c4-d0), Zeilen-Partikel (0x8010221c / 0x80101d9c / 0x80101ff0 / 0x801022d0) nicht
    portiert; Sperre nur fuer die GL-Zeilen gelesen (w1 der anderen Records nicht) — der Schuss-Pfad bleibt ohne Sperre.
11. G5-Zuschlag nach Zeile wirkt auf alle Waffen (Liste in B9) — Abnahme durch Nutzer/Orchestrator.
12. Schlafender RE2-Arm (grid & 0x1F != 1): Explosion setzt Zustand 3, Reaktion erst nach dem Aufwachen (RE2-Gate 4 fehlt im
    RE1.5-Resolver).
13. ~~`re2z_row_from_atktype[5] = 9` (bestehend): Art 5 trifft RE2-KI-Typen im Spiel nicht (fuer sie laeuft der RE2-Applier);~~
    **BERICHTIGT (Gegenpruefung M1):** ueber O-VB4 erreicht Art 5 jeden RE1.5-KI-Import-Zombie (Vorgabe im RE1.5-Flavor);
    die Bruecke lief dabei ueber Zeile 9 mit Reserve-Abzug. Behoben (GL-Bruecke, NACHBESSERUNG N3). Alter Wortlaut:
    nur im Zensus kuenstlich erreicht, nicht angefasst.
14. Treppe: `re15_stair_reset` (Raumwechsel) loescht +0x93 Bit 0 nicht — kein Original-Pendant; der naechste Spieler-Tick gibt
    es frei (gemessen: Mutation M49 laesst 203 gruen).
15. Sichtpruefung am echten Programm (RE15_FRAMEDUMP) fuer Hund/Spinne/Kakerlake/Tyrant/Alligator nicht gemacht: die
    Part-Farben werden nicht gezeichnet (INTEGRATIONSWUNSCH 1), die Clip-Wahl ist per Sonde belegt.

---

## NACHBESSERUNG (nach der Gegenpruefung `bau_b.gegenpruefung.md`: K1, M1, M2)

Stand: 2026-09-30, Arbeitsbaum `.claude/worktrees/r34g_b`, Zweig `r34g/b-schaden` (Basis 8dcd2995). Laufzeit-Ausgaben
`build/r34g_b/nb/` (unversioniert). Gebaut ausschliesslich ueber `build/r34g_b/nb/lb.sh` = `local_build.sh` mit
`RE15_BUILD_DIR=re15_port/build_r34_b` und einer exportierten Shell-Funktion `powershell` (-> echtes
`powershell.exe`) plus neutralisiertem `taskkill`: `local_build.sh` ersetzt PATH durch CLEAN_PATH ohne
WindowsPowerShell, dann faellt `do_build` auf `taskkill //F //IM re15_pc.exe` zurueck (Gegenpruefung §6) — mit dem
Wrapper laeuft der GEZIELTE Zweig (nur exe dieses Bauverzeichnisses), keine fremde re15_pc.exe wird beendet.

### N0 Reproduktion (Schritt 1 des RE-Gates, gegen HEAD 8dcd2995)
| Messung | Ergebnis |
|---|---|
| mess5 (Gegenpruefer, ROOM1140, RE2-Flavor, HE neben stehendem 0x10, RE2-Strom k = 0..23) | 24/24 `st=3 col=0 -> Ende 1 (st=1 hp=10 f10e=0x2001)` = Kriecher |
| mess1 M1 / M1b / M2 | Import AN -15, Import AUS -50, RE2-KI -5 |
| mess3 (12 Bodenfeuer-Treffer seitlich, Import AN) | je -15, +0x152 13 -> 11 -> ... -> -1, Treffer 7: `21a&0x60=0x20`, `mesh9=15` |

### N1 RE-Belege (selbst disassembliert in dieser Sitzung)
**K1 — welche Spalte stempelt RE2 bei einer EXPLOSION?**
* RE2 Op 47 (GL-Explosion, PSX.EXE): `80020d54 lui a3,0x1002` / `80020d58 ori a3,a3,0x9` / `80020d78 jal 0x800470c0`;
  zweiter Aufruf `80020d84 lui a3,0x1002` / `80020d88 ori a3,a3,0x9` / `80020d98 addiu v0,v0,900` (P.y + 900) /
  `80020db0 jal 0x800470c0` -> Hitcode **0x10020009** (Klammer 1, Zonenbit 0x20000, Zeile 9, Einzelmodus).
* RE2 Flug-Kontakt der GL-Runde (PSX.EXE): `8001ee90 lui s2,0x3` / `8001ee9c ori s2,s2,0x9` / `8001eed0 lb a3,27(v1)` /
  `8001eed8 jal 0x800470c0` mit Delay `8001eedc addu a3,a3,s2` -> 0x00030009 + (s8)+0x1B = **Klammer 0** — nur der
  Kontakt des FLIEGENDEN Geschosses; die Handgranate hat keinen (E2).
* Alle Aufrufer von 0x800470C0 (eigener Wort-Scan `jal 0x800470c0`, 17 Stellen): Op 47 0x10020009 (K1) @0x80020d78/db0,
  Op 48 Brand 0x0002000A (K0) @0x80021060/8c/0x800214f8/524, Op 49 Saeure 0x1002000B (K1) @0x800216ec/718, Op 40
  0x2002000A (K2) @0x800207bc.
* Spalte = Zone + 3K: `80047114 srl s6,s5,28`, `800472a8 sll v1,s6,1` (Delay) bzw. `80047310 sll v1,s6,1`,
  `80047320 addu v1,v1,s6`, `80047328 addu v0,v0,v1`, `80047330 sb v0,466(s1)`.
* EMZ0 DEATH Zeile 9 `table 0x8010CD68 9` = {0x80107438, 0x80108BEC, 0x80108BEC, 0x80108530 x6}; Zeilen 10/11
  `table 0x8010CD8C 18` = 0x80108530 x18 (spaltenunabhaengig). HURT Zeile 9 `table 0x8010CA84 9` = {0x80107438,
  0x80105BC0, 0x80105BC0, 0x80105438 x6}; Zeilen 10/11 = {0x80105BC0 x3, 0x80105438 x6}.
* Spalte 0 -> 0x80107438 (Knockdown) mit `+0x4 == 3`: `801074a0 beq v1,v0(=2),0x8010777c` / Delay `801074a4 addiu
  v0,zero,3`; `8010777c lbu v1,4(s2)` / `80107784 bne v1,v0`; `8010778c jal 0x80015fe8` / `80107794 andi v0,v0,0x3` /
  `80107798 bne v0,zero,0x801077b0` (Delay `addiu v0,zero,10`); `801077a0 lb v0,363(s2)` / `801077a8 bne v0,zero,
  0x801077dc`; `801077b0 sh v0(=10),342(s2)`; Leiche nur ueber `801077b4 lh v0,268(s2)` / `801077bc bne` oder
  `801077c8 lw v0,-1064(0x800d)` & 0x10000000 (`801077d4 beq v0,zero,0x801077f4` = Kriecher). Das Bit 0x10000000 von
  0x800CFBD8 setzt im ganzen RE2-PSX.EXE nur EIN Schreiber (`80058dcc lui v1,0x1000` / `80058df0 or` / `80058df8 sw
  v0,-1064(at)`, ein SCD-Opcode-Rumpf mit `lw v0,28(a0)`-Vorschub; eigener Xref-Scan `build/r34g_b/nb/xref_cfbd8.txt`)
  — im normalen Spiel frei, also Kriecher. Ergebnis: Spalte 0 = Wiederbelebung als Kriecher mit HP 10 (Port
  `re2z_hit_knockdown(death=1)` richtig).
* Spalte 3 -> 0x80108530 (Sturz-Tod): P2 `80108918 jal 0x80015fe8` mit Delay `8010891c sw v0(=7),4(s1)` = Leiche;
  Wiederbelebung nur hinter fuenf Toren (`80108920 andi 0x3 / bne`, `80108934 andi 0x4 / bne`, `80108940 lh 268`,
  `80108950 lb 363`, `80108964-70` 0x800CFBD8 & 0x10000000) mit HP 1 (`8010897c addiu v0,zero,1` / `80108980 sh
  v0,342`). Port `re2z_death_main` P2 = Leiche, die Wiederbelebung ist dort seit jeher OFFEN (benannt, alle Waffen).
* HURT Spalte 3 = 0x80105438 (Haupt-Treffer): P0 ruft die Element-Leiter nur fuer Zeile 16 (`8010551c addiu v0,zero,16`
  / `80105520 bne` / `80105550 jal 0x80106128`) und 14 (`80105728 addiu v0,zero,14` / `80105734 jal 0x80106510`).

**M1 — was darf das Bodenfeuer an einem RE1.5-KI-Kandidaten?**
* RE1.5-Tabellen (`re15_disasm.py read 0x8006f418 11 --w 2 --signed` = [10,20,1000,1000,1000,50,100,200,300,1000,0],
  `read 0x8006f430 11` = [3,3,9,10,11,14,15,16,17,18,20]): Art 5 = 50 @0x8006f422, Reaktion 14 @0x8006f435.
* RE2-Wert DESSELBEN Hitcodes am Zombie: Z10 w0 0x0050C8C8 @0x800A41E0 (0x16: 0x0050C850 @0x800A435C), K2 = `srlv`
  @0x80047254 um 20 / `andi v1,v1,0x3ff` @0x8004725c = **5**.
* Store-Liste FUN_800470C0 (eigene Auswertung `build/r34g_b_gp/re2_800470c0.dis`): +0x156 @0x80047268/488, +0x4
  @0x80047288/90, +0x1D0 @0x80047184/204, +0x1D2 @0x80047298/2d4/30c/330, +0x1D3 @0x80047334(+Folge), +0x5 @0x80047324,
  +0x1FC @0x80047278/49c, Puffer (s3) — **kein** Store auf 337/338/339 (+0x151..+0x153): der GL-Applier zieht keine
  Zonen-Reserve ab.
* BAUPLAN E4 nennt die Modell-Ausnahme NUR fuer Art 2/3/4 (`react_table[Art]` = 9/10/11); der Bau hatte sie auf
  `type < 11` ausgedehnt -> Art 5 an Import-Zombies = `s_re2_wpn_dmg_zombie[14]` = 15 (RE2-FLAMMENWERFER-Zeile 16
  @0x800A4258) — weder O-VB4 (50) noch RE2 (5).

**M2 — was pinnen?** Wache `if (e->re2_gl_stamp)` in `re2z_hurt` (Konsument im Bild X+1) und `+0x1D0 &= 0xFF00` je
Kandidat (`8004716c lhu v1,464(s0)` / `80047178 andi v1,v1,0xff00` / `80047184 sh v1,464(s0)`, VOR dem Band — also
auch fuer Kandidaten, die das Band/den Kasten verfehlen).

### N2 Entscheidungen (von Spur B getroffen, mit Beleg; zur Abnahme durch Orchestrator/Nutzer)
* **K1 -> Option (a), nur Zombie-Familie, nur Zeile 9 (HE).** Die Explosion der Handgranate stempelt fuer die
  RE2-Zombie-Familie die Spalte mit der Klammer der RE2-Explosion Op 47 (K1): +0x1D2 = Zone(P) + 3. Der Schaden bleibt
  E4 (K0 = 200 @0x800A41CC). Begruendung: (1) BAUPLAN §1.6 Soll "Tod"; (2) RE2s eigene Explosion stempelt K1 -> DEATH[9][3]
  = 0x80108530 (Sturz-Tod -> Leiche), die Kriecher-Wiederbelebung gehoert zum Flug-KONTAKT (K0), den die Handgranate nicht
  hat; (3) die BAUPLAN-Wahl "Spalte mit K0" war als "Folge" von E4 begruendet und ging von "DEATH[9][0] = Knockdown-TOD"
  aus — das ist widerlegt (Wiederbelebung, s. N1). NICHT geaendert: Saeure/Brand (DEATH 10/11 spaltenunabhaengig; HURT
  Spalte 0 = 0x80105BC0 mit Aetzung/Bein-Wegaetzen/Verkohlung = §1.6-Soll, Brand deckt sich mit Op 48 = K0; Saeure-Op 49
  waere K1 -> 0x80105438 ohne die Aetzung im P0 — benannt, nicht uebernommen), Hund (Spalte >= 3 -> nur Kern/Schrei
  @0x801046a8-d4 statt "zerplatzt", §1.6), Spinne/Kraehe/G5 (unveraendert K0). FOLGE fuer Brad 0x11 (ueberlebt HE):
  HURT[9][3] = 0x80105438 (Haupt-Treffer) statt HURT[9][0] = 0x80107438 (Knockdown) — die RE2-Reaktion eines Zombies,
  der die GL-Explosion ueberlebt; §1.6 "Brad HE HURT[9][0]" ist damit ersetzt.
* **M1 -> E4 auf Art 2..4 begrenzt; Bodenfeuer an RE1.5-KI-Kandidaten:** Typen unter dem RE2-Schadens-/HP-Modell
  (Import-Zombies im RE1.5-Flavor = Vorgabe; dieselbe Bedingung wie E4) bekommen den RE2-Record-Wert des Hitcodes, also
  genau das, was ein RE2-KI-Gegner desselben Typs fuer dieselbe Flamme bekommt (Zombie Z10 K2 = 5 @0x800A41E0); alle
  anderen RE1.5-KI-Kandidaten O-VB4 = 50 @0x8006f422. Grund: Nutzer-Auftrag der Import-Option ("die Schadenswerte fuer
  Zombies ... auch in RE1.5 AI", Kopf `re15_damage.c` 675-697, "HP UND SCHADEN GEHEN NUR ZUSAMMEN") — RE2-HP mit der
  RE1.5-Zahl 50 waere die verbotene Haelften-Mischung. Die Import-Bruecke laeuft fuer GL-Treffer als GL-Stempel: Zeile =
  Hitcode-Zeile (10), Spalte = Zone(P) + 3K, Richtung aus P mit `|= 1`, KEIN Reserve-Abzug; der Zerleger bleibt (sein
  Reserve-Tor oeffnet nur, wenn Schuesse die Reserve vorher geleert haben — wie im RE2-Flavor).
* **M2 -> Sonden**, kein Engine-Eingriff (Verhalten war richtig, gemessen mess4).

### N3 Gebaut (Datei:Zeile, Stand nach Commit 023f5532)
| Datei:Zeile | Inhalt | Beleg |
|---|---|---|
| `re15_damage.c:4211-4213` (`re2_gl_explosion_stempel`) | `k_spalte = (t.zombie && zeile == 9u) ? 1u : 0u` -> `re2_gl_stempel(..., k_spalte, ...)`: HE an der RE2-Zombie-Familie stempelt +0x1D2 = Zone + 3; Schaden unveraendert E4 (K0) | Op 47 0x10020009 @0x80020d54-58/0x80020d84-88, Spalte @0x80047310-30 (K1) |
| `re15_damage.c:43` | `re2gl_treffer_t {zeile, k, spalte}` — der Hitcode eines GL-Treffers an einem RE1.5-KI-Kandidaten | `andi v1,s5,0xffff` @0x80047214, `srl s6,s5,28` @0x80047114 |
| `re15_damage.c:2984` (`re15_e4_modell`) | E4-Bedingung ausgelagert (Herkunft 0x26 + RE2-Modell/Import) | Kopf `re15_damage.c` 675-697 |
| `re15_damage.c:3021` | E4-Modellwahl nur noch `type >= 2u && type <= 4u` | BAUPLAN E4, react_table @0x8006f432..34 |
| `re15_damage.c:3031-3035` | GL-Treffer an einem Typ unter dem RE2-Modell: HP -= (w0[Zeile] >> 10K) & 0x3FF des Typ-Records | `srlv` @0x80047254 / `andi v1,v1,0x3ff` @0x8004725c; Z10 0x0050C8C8 @0x800A41E0 |
| `re15_damage.c:3069-3070` | GL-Treffer -> `re15_re15_re2z_gore_hit_gl` statt der Hitscan-Bruecke | Store-Liste @0x80047184-0x8004749c |
| `re15_damage.c:3970` (`re2_gl_rec_typ`) | Records je Typ ohne Besitz-Test (Zombie/0x16/Hund/Kraehe/Spinne) | *(0x800A6A88 + Typ*4) |
| `re15_damage.c:4286-4290` (Applier, RE1.5-Pfad) | Spalte = Zone + 3K mit dem Band-Versatz des Kandidaten (Port-Zuordnung O-VB4), `gl` an den Gegnerzweig | @0x80047294-330 |
| `re15_damage.c` `re15_resolver_gegnerzweig` | Parameter `gl` (Resolver: NULL) | — |
| `enemy_ai_re2_zombie.c:6767` (`re2z_import_seed`) | Lazy-Init der Bruecke ausgelagert (unveraendert) | @0x8010081C/@0x80100820-28, @0x801006BC, @0x8010087C |
| `enemy_ai_re2_zombie.c:6825` (`re15_re15_re2z_gore_hit_gl`) | GL-Bruecke: +0x1D2 = Spalte, +0x1D0 = (alt & 0xFF00) \| 1 \| Richtung aus P, KEINE Reserve, Zerleger, Gore-Zweig | @0x80047330, @0x80047178/84, @0x80047200, @0x80047350-3d8, @0x80105288-3D8 |
| `enemy_ai_re2_zombie.c:6848` (`re2z_import_gore_dispatch`) | Gore-Zweig (3) der Bruecke ausgelagert (unveraendert) | @0x8010750C-708 |
| `include/re15_actor.h:1254` | Prototyp `re15_re15_re2z_gore_hit_gl` | — |

### N4 Sonden (neu bzw. geaendert; alle gruen, Stand 3558eaff + (99))
| Nr | Sonde | Pruefung | Ergebnis |
|---|---|---|---|
| 31 | schaden | HE an RE2-0x10: +0x1D2 = **3** (vorher 0) | gruen |
| 34 | schaden | Brad HE: Spalte **3** | gruen |
| 30 | schaden | Abgrenzung: Saeure/Brand Spalte 0, Hund HE Spalte < 3, +0x5 9 | gruen (0/0/1) |
| 81 | schaden | Explosion: +0x1D0 vorbelegt 0x12E0 -> 0x1221 | gruen |
| 82 | schaden | Applier: Kandidat ausserhalb des Kastens 0x34C0 -> 0x3400, getroffener 0x56E0 -> 0x5661 | gruen |
| 95 | schaden | Import-Zombie + Bodenfeuer: HP 80 -> 75, +0x5 14, +0x6 1, +0x1D2 6, +0x1D0 Bit 0 | gruen |
| 96 | schaden | 12 seitliche Flammen: je 5, Reserve 13/13/13, kein Bein; NEGATIV Pistole danach +0x152 13 -> 9 | gruen |
| 97 | schaden | NEGATIV Import AUS: 50 (O-VB4), keine Bruecke (+0x1D2 0) | gruen |
| 98 | schaden | Zerleger im GL-Stempel: +0x152 = -1 vorher -> seitliche Flamme reisst das Bein ab | gruen |
| 99 | schaden | E4 nur Art 2..4: Direktaufruf Art 5 = 50, Art 2 = 200 | gruen |
| 108/118 | reaktion | Bild X+1 nach dem GL-Stempel (Brand/Saeure an Brad): Reserve 13/13/13 unveraendert, +0x1D0 0x21 unveraendert, gl 0 | gruen |
| 109 | reaktion | NEGATIV: derselbe Treffer ohne GL-Stempel -> Reserve-Summe 39 -> 37 im Bild X+1 | gruen |
| 220 | reaktion | HE an stehendem 0x10 (ROOM1140, echter `re15_game_step`), 8 RNG-Verschiebungen: 8/8 Leiche, DEATH-Zelle 0x80108530 | gruen |
| 221 | reaktion | NEGATIV: Spalte von Hand 0 -> Kriecher HP 10, +0x10E 0x2001, DEATH-Zelle 0x80107438 | gruen |
| 222 | reaktion | Brad HE -> HURT 0x80105438 (Spalte 3); Brand -> 0x80105BC0 (Spalte 0) | gruen |

Zensus (Teil zensus, 352 Laeufe) nach der Nachbesserung: 0 Haenger, 0 unerwartet; RE2-Flavor Zombie 0x10/0x12/0x13/0x18
`TT TT TT HT` (vorher `tt ...`), Brad 0x11 `HT HT HT HT` (vorher `Ht ...`), 0x16 `HT TT HT HT` (vorher `Ht ...`); alle
uebrigen Zeilen unveraendert.

### N5 Mutationsproben (`build/r34g_b/nb/mut_nb.py`, Bau je Mutation ueber `lb.sh build`, Log `mut_nb.log`)
| Id | Mutation (Datei) | erwartet rot | Ergebnis |
|---|---|---|---|
| MN1 | Spalten-Klammer HE = 0 (Stand vor K1) (re15_damage.c) | 31 34 220 222 | 31/34/220/222 rot (220: Leiche 0, Kriecher 8) |
| MN2 | Klammer 1 fuer ALLE Zeilen der Zombie-Familie | 30 101 111 222 | 30/101-106/111-114/222 rot — Saeure/Brand mit Spalte 3 verlieren Element-Bits und DoT (belegt die Beschraenkung auf Zeile 9) |
| MN3 | Klammer 1 auch fuer den Hund | 30 120 | 30/120-124 rot (kein Zerplatzen) |
| MN4 | E4 wieder `type < 11` | 99 | 99 rot (Art 5 -> 15) |
| MN5 | RE2-Record-Schaden des GL-Treffers aus | 95 96 | 95/96 rot (50 statt 5) |
| MN6 | alte Bruecke fuer GL-Treffer (Hitscan-Stempel) | 95 96 | 95/96 rot (+0x1D2 1, Reserve -1, Bein ab) |
| MN7 | GL-Wache in `re2z_hurt` weg (= G06 der Gegenpruefung) | 108 118 | 108/118 rot (Reserve 13 -> 11, +0x1D0 0x21 -> 0x00) |
| MN8 | Applier ohne +0x1D0 &= 0xFF00 je Kandidat (= G03) | 82 | 82 rot (A 0x34C0, B 0x56E1) |
| MN9 | Explosions-Stempel ohne das Loeschen | 81 | 81 rot (0x12E1) |
| MN10 | GL-Bruecke ohne Bit 0 | 95 | 95 rot |
| MN11 | Zone im RE1.5-Pfad immer 1 | 95 | 95 rot (+0x1D2 7) |
| MN12 | GL-Bruecke ohne Zerleger | 98 | 98 rot |

Alle Dateien nach jeder Mutation aus der Sicherung zurueck (Pruefung Byte-gleich), danach `lb.sh build` und beide Sonden
gruen, `git diff` leer.

### N6 Messungen nach der Nachbesserung (Programme der Gegenpruefung, neu gegen `libre15_engine.a` gebaut, `build/r34g_b/nb/`)
| Messung | vorher | nachher |
|---|---|---|
| mess5 (24 RNG-Verschiebungen, HE an stehendem 0x10) | Kriecher 24/24 | **Leiche 24/24**, Spalte 3 |
| mess6 (6 Laeufe + 2. Granate) | 2. Granate noetig | Leiche 6/6 schon nach der 1. |
| mess1 M1 / M1b / M2 | -15 / -50 / -5 | **-5** / -50 / -5 |
| mess3 (12 seitliche Flammen, Import AN) | -15 je, +0x152 -> -1, Bein ab bei 7 | **-5 je, +0x152 13, kein Bein**, +0x1D0 0x0061/0x0081 |
| mess4 (Brad Art 4, X..X+3) | Reserve 13/13/13, 1D0 0x0021 | unveraendert (Wache wirkt; jetzt gepinnt 108/118) |

### N7 Folgen und Grenzen (benannt)
* **Brad 0x11 (und 0x16 mit viel HP) ueberlebt HE:** HURT[9][3] = 0x80105438 (Haupt-Treffer) statt 0x80107438 (Knockdown mit
  Arm-Abriss @0x8010750C-708). RE2-Reaktion eines Zombies, der die GL-Explosion ueberlebt. BAUPLAN §1.6 nennt noch
  HURT[9][0] -> INTEGRATIONSWUNSCH N-2.
* **Saeure-Spalte:** RE2 Op 49 stempelt 0x1002000B (K1, `800216e4 lui a3,0x1002` / Delay `800216f0 ori a3,a3,0xb`). E6 K0 bleibt
  (BAUPLAN §1.6: Brad-Saeure = Taumeln 0x80105BC0 mit Aetzung/Bein-Wegaetzen/DoT). Mutation MN2 belegt: mit Spalte 3
  verliert der ueberlebende Zombie Element-Bits und DoT (0x80105438-P0 ruft die Leiter nur fuer Zeile 16/14). Fuer die
  Toetung ist die Spalte bedeutungslos (DEATH 10/11 = 0x80108530 x18). OFFEN N-a (Entscheidung Orchestrator/Nutzer).
* **Wiederbelebung nach dem Sturz-Tod:** RE2 0x80108530-P2 belebt hinter fuenf Toren (~1/8) als Kriecher mit HP 1
  (@0x80108918-9ec); der Port-`re2z_death_main` endet seit jeher als Leiche (OFFEN, alle Waffen) — deshalb 8/8 bzw. 24/24
  Leiche statt ~7/8. OFFEN N-b.
* **Import-Zombie + Bodenfeuer:** Reaktion bleibt O-VB4 (+0x5 = 14, E7-Rueckfall bei NULL-Zeile); nur Schaden und Bruecke
  folgen dem RE2-Modell.

### N8 Volle Suite
* **Lauf 1** (all, 05:10-05:21, Stand 0cc7227f = K1/M1 ohne den Nachtrag N3b; parallel liefen die Suiten/Baue anderer
  Arbeitsbaeume): `99% tests passed, 3 tests failed out of 430` — rot nur die GUI-/exe-Integrationen
  `integration_r30_cut_blitz`, `integration_r30_sicherung_laden` ("Bild 280 ... nicht erreicht (keine EXIT_AT-Zeile,
  exit=1) — der Lauf ist vorher abgerissen"), `integration_r30_titel_puls` — dieselbe Klasse wie beim Bauer und beim
  Gegenpruefer (Gegenpruefung §6: `taskkill //IM` fremder `local_build.sh build`). Einzeln wiederholt (`ctest -R ...
  --repeat until-pass:3 -j 1`, 05:21-05:25): sicherung_laden und titel_puls sofort gruen, cut_blitz im 3. Versuch
  (Versuch 1/2: "0 mit 3D" bzw. "nur 1 sichtbare Kamerawechsel" — Kamera-Weg, kein Schadenspfad). Kein
  reproduzierbares Rot; unit_r34_schaden / unit_r34_reaktion gruen.
* **Lauf 2** (all, 05:26-05:39, Stand 16a8d90e = alle Aenderungen der Nachbesserung, gebaut ueber `lb.sh` =
  `local_build.sh` ohne Argument): **`100% tests passed, 0 tests failed out of 430`**, Abschlusszeile
  **`=== LOCAL-BUILD-OK (all) — Tests 430/430`** (795 s Testzeit). RE15_MIN_TESTS unveraendert.

### INTEGRATIONSWUNSCH (Nachbesserung)
* **N-1** `re15_port/tools/local_build.sh:147` (Integration): `/c/Windows/System32/WindowsPowerShell/v1.0` in `CLEAN_PATH`
  aufnehmen; sonst faellt `do_build` (:298-299) auf `taskkill //F //IM re15_pc.exe` zurueck und beendet JEDE re15_pc.exe der
  Maschine (auch die des Nutzers). Spur B baute deshalb ueber `build/r34g_b/nb/lb.sh` (exportierte `powershell`-Funktion,
  neutralisiertes `taskkill`), gemessen: der gezielte Zweig laeuft.
* **N-2** `analysis/befunde_runde34_granaten/BAUPLAN.md` §1.6 Zeile "Zombie 0x10/..." (nicht Spur-B-Datei): "Tod: HE DEATH[9][0]
  = 0x80107438 (Knockdown-Tod ...)" -> "HE DEATH[9][3] = 0x80108530 (Sturz-Tod; Spalte mit Klammer 1 wie RE2 Op 47,
  0x10020009 @0x80020d54-58)"; "Brad ... HE HURT[9][0] = 0x80107438" -> "HE HURT[9][3] = 0x80105438". E6 um "Spalte HE an
  der Zombie-Familie = Zone + 3" ergaenzen; E4 bleibt (Schaden K0).
* Unveraendert gueltig: INTEGRATIONSWUNSCH 1-3 oben (Part-Farben zeichnen, Applier binden, Feldbreite 20).

### OFFEN (Nachbesserung)
* **N-a** Saeure-Spalte (s. N7): RE2 Op 49 = K1, gebaut K0 nach BAUPLAN E6/§1.6 — Abnahme durch Orchestrator/Nutzer.
* **N-b** Port `re2z_death_main` P2 ohne die RE2-Wiederbelebung (HP 1, @0x80108918-9ec) — bestehende Luecke, alle Waffen.
* **N-c** Setzer von 0x800CFBD8 Bit 0x10000000 (einziger Schreiber `80058df8`, SCD-Opcode-Rumpf @0x80058dac) nicht einem
  Opcode zugeordnet; im Port ohne Produzent (Knockdown- und Sturz-Tod lesen es). Weg: Opcode-Tabelle des RE2-SCD auf
  0x80058dac durchsuchen.
* **H1-H8** der Gegenpruefung (Gator-Boss-Versatz, Hund-Immunitaet 0x80, Kraehe +0x98, Vier-Quadranten, ungepinnte Details
  G02/G05/G08-G11/G15, Treppen-Sonde ueber `re15_game_step`, Kakerlaken-Tabellen STAGE4/5, Unterzustand 13) waren nicht
  Teil dieses Nachbesserungsauftrags und bleiben wie in `bau_b.gegenpruefung.md` §5 beschrieben offen.

### N3b Nachtrag M1 — Art 5 auch im Direktaufruf nicht mehr ueber die GL-Explosiv-Zeile 9
| Datei:Zeile | Inhalt | Beleg |
|---|---|---|
| `enemy_ai_re2_zombie.c` `re2z_row_from_atktype[5]` | 9 -> **10**: Art 5 ist seit O-VB4 das Bodenfeuer, seine RE2-Zeile = die des Op-40-Hitcodes 0x2002000A | `lui a3,0x2002` @0x80020794 / `ori a3,a3,0xa` @0x800207a0 |
| `probe_r34_schaden.c` (39) | Direktaufruf Art 2/3/4/5 -> 9/11/10/**10** | — |
| Mutation **MN13** | `[5]` zurueck auf 9 -> 39 rot | s. N5b |

Im Spiel kommt Art 5 nur ueber den Applier (GL-Bruecke mit der Hitcode-Zeile, N3); der Tabelleneintrag deckt den
Direktaufruf (Zensus, Sonden). Damit laeuft Art 5 nirgends mehr ueber die GL-Explosiv-Zeile 9 (Gegenpruefung M1:
"nicht ueber die Explosivzeile 9").

### N5b Mutationsprobe MN13
| Id | Mutation (Datei) | erwartet rot | Ergebnis |
|---|---|---|---|
| MN13 | `re2z_row_from_atktype[5]` zurueck auf 9 (enemy_ai_re2_zombie.c) | 39 | 39 rot (`atktype 2/3/4/5 -> 9/11/10/9`), zurueckgesetzt, danach gruen |
