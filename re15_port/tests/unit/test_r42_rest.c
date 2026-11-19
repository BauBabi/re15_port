/*
 * test_r42_rest.c — Runde 42 Strang "rest" (v0.8.31), Stufe 1: Riegel der Restpunkte.
 *
 * msglaenge  (Restpunkt a) Jede Nachricht, die der Port aus einer ausgelieferten RDT (und den Port-Texten, die
 *            re15_msg_load_room_block danach einsetzt) in seine Tabelle legt, muss ihr EIGENES Ende mitbringen
 *            (0x01 + Argument bzw. die Ja/Nein-Marke 0x03). Original: es gibt keine Kopie und keine Laengengrenze -
 *            Message_on (Handler @0x800404f4, a1 = 0x300 @0x80040500) -> FUN_80027e68 Zweig 0x300 -> `jal 0x800c004c`
 *            @0x80027fb4; die Routine @0x800c004c (Laufzeit-Code, gelesen aus dem Savestate stage_saves/
 *            mzd_stage1_briefing.sav: `lui v0,0x800b / lw v0,-0x3888(v0)` = RDT-Zeiger 0x800ac778, `lw v0,0x3c(v0)` =
 *            RDT+0x3C, `lhu t0,0(a0+v0) / jr ra / addu v0,v0,t0` @0x800c0094-a4) liefert den Zeiger IN die RDT, die
 *            Schreibmaschine FUN_80028134 liest bis zum 0x01. Der Port kopiert (s_msg_raw, msg_common.c) - mit der
 *            alten PC-Kapazitaet 192 endete ROOM2040/2041 msg 0 (Aushang "Notice of damage door", 411 Byte bis zum 0x01,
 *            RDT-Datei-Offset 0x175C) nach 192 Byte: die Schreibmaschine lief in "Ende der Bytes" (fsm 6) statt ans 0x01.
 *            Teil 2 faehrt die Schreibmaschine selbst: ROOM2040 msg 0 und ROOM6000 msg 49 (Port-Text, 127 Byte, zwei Seiten) laufen
 *            mit der Aktionstaste Seite fuer Seite bis an ihr 0x01 (fsm 4 = Warten nach dem Ende).
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

/* Laenge bis einschliesslich des eigenen Endes (0x01 + Argument; Ja/Nein: bis zur 0x03), Steuercodes wie
 * re15_msg_decode_text / FUN_80028134 (2-Byte-Codes 02/04/05/06/09/0A/0B, 07 = 1 Byte). -1 = kein Ende in [0, len). */
static int eigenes_ende(const uint8_t *m, int len)
{
    int i = (len > 0 && m[0] == 0x04) ? 2 : 0;
    while (i < len) {
        const uint8_t b = m[i];
        if (b == 0x01) return (i + 2 <= len) ? i + 2 : -1;
        if (b == 0x03) return i + 1;
        if (b == 0x02 || b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }
        i += 1;
    }
    return -1;
}

static uint8_t   *s_raw = NULL;
static size_t     s_rawsz = 0;
static re15_rdt_t s_rdt;

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

/* RDT-Scheibe der Nachricht id wie re15_msg_load_room_block (off[id] .. off[id+1] bzw. Blockende). */
static const uint8_t *rdt_scheibe(int id, int *out_len)
{
    const uint8_t *b = s_rdt.messages;
    const int bs = s_rdt.messages_size;
    *out_len = 0;
    if (!b || bs < 4) return NULL;
    const int n_off = (b[0] | (b[1] << 8)) / 2;
    if (id >= n_off) return NULL;
    const int o0 = b[id * 2] | (b[id * 2 + 1] << 8);
    int o1 = bs;
    if (id + 1 < n_off) { const int nx = b[(id + 1) * 2] | (b[(id + 1) * 2 + 1] << 8); if (nx > o0 && nx <= bs) o1 = nx; }
    if (o1 <= o0 || o0 >= bs) return NULL;
    *out_len = o1 - o0;
    return b + o0;
}

