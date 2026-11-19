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
 *   (seite  Stufe 1: 6030 msg 4 Seite 2 mit "Leon:" - entfallen mit Stufe 4, msg 4 hat nur noch eine Seite, Satz 2 = msg 32.)
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
 *
 * Stufe 4 (Lesezeit und Zeilenzahl, tools/r42_namen/lesbar_s4.py):
 *   zeilen  Alle neuen bzw. geaenderten Untertitel des Strangs (6000 msg 40..42/48..50, 6030 msg 4/32, 10D0 msg 18/19/24/25,
 *           1150 msg 12/13, 2000 msg 4/5/7/8) wie im Spiel geladen: jede Seite hoechstens 2 Zeilen (40%-Disc: Zeilenwechsel
 *           y = 180 + 16 fest @0x800289a0/@0x800289ac, 0 von 700 Seiten mit drei Zeilen; Zeile 3 (y 212) laege auf dem
 *           Letterbox-Balken ab y 216, Tabelle @0x80072ecc, Halbwort @0x80072f06), keine Zeile mit nur einem Wort, jede Zeile
 *           <= 271 px (Zeile 1 mit dem Namenskopf), jede Seite mit dem Namenskopf. Gegenprobe rot auf 70d822290 (Stand vor Stufe 4).
 *   seitenwechsel  Schreibmaschine ohne Taste: Seite 2 der zweiseitigen Saetze erscheint N + 1 Bilder nach Seite 1, N = Bild des
 *           ersten Worts von Seite 2 in der Aufnahme minus Abstand zur Stimme (lesbar_s4.seitenwartezeit): 6000 msg 48/49/50 = 90/57/71
 *           (RE2-Stimme Xa 0x09/0x11/0x14 "of" 4.38 s / "to" 4.60 s / "prior" 5.08 s, Satz 2 41/81/81 Bilder nach der Stimme),
 *           10D0 msg 18/24/25 = 64/100/83 (main18/24/25.wav Pausenende 2.12/3.34/2.76 s). Fenster beginnt mit dem Namenskopf.
 *   standzeit  ROOM6030 Wagen-Programm (k_s80_6030_sub02): wie lange steht jede Seite (bis die Nachricht schliesst oder die naechste
 *           Zeile kommt; Sleep-Summe im Programm)? msg 4 und msg 32 (Leons zwei Saetze) stehen mindestens so lange wie die
 *           Original-Zeile msg 0 (RDT @0x13BE, zweizeilig, Nachlauf `04 01 01 63` @0x13F5 = 99 Bilder). Gegenprobe rot auf 70d822290
 *           (msg 4 Seite 2 stand ~20 Bilder).
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
            if (room == 0x6030 && (id == 4 || id == 32) && !strcmp(name, "Leon")) leon4_seiten += ns;   /* Stufe 4: 4 + 32 je eine Seite */
        }
        printf("    ROOM%04X: %d Nachrichten mit Namenskopf, %d Seiten, %d ohne Namen\n", room, nachrichten, seiten_n, ohne);
        ges_seiten += seiten_n; ges_ohne += ohne;
    }
    PRUEF(annette_6000 == 6, "ROOM6000: Annettes RE2-Saetze 40..42 (Satz 1) und 48..50 (Satz 2) geladen (%d von 6)", annette_6000);
    PRUEF(leon4_seiten == 2, "ROOM6030 msg 4 + msg 32 (Leons zwei Saetze, Stufe 4) je eine Seite mit \"Leon:\" (%d Seiten)", leon4_seiten);
    PRUEF(ges_seiten > 20 && ges_ohne == 0, "jede Seite traegt den Namenskopf ihrer Nachricht: %d Seiten, %d ohne Namen", ges_seiten,
          ges_ohne);
}

/* ---- Stufe 4: Zeilenzahl, Seitenwechsel, Standzeit -------------------------------------------------------------------------- */
#include "font_width.h"
#include "re15_story80_r41.h"

typedef struct { unsigned raum; int id; } nid_t;
static const nid_t k_s4[] = {
    { 0x6000, 40 }, { 0x6000, 41 }, { 0x6000, 42 }, { 0x6000, 48 }, { 0x6000, 49 }, { 0x6000, 50 },
    { 0x6030, 4 }, { 0x6030, 32 },
    { 0x10D0, 18 }, { 0x10D0, 19 }, { 0x10D0, 24 }, { 0x10D0, 25 },
    { 0x1150, 12 }, { 0x1150, 13 },
    { 0x2000, 4 }, { 0x2000, 5 }, { 0x2000, 7 }, { 0x2000, 8 },
};
#define S4_N ((int)(sizeof k_s4 / sizeof k_s4[0]))

