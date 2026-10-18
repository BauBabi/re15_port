/* test_r39_funk10f1.c — RIEGEL: Funkszene ROOM10F1 (Runde 39, Spur I "funk10f1").
 *
 * NUTZER-VORGABE (analysis/befunde_runde39/AUFTRAG.md): "In Room 10F1 möchte ich, das wenn Elza ihn betritt eine
 * Cutscene läuft wo sie Rechts zum Kommunikationssystem geht und von der Animation her drauf drückt und sagt "Hello,
 * anyone, can you hear me? We Need help!" / Dann kommt "..." / Dann "Uhh, this Thing is broken!". / Suche die dir
 * passenden Animationen dafür heraus."  Dossier analysis/befunde_runde39/I_funk10f1.md, Belege include/re15_funk10f1_r39.h,
 * Programm gen/funk10f1_r39_szene.inc (Generator tools/r39_funk10f1/szene_bauen.py).
 *
 * Verfahren wie test_r35_cut10f0.c: ausgelieferte RDT laden, Raum ueber scd_room_reenter aufbauen (dort feuert
 * re15_funk10f1_r39_install das Ereignis 27), jedes Bild in der Reihenfolge von platform/pc/main.c. Gemessen wird die
 * MECHANIK (Opcode-Form gegen die Original-Bytes, Weg gegen die Kollision, Nachrichten-/Stimmen-/Posenfolge in der VM).
 *
 * Teile (je ein ctest-Eintrag):
 *   form     Programm: Opcode-Laengen schliessen auf Evt_end; Set(9,70) zuerst; Rahmen/Schwanz; Druck-, Senk-,
 *            Umseh-, Stirn- und Schwanz-Opcodes BYTEGLEICH mit ROOM1031 sub13 (@0x029A4/@0x029A8/@0x029BE/@0x029C2/
 *            @0x02A32/@0x02A3A/@0x02A00/@0x02A48..@0x02A59); Warteschleife = sub14 @0x02A5A; Weg frei von allen
 *            Kollisionszellen (Abstand > gemessener Spielerradius 468); Ziel in der Text-Zone des Kommunikationssystems
 *   texte    drei Nachrichten = Nutzer-Wortlaut, Kopf = ROOM1031 msg 19 @0x2FA7 (Elza, Farbe 01), Ende 04 01 01 63
 *   szene    echte VM: Szene beim Raumaufbau, (9,70) ab dem ersten Bild, Elza dreht nach RECHTS (+Z) und geht zur
 *            Konsole, Kamera 1, Druck (Motion 11 + Halten) WAEHREND msg 40, dann Senken (0 rueckwaerts), msg 41 mit
 *            Hand an der Huefte (9) + Umsehen, msg 42 mit Hand an der Stirn (12); Stimme je Zeile angefordert
 *            (SCD_AUDIO_VOICE_ON 40, 41, 42); am Ende Spieler frei, Balken weg
 *   einmal   zweiter Raumaufbau: keine Szene, keine Nachricht, keine Leihe; Leon ROOM10F0: nie
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
#include "re15_map_hint.h"
#include "re15_funk10f1_r39.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static int                s_stimme[8];
static int                s_n_stimme = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint8_t *datei(uint16_t room, size_t *n)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    return slurp(rp, n);
}

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    free(s_raw); s_raw = datei(room, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (Muster test_r35_cut10f0.c frame). */
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e))
        if (e.kind == SCD_AUDIO_VOICE_ON && s_n_stimme < 8) s_stimme[s_n_stimme++] = e.sample_id;
}

/* Raum aufbauen OHNE die Flags anzufassen (test_r35_cut10f0.c room_boot). */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = 0; s_cine_was = 0; s_n_stimme = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int op_len(const uint8_t *pc)
{
    static const uint8_t k[0x5F] = {
        1,2,1,4,4,2,4,4, 2,4,3,1,1,6,2,4, 2,4,2,4,6,4,2,6, 2,2,2,6,1,1,1,1,
        1,4,4,6,4,3,6,4, 1,2,1,4,20,34,3,4, 1,1,8,8,4,3,12,4, 12,4,16,32,2,3,6,4,
        8,10,1,4,20,3,10,2, 16,8,2,3,18,10,5,22, 22,4,4,3,6,6,6,4, 4,4,6,4,4,4,4 };
    uint8_t op = pc[0];
    if (op >= 0x5F) return -1;
    if (op == 0x2C) return (pc[3] & 0x80) ? 28 : 20;
    if (op == 0x3B) return (pc[3] & 0x80) ? 40 : 32;
    if (op == 0x50) return (pc[3] & 0x80) ? 30 : 22;
    return (int)k[op];
}

