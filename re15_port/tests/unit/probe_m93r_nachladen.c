/* probe_m93r_nachladen.c - MESSSONDE (reiner Befund, KEIN Fix).
 *
 * Frage (Nutzer): "Bei der Beretta M93R klappt das automatische Nachladen nicht,
 * wenn sie leer ist, aber noch Munition im Inventar vorhanden."
 *
 * Original-Gate (selbst disassembliert, info/Re1.5/PSX.EXE, Sub 1 der Standard-FSM):
 *   @0x80033308 andi 0x40      SQUARE gehalten
 *   @0x8003331c beq            Magazin == 0  (FUN_8004ea6c @0x80033314)
 *   @0x80033344 beq            SQUARE-FLANKE 0x800ac76c @0x80033338
 *   @0x80033354 beq            Reserve       (FUN_8004eb70 @0x8003334c)
 *   @0x8003336c beq / sltiu 9  Waffen-Id < 9 (lbu 0x800aca5d @0x80033360)
 *   @0x80033378 sh 4 -> 0x800aca5a   NACHLADEN
 * GENAU FUENF Bedingungen. Kein Melee-Flag, kein Recoil-Flag.
 * Die Klassentrennung liegt im Dispatch, der JEDES BILD frisch 0x800aca5d liest
 * (@0x80032e60 lbu / @0x80032e74 Tabelle 0x80074030 / @0x80032e84 jalr).
 *
 * Der Port latcht die Klasse EINMAL beim Zieleintritt (player_common.c:916) und
 * fragt sie in re15_player_reload_start() ab (player_common.c:340).
 *
 * Die Sonde faehrt den ECHTEN Weg: game_step + Pad, R1 halten bis zielbereit,
 * dann Abzugsbursts, und protokolliert je Bild
 *   Magazinstand | Munitionsmenge | s_aim_melee | Phase/Recoil | Dauerfeuer-FSM |
 *   ob der Nachlade-Zweig betreten wird.
 * Abdeckung wird ausgegeben (Anzahl Bilder je Lauf).
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
#include "re15_menu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_dbg(int*,int*,int*,int*,int*);
extern int  re15_player_aim_clip(void);
extern int  re15_player_aim_phase_debug(void);
extern int  re15_player_aim_melee_dbg(void);      /* MESSHAKEN, nur lesend */
extern int  re15_player_aim_ready(void);
extern int  re15_ammo_mag_nonzero(void);
extern int  re15_ammo_reserve_slot(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

/* PL00W05 (M93R) EDD @0x8, selbst geparst: 14 Clips, Clip 13 (Nachladen) = 32 Bilder.
 * PL00W03 (Browning) = [22,16,52,1,50,30,10,23,1,24,1,24,1,32]. */
static const uint16_t FC_W05[14] = {22,16,52,1,50,30,10,22,1,23,1,23,1,32};
static const uint16_t FC_W03[14] = {22,16,52,1,50,30,10,23,1,24,1,24,1,32};

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

static int mag_qty(void)
{
    int s = re15_inv_equipped_slot();
    if (s < 0 || s >= RE15_INV_MAX_SLOTS) return -1;
    return (int)g_inv.slots[s].qty;
}
static int ammo_qty(void)
{
    int s = re15_inv_find_item(0x15);
    return (s < 0) ? -1 : (int)g_inv.slots[s].qty;
}

static void bringup(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 60; i++) scd_vm_tick();
    for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 30000; pl->z = 30000;
    re15_collision_set_band(0);
}

/* wid: 5 = M93R, 3 = Browning. mag = Startmagazin, res = Munitionsmenge 0x15.
 * zyklen = Anzahl Abzugszyklen (die Browning braucht 15, die M93R 5).
 * melee_zwang = Zieleintritt MIT DEM MESSER, danach Waffenwechsel ohne Zielabbruch. */
