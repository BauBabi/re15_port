/*
 * RE1.5 Rebuilt — die SICHERUNG im Hebetisch von Irons' Buero.
 *
 * Herleitung, Belege und alle Konstanten: include/re15_sicherung.h.
 * Modell + Textur: tools/sicherung_modell.py -> tools/sicherung_engine_export.py
 * -> gen/sicherung_prop.inc (eingebackene MD1/TIM-Bytes, kein Asset-Patch).
 */
#include "re15_sicherung.h"

#include "re15_scd.h"
#include "re15_item_modal.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include "re15_inventory.h"   /* nur fuer die Logzeile: Inventar-Platz nach "Yes" */
#endif

#include "gen/sicherung_prop.inc"

/* Der Hebetisch selbst ist Prop obj_id 0 des Raums (main00 @0x0E00). */
#define PLATTFORM_OBJ_ID   0

/* ⛔ FENSTER, NICHT EINSEITIGE SCHRANKE. Die Plattform muss IM RAUM *und* OBEN sein.
 * Eine blosse Schranke `y <= -1100` reicht NICHT: die Parkposition y=-20324
 * (main00 @0x0E00) unterschreitet sie ebenfalls, und das Modal ginge dann schon beim
 * Betreten des Raums auf, bevor der Tisch ueberhaupt erscheint. Genau das hat die Sonde
 * probe_sicherung_1150 gemessen — "Modal aufgemacht in Bild 0".
 * Gemessene Fahrt (dieselbe Sonde): geparkt -20324; Pos_set @0x0FB4 holt sie auf -305;
 * der Hochpunkt ist -1215 (91 Schritte a -10, For @0x0FF6), danach steht sie auf -1205
 * (10 Schritte a +1, For @0x1010); zurueck auf -305 und Parklage -20224 (Pos_set
 * @0x109E). Das Fenster faengt nur die Aufwaertsfahrt oben.
 * ⛔ Beide Schranken sind PORT-WAHL, KEINE ORIGINAL-ADRESSE — das Original oeffnet im
 * Hebetisch kein Modal. Sie trennen drei gemessene Lagen (Parken <= -20224, Start -305,
 * oben <= -1205), mehr nicht. */
#define IM_RAUM_AB         (-5000)    /* alles darunter ist die Parkposition   */
#define OBEN_BIS           (-1100)    /* Hochpunkt -1215, Stand -1205, Start -305 */

static uint8_t s_raum_aktiv;        /* Prop in diesem Raum angelegt?              */

/* ⛔ SPERRE JE FAHRT, NICHT JE RAUMAUFENTHALT (Runde 30, Nachschliff "sicherung-nein",
 * Dossier analysis/befunde_runde30/nachschliff-sicherung-nein.md).
 * Bis cac33993 wurde die Sperre nur in re15_sicherung_install zurueckgesetzt: wer mit "No"
 * antwortete, bekam die Sicherung im selben Aufenthalt nie wieder angeboten (Gegenpruefer
 * g2/taste_nein, Sonde: Fahrt 2 -> 0 Modale).
 * ORIGINAL-REGEL fuer eine abgelehnte Aufnahme — die Zone bleibt scharf:
 *   RE1.5 FUN_8001db28 Zustand 7: nur der JA-Zweig nullt das sce-Byte der ausloesenden
 *         Zone (`sb zero,0(v1)` @0x8001e090, v1 = [0x800aca30]); "No" (@0x8001e068
 *         `andi v0,v0,0x1` -> @0x8001e06c `bne`) springt nach @0x8001e0ec -> Zustand 8 ->
 *         Zustand 0 (@0x8001e16c) und fasst die Zone nicht an. Der Scan ueberspringt nur
 *         sce-0-Datensaetze (@0x80042f48 / @0x80042f50) -> erneutes Untersuchen oeffnet das
 *         Modal wieder. Der Handler startet nur aus Zustand 0 (@0x80043334): EIN Ausloesen =
 *         hoechstens EIN Modal.
 *   RE2   dasselbe: "No" (@0x800720c8 / @0x800720cc) -> Zustand 4..18 (@0x800723f0), Zone
 *         unberuehrt; nur der Ja-Weg nullt sie (`sb zero,0(v1)` @0x80072298).
 * Das Ausloesen der Sicherung ist die FAHRT von sub04 (@0x0F96-0x10B6). Also: hoechstens ein
 * Modal je Fahrt, und die NAECHSTE Fahrt bietet eine abgelehnte Sicherung wieder an.
 * WIEDER SCHARF, sobald die Plattform in der Parklage liegt — ⛔ PORT-WAHL, KEINE
 * ORIGINAL-ADRESSE (das Original hat im Hebetisch kein Modal). Abgeleitet aus der
 * gemessenen Fahrt (Sonde probe_r30_sicherung_nein, MESS-Zeilen, ROOM1150 = ROOM1151):
 * waehrend einer Fahrt bleibt y in [-1215, -301] (Pos_set @0x0FB4 -305, For @0x0FF6 91 x -10,
 * For @0x1010 10 x +1, For @0x1042 90 x +10, @0x105C 2 x +2, @0x106A 2 x -2); am Ende setzt
 * Pos_set @0x109E die Parklage y = -20224 (gemessen Bild 484 nach "No", 395 ohne Modal).
 * Die Parklage unterschreitet IM_RAUM_AB, eine Fahrt nie — die Sperre faellt also genau
 * einmal je Fahrt, und zwar nach deren Ende. */
