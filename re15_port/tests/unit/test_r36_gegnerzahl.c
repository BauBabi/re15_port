/* test_r36_gegnerzahl.c — RIEGEL Runde 36 Spur E: zwanzig Gegner-Plaetze je Raum wie das Original.
 *
 * Nutzer (AUFTRAG.md Z.19-20): "Der Port verwaltet hoechstens 15 Gegner je Raum, das Original 20. In ROOM1040
 * und 1030 fehlen deshalb die letzten fuenf Zombies [...] --> Das auch!"
 * Dossier: analysis/befunde_runde36/E_gegnerzahl.md. Konstante: include/re15_actor.h RE15_ACTOR_MAX.
 *
 * BELEGE (selbst disassembliert, PSX.EXE):
 *   Gegner-Feld 0x800acc2c, Stride 0x1F4, ZWANZIG Plaetze: FUN_80037250 `sltiu v0,v0,0x14` @0x800372b4 (Umbruch
 *   @0x800372c4/@0x800372c8 auf 0x800acc2c); das naechste Feld beginnt bei 0x800acc2c + 20*0x1F4 = 0x800af33c
 *   (FUN_80039b2c `addiu v1,v1,0x11cc` @0x80039b38 + `addiu v1,v1,-0x1e90` @0x80039b40).
 *   Sce_em_set (0x800420a0): Slot = pc[1] & 0x7f @0x800420f8, Entity = 0x800acc2c + Slot*0x1F4 (@0x800420dc/
 *   @0x80042118) OHNE Grenzpruefung; Tot-Bit-Gate @0x80042120-38 VOR dem Gleichzeitig-Limit @0x80042214-3c
 *   (Zaehler 0x800b0ff2, Maximum 0x800b0ff4 = Save(0x12,n)).
 *   Port: Script-Slot s = Aktor s+1 (scd_vm.c SCRIPT_SLOT_TO_ACTOR), also Spieler + 20 = 21.
 *
 * MECHANIK, die gemessen wird (echte RDTs, Raumaufbau ueber scd_room_reenter wie im Spiel):
 *   raum1030/raum1031 (Limit 6, Save(0x12,6) @0x1de2/@0x1e58; Tot-Bit = Slot, Records @0x1de6../@0x1e5c..)
 *   raum1040/raum1041 (Limit 5, Save(0x12,5) @0x11f0/@0x1218; Tot-Bit = 0x14 + Slot; beide Zweige Ck(4,5))
 *     Je k = 0..20 (Records 0..k-1 tot): es erscheinen GENAU die Records k..k+n-1, n = min(Limit, 20-k), als
 *     Aktor k+1..k+n — das ist die Original-Regel "Tot-Bit vor Limit" mit 20 Plaetzen. Vorher (16) war ab k = 10
 *     (1030) bzw. 11 (1040) der Raum zu leer, ab k = 15 leer.
 *     ROOM1030 zusaetzlich: die raumeigene Aufstellung (sub00 For-20 mit Opcode 0x53 Art 2 ueber work_vars[4],
 *     Else-Zweig @0x020CE..@0x0216E) setzt den i-ten anwesenden Zombie auf Pos_set Fall i — auch die Aktoren
 *     16..20 (Bindung kind 2 in scd_work_bind gegen RE15_ACTOR_MAX).
 *   raum20b0: ROOM20B0 sub00 (35 FX-Emitter Typ 0x24, Slots 0..34, @0x1aae..@0x1d56): die Slots 0..19 erscheinen
 *     (Aktor 1..20), die Slots 20..34 liegen im Original hinter dem 20er-Feld und bleiben verworfen.
 *   kein_platz: irons_tod_1150.c slots_zuweisen — ist kein Aktor-Platz frei, bekommt ein Port-Spawn den Slot
 *     RE15_ACTOR_MAX-1 (= Aktor RE15_ACTOR_MAX, verworfen) und das Tot-Bit 0xFF; vorher stand dort fest 0x0f
 *     (= Aktor 16), mit 21 Plaetzen ein GUELTIGER Aktor, der dann ueberschrieben worden waere.
 *
 * Teile (je ein ctest-Eintrag): feld raum1030 raum1031 raum1040 raum1041 raum20b0 kein_platz */
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
#include "re15_irons_tod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

