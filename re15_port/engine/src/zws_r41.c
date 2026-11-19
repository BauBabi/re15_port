/*
 * zws_r41.c — Runde 41, Zwischensequenzen mit den Gorilla-Filmen des Nutzers: Gorilla-Schrei vor der "Was war das?"-Zeile,
 * Film des Nutzers vor der Fortsetzung des Dialogs. Eine Tabelle, zwei Filme:
 *   114  ROOM1141 John und Elza: Port-Zeilen 61 (Schrei) / 60 (Film) im Programm ENTRIEGELN39 (tools/r39_dining/
 *        programme_bauen.py, Abschnitt "Runde 41 ZWS114"), vor "What was that?!" bzw. "Shit! that is what I was talking about!"
 *   11C  ROOM11C0 Leon und Ada: RDT-Anker in Original-sub02, Schrei vor Sleep 20 @0x01920 (dann "What!?" @0x01928), Film vor
 *        Sce_bgm_control @0x01982 (nach dem Umdrehen zum Anhaenger, vor dem Musikstart), danach der Auftritt in Kamera 12
 *        @0x01988..@0x01A5B unsichtbar vorgespult bis Cut_chg 5 @0x01A5C, dort Einblenden und msg 5
 *   zug600/zug601  ROOM6030 Leon / ROOM6031 Elza: Zugfilm (Runde 42 Strang finale) an der Port-Zeile 60 im Zugstart-Programm
 *   ende5090/ende5091  ROOM5090 Leon / ROOM5091 Elza: Endvideo (Runde 42 Strang finale Stufe 5) an der Port-Zeile 60 im Port-Programm
 *        statt sub02 (include/re15_endvideo_r42.h); Datei fehlt -> Zeile laeuft durch, Abspann folgt
 * Herleitung, Ablauf und Belege: include/re15_zws_r41.h.
 */
#include "re15_zws_r41.h"
#include "re15_vab.h"
#include "re15_fade.h"   /* RE2-Einblenden nach dem Film (Kanal-Engine fade_common.c) */
#include "re15_engine.h" /* g_engine.frame_count (Meldungen) */
#include "re15_scd.h"    /* g_scd.props (Endzustand Objekt 0 nach dem Vorspulen) */
#include "re15_actor.h"  /* g_actors (Endzustand Work 2/1, 2/2) */
#include "re15_birkinfilm_r42.h"   /* Runde 42 birkinfilm: Tabellenzeile birk6000 (ROOM6000, RE2-Film R409) */

#include <stdlib.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;   /* scd_vm.c: Stimmen-Riegel (Plattform stempelt je Bild) */

