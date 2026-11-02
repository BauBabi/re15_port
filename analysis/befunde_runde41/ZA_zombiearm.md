# Spur ZA "zombiearm" - Dossier (Runde 41, Basis master 789f6e681)

Baum `.claude/worktrees/r41_zombiearm`, Zweig r41/zombiearm.

## Auftrag (Wortlaut)
> "wtf, warum haben denn die dicken Zombies jetzt ploetzlich so einen duennen Stummel Arm?"
> "Den sollen sie nicht haben. Die sollen schon den original Arm haben...."
> Nachtrag: "Ja, das Aussehen der Modelle bei Resident Evil 1.5 ist entscheidend! Haben wir im Port jetzt Stummel
> Aermchen oder irgendeinen anderen Quatsch gemacht ist das falsch."
> Regel: "Ist ok, auch fuer die dicken Zombies. Wenn du zu 100% sicher nachweisen kannst, dass das auftaucht mit den
> duennen Aermchen, dann lag ich wohl falsch." Behalten nur mit Beleg am echten Original (RAM: Teil-Wort mit
> 0x400|0x80 und Skala UND im Bild desselben Moments sichtbar duenn), sonst fuer ALLE Zombie-Typen entfernen.

## Ergebnis in einem Satz
Die Spawn-Variante steht im Original-Code, wird im Original aber **nie sichtbar**: in allen 26 sauberen
Savestates mit Zombies liegen Skala und Tinte der Variante im Teil-Record, das Teil-Wort steht aber auf
`0x00000001` (ohne 0x400|0x80), und der Zeichner skaliert nur mit Bit 0x400. Ausserdem traefe sie im Original nie
einen Arm. -> Variante fuer **alle** Zombie-Typen abgeschaltet (Wuerfe bleiben fuer den Zufallsstrom).

## 1. Der dicke Zombie
- **Typ 0x11**, RE1.5-Modell **EM11** (`CDEMD0.EMS` Blob 1), im RE2-Modellmodus RE2 `EM011`, im Hybrid
  RE1.5-Geometrie unter RE2-Rig (`[enemy] Hybrid EM11: RE1.5-Geometrie (15 Meshes) unter RE2-Rig (15 Bones, 31 Clips)`).
- Masse aus dem Original (`re15_port/tools/emd_ansichtsblatt.py --typ 0x11`): Breite 1184 / Tiefe 852 gegen
  EM10 1033 / 736 und EM16 1031 / 755. Kurzaermeliges Hemd, nackte, dicke Unterarme. Bild `ZA_belege/em11_ansichtsblatt.png`.
- ROOM1140 laedt EM16, EM10, EM11 (Typen 0x16, 0x10 x2, 0x11 x2; Savestate `mzd_stage1_briefing.sav`: Slot 0 0x16,
  Slot 1/2 0x10, Slot 3/4 0x11).
- Welche Teile die Variante trifft (RE1.5-Teilnummern, Knochen-Reihenfolge laut `re2_hybrid_perm`: 0 Huefte,
  1-3 / 4-6 Beine, 7 Brust, 8-10 / 11-13 Arme, 14 Kopf):
  0x80101058 Teil 12 (Unterarm) Skala 0x5f4, 0x80101008 Teil 2 (Unterschenkel) 0x7e8, 0x801010a8 Teile 8+9
  (Oberarm + Unterarm) je 0x5f4, 0x8010113c Teil 5 0x7e8, 0x8010118c Teile 4+5 0x7e8/0x5f4.
  Skala nur auf x/z -> der dicke nackte Unterarm von EM11 schrumpft auf 37 % Dicke = der "duenne Stummel".
  Im Port wuerfelt der Port-Zufall, also auch Arm-Varianten (gemessen, Abschnitt 4).

## 2. RE: Original-Code (selbst disassembliert, `re15_disasm.py`)
Dispatch (STAGE1.BIN, Registrierung @0x8011e868-c4): `0x80072bac[0x10/0x11/0x12/0x16/0x18/0x1c-0x1f] = 0x80100424`
(`sw v0,11244(at)` @0x8011e88c = Typ 0x10, `sw v0,11248(at)` @0x8011e894 = 0x11, ...). Zustand 0 = `0x8011f7b4[0] =
0x80100688` (INIT). **Der dicke Zombie laeuft durch denselben INIT, kein Typ-Tor vor der Variante.**

