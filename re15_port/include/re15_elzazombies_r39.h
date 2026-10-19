/*
 * Runde 39 Spur Q "elzazombies" — Elza: Zombie-Durchbrueche in der Panikphase (Montage-Form wie Leon nach Irons' Tod).
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde39/Q_elzazombies.md.
 *
 * NUTZER (analysis/befunde_runde39/AUFTRAG.md, Nachtrag 10, woertlich): "Und auch hier sollen jetzt Zombies durchbrechen -
 * einmal in den Flur hinein von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden., die sollen
 * dann, wenn man in ROOM 1061 die Etage von unten ueber die Treppe erreicht aber moeglichst weit weg an der Seite stehen.
 * Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die Zombies nicht besiegt wurden und im ROOM 1031 ueber
 * die Absperrung/Das Rollo, so wie bei der cutscene."
 * NACHTRAG 11 (woertlich): "Ich korrigiere, die Zombies sollen nicht in ROM 1071 noch durchbrechen, sondern dann noch wie
 * bei leon im ROOM 1041."
 * ORCHESTRATOR (verbindlich): Elza-Raeume = ROOMxxx1; Ausloeser = steigende Flanke von Bank-9-Bit 97 (setzt NUR Spur P beim
 * 1041-Ruf, geloescht bei Johns Rettung); Montage-Form wie Leon; Gegner-Verhalten RE2-KI wie bisher; Bit 95 an Q.
 * Nachtrag 11: kein 1071-Durchbruch; 1041 wie Leons 1040 (Rolltor-Schnitt Cut 1); "falls die Zombies nicht besiegt
 * wurden" = die aus 10C1/1061; Zeitpunkt begruenden (Elza steht beim Start selbst in 1041), spielbar.
 * NACHTRAG 12 (woertlich, Teil 2): "Ausserdem habe ich mich entschlossen eine Angleichung zu machen der hereinbrechenden
 * Zombies. Bei Irons Tod und Johns Hilferuf sollen die Zombies in beiden Szenarien aus den gleichen Tueren brechen. Also bei
 * Leon zusaetzlich das Treppenhaus und bei Elza zusaetzlich ROOM 1071." Orchestrator: Leon 10C0 -> Treppenhaus 1060 in der
 * Montage nach Irons' Tod (Gegenstueck zu Elzas 10C1 -> 1061), Elza zusaetzlich ROOM1071 = Leons 1070-Schritt (1031 Cut 7);
 * der Rolltor-Durchbruch 1041 nur in der Panikphase ((9,97)=1, Entscheid zu Hinweis H1 der Abnahme 1).
 * RAUM-PAARE (cmp -l der ausgelieferten Dateien = 0 Bytes Unterschied): ROOM1060 = ROOM1061, ROOM10E0 = ROOM10E1, ROOM10C0 =
 * ROOM10C1, ROOM1070 = ROOM1071 — Leons Treppenhaus-Schritt ist Byte fuer Byte Elzas, Elzas 1071-Schritt Byte fuer Byte Leons.
 *
 * NUTZER-VORGABE / PORT-WAHL: RE1.5 hat diese Montage nicht. Die FORM ist Leons Montage (include/re15_irons_tod.h,
 * include/re15_montage_r38.h), deren Bausteine Original-Adressen tragen; jede Zeile der Programme traegt ihr Vorbild
 * (elzazombies_r39.c). Kein Asset-Patch.
 *
 * KETTE (Elza): Flanke (9,97) 0->1 im Spiel (nicht beim Laden) -> sobald keine Szene laeuft (re15_cine_active = (1,27) ||
 * (2,7), g_scd.player_mode != 2) startet im AKTUELLEN Raum (Spur P: ROOM1041) der Auftakt: Riegel, Pause, Knall ->
 *   [ROOM1061 Cut 2, nur wenn in 10E1 noch Zombies leben: die Ueberlebenden stehen hinter der 10C1-Tuer auf Etage 4]
 *   -> ROOM1031 [Cut 7, nur wenn in 1071 noch Zombies leben (Nachtrag 12): Leons 1070-Schritt, die Ueberlebenden vor der
 *      1071-Tuer, dann der Original-Knall @0x02862] + Original-Rollo-Szene: Rollo bricht (falls (4,15)=0) + Kriecher, Cut 12
 *      (= ROOM1031 sub08 @0x02850)
 *   -> Rueckkehr in den Startraum an die gemerkte Stelle (Lage, Gier, Schnitt), Riegel los.
 * DURCHBRUCH 1041 (Nachtrag 11): beim NAECHSTEN Betreten von 1041 durch eine Tuer im Suedteil, solange Zombies aus 1061
 * leben UND die Panikphase laeuft ((9,97)=1, Entscheid H1): sie ziehen nach 1041 um und brechen durchs Rolltor (Leons
 * 1040-Schritt, Cut 1), dann Steuerung zurueck.
 * SPAETERES BETRETEN ((9,95)=1): ROOM1061 = die 10E1-Kopien moeglichst weit weg vom Ankunftspunkt der Treppe (Etage 4) an
 * der Westseite, angehalten (entity+0x9 Bit 0x20) bis die Spielerin Etage 4 erreicht; ROOM1031 = die 1071-Kopien + die
 * Kriecher (Leons ROOM1030-Nachspawn k_p_1030_nach); ROOM1041 = die Durchgebrochenen hinter dem Rolltor (wie Leons
 * k_p_1040_nach).
 * LEON (Nachtrag 12): in der Montage nach Irons' Tod (irons_tod_1150.c) nach dem 1130-Schritt (bzw. direkt nach der Szene,
 * wenn 1140 leer ist) der Treppenhaus-Schritt ROOM1060 Cut 2 mit den in 10E0 am Leben gelassenen Zombies, dann weiter wie
 * bisher nach 1040. Spaeteres Betreten von 1060 ((9,73)=1): dieselben Kopien an der Westseite, gehalten bis Etage 4.
 */
