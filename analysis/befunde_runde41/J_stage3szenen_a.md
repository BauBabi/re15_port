# Runde 41 — Spur J "stage3szenen_a" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_stage3szenen_a`, Zweig `r41/stage3szenen_a`, Basis 4c667c97 (+ ae8ea0a7 AUFTRAG A-E3..A-E5).
Start 2026-10-24 (Uhr siehe Commit-Zeit). Kein Vorgaenger-Stand dieser Spur im Baum (0 eigene Commits beim Start).

## Punkte (Wortlaut, AUFTRAG.md)
1. "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist falsch in ROOM 3060."
2. "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von
   dieser Cutscene hat Elza auch erneut eine falsche Animation."
0. (Orchestrator-Entscheid A-E5, 2026-10-24 04:01) ERSTER Schritt: RBJ-Binder FUN_8001b3f8 byte-true bauen, in allen 12
   abweichenden Raeumen zur Laufzeit messen (welcher Record je Slot, vorher/nachher), eigener Commit mit `rbj-binder` in
   der ersten Zeile. Spur K uebernimmt ihn per cherry-pick unveraendert.

## Plan
- §0 RBJ-Binder: Disasm FUN_8001b3f8 -> Port rbj_resolve_slot / re15_emd_parse_rbj -> Laufzeitmessung 12 Raeume -> Commit `rbj-binder`.
- §1 3060 Marvin: SCD-Disasm ROOM3060 (Marvin-Abschnitt-Ende), Port-Trace (RE15_ANIM_TRACE/RE15_SCD_TRACE) + Framedump, Vergleich.
- §2 3061 Sherry-Schacht + Elza-Ende: SCD-Disasm ROOM3061, Port-Trace mit Elza, Vergleich.

## Laufendes Protokoll
(neueste Eintraege unten je Abschnitt)

## §0 RBJ-Marker-Binder FUN_8001b3f8 (A-E5, erster Schritt)

### §0.1 RE-Beleg (selbst disassembliert: `re15_disasm.py dis 0x8001b3f8 48` aus info/Re1.5/PSX.EXE)
Rohbytes @0x8001b3f8: `0b 80 02 3c 78 c7 42 8c 00 00 00 00 5c 00 46 8c 00 00 00 00 33 00 c0 10 21 38 00 00 ...`
| Adresse | Instruktion | Bedeutung |
|---|---|---|
| 0x8001b3fc / 0x8001b404 | `lw v0,-14472(v0)` (DAT_800ac778) / `lw a2,92(v0)` | RBJ-Zeiger = RDT-Kopf +0x5C |
| 0x8001b40c / 0x8001b410 | `beq a2,zero,0x8001b4dc` / Delay `addu a3,zero,zero` | kein RBJ -> nichts; **a3 = 0 EINMAL vor allen Records** |
| 0x8001b41c / 0x8001b424 / 0x8001b420 | `lw v0,0(a2)` / `addu t0,a2,v0` / `lbu t2,4(a2)` | Trailer = RBJ + u32@0; Record-Zahl = Byte @+4 |
| 0x8001b42c..38 | `lw v0,0(t0)` / `addu v1,a2,v0` / `lw a0,0(v1)` | Marker-u32 am EMR-Praefix des Records |
| 0x8001b440 / 0x8001b444 / 0x8001b448 | `andi v0,a0,0x1` / `beq` / `srl a0,a0,1` | Bit 0 = Spieler |
| 0x8001b450 / 0x8001b460 | `sw v0,0(t3)` = 0x800acbd4 / `sw v0,4(t3)` = 0x800acbd8 | Spieler-Kanal +0x180/+0x184; JEDER Record mit Bit 0 schreibt -> **der letzte gewinnt** |
| 0x8001b464 | `beq a0,zero,0x8001b4c8` | keine Gegner-Bits -> a3 bleibt |
| 0x8001b470..78 | `andi v0,a0,0x1` / `beq v0,zero,0x8001b4c0` / `srl a0,a0,1` | je Bitposition |
| 0x8001b47c..90 | `andi v0,a3,0xff` ... `sll v1,v1,2` | Index a3&0xff * 0x1F4 (Entity-Stride) |
| 0x8001b4a0 / 0x8001b4bc | `sw t1,0(at)` = 0x800acdac+i*0x1F4 / `sw v0,0(at)` = 0x800acdb0+i*0x1F4 | Gegner-Kanal +0x180/+0x184 |
| 0x8001b4c0 / 0x8001b4c4 | `bne a0,zero,0x8001b470` / Delay `addiu a3,a3,1` | **a3++ je Bitposition (auch 0-Bits unter dem hoechsten), NIE je Record zurueckgesetzt** |
| 0x8001b4c8..d8 | `addiu a1,a1,8` / `addiu t2,t2,-1` / `bne` / `addiu t0,t0,8` | naechster Record |
Aufrufer: Raumlader `jal 0x8001b3f8` @0x80039a08 (nach FUN_8003ef6c @0x80039a00). Einzige Schreiber der Kanaele:
ghidra1_V2.txt XREF DAT_800acbd4/acbd8/acdac/acdb0 je genau 1 (W) aus FUN_8001b3f8; kein `sw ..,0x180(..)` auf Entities
sonst im EXE (einziger Treffer 0x8002e3a0 = SPRT-Puffer). Sce_em_set (@0x800420a0, Tabelle @0x800745b8) loescht +0x180 nicht.

Port vor Runde 41: `rbj_resolve_slot` (enemy_common.c) = ERSTER Record mit Marker-Bit (1+i); Spieler = Record 0
(`re15_apply_room_cinematic` -> `re15_emd_parse_rbj`).

### §0.2 Zensus (statisch, eigenes Werkzeug re15_port/tools/r41_j/rbj_binder_zensus.py = Nachbau der Schleife oben)
Ergebnis deckungsgleich mit Spur A (A_belege/nb1d/rbj_a3_zensus.txt): 240 Raeume, 47 mit RBJ, 12 abweichend:
1040/1041 (Marker 0x7e: Spieler orig KEIN Record, Port rec0), 2000/3091/30E0/4000/4001/4010/5011/6030/6031 (Marker
0x1/0x2/0x3: Spieler orig rec2 / Port rec0; Gegner 1 orig rec2 / Port ungebunden), 3060 (0x1/0x2/0x4: Gegner orig
{0:1, 2:2} / Port {0:1, 1:2}).
Inhalt (rbj_zensus.py json, Clip-Hashes): 2000/30E0/4000 rec0 == rec2 (25/25 Clips gleich) -> Spieler unveraendert;
4001/4010/5011 rec2 = rec0 + Zusatzclips (11->14, 25->27, 20->21); **3091/6030/6031 rec0 = 1 Clip mit 2 Bildern
(Platzhalter), rec2 = die echten 14/22/20 Clips** -> Spieler bekommt dort erstmals die Szenen-Gesten (Spur K 3091).
1040/1041: kein Plc_motion im ganzen SCD (scd_dump_room.py) -> Spieler-Kanal ungenutzt.

### §0.3 Umsetzung (Commit e4a9f39c `feat(r41-stage3szenen_a): rbj-binder ...`, fuer Spur K per cherry-pick)
- `re15_port/engine/src/rbj_binder_r41.c` + `include/re15_rbj_binder_r41.h`: `re15_rbj_binder()` = die Schleife von
  FUN_8001b3f8 (Adressen je Zeile im Code), `re15_rbj_binder_spieler/_gegner`, alte Regel `re15_rbj_binder_alt_gegner`
  nur fuer Messzeile/Riegel. PORT-WAHL Schutzgrenzen: <= 8 Records, Gegner-Index < 32 (Original prueft nichts).
