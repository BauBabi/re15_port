/*
 * RE1.5 Rebuilt — Rolltor ROOM1050/1051: Sicherung einsetzen mit Nahansicht, Tor erst danach.
 *
 * Runde 34 Nacht, Spur A. Nutzerwortlaut, Herleitung und Konstanten: include/re15_rolltor.h,
 * Dossier analysis/befunde_runde34_nacht/A_rolltor.md.
 *
 * EINORDNUNG (memory beta-zu-retail): "Frage am Weltobjekt, Gegenstand einsetzen" ist in RE1.5
 * FERTIG — ROOM2060 (Generator-Sicherung) fuehrt den ganzen Ablauf vor:
 *   sub12 @0x015BA `2b 00 ff ff` Generator-Frage, @0x015C4 `21 0c 1f 00` Ja, @0x015CC
 *         `21 03 90 00` nicht eingesetzt -> @0x015D0 `2b 01 ff ff` "I need to insert the missing
 *         fuse before I can operate this."            <== Vorbild: Sperrsatz HINTER der Ja-Antwort
 *   sub18 @0x01678 `2b 03 ff ff` Zustandssatz, @0x0167E `2b 04 ff ff` "Will you use the Fuse?",
 *         @0x01688 `21 0c 1f 00` Ja -> @0x0168C `29 08` Nahansicht VORHER
 *   sub19 @0x0169E `22 03 90 01` eingesetzt, @0x016A2 `29 09` Nahansicht NACHHER,
 *         @0x016C8 `2b 05 ff ff` "You've used the Fuse.", `02 00`, `29 0a`, `3c 01` zurueck.
 * RE1.5 ist dort also massgeblich: dieselbe Reihenfolge, dieselben Saetze (msg 4/5 byte-genau),
 * kein Se_on in sub18/sub19 (Einsetzen stumm), kein Sleep zwischen Set und Text.
 *
 * ⛔ KORREKTUR (Gegenpruefung, Auflage 1): ROOM2060s Einsetzen ist ein ZEIGER-PANEL, keine
 * "gefuehrte Sicherung". obj 3 (Generator-Panel) und obj 4 (Sicherungs-Panel) tragen dasselbe
 * Zeigermodell (TIM @0x382B0, MD1 @0x33E4, bytegleich); Cut 7 und Cut 8/9 sind SENKRECHTE
 * Panelkameras (pos (19572,-17442,-22584) -> tgt (19572,15928,-22583)) ueber dem geparkten Zeiger
 * obj 4 (@0x00F12); sub01 @0x012B4..@0x0130C fuehrt ihn per Sce_key_ck, @0x01314..@0x01332
 * Ck(5,11,1) + Member_cmp(0x0F==7) + Sce_key_ck 0x40 -> sub19.
 * Der Verzicht auf diese Feinbedienung ist PORT-WAHL, keine Original-Adresse — Grund: ROOM1050s
 * Cut 7/8 sind WELTKAMERA-Nahansichten (Kamera @0x140/@0x160, pos (16171,-2733,-8185), Blick fast
 * waagerecht nach Osten, kein Panelrahmen), ROOM1050.RDT hat kein Zeigermodell (nOmodel=2: obj 0
 * Rolltor, obj 1 Raum-Prop) und keine sce-5-Zonen. Die RE1.5-Form fuer Weltkamera-Nahansichten
 * ist die DIREKTE Frage: ROOM1051 sub03 (@0x0DB4 `29 09`, @0x0DB6 `2b 05 ff ff`, derselbe Raum)
 * und ROOM1090 sub06 (@0x02702 msg 7, @0x02708 msg 8 "Will you use the Fire Extinguisher?",
 * @0x02712 Ja -> Wirkung). Ein Zeiger ueber Cut 7 muesste Ebene, Zonen und Zeigerlage erfinden.
 *
 * NACH DEM EINSETZEN keine weitere Nahansicht (Auftragspunkt 4): wie ROOM2060, das den Kasten
 * danach zum reinen Textplatz macht (sub00 @0x010DE / sub19 @0x016B8 -> msg 6 "The fuse is in
 * place."), ohne Cut_chg. In ROOM1050 ist der Kasten zugleich der Schalter (Slot 7, derselbe
 * Platz) — er faehrt dann den ausgelieferten sub02 (Frage, Tor, Ton), unveraendert.
 *
 * GEGENSTAND AUS DEM INVENTAR: RE1.5 kann es nicht — die Opcode-Tabelle 0x800744a8 endet bei 0x5E
 * (letzter Eintrag @0x80074620 -> 0x80042b04, kein Praedikat, kein Entfernen), und RE1.5s
 * Sicherung ist ein reines FLAG, nie ein Gegenstand (ROOM2030 sub06 @0x01FE4 "Will you take the
 * Fuse?" -> @0x01FF2 `22 03 6c 01`; ROOM2060 prueft (3,108) @0x010A2). Der Port fuehrt die
 * Sicherung aber als Inventar-Item 0x40 (Fundstelle Hebetisch ROOM1150, sicherung_1150.c) ->
 * Entfernen ist in RE1.5 UNFERTIG -> RE2-Ziel: Sce_item_lost, Opcode 0x62, Tabelle
 * 0x800a74c8 + 0x62*4 = 0x800a7650 -> 0x800585e4 (selbst disassembliert, re2_disasm.py):
 *     800585fc  lbu   a0,1(v0)          Item-Id = pc[1]
 *     80058600  jal   0x800696cc        Platz suchen (Schleife ueber 0x800d4a3c+4i, -1 = keiner)
 *     80058608  bltz  v0,0x80058634     kein Treffer -> nur nachruecken
 *     80058618  sb    zero,0x4a3c(at)   Id     := 0
 *     80058624  sb    zero,0x4a3d(at)   Anzahl := 0
 *     80058630  sb    zero,0x4a3e(at)   Flags  := 0
 *     80058634  jal   0x80069714        nachruecken
 *     80058640  addiu v0,zero,1         weiter im selben Durchlauf
 *     80058644  addiu v1,v1,2           Satzlaenge 2
 * RE2 benutzt ihn genau so in der Nahansicht: room1110 sub04 (+0x14 `29 07`, +0x38 `62 4a`,
 * +0x40 `29 06 3c 01`), room10B0 sub10 (+0x0C `29 09 62 33`), room60D0 sub06 (+0xDC `62 4d`).
 * Port-Gegenstueck: re15_inv_find_item (= RE1.5 FUN_8004dfec) + re15_inv_remove_slot (Verbrauch
 * @0x8004aef0/@0x8004af0c/@0x8004af28 `sb zero` + @0x8004af2c `jal 0x8004dadc` nachruecken) —
 * dieselbe Folge.
 */
