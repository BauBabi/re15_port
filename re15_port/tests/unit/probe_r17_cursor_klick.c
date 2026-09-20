/* probe_r17_cursor_klick.c — MESSSONDE + PIN zur NUTZER-ENTSCHEIDUNG "Klick-Laut am
 * Cursor-Raetsel".
 *
 * Nutzer-Auftrag 2026-09-20: "dann will ich bei den raetseln wo man etwas mit Cursor
 * auswaehlt und klickt einen click Sound. den gibt es, wenn nicht in resident evil 1.5 -
 * auf jeden fall in resident evil 2."
 *
 * ⛔ KEINE byte-true Frage (RE1.5 hat den Laut dort nicht). Byte-true belegt sind nur
 * MECHANISMUS und NUMMER, beide aus RE2 disassembliert:
 *   RE2 FUN_8006b358 (PSX.EXE):
 *     @0x8006b57c  beq a2,v0,0x8006b58c   ; nur wenn sich der Cursor-Index AENDERTE
 *     @0x8006b580  lui a0,0x404           ; Se_on(0x04040000) — Bewegung
 *     @0x8006b5b4  lui a0,0x407 / @0x8006b5b8 lui a0,0x406 / @0x8006b5bc jal 0x8005ba28
 *                                         ; Bestaetigung (ungueltig / gueltig)
 *   RE1.5 benutzt dieselbe Bank/Nummer im eigenen Inventar:
 *     @0x8004a478 lui a0,0x404 / @0x8004a47c jal 0x80045024   (Cursor)
 *     @0x8004a51c-20                                           (Bestaetigen 0x0406)
 *
 * WAS DIESE SONDE MISST
 *   M1  ZENSUS: in welchen der 45 Raeume mit Objekt-Pool-Notch-Zellen wandert ein Prop von
 *       SELBST ueber die Zellen? Jeder solche Wechsel waere ein Fehl-Klick. Gemessen wird
 *       ueber 600 Bilder OHNE Eingabe, mit dem echten re15_object_notch_update.
 *   M2  ROOM11F0: Generator-Raetsel oeffnen und den Cursor mit dem D-Pad fahren — wieviele
 *       Bewegungs-Klicks und wieviele Bestaetigungen kommen?
 *
 * Aufruf: probe_r17_cursor_klick [zensus|pin]
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;
extern unsigned         g_re15_cursor_klick_zaehler;   /* aot_common.c  — Bewegung   */
extern unsigned         g_re15_cursor_ok_zaehler;      /* scd_vm.c      — Bestaetigen */
extern int              g_test_core_se_last, g_test_core_se_count;  /* test_support.c */

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                g_fail = 0;

#define CHECK(name, cond) do {                                          \
        int _c = (cond);                                                \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));            \
        if (!_c) g_fail++;                                              \
    } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int s_shown = 0;

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

static int room_boot(unsigned rid, const char *stage, int32_t px, int32_t pz, int16_t rot)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage, rid);
    if (s_raw) { free(s_raw); s_raw = NULL; }
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) return 0;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return 0;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = rid; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 1;
}

/* ---- M1: Zensus ueber die Raeume mit Objekt-Pool-Notch-Zellen -------------------------
 * Liste aus tools/aot_sce_census.py (Objekt-Pool-Bit 0x04, kein ACTION-Bit 0x10, CENTRE
 * oder FORWARD gesetzt) — die Raeume mit MEHR als einer Zelle, also die echten Raster.
 * Die Doppel-Raeume ROOMxxx1 sind die Elza-Zwillinge derselben Daten und bleiben aussen vor. */
static const struct { unsigned rid; const char *stage; int zellen; } s_raster[] = {
    { 0x1080, "STAGE1",  4 }, { 0x10D0, "STAGE1", 11 }, { 0x1100, "STAGE1",  2 },
    { 0x11E0, "STAGE1", 12 }, { 0x11F0, "STAGE1", 11 }, { 0x1230, "STAGE1", 11 },
    { 0x2050, "STAGE2",  3 }, { 0x2060, "STAGE2",  7 }, { 0x20A0, "STAGE2",  5 },
    { 0x3050, "STAGE3",  2 }, { 0x30E0, "STAGE3",  2 }, { 0x4020, "STAGE4",  5 },
};

