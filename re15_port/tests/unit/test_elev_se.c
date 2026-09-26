/* test_elev_se.c — RIEGEL fuer den FAHRSTUHL-FAHRTON (⛔ RE2-ERGAENZUNG).
 *
 * Verlangt wird dreierlei:
 *  A) Die Mini-Bank shared_assets/RE2/ELEVSE.VBS laedt mit DEMSELBEN VAB-Code wie
 *     RE1.5 (re15_vab_parse / re15_edt_decode / re15_edt_resolve_layers_ex) und
 *     liefert genau die zwei aus RE2 geschnittenen Wellen:
 *       se 0x11 -> prog0 tone14 -> VAG 16400 B, pitch 761  (ROOM21B0.RDT @0x5C08 / @0x6664 / @0x1BA34)
 *       se 0x12 -> prog0 tone15 -> VAG  6336 B, pitch 861  (ROOM21B0.RDT @0x5C0C / @0x6684 / @0x1FA44)
 *     Jeder andere SE-Platz der Bank ist leer (keine Zufallstreffer).
 *  B) Der Anker-Scan findet im GELADENEN ROOM1080.RDT genau die gemessenen
 *     Fundstellen (0x746 / 0x7D8 / 0x86A, je zwei Pulse = 6 Anker) und in einem
 *     Nicht-Fahrstuhlraum (ROOM1140) KEINEN.
 *  C) Beim ECHTEN Fahrt-Ereignis im ECHTEN SCD-VM werden genau die beiden Ids
 *     0x11 und 0x12 ausgeloest, in dieser Reihenfolge, je einmal.
 *
 * Belege fuer jede Zahl: Kopf von engine/src/scd_elev_se.c und
 * tools/re2_elevator_cut.py. */

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_inventory.h"
#include "re15_vab.h"
#include "re15_elev_se.h"

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

extern scd_vm_t g_scd;
void scd_register_current_rdt(const re15_rdt_t *rdt);
extern int g_test_elev_se_last, g_test_elev_se_count;   /* test_support.c-Spion */

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("  FEHLER: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- A) die Bank -------------------------------------------------------- */
static void teil_a(void)
{
    printf("A) Mini-Bank ELEVSE.VBS\n");
    re15_elev_bank_rec_t rec;
    re15_elev_bank_rec(&rec);

    char p[600];
    snprintf(p, sizeof p, "%s/ELEVSE.VBS", RE15_ASSET_RE2_DIR);
    size_t n = 0;
    uint8_t *vbs = slurp(p, &n);
    if (!vbs) { printf("  FEHLER: %s nicht lesbar\n", p); g_fail++; return; }
    CHECK(n == rec.vbd_off + rec.vbd_size, "Dateigroesse %zu != %u", n,
          rec.vbd_off + rec.vbd_size);

    const uint8_t *edt = vbs + rec.edt_off;
    uint32_t vh_off = (uint32_t)edt[rec.edt_size-8]         | ((uint32_t)edt[rec.edt_size-7] << 8)
                    | ((uint32_t)edt[rec.edt_size-6] << 16) | ((uint32_t)edt[rec.edt_size-5] << 24);
    CHECK(vh_off + 4 <= rec.edt_size && memcmp(edt + vh_off, "pBAV", 4) == 0,
          "kein pBAV @vh_off=0x%X", vh_off);

    re15_vab_t vab;
    CHECK(re15_vab_parse(edt + vh_off, rec.edt_size - vh_off, &vab) == 0, "re15_vab_parse");
    CHECK(vab.vag_count == 2, "vag_count %d != 2", vab.vag_count);
    if (vab.vag_count == 2) {
        CHECK(vab.samples[0].size == rec.vag1_size, "VAG1 %u != %u",
              vab.samples[0].size, rec.vag1_size);
        CHECK(vab.samples[1].size == rec.vag2_size, "VAG2 %u != %u",
              vab.samples[1].size, rec.vag2_size);
    }

    /* Die zwei Saetze — Werte aus ROOM21B0.RDT @0x5C08/@0x5C0C + @0x6664/@0x6684. */
    struct { int se, tone, vag, pitch; } erwartet[2] = {
        { rec.se_ride,   14, 0, 761 },   /* Fahrt:   note73 fine57 center103 -> LUT[6][14]>>3 */
        { rec.se_arrive, 15, 1, 861 },   /* Ankunft: note74 fine 0 center101 -> LUT[9][ 0]>>3 */
    };
    for (int k = 0; k < 2; k++) {
        re15_edt_rec_t r;
        CHECK(re15_edt_decode(edt, erwartet[k].se, &r) == 0 && !r.empty,
              "EDT 0x%02X leer", erwartet[k].se);
        CHECK(r.prog == 0 && r.tone == erwartet[k].tone && r.prio == 3 &&
              r.voice == -16 && r.extra == 0,
              "EDT 0x%02X: prog=%d tone=%d prio=%d voice=%d extra=%d",
              erwartet[k].se, r.prog, r.tone, r.prio, r.voice, r.extra);
        int vags[8], tones[8];
        int cnt = re15_edt_resolve_layers_ex(edt, &vab, erwartet[k].se, vags, tones, 8);
        CHECK(cnt == 1, "SE 0x%02X: %d Lagen statt 1", erwartet[k].se, cnt);
        if (cnt == 1) {
            CHECK(vags[0] == erwartet[k].vag, "SE 0x%02X -> VAG %d statt %d",
                  erwartet[k].se, vags[0], erwartet[k].vag);
            const re15_vab_tone_t *t = &vab.tones[tones[0]];
            uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                                  t->center_note, t->pitch_shift);
            CHECK(pitch == erwartet[k].pitch, "SE 0x%02X: pitch %u statt %d",
                  erwartet[k].se, pitch, erwartet[k].pitch);
            printf("   SE 0x%02X: tone%d vol=%u pan=%u note=%u fine=%u center=%u "
                   "pitch=%u (%u Hz) VAG %u B\n",
                   erwartet[k].se, tones[0], t->vol, t->pan, t->min_note, t->pitch_shift,
                   t->center_note, pitch, (unsigned)((44100u * pitch) >> 12),
                   vab.samples[vags[0]].size);
        }
    }
    /* Kein weiterer SE-Platz darf belegt sein. */
    int belegt = 0;
    for (int se = 0; se < (int)(vh_off / 4); se++) {
        re15_edt_rec_t r;
        if (re15_edt_decode(edt, se, &r) == 0 && !r.empty) belegt++;
    }
    CHECK(belegt == 2, "%d belegte SE-Plaetze statt 2", belegt);
    free(vbs);
}

