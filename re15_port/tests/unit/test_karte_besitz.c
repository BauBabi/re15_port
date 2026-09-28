/* RE2-KARTENSYSTEM IM PORT — die Riegel zum Karten-BESITZ.
 *
 * Was hier festgenagelt wird (alles RE2-belegt, Adressen an den Pruefungen):
 *   1. Ohne den Plan wird eine UNBESUCHTE Kachel gar nicht gezeichnet
 *      (RE2 `beq v0,zero,0x8006e768` @0x8006E744).
 *   2. Mit dem Plan WIRD sie gezeichnet, und zwar in der Zustandszeile
 *      CLUT-Y 498 (RE2 `addiu s5,zero,498` @0x8006E71C).
 *   3. Die drei Zustandszeilen tragen genau die aus RE2s ST0.TIM gelesenen
 *      Eintraege (Datei-Offsets 0x10936 / 0x10996 / 0x109B6).
 *   4. Die Fundstellen-Tabelle nennt nur belegte Stellen und vergibt nichts
 *      beim blossen Betreten.
 *   5. Der Besitz haengt am Spiel-Flag und damit am vorhandenen Spielstand —
 *      ein Stand ohne neue Felder laedt weiter und verhaelt sich wie heute.
 */
#include <stdio.h>
#include <string.h>
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_inv_screen.h"
#include "re15_map_owned.h"

extern unsigned g_current_room_id;
extern void re15_map_zone_update(unsigned room, int x, int z);
extern void re15_inv_map_stage_init(int stage, int room_slot);
extern void re15_inv_screen_open(void);
extern int  re15_map_rect_count(unsigned page);
extern int  re15_map_rect_state(unsigned page, unsigned rect_idx);
extern void re15_map_visited_reset(void);

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Wie viele MAP4-Kachel-Ops traegt die Op-Liste des Kartenschirms, und wie viele
 * davon in der Unbesucht-Zeile? */
/* Die BEIDEN FESTEN Schirm-Sprites liegen ebenfalls auf MAP4: Titelbild
 * (30,30) 88x32 uv(0,0) @0x80047204-268 und Kompass (270,40) 32x48 uv(96,0)
 * @0x8004726c-2c0. Sie sind KEINE Raumkacheln und tragen weiter die
 * RE1.5-Palettenzeile. */
static int ist_fest(const re15_inv_op_t *o)
{
    return (o->x == 0x1e  && o->y == 0x1e && o->w == 0x58 && o->h == 0x20) ||
           (o->x == 0x10e && o->y == 0x28 && o->w == 0x20 && o->h == 0x30);
}

static void kacheln_zaehlen(unsigned room, int x, int z, unsigned page,
                            int *gesamt, int *unbesucht)
{
    static re15_inv_op_t ops[1024];
    int nops, i;
    *gesamt = 0; *unbesucht = 0;
    g_current_room_id = room;
    re15_map_zone_update(room, x, z);
    re15_inv_screen_open();
    g_inv_screen.substate = 1; g_inv_screen.item_state = 1;   /* MAP-Schirm */
    g_inv_screen.map_page = (unsigned char)page;
    nops = re15_inv_screen_build(&g_inv_screen, ops, 1024);
    for (i = 0; i < nops; i++) {
        if (ops[i].kind != RE15_INV_OP_SPRT) continue;
        if (ops[i].page != RE15_INV_PAGE_MAP4) continue;
        if (ist_fest(&ops[i])) continue;
        (*gesamt)++;
        if (ops[i].clut == RE15_INV_CLUT_MAP_UNBESUCHT) (*unbesucht)++;
    }
}

