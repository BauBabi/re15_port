/* KARTEN-UEBERGANG: springt der Marker beim Durchschreiten einer Tuer?
 *
 * Nutzer 2026-09-02, dritter Anlauf: "Springt immer noch durch die Kartenbereiche nach
 * dem Durchlaufen von Tueren von einem Ort zum anderen ... Debugge dich selber, indem du
 * dich durch die Raeume portest mit dem Debug Menu und durch die Tueren laeufst."
 *
 * ⛔ WARUM DIESES WERKZEUG UND NICHT WIEDER EINE TABELLEN-SONDE.
 * Meine bisherigen Messungen liefen auf der erzeugten Tabelle und bildeten die
 * Marker-Rechnung in Python NACH. Der Spieler sieht aber, was die ENGINE rechnet -
 * re15_inv_map_marker() mit Zonen-Nachfuehrung, Etagen-Umschaltung, Kasten-Klemmung und
 * der Reserve am Rand. Genau dort koennen Fehler sitzen, die eine Nachbildung nicht
 * zeigt. Dieses Werkzeug laedt deshalb JEDEN Raum wie das Spiel (re15_rdt_parse +
 * scd_room_reenter, also mit installierten AOTs), liest die Tueren aus g_aot - denselben
 * Daten, aus denen das Spiel die Tuer oeffnet - und ruft fuer beide Seiten die ECHTE
 * Marker-Funktion.
 *
 * Gemessen wird je Tuer:
 *   pa = Marker, wenn der Spieler in Raum A an der Tuer steht
 *   pb = Marker, wenn er in Raum B am Spawn erscheint
 * Liegen beide Raeume auf demselben Blatt, ist |pa-pb| der SPRUNG, den der Nutzer sieht.
 *
 * RE15_UEBERGANG_LISTE=1 druckt jede Tuer einzeln (das Debug-Werkzeug);
 * ohne die Variable laeuft es als Pin mit einer Schranke.
 */
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

extern re15_inv_screen_t g_inv_screen;
extern unsigned g_current_room_id;
extern re15_room_change_t g_room_change;

static re15_rdt_t s_rdt;
static int g_fail;

#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    uint8_t *b;
    long sz;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

/* Genau die Kette, die das Spiel beim Raumwechsel fährt. */
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
    /* ⛔ DAS BAND DER TUER SETZEN. Das Blatt der Karte haengt am Band des Spielers
     * (re15_inv_map_page_shown -> re15_map_floor_lookup). Ohne das landet jeder
     * Ort auf der Seite seiner HAUPTZEILE, und alle Uebergaenge auf Blaettern, die
     * ueber GAST-Zeilen zusammenhaengen (4 und 5), fielen als "anderes Blatt" aus
     * der Messung. Das Band steht im Door_aot_set-Record (pc[4]). */
    if (band >= 0) re15_collision_set_band(band);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

/* Der Marker, wie ihn der Kartenschirm zeichnet. Blatt wird mitgegeben, weil der
 * Schirm beim Durchschreiten NICHT umblaettert, solange beide Raeume darauf liegen. */
/* Welche Zone die Engine zuletzt gewaehlt hat - fuer die Fehlersuche. */
static unsigned g_zone_raum;
static int g_zone_idx, g_zone_seite, g_zone_synth;

static int marker(unsigned rid, int32_t x, int32_t z, int blatt, int *mx, int *my)
{
    int16_t a = 0, b = 0;
    const re15_map_zone_t *zn;
    g_current_room_id = rid;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = x;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = z;
    re15_map_zone_update(rid, x, z);
    zn = re15_map_zone_current();
    if (!zn) return 0;
    g_zone_raum = zn->room; g_zone_idx = zn->idx;
    g_zone_seite = zn->page; g_zone_synth = zn->synth;
    g_inv_screen.map_page = (uint8_t)(blatt >= 0 ? blatt : zn->page);
    re15_inv_map_marker(x, z, 0, &a, &b);
    if (a < 0 || b < 0) return 0;            /* Marker aus dem Bild genommen */
    *mx = a; *my = b;
    return 1;
}

static int blatt_von(unsigned rid, int32_t x, int32_t z)
{
    const re15_map_zone_t *zn;
    g_current_room_id = rid;
    re15_map_zone_update(rid, x, z);
    zn = re15_map_zone_current();
    if (!zn) return -1;
    /* Derselbe Aufruf, den der Kartenschirm macht - er beruecksichtigt das Band
     * und damit die GAST-Zeilen. zn->page waere die Seite der Hauptzeile. */
    return (int)re15_inv_map_page_shown();
}

