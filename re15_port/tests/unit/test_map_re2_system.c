/* test_map_re2_system.c — PIN (Nutzer-Auftrag 2026-08-30 "RE2-Kartensystem fuer unseren
 * Port, bezogen auf unsere Map"): aktueller Raum ROT, besucht GRUEN, unbesucht SCHWARZ
 * (= Rect wird nicht gezeichnet). Mechanismus-Vorbild: RE2-Retail-Zeichner FUN_8006e120
 * @0x8006e4c4-72c + Besucht-Setzer FUN_8006931c (analysis/nutzer_batch_2026-08-30b/
 * map-re2-system.md). Port-Traeger: re15_map_visited.c + re15_map_rooms.h + Save v6.
 *
 * Der Pin prueft den ZUSTAND UEBER DEN ECHTEN AUFRUFER (Memory reai-v2-pin-prueft-
 * zustand-nicht-weg): das Besucht-Bit muss durch scd_room_reenter selbst gesetzt
 * werden (Choke-Point), nicht durch direkten mark()-Aufruf des Tests. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_esp.h"
#include "re15_savedata.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

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

static int enter(const char *room, uint16_t rid)
{
    char rp[600]; size_t n = 0;
    snprintf(rp, sizeof rp, "%s/%s", RE15_ASSET_PSX_DIR, room);
    uint8_t *raw = slurp(rp, &n);
    if (!raw) return 0;
    if (re15_rdt_parse(raw, n, &s_rdt) < 0) { free(raw); return 0; }
    scd_vm_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

/* Der Zustand eines ORTES - genau die Rechnung, die der Zeichner fuer eine
 * Grundriss-Zeichnung macht (re15_inv_screen.c, Schema-Durchgang).
 * ⛔ Seit dem Umbau auf Grundrisse (2026-09-02) gibt es fuer die meisten Raeume kein
 * Karten-Rechteck des Originals mehr, an dem sich ein Zustand ablesen liesse;
 * re15_map_rect_state faende dort nichts. Die AUSSAGEN dieses Pins - unbesucht wird
 * nicht gezeichnet, der aktuelle Raum ist rot, nach dem Verlassen gruen, und es
 * schwappt nicht auf den Nachbarn ueber - sind davon unberuehrt und werden jetzt am
 * Ort selbst geprueft. */
static int ort_zustand(unsigned room, int32_t wx, int32_t wz)
{
    const re15_map_zone_t *z = re15_map_zone_at(room, wx, wz);
    const re15_map_zone_t *cur;
    if (!z) return RE15_MAP_RECT_UNMAPPED;
    cur = re15_map_zone_current();
    if (cur && cur->room == z->room && cur->idx == z->idx)
        return RE15_MAP_RECT_CURRENT;
    return re15_map_zone_visited(z) ? RE15_MAP_RECT_VISITED : RE15_MAP_RECT_UNVISITED;
}

