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
 *
 * Stufe 5 (alle Untertitel hoechstens zwei Zeilen je Seite, tools/r42_namen/zweizeilig_s5.py + lesbar_s5.py):
 *   alle_zeilen     jede Nachricht jedes Raums (STAGE1..6, Leon- und Elza-Raeume) wie im Spiel geladen (RDT + Port-Texte des
 *                   Raumaufbaus): keine Seite mit mehr als zwei Zeilen. Gegenprobe rot auf 4c53a03e6 (40 Seiten) und 8db089f5d.
 *   quellen_zeilen  jede Byte-Tabelle in engine/src (.c, gen/.inc) als Nachricht gelesen (Port-Texte, die erst bei einem
 *                   Ereignis eingesetzt werden; ohne Inventar-Beschreibungen): keine Text-Seite mit mehr als zwei Zeilen.
 *   seitenwechsel   zusaetzlich 10D0 msg 17/26, 1150 msg 9, 30E1 msg 12 (Seite 2 am Pausenende der Aufnahme).
 *
 * Stufe 5 Runde 2 (Pruefbericht namen/pruef_s5_r1/PRUEFUNG.md R1-R4):
 *   quellen_zeilen  liest jetzt auch Tabellen mit Kommentar hinter `{` (cut_10f0.c, irons_tod_1150.c waren unsichtbar).
 *                   Gegenprobe rot auf 7d7f4d1c6 (10F0 msg 7/16, 1150 msg 23/27).
 *   darstellung     jede Nachricht jedes Raums ueber Message_on GEZEIGT (echte VM; Full-Text-Raeume 1170/1240/1241 wie im Spiel):
 *                   kein gezeigtes Fenster mit mehr als 2 Zeilen oder ueber den Bildrand. Gegenprobe rot auf 7d7f4d1c6 (Vorspann).
 *   ohne_stimme     zweiseitige Zeilen der RDT-Skripte ohne Stimme (VOICE OFF): jede Seite sichtbar, keine von der naechsten Zeile
 *                   abgeschnitten (Pflicht 30E1 14, 5011 3, 11B0 5, 11B0 8, 4010 8). Gegenprobe rot auf 7d7f4d1c6.
 *
 * Stufe 6 (veraltete Aufnahmen bleiben stumm, include/re15_altaufnahme_r42.h, tools/r42_namen/altaufnahmen_bauen.py):
 *   altaufnahme     Tabelle der 13 alten Aufnahmen gegen den Bestand synchro/ (Liste beim Configure): (1) genau die 13 Saetze
 *                   3020 0/1, 30E0 3/10/11/13, 6030 2/3/4/5/22, 6031 1/2; (2) ihr Untertitel im Spiel ist NICHT mehr der
 *                   RDT-Satz (bei der Port-Zeile 6030 22 ein Story-Text) - der Text ist neu; (3) jede dieser Dateien, die
 *                   noch den Fingerabdruck des Tags v0.8.30 traegt, gilt als "alt", eine ersetzte nicht; (4) NEGATIVKONTROLLE:
 *                   keine andere Datei des ganzen Bestands gilt als alt, und keine andere Datei hat denselben Inhalt
 *                   (Spiegelraum-Kopie); (5) ein geaendertes Byte, ein Byte mehr oder weniger, derselbe Inhalt unter anderem
 *                   Namen -> nicht alt; Gross/Klein und Rueckstrich im Pfad egal; (6) keine Umleitung (stimmen_r39, elza_r36,
 *                   Rekorder r38, Hilferufe r39, Elza-Montage r38) fuehrt auf diese Saetze oder von ihnen weg.
 *                   Der ECHTE Lader wird in probe_r42_namen_altlader.c gemessen (unit_r42_namen_altlader, _altlader_ersatz).
 *                   Stufe 7 (Nachtrag 19.11.2026, Entscheid des Orchestrators, PORT-WAHL: "stumm bleibt nur, was einen ANDEREN
 *                   Satz spricht; eine Aufnahme, die den neuen Satz woertlich oder bis auf ein Wort spricht, spielt"):
 *                   (7) REGEL an allen 15 Saetzen mit neuem Untertitel (tests/unit/gen_r42_namen_altsaetze.inc: abgehoerter
 *                   Wortlaut der Aufnahme, Spracherkenner 19.11.2026): der Untertitel wird aus dem GELADENEN Raum gelesen und
 *                   Wort fuer Wort mit der Aufnahme verglichen (laengste gemeinsame Wortfolge, hier selbst gerechnet). In der
 *                   Tabelle steht ein Satz genau dann, wenn mehr als 1 Wort des Untertitels nicht gesprochen wird oder die
 *                   Aufnahme mehr als 2 Woerter sagt, die nicht im Untertitel stehen. (8) 3020 main02 ("Leon, give me a
 *                   hand.") und 6030 main21 ("... We made it down here ...") stehen NICHT in der Tabelle: die Datei mit dem
 *                   Abdruck des Tags gilt nicht als alt -> spielt. Gegenprobe rot auf e9ddc6b7a (dort 15 Eintraege).
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
#include <ctype.h>

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
        /* Stufe 5 (tools/r42_namen/zweizeilig_s5.py): Mod-Zeilen mit drei Zeilen, Seite 2 an der Sprechpause der Aufnahme */
        { 0x10D0, 17, 87, "main17.wav Pause 2.40-2.89 s" },
        { 0x10D0, 26, 98, "main26.wav Pause 2.98-3.28 s" },
        { 0x1150,  9, 96, "main09.wav Pause 2.45-3.19 s" },
        { 0x30E1, 12, 61, "main12.wav Pause 1.63-2.04 s (Elza)" },
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


/* ---- Stufe 5: alle Raeume - keine gezeigte Seite mit mehr als zwei Zeilen ------------------------------------------------------ */
/* Text einer Seite [a, e) als ASCII mit " / " an jedem Zeilenwechsel (fuer die Ausgabe). */
static void seite_text(const uint8_t *m, int a, int e, char *out, size_t n)
{
    size_t o = 0;
    for (int i = a; i < e && o + 4 < n; i++) {
        const uint8_t c = m[i];
        if (c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) { i++; continue; }
        if (c == 0x07) continue;
        if (c == 0x08) { out[o++] = ' '; out[o++] = '/'; out[o++] = ' '; continue; }
        uint8_t t[2] = { c, 0x01 };
        char d[8];
        re15_msg_decode_text(t, 2, d, sizeof d);
        if (c == 0x00) { out[o++] = ' '; continue; }
        for (int k = 0; d[k] && o + 1 < n; k++) out[o++] = d[k];
    }
    out[o] = 0;
}

static void teil_alle_zeilen(void)
{
    printf("=== alle_zeilen: jede Nachricht jedes Raums (RDT + Port-Texte wie im Spiel geladen) - je Seite hoechstens 2 Zeilen ===\n");
    int raeume = 0, nachrichten = 0, seiten_n = 0, schlecht = 0;
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned rr = 0; rr < 0x40; rr++)
            for (unsigned v = 0; v < 2; v++) {
                const unsigned room = (st << 12) | (rr << 4) | v;
                if (raum_laden(room) != 0 || !s_rdt.messages) continue;
                raeume++;
                for (int id = 0; id < 256; id++) {
                    int len = 0;
                    const unsigned char *m = re15_msg_get_raw(id, &len);
                    if (!m || len < 2) continue;
                    nachrichten++;
                    int anf[16], end[16];
                    const int ns = seiten_enden(m, len, anf, end, 16);
                    for (int s = 0; s < ns; s++) {
                        int z, mw, px;
                        seite_messen(m, anf[s], end[s], &z, &mw, &px);
                        seiten_n++;
                        if (z <= 2) continue;
                        schlecht++;
                        char t[600]; seite_text(m, anf[s], end[s], t, sizeof t);
                        char wav[700];
                        snprintf(wav, sizeof wav, "%s/STAGE%u/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, room >> 12, room, id);
                        const int kl = (m[0] == 0x04 && m[1] == 0x00) ? kopf_laenge(m + 2, len - 2) : 0;
                        int sl = 0; const uint8_t *sc = NULL; sl = rdt_nachricht(id, &sc);
                        const int aus_rdt = sc && len <= sl && memcmp(sc, m, (size_t)len) == 0;
                        printf("        ROOM%04X msg %3d Seite %d/%d: %d Zeilen, %s, Stimme %s, %s  \"%s\"\n", room, id, s + 1, ns, z,
                               kl ? "mit Namen" : "ohne Namen", datei_da(wav) ? "ja" : "nein", aus_rdt ? "RDT" : "Port-Text", t);
                    }
                }
            }
    PRUEF(raeume > 100 && nachrichten > 1000, "%d Raeume, %d Nachrichten, %d Seiten gelesen", raeume, nachrichten, seiten_n);
    PRUEF(schlecht == 0, "%d Seiten mit mehr als 2 Zeilen (RDT-Texte und Port-Texte des Raumaufbaus)", schlecht);
}

/* ---- Stufe 5 Teil B: Port-Texte, die erst bei einem Ereignis eingesetzt werden (re15_msg_install_text in ~25 Modulen) ----------
 * Statt jedes Ereignis nachzufahren: jede Byte-Tabelle `{ 0x.., 0x.., ... }` in engine/src/<name>.c und engine/src/gen/<name>.inc
 * wird wie die Schreibmaschine FUN_80028134 in Nachrichten zerlegt (Ende 01 + Argument bzw. 03; 2-Byte-Codes 02/04/05/06/09/0A/0B;
 * 07 = 1 Byte; 08 Zeile) und jede Seite mit mehr als zwei Zeilen gemeldet, wenn sie Text ist (nur Glyphen aus msg_common.c
 * re15_msg_glyph, keine Zeile ueber 64 Glyphen, mindestens 8 Buchstaben und 60 % Buchstaben - sonst sind es Bild- oder
 * Programmdaten). AUSGENOMMEN: das Inventar
 * (CHECK-Beschreibungen, eigenes Feld ab y = 0xA0, menu_common.c msg_open(.., 0x13, 0xa0); die Bank DEBUG.BIN @0x800c50de der Mod
 * hat dort selbst Seiten mit drei Zeilen) = Tabellen mit "desc" im Namen und gen/inv_desc_bank.inc, dazu der Akten-Leser
 * gen/inv_file_doc.inc (eigene Seiten). */
#include <dirent.h>
#ifndef RE15_ENGINE_SRC_DIR
#error RE15_ENGINE_SRC_DIR fehlt
#endif

static int glyphe_text(uint8_t c)
{
    return c == 0x00 || (c >= 0x0C && c <= 0x57 && c != 0x17) || c == 0x61 || c == 0xF2;
}

/* Eine Nachricht m[0..n): Seiten mit mehr als zwei Text-Zeilen zaehlen und ausgeben. */
static int quelle_nachricht(const char *datei, const char *name, const uint8_t *m, int n)
{
    int schlecht = 0, zeilen = 0, glyph = 0, lang = 0, text = 1, zeile_len = 0, seite = 1, buchst = 0, zeichen = 0;
    char t[400];
    size_t o = 0;
    for (int i = 0; i <= n; i++) {
        const int ende = (i == n);
        const uint8_t c = ende ? 0x02 : m[i];
        if (c == 0x02 || c == 0x08) {
            if (glyph) zeilen++;
            if (zeile_len > 64) lang = 1;   /* 271 px / schmalste Glyphen; Bild-/Programmdaten haben viel laengere "Zeilen" */
            glyph = 0; zeile_len = 0;
            if (c == 0x08) {
                if (o + 3 < sizeof t) { t[o++] = ' '; t[o++] = '/'; t[o++] = ' '; }
                continue;
            }
            t[o] = 0;
            /* Text = mindestens 8 Buchstaben und 60 % der Zeichen Buchstaben (Bild-/Programmdaten liegen als Ziffern 0x0C..0x15) */
            if (zeilen > 2 && text && !lang && buchst >= 8 && buchst * 10 >= zeichen * 6) {
                schlecht++;
                printf("        %s %s Seite %d: %d Zeilen  \"%s\"\n", datei, name, seite, zeilen, t);
            }
            zeilen = 0; o = 0; lang = 0; text = 1; seite++; buchst = 0; zeichen = 0;
            i++;                              /* Argument von 02 */
            continue;
        }
        if (c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) { i++; continue; }
        if (c == 0x07) continue;
        if (!glyphe_text(c)) { text = 0; continue; }
        if (c != 0x00) { glyph = 1; zeichen++; if ((c >= 0x1D && c <= 0x36) || (c >= 0x3D && c <= 0x56)) buchst++; }
        zeile_len++;
        if (c == 0x00) { if (o + 1 < sizeof t) t[o++] = ' '; continue; }
        uint8_t q[2] = { c, 0x01 };
        char d[8];
        re15_msg_decode_text(q, 2, d, sizeof d);
        for (int k = 0; d[k] && o + 1 < sizeof t; k++) t[o++] = d[k];
    }
    return schlecht;
}

static int hexwert(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

static int quelle_datei(const char *pfad, const char *datei, int *tabellen, int *nachrichten)
{
    size_t n = 0;
    uint8_t *d = slurp(pfad, &n);
    if (!d) return 0;
    int schlecht = 0;
    static uint8_t b[65536];
    for (size_t i = 0; i < n; i++) {
        if (d[i] != '{') continue;
        size_t j = i + 1;
        int nb = 0, ok = 1;
        for (;;) {
            while (j < n && (d[j] == ' ' || d[j] == '\t' || d[j] == '\r' || d[j] == '\n' || d[j] == ',')) j++;
            /* Runde 2 (Pruefbericht s5_r1 R2): Kommentare in der Tabelle ueberspringen - `{   /* Woman: ... * /` (cut_10f0.c,
             * irons_tod_1150.c) liess die ganze Tabelle aus */
            if (j + 1 < n && d[j] == '/' && d[j + 1] == '*') {
                size_t e = j + 2;
                while (e + 1 < n && !(d[e] == '*' && d[e + 1] == '/')) e++;
                j = e + 2;
                continue;
            }
            if (j + 1 < n && d[j] == '/' && d[j + 1] == '/') { while (j < n && d[j] != '\n') j++; continue; }
            if (j >= n) { ok = 0; break; }
            if (d[j] == '}') break;
            if (j + 2 < n && d[j] == '0' && (d[j + 1] == 'x' || d[j + 1] == 'X')) {
                unsigned v = 0;
                size_t k = j + 2;
                int ziff = 0;
                for (; k < n && ziff < 3; k++, ziff++) {
                    const int h = hexwert((char)d[k]);
                    if (h < 0) break;
                    v = v * 16 + (unsigned)h;
                }
                if (ziff < 1 || ziff > 2 || nb >= (int)sizeof b) { ok = 0; break; }
                b[nb++] = (uint8_t)v;
                j = k;
                continue;
            }
            ok = 0;
            break;
        }
        if (!ok || nb < 8) continue;
        /* Name der Tabelle: das Wort vor dem '[' rueckwaerts ab dem '{' */
        char name[96] = "?";
        {
            size_t a = i;
            while (a > 0 && d[a] != '[' && d[a] != ';' && d[a] != '}' && i - a < 200) a--;
            if (d[a] == '[') {
                size_t e = a;
                while (e > 0 && d[e - 1] == ' ') e--;
                size_t s = e;
                while (s > 0 && (isalnum(d[s - 1]) || d[s - 1] == '_')) s--;
                size_t l = e - s;
                if (l >= sizeof name) l = sizeof name - 1;
                memcpy(name, d + s, l);
                name[l] = 0;
            }
        }
        (*tabellen)++;
        if (strstr(name, "desc")) { i = j; continue; }   /* Inventar-Beschreibung (eigenes Feld) */
        int a = 0;
        for (int k = 0; k < nb;) {
            const uint8_t c = b[k];
            if (c == 0x01 || c == 0x03) {
                (*nachrichten)++;
                schlecht += quelle_nachricht(datei, name, b + a, k - a);
                k += (c == 0x01) ? 2 : 1;
                a = k;
                continue;
            }
            if (c == 0x02 || c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) { k += 2; continue; }
            k++;
        }
        if (a < nb) schlecht += quelle_nachricht(datei, name, b + a, nb - a);
        i = j;
    }
    free(d);
    return schlecht;
}

static void teil_quellen_zeilen(void)
{
    printf("=== quellen_zeilen: Port-Texte in engine/src (.c, gen/.inc) - je Seite hoechstens 2 Zeilen (ohne Inventar) ===\n");
    static const char *ordner[2] = { RE15_ENGINE_SRC_DIR, RE15_ENGINE_SRC_DIR "/gen" };
    int dateien = 0, tabellen = 0, nachrichten = 0, schlecht = 0;
    for (int o = 0; o < 2; o++) {
        DIR *dir = opendir(ordner[o]);
        if (!dir) { PRUEF(0, "Ordner %s lesbar", ordner[o]); continue; }
        struct dirent *e;
        while ((e = readdir(dir)) != NULL) {
            const char *nm = e->d_name;
            const size_t l = strlen(nm);
            const int c = l > 2 && !strcmp(nm + l - 2, ".c"), inc = l > 4 && !strcmp(nm + l - 4, ".inc");
            if (!c && !inc) continue;
            if (!strcmp(nm, "inv_desc_bank.inc") || !strcmp(nm, "inv_file_doc.inc")) continue;   /* Inventar / Akten-Leser */
            char pfad[700];
            snprintf(pfad, sizeof pfad, "%s/%s", ordner[o], nm);
            dateien++;
            schlecht += quelle_datei(pfad, nm, &tabellen, &nachrichten);
        }
        closedir(dir);
    }
    PRUEF(dateien > 300 && tabellen > 400 && nachrichten > 200, "%d Dateien, %d Byte-Tabellen, %d Nachrichten gelesen", dateien,
          tabellen, nachrichten);
    PRUEF(schlecht == 0, "%d Seiten mit mehr als 2 Zeilen in Port-Texten", schlecht);
}

/* ---- Stufe 5 Runde 2 (Pruefbericht s5_r1 R1/R3): Seiten so zaehlen, wie der Raum sie DARSTELLT; ohne Stimme geht keine Seite verloren
 * Beide Teile fahren die echte VM: ein Mini-Skript `2b <id> <Maske>` (+ Sleeps) laeuft im Faden 1, je Bild erst re15_msg_tick (die
 * Darstellung, die platform/pc/main.c ~6233 zeichnet), dann scd_vm_tick (~8213). Damit entscheidet op_message_on selbst ueber
 * Full-Text (ROOM1170/1240/1241, re15_room_full_text) oder Schreibmaschine - genau der Weg, den Runde 1 nicht mitgezaehlt hat. */
static uint8_t s_prog[64];

typedef struct { int zeilen_y[8]; int nz; int max_px; } r2_mass_t;

static void r2_glyph(int penx, int peny, unsigned char g, int attr, void *ctx)
{
    r2_mass_t *m = (r2_mass_t *)ctx;
    (void)attr;
    int k;
    for (k = 0; k < m->nz; k++) if (m->zeilen_y[k] == peny) break;
    if (k == m->nz && m->nz < 8) m->zeilen_y[m->nz++] = peny;
    const int rechts = penx + re15_font_width[g] - 34;   /* Stift-x 0x22 @0x80027eec */
    if (rechts > m->max_px) m->max_px = rechts;
}
static int r2_breite(unsigned char g, void *ctx) { (void)ctx; return re15_font_width[g]; }

static int r2_lebt(void)
{
    return g_scd.message_fsm_active ? (int)g_scd.message_active
                                    : (g_scd.message_display_frames > 0 || g_scd.message_nachhall > 0);
}

static void r2_ruhe(void)
{
    g_scd.message_active = 0; g_scd.message_fsm_active = 0; g_scd.message_display_frames = 0; g_scd.message_nachhall = 0;
    g_scd.message_query = 0; g_scd.message_select = 0;
}

static void teil_darstellung(void)
{
    printf("=== darstellung: jede Nachricht jedes Raums ueber Message_on gezeigt (Full-Text oder Schreibmaschine wie im Raum) - je\n"
           "    gezeigtem Fenster hoechstens 2 Zeilen, keine Zeile ueber den Bildrand (34 + Breite <= 320) ===\n");
    int raeume = 0, nachrichten = 0, fenster = 0, schlecht = 0, volltext = 0;
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned rr = 0; rr < 0x40; rr++)
            for (unsigned v = 0; v < 2; v++) {
                const unsigned room = (st << 12) | (rr << 4) | v;
                if (raum_laden(room) != 0 || !s_rdt.messages) continue;
                scd_vm_init(); re15_aot_init();
                raum_laden(room);
                raeume++;
                const int ft = re15_room_full_text(room);
                for (int id = 0; id < 256; id++) {
                    int len = 0;
                    const unsigned char *m = re15_msg_get_raw(id, &len);
                    if (!m || len < 2) continue;
                    nachrichten++;
                    if (ft) volltext++;
                    r2_ruhe();
                    const uint8_t prog[6] = { 0x2B, (uint8_t)id, 0x00, 0x00, 0x01, 0x00 };
                    memcpy(s_prog, prog, sizeof prog);
                    if (scd_thread_start(1, s_prog) != 0) { PRUEF(0, "ROOM%04X msg %d: Faden frei", room, id); continue; }
                    int max_z = 0, max_px = 0, n_f = 0;
                    for (int bild = 1; bild < 4000; bild++) {
                        g_scd_pad_held = 0;
                        g_scd_pad_edge = (g_scd.message_fsm_active && (g_scd.message_fsm == 1 || g_scd.message_fsm == 4
                                          || g_scd.message_select)) ? 0x4000 : 0;   /* Taste an Seiten-/End-/Frage-Warten */
                        const unsigned char *r = NULL; int l = 0, mi = 0;
                        if (re15_msg_tick(&r, &l, &mi) && r && l > 0 && mi == id) {
                            r2_mass_t ms; memset(&ms, 0, sizeof ms);
                            re15_msg_layout(34, 180, r, l, r2_glyph, r2_breite, &ms);
                            if (ms.nz > max_z) max_z = ms.nz;
                            if (ms.max_px > max_px) max_px = ms.max_px;
                            n_f++;
                        }
                        scd_vm_tick();
                        if (bild > 2 && !g_scd.threads[1].active && !r2_lebt()) break;
                    }
                    g_scd.threads[1].active = 0;
                    g_scd_pad_edge = 0;
                    fenster += n_f;
                    if (max_z <= 2 && 34 + max_px <= 320) continue;
                    schlecht++;
                    char txt[600]; re15_msg_decode_text(m, (size_t)len, txt, sizeof txt);
                    printf("        ROOM%04X msg %3d (%s): %d Zeilen, breiteste %d px  \"%.110s\"\n", room, id,
                           ft ? "Full-Text" : "Schreibmaschine", max_z, max_px, txt);
                }
            }
    PRUEF(raeume > 100 && nachrichten > 1000 && volltext > 10, "%d Raeume, %d Nachrichten (%d im Full-Text-Raum), %d gezeigte Fenster",
          raeume, nachrichten, volltext, fenster);
    PRUEF(schlecht == 0, "%d Nachrichten mit einem gezeigten Fenster von mehr als 2 Zeilen oder ueber den Bildrand", schlecht);
}

/* ohne_stimme: jede Message_on X der RDT-Skripte (main + subs), deren Nachricht (wie im Spiel geladen) einen zeitgesteuerten
 * Seitenwechsel `02 N` (N > 0) hat und deren naechstes Message_on Y im selben Skript ohne Verzweigung erreicht wird: Mini-Skript
 * `2b X` + Sleep-Summe + `2b Y` ohne Stimme (g_re15_voice_laeuft = 0 wie bei VOICE OFF; ctest setzt dazu RE15_VOICE=0). Jede Seite
 * von X muss erscheinen, und X muss von selbst enden (Schwanz `01 N`), bevor Y kommt - Y darf X nicht abschneiden.
 * Pflichtfaelle aus dem Pruefbericht s5_r1 R3: 30E1 msg 14 (Elza), 5011 msg 3 (Elza), 11B0 msg 5 (dazu 11B0 msg 8, 4010 msg 8). */
static int r2_verzweigt(uint8_t op)
{
    return (op >= 0x01 && op <= 0x08) || op == 0x0B || op == 0x0C || (op >= 0x0D && op <= 0x1C) || op == 0x21 || op == 0x23;
}

static int r2_nachlauf(const unsigned char *m, int len)
{
    for (int i = len - 2; i >= 0; i--) if (m[i] == 0x01) return m[i + 1];
    return 0;
}

static int r2_zeitseiten(const unsigned char *m, int len, int *seiten_n)
{
    int ja = 0, k = 1;
    for (int i = 0; i < len; ) {
        const unsigned char b = m[i];
        if (b == 0x01 || b == 0x03) break;
        if (b == 0x02) { if (i + 1 < len && m[i + 1] == 0) return 0; ja = 1; k++; i += 2; continue; }
        if (b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }
        i++;
    }
    *seiten_n = k;
    return ja;
}

static void teil_ohne_stimme(void)
{
    extern int g_re15_voice_laeuft, g_re15_voice_restbilder;
    printf("=== ohne_stimme (RE15_VOICE=%s): zweiseitige Zeilen der RDT-Skripte - keine Seite geht verloren ===\n",
           getenv("RE15_VOICE") ? getenv("RE15_VOICE") : "-");
    static const struct { unsigned raum; int id; } pflicht[] = {
        { 0x30E1, 14 }, { 0x5011, 3 }, { 0x11B0, 5 }, { 0x11B0, 8 }, { 0x4010, 8 } };
    int pflicht_ok[5] = { 0 };
    int faelle = 0, schlecht = 0, unbekannt = 0, verzoegert = 0;
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned rr = 0; rr < 0x40; rr++)
            for (unsigned v = 0; v < 2; v++) {
                const unsigned room = (st << 12) | (rr << 4) | v;
                if (raum_laden(room) != 0 || !s_rdt.messages) continue;
                scd_vm_init(); re15_aot_init();
                raum_laden(room);
                const uint8_t *skripte[1 + RE15_RDT_MAX_SUB_SCD];
                int ns = 0;
                if (s_rdt.main_scd) skripte[ns++] = s_rdt.main_scd;
                for (int k = 0; k < s_rdt.sub_scd_count && k < RE15_RDT_MAX_SUB_SCD; k++) if (s_rdt.sub_scd[k]) skripte[ns++] = s_rdt.sub_scd[k];
                for (int k = 0; k < ns; k++) {
                    const uint8_t *p = skripte[k];
                    const int grenze = (int)(s_raw + s_rawsz - p) - 8;
                    for (int pc = 0; pc < 0x4000 && pc < grenze;) {
                        const int sz = scd_opcode_size_at(p + pc);
                        if (sz <= 0 || p[pc] == 0x01) break;
                        if (p[pc] != 0x2B) { pc += sz; continue; }
                        const int x = p[pc + 1];
                        int len = 0, sn = 1;
                        const unsigned char *m = re15_msg_get_raw(x, &len);
                        if (!m || !r2_zeitseiten(m, len, &sn)) { pc += sz; continue; }
                        /* bis zum naechsten Message_on: Sleeps summieren, an Verzweigungen aufgeben */
                        int q = pc + sz, schlaf = 0, y = -1;
                        while (q < 0x4000 && q < grenze) {
                            const int s2 = scd_opcode_size_at(p + q);
                            if (s2 <= 0 || r2_verzweigt(p[q])) break;
                            if (p[q] == 0x2B) { y = q; break; }
                            if (p[q] == 0x09) schlaf += p[q + 2] | (p[q + 3] << 8);
                            q += s2;
                        }
                        const unsigned file_off = (unsigned)(p + pc - s_raw);
                        if (y < 0) {
                            unbekannt++;
                            printf("        (ROOM%04X msg %d @0x%05X: naechstes Message_on nicht ohne Verzweigung erreichbar - nicht gezaehlt)\n",
                                   room, x, file_off);
                            pc += sz; continue;
                        }
                        int o = 0;
                        memcpy(s_prog + o, p + pc, 4); o += 4;
                        for (int rest = schlaf; rest > 0; ) {
                            const int c = rest > 0x7fff ? 0x7fff : rest;
                            s_prog[o++] = 0x09; s_prog[o++] = 0x0A; s_prog[o++] = (uint8_t)c; s_prog[o++] = (uint8_t)(c >> 8);
                            rest -= c;
                        }
                        memcpy(s_prog + o, p + y, 4); o += 4;
                        s_prog[o++] = 0x01; s_prog[o++] = 0x00;
                        r2_ruhe();
                        g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
                        scd_thread_start(1, s_prog);
                        int seite_sicht[16] = { 0 }, y_bild = -1, abgeschnitten = 0;
                        const int yid = p[y + 1];
                        for (int bild = 1; bild < 3000; bild++) {
                            g_scd_pad_edge = 0; g_scd_pad_held = 0;
                            const unsigned char *r = NULL; int l = 0, mi = 0;
                            if (re15_msg_tick(&r, &l, &mi) && r && l > 0 && mi == x) {
                                int si = 0;   /* Seite = Zahl der `02` vor dem Fensteranfang */
                                for (int i = 0; i < (int)(r - m) && i < len; ) {
                                    const unsigned char b = m[i];
                                    if (b == 0x02) { si++; i += 2; continue; }
                                    if (b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }
                                    i++;
                                }
                                if (si < 16) seite_sicht[si]++;
                            }
                            const int x_lebt = (g_scd.message_id == x) && r2_lebt();
                            scd_vm_tick();
                            if (yid != x && g_scd.message_id == yid && r2_lebt()) { y_bild = bild; abgeschnitten = x_lebt; break; }
                            if (yid == x && !g_scd.threads[1].active) { y_bild = bild; break; }
                            if (!g_scd.threads[1].active) break;
                        }
                        g_scd.threads[1].active = 0;
                        const int letzte = (sn - 1 < 16) ? seite_sicht[sn - 1] : 0;
                        const int nl = r2_nachlauf(m, len);
                        int alle = 1;
                        for (int s = 0; s < sn && s < 16; s++) if (seite_sicht[s] <= 0) alle = 0;
                        const int ok = alle && !abgeschnitten;   /* jede Seite gesehen, X endete selbst, bevor Y kam */
                        faelle++;
                        if (!ok) schlecht++;
                        if (y_bild > schlaf + 2) verzoegert++;
                        for (int k2 = 0; k2 < 5; k2++) if (pflicht[k2].raum == room && pflicht[k2].id == x) pflicht_ok[k2] = ok ? 1 : -1;
                        printf("    %s ROOM%04X msg %3d @0x%05X: %d Seiten, Seite 1 %3d / letzte %3d Bilder (Nachlauf %3d)%s, naechste Zeile "
                               "msg %d nach %3d Bildern (Sleep-Summe %d)\n", ok ? "ok  " : "FAIL", room, x, file_off, sn, seite_sicht[0], letzte,
                               nl, abgeschnitten ? " ABGESCHNITTEN" : "", yid, y_bild, schlaf);
                        pc += sz;
                    }
                }
            }
    PRUEF(faelle >= 20, "%d zweiseitige Zeilen mit Nachfolger ohne Verzweigung gemessen (%d andere nicht gezaehlt), %d davon warten jetzt "
          "laenger als die Sleep-Summe", faelle, unbekannt, verzoegert);
    for (int k = 0; k < 5; k++)
        PRUEF(pflicht_ok[k] == 1, "Pflichtfall ROOM%04X msg %d: jede Seite sichtbar, Zeile endet selbst vor der naechsten (%s)", pflicht[k].raum, pflicht[k].id,
              pflicht_ok[k] == 0 ? "NICHT GEFUNDEN" : (pflicht_ok[k] > 0 ? "ja" : "NEIN"));
    PRUEF(schlecht == 0, "%d zweiseitige Zeilen verlieren ohne Stimme eine Seite oder werden von der naechsten Zeile abgeschnitten", schlecht);
}

/* ---- Stufe 6: veraltete Aufnahmen (alter Satz unter neuem Untertitel) -------------------------------------------------------- */
#include "re15_altaufnahme_r42.h"
#include "re15_stimmen_r39.h"
#include "re15_elza_r36.h"
#include "re15_elzaintro_r38.h"
#include "re15_rekorder_r38.h"
extern void re15_dining_r39_stamm(unsigned, int, char *, size_t);
extern void re15_johnruf_r39_stimme(unsigned, int, char *, size_t, char *, size_t);

#ifndef RE15_R42_ALT_BESTAND
#error RE15_R42_ALT_BESTAND fehlt
#endif

/* Stufe 7: alle 15 Saetze mit neuem Untertitel (erzeugt, tools/r42_namen/altaufnahmen_bauen.py) */
#define RE15_ALTSATZ_ZEILEN 1
static const struct { unsigned raum; int nr; int stumm; uint32_t groesse; uint64_t fnv; const char *gehoert; const char *neu; }
k_altsatz[] = {
#include "gen_r42_namen_altsaetze.inc"
};
#define ALTSATZ_N ((int)(sizeof k_altsatz / sizeof k_altsatz[0]))

/* Woerter eines Satzes: klein, Buchstaben/Ziffern, Apostroph im Wort; ein Wort direkt vor ':' ist der Sprechername und zaehlt nicht
 * (Namenskopf jeder Seite, re15_msg_decode_text haengt die Seiten aneinander). */
static int satz_woerter(const char *t, char w[][24], int max)
{
    int n = 0;
    for (const char *p = t; *p && n < max;) {
        if (!isalnum((unsigned char)*p)) { p++; continue; }
        int k = 0;
        while (isalnum((unsigned char)*p) || (*p == '\'' && isalnum((unsigned char)p[1]) && k > 0)) {
            if (k < 23) w[n][k++] = (char)tolower((unsigned char)*p);
            p++;
        }
        w[n][k] = 0;
        if (*p == ':') continue;          /* Sprechername */
        n++;
    }
    return n;
}
/* laengste gemeinsame Wortfolge */
static int wort_lcs(char a[][24], int na, char u[][24], int nu)
{
    static int d[65][65];
    memset(d, 0, sizeof d);
    for (int i = 0; i < na; i++)
        for (int j = 0; j < nu; j++)
            d[i + 1][j + 1] = !strcmp(a[i], u[j]) ? d[i][j] + 1 : (d[i][j + 1] > d[i + 1][j] ? d[i][j + 1] : d[i + 1][j]);
    return d[na][nu];
}

#define N_STUMM 13
static void teil_altaufnahme(void)
{
    printf("=== altaufnahme: 13 veraltete Aufnahmen (anderer Satz) gelten wie fehlende Dateien, keine andere Datei ist betroffen ===\n");
    static const struct { unsigned raum; int nr; } soll[N_STUMM] = {
        { 0x3020, 0 }, { 0x3020, 1 },
        { 0x30E0, 3 }, { 0x30E0, 10 }, { 0x30E0, 11 }, { 0x30E0, 13 },
        { 0x6030, 2 }, { 0x6030, 3 }, { 0x6030, 4 }, { 0x6030, 5 }, { 0x6030, 22 },
        { 0x6031, 1 }, { 0x6031, 2 },
    };
    /* Stufe 7: fast woertlich -> NICHT in der Tabelle, spielt (Entscheid 19.11.2026) */
    static const struct { unsigned raum; int nr; } spielt[2] = { { 0x3020, 2 }, { 0x6030, 21 } };
    /* (1) Tabelle = genau die 13 Saetze, Pfad nach der Ablage-Regel */
    const int n = re15_altaufnahme_r42_anzahl();
    PRUEF(n == N_STUMM, "Tabelle: %d Eintraege (Soll %d)", n, N_STUMM);
    int tab_ok = 0;
    for (int i = 0; i < N_STUMM && i < n; i++) {
        const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(i);
        char regel[80];
        snprintf(regel, sizeof regel, "synchro/stage%u/room%04x/main%02d.wav", soll[i].raum >> 12, soll[i].raum, soll[i].nr);
        if (e && e->raum == soll[i].raum && e->datei == soll[i].nr && !strcmp(e->pfad, regel) && e->groesse > 44) tab_ok++;
        else printf("        Eintrag %d: ROOM%04X main%02d \"%s\" passt nicht zum Soll ROOM%04X main%02d\n", i, e ? e->raum : 0,
                    e ? e->datei : -1, e ? e->pfad : "?", soll[i].raum, soll[i].nr);
    }
    PRUEF(tab_ok == N_STUMM, "Tabelle: %d von %d Eintraegen = Soll-Satz mit Regelpfad synchro/STAGEn/roomXXXX/mainNN.wav", tab_ok, N_STUMM);
    int spielt_frei = 0;
    for (int k = 0; k < 2; k++) {
        int drin = 0;
        for (int i = 0; i < n; i++) {
            const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(i);
            if (e->raum == spielt[k].raum && e->datei == spielt[k].nr) drin = 1;
        }
        spielt_frei += !drin;
    }
    PRUEF(spielt_frei == 2, "Stufe 7: 3020 main02 und 6030 main21 (fast woertlich) stehen NICHT in der Tabelle (%d von 2)", spielt_frei);

    /* (2) der Untertitel im Spiel ist neu: nicht mehr der RDT-Satz; Port-Zeilen 6030 21/22 = Story-Text mit Namenskopf.
     * (7) Stufe 7, REGEL: Untertitel aus dem geladenen Raum Wort fuer Wort gegen die Aufnahme (gehoert). In der Tabelle <=> die
     *     Aufnahme spricht einen ANDEREN Satz: mehr als RE15_ALTSATZ_UNTERTITEL_FREMD Woerter des Untertitels nicht gesprochen
     *     oder mehr als RE15_ALTSATZ_AUFNAHME_FREMD gesprochene Woerter nicht im Untertitel. */
    PRUEF(ALTSATZ_N == 15, "Satzliste: %d Saetze mit neuem Untertitel (Soll 15 = %d stumm + 2 fast woertlich)", ALTSATZ_N, N_STUMM);
    int neu_text = 0, wortlaut_ok = 0, regel_ok = 0, n_stumm = 0, n_spielt = 0;
    for (int i = 0; i < ALTSATZ_N; i++) {
        const unsigned raum = k_altsatz[i].raum; const int id = k_altsatz[i].nr;
        if (raum_laden(raum) != 0) { PRUEF(0, "ROOM%04X ladbar", raum); continue; }
        int len = 0;
        const unsigned char *m = re15_msg_get_raw(id, &len);
        char jetzt[512] = "", rdt[512] = "";
        if (m && len > 0) re15_msg_decode_text(m, (size_t)len, jetzt, sizeof jetzt);
        const uint8_t *r = NULL;
        const int rl = rdt_nachricht(id, &r);
        if (rl > 0) re15_msg_decode_text(r, (size_t)rl, rdt, sizeof rdt);
        const int ok = jetzt[0] && strchr(jetzt, ':') && (rl <= 0 || strcmp(jetzt, rdt) != 0);
        neu_text += ok;
        /* Wortlaut: Untertitel im Spiel == "neu" der Satzliste (ohne Sprechernamen) */
        static char wu[64][24], wn[64][24], wa[64][24];
        const int nu = satz_woerter(jetzt, wu, 64), nn = satz_woerter(k_altsatz[i].neu, wn, 64);
        const int na = satz_woerter(k_altsatz[i].gehoert, wa, 64);
        int gleich = nu == nn;
        for (int q = 0; gleich && q < nu; q++) gleich = !strcmp(wu[q], wn[q]);
        wortlaut_ok += gleich;
        const int l = wort_lcs(wa, na, wu, nu);
        const int u_fremd = nu - l, a_fremd = na - l;
        const int fast = u_fremd <= RE15_ALTSATZ_UNTERTITEL_FREMD && a_fremd <= RE15_ALTSATZ_AUFNAHME_FREMD;
        int in_tab = 0;
        for (int q = 0; q < n; q++) {
            const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(q);
            if (e->raum == raum && e->datei == id) in_tab = 1;
        }
        const int rok = (in_tab == !fast) && (in_tab == k_altsatz[i].stumm);
        regel_ok += rok;
        n_stumm += in_tab; n_spielt += !in_tab;
        printf("        ROOM%04X msg %2d  jetzt   \"%s\"\n                         %s \"%s\"%s\n"
               "                         gehoert \"%s\"\n"
               "                         -> %d von %d Woertern des Untertitels nicht gesprochen, %d von %d gesprochenen nicht im "
               "Untertitel: %s, %s%s%s\n", raum, id, jetzt,
               rl > 0 ? "RDT    " : "RDT     (keine: Port-Zeile)", rdt, ok ? "" : "   <- NICHT NEU", k_altsatz[i].gehoert,
               u_fremd, nu, a_fremd, na, fast ? "FAST WOERTLICH" : "anderer Satz", in_tab ? "in der Tabelle (stumm)" : "nicht in der Tabelle (spielt)",
               rok ? "" : "   <- REGEL VERLETZT", gleich ? "" : "   <- Satzliste weicht vom Untertitel ab");
    }
    PRUEF(neu_text == 15, "Untertitel im Spiel sind neu (nicht der RDT-Satz der Aufnahme): %d von 15", neu_text);
    PRUEF(wortlaut_ok == 15, "Satzliste des Werkzeugs = Untertitel im geladenen Raum, Wort fuer Wort: %d von 15", wortlaut_ok);
    PRUEF(regel_ok == 15, "REGEL Stufe 7: stumm nur, was einen ANDEREN Satz spricht (hoechstens %d Untertitelwort nicht gesprochen und "
          "hoechstens %d gesprochene Woerter nicht im Untertitel -> spielt): %d von 15 richtig eingeordnet", RE15_ALTSATZ_UNTERTITEL_FREMD,
          RE15_ALTSATZ_AUFNAHME_FREMD, regel_ok);
    PRUEF(n_stumm == N_STUMM && n_spielt == 2, "von den 15 Saetzen: %d stumm (Soll %d), %d spielen (Soll 2)", n_stumm, N_STUMM, n_spielt);

    /* (3)+(4) ganzer Bestand: alt gilt nur fuer die 13 Pfade mit dem Abdruck des Tags; keine andere Datei, kein Doppel */
    FILE *lf = fopen(RE15_R42_ALT_BESTAND, "r");
    PRUEF(lf != NULL, "Bestandsliste lesbar (%s)", RE15_R42_ALT_BESTAND);
    if (!lf) return;
    int dateien = 0, alt_noch = 0, alt_ersetzt = 0, fremd_alt = 0, doppel = 0, gesehen[N_STUMM] = { 0 }, unlesbar = 0;
    int fastw_da = 0, fastw_abdruck = 0, fastw_alt = 0;    /* Stufe 7: 3020 main02 / 6030 main21 */
    uint8_t *muster = NULL; size_t muster_n = 0; char muster_pfad[200] = "";
    char zeile[700];
    while (fgets(zeile, sizeof zeile, lf)) {
        size_t zl = strlen(zeile);
        while (zl && (zeile[zl - 1] == '\n' || zeile[zl - 1] == '\r')) zeile[--zl] = 0;
        if (!zl) continue;
        char voll[1200];
        snprintf(voll, sizeof voll, "%s/%s", RE15_SYNCHRO_DIR, zeile);          /* Liste relativ zu synchro/ */
        char rel[760], klein[760];
        snprintf(rel, sizeof rel, "synchro/%s", zeile);
        size_t q = 0;
        for (; rel[q] && q + 1 < sizeof klein; q++) klein[q] = (char)tolower((unsigned char)rel[q]);
        klein[q] = 0;
        size_t bn = 0;
        uint8_t *b = slurp(voll, &bn);
        if (!b) { unlesbar++; continue; }
        dateien++;
        const int ist = re15_altaufnahme_r42_ist(rel, b, bn);
        const uint64_t h = re15_altaufnahme_r42_summe(b, bn);
        int k = -1;
        for (int i = 0; i < n; i++) {
            const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(i);
            if (!strcmp(klein, e->pfad)) k = i;
            else if (e->groesse == bn && e->fnv == h) { doppel++; printf("        DOPPEL: %s hat den Inhalt von %s\n", rel, e->pfad); }
        }
        if (k < 0) {
            if (ist) { fremd_alt++; printf("        FREMD: %s gilt als alt\n", rel); }
            /* Stufe 7: die zwei fast woertlichen Aufnahmen - Datei mit dem Abdruck des Tags gilt NICHT als alt */
            for (int q = 0; q < ALTSATZ_N; q++) {
                char regel[80];
                snprintf(regel, sizeof regel, "synchro/stage%u/room%04x/main%02d.wav", k_altsatz[q].raum >> 12, k_altsatz[q].raum, k_altsatz[q].nr);
                if (k_altsatz[q].stumm || strcmp(klein, regel)) continue;
                const int abdruck = (k_altsatz[q].groesse == bn && k_altsatz[q].fnv == h);
                fastw_da++; fastw_abdruck += abdruck; fastw_alt += ist;
                printf("        FAST WOERTLICH: %s (%u Byte, %s) gilt als alt: %d -> %s\n", rel, (unsigned)bn,
                       abdruck ? "Abdruck des Tags v0.8.30" : "ersetzt", ist, ist ? "STUMM" : "spielt");
            }
        } else {
            const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(k);
            gesehen[k] = 1;
            const int abdruck = (e->groesse == bn && e->fnv == h);
            if (abdruck != ist) { fremd_alt++; printf("        WIDERSPRUCH: %s Abdruck %d, gilt als alt %d\n", rel, abdruck, ist); }
            if (ist) {
                alt_noch++;
                if (!muster) { muster = b; muster_n = bn; snprintf(muster_pfad, sizeof muster_pfad, "%s", rel); b = NULL; }
            } else {
                alt_ersetzt++;
                printf("        ERSETZT: %s (%u Byte) traegt nicht mehr den Abdruck des Tags -> spielt\n", rel, (unsigned)bn);
            }
        }
        free(b);
    }
    fclose(lf);
    int da = 0;
    for (int i = 0; i < N_STUMM; i++) da += gesehen[i];
    printf("    Bestand: %d Dateien gelesen (%d unlesbar); von den %d: %d noch alt (stumm), %d ersetzt (spielen), %d fehlen\n", dateien,
           unlesbar, N_STUMM, alt_noch, alt_ersetzt, N_STUMM - da);
    PRUEF(fastw_alt == 0, "Stufe 7: 3020 main02 / 6030 main21 gelten NICHT als alt -> spielen (%d von %d vorhandenen gelten als alt; "
          "%d tragen noch den Abdruck des Tags)", fastw_alt, fastw_da, fastw_abdruck);
    PRUEF(dateien >= 400 && unlesbar == 0, "Bestand synchro/ gelesen: %d Dateien", dateien);
    PRUEF(fremd_alt == 0, "NEGATIVKONTROLLE: keine der uebrigen %d Dateien gilt als alt (%d Verstoesse)", dateien - da, fremd_alt);
    PRUEF(doppel == 0, "keine andere Datei traegt den Inhalt einer der 13 alten Aufnahmen (Spiegelraum-Kopien: %d)", doppel);
    PRUEF(alt_noch + alt_ersetzt == da, "jede vorhandene der 13 Dateien ist entweder alt (Abdruck) oder ersetzt: %d + %d = %d", alt_noch,
          alt_ersetzt, da);

    /* (5) Abdruck ist scharf: ein Byte anders / mehr / weniger, anderer Name -> nicht alt; Schreibweise des Pfads egal */
    if (muster) {
        uint8_t *k = (uint8_t *)malloc(muster_n + 1);
        memcpy(k, muster, muster_n); k[muster_n] = 0;
        PRUEF(re15_altaufnahme_r42_ist(muster_pfad, k, muster_n) == 1, "%s unveraendert: alt", muster_pfad);
        k[muster_n - 1] ^= 1;
        PRUEF(re15_altaufnahme_r42_ist(muster_pfad, k, muster_n) == 0, "letztes Byte um 1 Bit geaendert: NICHT alt -> spielt");
        k[muster_n - 1] ^= 1; k[44] ^= 0x80;
        PRUEF(re15_altaufnahme_r42_ist(muster_pfad, k, muster_n) == 0, "Byte 44 geaendert: NICHT alt");
        k[44] ^= 0x80;
        PRUEF(re15_altaufnahme_r42_ist(muster_pfad, k, muster_n + 1) == 0, "ein Byte angehaengt: NICHT alt");
        PRUEF(re15_altaufnahme_r42_ist(muster_pfad, k, muster_n - 1) == 0, "ein Byte kuerzer: NICHT alt");
        PRUEF(re15_altaufnahme_r42_ist("synchro/STAGE6/room6030/main06.wav", k, muster_n) == 0,
              "derselbe Inhalt unter anderem Namen (room6030/main06.wav): NICHT alt");
        PRUEF(re15_altaufnahme_r42_ist("synchro/unused/main00.wav", k, muster_n) == 0, "derselbe Inhalt in synchro/unused: NICHT alt");
        char gross[200], rueck[200]; size_t q = 0;
        for (; muster_pfad[q]; q++) {
            gross[q] = (char)toupper((unsigned char)muster_pfad[q]);
            rueck[q] = muster_pfad[q] == '/' ? '\\' : muster_pfad[q];
        }
        gross[q] = 0; rueck[q] = 0;
        PRUEF(re15_altaufnahme_r42_ist(gross, k, muster_n) == 1 && re15_altaufnahme_r42_ist(rueck, k, muster_n) == 1,
              "Pfad gross geschrieben / mit Rueckstrich: weiter alt (audio_pc.c probiert room30E0 und room30e0)");
        PRUEF(re15_altaufnahme_r42_ist(NULL, k, muster_n) == 0 && re15_altaufnahme_r42_ist(muster_pfad, NULL, 0) == 0,
              "leere Eingaben: nicht alt");
        free(k); free(muster);
    } else {
        printf("    (alle 13 Dateien sind schon ersetzt - der Abdruck-Teil (5) hat kein Muster mehr und entfaellt)\n");
    }

    /* (6) keine Umleitung fuehrt auf diese Saetze oder von ihnen weg */
    int uml = 0;
    for (int i = 0; i < ALTSATZ_N; i++) {      /* alle 15 Saetze, auch die zwei fast woertlichen */
        const unsigned room = k_altsatz[i].raum; const int id = k_altsatz[i].nr;
        char dir[64], stamm[24], dir0[64], stamm0[24];
        snprintf(dir, sizeof dir, "synchro/STAGE%u/room%04X/", room >> 12, room); snprintf(stamm, sizeof stamm, "main%02d", id);
        snprintf(dir0, sizeof dir0, "%s", dir); snprintf(stamm0, sizeof stamm0, "%s", stamm);
        if (re15_elza_stimme_datei(room, id)) uml++;
        if (re15_stimmen_r39_stamm(room, id, stamm, sizeof stamm) != 0) uml++;
        re15_rekorder_r38_stimme(room, id, dir, sizeof dir, stamm, sizeof stamm);
        re15_dining_r39_stamm(room, id, stamm, sizeof stamm);
        re15_johnruf_r39_stimme(room, id, dir, sizeof dir, stamm, sizeof stamm);
        if (strcmp(dir, dir0) || strcmp(stamm, stamm0)) uml++;
        if (re15_elzaintro_r38_stimme_raum(room, id) != room) uml++;
    }
    for (int i = 0; i < re15_stimmen_r39_anzahl(); i++) {
        const unsigned r = re15_stimmen_r39_eintrag(i)->raum;
        if (r == 0x3020 || r == 0x30E0 || r == 0x6030 || r == 0x6031) uml++;
    }
    /* fremde Raeume, die per Raum-Umleitung in einen der vier Ordner lesen koennten (Elza-Montage r38, Hilferufe r39) */
    for (unsigned room = 0x1000; room <= 0x6FFF; room++) {
        if ((room & 0xF) > 1) continue;
        for (int id = 0; id < 64; id++) {
            const unsigned q = re15_elzaintro_r38_stimme_raum(room, id);
            if (q != room && (q == 0x3020 || q == 0x30E0 || q == 0x6030 || q == 0x6031)) uml++;
            char dir[64], stamm[24];
            snprintf(dir, sizeof dir, "synchro/STAGE%u/room%04X/", room >> 12, room); snprintf(stamm, sizeof stamm, "main%02d", id);
            re15_rekorder_r38_stimme(room, id, dir, sizeof dir, stamm, sizeof stamm);
            re15_johnruf_r39_stimme(room, id, dir, sizeof dir, stamm, sizeof stamm);
            if (room != 0x3020 && room != 0x30E0 && room != 0x6030 && room != 0x6031 &&
                (strstr(dir, "room3020") || strstr(dir, "room30E0") || strstr(dir, "room6030") || strstr(dir, "room6031"))) uml++;
        }
    }
    PRUEF(uml == 0, "keine Umleitung (stimmen_r39, elza_r36, Rekorder r38, Hilferufe r39, Elza-Montage r38) beruehrt die 15 Saetze (%d)", uml);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "texte") || !strcmp(teil, "alle")) teil_texte();
    if (!strcmp(teil, "zeilen") || !strcmp(teil, "alle")) teil_zeilen();
    if (!strcmp(teil, "seitenwechsel") || !strcmp(teil, "alle")) teil_seitenwechsel();
    if (!strcmp(teil, "standzeit") || !strcmp(teil, "alle")) teil_standzeit();
    if (!strcmp(teil, "fortsetzung") || !strcmp(teil, "alle")) teil_fortsetzung();
    if (!strcmp(teil, "alle_zeilen") || !strcmp(teil, "alle")) teil_alle_zeilen();
    if (!strcmp(teil, "quellen_zeilen") || !strcmp(teil, "alle")) teil_quellen_zeilen();
    if (!strcmp(teil, "darstellung") || !strcmp(teil, "alle")) teil_darstellung();
    if (!strcmp(teil, "ohne_stimme") || !strcmp(teil, "alle")) teil_ohne_stimme();
    if (!strcmp(teil, "altaufnahme") || !strcmp(teil, "alle")) teil_altaufnahme();
    printf(g_fail ? "=== FAIL ===\n" : "=== OK ===\n");
    return g_fail;
}