typedef struct { unsigned a, b; int32_t ax, az, bx, bz; int band, rez; } tuer_t;

int main(void)
{
    static tuer_t tuer[900];
    int ntuer = 0, i, st, r;
    int gesamt = 0, gross = 0, schlimmster = 0, summe = 0;
    int w_a = 0, w_b = 0, w_zone = 0, w_ma = 0, w_mb = 0, w_blatt = 0, w_einseitig = 0;
    int liste = getenv("RE15_UEBERGANG_LISTE") ? 1 : 0;
    int *werte;
    int proBlatt[13];

    printf("=== Karten-Uebergang: springt der Marker beim Durchschreiten? ===\n");
    re15_map_visited_reset();
    for (i = 0; i < 13; i++) re15_map_debug_reveal_page((unsigned)i);
    re15_inv_map_stage_init(0, 6);
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;

    /* ---- Tueren einsammeln: JEDER Raum wird geladen wie im Spiel ---- */
    for (st = 1; st <= 6; st++) {
        /* ⛔ BIS 0x400, NICHT BIS 0x100. Die Schleife lief nur ueber r = 0x00..0xF0,
         * hat also je Stage NUR die Raeume ROOM?000..ROOM?0F0 geladen - ROOM1130,
         * ROOM1140 und ROOM1170 (der vom Nutzer gemeldete Fall) waren nie dabei, und
         * die Blaetter 4 und 5 hatten deshalb NULL gemessene Uebergaenge, waehrend ich
         * Gesamtzahlen als Fortschritt berichtet habe. Stage 1 reicht bis ROOM1260. */
        for (r = 0; r < 0x400; r += 0x10) {
            unsigned rid = (unsigned)(st << 12) | (unsigned)r;
            int k;
            if (!betrete(rid, 0, 0, -1)) continue;
            for (k = 0; k < RE15_AOT_MAX && ntuer < 900; k++) {
                const re15_aot_t *s = &g_aot.slots[k];
                const re15_aot_door_params_t *d = &g_aot.door_params[k];
                unsigned ziel;
                if (!s->active || s->type != RE15_AOT_TYPE_DOOR) continue;
                /* ⛔ NULLFLAECHEN-RECORDS SIND SKRIPT-WARPS, KEINE TUEREN.
                 * Ein Aot_set mit Breite und Tiefe 0 trifft nach dem Original-
                 * Trefftest FUN_80042b64 (@0x80042b68-98: Treffer genau dann, wenn
                 * (u32)(px-x0) <= w) nur einen EINZIGEN Weltpunkt - man laeuft nicht
                 * hindurch, das Skript feuert ihn per Aot_on. Der Zonen-Generator
                 * wirft sie deshalb raus (gen_map_zones.py:313); die Messschienen
                 * muessen dieselbe Definition benutzen, sonst messen sie einen
                 * Kartensprung ueber einen Warp, den kein Spieler durchschreitet.
                 * Betroffen sind 13 Records, u.a. ROOM3060 -> ROOM3020 (33 px) und
                 * das Paar ROOM1170 <-> ROOM1240 (der Eintritts-Warp). */
                if (s->half_w == 0 && s->half_h == 0) continue;
                /* ⛔ DIESELBE FORMEL WIE DER WARP (aot_common.c:515-517):
                 *     dest_id = ((dest_stage+1) << 12) | (dest_room << 4) | Variante
                 * Ein erster Wurf nahm dest_room als vollen Bytewert und lud damit
                 * 147 von 192 Zielraeumen gar nicht erst. */
                ziel = (((unsigned)d->dest_stage + 1u) << 12)
                     | ((unsigned)d->dest_room << 4)
                     | (rid & 0x000Fu);
                if (ziel == rid) continue;
                tuer[ntuer].a = rid;      tuer[ntuer].b = ziel;
                tuer[ntuer].ax = s->x;    tuer[ntuer].az = s->z;
                tuer[ntuer].bx = d->spawn_x; tuer[ntuer].bz = d->spawn_z;
                /* ⛔ DAS BAND DER TUER STEHT IN door_params, NICHT IM SLOT.
                 * re15_aot_t.band ist der Aot_set-Operand `chain` (bei TREPPEN
                 * gefuellt, bei Tueren 0); das Band der Tuer - und damit das Blatt,
                 * auf dem ihre Marke liegt - steht in door_params[k].band
                 * (Door_aot_set pc[4]). Bis 2026-09-03 las diese Schiene das
                 * Slot-Feld und mass deshalb IMMER mit Band 0. */
                tuer[ntuer].band = (int)d->band;
                tuer[ntuer].rez = 0;
                ntuer++;
            }
        }
    }
    printf("  %d Tueren aus den installierten AOTs gelesen\n", ntuer);
    /* RE15_TUER_DUMP=<datei>: die Tueren ausgeben, die die ENGINE beim Betreten
     * installiert. Der Kartengenerator liest die RDT-Records statisch und sieht
     * dabei auch Tueren, die das Spiel dort gar nicht aufstellt (inerte Records,
     * sce == 0, oder skriptgeschuetzte Aot_set). Solche Tueren haben den
     * Grundriss-Loeser eingeschraenkt, ohne dass man sie begehen kann. */
    {
        const char *dp = getenv("RE15_TUER_DUMP");
        if (dp && *dp) {
            FILE *f = fopen(dp, "w");
            if (f) {
                int q;
                for (q = 0; q < ntuer; q++)
                    fprintf(f, "%04X %04X %d %d %d %d %d\n",
                            tuer[q].a, tuer[q].b, (int)tuer[q].ax, (int)tuer[q].az,
                            (int)tuer[q].bx, (int)tuer[q].bz, tuer[q].band);
                fclose(f);
                printf("  Tueren nach %s geschrieben\n", dp);
            }
        }
    }
    CHECK("es wurden ueberhaupt Tueren gefunden", ntuer >= 100);

    { int _z; for (_z = 0; _z < 13; _z++) proBlatt[_z] = 0; }
    werte = (int *)calloc((size_t)(ntuer > 0 ? ntuer : 1), sizeof(int));

    /* ⛔ GEMESSEN WIRD VON SPAWN ZU SPAWN, NICHT VON DER TRIGGER-MITTE.
     * Der Trigger deckt den ganzen Anlaufbereich ab; sein Mittelpunkt liegt bis zu zwei
     * Kartenpixel tiefer im Raum als die Stelle, an der der Spieler tatsaechlich steht,
     * wenn er die Tuer oeffnet. Was der Nutzer vergleicht, sind die beiden Punkte, an
     * denen man ERSCHEINT: der Spawn der Gegen-Tuer in A und der Spawn dieser Tuer in B.
     * Beide liegen bauartbedingt dicht an der gemeinsamen Wand. */
    /* ⛔ DIE A-SEITE IST DER TRIGGER, NICHT DER GEGEN-SPAWN. Der Spieler steht beim
     * Oeffnen IM Tuer-Trigger - dort zieht der Marker auf das Tuersymbol (siehe
     * tuer_anziehen in re15_inv_screen.c). Der Gegen-Spawn liegt dahinter und wuerde
     * diesen Zug nicht sehen. Gemessen wird also genau die Abfolge des Spielers:
     * im Trigger stehen, druecken, am Spawn erscheinen. */
    for (i = 0; i < ntuer; i++) {
        int j;
        tuer[i].rez = 0;
        for (j = 0; j < ntuer; j++) {
            if (tuer[j].a != tuer[i].b || tuer[j].b != tuer[i].a) continue;
            tuer[i].rez = 1;
            break;
        }
    }

    /* ---- je Tuer: Marker in A, dann Marker in B auf DEMSELBEN Blatt ---- */
    for (i = 0; i < ntuer; i++) {
        int pa_x, pa_y, pb_x, pb_y, blatt, d, pb;
        if (!betrete(tuer[i].a, tuer[i].ax, tuer[i].az, tuer[i].band)) { w_a++; if (liste) printf("     AUSFALL Raum A nicht ladbar: ROOM%04X -> ROOM%04X\n", tuer[i].a, tuer[i].b); continue; }
        blatt = blatt_von(tuer[i].a, tuer[i].ax, tuer[i].az);
        if (blatt < 0) { w_zone++; if (liste) printf("     AUSFALL keine Zone in A: ROOM%04X -> ROOM%04X\n", tuer[i].a, tuer[i].b); continue; }
        if (!marker(tuer[i].a, tuer[i].ax, tuer[i].az, blatt, &pa_x, &pa_y)) { w_ma++; if (liste) printf("     AUSFALL kein Marker in A: ROOM%04X -> ROOM%04X\n", tuer[i].a, tuer[i].b); continue; }
        if (!betrete(tuer[i].b, tuer[i].bx, tuer[i].bz, tuer[i].band)) { w_b++; if (liste) printf("     AUSFALL Raum B nicht ladbar: ROOM%04X -> ROOM%04X\n", tuer[i].a, tuer[i].b); continue; }
        pb = blatt_von(tuer[i].b, tuer[i].bx, tuer[i].bz);
        if (pb != blatt) {
            w_blatt++;
            if (liste)
                printf("     UEBERSPRUNGEN ROOM%04X(S%d) -> ROOM%04X(S%d)\n",
                       tuer[i].a, blatt, tuer[i].b, pb);
            continue;
        }
        if (!marker(tuer[i].b, tuer[i].bx, tuer[i].bz, blatt, &pb_x, &pb_y)) { w_mb++; if (liste) printf("     AUSFALL kein Marker in B: ROOM%04X -> ROOM%04X\n", tuer[i].a, tuer[i].b); continue; }
        if (!tuer[i].rez) { w_einseitig++; if (liste) printf("     AUSFALL ohne Gegen-Tuer: ROOM%04X -> ROOM%04X\n", tuer[i].a, tuer[i].b); continue; }   /* ohne Gegen-Tuer kein Vergleich */
        d = abs(pa_x - pb_x) > abs(pa_y - pb_y) ? abs(pa_x - pb_x) : abs(pa_y - pb_y);
        if (blatt >= 0 && blatt < 13) proBlatt[blatt]++;
        werte[gesamt++] = d;
        summe += d;
        if (d > schlimmster) schlimmster = d;
        if (d > 8) gross++;
        if (liste)
            printf("     Seite %2d  ROOM%04X -> ROOM%04X  (%3d,%3d) -> (%3d,%3d)  = %d px"
                   "   [B-Zone ROOM%04X z%d S%d synth %d]\n",
                   blatt, tuer[i].a, tuer[i].b, pa_x, pa_y, pb_x, pb_y, d,
                   g_zone_raum, g_zone_idx, g_zone_seite, g_zone_synth);
    }

    if (gesamt) {
        int j, k, tmp;
        for (j = 1; j < gesamt; j++) {          /* kleine Einfuegesortierung */
            tmp = werte[j];
            for (k = j - 1; k >= 0 && werte[k] > tmp; k--) werte[k + 1] = werte[k];
            werte[k + 1] = tmp;
        }
        printf("  [Sprung] %d Uebergaenge auf gleichem Blatt: Median %d px, "
               "Mittel %d px, schlimmster %d px, ueber 8 px: %d\n",
               gesamt, werte[gesamt / 2], summe / gesamt, schlimmster, gross);
        {
            int c2 = 0, c4 = 0, c8 = 0, q;
            for (q = 0; q < gesamt; q++) {
                if (werte[q] <= 2) c2++;
                if (werte[q] <= 4) c4++;
                if (werte[q] <= 8) c8++;
            }
            printf("  [Verteilung] <=2 px: %d (%d %%), <=4 px: %d (%d %%), "
                   "<=8 px: %d (%d %%)\n",
                   c2, c2 * 100 / gesamt, c4, c4 * 100 / gesamt, c8, c8 * 100 / gesamt);
        }
    }
    printf("  [Ausfaelle] Raum A nicht ladbar %d, Raum B nicht ladbar %d, keine Zone %d,"
           " Marker A %d, Marker B %d, anderes Blatt %d, ohne Gegen-Tuer %d\n",
           w_a, w_b, w_zone, w_ma, w_mb, w_blatt, w_einseitig);
        /* ---- ABDECKUNG DER MESSUNG ------------------------------------------------
     * ⛔ EINE MESSSCHIENE MUSS IHRE EIGENE ABDECKUNG PRUEFEN. Am 2026-09-02 lief die
     * Sammelschleife nur ueber r = 0x00..0xF0 und lud damit je Stage nur die Raeume
     * ROOM?000..ROOM?0F0. Die Blaetter 4 und 5 hatten NULL gemessene Uebergaenge,
     * der vom Nutzer gemeldete Fall (ROOM1170 <-> ROOM1130) war nicht in der
     * Pruefmenge - und die Gesamtzahlen sahen trotzdem gut aus.
     * Geprueft wird deshalb: genug Tueren, genug Blaetter, und Blatt 4 dabei. */
    {
        int bl, nbl = 0;
        for (bl = 0; bl < 13; bl++)
            if (proBlatt[bl] > 0) nbl++;
        printf("  [Abdeckung] %d Tueren, %d Uebergaenge auf %d verschiedenen"
               " Blaettern, davon %d auf Blatt 4\n", ntuer, gesamt, nbl, proBlatt[4]);
        CHECK("die Schiene liest mindestens 200 Tueren", ntuer >= 200);
        CHECK("es werden mindestens 8 Blaetter gemessen", nbl >= 8);
        CHECK("Blatt 4 ist in der Messung enthalten (ROOM1130/1140/1170)",
              proBlatt[4] >= 5);
    }
CHECK("es wurden Uebergaenge gemessen", gesamt >= 50);

    printf(g_fail ? "FAIL\n" : "OK\n");
    free(werte);
    return g_fail;
}
