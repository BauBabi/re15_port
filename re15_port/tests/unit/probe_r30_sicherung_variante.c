/* probe_r30_sicherung_variante.c — MESS-VARIANTE der Sicherung im Hebetisch (Runde 30, Thema H).
 *
 * ⛔ KEIN SPIELCODE. Diese Datei wird NUR in das Mess-Ziel `re15_pc_r30_sicherung` gelinkt
 * (probes/r30_sicherung.cmake, Schalter RE15_R30_SICHERUNG_VARIANTE=ON). Sie definiert
 * dieselben vier Symbole wie engine/src/sicherung_1150.c; weil sie als OBJEKT vor dem
 * statischen Archiv re15_engine steht, zieht der Linker das Archivmitglied
 * sicherung_1150.c.obj gar nicht erst — engine/, platform/ und include/ bleiben unberuehrt.
 *
 * WOZU: Die Ermittlung (analysis/befunde_runde30/sicherung.md) muss Sitz, Drehung und
 * Modell der Sicherung im ECHTEN Renderer pruefen koennen (Framedump), bevor ein
 * Bau-Agent irgendeine Zahl in den Spielcode schreibt. Alles, was hier variiert wird,
 * kommt aus der Umgebung — ohne Variablen verhaelt sich die Variante exakt wie der
 * Bestand (Sitz -628,-927,784, Drehung 0, eingebackenes Modell, Fenster -5000..-1100).
 *
 *   RE15_R30_SITZ="x,y,z[,rx,ry,rz]"   Sitz in PLATTFORM-Koordinaten + Prop-Drehung (0..4095)
 *   RE15_R30_OHNE_MODAL=1              das Item-Modal NICHT oeffnen (Fahrt bis oben ansehen)
 *   RE15_R30_MODAL_AB=<y>              obere Schranke des Modal-Fensters (Bestand -1100)
 *   RE15_R30_MD1=<datei> / RE15_R30_TIM=<datei>   anderes Modell/andere Textur (roh MD1/TIM)
 *   RE15_R30_NACHINSTALL=1             (Fortsetzungs-Agent) legt das Prop im ersten Spielbild
 *                                      nach, falls der Raumstart es NICHT angelegt hat. Damit
 *                                      laesst sich am LADE-Weg (CONTINUE bootet nicht durch
 *                                      scd_room_reenter, main.c:3989-4119) messen, ob Modell
 *                                      und Textur dort bereitstehen, OHNE main.c anzufassen.
 *   RE15_R30_SLOT0=1                   (Fortsetzungs-Agent) legt Item 0x40 im ersten Spielbild in
 *                                      Inventarplatz 0. Die Inventar-Abnahmeschienen
 *                                      (RE15_INV_GRID_SHOT / RE15_INV_CHECK_SHOT, main.c:4325-4372)
 *                                      arbeiten fest auf Platz 0 — so zeigen sie die Sicherung.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_sicherung.h"
#include "re15_scd.h"
#include "re15_item_modal.h"
#include "re15_room.h"
#include "re15_inventory.h"

#include "gen/sicherung_prop.inc"

#define PLATTFORM_OBJ_ID   0
#define IM_RAUM_AB         (-5000)    /* Bestand sicherung_1150.c:27 */
#define OBEN_BIS           (-1100)    /* Bestand sicherung_1150.c:28 */

static uint8_t s_raum_aktiv;
static uint8_t s_modal_ausgeloest;

static uint8_t *datei(const char *env, int *out_size)
{
    const char *p = getenv(env);
    if (!p || !*p) return NULL;
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "[r30-sicherung] %s=%s NICHT lesbar\n", env, p); return NULL; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (sz > 0) ? (uint8_t *)malloc((size_t)sz) : NULL;
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) { *out_size = (int)sz;
             fprintf(stderr, "[r30-sicherung] %s=%s (%ld B)\n", env, p, sz); }
    return b;
}

const uint8_t *re15_sicherung_md1_bytes(int *out_size)
{
    static uint8_t *s_b = NULL; static int s_n = 0, s_tried = 0;
    if (!s_tried) { s_tried = 1; s_b = datei("RE15_R30_MD1", &s_n); }
    if (s_b) { if (out_size) *out_size = s_n; return s_b; }
    if (out_size) *out_size = (int)sizeof(re15_sicherung_md1);
    return re15_sicherung_md1;
}

const uint8_t *re15_sicherung_tim_bytes(int *out_size)
{
    static uint8_t *s_b = NULL; static int s_n = 0, s_tried = 0;
    if (!s_tried) { s_tried = 1; s_b = datei("RE15_R30_TIM", &s_n); }
    if (s_b) { if (out_size) *out_size = s_n; return s_b; }
    if (out_size) *out_size = (int)sizeof(re15_sicherung_tim);
    return re15_sicherung_tim;
}