- Haken `enemy_common.c`: `rbj_resolve_slot` -> `re15_rbj_binder_gegner(mslot-1)` (Alias Runde 35 L bleibt);
  `re15_apply_room_cinematic` (1) Spieler = `re15_rbj_binder_spieler` statt Record 0; `re15_rbj_bind_room` schreibt die
  Messzeile `[rbj-binder] ROOMxxxx Spieler recN (alt rec0) Gegner i:recN(alt recM)`; je aufgeloestem Aktor
  `[rbj-binder] ROOMxxxx Aktor s Typ 0xTT Marker-Slot m -> recN gebunden`.
- Haken `platform/pc/main.c` (Raumwechsel, 1 Zeile + include): kein Spieler-Record (1040/1041) -> Leon/Elza auf die
  PL00/PL04-Basis wie in einem Raum ohne RBJ (sonst blieb die Bank des Vorraums; Original: Kanal nicht geschrieben,
  1040/1041 haben kein Plc_motion).
- Haken `platform/psx/src/asset_psx.c`: dieselbe Spieler-Regel (Paritaet der Inline-Kopie; PSX hier nicht gebaut).

### §0.4 Laufzeitmessung 12 Raeume (vorher = Hauptbaum-exe 09:57 = master vor dieser Spur, nachher = Baum-exe)
Werkzeuge `re15_port/tools/r41_j/lauf.sh` (eigene exe-Kopie + Laufordner), `rbj_12raeume.sh` (Leon: TITLE_SHOT +
DEBUG_JUMP <raum>@5; Elza: TITLE_CONFIRM_MS 1500 + PSELECT_AUTO(_SWITCH) + DEBUG_JUMP <leonraum>@250), `rbj_vergleich.sh`.
Laeufe `re15_port/build/r41j_laeufe/{vor,nach}_<raum>/debug.log`. Ergebnis (Spieler-Overlay = `[rbj] room .. cinematic
overlay`, Gegner = Messzeile):
| Raum | Spieler vorher | Spieler nachher | Gegner nachher (alt) | zur Laufzeit aufgeloest |
|---|---|---|---|---|
| 1040 | rec0 = 5 Clips/100 kf (Rolltor-Zombie-Record!) | PL00-Basis 24 Clips/712 kf | 0..5: rec0 (rec0) | - |
| 1041 | rec0 = 5 Clips/100 kf | PL04-Basis 24 Clips/705 kf | 0..5: rec0 (rec0) | - |
| 2000 | rec0 25/284 | rec2 25/284 (inhaltsgleich) | 0:rec1 (rec1), 1:rec2 (-1) | - |
| 3060 | rec0 2/50 | rec0 2/50 | 0:rec1 (rec1), 1:-1 (rec2), 2:rec2 (-1) | - |
| 3091 | rec0 **1 Clip/2 kf** | rec2 **14 Clips/329 kf** | 0:rec1 (rec1), 1:rec2 (-1) | - |
| 30E0 | rec0 25/284 | rec2 25/284 | 0:rec1, 1:rec2 (-1) | - |
| 4000 | rec0 25/284 | rec2 25/284 | 0:rec1, 1:rec2 (-1) | - |
| 4001 | rec0 11/264 | rec2 14/329 | 0:rec1, 1:rec2 (-1) | Aktor 1 Typ 0x49 -> rec1, **Aktor 2 Typ 0x4B (Sherry) -> rec2** (vorher ungebunden) |
| 4010 | rec0 25/284 | rec2 27/355 | 0:rec1, 1:rec2 (-1) | Aktor 1 Typ 0x42 -> rec1, **Aktor 2 Typ 0x40 (Marvin) -> rec2** (vorher ungebunden) |
| 5011 | rec0 20/524 | rec2 21/564 | 0:rec1, 1:rec2 (-1) | - |
| 6030 | rec0 **1/2** | rec2 **22/762** | 0:rec1, 1:rec2 (-1) | Aktor 1 Typ 0x4D -> rec1 |
| 6031 | rec0 **1/2** | rec2 **20/536** | 0:rec1, 1:rec2 (-1) | Aktor 1 Typ 0x4B -> rec1 |
"-" = in den 200/450 Bildern nach dem Sprung hat kein Aktor den Sub-0-Kanal angefordert (keine Szene gelaufen).
Gezielte Riegel vorher/nachher: `ctest -R "<Gruppen der RBJ-/Raum-Module>"` 164/164 gruen (Regex im §Tests).

**Folgerung fuer Punkt 1 (A-E5-Hypothese geprueft):** in ROOM3060 aendert der Binder nur Gegner 1/2 (Hunde; kein Hund
spielt Sub 0). Marvin ist dort entweder Gegner 0 (Folge-NPC, `Ck(4,0xC7)` sub00 @0x00F54, rec1 vorher wie nachher) oder
der SPIELER (s. §1). Der Binder ist also NICHT die Ursache von Punkt 1; Ursache siehe §1.

## Fortsetzung 2026-10-24 (Abend; Vorgaenger am Sitzungslimit ~12:23 beendet, Stand 541881b5 UNGEPRUEFT)
Vorliegend und NICHT wiederholt: Original-Savestate-Reihe `o3060_s2_*` (DuckStation, MZD-Disc = identische PSX.EXE),
`o3061_a_*` (nur Raumeintritt 3061, Szene sub02 nicht erreicht), Port-Laeufe `re15_port/build/r41j_laeufe/{m3060_vor,
m3060_nach,e3061_vor,cut3060}` (debug.log, anim.txt = RE15_ANIM_TRACE, Framedumps). Ablage der Original-Staende:
Scratchpad `J41/orig/` (Sitzung c41eae99), Bilder/Auszuege fuer die Abnahme unter `analysis/befunde_runde41/J_belege/`.
Die Behauptungen im Kopf von `re15_wclip_r41.h` sind unten SELBST nachdisassembliert (nicht uebernommen).

## §1 ROOM3060 — Marvins Ende des Abschnitts (Punkt 1)

### §1.1 Original-Skript (ROOM3060.RDT sub03, scd_dump_room.py, Datei-Offsets)
| Offset | Bytes | Bedeutung |
|---|---|---|
| 0x0105E | `29 04` | Cut_chg 4 |
| 0x01060 / 0x01064 / 0x0106C | `2e 01 00` / `32 00 0f 08 00 00 d8 bc` / `33 00 00 00 28 05 00 00` | Work_set Spieler; Pos_set (2063,0,-17192); Dir_set 1320 |
| 0x01084 / 0x0108E | `41 01 df fa 00 00 f4 b1 64 64` / `40 00 09 00 df fa f4 b1` | Plc_neck + Plc_dest Modus 9 (Drehen) auf Hund 2 (-1313,-19980) |
| 0x0109A..0x010B6 | `34 0c 42 00` x3, Sleep 5/5/30 | die drei Hunde (Work_set 2,0..2) losschicken |
| 0x010BA / **0x010BE** / 0x010C2 | `2e 01 00` / **`3f 02 06 00`** / `09 0a 28 00` | Work_set Spieler; **Plc_motion(2,6)**; Sleep 40 |
| 0x010D2 / 0x010DE / 0x010E4 | `24 10 00 00` / `24 10 01 00` / `47 03` | Spielermodell zurueck auf Leon (0/1 je Bit 3:117); Aot_on 3 = Tuer -> ROOM3020 |
Zensus aller 240 RDTs (eigenes Skript, Work_set-Verfolgung je Sub): Plc_motion mit pc[1] = 2 auf dem SPIELER (Work_set 1,0)
gibt es NUR in ROOM3060 sub03 @0x010BE und ROOM3061 sub02 @0x011DE; die uebrigen 15 (1021, 10D0, 1141, 1171, 3061
@0x011EA, 3091) stehen auf Work_set 2,n (NPC).

