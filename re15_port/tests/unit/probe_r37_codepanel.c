/* probe_r37_codepanel.c — Runde 37 Spur E "codepanel" (analysis/befunde_runde37/E_codepanel.md).
 *
 * Nutzer (AUFTRAG.md #9): "so lange die Code noch falsch eingegeben ist unten das blaue Licht leuchtet. Ist er dann
 * richtig eingegeben das grüne Licht. Die Kamera darf noch auf den Panels bleiben wenn der Piep und unlock sound kommt."
 *
 * Teile (je ein add_test):
 *   anker    Original-Bytes: sub19-Schwanz, sub17-Cut, sub01-Schloss-Flag in allen sechs RDTs; RE1.5 ROOM1100 sub02
 *            (Kamera NACH der Nachricht); RE2 ROOM2110 sub08 / ROOM4070 sub08 (Lampe am Piep, Kamera nach dem
 *            Aufschliessen); Kunst LAMPE2130.TIM == RE2 room2110/esp16.tim, Zeile 1 blau / 2 gruen. Die Port-Tabelle
 *            (re15_codepanel_r37_panel) muss mit diesen unabhaengig hier stehenden Zahlen uebereinstimmen.
 *   kamera   E2 am ECHTEN Skriptweg: Tastenfeld auf (sub17), Code mit Cursor + Quadrat eingeben (10D0 4312, 11E0 0513,
 *            1230 5632; dazu 10D1/11E1/1231). Piep und Aufschliessen kommen, waehrend der angezeigte Cut der Panel-Cut
 *            ist; die Kamera geht erst nach dem Schliessen der Nachricht zurueck (Cut von vorher, Auto-Kamera an);
 *            Flags/Tuer wie im Original. Frueh geschlossene Nachricht (Quadrat gehalten): sub19 wartet bis zum
 *            Aufschliess-Ton. GEGENPROBE mit abgeschaltetem Haken: die Kamera geht im Piep-Bild (= Befund vorher).
 *   lampe    E1: blau an der blau (10D0) bzw. rot (SECUGUARD) gemalten Lampe, solange der Code nicht richtig ist (auch
 *            nach Teil-Eingabe und nach "Wrong code"), gruen an der rot (10D0) bzw. gruen (SECUGUARD) gemalten Lampe ab
 *            dem Bild nach dem Piep; nichts ausserhalb des Panel-Cuts; Zellfolge rund 5,7,6,7 / Quadrat 3,4.
 *   zeichner E1 im Bild: der echte PC-Zeichner (platform/pc/src/codepanel_r37_pc.c, einkompiliert) auf dem echten
 *            Panel-Hintergrund (MDEC aus ROOM10D.BSS / ROOM11E.BSS): Mittel ueber das gemalte Glas der leuchtenden Lampe
 *            blau- bzw. gruen-dominant, die andere Lampe unveraendert.
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
#include "re15_engine.h"
#include "re15_bss.h"
#include "re15_codepanel_r37.h"
#include "re15_aufschliessen_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
#ifndef RE15_RE2_RDT_DIR
#define RE15_RE2_RDT_DIR "../info/re2leon/PL0/RDT"
#endif

extern scd_vm_t g_scd;

static int g_fail = 0;
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
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

/* ---- Stubs fuer den einkompilierten PC-Zeichner ------------------------------------------------------- */
static uint32_t s_fb[SCREEN_XRES * SCREEN_YRES];
uint32_t *re15_pc_framebuffer(void) { return s_fb; }
uint8_t *re15_pc_read_re2(const char *rel, int *size)
{
    char p[700]; size_t n = 0;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_RE2_DIR, rel);
    uint8_t *b = slurp(p, &n);
    if (b && size) *size = (int)n;
    return b;
}
extern void re15_codepanel_r37_pc_zeichnen(int cut_gezeigt);

