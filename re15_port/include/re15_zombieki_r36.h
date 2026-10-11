/*
 * Runde 36 Spur F "zombieki" — Punkt 1: Sicht-Bit des RE2-Zombies (unten); Punkt 3: Gorilla (Typ 0x27, ROOM11C0)
 * Schatten-Record und Blutlache der Leiche. Dossier: analysis/befunde_runde36/F_zombieki.md.
 *
 * Nutzer: "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."
 *
 * ORIGINAL (STAGE1.BIN, selbst disassembliert mit re15_disasm.py):
 *   INIT FUN_80116f50: Schatten-Record FUN_8001af5c(a0=0, a1=0, a2 = rec+6 + 100, a3 = rec+6 + 200,
 *     sp+16 = entity+0xB0, sp+20 = 0x80808080) @0x801171cc-f0; rec = *(entity+0x78) =
 *     Tabelle 0x80121368[+0x1e2 = 4 (@0x80117098-9c)] = 0x80121350 -> u16 @0x80121356 = 1600
 *     (Hitbox-Halbbreite). FUN_8001af5c legt a2/a3 als `sh s3,12(s0)` @0x8001b030 / `sh s4,14(s0)`
 *     @0x8001b038 auf entity+0xBC / +0xBE (s0 = sp+16 = entity+0xB0). => +0xBC = 1700, +0xBE = 1800.
 *     (Der Zombie uebergibt an derselben Stelle 0x258/0x2bc = 600/700 @0x80100c2c/c38 — die Port-
 *     Konvention nhx/nhz der Halbmasse ist damit dieselbe.)
 *   LEICHE FUN_8011be54 (Zustand 7): +0x7 == 0: +0x9e = 0x5a @0x8011be80-84, +0x7 = 1 @0x8011be90-94,
 *     Wort0 |= 2 @0x8011beac, |= 0x40 @0x8011bec8; Durchfall; +0x7 == 1: Tinte +0xC4 / +0xEC =
 *     (alt & 0xff000000) | 0x00ffff38 @0x8011bed0-f00, **+0xBC += 8 / +0xBE += 8 @0x8011bf08-14**,
 *     +0x9e-- (alt == 0 -> +0x7 = 2) @0x8011bf24-44; +0x7 >= 2: `jr ra` @0x8011bf48.
 *   Der Port fuehrte die Leiche bisher nur mit dem Zaehler (+0x9e) — Tinte und Wachstum standen als
 *   Kommentar, wurden aber nie geschrieben, und der Renderer zeichnete fuer 0x27 den 500x600-Standard-
 *   Charakterschatten (dieselbe Fehlerklasse wie der Hund in Runde 34).
 */
#ifndef RE15_ZOMBIEKI_R36_H
#define RE15_ZOMBIEKI_R36_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_rdt.h"     /* re15_sca_entry_t */

#define RE15_R36_AFFEN_HITBOX_HALB   1600   /* u16 @0x80121356 (Record 0x80121350[+6], Tabelle 0x80121368[4]) */
#define RE15_R36_AFFEN_SCHATTEN_X    (RE15_R36_AFFEN_HITBOX_HALB + 100)   /* `addiu a2,a3,100` @0x801171e8 */
#define RE15_R36_AFFEN_SCHATTEN_Z    (RE15_R36_AFFEN_HITBOX_HALB + 200)   /* `addiu a3,a3,200` @0x801171f0 */
#define RE15_R36_AFFEN_LACHE_SCHRITT 8      /* `addiu v0,v0,8` / `addiu v1,v1,8` @0x8011bf08-0c */
#define RE15_R36_AFFEN_LACHE_TAKTE   0x5a   /* `ori v0,zero,0x5a` / `sb v0,158(a0)` @0x8011be80-84 */

/* INIT-Haken (re15_maggot_ai_tick Zustand 0): Schatten-Record +0xBC/+0xBE wie FUN_8001af5c @0x801171cc-f0. */
void re15_r36_affen_lache_init(re15_actor_t *e);

/* Leichen-Haken (re15_maggot_ai_tick Zustand 7, waehrend +0x7 == 1): Tinte + Wachstum @0x8011bed0-bf14.
 * Der Zaehler +0x9e bleibt im Aufrufer (unveraendert). */
void re15_r36_affen_lache_tick(re15_actor_t *e);

/* Renderer-Haken (platform/pc/main.c Schattenquad): liefert fuer Typ 0x27 Halbmasse und Lachenfarbe.
 * Rueckgabe 1 = uebernommen (nhx/nhz gesetzt; *pool = 1 mit Farbe 0x00ffff38 -> r 0x38 g 0xff b 0xff
 * wie der RE1.5-Hund @0x801117F0), 0 = nicht zustaendig. */
