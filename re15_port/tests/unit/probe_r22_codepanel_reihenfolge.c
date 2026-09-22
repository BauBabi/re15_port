/* probe_r22_codepanel_reihenfolge.c — MESSSONDE: KOMMT "You've used the <Karte>."
 * VOR ODER NACH DER CODE-EINGABE?
 *
 * Nutzer-Befund (woertlich): "das Problem das ich Bei dir sah, war das du die discard
 * Abfrage auch bei Toren mit Raetsel panels machst, also wo man einen Code eingeben muss.
 * Da ist das natuerlich erst dann korrekt, wenn man den zugriffscode den man braucht
 * einmalig richtig eingegeben hat."
 *
 * Diese Sonde MISST die Reihenfolge am ausgelieferten Skript, statt sie zu behaupten.
 * Gefahren wird der Port-eigene SCD-VM auf den Original-Bytes von ROOM10D0.RDT
 * (Blue Keycard, msg 9 = die Wegwerf-Stelle aus gen_discard_sites.inc).
 *
 * ORIGINAL-SEITE (selbst disassembliert, Datei-Offsets in ROOM10D0.RDT):
 *   sub20  0x019AE Message_on 7    "A card reader. You have to use the Blue Keycard and
 *                                   insert four digits to unlock the door. Will you
 *                                   operate the card reader?"  (Ja/Nein, .msg-Code 0x03)
 *          0x019B8 Ck(12,31,0)     Antwort JA  (bank 12 = DAT_800b8520, Tabelle
 *                                   PTR_DAT_80074664[12])
 *          0x019C0 Ck(9,52,1)      Blue-Keycard-"genommen"-Bit (bank 9 = DAT_800b1078)
 *          0x019C4 Message_on 9    "You've used the Blue Keycard."   <<< Wegwerf-Stelle
 *          0x019CA Evt_exec sub17  >>> ERST JETZT geht das Tastenfeld auf
 *   sub17  0x018D6 Cut_chg 0x0b
 *          0x018E2 Message_on 0    "Enter the first number."
 *          0x018EC Set(5,0,1) / 0x018F0 Set(5,1,1) / 0x018F4 Set(5,2,1)
 *   sub01  0x01512 Ck(3,50,0) 0x01516 Ck(5,13,1) 0x0151A Ck(5,14,1)
 *          0x0151E Ck(5,15,1) 0x01522 Ck(5,16,1)
 *          0x01526 Evt_exec sub19
 *          0x0152A Set(3,50,1)     <<< das bestaendige "Code war richtig"-Flag
 *   sub19  0x0199E Message_on 5    "You've opened the lock."
 *
 * Zwischen 0x019C4 (msg 9) und 0x019CA (Evt_exec sub17) liegt KEINE Verzweigung:
 * 2b 09 ff ff | 02 | 00 | 04 ff 18 11. Die Sonde bestaetigt das dynamisch mit
 * Bildnummern.
 *
 * Die vier Ziffern-Riegel 5.13..5.16 werden hier GESETZT (wie probe_11f0_puzzle_pos
 * das Gewinnmuster setzt), weil die Tastenfeld-Bedienung selbst (Member 0x0F +
 * Sce_key_ck 0x40 -> sub06..sub16) nicht Gegenstand dieser Messung ist. Gemessen wird
 * die REIHENFOLGE der Message_on-Ereignisse und der Zeitpunkt von Set(3,50,1).
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Ein Bild in der Reihenfolge des PC-Ports (main.c 3626 / 3751 / 3951 / 4810). */
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* Vorbelegte Flags, die NACH scd_vm_init (das alles nullt) und VOR scd_room_reenter
 * (das main00 fahrt) gesetzt werden - damit main00 sie schon sieht. */
static struct { int bank, bit; } s_vor[8];
static int s_vor_n = 0;
static void vorbelegen(int bank, int bit)
{ if (s_vor_n < 8) { s_vor[s_vor_n].bank = bank; s_vor[s_vor_n].bit = bit; s_vor_n++; } }