static void teil_msglaenge(void)
{
    printf("=== msglaenge: keine Nachricht wird beim Ablegen abgeschnitten (Original liest in-place @0x800c004c) ===\n");
    int raeume = 0, nachrichten = 0, laengste = 0, schlecht = 0, ohne_ende = 0, port_texte = 0;
    unsigned laengste_raum = 0; int laengste_id = -1;
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned rr = 0; rr < 0x40; rr++)
            for (unsigned v = 0; v < 2; v++) {
                const unsigned room = (st << 12) | (rr << 4) | v;
                if (raum_laden(room) != 0 || !s_rdt.messages) continue;
                raeume++;
                for (int id = 0; id < 256; id++) {
                    int len = 0, sl = 0;
                    const unsigned char *m = re15_msg_get_raw(id, &len);
                    if (!m) continue;
                    nachrichten++;
                    const uint8_t *sc = rdt_scheibe(id, &sl);
                    const int aus_rdt = sc && len <= sl && memcmp(sc, m, (size_t)len) == 0;
                    int noetig;
                    if (aus_rdt) {
                        noetig = eigenes_ende(sc, sl);
                        if (noetig < 0) {   /* Original-Daten ohne 0x01: das Original liest in die naechste Nachricht weiter (OFFEN) */
                            ohne_ende++;
                            printf("        ROOM%04X msg %3d: schon die RDT-Scheibe (%d Byte) hat kein 0x01/0x03 - Original-Daten\n",
                                   room, id, sl);
                            noetig = sl;
                        }
                    } else {
                        port_texte++;
                        noetig = eigenes_ende(m, len);   /* Port-Text: muss sein Ende selbst tragen */
                        if (noetig < 0) noetig = len + 1;
                    }
                    if (noetig > laengste) { laengste = noetig; laengste_raum = room; laengste_id = id; }
                    if (len < noetig) {
                        schlecht++;
                        printf("        ROOM%04X msg %3d: %d Byte abgelegt, %d noetig (%s) - ABGESCHNITTEN\n", room, id, len, noetig,
                               aus_rdt ? "RDT" : "Port-Text");
                    }
                }
            }
    PRUEF(raeume > 100 && nachrichten > 1000, "%d Raeume, %d Nachrichten gelesen (%d Port-Texte, %d Original-Scheiben ohne Ende)",
          raeume, nachrichten, port_texte, ohne_ende);
    PRUEF(schlecht == 0, "%d Nachrichten abgeschnitten (laengste: ROOM%04X msg %d, %d Byte bis zum Ende)", schlecht,
          laengste_raum, laengste_id, laengste);
}

/* Die Schreibmaschine mit der Aktionstaste (virtuell 0x4000, msg_common.c) bis zum Ende fahren. */
static void schreibmaschine(unsigned room, int id, int soll_ende)
{
    if (raum_laden(room) != 0) { PRUEF(0, "ROOM%04X ladbar", room); return; }
    scd_vm_init(); re15_aot_init();
    raum_laden(room);   /* scd_vm_init leert nichts an der Nachrichtentabelle, aber erst danach oeffnen */
    re15_dialog_open(id, 1);
    int max_parse = 0, seiten = 0, n = 0;
    for (; n < 20000 && g_scd.message_active && g_scd.message_fsm != 4 && g_scd.message_fsm != 5 && g_scd.message_fsm != 6; n++) {
        const uint16_t edge = (g_scd.message_fsm == 1) ? 0x4000 : 0;
        if (edge) seiten++;
        g_scd_pad_edge = edge; g_scd_pad_held = 0;
        const unsigned char *r; int l, i;
        re15_msg_tick(&r, &l, &i);
        if ((int)g_scd.message_parse > max_parse) max_parse = (int)g_scd.message_parse;
    }
    int len = 0;
    (void)re15_msg_get_raw(id, &len);
    PRUEF((g_scd.message_fsm == 4 || g_scd.message_fsm == 5) && max_parse == soll_ende,
          "ROOM%04X msg %d: Schreibmaschine nach %d Bildern und %d Seitenwechseln im Zustand %d (4/5 = am 0x01 angekommen, 6 = Bytes zu Ende), "
          "gelesen bis %d von %d gespeicherten Byte, Ende bei %d", room, id, n, seiten, (int)g_scd.message_fsm, max_parse, len,
          soll_ende);
}

static void teil_schreibmaschine(void)
{
    printf("=== schreibmaschine: lange Texte laufen bis an ihr 0x01 ===\n");
    schreibmaschine(0x2040, 0, 411);   /* Aushang, RDT 0x175C..0x18F6: 0x01 bei +409, Argument +410 -> 411 */
    /* Strang namen Stufe 4: 6030 msg 4 ist jetzt 74 B (Satz 2 = msg 32); laengster zweiseitiger Port-Text = 6000 msg 49 (127 B,
     * gen/birkinfilm_r42.inc k_bf_6000_msg49, Seitenwechsel `02 57`). */
    schreibmaschine(0x6000, 49, 127);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "msglaenge") || !strcmp(teil, "alle")) teil_msglaenge();
    if (!strcmp(teil, "schreibmaschine") || !strcmp(teil, "alle")) teil_schreibmaschine();
    printf(g_fail ? "=== FAIL ===\n" : "=== OK ===\n");
    return g_fail;
}
