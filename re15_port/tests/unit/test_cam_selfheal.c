/* test_cam_selfheal.c — PIN fuer den SELBSTHEILENDEN Kamera-Apply.
 *
 * NUTZER-BEFUND (2026-08-21): "Wenn ich nach dem Generator-Raetsel direkt weglaufe,
 * wechselt die Kamera / das Bild nicht."  Raetselraum = ROOM11F0 (Nutzer nannte ROOM10F0 —
 * beide werden hier gemessen).
 *
 * ============================ SOLLSEITE (PSX.EXE, selbst disassembliert) ===================
 * Das Original fuehrt DREI Groessen, nicht zwei:
 *
 *   DAT_800afbb5 (u8)   = ANGEFORDERTER Cut.  Genau 3 Schreiber (vollstaendige XREF-Liste):
 *        FUN_800142f4 @0x80014300 `sb a0,-0x44b(at)`   — der Kamera-Setzer. Seine 7 Aufrufer:
 *              FUN_80014230 @0x800142ac  (RVD-ZONEN-SCAN)
 *              @0x80040300 (Cut_chg)  @0x80040368 (Cut_old)  @0x800404c8 (Cut_replace)
 *              @0x80039944  @0x800466e8  @0x8004c688
 *        FUN_8001d600 @0x8001d818 `sb zero`  (Boot -> Cut 0)
 *        FUN_8001d600 @0x8001d940 `sb v1`    (Tuer-Payload Byte 10, gelesen @0x8001d930)
 *      -> Port: g_scd.cam_id
 *
 *   DAT_800b0fe4 (s16)  = work_vars[0x0A] = ANGEZEIGTER (zuletzt angewandter) Cut.
 *      Basis work_vars = 0x800b0fd0 (work_var[0]@0x800b0fd0, [1]@0x800b0fd2)
 *      -> 0x800b0fe4 = 0x800b0fd0 + 0x14 = Index 0x0A.
 *      Genau 5 Schreiber: FUN_80021bbc @0x80021bfc (DER APPLY), FUN_8001d600 @0x8001d820 und
 *      @0x8001d948 (Raum-Warp — direkt NEBEN den DAT_800afbb5-Stores, beide aus demselben
 *      Register), Cut_chg @0x800402fc, Cut_old @0x80040364.
 *      Der RVD-Zonen-Scan ist KEINER davon.
 *      -> Port: g_scd.work_vars[0x0A]
 *
 *   DAT_800b5457 (u8)   = Dirty-Flag  -> Port: g_scd.cam_change_pending
 *
 * Per-Bild-Present FUN_8002137c:
 *   @0x800214e0 `lw v0,DAT_800aca3c` / @0x800214e8 `andi v0,v0,0x80`
 *   @0x800214ec `bne v0,zero,LAB_80021518`     Bit 0x80 -> Selbstheilung uebersprungen
 *   @0x800214f8 `lh  v1,DAT_800b0fe4`          ANGEZEIGT
 *   @0x80021500 `lbu v0,DAT_800afbb5`          ANGEFORDERT
 *   @0x80021508 `beq v1,v0,LAB_80021518`       gleich -> nichts
 *   @0x80021514 `sb  1,DAT_800b5457`           UNGLEICH -> JEDES BILD neu dirty
 *   @0x80021538 `beq v0,zero,LAB_80021568`     nicht dirty -> kein Apply
 *   @0x80021558 `jal FUN_80021bbc`             APPLY
 *   @0x80021618 `sb  zero,DAT_800b5457`        Dirty am Bildende geloescht
 * Apply-Kopf FUN_80021bbc:
 *   @0x80021be0 `lbu v1,DAT_800afbb5` / @0x80021be8 `lhu v0,DAT_800b0fe4`
 *   @0x80021bf4 `sh  v0,DAT_800b0fe8`          work_vars[0x0C] = alter ANGEZEIGTER Cut
 *   @0x80021bfc `sh  v1,DAT_800b0fe4`          work_vars[0x0A] = ANGEFORDERTER Cut
 *
 * WARUM DAS DER GANZE MECHANISMUS IST: der RVD-Zonen-Scan bewaffnet NICHTS. Er schreibt ueber
 * FUN_800142f4 nur DAT_800afbb5. Der Kamerawechsel beim Durchlaufen eines Raums ENTSTEHT
 * ausschliesslich aus diesem Per-Bild-Vergleich. Der Port hatte stattdessen ein EINMALIGES
 * Flag (`if (g_scd.cam_change_pending) {...; = 0; }`, platform/pc/main.c): ging es verloren
 * oder blieb der Apply in dem Bild aus, stand das Bild DAUERHAFT.
 *
 * ============================ WAS DIESE SONDE MISST ========================================
 *   M1  BELEG: die Raum-Bytes von ROOM11F0 (Cut_chg(0x0A) @0x15C0, Cut_old+Cut_auto @0x16F0)
 *   M2  ECHTER ABLAUF ROOM11F0: Tuer-Eintritt -> Raetsel-Subs durch die ECHTE VM -> weglaufen
 *       ueber die ECHTE RVD-Zonenkette. Der ANGEZEIGTE Cut MUSS in jedem Schritt nachziehen.
 *   M3  SELBSTHEILUNG (Fehler-Injektion): das Dirty-Flag wird nach der Anforderung geloescht
 *       (genau der Verlust, den der Port frueher nicht ueberleben konnte). Der naechste
 *       Present MUSS den Wechsel trotzdem ausfuehren — @0x800214f4 leitet ihn neu her.
 *   M4  NEGATIV-TEST: stimmen ANGEFORDERT und ANGEZEIGT ueberein, darf KEIN Apply laufen
 *       (sonst wuerde work_vars[0x0C] in jedem Bild ueberschrieben und jeder Cut-Wechsel
 *       BG/Licht/sprite.pri unnoetig neu laden).
 *   M5  TUER-STEMPEL: nach dem Raum-Eintritt mit Cut E muessen BEIDE Zellen E tragen
 *       (@0x8001d940 + @0x8001d948). Sonst schnappt der Vergleich sofort auf Cut 0 zurueck.
 *   M6  NACHBARRAUM ROOM10F0 (die vom Nutzer genannte ID) — dieselbe Kette.
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

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* ---------------------------------------------------------------------------------------
 * Das BILD-MODELL — exakt die Reihenfolge der beiden Hauptschleifen:
 *   scd_vm_tick()                        (platform/pc/main.c:3617)
 *   re15_aot_scan(x, z, ANGEZEIGTER Cut) (game_step_common.c `c->active_cut`, gefuellt aus
 *                                         platform/pc/main.c:4348 `gctx.active_cut =
 *                                         active_cut_idx`) — byte-true der cam_from-Filter
 *                                         FUN_80014230 @0x8001423c `lbu v0,DAT_800afbb5`
 *   re15_cam_present_tick()              (der Apply; platform/pc/main.c Cut-Block)
 * `s_shown` ist die Port-Zelle `active_cut_idx`/`s_last_cut_idx` bzw. `cam_active_cut`.
 * ------------------------------------------------------------------------------------- */
