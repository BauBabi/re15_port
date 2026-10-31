/*
 * RE1.5 Rebuilt — Runde 41 Spur O "keyitems": Key Items ueber das Inventar in RE2-Form, Kurbel ohne Fliegen.
 * Herleitung und Belege: include/re15_keyitems_r41.h, analysis/befunde_runde41/O_keyitems.md.
 */
#include "re15_keyitems_r41.h"

#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"        /* g_actors: Lage/Blick des Spielers                                  */
#include "re15_aot.h"          /* g_aot, re15_aot_point_in_quad                                      */
#include "re15_collision.h"    /* re15_collision_debug_band = Spieler-Band (+0x82)                   */
#include "re15_damage.h"       /* re15_player_equipped_weapon                                        */
#include "re15_engine.h"       /* g_engine.frame_count: nur Messhaken RE15_USE_TEST                  */
#include "re15_inventory.h"    /* g_inv, re15_inv_find_item, re15_inv_grant, re15_inv_remove_slot    */
#include "re15_item_modal.h"   /* re15_item_modal_start: "You can't carry any more items"            */
#include "re15_itembox.h"      /* g_itembox, RE15_BOX_SLOTS                                          */
#include "re15_menu.h"         /* re15_menu_gameplay_frozen                                          */
#include "re15_msg.h"          /* re15_msg_get_raw                                                   */
#include "re15_rolltor.h"      /* (9,63) Sicherung im Rolltor ROOM1050                               */
#include "re15_scd.h"          /* g_scd, re15_game_flag_*, scd_event_fire                            */
#include "re15_sicherung.h"    /* (9,53) Sicherung vom Hebetisch ROOM1150 genommen                   */
#include "re15_skeleton.h"     /* re15_cos_q12 / re15_sin_q12                                        */
#include "re15_ventilkette_r41.h"   /* Kasten 2010: Slot, Ereignis, Frage (Spur C)                   */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/keyitems_r41.inc"

extern unsigned g_current_room_id;
extern int scd_event_fire(uint8_t event_id);
extern void re15_player_waffe_roh(int id);     /* re15_damage.c, Runde 41 Spur K: aca5d ohne Ausruest-Platz */

#define KEIN_TOR   0xFF        /* tor_bank: keine Zusatzbedingung */
#define MARKER     0xFF        /* ereignis: sce-5-Platz, die Raum-sub01 fragt die Trefferzelle ab */

/* ---- Die Stellen -------------------------------------------------------------------------------------------------------
 * Abgeleitet aus den RDT-Bytes (Zensus der Message_on mit 0x03-Code + "Will you use"; Riegel unit_r41_keyitems_stellen liest
 * jede Zeile gegen die ausgelieferte RDT). raum = Leon-Raum; Bit 0 (Elza-Variante) zaehlt mit, ausser `nur_der`.
 * Reihenfolge der Felder: Raum, Item, Slot, Ereignis, Frage-Id, art, Tor (Bank, Bit, Wert) = Original-Bedingung der Frage. */
typedef struct {
    uint16_t raum;
    uint8_t  item;
    int8_t   slot;
    uint8_t  ereignis;
    uint8_t  frage;
    uint8_t  art;
    uint8_t  nur_der;          /* 1 = nur genau dieser Raum, die Elza-Variante hat die Stelle nicht */
    uint8_t  tor_bank, tor_bit, tor_wert;
} stelle_t;

