# Runde 34 (Granaten) — Bau Spur A: Wurf, Flug, Zuender, Explosion, Item-Debug

Stand: 2026-09-30, Zweig `r34g/a-granate`, Arbeitsbaum `.claude/worktrees/r34g_a`, Basis `r34g/c0-vertrag` (8d8651e4).
Bauverzeichnis `re15_port/build_r34_a`, Laufzeit-Ausgaben (unversioniert) `build/r34g_a/`.
Auftrag: BAUPLAN §3.1 A1-A10, K1-K3, K9, P1-P10, P31; Orchestrator-Vorgaben (O-VB4 entschieden, "alle Gegner").

STATUS: FERTIG (Code + Sonde + Mutationsproben + exe-Messung); Suite: siehe §8.

---

## 0. Arbeitspakete

| Paket | Inhalt | Stand | Datei:Zeile |
|---|---|---|---|
| A1 | Vertragsfelder (C0) + additiv: `re15_esp_granate_spawn`, Messschiene `re15_esp_granate_resolver_calls` | fertig | `include/re15_esp.h:252-263` |
| A2 | Tick in zwei Durchgaengen + Weltlage `wpos` mit Drehung | fertig | `engine/src/re15_esp.c:1332-1469` (Tick), `:1210-1308` (RotMatrix/ApplyMatrix/Weltlage) |
| A3 | Routine 30 (Wurf-Init) + Wort 0x800acaec | fertig | `re15_esp.c:781-838`; `engine/src/player_common.c:292-318` |
| A4 | Routine 29 (Flug/Abprall/Liegen) | fertig | `re15_esp.c:945-948` (Verteiler), `:965-1020` |
| A5 | Routine 31 + FUN_800199d4-Zwilling | fertig | `re15_esp.c:839-930`; Kind-Spawner `:1107-1126`; Spawn-Kern `:1051-1086` |
| A6 | Bodenklemme aus fuer Granatenplaetze + Kinder | fertig | `re15_esp.c:538-543` (`ESP_KEIN_BODEN`), Granate `:1128-1155`, Kinder `:1107-1126` |
| A7 | Routine 9 setzt den Licht-Latch | fertig | `re15_esp.c:709-722` |
| A8 | Spielschritt: ENT[9].resolve 0, Gate 9/10/11 + Art, Gier rot_y; Drehen im Zielen korrigiert | fertig | `engine/src/game_step_common.c:1769-1784`, `:1945-1981`; `player_common.c:949-956`, `:1023-1034` |
| A9 | Sonde `probe_r34_wurf` (7 Abschnitte, 18 Mutationsproben) | fertig | `tests/unit/probe_r34_wurf.c`, `tests/unit/probes/r34_wurf.cmake` |
| A10 | Item-Debug des Statusschirms | fertig | `engine/src/menu_common.c:1254-1337` |
| Diagnose | `RE15_GRANATE_LOG=<datei>` (nur Ausgabe) | fertig | `re15_esp.c:545-556`, `:1310-1330`, Ereigniszeilen in R29/R31/Kind/Spawn |

---

## 1. Was gebaut wurde

### 1.1 A2 — Zwei-Durchgang-Tick (`re15_esp_fx_tick`)
* **Durchgang 1** = Schleife 1 @0x80019e64-c4: Routine A fuer alle aktiven Zeilen-VM-Plaetze mit Flags-Bit 0
  (`lbu v0,108(v1)` / `andi v0,v0,0x1` / `beq` @0x80019e70-7c, `jalr` @0x80019e9c). Ein Kind mit Flags 0x0a
  (Bit 0 frei) wird hier NICHT freigegeben, sondern wartet auf Durchgang 2.
* **Durchgang 2** = Hauptlauf @0x80019ee0-0x8001a49c je Platz:
  (a) Kind-Init `andi v0,v1,0x8` / `xori v0,v1,0x9` (Delay) / `sb v0,108(a0)` @0x80019ef4-f00, Routine A einmal
  (`jalr` @0x80019f30); (b) Lebend-Gate @0x80019f44-50; (c) Follow (Bestand) @0x80019f58-fa4; (d) Weltlage;
  (e) Routine B @0x8001a2b4-d4; (f) Physik nach Neulesen der Flags (`lbu v0,108(a2)` @0x8001a2e8): euler +=
  Winkelgeschw. (@0x8001a2fc-330, NEU, 16 Bit in der Zeilenkopie), xlat += vel, danach vel += acc; die
  Port-Sammelklemme bleibt (ausser `floor_y == ESP_KEIN_BODEN`); (g) Anim @0x8001a38c-47c (Bestand).