static int s_shown = 0;
static int s_applies = 0;

static void frame(re15_actor_t *pl)
{
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, (uint8_t)s_shown);
    if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    if (re15_cam_present_tick()) { s_shown = (int)g_scd.cam_id; s_applies++; }
}

static void enter_room(re15_rdt_t *rdt, unsigned room_id, uint8_t entry_cut,
                       int32_t px, int32_t pz)
{
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    g_current_room_id = room_id;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0;
    scd_room_reenter(rdt, px, pz, entry_cut);
    s_shown = (int)entry_cut;          /* room_common.c Schritt 5 */
    g_scd.cut_auto_enabled = 1;        /* room_common.c Schritt 12 (Tuer-Eintritt = Gameplay) */
    for (int f = 0; f < 30; f++) frame(pl);
}

/* Die ZONEN-KETTE ablaufen: immer die erste RVD-Zone mit cam_from == ANGEZEIGTER Cut suchen,
 * in ihre Mitte stellen, drei Bilder ticken. Das ist exakt die Zustandsmaschine, die der
 * Nutzer beim Weglaufen durchlaeuft. Rueckgabe = Zahl der Umschaltungen; `*out_stuck` = 1,
 * wenn der ANGEZEIGTE Cut dem ANGEFORDERTEN irgendwann NICHT nachgezogen ist. */
