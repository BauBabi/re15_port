# Runde 36 Spur E — gegnerzahl (20 Gegner-Plaetze je Raum wie das Original)

Baum `.claude/worktrees/r36_gegnerzahl`, Zweig `r36/gegnerzahl`, Basis master 78a4a748.
Fortlaufend geschrieben; nach jedem Schritt committet.

## 0. Auftrag (AUFTRAG.md Z.15-20, VERTRAG §0 Spur E, woertlich)

> Zu den grenzen: [...] "Der Port verwaltet höchstens 15 Gegner je Raum, das Original 20. In ROOM1040 und 1030 fehlen
> deshalb die letzten fünf Zombies, das war auch vorher schon so." --> Das auch!

VERTRAG Spur E: "Grenze 2: Port fuehrt 15 Gegner je Raum, Original 20 (FUN_80037250 @0x800372b4 `sltiu 0x14`;
Sce_em_set @0x800420dc ohne Grenzpruefung): RE15_ACTOR_MAX anheben, alle Nutzer zensieren, ROOM1040/1030 Records
15..19 erscheinen; Voll-Suite" (Voll-Suite faehrt laut Arbeitsanweisung der Orchestrator; hier gezielte Laeufe).
Eigentuemer laut VERTRAG §1.4: "E = RE15_ACTOR_MAX + alle Arrays".

## 1. Arbeitsprotokoll (chronologisch)
- [start] Baum sauber auf 78a4a748; AUFTRAG/VERTRAG gelesen; L_cut1150.md §10.1/§10.5 (Vorrunde) gelesen.
- RDT-Zensus der Sce_em_set-Slots (§3.4), Original selbst disassembliert (§3.1-3.3), Zensus der Port-Nutzer (§4).
- Basis gebaut, Messung vorher an der exe (§2) -> Befund reproduziert (0 statt 5).
- RE15_ACTOR_MAX 21 + Sentinel irons_tod_1150.c, gebaut, Messung nachher (§6.1) -> 5 (Aktor 16..20).
- 484 Unit-Tests gruen (§7); Riegel unit_r36_gegnerzahl_* + integration_r36_gegnerzahl (Karte + CONTINUE + Tuer) gebaut;
  Sentinel-Pin mit dem alten Wert 0x0f gegengeprueft (rot), dann wieder auf den Fix.
- r35-Tests mit festen Annahmen angepasst (cut1150 Lauf F, granate Gator-Slot), gezielte Integrationslaeufe (§7).

## 2. Messung vorher

Gebauter Stand = Basis 78a4a748 (RE15_ACTOR_MAX 16), exe-Kopie `re15_pc_r36e_mess.exe` neben der exe, eigener
Arbeitsordner je Lauf (Werkzeug scratchpad `E36/lauf.sh`): `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=<raum>@250
RE15_EXIT_AT=200#<raum>`, Tot-Bits per `RE15_SET_FLAG` (Zone 7 = re15_em_status_zone Stage 1-3). Gezaehlt: aktive
Gegner in der letzten state.log-Zeile (Bild 200 im Raum).

| Lauf | Tot-Bits | Records im Skript (spawn-diag) | aktive Gegner Bild 200 (vorher) |
|---|---|---|---|
| v1030_alle | keine | 20 (Slots 0..19) | 6: Aktor 1..6 (Limit 6 @0x1de2) |
| v1030_tot15 | 7:0..7:14 (Records 0..14 tot) | 20 | **0** |
| v1040_alle | keine | 20 (Else-Zweig @0x1390..) | 5: Aktor 1..5 (Limit 5 @0x11f0) |
| v1040_tot15 | 7:20..7:34 (Records 0..14 tot) | 20 | **0** |

Befund reproduziert: sind die Records 0..14 erledigt, ist der Raum im Port leer; die Records 15..19 (Slots 15..19 =
Aktor 16..20) passieren Tot-Bit-Gate und Limit, werden dann von `op_sce_em_set` (`actor_slot < RE15_ACTOR_MAX`) verworfen.
Im Original stehen dort fuenf (§3: Feld mit 20 Plaetzen, Limit 6/5 nicht erreicht).
Keine Regression (seit jeher so, AUFTRAG: "das war auch vorher schon so") -> kein Bisect.