#ifndef RE15_ELZAZOMBIES_R39_H
#define RE15_ELZAZOMBIES_R39_H

#include <stdint.h>

/* ---- Bits ---------------------------------------------------------------------------------------------------------- */
/* Ausloeser: Bank 9 Bit 97 = Elza-Panikphase (VERTRAG 1.1: "P = 97 ... Q und R lesen es"). Nur gelesen. */
#define RE15_R39Q_PANIK_BANK      9
#define RE15_R39Q_PANIK_BIT       97
/* Bank 9 Bit 95 = "Elza-Montage gelaufen, Kopien existieren" (VERTRAG 1.1: 95 seit Nachtrag 10 an Q). Kein Original-/Port-
 * Flag reicht: (9,97) wird bei Johns Rettung geloescht (Spur P), die Kopien bleiben aber (Leons Kopien bleiben ebenso,
 * (9,74)/(9,76)); die Quell-Tot-Bits (7,0xe0..)/(7,0xc6..) unterscheiden "getoetet" nicht von "umgezogen". Gespeichert mit
 * g_game.flags; eingetragen in tests/unit/test_r36_kartenbits.c k_port[]. */
#define RE15_R39Q_BANK            9
#define RE15_R39Q_BIT_MONTAGE     95

/* ---- Raeume, Ereignis, Tuer-Slot, Signale -------------------------------------------------------------------------- */
#define RE15_R39Q_RAUM_1041       0x1041   /* Spur P: Johns Ruf -> Startraum der Montage (gelesen: aktueller Raum) */
#define RE15_R39Q_RAUM_1061       0x1061   /* UPPER STAIRS (DEBUG.BIN Satz 0x06) */
#define RE15_R39Q_RAUM_1031       0x1031   /* LOBBY (Satz 0x03) */
#define RE15_R39Q_RAUM_10E1       0x10E1   /* MED. EXAM ROOM (Satz 0x0E) = "medical room" */
#define RE15_R39Q_RAUM_1071       0x1071   /* LOBBY OFFICE (Satz 0x07) — Nachtrag 12: Quelle des 1031-Cut-7-Teils (Nachtrag 11 hatte ihn gestrichen) */
#define RE15_R39Q_RAUM_1060       0x1060   /* Leon: UPPER STAIRS = ROOM1061 Byte fuer Byte (Nachtrag 12, Treppenhaus-Schritt) */
#define RE15_R39Q_RAUM_10E0       0x10E0   /* Leon: MED. EXAM ROOM = ROOM10E1 Byte fuer Byte */
/* Port-Ereignis der Programme: < RE15_RDT_MAX_SUB_SCD (32); kein Sub in 1041 (12 Subs), 1061 (2), 1031 (16); die Port-
 * Ereignisse anderer Spuren sind 2,3,4,5,6,13,20..29 (Zensus der RE15_*_EREIGNIS in include/); seit dem master-Merge der
 * Nachbesserung 2 nutzt Spur R (hund1121_r39.c) Ereignis 30 NUR in ROOM1121, diese Spur nur in 1041/1061/1031/1060 — die
 * Weichen in scd_event_fire stehen hintereinander. Jede Abfrage prueft Raum UND Zustand, ein gleichnamiges fremdes Ereignis
 * in einem anderen Zustand bleibt unberuehrt. ROOM1060 (Leon) hat 2 Subs (sub00/sub01 = Evt_end). */
