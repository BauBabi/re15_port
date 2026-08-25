/* test_1210_gitterhaende.c — PIN: die Gitterhaende in ROOM1210 reagieren EINZELN,
 * wenn der Spieler an ihnen vorbeigeht.
 *
 * ⛔ BEWUSSTE NACHRUESTUNG (Nutzer-Auftrag), kein byte-true-Nachbau:
 * "im room 1210 strecken ab einem gewissen punkt alle Zombies die Haende aus dem gitter.
 *  das ist sau schlecht gemacht ... in Resident Evil 2 gibt es einen Flur, wo man
 *  durchlaeuft, und kommt man am Fenster vorbei, kommen die Haende der Zombies mit stoehnen
 *  und greifen einen. das wuerde ich gerne auch hier so umgesetzt haben wollen."
 *
 * Das Original schaltet ALLE ZEHN gleichzeitig (ROOM1210 sub02 `Member_set(12,1)`
 * @Datei 0x001EDA) — genau das war der Kritikpunkt. Statt dessen entscheidet jeder Arm
 * fuer sich; die Belege fuer Clips, Stoehnen, Griff und die beiden Reichweiten stehen im
 * Kopf von re15_writher_ai_tick case 1.
 *
 * DIE WACHE IST NICHT VAKUANT: sie laeuft den Flur wirklich ab (der Spieler wird Bild fuer
 * Bild an der gemessenen Arm-Reihe entlang bewegt) und verlangt DREI Dinge:
 *   (1) Es reagieren ueberhaupt Arme — sonst ist die Nachruestung tot.
 *   (2) Sie reagieren NICHT alle gleichzeitig — das ist der eigentliche Nutzer-Punkt.
 *       Gemessen wird die groesste Zahl gleichzeitig ausgefahrener Arme.
 *   (3) Jeder Arm kehrt in die Ruhe zurueck, wenn der Spieler weiter ist — sonst waere aus
 *       "alle auf einmal" nur "alle nacheinander, und dann bleiben sie" geworden.
 *   (4) Wer hineinlaeuft, wird WIRKLICH FESTGEHALTEN (Nutzer-Auftrag "Ja, stell das um, wenn
 *       dir da was fehlt, hole die Animation aus Resident Evil 2"): der Spieler geht in den
 *       Opfer-Zustand, spielt die Ringkampf-Clips der geliehenen Opfer-Bank, verliert beim
 *       Biss GENAU die 20 HP aus RE2s Parameter-Tabelle @0x80100014 — und kommt wieder frei.
 *       Der Griff darf sich also weder wegkuerzen noch festfressen.
 * Dazu die Gegenprobe, dass KEIN Arm seine Position veraendert (er steckt im Gitter).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"   /* re15_damage_seed_rng */