static int16_t s16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

/* Kleinster Abstand des Punkts (x,z) zur Zelle [x0,x1]x[z0,z1] (0 = darin). */
static double abstand(double x, double z, int x0, int x1, int z0, int z1)
{
    double dx = x < x0 ? x0 - x : (x > x1 ? x - x1 : 0.0);
    double dz = z < z0 ? z0 - z : (z > z1 ? z - z1 : 0.0);
    return sqrt(dx * dx + dz * dz);
}

/* ================================================================================================ */
static void teil_form(void)
{
    int plen = 0; const uint8_t *p = re15_funk10f1_r39_programm(&plen);
    size_t n31 = 0; uint8_t *d = datei(0x1031, &n31);
    if (!d || n31 < 0x3000) { PRUEF(0, "ROOM1031.RDT ladbar"); return; }
    printf("  Programm %d Bytes\n", plen);
    int off = 0, ok = 1, ende = -1, n_ck_ok = 1, n_msg = 0, msgs[4] = { -1, -1, -1, -1 };
    int dest_n = 0, dest_mode[8], dest_x[8], dest_z[8];
    int off_druck = -1, off_senk = -1, off_huefte = -1, off_neck = -1, off_stirn = -1, off_cut1 = -1;
    int off_msg[3] = { -1, -1, -1 }, off_schwanz = -1, off_warte = -1;
    while (off < plen) {
        int l = op_len(p + off);
        if (l <= 0 || off + l > plen) { ok = 0; printf("  Opcode 0x%02X @+%04X unbekannt\n", p[off], off); break; }
        const uint8_t op = p[off];
        if (op == 0x2B && n_msg < 4) { if (n_msg < 3) off_msg[n_msg] = off; msgs[n_msg++] = p[off + 1]; }
        if (op == 0x21 && !(p[off + 1] == 5 && p[off + 2] == 0x20 && p[off + 3] == 0)) n_ck_ok = 0;
        if (op == 0x11 && off_warte < 0) off_warte = off;
        if (op == 0x40 && dest_n < 8) {
            dest_mode[dest_n] = p[off + 2]; dest_x[dest_n] = s16(p + off + 4); dest_z[dest_n] = s16(p + off + 6);
            if (p[off + 3] != 0x20) ok = 0;
            dest_n++;
        }
        if (op == 0x29 && p[off + 1] == 1 && off_cut1 < 0) off_cut1 = off;
        if (op == 0x3F && p[off + 2] == 11) off_druck = off;
        if (op == 0x3F && p[off + 2] == 0) off_senk = off;
        if (op == 0x3F && p[off + 2] == 9) off_huefte = off;
        if (op == 0x3F && p[off + 2] == 12) off_stirn = off;
        if (op == 0x41 && p[off + 1] == 4) off_neck = off;
        if (op == 0x3C && off > plen - 20 && off_schwanz < 0) off_schwanz = off;
        if (op == 0x01) { ende = off; break; }
        off += l;
    }
    PRUEF(ok && ende == plen - 2, "Opcode-Laengen schliessen genau auf das Evt_end am Ende (+%04X); Plc_dest-Bit 0x20", ende);
    PRUEF(plen >= 12 && !memcmp(p, "\x22\x09\x46\x01", 4), "ERSTES Opcode = Set(9,70)=1 `22 09 46 01` (Einmal-Riegel)");
    PRUEF(!memcmp(p + 4, d + 0x02986, 4) && !memcmp(p + 8, d + 0x0298A, 4) && !memcmp(p + 12, d + 0x02990, 4),
          "Rahmen Set(2,7)/Set(1,27)/Work_set(1,0) == ROOM1031 sub13 @0x02986/@0x0298A/@0x02990");
    PRUEF(n_ck_ok && off_warte > 0 && !memcmp(p + off_warte, d + 0x02A5A, 12),
          "Warteschleife == ROOM1031 sub14 @0x02A5A `11 00 08 00 02 00 12 04 21 05 20 00`, jedes Ck = (5,0x20)");
    PRUEF(n_msg == 3 && msgs[0] == RE15_FUNK10F1_MSG_HALLO && msgs[1] == RE15_FUNK10F1_MSG_PUNKTE &&
          msgs[2] == RE15_FUNK10F1_MSG_KAPUTT, "Message_on genau 40 -> 41 -> 42 (%d Stueck)", n_msg);
    /* Weg: 9 -> 4 -> 4 -> 4 -> 9; der erste Schritt dreht nach RECHTS (+Z) */
    PRUEF(dest_n == 5 && dest_mode[0] == 9 && dest_mode[1] == 4 && dest_mode[2] == 4 && dest_mode[3] == 4 && dest_mode[4] == 9,
          "Plc_dest-Folge drehen/gehen/gehen/gehen/drehen (ROOM1031 sub13 @0x029CC/@0x029D6/@0x029E0)");
    PRUEF(dest_n == 5 && dest_z[0] > -350 + 2000 && dest_x[3] == RE15_FUNK10F1_PLATZ_X && dest_z[3] == RE15_FUNK10F1_PLATZ_Z &&
          dest_x[4] == RE15_FUNK10F1_KONSOLE_X && dest_z[4] == RE15_FUNK10F1_PLATZ_Z,
          "erster Wegpunkt rechts vom Eintritt (z %d > -350, Blick -X => rechts = +Z), Platz (%d,%d), Blick auf (%d,%d)",
          dest_n ? dest_z[0] : 0, dest_n > 3 ? dest_x[3] : 0, dest_n > 3 ? dest_z[3] : 0, dest_n > 4 ? dest_x[4] : 0,
          dest_n > 4 ? dest_z[4] : 0);
    /* Platz in der Text-Zone des Kommunikationssystems: Aot_set Slot 2 @0x00F72 (x 7000..7800, z 2600..12300) liegt
     * 620 vor dem Spieler (Standplatz-Test rotate((620,0)), aot_common.c) */
    {
        size_t nf = 0; uint8_t *f1 = datei(RE15_FUNK10F1_RAUM, &nf);
        int zx = f1 ? s16(f1 + 0x00F72 + 6) : 0, zz = f1 ? s16(f1 + 0x00F72 + 8) : 0;
        int zw = f1 ? s16(f1 + 0x00F72 + 10) : 0, zd = f1 ? s16(f1 + 0x00F72 + 12) : 0;
        const int sx = RE15_FUNK10F1_PLATZ_X + 620, sz = RE15_FUNK10F1_PLATZ_Z;
        PRUEF(f1 && f1[0x00F72] == 0x2C && f1[0x00F72 + 1] == 2 && f1[0x00F72 + 14] == 0 &&
              sx >= zx && sx <= zx + zw && sz >= zz && sz <= zz + zd,
              "Pruefpunkt vor dem Platz (%d,%d) liegt in Slot 2 msg 0 'A large communications device' (%d..%d, %d..%d)",
              sx, sz, zx, zx + zw, zz, zz + zd);
        /* Kollision: jede Strecke des Wegs (Eintritt (8400,-350) -> ... -> Platz) haelt > 468 Abstand zu jeder Zelle der
         * SCA (@RDT+0x20, Satz 12 B: w d x z typ ...; gemessener Schiebe-Radius 7400 - 6932 = 468, Dossier §3.1) */
        double minab = 1e9; int minzelle = -1;
        if (f1) {
            const uint32_t sca = (uint32_t)(f1[0x20] | (f1[0x21] << 8) | (f1[0x22] << 16) | ((uint32_t)f1[0x23] << 24));
            const uint32_t cnt = (uint32_t)(f1[sca + 4] | (f1[sca + 5] << 8));
            int px[5] = { 8400, dest_x[1], dest_x[2], dest_x[3], 0 }, pz[5] = { -350, dest_z[1], dest_z[2], dest_z[3], 0 };
            for (int s = 0; s < 3; s++)
                for (int k = 0; k <= 100; k++) {
                    double x = px[s] + (px[s + 1] - px[s]) * k / 100.0, z = pz[s] + (pz[s + 1] - pz[s]) * k / 100.0;
                    for (uint32_t c = 0; c < cnt; c++) {
                        const uint8_t *q = f1 + sca + 24 + c * 12;
                        int w = q[0] | (q[1] << 8), dd = q[2] | (q[3] << 8), cx = s16(q + 4), cz = s16(q + 6);
                        double a = abstand(x, z, cx, cx + w, cz, cz + dd);
                        if (a < minab) { minab = a; minzelle = (int)c; }
                    }
                }
        }
        PRUEF(minab > 468.0, "Weg frei: kleinster Abstand zu einer Kollisionszelle %.0f (Zelle %d) > Spielerradius 468",
              minab, minzelle);
        free(f1);
    }
    /* Gesten bytegleich mit Elzas Szene ROOM1031 sub13 */
    PRUEF(off_cut1 > 0 && off_druck == off_cut1 + 2, "Cut_chg 1 (Dialogkamera) direkt vor dem Druck");
    PRUEF(off_druck > 0 && !memcmp(p + off_druck, d + 0x029A4, 4) && !memcmp(p + off_druck + 4, d + 0x029A8, 4) &&
          !memcmp(p + off_druck + 8, d + 0x029AC, 4),
          "DRUCK = Plc_motion(0,11) + Plc_flg 4 + Sleep 20 == ROOM1031 @0x029A4/@0x029A8/@0x029AC (Rolltor-Notschalter)");
    PRUEF(off_druck > 0 && off_msg[0] == off_druck + 12 && off_senk > off_msg[0] && off_senk < off_msg[1],
          "msg 40 WAEHREND des Drueckens (nach dem Griff, vor dem Senken)");
    PRUEF(off_senk > 0 && !memcmp(p + off_senk, d + 0x029BE, 4) && !memcmp(p + off_senk + 4, d + 0x029C2, 4) &&
          !memcmp(p + off_senk + 8, d + 0x029C6, 4),
          "SENKEN = Plc_motion(0,0) + Plc_flg 0x80 + Sleep 30 == ROOM1031 @0x029BE/@0x029C2/@0x029C6");
    PRUEF(off_huefte == off_msg[1] + 4 && !memcmp(p + off_huefte, d + 0x02A32, 4) && !memcmp(p + off_huefte + 4, d + 0x02A36, 4) &&
          off_neck == off_huefte + 8 && !memcmp(p + off_neck, d + 0x02A3A, 10) && !memcmp(p + off_neck + 10, d + 0x02A44, 4),
          "msg 41 '...' = Huefte + Plc_neck 4 (horchen/umsehen) == ROOM1031 @0x02A32..@0x02A44 (nach 'Hey, is anyone here?!')");
    PRUEF(off_stirn == off_msg[2] + 4 && !memcmp(p + off_stirn, d + 0x02A00, 8),
          "msg 42 'broken' = Plc_motion(0,12) Hand an die Stirn + Sleep 100 == ROOM1031 @0x02A00/@0x02A04");
    PRUEF(off_schwanz > 0 && off_schwanz + 18 == plen && !memcmp(p + off_schwanz, d + 0x02A48, 18),
          "Schwanz Cut_auto 1 / Set(2,7)=0 / Set(1,27)=0 / Work_set / Plc_ret / Evt_end == ROOM1031 @0x02A48..@0x02A59");
    free(d);
}

