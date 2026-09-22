/*
 * item_discard_common.c — "You don't need this key any more. Discard it?"
 *
 * Die vollstaendige Herleitung (Wortlaut @0x800C508B, RE2-Regel LAB_80051718,
 * Benutzungsstellen-Zensus, Sackgassen-Beweis) steht im Kopf von
 * include/re15_item_discard.h. Hier nur noch, was der Code TUT, mit den Adressen
 * an den einzelnen Zeilen.
 */
#include "re15_item_discard.h"
#include "re15_inventory.h"    /* g_inv, re15_inv_find_item, re15_inv_remove_slot        */
#include "re15_scd.h"          /* g_scd.message_active / message_fsm_active              */
#include "re15_item_prompt.h"  /* re15_item_prompt_walk — Glyphenzahl des Skripts        */
#include "re15_msg_select.h"   /* re15_msg_select_blink_tick — Blink-Gatter @0x80028600  */

#include "gen/discard_sites.inc"

/* Port-Prompt-Schluessel 8 -> BSS-Skript [6] (item_prompt_common.c:60). */
#define RE15_DISCARD_PROMPT_KEY 8

/* Schreibmaschinen-Kadenz: identisch zum Aufnahme-Prompt, byte-true FUN_80028134
 * (@0x800281b0-c4 `DAT_800b8524 = 2 << s1`, s1 = 0 -> 1 Glyphe je 2 Bilder). Der
 * Schnellvorlauf ist derselbe: HELD virtuell 0x4000 -> Zaehler -= 4 (@0x80028228),
 * 2 Glyphen je Ablauf (@0x8002822c `sltiu 4` -> `ori 2`). Uebernommen aus
 * item_modal_common.c, damit beide Prompts identisch tippen. */
#define DISCARD_MSG_RELOAD 2

enum { D_AUS = 0, D_WARTET, D_FRAGT };

static int     s_zustand   = D_AUS;
static uint8_t s_item      = 0;
static int     s_slot      = -1;
static int     s_choice    = 0;    /* 0 = Yes (vorbelegt), 1 = No                       */
static int     s_reveal    = 0;
static int     s_reveal_total = 0;
static int     s_reveal_timer = 0;
static uint8_t s_blink     = 0;    /* DAT_800b8525 dieser Abfrage (@0x800285e8)         */
static int     s_faden     = -1;   /* SCD-Faden, der die Nachricht ausgab               */
static int     s_gefragt   = 0;
static int     s_weggeworfen = 0;

/* Anzahl 0 zurueckdrehen — dasselbe, was RE2 im Nein-Zweig tut (@0x800517C4 `sb v1,count`
 * mit v1 = 1). Seit die Anzahl erst beim Fragen faellt (siehe re15_discard_tick), kann das
 * nur noch den Zustand D_FRAGT betreffen; in D_WARTET ist die Anzahl unberuehrt
 * (@0x800517f4 verlaesst die Routine VOR dem Dekrement @0x80051810). */
static void anzahl_zurueck(void)
{
    if (s_zustand != D_AUS && s_slot >= 0 && s_slot < RE15_INV_MAX_SLOTS
        && g_inv.slots[s_slot].id == s_item && g_inv.slots[s_slot].qty == 0)
        g_inv.slots[s_slot].qty = 1;
}

void re15_discard_reset(void)
{
    /* HART: neues Spiel / Spielstand laden. Alles faellt weg.
     * ⛔ NICHT beim Raumwechsel — dafuer gibt es re15_discard_room_change(). */
    anzahl_zurueck();
    s_zustand = D_AUS; s_item = 0; s_slot = -1; s_choice = 0; s_faden = -1;
    s_reveal = s_reveal_total = s_reveal_timer = 0; s_blink = 0;
}

