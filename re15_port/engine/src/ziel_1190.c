/*
 * RE1.5 Rebuilt — Zielscheiben-Texte im Schiessstand ROOM1190/1191 (Runde 36 Spur H; Erstbau Runde 35).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_ziel1190.h und
 * analysis/befunde_runde36/H_ziel1190.md (Punkt 1). Kein Asset-Patch: die RDT bleibt byte-true, die
 * Knoepfe laufen wieder ueber den Original-Weg. Der Port legt vier eigene sce-1-Text-Plaetze an, die an
 * ihrer Scheibe haengen (super-Anbindung des Original-Scans), und zwei eigene Nachrichten (Id 6/7).
 */
#include "re15_ziel1190.h"

#include "re15_gameflow.h"   /* RE15_ROOM_BASE                                        */
#include "re15_room.h"       /* g_current_room_id                                     */
#include "re15_scd.h"        /* g_scd (props, work_vars), scd_audio_queue_push        */
#include "re15_aot.h"        /* g_aot, re15_aot_set                                   */
#include "re15_msg.h"        /* re15_msg_install_text, re15_dialog_open_mask          */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* "This target has a surprisingly" 0x08 "large number of bullet holes." — NUTZER-VORGABE (Text).
 * Glyphen: re15_msg_glyph (msg_common.c); Zeilenumbruch 0x08 wie ROOM1190 msg 2 @0x2E79. */
static const uint8_t k_viele[] = {
    /* T    h    i    s   _  t    a    r    g    e    t   _  h    a    s   _  a   _ */
    0x30,0x44,0x45,0x4f,0x00,0x50,0x3d,0x4e,0x43,0x41,0x50,0x00,0x44,0x3d,0x4f,0x00,0x3d,0x00,
    /* s    u    r    p    r    i    s    i    n    g    l    y                          */
    0x4f,0x51,0x4e,0x4c,0x4e,0x45,0x4f,0x45,0x4a,0x43,0x48,0x55,
    0x08,
    /* l    a    r    g    e   _  n    u    m    b    e    r   _  o    f   _              */
    0x48,0x3d,0x4e,0x43,0x41,0x00,0x4a,0x51,0x49,0x3e,0x41,0x4e,0x00,0x4b,0x42,0x00,
    /* b    u    l    l    e    t   _  h    o    l    e    s    .                         */
    0x3e,0x51,0x48,0x48,0x41,0x50,0x00,0x44,0x4b,0x48,0x41,0x4f,0x57,
};

/* "This target does not have" 0x08 "many bullet holes." — NUTZER-VORGABE (Text, "." Gleichlauf). */
static const uint8_t k_wenige[] = {
    /* T    h    i    s   _  t    a    r    g    e    t   _                              */
    0x30,0x44,0x45,0x4f,0x00,0x50,0x3d,0x4e,0x43,0x41,0x50,0x00,
    /* d    o    e    s   _  n    o    t   _  h    a    v    e                          */
    0x40,0x4b,0x41,0x4f,0x00,0x4a,0x4b,0x50,0x00,0x44,0x3d,0x52,0x41,
    0x08,
    /* m    a    n    y   _  b    u    l    l    e    t   _  h    o    l    e    s    .   */
    0x49,0x3d,0x4a,0x55,0x00,0x3e,0x51,0x48,0x48,0x41,0x50,0x00,0x44,0x4b,0x48,0x41,0x4f,0x57,
};

int re15_ziel1190_viele(unsigned room_id, int obj)
{
    if (RE15_ROOM_BASE(room_id) != RE15_ZIEL1190_RAUM || obj < 0 || obj > 3) return -1;
    int v = (obj == 0 || obj == 2);            /* Leon: Loesung A = Obj 0+2 (sub01 @0x02438-0x02468) */
    if ((room_id & 0xFu) == 1u) v = !v;        /* Elza-RDT (@0x800397ec): Loesung B = Obj 1+3         */
    return v;
}

int re15_ziel1190_text(int viele, uint8_t *out, int cap)
{
    const uint8_t *satz = viele ? k_viele : k_wenige;
    const int n = viele ? (int)sizeof k_viele : (int)sizeof k_wenige;
    if (!out || cap < n + 4) return 0;
    int o = 0;
    out[o++] = 0x04; out[o++] = 0x02;          /* Kopf wie msg 2 @0x2E5E               */
    memcpy(out + o, satz, (size_t)n); o += n;
    out[o++] = 0x01; out[o++] = 0x00;          /* Ende wie msg 2 @0x2E7F               */
    return o;
}

/* Pooleintrag des Objekts obj (Obj_model_set-Nummer), -1 wenn es nicht (aktiv) im Pool steht. */
static int prop_von_obj(int obj)
{
    for (int k = 0; k < (int)g_scd.prop_count && k < RE15_SCD_MAX_PROPS; k++)
        if (g_scd.props[k].active && (int)g_scd.props[k].obj_id == obj) return k;
    return -1;
}

