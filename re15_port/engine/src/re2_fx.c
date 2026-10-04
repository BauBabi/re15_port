/*
 * re2_fx.c — Runde 34 Spur D: die RE2-FX-MASCHINE (RE2-Retail-Effektpool) fuer den Saeure-/Brand-
 * Aufschlag der Granaten 0x0A/0x0B (BAUPLAN E8, §1.3/§1.4, K8) und das Bodenfeuer.
 *
 * Quelle: info/re2leon/PSX.EXE (t_addr 0x80010000), in der Bau-Sitzung mit re2_disasm.py gelesen
 * (Roh-Disasm build/r34g_d/dis/, Dossier analysis/befunde_runde34_granaten/bau_d.md). Jede
 * Verhaltens-Konstante traegt ihre @0x-Adresse. GTE-Worte (.word im Werkzeug) sind von Hand
 * dekodiert und im Dossier mit Rohwort belegt.
 *
 * Aufbau (1:1 zum Original):
 *   Registrierung  FUN_8001babc (Boot) + FUN_8001bca0             re2fx_register_core
 *   Spawner        FUN_8001cbe8 (0x4000) / FUN_8001bf10 (0xA003)  spawn_kern
 *   Pumpe          FUN_8001d300 (Update-Pass, Draw-Pass)          re2fx_tick
 *   Platz-Schritt  FUN_8001d68c (Op A?, Weltlage, Op B, Physik, Anim)   schritt
 *   Weltlage       FUN_8001d894                                   weltlage
 *   Ops            Tabelle 0x8009D868 (`lw v0,-10136(at)` + `jalr` @0x8001d6b8/@0x8001d6f0):
 *                  0/1/2/19/25/27/28/29/30/40/46/48/49/50/58/64
 *   RNG            FUN_80015FE8 = re15_re2_rand() (EIN Strom fuer alle RE2-Overlays, 0..255)
 *
 * Port-Zuordnungen (je im Dossier begruendet): Aufschlag-Platz (re2fx_aufschlag, E8 + O-VB1 §1.4),
 * Boden/Wand der Flammen (re2fx_boden, O-VB2 §2.4), keine RE2-Hitcodes im Aufschlag (E3).
 */
#include <string.h>
#include "re2_fx.h"
#include "re15_ai_flavor.h"   /* re15_re2_rand — FUN_80015FE8, geteilter RE2-Strom */
#include "re15_scd.h"         /* g_re15_pauseflags / RE15_PAUSE_ACTION (= 0x800CFBDC & 0x10000000) */
#include "re15_skeleton.h"    /* re15_sin_q12 / re15_cos_q12 = Tafel 0x800ADEAC (bytegleich RE1.5 0x800794C4) */
#include "re15_room.h"        /* g_room_rdt / g_room_rdt_ok — O-VB2-Abbildung */
#include "re15_collision.h"   /* room_coll (FUN_8001c6e8), box_blocked (FUN_8003b558), prop_box_hit */
#include "re15_aot.h"         /* re15_aot_water_at = FUN_800527b4-Zwilling; re15_esp_fx_culled */
#include "re15_math.h"        /* re15_gte_divide - RTPS-Kehrwert wie pc_draw_effects */

int  (*re2fx_applier)(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode) = NULL;
void (*re2fx_se_hook)(uint32_t code, const int32_t pos[3]) = NULL;
int32_t (*re2fx_boden_hook)(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt) = NULL;
int32_t (*re2fx_wasser_hook)(int32_t x, int32_t z) = NULL;

/* ---------------------------------------------------------------------------------------------
 * Byte-Zugriff auf das Platz-Abbild (little endian wie der R3000)
 * ------------------------------------------------------------------------------------------- */
static inline uint16_t rd16(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
static inline int16_t  rds16(const uint8_t *b, int o) { return (int16_t)rd16(b, o); }
static inline uint32_t rd32(const uint8_t *b, int o)
{ return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24); }
static inline void wr16(uint8_t *b, int o, uint32_t v) { b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); }
static inline void wr32(uint8_t *b, int o, uint32_t v)
{ b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); b[o + 2] = (uint8_t)(v >> 16); b[o + 3] = (uint8_t)(v >> 24); }

/* ---------------------------------------------------------------------------------------------
 * Pool 0x800D8CF0, 96 x 0x7C. +0x6C (Matrix-Zeiger) haelt der Port neben dem Abbild.
 * ------------------------------------------------------------------------------------------- */
typedef struct {
    uint8_t        b[RE2FX_PLATZ_BYTES];
    const uint8_t *mtx_src;                 /* +0x6C (a2 des Spawns), gelesen nur bei Status 0x800 */
} re2fx_platz_t;

static re2fx_platz_t s_pool[RE2FX_PLAETZE];
static int           s_cur;                 /* 0x800DCBD0 = laufender Platz */

/* Registrierte Bankdatei (CORE00.ESP) + die beiden 64er-Tabellen 0x800D4CD8 / 0x800D4E18 als
 * DATEI-OFFSETS (-1 = ungueltig, Boot `sw a1(=-1)` @0x8001bb2c/@0x8001bb34) und die 16er-Id-Liste
 * 0x800EAE48 (Boot 0xFF @0x8001bafc). */
static const uint8_t *s_esp;
static size_t         s_esp_size;
static int32_t        s_bank_off[64];
static int32_t        s_tab_off[64];
static uint8_t        s_registry[16];
/* Runde 35 Spur M: die RAUM-ESP (Registry-Plaetze 8..15, re2fx_register_raum). Ihre Offsets tragen
 * RE2FX_RAUM_MARKE, damit EIN Offsetraum fuer beide Dateien gilt (esp_at loest auf). */
static const uint8_t *s_raum;
static size_t         s_raum_size;
#define RE2FX_RAUM_MARKE 0x40000000u   /* Bit 30: s_bank_off/s_tab_off bleiben als int32 >= 0 (Gueltigkeitstest "< 0") */

static unsigned s_op_unbekannt;
static unsigned s_op_zaehler[96];