void re15_discard_room_change(void)
{
    /* ⛔ DER RAUMWECHSEL DARF DIE VORMERKUNG NICHT WEGWERFEN.
     *
     * ============================================================================
     * ZUERST DIE EHRLICHE ANTWORT: IN RE2 KANN DIESE LAGE GAR NICHT ENTSTEHEN
     * ============================================================================
     * Die Frage "was tut RE2, wenn der Spieler nach dem Aufschliessen den Raum wechselt,
     * bevor die Abfrage kam?" hat in RE2 keinen Fall — und DAS ist disassembliert:
     *
     *  (1) Die Vormerkung entsteht in DERSELBEN Routine, die die ausloesende Nachricht
     *      "You have used the <X>." mit der Freeze-Maske 0xFF000000 oeffnet:
     *          @0x8005164c  jal  FUN_8002fe38
     *          @0x80051650  _lui a3,0xff00          (Delay-Slot = 4. Argument)
     *          @0x80051670  sw   v0,DAT_800d4498    Fortsetzung einhaengen
     *  (2) Ab da ruft die HAUPTSCHLEIFE die Fortsetzung JEDES Bild:
     *          @0x80026370  lw   v1,-0x7d50(at)     at = s3+0x10000, s3 = 0x800cc1e8
     *          @0x8002637c  beq  v1,zero,LAB_8002638c
     *          @0x80026384  jalr v1
     *  (3) Ihr EINZIGER Aufschub ist das Belegt-Bit 0x80 von DAT_800e873c:
     *          @0x800517e0  lbu  v0,-0x78c4(v0)     DAT_800e873c
     *          @0x800517f0  andi v0,v0,0x80
     *          @0x800517f4  bne  v0,zero,LAB_80051870   -> return, NICHTS angefasst
     *  (4) Genau dieses Bit haelt auch den Freeze. FUN_8002fe38 setzt beim Oeffnen
     *      DAT_800e873c = 0x80, legt die Maske nach DAT_800e8760 und sichert das
     *      Steuerwort nach DAT_800e875c (Decompile RE2_Quellcode_V2/FUN_8002fe38.c,
     *      Modus-Zweig 0x100); FUN_8003027c legt sie an und nimmt sie erst beim
     *      SCHLIESSEN zurueck:
     *          @0x800307e0  lbu  v0,0x5c8c(s1)      s1 = 0x800e2ab0 -> DAT_800e873c
     *          @0x800307e8  andi v0,v0,0x7f         Belegt-Bit WEG
     *          @0x800307ec  sb   v0,0x5c8c(s1)
     *          @0x800307e4  lw   v1,0x5cac(s1)      DAT_800e875c (Steuerwort vorher)
     *          @0x800307f4  sw   v1,DAT_800cfbdc    Freeze ZURUECK
     *  (5) Und die Nachricht schliesst nur auf die Bestaetigungstaste — wobei sie im
     *      SELBEN Bild das Pad-Vorwort stopft, es gibt dort also keine zweite Flanke:
     *          @0x800307a0  lw   v0,DAT_800ce310
     *          @0x800307a8  andi v0,v0,0x3000       Bestaetigen?
     *          @0x800307ac  beq  v0,zero,default    nein -> Nachricht bleibt offen
     *          @0x800307b0  _ori v0,zero,0xffff
     *          @0x800307b8  sw   v0,DAT_800ce30c    Pad-Vorwort gestopft
     *  (6) Im naechsten Durchgang dekrementiert die Fortsetzung (@0x80051810), fragt
     *      (@0x80051834) und legt die Maske sofort wieder an (@0x80051844 `lui v1,0xff00`
     *      / @0x80051850 `sw v0,DAT_800cfbdc`).
     *
     * => Die Spanne "vorgemerkt" ist in RE2 GENAU die Spanne "Nachricht haelt 0xFF000000".
     *    Ein Bild mit vorgemerkter Abfrage UND freiem Spieler gibt es dort nicht, also
     *    kann der Spieler in diesem Fenster auch keinen Raum wechseln. Das Loch, das
     *    dieser Port hatte, ist eine PORT-Eigenschaft — es entsteht erst durch die
     *    laengere Port-Wartezeit (Ende des ausloesenden Unterprogramms statt Ende der
     *    Nachricht; Begruendung im Kopf von re15_discard_tick).
     *
     * ============================================================================
     * WAS AUS RE2 TROTZDEM MESSBAR IST — UND NUR DAS
     * ============================================================================
     * Der armierte Zustand sind ZWEI Zellen im raum-UEBERGREIFENDEN Arbeitsblock
     * (Basis 0x800cc1e8, @0x8005172c `addiu s0,s0,-0x3e18`):
     *     +0x82b0 = DAT_800d4498   der Fortsetzungs-Zeiger
     *     +0x8061 = DAT_800d4249   Phase/Platz (0 = Antwort-Zweig, sonst Platz+1;
     *                              @0x8005171c `lbu v1` / @0x800517e4 `addiu v1,v1,-1`)
     * Im SELBEN Block liegt das Inventar: +0x8854 = DAT_800d4a3c (@0x80051774/@0x80051808).
     *
     * Vollzensus per Byte-Muster ueber den ganzen Ghidra-Dump nach beiden Adressierungs-
     * formen des Zeigers (`b0 82 ?? ac` = +0x82b0 und `98 44 ?? ac` = DAT_800d4498(at)) —
     * 6 Schreib- und 2 Lesestellen im GANZEN Programm:
     *     @0x80051670  sw v0,...   LAB_80051718 einhaengen (Schluessel benutzt)
     *     @0x800517d0  sw zero,... nach der ANTWORT aushaengen
     *     @0x80051860  sw zero,... Zaehler != 0 -> keine Frage, aushaengen
     *     @0x80052168  sw v0,...   DAT_800522cc einhaengen (die andere AOT-Klasse)
     *     @0x80052294  sw v0,...   dito
     *     @0x800524e8  sw zero,... deren eigenes Aushaengen
     *     @0x80026370  lw v1,...   HAUPTSCHLEIFE (Punkt 2 oben)
     *     @0x80052048  lw v0,...   "haengt schon eine Fortsetzung?" -> dann nicht starten
     * ⛔ Ghidras eigene XREF-Liste nennt nur FUENF Schreibstellen: @0x800524e8 adressiert
     * ueber s1 (@0x800524e4 `addu at,s1,at`) und ist dort unbeschriftet. Ohne die
     * Byte-Muster-Suche faehrt man mit einer unvollstaendigen Liste.
     * KEINE der sechs liegt auf einem Raumwechsel-, Tuer- oder Lade-Pfad. Dazu:
     *   - Die Fortsetzung selbst ist RESIDENT — LAB_80051718 liegt in der EXE (0x8005xxxx),
     *     nicht in einem Stage-Overlay (0x80100000); sie bliebe ueber einen Raumwechsel
     *     hinweg aufrufbar. Laege sie im Overlay, waere Ueberleben unmoeglich.
     *   - Der Gegenstand ist im Wartezustand unversehrt: @0x800517f4 kehrt VOR dem
     *     Dekrement @0x80051810 um, ohne die Anzahl anzufassen.
     * => Die Zellen WUERDEN einen Raumwechsel ueberstehen. Dass RE2 das nutzt, ist damit
     *    NICHT gezeigt — es braucht es nie (siehe oben).
     *
     * ============================================================================
     * DIE PORT-ENTSCHEIDUNG, ALS SOLCHE BENANNT
     * ============================================================================
     * Der Port schliesst das Fenster dort, wo RE2 es nicht hat: solange vorgemerkt ist,
     * haelt re15_discard_pad_locked() den Pad-Riegel (Beleg dort). Damit kann der SPIELER
     * in diesem Fenster keinen Raum wechseln — dieselbe Lage wie RE2.
     * Ein SKRIPT kann es weiterhin (gerade das Unterprogramm, auf dessen Ende gewartet
     * wird, darf einen Raumwechsel enthalten). Fuer diesen Fall faellt die Vormerkung
     * NICHT weg, weil das der einzige Ausgang ohne stummen Verlust ist: Item 0x44
     * "Minidisc Player w/ Disc" hat genau EINE Benutzungsstelle (discard_sites.inc), es
     * wuerde also nie wieder gefragt und der Gegenstand bliebe fuer immer liegen. Die
     * Wahl folgt der RE2-Datenlage (Zellen im raum-uebergreifenden Block, kein Schreiber
     * auf dem Raum-Pfad, residente Fortsetzung), ist aber eine PORT-Entscheidung und
     * keine gemessene RE2-Verhaltensweise.
     *
     * Was NICHT mitgenommen wird: der Faden-Index (die Faeden des alten Raums gibt es
     * nicht mehr) und der Inventar-Platz (das Inventar kann nachgerueckt sein) — der
     * Platz wird beim Fragen neu gesucht. */
    if (s_zustand == D_AUS) return;
    if (s_zustand == D_FRAGT) {
        /* Kann der Spieler im Auslieferungsstand nicht ausloesen (die sichtbare Abfrage
         * friert den Spielschritt ein, game_step_common.c), ein Skript-Raumwechsel aber
         * schon: die bereits gefallene Anzahl zurueck und auf WARTEN zurueckstufen. */
        anzahl_zurueck();
        s_reveal = s_reveal_total = s_reveal_timer = 0; s_blink = 0; s_choice = 0;
        s_zustand = D_WARTET;
    }
    s_faden = -1;
    s_slot  = -1;
}

