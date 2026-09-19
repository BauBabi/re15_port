# Skeptiker-Pruefung: trefferhoehe.md (Runde 16, 2026-09-19)

Geprueft gegen: `info/re2leon/PSX.EXE` (t_addr 0x80010000), `info/re2leon/COMMON/BIN/EMOVL10_S0.BIN`
(53068 B, Zombie-Overlay @0x80100000), `info/Re1.5/PSX.EXE`, `PSX/BIN/STAGE1.BIN`,
`RE2_Quellcode_V2/FUN_800410cc.c` / `FUN_80041b20.c` / `FUN_80041ce4.c`, Port-Stand 6692c9cd.
Werkzeuge: `re2_disasm.py` / `re15_disasm.py`, eigene Python-Dumps der EXE-Tabellen, eigener Build
`re15_port/build_r16_sk_trefferhoehe` (Dossier-Sonde + Gegensonde `probe_r16_sk_trefferhoehe`,
Registrierung `tests/unit/probes/r16_sk_trefferhoehe.cmake`).

## Ergebnis-Tabelle

| # | Befund (Dossier) | haelt / widerlegt | Beleg (eigene Adressen / Messwerte) |
|---|---|---|---|
| 1 | §0.1/§1: Port trifft Kriecher (+0x10E=0x2001) und Aufsteher (EXEC[5] P6) mit EBEN auf jeder Distanz; 40 Divergenzen, 20 davon Kriecher/Aufsteher EBEN | **haelt** | Eigener Build der Dossier-Sonde: Log nach Filter der SCD-Zeilen **byte-identisch** zum Dossier-Log (`diff` leer), `grep -c DIVERGENZ` = 40. Kriecher W3 EBEN d=1500..9000 IST HIT dhp=16, W8 EBEN HIT 3801/200; Aufsteher-P6 EBEN HIT alle. Sonde misst den echten Resolver (`re15_player_weapon_fire`, Pad-Schuss in Phase B traf ebenfalls: hp 4000→3800). |
| 2 | §2.2: Applier FUN_800410CC prueft `word0>>26&7` gegen Zeilen `DAT_800A6DB4`; EBEN bei dy=0 → Zeile 3 Satz 2 `[02 00 00]` (nur Rumpf); TIEF → Zeile 6 `[01 02 04]` (Beine zuerst); Fenster aus Zombie-Record `0x800A412C+(id-1)*20`; Gruppe aus `DAT_800A6F8C` | **haelt** | Eigener Dump: `DAT_800a6db4 = 04 02 01 02 01 04 01 02 04 04 02 00 02 00 00 01 02 00`; `DAT_800a6f8c = FF 00 01 FF 02 00 00 00`; `PTR_DAT_800a6a88[0x10] = 0x800A412C`; Caller @0x80047ec0-c8 `addiu a1,v0,-1 / sll a1,a1,16 / or a1,a1,a2` (item-1 ⇒ Id 3 → idx 2 @0x800A4154 UP(-5000,-2000) LV(-3000,2000) DN(-500,3000) dmg 16/15/14; Id 7 → idx 6 @0x800A41A4 UP(-5000,500) dmg 200/60/40); Geometrie-Zeiger @0x800A68E8+item*24+grp*8 → 0x800A6618/34/50 (Pistole) und 0x800A6724/40/5C (Schrot), Flags/Boxen exakt wie im Dossier. Decompile FUN_800410cc: `uVar5 = *puVar9 >> 0x1a & 7`, `dy = puVar9[0xf] - player[+0x3c]`, Zeilenwahl `6/3/0`, `pb[2]→pb[1]→pb[0]`, `+0x1D2 = 3*Klammer + Teil`. Fensterrechnung FUN_80041b20 Fall 0..6 = Dossier. |
| 3 | §2.3: Maske INIT `\|= 0x0C000000` @0x80100984; Umschalten auf NUR Beine an den genannten Stellen; Rueckbau @0x801036DC (nur bei `+0x14D == 55`, P7), @0x80103730 (P8), @0x80103908 (EXEC[7] P4), @0x80107EA8, @0x801049F0 | **haelt** (mit drei Korrekturen) | Alle zitierten Instruktionen selbst disassembliert und byte-gleich: 0x80100984 `lui v1,0xc00 / or / sw v0,0(s2)` (+ `sh 500,494(s2)` @0x80100980 = +0x1EE!); 0x801032b8/e8, 0x80104068/98, 0x8010441c/44, 0x80106ae0/b38-50 (+ `sh 8193,270` = 0x2001), 0x801077d8/0x80107828, 0x80100af0/b38, 0x80100bb4/c0c, 0x80102be8/c10, 0x80102d38/d80, 0x80108974/89b4; Rueckbau 0x801036d0-f0 (`lbu v1,333 / addiu v0,55 / bne`), 0x80103730, 0x80103908/28, 0x80107ea8, 0x801049f0. Phasentabelle EXEC[5] @0x8010006c: [7]=0x80103628 (P7) .. [8]=0x801036f4 (P8) ⇒ 0x801036d0 liegt in P7, bestaetigt. **Korrekturen:** (a) 0x80104068-AC ist **EXEC[9] @0x80103E48** (Stoss/Taumel, Port :2579ff — Zeile :2704 liegt dort), NICHT „EXEC[8] Getup": EXEC[8] @0x80103B74 ist die FRESSER-Pose (Tabelle @0x801000b4, P3 @0x80103cd8, P4 @0x80103d60, Exit `sw 0x101,4` @0x80103d94) und enthaelt KEINEN Maskenwechsel — der Fresser behaelt Maske 3 (gut fuer die STEHEND-SOLL der Sonde, aber der Fix-Plan darf dort nicht `=1` setzen). (b) Eine weitere, ungelistete Schreibstelle im Root @0x8010039c-a8: `if (+0x10E&1 \|\| +0x21A&2) word0 \|= 0x04000000` (jedes Bild; effektiv Sicherung). Kurzfassung sagt „sieben Stellen", die Tabelle hat zehn. (c) EXEC[9] verlaesst seinen NUR-Beine-Zweig (Phase 1 @0x80104084) nach Clip-Ende (Phase += Ergebnis @0x8010412c-40) ueber Phase 2 → `sw 0x101,4` @0x80104148 **ohne** `lui 0xc00` im Handler (lui-Vollscan). Entweder bleibt der Zombie in RE2 dann stehend mit Maske 1, oder der Rueckbau liegt ausserhalb der 5 gelisteten Stellen — offen. |
| 4 | §2.2/§2.4/§0.1: „TIEF trifft Stehende/Kriecher nur bis 4100 (ab 4100 MISS)", „Schrot-HOCH nur Nah-Box <3100"; Sub-Box = `[start, start+4*depth4)` | **widerlegt (Grenzwerte)** | FUN_800410cc addiert VOR dem Box-Test die Zielradien: `*(short*)(iVar10+8) += *(short*)(puVar9+0x1ee) >> 2` (Nah-Box-TIEFE) und `+10/+0x12/+0x1a += +0x9A >> 2` (Breiten). Zombie-INIT: `sh v1,494(s2)` mit v1=500 @0x80100980 — **einziger** Schreiber von +0x1EE im Overlay (eigener sh/sw-Scan), liegt im gemeinsamen INIT-Pfad VOR den Varianten (Fresser 0x801 @0x80100adc, Kriecher @0x80100ae0ff) ⇒ gilt auch fuer Kriecher. In der /4-Skala des Dossiers: DOWN/LEVEL-Nah-Box **[100, 4600)**, Schrot-UP-Nah-Box **[100, 3600)**. Gegensonde `probe_r16_sk_trefferhoehe`: STEHEND und KRIECHER W3/W8 TIEF d=4100/4300/4599 → SOLL+1EE **HIT** (Dossier-SOLL MISS), d=4600 MISS; STEHEND W8 HOCH d=3500/3599 → SOLL+1EE HIT Zone 1 (Dossier MISS), **Port IST dort MISS** ⇒ zusaetzliche Divergenz im Band 3100..3600, die das Dossier-Raster (2500/3600) nicht sieht und deren §3-Beschreibung („Schrot-Boxen 3100/9100") sie sogar als korrekt fuehrt. Die 40 gemessenen Zellen (3600/5500) aendern sich nicht. Ferne Kante zaehlt als aussen (Vorzeichen-Test FUN_80041ce4, Produkt 0 ⇒ false). |
| 5 | §2.1: RE1.5 = Band-Schnitt @0x800120d0-ec, Y-Test @0x80012194-c0, +0x6 aus `DAT_8006f410`, ACTIVE-Tail STAGE1 @0x801015c0-3c (liegend: LEVEL weg, DOWN <0x1388) | **haelt** | `re15_disasm.py`: 0x800120d0 `lw v0,0(s0)` … `lui v1,0xe000 / and / beq`; 0x80012194-b8 `slti 501`-Paar; 0x80012430-50 `srl v0,v0,29 / lbu 0x8006f410[v0] / sb v0,6(s1)`; `DAT_8006f410 = 07 00 01 07 02 00 00 00`; STAGE1 0x801015c0 `lbu 9(a0) / andi 0x80`, 0x80101600 `jal 0x80012aa4 / ori a0,0xbb8`, 0x80101624-3c `lui v1,0xbfff / ori 0xffff / and / ori a0,0x1388 / jal 0x80012974`. |
| 6 | §3: Port-Ursache — `re2_rising` :1449-1455 faellt aufs RE1.5-Band; `lying` :1457-1466 nimmt Kriecher aus (`!(f10e&1)`); `re2_fenster` :1467-1527 Drittel nur fuer UP, DOWN ohne Sub-Box; Fenstertest :1670-1676; Zone :1996-1999 aus Zielhoehe; kein Feld fuer Bits 26-28 (`flags` uint8 re15_actor.h:43; „OPEN" enemy_ai_re2_zombie.c:1858) | **haelt** | Zeilen gelesen, Inhalt exakt wie zitiert; `grep re2z_parts` leer. IST-Verhalten der Sonde folgt daraus zwingend (Kriecher → Steh-Zweig → LEVEL-Fenster [-3000,2000] ∋ 0). |
| 7 | §2.3-Tabelle: Liege-Spawn 0x88 in RE2 „kein Ziel" (HP=-1 @0x80100A3C-40, +0x1D3\|=0x80 @0x80103804-14; Applier-Gates `+0x1D3==0`, `HP>=0`) | **haelt** | 0x80100a3c `addiu v0,zero,-1 / sh v0,342(s2)`; 0x80103804-14 `lbu v0,467 / ori 0x80 / sb v0,467`; Decompile-Gates `*(char*)(puVar9+0x1d3)=='\0'`, `-1 < *(short*)(puVar9+0x156)`. Port-IST hp=79/+0x1D3=0x80 (Sonde) — Divergenz-Quelle ist das Port-HP (Nachbar-Sonde), die Sonde bewertet ueber +0x1D3 richtig. |

Nicht geprueft: §2.3 letzter Absatz (Angriffs-Volumen-Zonen FUN_800470C0 @0x800472AC-30C); §2.2
Schadens-/Poise-Spalten; Vorwaerts-Achse der Box (lokal X der Matrix +0x24) — das Dossier fuehrt
sie selbst als offen (§5). Die Lueckenfreiheit 100→4100→8100 stuetzt die /4-Skala, bleibt aber
Herleitung.

## Fix-Plan (§4): Schritte auf unbelegter oder falscher Annahme

1. **Schritt 2, Sub-Box `start[b] <= dist < start[b]+4*depth4[b]`** und **Schritt 5(a) „TIEF nur
   noch <4100"**: falsch ohne `+0x1EE>>2` (Nah-Box-Tiefe) — belegt: +0x1EE = 500 @0x80100980.
   Richtig: DOWN/LEVEL-Nah-Box bis 4600, Schrot-UP bis 3600. Ebenso fehlt `+0x9A>>2` auf den
   Breiten (500 stehend @0x8010096c-74, 200 Kriecher @0x80106b18-24 / @0x80100b04-10) — der Port
   hat +0x9A nicht als Feld; fuer seitlich versetzte Ziele zaehlt das.
2. **Schritt 1, „EXEC[8] P0 (:2704, @0x80104098) = 1"**: die Stelle ist EXEC[9] (Stoss/Taumel).
   Wird `=1` in den Port-Zwilling der FRESSER-Pose (EXEC[8] @0x80103B74) gesetzt, werden alle
   ROOM1140-Fresser nach dem Aufstehen EBEN-immun — RE2 laesst die Maske dort auf 3
   (Exit `sw 0x101` @0x80103d94 ohne Maskenwechsel).
3. **Schritt 1, Rueckbau-Liste (5 Stellen)**: EXEC[9] erreicht `sw 0x101,4` @0x80104148 aus dem
   NUR-Beine-Zweig ohne Rueckbau im Handler. Ob RE2 diesen Zombie stehend mit Maske 1 laesst oder
   ein nicht gelisteter Rueckbau greift, ist nicht belegt — vor der Implementierung klaeren
   (sonst pendelt der Port zwischen „EBEN-immun nach Stoss" und „falsch zurueckgesetzt").
4. **Schritt 1, „EXEC[5] P7 bei Clip-Bild 55 — im Port die Clip-Frame-Quelle nutzen, die
   `re2z_clip_done` liest"**: dass diese Port-Zaehlung dem Original-Byte +0x14D (Clip-Bild) entspricht,
   ist nicht gemessen (Annahme).
5. **Schritt 4, Pin „TIEF → HIT Zone 0 nur d≤3600"**: das Raster laesst die Grenze offen; ein Pin
   mit 4100-Grenze wuerde eine falsche Implementierung gruen schalten. Zusicherungen bei 4599 (HIT)
   / 4600 (MISS) und Schrot-HOCH 3599 (HIT) / 3600 (MISS) aufnehmen.
6. **Schritt 4 / Sonden-`re2_soll`**: setzt `res` bei XZ-Treffer ohne Maskenpassung NICHT auf 0
   zurueck (Original: `uVar7 = 0` in Sub-Box 2/3 vor der Maskenpruefung). Bei dy=0 folgenlos, als
   Pin-Referenz fuer dy≠0 falsch.
7. **§2.4 Achse/Skala** (Dossier §5 selbst): Vorwaerts-Achse = lokal X der Matrix +0x24 nicht belegt;
   der Plan baut darauf.
8. **Schritt 3, Klammer 1/2 fuer Fern-Treffer**: aendert Schadensspalten (Id 3: 16/15/14, Id 7:
   200/60/40 laut Record-Dump) — der Plan verweist auf einen noch ausstehenden Tabellen-Dump; bis
   dahin Annahme.

## Sonden

* Dossier-Sonde, eigener Build: `re15_port/build_r16_sk_trefferhoehe/tests/unit/probe_r16_trefferhoehe.exe`
  → `sk_run.log` (identisch zum Dossier-Log).
* Gegensonde: `re15_port/tests/unit/probe_r16_sk_trefferhoehe.c`, Registrierung
  `re15_port/tests/unit/probes/r16_sk_trefferhoehe.cmake`, Binary im selben Build-Verzeichnis.
  Distanzen 3500/3599/3600/3900/4100/4300/4599/4600/4700, SOLL zweimal (Dossier-Modell vs. +0x1EE=500).