/* 0x8009DB44 = Einheitsmatrix (`bytes 0x8009db44 32`: 00 10 00 00 00 00 00 00 00 10 00 00 …). */
const uint8_t re2fx_einheitsmatrix[32] = {
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* O-VB1 (bau_d.md §1.4): B = Teil-11-Rotation des RE2-Spielerskeletts im Schussbild der WAAGRECHTEN
 * GL-Haltung, gerechnet mit dem Port-Skelettcode aus info/re2leon/PL0/PLD/PL01.PLD (Skelett, EMR
 * dir[1]) + PL01W09.PLW (Keyframes, EMR dir[1]; bytegleich PL01W0A/PL01W0B, md5 3eccb0d3…),
 * Clip 10 Bild 0 = Keyframe 271. Clip 10/12/14 = Laufachse (4079,7,-5)/(3885,-1239,-4)/(2932,2832,-6)
 * = waagrecht / hoch / tief. Laufachse = lokal +y (Muendung {120,1200,0} `addiu v0,zero,1200 /
 * sh v0,18(sp)` @0x80044f84-88); lokal +x = Welt-oben (x-Spalte (12,-4076,-23)) -> acc.x -23 der
 * Saeure (@0x80021738/@0x80021764) zieht nach unten. Die Sonde probe_r34_re2fx_knochen rechnet B
 * aus den Dateien nach. Zeilenweise m[r][c]. */
static const int16_t s_gl_basis[9] = {
       12, 4079,   -1,
    -4076,    7,  -25,
      -23,   -5, 4080 };

void re2fx_gl_basis(int16_t out[9]) { if (out) memcpy(out, s_gl_basis, sizeof s_gl_basis); }

const uint8_t *re2fx_platz(int i)
{
    if (i < 0 || i >= RE2FX_PLAETZE) return NULL;
    return s_pool[i].b;
}

uint8_t *re2fx_platz_sonde(int i)
{
    if (i < 0 || i >= RE2FX_PLAETZE) return NULL;
    return s_pool[i].b;
}

const uint8_t *re2fx_esp_daten(size_t *groesse)
{
    if (groesse) *groesse = s_esp ? s_esp_size : 0;
    return s_esp;
}

unsigned re2fx_op_unbekannt(void) { return s_op_unbekannt; }
unsigned re2fx_op_zaehler(int op) { return (op >= 0 && op < 96) ? s_op_zaehler[op] : 0u; }

/* Sichere Lesezugriffe in die registrierte Datei (das Original prueft nichts; die Datei ist
 * konsistent — ausserhalb liefert der Port 0 und zaehlt es als unbekannt). */
static const uint8_t k_null[32] = {0};             /* groesster Leser: 24-Byte-Step */
static const uint8_t *esp_at(uint32_t off, uint32_t n)
{
    if (off & RE2FX_RAUM_MARKE) {                      /* Runde 35 Spur M: Raum-ESP */
        off &= ~RE2FX_RAUM_MARKE;
        if (!s_raum || n > sizeof k_null || (size_t)off + n > s_raum_size) { s_op_unbekannt++; return k_null; }
        return s_raum + off;
    }
    if (!s_esp || n > sizeof k_null || (size_t)off + n > s_esp_size) { s_op_unbekannt++; return k_null; }
    return s_esp + off;
}

/* BEZUGSEBENE des Bodentests (Runde 34, Nachtrag nach mess_geg.md). RE2 FUN_8004fba0 setzt die
 * Grundwerte Rueckgabe := 0 (`sh zero,15228(at)` @0x8004fc3c) und Kontakt := (P.y > 0)
 * (@0x8004fc48-58): Ebene y 0. Die RE1.5-Raeume des Ports haben begehbare Boeden auch auf
 * -1800*Band; ohne Zelle darunter fielen die Flammen dort bis y 0 durch.
 * ⛔ PORT-ZUORDNUNG, KEINE ORIGINAL-ADRESSE: Ebene = Standhoehe des Werfers (RE1.5 Spieler-y :=
 * -1800*Band @0x8001d7b8-d4), gesetzt von der Granate unmittelbar vor dem Aufschlag
 * (re15_esp.c Routine 31, granate_boden). 0 = byte-gleich zu RE2. */
static int32_t s_boden_basis = 0;
void re2fx_boden_basis_setzen(int32_t y) { s_boden_basis = y; }

/* ---------------------------------------------------------------------------------------------
 * Registrierung — FUN_8001babc (Boot-Teil) + FUN_8001bca0
 * ------------------------------------------------------------------------------------------- */
void re2fx_reset(void)
{
    /* @0x8001bac4-e4: 96 Plaetze, Status +0x18 := 0 (`sh zero,-29432(at)` = 0x800D8D08). Der Port
     * nullt das ganze Abbild (die uebrigen Felder schreibt jeder Spawn neu, @0x8001cc80-0x8001cdbc).
     * Raumwechsel-Aufrufer = Plattform (re2fx_reset an denselben Stellen wie re15_esp_fx_reset). */
    s_boden_basis = 0;
    memset(s_pool, 0, sizeof s_pool);
    s_cur = 0;
    memset(s_op_zaehler, 0, sizeof s_op_zaehler);
    s_op_unbekannt = 0;
}

int re2fx_register_core(const uint8_t *raw, size_t size)
{
    /* Boot FUN_8001babc: Registry 16 x 0xFF (@0x8001bae8-b00), beide 64er-Tabellen := -1
     * (@0x8001bb08-34), Pool leer (@0x8001bac4-e4). */
    memset(s_registry, 0xFF, sizeof s_registry);
    for (int k = 0; k < 64; k++) { s_bank_off[k] = -1; s_tab_off[k] = -1; }
    s_esp = NULL; s_esp_size = 0;
    s_raum = NULL; s_raum_size = 0;                    /* Runde 35 Spur M: Raum-ESP mit geloescht */
    re2fx_reset();
    if (!raw || size < 12) return -1;
    /* a1 = Basis + align4(Groesse) - 4 = LETZTES Wort (@0x8001bb54-80: `srl 2 / sll 2`, `andi 3`
     * -> +4, dann `addiu a1,a1,-4`); a0 = a2 = Basis, a3 = 0 (@0x8001bb7c-90). */
    size_t al = (size + 3u) & ~(size_t)3u;
    int32_t ende = (int32_t)al - 4;
    int n = 0;
    for (;;) {
        /* FUN_8001bca0: `lbu v1,0(t1)` @0x8001bccc, Registry[t0+a3] := id (`sb v1,0(v0)` @0x8001bcdc),
         * id == 0xFF -> Ende (@0x8001bcd8); `lw v1,0(t2)` @0x8001bce0 = Offset, `addiu t2,t2,-4`
         * (RUECKWAERTS); Bank = Offset + Basis (`addu v1,v1,a2` @0x8001bcf8); Tabelle =
         * Bank + ((w&0xffff)*2 + (w>>16) + 2)*4 (@0x8001bd00-20); hoechstens 8 (`sltiu v0,t0,0x8`). */
        uint8_t id = raw[n];
        s_registry[n] = id;
        if (id == 0xFF) break;
        if (ende - 4 * n < 0 || (size_t)(ende - 4 * n) + 4 > size) return -2;
        uint32_t off = rd32(raw, ende - 4 * n);
        if ((size_t)off + 8 > size || id >= 64) return -3;
        uint32_t w = rd32(raw, (int)off);
        uint32_t tab = off + ((w & 0xffffu) * 2u + (w >> 16) + 2u) * 4u;
        if ((size_t)tab + 16 > size) return -4;
        s_bank_off[id] = (int32_t)off;
        s_tab_off[id]  = (int32_t)tab;
        n++;
        if (n >= 8) break;
    }
    if (n == 0) return -5;
    s_esp = raw; s_esp_size = size;
    return 0;
}

/* ---------------------------------------------------------------------------------------------
 * Spawner — FUN_8001cbe8 (aufgeschoben, 0x4000) / FUN_8001bf10 (sofort, 0xA003)
 * ------------------------------------------------------------------------------------------- */
static int spawn_kern(uint32_t a0, int16_t a1, const uint8_t *mtx, const int16_t *ofs, uint16_t st0)
{
    unsigned bank = a0 >> 24;                          /* `srl t6,a0,24` @0x8001cbec */
    unsigned sub  = (a0 >> 16) & 0xffu;                /* `srl v0,a0,16 / andi t5,v0,0xff` @0x8001cbf4-f8 */
    if (!s_esp || bank >= 64 || s_bank_off[bank] < 0 || s_tab_off[bank] < 0 || !mtx) return -1;
    uint32_t tab = (uint32_t)s_tab_off[bank];          /* `lw t4,19992(at)` = 0x800D4E18[bank] @0x8001cc0c */
    uint32_t bk  = (uint32_t)s_bank_off[bank];         /* `lw t1,19672(at)` = 0x800D4CD8[bank] @0x8001cc24 */
    uint32_t woff = rd16(esp_at(tab + (sub & 7u) * 2u, 2), 0);   /* `andi v0,t5,0x7 / sll 1 / lhu` @0x8001cbfc-cc18 */
    uint32_t skript = tab + woff * 4u;                 /* `sll v0,v0,2 / addu t4,t4,v0` @0x8001cc28-2c */
    unsigned nparts = rd16(esp_at(skript, 2), 0);      /* `lhu t3,0(t4)` @0x8001cc30 */
    uint32_t part0 = skript + 4u;                      /* `addiu t4,t4,4` @0x8001cc38 */
    unsigned n1 = rd16(esp_at(bk, 2), 0);              /* `lhu v0,0(t1)` @0x8001cc34 */

    /* Freier Platz von 95 abwaerts (@0x8001cc44-6c): Status +0x18 == 0. Voll -> 0xFF. */
    int i;
    for (i = RE2FX_PLAETZE - 1; i >= 0; i--)
        if (rd16(s_pool[i].b, 0x18) == 0) break;
    if (i < 0) return 0xFF;
    uint8_t *b = s_pool[i].b;
    const uint8_t *bh = esp_at(bk, 12);
    wr16(b, 0x18, st0);                                /* 0x4000 @0x8001cc80-84 / 0xA003 @0x8001bfa8-ac */
    b[0x1A] = 0; b[0x1B] = 0;                          /* @0x8001cc90-94 */
    wr32(b, 0x1C, (sub << 16) + bank);                 /* `sw v0,28(t0)` @0x8001cc98: +0x1C Bank, +0x1E Sub */
    wr16(b, 0x22, (uint16_t)a1);                       /* `sh a1,34(t0)` @0x8001cca0 */
    wr32(b, 0x24, 0);                                  /* @0x8001cca4 */
    wr16(b, 0x20, bh[10]);                             /* `lbu v0,10(t1) / sh v0,32(t0)` = Anim[0].Dauer, +0x21 = 0 */
    wr32(b, 0x28, (uint32_t)rd16(bh, 6) << 16);        /* `lhu v0,6(t1) / sll 16 / sw v0,40(t0)` -> +0x2A TPage */
    if (ofs) {                                         /* a3 != 0: 2 Worte nach +0x2C/+0x30 (@0x8001ccc0-d4) */
        wr32(b, 0x2C, (uint32_t)(uint16_t)ofs[0] | ((uint32_t)(uint16_t)ofs[1] << 16));
        wr32(b, 0x30, (uint32_t)(uint16_t)ofs[2] | ((uint32_t)(uint16_t)ofs[3] << 16));
    } else { wr32(b, 0x2C, 0); wr32(b, 0x30, 0); }     /* @0x8001ccd8-dc */
    wr32(b, 0x38, a0 << 16);                           /* `sll v0,a0,16 / sw v0,56(t0)` -> +0x3A Skala */
    wr32(b, 0x40, 0x10000u);                           /* `lui v0,0x1 / sw v0,64(t0)` -> +0x42 = 1 */
    wr32(b, 0x34, 0); wr32(b, 0x3C, 0); wr32(b, 0x44, 0); wr32(b, 0x48, 0);   /* @0x8001ccfc-d08 */
    wr16(b, 0x32, rd16(bh, 4) + ((sub >> 3) << 6));   /* CLUT = Kopf+4 + (sub>>3)*0x40 @0x8001ccf4-d10 */
    memcpy(b + 0x4C, mtx, 32);                         /* 8 Worte a2 -> +0x4C..+0x6B @0x8001cd14-50 */
    wr32(b, 0x70, bk + 8u);                            /* Anim-Tabelle `addiu v0,t1,8` @0x8001cd54 */
    wr32(b, 0x74, bk + (n1 * 4u + 4u) * 2u);           /* UV-Tabelle `sll v0,t7,1 / addu` @0x8001cd5c-64 */
    s_pool[i].mtx_src = mtx;                           /* `sw a2,108(t0)` @0x8001cd6c */
    wr32(b, 0x78, part0 + 4u);                         /* erster Step `addiu v0,t4,4` @0x8001cd68-70 */
    memcpy(b, esp_at(part0 + 4u, 24), 24);             /* 6 Worte Step -> +0x00..+0x17 @0x8001cd74-bc */
    if (--nparts == 0) return i;                       /* `addiu t3,t3,-1 / beq t3,zero` @0x8001cdb4-b8 */

    /* Mehrteil-Skripte (@0x8001cdc0-0x8001ceec): je weiterer Teil ein Platz UNTER dem letzten,
     * 100 Byte ab +0x18 des Haupt-Platzes kopiert, +0x1A := Haupt-Index, Teil k = Teil 0 + k
     * Spruenge (nsteps*24+4), Teile in umgekehrter Reihenfolge (t3 zaehlt abwaerts). */
    int scan = i;
    while (nparts != 0) {
        int j;
        for (j = scan - 1; j >= 0; j--)
            if (rd16(s_pool[j].b, 0x18) == 0) break;
        if (j < 0) return 0xFE;                        /* `addiu v0,zero,254` @0x8001ce10 */
        scan = j;
        uint8_t *c = s_pool[j].b;
        wr16(c, 0x18, st0); c[0x1B] = 0;               /* @0x8001ce2c-34 */
        memcpy(c + 0x18, b + 0x18, 100);               /* 16-B-Bloecke bis +96, dann 1 Wort @0x8001ce38-6c */
        s_pool[j].mtx_src = s_pool[i].mtx_src;         /* +0x6C liegt im kopierten Bereich */
        c[0x1A] = (uint8_t)i;                          /* `sb t6,26(t0)` @0x8001ce70 */
        uint32_t p = part0;
        for (unsigned k = nparts; k != 0; k--)         /* @0x8001ce7c-98 */
            p += (uint32_t)rd16(esp_at(p, 2), 0) * 24u + 4u;
        wr32(c, 0x78, p + 4u);                         /* @0x8001ce9c-a0 */
        memcpy(c, esp_at(p + 4u, 24), 24);             /* @0x8001cea4-ec */
        nparts--;
    }
    return i;
}

int re2fx_spawn(uint32_t a0, int16_t a1, const uint8_t mtx[32], const int16_t ofs[4])
{ return spawn_kern(a0, a1, mtx, ofs, 0x4000u); }
int re2fx_spawn_sofort(uint32_t a0, int16_t a1, const uint8_t mtx[32], const int16_t ofs[4])
{ return spawn_kern(a0, a1, mtx, ofs, 0xA003u); }

/* ---------------------------------------------------------------------------------------------
 * GTE-Hilfen: MVMVA sf=1 (rtv0) = (m·v) >> 12, 32-Bit-MAC (swc2 MAC1..3 = stlvnl, kein Saettigen)
 * ------------------------------------------------------------------------------------------- */
static int32_t mac12(int32_t m0, int32_t m1, int32_t m2, int32_t v0, int32_t v1, int32_t v2)
{
    int64_t s = (int64_t)m0 * v0 + (int64_t)m1 * v1 + (int64_t)m2 * v2;
    return (int32_t)(s >> 12);
}
static void matrix_lesen(const uint8_t *m32, int16_t rot[9], int32_t t[3])
{
    for (int k = 0; k < 9; k++) rot[k] = rds16(m32, 2 * k);
    t[0] = (int32_t)rd32(m32, 20); t[1] = (int32_t)rd32(m32, 24); t[2] = (int32_t)rd32(m32, 28);
}

/* RotMatrixY FUN_8008e8b4 auf die Einheitsmatrix (Tafel 0x800ADEAC `lw t9,-8532(t9)` @0x8008e8dc/
 * @0x8008e900; a >= 0: t1 = -sin, a < 0: Winkel -a, t1 = +sin; m0j = (c*m0j - t1*m2j)>>12,
 * m2j = (t1*m0j + c*m2j)>>12 @0x8008e918-0x8008ea14). Auf I: [[c,0,-t1],[0,4096,0],[t1,0,c]]. */
static void roty_einheit(int16_t a, int32_t r[9])
{
    int32_t s, c, t1;
    if (a >= 0) { int t = a & 0xfff;      s = re15_sin_q12(t); c = re15_cos_q12(t); t1 = -s; }
    else        { int t = (-a) & 0xfff;   s = re15_sin_q12(t); c = re15_cos_q12(t); t1 =  s; }
    r[0] = c;    r[1] = 0;    r[2] = -t1;
    r[3] = 0;    r[4] = 4096; r[5] = 0;
    r[6] = t1;   r[7] = 0;    r[8] = c;
}

/* ---------------------------------------------------------------------------------------------
 * FUN_8001d894 — Weltlage des laufenden Platzes (bau_d.md §1.1)
 * ------------------------------------------------------------------------------------------- */
static void weltlage(void)
{
    re2fx_platz_t *pl = &s_pool[s_cur];
    uint8_t *b = pl->b;
    if ((rd16(b, 0x18) & 0x800u) && pl->mtx_src)       /* Bit 0x800 @0x8001d8ac: Matrix *(+0x6C) -> +0x4C */
        memcpy(b + 0x4C, pl->mtx_src, 32);             /* 8 Worte @0x8001d8b8-fc */
    wr32(b, 0x3C, rd32(b, 0x34));                      /* `lw v1,52 / sw v1,60` @0x8001d954/64 */
    wr16(b, 0x40, rd16(b, 0x38));                      /* `lhu a0,56 / sh a0,64` @0x8001d95c/6c */
    int16_t rot[9]; int32_t t[3];
    matrix_lesen(b + 0x4C, rot, t);
    int32_t lx = rds16(b, 0x24), ly = rds16(b, 0x26), lz = rds16(b, 0x28);
    if (rd16(b, 0x18) & 0x400u) {                      /* `andi v0,v0,0x400` @0x8001d960 */
        /* Scratch = Einheit (@0x8001d908-50): rtv0 liefert lokal exakt; offset + lokal als s16
         * (`lhu +0x2C / addu / sh` @0x8001d9dc-a24); dann rtv0 mit +0x4C (@0x8001da28-68). */
        int16_t v0 = (int16_t)(rd16(b, 0x2C) + (uint32_t)lx);
        int16_t v1 = (int16_t)(rd16(b, 0x2E) + (uint32_t)ly);
        int16_t v2 = (int16_t)(rd16(b, 0x30) + (uint32_t)lz);
        int32_t mx = mac12(rot[0], rot[1], rot[2], v0, v1, v2);
        int32_t my = mac12(rot[3], rot[4], rot[5], v0, v1, v2);
        int32_t mz = mac12(rot[6], rot[7], rot[8], v0, v1, v2);
        wr16(b, 0x34, (uint32_t)(t[0] + mx));          /* `lw v0,96 / addu / sh v0,52` @0x8001da8c-98 */
        wr16(b, 0x36, (uint32_t)(t[1] + my));          /* @0x8001da9c-b0 */
        wr16(b, 0x38, (uint32_t)(t[2] + mz));          /* @0x8001dab4-c4 + @0x8001dc1c */
    } else {
        int32_t ry[9];                                 /* RotMatrixY(+0x22) @0x8001dac8-cc */
        roty_einheit(rds16(b, 0x22), ry);
        int32_t ax = mac12(ry[0], ry[1], ry[2], lx, ly, lz);
        int32_t ay = mac12(ry[3], ry[4], ry[5], lx, ly, lz);
        int32_t az = mac12(ry[6], ry[7], ry[8], lx, ly, lz);
        wr16(b, 0x34, (uint32_t)ax); wr16(b, 0x36, (uint32_t)ay); wr16(b, 0x38, (uint32_t)az);  /* @0x8001db50-70 */
        int32_t ox = rds16(b, 0x2C), oy = rds16(b, 0x2E), oz = rds16(b, 0x30);
        int32_t mx = mac12(rot[0], rot[1], rot[2], ox, oy, oz);   /* SetRotMatrix(+0x4C), lwc2 +0x2C @0x8001db78-bc */
        int32_t my = mac12(rot[3], rot[4], rot[5], ox, oy, oz);
        int32_t mz = mac12(rot[6], rot[7], rot[8], ox, oy, oz);
        wr16(b, 0x34, rd16(b, 0x34) + (uint32_t)(t[0] + mx));    /* `lhu 52 / addu / addu / sh` @0x8001dbd0-e4 */
        wr16(b, 0x36, rd16(b, 0x36) + (uint32_t)(t[1] + my));    /* @0x8001dbe0-c00 */
        wr16(b, 0x38, rd16(b, 0x38) + (uint32_t)(t[2] + mz));    /* @0x8001dbfc-1c */
    }
}

/* ---------------------------------------------------------------------------------------------
 * Op-Tabelle 0x8009D868
 * ------------------------------------------------------------------------------------------- */
static void op_rufen(unsigned op);
static void schritt(int mit_op_a);
static int  kind_an_lage(uint32_t a0, const uint8_t *quelle34);

static uint8_t *cur(void) { return s_pool[s_cur].b; }
static const uint8_t *anim_eintrag(const uint8_t *b, unsigned idx)
{ return esp_at(rd32(b, 0x70) + idx * 8u, 8); }
static uint32_t rng(void) { return re15_re2_rand(); }                 /* FUN_80015FE8 (0..255) */
/* Vorzeichenbehaftetes `x mod 8` wie im Code (`bgez / addiu +7 / sra 3 / sll 3 / subu`). */
static int32_t smod8(int32_t x) { int32_t q = (x < 0 ? x + 7 : x) >> 3; return x - (q << 3); }
/* Die Magic-Divisionen (mult + mfhi, sra, Vorzeichen-Korrektur, z.B. 0x55555556 = /3, 0x2e8ba2e9 >> 1
 * = /11, 0x66666667 >> 4 = /40) sind fuer den RNG-Bereich 0..255 identisch mit der C-Division mit
 * Abschneiden; der Port schreibt sie deshalb als `%` / `/` und zitiert je Stelle das Idiom. */
/* x * k / 1000 ueber `0x10624dd3` und `sra 6` (Op 19/58): (x*k*0x10624dd3) >> 38, dann minus Vorzeichen. */
static int32_t mal_durch_1000(uint32_t x, int32_t k, int vorzeichen)
{
    int32_t p = (int32_t)(x * (uint32_t)k);
    int64_t h = ((int64_t)p * 0x10624dd3LL) >> 32;          /* mfhi */
    int32_t r = (int32_t)(h >> 6);                          /* sra 6 */
    if (vorzeichen) r -= (p >> 31);                         /* subu v,v,(p>>31) */
    return r;
}

/* Op 0 = FUN_8001dc28 (`jr ra`). */
static void op_0(void) { }

/* Op 1 = FUN_8001dc30: Status := step[0x12], Anim := step[2], TPage |= step[0x14], Countdown :=
 * Anim-Dauer, Step-Index += step[0x0B], 24 Byte des neuen Steps nachladen (lwl/lwr/swl/swr
 * @0x8001dcc8-0x8001dd28, Rohworte 0x88430003 … 0xb8850014). */
static void op_1(void)
{
    uint8_t *b = cur();
    wr16(b, 0x18, rd16(b, 0x12));                     /* `lhu v1,18 / sh v1,24` @0x8001dc3c/44 */
    b[0x21] = b[0x02];                                 /* `lbu a0,2 / sb a0,33` @0x8001dc40/4c */
    wr16(b, 0x2A, rd16(b, 0x2A) | rd16(b, 0x14));     /* @0x8001dc48-60 */
    b[0x20] = anim_eintrag(b, b[0x21])[2];             /* @0x8001dc64-7c */
    b[0x1F] = (uint8_t)(b[0x1F] + b[0x0B]);            /* @0x8001dc8c-9c */
    memcpy(b, esp_at(rd32(b, 0x78) + (uint32_t)b[0x1F] * 24u, 24), 24);   /* @0x8001dcac-dd28 */
}

/* Op 2 = FUN_8001dd2c: wie Op 1, Anim-Start = step[2] + rand % (step[0x16] + 1) (`div` @0x8001dd70,
 * `mfhi` @0x8001dd98). */
static void op_2(void)
{
    uint8_t *b = cur();
    wr16(b, 0x2A, rd16(b, 0x2A) | rd16(b, 0x14));     /* @0x8001dd3c-54 */
    wr16(b, 0x18, rd16(b, 0x12));                     /* `sh a1,24(v0)` @0x8001dd4c */
    int32_t r = (int32_t)rng();                        /* `jal 0x80015fe8` @0x8001dd50 */
    int32_t m = (int32_t)rd16(b, 0x16) + 1;           /* `lhu v1,22 / addiu v1,v1,1` @0x8001dd64-6c */
    b[0x21] = (uint8_t)(b[0x02] + (uint32_t)(r % m));  /* @0x8001dd9c-a8 */
    b[0x20] = anim_eintrag(b, b[0x21])[2];             /* @0x8001ddb8-d0 */
    b[0x1F] = (uint8_t)(b[0x1F] + b[0x0B]);            /* @0x8001dde0-f0 */
    memcpy(b, esp_at(rd32(b, 0x78) + (uint32_t)b[0x1F] * 24u, 24), 24);   /* @0x8001de00-.. */
}

/* ---- O-VB2: FUN_8004fba0-Abbildung (bau_d.md §2.4, Nachbesserung N1 = Maengel M1) -----------
 * RE2 FUN_8004fba0(P, r, mask, a3): Rueckgabe = DAT_800C3B7C (s16, `lh v0,15228(v0)` @0x800500d8),
 * Kontakt = DAT_800DCBC8 (Op 28/29 werten ihn als != 0: `beq v0,zero` @0x8001fce4 / @0x8001fe84).
 *   Grundwerte: Rueckgabe := 0 (`sh zero,15228(at)` @0x8004fc3c), Kontakt := (P.y > 0)
 *     (`sw zero,-13368(at)` @0x8004fc34, `blez v1` / `sw v0(=1)` @0x8004fc48-58).
 *   Objekte (nur a3 == 0, `bne s0,zero,0x800500c8` @0x8004fc5c): XZ-Kasten FUN_80038950(P,obj,r,0)
 *     (a3 = 0 -> KEIN Hoehen-/Bandtest, `beq a3,zero,0x800389e0` @0x800389a4); oben = Mitte - Halbhoehe
 *     (`lhu a2,22(s1)` / `lw a0,0(s1)` / `subu v1,a0,a2` @0x8004fcbc-c8), unten = Mitte + Halbhoehe
 *     (`addu v0,a0,a2` @0x8004fcd4). P.y <= oben -> Kandidat oben (`slt v0,v1,a1` / `bne` @0x8004fccc-d0,
 *     `j 0x8004fd38 / addu a0,v1,zero` @0x8004fcd8-dc); P.y > unten -> nichts (`slt v0,v0,a1 / bne`
 *     @0x8004fce0-e4); sonst Kontakt |= 1 (`ori v0,v0,0x1` @0x8004fd0c) und Kandidat P.y - 1 (`addiu a0,v0,-1`
 *     @0x8004fd34). Rueckgabe = min(Rueckgabe, Kandidat) (`slt v0,a0,v0` / `sh a0,15228(at)` @0x8004fd44-54).
 *   Formen (@0x8004fd74-0x800500cc): XZ-Rechteck um r ERWEITERT (`addu s6,s6,s4` @0x8004fbf8, `sll a0,s4,1`
 *     / `addu v0,v0,a0` / `sltu` @0x8004fd84-98), unten s1 = -1800 * Bitindex(+12) (@0x8004fde4-e04),
 *     oben s0 = -1800 * ((+10 >> 6) & 0x1f) (@0x8004fe08-30); dann
 *       P.y > unten          -> nur Buchhaltung 0x800D5BE4 (`slt v0,s1,v1 / bne` @0x8004ffdc-e0),
 *       oben < P.y <= unten  -> Kontakt |= 1 (`slt v0,s0,v1` @0x8004ffe4, `ori v0,v0,0x1` @0x80050000),
 *       P.y <= oben          -> Rueckgabe = min(Rueckgabe, oben) (`slt v0,s0,v0` / `sh s0,15228(at)`
 *                               @0x80050034-44); P.y < oben -> fertig (`slt v0,v0,s0 / bne` @0x80050050-54),
 *     und fuer "innen" wie fuer P.y == oben: P.y < unten -> Kontakt |= 2 (`slt v0,v0,s1` / `ori v0,v0,0x2`
 *     @0x80050064-80).
 * PORT-ZUORDNUNG auf die RE1.5-Raumdaten (Dossier bau_d.md §N1):
 *   Form   <-> SCA-Zelle des Bandes b, gefiltert wie FUN_8001c6e8 mit den Argumenten der RE1.5-ESP-
 *              Routine 12 (Blutstropfen-Aufprall = RE1.5-Gegenstueck "Boden unter einem Effekt-Teilchen"):
 *              Maske 0x100 (`ori a3,zero,0x100` @0x800177d0), Baender 7..0 (`ori a2,zero,0x8` @0x800177c4),
 *              Bandfilter (`andi v0,v0,0xf002` / `bne s3,v0` @0x8001c89c-a0).
 *              oben  = -1800*(b+1) = Rueckgabe von FUN_8001c6e8 fuer Band b (@0x8001c868-88c),
 *              unten = -1800*b = Standhoehe des Bandes (Spieler-y := -1800 * +0x82 @0x8001d7b8-cc).
 *              Rechteck: FUN_8001c6e8 SCHRUMPFT um sein r (`addu v0,v0,s6` / `subu a0,a0,a3` @0x8001c8c8-d0),
 *              RE2 ERWEITERT um r -> Zellscan mit -r (gleicher Ausdruck (u32)(P.x + r - x) < (u32)(w + 2r)).
 *              Nur der ZELLEN-Teil von FUN_8001c6e8 laeuft (zelle_im_band); dessen Objekt-Stufe nicht
 *              (sie gaebe vor den Zellen des Bandes eine Objekt-Oberkante zurueck, @0x8001c810-840) — die
 *              Objekte laufen nach der RE2-Regel oben. (Maske | 0x10000 statt eigenem Scan waere FALSCH:
 *              FUN_8001c6e8 erweitert das Zellwort vorzeichenrichtig `lh v0,6(a1)` @0x8001c8a8, bei u0-Bit 7
 *              traefe dann Bit 0x10000.)
 *   Objekt <-> aktives Obj_model_set-Prop: XZ = FUN_8002da4c-Zwilling mit Rand r und dem EIGENEN Band des
 *              Props (RE2 hat kein Band-Tor, s.o.); oben = y - 2*hy (FUN_8001c6e8 `lhu v1,8(v0)` / `lhu v0,56(s0)`
 *              / `sll v1,v1,1` / `subu` @0x8001c828-834), unten = y (Standflaeche pool+0x38).
 *   Entfaellt: RE2-Formtyp-Tests 1..13 und Oberkanten-Feinstufe (+10 >> 11) * 100 (@0x8004ffb0-d0) — die
 *              RE1.5-Zellen sind Rechtecke ganzer Bandhoehe (OFFEN im Dossier); mask (RE2 0x2000 =
 *              Flammen-Klasse) hat kein RE1.5-Gegenstueck, die Zellklasse ist die der Routine 12.
 * Folge (gemessen, Sonde probe_r34_re2fx_raum): eine Flamme am Boden (Op 27, P.y = Q.y > 0) liegt unter
 * jeder Band-0-Zelle -> Rueckgabe 0 wie in RE2 (vorher -1800 -> +0x16 = 0xFFFF -> Op-19-Tor sofort offen). */

/* Zellen-Teil von FUN_8001c6e8 fuer EIN Band b: Quadrant FUN_8003b068 (Versatz 0x80010694 = {0,0,0,0},
 * `jal 0x8003b068` @0x8001c770; `and v0,v0,a3(0x80000000)` / `srl v1,v1,1` / `or` / `srl v0,v0,30`
 * @0x8003b084-a0), Zellen der Quadrantengruppe (@0x8001c778-794, Port: sca_rgn wie re15_collision_room_coll),
 * Filter `andi v0,v0,0xf002 / bne s3,v0` @0x8001c89c-a0 und `lh v0,6(a1) / and v0,v0,fp / beq`
 * @0x8001c8a8-b4, Rechteck `addu v0,v0,s6 / subu v1,v1,v0 / subu a0,a0,a3 / sltu` @0x8001c8bc-fc. */
static int zelle_im_band(int32_t x, int32_t z, int b, int32_t r15, uint32_t maske)
{
    const re15_rdt_t *rdt = &g_room_rdt;
    if (!rdt->sca || rdt->sca_count <= 0) return 0;
    unsigned zb = (unsigned)(z - (int32_t)(int16_t)rdt->ceiling_z) & 0x80000000u;
    unsigned xb = (unsigned)(x - (int32_t)(int16_t)rdt->ceiling_x) & 0x80000000u;
    int q = (int)((zb | (xb >> 1)) >> 30);
    int start = 0;
    for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    const int32_t rr = (int32_t)(int16_t)r15;          /* `sll v1,v1,16 / sra s6,v1,16` @0x8001c760-6c */
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        uint16_t w10 = (uint16_t)((uint16_t)e->u1 | ((uint16_t)e->floor << 8));
        uint16_t w08 = (uint16_t)((uint16_t)e->type | ((uint16_t)e->u0 << 8));
        if (((uint32_t)(int32_t)(int16_t)w10 & 0xf002u) != ((uint32_t)b << 12)) continue;
        if (((uint32_t)(int32_t)(int16_t)w08 & maske) == 0) continue;
        if ((uint32_t)(x - ((int32_t)e->x + rr)) >= (uint32_t)((int32_t)e->width   - 2 * rr)) continue;
        if ((uint32_t)(z - ((int32_t)e->z + rr)) >= (uint32_t)((int32_t)e->density - 2 * rr)) continue;
        return 1;
    }
    return 0;
}

