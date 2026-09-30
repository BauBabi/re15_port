/*
 * probe_r34n_a_rolltor.c — Spur A (Runde 34 Nacht), MESS-SONDE fuer den Bauplan
 * "Rolltor ROOM1050: Sicherung einsetzen mit Nahansicht, Tor erst danach".
 *
 * Dossier: analysis/befunde_runde34_nacht/A_rolltor.md (§4 Zeitlinie, §5 Bauplan).
 *
 * WAS SIE TUT: sie faehrt die GEPLANTEN Port-Unterprogramme (Bytecode genau wie im Dossier §5.3)
 * mit der ECHTEN VM ueber einen echten ROOM1050/ROOM1051-Raumaufbau — OHNE Engine-Aenderung:
 * scd_thread_start nimmt jeden Programmzeiger (scd_vm.c scd_thread_start), der Haken in
 * scd_event_fire ist also fuer die Messung nicht noetig. Die Sonde startet den Faden selbst in
 * Slot 10 (SCD_EVENT_SLOT_FIRST), genau dorthin, wohin scd_event_fire ihn legen wuerde.
 *
 * ⛔ Opcode 0x62 (RE2 Sce_item_lost, Tabelle 0x800a74c8[0x62] -> LAB_800585e4) ist in der VM
 * NOCH NICHT registriert — liefe er hier, faende op_unknown die Satzbreite 1 (s_opcode_sizes
 * [0x62] = 1) und laese das Item-Byte 0x40 als Plc_dest (8 Byte) = Desync. Die Sonde benutzt
 * deshalb eine Fassung mit `00 00` (zwei Nop) an dieser Stelle und nimmt die Sicherung SELBST
 * heraus, im selben Bild, in dem der Faden dort vorbeilaeuft (re15_inv_find_item + re15_inv_remove_slot = RE1.5
 * FUN_8004dfec / @0x8004aef0 + FUN_8004dadc). Diese Stelle ist die EINZIGE Abweichung vom Plan.
 *
 * GEMESSEN WIRD je Fall: Bildzahl bis zur Frage, Cut je Phase, offene Nachricht, Flags
 * (3,121) (9,63) (2,7), Inventar, Auto-Kamera, SCA-Zelle 19 (Rolltor zu?), und ob der Faden
 * jede geplante Opcode-Position genau so besucht, wie der Plan es sagt (keine Fremd-Opcodes).
 *
 * Kein Pin: diese Sonde prueft den PLAN, nicht den Port. Der Riegel fuer den Bau kommt mit dem Bau.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void scd_register_current_rdt(const re15_rdt_t *rdt);
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

/* ---- Der geplante Bytecode (Dossier §5.3). Jede Zeile mit ihrem Vorbild. ---------------- */

/* OHNE Sicherung im Inventar. */
static const uint8_t k_ohne[] = {
    /* +00 */ 0x2b, 0x00, 0x80, 0xff,   /* Message_on 0, Maske 0xFF80 = ROOM1050 sub02 @0x0CAC       */
    /* +04 */ 0x02, 0x00,               /* Evt_next + Nop             = sub02 @0x0CB0/@0x0CB1        */
    /* +06 */ 0x06, 0x00, 0x1a, 0x00,   /* Ifel_ck, Block bis +0x24                                   */
    /* +0A */ 0x21, 0x0c, 0x1f, 0x00,   /* Ck(12,31,0) = Ja           = sub02 @0x0CB6                */
    /* +0E */ 0x22, 0x02, 0x07, 0x01,   /* Set(2,7,1)                 = ROOM1051 sub03 @0x0DA4       */
    /* +12 */ 0x29, 0x07,               /* Cut_chg 7  (Nahansicht, Kamera @0x140)                     */
    /* +14 */ 0x2b, 0x02, 0xff, 0xff,   /* Message_on 2 "I need a fuse to run the shutter." (@0x0ED2) */
    /* +18 */ 0x02, 0x00,               /* Evt_next + Nop                                             */
    /* +1A */ 0x29, 0x03,               /* Cut_chg 3                  = ROOM1051 sub03 @0x0DC0       */
    /* +1C */ 0x3c, 0x01,               /* Cut_auto 1                 = ROOM1051 sub03 @0x0DC2       */
    /* +1E */ 0x22, 0x02, 0x07, 0x00,   /* Set(2,7,0)                 = ROOM1051 sub03 @0x0DD2       */
    /* +22 */ 0x08, 0x00,               /* Endif                                                      */
    /* +24 */ 0x01, 0x00,               /* Evt_end                                                    */
};

