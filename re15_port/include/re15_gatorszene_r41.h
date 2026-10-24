/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene": ROOM2090/2091 ohne Spinnen, Ankunftsszene mit dem Alligator,
 * Schluessel bei der Leiche. Dossier: analysis/befunde_runde41/F_gatorszene.md. Code: engine/src/gatorszene_r41.c.
 *
 * NUTZER-VORGABE (AUFTRAG.md 2026-10-23, woertlich):
 *   "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."
 *
 * ---- 1. SPINNEN ---------------------------------------------------------------------------------------------------
 * Das RDT stellt sie in main00 auf (ROOM2090.RDT und ROOM2091.RDT bytegleich):
 *   @0x00AB0 `44 00 25 00 00 00 00 73 9c ff 00 00 98 df ..`  Slot 0, Typ 0x25, Kill-Flag 0x73, (-100, 0, -8296)
 *   @0x00AC4 `44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2 ..`  Slot 1, Typ 0x25, Kill-Flag 0x74, (-2300, 0, -23896)
 * FORM (belegt): das Original verwirft einen Sce_em_set-Satz ueber das Kill-Flag-Tor FUN_800420a0 @0x80042120-38
 * (Satz erscheint nicht, `ori v0,zero,0x8000` / `sw v0,0(s0)` @0x80042230-38 = "Record verworfen"). Der Port verwirft
 * GENAU diese beiden Saetze beim Lesen (scd_vm.c op_sce_em_set, Haken neben re15_zombie_abstand_anwenden) — KEIN
 * RDT-Patch, kein Kill-Flag gesetzt (die Bits 0x73/0x74 bleiben unberuehrt). Schluessel = Raum-Basis + Typ +
 * ORIGINAL-Lage (Satz-Waechter wie zombie_abstand_r35.c), der Slot wird nicht verglichen. */
#ifndef RE15_GATORSZENE_R41_H
#define RE15_GATORSZENE_R41_H

#include <stdint.h>

#define RE15_GSZ_RAUM            0x2090u   /* Leon; Elza = 0x2091 (Raum-Basis & 0xFFFE)                     */
#define RE15_GSZ_SPINNE_TYP      0x25u     /* Sce_em_set pc[2] @0x00AB2 / @0x00AC6                          */
#define RE15_GSZ_SPINNE0_X       (-100)    /* @0x00AB8 `9c ff`                                              */
#define RE15_GSZ_SPINNE0_Z       (-8296)   /* @0x00ABC `98 df`                                              */
#define RE15_GSZ_SPINNE1_X       (-2300)   /* @0x00ACC `04 f7`                                              */
#define RE15_GSZ_SPINNE1_Z       (-23896)  /* @0x00AD0 `a8 a2`                                              */

/* HAKEN scd_vm.c op_sce_em_set: 1 = diesen Satz verwerfen (Spinne 0x25 in 2090/2091 an einer der zwei RDT-Lagen). */
int  re15_gatorszene_r41_spawn_weg(uint16_t room_id, uint8_t type, int16_t x, int16_t z);
/* NUR fuer Riegel, die die Original-Spinnen von 2090 als Messobjekt brauchen (Wasser-Kanal, Wandklemme): 0 = aus.
 * Standard 1. Kein Spielschalter, keine Umgebungsvariable. Rueckgabe = alter Wert. */
int  re15_gatorszene_r41_spinnen_weg_setzen(int an);
/* Pruefhaken: Zahl der verworfenen Saetze seit Programmstart. */
int  re15_gatorszene_r41_spinnen_verworfen(void);

