/* test_r41_birkin_kralle.c — Riegel: Birkins Kralle ACT[3] (Typ 0x30, STAGE3 FUN_801174fc, STAGE5 bytegleich +0x814).
 *
 * Nutzerbefund 2026-11-15: im regulaeren Birkin-Kampf (3070/3071) und mit der Schwanz-Gestalt (Labor 50E0/50F1) macht Birkin nach
 * dem Hieb wiederholt eine Rueckzieh-Animation, die er im Original nicht macht. Gemessen (master 0b86e36c6): JEDER fertig
 * gespielte Hieb lief in Clip 0xa, weil der Port das Clip-Ende als "Sonde" nahm (`if (done) e->sub_state_2 = 3`).
 *
 * Original (Version_40% STAGE3.BIN, selbst disassembliert):
 *   Phase 1 @0x8011762c: anim_set @0x80117640, +0x6 += Rueckgabe UNBEDINGT vor dem Tor (@0x8011765c-66c) -> am Clip-Ende
 *     Phase 2 @0x80117888 (HUB, +0x1df = 0x1e @0x801178bc/@0x801179a8). Im Fenster (+0x95 - 0x24) < 8 (@0x801176ec-f0) jedes Bild
 *     Angriffsbox @0x80117710 und danach die Sonde FUN_8001c2dc(Klauenpunkt, 0x190) @0x80117818-834; Flag (@0x8011783c) -> +0x6 = 3
 *     (@0x80117848-858) = Erholung: Clip 0xa @0x801178dc, Se(6) @0x801178fc. Phase 4 @0x80117904: Form 2 (grid & 0x10) endet
 *     bei +0x95 == 0x1e (@0x80117948-970), sonst am Clip-Ende; Phase 5 @0x80117974: +0x1df = 0x3c (@0x801179a4-a8).
 *   Drehung FUN_8001aac4(Spieler, 0x50) nur in Phase 0 @0x80117624, beim Abbruch @0x801176c8 und in Phase 2/5 (@0x801178a8 /
 *     @0x80117994) - Phase 1 beginnt bei 0x8011762c, NACH dem Aufruf aus Phase 0.
 *
 * Faelle (echte EM30-Bank aus CDEMD0.EMS, echtes ROOM3070.RDT in g_room_rdt, nur die Gegner-KI tickt):
 *   frei   Birkin und Spieler mitten im Raum (keine Zelle im Umkreis 1500): kein Clip 0xa, Rueckkehr in den HUB mit +0x1df 0x1e,
 *          und der Clip 3 lief bis zum letzten Bild.
 *   drehen wie frei, der Spieler geht in Phase 1 seitlich weg: die Gier bleibt von Phase-1-Beginn bis zu ihrem Ende stehen.
 *   wand   Birkin schlaegt gegen Zelle #8 (x >= -20100): Sonde im Fenster -> Clip 0xa; Form 2 endet bei +0x95 == 0x1e, +0x1df 0x3c.
 *   wand1  wie wand mit Form 1 (grid 0x00): Clip 0xa bis zum letzten Bild, +0x1df 0x3c.
 * Gegenprobe: die master-exe (0b86e36c6) faellt in frei (Clip 0xa), drehen (Gier dreht), wand (Clip 0xa bis Bild 46).
 *
 * Runde 2 (Pruefbericht r1: im Labor 50E0/50F1 zog die Schwanz-Gestalt weiter bei 8 von 11 Hieben zurueck): der Port zeichnet dort
 * das Ersatzmodell EM30_HANGAR.EMD (laengerer Arm, geduckte Keyframes); der Klauenpunkt lag bis 590 weiter vom Birkin weg. Die
 * Kralle rechnet jetzt mit dem Capcom-EM30 als Logik-Gerippe (enemy_ai_common.c re15_bk_kralle_logik_setzen, PORT-WAHL dort).
 *   hangar       gezeichnete Bank = EM30_HANGAR.EMD, Logik-Gerippe = EM30, Birkin so weit vor Zelle #8, dass nur der Hangar-Arm
 *                sie erreicht: KEIN Clip 0xa, +0x1df 0x1e. Gegenprobe (Stand r1 45dbd9e10, ohne Logik-Gerippe): Clip 0xa.
 *   hangar_ohne  dieselbe Lage ohne Logik-Gerippe: Clip 0xa (die Lage trennt die beiden Arme wirklich).
 *   hangar_wand  Logik-Gerippe, Birkin an Zelle #8 wie `wand`: die Sonde wirkt weiter (Clip 0xa bis 0x1e, +0x1df 0x3c).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_damage.h"
#include "re15_collision.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int16_t re15_atan2_q12(int32_t, int32_t);

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

static uint8_t s_blob[0x80000];
static size_t s_blob_len = 0;
static uint8_t *s_hangar = NULL;
void re15_bk_kralle_logik_setzen(const uint8_t *em30, size_t len);   /* enemy_ai_common.c */
/* gezeichnete 0x30-Bank = EM30_HANGAR.EMD (Ersatzmodell Labor/Hangar); s_blob behaelt das EM30 fuer das Logik-Gerippe */
static int hangar_bank(void)
{
    size_t n = 0;
    s_hangar = slurp(RE15_ASSET_PSX_DIR "/EMD/EM30_HANGAR.EMD", &n);
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!s_hangar || !eb || re15_emd_parse_container(s_hangar, n, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return eb->skel.bone_count > 16;
}
static int em30_bank(void)
{
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems || re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) != 0 || len > sizeof s_blob) { free(ems); return 0; }
    memcpy(s_blob, ems + off, len); free(ems); s_blob_len = len;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!eb || re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return eb->anim.clip_count > 0x0a;
}

