/* test_birkin_szenen.c — Raumprobe der Beta-Birkin-Auftritte (include/re15_birkin_formen.h).
 *
 * 1. HANGAR ROOM50A0/50A1 (Runde 41 Spur H, Runde 42 "hangar_aussen"): von Westen (4070/4071) und ohne Monitorraum davor nie
 *    eine Szene, auch nicht ueber die Schnitte 0..13; beim Betreten aus dem Monitorraum 5030/5031 (re15_bf_raum_eintritt) setzt
 *    die Szene Birkin (0x30) + Spider-Man (0x2B), Riegel Set(2,7)/Set(1,27) und Cut 12 (Zonen aus) und spielt den Video-
 *    Zeitplan (birkin_formen.c): Ansprung ohne Spruenge, Halten ueber dem Kopf, Wurf, Aufschlag, Flug aus dem Bild, Birkins
 *    Gang auf die Kamera zu; keine Figur steht laenger als einen Takt starr; danach Riegel auf, Kamera zurueck auf Cut 0 der
 *    Nordtuer, Zonen an, kein Birkin, der Spider-Man liegt tot (Zustand 7, 600 Takte still, kein Ziel); Wiedereintritt leer.
 *    Merker: Bit 39 (alte Spielstaende), Bit 38+39, Bank 3 Bit 0x49 -> leer; Kraftraum unveraendert.
 * 2. BAHNSTEIG ROOM5090: der Auslieferungsspawn sub00 @0x0124A bleibt Typ 0x30 (keine Umtypung mehr);
 *    nach Todesphase 2 wird er zum G5 (0x36, grid 0x13), ohne den Birkin-Schalter z5:0x1C zu setzen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re15_finale_r42.h"
#include "re15_birkin_formen.h"
#include "re2_ems.h"
#include "re15_g5auftritt_r41.h"   /* Runde 41: G5-Auftritt nach RE2 room7040 sub06 */
#include "re15_esp.h"              /* gruenes Blut der Hangar-Szene: Plaetze pruefen */

extern int re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int re15_g5_blut_zaehler(int idx);
extern int re15_g5_biss_blut_zahl(void);
extern void re15_g5_boss_spawn(int slot);
extern void re15_actors_anim_advance(void);               /* player_common.c: Leiche haelt ihr Bild (Runde 42) */
extern int  re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);   /* re15_damage.c: Zielhilfe (Runde 42) */

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static uint8_t s_ems_buf[2][0x80000];
static int bank_laden(uint8_t type, int idx, const char *ersatz)
{
    size_t len = 0;
    uint8_t *src = NULL;
    if (ersatz) {
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", ersatz);
        src = slurp(p, &len);
    } else {
        size_t esz = 0, off = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
        if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(type), &off, &len) == 0) {
            src = (uint8_t *)malloc(len); memcpy(src, ems + off, len);
        }
        free(ems);
    }
    if (!src || len > sizeof s_ems_buf[idx]) { free(src); return 0; }
    memcpy(s_ems_buf[idx], src, len); free(src);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_ems_buf[idx], len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

/* Bank der Bahnsteig-Form wie pc_enemy_load_bahnsteig (RE2-Format, Opfer-Bank EM30). */
static uint8_t *s_form_emd = NULL;
static int bank_form_laden(void)
{
    size_t len = 0;
    if (!s_form_emd) s_form_emd = slurp(RE15_ASSET_PSX_DIR "/../RE2/BAHNSTEIG_FORM.EMD", &len);
    else return 0;
    if (!s_form_emd) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) return 0;
    uint8_t typ = eb->type;
    memset(eb, 0, sizeof *eb); eb->type = typ;
    if (re2_emd_parse_bank(s_form_emd, len, eb) != 0) return 0;
    eb->ok = 1; eb->re2_rig = 1; eb->buf = NULL; eb->bb_form = 1;
    return 1;
}

static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) return NULL;
    extern void scd_register_current_rdt(const re15_rdt_t *);
    scd_register_current_rdt(NULL);                      /* Vorraum abmelden (sonst SUB01-RESEED ab Tick 1) */
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    /* RDT erst NACH dem Spawn registrieren: sonst setzt scd_vm_tick Slot 1 jeden Tick auf sub01
     * zurueck (SUB01-RESEED @0x8003f064), bevor sub00 laeuft - im Spiel laeuft sub00 im Raum-Init. */
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_register_current_rdt(&s_rdt);
    return buf;
}

/* Runde 41 Spur H: Pose eines Aktors (Clip, Bild, Lage, Gier) fuer den Starr-Riegel. */
static uint64_t pose_kennung(const re15_actor_t *e)
{
    return ((uint64_t)e->motion << 56) ^ ((uint64_t)e->anim_frame << 48) ^ ((uint64_t)(uint32_t)e->x << 24) ^
           ((uint64_t)(uint32_t)e->y << 12) ^ (uint64_t)(uint32_t)e->z ^ ((uint64_t)(uint16_t)e->rot_y << 40);
}

/* Runde 42 "hangar_aussen": Raum `id` laden und ein paar KI-Takte laufen lassen - der naechste re15_bf_raum_eintritt sieht ihn als
 * Vorraum (birkin_formen.c s_bf_ki_raum, wie im Spiel der letzte KI-Takt vor Tuersequenz und Einblendung). */
static void vorraum(const char *rel, unsigned id)
{
    uint8_t *buf = raum_laden(rel, id);
    CHECK(buf != NULL, "Vorraum %s nicht ladbar", rel);
    if (!buf) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->state = 1; pl->hp = 100;
    for (int f = 0; f < 3; f++) re15_enemy_ai_run_all(0);
    free(buf);
}
static int zahl_typ(uint8_t typ)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == typ) n++;
    return n;
}

/* Runde 42 (PORT-SZENE, birkin_formen.c Kopf "Runde 42"): Szene nur beim Betreten durch die Nordtuer aus dem Monitorraum, Inhalt wie
 * bisher, Ende ohne Kampf: Birkin entfernt, Spider-Man liegt tot (Zustand 7), Kamera zurueck auf Cut 0 der Nordtuer. */
/* Sherry-Ruf der Hangar-Szene (NUTZERWUNSCH 2026-11-16, re15_bf_ruf_hook): Zaehler + Birkins Clip/Bild beim Aufruf. */
static int s_ruf_n = 0, s_ruf_clip = -1, s_ruf_bild = -1, s_ruf_sm = -1;
static void test_ruf(void)
{
    const int bs = re15_bf_hangar_slot(1);
    s_ruf_n++;
    if (bs > 0) { s_ruf_clip = g_actors[bs].motion; s_ruf_bild = g_actors[bs].anim_frame; }
    s_ruf_sm = re15_bf_hangar_slot(0);
}

