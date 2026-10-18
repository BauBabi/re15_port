/*
 * RE1.5 Rebuilt — Runde 39 Spur C "inventar". Herleitung, Belege und alle Konstanten: include/re15_inventar_r39.h
 * und analysis/befunde_runde39/C_inventar.md.
 *   (1) Spind ROOM1000: erst Handgun-Munition (Satzform ROOM1001 @0x00C24), danach der Raumtext msg 0.
 *   (2) Gleiche Gegenstaende mit Zahl stapeln: Aufnahme (inventory_common.c), Kombinieren (menu_common.c
 *       exchange_match), Kiste (re15_itembox.c) — je ein Haken auf re15_inventar_r39_stapelt / _paar.
 *   (3) Status-Bild je Figur (inv_render_pc.c laedt ITEM/STPIC_<k>.TIM statt fest STPIC_00).
 */
#include "re15_inventar_r39.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_aot.h"
#include "re15_scd.h"         /* g_scd.work_vars, re15_game_flag_get */
#include "re15_inventory.h"   /* g_inv, re15_item_is_ammo, re15_inv_equipped_slot */
#include "re15_inv_ui.h"      /* RE15_INV_PTR: ausgelieferte EXE-Bytes [0x80074a8c,0x800762a0) */
#include "re15_itembox.h"     /* g_itembox (Messschiene) */
#include "re15_gameflow.h"    /* g_gameflow.character */
#include "re15_room.h"        /* g_current_room_id */
#include "re15_menu.h"        /* re15_menu_is_open (Messschiene) */
#include "re15_inv_screen.h"  /* g_inv_screen (Messschiene) */
#include "re15_item_discard.h" /* re15_discard_active (Messschiene) */

/* ================================== (1) SPIND ROOM1000 ========================================================== */

/* Den Satz aus `von` unveraendert nach `nach` tragen (alle parallelen Felder) — dieselbe Form wie itemorte_r38.c
 * satz_umziehen (Runde 38 K) / kartenitems_r36.c (Runde 36 N, Muenztelefon ROOM1030 Slot 8 -> 39). */
static void satz_umziehen(int von, int nach)
{
    g_aot.slots[nach]        = g_aot.slots[von];
    g_aot.door_params[nach]  = g_aot.door_params[von];
    g_aot.item_params[nach]  = g_aot.item_params[von];
    g_aot.flag_params[nach]  = g_aot.flag_params[von];
    g_aot.env_params[nach]   = g_aot.env_params[von];
    g_aot.stair_params[nach] = g_aot.stair_params[von];
    g_aot.slots[nach].was_inside = 0;
    g_aot.slots[von].active = 0;
    g_aot.slots[von].was_inside = 0;
}

/* Ist Slot s der Spind-Text (MESSAGE msg 0 auf dem Rechteck des Original-Satzes)? Genau das legt
 * feuerloescher_r37.c spind_umwidmen an (Satzform ROOM1000 @0x00CAA, Rechteck @0x00C24). */
static int ist_spind_text(int s)
{
    const re15_aot_t *a = &g_aot.slots[s];
    return a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_R39C_TEXT_MSG &&
           a->x == RE15_R39C_RECT_X + RE15_R39C_RECT_W / 2 && a->z == RE15_R39C_RECT_Z + RE15_R39C_RECT_D / 2 &&
           a->half_w == RE15_R39C_RECT_W / 2 && a->half_h == RE15_R39C_RECT_D / 2;
}

void re15_inventar_r39_install(uint16_t room_id)
{
    if (room_id != RE15_R39C_SPIND_RAUM) return;
    if (re15_game_flag_get(9, RE15_R39C_MUNITION_BIT)) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[inventar-r39] ROOM1000 Spind: Munition schon genommen (9,%d)=1 -> Slot %d Raumtext msg %d\n",
                RE15_R39C_MUNITION_BIT, RE15_R39C_SPIND_SLOT, RE15_R39C_TEXT_MSG);
#endif
        return;                                   /* der Text bleibt in Slot 3 (feuerloescher_r37.c) */
    }
    if (!ist_spind_text(RE15_R39C_SPIND_SLOT)) {
        /* Idempotenz: schon umgebaut (Munition in 3, Text in 11) -> nichts tun; sonst fremder Satz -> nicht anfassen. */
        return;
    }
    if (g_aot.slots[RE15_R39C_TEXT_SLOT].active) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[inventar-r39] ROOM1000 Spind: Slot %d belegt -> Spind unveraendert\n", RE15_R39C_TEXT_SLOT);
