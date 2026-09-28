/* probe_sicherung_1150.c — ABNAHME der Sicherung im Hebetisch von Irons' Buero.
 *
 * Nicht "Build ist gruen": diese Sonde faehrt den Ablauf wirklich und misst.
 *   1. ROOM1150 hochfahren und pruefen, dass das ZUSAETZLICHE Prop angelegt ist
 *      (obj_id 4, parent_obj 0, Sitz im Kuppelfach).
 *   2. sub04 ausloesen (die Hebetisch-Sequenz) und die Fahrt Bild fuer Bild
 *      protokollieren: y der Plattform UND die daraus verkettete Weltlage der
 *      Sicherung — faehrt sie wirklich mit, oder bleibt sie stehen?
 *   3. Pruefen, dass das Item-Modal oben aufgeht.
 *   4. Das Modal bestaetigen und pruefen, dass danach das Zone-9-Flag steht und
 *      das Welt-Modell verschwunden ist.
 *   5. Gegenprobe: Raum neu betreten -> die Sicherung darf NICHT wieder da sein.
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
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

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

/* Die Elternketten-Verkettung des Renderers (pc_prop_world, main.c:568-577) fuer den
 * EINEN Fall, den wir hier brauchen: Kind mit parent_obj, beide ohne X/Z-Drehung ausser
 * der rot_y der Plattform. Liefert die Weltlage der Sicherung. */
static void welt_von_kind(int kind, int eltern, long out[3])
{
    /* rot_y der Plattform in Q12-Sinus/Cosinus — dieselbe Tabelle wie die Engine
     * (re15_skeleton.h). */
    int32_t s = (int32_t)re15_sin_q12((int)g_scd.props[eltern].rot_y);
    int32_t c = (int32_t)re15_cos_q12((int)g_scd.props[eltern].rot_y);
    int32_t lx = g_scd.props[kind].x, ly = g_scd.props[kind].y, lz = g_scd.props[kind].z;
    out[0] = (long)(((int64_t)c * lx + (int64_t)s * lz) >> 12) + g_scd.props[eltern].x;
    out[1] = (long)ly + g_scd.props[eltern].y;
    out[2] = (long)(((int64_t)-s * lx + (int64_t)c * lz) >> 12) + g_scd.props[eltern].z;
}

/* ⛔ `frisch` trennt BOOT von WIEDEREINTRITT. scd_vm_init() ruft re15_game_state_init()
 * (scd_vm.c:495) und loescht damit ALLE Flagbaenke — ein Raumwechsel im echten Spiel tut
 * das NICHT (scd_room_setup.c:109: "scd_vm_init MINUS re15_game_state_init (keep
 * flags)"). Der erste Anlauf dieser Sonde rief es auch beim Wiedereintritt und hat sich
 * damit das Genommen-Flag selbst weggewischt — die gemeldete "Sicherung liegt wieder da"
 * war ein Sondenfehler, kein Portfehler. */