## 3. RE-Belege (Adressen, Bytes)

Alle Zeilen selbst disassembliert (`re15_disasm.py dis`, PSX.EXE des Auslieferungsstands `info/Re1.5/PSX.EXE`).

### 3.1 Sce_em_set (Opcode 0x44, Handler 0x800420a0) legt den Slot OHNE Grenzpruefung ab
```
800420ac lui   a2,0x800b
800420b0 addiu a2,a2,-14468      a2 = 0x800ac77c
800420dc addiu a0,a2,1200        a0 = 0x800acc2c   (Gegner-Feld)
800420f0 lbu   v0,1(a1)          pc[1]
800420f8 andi  s4,v0,0x7f        Slot = pc[1] & 0x7f   (0..127, KEIN Vergleich gegen 20)
800420fc andi  s1,v0,0x80
80042100 sll v0,s4,5 / 80042104 subu / 80042108 sll 2 / 8004210c addu / 80042110 sll 2   -> s4*500
80042118 addu  s0,v0,a0          Entity = 0x800acc2c + Slot*0x1F4
```
Danach Tot-Bit-Gate (`80042120 lbu a1,7(a1)` / `80042128 beq a1,0xff` / `80042130 jal 0x8004efe4` / `80042138 beq v0,zero`)
und Gleichzeitig-Limit (`800421d8/dc` a1 = 0x800b0ff2 / `80042214 lh v0,0(a1)` / `8004221c lh v1,4084(v1)` = 0x800b0ff4 /
`80042224 slt` / `80042228 bne` / voll: `80042230 ori v0,zero,0x8000` + `80042238 sw v0,0(s0)` / frei: `8004223c sh v0,0(a1)`).
Erfolgreicher Spawn zaehlt den Aktiv-Zaehler hoch: `8004254c lbu v0,-13746(v0)` (0x800aca4e) / `80042558 addiu v0,v0,1` /
`80042560 sb v0,-13746(at)`.

### 3.2 Das Gegner-Feld hat 20 Plaetze (drei unabhaengige Belege)
1. **Ziel-Umschaltung FUN_80037250** (naechster Gegner fuer die Zielerfassung) laeuft das Feld mit Umbruch bei 20 ab:
   `800372ac addiu a2,a2,1` / `800372b0 andi v0,a2,0xff` / `800372b4 sltiu v0,v0,0x14` / `800372b8 bne v0,zero,...` /
   `800372bc addiu a1,a1,500` / Umbruch `800372c0 addu a2,zero,zero` + `800372c4 lui a1,0x800b` + `800372c8 addiu a1,a1,-13268`
   (= 0x800acc2c). Decompilat RE_15_Quellcode_V2/FUN_80037250.c: `if (0x13 < (uVar3 & 0xff)) { uVar3 = 0; puVar2 = &DAT_800acc2c; }`.
2. **Das naechste Feld beginnt genau bei 0x800acc2c + 20*0x1F4 = 0x800af33c**: FUN_80039b2c (NPC-Licht-Init, Aufruf aus
   Sce_em_set @0x80042144) rechnet `80039b34 lui v1,0x800b` / `80039b38 addiu v1,v1,0x11cc` / `80039b40 addiu v1,v1,-0x1e90`
   = 0x800af33c, Stride 0x6C (`80039b58..80039b68`), Zaehler DAT_800afbb7; FUN_80039ca0 (NPC-Render+Licht) liest dieselbe
   Basis (`80039cb4 lui s3,0x800b` / `80039cb8 addiu s3,s3,-0xcc4` = 0x800af33c; ghidra1_V2.txt:138186 `lw v0,0x0(s3)=>DAT_800af33c`).
   Ein Slot 20 laege also schon IN diesem Feld.
3. Der Opcode 0x53 Art 2 (LAB_80040e18) nimmt dieselbe Basis (`80040e60 addiu v1,v1,4048` / `80040ec0 addiu v1,v1,-17316` =
   0x800acc2c, idx*500 @0x80040eb0..80040ebc); ROOM1030 sub00 @0x020CE..@0x0216E benutzt ihn in einer For-20-Schleife
   (Spur L r35, L_cut1150.md §10.1).