static int32_t re2fx_boden(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{
    if (re2fx_boden_hook) return re2fx_boden_hook(p, r, mask, a3, kontakt);
    (void)mask;
    /* Grundwerte gegen die Bezugsebene s_boden_basis (0 = RE2: @0x8004fc34-58 / @0x8004fc3c). */
    int k = (p[1] > s_boden_basis) ? 1 : 0;            /* @0x8004fc34-58 */
    int32_t f = s_boden_basis;                         /* @0x8004fc3c */
    if (g_room_rdt_ok) {
        if (a3 == 0) {                                 /* Objekt-Schleife @0x8004fc5c-fd68 */
            for (int q = 0; q < (int)g_scd.prop_count && q < RE15_SCD_MAX_PROPS; q++) {
                if (!g_scd.props[q].active) continue;
                if (!re15_collision_prop_box_hit(q, p[0], p[2], r, g_scd.props[q].band)) continue;
                int32_t oben  = (int32_t)g_scd.props[q].y - 2 * (int32_t)(uint16_t)g_scd.props[q].box_hy;
                int32_t unten = (int32_t)g_scd.props[q].y;
                int32_t c;
                if (p[1] <= oben)       c = oben;                     /* @0x8004fccc-dc */
                else if (p[1] <= unten) { k |= 1; c = p[1] - 1; }     /* @0x8004fce0-fd34 */
                else continue;                                        /* @0x8004fce4 */
                if (c < f) f = (int16_t)c;                            /* `sh a0` @0x8004fd44-54 */
            }
        }
        for (int b = 7; b >= 0; b--) {                 /* Formen-Schleife @0x8004fd74-0x800500cc; Baender 7..0 = a2 8 @0x800177c4 */
            const int32_t oben = -1800 * (b + 1), unten = -1800 * b;   /* @0x8001c868-88c / @0x8001d7b8-cc */
            if (!zelle_im_band(p[0], p[2], b, -r, 0x100u)) continue;  /* Maske 0x100 @0x800177d0, r -> -r */
            if (p[1] > unten) continue;                /* @0x8004ffdc-e0 */
            if (p[1] > oben) {
                k |= 1;                                /* @0x8004ffe4-5000c */
            } else {
                if (oben < f) f = oben;                /* @0x80050028-44 */
                if (p[1] < oben) continue;             /* @0x80050048-54 */
            }
            if (p[1] < unten) k |= 2;                  /* @0x8005005c-8c */
        }
    }
    *kontakt = k;
    return f;
}

int32_t re2fx_boden_sonde(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{
    int k = 0;
    int32_t f = re2fx_boden(p, r, mask, a3, &k);
    if (kontakt) *kontakt = k;
    return f;
}
static int32_t wasser(int32_t x, int32_t z)
{ return re2fx_wasser_hook ? re2fx_wasser_hook(x, z) : re15_aot_water_at(x, z); }

/* Op 19 = FUN_8001f2c0: Bodenfeuer brennt (Schaden ueber Op 40, Wachsen/Schrumpfen). */
static void op_40(void);
static void op_19(void)
{
    uint8_t *b = cur();
    if (rds16(b, 0x4A) != 0) {                         /* `lh v0,74(v1)` @0x8001f2d0 */
        if (rd16(b, 0x16) >= 0x10u && rd16(b, 0x04) >= 0x1001u)   /* `sltiu 0x10` @0x8001f2e8, `sltiu 0x1001` @0x8001f2fc */
            op_40();                                   /* `jal 0x80020758` @0x8001f308 */
        b = cur();
        wr16(b, 0x16, rd16(b, 0x16) + 1u);             /* @0x8001f31c-28 (nur in diesem Zweig) */
    }
    switch ((int8_t)b[0x1B]) {                         /* `lb v1,27(a2)` @0x8001f338 */
    case 0:                                            /* @0x8001f37c-84: tot */
        b[0] = 0; wr16(b, 0x18, 0);
        break;
    case 1:                                            /* @0x8001f388-3e4: schrumpfen */
        if (rds16(b, 0x42) == 0) { b[0x1B] = 0; break; }          /* @0x8001f3e8-ec */
        wr16(b, 0x42, (uint32_t)(rds16(b, 0x42) - 1));
        wr16(b, 0x04, (uint32_t)mal_durch_1000(rd16(b, 0x04), 990, 1));   /* 990 @0x8001f3a4-b4 */
        wr16(b, 0x06, (uint32_t)mal_durch_1000(rd16(b, 0x06), 980, 1));   /* 980 @0x8001f3c4-e4 */
        break;
    case 2:                                            /* @0x8001f3f0-4c8: wachsen */
        if (rds16(b, 0x42) == 0) {                     /* @0x8001f3f8 */
            b[0x1B] = 1;                               /* @0x8001f47c-84 */
            int32_t r = (int32_t)rng();
            wr16(b, 0x42, (uint32_t)(r % 11 + 90));    /* 0x2e8ba2e9 >> 1 = /11, `addiu v0,v0,90` @0x8001f488-c8 */
            break;
        }
        wr16(b, 0x42, (uint32_t)(rds16(b, 0x42) - 1));
        wr16(b, 0x04, (uint32_t)mal_durch_1000(rd16(b, 0x04), 1009, 1));  /* 1009 @0x8001f40c-18 */
        wr16(b, 0x06, (uint32_t)mal_durch_1000(rd16(b, 0x06), 1002, 1));  /* 1002 @0x8001f428-44 */
        break;
    case 3:                                            /* @0x8001f4cc-51c: Warten -> Zustand 2 */
        if (rds16(b, 0x42) != 0) { wr16(b, 0x42, (uint32_t)(rds16(b, 0x42) - 1)); break; }
        b[0x1B] = 2;
        { int32_t r = (int32_t)rng(); wr16(b, 0x42, (uint32_t)(smod8(r) + 30)); }   /* `addiu v0,v0,30` @0x8001f518 */
        break;
    default: break;
    }
}

/* Op 25 = FUN_8001fa08: Wassertest der Folgeflamme. */
static void op_25(void)
{
    uint8_t *b = cur();
    int32_t w = wasser(rds16(b, 0x34), rds16(b, 0x38));   /* `jal 0x800527b4` @0x8001fa20 */
    if (w != 0 && w < rds16(b, 0x36)) { b[0] = 0; wr16(b, 0x18, 0); }   /* @0x8001fa2c-58 */
}

/* Op 27 = FUN_8001fa9c: Bodenflamme startet. */
static void op_27(void)
{
    uint8_t *b = cur();
    wr16(b, 0x18, 0xB003u);                            /* `ori v1,zero,0xb003 / sh v1,24(a0)` @0x8001fabc-c0 */
    wr16(b, 0x2A, rd16(b, 0x2A) | 0x20u);              /* `ori v0,v0,0x20 / sh v0,42` @0x8001fac4-cc */
    int32_t r = (int32_t)rng();                        /* @0x8001fac8 */
    b[0x21] = (uint8_t)(r % 3);                        /* 0x55555556 @0x8001faa4-f4 */
    b[0x20] = anim_eintrag(b, b[0x21])[2];             /* @0x8001fb04-1c */
    int32_t P[3] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38) };   /* @0x8001fb2c-50 */
    int kontakt;
    int32_t f = re2fx_boden(P, 2, 8192u, 0, &kontakt); /* `jal 0x8004fba0` @0x8001fb4c (a1 2, a2 8192, a3 0) */
    b = cur();
    wr32(b, 0x14, (uint32_t)f);                        /* `sw v0,20(v1)` @0x8001fb60 (32 Bit!) */
    b[0x00] = 58;                                      /* @0x8001fb64-68 */
    b[0x01] = 28;                                      /* @0x8001fb74-78 */
    b[0x1B] = 2;                                       /* @0x8001fb84-8c */
    r = (int32_t)rng();                                /* @0x8001fb88 */
    wr16(b, 0x42, (uint32_t)(r % 3 + 8));              /* `addiu v0,v0,8 / sh v0,66` @0x8001fbb4-b8 */
}