int re15_r36_affen_lache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                                uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b);

/* Nur fuer Sonden: Leichen-Haken abschalten (Gegenprobe). */
void re15_r36_affen_lache_enable(int an);

/* ---------------------------------------------------------------------------------------------------------
 * Punkt 1 — "sie bleiben hier ewig Beteiligungslos stehen, obwohl ich ziemlich in Ihrer Naehe vor ihnen bin"
 *
 * MESSUNG (test_r36_zombieki_sicht, Stand v0.8.23): 45 stehende Zombies, 2653 Leon-Plaetze im Vorderkegel bis
 * 4000; der Zell-Rechteck-Strahl re15_re2_los_cells_blocked (Slab-Test gegen JEDE u0&4-Zelle, Port-Mapping der
 * Kraehen-Flugkopplung 3486be93, 2026-09-08, vom Zombie mitgelesen) nahm an 306 Plaetzen (11,5 %) die Sicht, an
 * denen der RE1.5-Sichtstrahl (Nibble 3) frei ist; an 68 davon (ROOM10B0, Terrassenkante #9 Nibble 1) blieb der
 * Zombie 480 Bilder stehen. Bei freier Sicht stand KEINER.
 *
 * ORIGINAL RE2 (Ziel, Beta -> Retail): der Zombie-Navigator FUN_8004A808 setzt +0x154&0x800 aus dem Strahl
 *   FUN_80050858(selfPart, playerPart, a2 = 0x2000 `li a2,0x2000` @0x8004a868, a3 = 1 `li a3,1` @0x8004a86c,
 *   jal @0x8004a8d8), `andi v0,v0,0xf7ff` @0x8004a87c, `ori v0,v0,0x800` @0x8004a8f0 bei Rueckgabe 0.
 *   FUN_80050858 nimmt NUR Saetze mit Attribut & 0x2000 (`and`/`beq` @0x800508bc-c8) und ueberspringt die Formen
 *   {1..5,13} (Tabelle @0x800A73B4 = [0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0], `lbu` @0x800508d0-e4); mit a3 = 1 prueft er
 *   zusaetzlich die HOEHE des Satzes (-(w>>11)*100 - ((w>>6)&0x1f)*1800 @0x80050b34-74) gegen den Strahl zwischen
 *   den Koerperteilen -> niedrige Saetze (Tische, Tresen, Kanten) nehmen keine Sicht.
 * ORIGINAL RE1.5 (Datenseite der Raeume): der RE1.5-Sensor FUN_8001b84c ruft FUN_8003dcc4 mit `ori a2,zero,0xf00`
 *   @0x8001b8d4 / `ori a3,zero,0x300` @0x8001b8ec (Delay-Slot des jal @0x8001b8e8): es blocken NUR Zellen, deren
 *   Boden-Nibble (w5 & 0xf00) == 0x300 ist (`and v1,t5,a0` @0x8003de7c, `bne v1,v0,LAB_8003e0a8` @0x8003de94;
 *   Band-Test `lbu v1,0x82(v1)` / `sra v0,v0,0x1c` / `bne` @0x8003de60-6c). Das Nibble ist der von Capcom je Zelle
 *   gesetzte Sicht-Blocker (Waende 3; Tresen ROOM1030 #18 = 0, Terrassenkante ROOM10B0 #4/#9 = 1) — das RE1.5-
 *   Gegenstueck zum RE2-Attribut 0x2000 + Hoehenpruefung.
 * PORT (Mapping, benannt): das Sicht-Bit des RE2-Zombies kommt NUR aus dem RE1.5-Strahl FUN_8003dcc4 (4 Regionen,
 *   Aktor-Maske wie Runde 35 Spur H); der Slab-Test bleibt der Kraehe (Flugkopplung) und dem Zellenarm.
 * --------------------------------------------------------------------------------------------------------- */

/* Sicht-Bit +0x154&0x800 des RE2-Zombies (Haken in enemy_ai_re2_zombie.c): 1 = Sicht frei. */
int re15_r36_zombie_sicht_frei(re15_actor_t *e, re15_actor_t *pl);

/* Nur fuer Sonden: 0 = alter Weg (re15_re2_los_clear mit Slab-Test) fuer die Gegenprobe im selben Lauf. */
void re15_r36_zombie_sicht_enable(int an);

/* enemy_ai_common.c: nur der RE1.5-Sichtstrahl FUN_8003dcc4 (Zellen mit Boden-Nibble 3, 4 Regionen,
 * re15_los_ray_blocked, Aktor-Maske) von e nach (px,pz); 1 = verdeckt. */
int re15_r36_los_ray_nib3(const re15_actor_t *e, int32_t px, int32_t pz);