### §1.2 RE-Beleg (selbst disassembliert, `re15_disasm.py` auf info/Re1.5/PSX.EXE)
- Plc_motion-Handler @0x80041b90: `lbu a2,1(v0)` (pc[1]), `lhu a1,2(v0)`, `lw v0,340(a0)` (Work-Entity),
  @0x80041ba8 `sb a1,148(v0)` +0x94 = Clip, @0x80041bb0 `sb v1,4(v0)` +0x4 = 4, @0x80041bb4 `sb zero,6(v0)`,
  @0x80041bc4 `sb a2,5(v0)` **+0x5 = pc[1]**, @0x80041bc8 `sh a1,452(v0)` +0x1c4 = pc[3]. Kein Spieler-Sonderweg.
- Spieler-Kommandotabelle @0x80073f90 [4] = 0x80030660 (Plc-Executor). 0x80030660: @0x8003068c `lbu v0,-13735(v0)`
  (= 0x800aca59 = Spieler+0x5), @0x8003069c Tabelle **0x80073e30**: [0] 0x80050cb8, [1] 0x80050ddc, **[2] 0x80050f00**.
- Kanal je Sub (jeweils `lw a0`/`lw a1` direkt vor `jal 0x8001f314` anim_set):
  Sub 0 @0x80050d40/48 = +0x180/+0x184 (Raum-RBJ), Sub 1 @0x80050e64/6c = +0x84/+0x16c (PLD-Grundbank),
  **Sub 2 @0x80050f88/90 = +0x170/+0x174**.
- Sub 2 (0x80050f00): Phase 0 -> @0x80050f34 +0x6 = 1, @0x80050f44 +0x8f = 7, @0x80050f54 +0x95 = 0 (+0x1c4 & 0x40 ->
  +0x8f = 0); Phase 1 anim_set; Rueckgabe != 0 (Clip-Ende) -> @0x80050fec **+0x6 = 2 = HALTEN** (kein anim_set mehr);
  nur mit +0x1c4 & 4 zurueck auf Phase 1 (@0x80051004/@0x80051010).
- Was +0x170/+0x174 beim Spieler ist: Waffen-Lader FUN_80036b68: Datei = u16-Tabelle @0x800741e8[Modell 0x800aca5c]
  + Waffe 0x800aca5d (@0x80036b84..bc4), Puffer 0x801d7700; @0x80036be4 `sw v0,-13368(at)` = 0x800acbc8 (+0x174, EDD),
  @0x80036c04 `sw v0,-13372(at)` = 0x800acbc4 (+0x170, EMR). Tabelle gelesen: `[76,76,76,76,97,97,97,97,76,76,76,76,76,
  76,76,118]` -> Modell 0x0D (Marvin) = 76 = **PL00Wxx**, Modell 4 (Elza) = 97 = **PL04Wxx**, Modell 15 = 118 = PL0FW00.
- PL00W01.PLW (EDD @+8): Clip 6 = **10 Bilder** (EDD+0x2e4..0x30c); PL04W01.PLW Clip 6 = 10 Bilder (EDD+0x3c0..0x3e8).

### §1.3 Messung Original (vorliegende Reihe o3060_s2, Marvin Modell 0x0D, Waffe 1; Werte aus RAM gelesen)
| t | Raum | cmd/sub/Phase | Clip/Bild | +0x168 (Bildzeiger) | Deutung |
|---|---|---|---|---|---|
| 2,0 s | 3060 | 1/0/1 | 3/0 | 0x801d78a8 = W-EDD+0x1a0 | W-Clip 3 (Stand) |
| 3,5 s | 3060 cut 4 | 1/0/0 | 1/1 | 0x801d7798 = W-EDD+0x90 | Pos_set (2063,-17192), Dir 1320 |
| 5,0 s | 3060 cut 3 | 4/**6**/1 | 1/15 | 0x801d77d0 = W-EDD+0xc8 = W-Clip 1 Bild 14 | nach dem Drehen (rot 1601): Sub 6 W-Clip 1 |
| **6,5 s** | 3060 cut 3 | **4/2/2** | **6**/0 | **0x801d7a10 = W-EDD+0x308 = W-Clip 6 Bild 9 (letztes)** | Plc_motion(2,6) gehalten |
| 8,0 s | **3020** cut 6 | 0/0/0 | - | - | Modell 0 (Leon) bei (-11250,-13250) |
+0x170 = 0x801d7b64 / +0x174 = 0x801d7708 in allen Staenden (= PLW-Puffer 0x801d7700 + dir); +0x1c4 = 0x20 bei 6,5 s
ist das Bild-Paritaetsbit (@0x800306c4 `xori v0,v0,0x20` je Executor-Bild). Bild: J_belege/o3060_65s_marvin.png.

### §1.4 Messung Port VORHER (Lauf m3060_vor, Stand master + rbj-binder, RE15_ANIM_TRACE)
Spieler (Marvin, PL0D) ab Sub-03-Start: F25 Clip 105 (W5), F28 W-Clip 1 (16), F44 W-Clip 2 (52), **F65 Clip 6 mit 50
Bildern = PL00.EDD Clip 6 (Knien)** — scd_vm.c `anim_use_pl00 = (entity != 0)` waehlt fuer pc[1] = 2 die PLD-Grundbank
statt +0x170/+0x174. Bild: Marvin kniet ab F75 (J_belege/vergleich_3060_marvin.png, Spalte "Port vorher F90").
**Ursache Punkt 1 = Plc_motion(2,6) auf dem Spieler spielt die falsche Bank.**

### §1.5 Messung Original nachgezogen (Video, J41b/orig/o3060_v4.mp4 ab Savestate o3060_s2_t00)
Cut 3 bei 10 Bildern/s (J_belege/o3060_ende_marvin_10fps.png): Marvin steht nach dem Drehen (W-Clip 1 -> 2), hebt in
3 Bildern den Arm vor die Brust (W-Clip 6, 10 Bilder) und haelt die Haltung bis zur Abblende; danach Schwarz (Tuer) und
ROOM3020 mit Leon. Der Port vorher kniete an dieser Stelle (PL00 Clip 6, 50 Bilder).

### §1.6 Umsetzung Punkt 1 (und 2b)
`engine/src/wclip_r41.c` + `include/re15_wclip_r41.h` (Vorgaenger-Stand 541881b5, hier geprueft und ergaenzt):
- Spieler-Plc_motion(2,c) ausserhalb ROOM1141 (dort bleibt Runde 39 dining_r39.c) = W-Bank-Clip c; Zeichner ueber den
  vorhandenen Haken `re15_dining_r39_w_clip` (main.c W-Bank-Composite wie Zielen/Ereignis-Griff), Halten im Spielschritt
  `w_halten` -> `re15_wclip_r41_halten`: Phase 2 = letztes Bild (@0x80050fec), Schleife nur mit +0x1c4 & 4.
- NEU in dieser Fortsetzung: Ende beim Raumwechsel (gemessen Lauf m3060_tuer: das erste ROOM3020-Bild zeigte Leon noch mit
  W-Clip 6; Original o3060_s2_t04 dort cmd 0 / Sub 0).
