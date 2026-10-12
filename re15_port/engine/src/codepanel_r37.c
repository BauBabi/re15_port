/* codepanel_r37.c — Runde 37 Spur E: Lampen und Kamera der drei Ziffern-Code-Panels.
 *
 * Belege, Lesart und Herleitung: include/re15_codepanel_r37.h und analysis/befunde_runde37/E_codepanel.md.
 * Kein Raum ausser ROOM10D0/10D1/11E0/11E1/1230/1231 wird beruehrt (Anker nur dort, Lampe nur im Panel-Cut). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_codepanel_r37.h"
#include "re15_aufschliessen_r36.h"   /* g_re15_auf_zaehler, Stellen-/Folgen-Tabelle (Runde 36 Spur G) */
#include "re15_scd.h"                 /* scd_thread_t, re15_game_flag_get */
#include "re15_engine.h"              /* g_engine.frame_count — nur fuer das Datei-Log */

extern uint16_t g_current_room_id;
/* Die ORIGINAL-Handler (scd_vm.c): Cut_old = RE1.5 0x8004032c (Tabelle @0x80074550), Cut_auto = 0x800403ac
 * (Tabelle @0x80074598). Sie laufen spaeter, aber auf den Original-Bytes X / X+2. */
extern int op_cut_old(scd_thread_t *t);
extern int op_cut_auto(scd_thread_t *t);

/* ---- Tabelle -------------------------------------------------------------------------------------------
 * Cut      = sub17 Cut_chg: 10D0 @0x018D6 `29 0b`, 10D1 @0x018C0 `29 0b`, 11E0/11E1 @0x01EBE `29 0f`,
 *            1230/1231 @0x0139E `29 09`.
 * Flag     = sub01 Set(3,F,1): 10D0 @0x0152A `22 03 32 01`, 10D1 @0x01514, 11E0/11E1 @0x01A8A `22 03 8b 01`,
 *            1230/1231 @0x00FF2 `22 03 89 01` (jeweils hinter Evt_exec sub19, Ck(3,F,0)+Ck(5,13..16,1)).
 * X        = sub19 Cut_old (Signatur k_sig, 18 B, in allen sechs Raeumen gleich).
 * Lampen   = Glasmitte, an den Panel-Cuts gemessen (Farbmaske max-Kanal minus Rest >= 20 bzw. 12, Dossier §0):
 *            10D0 Cut 11: rot x 116..127 / y 72..81 -> (121.5, 76.5), blau x 164..173 / y 72..81 -> (168.5, 76.5);
 *            11E0 Cut 15 == 1230 Cut 9: rot x 122..135 / y 166..183 -> (128.5, 174.5), gruen x 174..187 -> (180.5, 174.5).
 *            zu = die ROT gemalte Lampe leuchtet ROT, offen = die andere gemalte Lampe in IHRER Farbe (Nachtraege 3/3b,
 *            RE2 ROOM4070 @0x008C6 rot / @0x0088C blau, ROOM2130 @0x01774 rot / @0x017A8 gruen). */
static const re15_cp_panel_t k_panels[RE15_CP_RAEUME] = {
    { 0x10D0u, 11,  50, 0x0199Au, RE15_CP_FORM_RUND,    243, 153, RE15_CP_ZEILE_ROT, 337, 153, RE15_CP_ZEILE_BLAU  },
    { 0x10D1u, 11,  50, 0x01984u, RE15_CP_FORM_RUND,    243, 153, RE15_CP_ZEILE_ROT, 337, 153, RE15_CP_ZEILE_BLAU  },
    { 0x11E0u, 15, 139, 0x01F82u, RE15_CP_FORM_QUADRAT, 257, 349, RE15_CP_ZEILE_ROT, 361, 349, RE15_CP_ZEILE_GRUEN },
    { 0x11E1u, 15, 139, 0x01F82u, RE15_CP_FORM_QUADRAT, 257, 349, RE15_CP_ZEILE_ROT, 361, 349, RE15_CP_ZEILE_GRUEN },
    { 0x1230u,  9, 137, 0x01462u, RE15_CP_FORM_QUADRAT, 257, 349, RE15_CP_ZEILE_ROT, 361, 349, RE15_CP_ZEILE_GRUEN },
    { 0x1231u,  9, 137, 0x01462u, RE15_CP_FORM_QUADRAT, 257, 349, RE15_CP_ZEILE_ROT, 361, 349, RE15_CP_ZEILE_GRUEN },
};