static void test_hangar(unsigned raum, unsigned monitorraum, unsigned westraum)
{
    char rel[64]; snprintf(rel, sizeof rel, "STAGE5/ROOM%04X.RDT", raum);
    char mon[64]; snprintf(mon, sizeof mon, "STAGE5/ROOM%04X.RDT", monitorraum);
    char west[64]; snprintf(west, sizeof west, "STAGE4/ROOM%04X.RDT", westraum);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* (1) Westtuer (aus 4070/4071, Eintritts-Schnitt 11 = ROOM4070 main00 @0x011FE): nie eine Szene - auch nicht, wenn danach die
     *     Schnitte 0..13 angefordert werden (bis 98391e747 loesten Cut 4/5/6 aus). */
    vorraum(west, westraum);
    uint8_t *buf = raum_laden(rel, raum);
    CHECK(buf != NULL, "ROOM%04X nicht ladbar", raum);
    if (!buf) return;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    re15_bf_raum_eintritt(11);
    CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)), "EM30_HANGAR-Bank");
    CHECK(re15_bf_emd_datei(0x30u) && strstr(re15_bf_emd_datei(0x30u), "HANGAR"), "Modellwahl %04X != HANGAR", raum);
    CHECK(bank_laden(0x2Bu, 1, NULL), "EM2B-Bank");
    pl->active = 1; pl->state = 1; pl->x = -23878; pl->z = -16268; pl->hp = 100;
    for (int c = 0; c < 14; c++) {
        g_scd.cam_id = (uint8_t)c; g_scd.work_vars[0x0A] = (int16_t)c; g_scd.cut_auto_enabled = 1;
        for (int f = 0; f < 5; f++) { pl->hp = 100; re15_enemy_ai_run_all(0); }
    }
    CHECK(zahl_typ(0x30u) == 0 && zahl_typ(0x2Bu) == 0, "%04X von Westen: %d Birkin, %d Spider-Man", raum, zahl_typ(0x30u),
          zahl_typ(0x2Bu));
    CHECK(re15_bf_hangar_phase() == 0 && !re15_bf_hangar_riegel() && !re15_game_flag_get(RE15_BF_HANGAR_BANK, RE15_BF_HANGAR_BIT),
          "%04X von Westen: Szene (phase %d, Riegel %d)", raum, re15_bf_hangar_phase(), re15_bf_hangar_riegel());
    free(buf);

    /* (2) Gegenprobe Vorraum: ohne Monitorraum davor (Laden/Debug-Sprung, hier der Gang selbst) keine Szene */
    buf = raum_laden(rel, raum);
    if (!buf) return;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    re15_enemy_ai_run_all(0);                            /* Vorraum = der Gang selbst */
    re15_bf_raum_eintritt(0);
    CHECK(zahl_typ(0x30u) == 0 && !re15_bf_hangar_riegel(), "%04X ohne Monitorraum davor: Szene gestartet", raum);
    free(buf);

    /* (3) Nordtuer aus dem Monitorraum: Szene startet im Raumwechsel (Eintritts-Schnitt 0 = ROOM5030 main00 Slot 3 @0x00EC6) */
    vorraum(mon, monitorraum);
    buf = raum_laden(rel, raum);
    if (!buf) return;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;               /* Szenenkamera Cut 12 aus den RID des Raums */
    CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)) && bank_laden(0x2Bu, 1, NULL), "Banken");
    pl->active = 1; pl->state = 1; pl->x = -12117; pl->z = 4472; pl->hp = 100;
    g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0; g_scd.cam_change_pending = 0; g_scd.cut_auto_enabled = 1;
    re15_bf_raum_eintritt(0);
    int bs = re15_bf_hangar_slot(1), ms = re15_bf_hangar_slot(0);
    CHECK(bs > 0 && g_actors[bs].type == 0x30u, "kein Birkin gesetzt (slot %d)", bs);
    CHECK(ms > 0 && g_actors[ms].type == 0x2Bu, "kein Spider-Man gesetzt (slot %d)", ms);
    CHECK(re15_bf_hangar_riegel() && re15_game_flag_get(2, 7) && re15_game_flag_get(1, 27), "Riegel Set(2,7)/Set(1,27) fehlt");
    CHECK(g_scd.cam_id == 12 && g_scd.work_vars[0x0A] == 12 && g_scd.cam_change_pending && !g_scd.cut_auto_enabled,
          "Szenenkamera: cam %d wv %d pending %d auto %d", g_scd.cam_id, g_scd.work_vars[0x0A], g_scd.cam_change_pending,
          g_scd.cut_auto_enabled);
    if (bs <= 0 || ms <= 0) { free(buf); return; }
    re15_actor_t *b = &g_actors[bs], *m = &g_actors[ms];
    CHECK(re15_bf_szene_zeigt(b) && re15_bf_szene_zeigt(m) && !re15_bf_szene_zeigt(pl), "Region-Cull-Ausnahme falsch");

    /* (4) Ablauf nach dem Video-Zeitplan. Riegel: keine Figur steht laenger als 1 Takt starr (ausser Spider-Man im
     *     Vorlauf, den das Kamera-Standbild verdeckt), der Ansprung hat keine Spruenge (Video h4..8: hoechstens 485 je
     *     Bild), der Spider-Man wird hochgehalten und landet am Boden, dann springt er davon und ist weg. */
    int max_hoehe = 0, ende = -1, starr_b = 0, starr_m = 0, lauf_b = 0, lauf_m = 0, sprung_max = 0, sm_am_boden = 0;
    int sm_weg = -1, ph_seen[8] = { 0 }, griff_takte = 0;
    /* Runde 41 SF Neuaufbau ab NB2 (Nutzer 2026-11-03: Fangen und Wurf "vollkommen asynchron"): im Halten haengt der Spider-Man
     * mit seinem Griffpunkt (Knochen 19 + (0,485,0), vorher Knochen 20) an Birkins Sichelhand (15). r41/sichel 3f91e5c57 hielt ihn 866-1243 Einheiten (57-84 Bildpunkte)
     * daneben (eigene Bildversatz-Spur k_sm_halte5_dx/dy); NB2 6a1e9430d 39-126. Grenze 150 Einheiten = ~10 Bildpunkte in Birkins
     * Blicktiefe (~7000, 1 Bildpunkt ~14,6 Einheiten), also sichtbar Hand an Glied; gemessen ab dem 5. Haltetakt (Video h16), vorher
     * zieht der Griff ihn heran (h12-h14 396-762). */
    int griff_max = 0, griff_n = 0;
    uint64_t kb = pose_kennung(b), km = pose_kennung(m);
    int32_t mx = m->x, my = m->y, mz = m->z;
    /* Gruenes Blut (Nutzer 2026-11-13 "sieht aus wie gruenes Gas"): jeder Spawn der Szene ist der Original-Treffer der Spinne
     * STAGE2 @0x80113b08-54 - Effekt 0 / Sub 8 / Skala 0x2000 (`lui a0,0x8` @0x80113b10, `ori a0,a0,0x2000` @0x80113b30), Mischart
     * Original (abr_port 0), im Halten an Teil 0 des Spider-Man (`addiu a2,v1,64` @0x80113b54, a3 = 0x80118f18 Nullen). Zeitplan
     * (Port-Wahl nach Video 4): Wolke h24-h63 jedes 3. Bild = 14, Saeule h50-h54 + h55/57/59 = 8, Boden h82/h84 = 2 (y -80).
     * Vorher: Skala 0x3400/0x1800/0x3000/0x4000, abr_port 4 (ABR3), Anker Sichelhand + Bildversatz. */
    int blut_teil0 = 0, blut_boden = 0, blut_falsch = 0, blut_art = 0;
    /* Ansprung (Nutzer 2026-11-13 "gleitet ... mit gehobenen Armen ... Linear auf Birkin zu"): in Phase 2 (h0..h11) steht der Aktor
     * auf dem Boden bzw. springt flach darueber (y 0 .. -605, Video h5-h11 Spalt Fuss -> Boden 5..20 Bildpunkte) und spielt ATTACK2
     * 0x17 (@0x8011402c) bzw. ab h2 den gekippten GRIFF-Clip 0x11 B62 (@0x80112fdc, Runde 4 N5), Hoehe nach dem Video; vor
     * Aufgabe 2 Bogen bis y -1641 mit dem Sprung-Clip 0x16. Nach dem Abklingen (ab h18) ist die Wurzel nicht mehr gekippt. */
    int gleit_n = 0, gleit_falsch = 0, kipp_rest = 0;
    /* Halten (Nutzer 2026-11-13 "struggled ... viel mehr ... bis er letztendlich tot ist"): erst HURT Clip 4 (@0x80114a44), dann
     * nachlassend HURT Clip 7 (@0x80114a68), ab h52 tot in Clip 7 B5 (kompakteste haengende Haltung, Runde 3). */
    int halt_weh = 0, halt_zuck = 0, halt_tod = 0, halt_clip = -1, halt_bild = -1;
    /* Runde 7 (Wurf eindrehend, birkin_formen.c Kopf BF_H_LOS): im letzten Grifftakt h73 schon die Rueckenlage 0x0B B4 (Spider-Man-
     * Maschine @0x801150bc, +0x8f = 7 @0x801150dc), davor tot in Clip 7 B5. */
    int letzt_clip = -1, letzt_bild = -1;
    unsigned char fx_vorher[RE15_ESP_FX_MAX];
    re15_esp_fx_reset();
    int32_t weg_x = 0, weg_z = 0; int16_t weg_gier = 0;   /* Runde 42: letzte Lage des Spider-Man in der Szene */
    s_ruf_n = 0; s_ruf_clip = s_ruf_bild = s_ruf_sm = -1;
    re15_bf_ruf_hook(test_ruf);
    int ruf_takt = -1;
    for (int f = 0; f < 200 && ende < 0; f++) {
        const int ruf_vor = s_ruf_n;
        static const int32_t null3[3] = { 0, 0, 0 };
        int32_t t0[3];
        const int t0_ok = m->active && re15_enemy_part_point(m, 0, null3, t0);   /* Stand, den der Takt beim Spawn sieht */
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) fx_vorher[i] = re15_esp_fx_get(i) != NULL;
        re15_enemy_ai_run_all(0);
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *fx = re15_esp_fx_get(i);
            if (!fx || fx_vorher[i]) continue;
            if (fx->effect_id != 0 || fx->sub_index != 8 || fx->scale16 != 0x2000 || fx->abr_port != 0) blut_art++;
            if (t0_ok && fx->x == t0[0] && fx->y == t0[1] && fx->z == t0[2]) blut_teil0++;
            else if (fx->y == -80) blut_boden++;
            else blut_falsch++;
        }
        const int ph = re15_bf_hangar_phase();
        if (ph >= 0 && ph < 8) ph_seen[ph] = 1;
        const int sm_da = (re15_bf_hangar_slot(0) == ms) && m->active && m->state != 7;   /* Runde 42: ohne die Leiche */
        if (sm_da) { weg_x = m->x; weg_z = m->z; weg_gier = m->rot_y; }
        if (sm_da) {
            if (-m->y > max_hoehe) max_hoehe = -m->y;
            if (ph == 5 && m->y == 0) sm_am_boden = 1;
            if (ph == 3 && griff_takte >= 7 && m->rot_z != 0) kipp_rest++;
            if (ph == 3) { letzt_clip = m->motion; letzt_bild = m->anim_frame; if (m->motion == 4) halt_weh++; if (m->motion == 7) halt_zuck++; if (m->motion == 9) halt_tod++; }
            if (ph == 3 && m->motion != 0x0B) { halt_clip = m->motion; halt_bild = m->anim_frame; }
            if (ph == 2) { gleit_n++; if (m->y < -1100 || m->y > 0 || (m->motion != 0x17 && m->motion != 0x11)) gleit_falsch++; }
            if (ph == 2 || (ph == 3 && ++griff_takte < 54)) {   /* Ansprung + Halten bis Video h66 (Wurf ab h66 ist schnell) */
                double dx = m->x - mx, dy = m->y - my, dz = m->z - mz;
                int32_t d = (int32_t)sqrt(dx * dx + dy * dy + dz * dz);
                if (d > sprung_max) sprung_max = d;
            }
            mx = m->x; my = m->y; mz = m->z;
            if (ph == 3 && griff_takte >= 7) {   /* griff_takte 1 = h12 -> 7 = h18 (BF_H_GEHALTEN) */
                int32_t hand[3], griff[3];
                re15_enemy_bone_world_pos(b, 15, hand);
                {   /* Griffpunkt: Knochen 19 + (0, d, 0), d = 485 bis h48, dann linear zur Huefte (0) bei h60 (birkin_formen.c
                     * BF_SM_GRIFF_Y / BF_H_SACKT_*: tot sackt er im Griff ab); griff_takte 1 = h12 */
                    const int hh = 11 + griff_takte;
                    const int32_t gofs[3] = { 0, hh <= 48 ? 485 : hh >= 60 ? 0 : 485 * (60 - hh) / 12, 0 };
                    (void)re15_enemy_part_point(m, 19, gofs, griff); }
                const double gx = hand[0] - griff[0], gy = hand[1] - griff[1], gz = hand[2] - griff[2];
                const int ga = (int)sqrt(gx * gx + gy * gy + gz * gz);
                /* Runde 4 Punkt 5: Birkin haelt vorwaerts im Kreis (B10..B28 ab h17, Wiedereinstieg h36 und h52); dabei ueberblendet
                 * der Zeichner 12 Takte lang und der Griff folgt der GEZEICHNETEN Hand - die Abfrage hier liefert die Zielpose, daher
                 * zaehlen diese Takte nicht. */
                const int hw = 11 + griff_takte, wieder = (hw >= 36 && hw < 48) || (hw >= 52 && hw < 64);
                /* Runde 7 (Wurf eindrehend, birkin_formen.c Kopf BF_H_LOS): h71..h73 ueberblendet Birkin jeden Takt (Haltebilder B24/B20
                 * +0x8f = 2, dann B85 +0x8f = 5) und der Griff liegt an der GEZEICHNETEN Hand (bf_knochen_gezeichnet) - die Abfrage
                 * hier liefert die Zielpose, daher zaehlen diese Takte nicht. */
                const int wurf = hw > 70;
                if (!wieder && !wurf && ga > griff_max) griff_max = ga;
                griff_n++;
            }
        } else if (sm_weg < 0) sm_weg = f;
        if (ph == 7 && ende < 0) ende = f;
        if (s_ruf_n != ruf_vor && ruf_takt < 0) ruf_takt = f;
        if (re15_bf_hangar_riegel()) {
            uint64_t nb = pose_kennung(b), nm = pose_kennung(m);
            lauf_b = (nb == kb) ? lauf_b + 1 : 0;
            if (sm_da && ph >= 2) lauf_m = (nm == km) ? lauf_m + 1 : 0;
            if (lauf_b > starr_b) starr_b = lauf_b;
            if (lauf_m > starr_m) starr_m = lauf_m;
            kb = nb; km = nm;
        }
    }
    for (int p = 1; p <= 7; p++) CHECK(ph_seen[p], "Phase %d nie erreicht", p);
    /* Sherry-Ruf (NUTZERWUNSCH 2026-11-16, birkin_formen.c bf_sherry_ruf): genau einmal, im ersten Gangtakt - Birkin in WALK-Clip 1
     * Bild BF_GANG_BILD0 = 16 (@0x80117a64), der Spider-Man ist schon weg, die Szene laeuft noch. Stand 98391e747: 0-mal = ROT. */
    re15_bf_ruf_hook(NULL);
    CHECK(s_ruf_n == 1, "%04X Sherry-Ruf: %d-mal in der Szene (Soll 1)", raum, s_ruf_n);
    CHECK(s_ruf_clip == 1 && s_ruf_bild == 16, "%04X Sherry-Ruf: Birkin dabei in Clip %d Bild %d (Soll Gang, Clip 1 Bild 16)", raum,
          s_ruf_clip, s_ruf_bild);
    CHECK(s_ruf_sm < 0 && sm_weg >= 0 && ruf_takt > sm_weg && ruf_takt <= ende,
          "%04X Sherry-Ruf: Takt %d nicht zwischen Abgang des Spider-Man (%d) und Szenenende (%d)", raum, ruf_takt, sm_weg, ende);
    fprintf(stderr, "Hangar %04X Sherry-Ruf: %d-mal, Takt %d (Spider-Man weg %d, Ende %d), Birkin Clip %d Bild %d\n", raum, s_ruf_n,
            ruf_takt, sm_weg, ende, s_ruf_clip, s_ruf_bild);
    /* Runde 5: Griff bis h72 (Handhoehe ~2190), der Schleuder-Scheitel der Runden 3/4 (y -3305 frei in der Luft) entfaellt. */
    CHECK(max_hoehe > 2000, "Spider-Man nie hochgehalten (max %d)", max_hoehe);
    CHECK(sm_am_boden, "Spider-Man nie am Boden aufgeschlagen");
    CHECK(halt_weh >= 20 && halt_zuck >= 30 && halt_tod == 0 && halt_clip == 7 && halt_bild == 5,
          "Halten: %d Takte Clip 4, %d Takte Clip 7, %d Takte Clip 9, letzter Haltetakt Clip %d B%d (Soll >= 20, >= 30, 0, Clip 7 B5)",
          halt_weh, halt_zuck, halt_tod, halt_clip, halt_bild);
    CHECK(letzt_clip == 0x0B && letzt_bild == 4, "Wurf: letzter Grifftakt Clip %d B%d (Soll Rueckenlage 0x0B B4, Runde 7)", letzt_clip, letzt_bild);
    CHECK(kipp_rest == 0, "Halten: %d Takte ab h18 noch mit gekippter Wurzel (rot_z)", kipp_rest);
    CHECK(gleit_n >= 10 && gleit_falsch == 0, "Ansprung: %d von %d Takten hoeher als 1100 ueber dem Boden oder nicht Clip 0x17/0x11", gleit_falsch, gleit_n);
    CHECK(blut_art == 0, "gruenes Blut: %d Plaetze nicht Effekt 0 / Sub 8 / Skala 0x2000 / abr_port 0 (@0x80113b10/30)", blut_art);
    CHECK(blut_teil0 == 22 && blut_boden == 2 && blut_falsch == 0,
          "gruenes Blut: %d an Teil 0 des Spider-Man (Soll 22, @0x80113b54), %d am Boden (Soll 2), %d anderswo (Soll 0)",
          blut_teil0, blut_boden, blut_falsch);
    fprintf(stderr, "Gruenes Blut: %d Spawns an Teil 0, %d am Boden, %d anderswo, %d mit falscher Art\n", blut_teil0, blut_boden,
            blut_falsch, blut_art);
    CHECK(griff_n >= 40 && griff_max < 150, "Griff nicht synchron: Griffknochen bis %d Einheiten von der Sichelhand (Soll < 150, %d Haltetakte)", griff_max, griff_n);
    fprintf(stderr, "Griff: Abstand Sichelhand - Griffknochen im Halten hoechstens %d Einheiten\n", griff_max);
    CHECK(sprung_max <= 700, "Ansprung/Halten springt: %d Einheiten in einem Takt (Video h4..8 hoechstens 485)", sprung_max);
    CHECK(starr_b <= 1 && starr_m <= 1, "starr in der Szene: Birkin %d, Spider-Man %d Takte ohne Posenwechsel", starr_b, starr_m);
    CHECK(sm_weg > 0, "Spider-Man verlaesst das Bild nicht");
    CHECK(ende > sm_weg, "Szene endet nicht nach der Flucht (Ende %d, weg %d)", ende, sm_weg);
    CHECK(!re15_bf_hangar_riegel() && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27), "Riegel nach dem Szenenende noch zu");
    CHECK(g_scd.cam_id == 0 && g_scd.work_vars[0x0A] == 0 && g_scd.cut_auto_enabled,
          "Kamera nach dem Szenenende: cam %d wv %d auto %d (Soll Cut 0 der Nordtuer, Zonen an)", g_scd.cam_id,
          g_scd.work_vars[0x0A], g_scd.cut_auto_enabled);
    CHECK(re15_game_flag_get(RE15_BF_HANGAR_BANK, RE15_BF_HANGAR_BIT), "Szenenende setzt Bank-9-Bit %d nicht", RE15_BF_HANGAR_BIT);
    CHECK(!re15_game_flag_get(RE15_BF_HANGAR_BANK, RE15_BF_BESIEGT_BIT), "Szenenende setzt Bit %d (kein Kampf mehr)", RE15_BF_BESIEGT_BIT);
    /* (5) Ende ohne Kampf: kein Birkin mehr; der Spider-Man liegt tot, wo er das Bild verlassen hat */
    CHECK(zahl_typ(0x30u) == 0 && re15_bf_hangar_slot(1) < 0, "nach der Szene noch %d Birkin (Slot %d)", zahl_typ(0x30u),
          re15_bf_hangar_slot(1));
    const int ls = re15_bf_hangar_slot(0);
    CHECK(zahl_typ(0x2Bu) == 1 && ls > 0, "nach der Szene %d Spider-Man (Leiche Slot %d)", zahl_typ(0x2Bu), ls);
    if (ls > 0) {
        re15_actor_t *l = &g_actors[ls];
        CHECK(l->type == 0x2Bu && l->state == 7 && l->hp <= 0 && l->y == 0 && l->motion == 0x0B && l->anim_frame == 30,
              "Leiche: typ %02x Zustand %d hp %d y %d Clip %d Bild %d (Soll 0x2B, 7, <= 0, 0, 0x0B, 30)", l->type, l->state, l->hp,
              (int)l->y, l->motion, l->anim_frame);
        CHECK(l->x == weg_x && l->z == weg_z && l->rot_y == weg_gier, "Leiche (%d,%d) Gier %d statt der letzten Szenenlage (%d,%d) Gier %d",
              (int)l->x, (int)l->z, l->rot_y, (int)weg_x, (int)weg_z, weg_gier);
        /* im Gang (ROOM50A0 SCA: Halle x -12400..-2700 zwischen Innenblock sca 0 @0x013BC und Ostwand sca 4 @0x013EC, z -26600..-10090
         * zwischen Suedwand sca 12 @0x0144C und Wand sca 3 @0x013E0) */
        CHECK(l->x > -12400 && l->x < -2700 && l->z > -26600 && l->z < -10090, "Leiche (%d,%d) nicht im Gang", (int)l->x, (int)l->z);
        CHECK(re15_bf_szene_besitzt(ls) && !re15_bf_szene_zeigt(l), "Leiche: KI nicht gesperrt oder Cull-Ausnahme an");
        /* 600 Takte: liegt still, Zustand 7; keine Zielhilfe auf die Leiche (re15_player_aim_target ueberspringt Zustand 7) */
        const uint64_t k0 = pose_kennung(l);
        int bewegt = 0, fremd = 0, ziel = 0;
        pl->x = l->x - 1500; pl->z = l->z + 1500;
        for (int f = 0; f < 600; f++) {
            int32_t tx, tz;
            pl->hp = 100;
            re15_enemy_ai_run_all(0);
            re15_actors_anim_advance();
            if (pose_kennung(l) != k0 || l->state != 7 || !l->active) bewegt++;
            if (zahl_typ(0x30u) != 0 || zahl_typ(0x2Bu) != 1) fremd++;
            if (re15_player_aim_target(30000, &tx, &tz)) ziel++;
        }
        CHECK(bewegt == 0 && fremd == 0 && ziel == 0, "Leiche: %d Takte bewegt/nicht Zustand 7, %d Takte Zusatzfigur, %d Takte Ziel",
              bewegt, fremd, ziel);
        fprintf(stderr, "Hangar %04X: Leiche Slot %d bei (%d,%d) Gier %d, 600 Takte still, kein Ziel\n", raum, ls, (int)l->x,
                (int)l->z, l->rot_y);
    }
    fprintf(stderr, "Hangar %04X: Birkin slot %d, SM slot %d, SM weg Takt %d, Ende Takt %d, SM max Hoehe %d, groesster Schritt %d, "
                    "starr max %d/%d\n", raum, bs, ms, sm_weg, ende, max_hoehe, sprung_max, starr_b, starr_m);
    free(buf);

    /* (6) Wiedereintritt aus dem Monitorraum nach der Szene (Bit 38, ueber das Neuladen getragen - raum_laden loescht die Banken wie
     *     ein neues Spiel): keine Szene, kein Birkin, kein Spider-Man */
    vorraum(mon, monitorraum);
    buf = raum_laden(rel, raum);
    if (!buf) return;
    re15_game_flag_set(RE15_BF_HANGAR_BANK, RE15_BF_HANGAR_BIT, 1);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    pl->active = 1; pl->state = 1; pl->x = -12117; pl->z = 4472; pl->hp = 100;
    g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0;
    re15_bf_raum_eintritt(0);
    for (int f = 0; f < 30; f++) { pl->hp = 100; re15_enemy_ai_run_all(0); }
    CHECK(zahl_typ(0x30u) == 0 && zahl_typ(0x2Bu) == 0, "Wiedereintritt: %d Birkin, %d Spider-Man", zahl_typ(0x30u), zahl_typ(0x2Bu));
    CHECK(!re15_bf_hangar_riegel() && re15_bf_hangar_phase() == 7 && g_scd.cam_id != 12, "Wiedereintritt: Szene (phase %d, cam %d)",
          re15_bf_hangar_phase(), g_scd.cam_id);
    free(buf);
    g_room_rdt_ok = 0;
}