/* Eine Seite [a, e): Zeilen mit Glyphen, kleinste Wortzahl einer Zeile, breiteste Zeile (Glyphen inkl. Namenskopf, das Leerzeichen
 * 00 zaehlt mit; Steuercodes 04/05/06/09/0A/0B mit Argument zeichnen nicht; Breiten include/font_width.h = DEBUG.BIN[0x4416+code],
 * Zaehlweise wie test_r41_story80.c max_zeilenbreite). */
static void seite_messen(const uint8_t *m, int a, int e, int *zeilen, int *min_worte, int *max_px)
{
    int z = 0, w = 0, worte = 0, im_wort = 0, glyph = 0;
    *min_worte = 99; *max_px = 0;
    for (int i = a; i <= e; i++) {
        const int ende = (i == e);
        const uint8_t c = ende ? 0x08 : m[i];
        if (!ende && (c == 0x01 || c == 0x02 || c == 0x03)) { i = e - 1; continue; }
        if (!ende && (c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B)) { i++; continue; }
        if (!ende && c == 0x07) continue;
        if (c == 0x08) {
            if (glyph) { z++; if (worte < *min_worte) *min_worte = worte; if (w > *max_px) *max_px = w; }
            w = 0; worte = 0; im_wort = 0; glyph = 0;
            continue;
        }
        w += re15_font_width[c];
        if (c == 0x00) { im_wort = 0; continue; }
        glyph = 1;
        if (!im_wort) { worte++; im_wort = 1; }
    }
    *zeilen = z;
}

/* Seiten einer Nachricht: Anfang (hinter `02 N`) und Ende (Index des `02` bzw. `01`) */
static int seiten_enden(const uint8_t *m, int n, int *anf, int *end, int max)
{
    int k = 0, i = 0;
    anf[0] = 0;
    while (i < n && k < max) {
        const uint8_t b = m[i];
        if (b == 0x01 || b == 0x03) { end[k++] = i; break; }
        if (b == 0x02) { end[k++] = i; if (k < max) anf[k] = i + 2; i += 2; continue; }
        if (b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }
        i += 1;
    }
    return k;
}

static void teil_zeilen(void)
{
    printf("=== zeilen: neue/geaenderte Untertitel des Strangs - je Seite <= 2 Zeilen, keine Ein-Wort-Zeile, <= 271 px, Name ===\n");
    int geprueft = 0, schlecht = 0;
    for (int k = 0; k < S4_N; k++) {
        if (raum_laden(k_s4[k].raum) != 0) { PRUEF(0, "ROOM%04X ladbar", k_s4[k].raum); continue; }
        int len = 0;
        const unsigned char *m = re15_msg_get_raw(k_s4[k].id, &len);
        if (!m || len < 6) { PRUEF(0, "ROOM%04X msg %d vorhanden", k_s4[k].raum, k_s4[k].id); schlecht++; continue; }
        const int kl = kopf_laenge(m + 2, len - 2);
        int anf[8], end[8];
        const int ns = seiten_enden(m, len, anf, end, 8);
        char txt[512];
        re15_msg_decode_text(m, (size_t)len, txt, sizeof txt);
        for (int s = 0; s < ns; s++) {
            int z, mw, px;
            seite_messen(m, anf[s], end[s], &z, &mw, &px);
            const int a = anf[s];
            const int kopf_ok = kl && (a + 2 + kl <= len) && m[a] == 0x04 && m[a + 1] == 0x00 && !memcmp(m + a + 2, m + 2, (size_t)kl);
            const int ok = z >= 1 && z <= 2 && mw >= 2 && px <= 271 && kopf_ok;
            geprueft++;
            if (!ok) schlecht++;
            printf("    %s ROOM%04X msg %2d Seite %d/%d: %d Zeilen, kleinste Zeile %d Woerter, breiteste %d px, Name %s  \"%.80s\"\n",
                   ok ? "ok  " : "FAIL", k_s4[k].raum, k_s4[k].id, s + 1, ns, z, mw, px, kopf_ok ? "ja" : "NEIN", txt);
        }
    }
    PRUEF(geprueft >= S4_N && schlecht == 0, "%d Seiten geprueft, %d mit mehr als 2 Zeilen / Ein-Wort-Zeile / > 271 px / ohne Namen",
          geprueft, schlecht);
}

/* Schreibmaschine ohne Taste: Bild, in dem Seite 2 erscheint (Fenster [page, parse) nicht leer), und Bild, in dem die Nachricht
 * schliesst (-1 = nicht erreicht). kopf2: beginnt das Fenster von Seite 2 mit dem Namenskopf? */
