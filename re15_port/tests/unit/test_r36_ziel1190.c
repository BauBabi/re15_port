/**
 * @file test_r36_ziel1190.c
 * @brief Runde 36 Spur H — Zielscheiben-Texte HINTEN an den Scheiben, Knoepfe wieder Original.
 *
 * NUTZER-VORGABE (AUFTRAG.md Runde 36): die Saetze "This target has a surprisingly large number of bullet
 * holes." / "This target does not have many bullet holes" gehoeren an die Scheiben hinten (beim Anklicken),
 * nicht an die Knoepfe vorn; bei Elza sind die beiden anderen Scheiben "viele" (vertauscht).
 * Herleitung + Konstanten: include/re15_ziel1190.h, Dossier analysis/befunde_runde36/H_ziel1190.md.
 *
 * ECHTER WEG je Fall: ROOM1190/1191.RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter = Installer-
 * Block -> re15_ziel1190_install, sub01-Reseed je Bild), Spieler an die GEMESSENE Halteposition vor der
 * Scheibe (exe-Laeufe walkS*: x=-20845 vor Obj 0..2, x=-20792 vor Obj 3; Blick -x), Aktion ->
 * re15_aot_scan (Vorwaertspunkt 620 @0x80042bd0, sce-1-Weg) -> re15_scd_show_message -> Text-FSM.
 * Gemessen wird die Mechanik, nicht der Code-Wunsch:
 *   (A) Text-Plaetze: vier sce-1-Plaetze 17..20, sat 0x31, Band 0, Pause 0xffff, +-300/+-1200, Mitte =
 *       Scheibe (Obj 0..3 aus dem Pool).
 *   (B) Scheiben hinten, 2 Raeume x 3 Raumzustaende (Strom aus / an / nach Hunden) x 4 Scheiben: welche
 *       Nachricht aufgeht (Leon 0/2 -> 6, 1/3 -> 7; Elza vertauscht), ihr Klartext, Stimme 6/7 angemeldet,
 *       Welt-Freeze 0xffff0000, keine Ja/Nein-Frage, kein Raum-Ereignis.
 *   (C) Knoepfe vorn = Original: erste Nachricht ist die RDT-Nachricht (msg 0 bzw. 2) Byte fuer Byte,
 *       kein 6/7; Strom aus -> nach "Ja" msg 3; Strom an -> nach "Ja" faehrt die Scheibe ((4,n) kippt).
 *   (D) Der Platz FAEHRT MIT: nach der Fahrt steht Scheibe n bei x=-5140 (sub06) und ihr Platz auch;
 *       hinten ist dann nichts mehr anzuklicken, von der Bahn-Seite vor der vorderen Scheibe (Blick +x)
 *       kommt ihr Satz.
 *   (E) Betreten mit (4,n)=1: sub00 stellt Scheibe n vor (@0x02336..), der Platz wird dort angelegt.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_ziel1190.h"
#include "font_width.h"

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
extern uint8_t  g_aot_action_pressed;

static int s_fail = 0, s_pruef = 0;
#define PRUEFE(cond, ...) do { s_pruef++; if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fail = 1; } } while (0)

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

/* Scheiben (sub13 @0x0297E..: x=-21340, z) und gemessene Halteposition davor (exe, Laeufe walkS*). */
static const int32_t k_scheibe_z[4] = { -24516, -20916, -17316, -13716 };
static const int32_t k_halt_x[4]    = { -20845, -20845, -20845, -20792 };
/* Knopf-Mitte (main00 Aot_set: x -4400 + 300, z-Kante + 1200), Spieler 620 davor, Blick -x. */
static const int32_t k_knopf_z[4]   = { -24500, -20800, -17300, -13700 };

static const char *k_viele  = "This target has a surprisingly large number of bullet holes.";
static const char *k_wenige = "This target does not have many bullet holes.";