/* ==== Hangar: Merker beim Betreten (Runde 42; bis 98391e747 "liegen bleiben", Kopf re15_birkin_formen.h) ==============================
 * Raum laden wie raum_laden, aber mit Merkern VOR dem Raum-Init (scd_vm_init loescht alle Baenke = neues Spiel; ein Raumwechsel im
 * Spiel tut das nicht, ein geladener Spielstand bringt sie mit). */
typedef struct { uint8_t bank, bit, wert; } hl_merker_t;
static uint8_t *raum_laden_mit(const char *rel, unsigned id, const hl_merker_t *m, int n)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return NULL; }
    extern void scd_register_current_rdt(const re15_rdt_t *);
    scd_register_current_rdt(NULL);
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    for (int i = 0; i < n; i++) re15_game_flag_set(m[i].bank, m[i].bit, m[i].wert);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_register_current_rdt(&s_rdt);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    return buf;
}

/* Aus dem Monitorraum in den Gang mit Merkern m: Rueckgabe 1 = die Szene ist gestartet. Danach 30 Takte. */
static int hm_betreten(unsigned raum, unsigned monitorraum, const hl_merker_t *m, int n)
{
    char rel[64]; snprintf(rel, sizeof rel, "STAGE5/ROOM%04X.RDT", raum);
    char mon[64]; snprintf(mon, sizeof mon, "STAGE5/ROOM%04X.RDT", monitorraum);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    vorraum(mon, monitorraum);
    uint8_t *buf = raum_laden_mit(rel, raum, m, n);
    CHECK(buf != NULL, "ROOM%04X nicht ladbar", raum);
    if (!buf) return -1;
    CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)) && bank_laden(0x2Bu, 1, NULL), "Banken");
    pl->active = 1; pl->state = 1; pl->x = -12117; pl->z = 4472; pl->hp = 100;
    g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0;
    re15_bf_raum_eintritt(0);
    const int start = re15_bf_hangar_riegel();
    for (int f = 0; f < 30; f++) { pl->hp = 100; re15_enemy_ai_run_all(0); }
    free(buf);
    return start;
}

