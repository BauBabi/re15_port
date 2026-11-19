/* test_r42_finale_abspann.c — Runde 42 Strang finale, Plan S7 (Pruefbericht s1 r1: "Riegel fehlen: ohne Bit 129 bleiben Tuer 5 und
 * sub02 gesperrt").
 *
 * ROOM5090/5091 fuehren den Abspann-Vorspann sub02 (5090 @0x012C2: Set(2,7,1), Set(1,27,1) @0x012C6, Sleep 20, Message_on 0 "THANK YOU
 * FOR PLAYING !!!", Sleep 100, Aot_on(5) @0x012D6 -> Tuer Slot 5 -> ROOM6040/6041). Gestartet wird er von sub01 bei Ck(5,0x1C)
 * (5090 @0x012A6 -> Evt_exec sub02 @0x012BA). Im Port sind beide Wege gesperrt, bis das Finale beendet ist (Bank 9 Bit 129
 * RE15_R42_BIT_FINALE_ENDE, gesetzt im Kill-Bild finale_r42.c = RE2 FUN_80018fb0 @0x80103d7c): Streichliste op_evt_exec (scd_vm.c) und
 * Tuer-Riegel aot_fire_door (aot_common.c). Geprueft je Raum, echte RDT, echter SCD-Takt:
 *   ohne Bit 129: (5,0x1C) gesetzt -> 200 Bilder lang kein Kinobalken (1,27) und kein Raumwechsel; Tuer 5 direkt gefeuert -> kein
 *                 Raumwechsel.
 *   mit Bit 129:  derselbe Ablauf -> Kinobalken an, nach Sleep 20 + 100 (@0x012CA/@0x012D2) Raumwechsel nach 0x6040 bzw. 0x6041.
 * Gegenprobe: auf aaea5f8c2 (vor der Bindung an Bit 129) war die Sperre bedingungslos (sub02 immer gestrichen) -> der Teil "mit
 * Bit 129" ist dort rot.
 *
 * STUFE 5 (NUTZERWUNSCH 18.11.2026 "das thank you for Playing fliegt vollständig raus, und es kommt ein richtiges Endvideo";
 * include/re15_endvideo_r42.h): mit Bit 129 laeuft statt sub02 das Port-Programm in der RE2-Form ROOM4090. Geprueft je Raum in drei
 * Plattform-Lagen (keine Plattform / Datei fehlt / Film gespielt; die Plattform wird wie in platform/pc/main.c VOR dem Bild bedient):
 *   - Meldung 0 "THANK YOU FOR PLAYING !!!" erscheint in keinem Bild (re15_msg_tick).
 *   - Abblenden Kanal 2: ab dem ersten gezeichneten Bild steigt die Helligkeit um 4 je Bild (Schritt 0x200 @0x027F8 >> 7) bis 252,
 *     64 Bilder (0x8000 / 0x200); danach bis zum Raumwechsel KEIN Bild ohne Kachel (Bit 15) und keins heller als 240 vor dem Film
 *     (das Halten @0x02812 kommt vor dem Auslaufen).
 *   - Film-Zeile 60 nach dem Abblenden (Helligkeit dort >= 252), genau einmal; mit Film: eines der ersten zwei Bilder danach zeigt 255
 *     (Sce_fade_adjust 0x7FFF @0x0281C), dann je Bild 4 weniger.
 *   - Raumwechsel nach 0x6040 bzw. 0x6041 erst nach der Film-Zeile, hoechstens drei Bilder danach (Evt_next @0x0281A + Aot_on).
 * Gegenprobe Stufe 5: auf c5657d3ed (Stufe 4) zeigt sub02 Meldung 0 und hat keine Blende/Film-Zeile -> rot.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_story_r42.h"
#include "re15_fade.h"
#include "re15_zws_r41.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static re15_rdt_t s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t s_ctx;
static uint8_t *s_raw;

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

/* Plattform-Lage fuer die Film-Zeile: 0 = keine Plattform (PSX), 1 = Datei fehlt, 2 = Film gespielt. */
static int s_lage = 0;
static int s_msg0 = 0;          /* Meldung 0 in einem Bild gezeigt */
static int s_film_bild = -1;    /* Bild, in dem die Plattform den Film bedient hat */
static int s_bildnr = 0;

