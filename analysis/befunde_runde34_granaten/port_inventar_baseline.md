# Runde 34 (Granaten) — Port-Inventar + Ausgangsmessung (Baseline)

Stand: 2026-09-29, Repo-HEAD a358fd5d (master). RE-Phase: nur lesen, messen, dokumentieren.
Werkzeuge: `analysis/befunde_runde34_granaten/port_inventar_werkzeug/`,
Laufzeit-Ausgaben: `build/r34g_baseline/` (untracked).
Gemessene exe: `re15_port/build/platform/pc/re15_pc.exe` (6 379 410 Byte, 2026-09-29 19:46, NICHT neu gebaut).

Dieses Dossier ist das PORT-Inventar (Datei:Zeile) und die Ausgangsmessung. Es fuehrt KEINE neuen
Original-Konstanten ein; wo es Original-Adressen nennt, sind es die, die der Port-Quelltext oder die
Vor-Dossiers (`analysis/befunde_runde30/nachtrag-granate.md`, `analysis/waffen_fsm_2026-09-12/`)
schon zitieren — markiert als "(zit.)". Neu gemessen (Datei-Bytes, jeweils mit Datei-Offset) sind nur:
§1.1 PLW-Clips, §2.2 ESP-Zeilen der Granate und ihrer Kinder, §2.6 CLUT-Zeilen in TEX.TIM, §3.3 SE-Records,
§7.2 Anim-Records von Effekt 4; dazu zwei eigene Disasm-Auszuege (Routine B 29 @0x80018320, Haupttick
@0x80019e20), die nur die Port-LUECKE beschreiben, nicht den Bau.

## Gliederung
1. Code-Landkarte Waffe 9/10/11
2. ESP-VM des Ports
3. Audio
4. Gegner-Besitzer
5. Tests/Sonden + Muster
6. Mess-Harness
7. Ausgangsmessung 0x09/0x0A/0x0B in ROOM1140
- KONSTANTEN FUER DEN BAU (hier: Einhaengepunkte)
- PORT-ABGLEICH
- OFFEN

---

## 1. Code-Landkarte Waffe 9/10/11

### 1.1 Waffenbank W09/W0A/W0B (Ausruesten, Animation)

| Was | Datei:Zeile | Inhalt |
|---|---|---|
| PLW-Lader je Waffe (alle 21 Baenke, Mesh + EDD/EMR) | `platform/pc/main.c:3942-3981` | `RE15_WPN_MDL_MAX 21`; `PLD/<fam>W%02X.PLW`, dir[0]=EDD, dir[1]=EMR, dir[2]=MD1; `wpn_fam = pl_fam` (PL00 Leon / PL04 Elza, Bit 2 von charid, zit. base_table @0x800741e8) |
| Verbund-Skelett (PL00-Bindepose + W-Keyframes) | `platform/pc/main.c:4104-4131` | `wpn_skel[wi]`; Log `RE15_WPN_DBG` -> `wpnbank.log` |
| Equip-Watcher (Bank-Wechsel je Bild) | `platform/pc/main.c:8171-8220` | waehlt `wpn_anim[wid]`, fuettert `re15_player_set_aim_clip_lens`; Log `[equip] W-bank -> W%02X (Clips n, Rueckstoss-Clip7 fc=..)` |
| In-Hand-Mesh (Hand+Granate) | `platform/pc/main.c:8990-9010` | zeigt `wpn_md1[eq]` IMMER, solange ausgeruestet (keine Ausblendung nach dem Loslassen) |
| Waffen-Textur-Komposit (dir[3] in Slot 0) | `platform/pc/main.c:8951-8986` | `PLD/PL%02XW%02X.PLW` dir[3] -> `re15_render_pc_composite_slot0(..,200,480)` |

**Clip-Laengen (neu gemessen, `port_inventar_werkzeug/plw_clips.py`, Ausgabe `build/r34g_baseline/plw_clips.txt`),
EDD = PLW dir[0], Zaehlweise wie `engine/src/emd_common.c:48-99`:**

| Bank | Datei | sha1 (12) | Clips | Clip 6 Raise | Clip 7 Wurf MITTE | Clip 9 Wurf HOCH | Clip 11 Wurf TIEF | Holds 8/10/12 |
|---|---|---|---|---|---|---|---|---|
| PL00W09/0A/0B | 26 792 B, dir @0x6898, EDD 0x8..0x4EC | 1b5a63ce5b5c (alle drei gleich) | 13: `[22,16,52,1,50,30,10,35,1,40,1,40,1]` | 10 | **35** | **40** | **40** | 1/1/1 |
| PL04W09/0A/0B (Elza) | 28 132 B, dir @0x6DD4, EDD 0x8..0x608 | b034a12ca17e (alle drei gleich) | 14: `[22,16,52,1,105,30,10,35,1,40,1,40,1,15]` | 10 | 35 | 40 | 40 | 1/1/1 |

Keine Keyframe-Flag-Bits (`>>12`) in Clip 6..12. Folge: die Spawn-Bilder 19 (HOCH, Clip 9) / 22
(MITTE, Clip 7) / 24 (TIEF, Clip 11) (zit. @0x80033690/0x800336a4/0x80033758) liegen alle INNERHALB
ihrer Clips. Die drei Granatenbaenke sind byte-gleich -> eine Wurfanimation fuer alle drei Arten.

### 1.2 Spieler-FSM (Zielen, Wurf-Clip, Granaten-Bild)

| Was | Datei:Zeile | Inhalt |
|---|---|---|
| Aim-Phasen | `engine/src/player_common.c:144-149` | NONE/RAISE/READY/RELOAD/LOWER; `s_aim_recoil` = Wurf-/Rueckstoss-Clip laeuft |
| Clip-Laengen-Speicher | `player_common.c:160-161, 273-287` | `s_aim_clip_fcs[16]`, `aim_cur_fc()` |
| Raise-Eintritt, Klassen-Latch | `player_common.c:1031-1048` | `s_aim_melee = eq<3`, `s_aim_auto = 12/14/19`; Granate = Standard-Gun-FSM (Clip 6) |
| Raise -> Hold | `player_common.c:1050-1059` | Hold-Clip 8 |
| Zielhoehe (Hold) | `player_common.c:1060-1077` | `s_aim_elev` +1/0/-1 aus UP/DOWN; Hold 8/10/12 |
| Feuer-/Wurf-Start | `player_common.c:393-414` `re15_player_fire_start` | Clip `7+2*up+4*down` = 7/9/11, anim_frame 0, frac 7 |
| Wurf-Ende -> Hold | `player_common.c:1105-1122` | bei `anim_frame >= fc-1`: Hold-Clip 8/10/12 (Refire bei gehaltenem Quadrat) |
| R1-Loslassen: Recoil-Break | `player_common.c:940-975` | `recoil_break[16] = {0,0,7,7,10,...}` Index `eq-1` -> Waffe 9/10/11 = **10** (zit. byte2 @0x80074092+(w-1)*5); `anim_frame > 10` -> LOWER |
| Granaten-Bild-Export | `player_common.c:469-478` `re15_player_granate_frame()` | liefert `anim_frame` nur bei `!melee && recoil && !auto`, sonst -1 — **KEIN Waffen-Gate, KEIN Clip-Gate** (Gate `==9` sitzt im Aufrufer) |
| Zielhoehe-Export | `player_common.c:290` `re15_player_aim_elevation()` | -1/0/+1 |
| Aim-Reset/Interrupt | `player_common.c:295-316` | Raumwechsel/Treffer |
| Waffen-Log (je Bild) | `player_common.c:196-212` + `game_step_common.c:1678-1691` | `RE15_WAFFEN_LOG`: `F.. pad w clip fc frame ph rec auto takt mag fx` |

### 1.3 Feuer-Pfad, Entlade-Tabelle ENT, Granaten-Spawn

| Was | Datei:Zeile | Inhalt |
|---|---|---|
| R1-Block / Leer-Gate | `engine/src/game_step_common.c:1692-1713` | leeres Magazin + Quadrat-Flanke: Reload nur `eq<9` (zit. @0x80033368), sonst Klick `re15_audio_weapon_se(1)` — gilt auch fuer 9/10/11 |
| Abzug im Hold | `game_step_common.c:1714-1731` | `re15_player_fire_start()` (Quadrat GEHALTEN, zit. @0x80033308) |
| **ENT-Tabelle** | `game_step_common.c:1739-1804` | `re15_entlade_t {aktiv, resolve, ammo, n_fx, fx[3]}`; **`[9] = {1,1,1,0}`** (resolve=1 = PORT-BRUECKE, Kommentar 1769-1774), **`[10] = [11] = {1,0,1,0}`** (nur Munition, byte-true Stub @0x80033b58/78 zit.) |
| ENT-Auswertung | `game_step_common.c:1805-1850` | `resolve` -> `re15_player_weapon_fire(eq)` (Sofort-Hitscan beim ABZUG), dann `ammo`x `re15_ammo_consume()`, dann `n_fx` Spawns (fuer 9/10/11: keine) |
| **Granaten-Spawn** | `game_step_common.c:1942-1964` | nur `re15_player_equipped_weapon() == 9`; `gf==19&&elev>0` / `gf==22&&elev==0` / `gf==24&&elev<0` -> `re15_esp_fx_spawn_rows(global, 4, 0x0d, 0x1000, gunbone(0,0x12c,0x320 / 0,0,0x1f4 / 0,0,0x12c), floor_y = pl->y, param 0)` |
| Gun-Bone-Welt | `engine/src/re15_damage.c:1135-1159` `re15_player_gunbone_world` | `R*ofs + T` aus Render-Bone 11 (1 Bild alt) bzw. Engine-Pose-Rueckfall |
| Schrot-/Slash-/Dauerfeuer-Zweige (Muster) | `game_step_common.c:1880-1941, 1965-1973` | Pro-Bild-Hooks aus player_common (Vorlage fuer einen Wurf-Tick) |

**Befund am Code (Gate-Luecken, fuer den Bau wichtig):**
* Der Spawn ist in `game_step_common.c:1951` hart auf `== 9` gegatet (Spiegel @0x8003368c zit.);
  0x0A/0x0B spawnen nichts. Der Granaten-Bild-Export selbst prueft weder Waffe noch Clip.
* Der Spawn prueft den Clip nicht: Zielhoehe `ge` wird zum Spawn-Zeitpunkt gelesen, nicht beim
  Wurfstart. Im Wurf aendert der Port `s_aim_elev` nicht (nur im Hold, `player_common.c:1060`),
  also stimmt die Kopplung Clip<->Hoehe.
* Der Spawn laeuft NACH `re15_player_tick` im selben Bild (game_step), der ESP-Tick VOR game_step
  (§2.5) -> erster Tick des Granaten-Slots im Folgebild.

### 1.4 Schaden heute (Port-Bruecke)

