/*
 * kleinkram_r38.c — Runde 38 Spur Q "kleinkram": Inventar-Ton (P1), Elza ROOM1151 Irons (P2), ROOM30E0
 * Flammenwerfer (P3). Belege und Konstanten: include/re15_kleinkram_r38.h, Dossier
 * analysis/befunde_runde38/Q_kleinkram.md.
 *
 * Form der Anker wie elza_r36.c / codepanel_r37.c: je Raum EINMAL Datei-Offset + 8-Byte-Signatur im rohen RDT-Puffer
 * pruefen; der VM-Takt meldet jeden Opcode (scd_vm_tick), dieser Haken entscheidet vor dem Opcode-Sprung.
 * Stimmt eine Signatur nicht (anderer Stand), bleibt der Anker unscharf — lieber Original als falsch.
 */
#include "re15_kleinkram_r38.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_scd.h"          /* scd_thread_t, g_scd, scd_audio_queue_push, re15_game_flag_get */
#include "re15_aot.h"          /* g_aot, re15_aot_retype, RE15_AOT_TYPE_* */
#include "re15_msg.h"          /* re15_msg_install_text, re15_dialog_open_mask */
#include "re15_item_modal.h"   /* re15_item_modal_start / _active (FUN_8001db28-Port) */
#include "re15_inventory.h"    /* g_inv */
#include "re15_audio.h"        /* re15_audio_core_se (Bank 4 = CORE) */
#include "re15_elza_r36.h"     /* re15_elza_r36_meldung(0) = Text 20, RE15_ELZA_IRONS_* (Runde 36) */
#include "re15_engine.h"       /* g_engine.frame_count — nur fuer die Messzeilen */
#include "re15_room.h"         /* g_current_room_id */

int g_re15_r38q_anker_n = 0;

static int s_zaehler[RE15_R38Q_AKT_N];
static int s_ton_zaehler[16];

static void messzeile(const char *fmt, ...)
{
    /* stderr = debug.log im Arbeitsordner (main.c freopen); in Unit-Tests die Konsole. */
    va_list ap;
    fprintf(stderr, "[r38q] F%u ", (unsigned)g_engine.frame_count);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}

/* ======================================== P1 Inventar-Ton ======================================================= */

void re15_kleinkram_r38_inv_ton(int satz, const char *wo)
{
    if (satz >= 0 && satz < 16) s_ton_zaehler[satz]++;
    re15_audio_core_se(satz);                       /* Se_on(0x04<<24 | satz<<16, 0) — RE2 @0x80026530 / @0x8006a934 */
    fprintf(stderr, "[r38q] F%u Inventar-Ton Satz %d (%s)\n", (unsigned)g_engine.frame_count, satz, wo ? wo : "?");
}

int re15_kleinkram_r38_inv_ton_zaehler(int satz)
{
    return (satz >= 0 && satz < 16) ? s_ton_zaehler[satz] : 0;
}

/* ======================================== Anker P2/P3 =========================================================== */

typedef struct {
    uint16_t raum;
    uint32_t off;          /* RDT-Datei-Offset des Anker-Opcodes                    */
    uint8_t  sig[8];       /* erwartete Bytes ab off (Anker-Opcode + Folge-Opcode)  */
    uint8_t  aktion;
} anker_t;

enum { A_IRONS_TEXT = 1, A_IRONS_GESTE, A_IRONS_ENDE, A_FW_TEXT, A_FW_BIT };

static const anker_t k_anker[] = {
    /* P2 ROOM1151 sub03 (@0x00EC2..@0x00F72, Dump analysis Abschnitt 2.2):
     *   @0x00EFA `2b 02 00 00` Message_on 2 (Maske 0)      + @0x00EFE `2e 02 00` Work_set(2,0) `00` */
    { RE15_R38Q_RAUM_1151, 0x00EFAu, { 0x2b,0x02,0x00,0x00, 0x2e,0x02,0x00,0x00 }, A_IRONS_TEXT  },
    /*   @0x00F02 `3f 00 06 00` Irons Plc_motion(0,6)      + @0x00F06 `09 0a 32 00` Sleep 50 */
    { RE15_R38Q_RAUM_1151, 0x00F02u, { 0x3f,0x00,0x06,0x00, 0x09,0x0a,0x32,0x00 }, A_IRONS_GESTE },
    /*   @0x00F0A `3f 00 06 00` Irons Plc_motion(0,6)      + @0x00F0E `09 0a 32 00` Sleep 50 */
    { RE15_R38Q_RAUM_1151, 0x00F0Au, { 0x3f,0x00,0x06,0x00, 0x09,0x0a,0x32,0x00 }, A_IRONS_GESTE },
    /*   @0x00F12 `3f 00 03 00` Irons Plc_motion(0,3)      + @0x00F16 `43 00 04 00` Plc_flg 4 */
    { RE15_R38Q_RAUM_1151, 0x00F12u, { 0x3f,0x00,0x03,0x00, 0x43,0x00,0x04,0x00 }, A_IRONS_GESTE },
    /*   @0x00F16 `43 00 04 00` Irons Plc_flg 4 (Atem-Loop)  + @0x00F1A `09 0a 14 00` Sleep 20 */
    { RE15_R38Q_RAUM_1151, 0x00F16u, { 0x43,0x00,0x04,0x00, 0x09,0x0a,0x14,0x00 }, A_IRONS_GESTE },
    /*   @0x00F72 `01 00` Evt_end sub03                     + @0x00F74 `22 02 00 01 22 02` = Anfang sub04 */
    { RE15_R38Q_RAUM_1151, 0x00F72u, { 0x01,0x00,0x22,0x02,0x00,0x01,0x22,0x02 }, A_IRONS_ENDE  },
    /* P3 ROOM30E0 sub04 (@0x00FC0..@0x01006):
     *   @0x00FEE `2b 0f ff ff` Message_on 15 (Maske 0xffff) + @0x00FF2 `02 00` Evt_next + @0x00FF4 `29 02` Cut_chg 2 */
    { RE15_R38Q_RAUM_30E0, 0x00FEEu, { 0x2b,0x0f,0xff,0xff, 0x02,0x00,0x29,0x02 }, A_FW_TEXT     },
    /*   @0x00FFA `22 03 9c 01` Set(3,0x9C)                 + @0x00FFE `22 02 07 00` Set(2,7,0) */
    { RE15_R38Q_RAUM_30E0, 0x00FFAu, { 0x22,0x03,0x9c,0x01, 0x22,0x02,0x07,0x00 }, A_FW_BIT      },
};
#define ANKER_N ((int)(sizeof k_anker / sizeof k_anker[0]))