/* Raum hochfahren wie im Spiel. vorn_maske: Bits n -> (4,n)=1 VOR dem Betreten (Scheibe n vorn). */
static int raum_hoch(uint16_t raum, uint8_t *daten, long sz, re15_rdt_t *r, int strom, int hunde,
                     unsigned vorn_maske)
{
    if (re15_rdt_parse(daten, (size_t)sz, r) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    re15_game_flag_set(4, 243, (uint8_t)strom);
    re15_game_flag_set(4, 234, (uint8_t)hunde);
    for (int n = 0; n < 4; n++) re15_game_flag_set(4, (uint8_t)n, (uint8_t)((vorn_maske >> n) & 1u));
    scd_register_current_rdt(r);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(r->messages, r->messages_size);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = -2800; p->y = 0; p->z = -10900; p->rot_y = 2048; p->state = 1;   /* Durchgang am Tresenende */
    g_scd.player_mode = 0;
    scd_register_room_events(r);
    scd_room_reenter(r, p->x, p->z, 0);
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF); }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return 0;
}

static int prop_von_obj(int obj)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && (int)g_scd.props[k].obj_id == obj) return k;
    return -1;
}

typedef struct {
    int erste_id;          /* erste offene Nachricht (-1 = keine) */
    char text[200];        /* ihr Klartext */
    int choice;            /* Ja/Nein-Frage */
    int msg3;              /* msg 3 danach gesehen */
    int stimme;            /* VOICE_ON mit sample_id == erste_id angemeldet */
    uint32_t pause;        /* g_re15_pauseflags im Oeffnungsbild */
    int platz;             /* work_vars[0] nach dem Scan */
} lauf_t;

/* Spieler an (x,z,rot), Aktion, dann Bilder: Text-FSM mit "Weiter" (0x4000), an der Frage "Ja". */
static lauf_t druecken(int32_t x, int32_t z, int rot)
{
    lauf_t L; memset(&L, 0, sizeof L); L.erste_id = -1; L.platz = -99;
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    L.platz = (int)g_scd.work_vars[0];
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    for (int i = 0; i < 1500; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (L.erste_id < 0) {
                L.erste_id = g_scd.message_id;
                const char *t = re15_msg_get_text(g_scd.message_id);
                snprintf(L.text, sizeof L.text, "%s", t ? t : "");
                L.choice = re15_msg_is_choice(g_scd.message_id);
                L.pause = g_re15_pauseflags;
                scd_audio_event_t ev;
                while (scd_audio_queue_pop(&ev))
                    if (ev.kind == SCD_AUDIO_VOICE_ON && (int)ev.sample_id == L.erste_id) L.stimme = 1;
            }
            if (g_scd.message_id == 3) L.msg3 = 1;
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 ||
                                 g_scd.message_fsm == 4 || g_scd.message_fsm == 7))
                g_scd_pad_edge = 0x4000;
        }
        re15_msg_tick(0, 0, 0);
        g_scd_pad_edge = 0;
        scd_vm_tick();
        re15_aot_scan(p->x, p->z, 0xFF);
        int laeuft = g_scd.message_active;
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++)
            if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    return L;
}

/* Klartext der Original-Nachricht id des geladenen Raums (aus der RDT, nicht aus dem Port-Satz). */
static void orig_text(int id, char *out, size_t cap)
{
    const char *t = re15_msg_get_text(id);
    snprintf(out, cap, "%s", t ? t : "");
}