- Keine Konstante ohne Beleg: Sub 2 @0x80041bc4/0x80073e30[2], Kanal @0x80050f88/90, W-Bank @0x80036be4/@0x80036c04 +
  Tabelle @0x800741e8, Halten @0x80050fec, Bildzaehler @0x8001f618.

### §1.7 Messung Port NACHHER (Lauf m3060_tuer = Weg des Spiels, integration_r41_stage3szenen_a_3060)
Leon -> ROOM3020, Szene sub02 (RE15_FORCE_EVENT 2 = Ereignis des Aot slot 3 @0x00BE2) -> `[pld] Spielermodell -> PL0D`,
Tuer -> ROOM30C0; Sprung ROOM3040; **Tuer Slot 0 -> ROOM3060 Ankunft (2063,0,-17192) Cut 4** (ROOM3040 @0x0127E Door_aot_set sce 2, Bit 4:196 —
im Spiel setzt es ROOM3040 sub02 @0x014EE beim Aufschliessen);
`[wclip41] ROOM3060 F65 Spieler Plc_motion(2,6) -> W-Bank-Clip 6 (... Waffe 1, 10 Bilder)`; RE15_ANIM_TRACE: Clip 6 /
10 Bilder, Bilder 0..8 je einmal, **Bild 9 31x gehalten** bis zum Ende (Bild 104); vorher F25 W5 (Drehen), F28 W-Clip 1,
F44 W-Clip 2 wie das Original (t=5,0 s Sub 6 W-Clip 1). Danach Tuer -> ROOM3020 als PL00 (Leon). Bild:
J_belege/vergleich_3060_marvin_reihe.png (oben Original-Video 10 Bilder/s, unten Port-Framedump Lauf m3060_tuer3 alle
5 Bilder: Stehen, Arm vor die Brust, Halten — gleich) und J_belege/vergleich_3060_marvin.png.
- Wechsel zurueck zu Leon (Lauf m3060_wechsel, gleicher Weg, RE15_EXIT_AT 1250#3020, RE15_ANIM_TRACE): letztes ROOM3060-Bild
  F104 Spieler Motion 6 / Clip 6 / 10 Bilder / Bild 9; `[aot] DOOR FIRE slot=3 ... spawn=(-11250,0,-13250)` (ROOM3060
  @0x010E4 Aot_on 3), `[pld] Spielermodell -> PLD/PL00.PLD`; erstes ROOM3020-Bild **F0 Motion 210** (Port-Leerlauf
  W01-Clip 1, 16 Bilder, player_common.c RE15_MOTION_IDLE_SETTLE), F6 Motion 200 (W01-Clip 3 Stand), danach der Leerlauf-
  Wechsel 211/212 — kein W-Clip-6-Bild in 3020 (40 Clip-6-Zeilen, alle in 3060, davon 31x Bild 9). Original o3060_s2 t=8,0 s: ROOM3020 cut 6, Modell 0, cmd 0/Sub 0 bei (-11250,-13250) — gleich.

## §2 ROOM3061 — Sherry kriecht in den Schacht, Elzas Ende (Punkt 2)

### §2.1 Original-Skript (ROOM3061.RDT, scd_dump_room.py; Nachrichten rdt_msgdump.py)
- Ausloeser: main00 Door_aot_set Slot 1 @0x00E96 Rechteck (-4500,-27300, 2600x1300); sub00 @0x00F3A `46 01 03 31 ff 00 18 02`
  (Bit 4:244 = 1) = Aktions-AOT (Flags 0x31: 0x10 Aktion, 0x20 Vorwaerts-620) -> Evt_exec sub02. Bit 3:160 muss 1 sein
  (sonst setzt @0x01016 Slot 1 auf sce 1 zurueck). Aktionstaste = Quadrat (Memory reai-v2-door-transition).
- Akteure: em0 = Typ 0x4B Sherry (@0x00F44), em1 = Typ 0x49 John (@0x00F58), em2 = Hund 0x20 (@0x00F6C), Obj 0 = Gitter
  (@0x00F80, Lage (-19095,-495,-27045) rot (0,3072,0)). RBJ-Records: rec0 Marker 0x1 (Spieler, 11 Clips), rec1 Marker 0x2
  (em0 Sherry, Clips 90/75/60 Bilder).
- **Kriechen = sub04** (Evt_exec @0x01138, parallel zu sub02):
  `37/39 02 00 00`, `37/39 03 01 00` (SCA 2/3 aus) @0x0123E..4A; Sherry Plc_dest(9,1) auf (-19095,-30000) @0x01252 + Gosub 7;
  **Plc_motion(0,0)** @0x01266 (rec1 Clip 0, 90 B.), Sleep 33, Se_on; **Work_set(3,0) + Super_set `48 00 80 8e fc 00 c0 00 67 ff
  00 00 00 04 00 0c`** @0x0127E (Gitter an Sherrys Teil 14, lokal (252,192,-153), Rot (0,1024,3072)), Sleep 26, Se_on,
  **Super_reset `49 00 48 00 00 0c 00 00`** @0x0129E, Speed_set(0,40)x4/Sleep 9/(0,80)x4/(0,40)x2 Add_speed (Gitter rutscht +x);
  Evt_exec sub05 (Sherry Speed_set(0,-5) x60 Add_speed); Sleep 7; **Plc_motion(0,1)** @0x012E4 (75 B.), Sleep 20,
  **Plc_flg SET 1** (Sleep 22), **SET 2** (22), **SET 1** (11) = 20+22+22+11 = 75 = Clip-1-Laenge; **Plc_motion(0,2)** @0x01304
  (60 B.), For 34 x [**SET 4** Sleep 8, **SET 6** Sleep 26, **SET 5** Sleep 24, **SET 4** Sleep 2] = 60 = Clip-2-Laenge;
  SCA zurueck @0x01330..3C.
- **Elzas Ende = sub02** @0x011C6..0x011FA: Work_set Spieler, Plc_neck + Plc_dest(9,0) auf den Hund (-13350,-22000),
  Gosub 6, **Plc_motion(2,6)** @0x011DE, Sleep 5, John Plc_motion(2,6) @0x011EA, Sleep 60, Save 0x10 = 0x0F (Sherry), Aot_on 3
  -> ROOM30C1.

### §2.2 RE-Beleg (selbst disassembliert, PSX.EXE)
- Plc_flg @0x80041fb8: pc[1] = 0 -> @0x80041ffc `or` (+0x1c4 |= u16), **1 -> @0x80042008 `sh a2,452(v0)` (+0x1c4 = u16)**,
  2 -> @0x8004201c `xor`.
- NPC-Executor State 4 @0x80050be8 (Spieler-Gegenstueck 0x80030660 @0x800306b4..f8 identisch): Halbtakt-Tor
  @0x80050c00..0c (0x10 und 0x20), Sub-Tabelle 0x80076ca0 @0x80050c24, @0x80050c50 `xori v0,v0,0x20`,
  **@0x80050c6c `andi v0,v0,0x1` -> @0x80050c78 `jal 0x800369f8` (a0 = 0, a1 = 0)**,
  **@0x80050c94 `andi v0,v0,0x2` -> @0x80050ca0 `jal 0x800369f8` (a0 = 0, a1 = 1 @0x80050ca4)**.
- **FUN_800369f8 = Fuss-/Glied-Anker** (nicht Keyframe-Wurzelweg): s3 = Entity+0x188 (Part-Feld, Stride 0xAC = 172);
  @0x80036a34 `jal 0x80068098` (RotMatrix(+0x68) -> Entity-Matrix +0x20, Translation = +0x34/+0x38/+0x3c);
  @0x80036a48 `jal 0x80022da0` (Entity-Matrix x Part 0 lokal @s3+24), @0x80036a58 (x Part 1 @s3+196);
  @0x80036a60..70 s3 += a1*0x204 + 0x2b0 (Kette a1=0: Parts 2,3,4 / a1=1: Parts 5,6,7) -> @0x80036a7c (s3-320),
  @0x80036a8c (s3-148), @0x80036a9c (s3+24) = **feste Kette 0->1->2->3->4 bzw. 0->1->5->6->7** mit den LOKALEN Matrizen
  des aktuellen Bildes. Modus 0 (@0x80036aa4 `bne s2,zero`): **@0x80036ab4..ac8 `x -= (neu.x - Part[+84])`,
  @0x80036ad4..aec `z -= (neu.z - Part[+92])`**, Part+84/+92 = Welt-Translation dieses Parts aus dem letzten Zeichnen
  (Welt-Matrix Part+64, Zusammensetzen FUN_8001e8c8 / 0x80022da0). Das Kettenende bleibt also am Boden stehen, die Figur
  wandert um dessen Gegenbewegung. Modus != 0 (@0x80036af0..b44): nur Strecke -> +0x8c (hier nicht benutzt).
- Super_set @0x80040b20: pc[2] & 0xc0 = 0x80 und pc[3] & 0x80 -> **Vater = Gegner (pc[2]-0x80) Part (pc[3]&0x7f) Welt-
  Matrix** (@0x80040c18 `addiu at,at,-11340` = 0x8009d3b4 + pc[2]*0x1F4 = Gegner+0x188, @0x80040c30 +64), @0x80040c74
  `sw v0,116(a2)` (Objekt+0x74 = Vater-Zeiger); @0x80040c78..cbc lokal pos (pc+4/6/8) -> +0x34/+0x38/+0x3c,
  Rot (pc+10/12/14) -> +0x68/+0x6a/+0x6c. Super_reset @0x80040cd0 (Kopie Welt +0x5c/+0x60/+0x64 -> +0x34.., Vater =
  0x80072d4c). Im Port beide nicht umgesetzt (RE15_SCD_OPCODES_REFERENCE.md: `—defer`, scd_vm.c nur Laengen 16/8).
- Zensus Plc_flg mit Bit 1/2 in allen 240 RDTs: NUR ROOM3061 sub04 (5 Stellen: SET 1/2/1 @0x012EC/@0x012F4/@0x012FC,
  SET 6 @0x01316, SET 5 @0x0131E; SET 4 @0x0130E/@0x01326 ohne Bit 1/2).
- Andere Schreiber von +0x1c4 (Member 16, re15_actor_set_member case 16), Zensus aller 206 SCD-RDTs: Member_set 0x34 auf
  Member 16 = 0; Plc_motion pc[3] (@0x80041bc8 `sh a1,452(v0)`) in allen Aufrufen 0; Member_set2 0x35 auf Member 16 = 86
  Stellen, alle Kriechtor-Maschine ROOM1030/1031/1040/1041 = Member_get 0x3D (@0x80041238) + Calc 0x26 AND 0x0FFF / OR
  0x2000 bzw. OR 0x1000 -> Bit 0x1/0x2 bleiben unberuehrt. Port-Code setzt +0x1c4 nur mit 0x04/0x20/0x80/0x1000. Der
  Executor-Wechsel (§2.6) wirkt also nur in ROOM3061 sub04 (Riegel unit_r41_stage3szenen_a_zensus). Plc_motion-Flagbyte pc[3] ist in allen
  1359 Aufrufen 0.

### §2.3 Messung Original (DuckStation, MZD-Disc; Werkzeug `re15_port/tools/r41_j/j41_szene.py`)
Zustand `J41/orig/o3061_a.sav` (Elza, 3061, Sherry/John angekommen; Bits 4:244 = 1, 3:160 = 1 im RAM gelesen) gepatcht
(`re15_ss_patch.py` 0x800aca88 = -3500, 0x800aca90 = -26650, Spieler steht danach bei (-3500,-26276) im Slot-1-Rechteck)
-> `J41b/orig/p3061_aot.sav`; Laden, Quadrat im 1,5-s-Takt. Lauf v1 mit Kreuz/Kreis: kein Ausloesen (Aktion = Quadrat).
Lauf **o3061_v3** (Video 165 s, `J41b/orig/o3061_v3.mp4`): "It's locked." (Msg 0 @0x01040), Verfolgung, am Gitter
Msg 3-5; **Sherry kniet, hebt das Gitter mit der Hand ab und legt es zur Seite, geht auf alle Viere und kriecht IN die
Oeffnung, bis nur noch die Fuesse herausschauen und sie verschwunden ist** (J_belege/o3061_kriechen_4fps.png, ab 20 s,
4 Bilder/s); Cut 1, Hund erhebt sich und kommt; **Elza dreht sich zum Hund und zieht die Waffe (Arme vorn, halten)**,
John ebenso (J_belege/o3061_ende_elza_zieht.png); Abblende -> ROOM30C1, Spieler = Modell 0x0F (Sherry), Waffe 0
(Resume-Stand: Raum 0x0c, mdl 0xf, w0).

### §2.4 Messung Port VORHER (Lauf e3061_vor, Elza ueber PSELECT, RE15_SET_FLAG 4:244/3:160, RE15_FIRE_AOT 1@400)
- Sherry: Plc_motion(0,0/1/2) und Plc_flg SET 1/2/6/5/4 kommen an (debug.log F1085..F1258), aber **Sherry bleibt vor der
  Wand liegen** und kriecht nicht hinein (J_belege/e3061_vor_kriechen.png F1092..F1332); das Gitter bleibt am Platz.
  Ursache: enemy_ai_common.c re15_npc_executor — Bit 1 ruft `re15_clip_root_motion_delta` (Keyframe-Wurzelweg, falsche
  Mechanik, falsche Bank), **Bit 2 "deferred"**; Super_set/Super_reset fehlen.
- Elza: Plc_motion(2,6) (log Zeile 796) -> PL04-Grundbank Clip 6 = **Knien** (J_belege/e3061_vor_ende_elza_kniet.png
  F1416..F1452), Original zieht die Waffe = W-Bank PL04W01 Clip 6 (§1.2). Dieselbe Ursache wie Punkt 1.

### §2.5 Messung Original, Savestate-Reihe o3061_s2 (94 Slot-1-Staende im 2-s-Takt, ab Szenenstart; Auszuege
`J_belege/o3061_s2_akteure.txt`, `J_belege/o3061_s2_sherry_gitter.txt`; Gitter = Objekt 0, Struktur 0x800b3f98 (lokal
+0x34, Rot +0x68, Vater +0x74, Welt-Translation +0x5c), Sherry = Gegner 0 @0x800acc2c)
| Stand | Sherry Clip/Bild, +0x1c4 | Sherry (x,z) | Gitter lokal / Rot |
|---|---|---|---|
| t09 22 s | 0/20, 0x00 | (-18860,-26266) r1067 | (-19095,-495,-27045) / (0,3072,0), Vater 0x80072d4c |
| t10 24 s | 0/78, 0x00 | (-18860,-26266) | (-18356,-266,-26788) / (72,3072,0) (rutscht) |
| t11 26 s | 1/47, 0x22 | (-19054,-27044) | **(-18116,-266,-26788)** / (72,3072,0) = Endlage |
| t12 28 s | 2/30, 0x06 | (-18792,-28234) | dto. |
| t13 30 s | 2/22, 0x06 | (-18799,-29042) | dto. |
| t14..t18 | 2/.., 0x04 / 0x24 | **(-18795,-30700)** | dto. |
| t17 38 s | Spieler (Elza) cmd 4 / Sub 2 / Phase 2 / Clip 6; John Sub 2 Phase 2 Clip 6 | | |
| t19 42 s | Raum 0x0C (30C1), Spieler Modell 0x0F (Sherry), Waffe 0 | | |
Gitter-Loslass-Lage = Endlage - 640 (Speed_set-Summe) = (-18756,-266,-26788).

