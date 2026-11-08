# Runde 41 - Spur G2-RE2 - MESSBLATT NB4 (Messer, 2026-11-07): Trefferreaktion wie RE2 (T), Stoss von der Rampe ins Wasser (W), kein Kanister-Holmodus (K)

Baum `.claude/worktrees/r41_gatorfix`, Zweig `r41/gator_re2`, Kopf 3d57083b1 (Abnahme 6 = NB3 Runde 2 BESTANDEN). Rolle: Messer - Ist-Stand, RE2-Vorbild,
Riegel und Bauplan VOR dem Bau. Kein Spiel- und kein Testcode geaendert; dieser Commit bringt nur dieses Blatt und `G2_belege/nb4/` (Messdateien `mess_*`,
Werkzeuge `werkzeug/`, Lauf-Auszuege `laeufe/`, Harness-Ausgabe `harness/`).

Vorgaben (bindend, Wortlaut aus dem Auftrag):
- **(T)** Fuer JEDEN Alligator-Angriff, der die Figur trifft (Biss im Wasser, CROSS-Biss, Rammen, Hochbiss, Biss mit Kanister), gilt die Trefferreaktion der
  Figur aus RE2 (Weg, Richtung, Dauer, Animation, Fallen/Taumeln, Schaden, Unverwundbarkeit) statt des RE1.5-Niederwurfs (Orchestrator-Entscheid nach
  "Original oder nicht"; Memory reai-v2-original-oder-nicht, reai-v2-re2-ki-nur-angriffsmuster-gore-max, reai-v2-gelobter-stand-ist-referenz).
- **(W)** Nutzer 2026-11-07 ~15:00 woertlich: "er kann den Spieler ruhig In das Wasser stoßen." Ein Treffer oben an der Rampenkante darf die Figur von der Rampe
  ins Wasser stossen; Band 0 (nie durch die Aussenwaende aus dem Raum) bleibt.
- **(K)** Nutzer 2026-11-07 ~15:00 woertlich: "Die Frage stellt sich garnicht, da leon mit den alligator ganz normal seine runden drehen soll. Das heißt der
  alligator sollte Ganz natürlich irgendwann zum Kanister kommen, während er leo. Auf dem rundweg verfolgt...." -> kein eigener Kanister-Holmodus; Aufnahme
  nur, wenn der Kopf auf dem Verfolgungsweg in Aufnahmereichweite kommt (RE2-Aufnahmestufen 1-6 bleiben); kommt er nie vorbei, bleibt der Kanister liegen.
- Bleibt: Gang ohne Kanister bitgleich NB5 (Riegel gang/gang_elza), Vorgabe A (oben nicht sicher) und B (kein Haenger) aus NB3, Raumhalt Band 0,
  Kanister-Sitz am Kiefer Versatz 0, Rueckfall nur aus dem Haenge-Fenster.

## 0. Messwege, Laeufe, Belege

- exe: `re15_port/build/platform/pc/re15_pc.exe` md5 **5910fc24**6fc82eaaac9f64795492585a = die exe der Abnahme 6 (`local_build.sh build`: `ninja: no work to
  do.` / `=== LOCAL-BUILD-OK (build)`). Messkopie `re15_pc_g2nb4.exe` (gleiche md5), Skript `werkzeug/lauf4.sh`, nur eigene PIDs, kein gdigrab, Bilder nur als
  RE15_FRAMEDUMP-Serie. Laufordner (nicht im Baum) scratchpad `g2nb4/runs/<lauf>/`.
- **Zwei eigene exe-Laeufe** (Umgebung wortgleich den Abnahme-6-Laeufen, nur RE15_FRAMEDUMP-Fenster anders; `laeufe/<lauf>/env.txt`, `pid.txt`):
  `nb4_oben_leon` (Riegel-Szenario rampe_oben, Kanister nach der Szene im Becken, Cut 2, 4400 Bilder, exit 0) und `nb4_frei4_leon` (Abnahme-4-Rundweg frei4,
  8400 Bilder, exit 0). state.log, climb.log, gator_boss.log, gator_trag.log beider Laeufe sind **bytegleich** zu den Abnahme-6-Laeufen neu_oben_leon /
  neu_frei4_leon (cmp) - der Stand ist deterministisch, die 46 Abnahme-6-Laeufe (scratchpad `g2nb3/abn6/runs/`, exe 5910fc24) gelten als Messgrundlage mit.
- **Harness**: `test_r41_gatorfix raum` (Messkopie `test_r41_gatorfix_g2nb4.exe` md5 197d1ead, 16 Lagen x 2400 Bilder mit/ohne Kanister, echter Boss-Takt,
  Figur frei mit Niederwurf/Schub/Klemme): `raum: 0 Fehler`, exit 0, Lagenzeilen gleich der Abnahme 6 (`harness/mess_raum_harness.txt`).
- Baseline Riegel: `ctest -R "^unit_r41_gatorfix_" -j 2` auf diesem Stand (Abschnitt 7).
- Werkzeuge (`werkzeug/`): `treffer4.py` (je Treffer Port heute gegen RE2-Modell), `agg4.py` (Summe), `re2_modell.py` (RE2-Hook Bild fuer Bild),
  `reichweite.py` (Kanister-Reichweite auf dem Verfolgungsweg), `kanister4.py` (Kanister-Lebenslauf je Lauf), `rbj40a0.py` / `rbj2090.py` (Raum-Animationsbanken),
  `pld_edd.py` / `pld_cmp.py` (Clip-Laengen, Keyframe-Vergleich RE2/RE1.5 PL00), `raster4.py` (Bildraster aus RE15_FRAMEDUMP).
- RE2-Quellen: Overlay `info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN` (Basis 0x80100000), Disassembly `analysis/befunde_runde41/herkunft/alligator_werkzeuge/re2_em23.dis`
  (Auszuege `mess_re2_em23_treffer_kanister.dis`); EXE `RE2_Quellcode_V2/FUN_*.c` + `ghidra_re2_Leon.txt` (Auszuege `mess_re2_exe_helfer.txt`);
  `info/re2leon/PL0/RDT/ROOM40A0.RDT`, `info/re2leon/PL0/PLD/PL00.PLD`.

## 1. (T) Trefferreaktion

### 1.1 RE2: es gibt genau EINEN Treffer-Einstieg fuer die Figur - der Gator stellt die Reaktion selbst als Spieler-Haken