/* Op 28 = FUN_8001fbd0: Flamme in der Luft (Wasser, Landung, Wand). */
static void op_28(void)
{
    uint8_t *b = cur();
    int32_t w = wasser(rds16(b, 0x34), rds16(b, 0x38));   /* `jal 0x800527b4` @0x8001fbec */
    if (w != 0 && w < rds16(b, 0x36)) { b[0] = 0; wr16(b, 0x18, 0); }   /* @0x8001fbf8-24 */
    if (rds16(b, 0x0C) < 0) { b[0x08] = 0; wr16(b, 0x0C, 0); }         /* `bgez` @0x8001fc3c, @0x8001fc44-48 */
    int32_t P[3] = { rds16(b, 0x34), (int32_t)rds16(b, 0x36) - 900, rds16(b, 0x38) };   /* -900 @0x8001fc6c */
    int kontakt;
    int32_t f = re2fx_boden(P, 2, 8192u, 0, &kontakt); /* @0x8001fc7c */
    b = cur();
    P[1] += 900;                                       /* `addiu v1,v1,900` @0x8001fc8c */
    if (f < P[1]) {                                    /* `slt v1,s0,v1` @0x8001fc94 */
        op_rufen(b[0x02]);                             /* Op[step[2]] @0x8001fcac-c8, dann Ende OHNE +0x14 */
        return;
    }
    if (kontakt) {                                     /* `lw v0,-13368` = 0x800DCBC8 @0x8001fcdc */
        if ((uint32_t)f == rd32(b, 0x14)) op_rufen(b[0x03]);   /* gleicher Boden -> step[3] @0x8001fd14-30 */
        else                              op_rufen(b[0x02]);   /* @0x8001fd08-30 */
        b = cur();
    }
    wr32(b, 0x14, (uint32_t)f);                        /* `sw s0,20(v0)` @0x8001fd44 */
}