### §2.6 Umsetzung Punkt 2
- **Glied-Anker** `engine/src/fussanker_r41.c` + `include/re15_fussanker_r41.h` (Adressen je Zeile): feste Kette
  E-0-1-(2|5)-(3|6)-(4|7) mit den lokalen Matrizen des aktuellen Bildes (Kanal je Sub wie @0x80050d40/e64/f88),
  Vergleich gegen die Part-4/7-Welt-Lage des letzten Spielschritts (Hierarchie wie FUN_8001e8c8), x/z -= Differenz
  (@0x80036ab4..aec). Haken nur in `enemy_ai_common.c` (NPC-Code, VERTRAG 1.3 J): re15_npc_executor (alter Zweig
  Keyframe-Wurzelweg/"deferred" ersetzt, 2 Zeilen @0x80050c6c/@0x80050c94) und nach `re15_npc_ai_tick(s)` in
  re15_enemy_ai_run_all `re15_fussanker_r41_merken_aktor(e)` (1 Zeile: Vorbild-Stand nach Executor + Wandklemme, vor dem
  SCD-Takt des naechsten Bildes — SCD @0x8001cdec laeuft im Original vor den Entities @0x8001ce04).
  PORT-WAHL: ohne gemerkten Vorbild-Stand keine Verschiebung (Raumwechsel/erstes State-4-Bild; kein Lieferfall).