/* ---- Nachbesserung 1, Mangel 1 (Abnahme 0): NIEDRIGE Nibble-3-Moebel nehmen dem RE2-Zombie keine Sicht -------------
 * BEFUND (exe, ROOM1010, Abnahme-Lage Leon (3750,4300)): Slot 2 (205,7248) bleibt mit RE2-KI F1..F1000 in 1/0/1/0 —
 * der Nibble-3-Verhoertisch #5 und der Stuhlkreis #12 nehmen die Sicht; mit RE1.5-KI laeuft derselbe Zombie um den
 * Tisch auf Leon zu (F300 bei (2853,6614), Sub 12). ROOM1010 traegt in ALLEN Zellen fl = 0x03 (Nibble 3).
 * RE2 (Ziel): der Strahl laeuft von Kopfteil zu Kopfteil — Zombie +0x1c1 = 8 (`addiu v0,zero,8` / `sb v0,449(s2)`
 * @0x8010082c-30, EMZ0.BIN), Teil-Lage +0x5c/+0x60/+0x64 (FUN_8004A808 @0x8004a874-dc); FUN_80050858 mit a3 = 1
 * blockt nur, wo der Strahl im Satz-Rechteck UNTER der Oberkante `-(h>>11)*100 - ((h>>6)&0x1f)*1800` liegt
 * (@0x80050b34-74, Projektionen YZ/YX @0x80050b78-c30).
 * GEMESSEN: Kopfteil 8 Zombie y -2484..-2517, Leon -2319..-2514 (exe ROOM1010, re15_enemy_bone_world_pos(8) bzw.
 * head_world); Tischplatte #5 y = -1600 (Obj_model_set (200,-1600,5500)/(1800,-1600,5750), Anker der r34n_e-Ablage);
 * Stuhlkreise #12/#13/#14: Oberkante ueber -2000 nicht erreicht (Projektion mit den Engine-Matrizen probe_r34n_e_
 * dokumente kamera 1010, Cut 0: Marke y=-2000 liegt auf der Wand ueber der Lehne, y=-1600 auf der Tischkante —
 * F_belege/nb1_r1010_c0_hoehen.png).
 * PORT: der RE1.5-Strahl bleibt der Kreuzungstest (FUN_8003dcc4 je Zelle, re15_r36_los_zelle_kreuzt); eine gekreuzte
 * Nibble-3-Zelle mit BELEGTER Oberkante blockt nur, wenn der Kopf-zu-Kopf-Strahl in ihrem Grundriss tiefer liegt
 * (Form FUN_80050858 a3 = 1).
 * ⛔ NACHBESSERUNG 2 (Abnahme 1, Mangel 2): die NB1-Abbildung "jedes Obj_model_set-Objekt im Zell-Rechteck = Oberkante"
 * machte WAENDE mit Wandobjekten zu Moebeln (ROOM1040 #4 2052 x 37566: drei senkrechte Tafeln, MD1 nv4 x -1800..1800
 * y -2232..0 z 0..0, bei y -1900 -> Ostgang-Zombies sahen Leon im Westgang; Durchwand-Zensus 3628 Plaetze). Seit NB2 gilt
 * die Oberkante NUR fuer die Moebel-Zellen der Beleg-Tabelle k_r36_moebel (zombieki_r36.c), je Zelle mit Beleg:
 *   (a) RDT: Obj_model_set, dessen Modell AUF der Zelle ruht (MD1-Unterkante = Ursprung, y-Spanne nur nach oben) ->
 *       dessen y (Datei-Byte-Offset des y-Operanden pc[12..13]);
 *   (b) PORT-WAHL (Messwert): Projektion mit den Engine-Matrizen auf das Hintergrundbild (Kamera-Record + Bild genannt),
 *       eingetragen als OBERGRENZE.
 * Jede andere Nibble-3-Zelle hat volle Hoehe (Wand) — wie die RE1.5-Daten selbst (FUN_8003dcc4 kennt keine Hoehe).
 * LEONS KOPF (Abnahme 1, Mangel 5): nur aus head_world (Plc_neck-Ursprung, skeleton_common.c, je gezeichnetem Bild aus
 * Leons PLD-Pose gesetzt). Ohne head_world gibt es keinen Hoehenvergleich: die Zelle blockt (RE1.5-Datenlage) — keine
 * Ersatzzahl mehr. Die Sonden setzen head_world aus PL00.EMR/EDD (Bindpose-Knochen 8 wie skeleton_common.c). */
int re15_r36_los_zelle_kreuzt(int32_t ex, int32_t ez, int32_t px, int32_t pz, const re15_sca_entry_t *c);
void re15_r36_zombie_hoehe_enable(int an);   /* NUR Sonden: 0 = Abbildung vor NB1 (jede Nibble-3-Zelle blockt),
                                              * 2 = NB1-Abbildung (jedes Obj_model_set im Rechteck; Gegenprobe Mangel 2) */