#define RE15_R39Q_EREIGNIS        30
/* Tuer-Slot der Montage-Schnitte (Door_aot_set + Aot_on): frei in 1041 (Slots 0..8), 1061 (0..10, 16), 1031 (0..19)
 * — Zensus aller Aot_set/Door_aot_set/Item_aot_set/Aot_reset/Aot_on aller Faeden; RVD-Zonen belegen den Pool von oben. */
#define RE15_R39Q_TUER_SLOT       20
/* Knall-Signale Bank 5 Wort 0 (Raum-Scratch, geloescht beim Raum-Init FUN_8003ecec @0x8003ed74) — dieselbe Bauart wie
 * Leons (5,28)/(5,29) (re15_irons_tod.h): 28 = Tuerknall (Knall-Bank Satz 0), 29 = Knall wie ROOM1030 (Satz 1). Frei in
 * 1041 und 1031 (Bank-5-Zensus: Bits 0..20 + 32..35) und 1061 (keins). */
#define RE15_R39Q_SIG_KNALL_TUER  28
#define RE15_R39Q_SIG_KNALL_1030  29
/* "Alle durch"-Signal des Durchbruchs 1041 = Leons (5,30) RE15_IT_SIG_ALLE_DURCH: der Takt setzt es, das Programm wartet mit
 * der Ankunfts-Poll-Form ROOM1150 sub08 @0x0112A..@0x01132. Frei in 1041 (Bank-5-Zensus: Bits 0..20 + 32..35). */
#define RE15_R39Q_SIG_ALLE_DURCH  30

/* ---- Quellen der Kopien (Tot-Bits = Sce_em_set pc[7], Zone 7 = re15_em_status_zone fuer Stage 1-3, Gate @0x80042128-38) */
/* ROOM10E1 sub00 (= ROOM10E0 Byte fuer Byte): Fresser Typ 0x10 grid 0x06 @0x00D56/@0x00D6A/@0x00D7E/@0x00D92 bzw. Steher
 * grid 0x02 @0x00DBE.. (Else-Zweig nach (4,246)=1), Tot-Bits 0xe0..0xe3. "Am Leben gelassen" = Tot-Bit 0. */