_Static_assert(RE15_ACTOR_MAX == 21, "Spieler + 20 Gegner-Plaetze (FUN_80037250 sltiu 0x14 @0x800372b4)");
_Static_assert(RE15_ACTOR_MAX <= 32, "Aktor-Masken dieses Riegels sind uint32_t");

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static uint16_t           s_rdt_raum = 0;

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

/* Gegner-Bank aus CDEMD0.EMS (wie test_r35_cut1150.c / main.c pc_enemy_load): ohne Bank keine Wurzelbewegung. */
static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[0x60000];
static void bank_laden(uint8_t type)
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
    if (!eb || len > sizeof s_bank_buf) return;
    memcpy(s_bank_buf, s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf, len, &eb->skel_own, &eb->anim_own) == 0);
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r35_cut1150.c). */
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

/* Raum aufbauen wie das Spiel (scd_room_reenter = derselbe Einhaengepunkt wie die Tuer; die Flags in g_game
 * bleiben stehen, wie beim Raumwechsel). */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x16);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Bitmaske der aktiven Aktoren eines Typs (Bit s = Aktor s). */
static uint32_t maske(uint8_t typ)
{
    uint32_t m = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) m |= 1u << s;
    return m;
}
static int bits(uint32_t m) { int n = 0; while (m) { n += (int)(m & 1u); m >>= 1; } return n; }
static void maske_text(uint32_t m, char *out, size_t n)
{
    size_t o = 0; out[0] = 0;
    for (int s = 1; s < 32 && o + 4 < n; s++)
        if (m & (1u << s)) o += (size_t)snprintf(out + o, n - o, "%s%d", o ? "," : "", s);
}

/* ---- Teil: feld --------------------------------------------------------------------------------------- */
static void teil_feld(void)
{
    printf("== feld ==\n");
    PRUEF(RE15_ACTOR_MAX == 21, "RE15_ACTOR_MAX = %d = Spieler + 20 Gegner-Plaetze (@0x800372b4 sltiu 0x14)", RE15_ACTOR_MAX);
    PRUEF(sizeof g_actors / sizeof g_actors[0] == 21, "g_actors fuehrt %u Plaetze", (unsigned)(sizeof g_actors / sizeof g_actors[0]));
}

/* ---- Teile: raum1030 / raum1031 / raum1040 / raum1041 ---------------------------------------------------
 * Je k = 0..20: Records 0..k-1 tot (Zone 7 = re15_em_status_zone Stage 1-3, Tot-Bit = tot0 + Slot). */
typedef struct { uint16_t raum; uint8_t tot0; int limit; int32_t px, pz; int16_t rot; uint8_t cut; } raum_t;

/* Pos_set-Faelle 0..5 der Aufstellung ROOM1030 sub00 Else-Zweig ((4,15)=0): @0x020EC `32 00 ee dc 00 00 1a 93`,
 * @0x02100, @0x02114, @0x02128, @0x0213C, @0x02150 (x = Halbwort +2, z = Halbwort +6). */
static const int32_t k_1030_auf[6][2] = {
    { -8978, -27878 }, { -11700, -26671 }, { -10000, -30000 }, { -5400, -28110 }, { -7300, -25655 }, { -7300, -29655 } };