/* MIT Sicherung im Inventar. Die Sondenfassung traegt an +0x28 `00 00` statt `62 40`. */
static uint8_t k_mit[] = {
    /* +00 */ 0x2b, 0x00, 0x80, 0xff,   /* Message_on 0 (Frage)       = sub02 @0x0CAC                */
    /* +04 */ 0x02, 0x00,               /* Evt_next + Nop                                             */
    /* +06 */ 0x06, 0x00, 0x38, 0x00,   /* Ifel_ck aussen, Block bis +0x42                            */
    /* +0A */ 0x21, 0x0c, 0x1f, 0x00,   /* Ck(12,31,0) = Ja                                           */
    /* +0E */ 0x22, 0x02, 0x07, 0x01,   /* Set(2,7,1)                                                 */
    /* +12 */ 0x29, 0x07,               /* Cut_chg 7                                                  */
    /* +14 */ 0x2b, 0x02, 0xff, 0xff,   /* Message_on 2                                               */
    /* +18 */ 0x02, 0x00,               /* Evt_next + Nop                                             */
    /* +1A */ 0x2b, 0x14, 0xff, 0xff,   /* Message_on 20 "Will you use the Fuse?" (ROOM2060 msg 4)    */
    /* +1E */ 0x02, 0x00,               /* Evt_next + Nop             = ROOM2060 sub18 @0x01682      */
    /* +20 */ 0x06, 0x00, 0x14, 0x00,   /* Ifel_ck innen, Block bis +0x38                             */
    /* +24 */ 0x21, 0x0c, 0x1f, 0x00,   /* Ck(12,31,0) = Ja           = ROOM2060 sub18 @0x01688      */
    /* +28 */ 0x00, 0x00,               /* PLAN: 62 40 = Sce_item_lost(0x40), RE2 @0x800585e4         */
    /* +2A */ 0x22, 0x09, 0x3f, 0x01,   /* Set(9,63,1) "eingesetzt"   (Rolle von 2060 sub19 @0x0169E)*/
    /* +2E */ 0x29, 0x08,               /* Cut_chg 8                  (Rolle von 2060 sub19 @0x016A2)*/
    /* +30 */ 0x2b, 0x15, 0xff, 0xff,   /* Message_on 21 "You've used the Fuse." (2060 sub19 @0x016C8)*/
    /* +34 */ 0x02, 0x00,               /* Evt_next + Nop                                             */
    /* +36 */ 0x08, 0x00,               /* Endif innen                                                */
    /* +38 */ 0x29, 0x03,               /* Cut_chg 3                                                  */
    /* +3A */ 0x3c, 0x01,               /* Cut_auto 1                                                 */
    /* +3C */ 0x22, 0x02, 0x07, 0x00,   /* Set(2,7,0)                                                 */
    /* +40 */ 0x08, 0x00,               /* Endif aussen                                               */
    /* +42 */ 0x01, 0x00,               /* Evt_end                                                    */
};
#define MIT_ITEM_LOST_OFF 0x28

/* Nachrichten 20/21 = ROOM2060.RDT msg 4 @0x1855 / msg 5 @0x1875, Byte fuer Byte. */
static const uint8_t k_msg20[] = {
    0x04,0x02, 0x33,0x45,0x48,0x48,0x00,0x55,0x4b,0x51,0x00,0x51,0x4f,0x41,0x00,0x50,0x44,0x41,0x00,
    0x05,0x01, 0x22,0x51,0x4f,0x41, 0x05,0x00, 0x1b, 0x03, 0x02,0x01, 0x00 };
static const uint8_t k_msg21[] = {
    0x04,0x02, 0x35,0x4b,0x51,0x3a,0x52,0x41,0x00,0x51,0x4f,0x41,0x40,0x00,0x50,0x44,0x41,0x00,
    0x05,0x01, 0x22,0x51,0x4f,0x41, 0x05,0x00, 0x57, 0x01,0x00 };

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static int zelle19_solide(const re15_rdt_t *r)
{
    int n = 0, basis = 0;
    for (int g = 0; g < 5; g++) {
        int fi = basis + 19;
        basis += r->sca_rgn[g];
        if (fi < r->sca_count && r->sca[fi].u0 == 0xFF) n++;
    }
    return n;   /* 5 = Rolltor zu in allen Partitionen */
}