Selbst disassembliert (`mess_re2_em23_treffer_kanister.dis`):
- **Treffer-Einstieg 0x80101578**, je Bild aus dem Root aufgerufen (`jal 0x80101388` @0x80100328 -> `jal 0x80101578` @0x8010155c). Bedingungen: Spieler-Halbwort
  DAT_800cfc06 != 0 (`lhu v0,0(a1) / beq v0,zero` @0x80101590-98, gesetzt von der EXE-Kollision) und **PL+0x1D3 == 0** (`lbu v0,-565(v0) / bne` @0x801015a4-ac).
  Spielerstruktur = 0x800CFBF8 (`addiu a0=>DAT_800cfbf8,s3,0x3a10` @0x800360d0 ghidra_re2_Leon; s0 = a1 - 14 @0x801015b0).
  - PL+0x1B4 = Gator (`sw s1,-596(at)` @0x801015b8-bc), **PL+4 = 0x205** (`addiu v0,zero,517` / `sw v0,-1028(at)` @0x801015b4/@0x801015c0-c4) = Spieler-Routine 5,
    Unterzustand 2, Phase 0.
  - Unterzustand 3 statt 2, wenn FUN_80015910(Gator, Spieler) wahr ist (`jal 0x80015910` @0x801015c8, `sb v0,-1027(at)` +1 @0x801015d8-ec);
    FUN_80015910 = `((pl.gier - gator.gier) + 0x400) & 0xfff < 0x800` (RE2_Quellcode_V2/FUN_80015910.c) = **die Figur blickt wie der Gator = der Gator steht
    hinter ihr**. Sonst (Figur sieht den Gator an) Unterzustand 2.
  - **Richtung**: Achse a1 in {0, 1024, 2048, 3072} nach der Gator-Lage im RE2-Korridor (`lw v0,64(s1) / slti v0,v0,-8999` @0x801015f0-f8; `lw v0,56(s1) /
    slti v0,v0,-15599` @0x80101628-30) und erneut FUN_80015910 (@0x80101604/@0x80101640/@0x80101664): z >= -8999 -> wahr 0 / falsch 2048; x < -15599 -> wahr 3072 /
    falsch 1024; sonst wahr 2048 / falsch 0. Dann **FUN_8001569c(Spieler, Achse, 1536)** (`jal 0x8001569c` / `addiu a2,zero,1536` @0x80101684-88): dreht
    PL+0x76 (Gier) auf die Achse, wenn sie innerhalb +-1536 liegt, sonst um 1536 (RE2_Quellcode_V2/FUN_8001569c.c). Die Paarung "wahr -> a, falsch -> a+2048"
    heisst: der Stoss laeuft in beiden Unterzustaenden **laengs derselben Achse a** (Sub 2 rueckwaerts aus Gier a+2048, Sub 3 vorwaerts aus Gier a). Lesart: a ist
    die Korridorachse des Abschnitts, in dem der Gator schwimmt (Bahn x -23078 `addiu v0,zero,-23078 / sw v0,56(s0)` @0x80101048-4c, Kanister-Fenster laengs z
    obj[1].z - z @0x80100ad0-ec) - nicht weiter belegt. Belegt ist: nur die vier Achswerte werden je uebergeben.
  - **Schaden**: `lhu v0,536(s1) / andi v0,v0,0x20 / beq ... addiu a0,zero,60 / addiu a0,zero,30` @0x8010168c-a0 -> `jal 0x801038e4` @0x801016a4 ->
    **FUN_800401d4(Schaden, 0)** (`jal 0x800401d4 / addu a1,zero,zero` @0x80103918-1c, einziger Aufruf im Overlay): **60**, ab Gator+0x218 Bit 0x20 **30**;
    das Bit setzt der Gator, sobald seine HP +0x156 < 25000 (`lh v0,342(s0) / slti v0,v0,25000 / ... ori v0,v0,0x20 / sh v0,536(s0)` @0x80101ebc-dc; HP-INIT
    25000 + Tafel `addiu v0,v0,25000 / sh v0,342(s0)` @0x8010057c-80). Rueckgabe Bit 0 (One-Save) -> PL+6 = 4 (@0x80103924-34), Bit 1 (tot) -> PL+4 = 3
    (@0x80103938-48). Davor zwei Eintraege in die Queues 0x800eaad8 / 0x800eac28 (FUN_8003947c(3,0) @0x801038f8, FUN_800395b8(14,210,40,2) @0x8010390c) =
    Vibration, ohne Port-Gegenstueck, nicht Teil der Figur-Reaktion.
  - Gator danach: Biss-Sperre **+0x238 = 70** (`addiu v0,zero,70 / sb v0,568(a0)` @0x801016b0-b4, Port re15_gr2_biss_cd), +0x231 = 0 und FUN_80103d80(Gator)
    (@0x801016b8-bc; Gator-eigener Effekt mit atan2 0x800154ac Spieler->Gator, nicht die Figur).
- **Spieler-Haken**: der Gator traegt bei INIT 0x800CE300[0x23] = **0x80102D08** ein (`lui v0,0x8010 / addiu v0,v0,11528 / sw v0,-7284(at)` @0x801004f4-500);
  die EXE-Routine 5 (0x8004006C, Verteiler `jalr 0x800CE300[Greifer.typ]`, Herleitung enemy_ai_tentakel_g5.c Kopf "SPIELER-ROUTINE 5") ruft ihn. 0x80102D08:
  `lbu v0,5(a0)` -> Tafel @0x80104690 = [0] 0x80102D44, [1] 0x80102F48 (die anderen Unterzustaende, nicht Teil dieser Messung), **[2] 0x8010307C, [3] 0x801031F8**
  (die Treffer-Reaktion; der Treffer-Einstieg schreibt nur 2 oder 3).
- **Unterzustand 2 = 0x8010307C** (Tafel @0x80100074, 5 Phasen: 0x801030BC, 0x80103108, 0x80103150, 0x80103164, 0x801031C8):
  - Ph0: SE **0x04010001** an der Figur (`lui a0,0x401 / ori a0,a0,0x1 / jal 0x8005ba28` @0x801030bc-c8), Clip-Wort **0x00030012** = Clip 18, Blend 3
    (`sw v0,332(s1)` @0x801030cc-d4), **+0x144 = -800** (@0x801030e0-e4), +0x146/+0x148 = 0, **+0x1D3 |= 0x80** (gesperrt, @0x801030e8-100), +0x1C0 |= 0x12
    (Nacken aus), faellt in Ph1.
  - Ph1: Blend +0x14E == 0 -> Ph 2 (@0x80103108-1c); FUN_800152c8(pl,0) = Lage += RotY(Gier)*(+0x144,0,+0x148) (@0x80103120,
    RE2_Quellcode_V2/FUN_800152c8.c); FUN_8002959c(pl, +0x190, +0x194, 1024) (@0x8010312c-34); **+0x144 += 150** (@0x8010313c-4c).
  - Ph2: Clip-Wort **0x00030013** = Clip 19 (@0x80103150-58), Ph 3. Ph3: Vorschub, Clip **doppelt** (@0x80103170-98), **+0x144 += 15, > 0 -> 0** (@0x801031a4-c4).
  - Ph4 = Ende: PL+4 = 1 (Routine 1), **+0x1D3 = 0**, +0x1C0 &= 0xED (@0x801031c8-dc).