### 3.3 Die Pro-Bild-Schleife FUN_8001a50c ist NICHT an 20 gebunden, sondern an den Aktiv-Zaehler
```
8001a524 lui s1,0x800b / 8001a528 lbu s1,-13746(s1)   s1 = 0x800aca4e (Spawn-Zaehler, u8)
8001a52c addiu v0,v1,1192 / 8001a540 sw v0,0(v1)      Zeiger = 0x800acc2c
8001a55c lw v0,0(v1) / 8001a564 andi v0,v0,0x1 / 8001a568 beq v0,zero,...   inaktiv -> naechster Platz, NICHT gezaehlt
8001a570 lbu v0,8(v1) / ... / 8001a588 jalr v0 (0x80072bac[Typ]) / 8001a58c addiu s0,s0,1 (Delay-Slot: nur aktive zaehlen)
8001a5a8 addiu v0,v0,500 / 8001a5b0 andi v0,s0,0xff / 8001a5b4 sltu v0,v0,s1 / 8001a5b8 bne ...
```
Die Schleife tickt so viele AKTIVE Plaetze, wie Spawns gezaehlt sind — bei hoechstens 20 Records also genau das Feld.

### 3.4 RDT-Zensus der Sce_em_set-Slots (Werkzeug `re15_port/tools/r36_e/sce_em_slot_zensus.py`, Walker = aot_sce_census.py)
`files=240 Sce_em_set-Records=844 walk-stops=0`; Slot -> Records: 0:245 1:178 2:101 3:77 4:48 5:23 6:23 7:14 8:12 9:12 10:9
11..19: je 8, 20..34: je 2; max Slot 34.
| Raum | Records | Slots | Slot >= 15 (bisher verworfen) | Limit Save(0x12,n) |
|---|---|---|---|---|
| ROOM1030 / 1031 | 20 / 20 | 0..19 | 15..19 (Typ 0x16, Tot-Bits 0x0F..0x13) @0x1f12..@0x1f62 / @0x1f88..@0x1fd8 | 6 @0x1de2 / @0x1e58 |
| ROOM1040 / 1041 | 40 / 40 (zwei Zweige je 20) | 0..19 | 15..19 je Zweig (Typ 0x16, Tot-Bits 0x23..0x27) | 5 @0x11f0 / @0x1218 |
| ROOM20B0 / 20B1 | 35 / 35 (sub00, lueckenlos) | 0..34 | 15..34 (Typ 0x24 FX-Emitter, Tot-Bit 0xFF) | keins |
Dazu Work_set (0x2E) Art 2 auf die Gegner 15..19: ROOM1030 sub04 @0x2416..@0x2496 und sub06 @0x26b8..@0x2738, ROOM1031 sub04/06,
ROOM1040 sub09 @0x1d4e..@0x1e16 und sub10 @0x20e8..@0x2198, ROOM1041 sub09/10 — im Port bisher ins Leere (Aktor >= 16).
Kein anderer Raum benutzt Slot >= 15.

**ROOM20B0/20B1 (Slots 20..34):** im Original laegen diese Entities hinter dem 20er-Feld (0x800af33c..0x800b1087: NPC-Feld
0x800af33c, ..., SCD-Variablen 0x800b0fd0, Limit 0x800b0ff2/0x800b0ff4, Tot-Bit-Zonen 0x800b1038/0x800b1058) — ein
Ueberlauf des Prototyps, kein 21. Platz. Der Port verwirft sie weiter (Aktor > 20); dokumentiert unter OFFEN.

## 4. Zensus der Nutzer von RE15_ACTOR_MAX / festen 16/15 / Slot-Bitmasken

`grep -rn RE15_ACTOR_MAX re15_port`: 1012 Stellen in 268 Dateien (engine/platform/include ~250 in 30 Dateien, der Rest
Sonden/Tests). Geprueft nach Klassen (grep-Muster je Klasse in Klammern):