INIT FUN_80100688 (STAGE1.BIN; **byte-gleich** in `Version_40%/PSX/BIN/STAGE1.BIN`, diff der 175 Instruktionen ab
0x80100688 und der Registrierung ab 0x8011e860 leer, Tabelle @0x8011f7ec identisch):
```
80100840: lui a0,0x800b / 80100844 lw a0,-14460(a0)   ; a0 = aktuelles Entity (bleibt bis zum rng)
801008b0: jal 0x8001af20                               ; rng
801008bc: andi v0,v0,0x3
801008c0: bne v0,zero,0x801008f4                       ; 75 %: keine Variante
801008c8: jal 0x8001af20                               ; rng
801008d0: andi v0,v0,0x7
801008dc: addiu at,at,-2068                            ; 0x8011f7ec
801008e4: lw v0,0(at)
801008ec: jalr v0
```
Tabelle @0x8011f7ec = {0x80101058, 0x80101008, 0x801010a8, 0x801010a8, 0x80101008, 0x8010118c, 0x8010113c, 0x8010118c}.
Routine 0x80101008 (Beispiel): `lw v0,392(v0)` (Teile), `ori a0,a0,0x400` / `sw a0,344(v0)` (Teil 2 Wort),
`sh a1(=0x7e8),480/484(v0)` (Skala x/z), `sh 0x1000,482` (y), `sw 0x80,492` (+0x94), `sw 0x8080e0,496` (+0x98),
`ori a0,a0,0x80` / `sw a0,344(v0)`. Teil-Stride 0xac.

Zeichner (PSX.EXE FUN_8001e9ec): `lw v0,0(s2)` / `andi v0,v0,0x400` @0x8001ebb0 / `beq` -> sonst `lh 136/138/140`
(Skala) + `jal 0x80065ff0` (ScaleMatrix) @0x8001ebdc. **Ohne Bit 0x400 keine Skala.**

### 2a. Der rng ist im Original kein Zufall
FUN_8001af20 hasht das a0-Register (`srl v1,a0,7` @0x8001af30 ... `andi v0,a0,0xff`; Memory `reai-v2-rng-determinism`).
Hier ist a0 = Entity-Zeiger (`0x800acc2c + Slot*0x1f4`). Nachgerechnet (`ZA_belege/rng_hash_je_slot.txt`):
Slot 0..3 -> Wurf 0xc4/0xbc/0xb4/0xac (&3 = 0) -> Index 5 = **0x8010118c (Bein, Teile 4/5)**; Slot 4..15 -> keine
Variante. Im Original kaeme also nie ein Arm dran, und welches Glied, haengt nur am Slot.

### 2b. Gemessen: im Original ist die Variante weg, bevor sie gezeichnet wird
`ZA_belege/allsaves.py` ueber alle sauberen Savestates (`stage_saves/**`, ohne die 6 PATCHED): 26 Saves mit
Zombies (ROOM1140-Reihe, Tuer-Saves, Inventar-Saves, `mzd_demo/mzd_stage2_room.sav` u.a.). Ergebnis
(`ZA_belege/savestate_teilwoerter.txt`):
- Jeder Zombie in Slot 0..3 traegt in Teil 4 Skala (0x7e8,0x1000,0x7e8) + Tinte 0x80/0x8080e0 und in Teil 5
  (0x5f4,0x1000,0x5f4) + Tinte -> **die Routine 0x8010118c ist gelaufen**, genau wie 2a vorhersagt; Slot 4+ ohne.
- **98 Zombie-Eintraege mit diesem Varianten-Rest, bei allen 98 steht das Teil-Wort in allen 15 Teilen auf
  0x00000001.** Kein einziges 0x400 oder 0x80. (Drei Eintraege in `lamp_1170_stage1.sav` / `mzd_title_after_death.sav`
  zeigen Zufallsworte OHNE Varianten-Rest: Entity-Reste, deren +0x188 auf fremden Speicher zeigt.)
