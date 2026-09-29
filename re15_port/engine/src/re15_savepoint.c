/*
 * re15_savepoint.c — the phone save-point registry + pending signal. See
 * re15_savepoint.h. The SCD Message_on handler (scd_vm.c) calls
 * re15_savepoint_is() and, on a match, sets the pending signal the platform
 * consumes to open the save flow.
 */
#include "re15_savepoint.h"
#include "re15_inventory.h"   /* re15_inv_find_item — Karten-Suche NUR im Inventar (Runde 33) */
#include "re15_msg.h"         /* Text setzen + oeffnen (re15_msg_install_text / re15_dialog_open_mask) */
#include "re15_scd.h"         /* g_scd (Dialog-Zustand), re15_game_flag_get(12,31) = Ja/Nein-Antwort */
#include <stdio.h>
#include <stdlib.h>

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
 *   [PORT-S] = PORT-Entscheidung, semantisch gestuetzt:
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
    /* ⛔ 1071 ist KEINE Port-Annahme mehr (stand hier bis Runde 35 als [PORT-S]).
     * Die ungerade Id IST derselbe Raum in Elzas Dateivariante: der Raumlader
     * addiert das Elza-Bit auf den CD-Dateiindex (`srl a0,a0,31` @0x800397e4,
     * `addu a0,a0,v0` @0x800397ec), und die Stage-0-Tabelle @0x8007429c laeuft in
     * Schritten von 3 — Elza-RDT = Leon-RDT + 1. Derselbe Ort, derselbe
     * Ortsnamen-Index. */
    { 0x1070, 0x14, 1 }, { 0x1071, 0x14, 1 },   /* STAGE1 main20 — Telefon [PATCH] */
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

/* ===== Runde 33: Speichern nur mit Memory Card (RE2-Farbband-Ablauf) ===================
 * Herleitung + alle Bytes: analysis/befunde_runde33/speichern_memory_card.md.
 *
 * ZEICHENSATZ: RE2 und RE1.5 teilen die Buchstaben (A = 0x1D, a = 0x3D, '?' = 0x1B,
 * ',' = 0x18) — die Buchstabenbytes unten sind 1:1 die RE2-Bytes. Umgesetzt werden NUR die
 * Steuercodes, jeweils auf das RE1.5-Gegenstueck, das die ausgelieferten RE1.5-Raumtexte an
 * derselben Stelle benutzen:
 *   RE2 fc      Zeilenwechsel  -> RE1.5 08     (ROOM1150 msg 1 "progress 08 with this")
 *   RE2 fd 00   Seite (Taste)  -> RE1.5 02 00  (ROOM1150 msg 1 "phone. 02 00 You can")
 *   RE2 f9 01 / f9 00  Farbe 1 / Ende -> RE1.5 05 01 / 05 00 (Gegenstandsname, ROOM10D0
 *                              msg 9 "You've used the 05 01 Blue Keycard 05 00")
 *   RE2 fb 00   Ja/Nein        -> RE1.5 03 02  (alle 90 RE1.5-Auswahltexte enden 03 02 01 00)
 *   RE2 fe 00   Ende           -> RE1.5 01 00
 *   RE2 01 '.'                 -> RE1.5 57 '.' (RE1.5: 0x01 = Ende; Punkt = 0x57)
 * "Ink Ribbon" (RE2 `25 4a 47 00 2e 45 3e 3e 4b 4a`) -> "Memory Card", der RE1.5-Itemname
 * von 0x21 aus DEBUG.BIN @0x800C4BEA. */

/* MIT Karte — RE2 Meldung (0x100,1) Schlussseite ab RAM 0x8009F064:
 *   33 45 48 48 00 55 4b 51 00 51 4f 41 00 50 44 41 fc f9 01 <Ink Ribbon> f9 00 1b fb 00 fe 00
 *   "Will you use the" \n [Farbe 1]"Ink Ribbon"[Farbe 0] "?" [Ja/Nein] [Ende] */