/* ---- 2. ANKUNFTSSZENE (NUTZER-VORGABE, AUFTRAG.md Punkt + Nachtrag 6, Wortlaut im Dossier) ------------------------
 * "Leon kommt im Raum an, sieht die Zombie Leiche mit dem Schluessel in der Cutscene. Dann kommen oben in 2080 die Zombies
 *  rein. Dann erneut Kamera auf Zombie mit Schluessel. Aligator kommt frisst den Zombie samt Schluessel. Macht danach Richtung
 *  Leon eine schrei und Schuettel Animation. Danach geht der Kampf los, mit der Kampfmusik von Resident Evil 2."
 * FORM (je Baustein mit Vorbild, Dossier §1.2):
 *   - Szene = portseitig eingespielter SCD-Bytecode aus ORIGINAL-Opcodes, ausgefuehrt von der vorhandenen VM (Muster
 *     cut_10f0.c / irons_tod_1150.c), gestartet vom Raumaufbau ueber ein freies Ereignis (RE15_GSZ_EREIGNIS).
 *   - "Dann kommen oben in 2080 die Zombies rein" = MONTAGE-Schritt: Raumwechsel innerhalb der Szene ueber Door_aot_set +
 *     Aot_on (ROOM1240 sub02/03, ROOM1080 sub07..10; Port-Muster irons_tod_1150.c), Spieler ausserhalb des Bildes geparkt,
 *     zurueck nach 2090 auf seine Eintrittslage. 2080 ist von 2090 aus nicht sichtbar (Dossier §1.1: alle 13 Kameras von 2090
 *     blicken auf Hoehe -3132..-5634 ins Becken; der 2080-Boden liegt auf -14400, ROOM2090 Tuer 0 @0x00996 Ziel-y C7C0).
 *   - Gator: RE2-Clips des EM23 (Spur B), Abfolge = RE2-Intro (Clip 6 Durchbruch @0x801008ac-b8) + RE2-FRESSEN (Clip 4 Bild
 *     1..14 @0x801010ec-f4, Koppelbild 13 @0x80101104-34, Clip 5 @0x80101180-88, Clip 11 @0x801011d8-e0) mit der Leiche statt
 *     Leon; die Leiche haengt wie der RE2-Kanister als Kind der Part-6-Weltmatrix (@0x801018d0-e0); Wende auf Leon mit 75/Bild
 *     (RE2 R1 FUN_8001569c @0x801025f8-608); Schrei = RE2-Grossreaktion R4: SE 0 (@0x80102c0c-10) + Clip 10 Blend 3
 *     (@0x80102c40-48) + Stoss-Ereignis 1 (Ruetteln/Schuetteln @0x80101d44, wie das 0x28-Bildwort @0x801003f0-40c).
 *   - Musik: RE2 ROOM40A0 = MAIN1A mit Manuell-Start (INIT_TBL.DAT Tabelle @0x22C, Eintrag Stage 4 Raum 0x0A @0x2DA `5a ff`),
 *     gestartet von Sce_bgm_control `51 00 01 00 78 40` (ROOM40A0 SCD-main sub11 @0x071F, Handler @0x80057e20 -> FUN_8005b6f0
 *     Fall 1 SsSeqPlay), ausgeblendet mit `51 00 05 ..` (@0x0762/@0x078A, Fall 5 FUN_8007a120(seq,0x7f,0x5a)). */
#define RE15_GSZ_EREIGNIS        18    /* frei: ROOM2090 Sub-Tabelle @0x0ADC `08 00` = 4 Subs, ROOM2080 @0x0BF8 `06 00` = 3; kein anderer
                                        * Port-Hook nutzt 18 (Zensus der *_EREIGNIS-Konstanten); < RE15_RDT_MAX_SUB_SCD 32 */
#define RE15_GSZ_TUER_SLOT       20    /* AOT-Platz der Montage-Tuersaetze: frei in 2090 (main00 0..5 + Kamerazonen 48..63) und
                                        * 2080 (main00 0..4); Muster RE15_IT_TUER_SLOT */
