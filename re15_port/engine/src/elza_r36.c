/*
 * elza_r36.c — Runde 36 Spur L "elza": Irons tot in ROOM1151 (P2) und die Toene/Stimme der Elza-Szenen
 * ROOM1211 (P3 Unlock), ROOM1021 (P4 Schuesse, P5 Roys Sterbe-Ton), ROOM1011 (P6 main16.5 + Untertitel).
 * Herleitung, Belege und Port-Wahlen: include/re15_elza_r36.h und analysis/befunde_runde36/L_elza.md.
 */
#include "re15_elza_r36.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_scd.h"
#include "re15_engine.h"           /* g_engine.frame_count — nur fuer das Datei-Log */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned g_current_room_id;
extern void re15_audio_room_se(int se_id);   /* FUN_800453d0-Pfad (snd1-Raumbank) */

void (*re15_elza_se_hook)(int ton) = NULL;
int g_re15_elza_anker_n = 0;

/* ---- Messschiene (die GUI-exe hat kein stderr): RE15_ELZA_LOG=<pfad> ------------------------------------- */
static void logf_(const char *fmt, unsigned a, unsigned b, unsigned c, unsigned d)
{
    static int s_init = 0; static const char *s_pfad = NULL;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_ELZA_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "a");
    if (!f) return;
    fprintf(f, "F%u ", (unsigned)g_engine.frame_count);
    fprintf(f, fmt, a, b, c, d);
    fputc('\n', f);
    fclose(f);
}

/* ======================================== Texte ============================================================ */

/* "He has already passed..." — ⛔ NEUER TEXT (NUTZER-VORGABE, woertlich aus AUFTRAG.md). Nur die FORM ist belegt:
 *   Kopf `04 02` und Ende `01 00` wie jeder ausgelieferte Untersuchen-Text, z.B. ROOM1021 msg 9 @0x024FB
 *   (`04 02 24 41 3a 4f 00 40 41 3d 40 57 02 00 2c ... 57 57 57 01 00` = "He's dead.Poor Roy...").
 * Jedes Wort liegt so im Auslieferungsstand vor (Glyphen = Tabelle msg_common.c re15_msg_glyph):
 *   "He has"    ROOM1011.RDT @0x01233 (msg 18 "He has passed out ...")  24 41 00 44 3d 4f
 *   " already"  ROOM10D0.RDT @0x02226                                    00 3d 48 4e 41 3d 40 55
 *   " passed"   ROOM1011.RDT @0x01239 (msg 18)                           00 4c 3d 4f 4f 41 40
 *   "..."       ROOM1011.RDT @0x012BA (msg 19 "He is holding something...") 57 57 57
 * Eine Zeile (6 Woerter, kuerzer als ROOM1011 msg 19), kein Umbruch noetig. */
static const uint8_t k_irons_text[] = {
    0x04, 0x02,
    /* H    e   _    h    a    s  */
    0x24,0x41,0x00,0x44,0x3d,0x4f,
    /* _    a    l    r    e    a    d    y  */
    0x00,0x3d,0x48,0x4e,0x41,0x3d,0x40,0x55,
    /* _    p    a    s    s    e    d  */
    0x00,0x4c,0x3d,0x4f,0x4f,0x41,0x40,
    /* .    .    .  */
    0x57,0x57,0x57,
    0x01, 0x00,
};

/* "Roy: groans" — ROOM1011 msg 10 @0x010C9..0x010DF (23 Byte bis zum Beginn von msg 11 @0x010E0) WOERTLICH:
 * RE1.5 untertitelt Roys Stoehnen selbst so (sub02 @0x00CAE Message_on 10). Gemessen: main16.5.wav ist EIN
 * stimmhafter Ausbruch (0,6 s, Nulldurchgang 0,01-0,03) wie main00 "Officer: groans" / main10 "Roy: groans". */
static const uint8_t k_stoehnen_text[] = {
    0x04,0x00,0x05,0x02,0x2e,0x4b,0x55,0x16,0x05,0x00,0x00,0x5f,0x43,0x4e,0x4b,0x3d,0x4a,0x4f,0x5f,0x04,0x01,0x01,0x63,
};

const uint8_t *re15_elza_r36_meldung(int welche, int *out_len)
{
    if (welche == 0) { if (out_len) *out_len = (int)sizeof k_irons_text;    return k_irons_text; }
    if (out_len) *out_len = (int)sizeof k_stoehnen_text;
    return k_stoehnen_text;
}