static void raum_reihe(const raum_t *r, int tor_bit /* -1 = keins, sonst (4,tor_bit)=1 */)
{
    int fehler0 = g_fail;
    for (int k = 0; k <= 20; k++) {
        re15_game_state_init();
        if (tor_bit >= 0) re15_game_flag_set(4, (uint8_t)tor_bit, 1);
        for (int i = 0; i < k; i++) re15_game_flag_set(7, (uint8_t)(r->tot0 + i), 1);
        re15_irons_tod_zustand_setzen(RE15_IT_AUS);
        if (room_boot(r->raum, r->px, r->pz, r->rot, r->cut) != 0) return;
        frame();
        int n = (20 - k < r->limit) ? 20 - k : r->limit;
        uint32_t soll = 0;
        for (int i = 0; i < n; i++) soll |= 1u << (k + 1 + i);
        uint32_t ist = maske(0x16);
        char ts[96], ss[96]; maske_text(ist, ts, sizeof ts); maske_text(soll, ss, sizeof ss);
        PRUEF(ist == soll, "ROOM%04X%s k=%2d (Records 0..%d tot): %d Zombies, Aktoren {%s} (soll %d: {%s}; Zaehler 0x11=%d)",
              r->raum, tor_bit >= 0 ? " Tor offen" : "", k, k - 1, bits(ist), ts, n, ss, (int)g_scd.work_vars[0x11]);
        if (r->raum == 0x1030 && tor_bit < 0 && n > 0) {
            /* raumeigene Aufstellung: der i-te anwesende Zombie (= Aktor k+1+i) auf Fall i */
            int auf = 0;
            for (int i = 0; i < n && i < 6; i++) {
                const re15_actor_t *a = &g_actors[k + 1 + i];
                if (labs((long)a->x - k_1030_auf[i][0]) <= 64 && labs((long)a->z - k_1030_auf[i][1]) <= 64) auf++;
            }
            PRUEF(auf == n, "ROOM1030 k=%2d: sub00-Aufstellung (0x53 Art 2 ueber 20 Plaetze) setzt %d von %d auf Fall 0..%d",
                  k, auf, n, n - 1);
        }
    }
    printf("  -> ROOM%04X%s: %d Fehler\n", r->raum, tor_bit >= 0 ? " Tor offen" : "", g_fail - fehler0);
}

/* Standplaetze = Tuer-Ziele: 1040 Slot 1 @0x010B8 -> ROOM1030 (-26214,-3861) Blick 0 Cut 8;
 * 1030 Slot 2 @0x01CAA -> ROOM1040 (-20083,-3559) Blick 2048 Cut 0. */
static const raum_t k_1030 = { 0x1030, 0x00, 6, -26214, -3861, 0, 8 };
static const raum_t k_1031 = { 0x1031, 0x00, 6, -26214, -3861, 0, 8 };
static const raum_t k_1040 = { 0x1040, 0x14, 5, -20083, -3559, 2048, 0 };
static const raum_t k_1041 = { 0x1041, 0x14, 5, -20083, -3559, 2048, 0 };

static void teil_raum1030(void) { printf("== raum1030 ==\n"); raum_reihe(&k_1030, -1); }
static void teil_raum1031(void) { printf("== raum1031 ==\n"); raum_reihe(&k_1031, -1); }
/* ROOM1040 main00: Ck(4,5)==1 @0x011F8 (Tor offen) -> Records @0x011FC.., sonst Else @0x0138C -> @0x01390..;
 * beide Zweige tragen dieselben 20 Slots/Tot-Bits. */
static void teil_raum1040(void) { printf("== raum1040 ==\n"); raum_reihe(&k_1040, -1); raum_reihe(&k_1040, 5); }
static void teil_raum1041(void) { printf("== raum1041 ==\n"); raum_reihe(&k_1041, -1); raum_reihe(&k_1041, 5); }

/* ---- Teil: raum20b0 ---------------------------------------------------------------------------------- */
static void teil_raum20b0(void)
{
    printf("== raum20b0 ==\n");
    static const uint16_t raeume[2] = { 0x20B0, 0x20B1 };
    for (int r = 0; r < 2; r++) {
        re15_game_state_init();
        re15_irons_tod_zustand_setzen(RE15_IT_AUS);
        /* Standplatz: re15_room_spawns.h ROOM20B0 (-12635,0,-2905) */
        if (room_boot(raeume[r], -12635, -2905, 0, 0) != 0) return;
        uint32_t m = 0;
        for (int f = 0; f < 30 && m != 0x1ffffeu; f++) { frame(); m = maske(0x24); }
        int fremd = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type != 0x24) fremd++;
        char ts[96]; maske_text(m, ts, sizeof ts);
        PRUEF(m == 0x1ffffeu && fremd == 0,
              "ROOM%04X: Slots 0..19 erscheinen als Aktor 1..20 (Typ 0x24), Slots 20..34 hinter dem 20er-Feld verworfen: {%s}, fremde %d",
              raeume[r], ts, fremd);
    }
}

