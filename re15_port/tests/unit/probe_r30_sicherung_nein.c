/* probe_r30_sicherung_nein.c — RIEGEL (Runde 30, Nachschliff "sicherung-nein").
 *
 * SYMPTOM (Gegenpruefer, Lauf g2/taste_nein): wer im Sicherungs-Modal des Hebetischs
 * (ROOM1150/1151) mit "No" antwortet, bekommt die Sicherung im selben Raumaufenthalt nie
 * wieder angeboten. Die zweite Fahrt (Cut_chg(4) bei F1186) faehrt hoch, das Rohr ist zu
 * sehen, aber kein Modal. Ursache: die Sperre s_modal_ausgeloest in sicherung_1150.c galt je
 * RAUMAUFENTHALT (nur re15_sicherung_install setzte sie zurueck).
 *
 * ORIGINAL-REGEL fuer abgelehnte Aufnahmen (Dossier nachschliff-sicherung-nein.md §3):
 *   RE1.5  Zustand 7 der Aufnahme-FSM @0x8001e048: nur der JA-Zweig nullt das sce-Byte der
 *          ausloesenden Zone (`sb zero,0(v1)` @0x8001e090, v1 = [0x800aca30]); "No"
 *          (@0x8001e068 `andi v0,v0,0x1` -> @0x8001e06c `bne` -> @0x8001e0ec) laesst die
 *          Zone stehen, Zustand 8 schrumpft das Bild weg und endet mit Zustand 0
 *          (@0x8001e16c). Der Zonen-Scan ueberspringt nur sce-0-Records (@0x80042f48/50).
 *          -> erneutes Untersuchen oeffnet das Modal wieder.
 *   RE2    dieselbe Regel: "No" (@0x800720c8 `andi v0,v1,0x1` -> @0x800720cc) geht ueber
 *          Zustand 4..17 nach 18 (@0x800723f0 Se_on 0x405, Schirm zu), OHNE die Zone
 *          anzufassen; nur der JA-Weg nullt sie (`sb zero,0(v1)` @0x80072298).
 *   Ein Ausloesen = hoechstens EIN Modal: der Item-Handler startet nur bei Zustand 0
 *          (@0x80043334 `bne v0,zero` gegen DAT_80072d3b).
 *
 * UEBERTRAGEN auf die Port-Ergaenzung (der Ausloeser ist die FAHRT, sub04 @0x0F96-0x10B6):
 * die Sperre gilt je Fahrt. Wieder scharf, sobald die Plattform in der Parklage liegt
 * (Pos_set @0x109E y = -20224); innerhalb einer Fahrt bleibt y in [-1215, -301]
 * (Pos_set @0x0FB4 -305, For @0x0FF6 91 x -10, For @0x1010 10 x +1, For @0x1042 90 x +10,
 * @0x105C 2 x +2, @0x106A 2 x -2). ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE (das Original hat im
 * Hebetisch kein Modal) — gemessen von dieser Sonde (MESS-Zeilen).
 *
 * PRUEFUNGEN, je Raum (ROOM1150 und ROOM1151):
 *   Fall A "No, dann Yes":
 *     1  Fahrt 1 oeffnet genau EIN Modal (auch nach "No" nicht ein zweites, obwohl die
 *        Plattform noch ~100 Bilder im Fenster steht)
 *     2  das Modal geht OBEN auf (y im Fenster (-5000,-1100])
 *     3  nach "No": kein Genommen-Flag, Prop sichtbar, Item 0x40 NICHT im Inventar
 *     4  Fahrt 1 endet in der Parklage y = -20224 (Pos_set @0x109E)
 *     5  Fahrt 2 oeffnet wieder genau EIN Modal            <- der Befund
 *     6  nach "Yes": Flag (9,53), Prop weg, Item 0x40 im Inventar
 *     7  Fahrt 3 oeffnet KEIN Modal, Item 0x40 genau einmal im Inventar
 *   Fall B "Yes in Fahrt 1":
 *     8  Fahrt 1 genau EIN Modal, danach Flag, Prop weg, Item im Inventar
 *     9  Fahrt 2 oeffnet KEIN Modal, keine zweite Aufnahme, Prop bleibt weg
 *   Fall C "Inventar voll" (derselbe Zweig wie "No": @0x8001e054 `bltz` -> @0x8001e0ec):
 *    10  Fahrt 1: genau EIN Modal ("YOU CAN'T CARRY ANY MORE ITEMS"), nichts genommen
 *    11  Fahrt 2 (ein Platz frei geraeumt): das Modal geht WIEDER auf, "Yes" nimmt die Sicherung
 *
 * Die Bildschleife folgt der Reihenfolge des Spiels: SCD-Tick nur ohne Modal (main.c, der
 * Freeze @0x8001cdec), dann re15_sicherung_tick (game_step_common.c vor dem Freeze-Gate),
 * dann der Modal-Tick (main.c nach re15_game_step, @0x8001ce34).
 *
 * Rueckgabe 0 = alle Pruefungen bestanden, sonst die Nummer der ersten gerissenen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_sicherung.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* Parklage NACH der Szene: Pos_set @0x109E `32 00 24 af 00 b1 cc bb` -> y = 0xb100 = -20224
 * (ROOM1150.RDT; ROOM1151.RDT dieselben Bytes 0x22 frueher). */
