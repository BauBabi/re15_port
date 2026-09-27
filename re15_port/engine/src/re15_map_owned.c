/*=========================================================================
 * KARTEN-BESITZ — die Fundstellen und das Besitz-Gatter.
 * Mechanismus + Begruendung: re15_port/include/re15_map_owned.h
 *=======================================================================*/
#include "re15_map_owned.h"
#include "re15_scd.h"        /* re15_game_flag_get */

/*-------------------------------------------------------------------------
 * DIE FUNDSTELLEN — GESUCHT, NICHT GESETZT.
 *
 * Auftrag war ausdruecklich: erst SUCHEN, nichts erfinden, und die Karte
 * NICHT beim Betreten vergeben. Gesucht wurde in ALLEN 240 RDTs des
 * Asset-Baums (re15_port/shared_assets/PSX/ ** / *.RDT), die Raumtexte
 * glyphen-dekodiert mit der Tabelle des Ports (engine/src/msg_common.c:175-199)
 * nach map/plan/layout/chart/blueprint/…:
 *
 *   ROOM1030/1031 id15 @0x2C16  "A map of dowtown Raccoon City. …"
 *   ROOM30A0/30A1 id2  @0x0930  "A detailed map of product distribution …"
 *   ROOM4020/4021 id0  @0x0B84  "There's a map of the entire area."
 *   ROOM5030/5031 id0  @0x10E6  "A map of the lab is available here.
 *                                Will you file the laboratory map?"
 *                 id1  @0x1132  "You've taken the laboratory map."
 *
 * Von diesen VIER Wandplaenen ist genau EINER ein Erwerb; die anderen drei
 * sind reine Untersuchen-AOTs ohne jede Wirkung (ROOM4020 sub11 @0x00B06:
 * Cut_chg / Message_on 0 / Cut_old / Cut_auto — kein `Set`; ROOM30A0 sub02
 * @0x00876 ebenso). Sie bleiben deshalb ungenutzt: ihnen ein Bit zu geben
 * waere eine erfundene Fundstelle.
 *
 * ⛔ DIE EINE ECHTE FUNDSTELLE — ROOM5030/5031, byte-belegt im RDT:
 *   ROOM5030.RDT sub00 @0x1086  21 03 73 00   Ck (bank 3, bit 115, soll 0)
 *                               -> der Untersuchen-AOT (Aot_set slot 5, sce 3,
 *                                  @0x108A) existiert NUR, solange der Plan
 *                                  noch nicht genommen wurde
 *   ROOM5030.RDT sub02 @0x10C8  22 03 73 01   Set(bank 3, bit 115, 1)
 *                               -> direkt zwischen Aot_reset slot 5 (@0x10BA)
 *                                  und Message_on 1 (@0x10CC, "You've taken
 *                                  the laboratory map.")
 *   ROOM5031.RDT sub00 @0x108C / sub02 @0x1126 — dieselbe Folge in der
 *                               zweiten Figuren-Variante des Raums.
 * Zensus ueber alle 206 walkbaren RDTs: Flag(3,115) kommt an GENAU diesen
 * vier Stellen vor und nirgends sonst — es ist ausschliesslich das
 * "Laborplan genommen"-Flag. Es liegt damit schon heute im Spielstand
 * (re15_savedata.c `memcpy(out->flags, g_game.flags, …)`).
 *
 * ⛔ WELCHE BLAETTER der Laborplan freischaltet, ist eine PORT-SETZUNG und
 * KEINE Messung — RE1.5 fuehrt dafuer keine Tabelle (sein Zeichner liest kein
 * Flag, s. Header). Gesetzt sind die drei Blaetter, auf denen ueberhaupt
 * Labor-Raeume (0x50xx) gezeichnet sind: Seite 9 (ROOM5000..50B0, darunter
 * die Fundstelle ROOM5030 selbst), Seite 10 (ROOM50C0..50E0) und Seite 11
 * (ROOM50D0..5100). Dass EIN Fund MEHRERE Bereiche freischaltet, ist das
 * RE2-Muster (ROOM3040 setzt dort die Bits 2..8 in einem Zug).
 *
 * Fuer alle uebrigen Blaetter gibt es in RE1.5 KEINE Fundstelle. Sie bleiben
 * damit dauerhaft ohne Besitz und verhalten sich exakt wie bisher
 * (= RE2s Zweig OHNE Karte, `beq v0,zero,0x8006e768` @0x8006E744).
 *-----------------------------------------------------------------------*/
typedef struct {
    unsigned short room;   /* Raum der Fundstelle (Basis-Variante)          */
    unsigned char  bank;   /* SCD-Flag-Bank des `Set`                        */
    unsigned char  bit;    /* Bit-Nummer                                     */
    unsigned char  page;   /* freigeschaltetes Kartenblatt                   */
} re15_map_fund_t;

static const re15_map_fund_t s_map_funde[] = {
    /* Laborplan, ROOM5030 sub02 @0x10C8 `22 03 73 01` */
    { 0x5030, 3, 115,  9 },
    { 0x5030, 3, 115, 10 },
    { 0x5030, 3, 115, 11 },
};

#define FUND_COUNT ((int)(sizeof s_map_funde / sizeof s_map_funde[0]))

int re15_map_fund_count(void) { return FUND_COUNT; }

int re15_map_fund_get(int i, unsigned *room, int *bank, int *bit, unsigned *page)
{
    if (i < 0 || i >= FUND_COUNT) return 0;
    if (room) *room = s_map_funde[i].room;
    if (bank) *bank = s_map_funde[i].bank;
    if (bit)  *bit  = s_map_funde[i].bit;
    if (page) *page = s_map_funde[i].page;
    return 1;
}

int re15_map_owned_page(unsigned page)
{
    int i;
    for (i = 0; i < FUND_COUNT; i++)
        if (s_map_funde[i].page == page &&
            re15_game_flag_get(s_map_funde[i].bank, s_map_funde[i].bit))
            return 1;
    return 0;
}

uint32_t re15_map_owned_bits(void)
{
    uint32_t m = 0;
    int i;
    for (i = 0; i < FUND_COUNT; i++)
        if (s_map_funde[i].page < 32 &&
            re15_game_flag_get(s_map_funde[i].bank, s_map_funde[i].bit))
            m |= (uint32_t)1u << s_map_funde[i].page;
    return m;
}
