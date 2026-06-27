/*
 * Stubs for engine symbols referenced by tested modules.
 *
 * scd_vm.c calls re15_debug_text() (for opcode 0xFE DbgText). In the host
 * test build there's no rendering, so we record the call in a buffer
 * tests can inspect.
 */
#include <string.h>
#include "re15_engine.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_inventory.h"
#include "re15_to_re2.h"
#include "re15_scd.h"

re15_engine_state_t g_engine;

/* --- 2026-06-09: minimal stubs so the grown SCD VM links in the host test ---
 * scd_vm.c now references the AOT/actor/inventory/flag subsystems; the host VM
 * test exercises only the interpreter (dispatch, If/Switch, work-vars), so these
 * are inert stubs. (The test target was stale; these restore it.) */
re15_aot_state_t g_aot;
re15_actor_t     g_actors[RE15_ACTOR_MAX];
uint8_t          g_actor_count;
uint8_t          g_aot_action_pressed;

void re15_aot_init(void) {}
int  re15_aot_set(int slot, uint8_t type, uint8_t event_id,
                  int32_t cx, int32_t cz, int32_t half_w, int32_t half_h)
{ (void)slot;(void)type;(void)event_id;(void)cx;(void)cz;(void)half_w;(void)half_h; return 0; }
int  re15_aot_set_door(int slot, int32_t cx, int32_t cz, int32_t half_w, int32_t half_h,
                       uint8_t target_cut, int32_t spawn_x, int32_t spawn_y, int32_t spawn_z,
                       int16_t spawn_yaw_4096)
{ (void)slot;(void)cx;(void)cz;(void)half_w;(void)half_h;(void)target_cut;
  (void)spawn_x;(void)spawn_y;(void)spawn_z;(void)spawn_yaw_4096; return 0; }
int  re15_aot_set_item(int slot, int32_t cx, int32_t cz, int32_t half_w, int32_t half_h,
                       uint8_t item_type, uint8_t amount)
{ (void)slot;(void)cx;(void)cz;(void)half_w;(void)half_h;(void)item_type;(void)amount; return 0; }
int  re15_aot_set_stair(int slot, int32_t cx, int32_t cz, int32_t half_w, int32_t half_h,
                        uint8_t down_end, uint8_t band)
{ (void)slot;(void)cx;(void)cz;(void)half_w;(void)half_h;(void)down_end;(void)band; return 0; }
int  re15_aot_set_cam_switch(int slot, int32_t cx, int32_t cz, int32_t half_w, int32_t half_h,
                             uint8_t cam_from, uint8_t target_cut)
{ (void)slot;(void)cx;(void)cz;(void)half_w;(void)half_h;(void)cam_from;(void)target_cut; return 0; }
void re15_aot_reset(int slot) { (void)slot; }
void re15_inv_init(void) {}
void re15_game_state_init(void) {}
void re15_actor_init(void) {}
int32_t re15_actor_get_member(int slot, uint8_t member_id) { (void)slot;(void)member_id; return 0; }
void    re15_actor_set_member(int slot, uint8_t member_id, int32_t value)
{ (void)slot;(void)member_id;(void)value; }
int  re15_to_re2_member_id(int re15_id) { return re15_id; }
int  re15_game_flag_get(uint8_t zone, uint8_t idx) { (void)zone;(void)idx; return 0; }
void re15_game_flag_set(uint8_t zone, uint8_t idx, int value) { (void)zone;(void)idx;(void)value; }

#define DBGTEXT_BUF_MAX 64
char g_last_dbgtext[DBGTEXT_BUF_MAX] = {0};
int  g_dbgtext_call_count = 0;

void re15_debug_text(int x, int y, int z, const char *text)
{
    (void) x; (void) y; (void) z;
    g_dbgtext_call_count++;
    int i = 0;
    while (text[i] && i < DBGTEXT_BUF_MAX - 1) {
        g_last_dbgtext[i] = text[i];
        i++;
    }
    g_last_dbgtext[i] = '\0';
}
