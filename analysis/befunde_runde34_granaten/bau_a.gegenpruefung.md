# Runde 34 (Granaten) — GEGENPRUEFUNG Spur A (Skeptiker am gebauten Stand)

Stand: 2026-09-30, Zweig `r34g/a-granate` @ 85320e45 (Basis C0 8d8651e4), Arbeitsbaum `.claude/worktrees/r34g_a`.
Geprueft: `git diff 8d8651e4..HEAD` (Spur-A-Anteil; `master...HEAD` enthaelt zusaetzlich den C0-Vertrag, der nicht Teil
dieser Pruefung ist). Ich aendere KEINEN Code; nur diese Datei.

STATUS: ABGESCHLOSSEN. Urteil: **Kernmechanik byte-true bestaetigt (jede tragende Konstante selbst nachgelesen), vier
Maengel mittel, ein Infrastruktur-Befund kritisch (nicht Spur-A-Code), fuenf Hinweise** — Abschnitt 8/9.

## 0. Vorgehen

1. Jede tragende Konstante selbst mit `re15_disasm.py` / `re2_disasm.py` (dis/read/table/bytes) nachgelesen (§1).
2. Semantik gegen BAUPLAN §1.1/§1.2/K1-K3/K9 (Delay-Slots, Reihenfolge, Vorzeichen, s16/s32, Bildzaehlung) (§2).
3. Sonde: echte Engine? Erwartung unabhaengig? Negativ-Kontrollen? 12 EIGENE Mutationsproben (§3).
4. Dateibesitz (§4), Sonden + Bestandstests im Bauverzeichnis des Baums `build_r34_a` (§5), eigene exe-Messung 0x0A/0x0B (§5.3),
   Regressionsrisiko (§6), Auftragsluecken (§7).

Bauweg: `local_build.sh build|test|all` NICHT benutzt — er beendet nachweislich jede re15_pc.exe der Maschine (§5.4, Regel
"niemals taskkill per Bildname"). Stattdessen dieselbe Umgebung wie das Skript (CLEAN_PATH mit msys64 zuerst, CC/CXX fest) und
`cmake --build build_r34_a [--target probe_r34_wurf]`, danach ctest mit `PATH=/c/msys64/mingw64/bin:$PATH`.

## 1. Konstanten — selbst disassembliert (re15_disasm.py, info/Re1.5/PSX.EXE)

Alle unten gelisteten Werte in DIESER Sitzung nachgelesen; Ergebnis = stimmt, sofern nicht anders vermerkt.

