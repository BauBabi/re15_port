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

## INTEGRATIONSWUNSCH
(noch keiner)

## OFFEN
(fortlaufend)
