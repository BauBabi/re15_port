# Runde 34 (Granaten) — Bau B: Schaden und Gegnerreaktion aller Typen

Stand: 2026-09-29, Zweig `r34g/b-schaden`, Arbeitsbaum `.claude/worktrees/r34g_b`, Basis `r34g/c0-vertrag` (8d8651e4).
Auftrag: BAUPLAN §3.2 B1-B12, E4-E7, E13, E16, K4-K7, P14-P27; O-VB4 vom Orchestrator entschieden
(Bodenfeuer an Gegnern OHNE RE2-KI = RE1.5-Angriffsart 5 "Flaechenfeuer", 50 @0x8006f422 / Reaktion 14 @0x8006f435).
Bauverzeichnis `re15_port/build_r34_b`; Laufzeit-Ausgaben `build/r34g_b/` (unversioniert).

STATUS: IN ARBEIT (fortlaufend geschrieben).

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
| 31 | Explosion RE2-Zombie HP 80: -120, Zustand 3, +0x5 9, +0x6 0, +0x1D2 0, +0x1D3 15, gl_stamp, Reserve 13 | gruen |
| 32 | Richtung aus P (0x21 / 0x01) | gruen |
| 33 | Saeure -> Zeile 11, Brand -> Zeile 10 | gruen |
| 34 | Brad 0x11 HP 250 -> 50, HURT, Zeile 9, Spalte 0 | gruen |
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

## INTEGRATIONSWUNSCH
1. **Part-Farben/-Flags des Hundes und der Spinne zeichnen** (Spur C/D, `platform/pc/main.c` ~9717): `re15_re2z_gore_resolve`
   bedient nur die Zombie-Familie (`re15_re2z_owns_type`). Hund (17 Parts) und Spinne (20 Parts) tragen jetzt die
   Original-Farbworte in `re2z_part_tint[]` (0 = nicht gesetzt, KEIN Neutralwert 0x00808080) und die Wurf-Flags in
   `re2z_part_flags[]`. Wunsch: fuer Typ 0x20/0x25 (RE2-Flavor) das Farbwort als NCCT-Faktor wie beim Zombie
   (`ldrgb` @0x80027C2C / NCCT @0x80027D10), Wert 0 -> neutral.

## OFFEN
(fortlaufend)