/* ==================== SOLL-Zahlen (unabhaengig vom Port-Kopf, je mit Beleg) =========================== */
typedef struct {
    unsigned raum; const char *datei;
    unsigned x;            /* sub19 Cut_old (selbst gelesen) */
    unsigned s17;          /* sub17 Cut_chg */
    int      cut;
    unsigned s01_set;      /* sub01 Set(3,F,1) */
    int      flag;
    int      cursor_obj;   /* Work_set(3, n) in sub02..05 */
    int      code_slot[4]; /* AOT-Platz je Ziffer (Rechteck-Ecke + 1025 = Mitte) */
    int      falsch_slot;  /* eine Ziffer, die an keiner Stelle des Codes passt */
} soll_t;
static const soll_t k_soll[6] = {
    /* 10D0: 4312 = Plaetze 6,5,3,4 (sub09 @0x01646 / sub08 @0x015FA / sub06 @0x01562 / sub07 @0x015AE), Cursor obj 1
     * (sub02 @0x01532 `2e 03 01`), "9" = Platz 11 */
    { 0x10D0, "STAGE1/ROOM10D0.RDT", 0x0199A, 0x018D6, 11, 0x0152A,  50, 1, { 6, 5, 3, 4 }, 11 },
    { 0x10D1, "STAGE1/ROOM10D1.RDT", 0x01984, 0x018C0, 11, 0x01514,  50, 1, { 6, 5, 3, 4 }, 11 },
    /* 11E0: 0513 = Plaetze 17,12,8,10, Cursor obj 2 (sub02 @0x01B1A `2e 03 02`), "9" = Platz 16 */
    { 0x11E0, "STAGE1/ROOM11E0.RDT", 0x01F82, 0x01EBE, 15, 0x01A8A, 139, 2, { 17, 12, 8, 10 }, 16 },
    { 0x11E1, "STAGE1/ROOM11E1.RDT", 0x01F82, 0x01EBE, 15, 0x01A8A, 139, 2, { 17, 12, 8, 10 }, 16 },
    /* 1230: 5632 = Plaetze 11,12,9,8 (Zettel msg 10 "5632"), Cursor obj 0 (sub02 @0x00FFA `2e 03 00`), "7" = Platz 13 */
    { 0x1230, "STAGE1/ROOM1230.RDT", 0x01462, 0x0139E,  9, 0x00FF2, 137, 0, { 11, 12, 9, 8 }, 13 },
    { 0x1231, "STAGE1/ROOM1231.RDT", 0x01462, 0x0139E,  9, 0x00FF2, 137, 0, { 11, 12, 9, 8 }, 13 },
};
static const uint8_t k_sig[18] = { 0x2a, 0x00, 0x3c, 0x01, 0x2b, 0x05, 0xff, 0xff, 0x02, 0x00,
                                   0x22, 0x01, 0x1b, 0x00, 0x22, 0x02, 0x07, 0x00 };

/* Lampen: Glasmitte gemessen (Dossier §0) -> Ecke floor(Mitte - Kante/2 + 0.5); Kanten 48/64 (rund, RE2 4070) und
 * 26 (Quadrat, RE2 2130). */
typedef struct { int x0, y0, kante; } lage_t;

/* ==================== Spielbild wie die PC-Hauptschleife ================================================ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static unsigned           s_bild = 0;

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
    s_bild++;
}

static int room_boot(const soll_t *s)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, s->datei);
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
    g_current_room_id = (uint16_t)s->raum; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0; s_bild = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 1;
}

/* ---- Lampen-Buchfuehrung je Bild ---- */
static re15_cp_lampe_t s_l;
static int s_l_an = 0;
static void lampe_lesen(void) { s_l_an = re15_codepanel_r37_lampe(s_bild, s_shown, &s_l); }

/* Ein Bild wie die PC-Hauptschleife: ERST Hintergrund + Lampe (main.c, vor scd_vm_tick, mit dem bisher angezeigten
 * Cut), DANN VM + Spielschritt. s_l/s_l_an gehoeren damit zum Bild s_l_bild, die Ereignisse des Spielschritts ebenso. */
static int s_l_bild = -1;
static void bild(uint16_t held, uint16_t edge) { s_l_bild = (int)s_bild; lampe_lesen(); frame(held, edge); }

static int nachricht_schliessen(int max_bilder)
{
    for (int f = 0; f < max_bilder; f++) {
        if (!g_scd.message_active) return f;
        uint16_t b = (f % 4 == 0) ? RE15_PAD_BIT_SQUARE : 0;
        bild(b, b);
    }
    return -1;
}

static int cursor_prop(int obj)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == obj) return i;
    return -1;
}

/* Cursor per D-Pad auf die Mitte des AOT-Platzes (sub02..05: UP z+200 / DOWN z-200 / RIGHT x+200 / LEFT x-200,
 * z.B. ROOM10D0 @0x01536 `2f 02 c8 00`, @0x01542 `2f 02 38 ff`, @0x0154E `2f 00 c8 00`, @0x0155A `2f 00 38 ff`). */
static int cursor_zu(int obj, int slot)
{
    int ci = cursor_prop(obj); if (ci < 0) return 0;
    const re15_aot_t *a = &g_aot.slots[slot];
    int32_t zx = a->x, zz = a->z;     /* Port: Mitte (re15_object_notch_update CENTRE-Test) */
    for (int f = 0; f < 400; f++) {
        int32_t dx = zx - g_scd.props[ci].x, dz = zz - g_scd.props[ci].z;
        uint16_t b = 0;
        if (dx > 150) b = RE15_PAD_BIT_RIGHT; else if (dx < -150) b = RE15_PAD_BIT_LEFT;
        else if (dz > 150) b = RE15_PAD_BIT_UP; else if (dz < -150) b = RE15_PAD_BIT_DOWN;
        if (!b) { bild(0, 0); bild(0, 0); return 1; }
        bild(b, b);
    }
    return 0;
}