#include "re15_rolltor.h"

#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_sicherung.h"   /* RE15_SICHERUNG_ITEM = 0x40 */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ---- Die Port-Programme. Jede Zeile mit ihrem Vorbild. -----------------------------------------
 * Blocklaengen nach dem Muster sub02 @0x0CB2 `06 00 d0 00`: Blockende = Ifel_ck + 4 + Laenge =
 * hinter dem Endif (0x0CB2 + 4 + 0xD0 = 0x0D86, Endif @0x0D84). `02 00` = Evt_next + Nop wie
 * @0x0CB0/@0x0CB1 (Evt_next schiebt den PC um 1, @0x8003f260, Ertrag 2 @0x8003f26c). */

/* OHNE Sicherung im Inventar: Frage -> Ja -> Nahansicht Cut 7 + Sperrsatz -> zurueck. */
static const uint8_t k_ohne[] = {
    /* +00 */ 0x2b, 0x00, 0x80, 0xff,   /* Message_on 0, Maske 0xFF80    = sub02 @0x0CAC (unveraendert) */
    /* +04 */ 0x02, 0x00,               /* Evt_next + Nop                = sub02 @0x0CB0/@0x0CB1        */
    /* +06 */ 0x06, 0x00, 0x1a, 0x00,   /* Ifel_ck, Blockende +0x24                                     */
    /* +0A */ 0x21, 0x0c, 0x1f, 0x00,   /* Ck(12,31,0) = Ja              = sub02 @0x0CB6                */
    /* +0E */ 0x22, 0x02, 0x07, 0x01,   /* Set(2,7,1) Sperre             = ROOM1051 sub03 @0x0DA4       */
    /* +12 */ 0x29, 0x07,               /* Cut_chg 7 Nahansicht leer/rot (Kamera @0x140; Rolle wie
                                         * ROOM2060 sub18 @0x0168C `29 08`)                              */
    /* +14 */ 0x2b, 0x02, 0xff, 0xff,   /* Message_on 2 "I need a fuse to run the shutter." (msg @0x0ED2),
                                         * Maske wie ROOM2060 sub12 @0x015D0 `2b 01 ff ff`               */
    /* +18 */ 0x02, 0x00,               /* Evt_next + Nop                                               */
    /* +1A */ 0x29, 0x03,               /* Cut_chg 3 zurueck             = ROOM1051 sub03 @0x0DC0       */
    /* +1C */ 0x3c, 0x01,               /* Cut_auto 1                    = ROOM1051 sub03 @0x0DC2       */
    /* +1E */ 0x22, 0x02, 0x07, 0x00,   /* Set(2,7,0)                    = ROOM1051 sub03 @0x0DD2       */
    /* +22 */ 0x08, 0x00,               /* Endif                                                        */
    /* +24 */ 0x01, 0x00,               /* Evt_end                                                      */
};

