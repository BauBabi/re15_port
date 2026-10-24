/*
 * Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (Abnahme 1 Maengel 1-3, master-AUFTRAG E-N1/E-N2). Module
 * engine/src/msgtakt_r41.c, kamerahalt_r41.c; Dossier E_kleinkram_b.md "Nachbesserung 2".
 *
 *   msgtakt     Restore beim Schliessen IM Nachrichten-Takt wirkt erst hinter dem Spiel-Takt (Hauptschleife
 *               @0x80020de4 Spiel-Task < @0x80020f3c/@0x80010044 Nachrichten-Takt); ein Oeffnen im selben Bild holt
 *               ihn vorher nach (Schnappschuss @0x80027ec8 bleibt richtig); ohne takt_beginn alles wie vorher;
 *               Auswahl-Message_on laeuft durch nur mit Skript-Bit 0x02000000 (@0x8003f040-@0x8003f04c).
 *   kamerahalt  Apply-Bild = pending ODER work_vars[0x0A] != cam_id (Present @0x800214f4-@0x80021538 ->
 *               @0x80021558/@0x80021560); erstes Bild eines Raums und ROOM1240-Montage nicht.
 *   frage       ROOM2020 ohne Fenster, Leon GEHT in die Kurbel-Zone (Slot 3 @0x009AE), Aktion, beide Fragen per
 *               Quadrat: mit der Original-Reihenfolge kein Luecken-Bild zwischen Message_on 0 @0x0A5C und 1 @0x0A62
 *               (Geh-Bild bleibt, Original n9..n94 Clip 5 Bild 9) und Pos_set @0x0A8C im ERSTEN Bild nach dem
 *               Schliessen der Frage (Original n94 -> n95); Gegenprobe alte Reihenfolge: Luecke + ein Bild spaeter.
 * Nachbesserung 3 (Abnahme 2 Maengel 1/2, Orchestrator-Entscheid E-E2), Modul engine/src/rennstopp_r41.c:
 *   rennstopp   Renn-Stopp 0x80032604 @0x8003272c / 0x80032778 @0x800328f8-@0x8003295c ohne gerechnetes Renn-Tempo
 *               (Tabelle @0x80073f24 = 200 -> Schritte 160,120,80,40,0,-40), Clip 3 + Mischung 7, Abbruch
 *               OBEN/UNTEN/LINKS/RECHTS, kein Einstieg bei OBEN+RENNEN, Einstieg bei Zielen mit Waffe, Skript-Zweig.
 *   renntempo   Renn-Tempo = SquareRoot0(Fussweg)/4 + 150 (FUN_800369f8 Modus 1, @0x800328cc-@0x800328f4) gegen die
 *               GDB-Werte des Originals (Dossier "Nachbesserung 3"): o_renn2 198 210 171 182 204 211 214, o_rennstopp ab
 *               Bild 2 210 170 182 205 210 214 210 188 159 176 197; ueber den Spieler-Pfad (lauftempo + Skript-Stopp) die
 *               Kurbel-Lagen n94..n99 z -3099 -3233 -3327 -3381 -3395 -3369; Gegenprobe ohne Bank (Tempo 200): -3285;
 *               kein Tween-Bild (0x8000) im Renn-Clip 0 einer Waffen-Bank PL00Wxx/PL04Wxx.
 * Nachbesserung 3, Fortsetzung (DECIDE 0x80032604 vollstaendig), ueber den ECHTEN Spieler-Takt (re15_game_step, ROOM2020):
 *   rennpfad    Unterzustand 2 dreht mit 72 (@0x80073f25, @0x80032620-@0x800326a0) auch ohne OBEN und mit R1; OBEN im
 *               Stopp -> ein Bild GEHEN auch mit RENNEN (0x0101 @0x80032738, Rennen erst aus dem Gehen-DECIDE
 *               @0x80032444); Stopp-Bilder ohne Leerlauf-FSM, im Bild nach dem Stopp-Ende Stehen-Fall 0 (Mischung 7,
 *               @0x80032074-@0x8003209c); Gegenprobe ohne Modul (Stand vor Nachbesserung 3).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_gameflow.h"
#include "re15_menu.h"
#include "re15_map_hint.h"     /* re15_host_clock_set_us */
#include "re15_cut10f0.h"      /* re15_cine_active */
#include "re15_montage_fx.h"
#include "re15_msgtakt_r41.h"
#include "re15_kamerahalt_r41.h"
#include "re15_rennstopp_r41.h"
#include "re15_emd.h"
#include "re15_md1.h"          /* re15_pld_part (PLW-Verzeichnis) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
void re15_actor_step_all_walkers(void);

static int g_fail = 0, g_n = 0;
#define PRUEF(bed, ...) do { g_n++; int _ok = (bed) ? 1 : 0; if (!_ok) g_fail++; \
    printf("  %s: ", _ok ? "PASS" : "FAIL"); printf(__VA_ARGS__); printf("\n"); } while (0)

/* ============================================================================================ msgtakt */
static void schliessen(void)   /* wie msg_common.c fsm 6 */
{
    if (!re15_msgtakt_r41_schliessen_aufschieben()) re15_pauseflags_close();
}
static void oeffnen(uint32_t maske)   /* wie re15_dialog_open_mask */
{
    re15_msgtakt_r41_vor_oeffnen();
    re15_pauseflags_open(maske);
}