static void test_hangar_merker(unsigned raum, unsigned kraftraum, unsigned monitorraum)
{
    uint8_t *buf = NULL;
    /* (a) alter Spielstand aus 98391e747: Bit 39 (Hangar-Birkin besiegt) allein und mit Bit 38 -> Gang leer, keine Szene */
    for (int nur39 = 0; nur39 <= 1; nur39++) {
        const hl_merker_t m[] = { { RE15_BF_HANGAR_BANK, RE15_BF_BESIEGT_BIT, 1 }, { RE15_BF_HANGAR_BANK, RE15_BF_HANGAR_BIT, 1 } };
        const int s = hm_betreten(raum, monitorraum, m, nur39 ? 1 : 2);
        CHECK(s == 0 && zahl_typ(0x30u) == 0 && zahl_typ(0x2Bu) == 0 && re15_bf_hangar_phase() == 7 && g_scd.cam_id != 12,
              "%04X mit %s: Szene %d, %d Birkin, %d Spider-Man, phase %d", raum, nur39 ? "Bit 39" : "Bit 38+39", s, zahl_typ(0x30u),
              zahl_typ(0x2Bu), re15_bf_hangar_phase());
        re15_bf_reset();
    }
    /* (b) Reihenfolge: Kraftraum-Birkin besiegt (Bank 3 Bit 0x49) -> keine Szene, Gang leer; Gegenprobe ohne Merker -> Szene */
    {
        const hl_merker_t m[] = { { RE15_BF_KRAFTRAUM_BANK, RE15_BF_KRAFTRAUM_BIT, 1 } };
        const int s = hm_betreten(raum, monitorraum, m, 1);
        CHECK(s == 0 && zahl_typ(0x30u) == 0 && zahl_typ(0x2Bu) == 0 && !re15_game_flag_get(RE15_BF_HANGAR_BANK, RE15_BF_HANGAR_BIT),
              "%04X nach Kraftraum-Sieg: Szene %d, %d Birkin, %d Spider-Man", raum, s, zahl_typ(0x30u), zahl_typ(0x2Bu));
        re15_bf_reset();
        const int s2 = hm_betreten(raum, monitorraum, NULL, 0);
        CHECK(s2 == 1 && zahl_typ(0x30u) == 1 && zahl_typ(0x2Bu) == 1, "%04X Gegenprobe ohne Merker: Szene startet nicht (%d Birkin, %d SM)",
              raum, zahl_typ(0x30u), zahl_typ(0x2Bu));
        re15_bf_reset();                                 /* Riegel der angefangenen Szene loesen */
    }
    /* (c) Kraftraum unveraendert: mit den Hangar-Merkern (38 + 39) setzt sub00 dort weiter seinen Birkin (Sce_em_set 0x30) */
    {
        char kr[64]; snprintf(kr, sizeof kr, "STAGE5/ROOM%04X.RDT", kraftraum);
        const hl_merker_t m[] = { { RE15_BF_HANGAR_BANK, RE15_BF_HANGAR_BIT, 1 }, { RE15_BF_HANGAR_BANK, RE15_BF_BESIEGT_BIT, 1 } };
        buf = raum_laden_mit(kr, kraftraum, m, 2);
        CHECK(buf != NULL, "ROOM%04X nicht ladbar", kraftraum);
        if (buf) {
            CHECK(zahl_typ(0x30u) == 1, "ROOM%04X nach der Hangar-Szene: %d Birkin statt 1", kraftraum, zahl_typ(0x30u));
            free(buf); buf = NULL;
        }
        /* Gegenprobe der Form: ist der Kraftraum-Birkin besiegt (Bit 0x49, 0x3D gesetzt), setzt sub00 keinen (Tor @0x00AEA / @0x009CC) */
        const hl_merker_t m2[] = { { 3, 0x3D, 1 }, { RE15_BF_KRAFTRAUM_BANK, RE15_BF_KRAFTRAUM_BIT, 1 } };
        buf = raum_laden_mit(kr, kraftraum, m2, 2);
        if (buf) {
            CHECK(zahl_typ(0x30u) == 0, "ROOM%04X mit Bit 0x49: %d Birkin (Form-Gegenprobe)", kraftraum, zahl_typ(0x30u));
            free(buf); buf = NULL;
        }
    }
    g_room_rdt_ok = 0;
}