#define RE15_R39Q_BIT_10E1_0      0xe0
#define RE15_R39Q_N_10E1          4
#define RE15_R39Q_N_KRIECHER      3   /* wie Leons RE15_IT_N_KRIECHER (Kriecher durch das Tor, 1030-Szene sub08 viermal sub09) */
/* ROOM1071 sub00 (= ROOM1070 Byte fuer Byte, Leons RE15_IT_BIT_1070_0): Liegende Typ 0x10/0x10/0x10/0x11/0x11 @0x015CA/@0x015DE/
 * @0x015F2/@0x01606/@0x0161A `44 0i tt 88|87 00 00 p6 c6+i ..` bzw. Steher grid 0x00 @0x01632.. (Else nach (4,197)=1),
 * Tot-Bits 0xc6..0xca. "Falls nicht besiegt" = Tot-Bit 0 (Leons re15_irons_tod_lebend(1)). */
#define RE15_R39Q_BIT_1071_0      0xc6
#define RE15_R39Q_N_1071          5

/* ---- Schnitte, Etage ----------------------------------------------------------------------------------------------- */
#define RE15_R39Q_CUT_1061        2    /* ROOM10C1 Tuer Slot 0 @0x00E42: Zielschnitt `02` (Nutzlast +10); RID @0x000A0 */
#define RE15_R39Q_CUT_1031_TOR    12   /* ROOM1031 sub08 Cut_chg `29 0c` @0x02896 (Rollo aufgebrochen) */
#define RE15_R39Q_CUT_1031_TUER   7    /* Leons RE15_IT_CUT_1030_TUER: Kamera auf die 1070/1071-Tuer, RID @0x00140 (1030 = 1031 bis auf den
                                        * PRI-Zeiger +28: pos (-26820,-3114,-2574) target (-15858,-936,-5040)) */
#define RE15_R39Q_CUT_1041        1    /* ROOM1041 sub08 Cut_chg `29 01` @0x01A1A = Leons Rolltor-Schnitt; RID @0x00080 */
/* Etage 4 = y -7200: Tuer ROOM10C1 @0x00E42 Nutzlast y `e0 e3`, Etagen-Byte `04`; ROOM1061 Tuer Slot 1 @0x00D32 pc[4] = 4;
 * SCA-Band 4 = Zellen mit floor-Byte 0x43 (`>>4` = 4, Band-Gate @0x8003b8a0-b0). Sce_em_set traegt die Etage in pc[4]
 * und y = -pc[4]*1800 (ROOM1090 sub00 @0x02214 `.. 01 00 .. f8 f8`; Enemy +0x82 = pc[4] @0x800421c8/@0x800421d0). */
#define RE15_R39Q_ETAGE_1061      4
#define RE15_R39Q_Y_1061          (-7200)

/* ---- Zustand der Kette (nur RAM; waehrend der Montage kann weder gespeichert noch gestorben werden) ------------------ */
#define RE15_R39Q_AUS        0
#define RE15_R39Q_AUFTAKT    1   /* Startraum: Riegel, Pause, Knall, Tuer zum ersten Schritt */
#define RE15_R39Q_S1061      2   /* ROOM1061 Cut 2 */
#define RE15_R39Q_S1031      3   /* ROOM1031 [Cut 7, 1071-Kopien (Nachtrag 12)] + Cut 12 (Rollo) */
#define RE15_R39Q_RUECKKEHR  4   /* Startraum: Riegel los */
#define RE15_R39Q_D1041      5   /* Durchbruch 1041 (Nachtrag 11): Rolltor-Szene beim naechsten Betreten */

/* ---- Leon (Nachtrag 12): Treppenhaus-Schritt in der Montage nach Irons' Tod. Haken in irons_tod_1150.c (je 1 Zeile, "Runde 39
 * Spur Q"): umzug_vorbereiten, programm_bauen (Weiche der Tuer nach 1130 bzw. nach der Szene, Programm des Schritts),
 * re15_irons_tod_install (ROOM1060 -> Zustand RE15_IT_S1060, ROOM1040 nimmt S1060 als Vorgaenger). */
