/* ============================================================================
 *  test_fx_region_cull.c — WACHE fuer den Region-Cull der ESP-Effekt-Sprites.
 *
 *  NUTZER-BEFUND 2026-08-27: "alle Effekte wie Strom, Feuer etc. ueberdecken nicht
 *  sichtbare Bereiche. Zum Beispiel wenn sie noch um die Ecke hinter der Kamera sind."
 *
 *  Ursache: der Port zeichnete Effekt-Sprites OHNE den Sichtbarkeits-Test, den er fuer
 *  Spieler, NPCs und Props laengst hatte. Das Original fuehrt ihn in der ESP-Slot-Schleife
 *  FUN_80053240 direkt nach den flags-Gates und VOR jeder Projektion aus:
 *      800532fc: andi v0,v1,0x1           ; aktiv?
 *      80053308: andi v0,v1,0x2           ; sichtbar?
 *      80053314: lh   v0,-68(s0)          ; s0 = slot+0x6C -> slot+0x28 = World-X
 *      8005331c: lw   a1,-14448(a1)       ; a1 = DAT_800ac790 = Region-Quad des aktiven Cuts
 *      80053330: lh   v0,-64(s0)          ;                     slot+0x2C = World-Z
 *      80053334: jal  0x80014368          ; Punkt-im-Viereck (X/Z)
 *      8005333c: beq  v0,zero,0x80053474  ; AUSSERHALB -> Slot komplett uebersprungen
 *  DAT_800ac790 setzt der Cut-Wechsel (@0x80021c00 `jal 0x80014324` / @0x80021c0c
 *  `sw v0,-14448(at)`); FUN_80014324 liefert den ersten RVD-Satz (20 Byte, Tabelle RDT+0x28)
 *  mit rec[+2] == Cut-Id — im Port re15_rdt_get_region_quad().
 *
 *  DIE WACHE IST NICHT VAKUANT: sie faehrt die ECHTE SCD-VM ueber alle Raeume aller sechs
 *  Stages,
 *  sammelt die WIRKLICH gespawnten Effekt-Partikel und kreuzt jeden gegen JEDES Region-Quad
 *  des Raums. Verlangt werden BEIDE Richtungen:
 *    (a) es muss Paare geben, die GECULLT werden — das sind genau die Faelle, die der Nutzer
 *        gesehen hat (vor dem Fix wurden sie gezeichnet). Sind es null, misst die Wache nichts.
 *    (b) jeder Effekt muss in MINDESTENS EINEM Cut sichtbar bleiben — sonst hat ein zu
 *        scharfer Cull die Effekte ganz abgeschaltet und "gruen" waere eine Luege.
 *  Zusaetzlich Mindestzahlen fuer Raeume, Partikel und ausgewertete Paare.
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_esp.h"
#include "re15_actor.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)
#define RDT_MIN_SIZE 0x60

/* Gemessen 2026-08-27: 103 Raeume (8 mit Effekten), 124 Partikel, 1484 Paare, 944 gecullt
 * (63%), 0 Waisen. Die Schranken liegen darunter, aber weit ueber null. */
