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
#include "re15_engine.h"     /* g_engine.frame_count (nur Messschiene)            */

/* Der angestrebte Wert (Funktion der zehn Schalterbits) und der ANGEZEIGTE Wert, der ihm
 * mit genau EINEM Punkt je Bild folgt (RE2 sub04+0x0106 @ROOM2130.RDT 0x01216 `02`
 * evt_next innerhalb der Nachfuehrschleife). */
static int s_wert      = 0;
static int s_ziel      = 0;
static int s_roh       = 0;   /* ungeklemmte Summe — darf negativ sein (Nutzer-Vorgabe) */
static int s_aktiv     = 0;
static int s_geloest_vorframe = 0;
static int s_eingeschwungen   = 0;   /* erster Tick im Raum: ohne Nachfuehr-Animation */
/* STILLSTAND vor der Abnahme (Runde 31, Belege im Kopf von re15_panel_zeiger.h):
 * s_maske = Schaltermaske des letzten Ticks, s_ruhe = Bilder seit der letzten Zeiger-
 * bewegung bzw. Schalteraenderung, gesaettigt bei RE15_PANEL_RUHE_BILDER
 * (RE2 ROOM2130.RDT sub04 @0x0171C/@0x0171D sleep 0x1E). */
static unsigned s_maske = 0;
static int      s_ruhe  = RE15_PANEL_RUHE_BILDER;
/* DIE ZWEI GRUENEN LAMPEN (Runde 34 Nacht, Belege im Kopf von re15_panel_zeiger.h):
 * s_lampe_an[i] = Zustand im letzten Tick (0 oben, 1 unten), s_lampe_takt[i] = Bilder seit
 * dem Einschalten (0 im Einschaltbild -> RE2-Zelle 3, dann 4, 3, 4 ...). */
static int      s_lampe_an[2]   = { 0, 0 };
static int      s_lampe_takt[2] = { 0, 0 };

/* Messhaken fuer die Sonde. */
unsigned g_re15_panel_bestaet_zaehler = 0;

/* Die zehn Schalter des Raetsels sind Bank 5 / Bits 13..22 — selbst aus den Bytes gelesen,
 * ROOM11F0.RDT sub01 @Datei 0x012BE..0x012E2:
 *   21 05 0d 01 / 21 05 0e 00 / 21 05 0f 01 / 21 05 10 00 / 21 05 11 01 /
 *   21 05 12 00 / 21 05 13 01 / 21 05 14 00 / 21 05 15 01 / 21 05 16 00
 * (Ck(Bank 5, Bit 13..22) gegen 1,0,1,0,1,0,1,0,1,0). */
#define PANEL_BIT0   13
#define PANEL_BITS   10

/* Die zehn Schalterstellungen als Bitmaske: Bit 0 = Schalter 1 (Bank 5 Bit 13) .. Bit 9 =
 * Schalter 10 (Bank 5 Bit 22). Dieselbe Reihenfolge wie die Loesungskette
 * ROOM11F0.RDT @0x012BE..0x012E2 (`21 05 0d 01` .. `21 05 16 00`). */
static unsigned panel_maske(void)
{
    unsigned m = 0;
    for (int i = 0; i < PANEL_BITS; i++)
        if (re15_game_flag_get(5, (uint8_t)(PANEL_BIT0 + i))) m |= 1u << i;
    return m;
}

static int panel_ein_zaehlen(void)
{
    int n = 0;
    for (int i = 0; i < PANEL_BITS; i++)
        if (re15_game_flag_get(5, (uint8_t)(PANEL_BIT0 + i))) n++;
    return n;
}

/* ⛔ WOERTLICHE NUTZER-VORGABE vom 2026-09-26 (Wortlaut im Kopf von re15_panel_zeiger.h),
 * KEIN Original und kein RE-Beleg:
 *   Schalter :  1    2    3    4    5    6    7    8    9   10
 *   Wert     : +20  -20  -10  -30  +20  -40  +20  -50  +30  -60          */
static const int s_gewicht[RE15_PANEL_SCHALTER] = RE15_PANEL_GEWICHTE;

