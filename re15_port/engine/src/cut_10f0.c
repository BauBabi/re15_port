/*
 * RE1.5 Rebuilt — Neue Szene beim ERSTEN Betreten von ROOM10F0 (Ada / Leon / Marvin), danach
 * Kartenhinweis ROOM11C0 -> ROOM1150 und MAIN01 bis zum Parkplatz (Runde 35, Spur K).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_cut10f0.h und
 * analysis/befunde_runde35/K_cut10f0.md. Kein Asset-Patch: die RDT bleibt byte-true, der Port spielt die
 * Szene als Bytecode aus ORIGINAL-Opcodes ueber die vorhandene VM (gen/cut10f0_szene.inc, Generator
 * tools/r35_k/szene_bauen.py — dort das Vorbild jeder Opcode-Form mit Datei-Offset).
 */
#include "re15_cut10f0.h"

#include "re15_actor.h"
#include "re15_audio.h"
#include "re15_engine.h"        /* g_engine.frame_count (Logzeile der Hinweiskette) */
#include "re15_inv_screen.h"    /* g_inv_screen: Hinweis-/Zielfelder des Statusschirms */
#include "re15_map_hint.h"
#include "re15_msg.h"
#include "re15_room.h"
#include "re15_scd.h"
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>   /* getenv der Logzeilen RE15_SE_DEBUG / RE15_BGM_CTL_DEBUG */
#endif

extern unsigned g_current_room_id;

/* ---- Das Programm (1350 Bytes, 288 Opcodes) und der RE2-Tuerton ---------------------------------- */
#include "gen/cut10f0_szene.inc"
#include "gen/cut10f0_tuerton.inc"

/* ---- Nachrichten (.msg-Rohbytes). NEUE TEXTE (Nutzervorgabe AUFTRAG.md Z.44-63, Wortlaut
 * unveraendert; Zeilenumbrueche = Port-Satz unter der breitesten ausgelieferten Dialogzeile, 285 px,
 * ROOM30E0 msg 13 @0x13FD). Belegt ist die FORM und jede Glyphe (Umkehrung von msg_common.c re15_msg_glyph):
 *   Kopf `04 00 05 cc <Sprecher> 16 05 00 00`, Ende `04 01 01 63` (99 Bilder Standzeit):
 *     Leon   cc=01  ROOM1090 msg 1 @0x279C, ROOM11C0 msg 0 @0x1C98
 *     Woman  cc=02  ROOM1090 msg 0 @0x275C — Nutzer-Konvention vor der Vorstellung (Runde 34 Nacht)
 *     Ada    cc=02  ROOM11C0 msg 1 @0x1CD0 (nach "... Ada, Ada Wong")
 *     Marvin cc=07  ROOM10D0 msg 14 @0x211E `04 00 05 07 29 3d 4e 52 45 4a 16 05 00 00`
 *   Auslassung "..." = `57 57 57` (ROOM11C0 msg 8 @0x1E14), Umbruch 0x08, Leerzeichen 0x00, '-' 0x3B,
 *   Apostroph 0x3A, '!' 0x1A, '?' 0x1B, ',' 0x18, '.' 0x57 (tools/r35_k/texte_bauen.py). */
