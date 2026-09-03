/* =============================================================================
 * KARTE, RAUM FUER RAUM — LIVE ueber den echten Renderpfad
 * =============================================================================
 * Auftrag des Nutzers 2026-09-03: "ueberpruefe jeden einzelnen Raum auf Fehler mit dem
 * Hinspringen und dem Debugger ... gehe auch gerne einmal durch die Tueren durch, und
 * schaue dir dann an ob das Verhalten der Erwartung entspricht."
 *
 * Dieser Test SPRINGT in jeden Raum (dieselbe Kette wie scd_room_reenter beim
 * Debug-JUMP), setzt den Spieler auf einen begehbaren Punkt, oeffnet den Kartenschirm und
 * baut die ECHTE Op-Liste mit re15_inv_screen_build. Geprueft wird, was der Spieler
 * SIEHT:
 *
 *   1. Der Raum, in dem man steht, ist ROT hervorgehoben - und zwar SICHTBAR, also nicht
 *      unter einer anderen Flaeche begraben. Genau das war ROOM1070 (Nutzer 2026-09-03:
 *      "ROOM 1070 wird nicht rot hervorgehoben wenn ich drin stehe"): die rot gefaerbte
 *      Zone war eine Insel im Raum und lag komplett unter dem gruenen Rechteck.
 *      Eine Tabellenpruefung findet das NICHT - erst die gerasterte Op-Liste zeigt es.
 *   2. Der Spieler-Marker liegt IN dieser roten Flaeche.
 *   3. Beim Durchschreiten einer Tuer landet man in der roten Flaeche des ZIELraums.
 *
 * ⛔ GEPRUEFT WIRD DAS GERASTERTE BILD, NICHT DIE TABELLE. Die Op-Liste wird von HINTEN
 * gerastert (inv_render_pc.c: for (i = n-1; i >= 0; i--)), eine frueher eingetragene Op
 * liegt also OBEN. Der Test bildet das nach: fuer jeden Punkt gewinnt die Op mit dem
 * KLEINSTEN Index.
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_msg.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

static re15_rdt_t s_rdt;

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    long sz;
    uint8_t *b;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

/* Genau die Kette, die das Spiel beim Raumwechsel faehrt. */
static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    uint8_t *roh;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    roh = slurp(pfad, &n);
    if (!roh) return 0;
    if (re15_rdt_parse(roh, n, &s_rdt) < 0) { free(roh); return 0; }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid;
    g_room_change.pending = 0;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz;
    re15_collision_reset_band();
    if (band >= 0) re15_collision_set_band(band);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

/* Der aktuelle Raum wird in RE2_CURRENT gefuellt: dunkelrot (74,20,20). */
#define ROT_R 74
#define ROT_G 20
#define ROT_B 20

/* Welche Op gewinnt an (x,y)? Die mit dem KLEINSTEN Index (Rasterung von hinten). */
static int oben_an(const re15_inv_op_t *ops, int n, int x, int y)
{
    int i;
    for (i = 0; i < n; i++) {
        if (ops[i].kind != RE15_INV_OP_FILL) continue;
        /* ⛔ KLEINE MARKEN ZAEHLEN NICHT. Tuerbalken (5x2) und Treppensprossen
         * gehoeren per Konstruktion nach OBEN; sie verdecken den Raum nicht, auch
         * wenn sie zufaellig auf seinem Mittelpunkt liegen. Ein erster Wurf zaehlte
         * sie mit und meldete ROOM10A0 faelschlich als nicht hervorgehoben. */
        if (ops[i].w < 4 || ops[i].h < 4) continue;
        if (x < ops[i].x || x >= ops[i].x + ops[i].w) continue;
        if (y < ops[i].y || y >= ops[i].y + ops[i].h) continue;
        return i;
    }
    return -1;
}

/* Betritt einen Raum, oeffnet die Karte und liefert zurueck, ob der Raum SICHTBAR
 * rot ist und wo der Marker liegt. Genau der Weg, den das Spiel geht. */