/* ============================ Tabelle ============================ */
static const re15_zws_film_t k_filme[] = {
    /* Runde 42 birkinfilm: ROOM6000 Port-Zeile 44 im Programm k_bf_6000_prog (gen/birkinfilm_r42.inc), ohne Schrei, ohne Vorspulen,
     * Uebergang RE2 ROOM4090 (64 ab / Film / 64 auf), Bild 40 Zeilen tiefer (re15_birkinfilm_r42.h). */
    {   "birk6000", RE15_BF_RAUM,
        { -1, 0, { 0 }, 0 },
        { RE15_BF_MSG_FILM, 0, { 0 }, 0 },
        { -1, 0, { 0 }, 0 },
        -1, RE15_BF_FILM, RE15_BF_FILM_BILDER, RE15_ZWS_BLENDART_4090, RE15_BF_FILM_Y },
    /* Runde 42 birkinfilm Stufe 2: ROOM5011 Port-Zeile 47 im Programm k_bf_5011_prog (Elza, zwischen msg 3 @0x00640 und msg 4
     * @0x00690), sonst wie birk6000 (derselbe Film, Uebergang RE2 ROOM4090, Bild 40 Zeilen tiefer). */
    {   "birk5011", RE15_BF_RAUM_ELZA,
        { -1, 0, { 0 }, 0 },
        { RE15_BF_ELZA_MSG_FILM, 0, { 0 }, 0 },
        { -1, 0, { 0 }, 0 },
        -1, RE15_BF_FILM, RE15_BF_FILM_BILDER, RE15_ZWS_BLENDART_4090, RE15_BF_FILM_Y },
    {   "zws114", RE15_ZWS114_RAUM,
        { RE15_ZWS114_MSG_SCHREI, 0, { 0 }, 0 },                     /* ENTRIEGELN39: vor Sleep 20 @0x00DC6 + msg 6 */
        { RE15_ZWS114_MSG_FILM,   0, { 0 }, 0 },                     /* ENTRIEGELN39: nach Sleep 86 @0x00E10, vor msg 7 */
        { -1, 0, { 0 }, 0 },                                         /* kein Vorspulen: Einblenden direkt nach dem Film */
        RE15_ZWS_SCHREI_SATZ, RE15_ZWS114_FILM, RE15_ZWS114_FILM_BILDER },
    {   "zws11c", RE15_ZWS11C_RAUM,
        { -1, RE15_ZWS11C_ANKER_SCHREI, { 0x09, 0x0A, 0x14, 0x00 }, 4 },               /* sub02 Sleep 20 vor msg 4 */
        { -1, RE15_ZWS11C_ANKER_FILM,   { 0x54, 0x00, 0x01, 0x00, 0x00, 0x00 }, 6 },   /* sub02 Sce_bgm_control MAIN Play */
        { -1, RE15_ZWS11C_ANKER_SICHT,  { 0x29, 0x05 }, 2 },                           /* sub02 Cut_chg 5 vor msg 5 */
        RE15_ZWS_SCHREI_SATZ, RE15_ZWS11C_FILM, RE15_ZWS11C_FILM_BILDER },
    /* Runde 42 Strang finale Stufe 2: Zugfilm im Zugstart, ohne Schrei und ohne Vorspulen (re15_zws_r41.h "Zugfilm Raum 600") */
    {   "zug600", RE15_ZUG600_RAUM_LEON,
        { -1, 0, { 0 }, 0 },                                         /* kein Schrei */
        { RE15_ZUG600_MSG_FILM, 0, { 0 }, 0 },                       /* k_zug_seg z_film: nach dem Pultlicht, vor Cut 1 + msg 27 */
        { -1, 0, { 0 }, 0 },
        -1, RE15_ZUG600_FILM, RE15_ZUG600_FILM_BILDER },
    {   "zug601", RE15_ZUG600_RAUM_ELZA,
        { -1, 0, { 0 }, 0 },
        { RE15_ZUG600_MSG_FILM, 0, { 0 }, 0 },                       /* k_s6_6031_zug: nach dem Pultlicht, vor Cut 1 + msg 14 */
        { -1, 0, { 0 }, 0 },
        -1, RE15_ZUG600_FILM, RE15_ZUG600_FILM_BILDER },
    /* Runde 42 Strang finale Stufe 5: Endvideo nach dem Finale, Port-Programm statt sub02 (include/re15_endvideo_r42.h), ohne Schrei
     * und ohne Vorspulen; Blende im Skript nach RE2 ROOM4090 (@0x027F4 / @0x02812 / @0x0281C) */
    {   "ende5090", RE15_ENDE_RAUM_LEON,
        { -1, 0, { 0 }, 0 },
        { RE15_ENDE_MSG_FILM, 0, { 0 }, 0 },                         /* k_ende: nach dem Abblenden, vor Aot_on(5) @0x012D6 */
        { -1, 0, { 0 }, 0 },
        -1, RE15_ENDE_FILM_LEON, RE15_ENDE_FILM_BILDER, RE15_ZWS_BLENDART_SKRIPT },
    {   "ende5091", RE15_ENDE_RAUM_ELZA,
        { -1, 0, { 0 }, 0 },
        { RE15_ENDE_MSG_FILM, 0, { 0 }, 0 },                         /* k_ende: nach dem Abblenden, vor Aot_on(5) @0x012BA */
        { -1, 0, { 0 }, 0 },
        -1, RE15_ENDE_FILM_ELZA, RE15_ENDE_FILM_BILDER, RE15_ZWS_BLENDART_SKRIPT },
};
#define ZWS_ANZAHL ((int)(sizeof k_filme / sizeof k_filme[0]))

int re15_zws_r41_anzahl(void) { return ZWS_ANZAHL; }
const re15_zws_film_t *re15_zws_r41_film_nr(int i) { return (i >= 0 && i < ZWS_ANZAHL) ? &k_filme[i] : NULL; }
const re15_zws_film_t *re15_zws_r41_film_fuer_raum(unsigned raum)
{
    for (int i = 0; i < ZWS_ANZAHL; i++) if (k_filme[i].raum == (raum & 0xFFFFu)) return &k_filme[i];
    return NULL;
}