int re15_panel_zeiger_gewicht(int nr)
{
    if (nr < 1 || nr > RE15_PANEL_SCHALTER) return 0;
    return s_gewicht[nr - 1];
}

int re15_panel_zeiger_roh_aus_maske(unsigned maske)
{
    /* ⛔ ROHWERT — hier wird NICHT geklemmt. Nutzer: "Die Schalterwerte bleiben erhalten,
     * ein Schalter kann jederzeit wieder AUS." Ein an der Null klemmender Akkumulator
     * wuerde genau das brechen: das Ergebnis haenge dann am WEG (wie in RE2) statt am
     * ZUSTAND, und die zustandsbasierte RE1.5-Loesungspruefung @0x012BE..0x012E2 wuerde
     * dieselbe Schalterstellung mal auf 80 bringen und mal nicht. */
    int v = 0;
    for (int i = 0; i < RE15_PANEL_SCHALTER; i++)
        if (maske & (1u << i)) v += s_gewicht[i];
    return v;
}

int re15_panel_zeiger_ziel_aus_maske(unsigned maske)
{
    /* ⛔ ANZEIGE = max(0, Rohwert) — Nutzer-Vorgabe. Boden und Deckel sind deckungsgleich
     * mit RE2: sub04+0x0130 (@ROOM2130.RDT 0x01240) `23 00 05 04 00 00` -> var5 = 0 und
     * sub04+0x00B8 (@0x011C8) `23 00 05 02 64 00` -> var5 = 100. Der Deckel greift mit
     * diesen Gewichten nie (Maximum 90 = 20+20+20+30), steht aber, weil er belegt ist. */
    int v = re15_panel_zeiger_roh_aus_maske(maske);
    if (v > RE15_PANEL_MAX) v = RE15_PANEL_MAX;
    if (v < RE15_PANEL_MIN) v = RE15_PANEL_MIN;
    return v;
}

void re15_panel_zeiger_reset(void)
{
    s_wert = 0; s_ziel = 0; s_roh = 0; s_aktiv = 0;
    s_geloest_vorframe = 0; s_eingeschwungen = 0;
    s_maske = 0; s_ruhe = RE15_PANEL_RUHE_BILDER;
    s_lampe_an[0] = s_lampe_an[1] = 0;
    s_lampe_takt[0] = s_lampe_takt[1] = 0;
}

/* Die Lampenregel — NUTZER-VORGABE Runde 34 Nacht ("Das obere Licht soll angehen, wenn links
 * die 3 Schalter korrekt betaetigt sind ... Sobald eines der jeweiligen Schalter der
 * jeweiligen Seite nicht mehr korrekt ist ... wieder aus"). "Korrekt" = die RE1.5-Loesung:
 *   oben  (linke Spalte, Schalter 1..5):  Ck @0x012BE `21 05 0d 01`, @0x012C2 `21 05 0e 00`,
 *         @0x012C6 `21 05 0f 01`, @0x012CA `21 05 10 00`, @0x012CE `21 05 11 01`
 *   unten (rechte Spalte, Schalter 6..10): Ck @0x012D2 `21 05 12 00`, @0x012D6 `21 05 13 01`,
 *         @0x012DA `21 05 14 00`, @0x012DE `21 05 15 01`, @0x012E2 `21 05 16 00`
 * Beide zusammen <=> Maske 0x155 <=> Zeigerziel 80 <=> die Kette zieht. */
int re15_panel_lampe_an_aus_maske(int nr, unsigned maske)
{
    if (nr == 0) return (maske & RE15_PANEL_LAMPE_OBEN_MASKE)  == RE15_PANEL_LAMPE_OBEN_SOLL;
    if (nr == 1) return (maske & RE15_PANEL_LAMPE_UNTEN_MASKE) == RE15_PANEL_LAMPE_UNTEN_SOLL;
    return 0;
}