static void schreibmaschine_messen(int id, int *b_seite2, int *b_zu, int *kopf2)
{
    int len = 0;
    const unsigned char *m = re15_msg_get_raw(id, &len);
    const int kl = (m && len > 4) ? kopf_laenge(m + 2, len - 2) : 0;
    re15_dialog_open(id, 0);
    *b_seite2 = -1; *b_zu = -1; *kopf2 = 0;
    for (int bild = 1; bild < 3000; bild++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        const unsigned char *r; int l, i;
        re15_msg_tick(&r, &l, &i);
        if (*b_seite2 < 0 && g_scd.message_page > 0 && g_scd.message_parse > g_scd.message_page) {
            *b_seite2 = bild;
            const int p = (int)g_scd.message_page;
            *kopf2 = kl && (p + 2 + kl <= len) && m[p] == 0x04 && m[p + 1] == 0x00 && !memcmp(m + p + 2, m + 2, (size_t)kl);
        }
        if (!g_scd.message_active) { *b_zu = bild; break; }
    }
}

static void teil_seitenwechsel(void)
{
    printf("=== seitenwechsel: Seite 2 beim ersten Wort der Aufnahme (tools/r42_namen/lesbar_s4.py seitenwartezeit) ===\n");
    /* N = round(30 * t) - S; Seite 2 erscheint N + 1 Bilder nach dem Oeffnen (Seite 1 sofort per `04 00`, dann `02 N`) */
    static const struct { unsigned raum; int id, n; const char *beleg; } k_w[] = {
        { 0x6000, 48, 90, "RE2 Xa 0x09 \"of\" t=4.38 s -> 131 - Sleep 41 (RE2 @0x02722)" },
        { 0x6000, 49, 57, "RE2 Xa 0x11 \"to\" t=4.60 s -> 138 - Sleep 81 (RE2 @0x027D8)" },
        { 0x6000, 50, 71, "RE2 Xa 0x14 \"prior\" t=5.08 s -> 152 - Sleep 81 (RE2 @0x02850)" },
        { 0x10D0, 18, 64, "main18.wav Pausenende 2.12 s" },
        { 0x10D0, 24, 100, "main24.wav Pausenende 3.34 s" },
        { 0x10D0, 25, 83, "main25.wav Pausenende 2.76 s" },
    };
    for (unsigned k = 0; k < sizeof k_w / sizeof k_w[0]; k++) {
        if (raum_laden(k_w[k].raum) != 0) { PRUEF(0, "ROOM%04X ladbar", k_w[k].raum); continue; }
        scd_vm_init(); re15_aot_init();
        raum_laden(k_w[k].raum);
        int s2, zu, kopf2;
        schreibmaschine_messen(k_w[k].id, &s2, &zu, &kopf2);
        /* Takt 1 = erstes Bild mit Seite 1 (Message_on im Spiel-Takt, die Schreibmaschine laeuft im naechsten Bild: platform/pc/main.c
         * re15_audio_tick ~6201 -> re15_msg_tick ~6233 -> re15_game_step ~8213); die Stimme ist ab Takt 2 zu hoeren (re15_audio_tick
         * mischt vor dem Abholen der Ereignisse, audio_pc.c ~4011/~4038). Wort bei t -> Takt 2 + 30 t - S = Takt N + 2. */
        PRUEF(s2 == k_w[k].n + 2 && kopf2, "ROOM%04X msg %d: Seite 2 im Takt %d = %d Bilder nach Seite 1 (Soll N + 1 = %d; %s), "
              "Fenster mit Namenskopf %s", k_w[k].raum, k_w[k].id, s2, s2 - 1, k_w[k].n + 1, k_w[k].beleg, kopf2 ? "ja" : "NEIN");
    }
}