/* ============================ Zustand ============================ */
int (*re15_zws_r41_schrei_hook)(int satz) = NULL;
int re15_zws_r41_film_spieler = 0;
int g_re15_zws_anker_n = 0;

unsigned g_re15_zws_schreie = 0;
unsigned g_re15_zws_filme = 0;
int      g_re15_zws_schrei_takte = -1;
unsigned g_re15_zws_stimme_warten = 0;
unsigned g_re15_zws_vorspul_bilder = 0;

static const re15_zws_film_t *s_aktiv = NULL;   /* Film des laufenden Schreis/Films (Kennung, Datei, Bilder) */
static const re15_zws_film_t *s_anker = NULL;   /* Film mit armierten RDT-Ankern im geladenen Raum */
static const uint8_t *s_basis = NULL;           /* rohes RDT, gegen das die Anker geprueft sind */
static int s_schrei_rest = 0;     /* > 0: Schrei parkt */
static int s_schrei_laeuft = 0;
static int s_film = RE15_ZWS_FILM_AUS;
static unsigned s_warte = 0;      /* Anker: Takte auf eine laufende Sprachzeile */
static int s_vorspulen = 0;       /* 1 = Auftritt nach dem Film laeuft unsichtbar (11C) */
static int s_abblend_takte = 0;   /* Blendenart 4090: Takte seit der Abblende (RE15_ZWS4090_AB_BILDER) */

static void meldung(const re15_zws_film_t *f, const char *t, int a, int b)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[%s] %s (%d, %d) F%u\n", f ? f->kennung : "zws", t, a, b, (unsigned)g_engine.frame_count);
#else
    (void)f; (void)t; (void)a; (void)b;
#endif
}

void re15_zws_r41_install(uint16_t room_id)
{
    (void)room_id;
    s_schrei_rest = 0;
    s_schrei_laeuft = 0;
    s_film = RE15_ZWS_FILM_AUS;
    s_warte = 0;
    s_aktiv = NULL;
    s_vorspulen = 0;
    s_abblend_takte = 0;
}

void re15_zws_r41_raum_scan(const uint8_t *raw, long raw_size, unsigned room_id)
{
    g_re15_zws_anker_n = 0;
    s_anker = NULL; s_basis = NULL;
    const re15_zws_film_t *f = re15_zws_r41_film_fuer_raum(room_id);
    if (!f || !raw || raw_size <= 0) return;
    const re15_zws_stelle_t *st[3] = { &f->schrei, &f->film, &f->sichtbar };
    int n = 0;
    for (int i = 0; i < 3; i++) {
        if (st[i]->msg >= 0 || st[i]->n == 0) continue;                  /* Port-Zeile / keine Stelle: kein Anker */
        if ((long)st[i]->off + st[i]->n > raw_size || memcmp(raw + st[i]->off, st[i]->bytes, st[i]->n) != 0) {
            meldung(f, "RDT weicht am Anker ab -> kein Schrei/Film (Original-Ablauf); Datei-Offset", (int)st[i]->off, i);
            return;                                                      /* falscher Stand: lieber Original */
        }
        n++;
    }
    if (n == 0) return;
    s_anker = f; s_basis = raw;
    g_re15_zws_anker_n = n;
}

int  re15_zws_r41_film_offen(void)    { return s_film == RE15_ZWS_FILM_OFFEN; }
int  re15_zws_r41_film_zustand(void)  { return s_film; }
int  re15_zws_r41_schrei_rest(void)   { return s_schrei_laeuft ? s_schrei_rest : 0; }
const re15_zws_film_t *re15_zws_r41_aktiv(void) { return s_aktiv; }

/* Einblenden aus Schwarz wie RE2 ROOM1080 sub14: RE2 hat VOR seinem Movie_on das Einblenden gesetzt (Sce_fade_set @0x0252A);
 * dort laeuft der Fade-Takt FUN_8002c378 (@0x8002b448) einmal vor dem Bildwechsel, dessen Kachel (Helligkeit 240) erst NACH
 * dem Film gezeichnet wird. Der Port setzt den Kanal deshalb unmittelbar vor dem ersten wieder gezeigten Spielbild: es zeigt
 * 240, dann 224 ... 16, 0 (re15_fade_tick in re15_render_end_frame) - dieselbe Bildfolge. */