static int warten_auf_flag(int bank, int bit, int wert, int max)
{
    for (int f = 0; f < max; f++) { if (re15_game_flag_get((uint8_t)bank, (uint8_t)bit) == wert) return f; bild(0, 0); }
    return -1;
}

/* Tastenfeld oeffnen wie sub20 @0x019CA `04 ff 18 11` (nach "You've used the ... Keycard."): sub17 als Faden. */
static int panel_auf(const soll_t *s)
{
    int slot = -1;
    for (int i = 2; i < SCD_THREAD_COUNT; i++) if (!g_scd.threads[i].active) { slot = i; break; }
    if (slot < 0 || scd_thread_start(slot, s_rdt.sub_scd[17]) != 0) return 0;
    for (int f = 0; f < 10 && !g_scd.message_active; f++) bild(0, 0);
    if (nachricht_schliessen(400) < 0) return 0;           /* msg 0 "Enter the first number." */
    return warten_auf_flag(5, 1, 1, 30) >= 0;              /* sub17 @0x018F0 Set(5,1,1) */
}

/* Eine Ziffer: Cursor hin, Quadrat ein Bild (sub01 `51 01 40 00` liest das gehaltene Wort). */
static int ziffer(const soll_t *s, int slot)
{
    if (!cursor_zu(s->cursor_obj, slot)) return 0;
    bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    bild(0, 0);
    return 1;
}

/* ==================== Teil anker ========================================================================= */
static int bytes_gleich(const uint8_t *d, size_t n, size_t off, const uint8_t *soll, size_t len)
{
    return d && off + len <= n && memcmp(d + off, soll, len) == 0;
}