void re15_panel_zeiger_tick(void)
{
    if (!RE15_PANEL_IST_RAUM(g_current_room_id)) {
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
    {
        unsigned maske = panel_maske();
        int geloest_jetzt = re15_game_flag_get(4, 238);
        s_roh  = geloest_jetzt ? RE15_PANEL_ZIEL : re15_panel_zeiger_roh_aus_maske(maske);
        s_ziel = geloest_jetzt ? RE15_PANEL_ZIEL : re15_panel_zeiger_ziel_aus_maske(maske);
    }

    if (!s_eingeschwungen) {           /* Aufbau: sofort auf den Stand, keine Fahrt */
        s_eingeschwungen = 1;
        s_wert = s_ziel;
        s_geloest_vorframe = re15_game_flag_get(4, 238);
        s_maske = panel_maske();
        s_ruhe  = RE15_PANEL_RUHE_BILDER;   /* beim Raumaufbau steht der Zeiger bereits */
    } else {
        int vorher = s_wert;
        unsigned maske = panel_maske();
        if (s_wert < s_ziel)      s_wert++;   /* RE2: genau EIN Punkt je Bild (@0x01216) */
        else if (s_wert > s_ziel) s_wert--;
        /* STILLSTAND ZAEHLEN (RE2-Angleichung, Belege im Kopf von re15_panel_zeiger.h).
         * Bewegung in diesem Bild = RE2s letzter Durchlauf der Nachfuehrschleife
         * (@0x011FC var7 += 9 / @0x01216 evt_next). Eine Schalteraenderung ohne Fahrt
         * zaehlt ebenso: RE2 schlaeft hinter JEDEM Schalter 30 Bilder, auch wenn var4 = 0
         * die Schleife gar nicht betritt (@0x012A4/@0x012A5 `09`/`0a 1e 00`).
         * Bewegt im Bild k -> s_ruhe = 0; nach dem Bild k+30 steht s_ruhe = 30, und die
         * VM des Bildes k+31 laesst die Abnahme durch — genau RE2s Bild k+31
         * (Sleeping @0x80053A24 gibt auch im Null-Bild ab, `addiu v0,zero,2`). */
        if (s_wert != vorher || maske != s_maske) s_ruhe = 0;
        else if (s_ruhe < RE15_PANEL_RUHE_BILDER) s_ruhe++;
        s_maske = maske;
    }

    /* DIE ZWEI LAMPEN, je Bild aus den Schalterbits (kein Einrasten): wirksam im Bild, in dem
     * das Schalterbit wechselt (sub06..15 setzen es erst NACH der 16-Bild-Kippung, z.B.
     * @0x01340 `22 05 0d 01`), an UND aus. Takt 0 im Einschaltbild = RE2-Zelle 3 (Routine 1
     * @0x8001dc40/0x8001dc4c setzt Satz 4 im Zuendbild, Satz 4 `03 01 01 20`), dann je Bild
     * weiter (Dauer 1, FUN_8001d68c @0x8001d7b8..0x8001d880). */
    {
        unsigned maske = panel_maske();
        for (int i = 0; i < 2; i++) {
            int an = re15_panel_lampe_an_aus_maske(i, maske);
            s_lampe_takt[i] = (an && !s_lampe_an[i]) ? 0 : s_lampe_takt[i] + 1;
            if (s_lampe_takt[i] > 0x3FFF) s_lampe_takt[i] &= 1;   /* nur die Paritaet zaehlt */
            s_lampe_an[i] = an;
        }
    }

    /* Die BESTAETIGUNG. RE2 spielt sie unmittelbar hinter dem Geloest-Flag:
     *   sub04+0x064E (@ROOM2130.RDT 0x0175E)  22 04 3c 01   Raetsel geloest
     *   sub04+0x0652 (@ROOM2130.RDT 0x01762)  36 02 0c 01   se_on(Gruppe 2, 0x0C)
     * RE1.5s Gegenstueck zum Geloest-Flag ist Set(4,238,1), ROOM11F0.RDT sub01
     * @Datei 0x012EA `22 04 ee 01` (0xEE = 238), direkt hinter Evt_exec(sub18) @0x012E6.
     * RE1.5 selbst spielt dort nichts (0 Se_on im ganzen SCD) -> RE2-Ergaenzung.
     * Seit Runde 31 kommt die Flanke erst, wenn der Zeiger 80 erreicht und 30 Bilder
     * gestanden hat (re15_panel_zeiger_abnahme_haelt haelt Set(4,238,1) bis dahin zurueck) —
     * damit faellt der Ton wie in RE2 mit der Meldung "Power supply OK." zusammen
     * (RE2 @0x01758 message_on / @0x0175E set / @0x01762 se_on im selben Skriptschritt;
     * message_on @0x80054A8C kehrt mit `addiu v0,zero,1` = weiter zurueck). */
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
      if (lf) { fprintf(lf, "F%u raum=%04X cut=%d aktiv=%d maske=%03X ein=%d roh=%d ziel=%d wert=%d geloest=%d "
                            "strom=%d padsperre=%d msg=%d bestaet=%u ruhe=%d panelsperre=%d "
                            "lampe_o=%d lampe_u=%d lzelle_o=%d lzelle_u=%d\n",
                        (unsigned)g_engine.frame_count,
                        g_current_room_id, g_re15_active_cut, s_aktiv,
                        panel_maske(), panel_ein_zaehlen(), s_roh, s_ziel,
                        s_wert, geloest,
                        re15_game_flag_get(4, 243),                        /* sub18 @0x016F6 */
                        (g_re15_pauseflags & RE15_PAUSE_PAD) ? 1 : 0,
                        (int)g_scd.message_active, g_re15_panel_bestaet_zaehler,
                        s_ruhe, re15_panel_zeiger_sperrt(),
                        s_lampe_an[0], s_lampe_an[1],
                        s_lampe_an[0] ? RE15_PANEL_LAMPE_ZELLE_A + (s_lampe_takt[0] & 1) : 0,
                        s_lampe_an[1] ? RE15_PANEL_LAMPE_ZELLE_A + (s_lampe_takt[1] & 1) : 0);
                fflush(lf); } }
}

