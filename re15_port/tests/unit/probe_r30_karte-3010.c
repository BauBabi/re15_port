/* probe_r30_karte-3010.c — MESSSONDE Runde 30, Thema B (Karte nach der Irons-Cutscene).
 *
 * KEIN RIEGEL, NUR MESSUNG (Schritt 1 des RE-Gates): was tut der Port HEUTE am Ende von
 * ROOM1150 sub08?  Die Sonde laedt die echte ROOM1150.RDT, startet sub08 als SCD-Thread
 * und tickt, bis der Thread stirbt. Je Tick liest sie
 *     - den Programmzeiger des sub08-Threads (Datei-Offset in der RDT),
 *     - die Szenen-Flags (1,27) und (2,7)  [sub08 @0x012E4 `22 02 07 00`, @0x012E8 `22 01 1b 00`],
 *     - den Menue-Zustand re15_menu_stage() (= DAT_800b5359) / re15_menu_is_open().
 * und meldet, ob NACH dem Evt_end @0x012EC irgendetwas den Statusschirm anfordert.
 *
 * ORIGINAL-Bytes des Skript-Schwanzes (ROOM1150.RDT, selbst gelesen, tools/scd_dump_room.py):
 *     @0x012E0  42            Plc_ret
 *     @0x012E1  00            Nop
 *     @0x012E2  3c 01         Cut_auto(1)
 *     @0x012E4  22 02 07 00   Set(2,7,0)
 *     @0x012E8  22 01 1b 00   Set(1,27,0)
 *     @0x012EC  01 00         Evt_end
 * RE2-Gegenstueck (ROOM30B0.RDT sub15, @0x01A86): ... Plc_ret / Set(2,7,0) / Set(1,27,0) /
 *     Cut_auto(1) / `84 01` (Kartenhinweis) / Evt_end — der Hinweis steht UNMITTELBAR vor dem
 *     Evt_end, nachdem die Szenen-Flags gefallen sind.
 *
 * Aufruf: probe_r30_karte-3010   (schreibt nach stdout; Exit 0 = Messung gelaufen)
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
#include "re15_menu.h"

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);

#define SUB08_BEG 0x1106u   /* ROOM1150.RDT sub08 Anfang (Sub-Tabelle @0x0EA0) */
#define SUB08_END 0x12F0u   /* Ende des sub-Blocks                              */
#define EVT_END   0x12ECu   /* `01 00` Evt_end von sub08                        */

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

int main(void)
{
    char rp[600];
    size_t sz = 0;
    static re15_rdt_t rdt;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1150.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt oder ist leer\n", rp); return 77; }
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* M0: die zitierten Bytes selbst nachlesen — stimmt der Anker? */
    {
        static const uint8_t soll[14] = { 0x42, 0x00, 0x3c, 0x01, 0x22, 0x02, 0x07, 0x00,
                                          0x22, 0x01, 0x1b, 0x00, 0x01, 0x00 };
        int ok = memcmp(raw + 0x12E0, soll, sizeof soll) == 0;
        printf("[M0] ROOM1150.RDT %lu B; Schwanz @0x012E0:", (unsigned long)sz);
        for (int i = 0; i < 14; i++) printf(" %02x", raw[0x12E0 + i]);
        printf("  -> %s\n", ok ? "wie zitiert" : "WEICHT AB");
        printf("[M0] sub08-Zeiger der RDT: Datei 0x%04lX (erwartet 0x%04X), sub_scd_count=%d\n",
               (long)(rdt.sub_scd[8] - raw), SUB08_BEG, rdt.sub_scd_count);
        /* Signatur-Eindeutigkeit im Raum: wie oft steht der 14-Byte-Schwanz in der Datei? */
        int n = 0;
        for (size_t o = 0; o + sizeof soll <= sz; o++)
            if (memcmp(raw + o, soll, sizeof soll) == 0) { n++; printf("[M0]   Treffer @0x%05lX\n", (unsigned long)o); }
        printf("[M0] 14-Byte-Schwanz in ROOM1150.RDT: %d Treffer\n", n);
    }

    re15_actor_init(); scd_vm_init(); re15_aot_init();
    g_current_room_id = 0x1150; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Debug-JUMP-Spawn des Raums (DEBUG.BIN @0x0285C: X=-17370 Z=-11900 Band 0) */
    pl->x = -17370; pl->y = 0; pl->z = -11900; pl->rot_y = 0;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) {
        scd_vm_tick(); re15_actors_anim_advance(); re15_actor_step_all_walkers();
        re15_menu_start_poll(0, 1); re15_menu_fsm_tick(0, 0);
    }
    printf("[M1] vor der Szene: flag(3,94)=%d flag(1,27)=%d flag(2,7)=%d menu_stage=%d offen=%d\n",
           re15_game_flag_get(3, 94), re15_game_flag_get(1, 27), re15_game_flag_get(2, 7),
           re15_menu_stage(), re15_menu_is_open());

    /* M2: sub08 starten (so wie es die AUTO-Zone Slot 6 tut: Evt_exec-Payload sub 8) */
    /* scd_thread_start liefert 0 = gestartet / -1 = Slot belegt (NICHT die Slot-Nummer). */
    int slot = 5;
    int rc = scd_thread_start(slot, rdt.sub_scd[8]);
    printf("[M2] sub08 gestartet in Thread-Slot %d (rc=%d)\n", slot, rc);
    if (rc != 0) { printf("FAIL: Slot %d belegt\n", slot); return 1; }
    long last_off = -1;
    int  ende_bild = -1, menue_bild = -1, flag_fall = -1;
    int  f;
    for (f = 0; f < 6000; f++) {
        scd_thread_t *t = &g_scd.threads[slot];
        long off = (t->active && t->pc) ? (long)(t->pc - raw) : -1;
        if (off >= (long)SUB08_BEG && off < (long)SUB08_END) last_off = off;
        if (f < 3 || (f % 200) == 0)
            printf("     Bild %4d  pc=0x%04lX  cine=%d  cam=%d  Spieler=(%ld,%ld)  stage=%d\n",
                   f, off, re15_cine_active(), (int)g_scd.cam_id, (long)pl->x, (long)pl->z,
                   re15_menu_stage());
        scd_vm_tick();
        re15_actors_anim_advance();
        re15_actor_step_all_walkers();
        re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
        re15_menu_start_poll(0, 1);
        re15_menu_fsm_tick(0, 0);
        if (flag_fall < 0 && f > 10 && !re15_cine_active()) flag_fall = f;
        if (menue_bild < 0 && (re15_menu_stage() != 0 || re15_menu_is_open())) menue_bild = f;
        if (ende_bild < 0 && !g_scd.threads[slot].active) { ende_bild = f; }
        if (ende_bild >= 0 && f > ende_bild + 120) break;   /* 120 Bilder Nachlauf */
    }
    printf("[M2] sub08-Thread beendet in Bild %d; letzter pc im sub08-Bereich = 0x%04lX "
           "(Evt_end steht @0x%04X)\n", ende_bild, last_off, EVT_END);
    printf("[M2] Szenen-Flags gefallen (cine==0) in Bild %d; flag(3,94)=%d\n",
           flag_fall, re15_game_flag_get(3, 94));
    printf("[M3] Statusschirm angefordert/geoeffnet waehrend Szene + 120 Bilder Nachlauf: %s "
           "(erstes Bild %d; menu_stage=%d offen=%d substate=%d)\n",
           menue_bild >= 0 ? "JA" : "NEIN", menue_bild,
           re15_menu_stage(), re15_menu_is_open(), re15_menu_substate());
    free(raw);
    return 0;
}
