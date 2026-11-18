/*
 * test_r42_story6.c — Runde 42 "story" (v0.8.31), Stufe STAGE6: Zug 6030 (Leon, L21) / 6031 (Elza, E6, E9) und T-05 (Masken-Tiefe
 * ueber Marvins Liege, Cut 9). Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen Installern, SCD-VM, re15_game_step),
 * Geruest = test_r41_story80.c. Belege include/re15_story6_r42.h, Programme tools/r42_story/szenen_stage6.py, Texte
 * tools/r42_story/texte_stage6.py, Tiefen tools/maske/liege_tiefe_r42.py. PORT-STORY nach dem Konzept (3.2 L21, 4.2 E6/E9, 5 T-05).
 *
 * Teile (Aufruf: test_r42_story6 <teil>):
 *   texte      6030 msg 21/2/3/22/4/5 und 6031 msg 1/2/12..17 nach dem Raumaufbau: Wortlaut, Sprecherkopf/Farbe, Dauer, <= 3 Zeilen je
 *              Seite, <= 271 px; 6030 msg 4 zwei Seiten (Form ROOM3060 msg 7 @0x1244); 6031 msg 0 bleibt die story_r36-Fassung
 *   wagen6030  Leon: Wagen-Programm k_s80_6030_sub02, Folge 0,1,21,2,3,22,4,5,6,7,23,24,8 mit den neuen Saetzen; Stimmen-Ids = Ids
 *   wagen6031  Elza: sub02 -> k_s6_6031_wagen, Folge 0,1,2,12,13, Stimmen-Ids = Ids, danach Cut 9, Sperren frei, (3,22)=1
 *   vorher6031 Elza Fuehrerstand vor dem Tor ((3,89)=0): Original (sub08 bzw. nichts), kein Zugstart; Wagen: Original (msg 4)
 *   tuer6031   Elza Wagen nach dem Tor, vor dem Zugstart: Ausgang = Tuer (Slot 2 Typ Tuer), Tuer nach 5091 = msg 4; nach dem
 *              Zugstart Original (Slot 2 = msg 11)
 *   zug6031    Elza Fuehrerstand nach dem Tor: Zugstart E9 einmal, Folge 14..17, (9,130)=1, Zug-an-Ton vor dem Krachen, Sherry am
 *              Pult, John im Fuehrerstand, danach Sperren frei; erneut betreten: kein zweiter Zugstart
 *   t05        MASKS/ROOM6030.MSK == ROOM6031.MSK; Cut 9: Soll-Tiefen (51 Rechtecke ueber der Liegeflaeche), alle anderen
 *              unveraendert; keiner der 147 Kernpunkte Marvins wird verdeckt (Konzept [B36]: vorher 7 Rechtecke)
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_engine.h"
#include "re15_elza_r36.h"
#include "re15_kleinkram_r38.h"
#include "re15_pri.h"
#include "re15_story_r36.h"
#include "re15_story80_r41.h"
#include "re15_story_r42.h"
#include "re15_story6_r42.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ================================================ Geruest (Muster test_r38_kleinkram.c) ======================== */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static int                s_bild = 0;

/* Protokoll je Bild: neu geoeffnete Texte (Id + Wortlaut) und eingereihte Stimmen. */
#define PROT_N 64
static int  s_prot_id[PROT_N];
static int  s_prot_bild[PROT_N];
static int  s_prot_cut[PROT_N];   /* Runde 42 rest Stufe 2: angezeigter Cut im Oeffnungsbild (D4) */
static char s_prot_text[PROT_N][192];
static int  s_prot_n = 0;
static int  s_stimme[PROT_N];
static int  s_stimme_n = 0;
static int  s_offen_vorher = 0;
static int  s_id_vorher = -1;

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

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[4][0x60000];
static void bank_laden(uint8_t type, int buf)
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
    if (!s_ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb || len > sizeof s_bank_buf[buf]) return;
    memcpy(s_bank_buf[buf], s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
}

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

static void protokoll(void)
{
    const int offen = g_scd.message_active ? 1 : 0;
    const int id = offen ? (int)g_scd.message_id : -1;
    if (offen && (!s_offen_vorher || id != s_id_vorher) && s_prot_n < PROT_N) {
        const char *t = re15_msg_get_text(id);
        s_prot_id[s_prot_n] = id;
        s_prot_bild[s_prot_n] = s_bild;
        s_prot_cut[s_prot_n] = s_shown;
        snprintf(s_prot_text[s_prot_n], sizeof s_prot_text[0], "%s", t ? t : "");
        s_prot_n++;
    }
    s_offen_vorher = offen; s_id_vorher = id;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r38_kleinkram.c frame_pad). */
static void frame_pad(uint16_t pressed, uint16_t held)
{
    const unsigned char *raw; int len, id;
    if (!re15_item_modal_active()) scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
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
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = pressed;
    re15_game_step(&s_ctx);
    if (re15_item_modal_active())
        re15_item_modal_tick(re15_pad_virtual_word(pressed), re15_pad_virtual_word(held));
    protokoll();
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e))
        if (e.kind == SCD_AUDIO_VOICE_ON && s_stimme_n < PROT_N) s_stimme[s_stimme_n++] = (int)e.sample_id;
    s_bild++;
}
static void frame(void) { frame_pad(0, 0); }