- Damit zeichnet der Original-Zeichner die Glieder in voller Dicke. Bilder (Framebuffer aus dem Savestate, `fb.py`):
  `ZA_belege/original_1250_todeskamera_slot0_1_fb.png` (`mzd_stage1_combat_death.sav`, ROOM1250, Todeskamera):
  die beiden Zombies am Spieler sind Slot 0 (-12360,-20038) und Slot 1 (-12030,-18966), Spieler (-12435,-20389) -
  BEIDE tragen in Teil 4/5 Skala 0x7e8/0x5f4 + Tinte, beide Beine sind gleich dick, keine Verfaerbung.
  `ZA_belege/original_1140_engage_live_fb.png` (`mzd_stage1_engage_live.sav`): Slot 0..3 inkl. des dicken Slot 3
  (Typ 0x11) mit Varianten-Rest, Glieder normal.

Wer die Bits nach dem INIT wieder loescht, ist **offen**: kein `jal 0x8001e5b0`/`0x8001ee68` nach dem INIT
(einzige Aufrufer: Sce_em_set @0x80042530 VOR der Wurzel @0x8004259c, Spieler @0x80031624/@0x80031888, Liege-Zweig
@0x80100fe0), keine indirekte Referenz (lui/addiu-Paar oder Datenwort) auf 0x8001e5b0/0x8001ee68; die 64
base-getrackten Stores auf Teil-Woerter in PSX.EXE + STAGE1.BIN sind Varianten-, Gore- und Treffer-Pfade; die Schleifen
mit Teil-Stride 0xac in der EXE (Keyframe-Entpacker 0x8001f6c0-8a8, anim_set, 0x8003a8e8, 0x8005d1d0, 0x80060f10)
schreiben andere Felder. Zwei PCSX-Redux-Laeufe mit Exec-/Schreib-Haltepunkten (`ZA_belege/zarm_watch.lua`) erreichten
ROOM1140 nicht (Debug-Sprung landete in 1240 bzw. 1170). Naechster Weg: Haltepunkt-Lauf mit funktionierendem Sprung
oder ueber eine Tuer nach 1140, Schreib-Haltepunkt auf Teil 4 Wort von Slot 0 nach Exec 0x8010118c.
Fuer die Entscheidung ist das unerheblich: gemessen ist der Zustand, den der Zeichner liest.

### 2c. RE2
RE2-Zombie-INIT (EMOVL10_S0.BIN @0x8010065c-0x80100b78, selbst gelesen): nur Wuerfe fuer HP (Tabellen
@0x8010c600/620/670/690), Gang (@0x80100860-78), Resistenz (`andi 0xf` / `addiu 16` / `sb 547` @0x80100890-9c);
kein `jalr`, keine Skala-/Tinten-Stores auf Teile. Fuer Typ 0x11 einzig: `bne v1(Typ),17` @0x801008bc ->
HP 250 (`sh v1,342` @0x801008cc) und `+0x21a |= 0x8000` (@0x801008d0-d4, die Fett-Kadenz). **RE2 hat keine
Spawn-Variante mit duennen Gliedern.**

## 3. Aenderung
`re15_port/engine/src/enemy_ai_common.c` re15_enemy_ai_live_init: die Anwendung der Routinen steht in `#if 0` (Code
mit allen Adressen bleibt lesbar), der Kommentar haelt den Befund. Die beiden Wuerfe @0x801008b0/@0x801008c8 bleiben
(Zufallsstrom unveraendert, `probe_1030_stuck_who` Seed 8 bleibt gueltig). Der Teil-Reset bleibt (FUN_8001e5b0 schreibt
Wort 1 je Teil @0x8001e758 in Sce_em_set). Gilt fuer alle Typen und beide Gehirne (das RE2-Gehirn ruft denselben
INIT, enemy_ai_re2_zombie.c re2z_init; ohne markierte Teilschicht laeuft dort `re15_teile_umordnen` nicht).
Einordnung: **byte-true im sichtbaren Ergebnis** (Teil-Wort 1 wie in allen Original-Savestates), nicht als
Port-Wahl.