- **Unterzustand 3 = 0x801031F8** (Tafel @0x8010008c, 7 Eintraege: 0x80103238, 0x80103284, 0x801032DC, [3] Ende, 0x801032DC, 0x801032F4, 0x80103340):
  - Ph0: SE **0x04020001** (@0x80103238-44), Clip **20** Blend 3 (0x00030014 @0x80103248-50), **+0x144 = +800** (@0x8010325c-60), +0x1D3 |= 0x80, +0x1C0 |= 0x12.
  - Ph1: Vorschub, Clip 20 einfach (Ende -> Ph2), **+0x144 -= 120, < 0 -> 0** (@0x801032b4-d8).
  - Ph2: Clip **17** Blend 3 (0x00030011 @0x801032dc-e4), Ph 5, **+0x1D3 = 0** (@0x801032f0: ab hier wieder treffbar), faellt in Ph5: Clip 17 doppelt
    (@0x801032f4-1c), Ende -> Ph 6 mit Clip-Wort **0x00030006** (@0x80103328-3c).
  - Ph6: FUN_8002959c(pl, **+0x108, +0x17C** = Basisbank, a3 = **0x10400** = rueckwaerts) (@0x80103340-54); Ende -> PL+4 = 1, +0x1D3 = 0, +0x1C0 &= 0xED
    (@0x80103358-70).
- **Animationsbanken** (gemessen):
  - PL+0x190/+0x194 schreibt nur FUN_80018e08 (`sw v0,0x0(t5)=>DAT_800cfd88` @0x80018ea4, `sw v0,0x4(t5)=>DAT_800cfd8c` @0x80018eb8; ghidra_re2_Leon) aus
    RDT+0x60 = die **Raum-Animationsbank** (RBJ). +0x190 = EMR, +0x194 = EDD (FUN_8002959c liest EDD[clip] ueber param_3 = +0x194).
    ROOM40A0.RDT: RDT+0x60 -> 0x65e8, 2 Saetze: **Satz 0 Flags 0x1 (Leon) 21 Clips, Clip 17 = 16, 18 = 10, 19 = 30, 20 = 21 Bilder**; Satz 1 Flags 0x2 (Claire)
    nur 16 Clips (`mess_rbj40a0.txt`). EMR-Kopf Satz 0 = (0, 8, 15 Knochen, 80) = derselbe Kopf wie shared_assets/RE2/VENTIL1080.RBJ, das der Port fuer 2030/2031
    schon als Spieler-Overlay fuehrt (ventilkette_r41_pc.c re15_ventilkette_r41_pc_rbj).
  - ROOM2090.RDT / ROOM2091.RDT haben **kein** eigenes RBJ (RDT+0x5C = 0, `mess_rbj40a0.txt`).
  - Basisbank-Clip 6 (RE2 PL00.PLD, 25 Bilder) ist **Keyframe fuer Keyframe bytegleich RE1.5 PL00 Clip 11** (25 von 25; ebenso RE2 3/4/5 = RE1.5 8/9/10,
    Gegenprobe RE2 6 gegen RE1.5 12: 0 von 25; `mess_pld_vergleich.txt`). RE2s "Clip 6 rueckwaerts" ist also genau das Aufstehen des RE1.5-Niederwurfs
    (kd_clip(pl, 0x0b, 1), game_step_common.c Ph9/Ph4).
- **Bild fuer Bild** (`werkzeug/re2_modell.py` -> `mess_re2_modell.txt`; Blend +0x14E -1 je Aufruf solange != 0 @0x800299c0-cc, Bild +1 / Wrap / Rueckgabe 1
  @0x80029b28-4c):

| | Unterzustand 2 (Figur sieht den Gator an) | Unterzustand 3 (Gator hinter der Figur) |
|---|---|---|
| Weg | **3735 rueckwaerts**: 800, 650, 500, 350 (Clip 18, Blend 3 = 4 Bilder), dann 200, 185, ..., 5 | **3080 vorwaerts**: 800, 680, 560, 440, 320, 200, 80 |
| Dauer bis Routine 1 | **20 Bilder** (Clip 18 4 Bilder, Clip 19 doppelt 15 Bilder, Ende) | **54 Bilder** (Clip 20 21, Clip 17 doppelt 8, Basis-Clip 6 rueckwaerts 25) |
| Haltung | Taumeln rueckwaerts, faellt nicht (kein Liege-Clip) | wird nach vorn geworfen, liegt (Clip 17), steht auf (Clip 6 rueckwaerts = RE1.5 Clip 11 rueckwaerts) |
| gesperrt (+0x1D3 & 0x80) | Bild 0-19 (ganze Reaktion) | Bild 0-20 (bis zum Liegen), ab Clip 17 wieder treffbar |
| SE | 0x04010001 | 0x04020001 |
| Schaden | 60 / 30 (Gator-HP < 25000) ueber FUN_800401d4 | dto. |

- Wand/Kante: der Haken selbst hat **keine** Wand-Sonde; die Lage geht durch die EXE-Spielerklemme nach dem Entity-Pass (FUN_8003567c, @0x80026634, Port
  re15_collision_constrain_re2). RE2s Alligator-Raum hat **keine erhoehte Flaeche** (Messblatt NB3 Abschnitt 3) - Kante/Sturz kennt RE2 hier nicht.

### 1.2 Port heute (enemy_ai_boss_gator.c / game_step_common.c @3d57083b1)

Alle Angriffe laufen ueber `gb_biss_abschluss` (enemy_ai_boss_gator.c:452): `re15_player_take_damage(pl, GB_BITE_TYPE 5, ...)` = DAT_8006f418[5] = **50**
(@0x8006f422), dann `re15_player_knockdown_begin(re15_ai_facing_dir(e, pl))` (:483) = RE1.5-Niederwurf cmd-2 [4] (von vorn, Clips 0xd/0xe(/0xf/0x10)/0xb rw,
**s_kd_speed 1000** `ori v0,zero,0x3e8` @0x8003615c, -15*Bild @0x800361c8-e0) bzw. [5] (von hinten, Clips 0xc/0x10/0xb rw, 500 @0x800364c0)
(game_step_common.c:486-498, :728-850), `pl->hit_react |= 1`, `e->hit_stun = 0x64`, Biss-Sperre re15_gr2_biss_cd 70 (@0x801016b0-b4). Die Aufrufer:

| Angriff | Stelle | Phase |
|---|---|---|
| Biss im Wasser (LUNGE, Maul <= 1500 / dist < 1000) | enemy_ai_boss_gator.c:2457 | GBP_LUNGE (3) |
| Biss in der Messvariante B2 (RE2-Dash-Fenster Clip 3 f >= 114) | :2430 | GBP_LUNGE, nur env RE15_GB_LUNGE_B2 |
| Hochbiss (Clip 4, Bild 6..34, dist < 3900) | :2823 | GBP_GUARD (5) |
| Rammen (Koerper-Grundriss, einmal je Querung) | :2995 | GBP_CROSS (7), RAMMEN-Zeile in gator_boss.log |
| CROSS-Biss (Maul <= 1500) | :3005 | GBP_CROSS |
| Biss mit Kanister | dieselben Stellen (Kanister traegt seit NB2 nicht zur Bewegung bei) | - |