/* Raum wie im Spiel hochfahren (Muster test_room1050_sicherung.c (6)). */
static int raum_hoch(uint16_t raum, uint8_t *daten, long sz, re15_rdt_t *r)
{
    if (re15_rdt_parse(daten, (size_t)sz, r) != 0) return -1;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    scd_register_current_rdt(r);
    re15_msg_load_room_block(r->messages, r->messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = 17200 - 620; p->y = 0; p->z = -8550; p->rot_y = 0; p->state = 1;
    g_scd.player_mode = 0;
    scd_register_room_events(r);
    scd_room_reenter(r, p->x, p->z, 3);          /* Eintritts-Cut 3 = der Cut am Schalter (§2) */
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); }
    return 0;
}

typedef struct {
    int bild;                /* laufendes Sondenbild */
    int frage_bild;          /* erstes Bild mit Frage msg 0 im SELECT */
    int cut7_bild, cut8_bild, cut3_bild;
    int msg2_bild, msg20_bild, msg21_bild;
    int fremd;               /* Opcode-Positionen ausserhalb des Plans */
    int item_weg_bild;
} protokoll_t;

/* Ein Spielbild: Nachrichten-FSM, dann VM (Reihenfolge wie game_step -> main.c). `taste` wird
 * genau dann als EINE Flanke gegeben, wenn die FSM im passenden Wartezustand steht. */
static void bild(protokoll_t *pr, uint16_t taste_select, uint16_t taste_wait,
                 const uint8_t *prog, int prog_len, const int *plan, int plan_n, int mit_item)
{
    g_scd_pad_edge = 0; g_scd_pad_held = 0;
    if (g_scd.message_active) {
        if (g_scd.message_fsm == 3 && taste_select) g_scd_pad_edge = taste_select;
        if (g_scd.message_fsm == 4 && taste_wait)   g_scd_pad_edge = taste_wait;
    }
    re15_msg_tick(0, 0, 0);
    g_scd_pad_edge = 0;
    scd_thread_t *t = &g_scd.threads[SCD_EVENT_SLOT_FIRST];
    if (t->active && t->pc) {
        long off = (long)(t->pc - prog);
        if (off < 0 || off >= prog_len) pr->fremd++;
        else {
            int ok = 0;
            for (int k = 0; k < plan_n; k++) if (plan[k] == off) ok = 1;
            if (!ok) pr->fremd++;
        }
    }
    int stand_63 = re15_game_flag_get(9, 63);
    scd_vm_tick();
    /* 0x62-Ersatz: der Plan fuehrt Sce_item_lost(0x40) @+0x28 im SELBEN VM-Durchlauf aus wie
     * Set(9,63,1) @+0x2A (kein Ertrag dazwischen) — die Sonde nimmt die Sicherung deshalb in
     * genau dem Bild heraus, in dem (9,63) kippt. */
    if (mit_item && !stand_63 && re15_game_flag_get(9, 63) && pr->item_weg_bild < 0) {
        int s = re15_inv_find_item(0x40);            /* FUN_8004dfec */
        if (s >= 0) re15_inv_remove_slot(s);          /* @0x8004aef0 + FUN_8004dadc */
        pr->item_weg_bild = pr->bild;
    }
    if (g_scd.message_active && g_scd.message_id == 0 && g_scd.message_fsm == 3 && pr->frage_bild < 0)
        pr->frage_bild = pr->bild;
    if (g_scd.message_active && g_scd.message_id == 2  && pr->msg2_bild  < 0) pr->msg2_bild  = pr->bild;
    if (g_scd.message_active && g_scd.message_id == 20 && pr->msg20_bild < 0) pr->msg20_bild = pr->bild;
    if (g_scd.message_active && g_scd.message_id == 21 && pr->msg21_bild < 0) pr->msg21_bild = pr->bild;
    if (g_scd.cam_id == 7 && pr->cut7_bild < 0) pr->cut7_bild = pr->bild;
    if (g_scd.cam_id == 8 && pr->cut8_bild < 0) pr->cut8_bild = pr->bild;
    if (pr->cut7_bild >= 0 && g_scd.cam_id == 3 && pr->cut3_bild < 0) pr->cut3_bild = pr->bild;
    pr->bild++;
}