## 4. Messung Port (echtes Fenster)
Abgriff: `ZA_belege/fenster_bild.py` = PrintWindow (PW_RENDERFULLCONTENT) auf das Fenster der EIGENEN PID. gdigrab per
Titel griff hier daneben: sechs parallele Agenten-Fenster tragen denselben Titel "RE1.5 Rebuilt - PC" (gemessen
`Get-Process`), gdigrab lieferte 320x240 schwarz eines fremden Fensters. Kein AUTOSHOT, kein SOFTWARE_RENDER, Messkopien
`re15_pc_za_vorher.exe` (Basis + EINE temporaere Logzeile `[ZA-MESS]` in der Variante, nicht committet) und
`re15_pc_za_nachher.exe`. Rezept `ZA_belege/hunt2.sh`: `RE15_NO_INTRO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
RE15_AI_MODELS=re15 RE15_DEBUG_JUMP=1140@240 RE15_WINDOW_SCALE=3 RE15_RNG_ENTROPY=1 RE15_FORCE_CUT=1` (Hybrid wie im
Nutzerlauf; Entropie, weil der feste Seed in 1140 keinem Zombie eine Variante gibt - 0 von 5).

- **Vorher (master, Leon, RE2-Gehirn):** In den Entropie-Laeufen bekommt jeder Typ Varianten, auch die dicken, auch
  Arme, z.B. `slot 4 typ 11 idx 0 Teile 12/-1`, `slot 5 typ 11 idx 2/3 Teile 8/9`. Bild
  `ZA_belege/port_vorher_1140_leon_re2_hybrid.png` + Log (`slot 4 typ 11 idx 1 Teile 2`, `slot 5 typ 11 idx 2 Teile 8/9`):
  der linke dicke Zombie traegt Ober- und Unterarm als duenne Stange, der Aermel mit dem RPD-Abzeichen ist ein Stab.
- **Nachher, gleicher Aufbau:** `ZA_belege/port_nachher_1140_leon_re2_hybrid.png`, RE1.5-KI
  `ZA_belege/port_nachher_1140_leon_re15ki_hybrid.png` - volle Arme. Ausschnitt nebeneinander:
  `ZA_belege/vergleich_1140_dicke_zombies_vorher_nachher.png`.
- **Elza:** ROOM1140 wird fuer Elza zu ROOM1141 und hat dort keine Gegner (`port_nachher_1141_elza_re2.log`: keine
  `[enemy]`-Zeile). Zweiter Raum mit dicken Zombies (Zensus `Sce_em_set` Typ 0x11: 1020/1021, 1070/1071, 1090/1091,
  1110/1111, 1140, 11F0/11F1, 1250/1251, 3000/3001, 3010/3011, 30D0/30D1, 30E0/30E1): ROOM1250 Leon/RE2 und ROOM1251
  Elza/RE1.5-KI, `ZA_belege/port_nachher_1250_leon_re2_oben_1251_elza_re15ki_unten.png` (Kamera 1, Zombies klein am
  linken Rand).
- Die Variante haengt an keinem Gehirn und keiner Figur: beide Gehirne rufen re15_enemy_ai_live_init (RE1.5 Zustand 0,
  RE2 re2z_init), der Riegel deckt alle fuenf Typen ab.

Andere Zombie-Typen: dieselbe Variante trifft sie im Port gleich (0x10/0x16 bekamen Arm-/Bein-Varianten); da das
Original sie fuer niemanden zeigt, gibt es keinen Grund, sie dort zu behalten - abgeschaltet fuer alle (Regel des
Koordinators).

## 5. Riegel
`unit_za_spawnvariante` (`re15_port/tests/unit/test_za_spawnvariante.c`): Typen 0x10/0x11/0x12/0x16/0x18 je 400 Seeds:
Teilschicht aus, jedes Teil-Wort 1, Skala 0x1000, keine Tinte (PIN 1); naechster Wurf nach dem INIT = Hand-Erwartung
mit beiden Varianten-Wuerfen (PIN 2); Varianten-Wurf 0 in ~25 % (PIN 3, sonst prueft PIN 1 nichts).
- Gegenprobe gegen master (enemy_ai_common.c auf HEAD zurueckgesetzt, gebaut, Test laufen lassen): **PIN 1 rot** fuer
  alle fuenf Typen (`Teilschicht in 99 von 400 Spawns an`), PIN 2 und PIN 3 gruen -> der Riegel faengt die Variante,
  und der Zufallsstrom des Fix ist identisch mit dem von master.