int main(void)
{
    printf("=== RE2-Kartensystem: Besitz-Gatter + Zustandszeilen ===\n");

    /* ---------------------------------------------------------------- (4)
     * FUNDSTELLEN — nur Belegtes, und nichts, was beim Betreten vergeben wird. */
    {
        int n = re15_map_fund_count(), i, nur_5030 = 1, seiten = 0;
        printf("  [Bestand] %d Fundstellen-Zeilen\n", n);
        CHECK("es GIBT eine Fundstelle (RE1.5 ROOM5030 setzt Flag(3,115))", n > 0);
        for (i = 0; i < n; i++) {
            unsigned room = 0, page = 99; int bank = -1, bit = -1;
            CHECK("Fundstellen-Zeile lesbar",
                  re15_map_fund_get(i, &room, &bank, &bit, &page));
            /* ⛔ JEDE Zeile muss eine im RDT belegte Stelle nennen. Heute ist das
             * ausschliesslich ROOM5030/5031 sub02 @0x10C8 `22 03 73 01` — ein Zensus
             * ueber alle 206 walkbaren RDTs findet Flag(3,115) nirgends sonst.
             * Kommt je eine Zeile dazu, die NICHT aus einem `Set` im RDT stammt,
             * faellt dieser Riegel und zwingt zum Beleg. */
            if (!((room & ~1u) == 0x5030 && bank == 3 && bit == 115)) nur_5030 = 0;
            if (page <= 31) seiten |= 1 << page;
        }
        CHECK("jede Fundstelle ist ROOM5030/Flag(3,115) — keine erfundene Stelle",
              nur_5030);
        printf("  [Seiten] Laborplan oeffnet Blattmaske 0x%04X\n", seiten);
        CHECK("der Laborplan oeffnet die drei Labor-Blaetter 9/10/11",
              seiten == ((1 << 9) | (1 << 10) | (1 << 11)));
    }

    /* ---------------------------------------------------------------- (5)
     * BESITZ HAENGT AM SPIEL-FLAG — also am schon vorhandenen Spielstand-Feld
     * (re15_savedata.c: memcpy(out->flags, g_game.flags, ...)). Ein Stand OHNE
     * neue Felder traegt das Flag nicht und verhaelt sich damit wie heute. */
    re15_game_state_init();
    CHECK("frischer Stand: KEIN Blatt im Besitz (= Verhalten wie bisher)",
          re15_map_owned_bits() == 0);
    CHECK("frischer Stand: Blatt 9 nicht im Besitz", !re15_map_owned_page(9));

    re15_game_flag_set(3, 115, 1);      /* RE1.5 ROOM5030 sub02 @0x10C8 */
    CHECK("nach dem Laborplan: Blatt 9 im Besitz",  re15_map_owned_page(9));
    CHECK("nach dem Laborplan: Blatt 10 im Besitz", re15_map_owned_page(10));
    CHECK("nach dem Laborplan: Blatt 11 im Besitz", re15_map_owned_page(11));
    CHECK("der Laborplan oeffnet KEIN fremdes Blatt (Polizeirevier 2)",
          !re15_map_owned_page(2));
    re15_game_flag_set(3, 115, 0);
    CHECK("Flag zurueck -> Besitz weg (kein eigener, driftender Zustand)",
          re15_map_owned_bits() == 0);

    /* ---------------------------------------------------------------- (1)+(2)
     * DAS GATTER AM ZEICHNER. Gefahren wird der Fundraum ROOM5030 selbst: sein
     * Blatt 9 traegt 16 Raeume, von denen genau einer besucht ist. */
    {
        int g_ohne = 0, u_ohne = 0, g_mit = 0, u_mit = 0;
        re15_game_state_init();
        re15_map_visited_reset();
        kacheln_zaehlen(0x5030, -29828, -404, 9, &g_ohne, &u_ohne);
        printf("  [ohne Plan] %d Kachel-Ops, davon %d in der Unbesucht-Zeile\n",
               g_ohne, u_ohne);
        /* RE2 @0x8006E744: ohne Karte wird jeder Raum ohne Besucht-Bit uebersprungen. */
        CHECK("ohne Plan: KEINE unbesuchte Kachel wird gezeichnet", u_ohne == 0);
        CHECK("ohne Plan: der eigene Raum wird trotzdem gezeichnet", g_ohne > 0);

        re15_game_flag_set(3, 115, 1);
        kacheln_zaehlen(0x5030, -29828, -404, 9, &g_mit, &u_mit);
        printf("  [mit Plan]  %d Kachel-Ops, davon %d in der Unbesucht-Zeile\n",
               g_mit, u_mit);
        /* RE2 @0x8006E71C: mit Karte kommt die Zeile 498 dazu. */
        CHECK("mit Plan: unbesuchte Kacheln WERDEN gezeichnet", u_mit > 0);
        CHECK("mit Plan: es sind MEHR Kacheln als ohne", g_mit > g_ohne);
        CHECK("der Zuwachs sind genau die unbesuchten", g_mit - g_ohne == u_mit);

        /* Ein Blatt OHNE Fundstelle darf sich durch den Laborplan nicht aendern. */
        {
            int g_fremd_a = 0, u_fremd_a = 0, g_fremd_b = 0, u_fremd_b = 0;
            re15_game_flag_set(3, 115, 0);
            kacheln_zaehlen(0x5030, -29828, -404, 2, &g_fremd_a, &u_fremd_a);
            re15_game_flag_set(3, 115, 1);
            kacheln_zaehlen(0x5030, -29828, -404, 2, &g_fremd_b, &u_fremd_b);
            printf("  [Blatt 2] ohne %d/%d, mit %d/%d\n",
                   g_fremd_a, u_fremd_a, g_fremd_b, u_fremd_b);
            CHECK("Blatt 2 (keine Fundstelle) bleibt unveraendert",
                  g_fremd_a == g_fremd_b && u_fremd_a == 0 && u_fremd_b == 0);
        }
    }

    /* ---------------------------------------------------------------- (3)
     * DIE DREI ZUSTANDSZEILEN. Der Zeichner darf je Zustand nur die passende
     * CLUT einreihen — das ist RE2s Mechanismus (GetClut(256,s5) @0x8006E750). */
    {
        static re15_inv_op_t ops[1024];
        int nops, i, n_bes = 0, n_akt = 0, n_unb = 0, n_alt = 0;
        re15_game_state_init();
        re15_map_visited_reset();
        re15_game_flag_set(3, 115, 1);
        g_current_room_id = 0x5030;
        re15_map_zone_update(0x5030, -29828, -404);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
        g_inv_screen.map_page = 9;
        nops = re15_inv_screen_build(&g_inv_screen, ops, 1024);
        for (i = 0; i < nops; i++) {
            if (ops[i].kind != RE15_INV_OP_SPRT) continue;
            if (ops[i].page != RE15_INV_PAGE_MAP4) continue;
            if (ist_fest(&ops[i])) continue;      /* Titelbild + Kompass */
            if (ops[i].clut == RE15_INV_CLUT_MAP_BESUCHT)        n_bes++;
            else if (ops[i].clut == RE15_INV_CLUT_MAP_AKTUELL)   n_akt++;
            else if (ops[i].clut == RE15_INV_CLUT_MAP_UNBESUCHT) n_unb++;
            else                                                 n_alt++;
            /* ⛔ NEUTRALER TINT. RE2 moduliert nicht, es tauscht die CLUT-Zeile.
             * mod5(t5,128) == t5 — jeder andere Wert waere wieder eine Modulation. */
            if (ops[i].clut == RE15_INV_CLUT_MAP_BESUCHT ||
                ops[i].clut == RE15_INV_CLUT_MAP_AKTUELL ||
                ops[i].clut == RE15_INV_CLUT_MAP_UNBESUCHT) {
                if (ops[i].r != 128 || ops[i].g != 128 || ops[i].b != 128) n_alt += 1000;
            }
        }
        printf("  [Zeilen] besucht %d, aktuell %d, unbesucht %d, fremd %d\n",
               n_bes, n_akt, n_unb, n_alt);
        CHECK("der aktuelle Raum traegt die Aktuell-Zeile (RE2 502, @0x8006E648)",
              n_akt > 0);
        CHECK("unbesuchte Raeume tragen die Unbesucht-Zeile (RE2 498, @0x8006E71C)",
              n_unb > 0);
        CHECK("keine Kachel benutzt eine fremde CLUT oder einen Tint != 128",
              n_alt == 0);
    }

    /* ---------------------------------------------------------------- (3b)
     * DIE PALETTENWERTE SELBST - GEGEN RE2s ORIGINAL-TIM NACHGELESEN, nicht
     * gegen sich selbst. Der Riegel oeffnet info/re2leon/COMMON/DATA/ST0.TIM
     * und vergleicht die drei Halbwoerter an ihren Datei-Offsets mit den
     * Konstanten aus re15_inv_screen.h. Fehlt die Datei (Baum ohne RE2-Dump),
     * wird die Pruefung als UEBERSPRUNGEN gemeldet statt falsch-gruen zu sein.
     * NOETIG, weil meine erste von Hand gerechnete Umrechnung RGB888 -> RGB555
     * bei BEIDEN Farben falsch war (0xD802 statt 0xD902, 0x8061 statt 0x842D). */
    {
        const char *pf = RE15_RE2_ST0_TIM;
        FILE *f = fopen(pf, "rb");
        if (!f) {
            printf("  UEBERSPRUNGEN: %s nicht lesbar\n", pf);
        } else {
            struct { long off; unsigned short soll; const char *name; } q[2] = {
                { RE15_KARTE_ST0_OFF_AKTUELL,   RE15_KARTE_AKTUELL,   "aktuell   (CLUT-Y 502)" },
                { RE15_KARTE_ST0_OFF_UNBESUCHT, RE15_KARTE_UNBESUCHT, "unbesucht (CLUT-Y 498)" },
            };
            int k;
            for (k = 0; k < 2; k++) {
                unsigned char bb[2] = { 0, 0 };
                unsigned short ist;
                char txt[160];
                if (fseek(f, q[k].off, SEEK_SET) != 0 || fread(bb, 1, 2, f) != 2) {
                    printf("  FAIL: %s - Datei zu kurz\n", q[k].name);
                    g_fail = 1; continue;
                }
                ist = (unsigned short)(bb[0] | (bb[1] << 8));
                printf("  [ST0.TIM 0x%05lX] %s = 0x%04X (%d,%d,%d) stp%d\n",
                       q[k].off, q[k].name, ist,
                       (ist & 31) << 3, ((ist >> 5) & 31) << 3, ((ist >> 10) & 31) << 3,
                       (ist >> 15) & 1);
                snprintf(txt, sizeof txt, "%s steht byte-gleich in RE2s ST0.TIM", q[k].name);
                CHECK(txt, ist == q[k].soll);
            }
            fclose(f);
        }
        /* Und die DEKODIERUNG - genau die Stelle, an der ich mich verrechnet habe. */
        /* ⛔ BESUCHT kommt aus RE1.5, nicht aus RE2 (Berichtigung 2026-09-27, volle
         * Begruendung an RE15_KARTE_BESUCHT in include/re15_inv_screen.h). Der Riegel
         * liest deshalb RE1.5s EIGENE Palette nach — dieselbe Zeile 21, die auch der
         * RE1.5-Kartenzeichner waehlt (GetClut(0x100,0x1f5) @0x80046fdc-fe8). */
        {
            const char *pf15 = RE15_ASSET_PSX_DIR "/DATA/TEX.TIM";
            FILE *f15 = fopen(pf15, "rb");
            if (!f15) {
                printf("  UEBERSPRUNGEN: %s nicht lesbar\n", pf15);
            } else {
                unsigned char bb[2] = { 0, 0 };
                unsigned short ist = 0;
                if (fseek(f15, RE15_KARTE_TEX_OFF_BESUCHT, SEEK_SET) == 0 &&
                    fread(bb, 1, 2, f15) == 2) {
                    ist = (unsigned short)(bb[0] | (bb[1] << 8));
                    printf("  [TEX.TIM 0x%05X] besucht (RE1.5 Zeile 21) = 0x%04X (%d,%d,%d) stp%d\n",
                           (unsigned)RE15_KARTE_TEX_OFF_BESUCHT, ist,
                           (ist & 31) << 3, ((ist >> 5) & 31) << 3, ((ist >> 10) & 31) << 3,
                           (ist >> 15) & 1);
                    CHECK("besucht steht byte-gleich in RE1.5s TEX.TIM",
                          ist == RE15_KARTE_BESUCHT);
                } else {
                    printf("  FAIL: TEX.TIM zu kurz\n"); g_fail = 1;
                }
                fclose(f15);
            }
        }
        CHECK("0x81A4 dekodiert zu (32,104,0) halbtransparent",
              ((RE15_KARTE_BESUCHT & 31) << 3) == 32 &&
              (((RE15_KARTE_BESUCHT >> 5) & 31) << 3) == 104 &&
              (((RE15_KARTE_BESUCHT >> 10) & 31) << 3) == 0 &&
              (RE15_KARTE_BESUCHT >> 15) == 1);
        /* RE2s Wert bleibt als Alternative im Header — auch er wird nachgelesen, damit
         * ein Zurueckschalten nicht auf eine ungepruefte Zahl faellt. */
        CHECK("die RE2-Alternative 0xD902 dekodiert zu (16,64,176) halbtransparent",
              ((RE15_KARTE_BESUCHT_RE2 & 31) << 3) == 16 &&
              (((RE15_KARTE_BESUCHT_RE2 >> 5) & 31) << 3) == 64 &&
              (((RE15_KARTE_BESUCHT_RE2 >> 10) & 31) << 3) == 176 &&
              (RE15_KARTE_BESUCHT_RE2 >> 15) == 1);
        CHECK("0x842D dekodiert zu (104,8,8) halbtransparent",
              ((RE15_KARTE_AKTUELL & 31) << 3) == 104 &&
              (((RE15_KARTE_AKTUELL >> 5) & 31) << 3) == 8 &&
              (((RE15_KARTE_AKTUELL >> 10) & 31) << 3) == 8 &&
              (RE15_KARTE_AKTUELL >> 15) == 1);

        /* ⛔ RUNDE 30: DIE WANDLINIE (Eintrag 4 derselben Zeile 21) - und ob der
         * ZEICHNER die Werte ueberhaupt benutzt.
         * Dieser Riegel pruefte bisher nur die Defines gegen die Dateien. Dass
         * re2_ton in re15_inv_screen.c daneben eine FESTE Zahl fuehrte - RE2s Blau
         * (16,64,176) -, fiel ihm nicht auf: der Nutzer sah es am 2026-09-27 als
         * "ROOM 1000 ist blau" und "Roof die Wand unten blau". Die Op-Liste des
         * Zeichners prueft unit_r30_karte_nutzerstand (Schema-Fuellung und
         * Innenwand tragen die dekodierten Werte); hier steht der Dateibeleg. */
        {
            const char *pf15 = RE15_ASSET_PSX_DIR "/DATA/TEX.TIM";
            FILE *f15 = fopen(pf15, "rb");
            if (!f15) {
                printf("  UEBERSPRUNGEN: %s nicht lesbar\n", pf15);
            } else {
                unsigned char bb[2] = { 0, 0 };
                if (fseek(f15, RE15_KARTE_TEX_OFF_WAND, SEEK_SET) == 0 &&
                    fread(bb, 1, 2, f15) == 2) {
                    unsigned short ist = (unsigned short)(bb[0] | (bb[1] << 8));
                    printf("  [TEX.TIM 0x%05X] wand (RE1.5 Zeile 21 Eintrag 4) = 0x%04X "
                           "(%d,%d,%d) stp%d\n", (unsigned)RE15_KARTE_TEX_OFF_WAND, ist,
                           (ist & 31) << 3, ((ist >> 5) & 31) << 3, ((ist >> 10) & 31) << 3,
                           (ist >> 15) & 1);
                    CHECK("die Wandlinie steht byte-gleich in RE1.5s TEX.TIM",
                          ist == RE15_KARTE_WAND);
                } else {
                    printf("  FAIL: TEX.TIM zu kurz\n"); g_fail = 1;
                }
                fclose(f15);
            }
        }
        CHECK("Eintrag 4 liegt 3 Eintraege hinter Eintrag 1 (dieselbe CLUT-Zeile 21)",
              RE15_KARTE_TEX_OFF_WAND - RE15_KARTE_TEX_OFF_BESUCHT == 3 * 2);
        CHECK("0x5AD6 dekodiert zu (176,176,176) DECKEND (STP 0)",
              ((RE15_KARTE_WAND & 31) << 3) == 176 &&
              (((RE15_KARTE_WAND >> 5) & 31) << 3) == 176 &&
              (((RE15_KARTE_WAND >> 10) & 31) << 3) == 176 &&
              (RE15_KARTE_WAND >> 15) == 0);
        /* RE2: Eintrag 4 ist in allen drei Zustandszeilen bitgleich - die Wandlinie
         * traegt dort keinen Zustand. Eintrag 4 = Eintrag 1 + 6 Bytes. */
        {
            const char *pf = RE15_RE2_ST0_TIM;
            FILE *f = fopen(pf, "rb");
            if (!f) {
                printf("  UEBERSPRUNGEN: %s nicht lesbar\n", pf);
            } else {
                static const long zeile[3] = { RE15_KARTE_ST0_OFF_UNBESUCHT,
                                               RE15_KARTE_ST0_OFF_BESUCHT,
                                               RE15_KARTE_ST0_OFF_AKTUELL };
                unsigned short w4[3] = { 0, 1, 2 };
                int k, gelesen = 0;
                for (k = 0; k < 3; k++) {
                    unsigned char bb[2];
                    if (fseek(f, zeile[k] + 6, SEEK_SET) == 0 && fread(bb, 1, 2, f) == 2) {
                        w4[k] = (unsigned short)(bb[0] | (bb[1] << 8));
                        gelesen++;
                        printf("  [ST0.TIM 0x%05lX] Eintrag 4 = 0x%04X\n", zeile[k] + 6, w4[k]);
                    }
                }
                fclose(f);
                CHECK("RE2: Eintrag 4 (Wandlinie) ist in allen drei Zustandszeilen bitgleich",
                      gelesen == 3 && w4[0] == w4[1] && w4[1] == w4[2] && w4[0] != 0);
            }
        }
    }

    printf(g_fail ? "=== FAIL ===\n" : "=== OK ===\n");
    return g_fail;
}