static void text_einsetzen(uint8_t id, const uint8_t *t, size_t n)
{
    re15_msg_install_text(id, t, n);
    int d = re15_msg_compute_duration(t, n, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

/* ======================================== Anker (P3-P6) ==================================================== */

typedef struct {
    uint16_t raum;
    uint32_t off;          /* RDT-Datei-Offset des Anker-Opcodes                                 */
    uint8_t  sig[8];       /* erwartete Bytes ab off (Anker-Opcode + Folge-Opcode)               */
    uint8_t  aktion;
    uint8_t  verzug;       /* Bilder (VM-Takte) zwischen Anker und Aktion — Form der RE2-Vorlage */
} anker_t;

static const anker_t k_anker[] = {
    /* P3 ROOM1211 sub02 @0x020F8 `3f 00 00 00` Elza Plc_motion(0,0,0) = Aufschliess-Geste (RBJ rec0 Clip 0,
     * 20 Bilder), Folge @0x020FC `09 0a 3c 00` Sleep 60. Verzug 20 = RE2 ROOM1010 sub09: @0x01C18 `3f 00 00 00`
     * (Geste, ebenfalls 20 Bilder), @0x01C1C `09 0a 14 00` Sleep 20, @0x01C20 Se_on(2,0x0A). */
    { RE15_ELZA_RAUM_1211, 0x020F8u, { 0x3f,0x00,0x00,0x00, 0x09,0x0a,0x3c,0x00 }, RE15_ELZA_AKT_AUF,     20 },
    /* P4 ROOM1021 sub02 @0x020CE / @0x020D6 `3f 02 07 00` John Plc_motion(2,7) = Feuer-Clip (eigene Bank Clip 7,
     * 16 Bilder; Rueckstoss im Bild ab Clip-Bild 2). Verzug 1 = RE2 ROOM1010 sub30 @0x02676 Evt_exec sub31 +
     * @0x0267A Plc_motion(0,12,0) im selben Bild, sub31 @0x02680 `02 00` Evt_next, @0x0268E Se_on(2,0x0C). */
    { RE15_ELZA_RAUM_1021, 0x020CEu, { 0x3f,0x02,0x07,0x00, 0x09,0x0a,0x1e,0x00 }, RE15_ELZA_AKT_SCHUSS,   1 },
    { RE15_ELZA_RAUM_1021, 0x020D6u, { 0x3f,0x02,0x07,0x00, 0x2e,0x02,0x01,0x00 }, RE15_ELZA_AKT_SCHUSS,   1 },
    /* P5 ROOM1021 sub02 @0x020DE `3f 01 07 00` Roy Plc_motion(1,7) = Roys Todes-Clip, im Bild von Schuss 2.
     * Der Treffer kommt mit dem Schuss: Sterbe-Ton im Takt des Schuss-Tons (Verzug 1), hinter ihm eingereiht
     * (Ursache vor Wirkung). Ton = RE1.5-Zombie-Tod im Stehen FUN_80106c18 Phase 0 @0x80106da8/ac SE 5. */
    { RE15_ELZA_RAUM_1021, 0x020DEu, { 0x3f,0x01,0x07,0x00, 0x09,0x0a,0x64,0x00 }, RE15_ELZA_AKT_ROY_TOD,  1 },
    /* P6 ROOM1011 sub02 @0x00D54 `3f 00 09 00` Roy Plc_motion(0,9) = Roy sackt zusammen (direkt nach Cut_chg 1
     * @0x00D52, zwischen msg 16 @0x00D3A und msg 17 @0x00D5C). Verzug 0: die Zeile steht WIE eine Message_on
     * vor dem Opcode (Roys eigene Zeilen: `2b 10 00 00` @0x00D3A = Maske 0). */
    { RE15_ELZA_RAUM_1011, 0x00D54u, { 0x3f,0x00,0x09,0x00, 0x09,0x0a,0x64,0x00 }, RE15_ELZA_AKT_STOEHNEN, 0 },
};
#define ANKER_N ((int)(sizeof k_anker / sizeof k_anker[0]))

static const unsigned char *s_anker_pc[ANKER_N];
static int                  s_anker_idx[ANKER_N];

typedef struct { uint8_t aktion; uint8_t rest; uint16_t anker; } offen_t;
static offen_t s_offen[8];
static int     s_offen_n = 0;

static int      s_zaehler[RE15_ELZA_AKT_N];
static int      s_takt[RE15_ELZA_AKT_N];

int re15_elza_r36_zaehler(int aktion)      { return (aktion > 0 && aktion < RE15_ELZA_AKT_N) ? s_zaehler[aktion] : 0; }
int re15_elza_r36_letzter_takt(int aktion) { return (aktion > 0 && aktion < RE15_ELZA_AKT_N) ? s_takt[aktion] : -1; }
unsigned re15_elza_r36_anker_off(int i)
{
    return (i >= 0 && i < g_re15_elza_anker_n) ? k_anker[s_anker_idx[i]].off : 0u;
}

void re15_elza_r36_reset(void)
{
    memset(s_zaehler, 0, sizeof s_zaehler);
    for (int i = 0; i < RE15_ELZA_AKT_N; i++) s_takt[i] = -1;
    s_offen_n = 0;
}

void re15_elza_r36_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_elza_anker_n = 0;
    s_offen_n = 0;
    if (!raw || raw_size <= 0) return;
    for (int i = 0; i < ANKER_N; i++) {
        const anker_t *a = &k_anker[i];
        if (a->raum != (uint16_t)room_id) continue;
        if ((long)a->off + 8 > (long)raw_size) continue;
        if (memcmp(raw + a->off, a->sig, 8) != 0) {
            logf_("[elza] ROOM%04X Anker @0x%05X: Signatur weicht ab - nicht armiert %u %u",
                  room_id, a->off, 0, 0);
            continue;                               /* falscher Stand/Patch: lieber stumm als falsch */
        }
        s_anker_pc[g_re15_elza_anker_n]  = raw + a->off;
        s_anker_idx[g_re15_elza_anker_n] = i;
        g_re15_elza_anker_n++;
    }
    if (g_re15_elza_anker_n)
        logf_("[elza] ROOM%04X: %u Anker armiert %u %u", room_id, (unsigned)g_re15_elza_anker_n, 0, 0);
}

static void aktion_ausfuehren(int aktion, unsigned anker_off)
{
    switch (aktion) {
    case RE15_ELZA_AKT_AUF:
        if (re15_elza_se_hook) re15_elza_se_hook(RE15_ELZA_TON_AUF);
        break;
    case RE15_ELZA_AKT_SCHUSS:
        if (re15_elza_se_hook) re15_elza_se_hook(RE15_ELZA_TON_SCHUSS);
        break;
    case RE15_ELZA_AKT_ROY_TOD:
        re15_audio_room_se(RE15_ELZA_ROY_SE);       /* @0x80106da8 `ori a0,zero,0x5` / @0x80106dac jal 0x800453d0 */
        break;
    case RE15_ELZA_AKT_STOEHNEN: {
        /* Wie der Klartext-Zweig von op_message_on (scd_vm.c): Text unter der Port-Id ablegen, Stimme
         * (SCD_AUDIO_VOICE_ON, Raum + Id -> re15_elza_stimme_datei) einreihen, nicht blockierend oeffnen
         * mit der Maske von Roys Zeilen (`2b xx 00 00` = 0). Muster leiche_1110_1230.c. */
        text_einsetzen(RE15_ELZA_STOEHNEN_MSG_ID, k_stoehnen_text, sizeof k_stoehnen_text);
        scd_audio_event_t vev;
        memset(&vev, 0, sizeof vev);
        vev.kind      = (uint8_t)SCD_AUDIO_VOICE_ON;
        vev.sample_id = RE15_ELZA_STOEHNEN_MSG_ID;
        scd_audio_queue_push(&vev);
        re15_dialog_open_mask((int)RE15_ELZA_STOEHNEN_MSG_ID, 0, 0u);
        g_scd.message_arg2 = 0;
        g_scd.message_arg3 = 0;
        break;
    }
    default: return;
    }
    s_zaehler[aktion]++;
    s_takt[aktion] = (int)g_scd.tick_count;
    logf_("[elza] ROOM%04X Anker @0x%05X -> Aktion %u (Takt %u)", (unsigned)g_current_room_id, anker_off,
          (unsigned)aktion, (unsigned)g_scd.tick_count);
}

void re15_elza_r36_pc(const unsigned char *pc)
{
    for (int k = 0; k < g_re15_elza_anker_n; k++) {
        if (s_anker_pc[k] != pc) continue;
        const anker_t *a = &k_anker[s_anker_idx[k]];
        if (a->verzug == 0) { aktion_ausfuehren(a->aktion, a->off); return; }
        if (s_offen_n < (int)(sizeof s_offen / sizeof s_offen[0])) {
            s_offen[s_offen_n].aktion = a->aktion;
            s_offen[s_offen_n].rest   = a->verzug;
            s_offen[s_offen_n].anker  = (uint16_t)a->off;
            s_offen_n++;
        }
        return;
    }
}

void re15_elza_r36_tick(void)
{
    if (!s_offen_n) return;
    int n = 0;
    for (int i = 0; i < s_offen_n; i++) {
        offen_t o = s_offen[i];
        if (o.rest > 0) o.rest--;
        if (o.rest == 0) aktion_ausfuehren(o.aktion, o.anker);
        else s_offen[n++] = o;
    }
    s_offen_n = n;
}

/* ======================================== Stimme (P6) ====================================================== */

const char *re15_elza_stimme_datei(unsigned room_id, int msg_id)
{
    /* Nutzer-Datei synchro/STAGE1/room1011/main16.5.wav (nicht umbenannt/verschoben, VERTRAG §1.2). */
    if (room_id == RE15_ELZA_RAUM_1011 && msg_id == RE15_ELZA_STOEHNEN_MSG_ID) return RE15_ELZA_STOEHNEN_DATEI;
    return NULL;
}

/* ======================================== Irons tot (P2) =================================================== */

static re15_actor_t *irons_1151(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *a = &g_actors[s];
        if (a->active && a->type == RE15_ELZA_IRONS_TYP) return a;
    }
    return NULL;
}

