/*
 * panel_zeiger_common.c — der rote Leistungs-Zeiger des Boiler-Room-Bedienfelds.
 *
 * ALLE Belege (RE2-Datei-Offsets, die selbst vermessene RE1.5-Skalen-Eichung und die
 * ausdruecklich als NUTZER-ENTSCHEIDUNG gekennzeichnete Schaltergewichtung) stehen im
 * Kopf von include/re15_panel_zeiger.h. Hier steht nur die Mechanik.
 */
#include <stdio.h>
#include <stdlib.h>
#include "re15_panel_zeiger.h"
#include "re15_scd.h"        /* re15_game_flag_get / g_scd                       */
#include "re15_room.h"       /* g_current_room_id                                */
#include "re15_light.h"      /* g_re15_active_cut                                */
#include "re15_audio.h"      /* re15_audio_re2_panel_se + RE15_PANEL_SE_BESTAET  */

/* Der angestrebte Wert (Funktion der zehn Schalterbits) und der ANGEZEIGTE Wert, der ihm
 * mit genau EINEM Punkt je Bild folgt (RE2 sub04+0x0106 @ROOM2130.RDT 0x01216 `02`
 * evt_next innerhalb der Nachfuehrschleife). */
static int s_wert      = 0;
static int s_ziel      = 0;
static int s_aktiv     = 0;
static int s_geloest_vorframe = 0;
static int s_eingeschwungen   = 0;   /* erster Tick im Raum: ohne Nachfuehr-Animation */

/* Messhaken fuer die Sonde. */
unsigned g_re15_panel_bestaet_zaehler = 0;

/* Die zehn Schalter des Raetsels sind Bank 5 / Bits 13..22 — selbst aus den Bytes gelesen,
 * ROOM11F0.RDT sub01 @Datei 0x012BE..0x012E2:
 *   21 05 0d 01 / 21 05 0e 00 / 21 05 0f 01 / 21 05 10 00 / 21 05 11 01 /
 *   21 05 12 00 / 21 05 13 01 / 21 05 14 00 / 21 05 15 01 / 21 05 16 00
 * (Ck(Bank 5, Bit 13..22) gegen 1,0,1,0,1,0,1,0,1,0). */
#define PANEL_BIT0   13
#define PANEL_BITS   10

static int panel_ein_zaehlen(void)
{
    int n = 0;
    for (int i = 0; i < PANEL_BITS; i++)
        if (re15_game_flag_get(5, (uint8_t)(PANEL_BIT0 + i))) n++;
    return n;
}

int re15_panel_zeiger_ziel_aus_bits(int ein_schalter)
{
    /* ⛔ NUTZER-ENTSCHEIDUNG: RE15_PANEL_GEWICHT (16). Deckel/Boden dagegen sind RE2:
     * sub04+0x00B8 (@ROOM2130.RDT 0x011C8) `23 00 05 02 64 00` -> var5 = 100, und
     * sub04+0x0130 (@0x01240) `23 00 05 04 00 00` -> var5 = 0. */
    int v = ein_schalter * RE15_PANEL_GEWICHT;
    if (v > RE15_PANEL_MAX) v = RE15_PANEL_MAX;
    if (v < RE15_PANEL_MIN) v = RE15_PANEL_MIN;
    return v;
}

void re15_panel_zeiger_reset(void)
{
    s_wert = 0; s_ziel = 0; s_aktiv = 0;
    s_geloest_vorframe = 0; s_eingeschwungen = 0;
}