static const uint8_t k_msg06[] = {   /* Leon: Hey - how did you came in here? */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x24, 0x41, 0x55, 0x00,
    0x3b, 0x00, 0x44, 0x4b, 0x53, 0x00, 0x40, 0x45, 0x40, 0x00, 0x55, 0x4b, 0x51, 0x00, 0x3f, 0x3d,
    0x49, 0x41, 0x00, 0x45, 0x4a, 0x00, 0x44, 0x41, 0x4e, 0x41, 0x1b, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg07[] = {   /* Woman: Did you really think there was / only one staff card for the / Communication Room? */
    0x04, 0x00, 0x05, 0x02, 0x33, 0x4b, 0x49, 0x3d, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x20, 0x45, 0x40,
    0x00, 0x55, 0x4b, 0x51, 0x00, 0x4e, 0x41, 0x3d, 0x48, 0x48, 0x55, 0x00, 0x50, 0x44, 0x45, 0x4a,
    0x47, 0x00, 0x50, 0x44, 0x41, 0x4e, 0x41, 0x00, 0x53, 0x3d, 0x4f, 0x08, 0x4b, 0x4a, 0x48, 0x55,
    0x00, 0x4b, 0x4a, 0x41, 0x00, 0x4f, 0x50, 0x3d, 0x42, 0x42, 0x00, 0x3f, 0x3d, 0x4e, 0x40, 0x00,
    0x42, 0x4b, 0x4e, 0x00, 0x50, 0x44, 0x41, 0x08, 0x1f, 0x4b, 0x49, 0x49, 0x51, 0x4a, 0x45, 0x3f,
    0x3d, 0x50, 0x45, 0x4b, 0x4a, 0x00, 0x2e, 0x4b, 0x4b, 0x49, 0x1b, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg08[] = {   /* Woman: Anyway... the communication / system is completely destroyed. */
    0x04, 0x00, 0x05, 0x02, 0x33, 0x4b, 0x49, 0x3d, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x1d, 0x4a, 0x55,
    0x53, 0x3d, 0x55, 0x57, 0x57, 0x57, 0x00, 0x50, 0x44, 0x41, 0x00, 0x3f, 0x4b, 0x49, 0x49, 0x51,
    0x4a, 0x45, 0x3f, 0x3d, 0x50, 0x45, 0x4b, 0x4a, 0x08, 0x4f, 0x55, 0x4f, 0x50, 0x41, 0x49, 0x00,
    0x45, 0x4f, 0x00, 0x3f, 0x4b, 0x49, 0x4c, 0x48, 0x41, 0x50, 0x41, 0x48, 0x55, 0x00, 0x40, 0x41,
    0x4f, 0x50, 0x4e, 0x4b, 0x55, 0x41, 0x40, 0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg09[] = {   /* Woman: We won't reach anyone with it / anymore... */
    0x04, 0x00, 0x05, 0x02, 0x33, 0x4b, 0x49, 0x3d, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x33, 0x41, 0x00,
    0x53, 0x4b, 0x4a, 0x3a, 0x50, 0x00, 0x4e, 0x41, 0x3d, 0x3f, 0x44, 0x00, 0x3d, 0x4a, 0x55, 0x4b,
    0x4a, 0x41, 0x00, 0x53, 0x45, 0x50, 0x44, 0x00, 0x45, 0x50, 0x08, 0x3d, 0x4a, 0x55, 0x49, 0x4b,
    0x4e, 0x41, 0x57, 0x57, 0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg10[] = {   /* Marvin: Leon! You already made it! */
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x28, 0x41,
    0x4b, 0x4a, 0x1a, 0x00, 0x35, 0x4b, 0x51, 0x00, 0x3d, 0x48, 0x4e, 0x41, 0x3d, 0x40, 0x55, 0x00,
    0x49, 0x3d, 0x40, 0x41, 0x00, 0x45, 0x50, 0x1a, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg11[] = {   /* Leon: Hey Marvin, glad you made it! */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x24, 0x41, 0x55, 0x00,
    0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x18, 0x00, 0x43, 0x48, 0x3d, 0x40, 0x00, 0x55, 0x4b, 0x51,
    0x00, 0x49, 0x3d, 0x40, 0x41, 0x00, 0x45, 0x50, 0x1a, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg12[] = {   /* Leon: Allow me to introduce you. This is... */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x1d, 0x48, 0x48, 0x4b,
    0x53, 0x00, 0x49, 0x41, 0x00, 0x50, 0x4b, 0x00, 0x45, 0x4a, 0x50, 0x4e, 0x4b, 0x40, 0x51, 0x3f,
    0x41, 0x00, 0x55, 0x4b, 0x51, 0x57, 0x00, 0x30, 0x44, 0x45, 0x4f, 0x00, 0x45, 0x4f, 0x57, 0x57,
    0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg13[] = {   /* Ada: ... Ada, Ada Wong */
    0x04, 0x00, 0x05, 0x02, 0x1d, 0x40, 0x3d, 0x16, 0x05, 0x00, 0x00, 0x57, 0x57, 0x57, 0x00, 0x1d,
    0x40, 0x3d, 0x18, 0x00, 0x1d, 0x40, 0x3d, 0x00, 0x33, 0x4b, 0x4a, 0x43, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg14[] = {   /* Leon: Ada Wong. */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x1d, 0x40, 0x3d, 0x00,
    0x33, 0x4b, 0x4a, 0x43, 0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg15[] = {   /* Marvin: Hello, glad to meet another / Survivor! I'm Marvin. */
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x24, 0x41,
    0x48, 0x48, 0x4b, 0x18, 0x00, 0x43, 0x48, 0x3d, 0x40, 0x00, 0x50, 0x4b, 0x00, 0x49, 0x41, 0x41,
    0x50, 0x00, 0x3d, 0x4a, 0x4b, 0x50, 0x44, 0x41, 0x4e, 0x08, 0x2f, 0x51, 0x4e, 0x52, 0x45, 0x52,
    0x4b, 0x4e, 0x1a, 0x00, 0x25, 0x3a, 0x49, 0x00, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x57, 0x04,
    0x01, 0x01, 0x63,
};
static const uint8_t k_msg16[] = {   /* Leon: Anyway... looks like we can't / contact anyone with this thing / anymore. */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x1d, 0x4a, 0x55, 0x53,
    0x3d, 0x55, 0x57, 0x57, 0x57, 0x00, 0x48, 0x4b, 0x4b, 0x47, 0x4f, 0x00, 0x48, 0x45, 0x47, 0x41,
    0x00, 0x53, 0x41, 0x00, 0x3f, 0x3d, 0x4a, 0x3a, 0x50, 0x08, 0x3f, 0x4b, 0x4a, 0x50, 0x3d, 0x3f,
    0x50, 0x00, 0x3d, 0x4a, 0x55, 0x4b, 0x4a, 0x41, 0x00, 0x53, 0x45, 0x50, 0x44, 0x00, 0x50, 0x44,
    0x45, 0x4f, 0x00, 0x50, 0x44, 0x45, 0x4a, 0x43, 0x08, 0x3d, 0x4a, 0x55, 0x49, 0x4b, 0x4e, 0x41,
    0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg17[] = {   /* Marvin: Ohh... what do we do then?... */
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x2b, 0x44,
    0x44, 0x57, 0x57, 0x57, 0x00, 0x53, 0x44, 0x3d, 0x50, 0x00, 0x40, 0x4b, 0x00, 0x53, 0x41, 0x00,
    0x40, 0x4b, 0x00, 0x50, 0x44, 0x41, 0x4a, 0x1b, 0x57, 0x57, 0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg18[] = {   /* Leon: ... */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x57, 0x57, 0x57, 0x04,
    0x01, 0x01, 0x63,
};
static const uint8_t k_msg19[] = {   /* Leon: I know! The patrol car! We can / use it to get out of here! */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x25, 0x00, 0x47, 0x4a,
    0x4b, 0x53, 0x1a, 0x00, 0x30, 0x44, 0x41, 0x00, 0x4c, 0x3d, 0x50, 0x4e, 0x4b, 0x48, 0x00, 0x3f,
    0x3d, 0x4e, 0x1a, 0x00, 0x33, 0x41, 0x00, 0x3f, 0x3d, 0x4a, 0x08, 0x51, 0x4f, 0x41, 0x00, 0x45,
    0x50, 0x00, 0x50, 0x4b, 0x00, 0x43, 0x41, 0x50, 0x00, 0x4b, 0x51, 0x50, 0x00, 0x4b, 0x42, 0x00,
    0x44, 0x41, 0x4e, 0x41, 0x1a, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg20[] = {   /* Marvin: Yeah, you're right! That could / be our way out! */
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x35, 0x41,
    0x3d, 0x44, 0x18, 0x00, 0x55, 0x4b, 0x51, 0x3a, 0x4e, 0x41, 0x00, 0x4e, 0x45, 0x43, 0x44, 0x50,
    0x1a, 0x00, 0x30, 0x44, 0x3d, 0x50, 0x00, 0x3f, 0x4b, 0x51, 0x48, 0x40, 0x08, 0x3e, 0x41, 0x00,
    0x4b, 0x51, 0x4e, 0x00, 0x53, 0x3d, 0x55, 0x00, 0x4b, 0x51, 0x50, 0x1a, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg21[] = {   /* Leon: Okay, Marvin, you go with Ada to / the parking lot and wait there. */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x2b, 0x47, 0x3d, 0x55,
    0x18, 0x00, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x18, 0x00, 0x55, 0x4b, 0x51, 0x00, 0x43, 0x4b,
    0x00, 0x53, 0x45, 0x50, 0x44, 0x00, 0x1d, 0x40, 0x3d, 0x00, 0x50, 0x4b, 0x08, 0x50, 0x44, 0x41,
    0x00, 0x4c, 0x3d, 0x4e, 0x47, 0x45, 0x4a, 0x43, 0x00, 0x48, 0x4b, 0x50, 0x00, 0x3d, 0x4a, 0x40,
    0x00, 0x53, 0x3d, 0x45, 0x50, 0x00, 0x50, 0x44, 0x41, 0x4e, 0x41, 0x57, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg22[] = {   /* Leon: I'm going to get Chief Irons, / and I'll be right behind you! */
    0x04, 0x00, 0x05, 0x01, 0x28, 0x41, 0x4b, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x25, 0x3a, 0x49, 0x00,
    0x43, 0x4b, 0x45, 0x4a, 0x43, 0x00, 0x50, 0x4b, 0x00, 0x43, 0x41, 0x50, 0x00, 0x1f, 0x44, 0x45,
    0x41, 0x42, 0x00, 0x25, 0x4e, 0x4b, 0x4a, 0x4f, 0x18, 0x08, 0x3d, 0x4a, 0x40, 0x00, 0x25, 0x3a,
    0x48, 0x48, 0x00, 0x3e, 0x41, 0x00, 0x4e, 0x45, 0x43, 0x44, 0x50, 0x00, 0x3e, 0x41, 0x44, 0x45,
    0x4a, 0x40, 0x00, 0x55, 0x4b, 0x51, 0x1a, 0x04, 0x01, 0x01, 0x63,
};
static const uint8_t k_msg23[] = {   /* Marvin: Alright! Sounds like a plan. / Take care Leon! */
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x1d, 0x48,
    0x4e, 0x45, 0x43, 0x44, 0x50, 0x1a, 0x00, 0x2f, 0x4b, 0x51, 0x4a, 0x40, 0x4f, 0x00, 0x48, 0x45,
    0x47, 0x41, 0x00, 0x3d, 0x00, 0x4c, 0x48, 0x3d, 0x4a, 0x57, 0x08, 0x30, 0x3d, 0x47, 0x41, 0x00,
    0x3f, 0x3d, 0x4e, 0x41, 0x00, 0x28, 0x41, 0x4b, 0x4a, 0x1a, 0x04, 0x01, 0x01, 0x63,
};

static const struct { uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    {  6, k_msg06, (uint8_t)sizeof k_msg06 }, {  7, k_msg07, (uint8_t)sizeof k_msg07 },
    {  8, k_msg08, (uint8_t)sizeof k_msg08 }, {  9, k_msg09, (uint8_t)sizeof k_msg09 },
    { 10, k_msg10, (uint8_t)sizeof k_msg10 }, { 11, k_msg11, (uint8_t)sizeof k_msg11 },
    { 12, k_msg12, (uint8_t)sizeof k_msg12 }, { 13, k_msg13, (uint8_t)sizeof k_msg13 },
    { 14, k_msg14, (uint8_t)sizeof k_msg14 }, { 15, k_msg15, (uint8_t)sizeof k_msg15 },
    { 16, k_msg16, (uint8_t)sizeof k_msg16 }, { 17, k_msg17, (uint8_t)sizeof k_msg17 },
    { 18, k_msg18, (uint8_t)sizeof k_msg18 }, { 19, k_msg19, (uint8_t)sizeof k_msg19 },
    { 20, k_msg20, (uint8_t)sizeof k_msg20 }, { 21, k_msg21, (uint8_t)sizeof k_msg21 },
    { 22, k_msg22, (uint8_t)sizeof k_msg22 }, { 23, k_msg23, (uint8_t)sizeof k_msg23 },
};
#define CUT10F0_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

static uint8_t s_zustand = RE15_CUT10F0_AUS;

/* ---- Pruefhaken ---------------------------------------------------------------------------------- */
int re15_cut10f0_zustand(void) { return s_zustand; }

const uint8_t *re15_cut10f0_programm(int *out_len)
{
    if (out_len) *out_len = RE15_CUT10F0_PROG_LEN;
    return k_cut10f0_prog;
}

const uint8_t *re15_cut10f0_meldung(int msg_id, int *out_len)
{
    for (int i = 0; i < CUT10F0_N_MELDUNGEN; i++)
        if ((int)k_meldungen[i].id == msg_id) {
            if (out_len) *out_len = (int)k_meldungen[i].n;
            return k_meldungen[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

int re15_cut10f0_msg_offset(int msg_id)
{
    for (unsigned i = 0; i < sizeof k_cut10f0_msg_marken / sizeof k_cut10f0_msg_marken[0]; i++)
        if ((int)k_cut10f0_msg_marken[i].id == msg_id) return (int)k_cut10f0_msg_marken[i].off;
    return -1;
}

const uint8_t *re15_cut10f0_tuerton(int *out_len)
{
    if (out_len) *out_len = RE15_CUT10F0_TUERTON_LEN;
    return k_cut10f0_tuerton;
}

/* ---- Gemeinsame Helfer --------------------------------------------------------------------------- */
static int gesehen(void)
{
    return re15_game_flag_get(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT) != 0;
}

/* Laeuft das Programm in einem Faden? (Muster adaruf_1050.c programm_laeuft: genau EIN Faden.) */
static int programm_laeuft(void)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= k_cut10f0_prog && t->pc < k_cut10f0_prog + RE15_CUT10F0_PROG_LEN)
            return 1;
    }
    return 0;
}

static void meldungen_einsetzen(void)
{
    for (int i = 0; i < CUT10F0_N_MELDUNGEN; i++) {
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

/* ---- Installation beim Raumaufbau ---------------------------------------------------------------- */
void re15_cut10f0_install(uint16_t room_id)
{
    s_zustand = RE15_CUT10F0_AUS;
    /* Besucht-Latch des zweiten Kartenziels: Leon betritt ROOM1150 NACH der Szene -> (9,72)=1, die
     * Kachel hoert auf zu blinken (map_hint_common.c Eintrag K2). Das Besucht-Bit der Zone taugt dafuer
     * nicht — ROOM1150 ist vor der Szene laengst besucht (re15_cut10f0.h, Dossier §8.1). */
    if (room_id == RE15_CUT10F0_ZIEL2_RAUM && gesehen() &&
        !re15_game_flag_get(RE15_CUT10F0_ZIEL2_BESUCHT_BANK, RE15_CUT10F0_ZIEL2_BESUCHT_BIT)) {
        re15_game_flag_set(RE15_CUT10F0_ZIEL2_BESUCHT_BANK, RE15_CUT10F0_ZIEL2_BESUCHT_BIT, 1);
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[cut10f0] ROOM%04X nach der Szene betreten: (%d,%d)=1, Kartenziel ROOM1150 erreicht\n",
                (unsigned)room_id, RE15_CUT10F0_ZIEL2_BESUCHT_BANK, RE15_CUT10F0_ZIEL2_BESUCHT_BIT);
#endif
    }
    if (room_id != RE15_CUT10F0_RAUM) return;
    if (gesehen()) return;

    meldungen_einsetzen();
    /* RE2-Tuerbank: beim echten Eintritt durch die Tuer liegt der DOOR13-Tonteil schon in der Bank
     * (die Tuersequenz hat ihn geladen; gleiche Bytes -> kein Neuladen, audio_pc.c memcmp); beim
     * Debug-Sprung/CONTINUE laedt ihn diese Zeile. */
    re15_audio_re2_tuer_laden(k_cut10f0_tuerton, RE15_CUT10F0_TUERTON_LEN);
    int slot = scd_event_fire(RE15_CUT10F0_EREIGNIS);
    s_zustand = (slot >= 0) ? RE15_CUT10F0_LAEUFT : RE15_CUT10F0_AUS;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[cut10f0] ROOM%04X: Szene %s (Ereignis %d, Faden %d, Flag (%d,%d)=0)\n",
            (unsigned)room_id, slot >= 0 ? "gestartet" : "NICHT gestartet (kein freier Faden)",
            RE15_CUT10F0_EREIGNIS, slot, RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT);
#endif
}

const uint8_t *re15_cut10f0_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_CUT10F0_RAUM || event_id != RE15_CUT10F0_EREIGNIS) return NULL;
    if (gesehen()) return NULL;
    if (programm_laeuft()) return NULL;
    return k_cut10f0_prog;
}

/* ---- MAIN01 vom Ende der 1150-Montage bis zum Parkplatz ------------------------------------------ */
/* Zwei fluechtige Zustaende (nicht im Spielstand; beide stellen sich nach dem Laden im ersten Spielbild
 * aus den Flags wieder her):
 *   s_bgm_offen  Das Fenster ist OFFEN. Es oeffnet erst, wenn die Flags stehen UND keine Szene laeuft
 *                (re15_cine_active = (1,27) || (2,7), die Rahmen-Flags jeder Original-Szene) — setzt
 *                Spur L (9,73) am ANFANG ihrer Montage, beginnt MAIN01 trotzdem erst an deren Ende.
 *                Einmal offen bleibt es ueber spaetere Szenen hinweg offen ("durchweg").
 *   s_bgm_stand  Die letzte Auskunft an die Audio-Schicht (1 = 0xFF01 geliefert, 0 = Tabelle). Weicht
 *                sie vom Soll des laufenden Raums ab, stoesst der Tick die Raummusik an — das deckt das
 *                Montage-Ende, das Laden eines Spielstands (der Boot-BGM-Aufruf laeuft vor dem Restore,
 *                Block-memcpy @0x8002629c) und das Schliessen ausserhalb eines Raumwechsels. */
static uint8_t s_bgm_offen = 0;
static uint8_t s_bgm_stand = 0;

static int fenster_flags(void)
{
    return gesehen() &&
           re15_game_flag_get(RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT) != 0 &&
           re15_game_flag_get(RE15_CUT10F0_ZIEL1_ERREICHT_BANK, RE15_CUT10F0_ZIEL1_ERREICHT_BIT) == 0;
}

/* Rein: der Eintrag fuer (stage, room) beim jetzigen Zustand. */
static int weiche(int stage, int room)
{
    if (stage != 0) return -1;                          /* nur STAGE1 */
    if (room == RE15_CUT10F0_BGM_RAUM_ENDE) return -1;  /* im Parkplatz selbst dessen eigene Musik */
    if (!s_bgm_offen || !fenster_flags()) return -1;
    return RE15_CUT10F0_BGM_EINTRAG;
}

int re15_cut10f0_bgm_eintrag(int stage, int room)
{
    int e = weiche(stage, room);
    s_bgm_stand = (uint8_t)(e >= 0);                    /* die Audio-Schicht waehlt jetzt danach */
    return e;
}

int re15_cut10f0_bgm_haelt_main(void) { return s_bgm_stand; }

/* Haken audio_pc.c, SEQ_CTL-Zweig (Nachbesserung 1, Mangel 1; Rumpf hier statt in audio_pc.c, dort eine Zeile).
 * Hat die Weiche der Audio-Schicht MAIN01 geliefert, meint ein Skript-Befehl an Slot 0 die nicht geladene
 * TABELLEN-Musik des Raums (ROOM11D0 sub01 @0x01710 `54 00 02 00 00 00` gilt MAIN3B, Tabelle @0x80074828[0x1D]
 * = 0xFF7B). Angewandt stoppte er MAIN01 (FUN_80044da4 op 2 @0x80044e50 -> SsSeqStop), und weil jeder Raum des
 * Fensters denselben MAIN traegt, startete ihn nichts neu (FUN_80044210 Gleich-Zweig @0x80044280; SsSeqReplay
 * kehrt bei gestoppter Sequenz um, @0x8005ac94); die Nutzlast @0x80044f50/@0x80044f6c schriebe in die
 * MAIN01-Bank. Rueckgabe 1 = nicht anwenden. Slots 1/2 (SUB) bleiben unberuehrt. */
int re15_cut10f0_main_gesperrt(unsigned slot, int op, long cap_tick)
{
    if (slot != 0 || !s_bgm_stand) return 0;
#ifdef RE15_PLATFORM_PC
    if (getenv("RE15_BGM_CTL_DEBUG"))
        fprintf(stderr, "[bgm] Sce_bgm_control slot=0 op=%d im MAIN01-Fenster NICHT angewandt "
                        "(Runde 35 Spur K) capTick=%ld\n", op, cap_tick);
#else
    (void)op; (void)cap_tick;
#endif
    return 1;
}

/* Haken audio_pc.c, Se_on-Zweig (Nachbesserung 1; Rumpf hier, dort eine Zeile). Port-Bank 0x0E
 * (RE15_CUT10F0_SE_BANK) = die geladene RE2-Tuerbank: der Tuerknall der Szene in der Se_on-Form von ROOM10D0
 * sub21 @0x01A02. Die Original-Bank-Weiche FUN_80045024 kennt nur Bank 0..5, deshalb VOR ihr und vor deren
 * Debug-Zeile. Rueckgabe 1 = behandelt. */
int re15_cut10f0_se_on(unsigned bank, int sample_id)
{
    if (bank != RE15_CUT10F0_SE_BANK) return 0;
#ifdef RE15_PLATFORM_PC
    if (getenv("RE15_SE_DEBUG"))
        fprintf(stderr, "[se] Se_on bank=%u id=%d -> RE2-Tuerbank (Runde 35 Spur K)\n", bank, sample_id);
#endif
    re15_audio_re2_tuer_se(sample_id);
    return 1;
}

int re15_cut10f0_bgm_fenster(void)    { return s_bgm_offen; }

static void bgm_fenster_tick(void)
{
    const uint8_t war_offen = s_bgm_offen;
    if (!fenster_flags())           s_bgm_offen = 0;
    else if (!re15_cine_active())   s_bgm_offen = 1;    /* oeffnet erst nach dem Ende der Szene */
#ifdef RE15_PLATFORM_PC
    if (war_offen != s_bgm_offen)
        fprintf(stderr, "[cut10f0] MAIN01-Fenster %s in ROOM%04X: (%d,%d)=%d (%d,%d)=%d, Parkplatz erreicht (%d,%d)=%d\n",
                s_bgm_offen ? "auf" : "zu", (unsigned)g_current_room_id,
                RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, gesehen(),
                RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT,
                re15_game_flag_get(RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT),
                RE15_CUT10F0_ZIEL1_ERREICHT_BANK, RE15_CUT10F0_ZIEL1_ERREICHT_BIT,
                re15_game_flag_get(RE15_CUT10F0_ZIEL1_ERREICHT_BANK, RE15_CUT10F0_ZIEL1_ERREICHT_BIT));
#else
    (void)war_offen;
#endif
    const int stage = (int)((g_current_room_id >> 12) & 0xf) - 1;
    const int room  = (int)((g_current_room_id >> 4) & 0xff);
    const int soll  = weiche(stage, room) >= 0;
    if (soll == (int)s_bgm_stand) return;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[cut10f0] Raummusik-Anstoss in ROOM%04X: Soll %s (letzte Auskunft an die Audio-Schicht: %s)\n",
            (unsigned)g_current_room_id, soll ? "MAIN01" : "Tabelle", s_bgm_stand ? "MAIN01" : "Tabelle");
#endif
    /* Beim OEFFNEN die Skript-Latches des Raums leeren (room_common.c tut das vor jedem Raumwechsel):
     * was das Raumskript der TABELLEN-Musik mitgegeben hat (Status, Programm-Lautstaerken — ROOM1090
     * sub00 @0x022EE `54 00 00 01 78 33`), gehoert nicht auf die MAIN01-Bank (FUN_80044da4 schreibt in
     * die GELADENE Bank, @0x80044f50/@0x80044f6c). */
    if (soll) re15_audio_bgm_status_reset();
    /* Dieselbe Weiche wie beim Raumwechsel (room_common.c (15)): die Tabelle liefert jetzt MAIN01 ->
     * MAIN wechselt -> Ausblendung + Load (audio_pc.c schreibt "[bgm] ... entry=FF01 -> MAIN01"). */
    re15_audio_start_room_bgm(stage, room);
    s_bgm_stand = (uint8_t)soll;                        /* auch ohne Audio-Geraet nur EIN Anstoss */
}

/* ---- Szenen-Ende: Karte auf; je Spielbild das MAIN01-Fenster ------------------------------------- */
void re15_cut10f0_tick(void)
{
    if (s_zustand == RE15_CUT10F0_LAEUFT && !programm_laeuft()) {
        s_zustand = RE15_CUT10F0_FERTIG;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[cut10f0] Szene zu Ende: Kartenhinweis ROOM%04X -> ROOM%04X angefordert\n",
                RE15_CUT10F0_ZIEL1_RAUM, RE15_CUT10F0_ZIEL2_RAUM);
#endif
        /* Kartenhinweis wie nach ROOM1150 sub08 (RE2 Opcode 0x84 @0x800591C4 vor dem Evt_end) — hier
         * portseitig angefordert, weil das Programm nicht im RDT-Puffer liegt (kein Anker-Scan). */
        re15_map_hint_request(re15_map_hint_eintrag_fuer(RE15_CUT10F0_RAUM, RE15_CUT10F0_ZIEL1_RAUM));
    }
    bgm_fenster_tick();
}

/* ---- Statusschirm: Hinweiskette und zweites Kartenziel (Haken menu_common.c) ---------------------- */
int re15_cut10f0_hinweis_kette(int *hint_nr, int weiter)
{
    if (!hint_nr || !re15_map_hint_zeitgesteuert(*hint_nr)) return 0;   /* Runde-33-Hinweis: wie bisher */
    const int zeit_um = re15_map_hint_schritte() >=
                        (uint64_t)(RE15_CUT10F0_HINWEIS_PERIODEN * re15_map_hint_periode());
    const int folge = re15_map_hint_folge(*hint_nr);
    if (folge >= 0 && (weiter || zeit_um)) {
        int page = 0, rect = 0;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[hint] F%u Folge-Hinweis %d -> %d (%s)\n",
                (unsigned)g_engine.frame_count, *hint_nr, folge, weiter ? "START" : "Zeit");
#endif
        *hint_nr = folge;
        /* Im offenen Schirm auf das Folge-Ziel umschalten: Blatt + Rechteck neu, Zaehler neu wie beim
         * Oeffnen (menu_common.c hint_open); die Panelfahrt steht schon in der Endlage. */
        if (re15_map_hint_ziel(folge, &page, &rect)) {
            g_inv_screen.map_page  = (uint8_t)page;
            g_inv_screen.hint_page = (uint8_t)page;
            g_inv_screen.hint_rect = (uint8_t)rect;
            re15_map_hint_begin();
            g_inv_screen.hint_rot  = (uint8_t)re15_map_hint_rot();
        }
        return 1;
    }
    if (!zeit_um) return 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[hint] F%u Zeit um (Hinweis %d, %d Blinkperioden)\n",
            (unsigned)g_engine.frame_count, *hint_nr, RE15_CUT10F0_HINWEIS_PERIODEN);
#endif
    return 2;
}

void re15_cut10f0_ziel2_setzen(int karte_mit_ziel)
{
    int page = 0, rect = 0;
    g_inv_screen.ziel2_aktiv = karte_mit_ziel ? (uint8_t)re15_map_ziel_aktiv_n(1, &page, &rect) : 0;
    g_inv_screen.ziel2_page  = (uint8_t)page;
    g_inv_screen.ziel2_rect  = (uint8_t)rect;
}

/* ---- Gestenbank-Leihe und NPC-Record-Alias ------------------------------------------------------- */
unsigned re15_cut10f0_rbj_quelle(unsigned room_id)
{
    if (room_id != RE15_CUT10F0_RAUM || gesehen()) return 0;
    return RE15_CUT10F0_RBJ_RAUM;
}

int re15_cut10f0_rbj_record_alias(int slot)
{
    if (g_current_room_id != RE15_CUT10F0_RAUM) return -1;
    if (slot == RE15_CUT10F0_AKTOR_MARVIN) return RE15_CUT10F0_RBJ_NPC_RECORD;
    return -1;
}