Der Wegstoss selbst: kd_move (game_step_common.c:574-586) -> spieler_klemme (:524-534: FUN_8003b0a4 + `re15_gator_boss_raum_halt` :532) + Objekte + Sonde.
**RE2-Spieler-Trefferreaktion im Port heute**: nicht vorhanden. Wiederverwendbar: FUN_800401d4 = `re15_re2_player_damage(_mode)` (enemy_ai_re2_zombie.c:860-893,
One-Save-Latch geteilt mit Zombie/Hund/Kraehe; rohe RE2-Schadenszahlen 1:1 wie dort dokumentiert), FUN_8001569c = Port-Zwilling enemy_ai_boss_g5.c:1068
(und gatorszene_r41.c:768 ga_drehen), FUN_8002959c-Zaehler = `re15_re2_advance_959c` (enemy_ai_re2_dog.c:209), RE2-RBJ als Spieler-Overlay = Muster
ventilkette_r41_pc.c:178-193 + main.c:4430-4431 / :8506-8507, SE core 1/2 = `re15_audio_core_se(1/2)` (wie re15_player_stagger_cmd2). Die anderen RE2-Gegner
(Zombie/Hund/Kraehe/Spinne) fuehren ihre Spieler-Reaktion ueber eigene Opfer-Maschinen bzw. den RE1.5-Stagger - eine RE2-Reaktion "Routine 5 Sub 2/3 aus der
Raumbank" gibt es noch nirgends.

### 1.3 Port heute gemessen

`mess_treffer_exe_nb4.txt` / `_summe.txt` (eigene Laeufe) und `mess_treffer_exe_abn6.txt` / `_summe.txt` (9 Abnahme-6-Laeufe mit RE15_G2_OHNE_KANISTER=1:
oben_leon/elza, west4_leon/elza, west5_elza, frei4_leon/elza, kante_-2300_-16000, kante_100_-20900). Weg/Luftlinie nur in den Niederwurf-Bildern (Spieler-Clip
11..16), Dauer = diese Bilder. Art aus gator_trag.log-Phase (nur ab der Aufnahme vorhanden; davor "?"). RE2-Unterzustand mit FUN_80015910 auf die Gieren
im Trefferbild.

| Ort / Angriff | Treffer | heute Weg max / Luftlinie max / Mittel | heute Dauer | RE2 (Unterzustand) |
|---|---|---|---|---|
| Wasser, Biss (LUNGE) - 9 Laeufe | 198 | 18 026 / **11 312** / 1 673 | 0..92 Bilder | Sub 2 81x (3735 / 20), Sub 3 117x (3080 / 54) |
| Wasser, Biss - nb4_frei4_leon | 59 | 18 026 / **11 268** / 2 530 | 41..92 | Sub 2 19x, Sub 3 40x |
| oben, Hochbiss (GUARD) - 9 Laeufe | 25 | 5 917 / 4 918 / 1 658 | 41..92 | Sub 2 8x, Sub 3 17x |
| oben, Rammen | 14 | 5 811 / 4 733 / 1 982 | 41..92 | Sub 2 6x, Sub 3 6x (2 vor der Aufnahme) |
| oben, CROSS-Biss | 11 | 2 369 / 556 / 318 | 41..92 | Sub 2 6x, Sub 3 5x |

- RE1.5-Dauer 41 (von hinten [5]), 67 (von vorn [4] ohne Slam), 92 Bilder (mit Wand-Slam Clip 0xf) gegen RE2 20 bzw. 54 Bilder; Weg im Wasser bis 11 312
  Luftlinie (Abnahme 6: 13 059 bis zum Stillstand) gegen RE2 hoechstens 3735.
- Bild: `mess_frei4_leon_biss_F1186-F1240_hell.jpg` (nb4_frei4_leon, Biss F1189 an (-3902,-5868), Clip 13/14 von vorn, Leon fliegt bis (4500,-12531) =
  10 776 in 41 Bildern; Cut 0 zeigt das leere Nordbecken - die Figur ist ausserhalb des Kamerabilds, Beleg sind die Lagezeilen unter den Bildern).
  `mess_oben_leon_rammen_F1336-F1384_hell.jpg` (nb4_oben_leon, Rammen F1339 an (6732,-15855), Clip 12 = Niederwurf von hinten, Leon rutscht bis (4971,-17632)
  = Klemmlage vor der Suedleiste [9] und liegt dort an der Deckkante).

## 2. (W) Rampenkante

### 2.1 Raumdaten (ROOM2090.RDT = ROOM2091.RDT, md5 a48d11a4; SCA-Kopf @0x6dc, `G2_belege/abn6/sca_2090.txt`, Abnahme 6 Abschnitt 6 nachgelesen)

- Deck = Rampe [5] @0x730 x 1850..7200 z -18100..-14500 (Band 0, u1 **0x01** = Aufwaerts-Bit, Klettern).
- Band 1: [6] @0x73c Ostwand x 7200..9200 (u1 0x00), [7] @0x748 Plattform x -1700..1850 z -20000..-12450 (u1 0x00) = **Waende**; [8] @0x754 Nordleiste
  z -14500..-13500 und [9] @0x760 Suedleiste z -19100..-18100, je x 1850..7200, u1 **0x02**, floor 0x10 = **Absprung-Kanten**: die Abwaerts-Sonde verlangt genau
  dieses Bit (`if (!(s6 & 2u)) return 0;` climb_common.c:304, @0x8002d74c-50, kind = s6 & 0x0e @0x8002d780).

### 2.2 Wie die Figur heute an der Kante stoppt (Code + Messung)

- **Leisten-Klemme**: FUN_8003b0a4/FUN_8003bca8 schiebt eine Lage in [8]/[9] auf Kante +- (450 + 18) zurueck (DAT_80073e94+6 = 450 `lhu a1,6(v0)` @0x80031d6c;
  `addiu t0,v0,0x12` @0x8003bd20, `addiu a0,a0,-0x12` @0x8003bd2c) - Klemmlagen z -17632 / -14968. Jeder Schritt, der in der Leiste endet, landet wieder auf dem
  Deck. Die um den Radius erweiterte Suedleiste reicht z -17650..-19550, ihre Mitte -18600 liegt 968 hinter der Klemmlage -17632 (Nord spiegelbildlich); ein
  RE2-Schritt ist hoechstens 800 und endet damit in der Deckhaelfte der Leiste - ob FUN_8003bca8 nach der naechsten Kante oder nach der Vorlage +0x40/+0x44
  (game_step_common.c Kommentar ueber @0x8003bd44-48) zurueckschiebt, beides ist die Deckseite: **mit RE2-Schritten allein haelt die RE1.5-Klemme die Figur IMMER auf dem Deck** - (W) braucht einen eigenen Uebergang.
