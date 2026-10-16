/* rekorder_r38.c — Runde 38 Spur J: Stimmschloss ROOM1100/1101 mit der Aufnahme des Nutzers.
 *
 * Belege, Lesart und Herleitung: include/re15_rekorder_r38.h und analysis/befunde_runde38/J_rekorder.md.
 * Kein Raum ausser ROOM1100/ROOM1101 wird beruehrt (Anker nur dort, nur bei passender 56-Byte-Signatur von sub02). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rekorder_r38.h"
#include "re15_aufschliessen_r36.h"   /* re15_aufschliessen_r36_notice, RE15_AUF_WEG_SKRIPT (Runde 36 Spur G) */
#include "re15_scd.h"                 /* scd_thread_t, scd_audio_event_t, scd_audio_queue_push */
#include "re15_engine.h"              /* g_engine.frame_count — nur fuer das Datei-Log */
#include "re15_aot.h"                 /* g_aot (Messschiene: Tuer-Platz 1 nach @0x00CC8) */

extern unsigned g_current_room_id;
/* Die Plattform stempelt beide je Bild (audio_pc.c re15_audio_tick, Block vor der Warteschlange); die Engine haelt sie
 * (scd_vm.c), damit Tests und PSX ohne Sprachpfad linken. Dieselbe Bedingung wie der Stimmen-Riegel in op_message_on. */
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;

/* sub02 ab Message_on 4 (@0x00C90) bis Cut_auto 1 (@0x00CC6), selbst gelesen in ROOM1100.RDT und ROOM1101.RDT. */
static const uint8_t k_sig[RE15_REK_SIG_LAENGE] = {
    0x2b, 0x04, 0xff, 0xff,  0x02, 0x00,                                   /* @0x00C90 Message_on 4, @0x00C94 Evt_next+Nop */
    0x29, 0x0c, 0x09, 0x0a, 0x05, 0x00,  0x29, 0x0b, 0x09, 0x0a, 0x05, 0x00, /* @0x00C96 Cut 12 / @0x00C9C Cut 11 (+Sleep 5) */
    0x29, 0x0c, 0x09, 0x0a, 0x05, 0x00,  0x29, 0x0b, 0x09, 0x0a, 0x05, 0x00, /* @0x00CA2 / @0x00CA8 */
    0x29, 0x0c, 0x09, 0x0a, 0x05, 0x00,  0x29, 0x0b, 0x09, 0x0a, 0x05, 0x00, /* @0x00CAE / @0x00CB4 */
    0x2b, 0x02, 0xff, 0xff,  0x02, 0x00,                                   /* @0x00CBA Message_on 2, @0x00CBE Evt_next+Nop */
    0x22, 0x02, 0x07, 0x00,  0x29, 0x06,  0x3c, 0x01 };                    /* @0x00CC0 Set(2,7,0), Cut 6, Cut_auto 1 */

/* Notbremse fuer einen haengenden Stimmen-Kanal: dieselbe Grenze wie der Stimmen-Riegel in scd_vm.c op_message_on
 * (RE15_VOICE_NOTBREMSE = 300 Bilder = 10 s, "laengste Aufnahme 242,5 Bilder"); die Rekorder-Aufnahme hat 79,4 Bilder
 * (data-Laenge u32 @Datei 40 = 169414 B, 16 bit mono 32000 Hz -> 84707 Samples). Kein Original-Gegenstueck. */
#define REK_NOTBREMSE 300
/* Die Plattform stempelt g_re15_voice_laeuft am ANFANG von re15_audio_tick und arbeitet erst DANACH die Warteschlange
 * ab (audio_pc.c: Stempel-Block vor `while (scd_audio_queue_pop(&evt))`): die im Bild N angeforderte Aufnahme ist fuer
 * die VM erst im Bild N+2 sichtbar. So lange zaehlt sie als laufend. Port-Struktur, kein Original-Gegenstueck. */
#define REK_STEMPEL_BILDER 2

int g_re15_rekorder_anker_n = 0;
static const unsigned char *s_basis = NULL;     /* sub02-Signatur armiert -> rohes RDT */
static unsigned s_raum = 0;
static int      s_zustand = RE15_REK_AUS;
static int      s_bilder = 0;                   /* Spielbilder seit der Anforderung (re15_rekorder_r38_tick) */
static int      s_lief = 0;                     /* Plattform hat die Aufnahme als laufend gemeldet */
static int      s_warte = 0, s_warte_letzt = 0;
static unsigned s_anf = 0, s_pieps = 0, s_unterdrueckt = 0, s_tuer = 0;

