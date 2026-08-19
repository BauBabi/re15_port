/* probe_re2_hp_model.c — MESSUNG: Treffer bis zum Tod, ueber den ECHTEN Weg.
 *
 * Nutzer-Report (RE2-KI-Modus, Dinner Room): "Die Zombies stecken viel zu viel ein. Viel mehr
 * als im Original."
 *
 * Diese Sonde misst NICHT synthetisch: sie faehrt game_step() mit Pad-Eingaben (R1 halten ->
 * SQUARE) in einem echten Raum mit echten Sce_em_set-Spawns und zaehlt, wie viele Treffer bis
 * hp < 0 noetig sind — pro Waffe, pro Gegnertyp und pro Konfiguration:
 *
 *   RE1.5            — der byte-true RE1.5-Pfad (MUSS unveraendert bleiben)
 *   RE2 / Modell AUS — der bisherige RE2-Modus (RE2-Gehirn, RE1.5-Zahlen)  = "vorher"
 *   RE2 / Modell AN  — RE2-Gehirn + RE2-HP + RE2-Schadenszeilen            = "nachher"
 *
 * Kein add_test — reine Messsonde (die Zahlen wandern in den Report/PIN).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static re15_rdt_t         s_rdt;
static int                s_room_id  = 0x1140;
static int                s_fire_sub = -1;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

/* ⛔ ABSTAND NACHFUEHREN (2026-08-19): der RE2-Stagger stoesst den Zombie um -450 Einheiten
 * zurueck (`re2z_thrust(e, -450)`, `sh v0,324` @0x80105C88) und der Knockdown wirft ihn noch
 * weiter. Ohne Nachfuehren faellt er aus der Messer-Reichweite (1100 + Radius) und die Messung
 * misst REICHWEITE statt SCHADEN. Der Spieler laeuft hier also mit — Position/Rotation zu
 * setzen ist Harness-Hilfe, der SCHUSS-WEG (Aim-FSM + game_step + weapon_fire) bleibt echt.
 * Gleiches Verfahren wie probe_re2_hit_crow_spider.c track(). */
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);   /* Signatur wie re15_math.h */
static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Ein STEHENDER Gegner des gewuenschten Typs (Liege-Spawns sind per Elevations-Band
 * byte-true ausgeschlossen und wuerden die Messung verfaelschen). want_type < 0 = egal. */
static int pick_enemy(int want_type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40
            && !(g_actors[s].grid_id & 0x80)
            && (want_type < 0 || g_actors[s].type == (uint8_t)want_type)) return s;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40
            && (want_type < 0 || g_actors[s].type == (uint8_t)want_type)) return s;
    return -1;
}