#define PARK_NACH_SZENE  (-20224)

static int fehler = 0;
static int erste_nummer = 0;

static void pruefe(int nr, const char *was, int ok_)
{
    printf("   [%s] %d. %s\n", ok_ ? "OK " : "FEHL", nr, was);
    if (!ok_) { fehler++; if (!erste_nummer) erste_nummer = nr; }
}

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

static int slot_von_obj(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

static int sicherung_sichtbar(void)
{
    int s = slot_von_obj(RE15_SICHERUNG_OBJ_ID);
    return s >= 0 && g_scd.props[s].active;
}

static int anzahl_im_inventar(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < (int)(sizeof g_inv.slots / sizeof g_inv.slots[0]); i++)
        if (g_inv.slots[i].id == id) n++;
    return n;
}

/* Frischer Spielstart in einem der beiden Bueros (dieselbe Anlage wie probe_sicherung_1150). */
static void raum_frisch(re15_rdt_t *rdt, uint16_t rid)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -21000; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt->messages, rdt->messages_size); }
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, -21000, -18500, 0);
}

enum { ANTWORT_JA = 0, ANTWORT_NEIN = 1 };

typedef struct {
    int  modale;        /* wie oft re15_sicherung_tick in dieser Fahrt ein Modal aufmachte */
    int  bild_modal;    /* Bild (ab Ausloesen) des ersten Modals, -1 = keins */
    long y_modal;       /* Plattform-y in diesem Bild */
    int  bild_park;     /* Bild, in dem die Plattform wieder in der Parklage stand, -1 = nie */
    long y_park;
    long y_min, y_max;  /* Fahrt IM RAUM (Parklage ausgenommen) */
} fahrt_t;

/* Eine Fahrt: sub04 ausloesen und Bild fuer Bild wie das Spiel ticken. Ein offenes Modal
 * wird beantwortet, sobald der Text steht: ANTWORT_NEIN schaltet erst mit der
 * Links/Rechts-Flanke (virtuell 0x3000, @0x800285f0-Weg) auf "No" und bestaetigt dann mit
 * der virtuellen 0x4000 (Viereck). */