static const stelle_t k_stellen[] = {
    /* ROOM1090 Feuerloescher: sub00 @0x02336 Aot_set Slot 2 sce 3 sat 0xB1 (Viereck) -> sub06 @0x02702: msg 7, @0x02708 msg 8 */
    { 0x1090, 0x31, 2, 6, 8, 0, 1, KEIN_TOR, 0, 0 },
    /* Kartenleser: main00 Aot_set sce 3 -> sub20: msg 7 = Text + Frage in EINER Nachricht ("A card reader." | "You have to use
     * the X Keycard and insert four digits to unlock the door." | "Will you operate the card reader?" 03), Ck(9,<bit>,1) ->
     * msg 9 "You've used the ..." -> sub17 Tastenfeld. RE2-Form: Red Card Key ROOM2110 (Dossier). */
    { 0x10D0, 0x38, 1, 20, 7, RE15_R41O_KOMBI, 0, KEIN_TOR, 0, 0 },   /* @0x0103E, sub20 @0x019AE (10D1 @0x01998)           */
    { 0x11E0, 0x39, 4, 20, 7, RE15_R41O_KOMBI, 0, KEIN_TOR, 0, 0 },   /* @0x0158E, sub20 @0x01F96                            */
    { 0x1230, 0x37, 5, 20, 7, RE15_R41O_KOMBI, 0, KEIN_TOR, 0, 0 },   /* @0x00BB0, sub20 @0x01476                            */
    /* Zange: sub00 @0x017D0 Slot 6 -> sub26 @0x020AC msg 10, @0x020B2 msg 11 */
    { 0x11E0, 0x30, 6, 26, 11, 0, 0, KEIN_TOR, 0, 0 },
    /* Minidisc-Spieler: sub00 @0x00B14 Slot 2 -> sub07 @0x00D50 msg 1, @0x00D56 msg 3 */
    { 0x1100, 0x44, 2, 7, 3, 0, 0, KEIN_TOR, 0, 0 },
    /* Sicherung am Rolltor (Port-Programm rolltor_1050.c k_mit): Slot 7 @0x00C22, Frage = Port-Text 20 (2060 msg 4). */
    { 0x1050, 0x40, 7, 2, 20, 0, 0, 9, 63, 0 },                       /* Tor: noch nicht eingesetzt (k_mit, sonst sub02) */
    /* Kasten 2010 (Spur C): Port-Platz, Programm 2010_kasten_frage, Frage = Port-Text 41 (2060 msg 4). */
    { 0x2010, 0x40, RE15_R41C_SLOT_KASTEN, RE15_R41C_EV_KASTEN, RE15_R41C_MSG_KASTEN_FRAGE, RE15_R41O_FLAGITEM, 0, 3, 108, 1 },   /* Tor wie ventilkette_r41.c: sonst 2010_kasten_leer */
    /* KURBEL: sub00 @0x009AE Slot 3 -> sub02 @0x00A5C msg 0, @0x00A62 msg 1, @0x00A6C Ck(12,31,0). */
    { 0x2020, RE15_R41O_KURBEL_ITEM, 3, 2, 1,
      RE15_R41O_SOFORT | RE15_R41O_WAFFE | RE15_R41O_FLAGITEM, 0, KEIN_TOR, 0, 0 },
    /* Zeitbombe: sub00 @0x01456 Slot 8 -> sub07 @0x016E8 msg 0, @0x016EE msg 5 */
    { 0x2040, RE15_R41O_BOMBE_ITEM, 8, 7, 5, RE15_R41O_FLAGITEM, 0, KEIN_TOR, 0, 0 },
    /* Sicherung im Generator: sub00 @0x010C6 Slot 9 -> sub18 @0x01678 msg 3, @0x0167E msg 4 */
    { 0x2060, 0x40, 9, 18, 4, RE15_R41O_FLAGITEM, 0, KEIN_TOR, 0, 0 },
    /* Red Master Keycard: main00 @0x01FB4 Slot 10 -> sub10 @0x02578 msg 3, @0x0257E msg 4 */
    { 0x3050, 0x46, 10, 10, 4, 0, 0, KEIN_TOR, 0, 0 },
    /* Blue Master Keycard: sce-5-Platz 2 @0x010BC, sub01 @0x0137C Cmp(Var0==2) -> sub02 @0x01426 Ck(3,32,0) msg 0, @0x01440 msg 1.
     * Tor = die Bedingung des Text-Blocks @0x0142A; ROOM4001 hat die Stelle nicht (msg 1 ohne Message_on). */
    { 0x4000, 0x47, 2, MARKER, 1, 0, 1, 3, 32, 0 },
    /* Zuendkerze: sce-5-Platz 5 @0x00516, sub01 Case 5 @0x005DE -> sub02 @0x00612 msg 0, Ck(5,3,1) @0x00636, @0x0063A msg 1 */
    { 0x4030, 0x41, 5, MARKER, 1, 0, 0, 5, 3, 1 },
};
#define N_STELLEN ((int)(sizeof k_stellen / sizeof k_stellen[0]))