static uint8_t s_modal_ausgeloest;  /* in DIESER FAHRT schon aufgemacht?          */
#ifdef RE15_PLATFORM_PC
static uint8_t s_modal_offen;       /* nur Logzeile: unser Modal laeuft gerade     */
#endif


const uint8_t *re15_sicherung_md1_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_sicherung_md1);
    return re15_sicherung_md1;
}

const uint8_t *re15_sicherung_tim_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_sicherung_tim);
    return re15_sicherung_tim;
}

static int ist_irons_buero(uint16_t room_id)
{
    return room_id == 0x1150 || room_id == 0x1151;   /* Elza- und John-Variante */
}

/* Pool-Index eines Props ueber seine obj_id (dieselbe Suche wie pc_prop_world). */
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
#ifdef RE15_PLATFORM_PC
    s_modal_offen = 0;
#endif

    if (!ist_irons_buero(room_id)) return;
    /* Schon genommen -> gar nicht erst anlegen (dieselbe Regel, die auch das Original
     * fuer aufgenommene Welt-Modelle fuehrt, s. s_prop_taken_hidden in scd_vm.c). */
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return;
    /* Ohne die Plattform gibt es nichts, woran sich die Sicherung haengen koennte. */
    if (slot_von_obj_id(PLATTFORM_OBJ_ID) < 0) return;
    if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return;
    if (slot_von_obj_id(RE15_SICHERUNG_OBJ_ID) >= 0) return;   /* schon da */

    int i = (int)g_scd.prop_count++;
    g_scd.props[i].active   = 1;
    g_scd.props[i].obj_id   = RE15_SICHERUNG_OBJ_ID;
    g_scd.props[i].obj_type = 0;      /* Mesh-Zweig von FUN_8002c18c (kein Sprite-Gitter) */
    g_scd.props[i].band     = 1;      /* Etagenband wie die vier Props des Raums          */
    /* ⛔ DAS IST DER KERN: die Sicherung haengt an der Elternmatrix der Plattform —
     * genau die Anhaenge-Form (Obj_model_set pc[5] = 0xC0), die im ganzen Spiel nur
     * die beiden Deckelhaelften dieses Tisches benutzen. Dadurch faehrt sie mit,
     * statt in der Luft stehen zu bleiben. */
    g_scd.props[i].parent_obj = (int8_t)PLATTFORM_OBJ_ID;
    g_scd.props[i].x = RE15_SICHERUNG_POS_X;
    g_scd.props[i].y = RE15_SICHERUNG_POS_Y;
    g_scd.props[i].z = RE15_SICHERUNG_POS_Z;
    g_scd.props[i].rot_x = 0;         /* liegend ist schon im Modell (Laengsachse X) */
    /* Viertelkreis: Laengsachse entlang der langen Seite des Kuppelfachs. PORT-WAHL,
     * keine Original-Adresse — Herleitung aus der Tischgeometrie in re15_sicherung.h. */
    g_scd.props[i].rot_y = RE15_SICHERUNG_ROT_Y;
    g_scd.props[i].rot_z = 0;
    g_scd.props[i].vel_x = g_scd.props[i].vel_y = g_scd.props[i].vel_z = 0;
    g_scd.props[i].vel_ry = 0;
    g_scd.props[i].flags = 0x0001;    /* aktiv, NICHT kletterbar (kein 0x100)        */
    g_scd.props[i].box_cx = g_scd.props[i].box_cy = g_scd.props[i].box_cz = 0;
    g_scd.props[i].box_hx = g_scd.props[i].box_hy = g_scd.props[i].box_hz = 0;
                                      /* Nullbox = nicht kollidierbar: der Gegenstand
                                       * liegt auf der Plattform, er ist kein Hindernis */
    g_scd.props[i].member_0b = 0;
    s_raum_aktiv = 1;
}