static void bild(void)
{
    const unsigned char *raw = NULL; int len = 0, id = -1;
    /* platform/pc/main.c: re15_zws_r41_pc_bild() VOR re15_render_begin_frame -> der Film laeuft zwischen zwei Spielbildern */
    if (re15_zws_r41_film_offen()) {
        if (s_film_bild < 0) s_film_bild = s_bildnr;
        if (s_lage == 2) re15_zws_r41_film_gespielt(); else re15_zws_r41_film_fehlt();
    }
    scd_vm_tick();
    if (re15_msg_tick(&raw, &len, &id) && id == 0) s_msg0 = 1;
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    re15_fade_tick();           /* render_pc.c re15_render_end_frame: FUN_80021880 nach dem Spieltakt */
    s_bildnr++;
}

/* Raum aufbauen wie probe_r18 (aaea5f8c2): RDT, Akteure, AOT, SCD, Raumereignisse, Wiedereintritt auf Schnitt 12. Kampf als gelaufen
 * markiert (3,42), damit sub01 nicht sub04 (Kampfstart) startet. */
static int raum(const char *rel, unsigned id, int bit129)
{
    size_t n = 0;
    char pfad[512];
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    free(s_raw);
    s_raw = slurp(pfad, &n);
    if (!s_raw || re15_rdt_parse(s_raw, n, &s_rdt) != 0) { CHECK(0, "%s nicht lesbar", rel); return 0; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 12;
    re15_game_state_init();
    re15_fade_init();
    re15_zws_r41_install((uint16_t)id);
    re15_zws_r41_film_spieler = (s_lage != 0);
    s_msg0 = 0; s_film_bild = -1;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 8000; pl->y = 0; pl->z = -23500; pl->rot_y = 2048;
    re15_collision_set_band(0);
    re15_game_flag_set(3, 42, 1);                                     /* Kampf gelaufen (sub01 @0x01284 Ck(3,42)=0 -> sub04) */
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE, bit129);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 12);
    re15_game_flag_set(3, 42, 1);
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE, bit129);
    for (int f = 0; f < 5; f++) bild();
    return 1;
}