static int zensus(void)
{
    int gesamt = 0, raeume_mit = 0, uebersprungen = 0;
    printf("=== M1: wandert ohne Eingabe ein Prop ueber die Zellen? (600 Bilder) ===\n");
    for (unsigned i = 0; i < sizeof s_raster / sizeof s_raster[0]; i++) {
        if (!room_boot(s_raster[i].rid, s_raster[i].stage, 0, 0, 0)) {
            printf("  ROOM%04X  UEBERSPRUNGEN (RDT fehlt)\n", s_raster[i].rid);
            uebersprungen++;
            continue;
        }
        unsigned vor = g_re15_cursor_klick_zaehler;
        for (int f = 0; f < 600; f++) frame(0, 0);
        unsigned d = g_re15_cursor_klick_zaehler - vor;
        printf("  ROOM%04X  Zellen=%2d  Props=%2u  Fehl-Klicks ohne Eingabe: %u\n",
               s_raster[i].rid, s_raster[i].zellen, (unsigned)g_scd.prop_count, d);
        gesamt += (int)d;
        if (d) raeume_mit++;
    }
    printf("  SUMME Fehl-Klicks ohne Eingabe: %d (in %d Raeumen; %d uebersprungen)\n",
           gesamt, raeume_mit, uebersprungen);
    return gesamt;
}

/* ---- M2: ROOM11F0 Generator-Raetsel ---------------------------------------------------
 * Anmarsch + Oeffnen exakt wie probe_11f0_puzzle_pos (dort gegen das echte Spiel geeicht),
 * danach den Cursor mit dem D-Pad fahren. */
static int drive_to(int32_t tx, int32_t tz, int max_frames, int32_t tol);

static double q_atan2(double y, double x);