#endif
        return;
    }
    satz_umziehen(RE15_R39C_SPIND_SLOT, RE15_R39C_TEXT_SLOT);
    /* AUFNAHME in Satzform ROOM1001 @0x00C24 (op_item_aot_set: Mitte + halbe Ausdehnung, sat +3, Band +4). */
    re15_aot_set_item_tk_prop(RE15_R39C_SPIND_SLOT,
                              RE15_R39C_RECT_X + RE15_R39C_RECT_W / 2, RE15_R39C_RECT_Z + RE15_R39C_RECT_D / 2,
                              RE15_R39C_RECT_W / 2, RE15_R39C_RECT_D / 2,
                              RE15_R39C_MUNITION_ITEM, RE15_R39C_MUNITION_MENGE, RE15_R39C_MUNITION_BIT,
                              RE15_R39C_KEIN_MODELL);
    g_aot.slots[RE15_R39C_SPIND_SLOT].sce_flags = RE15_R39C_AOT_SAT;
    g_aot.slots[RE15_R39C_SPIND_SLOT].band      = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[inventar-r39] ROOM1000 Spind: Slot %d Aufnahme Item 0x%02X x%d Bit (9,%d) (Form ROOM1001 @0x00C24), "
                    "Raumtext msg %d -> Slot %d\n", RE15_R39C_SPIND_SLOT, RE15_R39C_MUNITION_ITEM,
            RE15_R39C_MUNITION_MENGE, RE15_R39C_MUNITION_BIT, RE15_R39C_TEXT_MSG, RE15_R39C_TEXT_SLOT);
#endif
}

/* ================================== (2) STAPELN ================================================================= */

static uint8_t obergrenze(uint8_t id)
{
    return *RE15_INV_PTR(0x80074DA8u + (uint32_t)id * 12u);       /* `lbu v1,0(at)` @0x8004e338 */
}

int re15_inventar_r39_stapelt(uint8_t id)
{
    if (id == 0 || id >= 0x22) return 0;          /* keine Zahl: `sltiu id,0x22` @0x80049124          */
    if (re15_item_is_ammo(id)) return 1;          /* 0x15..0x21 Munition + Memory Card                  */
    return obergrenze(id) == RE15_R39C_STAPEL_OBERGRENZE;   /* Waffen-Klasse: nur die Wurfgranaten 0x09..0x0B */
}

/* Hat die Id im ausgelieferten Satz schon den Selbst-Satz {id, id, 5, ..}? Dann entscheidet der Original-Satz. */
static int hat_selbst_satz(uint8_t id)
{
    const uint32_t row = 0x80074DA8u + (uint32_t)id * 12u;
    const uint8_t  n   = *RE15_INV_PTR(row + 9u);                  /* Anzahl @0x8004e9d8 */
    const unsigned char *p = RE15_INV_PTR(row + 4u);
    const uint32_t satz = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    for (uint8_t i = 0; i < n; i++) {
        const unsigned char *s = RE15_INV_PTR(satz + 4u * i);
        if (s[0] == id && s[2] == RE15_R39C_AKTION_SELBST) return 1;
    }
    return 0;
}

int re15_inventar_r39_paar(int a, int b, uint8_t *result, uint8_t *pic)
{
    if (a < 0 || b < 0 || a >= RE15_INV_MAX_SLOTS || b >= RE15_INV_MAX_SLOTS || a == b) return 0;
    const uint8_t id = g_inv.slots[a].id;
    if (id == 0 || g_inv.slots[b].id != id) return 0;
    if (!re15_inventar_r39_stapelt(id) || hat_selbst_satz(id)) return 0;
    /* Ausruestung: der Executor legt in den UNTEREN Platz zusammen (Aktion 5 -> Rumpf 2/3, @0x8004e670/@0x8004e678)
     * und leert den oberen, wenn die Summe passt (@0x8004e34c). War der obere ausgeruestet, zeigt sie danach auf den
     * zusammengelegten Stapel. */
    const int lo = a < b ? a : b, hi = a < b ? b : a;
    const int summe = (int)g_inv.slots[lo].qty + (int)g_inv.slots[hi].qty;
    if (re15_inv_equipped_slot() == hi && summe <= (int)obergrenze(id)) re15_inv_set_equipped_slot(lo);
    if (result) *result = id;
    if (pic)    *pic    = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[inventar-r39] Kombinieren: Item 0x%02X Platz %d + %d -> Aktion %d (Selbst-Stapeln, Form "
                    "`15 15 05 00` @0x80074CCC / RE2 `1e 05 1e ff` @0x800A9DB8), Obergrenze %d\n",
            (unsigned)id, a, b, RE15_R39C_AKTION_SELBST, (int)obergrenze(id));
#endif
    return RE15_R39C_AKTION_SELBST;
}