static void prot_leeren(void) { s_prot_n = 0; s_stimme_n = 0; s_offen_vorher = 0; s_id_vorher = -1; }

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, const uint8_t *banken, int nb)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_inventory_t inv_vorher = g_inv;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    g_inv = inv_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    for (int i = 0; i < nb && i < 4; i++) bank_laden(banken[i], i);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    re15_elza_r36_reset();
    re15_kleinkram_r38_reset();
    s_bild = 0;
    prot_leeren();
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int faden_im_programm(const uint8_t *p, int n)
{
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        const scd_thread_t *t = &g_scd.threads[i];
        if (t->active && t->pc >= p && t->pc < p + n) return i;
    }
    return -1;
}

/* ================================================ Soll-Wortlaut ================================================= */
typedef struct { uint16_t raum; int id; const char *text; } zeile_t;
static const zeile_t k_liste[] = {
    /* L21 (6030, Leon): Folge im Wagen 21, 2, 3, 22, 4, 5 */
    { 0x6030, 21, "John: Name's John. This is Sherry. We came down here with a girl named Elza." },
    { 0x6030,  2, "Leon: Where is she now?" },
    { 0x6030,  3, "John: We got split up in the fire. One second she was right behind us..." },
    { 0x6030, 22, "Sherry: Elza's still out there...!" },
    /* zwei Seiten: re15_msg_decode_text haengt die Seiten ohne Leerzeichen an (wie ROOM3060 msg 7 "side!Just") */
    { 0x6030,  4, "Leon: If she's as tough as you say, she'll find another way out.But we can't wait. When that reactor goes, nobody gets out." },
    { 0x6030,  5, "John: Then I'm staying with the kid. And I'll keep an eye on your friend here." },
    /* E6 (6031, Elza): msg 0 bleibt (story_r36), 1, 2 Neutext, 12, 13 neu */
    { 0x6031,  0, "Elza: Sherry! Is anyone else here?" },
    { 0x6031,  1, "Sherry: A policeman brought Marvin down here. Then he went back into the fire." },
    { 0x6031,  2, "Elza: Back? Why?" },
    { 0x6031, 12, "Sherry: He said he had to find someone. He told us not to wait for him." },
    { 0x6031, 13, "Elza: ...Then we won't. If that core goes, no one gets out. Let's get this train moving." },
    /* E9 (6031, Elza): Zugstart */
    { 0x6031, 14, "Sherry: It's moving! Elza, we did it!" },
    { 0x6031, 15, "John: What the hell was that? It came from the back car!" },
    { 0x6031, 16, "Sherry: It's that monster... it followed us!" },
    { 0x6031, 17, "Elza: John, stay with Sherry and Marvin. Don't open that door for anyone but me." },
};
#define LISTE_N ((int)(sizeof k_liste / sizeof k_liste[0]))
static const char *soll_text(uint16_t raum, int id)
{
    for (int i = 0; i < LISTE_N; i++) if (k_liste[i].raum == raum && k_liste[i].id == id) return k_liste[i].text;
    return NULL;
}

#include "font_width.h"
/* Zeilenbreite/Zeilen je Seite wie tools/r42_story/texte_stage6.py (`02 N` = neue Seite, Form ROOM3060 @0x1244) */
static int max_zeilenbreite(const unsigned char *b, int n, int *zeilen, int *seiten)
{
    int w = 0, mx = 0, z = 1, zmax = 1, s = 1;
    for (int i = 0; i < n; i++) {
        const unsigned char c = b[i];
        if (c == 0x01) break;
        if (c == 0x02) { if (w > mx) mx = w; w = 0; if (z > zmax) zmax = z; z = 1; s++; i++; continue; }
        if (c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) { i++; continue; }
        if (c == 0x03 || c == 0x07) continue;
        if (c == 0x08) { if (w > mx) mx = w; w = 0; z++; continue; }
        w += re15_font_width[c];
    }
    if (w > mx) mx = w;
    if (z > zmax) zmax = z;
    if (zeilen) *zeilen = zmax;
    if (seiten) *seiten = s;
    return mx;
}