static void teil_anker(void)
{
    printf("\n=== anker: Original-Bytes RE1.5/RE2 + Port-Tabelle ===\n");
    for (int i = 0; i < 6; i++) {
        const soll_t *s = &k_soll[i];
        char rp[600]; size_t n = 0;
        snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, s->datei);
        uint8_t *d = slurp(rp, &n);
        char t[200];
        snprintf(t, sizeof t, "ROOM%04X sub19 @0x%05X = 2a 00 3c 01 2b 05 ff ff 02 00 22 01 1b 00 22 02 07 00", s->raum, s->x);
        CHECK(t, bytes_gleich(d, n, s->x, k_sig, sizeof k_sig));
        uint8_t c17[2] = { 0x29, (uint8_t)s->cut };
        snprintf(t, sizeof t, "ROOM%04X sub17 @0x%05X Cut_chg %d", s->raum, s->s17, s->cut);
        CHECK(t, bytes_gleich(d, n, s->s17, c17, 2));
        uint8_t set[4] = { 0x22, 0x03, (uint8_t)s->flag, 0x01 };
        uint8_t exe[4] = { 0x04, 0xff, 0x18, 0x13 };
        snprintf(t, sizeof t, "ROOM%04X sub01 @0x%05X Evt_exec sub19 + Set(3,%d,1)", s->raum, s->s01_set - 4, s->flag);
        CHECK(t, bytes_gleich(d, n, s->s01_set - 4, exe, 4) && bytes_gleich(d, n, s->s01_set, set, 4));
        const re15_cp_panel_t *p = re15_codepanel_r37_raum(s->raum);
        snprintf(t, sizeof t, "Port-Tabelle ROOM%04X: Cut %d, Flag (3,%d), X @0x%05X", s->raum, s->cut, s->flag, s->x);
        CHECK(t, p && p->cut == s->cut && p->flag_bit == s->flag && p->sub19_x == s->x);
        free(d);
    }
    {   /* RE1.5 ROOM1100 sub02: Message_on 2 / Evt_next / Set(2,7,0) / Cut_chg 6 / Cut_auto 1 */
        char rp[600]; size_t n = 0;
        snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1100.RDT", RE15_ASSET_PSX_DIR);
        uint8_t *d = slurp(rp, &n);
        static const uint8_t k1100[] = { 0x2b, 0x02, 0xff, 0xff, 0x02, 0x00, 0x22, 0x02, 0x07, 0x00, 0x29, 0x06, 0x3c, 0x01 };
        CHECK("RE1.5 ROOM1100 sub02 @0x00CBA: Message_on 2, Evt_next, Set(2,7,0), DANN Cut_chg 6 + Cut_auto 1",
              bytes_gleich(d, n, 0x00CBA, k1100, sizeof k1100));
        free(d);
    }
    {   /* RE2 ROOM2110 sub08 / ROOM4070 sub08 */
        char rp[600]; size_t n = 0;
        snprintf(rp, sizeof rp, "%s/ROOM2110.RDT", RE15_RE2_RDT_DIR);
        uint8_t *d = slurp(rp, &n);
        if (!d) { printf("  [SKIP] RE2 ROOM2110.RDT fehlt (%s)\n", rp); }
        else {
            static const uint8_t piep[3] = { 0x36, 0x02, 0x0d }, blau[4] = { 0x64, 0x01, 0x16, 0x08 },
                                 auf[3] = { 0x36, 0x02, 0x0b }, cut[4] = { 0x29, 0x09, 0x3c, 0x01 },
                                 kill[4] = { 0x65, 0x01, 0x65, 0x02 };
            CHECK("RE2 ROOM2110 @0x01B5C Se_on(2,0x0D) Piep", bytes_gleich(d, n, 0x01B5C, piep, 3));
            CHECK("RE2 ROOM2110 @0x01B68 Lampen aus / @0x01B6C Lampe Zeile 1 (Unterindex 0x08) im Piep-Bild",
                  bytes_gleich(d, n, 0x01B68, kill, 4) && bytes_gleich(d, n, 0x01B6C, blau, 4));
            CHECK("RE2 ROOM2110 @0x01B90 Se_on(2,0x0B) Aufschliessen, ERST DANN @0x01B9C Cut_chg 9 + Cut_auto 1",
                  bytes_gleich(d, n, 0x01B90, auf, 3) && bytes_gleich(d, n, 0x01B9C, cut, 4));
            free(d);
        }
        snprintf(rp, sizeof rp, "%s/ROOM4070.RDT", RE15_RE2_RDT_DIR);
        d = slurp(rp, &n);
        if (!d) { printf("  [SKIP] RE2 ROOM4070.RDT fehlt\n"); }
        else {
            static const uint8_t an[4] = { 0x64, 0x03, 0x16, 0x0c }, aus[2] = { 0x65, 0x02 }, piep[3] = { 0x36, 0x02, 0x0c };
            CHECK("RE2 ROOM4070 @0x00DC2 runde Lampe blau an / @0x00DD2 rote aus / @0x00DD4 Piep",
                  bytes_gleich(d, n, 0x00DC2, an, 4) && bytes_gleich(d, n, 0x00DD2, aus, 2) &&
                  bytes_gleich(d, n, 0x00DD4, piep, 3));
            free(d);
        }
    }
    {   /* Kunst */
        char rp[600]; size_t n = 0, m = 0;
        snprintf(rp, sizeof rp, "%s/LAMPE2130.TIM", RE15_ASSET_RE2_DIR);
        uint8_t *t = slurp(rp, &n);
        char rq[600];
        snprintf(rq, sizeof rq, "%s/room2110/esp16.tim", RE15_RE2_RDT_DIR);
        uint8_t *e = slurp(rq, &m);
        if (e) CHECK("LAMPE2130.TIM bytegleich RE2 room2110/esp16.tim", t && n == m && memcmp(t, e, n) == 0);
        else   printf("  [SKIP] room2110/esp16.tim fehlt\n");
        /* CLUT-Zeile 1 / 2, Eintrag 6 (Datei 8+12+(Zeile*16+6)*2): 0xd1c9 = (72,112,160) blau, 0xa729 = (72,200,72) gruen */
        CHECK("CLUT-Zeile 1 Eintrag 6 = 0xd1c9 (blau), Zeile 2 Eintrag 6 = 0xa729 (gruen)",
              t && n > 100 && (t[20 + (16 + 6) * 2] | (t[21 + (16 + 6) * 2] << 8)) == 0xd1c9 &&
              (t[20 + (32 + 6) * 2] | (t[21 + (32 + 6) * 2] << 8)) == 0xa729);
        free(t); free(e);
    }
}

/* ==================== Teil kamera + lampe (ein Lauf je Raum) ============================================ */
typedef struct {
    int piep_bild, auf_bild, flag_bild, freigabe_bild;
    int cut_bei_piep, cut_bei_auf, cut_nach;
    int prev_cut;
    int gruen_ab, blau_bis;          /* erstes Bild gruen, letztes Bild blau */
    int lampe_ausserhalb;            /* Lampe ausserhalb des Panel-Cuts gesehen */
    int fehler_lage;
    int ok;
} lauf_t;

/* Erwartete Lampenlage (unabhaengig vom Port-Kopf). */
static int lage_ok(unsigned raum, const re15_cp_lampe_t *l)
{
    int rund = (raum == 0x10D0 || raum == 0x10D1);
    if (rund) {
        /* blau: Mitte (168.5, 76.5); gruen: (121.5, 76.5); Kante 48 (Zelle 5, S 24) bzw. 64 (S 32) */
        int k = (l->zelle == 5) ? 48 : 64;
        int mx2 = (l->zeile == RE15_CP_ZEILE_BLAU) ? 337 : 243, my2 = 153;
        return l->kante == k && l->x0 == (mx2 - k + 1) / 2 && l->y0 == (my2 - k + 1) / 2;
    }
    /* SECUGUARD: blau auf ROT (128.5, 174.5), gruen auf GRUEN (180.5, 174.5), Kante 26 */
    int mx2 = (l->zeile == RE15_CP_ZEILE_BLAU) ? 257 : 361, my2 = 349;
    return l->kante == 26 && l->x0 == (mx2 - 26 + 1) / 2 && l->y0 == (my2 - 26 + 1) / 2;
}

