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
 *           Form ROOM3060 @0x1244, N = ROOM6030 Nachlauf `01 63` @0x13F7 - unveraendert) und das gezeigte Fenster [page, parse) beginnt
 *           mit `04 00 05 01 "Leon:" 05 00`.
 * Gegenprobe (rot auf r42/story 8db089f5d): texte meldet 4 Seiten ohne Namen (6000 msg 40/41/42 Seite 2, 6030 msg 4 Seite 2),
 * seite meldet das Fenster ohne Kopf (Bericht BERICHT_s1_r1.md).
 *
 * Stufe 2 (Mod-Fortsetzungszeilen, include/re15_namen_r42.h, tools/r42_namen/fortsetzung_bauen.py):
 *   fortsetzung  ROOM10D0/1150/2000 (Leon) und 10D1/1151/2001 (Elza) wie im Spiel geladen. (1) Jede Nachricht mit eigener Aufnahme
 *           (synchro/STAGEn/roomXXXX/mainNN.wav vorhanden, audio_pc.c-Regel) beginnt mit einem Namenskopf. (2) Die fuenf Paare zeigen
 *           genau den Satz ihrer Aufnahme (Whisper medium.en, scratchpad/sprechernamen/asr_out.txt): Text je Nachricht gleich der
 *           erwarteten Fassung. (3) Wortlaut der Mod: A+B zusammen sind Wort fuer Wort die RDT-Nachrichten A+B (ohne Gross/Klein
 *           und Komma/Punkt am Wortende; nur die Trennstelle unterscheidet sich darin). (4) Laenge <= 128 Byte (PSX MSG_RAW_LEN).
 *           Gegenprobe rot auf 8db089f5d: 10D0 msg 19/25, 1150 msg 13, 2000 msg 5/8 ohne Namen, Texte am alten Schnitt.
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

/* ---- Stufe 2: Mod-Fortsetzungszeilen ---------------------------------------------------------------------------------------- */
#ifndef RE15_SYNCHRO_DIR
#error RE15_SYNCHRO_DIR fehlt
#endif

static int datei_da(const char *p) { FILE *f = fopen(p, "rb"); if (!f) return 0; fclose(f); return 1; }

/* RDT-Nachricht direkt aus dem Block (Offsettabelle wie msg_common.c re15_msg_load_room_block), ohne Port-Texte */
static int rdt_nachricht(int id, const uint8_t **out)
{
    const uint8_t *blk = s_rdt.messages; const int bs = (int)s_rdt.messages_size;
    if (!blk || bs < 4) return 0;
    const int n_off = (blk[0] | (blk[1] << 8)) / 2;
    if (id >= n_off) return 0;
    const int o0 = blk[id * 2] | (blk[id * 2 + 1] << 8);
    int o1 = bs;
    if (id + 1 < n_off) { const int nx = blk[(id + 1) * 2] | (blk[(id + 1) * 2 + 1] << 8); if (nx > o0 && nx <= bs) o1 = nx; }
    *out = blk + o0;
    return o1 - o0;
}