/* ---- Gegenstaende, die RE1.5 nur als Flag kennt -------------------------------------------------------------------------
 * Der Port fuehrt sie als Inventar-Eintrag (NUTZER-VORGABE Nachtrag 34: ohne Eintrag gaebe es im Inventar nichts auszuwaehlen; Form
 * RE2 Crank 0x43, Verbrauch sce_item_lost @0x800585e4). soll = genommen && !benutzt. */
typedef struct {
    uint8_t item;
    uint8_t ja_bank, ja_bit;      /* genommen                                  */
    uint8_t nein_bank, nein_bit;  /* benutzt                                   */
    uint8_t weg_bank, weg_bit;    /* zweite Weg-Bedingung (0xFF = keine)       */
} flagitem_t;

static const flagitem_t k_flagitems[] = {
    /* Kurbel: genommen ROOM2010 sub02 @0x008CA `22 03 72 01`, benutzt ROOM2020 sub02 @0x00A7A `22 03 71 01`. */
    { RE15_R41O_KURBEL_ITEM, 3, 114, 3, 113, KEIN_TOR, 0 },
    /* Zeitbombe: genommen ROOM1110 sub03 @0x00D32 `22 03 92 01`, gezuendet ROOM2040 sub02 @0x01624 `22 03 35 01`. */
    { RE15_R41O_BOMBE_ITEM,  3, 146, 3,  53, KEIN_TOR, 0 },
    /* Sicherung: genommen ROOM2030 sub06 @0x01FF2 `22 03 6c 01`, im Generator ROOM2060 sub19 @0x0169E `22 03 90 01`, im
     * Ventilkasten 2010 Bit (9,125) (Spur C): dort liegt sie nicht beim Spieler. */
    { 0x40, 3, 108, 3, 144, 9, RE15_R41C_BIT_KASTEN },
};
#define N_FLAGITEMS ((int)(sizeof k_flagitems / sizeof k_flagitems[0]))

/* Aufnahme-Fragen dieser Gegenstaende: bei vollem Inventar gilt RE2/RE1.5 "You can't carry any more items" (Item-Modal Zustand 5,
 * FUN_8004df2c @0x8001df14) statt der Frage; der Gegenstand bleibt liegen. */
static const struct { uint16_t raum; uint8_t msg; uint8_t item; } k_aufnahme[] = {
    { 0x2010, 0, RE15_R41O_KURBEL_ITEM },   /* sub02 @0x008BC */
    { 0x1110, 8, RE15_R41O_BOMBE_ITEM  },   /* sub03 @0x00D24 */
    { 0x2030, 2, 0x40 },                    /* sub06 @0x01FE4 */
};

/* RE2 haelt Verbrauchtes im Skript fest (sce_item_lost, ROOM60D0 @sub06 `62 4d` Fuse Case); RE1.5 hat dafuer keinen Opcode und
 * laesst den Gegenstand liegen. Die Zuendkerze hat keinen "You've used"-Text und damit keine Wegwerf-Abfrage (gen/discard_sites.inc):
 * sie verlaesst das Inventar mit dem Skript-Flag ihrer Benutzung ROOM4030 sub06 @0x0068C `22 03 4e 01`. */
#define ZUENDKERZE       0x41
#define ZUENDKERZE_BANK  3
#define ZUENDKERZE_BIT   78

/* 25bc-Grid-Kapazitaet @0x800b0fbc (Leon 10; item_modal_common.c INV_CAPACITY, FUN_8004df2c @0x8001df14). */
#define INV_KAPAZITAET   10

/* Vorwaerts-Punkt der Aktionstaste: `ori 0x26c` @0x80042bd0 = 620. */
#define VORWAERTS        620

