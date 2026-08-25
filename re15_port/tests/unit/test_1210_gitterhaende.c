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
#include "re15_math.h"   /* re15_squareroot0 */

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

    /* Die Kollisions-BAND-Wahl folgt im Spiel dem Spieler-y; der Flur-Durchlauf oben hat
     * sie verstellt. Fuer die Abtastung zurueck auf Band 0 (pl->y ist 0). */
    re15_collision_set_band(0);
    int32_t gx = 0, gz = 0, gd = 0x7fffffff; int gate_arm = -1;
    int32_t glx = 0, glz = 0;
    for (int i = 0; i < n; i++) {
        re15_actor_t *e = &g_actors[slots[i]];
        int32_t fs = re15_sin_q12(((int)e->rot_y + 0x400) & 0xfff);
        int32_t fc = re15_cos_q12(((int)e->rot_y + 0x400) & 0xfff);
        int32_t ex = e->x + (int32_t)((fs * 2420) >> 12);      /* Landepunkt der Lunge */
        int32_t ez = e->z + (int32_t)((fc * 2420) >> 12);
        /* Den begehbaren Streifen auf der Hoehe des Landepunkts abtasten und den Rand nehmen,
         * der ihm am naechsten liegt. (Die Abtastung ist dieselbe wie in probe_1210_griff;
         * ein Constrain-Lauf von der Flurmitte aus ist hier nicht verwendbar, weil die
         * Flurmitte auf mancher z-Zeile selbst nicht begehbar ist.) */
        int32_t px = 0, pz = ez; int have = 0; int32_t bestd = 0x7fffffff;
        for (int32_t xx = -30000; xx <= -8000; xx += 25) {
            if (!re15_collision_on_floor(&s_rdt, xx, pz)) continue;
            int32_t dd0 = xx - ex; if (dd0 < 0) dd0 = -dd0;
            if (!have || dd0 < bestd) { bestd = dd0; px = xx; have = 1; }
        }
        if (!have) continue;
        int32_t ddx = px - ex, ddz = pz - ez;
        int32_t dd = (int32_t)re15_squareroot0((uint32_t)((int64_t)ddx * ddx +
                                                          (int64_t)ddz * ddz));
        if (dd < gd) { gd = dd; gx = px; gz = pz; glx = ex; glz = ez; gate_arm = slots[i]; }
    }
    printf("  bester Arm: slot %d, Landepunkt (%ld,%ld), erreichbar bis (%ld,%ld), Abstand %ld\n",
           gate_arm, (long)glx, (long)glz, (long)gx, (long)gz, (long)gd);
    CHECK(gate_arm >= 0,
          "die Erreichbarkeits-Messung findet ueberhaupt einen Arm (slot %d, gemessener "
          "Abstand vom Landepunkt zum naechsten begehbaren Punkt: %ld)", gate_arm, (long)gd);
    /* ⛔ KEINE Zusage auf einen Schwellwert. Was hier steht, ist eine MESSUNG: wie nah der
     * begehbare Boden dem Landepunkt der Lunge kommt. Ob das im Spiel fuer einen Griff
     * reicht, haengt zusaetzlich an den Hindernissen im Flur (re15_collision_constrain laesst
     * auf der Zeile von Arm 3 nur bis x = -18164 zu, obwohl dort Boden bis -10400 liegt) und
     * ist NICHT als bestanden/durchgefallen formuliert, weil ich es nicht belegen kann.
     * Die MECHANIK des Griffs wird darunter separat geprueft. */

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
            {   int32_t d = e->x - sx[i]; if (d < 0) d = -d;
                int32_t q = e->z - sz2[i]; if (q < 0) q = -q;
                int32_t far = (d > q) ? d : q;
                if (far > moved) moved = far;      /* groesster Ausschlag ueber den Durchlauf */
            }
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
    int held_frames = 0, grabs = 0, longest_hold = 0, run = 0, freed = 0;
    int bites = 0, bad_bite = 0, bite_dmg_seen = 0;
    int victim_clip_seen = 0, prev_victim = 0;
    int32_t far_from_arm = 0;

    /* ---------------------------------------------------------------------------------
     * (4) DER GRIFF - am ECHTEN Fall, nicht mehr an einer gesetzten Stellung.
     *
     * WARUM DAS VORHER NICHT GING, und was sich geaendert hat (alles gemessen):
     * Der Spieler haengt an seinem Klemmer (Radius 450 \n0x80073e9a, Maske 1 \n0x80031d74),
     * der Arm an seinem eigenen (Radius 300 \n0x80120922, Maske 4 \n0x8010c318), und
     * dazwischen steht die SCA-Wand - 1060 dick im Westen, 1020 im Osten. Der
     * kleinstmoegliche URSPRUNGS-Abstand ist damit hart 1828 bzw. 1789 und liegt ueber
     * RE2s Tor 1200. Ein Ursprungs-Tor kann hier also NIE feuern, egal wie weit die Lunge
     * traegt. Das MESH reicht aber 1671 Einheiten nach vorn (Ausfahr-Clip 2), deshalb misst
     * das Tor jetzt die HAND. Belege im Kopf von re15_writher_ai_tick case 2.
     *
     * Die Wache stellt den Spieler dorthin, wo die KOLLISION ihn wirklich hinlaesst: an den
     * Rand des begehbaren Streifens, ermittelt mit re15_collision_constrain - kein Teleport
     * in die Wand. */
    re15_actor_t *ga = &g_actors[gate_arm];
    int32_t sx0 = 0, sz0 = ga->z; int have_edge = 0;
    /* KEIN re15_collision_on_floor-Vortest: in diesem Raum liefert die Funktion 1
     * INNERHALB einer Wand-Zelle, der Spieler laeuft im Komplement (gemessen). Der
     * Constrain-Lauf selbst ist der richtige Massstab - er ist der Pfad, den die Spielfigur
     * nimmt. */
    {   int32_t px = -19500, pz = ga->z;
        for (int step = 0; step < 400; step++) {
            int32_t dxs = (ga->x > px) ? 25 : -25;
            int32_t nx = px + dxs, nz = pz;
            re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
            if (nx == px && nz == pz) break;
            px = nx; pz = nz;
        }
        sx0 = px; sz0 = pz; have_edge = (px != -19500);
    }
    printf("  Standpunkt am Gitter (Kollisions-Grenze): (%ld,%ld), Arm %d auf (%ld,%ld)\n",
           (long)sx0, (long)sz0, gate_arm, (long)ga->x, (long)ga->z);
    CHECK(have_edge,
          "die Kollisions-Grenze auf der Hoehe des Arms ist bestimmbar - sonst misst (4) nichts");

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
            /* Leon darf beim Griff NICHT wegspringen (Nutzer-Report 2026-08-29: "wenn Leon
             * von den Armen getroffen wird, verschwindet er"). Die Opfer-Platzierung ist
             * ABSOLUT (re15_clip_root_motion_abs = Anker +0xa0/+0xa2 plus gedrehter
             * Clip-Offset, byte-true func_0x8001ad68); ohne den gemeinsamen Anker, den der
             * Zombie-Griff im Eintritts-Bild setzt und auf den Spieler kopiert
             * (FUN_8001ac38 @0x801025f0, Kopie @0x8001ad28-48), landete er an einem alten
             * Anker plus Clip-Offset. GEMESSEN vorher: Sprung von (-18164,-5897) auf
             * (-892,0) = 16508 Einheiten. */
            {   int32_t vdx = pl->x - g_actors[gate_arm].x;
                int32_t vdz = pl->z - g_actors[gate_arm].z;
                int32_t vd = (vdx < 0 ? -vdx : vdx) + (vdz < 0 ? -vdz : vdz);
                if (vd > far_from_arm) far_from_arm = vd;
            }
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
    /* `>= grabs - 1`, weil der letzte Griff beim Ende der 400 Bilder noch laufen darf -
     * das ist kein Festfressen (das prueft die naechste Zusage ueber die Griff-Dauer). */
    CHECK(freed >= grabs - 1,
          "und jeder Griff endet wieder (%d Zugriffe, %d mal frei) — RE2s Ringkampf-Budget 148 "
          "@0x80102828-2C, pro Bild -= 2 + 5*Taste @0x80102868-7C", grabs, freed);
    CHECK(far_from_arm > 0 && far_from_arm < 4000,
          "und Leon bleibt beim Griff AM ARM (groesster Abstand %ld) - vor dem Anker-Fix "
          "sprang er 16508 Einheiten weit weg (re15_clip_root_motion_abs platziert ABSOLUT "
          "vom Anker +0xa0/+0xa2, den der Zombie-Griff @0x801025f0 setzt und @0x8001ad28-48 "
          "auf den Spieler kopiert)", (long)far_from_arm);
    CHECK(longest_hold > 0 && longest_hold < 400,
          "kein Festfressen: der laengste Griff dauert %d Bilder (Budget 148 / 2 pro Bild = 74 "
          "Halte-Bilder plus Loese-Clip)", longest_hold);

    /* ⛔ GEGENPROBE UMGEDREHT (2026-08-28). Hier stand "kein Arm hat seine Position
     * veraendert". Genau das war der Defekt: an ihren Spawn-Positionen liegen alle zehn Arme
     * in NULL von NEUN Kamera-Vierecken des Raums und werden deshalb ueberhaupt nicht
     * gezeichnet (Zeichner-Weiche FUN_8001e8c8 `jal 0x80014368` @0x8001e974; im Port
     * `re15_aot_point_in_quad` vor der Mesh-Ausgabe). Die byte-true Lunge ist der
     * Mechanismus, der sie sichtbar macht: netto 2420 Einheiten (3*800 + 20 vorwaerts,
     * 30*20 + 200 rueckwaerts, 4*200 vorwaerts — FUN_8010c714, Schritt FUN_800245d8).
     * Die Wache verlangt jetzt den Ausschlag UND die Rueckkehr. */
    printf("  groesster Ausschlag eines Arms im Durchlauf: %d Einheiten\n", moved);
    CHECK(moved >= 2400,
          "die Arme fahren wirklich aus dem Gitter (groesster Ausschlag %d Einheiten; die "
          "Original-Lunge misst netto 2420, +0x8c = 0x320 @0x8010c7b8 ueber +0x9c = 3 "
          "@0x8010c7a8) — bei 0 waeren sie unsichtbar geblieben", moved);
    {   int home = 0;
        for (int i = 0; i < n; i++) {
            re15_actor_t *e = &g_actors[slots[i]];
            if (e->sub_state_1 == 0 && e->x == sx[i] && e->z == sz2[i]) home++;
        }
        CHECK(home >= 6,
              "und wer wieder in Ruhe ist, steht auch wieder im Gitter (%d von %d) — das "
              "Original kennt kein Zurueck (Ausgang @0x8010c8e4 -> +0x5 = 2/3), der Port loest "
              "die Lunge aber pro Arm wiederholt aus und muss ihn zuruecksetzen", home, n);
    }

    free(buf);
    if (fails) { printf("\n1210 GITTERHAENDE: FAIL (%d)\n", fails); return 1; }
    printf("\n1210 GITTERHAENDE: sie kommen einzeln, wenn man vorbeigeht\n");
    return 0;
}