| Was | Datei:Zeile | Inhalt |
|---|---|---|
| Hitscan-Resolver (FUN_80011f50-Pendant) | `engine/src/re15_damage.c:1922ff` `re15_player_weapon_fire` | naechster Gegner im Reach `s_player_wpn_reach[w] + hit_radius_min`, Band-/RE2-Applier-Gates |
| Reach-Tabelle | `re15_damage.c:1005-1008` | w9/w10/w11 = 1000 (zit. UNK_8006e5a0) |
| RE1.5-Schaden Zombies | `re15_damage.c:394-396` | w9 = 100, w10 = 200, w11 = 100 (zit. @0x8006e650) |
| RE2-Schaden Zombies (Default-Flavor) | `re15_damage.c:582-594` | w9 -> r9 = 200, w10 -> r11 = 200, w11 -> r10 = 200 (zit. @0x800A41CC/41F4/41E0); Typ 0x16: 80/200/80 (`:597-609`) |
| RE2-Applier fuer 9/10/11 AUS | `re15_damage.c:1674-1678, 1913-1920, 2302-2313` | NULL-Geometrie-Record (zit. @0x800A6350) -> Band-Gate-Bruecke bleibt |
| **Flaechen-Resolver (FUN_80012d60-Pendant)** | `re15_damage.c:3294-3355` `re15_resolve_attack(atk, attack_type, attacker_slot)` | testet ALLE aktiven Gegner (`re15_hitbox_test`) + IMMER den Spieler (`:3317`), Schaden `re15_damage_table[type]` |
| Schadenstabelle Angriffstyp | `re15_damage.c:41-53` | Typ 2 = **1000** (zit. DAT_8006f418); Reaktions-Code Typ 2 = 0x09 (`:56-58`, zit. DAT_8006f430) |
| Spieler-/Gegner-Zweig | `re15_damage.c:198-246` / `:2959-2992` | Spieler: Hit-Once-Latch `+0x93&1`, Zustand 2/3; Gegner: `+0x5 = react_table[type]`, `re15_re2_stamp_hit(e,1,type)` + `re15_re15_re2z_gore_hit` |
| Box-Test | `re15_damage.c:3243-3286` | `re15_hitbox_overlap`: R = Gegner-Radius + atk.radius, Hoehe `h = atk.radius + height` |
| Vorhandener Aufrufer | `re15_damage.c:3375-3385` `re15_enemy_attack` | `box.radius = 500; re15_resolve_attack(&box, 0, slot)` — **Muster fuer den Explosions-Aufruf** |

### 1.5 Munition / Inventar / Aufnahme

| Was | Datei:Zeile | Inhalt |
|---|---|---|
| Waffen-Props | `engine/src/inventory_common.c:183-202` | 9/10/11 = `{250, 0}` (zit. @0x80074da8, nur Byte-Wahrheit, kein Nachladen) |
| Magazin>0 / Abzug | `inventory_common.c:165-181` | `re15_ammo_mag_nonzero` (FUN_8004ea6c zit.), `re15_ammo_consume` (FUN_8004eae4 zit.) — Menge = qty des ausgeruesteten Platzes |
| Namen | `inventory_common.c:402` | "HAND GRENADE", "ACID GRENADE", "INCEND. GRENADE" |
| Einfuegen (1 Zelle) | `inventory_common.c:25-65` | 9/10/11 sind 1-Zellen-Items (Breit-Klasse erst 0x0e..0x13) |
| Equip-Commit Menue | `engine/src/menu_common.c:1405-1417, 2538-2546` | `re15_player_set_equipped_weapon(wid)` + `re15_audio_prime_weapon(wid)` |
| Aufnahme ROOM1150/1151 | `engine/src/granate_1150.c` / `include/re15_granate.h` | NUR Item 0x09 x1, Flag (9,56), obj 7, Sitz (-628,-145,478) rot 1024 (Port-Wahl, Runde 32) |
| Aufrufe der Aufnahme | `game_step_common.c:1046-1050` (tick), `scd_room_setup.c:419-423` (Tuer), `platform/pc/main.c:1402-1416` (MD1/TIM Slot 27), `main.c:4680-4691` (Boot/CONTINUE) | |
| **0x0A/0x0B-Quelle** | — | **keine** im Port (keine Platzierung, kein Pickup; Original-Zensus 0/0, `nachtrag-granate.md` §3). Nur ueber `RE15_GIVE` erreichbar. |

---

## 2. ESP-VM des Ports (`engine/src/re15_esp.c`, `include/re15_esp.h`)

### 2.1 Slot-Struktur `re15_esp_fx_t` (`include/re15_esp.h:150-202`), Pool 96 Plaetze (`:145`, zit. `sltiu 0x60` @0x8001978c)

| Port-Feld | Original-Slot (Stride 0x84, zit.) | Bemerkung fuer die Granate |
|---|---|---|
| `active` | = Flags-Bit0 (`esp_fx_kill` `re15_esp.c:517-521`) | Belegung getrennt gefuehrt, im Original dasselbe Byte +0x6c |
| `effect_id`, `sub_index`, `eff_idx`, `bank` | aus a0 von FUN_80019700 (zit.) | Bank-Aufloesung: Raum zuerst, dann CORE00 (`:335-341`) |
| `frame`, `timer` | +0x6e Anim-Index, +0x6d Timer | Routine 30 setzt +0x6e := 0x17 (zit.) -> `frame` |
| `x,y,z` (int32) | Anker (Spawn-Position) | |
| `xlat_x/y/z` (int32) | +0x34/+0x38/+0x3c (s32) | Welt = Anker + xlat (Draw `main.c:296-298`) |
| `drift_x/y/z` (int16) | +0x10/+0x12/+0x14 Geschwindigkeit | Routine 30 setzt die Wurfgeschwindigkeit hier (zit.) |
| `accel_x/y/z` | +0x08/+0x0a/+0x0c | Row 0 der Granate: (0,10,0) |
| `row[40]` | +0x00..+0x27 (Identitaetskopie) | enthaelt A/B-Selektor, +0x1e (Zuender), +0x26 (Abprall-Zaehler) |
| `flags` | +0x6c | |
| `row_cursor`, `rows_base`, `row_count` | +0x6f + Stream-Liste | |
| `param` (int16) | +0x2e | Spawn-Parameter a1 |
| `scale16` | +0x72 | Granate 0x1000 |
| `tpage`, `clut` | +0x30 / +0x32 | **`clut` wird vom PC-Draw NICHT gelesen** (§2.6) |
| `phys`, `floor_y` | (Port-Konstrukt) | `floor_y` = Ebene der zusammengefassten Bodenklemme (§2.4) |
| `follow_slot` | +0x74 Eltern-Matrix (Flags-Bit 0x04) | |
| **fehlt** | **+0x28/+0x2a/+0x2c (s16, Welt-Position)** | vom Original-Haupttick geschrieben (`sh` @0x8001a0e8/@0x8001a100, @0x8001a1fc/@0x8001a210/@0x8001a220, @0x8001a270/@0x8001a28c/@0x8001a2a4; eigene Disasm `build/r34g_baseline/dis_80019e20.txt`) und von Routine 29 gelesen (`lh t1,42(t0)` @0x80018330, `blez t1` @0x80018338; `lw v0,56(t0)` / `subu v0,v0,t1` / `sw v0,56(t0)` @0x800183c4-e0). Die Semantik von +0x2a in Routine 29 gehoert der Flug-RE-Spur (OFFEN O3). |
| **fehlt** | Matrix-/Offset-Felder des Haupttick (+0x20.., +0x40..+0x4b, +0x60..+0x6b) | Welt-Positions-Rechnung @0x8001a014-0x8001a2a4 (Disasm-Datei oben); der Port rechnet nur Anker+xlat |

### 2.2 Zeilen laden / fortschalten

| Was | Datei:Zeile |
|---|---|
| Row-Block-Zugriff (`sub & 7`, Streams, 40-Byte-Zeilen) | `re15_esp.c:170-236` |
| Header-Seed CLUT/TPAGE (`hdr_clut + ((sub&0xff)>>3)*0x40`) | `re15_esp.c:749-758` |
| Zeile kopieren (accel/vel neu seeden) | `re15_esp.c:475-486` `esp_fx_row_load` |
| Fortschalten (FUN_800174e4-Pendant) | `re15_esp.c:490-496` `esp_fx_row_advance` (haelt an der letzten Zeile) |

**Granaten-Zeilen (neu gelesen, `port_inventar_werkzeug/esp_rows.py`, Ausgabe `build/r34g_baseline/esp_rows.txt`, Parse wie `re15_esp.c`):**

| Effekt/sub | Row-sub (`&7`) | CLUT-Seed | Streams | Zeile 0 (Datei-Offset CORE00.ESP) | Zeile 1 |
|---|---|---|---|---|---|
| 4 / 0x0D (Granate) | 5 | 0x7B11 | 1 | @0x01AB8 A=**30** B=0 wh=(4096,4096) acc=(0,10,0) sonst 0 | @0x01AE0 A=0 B=0 alles 0 |
| 3 / 0x19 (Kind 0x03195000) | 1 | 0x78D1 | 1 | @0x003F4 A=**10** p0e=0x0013 p26=0x000a | @0x0041C A=0 |
| 3 / 0x0B (Kind 0x030B5400/5800) | 3 | 0x7851 | 1 | @0x004CC A=**10** acc=(0,5,0) p0e=0x0013 vel=(0,-130,0) p16=0x0040 p26=0x0008 | @0x004F4 A=0 acc=(0,5,0) vel=(0,-135,0) |

Folge: Die Kind-Effekte laufen ueber Routine 10 — die IST im Port (`re15_esp.c:652-665`). Nur die
Granate selbst (A 30 -> B 29 -> A 31) fehlt.

### 2.3 Dispatch A / B (welche Routinen es gibt)

| Loop | Datei:Zeile | Implementiert |
|---|---|---|
| Routine A (Loop 1, zit. @0x80019e84-9c) | `re15_esp.c:523-712` `esp_fx_dispatch` | 0, 3, 4, 5, 8, 9, 10, 11, 15, 16, 17, 18, 38 — `default:` = Noop (`:710`) |
| Routine B (Hauptloop, zit. @0x8001a2b4-d4) | `re15_esp.c:724-736` `esp_fx_dispatch_b` | NUR 12 (Boden-Abpraller, auf `floor_y` zusammengefasst) |
| **Fehlt fuer die Granate** | | **A 30** (Wurf-Init @0x8001843c zit.), **A 31** (Explosion @0x8001854c zit.), **B 29** (Flug/Abprall @0x80018320 zit.) |