/* sub19 ab X (selbst gelesen in allen sechs RDTs): Cut_old, Nop, Cut_auto 1, Message_on 5 (Maske 0xffff), Evt_next,
 * Nop, Set(1,27,0), Set(2,7,0), Evt_end. */
static const uint8_t k_sig[RE15_CP_SIG_LAENGE] = {
    0x2a, 0x00, 0x3c, 0x01, 0x2b, 0x05, 0xff, 0xff, 0x02, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x22, 0x02, 0x07, 0x00 };
#define CP_MSG_ID 5   /* X+4 `2b 05` */

/* ---- Groesse (PORT-WAHL, abgeleitet nach dem Muster RE15_PANEL_LAMPE_KANTE, re15_panel_zeiger.h) ---------
 * Bildgroesse eines RE2-ESP-Sprites = S * scale16 * H / (SZ * 256) (FUN_80077ed0 @0x80077f14..0x8007800c), dann mal
 * dem Verhaeltnis der gemalten Glasflaechen RE1.5 / RE2 ((b1/b2 + h1/h2) / 2):
 *   RUND: RE2 ROOM4070 Cut 6 (camera.rid), Lampe blau @0x00DC2 (scale16 0x0900 = pc[6..7]) -> Zelle 5 (S 0x18) 31,0 px
 *         auf Glas 7 x 6 px; rot @0x008C6 -> 30,2 px auf 8 x 6 px (tools/r37_e/re2_lampen_projektion.py).
 *         10D0 blau 10 x 10 -> 31,0 * (10/7 + 10/6)/2 = 48,0; rot 12 x 10 -> 30,2 * (12/8 + 10/6)/2 = 47,8 -> 48 px
 *         fuer S 24, also 2 px je Texel; die Zellen 6/7 (S 0x20) -> 64 px.
 *   QUADRAT: RE2 ROOM2130 Cut 6, Schalterlampe @0x01294 (scale16 0x02BA) -> 25,8 px auf Glas 17 x 15 px;
 *         SECUGUARD 14 x 18 -> 25,8 * (14/17 + 18/15)/2 = 26,1 -> 26 px (S 0x20). */
#define CP_RUND_PX_JE_TEXEL 2
#define CP_QUADRAT_KANTE    26

/* ---- Animation (ESP-0x16-Block, ROOM2130 effect.esp Saetze @0x10.. == ROOM2110 effect.esp @0xC04..) ---------
 *   Quadrat (Strom 2): Satz 4 `03 01 01 20`, Satz 5 `04 01 01 20`, Satz 6 `04 01 ff 20` (Schleife -> Satz 4)
 *   rund    (Strom 4): Satz 7 `05 01 01 18`, Satz 8 `07 01 01 20`, Satz 9 `06 01 01 20`, Satz 10 `07 01 01 20`,
 *                      Satz 11 `07 01 ff 20` (Schleife -> Satz 7)
 * Strom -> erster Satz: Strom-Tabelle @0x90 `04 00 12 00 20 00 2e 00 3c 00 4a 00` (x4 + 0x90), je 8 B Kopf, dann Zeile 0,
 * deren Byte 2 der Satz ist: Strom 2 @0x110 -> Zeile @0x118 `01 00 04 00 ..` = Satz 4; Strom 4 @0x180 -> @0x188
 * `01 00 07 00 ..` = Satz 7; beide mit Flags `03 ba` (Zeile+18 = 0xBA03, additiv wie die Generator-Lampe).
 * Dauer 1 = je Bild eine Zelle (FUN_8001d68c @0x8001d7b8..0x8001d880); Zelle des ersten Satzes im Einschaltbild
 * (Routine 1 @0x8001dc40/0x8001dc4c, Generator-Beleg C_generator.md §3.7). */
static const uint8_t k_rund_zelle[4] = { 5, 7, 6, 7 };
static const uint8_t k_rund_s[4]     = { 24, 32, 32, 32 };
static const uint8_t k_quad_zelle[2] = { 3, 4 };