#define RE15_GSZ_BANK            9     /* Bank 9 = 0x800b1078, gespeichert mit g_game.flags (VERTRAG 1.1: F = 110-113) */
#define RE15_GSZ_BIT_GESEHEN     110   /* Szene gesehen (gesetzt mit dem Kampfbeginn; Spur G liest es)                     */
#define RE15_GSZ_BIT_LIEGT       111   /* Schluessel liegt bei der Leiche (Alligator tot)                                 */
#define RE15_GSZ_BIT_GENOMMEN    112   /* Schluessel genommen (Item-Zone Taken-Bit)                                       */
#define RE15_GSZ_BIT_TOR         113   /* Tor 2090 <-> 20A0 mit dem Schluessel aufgeschlossen (Nachtrag 21)                */
/* Signal-Bits Bank 5 Wort 0 (Raum-Scratch, geloescht beim Raum-Init FUN_8003ecec @0x8003ed74; ROOM2090/2080-Skripte nutzen
 * Bank 5 nicht - Zensus der Disassemblierung): Programm setzt per Set, der Port-Takt reagiert (Form irons_tod (5,12)/(5,28)). */
#define RE15_GSZ_SIG_GATOR       28    /* 2090: Gator-Ablauf starten                                     */
#define RE15_GSZ_SIG_KNALL       29    /* 2090: Knall des Hereinbrechens oben in 2080 (Nachtrag 17)       */
#define RE15_GSZ_SIG_FERTIG      30    /* 2090: Port -> Programm: Gator-Ablauf fertig (Ankunfts-Poll)      */
#define RE15_GSZ_LEICHE_OBJ      2     /* ROOM2090 main00 @0x00A6C Obj_model_set obj 2 = treibende Zombie-Leiche (Bild Cut 0) */
#define RE15_GSZ_KEY_OBJ         4     /* Port-Prop (nOmodel = 4, RDT-Kopf Byte 2), Muster dining_r38 obj 6                */

/* Zustaende der Szene (nur RAM; waehrend der Szene kann weder gespeichert noch gekaempft werden). */
#define RE15_GSZ_AUS        0
#define RE15_GSZ_ANKUNFT    1   /* 2090: Programm 1 laeuft (Leiche), naechster Raum 2080        */
#define RE15_GSZ_MONTAGE    2   /* 2080: Programm 2 laeuft (Zombies kommen), naechster Raum 2090 */
#define RE15_GSZ_RUECKKEHR  3   /* 2090: Programm 3 laeuft (Leiche, dann Gator)                  */
#define RE15_GSZ_GATOR      4   /* 2090: Gator-Ablauf laeuft (Port-Takt)                         */
#define RE15_GSZ_KAMPF      5   /* 2090: Ablauf fertig, Kampf                                    */

/* HAKEN scd_room_setup.c (Ende des Installer-Blocks): Szene/Montage/Rueckkehr starten, Leiche/Schluessel herrichten. */
void re15_gatorszene_r41_install(uint16_t room_id);
/* HAKEN scd_vm.c scd_event_fire: Port-Programm fuer (Raum, RE15_GSZ_EREIGNIS, Zustand), sonst NULL. */
const uint8_t *re15_gatorszene_r41_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN scd_vm.c scd_vm_tick (jedes Bild, Gameplay-Lauf): Signale (5,28)/(5,29), Musik-Weiche. */
void re15_gatorszene_r41_tick(void);
/* HAKEN enemy_ai_boss_gator.c gb_takt (nach re15_gr2_takt): 0 = nichts, 1 = untergetaucht halten (Takt endet),
 * 2 = Szene bewegt den Gator (Takt endet, Boss taucht auf), 3 = Szene vorbei -> Boss in die Verfolgung (einmal). */
int  re15_gatorszene_r41_boss(int slot, void *gator_actor);
/* HAKEN enemy_ai_boss_gator.c re15_gator_boss_tick nach re15_gr2_nachtakt: Leiche + Schluessel an die Part-6-Matrix. */
void re15_gatorszene_r41_nachtakt(int slot, void *gator_actor);
/* PC-Anbindung (platform/pc/src/gatorszene_r41_pc.c, einmal beim Start): ENEMSE-SE des Gators (Bank 17). Ohne Anbindung
 * (Riegel) bleibt der Schrei stumm. */
void re15_gatorszene_r41_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int));
void re15_pc_gatorszene_r41_init(void);