static void teil_msgtakt(void)
{
    printf("== msgtakt ==\n");
    re15_msgtakt_r41_zuruecksetzen();
    re15_pauseflags_clear(); g_re15_pauseflags = 0x7;
    oeffnen(0xffff0000u);
    PRUEF(g_re15_pauseflags == 0xffff0007u, "Oeffnen: g_pauseflags |= Maske (@0x80027ed0): %08x", (unsigned)g_re15_pauseflags);
    schliessen();
    PRUEF(g_re15_pauseflags == 0x7u && !re15_msgtakt_r41_aktiv(), "ohne takt_beginn (PSX, Sonden): Restore sofort: %08x", (unsigned)g_re15_pauseflags);

    re15_msgtakt_r41_takt_beginn();
    oeffnen(0xffff0000u);
    schliessen();                                   /* im Nachrichten-Takt */
    re15_msgtakt_r41_takt_ende();
    PRUEF(g_re15_pauseflags == 0xffff0007u && re15_msgtakt_r41_restore_offen(),
          "Schliessen im Nachrichten-Takt: Spiel-Takt dieses Bilds sieht die Pause noch (%08x)", (unsigned)g_re15_pauseflags);
    re15_msgtakt_r41_nach_spieltakt();
    PRUEF(g_re15_pauseflags == 0x7u && !re15_msgtakt_r41_restore_offen(), "hinter dem Spiel-Takt: Restore (%08x)", (unsigned)g_re15_pauseflags);

    re15_msgtakt_r41_takt_beginn();
    oeffnen(0xffff0000u);
    schliessen();
    re15_msgtakt_r41_takt_ende();
    oeffnen(0x00800000u);                           /* neue Nachricht im selben Bild (Spiel-Takt) */
    PRUEF(g_re15_pauseflags == 0x00800007u, "Oeffnen bei offenem Restore: erst Restore, dann neue Maske (%08x)", (unsigned)g_re15_pauseflags);
    re15_msgtakt_r41_nach_spieltakt();
    schliessen();                                   /* ausserhalb des Takts: sofort */
    PRUEF(g_re15_pauseflags == 0x7u, "deren Schliessen stellt den Zustand VOR der ersten wieder her (%08x)", (unsigned)g_re15_pauseflags);

    re15_msgtakt_r41_zuruecksetzen();
    PRUEF(!re15_msgtakt_r41_frage_durchlaufen(0xffff0000u), "inaktiv: Auswahl parkt wie vorher");
    re15_msgtakt_r41_takt_beginn(); re15_msgtakt_r41_takt_ende();
    PRUEF(!re15_msgtakt_r41_frage_durchlaufen(0x00800000u), "Maske ohne Skript-Bit 0x02000000: parkt");
    PRUEF(re15_msgtakt_r41_frage_durchlaufen(0xff800000u), "Maske 0xff800000 (33 der 102 Auswahl-Message_on): laeuft durch");
    g_scd.message_query = 1; g_scd.message_active = 1;
    re15_msgtakt_r41_nach_spieltakt();
    PRUEF(g_scd.message_query == 1, "Frage noch offen: message_query bleibt (msg_block)");
    g_scd.message_active = 0;
    re15_msgtakt_r41_nach_spieltakt();
    PRUEF(g_scd.message_query == 0, "Frage zu: message_query faellt mit dem Restore");
    re15_msgtakt_r41_zuruecksetzen(); re15_pauseflags_clear();
}

/* ============================================================================================ kamerahalt */
static void teil_kamerahalt(void)
{
    printf("== kamerahalt ==\n");
    re15_montage_fx_set_active(0);
    g_scd.cam_change_pending = 0; g_scd.work_vars[0x0A] = 0; g_scd.cam_id = 0;
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 0, "erstes Bild des Raums: zeigen");
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 0, "angezeigt == angefordert, nicht pending: zeigen");
    g_scd.cam_id = 2;
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 1, "angefordert 2 != angezeigt 0 (@0x80021508/@0x80021514): halten");
    g_scd.cam_id = 0; g_scd.cam_change_pending = 1;
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 1, "Cut_chg auf denselben Cut (@0x800402f4 ohne Vergleich): halten");
    PRUEF(re15_kamerahalt_r41_haelt(0x2021) == 0, "Raumwechsel: zeigen");
    re15_montage_fx_set_active(1);
    PRUEF(re15_kamerahalt_r41_haelt(0x2021) == 0, "ROOM1240-Montage aktiv: zeigen");
    re15_montage_fx_set_active(0);
    g_scd.cam_change_pending = 0;
}

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}