static int text_ist(int id, const char *soll)
{
    const char *t = re15_msg_get_text(id);
    printf("    msg %2d \"%s\"\n", id, t ? t : "(keiner)");
    if (!t) return 0;
    /* Zeilenumbruch -> Leerzeichen (Muster test_r35_cut10f0.c text_normal) */
    char a[256]; size_t o = 0; int sp = 1;
    for (; *t && o + 1 < sizeof a; t++) {
        char c = (*t == '\n' || *t == '\r' || *t == '\t') ? ' ' : *t;
        if (c == ' ') { if (sp) continue; sp = 1; } else sp = 0;
        a[o++] = c;
    }
    while (o > 0 && a[o - 1] == ' ') o--;
    a[o] = 0;
    return strcmp(a, soll) == 0;
}

static void teil_texte(void)
{
    re15_game_state_init();
    if (room_boot(RE15_FUNK10F1_RAUM, 8400, -350, 2048) != 0) return;
    PRUEF(text_ist(RE15_FUNK10F1_MSG_HALLO, "Elza: Hello, anyone, can you hear me? We need help!"), "msg 40 woertlich (Nutzer)");
    PRUEF(text_ist(RE15_FUNK10F1_MSG_PUNKTE, "Elza: ..."), "msg 41 woertlich (Nutzer)");
    PRUEF(text_ist(RE15_FUNK10F1_MSG_KAPUTT, "Elza: Uhh, this thing is broken!"), "msg 42 woertlich (Nutzer)");
    size_t n31 = 0; uint8_t *d = datei(0x1031, &n31);
    int form = d != NULL;
    for (int id = RE15_FUNK10F1_MSG_HALLO; id <= RE15_FUNK10F1_MSG_KAPUTT && d; id++) {
        int n = 0; const uint8_t *b = re15_funk10f1_r39_meldung(id, &n);
        if (!b || n < 16 || memcmp(b, d + 0x2FA7, 12) != 0 || memcmp(b + n - 4, d + 0x2FA7 + 0x21, 4) != 0 ||
            re15_msg_compute_duration(b, (size_t)n, 0) <= 0) form = 0;
    }
    PRUEF(form, "Kopf `04 00 05 01 'Elza' 16 05 00 00` und Ende `04 01 01 63` == ROOM1031 msg 19 @0x2FA7/@0x2FC8, Dauer > 0");
    PRUEF(re15_funk10f1_r39_meldung(39, NULL) == NULL && re15_funk10f1_r39_meldung(43, NULL) == NULL,
          "nur die Ids 40..42 (VERTRAG Runde 39 1.2: Spur I = 40..44)");
    free(d);
}