/* Runde 30, Nachtrag K (Granate in derselben Fahrt, include/re15_granate.h): kann die Sicherung
 * in DIESER Fahrt noch ein Modal aufmachen? Die Granate wartet, solange das so ist ("erst
 * Sicherung, dann Granate"). Reines Lesen derselben Sperren, die re15_sicherung_tick prueft.
 * ⛔ Port-Wahl, keine Original-Adresse (das Original hat im Hebetisch keine Beute). */
int re15_sicherung_fahrt_offen(void)
{
    if (!s_raum_aktiv || s_modal_ausgeloest) return 0;
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return 0;
    int p = slot_von_obj_id(RE15_SICHERUNG_OBJ_ID);
    return p >= 0 && g_scd.props[p].active;
}

int re15_sicherung_tick(void)
{
    if (!s_raum_aktiv) return 0;
    int p = slot_von_obj_id(PLATTFORM_OBJ_ID);
    if (p < 0) return 0;
    /* +Y zeigt nach unten: OBEN heisst KLEINERES y. */
    int32_t py = g_scd.props[p].y;

#ifdef RE15_PLATFORM_PC
    /* Protokoll fuer den Nutzer (befund.log) und die Abnahme: wie ging unser Modal aus? */
    if (s_modal_offen && !re15_item_modal_active()) {
        s_modal_offen = 0;
        if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT))
            fprintf(stderr, "[sicherung] Yes: genommen, Flag (9,%d) gesetzt, Item 0x%02X in "
                            "Inventar-Platz %d\n", RE15_SICHERUNG_TAKEN_BIT, RE15_SICHERUNG_ITEM,
                    re15_inv_find_item(RE15_SICHERUNG_ITEM));
        else
            /* derselbe Zweig fuer "No" und "Inventar voll": @0x8001e054 `bltz` /
             * @0x8001e06c `bne` -> @0x8001e0ec */
            fprintf(stderr, "[sicherung] No/voll: nicht genommen, Sicherung bleibt liegen "
                            "(Hebetisch y=%d), die naechste Fahrt bietet sie wieder an\n",
                    (int)py);
    }
#endif

    /* WIEDER SCHARF JE FAHRT: Parklage = die Fahrt ist zu Ende (Pos_set @0x109E, s.o.).
     * ⛔ Port-Wahl, keine Original-Adresse — die Begruendung steht bei s_modal_ausgeloest. */
    if (py <= IM_RAUM_AB) {
        if (s_modal_ausgeloest) {
            s_modal_ausgeloest = 0;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[sicherung] Fahrt zu Ende, Hebetisch in der Parklage y=%d: "
                            "Sperre geloest\n", (int)py);
#endif
        }
        return 0;
    }
    if (s_modal_ausgeloest) return 0;          /* hoechstens EIN Modal je Fahrt (@0x80043334) */
    if (re15_item_modal_active()) return 0;
    if (re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT)) return 0;
    if (!g_scd.props[p].active) return 0;
    /* Fenster (IM_RAUM_AB, OBEN_BIS]: beide Seiten pruefen (s.o.). */
    if (py > OBEN_BIS) return 0;

    /* aot_slot -1 = es gibt keine AOT-Zone, die abgeschaltet werden muesste (die
     * Uebergabe kommt aus der Szene, nicht aus einem Aufsammel-Rechteck).
     * taken_bit setzt das Zone-9-Flag, taken_prop blendet das Welt-Modell aus —
     * beides erledigt das Modal beim Bestaetigen (item_modal_common.c:340/360). */
    re15_item_modal_start(RE15_SICHERUNG_ITEM, 1, RE15_SICHERUNG_TAKEN_BIT,
                          -1, RE15_SICHERUNG_OBJ_ID);
    s_modal_ausgeloest = 1;
#ifdef RE15_PLATFORM_PC
    s_modal_offen = 1;
    fprintf(stderr, "[sicherung] Modal auf (Hebetisch y=%d), Sperre bis zum Ende dieser Fahrt\n",
            (int)py);
#endif
    return 1;
}