| Klasse | Befund | Folge |
|---|---|---|
| Felder `[RE15_ACTOR_MAX]` (43 in engine/platform: affen_11c0 s_pool, enemy_ai_common s_zfoot_*/s_los_*/s_wander_*/s_gait_variant/s_zcrawl_*/s_dogpaw_*/s_maggot_pose_bild/s_prev_clip/s_writher_*, re2_crow/dog/zellenarm, boss_gator s_gb, enemy_common s_rbj_alias, granate_r35, irons_tod s_b1040_*, re15_damage s_re2_probe/collected, re15_g5_skin s_bone_add) | wachsen mit | keine |
| Schleifen/Grenzen `< RE15_ACTOR_MAX`, `RE15_ACTOR_MAX - 1` | symbolisch | laufen ueber 20 Gegner |
| feste 16/15 statt der Konstante (`\[16\]`, `< 16`, `<= 15`, `0x0f`, Text "Aktor 16"/"Slot 15") | **1 Fehler**: irons_tod_1150.c slots_zuweisen `s_prog[o + 1] = 0x0f` = "kein Platz: Slot 15 -> Aktor 16 = verworfen" — mit 21 Plaetzen ist Aktor 16 GUELTIG, der Port-Spawn haette einen belegten Platz ueberschrieben | behoben (Slot RE15_ACTOR_MAX-1), Pin `kein_platz` (alter Wert gemessen rot: 19/20 Plaetze heil, Aktor 16 ueberschrieben) |
| | alle anderen `[16]`-Felder sind Bone-/Part-/Clip-/HP-Tabellen (re2z_part_* je Teil, HP-Tabellen `rng&0xf`, s_registry der ESP-Bank, nav A/B/C je Tiefe, s_prop_push_count je Objekt) | keine |
| "hoechster freier Platz" | main.c ROOM2090: Alligator-Port-Spawn sucht rueckwaerts ab RE15_ACTOR_MAX-1 -> Slot 15 wird Slot 20 (gemessen r36e n2090: "Alligator (0x23) in Wasserzone gesetzt, Slot 20") | kein Code-Fehler; integration_r35_granate Lauf gator auf Slot 20 angepasst |
| Bitmasken `(1u << slot)` | enemy_common.c rbj_record_marker `& (1u << mslot)` (u32), main.c s_diag_seen (u32, ai < 32) — keine uint16-Maske | keine |
| Slot in Halbbytes / `& 0xf` | keine | keine |
| Slot-Felder int8_t/uint8_t (work_slot, contact_slot, ...) | bis 127 | keine |
| Render | Texturen/Modelle je TYP (g_enemy[RE15_ENEMY_MAX=4]), nicht je Aktor; NPC-Zeichenschleife main.c:9505 symbolisch | keine |
| Spielstand | re15_savedata_t traegt KEINEN Aktor-Pool (Spieler-Felder + Flags + Inventar/Kiste/Karte), Tot-Bits je Record in Zone 7/8 schon immer gespeichert; Pin test_map_speichern_laden "sizeof 944" gruen; alte Nutzerkarte `tests/unit/probes/r30_karte_nutzer_2026-09-27.mcr` mit der neuen exe: `[save] CONTINUE: resumed in room 1150 (hp=100)` | kompatibel |
| PSX | platform/psx/main.c nutzt nur den Spieler-Platz; Speicher g_actors 16*1884 = 30144 -> 21*1884 = 39564 Bytes (PC-Groesse, +9420) | PSX-Ziel hier nicht baubar (PSn00bSDK fehlt, Memory reai-v2-psx-build-gap) |
| Tests | 484 Unit-Tests gruen ohne Aenderung; Sonden mit `int x[16]` sind per `na < 16` begrenzt; integration_r35_cut1150 Lauf F (Erwartung "0 erscheinen, Auffuellung 5") und integration_r35_granate (Gator Slot 15) angepasst; test_r35_cut1150.c "Nachspawn 1030 +7 oder 15" bleibt (13 < 15) | s. §7 |
| Kommentare | irons_tod_1150.c Kopf "(RE15_ACTOR_MAX = 16)" berichtigt; re15_irons_tod.h Z.172 / L-Dossier beschreiben den Stand von Runde 35 (Historie, unveraendert) | — |