Eigene Gegenprobe B 29 @0x80018320 (`re15_disasm.py dis 0x80018320 72`, nur zur Beschreibung der Port-Luecke):
Slot-Zeiger `lw t0,21188(t0)` @0x80018324 (= 0x800b52c4); SE `lui a0,0x10a / ori a0,a0,0x1 / jal 0x80045024`
@0x80018350-58 (Zaehler 0) bzw. `lhu a0,38(t0) / sll a0,a0,8 / or a0,a0,v1 / jal 0x80045024` @0x80018418-24
mit `v1 = 0x010a0001` (@0x80018410/1c) und `a1 = sp+16` = {+0x28,+0x2a,+0x2c} (@0x800183fc-14) — also ein
POSITIONALER SE mit dem Abprall-Zaehler +0x26 in Byte 1. Flags 0x63 @0x80018368-6c, Selektor A := 0x1f (31)
@0x80018378-7c, B := 0 (Delay-Slot @0x80018384).

**So haengt man eine neue Routine ein:** `case 30:` / `case 31:` in `esp_fx_dispatch` (`re15_esp.c:527`),
`case 29:` in `esp_fx_dispatch_b` (heute Frueh-Return bei `!= 12`, `:727`). Zugriff auf die Row-Kopie ueber
`row_u16(f->row, off)` / direkte Bytes, auf Geschwindigkeit ueber `drift_*`, auf die Slot-Freigabe ueber
`esp_fx_kill` (`:517`). Waffen-Abfragen aus dem Slot heraus: Muster Routine 38 (`:588-609`, ruft
`re15_player_equipped_weapon()` extern). Routine 30 braucht zusaetzlich die Zielhoehe
(`re15_player_aim_elevation()`, `player_common.c:290`; im Original 0x800acaec, zit.).

### 2.4 Physik / Boden im Port (zusammengefasst)

`re15_esp.c:903-922`: fuer JEDEN Slot mit `phys` (alle Row-VM-Slots, `:784`) laeuft
`xlat += drift; drift += accel` und danach eine **Bodenklemme auf `floor_y` mit 50 % Rueckprall und 3/4
Reibung** — unabhaengig von Routine B. Das ist eine Port-Zusammenfassung (Kommentar `:914`), NICHT das
Original. Fuer die Granate heisst das heute: sie faellt mit (0,10,0) auf `floor_y = pl->y`
(Spawn `game_step_common.c:1960`) und federt dort nach Port-Regel. Beim Bau von B 29 muss diese
Sammelklemme fuer die Granaten-Slots aus dem Weg (sonst doppelte Bodenbehandlung).

### 2.5 Tick-Aufruf je Bild

| Was | Datei:Zeile | Rate / Reihenfolge |
|---|---|---|
| `re15_esp_fx_tick(re15_esp_room_bank())` | `platform/pc/main.c:5413` | im 30-Hz-Zweig (`target_fps == 30 || frame gerade`, `:5377`), NACH `scd_vm_tick()` (`:5378`), VOR `re15_game_step` (`main.c:7358`) |
| Pause-Gate im Tick selbst | `re15_esp.c:868` | `g_re15_pauseflags & RE15_PAUSE_ACTION` (zit. @0x80019e28-40) |
| eingefrorene Zweige (kein Tick) | `main.c:5337-5376` | Discard-Abfrage, Item-Modal, Statusschirm |
| Pool leeren | `platform/pc/src/room_pc.c:130`, `engine/src/scd_room_setup.c:199` | Raumwechsel/Raumstart |

Folge: Spawn im Bild N (game_step, nach dem Spieler-Tick) -> erste Routine-30-Ausfuehrung im Bild N+1.

**Original-Reihenfolge (eigene Disasm Hauptloop, `re15_disasm.py dis 0x8001cdd0 40`):**
`8001cdec jal 0x8003f038` (SCD) -> `8001cdf4 jal 0x8004f090` -> `8001cdfc jal 0x8001500c` ->
`8001ce04 jal 0x8001a50c` -> **`8001ce0c jal 0x80031c44` (Spieler-Dispatcher)** -> `8001ce14 jal 0x8002bd44`
(Objekte) -> `8001ce1c jal 0x800436a8` -> `8001ce24 jal 0x8004f0b0` -> **`8001ce2c jal 0x80019e20`
(Effekt-Tick)** -> `8001ce34 jal 0x8001db28` (Item-Modal). Im Original laeuft der Effekt-Tick also NACH dem
Spieler im SELBEN Bild: ein vom Spieler-FSM gespawnter Slot (Granate, Muendung, Huelse) bekommt seinen
ersten Routinen-Tick noch im Spawn-Bild. Der Port tickt die Effekte VOR `re15_game_step` -> **1 Bild
spaeter** (gilt fuer alle Spieler-Effekte, nicht nur die Granate).

### 2.6 Render der ESP-Sprites

| Was | Datei:Zeile |
|---|---|
| Draw-Schleife | `platform/pc/main.c:236-452` `pc_draw_effects` (Sichtbar-Bit, Region-Cull `re15_esp_fx_culled` `include/re15_aot.h:450`, GTE-Projektion, Sprite-Aufbau nach FUN_800534c4 zit.) |
| Textur-Slots Global-Bank | `main.c:120-146` (`k_global_fx_slot`: id0->20, id2->21, id3->22, id4->23, id8->44); Lader `main.c:3714-3736` (`extracted_fx/effect*.tim`) |
| Groesse | `step16 = S*scale16*camf/(sz<<4)`, defW/defH nur fuer Routinen 17/18 live (`main.c:360-380`) |
| Blend | ABE = Flags-Bit4, ABR aus `tpage` Bits 5-6 (`main.c:394-434`) |

**Luecke Palette:** `f->clut` wird im Engine-Teil gesetzt (`re15_esp.c:661, 756`), aber vom PC-Draw nie
gelesen (`main.c:445-448` uebergibt clut 0; Grep ueber `platform/pc`: kein Leser). Die Global-Sheets 2/3/4
sind 16-bpp-Standbilder EINER Palette (`shared_assets/extracted_fx/effect3_smoke.tim`, `effect4_shell.tim`,
je 131 092 B). Granate (Effekt 4, CLUT 0x7B11) und Explosions-Kinder (Effekt 3, CLUT 0x78D1 / 0x7851) haetten
im Port also die FALSCHE Farbe. Die Quellen liegen byte-true in `DATA/TEX.TIM` (neu gemessen,
`port_inventar_werkzeug/tex_clut_rows.py`, `build/r34g_baseline/tex_clut_rows.txt`; CLUT-Block VRAM(256,480) 32x24 @Datei 0x8):

| CLUT | VRAM | TEX.TIM Datei-Offset | erste Eintraege |
|---|---|---|---|
| 0x7851 (Eff. 3 sub 0x0B) | (272,481) | 0x00074 | `0000 9ce7 98c6 94a5 ...` |
| 0x78D1 (Eff. 3 sub 0x19) | (272,483) | 0x000F4 | `0000 ffff ebde d7de bf1c ...` |
| 0x7B11 (Eff. 4 sub 0x0D) | (272,492) | 0x00334 | `0000 b631 b1ef adce ...` |

Seiten: Effekt 3 = tpage 0x001e -> VRAM(896,256), Effekt 4 = tpage 0x001f -> VRAM(960,256) (EFF-Header,
`esp_rows.py`). Das 4-bpp-Schneiden aus TEX.TIM ist fuer Effekt 8 schon vorgemacht
(`tools/tex_tim_effect_slice.py`, Kommentar `main.c:3694-3713`).

### 2.7 Kind-Spawn-API und Hooks

| API | Datei:Zeile | Bemerkung |
|---|---|---|
| `re15_esp_fx_spawn_rows(bank,id,sub,scale16,x,y,z,floor_y,param)` | `re15_esp.c:763-794` | ein Slot je Stream, Row-VM an; Log-Zeile `SPAWN id= sub= scale= streams=` in `RE15_WAFFEN_LOG` (`:772-777`) |
| `re15_esp_fx_spawn_ex` | `re15_esp.c:323-350` | Legacy ohne Row-VM |
| Kind aus Routine 8/15 | `re15_esp.c:641-642, 582-583` | **Position = Anker `f->x/y/z`, NICHT Anker+xlat** — fuer einen fliegenden Slot (Granate) waere das die Wurf-Stelle, nicht der Ort des Slots. Der Kind-Spawner der Routine 31 (FUN_800199d4, zit.) nimmt die Slot-Position; Port-Aequivalent `x+xlat_x` usw. (Semantik: RE-Spur Explosion) |
| `re15_esp_bang_hook` | `re15_esp.c:716`, gesetzt `platform/pc/src/audio_pc.c:1683` | -> `re15_audio_weapon_se(0)` (Routine 9) |
| `re15_esp_shell_clink_hook` | `re15_esp.c:740`, gesetzt `audio_pc.c:1682` | -> `re15_audio_weapon_se(2)` (Routine B 12) |
| Latch 0x800b5358 ("Laerm-Latch" in Runde 30 / Port-Kommentar) | — | im Port NICHT gefuehrt (Kommentar `game_step_common.c:1867`: "consumer un-RE'd"). Eigener Xref-Scan (`port_inventar_werkzeug/latch_xref.py`, `build/r34g_baseline/latch_xref.txt`, PSX.EXE + STAGE1..6.BIN): Schreiber NUR `sb v0(=1)` @0x80017694 (Routine 9), @0x800180f0 (Routine 26 @0x800180b0), @0x8001857c (Routine 31); EINZIGER Leser `lbu v0,21336(v0)` @0x8001ce60 im Hauptloop direkt nach dem Item-Modal. Der Leser schreibt die LICHT-Zeile der aktiven Kamera (RDT+0x2C lightStart, Stride 40 je `0x800b0fe4`): `sb zero,3` @0x8001cef8, Byte +10 >= 0xd2 @0x8001cf28-34, +11 >= 0x8c @0x8001cf64-70, +12 >= 0x50 @0x8001cfa0-ac, Position +0x1c/+0x1e/+0x20 = Spieler + gedrehter Vektor (0x4b0 @0x8001cebc, Yaw 0x800acabe, `jal 0x8004f008` @0x8001cecc) bzw. Spieler-Y -800 @0x8001d018, +0x26 := 0x1770 @0x8001d080-84; davor Sicherung per `jal 0x8004ee38` (memcpy, 0x28 Byte) @0x8001cea0 (`dis_8001ce5c.txt`). Es ist also ein LICHTBLITZ (Muendungslicht), kein Laerm. Im Port gibt es keinen Leser. Wo der Latch geloescht wird: per Offset-Scan nicht gefunden (OFFEN O9) |
| Pool-Zaehler fuer Messung | `re15_esp_fx_count()` `re15_esp.c:303-308` | im Waffen-/State-Log als `fx=` |

---

## 3. Audio

### 3.1 Wie der Port SE-Ids der Form `0xBBRRxxFF` spielt (FUN_80045024-Pendant)

Es gibt im Port **keine** Funktion, die das gepackte 32-bit-Wort nimmt. Der Aufrufer zerlegt selbst und ruft
die Bank-Funktion:

| Byte 3 (Bank, zit. `bank = arg>>24` @0x80045028) | Port-Weg | Datei:Zeile |
|---|---|---|
| 1 = ARMS (ausgeruestete Waffe) | `re15_audio_weapon_se(record)` | `platform/pc/src/audio_pc.c:1052-1064` |
| 2 und 5 = RDT snd0 | `re15_audio_room_se_snd0(record)` | `audio_pc.c:845-849` |
| 3 = RDT snd1 (FUN_800453d0) | `re15_audio_room_se(record)` (`record < 0x19`) | `audio_pc.c:837-841` |
| 4 = CORE (resident) | `re15_audio_core_se(record)` | `audio_pc.c:976-982` |
| 0 / >=6 | verworfen | Abbildung `include/re15_audio.h:132-163` `re15_audio_se_bank_kind` |

* Byte 2 = Record-Index (EDT, 4 Byte je Record, `re15_edt_decode` `engine/src/vab_common.c:252-265`).
* **Byte 1 wird nirgends ausgewertet** (die Bank-Funktionen nehmen nur den Record). Routine 29 legt dort den
  Abprall-Zaehler +0x26 ab (§2.3) — Bedeutung in FUN_80045024 = OFFEN O5 (RE-Spur Audio).
* Byte 0 != 0 = positionaler Zweig (FUN_80045a64 zit.) — im Port NICHT portiert, er nimmt immer den
  Ton-eigenen vol/pan (Kommentar `audio_pc.c:668-680, 1047-1051`).
* Stimmen-/Prioritaets-Maschine: `se_play_layers` `audio_pc.c:682-782`, Pumpe `se_voice_pump` `:794-812`
  (FUN_800458d4 zit.).
* SCD-`Se_on` laeuft ueber die Warteschlange `re15_audio_tick` `audio_pc.c:3450-3475` (dieselbe Bank-Abbildung).

### 3.2 Baenke laden

| Bank | Lader | Wer laedt wann |
|---|---|---|
| ARMS (Bank 1) | `load_weapon_se_vab_pc(id)` `audio_pc.c:856-898`, `SOUND/ARMS%02X.EDH/.VB` | Audio-Init laedt **ARMS01** (`audio_pc.c:1767`); danach NUR `re15_audio_prime_weapon` aus dem Menue-Equip (`engine/src/menu_common.c:1416, 2545`) und nach LOAD (`platform/pc/main.c:4523-4526`) |
| CORE (Bank 4) | `load_core_se_vab_pc(idx)` `audio_pc.c:913-962` | Titel 0x11, Spielstart `g_gameflow.character` (`main.c:3286, 3487-3511`); Default CORE00 bei erstem `re15_audio_core_se` |
| snd0/snd1 | `re15_audio_load_room_banks` `audio_pc.c:3565-3603` | je Raum |

**Mess-Harness-Falle:** `RE15_EQUIP` setzt nur die Waffen-Id (`main.c:4244-4252`) und ruft
`re15_audio_prime_weapon` NICHT — die ARMS-Bank bleibt ARMS01. ARMS01 hat nur 10 Records (s. 3.3), Record 0x0A
(Granaten-Abprall) waere dort AUSSERHALB -> stumm verworfen (`audio_pc.c:1060`). Ein SE-Messlauf fuer die
Granate braucht Equip ueber das Menue oder einen Harness-Zusatz, der ARMS09 laedt.

### 3.3 SE-Records der Granate in den ausgelieferten Baenken (neu gemessen)

`port_inventar_werkzeug/se_records.py`, Ausgabe `build/r34g_baseline/se_records.txt`; Zerlegung wie
`re15_edt_decode`:

| SE (zit. Runde 30) | Bank-Datei | Records | Record @EDH-Offset | Bytes | Zerlegung |
|---|---|---|---|---|---|
| 0x010A0001 (+Zaehler<<8) Abprall/Aufschlag | `SOUND/ARMS09.EDH` (3156 B, pBAV @0x2C) | 11 | 0x0A @0x028 | `00 00 13 10` | prog 0, Ton 1, prio 3, Stimme 0, keine Layer |
| — gleich — | `ARMS0A.EDH`, `ARMS0B.EDH` | 11 | 0x0A @0x028 | `00 00 13 10` | **ARMS09 = ARMS0A = ARMS0B byte-gleich (EDH UND VB)** |
| — Gegenprobe — | `ARMS01.EDH` | **10** | 0x0A | — | ausserhalb (Default-Bank des Ports) |
| 0x04080001 Explosion | `SOUND/CORE00.EDH` (3176 B, pBAV @0x40) | 16 | 0x08 @0x020 | `00 00 93 00` | Ton 9, prio 3, **Stimme -16 = Direkt-Zweig** (sofortiges Key-On ohne Gate, `audio_pc.c:748-762`) |
| — Elza — | `CORE04.EDH` | 16 | 0x08 @0x020 | `00 00 93 00` | gleich |

ARMS09 fuehrt 9 nicht-leere Records: 0x00 `00 00 13 30` (Knall mit 1 Layer), 0x01 `00 00 33 10`,
0x02 `00 00 42 10`, 0x03/0x04 `00 00 53 10`, 0x05 `00 00 63 11`, 0x06 `00 00 73 11`, 0x07 `00 00 93 14`,
0x0A `00 00 13 10`.

### 3.4 Vorhandene Waffen-SE-Hooks

| Hook | Datei:Zeile | SE |
|---|---|---|
| Knall (Routine 9) | `re15_esp.c:646-651` -> `pc_bang` `audio_pc.c:1678` | ARMS 0 |
| Huelsen-Klick (B 12) | `re15_esp.c:735` -> `pc_shell_clink` `audio_pc.c:1677` | ARMS 2 |
| Leerklick | `game_step_common.c:1706` | ARMS 1 (auch fuer 9/10/11 bei leerem Magazin + Quadrat-Flanke) |
| Nachladen-Ende | `player_common.c:1146` | ARMS 3 (fuer 9..11 unerreichbar, Gate `eq<9`) |
| Messer-Schwung / -Ziehen | `player_common.c:412, 1044` | ARMS 5 / 8 |
| Gegner-Treffer-SE (Spieler) | `re15_damage.c:219-221` | Raum-SE 10 (nur Angriffstyp < 2) |
| Mess-Log jeder Waffen-SE | `audio_pc.c:1054-1058` | Zeile `SE arms_rec=.. bank=..(geladen=..) count=..` in `RE15_WAFFEN_LOG` — wird auch mit `RE15_NOAUDIO` geschrieben |

---

## 4. Gegner-Besitzer (wer reagiert auf die Granate)

### 4.1 Flavor und RE2-Besitz

| Was | Datei:Zeile | Inhalt |
|---|---|---|
| Flavor-Default | `engine/src/enemy_ai_re2_zombie.c:101` | **RE2** (`s_flavor = RE15_AI_FLAVOR_RE2`) |
| Env-Schalter | `enemy_ai_re2_zombie.c:104-138` | `RE15_AI_FLAVOR` = `re15`/`0`/`1.5` -> RE1.5, `2`/`re2` -> RE2, `m`/`mixed` -> MIXED; Menue `platform/pc/main.c:2404-2410` (nicht persistent) |
| Typ-Frage | `enemy_ai_re2_zombie.c:153-160` `re15_ai_re2_for_type` | RE2 -> 1; MIXED -> nur 0x20; RE1.5 -> 0 |
| RE2-Zombie-Familie | `enemy_ai_re2_zombie.c:186-190` `re15_re2z_owns_type` | 0x10, 0x11, 0x12, 0x13, 0x16, 0x18 |
| Voller RE2-Besitz | `engine/src/enemy_ai_re2_dog.c:75-90` `re15_re2_owns_type` | Zombies + 0x20 Hund, 0x21 Kraehe, 0x25/0x26 Spinnen |
| RE2-Import im RE1.5-Modus | `include/re15_ai_flavor.h:77-97` | Zerleger + Schadens-/HP-Modell fuer Zombies, Default AN (`RE15_RE15_RE2Z_IMPORT=0` aus) |
| Modell-Herkunft | `platform/pc/main.c:918-1187` `pc_enemy_load_ex` | RE2-Flavor + RE2-Besitz -> RE2-Bank (`pc_enemy_load_re2` `:730`) + Hybrid-Rig mit RE1.5-Mesh (`pc_enemy_hybrid_re15_models` `:759`, Aufruf `:1079`); sonst RE1.5-EMD |

### 4.2 Gegner-Tick-Module (Dispatch `engine/src/enemy_ai_common.c:14169-14551` `re15_enemy_ai_run_all`)

| Typ | Zweig (Zeile) | Modul / Tick |
|---|---|---|
| 0x10/0x11/0x12/0x16/0x18 Zombies | `:14222` | `re15_enemy_ai_live_step` -> bei RE2 `re15_re2z_tick` (`enemy_ai_common.c:5766`, `enemy_ai_re2_zombie.c:8299`) |
| 0x13 Zombie-Girl | `:14449` | `re15_zgirl_ai_tick` (`:10663`, RE2 -> `re15_re2z_tick` `:10681`) |
| 0x20 Hund | `:14307` | `re15_dog_ai_tick` (`:7670`, RE2 -> `re15_re2dog_tick` `enemy_ai_re2_dog.c:2372`) |
| 0x21 Kraehe | `:14283` | `re15_crow_ai_tick` (`:6757`, RE2 -> `re15_re2crow_tick` `enemy_ai_re2_crow.c:1796`) |
| 0x25 Spinne / 0x26 Baby | `:14351` / `:14347` | `re15_adult_spider_ai_tick` / `re15_spider_ai_tick` (`:8443`, RE2 `enemy_ai_re2_spider.c:3036/2911`) |
| 0x27 Gorilla/Maggot | `:14398` | `re15_maggot_ai_tick` (`:8815`) |
| 0x29 Kakerlake | `:14371` | `re15_cockroach_ai_tick` |
| 0x1A Writher / RE2-Zellenarm | `:14485` | `re15_writher_ai_tick` bzw. `re15_re2arm_tick` (`enemy_ai_re2_zellenarm.c`) |
| 0x23 Alligator | `:14501` | `re15_alligator_ai_tick` / Bosskampf `enemy_ai_boss_gator.c` |
| 0x2B Tyrant | `:14527` | `re15_tyrant_ai_tick` |
| 0x2D Efeu | `:14538` | `re15_ivy_ai_tick` |
| 0x24 FX-Emitter | `:14522` | `re15_fx_emitter_ai_tick` |
| 0x30/0x36 Birkin | `:14390` | `re15_birkin_root` (`:14103`); 0x36 in ROOM5090/5091 `re15_g5_boss_tick` (`enemy_ai_boss_g5.c`, `:14383`) |
| NPC 0x40/0x42/0x45/0x47/0x49/0x4B/0x4D | `:14433` | `re15_npc_ai_tick` (unverwundbar) |