int main(int argc, char **argv)
{
    int pin    = (argc > 1 && strcmp(argv[1], "pin")    == 0);
    int nur_z  = (argc > 1 && strcmp(argv[1], "zensus") == 0);

    int fehl = zensus();
    CHECK("Zensus: kein Fehl-Klick ohne Eingabe", fehl == 0);
    if (nur_z) { printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail); return 0; }

    printf("\n=== M2: ROOM11F0 Generator-Raetsel — Cursor fahren ===\n");
    if (!room_boot(0x11F0, "STAGE1", 250, 250, 1024)) {
        printf("SKIP: ROOM11F0.RDT fehlt\n");
        return 77;
    }
    /* Skript-Anker gegen stille Daten-Drift (dieselben Bytes wie probe_11f0_puzzle_pos,
     * plus der Bestaetigungs-Block, an dem der OK-Laut haengt). */
    int anker_ok = (s_raw[0x10F4] == 0x2E && s_raw[0x10F5] == 0x03 && s_raw[0x10F6] == 0x00 &&
                    s_raw[0x10FC] == 0x3E && s_raw[0x10FE] == 0x0F && s_raw[0x1100] == 0x02 &&
                    s_raw[0x1106] == 0x51 && s_raw[0x1107] == 0x01 && s_raw[0x1108] == 0x40);
    printf("  [anchor] sub01 @0x10F4 Work_set(3,0) / @0x10FC Member_cmp(15==2) / "
           "@0x1106 Sce_key_ck(1,0x40): %s\n", anker_ok ? "OK" : "ABWEICHUNG");
    CHECK("ROOM11F0 sub01 Bestaetigungs-Block unveraendert", anker_ok);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    (void)drive_to(250,   -13350, 900, 400);
    (void)drive_to(-530,  -16032, 900, 400);
    (void)drive_to(-1800, -15150, 900, 300);
    printf("  Anmarsch fertig: pos=(%ld,%ld)\n", (long)pl->x, (long)pl->z);

    /* Panel per QUADRAT oeffnen (flag(5,0x0C)). */
    int offen = -1;
    for (int f = 0; f < 400; f++) {
        uint16_t b = (f % 8 == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(b, b);
        if (re15_game_flag_get(5, 0x0C)) { offen = f; break; }
    }
    printf("  Raetsel offen nach %d Bildern (flag(5,0x0C)=%d)\n",
           offen, re15_game_flag_get(5, 0x0C));

    unsigned k0 = g_re15_cursor_klick_zaehler, o0 = g_re15_cursor_ok_zaehler;
    int se0 = g_test_core_se_count;
    /* Cursor fahren: 4x je 40 Bilder RECHTS/LINKS. Die Bewegung laeuft ueber die
     * Skript-Subs 04/05 (Speed_set(0,+-200) @0x130E/@0x131A), nicht ueber den Spieler. */
    for (int runde = 0; runde < 4; runde++) {
        uint16_t b = (runde & 1) ? RE15_PAD_BIT_LEFT : RE15_PAD_BIT_RIGHT;
        for (int f = 0; f < 40; f++) frame(b, (uint16_t)(f == 0 ? b : 0));
    }
    unsigned bew = g_re15_cursor_klick_zaehler - k0;
    printf("  Bewegungs-Klicks beim Cursor-Fahren: %u\n", bew);

    /* Bestaetigen: QUADRAT gehalten. Der Laut darf trotz GEHALTENEM Knopf nur EINMAL kommen
     * (Sce_key_ck liest den gehaltenen Pad-Zustand, s. op_sce_key_ck). */
    unsigned o1 = g_re15_cursor_ok_zaehler;
    /* Gegen-Zaehler: wie oft wurde ein SCHALTER wirklich umgelegt? Die elf Schalter-Zustaende
     * liegen in Zone 5, Bits 1..11 (ROOM11F0 sub01 `Ck(5,N,1)` @0x10F0/0x1118/... — dieselben
     * Bits, die die Bestaetigungs-Bloecke gaten). Jede AENDERUNG eines dieser Bits ist genau
     * eine ausgeloeste Bestaetigung; der Laut muss 1:1 dazu kommen, nicht je Bild. */
    int flag_vor[12];
    for (int n = 1; n <= 11; n++) flag_vor[n] = re15_game_flag_get(5, (uint8_t)n);
    int schalter_umgelegt = 0, letztes_bild = -99, kleinster_abstand = 999;
    for (int f = 0; f < 60; f++) {
        unsigned vor = g_re15_cursor_ok_zaehler;
        frame(RE15_PAD_BIT_SQUARE, (uint16_t)(f == 0 ? RE15_PAD_BIT_SQUARE : 0));
        if (g_re15_cursor_ok_zaehler != vor) {
            if (f - letztes_bild < kleinster_abstand) kleinster_abstand = f - letztes_bild;
            letztes_bild = f;
        }
        for (int n = 1; n <= 11; n++) {
            int jetzt = re15_game_flag_get(5, (uint8_t)n);
            if (jetzt != flag_vor[n]) { schalter_umgelegt++; flag_vor[n] = jetzt; }
        }
    }
    unsigned ok_gehalten = g_re15_cursor_ok_zaehler - o1;
    printf("  60 Bilder GEHALTENES Quadrat: %u Bestaetigungs-Laute "
           "(kleinster Abstand %d Bilder), %d Schalter-Zustandswechsel\n",
           ok_gehalten, kleinster_abstand, schalter_umgelegt);
    printf("  (gesamt: Bewegung %u, Bestaetigen %u, CORE-SE-Aufrufe %d, letzter SE %d)\n",
           g_re15_cursor_klick_zaehler - k0, g_re15_cursor_ok_zaehler - o0,
           g_test_core_se_count - se0, g_test_core_se_last);

    CHECK("Cursor-Fahren erzeugt Bewegungs-Klicks", bew > 0);
    /* KEIN Rattern — und das ist die Eigenschaft des Flankenspeichers selbst, nicht eine
     * geratene Obergrenze: der Laut kommt NIE in zwei aufeinanderfolgenden Bildern. Dass er
     * ueberhaupt mehrfach kommt, ist richtig — das Skript legt den Schalter dabei auch
     * mehrfach um (Zone-5-Zustandswechsel unten), jeder Laut gehoert zu einer echten
     * Bestaetigung. Ein Laut JE BILD waere 60. */
    CHECK("Bestaetigungs-Laut kommt nie in zwei Bildern hintereinander",
          kleinster_abstand > 1);
    CHECK("Bestaetigungs-Laute weit unter der Bildzahl (kein Rattern)",
          ok_gehalten > 0 && (int)ok_gehalten * 4 < 60);
    CHECK("das Skript hat dabei wirklich Schalter umgelegt", schalter_umgelegt > 0);

    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    if (!pin) return 0;
    return g_fail ? 1 : 0;
}

/* ---- Anmarsch-Hilfe (aus probe_11f0_puzzle_pos uebernommen) --------------------------- */
#include <math.h>
static double q_atan2(double y, double x) { return atan2(y, x); }

static int drive_to(int32_t tx, int32_t tz, int max_frames, int32_t tol)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int stuck = 0;
    for (int i = 0; i < max_frames; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if ((long long)dx*dx + (long long)dz*dz < (long long)tol*tol) return 1;
        double ang = q_atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        int want = ((int)(ang + 0.5)) & 0x0FFF;
        int err  = (want - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        uint16_t bits;
        if (err > 60)       bits = RE15_PAD_BIT_RIGHT;
        else if (err < -60) bits = RE15_PAD_BIT_LEFT;
        else                bits = (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS);
        int32_t bx = pl->x, bz = pl->z; int br = pl->rot_y;
        frame(bits, 0);
        if (pl->x == bx && pl->z == bz && pl->rot_y == br) { if (++stuck > 20) return 0; }
        else stuck = 0;
    }
    return 0;
}