uint8_t re15_discard_pending_item(void) { return (uint8_t)(s_zustand == D_AUS ? 0 : s_item); }

void re15_discard_restore(uint8_t item)
{
    re15_discard_reset();
    if (!item) return;
    if (re15_inv_find_item(item) < 0) return;   /* nicht mehr dabei -> nichts vorzumerken */
    s_item = item; s_slot = -1; s_faden = -1; s_choice = 0;
    s_zustand = D_WARTET;
}

void re15_discard_notice_message(unsigned room_id, uint8_t msg_id, int thread_slot)
{
    if (s_zustand != D_AUS) return;            /* eine Abfrage laeuft schon              */
    uint8_t item = 0;
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        if (re15_discard_sites[i].room == (uint16_t)room_id
            && re15_discard_sites[i].msg == msg_id) { item = re15_discard_sites[i].item; break; }
    }
    if (!item) return;                         /* keine Benutzungsstelle                 */

    /* Der Gegenstand muss WIRKLICH getragen werden. RE1.5 prueft das im Skript NICHT
     * (ROOM4000 sub02 z.B. fragt gar nicht nach Besitz) — der Port darf deshalb nicht
     * anbieten, etwas wegzuwerfen, was gar nicht da ist. */
    int slot = re15_inv_find_item(item);
    if (slot < 0 || slot >= RE15_INV_MAX_SLOTS) return;
    if (g_inv.slots[slot].qty == 0) return;    /* schon verbraucht -> nichts zu tun      */

    /* ⛔ HIER wird NICHT dekrementiert. RE2 haengt an dieser Stelle nur die Fortsetzung
     * LAB_80051718 ein (`sw v0,[0x800D4498]` @0x80051670); der Zaehler faellt erst in der
     * Fortsetzung, und zwar NACH der Warte-Schranke — siehe re15_discard_tick. Wer schon
     * beim Einhaengen dekrementiert, zeigt dem Spieler waehrend der ganzen Wartezeit einen
     * Schluessel mit Anzahl 0 im Inventar. */
    s_item    = item;
    s_faden   = (thread_slot >= 0 && thread_slot < SCD_THREAD_COUNT) ? thread_slot : -1;
    s_slot    = slot;
    s_choice  = 0;                             /* Yes vorbelegt — RE2 setzt die Auswahl-
                                                * zelle beim Nachrichtenstart auf exakt
                                                * 0x80, unteres Nibble 0 = Yes
                                                * (@0x8002FE88); der Aufnahme-Prompt des
                                                * Ports startet ebenso auf 0.            */
    s_zustand = D_WARTET;                      /* erst die Raum-Nachricht ausreden lassen */
}

