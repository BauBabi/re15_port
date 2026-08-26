/* test_1190_weste.c — PIN: die Polizeiweste in ROOM1190 (Prop 16)
 *
 * NUTZER-BEFUND (2026-08-26): "Im Room 1190 ist statt der Weste von Leon ein gelbes
 * viereck zu sehen."
 *
 * ── WAS DAS ORIGINAL TUT ─────────────────────────────────────────────────────────────
 * Es hat GAR KEINE feste Prop-Grenze. Der Objekt-Pool liegt @0x800b3f98, Schrittweite
 * 148 (0x94), und wird ueber die obj_id INDIZIERT — Obj_model_set-Handler LAB_80040914:
 *     8004093c  lbu   a3,0x1(a2)   ; a3 = obj_id = pc[1]
 *     80040944  sll   v0,a3,0x3    ; id*8
 *     80040948  addu  v0,v0,a3     ; id*9
 *     8004094c  sll   v0,v0,0x2    ; id*36
 *     80040950  addu  v0,v0,a3     ; id*37
 *     80040954  sll   v0,v0,0x2    ; id*148
 *     80040958  addu  a1,v0,v1     ; a1 = 0x800b3f98 + obj_id*148
 * Die Schleifen-Schranke ueber den Pool ist in BEIDEN Konsumenten der RDT-Kopfwert
 * nOmodel (RDT+0x02), gelesen ueber den residenten RDT-Zeiger DAT_800ac778:
 *     8002be5c  lbu v0,0x2(v0)     (FUN_8002bd44, Prop-Ausschub; Schritt @0x8002be78
 *                                   `addiu s0,s0,0x94`)
 *     80043758  lbu v0,0x2(v0)     (FUN_800436a8, AOT-Objektdurchlauf)
 * Zweit-Installation derselben obj_id trifft denselben Slot und schaltet ihn AUS:
 *     80040978  lw   v0,0x0(a1)            ; Flags des Slots
 *     80040980  beq  v0,zero,LAB_80040990  ; leer  -> Flags = pc[6..7]|1
 *     80040988  j    LAB_800409a8          ; belegt -> weiter bei Pos/Rot/Box …
 *     8004098c  _sw  zero,0x0(a1)          ;           … aber Flags = 0
 *
 * ── WAS DER PORT TAT ─────────────────────────────────────────────────────────────────
 * RE15_RDT_MAX_PROPS war 16. Zensus ueber alle 206 ausgelieferten RDTs (Kopfbyte
 * RDT+0x02): nOmodel = {0:49, 1:38, 2:30, 3:23, 4:32, 5:8, 6:3, 7:7, 8:4, 9:6, 12:4,
 * 17:2} — Maximum 17, und zwar in GENAU zwei Raeumen: ROOM1190 und ROOM1191. Genau dort
 * fiel Prop 16 weg. Prop 16 ist die POLIZEIWESTE: Tabellenbasis = u32 @RDT+0x30 =
 * 0x000240, Eintrag 16 = TIM @Datei 0x048904 (33312 B, 8bpp 128x256, CLUT 256x1
 * @(0,480)) + MD1 @Datei 0x0054C0 (4396 B). Die TIM dekodiert sichtbar als Weste mit
 * "POLICE"-Schriftzug vorn und RPD-Wappen hinten.
 *
 * ZWEITES, STILLES SYMPTOM: parse_props setzt `entries = npr * 2` MIT dem gekappten npr.
 * Die Grenzensuche rdt_next_boundary sah dadurch die beiden letzten Tabelleneintraege
 * nicht, also lief die Modell-Scheibe von Prop 15 ueber Prop 16 hinweg — gemessen 7816 B
 * statt der richtigen 3420 B (0x0054C0 - 0x004764). Nicht nur ein fehlendes Modell,
 * auch ein falsch geschnittenes Nachbarmodell.
 *
 * ── WAS DIESER PIN PRUEFT ────────────────────────────────────────────────────────────
 *   P1  ZENSUS-WACHE: kein ausgeliefertes RDT verlangt mehr Props, als der Port fasst.
 *       Faengt einen kuenftigen Raum, der ueber 17 geht — DIE Klasse Fehler, die diesen
 *       Bug erzeugt hat, und die stillschweigend abschneidet statt zu knallen.
 *   P2  ROOM1190: Kopf sagt 17, Parser liefert 17. (Mit dem alten Cap: 16 -> ROT.)
 *   P3  Prop 16 ist DA und ist byte-genau die Weste — Groessen UND FNV-1a-Pruefsumme
 *       ueber die echten Bytes, plus TIM-Kopf 8bpp/128x256. (Alt: Zeiger NULL -> ROT.)
 *   P4  Prop 15s Modell-Scheibe ist 3420 B, nicht 7816 — die Nachbar-Verfaelschung.
 *       (Alt: 7816 -> ROT.)
 *   P5  ZWEIT-INSTALLATION SCHALTET AB (@0x8004098c): obj_id zweimal installiert ergibt
 *       EINEN Pool-Eintrag mit Flags 0, keinen Duplikat-Slot. (Alt: 2 Eintraege -> ROT.)
 *   P6  Der Pool fasst mindestens die groesste game-weit vorkommende obj_id + 1.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "re15_rdt.h"
#include "re15_scd.h"

extern void scd_vm_init(void);
extern void scd_vm_tick(void);
extern int  scd_thread_start(int slot, const uint8_t *pc);

static int g_fail = 0;
#define CHECK(cond, msg) do {                                                  \
        if (!(cond)) { printf("  FAIL: %s\n", (msg)); g_fail++; }               \
        else         { printf("  ok  : %s\n", (msg)); }                         \
    } while (0)

static uint32_t fnv1a(const uint8_t *d, int n)
{
    uint32_t h = 0x811c9dc5u;
    for (int i = 0; i < n; i++) { h ^= d[i]; h *= 0x01000193u; }
    return h;
}

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    *out_sz = sz;
    return b;
}

/* ---- P1: Zensus ueber ALLE ausgelieferten RDTs -------------------------------------- */
static void census(int *max_nomodel, char *worst, size_t worst_sz, int *files)
{
    static const char *stages[] = { "STAGE1","STAGE2","STAGE3","STAGE4","STAGE5","STAGE6" };
    *max_nomodel = 0; *files = 0; worst[0] = 0;
    for (unsigned s = 0; s < sizeof stages / sizeof *stages; s++) {
        char dirp[512];
        snprintf(dirp, sizeof dirp, "%s/%s", RE15_ASSET_PSX_DIR, stages[s]);
        DIR *d = opendir(dirp);
        if (!d) continue;
        struct dirent *e;
        while ((e = readdir(d)) != NULL) {
            size_t n = strlen(e->d_name);
            if (n < 5 || strcmp(e->d_name + n - 4, ".RDT") != 0) continue;
            char fp[1024];
            snprintf(fp, sizeof fp, "%s/%s", dirp, e->d_name);
            long sz = 0;
            uint8_t *b = slurp(fp, &sz);
            if (!b || sz < 0x40) { free(b); continue; }
            (*files)++;
            if (b[2] > *max_nomodel) {
                *max_nomodel = b[2];
                snprintf(worst, worst_sz, "%s/%s", stages[s], e->d_name);
            }
            free(b);
        }
        closedir(d);
    }
}