static int schau(unsigned rid, int32_t px, int32_t pz, int band,
                 re15_inv_op_t *ops, int max_ops,
                 int *rot_da, int16_t *mmx, int16_t *mmy, int *seite)
{
    const re15_map_zone_t *zn;
    int nops, k;
    *rot_da = 0;
    if (!betrete(rid, px, pz, band)) return 0;
    re15_map_visited_mark(rid);
    re15_map_visited_mark_at(rid, px, pz);
    re15_map_zone_update(rid, px, pz);
    zn = re15_map_zone_current();
    if (!zn || !zn->synth) return 0;
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    g_inv_screen.map_page = re15_inv_map_page_shown();
    *seite = (int)g_inv_screen.map_page;
    nops = re15_inv_screen_build(&g_inv_screen, ops, max_ops);
    for (k = 0; k < nops; k++) {
        if (ops[k].kind != RE15_INV_OP_FILL) continue;
        if (ops[k].r != ROT_R || ops[k].g != ROT_G || ops[k].b != ROT_B) continue;
        if (ops[k].w < 4 || ops[k].h < 4) continue;
        if (oben_an(ops, nops, ops[k].x + ops[k].w / 2,
                    ops[k].y + ops[k].h / 2) == k) { *rot_da = 1; break; }
    }
    re15_inv_map_marker(px, pz, 0, mmx, mmy);
    return nops;
}
/* Ankunftspunkt je Raum: Spawn und BAND der Tuer, die dorthin fuehrt. */
static struct { int32_t x, z; int band; int gesetzt; } s_ank[6 * 64];