void re15_discard_tick(uint16_t pad_edge, uint16_t pad_held)
{
    if (s_zustand == D_AUS) return;

    if (s_zustand == D_WARTET) {
        /* RE2 wartet an genau dieser Stelle darauf, dass das Nachrichtensystem frei wird:
         *   800517E0 lbu  v0,[0x800E873C]
         *   800517F0 andi v0,v0,0x80
         *   800517F4 bne  v0,zero,0x80051870   ; belegt -> weiter warten
         * Im Port sind das die beiden Nachrichten-Zustaende von g_scd. */
        if (g_scd.message_active || g_scd.message_fsm_active) return;

        /* ⛔ ZWEITE SCHRANKE: das Unterprogramm, das die Nachricht ausgegeben hat,
         *    muss ZU ENDE sein.
         *
         * Warum es die Nachrichten-Schranke allein NICHT tut — gemessen an der echten
         * ROOM1090 sub03 (Messung unten in tests/unit/r21_discard_wegwerfen.c,
         * RE15_DISCARD_SZENE_MESSUNG=1), Bild fuer Bild im echten VM:
         *     Bild   2  Message_on 9 @0x2502  "You've used the Fire Extinguisher."
         *     Bild  72  Nachricht ausgeredet  -> hier ging die Abfrage auf
         *     Bild  73  Set(2,7,1) @0x2508    -> hier haette die Szene begonnen
         *     Bild 677  Evt_end    @0x26E4    -> Ende des Unterprogramms
         * Die Abfrage erwischte also GENAU das eine Bild ZWISCHEN der Nachricht und dem
         * Beginn der Szene. Weil ein sichtbarer Prompt den SCD-Takt anhaelt
         * (platform/pc/main.c: `re15_discard_frozen()` ueberspringt scd_vm_tick), kam die
         * Szene danach gar nicht mehr in Gang — sub03 stand auf @0x2508 fest.
         *
         * ⛔ Damit ist auch belegt, dass ein Riegel auf dem Szenen-Fenster flag(2,7)
         * NICHT reicht: im Bild 72 ist das Fenster noch gar nicht offen. (Der Riegel
         * bleibt trotzdem unten stehen, fuer die Faelle, in denen die Nachricht INNERHALB
         * eines Fensters faellt.)
         *
         * Der Faden dagegen laeuft ueber die ganze Szene: gemessen war in jedem der Bilder
         * 60..90 genau Faden 0 aktiv, und er bleibt es bis zum Evt_end in Bild 677.
         *
         * BEGRUENDUNG, kein Fund: RE1.5 hat fuer diese Abfrage KEINEN eigenen Ausloeser
         * (Befund analysis/befunde_2026-09-21/discard-re2-mechanismus.md) — der Port
         * haengt sie an op_message_on. RE2 braucht die Schranke nicht, weil dort der
         * TUER-Handler ausloest und die Fortsetzung aus der Hauptschleife gerufen wird
         * (@0x80026384 `jalr v1` auf DAT_800D4498), also nie aus einem laufenden Ereignis
         * heraus. Diese Schranke stellt fuer RE1.5 genau diese Eigenschaft her: die
         * Abfrage laeuft erst, wenn das ausloesende Ereignis vorbei ist. Sie ist damit
         * eine PORT-Schranke mit RE2-Vorbild, keine nachgewiesene RE1.5-Konstante. */
        if (s_faden >= 0 && s_faden < SCD_THREAD_COUNT && g_scd.threads[s_faden].active)
            return;
        /* DRITTE SCHRANKE: laeuft (noch) eine Szene, wartet die Abfrage ebenfalls.
         * flag(2,7) ist byte-true das Pause-Bit 0x01000000 in DAT_800aca40, das die
         * Eingabe auf 0xf000 maskiert (@0x800304f4-@0x8003051c) — woertlich "der Spieler
         * hat keine Kontrolle". Herleitung + Zensus ueber 206 RDTs: re15_cine_active
         * (engine/src/game_state.c). Greift, wenn ein ANDERER Faden die Szene haelt. */
        if (re15_cine_active()) return;
        /* Den Platz JETZT (neu) suchen: nach einem Raumwechsel ist der alte Index
         * ungueltig (s_slot == -1, siehe re15_discard_room_change), und das Inventar kann
         * zwischendurch nachgerueckt sein. Ist der Gegenstand ganz weg (Itembox o.ae.),
         * gibt es nichts mehr wegzuwerfen. */
        if (s_slot < 0 || s_slot >= RE15_INV_MAX_SLOTS || g_inv.slots[s_slot].id != s_item)
            s_slot = re15_inv_find_item(s_item);
        if (s_slot < 0 || s_slot >= RE15_INV_MAX_SLOTS || g_inv.slots[s_slot].id != s_item
            || g_inv.slots[s_slot].qty == 0) {
            re15_discard_reset(); return;
        }
        /* JETZT erst faellt der Zaehler — byte-true die Reihenfolge von LAB_80051718:
         * die Warte-Schranke @0x800517F4 `bne v0,zero,0x80051870` verlaesst die Routine,
         * OHNE die Anzahl anzufassen; erst dahinter steht
         *   80051808 lbu   v0,count
         *   80051810 addiu v0,v0,-1      ; Zaehler -= 1
         *   8005181C sb    v0,count
         *   80051824 bne   v0,zero,...   ; > 0 -> KEINE Abfrage
         *   80051834 jal   FUN_8002FE38  ; == 0 -> FRAGEN (a1=0x100 @0x8005182C, a2=9)
         * Gemessen (tools/gen_discard_sites.py): alle neun Gegenstaende werden mit Anzahl 1
         * ausgegeben und haben genau EINE Benutzungsstelle — die Anzahl faellt also von 1
         * auf 0 und es wird gefragt. Die Regel steht trotzdem als Zaehler da: ein Gegenstand
         * mit mehreren Benutzungsstellen wuerde sonst zu frueh gemeldet. */
        uint8_t n = (uint8_t)(g_inv.slots[s_slot].qty - 1);   /* @0x80051810 */
        g_inv.slots[s_slot].qty = n;                          /* @0x8005181C */
        if (n != 0) { re15_discard_reset(); return; }         /* @0x80051824 */
        s_reveal       = 0;
        s_reveal_total = re15_item_prompt_walk(RE15_DISCARD_PROMPT_KEY, s_item,
                                               0, 0, 0);   /* nur zaehlen */
        s_reveal_timer = 1;                    /* Startwert 1 << s1, s1 = 0 (@0x800281a0-ac) */
        s_blink        = 0;                    /* Blink-Zaehler beim Oeffnen (@0x80027eb0)   */
        s_zustand      = D_FRAGT;
        s_gefragt++;
        return;
    }

    /* --- D_FRAGT: tippen, dann Yes/No ------------------------------------------------ */
    if (s_reveal < s_reveal_total) {
        int budget = 1;                                   /* s2 = 1            @0x800281d8 */
        int t0     = s_reveal_timer;
        s_reveal_timer = t0 - 1;                          /* @0x800281f0/f8               */
        if (((s_reveal_timer & 0xff) != 0) && (pad_held & 0x4000)) {
            s_reveal_timer = t0 - 4;                      /* @0x80028228                  */
            if (DISCARD_MSG_RELOAD < 4) budget = 2;       /* @0x8002822c/38               */
        }
        if (s_reveal_timer > 0) return;                   /* @0x8002823c-48               */
        s_reveal += budget;
        if (s_reveal > s_reveal_total) s_reveal = s_reveal_total;
        s_reveal_timer = DISCARD_MSG_RELOAD;
        return;                                           /* vor dem Ende keine Eingabe   */
    }

    /* Auswahl + Bestaetigen — dieselben virtuellen Bits wie der Aufnahme-Prompt
     * (item_modal_common.c Zustand 6): 0x3000 = Menue links/rechts (roh Steuerkreuz),
     * 0x4000 = BESTAETIGEN (roh SQUARE, Preset-Tabelle @0x80073dbc[14]). */
    /* Blink-Zaehler des Cursors — ein Bild, byte-true @0x800285d4 vor @0x800285f0
     * (Herleitung in include/re15_msg_select.h). Erst nullen, dann dekrementieren:
     * ein Tastendruck macht den Cursor im SELBEN Bild wieder sichtbar. */
    s_blink = re15_msg_select_blink_tick(s_blink, (pad_edge & 0x3000) != 0);
    if (pad_edge & 0x3000) s_choice ^= 1;      /* @0x800285d8 xori v0,v0,0x1 */
    /* ABBRECHEN mit CROSS (virtuell 0x8000) = "No". Das ist die Konvention des Ports
     * (Bestaetigen SQUARE, Abbrechen CROSS) und KEINE byte-true Regel: RE2s Abfrage
     * kennt nur den Bestaetigen-Knopf (FUN_80030844, DAT_800ce310 & 0x1000), die
     * physische Taste dahinter ist in Untersuchung A ausdruecklich NICHT aufgeloest
     * worden. Wer abbricht, behaelt den Gegenstand — der sichere Ausgang. */
    if (pad_edge & 0x8000) { s_choice = 1; }
    else if (!(pad_edge & 0x4000)) return;

    if (s_choice == 0) {
        /* JA: Slot leeren + nachruecken. RE2: `sb zero,id` @0x80051774,
         * `sb zero,flags` @0x80051794, `jal FUN_80069714` @0x80051798.
         * Der Port hat dafuer re15_inv_remove_slot (Consume @0x8004aef0-af28) samt
         * anschliessender Kompaktierung (FUN_8004dadc). */
        if (s_slot >= 0 && s_slot < RE15_INV_MAX_SLOTS && g_inv.slots[s_slot].id == s_item) {
            re15_inv_remove_slot(s_slot);
            s_weggeworfen++;
        }
        s_zustand = D_AUS; s_item = 0; s_slot = -1; s_faden = -1;
        s_reveal = s_reveal_total = s_reveal_timer = 0; s_blink = 0;
        return;
    }
    /* NEIN: Anzahl auf 1 zurueck (@0x800517C4) — sonst liefe sie beim naechsten
     * Gebrauch auf 0xFF unter, und es wuerde nie wieder gefragt. Die Abfrage kommt
     * beim naechsten Gebrauch erneut, genau wie in RE2. */
    re15_discard_reset();
}

