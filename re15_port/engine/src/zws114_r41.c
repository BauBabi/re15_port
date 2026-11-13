/*
 * zws114_r41.c — Runde 41, Zwischensequenz Raum 114 (ROOM1141, John und Elza): Gorilla-Schrei vor "What was that?!",
 * Film des Nutzers vor "Shit! that is what I was talking about!". Herleitung, Ablauf und Belege: include/re15_zws114_r41.h.
 * Die Zeilen selbst stehen im Programm ENTRIEGELN39 (tools/r39_dining/programme_bauen.py, Abschnitt "Runde 41 ZWS114").
 */
#include "re15_zws114_r41.h"
#include "re15_vab.h"
#include "re15_fade.h"   /* RE2-Einblenden nach dem Film (Kanal-Engine fade_common.c) */

#include <stdlib.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

extern unsigned g_current_room_id;

int (*re15_zws114_r41_schrei_hook)(int satz) = NULL;
int re15_zws114_r41_film_spieler = 0;

unsigned g_re15_zws114_schreie = 0;
unsigned g_re15_zws114_filme = 0;
int      g_re15_zws114_schrei_takte = -1;

static int s_schrei_rest = 0;     /* > 0: Message_on 61 parkt */
static int s_schrei_laeuft = 0;
static int s_film = RE15_ZWS114_FILM_AUS;

static void meldung(const char *t, int a, int b)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[zws114] %s (%d, %d)\n", t, a, b);
#else
    (void)t; (void)a; (void)b;
#endif
}

void re15_zws114_r41_install(uint16_t room_id)
{
    (void)room_id;
    s_schrei_rest = 0;
    s_schrei_laeuft = 0;
    s_film = RE15_ZWS114_FILM_AUS;
}

int  re15_zws114_r41_film_offen(void)    { return s_film == RE15_ZWS114_FILM_OFFEN; }
int  re15_zws114_r41_film_zustand(void)  { return s_film; }
int  re15_zws114_r41_schrei_rest(void)   { return s_schrei_laeuft ? s_schrei_rest : 0; }

/* Film zu Ende: RE2 ROOM1080 sub14 hat VOR seinem Movie_on das Einblenden gesetzt (Sce_fade_set @0x0252A); in RE2 laeuft der
 * Fade-Takt FUN_8002c378 (@0x8002b448) dabei einmal vor dem Bildwechsel, dessen Kachel (Helligkeit 240) erst NACH dem Film
 * gezeichnet wird. Der Port setzt den Kanal deshalb hier, unmittelbar nach dem Film: das naechste gezeichnete Bild zeigt 240,
 * dann 224 ... 16, 0 (re15_fade_tick in re15_render_end_frame) - dieselbe Bildfolge. */
void re15_zws114_r41_film_gespielt(void)
{
    if (s_film != RE15_ZWS114_FILM_OFFEN) return;
    s_film = RE15_ZWS114_FILM_GESPIELT;
    re15_fade_config(RE15_ZWS114_BLENDE_KANAL, RE15_ZWS114_BLENDE_ABR, RE15_ZWS114_BLENDE_RGB,
                     (int16_t)RE15_ZWS114_BLENDE_WERT, RE15_ZWS114_BLENDE_EBENE);   /* FUN_8002c1a0 */
    g_fade_ch[RE15_ZWS114_BLENDE_KANAL].level = RE15_ZWS114_BLENDE_PEGEL;                /* @0x80057fa0 */
    meldung("Film zu Ende: Einblenden wie RE2 ROOM1080 sub14 (Kanal, Pegel)", RE15_ZWS114_BLENDE_KANAL,
            RE15_ZWS114_BLENDE_PEGEL);
}

void re15_zws114_r41_film_fehlt(void)
{
    if (s_film == RE15_ZWS114_FILM_OFFEN) s_film = RE15_ZWS114_FILM_GESPIELT;   /* kein Bild gezeigt -> kein Einblenden */
}

/* Message_on 61: Ton starten, dann je VM-Takt einmal hier vorbei (op_message_on parkt mit `return 2`, der pc bleibt
 * stehen), bis die Tonlaenge abgelaufen ist. */
static int schrei(void)
{
    if (!s_schrei_laeuft) {
        int n = re15_zws114_r41_schrei_hook ? re15_zws114_r41_schrei_hook(RE15_ZWS114_SCHREI_SATZ) : -1;
        g_re15_zws114_schreie++;
        g_re15_zws114_schrei_takte = n;
        meldung("Schrei: ROOM11C0 snd1 Satz / Takte", RE15_ZWS114_SCHREI_SATZ, n);
        if (n <= 0) return 1;                  /* kein Ton (keine Plattform/Bank) -> weiter wie ohne Zeile */
        s_schrei_laeuft = 1;
        s_schrei_rest = n;
        return 2;
    }
    if (--s_schrei_rest > 0) return 2;
    s_schrei_laeuft = 0;
    return 1;                                  /* Ton zu Ende: weiter mit Sleep 20 @0x00DC6, dann msg 6 */
}

/* Message_on 60: Film anfordern und parken, bis die Plattform ihn gespielt hat (zwischen zwei Spielbildern). */
static int film(void)
{
    if (s_film == RE15_ZWS114_FILM_AUS) {
        g_re15_zws114_filme++;
        if (!re15_zws114_r41_film_spieler) {
            meldung("Film: keine Plattform - Zeile laeuft durch", 0, 0);
            return 1;
        }
        s_film = RE15_ZWS114_FILM_OFFEN;
        meldung("Film angefordert", RE15_ZWS114_FILM_BILDER, 0);
        return 2;
    }
    if (s_film == RE15_ZWS114_FILM_OFFEN) return 2;
    s_film = RE15_ZWS114_FILM_AUS;             /* gespielt: weiter mit msg 7 */
    meldung("Film zu Ende -> msg 7", 0, 0);
    return 1;
}

int re15_zws114_r41_message_on(const uint8_t *pc)
{
    if (!pc || (g_current_room_id & 0xFFFFu) != RE15_ZWS114_RAUM) return 0;
    if (pc[1] == RE15_ZWS114_MSG_SCHREI) return schrei();
    if (pc[1] == RE15_ZWS114_MSG_FILM) return film();
    return 0;
}

int re15_zws114_r41_satz_takte(const uint8_t *edt, const uint8_t *vh, int vh_size, const uint8_t *vb, int vb_size,
                               int satz)
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
            const long long zaehler = (long long)proben * 0x1000 * RE15_ZWS114_TAKTE_JE_S;
            const long long nenner  = (long long)pitch * 44100;
            const int l = (int)((zaehler + nenner - 1) / nenner);
            if (l > takte) takte = l;
        }
    }
    free(vab);
    return takte;
}