/* ---- Die Szene in der VM fahren ------------------------------------------------------------------- */
typedef struct {
    int gesehen_bild;            /* erstes Bild mit (9,70) = 1 */
    int msg_folge[8], n_msg;     /* Folge der angezeigten Nachrichten-Ids */
    int mo_folge[16], n_mo;      /* Folge der Spieler-Motions (nur Wechsel) */
    int halten_bei_40;           /* Bilder mit msg 40 aktiv UND Motion 11 UND Plc_flg 4 */
    int rueck_bei_0;             /* Bilder mit Motion 0 rueckwaerts (0x80) */
    int druck_x, druck_z, druck_rot, druck_cam;
    int yaw_nach_dreh;           /* Gierung nach der ersten Drehung (Beginn des ersten Gehens) */
    int z_max_weg0;              /* z bei Ankunft am ersten Wegpunkt */
    int ende_bild, frei_bild;    /* Faden zu Ende / Spieler frei (player_mode 0) */
    int balken_waehrend, balken_ende;
} lauf_t;

static void lauf(lauf_t *L, int max_bilder)
{
    memset(L, 0, sizeof *L);
    L->gesehen_bild = L->ende_bild = L->frei_bild = -1; L->yaw_nach_dreh = -1; L->druck_cam = -1;
    int letzte_msg = -1, letzte_mo = -1, war = 0;
    for (int f = 0; f < max_bilder; f++) {
        frame();
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        const int laeuft = re15_funk10f1_r39_programm_laeuft();
        if (laeuft) war = 1;
        if (L->gesehen_bild < 0 && re15_game_flag_get(RE15_FUNK10F1_GESEHEN_BANK, RE15_FUNK10F1_GESEHEN_BIT))
            L->gesehen_bild = f;
        if (g_scd.message_active && (int)g_scd.message_id != letzte_msg && L->n_msg < 8)
            L->msg_folge[L->n_msg++] = letzte_msg = (int)g_scd.message_id;
        if ((int)pl->motion != letzte_mo && L->n_mo < 16) L->mo_folge[L->n_mo++] = letzte_mo = (int)pl->motion;
        if (L->yaw_nach_dreh < 0 && pl->motion == RE15_PLAYER_MOTION_WALK && pl->z > -300) L->yaw_nach_dreh = pl->rot_y & 4095;
        if (pl->motion == 11 && g_scd.message_active && g_scd.message_id == RE15_FUNK10F1_MSG_HALLO &&
            (pl->anim_flags & 0x04)) {
            L->halten_bei_40++;
            L->druck_x = pl->x; L->druck_z = pl->z; L->druck_rot = pl->rot_y & 4095; L->druck_cam = (int)g_scd.cam_id;
        }
        if (pl->motion == 0 && (pl->anim_flags & 0x80)) L->rueck_bei_0++;
        if (laeuft && re15_game_flag_get(1, 27) && re15_game_flag_get(2, 7)) L->balken_waehrend++;
        if (war && !laeuft && L->ende_bild < 0) {
            L->ende_bild = f;
            L->balken_ende = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        }
        if (L->ende_bild >= 0 && g_scd.player_mode == 0 && L->frei_bild < 0) L->frei_bild = f;
        if (L->frei_bild >= 0 && f > L->frei_bild + 5) break;
    }
}