int main(void)
{
    printf("== test_1190_weste ==\n");

    /* ---- P1 ------------------------------------------------------------------------ */
    {
        int mx = 0, files = 0; char worst[256];
        census(&mx, worst, sizeof worst, &files);
        printf("  Zensus: %d RDTs, groesstes nOmodel = %d (%s), Port fasst %d\n",
               files, mx, worst, RE15_RDT_MAX_PROPS);
        CHECK(files >= 200, "Zensus hat die RDTs wirklich gelesen (>= 200 Dateien)");
        CHECK(mx == 17, "groesstes nOmodel game-weit ist 17 (Messwert 2026-08-26)");
        CHECK(mx <= RE15_RDT_MAX_PROPS,
              "RE15_RDT_MAX_PROPS deckt JEDEN ausgelieferten Raum ab "
              "(sonst schneidet der Parser still ab)");
    }

    /* ---- P2..P4 -------------------------------------------------------------------- */
    {
        char fp[512];
        snprintf(fp, sizeof fp, "%s/STAGE1/ROOM1190.RDT", RE15_ASSET_PSX_DIR);
        long sz = 0;
        uint8_t *b = slurp(fp, &sz);
        if (!b) { printf("  FAIL: %s nicht lesbar\n", fp); return 1; }

        printf("  ROOM1190.RDT: %ld B, Kopf nOmodel = %u\n", sz, b[2]);
        CHECK(b[2] == 17, "P2a Kopfbyte RDT+0x02 == 17");

        static re15_rdt_t rdt;
        int pr = re15_rdt_parse(b, (size_t)sz, &rdt);
        CHECK(pr == 0, "P2b RDT parst");
        printf("  Parser prop_count = %d\n", rdt.prop_count);
        CHECK(rdt.prop_count == 17,
              "P2c Parser liefert ALLE 17 Props (mit dem alten Cap 16 waren es 16)");

        /* P3 — Prop 16 ist byte-genau die Weste.
         * ⛔ DIE KAPAZITAETS-SCHRANKE MUSS VOR DEM ZUGRIFF STEHEN. Faellt
         * RE15_RDT_MAX_PROPS je wieder auf 16, ist prop_tim[16] KEIN NULL-Zeiger,
         * sondern ein Zugriff HINTER das Feld — der Pin wuerde abstuerzen statt
         * sauber rot zu werden, und ein Absturz sagt weniger als ein Befund.
         * (Gemessen: mit Cap 16 endete dieser Pin mit Exit 139.) */
        CHECK(RE15_RDT_MAX_PROPS > 16,
              "P3z Kapazitaet reicht ueberhaupt bis Prop 16 (sonst waere alles "
              "Folgende ein Zugriff hinter das Feld)");
        if (RE15_RDT_MAX_PROPS > 16) {
        CHECK(rdt.prop_tim[16] != NULL && rdt.prop_md1[16] != NULL,
              "P3a Prop 16 hat TIM und MD1 (mit dem alten Cap waren beide NULL)");
        }
        if (RE15_RDT_MAX_PROPS > 16 && rdt.prop_tim[16] && rdt.prop_md1[16]) {
            printf("  Prop16: TIM %d B @Datei 0x%06lX, MD1 %d B @Datei 0x%06lX\n",
                   rdt.prop_tim_size[16], (unsigned long)(rdt.prop_tim[16] - b),
                   rdt.prop_md1_size[16], (unsigned long)(rdt.prop_md1[16] - b));
            CHECK(rdt.prop_tim[16] - b == 0x048904L, "P3b TIM liegt @Datei 0x048904");
            CHECK(rdt.prop_md1[16] - b == 0x0054C0L, "P3c MD1 liegt @Datei 0x0054C0");
            CHECK(rdt.prop_tim_size[16] == 33312,    "P3d TIM ist 33312 B");
            CHECK(rdt.prop_md1_size[16] == 4396,     "P3e MD1 ist 4396 B");

            /* TIM-Kopf: id 0x10, flags&3 == 1 (8bpp); CLUT 256x1 @(0,480);
             * Pixelblock 64x256 Halbworte = 128x256 Texel. */
            const uint8_t *t = rdt.prop_tim[16];
            uint32_t id  = (uint32_t)t[0] | ((uint32_t)t[1] << 8) |
                           ((uint32_t)t[2] << 16) | ((uint32_t)t[3] << 24);
            uint32_t fl  = (uint32_t)t[4] | ((uint32_t)t[5] << 8) |
                           ((uint32_t)t[6] << 16) | ((uint32_t)t[7] << 24);
            uint32_t cl  = (uint32_t)t[8] | ((uint32_t)t[9] << 8) |
                           ((uint32_t)t[10] << 16) | ((uint32_t)t[11] << 24);
            uint16_t cx  = (uint16_t)(t[12] | (t[13] << 8));
            uint16_t cy  = (uint16_t)(t[14] | (t[15] << 8));
            uint16_t cw  = (uint16_t)(t[16] | (t[17] << 8));
            uint16_t ch  = (uint16_t)(t[18] | (t[19] << 8));
            const uint8_t *px = t + 8 + cl;
            uint16_t pw  = (uint16_t)(px[8]  | (px[9]  << 8));
            uint16_t ph  = (uint16_t)(px[10] | (px[11] << 8));
            printf("  TIM: id=0x%02X flags=%u CLUT %ux%u @(%u,%u)  Pixel %ux%u -> %u x %u\n",
                   id, fl & 3u, cw, ch, cx, cy, pw, ph, (unsigned)(pw * 2), ph);
            CHECK(id == 0x10u,                 "P3f TIM-Magic 0x10");
            CHECK((fl & 3u) == 1u,             "P3g 8bpp");
            CHECK(cw == 256 && ch == 1,        "P3h CLUT 256x1");
            CHECK(cx == 0 && cy == 480,        "P3i CLUT liegt @(0,480)");
            CHECK(pw * 2 == 128 && ph == 256,  "P3j Textur ist 128x256");

            /* Inhalt, nicht nur Groesse: FNV-1a ueber die echten Bytes. */
            uint32_t ht = fnv1a(rdt.prop_tim[16], rdt.prop_tim_size[16]);
            uint32_t hm = fnv1a(rdt.prop_md1[16], rdt.prop_md1_size[16]);
            printf("  Prop16 FNV1a: TIM=0x%08X MD1=0x%08X\n", ht, hm);
            CHECK(ht == 0x843C32DDu, "P3k TIM-Inhalt == die Polizeiwesten-Textur");
            CHECK(hm == 0x83F26033u, "P3l MD1-Inhalt == das Westen-Modell");
        }

        /* P4 — der Nachbar darf nicht ueber Prop 16 hinweggeschnitten sein. */
        printf("  Prop15 MD1 = %d B (falsch waeren 7816 = ueber Prop 16 hinweg)\n",
               rdt.prop_md1_size[15]);
        CHECK(rdt.prop_md1_size[15] == 3420,
              "P4a Prop 15s Modell endet bei Prop 16 (0x0054C0 - 0x004764 = 3420)");
        CHECK(rdt.prop_md1_size[15] != 7816,
              "P4b Prop 15 ist NICHT mehr ueber Prop 16 hinweggeschnitten");
        if (rdt.prop_md1[15])
            CHECK(fnv1a(rdt.prop_md1[15], rdt.prop_md1_size[15]) == 0x0563A0B9u,
                  "P4c Prop 15s Modell-Bytes stimmen exakt");

        free(b);
    }

    /* ---- P5: Zweit-Installation schaltet ab (@0x8004098c) --------------------------- */
    {
        /* Zwei Obj_model_set-Records fuer DIESELBE obj_id 3, danach Evt_end.
         * Layout (LAB_80040914): pc[0]=0x2D, pc[1]=obj_id, pc[2]=type, pc[4]=band,
         * pc[6..7]=flags-Rohwert, pc[10..15]=pos, pc[16..21]=rot, pc[22..33]=box.
         * Opcode-Laenge fest 34 (@0x80040aa4 `addiu v0,a2,0x22`). */
        static uint8_t bc[34 + 34 + 1];
        memset(bc, 0, sizeof bc);
        bc[0]  = 0x2D; bc[1] = 3; bc[2] = 0; bc[4] = 1; bc[6] = 0x0A; bc[7] = 0x00;
        bc[34] = 0x2D; bc[35] = 3; bc[36] = 0; bc[38] = 1; bc[40] = 0x0A; bc[41] = 0x00;
        bc[68] = 0x00;   /* Nop -> Thread laeuft weiter, Test liest danach den Pool */

        scd_vm_init();
        scd_thread_start(0, bc);
        scd_vm_tick();

        printf("  nach zwei Obj_model_set(id=3): prop_count = %u\n",
               (unsigned)g_scd.prop_count);
        CHECK(g_scd.prop_count == 1,
              "P5a zweimal dieselbe obj_id ergibt EINEN Pool-Eintrag, keinen Duplikat-Slot");
        if (g_scd.prop_count >= 1) {
            printf("  pool[0]: obj_id=%u flags=0x%04X aktiv=%u\n",
                   (unsigned)g_scd.props[0].obj_id, (unsigned)g_scd.props[0].flags,
                   (unsigned)g_scd.props[0].active);
            CHECK(g_scd.props[0].obj_id == 3, "P5b es ist obj_id 3");
            CHECK(g_scd.props[0].flags == 0,
                  "P5c Flags == 0 (@0x8004098c `sw zero,0x0(a1)`) = Objekt AUS");
            CHECK(g_scd.props[0].active == 0, "P5d und damit inaktiv");
        }

        /* Gegenprobe: EINE Installation muss weiterhin AN sein — sonst waere P5c
         * vakuant (alles aus = trivial bestanden). */
        static uint8_t bc1[34 + 1];
        memset(bc1, 0, sizeof bc1);
        bc1[0] = 0x2D; bc1[1] = 3; bc1[2] = 0; bc1[4] = 1; bc1[6] = 0x0A; bc1[7] = 0x00;
        scd_vm_init();
        scd_thread_start(0, bc1);
        scd_vm_tick();
        printf("  Gegenprobe eine Installation: prop_count=%u flags=0x%04X aktiv=%u\n",
               (unsigned)g_scd.prop_count, (unsigned)g_scd.props[0].flags,
               (unsigned)g_scd.props[0].active);
        CHECK(g_scd.prop_count == 1 && g_scd.props[0].flags == 0x000Bu &&
              g_scd.props[0].active == 1,
              "P5e GEGENPROBE: eine einzelne Installation bleibt AN (Flags 0x0A|1 = 0x0B)");
    }

    /* ---- P6 ------------------------------------------------------------------------ */
    printf("  Pool: RE15_SCD_MAX_PROPS = %d, groesste game-weite obj_id = 16\n",
           RE15_SCD_MAX_PROPS);
    CHECK(RE15_SCD_MAX_PROPS >= 17,
          "P6 der Prop-Pool fasst die groesste vorkommende obj_id (16) + 1");

    printf(g_fail ? "== test_1190_weste: %d FEHLER ==\n" : "== test_1190_weste: OK ==\n",
           g_fail);
    return g_fail ? 1 : 0;
}
