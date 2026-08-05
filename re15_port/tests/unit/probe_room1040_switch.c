/* probe_room1040_switch.c — DIAGNOSE (kein ctest noetig): Nutzer-Report BUG 3
 * "In ROOM1040 kommt DIREKT BEIM BETRETEN die Frage, ob man den Schalter betaetigen
 *  will — obwohl man gar nicht am Schalter steht. Kontrolle kommt nicht zurueck."
 *
 * MESSUNG: Raum 1040 exakt wie der Port ihn beim Tuer-Eintritt aufsetzt
 * (scd_room_reenter) hochfahren und protokollieren:
 *   - welche AOT-Slots installiert sind (Typ, Rect, Band, Flags)
 *   - ob der Spieler beim Eintritts-Spawn in irgendeinem Rect steht
 *   - ob eine Message/Choice-Dialog offen ist (g_scd.message_*)
 *   - welche SCD-Threads laufen
 *
 * Argument 1 (optional) = Raum-Hex (Default 1040), Argument 2/3 = Spawn x/z.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static const char *type_name(uint8_t t)
{
    switch (t) {
    case RE15_AOT_TYPE_GENERIC:    return "GENERIC";
    case RE15_AOT_TYPE_DOOR:       return "DOOR";
    case RE15_AOT_TYPE_ITEM:       return "ITEM";
    case RE15_AOT_TYPE_CAM_SWITCH: return "CAM_SWITCH";
    case RE15_AOT_TYPE_STAIR:      return "STAIR";
    case RE15_AOT_TYPE_MESSAGE:    return "MESSAGE";
    case RE15_AOT_TYPE_AUTO_EVENT: return "AUTO_EVENT";
    case RE15_AOT_TYPE_EXAMINE_WORKVAR: return "EXAMINE_WV";
    case RE15_AOT_TYPE_FLAG_CHG:   return "FLAG_CHG";
    case RE15_AOT_TYPE_WATER:      return "WATER";
    case RE15_AOT_TYPE_RAMP:       return "RAMP";
    case RE15_AOT_TYPE_NONE:       return "NONE";
    default:                       return "?";
    }
}

static void dump(const char *tag, int32_t px, int32_t pz)
{
    printf("\n== %s ==\n", tag);
    printf("   msg: active=%d query=%d fsm=%d id=%d block=?  player_mode=%d\n",
           g_scd.message_active, g_scd.message_query, g_scd.message_fsm_active,
           g_scd.message_id, g_scd.player_mode);
    printf("   threads:");
    for (int i = 0; i < SCD_THREAD_COUNT; i++)
        if (g_scd.threads[i].active) printf(" [%d]", i);
    printf("\n");
    for (int i = 0; i < 16; i++) {
        const re15_aot_t *s = &g_aot.slots[i];
        if (!s->active && s->type == 0) continue;
        long dx = (long)(px - s->x); if (dx < 0) dx = -dx;
        long dz = (long)(pz - s->z); if (dz < 0) dz = -dz;
        int inside = (dx <= (long)s->half_w) && (dz <= (long)s->half_h);
        printf("   slot %2d: act=%d type=%-11s ev=0x%02x band=0x%02x sceflags=0x%02x "
               "c=(%6ld,%6ld) h=(%5ld,%5ld) inside=%d\n",
               i, s->active, type_name(s->type), s->event_id, s->band, s->sce_flags,
               (long)s->x, (long)s->z, (long)s->half_w, (long)s->half_h, inside);
    }
    for (int a = 1; a < RE15_ACTOR_MAX; a++) {
        if (!g_actors[a].active) continue;
        printf("   actor %2d: type=0x%02x pos=(%6ld,%6ld,%6ld) floor=%d\n", a,
               g_actors[a].type, (long)g_actors[a].x, (long)g_actors[a].y,
               (long)g_actors[a].z, g_actors[a].floor);
    }
    printf("   flags z4: ");
    for (int b = 0; b < 8; b++) printf("%d", re15_game_flag_get(4, (uint8_t)(0xf0 + b)));
    printf("  f(4,5)=%d f(4,4)=%d f(5,0x21)=%d f(5,0x20)=%d f(5,0x22)=%d f(5,0x23)=%d f(5,0)=%d\n",
           re15_game_flag_get(4, 5), re15_game_flag_get(4, 4),
           re15_game_flag_get(5, 0x21), re15_game_flag_get(5, 0x20),
           re15_game_flag_get(5, 0x22), re15_game_flag_get(5, 0x23),
           re15_game_flag_get(5, 0));
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    unsigned room = (argc > 1) ? (unsigned)strtoul(argv[1], NULL, 16) : 0x1040u;
    int32_t px = (argc > 3) ? (int32_t)strtol(argv[2], NULL, 10) : -24000;
    int32_t pz = (argc > 3) ? (int32_t)strtol(argv[3], NULL, 10) :  16500;

    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT", base,
             ((room >> 12) & 0xF) ? ((room >> 12) & 0xF) : 1u, room);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "RDT nicht lesbar: %s\n", path); return 77; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { fprintf(stderr, "parse fail\n"); return 1; }
    printf("ROOM%04X: main_scd=%p sub_scd[0]=%p sub_scd[1]=%p sub_scd[2]=%p\n",
           room, (void *)rdt.main_scd, (void *)rdt.sub_scd[0],
           (void *)rdt.sub_scd[1], (void *)rdt.sub_scd[2]);

    scd_vm_init();
    re15_actor_init();
    g_current_room_id = room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;

    /* Argument 4 = "preflag": Flag(5,0x21) VORHER setzen — das ist der Zustand, den ein
     * frueherer SQUARE-Druck auf den Schalter-AOT (slot 4, sce=4 -> group 5 bit 0x21)
     * hinterlaesst. Nichts loescht ihn wieder. */
    if (argc > 4 && argv[4][0] == '1') re15_game_flag_set(5, 0x21, 1);
    /* wie room_common.c:208 / main.c:2470 — sonst ist re15_msg_is_choice() blind */
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt.messages, rdt.messages_size); }

    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, px, pz, 0);
    dump("nach scd_room_reenter (= Tuer-Eintritt)", px, pz);

    /* Weitere 30 Frames ohne Eingabe (kein Action-Druck!) */
    extern uint8_t g_aot_action_pressed;
    for (int f = 0; f < 30; f++) {
        g_aot_action_pressed = 0;
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, g_scd.cam_id);
    }
    dump("nach 30 Frames ohne Eingabe", px, pz);

    /* Argument 5 = "press": kompletter Nutzer-Ablauf.
     *   1) Spieler an den Schalter-AOT (slot 4, sce=4 -> flag(5,0x21)) stellen,
     *      EINMAL die Aktionstaste druecken  -> was passiert JETZT?
     *   2) Raum verlassen + wieder betreten (scd_room_reenter)  -> was passiert DANN? */
    if (argc > 5 && argv[5][0] == '1') {
        const re15_aot_t *sw = &g_aot.slots[4];
        pl->x = sw->x; pl->z = sw->z; pl->rot_y = 0;
        for (int f = 0; f < 3; f++) {
            g_aot_action_pressed = (f == 1);
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, g_scd.cam_id);
        }
        g_aot_action_pressed = 0;
        for (int f = 0; f < 60; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, g_scd.cam_id); }
        dump("1) SQUARE am Schalter gedrueckt (+60 Frames)", pl->x, pl->z);

        int prompt_am_schalter = g_scd.message_active;

        pl->x = px; pl->z = pz;
        scd_room_reenter(&rdt, px, pz, 0);
        dump("2) Raum verlassen + WIEDER BETRETEN", px, pz);

        /* REGRESSIONS-GATE (Nutzer-Report BUG 3). Zwei Haelften, beide muessen stimmen:
         *  (a) BEIM DRUECKEN muss die Rolltor-Frage kommen. Vorher tat der Schalter NICHTS, weil
         *      sein Gate in sub01 sitzt und der Port sub01 nur EINMAL beim Raum-Eintritt fuhr —
         *      das Original seedet Thread-Slot 1 in JEDEM Gameplay-Frame neu auf sub_scd[1]
         *      (FUN_8003f038 @0x8003f064/70/80/84, gerufen @0x8001cdec).
         *  (b) BEIM BETRETEN darf sie NICHT kommen und die Kontrolle muss beim Spieler bleiben.
         *      Vorher latchte Flag(5,0x21) dauerhaft, sub01s Gate `Ck(5,0x21)==1 && Ck(5,0)==0`
         *      war im Tuer-Frame sofort wahr und startete sub08 (Message_on(0) = blockierende
         *      YES/NO-Query) mit dem Spieler 31.400 Einheiten vom Schalter entfernt. Der Wisch
         *      von Bank-5-Wort-1 am Ende jedes VM-Laufs (FUN_8003ebf4 @0x8003ec1c aus
         *      FUN_8003f0a0 @0x8003f18c) macht daraus wieder den Ein-Frame-Handshake. */
        int bad = 0;
        if (!prompt_am_schalter) {
            printf("FAIL: SQUARE am Schalter loest die Rolltor-Frage NICHT aus\n"); bad = 1;
        }
        if (g_scd.message_active) {
            printf("FAIL: Rolltor-Frage erscheint beim BETRETEN (Spieler ist nicht am Schalter)\n");
            bad = 1;
        }
        if (g_scd.player_mode == 2) {
            printf("FAIL: player_mode bleibt 2 — Kontrolle kommt nicht zurueck\n"); bad = 1;
        }
        if (re15_game_flag_get(5, 0x21)) {
            printf("FAIL: Flag(5,0x21) latcht ueber den Frame hinaus (Bank-5-Wort-1-Wisch fehlt)\n");
            bad = 1;
        }
        if (bad) { printf("\nERGEBNIS: ROOM1040-Schalter DEFEKT\n"); return 1; }
        printf("\nERGEBNIS: Schalter reagiert am Schalter, kein Prompt beim Betreten, Kontrolle da\n");
    }
    return 0;
}