/* ---- B) die Anker ------------------------------------------------------- */
static void teil_b(void)
{
    printf("B) Anker-Scan aus den Raumdaten\n");
    /* Die GEMESSENE Fundstellen-Tabelle (gen/re15_elev_se.inc). */
    CHECK(re15_elev_se_hit_count() == 12, "Fundstellen-Tabelle %d statt 12",
          re15_elev_se_hit_count());
    int n1080 = 0;
    for (int i = 0; i < re15_elev_se_hit_count(); i++) {
        unsigned room = 0, off = 0;
        re15_elev_se_hit(i, &room, &off);
        CHECK(room == 0x1080 || room == 0x1081 || room == 0x4020 || room == 0x4021,
              "Fundstelle in fremdem Raum 0x%04X", room);
        if (room == 0x1080) n1080++;
    }
    CHECK(n1080 == 3, "ROOM1080: %d Fundstellen statt 3", n1080);

    size_t n = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1080.RDT", &n);
    if (!raw) { printf("  FEHLER: ROOM1080.RDT fehlt\n"); g_fail++; return; }
    re15_rdt_t rdt;
    CHECK(re15_rdt_parse(raw, n, &rdt) >= 0, "re15_rdt_parse ROOM1080");
    scd_register_current_rdt(&rdt);          /* <- die Verdrahtung beim Raumwechsel */
    CHECK(g_re15_elev_anchor_n == 6, "ROOM1080: %d Anker statt 6", g_re15_elev_anchor_n);

    /* Die Anker muessen an den gemessenen Datei-Offsets liegen (0x746/0x7D8/0x86A,
     * Puls 2 jeweils +0x10). Geprueft ueber den Ausloeser selbst. */
    static const unsigned off1080[3] = { 0x746, 0x7D8, 0x86A };
    for (int k = 0; k < 3; k++) {
        re15_elev_se_reset_log();
        g_test_elev_se_count = 0; g_test_elev_se_last = -1;
        re15_elev_se_pc(raw + off1080[k]);
        re15_elev_se_pc(raw + off1080[k] + 0x10);
        const unsigned char *f = NULL;
        int cnt = re15_elev_se_fired(&f);
        CHECK(cnt == 2 && f[0] == 0x11 && f[1] == 0x12,
              "Offset 0x%X: %d Ausloeser %02X/%02X statt 2x 11/12",
              off1080[k], cnt, cnt > 0 ? f[0] : 0, cnt > 1 ? f[1] : 0);
        CHECK(g_test_elev_se_count == 2, "Audio-Ruf %d statt 2", g_test_elev_se_count);
    }
    /* Keine Fehlauslesung an einer beliebigen anderen Stelle des Raums. */
    re15_elev_se_reset_log();
    for (size_t o = 0; o < n; o++) {
        if (o == 0x746 || o == 0x756 || o == 0x7D8 || o == 0x7E8 ||
            o == 0x86A || o == 0x87A) continue;
        re15_elev_se_pc(raw + o);
    }
    { const unsigned char *f = NULL;
      CHECK(re15_elev_se_fired(&f) == 0, "Fehlauslesung an fremder Stelle"); }
    scd_register_current_rdt(NULL);
    free(raw);

    /* Nicht-Fahrstuhlraum: kein einziger Anker. */
    size_t n2 = 0;
    uint8_t *raw2 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &n2);
    if (raw2) {
        re15_rdt_t r2;
        if (re15_rdt_parse(raw2, n2, &r2) >= 0) {
            scd_register_current_rdt(&r2);
            CHECK(g_re15_elev_anchor_n == 0, "ROOM1140: %d Anker statt 0",
                  g_re15_elev_anchor_n);
            scd_register_current_rdt(NULL);
        }
        free(raw2);
    }
}

