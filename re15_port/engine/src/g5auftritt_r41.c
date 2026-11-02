/*
 * RE1.5 Rebuilt — G5-Auftritt nach dem Rausschiessen der Riesenform (Kopf: include/re15_g5auftritt_r41.h,
 * Dossier analysis/befunde_runde41/G5_auftritt.md).
 *
 * RE2 ROOM7040 MAIN sub06 ab der Armierung, Bild fuer Bild (Sleep n = n Bilder, Wsleeping wartet auf 0x800cfbd8 & 0x20
 * @0x80053ac0-d0):
 *
 *   Bild    0  @0x018DA/@0x018DE  `2e 03 00 | 34 17 01 00`  Work_set(Gegner 0), Member_set(0x17,1) = G5 +0x1D4 Bit 0
 *   Bild  437  @0x0192A `32 00 64 19 00 00 ee a3`, @0x01932 `33 00 00 00 00 08 00 00`, @0x0193A `29 0a`
 *   Bild  807  @0x01950 `29 05`, @0x01956 `32 00 4c 2c 00 00 ee a3`
 *   Bild 1162  @0x01962 `29 06`, @0x0196C `32 00 02 3a 00 00 ee a3`, @0x01974 Dir_set 0x800, @0x0197C `42` Plc_ret,
 *              @0x01981 `3c 01` Cut_auto(1), @0x01983 `22 02 07 00`, @0x01987 `22 01 1b 00`
 *
 * Dazwischen laeuft der G5 sein Intro (Sub 2 [T0..T14], enemy_ai_boss_g5.c) — unveraendert, er ist Overlay-Code, kein Skript.
 *
 * ⛔ PORT-WAHL (je im Code markiert, Begruendung im Dossier):
 *   - Die Schnitte 9 und 10 sind RE2-Szenenkameras (RVD nur Ankerzone) ohne RE1.5-Gegenstueck: Szenen-Schnitte 16/17 = RE2s Kamera
 *     mit RE2s Hintergrund und Masken (der Wagen ist an der Stelle zellengleich, ROOM5090 SCA [21]/[25] = room7040 [2]/[4]). Erster
 *     Versuch mit Cut 11 fuer Schnitt 10: Leon auf RE2s 6500 lag 1126 vor der Kamera rechts ausser Bild, der G5 fuellte das Bild
 *     (Lauf leon, F1995-F2364, Dossier 3).
 *   - Schnitte 5 und 6 (Spielkameras) -> Cut 12, die RE1.5-Kamera desselben Blicks nach Westen.
 *   - Spielerlage im Cut-12-Teil 13873 statt 11340/14850 (s. G5A_X_SCHNITT5).
 *   - Die Spieler-Gesten (Plc_motion/Plc_neck/Plc_cnt) sind RE2-PL0-Clips und werden nicht uebertragen: Leon steht.
 *   - Der Teil VOR der Armierung (Deckenplatte, Dach-Tentakel, Lauf aus dem Hinterwagen) ist im Port die Rueckstoss-Szene der
 *     Form-Spur; diese Szene beginnt mit der Armierung.
 */
#include "re15_g5auftritt_r41.h"
#include "re15_birkin_formen.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"        /* g_engine.frame_count (Protokollzeile) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);   /* enemy_ai_boss_g5.c */
void re15_player_aim_interrupt(void);                         /* player_common.c */
void re15_player_event_reach_end(void);                       /* player_common.c */

/* RE2 room7040 RID Schnitt 9 @0x184 = `00 00 b7 73 8c 0a 00 00 8e fc ff ff 84 8e ff ff d0 f9 ff ff 00 ee ff ff fe c5 ff ff d8 0d 00 00`
 * (RID-Offset RDT-Kopf [7] = 0x64, 32 Byte je Satz). fov 0x73B7 -> H = 231 (fov >> 7, camera_common.c). Der PRI-Zeiger 0xDD8 gilt
 * in RE2s RDT; die Plattform liest den Abschnitt aus RE15_G5A_PRI_DATEI, deshalb hier "kein Abschnitt". */
