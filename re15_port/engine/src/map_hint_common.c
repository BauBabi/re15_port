/* ============================================================================
 * map_hint_common.c — KARTENHINWEIS NACH DER IRONS-SZENE
 * ⛔ DAS IST EINE RE2-ERGAENZUNG. RE1.5 HAT DIESEN MECHANISMUS NICHT.
 * ============================================================================
 * Begruendung, RE2-Mechanismus und Konstanten-Belege: include/re15_map_hint.h und
 * analysis/befunde_runde30/karte-3010.md. Hier stehen der AUSLOESER (Anker im Skript),
 * das ZIEL und der BLINKER.
 *
 * 1. DER AUSLOESER
 * ----------------
 * RE2 setzt den Hinweis per Opcode 0x84 UNMITTELBAR vor das Evt_end des Szenen-Skripts
 * (ROOM30B0.RDT @0x01A86 `84 01`, direkt hinter dem Abbau der Szenen-Flags
 * `22 02 07 00` / `22 01 1b 00` und `3c 01`; ROOM3010.RDT @0x026EE `84 02 01 00`).
 * RE1.5s ROOM1150 sub08 endet in derselben Schwanzform, nur ohne den Hinweis
 * (ROOM1150.RDT, selbst gelesen):
 *     @0x012E0  42            Plc_ret
 *     @0x012E1  00            Nop
 *     @0x012E2  3c 01         Cut_auto(1)
 *     @0x012E4  22 02 07 00   Set(2,7,0)
 *     @0x012E8  22 01 1b 00   Set(1,27,0)
 *     @0x012EC  01 00         Evt_end         <- hier gehoert der Hinweis hin
 * Der Anker ist diese 14-Byte-Folge, gesucht im GELADENEN RDT-Puffer; ausgeloest wird,
 * wenn der Programmzeiger auf ihrem Evt_end steht (Signatur + 12).
 *
 * ⛔ DIE SIGNATUR ALLEIN IST NICHT EINDEUTIG. Gemessen ueber alle 240 RE1.5-RDTs: 11
 * Treffer in 8 Dateien (ROOM1021 @0x21B0, ROOM1050 @0x0DF2, ROOM1150 @0x12E0, ROOM3070
 * @0x35B8, ROOM3071 @0x36A9/@0x37F3, ROOM30E0 @0x0E54, ROOM4010 @0x0D84/@0x0F68, ROOM5011
 * @0x092C/@0x0DFC) — es ist die gewoehnliche Schwanzform einer Szene. Der Anker wird
 * deshalb NUR im Quellraum 0x1150 gesetzt (dort 1 Treffer). ROOM1151 (Elza) hat die
 * Szene gar nicht (8 Subs, kein sub08) und bekommt keinen Hinweis (Auftrag: ROOM 1150).
 *
 * EINHAENGEPUNKT: wie beim Fahrstuhl-Ton die EINE Stelle im SCD-Verteiler, an der der
 * Programmzeiger ohnehin gelesen wird, hinter dem Wachposten g_re15_map_hint_anchor_n
 * (in 239 von 240 Raeumen 0). op_evt_end selbst bleibt unberuehrt.
 *
 * 2. DAS ZIEL
 * -----------
 * "communication ROOM" = ROOM10F0, Raumname "COMMUNIC. ROOM" (DEBUG.BIN @Datei 0x027C0,
 * `01 00 d0 20 a2 fe 00 00` + Name; Satzindex-Formel @0x8001D39C-C8). RE2 fuehrt das
 * Blatt je Hinweis FEST (Sprungtabelle @0x80011D20, Blaetter 5/3/5/16/3) und den Zielraum
 * als Byte (@0x800A9BA0). Der Port leitet das Blatt ab: seine Zonen-Tabelle traegt es
 * schon. Gesucht wird die HAUPTZEILE (etage == 0) — ROOM10F0 hat daneben eine Gastzeile
 * auf Blatt 2 (etage == 1). Blatt und Rechteck stehen damit NICHT als Zahl im Code; der
 * Riegel unit_r30_hinweis_zeichner haelt Blatt 3 / Rechteck 9 fest.
 *
 * 3. DER BLINKER
 * --------------
 * RE2 FUN_8006F1C4 @0x8006F20C-0x8006F284, einmal je Durchgang = je VBlank (Takt-Beleg
 * in re15_map_hint.h). Der Port rechnet die Zahl der faelligen Schritte aus der
 * vergangenen WANDZEIT (re15_host_clock_us) — so haengt der Blinker nicht an der
 * Bildrate der Anzeige. Die Rechnung ist die der Titel-Uhr (title_pulse.c der Titel-Spur,
 * re15_title_tick_count): floor(Zeit * 59826 mHz / (VBlanks je Schritt * 10^9)).
 * ==========================================================================*/

#include <string.h>

#include "re15_map_hint.h"
#include "re15_audio.h"
#include "re15_room.h"

#include "gen/re2_hint_bank.inc"   /* RE2_HINT_EDT_SIZE / _VBD_OFF / _VBD_SIZE / RE2_HINT_SE */