/* ============================================================================================ rennstopp */
static void teil_rennstopp(void)
{
    printf("== rennstopp ==\n");
    re15_actor_t a; memset(&a, 0, sizeof a);
    const int RUN = 100, IDLE = 200;
    re15_rennstopp_r41_zuruecksetzen();
    a.state = 1; a.motion = RUN; a.x = 0; a.z = 0; a.rot_y = 0;
    PRUEF(re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS, RUN, IDLE, 200, 0) == 0 && a.x == 0,
          "OBEN+RENNEN gehalten (& 0x201 == 0x201 @0x800326fc): kein Stopp");
    static const int soll[6] = { 160, 280, 360, 400, 400, 360 };
    int ok = 1, r = 0;
    for (int i = 0; i < 6; i++) {
        r = re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
        if (r != 1 || a.x != soll[i]) ok = 0;
        if (i == 0 && (a.motion != IDLE || a.anim_frac != 7 || a.anim_frame != 0)) ok = 0;
    }
    PRUEF(ok, "Loslassen ohne gerechnetes Renn-Tempo (Tabelle 200): Clip 3 + Mischung 7 (@0x80032904/@0x80032910), "
              "x 160,280,360,400,400,360 (Tempo -40 @0x8003293c, letzter Schritt -40 @0x8003295c): x=%d", a.x);
    r = re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    PRUEF(r == 0 && !re15_rennstopp_r41_aktiv() && a.x == 360, "Tempo < 0 -> Stehen (@0x80032958), danach kein Schritt");
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0;
    re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_UP, RUN, IDLE, 200, 0);          /* RENNEN losgelassen, OBEN gehalten */
    PRUEF(re15_rennstopp_r41_aktiv() && a.x == 160, "nur RENNEN losgelassen: Stopp-Bild (+6 = 2 im Rennen-DECIDE)");
    r = re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_UP, RUN, IDLE, 200, 0);
    PRUEF(r == 2 && !re15_rennstopp_r41_aktiv() && a.x == 160, "OBEN im Stopp -> Gehen (0x0101 @0x80032738, Rueckgabe 2), kein Stopp-Schritt");
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0;
    re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    r = re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS, RUN, IDLE, 200, 0);
    PRUEF(r == 2 && !re15_rennstopp_r41_aktiv() && a.x == 160,
          "OBEN+RENNEN im Stopp -> auch Gehen (OBEN @0x80032738 vor jeder RENNEN-Pruefung), Rueckgabe 2");
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0; a.state = 1;
    PRUEF(re15_rennstopp_r41_unterzustand2(&a, RUN) == 1, "Unterzustand 2 im Rennen");
    re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    PRUEF(re15_rennstopp_r41_unterzustand2(&a, RUN) == 1, "Unterzustand 2 im Stopp (+6 >= 2)");
    for (int i = 0; i < 5; i++) re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    PRUEF(!re15_rennstopp_r41_aktiv() && re15_rennstopp_r41_unterzustand2(&a, RUN) == 0, "nach dem Stopp-Ende: Stehen (Unterzustand 0)");
    a.anim_frame = 3; a.anim_frac = 1;
    r = re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    PRUEF(r == 0 && a.anim_frame == 0 && a.anim_frac == 7, "Folgebild: Stehen-Fall 0 Bild 0 / Mischung 7 (@0x80032074-@0x8003209c)");
    a.anim_frac = 3;
    re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    PRUEF(a.anim_frac == 3, "nur EIN Folgebild");
    a.state = 4;
    PRUEF(re15_rennstopp_r41_unterzustand2(&a, RUN) == 0, "Kommandowort 4: kein Unterzustand 2");
    a.state = 1;
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0;
    re15_rennstopp_r41_takt(&a, 0, RUN, IDLE, 200, 0);
    r = re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_LEFT, RUN, IDLE, 200, 0);
    PRUEF(r == 0 && !re15_rennstopp_r41_aktiv(), "LINKS im Stopp -> Drehen (0x0301 @0x80032750)");
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0;
    r = re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS | RE15_PAD_BIT_R1, RUN, IDLE, 200, 1);
    PRUEF(r == 1 && a.x == 160, "Zielen mit Waffe aus dem Rennen (@0x80032708-@0x80032720): erst der Stopp");
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0;
    r = re15_rennstopp_r41_takt(&a, RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS | RE15_PAD_BIT_R1, RUN, IDLE, 200, 0);
    PRUEF(r == 0, "R1 ohne Waffe: weiter rennen");
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0; a.state = 1;
    int n = 0;
    while (re15_rennstopp_r41_skript(&a, RUN, IDLE, 200)) n++;
    PRUEF(n == 6 && a.x == 360 && a.motion == IDLE, "Skript-Zweig (Pad gesperrt, Set(2,7)): derselbe Stopp, %d Schritte, x=%d", n, a.x);
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = RUN; a.x = 0;
    re15_rennstopp_r41_skript(&a, RUN, IDLE, 200);
    a.state = 4;                                                               /* Plc_motion uebernimmt */
    r = re15_rennstopp_r41_skript(&a, RUN, IDLE, 200);
    PRUEF(r == 0 && !re15_rennstopp_r41_aktiv() && a.x == 160, "Kommandowort 4 (Plc_motion) beendet den Stopp");
    re15_rennstopp_r41_zuruecksetzen();
}