- **Super_set/Super_reset** `engine/src/superobj_r41.c` + `include/re15_superobj_r41.h`: Vater-Art 0x80 + Part-Bit
  (Gegner-Teil-Weltmatrix) und 0x00 (Welt); Reset uebernimmt die Welt-Lage, Rot = pc+2/4/6. Haken: `scd_vm.c`
  register_opcodes (0x48/0x49, 1 Block), `platform/pc/main.c` pc_prop_world (4 Zeilen: angehaengtes Prop = Vater x lokal).
  OFFEN (kein Lieferfall): Vater 0x40/0xc0, Gegner ausserhalb State 4 (ROOM4030/4031 sub07 `48 00 80 81`, Typ 0x2D:
  dort wird der Opcode wie bisher uebersprungen, Messzeile `[superobj41] ... OFFEN`).
- PSX-Plattform: kein Zeichner-Haken fuer angehaengte Props (asset/render_psx nicht gebaut) — OFFEN fuer die Paritaet.

### §2.7 Messung Port NACHHER (Laeufe e3061_nach1 = Anker, e3061_nach2 = Anker + Super_set; Elza, RE15_FUSSANKER_LOG)
- Anker-Takte (debug.log `[fussanker41]`): Clip 1: Kette 0 x22, Kette 1 x22, Kette 0 x11; Clip 2: (Kette 1 x26,
  Kette 0 x24) x2 — = die Plc_flg-Folge von sub04.
- Sherry-Lage Port gegen Original: Clip 1 Bild 47 **(-19055,-27019) / (-19054,-27044)**; Clip 2 Bild 30 (1. Runde)
  **(-18791,-28248) / (-18792,-28234)**; Clip 2 Bild 22 (2. Runde) **(-18799,-29081) / (-18799,-29042)**.
- Gitter: Super_reset F1040 Welt-Lage **(-18753,-266,-26778)** gegen Original-Loslass-Lage (-18756,-266,-26788);
  Endlage Port (-18113,-266,-26778) gegen (-18116,-266,-26788).
- Bild: Sherry zieht das Gitter mit beiden Haenden heraus, legt es links flach ab, kriecht hinein, bis nur die Beine
  herausschauen, dann ist sie weg (J_belege/e3061_nach_gitter_kriechen.png, Seite an Seite mit dem Original:
  J_belege/vergleich_3061_gitter_kriechen.png).
