/* unit_door_seq.c — Riegel fuer die RE2-Tuersequenz (engine/src/door_seq_common.c).
 *
 * 1. Skriptmaschine Bild fuer Bild gegen den Katalog-Simulator (tools/tor/tuerkatalog.py,
 *    vom Skeptiker mit eigenem Simulator nachgerechnet): Lage und Drehung der Objekte 0..2
 *    in JEDEM Bild, Bilder mit Se_on, Bildzahl, Schliesston-Merker - fuer RE2 DOOR2E
 *    (echte Datei, Variante 0 und 1) und das Tor ROOM1170 (eingebacken, Variante 0 und 1).
 *    Referenz: tests/unit/gen/tuerseq_referenz.inc (tools/tor/tuerseq_referenz.py).
 * 2. RotMatrix (RE2 libgte @0x8008e1f4) an Achsenwinkeln.
 * 3. Zuordnung: Tor-Tueren ja, Intro-Uebergabe ROOM1170 Slot 3 (Rechteck 0, Band 0) nein.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_door_seq.h"
#include "gen/tuerseq_referenz.inc"

static int g_fehler;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; printf("FEHLER: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *lesen(const char *pfad, int *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long g = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)g);
    if (b && fread(b, 1, (size_t)g, f) != (size_t)g) { free(b); b = NULL; }
    fclose(f);
    *n = (int)g;
    return b;
}

static void lauf(const char *name, const uint8_t *teil, int n, int variante,
                 const int32_t (*ref)[3][7], int n_ref, const int *tone, int n_tone, int schliess)
{
    re15_door_seq_t s;
    PRUEF(re15_door_seq_start(&s, teil, n, variante, 0, 0) == 0, "%s: Start", name);
    int bild = 0, ton_i = 0, abw = 0;
    while (re15_door_seq_bild(&s, 1)) {
        if (bild < n_ref) {
            for (int o = 0; o < 3; o++) {
                const int32_t *r = ref[bild][o];
                const re15_door_obj_t *ob = &s.obj[o];
                int an = ob->on ? 1 : 0;
                if (an != r[0] || (an && (ob->pos[0] != r[1] || ob->pos[1] != r[2] || ob->pos[2] != r[3]
                                   || ob->rot[0] != (uint16_t)r[4] || ob->rot[1] != (uint16_t)r[5]
                                   || ob->rot[2] != (uint16_t)r[6]))) {
                    if (abw++ < 5)
                        printf("  %s Bild %d Obj %d: ist an=%d (%d,%d,%d) (%u,%u,%u)  soll an=%d (%d,%d,%d) (%d,%d,%d)\n",
                               name, bild, o, an, ob->pos[0], ob->pos[1], ob->pos[2], ob->rot[0], ob->rot[1],
                               ob->rot[2], (int)r[0], (int)r[1], (int)r[2], (int)r[3], (int)r[4], (int)r[5], (int)r[6]);
                }
            }
        }
        for (int t = 0; t < s.n_ton; t++) {
            PRUEF(ton_i < n_tone && tone[ton_i] == bild, "%s: Se_on in Bild %d unerwartet", name, bild);
            ton_i++;
        }
        bild++;
        if (bild > 2000) break;
    }
    PRUEF(abw == 0, "%s: %d Objekt-Abweichungen", name, abw);
    PRUEF(bild == n_ref, "%s: %d Bilder, Referenz %d", name, bild, n_ref);
    PRUEF(ton_i == n_tone, "%s: %d Se_on, Referenz %d", name, ton_i, n_tone);
    PRUEF(s.schliesston == schliess, "%s: Schliesston-Merker %d, Referenz %d", name, s.schliesston, schliess);
    PRUEF(s.notizen == 0, "%s: Notizen 0x%x", name, s.notizen);
    PRUEF(s.md1_ok && s.tim_ok, "%s: MD1/TIM nicht lesbar", name);
    printf("%-16s %d Bilder, %d Se_on, Schliesston %d, Abweichungen %d\n", name, bild, ton_i, s.schliesston, abw);
    re15_door_seq_ende(&s);
}

int main(void)
{
    /* --- DOOR2E aus der RE2-Datei; Modellteil ueber die EXE-Tabelle @0x8009a520 (12 B je Tuer:
     *     u16 Tonteil, u16 Modellteil, u32 Sektor; 03 Abschnitt 2.1). --- */
    int n_exe = 0, n_do2 = 0;
    uint8_t *exe = lesen(RE15_RE2_EXE_PATH, &n_exe);
    uint8_t *do2 = lesen(RE15_RE2_DOOR_DIR "/DOOR2E.DO2", &n_do2);
    PRUEF(exe && do2, "RE2-Dateien fehlen");
    if (exe && do2) {
        uint32_t t_addr = exe[0x18] | (exe[0x19] << 8) | (exe[0x1a] << 16) | ((uint32_t)exe[0x1b] << 24);
        uint32_t e = 0x8009A520u + 0x2E * 12 - t_addr + 0x800;
        int mdl = exe[e + 2] | (exe[e + 3] << 8);
        uint32_t sek = exe[e + 4] | (exe[e + 5] << 8) | (exe[e + 6] << 16) | ((uint32_t)exe[e + 7] << 24);
        PRUEF(sek * 0x800 + (uint32_t)mdl == (uint32_t)n_do2, "DOOR2E: Tabelle %u/%d passt nicht zur Datei %d", sek, mdl, n_do2);
        lauf("DOOR2E V0", do2 + sek * 0x800, mdl, 0, ref_door2e_v0_bilder,
             (int)(sizeof ref_door2e_v0_bilder / sizeof ref_door2e_v0_bilder[0]),
             ref_door2e_v0_tone, ref_door2e_v0_n_tone, ref_door2e_v0_schliesston);
        lauf("DOOR2E V1", do2 + sek * 0x800, mdl, 1, ref_door2e_v1_bilder,
             (int)(sizeof ref_door2e_v1_bilder / sizeof ref_door2e_v1_bilder[0]),
             ref_door2e_v1_tone, ref_door2e_v1_n_tone, ref_door2e_v1_schliesston);
    }

    /* --- das Tor --- */
    int n_tor = 0;
    const uint8_t *tor = re15_door_seq_archiv(RE15_DOOR_ARCHIV_TOR1170, &n_tor);
    PRUEF(tor && n_tor > 0, "Torarchiv fehlt");
    lauf("TOR1170 V0", tor, n_tor, 0, ref_tor_v0_bilder, (int)(sizeof ref_tor_v0_bilder / sizeof ref_tor_v0_bilder[0]),
         ref_tor_v0_tone, ref_tor_v0_n_tone, ref_tor_v0_schliesston);
    lauf("TOR1170 V1", tor, n_tor, 1, ref_tor_v1_bilder, (int)(sizeof ref_tor_v1_bilder / sizeof ref_tor_v1_bilder[0]),
         ref_tor_v1_tone, ref_tor_v1_n_tone, ref_tor_v1_schliesston);

    /* --- RotMatrix an Achsenwinkeln --- */
    {
        int16_t m[9];
        uint16_t r0[3] = {0, 0, 0}, ry[3] = {0, 1024, 0}, ryn[3] = {0, (uint16_t)-1024, 0};
        re15_door_rotmatrix(r0, m);
        PRUEF(m[0] == 4096 && m[4] == 4096 && m[8] == 4096 && m[1] == 0 && m[2] == 0 && m[6] == 0, "RotMatrix(0)");
        re15_door_rotmatrix(ry, m);
        PRUEF(m[0] == 0 && m[2] == 4096 && m[4] == 4096 && m[6] == -4096 && m[8] == 0, "RotMatrix(y=1024)");
        re15_door_rotmatrix(ryn, m);
        PRUEF(m[2] == -4096 && m[6] == 4096, "RotMatrix(y=-1024)");
    }

    /* --- Zuordnung --- */
    {
        int v = -1;
        PRUEF(re15_door_seq_zuordnen(0x1170, 2550, 15250, 1050, 850, 4, &v) == RE15_DOOR_ARCHIV_TOR1170 && v == 0, "1170 Slot 0");
        PRUEF(re15_door_seq_zuordnen(0x1170, -11065, -27850, 875, 600, 4, &v) == RE15_DOOR_ARCHIV_TOR1170 && v == 1, "1170 Slot 6");
        PRUEF(re15_door_seq_zuordnen(0x1171, -11065, -27850, 875, 600, 4, &v) == RE15_DOOR_ARCHIV_TOR1170 && v == 1, "1171 Slot 5");
        PRUEF(re15_door_seq_zuordnen(0x1170, 0, 0, 0, 0, 0, &v) == RE15_DOOR_ARCHIV_KEINS, "1170 Slot 3 (Intro) darf nicht");
        PRUEF(re15_door_seq_zuordnen(0x1140, 2550, 15250, 1050, 850, 4, &v) == RE15_DOOR_ARCHIV_KEINS, "anderer Raum darf nicht");
    }

    free(exe);
    free(do2);
    printf(g_fehler ? "unit_door_seq: %d FEHLER\n" : "unit_door_seq: OK\n", g_fehler);
    return g_fehler ? 1 : 0;
}