/* Ist Platz SLOT0+n einer unserer Text-Plaetze? (sce 1 mit unserer Id, unser Rechteck) */
static int ist_unser_platz(const re15_aot_t *a)
{
    return a->active && a->type == RE15_AOT_TYPE_MESSAGE &&
           (a->event_id == RE15_ZIEL1190_MSG_VIELE || a->event_id == RE15_ZIEL1190_MSG_WENIGE) &&
           a->half_w == RE15_ZIEL1190_HALB_X && a->half_h == RE15_ZIEL1190_HALB_Z;
}

void re15_ziel1190_install(uint16_t room_id)
{
    if (RE15_ROOM_BASE(room_id) != RE15_ZIEL1190_RAUM) return;
    for (int n = 0; n < 4; n++) {
        const int slot = RE15_ZIEL1190_SLOT0 + n;
        re15_aot_t *a = &g_aot.slots[slot];
        if (a->active && !ist_unser_platz(a)) continue;   /* nie einen Raum-Platz ueberschreiben */
        const int k = prop_von_obj(n);
        if (k < 0) continue;                              /* keine Scheibe -> kein Text-Platz     */
        const uint8_t id = re15_ziel1190_viele(room_id, n) ? RE15_ZIEL1190_MSG_VIELE
                                                           : RE15_ZIEL1190_MSG_WENIGE;
        /* Rechteck relativ zur Scheibe (super-Anbindung): Mitte = Objekt-x/z, +-300 / +-1200. */
        re15_aot_set(slot, RE15_AOT_TYPE_MESSAGE, id, g_scd.props[k].x, g_scd.props[k].z,
                     RE15_ZIEL1190_HALB_X, RE15_ZIEL1190_HALB_Z);
        a->sce_flags    = RE15_ZIEL1190_SAT;
        a->band         = RE15_ZIEL1190_BAND;
        a->pause_mask16 = RE15_ZIEL1190_PAUSE;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[ziel1190] ROOM%04X Scheibe %d -> Text-Platz %d (Nachricht %d, %s) bei (%d,%d)\n",
                (unsigned)room_id, n, slot, (int)id,
                id == RE15_ZIEL1190_MSG_VIELE ? "viele Einschuesse" : "wenige Einschuesse",
                (int)g_scd.props[k].x, (int)g_scd.props[k].z);
#endif
    }
}

void re15_ziel1190_folgen(void)
{
    if (RE15_ROOM_BASE(g_current_room_id) != RE15_ZIEL1190_RAUM) return;
    for (int n = 0; n < 4; n++) {
        re15_aot_t *a = &g_aot.slots[RE15_ZIEL1190_SLOT0 + n];
        if (!ist_unser_platz(a)) continue;
        const int k = prop_von_obj(n);
        if (k < 0) continue;
        /* Versatz = Objekt-x/z (@0x80042d7c / @0x80042db4), Ecke = Versatz + rec (@0x80042e60-84):
         * mit dem Rechteck relativ zur Scheibe ist die Mitte die Objektlage selbst. */
        a->x = g_scd.props[k].x;
        a->z = g_scd.props[k].z;
    }
}

int re15_ziel1190_show(uint8_t index, uint32_t pause_mask)
{
    if (RE15_ROOM_BASE(g_current_room_id) != RE15_ZIEL1190_RAUM) return 0;
    if (index != RE15_ZIEL1190_MSG_VIELE && index != RE15_ZIEL1190_MSG_WENIGE) return 0;
    uint8_t buf[64];
    const int n = re15_ziel1190_text(index == RE15_ZIEL1190_MSG_VIELE, buf, (int)sizeof buf);
    if (n <= 0) return 0;
    /* Text erst HIER ablegen (wie leiche_1110_1230.c): Tuer- und Ladeweg sind damit automatisch
     * abgedeckt, die Reihenfolge Teardown/Installer/Nachrichtenblock spielt keine Rolle. */
    re15_msg_install_text(index, buf, (size_t)n);
    {
        int d = re15_msg_compute_duration(buf, (size_t)n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(index, d);
    }
    {   /* Stimme: synchro/STAGE1/room<Raum>/main<Id>.wav — fehlt sie, stumm mit Untertitel. */
        scd_audio_event_t vev;
        memset(&vev, 0, sizeof vev);
        vev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
        vev.sample_id = index;
        scd_audio_queue_push(&vev);
    }
    /* Oeffnen wie der sce-1-Weg selbst (re15_scd_show_message: nicht blockierend, Maske des Platzes). */
    re15_dialog_open_mask((int)index, 0, pause_mask);
    g_scd.message_arg2 = 0;
    g_scd.message_arg3 = 0;
#ifdef RE15_PLATFORM_PC
    {   /* welcher Platz: der Vorwaerts-Treffer stempelt work_vars[0] = Platz (@0x80042f3c) */
        const int platz = (int)g_scd.work_vars[0];
        fprintf(stderr, "[ziel1190] ROOM%04X Scheibe %d angeklickt (Platz %d) -> Nachricht %d (%s)\n",
                (unsigned)g_current_room_id, platz - RE15_ZIEL1190_SLOT0, platz, (int)index,
                index == RE15_ZIEL1190_MSG_VIELE ? "viele Einschuesse" : "wenige Einschuesse");
    }
#endif
    return 1;
}
