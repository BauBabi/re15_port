/*
 * Runde 39 Spur Q "elzazombies" — Elza: Zombie-Durchbrueche in der Panikphase (Montage-Form wie Leon nach Irons' Tod).
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde39/Q_elzazombies.md.
 *
 * NUTZER (analysis/befunde_runde39/AUFTRAG.md, Nachtrag 10, woertlich): "Und auch hier sollen jetzt Zombies durchbrechen -
 * einmal in den Flur hinein von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden., die sollen
 * dann, wenn man in ROOM 1061 die Etage von unten ueber die Treppe erreicht aber moeglichst weit weg an der Seite stehen.
 * Dann das gleiche nochmal wie bei Leon in den ROOM 1071, falls die Zombies nicht besiegt wurden und im ROOM 1031 ueber
 * die Absperrung/Das Rollo, so wie bei der cutscene."
 * ORCHESTRATOR (verbindlich): Elza-Raeume = ROOMxxx1; Ausloeser = steigende Flanke von Bank-9-Bit 97 (setzt NUR Spur P beim
 * 1041-Ruf, geloescht bei Johns Rettung); Montage-Form wie Leon; Gegner-Verhalten RE2-KI wie bisher; Bit 95 an Q.
 *
 * NUTZER-VORGABE / PORT-WAHL: RE1.5 hat diese Montage nicht. Die FORM ist Leons Montage (include/re15_irons_tod.h,
 * include/re15_montage_r38.h), deren Bausteine Original-Adressen tragen; jede Zeile der Programme traegt ihr Vorbild
 * (elzazombies_r39.c). Kein Asset-Patch.
 *
 * KETTE (Elza): Flanke (9,97) 0->1 im Spiel (nicht beim Laden) -> sobald keine Szene laeuft (re15_cine_active = (1,27) ||
 * (2,7), g_scd.player_mode != 2) startet im AKTUELLEN Raum (Spur P: ROOM1041) der Auftakt: Riegel, Pause, Knall ->
 *   [ROOM1061 Cut 2, nur wenn in 10E1 noch Zombies leben: die Ueberlebenden stehen hinter der 10C1-Tuer auf Etage 4]
 *   -> [ROOM1031 Cut 7, nur wenn in 1071 noch Zombies leben: Tuerknall + Kopien vor der 1071-Tuer]
 *   -> ROOM1031 Original-Knall + Rollo bricht (falls (4,15)=0) + Kriecher -> Cut 12 (= ROOM1031 sub08 @0x02850)
 *   -> Rueckkehr in den Startraum an die gemerkte Stelle (Lage, Gier, Schnitt), Riegel los.
 * SPAETERES BETRETEN ((9,95)=1): ROOM1061 = die 10E1-Kopien moeglichst weit weg vom Ankunftspunkt der Treppe (Etage 4) an
 * der Westseite, angehalten (entity+0x9 Bit 0x20) bis die Spielerin Etage 4 erreicht; ROOM1031 = die 1071-Kopien + Kriecher
 * wie Leons ROOM1030-Nachspawn.
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
#define RE15_R39Q_RAUM_1071       0x1071   /* LOBBY OFFICE (Satz 0x07) */
/* Port-Ereignis der Programme: < RE15_RDT_MAX_SUB_SCD (32); kein Sub in 1041 (12 Subs), 1061 (2), 1031 (16); die Port-
 * Ereignisse anderer Spuren sind 2,3,4,5,6,13,20..29 (Zensus der RE15_*_EREIGNIS in include/). Jede Abfrage prueft
 * Raum UND Zustand, ein gleichnamiges fremdes Ereignis in einem anderen Zustand bleibt unberuehrt. */
#define RE15_R39Q_EREIGNIS        30
/* Tuer-Slot der Montage-Schnitte (Door_aot_set + Aot_on): frei in 1041 (Slots 0..8), 1061 (0..10, 16), 1031 (0..19)
 * — Zensus aller Aot_set/Door_aot_set/Item_aot_set/Aot_reset/Aot_on aller Faeden; RVD-Zonen belegen den Pool von oben. */
#define RE15_R39Q_TUER_SLOT       20
/* Knall-Signale Bank 5 Wort 0 (Raum-Scratch, geloescht beim Raum-Init FUN_8003ecec @0x8003ed74) — dieselbe Bauart wie
 * Leons (5,28)/(5,29) (re15_irons_tod.h): 28 = Tuerknall (Knall-Bank Satz 0), 29 = Knall wie ROOM1030 (Satz 1). Frei in
 * 1041 und 1031 (Bank-5-Zensus: Bits 0..20 + 32..35) und 1061 (keins). */
#define RE15_R39Q_SIG_KNALL_TUER  28
#define RE15_R39Q_SIG_KNALL_1030  29

/* ---- Quellen der Kopien (Tot-Bits = Sce_em_set pc[7], Zone 7 = re15_em_status_zone fuer Stage 1-3, Gate @0x80042128-38) */
/* ROOM10E1 sub00 (= ROOM10E0 Byte fuer Byte): Fresser Typ 0x10 grid 0x06 @0x00D56/@0x00D6A/@0x00D7E/@0x00D92 bzw. Steher
 * grid 0x02 @0x00DBE.. (Else-Zweig nach (4,246)=1), Tot-Bits 0xe0..0xe3. "Am Leben gelassen" = Tot-Bit 0. */