static void logf_(const char *fmt, unsigned a, unsigned b, unsigned c)
{
    static const char *s_pfad = NULL; static int s_init = 0;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_REKORDER_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "ab");
    if (!f) return;
    fprintf(f, "F%u raum=%04X ", (unsigned)g_engine.frame_count, (unsigned)g_current_room_id);
    fprintf(f, fmt, a, b, c);
    fputc('\n', f);
    fclose(f);
}

static int ist_raum(unsigned room_id) { return room_id == 0x1100u || room_id == 0x1101u; }

void re15_rekorder_r38_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_rekorder_anker_n = 0;
    s_basis = NULL; s_raum = room_id; s_zustand = RE15_REK_AUS;
    s_bilder = 0; s_lief = 0; s_warte = 0; s_warte_letzt = 0;
    if (!ist_raum(room_id) || !raw || raw_size <= 0) return;
    if ((long)RE15_REK_MSG4 + RE15_REK_SIG_LAENGE > (long)raw_size) return;
    if (memcmp(raw + RE15_REK_MSG4, k_sig, sizeof k_sig) != 0) {
        logf_("Anker @0x%05X: Signatur weicht ab - Rekorder nicht armiert", RE15_REK_MSG4, 0u, 0u);
        return;                                  /* falscher Stand: lieber Original (Runde-36-Folge an msg 2) */
    }
    s_basis = raw;
    s_zustand = RE15_REK_BEREIT;
    g_re15_rekorder_anker_n = 1;
}

/* Wie der Klartext-Zweig von op_message_on (scd_queue_voice): ein Stimmen-Ereignis einreihen; die Plattform spielt es
 * ueber re15_voice_play (Raum, Id) -> re15_rekorder_r38_stimme setzt die Nutzer-Datei ein. */
static void stimme_anfordern(void)
{
    scd_audio_event_t ev;
    memset(&ev, 0, sizeof ev);
    ev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
    ev.sample_id = (uint8_t)RE15_REKORDER_STIMME_ID;
    scd_audio_queue_push(&ev);
}

int re15_rekorder_r38_pc(void *thread)
{
    scd_thread_t *t = (scd_thread_t *)thread;
    if (!s_basis || !t || !t->pc) return 0;
    const unsigned char *pc = t->pc;

    if (pc == s_basis + RE15_REK_START && s_zustand == RE15_REK_BEREIT) {
        /* @0x00C94: Message_on 4 ist in DIESEM Takt ausgefuehrt (die Nachricht ist offen) -> "reinsprechen". Hinter dem
         * Stimmen-Ereignis von Message_on 4 selbst eingereiht, damit die Aufnahme den Kanal behaelt. */
        stimme_anfordern();
        s_anf++;
        s_zustand = RE15_REK_STIMME; s_bilder = 0; s_lief = 0; s_warte = 0;
        logf_("stimme an: sub02 @0x%05X (Message_on 4 im selben Takt), Id %u -> " RE15_REKORDER_ORDNER
              RE15_REKORDER_STAMM ".wav (Anforderung %u)", RE15_REK_START, (unsigned)RE15_REKORDER_STIMME_ID, s_anf);
        return 0;                                /* Evt_next normal */
    }
    if (pc == s_basis + RE15_REK_WARTE && s_zustand == RE15_REK_STIMME) {
        /* @0x00C96: Nachricht 4 ist zu. Erst weiter, wenn die Aufnahme zu Ende ist. */
        int laeuft = (s_bilder < REK_STEMPEL_BILDER) || (g_re15_voice_laeuft && g_re15_voice_restbilder > 0);
        if (laeuft && s_warte < REK_NOTBREMSE) { s_warte++; return 2; }
        s_zustand = RE15_REK_FREI;
        s_warte_letzt = s_warte;
        logf_("frei: sub02 @0x%05X Cut_chg 12 nach %u Wartetakten (Notbremse %u)", RE15_REK_WARTE, (unsigned)s_warte,
              (s_warte >= REK_NOTBREMSE) ? 1u : 0u);
        return 0;                                /* Cut_chg 12 normal */
    }
    if (pc == s_basis + RE15_REK_ACCEPTED && s_zustand == RE15_REK_FREI) {
        /* @0x00C9C: erstes ACCEPTED. Piep im selben Takt (RE2 ROOM2110 @0x01B5C Piep / @0x01B6C Anzeige offen),
         * Aufschliessen +15 (RE2 @0x01B8C Sleep 15 / @0x01B90) = Runde-36-Folge KARTE_AUF der Stelle (Raum, 2). */
        int art = re15_aufschliessen_r36_notice(s_raum, 2, RE15_AUF_WEG_SKRIPT);
        s_pieps++;
        s_zustand = RE15_REK_PIEP;
        logf_("ACCEPTED: sub02 @0x%05X Cut_chg %u -> Piep + Aufschliessen (Runde-36-Art %u, 2 = KARTE_AUF)",
              RE15_REK_ACCEPTED, (unsigned)RE15_REK_ACCEPTED_CUT, (unsigned)art);
        return 0;                                /* Cut_chg 11 normal, selber Takt */
    }
    if (pc == s_basis + RE15_REK_TUER && s_zustand == RE15_REK_ENDE) {
        /* Nur Messschiene: @0x00CC8 Aot_reset Platz 1 sce 2 ist ausgefuehrt (Tuer nach ROOM1110 frei). */
        const re15_aot_t *a = &g_aot.slots[1];
        s_tuer++;
        logf_("Tuer frei: Platz 1 aktiv %u typ %u (1 = Tuer) nach @0x%05X", (unsigned)a->active, (unsigned)a->type,
              0x00CC8u);
    }
    return 0;
}