Laufzeit: die GLEICHZEITIGE Zahl in ROOM1030/1040 bleibt durch das Original-Limit (Save(0x12,6) @0x1de2 / Save(0x12,5)
@0x11f0) bei 6/5 (§6.1: 6/5 vorher und nachher), es aendert sich nur, WELCHE Records stehen. Einziger Raum mit mehr
gleichzeitigen Aktoren: ROOM20B0/20B1 (FX-Emitter 0x24, Drift ohne Schaden) 15 -> 20 (§6.2).

## 5. Umsetzung (Dateien, Konstanten)

| Datei | Aenderung | Beleg |
|---|---|---|
| re15_port/include/re15_actor.h | `RE15_ACTOR_MAX 16 -> 21` (Spieler + 20) mit Belegkommentar | §3.1/§3.2: @0x800372b4 `sltiu v0,v0,0x14`, Feld-Ende 0x800af33c @0x80039b38/@0x80039b40, Sce_em_set @0x800420f8/@0x80042118 |
| re15_port/engine/src/irons_tod_1150.c | slots_zuweisen: "kein Platz" = Slot `RE15_ACTOR_MAX - 1` (Aktor 21, verworfen) statt fest `0x0f` (Aktor 16, jetzt gueltig); Kopfkommentar "(RE15_ACTOR_MAX = 21 ...)" | §4 Zensus; Pin kein_platz |
| re15_port/tests/integration/test_r35_cut1150.cmake | Lauf F: "5 erscheinen im Port, Auffuellung 0" / "Gleichzeitig-Zaehler 5 -> 5" (vorher "0 erscheinen ... Auffuellung 5" / "5 -> 0") | Records 15..19 erscheinen selbst (§6.1) |
| re15_port/tests/integration/test_r35_granate.cmake | Lauf gator: Boss-Slot 15 -> 20 | gemessen r36e n2090 (§4) |
| re15_port/tests/unit/test_r36_gegnerzahl.c, probe_r36_gegnerzahl_karte.c, probes/r36_gegnerzahl.cmake, tests/integration/test_r36_gegnerzahl.cmake | NEU: Riegel + Karte + Integration | §7 |
| re15_port/tools/r36_e/sce_em_slot_zensus.py | NEU: RDT-Zensus der Sce_em_set-Slots | §3.4 |

Keine neue Engine-Datei noetig (eine Konstante + ein Sentinel); keine Assets, keine Sprachdateien, keine Bank-9-Bits,
keine Nachrichten-IDs, keine Ereignisnummern.

## 6. Messung nachher

### 6.1 exe, gleiche Laeufe wie §2 (Stand mit RE15_ACTOR_MAX 21)
| Lauf | aktive Gegner Bild 200 (nachher) |
|---|---|
| n1030_alle | 6: Aktor 1..6 (unveraendert) |
| n1030_tot15 | **5: Aktor 16..20 = Records 15..19** (Typ 0x16, Zustand 1) |
| n1040_alle | 5: Aktor 1..5 (unveraendert) |
| n1040_tot15 | **5: Aktor 16..20 = Records 15..19**, hinter dem Tor (x -29052..-29108, z 12693..18515) |

### 6.2 Montage ROOM1040 (Spur L, re15_irons_tod_sichtbar_1040) ueber den ECHTEN Weg (integration_r35_cut1150, gruen 509,91 s)
Lauf F (Karte `nach10f0;ersteszene;tot1040_15` = Records 0..14 tot, CONTINUE in 1130, Tuer -> 1150 -> Kette), debug.log:
`[irons-tod] 1040: 5 Raum-Records leben, 5 erscheinen im Port, Auffuellung 0` / `1040: Gleichzeitig-Zaehler 5 -> 5` /
`Bilanz 1040: 5 Zombies, durchs Tor (z < -360) 5, letzter bei Bild 273 von 363; 16@220 17@202 18@273 19@209 20@244;
Spieler-HP 100 -> min 100; Kappe 0`. Vorher (Runde 35, gleicher Lauf): `0 erscheinen im Port, Auffuellung 5`,
`Zaehler 5 -> 0`, die fuenf waren Port-Auffuell-Records. Jetzt kommen die ECHTEN Records 15..19 (Aktor 16..20) durchs
Tor, mit denselben Durchgangsbildern wie Lauf A (alle leben: `1@220 2@202 3@273 4@209 5@244`, `20 Raum-Records leben,
5 erscheinen im Port, Auffuellung 0`). Die Auffuellung rechnet weiter ueber ERSCHEINENDE Records und ergibt 5/5
(Riegel unit_r35_cut1150_montage_1040_reihe k = 0..20 gruen).

