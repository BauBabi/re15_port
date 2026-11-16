/* test_r41_zielhilfe.c — Runde 41 "zielhilfe": Waffe heben, Gegner hinter Leon.
 *
 * Nutzer 2026-11-16: "wenn ich die Waffe hebe, und der Auto Anvisieren Mechanismus kommt, klappt er nur wenn
 * der Gegner in einen nicht zu hohen Winkel ist. Ist der Gegner zum Beispiel 180 Grad hinter mir, dreht sich
 * Leon nicht komplett bis zum Gegner sondern nur teilweise."
 *
 * Mechanismus (selbst disassembliert, Bericht scratchpad/zielhilfe/BERICHT_r1.md):
 *   RE1.5 Gun-FSM Sub 0 (HEBEN) @0x80032f18: Phase 0 @0x80032f64 setzt Clip 6 (@0x80032f70), rastet das Ziel ein
 *     (`jal 0x8003703c` / `ori a0,zero,0x7530` @0x80032fa8-ac, keine Winkelgrenze) und FAELLT DURCH in den
 *     Phase-1-Rumpf @0x80032fcc: `ori a1,zero,0xc8` @0x80032fe0 / `jal 0x8001a8f8` @0x80032fec (Drehschritt 200
 *     je Bild), dann anim_set @0x800330a8; `aca5b += anim_set()` @0x800330c0-cc. anim_set zaehlt +0x95 hoch und
 *     meldet 1 bei Bild >= Clip-Laenge (@0x8001f618-63c). Phase 2 -> HALTEN @0x80033168 (kein 0x8001a8f8 mehr).
 *     => gedreht wird in genau <Clip-6-Laenge> Bildern (Pistole PL00W03 Clip 6 = 10 Bilder) x 200 = 2000 < 2048.
 *   RE2 Retail Heben @0x8004326c (alle Schusswaffen-Tabellen 0x800a702c/48/68/80/98/b0): Phase 0 @0x800432d4 faellt
 *     ebenso durch; Drehschritt `li a3,0xd8` @0x800434a0 (0x120 fuer Waffe 0xd @0x800434ac), `jal 0x80015558`
 *     @0x800434b0 (gleicher Algorithmus wie 0x8001a8f8); Heben-Clip 9 = 10 Bilder (PL00W02.PLW EDD-Eintrag 9 @0x2c
 *     = 0x000a) => 10 x 216 = 2160 >= 2048: RE2 dreht bis 180 Grad vollstaendig.
 *
 * Modi:
 *   mess      Messung im Port: Gegner bei 0/45/90/135/180 (und -90/-135) Grad, Pistole/Schrotflinte/Messer/MG,
 *             je Bild Phase, Clip-Bild, Blick, Restwinkel; Zusammenfassung je Fall.
 *   hinten    Riegel: 180 Grad -> Restwinkel 0 nach dem Heben (Pistole, Messer-Wiederheben, MG, Schrotflinte).
 *   vorne     Riegel: 0 Grad -> Blick bleibt bitgleich auf dem Ziel (Folge = master-Folge: Blick jedes Bild = Peilung).
 *   wand      Riegel: Zombie hinter der Wand (ROOM1100) -> kein Ziel, Blick bleibt (wie bisher, Runde 39 Spur D).
 */
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_enemy_ai.h"
#include "re15_skeleton.h"
#include "re15_treffer_r39.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern int  re15_player_aim_dbg(int *phase, int *recoil, int *autof, int *auto_cls, int *takt);
extern int  re15_player_aim_clip(void);
extern int  re15_player_wbank_clip_fc(int clip);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Clip-Laengen der W-Bank direkt aus der PLW (EDD ab Byte 8: je Clip u16 Bildzahl, u16 Offset) — dieselben
 * Zahlen, die die Plattform beim Bankwechsel ueber re15_player_set_aim_clip_lens einspeist. */
