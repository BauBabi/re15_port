/* test_r34n_f_leiche.c — RIEGEL (Runde 34 Nacht, Spur F): die Leichen ROOM1110 / ROOM1230 zeigen den
 * neuen Text und bieten EINMAL Handgun-Munition an, wiederholbar bis zur Annahme.
 *
 * Dossier: analysis/befunde_runde34_nacht/F_leichen.md (Abschnitte 4, 6, 9), Gegenpruefung
 * F_leichen.gegenpruefung.md (Auflagen 1, 4, 5). Konstanten und Belege: include/re15_leiche.h.
 *
 * Verfahren wie die Sonde probe_r34n_f_leiche (aus der dieser Riegel entsteht) — ABER mit den
 * ECHTEN Haken statt der Sonden-Simulation: re15_leiche_message_on sitzt in op_message_on (scd_vm.c),
 * re15_leiche_tick in re15_game_step (game_step_common.c). Die ausgelieferte RDT wird geladen, der
 * Raum ueber scd_room_reenter aufgebaut, der Spieler vor die Leiche gestellt (Vorwaerts-620-Punkt im
 * Rechteck) und EINMAL Quadrat gedrueckt — der echte Aktions-Scan startet das Original-Ereignis
 * (ROOM1110 Slot 5 -> sub02, ROOM1230 Slot 18 -> sub21). Bild-Reihenfolge wie main.c:
 * [SCD-Takt ausser bei laufendem Modal, main.c:5343] -> re15_msg_tick (main.c:5503) ->
 * re15_game_step (main.c:7358) -> re15_item_modal_tick (main.c:7380).
 *
 * Teile (je ein ctest-Eintrag, Aufruf: test_r34n_f_leiche <teil>):
 *   texte      die vier eingebackenen Texte = die Belegstellen der RDTs Byte fuer Byte
 *   nein_1110  Druck -> Nachricht 20 (nicht 0), 2 Seiten, Modal im Schliess-Bild, Faden steht waehrend
 *   nein_1230  des Modals nach Evt_next, "No" -> Bit 0, Inventar gleich, Platz wieder scharf; zweites
 *              Untersuchen: WIEDER Nachricht 20/22 + Modal. RDT-Nachricht 0/10 bleibt bytegleich.
 *   ja_1110    "Yes" -> Bit (9,61)/(9,62), H. Gun Bullets +7 (15 halbiert, gestapelt); zweites
 *   ja_1230    Untersuchen -> Nachricht 21/23 (kurz, eine Seite), KEIN Modal.
 *   voll       Inventar voll ohne Handgun-Munition -> "can't carry", Bit 0, wiederholbar.
 *   varianten  ROOM1111 / ROOM1231 (Elza) verhalten sich identisch (nein + ja).
 *   laden      Bit vor dem Raumaufbau gesetzt (= geladener Stand) -> kurzer Text, kein Modal;
 *              re15_savedata capture/restore traegt Bit 61/62.
 *   andere     alle uebrigen Nachrichten beider Raeume oeffnen unveraendert unter ihrer eigenen Id
 *              (u.a. ROOM1230 msg 0 = Tastenfeld), ohne Angebot.
 *   stimme     laeuft beim Druck noch eine Aufnahme (120 Bilder), oeffnet der Leichen-Text erst danach
 *              — der Haken sitzt HINTER dem Stimmen-Riegel (Auflage 1).
 *   nachhall   laeuft die EIGENE Aufnahme (main20.wav) laenger als der Text, oeffnet das Modal trotzdem
 *              im Bild, in dem der Freeze faellt, und beendet den Nachhall (ein Nachrichtenkanal,
 *              @0x80027e74..80); der Faden steht waehrend des Modals.
 *   latch      verschwindet der offene Leichen-Text ohne Schliessen (Raum-Neuaufbau, fremde Raum-Basis),
 *              geht KEIN Modal auf (Auflage 5).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"      /* re15_sin_q12 / re15_cos_q12 */
#include "re15_item_modal.h"
#include "re15_engine.h"        /* re15_pad_virtual_word */
#include "re15_savedata.h"
#include "re15_leiche.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern uint32_t g_re15_pauseflags;
extern int g_re15_voice_laeuft;
extern int g_re15_voice_restbilder;

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint8_t *rdt_datei(uint16_t room, size_t *n)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    return slurp(rp, n);
}

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

/* Stimmen-Simulation: die Plattform stempelt je Bild laeuft/restbilder (audio_pc.c); hier zaehlt der
 * Riegel selbst herunter. s_stimme_bei_eigener: startet eine Aufnahme dieser Laenge, sobald die
 * Leichen-Zeile ihre Stimme queued (Nachhall-Fall). */