static void teil_texte(void)
{
    printf("== texte ==\n");
    static const uint16_t k_raeume[] = { 0x6030, 0x6031 };
    static const unsigned char k_schwanz[4] = { 0x04, 0x01, 0x01, 0x63 };   /* ROOM6030 @0x13F5, ROOM6031 @0x1161 */
    for (unsigned r = 0; r < 2; r++) {
        if (rdt_laden(k_raeume[r]) != 0) { PRUEF(0, "ROOM%04X ladbar", k_raeume[r]); continue; }
        g_current_room_id = k_raeume[r];
        re15_msg_clear_room_block();
        re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
        for (int i = 0; i < LISTE_N; i++) {
            if (k_liste[i].raum != k_raeume[r]) continue;
            const char *t = re15_msg_get_text(k_liste[i].id);
            int len = 0, z = 0, sn = 0, tn = 0;
            const unsigned char *raw = re15_msg_get_raw(k_liste[i].id, &len);
            const int px = raw ? max_zeilenbreite(raw, len, &z, &sn) : -1;
            const uint8_t *tab = re15_story6_r42_text(k_liste[i].raum, (uint8_t)k_liste[i].id, &tn);
            const int dauer = raw ? re15_msg_compute_duration(raw, (size_t)len, 0) : 0;   /* abgelegt mit re15_msg_install_durations (story6_r42.c) */
            /* 6031 msg 0 = story_r36 (nicht Teil der Stufe); sonst Bytes = Tabelle der Stufe */
            const int quelle_ok = tab ? (raw && tn == len && memcmp(raw, tab, (size_t)len) == 0)
                                      : (k_liste[i].raum == 0x6031 && k_liste[i].id == 0);
            PRUEF(t && strcmp(t, k_liste[i].text) == 0 && quelle_ok && len <= 192 && px > 0 && px <= 271 && z <= 3 && dauer > 0 &&
                  len >= 4 && memcmp(raw + len - 4, k_schwanz, 4) == 0,
                  "ROOM%04X msg %2d (%d B, %d Seite(n), %d Zeilen je Seite, max %d px, Dauer %d, Schwanz 04 01 01 63): \"%s\"",
                  k_liste[i].raum, k_liste[i].id, len, sn, z, px, dauer, t ? t : "(fehlt)");
            if (k_liste[i].raum == 0x6030 && k_liste[i].id == 4)
                PRUEF(sn == 2, "ROOM6030 msg 4 hat zwei Seiten (Seitenwechsel `04 01 02 63 04 00`, Form ROOM3060 @0x1244)");
            /* Sprecher: Farbe im Kopf `04 00 05 <Farbe>` (Leon/Elza 01, Sherry 06, John 07) */
            const int soll = !strncmp(k_liste[i].text, "Sherry", 6) ? 6 : !strncmp(k_liste[i].text, "John", 4) ? 7 : 1;
            PRUEF(raw && len > 4 && raw[2] == 0x05 && raw[3] == soll, "ROOM%04X msg %2d Farbe %d (soll %d)", k_liste[i].raum,
                  k_liste[i].id, raw && len > 4 ? raw[3] : -1, soll);
        }
    }
}

/* ================================================ Szenen ======================================================= */
/* Laeuft die Szene bis zum Ende des Programms (Faden verlaesst es) und vergleicht die Text- und Stimmen-Folge. */
/* U-03 (Runde 42 rest Stufe 3): Bilder, in denen Elza vor msg 0 zu Marvin (Mod-6031 (-21241,-26118)) gedreht steht. Gier-Konvention
 * dir = (cos, -sin), 0 = +X, 1024 = -Z (dachkiste_r36.c wp_yaw_zu). */
static int s_u03_marvin_bilder = 0;
static int s_u03_nacken_bilder = 0;
static void u03_messen(uint16_t raum)
{
    if (raum != 0x6031 || s_prot_n > 0) return;
    const re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const double dx = -21241.0 - p->x, dz = -26118.0 - p->z;
    int soll = (int)lround(atan2(-dz, dx) / (2.0 * 3.14159265358979) * 4096.0) & 4095;
    int d = ((int)(p->rot_y & 4095) - soll) & 4095; if (d > 2048) d = 4096 - d;
    if (d <= 64 && re15_game_flag_get(2, 7)) s_u03_marvin_bilder++;
    /* G5 (Pruefung s3_r1): Nackenziel in allen drei Achsen = Mod-Marvin @0x00E42 (-21241,-920,-26118), wie Disc @0x0072E =
     * Disc-Marvin @0x005EA. Ein Nackenziel mit y 0 (Boden unter der Liege) zaehlt nicht. */
    if (p->neck_tx == -21241 && p->neck_ty == -920 && p->neck_tz == -26118 && (p->neck_flags & 0x04)) s_u03_nacken_bilder++;
}