static void test_bahnsteig(void)
{
    uint8_t *buf = raum_laden("STAGE5/ROOM5090.RDT", 0x5090u);
    CHECK(buf != NULL, "ROOM5090 nicht ladbar");
    if (!buf) return;
    /* Stufe 3: die Riesenform ist ab der Raumladung da (keine EM30-Runde, keine Mutation in 5090, E5/E6/E17). */
    CHECK(re15_bf_emd_datei(0x30u) == NULL && re15_bf_bahnsteig_bank_faellig(0x30u) && !re15_bf_mutation_offen(),
          "5090: Riesenform nicht ab der Raumladung");
    re15_bf_modell_hook(NULL);
    int bs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && (g_actors[s].type == 0x30u || g_actors[s].type == 0x36u)) bs = s;
    CHECK(bs > 0 && g_actors[bs].type == 0x30u, "5090-Spawn nicht 0x30 (slot %d type 0x%02X)",
          bs, bs > 0 ? g_actors[bs].type : 0);
    if (bs <= 0) { free(buf); return; }
    CHECK(bank_laden(0x30u, 0, NULL), "EM30-Opfer-Bank");
    re15_actor_t *b = &g_actors[bs];
    re15_game_flag_set(5, 0x1c, 0);
    CHECK(bank_form_laden(), "Bank der Bahnsteig-Form");
    /* Kampfposition wie nach sub04 (Pos_set(1200,0,-23350) @0x12FE), Spieler an der Kampfmarke (300,-23400) */
    b->x = 1200; b->z = -23350; b->grid_id = 0x13;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 300; pl->z = -23400; pl->hp = 100;
    re15_enemy_ai_run_all(0);                            /* erster Formen-Tick: Ctor-Teil, eigene HP */
    /* Runde 42 Strang finale (Plan S4, NUTZERWUNSCH 07.11. "die Form Birkin G5 aus dem Finale wieder entfernst"): in 5090 traegt die
     * Form die HP des RE2-Super-Tyrant 20000 + 200 (@0x80101b78-0x80101be0, re15_finale_r42.h) statt der G4-1000; ein toedlicher
     * Zustand fuehrt weder in die Rueckstoss-Szene noch zum G5, die HP stehen auf der Untergrenze 10000 (@0x80101ac8-dc). Bis Runde 41
     * prueften diese Zeilen Rueckstoss, Sturz und G5-Auftritt (Stand aaea5f8c2). */
    CHECK(b->hp == RE15_FI_HP_START, "Form kommt mit HP %d statt %d (Tyrant-Ctor @0x80101be0)", b->hp, RE15_FI_HP_START);
    b->state = 3; b->sub_state_3 = 0; b->hp = 0; b->birkin_flags = 0;
    int f, szene = 0, g5 = 0, hp_min = 32767;
    for (f = 0; f < 900; f++) {
        pl->hp = 100; pl->hit_react = 0;
        re15_enemy_ai_run_all(0);
        if (re15_bf_rueckstoss_phase() != 0) szene = 1;
        if (b->type != 0x30u) g5 = 1;
        if (b->hp < hp_min) hp_min = b->hp;
    }
    CHECK(!szene, "Rueckstoss-Szene lief (kein G5-Uebergang mehr, Plan S4)");
    CHECK(!g5 && b->type == 0x30u && b->active, "Form wurde 0x%02X / inaktiv (kein G5 mehr)", b->type);
    CHECK(hp_min >= RE15_FI_HP_UNTEN, "HP fielen auf %d unter die Untergrenze %d", hp_min, RE15_FI_HP_UNTEN);
    CHECK(re15_game_flag_get(5, 0x1c) == 0, "Birkin-Schalter z5:0x1C ohne Abschuss gesetzt");
    fprintf(stderr, "Bahnsteig: toedlicher Zustand -> HP %d, kein Rueckstoss, kein G5 (%d Ticks)\n", b->hp, f);
    free(buf);
}