/* Code eingeben und alles messen. mit_haken = 0: Gegenprobe (Haken aus = Verhalten vorher).
 * frueh = 1: Quadrat ab dem Piep gehalten (Nachricht so frueh wie moeglich zu). */
static lauf_t lauf(const soll_t *s, int mit_haken, int frueh)
{
    lauf_t r; memset(&r, 0, sizeof r);
    r.piep_bild = r.auf_bild = r.flag_bild = r.freigabe_bild = r.gruen_ab = r.blau_bis = -1;
    if (!room_boot(s)) { printf("  FAIL: %s fehlt\n", s->datei); return r; }
    if (!mit_haken) g_re15_codepanel_anker_n = 0;
    if (!panel_auf(s)) { printf("  FAIL: Tastenfeld oeffnet nicht\n"); return r; }
    r.prev_cut = (int)g_scd.cam_id_prev;
    unsigned fr0 = re15_codepanel_r37_freigaben();
    for (int k = 0; k < 4; k++) {
        if (s_l_an && s_l.zeile == RE15_CP_ZEILE_BLAU) r.blau_bis = s_l_bild;
        if (!ziffer(s, s->code_slot[k])) { printf("  FAIL: Ziffer %d nicht erreicht\n", k + 1); return r; }
        if (k < 3) {
            for (int f = 0; f < 20 && !g_scd.message_active; f++) bild(0, 0);
            if (s_l_an && s_l.zeile == RE15_CP_ZEILE_BLAU) r.blau_bis = s_l_bild;
            if (nachricht_schliessen(400) < 0) { printf("  FAIL: Ziffern-Nachricht bleibt\n"); return r; }
            if (warten_auf_flag(5, 1, 1, 30) < 0) { printf("  FAIL: Tastenfeld nicht wieder frei\n"); return r; }
        }
    }
    unsigned z0 = g_re15_auf_zaehler;
    int halten = 0;
    for (int f = 0; f < 600; f++) {
        uint16_t b = 0;
        if (frueh && r.piep_bild >= 0) b = RE15_PAD_BIT_SQUARE;                 /* gehalten */
        else if (!frueh && r.auf_bild >= 0 && (int)s_bild >= r.auf_bild + 40)
            b = ((s_bild & 3) == 0) ? RE15_PAD_BIT_SQUARE : 0;                    /* normal schliessen */
        unsigned za = g_re15_auf_zaehler;
        int flag_vorher = re15_game_flag_get(3, (uint8_t)s->flag);
        int gezeigt_vorher = s_shown;                 /* Cut, mit dem das Bild jetzt gezeichnet wird */
        bild(b, b);
        int jetzt = s_l_bild;
        /* Ereignisse des Spielschritts von Bild `jetzt`; s_shown = Cut des NAECHSTEN Bilds */
        if (!flag_vorher && re15_game_flag_get(3, (uint8_t)s->flag)) r.flag_bild = jetzt;
        if (g_re15_auf_zaehler != za) {
            if (g_re15_auf_letzter == 1 && r.piep_bild < 0) { r.piep_bild = jetzt; r.cut_bei_piep = s_shown; }
            if (g_re15_auf_letzter == 2 && r.auf_bild < 0)  { r.auf_bild  = jetzt; r.cut_bei_auf  = s_shown; }
        }
        if (re15_codepanel_r37_haelt()) halten = 1;
        if (s_l_an) {                                  /* Lampe des Bilds `jetzt` */
            if (s_l.zeile == RE15_CP_ZEILE_GRUEN && r.gruen_ab < 0) r.gruen_ab = jetzt;
            if (s_l.zeile == RE15_CP_ZEILE_BLAU) r.blau_bis = jetzt;
            if (!lage_ok(s->raum, &s_l)) r.fehler_lage++;
            if (gezeigt_vorher != s->cut) r.lampe_ausserhalb++;
        }
        if (r.freigabe_bild < 0 && r.piep_bild >= 0 && s_shown != s->cut) { r.freigabe_bild = jetzt; r.cut_nach = s_shown; }
        if (r.freigabe_bild >= 0 && !g_scd.message_active && jetzt > r.freigabe_bild + 5) break;
    }
    (void)z0; (void)halten; (void)fr0;
    r.ok = 1;
    return r;
}