/* ============================================================================================ renntempo */
static re15_emd_skeleton_t  s_rt_bind, s_rt_roh, s_rt_sk;
static re15_emd_animation_t s_rt_an;
/* PL00-Bindepose + Keyframe-Vorrat der Bank = der Verbund des Ports (platform/pc/main.c, "w03-composite"). */
static int rt_laden(const char *bank)
{
    char p[600]; size_t n0 = 0, n1 = 0, n2 = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR); uint8_t *b0 = slurp(p, &n0);
    snprintf(p, sizeof p, "%s/PLD/%s.EMR", RE15_ASSET_PSX_DIR, bank); uint8_t *b1 = slurp(p, &n1);
    snprintf(p, sizeof p, "%s/PLD/%s.EDD", RE15_ASSET_PSX_DIR, bank); uint8_t *b2 = slurp(p, &n2);
    if (!b0 || !b1 || !b2) return 0;
    if (re15_emd_parse_skeleton(b0, n0, &s_rt_bind) != 0 || re15_emd_parse_skeleton(b1, n1, &s_rt_roh) != 0 ||
        re15_emd_parse_animation(b2, n2, &s_rt_an) != 0) return 0;
    s_rt_sk = s_rt_bind;
    s_rt_sk.keyframe_data       = s_rt_roh.keyframe_data;
    s_rt_sk.keyframe_data_size  = s_rt_roh.keyframe_data_size;
    s_rt_sk.keyframe_count      = s_rt_roh.keyframe_count;
    s_rt_sk.keyframe_size_bytes = s_rt_roh.keyframe_size_bytes;
    return 1;                                       /* Puffer bleiben bis Prozessende (keyframe_data aliast b1) */
}
/* ungemischte Pose eines Bilds (Stehclip vor dem Rennen; das Original steht dort mit Zaehler 0) */
static void rt_stehpose(int clip, int bild, int16_t w[][3], int32_t wurzel[3])
{
    const int kf = (int)(re15_emd_get_frame_entry(&s_rt_an, clip, bild) & 0xfffu);
    for (int b = 0; b < s_rt_sk.bone_count; b++) re15_emd_get_keyframe_angles(&s_rt_sk, kf, b, &w[b][0], &w[b][1], &w[b][2]);
    int16_t px = 0, py = 0, pz = 0; re15_emd_get_keyframe_position(&s_rt_sk, kf, &px, &py, &pz);
    wurzel[0] = px; wurzel[1] = py; wurzel[2] = pz;
}
/* Folge ab dem Rennbeginn: Bild k mit Zaehler 7-k (Fall 0 @0x800327fc, FUN_8001f3bc zaehlt je Bild einen ab). */
static int rt_folge(int stehbild, int gier, int n, int32_t *aus)
{
    int16_t w[RE15_EMD_MAX_BONES][3]; int32_t wu[3];
    rt_stehpose(2, stehbild, w, wu);
    for (int k = 0; k < n; k++) {
        int32_t t = 0;
        if (!re15_rennstopp_r41_fussrechnung(&s_rt_sk, &s_rt_an, 0, k, k < 7 ? 7 - k : 0, gier, gier, k == 0,
                                             (const int16_t (*)[3])w, wu, &t, NULL)) return 0;
        aus[k] = t;
    }
    return 1;
}
static int rt_gleich(const int32_t *ist, const int *soll, int von, int bis, char *txt, size_t m)
{
    int ok = 1; size_t o = 0;
    for (int k = von; k < bis; k++) {
        if (ist[k] != soll[k]) ok = 0;
        o += (size_t)snprintf(txt + o, o < m ? m - o : 0, "%s%d", k == von ? "" : " ", (int)ist[k]);
        if (o >= m) o = m - 1;
    }
    return ok;
}
static void teil_renntempo(void)
{
    printf("== renntempo ==\n");
    if (!rt_laden("PL00W03")) { PRUEF(0, "PL00.EMR / PL00W03.EDD/.EMR nicht lesbar"); return; }
    PRUEF(s_rt_sk.bone_count >= 8 && s_rt_sk.bone_parent[2] == 1 && s_rt_sk.bone_parent[5] == 1 && s_rt_sk.bone_parent[4] == 3 &&
          s_rt_sk.bone_parent[7] == 6, "Fusskette 0-1-2-3-4 / 0-1-5-6-7 = Knochenbaum des Spielers (Eltern 2<-1, 5<-1, 4<-3, 7<-6)");
    char txt[400];
    int32_t ist[16];
    /* o_renn2: Kurbel ROOM2020, Blick 1024, Stehclip 2 `mo=2/44` (zuletzt Bild 43). */
    static const int o_renn2[7] = { 198, 210, 171, 182, 204, 211, 214 };
    re15_rennstopp_r41_zuruecksetzen();
    int ok = rt_folge(43, 1024, 7, ist) && rt_gleich(ist, o_renn2, 0, 7, txt, sizeof txt);
    PRUEF(ok, "o_renn2 (Original-GDB 198 210 171 182 204 211 214): %s", txt);
    /* o_rennstopp: freie Strecke, Blick 0, `mo=2/40` (zuletzt Bild 39); Bild 1 (269) = GDB-Lageversatz, s. Header. */
    static const int o_rennstopp[12] = { 269, 210, 170, 182, 205, 210, 214, 210, 188, 159, 176, 197 };
    re15_rennstopp_r41_zuruecksetzen();
    ok = rt_folge(39, 0, 12, ist) && rt_gleich(ist, o_rennstopp, 1, 12, txt, sizeof txt);
    PRUEF(ok, "o_rennstopp ab Bild 2 (Original-GDB 210 170 182 205 210 214 210 188 159 176 197): %s (Bild 1 %d, Original 269 nach "
              "GDB-Lageversatz)", txt, (int)ist[0]);

    /* Spieler-Pfad: lauftempo wie player_common.c (Bild 1: erstes_bild, danach anim_frame++ / anim_frac-- am Takt-Anfang),
     * dann Pos_set @0x0A8C (3650,-2925) und der Stopp im Skript-Zweig (Pad gesperrt) = Original n93..n99. */
    re15_player_set_w_banks(&s_rt_sk, &s_rt_an);
    re15_rennstopp_r41_zuruecksetzen();
    re15_actor_t a; memset(&a, 0, sizeof a);
    a.state = 1; a.motion = 211; a.rot_y = 1024; a.x = 2600; a.z = -3018; a.anim_prev_valid = 1;
    { int16_t w[RE15_EMD_MAX_BONES][3]; int32_t wu[3]; rt_stehpose(2, 43, w, wu);
      for (int b = 0; b < s_rt_sk.bone_count; b++) { a.prev_angles[b][0] = w[b][0]; a.prev_angles[b][1] = w[b][1]; a.prev_angles[b][2] = w[b][2]; }
      a.prev_root[0] = wu[0]; a.prev_root[1] = wu[1]; a.prev_root[2] = wu[2]; }
    for (int k = 0; k < 7; k++) {
        if (k > 0) { a.anim_frame++; if (a.anim_frac > 0) a.anim_frac--; }
        ist[k] = re15_rennstopp_r41_lauftempo(&a, a.motion != 100, a.rot_y, 200);
        if (k == 0) { a.motion = 100; a.anim_frame = 0; a.anim_frac = 7; a.motion_init_delay = 0; }   /* player_common.c Clipwechsel */
    }
    ok = rt_gleich(ist, o_renn2, 0, 7, txt, sizeof txt);
    PRUEF(ok, "Spieler-Pfad re15_rennstopp_r41_lauftempo (Bank gespiegelt, prev_angles = Stehbild 43): %s", txt);
    a.x = 3650; a.z = -2925;                                                   /* Pos_set @0x0A8C */
    static const int o_z[6] = { -3099, -3233, -3327, -3381, -3395, -3369 };    /* o_renn2 n94..n99 */
    int32_t zz[8]; int n = 0;
    while (n < 8 && re15_rennstopp_r41_skript(&a, 100, 200, 200)) zz[n++] = a.z;
    ok = (n == 6);
    for (int k = 0; k < 6 && ok; k++) if (zz[k] != o_z[k]) ok = 0;
    PRUEF(ok && a.x == 3650 && a.motion == 200,
          "Kurbel nach dem Rennen: Stopp ab Tempo 214, z %d %d %d %d %d %d (Original n94..n99 -3099 -3233 -3327 -3381 -3395 -3369), %d Schritte",
          n > 0 ? (int)zz[0] : 0, n > 1 ? (int)zz[1] : 0, n > 2 ? (int)zz[2] : 0, n > 3 ? (int)zz[3] : 0, n > 4 ? (int)zz[4] : 0,
          n > 5 ? (int)zz[5] : 0, n);
    /* Gegenprobe: ohne Bank bleibt das Tabellen-Tempo 200 (wie vor Nachbesserung 3) -> der Riegel unterscheidet. */
    re15_player_set_w_banks(NULL, NULL);
    re15_rennstopp_r41_zuruecksetzen();
    a.motion = 211; a.anim_frame = 0; a.anim_frac = 0; a.x = 2600; a.z = -3018;
    for (int k = 0; k < 7; k++) {
        if (k > 0) { a.anim_frame++; if (a.anim_frac > 0) a.anim_frac--; }
        ist[k] = re15_rennstopp_r41_lauftempo(&a, a.motion != 100, a.rot_y, 200);
        if (k == 0) { a.motion = 100; a.anim_frame = 0; a.anim_frac = 7; }
    }
    a.x = 3650; a.z = -2925;
    while (re15_rennstopp_r41_skript(&a, 100, 200, 200)) { }
    PRUEF(ist[6] == 200 && a.z == -3285, "Gegenprobe ohne Waffen-Bank: Tempo 200, Kurbel bei z %d (Stand 635be823: -3285)", (int)a.z);
    re15_rennstopp_r41_zuruecksetzen();

    /* Tween-Bilder (0x8000, FUN_8001f8b4) im Renn-Clip 0 der Waffen-Baenke (EDD = PLW-Verzeichnis 0): keine. */
    int marken = 0, geprueft = 0;
    static const char *const fam[2] = { "PL00", "PL04" };
    for (int f = 0; f < 2; f++)
        for (int wi = 0; wi <= 0x14; wi++) {
            char p[600]; size_t nb = 0;
            snprintf(p, sizeof p, "%s/PLD/%sW%02X.PLW", RE15_ASSET_PSX_DIR, fam[f], wi);
            uint8_t *bb = slurp(p, &nb);
            unsigned long off = 0, len = 0;
            re15_emd_animation_t an;
            if (bb && re15_pld_part(bb, (long)nb, RE15_PLD_EDD, &off, &len) && len > 0 &&
                re15_emd_parse_animation(bb + off, len, &an) == 0 && an.clip_count > 0) {
                geprueft++;
                for (int k = 0; k < an.clips[0].frame_count; k++)
                    if (re15_emd_get_frame_entry(&an, 0, k) & 0x8000u) marken++;
            }
            free(bb);
        }
    PRUEF(geprueft >= 2 && marken == 0, "Renn-Clip 0 ohne Tween-Bild in %d Waffen-Baenken PL00Wxx/PL04Wxx (%d Marken)", geprueft, marken);
}