/* MIT Sicherung im Inventar: wie oben, dann die Einsetz-Frage in derselben Nahansicht. */
static const uint8_t k_mit[] = {
    /* +00 */ 0x2b, 0x00, 0x80, 0xff,   /* Message_on 0 (Schalterfrage)  = sub02 @0x0CAC                */
    /* +04 */ 0x02, 0x00,               /* Evt_next + Nop                                               */
    /* +06 */ 0x06, 0x00, 0x38, 0x00,   /* Ifel_ck aussen, Blockende +0x42                              */
    /* +0A */ 0x21, 0x0c, 0x1f, 0x00,   /* Ck(12,31,0) = Ja                                             */
    /* +0E */ 0x22, 0x02, 0x07, 0x01,   /* Set(2,7,1)                                                   */
    /* +12 */ 0x29, 0x07,               /* Cut_chg 7                                                    */
    /* +14 */ 0x2b, 0x02, 0xff, 0xff,   /* Message_on 2 (Zustandssatz; Rolle wie 2060 sub18 @0x01678)   */
    /* +18 */ 0x02, 0x00,               /* Evt_next + Nop                = ROOM2060 sub18 @0x0167C      */
    /* +1A */ 0x2b, 0x14, 0xff, 0xff,   /* Message_on 20 "Will you use the Fuse?" = 2060 sub18 @0x0167E */
    /* +1E */ 0x02, 0x00,               /* Evt_next + Nop                = ROOM2060 sub18 @0x01682      */
    /* +20 */ 0x06, 0x00, 0x14, 0x00,   /* Ifel_ck innen, Blockende +0x38                               */
    /* +24 */ 0x21, 0x0c, 0x1f, 0x00,   /* Ck(12,31,0) = Ja              = ROOM2060 sub18 @0x01688      */
    /* +28 */ 0x62, 0x40,               /* Sce_item_lost(0x40 "Fuse")    = RE2 0x800585e4 (Port-Opcode)  */
    /* +2A */ 0x22, 0x09, 0x3f, 0x01,   /* Set(9,63,1) eingesetzt (Rolle von 2060 sub19 @0x0169E)       */
    /* +2E */ 0x29, 0x08,               /* Cut_chg 8 beide gruen (Kamera @0x160; Rolle von 2060 sub19
                                         * @0x016A2 `29 09`)                                             */
    /* +30 */ 0x2b, 0x15, 0xff, 0xff,   /* Message_on 21 "You've used the Fuse." = 2060 sub19 @0x016C8  */
    /* +34 */ 0x02, 0x00,               /* Evt_next + Nop                = ROOM2060 sub19 @0x016CC      */
    /* +36 */ 0x08, 0x00,               /* Endif innen                                                  */
    /* +38 */ 0x29, 0x03,               /* Cut_chg 3                     = ROOM1051 sub03 @0x0DC0       */
    /* +3A */ 0x3c, 0x01,               /* Cut_auto 1                    = ROOM1051 sub03 @0x0DC2       */
    /* +3C */ 0x22, 0x02, 0x07, 0x00,   /* Set(2,7,0)                    = ROOM1051 sub03 @0x0DD2       */
    /* +40 */ 0x08, 0x00,               /* Endif aussen                                                 */
    /* +42 */ 0x01, 0x00,               /* Evt_end                                                      */
};