static int bank_laden(int item)
{
    char p[256]; size_t n = 0;
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/PLD/PL00W%02X.PLW", item);
    uint8_t *b = slurp(p, &n);
    if (!b || n < 12) { printf("FAIL: %s nicht lesbar\n", p); fails++; free(b); return 0; }
    int clips = (int)(b[10] | (b[11] << 8)) / 4;
    uint16_t fc[32] = {0};
    if (clips > 32) clips = 32;
    for (int i = 0; i < clips; i++) fc[i] = (uint16_t)(b[8 + 4 * i] | (b[9 + 4 * i] << 8));
    re15_player_set_aim_clip_lens(fc, clips);
    free(b);
    return fc[6];
}

static void spieler(int16_t rot)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0; pl->floor = 0;
    pl->rot_y = rot; pl->state = 0; pl->motion = 200;
}

static void zombie(int slot, int32_t x, int32_t z)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x10; e->state = 1; e->sub_state_1 = 1; e->hp = 100;
    e->hit_radius_min = 400; e->x = x; e->z = z; e->y = 0; e->floor = 0;
}

static int peilung(int32_t x, int32_t z, int32_t px, int32_t pz)
{
    return ((int)re15_atan2_q12(z - pz, x - px) - 0x400) & 0xfff;
}

/* Restwinkel Ziel - Blick, zentriert -2048..2047 */
static int rest(int bearing, int16_t rot)
{
    int d = (bearing - (int)rot) & 0xfff;
    return d >= 0x800 ? d - 0x1000 : d;
}

typedef struct { int dreh_bilder; int letztes_dreh_bild; int rest_ende; int ziel; int16_t folge[48]; } lauf_t;

/* Gegner unter relativem Winkel `rel` (4096 = 360 Grad) zum Blick 0 in Abstand 3000 (Messer 1500), R1 halten, `bilder` Bilder.
 * Gibt je Bild Phase/Clip-Bild/Blick aus, wenn `log`. Bild 0 = das erste Bild mit gehaltenem R1. */
static lauf_t lauf(int item, int rel, int bilder, int log, int vorheben)
{
    const int32_t R = (item < 3) ? 1500 : 3000;   /* Messer ziehen: Radius 0x7d0 @0x800355d0 */
    lauf_t r; memset(&r, 0, sizeof r);
    re15_actor_init();
    re15_treffer_r39_zielhilfe_aus(0);
    spieler(0);
    re15_player_set_equipped_weapon(item);
    bank_laden(item);
    re15_player_aim_reset();
    /* Messer: einmal ziehen + senken, damit der zweite Lauf das WIEDERHEBEN (Sub 0, Clip 6) zeigt. */
    if (vorheben) {
        for (int f = 0; f < 40; f++) re15_player_tick(NULL, RE15_PAD_BIT_R1);
        for (int f = 0; f < 60 && re15_player_aim_active(); f++) re15_player_tick(NULL, 0);
        spieler(0);
    }
    /* Lage per Suche: der Punkt im Abstand 3000, dessen Peilung (Port-Konvention wie die Zielhilfe selbst,
     * (atan2 - 0x400) & 0xfff) dem gewuenschten relativen Winkel am naechsten kommt. */
    int32_t ex = 0, ez = 3000; int best = 1 << 30;
    for (int t = 0; t < 4096; t++) {
        int32_t x = (int32_t)((int64_t)re15_cos_q12(t) * R / 4096);
        int32_t z = (int32_t)((int64_t)re15_sin_q12(t) * R / 4096);
        int d = rest(rel & 0xfff, (int16_t)peilung(x, z, 0, 0)); if (d < 0) d = -d;
        if (d < best) { best = d; ex = x; ez = z; }
    }
    zombie(2, ex, ez);
    int b = peilung(ex, ez, 0, 0);
    re15_actor_t *pl0 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* 0 Grad = Leon blickt GENAU auf die Peilung (die catan-Rasterung des Originals, catan(0) = 1, laesst die
     * +X-Achse auf 0x3ff statt 0 fallen; deshalb den Blick auf die Peilung setzen statt den Gegner zu schieben). */
    if (rel == 0) pl0->rot_y = (int16_t)b;
    int32_t tx, tz; r.ziel = re15_player_aim_target(30000, &tx, &tz);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (log) printf("  item %d rel %d (Peilung %d, Gegner %d,%d) Ziel=%d\n", item, rel, b, ex, ez, r.ziel);
    for (int f = 0; f < bilder; f++) {
        int16_t vor = pl->rot_y;
        re15_player_tick(NULL, RE15_PAD_BIT_R1);
        int ph = 0; re15_player_aim_dbg(&ph, NULL, NULL, NULL, NULL);
        if (pl->rot_y != vor) { r.dreh_bilder++; r.letztes_dreh_bild = f; }
        if (f < 48) r.folge[f] = pl->rot_y;
        if (log) printf("    F%02d phase=%d clip=%d af=%d rot=%6d rest=%5d (%.1f Grad)\n", f, ph,
                        re15_player_aim_clip(), (int)pl->anim_frame, (int)pl->rot_y, rest(b, pl->rot_y),
                        rest(b, pl->rot_y) * 360.0 / 4096.0);
    }
    r.rest_ende = rest(b, pl->rot_y);
    return r;
}