/* ---- KNALL (Nachtrag 17, Wortlaut im Dossier): "Muss natuerlich ein knallen sein, als ob die Zombies rein brechen, so wie bei
 * STAGE1 von Elza/Leon am Ende in der Cutscene nach Irons." = der Tuerknall der Irons-Montage: irons_tod_1150.c k_p_szene
 * `OP_KNALL(RE15_IT_SIG_KNALL_TUER), OP_SLEEP(45)` vor dem Schnitt, Takt -> re15_audio_re2_tuer_se(KNALL_SE_TUER) auf der
 * Knall-Bank gen/knall_bank.inc Satz 0 = RE2 ROOM1060 sub05 Se_on 0x0a @0x00F48 (EDT 0x0a `00 00 85 20` -> VAG 5 @Datei
 * 0x066B8, 8768 B, sha1 192e09df...) - derselbe Satz wie Spur G (kanister2080, Nachtrag 17). Form "erst Knall, dann die
 * Zombies, dann der Schnitt" = Runde 36 Spur K (K3) der Irons-Montage. */

/* ---- 2080-EINSTELLUNG (Nachtrag 20, Wortlaut im Dossier): "In 2080, nachdem die Zombies durch die Tuer gebrochen sind, sollen
 * sie oberhalb von der Treppe laufen, nicht unten." - dieselbe Lage wie Spur G: Band 8 (oberer Laufgang; Treppe = Aot_set
 * Slot 2 sce 12 @0x0BE2 Band 5 -> 8), die acht Lagen von re15_kanister2080_r41.h k_zombies (Stand Spur G f78be367: "alle mit
 * Fuss UND Kopf im Bild von Cut 2"), Kamera 2080 Cut 2 (RID @0x0A0). */

/* ---- TOR 2090 <-> 20A0 (Nachtrag 21, Wortlaut im Dossier): "man kommt ueber die Leiter zum Aligator, nicht ueber das Tor. Das
 * Tor soll verschlossen sein, und dann mit den Schluessel den man vom Aligator bekommt geoeffnet werden!"
 * FORM = RE2-Tuer-Handler LAB_80051514 wie Runde 36 Spur Q / Runde 38 Spur F (schluessel_r36.c, dining_r38.c): der Tuer-Satz
 * wird zum sce-3-Ereignis umgewidmet (re15_aot_retype = Aot_reset @0x80040760-a8), der Druck prueft den Besitz im Inventar
 * (`jal FUN_800696cc` @0x80051628, `bltz` @0x80051634): mit Schluessel Text (@0x8005164c), Aufschliess-Ton (Se_on(2,0x25)
 * @0x80051654/58 = RE2_DOOR_SE_AUF_KEY), Wegwerf-Fortsetzung (@0x80051670), Schlossbit (@0x80051690) und der Satz wird
 * wieder die Original-Tuer (kein Durchgang bei diesem Druck, @0x80051698); ohne Schluessel Verschlossen-Ton (@0x800516a0/a4,
 * Art M der Mini-Bank) und Text (@0x800516b8).
 *   2090/2091 Slot 1 @0x009B6 (-> 20A0 (4950,-3600,4100) Cut 13): Ereignis RE15_GSZ_TOR_EREIGNIS, Texte 42/43 (VERTRAG
 *     2090/2091 42-46; ROOM2090 hat keinen Nachrichtenblock, RDT +0x3C = 0) = die Original-Saetze von ROOM20A0 msg 1
 *     @0x203C "It's locked." und msg 2 @0x204C "You've opened the lock." (bytegleich, dieselbe Tor-Gegend).
 *   20A0/20A1 Slot 0 @0x01746 (-> 2090 (6300,-1800,-16250) Cut 2): gesperrt, bis Bit 113 steht; der Druck zeigt die
 *     Original-Meldung msg 1 "It's locked." von 20A0 (aufgeschlossen wird an der 2090-Seite, wo man den Schluessel bekommt). */