/* Nachricht 20 = ROOM2060.RDT msg 4 @0x1855 (32 B), Byte fuer Byte: Kopf `04 02`, "Will you use
 * the ", Gegenstandsname GRUEN (`05 01 … 05 00`), "?" (0x1b), Ja/Nein (`03`), Ende `02 01 00`.
 * Rahmen identisch mit ROOM1090 msg 8 @0x2934 ("Will you use the Fire Extinguisher?"). */
static const uint8_t k_msg20[] = {
    0x04, 0x02,
    0x33, 0x45, 0x48, 0x48, 0x00, 0x55, 0x4b, 0x51, 0x00, 0x51, 0x4f, 0x41, 0x00, 0x50, 0x44, 0x41, 0x00,
    0x05, 0x01, 0x22, 0x51, 0x4f, 0x41, 0x05, 0x00,
    0x1b, 0x03, 0x02, 0x01, 0x00,
};

/* Nachricht 21 = ROOM2060.RDT msg 5 @0x1875 (29 B), Byte fuer Byte: "You've used the Fuse." */
static const uint8_t k_msg21[] = {
    0x04, 0x02,
    0x35, 0x4b, 0x51, 0x3a, 0x52, 0x41, 0x00, 0x51, 0x4f, 0x41, 0x40, 0x00, 0x50, 0x44, 0x41, 0x00,
    0x05, 0x01, 0x22, 0x51, 0x4f, 0x41, 0x05, 0x00,
    0x57, 0x01, 0x00,
};