/* Op 29 = FUN_8001fd5c: gelandete Flamme gleitet, legt Folgeflammen. */
static void op_29(void)
{
    uint8_t *b = cur();
    int16_t vx = rds16(b, 0x0C);                       /* `lh v0,12(a1)` @0x8001fd6c */
    if (vx <= 0) {                                     /* `bgtz` @0x8001fd74 */
        b[0x08] = 0; wr16(b, 0x0C, 0);                 /* @0x8001fd7c-84 */
    } else if (vx >= 61 && (b[0x02] % 15u) == 0) {     /* `slti v0,v0,61` @0x8001fd78; 0x88888889 @0x8001fd94-b8 */
        /* Skala *0.8: `lhu v0,58 / sll 2 / mult 0x66666667 / mfhi / sra 1 / subu sign` @0x8001fdc0-ec. */
        int32_t s4 = (int32_t)(rd16(b, 0x3A) * 4u);
        int32_t skala = (int32_t)(((int64_t)s4 * 0x66666667LL) >> 33) - (s4 >> 31);
        /* `lui v0,0x504 / or` @0x8001fdf0-f8; a1 = +0x22, a2 = 0x8009DB44, a3 = Platz+0x34 (@0x8001fdd0-dd8). */
        int j = kind_an_lage(0x05040000u | (uint32_t)skala, b + 0x34);
        if (j >= 0 && j < RE2FX_PLAETZE)               /* `andi v1,v0,0xff / sltiu v0,v1,0xff` @0x8001fdfc-e04 (Port: nur echte Plaetze) */
            wr16(s_pool[j].b, 0x4A, 1);                /* `sh v1(=1),-29382(at)` = +0x4A @0x8001fe20 */
    }
    b = cur();
    b[0x02] = (uint8_t)(b[0x02] + 1);                  /* @0x8001fe30-3c */
    int32_t P[3] = { rds16(b, 0x34), (int32_t)rds16(b, 0x36) - 100, rds16(b, 0x38) };   /* -100 @0x8001fe60 */
    int kontakt;
    (void)re2fx_boden(P, 2, 8192u, 0, &kontakt);       /* `jal 0x8004fba0` @0x8001fe70 */
    if (kontakt) op_rufen(cur()[0x03]);                /* Op[step[3]] @0x8001fe84-b4 */
}

/* Op 30 = FUN_8001fecc: Folgeflamme startet (Op 2, dann Zustand/Zaehler nach Sub). */
static void op_30(void)
{
    op_2();                                            /* `jal 0x8001dd2c` @0x8001fed4 */
    uint8_t *b = cur();
    if (b[0x1E] == 4) {                                /* `lbu v1,30(a0)` / `addiu v0,zero,4` @0x8001fee8-f0 */
        b[0x1B] = 2;                                   /* @0x8001fef8-ff00 */
        int32_t r = (int32_t)rng();
        wr16(b, 0x42, (uint32_t)(smod8(r) + 2));       /* `addiu v0,v0,2 / sh v0,66` @0x8001ff28-30 */
    } else {
        b[0x1B] = 3;                                   /* `addiu v0,zero,3` @0x8001fef4 / `sb v0,27` @0x8001ff38 */
        int32_t r = (int32_t)rng();
        wr16(b, 0x42, (uint32_t)((r % 6) * 50 + 700)); /* 0x2aaaaaab, *50, `addiu v1,v1,700` @0x8001ff3c-84 */
    }
}

/* Op 50 = FUN_80021970: Gleiten aus (nach Treffer). */
static void op_50(void)
{
    uint8_t *b = cur();
    b[0x02] = 64;                                      /* @0x80021978-7c */
    b[0x08] = 0; b[0x03] = 0; wr16(b, 0x0C, 0);        /* @0x8002198c-9c */
    wr16(b, 0x12, rd16(b, 0x12) & 0xFFFDu);            /* `andi v0,v0,0xfffd` @0x800219a8 */
}

/* Op 40 = FUN_80020758: Nachbrenner-Treffer ueber den RE2-Applier (re2fx_applier). */
static void op_40(void)
{
    uint8_t *b = cur();
    /* Box: 8 Byte von 0x80010910 frisch je Aufruf (lwl/lwr/swl/swr @0x80020770-8c, Rohworte 0x88c20003
     * 0x98c20000 0x88c40007 0x98c40004 0xaba20023 0xbba20020 0xaba40027 0xbba40024);
     * `read 0x80010910 4 --w 2 --signed` = [-600, 0, 300, 150]. */
    const int16_t box[4] = { -600, 0, 300, 150 };
    const int32_t P[3] = { rds16(b, 0x34), (int32_t)rds16(b, 0x36) - 100, rds16(b, 0x38) };   /* -100 @0x800207a4 */
    int treffer = 0;
    if (re2fx_applier)                                 /* `jal 0x800470c0` @0x800207bc */
        treffer = re2fx_applier(P, rds16(b, 0x22), box, 0x2002000Au);   /* `lui a3,0x2002 / ori a3,a3,0xa` @0x80020794/a0 */
    if (treffer) op_50();                              /* `jal 0x80021970` @0x800207cc */
}

/* Op 46 = FUN_80020b60: Flamme gelandet. */
static void op_46(void)
{
    uint8_t *b = cur();
    b[0x09] = 0; b[0x02] = 0; wr16(b, 0x0E, 0);        /* @0x80020b70-80 */
    b[0x00] = 19;                                      /* @0x80020b84-88 */
    b[0x01] = 29;                                      /* @0x80020b94-98 */
    wr16(b, 0x0C, 180);                                /* `addiu v0,zero,180 / sh v0,12` @0x80020ba4-ac */
    int32_t r = (int32_t)rng();                        /* @0x80020ba8 */
    b[0x08] = (uint8_t)(-10 - (r % 11));               /* 0x2e8ba2e9, `addiu v1,zero,-10 / subu` @0x80020bb0-f4 */
    r = (int32_t)rng();                                /* @0x80020bf0 */
    wr16(b, 0x42, (uint32_t)(smod8(r) + 38));          /* `addiu v0,v0,38` @0x80020c20 */
    wr16(b, 0x12, rd16(b, 0x12) & 0xFFFEu);            /* `andi v1,v1,0xfffe` @0x80020c28-30 */
}

/* Op 64 = FUN_80022728 (Optab [64] @0x8009D968): Landung NACH einem Luft-Wandkontakt. Erreichbar,
 * weil Op 28 bei Kontakt + gleichem Boden Op[step[3]] = Op 50 ruft (@0x8001fd14-30) und Op 50
 * step[2] := 64 setzt (@0x80021978-7c); die naechste Boden-Beruehrung dispatcht Op[step[2]] = 64
 * (@0x8001fcac-c8). (Korrektur zu Saeure-GP OFFEN 3 "nie angesprungen".) */
static void op_64(void)
{
    uint8_t *b = cur();
    uint32_t boden = rd32(b, 0x14);                    /* `lw v1,20(v0)` @0x80022734 */
    b[0x09] = 0; b[0x02] = 0; wr16(b, 0x0E, 0);        /* @0x80022738-48 */
    wr16(b, 0x36, boden);                              /* `sh v1,54(v0)` @0x8002274c */
    b[0x01] = 0;                                       /* `sb zero,1(a0)` @0x80022750 */
    wr16(b, 0x12, rd16(b, 0x12) & 0xFFFEu);            /* `andi v0,v0,0xfffe` @0x80022768-70 */
}

/* Op 58 = FUN_80022254: Flamme in der Luft waechst/schrumpft. */
static void op_58(void)
{
    uint8_t *b = cur();
    switch ((int8_t)b[0x1B]) {                         /* `lb v1,27(a2)` @0x80022264 */
    case 0: b[0] = 0; wr16(b, 0x18, 0); break;         /* @0x800222a0-a8 */
    case 1:                                            /* @0x800222ac-330 */
        if (rds16(b, 0x42) == 0) { b[0x1B] = 0; break; }
        wr16(b, 0x42, (uint32_t)(rds16(b, 0x42) - 1));
        wr16(b, 0x04, (uint32_t)mal_durch_1000(rd16(b, 0x04), 880, 1));   /* 880 @0x800222c8-dc */
        wr16(b, 0x06, (uint32_t)mal_durch_1000(rd16(b, 0x06), 800, 0));   /* 800, OHNE Vorzeichenkorrektur @0x800222e8-328 */
        break;
    case 2:                                            /* @0x80022334-3e4 */
        if (rds16(b, 0x42) == 0) {
            b[0x1B] = 1;                               /* @0x800223b8-c0 */
            int32_t r = (int32_t)rng();
            wr16(b, 0x42, (uint32_t)(r - ((r + (int32_t)((uint32_t)r >> 31)) >> 1 << 1) + 2));   /* r%2 + 2 @0x800223c4-e4 */
            break;
        }
        wr16(b, 0x42, (uint32_t)(rds16(b, 0x42) - 1));
        wr16(b, 0x04, (uint32_t)mal_durch_1000(rd16(b, 0x04), 1010, 1));  /* 1010 @0x80022350-64 */
        wr16(b, 0x06, (uint32_t)mal_durch_1000(rd16(b, 0x06), 1007, 1));  /* 1007 @0x80022370-84 */
        break;
    default: break;
    }
}