/* ============================================================================================ frage */

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0, s_r41 = 1;
static uint64_t           s_uhr = 1000000;

static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held; s_ctx.pad_pressed = pressed;
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    if (re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7)) g_scd.player_mode = 2;   /* main.c Skript-Modus */
    if (s_r41) re15_msgtakt_r41_takt_beginn();
    re15_msg_tick(&raw, &len, &id);
    if (s_r41) re15_msgtakt_r41_takt_ende();
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    re15_game_step(&s_ctx);
    if (s_r41) re15_msgtakt_r41_nach_spieltakt();
    re15_fade_tick();
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, int band)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100 || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("  FEHLER: ROOM%04X\n", room); g_fail++; return -1; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = -band * 0x708;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(band);
    s_shown = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

typedef struct { int ok, f_msg0, f_msg1, f_zu, f_pos, bild_msg0, luecke, pos_x, pos_z, motion_pos; } frage_t;

static frage_t frage_lauf(int r41)
{
    frage_t r; memset(&r, 0, sizeof r); r.f_msg0 = r.f_msg1 = r.f_zu = r.f_pos = -1;
    s_r41 = r41;
    {   static int s_gemerkt = 0; static re15_game_state_t s_ausgang;   /* jeder Lauf vom selben Spielstand */
        if (!s_gemerkt) { s_ausgang = g_game; s_gemerkt = 1; } else g_game = s_ausgang; }
    re15_msgtakt_r41_zuruecksetzen();
    re15_game_flag_set(3, 0x72, 1);                     /* Kurbel aufgenommen (ROOM2010 sub02 @0x008CA) */
    if (room_boot(0x2020, 2600, -1500, 1024, 1) != 0) return r;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 3; f++) bild(0, 0);
    int offen_vorher = 0, id_vorher = -1, letzte_bestaet = -100, frame_bei_msg0 = -1;
    for (int f = 0; f < 600; f++) {
        uint16_t held = 0, pr = 0;
        if (r.f_msg0 < 0) {                              /* gehen, ab der Kante Aktion jedes zweite Bild */
            held = RE15_PAD_BIT_UP;
            if (pl->z <= -3300 && (f & 1)) { held |= RE15_PAD_BIT_SQUARE; pr = RE15_PAD_BIT_SQUARE; }
        } else if (g_scd.message_active && f - letzte_bestaet >= 12) {
            held = pr = RE15_PAD_BIT_SQUARE; letzte_bestaet = f;
        }
        const int af_vorher = pl->anim_frame;
        bild(pr, held);
        const int offen = g_scd.message_active ? 1 : 0, id = g_scd.message_id;
        if (offen && !offen_vorher && id == 0 && r.f_msg0 < 0) { r.f_msg0 = f; frame_bei_msg0 = pl->anim_frame; }
        if (offen && id == 1 && r.f_msg1 < 0) r.f_msg1 = f;
        if (r.f_msg0 >= 0 && r.f_msg1 < 0 && f > r.f_msg0 && pl->anim_frame != af_vorher) r.luecke++;
        if (!offen && offen_vorher && id_vorher == 1 && r.f_zu < 0) r.f_zu = f;
        if (r.f_zu >= 0 && r.f_pos < 0 && pl->x == 3650) {
            r.f_pos = f; r.pos_x = pl->x; r.pos_z = pl->z; r.motion_pos = pl->motion;
        }
        offen_vorher = offen; id_vorher = id;
        if (r.f_pos >= 0 && f > r.f_pos + 2) break;
    }
    r.bild_msg0 = frame_bei_msg0;
    r.ok = r.f_msg0 >= 0 && r.f_msg1 > r.f_msg0 && r.f_zu > r.f_msg1 && r.f_pos > r.f_zu;
    s_r41 = 1;
    re15_msgtakt_r41_zuruecksetzen(); re15_pauseflags_clear();
    return r;
}