/* ---- Hinweis-Tabelle (Port-Gegenstueck zu RE2 @0x80011D20 / @0x800A9BA0) ------------ */
typedef struct {
    unsigned short quelle;         /* Raum, in dem der Anker gilt                         */
    unsigned char  sig[14];        /* Schwanz des Szenen-Skripts                          */
    unsigned char  pc_versatz;     /* Evt_end = sig + pc_versatz                          */
    unsigned short ziel_raum;      /* Raum, dessen Kachel blinkt                          */
    unsigned char  ziel_idx;       /* Zone dieses Raums                                   */
} re15_map_hint_eintrag_t;

static const re15_map_hint_eintrag_t s_hints[] = {
    /* ROOM1150.RDT @Datei 0x012E0 (1 Treffer in der Datei), Evt_end sub08 @0x012EC;
     * Ziel ROOM10F0 "COMMUNIC. ROOM" (DEBUG.BIN @0x027C0), Zone 0. */
    { 0x1150,
      { 0x42, 0x00, 0x3c, 0x01, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x01, 0x00 },
      12, 0x10F0, 0 },
};
#define HINT_COUNT ((int)(sizeof s_hints / sizeof s_hints[0]))

int g_re15_map_hint_anchor_n = 0;

static const unsigned char *s_anchor_pc  = NULL;
static const unsigned char *s_anchor_raw = NULL;
static int                  s_anchor_nr  = -1;
static int                  s_pending    = -1;

void re15_map_hint_room_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_map_hint_anchor_n = 0;
    s_anchor_pc  = NULL;
    s_anchor_raw = NULL;
    s_anchor_nr  = -1;
    s_pending    = -1;            /* eine Anforderung gehoert zu IHREM Raum */
    if (raw == NULL || raw_size <= 0) return;

    for (int h = 0; h < HINT_COUNT; h++) {
        const re15_map_hint_eintrag_t *e = &s_hints[h];
        const int len = (int)sizeof e->sig;
        if (room_id != (unsigned)e->quelle) continue;
        for (int i = 0; i + len <= raw_size; i++) {
            if (raw[i] != e->sig[0]) continue;
            if (memcmp(raw + i, e->sig, (size_t)len) != 0) continue;
            s_anchor_pc  = raw + i + e->pc_versatz;
            s_anchor_raw = raw;
            s_anchor_nr  = h;
            g_re15_map_hint_anchor_n = 1;
            return;               /* im Quellraum genau 1 Treffer (Riegel) */
        }
    }
}

void re15_map_hint_pc(const unsigned char *pc)
{
    if (pc == NULL || pc != s_anchor_pc) return;
    s_pending = s_anchor_nr;      /* RE2: Handler 0x84 @0x800591C4 setzt die Anforderung */
}

long re15_map_hint_anchor_off(void)
{
    if (!s_anchor_pc || !s_anchor_raw) return -1;
    return (long)(s_anchor_pc - s_anchor_raw);
}

int  re15_map_hint_pending(void) { return s_pending; }
void re15_map_hint_take(void)    { s_pending = -1; }

int re15_map_hint_ziel(int nr, int *page, int *rect)
{
    if (nr < 0 || nr >= HINT_COUNT) return 0;
    const int n = re15_map_zone_count();
    for (int i = 0; i < n; i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        if (!zn) continue;
        if (zn->room != s_hints[nr].ziel_raum || zn->idx != s_hints[nr].ziel_idx) continue;
        if (zn->etage) continue;          /* Gastzeile auf fremdem Blatt: nicht das Ziel */
        if (zn->rect == 255) continue;    /* Schema-Zeichnung ohne Kachel                */
        if (page) *page = (int)zn->page;
        if (rect) *rect = (int)zn->rect;
        return 1;
    }
    return 0;
}

/* ---- Wanduhr -------------------------------------------------------------------------- */
static uint64_t s_host_us = 0;
void     re15_host_clock_set_us(uint64_t now_us) { s_host_us = now_us; }
uint64_t re15_host_clock_us(void)                { return s_host_us; }

uint64_t re15_map_hint_vblanks(uint64_t elapsed_us)
{
    /* Schritte = Zeit * VBlank-Rate / VBlanks je Schritt
     *          = elapsed_us * 59826 mHz / (1 * 1000 mHz/Hz * 10^6 us/s).
     * 64 Bit reichen fuer elapsed_us < 2^64 / 59826 (knapp 10 Jahre). */
    const uint64_t den = (uint64_t)RE15_HINT_VBLANKS_JE_SCHRITT * 1000ull * 1000000ull;
    return (elapsed_us * (uint64_t)RE15_HINT_VBLANK_MILLIHZ) / den;
}

/* ---- Blinker -------------------------------------------------------------------------- */
/* Zaehler [0x800D5C18] und Richtung [0x800D5C19] — im Original Bytes (`lbu`/`sb`). */
static uint8_t  s_zaehler  = RE15_HINT_ZAEHLER_START;
static uint8_t  s_richtung = RE15_HINT_RICHTUNG_START;
static uint64_t s_start_us = 0;
static uint64_t s_schritte = 0;