/* Woerter ohne Gross/Klein, Komma/Punkt am Wortende weg; mit ab: Sprecherkopf "Name:" am Anfang weg. */
static int norm_worte(const char *txt, char w[][32], int max, int ab)
{
    int n = 0; const char *p = txt;
    if (ab) { const char *c = strchr(p, ':'); if (c && c - p < 12) p = c + 1; }
    while (*p && n < max) {
        while (*p == ' ') p++;
        if (!*p) break;
        int k = 0;
        while (*p && *p != ' ' && k < 31) { const char c = *p++; w[n][k++] = (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c; }
        while (*p && *p != ' ') p++;
        while (k > 0 && (w[n][k - 1] == ',' || w[n][k - 1] == '.')) k--;
        w[n][k] = 0; n++;
    }
    return n;
}

static void teil_fortsetzung(void)
{
    printf("=== fortsetzung: Mod-Fortsetzungszeilen am Satzende der Aufnahme, jede mit Namen (10D0/1150/2000 + Elza 10D1/1151/2001) ===\n");
    static const struct { unsigned raum; int a, b; const char *ta, *tb; } paare[5] = {
        { 0x10D0, 18, 19, "Marvin: You've already seen him, I thought he was the only one left at the station.",
                          "Marvin: Tell me, are there other survivors?" },
        { 0x10D0, 24, 25, "Leon: I don't know, it remains to try, the helicopter abandoned us, the phones are down.",
                          "Leon: And with the building besieged by those things, I don't see any more options." },
        { 0x1150, 12, 13, "Irons: And remember this Leon, never let your guard down... uniform or not.",
                          "Irons: Those things... are no longer what they were, don't make my mistake..." },
        { 0x2000, 4, 5,   "Leon: Yes, before meeting Ada I saw her coming in this way, if I hadn't seen her...",
                          "Leon: Maybe we wouldn't have found this way out." },
        { 0x2000, 7, 8,   "Ada: So there is no time to lose, we do not know what things are in the sewers.",
                          "Ada: Zombies are not the only things we should worry about here." },
    };
    static const unsigned raeume[6] = { 0x10D0, 0x1150, 0x2000, 0x10D1, 0x1151, 0x2001 };
    int stimmen = 0, ohne = 0;
    for (int r = 0; r < 6; r++) {
        const unsigned room = raeume[r];
        if (raum_laden(room) != 0) { PRUEF(0, "ROOM%04X ladbar", room); continue; }
        int st = 0, oh = 0;
        for (int id = 0; id < 64; id++) {
            char wav[700];
            snprintf(wav, sizeof wav, "%s/STAGE%u/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, room >> 12, room, id);
            if (!datei_da(wav)) continue;
            int len = 0;
            const unsigned char *m = re15_msg_get_raw(id, &len);
            if (!m || len < 6) continue;
            st++;
            const int kl = (m[0] == 0x04 && m[1] == 0x00) ? kopf_laenge(m + 2, len - 2) : 0;
            if (!kl) {
                char txt[512];
                re15_msg_decode_text(m, (size_t)len, txt, sizeof txt);
                printf("        ROOM%04X msg %2d mit Aufnahme OHNE NAMEN: \"%.60s\"\n", room, id, txt);
                oh++;
            }
        }
        printf("    ROOM%04X: %d Nachrichten mit Aufnahme, %d ohne Namen\n", room, st, oh);
        stimmen += st; ohne += oh;
    }
    PRUEF(stimmen > 20 && ohne == 0, "jede Nachricht mit Aufnahme beginnt mit dem Namen: %d mit Aufnahme, %d ohne Namen", stimmen, ohne);

    for (int i = 0; i < 5; i++) {
        if (raum_laden(paare[i].raum) != 0) { PRUEF(0, "ROOM%04X ladbar", paare[i].raum); continue; }
        char ta[512] = "", tb[512] = "", oa[512] = "", ob[512] = "";
        int la = 0, lb = 0;
        const unsigned char *ma = re15_msg_get_raw(paare[i].a, &la), *mb = re15_msg_get_raw(paare[i].b, &lb);
        if (ma) re15_msg_decode_text(ma, (size_t)la, ta, sizeof ta);
        if (mb) re15_msg_decode_text(mb, (size_t)lb, tb, sizeof tb);
        PRUEF(!strcmp(ta, paare[i].ta), "ROOM%04X msg %d = Satz der Aufnahme main%02d: \"%s\"", paare[i].raum, paare[i].a, paare[i].a, ta);
        PRUEF(!strcmp(tb, paare[i].tb), "ROOM%04X msg %d = Satz der Aufnahme main%02d: \"%s\"", paare[i].raum, paare[i].b, paare[i].b, tb);
        PRUEF(la > 0 && la <= 128 && lb > 0 && lb <= 128, "ROOM%04X msg %d/%d: %d/%d Byte <= 128 (PSX MSG_RAW_LEN)", paare[i].raum,
              paare[i].a, paare[i].b, la, lb);
        /* Wortlaut der Mod: RDT A+B gegen Port A+B */
        const uint8_t *ra = NULL, *rb = NULL;
        const int nra = rdt_nachricht(paare[i].a, &ra), nrb = rdt_nachricht(paare[i].b, &rb);
        if (nra > 0) re15_msg_decode_text(ra, (size_t)nra, oa, sizeof oa);
        if (nrb > 0) re15_msg_decode_text(rb, (size_t)nrb, ob, sizeof ob);
        static char w1[96][32], w2[96][32];
        int n1 = norm_worte(oa, w1, 96, 1); n1 += norm_worte(ob, w1 + n1, 96 - n1, 0);
        int n2 = norm_worte(ta, w2, 96, 1); n2 += norm_worte(tb, w2 + n2, 96 - n2, 1);
        int gleich = (n1 == n2 && n1 > 5);
        for (int k = 0; gleich && k < n1; k++) if (strcmp(w1[k], w2[k])) gleich = 0;
        PRUEF(gleich, "ROOM%04X msg %d/%d: Wortlaut der Mod Wort fuer Wort erhalten (%d/%d Woerter)", paare[i].raum, paare[i].a,
              paare[i].b, n1, n2);
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "texte") || !strcmp(teil, "alle")) teil_texte();
    if (!strcmp(teil, "seite") || !strcmp(teil, "alle")) teil_seite();
    if (!strcmp(teil, "fortsetzung") || !strcmp(teil, "alle")) teil_fortsetzung();
    printf(g_fail ? "=== FAIL ===\n" : "=== OK ===\n");
    return g_fail;
}