static void szene_pruefen(const uint8_t *prog, int pn, const uint8_t *folge, int fn, uint16_t raum, int max_bilder)
{
    int start = -1, ende = -1;
    s_u03_marvin_bilder = 0;
    s_u03_nacken_bilder = 0;
    for (int f = 0; f < max_bilder; f++) {
        frame();
        u03_messen(raum);
        if (start < 0) { if (faden_im_programm(prog, pn) >= 0) start = s_bild; }
        else if (faden_im_programm(prog, pn) < 0) { ende = s_bild; break; }
    }
    PRUEF(start >= 0 && ende > start, "ROOM%04X Programm lief von Bild %d bis %d (%d Bilder)", raum, start, ende, ende - start);
    printf("   Texte (%d):", s_prot_n);
    for (int i = 0; i < s_prot_n; i++) printf(" %d", s_prot_id[i]);
    printf("\n   Stimmen (%d):", s_stimme_n);
    for (int i = 0; i < s_stimme_n; i++) printf(" %d", s_stimme[i]);
    printf("\n");
    int folge_ok = (s_prot_n == fn), stimme_ok = (s_stimme_n == fn), text_ok = 1;
    for (int i = 0; i < fn && i < s_prot_n; i++) if (s_prot_id[i] != folge[i]) folge_ok = 0;
    for (int i = 0; i < fn && i < s_stimme_n; i++) if (s_stimme[i] != folge[i]) stimme_ok = 0;
    for (int i = 0; i < s_prot_n; i++) {
        const char *soll = soll_text(raum, s_prot_id[i]);
        printf("   %2d. Bild %4d msg %2d \"%s\"%s\n", i + 1, s_prot_bild[i], s_prot_id[i], s_prot_text[i],
               (soll && strcmp(soll, s_prot_text[i]) != 0) ? "  <- WEICHT AB" : "");
        if (soll && strcmp(soll, s_prot_text[i]) != 0) text_ok = 0;
    }
    PRUEF(folge_ok, "ROOM%04X: genau %d Zeilen in der Soll-Folge (gemessen %d)", raum, fn, s_prot_n);
    PRUEF(stimme_ok, "ROOM%04X: Stimmen-Ids = Message-Ids in derselben Folge (%d)", raum, s_stimme_n);
    PRUEF(text_ok, "ROOM%04X: jeder angezeigte Wortlaut = Soll-Liste", raum);
}

static const re15_actor_t *aktor_typ(uint8_t typ)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) return &g_actors[i];
    return NULL;
}

static void zaehler_null(void) { memset(g_re15_s6_zaehler, 0, sizeof g_re15_s6_zaehler); }

static void teil_wagen6030(void)
{
    printf("== wagen6030 (Leon, L21) ==\n");
    re15_game_state_init();
    re15_inv_init();
    zaehler_null();
    static const uint8_t k_banken[] = { 0x49, 0x40, 0x4B, 0x4D };   /* ROOM6030 sub06 @0x0101C/@0x01030/@0x01044, sub07 @0x010D4 */
    /* Eintritt Cut 6 (sub00 Case 6 -> Gosub sub06), Leon in der Marke Slot 5 (-19730,-25570,1600,6300) wie test_r41_story80 */
    if (room_boot(0x6030, -18930, -22420, 0, 6, k_banken, 4) != 0) return;
    int pn = 0, fn = 0;
    const uint8_t *prog = re15_story80_programm(RE15_S80_P_WAGEN, &pn);
    const uint8_t *folge = re15_story80_folge(RE15_S80_P_WAGEN, &fn);
    static const uint8_t k_soll[] = { 0, 1, 21, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8 };
    PRUEF(fn == (int)sizeof k_soll && !memcmp(folge, k_soll, sizeof k_soll), "Soll-Folge des Wagen-Programms unveraendert (13 Zeilen)");
    szene_pruefen(prog, pn, folge, fn, 0x6030, 12000);
    int alt = 0;
    for (int i = 0; i < s_prot_n; i++)
        if (strstr(s_prot_text[i], "fighting the monsters") || strstr(s_prot_text[i], "catch up")) alt = 1;
    PRUEF(!alt, "kein 'fighting the monsters' / 'catch up' mehr (L21)");
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_WAGEN] == 0 && g_re15_s6_zaehler[RE15_S6_P_ZUG] == 0, "6031-Programme laufen in 6030 nicht");
}

