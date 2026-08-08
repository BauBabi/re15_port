/*
 * re15_savepoint.c — the phone save-point registry + pending signal. See
 * re15_savepoint.h. The SCD Message_on handler (scd_vm.c) calls
 * re15_savepoint_is() and, on a match, sets the pending signal the platform
 * consumes to open the save flow.
 */
#include "re15_savepoint.h"

/* The 16 phone/computer save-points: {room, main<NN> message id}. This is the
 * COMPLETE set - every room + message index game-wide whose .msg body decodes to
 * "You can save your progress with this. Save is not available in this preview"
 * (exhaustive scan of all room RDT message blocks: RDT+0x3c table, atlas decode
 * char = byte + 0x24). Each has a Leon room (even id) + an Elza mirror (odd id).
 * (Was 14 - STAGE5 5010/5011 were missing, so their computers showed the dormant
 * flavor message instead of the save menu; verified byte-true 2026-07-20.) */
/* loc = Ortsnamen-Index (sysmes 0x1a+loc aus DEBUG.BIN @0x6197ff / SJIS-Tabelle 0x80073628 +
 * 0x13*loc). BELEG-LAGE dreistufig (Nutzer-Entscheidungen 2026-08-08):
 *   [PATCH]  = Vorprojekt-Patch definiert den Wert: 1150/1151 -> 0 (SCD_SAVE_RET @0x800708c0
 *              sb zero), 1070 -> 1 (AOT_TYPE1_HOOK @0x8007087c sb v0=1; RDT-Sentinel
 *              ROOM1070.RDT @0x1568). Mehr kennt der Patch game-weit nicht.
 *   [PORT-S] = PORT-Entscheidung, semantisch gestuetzt: 1071 = Elza-Spiegel des Telefons;
 *              1120/1121 = der Vorraum des Treppenhauses (sysmes 0x1c "West Staircase 1F");
 *              4010/4011 = Debug-Raumname "SECURITY ROOM" (DEBUG.BIN @0x2642-Tabelle)
 *              == sysmes 0x20; 5010/5011 -> sysmes 0x21 "Monitor Room".
 *   [PORT-L] = PORT-Entscheidung OHNE semantischen Anker, reine Listen-Reihenfolge der
 *              8 sysmes-Namen ueber die 8 Save-Orte (die Namensliste stammt erkennbar aus
 *              einem aelteren Build-Stand — "Sewer ..." passt zu KEINEM Debug-Raumnamen
 *              der Stage 2/3; recheck-Dossier §2.1). Im Original ist der Resolver ein
 *              return-0-Stub @0x80026e4c — JEDE Zuordnung hier ist Port-Ware.
 * Anzeige-Hinweise: sysmes 0x1c-0x1f sind laenger als die 0x10-Byte-Kopie der Slot-Zeile
 * (@0x80026830) -> byte-true auf 16 Glyphen gekappt; die SJIS-Kartentitel-Tabelle hat nur
 * die Eintraege 0..6 -> loc 7 clampt dort auf 0 (re15_mc_title.c, nur externes Metadatum). */
static const struct { unsigned room; uint8_t msg; uint8_t loc; } s_savepoints[] = {
    { 0x1070, 0x14, 1 }, { 0x1071, 0x14, 1 },   /* STAGE1 main20 — Telefon [PATCH; 1071 PORT-S] */
    { 0x1120, 0x06, 2 }, { 0x1121, 0x06, 2 },   /* STAGE1 main06 — Treppenhaus-Vorraum [PORT-S] */
    { 0x1150, 0x01, 0 }, { 0x1151, 0x01, 0 },   /* STAGE1 main01 — Schreibmaschine [PATCH] */
    { 0x2010, 0x03, 3 }, { 0x2011, 0x03, 3 },   /* STAGE2 main03 — [PORT-L] Sewer Maintainance */
    { 0x30A0, 0x01, 4 }, { 0x30A1, 0x01, 4 },   /* STAGE3 main01 — [PORT-L] Sewer Control Room */
    { 0x30B0, 0x01, 5 }, { 0x30B1, 0x01, 5 },   /* STAGE3 main01 — [PORT-L] Factory - Office (examine via sce=0 work-var AOT - deferred) */
    { 0x4010, 0x2B, 6 },                        /* STAGE4 main43 — [PORT-S] Security Room */
    { 0x4011, 0x07, 6 },                        /* STAGE4 main07 — [PORT-S] Security Room */
    { 0x5010, 0x06, 7 }, { 0x5011, 0x2D, 7 },   /* STAGE5 main06 / main45 — [PORT-S] Monitor Room */
};

int re15_savepoint_is(unsigned room_id, uint8_t msg_id)
{
    for (unsigned i = 0; i < sizeof(s_savepoints) / sizeof(s_savepoints[0]); i++)
        if (s_savepoints[i].room == room_id && s_savepoints[i].msg == msg_id)
            return 1;
    return 0;
}

static int s_savepoint_pending = 0;
int  re15_savepoint_pending(void)        { return s_savepoint_pending; }
void re15_savepoint_set_pending(int on)  { s_savepoint_pending = on ? 1 : 0; }

/* Gameplay cut latched at the examine action (see the header). */
static int s_savepoint_cut = -1;
void re15_savepoint_set_cut(int cut)  { s_savepoint_cut = cut; }
int  re15_savepoint_saved_cut(void)   { return s_savepoint_cut; }

/* Ortsnamen-Latch (Patch-Analog: das Live-Global 0x800B0FBF, geschrieben von den beiden
 * Trigger-Hooks BEVOR das Kartenmenue oeffnet — hier beim Pending-Set der Intercepts). */
static uint8_t s_savepoint_loc = 0;
void re15_savepoint_latch_loc(unsigned room_id)
{
    s_savepoint_loc = 0;                     /* unbekannter Raum / RE15_SAVE_TEST -> Stub-Wert 0 */
    for (unsigned i = 0; i < sizeof(s_savepoints) / sizeof(s_savepoints[0]); i++)
        if (s_savepoints[i].room == room_id) { s_savepoint_loc = s_savepoints[i].loc; break; }
}
uint8_t re15_savepoint_loc(void) { return s_savepoint_loc; }

void re15_savepoint_reset(void) { s_savepoint_pending = 0; s_savepoint_cut = -1; s_savepoint_loc = 0; }