/* ============================================================================================ rennpfad */
typedef struct { int32_t x, z; int rot, mo, frac, aktiv; } rp_t;

static rp_t rp_bild(uint16_t held)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    bild(0, held);
    rp_t b = { pl->x, pl->z, (int)pl->rot_y, (int)pl->motion, (int)pl->anim_frac, re15_rennstopp_r41_aktiv() };
    return b;
}
static int rp_drehung(rp_t a, rp_t b) { int d = (b.rot - a.rot) & 0xfff; return d >= 0x800 ? d - 0x1000 : d; }
static int rp_weg(rp_t a, rp_t b)
{
    const long dx = (long)b.x - a.x, dz = (long)b.z - a.z;
    long w = 0; while ((w + 1) * (w + 1) <= dx * dx + dz * dz) w++;
    return (int)w;
}
/* Weg nach einer Drehung: der Vorschub rundet je Achse ab (player_common.c x += cos*v/4096), der Betrag weicht um <= 1 ab. */
static int rp_weg_um(rp_t a, rp_t b, int soll) { const int w = rp_weg(a, b); return w >= soll - 1 && w <= soll + 1; }
/* ROOM2020 Tunnel (z -3150, wie der Weg des Nutzers 11350 -> West), Blick 0 (+x), 8 Bilder Rennen. */
static rp_t rp_start(int waffe)
{
    extern void re15_player_set_equipped_weapon(int weapon_id);
    static int s_gemerkt = 0; static re15_game_state_t s_ausgang;
    if (!s_gemerkt) { s_ausgang = g_game; s_gemerkt = 1; } else g_game = s_ausgang;
    re15_msgtakt_r41_zuruecksetzen();
    re15_rennstopp_r41_zuruecksetzen();
    re15_game_flag_set(3, 0x72, 1);
    rp_t r = { 0, 0, 0, -1, 0, 0 };
    if (room_boot(0x2020, 4000, -3150, 0, 1) != 0) return r;
    re15_player_set_equipped_weapon(waffe);
    for (int f = 0; f < 3; f++) bild(0, 0);
    for (int f = 0; f < 8; f++) r = rp_bild(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS);
    return r;
}