static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

void re15_sicherung_install(uint16_t room_id)
{
    s_raum_aktiv = 0;
    s_modal_ausgeloest = 0;
    if (!(room_id == 0x1150 || room_id == 0x1151)) return;
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return;
    if (slot_von_obj_id(PLATTFORM_OBJ_ID) < 0) return;
    if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return;
    if (slot_von_obj_id(RE15_SICHERUNG_OBJ_ID) >= 0) return;

    int sx = RE15_SICHERUNG_POS_X, sy = RE15_SICHERUNG_POS_Y, sz = RE15_SICHERUNG_POS_Z;
    int rx = 0, ry = 0, rz = 0;
    { const char *e = getenv("RE15_R30_SITZ");
      if (e && *e) sscanf(e, "%d,%d,%d,%d,%d,%d", &sx, &sy, &sz, &rx, &ry, &rz); }

    int i = (int)g_scd.prop_count++;
    g_scd.props[i].active   = 1;
    g_scd.props[i].obj_id   = RE15_SICHERUNG_OBJ_ID;
    g_scd.props[i].obj_type = 0;
    g_scd.props[i].band     = 1;
    g_scd.props[i].parent_obj = (int8_t)PLATTFORM_OBJ_ID;
    g_scd.props[i].x = sx; g_scd.props[i].y = sy; g_scd.props[i].z = sz;
    g_scd.props[i].rot_x = (int16_t)rx;
    g_scd.props[i].rot_y = (int16_t)ry;
    g_scd.props[i].rot_z = (int16_t)rz;
    g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
    g_scd.props[i].vel_ry = 0;
    g_scd.props[i].flags = 0x0001;
    g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
    g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
    g_scd.props[i].member_0b = 0;
    s_raum_aktiv = 1;
    fprintf(stderr, "[r30-sicherung] Prop slot=%d Sitz=(%d,%d,%d) rot=(%d,%d,%d)\n",
            i, sx, sy, sz, rx, ry, rz);
}

int re15_sicherung_tick(void)
{
    static int s_init = 0, s_ohne = 0, s_oben_bis = OBEN_BIS;
    if (!s_init) {
        s_init = 1;
        s_ohne = getenv("RE15_R30_OHNE_MODAL") != NULL;
        { const char *e = getenv("RE15_R30_MODAL_AB"); if (e && *e) s_oben_bis = atoi(e); }
    }
    { static int s_slot0 = 0;
      if (!s_slot0 && getenv("RE15_R30_SLOT0")) {
          /* Wert 1 = die Sicherung; jeder andere Wert (z.B. 0x24) = dieses Item. Damit laesst
           * sich die Gegenprobe "zeigt der CHECK-Schirm bei ANDEREN Bloecken ein Foto?" fahren. */
          long id = strtol(getenv("RE15_R30_SLOT0"), NULL, 0);
          if (id <= 1 || id > 255) id = RE15_SICHERUNG_ITEM;
          s_slot0 = 1;
          g_inv.slots[0].id  = (uint8_t)id;
          g_inv.slots[0].qty = 1;
          fprintf(stderr, "[r30-sicherung] SLOT0: Item 0x%02lX in Inventarplatz 0\n", id);
      } }
    if (!s_raum_aktiv && getenv("RE15_R30_NACHINSTALL") &&
        (g_current_room_id == 0x1150 || g_current_room_id == 0x1151) &&
        slot_von_obj_id(RE15_SICHERUNG_OBJ_ID) < 0) {
        re15_sicherung_install((uint16_t)g_current_room_id);
        fprintf(stderr, "[r30-sicherung] NACHINSTALL im Spielbild: raum_aktiv=%d\n",
                (int)s_raum_aktiv);
    }
    if (s_ohne) return 0;
    if (!s_raum_aktiv || s_modal_ausgeloest) return 0;
    if (re15_item_modal_active()) return 0;
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return 0;
    int p = slot_von_obj_id(PLATTFORM_OBJ_ID);
    if (p < 0 || !g_scd.props[p].active) return 0;
    int32_t py = g_scd.props[p].y;
    if (py <= IM_RAUM_AB || py > s_oben_bis) return 0;
    re15_item_modal_start(RE15_SICHERUNG_ITEM, 1, RE15_SICHERUNG_TAKEN_BIT,
                          -1, RE15_SICHERUNG_OBJ_ID);
    s_modal_ausgeloest = 1;
    fprintf(stderr, "[r30-sicherung] Modal bei Plattform y=%d\n", (int)py);
    return 1;
}