static void teil_kamera_lampe(int nur_lampe)
{
    printf("\n=== %s ===\n", nur_lampe ? "lampe: blau solange falsch, gruen wenn richtig (E1)"
                                     : "kamera: Panel bleibt bei Piep und Aufschliessen (E2)");
    for (int i = 0; i < 6; i++) {
        const soll_t *s = &k_soll[i];
        char t[260];
        printf(" -- ROOM%04X (Panel-Cut %d, Flag (3,%d)) --\n", s->raum, s->cut, s->flag);
        lauf_t r = lauf(s, 1, 0);
        if (!r.ok) { g_fail++; continue; }
        printf("    Piep Bild %d (Cut %d), Flag Bild %d, Aufschliessen Bild %d (Cut %d), Kamera zurueck Bild %d -> Cut %d "
               "(vorher %d); blau bis %d, gruen ab %d; Wartebilder %d\n",
               r.piep_bild, r.cut_bei_piep, r.flag_bild, r.auf_bild, r.cut_bei_auf, r.freigabe_bild, r.cut_nach,
               r.prev_cut, r.blau_bis, r.gruen_ab, re15_codepanel_r37_wartebilder());
        if (!nur_lampe) {
            snprintf(t, sizeof t, "ROOM%04X: Piep kommt, der angezeigte Cut ist der Panel-Cut %d", s->raum, s->cut);
            CHECK(t, r.piep_bild >= 0 && r.cut_bei_piep == s->cut);
            snprintf(t, sizeof t, "ROOM%04X: Aufschliessen +15 nach dem Piep (RE2 Sleep 15 @0x01B58), noch auf dem Panel",
                     s->raum);
            CHECK(t, r.auf_bild == r.piep_bild + 15 && r.cut_bei_auf == s->cut);
            snprintf(t, sizeof t, "ROOM%04X: Kamera erst nach dem Aufschliessen zurueck, auf den Cut von vorher (%d)",
                     s->raum, r.prev_cut);
            CHECK(t, r.freigabe_bild > r.auf_bild && r.cut_nach == r.prev_cut);
            snprintf(t, sizeof t, "ROOM%04X: Endzustand wie Original (Flag (3,%d)=1, Eingabe frei (2,7)=0, Auto-Kamera an)",
                     s->raum, s->flag);
            CHECK(t, re15_game_flag_get(3, (uint8_t)s->flag) == 1 && re15_game_flag_get(2, 7) == 0 &&
                     g_scd.cut_auto_enabled == 1 && !re15_codepanel_r37_haelt());
            /* Gegenprobe: ohne Haken geht die Kamera im Piep-Bild (= Befund vorher) */
            lauf_t g = lauf(s, 0, 0);
            printf("    GEGENPROBE ohne Haken: Piep Bild %d -> angezeigter Cut %d, Kamera zurueck Bild %d\n",
                   g.piep_bild, g.cut_bei_piep, g.freigabe_bild);
            snprintf(t, sizeof t, "ROOM%04X GEGENPROBE: ohne Haken verlaesst die Kamera das Panel vor dem Aufschliessen",
                     s->raum);
            CHECK(t, g.ok && g.freigabe_bild >= 0 && g.freigabe_bild <= g.piep_bild + 1 && g.cut_bei_auf != s->cut);
            if (i == 0 || i == 2 || i == 4) {
                lauf_t q = lauf(s, 1, 1);
                printf("    FRUEH (Quadrat gehalten): Piep %d, Aufschliessen %d, Kamera zurueck %d, gewartet %d Bilder\n",
                       q.piep_bild, q.auf_bild, q.freigabe_bild, re15_codepanel_r37_wartebilder());
                snprintf(t, sizeof t, "ROOM%04X FRUEH: Nachricht vor +15 zu -> sub19 wartet, Kamera erst nach dem "
                         "Aufschliessen", s->raum);
                CHECK(t, q.ok && q.auf_bild == q.piep_bild + 15 && q.freigabe_bild > q.auf_bild &&
                         re15_codepanel_r37_wartebilder() > 0);
            }
        } else {
            snprintf(t, sizeof t, "ROOM%04X: blau bis zum Piep-Bild, gruen ab dem naechsten Bild", s->raum);
            CHECK(t, r.blau_bis >= 0 && r.gruen_ab >= 0 && r.blau_bis == r.piep_bild && r.gruen_ab == r.piep_bild + 1);
            snprintf(t, sizeof t, "ROOM%04X: Lampe an der erwarteten Lage/Kante (blau %s, gruen %s), %d Abweichungen",
                     s->raum, (s->raum >> 4) == 0x10D ? "blau gemalt" : "rot gemalt",
                     (s->raum >> 4) == 0x10D ? "rot gemalt" : "gruen gemalt", r.fehler_lage);
            CHECK(t, r.fehler_lage == 0);
            snprintf(t, sizeof t, "ROOM%04X: keine Lampe ausserhalb des Panel-Cuts", s->raum);
            CHECK(t, r.lampe_ausserhalb == 0);
        }
    }
    if (nur_lampe) {
        /* falscher Code: bleibt blau, Kamera ueber sub16 wie bisher */
        const soll_t *s = &k_soll[0];
        printf(" -- ROOM10D0 falscher Code (9999) --\n");
        if (room_boot(s) && panel_auf(s)) {
            int gruen = 0, blau = 0, msg4 = 0;
            for (int k = 0; k < 4; k++) {
                if (!ziffer(s, s->falsch_slot)) { g_fail++; break; }
                for (int f = 0; f < 20 && !g_scd.message_active; f++) bild(0, 0);
                if (g_scd.message_active && g_scd.message_id == 4) msg4 = 1;
                if (s_l_an) { if (s_l.zeile == RE15_CP_ZEILE_GRUEN) gruen++; else blau++; }
                nachricht_schliessen(400);
                if (k < 3) warten_auf_flag(5, 1, 1, 30);
            }
            for (int f = 0; f < 40; f++) { bild(0, 0); if (s_l_an && s_l.zeile == RE15_CP_ZEILE_GRUEN) gruen++; }
            printf("    msg4=%d blau-Bilder %d gruen-Bilder %d Panel-Cut jetzt %d Flag(3,50)=%d\n", msg4, blau, gruen,
                   s_shown, re15_game_flag_get(3, 50));
            CHECK("ROOM10D0 falscher Code: \"Wrong code, try again.\" (msg 4) bei BLAUER Lampe, nie gruen",
                  msg4 && blau > 0 && gruen == 0);
            CHECK("ROOM10D0 falscher Code: Panel zu (sub16 Cut_old), Schloss bleibt zu", s_shown != 11 &&
                  re15_game_flag_get(3, 50) == 0);
        } else { printf("  FAIL: ROOM10D0 Panel\n"); g_fail++; }
        /* Zellfolge (ESP-0x16 Anim-Saetze): rund 5,7,6,7 / Quadrat 3,4 ab dem Einschaltbild */
        {
            static const soll_t *rs[2] = { &k_soll[0], &k_soll[2] };
            static const int soll_rund[5] = { 5, 7, 6, 7, 5 }, soll_quad[4] = { 3, 4, 3, 4 };
            for (int j = 0; j < 2; j++) {
                room_boot(rs[j]);
                re15_cp_lampe_t l; int ok = 1, n = j ? 4 : 5;
                (void)re15_codepanel_r37_lampe(1000, -1, &l);                     /* aus */
                for (int f = 0; f < n; f++) {
                    if (!re15_codepanel_r37_lampe(1001 + (unsigned)f, rs[j]->cut, &l)) { ok = 0; break; }
                    if (l.zelle != (j ? soll_quad[f] : soll_rund[f])) ok = 0;
                    if (!j && l.s != (l.zelle == 5 ? 24 : 32)) ok = 0;
                }
                CHECK(j ? "Quadrat: Zellen 3,4,3,4 ab dem Einschaltbild (Saetze 4..6 `03 01 01 20`/`04 01 01 20`/`04 01 ff 20`)"
                        : "rund: Zellen 5(S24),7,6,7,5 ab dem Einschaltbild (Saetze 7..11 `05 01 01 18` .. `07 01 ff 20`)",
                      ok);
                CHECK("nicht im Panel-Cut / anderer Cut: keine Lampe", !re15_codepanel_r37_lampe(2000, 0, &l));
            }
            soll_t fremd = k_soll[0]; fremd.raum = 0x10C0; fremd.datei = "STAGE1/ROOM10C0.RDT";
            if (room_boot(&fremd)) {
                re15_cp_lampe_t l;
                CHECK("anderer Raum (ROOM10C0) mit Cut 11: keine Lampe", !re15_codepanel_r37_lampe(10, 11, &l) &&
                      g_re15_codepanel_anker_n == 0);
            }
        }
    }
}