static void room_boot(uint16_t room, const char *file)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, file);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) { printf("SKIP: %s fehlt\n", rp); exit(77); }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse %s\n", file); exit(1); }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    for (int i = 0; i < s_vor_n; i++)
        re15_game_flag_set((uint8_t)s_vor[i].bank, (uint8_t)s_vor[i].bit, 1);
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
}

/* ---- Protokoll: bei JEDEM Wechsel von message_active/message_id eine Zeile ------------ */
static int s_prev_active = -1, s_prev_id = -1;
static int s_seen_n = 0;
static struct { int frame; int id; } s_seen[64];

static void log_frame(int f, int lockbank, int lockbit)
{
    int act = (int)g_scd.message_active, id = (int)g_scd.message_id;
    if (act != s_prev_active || (act && id != s_prev_id)) {
        if (act) {
            printf("   f%-4d Nachricht %2d GEHT AUF   flag(%d,%d)=%d  5.0=%d 5.2=%d "
                   "5.13..16=%d%d%d%d\n",
                   f, id, lockbank, lockbit,
                   re15_game_flag_get((uint8_t)lockbank, (uint8_t)lockbit),
                   re15_game_flag_get(5, 0), re15_game_flag_get(5, 2),
                   re15_game_flag_get(5, 13), re15_game_flag_get(5, 14),
                   re15_game_flag_get(5, 15), re15_game_flag_get(5, 16));
            if (s_seen_n < 64) { s_seen[s_seen_n].frame = f; s_seen[s_seen_n].id = id; s_seen_n++; }
        }
        s_prev_active = act; s_prev_id = id;
    }
}