static const struct { int item; const char *name; int vorheben; } k_waffen[] = {
    { 3,  "Pistole (Browning HP, Item 3, Gun-FSM)",        0 },
    { 8,  "Schrotflinte (Item 8, Gun-FSM)",                 0 },
    { 1,  "Messer ZIEHEN (Item 1, Melee Sub 4, Clip 0xD)",  0 },
    { 1,  "Messer WIEDERHEBEN (Item 1, Melee Sub 0, Clip 6)", 1 },
    { 12, "MG (Item 12, Dauerfeuer-FSM)",                   0 },
    { 7,  "Super Redhawk (Item 7, Heben-Clip 8 Bilder)",    0 },
};
static const int k_winkel[] = { 0, 512, 1024, 1536, 2048, -1024, -1536 };

static int mode_mess(void)
{
    printf("== mess ==\n");
    for (unsigned w = 0; w < sizeof k_waffen / sizeof k_waffen[0]; w++) {
        printf("-- %s\n", k_waffen[w].name);
        for (unsigned a = 0; a < sizeof k_winkel / sizeof k_winkel[0]; a++) {
            int voll = (k_winkel[a] == 2048 && w == 0);
            lauf_t r = lauf(k_waffen[w].item, k_winkel[a], 40, voll, k_waffen[w].vorheben);
            printf("   %4d Grad: Ziel=%d Drehbilder=%2d letztes=F%02d Rest=%5d (%.1f Grad)\n",
                   k_winkel[a] * 360 / 4096, r.ziel, r.dreh_bilder, r.letztes_dreh_bild, r.rest_ende,
                   r.rest_ende * 360.0 / 4096.0);
        }
    }
    /* weitere Waffen nur 180 Grad (Uebersicht) */
    static const int k_mehr[] = { 4, 5, 6, 7, 13, 19, 20 };
    printf("-- weitere Waffen, 180 Grad\n");
    for (unsigned i = 0; i < sizeof k_mehr / sizeof k_mehr[0]; i++) {
        lauf_t r = lauf(k_mehr[i], 2048, 40, 0, 0);
        printf("   Item %2d: Heben-Clip-Bilder=%d Drehbilder=%2d Rest=%5d (%.1f Grad)\n", k_mehr[i],
               re15_player_wbank_clip_fc(6), r.dreh_bilder, r.rest_ende, r.rest_ende * 360.0 / 4096.0);
    }
    return 0;
}

static int mode_hinten(void)
{
    printf("== hinten ==\n");
    for (unsigned w = 0; w < sizeof k_waffen / sizeof k_waffen[0]; w++) {
        for (int s = 0; s < 2; s++) {
            int rel = s ? 2048 : -2047;   /* genau hinter Leon und knapp daneben (andere Drehrichtung) */
            lauf_t r = lauf(k_waffen[w].item, rel, 40, 0, k_waffen[w].vorheben);
            printf("  %-50s rel %5d: Drehbilder=%2d Rest=%d\n", k_waffen[w].name, rel, r.dreh_bilder, r.rest_ende);
            CHECK(r.ziel == 1, "%s: Gegner hinter Leon muss Ziel sein", k_waffen[w].name);
            CHECK(r.rest_ende == 0, "%s rel %d: Leon muss nach dem Heben ganz zum Ziel gedreht sein, Rest %d",
                  k_waffen[w].name, rel, r.rest_ende);
        }
    }
    return 0;
}