static void lauf(const char *rel, unsigned id, unsigned ziel, int lage)
{
    static const char *const k_lage[3] = { "keine Plattform", "Datei fehlt", "Film gespielt" };
    s_lage = lage;
    for (int bit = 0; bit <= 1; bit++) {
        if (bit == 0 && lage != 0) continue;                          /* die Sperre einmal je Raum */
        if (!raum(rel, id, bit)) return;
        re15_game_flag_set(5, 0x1c, 1);                               /* Abspann-Ausloeser (5,0x1C), wie das Kill-Bild */
        const unsigned filme0 = g_re15_zws_filme;
        int balken = 0, wechsel = -1;
        int erstes = -1, schwarz = -1, loch = -1, zu_hell = -1, schritt_falsch = -1, film_zeile = -1, hell_film = -1;
        int vorher = -1, nach_film[3] = { -1, -1, -1 };
        s_bildnr = 0;
        for (int f = 0; f < 400 && wechsel < 0; f++) {
            const unsigned filme_vor = g_re15_zws_filme;
            bild();
            if (re15_game_flag_get(1, 27)) balken = 1;               /* sub02 Set(1,27,1) @0x012C6 */
            const re15_fade_ch_t *c = &g_fade_ch[2];
            const int hell = c->drawn ? (int)c->out_r : -1;           /* Kachel dieses Bilds (vor dem Schritt) */
            if (g_re15_zws_filme != filme_vor && film_zeile < 0) { film_zeile = f; hell_film = hell; }
            if (hell >= 0 && erstes < 0) erstes = f;
            if (erstes >= 0 && film_zeile < 0) {
                if (hell < 0 && loch < 0) loch = f;                   /* Kachel weg = Spielbild blitzt durch */
                if (hell >= 0 && vorher >= 0 && schwarz < 0 && hell != vorher + 4 && schritt_falsch < 0) schritt_falsch = f;
                if (hell >= 252 && schwarz < 0) schwarz = f;
                if (schwarz >= 0 && hell >= 0 && hell < 240 && zu_hell < 0) zu_hell = f;
            }
            if (film_zeile >= 0 && f > film_zeile && f <= film_zeile + 3) nach_film[f - film_zeile - 1] = hell;
            vorher = hell;
            if (g_room_change.pending) wechsel = f;
        }
        const unsigned dest = g_room_change.pending ? g_room_change.room_id : 0u;
        if (!bit) {
            CHECK(!balken, "%04X ohne Bit 129: sub02 lief (Kinobalken (1,27) an)", id);
            CHECK(wechsel < 0, "%04X ohne Bit 129: Raumwechsel nach %04X im Bild %d", id, dest, wechsel);
            g_room_change.pending = 0;
            re15_aot_fire_slot(5);                                    /* Tuer 5 direkt (Rect 0, nur ueber Aot_on erreichbar) */
            for (int f = 0; f < 3; f++) bild();
            CHECK(!g_room_change.pending, "%04X ohne Bit 129: Tuer 5 oeffnet nach %04X", id, g_room_change.room_id);
            fprintf(stderr, "%04X Bit 129 = 0: Kinobalken %d, Raumwechsel %04X im Bild %d\n", id, balken, dest, wechsel);
            continue;
        }
        CHECK(balken, "%04X %s: sub02/Port-Programm lief nicht (kein Kinobalken)", id, k_lage[lage]);
        CHECK(!s_msg0, "%04X %s: Meldung 0 \"THANK YOU FOR PLAYING !!!\" erscheint (NUTZERWUNSCH 18.11.: raus)", id, k_lage[lage]);
        CHECK(wechsel >= 0 && dest == ziel, "%04X %s: Raumwechsel nach %04X im Bild %d (soll %04X)", id, k_lage[lage], dest, wechsel,
              ziel);
        CHECK(erstes >= 0, "%04X %s: keine Abblende auf Kanal 2 (RE2 ROOM4090 @0x027F4)", id, k_lage[lage]);
        CHECK(schritt_falsch < 0, "%04X %s: Abblende nicht +4 je Bild (Schritt 0x200 @0x027F8), Bild %d", id, k_lage[lage],
              schritt_falsch);
        CHECK(schwarz >= 0 && schwarz - erstes == 63,
              "%04X %s: Schwarz (>= 252) im Bild %d, erste Kachel %d (soll 64 Bilder, 0x8000/0x200)", id, k_lage[lage], schwarz, erstes);
        CHECK(loch < 0, "%04X %s: Bild %d ohne Kachel zwischen Abblenden und Film (Spielbild blitzt)", id, k_lage[lage], loch);
        CHECK(zu_hell < 0, "%04X %s: Bild %d wieder heller als 240 vor dem Film", id, k_lage[lage], zu_hell);
        CHECK(film_zeile >= 0 && g_re15_zws_filme - filme0 == 1, "%04X %s: Film-Zeile %d mal erreicht (Bild %d), soll genau einmal",
              id, k_lage[lage], (int)(g_re15_zws_filme - filme0), film_zeile);
        CHECK(film_zeile < 0 || hell_film >= 252, "%04X %s: Film-Zeile bei Helligkeit %d (soll schwarz, >= 252)", id, k_lage[lage],
              hell_film);
        CHECK(film_zeile < 0 || (schwarz >= 0 && film_zeile > schwarz), "%04X %s: Film-Zeile im Bild %d vor dem Schwarz (Bild %d)",
              id, k_lage[lage], film_zeile, schwarz);
        CHECK(wechsel < 0 || film_zeile < 0 || (wechsel > film_zeile && wechsel <= film_zeile + 3),
              "%04X %s: Raumwechsel im Bild %d, Film-Zeile im Bild %d (soll direkt danach: Evt_next @0x0281A, Aot_on)",
              id, k_lage[lage], wechsel, film_zeile);
        if (lage == 2) {
            const re15_zws_film_t *fa = re15_zws_r41_aktiv();
            CHECK(s_film_bild >= 0 && fa && fa->raum == (id & 0xFFFFu), "%04X Film gespielt: Plattform nicht bedient oder falscher Film (%s)",
                  id, fa ? fa->kennung : "-");
            /* nach dem Film: Pegel 0x7FFF (@0x0281C) -> 255, dann -4 je Bild (Schritt -0x200 @0x02816) */
            CHECK(nach_film[0] == 255 || nach_film[1] == 255, "%04X Film gespielt: nach dem Film Helligkeit %d/%d/%d (soll 255, dann -4)",
                  id, nach_film[0], nach_film[1], nach_film[2]);
        }
        fprintf(stderr, "%04X Bit 129, %s: Kinobalken %d, Meldung 0 %d, Kachel ab Bild %d, schwarz Bild %d, Film-Zeile Bild %d "
                        "(Helligkeit %d), Plattform Bild %d, danach %d/%d/%d, Raumwechsel %04X im Bild %d\n",
                id, k_lage[lage], balken, s_msg0, erstes, schwarz, film_zeile, hell_film, s_film_bild,
                nach_film[0], nach_film[1], nach_film[2], dest, wechsel);
    }
}

int main(void)
{
    for (int lage = 0; lage < 3; lage++) {
        lauf("STAGE5/ROOM5090.RDT", 0x5090u, 0x6040u, lage);
        lauf("STAGE5/ROOM5091.RDT", 0x5091u, 0x6041u, lage);
    }
    free(s_raw);
    if (fails) { fprintf(stderr, "test_r42_finale_abspann: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_r42_finale_abspann: OK\n");
    return 0;
}