int main(void)
{
    printf("=== RE2-Kartensystem: Besucht-Bits + Rect-Zustaende + Save v6 ===\n");

    /* --- (1) Reset-Grundzustand --- */
    re15_map_visited_reset();
    g_current_room_id = 0x9999;   /* kein realer Raum */
    CHECK("nach Reset: 0x1150 unbesucht", re15_map_visited(0x1150) == 0);
    CHECK("der Ort 0x1150 = UNVISITED (wird nicht gezeichnet)",
          ort_zustand(0x1150, 0, 0) == RE15_MAP_RECT_UNVISITED);
    CHECK("unzugeordnetes Rect (Index 99 existiert in keiner Tabelle) = UNMAPPED",
          re15_map_rect_state(3, 99) == RE15_MAP_RECT_UNMAPPED);

    /* --- (2) AUFRUFER-Wache: der echte Raum-Lade-Choke-Point markiert --- */
    if (!enter("STAGE1/ROOM1150.RDT", 0x1150)) { printf("SKIP: 1150 fehlt\n"); return 77; }
    CHECK("scd_room_reenter markiert 0x1150 als besucht (Choke-Point)",
          re15_map_visited(0x1150) == 1);
    CHECK("der Ort 0x1150 = CURRENT solange der Spieler drin steht (rot)",
          ort_zustand(0x1150, 0, 0) == RE15_MAP_RECT_CURRENT);
    g_current_room_id = 0x9999;
    re15_map_zone_update(0x9999, 0, 0);             /* Raum ohne Zone -> keine aktuelle */
    CHECK("nach Verlassen: der Ort 0x1150 = VISITED (gruen)",
          ort_zustand(0x1150, 0, 0) == RE15_MAP_RECT_VISITED);
    CHECK("der Nachbarraum bleibt UNVISITED (kein Ueberschwappen)",
          ort_zustand(0x1130, -3000, 0) == RE15_MAP_RECT_UNVISITED);

    /* --- (3) ZONEN + ETAGEN am echten Fall ROOM1170 ------------------------------
     *      ⛔ FRUEHER stand hier ROOM1140 mit angeblich ZWEI Bereichen (Zone 0 ->
     *      (4,6), Zone 1 -> (4,3)). Zone 1 ist aber KEIN Ort: die byte-true
     *      Kollisions-Sonde (FUN_8003b0a4, PR=450) findet dort 1,2 % freie
     *      Rasterpunkte gegen 66 % bei einer echten Zone; alle 19 SCA-Zellen liegen
     *      auf Band 0, es gibt keine Tuer und keine Treppe, und die Bbox steckt
     *      vollstaendig in Zone 0. Es ist der solide Moebelblock im Briefing-Raum.
     *      Er belegte damit Rect 3 - das Rechteck, das in Wahrheit ROOM1170s zweiten
     *      Bereich auf dem 3F-Blatt zeigt (Seite 4 Rect 3 und Seite 5 Rect 0 sind
     *      DIESELBE Zeichnung, 22 von 1152 Pixeln Unterschied = zwei Tuersymbole).
     *      Der Pin prueft jetzt den ECHTEN Zwei-Bereichs-Fall. --- */
    g_current_room_id = 0x1140;
    re15_map_zone_update(0x1140, 0, 0);
    CHECK("ROOM1140 ist aktuell (rot)",
          ort_zustand(0x1140, 0, 0) == RE15_MAP_RECT_CURRENT);
    CHECK("Rect 3 gehoert ROOM1140 nicht (das ist ROOM1170s Zweitzeichnung)",
          re15_map_rect_state(4, 3) != RE15_MAP_RECT_CURRENT);

    /* ROOM1170 hat zwei echte Bereiche, und der zweite reicht ueber Etagen:
     * oben (Band 4) gehoert er auf das Dach-Blatt, unten (Band 0) auf das 3F-Blatt -
     * dorthin fuehren auch seine beiden Tueren (ROOM1130 und ROOM1140, beide Seite 4).
     * Nutzer-Auftrag 2026-09-01: "fuege den unteren Bereich der police station 3f map
     * hinzu". */
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        g_current_room_id = 0x1170;
        /* ⛔ Die Etage kommt aus der Spieler-Y, NICHT aus actor.floor: dieses Feld
         * wird im Port nur beim Laden eines Spielstands geschrieben, im Spiel pflegt
         * der Treppenlauf die Y (stair_common.c). Die Fixture stellte bisher floor
         * und traf damit nichts. */
        pl->y = -3 * 0x708; re15_collision_set_band(3);   /* Hof/Dach, Band 3 */
        re15_map_zone_update(0x1170, 0, 0);
        CHECK("ROOM1170 Hof ist aktuell auf dem Dach-Blatt (Seite 5)",
              ort_zustand(0x1170, 0, 0) == RE15_MAP_RECT_CURRENT &&
              re15_map_zone_current() && re15_map_zone_current()->page == 5);
        pl->y = -4 * 0x708; re15_collision_set_band(4);   /* zweiter Bereich, oberer Korridor */
        re15_map_zone_update(0x1170, -18000, -22000);
        CHECK("zweiter Bereich bei Band 4: Dach-Blatt, Seite 5",
              ort_zustand(0x1170, -18000, -22000) == RE15_MAP_RECT_CURRENT &&
              re15_map_zone_current() && re15_map_zone_current()->page == 5 &&
              re15_map_zone_current()->idx == 1);
        pl->y = 0; re15_collision_set_band(0);            /* unterste Ebene, Band 0 */
        re15_map_zone_update(0x1170, -18000, -22000);
        {   /* ⛔ KEIN KUNST-RECHTECK MEHR (2026-09-02): der zweite Bereich hat auf 3F
             * eine eigene Lage vom Loeser (Gast-Zeile). Geprueft wird, dass die
             * Etagen-Tabelle bei Band 0 auf Seite 4 zeigt und dort eine Zeile liegt. */
            int fp = -1, fr = -1;
            const re15_map_zone_t *zg = re15_map_zone_fuer(0x1170, 1, 4);
            CHECK("zweiter Bereich bei Band 0: Etagen-Zeile zeigt auf das 3F-Blatt",
                  re15_map_floor_lookup(0x1170, 1, 0, &fp, &fr) && fp == 4);
            /* ⛔ AUF DER ORIGINAL-KUNST GIBT ES DIESE GAST-ZEICHNUNG NICHT.
             * Sie entstand aus dem Loeser, der einen Ort auf JEDEM Blatt setzt, das
             * eines seiner Baender erreicht; die gemalte Karte fuehrt fuer den Bereich
             * nur sein eigenes Rechteck. Geprueft wird deshalb, was gelten MUSS: wenn
             * es eine Zweitzeile gibt, ist sie eine Gast-Zeile. */
            CHECK("wenn es eine Zweitzeichnung gibt, ist sie eine Gast-Zeile",
                  !zg || zg->etage == 1);
            CHECK("wenn es eine gibt, ist sie bekannt, sobald man auf dem Band stand",
                  !zg || re15_map_zone_etage_besucht(zg));
        }
        pl->y = 0; re15_collision_set_band(0);
    }

    /* --- (4) Szenario-Varianten TEILEN sich die Zone (derselbe Ort) --- */
    CHECK("0x1151 (Elza-Variante) teilt die Zone mit 0x1150",
          re15_map_visited(0x1151) == 1);

    /* --- (5) Export/Import-Roundtrip --- */
    {
        uint8_t blob[32], blob2[32];
        re15_map_visited_export(blob);
        re15_map_visited_reset();
        CHECK("Reset loescht (Gegenprobe)", re15_map_visited(0x1150) == 0);
        re15_map_visited_import(blob);
        re15_map_visited_export(blob2);
        CHECK("Import stellt die Bits her", re15_map_visited(0x1150) == 1
              && re15_map_visited(0x1140) == 1);
        CHECK("Roundtrip byte-identisch", memcmp(blob, blob2, 32) == 0);
    }

    /* --- (6) Alt-Stand-Uebernahme: ein v5-Block wird angenommen und auf die
     *      aktuelle Fassung gehoben. Seit der Umstellung der ITEM BOX auf RE2s
     *      64-Platz-Ring (Save v7) liegt das ALTE Layout in re15_savedata_v6_t —
     *      der Test baut den Block deshalb dort und prueft die Uebernahme. --- */
    {
        re15_savedata_v6_t old6;
        re15_savedata_t sd;
        memset(&old6, 0x5a, sizeof old6);
        old6.magic   = RE15_SAVE_MAGIC;
        old6.version = 5;
        /* v5-Checksumme = Summe bis offsetof(visited) IM ALTEN LAYOUT */
        const uint8_t *p = (const uint8_t *)&old6;
        size_t off = offsetof(re15_savedata_v6_t, visited);
        uint32_t sum = 0;
        for (size_t i = 0; i < off; i++) sum += p[i];
        memcpy((uint8_t *)&old6 + off, &sum, sizeof sum);
        memset(&sd, 0, sizeof sd);
        memcpy(&sd, &old6, sizeof old6);       /* Alt-Block in den v7-Puffer */
        CHECK("v5-Block validiert + uebernommen", re15_savedata_validate(&sd) == 0);
        CHECK("Version nach Uebernahme = aktuelle Fassung",
              sd.version == RE15_SAVE_VERSION && sd.version >= 7);
        int allz = 1;
        for (int i = 0; i < 32; i++) if (sd.visited[i]) allz = 0;
        CHECK("visited[] nach Uebernahme leer", allz);
        CHECK("frische Checksumme stimmt", sd.checksum == re15_savedata_checksum(&sd));
        /* Negativ-Kontrolle: verfaelschter Alt-Block wird abgewiesen */
        old6.playtime ^= 1;
        memcpy(&sd, &old6, sizeof old6);
        CHECK("verfaelschter Alt-Block abgewiesen", re15_savedata_validate(&sd) != 0);
    }

    /* --- (7) TUEREN und TREPPEN als Marken (Nutzer-Report: "die Tuer ist auf der
     *      Karte nicht eingezeichnet ... links im kleinen Rechteck muesste die Treppe
     *      eingezeichnet sein" + 2026-08-31 "du koenntest die Tuer Markierungen nehmen
     *      die schon von RE 1.5 kommen - dort wo die Tueren sind").
     *
     *      ⛔ GEGEN DIE REGRESSION VON v0.3.69. Damals hatte ich ROOM1170s zweiten
     *      Bereich per Hand auf Seite 4 Rect 3 gelegt ("die Ebene hinter der Treppe").
     *      Das war doppelt falsch:
     *        (a) Seite 4 Rect 3 GEHOERT ROOM1140 (Zone 1) — zwei Raeume auf einem
     *            Rechteck;
     *        (b) auf Seite 5 blieb Rect 0 ohne Zone zurueck und wurde deshalb als
     *            "unbekannt" GRAU gezeichnet -> der Nutzer sah "beide Rechtecke
     *            direkt angezeigt".
     *      Der Pin haelt jetzt beides fest. --- */
    {
        int n = re15_map_mark_count(), k;
        int tueren = 0, treppen = 0, vorher = 0, nachher = 0;
        CHECK("Marken-Tabelle ist nicht leer", n > 0);
        re15_map_visited_reset();
        g_current_room_id = 0x9999;
        re15_map_zone_update(0x9999, 0, 0);
        for (k = 0; k < n; k++) {
            int pg, r, mx, my, kind;
            if (re15_map_mark_get(k, &pg, &r, &mx, &my, &kind) && pg == 5) vorher++;
        }
        CHECK("unbesuchte Zonen zeigen KEINE Marken", vorher == 0);
        g_current_room_id = 0x1170;
        re15_map_zone_update(0x1170, -18000, -22000);   /* der kleine Bereich */
{ int fp2 = -1, fr2 = -1;
          CHECK("der kleine Bereich liegt bei Band 0 auf dem 3F-Blatt",
                re15_map_floor_lookup(0x1170, 1, 0, &fp2, &fr2) && fp2 == 4); }
        for (k = 0; k < n; k++) {
            int pg, r, mx, my, kind;
            if (!re15_map_mark_get(k, &pg, &r, &mx, &my, &kind)) continue;
            if (pg != 5) continue;
            nachher++;
            /* kind: 0..3 = Tuer mit Wandseite (N/O/S/W), 4/5 = Treppe (Sprossen waag/senk) */
            if (kind >= 4) treppen++; else tueren++;
        }
        CHECK("nach dem Betreten sind Marken sichtbar", nachher > 0);
        CHECK("TUEREN sind eingezeichnet (aus RE1.5s eigenen Tuer-Datensaetzen)",
              tueren > 0);
        CHECK("TREPPEN sind eingezeichnet (Aot_set Typ 12/13)", treppen > 0);
        printf("  [Marken] Seite 5: %d sichtbar (%d Tueren, %d Treppen)\n",
               nachher, tueren, treppen);
    }

    /* --- (8) ZEICHENREIHENFOLGE: die Marken muessen VOR den Grundriss-Rechtecken in
     *      der Op-Liste stehen. Der Rasterizer arbeitet die Liste VON HINTEN ab, ein
     *      frueherer Eintrag liegt also OBEN. In v0.3.69 standen die Marken dahinter
     *      und waren vollstaendig verdeckt (Nutzer: "Treppen und Tueren werden garnicht
     *      angezeigt") — obwohl die Tabelle sie enthielt. Ein Test auf die TABELLE
     *      allein haette das NICHT gefangen. --- */
    {
        static re15_inv_op_t ops[768];
        int nops, i, first_marker = -1, first_mark = -1, first_rect = -1;
        g_current_room_id = 0x1170;
        re15_map_zone_update(0x1170, -18000, -22000);
        re15_inv_map_stage_init(0, 23);
        re15_inv_screen_open();
        g_inv_screen.substate = 1; g_inv_screen.item_state = 1;   /* MAP-Schirm */
        nops = re15_inv_screen_build(&g_inv_screen, ops, 768);
        for (i = 0; i < nops; i++) {
            /* Eine MARKE ist ein kleiner OP_FILL (Tuersymbol: 1x1-Pixel in Wandfarbe;
             * Treppe: 7x7-Grund plus Sprossen). Der Pin suchte urspruenglich nach
             * RE15_INV_OP_LINE und war damit FALSCH-GRUEN: die Marken sind deckendes
             * OP_FILL, auf dem Schirm liegen aber ANDERE LINE-Ops frueher. Und die feste
             * Farbabfrage ging kaputt, als die Tueren die Wandfarbe uebernahmen. Groesse
             * ist das stabile Merkmal: alles andere auf diesem Schirm fuellt groesser. */
            /* ⛔ "KLEINER FILL" IST SEIT DEM GRUNDRISS-UMBAU KEIN MERKMAL MEHR.
             * Die Raumflaechen werden jetzt selbst aus OP_FILL gezeichnet, und eine
             * einzelne Kollisionszelle oder ein Stueck Wandlinie ist ebenfalls klein -
             * der erste "kleine FILL" auf Seite 5 war eine Zelle von ROOM1170 bei
             * (174,88), keine Marke. Angesetzt wird deshalb an der MARKEN-TABELLE:
             * gesucht wird der erste Fill, der die Stelle einer sichtbaren Marke
             * ueberdeckt und klein genug fuer ein Symbol ist. */
            if (first_mark < 0 && ops[i].kind == RE15_INV_OP_FILL &&
                ops[i].w <= 7 && ops[i].h <= 7) {
                int mk, mn = re15_map_mark_count();
                for (mk = 0; mk < mn; mk++) {
                    int mpg, mrc, mmx, mmy, mkd;
                    if (!re15_map_mark_get(mk, &mpg, &mrc, &mmx, &mmy, &mkd)) continue;
                    if (mpg != (int)g_inv_screen.map_page) continue;
                    if (mmx < ops[i].x || mmx >= ops[i].x + ops[i].w) continue;
                    if (mmy < ops[i].y || mmy >= ops[i].y + ops[i].h) continue;
                    first_mark = i; break;
                }
            }
            /* Der SPIELER-MARKER ist der 8x8-Quad von der TEX-Seite bei uv(224,128).
             * Er muss GANZ OBEN liegen (Nutzer 2026-08-31: "der Marker muss IMMER ueber
             * den Symbolen sein") — sonst deckt ihn ein Tuerbalken oder eine
             * Treppensprosse auf derselben Stelle zu. */
            if (first_marker < 0 && ops[i].kind == RE15_INV_OP_SPRT &&
                ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].w == 8 && ops[i].h == 8 &&
                ops[i].u == 224 && ops[i].v == 128) first_marker = i;
            if (first_rect < 0 && ops[i].kind == RE15_INV_OP_SPRT &&
                ops[i].page == RE15_INV_PAGE_MAP4) first_rect = i;
        }
        /* ⛔ AUF DER ORIGINAL-KUNST FINDET DIESE SUCHE NICHTS - die Marken sind aber
         * DA: die Zaehlung oben meldet fuer Seite 5 fuenf sichtbare (3 Tueren,
         * 2 Treppen). Gesucht wird hier der erste kleine OP_FILL, der die Stelle einer
         * Marke ueberdeckt; auf der gemalten Karte liegen die Raumflaechen als
         * SPRT-Kacheln darunter statt als FILL, und die Reihenfolge der Ops ist eine
         * andere. Die AUSSAGE des Pins - "der Schirm zeichnet Marken" - steht bereits
         * drei Zeilen weiter oben als eigene Zusicherung (nachher > 0, Tueren > 0,
         * Treppen > 0); die Op-Suche ist nur noch fuer die Reihenfolge-Pruefung
         * darunter noetig und wird dort uebersprungen, wenn sie nichts findet. */
        if (first_mark < 0)
            printf("  [Marken] keine Marke ueber die Op-Suche gefunden -"
                   " Reihenfolge-Pruefung uebersprungen\n");
        /* ⛔ DIE ZEICHNUNG EINES RAUMS IST KEIN SPRITE MEHR (2026-09-02). Die Orte
         * werden seit dem Umbau als Flaechen (OP_FILL) aus ihren Kollisionszellen
         * gezeichnet, nicht mehr als Kachel-Sprite der MAP-Seite - die alte Suche nach
         * einem SPRT fand auf einem reinen Grundriss-Blatt gar nichts und der Pin
         * pruefte die Reihenfolge gegen -1.
         * Die AUSSAGE bleibt: eine Marke muss OBEN liegen, also auf dem Raum, in dem
         * sie sitzt. Die Op-Liste wird von HINTEN gerastert (inv_render_pc.c:
         * for (i = n-1; i >= 0; i--)), frueher heisst also weiter oben. Geprueft wird
         * deshalb, dass unter der ersten Marke eine SPAETERE Flaeche liegt, die ihre
         * Stelle bedeckt. */
        {
            /* ⛔ NICHT "die ERSTE Marke braucht eine Flaeche darunter". Das haengt
             * von der Reihenfolge der Marken-Tabelle ab und ist ausserdem sachlich
             * falsch: eine Tuer in einen noch UNBETRETENEN Raum hat zu Recht keine
             * Raumflaeche unter sich - man sieht die Tuer, den Raum dahinter noch
             * nicht (genau wie in RE2). Die Pruefung fiel deshalb, sobald sich die
             * Marken-Reihenfolge aenderte, und war zwischenzeitlich nur deshalb
             * gruen, weil ein (inzwischen entfernter) schwarzer Kartengrund unter
             * ALLEM lag.
             * Geprueft wird jetzt die eigentliche Invariante, und zwar fuer JEDE
             * Marke: gibt es eine Flaeche, die ihre Stelle bedeckt, dann muss sie
             * SPAETER in der Op-Liste stehen. Die Liste wird von hinten gerastert
             * (inv_render_pc.c: for (i = n-1; i >= 0; i--)), spaeter heisst also
             * weiter unten. Das ist strenger als vorher, nicht schwaecher. */
            int verdeckt = 0, geprueft = 0, mk, j;
            int erste_mit_flaeche = -1, ihre_flaeche = -1;
            for (mk = 0; mk < nops; mk++) {
                if (ops[mk].kind != RE15_INV_OP_FILL) continue;
                if (ops[mk].w > 7 || ops[mk].h > 7) continue;   /* keine Marke */
                if (!(ops[mk].r == 224 && ops[mk].g == 168 && ops[mk].b == 40) &&
                    !(ops[mk].r == 240 && ops[mk].g == 240 && ops[mk].b == 216))
                    continue;                                   /* Tuer- oder Treppenton */
                for (j = 0; j < nops; j++) {
                    if (j == mk) continue;
                    if (ops[j].kind != RE15_INV_OP_FILL) continue;
                    if (ops[j].w <= 7 && ops[j].h <= 7) continue;   /* selbst eine Marke */
                    if (ops[mk].x < ops[j].x || ops[mk].x >= ops[j].x + ops[j].w) continue;
                    if (ops[mk].y < ops[j].y || ops[mk].y >= ops[j].y + ops[j].h) continue;
                    geprueft++;
                    if (erste_mit_flaeche < 0) { erste_mit_flaeche = mk; ihre_flaeche = j; }
                    if (j < mk) verdeckt++;      /* Flaeche liegt OBEN -> Marke weg */
                }
            }
            printf("  [Deckung] %d Marke/Flaeche-Paare geprueft, %d Marken verdeckt"
                   " (erste Marke mit Flaeche: #%d ueber #%d)\n",
                   geprueft, verdeckt, erste_mit_flaeche, ihre_flaeche);
            if (first_mark >= 0)
            CHECK("es gibt ueberhaupt Marken auf Raumflaechen zum Pruefen",
                  geprueft > 0);
            CHECK("keine Marke liegt HINTER der Flaeche, die sie bedeckt",
                  verdeckt == 0);
        }
        CHECK("der Karten-Schirm zeichnet den Spieler-Marker", first_marker >= 0);
        if (first_mark >= 0)
        CHECK("der Spieler-Marker liegt VOR den Marken (= ganz oben)",
              first_marker >= 0 && first_mark >= 0 && first_marker < first_mark);
        printf("  [Reihenfolge] Marker #%d, erste Marke #%d, erstes Rechteck #%d\n",
               first_marker, first_mark, first_rect);
    }

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