static void teil_rennpfad(void)
{
    printf("== rennpfad ==\n");
    const uint16_t U = RE15_PAD_BIT_UP, X = RE15_PAD_BIT_CROSS, L = RE15_PAD_BIT_LEFT, M = RE15_PAD_BIT_R1;
    extern void re15_player_set_equipped_weapon(int weapon_id);
    for (int gegen = 0; gegen < 2; gegen++) {
        re15_rennstopp_r41_pruef_aus(gegen);
        const char *w = gegen ? "Gegenprobe ohne Modul" : "Modul";
        /* A: aus dem Rennen nur LINKS */
        rp_t r0 = rp_start(0), a1 = rp_bild(L), a2 = rp_bild(L), a3 = rp_bild(L);
        printf("  [%s] A Rennen->LINKS: Renn-Bild x %d mo %d | S1 dreh %d mo %d weg %d | S2 dreh %d mo %d weg %d | S3 dreh %d\n", w,
               (int)r0.x, r0.mo, rp_drehung(r0, a1), a1.mo, rp_weg(r0, a1), rp_drehung(a1, a2), a2.mo, rp_weg(a1, a2), rp_drehung(a2, a3));
        const int okA = r0.mo == 100 && rp_drehung(r0, a1) == -72 && a1.mo == 200 && a1.aktiv && rp_weg_um(r0, a1, 160) &&
                        rp_drehung(a1, a2) == -72 && a2.mo == 105 && !a2.aktiv && rp_weg(a1, a2) == 0 && rp_drehung(a2, a3) == -96;
        /* B: RENNEN los, OBEN+LINKS gehalten */
        rp_t s0 = rp_start(0), b1 = rp_bild(U | L), b2 = rp_bild(U | L), b3 = rp_bild(U | L);
        printf("  [%s] B Rennen->OBEN+LINKS: S1 dreh %d mo %d weg %d | S2 dreh %d mo %d weg %d | S3 dreh %d mo %d\n", w,
               rp_drehung(s0, b1), b1.mo, rp_weg(s0, b1), rp_drehung(b1, b2), b2.mo, rp_weg(b1, b2), rp_drehung(b2, b3), b3.mo);
        const int okB = rp_drehung(s0, b1) == -72 && b1.mo == 200 && rp_weg_um(s0, b1, 160) &&
                        rp_drehung(b1, b2) == -72 && b2.mo == 105 && rp_weg_um(b1, b2, 75) && rp_drehung(b2, b3) == -48;
        /* C: Stopp, dann OBEN+RENNEN */
        rp_t c0 = rp_start(0), c1 = rp_bild(0), c2 = rp_bild(U | X), c3 = rp_bild(U | X);
        printf("  [%s] C Stopp->OBEN+RENNEN: S1 mo %d weg %d | S2 mo %d weg %d | S3 mo %d weg %d\n", w,
               c1.mo, rp_weg(c0, c1), c2.mo, rp_weg(c1, c2), c3.mo, rp_weg(c2, c3));
        const int okC = c1.mo == 200 && rp_weg(c0, c1) == 160 && c2.mo == 105 && rp_weg(c1, c2) == 75 &&
                        c3.mo == 100 && rp_weg(c2, c3) == 200;
        /* D: ganzer Stopp, Folgebild = Stehen-Fall 0 */
        rp_t d[9]; d[0] = rp_start(0);
        for (int i = 1; i < 9; i++) d[i] = rp_bild(0);
        int okD = 1;
        static const int soll_weg[6] = { 160, 120, 80, 40, 0, 40 };
        for (int i = 1; i <= 6; i++) if (d[i].mo != 200 || rp_weg(d[i - 1], d[i]) != soll_weg[i - 1]) okD = 0;
        okD = okD && d[6].aktiv == 0 && d[7].mo == 200 && d[7].frac == 7 && rp_weg(d[6], d[7]) == 0 && d[8].frac == 6;
        printf("  [%s] D Stopp: weg %d %d %d %d %d %d (x %d -> %d), mo %d..%d, Folgebild mo %d Mischung %d, danach %d\n", w,
               rp_weg(d[0], d[1]), rp_weg(d[1], d[2]), rp_weg(d[2], d[3]), rp_weg(d[3], d[4]), rp_weg(d[4], d[5]), rp_weg(d[5], d[6]),
               (int)d[0].x, (int)d[6].x, d[1].mo, d[6].mo, d[7].mo, d[7].frac, d[8].frac);
        /* E: Zielen mit Waffe aus dem Rennen, LINKS gehalten */
        rp_t e0 = rp_start(3), e1 = rp_bild(U | X | M | L);
        printf("  [%s] E Rennen->Zielen+LINKS (Waffe 3): S1 dreh %d mo %d weg %d\n", w, rp_drehung(e0, e1), e1.mo, rp_weg(e0, e1));
        const int okE = rp_drehung(e0, e1) == -72 && e1.mo == 200 && rp_weg_um(e0, e1, 160);
        re15_player_set_equipped_weapon(0);
        if (!gegen) {
            PRUEF(okA, "A: aus dem Rennen LINKS: Stopp-Bild dreht 72 (@0x80073f25, @0x80032660-@0x800326a0), 160 Vorschub; "
                       "Folgebild 72 + Drehen 0x0301 (@0x80032750) ohne Schritt; danach 96 (Drehen-DECIDE)");
            PRUEF(okB, "B: RENNEN los, OBEN+LINKS: Stopp-Bild 72 statt 48, Folgebild 72 + Gehen 0x0101 (@0x80032738), danach 48");
            PRUEF(okC, "C: OBEN+RENNEN im Stopp: ein Bild Gehen (0x0101 @0x80032738), Rennen erst aus dem Gehen-DECIDE (@0x80032444)");
            PRUEF(okD, "D: 6 Stopp-Bilder mo 200 ohne Leerlauf-FSM, Folgebild Stehen-Fall 0 Mischung 7 (@0x80032074-@0x8003209c)");
            PRUEF(okE, "E: Zielen mit Waffe aus dem Rennen: Drehen 72 vor dem Zielen (@0x80032620 vor @0x80032708), dann Stopp");
        } else {
            PRUEF(!okA && !okB && !okC && !okD && !okE, "Gegenprobe ohne Modul (Stand vor Nachbesserung 3): A-E fallen durch (%d%d%d%d%d)",
                  okA, okB, okC, okD, okE);
        }
    }
    re15_rennstopp_r41_pruef_aus(0);
    re15_rennstopp_r41_zuruecksetzen();
    re15_msgtakt_r41_zuruecksetzen(); re15_pauseflags_clear();
}