- Endlage: nach der Schleife (For-Zaehler 2 @0x01308 `0d 00 22 00 02 00`, For/Next @0x8003f540/@0x8003f674 selbst
  gelesen: genau 2 Runden) schaltet sub04 @0x01330..0x0133C die SCA-Eintraege Region 2/0 (#9) und 3/1 (#17) wieder ein
  (Rechteck x -31340..12792, z -30382..-26744); Sherry steht darin und wird von der NPC-Wandklemme (Wurzel @0x8011cc64
  `jal 0x8003b0a4`, Port `re15_npc_wall_clamp`) herausgeschoben. GEMESSEN Port (integration_r41_stage3szenen_a_3061,
  state.txt, Ausloesen per Quadrat §2.11): **F1299 -> F1300 Sprung (-18792,-29716) -> (-18792,-30700)**; Original o3061_s2/s3 ab t14 bzw. t12 **(-18795,-30700)**.
  Gleiche Endlage — keine Rest-Abweichung (die frueheren Laeufe e3061_nach1/2 lasen nur die letzte Anker-Zeile).

### §2.8 Dichte Original-Reihe o3061_s3 (Slot-1 alle 0,4 s, `J_belege/o3061_s3_dicht.txt`)
- t03 Clip 0 Bild 52: Gitter lokal **(252,192,-153) Rot (0,1024,3072)** = Super_set-Werte (angehaengt); t04 Bild 79:
  rutscht (-18276); ab t05 Endlage (-18116,-266,-26788).
- t05 Clip 1 Bild 17: Sherry x **-18980 = -18860 - 24*5** (sub05 Speed_set(0,-5), 7+17 Bilder vor dem ersten Anker-Bild 20).
- Clip 2: t08 Bild 19 +0x1c4 0x26 (SET 6), t09 Bild 44 0x05, t10 Bild 12 0x06, t11 Bild 39 0x25 (SET 5), **t13 Bild 29
  0x24 = nur SET 4** -> die Schleife ist nach 2 Runden zu Ende (For-Zaehler 2 bestaetigt).
- Lagen: t11 Runde 2 Bild 39 **(-18810,-29279)** — Port F1244 (-18807,-29269); t12 Bild 5 nach Schleifenende
  **(-18795,-30700)** = Herausschieben nach dem SCA-Wiedereinschalten (§2.7, im Port ebenso gemessen).

### §2.9 Bild-Zaehler des Originals und bytegleicher Riegel
- FUN_8001f314 setzt +0x168 = Eintrag des Bildes +0x95 (@0x8001f36c) und ruft FUN_8001f3bc; dort wird NACH dem Setzen der
  Pose hochgezaehlt (@0x8001f618 `addiu v0,v0,1` / @0x8001f61c `sb v0,149(v1)`, Clip-Ende -> 0 @0x8001f63c, Rueckgabe 1).
  Ein Savestate traegt also +0x95 = gezeichnetes Bild + 1 (3060: Phase 2 mit +0x95 = 0 und +0x168 = Clip-6-Bild 9).
- Mit dieser Zuordnung trifft der Port-Anker die Original-Reihe o3061_s3 **bytegleich**: Start t08 (Runde 1 Bild 18,
  (-18780,-28017)) -> nach 43 Anker-Bildern **(-18783,-28806) = t10**; Start t09 (Bild 43, (-18791,-28390)) -> nach 45
  Anker-Bildern **(-18810,-29279) = t11** (Riegel unit_r41_stage3szenen_a_anker, Abweichung 0).
- Gitter: Sherry (-18860,0,-26266) r1067 Clip 0 Bild 58 -> Super_reset-Lage **(-18756,-266,-26788)** = Original-Loslass-Lage
  bytegleich (unit_r41_stage3szenen_a_superobj).
- Sherrys Knochen-Eltern (CDEMD1 Typ 0x4B, Sektor 1776): `-1 0 1 2 3 1 5 6 0 0 9 10 0 12 13` — die feste Kette
  0-1-2-3-4 / 0-1-5-6-7 von FUN_800369f8 IST die Hierarchie; Teil 14 (Gitter-Hand) haengt an 13-12-0.
- Im Spiellauf bleibt ein konstanter Rest (3,10) aus Sherrys Lage VOR dem Kriechen (Port (-18857,-26256), Original
  (-18860,-26266), Ende des Plc_dest-Gangs in sub02/sub04 — nicht Teil dieser Punkte, OFFEN notiert).

### §2.10 Messung Port NACHHER — Elzas Ende (Punkt 2b) und John
- `[wclip41] ROOM3061 F1456 Spieler Plc_motion(2,6) -> W-Bank-Clip 6 (... Waffe 1, 10 Bilder)` (PL04W01, Modell 4 ->
  Tabelle @0x800741e8 = 97 = PL04Wxx); RE15_ANIM_TRACE: Clip 6 / 10 Bilder, Bild 9 gehalten (Riegel: 56x bis zum Wechsel).
  Original o3061_s2 t17: Spieler cmd 4 / Sub 2 / Phase 2 / Clip 6.
- John (Typ 0x49) Plc_motion(2,6) @0x011EA: eigene Bank 1 Clip 6, 10 Bilder, Bild 9 gehalten (60 Zeilen im Trace) —
  Original t17: Gegner 1 Sub 2 Phase 2 Clip 6. (Port-Weg unveraendert, war schon richtig.)
- Bild: J_belege/vergleich_3061_elza_ende.png (oben Original-Video 6 Bilder/s, unten Port alle 8 Bilder): Drehen zum Hund,
  Arme vor (W-Clip 6), Halten — gleich; vorher kniete Elza (J_belege/e3061_vor_ende_elza_kniet.png).

### §2.11 Ausloesen wie im Spiel (Quadrat am AOT statt Abkuerzungs-Schalter)
Der Riegel integration_r41_stage3szenen_a_3061 loest die Szene jetzt so aus wie der Spieler (vorher RE15_FIRE_AOT 1@450):
Spielstand Elza (-3500,-25500) Blick 1024 + Bits 4:244/3:160 (probe_r36_elza_karte), CONTINUE; ab Spielbild 420 0,5 s
vor, dann Quadrat im 1,5-s-Takt. Gemessen (Lauf `re15_port/build/r41_stage3szenen_a_wd_3061_c42fad07/lauf`, debug.log +
state.txt): F420 vor -> **F434 Elza an der Wand (-3165,-26240)**, Vorwaerts-620-Punkt (-3165,-26860) im Slot-1-Rechteck
x -4500..-1900 / z -27300..-26000 (main00 @0x00E96; FORWARD-Test @0x80042ea0, aot_common.c); **F450 Quadrat ->
`[scd] thread start slot=10 first_op=0x22` = sub02** (sub00 @0x00F3A Aktions-AOT); **F497 Cut_chg(6) @0x01062**; F1049
Super_set / F1075 Super_reset (-18753,-266,-26778); Anker-Takte 22/22/11 + 2 x 26/24; Sherry-Lagen je (3,10) neben dem
Original; F1300 Sherry (-18792,-30700); F1456 Elza W-Clip 6, 56 x Bild 9; Wechsel 30C1 als PL0F. Elza steht ab F136
bei (-3165,-25115) statt (-3500,-25500): Johns Einlauf (sub10) schiebt sie F131..F136 (§4.6, Spur H).

## §3 Tests (registriert NUR in re15_port/tests/unit/probes/r41_stage3szenen_a.cmake, + _rbj.cmake des Binders)
| Test | misst | Ergebnis |
|---|---|---|
| unit_r41_stage3szenen_a_zensus | 206 RDTs mit SCD + 34 Platzhalter: Spieler-Plc_motion(2,c) nur 3060 @0x010BE / 3061 @0x011DE; Plc_flg Bit 1/2 nur 3061 (5 Stellen); Super_set nur 3061 @0x0127E + 4030/4031; andere Schreiber von +0x1c4 Bit 1/2: Member_set 0, Plc_motion pc[3] 0, Member_set2 86 alle bit-erhaltend | gruen |
| unit_r41_stage3szenen_a_anker | Anker an Sherrys echter Bank, o3061_s3 t08->t10 und t09->t11: Abweichung **0** | gruen |
| unit_r41_stage3szenen_a_superobj | Super_set/_reset am echten Bytestrom: Loslass-Lage = Original **(0,0,0)**, Vater ausserhalb State 4 -> uebersprungen | gruen |
| unit_r41_stage3szenen_a_wclip | (2,6) -> W-Clip 6, Halten Bild 9, Schleife, Ende bei Befehl/Raumwechsel, Sub 0/1 und 1141 unberuehrt | gruen |
| integration_r41_stage3szenen_a_3060 | echte exe: 3020-Szene -> PL0D -> Tuer 3040 -> 3060, W-Clip 6 / 10 Bilder, Bild 9 >= 25x | gruen |
| integration_r41_stage3szenen_a_3061 | echte exe: Spielstand Elza (-3500,-25500) + CONTINUE ROOM3061, vor an die Wand + QUADRAT am Slot-1-AOT -> sub02 (Cut_chg 6 @0x01062, §2.11), Gitter-Lage, Anker-Takte, Sherry-Lagen (Rest (3,10)), Sherry im Schacht (-30700), Elza W-Clip 6 gehalten, Wechsel 30C1/PL0F | gruen |
| unit_r41_stage3szenen_a_rbj_{mechanik,zensus,kanal} | Binder (§0, Vorgaenger) | gruen |

Gezielte Laeufe (keine volle Suite; re15_pc.exe 20:31 = letzter Code-Stand, Test-exe 20:48 nach der Zensus-Erweiterung):
- `ctest --test-dir re15_port/build -R "^(unit|probe)_(r41_stage3szenen_a|npc|marvin|1090_npc|1090_push|room1040|lane_b1|r33_tuer1120|r34n_d_adaruf|r35_affen|r35_cut1150|r35_cut10f0|r35_entladen|r36_elza|r36_szene|r36_szenenleon|r36_gegnerzahl|r36_schluessel|r36_zombieki_szenen|r37_elzaems|r37_roy|r37_szene11c0|r38_dining1141|r38_elzatoene|r38_heliport1171|r38_kleinkram|r38_npckante|r38_szenenschutz|r39_dining1141|r39_elzazombies|r39_funk10f1|r39_heliport1171|r39_johnruf1041|r16_tuer1040|room1140|anim)" -j 4 --timeout 600` ->
  **`100% tests passed, 0 tests failed out of 218`**
- `ctest --test-dir re15_port/build -R "^integration_(r41_stage3szenen_a|r39_dining1141|r37_elzaems_elza3091|r37_szene11c0|r36_szene1150)" -j 3 --timeout 1200`
  -> **`100% tests passed, 0 tests failed out of 9`** (3061 dabei noch mit RE15_FIRE_AOT)
- `ctest --test-dir re15_port/build -R "^integration_r41_stage3szenen_a_3061$" --timeout 1200` (Ausloesen per Quadrat, §2.11)
  -> **`100% tests passed, 0 tests failed out of 1`**
- nach der Abbruch-Haertung des Riegelskripts (exe-Kopie wird auch bei Abbruch entfernt):
  `ctest --test-dir re15_port/build -R "^integration_r41_stage3szenen_a_" -j 2 --timeout 1200` ->
  **`100% tests passed, 0 tests failed out of 2`** (3061 per Quadrat 61,7 s, 3060 148,0 s)

## §4 OFFEN (je mit Adresse; keiner betrifft das, was der Nutzer in 3060/3061 sieht)
1. Super_set-Vaterarten 0x40 (Spieler), 0xc0 (Objekt) und Gegner ausserhalb State 4 (Lieferfall nur ROOM4030/4031 sub07
   `48 00 80 81`, Typ 0x2D Teil 1): der Opcode wird wie vor dieser Spur uebersprungen (Messzeile `[superobj41] ... OFFEN`).
   GEMESSEN Lauf r4030_super (Sprung 4030, RE15_FORCE_EVENT 6 -> sub07): `[superobj41] F171 Super_set Objekt 0 Art 0x80
   Teil-Byte 0x81 lokal (0,0,0) Rot (0,0,0) -> OFFEN (Vater nicht berechenbar, uebersprungen)`, Gegner 0 Typ 0x2D in State 1
   — ROOM4030 verhaelt sich wie vor dieser Spur. Fuer die Gegner-Pose ausserhalb State 4 fehlt dem Port die
   Kanal-Zuordnung je AI-Zustand (Zeichner main.c).
2. PSX-Plattform: Zeichner-Haken fuer angehaengte Props (pc_prop_world) und fuer den W-Bank-Clip (dining_r39-Haken) gibt
   es nur im PC-Zeichner; die Engine-Module (fussanker/superobj/wclip) sind plattformneutral. PSX in dieser Runde nicht gebaut.
3. Sherrys Lage VOR dem Kriechen: Port (-18857,-26256), Original (-18860,-26266) — Ende des NPC-Gangs (Plc_dest Modus 5/9 in
   sub02/sub10 @0x013B2.., Sub-VM-WALK); danach traegt der Anker den Rest (3,10) unveraendert weiter. NPC-Gang-Genauigkeit,
   nicht Teil dieser Punkte.
4. Spieler-Executor 0x80030660 hat dieselben Anker-Bits (@0x800306cc `andi 0x1` / @0x800306e8 `andi 0x2`); kein ausgelieferter
   Datensatz setzt sie am Spieler (Zensus) -> nicht verdrahtet.
5. Vorbild-Stand des Ankers nach dem NPC-Takt: ein spaeteres Objekt-Ausschieben desselben Bildes (FUN_8002bd44 @0x8002be0c)
   waere nicht enthalten — in ROOM3061 kein Fall (Gitter ohne Box).
6. Beobachtung ohne Bewertung: Johns Einlauf (sub10) schiebt im Port eine im Ausloese-Rechteck wartende Elza bis x 262; im
   Original stand die Figur erst nach dem Einlauf dort (o3061_a). NPC-Spieler-Kollision = Spur H. Der Riegel 3061 wartet
   deshalb ausserhalb des Rechtecks ((-3500,-25500), dort F131..F136 nur auf (-3165,-25115) geschoben) und geht erst nach
   dem Einlauf selbst an die Wand (§2.11) — kein Abkuerzungs-Schalter mehr.

## §5 Fuer den Nutzer
- Sehen: **Leon**, STAGE3, Ende von Marvins Abschnitt (ROOM3020-Szene -> als Marvin ueber 30C0 ... 3040 -> Tuer -> ROOM3060):
  Marvin dreht sich zu den Hunden, hebt die Arme mit der Waffe vor die Brust und haelt die Haltung (Waffen-Bank PL00W01
  Clip 6; vorher: er kniete sich hin); dann Wechsel zurueck zu Leon in ROOM3020. **Elza**, ROOM3061 (an der Wand beim
  Gitter Quadrat druecken): Sherry zieht das Gitter mit beiden Haenden heraus, legt es links ab, geht auf alle Viere und
  kriecht in die Oeffnung, bis sie verschwunden ist (vorher: sie lag vor der Wand); am Ende dreht sich Elza zum Hund und
  hebt die Arme mit der Waffe (PL04W01 Clip 6; vorher: Knien), John ebenso (eigene Bank Clip 6); dann ROOM30C1 als Sherry.
- Keine neuen Assets (shared_assets/RE2 unveraendert), keine Sprachdateien, keine neuen Bits/Nachrichten-Ids.
- Ueberschneidung Spur K: Sherrys MODELL ab 30C1 (PL0F) und die 3091-Szene gehoeren K; hier nur die Schacht-Bewegung der
  NPC-Sherry (Typ 0x4B, EM4B aus CDEMD1) in 3061. Der RBJ-Binder (§0, e4a9f39c) wird von K per cherry-pick uebernommen.
- Gemeinsame Dateien (Haken, je "Runde 41 Spur J"): enemy_ai_common.c (Executor-Bits 2 Zeilen, Vorbild-Stand 1 Zeile),
  scd_vm.c (register_opcodes 0x48/0x49, 1 Block), platform/pc/main.c (pc_prop_world 4 Zeilen; RBJ-Spieler 1 Zeile §0),
  dining_r39.c (3 Zeilen wclip), enemy_common.c + platform/psx/src/asset_psx.c (RBJ-Binder §0).
- Werkzeuge: `re15_port/tools/r41_j/j41_szene.py` (DuckStation-Szene aus einem Savestate: Video und/oder Slot-1-Reihe,
  settings.ini bytegleich zurueck), `lauf.sh` (Port-Messlauf mit eigener exe-Kopie).

## Nachbesserung 1 (nach Abnahme J 0, `J_abnahme_0.md`; Start 2026-10-24 spaet, Kopf 238c1319, Baum sauber)
Ausgangslage: Abnahme J 0 bewertet alle drei Nutzer-Punkte (1 ROOM3060 Marvin, 2a Sherry kriecht, 2b Elzas Ende) und A-E5
(RBJ-Binder) als **erfuellt**; offen sind zwei Gate-Maengel: **V1** (Vertrag, formal) und **M2** (Doku). Dazu Hinweise H1-H4
(keine Maengel) und Nebenbefund N1 (Hund, nicht Spur J). Orchestrator-Entscheid **J-E1** (master dba6ff8e, AUFTRAG.md
"2026-10-24 23:10"): V1 angenommen, M2 berichtigt diese Nachbesserung. Messungen der Abnahme werden hier NICHT wiederholt,
nur zitiert (Laeufe im Scratchpad `J41abn0/`).
Plan: V1 = Entscheid J-E1 dokumentieren (Haken bleiben unveraendert); M2 = Kopf `re15_superobj_r41.h` + §2.6/§4.1 mit der
Beleg-Kette (SCD-Bytes je Glied) berichtigen; H1 = Riegel 3060 nimmt die Tuer per Quadrat; H2 = Kopf der probes-Datei;
Bau + gezielte Riegel.