static void pruefe_plaetze(uint16_t raum, const char *name, unsigned vorn_maske)
{
    for (int n = 0; n < 4; n++) {
        const re15_aot_t *a = &g_aot.slots[RE15_ZIEL1190_SLOT0 + n];
        const int k = prop_von_obj(n);
        const int viele = re15_ziel1190_viele(raum, n);
        PRUEFE(k >= 0, "ROOM%04X %s: Scheibe %d nicht im Pool", raum, name, n);
        PRUEFE(a->active && a->type == RE15_AOT_TYPE_MESSAGE,
               "ROOM%04X %s: Platz %d kein sce-1-Platz", raum, name, RE15_ZIEL1190_SLOT0 + n);
        PRUEFE(a->event_id == (viele ? RE15_ZIEL1190_MSG_VIELE : RE15_ZIEL1190_MSG_WENIGE),
               "ROOM%04X %s: Platz %d Nachricht %d", raum, name, RE15_ZIEL1190_SLOT0 + n, a->event_id);
        PRUEFE(a->sce_flags == 0x31 && a->band == 0 && a->pause_mask16 == 0xFFFF &&
               a->half_w == 300 && a->half_h == 1200,
               "ROOM%04X %s: Platz %d Form sat=%02x band=%d pause=%04x halb=%d/%d", raum, name,
               RE15_ZIEL1190_SLOT0 + n, a->sce_flags, a->band, a->pause_mask16, a->half_w, a->half_h);
        if (k >= 0) {
            const int32_t soll_x = ((vorn_maske >> n) & 1u) ? -5220 : -21340;
            PRUEFE(g_scd.props[k].x == soll_x && g_scd.props[k].z == k_scheibe_z[n],
                   "ROOM%04X %s: Scheibe %d steht bei (%d,%d), erwartet (%d,%d)", raum, name, n,
                   g_scd.props[k].x, g_scd.props[k].z, soll_x, k_scheibe_z[n]);
            PRUEFE(a->x == g_scd.props[k].x && a->z == g_scd.props[k].z,
                   "ROOM%04X %s: Platz %d Mitte (%d,%d) != Scheibe (%d,%d)", raum, name,
                   RE15_ZIEL1190_SLOT0 + n, a->x, a->z, g_scd.props[k].x, g_scd.props[k].z);
        }
    }
}

/* (B) Scheiben hinten anklicken. */
static void fall_hinten(uint16_t raum, uint8_t *d, long sz, int strom, int hunde, const char *name)
{
    re15_rdt_t r;
    if (raum_hoch(raum, d, sz, &r, strom, hunde, 0) != 0) { PRUEFE(0, "Parse"); return; }
    pruefe_plaetze(raum, name, 0);
    for (int n = 0; n < 4; n++) {
        if (raum_hoch(raum, d, sz, &r, strom, hunde, 0) != 0) { PRUEFE(0, "Parse"); return; }
        const unsigned b4 = (unsigned)re15_game_flag_get(4, 0) | ((unsigned)re15_game_flag_get(4, 1) << 1) |
                            ((unsigned)re15_game_flag_get(4, 2) << 2) | ((unsigned)re15_game_flag_get(4, 3) << 3);
        lauf_t L = druecken(k_halt_x[n], k_scheibe_z[n], 2048);
        const int viele = re15_ziel1190_viele(raum, n);
        const int soll = viele ? RE15_ZIEL1190_MSG_VIELE : RE15_ZIEL1190_MSG_WENIGE;
        printf("  ROOM%04X %-11s Scheibe %d hinten (%d,%d): Platz %d msg %d stimme=%d pause=%08X \"%s\"\n",
               raum, name, n, k_halt_x[n], k_scheibe_z[n], L.platz, L.erste_id, L.stimme,
               (unsigned)L.pause, L.text);
        PRUEFE(L.platz == RE15_ZIEL1190_SLOT0 + n, "ROOM%04X %s Scheibe %d: Vorwaerts-Treffer Platz %d",
               raum, name, n, L.platz);
        PRUEFE(L.erste_id == soll, "ROOM%04X %s Scheibe %d: Nachricht %d, erwartet %d", raum, name, n,
               L.erste_id, soll);
        PRUEFE(strcmp(L.text, viele ? k_viele : k_wenige) == 0,
               "ROOM%04X %s Scheibe %d: Text \"%s\"", raum, name, n, L.text);
        PRUEFE(!L.choice && !L.msg3, "ROOM%04X %s Scheibe %d: Frage/Folgetext", raum, name, n);
        PRUEFE(L.stimme, "ROOM%04X %s Scheibe %d: Stimme %d nicht angemeldet", raum, name, n, soll);
        PRUEFE((L.pause & 0xFFFF0000u) == 0xFFFF0000u,
               "ROOM%04X %s Scheibe %d: Pause %08X (erwartet 0xffff0000)", raum, name, n, (unsigned)L.pause);
        const unsigned b4n = (unsigned)re15_game_flag_get(4, 0) | ((unsigned)re15_game_flag_get(4, 1) << 1) |
                             ((unsigned)re15_game_flag_get(4, 2) << 2) | ((unsigned)re15_game_flag_get(4, 3) << 3);
        PRUEFE(b4n == b4, "ROOM%04X %s Scheibe %d: Anklicken darf keine Scheibe fahren", raum, name, n);
    }
}