| Bereich | Port-Stelle | Original (eigene Lesung) | Urteil |
|---|---|---|---|
| R30 Satz/Flags/B/A/Zuender | re15_esp.c case 30 | `ori v0,zero,0x17; sb v0,110(v1)` @0x80018448/50; `ori 0x3; sb 108` @0x8001845c/60; `ori 0x1d; sh 2` @0x8001846c/70; `sh zero,0` @0x80018478; `ori 0x2a` @0x80018474 / `sh v0,30` @0x8001847c | stimmt |
| R30 HOCH/MITTE/TIEF | case 30 | 0x17c/-110/0x15 @0x80018494-a8, acc -2 im Delay von `j` @0x800184b0 -> `sh v0,8(v1)` im Delay von `jal 0x8001af20` @0x800184dc; 0x118/-50/0x18, -1 @0x800184bc-d4; 0x50/0/1, -1, Zaehler 5 @0x80018518-38 | stimmt (Delay-Slots richtig gelesen) |
| R30 Zaehler | case 30 | RNG FUN_8001af20: `lhu t1` @0x8001af28 wird NIE gelesen; `srl v1,a0,7 / andi 0xff / addu / andi 0xff` @0x8001af30-3c, Rueckgabe `andi v0,a0,0xff` @0x8001af4c; R30: `bgez`/`sra 2`/`sll 2`/`subu` = & 3, `addiu 7` @0x80018504, `sh v0,38(a0)` @0x8001850c | stimmt |
| RNG-Zustand 0x800ac774 | (nicht gefuehrt) | Xref (eigener lui/imm-Scan, EXE + STAGE1..6): nur `addiu v0,v0,-14476` @0x8001af24 und Seed `sw v0,-14476(at)` @0x80031634 | Bauer-Schluss (kein Leser) bestaetigt |
| Wort 0x800acaec | player_common.c:293-318 | ALLE Schreiber gelistet (Xref 30 Stellen): nur Bits 13-15 (Ziel, Gun- und Auto-/Melee-FSM `andi 0x1fff/0xbfff; ori 0x2000/0x4000/0x8000`), Bit 1 (Gift `ori 0x2` @0x80012eac, Heilung `andi 0xfffd` @0x8004b010), Init 0 @0x80031720 (+ SCD Member_set 17). Der Zaehler haengt nur an den Bits 0/1/7/8 -> die Port-Zusammensetzung (s_aim_elev + `status_flags & 0x1fff`) ist inhaltlich vollstaendig | stimmt (aber ungeprueft, §3) |
| R29 komplett | esp_fx_dispatch_b_29 | `lh t1,42(t0)` @0x80018330, `blez` @0x80018338; Liegen SE @0x80018350-58 (a1 = sp+16 unbeschrieben), `ori 0x63; sb 108` @0x80018368-6c, `ori 0x1f; sh 0` @0x80018378-7c, `sh zero,2` im Delay @0x80018384; Abprall: vx `lhu`+`sll/sra 16` = s16, 0x55555556-Idiom (mfhi - Vorzeichen = trunc), `subu a3,a3,a2; sh a3,16` @0x800183c8-cc; Zaehler `addiu -1; sh 38` @0x800183d0-d4; `lw 56; subu t1; sw 56` @0x800183c4/dc/e0; vy `subu zero; sh 18` @0x800183f4-f8; SE-Punkt `lh 40/42/44` @0x800183d8/0x80018400/0x8001840c, Code `lhu 38; sll 8; or 0x010a0001` @0x80018410-28 | stimmt (s16-Lesung, Trunkierung, Reihenfolge) |
| R31 komplett | case 31 | `lhu v1,30(a1)` @0x80018560; `beq v1,zero,0x80018688` @0x80018568; `ori v0,zero,0x1` im Delay von `bne` @0x80018574 -> Latch `sb` @0x8001857c; `ori 0x61; sb 108` @0x80018580-84; P `lh 40/42/44`, `addiu -500` @0x800185a8; `ori a0,zero,0x1f4` @0x80018598, `ori a2,zero,0x2` @0x800185b4, `jal 0x80012d60` @0x800185b8 (`sw v0,24(sp)` im Delay); Kind 0x03195000 a1 = `lh 46` / a2 = 0x80072d4c / a3 = &P @0x800185c0-e0; SE 0x04080001 @0x800185e4-ec; Z2 @0x80018600-64 (Zuender NEU gelesen @0x80018600); Abzug `addiu -1` @0x8001867c + `sh` im Delay @0x80018684; Z0: `sb zero,108(a1)` @0x800186b0 VOR `jal 0x800199d4` @0x800186c8, kein Abzug | stimmt |
| E8-Tabelle | esp_granate_re2_art | RE2 `lbu v0,0(a1)` (0x800cfd06) / `addiu v0,v0,-9` / `sb v0,27(v1)` @0x8001f1a8-b8; RE2-Optab @0x8009d868: [48] 0x80020f3c, [49] 0x800215c8 (`re2_disasm.py table`) -> RE1.5 0x0A (Saeure) = 2, 0x0B (Brand) = 1 | stimmt |
| Tick zwei Durchgaenge | re15_esp_fx_tick | Schleife 1 @0x80019e64-c4 (`andi 0x1` @0x80019e78); Kind-Init `andi 0x8` @0x80019ef4 / `xori 0x9` im Delay @0x80019efc / `sb` @0x80019f00 / `jalr` @0x80019f30 VOR dem Lebend-Gate @0x80019f44-50; Weltlage @0x8001a118-2a4 VOR Routine B @0x8001a2b4-d4; Physik nach Neulesen `lbu 108` @0x8001a2e8: euler += angvel `lhu/addu/sh` @0x8001a2fc-320 (16 Bit), xlat (`lw`, s32) += vel (`lh`, s16) @0x8001a324-360, DANACH vel += acc @0x8001a354-388; Anim @0x8001a38c-47c | stimmt |
| Weltlage Flags&0x80==0 | esp_fx_weltlage | Einheitsmatrix 0x80072d4c -> 0x1f800000; SVECTOR (euler.x, euler.y + `lhu 46`, euler.z) @0x8001a16c-19c; RotMatrix @0x8001a1a0; xlat `lhu 52/56/60` (lo16) @0x8001a1b8-d4; ApplyMatrix @0x8001a1e4; `sh` +0x28/2a/2c @0x8001a1fc/10/20; zweite ApplyMatrix(Anker +0x4c, Versatz +0x40/44/48) @0x8001a248; `addu`+`sh` mit +0x60/64/68 @0x8001a258-2a4 (16 Bit) | stimmt (Port traegt Anker.R*Versatz+Anker.T in x/y/z; 16-Bit-Summe = (int16)(r + x)) |
| RotMatrix-Zwilling | esp_rotmatrix/esp_trig | FUN_80068098 vollstaendig gelesen: Negativzweig `subu t7,zero,t7` + `andi 0xfff` im Delay, sin negiert (`subu t3,zero,t8` @0x800680d0 / `subu t6,zero,t4` @0x80068134 / `subu t5,zero,t8` @0x800681c0), cos unveraendert; alle 9 `sh` (@0x8006816c/80/94|d4, @0x8006820c/2c, @0x80068274/a4/ec, @0x80068318) mit derselben Rundung (Negation VOR `sra 12`) | stimmt; `multu` = gleiche Low-32 wie signed (Produkt < 2^25) |
| Sinus-Tabelle | re15_trig_lut.c | 4096 Worte @0x800794c4 gegen `re15_trig_lut[]` verglichen (eigenes Skript): identisch; Asymmetrie nachgerechnet: 3888 Indizes mit sin(4096-k) != -sin(k), z. B. -sin(24) = -151, sin(4072) = -144 | stimmt |
| ApplyMatrix | esp_applymatrix | @0x800661c0: `ctc2` RT, `lwc2` VXY0/VZ0, `0x4a486012` = MVMVA sf=1, mx=RT, v=V0, cv=3 (keine), lm=0; `swc2` Reg 25..27 = MAC1..3 | stimmt |
| Spawn-Gate/Code/Versatz/Gier | game_step_common.c:1965-1981 | `lbu -13731` / `ori 0x9` / `bne` @0x80033684-8c; 0x13/0x16/0x18 @0x80033690/0x800336a4/0x80033758; Bits 0x8000/0x4000/0x2000 @0x800336b4/0x80033714/0x80033770; `lui a0,0x40d; ori 0x1000`; a1 = `lh -13634` (0x800acabe) @0x800336cc/2c/88; a2 = [0x800acbdc]+0x7a4; Versatz {0,0x12c,0x320}/{0,0,0x1f4}/{0,0,0x12c} | stimmt |
| Recoil-Break | player_common.c (Bestand) | `andi v0,a0,0x100` @0x80033600, Byte2 @0x80074092+(w-1)*5, `sltu v0,v0,a0` @0x8003363c (Bild > Byte2), `sh 3 -> 0x800aca5a` @0x8003364c, dann `j 0x800337ac` = KEIN Spawn | stimmt |
| Drehen im Zielen | player_common.c:1023-1034 | Gun-FSM: RAISE `andi 0x8` -> `subu` Byte0 @0x80033000-48, `andi 0x2` -> `addu` @0x80033050-94; HOLD Byte1 (0x80074091) @0x800333a0-434; ABZUG Byte1 `srl 1` @0x8003355c-fc; LOWER `addiu -24/+24` @0x80033cd8-d1c; RELOAD @0x80033de8-e2c; Melee-FSM gleiche Richtung (`subu` Byte0 @0x80034fd0-5064); virtuelles Bit 3 = LINKS / Bit 1 = RECHTS (pad_common.c-Tabelle = Preset 0x80073dbc); Parametersatz aller Waffen 1,3..16: `18 30 ..` (`read 0x80074090`) | stimmt (Rate 24/48 fuer alle Waffen gleich) |
| Resolver-Tabellen | re15_damage.c (Bestand) | `read 0x8006f418 11 --w 2 --signed` = [10,20,1000,1000,1000,50,100,200,300,1000,0]; `read 0x8006f430 11 --w 1` = [3,3,9,10,11,14,15,16,17,18,20] | stimmt |
| Item-Debug | menu_common.c:1283-1328 | @0x8004a0cc-0x8004a360 komplett gelesen: SELECT `andi 0x100` @0x8004a140, Halbwort `sh v0,9832(at)` @0x8004a150, SE 0x04090000 @0x8004a154-58; Zustand 1 faellt nach 2 @0x8004a194-a0; Zustand 2 Id/0xff/+3 := 0 @0x8004a1f0/204/220, Zustand 3 @0x8004a22c; Zustand 3 `addu v1,a1,zero` (Flanke) im Delay @0x8004a188, R1/L1/R2/L2 = 0x8/0x4/0x2/0x1 (+1/-1/+10/+246), Kreis 0x20, Dreieck 0x10 (`andi v0,a1,0x10` im Delay @0x8004a300, Ziel 0x800ac766-8398+27157+4k = 0x800b10ad+4k); Kappung `sltiu 0x48` / `sb 0x47` @0x8004a350-5c | stimmt bis auf §2.2 (Ruecksetzen beim Oeffnen fehlt) |
| Kind-Spawner | esp_fx_spawn_kind | FUN_800199d4: Suche ab 0 `sltiu v0,t3,0x60` @0x80019a60, `lbu 108` / `beq` @0x80019a7c-84, `ori v0,zero,0xa` @0x80019a88 / `sb` @0x80019aa4, `sh s2,46(t0)` @0x80019ab8, Versatz a3 -> +0x40.. @0x80019abc-dc, a2 -> +0x74 und Matrix -> +0x4c..0x68 @0x80019af4-b30, +0x28/2a/2c := 0 @0x80019b44-4c | stimmt (spawn_ex memset nullt wpos/granate_art ebenfalls) |