/* ---- Aufschlaege ------------------------------------------------------------------------- */
static void se(uint32_t code, const int32_t pos[3]) { if (re2fx_se_hook) re2fx_se_hook(code, pos); }

/* Kind-Spawn mit a3 = Zeiger auf das eigene Abbild ab +0x34 (Saeure) — FUN_8001cbe8 kopiert die 2
 * Worte +0x34..+0x3B nach +0x2C..+0x33 und ueberschreibt dann +0x32 mit der CLUT. */
static int kind_an_lage(uint32_t a0, const uint8_t *quelle34)
{
    int16_t ofs[4] = { rds16(quelle34, 0), rds16(quelle34, 2), rds16(quelle34, 4), rds16(quelle34, 6) };
    return spawn_kern(a0, rds16(cur(), 0x22), re2fx_einheitsmatrix, ofs, 0x4000u);
}

/* Op 49 = FUN_800215c8: SAEURE-Aufschlag (Sprungtabelle 0x80010950 = {0x80021678, 0x800217D4,
 * 0x80021854, 0x80021894, 0x800218F4}, Phase = +0x12 `lhu v1,18(v1)` @0x8002164c, `sltiu 0x5`). */
static void op_49(void)
{
    uint8_t *b = cur();
    const int32_t P[3] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38) };   /* sp+16 @0x80021628-48 */
    unsigned phase = rd16(b, 0x12);
    const int lebens255 = (b[0x0B] == 255);            /* `lbu v1,11(a0) / addiu v0,zero,255` */
    switch (phase) {
    case 0:
        wr16(b, 0x12, 1);                              /* @0x8002168c-90 */
        wr16(b, 0x18, 0x8403u);                        /* `ori v0,zero,0x8403 / sh v0,24` @0x80021694-98 */
        b[0x00] = 0;                                   /* @0x8002169c */
        b[0x01] = 49;                                  /* `sb v0(=49),1(v1)` @0x800216b0 */
        se(0x01130001u, P);                            /* `lui a0,0x113 / ori a0,a0,0x1` @0x80021678-7c, jal @0x800216ac */
        /* E3: die RE2-Treffer `jal 0x800470c0` mit 0x1002000B bei y (@0x800216ec) und y+1800
         * (@0x8002170c-18) laufen im Port NICHT — der Schaden kommt aus dem RE1.5-Resolver (Art 3). */
        b = cur();
        if (b[0x0B] == 255) {                          /* @0x8002172c-34 */
            wr16(b, 0x0E, 240); b[0x08] = 0; wr16(b, 0x0C, 0);   /* @0x8002173c-54 */
            wr16(b, 0x36, rd16(b, 0x36) - 470u);       /* `addiu v0,v0,-470` @0x80021758 */
        } else {
            b[0x08] = (uint8_t)(int8_t)-23;            /* `addiu v0,zero,-23 / sb v0,8(a0)` @0x80021738/64 */
            wr16(b, 0x0C, 0); wr16(b, 0x0E, 240);      /* @0x80021770-78 */
        }
        b[0x09] = 0;                                   /* `sb zero,9(v1)` @0x8002177c */
        kind_an_lage(0x030F2000u, b + 0x34);           /* @0x80021780-a4 */
        kind_an_lage(0x040C2000u, cur() + 0x34);       /* @0x800217a8-c4 */
        kind_an_lage(0x041D1800u, cur() + 0x34);       /* `lui a0,0x41d` @0x800217c8, `ori` @0x800217d0, jal @0x800218e4 */
        break;
    case 1:
        wr16(b, 0x12, 2);                              /* @0x800217dc-e0 */
        if (lebens255) wr16(b, 0x36, rd16(b, 0x36) - 600u);   /* @0x800217f4-800 */
        kind_an_lage(0x031F2000u, cur() + 0x34);       /* `lui a0,0x31f` @0x800217f0, jal @0x8002181c */
        b = cur();
        if (b[0x0B] == 255) wr16(b, 0x36, rd16(b, 0x36) - 470u);   /* @0x80021830-50 */
        break;
    case 2:
        wr16(b, 0x12, 3);                              /* @0x80021864-68 */
        if (lebens255) wr16(b, 0x36, rd16(b, 0x36) - 600u);   /* @0x80021870-84 */
        kind_an_lage(0x03142000u, cur() + 0x34);       /* `lui a0,0x314` @0x80021888-90 */
        break;
    case 3:
        wr16(b, 0x12, 4);                              /* @0x800218a4-a8 */
        if (lebens255) wr16(b, 0x36, rd16(b, 0x36) - 600u);   /* @0x800218b0-c4 */
        kind_an_lage(0x040D2800u, cur() + 0x34);       /* `lui a0,0x40d / ori a0,a0,0x2800` @0x800218c8-cc */
        break;
    case 4:
        if (lebens255) wr16(b, 0x36, rd16(b, 0x36) - 600u);   /* @0x80021908-1c */
        kind_an_lage(0x030F2000u, cur() + 0x34);       /* @0x80021920-3c */
        b = cur();
        b[0x01] = 0; b[0x00] = 0; wr16(b, 0x18, 0);    /* Platz frei @0x80021950-58 */
        break;
    default: break;                                    /* `sltiu v0,v1,0x5 / beq` @0x80021654-58 */
    }
}

/* Eine Bodenflamme 0x0505xxxx (Op 48, @0x800210e0-0x8002122c usw.). */
static void bodenflamme(int16_t gier_zusatz_art, const uint8_t *m4c)
{
    int32_t r = (int32_t)rng();                        /* Skala: 7168 + (rng mod 8)*768 */
    uint32_t skala = (uint32_t)(smod8(r) * 3 * 256 + 7168);   /* `sll 1/addu/sll 8/addiu 7168` @0x80021104-10 */
    uint32_t a0 = 0x05050000u | skala;                 /* `lui v0,0x505 / or` @0x80021114-1c */
    int32_t r2 = (int32_t)rng();
    int32_t g = (int32_t)rd16(cur(), 0x22);            /* `lhu a1,34(t0)` */
    if (gier_zusatz_art == 0)      g += r2 % 40;                    /* 0x66666667 >> 4 @0x80021120-164 */
    else if (gier_zusatz_art == 1) g += r2 % 80 + 400;              /* >> 5, `addiu a1,a1,400` @0x800212b8 */
    else                           g += r2 % 80 - 400;              /* `addiu a1,a1,-400` @0x80021410 */
    int j = spawn_kern(a0, (int16_t)g, m4c, NULL, 0x4000u);         /* a2 = Platz+0x4C, a3 = 0 */
    if (j >= 0 && j < RE2FX_PLAETZE) {                  /* `andi s0,v0,0xff / sltiu v0,s0,0xff` @0x80021174-7c (Port: nur echte Plaetze) */
        uint8_t *c = s_pool[j].b;
        int32_t r3 = (int32_t)rng();
        wr16(c, 0x0C, rd16(c, 0x0C) + (uint32_t)(r3 % 25));        /* 0x51eb851f >> 3 @0x8002118c-e0 */
        int32_t r4 = (int32_t)rng();
        wr16(c, 0x4A, 1);                              /* `sh s1(=1),-29382(at)` @0x8002121c */
        c[0x09] = (uint8_t)(c[0x09] + (uint32_t)smod8(r4));         /* `lbu -29447 / addu / sb` @0x8002120c-2c */
    }
}

/* Op 48 = FUN_80020f3c: BRAND-Aufschlag (Phase +0x12: 0 Aufschlag, 1 Platz frei). */
static void op_48(void)
{
    uint8_t *b = cur();
    unsigned phase = rd16(b, 0x12);                    /* `lhu v1,18(a3)` @0x80020fa0 */
    if (phase == 1) { b[0x01] = 0; b[0x00] = 0; wr16(b, 0x18, 0); return; }   /* @0x800215a4-ac */
    if (phase != 0) return;                            /* @0x80020fbc */
    uint32_t alt60 = rd32(b, 0x60), alt64 = rd32(b, 0x64), alt68 = rd32(b, 0x68);   /* sp+16..24 @0x80020fc4-e4 */
    int32_t x = rds16(b, 0x34), y = rds16(b, 0x36), z = rds16(b, 0x38);            /* @0x80020fe8-f0 */
    wr16(b, 0x18, 0x8000u);                            /* `ori v0,zero,0x8000 / sh v0,24(a3)` @0x80020ff4-f8 */
    b[0x00] = 0;                                       /* @0x80020ffc */
    wr32(b, 0x60, (uint32_t)x);                        /* Translation := Aufschlaglage @0x80021000/14/18 */
    wr16(b, 0x12, 1);                                  /* `sh s1(=1),18(a3)` @0x80021010 */
    wr32(b, 0x64, (uint32_t)y);
    wr32(b, 0x68, (uint32_t)z);
    b[0x01] = 48;                                      /* @0x8002101c */
    {
        int32_t t[3] = { (int32_t)rd32(b, 0x60), (int32_t)rd32(b, 0x64), (int32_t)rd32(b, 0x68) };
        se(0x01120001u, t);                            /* `lui a0,0x112` @0x80020fd4, `ori` @0x80021028, jal @0x8002102c, a1 = Platz+0x60 */
    }
    /* Status-Bit 0x80 wird NACH dem eigenen Schreiben 0x8000 getestet (@0x80021040-4c) -> immer 0:
     * der Zweig 0x800214e8 ist toter Code (Saeure-GP §10). E3: die zwei Treffer 0x0002000A
     * (@0x80021060/@0x8002108c) laufen im Port NICHT (RE1.5-Resolver Art 4). */
    b = cur();
    const uint8_t *m4c = b + 0x4C;                     /* a2 = Platz+0x4C (@0x800210a4-b0) */
    spawn_kern(0x040C2800u, 0, m4c, NULL, 0x4000u);    /* `lui a0,0x40c / ori 0x2800`, a1 = 0, a3 = 0 @0x80021094-b8 */
    spawn_kern(0x041D2700u, 0, cur() + 0x4C, NULL, 0x4000u);   /* @0x800210c0-d8 */
    bodenflamme(0, cur() + 0x4C);                      /* @0x800210e0-0x8002122c */
    bodenflamme(1, cur() + 0x4C);                      /* @0x80021230-0x80021384 */
    bodenflamme(2, cur() + 0x4C);                      /* @0x80021388-0x800214dc */
    b = cur();
    /* Ende @0x80021578-a0: +0x60/+0x64/+0x68 := sp+16/20/24 = ALTE Translation (y + 1800 + 1800 durch
     * @0x80021080 und @0x800210b4 auf der Kopie sp+20). Die Kinder haben ihre Matrix schon kopiert. */
    wr32(b, 0x60, alt60);
    wr32(b, 0x64, alt64 + 1800u + 1800u);
    wr32(b, 0x68, alt68);
}

/* Runde 35 Spur M (Glassplitter der RE2-Raum-ESP room1090), definiert am Dateiende. */
static void op_5(void);
static void op_16(void);
static void op_39(void);
static void op_84(void);

static void op_rufen(unsigned op)
{
    if (op < 96) s_op_zaehler[op]++;
    switch (op) {
    case 0:  op_0();  break;    /* 0x8001dc28 */
    case 1:  op_1();  break;    /* 0x8001dc30 */
    case 2:  op_2();  break;    /* 0x8001dd2c */
    case 5:  op_5();  break;    /* 0x8001df90 (Runde 35 Spur M) */
    case 16: op_16(); break;    /* 0x8001f128 (Runde 35 Spur M) */
    case 19: op_19(); break;    /* 0x8001f2c0 */
    case 25: op_25(); break;    /* 0x8001fa08 */
    case 27: op_27(); break;    /* 0x8001fa9c */
    case 28: op_28(); break;    /* 0x8001fbd0 */
    case 29: op_29(); break;    /* 0x8001fd5c */
    case 30: op_30(); break;    /* 0x8001fecc */
    case 39: op_39(); break;    /* 0x800206fc (Runde 35 Spur M) */
    case 40: op_40(); break;    /* 0x80020758 */
    case 46: op_46(); break;    /* 0x80020b60 */
    case 48: op_48(); break;    /* 0x80020f3c */
    case 49: op_49(); break;    /* 0x800215c8 */
    case 50: op_50(); break;    /* 0x80021970 */
    case 58: op_58(); break;    /* 0x80022254 */
    case 64: op_64(); break;    /* 0x80022728 */
    case 84: op_84(); break;    /* 0x80025348 (Runde 35 Spur M) */
    default: s_op_unbekannt++; break;   /* von keinem Aufschlag-Skript erreicht (bau_d.md §3) */
    }
}