/* (C) Knoepfe vorn = Original-Nachricht, Original-Mechanik. */
static void fall_knopf(uint16_t raum, uint8_t *d, long sz, int strom, int hunde, const char *name)
{
    re15_rdt_t r;
    for (int n = 0; n < 4; n++) {
        if (raum_hoch(raum, d, sz, &r, strom, hunde, 0) != 0) { PRUEFE(0, "Parse"); return; }
        const int orig_id = hunde ? 2 : 0;
        char orig[200]; orig_text(orig_id, orig, sizeof orig);
        const int vorher = re15_game_flag_get(4, (uint8_t)n);
        lauf_t L = druecken(-4100 + 620, k_knopf_z[n], 2048);
        printf("  ROOM%04X %-11s Knopf %d: msg %d choice=%d msg3=%d (4,%d) %d->%d \"%s\"\n", raum, name, n,
               L.erste_id, L.choice, L.msg3, n, vorher, re15_game_flag_get(4, (uint8_t)n), L.text);
        PRUEFE(L.erste_id == orig_id, "ROOM%04X %s Knopf %d: Nachricht %d, Original %d", raum, name, n,
               L.erste_id, orig_id);
        PRUEFE(strcmp(L.text, orig) == 0, "ROOM%04X %s Knopf %d: Text \"%s\" != Original \"%s\"", raum, name,
               n, L.text, orig);
        PRUEFE(strstr(L.text, "target") == NULL, "ROOM%04X %s Knopf %d: Scheiben-Satz am Knopf", raum, name, n);
        if (!hunde) {
            PRUEFE(L.choice, "ROOM%04X %s Knopf %d: Ja/Nein-Frage verloren", raum, name, n);
            if (!strom) PRUEFE(L.msg3, "ROOM%04X %s Knopf %d: ohne Strom muss msg 3 folgen", raum, name, n);
            else PRUEFE(re15_game_flag_get(4, (uint8_t)n) != vorher,
                        "ROOM%04X %s Knopf %d: nach Ja muss die Scheibe fahren ((4,%d))", raum, name, n, n);
        } else {
            PRUEFE(!L.choice, "ROOM%04X %s Knopf %d: msg-2-Platz ist keine Frage", raum, name, n);
        }
    }
}

