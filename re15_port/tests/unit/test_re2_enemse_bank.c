/**
 * @file test_re2_enemse_bank.c
 * @brief PIN — byte-true ENEMSE-Bank-Wahl FUN_80052b38 + Raum-kind-Paar @0x8005728c-b8.
 *
 * Hintergrund: Nutzer-Report 2026-08-17 "die Zombies in der RE2-AI haben den falschen Sound".
 * Der Mechanismus ist jetzt disassembliert (re2_ems.c, Kopfkommentar mit allen @0x-Zitaten):
 *   - Enemy-Spawn @0x8005728c-0x800572b8: kind == A -> nichts; A == 0 -> A = kind; sonst B = kind
 *   - FUN_80052b38 @0x80052b40-0x80052c2c (Decompilat RE2_Quellcode_V2/FUN_80052b38.c):
 *       Zeilen-Scan ueber @0x800a7400 (2 B/Zeile, Terminator kindA == 0xFF); ZEILENINDEX = Bank;
 *       Treffer (a) Zeile.kindA == A && (B == 0 || B == Zeile.kindB) -> 2. Haelfte = Zeile.kindB
 *       Treffer (b) Zeile.kindB == A && (B == 0 || B == Zeile.kindA) -> 2. Haelfte = A
 *       kein Treffer -> 0xFF (Lader FUN_8005a09c bricht dann ab)
 * Der Test pinnt die REINE Mechanik gegen die vendorte Tabelle. Er behauptet NICHT, dass die
 * Tabellen-ids Gegner-kinds sind — das ist ausdruecklich widerlegt (record+3 vs record+7,
 * 15 ids < 0x10); genau deshalb ist die Mechanik im Spiel noch nicht verdrahtet.
 */
#include "re2_ems.h"

#include <stdio.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

