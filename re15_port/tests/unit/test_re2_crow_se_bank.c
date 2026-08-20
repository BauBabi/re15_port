/* test_re2_crow_se_bank.c — HERLEITUNG + PIN: die RE2-Kraehe (kind 0x21) spielt aus
 * ENEMSE-Bank 7, NICHT aus Bank 21.
 *
 * Nutzer-Report 2026-08-20 (echter Spieltest v0.3.3): "Bei RE2-AI haben die Kraehen den
 * falschen Sound."
 *
 * WIDERLEGT wird damit der bisherige Beleg im Port ("kind 0x21 steht in Paar-Zeile 21 =
 * {0x21,0x00} @0x800A7400"). Der Fehler war eine KATEGORIEN-VERWECHSLUNG: die Paar-Tabelle
 * @0x800A7400 fuehrt ueberhaupt keine Gegner-kinds, sondern die SOUND-ID aus dem Spawn-Record.
 * Dass dort ausgerechnet eine 0x21 steht, ist Zufall — es ist die Sound-Id 0x21, nicht der
 * kind 0x21. Dieser Test fuehrt die Herleitung Schritt fuer Schritt an den ECHTEN Bytes vor,
 * damit die naechste Runde sie nicht wieder zurueckdreht.
 *
 * STUFE 1  Die Tabelle KANN keine kinds fuehren (struktureller Widerspruch).
 * STUFE 2  Byte-Anker in info/re2leon/PSX.EXE: Sce_em_set ist Opcode 0x44; Record+3 ist der
 *          kind, Record+7 wird entity+0x1FA, und NUR +0x1FA wird gegen die Tabelle verglichen.
 * STUFE 3  Zensus ueber die echten RE2-Raumdaten: kind 0x21 traegt Sound-Id 0x0D.
 * STUFE 4  0x0D steht in genau EINER Zeile: Zeile 7 -> Bank 7, flag2000 = 0.
 * STUFE 5  Daten-Gegenprobe an ENEMSE.VBS: Bank 7 passt exakt auf das Kraehen-Modul,
 *          Bank 21 ist eine fremde, doppelt so grosse Bank (NEGATIVPROBE).
 * STUFE 6  Der Port waehlt jetzt wirklich 7 (Hook-Messung).
 * STUFE 7  Die Kanal-/Prioritaets-Maschine des RE2-SE-Triggers mit den ECHTEN Bank-7-Werten.
 */
#include "re2_ems.h"
#include "re15_ai_flavor.h"
#include "re15_vab.h"
#include "re15_actor.h"
#include "re15_enemy_ai.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static int g_bank_sel = -1, g_flag2000_seen = -1, g_last_se = -1;
static void cap_bank(int bank) { g_bank_sel = bank; }
static void cap_se(int se_id, int flag2000) { g_last_se = se_id; g_flag2000_seen = flag2000; }