#define RE15_R39Q_BIT_10E1_0      0xe0
#define RE15_R39Q_N_10E1          4
/* ROOM1071 sub00 (= ROOM1070): 5 Records Typen 0x10,0x10,0x10,0x11,0x11 @0x015CA.. / Steher-Zweig @0x01632.., Tot-Bits
 * 0xc6..0xca (= Leons RE15_IT_BIT_1070_0). */
#define RE15_R39Q_BIT_1071_0      0xc6
#define RE15_R39Q_N_1071          5
#define RE15_R39Q_N_KRIECHER      3   /* wie Leons RE15_IT_N_KRIECHER (Kriecher durch das Tor, 1030-Szene sub08 viermal sub09) */

/* ---- Schnitte, Etage ----------------------------------------------------------------------------------------------- */
#define RE15_R39Q_CUT_1061        2    /* ROOM10C1 Tuer Slot 0 @0x00E42: Zielschnitt `02` (Nutzlast +10); RID @0x000A0 */
#define RE15_R39Q_CUT_1031_TUER   7    /* ROOM1031 RID @0x00140 = ROOM1030 Cut 7 (Kamera auf die 1071-Tuer) */
#define RE15_R39Q_CUT_1031_TOR    12   /* ROOM1031 sub08 Cut_chg `29 0c` @0x02896 (Rollo aufgebrochen) */
/* Etage 4 = y -7200: Tuer ROOM10C1 @0x00E42 Nutzlast y `e0 e3`, Etagen-Byte `04`; ROOM1061 Tuer Slot 1 @0x00D32 pc[4] = 4;
 * SCA-Band 4 = Zellen mit floor-Byte 0x43 (`>>4` = 4, Band-Gate @0x8003b8a0-b0). Sce_em_set traegt die Etage in pc[4]
 * und y = -pc[4]*1800 (ROOM1090 sub00 @0x02214 `.. 01 00 .. f8 f8`; Enemy +0x82 = pc[4] @0x800421c8/@0x800421d0). */
#define RE15_R39Q_ETAGE_1061      4
#define RE15_R39Q_Y_1061          (-7200)

/* ---- Zustand der Kette (nur RAM; waehrend der Montage kann weder gespeichert noch gestorben werden) ------------------ */
#define RE15_R39Q_AUS        0
#define RE15_R39Q_AUFTAKT    1   /* Startraum: Riegel, Pause, Knall, Tuer zum ersten Schritt */
#define RE15_R39Q_S1061      2   /* ROOM1061 Cut 2 */
#define RE15_R39Q_S1031      3   /* ROOM1031 Cut 7 / Cut 12 */
#define RE15_R39Q_RUECKKEHR  4   /* Startraum: Riegel los */

/* HAKEN scd_room_setup.c (Tuer-Weg, nach dem Init-Lauf, Ende des Installer-Blocks): Montage-Schritt / Rueckkehr /
 * Nachspawn 1061/1031. */
void re15_elzazombies_r39_install(uint16_t room_id);
/* HAKEN main.c (Boot-/CONTINUE-Weg ohne scd_room_reenter, nach dem Restore der Flags): Kette AUS, die Flanke von (9,97)
 * ist "gesehen" (Laden ist kein Spielereignis), dann wie install (Nachspawns). */
void re15_elzazombies_r39_geladen(uint16_t room_id);
/* HAKEN scd_event_fire: Port-Programm fuer (Raum, Ereignis 30, Zustand) oder NULL. */
const uint8_t *re15_elzazombies_r39_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN scd_vm_tick: 1 = der Reseed von sub01 unterbleibt (Montage-Schritt in 1061/1031). */
int  re15_elzazombies_r39_sub01_gesperrt(void);
/* HAKEN scd_vm_tick (jedes Spielbild, Kopf des VM-Laufs): Flanke, Start, Knall-Signale, Verhalten in den Schnitten,
 * Halten/Freigeben der 1061-Kopien, Messung. */
void re15_elzazombies_r39_tick(void);

/* ---- Pruefhaken (kein Spielverhalten) ------------------------------------------------------------------------------ */
int  re15_elzazombies_r39_zustand(void);
void re15_elzazombies_r39_zustand_setzen(int z);
int  re15_elzazombies_r39_ausstehend(void);
int  re15_elzazombies_r39_lebend_10e1(void);
int  re15_elzazombies_r39_lebend_1071(void);
const uint8_t *re15_elzazombies_r39_laufprogramm(int *out_len);
/* Kopie-Tot-Bits (Zone 7) — Zensus im Riegel: 1061 (4), 1031 (5), Kriecher (3) = 12 in dieser Reihenfolge. */
int  re15_elzazombies_r39_kopiebits(const uint8_t **out);
/* Lagen (Riegel / Messung): Montage-Spawn 1061 (Tuer-Packung), Nachspawn 1061 (Weit-weg-Packung). */
typedef struct { int16_t x, z, gier; } re15_r39q_lage_t;
int  re15_elzazombies_r39_lagen(int welche /* 0 = Montage 1061, 1 = Nachspawn 1061 */, const re15_r39q_lage_t **out);
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