static const unsigned char *s_anker_pc[ANKER_N];
static int                  s_anker_idx[ANKER_N];

/* P3: Zustand der Aufnahme an @0x00FEE (0 = keine, 1 = Modal laeuft/ausgewertet beim naechsten Halt). */
static int s_fw_phase = 0;
static int s_fw_vorher = 0;          /* Flammenwerfer-Zellen vor dem Modal */
static int s_fw_bit_ueber = 0;       /* 1 = Set(3,0x9C) @0x00FFA auslassen (nicht genommen) */

void re15_kleinkram_r38_reset(void)
{
    memset(s_zaehler, 0, sizeof s_zaehler);
    memset(s_ton_zaehler, 0, sizeof s_ton_zaehler);
    s_fw_phase = 0; s_fw_vorher = 0; s_fw_bit_ueber = 0;
}

int re15_kleinkram_r38_zaehler(int aktion)
{
    return (aktion > 0 && aktion < RE15_R38Q_AKT_N) ? s_zaehler[aktion] : 0;
}

void re15_kleinkram_r38_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_r38q_anker_n = 0;
    s_fw_phase = 0; s_fw_bit_ueber = 0;
    if (!raw || raw_size <= 0) return;
    for (int i = 0; i < ANKER_N; i++) {
        const anker_t *a = &k_anker[i];
        if (a->raum != (uint16_t)room_id) continue;
        if ((long)a->off + 8 > (long)raw_size) continue;
        if (memcmp(raw + a->off, a->sig, 8) != 0) {
            messzeile("ROOM%04X Anker @0x%05X: Signatur weicht ab - nicht armiert", room_id, (unsigned)a->off);
            continue;
        }
        s_anker_pc[g_re15_r38q_anker_n]  = raw + a->off;
        s_anker_idx[g_re15_r38q_anker_n] = i;
        g_re15_r38q_anker_n++;
    }
    if (g_re15_r38q_anker_n)
        messzeile("ROOM%04X: %d Anker armiert", room_id, g_re15_r38q_anker_n);
}

static int flammenwerfer_zellen(void)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == RE15_R38Q_FW_ITEM) n++;
    return n;
}

/* P2: Text 20 statt msg 2, Form des Klartext-Zweigs von op_message_on (scd_vm.c: Stimme einreihen,
 * re15_dialog_open_mask nicht blockierend, message_arg2/3 = pc[2]/pc[3]) — Muster leiche_1110_1230.c /
 * elza_r36.c Aktion Stoehnen. Maske = die des ersetzten Message_on (`2b 02 00 00` -> 0). */
static void irons_text(const scd_thread_t *t)
{
    int n = 0;
    const uint8_t *txt = re15_elza_r36_meldung(0, &n);       /* "He has already passed..." (Runde 36) */
    re15_msg_install_text((unsigned char)RE15_ELZA_IRONS_MSG_ID, txt, (size_t)n);
    {
        int d = re15_msg_compute_duration(txt, (size_t)n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)RE15_ELZA_IRONS_MSG_ID, d);
    }
    scd_audio_event_t vev;
    memset(&vev, 0, sizeof vev);
    vev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
    vev.sample_id = RE15_ELZA_IRONS_MSG_ID;
    scd_audio_queue_push(&vev);
    uint32_t maske = ((uint32_t)(t->pc[2] | ((uint32_t)t->pc[3] << 8))) << 16;   /* @0x80040508 / @0x8004051c */
    re15_dialog_open_mask((int)RE15_ELZA_IRONS_MSG_ID, 0, maske);
    g_scd.message_arg2 = t->pc[2];
    g_scd.message_arg3 = t->pc[3];
}