static void teil_wagen6031(void)
{
    printf("== wagen6031 (Elza, E6) ==\n");
    re15_game_state_init();
    re15_inv_init();
    zaehler_null();
    static const uint8_t k_banken[] = { 0x40, 0x49, 0x4B };   /* ROOM6031 sub06 @0x00E42/@0x00E56/@0x00E6A */
    /* Eintritt Cut 6, Elza in der Marke Slot 5 (main00 @0x00CC6 Rechteck (-25130,-25270,1660,5200)) */
    if (room_boot(0x6031, -24300, -21200, 2048, 6, k_banken, 3) != 0) return;
    int pn = 0, fn = 0;
    const uint8_t *prog = re15_story6_r42_programm(RE15_S6_P_WAGEN, &pn);
    const uint8_t *folge = re15_story6_r42_folge(RE15_S6_P_WAGEN, &fn);
    szene_pruefen(prog, pn, folge, fn, 0x6031, 9000);
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_WAGEN] == 1, "sub02 genau einmal umgeleitet (%u)", g_re15_s6_zaehler[RE15_S6_P_WAGEN]);
    for (int f = 0; f < 60; f++) frame();
    PRUEF(!re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27) && re15_game_flag_get(3, 22),
          "nach der Szene: Sperren (2,7)=%d (1,27)=%d frei (sub02 @0x00E00/@0x00E04), (3,22)=%d (@0x00D78)",
          re15_game_flag_get(2, 7), re15_game_flag_get(1, 27), re15_game_flag_get(3, 22));
    /* Programmkopf = Original-Bytes sub02 [@0x00D78, @0x00DB2), dann Runde 42 rest Stufe 2 (D4) Cut_chg 11 in Leons Form
     * (ROOM6030 sub02 @0x00DC0 `29 0b` vor msg 0), dann [@0x00DB2, @0x00DFA) */
    /* Runde 42 rest Stufe 3, U-03: nach dem Cut 11 Capcoms Blick auf Marvin (Disc Version_40% ROOM6041 sub06 @0x00724..@0x00746,
     * Ziele um Marvins Mod-Versatz (0,+400) verschoben), Nacken zurueck (Disc ROOM6040 @0x006F8), dieselbe Drehung @0x00D9E..@0x00DB2,
     * dann wie bisher [@0x00DB2, @0x00DFA). Block = 8 + 12 + 10 + 4 + 10 + 4 + 10 = 58 Byte (0x3A). */
    static const uint8_t k_u03[0x3A] = {
        0x40, 0x00, 0x09, 0x20, 0xbd, 0xad, 0xde, 0x99,                         /* drehen (-21059,-26146) = Disc @0x00724 + (0,400) */
        0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, 0x20, 0x00, /* warten (5,0x20)                                   */
        0x41, 0x01, 0x07, 0xad, 0x68, 0xfc, 0xfa, 0x99, 0x28, 0x28,             /* Nacken Marvin (-21241,-920,-26118) = Mod @0x00E42 */
        0x09, 0x0a, 0x28, 0x00,                                                 /* Sleep 40  = Disc @0x00738                          */
        0x41, 0x01, 0x99, 0xa7, 0x00, 0x00, 0x4f, 0x9f, 0x28, 0x28,             /* Nacken Wagenmitte = Disc @0x0073C                  */
        0x09, 0x0a, 0x78, 0x00,                                                 /* Sleep 120 = Disc @0x00746                          */
        0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x28 };           /* Nacken zurueck = Disc ROOM6040 @0x006F8            */
    PRUEF(pn > 0x84 + 0x3A + 0x14 && s_rawsz > 0x0DFA && !memcmp(prog, s_raw + 0x0D78, 0x3A) && prog[0x3A] == 0x29 &&
          prog[0x3B] == 0x0B && !memcmp(prog + 0x3C, k_u03, sizeof k_u03) && !memcmp(prog + 0x76, s_raw + 0x0D9E, 0x14) &&
          !memcmp(prog + 0x8A, s_raw + 0x0DB2, 0x48),
          "Programmkopf = ROOM6031 @0x00D78..@0x00DB2 + `29 0b` + U-03 (Disc ROOM6041 sub06) + @0x00D9E..@0x00DFA");
    PRUEF(s_u03_marvin_bilder >= 30, "U-03: Elza blickt vor msg 0 zu Marvin (Gier +-64 zur Richtung Marvin in %d Bildern, Soll >= 30)",
          s_u03_marvin_bilder);
    PRUEF(s_u03_nacken_bilder >= 40, "U-03/G5: Nackenziel = Mod-Marvin (-21241,-920,-26118) in %d Bildern (Soll >= 40 = Sleep 40 "
          "Disc @0x00738)", s_u03_nacken_bilder);
    /* D4: Elzas Gespraech beginnt mit dem Gruppenbild Cut 11 (wie Leon), Schluss wie Original Cut 9 (@0x00DFA) */
    PRUEF(s_prot_n >= 1 && s_prot_id[0] == 0 && s_prot_cut[0] == 11, "D4: msg 0 mit Cut 11 (gemessen Cut %d)",
          s_prot_n >= 1 ? s_prot_cut[0] : -1);
    PRUEF(s_shown == 9 || g_scd.cam_id == 9, "D4: nach der Szene Cut 9 wie Original @0x00DFA (angezeigt %d, cam %d)", s_shown,
          g_scd.cam_id);
}

/* Fuehrerstand betreten: Eintritt Cut 0 (sub00 @0x00D2A Case 0 -> Gosub sub07), Elza am Ziel der Tuer Slot 1 (main00 @0x00C52) */
static int fuehrerstand(void)
{
    static const uint8_t k_banken[] = { 0x4B, 0x49, 0x40 };
    return room_boot(0x6031, -3873, -23487, 2048, 0, k_banken, 3);
}
static int wagen(void)
{
    static const uint8_t k_banken[] = { 0x40, 0x49, 0x4B };
    return room_boot(0x6031, -17970, -20380, 1100, 6, k_banken, 3);   /* Tuerziel ROOM6001 main00 @0x00DDA (-17970,-20380) Cut 6 */
}

