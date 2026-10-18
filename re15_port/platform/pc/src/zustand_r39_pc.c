/*
 * zustand_r39_pc.c — Runde 39 Spur F "zustand": PC-Seite der Kleidungs-Wiederherstellung
 * (engine/src/zustand_r39.c, include/re15_zustand_r39.h; Dossier analysis/befunde_runde39/F_zustand.md).
 * Nachbesserung 1 (Abnahme 0, Mangel M1): die Funktion stand als 19-Zeilen-Block in platform/pc/main.c
 * (nur den Spuren A/B zugeteilt); in main.c bleibt der 2-Zeilen-Haken im Blut-Decal-Sync, der den
 * tatsaechlich geladenen Modell-Index uebergibt (main.c s_player_model_idx, sonst Charakter-Nibble).
 *
 * Spielerhaut (Slot 0) aus der aktuellen PLD neu laden = der Haut-Upload des Spieler-Ladens des Originals
 * (FUN_800314b0 -> FUN_8004ee78 Slot 0x17 @0x800316c8) — die Kleidung ohne Blut-Decals (NUTZER-VORGABE:
 * volle Heilung = ganze Kleidung; das Original loescht die Decals nie, blood_decals.md). Familie wie der
 * Spielstart / pc_sync_player_model: PL%02X aus der unteren Nibble von DAT_800aca5c (`andi v0,a0,0xf`
 * @0x8003976c; Leon 0, Elza 4; Tabelle @0x80073f70, Leser FUN_800314b0 @0x800314d4) bzw. der geladene
 * Modellwechsel-Index (1 = R.P.D.-Weste PL01, Flag(3,0x75) -> work_vars[0x10], re15_savedata.c).
 *
 * Quelle der TIM — dieselbe Reihenfolge wie main.c pc_read_pl_part: erst die lose Teildatei PLD/PLxx.TIM
 * (Leon PL00 und Elza PL04 liegen so im ausgelieferten Baum), sonst der Schnitt aus dem Container
 * PLD/PLxx.PLD (re15_pld_part, Teil RE15_PLD_TIM; die Weste PL01 liegt NUR als Container vor).
 * re15_render_pc_upload_tim_slot kopiert die Texel in einen eigenen RGBA-Puffer (render_pc.c, malloc),
 * danach wird hier der GELESENE Puffer freigegeben — immer der malloc'd Puffer (Teildatei oder ganzer
 * Container), nie der Schnittzeiger. (Die Fassung in main.c vor der Nachbesserung gab den Rueckgabezeiger
 * von pc_read_pl_part frei; fuer einen Container-Schnitt zeigt der IN den Puffer — Messung Dossier
 * "Nachbesserung 1", Westen-Lauf.) Der Upload bumpt die Slot-0-Generation: der Waffen-Composite legt
 * sich wieder auf, der Blut-Decal-Sync findet die (schon genullte) Wund-Tabelle leer.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_tim.h"        /* re15_tim_parse, re15_render_pc_upload_tim_slot */
#include "re15_md1.h"        /* re15_pld_part, RE15_PLD_TIM */
#include "asset_root_pc.h"   /* re15_pc_read_any = dieselbe Wurzelliste wie main.c pc_read_shared */

void re15_pc_zustand_r39_haut_neu(int modell)
{
    char stem[8], pfad[32];
    snprintf(stem, sizeof stem, "PL%02X", (unsigned)(modell & 0x0F));
    int sz = 0;
    const uint8_t *tim = NULL;
    int tim_len = 0;
    const char *quelle = "Teildatei";
    snprintf(pfad, sizeof pfad, "PLD/%s.TIM", stem);
    uint8_t *puffer = re15_pc_read_any(pfad, &sz);      /* malloc'd -> unten free */
    if (puffer && sz > 0) {
        tim = puffer; tim_len = sz;
    } else {
        free(puffer); puffer = NULL; sz = 0;
        snprintf(pfad, sizeof pfad, "PLD/%s.PLD", stem);
        puffer = re15_pc_read_any(pfad, &sz);
        unsigned long off = 0, len = 0;
        if (puffer && sz > 0 && re15_pld_part(puffer, (long)sz, RE15_PLD_TIM, &off, &len) &&
            len > 0 && off + len <= (unsigned long)sz) {
            tim = puffer + off; tim_len = (int)len;     /* zeigt IN den Container */
            quelle = "Container";
        }
    }
    re15_tim_t nt;
    if (tim && re15_tim_parse(tim, tim_len, &nt) == 0) {
        re15_render_pc_upload_tim_slot(&nt, 0);          /* Slot 0 = Spielerhaut; kopiert */
        fprintf(stderr, "[r39-zustand] Spielerhaut %s.TIM neu geladen (%dx%d, %s %s) - Kleidung ganz\n",
                stem, nt.width, nt.height, quelle, pfad);
    } else {
        fprintf(stderr, "[r39-zustand] Spielerhaut %s.TIM NICHT neu ladbar (%s)\n", stem, pfad);
    }
    free(puffer);                                        /* nur der gelesene Puffer, nie der Schnitt */
}