/* NUR Sonden: Oberkante einer Zelle (0 = unbelegt) und Grund (1 = Obj_model_set ruht darauf, 2 = Messwert). */
int32_t re15_r36_zelle_oberkante(const re15_sca_entry_t *c, int *grund);

/* ---- Nachbesserung 2, Mangel 1 (Abnahme 1): der TASCHENFALL ROOM1010 Slot 1 -----------------------------------------
 * GEMESSEN (test_r36_zombieki_eintritt spur 1010 1020 0 1 1 140 1 + RE15_COLL_TRACE): der RE2-Rueckstoss nach dem Griff
 * (EXEC[3] P8: Wurzeldelta + `addiu a2,a2,-30` / `jal 0x800152c8` / `sh a2,324(s1)` @0x80102CA0-AC, EMZ0.BIN; 140..290
 * Einheiten je Bild) traegt den Zombie in den Keil zwischen Stuhlkreis #13 (t3, Mitte (2450,2800) r 600) und Raute #15
 * (t2 3050..4650 / 1400..3000). Die Klemme (FUN_8003b0a4-Zwilling, Zellen nacheinander) schiebt ihn je Bild erst aus
 * dem Kreis (radial, ueber die Rautenmitte z 2200 hinweg) und dann aus der Raute nach SUEDWEST: F124 (3031,2336) ->
 * #13 (3234,2174) -> #15 (2955,1895). Die Tasche (2450..3050 / 1550..2200, zwischen #3, #5, #13, #15) hat fuer Radius
 * 400 KEINE stabile Lage (die aufgeblasenen #13 und #15 ueberlappen an der Rautenspitze): er tritt dort fuer immer
 * auf der Stelle (st1/1/1, 1220..2500 vor Leon).
 * RE2 (Ziel): die Zombie-Wurzel ruft die Klemme EINMAL je Bild nach dem Zustand (FUN_8003567c -> FUN_8004c1bc
 * @0x80100638), ebenfalls Satz fuer Satz ohne Rueckgabe-auf-Vorlage — die Keil-Lage gibt es in RE2s Raeumen nicht.
 * PORT-WAHL (Nutzer: "Das darf so nicht sein"): waehrend des Wurfs (EXEC[3] P7/P8, +0x5 = 3, +0x6 = 7/8) endet ein Bild
 * nicht in einer Lage, die die Klemme nicht aufloesen kann: verschiebt die Klemme die eben geklemmte Lage erneut,
 * bleibt der Zombie auf der Lage VOR dem Bild. FORM = der Rueckfall der Original-Klemme auf die Vorlage
 * (`*lx = prevx; *lz = prevz` im Kontaktcode-0-Zweig der Diagonal-Schieber LAB_8003cb9c / LAB_8003c2cc, re15_collision.c
 * push_diag6/push_diag7). Keine neue Zahl. */
void re15_r36_wurf_klemme(re15_actor_t *e, int32_t ox, int32_t oz);
void re15_r36_wurf_klemme_enable(int an);   /* NUR Sonden: 0 = Weg vor NB2 (Gegenprobe) */