/* (D) Fahrt per Knopf: der Platz faehrt mit; hinten nichts mehr, vorn (vom Bahn-Ende, Blick +x) der Satz. */
static void fall_faehrt_mit(uint16_t raum, uint8_t *d, long sz, int n)
{
    re15_rdt_t r;
    if (raum_hoch(raum, d, sz, &r, 1, 0, 0) != 0) { PRUEFE(0, "Parse"); return; }
    lauf_t K = druecken(-4100 + 620, k_knopf_z[n], 2048);          /* "Ja" -> sub0(2+n) -> sub06 */
    PRUEFE(K.erste_id == 0 && re15_game_flag_get(4, (uint8_t)n) == 1,
           "ROOM%04X Fahrt %d: Knopf (msg %d, (4,%d)=%d)", raum, n, K.erste_id, n, re15_game_flag_get(4, (uint8_t)n));
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = -2800; p->z = -10900;
    int32_t min_abstand = 0x7fffffff;
    for (int i = 0; i < 400; i++) {                                 /* Fahrt sub06: 16 + 92 Bilder */
        scd_vm_tick(); re15_aot_scan(p->x, p->z, 0xFF);
        const int k = prop_von_obj(n);
        const re15_aot_t *a = &g_aot.slots[RE15_ZIEL1190_SLOT0 + n];
        if (k >= 0) {
            int32_t dd = a->x - g_scd.props[k].x; if (dd < 0) dd = -dd;
            if (i == 20 || i == 60) PRUEFE(dd == 0, "ROOM%04X Fahrt %d Bild %d: Platz %d / Scheibe %d", raum, n,
                                           i, a->x, g_scd.props[k].x);
            if (dd < min_abstand) min_abstand = dd;
        }
    }
    const int k = prop_von_obj(n);
    const re15_aot_t *a = &g_aot.slots[RE15_ZIEL1190_SLOT0 + n];
    printf("  ROOM%04X Fahrt Scheibe %d: Scheibe x=%d, Platz x=%d\n", raum, n, k >= 0 ? g_scd.props[k].x : 0, a->x);
    /* Fahrtende = -21340 + 150 * (16 + 92): sub06 Speed_set x +150 `2f 00 96 00` @0x0269A, For 16
     * `0d 00 04 00 10 00` @0x026A2 + For 92 `0d 00 04 00 5c 00` @0x026B0, je Durchgang ein Add_speed.
     * (Der Betreten-Wert von sub00 ist -5220 @0x02336 — die Original-Daten weichen um 80 ab.) */
    const int32_t ende = -21340 + 150 * (16 + 92);
    PRUEFE(k >= 0 && g_scd.props[k].x == ende, "ROOM%04X Fahrt %d: Scheibe steht bei x=%d, erwartet %d",
           raum, n, k >= 0 ? g_scd.props[k].x : 0, ende);
    PRUEFE(a->x == ende && a->z == k_scheibe_z[n], "ROOM%04X Fahrt %d: Platz bei (%d,%d)", raum, n, a->x, a->z);
    lauf_t H = druecken(k_halt_x[n], k_scheibe_z[n], 2048);         /* hinten: Scheibe weg */
    PRUEFE(H.erste_id < 0, "ROOM%04X Fahrt %d: hinten darf nichts mehr kommen (msg %d)", raum, n, H.erste_id);
    lauf_t V = druecken(ende - 45 - 450, k_scheibe_z[n], 0);        /* Bahn-Seite vor der vorderen Scheibe */
    const int viele = re15_ziel1190_viele(raum, n);
    printf("  ROOM%04X Scheibe %d vorn (Blick +x): msg %d \"%s\"\n", raum, n, V.erste_id, V.text);
    PRUEFE(V.erste_id == (viele ? (int)RE15_ZIEL1190_MSG_VIELE : (int)RE15_ZIEL1190_MSG_WENIGE) &&
           strcmp(V.text, viele ? k_viele : k_wenige) == 0,
           "ROOM%04X Fahrt %d: vorn msg %d \"%s\"", raum, n, V.erste_id, V.text);
}

static int zeilenbreite(const uint8_t *b, int von, int bis)
{
    int w = 0;
    for (int i = von; i < bis; i++) w += re15_font_width[b[i]];
    return w;
}