#define RE15_GSZ_TOR_SLOT        1     /* ROOM2090/2091 main00 Door_aot_set @0x009B6 `3b 01 02 31 01 ..`                   */
#define RE15_GSZ_TOR_EREIGNIS    19    /* frei: ROOM2090 Sub-Tabelle @0x0ADC = 4 Subs; 18 = die Szene                      */
#define RE15_GSZ_TOR20A0_SLOT    0     /* ROOM20A0/20A1 main00 Door_aot_set @0x01746 `3b 00 02 31 02 ..`                   */
#define RE15_GSZ_TOR20A0_EREIGNIS 20   /* frei: ROOM20A0 Sub-Tabelle @0x1A24 `16 00` = 11 Subs; kein Port-Ereignis in 20A0  */
#define RE15_GSZ_MSG_ZU          42    /* 2090/2091: "It's locked."            (= ROOM20A0 msg 1 @0x203C)              */
#define RE15_GSZ_MSG_AUF         43    /* 2090/2091: "You've opened the lock." (= ROOM20A0 msg 2 @0x204C)              */
#define RE15_GSZ_MSG20A0_ZU      1     /* 20A0/20A1: Original msg 1 "It's locked." (@0x203C)                           */
int  re15_gatorszene_r41_tor_gesperrt(void);                              /* Pruefhaken: 1 = Tor-Satz umgewidmet     */
const uint8_t *re15_gatorszene_r41_tor_text(int nr, int *out_len);      /* Pruefhaken: 0 = msg 42, 1 = msg 43        */
/* Wegwerf-Kette (item_discard_common.c, RE2 @0x80051670): (2090/2091, msg 43) -> Water Key. */
int  re15_gatorszene_r41_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item);

/* ---- 3. SCHLUESSEL (Dossier §3): der nie platzierte RE1.5-"Water Key" (Namenstabelle DEBUG.BIN @0x800c495c Eintrag 0x45,
 * CHECK 'The label says: "Sewer Mainteinance Room."' @0x800c50de Eintrag 0x45, Icon ITEMALL.PIX Tile 0x45, Bild ITPS.ITP Block
 * 0x45) unter der nie platzierten Id 0x34 (0x45 = Parking Lot Key seit Runde 36 Spur Q); Weltmodell RE2 Item 0x63 Platform Key
 * (ROOM7040.RDT MD1 @0x00354C). Bits: 111 = liegt bei der Leiche, 112 = genommen. */
#define RE15_GSZ_KEY_ITEM        0x34
int  re15_gatorszene_r41_obj_id(uint16_t room_id);                       /* main.c Prop-Lader: 4 in 2090/2091, sonst -1 */
const uint8_t *re15_gatorszene_r41_md1_bytes(uint16_t room_id, int *n);
const uint8_t *re15_gatorszene_r41_tim_bytes(uint16_t room_id, int *n);
const uint8_t *re15_gatorszene_r41_name(uint8_t item_id);                 /* 0x07-terminiert (Namensblob-Form)          */
const uint8_t *re15_gatorszene_r41_desc(uint8_t item_id);                 /* CHECK-Text, 01 00-terminiert               */
int  re15_gatorszene_r41_ist_schluessel(uint8_t item_id);                 /* Wegwerf-Text "key" (item_prompt_common.c)  */
void re15_gatorszene_r41_icon_einsetzen(uint8_t *itemall, int size);      /* Tile 0x34 <- Original-Tile 0x45            */
void re15_gatorszene_r41_bild_einsetzen(uint8_t *itps, int size);         /* Block 0x34 <- Original-Block 0x45          */

/* Pruefhaken (kein Spielverhalten). Szenenfolge: je Ereignis (Art, Raum, Bild, Wert). */
typedef struct { char art; uint16_t raum; unsigned bild; int wert; int wert2; } re15_gsz_folge_t;
int  re15_gatorszene_r41_zustand(void);
int  re15_gatorszene_r41_folge_n(void);
const re15_gsz_folge_t *re15_gatorszene_r41_folge(int i);
const uint8_t *re15_gatorszene_r41_programm(int nr, int *out_len);
int  re15_gatorszene_r41_gator_phase(void);

#endif /* RE15_GATORSZENE_R41_H */