/* ---------------------------------------------------------------------------------------------------------
 * Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern."
 *
 * MESSUNG (test_r36_zombieki_affen_block fest ..., Stand nach Punkt 1): ein Gorilla in der JAGD (Sub 3), dessen
 * Sichtlatch +0x1d0 faellt (Wagenblock #2/#7 = Nibble 3 dazwischen) und der an der Wandklemme haengt, bleibt
 * 1200/1200 Bilder in Sub 3 an derselben Stelle (z.B. Leon (-7000,12500), Gorilla (-14000,-3000): Abstand 8415
 * -> 8476, Klemme 1200, Sicht 0, nur Sub 3) — das ist das "Blockiert" des Nutzers.
 * ORIGINAL (STAGE1.BIN, A[3] @0x80117b40-c74, selbst disassembliert): die Entscheidung der Jagd kennt nur die
 *   Ausgaenge Biss (a804(0xbb8,0x180) @0x80117a60-74), Wiedereintritt mit Sicht (`sltiu v0,v0,0xbb8` + `andi 1` +
 *   `lbu 7` @0x80117b60-98 bzw. `sltiu v0,v0,0x1771` @0x80117bc8-c08) und FERN + SICHT -> Auswahl Sub 4
 *   (`sltiu v0,v0,0x1771` / `bne` @0x80117c34-38, `andi v0,v0,0x1` / `beq` @0x80117c48-4c, `ori v0,zero,4` /
 *   `sb v0,5(v1)` @0x80117c50-54). Fuer "keine Sicht" gibt es KEINEN Ausgang; B[3] steuert dabei immer auf
 *   playerX (`addiu a0,a0,-13688` = 0x800aca88 @0x80117d4c, `jal 0x8001a8f8` @0x80117d50) — gegen den Wagen.
 *   Die Wagen ueberquert das Original NUR per Sprungpad aus Sub 4 (A[4] @0x80117ecc-8011802c: FUN_8003b93c attr
 *   0x10, Yaw-Fenster `((zc&0xf0)<<4 - Yaw + 512) & 0xfff < 1024` @0x80117f18-38, +0x7 = 1 @0x80117f50-74).
 *   Der Blind-Zonensprung auf der Stelle ist Original (Runde 35 Spur J, GDB 400/400).
 * PORT-WAHL (NUTZER-VORGABE "irgendwie verbessern", kein Original-Verhalten) — Stand NACHBESSERUNG 1 (Abnahme 0,
 *   Maengel 4-6; die fruehere "naechstes Pad"-Suche schickte Gorillas von Leon weg und war ueber 52 Seeds netto
 *   schlechter, AUS 60561 / AN 76667 blockierte Bilder):
 *   GEMESSEN (Lagekarte der Blockaden, 52 Seeds AUS): 38 % aller blockierten Bilder auf EINER Stelle (6418,-3617) =
 *   MITTE im Pad #36 der Ost-Engstelle (2711 breit < 2*1600); dort ist der Kontaktcode der Platzhalter 0xf00
 *   (coll_contact_dir, FUN_8003b93c @0x8003ba8c), das Yaw-Fenster @0x80117f18-38 passt nie zur Richtung auf Leon.
 *   Begehbarkeitskarte (echte Klemme, Radius 1600): Nord- und Suedteil des Parkplatzes sind fuer den Gorilla zu Fuss
 *   GETRENNT, nur die 0x10-Pads (#36 Ost, #47 West) verbinden sie per Sprung.
 *   UMSETZUNG: Raster aus der Original-Klemme (re15_collision_constrain_contact_band = FUN_8003b0a4-Zwilling mit
 *   Radius box[6] = 1600 @0x80121356, Band +0x82, Maske 4 wie der 0x27-Zweig), Schritt = Radius/2; Komponenten +
 *   Breitensuche zu Leon. Gleiche Komponente -> zu Fuss den kuerzesten Weg (8 Knoten Vorschau, gerade erreichbar);
 *   andere Komponente -> Anlaufpunkt vor dem verbindenden 0x10-Pad auf der EIGENEN Seite, dann gerader Anlauf auf
 *   die Pad-Mitte: die Kontaktseite zeigt auf Leons Seite, der Sprung ist der des Originals (Sub 4: A[4] Path A
 *   @0x80117ecc-8011802c; Sub 3: +0x9f/+0x5/6/7 @0x80117f40-74). Mitte im Pad verkeilt -> in die Code-Richtung
 *   (zc & 0xf0) << 4 drehen, damit das Fenster passt (Sprung = Original). Beginn nur an der Wandklemme +0x1d6
 *   (@0x80117b18-20 / @0x80118054-5c), in Sub 3 nur ohne Sicht; NAH (+0x1d4 < 0x1771 @0x80117bc8 / @0x8011806c)
 *   bleibt Original (die Spur-J-Riegel takt/griff/schrot/finisher bildgenau), ausser der Gorilla steckt im Pad.
 *   MESSUNG (52 Seeds x 6000 Bilder Zufallsgang, je Lauf eigener Prozess): blockiert AUS 75408 -> AN 2380, Griffe
 *   226 -> 254, besser 39 / schlechter 3 (+194 hoechstens) / gleich 10; exe Kampf-Layout: Slot 2 Angriff F896
 *   (v0.8.23 nie), Slot 3 F606 (v0.8.23 F1012).
 * NACHBESSERUNG 2 (Abnahme 1, Maengel 3/4):
 *   GEMESSEN (Befundstelle Leon (-24000,10000)): Leon steht in der Nordwest-Tasche, die das Raster der Original-Klemme
 *   (Radius 1600) als EIGENE Komponente ohne Pad fuehrt (Spitzenberuehrung bei (-25850,15750)). Der NB1-Umweg fand kein
 *   Pad zu Leons Komponente und schaltete ab -> Gorilla 2 drueckte in Sub 4 mit Sicht gegen die Wand (-321,12280), Gorilla 1
 *   sprang nach dem Pad-Mitte-Sprung in der Original-Blind-Schleife (A[4] +0x7 = 3 @0x80117fe4-80118024) bei (-13518,-316).
 *   Ein erzwungener Path-B-Weitsprung (+0x5 = 7, +0x7 = 0 @0x801180cc-f8) vom Nordstreifen landet 2050 vor Leon: die Tasche
 *   ist ueber ihren Rand angreifbar.
 *   UMSETZUNG: Ziel = Leons Knoten, wenn seine Komponente zu Fuss + ueber 0x10-Pads (Breitensuche ueber Komponenten, auch
 *   mehrere Spruenge) erreichbar ist, SONST der erreichbare Knoten, der Leon am naechsten liegt; der erste Sprung auf dem
 *   Weg ist der Original-Sprung wie in NB1. Zwei Gorillas im KOERPERKONTAKT (Radien 2 x 1600 @0x80121356 + ein
 *   Rasterschritt) auf demselben Pad: der dem Pad naehere behaelt es, der andere nimmt ein anderes (gemessen: beide auf
 *   Pad #36 schoben sich 1200 Bilder fest). Keine neue Zahl.
 *   MESSUNG: Befundstelle beide Gorillas Angriff (Sonde F597/F929, exe F850/F1912), 0 Blind-Spruenge (NB1: 48);
 *   Zufallsgang 52 Seeds mit Anker 400: AUS 114029 -> AN 15857 (NB1 27597), Anker 40: 75408 -> NB2 siehe Dossier.
 * NACHBESSERUNG 3 (Abnahme 2, Maengel 1/2), Belege in zombieki_r36.c:
 *   (1) Weg = der KUERZESTE (A*, Pads als Kanten mit ihrer Weglaenge) statt der ersten Pad-Kante in SCA-Reihenfolge
 *       (#36 vor #47 -> Nordwest-Gang ueber das Ost-Pad, Angriff 850..920 Bilder spaeter als v0.8.23).
 *   (2) Frei-Test mit Vorlage = Knoten + 1: mit Vorlage = Knoten setzte der Rechteck-Schieber im Code-0-Zweig auf die
 *       Vorlage zurueck (@0x8003be8c-a0) — Knoten IN Zellen galten als frei (die "Nordwest-Tasche"). Kanten = Schritt, den
 *       die Klemme in beide Richtungen nicht aufhebt; Startknoten gerade erreichbar; Leons Knoten auf Leons Seite.
 *   (3) Beginn erst, wenn der Gorilla FERN haengt (Verbraucher des Original-Zaehlers +0x1de @0x80117448-6c), sofort in
 *       der Jagd an einer 0x10-Pad-Kante mit Weg ueber das Pad; NAH bleibt Original (Spur-J-Riegel griff/szene).
 *   MESSUNG: Sonde Nordwest-Gang erster Angriff 352 (Original-Regel 430), kein Gorilla ueber #36; Zufallsgang 52 Seeds
 *   Anker 400 AUS 114029 -> 17975, Anker 40 75408 -> 221; exe zehn Stellen: Dossier NB3.
 * NACHBESSERUNG 4 (Abnahme 3, Maengel 1/2), Belege in zombieki_r36.c (re15_r36_affen_fest_takt):
 *   ORIGINAL GEMESSEN (DuckStation-GDB, r3 s033, Haltepunkt 0x80116db8, Lagen per Speicherschreiben ab VSync 10029): der
 *   Stau zweier Gorillas am Nordwest-Rand (einer NAH an der #21-Ecke, einer FERN dahinter, Koerperkontakt) haelt im Original
 *   900/900 Bilder (e2 +0x1de 179 -> 1079); ein einzelner naher Gorilla an der #21-Ecke wartet 900 Bilder, in der Ostgasse
 *   (Pad #36, 2711 < 2 x 1600) warten beide — die Nah-Wartezeit ist Original (A[3] unter 0x1771 nur Biss < 0xbb8
 *   @0x80117a60-74, B[3] auf playerX @0x80117d4c) und bleibt (Spur-J-Riegel griff/szene pinnen sie).
 *   VORTRITT (PORT-WAHL): steht ein Gorilla FERN seit R36_VORTRITT_START Bildern im Anker und im Koerperkontakt mit einem
 *   NAHEN Gorilla vor ihm, geht der NAHE den Umweg zu Fuss, bis sein gerader Weg zu Leon frei ist.
 *   MESSUNG: Stau-Sonde beide greifen an (91/101, vorher keiner), exe Nordwest-Rand FERN 1465/691 -> 0/50; Zufallsgang 52 Seeds
 *   Anker 400 17975 -> 16352, Anker 40 221 -> 125.
 * NACHBESSERUNG 5 (Abnahme 4, Mangel 1), Belege in zombieki_r36.c (R36_NAH_START, r36_nah_route):
 *   NAH-UMWEG (PORT-WAHL): ein naher Gorilla in der Jagd, der R36_NAH_START = 354 Bilder an der Klemme ausserhalb der Bissweite
 *   (0xbb8 @0x80117a60) im Anker steht, geht zu Fuss zum naechsten FREIEN Bissplatz (Knoten < 0xbb8 vor Leon, kein anderer
 *   Gorilla in Koerperkontakt-Abstand, Radius box[6] @0x80121356). 354 = laengste Nah-Wartezeit des kanonischen Ankunftskampfes
 *   (353, Spur-J-Riegel szene) + 1. Kein freier Bissplatz -> Original (Anstehen: in der Nordwest-Ecke passt nur EIN Gorilla in
 *   Bissweite, Durchmesser der Bissplaetze 1649..1833 < 3168).
 *   MESSUNG: exe Stelle 3 Slot 2/3 fest 1653/1875 -> 429/368, 14 Stellen 10466 -> 7500; kanonischer Kampf bitgleich.
 * NACHBESSERUNG 6 (Abnahme 5, Maengel 1/2), Belege in zombieki_r36.c (r36_nah_route, r36_pad_sprung_code, re15_r36_affen_fest_takt):
 *   OSTGASSE (PORT-WAHL): hat die eigene Fuss-Komponente keinen freien Bissplatz, nimmt der Nah-Umweg den naechsten freien Bissplatz
 *   auf LEONS Seite ueber die 0x10-Pads (A* mit den Pad-Kanten des FERN-Umwegs); Sprung = Original A[4] Path A (@0x80117ee0-80118074)
 *   nur mit Seitencode (0xf00 = Platzhalter @0x8001bf74), Beginn ab R36_FEST_START wie der FERN-Umweg (zu Fuss weiter ab 354 =
 *   J-Fenster). VERKEILT (Mitte im Pad, kein Knoten gerade erreichbar) springt er mit dem Code der Pad-Seite, ueber die sein Weg
 *   fuehrt (Codes @0x8003ba8c / @0x8001bfa4-bfec). NACHRUECKEN: stirbt ein Gorilla, der in Bissweite stand (+0x1d4 < 0xbb8), gilt
 *   die Nah-Wartezeit jedes NAH wartenden Gorillas als abgelaufen (Zustand 3/7, Tabelle @0x801213c8).
 *   MESSUNG: Gasse einzeln G1 kein Angriff (NAH fest 2258) -> Angriff F313 (0); O1 G1/G2 1368/1615 -> 0/248, beide greifen an;
 *   Nachruecken Biss 588 -> 105 Bilder nach dem Tod; kanonischer Kampf bitgleich.
 * --------------------------------------------------------------------------------------------------------- */