/* Zustand */
static const stelle_t *s_vormerkung = NULL;    /* USE bestaetigt, Inventar schliesst noch                     */
static const stelle_t *s_lauf       = NULL;    /* USE-Lauf: die Skript-Texte bis zur Frage entfallen          */
static int      s_lauf_takte        = 0;       /* game_step-Takte seit dem Ereignis-Start                     */
static int      s_evt_next_weg      = 0;       /* Evt_next direkt hinter einer entfallenen Nachricht          */
static int      s_plc_wartet        = 0;       /* Kurbel: Frage durch, Plc_motion kommt im selben Takt        */
static int      s_text_ende         = -1;      /* Kombi-Text: Parse-Stelle des letzten Seitenumbruchs         */
static int      s_waffe_id          = -1;      /* fuer die Kurbel abgelegte Waffe, RE2 69f3/69f5              */
static int      s_waffe_szene       = 0;       /* Plc_motion gelaufen: Plc_ret beendet die Szene              */
static int8_t   s_soll_alt[N_FLAGITEMS];       /* -1 = noch kein Vergleich in diesem Raum                     */
static int      s_kerze_alt         = -1;
static unsigned s_zaehler[2];
static int      s_test_ja           = 0;       /* Pruefhaken: die entfallene Frage gilt als Ja                */

static int logan(void)
{
    static int s = -1;
    if (s < 0) s = getenv("RE15_KEYITEMS_LOG") ? 1 : 0;
    return s;
}
#ifdef RE15_PLATFORM_PC
#define LOG(...) do { if (logan()) fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define LOG(...) do { } while (0)
#endif

/* Der Raum der Zeile: Elza-Raeume sind die ROOMxxx1-Varianten mit denselben Skripten (VERTRAG Kopf). */
static int raum_passt(const stelle_t *st, unsigned raum)
{
    return st->nur_der ? raum == st->raum : (raum & 0xFFFEu) == st->raum;
}

static int tor_offen(const stelle_t *st)
{
    return st->tor_bank == KEIN_TOR ||
           re15_game_flag_get(st->tor_bank, st->tor_bit) == (int)st->tor_wert;
}

static int in_box(uint8_t item)
{
    for (int i = 0; i < RE15_BOX_SLOTS; i++)
        if (g_itembox.slots[i].id == item) return 1;
    return 0;
}

static int hat_irgendwo(uint8_t item) { return re15_inv_find_item(item) >= 0 || in_box(item); }

static int anzahl_irgendwo(uint8_t item)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == item) n++;
    for (int i = 0; i < RE15_BOX_SLOTS; i++) if (g_itembox.slots[i].id == item) n++;
    return n;
}

/* Eintraege desselben Gegenstands, die nicht am Flag haengen: die Sicherung vom Hebetisch ROOM1150 ist ein echter Eintrag ((9,53),
 * sicherung_1150.c), bis sie im Rolltor sitzt ((9,63), rolltor_1050.c). Wer sie noch traegt, wenn er die aus ROOM2030 nimmt, hat zwei
 * Sicherungen; die Benutzung der einen darf die andere nicht mitnehmen, sonst bleibt das Rolltor zu. */
static int fremde_eintraege(const flagitem_t *f)
{
    return f->item == RE15_SICHERUNG_ITEM && re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) &&
           !re15_game_flag_get(RE15_ROLLTOR_EINGESETZT_BANK, RE15_ROLLTOR_EINGESETZT_BIT) ? 1 : 0;
}

static int platz_frei(void)
{
    for (int i = 0; i < INV_KAPAZITAET; i++)
        if (g_inv.slots[i].id == 0) return 1;
    return 0;
}

/* Die Treffer-Geometrie der Aktionstaste fuer EINEN Slot (FUN_80042bac, Port re15_aot_scan): Band-Test @0x80042cac, dann
 * Zentrum (Flag 0x40 @0x80042ea8), dann Vorwaerts-Punkt (Flag 0x20 @0x80042ef8), Rechteck oder Viereck (Flag 0x80). RE2 prueft
 * dasselbe in FUN_80051088 (@0x80051390) mit sat 0x21. */