static fahrt_t fahrt(int antwort)
{
    fahrt_t r;
    memset(&r, 0, sizeof r);
    r.bild_modal = -1; r.bild_park = -1;
    r.y_min = 0x7fffffff; r.y_max = -0x7fffffff;

    int thread = scd_event_fire(4);
    int war_im_raum = 0;
    for (int f = 0; f < 900; f++) {
        if (!re15_item_modal_active()) scd_vm_tick();          /* Freeze @0x8001cdec */
        int p = slot_von_obj(0);
        long py = p >= 0 ? (long)g_scd.props[p].y : 0;
        if (re15_sicherung_tick()) {                          /* game_step_common.c */
            r.modale++;
            if (r.bild_modal < 0) { r.bild_modal = f; r.y_modal = py; }
            printf("   MESS Bild %3d: Modal auf, Plattform y=%ld (Modal Nr. %d dieser Fahrt)\n",
                   f, py, r.modale);
        }
        if (re15_item_modal_active()) {                       /* main.c nach dem Step */
            uint16_t edge = 0;
            uint8_t typ = 0; int wahl = 0;
            int art = re15_item_modal_prompt(&typ, &wahl);
            if (re15_item_modal_prompt_ready() && art == 1) {
                if (antwort == ANTWORT_NEIN && wahl == 0) edge = 0x1000;   /* auf "No" */
                else                                      edge = 0x4000;   /* bestaetigen */
            } else if (re15_item_modal_prompt_ready() && art == 2) {
                edge = 0x4000;                         /* "can't carry": wegdruecken */
            }
            re15_item_modal_tick(edge, edge);
        }
        if (py > -5000) {
            war_im_raum = 1;
            if (py < r.y_min) r.y_min = py;
            if (py > r.y_max) r.y_max = py;
        } else if (war_im_raum && r.bild_park < 0) {
            r.bild_park = f; r.y_park = py;
            printf("   MESS Bild %3d: Plattform wieder in der Parklage y=%ld\n", f, py);
        }
        /* Fahrt zu Ende: geparkt und kein Modal mehr offen; noch 20 Bilder nachlaufen
         * lassen (Set/Cut_old/Evt_end @0x10A6-0x10B4). */
        if (r.bild_park >= 0 && f > r.bild_park + 20 && !re15_item_modal_active()) break;
    }
    printf("   MESS Fahrt: thread=%d, im Raum y von %ld bis %ld, Modale=%d (erstes Bild %d y=%ld), "
           "geparkt Bild %d y=%ld\n",
           thread, r.y_max, r.y_min, r.modale, r.bild_modal, r.y_modal, r.bild_park, r.y_park);
    return r;
}

static int fall_a(re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Fall A: \"No\" in Fahrt 1, \"Yes\" in Fahrt 2 ==\n", rid);
    raum_frisch(rdt, rid);
    if (!sicherung_sichtbar()) { pruefe(1, "Sicherung ist nach dem Raumstart angelegt", 0); return 1; }

    printf("  -- Fahrt 1 (Antwort No)\n");
    fahrt_t f1 = fahrt(ANTWORT_NEIN);
    pruefe(1, "Fahrt 1 oeffnet genau EIN Modal (nach No kein zweites in derselben Fahrt)",
           f1.modale == 1);
    pruefe(2, "das Modal geht OBEN auf (y im Fenster (-5000,-1100])",
           f1.bild_modal >= 0 && f1.y_modal > -5000 && f1.y_modal <= -1100);
    printf("   nach No: Flag(9,%d)=%d, Prop sichtbar=%d, Item 0x40 im Inventar=%d\n",
           RE15_SICHERUNG_TAKEN_BIT, re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT),
           sicherung_sichtbar(), anzahl_im_inventar(RE15_SICHERUNG_ITEM));
    pruefe(3, "nach No: kein Genommen-Flag, Prop sichtbar, Item nicht im Inventar",
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) == 0 && sicherung_sichtbar()
           && anzahl_im_inventar(RE15_SICHERUNG_ITEM) == 0);
    pruefe(4, "Fahrt 1 endet in der Parklage y=-20224 (Pos_set @0x109E)",
           f1.bild_park >= 0 && f1.y_park == PARK_NACH_SZENE);

    printf("  -- Fahrt 2 (Antwort Yes)\n");
    fahrt_t f2 = fahrt(ANTWORT_JA);
    pruefe(5, "Fahrt 2 oeffnet wieder genau EIN Modal", f2.modale == 1);
    printf("   nach Yes: Flag(9,%d)=%d, Prop sichtbar=%d, Item 0x40 im Inventar=%d (Platz %d)\n",
           RE15_SICHERUNG_TAKEN_BIT, re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT),
           sicherung_sichtbar(), anzahl_im_inventar(RE15_SICHERUNG_ITEM),
           re15_inv_find_item(RE15_SICHERUNG_ITEM));
    pruefe(6, "nach Yes: Flag (9,53), Prop weg, Item 0x40 im Inventar",
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) == 1 && !sicherung_sichtbar()
           && anzahl_im_inventar(RE15_SICHERUNG_ITEM) == 1);

    printf("  -- Fahrt 3\n");
    fahrt_t f3 = fahrt(ANTWORT_JA);
    pruefe(7, "Fahrt 3 oeffnet KEIN Modal, Item 0x40 genau einmal im Inventar",
           f3.modale == 0 && anzahl_im_inventar(RE15_SICHERUNG_ITEM) == 1
           && !sicherung_sichtbar());
    return 0;
}