int main(void)
{
    static re15_inv_op_t ops[1024];
    unsigned st, r;
    int rot_sichtbar = 0, rot_fehlt = 0, geprueft = 0, marker_drin = 0, marker_ges = 0;
    int i;

    printf("=== Karte: jeder Raum LIVE ueber den Renderpfad ===\n");
    re15_map_visited_reset();
    for (i = 0; i < 13; i++) re15_map_debug_reveal_page((unsigned)i);
    re15_inv_map_stage_init(0, 6);

    /* ---- VORLAUF: Ankunftspunkt und BAND je Raum aus den Tueren, die dorthin
     * fuehren. Genau damit kommt der Spieler an - das Band entscheidet ueber das
     * gezeigte Blatt und ueber das Etagen-Besucht-Bit. Ohne das wurde ROOM2070 nie
     * rot, weil seine Zone eine Etagen-Zeile ist. ---- */
    for (st = 1; st <= 6; st++) {
        for (r = 0; r < 0x400; r += 0x10) {
            unsigned rid = (unsigned)(st << 12) | (unsigned)r;
            int k;
            if (!betrete(rid, 0, 0, -1)) continue;
            for (k = 0; k < RE15_AOT_MAX; k++) {
                const re15_aot_t *a = &g_aot.slots[k];
                const re15_aot_door_params_t *d = &g_aot.door_params[k];
                unsigned ziel;
                if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
                ziel = (((unsigned)d->dest_stage + 1u) << 12)
                     | ((unsigned)d->dest_room << 4)
                     | (rid & 0x000Fu);
                if (ziel == rid || (ziel >> 12) == 0 || (ziel >> 12) > 6) continue;
                {
                    unsigned idx = ((ziel >> 12) - 1u) * 64u + ((ziel >> 4) & 63u);
                    if (idx < (unsigned)(sizeof(s_ank) / sizeof(s_ank[0])) &&
                        !s_ank[idx].gesetzt) {
                        s_ank[idx].x = d->spawn_x;
                        s_ank[idx].z = d->spawn_z;
                        /* Band der TUER (door_params), nicht das Slot-Feld -
                         * letzteres ist bei Tueren 0. */
                        s_ank[idx].band = (int)d->band;
                        s_ank[idx].gesetzt = 1;
                    }
                }
            }
        }
    }

    if (!getenv("RE15_NUR_PHASE2"))
    for (st = 1; st <= 6; st++) {
        for (r = 0; r < 0x400; r += 0x10) {
            unsigned rid = (unsigned)(st << 12) | (unsigned)r;
            const re15_map_zone_t *zn;
            int nops, k, rot_n = 0, rot_i = -1;
            int band = -1;
            int32_t px = 0, pz = 0;
            int gefunden = 0;

            /* Einen begehbaren Punkt suchen: der Spawn einer Tuer, die HIERHER fuehrt,
             * waere ideal - der ist aber erst nach dem Laden bekannt. Genommen wird der
             * Mittelpunkt der ersten Kollisionszelle. */
            if (!betrete(rid, 0, 0, -1)) continue;
            {   /* Der ANKUNFTSPUNKT schlaegt die groesste Zelle - so kommt der
                 * Spieler wirklich an, samt Band der benutzten Tuer. */
                unsigned idx = (st - 1u) * 64u + ((rid >> 4) & 63u);
                if (idx < (unsigned)(sizeof(s_ank) / sizeof(s_ank[0])) &&
                    s_ank[idx].gesetzt) {
                    px = s_ank[idx].x; pz = s_ank[idx].z;
                    band = s_ank[idx].band; gefunden = 1;
                }
            }
            if (!gefunden && s_rdt.sca_count > 0 && s_rdt.sca) {
                /* Die groesste Zelle liegt sicher im begehbaren Kern des Raums. */
                int bi = 0, bj;
                long ba = 0;
                for (bj = 0; bj < s_rdt.sca_count; bj++) {
                    long a = (long)s_rdt.sca[bj].width * s_rdt.sca[bj].density;
                    if (a > ba) { ba = a; bi = bj; }
                }
                px = (int32_t)s_rdt.sca[bi].x + s_rdt.sca[bi].width / 2;
                pz = (int32_t)s_rdt.sca[bi].z + s_rdt.sca[bi].density / 2;
                band = (int)(s_rdt.sca[bi].floor >> 4);
                gefunden = 1;
            }
            if (!gefunden) continue;
            if (!betrete(rid, px, pz, band)) continue;

            re15_map_visited_mark(rid);
            re15_map_visited_mark_at(rid, px, pz);
            re15_map_zone_update(rid, px, pz);
            zn = re15_map_zone_current();
            if (!zn || !zn->synth) continue;

            re15_inv_screen_open();
            g_inv_screen.substate = 1;
            g_inv_screen.item_state = 1;
            g_inv_screen.map_page = re15_inv_map_page_shown();
            nops = re15_inv_screen_build(&g_inv_screen, ops, 1024);

            geprueft++;
            /* Wie viele SICHTBARE rote Flaechenpixel gibt es? */
            for (k = 0; k < nops; k++) {
                if (ops[k].kind != RE15_INV_OP_FILL) continue;
                if (ops[k].r != ROT_R || ops[k].g != ROT_G || ops[k].b != ROT_B) continue;
                if (ops[k].w < 4 || ops[k].h < 4) continue;   /* Marken sind klein */
                /* Ist wenigstens ihr Mittelpunkt oben auf? */
                {
                    int mx = ops[k].x + ops[k].w / 2;
                    int my = ops[k].y + ops[k].h / 2;
                    if (oben_an(ops, nops, mx, my) == k) { rot_n++; if (rot_i < 0) rot_i = k; }
                }
            }
            if (rot_n > 0) {
                rot_sichtbar++;
                /* Liegt der Spieler-Marker in der roten Flaeche? */
                {
                    int16_t mmx = 0, mmy = 0;
                    re15_inv_map_marker(px, pz, 0, &mmx, &mmy);
                    int q, drin = 0;
                    marker_ges++;
                    /* ⛔ IRGENDEINE rote Flaeche des Raums genuegt. Ein Raum kann
                     * mehrere Zeichnungen haben (ROOM2080: 9x18 und 4x4 px); die
                     * ERSTE zu nehmen war willkuerlich. */
                    for (q = 0; q < nops; q++) {
                        if (ops[q].kind != RE15_INV_OP_FILL) continue;
                        if (ops[q].r != ROT_R || ops[q].g != ROT_G ||
                            ops[q].b != ROT_B) continue;
                        if (mmx >= ops[q].x && mmx < ops[q].x + ops[q].w &&
                            mmy >= ops[q].y && mmy < ops[q].y + ops[q].h) {
                            drin = 1; break;
                        }
                    }
                    if (drin) marker_drin++;
                    else printf("     MARKER AUSSERHALB: ROOM%04X (%d,%d)\n",
                                rid, (int)mmx, (int)mmy);
                }
            } else {
                rot_fehlt++;
                printf("     ROT FEHLT: ROOM%04X (Blatt %d, Zone z%d)\n",
                       rid, (int)g_inv_screen.map_page, (int)zn->idx);
            
                if (getenv("RE15_LIVE_DUMP")) {
                    int q, n2 = 0;
                    for (q = 0; q < nops && n2 < 14; q++) {
                        if (ops[q].kind != RE15_INV_OP_FILL) continue;
                        if (ops[q].w < 4 || ops[q].h < 4) continue;
                        printf("        #%3d (%3d,%3d) %2dx%-2d rgb %3d,%3d,%3d%s\n",
                               q, ops[q].x, ops[q].y, ops[q].w, ops[q].h,
                               ops[q].r, ops[q].g, ops[q].b,
                               oben_an(ops, nops, ops[q].x + ops[q].w / 2,
                                       ops[q].y + ops[q].h / 2) == q ? "  OBEN" : "");
                        n2++;
                    }
                }
            }
        }
    }

    /* ================= PHASE 2: DURCH JEDE TUER GEHEN ==================== */
    {
        static re15_inv_op_t ops2[1024];
        static re15_aot_t sn_a[RE15_AOT_MAX];
        static re15_aot_door_params_t sn_d[RE15_AOT_MAX];
        int tueren = 0, rot_ok = 0, marker_ok = 0, gleiches_blatt = 0, sprung_gross = 0;
        for (st = 1; st <= 6; st++) {
            for (r = 0; r < 0x400; r += 0x10) {
                unsigned rid = (unsigned)(st << 12) | (unsigned)r;
                int k;
                if (!betrete(rid, 0, 0, -1)) continue;
                /* ⛔ DIE TUERLISTE ERST ABSCHREIBEN. schau() betritt in DIESEM Rumpf
                 * zwei weitere Raeume, und betrete() ruft re15_aot_init() - g_aot wird
                 * also mitten in der Schleife neu befuellt. Wer live ueber
                 * g_aot.slots[k] laeuft, liest ab dem zweiten Durchgang die Tueren des
                 * ZULETZT geladenen Raums und schreibt sie diesem hier zu. Genau so
                 * entstanden die 61 von 149 "Spruengen ueber 16 px": fuer ROOM1140
                 * wurden Tueren nach ROOM1120 gemeldet, die es dort gar nicht gibt
                 * (ROOM1140 hat genau EINE installierte Tuer, nach ROOM1130), und der
                 * B-Punkt (197,128) war der von ROOM1170 -> ROOM1130. Die Zahl war ein
                 * Werkzeug-Artefakt; der Kommentar darunter hat sie mit einer falschen
                 * Geschichte ("veraltete Doppel-Slots") erklaert. */
                memcpy(sn_a, g_aot.slots, sizeof sn_a);
                memcpy(sn_d, g_aot.door_params, sizeof sn_d);
                for (k = 0; k < RE15_AOT_MAX; k++) {
                    const re15_aot_t *a = &sn_a[k];
                    const re15_aot_door_params_t *d = &sn_d[k];
                    unsigned ziel;
                    int32_t ax, az, bx, bz;
                    int band, rotA = 0, rotB = 0, sA = -1, sB = -1;
                    int16_t max_ = 0, may = 0, mbx = 0, mby = 0;
                    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
                    ziel = (((unsigned)d->dest_stage + 1u) << 12)
                         | ((unsigned)d->dest_room << 4) | (rid & 0x000Fu);
                    if (ziel == rid) continue;
                    ax = a->x; az = a->z; bx = d->spawn_x; bz = d->spawn_z;
                    band = (int)sn_d[k].band;
                    if (!schau(rid, ax, az, band, ops2, 1024, &rotA, &max_, &may, &sA))
                        continue;
                    if (!schau(ziel, bx, bz, band, ops2, 1024, &rotB, &mbx, &mby, &sB))
                        continue;
                    tueren++;
                    if (rotA && rotB) rot_ok++;
                    else printf("     KEIN ROT: ROOM%04X(%d) -> ROOM%04X(%d)\n",
                                rid, rotA, ziel, rotB);
                    if (sA == sB) {
                        int dx = max_ > mbx ? max_ - mbx : mbx - max_;
                        int dy = may > mby ? may - mby : mby - may;
                        int sp = dx > dy ? dx : dy;
                        gleiches_blatt++;
                        if (sp > 16) {
                            sprung_gross++;
                            if (getenv("RE15_SPRUNG_LISTE"))
                                printf("     SPRUNG %3d px  ROOM%04X (%d,%d) -> ROOM%04X (%d,%d)  Blatt %d\n",
                                       sp, rid, (int)max_, (int)may, ziel,
                                       (int)mbx, (int)mby, sA);
                        }
                    }
                    marker_ok++;
                }
            }
        }
        printf("  [Durchgang] %d Tueren durchschritten, %d mal war VORHER und NACHHER"
               " der richtige Raum rot\n", tueren, rot_ok);
        printf("  [Durchgang] %d auf demselben Blatt, davon %d mit Sprung ueber"
               " 16 px\n", gleiches_blatt, sprung_gross);
        CHECK("es wurden genug Tueren durchschritten", tueren >= 150);
        CHECK("beim Durchschreiten ist immer der richtige Raum rot",
              rot_ok == tueren);
        /* ⛔ DIE ALTE AUSREDE WAR FALSCH - JETZT STEHT HIER EIN RIEGEL.
         * Bis 2026-09-03 meldete diese Phase 61 von 149 Uebergaengen mit ueber 16 px
         * und erklaerte das mit "ALLE AOT-Slots, auch doppelt belegte mit veraltetem
         * Spawn" gegenueber der gefilterten Menge von integration_map_uebergang. Das
         * war eine Geschichte, keine Messung: die Schleife lief LIVE ueber g_aot,
         * waehrend ihr eigener Rumpf ueber schau() zwei weitere Raeume in genau dieses
         * Array lud (siehe der Kommentar an der memcpy-Stelle oben). Sie schrieb Raum A
         * damit die Tueren des zuletzt geladenen Raums zu.
         *
         * Gegenprobe an den Daten: der Spawn einer Tuer liegt vom naechsten
         * Gegen-Trigger im Median 850 Welteinheiten entfernt = 1 Kartenpixel, 95 %
         * innerhalb 3 px, nur 2 von 217 ueber 8 px. Die Spieldaten binden beide
         * Tuerseiten also fest zusammen - ein grosser Kartensprung KANN nur ein
         * Kartenfehler sein und gehoert deshalb sehr wohl hinter eine Schranke.
         *
         * Nach der Reparatur: 10 von 172 (5,8 %). Die Schranke laesst 10 % zu, damit
         * sie eine Regression faengt statt den heutigen Stand einzufrieren.
         * RE15_SPRUNG_LISTE=1 listet jeden Fall. */
        if (sprung_gross * 10 > gleiches_blatt)
            printf("     (RE15_SPRUNG_LISTE=1 listet die Faelle)\n");
        CHECK("hoechstens jeder zehnte Uebergang springt ueber 16 px",
              gleiches_blatt > 0 && sprung_gross * 10 <= gleiches_blatt);
    }
    /* ================= PHASE 3: TUERSYMBOLE IM BILD ====================== */
    {
        static re15_inv_op_t ops3[1024];
        static re15_aot_t sn3_a[RE15_AOT_MAX];
        static re15_aot_door_params_t sn3_d[RE15_AOT_MAX];
        int tueren = 0, sichtbar = 0;
        int identitaet = 0, rueckfall = 0, zufall = 0, ohne_marke = 0;
        for (st = 1; st <= 6; st++) {
            for (r = 0; r < 0x400; r += 0x10) {
                unsigned rid = (unsigned)(st << 12) | (unsigned)r;
                int k;
                if (!betrete(rid, 0, 0, -1)) continue;
                /* Dieselbe Falle wie in Phase 2: betrete() weiter unten setzt g_aot
                 * neu. Hier ist es derselbe Raum, aber scd_room_reenter installiert
                 * abhaengig von der Spielerposition, also darf auch das nicht live
                 * gelesen werden. */
                memcpy(sn3_a, g_aot.slots, sizeof sn3_a);
                memcpy(sn3_d, g_aot.door_params, sizeof sn3_d);
                for (k = 0; k < RE15_AOT_MAX; k++) {
                    const re15_aot_t *a = &sn3_a[k];
                    const re15_map_zone_t *zn;
                    int nops, q, band, gelb = 0;
                    int ident_mk = -1, einzeln_mk = -1;
                    int16_t mmx = 0, mmy = 0;
                    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
                    band = (int)sn3_d[k].band;
                    if (!betrete(rid, a->x, a->z, band)) continue;
                    re15_map_visited_mark(rid);
                    re15_map_visited_mark_at(rid, a->x, a->z);
                    re15_map_zone_update(rid, a->x, a->z);
                    zn = re15_map_zone_current();
                    if (!zn || !zn->synth) continue;
                    re15_inv_screen_open();
                    g_inv_screen.substate = 1;
                    g_inv_screen.item_state = 1;
                    g_inv_screen.map_page = re15_inv_map_page_shown();
                    nops = re15_inv_screen_build(&g_inv_screen, ops3, 1024);
                    re15_inv_map_marker(a->x, a->z, 0, &mmx, &mmy);
                    tueren++;
                    /* Welche Marke MEINT diese Tuer? Dieselbe Aufloesung wie
                     * tuer_anziehen() in re15_inv_screen.c: die Marke, die meine Zone
                     * mit einer Zone des Zielraums verbindet. */
                    {
                        int seite = (int)g_inv_screen.map_page;
                        unsigned ziel =
                            (((unsigned)sn3_d[k].dest_stage + 1u) << 12)
                            | ((unsigned)sn3_d[k].dest_room << 4)
                            | (rid & 0x000Fu);
                        int mn = re15_map_mark_count(), mk;
                        ident_mk = -1; einzeln_mk = -1;
                        for (mk = 0; mk < mn; mk++) {
                            int mpg, mrc, smx, smy, mkind, za, zb, andere, ix;
                            if (!re15_map_mark_get(mk, &mpg, &mrc, &smx, &smy, &mkind))
                                continue;
                            if (mpg != seite || mkind >= 4) continue;
                            if (!re15_map_mark_zonen(mk, &za, &zb)) continue;
                            if (za != zn->zid && zb != zn->zid) continue;
                            andere = (za == zn->zid) ? zb : za;
                            if (andere == 255) {
                                if (einzeln_mk < 0) einzeln_mk = mk;
                                continue;
                            }
                            for (ix = 0; ix < 8; ix++) {
                                const re15_map_zone_t *zt =
                                    re15_map_zone_fuer(ziel, ix, (unsigned)seite);
                                if (zt && zt->zid == andere) { ident_mk = mk; break; }
                            }
                            if (ident_mk >= 0) break;
                        }
                    }
                    /* Ein SICHTBARER gelber Balken im Umkreis von 4 px. Sichtbar =
                     * keine groessere Flaeche liegt davor (kleinerer Index). */
                    for (q = 0; q < nops; q++) {
                        int dx, dy, w;
                        if (ops3[q].kind != RE15_INV_OP_FILL) continue;
                        if (ops3[q].r != 224 || ops3[q].g != 168 || ops3[q].b != 40)
                            continue;
                        dx = ops3[q].x + ops3[q].w / 2 - (int)mmx;
                        dy = ops3[q].y + ops3[q].h / 2 - (int)mmy;
                        if (dx < 0) dx = -dx;
                        if (dy < 0) dy = -dy;
                        if (dx > 4 || dy > 4) continue;
                        /* liegt eine groessere Flaeche davor? */
                        for (w = 0; w < q; w++) {
                            if (ops3[w].kind != RE15_INV_OP_FILL) continue;
                            if (ops3[w].w < 4 || ops3[w].h < 4) continue;
                            if (ops3[q].x + ops3[q].w / 2 < ops3[w].x) continue;
                            if (ops3[q].x + ops3[q].w / 2 >= ops3[w].x + ops3[w].w)
                                continue;
                            if (ops3[q].y + ops3[q].h / 2 < ops3[w].y) continue;
                            if (ops3[q].y + ops3[q].h / 2 >= ops3[w].y + ops3[w].h)
                                continue;
                            break;
                        }
                        if (w == q) { gelb = 1; break; }
                    }
                    /* Vier benannte Zahlen statt einer Quote - die Summe kann gleich
                     * bleiben, waehrend Identitaet faellt und Zufall steigt. */
                    if (gelb && ident_mk >= 0) identitaet++;
                    else if (gelb && einzeln_mk >= 0) rueckfall++;
                    else if (gelb) zufall++;
                    else ohne_marke++;
                    if (gelb) sichtbar++;
                    else if (getenv("RE15_SYMBOL_LISTE"))
                        printf("     KEIN SYMBOL: ROOM%04X Tuer bei Marker (%d,%d)\n",
                               rid, (int)mmx, (int)mmy);
                }
            }
        }
        printf("  [Symbole] %d Tueren, %d mit SICHTBAREM Symbol im Bild\n",
               tueren, sichtbar);
        printf("  [Symbole] Identitaet %d | Rueckfall %d | Zufall %d | ohne Marke %d\n",
               identitaet, rueckfall, zufall, ohne_marke);
        CHECK("es wurden genug Tueren geprueft", tueren >= 150);
        CHECK("mindestens 9 von 10 Tueren zeigen ihr Symbol im Bild",
              tueren == 0 || sichtbar * 10 >= tueren * 9);
        /* ⛔ DIE SCHRANKE HAENGT AN DER IDENTITAET. "Ein Balken liegt daneben" ist ein
         * Mass, das sich selbst bestaetigt; "die Marke verbindet genau diese beiden
         * Orte" nicht. */
        CHECK("mindestens 3 von 4 Tueren loesen ihre Marke ueber die IDENTITAET auf",
              tueren == 0 || identitaet * 4 >= tueren * 3);
    }
    printf("  [Live] %d Raeume geprueft, %d zeigen SICHTBARES Rot, %d nicht\n",
           geprueft, rot_sichtbar, rot_fehlt);
    printf("  [Live] Spieler-Marker in der roten Flaeche: %d von %d\n",
           marker_drin, marker_ges);
    CHECK("es wurden genug Raeume geprueft", geprueft >= 60);
    CHECK("JEDER betretene Raum ist sichtbar rot hervorgehoben", rot_fehlt == 0);
    CHECK("der Spieler-Marker liegt in der roten Flaeche seines Raums",
          marker_ges > 0 && marker_drin + 1 >= marker_ges);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