int re15_panel_zeiger_wert(void) { return s_wert; }
int re15_panel_zeiger_ziel(void) { return s_ziel; }
int re15_panel_zeiger_roh(void)  { return s_roh;  }
int re15_panel_zeiger_ruhe(void) { return s_ruhe; }

/* Der Zeiger steht auf 80 und hat RE15_PANEL_RUHE_BILDER Bilder gestanden — RE2s Weg vom
 * Schleifenende @0x01708 ueber sleep 30 @0x0171C zur Pruefung cmp(var5 == 80) @0x01752.
 * Die Maske wird LIVE gegen den letzten Tick gehalten: steht in diesem Bild schon ein
 * neues Schalterbit, das der Tick noch nicht gesehen hat, ist nichts frei. */
int re15_panel_zeiger_abnahme_frei(void)
{
    if (!s_eingeschwungen) return 0;
    if (panel_maske() != s_maske) return 0;
    return s_wert == RE15_PANEL_ZIEL && s_ziel == RE15_PANEL_ZIEL &&
           s_ruhe >= RE15_PANEL_RUHE_BILDER;
}

int re15_panel_zeiger_abnahme_haelt(const uint8_t *pc, const uint8_t *raw)
{
    if (!pc || !raw) return 0;
    if (!RE15_PANEL_IST_RAUM(g_current_room_id)) return 0;
    if ((size_t)(pc - raw) != (size_t)RE15_PANEL_ABNAHME_OFF) return 0;
    /* Anker: genau die ausgelieferten Bytes, sonst nichts halten.
     *   @0x012E6 `04 ff 18 12` Evt_exec(sub18)   @0x012EA `22 04 ee 01` Set(4,238,1) */
    if (pc[0] != 0x04 || pc[1] != 0xFF || pc[2] != 0x18 || pc[3] != 0x12) return 0;
    if (pc[4] != 0x22 || pc[5] != 0x04 || pc[6] != 0xEE || pc[7] != 0x01) return 0;
    return re15_panel_zeiger_abnahme_frei() ? 0 : 1;
}