static void test_modellwahl(void)
{
    g_current_room_id = 0x4080u;
    CHECK(re15_bf_emd_datei(0x29u) && strstr(re15_bf_emd_datei(0x29u), "EM29_ROT"), "4080: kein roter Affe");
    g_current_room_id = 0x4081u;
    CHECK(re15_bf_emd_datei(0x29u) != NULL, "4081: kein roter Affe");
    g_current_room_id = 0x50A0u;
    CHECK(re15_bf_emd_datei(0x29u) == NULL, "50A0: Affe darf Auslieferung bleiben");
    /* Stufe 3: Riesenform in 5080/5081 (Flucht) und 5090/5091 (Rueckstoss), nirgends sonst */
    g_current_room_id = 0x5080u;
    CHECK(re15_bf_bahnsteig_bank_faellig(0x30u) && re15_bf_flucht_raum() && !re15_bf_bahnsteig_aktiv(), "5080: keine Riesenform/Flucht");
    g_current_room_id = 0x5081u;
    CHECK(re15_bf_bahnsteig_bank_faellig(0x30u) && re15_bf_flucht_raum(), "5081: keine Riesenform/Flucht");
    g_current_room_id = 0x5091u;
    CHECK(re15_bf_bahnsteig_bank_faellig(0x30u) && !re15_bf_flucht_raum() && re15_bf_bahnsteig_aktiv(), "5091: keine Riesenform");
    g_current_room_id = 0x50A0u;
    CHECK(!re15_bf_bahnsteig_bank_faellig(0x30u), "50A0: Riesenform im Hangar");
    /* Runde 41 Spur SF (Nutzer E17): Labor-Kampf 50E0 (Leon) / 50F1 (Elza) traegt dieselbe Form wie der Hangar,
     * mit Schwanzkette (Knochen 16-22, tools/birkin_formen.py). */
    static const uint16_t labor[] = {0x50E0u, 0x50F1u};
    for (int i = 0; i < 2; i++) {
        g_current_room_id = labor[i];
        CHECK(re15_bf_emd_datei(0x30u) && strstr(re15_bf_emd_datei(0x30u), "HANGAR"),
              "%04X Labor: nicht die Hangar-Form", labor[i]);
    }
    g_current_room_id = 0x50E0u;
    re15_enemy_reset();
    CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)), "50E0: EM30_HANGAR-Bank");
    {
        re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
        CHECK(eb && eb->skel.bone_count == 23 && eb->md1.mesh_count == 23,
              "50E0: Form ohne Schwanzkette (Knochen %d, Meshes %d)", eb ? eb->skel.bone_count : -1,
              eb ? eb->md1.mesh_count : -1);
    }
    g_current_room_id = 0x3070u;
    CHECK(!re15_bf_bahnsteig_bank_faellig(0x30u), "3070: Riesenform");
    CHECK(re15_bf_emd_datei(0x30u) == NULL, "3070: Birkin muss EM30 bleiben");
    CHECK(!re15_bf_bahnsteig_aktiv() && !re15_bf_hangar_aktiv(), "3070: Formen aktiv");
}

/* Abnahme 2: Schnitt aus der Spielerlage gegen die RVD-Zonen des Raums (Datei @0x490, 20 Byte je Zone, Ende 0xFFFF): kein Streifen des
 * gewaehlten Schnitts darf an dieser Lage sofort weiterschalten, sonst pendelt die Kamera, und aus jeder Lage fuehrt der Schnitt zu Bild. */