/* ===================== Teil A: ROOM10D0 (Kartenleser + 4 Ziffern) ===================== */
static int teil_a(void)
{
    printf("\n=== Teil A: ROOM10D0, Blue Keycard, msg 9 — Kartenleser mit 4-Ziffern-Code ===\n");

    /* Skript-Anker gegen stille Daten-Drift: sub20 @0x019C4 msg 9 direkt vor
     * @0x019CA Evt_exec sub17, ohne Verzweigung dazwischen. */
    static const uint8_t anker[] = { 0x2b,0x09,0xff,0xff, 0x02, 0x00, 0x04,0xff,0x18,0x11 };
    room_boot(0x10D0, "STAGE1/ROOM10D0.RDT");
    if (memcmp(s_raw + 0x019C4, anker, sizeof anker) != 0) {
        printf("FAIL: ROOM10D0 @0x019C4 ist nicht `Message_on 9; Evt_next; Nop; Evt_exec sub17`\n");
        return 1;
    }
    printf("[anker] @0x019C4 2b 09 ff ff | 02 | 00 | 04 ff 18 11   (msg 9 -> sub17, keine Verzweigung)  OK\n");
    /* sub17 @0x018E2 Message_on 0 = "Enter the first number." */
    if (!(s_raw[0x018E2] == 0x2b && s_raw[0x018E3] == 0x00)) {
        printf("FAIL: ROOM10D0 sub17 @0x018E2 ist nicht `Message_on 0`\n"); return 1;
    }
    /* sub01 @0x01512..0x0152A: Ck(3,50,0) + 4 Riegel -> sub19 -> Set(3,50,1) */
    static const uint8_t anker2[] = { 0x21,0x03,0x32,0x00, 0x21,0x05,0x0d,0x01,
                                      0x21,0x05,0x0e,0x01, 0x21,0x05,0x0f,0x01,
                                      0x21,0x05,0x10,0x01, 0x04,0xff,0x18,0x13,
                                      0x22,0x03,0x32,0x01 };
    if (memcmp(s_raw + 0x01512, anker2, sizeof anker2) != 0) {
        printf("FAIL: ROOM10D0 sub01 @0x01512 ist nicht die 4-Riegel-Kette -> Set(3,50,1)\n");
        return 1;
    }
    printf("[anker] @0x01512 Ck(3,50,0)+Ck(5,13..16,1) -> Evt_exec sub19 -> @0x0152A Set(3,50,1)  OK\n");

    /* Die Karte liegt im Inventar: das "genommen"-Bit der Blue Keycard (bank 9 bit 52,
     * Ck @0x019C0) setzen — sonst nimmt sub20 den Zweig msg 8 ("You have not the ..."). */
    re15_game_flag_set(9, 52, 1);

    /* sub20 starten (im Spiel: QUADRAT auf dem Kartenleser-AOT main00 @0x0103E,
     * Aot_set slot1 sce=3, Daten ff 00 18 14 = Ereignistyp 0x18 / sub 20). */
    int slot = -1;
    for (int i = 2; i < SCD_THREAD_COUNT; i++) if (!g_scd.threads[i].active) { slot = i; break; }
    if (slot < 0) { printf("FAIL: kein freier SCD-Thread\n"); return 1; }
    if (scd_thread_start(slot, s_rdt.sub_scd[20]) != 0) {
        printf("FAIL: sub20 startet nicht\n"); return 1;
    }
    printf("[lauf] sub20 gestartet in Thread %d; QUADRAT alle 6 Bilder (= Ja auf msg 7)\n", slot);

    s_prev_active = -1; s_prev_id = -1; s_seen_n = 0;
    int f_riegel = -1, f_lock = -1;
    for (int f = 0; f < 900; f++) {
        /* Sobald das Tastenfeld offen ist (5.0/5.1/5.2 gesetzt von sub17 @0x018EC-F4),
         * die vier Ziffern-Riegel setzen = "der Code war richtig". */
        if (f_riegel < 0 && re15_game_flag_get(5, 2)) {
            for (int b = 13; b <= 16; b++) re15_game_flag_set(5, (uint8_t)b, 1);
            f_riegel = f;
            printf("   f%-4d [Sonde] 5.13..16 := 1   (ersetzt die vier richtigen Tastendruecke)\n", f);
        }
        uint16_t e = (g_scd.message_active && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        log_frame(f, 3, 50);
        if (f_lock < 0 && re15_game_flag_get(3, 50)) {
            f_lock = f;
            printf("   f%-4d flag(3,50) := 1   (Set @0x0152A, sub01)\n", f);
        }
    }

    printf("[folge] ");
    for (int i = 0; i < s_seen_n; i++) printf("%smsg%d@f%d", i ? " -> " : "", s_seen[i].id, s_seen[i].frame);
    printf("\n");

    /* Urteil: msg 9 muss VOR msg 0 liegen, und flag(3,50) muss NACH msg 9 gesetzt werden. */
    int f9 = -1, f0 = -1, f5 = -1;
    for (int i = 0; i < s_seen_n; i++) {
        if (s_seen[i].id == 9 && f9 < 0) f9 = s_seen[i].frame;
        if (s_seen[i].id == 0 && f0 < 0 && f9 >= 0) f0 = s_seen[i].frame;
        if (s_seen[i].id == 5 && f5 < 0 && f9 >= 0) f5 = s_seen[i].frame;
    }
    printf("[urteil] msg9(\"You've used the Blue Keycard.\")=f%d   "
           "msg0(\"Enter the first number.\")=f%d   msg5(\"You've opened the lock.\")=f%d   "
           "flag(3,50)=1 ab f%d\n", f9, f0, f5, f_lock);
    if (f9 < 0) { printf("FAIL: msg 9 ist nie aufgegangen\n"); return 1; }
    if (f0 < 0) { printf("FAIL: msg 0 (Code-Eingabe) ist nach msg 9 nie aufgegangen\n"); return 1; }
    if (!(f9 < f0)) { printf("FAIL: msg 9 lag NICHT vor der Code-Eingabe\n"); return 1; }
    if (f_lock >= 0 && !(f9 < f_lock)) { printf("FAIL: flag(3,50) stand schon bei msg 9\n"); return 1; }
    printf("OK  msg 9 liegt VOR der Code-Eingabe; das bestaendige Flag(3,50) kommt erst danach.\n");
    return 0;
}

/* ===================== Teil B: ROOM1100 (Cursor-Panel, Gegenprobe) ==================== */
static int teil_b(void)
{
    printf("\n=== Teil B: ROOM1100, Minidisc Player w/ Disc, msg 4 — Cursor-Panel (Gegenprobe) ===\n");
    room_boot(0x1100, "STAGE1/ROOM1100.RDT");

    /* sub02 @0x00C68 Set(4,232,1) ... @0x00C90 Message_on 4 — das bestaendige Flag steht
     * BEREITS, wenn die Wegwerf-Stelle aufgeht. */
    if (!(s_raw[0x00C68] == 0x22 && s_raw[0x00C69] == 0x04 && s_raw[0x00C6A] == 0xe8 &&
          s_raw[0x00C6B] == 0x01 && s_raw[0x00C90] == 0x2b && s_raw[0x00C91] == 0x04)) {
        printf("FAIL: ROOM1100 sub02 ist nicht `Set(4,232,1) ... Message_on 4`\n"); return 1;
    }
    printf("[anker] sub02 @0x00C68 Set(4,232,1)  ->  @0x00C90 Message_on 4  OK\n");

    int slot = -1;
    for (int i = 2; i < SCD_THREAD_COUNT; i++) if (!g_scd.threads[i].active) { slot = i; break; }
    if (slot < 0) { printf("FAIL: kein freier SCD-Thread\n"); return 1; }
    if (scd_thread_start(slot, s_rdt.sub_scd[2]) != 0) { printf("FAIL: sub02 startet nicht\n"); return 1; }

    s_prev_active = -1; s_prev_id = -1; s_seen_n = 0;
    int f_flag = -1;
    for (int f = 0; f < 600; f++) {
        uint16_t e = (g_scd.message_active && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        log_frame(f, 4, 232);
        if (f_flag < 0 && re15_game_flag_get(4, 232)) { f_flag = f; }
    }
    int f4 = -1;
    for (int i = 0; i < s_seen_n; i++) if (s_seen[i].id == 4 && f4 < 0) f4 = s_seen[i].frame;
    printf("[urteil] flag(4,232)=1 ab f%d, msg4(\"You've used the Minidisc Player w/ Disc.\")=f%d\n",
           f_flag, f4);
    if (f4 < 0) { printf("FAIL: msg 4 ist nie aufgegangen\n"); return 1; }
    if (!(f_flag >= 0 && f_flag <= f4)) {
        printf("FAIL: flag(4,232) stand bei msg 4 noch nicht\n"); return 1;
    }
    printf("OK  ROOM1100 setzt das bestaendige Flag VOR der Wegwerf-Stelle — hier ist nichts zu aendern.\n");
    return 0;
}


/* ============ Teil C: das Flag(3,139) wird schon im INTRO-RAUM gesetzt ============ */
/* Vollzensus (tools/discard_codegate.py) fand fuer flag(3,139) DREI Schreiber:
 *   ROOM11E0 @0x01A8A / ROOM11E1 @0x01A8A  = "Code richtig" (die Riegel-Kette)
 *   ROOM1240 @0x0055A                      = die ERSTE Anweisung von sub02
 * ROOM1240 ist der Neuspiel-Startraum (engine/src/re15_gameflow.c:16
 * `#define RE15_NEWGAME_ROOM 0x1240`) und sub00 @0x00544 ruft sub02 beim Betreten.
 * Damit steht flag(3,139) ab dem ersten Bild eines neuen Spiels auf 1 - und ROOM11E0s
 * main00 @0x01576 `Ck(3,139,0)` nimmt dann den Else-Zweig @0x016C0. Diese Messung
 * zeigt, WELCHE Ausloeser der Raum in beiden Faellen installiert. */
static int aot_mit_event(int ev)
{
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].event_id == (uint8_t)ev
            && (g_aot.slots[i].type == RE15_AOT_TYPE_GENERIC
                || g_aot.slots[i].type == RE15_AOT_TYPE_AUTO_EVENT))
            return i;
    return -1;
}