* **Weltlage** (V1a, `esp_fx_weltlage`): Flags & 0x80 == 0 → `wpos = (s16)(x + RotMatrix(euler.x, euler.y + Gier,
  euler.z) * (s16)xlat)` (@0x8001a118-2a4); x/y/z traegt im Port Anker.R*Versatz + Anker.T (beim Spawn eingerechnet).
  RotMatrix-Zwilling = FUN_80068098 inkl. **Negativwinkel-Zweig** (siehe §2.2 — die Tabelle 0x800794c4 ist nicht
  punktsymmetrisch, `re15_sin_q12(a & 0xfff)` waere fuer negative Winkel falsch). ApplyMatrix = FUN_800661c0 (MVMVA
  sf=1, MAC1..3 ungesaettigt). Flags & 0x80 != 0: Zweig @0x80019fc8-0x8001a114 gelesen (Welt = Anker.R *
  (RotMatrix(euler) * xlat + Versatz) + Anker.T, RotMatrix OHNE Gier); der Port hat keine Anker-Matrix → dort
  bleibt `wpos = (s16)(x + xlat)` (= die bisherige Zeichenlage, OFFEN 1).

### 1.2 A3 Routine 30, A4 Routine 29, A5 Routine 31, A7 Routine 9 — siehe Konstanten §2.1
* R30 liest das Wort 0x800acaec ueber `re15_player_acaec()` (player_common.c): Zielbits aus der Ziel-FSM
  (s_aim_elev) + `status_flags & 0x1fff`; Test-Haken `re15_player_acaec_override_for_test`.
* R31 fuer Art 3/4 (E8): Flags 0x61, P, Resolver Art 3/4, dann `re15_esp_aufschlag_hook(re2_art, wpos, Gier)` mit
  expliziter Tabelle Art 3 → 2 (Saeure), Art 4 → 1 (Brand); keine HE-Kinder, kein SE 0x04080001, kein Latch.
  Ein Platz ohne `granate_art` (0), der Routine 31 faehrt, gilt als Art 2 (Original `ori a2,zero,0x2` fest).
* FUN_800199d4-Zwilling `esp_fx_spawn_kind`: Start-Flags 0x0a (@0x80019a88), Anker = P, Gier des Elternplatzes,
  erster freier Platz ab 0 — Rauch #2 belegt nachweislich den gerade freigegebenen Granatenplatz (Pruefung 28).