- Nach dem Fix: `ctest -R "za_spawn|zombie_gore|1030_stuck_who|re2z|re2_gore80"` 30/30 gruen (u.a. unit_zombie_gore,
  unit_1030_stuck_who, alle unit_re2z_*, unit_r37_re2zombie_*, integration_r37_re2zombie, unit_re2_gore80); dazu
  `ctest -R "r38_montage|elzaems|brand|1140"` 35/35 gruen.

## 6. Inventur: Aussehen der Gegnermodelle beim Spawn / im Normalzustand (ohne Treffer/Tod)
Gesucht: alle Stellen, an denen der Port Modell, Geometrie, Skala, Tinte oder Variante eines Gegners setzt, bevor er
getroffen wird (grep `re15_teile_markieren`, `.scale[`, `re15_bf_emd_datei`, Modell-Lader main.c). Treffer-/Tod-Gore
(zombiegore_r15.c, 80%-Zerfetzen enemy_ai_re2_zombie.c:7713, Spinnen-Bein, Birkin-Opfer) ist ausgenommen.

| # | Was | Ort | Beleg im Original | Stand |
|---|-----|-----|-------------------|-------|
| 1 | Spawn-Variante: duenne + verfaerbte Glieder, 25 % aller Zombies | enemy_ai_common.c re15_enemy_ai_live_init | Code ja, **sichtbar nein** (Abschnitt 2b) | **abgeschaltet (diese Spur)** |
| 2 | Modellherkunft "RE2" (Default `re15_ai_models()`): vom RE2-Gehirn gefuehrte Typen (`re15_re2_owns_type`) mit RE2-Retail-EMD (z.B. EM010/EM011/EM016) | enemy_ai_re2_zombie.c re15_ai_models, main.c pc_enemy_load_ex | nein, Port-Option (Menue "RE2 AI + Model") | gemeldet, nicht geaendert |
| 3 | Hybrid "RE1.5-Modelle": RE1.5-Geometrie + TIM unter RE2-Skeletthierarchie, Haltung/Gang aus RE2-Clips, Bind-Laengen RE1.5, Wurzelhoehe angeglichen | main.c pc_enemy_hybrid_re15_models (~Z.977), re2_ems.c re2_hybrid_apply / re2_hybrid_perm | nein, Port-Option (Welle G); Geometrie selbst ist Original | gemeldet; die Nutzer-Session lief so (`Hybrid EM16/EM10/EM11`) |
| 4 | Beta-Birkin "Hangar" EM30_HANGAR.EMD (50A0/50A1) und Bahnsteig-Form (5090) | birkin_formen.c re15_bf_emd_datei | nein, Nachbildung nach 80%-Video (im Kopf so markiert) | gemeldet |
| 5 | Roter Affe EM29_ROT.EMD in 4080/4081 (nur CLUT umgefaerbt) | birkin_formen.c:28 | nein, nach 80%-Video 1 4:11-4:19; `RE15_KEIN_ROTER_AFFE=1` = Auslieferung | gemeldet |
| 6 | Baby-Spinne 0x26 mit RE2-Modell | main.c:7872 `pc_enemy_load_ex(0x26, 1)` | nein (RE1.5-0x26 ist der Feuer-Emitter) | gemeldet |
| 7 | Alligator mit RE2-Modell, RE1.5-Modell (CDEMD0 Block 25) nur mit `RE15_GATOR_ORIGINAL` | enemy_ai_boss_gator.c re15_gator_original | nein, Port-Option | gemeldet |
| 8 | Brennende Zombies (Flammen-FX an acht Teilen) | brandzombie_r15.c | ja, INIT @0x801008f4 bei Flag 1/31; Uebertragung ins RE2-Gehirn ist GESTALTUNG (`re15_brand_re2_takt`) | gemeldet (FX, kein Modellumbau) |
| 9 | Elza-Szenario: Modelle aus CDEMD1.EMS | roy_pc.c re15_roy_pc_emd | ja (`bltz` @0x80022334) | in Ordnung |

Bilder Port gegen Original: nur fuer Punkt 1 erstellt (Abschnitt 4). Kein weiterer Punkt ist ein Stummel-artiger
Geometriefehler; 2-7 sind bewusste Port-Optionen/Nachbildungen und werden nur gemeldet.