### 4.3 Wie ein Treffer heute beim Gegner ankommt (fuer Granaten-Ids)

| Pfad | Datei:Zeile | Was er fuer 9/10/11 tut |
|---|---|---|
| Hitscan `re15_player_weapon_fire(w)` | `re15_damage.c:1922ff` | `+0x5 = weapon_id` (zit. @0x800124bc) -> RE2-Stempel `re15_re2_stamp_hit(e,0,w)` (`re15_damage.c:2733`) -> RE2-Reaktionszeile `re2z_row_from_weapon`: **w9 -> 9, w10 -> 11, w11 -> 10** (`enemy_ai_re2_zombie.c:3829-3834`, Port-Zuordnung mit Beleg-Kommentar `:3784-3790`) = STAGGER-Klasse |
| RE2-Schaden Zombie | `re15_damage.c:582-594` | 200 / 200 / 200 (Typ 0x16: 80 / 200 / 80) |
| RE1.5-Schaden Zombie | `re15_damage.c:394-396` | 100 / 200 / 100 |
| Flaechen-Resolver `re15_resolve_attack(atk, type, slot)` | `re15_damage.c:3294-3355` | heute NUR vom Gegner-Biss gerufen (`:3375-3385`, Typ 0); Gegner-Zweig stempelt `re15_re2_stamp_hit(e, 1, type)` (`:2981`) und `re15_re2z_row_for_atktype`: Typ 2 -> RE2-Zeile **17** (`enemy_ai_re2_zombie.c:3847`, Port-Zuordnung "Instakill-Klasse") |
| RE2-Trefferpause/Stun | `re15_damage.c:552-579` | `s_re2_stun_*[w]` je Typ |

Folge fuer den Bau: Geht der Explosionsschaden ueber `re15_resolve_attack(...,2,...)` (Spiegel FUN_80012d60 zit.),
reagieren RE2-Zombies heute ueber Zeile 17 (Port-Zuordnung fuer Angriffstyp 2), NICHT ueber die
Granaten-Zeilen 9/10/11. Welche RE2-Zeile eine RE2-Explosion stempelt, gehoert der RE-Spur Gegnerreaktion
(RE2-AoE-Zwilling FUN_80047664 wird im Port-Kommentar `re15_damage.c:1176-1177` genannt).

### 4.4 Vorhandene RE2-Reaktions-Bausteine fuer die drei Granatenzeilen (Zombie, `enemy_ai_re2_zombie.c`)

Die Reaktion AM Gegner ist im RE2-Zombie-Gehirn schon gebaut und haengt nur an der Zeile +0x5:

| RE2-Zeile (Port-Quelle) | Reaktion im Port | Datei:Zeile (zit. Original-Adresse im Kommentar) |
|---|---|---|
| 9 GL Explosiv (w9) | Tod -> `re2z_death_rip` (Zerreissen), Russ `re2z_gore_soot` | Tabelle `:3885` `{5,2,2, 1,1,1, 1,1,1}`; Tod-Routing `:5338` (0x80108BEC); Russ `:4723`, Aufruf `:5722` (0x8010640C) |
| 10 GL Brand (w11) | Verkohlung `re2z_gore_burn` | Tabelle `:3886`; `:4669`, Aufrufe `:4776, 4968, 5204, 6808, 6815, 6927, 6933` (0x80106128) |
| 11 GL Saeure (w10) | Aetzung `re2z_gore_acid`, Unterschenkel-Wegaetzen `re2z_stagger_acid_leg` | Tabelle `:3887`; `:4706`, `:4896`, Aufrufe `:4973, 6811, 6942` (0x80106310) |
| Zuordnung w->Zeile | `re2z_row_from_weapon[22]` | `:3829-3834` (w9->9, w10->11, w11->10) |

Gemessen (§7, NAH-Lauf): Waffe 9 trifft Zombie 2 in d=1299 per Bruecke IM ABZUGSBILD -> st 1->3, ss1=9,
weggeschleudert (d 1299 -> 2792 in 5 Bildern, Endlage 3093 ab F836), Blut (Effekt 0), Raum-Effekt 7 sub 2 und
Feuer (Effekt 8 sub 3) im Abzugsbild, ab Abzug+45 Leiche (st 7).
Waffen 10/11 erreichen diese Bausteine heute nie (ENT `resolve=0`, kein Spawn).

---

## 5. Tests / Sonden, die heute Granaten beruehren, und das Muster fuer neue

### 5.1 Bestand

| Test / Sonde | Registrierung | Was er an 9/10/11 festhaelt |
|---|---|---|
| `unit_r30_granate` (`tests/unit/probe_r30_granate.c`) | `tests/unit/probes/r30_granate.cmake` | NUR Aufnahme: Prop-Modell aus PL00W09.PLW, Sitz im Fach, Modal-Reihenfolge, Flag (9,56) — 14 Pruefungen, Rueckgabe = Nummer der ersten gerissenen. **Nichts zum Wurf.** |
| `probe_r30_granate_karte` (Werkzeug, kein Test) | ebd. | schreibt Speicherkarten fuer den Lade-Riegel |
| `integration_r30_granate_laden` (`tests/integration/test_r30_granate_laden.cmake`) | ebd., nur `if(TARGET re15_pc)` | echte exe: Lade-Weg 1150/1151, Negativ-Kontrolle, CHECK-Foto 0x09 |
| `unit_r32_hebetisch_faecher`, `unit_r31_hebetisch`, `unit_r30_irons_tisch` | `probes/r32_hebetisch_faecher.cmake`, `probes/r31_hebetisch.cmake`, `unit/CMakeLists.txt` | Granaten-Prop im Pool/Fach (obj 7) |
| `unit_re2_weapon_rows` (`tests/unit/test_re2_weapon_rows.c`) | `unit/CMakeLists.txt:1509` | Namensliste inkl. "Hand/Acid/Incend. Grenade" (`:88-89`); RE2-Zeilen je Waffe |
| `unit_re2_hp_model`, `unit_re2_zombie_teardeath`, `test_re2_hit_reaction`, `test_re15_re2z_import` | `unit/CMakeLists.txt:1705, 2294` u.a. | Schadens-/Reaktionszeilen fuer alle 22 Waffen-Ids (darin w9..w11 und die Werfer-Bruecken 15..18/20, Kommentar `game_step_common.c:1787-1803`) |

Es gibt **keinen** Test zum Wurf (Clip 7/9/11, Spawn-Bild 19/22/24, Munitionsabzug), zur ENT-Bruecke
`[9].resolve` oder zu `re15_player_granate_frame`. Eine Aenderung von `ENT[9]` bricht also keinen Test —
ausser indirekt Pins, die ueber den Bruecken-Resolve eine Reaktionszeile fuer Waffe 9 erwarten (vor dem Bau
einzeln pruefen: Grep `weapon_fire\(` in den oben genannten Dateien).

### 5.2 Muster: neue Unit-Sonde (Memory reai-v2-sonden-je-thema)

1. Quelle `re15_port/tests/unit/probe_<thema>.c` (Rueckgabe 0 = gruen, sonst Nummer der Pruefung).
2. Registrierung in EIGENER Datei `re15_port/tests/unit/probes/<thema>.cmake` (GLOB-Include am Ende von
   `tests/unit/CMakeLists.txt:4060-4063`), Vorlage `probes/r30_granate.cmake`:
   `add_executable(probe_x ${CMAKE_CURRENT_LIST_DIR}/../probe_x.c)`,
   `target_link_libraries(probe_x PRIVATE re15_engine re15_test_support)`,
   `target_include_directories(probe_x PRIVATE ${CMAKE_SOURCE_DIR}/include)`,
   bei Asset-Bedarf `target_compile_definitions(... RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")`,
   unter Linux `m` linken, dann `add_test(NAME unit_x COMMAND probe_x)` + `TIMEOUT`.
3. Eigenes Bauverzeichnis je Agent `re15_port/build_<thema>` mit
   `-DFETCHCONTENT_SOURCE_DIR_SDL2=<repo>/re15_port/build/_deps/sdl2-src`, `--target probe_x`
   (Aus Git-Bash: PATH mit msys64 zuerst, Memory reai-v2-build-path-shadowing / `tools/local_build.sh`).
4. **Test-Harnisch-Fallen:**
   * `tests/test_support.c:145-148` zwingt per Konstruktor den **RE1.5-Flavor** — eine Sonde fuer RE2-Gegner
     muss `re15_ai_flavor_set(RE15_AI_FLAVOR_RE2)` selbst rufen.
   * `re15_audio_weapon_se` ist dort ein **leerer Stub** (`test_support.c:56`) — ARMS-SEs (Abprall 0x0A)
     sind in Unit-Tests nicht beobachtbar; `re15_audio_core_se` hat einen Spion
     (`g_test_core_se_last/_count`, `:63-65`), `re15_audio_room_se` ein Log (`:44-54`).
   * `re15_player_gunbone_world` faellt ohne Renderer auf die Engine-Pose zurueck (`re15_damage.c:1140-1154`).

### 5.3 Muster: echter exe-Integrationstest

* Skript `re15_port/tests/integration/test_<thema>.cmake`, Aufruf aus der Themen-`probes/*.cmake` in
  `if(TARGET re15_pc)` mit `${CMAKE_COMMAND} -DRE15_PC_EXE=$<TARGET_FILE:re15_pc> -DWORKDIR=... -P ...`
  (Vorlage `probes/r30_granate.cmake` Ende).
* Start der exe NUR ueber `include(spiel_lauf.cmake)` / Makro `re15_start_spiel(_erg _timeout KEY=WERT... exe)`
  (`tests/integration/spiel_lauf.cmake:1-40`: Umgebung per `set(ENV)`, NICHT `cmake -E env` — der haengt).
* Pruefung am `debug.log` / `RE15_WAFFEN_LOG` / `RE15_STATE_LOG` im WORKDIR; Ende per `RE15_EXIT_AT=<bild>#<raum>`.
* `set_tests_properties(... TIMEOUT 600)` wie beim Granaten-Lade-Riegel.

### 5.4 RE15_MIN_TESTS

`re15_port/tools/local_build.sh:63-95, 330-341`: `do_test` verlangt eine ctest-Summenzeile und
`total >= ${RE15_MIN_TESTS:-428}` (Standard heute **428**, Code-Zeile 340). Wer Tests hinzufuegt, hebt den
Standard im Skript mit an (Kopf-Kommentar fuehrt die Historie je Runde). Ohne `-DRE15_BUILD_TESTS=ON`
meldet ctest "No tests were found" mit EXIT 0 — deshalb nur ueber `local_build.sh` bauen.

---

## 6. Mess-Harness (Env-Variablen, Fundstelle)

