/*
 * RE1.5 Rebuilt — Sprachaufnahmen der Port-Szenen (Runde 39 Spur A "stimmen").
 *
 * Herleitung, Belege und Form: include/re15_stimmen_r39.h und analysis/befunde_runde39/A_stimmen.md.
 * Kein Asset-Patch: die Nutzer-Dateien unter synchro/ bleiben, wie sie sind (VERTRAG §1.2); die Zuordnung steht hier.
 */
#include "re15_stimmen_r39.h"

#include <stdio.h>

/* Je Zeile: die Message_on-Stelle der Szene (Port-Programm bzw. RDT) und der gemessene Inhalt der Datei
 * (Diktat / Wahl unter den Raum-Saetzen / DTW-Nachbar unter den Roh-Exporten, Dossier §1). */
static const re15_stimmen_r39_t k_tabelle[] = {
    /* Punkt 1 — ROOM1050 Ada-Ruf (adaruf_1050.c k_ruf, Runde 34 Nacht Spur D / Runde 35 Spur E) */
    { 0x1050u, 22,  9 },   /* +0x1C `2b 16 00 00` "Woman: Hello? Anyone? Please, get me out of here!"  main09 4,284 s,
                            * Wahl 0,564, DTW Ada 22:34:51 1,367 */
    { 0x1050u, 23, 10 },   /* +0x50 `2b 17 00 00` "Leon: Another civilian survivor."                   main10 1,869 s,
                            * Diktat "Another civilian survivor" 0,935 */
    { 0x1050u, 24, 11 },   /* +0x80 `2b 18 00 00` "Leon: I have to help her!"                          main11 1,358 s,
                            * Diktat "I have to help her" 0,871 */
    /* Punkt 2 — ROOM1150 Irons' Tod (irons_tod_1150.c k_p_szene, Runde 35 Spur L); msg 29 seit Nachtrag 2 mit main22 */
    { 0x1150u, 22, 15 },   /* OP_MSG(22) "Leon: Sir!"                                  main15 0,720 s, DTW Leon 23:09:46 1,428 */
    { 0x1150u, 23, 16 },   /* OP_MSG(23) "Leon: Sir, the communication system ..."     main16 5,747 s, Diktat "... the
                            * communication system can't be fixed | to use the patrol car ..." */
    { 0x1150u, 24, 17 },   /* OP_MSG(24) "Leon: I came to get you, come with me!"      main17 2,183 s, Wahl 0,679 */
    { 0x1150u, 25, 18 },   /* OP_MSG(25) "Irons: Leon... I... I'm proud ..."           main18 6,049 s, Diktat "... proud to
                            * have an officer as dependable as you", DTW Irons 15:11:18 1,444 */
    { 0x1150u, 26, 19 },   /* OP_MSG(26) "Irons: But... I... I'm not going to make it..." main19 5,224 s, Diktat "But I I'm
                            * not going to make that I'm sorry" */
    { 0x1150u, 27, 20 },   /* OP_MSG(27) "Irons: Please... one last favor..."          main20 6,049 s, Wahl 0,892 */
    { 0x1150u, 28, 21 },   /* OP_MSG(28) "Irons: Get them out of here... Leon..."      main21 2,601 s, Wahl 0,765 */
    { 0x1150u, 29, 22 },   /* OP_MSG(29) "Leon: SIR, Sir?!"  main22 2,218 s — NUTZER-NACHTRAG 2 (2026-10-18 09:45, 32ab1f97:
                            * "das SIR, sir! habe ich ... mit main22 ergänzt"); gemessen (Dossier §9.5): zwei Rufe in der
                            * Huellkurve 0,15-0,55 s / 1,55-2,00 s (main15 "Sir!" = ein Ruf 0,10-0,50 s), Diktat "Slur" |
                            * "tour/sure/sur", F0 ~298 Hz = Leons Ruf wie main15 (281 Hz; Irons 89-121 Hz) */
    /* Punkt 3 — ROOM11B0 Marvin (gen/szl11b0_szene.inc, Runde 36 Spur J / Runde 38 Spur C) */
    { 0x11B0u, 20, 13 },   /* +0x0008 `2b 14 00 00` "Marvin: But, hold on..."                main13 1,707 s, DTW Marvin
                            * 16:45:42 1,686 / 16:45:25 1,756 / 15:59:15 1,849 (Diktat der drei: "But hold on") */
    { 0x11B0u, 21, 14 },   /* +0x010A `2b 15 00 00` "Marvin: I just want to make sure / we don't have any unwanted
                            * followers."  main14 3,936 s, Diktat "... want to make sure we don't have an unwanted
                            * followers", Wahl 0,690 */
    /* Punkt 4 — ROOM1171 Elza/John (RDT sub02) */
    { 0x1171u,  3,  2 },   /* @0x01586 `2b 03 00 00` "John: Alright, let's get down from here!"  main02 2,264 s, DTW
                            * Kendo 19:11:11 1,424 / 19:09:42 1,651 (Wahl der beiden Takes 0,411/0,522) */
};
#define STIMMEN_N ((int)(sizeof k_tabelle / sizeof k_tabelle[0]))

int re15_stimmen_r39_anzahl(void) { return STIMMEN_N; }

const re15_stimmen_r39_t *re15_stimmen_r39_eintrag(int i)
{
    return (i >= 0 && i < STIMMEN_N) ? &k_tabelle[i] : NULL;
}

int re15_stimmen_r39_stamm(unsigned room_id, int msg_id, char *stamm, size_t n)
{
    if (msg_id < 0 || msg_id > 255) return 0;
    for (int i = 0; i < STIMMEN_N; i++)
        if (k_tabelle[i].raum == room_id && (int)k_tabelle[i].nachricht == msg_id) {
            if (stamm && n) snprintf(stamm, n, "main%02u", (unsigned)k_tabelle[i].datei);
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[voice] ROOM%04X msg %d <- main%02u.wav (Runde 39 Spur A, Nutzer-Reihenfolge)\n",
                    room_id, msg_id, (unsigned)k_tabelle[i].datei);
#endif
            return 1;
        }
    for (int i = 0; i < STIMMEN_N; i++)
        if (k_tabelle[i].raum == room_id && (int)k_tabelle[i].datei == msg_id) {
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[voice] ROOM%04X msg %d stumm: main%02d.wav gehoert zu msg %u (Runde 39 Spur A)\n",
                    room_id, msg_id, msg_id, (unsigned)k_tabelle[i].nachricht);
#endif
            return -1;
        }
    return 0;
}

void re15_stimmen_r39_fehlt(unsigned room_id, int msg_id, const char *pfad)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[voice] ROOM%04X msg %d: keine Aufnahme (%s) - Zeile bleibt stumm\n",
            room_id, msg_id, pfad ? pfad : "?");
#else
    (void)room_id; (void)msg_id; (void)pfad;
#endif
}