static const uint8_t k_frage_rest[] = {
    0x33,0x45,0x48,0x48,0x00,0x55,0x4b,0x51,0x00,0x51,0x4f,0x41,0x00,0x50,0x44,0x41, /* Will you use the */
    0x08,                                                                            /* fc -> 08 */
    0x05,0x01,                                                                       /* f9 01 -> 05 01 */
    0x29,0x41,0x49,0x4b,0x4e,0x55,0x00,0x1f,0x3d,0x4e,0x40,                          /* Memory Card @0x800C4BEA */
    0x05,0x00,                                                                       /* f9 00 -> 05 00 */
    0x1b,                                                                            /* ? */
    0x03,0x02,                                                                       /* fb 00 -> 03 02 */
    0x01,0x00                                                                        /* fe 00 -> 01 00 */
};

/* OHNE Karte — RE2 Meldung (0x100,0) Schlussseite ab RAM 0x8009EFE6:
 *   25 42 00 25 00 44 3d 40 00 3d 4a 00 f9 01 <Ink Ribbon> f9 00 18 00 25 fc
 *   3f 4b 51 48 40 00 4f 3d 52 41 00 49 55 00 4c 4e 4b 43 4e 41 4f 4f 01 01 01 fe 00
 *   "If I had an " [Farbe 1]"Ink Ribbon"[Farbe 0] ", I" \n "could save my progress..."
 * EINZIGE Wortaenderung neben dem Namen: "an" -> "a" (Artikel vor Konsonant; `3d 4a` -> `3d`). */
static const uint8_t k_hinweis_rest[] = {
    0x25,0x42,0x00,0x25,0x00,0x44,0x3d,0x40,0x00,0x3d,0x00,                          /* If I had a_ */
    0x05,0x01,                                                                       /* f9 01 -> 05 01 */
    0x29,0x41,0x49,0x4b,0x4e,0x55,0x00,0x1f,0x3d,0x4e,0x40,                          /* Memory Card @0x800C4BEA */
    0x05,0x00,                                                                       /* f9 00 -> 05 00 */
    0x18,0x00,0x25,                                                                  /* , I */
    0x08,                                                                            /* fc -> 08 */
    0x3f,0x4b,0x51,0x48,0x40,0x00,0x4f,0x3d,0x52,0x41,0x00,0x49,0x55,0x00,           /* could save my_ */
    0x4c,0x4e,0x4b,0x43,0x4e,0x41,0x4f,0x4f,                                         /* progress */
    0x57,0x57,0x57,                                                                  /* 01 01 01 -> 57 57 57 */
    0x01,0x00                                                                        /* fe 00 -> 01 00 */
};

int re15_savepoint_card_slot(void)
{
    return re15_inv_find_item(RE15_SAVEPOINT_CARD_ITEM);
}

int re15_savepoint_build_text(const uint8_t *orig, int orig_len, int with_card,
                              uint8_t *out, int cap)
{
    /* Anfang der ausgelieferten Meldung bis einschliesslich des 1./2. `02 xx`. Die
     * Steuercodes werden wie im Dialog-FSM gelaufen (2-Byte-Codes 02/04/05/06/09/0A/0B),
     * damit ein Argumentbyte nie als Code gelesen wird. Alle 16 Stellen haben die Form
     * `04 02` Satz1 `02 00` Satz2 `08` Satz2b `02 00` Satz3 `08` Satz3b `01 00`
     * (rdt_msgdump ueber die 16 Raeume, Dossier §2). Fehlt die Form, wird nur `04 02`
     * (Schreibtempo) uebernommen und RE2s Schlussseite steht allein. */
    int want = with_card ? 2 : 1, seen = 0, cut = 0;
    if (orig && orig_len > 0) {
        for (int i = 0; i < orig_len; ) {
            uint8_t b = orig[i];
            if (b == 0x01 || b == 0x03) break;
            if (b == 0x02) {
                if (i + 1 >= orig_len) break;
                if (++seen == want) { cut = i + 2; break; }
                i += 2; continue;
            }
            if (b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) { i += 2; continue; }
            i += 1;
        }
    }
    static const uint8_t k_tempo[] = { 0x04, 0x02 };   /* RE1.5-Kopf aller 16 Meldungen */
    const uint8_t *pre = cut ? orig : k_tempo;
    int npre = cut ? cut : (int)sizeof k_tempo;
    const uint8_t *rest = with_card ? k_frage_rest : k_hinweis_rest;
    int nrest = with_card ? (int)sizeof k_frage_rest : (int)sizeof k_hinweis_rest;
    if (!out || cap < npre + nrest) return 0;
    for (int i = 0; i < npre; i++)  out[i] = pre[i];
    for (int i = 0; i < nrest; i++) out[npre + i] = rest[i];
    return npre + nrest;
}