static int mode_vorne(void)
{
    printf("== vorne ==\n");
    for (unsigned w = 0; w < sizeof k_waffen / sizeof k_waffen[0]; w++) {
        lauf_t r = lauf(k_waffen[w].item, 0, 40, 0, k_waffen[w].vorheben);
        int b = peilung(g_actors[2].x, g_actors[2].z, 0, 0);
        int abw = 0;
        for (int f = 0; f < 40; f++) if (r.folge[f] != (int16_t)b) abw++;
        printf("  %-50s 0 Grad: Peilung %d, Bilder mit Blick != Peilung: %d\n", k_waffen[w].name, b, abw);
        CHECK(abw == 0, "%s: 0-Grad-Fall muss jedes Bild genau auf der Peilung %d stehen (%d Abweichungen)",
              k_waffen[w].name, b, abw);
    }
    return 0;
}

static int mode_wand(void)
{
    printf("== wand ==\n");
    /* ROOM1100 L-Flur (Lage aus test_r39_treffer mode_zielhilfe): Spieler (-14000,-13000), Zombie A (-18500,-24500)
     * hinter der Wandzelle [2]. Spieler blickt von A WEG (Peilung + 180 Grad). */
    char p[256]; size_t n = 0;
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1100.RDT");
    uint8_t *buf = slurp(p, &n);
    if (!buf || re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: ROOM1100\n"); return 1; }
    g_room_rdt_ok = 1; g_current_room_id = 0x1100;
    re15_actor_init();
    re15_treffer_r39_zielhilfe_aus(0);
    re15_player_set_equipped_weapon(3);
    bank_laden(3);
    re15_player_aim_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    spieler(0); pl->x = -14000; pl->z = -13000;
    zombie(2, -18500, -24500);
    int b = peilung(-18500, -24500, pl->x, pl->z);
    pl->rot_y = (int16_t)((b + 2048) & 0xfff);
    int16_t start = pl->rot_y;
    int32_t tx, tz;
    int ziel = re15_player_aim_target(30000, &tx, &tz);
    int bewegt = 0;
    for (int f = 0; f < 40; f++) { re15_player_tick(NULL, RE15_PAD_BIT_R1); if (pl->rot_y != start) bewegt++; }
    printf("  verdeckter Zombie hinter Leon: Ziel=%d, Bilder mit gedrehtem Blick=%d\n", ziel, bewegt);
    CHECK(ziel == 0, "verdeckter Zombie darf kein Ziel sein (Runde 39 Spur D, RE2 @0x800422a8-b0)");
    CHECK(bewegt == 0, "ohne Ziel darf sich Leon beim Heben nicht drehen (%d Bilder)", bewegt);
    /* Gegenprobe: ohne Sichtlinien-Pruefung (alter Stand FUN_8003703c) wird er Ziel und Leon dreht. */
    re15_player_aim_reset(); spieler(start); pl->x = -14000; pl->z = -13000;
    re15_treffer_r39_zielhilfe_aus(1);
    ziel = re15_player_aim_target(30000, &tx, &tz);
    re15_treffer_r39_zielhilfe_aus(0);
    printf("  Gegenprobe ohne Sichtlinie: Ziel=%d\n", ziel);
    CHECK(ziel == 1, "Gegenprobe: ohne Sichtlinie muss der Zombie Ziel sein");
    free(buf);
    return 0;
}

int main(int argc, char **argv)
{
    const char *m = argc > 1 ? argv[1] : "mess";
    int rc;
    if (!strcmp(m, "mess"))        rc = mode_mess();
    else if (!strcmp(m, "hinten")) rc = mode_hinten();
    else if (!strcmp(m, "vorne"))  rc = mode_vorne();
    else if (!strcmp(m, "wand"))   rc = mode_wand();
    else { printf("Modus? mess|hinten|vorne|wand\n"); return 2; }
    if (rc || fails) { printf("RESULT: FAIL (%d)\n", fails); return 1; }
    printf("RESULT: PASS\n");
    return 0;
}