static void teil_vorher6031(void)
{
    printf("== vorher6031: vor dem Tor ((3,89)=0) ==\n");
    re15_game_state_init();
    re15_inv_init();
    zaehler_null();
    re15_game_flag_set(3, 22, 1); re15_game_flag_set(4, 23, 1);   /* Wagen und gescheiterter Start gesehen */
    if (fuehrerstand() != 0) return;
    for (int f = 0; f < 900; f++) frame();
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_ZUG] == 0 && s_prot_n == 0 && !re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA),
          "Fuehrerstand vor dem Tor: kein Zugstart, kein Text (%d), (9,130)=0", s_prot_n);
    PRUEF(aktor_typ(0x4B) != NULL, "Sherry steht am Pult (sub07 @0x00F1E, Zweig (3,89)=0)");
    if (wagen() != 0) return;
    for (int f = 0; f < 120; f++) frame();
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_TUER] == 0 && g_aot.slots[3].active && g_aot.slots[3].event_id == 4,
          "Wagen vor dem Tor: Original (sub06 @0x00E90 Slot 3 -> msg %d, Programm %u)", (int)g_aot.slots[3].event_id,
          g_re15_s6_zaehler[RE15_S6_P_TUER]);
}

static void teil_tuer6031(void)
{
    printf("== tuer6031: nach dem Tor, vor dem Zugstart ==\n");
    re15_game_state_init();
    re15_inv_init();
    zaehler_null();
    re15_game_flag_set(3, 22, 1); re15_game_flag_set(4, 23, 1); re15_game_flag_set(3, 89, 1);
    if (wagen() != 0) return;
    for (int f = 0; f < 120; f++) frame();
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_TUER] == 1 && g_re15_s6_zaehler[RE15_S6_P_ZUG] == 0,
          "sub06 @0x00EEA -> Wagen-Programm (%u), kein Zugstart im Wagen (%u)", g_re15_s6_zaehler[RE15_S6_P_TUER],
          g_re15_s6_zaehler[RE15_S6_P_ZUG]);
    PRUEF(g_aot.slots[3].active && g_aot.slots[3].event_id == 4, "Tuer nach 5091 (Slot 3) -> msg 4 'I have to get this thing moving "
          "first!' (sub06 @0x00E90), ist %d", (int)g_aot.slots[3].event_id);
    PRUEF(g_aot.slots[2].active && g_aot.slots[2].type == RE15_AOT_TYPE_DOOR,
          "Ausgang Slot 2 ist wieder die Tuer (main00 @0x00C72), Typ %d", (int)g_aot.slots[2].type);
    printf("== tuer6031: nach dem Zugstart ==\n");
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA, 1);
    zaehler_null();
    if (wagen() != 0) return;
    for (int f = 0; f < 120; f++) frame();
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_TUER] == 0 && g_aot.slots[2].active && g_aot.slots[2].event_id == 11,
          "nach dem Zugstart Original: Slot 2 -> msg %d (soll 11 'You can't get off', sub06 @0x00EBE), Programm %u",
          (int)g_aot.slots[2].event_id, g_re15_s6_zaehler[RE15_S6_P_TUER]);
}

static void teil_zug6031(void)
{
    printf("== zug6031: Zugstart E9 ==\n");
    re15_game_state_init();
    re15_inv_init();
    zaehler_null();
    re15_game_flag_set(3, 22, 1); re15_game_flag_set(4, 23, 1); re15_game_flag_set(3, 89, 1);
    if (fuehrerstand() != 0) return;
    int pn = 0, fn = 0;
    const uint8_t *prog = re15_story6_r42_programm(RE15_S6_P_ZUG, &pn);
    const uint8_t *folge = re15_story6_r42_folge(RE15_S6_P_ZUG, &fn);
    /* Toene: Zug-an (Zaehler [3]) vor msg 14 und vor dem Krachen ([4]), das Krachen vor msg 15 */
    int bild_an = -1, bild_krach = -1, start = -1, ende = -1, sherry = 0, john = 0;
    int32_t jx = 0, jz = 0;
    for (int f = 0; f < 9000; f++) {
        frame();
        if (bild_an < 0 && g_re15_s6_zaehler[RE15_S6_P_N]) bild_an = s_bild;
        if (bild_krach < 0 && g_re15_s6_zaehler[RE15_S6_P_N + 1]) bild_krach = s_bild;
        if (aktor_typ(0x4B)) sherry = 1;
        if (aktor_typ(0x49) && !john) { john = 1; jx = aktor_typ(0x49)->x; jz = aktor_typ(0x49)->z; }
        if (start < 0) { if (faden_im_programm(prog, pn) >= 0) start = s_bild; }
        else if (faden_im_programm(prog, pn) < 0) { ende = s_bild; break; }
    }
    PRUEF(start >= 0 && ende > start, "Zugstart-Programm lief von Bild %d bis %d (%d Bilder)", start, ende, ende - start);
    int folge_ok = (s_prot_n == fn), stimme_ok = (s_stimme_n == fn), text_ok = 1;
    for (int i = 0; i < fn && i < s_prot_n; i++) if (s_prot_id[i] != folge[i]) folge_ok = 0;
    for (int i = 0; i < fn && i < s_stimme_n; i++) if (s_stimme[i] != folge[i]) stimme_ok = 0;
    for (int i = 0; i < s_prot_n; i++) {
        const char *soll = soll_text(0x6031, s_prot_id[i]);
        printf("   %2d. Bild %4d msg %2d \"%s\"\n", i + 1, s_prot_bild[i], s_prot_id[i], s_prot_text[i]);
        if (!soll || strcmp(soll, s_prot_text[i]) != 0) text_ok = 0;
    }
    PRUEF(folge_ok && text_ok, "Zeilen 14, 15, 16, 17 in dieser Folge mit Soll-Wortlaut (gemessen %d)", s_prot_n);
    PRUEF(stimme_ok, "Stimmen-Ids = Message-Ids (%d)", s_stimme_n);
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_ZUG] == 1, "sub07 @0x00F54 -> Zugstart genau einmal (%u)", g_re15_s6_zaehler[RE15_S6_P_ZUG]);
    PRUEF(re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA), "(9,130)=1 nach dem Zugstart");
    PRUEF(bild_an > 0 && s_prot_n >= 1 && bild_an < s_prot_bild[0], "Zug-an-Ton (Bild %d) vor msg 14 'It's moving!' (Bild %d)",
          bild_an, s_prot_n ? s_prot_bild[0] : -1);
    PRUEF(bild_krach > bild_an && s_prot_n >= 2 && bild_krach > s_prot_bild[0] && bild_krach < s_prot_bild[1],
          "Krachen (Bild %d) nach msg 14 und vor msg 15 (Bild %d)", bild_krach, s_prot_n >= 2 ? s_prot_bild[1] : -1);
    PRUEF(sherry && john && jx == -2900 && jz == -23800, "Sherry (0x4B) am Pult und John (0x49) im Fuehrerstand bei (%d,%d)", jx, jz);
    for (int f = 0; f < 60; f++) frame();
    PRUEF(!re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27), "Sperren (2,7)/(1,27) frei (sub08 @0x0111A/@0x0111E)");
    printf("== zug6031: erneut betreten ==\n");
    zaehler_null();
    if (fuehrerstand() != 0) return;
    for (int f = 0; f < 600; f++) frame();
    PRUEF(g_re15_s6_zaehler[RE15_S6_P_ZUG] == 0 && s_prot_n == 0, "kein zweiter Zugstart, kein Text (%d)", s_prot_n);
}

