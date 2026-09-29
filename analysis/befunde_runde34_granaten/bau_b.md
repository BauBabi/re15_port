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

## INTEGRATIONSWUNSCH
(noch keiner)

## OFFEN
(fortlaufend)