/* ---- Zustand -------------------------------------------------------------------------------------------- */
int g_re15_codepanel_anker_n = 0;
static const re15_cp_panel_t *s_panel = NULL;
static const unsigned char   *s_x     = NULL;    /* armierter Anker X im rohen RDT */
static int      s_halt = 0;                       /* Kamera wird gehalten (X..X+3 uebersprungen) */
static unsigned s_auf0 = 0;                       /* g_re15_auf_zaehler beim Halten */
static int      s_folge_n = 0, s_folge_letztes = 0;
static int      s_warte = 0, s_warte_letzt = 0;
static unsigned s_freigaben = 0;
static int      s_lampe_zustand = 0;              /* 0 aus, 1 zu, 2 offen */
static unsigned s_lampe_ein = 0;                  /* Bild des Einschaltens (Takt 0) */

static void logf_(const char *fmt, unsigned a, unsigned b, unsigned c)
{
    static const char *s_pfad = NULL; static int s_init = 0;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_CODEPANEL_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "ab");
    if (!f) return;
    fprintf(f, "F%u raum=%04X ", (unsigned)g_engine.frame_count, (unsigned)g_current_room_id);
    fprintf(f, fmt, a, b, c);
    fputc('\n', f);
    fclose(f);
}

const re15_cp_panel_t *re15_codepanel_r37_panel(int i)
{
    return (i >= 0 && i < RE15_CP_RAEUME) ? &k_panels[i] : NULL;
}

const re15_cp_panel_t *re15_codepanel_r37_raum(unsigned room_id)
{
    for (int i = 0; i < RE15_CP_RAEUME; i++)
        if (k_panels[i].raum == (uint16_t)room_id) return &k_panels[i];
    return NULL;
}

/* Laenge und letztes Bild der Aufschliess-Folge an (Raum, Nachricht 5) — aus der Runde-36-Tabelle
 * (gen/r36_aufschliessen.inc: KARTE_AUF = Piep +0 / Aufschliessen +15). Keine Stelle -> 0 (nichts abzuwarten). */
static void folge_bestimmen(unsigned room_id)
{
    s_folge_n = 0; s_folge_letztes = 0;
    for (int i = 0; i < re15_aufschliessen_r36_stellen(); i++) {
        unsigned r = 0; int msg = -1, art = -1, wege = 0;
        if (re15_aufschliessen_r36_stelle(i, &r, &msg, &art, &wege) != 0) continue;
        if (r != room_id || msg != CP_MSG_ID || !(wege & RE15_AUF_WEG_SKRIPT)) continue;
        int bild[3] = { 0, 0, 0 };
        s_folge_n = re15_aufschliessen_r36_folge(art, NULL, bild, 3);
        s_folge_letztes = (s_folge_n > 0) ? bild[s_folge_n - 1] : 0;
        return;
    }
}

void re15_codepanel_r37_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_codepanel_anker_n = 0;
    s_panel = re15_codepanel_r37_raum(room_id);
    s_x = NULL; s_halt = 0; s_warte = 0;
    s_lampe_zustand = 0;
    if (!s_panel || !raw || raw_size <= 0) return;
    if ((long)s_panel->sub19_x + RE15_CP_SIG_LAENGE > (long)raw_size) return;
    if (memcmp(raw + s_panel->sub19_x, k_sig, sizeof k_sig) != 0) {
        logf_("Anker @0x%05X: Signatur weicht ab - Kamera nicht gehalten (%u/%u)", s_panel->sub19_x, 0u, 0u);
        return;                                   /* falscher Stand: lieber Original als falsch */
    }
    s_x = raw + s_panel->sub19_x;
    g_re15_codepanel_anker_n = 1;
    folge_bestimmen(room_id);
}

