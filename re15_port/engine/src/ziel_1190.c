/*
 * RE1.5 Rebuilt — Zielscheiben-Texte im Schiessstand ROOM1190/1191 (Runde 35 Spur H, Punkt 2).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_ziel1190.h und
 * analysis/befunde_runde35/H_raeume.md (Punkt 2). Kein Asset-Patch: die RDT bleibt byte-true; der
 * Port legt eine eigene Nachricht (Id 6/7) an, deren zweite Seite die unveraenderten Bytes der
 * Original-Nachricht des Platzes sind, und oeffnet sie an deren Stelle.
 */
#include "re15_ziel1190.h"

#include "re15_gameflow.h"   /* RE15_ROOM_BASE                                        */
#include "re15_room.h"       /* g_current_room_id                                     */
#include "re15_scd.h"        /* g_scd, scd_audio_queue_push                           */
#include "re15_aot.h"        /* g_aot                                                 */
#include "re15_msg.h"        /* re15_msg_install_text / _get_raw / re15_dialog_open_mask */
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

/* Scheiben-Plaetze: linke Kante x=-4400, Z-Kante je Slot (main00 Aot_set @0x02226/0x0223A/
 * 0x0224E/0x02262 Bytes 8..9: `9c 9b` -25700, `10 aa` -22000, `bc b7` -18500, `cc c5` -14900). */
static const int32_t k_kante_z[4] = { -25700, -22000, -18500, -14900 };

int re15_ziel1190_viele(int slot)
{
    if (slot < 0 || slot > 3) return -1;
    return (slot == 0 || slot == 2) ? 1 : 0;   /* ganz links + 3. von links (Kamera-Messung, .h) */
}

int re15_ziel1190_slot(void)
{
    if (RE15_ROOM_BASE(g_current_room_id) != RE15_ZIEL1190_RAUM) return -1;
    const int s = (int)g_scd.work_vars[0];        /* FORWARD-Stempel @0x80042f3c */
    if (s < 0 || s > 3) return -1;
    const re15_aot_t *a = &g_aot.slots[s];
    if (!a->active) return -1;
    if (a->x - a->half_w != RE15_ZIEL1190_KANTE_X) return -1;
    if (a->z - a->half_h != k_kante_z[s]) return -1;
    return s;
}

int re15_ziel1190_bauen(int slot, unsigned orig_msg, uint8_t *out, int cap)
{
    const int v = re15_ziel1190_viele(slot);
    if (v < 0 || !out) return 0;
    int olen = 0;
    const unsigned char *orig = re15_msg_get_raw((int)orig_msg, &olen);
    /* Original-Kopf `04 02` (@0x2E14 / @0x2E5E) muss da sein — sonst nichts anfassen. */
    if (!orig || olen < 3 || orig[0] != 0x04) return 0;
    const uint8_t *satz = v ? k_viele : k_wenige;
    const int n = v ? (int)sizeof k_viele : (int)sizeof k_wenige;
    const int total = 2 + n + 2 + (olen - 2);
    if (total > cap) return 0;
    int o = 0;
    out[o++] = orig[0]; out[o++] = orig[1];           /* Kopf der Original-Nachricht      */
    memcpy(out + o, satz, (size_t)n); o += n;          /* Seite 1: der Scheiben-Satz       */
    out[o++] = 0x02; out[o++] = 0x00;                  /* Seitenumbruch (msg 4 @0x2EAF-Form) */
    memcpy(out + o, orig + 2, (size_t)(olen - 2));     /* Seite 2: Original, Byte fuer Byte */
    o += olen - 2;
    return o;
}

/* Text unter der Port-Id ablegen + Stimme anmelden (wie leiche_1110_1230.c). Liefert die Id. */
static int ablegen(int slot, unsigned orig_msg)
{
    uint8_t buf[128];                                  /* PSX MSG_RAW_LEN = 128 (msg_common.c) */
    const int n = re15_ziel1190_bauen(slot, orig_msg, buf, (int)sizeof buf);
    if (n <= 0) return -1;
    const uint8_t id = re15_ziel1190_viele(slot) ? RE15_ZIEL1190_MSG_VIELE : RE15_ZIEL1190_MSG_WENIGE;
    re15_msg_install_text(id, buf, (size_t)n);
    {
        int d = re15_msg_compute_duration(buf, (size_t)n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
    }
    {   /* Stimme: synchro/STAGE1/room<Raum>/main<Id>.wav — fehlt sie, stumm mit Untertitel. */
        scd_audio_event_t vev;
        memset(&vev, 0, sizeof vev);
        vev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
        vev.sample_id = id;
        scd_audio_queue_push(&vev);
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[ziel1190] ROOM%04X Slot %d -> Port-Nachricht %d (%s) + Original msg %u\n",
            (unsigned)g_current_room_id, slot, (int)id,
            re15_ziel1190_viele(slot) ? "viele Einschuesse" : "wenige Einschuesse", orig_msg);
#endif
    return (int)id;
}

int re15_ziel1190_message_on(const uint8_t *pc, uint32_t pause_mask)
{
    if (!pc || pc[1] != RE15_ZIEL1190_MSG_SCHALTER) return 0;
    const int slot = re15_ziel1190_slot();
    if (slot < 0) return 0;
    /* Park-Semantik = der Ja/Nein-Zweig von op_message_on (scd_vm.c): erstes Betreten oeffnet
     * blockierend, danach parken bis das FSM fertig ist (Antwort steht dann in (12,31)). */
    if (g_scd.message_query == 0 && !g_scd.message_fsm_active) {
        const int id = ablegen(slot, RE15_ZIEL1190_MSG_SCHALTER);
        if (id < 0) return 0;                          /* Original-Weg bleibt */
        re15_dialog_open_mask(id, 1, pause_mask);
        g_scd.message_arg2 = pc[2];
        g_scd.message_arg3 = pc[3];
        return 2;
    }
    if (g_scd.message_active) return 2;
    g_scd.message_query = 0;
    return 1;
}

int re15_ziel1190_show(uint8_t index, uint32_t pause_mask)
{
    if (index != RE15_ZIEL1190_MSG_NICHTS) return 0;
    const int slot = re15_ziel1190_slot();
    if (slot < 0) return 0;
    const int id = ablegen(slot, RE15_ZIEL1190_MSG_NICHTS);
    if (id < 0) return 0;
    re15_dialog_open_mask(id, 0, pause_mask);          /* wie re15_scd_show_message: nicht blockierend */
    g_scd.message_arg2 = 0;
    g_scd.message_arg3 = 0;
    return 1;
}