int re15_kleinkram_r38_pc(void *thread)
{
    scd_thread_t *t = (scd_thread_t *)thread;
    if (!t || !t->pc) return 0;
    for (int k = 0; k < g_re15_r38q_anker_n; k++) {
        if (s_anker_pc[k] != t->pc) continue;
        const anker_t *a = &k_anker[s_anker_idx[k]];
        switch (a->aktion) {
        case A_IRONS_TEXT:
            irons_text(t);
            s_zaehler[RE15_R38Q_AKT_IRONS_TEXT]++;
            messzeile("ROOM1151 sub03 @0x%05X Message_on %d -> Text %d", (unsigned)a->off, RE15_R38Q_1151_MSG_ORIG,
                      RE15_ELZA_IRONS_MSG_ID);
            t->pc += 4;
            return 1;
        case A_IRONS_GESTE:
            s_zaehler[RE15_R38Q_AKT_IRONS_GESTE]++;
            messzeile("ROOM1151 sub03 @0x%05X Irons-Geste %02x entfaellt (Totenpose)", (unsigned)a->off,
                      (unsigned)a->sig[0]);
            t->pc += 4;
            return 1;
        case A_IRONS_ENDE:
            if (g_aot.slots[RE15_ELZA_IRONS_SLOT].active &&
                g_aot.slots[RE15_ELZA_IRONS_SLOT].type == RE15_AOT_TYPE_EXAMINE_WORKVAR)
                re15_aot_retype(RE15_ELZA_IRONS_SLOT, RE15_ELZA_IRONS_SCE_TEXT, RE15_ELZA_IRONS_FLAGS,
                                RE15_ELZA_IRONS_MSG_ID, RE15_ELZA_IRONS_MASKE, 0);
            s_zaehler[RE15_R38Q_AKT_IRONS_ENDE]++;
            messzeile("ROOM1151 sub03 Ende: Slot %d -> Text-Platz %d, (3,0x9D)=%d", RE15_ELZA_IRONS_SLOT,
                      RE15_ELZA_IRONS_MSG_ID,
                      re15_game_flag_get(RE15_R38Q_1151_GESPRAECH_BANK, RE15_R38Q_1151_GESPRAECH_BIT));
            return 0;                                   /* Evt_end normal */
        case A_FW_TEXT:
            if (s_fw_phase == 0) {
                if (re15_item_modal_active()) return 2;   /* Original-Handler startet nur aus Zustand 0 (@0x80043334) */
                s_fw_vorher = flammenwerfer_zellen();
                re15_item_modal_start(RE15_R38Q_FW_ITEM, RE15_R38Q_FW_MENGE, 0, -1, 0xFF);
                s_fw_phase = 1;
                s_zaehler[RE15_R38Q_AKT_FW_MODAL]++;
                messzeile("ROOM30E0 sub04 @0x%05X Message_on 15 -> Aufnahme-Modal Item 0x%02X x%d", (unsigned)a->off,
                          RE15_R38Q_FW_ITEM, RE15_R38Q_FW_MENGE);
                return 2;                               /* parken, bis das Modal zu ist (VM steht waehrenddessen) */
            }
            if (re15_item_modal_active()) return 2;
            {
                int nachher = flammenwerfer_zellen();
                s_fw_phase = 0;
                if (nachher > s_fw_vorher) {
                    s_fw_bit_ueber = 0;
                    s_zaehler[RE15_R38Q_AKT_FW_GENOMMEN]++;
                    messzeile("ROOM30E0 Flammenwerfer genommen: Zellen %d -> %d", s_fw_vorher, nachher);
                } else {
                    /* "No" oder voll: liegt weiter im Spind -> Panel wieder wie main00 @0x00A5A, Bit bleibt 0. */
                    s_fw_bit_ueber = 1;
                    re15_aot_retype(RE15_R38Q_FW_PANEL_SLOT, RE15_R38Q_FW_PANEL_SCE, RE15_R38Q_FW_PANEL_FLAGS,
                                    RE15_R38Q_FW_PANEL_P0, RE15_R38Q_FW_PANEL_P1, 0);
                    s_zaehler[RE15_R38Q_AKT_FW_LIEGT]++;
                    messzeile("ROOM30E0 Flammenwerfer NICHT genommen (No/voll): Panel Slot %d wieder scharf, "
                              "Zellen %d", RE15_R38Q_FW_PANEL_SLOT, nachher);
                }
            }
            t->pc += 4;                                 /* weiter mit Evt_next @0x00FF2 wie nach dem Text */
            return 1;
        case A_FW_BIT:
            if (!s_fw_bit_ueber) return 0;              /* genommen: Set(3,0x9C) wie im Original */
            s_fw_bit_ueber = 0;
            s_zaehler[RE15_R38Q_AKT_FW_BIT_UEBER]++;
            messzeile("ROOM30E0 sub04 @0x%05X Set(3,0x%02X) entfaellt (nicht genommen)", (unsigned)a->off,
                      RE15_R38Q_FW_DONE_BIT);
            t->pc += 4;
            return 1;
        default:
            return 0;
        }
    }
    return 0;
}