/* Opcode-Positionen des Plans (die VM steht zu Bildbeginn nur auf diesen). Evt_next-Nachfolger
 * (+1) gehoeren dazu, weil Evt_next den PC nur um 1 schiebt (@0x8003f260) — das Nop dahinter. */
static const int k_plan_ohne[] = { 0x00, 0x04, 0x05, 0x06, 0x0A, 0x0E, 0x12, 0x14, 0x18, 0x19,
                                   0x1A, 0x1C, 0x1E, 0x22, 0x24 };
static const int k_plan_mit[]  = { 0x00, 0x04, 0x05, 0x06, 0x0A, 0x0E, 0x12, 0x14, 0x18, 0x19,
                                   0x1A, 0x1E, 0x1F, 0x20, 0x24, 0x28, 0x2A, 0x2E, 0x30, 0x34,
                                   0x35, 0x36, 0x38, 0x3A, 0x3C, 0x40, 0x42 };

static int fall(const char *name, uint16_t raum, uint8_t *daten, long sz,
                int mit_sicherung, uint16_t antwort_schalter, uint16_t antwort_sicherung)
{
    uint8_t *kopie = (uint8_t *)malloc((size_t)sz);
    memcpy(kopie, daten, (size_t)sz);             /* Sca_id_set schreibt in den Puffer */
    re15_rdt_t r;
    if (raum_hoch(raum, kopie, sz, &r) != 0) { printf("FAIL %s: Raum\n", name); free(kopie); return 1; }

    re15_inv_init();
    re15_inv_load_briefing();
    if (mit_sicherung) {
        for (int s = 0; s < RE15_INV_MAX_SLOTS; s++)
            if (g_inv.slots[s].id == 0) { g_inv.slots[s].id = 0x40; g_inv.slots[s].qty = 1; break; }
    }
    re15_msg_install_text(20, k_msg20, sizeof k_msg20);
    re15_msg_install_text(21, k_msg21, sizeof k_msg21);

    int vorher_zelle = zelle19_solide(&r);
    uint8_t cam_vorher = g_scd.cam_id;

    const uint8_t *prog = mit_sicherung ? k_mit : k_ohne;
    int len = mit_sicherung ? (int)sizeof k_mit : (int)sizeof k_ohne;
    const int *plan = mit_sicherung ? k_plan_mit : k_plan_ohne;
    int plan_n = mit_sicherung ? (int)(sizeof k_plan_mit / sizeof k_plan_mit[0])
                               : (int)(sizeof k_plan_ohne / sizeof k_plan_ohne[0]);

    scd_thread_start(SCD_EVENT_SLOT_FIRST, prog);
    protokoll_t pr = { 0, -1, -1, -1, -1, -1, -1, -1, 0, -1 };
    int schalter_beantwortet = 0;
    for (int i = 0; i < 3000 && g_scd.threads[SCD_EVENT_SLOT_FIRST].active; i++) {
        uint16_t sel = 0, wait = 0;
        if (g_scd.message_active && g_scd.message_id == 0)  { sel = antwort_schalter; schalter_beantwortet = 1; }
        if (g_scd.message_active && g_scd.message_id == 20) sel = antwort_sicherung;
        if (g_scd.message_active && (g_scd.message_id == 2 || g_scd.message_id == 21)) wait = 0x4000;
        bild(&pr, sel, wait, prog, len, plan, plan_n, mit_sicherung);
    }
    (void)schalter_beantwortet;
    int aktiv = g_scd.threads[SCD_EVENT_SLOT_FIRST].active;
    int s40 = re15_inv_find_item(0x40);
    printf("%-34s Frage@%-4d Cut7@%-4d msg2@%-4d msg20@%-4d Item-weg@%-4d Cut8@%-4d msg21@%-4d "
           "Cut3@%-4d Ende@%-4d | cam %u->%u auto=%u (2,7)=%d (3,121)=%d (9,63)=%d Fuse-Slot=%d "
           "Zelle19 %d->%d fremd=%d laeuft=%d\n",
           name, pr.frage_bild, pr.cut7_bild, pr.msg2_bild, pr.msg20_bild, pr.item_weg_bild,
           pr.cut8_bild, pr.msg21_bild, pr.cut3_bild, pr.bild, (unsigned)cam_vorher,
           (unsigned)g_scd.cam_id, (unsigned)g_scd.cut_auto_enabled, re15_game_flag_get(2, 7),
           re15_game_flag_get(3, 121), re15_game_flag_get(9, 63), s40,
           vorher_zelle, zelle19_solide(&r), pr.fremd, aktiv);
    free(kopie);
    return (pr.fremd != 0 || aktiv) ? 1 : 0;
}

