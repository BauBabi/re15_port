# Runde 34 (Granaten) — Bau Spur A: Wurf, Flug, Zuender, Explosion, Item-Debug

Stand: 2026-09-30, Zweig `r34g/a-granate`, Arbeitsbaum `.claude/worktrees/r34g_a`, Basis `r34g/c0-vertrag` (8d8651e4).
Bauverzeichnis `re15_port/build_r34_a`, Laufzeit-Ausgaben (unversioniert) `build/r34g_a/`.
Auftrag: BAUPLAN §3.1 A1-A10, K1-K3, K9, P1-P10, P31; Orchestrator-Vorgaben (O-VB4 entschieden, "alle Gegner").

STATUS: FERTIG (Code + Sonde + 18 Mutationsproben + exe-Messung); Suite: kein reproduzierbares Rot, Zielzeile unter Fremd-Kills nicht erreicht (§8).
NACHBESSERUNG (Gegenpruefung `bau_a.gegenpruefung.md` M-1..M-5, H-1..H-5): §9 — M-2/M-3/M-4 + H-1 gebaut, 11 neue Mutationsproben, M-1 ohne Fremd-Kill
(Wrapper `build/r34g_a/lb.sh`, INTEGRATIONSWUNSCH 8 mit fertigem Patch). Suite §9.6: 413/413 Nicht-exe-Tests in zwei Laeufen gruen, alle 16 exe-Tests
gruen im Beweislauf mit umbenannter byte-gleicher exe; Zielzeile `LOCAL-BUILD-OK (all)` in drei Laeufen (5/9/2 exe-Rot) wegen fremder
`taskkill /IM` NICHT erreicht.

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
  ist die Original-Lage. **Ergaenzung (Gegenpruefung H-2):** auch die Blut-Stroeme des RE2-Zombies
  (`enemy_ai_re2_zombie.c:1113`, param = yaw) und des Hundes (`enemy_ai_re2_dog.c:376-378`, param = rot_y + off) sind
  Zeilen-VM-Plaetze mit xlat ≠ 0 und Gier ≠ 0 — ihre gezeichnete Flugrichtung aendert sich mit C2 (Original: RotY(+0x2e)
  wirkt auf xlat, @0x8001a16c-1a0); Blut-Pins muessen das in C2 erwarten. Latch-Setzer: R31 (nur Art 2) und R9 (jeder Schuss). SE-Codes: 0x010A0001|(n<<8) (Bank 1
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
8. ⛔ `re15_port/tools/local_build.sh:293-300` (Integration, DRINGEND): powershell per absolutem Pfad aufrufen
   (`/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe`) und den Rueckfall `taskkill //F //IM re15_pc.exe`
   streichen (lieber gar nicht beenden als alle). Gemessen: unter CLEAN_PATH (`:147`) fehlt powershell, jeder
   build-Schritt beendet jede re15_pc.exe der Maschine (Waechter: 9 Fremd-Kills in 12 min, §8). Betroffen sind
   `build`, `test` (`do_build && do_test`, `:360`) und `all` (`:361`). **Fertiger Patch** (Ersatz fuer `:293-300`) und
   die Zwischenloesung ohne Aenderung der Datei: §9.1.

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
5. ~~Auto-Nachfuehrung beim Heben~~ → **GESCHLOSSEN** (Nachbesserung H-1, §9.5): FUN_8001a8f8 selbst gelesen, Schritte
   ohne Maske, Richtungsgrenze um +s verschoben; `player_common.c:1068-1074`, Sonde 128-130/153.
6. **exe-Treffer** einer geworfenen Granate an einem Zombie per Skript nicht erreicht (Zombies 1140 stehen/wandern
   ausserhalb der Wurfweiten); Abnahme im Integrationstest.

---

## 7. Commits (Zweig `r34g/a-granate`)

b9ee0468 (A2-A8), 690c2053 (Sonde 1-5), 8aca11ef (Drehen + Sonde 6), a9a772da (A10 + Sonde 7), 76894e74 (R9-Latch-
Pruefung), 5239b817 (Sonde 6a Zombie 700), + Dossier-Commits.
NACHBESSERUNG: 8a33ddbd (M-2/M-3/M-4 Code + Sonde), 82205b96 (H-1 Code + Sonde), 356d331c und folgende (Dossier §9).

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
* **Lauf 4** (all): `98% tests passed, 7 tests failed out of 429`, 1025.45 s. Rot nur exe-Tests: `integration_r30_cut_blitz`,
  `integration_elza_vollstart` (Szene im Zeitlimit nicht fertig), `integration_r30_granate_laden`, `..._irons_tisch_bild`,
  `..._irons_tisch_licht`, `..._sicherung_bild` (exit=1 bzw. abgerissen), `integration_r33_speichern` (exit=1).
  Einzeln nacheinander (`build/r34g_a/rerun_lauf4.txt`): die ersten sechs **gruen**; `r33_speichern` 3x rot (exit=1,
  Abriss an wechselnden Stellen: Bild 120, gleich nach dem Start), dann **gruen** (92.08 s). Von Hand 3x nachgestellt
  (`build/r34g_a/r33a`, gleiche Umgebung ohne ctest): 3x rc 0, `EXIT_AT: Bild 460`, Hinweis-Zeile vorhanden.
* **URSACHE DER exit=1-ABRISSE, GEMESSEN**: ein Prozess-Waechter (`build/r34g_a/watch_kill.ps1`, fragt je 150 ms nach
  taskkill.exe) fing waehrend eines roten r33_speichern-Laufs
  `02:16:13.099 KILL pid=4248 cmd=C:\Windows\System32	askkill.exe /F /IM re15_pc.exe parent=26880`
  (`build/r34g_a/watch_kill.txt`) — eine FREMDE Sitzung beendet re15_pc.exe per Bildname (taskkill /F -> exit 1) und
  trifft damit die exe-Tests aller Baeume. Kein Absturz: das Anwendungs-Ereignisprotokoll der letzten 2 h enthaelt keinen
  Eintrag zu re15_pc.exe. Der Elternprozess 26880 war beim Nachsehen schon beendet (nicht zuordenbar).
* **Lauf 5** (all, Waechter aktiv): `99% tests passed, 5 tests failed out of 429`, 978.61 s. Rot: `irons_tisch_bild`,
  `irons_tisch_licht`, `titel_puls`, `boot_bg_pin`, `relatch_pin`. Einzeln (`build/r34g_a/rerun_lauf5.txt`): `boot_bg_pin`,
  `relatch_pin` gruen; `irons_tisch_bild`, `titel_puls` im zweiten Anlauf gruen; `irons_tisch_licht` im dritten Anlauf
  gruen (der zweite wurde nachweislich von `02:39:14.957 taskkill /F /IM re15_pc.exe` getroffen). Waechter-Log
  `build/r34g_a/watch_kill_lauf5.txt`: 9 Fremd-Kills zwischen 02:27:57 und 02:39:14, Elternprozesse u. a.
  `bash.exe re15_port/tools/local_build.sh test` und `... local_build.sh build` (parallele Spuren).
* **ERGEBNIS**: kein Test ist reproduzierbar rot. Alle Nicht-exe-Tests (Unit, Sonden inkl. `unit_r34_wurf`, Property)
  waren in JEDEM der Laeufe 2-5 gruen; jedes exe-Rot wurde einzeln gruen wiederholt. Eine Zielzeile
  `=== LOCAL-BUILD-OK (all) — Tests 429/429` war unter den Fremd-Kills nicht erreichbar.
* ⛔ **SELBSTBEFUND — local_build.sh toetet fremde exe-Laeufe (auch meine Laeufe haben das getan)**:
  `tools/local_build.sh:293-300` will seit Runde 31 nur die exe des EIGENEN Bauverzeichnisses beenden
  (`Get-Process re15_pc | Where Path ... | Stop-Process`), prueft dafuer aber `command -v powershell` — unter dem
  CLEAN_PATH des Skripts (`:147`: msys64, CMake, Ninja, /usr/bin, System32, Windows, Wbem) liegt powershell.exe
  NICHT (es liegt in `System32\WindowsPowerShell1.0`). GEMESSEN mit genau diesem PATH: `powershell: FEHLT`,
  `cygpath: /usr/bin/cygpath`, `taskkill: /c/Windows/System32/taskkill`. Damit laeuft IMMER der Rueckfall
  `taskkill //F //IM re15_pc.exe` = jede re15_pc.exe der Maschine (alle Baeume UND die exe des Nutzers im Hauptbaum)
  wird in jedem `build`-Schritt beendet. Meine eigenen Aufrufe (build 0/1/2 und die Suite-Laeufe 1-5 = 8
  build-Schritte, 2026-09-30 00:05-02:25) haben das ebenso ausgeloest; die exe des Nutzers
  (`C:\workspace\git
eAi_v2
e15_portuild\platform\pc
e15_pc.exe`, pid 28632, um 00:47 laufend) war um
  01:17 nicht mehr in der Prozessliste. -> INTEGRATIONSWUNSCH 8. Seit dem Befund keine weiteren local_build.sh-Laeufe
  von mir (Wiederholungen nur per ctest, ohne Bauschritt).

---

## 9. NACHBESSERUNG (Gegenpruefung `bau_a.gegenpruefung.md`, 2026-09-30)

Bauverzeichnis weiter `re15_port/build_r34_a`; Laufzeit-Ausgaben `build/r34g_a/nb_*`, `mutation_nb*`, `watch_kill_nb.txt`.
Gebaut NUR ueber `local_build.sh` — aufgerufen durch den Wrapper `build/r34g_a/lb.sh` (§9.1); Mutationsproben wie bisher
gezielt (`cmake --build build_r34_a --target probe_r34_wurf`, msys64 vorn im PATH, kein re15_pc-Link).

### 9.0 Uebersicht

| Nr | Schwere | Ergebnis | Datei:Zeile | Commit |
|---|---|---|---|---|
| M-1 | kritisch (Infrastruktur) | fremde Datei NICHT geaendert; INTEGRATIONSWUNSCH 8 mit fertigem Patch; eigene Laeufe nachweislich ohne Fremd-Kill (exportierte Shell-Funktion `powershell` -> Skript-eigener Zweig `:293-297`) | `build/r34g_a/lb.sh` (unversioniert) | — |
| M-2 | mittel | gebaut: Debug-Zustand/-Id beim Oeffnen := 0 | `menu_common.c:1410-1421` | 8a33ddbd |
| M-3 | mittel | gebaut: Kinder von R8/R15 mit Start-Flags 0x0a; Kommentar Durchgang 1 korrigiert | `re15_esp.c:565-570`, `:641-660` (R15), `:716-726` (R8), `:1110-1143`, `:1400-1408` | 8a33ddbd |
| M-4 | mittel | Sonde: Gier -24, echte acaec-Zusammensetzung (Override in Abschnitt 6 aus), HOCH/TIEF/Gift | `probe_r34_wurf.c:395-418`, `:648-651`, `:700-710`, `:778-832` | 8a33ddbd |
| M-5 | mittel | Suite ueber den Wrapper, §9.6 | — | — |
| H-1 | Hinweis | zusaetzlich gebaut (eigene Datei, eigener Commit, einzeln rueckbaubar): FUN_8001a8f8 byte-true | `player_common.c:1050-1075` | 82205b96 |
| H-2 | Hinweis | §6 (Hinweis an C) ergaenzt | `bau_a.md` §6 | — |
| H-3/H-4/H-5 | Hinweis | unveraendert Integration: INTEGRATIONSWUNSCH 1/2 (A nur mit C1/C2 zum Nutzer), OFFEN 6 / Wunsch 7 (exe-Treffer am Zombie in `test_r34_granaten`), Wunsch 6 (UTILITY-Menue) | — | — |

### 9.1 M-1 — `local_build.sh` beendet jede re15_pc.exe (NICHT Spur-A-Code, Datei nicht geaendert)

* **Messung** (wie Bauer/Pruefer): unter dem CLEAN_PATH des Skripts (`:147`) liefert `command -v powershell` nichts,
  `command -v taskkill` = `/c/Windows/System32/taskkill` -> immer der Rueckfall `taskkill //F //IM re15_pc.exe` (`:298-299`).
  `test` (`do_build && do_test`, `:360`) und `all` (`:361`) laufen ebenfalls durch `do_build`.
* **Zwischenloesung ohne Dateiaenderung** (`build/r34g_a/lb.sh`): der Wrapper exportiert
  `powershell() { /c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe "$@"; }` (`export -f`). `command -v` findet
  eine exportierte Funktion unabhaengig vom PATH -> das Skript nimmt seinen EIGENEN Zweig `:293-297`
  (`Get-Process re15_pc | Where Path beginnt mit <Bauverzeichnis> | Stop-Process`). Der Wrapper prueft das vor dem
  `exec` unter `PATH=/usr/bin:/c/Windows/System32` und bricht sonst ab (exit 97). Gemessen:
  * Vorpruefung: `cv powershell: powershell`, `ZWEIG=powershell`; jedes Bau-Log beginnt mit
    `=== lb.sh: Kill-Zweig = powershell (nur build_r34_a)`.
  * Filter TROCKEN (nur `Select-Object`, kein Stop): Praefix `...\r34g_b\re15_port\build_r34_b` -> genau
    `8700 ...\r34g_b\...\re15_pc.exe`; Praefix `...\r34g_a\re15_port\build_r34_a` -> leer.
  * Waechter `build/r34g_a/watch_kill_nb.ps1` (100 ms, taskkill/tskill mit Eltern und Grosseltern) ueber alle eigenen
    Baulaeufe und die Suite (eigene Kette aus Win32_Process: Bash 28844 `local_build.sh all` -> 37720 -> ctest 32292;
    `build/r34g_a/lb_pids.txt`): KEIN taskkill aus diesem Baum. Protokolliert wurden nur FREMDE, u. a.
    `03:54:05.491 taskkill /F /IM re15_pc.exe parent=2264` (r34n_generator, `local_build.sh test` gestartet 03:54:04),
    `03:56:33.110 ... grandparent=35800 local_build.sh build` (nicht meine PIDs), 03:55:55 / 03:57:03 / 04:02:37
    (Kommandozeile nicht mehr lesbar, Eltern nicht meine PIDs). `build/r34g_a/watch_kill_nb.txt`.
* **Fertiger Patch fuer INTEGRATIONSWUNSCH 8** (Ersatz fuer `local_build.sh:293-300`, fuer Integration/master):

      _ps=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
      if [ -x "$_ps" ] && command -v cygpath >/dev/null 2>&1; then
          _bw="$(cygpath -w "$(cd "$BUILD_REL" && pwd)")"
          "$_ps" -NoProfile -Command \
            "Get-Process re15_pc -ErrorAction SilentlyContinue | Where-Object { \$_.Path -and \$_.Path.StartsWith('$_bw', [System.StringComparison]::OrdinalIgnoreCase) } | Stop-Process -Force" \
            >/dev/null 2>&1 || true
      fi
      # KEIN `taskkill //IM`-Rueckfall: lieber nichts beenden (der Link scheitert dann mit "Permission denied" und
      # das Skript bricht ab) als jede re15_pc.exe der Maschine (Nutzer-exe im Hauptbaum, Messlaeufe anderer Spuren).

* **Hinweis an den Orchestrator**: bis der Patch in master ist, beendet JEDER `local_build.sh build|test|all` eines anderen
  Baums weiter fremde exe-Laeufe (waehrend dieser Nachbesserung gemessen, s. oben) — auch die exe des Nutzers im
  Hauptbaum. Alle Spuren koennen das `lb.sh`-Muster (exportierte Funktion) sofort nutzen, ohne die Datei anzufassen.

### 9.2 M-2 — Item-Debug beim Oeffnen des Statusschirms aus

* **Disasm (selbst, `re15_disasm.py dis 0x800460b8 200` / `dis 0x800463c0 60`)**: FUN_800460b8 laeuft von 0x800460b8 bis
  zum Kompaktierer `jal 0x8004dadc` @0x800464a0 gerade durch (nur die Bildschirmmodus-Zweige @0x800461f4-0x80046368 laufen
  wieder zusammen):

      800463e0 sb zero,9660(at)  25bc   ...  80046400 sb zero,9687(at)  25d7
      80046488 lui at,0x800b
      8004648c sb zero,9832(at)         0x800b2668 (Debug-Zustand) := 0
      80046490 lui at,0x800b
      80046494 sb zero,9833(at)         0x800b2669 (Debug-Id)      := 0
      8004649c sb v1,9678(at)           25ce (Equip-Schnappschuss)
      800464a0 jal 0x8004dadc           Kompaktierer

* **Xref** (`build/r34g_a/xref_imm.py`, lui 0x800b + imm): 0x800b2668 -> `0x8004648c sb`, `0x8004a150 sh`, `0x8004a164 lbu`,
  `0x8004a22c/254/284/2b4/2e4/308 sb`; 0x800b2669 -> `0x80046494 sb`, `0x8004a1b4/1e4/248/278/2a8/2d8 lbu`,
  `0x8004a260/290/2c0/2f0 sb`, `0x8004a344 addiu`. Ausser FUN_8004a0cc schreibt NUR die Oeffnungs-Init.
* **Port**: `menu_common.c:1410-1421` in `phase0_init` VOR dem Schnappschuss (Original-Reihenfolge). Alle Oeffnungswege
  laufen durch `phase0_init` (Phase 0 in `menu_task_dispatch`, `re15_menu_toggle`, `re15_menu_toggle_box`). Beim
  Schliessen bleibt der Zustand stehen (wie im Original, kein weiterer Schreiber).
* **Sonde** 149 (Vorbedingung: Zustand 3, Id 3, Platz 0 = `03 ff`), 150 (nach Schliessen OHNE KREIS + Wiederoeffnen:
  Zustand 0 / Id 0), 151 (ITEM bestaetigt, R1/L1/R2/L2: Platz 0 unveraendert, Zustand 0), 152 (Positiv-Kontrolle: SELECT
  danach wie gewohnt -> Zustand 3, Id 0, `00 ff`). Mutation N1 (Nullung entfernt) -> ROT 150, 151.

### 9.3 M-3 — Kinder von Routine 8 und 15 ueber den 0x0a-Kern

* **Disasm (selbst)**: R8 @0x800175ec: `addiu a2,a3,76` @0x80017604 (a2 = Platz+0x4c), `lh a1,46(a3)` @0x80017614,
  `lui v0,0x200` @0x80017624 (Kat 2), `addiu a3,a3,64` @0x8001762c (a3 = Platz+0x40), `jal 0x800199d4` @0x80017634.
  R15 @0x80017ac8: `lh a1,46(a3)` @0x80017b1c, `lw a2,116(a3)` @0x80017b20, `addiu a3,a3,64` @0x80017b24,
  `jal 0x800199d4` @0x80017b38. FUN_800199d4: `beq v0,zero,0x80019aa4` / Delay `ori v0,zero,0xa` @0x80019a84-88 ->
  `sb v0,108(t0)` @0x80019aa4. FUN_80019700: `ori v0,zero,0x3` @0x800197b4 -> `sb v0,108(t0)` @0x800197d0.
* **jal-Scan PSX.EXE + STAGE1..6.BIN** (`build/r34g_a/jal_scan_ovl.py`, Overlays ohne Header @0x80100000):
  `jal 0x800199d4` 16x, ALLE in den ESP-Routinen (0x800172f8, 0x80017634, 0x80017b38, 0x80017da0, 0x80018054,
  0x800185dc, 0x80018640, 0x80018660, 0x800186c8, 0x800189c4, 0x80018a60, 0x80018a98, 0x80018b34, 0x80018bc4, 0x80018c68,
  0x800191e0); `jal 0x80019700` 40x in der EXE (0x8002c74c-0x8002c8fc, 0x800336ec-0x80033e88, 0x800348b0-0x80034bdc,
  0x80038794/bc, 0x80041954, 0x80045710), in STAGE1..5 nur 0x80019700 (98/77/97/99/110 Stellen), STAGE6 keine.
  -> Der oeffentliche Weg `re15_esp_fx_spawn_rows` (Waffen-FSM, SCD-Op 0x3A, Gegner-Blut) bleibt 0x03; nur die Kinder der
  Routinen gehen ueber den neuen internen Weg `esp_fx_spawn_rows_flags(.., 0x0a)` (gleiche Mess-Log-Zeile).
* **Port**: `re15_esp.c:658-660` (R15), `:724-726` (R8), `:1110-1143` (Weg mit Start-Flags), Kommentar Durchgang 1
  `:1400-1408` (es gibt im Original KEINEN "Flags-3-Kind ohne A"-Fall). Lage (Anker = Eltern-Anker: a3 = Eltern+0x40,
  a2 = Eltern-Matrix; der Port traegt Anker.R*Versatz+Anker.T in x/y/z), Boden (Port-Sammelklemme, eigenes Thema),
  Skala und Gier unveraendert.
* **Sichtbare Wirkung** nur, wenn das Kind UNTER dem Eltern-Index landet (freier Platz darunter): der Zweitblitz des
  Muendungsfeuers (Kat 2 sub 4, Zeile 0 @CORE00 0x13D8 = R10: Flags 0x13, Satz 6) zeigt im Spawnbild schon seinen
  Satz 6 statt Satz 1; die Salven-Huelse (Kat 4 sub 0, Zeile 0 @0x18D8 = R16: Flags 0x63 = Physik-/Bild-Stopp) steht im
  Spawnbild still statt ein Bild lang zu laufen, ihr Halten endet ein Bild frueher. Landet das Kind darueber (leerer
  Pool = Normalfall der Bestandssonden), ist der Zustand je Bild unveraendert (A in Durchgang 2 statt 1).
* **Sonde Abschnitt 8** (`probe_r34_wurf kinder`, `probe_r34_wurf.c:1040-1157`): 8a — Saeuregranate (Art 3, keine Kinder)
  auf Platz 0 erreicht Zuender 0 genau im Bild, in dem das Muendungsfeuer (Plaetze 1/2) Routine 8 faehrt -> Kind auf
  Platz 0 < 1: 161/162 Vorbedingungen, 163 Kind unten = Flags 0x13, Zeile 1 (R10 einmal), 164 identisch zum Kind ueber dem
  Eltern-Platz (leerer Pool, Platz 2). 8b — Altplatz ohne Bank auf Platz 0 faellt im Hauptlauf von Bild 1; Salve
  id 4 sub 2 auf Platz 1: Kind #1 (Bild 1) auf Platz 2, Kind #2 (Bild 4) auf Platz 0: 165/166 Vorbedingungen,
  167 Kind #1 = Flags 0x63, +0x16 = 1, Satz 0, 168 Kind #2 identisch. Gemessen:
  `8a R8-Kind unten (Platz 0): fl 13 Zeile 1 Satz 6 clut 7a91 tpage 003f | oben (Platz 2): fl 13 Zeile 1 Satz 6`,
  `8b R15-Kind #1 (Platz 2): fl 63 +16 1 sichtbar 1 | #2 (Platz 0): fl 63 +16 1 sichtbar 1`.
  (Erste Fassung erwartete fuer 0x63 "unsichtbar" — falsch: 0x63 traegt Bit 1; korrigiert, bevor committet wurde.)
* **Mutationen**: N2 (R8-Kind 0x03) -> ROT 163, 164; N3 (R15-Kind 0x03) -> ROT 168; N4 (Kind-Init aus) -> ROT 26-30,
  163, 164, 167, 168; N5 (Kind auch in Durchgang 1 dispatcht, Gate `& 0x09`) -> GRUEN = **aequivalente Mutante fuer die
  CORE00-Daten**: jedes Kind, das eine Routine spawnt (R8 -> Kat 2 sub 4/5 = R10; R15 -> Kat 2 sub 0 = R8/R10, Kat 3 sub 0
  = R10, Kat 4 sub 0 = R16; R31 -> Kat 3 sub 1/3 = R10; `build/r34g_a/esp_rows_dump.py`), beginnt mit einer Routine,
  die die Flags aus der Zeile setzt (Bit 3 weg) -> die Kind-Init greift danach nicht mehr, A laeuft in beiden Faellen
  einmal. Das Gate selbst ist byte-true Bit 0 (`andi v0,v0,0x1` @0x80019e78).

### 9.4 M-4 — tote Sondenstellen G2/G4/G5

* **Gier -24** (Pruefungen 66/67, `probe_r34_wurf.c:395-418`): Erwartung unabhaengig aus PSX.EXE gerechnet
  (`build/r34g_a/gier_neg_erwartung.py`: Tabelle 0x800794c4 + Formeln FUN_80068098/FUN_800661c0):
  `[24]` @0x80079524 = 0x0FFD0097 (sin 151, cos 4093), `[4072]` @0x8007D464 = 0x0FFDFF70 (sin -144, cos 4093);
  Negativzweig `bgez t7` @0x800680a0 faellt durch, `subu t7,zero,t7` @0x800680a8, `andi t7,t7,0xfff` @0x800680b0,
  `subu t3,zero,t8` @0x800680d0 (sin negiert), `sra t0,t9,16` @0x800680d8 (cos). Original: M = [[4093,0,-151],[0,4096,0],
  [151,0,4093]] -> Liegestelle (5004, 9, -16089); `a & 0xfff` gaebe (5007, 9, -16109). Gegenprobe des Skripts: Gier 0 ->
  (5078, 9, -16527) = Pruefung 19, Gier 1024 -> (-5099, 9, -30208) = Pruefung 54. Gemessen: `Gier -24: Liegestelle
  (5004,9,-16089)`. Mutation G2 -> ROT 66.
* **Echte Zusammensetzung von `re15_player_acaec`** (`probe_r34_wurf.c:648-651`: Override in Abschnitt 6 AUS; 115: 6a
  mit MITTE gesund = K1 MITTE, Zaehler 7; 116-127 = 6d, `:778-832`): Hoehe ueber das Steuerkreuz im HALTEN, Gift-Bit in
  `status_flags`. Gemessen:

      6d HOCH gesund    : Hoehe 1 Wort 8000, Abzug 0, Spawn A+19, xlat (380,-110,21) vel (378,-100,21) acc_x -2 Zaehler 7
      6d HOCH vergiftet : Hoehe 1 Wort 8002, Abzug 0, Spawn A+19, xlat (380,-110,21) vel (378,-100,21) acc_x -2 Zaehler 9
      6d TIEF vergiftet : Hoehe -1 Wort 2002, Abzug 0, Spawn A+24, xlat (80,0,1) vel (79,10,1) acc_x -1 Zaehler 5
      6d MITTE vergiftet: Hoehe 0 Wort 4002, Abzug 0, Spawn A+22, xlat (280,-50,24) vel (279,-40,24) acc_x -1 Zaehler 9

  Erwartung: K1 (HOCH `ori 0x17c`/`addiu -110`/`ori 0x15` @0x80018494-a8, acc_x -2 @0x800184b0; TIEF `ori 0x50`/`ori 0x1`
  @0x80018518-24, acc_x -1 @0x80018528-2c, Zaehler 5 @0x80018530/38; MITTE @0x800184bc-d4), Zaehler-Formel
  ((a + ((a>>7)&0xff)) & 0xff) % 4 + 7 (@0x8001af30-4c, `addiu v0,v0,7` @0x80018504): 0x8000 -> 7, 0x8002 -> 9,
  0x4002 -> 9; Schreiber HOCH `ori 0x8000` @0x80033228-38, TIEF `ori 0x2000` @0x80033270-84, MITTE `ori 0x4000`
  @0x800332bc-c8 (je `andi 0x1fff` davor), Gift `ori 0x2` @0x80012eac; Spawn-Clipbild HOCH 0x13 @0x80033690, TIEF 0x18
  @0x80033758. Mutation G4 (HOCH/TIEF-Bit vertauscht) -> ROT 117, 118, 120, 121, 123, 124; G5 (ohne Status-Unterbits)
  -> ROT 121, 127.

### 9.5 H-1 — Auto-Nachfuehrung beim Heben (zusaetzlich; schliesst OFFEN 5)

* **Disasm (selbst, `dis 0x8001a8f8 52`)** und Aufrufer (`jal_scan.py 0x8001a8f8`: 0x80032fec gun sub0 mit `ori a1,zero,
  0xc8` @0x80032fe0, 0x80033f94 L1-Nachziehen mit `ori a1,zero,0xc8` @0x80033f90, 0x80034fbc melee mit `ori a1,zero,0xc0`
  @0x80034fb0, dazu 0x80034134, 0x80035620):

      8001a958 lhu  a2,106(v1)          rot (u16, Platz +0x6a)
      8001a960 subu v0,a0,a2            t - rot            (t = atan2 & 0xfff, `andi v0,v0,0xfff` @0x8001a768)
      8001a964 addu v0,s1,v0            + s
      8001a968 andi a1,v0,0xfff         a1 = (t - rot + s) & 0xfff
      8001a96c sll  v0,s1,16 / 8001a970 sra v0,v0,15 / 8001a974 slt v0,a1,v0      a1 < 2s ?
      8001a978 beq  v0,zero,0x8001a988 / Delay 8001a97c subu v0,a2,s1
      8001a984 sh   a0,106(v1)          ja:   rot := t
      8001a988 sh   v0,106(v1)          nein: rot := rot - s   (16 Bit, OHNE & 0xfff)
      8001a98c sltiu v0,a1,0x801 / 8001a994 sll a0,s1,1 / 8001a9ac addu v1,v1,a0 / 8001a9b0 sh v1,106(v0)
                                        a1 <= 0x800: rot += 2s (netto rot + s)

* **Port** `player_common.c:1050-1075` genau so. Vorher (zentrierte Differenz, Klemme +-s, `& 0xfff` je Schritt) zwei
  Abweichungen: (1) Schritt ueber 0 -> 4096-k statt -k (anderer RotMatrix-Zweig fuer die Granaten-Gier, M-4/G2);
  (2) NEUER Befund: fuer (t - rot) & 0xfff in [0x801 - s, 0x7ff] (Ziel knapp unter 180 Grad, Bandbreite s-1 = 199 bzw.
  191 Einheiten, gut 17 Grad) drehte der Port +s, das Original -s — die Halbebenen-Grenze liegt wegen des `+ s` in a1
  um s verschoben.
* **Sonde 6e** (`probe_r34_wurf.c:834-876`): Ziel Zombie 700 in +x, Peilung des Port-atan2 = 4095 (Vorbedingung 128,
  Aufbau). 129: rot -300 -> -100 -> 4095 (alt: 3996); 130: rot 2196 -> 1996 -> 1796 (alt: 2396); 153 Negativ-Kontrolle:
  rot 2296 -> 2496 -> 2696 (ausserhalb des Bands drehen alt und Original gleich). Mutationen H1a (alter Schritt) -> ROT
  129, 130; H1b (nur Maske) -> ROT 129; H1c (Grenze ohne +s) -> ROT 130.
* **Beobachtung ausserhalb des Dateibesitzes** (nur Hinweis): der Lauf-Lenker FUN_8001aac4 ist baugleich (`subu v0,a2,s1`
  / `sh v0,106(v1)` @0x8001ab4c/58, `addu v1,v1,a0` / `sh` @0x8001ab7c-80, ohne Maske); sein Port-Zwilling
  `re15_enemy_steer_point` (`enemy_ai_common.c:3499-3513`) maskiert jeden Schritt (`& 0x0fff`) — gleiche Klasse wie H-1
  fuer Gegner (die Richtungslogik dort rechnet bereits mit `+ slew`). INTEGRATIONSWUNSCH 9.

### 9.6 M-5 — Suite (volle Laeufe ueber den Wrapper, eigene Sonden)

* **Eigene Sonde**: `probe_r34_wurf` ALLE PRUEFUNGEN GRUEN (`build/r34g_a/nb_probe4.txt`); jeder Abschnitt einzeln
  (`mitte zeit gier saeure rand schritt debug kinder`) rc 0 (`build/r34g_a/nb_probe_<abschnitt>.txt`).
* **Suite 1** (`lb.sh all` = configure + build + test, 04:00:39-04:09:14; Windows-Kette aus Win32_Process: Bash 28844 ->
  37720 -> ctest 32292): `99% tests passed, 5 tests failed out of 429`, 500.06 s (`build/r34g_a/nb_suite1_ctest.log`).
  Rot NUR exe-Tests, je `exit=1` ohne Meldung: `integration_r30_cut_blitz` ([B] debug.log zuletzt 04:02:36.988 -> Waechter
  04:02:37.206 taskkill), `integration_r30_granate_laden` ([a] 04:04:17.52 -> 04:04:19.792), `integration_r30_irons_tisch_bild`
  ([S] 04:05:31.628 -> 04:05:32.227), `integration_r33_speichern` ([a] Start 04:08:15.45 + 9,90 s -> 04:08:25.446
  `taskkill /F /IM re15_pc.exe`, Grosseltern 28132 `local_build.sh build` = fremd), `integration_relatch_pin` (Abriss nach
  Bild 90 um ~04:08:48; kein Eintrag — der Waechter fragt alle 100 ms, ein taskkill kann kuerzer leben).
* **Einzel-Wiederholung per ctest** (04:09:41-04:10:59, `build/r34g_a/rerun_nb_suite1.txt`): alle 5 wieder rot; dabei
  Fremd-Kills 04:09:41.767 (cut_blitz nach 0,77 s), 04:10:28.081 (irons_tisch_bild), 04:10:45.693 (r33_speichern), je
  `local_build.sh build` anderer Baeume.
* **Suite 2** (`lb.sh test`, 04:17:32-04:28:06; Windows-Kette Bash 32740 -> 1988 -> ctest 27240): `98% tests passed,
  9 tests failed out of 429`, 631.79 s (`build/r34g_a/nb_suite2_ctest.log`). Rot NUR exe-Tests (cut_blitz, granate_laden,
  irons_tisch_laden, irons_tisch_bild, irons_tisch_licht, sicherung_bild, r32_tor_hell, r33_speichern, boot_bg_pin; alle
  `exit=1`/Abriss). Im Fenster 10 fremde taskkill-Aufrufe (9 lesbar `taskkill /F /IM re15_pc.exe`, 1 nicht mehr lesbar),
  0 aus meiner Kette.
* **Suite 3** (`lb.sh test`, 04:44:22-04:58:12, Windows-Bash-PID 32144 laut `lb_pids.txt`): `99% tests passed, 2 tests failed
  out of 429`, 826.82 s (`build/r34g_a/nb_suite3_ctest.log`). Rot: `integration_r30_cut_blitz` ([B] Abriss nach Bild 33 in
  ROOM1240, 04:47:50.2-04:47:51.7, exit=1 ohne Meldung; kein Waechter-Eintrag = zu kurzer taskkill) und
  `integration_r33_speichern` ([b] letzter Log-Schreibzugriff 04:56:40.602 -> Waechter 04:56:41.752 fremdes
  `taskkill /F /IM re15_pc.exe`). Beide liefen in beiden Beweislaeufen mit der umbenannten exe gruen.
* **Beweislauf gegen die Fremd-Kills** (`build/r34g_a/exe_kopie_lauf.sh`): exakt die ctest-Kommandos (`ctest -N -V`), nur
  mit einer BYTE-GLEICHEN Kopie der exe unter anderem Bildnamen (`re15_pc_nb.exe`, md5 `be0159f91324c255c236a874a7cf503d`
  = Original, gleiches Verzeichnis = gleiche Asset-Wurzel; danach geloescht): **alle 16 exe-Tests rc 0**
  (04:28:22-04:43:00, `build/r34g_a/kopie_nb_alle.txt`: weste_load_pin, r30_cut_blitz, elza_vollstart, r30_granate_laden,
  r30_irons_tisch_laden/_bild/_licht, r30_sicherung_laden/_bild, r30_titel_puls, r32_tor_hell, r33_speichern, boot_bg_pin,
  dark_start_pin, relatch_pin, save_counter_pin). Im selben Fenster protokollierte der Waechter 16 fremde
  taskkill-Aufrufe (15 lesbar `taskkill /F /IM re15_pc.exe`, 1 nicht mehr lesbar; 04:29:06-04:38:00) — ohne Wirkung auf die Kopie. Vorher schon die 5 Rot aus Suite 1
  (04:11:58-04:17:22, `build/r34g_a/kopie_nb1.txt`): 5/5 rc 0.
* **Ergebnis**: alle 413 Nicht-exe-Tests (Unit, Sonden inkl. `unit_r34_wurf`, Blut-/Huelsen-/Muendungs-Pins
  `unit_r26_mg_blut`, `unit_r17_waffen_loop_pin`, `probe_abzug_takt`, `unit_r30_granate`, `unit_espr_11e0`, C-Integration)
  in ALLEN DREI Suiten gruen; alle 16 exe-Tests gruen (Suite 3: 14 direkt, alle 16 im Beweislauf). Das exe-Rot wechselt
  von Lauf zu Lauf (5 / 9 / 2 Tests, jeweils andere) — kein reproduzierbares Rot. **Die Zielzeile `=== LOCAL-BUILD-OK (all) — Tests 429/429` ist NICHT erreicht**: Ursache sind
  fremde `local_build.sh build|test|all` (M-1 in den anderen Baeumen; Waechter 03:54-04:57: 55 taskkill-Eintraege =
  42 lesbar `taskkill /F /IM re15_pc.exe`, 10 mit nicht mehr lesbarer Kommandozeile, 3 PID-genaue eines anderen Baums;
  keiner aus diesem Baum). Ein ungestoerter Gesamtlauf braucht INTEGRATIONSWUNSCH 8 in allen laufenden Baeumen (oder eine
  ruhige Maschine) — vor dem Merge von A nachzuholen (Orchestrator).

### 9.7 Mutationsproben der Nachbesserung (`build/r34g_a/mutation_nb.py`, `mutation_nb_ergebnis.txt`, `mutation_nb_h1.txt`)

| Nr | Mutation | Ergebnis |
|---|---|---|
| N1 | M-2: Nullung beim Oeffnen entfernt (menu_common.c) | ROT 150, 151 |
| N2 | M-3: Routine-8-Kind Flags 0x03 | ROT 163, 164 |
| N3 | M-3: Routine-15-Kind Flags 0x03 | ROT 168 |
| N4 | M-3: Kind-Init im Hauptlauf aus | ROT 26, 27, 28, 29, 30, 163, 164, 167, 168 |
| N5 | M-3: Kind auch in Durchgang 1 dispatcht | GRUEN — aequivalent fuer CORE00 (§9.3) |
| G2 | M-4: RotMatrix-Negativzweig -> `a & 0xfff` | ROT 66 |
| G4 | M-4: acaec HOCH/TIEF-Bit vertauscht | ROT 117, 118, 120, 121, 123, 124 |
| G5 | M-4: acaec ohne Status-Unterbits | ROT 121, 127 |
| H1a | H-1: alter Port-Schritt | ROT 129, 130 |
| H1b | H-1: Schritt maskiert | ROT 129 |
| H1c | H-1: Richtungsgrenze ohne +s | ROT 130 |
| — | Rueckbau (beide Laeufe) | `ZURUECK: bau rc=0, sonde rc=0 (GRUEN)`, `git status` nur `build/` |

Die drei toten Stellen der Gegenpruefung (G2, G4, G5) sind jetzt rot.

### 9.8 INTEGRATIONSWUNSCH (neu)

9. `re15_port/engine/src/enemy_ai_common.c:3499-3513` (`re15_enemy_steer_point`, Gegner-KI): Schritte ohne `& 0x0fff`
   wie FUN_8001aac4 (`sh v0,106(v1)` @0x8001ab58 = rot - s, `addu`/`sh` @0x8001ab7c-80 = + 2s); Einrasten bleibt
   `bearing` (0..4095). Vorher pruefen, ob Gegner-Leser rot_y ungemaskert als Index nutzen. Nur Hinweis (Gegenstueck
   zu H-1), kein Spur-A-Thema.

### 9.9 OFFEN (neu)

7. Routinen mit Kind-Spawn, die der Port noch nicht kennt (R2, R19, R25, R39, R43 = noop), muessen beim Bau ueber
   `esp_fx_spawn_rows_flags(.., 0x0a)` spawnen (jal @0x800172f8 / 0x80017da0 / 0x80018054 / 0x800189c4-0x80018c68 /
   0x800191e0).