### 6.3 Raeume wie im Spiel
ROOM1030: die fuenf stehen im Bild 1 auf den Plaetzen der Records 0..4 (Aktor 16 @(-8978,-27878) = Record 0 @0x1de6) —
das ist die raumeigene Aufstellung (sub00 For-20-Schleife mit Opcode 0x53 Art 2 ueber die 20 Plaetze, @0x020CE..@0x0216E,
s. §3.2 Punkt 3), dieselbe, die bisher die Aktoren 1..6 setzte; danach gehen sie auf den Spieler zu (Aktor 16: d 5369 ->
3347 in 160 Bildern).

ROOM2090 (Port-Alligator, main.c "hoechster freier Platz"): `Alligator (0x23) in Wasserzone gesetzt, Slot 20`;
integration_r35_granate Lauf gator: `RE15_FORCE_EXPLOSION F60 art=2 slot=20 t=23 ... Treffer=1`, HP 3000 -> 2000.

### 6.4 Folgebefund aus dem Zensus: die SCHATTENLISTE hing an der alten Grenze (gemessen, behoben)
render_pc.c `#define SHADOW_QUAD_MAX 16` = Spieler + 15. Ein Schatten je Aktor und Bild (Spieler-Block main.c "RE1.5
CHARACTER SHADOW" + NPC-Schleife main.c:9505ff., im Original FUN_8001b064 je Entity, ohne eigene Obergrenze; Katalog
RE15_FUN_CATALOG.md FUN_8001b064 "per entity"). Messhaken `RE15_SCHATTEN_LOG=1` (neu, eine Zeile je Aenderung),
ROOM20B0 per Debug-Sprung, Bild 0..300 (scratchpad E36/runs/s20b0_*):
| Kapazitaet | Zeilen |
|---|---|
| 16 (Zwischenstand: 21 Aktoren, alte Liste) | `gezeichnet 16, verworfen 5` (3x), `gezeichnet 16, verworfen 1` |
| RE15_ACTOR_MAX = 21 (Fix) | `gezeichnet 21, verworfen 0` (3x), `gezeichnet 17, verworfen 0` |
Ohne den Fix haetten fuenf Emitter in ROOM20B0 keinen Schatten gehabt (die letzten nach Slot). In ROOM1030/1040 bleibt
die gleichzeitige Zahl beim Limit (6/5, Montage hoechstens 6 + 7 = 13 Zombies + Spieler), dort griff die Grenze nicht.
Weitere Render-Kapazitaeten geprueft: Dreieckswarteschlange TEXTRI_QUEUE_MAX 8192 (je Dreieck, nicht je Aktor; die
hoechste gleichzeitige Zombiezahl je Raum ist unveraendert, s. §4 Laufzeit), TIM-Slots/Modellbaenke je TYP (4), SPU
24 Stimmen (= PSX-Hardware) — keine weitere Kopplung an 16.

