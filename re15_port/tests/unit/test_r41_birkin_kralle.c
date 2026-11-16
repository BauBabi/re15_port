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
 *   hangar_wand  Logik-Gerippe, Birkin an Zelle #8 wie `wand`: Clip 0xa bis 0x1e, +0x1df 0x3c. Seit Runde 1 "synchron" loest das
 *                die GEZEICHNETE Klaue aus (ROOM3070 steht nicht in der Tabelle sichtbarer Waende -> alle Zellen Wand).
 *
 * Runde 1 "synchron" (NUTZERENTSCHEID 16.11.2026: Rueckzug nur, wenn die Klaue sichtbar gegen eine Wand schlaegt, synchron dazu;
 * nur Schwanz-Gestalt = Ersatzmodell gezeichnet, re15_birkin_klaue_r41.h). Echte Raeume ROOM50A0 / ROOM50F1:
 *   schwanz_wand       Hangar 50A0, Birkin vor der Ostwand #4 (x >= -2700): in keinem Takt der Phase 1 steckt die gezeichnete
 *                      Klinge in der Wand, ohne dass im SELBEN Takt +0x6 = 3 gesetzt wird; der naechste Takt laeuft Clip 0xa mit
 *                      +0x8f = RE15_BKK_UEBERBLENDUNG - 1. Gegenprobe Merge-Commit e7858c897: die Klinge steckt Takte lang in der
 *                      Wand, bevor die Capcom-Sonde (erst ab +0x95 0x24) anschlaegt.
 *   schwanz_gelaender  Labor 50F1, Birkin vor dem Gelaender #12 (Oberkante 1900 gemessen): die gezeichnete Klinge kommt nicht
 *                      unter die Oberkante -> kein Clip 0xa. Gegenprobe Merge-Commit: die Capcom-Sonde schlaegt bei af 40 an.
 *
 * Runde 2 "synchron" (Pruefbericht r1: 5 Kontakte ausserhalb des Kamerabilds; Gelaender 50E0 #16 galt als Wand bis zur Decke).
 * Die Kralle prueft jetzt mit der Kamera des laufenden Cuts (g_scd.cam_id); jeder Fall setzt den Cut, den das Spiel dort zeigt.
 *   schwanz_gelaender16  50E0, Birkin bei (-13500,-10500), Blick nach Westen auf #16 (Pruefer-Probe r1, Cut 4). Stand r1: Kontakt
 *                        bei af 36 in 2773 Hoehe ueber der Oberkante 1900 -> Clip 0xa. Neu: kein Kontakt ueber der Oberkante;
 *                        der erste Kontakt liegt unter 1900 (die Klinge senkt sich bei af 38 ins Gelaender, 1871 hoch).
 *   schwanz_ausser_bild  50E0 vor der Suedwand #3, Birkin (-10496,-25129), Spieler (-7544,-26375) (Lage des Hiebs F1715, Pruefer
 *                        r1). Laufender Cut 5: die Klinge trifft #3 im Bild -> Rueckzug im Kontaktbild. Dieselbe Lage mit Cut 0
 *                        (Birkin und Wand ausserhalb des Bilds): die Klinge steckt genauso in #3, aber nicht sichtbar -> kein
 *                        Rueckzug. Gegenprobe Stand r1: Rueckzug auch mit Cut 0.
 *   schwanz_abdeckung    jede SCA-Zelle (Band 0, solide) von ROOM50E0/50F1/50A0/50A1 steht in der Tabelle sichtbarer Waende
 *                        (re15_bkk_zelle_art != 0) - keine Zelle gilt mehr ungeprueft als Wand.
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
#include "re15_birkin_klaue_r41.h"
#include "re15_scd.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int16_t re15_atan2_q12(int32_t, int32_t);

static int fails = 0;
#ifndef BK_WAND_X
#define BK_WAND_X (-4000)
#endif
#ifndef BK_GEL_X
#define BK_GEL_X (-12200)
#endif
#ifndef BK_GEL_DX
#define BK_GEL_DX 1200
#endif
#ifndef BK_GEL_DZ
#define BK_GEL_DZ 0
#endif
#ifndef BK_GEL_Z
#define BK_GEL_Z (-16000)
#endif
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
int re15_bk_kralle_logik_aktiv(void);
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

