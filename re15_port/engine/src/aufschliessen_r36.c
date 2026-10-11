/* aufschliessen_r36.c — Runde 36 Spur G (Punkt 3): RE2-Tonfolge beim ERFOLGREICHEN Aufschliessen.
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL. Belege im Kopf von include/re15_aufschliessen_r36.h und im Dossier
 * analysis/befunde_runde36/G_tueren.md (G3). Die Stellen (Raum, Nachricht) -> Art liefert
 * gen/r36_aufschliessen.inc (tools/gen_r36_aufschliessen.py, 29 Stellen mit Datei-Offset je Message_on).
 *
 * Folge je Art (Satz der Mini-Bank KARTSE.VBS, Spielbild relativ zum Oeffnen der Nachricht):
 *   KARTE      Karte ein (0) / Piep (15) / Aufschliessen (30)   = RE2 ROOM2110 sub08 @0x01B4C / Sleep 15
 *                                                                 @0x01B58 / @0x01B5C / Sleep 15 @0x01B8C / @0x01B90
 *   KARTE_EIN  Karte ein (0)                                     = erster Schritt derselben Folge; in RE1.5-Raeumen
 *                                                                 mit Ziffernfeld kommt das Aufschliessen erst nach
 *                                                                 dem Code ("You've opened the lock.")
 *   KARTE_AUF  Piep (0) / Aufschliessen (15)                     = zweiter Teil (@0x01B5C, Sleep 15, @0x01B90)
 *   RIEGEL     Riegel von dieser Seite (0)                       = RE2 Tuer-Handler @0x800515f8 Se_on(2,0x26),
 *                                                                 im selben Bild wie der Text (@0x800515ec)
 * Die Zuordnung Nachricht -> Art (Wortlaut, Ziffernfeld im Raum) ist PORT-WAHL; belegt ist je Art die Folge. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_aufschliessen_r36.h"
#include "re15_engine.h"           /* g_engine.frame_count — nur fuer das Datei-Log */

#include "gen/r36_aufschliessen.inc"  /* re15_auf_stellen[], RE15_AUF_ART_*, RE15_AUF_STELLEN */
#include "gen/re2_karten_bank.inc"    /* RE2_KARTE_* (Satz-TOC KARTSE.VBS) */

/* RE2 ROOM2110 sub08: Sleep 15 (`09` + `0a 0f 00` @Datei 0x01B58 und 0x01B8C) zwischen den drei Se_on. */
#define RE2_KARTE_SLEEP 15

typedef struct { int n; int satz[3]; int bild[3]; } folge_t;

static const folge_t k_folgen[4] = {
    /* RE15_AUF_ART_KARTE_EIN */ { 1, { RE2_KARTE_SE_KARTE_EIN }, { 0 } },
    /* RE15_AUF_ART_KARTE     */ { 3, { RE2_KARTE_SE_KARTE_EIN, RE2_KARTE_SE_PIEP, RE2_KARTE_SE_AUF_E },
                                    { 0, RE2_KARTE_SLEEP, 2 * RE2_KARTE_SLEEP } },
    /* RE15_AUF_ART_KARTE_AUF */ { 2, { RE2_KARTE_SE_PIEP, RE2_KARTE_SE_AUF_E }, { 0, RE2_KARTE_SLEEP } },
    /* RE15_AUF_ART_RIEGEL    */ { 1, { RE2_KARTE_SE_AUF_HIER }, { 0 } },
};
#if RE15_AUF_ART_KARTE_EIN != 0 || RE15_AUF_ART_KARTE != 1 || RE15_AUF_ART_KARTE_AUF != 2 || RE15_AUF_ART_RIEGEL != 3
#error "gen/r36_aufschliessen.inc nummeriert die Arten anders als k_folgen[]"
#endif

static re15_auf_spieler_t s_spieler = NULL;
static struct { int aktiv, frisch, art, bild, naechster; unsigned room; int msg; } s_lauf;

unsigned g_re15_auf_zaehler = 0;
int      g_re15_auf_letzter = -1;

void re15_aufschliessen_r36_setze_spieler(re15_auf_spieler_t f) { s_spieler = f; }

int re15_aufschliessen_r36_stellen(void) { return RE15_AUF_STELLEN; }

int re15_aufschliessen_r36_stelle(int i, unsigned *room, int *msg, int *art, int *wege)
{
    if (i < 0 || i >= RE15_AUF_STELLEN) return -1;
    const re15_auf_stelle_t *s = &re15_auf_stellen[i];
    if (room) *room = s->room;
    if (msg)  *msg  = s->msg;
    if (art)  *art  = s->art;
    if (wege) *wege = s->wege;
    return 0;
}