### 6.5 ROOM20B0/20B1: 15 -> 20 FX-Emitter, Slots 20..34 bleiben verworfen
unit_r36_gegnerzahl_raum20b0 und integration_r36_gegnerzahl Lauf e: Aktor 1..20 vom Typ 0x24. Original: 35 Records
(sub00 @0x1aae..@0x1d56, Slot 0..34, Tot-Bit 0xFF, kein Limit). Die Slots 20..34 liegen hinter dem 20er-Feld: Entity =
0x800acc2c + Slot*0x1F4 (§3.1) = 0x800af33c (NPC-Feld, FUN_80039b2c/FUN_80039ca0) bis 0x800b1087 — darin die SCD-Variablen
0x800b0fd0.. (Save/Calc, Limit 0x800b0ff2/0x800b0ff4) und die Tot-Bit-Zonen 7/8 (0x800b1038/0x800b1058). Sce_em_set
schreibt fuer Slot 34 u.a. `sh v0,442(s0)` @0x80042210 -> 0x800b104e (Zone 7) und `sh zero,452(s0)` @0x8004216c /
`sb v0,454(s0)` @0x80042170 -> 0x800b1058/0x800b105a (Zone 8) — alles VOR dem Limit-Gate @0x80042214 (ohne Save(0x12) gilt
die Vorgabe 0xFF), und der Aktiv-Zaehler 0x800aca4e steigt auf 35 (@0x8004254c-60), den die Pro-Bild-Schleife als Grenze
nimmt (@0x8001a528/@0x8001a5b4, sie sucht ueber das Feld hinaus weiter). Ob das Original dabei sichtbar 35 Emitter zeigt,
ist dynamisch nicht gemessen (OFFEN §8). Das ist ein Ueberlauf des Prototyps in fremde Felder, kein 21. Platz — der Port
bildet ihn nicht nach (PORT-ENTSCHEIDUNG, Beleg = Feldende §3.2) und verwirft die Slots 20..34 wie bisher die 15..34.

## 7. Tests (Namen, Ergebnis)

| Test | misst | Ergebnis |
|---|---|---|
| unit_r36_gegnerzahl_feld | RE15_ACTOR_MAX = 21, g_actors 21 Plaetze (+ `_Static_assert`) | gruen |
| unit_r36_gegnerzahl_raum1030 / _raum1031 | echte RDT, Raumaufbau ueber scd_room_reenter; je k = 0..20 (Records 0..k-1 tot) erscheinen GENAU Aktor k+1..k+n, n = min(6, 20-k) (Tot-Bit vor Limit @0x80042120-3c, Limit 6 @0x1de2/@0x1e58); 1030: die sub00-Aufstellung (0x53 Art 2 ueber 20 Plaetze) setzt den i-ten anwesenden auf Pos_set-Fall i (auch Aktor 16..20); 1031 mit seiner Vorbedingung Ck(3,0x7d) @0x01E54 | gruen (je 21 Faelle + 20 Aufstellungen) |
| unit_r36_gegnerzahl_raum1040 / _raum1041 | dito, Limit 5 (@0x11f0/@0x1218), beide Zweige (Ck(4,5) @0x011F8) | gruen (je 42 Faelle) |
| unit_r36_gegnerzahl_raum20b0 | ROOM20B0/20B1: Slots 0..19 = Aktor 1..20 (Typ 0x24), Slots 20..34 verworfen, keine fremden Aktoren | gruen |
| unit_r36_gegnerzahl_kein_platz | alle 20 Plaetze belegt + Nachspawn-Programm 14 (irons_tod_1150.c): 5/5 Port-Spawns tragen Slot 20 + Tot-Bit 0xFF, nach 10 Bildern alle 20 Plaetze unveraendert | gruen; GEGENPROBE mit dem alten Wert 0x0f: rot (0/5, nur 19/20 heil = Aktor 16 ueberschrieben) |
| integration_r36_gegnerzahl | ECHTER Weg: Karte (probe_r36_gegnerzahl_karte) + CONTINUE + Aktionstaste an der Original-Tuer; letzte state.log-Zeile im Zielraum: a 1030->1040 (1040 Records 0..14 tot) {16..20}; b 1040->1030 (1030 Records 0..14 tot) {16..20}; c Kontrolle 1040 alle leben {1..5}; d 1030 Records 0..16 tot {18,19,20}; e ROOM20B0 (Debug-Sprung, beschleunigter Renderer, RE15_SCHATTEN_LOG): Aktor 1..20 Typ 0x24, ein Bild mit `gezeichnet 21, verworfen 0`, nie `verworfen >= 1` | gruen, 100,1 s (a-d allein: 69,5 s) |
| integration_r35_cut1150 (angepasst) | Kette ueber den echten Weg, Lauf F jetzt "5 erscheinen im Port, Auffuellung 0", "Zaehler 5 -> 5" | gruen, 509,91 s (§6.2) |
| integration_r35_granate (angepasst) | Lauf gator auf Slot 20 | gruen, 172,53 s |
| gezielter Lauf (Montage-/Spawn-Raeume + geaenderte Tests) | `ctest -R "^(integration_r35_cut1150\|integration_r35_granate\|integration_r36_gegnerzahl\|integration_room_probe_sweep\|integration_room_transition\|integration_headless_run\|integration_cross_stage\|integration_r34n_g_schrift1150\|prop_spawn_entered\|probe_r16_tuer1040_[ABFN])$" --timeout 600` | `100% tests passed, 0 tests failed out of 13` (816,47 s) |
| alle 484 Unit-Tests (`ctest -R "^unit_" -E "android_bash" --timeout 600 -j4`) | Gesamtbestand mit RE15_ACTOR_MAX 21 | `100% tests passed, 0 tests failed out of 484` (183,8 s), darunter unit_r35_cut1150_montage_1040_reihe (k = 0..20 je 5 Zombies) |