/* Ein Schritt des Zaehlers, OHNE Ton — der gemeinsame Kern von step und periode. Gibt 1
 * zurueck, wenn RE2 an dieser Stelle den Ton spielt. */
static int zaehl_schritt(uint8_t *zaehler, uint8_t *richtung)
{
    unsigned z = *zaehler;
    int ton = 0;
    if (*richtung != 0) {                          /* beq v0,zero,0x8006f258 @0x8006F218  */
        if (z < RE15_HINT_ZAEHLER_UNTEN) {         /* sltiu v0,v0,0xa @0x8006F22C          */
            ton = 1;                               /* lui a0,0x22b @0x8006F234 / jal @0x8006F238 */
            *richtung = 0;                         /* sb zero,[0x800D5C19] @0x8006F244     */
        }
        z -= RE15_HINT_SCHRITT;                    /* addiu v0,v0,-2 @0x8006F254           */
    } else {
        if (!(z < RE15_HINT_ZAEHLER_OBEN))         /* sltiu v0,v1,0x51 @0x8006F264         */
            *richtung = 1;                         /* sb v0,[0x800D5C19] @0x8006F278       */
        z += RE15_HINT_SCHRITT;                    /* addiu v0,v1,2 @0x8006F26C/@0x8006F27C */
    }
    *zaehler = (uint8_t)z;                         /* sb v0,[0x800D5C18] @0x8006F284       */
    return ton;
}

/* Ein Schritt samt Ton; ton_erlaubt = 0 unterdrueckt nur die Ausgabe (hoechstens ein Ton
 * je re15_map_hint_tick), der Zustand laeuft unveraendert. 1 = hier faellt der Ton. */
static int schritt(int ton_erlaubt)
{
    int ton = zaehl_schritt(&s_zaehler, &s_richtung);
    if (ton && ton_erlaubt)
        re15_audio_re2_hint_se(RE2_HINT_SE);      /* Se(2,0x2B) @0x8006F234-38            */
    s_schritte++;
    return ton;
}

void re15_map_hint_step(void) { (void)schritt(1); }

int re15_map_hint_periode(void)
{
    /* Die Folge aus dem Startzustand nachspielen, bis er wiederkehrt. Der Startzustand
     * liegt selbst auf dem Zyklus (Dossier 3.5 a: Ton-Schritte 2, 80, 158, ...). */
    static int s_periode = 0;
    if (s_periode == 0) {
        uint8_t z = RE15_HINT_ZAEHLER_START, r = RE15_HINT_RICHTUNG_START;
        int n = 0;
        do { (void)zaehl_schritt(&z, &r); n++; }
        while ((z != RE15_HINT_ZAEHLER_START || r != RE15_HINT_RICHTUNG_START) && n < 4096);
        s_periode = n;
    }
    return s_periode;
}

void re15_map_hint_begin(void)
{
    s_zaehler  = RE15_HINT_ZAEHLER_START;          /* @0x8006F6DC / @0x8006F6F4 */
    s_richtung = RE15_HINT_RICHTUNG_START;         /* @0x8006F6B4 / @0x8006F6C4 */
    s_start_us = re15_host_clock_us();
    s_schritte = 0;
}

void re15_map_hint_tick(void)
{
    uint64_t now = re15_host_clock_us();
    uint64_t total = (now > s_start_us) ? re15_map_hint_vblanks(now - s_start_us) : 0;
    if (total <= s_schritte) return;               /* Uhr steht oder lief rueckwaerts */
    uint64_t due = total - s_schritte;
    /* Nach einem Stillstand (Fenster gezogen, Haltepunkt) nicht Tausende Schritte
     * nachholen: die Folge ist periodisch, der Zustand nach n Schritten ist der nach
     * n - k*Periode. Bis zu zwei Perioden bleiben stehen, damit ein Ton nicht verloren
     * geht; mehr als einer je Bild wird ohnehin nicht gespielt. */
    {
        const uint64_t p = (uint64_t)re15_map_hint_periode();
        if (p > 0 && due > 2 * p) {
            uint64_t weg = ((due - p) / p) * p;
            s_schritte += weg;
            due -= weg;
        }
    }
    {
        int frei = 1;
        while (due--) if (schritt(frei)) frei = 0;
    }
}

int      re15_map_hint_rot(void)      { return s_richtung == 0; }
int      re15_map_hint_zaehler(void)  { return (int)s_zaehler; }
int      re15_map_hint_richtung(void) { return (int)s_richtung; }
uint64_t re15_map_hint_schritte(void) { return s_schritte; }

/* ---- Satz-TOC der Mini-Bank (gen/re2_hint_bank.inc) ------------------------------- */
void re15_map_hint_bank_rec(re15_map_hint_bank_rec_t *out)
{
    if (!out) return;
    out->edt_off   = RE2_HINT_EDT_OFF;
    out->edt_size  = RE2_HINT_EDT_SIZE;
    out->vbd_off   = RE2_HINT_VBD_OFF;
    out->vbd_size  = RE2_HINT_VBD_SIZE;
    out->vag1_size = RE2_HINT_VAG1_SIZE;
    out->se_hint   = RE2_HINT_SE;
}