int re15_aufschliessen_r36_folge(int art, int *satz, int *bild, int max)
{
    if (art < 0 || art > 3) return 0;
    const folge_t *f = &k_folgen[art];
    int n = f->n < max ? f->n : max;
    for (int k = 0; k < n; k++) { if (satz) satz[k] = f->satz[k]; if (bild) bild[k] = f->bild[k]; }
    return n;
}

void re15_karte_bank_rec(re15_karte_bank_rec_t *out)
{
    if (!out) return;
    out->edt_off  = RE2_KARTE_EDT_OFF;
    out->edt_size = RE2_KARTE_EDT_SIZE;
    out->vbd_off  = RE2_KARTE_VBD_OFF;
    out->vbd_size = RE2_KARTE_VBD_SIZE;
    out->se_count = RE2_KARTE_SE_COUNT;
}

/* Messschiene (die GUI-exe hat kein stderr): RE15_AUFSCHLIESS_LOG=<pfad> — je gespieltem Ton eine Zeile mit
 * Spielbild (fuer RE15_AUDIO_CAP_SYNC: Versatz = Bild * 1470 Stereo-Frames, wie RE15_TUERSE_LOG). */
static void protokoll(int satz, int versatz)
{
    static const char *s_pfad = NULL; static int s_init = 0;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_AUFSCHLIESS_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "ab");
    if (!f) return;
    static const char *namen[] = { "KARTE_EIN", "PIEP", "AUF_E", "AUF_HIER" };
    static const char *arten[] = { "KARTE_EIN", "KARTE", "KARTE_AUF", "RIEGEL" };
    fprintf(f, "F%u raum=%04X nachricht=%d art=%s satz=%d(%s) bild=+%d nr=%u\n", (unsigned)g_engine.frame_count,
            s_lauf.room, s_lauf.msg, (s_lauf.art >= 0 && s_lauf.art < 4) ? arten[s_lauf.art] : "?",
            satz, (satz >= 0 && satz < 4) ? namen[satz] : "?", versatz, g_re15_auf_zaehler);
    fclose(f);
}

static void faellige_spielen(void)
{
    const folge_t *f = &k_folgen[s_lauf.art];
    while (s_lauf.naechster < f->n && f->bild[s_lauf.naechster] <= s_lauf.bild) {
        int satz = f->satz[s_lauf.naechster];
        g_re15_auf_zaehler++;
        g_re15_auf_letzter = satz;
        protokoll(satz, f->bild[s_lauf.naechster]);
        /* RE2 Se_on 0x36: Bezugsobjekt pc[4] = 0 in ROOM2110 sub08 (`36 02 0c 00 00 00 ..`) bzw. a1 = 0 im
         * Tuer-Handler (@0x800515fc addu a1,zero,zero) -> nicht positional. */
        if (s_spieler) s_spieler(satz);
        s_lauf.naechster++;
    }
    if (s_lauf.naechster >= f->n) s_lauf.aktiv = 0;
}

int re15_aufschliessen_r36_notice(unsigned room_id, uint8_t msg_id, int weg)
{
    for (int i = 0; i < RE15_AUF_STELLEN; i++) {
        const re15_auf_stelle_t *s = &re15_auf_stellen[i];
        if (s->room != (uint16_t)room_id || s->msg != msg_id) continue;
        if (!(s->wege & weg)) return -1;
        s_lauf.aktiv = 1; s_lauf.frisch = 1; s_lauf.art = s->art; s_lauf.bild = 0; s_lauf.naechster = 0;
        s_lauf.room = room_id; s_lauf.msg = msg_id;
        faellige_spielen();                 /* Bild 0 im selben Bild wie der Text (RE2 @0x800515ec/@0x800515f8) */
        return s->art;
    }
    return -1;
}

void re15_aufschliessen_r36_tick(void)
{
    if (!s_lauf.aktiv) return;
    /* Das Bild, in dem die Nachricht aufging, zaehlt als Bild 0 (der Haken in scd_vm.c laeuft im selben
     * Spielschritt VOR diesem Tick): erst das NAECHSTE Bild ist +1 - sonst kaeme der Piep nach 14 statt
     * 15 Bildern (gemessen: probe_r36_tueren aufschliessen, ROOM10D0 +14 / ROOM3010 +14/+29). */
    if (s_lauf.frisch) { s_lauf.frisch = 0; return; }
    s_lauf.bild++;
    faellige_spielen();
}