static void raum_hochfahren(re15_rdt_t *rdt, int32_t px, int32_t pz, int frisch)
{
    if (frisch) { scd_vm_init(); re15_actor_init(); }
    g_current_room_id = 0x1150;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt->messages, rdt->messages_size); }
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, px, pz, 0);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1150.RDT", base);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "RDT nicht lesbar: %s\n", path); return 77; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { fprintf(stderr, "parse fail\n"); return 1; }

    re15_game_state_init();
    /* Spielerposition: Westseite des Mitteltisches (der autorisierte Ausloeser-Record
     * slot 1 @0x0D7E, rect=(-21800,-20000,1500,3300)). */
    raum_hochfahren(&rdt, -21000, -18500, 1);

    printf("\n== 1. Prop-Anlage ==\n");
    printf("   prop_count=%d\n", (int)g_scd.prop_count);
    int si = slot_von_obj(RE15_SICHERUNG_OBJ_ID);
    int pi = slot_von_obj(0);
    pruefe(1, "Sicherung als Prop angelegt", si >= 0);
    pruefe(2, "Plattform (obj 0) vorhanden", pi >= 0);
    if (si < 0 || pi < 0) { printf("\nAbbruch.\n"); return erste_nummer ? erste_nummer : 1; }
    printf("   Sicherung: slot=%d obj_id=%d parent=%d pos=(%ld,%ld,%ld) active=%d\n",
           si, g_scd.props[si].obj_id, g_scd.props[si].parent_obj,
           (long)g_scd.props[si].x, (long)g_scd.props[si].y, (long)g_scd.props[si].z,
           g_scd.props[si].active);
    printf("   Plattform: slot=%d pos=(%ld,%ld,%ld) rot_y=%d\n",
           pi, (long)g_scd.props[pi].x, (long)g_scd.props[pi].y,
           (long)g_scd.props[pi].z, g_scd.props[pi].rot_y);
    pruefe(3, "haengt an der Elternmatrix der Plattform",
           g_scd.props[si].parent_obj == 0);
    pruefe(4, "sitzt auf dem Boden des Kuppelfachs (y=-1062 = -1036 - 26)",
           g_scd.props[si].y == RE15_SICHERUNG_POS_Y);
    pruefe(5, "ist sichtbar", g_scd.props[si].active == 1);

    printf("\n== 2. Hebetisch ausloesen (sub04) und die Fahrt messen ==\n");
    long w0[3]; welt_von_kind(si, pi, w0);
    printf("   vor dem Start: Plattform y=%ld, Sicherung Welt=(%ld,%ld,%ld)\n",
           (long)g_scd.props[pi].y, w0[0], w0[1], w0[2]);

    scd_event_fire(4);
    long y_min = 0x7fffffff, y_max = -0x7fffffff;
    long sich_y_bei_min = 0;
    int  modal_bild = -1;
    int  mitgefahren = 1;
    long letzte_diff = (long)g_scd.props[si].y;   /* y-Abstand Kind<->Eltern ist konstant */
    for (int f = 0; f < 400; f++) {
        scd_vm_tick();
        if (!re15_item_modal_active()) {
            if (re15_sicherung_tick() && modal_bild < 0) modal_bild = f;
        }
        int p = slot_von_obj(0), s2 = slot_von_obj(RE15_SICHERUNG_OBJ_ID);
        if (p < 0) continue;
        long py = (long)g_scd.props[p].y;
        /* ⛔ NUR die Fahrt IM RAUM messen. Die Parkposition y=-20324 ist kein Hochpunkt;
         * sie in y_min mitzuzaehlen machte den "Hub" zu 20023 und den Hochpunkt zu
         * -20324 — der erste Anlauf dieser Sonde hat sich daran selbst getaeuscht. */
        if (py > -5000) {
            if (py < y_min) {
                y_min = py;
                if (s2 >= 0) { long w[3]; welt_von_kind(s2, p, w); sich_y_bei_min = w[1]; }
            }
            if (py > y_max) y_max = py;
        }
        /* Mitfahren = der LOKALE Abstand bleibt konstant; die Weltlage folgt dann
         * zwangslaeufig der Plattform (Verkettung nt = pr*t + pt). */
        if (s2 >= 0 && (long)g_scd.props[s2].y != letzte_diff) mitgefahren = 0;
        if (f < 3 || (f % 40) == 0) {
            long w[3]; if (s2 >= 0) welt_von_kind(s2, p, w); else w[0]=w[1]=w[2]=0;
            printf("   Bild %3d: Plattform y=%6ld   Sicherung Welt y=%6ld %s\n",
                   f, py, w[1], (s2 < 0 ? "(Prop weg)" : ""));
        }
    }
    printf("   Plattform-Hub: y von %ld bis %ld (Hub %ld)\n", y_max, y_min, y_max - y_min);
    pruefe(6, "die Plattform faehrt ueberhaupt (Hub > 500)", (y_max - y_min) > 500);
    pruefe(7, "die Plattform kommt oben ueber -1100 hinaus", y_min <= -1100);
    pruefe(8, "die Sicherung faehrt mit (lokaler Abstand konstant)", mitgefahren);
    printf("   Sicherung-Welt-y am Hochpunkt: %ld (Plattform %ld)\n",
           sich_y_bei_min, y_min);
    /* Bezug ist die Lage IM RAUM (Plattform auf -305 => Sicherung auf -1232), nicht
     * die Parkposition -21251: sonst vergleicht man gegen einen Ort ausserhalb des Raums. */
    pruefe(9, "die Sicherung ist oben mit angehoben",
           sich_y_bei_min < (-305L + RE15_SICHERUNG_POS_Y) - 500L);
    printf("   Modal aufgemacht in Bild %d\n", modal_bild);
    /* ⛔ NICHT NUR "geht auf", sondern "geht OBEN auf". Der erste Anlauf pruefte nur
     * modal_bild >= 0 und war deshalb blind dafuer, dass das Modal schon in Bild 0
     * aufging — bei geparkter Plattform, weit vor der Fahrt. */
    pruefe(10, "das Item-Modal geht auf", modal_bild >= 0);
    printf("   (die Aufwaertsfahrt beginnt fruehestens ab Bild 40)\n");
    pruefe(16, "und zwar NICHT schon beim Betreten des Raums", modal_bild > 40);
    pruefe(11, "das Modal laeuft", re15_item_modal_active());

    printf("\n== 3. Modal bestaetigen ==\n");
    for (int f = 0; f < 4000 && re15_item_modal_active(); f++)
        re15_item_modal_tick(0x4000, 0x4000);     /* SQUARE = bestaetigen */
    pruefe(12, "das Modal ist danach zu", !re15_item_modal_active());
    int flag = re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT);
    printf("   Zone-9-Flag %d = %d\n", RE15_SICHERUNG_TAKEN_BIT, flag);
    pruefe(13, "das Genommen-Flag steht", flag == 1);
    int si2 = slot_von_obj(RE15_SICHERUNG_OBJ_ID);
    printf("   Prop nach der Aufnahme: slot=%d active=%d\n",
           si2, si2 >= 0 ? g_scd.props[si2].active : -1);
    pruefe(14, "das Welt-Modell ist ausgeblendet",
           si2 < 0 || g_scd.props[si2].active == 0);

    printf("\n== 4. Gegenprobe: Raum neu betreten ==\n");
    printf("   Flag VOR dem Wiedereintritt:  %d\n",
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT));
    raum_hochfahren(&rdt, -21000, -18500, 0);
    printf("   Flag NACH dem Wiedereintritt: %d\n",
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT));
    int si3 = slot_von_obj(RE15_SICHERUNG_OBJ_ID);
    printf("   prop_count=%d, Sicherungs-Slot=%d, active=%d\n",
           (int)g_scd.prop_count, si3,
           si3 >= 0 ? g_scd.props[si3].active : -1);
    pruefe(15, "die Sicherung liegt NICHT wieder da",
           si3 < 0 || g_scd.props[si3].active == 0);

    printf("\n%s — %d Pruefung(en) gerissen\n",
           fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    free(buf);
    return fehler ? erste_nummer : 0;
}