static const re15_camera_cut_t k_re2_schnitt9 = {
    0u, 29623u,
    2700, -882, -29052,
    -1584, -4608, -14850,
    0xFFFFFFFFu
};
/* RE2 room7040 RID Schnitt 10 @0x1A4 = `00 00 3c 68 f2 22 00 00 58 fc ff ff d2 9a ff ff 52 f9 ff ff 26 f2 ff ff 46 ab ff ff 2c 0f 00 00`:
 * fov 0x683C (H 208), Auge (8946,-936,-25902), Ziel (-1710,-3546,-21690). */
static const re15_camera_cut_t k_re2_schnitt10 = {
    0u, 26684u,
    8946, -936, -25902,
    -1710, -3546, -21690,
    0xFFFFFFFFu
};

/* Spielerlagen. z -23570 = 0xA3EE in jedem Pos_set/Plc_dest von sub06 (@0x018B0, @0x0192A, @0x01956, @0x0196C). */
#define G5A_Z               (-23570)
#define G5A_GIER_WEST       0x800       /* Dir_set @0x01932 / @0x01974 `33 00 00 00 00 08 00 00` */
/* Bild 0: das Plc_dest-Ziel @0x018B0 `40 00 05 20 78 05 ee a3` (x 0x0578), auf dem Leon bei der Armierung steht. */
#define G5A_X_ARMIERUNG     1400
#define G5A_X_SCHNITT10     6500        /* @0x0192A */
/* ⛔ PORT-WAHL: RE2 stellt Leon fuer Kamera 5 auf 11340 (@0x01956) und fuer Kamera 6 auf 14850 (@0x0196C). RE1.5 hat fuer beide
 * nur Cut 12, und 14850 laege dort im Zonenstreifen 12 -> 13 (RVD @0x4CC, Westkante x 14000..14100), die Kampfkamera wuerde sofort
 * nach Osten (Cut 13) springen und den G5 hinter sich lassen. Leon steht deshalb den ganzen Cut-12-Teil auf der Ostkante des
 * Streifens 13 -> 12 (RVD @0x4F4 x 13873), der oestlichsten Lage, die der Raum beim Betreten von Osten in Cut 12 zeigt
 * (Kampfstart sub01 @0x1290 Cmp(Cut == 12)) - 977 vor RE2s Endlage. Gesetzt wird wie in RE2 zweimal, bei Bild 807 (@0x01956) und am
 * Szenenende (@0x0196C): die zweite Setzung nimmt zurueck, was Arme und Koerper in den 355 Bildern verschoben haben (gemessen bis
 * x 14957, Riegel test_r41_g5auftritt), sonst stuende Leon am Ende im Streifen 12 -> 13. */
#define G5A_X_SCHNITT5      13873

/* Kameras. RE2-Schnitte 9 und 10 sind Szenenkameras ohne RE1.5-Gegenstueck -> Szenen-Schnitte 16/17 (re15_g5auftritt_r41.h).
 * ⛔ PORT-WAHL: RE2-Schnitte 5 (14220,-1854,-24606)->(3582,..) und 6 (21042,-882,-24390)->(11898,..) sind Spielkameras; RE1.5s Kamera
 * desselben Blicks ist Cut 12 (18526,-2656,-24660)->(1496,1514,-20370) (RID @0x60 + 12*32), fuer beide. */
#define G5A_SCHNITT5_RE15   0x0C
#define G5A_LICHT_RE15      0x0B   /* Lichtsatz des Wagenendes (Cut 11 = RE2-Spielkamera 4 am selben Ort) */