## 8. OFFEN

* **Original dynamisch nicht gemessen** (kein Savestate des Auslieferungsstands in ROOM1030/1040/20B0 vorhanden; die
  sechs PATCHED-EXE-Staende scheiden aus). Der Mechanismus ist statisch vollstaendig belegt (§3: Feld 20, Sce_em_set
  ohne Grenze, Tot-Bit vor Limit). Messweg, falls gewuenscht: DuckStation-GDB (Muster analysis/befunde_runde35/
  H_raeume.md "RE2-ORIGINAL GEMESSEN": `[Debug] EnableGDBServer = true`, 127.0.0.1:2345, danach zurueck) — Zone-7-Bits
  0x14..0x22 bei 0x800b1038 setzen, per Debug-Menue ROOM1040 laden, Entities 0x800acc2c + s*0x1F4 (s = 15..19) lesen:
  Wort +0x00 Bit 0 und Typbyte +0x08 = 0x16 erwartet. Fuer ROOM20B0 dieselbe Lesung fuer s = 20..34 plus
  0x800b1038..0x800b1077 vor/nach dem Laden (Ueberlauf in die Tot-Bit-Zonen, §6.5).
* **CONTINUE in ROOM20B0 mit einer Werkzeug-Karte** (Kamera aus der Karte = Cut 0): 300 Bilder lang `[schatten]
  gezeichnet 0`, die Kamera blieb in Cut 0, waehrend die Raumszene den Spieler von x 2380 nach -15920 fuehrte
  (Lauf lauf_*_e in build/r36_gegnerzahl_wd, verworfen zugunsten des Sprungs). Ursache vermutlich der synthetische
  Kartenstand (camera_cut 0 statt des Schnitts am Standplatz) — nicht Teil dieser Spur, nicht weiter verfolgt.
* PSX-Ziel: Speicher von g_actors +5 x sizeof(re15_actor_t) (PC 1884 B -> +9420 B); das PSX-Ziel ist in diesem Baum
  nicht baubar (PSn00bSDK fehlt).

## 9. Fuer den Nutzer

* ROOM1030/1031/1040/1041: hat Leon (oder Elza) die ersten Zombies eines Raums erledigt, kommen jetzt wie im Original
  die naechsten aus der Liste nach — bis zu allen 20 Records je Raum (gleichzeitig weiterhin hoechstens 6 in 1030 und
  5 in 1040, das ist die Original-Grenze im Raumskript). Vorher war ab 15 erledigten Zombies Schluss.
* Die Montage in 1040 (Irons-Szene) nimmt jetzt die echten restlichen Raum-Zombies statt der Port-Ersatzzombies, wenn
  genug davon uebrig sind; das Ergebnis bleibt "5 Zombies durchs Tor".
* ROOM20B0 (Stage 2) zeigt jetzt 20 statt 15 der kleinen gruenen Schwebe-Effekte; das Original fuehrt dort 35 Eintraege,
  von denen 15 ueber sein eigenes Gegner-Feld hinausschreiben — die bleiben weg.
* Alte Spielstaende laden unveraendert (das Speicherformat enthaelt keine Gegnerliste; gemessen mit der Karte
  vom 27.09.).
* Keine neuen Assets, keine Sprachdateien, kein Paket-Gate. Neuer Messhaken: `RE15_SCHATTEN_LOG=1` (Schattenliste je
  Bild ins debug.log). Keine Entscheidungsfragen.