static int aktion_trifft(const re15_aot_t *a)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int pb = re15_collision_debug_band();
    if (pb >= 0 && !(a->band & 0x80) && (int)a->band != pb) return 0;
    const uint8_t fl = a->sce_flags;
    if (fl == 0 || (fl & 0x40)) {
        const int innen = a->has_quad
                        ? re15_aot_point_in_quad(pl->x, pl->z, a->xs, a->zs)
                        : (abs(pl->x - a->x) <= a->half_w && abs(pl->z - a->z) <= a->half_h);
        if (innen) return 1;
    }
    if (fl == 0 || (fl & 0x20)) {
        const int32_t c = re15_cos_q12((int)pl->rot_y), s = re15_sin_q12((int)pl->rot_y);
        const int32_t fx = pl->x + (int32_t)((VORWAERTS * c) >> 12);
        const int32_t fz = pl->z - (int32_t)((VORWAERTS * s) >> 12);
        return a->has_quad ? re15_aot_point_in_quad(fx, fz, a->xs, a->zs)
                           : (abs(fx - a->x) <= a->half_w && abs(fz - a->z) <= a->half_h);
    }
    return 0;
}

static const stelle_t *stelle_vor_dem_spieler(uint8_t item)
{
    const unsigned raum = g_current_room_id;
    for (int i = 0; i < N_STELLEN; i++) {
        const stelle_t *st = &k_stellen[i];
        if (st->item != item || !raum_passt(st, raum) || !tor_offen(st)) continue;
        const re15_aot_t *a = &g_aot.slots[st->slot];
        if (!a->active) continue;
        if (st->ereignis == MARKER) { if (a->type != RE15_AOT_TYPE_EXAMINE_WORKVAR) continue; }
        else if (a->type != RE15_AOT_TYPE_GENERIC || a->event_id != st->ereignis) continue;
        if (aktion_trifft(a)) return st;
    }
    return NULL;
}

int re15_keyitems_r41_use(uint8_t item_id)
{
    const stelle_t *st = stelle_vor_dem_spieler(item_id);
    if (!st) {
        LOG("[keyitems] USE 0x%02x ROOM%04X: keine Stelle (You can't use it here.)\n", (unsigned)item_id, g_current_room_id);
        return 0;
    }
    s_vormerkung = st;
    LOG("[keyitems] USE 0x%02x ROOM%04X Slot %d Ereignis %d -> Inventar schliesst\n", (unsigned)item_id, g_current_room_id,
        (int)st->slot, (int)st->ereignis);
    return 1;
}

int re15_keyitems_r41_schliessen(void)
{
    const stelle_t *st = s_vormerkung;
    if (!st) return 0;
    s_vormerkung = NULL;
    s_lauf = st;
    s_lauf_takte = 0;
    s_evt_next_weg = 0;
    s_plc_wartet = 0;
    if (st->ereignis == MARKER) {
        /* wie die Aktionstaste: Treffer-Variable (Vorwaerts-Treffer @0x80042f3c) stempeln, die Raum-sub01 startet das Ereignis */
        g_scd.work_vars[0] = (int16_t)st->slot;
        g_scd.examine_poll_pending = 1;
    } else if (scd_event_fire(st->ereignis) < 0) {
        s_lauf = NULL;
        LOG("[keyitems] Ereignis %d nicht gestartet (alle Plaetze belegt)\n", (int)st->ereignis);
        return 0;
    }
    if (st->art & RE15_R41O_WAFFE) {
        /* RE2 @0x80068cf8-@0x80068d40: fuer Valve Handle, Manhole Opener und Crank 5bf8 := 0x80, 5bfa := 0 (@0x80068d34), also
         * Waffe 0 = kein Anhang an Knochen 11. Das RE1.5-Commit kennt 0 nicht (0x80 -> 1 @0x80046668), darum nur die Id.
         * PORT-WAHL Rueckkehr: RE2 legt erst bei der naechsten Oeffnung wieder an (@0x80068be4-@0x80068c34, bis dahin unbewaffnet,
         * gerade neben den vier Zombies von ROOM2020); der Port am Ende der Szene (Plc_ret @0x0B16), Platz 25c8 bleibt unberuehrt. */
        s_waffe_id = re15_player_equipped_weapon();
        s_waffe_szene = 0;
        re15_player_waffe_roh(0);
    }
    LOG("[keyitems] USE-Lauf ROOM%04X Ereignis %d Frage %d\n", g_current_room_id, (int)st->ereignis, (int)st->frage);
    return 1;
}

