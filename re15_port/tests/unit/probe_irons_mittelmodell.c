/* probe_irons_mittelmodell.c — ROOM1150 (Irons' Buero): WAS steht in der Mitte, und WAS
 * loest der Aktionsdruck daran wirklich aus?
 *
 * NUTZER-FRAGE (woertlich): "du sagtest du hast dieses tisch model in der mitte von Irons
 * Office repariert zum ausloesen. wie triggert man das?" — und auf die Antwort "das ist die
 * Item-Box": "nein, nicht die item boxen. irgendwie konnte man in den modell in der mitte
 * noch eine ani[mation]".
 *
 * RDT-GRUNDWAHRHEIT (re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT, Datei-Offsets, selbst
 * gedumpt mit re15_port/tools/scd_dump_room.py — Walk opcode-exakt bis Evt_end, kein Desync):
 *
 *   main00 @0x0D5E:
 *     @0x0D7E  2c 01 00 31 00 00 d8 aa e0 b1 dc 05 e4 0c  ff 00 18 04 00 00
 *              = Aot_set slot=1, sce=0(!), flags=0x31 (ACTION), Rect Ecke(-21800,-20000)
 *                Groesse(1500,3300) -> Zentrum(-21050,-18350) Halb(750,1650);
 *                Nutzlast {ff 00 18 04} = dieselbe Event-Form wie die Nachbar-Records
 *                (vgl. slot3 "...ff 00 18 06" -> sub6, slot5 "...ff 00 18 07" -> sub7)
 *                -> Ziel = sub 4.
 *                sce=0 -> Handler[0] @0x8004305C = INERT. Der Auslöser ist im
 *                Auslieferungsstand ABGESCHALTET, sein Rechteck aber vollstaendig da.
 *     @0x0DBA  2c 04 01 31 00 00 74 aa f8 ad 68 10 50 14 00 00 ff ff 00 00
 *              = Aot_set slot=4, sce=1 MESSAGE msg0, Ecke(-21900,-21000) Groesse(4200,5200)
 *                -> x[-21900..-17700] z[-21000..-15800]. Dieses Rechteck UMSCHLIESST den
 *                ganzen Schreibtisch-Bereich UND slot1.
 *     @0x0DCE  2c 05 03 31 ... ff 00 18 07  = Item-Box-Tisch (sub07, Message 3)
 *     @0x0E00  2d 00 ... 24 af 9c b0 cc bb ... 00 08 = Obj 0 (MD1) @(-20700,-20324,-17460)
 *                rot_y=2048 — ausserhalb des Raums geparkt.
 *     @0x0E66  2d 03 ... 14 a1 00 00 4c d2 ... 00 04 = Obj 3 (MD1) @(-24300,0,-11700)
 *                rot_y=1024, Box c(0,-1080,0) h(900,1080,720) = die ITEM-BOX
 *                (deckungsgleich mit Aot-Slot 5, vgl. test_room1150_itembox.c).
 *
 *   sub04 @0x0F96..0x10B6 = die Szene: Cut_chg(4) @0x0FB2, Work_set(3,0) = OBJEKT 0,
 *     Pos_set(-20700,-305,-17460) @0x0FB4 (holt Obj 0 aus der Parkposition in den Raum),
 *     dann Speed_set/Add_speed auf Achse 1 ueber For-Schleifen 91/10/90/2/2 Bilder mit
 *     Se_on-Geraeuschen (Bank 2, ids 0x0a/0x0c/0x0d), am Ende Pos_set y=-20224 @0x109E =
 *     parkt sich selbst zurueck und loest die Cutscene-Klammern (@0x10A6-0x10B2).
 *
 * DIESE SONDE MISST (nicht modelliert — echter Scan-Pfad re15_aot_scan):
 *   A) welche AOT-Records der Raum installiert (inkl. der Port-Zugabe in Slot 60)
 *   B) wo die vier Objekte stehen
 *   C) Gegenprobe: laeuft sub04, wenn man es direkt feuert?
 *   D) Raster von Spielerstandorten + Blickrichtungen rund um den Schreibtisch: startet
 *      IRGENDEINE Kombination die Szene — oder kommt immer nur der Text aus Slot 4?
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
#include "re15_msg.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_collision.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;
extern void             re15_actors_anim_advance(void);

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FEHL: %s\n", name); g_fail = 1; } \
                               else printf("  OK  : %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static re15_rdt_t s_rdt;
static int s_last_msg   = -1;
static int s_scene_seen = 0;    /* Objekt 0 auf Arbeitshoehe -305 gesehen */