Commit-Messages der Code-Commits (b9ee0468, 8aca11ef, a9a772da, 5239b817, 1c0882fa) tragen die Adressen: geprueft.

## 2. Semantik gegen BAUPLAN (Zeitlinie, Einordnung)

### 2.1 Stimmt
* **Zwei-Durchgang-Tick**: Reihenfolge Kind-Init -> Lebend-Gate -> Follow -> Weltlage -> B -> Physik (Flags neu gelesen) -> Anim
  deckt sich Instruktion fuer Instruktion mit @0x80019ee0-0x8001a49c. Ein 0x0a-Kind wird in Durchgang 1 uebersprungen
  (Bit 0 frei) und bekommt Routine A genau einmal in der Kind-Init — wie das Original.
* **Bildzaehlung**: Sonde (Spawn -> Tick = Bild 0) = BAUPLAN §1.1; der unabhaengige Simulator der RE-Gegenpruefung
  (`re_wurf_gegen_werkzeug/wurf_sim_gegen.py`, von mir neu gelaufen) liefert fuer alle fuenf Faelle exakt die Sonden-Erwartung
  (L/X/Z2/frei, Kontaktbilder, Eindringtiefen 136/90/16/25/16/3/9/9, xlat bei L).
* **R30-Satzkonvention** "Satz-1, Zeitgeber 0": gleichwertig, weil Satz 0 (CORE00.ESP @0x1730 `00 01 01 10`) und Satz 23
  (@0x17E8 `10 01 01 10`) je Dauer 1 tragen und Satz 35 (@0x1848 `17 01 ff 10`) auf 23 schleift (eigener xxd). Der Spawner
  selbst setzt `+0x6e := 1` (@0x800198a0-a4) und `+0x6d := Satz0.Dauer` (@0x8001989c/bc) — nachgelesen.
* **R31 Art 3/4 (E8)**: keine HE-Inhalte, Latch nur Art 2, Aufschlag an Q = wpos (nicht P), Gier = +0x2e, explizite Tabelle
  3 -> 2 / 4 -> 1; Zuender-Zeitstruktur (Platz frei bei 0) bleibt.
* **Spielschritt**: ENT[9] = {aktiv 1, resolve 0, ammo 1, n_fx 0} (Entlade 0x80033B38 = nur `jal 0x8004eae4`), Gate 9/10/11 ->
  Art 2/3/4, Gier = rot_y, Versatz je Zielhoehe. Spieler-Tick (`game_step_common.c:1619`) vor Spawn-Block (`:1963`) entspricht
  der Gun-FSM (Recoil-Break @0x80033600-50 VOR dem Spawn @0x80033680).