/* Beim Szenenstart (Leons umzug_vorbereiten): die 10E0-Ueberlebenden ziehen nach 1060 um (Quelle 1, Kopie 124+i = 0; schon tote:
 * Kopie 1). Merkt die Zahl fuer die Weiche. */
void re15_r39q_leon_umzug(void);
int  re15_r39q_leon_1060_zahl(void);                       /* lebende 10E0-Zombies beim Szenenstart (0 = kein 1060-Schritt) */
/* Weiche: den LETZTEN Tuersatz eines Leon-Programms (Form irons_tod_1150.c OP_DOOR + Aot_on + Evt_end, davor Knall + Sleep 45)
 * auf ROOM1060 Cut 2 Etage 4 (Parkplatz hinter der Kamera) stellen, Knall = Tuerknall Satz 0; nur wenn re15_r39q_leon_1060_zahl()
 * > 0. */
void re15_r39q_leon_tuer(uint8_t *prog, int len);
/* Programm des 1060-Schritts in `prog` (max Byte): Kopien hinter der 10C0-Tuer (Raumaufbau), Riegel, 130 Bilder, Knall Satz 1,
 * 45 Bilder, Tuer -> ROOM1040 Cut 1 (Leons Parkplatz). Ergebnis = Laenge (0 = passt nicht). */
int  re15_r39q_leon_1060_bauen(uint8_t *prog, int max);

/* HAKEN scd_room_setup.c (Tuer-Weg, nach dem Init-Lauf, Ende des Installer-Blocks): Montage-Schritt / Rueckkehr /
 * Nachspawn 1061/1031 (Elza) bzw. 1060 (Leon). */
void re15_elzazombies_r39_install(uint16_t room_id);
/* HAKEN main.c (Boot-/CONTINUE-Weg ohne scd_room_reenter, nach dem Restore der Flags): Kette AUS, die Flanke von (9,97)
 * ist "gesehen" (Laden ist kein Spielereignis), dann wie install (Nachspawns). */
void re15_elzazombies_r39_geladen(uint16_t room_id);
/* HAKEN scd_event_fire: Port-Programm fuer (Raum, Ereignis 30, Zustand) oder NULL. */
const uint8_t *re15_elzazombies_r39_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN scd_vm_tick: 1 = der Reseed von sub01 unterbleibt (Montage-Schritt in 1061/1031, Durchbruch 1041). Leons 1060-Schritt
 * sperrt ueber re15_irons_tod_sub01_gesperrt (Zustand RE15_IT_S1060). */
int  re15_elzazombies_r39_sub01_gesperrt(void);
/* HAKEN scd_vm_tick (jedes Spielbild, Kopf des VM-Laufs): Flanke, Start, Knall-Signale, Verhalten in den Schnitten,
 * Halten/Freigeben der 1061-Kopien, Messung. */
void re15_elzazombies_r39_tick(void);

/* ---- Pruefhaken (kein Spielverhalten) ------------------------------------------------------------------------------ */
int  re15_elzazombies_r39_zustand(void);
void re15_elzazombies_r39_zustand_setzen(int z);
int  re15_elzazombies_r39_ausstehend(void);
int  re15_elzazombies_r39_lebend_10e1(void);
int  re15_elzazombies_r39_lebend_1061(void);   /* lebende 1061-Kopien (Bedingung des Durchbruchs 1041) */
int  re15_elzazombies_r39_lebend_1041(void);   /* lebende 1041-Kopien (Nachspawn 1041) */
int  re15_elzazombies_r39_durchbruch_zahl(void);   /* Zahl der beim letzten Durchbruch umgezogenen Kopien */
const uint8_t *re15_elzazombies_r39_laufprogramm(int *out_len);
/* Kopie-Tot-Bits (Zone 7) — Zensus im Riegel: 1061/1060 (4), 1041 (4), Kriecher (3), 1031-Cut-7 (5) = 16 in dieser Reihenfolge.
 * Die 1061-Bits gelten auch fuer Leons 1060-Kopien, die 1031-Cut-7-Bits sind Leons 1030-Cut-7-Bits (47,48,49,75,76): dieselben
 * Zombies in beiden Szenarien tragen dasselbe Bit — wie im Original die Quell-Records der Raum-Paare (ROOM10E0/10E1 0xe0..0xe3,
 * ROOM1070/1071 0xc6..0xca, Dateien byte-gleich). Leon und Elza teilen nie einen Spielstand. */