/* Rueckgabe: Trefferzahl (bis hp < 0 oder Budget-Ende). *out_dead = 1, wenn er gestorben ist. */
static int measure(int flavor, int model_on, int weapon, int want_type,
                   int *out_hp0, int *out_type, int *out_dmg, int *out_dead)
{
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_re2_damage_model_set(model_on);
    bringup();
    re15_inv_load_briefing();
    /* Die Briefing-Ausruestung kennt nur Messer + Pistole. Ohne volles Magazin haelt die
     * Feuer-Klemme in game_step (@0x80033300-84, `re15_ammo_mag_nonzero`) jeden anderen
     * Schuss auf — die Messung saehe dann faelschlich "0 Schaden". */
    if (weapon >= 3) {
        g_inv.slots[1].id  = (uint8_t)weapon;
        g_inv.slots[1].qty = 250;
    }
    re15_player_set_equipped_weapon(weapon);

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = pick_enemy(want_type);
    if (out_dead) *out_dead = 0;
    if (slot < 0) { if (out_hp0) *out_hp0 = 0; if (out_type) *out_type = -1; return -1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    const int32_t fire_back = (weapon < 3) ? 900 : 2600;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, fire_back);

    int hp0 = e->hp;
    if (out_hp0)  *out_hp0  = hp0;
    if (out_type) *out_type = e->type;

    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, fire_back); frame(RE15_PAD_BIT_R1, 0);
    }

    int hits = 0, hp_last = e->hp, first_dmg = 0, dead = 0;
    for (int f = 0; f < 4000; f++) {
        pl->hp = 100;
        track(slot, fire_back);   /* Rueckstoss -450 @0x80105C88 nachlaufen (s.o.) */
        /* SQUARE als Impuls (2 Frames an / 2 aus): die Press-Flanke ist der Ausloeser des
         * Nachlade-/Leerklick-Zweigs UND der einzige Weg, der Schuss fuer Schuss feuert. */
        int on = ((f >> 1) & 1) == 0;
        frame((uint16_t)(RE15_PAD_BIT_R1 | (on ? RE15_PAD_BIT_SQUARE : 0)),
              (uint16_t)(on && ((f & 3) == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) { if (!hits) first_dmg = hp_last - e->hp; hits++; }
        hp_last = e->hp;
        /* TOD-KRITERIUM: nicht nur hp<0. Die RE2-Spinne wird als LEICHE wieder auf hp = 1
         * gesetzt (`sh 1,342` EMS25.BIN @0x80103EB0 / @0x801041AC, Port
         * enemy_ai_re2_spider.c:2149/2179) — mit `hp<0` allein meldet die Sonde bei ihr
         * faelschlich "stirbt nicht". Zustand 3 = DEATH (@0x8004728C-90), 7 = CORPSE. */
        if (e->hp < 0 || e->state == 3 || e->state == 7) { dead = 1; break; }
    }
    if (out_dmg)  *out_dmg  = first_dmg;
    if (out_dead) *out_dead = dead;
    return hits;
}

static const char *WNAME(int w)
{
    switch (w) { case 1: return "Messer"; case 3: return "Pistole"; case 5: return "Magnum-Kl.";
                 case 8: return "Schrot"; case 12: return "Ingram"; case 13: return "SPAS-12";
                 default: return "?"; }
}

static void row(const char *room, int weapon, int want_type)
{
    int hp15=0,t15=0,d15=0,k15=0, hpOff=0,tOff=0,dOff=0,kOff=0, hpOn=0,tOn=0,dOn=0,kOn=0;
    int n15  = measure(RE15_AI_FLAVOR_RE15, 1, weapon, want_type, &hp15,  &t15,  &d15,  &k15);
    int nOff = measure(RE15_AI_FLAVOR_RE2,  0, weapon, want_type, &hpOff, &tOff, &dOff, &kOff);
    int nOn  = measure(RE15_AI_FLAVOR_RE2,  1, weapon, want_type, &hpOn,  &tOn,  &dOn,  &kOn);
    /* ⛔ NICHT GETROFFEN heisst NICHT "0 Schaden": landet in KEINEM der drei Laeufe ein
     * Treffer, ist das ein MESSRAHMEN-Artefakt (bei der Kraehe das Elevations-Band — die
     * fliegende Kraehe braucht den UP/DOWN-Pad-Zug, siehe probe_re2_hit_crow_spider.c).
     * Solche Zeilen werden als "n/a" ausgewiesen statt als Null-Schaden gemeldet. */
    if (n15 <= 0 && nOff <= 0 && nOn <= 0) {
        printf("  %-13s %-11s n/a — kein Treffer in KEINEM Modus (Messrahmen: Elevations-Band/"
               "Reichweite), KEIN Befund\n", room, WNAME(weapon));
        return;
    }
    printf("  %-13s %-11s RE1.5: %3d%s (hp %3d dmg %3d) | RE2 AUS: %3d%s (hp %3d dmg %3d)"
           " | RE2 AN: %3d%s (hp %3d dmg %3d)\n",
           room, WNAME(weapon), n15, k15?"T":"-", hp15, d15,
           nOff, kOff?"T":"-", hpOff, dOff, nOn, kOn?"T":"-", hpOn, dOn);
    (void)t15; (void)tOff; (void)tOn;
}

static int load_room(const char *base, const char *sub, int room_id, int fsub)
{
    char path[600];
    snprintf(path, sizeof path, "%s/%s", base, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse: %s\n", path); return 0; }
    s_room_id = room_id; s_fire_sub = fsub;
    return 1;
}

int main(void)
{
    const char *envb = getenv("RE15_ASSET_DIR");
    const char *base = (envb && *envb) ? envb : RE15_ASSET_PSX_DIR;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    static const int weapons[] = { 1, 3, 5, 8, 12, 13 };

    if (load_room(base, "STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        printf("\n===== ROOM1140 Dinner Room — ZOMBIE Typ 0x10 =====\n");
        for (unsigned i = 0; i < sizeof weapons / sizeof weapons[0]; i++)
            row("1140 z-0x10", weapons[i], 0x10);
        printf("\n===== ROOM1140 Dinner Room — ZOMBIE Typ 0x11 =====\n");
        for (unsigned i = 0; i < sizeof weapons / sizeof weapons[0]; i++)
            row("1140 z-0x11", weapons[i], 0x11);
        printf("\n===== ROOM1140 Dinner Room — ZOMBIE Typ 0x16 =====\n");
        for (unsigned i = 0; i < sizeof weapons / sizeof weapons[0]; i++)
            row("1140 z-0x16", weapons[i], 0x16);
    }
    if (load_room(base, "STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        printf("\n===== ROOM1190 — HUND 0x20 =====\n");
        for (unsigned i = 0; i < sizeof weapons / sizeof weapons[0]; i++)
            row("1190 dog", weapons[i], 0x20);
    }
    if (load_room(base, "STAGE1/ROOM10C0.RDT", 0x10c0, -1)) {
        printf("\n===== ROOM10C0 — KRAEHE 0x21 =====\n");
        for (unsigned i = 0; i < sizeof weapons / sizeof weapons[0]; i++)
            row("10C0 crow", weapons[i], 0x21);
    }
    /* Die Adult-Spinne steht NICHT in ROOM1090 (dort nur 7x Baby 0x26) — sie ist ein
     * STAGE2-Gegner; ROOM2050 stellt 2 Stueck auf (Raum-Suche in probe_re2_hit_crow_spider.c). */
    if (load_room(base, "STAGE2/ROOM2050.RDT", 0x2050, -1)) {
        printf("\n===== ROOM2050 — ADULT-SPINNE 0x25 =====\n");
        for (unsigned i = 0; i < sizeof weapons / sizeof weapons[0]; i++)
            row("2050 spider", weapons[i], 0x25);
    }
    return 0;
}