static void waffe_zurueck(void)
{
    if (s_waffe_id < 0) return;
    re15_player_waffe_roh(s_waffe_id);
    LOG("[keyitems] Waffe %d wieder in der Hand\n", s_waffe_id);
    s_waffe_id = -1;
    s_waffe_szene = 0;
}

/* Parse-Stelle des letzten Seitenumbruchs vor dem Ja/Nein-Code: dort endet die Beschreibung. Schrittweite wie re15_dialog_step. */
static int kombi_ende(int msg)
{
    int n = 0, letzte = -1;
    const unsigned char *r = re15_msg_get_raw(msg, &n);
    if (!r) return -1;
    for (int i = 0; i < n; i++) {
        const unsigned char b = r[i];
        if (b == 0x03) return letzte;
        if (b == 0x01) return -1;
        if (b == 0x02) { letzte = i; i++; continue; }
        if (b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0a || b == 0x0b) i++;
    }
    return -1;
}

int re15_keyitems_r41_message_on(uint8_t msg_id)
{
    const unsigned raum = g_current_room_id;
    s_text_ende = -1;
    if (s_lauf) {
        /* USE-Lauf: RE2 startet das Ereignis ohne Text und Frage. Antwort Ja = 0 (Ck(12,31,0), re15_dialog_step case 3). */
        re15_game_flag_set(12, 31, 0);
        s_evt_next_weg = 1;
        if (msg_id == s_lauf->frage) {
            s_zaehler[1]++;
            LOG("[keyitems] Frage %d entfaellt: Ja (USE)\n", (int)msg_id);
            if (s_lauf->art & RE15_R41O_SOFORT) s_plc_wartet = 1;
            s_lauf = NULL;
        }
        return 1;
    }
    for (unsigned i = 0; i < sizeof k_aufnahme / sizeof k_aufnahme[0]; i++) {
        if ((raum & 0xFFFEu) != k_aufnahme[i].raum || msg_id != k_aufnahme[i].msg || platz_frei()) continue;
        re15_game_flag_set(12, 31, 1);
        re15_item_modal_start(k_aufnahme[i].item, 1, 0, -1, 0xFF);   /* Zustand 5 -> script[1], Zustand 8: nichts vergeben */
        LOG("[keyitems] Aufnahme ROOM%04X msg %d: Inventar voll\n", raum, (int)msg_id);
        return 1;
    }
    for (int i = 0; i < N_STELLEN; i++) {
        const stelle_t *st = &k_stellen[i];
        if (st->frage != msg_id || !raum_passt(st, raum)) continue;
        /* Altstand: Flag-Gegenstand ohne Eintrag (volles Inventar) -> die Frage bleibt, sonst waere die Stelle tot. */
        if ((st->art & RE15_R41O_FLAGITEM) && !hat_irgendwo(st->item)) return 0;
        re15_game_flag_set(12, 31, s_test_ja ? 0 : 1);  /* Antwort Nein */
        s_zaehler[0]++;
        if (st->art & RE15_R41O_KOMBI) {
            s_text_ende = kombi_ende((int)msg_id);
            LOG("[keyitems] Frage %d: nur Beschreibung (Seite endet @%d)\n", (int)msg_id, s_text_ende);
            return 0;
        }
        LOG("[keyitems] Frage %d entfaellt: Nein (Aktionstaste)\n", (int)msg_id);
        return 1;
    }
    return 0;
}

int re15_keyitems_r41_evt_next(void)
{
    if (!s_evt_next_weg) return 0;
    s_evt_next_weg = 0;
    return 1;
}

int re15_keyitems_r41_sleep(void) { return s_plc_wartet; }

int re15_keyitems_r41_plc_flags(void)
{
    if (!s_plc_wartet) return 0;
    s_plc_wartet = 0;
    s_waffe_szene = 1;
    return 0x40;   /* RE2 ROOM1110 sub15 @0x02FC1 `3f 00 0f 45`, RE1.5 @0x80050d24 `andi v0,v0,0x40` -> @0x80050d30 +0x8f := 0 */
}

