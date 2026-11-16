/* test_clutanim_monitore.c — RIEGEL: Paletten-Animation der Typ-3-Objekte (Monitore ROOM5030, Wasser).
 *
 * Nutzer 2026-11-15: "Die Monitore im Original laufen uebrigens, also grieseln, bei uns sind die Bilder
 * statisch." Mechanismus und Adressen: include/re15_clutanim.h (FUN_8002b968 / FUN_8002bc04).
 *
 * Die Sollwerte sind im ORIGINAL GEMESSEN (DuckStation, MZD-Mod-Disc, Debug-JUMP "MONITOR ROOM", zwei
 * Savestates, RAM ab 0x800b267c und Pool 0x800b3f98 gelesen):
 *   [0x800b281e] = 7
 *   Eintrag 0..6 Takt/Laenge 4/7, Eintrag 0..4 Quelle VRAM-y 491 (= TIM-Zeile 0), 5..6 = 492 (Zeile 1)
 *   Pool-CLUT-y der Monitor-Vierecke: obj 1..9 = 480..488, obj 10 = 485, obj 11 = 486
 *   Savestate a: Zaehler 12, DR_MOVE sx 32 / 32; Savestate b: Zaehler 17, sx 48 (Puffer 0) / 64 (Puffer 1)
 *   VRAM (304,487) und (304,488) = 0x0000 in drei Aufnahmen.
 *
 *   P1  ROOM5030 ueber den echten Raumaufbau (scd_room_reenter): Anzahl 7, Zielzeilen wie gemessen.
 *   P2  Quellen: Eintrag 0..4 Zeile 0, Eintrag 5 = obj 10 Zeile 1, Eintrag 6 = obj 11 Zeile 1; obj 6 zeigt
 *       deshalb Rauschen (Zeile 1), obwohl sein Modell Zeile 0 nennt.
 *   P3  obj 8 und obj 9: Zielzeile 7 bzw. 8 schreibt kein getickter Eintrag -> 2 (nichts zeichnen).
 *   P4  Takt: nach 12 Bildern Zaehler 12 und sx 32; nach 17 Bildern Zaehler 17, sx 64; 28 Bilder = Umlauf.
 *   P5  Wasser ROOM2000: drei Objekte, Anzahl 3, alle animiert aus Zeile 0.
 *   P6  Zensus aller Typ-3-Objekte der Mod-RDTs: 8 bpp und kein Texel im UV-Bereich mit Index >= 16
 *       (Beleg dafuer, dass die 16 kopierten Farben jeden gezeichneten Texel abdecken).
 *   P7  Gegenprobe: ROOM1140 (kein Typ 3) -> Anzahl 0, objekt() == 0.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_clutanim.h"

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int g_fail = 0;
#define CHECK(c, ...) do { if (!(c)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}

static re15_rdt_t s_rdt;

static int raum(const char *datei, unsigned id)
{
    size_t n = 0;
    char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, datei);
    uint8_t *buf = slurp(p, &n);
    if (!buf) { printf("  FAIL: %s nicht lesbar\n", p); g_fail++; return 0; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("  FAIL: Parse %s\n", p); g_fail++; return 0; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_current_room_id = id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->x = -25000; pl->z = -13000;
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

static void p1_bis_p4(void)
{
    printf("P1-P4 ROOM5030\n");
    if (!raum("STAGE5/ROOM5030.RDT", 0x5030)) return;
    CHECK(re15_clutanim_anzahl() == 7, "Anzahl %d, gemessen 7", re15_clutanim_anzahl());
    static const int soll_ziel[12] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 5, 6 };   /* Pool-CLUT 480+.. gemessen */
    for (int i = 0; i < 12; i++)
        CHECK(re15_clutanim_zielzeile(i) == soll_ziel[i], "obj %d Zielzeile %d, gemessen %d",
              i, re15_clutanim_zielzeile(i), soll_ziel[i]);
    for (int k = 0; k < 7; k++) {
        int z, a, b, ziel, qo, qz, sx;
        re15_clutanim_eintrag(k, &z, &a, &b, &ziel, &qo, &qz, &sx);
        CHECK(a == 4 && b == 7, "Eintrag %d a/b %d/%d, gemessen 4/7", k, a, b);
        CHECK(qz == (k < 5 ? 0 : 1), "Eintrag %d Quellzeile %d", k, qz);
        CHECK(ziel == k, "Eintrag %d Ziel %d", k, ziel);
    }
    { int qo = -1; re15_clutanim_eintrag(5, NULL, NULL, NULL, NULL, &qo, NULL, NULL);
      CHECK(qo == 10, "Eintrag 5 Quelle obj %d, erwartet 10 (Ueberlauf)", qo);
      re15_clutanim_eintrag(6, NULL, NULL, NULL, NULL, &qo, NULL, NULL);
      CHECK(qo == 11, "Eintrag 6 Quelle obj %d, erwartet 11", qo); }
    { int qo = -1, qz = -1, un = -1;
      CHECK(re15_clutanim_objekt(6, &qo, &qz, &un) == 1 && qo == 10 && qz == 1,
            "obj 6 zeigt Quelle obj %d Zeile %d (Soll 10/1 = Rauschen)", qo, qz);
      CHECK(re15_clutanim_objekt(1, &qo, &qz, &un) == 1 && qz == 0, "obj 1 Zeile %d", qz);
      CHECK(re15_clutanim_objekt(8, NULL, NULL, NULL) == 2, "obj 8 muss unsichtbar sein (Zeile 487 = 0)");
      CHECK(re15_clutanim_objekt(9, NULL, NULL, NULL) == 2, "obj 9 muss unsichtbar sein (Zeile 488 = 0)");
      CHECK(re15_clutanim_objekt(0, NULL, NULL, NULL) == 0, "obj 0 nicht installiert");
      CHECK(re15_clutanim_max_unter(1) == 6, "max Unterpalette %d, Soll (4*7-1)/4 = 6", re15_clutanim_max_unter(1)); }

    /* P4: Savestate a — Zaehler 12, letztes sx 32 */
    for (int f = 0; f < 12; f++) re15_clutanim_tick();
    { int z, sx; re15_clutanim_eintrag(0, &z, NULL, NULL, NULL, NULL, NULL, &sx);
      CHECK(z == 12 && sx == 32, "nach 12 Bildern Zaehler %d sx %d, gemessen 12/32", z, sx);
      int z7; re15_clutanim_eintrag(7, &z7, NULL, NULL, NULL, NULL, NULL, NULL);
      CHECK(z7 == 0, "Eintrag 7 darf nicht ticken (Zaehler %d)", z7); }
    for (int f = 12; f < 17; f++) re15_clutanim_tick();
    { int z, sx, un = -1; re15_clutanim_eintrag(3, &z, NULL, NULL, NULL, NULL, NULL, &sx);
      CHECK(z == 17 && sx == 64, "nach 17 Bildern Zaehler %d sx %d, gemessen 17/64", z, sx);
      re15_clutanim_objekt(3, NULL, NULL, &un);
      CHECK(un == 4, "Unterpalette %d, Soll 16/4 = 4", un); }
    for (int f = 17; f < 28; f++) re15_clutanim_tick();
    { int z; re15_clutanim_eintrag(6, &z, NULL, NULL, NULL, NULL, NULL, NULL);
      CHECK(z == 0, "nach 28 Bildern Zaehler %d, Soll 0 (Umlauf b*a = 28)", z); }
}