static void tick(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    re15_actors_anim_advance();
    re15_aot_stamp_entities();
    re15_object_notch_update();
    g_aot_action_pressed = 0;
    if (g_scd.message_display_frames > 0 || g_scd.message_query)
        s_last_msg = (int)g_scd.message_id;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].obj_id == 0 && g_scd.props[i].y == -305) s_scene_seen = 1;
}

/* ⛔ DEN TEXT WIRKLICH SCHLIESSEN. Ein Examine-Text traegt die Pause-Maske 0xffff
 * (Payload u16@+2, @0x80043098) und friert damit Spieler/AI/Anim/Skript ueber
 * g_re15_pauseflags (= DAT_800aca40) ein. Wer nur message_display_frames auf 0 setzt,
 * laesst die Maske stehen — dann ist player_mode 2, in_cinematic wahr, und der AOT-Scan
 * unterdrueckt ab da JEDEN Nicht-Kamera-Record. Genau daran ist der erste Messlauf
 * dieser Sonde gescheitert (nach dem ersten Text war alles tot und "0 Treffer" war ein
 * Artefakt der Messung, nicht des Spiels). */
static void msg_reset(void)
{
    g_scd.message_display_frames = 0;
    g_scd.message_query          = 0;
    g_scd.message_active         = 0;
    re15_pauseflags_clear();
}

static int enter(void)
{
    char rp[600]; size_t n = 0;
    snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, "STAGE1/ROOM1150.RDT");
    uint8_t *raw = slurp(rp, &n);
    if (!raw) return 0;
    if (re15_rdt_parse(raw, n, &s_rdt) < 0) { free(raw); return 0; }
    scd_vm_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = 0x1150; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -17150; pl->y = 0; pl->z = -11960; pl->rot_y = 1600;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 12; f++) tick();
    return 1;
}

/* Ein Aktionsdruck an (x,z) mit Blick rot. Rueckgabe 1 = Szene sub04 lief an. */
static int press_at(int32_t x, int32_t z, int16_t rot, int *out_msg)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z; pl->rot_y = rot;
    s_last_msg = -1; s_scene_seen = 0;
    msg_reset();
    for (int f = 0; f < 3; f++) tick();          /* msg_block abbauen */
    msg_reset();
    s_last_msg = -1;
    g_aot_action_pressed = 1;
    tick();
    /* sub04 braucht bis zum Pos_set nur Se_on + Sleep(5) + Cut_chg (@0x0FA2-0x0FB4) —
     * 25 Bilder reichen sicher, damit das Raster nicht minutenlang laeuft. */
    for (int f = 0; f < 25; f++) tick();
    *out_msg = s_last_msg;
    return s_scene_seen;
}

/* Alte Fassung nachstellen: Slot 1 zurueck auf inert, dafuer der erfundene Record in
 * Slot 60 (so wie scd_room_setup.c es bis 2026-09-20 tat). */
static void stelle_alte_fassung_her(void)
{
    g_aot.slots[1].type = RE15_AOT_TYPE_NONE;
    re15_aot_set(60, RE15_AOT_TYPE_GENERIC, 4, -20700, -17460, 900, 900);
}

/* Das Raster ueber den Schreibtisch-Bereich. alt=1 stellt vor jedem Raum-Eintritt die
 * Fassung vom 2026-08-30 her. Rueckgabe = Zahl der Kombinationen, die sub04 starten. */