static void teil_frage(void)
{
    printf("== frage ==\n");
    frage_t a = frage_lauf(1);
    printf("  Original-Reihenfolge: msg0 Bild %d (Geh-Bild %d), msg1 Bild %d, Frage zu %d, Pos_set %d (%d,%d) mo %d, Luecken %d\n",
           a.f_msg0, a.bild_msg0, a.f_msg1, a.f_zu, a.f_pos, a.pos_x, a.pos_z, a.motion_pos, a.luecke);
    PRUEF(a.ok, "Szene lief (Aktion im Gehen -> msg 0 -> Frage -> Ja -> Pos_set)");
    PRUEF(a.ok && a.luecke == 0, "kein Spieler-Bild zwischen Message_on 0 @0x0A5C und 1 @0x0A62 (Original n9..n47 Bild 9): %d", a.luecke);
    PRUEF(a.ok && a.f_pos == a.f_zu + 1, "Pos_set @0x0A8C im ersten Bild nach dem Schliessen (Original n94 -> n95): zu %d, Pos_set %d", a.f_zu, a.f_pos);
    PRUEF(a.ok && a.pos_z == -3018 && a.motion_pos == 200, "im Pos_set-Bild Geh-Stopp an der Kurbel (3650,-3018), Clip 3 = mo 200 (@0x80032328/@0x80032480, @0x80032088): z %d mo %d",
          a.pos_z, a.motion_pos);
    frage_t b = frage_lauf(0);
    printf("  alte Reihenfolge (Gegenprobe): msg0 %d, msg1 %d, zu %d, Pos_set %d, Luecken %d\n", b.f_msg0, b.f_msg1, b.f_zu, b.f_pos, b.luecke);
    PRUEF(b.ok && b.luecke >= 1 && b.f_pos == b.f_zu + 2, "Gegenprobe ohne Original-Reihenfolge: Luecken-Bild (%d) und Pos_set 2 Bilder nach dem Schliessen (%d)",
          b.luecke, b.f_pos - b.f_zu);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "msgtakt")    || !strcmp(teil, "alle")) teil_msgtakt();
    if (!strcmp(teil, "kamerahalt") || !strcmp(teil, "alle")) teil_kamerahalt();
    if (!strcmp(teil, "rennstopp")  || !strcmp(teil, "alle")) teil_rennstopp();
    if (!strcmp(teil, "renntempo")  || !strcmp(teil, "alle")) teil_renntempo();
    if (!strcmp(teil, "frage")      || !strcmp(teil, "alle")) teil_frage();
    if (!strcmp(teil, "rennpfad")   || !strcmp(teil, "alle")) teil_rennpfad();
    printf("%s: %d/%d bestanden\n", g_fail ? "FAIL" : "OK", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