static void p5(void)
{
    printf("P5 ROOM2000 Wasser\n");
    if (!raum("STAGE2/ROOM2000.RDT", 0x2000)) return;
    CHECK(re15_clutanim_anzahl() == 3, "Anzahl %d, Soll 3", re15_clutanim_anzahl());
    for (int i = 0; i < 3; i++) {
        int qo = -1, qz = -1;
        CHECK(re15_clutanim_objekt(i, &qo, &qz, NULL) == 1 && qo == i && qz == 0,
              "Wasser obj %d: Quelle %d Zeile %d", i, qo, qz);
    }
}

static int tim_index(const re15_tim_t *t, int x, int y)
{
    const uint8_t *s = (const uint8_t *)t->pixels;
    if (x < 0 || y < 0 || x >= t->width || y >= t->height) return 0;
    return s[(long)y * t->width + x];
}

static void p6(void)
{
    printf("P6 Zensus Typ-3-Texturen\n");
    static const char *raeume[] = {
        "STAGE1/ROOM11A0.RDT", "STAGE1/ROOM11A1.RDT", "STAGE1/ROOM1260.RDT", "STAGE1/ROOM1261.RDT",
        "STAGE2/ROOM2000.RDT", "STAGE2/ROOM2001.RDT", "STAGE2/ROOM2020.RDT", "STAGE2/ROOM2021.RDT",
        "STAGE2/ROOM2040.RDT", "STAGE2/ROOM2041.RDT", "STAGE2/ROOM2050.RDT", "STAGE2/ROOM2051.RDT",
        "STAGE2/ROOM2070.RDT", "STAGE2/ROOM2071.RDT", "STAGE2/ROOM2080.RDT", "STAGE2/ROOM2081.RDT",
        "STAGE2/ROOM2090.RDT", "STAGE2/ROOM2091.RDT", "STAGE2/ROOM20A0.RDT", "STAGE2/ROOM20A1.RDT",
        "STAGE2/ROOM20B0.RDT", "STAGE2/ROOM20B1.RDT", "STAGE5/ROOM5030.RDT", "STAGE5/ROOM5031.RDT" };
    /* Typ-3-Objekte je Raum laut Obj_model_set-Zensus (scratchpad zensus.py, MECHANISMUS.md §3) — hier
     * reicht: JEDES Prop-Modell dieser Raeume, dessen TIM 8 bpp ist, wird geprueft; Typ-3-Objekte sind darunter. */
    int geprueft = 0;
    for (unsigned r = 0; r < sizeof raeume / sizeof raeume[0]; r++) {
        size_t n = 0; char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, raeume[r]);
        uint8_t *buf = slurp(p, &n);
        re15_rdt_t rd;
        if (!buf || re15_rdt_parse(buf, n, &rd) != 0) { CHECK(0, "%s", raeume[r]); free(buf); continue; }
        /* die Typ-3-Objekte dieses Raums ueber den echten Raumaufbau bestimmen */
        if (!raum(raeume[r], 0)) { free(buf); continue; }
        printf("  %s: %d angemeldet\n", raeume[r], re15_clutanim_anzahl());
        for (int o = 0; o < rd.prop_count && o < RE15_RDT_MAX_PROPS; o++) {
            if (re15_clutanim_zielzeile(o) < 0) continue;
            re15_tim_t t; re15_md1_t m;
            CHECK(re15_tim_parse(rd.prop_tim[o], rd.prop_tim_size[o], &t) == 0 && t.bpp == 8,
                  "%s obj %d: TIM nicht 8 bpp", raeume[r], o);
            if (re15_md1_parse(rd.prop_md1[o], rd.prop_md1_size[o], &m) != 0) continue;
            int maxi = 0;
            for (int h = 0; h < m.mesh_count; h++) {
                const re15_md1_mesh_t *me = &m.meshes[h];
                for (int i = 0; i < me->triangle_count; i++) {
                    const re15_md1_tri_uv_t *u = &me->triangle_uvs[i];
                    int u0 = u->u0, u1 = u->u0, v0 = u->v0, v1 = u->v0;
                    int us[2] = { u->u1, u->u2 }, vs[2] = { u->v1, u->v2 };
                    for (int q = 0; q < 2; q++) { if (us[q] < u0) u0 = us[q]; if (us[q] > u1) u1 = us[q];
                                                  if (vs[q] < v0) v0 = vs[q]; if (vs[q] > v1) v1 = vs[q]; }
                    for (int y = v0; y <= v1; y++) for (int x = u0; x <= u1; x++) { int k = tim_index(&t, x, y); if (k > maxi) maxi = k; }
                }
                for (int i = 0; i < me->quad_count; i++) {
                    const re15_md1_quad_uv_t *u = &me->quad_uvs[i];
                    int u0 = u->u0, u1 = u->u0, v0 = u->v0, v1 = u->v0;
                    int us[3] = { u->u1, u->u2, u->u3 }, vs[3] = { u->v1, u->v2, u->v3 };
                    for (int q = 0; q < 3; q++) { if (us[q] < u0) u0 = us[q]; if (us[q] > u1) u1 = us[q];
                                                  if (vs[q] < v0) v0 = vs[q]; if (vs[q] > v1) v1 = vs[q]; }
                    for (int y = v0; y <= v1; y++) for (int x = u0; x <= u1; x++) { int k = tim_index(&t, x, y); if (k > maxi) maxi = k; }
                }
            }
            CHECK(maxi < 16, "%s obj %d: Texel-Index %d >= 16 im UV-Bereich", raeume[r], o, maxi);
            geprueft++;
        }
        free(buf);
    }
    CHECK(geprueft == 60, "%d Typ-3-Objekte angemeldet, Soll 60 (Raumaufbau mit Eintritt 0; Zahlen je Raum oben)", geprueft);
    printf("  %d animierte Objekte geprueft\n", geprueft);
}

static void p7(void)
{
    printf("P7 Gegenprobe ROOM1140\n");
    if (!raum("STAGE1/ROOM1140.RDT", 0x1140)) return;
    CHECK(re15_clutanim_anzahl() == 0, "Anzahl %d, Soll 0", re15_clutanim_anzahl());
    for (int i = 0; i < 8; i++) CHECK(re15_clutanim_objekt(i, NULL, NULL, NULL) == 0, "obj %d animiert?", i);
}

int main(void)
{
    p1_bis_p4();
    p5();
    p6();
    p7();
    printf(g_fail ? "FAIL (%d)\n" : "PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