static int walk_chain(re15_actor_t *pl, const char *tag, int *out_stuck)
{
    int seen[64], nseen = 0, sw = 0;
    *out_stuck = 0;
    seen[nseen++] = s_shown;
    for (int step = 0; step < 12; step++) {
        int pick = -1;
        for (int i = RE15_AOT_MAX - 1; i >= 0; i--) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active || a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;
            if (a->cam_from_filter != (uint8_t)s_shown) continue;
            int fresh = 1;
            for (int s = 0; s < nseen; s++) if (seen[s] == (int)a->event_id) fresh = 0;
            if (fresh) { pick = i; break; }
        }
        if (pick < 0) break;
        int before = s_shown;
        pl->x = g_aot.slots[pick].x; pl->z = g_aot.slots[pick].z;
        for (int f = 0; f < 3; f++) frame(pl);
        if ((int)g_scd.cam_id != s_shown) *out_stuck = 1;   /* Anforderung nicht angezeigt! */
        if (s_shown != before) { sw++; if (nseen < 64) seen[nseen++] = s_shown; }
        else break;
    }
    printf("[%s] %d Umschaltungen, %d Cuts, Endstand angezeigt=%d angefordert=%u "
           "wv0A=%d auto=%d stuck=%d\n", tag, sw, nseen, s_shown, (unsigned)g_scd.cam_id,
           (int)g_scd.work_vars[0x0A], (int)g_scd.cut_auto_enabled, *out_stuck);
    return sw;
}

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", base);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* ---- M1 BELEG aus den Rohbytes ---------------------------------------------------- */
    CHECK(raw[0x15C0] == 0x29 && raw[0x15C1] == 0x0A,
          "ROOM11F0 @0x15C0 ist nicht Cut_chg(0x0A) (%02x %02x)", raw[0x15C0], raw[0x15C1]);
    CHECK(raw[0x16F0] == 0x2A && raw[0x16F2] == 0x3C && raw[0x16F3] == 0x01,
          "ROOM11F0 @0x16F0 ist nicht Cut_old+Cut_auto(1) (%02x %02x %02x %02x)",
          raw[0x16F0], raw[0x16F1], raw[0x16F2], raw[0x16F3]);

    /* ---- M5 TUER-STEMPEL: beide Zellen tragen den Eintritts-Cut ------------------------
     * Der Eintritts-Cut wird aus einer ECHTEN Tuer des Raums genommen, nicht erfunden:
     * die erste DOOR-AOT von ROOM11F0 und ihr Ziel-Cut. */
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    g_current_room_id = 0x11F0; g_room_change.pending = 0;
    {
        re15_actor_t *pl0 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl0->active = 1; pl0->hp = 100;
        scd_room_reenter(&rdt, -19554, 22684, 0);
    }
    int door_cut = -1;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_DOOR) {
            door_cut = (int)g_aot.door_params[i].target_cut; break;
        }
    printf("[M5] erster DOOR-AOT von ROOM11F0: Ziel-Cut %d\n", door_cut);
    CHECK(door_cut >= 0, "ROOM11F0 hat keine DOOR-AOT — Eintritts-Cut nicht belegbar");
    {
        uint8_t ec = (uint8_t)(door_cut > 0 ? door_cut : 3);   /* != 0, damit der Test greift */
        scd_room_reenter(&rdt, -19554, 22684, ec);
        printf("[M5] nach Eintritt mit Cut %u: cam_id=%u work_vars[0x0A]=%d\n",
               ec, (unsigned)g_scd.cam_id, (int)g_scd.work_vars[0x0A]);
        CHECK(g_scd.cam_id == ec,
              "Tuer-Eintritt stempelt DAT_800afbb5 nicht (@0x8001d940): cam_id=%u != %u",
              (unsigned)g_scd.cam_id, ec);
        CHECK((int)g_scd.work_vars[0x0A] == (int)ec,
              "Tuer-Eintritt stempelt work_vars[0x0A] nicht (@0x8001d948): %d != %u",
              (int)g_scd.work_vars[0x0A], ec);
        /* ...und der Present darf JETZT nichts tun (beide gleich) — sonst schnappt die
         * Kamera nach jeder Tuer auf Cut 0 zurueck. */
        s_shown = (int)ec;
        int applied = re15_cam_present_tick();
        CHECK(applied == 0,
              "nach dem Tuer-Eintritt feuert der Present-Apply obwohl beide Zellen gleich sind");
    }

    /* ---- M2 ECHTER ABLAUF ------------------------------------------------------------- */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    enter_room(&rdt, 0x11F0, 0, -19554, 22684);
    int n_cam = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_CAM_SWITCH) n_cam++;
    printf("[M2] ROOM11F0: %d RVD-Zonen, angezeigt=%d angefordert=%u auto=%d\n",
           n_cam, s_shown, (unsigned)g_scd.cam_id, (int)g_scd.cut_auto_enabled);
    CHECK(n_cam > 0, "ROOM11F0 hat keine RVD-Zonen — Messung sinnlos");

    int stuck = 0;
    int sw_pre = walk_chain(pl, "M2a POSITIV-KONTROLLE (vor dem Raetsel)", &stuck);
    CHECK(sw_pre > 0, "schon ohne Raetsel schaltet die Kamera nie um");
    CHECK(!stuck, "vor dem Raetsel: angezeigter Cut zieht dem angeforderten nicht nach");

    /* Zurueck an den Start, dann die ECHTEN Raetsel-Opcodes durch die ECHTE VM fahren:
     * @0x15C0 = Cut_chg(0x0A) (Kamera auf das Panel) und @0x16F0 = Cut_old + Cut_auto(1)
     * (Ausstieg). Nichts wird von Hand nachgebildet — die Bytes des Raums laufen. */
    pl->x = -19554; pl->z = 22684;
    for (int f = 0; f < 3; f++) frame(pl);

    scd_thread_start(3, raw + 0x15C0);
    for (int f = 0; f < 60; f++) frame(pl);
    printf("[M2b] nach Cut_chg(0x0A): angezeigt=%d angefordert=%u wv0A=%d wv0C=%d auto=%d\n",
           s_shown, (unsigned)g_scd.cam_id, (int)g_scd.work_vars[0x0A],
           (int)g_scd.work_vars[0x0C], (int)g_scd.cut_auto_enabled);
    CHECK(g_scd.cam_id == 0x0A, "Cut_chg(0x0A) hat den Cut nicht angefordert (%u)",
          (unsigned)g_scd.cam_id);
    CHECK(s_shown == 0x0A, "Cut_chg(0x0A) wurde nicht ANGEZEIGT (angezeigt=%d)", s_shown);
    CHECK(g_scd.cut_auto_enabled == 0,
          "Cut_chg hat den Auto-Scan nicht abgeschaltet (@0x800402d4 `ori aca3c,0x100`)");

    scd_thread_start(4, raw + 0x16F0);
    for (int f = 0; f < 60; f++) frame(pl);
    printf("[M2c] nach Cut_old+Cut_auto(1): angezeigt=%d angefordert=%u wv0A=%d auto=%d\n",
           s_shown, (unsigned)g_scd.cam_id, (int)g_scd.work_vars[0x0A],
           (int)g_scd.cut_auto_enabled);
    CHECK(g_scd.cut_auto_enabled == 1,
          "Cut_auto(1) hat den Auto-Scan nicht wieder scharf gemacht (@0x80040378-84)");
    CHECK(s_shown == (int)g_scd.cam_id,
          "nach dem Raetsel: angezeigt=%d != angefordert=%u", s_shown, (unsigned)g_scd.cam_id);

    int sw_post = walk_chain(pl, "M2d NACH DEM RAETSEL (weglaufen)", &stuck);
    CHECK(sw_post > 0, "nach dem Raetsel wechselt die Kamera NIE mehr (Nutzer-Befund)");
    CHECK(!stuck, "nach dem Raetsel: angezeigter Cut zieht dem angeforderten nicht nach");
    CHECK(sw_post == sw_pre, "nach dem Raetsel nur %d statt %d Umschaltungen", sw_post, sw_pre);

    /* ---- M3 SELBSTHEILUNG (Fehler-Injektion) -------------------------------------------
     * Genau der Verlust, an dem der Port frueher haengenblieb: eine Anforderung steht an,
     * aber das Dirty-Flag ist weg (frueher: von einem anderen Verbraucher konsumiert, oder
     * der Apply lief in dem Bild nicht). Das Original leitet es im NAECHSTEN Bild aus dem
     * Vergleich @0x800214f4 NEU her — der Port muss das jetzt auch. */
    {
        int before_shown = s_shown;
        int target = -1;
        for (int i = RE15_AOT_MAX - 1; i >= 0; i--) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (a->active && a->type == RE15_AOT_TYPE_CAM_SWITCH &&
                a->cam_from_filter == (uint8_t)s_shown && (int)a->event_id != s_shown) {
                target = i; break;
            }
        }
        CHECK(target >= 0, "M3: keine RVD-Zone ab dem aktuellen Cut %d gefunden", s_shown);
        if (target >= 0) {
            /* EIN Bild ohne Present: Scan fordert an, danach geht das Flag verloren. */
            pl->x = g_aot.slots[target].x; pl->z = g_aot.slots[target].z;
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, (uint8_t)s_shown);
            unsigned requested = g_scd.cam_id;
            CHECK((int)requested != before_shown,
                  "M3: der Scan hat gar nichts angefordert (cam_id=%u, angezeigt=%d)",
                  requested, before_shown);
            g_scd.cam_change_pending = 0;                 /* <-- DER VERLUST */
            CHECK(s_shown == before_shown, "M3: Vorbedingung verletzt");
            /* Naechstes Bild, ganz normal: der Vergleich MUSS den Wechsel retten. */
            frame(pl);
            printf("[M3] Flag-Verlust bei Anforderung %u: angezeigt %d -> %d\n",
                   requested, before_shown, s_shown);
            CHECK(s_shown == (int)requested,
                  "M3 SELBSTHEILUNG FEHLT: Flag verloren -> angezeigt bleibt %d statt %u "
                  "(@0x800214f4-0x80021514 leitet das Dirty-Bit jedes Bild NEU her)",
                  s_shown, requested);
        }
    }

    /* ---- M4 NEGATIV-TEST: kein Apply ohne Unterschied ---------------------------------- */
    {
        g_scd.cam_change_pending = 0;
        g_scd.cam_id          = (uint8_t)s_shown;
        g_scd.work_vars[0x0A] = (int16_t)s_shown;
        g_scd.work_vars[0x0C] = 0x7F;                  /* Marker */
        int fired = 0;
        for (int f = 0; f < 10; f++) fired += re15_cam_present_tick();
        printf("[M4] 10 Bilder ohne Unterschied: %d Applies, wv0C=%d\n",
               fired, (int)g_scd.work_vars[0x0C]);
        CHECK(fired == 0,
              "M4 NEGATIV-TEST: %d Applies obwohl angefordert == angezeigt "
              "(@0x80021508 `beq` muss abbrechen)", fired);
        CHECK((int)g_scd.work_vars[0x0C] == 0x7F,
              "M4: work_vars[0x0C] wurde ohne Cut-Wechsel ueberschrieben (%d)",
              (int)g_scd.work_vars[0x0C]);
    }

    free(raw);

    /* ---- M6 NACHBARRAUM ROOM10F0 (die vom Nutzer genannte ID) -------------------------- */
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM10F0.RDT", base);
    size_t sz2 = 0; uint8_t *raw2 = read_file(rp, &sz2);
    if (raw2) {
        re15_rdt_t rdt2;
        if (re15_rdt_parse(raw2, sz2, &rdt2) == 0) {
            enter_room(&rdt2, 0x10F0, 0, 0, 0);
            re15_actor_t *p2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            int st2 = 0;
            int sw2 = walk_chain(p2, "M6 ROOM10F0", &st2);
            CHECK(sw2 > 0, "ROOM10F0: die Kamera wechselt nie");
            CHECK(!st2, "ROOM10F0: angezeigter Cut zieht dem angeforderten nicht nach");
        }
        free(raw2);
    } else {
        printf("[M6] SKIP: ROOM10F0.RDT fehlt\n");
    }

    printf(g_fail ? "test_cam_selfheal: ABWEICHUNG\n" : "test_cam_selfheal: OK\n");
    return g_fail;
}