int re15_rekorder_r38_message_on(unsigned room_id, const unsigned char *pc)
{
    if (!s_basis || !pc || room_id != s_raum) return 0;
    if (pc == s_basis + RE15_REK_MSG2 && s_zustand == RE15_REK_PIEP) {
        s_zustand = RE15_REK_ENDE;
        s_unterdrueckt++;
        logf_("Message_on 2 @0x%05X: Piep/Aufschliessen schon am ACCEPTED gespielt - Runde-36-Folge hier aus (%u)",
              RE15_REK_MSG2, s_unterdrueckt, 0u);
        return 1;
    }
    return 0;
}

void re15_rekorder_r38_tick(void)
{
    if (s_zustand != RE15_REK_STIMME) return;
    s_bilder++;
    int laeuft = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
    if (laeuft && !s_lief) {
        s_lief = 1;
        logf_("stimme laeuft: Plattform meldet %u Restbilder (%u Bilder nach der Anforderung)",
              (unsigned)g_re15_voice_restbilder, (unsigned)s_bilder, 0u);
    } else if (!laeuft && s_lief == 1) {
        s_lief = 2;
        logf_("stimme zu Ende: %u Bilder nach der Anforderung", (unsigned)s_bilder, 0u, 0u);
    }
}

int re15_rekorder_r38_stimme(unsigned room_id, int voice_id, char *ordner, size_t n_ordner, char *stamm, size_t n_stamm)
{
    if (!ist_raum(room_id) || voice_id != RE15_REKORDER_STIMME_ID) return 0;
    if (ordner && n_ordner) snprintf(ordner, n_ordner, "%s", RE15_REKORDER_ORDNER);
    if (stamm && n_stamm)   snprintf(stamm, n_stamm, "%s", RE15_REKORDER_STAMM);
    return 1;
}

int re15_rekorder_r38_ist_datei(unsigned rid, const char *name)
{
    return rid == 0x1100u && name && strcmp(name, RE15_REKORDER_STAMM ".wav") == 0;
}

int      re15_rekorder_r38_zustand(void)          { return s_zustand; }
int      re15_rekorder_r38_wartebilder(void)      { return s_warte_letzt; }
unsigned re15_rekorder_r38_anforderungen(void)    { return s_anf; }
unsigned re15_rekorder_r38_pieps(void)            { return s_pieps; }
unsigned re15_rekorder_r38_unterdrueckt(void)     { return s_unterdrueckt; }
unsigned re15_rekorder_r38_tuer_frei(void)        { return s_tuer; }
int      re15_rekorder_r38_bilder_seit_start(void) { return s_bilder; }