void re15_elza_r36_install(uint16_t room_id)
{
    if (room_id != RE15_ELZA_RAUM_1151) return;

    /* (1) Totenpose — dieselben Feldwerte wie irons_tod_1150.c irons_pose(2, 89, gehalten) (Runde 35 Spur L):
     *     Motion-Sub 0 (+0x5 = 0), Zustand 4 (Executor @0x80041bb0), Phase 2 = Halt (@0x80050da4 `sb 2,6`),
     *     +0x94 = Clip 2, +0x95 = Bild 89, +0x1c4 = 0 (kein Loop-Bit 0x04 -> kein Atem-Loop wie sub02 @0x00EB8
     *     `3f 00 03 00` + @0x00EBC `43 00 04 00`), +0x8f = 0 (kein Ueberblenden). Der Installer laeuft NACH dem
     *     Init-Lauf, ueberschreibt also die Liege-Atem-Pose aus sub02. */
    re15_actor_t *ir = irons_1151();
    if (ir) {
        re15_actor_set_motion(ir, (int16_t)RE15_ELZA_IRONS_CLIP_TOT);
        ir->state       = 4;
        ir->sub_state_1 = 0;
        ir->sub_state_2 = 2;
        ir->anim_frame  = (uint8_t)RE15_ELZA_IRONS_BILD_TOT;
        ir->anim_flags  = 0;
        ir->anim_frac   = 0;
        ir->motion_init_delay = 0;
        /* (2) Kopf: Plc_neck Modus 0 mit Geschwindigkeit 0 (op_plc_neck: +0x1b8 = 0x80|0x12, +0x9e/+0x9f = 0),
         *     Akkus 0 -> der Kopf liegt auf dem Clip, keine Verfolgung (Belege im Header). re15_npc_neck_spawn_init
         *     laesst Flags/Geschwindigkeit/Akkus stehen, sobald Bit 0x80 gesetzt ist (game_step_common.c). */
        ir->neck_flags = (uint8_t)RE15_ELZA_IRONS_NECK_FLAGS;
        ir->neck_speed = (int16_t)RE15_ELZA_IRONS_NECK_SPEED;
        ir->neck_tx = 0; ir->neck_ty = 0; ir->neck_tz = 0;
        ir->neck_yaw = 0; ir->neck_pitch = 0; ir->neck_sweep = 0;
    }

    /* (3) Anklicken: Slot 2 wie ein Aot_reset auf sce 1 (LAB_80040738; Rechteck/Band bleiben die des Platzes an der
     *     Liege) -> Text 20. (Die Annahme "work_vars[0] wird nie 2" galt nicht — Runde 38 Spur Q, s.u.) */
    text_einsetzen(RE15_ELZA_IRONS_MSG_ID, k_irons_text, sizeof k_irons_text);
    /* Runde 38 Spur Q: erst NACH der Hinbeuge-Szene (sub01 @0x00EA8 Set(3,0x9D)) umbauen — davor bleibt Slot 2 der
     * Original-Platz und sub03 laeuft mit Text 20 (re15_kleinkram_r38.h; der Scan stempelt work_vars[0] auch fuer sce 1). */
    if (re15_game_flag_get(3, 0x9D) &&
        g_aot.slots[RE15_ELZA_IRONS_SLOT].active &&
        g_aot.slots[RE15_ELZA_IRONS_SLOT].type == RE15_AOT_TYPE_EXAMINE_WORKVAR)
        re15_aot_retype(RE15_ELZA_IRONS_SLOT, RE15_ELZA_IRONS_SCE_TEXT, RE15_ELZA_IRONS_FLAGS,
                        RE15_ELZA_IRONS_MSG_ID, RE15_ELZA_IRONS_MASKE, 0);
    logf_("[elza] ROOM1151: Irons gefunden=%u, Slot %u -> Text %u %u", ir ? 1u : 0u, RE15_ELZA_IRONS_SLOT,
          RE15_ELZA_IRONS_MSG_ID, 0);
}