- **Raumhalt Band 1** (re15_gator_boss_raum_halt, enemy_ai_boss_gator.c:791-818; Aufruf game_step_common.c:532 und enemy_ai_boss_gator.c:3338 Boss-Push): nur fuer Schritte, die
  durch die Leiste hindurch gehen (RE1.5-Niederwurf 2062 im ersten Bild): Deck-Rechteck x 2300..6750, z -17650..-14950 (gb_deck_gueltig :780-783), Klemme
  auf GB_PRK = 468 (:768-769). Band 0 (:802-804): Lage ausserhalb Aussenwaende/in Block [4]/[5] -> Lage davor.
- Gemessen: Harness raum, 9 Oben-Lagen x 2400 Bilder: **Deck verlassen 0**, Raumhalt-Eingriffe 1..53 je Lage (gesamt 385 mit Wasser-Lagen;
  `harness/mess_raum_harness.txt`). exe nb4_oben_leon: 20 Treffer oben, 0 RAUMHALT-Zeilen - die Figur bleibt durch die Leisten-Klemme (Bild F1344-F1384:
  liegt an z -17632).

### 2.3 Wie ein Abgang von der Rampe ins Wasser im Port heute funktioniert (einziger Weg: Abstieg per Aktionstaste, climb_common.c)

- Sonde (re15_climb_try_start, climb_common.c:211-315): Punkt 800 voraus auf 90 Grad gerastet (@0x8002d4d4-28), Zellwort Bit 0x2 (@0x8002d74c-50), Boden darunter tiefer
  (`re15_collision_room_coll` @0x8002d764-7c) -> Substate 9 -> 10.
- **Substate 10 = Herunterspringen** (climb_sub10, climb_common.c:472-570, alle Konstanten mit Adresse): Ph0/1 Clip D0 (+0x94 = 3) bis Bild 0x0d (@0x80038368-bc); **Ph2**:
  Clip D1 (+0x94 = 4), Tempo 800 (@0x80038408-10), Vorschub, **y += 500** (@0x8003843c-44), **Boden h = room_coll(x, z, Band)** (@0x80038448), **Band =
  -(h / 1800)** (@0x80038460-bc), SCA-Klemme (@0x800384c4), Objekt-Pass; **Ph3**: **y += 90 + 50 t** je Bild (@0x80038660-9c) bis h - 800 < y; **Ph4/5**:
  Landeclip 5/6 (@0x800386f4-708), **y = h** (@0x80038724-2c), Spritzer (Spur A, @0x80038758-cc), +0x93 frei; Ph6 Ende.
- Gemessen (Abnahme-6-Lauf neu_west4_elza, climb.log T2502-T2539, `mess_abstieg_west4_elza.txt`): Nordkante (6732,-1800,-14968) Band 1 -> Ph2 (6732,-1210,
  -14032) **Band 0** -> -1070, -880, -640 -> Landung **(6732,0,-14032)** in 4 Fallbildern; 38 Bilder gesamt. Die Klemme auf Band 0 schiebt dabei vom Rampenblock
  [5] auf -14500 + 468 = -14032.

### 2.4 Was fuer (W) passieren muss

Ein Treffer-Stoss traegt die Figur ueber die Kante, wenn ein RE2-Schritt die Lage aus dem Deck in eine Band-1-Zelle mit Abwaerts-Bit ([8] Nord / [9] Sued)
fuehrt. Dann muessen in DIESEM Schritt (statt Leisten-Klemme + Raumhalt Band 1) die Fallschritte des Substate 10 ab Ph2 greifen: y += 500, Boden aus
room_coll, Band = -(h/1800) VOR der Klemme (sonst schiebt [8]/[9] zurueck), Klemme auf Band 0, dann y += 90 + 50 t bis zur Landung y = h, Spritzer. Ostwand [6]
und Plattform [7] (u1 0) bleiben Wand (Klemme wie heute). Band 0 Raumhalt bleibt (nie aus dem Raum, nie in [4]/[5]). Die waagrechten RE2-Schritte laufen
weiter (RE2 hat keinen Sturz; die Fall-/Landeanimation ist PORT-WAHL, Abschnitt 6 O-W1).

Wie oft das passiert (Modell auf die gemessenen Treffer, `mess_treffer_exe_*.txt` Spalten A/A90; RE2-Schritte der Abschnitt-1.1-Tabelle ab der Trefferlage,
"Wasser" = Mittelpunkt jenseits der Kantenlinie z -18100 / -14500, Wand = 450 vor x 7200 / 1850):

| Richtung (PORT-Lesart, Abschnitt 6 O-T1) | Wasser Sued | Wasser Nord | Ostwand [6] | Plattform [7] | bleibt oben |
|---|---|---|---|---|---|
| A = Gator-Gier unrastet, 9 Abnahme-6-Laeufe (48 Oben-Treffer mit Gator-Gier) | 20 | 5 | 12 | 10 | 1 |
| **A90 = Gator-Gier auf die naechste der vier RE2-Achsen**, dto. | **30** | **11** | 1 | 5 | 1 |
| A90, nb4_oben_leon (18 Oben-Treffer mit Gier) | 11 | 5 | 0 | 2 | 0 |

Die meisten Stoesse erreichen die Kante im 1.-4. RE2-Bild (Trefferlagen liegen an den Klemmlagen z -17609..-17632 / -14959..-14968).

## 3. (K) Kanister

### 3.1 RE2

- Zielmodus +0x1C0 & 4 (nur Zone 1, +0x22C == 0, `lbu v0,556(s0)` @0x80100a0c-14): Bank 5 Bit 1 (`jal 0x80077360`, a1 = 1 @0x80100a1c-2c) und Stufe < 2
  (`lbu v0,550(s0) / sltiu v0,v0,0x2` @0x80100a34-3c) -> `ori v0,v0,0x4 / sb v0,448(s0)` (@0x80100a48-5c), Halsziel Kanister (x, y - 800, obj[1].z)
  (@0x80100a4c-70); aus, sobald obj[1].z - z < 9000 - 120 (`addiu v0,v0,-9000 / slti v0,v0,-120` @0x80100a74-8c) und Stufe 0 (@0x80100a98-b4). Wirkung: Hals
  zum Kanister und Biss-Sperre T1 (`lbu v0,448(a0) / andi v0,v0,0x4 / bne` @0x8010134c-58). **Der RE2-Gator lenkt den Koerper nie** (Gier-Schreiber nur
  @0x80100280/2a0/948/ec0/2404/2440/27c8/2a34): er schwimmt die Korridorbahn, und der Kanister liegt AUF ihr (Liegestelle (-22816,-1984,-9500) ROOM40A0 sub05
  Pos_set @SCD 0x0484, Bahn x -23078 @0x80101048-4c = 262 daneben).