/* bild = KI-Bild der Szene; Bild 0 ist das Bild der Armierung, in dem der G5 [T0] laeuft. `frisch`: der erste Tick nach dem Start
 * zaehlt nicht weiter - gleich, ob der Start aus re15_bf_raumtick kommt (Tick im selben Durchlauf) oder von aussen (Tick im naechsten). */
static struct { int laeuft; int bild; int slot; int frisch; } s_g5a = { 0, -1, -1, 0 };

static int raum_5090(void) { return (g_current_room_id & 0xFFFEu) == 0x5090u; }

const re15_camera_cut_t *re15_g5a_schnitt(int cut)
{
    if (!raum_5090()) return (const re15_camera_cut_t *)0;
    if (cut == RE15_G5A_SCHNITT9)  return &k_re2_schnitt9;
    if (cut == RE15_G5A_SCHNITT10) return &k_re2_schnitt10;
    return (const re15_camera_cut_t *)0;
}

int re15_g5a_re2_schnitt(int cut)
{
    if (!raum_5090()) return -1;
    return cut == RE15_G5A_SCHNITT9 ? 9 : (cut == RE15_G5A_SCHNITT10 ? 10 : -1);
}

/* ⛔ PORT-WAHL: RE2s Lichtsaetze von Schnitt 9/10 (light.lit @0x500 + n*40) beleuchten RE2-Figuren. Im Port stehen in diesen Bildern
 * die RE1.5-Spielfigur und der G5 desselben Wagenendes, das Cut 11 beleuchtet; der Lichtsatz ist der von Cut 11. */
int re15_g5a_licht_schnitt(int cut) { return re15_g5a_re2_schnitt(cut) >= 0 ? G5A_LICHT_RE15 : cut; }

int re15_g5a_laeuft(void) { return s_g5a.laeuft; }
int re15_g5a_bild(void)   { return s_g5a.laeuft ? s_g5a.bild : -1; }

/* Cut_chg-Zwilling wie bf_kamera (birkin_formen.c, Abnahme 1 M9): Zonen aus, Schnitt anfordern. Cut_auto(0) steht in RE2 am
 * Szenenkopf @0x01780 `3c 00`. */
static void kamera(int cut)
{
    g_scd.cut_auto_enabled = 0;
    if ((int)g_scd.work_vars[0x0A] != cut || g_scd.cam_id != cut) { g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 1; }
}

/* Pos_set + Dir_set auf den Spieler (Work_set(1,0) @0x01952 usw.): X/Y/Z und alle drei Winkel (op_pos_set / op_dir_set). Die Pose
 * geht auf Stehen, wie der Port jede Szene ohne eigene Spieler-Clips haelt (player_common.c, player_mode 2). */
static void spieler_setzen(int32_t x)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->y = 0; pl->z = G5A_Z;
    pl->rot_x = 0; pl->rot_y = G5A_GIER_WEST; pl->rot_z = 0;
    pl->walk_active = 0;
    re15_actor_set_motion(pl, 200);                      /* IDLE-Waechter 200 (player_common.c RE15_MOTION_IDLE) */
    pl->anim_frame = 0; pl->anim_frac = 7;
}

void re15_g5a_reset(void)
{
    if (s_g5a.laeuft) {                                  /* Raumwechsel mitten in der Szene: Sperre nicht stehen lassen */
        re15_game_flag_set(2, 7, 0);
        re15_game_flag_set(1, 27, 0);
    }
    s_g5a.laeuft = 0; s_g5a.bild = -1; s_g5a.slot = -1; s_g5a.frisch = 0;
}