static void test_schnitt_aus_lage(const char *rdt)
{
    size_t n = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rdt);
    uint8_t *b = slurp(p, &n);
    CHECK(b != NULL, "%s nicht ladbar", rdt);
    if (!b) return;
    static const struct { int32_t x; int cut; } soll[] = {
        { -400, 11 }, { 2749, 11 }, { 2750, 12 }, { 13999, 12 }, { 14000, 13 }, { 20249, 13 }, { 20250, 14 }, { 25000, 14 },
    };
    for (unsigned i = 0; i < sizeof soll / sizeof soll[0]; i++)
        CHECK(re15_bf_schnitt_aus_lage(soll[i].x) == soll[i].cut, "x %d: Cut %d statt %d", soll[i].x, re15_bf_schnitt_aus_lage(soll[i].x), soll[i].cut);
    for (int32_t x = -400; x <= 25000; x += 50)
        for (int32_t z = -26400; z <= -22168; z += 400) {
            int c = re15_bf_schnitt_aus_lage(x), treffer = 0;
            for (size_t o = 0x490; o + 20 <= n && !(b[o] == 0xff && b[o + 1] == 0xff); o += 20) {
                int16_t q[8];
                memcpy(q, b + o + 4, sizeof q);
                if (b[o + 2] != c || b[o + 3] == 0) continue;        /* Zone dieses Schnitts, Ziel 0 = Ankerzone ohne Wechsel */
                /* konvexes Viereck: alle Kanten auf derselben Seite */
                {
                    int pos = 0, neg = 0;
                    for (int k = 0; k < 4; k++) {
                        int64_t ax = q[k * 2], az = q[k * 2 + 1], bx = q[(k * 2 + 2) & 7], bz = q[(k * 2 + 3) & 7];
                        int64_t kreuz = (bx - ax) * ((int64_t)z - az) - (bz - az) * ((int64_t)x - ax);
                        if (kreuz > 0) pos++; else if (kreuz < 0) neg++;
                    }
                    if (!(pos && neg)) treffer++;
                }
            }
            CHECK(treffer == 0, "%s: Lage (%d,%d) Cut %d liegt in einem Wechselstreifen", rdt, x, z, c);
        }
    free(b);
}

/* Abnahme 2, G5-Biss: FUN_800401d4(40, 1) @0x80104118 / @0x80104170 toetet nie, klemmt auf HP 0 @0x800402b0. Devour verlangt HP <= 0
 * (`bgtz` @0x801008e0, @0x80103e80). Der Todes-Latch @0x800402d8 startet die Game-Over-FSM mitten im Fressen; der Kau-Zyklus
 * @0x80104b0c-6c senkt das Gewicht, sonst bleibt das Ende-Gate @0x80101d14 zu. */
extern void re15_g5_boss_tick(int slot);
extern int  re15_g5_fress_todeslatch(void);
extern int  re15_death_presentation_active(void);
extern int  re15_g5_blob_zustand(int *blob, int *ph, int32_t *gewicht0);
static void test_g5_fressen(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    CHECK(ems != NULL, "RE2/CDEMD0.EMS fehlt");
    if (!ems) return;
    re15_enemy_reset();
    re15_actor_init();
    g_current_room_id = 0x5090u;
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x36);
    CHECK(eb && re2_ems_load_bank(ems, n, 0x36, eb, NULL) == 0, "EM036 laedt nicht");
    if (!eb) { free(ems); return; }
    eb->buf = NULL; eb->ok = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->x = 24000; pl->z = -23400; pl->y = 0;
    re15_actor_t *e = &g_actors[2];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600; e->grid_id = 0x13;
    {   /* Auftritt bis zum Kampf */
        int sub = 2, t;
        for (t = 0; t < 1400 && (sub == 2 || t == 0); t++) { re15_g5_boss_tick(2); (void)re15_g5_boss_zustand(&sub, NULL, NULL); }
        CHECK(sub != 2, "G5-Auftritt endet nicht");
    }
    {   /* Spieler stirbt nicht am Biss, nie bei HP > 0 gefressen, HP faellt ueber 0 */
        int biss = 0, null_gesehen = 0, frass_bei = 999, sub = 0, ph = 0, reaktion = 0;
        for (int t = 0; t < 12000 && !re15_g5_fress_todeslatch(); t++) {
            int vor = pl->hp;
            e->x = 10000; e->z = -23400;                      /* u >= 9001 @0x801008a0 */
            pl->x = e->x + 4500; pl->z = e->z;
            if (reaktion > 0) reaktion--; else pl->hit_react = 0;   /* Treffer-Reaktion des Spielers, je Biss ein Treffer */
            e->hp = 600;
            re15_g5_boss_tick(2);
            if (vor - pl->hp == 20) biss++;                   /* 40 @0x80104118, halb auf Port-HP; Tentakel 15 @0x801016d0 zaehlt nicht */
            if (pl->hp < vor) reaktion = 30;
            if (pl->hp == 0) null_gesehen = 1;
            if (pl->hp < 0) { frass_bei = vor; break; }
        }
        (void)re15_g5_boss_zustand(&sub, &ph, NULL);
        CHECK(biss >= 2, "nur %d Bisse vor dem Fressen, Sperre +0x229 laeuft nicht ab (@0x80100188-98)", biss);
        CHECK(null_gesehen, "HP faellt nicht auf 0 (Biss a1 = 1 @0x80104170 klemmt auf 0 @0x800402b0)");
        CHECK(frass_bei == 0, "gefressen bei HP %d statt 0 (`bgtz` @0x801008e0 / @0x80103e80)", frass_bei);
        CHECK(sub == 4, "kein Devour (Sub %d)", sub);
    }
    CHECK(re15_g5_fress_todeslatch(), "Todes-Latch nicht gesetzt (@0x800402d8)");
    CHECK(re15_death_presentation_active(), "Game-Over-FSM startet nicht im Fressen (@0x80015014-30)");
    CHECK(!re15_player_is_dead(), "Opfer-Modus 4 besitzt den Spieler, is_dead muss 0 bleiben");
    {   /* Kauen: Gewicht pendelt, nach 150 Bildern Clip 2 faellt das Ende-Gate */
        int blob = 4, ph = 0, p2 = 0, p3 = 0, ende = -1;
        int32_t g0 = 0, g_max = -99999, g_min = 99999;
        for (int t = 0; t < 2400 && ende < 0; t++) {
            re15_g5_boss_tick(2);
            (void)re15_g5_blob_zustand(&blob, &ph, &g0);
            if (blob == 4 && ph == 2) p2++;
            if (blob == 4 && ph == 3) {
                p3++;
                if (g0 > g_max) g_max = g0;
                if (g0 < g_min) g_min = g0;
            }
            if (blob != 4) ende = t;
        }
        CHECK(p2 > 0 && p3 > 0, "Kau-Zyklus laeuft nicht (Phase 2 %d, Phase 3 %d)", p2, p3);
        CHECK(g_min < 6000 && g_max >= 10001, "Kauen: Gewicht %d..%d, Ende-Gate 6000 @0x80101d14 kann nicht fallen", g_min, g_max);
        CHECK(ende >= 0, "Ende-Gate faellt nie, Fressen laeuft endlos");
        fprintf(stderr, "G5: Fressen Blob zurueck nach %d Ticks, Kauen %d..%d\n", ende, g_min, g_max);
    }
    CHECK(re15_g5_fress_todeslatch(), "Latch faellt mit dem Blob, FSM wuerde zurueckgesetzt (kein Routine-Exit @0x80101d20-24)");
    pl->hp = 100;                                             /* Neues Spiel ueber den Titel: s_g5 steht noch */
    CHECK(!re15_g5_fress_todeslatch(), "Latch ueberlebt den Spielerzustand");
    free(ems);
}