/* ---------------------------------------------------------------------------------------------
 * FUN_8001d68c — ein Platz im Draw-Pass
 * ------------------------------------------------------------------------------------------- */
static void schritt(int mit_op_a)
{
    uint8_t *b = cur();
    if (mit_op_a) op_rufen(b[0x00]);                   /* @0x8001d690-c4 */
    weltlage();                                        /* `jal 0x8001d894` @0x8001d6c8 */
    op_rufen(cur()[0x01]);                             /* Op B @0x8001d6d0-fc */
    b = cur();
    if (rd16(b, 0x18) & 2u) {                          /* Physik @0x8001d70c-798: lokal += vel, DANACH vel += acc */
        int8_t ax = (int8_t)b[0x08], ay = (int8_t)b[0x09], az = (int8_t)b[0x0A];
        wr16(b, 0x24, rd16(b, 0x24) + rd16(b, 0x0C));
        wr16(b, 0x26, rd16(b, 0x26) + rd16(b, 0x0E));
        wr16(b, 0x28, rd16(b, 0x28) + rd16(b, 0x10));
        wr16(b, 0x0C, rd16(b, 0x0C) + (uint32_t)(int32_t)ax);
        wr16(b, 0x0E, rd16(b, 0x0E) + (uint32_t)(int32_t)ay);
        wr16(b, 0x10, rd16(b, 0x10) + (uint32_t)(int32_t)az);
    }
    uint16_t st = rd16(b, 0x18);                       /* neu gelesen @0x8001d7a4 */
    if (st & 1u) {                                     /* Anim @0x8001d7ac-884 */
        if (b[0x20] == 0) {
            b[0x21] = (uint8_t)(b[0x21] + (st & 1u));  /* @0x8001d7c8-d4 */
            const uint8_t *e = anim_eintrag(b, b[0x21]);
            if (e[2] == 0) {
                if (e[0] == 0) { b[0x01] = 0; b[0x00] = 0; wr16(b, 0x18, 0); }   /* ENDE -> frei @0x8001d814-20 */
            } else if (e[2] == 0xFF) {                 /* LOOP @0x8001d824-50 */
                b[0x21] = e[0];
                e = anim_eintrag(b, b[0x21]);
            }
            b[0x20] = e[2];                            /* @0x8001d85c-64 */
        }
        b[0x20] = (uint8_t)(b[0x20] - 1);              /* @0x8001d874-80 */
    }
}

/* ---------------------------------------------------------------------------------------------
 * FUN_8001d300 — die Pumpe (einziger Aufrufer @0x80026980)
 * ------------------------------------------------------------------------------------------- */
static int pause_bank(unsigned bank, int update_pass)
{
    /* Pause-Zweig @0x8001d334-0x8001d510: Update-Pass Baenke 26/28/40/21/22/1, Draw-Pass ohne 1
     * (@0x8001d354-80 bzw. @0x8001d454-78). */
    if (bank == 26 || bank == 28 || bank == 40 || (bank - 21u) < 2u) return 1;
    return update_pass && bank == 1;
}

void re2fx_tick(void)
{
    if (!s_esp) return;
    const int pause = (g_re15_pauseflags & RE15_PAUSE_ACTION) != 0;   /* `lw 0x800CFBDC / lui 0x1000 / and` @0x8001d318-2c */
    /* Update-Pass @0x8001d514-5cc (bzw. Pause @0x8001d334-430). */
    for (s_cur = 0; s_cur < RE2FX_PLAETZE; s_cur++) {
        uint8_t *b = cur();
        if (pause && !pause_bank(b[0x1C], 1)) continue;
        if (!(rd16(b, 0x18) & 0x8000u)) continue;
        unsigned par = b[0x1A];
        if (par != 0 && par < RE2FX_PLAETZE && rd16(s_pool[par].b, 0x18) == 0) {   /* Waisen @0x8001d544-7c */
            b[0x01] = 0; b[0x00] = 0; wr16(b, 0x18, 0);
        }
        op_rufen(cur()[0x00]);                          /* Op A @0x8001d580-a4 */
    }
    /* Draw-Pass @0x8001d5d0-668 (bzw. Pause @0x8001d434-50c). */
    for (s_cur = 0; s_cur < RE2FX_PLAETZE; s_cur++) {
        uint8_t *b = cur();
        if (pause && !pause_bank(b[0x1C], 0)) continue;
        if (rd16(b, 0x18) & 0x4000u) {                /* Befoerderung `sh s2(=0xa003),24(v1)` @0x8001d5f8-608 */
            wr16(b, 0x18, 0xA003u);
            op_rufen(b[0x00]);                         /* Op A einmal @0x8001d604-1c */
        }
        if (rd16(cur(), 0x18) & 0x8000u)               /* @0x8001d630-38 */
            schritt(0);                                /* `jal 0x8001d68c` + `addu a0,zero,zero` @0x8001d644-48 */
    }
}


/* ---------------------------------------------------------------------------------------------
 * Billboards - FUN_80077924 (Schleife) + FUN_80077ed0 (POLY_FT4-Bau), Paketfarbe 0x808080 aus
 * FUN_800783b4 (`lui a0,0x2c80 / ori a0,a0,0x8080` @0x800783cc-d0; FUN_80077ed0 schreibt nur das
 * Code-Byte `sb s2,7(t2)` @0x8007808c).
 * ------------------------------------------------------------------------------------------- */
int re2fx_quads(const re15_camera_view_t *cam, int cx, int cy, int camf,
                int has_region, const int16_t rxs[4], const int16_t rzs[4],
                re2fx_quad_t *out, int max)
{
    if (!s_esp || !cam || !out || max <= 0) return 0;
    int n = 0;
    for (int i = RE2FX_PLAETZE - 1; i >= 0; i--) {     /* s0 = 0x800DBB70 - 124 abwaerts @0x800779e8 */
        const uint8_t *b = s_pool[i].b;
        uint16_t st = rd16(b, 0x18);
        if ((st & 0xA000u) != 0xA000u) continue;        /* `andi v1,s2,0xa000` @0x80077a18-24 */
        int32_t wx = rds16(b, 0x34), wy = rds16(b, 0x36), wz = rds16(b, 0x38);   /* @0x800779ec-a00 */
        if (re15_esp_fx_culled(wx, wz, has_region, rxs, rzs)) continue;         /* `jal 0x8002c820` @0x80077a30 */
        const uint8_t *e = anim_eintrag(b, b[0x21]);    /* +0x70 + Anim*8 @0x80077a04-14 */
        unsigned nprim = e[1];                          /* `lbu s3,1(s1)` @0x80077a20 */
        if (nprim == 0) continue;                       /* @0x80077a40 */
        uint8_t code = (st & 0x1000u) ? 0x2E : 0x2C;   /* @0x80077a44-50 */
        if (st & 0x200u) continue;                      /* Alternativpfad @0x80077a54-60 - von keinem
                                                         * Aufschlag-Skript gesetzt (bau_d.md §4) */
        unsigned size = e[3], cell = e[0];              /* `lbu s6,3(s1)` / `lbu s7,0(s1)` @0x80077a58-5c */
        if (size == 0) continue;
        /* RTPS (byte-true wie pc_draw_effects: view = (rot*world)>>12 + trans). */
        int32_t vx = (int32_t)(((int64_t)cam->rot[0] * wx + (int64_t)cam->rot[1] * wy + (int64_t)cam->rot[2] * wz) >> 12) + cam->trans[0];
        int32_t vy = (int32_t)(((int64_t)cam->rot[3] * wx + (int64_t)cam->rot[4] * wy + (int64_t)cam->rot[5] * wz) >> 12) + cam->trans[1];
        int32_t vz = (int32_t)(((int64_t)cam->rot[6] * wx + (int64_t)cam->rot[7] * wy + (int64_t)cam->rot[8] * wz) >> 12) + cam->trans[2];
        uint32_t sz3 = (uint32_t)(vz < 0 ? 0 : (vz > 0xFFFF ? 0xFFFF : vz));
        if ((sz3 >> 9) == 0) continue;                  /* `sra v0,v1,9 / beq` @0x80077f58-5c */
        int32_t ir1 = vx > 0x7FFF ? 0x7FFF : (vx < -0x8000 ? -0x8000 : vx);
        int32_t ir2 = vy > 0x7FFF ? 0x7FFF : (vy < -0x8000 ? -0x8000 : vy);
        uint32_t nrec = re15_gte_divide((uint32_t)cam->fov_screen_dist, sz3);
        int32_t sx = cx + (int32_t)(((int64_t)ir1 * (int64_t)nrec) >> 16);
        int32_t sy = cy + (int32_t)(((int64_t)ir2 * (int64_t)nrec) >> 16);
        int32_t szk = (int32_t)sz3; if (szk > 32767) szk = 32767;   /* `addiu a0,zero,32767 / sltu` @0x80077f64-74 */
        /* step = size * Skala(+0x3A) * camf / (SZ << 4) (`mult a2,t0` / `mult t0,a0` / `div t1,v0`
         * @0x80077f14-80077fd8); Breite/Hoehe = step * Aspekt X/Y (+0x04/+0x06, @0x80078004-40);
         * Texelschritt = Breite / size (`divu` @0x8007801c, @0x8007805c). */
        int32_t t1 = (int32_t)((uint32_t)size * (uint32_t)rd16(b, 0x3A) * (uint32_t)camf);
        int32_t step = t1 / (szk << 4);
        uint32_t w16 = (uint32_t)step * (uint32_t)rd16(b, 0x04);
        uint32_t h16 = (uint32_t)step * (uint32_t)rd16(b, 0x06);
        uint32_t sxs = w16 / size, sys = h16 / size;
        unsigned usz = size, vsz = size;
        if (sxs > 0x1FFFFu) usz = size - 1;             /* `sltu v0,a0(0x1ffff),t7 / addiu a2,a2,-1` @0x80078070-7c */
        if (sys > 0x1FFFFu) vsz = size - 1;             /* `addiu t5,t5,-256` @0x80078088 */
        uint32_t sxy_x = (uint32_t)(uint16_t)sx << 16, sxy_y = (uint32_t)(uint16_t)sy << 16;
        for (unsigned k = 0; k < nprim && n < max; k++) {
            const uint8_t *uv = esp_at(rd32(b, 0x74) + (cell + k) * 4u, 4);   /* +0x74 + Zelle*4 @0x80077f2c-34 */
            uint32_t x0 = sxy_x + (uint32_t)((int32_t)(int8_t)uv[2] * (int32_t)sxs);   /* `lb t1,2(t4) / mult t1,t7` @0x80078090-ac */
            uint32_t y0 = sxy_y + (uint32_t)((int32_t)(int8_t)uv[3] * (int32_t)sys);   /* `lb t0,3(t4) / mult t0,t6` @0x800780a0-cc */
            re2fx_quad_t *q = &out[n++];
            q->platz = i;
            q->x0 = (int16_t)(x0 >> 16);                 /* `srl a0,a0,16` @0x800780c0 */
            q->x1 = (int16_t)((x0 + w16) >> 16);         /* `addu t1,a0,t3 / srl t1,t1,16` @0x800780b0-b4 */
            q->y0 = (int16_t)(y0 >> 16);                 /* `and v0,v0,v1(0xffff0000)` @0x800780dc */
            q->y1 = (int16_t)((y0 + h16) >> 16);         /* `addu t0,v0,a1 / and` @0x800780d0-d4 */
            q->u0 = uv[0]; q->v0 = uv[1];               /* `lbu a0,0(t4)` / `lbu v1,1(t4)` @0x80078104/14 */
            q->u1 = (uint8_t)(uv[0] + usz);              /* `addu t1,a0,a2` @0x8007810c */
            q->v1 = (uint8_t)(uv[1] + vsz);              /* `addu t0,v1,t5` @0x80078120 */
            q->clut = rd16(b, 0x32); q->tpage = rd16(b, 0x2A);
            q->code = code; q->sz = szk; q->vz = vz;
        }
    }
    return n;
}