static int lauf(const char *name, int wid, int mag, int res, const uint16_t *fcs,
                int zyklen, int melee_zwang, int *out_reload_frame)
{
    bringup();
    re15_inv_init();
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;          /* KNIFE (Briefing-Slot 0) */
    g_inv.slots[1].id = (uint8_t)wid; g_inv.slots[1].qty = (uint8_t)mag;
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = (uint8_t)res;
    re15_inv_set_prev_equip_slot(0x80);
    re15_player_set_aim_clip_lens(fcs, 14);
    re15_player_set_equipped_weapon(melee_zwang ? 1 : wid);

    printf("\n===== %s =====\n", name);
    printf("Start: Waffe=%d Magazin=%d Munition0x15=%d eqSlot=%d\n",
           re15_player_equipped_weapon(), mag_qty(), ammo_qty(), re15_inv_equipped_slot());

    int bilder = 0, reload_frame = -1, refill_frame = -1;
    int raise = 0;
    for (; raise < 90 && !re15_player_aim_ready(); raise++) { frame(RE15_PAD_BIT_R1, 0); bilder++; }
    printf("ZIELBEREIT nach %d R1-Bildern (melee_latch=%d)\n", raise, re15_player_aim_melee_dbg());

    if (melee_zwang) {
        re15_player_set_equipped_weapon(wid);   /* Waffenwechsel MITTEN im Zielen */
        printf("  -> Waffe im Zielen auf %d gewechselt; melee_latch bleibt %d\n",
               re15_player_equipped_weapon(), re15_player_aim_melee_dbg());
    }

    printf("  B | pad    | w | mag | 0x15 | melee | ph rec | magNZ | res | clip fr/fc\n");
    /* Je Zyklus 40 Bilder: Flanke im Bild 0, Abzug in den letzten 6 Bildern LOS. */
    for (int z = 0; z < zyklen; z++) {
        for (int k = 0; k < 40; k++) {
            int halten = (k < 34);
            uint16_t cur  = (uint16_t)(RE15_PAD_BIT_R1 | (halten ? RE15_PAD_BIT_SQUARE : 0));
            uint16_t edge = (uint16_t)((k == 0) ? RE15_PAD_BIT_SQUARE : 0);
            int ph=0,rc=0,af=0,ac=0,tk=0;
            re15_player_aim_dbg(&ph,&rc,&af,&ac,&tk);
            int m0 = mag_qty(), a0 = ammo_qty();
            frame(cur, edge);
            bilder++;
            int ph2=0,rc2=0,af2=0,ac2=0,tk2=0;
            int fc = re15_player_aim_dbg(&ph2,&rc2,&af2,&ac2,&tk2);
            if (ph2 == 3 && reload_frame < 0) reload_frame = bilder;
            if (a0 >= 0 && ammo_qty() >= 0 && ammo_qty() < a0 && refill_frame < 0) refill_frame = bilder;
            if (k == 0 || ph != ph2 || m0 != mag_qty() || a0 != ammo_qty())
                printf("%4d | %s | %d | %3d | %4d |   %d   | %d%d %d%d |   %d   |  %d  | %2d %2d/%d\n",
                       bilder, halten ? (k==0?"R1+SQ^":"R1+SQ ") : "R1    ",
                       re15_player_equipped_weapon(), mag_qty(), ammo_qty(),
                       re15_player_aim_melee_dbg(), ph, ph2, rc, rc2,
                       re15_ammo_mag_nonzero(), re15_ammo_reserve_slot(),
                       re15_player_aim_clip(), (int)g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame, fc);
        }
    }
    printf("ABDECKUNG: %d Bilder gefahren; Nachlade-Phase(3) zuerst in Bild %d; Nachfuellung in Bild %d\n",
           bilder, reload_frame, refill_frame);
    printf("ENDE: Magazin=%d Munition0x15=%d melee_latch=%d\n",
           mag_qty(), ammo_qty(), re15_player_aim_melee_dbg());
    if (out_reload_frame) *out_reload_frame = reload_frame;
    return bilder;
}