* **Drehen im Zielen**: Richtung (LINKS = minus) und Raten 24/48/24/24/24 fuer RAISE/HOLD/ABZUG/LOWER/RELOAD in Gun- UND
  Melee-FSM belegt; alle Waffen-Parametersaetze tragen `18 30` -> die feste Rate ist fuer alle Waffen korrekt.
* **ESP-Datenlage (eigener Scan aller 189 Raum-ESPs + CORE00.ESP, `sub`-Offset 0 = unbenutzt ausgeschlossen)**: Routinen
  29/30/31 kommen NUR in CORE00 Effekt 4 sub&7 5 Zeile 0 vor (keine Fremdwirkung der neuen Routinen); KEINE Zeile setzt ueber
  Routinen 3/4/8/16/38 ein Flags-Byte mit Bit 3 (Kind-Init greift nie ungewollt); Winkelgeschwindigkeit ist in ALLEN Zeilen 0
  (euler += angvel ist datenseitig wirkungslos); Raum-ESPs definieren nie die Ids 0/2/3/4/8 (Kinder 3 kommen immer aus CORE00).

### 2.2 Abweichungen / Luecken (Details in §8)
* **Item-Debug-Zustand wird beim Oeffnen des Statusschirms nicht zurueckgesetzt.** Original @0x8004648c `sb zero,9832(at)`
  (0x800b2668) und @0x80046494 `sb zero,9833(at)` (0x800b2669) in derselben Init-Folge wie die 25bc/bd/be/d6-Nullung
  (@0x800463e0-f8) und der Equip-Schnappschuss @0x8004649c (Port: `phase0_init`, menu_common.c:1400ff). Xref: das sind die
  EINZIGEN weiteren Schreiber beider Bytes. Port: `s_dbg_zustand`/`s_dbg_id` (menu_common.c:1283-1284) werden nur in
  `item_debug` geschrieben -> Schliessen ohne KREIS, Wiederoeffnen, R1 im ITEM-Raster ueberschreibt sofort Platz 25bd (nach dem
  Reiter-Bestaetigen 0) mit (Id+1, 255).
* **P8 nur fuer Routine 31 umgesetzt.** BAUPLAN P8 nennt ausdruecklich `re15_esp.c:582-583` (Routine 15) und `:641-642`
  (Routine 8) als Kind-Spawns mit Flags 3; Original rufen beide FUN_800199d4 (`jal 0x800199d4` @0x80017b38 bzw. @0x80017634)
  -> Start-Flags 0x0a (@0x80019a88). Heute `re15_esp.c:645` / `:704` weiter `re15_esp_fx_spawn_rows` (Flags 3). Mit dem neuen
  Zwei-Durchgang-Tick bekommt ein solches Kind auf KLEINEREM Index im Spawnbild Weltlage/B/Physik/Anim, aber KEINE Routine A
  (Original: Kind-Init = A einmal im selben Bild). Weder im Dossier als gebaut noch als OFFEN gefuehrt.
* **Auto-Nachfuehrung maskiert** (Bauer OFFEN 5, hiermit belegt): FUN_8001a8f8 speichert die Schritte OHNE Maske
  (`subu v0,a2,s1; sh v0,106(v1)` @0x8001a97c/88, `addu v1,v1,a0; sh v1,106(v0)` @0x8001a9ac-b0), nur der Einrast-Wert ist
  atan2 & 0xfff (@0x8001a984, `andi v0,v0,0xfff` @0x8001a768). Port `player_common.c:1053` maskiert jeden Schritt. Folge: die
  Granaten-Gier liegt nach einem Nachfuehr-Schritt ueber 0 hinweg im Port bei 4096-k statt -k -> anderer Tabellenzweig
  (-sin(k) vs sin(4096-k) = -sin(k-1)), bei 12000 Wurfweite etwa 20 Einheiten seitlich. Die Nachfuehrung laeuft vor fast jedem
  Wurf (a_wurf1: Gier 215 = auf den Zombie nachgefuehrt).