int main(void)
{
    long sz0 = 0, sz1 = 0;
    uint8_t *d0 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &sz0);
    uint8_t *d1 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1191.RDT", &sz1);
    if (!d0 || !d1) { printf("FAIL: RDT fehlt\n"); return 1; }

    /* Textform: Kopf 04 02 + Satz + 01 00 (= ROOM1190 msg 2 @0x2E5E), Umbruch 0x08, Zeilen <= 271 px. */
    for (int v = 0; v <= 1; v++) {
        uint8_t b[96];
        const int n = re15_ziel1190_text(v, b, (int)sizeof b);
        PRUEFE(n > 4 && b[0] == 0x04 && b[1] == 0x02 && b[n - 2] == 0x01 && b[n - 1] == 0x00,
               "Text %d: Kopf/Ende", v);
        int umbruch = -1;
        for (int i = 2; i < n - 2; i++) if (b[i] == 0x08) umbruch = i;
        PRUEFE(umbruch > 2, "Text %d: kein Zeilenumbruch", v);
        if (umbruch > 2) {
            const int w1 = zeilenbreite(b, 2, umbruch), w2 = zeilenbreite(b, umbruch + 1, n - 2);
            printf("  Text %s: %d Byte, Zeilen %d px / %d px\n", v ? "viele" : "wenige", n, w1, w2);
            PRUEFE(w1 <= 271 && w2 <= 271, "Text %d: Zeile zu breit (%d/%d)", v, w1, w2);
        }
        re15_msg_install_text(9, b, (size_t)n);
        PRUEFE(strcmp(re15_msg_get_text(9), v ? k_viele : k_wenige) == 0,
               "Text %d dekodiert zu \"%s\"", v, re15_msg_get_text(9));
    }
    /* Zuordnung: Leon 0/2 viele, Elza vertauscht, sonst kein Fall. */
    PRUEFE(re15_ziel1190_viele(0x1190, 0) == 1 && re15_ziel1190_viele(0x1190, 1) == 0 &&
           re15_ziel1190_viele(0x1190, 2) == 1 && re15_ziel1190_viele(0x1190, 3) == 0 &&
           re15_ziel1190_viele(0x1191, 0) == 0 && re15_ziel1190_viele(0x1191, 1) == 1 &&
           re15_ziel1190_viele(0x1191, 2) == 0 && re15_ziel1190_viele(0x1191, 3) == 1 &&
           re15_ziel1190_viele(0x1180, 0) < 0 && re15_ziel1190_viele(0x1190, 4) < 0, "Zuordnung viele/wenige");

    printf("(B) Scheiben hinten\n");
    fall_hinten(0x1190, d0, sz0, 0, 0, "Strom aus");
    fall_hinten(0x1190, d0, sz0, 1, 0, "Strom an");
    fall_hinten(0x1190, d0, sz0, 1, 1, "nach Hunden");
    fall_hinten(0x1191, d1, sz1, 0, 0, "Strom aus");
    fall_hinten(0x1191, d1, sz1, 1, 0, "Strom an");
    fall_hinten(0x1191, d1, sz1, 1, 1, "nach Hunden");

    printf("(C) Knoepfe = Original\n");
    fall_knopf(0x1190, d0, sz0, 0, 0, "Strom aus");
    fall_knopf(0x1190, d0, sz0, 1, 0, "Strom an");
    fall_knopf(0x1190, d0, sz0, 1, 1, "nach Hunden");
    fall_knopf(0x1191, d1, sz1, 1, 0, "Strom an");
    fall_knopf(0x1191, d1, sz1, 1, 1, "nach Hunden");

    printf("(D) Platz faehrt mit\n");
    fall_faehrt_mit(0x1190, d0, sz0, 0);
    fall_faehrt_mit(0x1191, d1, sz1, 3);

    printf("(E) Betreten mit Scheibe 1+2 vorn\n");
    {
        re15_rdt_t r;
        if (raum_hoch(0x1190, d0, sz0, &r, 1, 0, 0x6u) == 0) pruefe_plaetze(0x1190, "vorn 1+2", 0x6u);
        if (raum_hoch(0x1191, d1, sz1, &r, 1, 0, 0x6u) == 0) pruefe_plaetze(0x1191, "vorn 1+2", 0x6u);
    }

    /* Anderer Raum: keine Plaetze, kein Abfangen. */
    {
        g_current_room_id = 0x1180;
        re15_aot_init();
        re15_ziel1190_install(0x1180);
        PRUEFE(!g_aot.slots[RE15_ZIEL1190_SLOT0].active, "ROOM1180: Platz angelegt");
        PRUEFE(re15_ziel1190_show(RE15_ZIEL1190_MSG_VIELE, 0) == 0, "ROOM1180: show abgefangen");
    }

    free(d0); free(d1);
    printf("test_r36_ziel1190: %d Pruefungen\n", s_pruef);
    if (s_fail) { printf("test_r36_ziel1190: FAIL\n"); return 1; }
    printf("test_r36_ziel1190: OK\n");
    return 0;
}