static uint8_t *s_raum_raw = NULL;
static int raum_laden(const char *datei, uint16_t id)
{
    size_t n = 0;
    free(s_raum_raw);
    s_raum_raw = slurp(datei, &n);
    if (!s_raum_raw || re15_rdt_parse(s_raum_raw, n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    g_current_room_id = id;
    re15_collision_reset_band();
    re15_collision_set_band(0);
    return 1;
}

typedef struct {
    int ticks, clip_a, clip_a_max, clip3_max, aus_sub, cd, gier_dreht, sonde_af;
    int kontakt_af, durch, frac_a;   /* Runde 1 synchron: erstes Kontaktbild, Takte Klinge-in-Wand ohne +0x6 = 3, +0x8f im 1. 0xa-Takt */
    int kontakt_zelle; int32_t kontakt_y;  /* Runde 2: Zelle und Hoehe (Welt-y) des tiefsten sichtbaren Pruefpunkts im Kontaktbild */
} ergebnis_t;

/* Ein Hieb ab Phase 0. seitlich != 0: der Spieler geht ab Phase 1 je Bild um `seitlich` in Z. */
static int s_cut = 0;   /* Kamera des laufenden Cuts (g_scd.cam_id) fuer den Hieb */
static ergebnis_t hieb(uint8_t grid, int32_t bx, int32_t bz, int32_t px, int32_t pz, int32_t seitlich)
{
    g_scd.cam_id = (uint8_t)s_cut;
    ergebnis_t r; memset(&r, 0, sizeof r); r.sonde_af = -1; r.kontakt_af = -1; r.frac_a = -1; r.kontakt_zelle = -1;
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
        if (e->motion == 0x0a) { if (!r.clip_a) r.frac_a = e->anim_frac; r.clip_a = 1; if (e->anim_frame > r.clip_a_max) r.clip_a_max = e->anim_frame; }
        if (e->motion == 3 && (e->sub_state_2 == 1 || e->sub_state_2 == 3) && re15_bk_kralle_logik_aktiv() && re15_bkk_kontakt(e)) {
            if (r.kontakt_af < 0) {
                r.kontakt_af = e->anim_frame;
                re15_bkk_lage_t l; re15_bkk_befund_t b;
#ifndef BKK_ALT_API   /* nur fuer die Gegenprobe gegen den Merge-Commit (alte Schnittstelle ohne Kamera) */
                re15_camera_view_t v;
                if (re15_bkk_lage_logik(e, &l) && re15_bkk_sicht(&v) &&
                    re15_bkk_pruefen(re15_bkk_klinge(), &l, &g_room_rdt, (uint16_t)g_current_room_id, &v, &b)) {
#else
                if (re15_bkk_lage_logik(e, &l) && re15_bkk_pruefen(re15_bkk_klinge(), &l, &g_room_rdt, (uint16_t)g_current_room_id, &b)) {
#endif
                    r.kontakt_zelle = b.zelle; r.kontakt_y = b.punkt[1];
                }
            }
            if (e->sub_state_2 == 1) r.durch++;          /* Klinge steckt (Pose dieses Takts), Rueckzug nicht ausgeloest */
        }
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
        s_cut = 5;                                              /* 3070: Kampfschnitt (Messlaeufe RE15_FORCE_CUT=5) */
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
    if (!strcmp(fall, "schwanz_wand") || !strcmp(fall, "schwanz_gelaender")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        const int wand = !strcmp(fall, "schwanz_wand");
        if (wand ? !raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A0.RDT", 0x50A0u)
                 : !raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50F1.RDT", 0x50F1u)) {
            fprintf(stderr, "FAIL: Raum nicht ladbar\n"); return 1;
        }
        /* wand: Ostwand #4 (x -2700..-700, z -28700..-8200), Birkin 1300 davor (x -4000), Blick +x, Spieler 1200 vor ihm.
         *   Gegenprobe Merge-Commit (Lagesuche suche/): Klinge 3 Takte in der Wand (af 35..37), Sonde erst bei af 38, +0x8f 2.
         * gelaender: Laufsteg zwischen #2 (x <= -14245) und #12 (x >= -9725), Birkin bei (-12200, -16000), Blick +x auf #12.
         *   Neu: die gezeichnete Klinge kommt in keinem Bild unter die Oberkante 1900 von #12 -> kein Clip 0xa. Gegenprobe
         *   Merge-Commit: die Capcom-Sonde (EM30-Klauenpunkt, Radius 0x190 @0x80117818) schlaegt bei af 40 an -> Clip 0xa. */
        s_cut = wand ? 4 : 5;                                   /* 50A0 Gang-Cut 4, 50F1 Kampf-Cut 5 (state.log cam= der Messlaeufe) */
        ergebnis_t r = wand ? hieb(0x10, BK_WAND_X, -15000, BK_WAND_X + 1200, -15000, 0) : hieb(0x10, BK_GEL_X, BK_GEL_Z, BK_GEL_X + BK_GEL_DX, BK_GEL_Z + BK_GEL_DZ, 0);
        printf("%s: ticks=%d clip0xa=%d bis Bild %d kontakt_af=%d durch=%d frac_a=%d sonde_af=%d aus_sub=%d cd=0x%02x\n", fall,
               r.ticks, r.clip_a, r.clip_a_max, r.kontakt_af, r.durch, r.frac_a, r.sonde_af, r.aus_sub, r.cd);
        if (wand) {
            CHECK(r.kontakt_af >= 0, "schwanz_wand: die gezeichnete Klinge muss die Ostwand #4 erreichen (Lage pruefen)");
            CHECK(r.durch == 0, "schwanz_wand: %d Takt(e) steckt die gezeichnete Klinge in der Wand, ohne dass +0x6 = 3 gesetzt wird", r.durch);
            CHECK(r.clip_a && r.sonde_af == r.kontakt_af, "schwanz_wand: Rueckzug im Kontaktbild (af %d), ausgeloest bei af %d", r.kontakt_af, r.sonde_af);
            CHECK(r.frac_a == RE15_BKK_UEBERBLENDUNG - 1, "schwanz_wand: erster 0xa-Takt +0x8f = %d erwartet, war %d", RE15_BKK_UEBERBLENDUNG - 1, r.frac_a);
            CHECK(r.aus_sub == 1 && r.cd == 0x3c, "schwanz_wand: Phase 5 -> HUB mit +0x1df 0x3c (@0x801179a4-a8), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        } else {
            CHECK(r.kontakt_af < 0 && !r.clip_a, "schwanz_gelaender: Klaue ueber dem Gelaender -> kein Kontakt, kein Clip 0xa (kontakt_af %d)", r.kontakt_af);
            CHECK(r.aus_sub == 1 && r.cd == 0x1e, "schwanz_gelaender: Clip-Ende -> HUB mit +0x1df 0x1e (@0x801178bc), aus_sub=%d cd=0x%02x", r.aus_sub, r.cd);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_gelaender16") || !strcmp(fall, "schwanz_ausser_bild")) {
        if (!hangar_bank()) { fprintf(stderr, "FAIL: EM30_HANGAR.EMD nicht ladbar\n"); return 1; }
        re15_bk_kralle_logik_setzen(s_blob, s_blob_len);
        if (!raum_laden(RE15_ASSET_PSX_DIR "/STAGE5/ROOM50E0.RDT", 0x50E0u)) { fprintf(stderr, "FAIL: ROOM50E0 nicht ladbar\n"); return 1; }
        if (!strcmp(fall, "schwanz_gelaender16")) {
            /* Pruefer-Probe r1 (probe16/t16.c): Birkin (-13500,-10500), Spieler 1200 westlich auf dem Steg, Cut 4 (Messbild e0_16_c4) */
            s_cut = 4;
            ergebnis_t r = hieb(0x10, -13500, -10500, -14700, -10500, 0);
            printf("%s: ticks=%d clip0xa=%d kontakt_af=%d zelle=%d y=%d durch=%d sonde_af=%d aus_sub=%d cd=0x%02x\n", fall, r.ticks,
                   r.clip_a, r.kontakt_af, r.kontakt_zelle, (int)r.kontakt_y, r.durch, r.sonde_af, r.aus_sub, r.cd);
            /* Gelaender-Oberkante -1900 (BKK_GELAENDER_OBEN, birkin_klaue_r41.c): ein Kontakt an #16 nur UNTER der Oberkante */
            CHECK(r.kontakt_af != 36, "schwanz_gelaender16: bei af 36 geht die Klinge in 2773 Hoehe ueber das Gelaender -> kein Kontakt");
            CHECK(r.kontakt_af < 0 || r.kontakt_y >= -1900, "schwanz_gelaender16: Kontakt ueber der Oberkante 1900 (y %d, Zelle %d)", (int)r.kontakt_y, r.kontakt_zelle);
            CHECK(r.durch == 0 && (!r.clip_a || r.sonde_af == r.kontakt_af), "schwanz_gelaender16: Rueckzug nur im Kontaktbild (kontakt_af %d, sonde_af %d)", r.kontakt_af, r.sonde_af);
        } else {
            /* Hieb F1715 (Pruefer r1, Lauf N_50E0schiess): Birkin (-10496,-25129), Spieler (-7544,-26375); Kontakt F1723 af 29 an #3 */
            s_cut = 5;
            ergebnis_t r5 = hieb(0x10, -10496, -25129, -7544, -26375, 0);
            s_cut = 0;
            ergebnis_t r0 = hieb(0x10, -10496, -25129, -7544, -26375, 0);
            printf("%s: Cut 5 clip0xa=%d kontakt_af=%d zelle=%d sonde_af=%d durch=%d | Cut 0 clip0xa=%d kontakt_af=%d sonde_af=%d\n", fall,
                   r5.clip_a, r5.kontakt_af, r5.kontakt_zelle, r5.sonde_af, r5.durch, r0.clip_a, r0.kontakt_af, r0.sonde_af);
            CHECK(r5.clip_a && r5.kontakt_zelle == 3 && r5.sonde_af == r5.kontakt_af && r5.durch == 0,
                  "schwanz_ausser_bild: Cut 5 - Klinge trifft #3 im Bild -> Rueckzug im Kontaktbild (kontakt_af %d Zelle %d, sonde_af %d)",
                  r5.kontakt_af, r5.kontakt_zelle, r5.sonde_af);
            CHECK(!r0.clip_a && r0.kontakt_af < 0, "schwanz_ausser_bild: Cut 0 - dieselbe Klinge in #3, aber ausserhalb des Bilds -> kein Rueckzug (kontakt_af %d)", r0.kontakt_af);
        }
        re15_bk_kralle_logik_setzen(NULL, 0);
    }
    if (!strcmp(fall, "schwanz_abdeckung")) {
        static const struct { const char *datei; uint16_t id; } R[] = {
            { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50E0.RDT", 0x50E0u }, { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50F1.RDT", 0x50F1u },
            { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A0.RDT", 0x50A0u }, { RE15_ASSET_PSX_DIR "/STAGE5/ROOM50A1.RDT", 0x50A1u } };
        for (size_t k = 0; k < sizeof R / sizeof R[0]; k++) {
            if (!raum_laden(R[k].datei, R[k].id)) { fprintf(stderr, "FAIL: %s nicht ladbar\n", R[k].datei); return 1; }
            int n = 0, wand = 0, gel = 0;
            for (int i = 0; i < g_room_rdt.sca_count; i++) {
                const re15_sca_entry_t *c = &g_room_rdt.sca[i];
                const int typ = c->type & 0x0f;
                if ((typ != 1 && typ != 3) || !(c->u0 & 1) || (c->floor >> 4) != 0 || !c->width || !c->density) continue;
#ifndef BKK_ALT_API
                const int art = re15_bkk_zelle_art(R[k].id, c);
#else
                const int art = 0;   /* Gegenprobe: der Merge-Commit hat keine Tabelle je Zelle */
#endif
                n++; wand += art == 1; gel += art == 2;
                CHECK(art != 0, "schwanz_abdeckung: ROOM%04X Zelle #%d (x %d z %d w %u d %u) fehlt in der Tabelle sichtbarer Waende",
                      (unsigned)R[k].id, i, (int)c->x, (int)c->z, (unsigned)c->width, (unsigned)c->density);
            }
            printf("ROOM%04X: %d Zellen, Wand %d, Gelaender %d\n", (unsigned)R[k].id, n, wand, gel);
            CHECK(n > 0, "schwanz_abdeckung: ROOM%04X ohne Zellen", (unsigned)R[k].id);
        }
    }
    if (fails) { fprintf(stderr, "test_r41_birkin_kralle %s: %d Fehler\n", fall, fails); return 1; }
    printf("test_r41_birkin_kralle %s: OK\n", fall);
    return 0;
}