int main(void)
{
    long sz0 = 0, sz1 = 0;
    uint8_t *d1050 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1050.RDT", &sz0);
    uint8_t *d1051 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1051.RDT", &sz1);
    if (!d1050 || !d1051) { printf("FAIL: RDT nicht lesbar\n"); return 1; }

    const uint16_t JA = 0x4000;                 /* virtuell 0x4000 = SELECT bestaetigen (msg_common.c) */
    const uint16_t NEIN = 0x1000 | 0x4000;      /* Links/Rechts-Flanke schaltet, Bestaetigen im selben Bild
                                                 * kommt erst NACH dem Umschalten (case 3: lr vor act) */
    int f = 0;
    printf("=== Sonde r34n_a_rolltor: geplanter Bytecode an der echten VM ===\n");
    f |= fall("1050 ohne Sicherung, Schalter Ja", 0x1050, d1050, sz0, 0, JA, 0);
    f |= fall("1050 ohne Sicherung, Schalter Nein", 0x1050, d1050, sz0, 0, NEIN, 0);
    f |= fall("1050 mit Sicherung, Ja/Ja", 0x1050, d1050, sz0, 1, JA, JA);
    f |= fall("1050 mit Sicherung, Ja/Nein", 0x1050, d1050, sz0, 1, JA, NEIN);
    f |= fall("1051 ohne Sicherung, Schalter Ja", 0x1051, d1051, sz1, 0, JA, 0);
    f |= fall("1051 mit Sicherung, Ja/Ja", 0x1051, d1051, sz1, 1, JA, JA);
    printf(f ? "ERGEBNIS: AUFFAELLIG (fremd/laeuft oben pruefen)\n" : "ERGEBNIS: Plan laeuft sauber\n");

    /* GEGENPROBE zum Risiko "HAKEN 2 fehlt": dieselbe Fassung mit den ECHTEN Bytes `62 40` an
     * +0x28, ohne registrierten Opcode 0x62. op_unknown schiebt um 1 (s_opcode_sizes[0x62] = 1)
     * und liest `40` als Plc_dest (8 B). Die Ersatz-Entnahme der Sonde bleibt an; sie greift nur,
     * wenn (9,63) kippt. */
    k_mit[MIT_ITEM_LOST_OFF] = 0x62; k_mit[MIT_ITEM_LOST_OFF + 1] = 0x40;
    {
        int g = fall("GEGENPROBE 0x62 unregistriert", 0x1050, d1050, sz0, 1, JA, JA);
        /* Mass ist das ERGEBNIS, nicht die PC-Stichprobe: der Fremdlauf passiert innerhalb EINES
         * VM-Durchlaufs (kein Ertrag an einer Fremdposition), die Stichprobe am Bildanfang sieht
         * ihn nicht. Verschluckt ist das Einsetzen, wenn nach Ja/Ja weder (9,63) steht noch die
         * Sicherung fehlt. */
        int verschluckt = !re15_game_flag_get(9, 63) && re15_inv_find_item(0x40) >= 0;
        printf("GEGENPROBE: fremd/laeuft=%d, Einsetzen %s\n", g,
               verschluckt ? "STILL VERSCHLUCKT (62 -> op_unknown pc+1, `40` = Plc_dest 8 B, "
                             "Default @+0x31, im Plan erst wieder ab Endif @+0x36) - Haken 2 (Opcode 0x62) ist Pflicht"
                           : "gelaufen - Annahme pruefen");
    }
    free(d1050); free(d1051);
    return 0;
}
