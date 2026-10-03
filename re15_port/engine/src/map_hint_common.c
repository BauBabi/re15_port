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

#include <stdio.h>
#include <string.h>

#include "re15_map_hint.h"
#include "re15_audio.h"
#include "re15_room.h"
#include "re15_scd.h"         /* re15_game_flag_get — Szenen-Flag des Hinweises (Runde 33) */
#include "re15_engine.h"     /* g_engine.frame_count — nur fuer die Messschiene */

#include "gen/re2_hint_bank.inc"   /* RE2_HINT_EDT_SIZE / _VBD_OFF / _VBD_SIZE / RE2_HINT_SE */

/* ---- Hinweis-Tabelle (Port-Gegenstueck zu RE2 @0x80011D20 / @0x800A9BA0) ------------ */
typedef struct {
    unsigned short quelle;         /* Raum, in dem der Anker gilt                         */
    unsigned char  sig[14];        /* Schwanz des Szenen-Skripts                          */
    unsigned char  pc_versatz;     /* Evt_end = sig + pc_versatz                          */
    unsigned short ziel_raum;      /* Raum, dessen Kachel blinkt                          */
    unsigned char  ziel_idx;       /* Zone dieses Raums                                   */
    /* Runde 33: das Spiel-Flag, das die Szene als GELAUFEN markiert — "der Hinweis ist
     * gezeigt worden" (Abschnitt 4 unten). */
    unsigned char  fertig_bank, fertig_bit;
    /* Runde 35 Spur K: FOLGE-Hinweis (Index in dieser Tabelle, -1 = keiner) und ob der Schirm
     * ZEITGESTEUERT weiterschaltet/schliesst ("kurz eine Weile", re15_cut10f0.h) — der Runde-33-
     * Eintrag bleibt wie bisher: nur START/Abbruch schliesst (RE2 @0x8006F884). */
    signed char    folge;
    unsigned char  zeitgesteuert;
    /* Runde 35 Spur K (Fortsetzung): eigenes "Ziel erreicht"-Flag (Bank, Bit) statt des Besucht-Bits
     * der Zielzone — fuer ein Ziel, das VOR dem Hinweis schon besucht war (ROOM1150 nach der
     * ROOM10F0-Szene: re15_cut10f0.h RE15_CUT10F0_ZIEL2_BESUCHT_*). Bank 0 = Besucht-Bit der Zone
     * (Runde-33-Regel, unveraendert). */
    unsigned char  erreicht_bank, erreicht_bit;
} re15_map_hint_eintrag_t;