- **Aufnahme Stufe 1**: Bank 5 Bit 0 (`jal 0x80077360`, a1 = 0 @0x80100ab8-c8) und **obj[1].z - z - 8601 < 0x18f** (`lui v0,0x800d / lw v0,1372(v0) / lw v1,64(s0) /
  subu / addiu v0,v0,-8601 / sltiu v0,v0,0x18f` @0x80100ad0-ec) und Stufe 0 (@0x80100af4-fc) -> `sb v0(1),550(s0)` (@0x80100b08); nur laengs, keine Querbedingung
  (Korridor 1 Spur breit).

### 3.2 Port heute (Dateien/Zeilen)

- **RUNDE (E1, PORT-WAHL NB1 "spaetestens rund 30 s")**: kanister2080_r41.c `runde_zaehlen` :988-993 (Aufruf :831), `anschwimm_lage` :994-1017 - liegt der
  Kanister **RE15_K2080_RUNDE_AB 450** Bilder frei (re15_kanister2080_r41.h:232) und voraus, spaetestens nach **RE15_K2080_RUNDE_SPAET 600** (:237), wird er
  Steuerziel bis zur Aufnahme: `re15_k2080_r41_anschwimmen` :1023-1063 (Kanten-Umweg, Slew 0x30, RE2-Vortrieb), Aufruf in der Verfolgung
  enemy_ai_boss_gator.c:2010; **in der Belagerung GUARD :2506 `if (re15_k2080_r41_anschwimmen_aktiv(e)) oben = 0;`** - waehrend der RUNDE gilt eine Figur oben
  als "nicht oben", der Gator verlaesst die Belagerung und holt den Kanister. Das IST die 28-s-Pause.
- Anschwimmen N3 (RE2-Zielmodus als Steuerziel, Grenze RE15_K2080_ANSCHWIMM_AB 8880 :229) ist seit NB1 nur noch Pruefhaken (`s_anschw_alt`).
- Zielmodus-Bit (Biss-Sperre): gator_re2_r41.c `gr2_zielmodus_takt` :586-601 (nur Verfolgung, nur Zone der Liegestelle; an bei Stufe < 2, aus bei laengs < 8880 x
  Skala), wirkt ueber re15_gr2_biss_erlaubt :1347.