int re15_discard_active(void) { return s_zustand != D_AUS; }
int re15_discard_frozen(void) { return s_zustand == D_FRAGT; }

int re15_discard_pad_locked(void)
{
    /* ⛔ SOLANGE VORGEMERKT IST, HAT DER SPIELER KEINE KONTROLLE — byte-true die Lage,
     * in der RE2 sich waehrend derselben Spanne befindet.
     *
     * RE2 oeffnet die ausloesende Nachricht "You have used the <X>." mit der Freeze-Maske
     * 0xFF000000 als viertem Wort (@0x8005164c `jal FUN_8002fe38` / @0x80051650
     * `_lui a3,0xff00`). FUN_8002fe38 legt sie weg und sichert das aktuelle Steuerwort:
     *     @0x8002fe90  sw a3,-0x78a0(at)   -> DAT_800e8760  (die Maske)
     *     @0x8002fea4  sw v1,-0x78a4(at)   -> DAT_800e875c  (Steuerwort vorher)
     * Der Nachrichten-Takt FUN_8003027c legt sie beim OEFFNEN an und nimmt sie erst beim
     * SCHLIESSEN wieder weg (Decompile RE2_Quellcode_V2/FUN_8003027c.c, param_1 = 0x800e2ab0):
     *     case 0:       DAT_800cfbdc |= *(param_1 + 0x5cb0)   ; = DAT_800e8760
     *     LAB_800307e0: *(param_1 + 0x5c8c) &= 0x7f           ; Belegt-Bit weg
     *                   DAT_800cfbdc  = *(param_1 + 0x5cac)   ; = DAT_800e875c
     * Und die Abfrage legt dieselbe Maske sofort wieder an
     *     @0x80051844  lui v1,0xff00
     *     @0x80051850  sw  v0,DAT_800cfbdc
     * Zwischen Nachrichten-Ende und Abfrage liegt in RE2 hoechstens EIN Bild, denn der
     * Aufschub der Fortsetzung ist GENAU dasselbe Belegt-Bit, das den Freeze haelt
     * (@0x800517f0/@0x800517f4 gegen LAB_800307e0). Und in diesem einen Bild gibt es keine
     * Tastenflanke, weil das Schliessen der Nachricht das Pad-Vorwort stopft — selbst
     * disassembliert, FUN_8003027c case 5:
     *     @0x800307a0  lw   v0,DAT_800ce310
     *     @0x800307a8  andi v0,v0,0x3000     Bestaetigungstaste?
     *     @0x800307ac  beq  v0,zero,default  nein -> Nachricht bleibt offen
     *     @0x800307b0  _ori v0,zero,0xffff
     *     @0x800307b8  sw   v0,DAT_800ce30c  >>> Pad-Vorwort gestopft <<<
     * (Die Reihenfolge Fortsetzung/Nachrichten-Takt INNERHALB des Bildes ist dafuer
     * nicht noetig und deshalb hier auch nicht behauptet.)
     *
     * Der Port wartet an dieser Stelle laenger als RE2 (auf das Ende des ausloesenden
     * Unterprogramms — Begruendung im Kopf von re15_discard_tick). Genau diese
     * Verlaengerung hat das Loch aufgerissen: 81 Bilder mit freiem Pad und unsichtbarer,
     * armierter Abfrage (ROOM1100 sub02). Solange die Vormerkung steht, haelt der Port
     * deshalb denselben Riegel wie RE2 — Bit 0x01000000, dessen WIRKUNG in RE1.5
     * woertlich disassembliert ist:
     *     @0x800304f4  lw   v0,DAT_800aca40
     *     @0x800304f8  lui  v1,0x100
     *     @0x80030500  beq  v0,zero,LAB_80030520
     *     @0x80030514  andi v0,v0,0xf000      nur noch die vier Menuebits
     *     @0x8003051c  sw   v0,DAT_800ac768
     * Die vier Menuebits bleiben offen — die Abfrage selbst muss ja bedienbar sein.
     *
     * ⛔ NUR das Pad-Bit, NICHT die ganze 0xFF000000: RE15_PAUSE_SCD (0x02000000) wuerde
     * das ausloesende Unterprogramm anhalten, auf dessen Ende der Port gerade wartet —
     * das waere eine Verklemmung. RE2 hat das Problem nicht, weil seine Wartezeit die
     * Nachricht selbst ist. */
    return s_zustand != D_AUS;
}

int re15_discard_prompt(uint8_t *out_item, int *out_choice)
{
    if (s_zustand != D_FRAGT) return 0;
    if (out_item)   *out_item   = s_item;
    if (out_choice) *out_choice = s_choice;
    return RE15_DISCARD_PROMPT_KEY;
}

uint8_t re15_discard_blink(void)    { return s_blink; }
int re15_discard_reveal(void)       { return s_reveal; }
int re15_discard_reveal_total(void) { return s_reveal_total; }
int re15_discard_ready(void)        { return s_zustand == D_FRAGT && s_reveal >= s_reveal_total; }
int re15_discard_gefragt(void)      { return s_gefragt; }
int re15_discard_weggeworfen(void)  { return s_weggeworfen; }