### 1.3 A8 Spielschritt (`game_step_common.c`)
* `ENT[9] = {1,0,1,0}` (P1): kein Sofort-Hitscan mehr im Abzugsbild.
* Spawn-Gate 9/10/11 mit Art 2/3/4 (explizit), Gier = `pl->rot_y`, Spawn ueber `re15_esp_granate_spawn` (floor "kein
  Boden").
* **Drehen im Zielen (player_common.c) — GEMESSEN ABWEICHEND, KORRIGIERT**: vorher liefen Lauf-Drehung (Stehen 96,
  @0x80073ee4) UND Ziel-Drehung (mit vertauschtem Vorzeichen) gleichzeitig: Wurf (Rueckstoss) −72/+72 je Bild
  (Sonde 6c vor dem Fix, `build/r34g_a/probe_schritt1.txt`). Original (selbst disassembliert): RAISE Sub 0 LINKS
  (virtuell 0x8) `subu` Byte0 = 24 (@0x80033000-4c, `addiu at,at,16528` @0x80033028), HOLD Sub 1 Byte1 = 48
  (@0x800333a0-e8), ABZUG Sub 2 Byte1 `srl 1` = 24 (@0x8003355c-fc), LOWER Sub 3 `addiu v0,v0,-24` (@0x80033cf0),
  RELOAD Sub 4 −24 (@0x80033e00); RECHTS (0x2) jeweils plus; `sh` auf 0x800acabe ohne `& 0xfff`. Jetzt: im Zielen
  nur die Ziel-Drehung, LINKS minus / RECHTS plus, 16-Bit-Umlauf wie die Lauf-Drehung. Nachher gemessen −24/+24.
  **Wirkt fuer ALLE Waffen im Zielen** (HOLD mit gehaltenem OBEN/UNTEN dreht jetzt mit 48 statt netto 0; RAISE/
  LOWER/RELOAD/Abzug 24 statt 72).

### 1.4 A10 Item-Debug (`menu_common.c`, Kopf von `item_mode`)
SELECT (rohe Flanke) → Zustand 1 + Id 0 (Halbwort-Store), SE CORE 9; Zustand 1/2 schreibt inv[25bd] = (Id, 255, +2
bleibt, +3 = 0), Zellen-Uebersteuerung = Identitaet (= Bild ITEMALL[Id]); Zustand 3: R1 +1, L1 −1, R2 +10, L2 +246,
Kreis Ende, Dreieck Menge + 1; Kappung Id ≤ 0x47. Laeuft in JEDEM ITEM-Zustand (vor dem 25c2-Sprung).
Nicht portiert: Pad-2-Auffueller @0x8004a0dc-130 (kein zweites Pad im Port).

---

## 2. Konstanten mit Adresse (alle in dieser Sitzung mit `re15_disasm.py` nachgelesen)

### 2.1 Routinen
| Name | Wert | Adresse / Instruktion | Code |
|---|---|---|---|
| R30 Satz / Flags / B / A / Zuender | 0x17 / 3 / 29 / 0 / 42 | `ori v0,zero,0x17; sb v0,110(v1)` @0x80018448-50; `ori v0,zero,0x3` @0x8001845c-60; `ori v0,zero,0x1d; sh v0,2(v1)` @0x8001846c-70; `sh zero,0(v1)` @0x80018478; `ori v0,zero,0x2a` @0x80018474 / `sh v0,30(v1)` @0x8001847c | re15_esp.c case 30 |
| R30 HOCH | v (0x17c, −110, 0x15), acc_x −2 | @0x80018494-a8, `addiu v0,zero,-2` @0x800184b0 → `sh v0,8(v1)` @0x800184dc | " |
| R30 MITTE | v (0x118, −50, 0x18), acc_x −1 | @0x800184bc-d4 | " |
| R30 TIEF | v (0x50, 0, 1), acc_x −1, Zaehler 5 | @0x80018518-38 | " |
| R30 Zaehler | ((a + ((a>>7)&0xff)) & 0xff) % 4 + 7 | `lhu a0,-13588(a0)` @0x80018484; RNG @0x8001af30-4c; `addiu v0,v0,7` @0x80018504, `sh v0,38(a0)` @0x8001850c | " |
| acaec-Schreiber | RAISE 0x4000, HOCH 0x8000, TIEF 0x2000, MITTE 0x4000, LOWER keine | @0x80032f98-a4, @0x80033228-38, @0x80033270-84, @0x800332bc-c8, @0x80033cc0-c8 | player_common.c `re15_player_acaec` |
| R29 Boden | Welt-y > 0 | `lh t1,42(t0)` @0x80018330, `blez` @0x80018338 | re15_esp.c `esp_fx_dispatch_b_29` |
| R29 Liegen | SE 0x010A0001, Flags 0x63, A 31, B 0 | @0x80018350-58, @0x80018368-6c, @0x80018378-7c, @0x80018384 | " |
| R29 Abprall | vx −= vx/3, Zaehler −1, xlat_y −= y, vy := −(vy/3), SE 0x010A0001\|(n<<8) | @0x80018388-cc, @0x800183d0-d4, @0x800183c4/dc/e0, @0x800183e4-f8, @0x80018410-28 | " |
| R31 Explosion | Zuender == 7; Latch; Flags 0x61; P = (x, y−500, z); Resolver(500, P, Art) | @0x8001856c-70; @0x8001857c; @0x80018580-84; @0x80018594-bc; @0x80018598/a4/b4/b8 | re15_esp.c case 31 |
| R31 Kinder | 0x03195000 (Z7 + Z2), 0x030B5400 (Z2), 0x030B5800 (Z0, nach `sb zero,108(a1)`) | @0x800185c0-dc, @0x8001860c-40, @0x80018648-60, @0x80018698/a8, @0x800186b0, @0x800186c8 | " |
| R31 SE | 0x04080001 an P | @0x800185e4-ec | " |
| R31 Abzug | Zuender −1 | @0x8001867c-84 | " |
| R9 Latch | 0x800b5358 := 1 nach dem SE | `ori v0,zero,0x1` @0x8001768c, `sb v0,21336(at)` @0x80017694, Vorschub @0x80017698 | re15_esp.c case 9 |
| Kind-Spawner | Start-Flags 0x0a | `ori v0,zero,0xa` @0x80019a88 / `sb v0,108(t0)` @0x80019aa4; Suche ab 0 `sltiu v0,t3,0x60` @0x80019a60 | `esp_fx_spawn_kind` |
| Resolver-Art 3/4 | 1000/1000, Reaktion 10/11 | `read 0x8006f418 11 --w 2 --signed` = [10,20,1000,1000,1000,50,...], `read 0x8006f430` = [3,3,9,10,11,14,...] (C0-Dossier §3.2) | R31 (E3) |

### 2.2 Weltlage, Mathematik
| Name | Beleg |
|---|---|
| Weltlage Flags&0x80 == 0 | `lhu v0,32(a1)` / `lhu v0,34(a1)` + `lhu v1,46(a1)` / `addu` @0x8001a16c-84; `jal 0x80068098` @0x8001a1a0; `jal 0x800661c0` @0x8001a1e4; `sh v0,40/42/44(a0)` @0x8001a1fc/10/20; Anker @0x8001a248-2a4 |
| Weltlage Flags&0x80 | @0x80019fc8-0x8001a114: `jal 0x80068098` mit a0 = Platz+0x20 (ohne Gier) @0x8001a020, xlat → r, r + Versatz, `jal 0x800661c0` mit a0 = Platz+0x4c @0x8001a0c8 |
| Physik | euler @0x8001a2fc-330, xlat/vel @0x8001a324-388 |
| RotMatrix | Tabelle 0x800794c4 (`lw t9,-27452(t9)`), Negativzweig `subu t7,zero,t7` @0x800680a8 / `subu t3,zero,t8` @0x800680d0; Eintraege `sh` @0x8006816c/80/94, @0x8006820c/2c, @0x80068274/a4/ec, @0x80068318 |
| Tabellen-Asymmetrie (gemessen, `build/r34g_a/trigsym.py`) | tab[4095].sin = 0, tab[4094].sin = −6 = −tab[1].sin; in 3888 von 4095 Indizes ist sin(4096−k) ≠ −sin(k) |
| ApplyMatrix | Rohworte @0x800661c0: ctc2 0x48c80000..0x48cc2000, lwc2 0xc8a00000/0xc8a10004, MVMVA 0x4a486012 (sf=1, RT, V0, cv=3), swc2 MAC1..3 0xe8d90000..0xe8db0008 |

### 2.3 Spielschritt / Item-Debug
| Name | Wert | Beleg |
|---|---|---|
| Entlade 9/10/11 | nur Munition | Tabelle `table 0x80074100` [9]/[10]/[11] = 0x80033b38/58/78; `jal 0x8004eae4` @0x80033b40/60 |
| Spawn-Gate Original | nur Id 9 | `lbu v1,-13731(v1)` / `ori v0,zero,0x9` / `bne v1,v0,0x800337ac` @0x80033684-8c |
| Waffen-Parametersatz 9/10/11 | `18 30 0a 01 00` | `bytes 0x80074090 64`: @0x800740b8, @0x800740bd, @0x800740c2 |
| Item-Debug | siehe §1.4 | @0x8004a134-15c, @0x8004a160-18c, @0x8004a194-a0, @0x8004a1a4-22c, @0x8004a238-35c |
| Datei 0xb | 86400 B = ITEMALL.PIX | Dateitabelle `read 0x8006f43c 16 --w 4 --stride 8` Index 11; Lader FUN_80013b60 `sll a0,a0,3` @0x80013b70, `lw v0,0(at)` @0x80013ba0 |
| UTILITY MENU | nur aus der Spielschleife | einziger Aufrufer `jal 0x8001443c` @0x8001c988 (Scan `build/r34g_a/jal_scan.py`) |

---

## 3. Sonde `probe_r34_wurf` (ctest `unit_r34_wurf`)

Echte Engine, CORE00.ESP als globale Bank, Spione auf `re15_esp_se_hook` / `re15_esp_aufschlag_hook`, Erwartung =
Simulator der Gegenpruefung (`re_wurf_gegen_werkzeug/wurf_sim_gegen.py`, Ausgabe `build/r34g_a/sim_extra.py`).
Rueckgabe = erste fehlgeschlagene Pruefung (alle Nummern < 256).

| Abschnitt | Pruefungen | Inhalt |
|---|---|---|
| 1 MITTE gesund | 11-30 | Spawn (Art 2, Flags 3, CLUT 0x7B11, TPAGE 0x1F); Bild 0 xlat (280,−50,24), vel (279,−40,24), acc (−1,10,0), Zuender 42, Zaehler 7, A 0, B 29, Satz 23, wpos = Spawnpunkt; Bild 1 Satz 24; L 73 / X 109 / Z2 114 / frei 116; xlat bei L (11929, 2483, 1752); Liegestelle Welt-y 9; SE-Folge 0x010A0601..0x010A0001 (Bilder 29/47/55/60/64/67/70/73, Eindringtiefen 136/90/16/25/16/3/9/9) + 0x04080001 an P (Bild 109); genau 1 Resolver-Aufruf, erst in Bild 109; Latch genau 1x in Bild 109; Dummy 0x27 180 → −820, +4 = 3, +5 = 9, +6 = 1; Spieler 900 entfernt 100 → −900; Kind 0x03195000 im Explosionsbild initialisiert (Flags 0x13, Satz 10, Skala 0x5000, CLUT 0x78D1, TPAGE 0x1E, Anker = wpos = P); Bild 114 Feuerball #2 + Rauch #1 (0x5400, Flags 0x13, Satz 8, CLUT 0x7851, TPAGE 0x5E, xlat_y −135, vel_y −130); Bild 116 Rauch #2 (0x5800) AUF dem Granatenplatz; Feuerball #1 weg in Bild 122 (X..X+12), Rauch #1 weg in Bild 129 (X+5..X+19) |
| 2 Zeitlinien | 31-50 | HOCH gesund 88/124/129/131, 8 Kontakte, xlat (18760,3180,1848); TIEF 40/76/81/83, 6, (1543,792,40); HOCH vergiftet 94/130/135/137, 10, (18721,3180,1974); MITTE vergiftet 79/115/120/122, 10, (11938,2483,1896) |
| 3 Gier 1024 | 51-55 | L/X unveraendert, Liegestelle = Spawn + (1752, ., −11929); Negativ: Gier 0 endet anders |
| 4 0x0A / 0x0B | 56-65 | X 109 / frei 116; Resolver 1x, Schaden erst in Bild 109; Dummy +5 = 10 / 11; Aufschlag-Spion 1x (re2_art 2 / 1, q = wpos, Gier 777); keine HE-Inhalte (0 SE 0x04080001, Latch 0, 0 Kinder), 8 Kontakt-SEs |
| 1 (Nachtrag) | 81-82 | Bild 73: Flags 0x63, A 31, B 0, sichtbar; Bild 109: Flags 0x61, unsichtbar, Zuender 6 |
| 5 Rand | 71-80 | Pool voll → NULL, kein Granatenplatz; Raumwechsel nach dem Liegen: 0 Resolver, Dummy 180, Pool leer; a ohne Zielbits → senkrechter Fall, 1 Kontakt; Spieler 1000 entfernt unversehrt, 949 entfernt −900 (R = 950 streng); Huelse behaelt Klemme (floor_y 60); Routine 9 Latch 0 → 1 |
| 6 Spielschritt | 91-114 | ROOM1140, re15_game_step + ESP-Tick danach: Abzug Bild 0, Menge 5 → 4; Zombie 700 vor Leon (in Hitscan-Reichweite 1000) unversehrt; Spawn A+22 mit Art 2/3/4 fuer Waffe 9/10/11, Gier 0, Anker = Knochen + {0,0,0x1f4}; R1 los ab Bild 15 → kein Spawn, Menge 4; R1 nur in Bild 3..8 los → Spawn 22 (Negativ); Drehen LINKS/RECHTS je Bild −24/+24, Spawn-Gier 1000 ∓ 528 |
| 7 Item-Debug | 131-148 | Raster offen; R1 ohne SELECT wirkungslos; SELECT → Zustand 3, Platz 0 = 00 ff, +3 = 0, SE CORE 9 1x; 9x R1 → 09 ff; R1-Flanke schreibt erst im Folgebild; 10x → 0A, 11x → 0B; L1/R2/L2; Kappung 0x47 (Unter-/Ueberlauf); Dreieck Menge +1; Kreis beendet; Schliessen ruestet 9 aus; SELECT im Reiter-Modus wirkungslos |

Laeufe: `build/r34g_a/probe_lauf6.txt` (ALLE PRUEFUNGEN GRUEN). Bauverzeichnis der Sonde = `build_r34_a` (Orchestrator-
Vorgabe; der BAUPLAN nannte `build_r34_wurf`).

### 3.1 Mutationsproben (`build/r34g_a/mutation.py`, Ergebnis `build/r34g_a/mutation_ergebnis.txt`)
Je Mutation: Datei sichern, eine Stelle aendern, Sonde bauen + laufen, Datei byte-gleich zuruecksetzen (vom Skript
geprueft), am Ende Neubau + Lauf gruen (`ZURUECK: bau rc=0, sonde rc=0 (GRUEN)`), `git status` sauber.

| Nr | Mutation | Ergebnis (erste Pruefung) |
|---|---|---|
| M1 | R30 MITTE vx 0x118 → 0x117 | rot 12 (+15,18,19,49,53,54) |
| M2 | R29 vx/3 → vx/2 | rot 18 (+19,34,39,44,49,53,54,77) |
| M3 | R31 Explosion bei Zuender 8 | rot 21 (+22..29,57,59,62,64) |
| M4 | Sammel-Bodenklemme fuer die Granate an | rot 17 (43 Pruefungen) |
| M5 | Weltlage ohne Drehung | rot 54 (+55) |
| M6 | Kind-Init Flags&8 entfernt | rot 26 (+27..30) |
| M7 | Routine 9 ohne Latch | rot 80 |
| M8 | Zaehler-Formel +6 statt +7 | rot 13 (31 Pruefungen) |
| M9 | Resolver-Art immer 2 | rot 58 (+63) |
| M10 | ENT[9].resolve wieder 1 (Bruecke) | rot 93 (erst nach Umbau 6a auf Zombie 700; mit Dummy 0x27 in 1299 blieb sie gruen — Reichweite 1000) |
| M11 | Spawn-Gate nur Id 9 | rot 100 (+101,102,106,107,108) |
| M12 | Spawn-Gier 0 statt rot_y | rot 112 (+114) |
| M13 | Lauf-Drehung im Zielen wieder an | rot 111 (+112..114) |
| M14 | Item-Debug Kappung 0x49 | rot 143 |
| M15 | SELECT setzt Id nicht zurueck | rot 145 (+146,147) |
| M16 | R31 Platz frei NACH dem Kind | rot 28 |
| M17 | R29 Liegen-Flags 0x23 statt 0x63 | rot 81 |
| M18 | R31 Explosions-Flags 0x63 statt 0x61 (sichtbar) | rot 82 |

---

## 4. Messungen an der echten exe (`build_r34_a/platform/pc/re15_pc.exe`, Skript `build/r34g_a/lauf_a.sh`)

Beschleunigter Renderer, `RE15_NOAUDIO=1`, ROOM1140 per `RE15_DEBUG_JUMP=1140@250`, `RE15_INPUT_SCRIPT_BASIS=spiel`,
Bilder per `RE15_FRAMEDUMP` (kein AUTOSHOT/SOFTWARE_RENDER).

* **a_wurf1** (`RE15_GIVE=9:5 RE15_EQUIP=9`, `W1,M1,MA0.2,M2.5,W4`): `gr.log`: Spawn F=382 (Anker (−6851,−2474,−18279),
  Gier 215 = die vom Heben auf den naechsten Zombie gedrehte Blickrichtung), Routine 30 im Tick F=383 (ESP-Tick laeuft in
  main.c noch VOR dem Spielschritt → 1 Bild spaet, INTEGRATIONSWUNSCH 1), erster Kontakt F=412 (= 383 + 29), Liegen
  F=456 (+73), Explosion F=492 (+109), frei F=498; xlat bei L (11929, 2483, 1752) = Sonde/Simulator. Kein Absturz, rc 0.
* **a_debug1** (Standard-Inventar, `S0.1,W2,A0.1,W1,E0.1,W0.3,9x(M0.1,W0.2),X0.1,W1,S0.1,W3,M1,MA0.2,M2.5,W5`): das
  Item-Debug liefert die Granate (debug.log `[equip] W-bank -> W09`, Wurf F=667, Explosion, frei). **Befund**: dieselbe
  SELECT-Flanke oeffnet zusaetzlich das UTILITY/DEBUG MENU (debug.log `[debug-menu] OPEN (frame 396)`, Bilder 396-480
  in `build/r34g_a/a_debug1/debug_sheet.png`: "DEBUG MENU / UTILITY MENU / JUMP 114 BRIEFING ROOM" ueber dem Inventar)
  → INTEGRATIONSWUNSCH 6.
* **a_tief1 / a_tief2** (TIEF nach Vorwaertsgehen): TIEF-Anker y −772, xlat bei L (1543, 792, 40) exakt; kein Zombie im
  Radius (Treffer per Skript nicht erreicht — die Trefferwirkung belegt die Unit-Sonde; die exe-Trefferabnahme gehoert
  dem Integrationstest `test_r34_granaten`). Beide Laeufe endeten mit rc 137 NACH `[flow] EXIT_AT ... -> exit`
  (Prozessende haengt; bekanntes Muster, Memory reai-v2-flaky-testhaken-exit), Daten vollstaendig.

---

## 5. Abweichungen vom BAUPLAN / Entscheidungen (mit Grund)

1. **Drehen im Zielen korrigiert** (A8 erlaubte das bei gemessener Abweichung): wirkt fuer alle Waffen, nicht nur die
   Granate (§1.3). Byte-true nach Gun-FSM Sub 0..4; `& 0xfff` in der Ziel-Drehung entfernt (Original `sh` ohne Maske).
   Die Auto-Nachfuehrung beim Heben maskiert weiterhin (nicht untersucht, OFFEN 5).
2. **Weltlage Flags&0x80**: Zweig gelesen, aber ohne Anker-Matrix nicht nachbaubar → `x + xlat` (OFFEN 1).
3. **R31 ohne granate_art = Art 2** (Original-Konstante `ori a2,zero,0x2`).
4. **Anim-Satz in R30** mit der Port-Konvention "Satz − 1, Zeitgeber 0" (wie R5/R10); gleichwertig, weil Satz 0 (Spawner-
   +0x6d) und Satz 23 Dauer 1 tragen (CORE00.ESP @0x1730 bzw. @0x17E8 `10 01 01 10`); Sonde 13/15 belegt Satz 23 im
   Spawnbild und 24 im Folgebild.
5. **Kinder ohne Boden**: auch die Kinder (Feuerball/Rauch) bekommen `ESP_KEIN_BODEN` (Original-Tick ohne Klemme).
6. **Liegen-SE-Lage = Granatenlage** (E14, Port-Wahl, gekennzeichnet).
7. **Messschiene** `re15_esp_granate_resolver_calls()` (Zaehler, kein Verhalten) fuer "genau EIN Resolver-Aufruf".

---

## 6. Hinweise an die Spuren

* **B**: Aufruf `re15_resolve_attack(&{P.x, P.y, P.z, 500}, Art 2/3/4, -1)` genau einmal im Zuender-7-Bild; P steht in
  `atk->x/y/z` (Peilquelle fuer den RE2-Stempel, B3). Der Spieler-Zweig wird mitgetroffen (Pruefung 25/78).
* **C**: `wpos` ist jetzt fuer ALLE Plaetze gueltig (Flags&0x80: x + xlat wie bisher). Fuer Plaetze mit Gier ≠ 0 und
  xlat ≠ 0 (Granate; auch W14-Flammenstrahl param 3000, Huelsen haben param 0) weicht `wpos` von `x + xlat` ab — das
  ist die Original-Lage. Latch-Setzer: R31 (nur Art 2) und R9 (jeder Schuss). SE-Codes: 0x010A0001|(n<<8) (Bank 1
  Satz 0x0A), 0x04080001 (Bank 4 Satz 8).
* **D**: Aufschlag-Haken bekommt q = Granaten-Weltlage (nicht P), Gier = Platz +0x2e, re2_art 2/1.

## INTEGRATIONSWUNSCH (fremde Dateien — NICHT geaendert)

1. `platform/pc/main.c:5413` (C1/E10): `re15_esp_fx_tick` hinter `re15_game_step` (`main.c:7380`) — gemessen (a_wurf1):
   Routine 30 laeuft sonst ein Bild nach dem Spawn (Spawn F=382, R30 F=383).
2. `platform/pc/main.c:236-452` `pc_draw_effects` (C2): Lage und Region-Cull aus `f->wpos` statt `f->x + f->xlat_x`;
   sonst fliegt das gezeichnete Granaten-Sprite ungedreht (entlang lokal +x), waehrend Explosion/Kinder an der
   gedrehten Weltlage sitzen (a_wurf1: Gier 215).
3. `platform/pc/main.c` (C3): Latch-Leser/-Loescher fuer `g_re15_licht_latch` (@0x8001ce60 / @0x8001d1b4).
4. `platform/pc/src/audio_pc.c` / `main.c` (C4): `re15_esp_se_hook` binden.
5. `platform/pc/main.c` (C/D): `re15_esp_aufschlag_hook = re2fx_aufschlag`.
6. `platform/pc/main.c:7170-7171` (UTILITY MENU): `if (gctx.pad_pressed & RE15_PAD_BIT_SELECT)` zusaetzlich an
   `!re15_menu_gameplay_frozen()` binden. Beleg: FUN_8001443c hat genau einen Aufrufer `jal 0x8001443c` @0x8001c988 in
   der Spielschleife (Task 0), die waehrend des Statusschirms geparkt ist (menu_common.c Stufe 2, Task-Start
   @0x8001cb34-44). Gemessen (a_debug1): SELECT im ITEM-Raster oeffnet im Port BEIDE (Item-Debug und DEBUG MENU).
7. Integrationstest `test_r34_granaten` (C9): Item-Debug-Weg als exe-Abnahme (Skript wie a_debug1, nach Wunsch 6).

## OFFEN

1. **Weltlage Flags&0x80** (Muendungsfeuer u. a.): Original = Anker.R·(RotMatrix(euler)·xlat + Versatz) + Anker.T
   (@0x80019fc8-0x8001a114); der Port fuehrt keine Anker-Matrix (Waffenknochen) je Platz. Naechster Schritt:
   Knochenmatrix beim Spawn in den Platz kopieren (`re15_player_gunbone_world` liefert R und T), dann den Zweig bauen.
2. **Liegen-SE-Lage** (Original Stapelrest sp+16, E14) — unveraendert O2 des BAUPLANs.
3. ~~Zustands-Schreiber des RNG~~ → **GESCHLOSSEN**: Xref-Scan (`build/r34g_a/xref_imm.py 0x800ac774`, lui-Paare,
   PSX.EXE + STAGE1..6.BIN) findet genau zwei Stellen: FUN_8001af20 selbst (`addiu v0,v0,-14476` @0x8001af24; das
   geladene `lhu t1` @0x8001af28 wird nie gelesen) und den Seed `ori v0,zero,0x1c3` / `sw v0,-14476(at)` @0x8003162c-34
   (Spieler-Init). Kein Overlay-Verweis. Der Schreiber `sw a0,0(v0)` @0x8001af48 aus Routine 30 hat also keinen Leser —
   ohne beobachtbare Wirkung; der Port muss ihn nicht fuehren.
4. **Item-Debug-Nebenwirkung**: Datei 0xb (ITEMALL) ueberschreibt 0x801a0000, wo sonst MIXITEM fuer die EXCHANGE-Bilder
   liegt — im selben Menue-Lauf zeigte das Original danach falsche Kombinationsbilder. Nicht nachgebaut. Pad-2-Auffueller
   @0x8004a0dc-130 nicht portiert (kein zweites Pad). Breite Waffen (+2 = 1/2) behalten beim Debug ihre Anzeigeart.
5. **Auto-Nachfuehrung beim Heben** (`player_common.c` Ziel-Block, `& 0xfff`): Maskierung gegen FUN_8001a8f8 nicht geprueft.
6. **exe-Treffer** einer geworfenen Granate an einem Zombie per Skript nicht erreicht (Zombies 1140 stehen/wandern
   ausserhalb der Wurfweiten); Abnahme im Integrationstest.

---

## 7. Commits (Zweig `r34g/a-granate`)

b9ee0468 (A2-A8), 690c2053 (Sonde 1-5), 8aca11ef (Drehen + Sonde 6), a9a772da (A10 + Sonde 7), 76894e74 (R9-Latch-
Pruefung), 5239b817 (Sonde 6a Zombie 700), + Dossier-Commits.

## 8. Suite

* **Lauf 1** (`local_build.sh` all): Bau-Abbruch `ld: cannot open output file platform\pc
e15_pc.exe: Permission denied`
  — die exe eines eigenen Messlaufs (a_tief2) war noch nicht beendet. Keine eigene re15_pc.exe lief danach mehr
  (Prozessliste per Win32_Process; nur fremde Baeume r34g_c/r34g_d + Hauptbaum des Nutzers, NICHT angefasst).
* **Lauf 2** (all, 429 Tests = 428 + unit_r34_wurf): `99% tests passed, 2 tests failed out of 429`, 1315.61 s.
  Rot: `integration_r30_granate_laden` (Lauf [c] abgerissen, exit=1, debug.log endet nach Bild 96) und
  `integration_r30_irons_tisch_bild` (Lauf P0 exit=1 vor dem Laden des Spielstands, debug.log endet nach dem
  Fensteraufbau). Parallel liefen die Suiten der Spuren C und D (je ctest + re15_pc.exe).
  Einzeln wiederholt: `irons_tisch_bild` **gruen** (65.44 s); `granate_laden` 3x: rot ([a] abgerissen nach Bild 240,
  exit=1), rot ([d] Spielstand nicht geladen, exit=1), **gruen** (116.93 s). Der Lauf [a] von Hand nachgestellt
  (`build/r34g_a/gl_a`, gleiche Umgebung): rc 0, `[granate] Modal auf`, `EXIT_AT: Bild 280`. exit=1 an wechselnden
  Stellen (Start, Bild 96, Bild 240..280, Spielstand-Laden) ohne Meldung passt zu keinem Pfad der exe (SDL_QUIT ->
  exit(0) render_pc.c:746-747, Absturz -> 0xC0000005); die Fehlerbilder beruehren keinen geaenderten Code-Pfad
  (kein SELECT, keine Waffe 9-11 im Zielen, keine Granate im ESP-Pool). -> Last-Flattern, kein reproduzierbares Rot.
* **Lauf 3** (all, nach Pruefung 81/82): `99% tests passed, 2 tests failed out of 429`, 1110.13 s. Rot diesmal ANDERE
  exe-Tests: `integration_r30_sicherung_laden` ([b] abgerissen nach Bild 180, exit=1) und `integration_r30_titel_puls`
  (Wanduhr-Periode 1977925 us ausserhalb der Toleranz 66155 us — Zeitmessung unter Last). Die beiden Rot aus Lauf 2
  waren hier gruen. Einzeln: `titel_puls` gruen (41.64 s); `sicherung_laden` einmal rot (exit=1, [b]), dann per
  ctest zusammen mit `granate_laden` beide **gruen** (65.01 s / 145.29 s). Lauf [b] von Hand 3x nachgestellt
  (`build/r34g_a/sl_b`): 3x rc 0, je `EXIT_AT: Bild 280`, 26-27 s. Waehrenddessen liefen die Suiten der Spuren B, C
  und D (Prozessliste: ctest + re15_pc.exe aus r34g_b/r34g_c/r34g_d).
  -> Kein reproduzierbares Rot; jedes Rot wurde einzeln gruen wiederholt.
* **Lauf 4**: siehe unten.