/* ---------------------------------------------------------------------------------------------
 * Aufschlag-Einstieg (E8 + O-VB1) — Port-Zuordnung, bau_d.md §1.4 / §3
 * ------------------------------------------------------------------------------------------- */
void re2fx_aufschlag(int re2_art, const int32_t q[3], int16_t gier)
{
    if (!s_esp || !q) return;
    if (re2_art != 1 && re2_art != 2) return;          /* 1 Brand (Op 48), 2 Saeure (Op 49); Explosiv bleibt RE1.5 (E8) */
    /* Matrix M: M.rot = RotY(gier) * B (O-VB1), M.t = Q. RotY = RotMatrixY-Konvention (roty_einheit). */
    int32_t ry[9];
    roty_einheit(gier, ry);
    uint8_t m[32];
    memset(m, 0, sizeof m);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++) {
            int64_t s = (int64_t)ry[r * 3 + 0] * s_gl_basis[0 * 3 + c]
                      + (int64_t)ry[r * 3 + 1] * s_gl_basis[1 * 3 + c]
                      + (int64_t)ry[r * 3 + 2] * s_gl_basis[2 * 3 + c];
            wr16(m, 2 * (r * 3 + c), (uint32_t)(int32_t)(s >> 12));
        }
    wr32(m, 20, (uint32_t)q[0]); wr32(m, 24, (uint32_t)q[1]); wr32(m, 28, (uint32_t)q[2]);
    /* Der Platz ist eine RE2-Runde im Aufschlagbild: FUN_8001bf10 mit 0x020C1000 (Bank 2 Skr. 4 =
     * Brand/Saeure-Runde, `lui a0,0x20c / ori a0,a0,0x1000` @0x80044f9c-a0), a1 = Gier (RE2: +0x76
     * `lh a1,118(s1)` @0x80044fa8; Port: RE1.5 +0x2E), a3 = NULL (Q ist schon die Weltlage). */
    int i = spawn_kern(0x020C1000u, gier, m, NULL, 0xA003u);
    if (i < 0 || i >= RE2FX_PLAETZE) return;           /* Pool voll -> kein Aufschlag */
    uint8_t *b = s_pool[i].b;
    s_pool[i].mtx_src = NULL;                          /* Matrix liegt im Abbild; ohne 0x800 nie gelesen */
    /* Stand nach Op 17 (@0x8001f198-0x8001f284), ohne RE2-Flug: */
    b[0x1B] = (uint8_t)re2_art;                        /* `sb v0,27(v1)` @0x8001f1b8 (explizit, NIE Id-9) */
    b[0x00] = 0;                                       /* Port: kein Op 22 (Rauchspur des Flugs, @0x8001f1c4) */
    b[0x01] = (uint8_t)(47 + re2_art);                 /* Op 15 -> Op[step[2]=47 + Art] (`lb v1,27 / addu / jalr` @0x8001f0e4-104) */
    wr16(b, 0x18, 0xB403u);                            /* `ori v0,zero,0xb403 / sh v0,24` @0x8001f1e4-e8 */
    b[0x21] = 18;                                      /* `addiu v0,zero,18 / sb v0,33` @0x8001f1ec-f0 */
    wr16(b, 0x2A, rd16(b, 0x2A) | 0x20u);              /* @0x8001f1f4-204 */
    b[0x20] = anim_eintrag(b, 18)[2];                  /* @0x8001f208-220 */
    b[0x0B] = 15;                                      /* Brand/Saeure `addiu a2,zero,15` @0x8001f1d4 / `sb a2,11` @0x8001f284 */
}

/* =============================================================================================
 * Runde 35 Spur M — RAUM-ESP + die Ops der Glassplitter (RE2 Leon room1090, Fenster-Ereignis
 * sub15). Dossier analysis/befunde_runde35/M_cut11c0_fenster.md §2.3. Alle Adressen RE2 PSX.EXE.
 * ============================================================================================= */

/* Raum-Registrierung = FUN_8001bca0 mit Registry-Basis 8 (die Raum-Haelfte der 16er-Id-Liste
 * 0x800EAE48; der Kern belegt 0..7, re2fx_register_core). Gleiche Kopf-/Tabellenrechnung wie dort
 * (@0x8001bccc-0x8001bd20). Offsets tragen RE2FX_RAUM_MARKE (Port: zwei Dateien, EIN Offsetraum). */
int re2fx_register_raum(const uint8_t *raw, size_t size)
{
    for (int k = 0; k < 8; k++) {                      /* alte Raum-Banken austragen */
        uint8_t id = s_registry[8 + k];
        if (id != 0xFF && id < 64 && s_bank_off[id] >= 0 && ((uint32_t)s_bank_off[id] & RE2FX_RAUM_MARKE)) {
            s_bank_off[id] = -1; s_tab_off[id] = -1;
        }
        s_registry[8 + k] = 0xFF;
    }
    s_raum = NULL; s_raum_size = 0;
    if (!raw || size < 12) return -1;
    size_t al = (size + 3u) & ~(size_t)3u;
    int32_t ende = (int32_t)al - 4;                    /* LETZTES Wort, wie @0x8001bb54-80 */
    int n = 0;
    for (;;) {
        uint8_t id = raw[n];
        s_registry[8 + n] = id;                        /* `sb v1,0(v0)` @0x8001bcdc */
        if (id == 0xFF) break;                         /* @0x8001bcd8 */
        if (ende - 4 * n < 0) return -2;
        uint32_t off = rd32(raw, ende - 4 * n);        /* `lw v1,0(t2)` @0x8001bce0, rueckwaerts */
        if ((size_t)off + 8 > size || id >= 64) return -3;
        uint32_t w = rd32(raw, (int)off);
        uint32_t tab = off + ((w & 0xffffu) * 2u + (w >> 16) + 2u) * 4u;   /* @0x8001bd00-20 */
        if ((size_t)tab + 16 > size) return -4;
        if (s_bank_off[id] >= 0 && !((uint32_t)s_bank_off[id] & RE2FX_RAUM_MARKE)) return -6;  /* Kern-Id belegt */
        s_bank_off[id] = (int32_t)(off | RE2FX_RAUM_MARKE);
        s_tab_off[id]  = (int32_t)(tab | RE2FX_RAUM_MARKE);
        n++;
        if (n >= 8) break;                             /* `sltiu v0,t0,0x8` @0x8001bd24 / `bne` @0x8001bd28 */
    }
    if (n == 0) return -5;
    s_raum = raw; s_raum_size = size;
    return 0;
}

int re2fx_bank_registriert(unsigned bank)
{
    return bank < 64 && s_bank_off[bank] >= 0 && s_tab_off[bank] >= 0;
}

/* Op 16 = FUN_8001f128: Boden unter dem Platz merken, dann Fall-Op 5. */
static void op_16(void)
{
    uint8_t *b = cur();
    int32_t p[3] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38) };   /* `lh v0,52/54/56` @0x8001f13c-54 */
    int kontakt = 0;
    int32_t f = re2fx_boden(p, 2, 0x2000u, 0, &kontakt);   /* a1 2 / a2 8192 / a3 0, `jal 0x8004fba0` @0x8001f15c */
    b = cur();
    wr32(b, 0x14, (uint32_t)f);                        /* `sw v0,20(v1)` @0x8001f174 */
    b[0x00] = b[0x0B];                                 /* `lbu a0,11 / sb a0,0` @0x8001f170/78 */
    b[0x01] = 5;                                       /* `addiu v0,zero,5 / sb v0,1` @0x8001f184/8c */
}

/* Op 5 = FUN_8001df90: Fall bis Wasser/Boden. */
static void op_5(void)
{
    uint8_t *b = cur();
    int32_t w = wasser(rds16(b, 0x34), rds16(b, 0x38));    /* `jal 0x800527b4` @0x8001dfa8 */
    if (w != 0 && w < (int32_t)rds16(b, 0x36)) {       /* `beq a0,zero` @0x8001dfb4 / `slt` @0x8001dfd0 */
        int16_t ofs[4] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38), rds16(b, 0x3A) };   /* a3 = Platz+52 @0x8001dfe4 */
        (void)re2fx_spawn(0x1A010000u | rd16(b, 0x3A), 0, re2fx_einheitsmatrix, ofs);   /* `lui a0,0x1a01 / or` @0x8001dfec-f4, `jal 0x8001cbe8` @0x8001dff0 */
        b = cur();
        b[0x01] = 0; b[0x00] = 0; wr16(b, 0x18, 0);    /* @0x8001e004-14 */
        if (b[0x1C] == 29) op_rufen(b[0x02]);          /* `addiu v0,zero,29 / bne` @0x8001e01c-20, Op[step+2] */
        return;
    }
    int32_t p[3] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38) };   /* sp+16/20/24 @0x8001e040-64 */
    int kontakt = 0;
    int32_t f = re2fx_boden(p, 2, 0x2000u, 0, &kontakt);   /* `jal 0x8004fba0` @0x8001e060 */
    b = cur();
    if (f < p[1]) { op_rufen(b[0x02]); return; }      /* `slt v1,a0,v1 / beq` @0x8001e070-74 -> Op[+2] */
    if (kontakt == 0) { wr32(b, 0x14, (uint32_t)f); return; }   /* 0x800DCBC8 == 0 -> `sw a0,20(v0)` @0x8001e108 */
    if (f == (int32_t)rd32(b, 0x14)) op_rufen(b[0x03]);   /* `beq a0,v0` @0x8001e0bc -> Op[+3] @0x8001e0d0 */
    else                              op_rufen(b[0x02]);   /* @0x8001e0c4 -> Op[+2] */
}

/* Op 39 = FUN_800206fc: Aufprall — Op A 84, Glitzern Bank 0x14 an der Lage. */
static void op_39(void)
{
    uint8_t *b = cur();
    b[0x00] = 84;                                      /* `addiu v0,zero,84 / sb v0,0` @0x80020700/14 */
    b[0x01] = 0;                                       /* `sb zero,1(v0)` @0x80020724 */
    int16_t ofs[4] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38), rds16(b, 0x3A) };   /* a3 = Platz+52 @0x8002073c */
    (void)re2fx_spawn(0x14000000u | rd16(b, 0x3A), 0, re2fx_einheitsmatrix, ofs);   /* `lui a0,0x1400` @0x8002070c, `jal 0x8001cbe8` @0x80020740 */
}

/* Op 84 = FUN_80025348: Platz frei + zweites Glitzern Bank 0x14. */
static void op_84(void)
{
    uint8_t *b = cur();
    b[0x01] = 0; b[0x00] = 0;                          /* @0x80025360-64 */
    wr16(b, 0x18, 0);                                  /* `sh zero,24(v0)` @0x80025378 */
    int16_t ofs[4] = { rds16(b, 0x34), rds16(b, 0x36), rds16(b, 0x38), rds16(b, 0x3A) };   /* a3 = Platz+52 @0x80025380 */
    (void)re2fx_spawn(0x14000000u | rd16(b, 0x3A), 0, re2fx_einheitsmatrix, ofs);   /* `lui a0,0x1400` @0x8002534c, `jal 0x8001cbe8` @0x80025384 */
}