/* Haken in re15_maggot_ai_tick A[3] (nach dem Biss-Test, vor B[3]); sub = 3 (andere Werte: Rueckgabe 0, keine
 * Wirkung). Rueckgabe 1 = der 0x10-Sprung (Sub 7, +0x7 = 1) wurde ausgeloest. */
int re15_r36_affen_padsuche_decide(re15_actor_t *e, int slot, int sub);

/* Haken in B[3]: Steuerziel (playerX oder Pad-Mitte bzw. Punkt in Pad-Richtung); 1 = Pad-Suche aktiv. */
int re15_r36_affen_padsuche_ziel(const re15_actor_t *e, int slot, int sub, int32_t plx, int32_t plz, int32_t *tx, int32_t *tz);

/* NB3: Gehirn-Schwanz des Gorillas (Zustand 1, nach den Timern +0x1dc/+0x1e1): +0x1de = Bilder an der Klemme
 * (@0x80117448-6c, ohne Verbraucher in der Beta) + Anker-Messung — der Beginn des Umwegs. */
void re15_r36_affen_fest_takt(const re15_actor_t *e);

/* Nur fuer Sonden: Pad-Suche abschalten (Gegenprobe = Original-Verhalten). */
void re15_r36_affen_padsuche_enable(int an);
int  re15_r36_affen_padsuche_aktiv(int slot);
int  re15_r36_affen_umweg_lesen(int slot, int32_t *x, int32_t *z, int *pad);   /* NUR Sonden */
void re15_r36_affen_fest_lesen(int slot, int *h1de, int *fest, int *vortritt);   /* NUR Sonden (NB4) */
void re15_r36_affen_vortritt_enable(int an);   /* NUR Sonden (NB4): 0 = Stand NB3 ohne Vortritt (Gegenprobe) */
int  re15_r36_affen_nah_lesen(int slot);        /* NUR Sonden (NB5): Bilder NAH im Anker (Jagd, ausser Bissweite) */
void re15_r36_affen_nah_enable(int an);         /* NUR Sonden (NB5): 0 = Stand NB4 ohne Nah-Umweg (Gegenprobe), >= 10: Schwelle an - 10 */
void re15_r36_affen_nb6_enable(int bits);       /* NUR Sonden (NB6): Bit 0 Nah-Umweg ueber 0x10-Pads, Bit 1 Nachruecken; 0 = Stand NB5 */
int  re15_r36_affen_nah_grund(int slot);        /* NUR Sonden (NB6): letzte Antwort von r36_nah_route (Codes in zombieki_r36.c) */
int  re15_r36_affen_bissplaetze_frei(int slot); /* NUR Sonden (NB6): freie Bissplatz-Knoten des Umweg-Rasters (0 = steht an) */
/* NB6b (Fortsetzung nach dem Wochenlimit): im Nah-Umweg ueber ein 0x10-Pad gilt "am Anlaufpunkt" auch, wenn der Gorilla das Pad
 * im Sinne der Box-Abfrage des Original-Pad-Sprungs erreicht hat (FUN_8003b93c(+0x34, box[6]+100, +0x82, 0x10) @0x80117ee0-f00),
 * nicht erst am Raster-Startknoten der Pad-Kante. GEMESSEN (exe Stelle 3, Messschiene RE15_AFFEN_LOG): in der Ostgasse (1550 breit
 * < 2 x 1600) liegt dieser Knoten 1970 ausserhalb der Muendung; der Gorilla an der Pad-Kante lief von Leon weg, kippte ueber 0x1771
 * (FERN, Umweg geloescht) und zaehlte an der Kante neu — S2 griff nie an. Die Regel gilt nur AUSSERHALB des Pads auf der Startseite
 * des Wegs (Kontaktcode FUN_8001bf04 @0x8001bfa4/ac/e8/ec gleich dem des Startknotens). NB6g: mit der MITTE im Pad (0xf8,
 * Platzhalter @0x8001bf74) springt er mit dem Seitencode des Startknotens genau EINMAL je Pad-Aufenthalt (Grund 14; Merker
 * s_r36_g14_pad1, geloescht beim Verlassen des Pads), danach zu Fuss zum Startknoten wie NB6a. GEMESSEN: Gasse einzeln G1 Angriff
 * 157 (ohne Grund 14: nie, 2258 FERN); exe-Stelle 7 (Pad #47): der eine Sprung bleibt in der Flugklemme am Start haengen
 * (Port-Verhalten, OFFEN), der Original-Finisher a804(0x1388,0xc0) -> Sub 8 (@0x80118ddc) trifft Leon trotzdem; Wiederholung
 * alle ~85 Bilder (NB6c) entfaellt. Nachher: Stelle 3 S2 Angriff F1651 (vorher nie), Stelle 8 S3 0 (vorher 468). */