/* PORT-WAHL Szenenende der Kurbel: RE2 ROOM1110 sub13 verdeckt den Wechsel Kurbel -> Stehen mit einer Blende um Pos_set und Plc_ret (@0x02F24
 * `53 02 02 07 00 18` ... @0x02F4E `53 02 02 07 00 e8`). RE1.5 hat dort keine Blende, aber den Schnitt @0x0B0E `29 03` im selben
 * Takt wie Plc_ret @0x0B16: ohne Mischung (wie Bit 0x40, @0x80050d30) faellt der Wechsel in den Schnitt, sonst kippt Leon nach dem
 * Schnitt sieben Bilder lang von der Kurbel-Pose in die Stehpose zurueck. */
int re15_keyitems_r41_plc_ret(void) { return s_waffe_szene; }

int re15_keyitems_r41_text_ende(int parse)
{
    if (s_text_ende < 0 || parse != s_text_ende) return 0;
    s_text_ende = -1;
    return 1;
}

void re15_keyitems_r41_tick(void)
{
    extern int re15_menu_gameplay_frozen(void);
    if (re15_menu_gameplay_frozen()) return;
    if (s_lauf) {
        /* die Frage faellt in allen Stellen im ersten Skripttakt (Riegel unit_r41_keyitems_stellen): kam sie nicht, gilt die
         * Stelle nicht mehr, und die naechste Aktionstaste darf kein Ja erben. */
        if (++s_lauf_takte > 1) {
            LOG("[keyitems] USE-Lauf ohne Frage beendet (Ereignis %d)\n", (int)s_lauf->ereignis);
            s_lauf = NULL;
        }
    }
    /* Plc_ret (@0x80041f90 Spieler-Kommando zurueck auf 1): die Szene ist vorbei. */
    if (s_waffe_szene && g_actors[RE15_ACTOR_SLOT_PLAYER].state != 4) waffe_zurueck();
    for (int i = 0; i < N_FLAGITEMS; i++) {
        const flagitem_t *f = &k_flagitems[i];
        const int soll = re15_game_flag_get(f->ja_bank, f->ja_bit) && !re15_game_flag_get(f->nein_bank, f->nein_bit) &&
                         !(f->weg_bank != KEIN_TOR && re15_game_flag_get(f->weg_bank, f->weg_bit));
        const int fremd = fremde_eintraege(f);
        const int ist = anzahl_irgendwo(f->item);
        if (soll && ist < 1 + fremd && platz_frei()) {
            if (re15_inv_grant(f->item, 1) == 0)
                LOG("[keyitems] Gegenstand 0x%02x ins Inventar ((%d,%d) gesetzt)\n", (unsigned)f->item, f->ja_bank, f->ja_bit);
        } else if (!soll && s_soll_alt[i] == 1) {
            /* NUR an der Flanke: Sicherung 0x40 kann auch die aus ROOM1150 sein ((3,108) = 0). RE2 sce_item_lost @0x800585e4. */
            const int p = re15_inv_find_item(f->item);
            if (p >= 0 && ist > fremd) re15_inv_remove_slot(p);
            LOG("[keyitems] Gegenstand 0x%02x verlaesst das Inventar (Platz %d)\n", (unsigned)f->item, p);
        }
        s_soll_alt[i] = (int8_t)soll;
    }
    {
        const int kerze = re15_game_flag_get(ZUENDKERZE_BANK, ZUENDKERZE_BIT);
        if (kerze && s_kerze_alt == 0) {
            const int p = re15_inv_find_item(ZUENDKERZE);
            if (p >= 0) re15_inv_remove_slot(p);
        }
        s_kerze_alt = kerze;
    }
}

void re15_keyitems_r41_raum(uint16_t room_id)
{
    (void)room_id;
    waffe_zurueck();
    s_vormerkung = NULL;
    s_lauf = NULL;
    s_evt_next_weg = 0;
    s_plc_wartet = 0;
    s_text_ende = -1;
    memset(s_soll_alt, -1, sizeof s_soll_alt);
    s_kerze_alt = -1;
}

