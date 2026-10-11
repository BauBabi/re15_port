/*
 * RE1.5 Rebuilt — Runde 36 Spur I "inventar". Herleitung und Belege: include/re15_inventar_r36.h und
 * analysis/befunde_runde36/I_inventar.md.
 */
#include "re15_inventar_r36.h"

#include "re15_inventory.h"   /* g_inv, re15_inv_find_item, re15_inv_remove_slot, Ausruest-Platz */
#include "re15_damage.h"      /* re15_player_equipped_weapon / _set_equipped_weapon (DAT_800aca5d) */
#include "re15_audio.h"       /* re15_audio_prime_weapon (Bank-Lader FUN_80043d8c)               */
#include "re15_menu.h"        /* re15_menu_gameplay_frozen                                       */
#include "re15_esp.h"         /* re15_esp_fx_get / granate_art                                    */
#include "re15_messer.h"      /* RE15_MESSER_ID / RE15_MESSER_NICHTS (Runde 35 Spur E)            */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

extern int re15_player_aim_active(void);   /* player_common.c — Zielzustand (Phase != NONE) */

int re15_inventar_r36_leer_weg_id(uint8_t id)
{
    if (id == RE15_R36_KARTE_ID) return 1;
    return (id >= RE15_R36_GRANATE_ERSTE && id <= RE15_R36_GRANATE_LETZTE);
}

static int ist_granate(int waffe)
{
    return waffe >= RE15_R36_GRANATE_ERSTE && waffe <= RE15_R36_GRANATE_LETZTE;
}

/* ===== I1/I2: Memory Card beim Speichern =================================================== */

/* Verdichten auf einem Platz-Feld — dieselbe Rechnung wie re15_inv_compact (FUN_8004dadc: erster
 * freier Platz @0x8004daec, Nachruecken @0x8004db4c-dba8, Ausruest-Platz -1 @0x8004dbac-dbc8), nur ohne
 * die Symbolzellen (die gibt es im gespeicherten Block nicht). Muss Platz fuer Platz das Ergebnis von
 * re15_inv_remove_slot im Live-Inventar liefern (Riegel unit_r36_inventar (A)). */
static void block_verdichten(re15_inv_slot_t *s, uint8_t *ausruest)
{
    int f = -1;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (s[i].id == 0) { f = i; break; }
    if (f < 0 || f >= RE15_INV_MAX_SLOTS - 1) return;
    for (int i = f; i < RE15_INV_MAX_SLOTS - 1; i++) s[i] = s[i + 1];
    s[RE15_INV_MAX_SLOTS - 1].id    = 0;
    s[RE15_INV_MAX_SLOTS - 1].qty   = 0;
    s[RE15_INV_MAX_SLOTS - 1].flags = 0;
    if (*ausruest != RE15_MESSER_NICHTS && (int)*ausruest > f) (*ausruest)--;
}

int re15_inventar_r36_karte_im_block(re15_savedata_t *sd)
{
    if (!sd) return -1;
    int k = -1;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)            /* FUN_800696cc @0x801c1190 (nur Inventar) */
        if (sd->inv[i].id == RE15_R36_KARTE_ID) { k = i; break; }
    if (k < 0) return -1;
    uint8_t n = sd->inv[k].qty;
    if (n > 0) n--;                                         /* `addiu v0,v0,-1` @0x801c11ac. Waechter: eine
                                                             * Karte mit Menge 0 raeumt der Tick vorher weg
                                                             * (re15_inventar_r36_tick), 0 -> 255 kommt nie vor */
    sd->inv[k].qty = n;                                     /* `sb v0,19005(at)` @0x801c11b8 */
    if (n == 0) {                                           /* `bne v0,zero` @0x801c11c0 */
        sd->inv[k].id    = 0;                               /* `sb zero,19004(at)` @0x801c11d0 */
        sd->inv[k].flags = 0;                               /* `sb zero,19006(at)` @0x801c11dc */
        block_verdichten(sd->inv, &sd->equipped_slot);      /* `jal 0x80069714` @0x801c11e0 */
    }
    sd->checksum = re15_savedata_checksum(sd);
    return k;
}