static void einblenden(const char *was)
{
    re15_fade_config(RE15_ZWS_BLENDE_KANAL, RE15_ZWS_BLENDE_ABR, RE15_ZWS_BLENDE_RGB,
                     (int16_t)RE15_ZWS_BLENDE_WERT, RE15_ZWS_BLENDE_EBENE);   /* FUN_8002c1a0 */
    g_fade_ch[RE15_ZWS_BLENDE_KANAL].level = RE15_ZWS_BLENDE_PEGEL;          /* @0x80057fa0 */
    meldung(s_aktiv, was, RE15_ZWS_BLENDE_KANAL, RE15_ZWS_BLENDE_PEGEL);
}

/* Ende des unsichtbaren Vorspulens (Anker erreicht oder Notbremse): Endzustand fuer den Nachweis mitschreiben (Objekt 0 =
 * Work 3/0, Gorillas Work 2/1 / 2/2 = g_actors[2] / [3] (scd_work_bind: Slot = Index + 1), Member 0x0C = grid_id), dann
 * einblenden. */
static void vorspulen_ende(const char *grund)
{
    s_vorspulen = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[%s] F%u Vorspulen Ende (%s) nach %u Bildern:", s_aktiv ? s_aktiv->kennung : "zws",
            (unsigned)g_engine.frame_count, grund, g_re15_zws_vorspul_bilder);
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == 0)
            fprintf(stderr, " obj0=(%d,%d,%d r%d/%d/%d)", (int)g_scd.props[i].x, (int)g_scd.props[i].y, (int)g_scd.props[i].z,
                    (int)g_scd.props[i].rot_x, (int)g_scd.props[i].rot_y, (int)g_scd.props[i].rot_z);
    for (int s = 2; s <= 3; s++)
        fprintf(stderr, " w2/%d=(%d,%d,%d r%d g%02x)", s - 1, (int)g_actors[s].x, (int)g_actors[s].y, (int)g_actors[s].z,
                (int)g_actors[s].rot_y, (unsigned)g_actors[s].grid_id);
    fprintf(stderr, " cam=%d\n", (int)g_scd.cam_id);
#else
    (void)grund;
#endif
    einblenden("Einblenden aus Schwarz wie RE2 ROOM1080 sub14 (Kanal, Pegel)");
}

/* Blendenart 4090 (re15_zws_r41.h "ZWEITE BLENDENART"): Kanal, ABR, RGB und Ebene wie ROOM1080 (dieselben Bytes @0x027F5-@0x027F7
 * `02 02 07` und `li a3,0x7` @0x80057f14), nur Wert und Pegel anders. */
static void blende_4090(int16_t wert, uint16_t pegel, const char *was)
{
    re15_fade_config(RE15_ZWS_BLENDE_KANAL, RE15_ZWS_BLENDE_ABR, RE15_ZWS_BLENDE_RGB, wert, RE15_ZWS_BLENDE_EBENE);
    g_fade_ch[RE15_ZWS_BLENDE_KANAL].level = pegel;
    meldung(s_aktiv, was, (int)wert, (int)pegel);
}

/* Film zu Ende. 114: sofort einblenden. 11C (Tabellenspalte sichtbar): erst den Original-Auftritt unsichtbar vorspulen.
 * Blendenart 4090: Pegel 0x7FFF wie RE2 @0x0281C (die Einblende selbst ist seit der Anforderung gesetzt). */
void re15_zws_r41_film_gespielt(void)
{
    if (s_film != RE15_ZWS_FILM_OFFEN) return;
    s_film = RE15_ZWS_FILM_GESPIELT;
    if (s_aktiv && s_aktiv->blende == RE15_ZWS_BLENDART_4090) {
        g_fade_ch[RE15_ZWS_BLENDE_KANAL].level = RE15_ZWS4090_NACH_PEGEL;
        meldung(s_aktiv, "Film zu Ende: Einblende wie RE2 ROOM4090 @0x0281C (Kanal, Pegel)", RE15_ZWS_BLENDE_KANAL,
                RE15_ZWS4090_NACH_PEGEL);
        return;
    }
    if (s_aktiv && s_aktiv->sichtbar.n > 0 && s_anker == s_aktiv) {
        s_vorspulen = 1;
        g_re15_zws_vorspul_bilder = 0;
        meldung(s_aktiv, "Film zu Ende: Auftritt unsichtbar vorspulen bis Datei-Offset", (int)s_aktiv->sichtbar.off, 0);
        return;
    }
    if (s_aktiv && s_aktiv->blende == RE15_ZWS_BLENDART_SKRIPT) {   /* Endvideo: Sce_fade_adjust im Skript (RE2 ROOM4090 @0x0281C) */
        meldung(s_aktiv, "Film zu Ende: Blende macht das Skript (RE2 ROOM4090 @0x0281C)", 0, 0);
        return;
    }
    einblenden("Film zu Ende: Einblenden wie RE2 ROOM1080 sub14 (Kanal, Pegel)");
}