static unsigned char *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (out_sz) *out_sz = sz;
    return b;
}
static uint32_t rd32(const unsigned char *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* RE2 PSX.EXE: t_addr 0x80010000, Text ab Datei-Offset 0x800 (PS-X-EXE-Header). */
#define RE2_T_ADDR 0x80010000u
static long fo(uint32_t addr) { return (long)(addr - RE2_T_ADDR) + 0x800; }

/* ---------------------------------------------------------------------------------------- */
int main(void)
{
    /* ===== STUFE 1: die Paar-Tabelle kann keine kinds fuehren ============================
     * Gegner-kinds beginnen bei RE2_EMS_KIND_MIN (0x10) — das ist keine Behauptung, sondern
     * das Layout der CDEMD0-TOC @0x8009adf4 (4 Records ab kind 0x10, re2_ems.h). Enthaelt die
     * Tabelle Werte < 0x10, koennen es keine kinds sein. */
    {
        int below = 0, rows = 0, distinct = 0, seen[16] = { 0 };
        for (int b = 0; b <= RE2_ENEMSE_BANK_COUNT; b++) {
            int k0 = -1, k1 = -1;
            if (re2_enemse_pair_row(b, &k0, &k1) != 0) break;
            if (k0 == 0xFF) break;
            rows++;
            if ((k0 != 0 && k0 < RE2_EMS_KIND_MIN) || (k1 != 0 && k1 < RE2_EMS_KIND_MIN)) below++;
            if (k0 != 0 && k0 < RE2_EMS_KIND_MIN && !seen[k0]) { seen[k0] = 1; distinct++; }
            if (k1 != 0 && k1 < RE2_EMS_KIND_MIN && !seen[k1]) { seen[k1] = 1; distinct++; }
        }
        CHECK(rows == 73, "Paar-Tabelle hat 73 Zeilen vor dem 0xFF-Terminator, gezaehlt %d", rows);
        CHECK(below == 28,
              "28 Zeilen fuehren einen Wert < 0x10 (im kind-Raum unmoeglich) — gezaehlt %d", below);
        CHECK(distinct == 15,
              "es sind die 15 verschiedenen Werte 0x01..0x0F — gezaehlt %d", distinct);
    }

    /* ===== STUFE 2: Byte-Anker in der echten RE2-EXE =================================== */
    long exe_sz = 0;
    unsigned char *exe = slurp(RE15_RE2_EXE_PATH, &exe_sz);
    CHECK(exe != NULL, "info/re2leon/PSX.EXE nicht lesbar (%s) — ohne sie ist die Herleitung "
                       "nicht nachpruefbar", RE15_RE2_EXE_PATH);
    if (exe) {
        /* (a) Der SCD-Opcode-Dispatch: Tabelle @0x800A74C8, Eintrag [0x44] @0x800A75D8 zeigt
         *     auf 0x8005714C = den Sce_em_set-Handler. (0x8005714C ist die EINZIGE Wort-
         *     Referenz dieser Adresse in der ganzen EXE — daher ist der Index eindeutig.) */
        CHECK(rd32(exe + fo(0x800A75D8u)) == 0x8005714Cu,
              "Dispatch[0x44] @0x800A75D8 = 0x%08X, erwartet 0x8005714C (Sce_em_set)",
              rd32(exe + fo(0x800A75D8u)));
        {   int refs = 0;
            for (long o = 0x800; o + 4 <= exe_sz; o += 4)
                if (rd32(exe + o) == 0x8005714Cu) refs++;
            CHECK(refs == 1, "0x8005714C wird genau einmal als Wort referenziert (Dispatch), "
                             "gezaehlt %d", refs);
        }
        /* (b) Im Handler: Record+3 -> jal 0x8001b710 (kind->Overlay-Klemme) = DER KIND. */
        CHECK(rd32(exe + fo(0x800571ECu)) == 0x90440003u,
              "0x800571EC = `lbu a0,3(v0)` (0x90440003), gelesen 0x%08X", rd32(exe + fo(0x800571ECu)));
        {   uint32_t w = rd32(exe + fo(0x800571F0u));   /* jal 0x8001b710 */
            CHECK((w >> 26) == 3u && ((w & 0x03FFFFFFu) << 2) == (0x8001B710u & 0x0FFFFFFFu),
                  "0x800571F0 = `jal 0x8001b710`, gelesen 0x%08X", w);
        }
        /* (c) Record+7 -> entity+0x1FA (506). Das ist die SOUND-ID, nicht der kind. */
        CHECK(rd32(exe + fo(0x80057274u)) == 0x90420007u,
              "0x80057274 = `lbu v0,7(v0)` (0x90420007), gelesen 0x%08X", rd32(exe + fo(0x80057274u)));
        CHECK(rd32(exe + fo(0x80057280u)) == 0xA20201FAu,
              "0x80057280 = `sb v0,506(s0)` (0xA20201FA), gelesen 0x%08X", rd32(exe + fo(0x80057280u)));
        /* (d) FUN_80052b38 vergleicht GENAU dieses Byte gegen die Tabelle. */
        CHECK(rd32(exe + fo(0x80052C48u)) == 0x808301FAu,
              "0x80052C48 = `lb v1,506(a0)` (0x808301FA), gelesen 0x%08X", rd32(exe + fo(0x80052C48u)));
        /* (e) Die vendorte Paar-Tabelle == die EXE-Bytes @0x800A7400 (Datei 0x97C00). */
        {   int diff = 0;
            for (int b = 0; b <= RE2_ENEMSE_BANK_COUNT; b++) {
                int k0 = -1, k1 = -1;
                if (re2_enemse_pair_row(b, &k0, &k1) != 0) break;
                if (k0 != exe[fo(0x800A7400u) + b*2] || k1 != exe[fo(0x800A7400u) + b*2 + 1]) diff++;
            }
            CHECK(diff == 0, "vendorte Paar-Tabelle weicht in %d Zeilen von der EXE ab", diff);
        }
        /* (f) Der Anker der ALTEN Behauptung, mit korrekter Deutung: Zeile 21 = {0x21,0x00}
         *     — die Zahl stimmt, ihre BEDEUTUNG war falsch (Sound-Id 0x21, nicht kind 0x21). */
        CHECK(exe[fo(0x800A7400u) + 21*2] == 0x21 && exe[fo(0x800A7400u) + 21*2 + 1] == 0x00,
              "Zeile 21 ist weiterhin {0x21,0x00} — nur bedeutet die 0x21 eine SOUND-ID");
        CHECK(exe[fo(0x800A7400u) + 7*2] == 0x0D && exe[fo(0x800A7400u) + 7*2 + 1] == 0x00,
              "Zeile 7 @Datei 0x97C0E = {0x0D,0x00}");
    }

    /* ===== STUFE 3: Zensus der echten RE2-Raumdaten ====================================
     * Sce_em_set = Opcode 0x44, 22 Byte, +3 kind, +7 Sound-Id, +10/12/14 x/y/z (die
     * Koordinaten-Offsets aus @0x800572C8/E0/F8 dienen hier als Plausibilitaets-Filter).
     * Die beiden RE2-Raeume mit Kraehen sind ROOM1090 und ROOM2110 (aus dem Zensus ueber
     * alle 495 RDTs; hier werden genau diese zwei erneut ausgezaehlt). */
    {
        static const char *rooms[2] = { "ROOM1090.RDT", "ROOM2110.RDT" };
        static const int   expect[2] = { 28, 9 };
        for (int r = 0; r < 2; r++) {
            char path[512];
            snprintf(path, sizeof path, "%s/%s", RE15_RE2_RDT_DIR, rooms[r]);
            long sz = 0;
            unsigned char *d = slurp(path, &sz);
            CHECK(d != NULL, "%s nicht lesbar", path);
            if (!d) continue;
            uint32_t offs[23];
            for (int i = 0; i < 23; i++) offs[i] = rd32(d + 8 + i*4);
            int crow = 0, crow_other_sound = 0, sound21 = 0;
            for (int s = 0; s < 2; s++) {                       /* 17 = SCD-INIT, 18 = SCD-MAIN */
                uint32_t o = offs[s ? 18 : 17];
                if (o == 0 || (long)o >= sz) continue;
                uint32_t e = (uint32_t)sz;
                for (int i = 0; i < 23; i++) if (offs[i] > o && (long)offs[i] <= sz && offs[i] < e) e = offs[i];
                for (uint32_t q = o; q + 22 <= e; q += 2) {
                    if (d[q] != 0x44) continue;
                    int kind = d[q+3], snd = d[q+7];
                    int16_t x = (int16_t)(d[q+10] | (d[q+11] << 8));
                    int16_t z = (int16_t)(d[q+14] | (d[q+15] << 8));
                    if (kind < RE2_EMS_KIND_MIN || kind > 0x5A) continue;
                    if (x <= -32000 || x >= 32000 || z <= -32000 || z >= 32000) continue;
                    if (snd == 0x21) sound21++;         /* NEGATIVPROBE zur alten Bank 21 */
                    if (kind != 0x21) continue;
                    crow++;
                    if (snd != 0x0D) crow_other_sound++;
                }
            }
            CHECK(sound21 == 0,
                  "%s: kein einziger Spawn-Record traegt Sound-Id 0x21 — Bank 21 kann in einem "
                  "Kraehen-Raum also gar nicht geladen werden (gezaehlt %d)", rooms[r], sound21);
            CHECK(crow == expect[r], "%s: %d Kraehen-Spawns (Opcode 0x44, +3 == 0x21), erwartet %d",
                  rooms[r], crow, expect[r]);
            CHECK(crow_other_sound == 0,
                  "%s: %d Kraehen-Records mit einer anderen Sound-Id als 0x0D — die Zuordnung "
                  "kind 0x21 -> Sound-Id 0x0D muss ausnahmslos gelten", rooms[r], crow_other_sound);
            free(d);
        }
    }

    /* ===== STUFE 4: Sound-Id 0x0D -> Zeile 7, eindeutig, erste Haelfte ================= */
    {
        int rows_with_0d = 0, row = -1, half = -1;
        for (int b = 0; b <= RE2_ENEMSE_BANK_COUNT; b++) {
            int k0 = -1, k1 = -1;
            if (re2_enemse_pair_row(b, &k0, &k1) != 0) break;
            if (k0 == 0xFF) break;
            if (k0 == 0x0D) { rows_with_0d++; row = b; half = 0; }
            if (k1 == 0x0D) { rows_with_0d++; row = b; half = 1; }
        }
        CHECK(rows_with_0d == 1, "Sound-Id 0x0D steht in genau EINER Zeile, gezaehlt %d", rows_with_0d);
        CHECK(row == 7 && half == 0, "0x0D steht in Zeile 7 in der ERSTEN Haelfte (row=%d half=%d)",
              row, half);
        int fk = -2;
        int bank = re2_enemse_select_bank(0x0D, 0, &fk);
        CHECK(bank == 7, "FUN_80052b38(0x0D, 0) = %d, erwartet 7", bank);
        CHECK(fk == 0x00, "zweite Zeilenhaelfte ist leer -> KEIN word0|0x2000 (@0x80052C4C-88) "
                          "-> flag2000 = 0, fk=0x%02X", fk);
    }

    /* ===== STUFE 5: Daten-Gegenprobe an ENEMSE.VBS ===================================== */
    {
        long vbs_sz = 0;
        unsigned char *vbs = slurp(RE15_ASSET_RE2_DIR "/ENEMSE.VBS", &vbs_sz);
        CHECK(vbs != NULL, "shared_assets/RE2/ENEMSE.VBS nicht lesbar");
        if (vbs) {
            struct { int bank; uint32_t edt_off, edt_size, vbd_size; int live, vags; } expect[2] = {
                { 7,  0x86800u,  0xCA8u, 0x0BCF0u, 7,  8  },   /* die Kraehen-Bank        */
                { 21, 0x189000u, 0xCA8u, 0x1FE00u, 15, 16 },   /* was der Port vorher nahm */
            };
            for (int i = 0; i < 2; i++) {
                re2_enemse_rec_t rec;
                CHECK(re2_enemse_toc_entry(expect[i].bank, &rec) == 0, "TOC-Eintrag %d fehlt",
                      expect[i].bank);
                CHECK(rec.edt_off == expect[i].edt_off && rec.edt_size == expect[i].edt_size
                      && rec.vbd_size == expect[i].vbd_size,
                      "Bank %d: EDT @0x%X (0x%X), VBD 0x%X — erwartet @0x%X (0x%X), 0x%X",
                      expect[i].bank, rec.edt_off, rec.edt_size, rec.vbd_size,
                      expect[i].edt_off, expect[i].edt_size, expect[i].vbd_size);
                if (rec.edt_off + rec.edt_size > (uint32_t)vbs_sz) continue;
                const unsigned char *edt = vbs + rec.edt_off;
                uint32_t vh_off = rd32(edt + rec.edt_size - 8);
                int n = (int)(vh_off / 4), live = 0;
                for (int id = 0; id < n; id++) if (rd32(edt + id*4) != 0xFFFFFFFFu) live++;
                CHECK(n == 32, "Bank %d: SE-Map hat 32 Eintraege, gelesen %d", expect[i].bank, n);
                CHECK(live == expect[i].live, "Bank %d: %d lebende SE-Ids, erwartet %d",
                      expect[i].bank, live, expect[i].live);
                re15_vab_t vab;
                if (re15_vab_parse(edt + vh_off, (size_t)rec.edt_size - vh_off, &vab) == 0)
                    CHECK(vab.vag_count == expect[i].vags, "Bank %d: %d VAGs, erwartet %d",
                          expect[i].bank, vab.vag_count, expect[i].vags);
            }
            /* Bank 7, Eintrag fuer Eintrag — das sind die Bytes, die die Kraehe hoert.
             * (prog, tone, Kanal, Prio-Nibble; Reihenfolge = die SE-Ids des Moduls) */
            {
                re2_enemse_rec_t rec;
                re2_enemse_toc_entry(7, &rec);
                const unsigned char *edt = vbs + rec.edt_off;
                struct { int id; uint32_t word; int prog, tone, chan, prio; } m[7] = {
                    { 0, 0x04110000u, 0, 1, 4, 1  },
                    { 1, 0x05210000u, 0, 2, 5, 1  },   /* Fluegelschlag  @0x80100CA0 u.a. */
                    { 2, 0x073A0000u, 0, 3, 7, 10 },   /* Picken         @0x80102654      */
                    { 3, 0x05420000u, 0, 4, 5, 2  },   /* Kreischen      @0x80101BD0 u.a. */
                    { 4, 0x07510000u, 0, 5, 7, 1  },   /* Strike-Impact  @0x80102058      */
                    { 5, 0x06610000u, 0, 6, 6, 1  },   /* Thud           @0x80101C74 u.a. */
                    { 6, 0x047A0000u, 0, 7, 4, 10 },   /* Kraechzen      @0x801033D0      */
                };
                for (int i = 0; i < 7; i++) {
                    uint32_t w = rd32(edt + m[i].id * 4);
                    CHECK(w == m[i].word, "Bank 7 id %d = 0x%08X, erwartet 0x%08X",
                          m[i].id, w, m[i].word);
                    re2_enemse_se_t se;
                    re2_enemse_decode_entry(w, &se);
                    CHECK(!se.silent && se.prog == m[i].prog && se.tone == m[i].tone
                          && se.chan == m[i].chan && se.prio == m[i].prio && se.extra == 0,
                          "Bank 7 id %d dekodiert prog=%d tone=%d chan=%d prio=%d extra=%d",
                          m[i].id, se.prog, se.tone, se.chan, se.prio, se.extra);
                }
                /* NEGATIVPROBE: ab id 7 ist die Kraehen-Bank leer — das Modul ruft nur 1..6
                 * (jal-0x8005bd6c-Scan ueber EMOVL21_S0.BIN: 25 Aufrufe, ids {1,2,3,4,5,6}),
                 * die Bank deckt also exakt seinen Bedarf. Bank 21 hat dagegen bis id 14
                 * Eintraege = eine fremde, wesentlich groessere Gegner-Bank. */
                for (int id = 7; id < 32; id++)
                    CHECK(rd32(edt + id*4) == 0xFFFFFFFFu,
                          "Bank 7 id %d muesste stumm sein (0xFFFFFFFF), ist 0x%08X",
                          id, rd32(edt + id*4));
                re2_enemse_rec_t r21;
                re2_enemse_toc_entry(21, &r21);
                CHECK(rd32(vbs + r21.edt_off + 14*4) != 0xFFFFFFFFu,
                      "Bank 21 hat bis id 14 Eintraege (Beleg, dass sie NICHT die Kraehen-Bank ist)");
            }
            free(vbs);
        }
    }

    /* ===== STUFE 6: der Port waehlt wirklich 7 ========================================= */
    {
        g_bank_sel = -1; g_flag2000_seen = -1;
        re15_re2crow_audio_hook(cap_se, cap_bank);
        CHECK(g_bank_sel == 7, "re15_re2crow_audio_hook waehlt Bank %d, erwartet 7", g_bank_sel);
        /* Und der SE-Trigger reicht flag2000 = 0 durch (Zeile 7 hat keine zweite Haelfte). */
        re15_re2crow_se_play(3);
        CHECK(g_last_se == 3 && g_flag2000_seen == 0,
              "SE-Aufruf: se=%d flag2000=%d, erwartet se=3 flag2000=0", g_last_se, g_flag2000_seen);
        re15_re2crow_audio_hook(NULL, NULL);
    }

    /* ===== STUFE 7: Kanal-/Prioritaets-Maschine mit den ECHTEN Bank-7-Werten ===========
     * FUN_8005bd6c legt den SPU-Kanal FEST (`andi s1,v0,0x1f` @0x8005bdfc auf EDT-Byte3) und
     * laesst pro Kanal nur EINEN Laut zu: `jal 0x8005c92c` @0x8005be08 + `bne v0,zero`
     * @0x8005be14 verwerfen den SE KOMPLETT; @0x8005be98 latcht `prio & 7` nach
     * DAT_800d4ca0[chan*2]; die Frame-Pumpe gibt ihn wieder frei (`sb zero,...` @0x8005c870).
     * Das Gate selbst ist zeilengleich mit FUN_80045a18 -> re15_se_prio_gate (vab_common.c).
     * Gepinnt werden die Faelle, die im Kraehen-Schwarm tatsaechlich auftreten. */
    {
        unsigned char prio[8];
        memset(prio, 0, sizeof prio);
        /* SE 1 Fluegelschlag: Kanal 5, Nibble 1 -> frei, wird gespielt und latcht 1. */
        CHECK(re15_se_prio_gate(prio, 5, 1) == 0, "SE 1 auf freiem Kanal 5 muss durchkommen");
        prio[5] = 1;
        /* Ein ZWEITER Fluegelschlag (zweite Kraehe) im selben Fenster: Gleichstand, Nibble 1
         * hat Bit 3 nicht -> er kommt durch und SCHNEIDET den ersten ab (ein Kanal, ein Laut).
         * Vorher stapelte der Port beliebig viele Kopien nebeneinander. */
        CHECK(re15_se_prio_gate(prio, 5, 1) == 0, "zweiter SE 1: Gleichstand ohne Bit 3 -> erlaubt");
        /* SE 3 Kreischen: derselbe Kanal 5, Nibble 2 -> hoeher, kommt durch. */
        CHECK(re15_se_prio_gate(prio, 5, 2) == 0, "SE 3 (prio 2) ueber laufendem SE 1 (prio 1)");
        prio[5] = 2;
        /* ...und ab jetzt faellt der Fluegelschlag AUS, solange das Kreischen laeuft. */
        CHECK(re15_se_prio_gate(prio, 5, 1) == 1,
              "SE 1 (prio 1) muss verworfen werden, solange Kanal 5 auf prio 2 steht");
        /* SE 2 Picken: Kanal 7, Nibble 10 = prio 2 MIT Bit 3 -> beim ersten Mal frei (0<2), */
        CHECK(re15_se_prio_gate(prio, 7, 10) == 0, "SE 2 auf freiem Kanal 7 muss durchkommen");
        prio[7] = 2;
        /* beim zweiten Mal Gleichstand + Bit 3 -> VERWORFEN (@0x8005c960-64). */
        CHECK(re15_se_prio_gate(prio, 7, 10) == 1,
              "SE 2 erneut: Gleichstand + Nibble-Bit 3 -> verwerfen");
        /* SE 6 Kraechzen teilt sich Kanal 4 mit SE 0 (Nibble 10 bzw. 1) — dieselbe Regel. */
        memset(prio, 0, sizeof prio);
        CHECK(re15_se_prio_gate(prio, 4, 10) == 0, "SE 6 auf freiem Kanal 4");
        prio[4] = 2;
        CHECK(re15_se_prio_gate(prio, 4, 1) == 1, "SE 0 (prio 1) faellt unter laufendem SE 6 aus");
        /* NEGATIV: ein Kanal ausserhalb 0..7 wird nicht gegatet (Zensus: es gibt nur 2..7). */
        CHECK(re15_se_prio_gate(prio, 9, 0) == 0, "Kanal ausserhalb 0..7 -> kein Gate");
    }

    /* ===== STUFE 8: SCHWARM-MESSUNG (vorher/nachher, ohne Klang-Urteil) ================
     * Vier Kraehen laufen durch das ECHTE Brain. Gemessen wird, was pro Frame an SE-Starts
     * anfaellt und wie viele davon im Original ueberhaupt klingen koennen: der Kanal steht im
     * EDT-Byte3 fest, und die Frame-Pumpe keyt pro Kanal genau EINEN Satz — mehrere SEs auf
     * demselben Kanal im selben Frame ergeben also EINEN Laut, nicht mehrere. Der alte Port
     * legte jeden einzelnen auf einem eigenen Mixer-Slot ab. */
    {
        static const int ch_of_id[7] = { 4, 5, 7, 5, 7, 6, 4 };   /* Bank-7-EDT, Byte3 & 0x1f */
        static const int pr_of_id[7] = { 1, 1, 10, 2, 1, 1, 10 }; /* Bank-7-EDT, Byte2 & 0x0f */
        /* Wie lange belegt ein SE seinen Kanal? Aus DEN GELADENEN BYTES gerechnet, mit genau
         * derselben Kette wie der Port-Mixer: pcm_len = (VAG-Bytes/16)*28 (ADPCM 16->28),
         * Schritt = note2pitch2(tone) << 4 in Q16 auf 44100 Hz, Frame = 1470 Ausgabesamples. */
        int dur_frames[7]; for (int i = 0; i < 7; i++) dur_frames[i] = 1;
        {
            long vsz = 0;
            unsigned char *v = slurp(RE15_ASSET_RE2_DIR "/ENEMSE.VBS", &vsz);
            re2_enemse_rec_t rec;
            if (v && re2_enemse_toc_entry(7, &rec) == 0 && rec.edt_off + rec.edt_size <= (uint32_t)vsz) {
                const unsigned char *edt = v + rec.edt_off;
                uint32_t vh_off = rd32(edt + rec.edt_size - 8);
                re15_vab_t vab;
                if (re15_vab_parse(edt + vh_off, (size_t)rec.edt_size - vh_off, &vab) == 0) {
                    for (int id = 0; id < 7; id++) {
                        re2_enemse_se_t se;
                        re2_enemse_decode_entry(rd32(edt + id*4), &se);
                        int ti = se.prog * RE15_VAB_TONES_PER_PROGRAM + se.tone;
                        const re15_vab_tone_t *t = &vab.tones[ti];
                        int vag = (int)t->vag_index - 1;
                        if (vag < 0 || vag >= vab.vag_count) continue;
                        long pcm = (long)(vab.samples[vag].size / 16) * 28;
                        uint32_t step = (uint32_t)re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                                                      t->center_note, t->pitch_shift) << 4;
                        if (!step) step = 0x8000;
                        long outs = (long)((unsigned long long)pcm * 65536ull / step);
                        dur_frames[id] = (int)((outs + 1469) / 1470);
                        if (dur_frames[id] < 1) dur_frames[id] = 1;
                    }
                }
            }
            free(v);
            printf("MESSUNG Bank-7-Samplelaengen in Frames (id0..id6): %d %d %d %d %d %d %d\n",
                   dur_frames[0], dur_frames[1], dur_frames[2], dur_frames[3],
                   dur_frames[4], dur_frames[5], dur_frames[6]);
        }
        int busy_until[8]; memset(busy_until, 0, sizeof busy_until);
        unsigned char chprio[8]; memset(chprio, 0, sizeof chprio);
        long gated = 0, cut = 0;

        re15_actor_init();
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        memset(pl, 0, sizeof *pl);
        pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = 0;
        for (int s = 1; s <= 4; s++) {
            re15_actor_t *c = &g_actors[s];
            memset(c, 0, sizeof *c);
            c->active = 1; c->type = 0x21; c->state = 0;
            c->x = (int32_t)(1200 * s); c->z = (int32_t)(-900 * s);
        }
        g_bank_sel = -1;
        re15_re2crow_audio_hook(cap_se, cap_bank);
        CHECK(g_bank_sel == 7, "Schwarm-Messung laeuft auf Bank %d", g_bank_sel);

        long starts_old = 0, starts_new = 0, peak_old = 0, peak_new = 0, collisions = 0;
        for (int f = 0; f < 900; f++) {
            int used[8]; memset(used, 0, sizeof used);
            int this_frame_old = 0, this_frame_new = 0;
            for (int s = 1; s <= 4; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!e->active) continue;
                int32_t dx = pl->x - e->x, dz = pl->z - e->z;
                int64_t d2 = (int64_t)dx * dx + (int64_t)dz * dz;
                uint32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
                e->ai_dist = r;
                g_last_se = -1;
                re15_re2crow_tick(s);
                if (g_last_se >= 0 && g_last_se <= 6) {
                    this_frame_old++;                       /* alter Port: eigener Mixer-Slot */
                    int ch = ch_of_id[g_last_se];
                    /* Frame-Pumpe: ein ausgeklungener Kanal gibt seine Prioritaet frei
                     * (`sb zero,DAT_800d4ca0[chan*2]` @0x8005c870). */
                    if (f >= busy_until[ch]) chprio[ch] = 0;
                    if (re15_se_prio_gate(chprio, ch, pr_of_id[g_last_se])) { gated++; continue; }
                    if (used[ch]) collisions++;             /* zweiter Start im SELBEN Frame */
                    else { used[ch] = 1; this_frame_new++; }
                    if (f < busy_until[ch]) cut++;          /* schneidet den laufenden Laut ab */
                    chprio[ch]    = (unsigned char)(pr_of_id[g_last_se] & 7);
                    busy_until[ch] = f + dur_frames[g_last_se];
                }
            }
            starts_old += this_frame_old; starts_new += this_frame_new;
            if (this_frame_old > peak_old) peak_old = this_frame_old;
            if (this_frame_new > peak_new) peak_new = this_frame_new;
            pl->x = (f & 128) ? 1500 : -1500;               /* Spieler bewegt sich, damit das
                                                             * Brain durch seine Substates laeuft */
            if (pl->hp < 40) pl->hp = 100;
        }
        printf("MESSUNG Schwarm (4 Kraehen, 900 Frames, Bank 7): SE-Trigger=%ld | ALT: %ld "
               "Samples gestartet, Spitze %ld/Frame | NEU: %ld gestartet (%ld vom Prio-Gate "
               "verworfen, %ld Doppel im selben Frame), davon %ld schneiden den laufenden "
               "Kanal ab, Spitze %ld/Frame\n",
               starts_old, starts_old, peak_old, starts_new, gated, collisions, cut, peak_new);
        CHECK(starts_old > 0, "der Schwarm muss ueberhaupt SEs ausloesen (gemessen %ld)", starts_old);
        CHECK(peak_new <= 4, "pro Frame koennen hoechstens 4 verschiedene Kanaele starten "
                             "(Bank 7 nutzt 4..7), gemessen %ld", peak_new);
        CHECK(starts_new + collisions + gated == starts_old, "Buchhaltung: %ld + %ld + %ld != %ld",
              starts_new, collisions, gated, starts_old);
        CHECK(gated > 0, "im Schwarm MUSS das Prio-Gate greifen (Kanal 5 traegt SE 1 und SE 3, "
                         "Kanal 7 SE 2 und SE 4, Kanal 4 SE 0 und SE 6) — gemessen %ld", gated);
        /* ===== RE1.5-REGRESSIONSWACHE ===================================================
         * Der RE1.5-Default darf von all dem NICHTS mitbekommen: die RE1.5-Kraehe (Brain
         * FUN_80112020) spielt ueber den RAUM-SE-Pfad (re15_audio_room_se / FUN_800453d0),
         * nicht ueber den ENEMSE-Trigger. Unter RE15_AI_FLAVOR_RE15 darf der ENEMSE-Hook
         * deshalb ueberhaupt nicht feuern. */
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        g_last_se = -1;
        for (int f = 0; f < 300; f++) {
            re15_enemy_ai_run_all(1);
            pl->x = (f & 64) ? 900 : -900;
            if (pl->hp < 40) pl->hp = 100;
        }
        CHECK(g_last_se == -1,
              "RE1.5-Flavor darf den RE2-ENEMSE-Hook NIE rufen — gesehen se=%d", g_last_se);
        re15_re2crow_audio_hook(NULL, NULL);
    }

    if (exe) free(exe);
    printf(fails ? "test_re2_crow_se_bank: %d FAIL\n" : "test_re2_crow_se_bank: OK\n", fails);
    return fails ? 1 : 0;
}
