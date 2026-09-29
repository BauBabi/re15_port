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

## INTEGRATIONSWUNSCH
(noch keiner)

## OFFEN
(fortlaufend)