static int folge_enthaelt(const int *a, int n, const int *soll, int m)
{
    int j = 0;
    for (int i = 0; i < n && j < m; i++) if (a[i] == soll[j]) j++;
    return j == m;
}

static void teil_szene(void)
{
    re15_game_state_init();
    if (room_boot(RE15_FUNK10F1_RAUM, 8400, -350, 2048) != 0) return;
    PRUEF(re15_funk10f1_r39_programm_laeuft(), "Raumaufbau ROOM10F1 mit (9,70)=0: Funkszene laeuft (Ereignis 27)");
    PRUEF(re15_funk10f1_r39_rbj_quelle(RE15_FUNK10F1_RAUM) == RE15_FUNK10F1_RBJ_RAUM,
          "Gestenblock-Leihe ROOM1031 (Elzas Bibliothek), solange die Szene aussteht");
    lauf_t L; lauf(&L, 3000);
    printf("    Nachrichten:"); for (int i = 0; i < L.n_msg; i++) printf(" %d", L.msg_folge[i]); printf("\n");
    printf("    Motions:"); for (int i = 0; i < L.n_mo; i++) printf(" %d", L.mo_folge[i]); printf("\n");
    printf("    Stimme:"); for (int i = 0; i < s_n_stimme; i++) printf(" %d", s_stimme[i]); printf("\n");
    printf("    Druck bei (%d,%d) Gierung %d Cut %d, %d Bilder; Ende Bild %d, frei Bild %d\n", L.druck_x, L.druck_z,
           L.druck_rot, L.druck_cam, L.halten_bei_40, L.ende_bild, L.frei_bild);
    PRUEF(L.gesehen_bild == 0, "(9,70) = 1 ab dem ersten Bild (Set als erstes Opcode)");
    PRUEF(L.yaw_nach_dreh > 2048 + 400 && L.yaw_nach_dreh < 3072,
          "Elza dreht nach RECHTS: Gierung %d zwischen Eintritt 2048 (-X) und 3072 (+Z)", L.yaw_nach_dreh);
    static const int soll_msg[3] = { RE15_FUNK10F1_MSG_HALLO, RE15_FUNK10F1_MSG_PUNKTE, RE15_FUNK10F1_MSG_KAPUTT };
    PRUEF(L.n_msg == 3 && !memcmp(L.msg_folge, soll_msg, sizeof soll_msg), "angezeigt genau msg 40 -> 41 -> 42");
    PRUEF(s_n_stimme == 3 && !memcmp(s_stimme, soll_msg, sizeof soll_msg),
          "Stimme je Zeile angefordert (SCD_AUDIO_VOICE_ON 40, 41, 42 -> synchro/STAGE1/room10F1/main40..42.wav)");
    static const int soll_mo[4] = { 11, 0, 9, 12 };
    PRUEF(folge_enthaelt(L.mo_folge, L.n_mo, soll_mo, 4), "Gesten in der Folge 11 (Druck) -> 0 (Senken) -> 9 (Huefte) -> 12 (Stirn)");
    PRUEF(L.halten_bei_40 >= 60 && L.rueck_bei_0 >= 10,
          "Druck GEHALTEN, waehrend msg 40 steht (%d Bilder Motion 11 + Plc_flg 4), danach Clip 0 rueckwaerts (%d Bilder)",
          L.halten_bei_40, L.rueck_bei_0);
    PRUEF(L.druck_x > 6400 && L.druck_x < 7000 && L.druck_z > 5300 + 468 && L.druck_z < 7500 - 468 &&
          (L.druck_rot < 128 || L.druck_rot > 4096 - 128) && L.druck_cam == 1,
          "Druck an der Konsole zwischen den Tischen 13/12 (x %d, z %d), Blick nach Osten (%d), Kamera Cut 1",
          L.druck_x, L.druck_z, L.druck_rot);
    PRUEF(L.balken_waehrend > 300 && L.ende_bild > 0 && !L.balken_ende && L.frei_bild >= L.ende_bild,
          "Balken + Pad-Sperre waehrend der Szene (%d Bilder), danach weg; Spieler frei ab Bild %d", L.balken_waehrend,
          L.frei_bild);
}