#define MIN_ROOMS          80
#define MIN_PARTICLES     100
#define MIN_PAIRS        1000
#define MIN_CULLED_PAIRS  500

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f); fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz; return buf;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int rooms = 0, particles = 0, pairs = 0, culled = 0, orphan = 0, fail = 0;
    int rooms_with_fx = 0;

    printf("=== ESP-Region-Cull-Zensus (FUN_80053240 @0x80053334) ===\n");

    for (unsigned key = 0; key < 6u * 0x28u; key++) {
        int      stage = 1 + (int)(key / 0x28u);
        unsigned rid   = (unsigned)stage * 0x1000u + (key % 0x28u) * 0x10u;
        char path[600];
        snprintf(path, sizeof path, "%s/STAGE%d/ROOM%04X.RDT", base, stage, rid);
        size_t sz = 0;
        uint8_t *buf = read_file(path, &sz);
        if (!buf) continue;
        if (sz < RDT_MIN_SIZE) { free(buf); continue; }
        re15_rdt_t rdt;
        if (re15_rdt_parse(buf, sz, &rdt) != 0) { free(buf); continue; }

        scd_vm_init();
        re15_esp_fx_reset();
        g_current_room_id = rid;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, 0, 0, /*entry_scenario=*/0);
        for (int f = 0; f < 8; f++) scd_vm_tick();
        for (int e = 0; e < rdt.sub_scd_count; e++) {
            if (!rdt.sub_scd[e]) continue;
            scd_event_fire((uint8_t)e);
            for (int f = 0; f < 6; f++) scd_vm_tick();
        }
        rooms++;

        /* Alle Region-Quads dieses Raums einsammeln (ein Quad je Cut, sofern vorhanden). */
        int16_t qx[64][4], qz[64][4];
        int     has[64];
        int ncuts = 0;
        for (int c = 0; c < 64; c++) {
            has[c] = re15_rdt_get_region_quad(&rdt, c, qx[c], qz[c]);
            if (has[c]) ncuts++;
        }

        int room_fx = 0;
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(i);
            if (!f || !f->active) continue;
            int32_t px = f->x + f->xlat_x, pz = f->z + f->xlat_z;
            int vis_anywhere = 0, this_pairs = 0, this_culled = 0;
            for (int c = 0; c < 64; c++) {
                if (!has[c]) continue;
                this_pairs++;
                if (re15_esp_fx_culled(px, pz, 1, qx[c], qz[c])) this_culled++;
                else vis_anywhere = 1;
            }
            if (!this_pairs) continue;      /* Raum ohne Region-Daten: Original cullt nicht */
            particles++;
            room_fx++;
            pairs  += this_pairs;
            culled += this_culled;
            if (!vis_anywhere) {
                printf("  WARN ROOM%04X fx id=%d @(%d,%d): in KEINEM der %d Cuts sichtbar\n",
                       rid, f->effect_id, px, pz, this_pairs);
                orphan++;
            }
        }
        if (room_fx) {
            rooms_with_fx++;
            printf("  ROOM%04X: %d Partikel, %d Cut-Quads\n", rid, room_fx, ncuts);
        }
        free(buf);
    }

    printf("\n%d Raeume gefahren (%d mit Effekten), %d Partikel, %d (Partikel,Cut)-Paare, "
           "%d gecullt (%d%%), %d ohne sichtbaren Cut.\n",
           rooms, rooms_with_fx, particles, pairs, culled,
           pairs ? (culled * 100 / pairs) : 0, orphan);

    /* --- (a) der Cull muss wirklich etwas tun ------------------------------------------ */
    if (culled < MIN_CULLED_PAIRS) {
        printf("FAIL: nur %d gecullte Paare (erwartet >= %d). Entweder ist der Cull "
               "abgeschaltet — dann ist der Nutzer-Fehler zurueck — oder die Messung "
               "kollabiert.\n", culled, MIN_CULLED_PAIRS);
        fail = 1;
    } else {
        printf("  PASS: %d Paare werden gecullt (@0x8005333c beq -> Slot uebersprungen)\n", culled);
    }

    /* --- (b) kein Ueber-Cull ------------------------------------------------------------ */
    if (orphan) {
        printf("FAIL: %d Partikel sind in KEINEM Cut sichtbar — der Cull ist zu scharf "
               "(das Original zeichnet jeden Effekt in seiner eigenen Region).\n", orphan);
        fail = 1;
    } else {
        printf("  PASS: jeder Partikel bleibt in mindestens einem Cut sichtbar\n");
    }

    /* --- Nicht-vakuant ------------------------------------------------------------------ */
    if (rooms < MIN_ROOMS || particles < MIN_PARTICLES || pairs < MIN_PAIRS) {
        printf("FAIL: Messung zu klein (Raeume %d/%d, Partikel %d/%d, Paare %d/%d) — "
               "gruen waere hier bedeutungslos\n",
               rooms, MIN_ROOMS, particles, MIN_PARTICLES, pairs, MIN_PAIRS);
        fail = 1;
    }

    if (fail) { printf("FX REGION CULL: FAIL\n"); return 1; }
    printf("FX REGION CULL: Effekte folgen dem Region-Gate des Originals\n");
    return 0;
}