| Variable | Bedeutung | Fundstelle |
|---|---|---|
| `RE15_GIVE="<item>:<menge>,..."` | legt Gegenstaende in freie Inventarplaetze (Mess-Harness, kein Spielverhalten) | `platform/pc/main.c:4215-4242` |
| `RE15_EQUIP=<item>` | setzt NUR die Waffen-Id (`re15_player_set_equipped_weapon`); **kein** ARMS-Bank-Wechsel (§3.2) | `main.c:4244-4252` |
| `RE15_DEBUG_JUMP="<raumhex>@<bild>"` bzw. `@gp` | Sprung ueber den Debug-Menue-Weg | `main.c:6975-7010` |
| `RE15_INPUT_SCRIPT="<Buchstaben><Sek>,..."` | Pad-Zeitleiste: U/D/L/R, X Kreuz, A Quadrat, M R1, S Start, E Select, Q L1, T Dreieck, W warten, B Kreuz-Mash, P Touch-R1 | `platform/pc/src/input_pc.c:44-59, 72-139` |
| `RE15_INPUT_SCRIPT_START=<n>` | Vorlauf (Default 90) | `input_pc.c:67, 108-109` |
| `RE15_INPUT_SCRIPT_BASIS=spiel` | Zeitachse = `g_engine.frame_count` statt gerenderter Ticks | `input_pc.c:70-75, 110-111, 395-397` |
| `RE15_FRAMEDUMP="<a>-<b>/<schritt>:<praefix>"` bzw. `"<bild>:<pfad>"` | PPM-Abzug des Backbuffers in Fenstergroesse | `main.c:10755-10780` |
| `RE15_WINDOW_SCALE=<1..8>` | Fenster-/Dump-Groesse (3 = 960x720); ohne wird nach Desktop gekappt | `platform/pc/src/render_pc.c:591-599` |
| `RE15_WAFFEN_LOG=<datei>` | je Bild `F pad w clip fc frame ph rec auto takt mag fx` + jede Spawn-Zeile + jede Waffen-SE-Zeile | `engine/src/player_common.c:196-203`, Schreiber `game_step_common.c:1678-1691`, `re15_esp.c:772-777`, `audio_pc.c:1054-1058` |
| `RE15_STATE_LOG=<datei>` | je Bild Spieler (Pos, hp, Zustand, Clip `ac`, `fx`, Magazin `mg`) + je Gegner `st ss1 ss2 ss3 g mo af stun d @(x,z,r)` — **ohne Gegner-HP** | `main.c:7404-7450` |
| `RE15_FX_LOG=<datei>` | je gezeichnetem Effekt-Slot id/sub/frame/Position/xlat/drift/Slot | `main.c:225-232, 302-314, 416-421` |
| `RE15_EXIT_AT="<bild>[#<raumhex>]"` | Ende am Bild (nach end_frame) | `main.c:10783-10800` |
| `RE15_NOAUDIO=1` | kein SDL-Audiogeraet; SE-Log-Zeilen kommen trotzdem | `platform/pc/src/audio_pc.c:1699-1702` |
| `RE15_NO_INTRO=1` | CAPCOM.STR aus | `main.c:3159` |
| `RE15_TITLE_SHOT=<bmp>` + `RE15_TITLE_SHOT_AF=<n>` | Titel-Abzug und Auto-NEW-GAME nach n Durchgaengen | `main.c:3261, 3564-3565` |
| `RE15_AI_FLAVOR=re15|re2|mixed` | Gegner-Gehirn (Default RE2) | `engine/src/enemy_ai_re2_zombie.c:104-138` |
| `RE15_SET_FLAG` / `RE15_SET_FLAG_AT` | Flags vor/nach Raumstart | `main.c:4567`, `:7018-7040` |
| `RE15_RE2_TRACE=1` | RE2-KI-Trace `re2_ki.log` | `engine/src/enemy_ai_common.c:53-70` |
| `RE15_WPN_DBG=1` | `wpnbank.log` (Bank-Wechsel) | `main.c:4117, 8210` |
| `RE15_SE_DEBUG=1` | SE-Stimmen-Details (stderr) | `audio_pc.c:687-688` |
| `RE15_FPS` | Bildrate (Default 30) | `main.c:3593-3603`, `input_pc.c:106` |
| VERBOTEN fuer Bildbelege | `RE15_AUTOSHOT`, `RE15_SOFTWARE_RENDER` | `main.c:10867`, `render_pc.c:624` (Memory reai-v2-visual-verify-gdigrab) |

**Zeitachsen-Falle:** `g_engine.frame_count` wird beim Raumeintritt auf 0 gesetzt (`main.c:7798`, auch
beim Debug-Sprung). Mit `RE15_INPUT_SCRIPT_BASIS=spiel` muss `RE15_INPUT_SCRIPT_START` deshalb groesser als
das Sprungbild sein (sonst laeuft das Skript schon im Startraum an); `RE15_FRAMEDUMP`- und `RE15_EXIT_AT`-Bilder
zaehlen ab Raumeintritt. Das State-/Waffen-Log enthaelt die Bilder VOR dem Sprung mit denselben Nummern — beim
Auswerten am Raumwechsel trennen (Spielerposition springt, in ROOM1140 auf `PL(-7600,-17600)`).

---

## 7. Ausgangsmessung 0x09 / 0x0A / 0x0B in ROOM1140 (vorhandene exe, NICHT neu gebaut)

### 7.1 Aufbau

* exe `re15_port/build/platform/pc/re15_pc.exe` (6 379 410 B, 2026-09-29 19:46), beschleunigter Renderer,
  `RE15_WINDOW_SCALE=3` (Framedumps 960x720), KEIN AUTOSHOT, KEIN SOFTWARE_RENDER, `RE15_NOAUDIO=1`.
* Laeufer `port_inventar_werkzeug/lauf_baseline.sh <marke>`: `RE15_DEBUG_JUMP=1140@250`,
  `RE15_GIVE=<id>:5`, `RE15_EQUIP=<id>`, `RE15_INPUT_SCRIPT_BASIS=spiel`, Start 300, Logs `wf.log`
  (Waffen), `state.log` (Spieler + Gegner je Bild), `fx.log` (Effekt-Draw), `env.txt`.
  Auswertung `port_inventar_werkzeug/auswertung.py <marke>` -> `build/r34g_baseline/<marke>/auswertung.txt`.
* Skript NAH (identische Eingaben fuer alle drei Granaten, dadurch identischer Zombieweg — gemessen: Zombie 2
  in allen drei Laeufen im Abzugsbild F795 auf d=1299):
  `W1,U1.5,W1,M1,MA0.2,M2.5,W1.5,M1,MU0.5,MUA0.2,MU2.5,W1.5,M2.1,MA0.2,M2.5,W3`
  = Wurf 1 MITTE (fern), Wurf 2 HOCH (fern), Wurf 3 MITTE (nah). Leon laeuft zuerst gegen den Tisch
  (steht dann auf (-5118,-17600)).
* Skript TIEF: `W1,U1.5,W1,M1,MD0.5,MDA0.2,MD2.5,W3`.
* Gegner in ROOM1140 beim Eintritt: Slot 1 Typ 0x16 (liegend, g=0x88), Slots 2/3 Typ 0x10, Slots 4/5 Typ 0x11,
  Abstaende 6122..8748 (Kalibrierlauf `kalib`), alle fressend (st 1, ss1 8).
* Bildnummern = Bilder nach dem Raumeintritt (Zaehler-Reset `main.c:7798`). Das Waffen-Log-Bild F zeigt
  den Zustand VOR dem Feuer-Pfad dieses Bilds; "Abzug im Bild F" = das Bild, in dessen game_step der
  Abzug fiel (Log F+1 zeigt frame=1).

### 7.2 Ergebnis (Flavor RE2 = Default)

| Granate / Zielhoehe (Lauf) | Abzug | Wurf-Clip, fc, rec-Bilder | Spawn (Bild, anim_frame) | Spawn-Anker rel. Leon (x,y,z) | Flug im Port | fx-Slots | Munition | Schaden / Gegnerreaktion |
|---|---|---|---|---|---|---|---|---|
| 0x09 MITTE (g09nah W1) | F435 | Clip 7, 35, F435..F468 (frame 0..33) | F457, **22**: `id=4 sub=13 scale=0x1000 streams=1` | (+592, -2474, -819) | senkrechter Fall, acc (0,10,0), x/z konstant, Boden y=0 nach 23 Bildern, danach Port-Federklemme | 0 -> 1, **nie frei** | 5 -> 4 **im Abzugsbild** | keiner (Zombies >= 4283 entfernt) |
| 0x09 HOCH (g09nah W2) | F606 | Clip 9, 40, F606..F644 (0..38) | F625, **19** | (+791, -3171, +406) | senkrechter Fall 3171 | 1 -> 2 | 4 -> 3 in F606 | keiner (>= 3907) |
| 0x09 TIEF (g09tief) | F450 | Clip 11, 40, F450..F488 (0..38) | F474, **24** | (+650, -772, -799) | senkrechter Fall 772 | 0 -> 1 | 5 -> 4 in F450 | keiner (>= 4463) |
| **0x09 MITTE NAH (g09nah W3)** | F795 | Clip 7, 35, F795..F828 | F817, 22 | (-569, -2474, -838) | wie oben | 15 -> 30 -> 7 (Blut/Feuer), Granate +1 | 3 -> 2 in F795 | **Zombie 2 (0x10, d=1299) IM ABZUGSBILD F795 getroffen: st 1->3, ss1=9** (Bruecke `ENT[9].resolve`), 22 Bilder BEVOR die Granate die Hand verlaesst; im selben Bild Spawns Effekt 8 sub 3 (x2), Effekt 7 sub 2 (x2), Effekt 0 (x3, dann je 1 in F797/799/801/803/805 = 8); weggeschleudert d 1299 -> 2792 bis F800; Leiche st 7 ab F840 |
| 0x0A MITTE / HOCH / NAH (g0a) | F435 / F606 / F795 | wie 0x09 (Bank W0A, "Clips 13, Rueckstoss-Clip7 fc=35") | **kein Spawn** | — | — | 0 bleibt 0 | 5->4, 4->3, 3->2 je im Abzugsbild | **keiner**: Zombie 2 in d=1299 bleibt unverletzt, greift in F800 (Abzug+5, ss1 1->3) — Wurf bei frame 5 abgebrochen, Leon hp 100 -> 80 (F821) -> 60 (F859) |
| 0x0B MITTE / HOCH / NAH (g0b) | wie 0x0A | Bank W0B, sonst wie 0x0A | kein Spawn | — | — | 0 | wie 0x0A | wie 0x0A (bitgleiches Zustandsprotokoll) |
| 0x0A / 0x0B TIEF (g0atief, g0btief) | F450 | Clip 11, 40, F450..F488 | kein Spawn | — | — | 0 | 5 -> 4 in F450 | keiner |

