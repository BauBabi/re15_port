# BEFUND C — Gruppe „gore-vollausbau": Kopf-Explosion, Torso-Abschuss, Stumpf-Geometrie

Nutzer 2026-09-12: „Es fehlt immernoch diverse Brutalitaet aus Resident Evil 2 — man kann
Torsos abschiessen, der Kopf fliegt jetzt weg, aber in RE 2 explodiert er quasi etc."

Dieses Dossier ist die UMSETZUNGS-SPEZIFIKATION fuer abtrennen.md Schritt 3 (packed-id-FX)
und Schritt 4 (Stumpf-Geometrie) — mit fertigen Bau-Daten. Alle Dumps frisch gezogen
(2026-09-12): `re2_disasm.py dis … --bin EMZ0.BIN` (info/re2leon, Overlay raw @0x80100000),
Python-Parser direkt auf CORE00.ESP (beide Spiele), RE2-RDTs (info/re2leon/PL0/RDT),
CDEMD0.EMS (beide Spiele). Vorbefunde: analysis/schrot_befunde_2026-09-12/abtrennen.md,
analysis/befunde_runde3_2026-09-12/kopf-wegschiessen.md.

## 0. IST-Stand (heute gemessen, nicht aus den Vorbefunden uebernommen)

* kopf-wegschiessen Schritt 1 IST VERDRAHTET: `re2z_hit_ragdoll` P0 prueft
  `re15_player_aim_elevation() > 0` → `re2z_rag231 = 2` → `re2z_death_magnum`
  (enemy_ai_re2_zombie.c:5636-5642, Original @0x8010679C-BC). Der Kopf FLIEGT.
* kopf-wegschiessen Schritt 2 (Teile-Masken-Stempel) ebenfalls drin (re15_damage.c:1856-58).
* Was FEHLT ist exakt die Meldung des Nutzers: (a) die drei 92C4-Blutspawns laufen als EIN
  generischer Sprite an der HUEFTE (`re2z_blood_fx_at` → `re15_esp_fx_spawn_ex(room, 0, 0,
  0x1500, PartWeltpos…)`, enemy_ai_re2_zombie.c:1007-1010 — id/Anker/Skalen des Originals
  verworfen, :3923-3927 `(void)packed_id`); (b) der Rumpf-/Bein-STUMPF wird weggeklemmt
  (main.c:8391 `gore_mesh[nbi] < mesh_count`), weil `re2_hybrid_apply` das RE2-MD1 ersetzt
  (re2_ems.c:372).

---

## 1. DIE GEPACKTE EFFEKT-ID IST IN BEIDEN ENGINES DASSELBE WORT (der Schluessel-Befund)

**RE2 `FUN_8001bf10`** (RE2_Quellcode_V2/FUN_8001bf10.c:23-28, 57):

```c
iVar4  = (param_1 >> 0x18) * 4;                    // GRUPPE  = Byte 3 → Tabellen-Index
uVar15 =  param_1 >> 0x10 & 0xff;                  // SUB     = Byte 2
… *(ushort*)((param_1 >> 0x10 & 7)*2 + rowbase) …  // sub&7   = Stream-Satz-Wahl
… uVar3 + (short)(uVar15 >> 3) * 0x40;             // sub>>3  = CLUT-Zeilen-Shift (+0x40)
*(uint *)(&DAT_800d8d28 + iVar4) = param_1 << 0x10; // low16  = SCALE (Q12)
```