const uint8_t *re15_keyitems_r41_name(uint8_t item_id)
{
    return item_id == RE15_R41O_KURBEL_ITEM ? re15_r41o_kurbel_name : NULL;
}
const uint8_t *re15_keyitems_r41_desc(uint8_t item_id)
{
    return item_id == RE15_R41O_KURBEL_ITEM ? re15_r41o_kurbel_desc : NULL;
}
void re15_keyitems_r41_icon_einsetzen(uint8_t *itemall, int size)
{
    if (itemall && (RE15_R41O_KURBEL_ITEM + 1) * 1200 <= size)
        memcpy(itemall + RE15_R41O_KURBEL_ITEM * 1200, re15_r41o_kurbel_icon, 1200);
}
void re15_keyitems_r41_bild_einsetzen(uint8_t *itps, int size)
{
    if (!itps) return;
    if ((RE15_R41O_KURBEL_ITEM + 1) * 0x3000 <= size)
        memcpy(itps + RE15_R41O_KURBEL_ITEM * 0x3000, re15_r41o_kurbel_itps, 0x3000);
    if ((RE15_R41O_BOMBE_ITEM + 1) * 0x3000 <= size) {
        /* crect (0,489), prect (832,256) wie jeder sichtbare RE1.5-Block (Block+0x0C, Bildblock @0x214 +4) */
        uint8_t *b = itps + RE15_R41O_BOMBE_ITEM * 0x3000;
        b[0x0C] = 0;    b[0x0D] = 0;    b[0x0E] = 0xE9; b[0x0F] = 0x01;
        b[0x218] = 0x40; b[0x219] = 0x03; b[0x21A] = 0x00; b[0x21B] = 0x01;
    }
}

void re15_keyitems_r41_test_ja(int an) { s_test_ja = an; }

/* Pruefhaken: der Ja-Zweig einer Stelle ohne Menue und ohne Lage-Pruefung (Gegenstueck zu re15_aot_fire_slot der Aktionstaste). */
int re15_keyitems_r41_testlauf(uint8_t item_id)
{
    const unsigned raum = g_current_room_id;
    for (int i = 0; i < N_STELLEN; i++) {
        const stelle_t *st = &k_stellen[i];
        if (st->item != item_id || !raum_passt(st, raum)) continue;
        s_vormerkung = st;
        return re15_keyitems_r41_schliessen();
    }
    return 0;
}

/* Messhaken RE15_USE_TEST="<item>@<bild>#<hexraum>" (Muster RE15_FIRE_AOT, main.c): einmal re15_keyitems_r41_testlauf. */
void re15_keyitems_r41_testhaken(void)
{
    static int init = 0, id = -1, fertig = 0;
    static long bild = 0;
    static unsigned raum = 0;
    if (!init) {
        const char *e = getenv("RE15_USE_TEST");
        init = 1;
        if (e && *e) {
            id = (int)strtol(e, NULL, 0);
            const char *at = strchr(e, 0x40), *hs = strchr(e, 0x23);
            if (at) bild = atol(at + 1);
            if (hs) raum = (unsigned)strtol(hs + 1, NULL, 16);
        }
    }
    if (id < 0 || fertig || (long)g_engine.frame_count < bild || (raum && g_current_room_id != raum)) return;
    fertig = 1;
    {
        const int r = re15_keyitems_r41_testlauf((uint8_t)id);
        (void)r;
        LOG("[keyitems] Messhaken USE 0x%02x F%u: %d\n", (unsigned)id, (unsigned)g_engine.frame_count, r);
    }
}

int re15_keyitems_r41_stellen_anzahl(void) { return N_STELLEN; }
int re15_keyitems_r41_stelle(int i, uint16_t *raum, uint8_t *item, int *slot, int *ereignis, uint8_t *frage, uint8_t *art)
{
    if (i < 0 || i >= N_STELLEN) return 0;
    const stelle_t *st = &k_stellen[i];
    if (raum) *raum = st->raum;
    if (item) *item = st->item;
    if (slot) *slot = (int)st->slot;
    if (ereignis) *ereignis = st->ereignis == MARKER ? -1 : (int)st->ereignis;
    if (frage) *frage = st->frage;
    if (art) *art = st->art;
    return 1;
}
int re15_keyitems_r41_lauf_aktiv(void) { return s_lauf != NULL || s_vormerkung != NULL; }
unsigned re15_keyitems_r41_verschluckt(int ja) { return s_zaehler[ja ? 1 : 0]; }