static int raum_3070(void)
{
    size_t n = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE3/ROOM3070.RDT", &n);
    if (!raw || re15_rdt_parse(raw, n, &g_room_rdt) != 0) return 0;   /* raw bleibt fuer die Zeiger des RDT am Leben */
    g_room_rdt_ok = 1;
    g_current_room_id = 0x3070u;
    re15_collision_reset_band();
    re15_collision_set_band(0);
    return 1;
}

static int16_t blick(int32_t ex, int32_t ez, int32_t tx, int32_t tz)
{
    return (int16_t)((((int)re15_atan2_q12(tz - ez, tx - ex) - 0x400)) & 0xfff);
}

typedef struct {
    int ticks, clip_a, clip_a_max, clip3_max, aus_sub, cd, gier_dreht, sonde_af;
} ergebnis_t;

/* Ein Hieb ab Phase 0. seitlich != 0: der Spieler geht ab Phase 1 je Bild um `seitlich` in Z. */
static ergebnis_t hieb(uint8_t grid, int32_t bx, int32_t bz, int32_t px, int32_t pz, int32_t seitlich)
{
    ergebnis_t r; memset(&r, 0, sizeof r); r.sonde_af = -1;
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = px; pl->y = 0; pl->z = pz; pl->hp = 100; pl->hit_react = 1;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x30; e->state = 0; e->grid_id = grid; e->x = bx; e->y = 0; e->z = bz;
    re15_enemy_apply_hitbox(e, 0x30);
    re15_enemy_spawn_root(1);                                   /* INIT -> BRAIN, Unterzustand 0 (Nibble 0) */
    e->rot_y = blick(bx, bz, px, pz);
    e->state = 1; e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0; e->birkin_atk_cd = 0;
    int16_t gier1 = 0; int gier1_ok = 0;
    for (int f = 0; f < 400; f++) {
        pl->hit_react = 1;                                     /* Spieler+0x93 gesetzt: kein Treffer-Block (@0x8011772c), nur Box + Sonde */
        const uint8_t ph_vor = e->sub_state_2;
        re15_enemy_ai_run_all(0);
        r.ticks++;
        if (e->sub_state_1 != 3) { r.aus_sub = e->sub_state_1; r.cd = e->birkin_atk_cd; break; }
        if (e->motion == 0x0a) { r.clip_a = 1; if (e->anim_frame > r.clip_a_max) r.clip_a_max = e->anim_frame; }
        if (e->motion == 3 && e->anim_frame > r.clip3_max) r.clip3_max = e->anim_frame;
        if (ph_vor == 1 && e->sub_state_2 == 3 && r.sonde_af < 0) r.sonde_af = e->anim_frame;
        if (e->sub_state_2 == 1) {
            if (!gier1_ok) { gier1 = e->rot_y; gier1_ok = 1; }
            else if (e->rot_y != gier1) r.gier_dreht = 1;
            if (seitlich) pl->z += seitlich;
        }
    }
    return r;
}