/* ==================== Teil zeichner ====================================================================== */
static int bss_cut(const char *datei, int cut, uint8_t *rgb)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, datei);
    uint8_t *d = slurp(p, &n);
    if (!d || (size_t)(cut + 1) * 0x10000u > n) { free(d); return -1; }
    re15_bss_chunk_t ch;
    int rv = -1;
    if (re15_bss_parse_chunk(d + (size_t)cut * 0x10000u, 0x10000, &ch) && re15_bss_chunk_has_video(&ch)) {
        size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
        int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
        int k = co ? re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant,
                                         ch.version, co, cap) : -1;
        if (k >= 0 && re15_bss_mdec_decode(co, (size_t)k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) == 0) rv = 0;
        free(co);
    }
    free(d);
    return rv;
}

static void fb_aus_rgb(const uint8_t *rgb)
{
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++)
        s_fb[i] = ((uint32_t)rgb[i * 3] << 24) | ((uint32_t)rgb[i * 3 + 1] << 16) | ((uint32_t)rgb[i * 3 + 2] << 8) | 0xffu;
}

static void mittel(int x0, int y0, int x1, int y1, int out[3])
{
    long s[3] = { 0, 0, 0 }; int n = 0;
    for (int y = y0; y <= y1; y++) for (int x = x0; x <= x1; x++) {
        uint32_t p = s_fb[y * SCREEN_XRES + x];
        s[0] += (p >> 24) & 0xff; s[1] += (p >> 16) & 0xff; s[2] += (p >> 8) & 0xff; n++;
    }
    for (int c = 0; c < 3; c++) out[c] = (int)(s[c] / (n ? n : 1));
}