**RE1.5 `FUN_80019700`** (RE_15_Quellcode_V2/FUN_80019700.c:23-28, 45, 88) — WORTGLEICH:
`(param_1 >> 0x18)` indiziert `DAT_800b2248`/`DAT_800b22d4`, `(param_1>>0x10&7)*2` waehlt
den Stream-Satz, `((param_1>>0x10&0xff)>>3)*0x40` shiftet die CLUT, und
`*(short*)(slot+0x72) = (short)param_1` = die Q12-Skala — exakt das Feld, das der Port
schon als `f->scale16` fuehrt (re15_esp.c:334 „spawn packed-arg low16 (Q12; @entry+0x72)").

**Dekodier-Regel (byte-true, beide Engines):**

```
effekt_id = packed >> 24;   sub = (packed >> 16) & 0xFF;   scale16 = packed & 0xFFFF (Q12)
```

**Die GRUPPE ist die Effekt-Id der ESP-Datei** — der Registrar beweist es
(RE2_Quellcode_V2/FUN_8001bca0.c:24-26):

```c
*(uint **)(&DAT_800d4cd8 + id*4) = eff_body;                      // Header-Zeiger je ID
*(uint **)(&DAT_800d4e18 + id*4) = eff_body + (ca*2 + cb + 2);    // = ROWBLK (dieselbe
                                                                  //   Formel wie re15_esp.c:120)
```

Raum-Bank (RDT-ESP, Registry-Slots 8-15, `FUN_8001bba4` ruft `FUN_8001bca0(…, 8)`) und
Global-Bank (CORE00.ESP) teilen den ID-NAMENSRAUM — genau die Room-first-Global-fallback-
Aufloesung, die `re15_esp_fx_spawn_rows` (re15_esp.c:745-751) bereits implementiert.

Der Port-Spawner ist also der 1:1-Zwilling: **`re15_esp_fx_spawn_rows(bank, packed>>24,
(packed>>16)&0xFF, packed&0xFFFF, x, y, z, floor, yaw)`** — `param`(=a1-Winkel) landet wie im
Original bei slot+0x2E (FUN_80019700 `*(u16*)(slot+0x2e) = param_2`).

### 1.1 Die Effekt-Banken, frisch gedumpt

| Bank | Ids | Zombie-relevante Semantik |
|---|---|---|
| RE2 CORE00.ESP (0x217C B) | **3,5,0,1,2,6,7,4** (Header 8 B @0) | 0=Blut, 5=Feuer/Brand, 4=Aetz-Spritzer, 6=Spark |
| RE2 ROOM1000.RDT off[18] | **13,9,14,8,10** | 8=Fleischbrocken, 9=Brocken-Kette, 10=Beiss-FX |
| RE2 ROOM1010.RDT off[18] | 9,8,17,18,19,20,10,24 | dito (Raum-Ids max 8, Installer-Kappe `uVar5<8`) |
| RE1.5 CORE00.ESP (0x1BB4 B) | **3,8,0,2,4** | 0=Blut, 3=Rauch/Muendungsblitz, 2=Muendungsfeuer, 4=Huelse/Projektil, **8=FEUER** (Beleg: render_pc.c:198-199 „44 = GLOBAL-Effekt-Id 0x08 (FEUER, CORE00.ESP)", ROOM1090-Truemmerflammen) |
| RE1.5 Zombie-Raeume (1140/1240/10D0/10C0) | **5,7** | Brocken-Kette (s.u.); 1190 nur {7}, 1090 {5,7,9,16} |

### 1.2 ⛔ Beweis: RE2-Raum-Id 8/9 == RE1.5-Raum-Id 5/7 — BYTE-IDENTISCHE Sprite-Bodies

Byte-Vergleich (Python, EFF-Body = (ca*2+cb+2)*4 B ab Body-Start):

* RE2 ROOM1000 **id 8** (ca=9, cb=29, 196 B) == RE1.5 ROOM1140 **id 5**: **identisch=True**.
* RE2 ROOM1000 **id 9** (ca=11, cb=9, 132 B) == RE1.5 ROOM1140 **id 7**: **identisch=True**.

Capcom hat die Brocken-Kette von RE1.5 nach RE2 UMNUMMERIERT (5→8, 7→9); nur die
Row-Programme wurden weiterentwickelt. **Die Ziel-Ids fuer die RE2-Brocken sind damit
byte-belegt: (8,·) → Raum-Id 5, (9,·) → Raum-Id 7.**
⛔ NIEMALS die Identitaet 8→8 nehmen: RE1.5-**CORE00**-Id 8 ist FEUER — ein Raum ohne Id 5
wuerde beim Global-Fallback Flammen statt Fleisch spawnen.

### 1.3 Blut-Subs decken sich semantisch (RE1.5-Praesentation vorhanden)

RE1.5 CORE00 id 0 (rowblk @0x92C, Sub-Offsets 4/68/163/340 — nur Subs 0-3 existieren):

| Sub | Streams | Inhalt (Row 0: accel / vel) |
|---|---|---|
| 0 | 3 | Ballistik-Buendel Routine 3: (-2,8,0)/(74,-70,0) … — der Spray |
| 1 | 4 | flaches Buendel + 1 Kettenglied Routine 4 |
| 2 | **6** | **FONTAENE**: Strahl (0,10,0)/(0,-100,0) aufwaerts + 5 Landeklecks-Ketten Routine 4 |
| 3 | 1 | Einzel-Klecks Routine 5 |

RE2 id 0 nutzt im Zombie-Gore Sub 0 (Strahlen) und Sub 2 (0x0002xxxx, Fontaene) — dieselben
Sub-Nummern tragen in RE1.5 dieselben Klassen. **Sub wird 1:1 durchgereicht** (Ids 0-3;
RE1.5-Id 0 hat keine Subs 4-7 — kommt im Zombie-Gore auch nicht vor).

---

## 2. AUFTRAG 1 — KOPF-EXPLOSION KOMPLETT: der 92C4-Spawn-Zensus

`FUN_801092C4`, Phase 0 (= der EINE Frame, in dem die Todeszelle betreten wird; frisch
disassembliert 0x80109330-0x801094F0 — es gibt in 92C4 KEINE weiteren Spawns in P1-P4,
jal-Scan des Overlays: naechste 0x8001bf10-Site nach 0x80109438 ist 0x80109710 = 9610/Burst):

| # | @jal | a0 packed | dekodiert (id,sub,scale) | a1 Winkel | a2 Anker | a3 Positions-Vektor |
|---|---|---|---|---|---|---|
| 1 | @0x801093D4 (a0 @0x801093C0) | 8000 | **(0, 0, 8000=1.95x)** Blut-Strahlen | +0x76 (lh 118 @0x801093CC) | Part-0-Matrix (+0x198+72, @0x801093C8/D0) | {rec8+0x2C, rec8+0x30 **+300**, rec8+0x34} @0x801093A0-C4 = **Halsansatz** (Kopf-Part-Position, y+300 nach unten) |
| 2 | @0x80109420 (a0 @0x801093FC-400) | 0x08001B58 | **(8, 0, 7000=1.71x)** FLEISCHBROCKEN → RE1.5-Raum-Id **5** | Peilung zum Spieler (FUN_800154AC @0x801093F4: self+0x38/+0x40 vs 0x800CFC30/38) | Part-0-Matrix | wie #1, aber y **-400** (@0x8010941C-24) = ueber dem Hals |
| 3 | @0x80109438 (a0 @0x80109428-2C) | 0x00021F40 | **(0, 2, 8000)** Blut-FONTAENE | +0x76 (@0x80109434) | Part-0-Matrix | wie #2 |

Danach (kein Spawn, aber Teil des „explodiert"-Bildes):
* Kopf-Part 8: `flags|=0x4A` (fliegt, Drift-Physik, verschwindet nach 29 Frames
  `sltiu 0x1d` @0x80028DC8), Kinetik 400/-100/10/-50, Kurs Peilung+2048, Tinte 0x00101040
  (@0x80109460-98) — im Port komplett (enemy_ai_re2_zombie.c:5124-5250).
* `FUN_8001CEFC(5, 3, &Part8-Matrix)` @0x80109494 = loescht die (Gruppe 5, Sub 3)-Emitter
  am Kopf — die BRAND-Tinten-FX 0x0503xxxx (Dekodierung passt exakt).
* Stumpf-Marker Part 0 (0x80, 64/0x00101010) @0x801094A8-C8; SE ENEMSE 2 @0x801094C4;
  50 % kopfloses Weiterlaufen @0x801094D8-F0.

**a3-Semantik (belegt):** param_4 ist die SPAWN-POSITION relativ zur Anker-Matrix. Beweis:
RE1.5-Aufrufer FUN_8002c444.c:100-124 uebergibt WELT-Koordinaten (`local_40 = X+900,
local_38 = Z±900`) gegen die STATISCHE Matrix `&DAT_80072d4c` — mit Null-Anker ist der
Vektor die Weltposition; mit Part-Anker ist er der Versatz. rec+0x2C/+0x30/+0x34 = die
Part-Position im Anker-Raum (der Haupt-Handler benutzt dieselben Felder als Emitter-Ursprung,
Blockkommentar enemy_ai_re2_zombie.c:4828). **Port-Aequivalent: Kopf-Bone-WELTPOSITION
+ (0,+300,0) bzw. (0,-100,0)** — heute spawnt der Port alle drei an der HUEFTE (Part 0),
das ist eine sichtbare Divergenz (Fontaene gehoert an den Hals).

### 2.1 Der komplette Zombie-Gore-Id-Zensus → FERTIGE MAPPING-TABELLE

Alle 88 `jal 0x8001bf10`-Sites in EMZ0.BIN gescannt; die Gore-relevanten mit Dekodierung.
Ziel-Spalte = `re15_esp_fx_spawn_rows(bank, id, sub, scale, x,y,z, floor=e->y, yaw)`:

| RE2 packed | (id,sub,scale) | Quelle/Frame | Anker + Offset | → Port-Ziel (Bank, Id, Sub, Scale) | Status |
|---|---|---|---|---|---|
| 8000 | (0,0,8000) | 92C4-P0 #1; 8BEC Rumpf @0x80109000 (ofs {0,-500,0}) + Nachbluten P1 2x je geradem +0x15A-Stand @0x801090E8/@0x80109120; Zerleger-Oberschenkel @:4540; DEATH-Zweig 3 @0x8010841C | s. Zeilen | ROOM→GLOBAL, **0, 0, 8000** | BELEGT (Dekodier-Regel + §1.3) |
| 8096 | (0,0,8096) | 9610-Burst @0x801096E8, ofs {0,-300,0}, Anker Part 0 | Brust | GLOBAL 0,0,8096 | BELEGT |
| 6096 | (0,0,6096) | Haupt-Handler-Treffer @0x8010567C; Burst @0x80109718 ofs {0,-100,0} | Zone (+0x1D2%3) | 0,0,6096 (Treffer-Pfad bleibt per Mandat RE1.5-0x2000, re2z_blood_fx_dir :3980-86) | BELEGT |
| 3000 | (0,0,3000) | 8BEC-Kopf @0x80108E18, Winkel rand*16, Anker **Part 8**, a3=0 | Kopf direkt | 0,0,3000 | BELEGT |
| 2000 | (0,0,2000) | Teil-Landeklecks @:4273 (Anker = fliegendes Teil) | Teil-Weltpos | 0,0,2000 | BELEGT |
| 7000 | (0,0,7000) | Zerleger-Schienbein @:4558 | Schienbein | 0,0,7000 | BELEGT |
| 5000/6000/4000 | (0,0,x) | Knockdown-Arm-Abriss @0x801076A4/C4/E4 (v={0,0,0}/{0,400,0}/{0,800,0}) | R-Unterarm Part 3 | 0,0,x | BELEGT |
| 0x00021F40 | (0,2,8000) | 92C4-P0 #3 | Hals | **0, 2, 8000** = RE1.5-Fontaene | BELEGT (§1.3) |
| 0x08001B58 | (8,0,7000) | 92C4-P0 #2 | Hals, Winkel=Peilung | **ROOM-Id 5, Sub 0, 7000** | BELEGT (§1.2 byte-identisch) |
| 0x08001000 | (8,0,4096) | Knockdown @0x801075FC/@0x80107704 | Unterarm | ROOM-Id 5, 0, 4096 | BELEGT |
| 0x08000000\|((r+4000)*4) | (8,0,16000-17020) | 8BEC-Rumpf-Brocken @0x8010902C-54, Winkel rand*16 | Part 0 | ROOM-Id 5, 0, scale durchreichen | BELEGT |
| 0x09020000\|(r*4+1536) | (9,2,1536-2556) | 9610-Burst-Brocken, (rand&3)+1 Stueck @0x80109780-88 | Part 0 | **ROOM-Id 7, Sub 2**, scale durchreichen | BELEGT (§1.2) |
| 0x05032710/1388/13E8 | (5,3,10000/5000/5096) | Brand-Tinten-Emitter FUN_80106128 @:4368-4381 (Zeile 10 GL Brand / Zeile 16 Flammenwerfer) | Part 0/8/3/6 | RE1.5-**CORE00-Id 8** (FEUER), Sub 3 | KLASSEN-ZUORDNUNG (RE1.5-Id 8 = Feuer, Beleg §1.1; Routine-17-Familie wie die 1090-Feuer-Ids 9/16) — Sichtpruefung bei Abnahme |
| 0x040F1770/0FA0/0BB8/07D0 | (4,15,6000/4000/3000/2000) | Aetz-Emitter FUN_80106310 @:4409-4420 + Zerleger-Saeure @:4587-4592 (Zeile 11 GL Saeure) | Part 0/12/3, Schienbein/Fuss | RE1.5 hat KEIN Aetz-Sheet: Klassen-Fallback **0, 3, x** (Einzel-Klecks); Kern der Optik ist ohnehin die Part-TINTE (FUN_80106310) | KLASSEN-ZUORDNUNG, offen zur Abnahme |
| 0x0A001000 | (10,0,4096) | Fress-Biss Frame 10 @0x80104DE0-F4, Anker Part 8 | Kopf | 0, 0, 4096 (Blut klein) | KLASSEN-ZUORDNUNG (RE1.5-Raeume tragen kein Id-10-Sheet) |
| 0x0600xxxx / 0x040Cxxxx | (6,0,x)/(4,12,x) | Spark Shot FUN_80106510 @:4446-4457 | — | **UNERREICHBAR im Port**: Zeile 14 steht nicht in `re2z_row_from_weapon` (:3604-3609, RE1.5 hat keine Elektrowaffe) — kein Mapping noetig | N/A |

Randbedingungen:
* **Winkel** immer als `param` durchreichen (Slot+0x2E, s. §1).
* **Raum ohne Id 5** (z. B. ROOM1190 traegt nur {7}): `re15_esp_find_id` schlaegt fehl →
  NICHT auf global durchfallen lassen (dort ist 5 nicht belegt → spawn_rows spawnt nichts,
  harmlos), optional Klassen-Fallback (0,0,scale).
* CLUT-Shift `sub>>3` und Stream-Wahl `sub&7` erledigt der Port-Spawner bereits byte-true
  (esp_fx_seed_header re15_esp.c:733-742, Rowblk-Zugriff :768-775).

### 2.2 Bau-Ort fuer Auftrag 1

`re2z_gore_fx` (enemy_ai_re2_zombie.c:3923) wird vom Stand-in zum Dekoder:
Id/Sub/Scale nach obiger Tabelle aufloesen (kleine switch ueber `packed>>24`), Position =
Part-Weltpos (wie heute via `re15_enemy_bone_world_pos`) + dokumentierter Offset, Spawn via
`re15_esp_fx_spawn_rows` statt `spawn_ex`. Die drei 92C4-Sites brauchen zusaetzlich den
HALS-Anker: Aufrufe von `re2z_blood_fx_at(e, 0, …)` (:5147/5152/5155) auf einen neuen
Helfer mit (anker_part=8, ofs_y=+300 bzw. -100) umstellen — jede Zahl traegt ihr @0x aus §2.

---

## 3. AUFTRAG 2 — TORSO ABSCHIESSEN: welcher Pfad, welche Bedingungen, was blockiert

**Der Original-Pfad ist `FUN_80108BEC` = `re2z_death_rip`** (Zerreiss-Tod: Kopf ab +
Bein(e) ab + **RUMPF-STUMPF** `part_mesh[0] = 16`, Geometrie-Woerter von Record 16 → Part 0,
`lw 2760..2772 → sw 8..20` @0x80108FB8-FDC). Es gibt KEINEN HURT-Torso-Zerleger — der
HURT-Zerleger (@0x80105288) nimmt nur Beine.

**Bedingungen im RE2-Original** (DEATH-Tabelle @0x8010CC24, Dump abtrennen §2.1; Dispatch
Zeile=+0x5/Spalte=+0x1D2 @0x801084E0-518):

| Weg | Zeile | Spalte | Gate |
|---|---|---|---|
| **SPAS/Custom-Schrot W13** | 8 (@0x8010CD44) | 1 (Rumpf) | HP<0 (Applier `sw 3,4` @0x8004728C) |
| GL Explosiv | 9 (@0x8010CD68) | 0/1/2 | dito |
| Rakete | 17 (@0x8010CE88) | 1 | dito |

Und IN der Zelle (P0, frisch verifiziert am Port-Zwilling :5270-5283 mit @0x):
`(rand&3)!=0 && 0x800CFBF8>=0 && Zeile∉{9,17}` → Ausstieg in den BURST (nur 1/4 zerreisst).
**`0x800CFBF8 < 0` = Spieler zielt HOCH (kopf-wegschiessen §3) ueberspringt den Ausstieg →
SPAS + hoch zielen zerreisst DETERMINISTISCH.** Zeile 9 wuerfelt zusaetzlich
`(r1>>(r2&7))&3 != 0` → Burst (@0x80108CF4-D40).

**Erreichbarkeit im Port** (re2z_row_from_weapon :3604-3609): RE1.5-W13 (SPAS-12) → Zeile 8,
W9 Handgranate/W15 GL-Explosiv → Zeile 9, W18 Rakete → Zeile 17. Dispatch ist verdrahtet
(re2z_death :6955-6969, `RE2ZD_8BEC → re2z_death_rip`). Die Pump-Schrotflinte W8 (Zeile 7)
zerreisst NIE — das ist RE2-authentisch (Zeile 7 col1 = 66FC).

**Was im Port blockiert:**
1. **Kein Rumpf-Stumpf sichtbar** — `part_mesh[0]=16` wird von main.c:8391
   (`gore_mesh[nbi] < mesh_count`, RE1.5-MD1 hat 15) weggeklemmt → Torso bleibt INTAKT,
   nur Kopf/Beine fliegen. Blocker 2, Bauplan §4.
2. **FX generisch** — die Brocken-Salve (8,0,16000+) und die Nachblut-Fontaenen laufen als
   Einzel-Sprite (§2.1 fixt das).
3. Optional-Divergenz: der Zielen-HOCH-Uebersprung des Burst-Ausstiegs (@0x80108C94-A0) ist
   im Port OPEN (:5274-76 dokumentiert) — mit `re15_player_aim_elevation() > 0` verdrahten
   (derselbe belegte Produzent wie im Ragdoll :5636), dann ist „SPAS + hoch = sicher
   zerreissen" spielbar statt 25 %.

Der Nutzer-Move „Torso abschiessen": **SPAS-12 fuehren (W13), Rumpf-/Level-Schuss, Kill** —
1/4 (bzw. mit Fix 3 beim Hoch-Zielen 100 %) → voller Zerreiss mit Rumpf-Stumpf; Rest Burst
(Fleischbrocken-Explosion — nach §2.1 dann auch die sichtbar).

---

## 4. AUFTRAG 3 — STUMPF-GEOMETRIE (Blocker 2) BAUFERTIG

### 4.1 Die Messdaten (frisch, CDEMD0.EMS beider Spiele)

RE2 EM010 (kind 0x10: EMD @EMS+0x2A800 = TOC-Sektor 0x55; TIM @0x1A000 = Sektor 0x34,
gen/re2_ems_toc.inc): MD1 = dir[7], 34 Objekte = 17 Meshes. RE1.5 EM10: 15 Meshes, TIM in
dir[8]. **Beide TIMs formatgleich**: 8bpp, PIX 128 Worte x 256 (= 256x256 px), 2 CLUTs
(x=0 y=480 w=256 h=2), Pages 128/129, CLUT-Ids 0x7800/0x7840.

UV-Belegung der Reserve-Meshes (alle Tri+Quad-UVs gescannt):

| Mesh | Inhalt | Page | u | v | Gegenprobe RE1.5-Atlas |
|---|---|---|---|---|---|
| RE2 15 | Bein-Stumpf (Oberschenkel gekappt: y bis 369 statt 529; 26 Verts, alle ≠ Mesh 9) | **128** | **105-126** | **146-250** | Diese Region nutzt in RE1.5 KEIN Mesh (Page-128-Nutzer: Haende u73-127 v84-127, Kopf u0-118 v3-118) → Inhalt dort UNDEFINIERT |
| RE2 16 | Rumpf-Stumpf (Brust-Klon: identische Counts 41v/24t/27q wie Mesh 0, 20/41 Verts verformt — Schultern/Hals eingerissen, y -852→-782) | 129 | 0-126 | 0-81 | = exakt die BRUST-Region (RE1.5-Mesh 7: Page 129 u1-126 v0-86) — aliast RE1.5-Brusthaut |

**Antwort auf die TIM-Frage: die RE2-TIM-Seite ist noetig.** Mesh 15 zeigt mit der
RE1.5-Palette undefinierte Texel (die Gore-Schnittflaeche liegt in einer RE1.5-seitig
unbelegten Atlas-Region); Mesh 16 wuerde zwar „Haut" zeigen (Brust-Alias), aber die falsche.
Kein UV-Umrechnen — die RE2-TIM in einen ZWEITEN Slot laden und nur fuer Gore-Meshes binden.
(Der Queue-Renderer captured den Slot pro Dreieck: render_pc.c:865-887 `s_textri_slot` —
per-Part-Rebind ist verlustfrei.)

### 4.2 Patch-Plan (Datei:Zeile, vollstaendig)

1. **Bank-Felder** — re15_port/include/re15_enemy.h:66 (vor `tpage`):
   `re15_md1_t md1_gore; uint8_t md1_gore_ok; int pc_tex_slot_gore;` (Gore-Seitenbank:
   RE2-MD1 mit den Reserve-Meshes 15/16 + eigener TIM-Slot; -1/0 = keine).
2. **`re2_hybrid_apply` behaelt das RE2-MD1** — re15_port/engine/src/re2_ems.c:372:
   vor `eb->md1 = *md15;` → `eb->md1_gore = re2_md1; eb->md1_gore_ok = 1;`
   (`re2_md1` ist die lokale Kopie :347; ihre Zeiger aliasen das residente RE2-EMS —
   pc_re2_cdemd haelt es fuer immer, main.c:531-545 — kein Lifetime-Problem).
3. **RE2-TIM in den Gore-Slot** — main.c `pc_enemy_hybrid_re15_models` (:640ff, VOR dem
   RE1.5-TIM-Upload :633-636): RE2-TIM erneut aus dem EMS ziehen
   (`re2_ems_locate(ems, sz, type, RE2_EMS_REC_TIM, …)` + `re15_tim_parse`) und nach
   `RE15_TIM_SLOT_GORE(bank)` hochladen. Slot-Vergabe: render_pc.c:177
   `RE15_TIM_SLOT_MAX 46 → 50`, neue Zeile in der Slot-Karte
   `46..49 = RE2-Gore-TIM je Enemy-Bank (g_enemy 0..3)`; Makro in main.c
   `#define RE15_TIM_SLOT_GORE(b) (46 + (b))`. (11..18 Banken, 19-25/36-44 sind belegt —
   Slot-Karte render_pc.c:176-199.)
4. **Renderer-Weiche** — main.c:8388-8408 (heutige Klammer :8391):
   ```c
   int nmi = nbi; const re15_md1_t *src_md1 = npc_md1; int gore_side = 0;
   if (gore_on) {
       if (gore_mesh[nbi] < (uint8_t)npc_md1->mesh_count) nmi = (int)gore_mesh[nbi];
       else if (hb && hb->md1_gore_ok && gore_mesh[nbi] < hb->md1_gore.mesh_count) {
           src_md1 = &hb->md1_gore; nmi = (int)gore_mesh[nbi]; gore_side = 1;
       }
   }
   if (!gore_side && npc_remap) { …wie bisher… }   /* Gore-Meshes sind RE2-INDIZES:
                                                     * Remap NICHT anwenden */
   if (nmi >= src_md1->mesh_count) continue;
   const re15_md1_mesh_t *nm = &src_md1->meshes[nmi];
   ```
   `hb` existiert im Zeichenblock schon (:7975 `re15_enemy_find(npc->type)`).
   Vor den Tri/Quad-Emits bei `gore_side`: `re15_render_pc_bind_tim_slot(hb->pc_tex_slot_gore)`,
   nach dem Part zurueck auf `av.pc_tex_slot` (Bind ist per-Tri gecaptured, s. 4.1).
5. **RE1.5-Flavor** (Import-Bruecke, abtrennen Schritt 2): dieselben Felder am RE1.5-`eb`
   fuellen — kleiner Lader in main.c (neben pc_enemy_hybrid…): RE2-EMS cachen (pc_re2_cdemd
   laeuft flavor-unabhaengig), `re2_ems_locate(kind, REC_EMD)` → `re15_md1_parse(emd+D[7])`
   NUR fuers MD1 + TIM-Upload. Greift erst, wenn abtrennen Schritt 2 (Todes-Gore durch die
   Bruecke, :6176) gebaut ist.
6. **Pose des Stumpfs**: kein neuer Code — Mesh 16 ersetzt Part 0 (faehrt auf Bone 0),
   Mesh 15 ersetzt den Oberschenkel-Part; die Part→Bone-Uebersetzung existiert
   (re2z_part_to_bone :986-992). ⚠️ Die RE2-Gore-Meshes sind im RE2-BIND-RAUM modelliert —
   im Hybrid posieren RE1.5-Bindlaengen; fuer Part 0 (Wurzel, Kante Huefte↔Brust = (0,0,0),
   re2_ems.h:96-98) ist das exakt, fuer den Oberschenkel-Stumpf betraegt die Abweichung die
   Bindlaengen-Differenz (±2 Einheiten laut re2_ems.h:100-107-Messung) — akzeptiert,
   dokumentieren.
7. **Fixtures** (Memory reai-v2-pin-fixture-verschiebung): (i) Hybrid-Pin — RE2-Flavor MIT
   `re2_hybrid_apply`: nach Zerleger/8BEC wird `gore_mesh 15/16` GEZEICHNET (Quelle
   Gore-Bank), nicht geklemmt — exakt die Luecke, durch die test_re2_zombie_teardeath (misst
   die PURE RE2-Bank) am Nutzer vorbei gruen war (abtrennen §1.2-2). (ii) FX-Pin —
   `re2z_gore_fx(0x08001B58)` landet als (Raum-Id 5, Sub 0, scale 7000)-Spawn
   (re15_esp-Slot-Zaehlung wie probe_10d0_situp_re2). (iii) 92C4-Anker-Pin —
   `re15_re2z_last_fx_pos` liegt am Hals (Kopf-Bone+300), nicht an der Huefte.

---

## 5. AUFTRAG 4 — REIHENFOLGE / AUFWAND / SOFORT-WIRKUNG (Nutzer-Default = RE2-Flavor)

| # | Schritt | Ort | Aufwand | Wirkt im RE2-Flavor sofort? |
|---|---|---|---|---|
| 1 | **FX-Dekoder** (§2): re2z_gore_fx → spawn_rows-Mapping + 92C4-Hals-Anker + Original-Skalen | enemy_ai_re2_zombie.c :1007/:3923/:5147-5155 | ~0.5-1 Tag, reine Logik | **JA** — Kopf-„Explosion" (Fontaene+Brocken+Spray am Hals), Burst-Fleischbrocken, Zerleger-/8BEC-Blut. Groesster sichtbarer Hebel |
| 2 | **Stumpf-Bank** (§4.2 Punkte 1-4): Gore-MD1 + RE2-TIM-Slot + Renderer-Weiche | re15_enemy.h:66, re2_ems.c:372, main.c:640/8388-8408, render_pc.c:177 | ~1 Tag | **JA** — Bein-Stumpf (Zerleger, Pistolen-Weg) und Rumpf-Stumpf (SPAS/Granate/Rakete) |
| 3 | Zielen-HOCH-Gate im 8BEC-P0 (@0x80108C94-A0) verdrahten | enemy_ai_re2_zombie.c:5270-83 | Minuten | JA — „SPAS + hoch = sicher zerreissen" |
| 4 | Fixtures (§4.2 Punkt 7) | tests/unit | ~0.5 Tag | Pflicht vor Release |
| 5 | RE1.5-Flavor: abtrennen Schritt 2 (Todes-Gore-Bruecke :6176/PIN 8) + §4.2 Punkt 5 | enemy_ai_re2_zombie.c, main.c | ~1 Tag | nein (nur RE1.5-Flavor) |
| 6 | Kinder-Kaskade im Flug + FUN_8001CEFC-Anker-Kill (abtrennen Schritt 5; nach Schritt 1 haben FX einen (id,sub,anker)-Schluessel) | :4312ff | ~1 Tag | JA (Teile fliegen zusammenhaengend) — niedrigere Prioritaet |

Abhaengigkeiten: 1, 2, 3 sind unabhaengig voneinander; 6 setzt 1 voraus (Anker-Schluessel);
5 setzt fuer den Stumpf 2 voraus.

## 6. Unklar / Restposten

1. **Brand→Feuer-Zuordnung (5,3)→(CORE00-8,3)** und **Aetz→(0,3)**: Klassen-Zuordnungen mit
   Port-interner Evidenz (RE1.5-Id 8 = Feuer, render_pc.c:198), aber ohne Byte-Identitaet —
   bei Abnahme sichtpruefen (re15-port-visual-verify), sonst Sub/Scale nachjustieren
   NUR mit neuem Beleg.
2. rec+0x2C-Produzent (wer schreibt die Part-Position in den Record): Konsumenten-Beweis
   reicht fuer den Bau (§2, zwei unabhaengige Leser); Produzent vermutlich der
   Record-Zeichner FUN_80027434/FUN_80027ff0 — nicht zu Ende RE'd, nicht tragend.
3. RE2-Raum-Id 10 (Beiss-FX) und die RE2-Row-Programme generell: der Port spielt per Mandat
   die RE1.5-Row-Programme (Praesentation aus RE1.5); wer die RE2-Rows byte-true will,
   muesste die RE2-Routinen-Tabelle (Pendant zu RE1.5 @0x80071d40) separat RE'en — bewusst
   NICHT Teil dieses Ausbaus.
4. Stumpf-Marker-Konsument (+0x98/+0x9C der 0x80-Parts) weiterhin OFFEN (abtrennen §Unklar 3)
   — aendert nichts an Meshtausch/Flug/FX.