static int s_stimme_rest = 0;
static int s_stimme_bei_eigener = 0;
static int s_letzte_stimme = -1;

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    if (!re15_item_modal_active()) scd_vm_tick();        /* main.c:5343 Modal-Freeze des SCD-Takts */
    re15_msg_tick(&raw, &len, &id);                      /* main.c:5503 */
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);                              /* main.c:7358 — enthaelt re15_leiche_tick */
    if (re15_item_modal_active())                        /* main.c:7380 */
        re15_item_modal_tick(re15_pad_virtual_word(edge), re15_pad_virtual_word(held));
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) {
        if (e.kind == SCD_AUDIO_VOICE_ON) {
            s_letzte_stimme = (int)e.sample_id;
            if (s_stimme_bei_eigener && (e.sample_id == RE15_LEICHE_1110_MSG_LANG
                                          || e.sample_id == RE15_LEICHE_1230_MSG_LANG))
                s_stimme_rest = s_stimme_bei_eigener;
        }
    }
    if (s_stimme_rest > 0) s_stimme_rest--;
    g_re15_voice_laeuft     = s_stimme_rest > 0;
    g_re15_voice_restbilder = s_stimme_rest;
}

static int room_boot(uint16_t room)
{
    free(s_raw); s_raw = rdt_datei(room, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100 || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) {
        printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1;
    }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    s_shown = 0;
    re15_msg_clear_room_block();                                     /* Teardown wie room_pc.c:122 */
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);   /* wie room_common.c:392 */
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 0;
}

static int vorwaerts_trifft(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

/* Standplatz vor dem Leichen-Platz suchen (Vorwaerts-620-Punkt im Rechteck, FUN_80042bac). */
static int stellen(int slot)
{
    re15_aot_t *a = &g_aot.slots[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    for (int k = 0; k < 5; k++) {
        long zx = a->x + dxf[k] * (long)(a->half_w / 2);
        long zz = a->z + dzf[k] * (long)(a->half_h / 2);
        for (int d = 0; d < 16; d++) {
            int yaw = d * 256;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            frame(0, 0);
            g_room_change.pending = 0;
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw && vorwaerts_trifft(a, px, pz, yaw))
                return 1;
        }
    }
    return 0;
}

static long faden_pc(void)
{
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc && t->pc >= s_raw && t->pc < s_raw + s_rawsz)
            return (long)(t->pc - s_raw);
    }
    return -1;
}

static int inv_menge(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n += g_inv.slots[i].qty;
    return n;
}

typedef struct {
    int ids[8], n_ids;          /* geoeffnete Nachrichten in Reihenfolge */
    int seiten;                 /* Seitenstopps (PAGE_WAIT) */
    int auf_bild, zu_bild;      /* erste Nachricht */
    int modal_starts, modal_auf, modal_zu, prompt;
    long pc_bei_zu, pc_min_modal, pc_max_modal;
    int msg_aktiv_bei_modal;    /* message_active im Bild, in dem das Modal aufging */
    int fsm_bei_modal;          /* message_fsm in diesem Bild: 7 = der Text war im Nachhall */
    int scharf, ende_bild;
} ablauf_t;