#include "re15_skeleton.h" /* re15_sin_q12/cos_q12 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob[0x80000];
static uint8_t            s_blob_donor[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Eine Bank aus CDEMD0.EMS in die Registry legen — wie pc_enemy_load, ohne Renderer.
 * Ohne sie ist clip_len 0 und die Phasen laufen im Setz-Bild durch. */
static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len,
                                                          &eb->skel_loco, &eb->anim_loco) == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len,
                                                            &eb->skel_victim, &eb->anim_victim) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1210: die Gitterhaende reagieren einzeln ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    CHECK(load_bank(0x1A, s_blob, sizeof s_blob),
          "EM01A-Bank geladen — ohne sie ist clip_len 0 und die Wache blind");
    /* OPFER-BANK-LEIHGABE wie auf der Plattform (main.c, Block "OPFER-BANK-LEIHGABE"):
     * EM01A hat kein Paar 3, der Arm leiht sich das des Zombies (0x10). */
    int donor = load_bank(0x10, s_blob_donor, sizeof s_blob_donor);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    CHECK(donor && db && db->victim_ok,
          "EM010-Opferbank als Leihgabe geladen (victim_ok=%d, %d Clips) — ohne sie kann der "
          "Arm nur zupacken statt festzuhalten", db ? db->victim_ok : -1,
          (db && db->victim_ok) ? db->anim_victim.clip_count : -1);
    if (db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    for (int f = 0; f < 8; f++) frame_step();

    /* Die Arme einsammeln + ihre Startpositionen sichern. */
    int   slots[RE15_ACTOR_MAX], n = 0;
    int32_t sx[RE15_ACTOR_MAX], sz2[RE15_ACTOR_MAX];
    int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n] = s; sx[n] = e->x; sz2[n] = e->z; n++;
        if (e->z < zmin) zmin = e->z;
        if (e->z > zmax) zmax = e->z;
    }
    printf("  %d Arme, z-Spanne %ld .. %ld\n", n, (long)zmin, (long)zmax);
    CHECK(n >= 8, "mindestens 8 Arme im Raum (%d) — sonst misst die Wache nichts", n);

    /* DEN FLUR ABLAUFEN: der Spieler faehrt in der Flurmitte von z(max) nach z(min). */
    int32_t mid_x = -19500;                 /* Flurmitte: Arme auf -25000 und -14000 */
    int reacted[RE15_ACTOR_MAX]; memset(reacted, 0, sizeof reacted);
    int returned[RE15_ACTOR_MAX]; memset(returned, 0, sizeof returned);
    int max_at_once = 0, moved = 0;
    int32_t step = 75;   /* GEHTEMPO, byte-true: WALK = 0x4B pro Bild aus den
                          * Modus-Tabellen 0x80076cXX (FUN_80041BE4) — nicht geschaetzt. */

    for (int32_t z = zmax + 3000; z > zmin - 3000; z -= step) {
        pl->x = mid_x; pl->z = z; pl->hp = 100;
        frame_step();
        int at_once = 0;
        for (int i = 0; i < n; i++) {
            re15_actor_t *e = &g_actors[slots[i]];
            int out = (e->sub_state_1 >= 1 && e->sub_state_1 <= 2)
                   || (e->sub_state_1 >= 4 && e->sub_state_1 <= 5);   /* 4/5 = Halten/Abwerfen */
            if (out) { at_once++; reacted[i] = 1; }
            else if (reacted[i]) returned[i] = 1;
            if (e->x != sx[i] || e->z != sz2[i]) moved++;
        }
        if (at_once > max_at_once) max_at_once = at_once;
    }

    int n_reacted = 0, n_returned = 0;
    for (int i = 0; i < n; i++) { n_reacted += reacted[i]; n_returned += returned[i]; }
    printf("  Durchlauf: %d von %d Armen haben reagiert, %d davon sind wieder zur Ruhe gekommen; "
           "hoechstens %d gleichzeitig ausgefahren\n", n_reacted, n, n_returned, max_at_once);

    /* (1) Es reagiert ueberhaupt etwas. */
    CHECK(n_reacted >= 6,
          "die Arme reagieren beim Vorbeigehen (%d von %d) — vorher stand hier Clip 0 fuer immer",
          n_reacted, n);
    /* (2) Der eigentliche Nutzer-Punkt: NICHT alle auf einmal. */
    CHECK(max_at_once < n,
          "und sie fahren NICHT alle gleichzeitig aus (hoechstens %d von %d gleichzeitig) — "
          "genau das war der Kritikpunkt am Original, das per Member_set(12,1) @0x001EDA alle "
          "zehn auf einen Schlag schaltet", max_at_once, n);
    CHECK(max_at_once <= 4,
          "es reagiert jeweils nur die unmittelbare Nachbarschaft (%d gleichzeitig; "
          "REACH_Z 1700 = Tiefe des Original-Rechtecks @0x1EAE, Arm-Abstand 1200..1396)",
          max_at_once);
    /* (3) Sie kommen wieder zur Ruhe. */
    CHECK(n_returned >= 4,
          "und sie ziehen sich wieder zurueck, wenn der Spieler weiter ist (%d von %d)",
          n_returned, n_reacted);
    /* ZWEITER DURCHLAUF — AN EINEM WIRKLICH ERREICHBAREN ARM VORBEI.
     *
     * ⛔ DIE FALLE, DIE HIER GEMESSEN WERDEN MUSS: die Kreaturen stehen HINTER dem Gitter,
     * also GROESSTENTEILS AUSSERHALB des begehbaren Bodens. Gemessen (probe_1210_griff,
     * 2D-Abtastung des Bodens im 50er-Raster):
     *     Arm  1 (-25000, -8847) -> naechster Boden 2792 entfernt
     *     Arm  2 (-25000,-10247) -> 2839      Arm  4 (-14000, -7297) -> 1348
     *     Arm  5 (-25000,-15747) -> 2246      Arm  7..10 (-14000, …) -> je 2697
     *     Arm  3 (-14000, -5897) ->    0      Arm  6 (-25000,-17130) ->  849
     * Nur ZWEI der zehn liegen im Griff-Tor (< 1200, `sltiu v0,s2,0x4b0` @0x801018f4); die
     * anderen acht kann ein Spieler ueberhaupt nicht beruehren — sie strecken die Haende raus
     * und stoehnen, mehr nicht. Und das Modell holt das nicht auf: EM01A misst gemessen nur
     * 442 Einheiten Radius im Ausfahr-Clip (680 im laengsten Clip).
     *
     * Ein Durchlauf, der den Spieler einfach auf x = -24400 setzt, teleportiert ihn IN DIE
     * WAND und misst damit nichts, was im Spiel passieren kann. Deshalb sucht die Wache den
     * naechstgelegenen BEGEHBAREN Punkt und laeuft dort vorbei — auf echtem Boden.
     * Solange der Arm haelt, laeuft der Spieler NICHT weiter (das ist der Sinn eines
     * Festhaltens) — der Fortschritt pausiert, statt den Griff wegzuschieben. */
    int32_t gx = 0, gz = 0, gd = 0x7fffffff; int gate_arm = -1;
    for (int i = 0; i < n; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        for (int32_t x = e->x - 4000; x <= e->x + 4000; x += 50)
            for (int32_t z = e->z - 4000; z <= e->z + 4000; z += 50) {
                if (!re15_collision_on_floor(&s_rdt, x, z)) continue;
                re15_actor_t probe = *pl; probe.x = x; probe.z = z;
                int32_t dd = re15_enemy_player_dist(e, &probe);
                if (dd < gd) { gd = dd; gx = x; gz = z; gate_arm = slots[i]; }
            }
    }
    printf("  naechster begehbarer Punkt an einem Arm: slot %d, (%ld,%ld), Abstand %ld\n",
           gate_arm, (long)gx, (long)gz, (long)gd);
    CHECK(gate_arm >= 0 && gd < 0x4b0,
          "mindestens ein Arm ist von BEGEHBAREM Boden aus im Griff-Tor (Abstand %ld < 1200, "
          "`sltiu v0,s2,0x4b0` @0x801018f4) — sonst waere der ganze Griff unerreichbar und "
          "jede weitere Zusage darueber vakuant", (long)gd);

    int held_frames = 0, grabs = 0, longest_hold = 0, run = 0, freed = 0;
    int bites = 0, bad_bite = 0, bite_dmg_seen = 0;
    int victim_clip_seen = 0, prev_victim = 0;

    /* VOR dem Arm entlanggehen, nicht neben ihm: RE2s Tor sind zwei Halb-Sektoren um
     * Yaw ± 256 (`addiu a2,a2,256` / `addiu a3,zero,256` @0x8010193c-4c), zusammen also ein
     * 45-Grad-Kegel NACH VORN. Wer seitlich an einem Arm vorbeilaeuft, wird nicht gegriffen.
     * Die Bahn ist deshalb die Senkrechte zur Blickrichtung, 600 Einheiten davor — und jeder
     * Punkt wird gegen den begehbaren Boden geprueft. */
    re15_actor_t *ga = &g_actors[gate_arm];
    int32_t fsn = re15_sin_q12(((int)ga->rot_y + 0x400) & 0xfff);   /* Blickrichtung */
    int32_t fcs = re15_cos_q12(((int)ga->rot_y + 0x400) & 0xfff);
    /* Den BEGEHBAREN Punkt suchen, der dem Arm am naechsten liegt UND vor ihm — die
     * Skalarprojektion auf die Blickrichtung muss positiv sein, sonst steht der Spieler
     * hinter dem Gitter. Von dort aus wird nicht gelaufen, sondern GEWARTET: Arm 3 sitzt an
     * einer Quergang-Oeffnung, der begehbare Streifen davor ist nur wenige Bilder breit
     * (2D-Abtastung oben), und der Ausfahr-Clip 2 braucht allein 30 Bilder. "Hineinlaufen und
     * stehenbleiben" ist damit der Fall, den ein Spieler wirklich herstellen kann. */
    int32_t sx0 = 0, sz0 = 0, sd = 0x7fffffff;
    for (int32_t x = ga->x - 3000; x <= ga->x + 3000; x += 25)
        for (int32_t z = ga->z - 3000; z <= ga->z + 3000; z += 25) {
            if (!re15_collision_on_floor(&s_rdt, x, z)) continue;
            int64_t proj = (int64_t)(x - ga->x) * fsn + (int64_t)(z - ga->z) * fcs;
            if (proj <= 0) continue;                       /* hinter dem Arm */
            re15_actor_t probe = *pl; probe.x = x; probe.z = z;
            int32_t dd = re15_enemy_player_dist(ga, &probe);
            if (dd < sd) { sd = dd; sx0 = x; sz0 = z; }
        }
    printf("  Standpunkt VOR Arm %d: (%ld,%ld), Abstand %ld\n",
           gate_arm, (long)sx0, (long)sz0, (long)sd);
    CHECK(sd < 0x4b0,
          "und dieser Punkt liegt VOR dem Arm im Griff-Kegel (Abstand %ld) — RE2s Tor sind zwei "
          "Halb-Sektoren um Yaw +-256 (@0x8010193c-4c), seitlich vorbei wird nicht gegriffen",
          (long)sd);

    for (int f = 0; f < 400; f++) {
        if (!re15_player_is_grabbed()) { pl->x = sx0; pl->z = sz0; }
        int16_t hp_before = pl->hp;
        frame_step();
        int vs = re15_player_victim_state();
        if (vs) {
            held_frames++; run++;
            if (!prev_victim) grabs++;
            /* Die Opfer-Clips der geliehenen Bank: Intro 0 / Halten 1 / Loesen 2 (Variante 0,
             * re15_victim_clip_map). Ein Griff, in dem Leon seine Steh-Clips behielte, waere
             * kein Festhalten, sondern nur ein eingefrorener Spieler. */
            if (pl->motion <= 2) victim_clip_seen = 1;
        } else {
            if (prev_victim) { if (run > longest_hold) longest_hold = run; freed++; }
            run = 0;
        }
        prev_victim = (vs != 0);
        if (pl->hp < hp_before) {
            int dmg = (int)hp_before - (int)pl->hp;
            bites++; bite_dmg_seen = dmg;
            if (dmg != 20) bad_bite++;      /* Tabelle @0x80100014: stehend = 20 HP */
        }
        pl->hp = 100;                        /* am Leben halten, damit der Flur zu Ende laeuft */
    }
    if (run > longest_hold) longest_hold = run;

    /* (4) Es ist ein echtes FESTHALTEN — und es endet wieder. */
    printf("  Griff: %d Zugriffe, %d Bilder im Opfer-Zustand, laengster Griff %d Bilder, "
           "%d mal wieder frei; %d Bisse (zuletzt %d HP)\n",
           grabs, held_frames, longest_hold, freed, bites, bite_dmg_seen);
    CHECK(grabs >= 1,
          "wer hineinlaeuft, wird festgehalten (%d Zugriffe) — vorher war es nur ein Zupacken "
          "mit Kontakt-Schaden, weil EM01A keine Opfer-Bank hat", grabs);
    CHECK(victim_clip_seen,
          "und Leon spielt dabei die Ringkampf-Clips der geliehenen Opfer-Bank (Intro 0 / "
          "Halten 1 / Loesen 2, re15_victim_clip_map) — nicht seine Steh-Animation");
    CHECK(bites >= 1 && bad_bite == 0,
          "der Biss kostet GENAU die 20 HP aus RE2s Parameter-Tabelle @0x80100014 "
          "(`10 14 01 05 …`, stehender Fall): %d Bisse, %d davon mit anderem Betrag",
          bites, bad_bite);
    CHECK(freed >= grabs,
          "und jeder Griff endet wieder (%d Zugriffe, %d mal frei) — RE2s Ringkampf-Budget 148 "
          "@0x80102828-2C, pro Bild -= 2 + 5*Taste @0x80102868-7C", grabs, freed);
    CHECK(longest_hold > 0 && longest_hold < 400,
          "kein Festfressen: der laengste Griff dauert %d Bilder (Budget 148 / 2 pro Bild = 74 "
          "Halte-Bilder plus Loese-Clip)", longest_hold);

    /* Gegenprobe: ein Arm steckt im Gitter und bewegt sich NICHT. */
    CHECK(moved == 0,
          "kein Arm hat seine Position veraendert (%d Abweichungen) — die Original-Maschine "
          "wuerde mit +0x8c = 0x320 (@0x8010c7b8) rund 2400 Einheiten vorschnellen, ein im "
          "Gitter steckender Arm kann das nicht", moved);

    free(buf);
    if (fails) { printf("\n1210 GITTERHAENDE: FAIL (%d)\n", fails); return 1; }
    printf("\n1210 GITTERHAENDE: sie kommen einzeln, wenn man vorbeigeht\n");
    return 0;
}