int re15_zws_r41_vorspulen(void) { return s_vorspulen; }

void re15_zws_r41_vorspul_bild(void)
{
    if (!s_vorspulen) return;
    if (++g_re15_zws_vorspul_bilder >= RE15_ZWS_VORSPUL_NOTBREMSE) vorspulen_ende("NOTBREMSE - Anker nicht erreicht");
}

void re15_zws_r41_film_fehlt(void)
{
    if (s_film == RE15_ZWS_FILM_OFFEN) s_film = RE15_ZWS_FILM_GESPIELT;   /* kein Bild gezeigt -> kein Einblenden */
}

/* Schrei: Ton starten, dann je VM-Takt einmal hier vorbei (die Stelle parkt mit 2, der pc bleibt stehen), bis die
 * Tonlaenge abgelaufen ist. 1 = fertig (Port-Zeile verbraucht / Original-Opcode laeuft). */
static int schrei(const re15_zws_film_t *f)
{
    if (!s_schrei_laeuft) {
        int n = re15_zws_r41_schrei_hook ? re15_zws_r41_schrei_hook(f->schrei_satz) : -1;
        s_aktiv = f;
        g_re15_zws_schreie++;
        g_re15_zws_schrei_takte = n;
        meldung(f, "Schrei: ROOM11C0 snd1 Satz / Takte", f->schrei_satz, n);
        if (n <= 0) return 1;                  /* kein Ton (keine Plattform/Bank) -> weiter wie ohne Haken */
        s_schrei_laeuft = 1;
        s_schrei_rest = n;
        return 2;
    }
    if (--s_schrei_rest > 0) return 2;
    s_schrei_laeuft = 0;
    meldung(f, "Schrei zu Ende -> weiter (Sleep 20 vor der Zeile = kurze Pause)", f->schrei_satz, g_re15_zws_schrei_takte);
    return 1;
}

/* Film: anfordern und parken, bis die Plattform ihn gespielt hat (zwischen zwei Spielbildern). */
static int film(const re15_zws_film_t *f)
{
    if (s_film == RE15_ZWS_FILM_AUS) {
        g_re15_zws_filme++;
        s_aktiv = f;
        if (!re15_zws_r41_film_spieler) {
            meldung(f, "Film: keine Plattform - Stelle laeuft durch", 0, 0);
            return 1;
        }
        if (f->blende == RE15_ZWS_BLENDART_4090) {       /* RE2 ROOM4090 @0x027F4: erst 64 Bilder abblenden */
            s_film = RE15_ZWS_FILM_ABBLENDEN;
            s_abblend_takte = 0;
            blende_4090(RE15_ZWS4090_AB_WERT, RE15_ZWS4090_AB_PEGEL, "Abblende wie RE2 ROOM4090 @0x027F4 (Wert, Pegel)");
            return 2;
        }
        s_film = RE15_ZWS_FILM_OFFEN;
        meldung(f, "Film angefordert", f->film_bilder, 0);
        return 2;
    }
    if (s_film == RE15_ZWS_FILM_ABBLENDEN) {
        if (++s_abblend_takte < RE15_ZWS4090_AB_BILDER) return 2;
        blende_4090(RE15_ZWS4090_AUF_WERT, RE15_ZWS4090_AUF_PEGEL, "Einblende gesetzt wie RE2 ROOM4090 @0x02812 (Wert, Pegel)");
        s_film = RE15_ZWS_FILM_OFFEN;                   /* @0x02818 `6f 0b`: der Film laeuft zu Beginn des naechsten Bilds */
        meldung(f, "Film angefordert (nach der Abblende, Takte)", f->film_bilder, s_abblend_takte);
        return 2;
    }
    if (s_film == RE15_ZWS_FILM_OFFEN) return 2;
    s_film = RE15_ZWS_FILM_AUS;                /* gespielt: weiter mit der Fortsetzung */
    meldung(f, f->film.msg < 0 ? "Film zu Ende -> Original-Opcode am Anker"
               : f->raum == RE15_ZWS114_RAUM ? "Film zu Ende -> msg 7" : "Film zu Ende -> weiter nach der Port-Zeile", 0, 0);
    return 1;
}