static void teil_zeichner(void)
{
    printf("\n=== zeichner: echter PC-Zeichner auf dem echten Panel-Hintergrund ===\n");
    static uint8_t rgb[RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT * 3];
    struct { const soll_t *s; const char *bss; int an[4], aus[4]; int gruen_an[4], gruen_aus[4]; } f[2] = {
        /* 10D0 Cut 11: blau-Glas x 164..173 / y 72..81, rot-Glas x 116..127 / y 72..81 */
        { &k_soll[0], "STAGE1/ROOM10D.BSS", { 164, 72, 173, 81 }, { 116, 72, 127, 81 }, { 116, 72, 127, 81 }, { 164, 72, 173, 81 } },
        /* 11E0 Cut 15: rot x 122..135 / gruen x 174..187, y 166..183 */
        { &k_soll[2], "STAGE1/ROOM11E.BSS", { 122, 166, 135, 183 }, { 174, 166, 187, 183 }, { 174, 166, 187, 183 }, { 122, 166, 135, 183 } },
    };
    for (int i = 0; i < 2; i++) {
        const soll_t *s = f[i].s;
        if (bss_cut(f[i].bss, s->cut, rgb) != 0) { printf("  FAIL: %s Cut %d\n", f[i].bss, s->cut); g_fail++; continue; }
        room_boot(s);
        for (int zustand = 0; zustand < 2; zustand++) {
            re15_game_flag_set(3, (uint8_t)s->flag, (uint8_t)zustand);
            int vorher_an[3], vorher_aus[3], nach_an[3], nach_aus[3];
            const int *an = zustand ? f[i].gruen_an : f[i].an, *aus = zustand ? f[i].gruen_aus : f[i].aus;
            int min_dom = 1000;
            for (int k = 0; k < 4; k++) {                      /* vier Bilder = alle Zellen der Folge */
                g_engine.frame_count++;                        /* der Zeichner taktet mit dem Bildzaehler */
                fb_aus_rgb(rgb);
                mittel(an[0], an[1], an[2], an[3], vorher_an); mittel(aus[0], aus[1], aus[2], aus[3], vorher_aus);
                re15_codepanel_r37_pc_zeichnen(s->cut);
                mittel(an[0], an[1], an[2], an[3], nach_an); mittel(aus[0], aus[1], aus[2], aus[3], nach_aus);
                int d = zustand ? nach_an[1] - (nach_an[0] > nach_an[2] ? nach_an[0] : nach_an[2])
                                : nach_an[2] - (nach_an[0] > nach_an[1] ? nach_an[0] : nach_an[1]);
                if (d < min_dom) min_dom = d;
                printf("    ROOM%04X %s Bild %d: Glas der Lampe vorher (%d,%d,%d) -> (%d,%d,%d); andere Lampe (%d,%d,%d) -> "
                       "(%d,%d,%d)\n", s->raum, zustand ? "GRUEN" : "BLAU", k, vorher_an[0], vorher_an[1], vorher_an[2],
                       nach_an[0], nach_an[1], nach_an[2], vorher_aus[0], vorher_aus[1], vorher_aus[2],
                       nach_aus[0], nach_aus[1], nach_aus[2]);
            }
            char t[200];
            snprintf(t, sizeof t, "ROOM%04X %s: leuchtende Lampe %s-dominant (min. Abstand %d >= 30)", s->raum,
                     zustand ? "richtig" : "falsch", zustand ? "gruen" : "blau", min_dom);
            CHECK(t, min_dom >= 30);
            fb_aus_rgb(rgb); re15_codepanel_r37_pc_zeichnen(-1);
            mittel(an[0], an[1], an[2], an[3], nach_an);
            mittel(an[0], an[1], an[2], an[3], vorher_an);
            snprintf(t, sizeof t, "ROOM%04X %s: nicht im Panel-Cut gezeigt -> nichts gezeichnet", s->raum,
                     zustand ? "richtig" : "falsch");
            CHECK(t, nach_an[0] == vorher_an[0] && nach_an[1] == vorher_an[1] && nach_an[2] == vorher_an[2]);
        }
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "anker"))    teil_anker();
    if (alle || !strcmp(teil, "kamera"))   teil_kamera_lampe(0);
    if (alle || !strcmp(teil, "lampe"))    teil_kamera_lampe(1);
    if (alle || !strcmp(teil, "zeichner")) teil_zeichner();
    printf("\n%s: %d Fehler\n", g_fail ? "FAIL" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