/* ================================================ Ruckeln (Runde 42 rest b) ===================================== */
/* E9-Fahrt: ab dem ersten Takt mit aktivem sub05-Faden (Licht-Schleife @0x00E2A) zeigt der Port das RE2-Ruckeln ROOM7030 sub14
 * (story6_r42.c k_ruck): sichtbar nur die Sce_shake_on mit pc[1] = 1 (FUN_8002ac94 `bgtz a1` @0x8002acb8), je Durchlauf 64 Bilder,
 * ab dem dritten jeder zweite ein Bild laenger (Evt_next @0x0BDC). Vor der Fahrt kein Ruckeln. */
static void teil_ruck6031(void)
{
    printf("== ruck6031: Ruckeln der Fahrt nach dem Zugstart E9 ==\n");
    re15_game_state_init();
    re15_inv_init();
    zaehler_null();
    re15_game_flag_set(3, 22, 1); re15_game_flag_set(4, 23, 1); re15_game_flag_set(3, 89, 1);
    if (fuehrerstand() != 0) return;
    const uint8_t *licht = s_rdt.sub_scd[RE15_S6_SUB_LICHT];
    static const struct { int bild, dx, dy; } k_soll[] = {   /* sichtbare Bilder eines Durchlaufs (RE2 ROOM7030 sub14) */
        { 0, 1, 1 }, { 2, 1, 0 }, { 7, 1, 0 }, { 9, 1, 1 }, { 21, 1, 1 }, { 32, 1, 1 }, { 34, 1, 0 }, { 39, 1, 0 }, { 41, 1, 1 }, { 52, 1, 1 },
    };
    enum { N = 387 };   /* k = 1..386 = Bild 0..385 = Durchlaeufe 0..5 (Start 322, Ende 385) */
    static int s_dx[N], s_dy[N];
    int licht_bild = -1, vorher = 0, n = 0;
    for (int f = 0; f < 9000 && n < N; f++) {
        frame();
        int dx = 0, dy = 0;
        const int an = re15_story6_r42_ruck(&dx, &dy);
        if (licht_bild < 0) {
            if (an) vorher++;
            if (licht && faden_im_programm(licht, 24) >= 0) licht_bild = s_bild;
            else continue;
        }
        s_dx[n] = dx; s_dy[n] = dy; n++;
    }
    PRUEF(licht_bild > 0 && vorher == 0, "Licht-Schleife sub05 ab Bild %d, davor %d Bilder mit Ruckeln", licht_bild, vorher);
    /* Takt der Licht-Schleife = k = 0; das Ruckeln beginnt im naechsten Takt (story6-Takt vor den Faeden, PORT-WAHL im Kopf). */
    int fehler = 0, sichtbar = 0;
    for (int k = 1; k < n; k++) {
        const int b = k - 1;                           /* Bild seit Beginn des Ruckelns */
        int t, it;                                     /* Durchlauf it, Bild t darin: Starts 0, 64, 128, 193, 257, 322 */
        static const int k_start[] = { 0, 64, 128, 193, 257, 322, 386 };
        for (it = 0; it < 6 && b >= k_start[it + 1]; it++) {}
        t = b - k_start[it];
        int sdx = 0, sdy = 0;
        for (unsigned i = 0; i < sizeof k_soll / sizeof k_soll[0]; i++)
            if (k_soll[i].bild == t) { sdx = k_soll[i].dx; sdy = k_soll[i].dy; }
        if (s_dx[k] || s_dy[k]) sichtbar++;
        if (s_dx[k] != sdx || s_dy[k] != sdy) {
            if (fehler < 6) printf("        Bild %d (Durchlauf %d, Takt %d): (%d,%d), Soll (%d,%d)\n", b, it, t, s_dx[k], s_dy[k], sdx, sdy);
            fehler++;
        }
    }
    PRUEF(n == N && fehler == 0 && sichtbar == 60,
          "%d Bilder der Fahrt gemessen: %d sichtbare Ruckler (Soll 6 Durchlaeufe x 10 = 60), %d Abweichungen vom RE2-Takt", n, sichtbar,
          fehler);
}