int re15_codepanel_r37_pc(void *thread)
{
    scd_thread_t *t = (scd_thread_t *)thread;
    if (!s_x || !t || !t->pc) return 0;
    if (t->pc == s_x) {
        /* X: Cut_old / Nop / Cut_auto 1 jetzt NICHT — die Kamera bleibt auf dem Panel-Cut. Message_on 5 (X+4) laeuft
         * im selben Takt weiter (Piep +0 wie bisher). */
        s_halt = 1;
        s_auf0 = g_re15_auf_zaehler;
        s_warte = 0;
        t->pc += RE15_CP_X_UEBER;
        logf_("kamera halten: sub19 @0x%05X Cut_old/Cut_auto verschoben (Folge %u Toene, letzter +%u)",
              s_panel->sub19_x, (unsigned)s_folge_n, (unsigned)s_folge_letztes);
        return 1;
    }
    if (s_halt && t->pc == s_x + RE15_CP_X_NACH_MSG) {
        /* X+10: die Nachricht ist zu (Evt_next X+8 gab erst nach dem Schliessen frei). Kamera erst zurueck, wenn die
         * Folge der Nachricht gespielt ist (Aufschliessen = letzter Ton). Notbremse: hoechstens letztes Bild + 2
         * Wartebilder (die Folge laeuft je Spielbild weiter, aufschliessen_r36_tick).
         * PORT-WAHL (Abnahme E#0 H1): die Notbremse hat kein Original-Gegenstueck. RE2 ROOM2110 @0x01B9C setzt die
         * Kamera direkt hinter dem Aufschliess-Se_on zurueck; der Port wartet auf den Folgezaehler und haelt die Kamera
         * hoechstens bis 2 Bilder nach dem letzten Ton der Folge, damit ein stehender Zaehler die Kamera nie festhaelt.
         * In allen Messlaeufen der Abnahme griff sie nicht ("gewartet 0 Bilder"). */
        unsigned gespielt = g_re15_auf_zaehler - s_auf0;
        if ((int)gespielt < s_folge_n && s_warte < s_folge_letztes + 2) { s_warte++; return 2; }
        scd_thread_t tmp = *t;
        tmp.pc = s_x;                      op_cut_old(&tmp);    /* X   `2a`    */
        tmp.pc = s_x + 2;                  op_cut_auto(&tmp);   /* X+2 `3c 01` */
        s_halt = 0; s_warte_letzt = s_warte; s_freigaben++;
        logf_("kamera zurueck: Toene %u/%u, gewartet %u Bilder", gespielt, (unsigned)s_folge_n, (unsigned)s_warte);
        return 0;                                            /* Set(1,27,0) normal ausfuehren */
    }
    return 0;
}

int re15_codepanel_r37_lampe(unsigned bild, int cut_gezeigt, re15_cp_lampe_t *out)
{
    const re15_cp_panel_t *p = s_panel;
    int zustand = 0;                                   /* 0 aus, 1 zu (rote Lampe), 2 offen */
    if (p && p->raum == (uint16_t)g_current_room_id && cut_gezeigt == (int)p->cut)
        zustand = re15_game_flag_get(3, p->flag_bit) ? 2 : 1;
    if (zustand != s_lampe_zustand || bild < s_lampe_ein) {
        if (zustand == 2)
            logf_("lampe OFFEN: Zeile %u (0 rot, 1 blau, 2 gruen) Cut %u Flag (3,%u)", (unsigned)p->auf_zeile,
                  (unsigned)cut_gezeigt, (unsigned)p->flag_bit);
        else if (zustand == 1)
            logf_("lampe ZU: Zeile %u (0 rot, 1 blau, 2 gruen) Cut %u Flag (3,%u)", (unsigned)p->zu_zeile,
                  (unsigned)cut_gezeigt, (unsigned)p->flag_bit);
        s_lampe_zustand = zustand;
        s_lampe_ein = bild;
    }
    if (!zustand) return 0;
    unsigned takt = bild - s_lampe_ein;
    int offen = (zustand == 2);
    int mx2 = offen ? p->auf_x2 : p->zu_x2;
    int my2 = offen ? p->auf_y2 : p->zu_y2;
    re15_cp_lampe_t l;
    if (p->form == RE15_CP_FORM_RUND) {
        l.zelle = k_rund_zelle[takt & 3];
        l.s     = k_rund_s[takt & 3];
        l.kante = l.s * CP_RUND_PX_JE_TEXEL;
    } else {
        l.zelle = k_quad_zelle[takt & 1];
        l.s     = 32;
        l.kante = CP_QUADRAT_KANTE;
    }
    /* Ecke = floor(Mitte - Kante/2 + 0.5) (wie RE15_PANEL_LAMPE_X0); die Zellen sind um ihre Mitte versetzt
     * (ROOM2130 effect.esp @0x70..: Zelle 5 `a0 00 f4 f4` = -12, sonst `.. f0 f0` = -16 = S/2). */
    l.x0 = (mx2 - l.kante + 1) >> 1;
    l.y0 = (my2 - l.kante + 1) >> 1;
    l.zeile = offen ? p->auf_zeile : p->zu_zeile;
    l.offen = offen;
    if (out) *out = l;
    return 1;
}

int      re15_codepanel_r37_haelt(void)       { return s_halt; }
unsigned re15_codepanel_r37_freigaben(void)   { return s_freigaben; }
int      re15_codepanel_r37_wartebilder(void) { return s_warte_letzt; }