Weitere Messpunkte:
* **SE:** in allen Laeufen **0** Waffen-SE-Zeilen (`SE arms_rec=`) — kein Wurf-, Abprall- oder Explosions-SE
  (Zaehlung `grep '^    SE '` je wf.log: g09 0, g09nah 0, g09tief 0, g0a 0, g0b 0, g0atief 0, g09nah_re15 0).
* **Sprite im Flug:** `fx.log` zeigt fuer die Granaten-Slots `frame` 0..7 im Kreis (je 138x in g09nah) = Anim-Records
  0..7 von Effekt 4 = die Huelsen-Zellen 0..7 (CORE00.ESP @0x01730..0x01768, Zellen u=112..224 v=88) und
  Textur-Slot 23 (Huelsen-Standbild). Routine 30 setzt im Original +0x6e := 0x17 (zit.) = Records 23..35
  (@0x017E8..0x01848, Zellen 16..23, Schleife bei 35 auf 23) — im Port nie erreicht. Im Bild: kleiner grauer
  Quader neben der Hand (`g09_wurf1_lupe.png` F458..F473), der senkrecht zu Boden faellt.
* **Wurf-Unterbrechung:** In `g09` (fruehere Variante mit 5 Wuerfen) brach Zombie 2 den TIEF-Wurf bei frame 23
  per Griff ab (F800, kein Spawn, Munition trotzdem weg) — Abbruch ueber `re15_player_aim_interrupt`
  (`player_common.c:310-316`), nicht ueber den R1-Recoil-Break.
* **Flavor RE1.5** (`g09nah_re15`, gleiches Skript): Wurf, Spawn, Munition wie RE2; die Zombies bleiben den
  ganzen Lauf auf st 1 / ss1 12 (Slot 2: d=3873) — **kein Nah-Treffer messbar** (OFFEN O8).
* Kein Spieler-Schaden durch eine Granate in irgendeinem Lauf (hp-Wechsel nur durch den Zombie-Griff).
* Gerendert wird die Granate NUR in Leons Hand-Mesh (`main.c:8990-9010`, immer sichtbar solange ausgeruestet);
  ob das Hand-Mesh nach dem Loslassen die Granate verliert, war auf den Dumps nicht aufloesbar (OFFEN O7).

### 7.3 Bilder (Werkzeug-Ordner)

| Datei | Inhalt |
|---|---|
| `g09_wurf1_mitte.png` | Gesamtbild Wurf 1 (MITTE), 8 Bilder F434..F479 |
| `g09_wurf1_lupe.png` | Lupe auf Leon, F434..F500: Ausholen, Loslassen, grauer Huelsen-Sprite F458..F473, faellt senkrecht |
| `g09_wurf2_hoch_lupe.png` | Lupe HOCH-Wurf F605..F740: Ueberkopf-Wurf, Sprite an der erhobenen Hand F626, faellt senkrecht; F740 Zombie 2 naht |
| `g09tief_lupe.png` | Lupe TIEF-Wurf F448..F520: Rollwurf in der Hocke, Sprite am Boden F476/F480 |
| `g09nah_wurf3_treffer_lupe.png` | Lupe NAH-Wurf F792..F900: Blutwolke am Zombie schon F796 (Abzug+1), Zusammenbruch/Feuer F800..F812, Granate verlaesst die Hand erst F817, Leiche brennt |
| `g0a_wurf3_nah_lupe.png` | Saeuregranate NAH F792..F900: kein Treffer, Zombie greift ab F800, Biss |

Rohdaten: `build/r34g_baseline/{kalib,erkund_nah,g09,g09nah,g09tief,g0a,g0b,g0atief,g0btief,g09nah_re15}/`
(je `env.txt`, `debug.log`, `wf.log`, `state.log`, ab g09nah auch `fx.log`, `auswertung.txt`).

---

## KONSTANTEN FUER DEN BAU

Fuer dieses Thema stehen hier (a) die EINHAENGEPUNKTE und (b) die wenigen Daten-Konstanten, die dieses Dossier
selbst aus Datei-Bytes gelesen hat. Verhaltens-Konstanten der Routinen 29/30/31 (Geschwindigkeiten, Zuender,
Radius, SE-Ids) liefern die RE-Dossiers der Runde — hier nur als "(zit.)" im Kontext.

### (a) Einhaengepunkte (Datei:Zeile, Stand a358fd5d)

| # | Wo | Was dort hin muss / was dort heute steht |
|---|---|---|
| E1 | `engine/src/re15_esp.c:523-712` `esp_fx_dispatch` (switch `:527`) | `case 30:` Wurf-Init, `case 31:` Explosion. Konvention Anim-Index: `f->frame = wert - 1; f->timer = 0;` (Muster Routinen 5/10/16, `:551-553, 662, 619`) |
| E2 | `re15_esp.c:724-736` `esp_fx_dispatch_b` | `case 29:` Flug/Abprall; heute Frueh-Return fuer alles ausser 12 (`:727`) |
| E3 | `re15_esp.c:903-922` Sammel-Physik + Bodenklemme | fuer Granaten-Slots umgehen oder durch das Original-Positionsmodell ersetzen (sonst doppelte Bodenbehandlung) |
| E4 | `include/re15_esp.h:150-202` `re15_esp_fx_t` | Welt-Position +0x28/+0x2a/+0x2c (s16) fehlt; Draw rechnet sie als Anker+xlat (`platform/pc/main.c:296-298`) |
| E5 | `re15_esp.c:582-583, 641-642` Kind-Spawns | Position = Anker; Explosions-Kinder brauchen die Slot-Welt-Position (Anker+xlat) |
| E6 | `engine/src/game_step_common.c:1775` `ENT[9] = {1,1,1,0}` | `resolve=1` = Bruecke (Sofort-Hitscan beim Abzug) -> 0, sobald der Flaechenschaden steht; `[10]/[11]` (`:1779-1780`) |
| E7 | `game_step_common.c:1947-1964` Granaten-Spawn | Gate `== 9` (`:1951`); fuer 0x0A/0x0B (RE2-Ziel) erweitern |
| E8 | `engine/src/re15_damage.c:3294-3355` `re15_resolve_attack(atk,type,slot)` | Flaechenschaden-Eintritt; Aufrufmuster `re15_enemy_attack` `:3375-3385` (radius 500, Typ 0). Angreifer-Slot fuer einen Effekt: keiner (`-1`) — Selbstausschluss `:3332` |
| E9 | `engine/src/enemy_ai_re2_zombie.c:3847` `re2z_row_from_atktype` / `re15_damage.c:2869` `re15_re2_stamp_hit` | welche RE2-Zeile die Explosion stempelt (heute Typ 2 -> 17); Reaktions-Bausteine 9/10/11 liegen bereit (§4.4) |
| E10 | `platform/pc/src/audio_pc.c:1052-1064` / `:976-982` | Abprall = `re15_audio_weapon_se(0x0A)` (braucht ARMS09 resident), Explosion = `re15_audio_core_se(8)`; Byte 1 und Positionszweig nicht portiert |
| E11 | `platform/pc/main.c:4244-4252` `RE15_EQUIP` | fuer SE-Messungen `re15_audio_prime_weapon(id)` mitrufen (heute bleibt ARMS01 geladen) |
| E12 | `platform/pc/main.c:236-452` `pc_draw_effects`, Lader `:3714-3736` | Palette `f->clut` auswerten; 4-bpp-Seiten + CLUT-Zeilen aus `DATA/TEX.TIM` (Muster Effekt 8, `tools/tex_tim_effect_slice.py`) |
| E13 | `platform/pc/main.c:7438-7446` `RE15_STATE_LOG` (Gegner-Zeile) | Gegner-HP fehlt im Log (Schadensmessung nur ueber Zustandswechsel moeglich) |
| E14 | `re15_port/tests/unit/probes/<thema>.cmake` + `tests/unit/probe_<thema>.c`; exe-Test `tests/integration/test_<thema>.cmake` | Muster §5.2/5.3; `tests/test_support.c:56` Waffen-SE-Stub ohne Spion; `tools/local_build.sh:340` RE15_MIN_TESTS 428 |
| E15 | `engine/src/player_common.c:469-478` `re15_player_granate_frame` | liefert nur das Bild; Zielhoehe fuer Routine 30 aus `re15_player_aim_elevation()` (`:290`) |
| E16 | Latch 0x800b5358 (Setzer Routine 31 zit. @0x8001857c) | im Port kein Feld und kein Leser; Original-Leser = Lichtblitz im Hauptloop @0x8001ce5c-0x8001d084 (§2.7). Einhaengen hinter dem Item-Modal-Tick (Original-Reihenfolge `8001ce34 jal 0x8001db28` -> `8001ce5c`) |
| E17 | `platform/pc/main.c:5413` Effekt-Tick | im Original NACH dem Spieler-Dispatcher im selben Bild (`8001ce0c` -> `8001ce2c`, §2.5); Port tickt davor -> erster Granaten-Tick 1 Bild spaeter |

### (b) Selbst gelesene Daten-Konstanten (Datei-Offset + Bytes)