### 2.3 P8 (Kind-Spawner) — Xref beider Spawner
Eigener `jal`-Scan der EXE: FUN_80019700 (Start-Flags 3) wird NUR ausserhalb des ESP-Ticks gerufen (0x8002c74c..0x8002c8fc,
Gun-FSM 0x800336ec..0x80033e88, 0x800348b0..0x80034bdc, 0x80038794/bc, 0x80041954, 0x80045710). ALLE Kind-Spawns aus
ESP-Routinen gehen ueber FUN_800199d4 (Start-Flags 0x0a): R2 @0x800172f8, **R8 @0x80017634**, **R15 @0x80017b38**, R19
@0x80017da0, R25 @0x80018054, R31 @0x800185dc/640/660/6c8, R39 @0x800189c4..0x80018c68, R43 @0x800191e0 (Tabelle
0x80071d40 gegen die Aufrufadressen gelegt). Der neue Code-Kommentar im Tick (re15_esp.c, Durchgang 1: "Ein in Durchgang 1
an einem KLEINEREN Index gespawntes Kind (FUN_80019700, Flags 3) bekommt im Spawnbild jetzt Weltlage/B/Physik/Anim, aber
kein A — wie im Original") beschreibt damit einen Fall, den das Original nicht hat; er trifft nur die zwei Port-Routinen
R8/R15, die noch mit Flags 3 spawnen.

## 3. Sonde `probe_r34_wurf` — echt? unabhaengig? Negativ-Kontrollen? eigene Mutationsproben

* **Echte Engine**: ja — CORE00.ESP (shared_assets) als globale Bank, `re15_esp_fx_tick`, `re15_resolve_attack`, Abschnitt 6 mit
  `re15_game_step` in ROOM1140, Abschnitt 7 mit der echten Menue-FSM. Spione nur auf den Haken (SE/Aufschlag).
* **Erwartung unabhaengig**: Zeitlinien/Tiefen/Wege aus dem Simulator der RE-Gegenpruefung (von mir neu gerechnet, identisch);
  Kinder-Lebensdauern aus den CORE00-Saetzen (BAUPLAN §1.2); Item-Debug aus Disasm + Original-Lauf n1. Nicht aus dem Code
  abgeschrieben.
* **Negativ-Kontrollen vorhanden**: 55 (Gier 0 != 1024), 72/73 (Pool voll), 75 (Raumwechsel ohne Resolver), 76 (a ohne Zielbits),
  77/78 (Spieler 1000 unversehrt / 949 getroffen), 79 (Klemme fuer Huelse bleibt), 110 (R1 los nur bis Bild 8 -> Spawn), 132 (R1
  ohne SELECT), 148 (SELECT im Reiter-Modus).
* **Baseline** (gebaut im Bauverzeichnis des Baums, `build_r34_a`): `probe_r34_wurf: ALLE PRUEFUNGEN GRUEN`.

### 3.1 Eigene Mutationsproben
Skript (Scratchpad, `gp_a_r34/mut.py`): Datei binaer sichern, genau EINE Stelle ersetzen (Muster muss genau einmal passen), Sonde
bauen + laufen, byte-gleich zuruecksetzen, `git diff --quiet` je Datei; am Ende Neubau + Lauf GRUEN, `git status` ohne
Code-Aenderung.

| Nr | Mutation (Datei) | Ergebnis |
|---|---|---|
| G1 | R29 `vy := +trunc(vy/3)` statt `-` (re15_esp.c:1006) | ROT 17 (+34 weitere) |
| G2 | RotMatrix-Negativzweig durch `a & 0xfff` ersetzt (re15_esp.c:1236-1238) | **GRUEN** — keine Pruefung faehrt einen negativen Winkel |
| G3 | Kind-Start-Flags 0x03 statt 0x0a (re15_esp.c:1121) | ROT 28 (nur der Rauch #2 auf dem eigenen Platz; 26/27 bleiben gruen, weil die Kinder dort auf hoeheren Indizes landen) |
| G4 | `re15_player_acaec`: HOCH/TIEF-Bit vertauscht (player_common.c:314) | **GRUEN** — die Zusammensetzung wird nie gefahren |
| G5 | `re15_player_acaec` ohne Status-Unterbits (Gift) (player_common.c:312) | **GRUEN** — dito |
| G6 | Weltlage NACH Routine B (re15_esp.c:1407) | ROT 17 (+33) |
| G7 | Zuender-2-Kinder bei 3 (re15_esp.c:918) | ROT 27, 30 |
| G8 | P.y = Welt-y - 400 (re15_esp.c:572) | ROT 21, 26, 29 |
| G9 | E8-Tabelle Saeure/Brand vertauscht (re15_esp.c:583) | ROT 59, 64 |
| G10 | Art 10/11 im Spawn-Gate vertauscht (game_step_common.c:1967) | ROT 101, 107 |
| G11 | Ziel-Drehung LINKS = plus (player_common.c:1033) | ROT 111, 112 |
| G12 | Item-Debug L2 = +245 (menu_common.c:1314) | ROT 141, 142 |
| — | Rueckbau | `ZURUECK: bau OK, sonde rc=0 GRUEN`, `git status` nur Dossier + build/ |

Urteil: die tragende Mechanik (R29, R30-Geschwindigkeiten, R31-Zeitlinie/Inhalt, Weltlage-Reihenfolge, Kind-Flags, E8, Gate,
Drehen, Item-Debug) ist von der Sonde gedeckt. **Drei tote Stellen**: G2 (Negativwinkel — genau der Zweig, fuer den der eigene
RotMatrix-Zwilling gebaut wurde, und seit dem Wegfall von `& 0xfff` im Zielen im Spiel real erreichbar: LINKS aus Gier < 24),
G4/G5 (Abschnitte 1-5 setzen das Wort per `re15_player_acaec_override_for_test`, das Ueberschreiben bleibt bis zum Ende von
`main` an — auch Abschnitt 6 faehrt NICHT die echte Zusammensetzung; HOCH/TIEF/Gift aus der Ziel-FSM sind nirgends geprueft).

## 4. Dateibesitz

`git diff --stat 8d8651e4..HEAD`: `re15_esp.c`, `re15_esp.h`, `game_step_common.c`, `player_common.c`, `menu_common.c`,
`probe_r34_wurf.c`, `probes/r34_wurf.cmake`, `bau_a.md` — alles Spur-A-Dateien laut BAUPLAN §3.0. Keine fremde Datei
geaendert; Wuensche an fremde Dateien stehen als INTEGRATIONSWUNSCH 1-8 im Dossier. `build/` (Laufzeit-Ausgaben)
unversioniert. **Eingehalten.**

## 5. Sonden und Bestandstests im Bauverzeichnis des Baums (`re15_port/build_r34_a`)

### 5.1 Bau
`ninja -n` zeigte nach meinen Mutationslaeufen nur Relinks (0 Uebersetzungen: die Quellen sind byte-gleich HEAD). Voller
`cmake --build build_r34_a` (602/602, rc 0) in der Umgebung von local_build.sh, OHNE dessen Kill-Rueckfall.

### 5.2 ctest (429 Tests)
| Menge | Ergebnis |
|---|---|
| alle Nicht-`integration_`-Tests (398, `-j 4`) | **398/398 gruen** (u. a. unit_r34_wurf, unit_r30_granate, probe_abzug_takt, unit_aim_all_around, unit_aim_lower_exit, r29_1d3_doppelabzug, r30b_muendungshoehe, unit_r26_mg_blut, unit_inv_fsm, unit_espr_11e0, unit_montage_fx) |
| C-Integrationstests #411-#424 und #429 (u. a. room_transition, door_traversal, flag/item_name_census, fx_region_cull, pri_masken) | 15/15 gruen |
| `integration_r30_granate_laden` (exe) | gruen |
| uebrige exe-Tests (15) im Stapel | 10 gruen, 5 rot: irons_tisch_laden/bild/licht, sicherung_laden (je exit=1 nach 5-20 s), r32_tor_hell (Zeitablauf 120 s) |
| die 5 einzeln wiederholt | 4 gruen; sicherung_laden erneut exit=1 nach **0,34 s**, zeitgleich vom Waechter erfasst: `03:22:49.563 taskkill.exe /F /IM re15_pc.exe` und `03:22:56.405 taskkill.exe /F /IM re15_pc.exe` (Fremdprozesse); dritter Lauf allein **gruen** (43,46 s) |

**Ergebnis: alle 429 Tests im Baum gruen, kein reproduzierbares Rot.** Die Zielzeile `=== LOCAL-BUILD-OK (all) — Tests 429/429`
existiert trotzdem nicht (kein ungestoerter Gesamtlauf moeglich, §5.4).

### 5.3 Eigene exe-Messung 0x0A / 0x0B (fehlte beim Bauer: seine exe-Laeufe waren nur Id 9)
Skript des Bauers `build/r34g_a/lauf_a.sh` (beschleunigter Renderer, `RE15_NOAUDIO`, ROOM1140 per `RE15_DEBUG_JUMP=1140@250`,
kein AUTOSHOT/SOFTWARE_RENDER), Ausgaben `build/r34g_a/gp_*`.
* `gp_saeure1..3` (GIVE/EQUIP 10): **dreimal rc 1** — Lauf 1 endet 03:24:16, Waechter: `03:24:16.488 taskkill.exe /F /IM
  re15_pc.exe parent=... local_build.sh build`; Lauf 2: `03:24:39.220 taskkill /F /IM re15_pc.exe`; Lauf 3 ohne Eintrag
  (Waechter pollt alle 150 ms), kein Absturzeintrag im Anwendungsprotokoll.
* Gegenprobe mit **byte-gleicher Kopie der exe unter anderem Namen** (`gp_a_re15.exe`, md5 gleich, danach geloescht):
  `gp_saeure4` und `gp_brand1` laufen durch (rc 0) — damit ist der Kill per Bildname als Ursache belegt.
  * 0x0A: `[equip] W-bank -> W0A (Clips 13, Rueckstoss-Clip7 fc=35)`; `F=382 SPAWN granate art=3 slot=0 anker=(-6851,-2474,-18279)
    gier=215`; Liegen T=702 `010a0001` an (4996,9,-20488); T=738 `EV resolver art=3 P=(4996,-491,-20488) r=500`, `EV aufschlag
    re2_art=2 q=(4996,9,-20488) gier=215`; T=745 `EV frei art=3`; kein Latch, kein Kind, kein SE 0x0408.
  * 0x0B: `W-bank -> W0B`; `art=4`; T=738 `EV resolver art=4`, `EV aufschlag re2_art=1`; T=745 frei.
  -> Wurf, Flug, Zuender und Uebergabe fuer beide Granaten in der echten exe wie Sonde/BAUPLAN (Aufschlag-Haken noch NULL bis C/D).

### 5.4 Infrastruktur: local_build.sh beendet jede re15_pc.exe (Bauer-Selbstbefund bestaetigt und ergaenzt)
* `tools/local_build.sh:293-300`: `command -v powershell` unter CLEAN_PATH (`:147`) — nachgemessen mit genau diesem PATH:
  `powershell: FEHLT`, `cygpath: /usr/bin/cygpath`, `taskkill: /c/Windows/System32/taskkill` -> IMMER `taskkill //F //IM re15_pc.exe`.
* Ergaenzung: nicht nur `build` — auch `test` (`do_build && do_test`, `:360`) und `all` (`:361`) laufen durch do_build. Waehrend
  meiner Pruefung liefen `local_build.sh test` (build_r34_b, build_r34_c) und `local_build.sh build` (build_r34_d) parallel; jeder
  ihrer Starts hat meine exe-Laeufe beendet (§5.2/§5.3).

## 6. Regressionsrisiko fuer bestehende Effekte / Gegner / Waffen

* **Zwei-Durchgang-Tick (alle Zeilen-VM-Effekte)**: durch den ESP-Datenscan (§2.1) begrenzt — keine fremde Zeile nutzt 29/30/31,
  keine setzt Bit 3, keine hat Winkelgeschwindigkeit. Rest-Risiko: Kinder von R8/R15 auf kleinerem Index (Muendungs-
  Zweitblitz Kategorie 2, Burst-Huelsen) bekommen im Spawnbild jetzt Physik/Anim VOR ihrer ersten Routine A (vorher: gar nichts
  im Spawnbild; Original: A in der Kind-Init) — 1-Bild-Abweichung, siehe P8.
* **Drehen im Zielen (alle Waffen)**: gewollte, belegte Verhaltensaenderung — RAISE/ABZUG/LOWER/RELOAD drehen 24 statt 72 je Bild,
  HOLD mit OBEN/UNTEN dreht jetzt 48 (vorher netto 0); HOLD ohne OBEN/UNTEN war vorher zufaellig richtig (-96 + 48 = -48). Die
  Gier kann jetzt negativ werden (kein `& 0xfff`) — wie im Original; dadurch wird der ungetestete Negativzweig (G2) im Spiel real.
* **wpos fuer ALLE Plaetze** (erst mit C2 sichtbar): der Hinweis an Spur C (bau_a.md §6) nennt Granate, W14 und Huelsen
  (param 0). Er ist UNVOLLSTAENDIG: Blut-Stroeme des RE2-Zombies (`enemy_ai_re2_zombie.c:1113`, param = yaw) und des Hundes
  (`enemy_ai_re2_dog.c:376-378`, param = rot_y + off) sind Zeilen-VM-Plaetze mit xlat != 0 und Gier != 0 — ihre gezeichnete
  Flugrichtung aendert sich mit C2 (Original-Verhalten: RotY(+0x2e) wirkt auf xlat). C2 muss das bei Blut-Pins erwarten.
* **Zwischenstand ohne C** (Merge-Reihenfolge BAUPLAN §4: A vor C): der Zeichner nimmt noch `x + xlat` (ungedreht), Physik und
  Explosion laufen auf dem gedrehten wpos -> fuer Gier != 0 fliegt das gezeichnete Sprite in eine andere Richtung als die
  Granate, die explodiert (Bauer-Messung a_wurf1, Gier 215; INTEGRATIONSWUNSCH 2). R30 laeuft bis C1 ein Bild spaet. A darf nicht
  ohne C1/C2 an den Nutzer gehen.
* **Gegner**: A ruft den Resolver fuer Art 2/3/4 erstmals wirklich auf (vorher: Hitscan-Bruecke). Die Reaktion der Typen ist
  Spur B; die A-Sonde deckt nur den Dummy 0x27 und den Spieler. Die exe-Abnahme "Explosion trifft Zombie ohne Haenger" (der
  Kern des Nutzer-Befunds) fehlt noch (Bauer OFFEN 6; auch meine Laeufe: Landepunkt (4996,-20488) liegt ~5000 von den Zombies
  in ROOM1140 bei x -1800..200 / z -21600..-19600) und gehoert zwingend in `test_r34_granaten` (beide Flavors).
* **Item-Debug**: SELECT im ITEM-Raster oeffnet im Port zusaetzlich das UTILITY/DEBUG-MENU (Bauer-Messung; INTEGRATIONSWUNSCH 6,
  von mir bestaetigt: einziger Aufrufer von FUN_8001443c ist `jal 0x8001443c` @0x8001c988, keine Zeiger-Referenz). Dazu kommt
  der fehlende Reset beim Oeffnen (§2.2).

## 7. Luecken gegenueber dem Auftrag der Spur

1. P8 nur fuer R31 (R8/R15 weiter Flags 3) — weder gebaut noch als OFFEN gefuehrt.
2. A10: Ruecksetzen 0x800b2668/69 beim Oeffnen des Statusschirms fehlt (@0x8004648c / @0x80046494).
3. Sonde: Negativwinkel (G2) und echte `re15_player_acaec`-Zusammensetzung (G4/G5, inkl. Gift-Bit aus `status_flags`) ungeprueft.
4. Volle Suite: Zielzeile `=== LOCAL-BUILD-OK (all)` nie erreicht (Ursache belegt, §5.4); alle 429 Tests einzeln gruen (§5.2).
5. OFFEN 5 des Bauers ist mit §2.2 statisch beantwortet (Original maskiert die Nachfuehr-Schritte NICHT) — Korrektur in
   `player_common.c:1053` steht aus.
6. exe-Treffer an einem Zombie (Nutzer-Absturzszenario) weiter ungemessen (Integration).

## 8. MAENGEL

| Nr | Schwere | Ort | Mangel | Beleg | Vorschlag |
|---|---|---|---|---|---|
| M-1 | kritisch (Infrastruktur, NICHT Spur-A-Code) | re15_port/tools/local_build.sh:293-300 (+ :147, :360-361) | `build`, `test` und `all` beenden jede re15_pc.exe der Maschine (auch die exe des Nutzers im Hauptbaum und alle exe-Tests paralleler Spuren) | PATH-Messung `powershell: FEHLT`; Waechter 03:22:49 / 03:22:56 / 03:24:16 (`local_build.sh build`) / 03:24:39; drei eigene exe-Laeufe rc 1, byte-gleiche Kopie unter anderem Namen rc 0 | powershell mit absolutem Pfad `/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe` aufrufen, `taskkill //IM`-Rueckfall streichen (INTEGRATIONSWUNSCH 8); danach die volle Suite fuer A erneut |
| M-2 | mittel | re15_port/engine/src/menu_common.c:1283-1284, :1400 (phase0_init) | Item-Debug-Zustand/Id werden beim Oeffnen des Statusschirms nicht auf 0 gesetzt -> nach Schliessen ohne KREIS ueberschreibt der naechste R1/L1/R2/L2/Dreieck im ITEM-Raster Platz 25bd | Original @0x8004648c `sb zero,9832(at)`, @0x80046494 `sb zero,9833(at)` in der Oeffnungs-Init (neben 25bc/bd/be/d6 @0x800463e0-f8, Snapshot @0x8004649c); Xref: einzige weitere Schreiber | in `phase0_init` `s_dbg_zustand = 0; s_dbg_id = 0;` mit beiden Adressen; Sonde: Debug in Zustand 3 lassen, schliessen, oeffnen, R1 -> Platz unveraendert (Negativ-Kontrolle) |
| M-3 | mittel | re15_port/engine/src/re15_esp.c:645 (R15), :704 (R8), Kommentar Durchgang 1 im Tick | BAUPLAN P8 nennt R15/R8 als Kind-Spawns mit Flags 3; umgebaut wurde nur R31. Mit dem Zwei-Durchgang-Tick bekommt ein R8/R15-Kind auf kleinerem Index Physik/Anim vor seiner ersten Routine A; der Kommentar behauptet dafuer "wie im Original" | R8 `jal 0x800199d4` @0x80017634, R15 @0x80017b38; FUN_800199d4 `ori v0,zero,0xa` @0x80019a88; jal-Scan: FUN_80019700 hat KEINEN Aufrufer im ESP-Tick | R8/R15 ueber den 0x0a-Kern (`esp_fx_spawn_rows_core(..., 0x0a, ...)` mit ihrer bisherigen Lage/floor) spawnen, Kommentar korrigieren, Sondenfall "freier Platz unter dem Eltern-Index" |
| M-4 | mittel | re15_port/tests/unit/probe_r34_wurf.c (Abschnitte 1-6) | Zwei tragende Pfade ohne Pruefung: RotMatrix-Negativzweig und die echte Zusammensetzung von `re15_player_acaec` (Override bleibt bis `main`-Ende an) | eigene Mutationen G2, G4, G5 bleiben GRUEN | Wurf mit Gier -24 (Erwartung aus FUN_80068098: sin -151/cos 4093, NICHT sin(4072) = -144); Abschnitt 6 mit Override AUS: HOCH/TIEF ueber die Ziel-FSM und Gift-Bit in `status_flags` -> Zaehler 9 |
| M-5 | mittel | analysis/befunde_runde34_granaten/bau_a.md §8 / Abnahme A | Volle Suite ohne Zielzeile (Auftrag verlangt `LOCAL-BUILD-OK (all)`) | Bauer Laeufe 1-5 ohne Zielzeile; eigene Laeufe: 429/429 einzeln gruen, Stapel nur durch Fremd-Kills rot | nach M-1 einen ungestoerten Gesamtlauf nachholen, bevor A gemergt wird |
| H-1 | hinweis | re15_port/engine/src/player_common.c:1053 | Auto-Nachfuehrung maskiert jeden Schritt `& 0xfff`, Original nicht -> Granaten-Gier im falschen Tabellenzweig (~20 Einheiten seitlich bei 12000) | FUN_8001a8f8 `sh v0,106(v1)` @0x8001a988, `sh v1,106(v0)` @0x8001a9b0 ohne Maske; nur Einrasten = atan2 & 0xfff @0x8001a768/84 | Maske in den Schrittzweigen entfernen (schliesst Bauer-OFFEN 5) |
| H-2 | hinweis | analysis/befunde_runde34_granaten/bau_a.md §6 (Hinweis an C) | Liste der Plaetze, deren wpos von x + xlat abweicht, unvollstaendig (Blut RE2-Zombie/Hund mit Gier) | `enemy_ai_re2_zombie.c:1113` (param yaw), `enemy_ai_re2_dog.c:376-378` (param rot_y+off) | C2 ergaenzen: Blut-Richtung aendert sich sichtbar, Pins entsprechend |
| H-3 | hinweis | Integration (main.c, Spur C) | Zwischenstand A ohne C1/C2: Sprite ungedreht, Explosion gedreht; R30 ein Bild spaet | a_wurf1 (Gier 215); INTEGRATIONSWUNSCH 1/2 | A nur zusammen mit C1/C2 an den Nutzer ausliefern |
| H-4 | hinweis | Integration `test_r34_granaten` | Nutzer-Absturzszenario (Explosion trifft Zombie) in der exe weiter ungemessen | Landepunkte der Laeufe ~5000 von den ROOM1140-Zombies | Integrationstest mit Wurf nah an einen Zombie, beide Flavors, Reaktion im Bild X+1, kein Haenger |
| H-5 | hinweis | re15_port/platform/pc/main.c:7170-7171 (Spur C) | SELECT im ITEM-Raster oeffnet zusaetzlich das UTILITY-Menue | einziger Aufrufer `jal 0x8001443c` @0x8001c988 | INTEGRATIONSWUNSCH 6 umsetzen |

## 9. FAZIT

Spur A ist in der Kernmechanik korrekt und byte-true belegt: Zwei-Durchgang-Tick, Weltlage mit RotMatrix-/ApplyMatrix-Zwilling,
Routinen 29/30/31, Kind-Spawner 0x0a, Latch, Spawn-Gate 9/10/11, Entfall der Hitscan-Bruecke und die Drehkorrektur stimmen
Instruktion fuer Instruktion mit meiner eigenen Disassembly; die Sonde faehrt die echte Engine mit unabhaengiger Erwartung und
ist in der tragenden Mechanik mutationsfest (9 von 12 eigenen Mutationen rot). Alle 429 Tests des Baums sind gruen (exe-Rot nur
durch Fremd-Kills, belegt); in der echten exe laufen jetzt auch Saeure- und Brandgranate bis zur Uebergabe. Offen und vor dem
Merge zu schliessen: der fehlende Item-Debug-Reset beim Oeffnen (M-2), P8 fuer R8/R15 (M-3), die drei toten Sondenstellen (M-4)
und ein ungestoerter Gesamtlauf nach der Korrektur von local_build.sh (M-1/M-5). A darf nur zusammen mit C1/C2 zum Nutzer.