static int teil_c(void)
{
    printf("\n=== Teil C: ROOM1240 (Neuspiel-Startraum) setzt flag(3,139) - Kollision ===\n");

    /* C1: ROOM1240 sub02 @0x0055A */
    s_vor_n = 0;
    room_boot(0x1240, "STAGE1/ROOM1240.RDT");
    if (!(s_raw[0x0055A] == 0x22 && s_raw[0x0055B] == 0x03 && s_raw[0x0055C] == 0x8b &&
          s_raw[0x0055D] == 0x01 && s_raw[0x00544] == 0x04 && s_raw[0x00547] == 0x02)) {
        printf("FAIL: ROOM1240 ist nicht `sub00 -> Evt_exec sub02` / `sub02 @0x0055A Set(3,139,1)`\n");
        return 1;
    }
    printf("[anker] ROOM1240 sub00 @0x00544 Evt_exec sub02  ->  sub02 @0x0055A Set(3,139,1)  OK\n");
    int f139 = -1;
    for (int f = 0; f < 120; f++) {
        frame(0, 0);
        if (f139 < 0 && re15_game_flag_get(3, 139)) f139 = f;
    }
    printf("[mess ] Nach dem Betreten von ROOM1240: flag(3,139)=%d (ab Bild %d)\n",
           re15_game_flag_get(3, 139), f139);
    if (!re15_game_flag_get(3, 139)) {
        printf("FAIL: ROOM1240 setzt flag(3,139) doch nicht\n"); return 1;
    }

    /* C2: ROOM11E0 ohne das Flag -> Kartenleser da? */
    s_vor_n = 0;
    room_boot(0x11E0, "STAGE1/ROOM11E0.RDT");
    int a_ohne = aot_mit_event(20);
    printf("[mess ] ROOM11E0 mit flag(3,139)=0: Ausloeser fuer sub20 (Kartenleser) %s\n",
           a_ohne >= 0 ? "VORHANDEN" : "FEHLT");

    /* C3: ROOM11E0 MIT dem Flag (wie nach dem Intro) -> Kartenleser da? */
    vorbelegen(3, 139);
    room_boot(0x11E0, "STAGE1/ROOM11E0.RDT");
    int a_mit = aot_mit_event(20);
    printf("[mess ] ROOM11E0 mit flag(3,139)=1: Ausloeser fuer sub20 (Kartenleser) %s\n",
           a_mit >= 0 ? "VORHANDEN" : "FEHLT");
    s_vor_n = 0;

    if (a_ohne < 0) { printf("FAIL: ohne das Flag muesste der Kartenleser da sein\n"); return 1; }
    if (a_mit >= 0) { printf("FAIL: mit dem Flag muesste der Kartenleser weg sein\n"); return 1; }
    printf("OK  flag(3,139) ist KEIN brauchbarer Marker fuer ROOM11E0/11E1: der Intro-Raum\n"
           "    setzt es vorab, und dann gibt es den Kartenleser gar nicht mehr.\n");
    return 0;
}

int main(int argc, char **argv)
{
    const char *nur = (argc >= 2) ? argv[1] : NULL;
    int rc = 0;
    if (!nur || strcmp(nur, "a") == 0) rc |= teil_a();
    if (!nur || strcmp(nur, "b") == 0) rc |= teil_b();
    if (!nur || strcmp(nur, "c") == 0) rc |= teil_c();
    free(s_raw);
    printf("\n%s\n", rc ? "=== SONDE: FAIL ===" : "=== SONDE: OK ===");
    return rc;
}