/* ================================== (3) STATUS-BILD ============================================================= */

int re15_inventar_r39_stpic_index(void)
{
    int k = (int)g_scd.work_vars[0x10];          /* = untere Nibble von DAT_800aca5c nach dem Raumlader-Merge
                                                  * @0x80039784 (Port: pc_sync_player_model, dasselbe Feld)     */
    if (k < 0 || k > 15) k = g_gameflow.character & 0x0F;
    return k;
}

void re15_inventar_r39_stpic_pfad(int index, char *out, int out_len)
{
    if (!out || out_len <= 0) return;
    if (index < 0 || index > 15) index = 0;
    snprintf(out, (size_t)out_len, "ITEM/STPIC_%02X.TIM", (unsigned)index);   /* Datei-Id 0x2C + index */
}

/* ================================== MESSSCHIENE ================================================================= */

void re15_inventar_r39_log_frame(unsigned frame)
{
#ifdef RE15_PLATFORM_PC
    static int s_an = -1;
    static re15_inv_slot_t s_inv[RE15_INV_MAX_SLOTS], s_box[RE15_BOX_SLOTS];
    static int s_eq = -2, s_raum = -1;
    static int s_m[12] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    if (s_an < 0) s_an = getenv("RE15_INV_LOG") ? 1 : 0;
    if (!s_an) return;
    {   /* Menue-, Nachrichten- und Wegwerf-Zustand — nur zur Einstellung der Eingabe-Bilder der Riegel.
         * fsm/sel = Dialog-Zustand (0 TYPING, 1 PAGE_WAIT, 2 PAGE_TIMED, 3 SELECT) / Ja-Nein sichtbar,
         * bereit = Wegwerf-Abfrage fertig getippt (re15_discard_ready). */
        const int m[12] = { re15_menu_is_open(), (int)g_inv_screen.substate, (int)g_inv_screen.item_state,
                            (int)g_inv_screen.item_cursor, (int)g_inv_screen.second_cursor,
                            (int)g_inv_screen.action_dir, (int)g_scd.message_active, (int)g_scd.message_id,
                            re15_discard_active(), (int)g_scd.message_fsm, (int)g_scd.message_select,
                            re15_discard_active() ? re15_discard_ready() : 0 };
        if (memcmp(m, s_m, sizeof m)) {
            memcpy(s_m, m, sizeof m);
            fprintf(stderr, "[inv-r39-menu] F%u offen=%d sub=%d st=%d cur=%d cur2=%d cmd=%d msg=%d/%d wegwerf=%d "
                            "fsm=%d sel=%d bereit=%d\n",
                    frame, m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11]);
        }
    }
    if (!memcmp(s_inv, g_inv.slots, sizeof s_inv) && !memcmp(s_box, g_itembox.slots, sizeof s_box) &&
        s_eq == re15_inv_equipped_slot() && s_raum == (int)g_current_room_id)
        return;
    memcpy(s_inv, g_inv.slots, sizeof s_inv);
    memcpy(s_box, g_itembox.slots, sizeof s_box);
    s_eq = re15_inv_equipped_slot();
    s_raum = (int)g_current_room_id;
    fprintf(stderr, "[inv-r39] F%u R%04X eq=%d inv=", frame, (unsigned)g_current_room_id, s_eq);
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        fprintf(stderr, "%s%02X:%u", i ? "," : "", (unsigned)g_inv.slots[i].id, (unsigned)g_inv.slots[i].qty);
    fprintf(stderr, " box=");
    int n = 0;
    for (int i = 0; i < RE15_BOX_SLOTS; i++)
        if (g_itembox.slots[i].id)
            fprintf(stderr, "%s%d:%02X:%u", n++ ? "," : "", i, (unsigned)g_itembox.slots[i].id,
                    (unsigned)g_itembox.slots[i].qty);
    fprintf(stderr, "%s\n", n ? "" : "-");
#else
    (void)frame;
#endif
}