int re15_panel_zeiger_sperrt(void)
{
    /* NUR DIE ENDSPERRE (Runde 34 Nacht, NUTZER-VORGABE "man soll sich frei bewegen koennen
     * Ausser ganz am Ende", Belege im Kopf von re15_panel_zeiger.h). Die Runde-31-Zweige
     * "Maske geaendert / Zeiger faehrt / Ruhe < 30" sperrten nach JEDEM Schalter — das nimmt
     * der Nutzer zurueck, und RE1.5 hat es nie getan (kein `22 02 07 01` in sub01..sub17). */
    if (!RE15_PANEL_IST_RAUM(g_current_room_id) || !s_eingeschwungen) return 0;
    if (!re15_game_flag_get(5, 0)) return 0;        /* Raetsel nicht aktiv (sub16 @0x015C2) */
    if (re15_game_flag_get(4, 238)) return 0;       /* abgenommen: sub18 sperrt selbst @0x01736 */
    /* LIVE (nicht s_maske des letzten Ticks): das Schalterbit, das die VM in DIESEM Bild
     * gesetzt hat (sub06..15, z.B. @0x01340 `22 05 0d 01`), sperrt schon die Pad-Woerter, die
     * in diesem Bild fuer die naechste VM entstehen — sonst rutschte ein gehaltener Knopf fuer
     * ein Bild durch. Die Maske 0x155 ist die EINZIGE, die auf 80 zielt (r27-Riegel: 1 von
     * 1024), und genau die RE1.5-Loesung @0x012BE..0x012E2. Damit umfasst diese eine Bedingung
     * die ganze RE2-Endstrecke: Fahrt auf 80 (@0x011E0..0x01218), Stillstand 30 (@0x0171C),
     * Pruefung (@0x01752) — RE2 haelt Bank 2 Bit 7 bis @0x01818. Die Sperre endet von selbst:
     * die Abnahme setzt 4:238 (@0x012EA, Zeile oben) und sub18 legt im selben Bild seine
     * eigene Sperre (@0x01736 `22 02 07 01`); aendert eine vorher begonnene Kippung die
     * Maske, faellt sie im Bild des Bits. */
    return panel_maske() == RE15_PANEL_LOESUNGSMASKE;
}

int re15_panel_lampen(void)
{
    return (s_lampe_an[0] ? 1 : 0) | (s_lampe_an[1] ? 2 : 0);
}

int re15_panel_lampe_takt(int nr)
{
    return (nr == 0 || nr == 1) ? s_lampe_takt[nr] : 0;
}

int re15_panel_lampe_sicht(int nr, int *x0, int *y0, int *kante, int *zelle)
{
    /* Sichtbar wie der Zeiger: Raum 11F0/11F1 und die Raetselbuehne Cut 10 (sub16 @0x015C0
     * `29 0a`). Die Lampen sind im Hintergrund dieses Cuts gemalt (Rahmen x 213..231 /
     * y 67..85 und y 126..143, C_generator.md §3.6); Cut 12 zeigt sie zwar schraeg, wird aber
     * nie angefahren (kein `29 0c`, keine RVD-Zone). */
    if (nr != 0 && nr != 1) return 0;
    if (!RE15_PANEL_IST_RAUM(g_current_room_id)) return 0;
    if (g_re15_active_cut != RE15_PANEL_CUT) return 0;
    if (!s_lampe_an[nr]) return 0;
    if (x0)    *x0    = RE15_PANEL_LAMPE_X0;
    if (y0)    *y0    = nr ? RE15_PANEL_LAMPE_Y0_UNTEN : RE15_PANEL_LAMPE_Y0_OBEN;
    if (kante) *kante = RE15_PANEL_LAMPE_KANTE;
    if (zelle) *zelle = RE15_PANEL_LAMPE_ZELLE_A + (s_lampe_takt[nr] & 1);
    return 1;
}

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
    if (!RE15_PANEL_IST_RAUM(g_current_room_id)) return 0;
    if (g_re15_active_cut != RE15_PANEL_CUT) return 0;
    if (sx) *sx = RE15_PANEL_SPITZE_X;
    /* y = 177 - wert*1.23, ganzzahlig und kaufmaennisch gerundet (Eichung s. Header (a)). */
    if (sy) *sy = RE15_PANEL_ANKER_Y
                - (s_wert * RE15_PANEL_SPANNE_Y + 50) / 100;
    return 1;
}