static void teil_standzeit(void)
{
    printf("=== standzeit: ROOM6030 Wagen-Programm (k_s80_6030_sub02) - wie lange steht jede Seite? ===\n");
    if (raum_laden(0x6030) != 0) { PRUEF(0, "ROOM6030 ladbar"); return; }
    scd_vm_init(); re15_aot_init();
    raum_laden(0x6030);
    int pn = 0;
    const uint8_t *p = re15_story80_programm(RE15_S80_P_WAGEN, &pn);
    if (!p || pn < 8) { PRUEF(0, "Wagen-Programm vorhanden"); return; }
    /* Message_on und die Sleep-Summe bis zur naechsten (`09 0a NN 00`, Bilder; Opcode-Laengen scd_opcode_size_at) */
    int mid[40], fenster[40], nm = 0;
    for (int pc = 0; pc < pn;) {
        const int sz = scd_opcode_size_at(p + pc);
        if (sz <= 0) { PRUEF(0, "Programm +0x%04X: Opcode %02x unbekannt", pc, p[pc]); return; }
        if (p[pc] == 0x2B && nm < 40) { mid[nm] = p[pc + 1]; fenster[nm] = 0; nm++; }
        else if (p[pc] == 0x09 && p[pc + 1] == 0x0A && nm > 0) fenster[nm - 1] += p[pc + 2];
        pc += sz;
    }
    int v0 = -1, v4 = -1, v32 = -1, s2_4 = 0, kurz = 0;
    for (int i = 0; i < nm; i++) {
        int s2, zu, kopf2;
        schreibmaschine_messen(mid[i], &s2, &zu, &kopf2);
        const int w = (i + 1 < nm) ? fenster[i] : 9999;          /* letzte Zeile: kein Nachfolger im Programm */
        const int ende = (zu > 0 && zu < w) ? zu : w;            /* sichtbar bis zum Schliessen oder bis zur naechsten Zeile */
        const int sicht1 = (s2 > 0 && s2 < ende) ? s2 : ende;
        const int sicht2 = (s2 > 0) ? (s2 < ende ? ende - s2 : 0) : -1;
        printf("    msg %2d: Text lebt %3d Bilder, naechste Zeile nach %4d -> Seite 1 sichtbar %3d Bilder", mid[i], zu, w, sicht1);
        if (s2 > 0) printf(", Seite 2 ab Bild %d sichtbar %d Bilder", s2, sicht2);
        printf("%s\n", (zu > w) ? "  (von der naechsten Zeile ersetzt)" : "");
        if (s2 > 0 && sicht2 < 99) kurz++;
        if (mid[i] == 0) v0 = sicht1;
        if (mid[i] == 4) { v4 = sicht1; if (s2 > 0) s2_4 = 1; }
        if (mid[i] == 32) v32 = sicht1;
    }
    PRUEF(v0 >= 99, "Vergleich: Original-Zeile msg 0 (RDT @0x13BE, zweizeilig, Nachlauf `01 63` @0x13F7) steht %d Bilder", v0);
    PRUEF(!s2_4 && v4 >= v0, "msg 4 (Leons erster Satz) eine Seite (%s), steht %d Bilder >= msg 0 (%d)", s2_4 ? "NEIN" : "ja", v4, v0);
    PRUEF(v32 >= v0, "msg 32 (Leons zweiter Satz, eigene Nachricht) steht %d Bilder >= msg 0 (%d)", v32, v0);
    PRUEF(kurz == 0, "keine zweite Seite im Wagen-Programm steht kuerzer als 99 Bilder (%d)", kurz);
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

/* "Name: a,Name: b" -> "Name: a, b": jeder weitere Namenskopf (Seite 2, Stufe 4) wird zu einem Leerzeichen */
static void ohne_koepfe(const char *in, char *out, size_t n)
{
    const char *c = strchr(in, ':');
    size_t nl = (c && c - in < 12) ? (size_t)(c - in) + 1 : 0, o = 0;
    for (size_t i = 0; in[i] && o + 1 < n;) {
        if (nl && i > 0 && !strncmp(in + i, in, nl)) { out[o++] = ' '; i += nl; continue; }
        out[o++] = in[i++];
    }
    out[o] = 0;
}

static void teil_fortsetzung(void)
{
    printf("=== fortsetzung: Mod-Fortsetzungszeilen am Satzende der Aufnahme, jede mit Namen (10D0/1150/2000 + Elza 10D1/1151/2001) ===\n");
    static const struct { unsigned raum; int a, b; const char *ta, *tb; } paare[5] = {
        /* Stufe 4: 10D0 msg 18/24/25 zwei Seiten (Seite 2 mit Namenskopf; re15_msg_decode_text haengt Seiten ohne Leerzeichen an) */
        { 0x10D0, 18, 19, "Marvin: You've already seen him,Marvin: I thought he was the only one left at the station.",
                          "Marvin: Tell me, are there other survivors?" },
        { 0x10D0, 24, 25, "Leon: I don't know, it remains to try,Leon: the helicopter abandoned us, the phones are down.",
                          "Leon: And with the building besieged by those things,Leon: I don't see any more options." },
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
        char ka[512], kb[512];   /* Stufe 4: Namenskoepfe der zweiten Seiten fuer den Wortvergleich entfernen */
        ohne_koepfe(ta, ka, sizeof ka); ohne_koepfe(tb, kb, sizeof kb);
        int n2 = norm_worte(ka, w2, 96, 1); n2 += norm_worte(kb, w2 + n2, 96 - n2, 1);
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
    if (!strcmp(teil, "zeilen") || !strcmp(teil, "alle")) teil_zeilen();
    if (!strcmp(teil, "seitenwechsel") || !strcmp(teil, "alle")) teil_seitenwechsel();
    if (!strcmp(teil, "standzeit") || !strcmp(teil, "alle")) teil_standzeit();
    if (!strcmp(teil, "fortsetzung") || !strcmp(teil, "alle")) teil_fortsetzung();
    printf(g_fail ? "=== FAIL ===\n" : "=== OK ===\n");
    return g_fail;
}