int  re15_elzazombies_r39_kopiebits(const uint8_t **out);
int  re15_elzazombies_r39_lebend_1071(void);   /* lebende 1071-Records (Tot-Bits 0xc6..0xca = 0) */
/* Lagen (Riegel / Messung): Montage-Spawn 1061 (Tuer-Packung), Nachspawn 1061 (Weit-weg-Packung), 1041 (Gassen). */
typedef struct { int16_t x, z, gier; } re15_r39q_lage_t;
int  re15_elzazombies_r39_lagen(int welche /* 0 = Montage 1061, 1 = Nachspawn 1061, 2 = 1041 */, const re15_r39q_lage_t **out);
void re15_elzazombies_r39_park_1061(int32_t *x, int32_t *z);
/* Rueckkehr-Stelle (Lage/Gier/Schnitt beim Start gemerkt). */
void re15_elzazombies_r39_rueck(uint16_t *raum, int32_t *x, int32_t *y, int32_t *z, int16_t *gier, uint8_t *cut);
/* Halten der 1061-Kopien (Nachspawn): 1 = noch gehalten (Spielerin nicht auf Etage 4). */
int  re15_elzazombies_r39_1061_gehalten(void);
/* Ereignisfolge der Montage (wie Leons re15_irons_tod_folge): 'K' Knall (wert = Satz 0/1, 2 = Original-Se_on ROOM1031
 * @0x02862), 'S' Spawn (wert = Zahl), 'C' Kamera (wert = Cut), 'R' Raum (wert = Raum-Id). */
typedef struct { char art; uint16_t raum; int takt; int wert; } re15_r39q_folge_t;
int  re15_elzazombies_r39_folge_n(void);
const re15_r39q_folge_t *re15_elzazombies_r39_folge(int i);
void re15_elzazombies_r39_reset(void);   /* RAM-Zustand loeschen (Riegel/Sonden) */
/* Nur fuer Riegel/Sonden: 0 = Stand vor Spur Q (keine Flanke, kein Nachspawn) als Gegenprobe im selben Binary. */
void re15_elzazombies_r39_enable(int an);
/* MESSUNG je Montage-Schnitt (Raum, Cut) und Zombie: Abstand zur Bodenprojektion der Kamera im ersten/letzten Bild des
 * Abschnitts, Lagen, Abschnitts-Bild des Einzelstopps (0 = nie). debug.log `[elzazombies-r39] ROOMxxxx Cut c Bild n: ...`
 * beim Ende jedes Abschnitts. Reset beim Start der Kette. */
typedef struct { uint16_t raum; uint8_t cut, aktor, bit; int b0, b1; int32_t d0, d1, x0, z0, x1, z1; int halt; } re15_r39q_spur_t;
int  re15_elzazombies_r39_spuren(const re15_r39q_spur_t **out);
void re15_elzazombies_r39_abschnitt_beenden(void);   /* Riegel: laufenden Abschnitt schliessen (exe: Raumwechsel) */
int  re15_elzazombies_r39_hp_min(void);              /* kleinste Spieler-HP ueber die Kette */
/* Freigabe der 1061-Kopien (Nachspawn): Spielerlage beim Erreichen der Etage, kleinster Abstand einer Kopie; Ergebnis =
 * Zahl der freigegebenen Kopien (0 = noch keine Freigabe). */
int  re15_elzazombies_r39_freigabe(int32_t *px, int32_t *pz, int32_t *dmin);

#endif /* RE15_ELZAZOMBIES_R39_H */