static int fall_b(re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Fall B: \"Yes\" in Fahrt 1 ==\n", rid);
    raum_frisch(rdt, rid);
    if (!sicherung_sichtbar()) { pruefe(8, "Sicherung ist nach dem Raumstart angelegt", 0); return 1; }

    printf("  -- Fahrt 1 (Antwort Yes)\n");
    fahrt_t f1 = fahrt(ANTWORT_JA);
    pruefe(8, "Fahrt 1: genau EIN Modal, danach Flag, Prop weg, Item im Inventar",
           f1.modale == 1 && re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) == 1
           && !sicherung_sichtbar() && anzahl_im_inventar(RE15_SICHERUNG_ITEM) == 1);

    printf("  -- Fahrt 2\n");
    fahrt_t f2 = fahrt(ANTWORT_JA);
    pruefe(9, "Fahrt 2: KEIN Modal, keine zweite Aufnahme, Prop bleibt weg",
           f2.modale == 0 && anzahl_im_inventar(RE15_SICHERUNG_ITEM) == 1
           && !sicherung_sichtbar());
    return 0;
}

static int fall_c(re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Fall C: Inventar voll in Fahrt 1, ein Platz frei in Fahrt 2 ==\n", rid);
    raum_frisch(rdt, rid);
    if (!sicherung_sichtbar()) { pruefe(10, "Sicherung ist nach dem Raumstart angelegt", 0); return 1; }
    /* alle Plaetze mit einem Nicht-Waffen-Item belegen (0x41 "Spark Plug"; der Waffenzweig
     * @0x8001df40 greift nur fuer Ids 0x0e..0x13) */
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) { g_inv.slots[i].id = 0x41; g_inv.slots[i].qty = 1; }

    printf("  -- Fahrt 1 (Inventar voll)\n");
    fahrt_t f1 = fahrt(ANTWORT_JA);
    printf("   nach Fahrt 1: Flag(9,%d)=%d, Prop sichtbar=%d, Item 0x40 im Inventar=%d\n",
           RE15_SICHERUNG_TAKEN_BIT, re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT),
           sicherung_sichtbar(), anzahl_im_inventar(RE15_SICHERUNG_ITEM));
    pruefe(10, "Fahrt 1 (voll): genau EIN Modal, nichts genommen, Prop sichtbar",
           f1.modale == 1 && re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) == 0
           && sicherung_sichtbar() && anzahl_im_inventar(RE15_SICHERUNG_ITEM) == 0);

    g_inv.slots[3].id = 0; g_inv.slots[3].qty = 0;
    printf("  -- Fahrt 2 (Platz 3 frei, Antwort Yes)\n");
    fahrt_t f2 = fahrt(ANTWORT_JA);
    pruefe(11, "Fahrt 2: das Modal geht wieder auf, Yes nimmt die Sicherung (Platz 3)",
           f2.modale == 1 && re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) == 1
           && !sicherung_sichtbar() && re15_inv_find_item(RE15_SICHERUNG_ITEM) == 3);
    return 0;
}

static int ein_raum(uint16_t rid, const char *datei)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/%s", base, datei);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "RDT nicht lesbar: %s\n", path); return 77; }
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { fprintf(stderr, "parse fail %s\n", datei); free(buf); return 1; }
    fall_a(&rdt, rid);
    fall_b(&rdt, rid);
    fall_c(&rdt, rid);
    free(buf);
    return 0;
}

int main(void)
{
    int a = ein_raum(0x1150, "ROOM1150.RDT");
    if (a == 77) return 77;
    int b = ein_raum(0x1151, "ROOM1151.RDT");
    if (b == 77) return 77;
    printf("\n%s — %d Pruefung(en) gerissen\n",
           fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? erste_nummer : 0;
}