/* ---- Teil: kein_platz -------------------------------------------------------------------------------- */
static void teil_kein_platz(void)
{
    printf("== kein_platz ==\n");
    /* ROOM1040 ohne Szene aufbauen, dann alle 20 Gegner-Plaetze belegen und das Nachspawn-Programm 14 ausloesen
     * (Zustand AUS + (9,75)=1, irons_tod_1150.c programm_bauen -> slots_zuweisen). */
    re15_game_state_init();
    for (int i = 0; i < 20; i++) re15_game_flag_set(7, (uint8_t)(0x14 + i), 1);   /* Raum leer */
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1040, -20083, -3559, 2048, 0) != 0) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *a = &g_actors[s];
        memset(a, 0, sizeof *a);
        a->active = 1; a->type = 0x21; a->em_flag_id = 0xFF;      /* Platzhalter (Kraehe, ohne Bank still) */
        a->x = -1000 * s; a->z = 30000; a->y = 0;
    }
    re15_game_flag_set(RE15_IT_BANK, RE15_IT_BIT_1040, 1);
    for (int i = 0; i < 5; i++) {   /* die fuenf Auffuell-Records scharf (Tot-Bits 83,84,90,91,95 = 0) */
        static const uint8_t b[5] = { 83, 84, 90, 91, 95 };
        re15_game_flag_set(7, b[i], 0);
    }
    re15_enemy_ai_set_paused(1);   /* Platzhalter ohne KI: gemessen wird allein der Spawn-Weg der VM */
    int slot = scd_event_fire(RE15_IT_EREIGNIS);
    PRUEF(slot >= 0, "Nachspawn-Programm 14 laeuft (Faden %d)", slot);
    int len = 0; const uint8_t *p = re15_irons_tod_laufprogramm(&len);
    int n44 = 0, verworfen = 0;
    for (int o = 0; p && o + 20 <= len; ) {
        uint8_t op = p[o];
        int w = (op == 0x44) ? 20 : (op == 0x24 || op == 0x22 || op == 0x09 || op == 0x37) ? 4 : (op == 0x01 || op == 0x29 || op == 0x47 || op == 0x18) ? 2
              : (op == 0x46) ? 10 : (op == 0x4b) ? 3 : (op == 0x36) ? 12 : (op == 0x3b) ? 32 : 0;
        if (w == 0) break;
        if (op == 0x44) { n44++; if (p[o + 1] == (uint8_t)(RE15_ACTOR_MAX - 1) && p[o + 7] == 0xff) verworfen++; }
        o += w;
    }
    PRUEF(n44 == 5 && verworfen == 5, "kein Platz frei: %d von %d Port-Spawns tragen Slot %d (= Aktor %d, verworfen) + Tot-Bit 0xFF",
          verworfen, n44, RE15_ACTOR_MAX - 1, RE15_ACTOR_MAX);
    for (int f = 0; f < 10; f++) frame();
    int heil = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x21 && g_actors[s].x == -1000 * s && g_actors[s].z == 30000) heil++;
    PRUEF(heil == RE15_ACTOR_MAX - 1, "nach 10 Bildern: alle %d belegten Plaetze unveraendert (%d) — kein Platz ueberschrieben",
          RE15_ACTOR_MAX - 1, heil);
    re15_enemy_ai_set_paused(0);
    re15_game_flag_set(RE15_IT_BANK, RE15_IT_BIT_1040, 0);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    re15_game_state_init();
    if (alle || !strcmp(teil, "feld"))       teil_feld();
    if (alle || !strcmp(teil, "raum1030"))   teil_raum1030();
    if (alle || !strcmp(teil, "raum1031"))   teil_raum1031();
    if (alle || !strcmp(teil, "raum1040"))   teil_raum1040();
    if (alle || !strcmp(teil, "raum1041"))   teil_raum1041();
    if (alle || !strcmp(teil, "raum20b0"))   teil_raum20b0();
    if (alle || !strcmp(teil, "kein_platz")) teil_kein_platz();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