int main(int argc, char **argv)
{
    const char *fall = argc > 1 ? argv[1] : "alle";
    const int alle = !strcmp(fall, "alle");
    if (!em30_bank()) { fprintf(stderr, "FAIL: EM30-Bank (CDEMD0.EMS) nicht ladbar\n"); return 1; }
    if (!raum_3070()) { fprintf(stderr, "FAIL: ROOM3070.RDT nicht ladbar\n"); return 1; }
    const re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    const int fc3 = eb->anim.clips[3].frame_count, fca = eb->anim.clips[0x0a].frame_count;
    printf("EM30: Clip 3 = %d Bilder, Clip 0xa = %d Bilder\n", fc3, fca);

    if (alle || !strcmp(fall, "frei")) {
        ergebnis_t r = hieb(0x10, -24000, -16500, -22500, -15000, 0);
        printf("frei : ticks=%d clip0xa=%d clip3_max=%d aus_sub=%d cd=0x%02x\n", r.ticks, r.clip_a, r.clip3_max, r.aus_sub, r.cd);
        CHECK(r.clip3_max == fc3 - 1, "frei: Clip 3 muss bis zum letzten Bild laufen (%d), erreichte %d (Abbruch statt Clip-Ende?)", fc3 - 1, r.clip3_max);
        CHECK(!r.clip_a, "frei: ohne Klaue in einer Zelle KEIN Clip 0xa (Erholung nur ueber die Sonde @0x80117834-858)");
        CHECK(r.aus_sub == 1 && r.cd == 0x1e, "frei: Clip-Ende -> Phase 2 -> HUB mit +0x1df 0x1e (@0x8011765c-66c, @0x801178bc), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
    }
    if (alle || !strcmp(fall, "drehen")) {
        ergebnis_t r = hieb(0x10, -24000, -16500, -22500, -15000, 6);
        printf("drehen: ticks=%d gier_dreht=%d aus_sub=%d\n", r.ticks, r.gier_dreht, r.aus_sub);
        CHECK(!r.gier_dreht, "drehen: in Phase 1 keine Nachdrehung (FUN_8001aac4 nur Phase 0 @0x80117624, Abbruch @0x801176c8, Phase 2/5)");
    }
    if (alle || !strcmp(fall, "wand")) {
        ergebnis_t r = hieb(0x10, -21300, -15000, -20400, -15000, 0);
        printf("wand : ticks=%d clip0xa=%d bis Bild %d sonde_af=%d aus_sub=%d cd=0x%02x\n", r.ticks, r.clip_a, r.clip_a_max, r.sonde_af, r.aus_sub, r.cd);
        CHECK(r.clip_a, "wand: Klaue in Zelle #8 -> Sonde -> Clip 0xa (@0x80117848-858, @0x801178dc)");
        CHECK(r.sonde_af >= 0x24 && r.sonde_af <= 0x2b, "wand: Sonde nur im Fenster +0x95 0x24..0x2b (@0x801176ec-f0), war %d", r.sonde_af);
        CHECK(r.clip_a_max == 0x1e, "wand: Form 2 endet die Erholung bei +0x95 == 0x1e (@0x8011795c-970), lief bis %d", r.clip_a_max);
        CHECK(r.aus_sub == 1 && r.cd == 0x3c, "wand: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
    }
    if (alle || !strcmp(fall, "wand1")) {
        ergebnis_t r = hieb(0x00, -21300, -15000, -20400, -15000, 0);
        printf("wand1: ticks=%d clip0xa=%d bis Bild %d aus_sub=%d cd=0x%02x\n", r.ticks, r.clip_a, r.clip_a_max, r.aus_sub, r.cd);
        CHECK(r.clip_a && r.clip_a_max == fca - 1, "wand1: Form 1 spielt Clip 0xa bis zum letzten Bild %d (+0x6 += Rueckgabe @0x8011792c-938), lief bis %d", fca - 1, r.clip_a_max);
        CHECK(r.aus_sub == 1 && r.cd == 0x3c, "wand1: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
    }
    if (!strcmp(fall, "hangar") || !strcmp(fall, "hangar_ohne") || !strcmp(fall, "hangar_wand")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        const int mit = strcmp(fall, "hangar_ohne") != 0, an_wand = !strcmp(fall, "hangar_wand");
        re15_bk_kralle_logik_setzen(mit ? s_blob : NULL, mit ? s_blob_len : 0);
        /* Blick +x auf Zelle #8 (x >= -20100). Groesster x-Vorlauf der Klaue im Fenster: EM30 2092, Hangar 2478 (af 40,
         * r2/tools/klauenpunkt.py); mit Sondenradius 400 erreicht von -22800 aus nur der Hangar-Arm die Zelle. */
        const int32_t bx = an_wand ? -21300 : -22800;
        ergebnis_t r = hieb(0x10, bx, -15000, -20400, -15000, 0);
        printf("%s: ticks=%d clip0xa=%d bis Bild %d sonde_af=%d clip3_max=%d aus_sub=%d cd=0x%02x\n", fall, r.ticks, r.clip_a,
               r.clip_a_max, r.sonde_af, r.clip3_max, r.aus_sub, r.cd);
        if (!strcmp(fall, "hangar")) {
            CHECK(!r.clip_a, "hangar: mit dem EM30-Logik-Gerippe erreicht die Klaue Zelle #8 nicht -> kein Clip 0xa (gezeichnet: Hangar-Arm)");
            CHECK(r.aus_sub == 1 && r.cd == 0x1e, "hangar: Clip-Ende -> HUB mit +0x1df 0x1e (@0x801178bc), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        } else if (!mit) {
            CHECK(r.clip_a, "hangar_ohne: ohne Logik-Gerippe reicht der Hangar-Arm in Zelle #8 -> Clip 0xa (die Lage trennt die Arme)");
        } else {
            CHECK(r.clip_a && r.clip_a_max == 0x1e, "hangar_wand: die Sonde wirkt mit dem Logik-Gerippe weiter, Clip 0xa bis 0x1e (@0x8011795c-970), war %d/%d", r.clip_a, r.clip_a_max);
            CHECK(r.aus_sub == 1 && r.cd == 0x3c, "hangar_wand: +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (fails) { fprintf(stderr, "test_r41_birkin_kralle %s: %d Fehler\n", fall, fails); return 1; }
    printf("test_r41_birkin_kralle %s: OK\n", fall);
    return 0;
}