/* Lauf E - DER ECHTE WEG durch die Port-Zustandsmaschinen.
 * Messer angelegt (Briefing-Default, DAT_800aca5d==1), zielen, dann das INVENTAR
 * ueber die START-Flanke oeffnen (game_step_common.c:1133-1137 - dort steht KEINE
 * Zielbedingung), im Menue die M93R anlegen (Cursor = re15_inv_set_equipped_slot,
 * byte-true DAT_800b25c8) und schliessen (echter Close-Commit menu_common.c:1885-1888
 * bzw. :1347 -> re15_player_set_equipped_weapon). R1 bleibt GEHALTEN. Danach Abzug. */
static int lauf_E(int *out_reload_frame)
{
    bringup();
    re15_inv_init();
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;   /* KNIFE  (Briefing-Slot 0) */
    g_inv.slots[1].id = 0x05; g_inv.slots[1].qty = 0;   /* M93R, Magazin LEER       */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;  /* H.GUN BULLETS            */
    re15_inv_set_prev_equip_slot(0x80);
    re15_player_set_aim_clip_lens(FC_W05, 14);
    re15_player_set_equipped_weapon(1);                 /* Messer angelegt */

    printf("\n===== E  ECHTER WEG: Messer zielen -> Inventar -> M93R anlegen -> Abzug =====\n");
    int bilder = 0, raise = 0;
    for (; raise < 90 && !re15_player_aim_ready(); raise++) { frame(RE15_PAD_BIT_R1, 0); bilder++; }
    printf("ZIELBEREIT (Messer) nach %d R1-Bildern: melee_latch=%d Waffe=%d\n",
           raise, re15_player_aim_melee_dbg(), re15_player_equipped_weapon());

    frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_START), RE15_PAD_BIT_START); bilder++;
    printf("nach START-Flanke: menue_eingefroren=%d\n", re15_menu_gameplay_frozen());
    if (!re15_menu_gameplay_frozen()) {
        printf("FEHLLAUF: Menue oeffnet im Zielzustand NICHT - dieser Lauf sagt NICHTS\n");
        if (out_reload_frame) *out_reload_frame = -2;
        return bilder;
    }
    for (int i = 0; i < 20; i++) { frame(RE15_PAD_BIT_R1, 0); bilder++; }
    {
        int ph=0,rc=0,af=0,ac=0,tk=0; re15_player_aim_dbg(&ph,&rc,&af,&ac,&tk);
        printf("im Menue (20 Bilder): Zielphase=%d recoil=%d melee_latch=%d (eingefroren erhalten)\n",
               ph, rc, re15_player_aim_melee_dbg());
    }
    printf("Menue-Zustand: stage=%d phase=%d substate=%d\n",
           re15_menu_stage(), re15_menu_phase(), re15_menu_substate());
    re15_inv_set_equipped_slot(1);            /* Cursor: M93R anlegen (DAT_800b25c8 := 1) */
    /* Zweite START-Flanke = CANCEL/Schliessen (menu_common.c:234 bzw. :333 -> s_phase=2),
     * der Close-Lauf traegt den echten Equip-Commit menu_common.c:1339-1350. */
    frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_START), RE15_PAD_BIT_START); bilder++;
    for (int i = 0; i < 200 && re15_menu_gameplay_frozen(); i++) { frame(RE15_PAD_BIT_R1, 0); bilder++; }
    printf("Menue zu nach %d Bildern gesamt; eingefroren=%d\n", bilder, re15_menu_gameplay_frozen());
    printf("nach Menue-Schliessen: Waffe=%d eqSlot=%d melee_latch=%d Zielphase=%d\n",
           re15_player_equipped_weapon(), re15_inv_equipped_slot(),
           re15_player_aim_melee_dbg(), re15_player_aim_phase_debug() & 0xf);
    int reload_frame = -1;
    printf("  B | pad    | w | mag | 0x15 | melee | ph | magNZ | res\n");
    for (int z = 0; z < 4; z++) {
        for (int k = 0; k < 40; k++) {
            int halten = (k < 34);
            uint16_t cur  = (uint16_t)(RE15_PAD_BIT_R1 | (halten ? RE15_PAD_BIT_SQUARE : 0));
            uint16_t edge = (uint16_t)((k == 0) ? RE15_PAD_BIT_SQUARE : 0);
            int m0 = mag_qty(), a0 = ammo_qty();
            frame(cur, edge); bilder++;
            int ph2=0,rc2=0,af2=0,ac2=0,tk2=0; re15_player_aim_dbg(&ph2,&rc2,&af2,&ac2,&tk2);
            if (ph2 == 3 && reload_frame < 0) reload_frame = bilder;
            if (k == 0 || m0 != mag_qty() || a0 != ammo_qty())
                printf("%4d | %s | %d | %3d | %4d |   %d   | %d  |   %d   |  %d\n",
                       bilder, (k==0)?"R1+SQ^":"R1+SQ ", re15_player_equipped_weapon(),
                       mag_qty(), ammo_qty(), re15_player_aim_melee_dbg(), ph2,
                       re15_ammo_mag_nonzero(), re15_ammo_reserve_slot());
        }
    }
    printf("ABDECKUNG E: %d Bilder; Nachlade-Phase(3) zuerst in Bild %d%s\n",
           bilder, reload_frame, (reload_frame < 0) ? "   <== NIE (stumm verschluckt)" : "");
    printf("ENDE E: Magazin=%d Munition0x15=%d\n", mag_qty(), ammo_qty());
    if (out_reload_frame) *out_reload_frame = reload_frame;
    return bilder;
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    int rA=-1, rB=-1, rC=-1, rD=-1, rE=-1;
    int nA = lauf("A  M93R (Id 5), Magazin 15, Munition 50 - der Nutzerfall",
                  5, 15, 50, FC_W05, 7, 0, &rA);
    int nB = lauf("B  BROWNING (Id 3), Magazin 15, Munition 50 - Gegenprobe (1 Schuss/Abzug)",
                  3, 15, 50, FC_W03, 18, 0, &rB);
    int nC = lauf("C  M93R, Magazin SCHON 0 - nur das Gate",
                  5, 0, 50, FC_W05, 5, 0, &rC);
    int nD = lauf("D  M93R, Magazin 0, aber mit dem Messer angezielt (melee-Latch)",
                  5, 0, 50, FC_W05, 5, 1, &rD);
    int rD2=-1;
    int nD2 = lauf("D2 M93R, Magazin 15, mit dem Messer angezielt (melee-Latch) - feuert sie?",
                   5, 15, 50, FC_W05, 7, 1, &rD2);
    (void)nD2; (void)rD2;
    int nE = lauf_E(&rE);

    printf("\n================ ZUSAMMENFASSUNG ================\n");
    printf("Abdeckung gesamt: %d Bilder (A=%d B=%d C=%d D=%d E=%d)\n",
           nA+nB+nC+nD+nE, nA,nB,nC,nD,nE);
    printf("A M93R normal         : Nachlade-Phase in Bild %d %s\n", rA, rA>0?"":"(NIE)");
    printf("B Browning normal     : Nachlade-Phase in Bild %d %s\n", rB, rB>0?"":"(NIE)");
    printf("C M93R leer von Start : Nachlade-Phase in Bild %d %s\n", rC, rC>0?"":"(NIE)");
    printf("D M93R, melee-Latch   : Nachlade-Phase in Bild %d %s\n", rD, rD>0?"":"(NIE)");
    printf("E M93R ueber das Menue: Nachlade-Phase in Bild %d %s\n", rE, rE>0?"":"(NIE)");
    return 0;
}