| Name | Wert | Adresse / Datei-Offset | Bytes | Verwendung |
|---|---|---|---|---|
| Wurf-Clip MITTE fc | 35 | `PLD/PL00W09.PLW` 0x24 (EDD dir[0] @0x8, Clip 7) | `23 00 08 03` | Clip 7 laeuft 35 Bilder (Spawn-Bild 22 liegt darin); W0A/W0B byte-gleich, PL04W09 @0x24 `23 00 e8 03` |
| Wurf-Clip HOCH fc | 40 | PL00W09.PLW 0x2C (Clip 9) | `28 00 98 03` | Spawn-Bild 19 |
| Wurf-Clip TIEF fc | 40 | PL00W09.PLW 0x34 (Clip 11) | `28 00 3c 04` | Spawn-Bild 24 |
| Granate Zeile 0 | A=30, B=0, wh 0x1000/0x1000, acc (0,10,0) | `DATA/CORE00.ESP` 0x1AB8 (Effekt 4 Row-sub 5) | `1e 00 00 00 00 10 00 10 00 00 0a 00 ...` | Start-Routine der Granate |
| Granaten-Anim | Records 23..35 -> Zellen 16..23, Schleife 35->23 | CORE00.ESP 0x017E8..0x0184F | `10 01 01 10 ...` / Ende `17 01 ff 10` | Sprite der fliegenden Granate (Routine 30 setzt +0x6e := 0x17, zit.) |
| Kind 3/0x19 Zeile 0 | A=10, flags 0x13, anim 0x0a | CORE00.ESP 0x003F4 | (esp_rows.txt) | laeuft ueber vorhandene Routine 10 |
| Kind 3/0x0B Zeile 0 | A=10, acc (0,5,0), vel (0,-130,0), flags 0x13, tpage\|0x40, anim 8 | CORE00.ESP 0x004CC | (esp_rows.txt) | dito |
| Abprall-SE-Record | prog 0, Ton 1, prio 3, Stimme 0 | `SOUND/ARMS09.EDH` 0x028 (Record 0x0A) | `00 00 13 10` | SE 0x010A0001 (zit.); ARMS0A/0B byte-gleich |
| Explosions-SE-Record | Ton 9, prio 3, Direkt-Zweig | `SOUND/CORE00.EDH` 0x020 (Record 8) | `00 00 93 00` | SE 0x04080001 (zit.) |
| CLUT Granate | 0x7B11 -> VRAM(272,492) | `DATA/TEX.TIM` 0x00334 | `0000 b631 b1ef adce ...` | Palette des Flug-Sprites |
| CLUT Kind 3/0x19 | 0x78D1 -> VRAM(272,483) | TEX.TIM 0x000F4 | `0000 ffff ebde d7de ...` | Palette Explosion |
| CLUT Kind 3/0x0B | 0x7851 -> VRAM(272,481) | TEX.TIM 0x00074 | `0000 9ce7 98c6 94a5 ...` | Palette Rauch |
| Routine 29 SE / Flags / Selektor | `0x010a0001`, Flags 0x63, A := 0x1f | PSX.EXE @0x80018350-58, @0x80018368-6c, @0x80018378-84, @0x80018410-28 | `lui a0,0x10a; ori a0,a0,0x1; jal 0x80045024` / `ori v0,zero,0x63; sb v0,108(v1)` / `ori v0,zero,0x1f; sh v0,0(v1); sh zero,2(v1)` | nur zur Beschreibung der Luecke; volle RE in den Routinen-Dossiers |
| Effekt-Tick-Reihenfolge Original | Spieler vor Effekten im selben Bild | PSX.EXE @0x8001ce0c / @0x8001ce2c | `jal 0x80031c44` / `jal 0x80019e20` | E17: Port tickt Effekte vor game_step (1 Bild Versatz) |
| Latch-Leser 0x800b5358 | einziger Leser, Lichtzeile der aktiven Kamera | PSX.EXE @0x8001ce5c-0x8001d084 | `lbu v0,21336(v0)` @0x8001ce60; `sb zero,3(v0)` @0x8001cef8; Klemmen `sltiu 0xd2/0x8c/0x50` @0x8001cf28/64/a0; `ori v1,zero,0x1770; sh v1,38(v0)` @0x8001d080-84 | E16: Lichtblitz bei Schuss/Explosion fehlt im Port |

---

## PORT-ABGLEICH

### Einordnung je Mechanismus (fertig / unfertig im Auslieferungsstand, zit. aus Runde 30 / waffen_fsm SPEC)

| Mechanismus | RE1.5-Stand | Massgeblich |
|---|---|---|
| Wurfanimation (Clips 7/9/11, Bank W09/W0A/W0B) | fertig (drei byte-gleiche Baenke) | RE1.5 |
| Munitionsabzug beim Abzug (Stubs @0x80033b38/58/78 zit.) | fertig | RE1.5 |
| Projektil 0x040D1000 fuer 0x09 (Spawn @0x800336bc-0x800337a4 zit.) + Routinen 30/29/31 | fertig (Code vorhanden) | RE1.5 |
| Projektil/Wirkung fuer 0x0A/0x0B | UNFERTIG (Spawn hart auf Id 9 @0x8003368c zit., Handler nur Munition) | RE2 Retail (Beleg durch die RE2-Spur) |
| Gegnerreaktion auf die Explosion | RE1.5: Flaechenresolver FUN_80012d60 Typ 2 (zit.); RE2-Flavor: RE2-Zeilen | je Flavor |

### Was der Port heute tut, und die Luecke

| Mechanismus | Port heute (Datei:Zeile) | gemessen (§7) | Luecke |
|---|---|---|---|
| Ausruesten / Bank | `main.c:8171-8220`, `:3958-3981` | W09/W0A/W0B geladen, Clip7 fc=35 | keine |
| Wurfanimation | `player_common.c:393-414, 1105-1122, 940-975` | MITTE 35 / HOCH 40 / TIEF 40 Bilder, alle drei Arten gleich | keine gemessen |
| Munition | `game_step_common.c:1810-1811` + `inventory_common.c:176-181` | -1 im Abzugsbild | keine |
| Spawn 0x09 | `game_step_common.c:1947-1964` | Bild 22/19/24, `id=4 sub=13 scale=0x1000` | keine (Zeitpunkt/Offset wie zit.) |
| Spawn 0x0A/0x0B | Gate `== 9` `:1951` | kein Spawn | RE2-Ziel fehlt ganz |
| Flug | Routine A 30 / B 29 fehlen (`re15_esp.c:710, 727`) | senkrechter Fall mit acc (0,10,0) an der Hand, Port-Federklemme (`re15_esp.c:916-921`) | Wurfgeschwindigkeit, Abprall, Zaehler, SEs |
| Explosion | Routine A 31 fehlt | nichts; Slot nie frei (fx steigt je Wurf um 1) | Zuender, Kind-Effekte 3/0x19, 3/0x0B, SE, Flaechenschaden, Slot-Freigabe, Licht-Latch 0x800b5358 |
| Schaden 0x09 | Bruecke `ENT[9].resolve=1` -> `re15_player_weapon_fire(9)` beim ABZUG (`game_step_common.c:1808-1809`) | Zombie in d=1299 stirbt im Abzugsbild, 22 Bilder vor dem Loslassen; ein Ziel, kein Radius, Spieler nie betroffen | Zeitpunkt, Ort, Radius, Mehrfachtreffer, Spielerschaden (zit. FUN_80012d60 prueft den Spieler) |
| Schaden 0x0A/0x0B | `ENT[10/11].resolve=0` | keiner | RE2-Ziel |
| Gegnerreaktion | RE2-Zeilen 9/10/11 fertig (`enemy_ai_re2_zombie.c` §4.4), erreichbar nur ueber die Bruecke (w9) | w9: Zerreiss-Tod + Feuer; w10/w11: keine | Stempel aus der Explosion; Zeile fuer Typ 2 heute 17 |
| SE | keine Aufrufe | 0 SE-Zeilen in allen Laeufen | Abprall ARMS 0x0A, Explosion CORE 8, Positionszweig, Byte 1 |
| Sprite | Draw ohne Palette (`main.c:445-448`), Anim 0..7 | grauer Huelsen-Quader | Anim-Start 23, CLUT 0x7B11 / 0x78D1 / 0x7851, 4-bpp-Seiten |
| Tests | nur Aufnahme-Riegel (`probes/r30_granate.cmake`) | — | kein Wurf-/Explosions-Riegel |
| Lichtblitz (Latch 0x800b5358) | kein Leser im Port | — | Original-Leser @0x8001ce5c-0x8001d084 schreibt die Kamera-Lichtzeile (auch fuer jeden Schuss, Routine 9) |
| Tick-Reihenfolge | Effekt-Tick vor `re15_game_step` (`main.c:5413` / `:7358`) | Spawn F457 -> erster Slot-Tick F458 | Original: Effekt-Tick nach dem Spieler im selben Bild (`8001ce0c` -> `8001ce2c`) |

---

## OFFEN

| # | Offen | Versuchte Wege | Naechster Weg |
|---|---|---|---|
| O1 | Mechanik der Routinen 30/29/31 im Detail (Geschwindigkeit je Zielhoehe, Abprall, Zuender, Kind-Spawner FUN_800199d4, Flaechenschaden) | nicht Auftrag dieses Dossiers; nur B 29 zur Luecken-Beschreibung disassembliert (§2.3) | RE-Dossiers der Runde (Flug/Explosion) |
| O2 | RE2-Vorbild fuer 0x0A/0x0B | nicht Auftrag dieses Dossiers | RE2-Spur (GL-Saeure/-Brand) |
| O3 | Semantik von Slot +0x2a in Routine 29 (`lh t1,42(t0)`/`blez` @0x80018330-38, `subu`/`sw 56(t0)` @0x800183dc-e0): +0x28/+0x2a/+0x2c schreibt der Haupttick als Welt-Position (sh @0x8001a0e8..@0x8001a2a4) | eigene Disasm von FUN_80019e20 (`build/r34g_baseline/dis_80019e20.txt`); Beobachtung: der Boden von ROOM1140 liegt auf Welt-Y 0 (Port-Granate landet bei Anker+xlat = 0 = `pl->y`) | Haupttick @0x8001a014-0x8001a2a4 samt +0x40/+0x60-Feldern vollstaendig RE'en — liegt bei der Flug-Spur (`re_wurf_flug_explosion.md` §2.3/§2.4, parallel entstanden; dort "Boden = Ebene Welt-y 0", passt zur eigenen ROOM1140-Beobachtung) |
| O4 | ~~Reihenfolge Effekt-Tick vs. Spieler-Dispatcher~~ GEKLAERT: Original `8001ce0c jal 0x80031c44` vor `8001ce2c jal 0x80019e20` (§2.5); Port umgekehrt | Hauptloop-Disasm `re15_disasm.py dis 0x8001cdd0 40` | im Bau entscheiden (E17) |
| O5 | Bedeutung von Byte 1 der SE-Id in FUN_80045024 (Routine 29 legt dort den Zaehler ab) | Port-Code gelesen: kein Leser | FUN_80045024 disassemblieren (Audio-Spur) |
| O6 | Gegner-HP im Messlauf | `RE15_STATE_LOG`/`RE15_RE2_TRACE` gelesen: kein HP-Feld | Harness-Zusatz (E13) im Bau |
| O7 | Verschwindet die Granate im Hand-Mesh nach dem Loslassen? | Lupe 960x720 (F431..F500) — Hand zu klein aufgeloest | PLW-Keyframes/Mesh-Wechsel im Original pruefen oder hoeher aufgeloeste Lupe; Savestate |
| O8 | Nah-Messung im RE1.5-Flavor | Lauf `g09nah_re15` mit identischem Skript: Zombies bleiben st 1 / ss1 12, d >= 3873 | anderer Laufweg (um den Tisch) oder Raum mit naeheren RE1.5-Zombies |
| O9 | Latch 0x800b5358: Leser GEFUNDEN (@0x8001ce60, Lichtblitz, §2.7); OFFEN ist, WO er zurueckgesetzt wird und wie die gesicherte Lichtzeile wiederhergestellt wird | eigener Offset-Scan `latch_xref.py` (Offsets 0x5354/0x5356/0x5358/0x5359, jede Basis): keine Loeschung gefunden | Block-Loeschung (memset/Struktur-Basis) suchen, Savestate-RAM vor/nach einem Schuss vergleichen (re15-savestate-ghidra) |
| O10 | Welche RE2-Zeile eine Explosion stempelt (Port heute Typ 2 -> 17) | Port-Kommentar `re15_damage.c:1176-1177` nennt RE2-AoE FUN_80047664 | RE2-Spur Gegnerreaktion |