static int raster(const char *titel, int alt)
{
    printf("-- Aktionsdruck-Raster (%s) --\n", titel);
    if (!enter()) return -1;
    if (alt) stelle_alte_fassung_her();
    int treffer = 0, felder = 0, mit_text = 0, spielbar = 0;
    int msg_hist[16]; memset(msg_hist, 0, sizeof msg_hist);
    int32_t best_x = 0, best_z = 0, best_r = -1;
    for (int32_t x = -23000; x <= -18500; x += 300) {
        for (int32_t z = -21500; z <= -15000; z += 300) {
            for (int r = 0; r < 8; r++) {
                int msg = -1;
                int scene = press_at(x, z, (int16_t)(r * 512), &msg);
                felder++;
                if (scene) {
                    treffer++;
                    /* NUR Standorte auf einer echten SCA-Bodenzelle mit Band 0 zaehlen als
                     * SPIELBAR: im laufenden Spiel gated der ACTION-Scan ueber das Band
                     * (aot_common.c:1111, FUN_80042cac @0x80042cac) und alle Records dieses
                     * Raums tragen Band 0 (Aot_set pc[4]=0x00). Punkte ohne Zelle (Band -1)
                     * erreicht der Spieler gar nicht erst. */
                    if (re15_collision_floor_band_at(&s_rdt, x, z, 3) == 0) {
                        spielbar++;
                        if (best_r < 0) { best_x = x; best_z = z; best_r = r * 512; }
                    }
                    if (!enter()) return -1;     /* Szene laeuft -> Raum frisch machen */
                    if (alt) stelle_alte_fassung_her();
                }
                if (msg >= 0) { mit_text++; if (msg < 16) msg_hist[msg]++; }
            }
        }
    }
    printf("   Ergebnis: %d von %d Standort/Richtungs-Kombinationen starten sub04\n"
           "   davon auf einer SCA-Bodenzelle mit Band 0 (so wie im Spiel gegatet): %d\n",
           treffer, felder, spielbar);
    if (best_r >= 0)
        printf("   erster begehbarer Treffer: Standort (%ld,%ld), Blickrichtung %ld\n",
               (long)best_x, (long)best_z, (long)best_r);
    printf("   Druecke mit Text statt Szene: %d; msg-id-Verteilung:", mit_text);
    for (int m = 0; m < 16; m++) if (msg_hist[m]) printf(" %d:%dx", m, msg_hist[m]);
    printf("\n");
    return treffer;
}