/* ---- C) das echte Fahrt-Ereignis im echten VM ---------------------------- */
static void teil_c(void)
{
    printf("C) Fahrt im echten SCD-VM\n");
    size_t n = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1080.RDT", &n);
    if (!raw) { printf("  FEHLER: ROOM1080.RDT fehlt\n"); g_fail++; return; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, n, &rdt) < 0) { printf("  FEHLER: parse\n"); g_fail++; free(raw); return; }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_inv_init();
    re15_game_state_init();
    memset(&g_scd, 0, sizeof g_scd);
    g_scd.work_slot = -1;
    g_current_room_id = 0x1080;
    scd_register_current_rdt(&rdt);
    re15_elev_se_reset_log();
    g_test_elev_se_count = 0;

    /* Den Faden genau auf die Fahrt setzen (Datei-Offset 0x746, selbst gemessen):
     * Set bit28 / Sleep 8 / Set 0 / Sleep 90 / Set bit28 / Sleep 8 / Set 0 / Sleep 20. */
    scd_thread_start(0, rdt.raw + 0x746);
    for (int fr = 0; fr < 200; fr++) scd_vm_tick();

    const unsigned char *f = NULL;
    int cnt = re15_elev_se_fired(&f);
    printf("   ausgeloest: %d (", cnt);
    for (int i = 0; i < cnt; i++) printf("%s0x%02X", i ? "," : "", f[i]);
    printf(")\n");
    CHECK(cnt == 2, "%d Ausloeser statt 2", cnt);
    if (cnt >= 1) CHECK(f[0] == 0x11, "erster Ausloeser 0x%02X statt 0x11", f[0]);
    if (cnt >= 2) CHECK(f[1] == 0x12, "zweiter Ausloeser 0x%02X statt 0x12", f[1]);
    CHECK(g_test_elev_se_count == cnt, "Audio-Rufe %d != Ausloeser %d",
          g_test_elev_se_count, cnt);

    scd_register_current_rdt(NULL);
    free(raw);
}

int main(void)
{
    printf("=== RIEGEL Fahrstuhl-Fahrton (RE2-ERGAENZUNG) ===\n");
    teil_a();
    teil_b();
    teil_c();
    if (g_fail) { printf("FEHLGESCHLAGEN: %d Pruefung(en)\n", g_fail); return 1; }
    printf("OK\n");
    return 0;
}