/* Abnahme 3 N5: der G5 blutet. Wache `lbu v0,6(s3)` @0x801025e8 ist die Phase der Treffer-Routine (im ersten Tick 0), nicht die KI-Phase; Blick
 * `slti v0,a1,-64` @0x80102640, Zeilen 16 und 9..12 ohne Blut @0x80102658-64. Biss-Serie @0x801042d8: 3 Spawns je Bild, 10 Bilder (@0x801041a8). */
static void test_g5_blut(void)
{
    size_t ems_n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_n);
    CHECK(ems != NULL, "RE2 CDEMD0.EMS fehlt");
    if (!ems) return;
    re15_enemy_reset(); re15_actor_init();
    {   re15_enemy_bank_t *eb = re15_enemy_alloc(0x36u);
        CHECK(eb && re2_ems_load_bank(ems, ems_n, 0x36u, eb, NULL) == 0, "G5-Bank");
        if (eb) { eb->buf = NULL; eb->ok = 1; }
        eb = re15_enemy_alloc(0x37u);
        CHECK(eb && re2_ems_load_bank(ems, ems_n, 0x37u, eb, NULL) == 0, "Arm-Bank");
        if (eb) { eb->buf = NULL; eb->ok = 1; }
    }
    g_current_room_id = 0x5090u;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *e = &g_actors[2];
    pl->active = 1; pl->hp = 100; pl->x = 14000; pl->z = -23400; pl->rot_y = 2048;   /* Blick nach Westen: dirx = -4096 */
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36u; e->flags = 1; e->grid_id = 0x13u; e->hp = 600; e->x = 8000; e->z = -23400;
    re15_g5_boss_spawn(2);
    re15_g5_boss_tick(2);
    {   int sub = -1, ph = -1, n = 0;                         /* bis in die Lauf-Phase: KI-Phase != 0 */
        for (; n < 2000; n++) { re15_g5_boss_tick(2); (void)re15_g5_boss_zustand(&sub, &ph, NULL); if (sub <= 1 && ph != 0) break; pl->hp = 100; pl->hit_react = 0; }
        CHECK(n < 2000 && ph != 0, "G5: keine KI-Phase != 0 (Sub %d Phase %d)", sub, ph);
    }
    #define TREFFER(waffe) do { e->hit_react |= 1u; e->sub_state_1 = (waffe); e->re2z_self1d3 = 0; re15_g5_boss_tick(2); e->hit_react = 0; e->re2z_self1d3 = 0; re15_g5_boss_tick(2); } while (0)
    {   int a = re15_g5_blut_zaehler(3), sub = -1, ph = -1;
        (void)re15_g5_boss_zustand(&sub, &ph, NULL);
        TREFFER(3);
        CHECK(re15_g5_blut_zaehler(3) >= a + 2, "N5: Pistole, KI-Phase %d: %d Spawns statt mindestens 2 (@0x801027ec, @0x80102848)", ph, re15_g5_blut_zaehler(3) - a);
        fprintf(stderr, "G5: Pistole in KI-Phase %d: %d Blut-Spawns\n", ph, re15_g5_blut_zaehler(3) - a);
    }
    for (int i = 0; i < 30; i++) { pl->hp = 100; pl->hit_react = 0; re15_g5_boss_tick(2); }
    {   int a = re15_g5_blut_zaehler(3);
        pl->rot_y = 0;                                        /* Blick nach Osten: dirx = +4096, `slti v0,a1,-64` @0x80102640 */
        TREFFER(3);
        CHECK(re15_g5_blut_zaehler(3) == a, "N5: Blick nach Osten, %d Spawns statt 0", re15_g5_blut_zaehler(3) - a);
        pl->rot_y = 2048;
    }
    for (int i = 0; i < 30; i++) { pl->hp = 100; pl->hit_react = 0; re15_g5_boss_tick(2); }
    {   int a = re15_g5_blut_zaehler(3);                      /* Waffe 15 -> Zeile 9 (Tabelle s_g5_zeile_von_waffe) */
        TREFFER(15);
        CHECK(re15_g5_blut_zaehler(3) == a, "N5: Zeile 9 %d Spawns statt 0 (@0x80102660-64)", re15_g5_blut_zaehler(3) - a);
    }
    {   /* a1-Regel FUN_800401d4: a1 = 0 @0x801016d4 Gnade -14..-1 -> HP 0 (`slti v0,v0,-14` @0x80040288), bei HP 0 tot; a1 = 1 toetet nie (`bltz` @0x80040264) */
        extern void re15_g5_tent_treffer_test(re15_actor_t *p, int schaden, int a1);
        pl->hp = 5;  re15_g5_tent_treffer_test(pl, 7, 0);
        CHECK(pl->hp == 0, "Arm a1 = 0 bei HP 5: HP %d statt 0 (Gnade)", pl->hp);
        pl->hp = 0;  re15_g5_tent_treffer_test(pl, 7, 0);
        CHECK(pl->hp < 0, "Arm a1 = 0 bei HP 0: HP %d, muss toeten", pl->hp);
        pl->hp = 0;  re15_g5_tent_treffer_test(pl, 7, 1);
        CHECK(pl->hp == 0, "Arm a1 = 1 bei HP 0: HP %d, toetet nie", pl->hp);
        pl->hp = 3;  re15_g5_tent_treffer_test(pl, 7, 1);
        CHECK(pl->hp == 0, "Arm a1 = 1 bei HP 3: HP %d", pl->hp);
        pl->hp = 100; pl->hit_react = 0;
    }
    {   /* Biss: Lunge trifft den stehenden Spieler vor der Masse, danach 10 Bilder je 3 Spawns */
        int a = re15_g5_biss_blut_zahl(), hp0, gebissen = 0;
        pl->x = e->x + 4500; pl->z = e->z; pl->rot_y = 2048;
        for (int i = 0; i < 2400 && !gebissen; i++) {
            pl->hit_react = 0;
            hp0 = pl->hp = 100;
            pl->x = e->x + 4500; pl->z = e->z;
            re15_g5_boss_tick(2);
            if (pl->hp < hp0) gebissen = 1;
        }
        CHECK(gebissen, "G5 beisst nicht");
        for (int i = 0; i < 40; i++) { pl->hp = 100; re15_g5_boss_tick(2); }
        CHECK(re15_g5_biss_blut_zahl() - a == 30, "N5: Biss-Serie %d Spawns statt 30 (@0x801042d8, +0x21B = 10 @0x801041a8)", re15_g5_biss_blut_zahl() - a);
    }
    free(ems);
}

int main(void)
{
    test_modellwahl();
    test_hangar(0x50A0u, 0x5030u, 0x4070u);   /* Leon: Monitorraum 5030, Westtuer aus 4070 */
    test_hangar(0x50A1u, 0x5031u, 0x4071u);   /* Elza: RDT bytegleich, eigener Raum */
    test_hangar_merker(0x50A0u, 0x50E0u, 0x5030u);   /* Merker beim Betreten: Leon, Kraftraum 50E0 */
    test_hangar_merker(0x50A1u, 0x50F1u, 0x5031u);   /* Elza, Kraftraum 50F1 */
    test_bahnsteig();
    test_schnitt_aus_lage("STAGE5/ROOM5090.RDT");
    test_schnitt_aus_lage("STAGE5/ROOM5091.RDT");
    test_g5_fressen();
    test_g5_blut();
    if (fails) { fprintf(stderr, "test_birkin_szenen: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_szenen: OK\n");
    return 0;
}