int main(void)
{
    /* ---- (1) Byte-Anker der vendorten Paar-Tabelle @0x800a7400 ---- */
    struct { int bank, k0, k1; } anchors[] = {
        { 0,  0x03, 0x00 },   /* Zeile 0 ist NICHT der Zombie — der Anker gegen die alte Annahme */
        { 11, 0x10, 0x00 },
        { 21, 0x21, 0x00 },
        { 31, 0x00, 0x20 },
        { 42, 0x05, 0x11 },
        { 66, 0x10, 0x20 },
        { 72, 0x34, 0x00 },
        { 73, 0xFF, 0x00 },   /* Schleifen-Terminator (`while (cVar2 != -1)` @0x80052bec) */
    };
    for (unsigned i = 0; i < sizeof anchors / sizeof anchors[0]; i++) {
        int k0 = -1, k1 = -1;
        CHECK(re2_enemse_pair_row(anchors[i].bank, &k0, &k1) == 0, "Zeile %d fehlt", anchors[i].bank);
        CHECK(k0 == anchors[i].k0 && k1 == anchors[i].k1,
              "Zeile %d: {0x%02X,0x%02X} erwartet, {0x%02X,0x%02X} gelesen",
              anchors[i].bank, anchors[i].k0, anchors[i].k1, k0, k1);
    }

    /* ---- (2) Zeilen-Scan: erste Haelfte, B == 0 (Ein-id-Raum) ---- */
    struct { int a, bank, flag; } single[] = {
        { 0x03, 0,  0x00 },
        { 0x10, 11, 0x00 },
        { 0x12, 12, 0x00 },
        { 0x13, 13, 0x00 },
        { 0x16, 17, 0x00 },
        { 0x18, 19, 0x00 },
        { 0x21, 21, 0x00 },
    };
    for (unsigned i = 0; i < sizeof single / sizeof single[0]; i++) {
        int fk = -2;
        int b = re2_enemse_select_bank(single[i].a, 0, &fk);
        CHECK(b == single[i].bank, "select(0x%02X,0) = %d, erwartet %d", single[i].a, b, single[i].bank);
        CHECK(fk == single[i].flag, "select(0x%02X,0) 2.Haelfte = 0x%02X, erwartet 0x%02X",
              single[i].a, fk, single[i].flag);
    }

    /* ---- (3) Zweite-Haelfte-Treffer (Zweig (b)): id steht NUR als zweiter kind ---- */
    {   int fk = -2;
        int b = re2_enemse_select_bank(0x20, 0, &fk);
        CHECK(b == 31, "select(0x20,0) = %d, erwartet 31 (Zeile {0x00,0x20})", b);
        CHECK(fk == 0x20, "select(0x20,0): der Treffer-kind IST die 2. Haelfte -> flag2000");
    }
    {   int fk = -2;
        int b = re2_enemse_select_bank(0x11, 0, &fk);
        CHECK(b == 42, "select(0x11,0) = %d, erwartet 42 (Zeile {0x05,0x11})", b);
        CHECK(fk == 0x11, "select(0x11,0): 2. Haelfte = 0x11");
    }

    /* ---- (4) Echtes PAAR: B != 0 muss die Zeile mit-selektieren ---- */
    {   int fk = -2;
        int b = re2_enemse_select_bank(0x10, 0x20, &fk);
        CHECK(b == 66, "select(0x10,0x20) = %d, erwartet 66 ({0x10,0x20})", b);
        CHECK(fk == 0x20, "select(0x10,0x20): 2. Haelfte = 0x20");
    }
    {   int fk = -2;                       /* dieselbe Zeile, Rollen getauscht -> Zweig (b) */
        int b = re2_enemse_select_bank(0x20, 0x10, &fk);
        CHECK(b == 66, "select(0x20,0x10) = %d, erwartet 66", b);
        CHECK(fk == 0x20, "select(0x20,0x10): der ABGEFRAGTE kind ist die 2. Haelfte");
    }

    /* ---- (5) NEGATIV: kein Treffer -> 0xFF (Lader laedt dann GAR KEINE Bank) ---- */
    {   int fk = -2;
        CHECK(re2_enemse_select_bank(0, 0, &fk) == 0xFF, "A == 0 -> 0xFF (@0x80052b44)");
        CHECK(re2_enemse_select_bank(0x99, 0, &fk) == 0xFF, "unbekannte id -> 0xFF");
        CHECK(re2_enemse_select_bank(0x10, 0x11, &fk) == 0xFF,
              "Paar {0x10,0x11} steht in KEINER Zeile -> 0xFF");
        CHECK(fk == -1, "0xFF -> keine 2. Haelfte");
    }

    /* ---- (6) Raum-kind-Paar-Aufbau, byte-true @0x8005728c-0x800572b8 ---- */
    {   int a = -1, b = -1;
        re2_enemse_room_reset();
        re2_enemse_room_kinds(&a, &b);
        CHECK(a == 0 && b == 0, "Reset (FUN_80052f3c @0x80053028/30) nullt beide");
        re2_enemse_room_add_kind(0x16);          /* A leer -> A */
        re2_enemse_room_add_kind(0x16);          /* == A -> nichts (beq @0x80057298) */
        re2_enemse_room_kinds(&a, &b);
        CHECK(a == 0x16 && b == 0x00, "erster kind -> A, Wiederholung ohne Wirkung (a=0x%02X b=0x%02X)", a, b);
        re2_enemse_room_add_kind(0x10);          /* != A, A gesetzt -> B */
        re2_enemse_room_add_kind(0x11);          /* ueberschreibt B (kein Fehler, byte-true) */
        re2_enemse_room_kinds(&a, &b);
        CHECK(a == 0x16 && b == 0x11, "dritter kind UEBERSCHREIBT B (a=0x%02X b=0x%02X)", a, b);
        /* Und genau dieses (in RE1.5-Raeumen normale) Tripel hat in RE2 keine Zeile: */
        CHECK(re2_enemse_select_bank(a, b, NULL) == 0xFF,
              "ROOM1140-Tripel {0x16,0x10,0x11} -> kein Tabellen-Treffer (Beleg fuer das OPEN)");
        re2_enemse_room_reset();
    }

    printf(fails ? "test_re2_enemse_bank: %d FAIL\n" : "test_re2_enemse_bank: OK\n", fails);
    return fails ? 1 : 0;
}