/* EIN Untersuchen: Druck, Text blaettern, Modal beantworten (antwort 0 = Yes, 1 = No). */
static ablauf_t untersuchen(int slot, int antwort)
{
    ablauf_t r; memset(&r, 0, sizeof r);
    r.auf_bild = r.zu_bild = r.modal_auf = r.modal_zu = r.scharf = r.ende_bild = -1;
    r.pc_bei_zu = -1; r.pc_min_modal = 0x7fffffff; r.pc_max_modal = -1;
    int msg_alt = 0, fsm_alt = -1, modal_alt = 0, gewaehlt = 0;
    for (int f = 0; f < 1200; f++) {
        uint16_t e = 0;
        if (f == 1) e = RE15_PAD_BIT_SQUARE;
        else if (g_scd.message_active && g_scd.message_fsm == 1 && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;
        else if (g_scd.message_active && g_scd.message_parse > 0 && g_scd.message_fsm != 7 && (f % 10) == 0) {
            int rl = 0; re15_msg_get_raw((int)g_scd.message_id, &rl);
            if (g_scd.message_parse >= rl - 2) e = RE15_PAD_BIT_SQUARE;
        } else if (re15_item_modal_active() && re15_item_modal_prompt_ready()) {
            uint8_t ty; int ch = 0;
            int pr = re15_item_modal_prompt(&ty, &ch);
            if (pr) r.prompt = pr;
            if (pr == 2 && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;                   /* can't carry */
            else if (pr == 1 && ch != antwort && (f % 10) == 0) e = RE15_PAD_BIT_RIGHT; /* Ja/Nein */
            else if (pr == 1 && ch == antwort && (f % 10) == 5 && !gewaehlt) { e = RE15_PAD_BIT_SQUARE; gewaehlt = 1; }
        }
        frame(e, e);

        long pc = faden_pc();
        int akt = g_scd.message_active && g_scd.message_fsm != 7;   /* Nachhall zaehlt als zu */
        if (akt && !msg_alt) {
            if (r.n_ids < 8) r.ids[r.n_ids++] = (int)g_scd.message_id;
            if (r.auf_bild < 0) r.auf_bild = f;
        }
        if (akt && (int)g_scd.message_fsm != fsm_alt) {
            if (g_scd.message_fsm == 1) r.seiten++;
            fsm_alt = (int)g_scd.message_fsm;
        }
        if (!akt && msg_alt) {
            if (r.zu_bild < 0) { r.zu_bild = f; r.pc_bei_zu = pc; }
            fsm_alt = -1;
        }
        msg_alt = akt;
        int mo = re15_item_modal_active();
        if (mo && !modal_alt) { r.modal_starts++; if (r.modal_auf < 0) r.modal_auf = f;
                                r.msg_aktiv_bei_modal = g_scd.message_active;
                                r.fsm_bei_modal = (int)g_scd.message_fsm; }
        if (mo) { if (pc < r.pc_min_modal) r.pc_min_modal = pc; if (pc > r.pc_max_modal) r.pc_max_modal = pc; }
        if (!mo && modal_alt) r.modal_zu = f;
        modal_alt = mo;
        if (r.zu_bild >= 0 && pc < 0 && !mo && r.scharf < 0) {
            r.scharf = g_aot.slots[slot].active ? 1 : 0;
            r.ende_bild = f;
            break;
        }
    }
    printf("    Ablauf: Nachrichten");
    for (int i = 0; i < r.n_ids; i++) printf(" %d", r.ids[i]);
    printf(" | auf F%d zu F%d, %d Seitenstopps | Modal %dx auf F%d zu F%d Prompt %d | Faden bei zu @0x%05lX, "
           "waehrend Modal @0x%05lX..@0x%05lX | Ende F%d scharf %d | Stimme %d\n",
           r.auf_bild, r.zu_bild, r.seiten, r.modal_starts, r.modal_auf, r.modal_zu, r.prompt,
           r.pc_bei_zu, r.pc_min_modal, r.pc_max_modal, r.ende_bild, r.scharf, s_letzte_stimme);
    return r;
}

typedef struct { uint16_t room; int slot, msg_orig, lang, kurz, bit; long pc_park; } leiche_def_t;
static leiche_def_t def_von(uint16_t room)
{
    leiche_def_t d;
    d.room = room;
    if ((room & 0xFFF0u) == RE15_LEICHE_1110_RAUM) {
        d.slot = 5; d.msg_orig = RE15_LEICHE_1110_MSG_ORIG;
        d.lang = RE15_LEICHE_1110_MSG_LANG; d.kurz = RE15_LEICHE_1110_MSG_KURZ;
        d.bit = RE15_LEICHE_1110_BIT; d.pc_park = 0x0D09;   /* nach Evt_next @0x0D08 (Sonde) */
    } else {
        d.slot = 18; d.msg_orig = RE15_LEICHE_1230_MSG_ORIG;
        d.lang = RE15_LEICHE_1230_MSG_LANG; d.kurz = RE15_LEICHE_1230_MSG_KURZ;
        d.bit = RE15_LEICHE_1230_BIT; d.pc_park = 0x014C1;  /* nach Evt_next @0x014C0 (Sonde) */
    }
    return d;
}

static int raw_gleich(int id, const uint8_t *soll, int n)
{
    int len = 0; const unsigned char *m = re15_msg_get_raw(id, &len);
    return m && len == n && memcmp(m, soll, (size_t)n) == 0;
}

/* Die RDT-Nachricht msg_orig muss nach dem Untersuchen bytegleich zur Datei im Speicher stehen. */
static int rdt_nachricht_unveraendert(int msg)
{
    const uint8_t *blk = s_rdt.messages;
    int off = blk[msg * 2] | (blk[msg * 2 + 1] << 8);
    int len = 0; const unsigned char *m = re15_msg_get_raw(msg, &len);
    return m && len > 4 && memcmp(m, blk + off, (size_t)len) == 0;
}

static int vorbereiten(uint16_t room, int voll)
{
    re15_game_state_init();
    if (room_boot(room) != 0) return -1;
    re15_inv_load_briefing();
    if (voll)
        for (int i = 0; i < 10; i++) { g_inv.slots[i].id = (uint8_t)(0x22 + i); g_inv.slots[i].qty = 1; g_inv.slots[i].flags = 0; }
    leiche_def_t d = def_von(room);
    if (!stellen(d.slot)) { printf("  FEHLER: kein Standplatz vor Platz %d\n", d.slot); g_fail++; return -1; }
    return 0;
}

/* Erstes Untersuchen mit Angebot pruefen (lang, Modal im Schliess-Bild, Faden steht). */
static void pruef_angebot(const leiche_def_t *d, const ablauf_t *r, const char *wer)
{
    int ln = 0; const uint8_t *lang = re15_leiche_text(d->room, 0, &ln);
    PRUEF(r->n_ids == 1 && r->ids[0] == d->lang, "%s: Nachricht %d (Port, lang) statt %d", wer, d->lang, d->msg_orig);
    PRUEF(raw_gleich(d->lang, lang, ln), "%s: Rohbytes der Nachricht %d = eingebackener langer Text (%d B)", wer, d->lang, ln);
    PRUEF(rdt_nachricht_unveraendert(d->msg_orig), "%s: RDT-Nachricht %d bleibt bytegleich im Speicher", wer, d->msg_orig);
    PRUEF(r->seiten == 1, "%s: zwei Seiten (1 Seitenstopp), gemessen %d", wer, r->seiten);
    PRUEF(s_letzte_stimme == d->lang, "%s: Stimme unter der Port-Id %d (main%02d.wav)", wer, d->lang, d->lang);
    PRUEF(r->modal_starts == 1 && r->modal_auf == r->zu_bild,
          "%s: genau ein Modal, im Bild, in dem der Text zugeht (F%d / F%d)", wer, r->modal_auf, r->zu_bild);
    PRUEF(r->pc_min_modal == d->pc_park && r->pc_max_modal == d->pc_park,
          "%s: Faden steht waehrend des Modals auf @0x%05lX", wer, d->pc_park);
    PRUEF(r->scharf == 1, "%s: Leichen-Platz %d danach wieder scharf", wer, d->slot);
}

static void teil_nein(uint16_t room)
{
    leiche_def_t d = def_von(room);
    printf("\n=== nein ROOM%04X ===\n", room);
    if (vorbereiten(room, 0) != 0) return;
    int m0 = inv_menge(RE15_LEICHE_ITEM);
    for (int runde = 1; runde <= 2; runde++) {
        char wer[32]; snprintf(wer, sizeof wer, "Runde %d No", runde);
        ablauf_t r = untersuchen(d.slot, 1);
        pruef_angebot(&d, &r, wer);
        PRUEF(r.prompt == 1, "%s: Ja/Nein-Frage", wer);
        PRUEF(re15_game_flag_get(9, (uint8_t)d.bit) == 0, "%s: Bit (9,%d) bleibt 0", wer, d.bit);
        PRUEF(inv_menge(RE15_LEICHE_ITEM) == m0, "%s: Munition unveraendert %d", wer, m0);
    }
}

static void teil_ja(uint16_t room)
{
    leiche_def_t d = def_von(room);
    printf("\n=== ja ROOM%04X ===\n", room);
    if (vorbereiten(room, 0) != 0) return;
    int m0 = inv_menge(RE15_LEICHE_ITEM);
    int slots0 = 0; for (int i = 0; i < 10; i++) if (g_inv.slots[i].id) slots0++;
    ablauf_t r = untersuchen(d.slot, 0);
    pruef_angebot(&d, &r, "Runde 1 Yes");
    PRUEF(re15_game_flag_get(9, (uint8_t)d.bit) == 1, "Runde 1 Yes: Bit (9,%d) gesetzt", d.bit);
    PRUEF(inv_menge(RE15_LEICHE_ITEM) == m0 + RE15_LEICHE_MENGE / 2,
          "Runde 1 Yes: H. Gun Bullets %d -> %d (+%d = 15 halbiert)", m0, inv_menge(RE15_LEICHE_ITEM), RE15_LEICHE_MENGE / 2);
    int slots1 = 0; for (int i = 0; i < 10; i++) if (g_inv.slots[i].id) slots1++;
    PRUEF(m0 == 0 || slots1 == slots0, "Runde 1 Yes: gestapelt (belegte Plaetze %d -> %d)", slots0, slots1);

    int kn = 0; const uint8_t *kurz = re15_leiche_text(room, 1, &kn);
    for (int runde = 2; runde <= 3; runde++) {
        int m1 = inv_menge(RE15_LEICHE_ITEM);
        ablauf_t r2 = untersuchen(d.slot, 0);
        char wer[32]; snprintf(wer, sizeof wer, "Runde %d nach Yes", runde);
        PRUEF(r2.n_ids == 1 && r2.ids[0] == d.kurz, "%s: Nachricht %d (kurz)", wer, d.kurz);
        PRUEF(raw_gleich(d.kurz, kurz, kn), "%s: Rohbytes = kurzer Text (%d B)", wer, kn);
        PRUEF(r2.seiten == 0, "%s: eine Seite (kein Seitenstopp)", wer);
        PRUEF(r2.modal_starts == 0, "%s: KEIN Modal", wer);
        PRUEF(inv_menge(RE15_LEICHE_ITEM) == m1, "%s: keine zweite Munition", wer);
        PRUEF(r2.scharf == 1, "%s: Platz wieder scharf", wer);
        PRUEF(rdt_nachricht_unveraendert(d.msg_orig), "%s: RDT-Nachricht %d bytegleich", wer, d.msg_orig);
    }
}

static void teil_voll(void)
{
    static const uint16_t raeume[2] = { 0x1110, 0x1230 };
    for (int k = 0; k < 2; k++) {
        leiche_def_t d = def_von(raeume[k]);
        printf("\n=== voll ROOM%04X ===\n", raeume[k]);
        if (vorbereiten(raeume[k], 1) != 0) continue;
        for (int runde = 1; runde <= 2; runde++) {
            char wer[32]; snprintf(wer, sizeof wer, "Runde %d voll", runde);
            ablauf_t r = untersuchen(d.slot, 0);
            pruef_angebot(&d, &r, wer);
            PRUEF(r.prompt == 2, "%s: \"can't carry\" (Prompt 2)", wer);
            PRUEF(re15_game_flag_get(9, (uint8_t)d.bit) == 0, "%s: Bit (9,%d) bleibt 0", wer, d.bit);
            PRUEF(inv_menge(RE15_LEICHE_ITEM) == 0, "%s: keine Munition eingefuegt", wer);
        }
    }
}

static void teil_laden(void)
{
    printf("\n=== laden: Speicherstand traegt Bit 61/62 ===\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    g_current_room_id = 0x1110;
    re15_game_flag_set(9, RE15_LEICHE_1110_BIT, 1);
    re15_game_flag_set(9, RE15_LEICHE_1230_BIT, 0);
    static re15_savedata_t sd;
    memset(&sd, 0, sizeof sd);
    re15_savedata_capture(&sd, 1234, 1);
    re15_game_state_init();
    PRUEF(re15_game_flag_get(9, RE15_LEICHE_1110_BIT) == 0, "nach dem Loeschen Bit 61 = 0");
    uint16_t raum = 0;
    int rc = re15_savedata_restore(&sd, &raum);
    PRUEF(rc == 0 && raum == 0x1110, "restore ok, Raum 0x%04X", raum);
    PRUEF(re15_game_flag_get(9, RE15_LEICHE_1110_BIT) == 1, "Bit 61 aus dem Speicherstand zurueck");
    PRUEF(re15_game_flag_get(9, RE15_LEICHE_1230_BIT) == 0, "Bit 62 aus dem Speicherstand zurueck (0)");

    static const uint16_t raeume[2] = { 0x1110, 0x1230 };
    for (int k = 0; k < 2; k++) {
        leiche_def_t d = def_von(raeume[k]);
        printf("\n--- laden ROOM%04X: Bit vor dem Raumaufbau gesetzt ---\n", raeume[k]);
        re15_game_state_init();
        re15_game_flag_set(9, (uint8_t)d.bit, 1);
        /* vorbereiten() ruft re15_game_state_init — hier deshalb von Hand, damit das Bit steht. */
        if (room_boot(raeume[k]) != 0) continue;
        re15_inv_load_briefing();
        if (!stellen(d.slot)) { printf("  FEHLER: kein Standplatz\n"); g_fail++; continue; }
        int m0 = inv_menge(RE15_LEICHE_ITEM);
        ablauf_t r = untersuchen(d.slot, 0);
        PRUEF(r.n_ids == 1 && r.ids[0] == d.kurz, "geladen: Nachricht %d (kurz)", d.kurz);
        PRUEF(r.seiten == 0 && r.modal_starts == 0, "geladen: eine Seite, kein Modal");
        PRUEF(inv_menge(RE15_LEICHE_ITEM) == m0, "geladen: keine Munition");
    }
}

/* Alle uebrigen Nachrichten des Raums ueber einen synthetischen Message_on — sie muessen unter ihrer
 * eigenen Id mit den RDT-Bytes aufgehen, und kein Angebot darf entstehen. */
static void teil_andere(void)
{
    static const uint16_t raeume[4] = { 0x1110, 0x1111, 0x1230, 0x1231 };
    for (int k = 0; k < 4; k++) {
        uint16_t room = raeume[k];
        leiche_def_t d = def_von(room);
        printf("\n=== andere ROOM%04X ===\n", room);
        re15_game_state_init();
        if (room_boot(room) != 0) continue;
        const uint8_t *blk = s_rdt.messages;
        int n = (blk[0] | (blk[1] << 8)) / 2;
        int ok = 0, gesamt = 0;
        for (int id = 0; id < n; id++) {
            if (id == d.msg_orig) continue;
            uint8_t code[] = { SCD_OP_MESSAGE_ON, (uint8_t)id, 0x00, 0x00, SCD_OP_EVT_END };
            scd_vm_init();
            g_current_room_id = room;
            re15_msg_clear_room_block();
            re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
            scd_thread_start(0, code);
            scd_vm_tick();
            gesamt++;
            int gut = g_scd.message_active && (int)g_scd.message_id == id
                      && rdt_nachricht_unveraendert(id) && !re15_leiche_angebot_offen();
            if (gut) ok++;
            else printf("    Nachricht %d: aktiv %d id %d angebot %d\n", id, (int)g_scd.message_active,
                        (int)g_scd.message_id, re15_leiche_angebot_offen());
            { extern void re15_discard_reset(void); re15_discard_reset(); }
        }
        PRUEF(ok == gesamt && gesamt == n - 1, "ROOM%04X: %d von %d anderen Nachrichten unveraendert unter "
              "eigener Id, kein Angebot", room, ok, gesamt);
        /* Gegenprobe: die Leichen-Nachricht selbst wird uebernommen */
        {
            uint8_t code[] = { SCD_OP_MESSAGE_ON, (uint8_t)d.msg_orig, 0xff, 0xff, SCD_OP_EVT_END };
            scd_vm_init();
            g_current_room_id = room;
            scd_thread_start(0, code);
            scd_vm_tick();
            PRUEF((int)g_scd.message_id == d.lang && re15_leiche_angebot_offen(),
                  "ROOM%04X: Gegenprobe msg %d -> Port-Nachricht %d mit Angebot", room, d.msg_orig, d.lang);
        }
    }
}

static void teil_stimme(void)
{
    static const uint16_t raeume[2] = { 0x1110, 0x1230 };
    for (int k = 0; k < 2; k++) {
        leiche_def_t d = def_von(raeume[k]);
        printf("\n=== stimme ROOM%04X: Aufnahme laeuft beim Druck noch 120 Bilder ===\n", raeume[k]);
        if (vorbereiten(raeume[k], 0) != 0) continue;
        s_stimme_rest = 0;
        ablauf_t ohne = untersuchen(d.slot, 1);            /* Bezug: ohne Aufnahme */
        s_stimme_rest = 121;                               /* ab dem Druckbild (F1) noch 120 Bilder */
        ablauf_t mit = untersuchen(d.slot, 1);
        s_stimme_rest = 0;
        PRUEF(ohne.auf_bild > 0 && ohne.auf_bild < 60, "ohne Aufnahme: Text auf F%d (nach Sleep 30)", ohne.auf_bild);
        PRUEF(mit.auf_bild >= 120, "mit Aufnahme: Text erst nach deren Ende auf F%d (>= F120)", mit.auf_bild);
        PRUEF(mit.n_ids >= 1 && mit.ids[0] == d.lang, "mit Aufnahme: trotzdem Port-Nachricht %d", d.lang);
        PRUEF(mit.modal_starts == 1 && mit.modal_auf == mit.zu_bild, "mit Aufnahme: Modal im Schliess-Bild");
    }
}

static void teil_nachhall(void)
{
    static const uint16_t raeume[2] = { 0x1110, 0x1230 };
    for (int k = 0; k < 2; k++) {
        leiche_def_t d = def_von(raeume[k]);
        printf("\n=== nachhall ROOM%04X: eigene Aufnahme 400 Bilder ===\n", raeume[k]);
        if (vorbereiten(raeume[k], 0) != 0) continue;
        s_stimme_bei_eigener = 400;
        ablauf_t r = untersuchen(d.slot, 1);
        s_stimme_bei_eigener = 0; s_stimme_rest = 0;
        PRUEF(r.modal_starts == 1 && r.modal_auf == r.zu_bild,
              "Modal im Bild, in dem der Freeze faellt (F%d / F%d), trotz laufender Aufnahme", r.modal_auf, r.zu_bild);
        PRUEF(r.fsm_bei_modal == 7, "der Text war beim Schliessen im Nachhall (msg-FSM Zustand 7)");
        PRUEF(r.msg_aktiv_bei_modal == 0, "Nachhall beim Oeffnen des Modals beendet (message_active 0)");
        PRUEF(r.pc_min_modal == d.pc_park && r.pc_max_modal == d.pc_park,
              "Faden steht waehrend des Modals auf @0x%05lX", d.pc_park);
        PRUEF(r.scharf == 1, "Platz danach wieder scharf");
    }
}

/* Auflage 5: das Angebot darf NUR auf das Schliessen des EIGENEN Textes folgen. Zwei Wege, auf denen
 * ein offener Leichen-Text verschwindet, ohne dass der Spieler ihn schliesst:
 *   a) Raum-Neuaufbau (Laden, Selbst-Tuer): die Transitions-FSM loescht die Pause-Flags
 *      (@0x8001ca44 / @0x8001caec `sw zero,0x800aca40`, Port re15_pauseflags_clear) und
 *      scd_room_reenter nullt g_scd -> message_id 0 != 20/22.
 *   b) Wechsel der Raum-Basis bei stehendem g_scd (Sprung).
 * In beiden Faellen darf KEIN Modal aufgehen. */
static int text_oeffnen(const leiche_def_t *d)
{
    for (int f = 0; f < 120; f++) {
        frame(f == 1 ? RE15_PAD_BIT_SQUARE : 0, f == 1 ? RE15_PAD_BIT_SQUARE : 0);
        if (g_scd.message_active && (int)g_scd.message_id == d->lang) return f;
    }
    return -1;
}

static void teil_latch(void)
{
    static const uint16_t raeume[2] = { 0x1110, 0x1230 };
    for (int k = 0; k < 2; k++) {
        leiche_def_t d = def_von(raeume[k]);
        printf("\n=== latch ROOM%04X ===\n", raeume[k]);
        /* a) Raum-Neuaufbau */
        if (vorbereiten(raeume[k], 0) != 0) continue;
        int f = text_oeffnen(&d);
        PRUEF(f > 0 && re15_leiche_angebot_offen(), "a) Text %d offen (F%d), Angebot scharf", d.lang, f);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_pauseflags_clear();
        re15_msg_clear_room_block();
        scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
        re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
        int starts = 0;
        for (int i = 0; i < 90; i++) { frame(0, 0); if (re15_item_modal_active()) starts++; }
        PRUEF(starts == 0 && !re15_leiche_angebot_offen(), "a) Raum-Neuaufbau bei offenem Text: kein Modal (%d Bilder)", starts);
        /* b) Raum-Basis wechselt bei stehendem g_scd */
        if (vorbereiten(raeume[k], 0) != 0) continue;
        f = text_oeffnen(&d);
        PRUEF(f > 0 && re15_leiche_angebot_offen(), "b) Text %d offen (F%d), Angebot scharf", d.lang, f);
        g_current_room_id = 0x1100;                 /* anderer Raum, g_scd bleibt stehen */
        re15_pauseflags_clear();
        g_scd.message_active = 0; g_scd.message_fsm_active = 0;
        starts = 0;
        for (int i = 0; i < 30; i++) {
            re15_game_step(&s_ctx);                 /* nur der Spielschritt (Raum-Id bleibt fremd) */
            if (re15_item_modal_active()) starts++;
        }
        g_current_room_id = raeume[k];
        PRUEF(starts == 0 && !re15_leiche_angebot_offen(), "b) fremde Raum-Basis: kein Modal (%d Bilder)", starts);
    }
}

static void teil_texte(void)
{
    printf("\n=== texte: eingebackene Texte = Belegstellen der RDTs ===\n");
    size_t n1110 = 0, n1230 = 0, n1011 = 0;
    uint8_t *r1110 = rdt_datei(0x1110, &n1110), *r1230 = rdt_datei(0x1230, &n1230), *r1011 = rdt_datei(0x1011, &n1011);
    if (!r1110 || !r1230 || !r1011) { printf("  FEHLER: RDT fehlt\n"); g_fail++; return; }
    int ln, kn;
    const uint8_t *l = re15_leiche_text(0x1110, 0, &ln), *k = re15_leiche_text(0x1110, 1, &kn);
    /* 1110 lang = @0x0D68..0x0D8C (Kopf, Seite 1, Umbruch) + ROOM1011 @0x012A3 (23 B) + @0x0DA1 "." + @0x0DD3 Ende */
    PRUEF(l && ln == 63 && !memcmp(l, r1110 + 0x0D68, 37) && !memcmp(l + 37, r1011 + 0x12A3, 23)
          && l[60] == r1110[0x0DA1] && !memcmp(l + 61, r1110 + 0x0DD3, 2), "1110 lang (63 B) = RDT-Bytes");
    PRUEF(k && kn == 37 && !memcmp(k, r1110 + 0x0D68, 35) && !memcmp(k + 35, r1110 + 0x0DD3, 2),
          "1110 kurz (37 B) = ROOM1110 @0x0D68 Seite 1 + Ende @0x0DD3");
    l = re15_leiche_text(0x1230, 0, &ln); k = re15_leiche_text(0x1230, 1, &kn);
    PRUEF(l && ln == 50 && !memcmp(l, r1230 + 0x16F4, 24) && !memcmp(l + 24, r1011 + 0x12A3, 23)
          && l[47] == r1230[0x1720] && !memcmp(l + 48, r1230 + 0x1752, 2), "1230 lang (50 B) = RDT-Bytes");
    PRUEF(k && kn == 24 && !memcmp(k, r1230 + 0x16F4, 22) && !memcmp(k + 22, r1230 + 0x1752, 2),
          "1230 kurz (24 B) = ROOM1230 @0x16F4 Seite 1 + Ende @0x1752");
    /* Teil-Belege der Zusammensetzung */
    PRUEF(!memcmp(r1011 + 0x12A3, r1110 + 0x0D8D, 13) && !memcmp(r1011 + 0x12B0, r1110 + 0x0EC4, 10),
          "\"He is holding\" = ROOM1110 @0x0D8D, \" something\" = ROOM1110 @0x0EC4");
    /* lesbar, keine Auswahl, Varianten tragen dieselbe Zuordnung */
    char a[160];
    l = re15_leiche_text(0x1111, 0, &ln);
    re15_msg_decode_text(l, (size_t)ln, a, sizeof a);
    PRUEF(!strcmp(a, "It's a police officer, he's dead.He is holding something."), "1110/1111 lang liest \"%s\"", a);
    l = re15_leiche_text(0x1231, 0, &ln);
    re15_msg_decode_text(l, (size_t)ln, a, sizeof a);
    PRUEF(!strcmp(a, "A miserable death...He is holding something."), "1230/1231 lang liest \"%s\"", a);
    k = re15_leiche_text(0x1110, 1, &kn); re15_msg_decode_text(k, (size_t)kn, a, sizeof a);
    PRUEF(!strcmp(a, "It's a police officer, he's dead."), "1110 kurz liest \"%s\"", a);
    k = re15_leiche_text(0x1230, 1, &kn); re15_msg_decode_text(k, (size_t)kn, a, sizeof a);
    PRUEF(!strcmp(a, "A miserable death..."), "1230 kurz liest \"%s\"", a);
    PRUEF(re15_leiche_text(0x1050, 0, &ln) == NULL, "fremder Raum: kein Text");
    free(r1110); free(r1230); free(r1011);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "texte"))     teil_texte();
    if (alle || !strcmp(teil, "nein_1110")) teil_nein(0x1110);
    if (alle || !strcmp(teil, "nein_1230")) teil_nein(0x1230);
    if (alle || !strcmp(teil, "ja_1110"))   teil_ja(0x1110);
    if (alle || !strcmp(teil, "ja_1230"))   teil_ja(0x1230);
    if (alle || !strcmp(teil, "voll"))      teil_voll();
    if (alle || !strcmp(teil, "varianten")) { teil_nein(0x1111); teil_ja(0x1111); teil_nein(0x1231); teil_ja(0x1231); }
    if (alle || !strcmp(teil, "laden"))     teil_laden();
    if (alle || !strcmp(teil, "andere"))    teil_andere();
    if (alle || !strcmp(teil, "stimme"))    teil_stimme();
    if (alle || !strcmp(teil, "nachhall"))  teil_nachhall();
    if (alle || !strcmp(teil, "latch"))     teil_latch();
    printf("\ntest_r34n_f_leiche %s: %d FEHLER\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