void re15_panel_zeiger_tick(void)
{
    if (g_current_room_id != RE15_PANEL_RAUM) {
        if (s_aktiv || s_eingeschwungen) re15_panel_zeiger_reset();
        return;
    }
    /* Das Raetsel laeuft, solange Bank 5 Bit 0 steht: sub16 setzt es beim Einstieg
     * (ROOM11F0.RDT @Datei 0x015C2 ff., 13x `22 05 xx 01`), sub17 loescht es beim
     * Ausstieg (@Datei 0x0168C ff., 23x `22 05 xx 00`). */
    s_aktiv = re15_game_flag_get(5, 0);

    /* GEMESSEN (RE15_PANEL_LOG, ROOM11F0 Cut 10, Bits 5:0 + 5:13/15/17/19/21 ab Bild 240):
     * in dem Bild, in dem die Loesungskette @Datei 0x012BE..0x012E2 zieht, laeuft sub18 und
     * RAEUMT Bank 5 wieder ab (@Datei 0x0168C ff. bzw. sub18 @0x16F6) — `aktiv` faellt schon
     * im naechsten Bild auf 0 zurueck. Wuerde der Zeiger AUS den Bits weiterrechnen, fiele er
     * im Augenblick des Loesens auf 0 statt auf 80 zu stehen.
     * Darum: ab dem Geloest-Flag steht das Ziel FEST auf 80. Das ist genau RE2s Verhalten —
     * dort wird var5 nach dem "Power supply OK."-Zweig (sub04+0x0642 @ROOM2130.RDT 0x01752)
     * NICHT zurueckgesetzt, der Zeiger bleibt auf 80 stehen. */
    s_ziel = re15_game_flag_get(4, 238)
           ? RE15_PANEL_ZIEL
           : re15_panel_zeiger_ziel_aus_bits(panel_ein_zaehlen());

    if (!s_eingeschwungen) {           /* Aufbau: sofort auf den Stand, keine Fahrt */
        s_eingeschwungen = 1;
        s_wert = s_ziel;
        s_geloest_vorframe = re15_game_flag_get(4, 238);
    } else if (s_wert < s_ziel) {
        s_wert++;                      /* RE2: genau EIN Punkt je Bild (@0x01216) */
    } else if (s_wert > s_ziel) {
        s_wert--;
    }

    /* Die BESTAETIGUNG. RE2 spielt sie unmittelbar hinter dem Geloest-Flag:
     *   sub04+0x064E (@ROOM2130.RDT 0x0175E)  22 04 3c 01   Raetsel geloest
     *   sub04+0x0652 (@ROOM2130.RDT 0x01762)  36 02 0c 01   se_on(Gruppe 2, 0x0C)
     * RE1.5s Gegenstueck zum Geloest-Flag ist Set(4,238,1), ROOM11F0.RDT sub01
     * @Datei 0x012EA `22 04 ee 01` (0xEE = 238), direkt hinter Evt_exec(sub18) @0x012E6.
     * RE1.5 selbst spielt dort nichts (0 Se_on im ganzen SCD) -> RE2-Ergaenzung. */
    int geloest = re15_game_flag_get(4, 238);
    if (geloest && !s_geloest_vorframe) {
        g_re15_panel_bestaet_zaehler++;
        re15_audio_re2_panel_se(RE15_PANEL_SE_BESTAET);   /* RE2 Gruppe 2 / 0x0C */
    }
    s_geloest_vorframe = geloest;

    /* MESSCHIENE (env-gegatet, im Normalpfad stumm). Die SDL-GUI-exe hat kein brauchbares
     * stderr, also in eine DATEI: RE15_PANEL_LOG=<pfad>. Eine Zeile je Bild. */
    { static FILE *lf = (FILE *)0; static int init = 0;
      if (!init) { init = 1; const char *e = getenv("RE15_PANEL_LOG");
                   if (e && *e) lf = fopen(e, "w"); }
      if (lf) { fprintf(lf, "raum=%04X cut=%d aktiv=%d ein=%d ziel=%d wert=%d geloest=%d\n",
                        g_current_room_id, g_re15_active_cut, s_aktiv,
                        panel_ein_zaehlen(), s_ziel, s_wert, geloest);
                fflush(lf); } }
}

int re15_panel_zeiger_wert(void) { return s_wert; }
int re15_panel_zeiger_ziel(void) { return s_ziel; }

int re15_panel_zeiger_sicht(int *sx, int *sy)
{
    /* Sichtbar, solange die RAETSELBUEHNE im Bild ist — nicht, solange Bank 5 Bit 0 steht.
     * Grund, gemessen: sub18 raeumt Bank 5 im selben Bild ab, in dem das Raetsel faellt
     * (s. re15_panel_zeiger_tick). Ein Bit-Gatter wuerde den Zeiger genau dann wegnehmen,
     * wenn er auf 80 stehen SOLL. RE2 macht es genauso: der Zeiger ist Objektmodell 2 des
     * Raums und lebt, solange der Panel-Cut laeuft (sub04 @0x000E `29 06` Cut_chg 6 ...
     * @0x061C `29 04` Cut_chg 4 zurueck). RE1.5s Panel-Cut ist die 0x0A aus sub16
     * @Datei 0x015C0 (`29 0a`); sub18 schaltet danach auf 8/0x0D/0x0E weiter, damit
     * verschwindet der Zeiger von selbst. */
    if (g_current_room_id != RE15_PANEL_RAUM) return 0;
    if (g_re15_active_cut != RE15_PANEL_CUT) return 0;
    if (sx) *sx = RE15_PANEL_SPITZE_X;
    /* y = 177 - wert*1.23, ganzzahlig und kaufmaennisch gerundet (Eichung s. Header (a)). */
    if (sy) *sy = RE15_PANEL_ANKER_Y
                - (s_wert * RE15_PANEL_SPANNE_Y + 50) / 100;
    return 1;
}