- Aufnahme-Fenster (gr2_kan_takt Stufe 0, :611-640): Phase CHASE/LURK, Szenen-Kanister: laengs **[0, (8601 + 0x18f) x 2731/4096) = [0, 6000)** (:617, :624 "vom
  Kopf an", PORT-WAHL NB1), **|quer| < hit_radius_min 1466 + 267 = 1733** (:618, PORT-WAHL Koerperbreite), danach Stufen 1-6 RE2 (:641 ff.).

### 3.3 Gemessen (`mess_kanister_lebenslauf_abn6.txt`, `mess_kanister_reichweite_abn6.txt`)

- **Liegestelle**: in allen 14 Abnahme-6-Laeufen mit Szene **(-6870,-12651)** (Leon) bzw. **(-6842,-12641)** (Elza) = Nord-Wurf (RE15_K2080_WURF_GIER_NORD 3072),
  Nordwest-Becken nordwestlich der Plattform, y -1200.
- **Wer bringt den Gator hin**: in 12 von 14 Laeufen beginnt die **RUNDE** bei F492-F642 (450-600 Bilder nach dem Aufschlag) und endet mit der Aufnahme
  F572-F1001; ohne RUNDE nahm er ihn nur in kante_-500_-21800 (Stufe 1 F372, laengs 5209, quer 1449) und nach 21 Bildern RUNDE in frei4_leon (F558).
  Zielmodus-Bit an in 3 Laeufen (oben_leon/oben_elza F690-F933, frei4_elza F861-F909).
- **Die 28-s-Pause oben** (rampe_oben, nb4_oben_leon = neu_oben_leon): letzter Treffer F483, RUNDE an F492 (Gator (6000,-16704) laesst die Belagerung),
  Zielmodus F690-F933, Stufe 1 F1001, angehaengt F1013, naechster Treffer F1339 (Rammen) = 856 Bilder; davon RUNDE + Aufnahme 509 Bilder, danach 326 Bilder der
  normale Takt.
- **Reichweite auf dem Verfolgungsweg ohne RUNDE/Zielmodus** (`werkzeug/reichweite.py`): Bahn = gator_trag.log ab der Aufnahme in den Laeufen mit
  RE15_G2_OHNE_KANISTER=1 (dort ist die Verfolgung bitgleich der ohne Kanister, Abnahme 6 Abschnitt 3c / NB2 Riegel (a)); Praedikat = Port-Fenster 3.2 gegen die
  Liegestelle, Phase CHASE/LURK. **Rundweg** frei4_leon: 9 Besuche in 7805 Bildern (F698, F816, F921, F1009, F1618, F3002, F3542, F3552, F5705; F698-F1009 noch
  im Nachlauf der RUNDE); frei4_elza: 14 Besuche in 7454 Bildern (F1168..F2956, danach bis F8400 keiner); Lage kante_-2300_-16000 (Elza im Westen): 12;
  kante_100_-20900 (Leon): 3. **Figur an/auf der Rampe** (oben_leon/elza, west4_leon/elza, west5_elza, kante_0/1500/4073/5000 und -1300): **0 Besuche** -
  die Belagerung bleibt an der Rampe, die Wurzel kommt der Liegestelle hoechstens 1981-6264 nahe.
- Lesart: auf dem Rundweg kommt der Kopf "ganz natuerlich" wiederholt an die Liegestelle (erster Besuch ab Freiwerden, je Lauf 3-14 Besuche); wer an der Rampe
  bleibt, holt den Kanister nicht - so verlangt es (K). Ob der allererste Besuch ohne RUNDE frueher oder spaeter faellt, als heute aufgenommen wird, misst erst
  der Bau (die Laeufe haben vor F492 keine Bahnzeilen).

## 4. Riegel (Soll; Bauer und Abnehmer messen mit denselben Werkzeugen dieselbe Zahl)

| Riegel | Messung | Soll | Herleitung |
|---|---|---|---|
| **T1 Harness Weg/Dauer je Angriff** | neuer Harness-Teil (test_r41_gatorfix), je Angriff (LUNGE :2457, Hochbiss :2823, Rammen :2995, CROSS-Biss :3005, LUNGE-B2 :2430; je mit/ohne Kanister; Leon und Elza) Figur im freien Wasser bzw. Deckmitte, Gier so gesetzt, dass Sub 2 und Sub 3 je einmal vorkommen | Unterzustand = FUN_80015910; Bild-Schritte **exakt** `mess_re2_modell.txt` (Sub 2: 800,650,500,350,200,185..5 = 3735 rueckwaerts in 20 Bildern; Sub 3: 800..80 = 3080 vorwaerts, Ende nach 54 Bildern); Richtung = gewaehlte Lesart (O-T1); danach Steuerung zurueck (Routine 1) | Abschnitt 1.1 (@0x801030bc-0x801031dc, @0x80103238-0x80103370) |
| **T2 Animation** | derselbe Harness-Teil, Clip/Bild je Bild der Figur | Sub 2: Raum-Clip 18 (4 Bilder) -> 19 (doppelt); Sub 3: Raum-Clip 20 -> 17 (doppelt) -> PL00 Clip 11 rueckwaerts (= RE2 Basis-Clip 6); 0 Bilder mit RE1.5-Niederwurf-Clips 12..16 nach einem Gator-Treffer | ROOM40A0 RBJ Satz 0, `mess_rbj40a0.txt`; `mess_pld_vergleich.txt` |
| **T3 Schaden / Sperre / SE** | derselbe Teil + exe | Schaden 60, ab Gator-HP < 25000 **30** (die exe-Laeufe setzen RE15_K2080_GATOR_HP=24990 -> 30) ueber re15_re2_player_damage (One-Save wie RE2); getroffen werden nur, wenn nicht gesperrt: Sub 2 Bild 0-19, Sub 3 Bild 0-20 gesperrt, Sub 3 ab Clip 17 treffbar; SE core 1 (Sub 2) / 2 (Sub 3); Biss-Sperre 70 bleibt | @0x8010168c-a0, @0x80101ebc-dc, @0x80103918, @0x801015a4-ac, @0x801030e8-100/@0x801032f0, @0x801030bc-c8/@0x80103238-44 |
| **T4 exe Weg** | `treffer4.py`/`agg4.py` auf die exe-Riegel-Laeufe (rampe_oben, frei, abn2_*) und frei4_leon/elza | jede Luftlinie Treffer -> Ende Reaktion <= 3735 (heute bis 11 312); Dauer 20 oder 54 Bilder (Wand/Klemme duerfen kuerzen, nicht verlaengern) | Abschnitt 1.1 |
| **T5 Gegenprobe** | Pruefhaken "Treffer alt" (Stand 3d57083b1) | RE1.5-Niederwurf wieder da: Luftlinie > 3735 in frei4_leon, Clips 12..16 | rot ohne Bau |
| **W1 Kante -> Wasser** | Harness raum (Oben-Lagen) + exe rampe_oben/oben_kante: Trefferbilder, deren RE2-Schritt die Kantenlinie [8]/[9] quert | Figur faellt ins Wasser: im Uebergangsbild Band 1 -> 0 vor der Klemme, y -1800 -> -1300 (+500), dann +90/+140/+190... bis y = Boden (0), Spritzer; danach Figur im Wasser (y 0) innerhalb des Raums; 0 RAUMHALT-Zeilen Band 1 fuer diese Stoesse | Substate 10 @0x8003843c-44, @0x80038448, @0x80038460-bc, @0x80038660-9c, @0x80038724-2c; Nutzer "ruhig In das Wasser stoßen" |
| **W2 Wand bleibt Wand** | dto. | Stoss gegen Ostwand [6] / Plattform [7]: Figur bleibt oben (Klemme), y -1800 | u1 0x00 der Zellen [6]/[7] |
| **W3 nie aus dem Raum** | aussenlog (alle exe-Laeufe), Harness raum | 0 Bilder ausserhalb Aussenwaende / in Block [4]/[5] auf Band 0; Raumhalt Band 0 unveraendert | Abnahme 5 M1/M2, Abnahme 6 |
| **W4 Abstieg unveraendert** | exe west4_elza (Abnahme-4-Eingabe) | Aktionstasten-Abstieg F1627.. bitgleich (climb.log sub 9/10) | climb_common.c |
| **K1 kein Holmodus** | alle Laeufe mit Szenen-Kanister | 0 Zeilen "RUNDE zum Kanister" / "ANSCHWIMMEN an"; 0 Zeilen "Zielmodus 1" (PORT-WAHL O-K1); Gator-Bewegung mit liegendem Kanister bitgleich ohne Kanister bis zur Aufnahme (Harness: Lagen x 1800 Bilder, Kanister liegend an der Liegestelle gegen ohne Szene) | Nutzer (K) |
| **K2 Aufnahme nur in Reichweite** | gator_boss.log "KANISTER Stufe 1" + `reichweite.py` | jede Stufe-1-Zeile in Phase CHASE/LURK mit laengs in [0, 6000), quer < 1733; Gegenprobe: Kanister abseits jeder Bahn (Lage, die die Verfolgung nie streift) -> Stufe 0 bis Laufende | RE2 @0x80100ad0-ec; Port-Fenster :617-625 |
| **K3 oben ohne 28-s-Pause** | obenlog / NB3-Riegel A1/A2 mit liegendem Kanister (rampe_oben, abn2_rampe_west) | laengste Pause ab erstem Oben-Bild <= 600 Bilder (heute 851-856) | Messblatt NB3 A1/A2 |
| **K4 Stufen 1-6 / Sitz** | Kanister-Sitz debug.log | Stufen 1-6 RE2 unveraendert, "Abweichung max 0" | NB2 |
| **NB3/NB5 bleiben** | ctest `^unit_r41_gatorfix_` (28) und `^integration_r41_gatorfix_|^integration_r41_re2gator` (20) | gruen; gang/gang_elza 0 von 2700 abweichend; Vorgabe A (A1/A2, oben_kante), B (B1-B3 Haenge-Fenster 0, ohne Fortschritt <= 450); Sitz 0; Rueckfall nur aus dem Haenge-Fenster | Abnahme 6 |

Hinweis fuer die Riegel A1/A2 (oben nicht sicher): mit (W) faellt die Figur nach vielen Oben-Treffern ins Wasser; die bestehenden Auswerter zaehlen "Zeit
nach einem Sturz ins Wasser zaehlt nicht" (Messblatt NB3 A2) - Szenarien, die oben stehen bleiben wollen, muessen danach wieder hochklettern oder die
Pause ab dem letzten Oben-Bild messen. Die Messgroesse bleibt, die Szenarien pruefen.

## 5. Bauplan (kleinster Eingriff; je Punkt Quelle; die Form entscheidet der Bauer)

1. **(T) RE2-Spieler-Reaktion als eigene Maschine** (neue Datei, z. B. engine/src/gator_treffer_r41.c, PC und PSX): Eintritt aus `gb_biss_abschluss`
   (enemy_ai_boss_gator.c:452) statt `re15_player_take_damage` + `re15_player_knockdown_begin` (:454, :483); Lethal-Zweig (Fressen, :456-480) unveraendert,
   aber auf den RE2-Schaden umgestellt.
   - Unterzustand = FUN_80015910(Gator, Figur) (@0x801015c8-ec); Gier der Figur = FUN_8001569c(Figur, Achse, 1536) (@0x80101684-88; Port-Zwilling
     enemy_ai_boss_g5.c:1068) mit der Achse nach O-T1.
   - Schaden FUN_800401d4: `re15_re2_player_damage(pl, gator_hp < 25000 ? 30 : 60)` (@0x8010168c-a0, @0x80101ebc-dc); Rueckgabe Bit 0 -> Phase 4
     (@0x80103924-34), Bit 1 -> Tod/Fressen.
   - Sub 2 / Sub 3 Phase fuer Phase wie `mess_re2_modell.txt` (Konstanten -800/+150/+15 bzw. +800/-120 mit den Adressen aus 1.1), Vorschub = Gier-Richtung x
     Tempo (FUN_800152c8), durch den Dispatcher-Schwanz wie jeder Spielerschritt (re15_player_body_and_walls -> spieler_klemme); Sperre = pl->hit_react Bit 0
     genau in den RE2-Bildern (+0x1D3 & 0x80); SE re15_audio_core_se(1) / (2).
   - Animation: RE2-ROOM40A0-RBJ (RDT+0x60 @0x65e8, Satz 0, 14 328 B = dirOff 0x37e8 + 2 x 8) als `shared_assets/RE2/GATOR40A0.RBJ` (bytegleich ausschneiden,
     Muster tools/r41_ventilkette re2_ventil_export.py) und fuer 2090/2091 wie VENTIL1080.RBJ als Spieler-Overlay binden (platform/pc/main.c Boot- und Tuerweg,
     Muster ventilkette_r41_pc.c:178-193); PSX: dieselbe Datei im Raum-Arena-Pfad. Clips 18/19/20/17 aus dem Overlay, Schluss = PL00 Clip 11 rueckwaerts
     (kd_clip-Muster). Kein RE1.5-Niederwurf-Clip mehr nach Gator-Treffern.
   - Pruefhaken `re15_gator_treffer_alt_setzen(1)` = Stand 3d57083b1 (Gegenprobe T5).
2. **(W) Kante**: in der Bewegung der neuen Maschine (vor der Klemme): liegt die Figur auf Band 1 und der Zielpunkt des RE2-Schritts in einer Zelle mit
   Abwaerts-Bit 0x2 (re15_collision_floor_typeword, wie climb_common.c:260/304), dann die Fallschritte von climb_sub10 Ph2-Ph4 (climb_common.c:491-560, Adressen
   in 2.3) - als gemeinsamer Helfer aus climb_common.c herausgezogen, nicht kopiert - waehrend die RE2-Schritte waagrecht weiterlaufen. `re15_gator_boss_raum_halt`
   Band 1 greift fuer diesen Schritt nicht (Ausnahme "Fall aktiv"); Band 0 unveraendert. Gegen [6]/[7] Klemme wie heute.
3. **(K)**: RUNDE (kanister2080_r41.c :988-1017, Konstanten re15_kanister2080_r41.h:232/:237) und das Anschwimmen als Spielverhalten entfernen (Pruefhaken
   behalten: `re15_k2080_runde_alt_setzen(1)` = Stand 3d57083b1); GUARD :2506 wird damit wirkungslos; `gr2_zielmodus_takt` fuer den Szenen-Kanister aus
   (O-K1). PORT-WAHL-Kommentar an beiden Stellen mit dem Nutzer-Wortlaut (K). Aufnahme-Fenster und Stufen 1-6 unveraendert.
4. Messwerkzeuge: Harness-Teil T1-T3 + W1 (Figur je Lage, Treffer erzwingen oder Lagen der Raster), exe-Auswerter `treffer4.py`-Logik als Riegel.

## 6. Offen (Entscheid noetig bzw. nicht gemessen)

- **O-T1 Richtung (Entscheid Orchestrator).** RE2 gibt nur die vier Korridorachsen an FUN_8001569c (@0x80101610-80); ROOM2090 ist kein Korridor. Lesarten:
  A = Gator-Gier (Stoss laengs der Gator-Blickrichtung), **A90 = Gator-Gier auf die naechste Achse 0/1024/2048/3072 gerastet** (behaelt die RE2-Wertemenge der
  Achse; Messvorschlag). Folge fuer (W): A90 stoesst 41 von 48 Oben-Treffern ueber eine Kante, A 25 von 48 (Abschnitt 2.4).
- **O-W1 Animation im Sturz.** RE2 hat keinen Sturz. Moeglich: RE2-Clip laeuft weiter, nur y faellt (RE2-Haltung), oder ab dem Kantenbild RE1.5 Fall-/Landeclips
  D1/5/6 (W01, climb_sub10). Kein Original-Beleg fuer eine Mischung; Entscheid noetig. Waagrechter Weg nach der Kante: RE2-Schritte laufen weiter (Messvorschlag),
  Landung im Wasser y = 0.
- **O-K1 Zielmodus-Bit.** Die Biss-Sperre +0x1C0 & 4 ist RE2-original (@0x8010134c-58), haengt dort aber an der Korridorbahn, auf der der Kanister liegt. (K)
  verlangt "verfolgt ... wie ohne Kanister": Vorschlag aus fuer den Szenen-Kanister (PORT-WAHL mit Nutzer-Wortlaut); heute in 3 von 14 Laeufen aktiv.
- **O-T2 Elza.** ROOM40A0-RBJ Satz 1 (Claire) hat nur 16 Clips - die Clips 17-20 gibt es nur im Leon-Satz. Elza bekommt Satz 0 (wie die Port-Leihen
  re15_emd_parse_rbj_record record 0); EMR-Kopf identisch (15 Knochen, 80 B), Sichtpruefung am Bild noetig.
- **O-T3 Vibration** (0x800eaad8/0x800eac28) und Gator-Effekt FUN_80103d80 nicht Teil dieser Messung.
- Nicht gemessen: PSX-Bau; der allererste natuerliche Besuch ohne RUNDE (Bahnzeilen erst ab der Aufnahme); Elza-Harness raum_elza (Abnahme 6: bitgleich Leon);
  Tod der Figur (Messhaken hp 100); was die EXE-Kollision DAT_800cfc06 genau setzt (der Port prueft die Kontakte selbst, NB3 Abschnitt 3).

## 7. Tests auf diesem Stand

- `RE15_BUILD_DIR=<Baum>/re15_port/build bash re15_port/tools/local_build.sh build`: `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
- `ctest --test-dir <Baum>/re15_port/build -R "^unit_r41_gatorfix_" -j 2 --timeout 900` (PATH msys64 zuerst, SDL_ASSERT=always_ignore):
  **`100% tests passed, 0 tests failed out of 28`**, `Total Test time (real) =  84.17 sec` (`ctest_unit.txt`). Die exe-Riegel (20, ~73 min) hat die Abnahme 6 auf
  derselben exe 5910fc24 gruen gemessen (`G2_belege/abn6/ctest_exe.txt`); hier nicht wiederholt.
- Harness `test_r41_gatorfix raum`: `raum: 0 Fehler`, exit 0. exe nb4_oben_leon / nb4_frei4_leon exit 0, bytegleich Abnahme 6.