static const re15_map_hint_eintrag_t s_hints[] = {
    /* ROOM1150.RDT @Datei 0x012E0 (1 Treffer in der Datei), Evt_end sub08 @0x012EC;
     * Ziel ROOM10F0 "COMMUNIC. ROOM" (DEBUG.BIN @0x027C0), Zone 0.
     * Szene gelaufen = flag(3,94): sub08 @0x01110 `22 03 5e 01` Set(3,94,1) am ANFANG der
     * Szene, main00 @0x00DE6 `21 03 5e 00` Ck(3,94)==0 legt die AUTO-Zone nur davor an. */
    { 0x1150,
      { 0x42, 0x00, 0x3c, 0x01, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x01, 0x00 },
      12, 0x10F0, 0, 3, 94, -1, 0, 0, 0 },
    /* Runde 35 Spur K: nach der ROOM10F0-Szene (Port-Programm cut_10f0.c, nicht im RDT-Puffer —
     * KEIN Anker, pc_versatz 0; Anforderung per re15_map_hint_request aus re15_cut10f0_tick).
     * Eintrag 1: Ziel ROOM11C0 "PARKING LOT" (Zone 0), danach Eintrag 2: Ziel ROOM1150 (Zone 0);
     * "Szene gesehen" = (9,71), gesetzt als erstes Opcode des Programms. Beide blinken in der
     * normalen Karte, bis ihr Zielort besucht ist (Abschnitt 4, re15_map_ziel_aktiv_n):
     * ROOM11C0 ueber das Besucht-Bit seiner Zone; ROOM1150 — vor der Szene laengst besucht (erste
     * Irons-Szene (3,94)) — ueber den Latch (9,72) "ROOM1150 NACH der Szene betreten"
     * (RE15_CUT10F0_ZIEL2_BESUCHT_BANK/_BIT, gesetzt in re15_cut10f0_install). */
    { 0x10F0, { 0 }, 0, 0x11C0, 0, 9, 71,  2, 1, 0, 0 },
    { 0x10F0, { 0 }, 0, 0x1150, 0, 9, 71, -1, 1, 9, 72 },
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
        if (e->pc_versatz == 0) continue;     /* Runde 35 Spur K: portseitig angeforderter Eintrag, kein Anker */
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

/* Runde 35 Spur K: portseitige Anforderung (Gegenstueck zu re15_map_hint_pc fuer Programme, die nicht
 * im RDT-Puffer liegen), Eintragssuche und die Folge-/Zeitsteuerung des Schirms. */
void re15_map_hint_request(int nr)
{
    if (nr >= 0 && nr < HINT_COUNT) s_pending = nr;
}

int re15_map_hint_eintrag_fuer(unsigned quelle, unsigned ziel_raum)
{
    for (int h = 0; h < HINT_COUNT; h++)
        if ((unsigned)s_hints[h].quelle == quelle && (unsigned)s_hints[h].ziel_raum == ziel_raum) return h;
    return -1;
}

int re15_map_hint_folge(int nr)
{
    return (nr >= 0 && nr < HINT_COUNT) ? (int)s_hints[nr].folge : -1;
}

int re15_map_hint_zeitgesteuert(int nr)
{
    return (nr >= 0 && nr < HINT_COUNT) ? (int)s_hints[nr].zeitgesteuert : 0;
}

int re15_map_hint_anzahl(void) { return HINT_COUNT; }

/* Die HAUPTZEILE des Zielraums in der Zonen-Tabelle (NULL = keine). */
static const re15_map_zone_t *ziel_zone(int nr)
{
    if (nr < 0 || nr >= HINT_COUNT) return NULL;
    const int n = re15_map_zone_count();
    for (int i = 0; i < n; i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        if (!zn) continue;
        if (zn->room != s_hints[nr].ziel_raum || zn->idx != s_hints[nr].ziel_idx) continue;
        if (zn->etage) continue;          /* Gastzeile auf fremdem Blatt: nicht das Ziel */
        if (zn->rect == 255) continue;    /* Schema-Zeichnung ohne Kachel                */
        return zn;
    }
    return NULL;
}

int re15_map_hint_ziel(int nr, int *page, int *rect)
{
    const re15_map_zone_t *zn = ziel_zone(nr);
    if (!zn) return 0;
    if (page) *page = (int)zn->page;
    if (rect) *rect = (int)zn->rect;
    return 1;
}

/* ==========================================================================================
 * 4. NACH DEM HINWEIS (Runde 33, Thema K) — ⛔ PORT-WAHL AUF NUTZERWUNSCH
 * ==========================================================================================
 * Nutzer (analysis/befunde_runde33/AUFTRAG.md): nach dem Hinweis muss man in der Karte zu
 * 2F wechseln koennen, auch ohne 2F betreten zu haben, und der Raum muss "angezeigt
 * bleiben, wie bei Resident Evil 2 bei Zielraeumen auch".
 *
 * RE2 TUT BEIDES NICHT (Dossier analysis/befunde_runde33/karte_zielraum.md §2):
 *  - Etagenwahl: ein Blatt ist in RE2 waehlbar, wenn sein Bit in Bank 35 (0x800D4908)
 *    steht (`jal 0x80077360` @0x8006D934 / @0x8006D998). Gesetzt wird es beim Betreten
 *    eines Raums (FUN_8006931C `addiu a0,s0,234` @0x800693B0, `jal 0x8007730c` @0x800693B4)
 *    und per Skript bei der Kartenaufnahme (ROOM20B0 sub10 Set(35,2..4) @0x037E6-EE) —
 *    NICHT vom Hinweis (Handler @0x800591C4 schreibt vier Dinge, Modus 4 hat 0 Bit-Setzer).
 *  - Zielraum: RE2s normaler Zeichner FUN_8006E120 hat keinen Zustand "Ziel" (Zeilen
 *    501/506 +1, 498/503, Raumschleife @0x8006E46C-0x8006E770); das Blinken gibt es nur
 *    im Hinweis-Modus 4, der nichts hinterlaesst.
 *
 * DER PORT, auf den ausdruecklichen Wunsch:
 *  (a) Das Blatt des Zielraums wird mit dem Hinweis waehlbar — das Gegenstueck zu RE2s
 *      Skript-Set auf Bank 35 (RE2s eigenes Mittel, ein Blatt ohne Besuch freizugeben).
 *      Das Gatter selbst bleibt RE2s Regel: jedes andere Blatt nur nach Besuch.
 *  (b) Der Zielraum blinkt in der normalen Karte weiter wie im Hinweis (CLUT 502 / 498),
 *      bis er BESUCHT ist, OHNE den Hinweis-Ton (RE2s normale Karte spielt ihn nicht: dort
 *      nur Se(4,9) @0x8006D7EC, Se(4,4) @0x8006D9D8, Se(4,5) @0x8006D9FC). Der Takt ist
 *      der des Zaehlers, den RE2s normaler Kartenschirm in Zustand 3 faehrt
 *      (@0x8006D87C-0x8006D8D4) — derselbe Zaehler, dieselben Schwellen wie im Hinweis-
 *      Zeichner (@0x8006F20C-0x8006F284), also zaehl_schritt oben.
 *
 * ⛔ KEIN NEUER ZUSTAND, KEIN SPEICHERFELD. "Hinweis gezeigt" ist das Szenen-Flag des
 * Tabelleneintrags (ROOM1150: (3,94), gesetzt am Anfang der Szene @0x01110 — gespeichert
 * ist es mit g_game.flags seit v1), "Ziel erreicht" ist das Besucht-Bit des Zielorts
 * (gespeichert seit v6; Leons ROOM10F0 und Elzas ROOM10F1 teilen es, zone_bit maskiert
 * die Varianten). Ein eigenes Feld waere eine zweite Wahrheit — und seine Hebung fuer
 * alte Staende muesste genau diese beiden Bits lesen. Die Flag-Wahl ist gemessen: (3,94)
 * kommt in allen 240 RDTs nur in ROOM1150 vor (main00 @0x00DE6, sub08 @0x01110;
 * analysis/befunde_runde33/karte_werkzeug/r33_re15_flag_zensus.py). Zwischen dem Setzen
 * am Szenenanfang und dem Hinweis am Szenenende kann nicht gespeichert werden (die Szene
 * haelt den Spieler, flag(1,27)/(2,7) @0x01114/@0x01118). */
static int hint_gezeigt(int nr)
{
    if (nr < 0 || nr >= HINT_COUNT) return 0;
    return re15_game_flag_get(s_hints[nr].fertig_bank, s_hints[nr].fertig_bit) != 0;
}

int re15_map_ziel_blatt_frei(unsigned page)
{
    for (int h = 0; h < HINT_COUNT; h++) {
        const re15_map_zone_t *zn;
        if (!hint_gezeigt(h)) continue;
        zn = ziel_zone(h);
        if (zn && (unsigned)zn->page == page) return 1;
    }
    return 0;
}

int re15_map_blatt_waehlbar(unsigned page)
{
    return re15_map_page_known(page) || re15_map_ziel_blatt_frei(page);
}

/* Runde 35 Spur K: das k-te (0-basiert) markierte Ziel — mehrere Ziele zugleich (ROOM11C0 und
 * ROOM1150 nach der ROOM10F0-Szene, "So lange der Raum nicht besucht ist, blinken beide weiter"). */
int re15_map_ziel_aktiv_n(int k, int *page, int *rect)
{
    int n = 0;
    for (int h = 0; h < HINT_COUNT; h++) {
        const re15_map_zone_t *zn;
        if (!hint_gezeigt(h)) continue;
        zn = ziel_zone(h);
        if (!zn) continue;
        if (s_hints[h].erreicht_bank                      /* erreicht -> aus: eigener Latch ... */
                ? re15_game_flag_get(s_hints[h].erreicht_bank, s_hints[h].erreicht_bit) != 0
                : re15_map_zone_visited(zn)) continue;    /* ... sonst das Besucht-Bit der Zone */
        if (n++ != k) continue;
        if (page) *page = (int)zn->page;
        if (rect) *rect = (int)zn->rect;
        return 1;
    }
    return 0;
}

int re15_map_ziel_aktiv(int *page, int *rect)
{
    return re15_map_ziel_aktiv_n(0, page, rect);
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
    fprintf(stderr, "[hint] F%u begin t0=%llu us (Zaehler %d, Richtung %d)\n",
            (unsigned)g_engine.frame_count, (unsigned long long)s_start_us,
            (int)s_zaehler, (int)s_richtung);
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
     * geht; mehr als einer je Bild wird ohnehin nicht gespielt.
     * Port-Wahl, keine Original-Adresse: RE2 holt nichts nach (ein Zaehlschritt je
     * Durchgang, ein langsamer Durchgang verliert Zeit); "zwei Perioden" und "ein Ton je
     * Bild" gelten nur fuer Stillstaende. Messung: unit_r30_hinweis_fsm, 10 s Stillstand
     * -> 598 Schritte = floor(10 s * 59,826), 1 Ton. */
    {
        const uint64_t p = (uint64_t)re15_map_hint_periode();
        if (p > 0 && due > 2 * p) {
            uint64_t weg = ((due - p) / p) * p;
            s_schritte += weg;
            due -= weg;
        }
    }
    {
        int frei = 1, rot_vorher = (s_richtung == 0);
        while (due--) if (schritt(frei)) frei = 0;
        /* Messschiene fuer die Abnahme (debug.log): Wanduhr-Zeit jedes Phasenwechsels und
         * jedes Tons, relativ zum Nullpunkt des Hinweises. */
        if ((s_richtung == 0) != rot_vorher || !frei)
            fprintf(stderr, "[hint] F%u t=%llu us schritt=%llu %s%s\n",
                    (unsigned)g_engine.frame_count,
                    (unsigned long long)(now - s_start_us), (unsigned long long)s_schritte,
                    (s_richtung == 0) ? "rot" : "umriss", frei ? "" : " +Ton Se(2,0x2B)");
    }
}

/* ---- Runde 33: der Blinker der ZIELKACHEL in der normalen Karte (Abschnitt 4 b) ---------
 * Eigener Zaehler, damit der Hinweis-Zaehler (und sein Ton-Protokoll) unberuehrt bleibt.
 * Schritt = zaehl_schritt, also RE2s Zaehler Befehl fuer Befehl (normaler Kartenschirm
 * Zustand 3 @0x8006D87C-0x8006D8D4 = Hinweis-Zeichner @0x8006F20C-0x8006F284), ein Schritt
 * je VBlank (Teiler 0 @0x80068A1C), auf der Wanduhr wie der Hinweis. KEIN Ton: der Rueck-
 * gabewert von zaehl_schritt wird verworfen (RE2s normale Karte ruft Se(2,0x2B) nie).
 * Start mit den Werten des Hinweis-Inits (Zaehler 10 @0x8006F6DC, Richtung 1 @0x8006F6B4):
 * PORT-WAHL — RE2s normaler Schirm setzt den geteilten Zaehler beim Oeffnen nicht neu
 * (Schreiber nur @0x8006D8A0/C4/D4 in FUN_8006D650), er laeuft dort mit dem Stand weiter,
 * den der letzte Modus hinterliess; der Port beginnt jede Kartenansicht wie der Hinweis,
 * damit die Kachel so auftaucht, wie der Spieler sie aus dem Hinweis kennt (rot ab dem
 * zweiten Schritt). Nach einem Stillstand wird wie beim Hinweis ueber die Periode
 * gefaltet. */
static uint8_t  s_zb_zaehler  = RE15_HINT_ZAEHLER_START;
static uint8_t  s_zb_richtung = RE15_HINT_RICHTUNG_START;
static uint64_t s_zb_start_us = 0;
static uint64_t s_zb_schritte = 0;

void re15_map_ziel_blink_begin(void)
{
    s_zb_zaehler  = RE15_HINT_ZAEHLER_START;
    s_zb_richtung = RE15_HINT_RICHTUNG_START;
    s_zb_start_us = re15_host_clock_us();
    s_zb_schritte = 0;
}

void re15_map_ziel_blink_tick(void)
{
    uint64_t now = re15_host_clock_us();
    uint64_t total = (now > s_zb_start_us) ? re15_map_hint_vblanks(now - s_zb_start_us) : 0;
    if (total <= s_zb_schritte) return;
    uint64_t due = total - s_zb_schritte;
    const uint64_t p = (uint64_t)re15_map_hint_periode();
    if (p > 0 && due > p) {                         /* Stillstand: ueber die Periode falten */
        uint64_t weg = (due / p) * p;
        s_zb_schritte += weg;
        due -= weg;
    }
    while (due--) { (void)zaehl_schritt(&s_zb_zaehler, &s_zb_richtung); s_zb_schritte++; }
}

int      re15_map_ziel_blink_rot(void)      { return s_zb_richtung == 0; }
uint64_t re15_map_ziel_blink_schritte(void) { return s_zb_schritte; }

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