void re15_bf_g5_auftritt_start(int slot)
{
    if (s_g5a.laeuft || !raum_5090() || slot < 1 || slot >= RE15_ACTOR_MAX) return;
    s_g5a.laeuft = 1; s_g5a.bild = 0; s_g5a.slot = slot; s_g5a.frisch = 1;
    /* Szenenkopf sub06 @0x01778 `22 02 07 01` / @0x0177C `22 01 1b 01`: Spielersperre + Szenenbalken (re15_cine_active) */
    re15_game_flag_set(2, 7, 1);
    re15_game_flag_set(1, 27, 1);
    re15_player_aim_interrupt();                         /* die Rueckstoss-Szene liess Leon zielen und feuern */
    spieler_setzen(G5A_X_ARMIERUNG);
    kamera(RE15_G5A_SCHNITT);                            /* @0x01892 `29 09`: Schnitt 9 steht bei der Armierung schon */
    /* @0x018DE Member_set(0x17,1): Typwechsel + Armierung (grid 0x13 = +0x1D4-Bit-0-Analogon, enemy_ai_boss_g5.c). Der G5-Tick
     * desselben KI-Durchlaufs laeuft danach den Konstruktor und [T0] (X -9000 @0x801011d0, Clip 1, SE 10). */
    re15_bf_morph_zu_g5(slot);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[g5auftritt] F%u Raum %04X: Armierung, Schnitt %d (RE2 room7040 Cut 9), Slot %d\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, RE15_G5A_SCHNITT, slot);
#endif
}

void re15_g5a_tick(void)
{
    if (!s_g5a.laeuft) return;
    if (!raum_5090()) { re15_g5a_reset(); return; }
    if (s_g5a.frisch) s_g5a.frisch = 0; else s_g5a.bild++;
    const int b = s_g5a.bild;
    if (b < RE15_G5A_BILD_SCHNITT10) {
        kamera(RE15_G5A_SCHNITT);
    } else if (b < RE15_G5A_BILD_SCHNITT5) {
        if (b == RE15_G5A_BILD_SCHNITT10) spieler_setzen(G5A_X_SCHNITT10);
        kamera(RE15_G5A_SCHNITT10);
    } else if (b < RE15_G5A_BILD_ENDE) {
        if (b == RE15_G5A_BILD_SCHNITT5) spieler_setzen(G5A_X_SCHNITT5);
        kamera(G5A_SCHNITT5_RE15);
    } else {
        /* Szenenende @0x0197C..@0x01987. Plc_ret = RE1.5-Handler 0x80041f88 (scd_vm.c op_plc_ret): Spieler +0x4 = 1, +0x5..+0x7 = 0,
         * Kommandowort live; Pose Stehen. Set(1,4,1) @0x0197D hat im Port keinen Leser und wird nicht gesetzt. */
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        spieler_setzen(G5A_X_SCHNITT5);                  /* @0x0196C Pos_set + @0x01974 Dir_set 0x800 (Lage: PORT-WAHL oben) */
        re15_player_event_reach_end();
        re15_actor_set_motion(pl, 200); pl->anim_frame = 0;
        pl->state = 1; pl->sub_state_1 = 0; pl->sub_state_2 = 0; pl->sub_state_3 = 0;
        g_scd.player_mode = 1;
        /* Kamera: RE2 schaltet auf Kamera 6 und gibt die Zonen frei; im Port der Zonenschnitt der Spielerlage (Cut 12 bei 13873). */
        kamera(re15_bf_schnitt_aus_lage(pl->x));
        g_scd.cut_auto_enabled = 1;                      /* @0x01981 `3c 01` */
        re15_game_flag_set(2, 7, 0);                     /* @0x01983 */
        re15_game_flag_set(1, 27, 0);                    /* @0x01987 */
#ifdef RE15_PLATFORM_PC
        {   int sub = -1, ph = -1;
            (void)re15_g5_boss_zustand(&sub, &ph, 0);
            fprintf(stderr, "[g5auftritt] F%u Szenenende Bild %d: G5 Sub %d Ph %d, Spieler x %d, Cut %d\n",
                    (unsigned)g_engine.frame_count, b, sub, ph, (int)pl->x, (int)g_scd.cam_id); }
#endif
        s_g5a.laeuft = 0; s_g5a.slot = -1;
    }
}