/* RE2 Unterzustand DAT_800D424A: 0 = frei, 1 = Abfrage offen (@0x80051b54-5c). */
static int s_ask = 0;
static int s_ask_sel = 0;   /* die Ja/Nein-Auswahl stand wirklich auf dem Schirm */

void re15_savepoint_examine(unsigned room_id, uint8_t msg_id, uint32_t pause_mask)
{
    re15_savepoint_latch_loc(room_id);   /* Ortsindex (Patch-Analog, wie bisher beim Examine) */
    int olen = 0;
    const unsigned char *orig = re15_msg_get_raw((int)msg_id, &olen);
    int slot = re15_savepoint_card_slot();   /* RE2 @0x80051b34 jal 0x800696cc(0x1E) */
    uint8_t text[128];
    int n = re15_savepoint_build_text(orig, olen, slot >= 0, text, (int)sizeof text);
    re15_msg_install_text(RE15_SAVEPOINT_MSG_ID, text, (size_t)n);
    /* NICHT blockierend: der Skript-Freeze kommt aus der Maske (0xffff0000 enthaelt
     * RE15_PAUSE_SCD), wie bei der ausgelieferten Meldung an derselben Stelle. */
    re15_dialog_open_mask(RE15_SAVEPOINT_MSG_ID, 0, pause_mask);
    s_ask     = (slot >= 0);             /* RE2: mit Farbband -> Unterzustand 1 */
    s_ask_sel = 0;
    if (getenv("RE15_MSG_LOG"))
        fprintf(stderr, "[save] room=%04x msg=%d card_slot=%d -> %s\n", room_id, msg_id, slot,
                slot >= 0 ? "FRAGE (Memory Card benutzen?)" : "HINWEIS (keine Memory Card)");
}

int re15_savepoint_asking(void) { return s_ask; }

int re15_savepoint_poll(void)
{
    if (!s_ask) return 0;
    if (g_scd.message_id != RE15_SAVEPOINT_MSG_ID) { s_ask = 0; return 0; }  /* von fremdem Text verdraengt */
    if (g_scd.message_select) s_ask_sel = 1;
    /* RE2 @0x80051ba4-b0: warten, solange das Nachrichtensystem belegt ist (DAT_800E873C & 0x80).
     * Im Port: der Dialog-FSM laeuft noch (Zustand 7 = Untertitel-Nachhall zaehlt als fertig —
     * die Antwort ist dort schon geschrieben, msg_common.c fsm==6). */
    if (g_scd.message_fsm_active && g_scd.message_fsm != 7) return 0;
    s_ask = 0;
    /* Antwort = Flag (12,31), vom Dialog-FSM beim Bestaetigen geschrieben (msg_common.c
     * Zustand 3: re15_game_flag_set(12,31,choice), 0 = JA) — RE2 @0x80051bb4 `andi v0,v1,0x1`. */
    int ja = s_ask_sel && re15_game_flag_get(12, 31) == 0;
    if (getenv("RE15_MSG_LOG"))
        fprintf(stderr, "[save] Antwort: %s\n", ja ? "JA -> Speicherbildschirm" : "NEIN -> zurueck");
    return ja;
}

void re15_savepoint_reset(void)
{
    s_savepoint_pending = 0; s_savepoint_cut = -1; s_savepoint_loc = 0;
    s_ask = 0; s_ask_sel = 0;
}