int main(void)
{
    printf("=== ROOM1150 Irons-Buero: Mittelmodell + Ausloeser ===\n");
    if (!enter()) { printf("SKIP: ROOM1150.RDT fehlt\n"); return 77; }

    printf("-- Objekte (Obj_model_set) --\n");
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        printf("   prop%d id=0x%02X aktiv=%d pos=(%ld,%ld,%ld) rot_y=%d box_h=(%d,%d,%d)\n",
               i, g_scd.props[i].obj_id, g_scd.props[i].active,
               (long)g_scd.props[i].x, (long)g_scd.props[i].y, (long)g_scd.props[i].z,
               (int)g_scd.props[i].rot_y, (int)g_scd.props[i].box_hx,
               (int)g_scd.props[i].box_hy, (int)g_scd.props[i].box_hz);

    printf("-- AOT-Records --\n");
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        if (!a->active) continue;
        printf("   slot%-3d typ=%u ev=%u flags=0x%02X x=[%ld..%ld] z=[%ld..%ld]\n",
               s, a->type, a->event_id, a->sce_flags,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h));
    }

    /* (B2) Kollisions-Gegenprobe: wie viele RVD-Kamerazonen hat der Raum, und wo liegen
     * sie? re15_rdt_apply_zones_as_aots (rdt_common.c:441) belegt sie von OBEN nach unten:
     * slot = RE15_AOT_MAX-1-installed = 63,62,61,... Ein Port-Einbau in Slot 60 loescht
     * also die VIERTE Kamerazone des Raums. */
    {
        re15_aot_init();
        int n = re15_rdt_apply_zones_as_aots(&s_rdt, 16);
        printf("   RVD-Kamerazonen in ROOM1150: %d -> belegte Slots %d..63\n",
               n, RE15_AOT_MAX - n);
        printf("   Slot 60 vor dem Port-Einbau: aktiv=%d typ=%u (3=CAM_SWITCH) "
               "cam %u->%u x=[%ld..%ld] z=[%ld..%ld]\n",
               g_aot.slots[60].active, g_aot.slots[60].type,
               g_aot.slots[60].cam_from_filter, g_aot.slots[60].event_id,
               (long)(g_aot.slots[60].x - g_aot.slots[60].half_w),
               (long)(g_aot.slots[60].x + g_aot.slots[60].half_w),
               (long)(g_aot.slots[60].z - g_aot.slots[60].half_h),
               (long)(g_aot.slots[60].z + g_aot.slots[60].half_h));
        CHECK("Slot 60 ist im unberuehrten Raum eine RVD-Kamerazone (nicht frei)",
              g_aot.slots[60].active && g_aot.slots[60].type == RE15_AOT_TYPE_CAM_SWITCH);
        if (!enter()) return 77;
    }

    CHECK("Objekt 0 parkt beim Betreten auf y=-20324 (main00 @0x0E00)",
          g_scd.prop_count > 0 && g_scd.props[0].obj_id == 0 && g_scd.props[0].y == -20324);
    CHECK("Slot 4 ist der grosse MESSAGE-Record ueber dem Schreibtisch (main00 @0x0DBA)",
          g_aot.slots[4].active && g_aot.slots[4].type == RE15_AOT_TYPE_MESSAGE);

    /* (B3) ANHAENGE-FORM. Obj_model_set pc[5] waehlt die ELTERNMATRIX, mit der der
     * Objekt-Zeichner FUN_8002c18c die lokale Matrix verkettet (LAB_80040914 @0x80040a04
     * `andi v1,a0,0xc0`; 0xC0-Zweig @0x80040a84-9c: 0x800ad0e0 + 148*pc[5] = Objektpool
     * 0x800b3f98 + 148*(pc[5]-0xC0) + 0x48 = die Weltmatrix jenes Objekts). ROOM1150
     * Objekt 1 und 2 tragen pc[5]=0xC0 -> Kinder von Objekt 0. Zensus ueber alle 240 RDTs
     * (672 Obj_model_set): 666x 0x00, 2x 0x80 (ROOM4030/4031), 4x 0xC0 = genau diese vier. */
    printf("-- Anhaenge-Form (Obj_model_set pc[5]) --\n");
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        printf("   prop%d eltern=%d\n", i, (int)g_scd.props[i].parent_obj);
    CHECK("Objekt 1 haengt an Objekt 0 (pc[5]=0xC0)", g_scd.props[1].parent_obj == 0);
    CHECK("Objekt 2 haengt an Objekt 0 (pc[5]=0xC0)", g_scd.props[2].parent_obj == 0);
    CHECK("Objekt 0 und 3 haengen an nichts (pc[5]=0x00)",
          g_scd.props[0].parent_obj == -1 && g_scd.props[3].parent_obj == -1);

    /* (B4) ROOM6020: derselbe Slot-60-Konflikt wie in der alten 1150-Fassung? NUR MESSUNG,
     * keine Behauptung — der Box-Trigger dort ist eine dokumentierte PORT-WAHL ohne
     * autorisierten Record (scd_room_setup.c). */
    {
        char rp[600]; size_t n = 0; re15_rdt_t r6;
        snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, "STAGE6/ROOM6020.RDT");
        uint8_t *raw = slurp(rp, &n);
        if (raw && re15_rdt_parse(raw, n, &r6) >= 0) {
            re15_aot_init();
            int nz = re15_rdt_apply_zones_as_aots(&r6, 16);
            printf("   [ROOM6020] RVD-Kamerazonen: %d -> Slots %d..63; Slot 60 vorher: "
                   "aktiv=%d typ=%u\n", nz, RE15_AOT_MAX - nz,
                   g_aot.slots[60].active, g_aot.slots[60].type);
        }
        if (!enter()) return 77;
    }

    /* (C) Gegenprobe im frischen Raum: die Szene selbst ist heil. */
    s_scene_seen = 0;
    int deckel_auf = 0, deckel_zu_am_ende;
    scd_event_fire(4);
    for (int f = 0; f < 700; f++) {
        tick();
        /* Die beiden Deckelhaelften laufen in LOKALEN Koordinaten von Objekt 0 gegenlaeufig
         * auf Achse 2 auseinander: sub04 @0x0FC6-0x0FDE, For 15x { Work_set(3,1)
         * Speed_set(2,+10) Add_speed ; Work_set(3,2) Speed_set(2,-10) Add_speed } = +/-150,
         * und @0x1078-0x1094 wieder zurueck. */
        if (g_scd.props[1].z == 150 && g_scd.props[2].z == -150) deckel_auf = 1;
    }
    deckel_zu_am_ende = (g_scd.props[1].z == 0 && g_scd.props[2].z == 0);
    printf("   Deckelhaelften: ganz offen gesehen=%d, am Ende wieder zu=%d "
           "(z1=%ld z2=%ld)\n", deckel_auf, deckel_zu_am_ende,
           (long)g_scd.props[1].z, (long)g_scd.props[2].z);
    CHECK("Deckelhaelften oeffnen auf +/-150 (sub04 @0x0FC6)", deckel_auf);
    CHECK("und schliessen wieder (sub04 @0x1078)", deckel_zu_am_ende);
    printf("   nach direktem Feuern: prop0.y=%ld, Arbeitshoehe gesehen=%d\n",
           (long)g_scd.props[0].y, s_scene_seen);
    CHECK("sub04 direkt gefeuert hebt Objekt 0 auf Arbeitshoehe (Pos_set y=-305 @0x0FB4)",
          s_scene_seen);
    CHECK("und parkt es selbst zurueck (Pos_set y=-20224 @0x109E)",
          g_scd.props[0].y == -20224);

    /* (D0) Einzel-Diagnose mitten im Rechteck von Slot 1. */
    if (!enter()) return 77;
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->x = -21050; pl->z = -18350; pl->rot_y = 0;
        msg_reset();
        for (int f = 0; f < 3; f++) tick();
        msg_reset();
        g_aot_action_pressed = 1;
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        printf("   [Diagnose] Standort (-21050,-18350) rot=0 -> fired_event=%u, "
               "player_mode=%u, letterbox=%d, msg_frames=%d, band=%d, "
               "slot1{typ=%u ev=%u flags=0x%02X band=%u} slot4{typ=%u band=%u}\n",
               g_aot.fired_event_id_this_frame, g_scd.player_mode,
               (int)g_scd.letterbox_countdown, g_scd.message_display_frames,
               re15_collision_debug_band(),
               g_aot.slots[1].type, g_aot.slots[1].event_id,
               g_aot.slots[1].sce_flags, g_aot.slots[1].band,
               g_aot.slots[4].type, g_aot.slots[4].band);
        g_aot_action_pressed = 0;
    }

    /* (D-vor) BODEN-BAND am Ausloese-Ort. Der ACTION-Scan gated ueber das Band
     * (aot_common.c:1111, byte-true FUN_80042cac @0x80042cac): ist im laufenden Spiel ein
     * Band gesetzt (pb >= 0), muss es zum Band des Records passen — und ALLE Records dieses
     * Raums tragen Band 0 (Aot_set pc[4] = 0x00). In dieser Sonde laeuft keine Kollision,
     * also ist pb = -1 = ungegatet; hier die Bandlage, die das echte Spiel dort haette. */
    printf("-- Boden-Band (SCA) am Schreibtisch --\n");
    {
        static const int32_t pkt[][2] = {
            { -21050, -18350 }, { -22400, -20000 }, { -21500, -18000 },
            { -22000, -19000 }, { -20900, -17000 },
        };
        for (int k = 0; k < 5; k++)
            printf("   (%ld,%ld): Band %d (start 3)  auf_boden=%d\n",
                   (long)pkt[k][0], (long)pkt[k][1],
                   re15_collision_floor_band_at(&s_rdt, pkt[k][0], pkt[k][1], 3),
                   re15_collision_on_floor(&s_rdt, pkt[k][0], pkt[k][1]));
    }

    /* (D) Raster A = IST-Stand (Slot 1 @0x0D7E armiert). */
    int treffer_neu = raster("A  IST-Stand: Slot 1 (@0x0D7E) armiert", 0);
    CHECK("IST-Stand: der Schreibtisch ist per Aktionsdruck erreichbar", treffer_neu > 0);

    /* (E) Raster B = die Fassung vom 2026-08-30, mit DERSELBEN Messschiene nachgestellt:
     * Slot 1 bleibt inert, stattdessen ein erfundener GENERIC-Record in Slot 60 am
     * Pos_set-Ziel (-20700,-17460) mit Halb-Kante 900. */
    int treffer_alt = raster("B  Fassung 2026-08-30: erfundener Record in Slot 60", 1);
    CHECK("alte Fassung war per Aktionsdruck NICHT erreichbar", treffer_alt == 0);

    printf("BEFUND: erreichbar-per-Aktionsdruck  IST=%s (%d Treffer)  ALT=%s (%d Treffer)\n",
           treffer_neu ? "JA" : "NEIN", treffer_neu,
           treffer_alt ? "JA" : "NEIN", treffer_alt);
    if (g_fail) { printf("FEHLGESCHLAGEN\n"); return 1; }
    printf("OK\n");
    return 0;
}