static void text_einsetzen(uint8_t id, const uint8_t *m, int len)
{
    re15_msg_install_text(id, m, (size_t)len);
    /* Dauer-Eintrag wie fuer jede RDT-Nachricht (re15_msg_load_room_block). */
    int d = re15_msg_compute_duration(m, (size_t)len, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

const uint8_t *re15_rolltor_programm(int mit, int *len)
{
    if (len) *len = mit ? (int)sizeof k_mit : (int)sizeof k_ohne;
    return mit ? k_mit : k_ohne;
}

const uint8_t *re15_rolltor_meldung(uint8_t id, int *len)
{
    if (id == RE15_ROLLTOR_MSG_FRAGE)   { if (len) *len = (int)sizeof k_msg20; return k_msg20; }
    if (id == RE15_ROLLTOR_MSG_BENUTZT) { if (len) *len = (int)sizeof k_msg21; return k_msg21; }
    if (len) *len = 0;
    return 0;
}

const uint8_t *re15_rolltor_ereignis(uint16_t raum, uint8_t ereignis)
{
    if (raum != RE15_ROLLTOR_RAUM_LEON && raum != RE15_ROLLTOR_RAUM_ELZA) return 0;
    if (ereignis != RE15_ROLLTOR_EREIGNIS) return 0;
    /* Eingesetzt -> der ausgelieferte sub02 (Frage, Tor, Ton). */
    if (re15_game_flag_get(RE15_ROLLTOR_EINGESETZT_BANK, RE15_ROLLTOR_EINGESETZT_BIT)) return 0;
    /* Tor schon offen (Spielstand aus v0.8.19 oder frueher) -> ebenfalls ausgeliefert; sub00
     * @0x0C1E installiert den Schalter dann ohnehin nicht. */
    if (re15_game_flag_get(RE15_ROLLTOR_OFFEN_BANK, RE15_ROLLTOR_OFFEN_BIT)) return 0;

    /* Besitz = Inventar-Suche zum Zeitpunkt des Ausloesens (RE2 Keep_Item_ck 0x5E -> 0x800584f0:
     * `jal 0x800696cc`, Treffer? `nor`/`srl 31`). Im Raum kann sich der Besitz waehrend des Fadens
     * nicht aendern: ROOM1050/1051 haben keine Kiste (Kisten nur ROOM1150/1151, re15_itembox.c),
     * keine Fundstelle der Sicherung, und die Frage/Texte frieren Spieler und Menue (Maske
     * 0xFF80/0xFFFF). RE1.5s Genommen-Bit taugt nicht als Besitz — der Port hat die RE2-Kiste. */
    int mit = re15_inv_find_item(RE15_SICHERUNG_ITEM) >= 0;

    /* Die Texte beim Ausloesen einsetzen: so gelten sie an JEDEM Raumstart-Weg (Tuer, Boot,
     * CONTINUE, Debug-Sprung) ohne weiteren Haken; der Raum-Teardown loescht sie wieder
     * (re15_msg_clear_room_block). */
    text_einsetzen(RE15_ROLLTOR_MSG_FRAGE, k_msg20, (int)sizeof k_msg20);
    text_einsetzen(RE15_ROLLTOR_MSG_BENUTZT, k_msg21, (int)sizeof k_msg21);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[rolltor] ROOM%04X Ereignis %u -> Port-Programm %s (Sicherung %s, (9,63)=0)\n",
            (unsigned)raum, (unsigned)ereignis, mit ? "MIT" : "OHNE", mit ? "im Inventar" : "fehlt");
#endif
    return mit ? k_mit : k_ohne;
}

int re15_rolltor_op_item_lost(scd_thread_t *t)
{
    /* PC-SCHRANKE: nur im Port-Programm ist 0x62 der RE2-Opcode. Ueberall sonst (RDT-Bytecode)
     * bleibt es exakt op_unknown: Satzbreite s_opcode_sizes[0x62] = 1 (= RE1.5-Default
     * LAB_8003f1d8 pc+=1, return 1), weil RE1.5s Tabelle 0x800744a8 bei 0x5E endet. */
    uintptr_t pc = (uintptr_t)t->pc;
    uintptr_t lo = (uintptr_t)k_mit, hi = lo + sizeof k_mit;
    if (pc < lo || pc >= hi) {
        t->pc += 1;
        return 1;
    }
    uint8_t id = t->pc[1];                              /* @0x800585fc lbu a0,1(v0) */
    int s = re15_inv_find_item(id);                     /* @0x80058600 jal 0x800696cc */
    if (s >= 0) {
        re15_inv_remove_slot(s);                        /* @0x80058618/24/30 sb zero + @0x80058634 */
    } else {
        re15_inv_compact();                             /* @0x80058608 bltz -> @0x80058634 */
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[rolltor] Sce_item_lost(0x%02X): Platz %d %s\n", (unsigned)id, s,
            s >= 0 ? "geleert" : "- nicht im Inventar");
#endif
    t->pc += 2;                                         /* @0x80058644 addiu v1,v1,2 */
    return 1;                                           /* @0x80058640 addiu v0,zero,1 */
}