/* ================================================ T-05 ========================================================= */
#include "gen_r42_liege_tiefe.inc"
static uint8_t *msk_laden(uint16_t raum, size_t *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, (unsigned)raum);
    return slurp(p, n);
}
static void teil_t05(void)
{
    printf("== t05 ==\n");
    size_t n30 = 0, n31 = 0;
    uint8_t *m30 = msk_laden(0x6030, &n30), *m31 = msk_laden(0x6031, &n31);
    PRUEF(m30 && m31 && n30 == n31 && !memcmp(m30, m31, n30), "MASKS/ROOM6030.MSK == ROOM6031.MSK (%u B)", (unsigned)n30);
    if (!m31) return;
    static re15_pri_cut_t cut;
    const uint32_t off = re15_pri_msk_section_offset(m31, n31, 9);
    const int nm = off ? re15_pri_parse_section(m31, n31, off, &cut) : 0;
    const int ns = (int)(sizeof k_t05_soll / sizeof k_t05_soll[0]);
    PRUEF(nm == ns && nm == 91, "Cut 9: %d Rechtecke (Soll-Tabelle %d)", nm, ns);
    int geaendert = 0, falsch = 0;
    for (int i = 0; i < ns && i < nm; i++) {
        if (k_t05_soll[i].tiefe != k_t05_soll[i].tiefe_alt) geaendert++;
        if ((int)cut.masks[i].depth != k_t05_soll[i].tiefe) {
            falsch++;
            printf("   Rechteck %d: Tiefe %d, soll %d\n", i, (int)cut.masks[i].depth, k_t05_soll[i].tiefe);
        }
    }
    PRUEF(falsch == 0 && geaendert == 51, "Tiefen = Soll (%d geaendert, %d abweichend)", geaendert, falsch);
    /* Marvins Kern (Konzept [B36]): kein Punkt liegt hinter einer Maske; mit den alten Tiefen schon */
    int verdeckt_neu = 0, verdeckt_alt = 0;
    const int nk = (int)(sizeof k_t05_kern / sizeof k_t05_kern[0]);
    for (int k = 0; k < nk; k++) {
        int nneu = 0, nalt = 0;
        for (int i = 0; i < nm && i < ns; i++) {
            const re15_pri_mask_t *m = &cut.masks[i];
            const int x = (int16_t)m->dstX, y = (int16_t)m->dstY;
            if (k_t05_kern[k].sx < x || k_t05_kern[k].sx >= x + m->width || k_t05_kern[k].sy < y || k_t05_kern[k].sy >= y + m->height)
                continue;
            if ((int)m->depth * 64 < k_t05_kern[k].vz) nneu++;
            if (k_t05_soll[i].tiefe_alt * 64 < k_t05_kern[k].vz) nalt++;
        }
        if (nneu) verdeckt_neu++;
        if (nalt) verdeckt_alt++;
    }
    PRUEF(verdeckt_neu == 0 && verdeckt_alt > 0, "Marvins Kern (%d Punkte): verdeckt vorher %d, jetzt %d", nk, verdeckt_alt, verdeckt_neu);
    free(m30); free(m31);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "texte")      || !strcmp(teil, "alle")) teil_texte();
    if (!strcmp(teil, "wagen6030")  || !strcmp(teil, "alle")) teil_wagen6030();
    if (!strcmp(teil, "wagen6031")  || !strcmp(teil, "alle")) teil_wagen6031();
    if (!strcmp(teil, "vorher6031") || !strcmp(teil, "alle")) teil_vorher6031();
    if (!strcmp(teil, "tuer6031")   || !strcmp(teil, "alle")) teil_tuer6031();
    if (!strcmp(teil, "zug6031")    || !strcmp(teil, "alle")) teil_zug6031();
    if (!strcmp(teil, "ruck6031")   || !strcmp(teil, "alle")) teil_ruck6031();
    if (!strcmp(teil, "t05")        || !strcmp(teil, "alle")) teil_t05();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