/* ---- Nachbesserung 1, Mangel 2 (ROOM10B0 Terrasse): ZONENLOSER RE2-Zombie --------------------------
 * GEMESSEN (test_r36_zombieki_eintritt spur 10B0 1260 7 1 1): Steuerziel steer = (0,0) — die Terrasse liegt
 * in keiner BLK-Zone (ROOM10B0.RDT @0x1680), der Navigator (FUN_80039e7c-Zwilling re15_nav_update_steer)
 * findet fuer "keine Zone" keinen Pfad und laesst das Ziel stehen; die Zombies liefen zum Weltursprung (Osten).
 * RE2 FUN_8004A808 (selbst gelesen): Ziel +0x1c4/+0x1c6 schreibt NUR der Navigator — gleiche Zone `sh` @Z.46,
 * Route FUN_8004ab60 @Z.347, oder DIREKT die Zielposition im Zweig +0x10E & 0x1000 (`lw v0,0(s3)` /
 * `sh v0,452(s0)` / `lw v1,8(s3)` / `sh v1,454(s0)` @0x8004a90c-924). Mit Zone 0xff (FUN_8004aab8) und a3 = 0
 * schreibt er nichts (`return 2`) — ein Zustand, den RE2s eigene Raumdaten nicht erzeugen.
 * PORT-WAHL (Nutzer: "Das darf so nicht sein"): steht der RE2-Zombie in KEINER Zone eines Raums MIT Graph,
 * gilt die direkte Form (Ziel = Spielerlage) — dieselbe, die der Port-Navigator ohne Graph schon nimmt
 * (nav_zone_common.c "no BLK graph: raw target"). Keine neue Konstante. */
void re15_r36_nav_zonenlos(re15_actor_t *e, const re15_actor_t *pl);
void re15_r36_nav_zonenlos_enable(int an);   /* NUR Sonden: 0 = alter Weg (Gegenprobe) */

void re15_r36_affen_log_tick(void);           /* NUR Messschiene (NB6): RE15_AFFEN_LOG=<datei> (env, PC), je Bild der Port-Umweg-Zustand je Gorilla; kein Verhalten */

#endif /* RE15_ZOMBIEKI_R36_H */