static void teil_einmal(void)
{
    re15_game_state_init();
    if (room_boot(RE15_FUNK10F1_RAUM, 8400, -350, 2048) != 0) return;
    lauf_t L; lauf(&L, 3000);
    PRUEF(L.ende_bild > 0, "erste Szene durchgelaufen (Bild %d)", L.ende_bild);
    /* zweites Betreten: dieselben Flags, neuer Raumaufbau */
    if (room_boot(RE15_FUNK10F1_RAUM, 8400, -350, 2048) != 0) return;
    lauf(&L, 200);
    PRUEF(!re15_funk10f1_r39_programm_laeuft() && L.n_msg == 0 && s_n_stimme == 0 && g_scd.player_mode == 0,
          "zweites Betreten: keine Szene, keine Nachricht, keine Stimme, Spieler frei");
    PRUEF(re15_funk10f1_r39_rbj_quelle(RE15_FUNK10F1_RAUM) == 0 &&
          re15_funk10f1_r39_ereignis(RE15_FUNK10F1_RAUM, RE15_FUNK10F1_EREIGNIS) == NULL,
          "keine Leihe mehr, Ereignis 27 liefert kein Programm");
    /* Leon: ROOM10F0 — dieselbe RDT, die Funkszene gehoert NUR zu Elzas 10F1 */
    re15_game_state_init();
    re15_game_flag_set(9, 71, 1);   /* Leons eigene Szene (Runde 35 K) gesehen, damit der Raum frei ist */
    if (room_boot(0x10F0, 8400, -350, 2048) != 0) return;
    lauf(&L, 200);
    PRUEF(!re15_funk10f1_r39_programm_laeuft() && L.n_msg == 0 &&
          !re15_game_flag_get(RE15_FUNK10F1_GESEHEN_BANK, RE15_FUNK10F1_GESEHEN_BIT),
          "Leon ROOM10F0: keine Funkszene, (9,70) bleibt 0");
    PRUEF(re15_funk10f1_r39_rbj_quelle(0x10F0) == 0 && re15_funk10f1_r39_ereignis(0x10F0, RE15_FUNK10F1_EREIGNIS) == NULL,
          "Leon ROOM10F0: keine Leihe, kein Programm");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "form";
    printf("test_r39_funk10f1 [%s]\n", teil);
    if (!strcmp(teil, "form")) teil_form();
    else if (!strcmp(teil, "texte")) teil_texte();
    else if (!strcmp(teil, "szene")) teil_szene();
    else if (!strcmp(teil, "einmal")) teil_einmal();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