int re15_inventar_r36_karte_verbrauchen(void)
{
    int k = re15_inv_find_item(RE15_R36_KARTE_ID);
    if (k < 0) return -1;
    uint8_t n = g_inv.slots[k].qty;
    if (n > 0) n--;                                         /* @0x801c11ac (Waechter wie oben) */
    g_inv.slots[k].qty = n;
    if (n == 0) re15_inv_remove_slot(k);                    /* @0x801c11d0-e0 = RE1.5 @0x8004aee4-af2c */
    return (int)n;
}

/* ===== I3: leere Plaetze raeumen, Waffe nach dem Wurf nachziehen ============================ */

/* Fliegt/liegt noch ein Granatenplatz im Effekt-Pool? Die Bank der Granate (ARMS-Satz 0x0A: Abprall
 * 0x010A0001 | n<<8 @0x80018410-28, Liegen @0x80018350-58) muss bis zur Zuendung geladen bleiben —
 * mit der Messer-Bank laege Satz 0x0A ausserhalb ihrer Saetze. */
static int granate_unterwegs(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->granate_art != 0) return 1;
    }
    return 0;
}

int re15_inventar_r36_tick(void)
{
    if (re15_menu_gameplay_frozen()) return 0;              /* Statusschirm/Kiste offen: nichts anfassen */
    int weg = 0;
    /* (a) leere Plaetze. Alle hier gemeinten Ids sind EIN-Zellen-Gegenstaende (Breit-Fenster nur
     * 0x0e..0x13, `sltiu v0,v0,0x6` @0x8004dc78), ein Platz = eine Zelle. */
    for (int i = 0; i < RE15_INV_MAX_SLOTS; ) {
        uint8_t id = g_inv.slots[i].id;
        if (!re15_inventar_r36_leer_weg_id(id) || g_inv.slots[i].qty != 0) { i++; continue; }
        if (re15_inv_equipped_slot() == i)
            re15_inv_set_equipped_slot(RE15_MESSER_NICHTS); /* UNEQUIP @0x8004ad44-4c: 25c8 := 0x80 */
        re15_inv_remove_slot(i);                            /* @0x8004aee4-af2c + FUN_8004dadc; Platz i
                                                             * ist danach der Nachfolger -> nicht i++ */
        weg++;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[inventar-r36] Platz %d Id %02x Menge 0 -> entfernt (Ausruest-Platz 0x%02x, "
                        "Waffe %d)\n", i, (unsigned)id, (unsigned)re15_inv_equipped_slot(),
                re15_player_equipped_weapon());
#endif
    }
    /* (b) Waffe nachziehen, sobald der Wurf vorbei ist: die Wurf-FSM liest die Waffe jedes Bild
     * (`lbu 0x800aca5d` @0x80032e60) und das Projektil spawnt nur bei aca5d == 9 (@0x80033684-8c) —
     * die Id darf erst nach dem Wurf wechseln (PORT-WAHL des Zeitpunkts). */
    if (re15_inv_equipped_slot() == RE15_MESSER_NICHTS && ist_granate(re15_player_equipped_weapon())
        && !re15_player_aim_active() && !granate_unterwegs()) {
        int alt = re15_player_equipped_weapon();
        re15_player_set_equipped_weapon(RE15_MESSER_ID);    /* 0x80 -> 1: @0x8004666c / @0x80046688 */
        re15_inv_set_equipped_slot(RE15_MESSER_NICHTS);     /* der Setzer leitet den Platz per Suche ab
                                                             * (kein Messer im Inventar -> 0x80); fest-
                                                             * nageln wie re15_messer_startinventar */
        re15_audio_prime_weapon(RE15_MESSER_ID);            /* Bank `jal 0x80043d8c` @0x800466c4 */
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[inventar-r36] Waffe %d (leer, abgelegt) -> %d (Messer-Rueckfall 0x80)\n",
                alt, re15_player_equipped_weapon());
#endif
        (void)alt;
    }
    return weg;
}
