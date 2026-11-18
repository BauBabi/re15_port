/*
 * test_r42_namen.c — Runde 42 Strang "namen", Stufe 1: jede Seite eines gesprochenen Untertitels in ROOM6000 (Annette, Birkin-Film)
 * und ROOM6030 (Leon, msg 4) beginnt mit dem Namenskopf `05 <Farbe> <Sprecher>: 05 00`.
 *
 * NUTZER 18.11.2026 (woertlich): "Du hast bei den übernommenen Sätzen aus Resident Evil 2 nicht den Namen in den Dialog mit davor
 * geschrieben wie bei den rest auch...." - "Ich meinte bei Anette. kann aber auch sein das es nur so aussah." Memory
 * reai-v2-sprechername-vor-satz: jeder gesprochene Satz traegt den Sprechernamen wie die uebrigen Untertitel, Systemtexte nicht.
 * Befund (Zensus scratchpad/sprechernamen/BERICHT.md A2): 6000 msg 40/41/42 (RE2 ROOM4090 msg 0x0A+0x0B, 0x14+0x15, 0x17+0x18) waren
 * je EINE Nachricht mit Seitenwechsel `04 01 02 N 04 00`, Seite 2 ohne Namen; ebenso 6030 msg 4 (Seite 2 "But we can't wait. ...").
 *
 * Teile (Aufruf: test_r42_namen <teil>):
 *   texte   ROOM6000 und ROOM6030 wie im Spiel geladen (re15_msg_load_room_block: RDT-Texte + alle Port-Texte des Raums). Jede
 *           Nachricht, deren erste Seite mit einem Namenskopf beginnt (`04 00 05 cc <Name>: 05 00`, Form ROOM6000 msg 1 @0x01287,
 *           ROOM6030 msg 0 @0x013BE), traegt denselben Kopf auf JEDER weiteren Seite (direkt hinter dem `04 00` nach `02 N`).
 *           Dazu: die Annette-Saetze 40..42/48..50 in 6000 sind da, Leons 6030 msg 4 hat zwei Seiten.
 *   seite   Schreibmaschine (msg_common.c, ohne Taste) auf 6030 msg 4: Seite 2 kommt nach dem Seitenwechsel `02 63` (0x63 Bilder,
 *           Form ROOM3060 @0x1244, N = ROOM6030 Nachlauf @0x13F5 - unveraendert) und das gezeigte Fenster [page, parse) beginnt
 *           mit `04 00 05 01 "Leon:" 05 00`.
 * Gegenprobe (rot auf r42/story 8db089f5d): texte meldet 4 Seiten ohne Namen (6000 msg 40/41/42 Seite 2, 6030 msg 4 Seite 2),
 * seite meldet das Fenster ohne Kopf (Bericht BERICHT_s1_r1.md).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_aot.h"
#include "re15_room.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

static int g_fail = 0;
#define PRUEF(cond, ...) do { if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); g_fail = 1; } \
                              else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)(sz > 0 ? sz : 1));
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint8_t   *s_raw = NULL;
static size_t     s_rawsz = 0;
static re15_rdt_t s_rdt;

/* wie test_r42_rest.c raum_laden: RDT parsen, Nachrichtentabelle wie im Spiel (RDT + Port-Texte nach g_current_room_id) */
static int raum_laden(unsigned room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, room >> 12, room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_current_room_id = room;
    re15_msg_clear_room_block();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Namenskopf am Anfang einer Seite: `05 cc <Glyphen> 05 00` (hoechstens 20 Byte). Laenge oder 0. */
static int kopf_laenge(const uint8_t *m, int n)
{
    if (n < 4 || m[0] != 0x05) return 0;
    for (int i = 2; i + 1 < n && i < 20; i++) {
        if (m[i] == 0x05) return (m[i + 1] == 0x00) ? i + 2 : 0;
        if (m[i] < 0x0C) return 0;   /* Steuerbyte im Namen: kein Kopf */
    }
    return 0;
}

/* Seitenanfaenge einer Nachricht: Index hinter jedem `02 N` (Steuercodes wie re15_msg_decode_text / FUN_80028134: 2-Byte-Codes
 * 02/04/05/06/09/0A/0B, 07 = 1 Byte, 01 = Ende). */
static int seiten(const uint8_t *m, int n, int *anf, int max)
{
    int k = 0, i = 0;
    if (k < max) anf[k++] = 0;
    while (i < n) {
        const uint8_t b = m[i];
        if (b == 0x01 || b == 0x03) break;
        if (b == 0x02) { if (k < max) anf[k++] = i + 2; i += 2; continue; }
        if (b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }
        i += 1;
    }
    return k;
}

static void teil_texte(void)
{
    printf("=== texte: jede Seite eines Untertitels mit Namenskopf traegt den Kopf (6000 / 6030) ===\n");
    static const unsigned raeume[2] = { 0x6000, 0x6030 };
    int ges_seiten = 0, ges_ohne = 0, annette_6000 = 0, leon4_seiten = 0;
    for (int r = 0; r < 2; r++) {
        const unsigned room = raeume[r];
        if (raum_laden(room) != 0) { PRUEF(0, "ROOM%04X ladbar", room); continue; }
        int nachrichten = 0, seiten_n = 0, ohne = 0;
        for (int id = 0; id < 64; id++) {
            int len = 0;
            const unsigned char *m = re15_msg_get_raw(id, &len);
            if (!m || len < 6 || m[0] != 0x04 || m[1] != 0x00) continue;
            const int kl = kopf_laenge(m + 2, len - 2);
            if (!kl) continue;                       /* Systemtext / ohne Namen: nicht Gegenstand */
            nachrichten++;
            char name[24]; int nn = 0;
            {   char txt[512];
                re15_msg_decode_text(m, (size_t)len, txt, sizeof txt);
                for (; nn < 23 && txt[nn] && txt[nn] != ':'; nn++) name[nn] = txt[nn];
                name[nn] = 0;
            }
            int anf[16];
            const int ns = seiten(m, len, anf, 16);
            for (int s = 0; s < ns; s++) {
                seiten_n++;
                const int a = anf[s];
                /* Seite 1: Kopf ab Byte 2 (hinter `04 00`); weitere: `04 00` des Seitenwechsels, dann der Kopf */
                const int ok = (a + 2 + kl <= len) && m[a] == 0x04 && m[a + 1] == 0x00 && !memcmp(m + a + 2, m + 2, (size_t)kl);
                if (!ok) {
                    ohne++;
                    printf("        ROOM%04X msg %2d (%s) Seite %d von %d: OHNE NAMEN\n", room, id, name, s + 1, ns);
                }
            }
            if (room == 0x6000 && ((id >= 40 && id <= 42) || (id >= 48 && id <= 50)) && !strcmp(name, "Annette")) annette_6000++;
            if (room == 0x6030 && id == 4 && !strcmp(name, "Leon")) leon4_seiten = ns;
        }
        printf("    ROOM%04X: %d Nachrichten mit Namenskopf, %d Seiten, %d ohne Namen\n", room, nachrichten, seiten_n, ohne);
        ges_seiten += seiten_n; ges_ohne += ohne;
    }
    PRUEF(annette_6000 == 6, "ROOM6000: Annettes RE2-Saetze 40..42 (Satz 1) und 48..50 (Satz 2) geladen (%d von 6)", annette_6000);
    PRUEF(leon4_seiten == 2, "ROOM6030 msg 4 (Leon) hat zwei Seiten (%d)", leon4_seiten);
    PRUEF(ges_seiten > 20 && ges_ohne == 0, "jede Seite traegt den Namenskopf ihrer Nachricht: %d Seiten, %d ohne Namen", ges_seiten,
          ges_ohne);
}

static void teil_seite(void)
{
    printf("=== seite: 6030 msg 4 in der Schreibmaschine, Seite 2 ohne Taste ===\n");
    if (raum_laden(0x6030) != 0) { PRUEF(0, "ROOM6030 ladbar"); return; }
    scd_vm_init(); re15_aot_init();
    raum_laden(0x6030);
    int len = 0;
    const unsigned char *m = re15_msg_get_raw(4, &len);
    const int kl = (m && len > 4) ? kopf_laenge(m + 2, len - 2) : 0;
    PRUEF(kl == 9 && m[2] == 0x05 && m[3] == 0x01, "6030 msg 4 beginnt mit `04 00 05 01 \"Leon:\" 05 00` (%d B Kopf)", kl);
    if (!kl) return;
    re15_dialog_open(4, 0);
    int bild = 0, b_seite2 = -1, b_seite1 = -1, fenster_ok = 0;
    for (; bild < 2000 && g_scd.message_active; bild++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        const unsigned char *r; int l, i;
        re15_msg_tick(&r, &l, &i);
        if (b_seite1 < 0 && g_scd.message_fsm == 2) b_seite1 = bild;           /* Seite 1 steht, Seitenwechsel-Zeitgeber laeuft */
        if (b_seite2 < 0 && g_scd.message_page > 0 && g_scd.message_parse > g_scd.message_page) {
            b_seite2 = bild;
            const int p = (int)g_scd.message_page;
            fenster_ok = (p + 2 + kl <= len) && m[p] == 0x04 && m[p + 1] == 0x00 && !memcmp(m + p + 2, m + 2, (size_t)kl);
            break;
        }
    }
    PRUEF(b_seite1 >= 0 && b_seite2 > b_seite1 && b_seite2 - b_seite1 >= 0x63 - 1 && b_seite2 - b_seite1 <= 0x63 + 1,
          "Seite 2 ohne Taste %d Bilder nach dem Seitenende (`02 63`, unveraendert)", b_seite2 - b_seite1);
    PRUEF(fenster_ok, "gezeigtes Fenster von Seite 2 beginnt mit `04 00 05 01 \"Leon:\" 05 00`");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "texte") || !strcmp(teil, "alle")) teil_texte();
    if (!strcmp(teil, "seite") || !strcmp(teil, "alle")) teil_seite();
    printf(g_fail ? "=== FAIL ===\n" : "=== OK ===\n");
    return g_fail;
}