int re15_zws_r41_message_on(const uint8_t *pc)
{
    if (!pc) return 0;
    const re15_zws_film_t *f = re15_zws_r41_film_fuer_raum(g_current_room_id);
    if (!f) return 0;
    if (f->schrei.msg >= 0 && pc[1] == (uint8_t)f->schrei.msg) return schrei(f);
    if (f->film.msg >= 0 && pc[1] == (uint8_t)f->film.msg) return film(f);
    return 0;
}

/* RDT-Anker: vor dem Start dieselbe Bedingung wie der Stimmen-Riegel in op_message_on (laufende Sprachzeile ausreden lassen,
 * Notbremse RE15_ZWS_STIMME_NOTBREMSE) - wie bei der Port-Zeile, deren Haken hinter diesem Riegel steht. */
int re15_zws_r41_pc(const uint8_t *pc)
{
    const re15_zws_film_t *f = s_anker;
    if (!f || !s_basis || !pc) return 0;
    if (s_vorspulen && f->sichtbar.n > 0 && pc == s_basis + f->sichtbar.off) {   /* Cut_chg 5 @0x01A5C: wieder sichtbar */
        vorspulen_ende("Anker Cut_chg 5");
        return 0;
    }
    const re15_zws_stelle_t *st;
    int ist_schrei;
    if (f->schrei.msg < 0 && pc == s_basis + f->schrei.off) { st = &f->schrei; ist_schrei = 1; }
    else if (f->film.msg < 0 && pc == s_basis + f->film.off) { st = &f->film; ist_schrei = 0; }
    else return 0;
    (void)st;
    const int begonnen = ist_schrei ? s_schrei_laeuft : (s_film != RE15_ZWS_FILM_AUS);
    if (!begonnen) {
        if (g_re15_voice_laeuft && g_re15_voice_restbilder > 0 && s_warte < RE15_ZWS_STIMME_NOTBREMSE) { s_warte++; return 2; }
        if (s_warte) meldung(f, "Anker: auf die Sprachzeile gewartet (Datei-Offset, Takte)", (int)st->off, (int)s_warte);
        g_re15_zws_stimme_warten = s_warte;
        s_warte = 0;
    }
    const int r = ist_schrei ? schrei(f) : film(f);
    return r == 2 ? 2 : 0;
}

int re15_zws_r41_satz_takte(const uint8_t *edt, const uint8_t *vh, int vh_size, const uint8_t *vb, int vb_size, int satz)
{
    if (!edt || !vh || !vb || vh_size <= 0 || vb_size <= 0 || satz < 0) return -1;
    re15_vab_t *vab = (re15_vab_t *)malloc(sizeof *vab);
    if (!vab) return -1;
    int takte = -1;
    if (re15_vab_parse(vh, (size_t)vh_size, vab) == 0) {
        int vags[8], tones[8];
        const int n = re15_edt_resolve_layers_ex(edt, vab, satz, vags, tones, 8);
        for (int k = 0; k < n; k++) {
            if (vags[k] < 0 || vags[k] >= vab->vag_count || tones[k] < 0) continue;
            const re15_vab_sample_t *s = &vab->samples[vags[k]];
            if (s->size == 0 || (long)s->offset + (long)s->size > (long)vb_size) continue;
            const size_t cap = (s->size / 16) * 28;
            int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
            if (!pcm) continue;
            const int proben = re15_vag_adpcm_decode(vb + s->offset, s->size, pcm, cap);   /* END-Flag-Abbruch */
            free(pcm);
            const re15_vab_tone_t *t = &vab->tones[tones[k]];
            /* dieselbe Tonhoehe wie audio_pc.c se_play_layers: note2pitch2(min_note, shift, center, shift) */
            const unsigned pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
            if (pitch == 0 || proben <= 0) continue;
            /* Dauer = proben * 0x1000 / pitch / 44100 s, in Takten aufgerundet */
            const long long zaehler = (long long)proben * 0x1000 * RE15_ZWS_TAKTE_JE_S;
            const long long nenner  = (long long)pitch * 44100;
            const int l = (int)((zaehler + nenner - 1) / nenner);
            if (l > takte) takte = l;
        }
    }
    free(vab);
    return takte;
}
