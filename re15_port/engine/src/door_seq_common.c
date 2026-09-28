/* door_seq_common.c — RE2-Tuersequenz: Skriptmaschine, Objekte, Matrizen, Blende.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail), Begruendung im Kopf von include/re15_door_seq.h.
 *
 * Die Skriptmaschine ist die normale RE2-SCD-Maschine (Dispatch 0x800a74c8), eingeschraenkt
 * auf die 39 Opcodes, die in den 498 RE2-Tuerskripten vorkommen (03 Abschnitt 6). Semantik je
 * Opcode mit Handler-Adresse und PC-Vorschub aus analysis/tor_1170/04_tuerkatalog.md 1.4; die
 * Ablaufsteuerung (Ebenen, Wenn-Stapel, Gosub) ist die des Katalog-Simulators
 * tools/tor/tuerkatalog.py (VM.step), den der Skeptiker mit einem EIGENEN Simulator ueber alle
 * 146 Varianten nachgerechnet hat (04_tuerkatalog.skeptiker.md). unit_door_seq prueft diese
 * Datei Bild fuer Bild gegen den Simulator (DOOR2E Variante 0/1 und das Tor).
 */
#include "re15_door_seq.h"

#include <stdlib.h>
#include <string.h>

#include "gen/re2_rcossin.inc"   /* re2_rcossin_tbl[4096] @0x800adeac */

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static int16_t  rs16(const uint8_t *p) { return (int16_t)rd16(p); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* ===========================================================================
 * RotMatrix — RE2 libgte @0x8008e1f4, Befehl fuer Befehl.
 *   Winkel: a >= 0 -> Eintrag [a & 0xfff]; a < 0 -> Eintrag [(-a) & 0xfff], sin negiert
 *     (@0x8008e1fc bgez / @0x8008e204 subu / @0x8008e22c subu t3,zero,t8).
 *   Eintrag: unteres Halbwort sin, oberes cos (@0x8008e224/28 sll/sra 16, @0x8008e234 sra 16).
 *   m[0][2] = sy (@0x8008e2c8 sh t6,4(a1)); m[1][2] = -(cy*sx)>>12 (@0x8008e2d0/d4);
 *   m[2][2] = (cy*cx)>>12 (@0x8008e2ec); m[0][0] = (cz*cy)>>12 (@0x8008e364/68);
 *   m[0][1] = -(sz*cy)>>12 (@0x8008e37c..88); t8 = (cz*-sy)>>12;
 *   m[1][0] = (sz*cx)>>12 - (t8*sx)>>12 (@0x8008e3c8/d0); m[2][0] = (sz*sx)>>12 + (t8*cx)>>12
 *   (@0x8008e3f8/e400); t8' = (sz*-sy)>>12; m[1][1] = (cz*cx)>>12 + (t8'*sx)>>12 (@0x8008e440/48);
 *   m[2][1] = (cz*sx)>>12 - (t8'*cx)>>12 (@0x8008e470).
 *   multu auf vorzeichenerweiterten Werten: die unteren 32 Bit sind die des signierten Produkts.
 * ======================================================================== */
static void rcossin(int a, int32_t *s, int32_t *c)
{
    int neg = a < 0;
    uint32_t w = re2_rcossin_tbl[(neg ? -a : a) & 0xfff];
    int32_t sn = (int16_t)(w & 0xffff);
    *s = neg ? -sn : sn;
    *c = (int16_t)(w >> 16);
}

void re15_door_rotmatrix(const uint16_t rot[3], int16_t m[9])
{
    int32_t sx, cx, sy, cy, sz, cz;
    rcossin((int16_t)rot[0], &sx, &cx);     /* lh t7,0(a0): Winkel als s16 */
    rcossin((int16_t)rot[1], &sy, &cy);
    rcossin((int16_t)rot[2], &sz, &cz);
    int32_t nsy = -sy;
    m[2] = (int16_t)sy;
    m[5] = (int16_t)((-(cy * sx)) >> 12);
    m[8] = (int16_t)((cy * cx) >> 12);
    m[0] = (int16_t)((cz * cy) >> 12);
    m[1] = (int16_t)((-(sz * cy)) >> 12);
    int32_t t8 = (cz * nsy) >> 12;
    m[3] = (int16_t)(((sz * cx) >> 12) - ((t8 * sx) >> 12));
    m[6] = (int16_t)(((sz * sx) >> 12) + ((t8 * cx) >> 12));
    int32_t t8b = (sz * nsy) >> 12;
    m[4] = (int16_t)(((cz * cx) >> 12) + ((t8b * sx) >> 12));
    m[7] = (int16_t)(((cz * sx) >> 12) - ((t8b * cx) >> 12));
}

/* Kameramatrix der Tuerszene (0x800dcba8 nach FUN_80076cb0 mit Auge (10000,0,0), Ziel 0). */
static void kamera(re15_door_mat_t *k)
{
    static const int16_t R[9] = { 0, 0, 4096,  0, 4096, 0,  -4096, 0, 0 };
    memcpy(k->m, R, sizeof R);
    k->t[0] = 0; k->t[1] = 0; k->t[2] = RE15_DOOR_KAMERA_X;
}

/* Verkettung (analysis/tor_1170/08_re_zeichnen.md 1.1, FUN_80014234):
 *   i: W = P * R per MulMatrix0 = je Spalte MVMVA sf=1 (Wort 4a49e012, @0x80014518/58/9c),
 *      Ablage aus IR (lm=0: auf -0x8000..0x7fff gesaettigt) als s16 (@0x80014528..30).
 *   k: T = P * pos + P.t per MVMVA sf=1 cv=TR (Wort 4a480012, @0x800145f8); pos = untere 16 Bit
 *      von obj+56/60/64 (@0x800145d8/dc lhu, @0x800145ec lwc2); Ablage aus IR (@0x80014600..08),
 *      also ebenfalls auf +-0x7fff gesaettigt. (TR<<12 + S) >> 12 == TR + (S >> 12). */
static int32_t ir_sat(int32_t v) { return v < -0x8000 ? -0x8000 : (v > 0x7fff ? 0x7fff : v); }

static void verketten(const re15_door_mat_t *a, const int16_t r[9], const int32_t p[3],
                      re15_door_mat_t *out)
{
    re15_door_mat_t o;
    int16_t pv[3] = { (int16_t)p[0], (int16_t)p[1], (int16_t)p[2] };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int32_t s = 0;
            for (int k = 0; k < 3; k++) s += (int32_t)a->m[i * 3 + k] * r[k * 3 + j];
            o.m[i * 3 + j] = (int16_t)ir_sat(s >> 12);
        }
        int32_t t = 0;
        for (int k = 0; k < 3; k++) t += (int32_t)a->m[i * 3 + k] * pv[k];
        o.t[i] = ir_sat((t >> 12) + a->t[i]);
    }
    *out = o;
}

/* ===========================================================================
 * Start (Door_init FUN_80013c1c)
 * ======================================================================== */
static void ereignis_starten(re15_door_seq_t *s, re15_door_evt_t *e, int skript)
{
    /* FUN_800530ec: PC = Skriptbeginn, Wenn-Stapel und Ebenen der Ebene sub zuruecksetzen. */
    e->pc = (skript >= 0 && skript < s->n_skripte) ? s->skript_off[skript] : 0;
    e->ifsp_sub = e->sub;
    e->ifsp = 0;
    e->aktiv = (skript >= 0 && skript < s->n_skripte);
    e->ifn[0] = -1;
    e->lvl[0] = -1;
}

int re15_door_seq_start(re15_door_seq_t *s, const uint8_t *teil, int teil_groesse,
                        int variante, int var0e, int tuer_nr)
{
    memset(s, 0, sizeof *s);
    if (!teil || teil_groesse < 12) return -1;
    uint32_t md1_rel = rd32(teil + 0), tim_rel = rd32(teil + 4);   /* @0x80013d24..4c */
    if (md1_rel < 10 || tim_rel <= md1_rel || (int)tim_rel >= teil_groesse) return -1;
    s->teil = teil;
    s->teil_groesse = teil_groesse;

    /* SCD ab +8 (@0x80013e10..14 0x801a1008): u16-Tabelle, Anzahl = erster Eintrag / 2. */
    s->scd_groesse = (int)md1_rel - 8;
    s->scd = (uint8_t *)malloc((size_t)s->scd_groesse);
    if (!s->scd) return -1;
    memcpy(s->scd, teil + 8, (size_t)s->scd_groesse);
    uint16_t erster = rd16(s->scd);
    if (erster == 0 || (erster & 1) || erster > s->scd_groesse) { re15_door_seq_ende(s); return -1; }
    s->n_skripte = erster / 2;
    if (s->n_skripte > RE15_DOOR_SKRIPTE_MAX) s->n_skripte = RE15_DOOR_SKRIPTE_MAX;
    for (int i = 0; i < s->n_skripte; i++) s->skript_off[i] = rd16(s->scd + 2 * i);

    s->md1_ok = re15_md1_parse(teil + md1_rel, (int)(tim_rel - md1_rel), &s->md1) == 0;
    s->tim_ok = re15_tim_parse(teil + tim_rel, teil_groesse - (int)tim_rel, &s->tim) == 0;

    /* Variablen (04 1.4 / 03 Abschnitt 4): var 12 = Payload+13 & 0x7f (@0x80013e5c lbu /
     * @0x80013e6c andi 0xff7f / @0x80013e74), var 13 = 1 bis der Ton geladen ist
     * (@0x80013e68), var 14 = Payload+13 & 0x80 (@0x80013e84/8c), var 15 = Payload+12
     * (@0x80013e90/98). */
    s->var[0x0C] = (int16_t)(variante & 0x7f);
    s->var[0x0D] = 1;
    s->var[0x0E] = (int16_t)(var0e & 0x80);
    s->var[0x0F] = (int16_t)tuer_nr;
    s->bild = 0;                 /* @0x80013de0 sh zero,558(a0) */
    s->schliesston = 0;          /* @0x80013e9c sh zero,584(v1) */
    for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) s->obj[i].eltern = -1;
    for (int i = 0; i < RE15_DOOR_PLAETZE; i++) { s->ev[i].work_typ = -1; }
    /* Blende-Kanal ruht, bis ein Skript ihn setzt (Pegel mit Bit 15 = fertig). */
    s->blende_pegel = 0x8000;
    /* Platz 10 startet Skript 0 (@0x80013e24 addu a1,zero,zero / @0x80013e28 jal 0x800530ec). */
    ereignis_starten(s, &s->ev[0], 0);
    return 0;
}

void re15_door_seq_ende(re15_door_seq_t *s)
{
    free(s->scd);
    s->scd = NULL;
    s->scd_groesse = 0;
}

/* ===========================================================================
 * Skriptmaschine
 * Rueckgabe wie die Handler: 1 = weiter im selben Bild, 2 = Bild zu Ende, 0 = Bedingung falsch
 * (@0x800140e8 / @0x800140f0).
 * ======================================================================== */
static re15_door_obj_t *arbeitsobjekt(re15_door_seq_t *s, re15_door_evt_t *e)
{
    /* Work_set Typ 5 = Tuerobjekt (@0x80055904, Sprungtabelle @0x800111f0, Typ 5 ->
     * @0x800559b0 lw v0,19928(at)). */
    if (e->work_typ != 5 || e->work_id < 0 || e->work_id >= RE15_DOOR_OBJEKTE) {
        s->notizen |= RE15_DOOR_NOTIZ_OHNE_WORK;
        return NULL;
    }
    return &s->obj[e->work_id];
}

static int schritt(re15_door_seq_t *s, int platz, re15_door_evt_t *e);

/* FUN_80053f50: Bedingungsblock aus n Bytes, Glieder mit u16-Verknuepfung (0 = und). */
static int bedingung(re15_door_seq_t *s, int platz, re15_door_evt_t *e, int nbytes)
{
    int ende = e->pc + nbytes;
    int r = schritt(s, platz, e);
    for (int g = 0; e->pc != ende && g < 32; g++) {
        uint16_t verkn = rd16(s->scd + e->pc);
        e->pc += 2;
        int v = schritt(s, platz, e);
        r = verkn ? (r | v) : (r & v);
    }
    return r;
}

static int schritt(re15_door_seq_t *s, int platz, re15_door_evt_t *e)
{
    const uint8_t *b = s->scd + e->pc;
    int pc = e->pc;
    int sb = e->sub;
    if (pc < 0 || pc >= s->scd_groesse) { s->notizen |= RE15_DOOR_NOTIZ_FORMAT; e->aktiv = 0; return 2; }

    switch (b[0]) {
    case 0x00:  /* Nop, Handler 0x800537e4, @0x800537ec addiu v0,v0,1 */
        e->pc += 1; return 1;
    case 0x01:  /* Evt_end, Handler 0x800537fc: Ebene 0 -> aus (@0x80053808 sb zero,1(a3)), sonst Ruecksprung (@0x80053850) */
        if (e->sub == 0) { e->aktiv = 0; return 2; }
        e->sub--;
        e->pc = e->ret[e->sub];
        e->ifsp_sub = e->sub;
        e->ifsp = (uint8_t)(e->ifn[e->sub] + 1);
        return 1;
    case 0x02:  /* Evt_next, Handler 0x80053860: Bild beenden (@0x80053874 Rueckgabe 2) */
        e->pc += 1; return 2;
    case 0x03:  /* Evt_chain, Handler 0x80053878: Neustart mit Skript b[3] (@0x80053888) */
        ereignis_starten(s, e, b[3]); return 1;
    case 0x04: {/* Evt_exec, Handler 0x800538a4, @0x800538bc addiu v0,v0,4 */
        e->pc += 4;
        int ziel = b[1];
        if (ziel >= 14) {   /* @0x800531ac sltiu v0,a0,0xe: sonst freien Platz 10..13 suchen */
            ziel = -1;
            for (int c = 0; c < RE15_DOOR_PLAETZE; c++)
                if (!s->ev[c].aktiv) { ziel = RE15_DOOR_ERSTER_PLATZ + c; break; }
            /* alle belegt: die Suche nimmt Platz 13 (@0x800531d0 beq a0,a2(13) steht VOR dem
             * Aktiv-Test, @0x800531f0 springt dorthin zurueck) - Platz 13 startet neu */
            if (ziel < 0) ziel = RE15_DOOR_ERSTER_PLATZ + RE15_DOOR_PLAETZE - 1;
        }
        if (ziel < RE15_DOOR_ERSTER_PLATZ) return 1;
        re15_door_evt_t *n = &s->ev[ziel - RE15_DOOR_ERSTER_PLATZ];
        n->sub = 0;
        memset(n->speed, 0, sizeof n->speed);   /* @0x80053214..28: sechs Worte ab +344 */
        ereignis_starten(s, n, b[3]);
        return 1; }
    case 0x05:  /* Evt_kill, Handler 0x800538dc, @0x80053914 addiu v0,v0,2 */
        if (b[1] >= RE15_DOOR_ERSTER_PLATZ && b[1] < RE15_DOOR_ERSTER_PLATZ + RE15_DOOR_PLAETZE)
            s->ev[b[1] - RE15_DOOR_ERSTER_PLATZ].aktiv = 0;
        e->pc += 2; return 1;
    case 0x06:  /* Ifel_ck, Handler 0x80053924, @0x8005392c addiu a1,a2,4: Sprungziel pc+4+u16@2 merken */
        e->pc += 4;
        e->ifn[sb]++;
        if (e->ifsp < 8) e->ifstk[e->ifsp_sub][e->ifsp] = (uint16_t)(e->pc + rd16(b + 2));
        e->ifsp++;
        return 1;
    case 0x07:  /* Else_ck, Handler 0x80053964, @0x8005397c pc += u16@2 */
        e->ifsp--;
        e->pc = (uint16_t)(pc + rd16(b + 2));
        e->ifn[sb]--;
        return 1;
    case 0x08:  /* Endif, Handler 0x800539a0, @0x800539b8 addiu v0,v0,2 */
        e->ifsp--;
        e->pc += 2;
        e->ifn[sb]--;
        return 1;
    case 0x09:  /* Sleep, Handler 0x800539dc: PC += 1 (@0x800539e4), Zaehler = u16@2 (@0x80053a10) */
        e->pc += 1;
        e->lvl[sb]++;
        e->cnt[sb][e->lvl[sb] & 3] = rd16(b + 2);
        return 1;
    case 0x0A: {/* Sleeping, Handler 0x80053a24: Zaehler-1, Bild beenden; bei 0 weiter (@0x80053a6c +3) */
        int l = e->lvl[sb] & 3;
        e->cnt[sb][l]--;
        if (e->cnt[sb][l] == 0) { e->pc += 3; e->lvl[sb]--; }
        return 2; }
    case 0x0D: {/* For, Handler 0x80053b1c, @0x80053b58 addiu t0,t0,6: s16@2 Blocklaenge, u16@4 Anzahl */
        int16_t blk = rs16(b + 2);
        uint16_t n = rd16(b + 4);
        if (n == 0) { e->pc = (uint16_t)(pc + blk + 6); return 1; }
        e->lvl[sb]++;
        int l = e->lvl[sb] & 3;
        e->cnt[sb][l] = n;
        e->lstart[sb][l] = (uint16_t)(pc + 6);
        e->lend[sb][l] = (uint16_t)(pc + 6 + blk);
        e->lifn[sb][l] = e->ifn[sb];
        e->pc = (uint16_t)(pc + 6);
        return 1; }
    case 0x0E: {/* Next, Handler 0x80053cbc, @0x80053d1c addiu v0,v0,2 */
        int l = e->lvl[sb] & 3;
        e->cnt[sb][l]--;
        if (e->cnt[sb][l] != 0) e->pc = e->lstart[sb][l];
        else { e->pc += 2; e->lvl[sb]--; }
        return 1; }
    case 0x0F: {/* While, Handler 0x80053d3c, @0x80053da0 addiu a1,a1,4 */
        e->lvl[sb]++;
        int l = e->lvl[sb] & 3;
        e->lstart[sb][l] = (uint16_t)pc;
        e->lend[sb][l] = (uint16_t)(pc + 4 + rs16(b + 2));
        e->lifn[sb][l] = e->ifn[sb];
        e->pc = (uint16_t)(pc + 4);
        if (!bedingung(s, platz, e, b[1])) { e->pc = e->lend[sb][l]; e->lvl[sb]--; }
        return 1; }
    case 0x10: {/* Ewhile, Handler 0x80053e0c: zurueck zum While (@0x80053e2c) */
        int l = e->lvl[sb] & 3;
        e->pc = e->lstart[sb][l];
        e->lvl[sb]--;
        return 1; }
    case 0x13: {/* Switch, Handler 0x80054020, @0x80054040 addiu a3,a3,4 */
        e->lvl[sb]++;
        int l = e->lvl[sb] & 3;
        int a3 = pc + 4;
        e->lend[sb][l] = (uint16_t)(a3 + rd16(b + 2));
        e->lifn[sb][l] = e->ifn[sb];
        int16_t v = s->var[b[1]];
        for (int g = 0; g < 64 && a3 + 6 <= s->scd_groesse; g++) {
            uint8_t o2 = s->scd[a3];
            if (o2 == 0x15) { a3 += 2; break; }            /* Default */
            if (o2 == 0x16) { e->lvl[sb]--; a3 += 2; break; } /* Eswitch */
            uint16_t cb = rd16(s->scd + a3 + 2);
            int16_t cv = rs16(s->scd + a3 + 4);
            a3 += 6;
            if (cv == v) break;
            a3 += cb;
        }
        e->pc = (uint16_t)a3;
        return 1; }
    case 0x14:  /* Case, Handler 0x800540f8, @0x80054100 addiu v0,v0,6 (direkt: Durchfall) */
        e->pc += 6; return 1;
    case 0x15:  /* Default, Handler 0x80054110, @0x80054118 addiu v0,v0,2 */
        e->pc += 2; return 1;
    case 0x16:  /* Eswitch, Handler 0x80054128, @0x8005414c addiu v0,v0,2 */
        e->lvl[sb]--; e->pc += 2; return 1;
    case 0x17:  /* Goto, Handler 0x8005415c: pc += s16@4 (@0x80054190), Ebenen aus b[1]/b[2] */
        e->ifn[sb] = (int8_t)b[1];
        e->lvl[sb] = (int8_t)b[2];
        e->ifsp_sub = (uint8_t)sb;
        e->ifsp = (uint8_t)((int8_t)b[1] + 1);
        e->pc = (uint16_t)(pc + rs16(b + 4));
        return 1;
    case 0x18:  /* Gosub, Handler 0x800541a8, @0x800541b4 addiu v1,v1,2 */
        if (e->sub + 1 >= RE15_DOOR_EBENEN || b[1] >= s->n_skripte) { s->notizen |= RE15_DOOR_NOTIZ_FORMAT; e->pc += 2; return 1; }
        e->ret[sb] = (uint16_t)(pc + 2);
        e->sub++;
        e->ifn[e->sub] = -1;
        e->lvl[e->sub] = -1;
        e->ifsp_sub = e->sub;
        e->ifsp = 0;
        e->pc = s->skript_off[b[1]];
        return 1;
    case 0x1A: {/* Break, Handler 0x80054268: ans Blockende (@0x80054290 lw v1,96(v1)) */
        int l = e->lvl[sb] & 3;
        e->pc = e->lend[sb][l];
        e->ifn[sb] = e->lifn[sb][l];
        e->lvl[sb]--;
        return 1; }
    case 0x1D: {/* Work_copy, Handler 0x800542b4, @0x800542c8 addiu a1,a1,4: Variable in die Skriptbytes */
        e->pc += 4;
        uint16_t v = (uint16_t)s->var[b[1]];
        int ziel = e->pc + b[2];
        if (ziel + 1 < s->scd_groesse) {
            if (b[3]) { s->scd[ziel] = (uint8_t)v; s->scd[ziel + 1] = (uint8_t)(v >> 8); }
            else s->scd[ziel] = (uint8_t)v;
        }
        return 1; }
    case 0x23: {/* Cmp, Handler 0x80054474, @0x80054484 addiu v0,v0,6 */
        e->pc += 6;
        int16_t a = s->var[b[2]], w = rs16(b + 4);
        int r;
        switch (b[3]) {
        case 0: r = a == w; break;  case 1: r = a > w; break;   case 2: r = a >= w; break;
        case 3: r = a < w; break;   case 4: r = a <= w; break;  case 5: r = a != w; break;
        case 6: r = (a & w) != 0; break;
        default:                    /* @0x80054498 sltiu v1,a2,7 / @0x800544a8 beq -> @0x80054514 jr ra:
                                     * Rueckgabe = altes v0 = Variablennummer << 1 (@0x8005448c/90);
                                     * der Scheduler wertet 1/2/sonst aus */
            return (int)b[2] << 1;
        }
        return r ? 1 : 0; }
    case 0x24:  /* Save, Handler 0x8005451c, @0x8005452c addiu v0,v0,4 */
        e->pc += 4; s->var[b[1]] = rs16(b + 2); return 1;
    case 0x25:  /* Copy, Handler 0x8005454c, @0x8005455c addiu v0,v0,3 */
        e->pc += 3; s->var[b[1]] = s->var[b[2]]; return 1;
    case 0x26: {/* Calc, Handler 0x8005458c, @0x800545a4 addiu v0,v0,6 */
        e->pc += 6;
        int32_t a = s->var[b[3]], w = rs16(b + 4), ua = (uint16_t)a, r;
        switch (b[2]) {
        case 0: r = ua + w; break;          case 1: r = ua - w; break;
        case 2: r = a * w; break;           case 3: r = w ? a / w : 0; break;
        case 4: r = w ? a % w : 0; break;   case 5: r = ua | w; break;
        case 6: r = ua & w; break;          case 7: r = ua ^ w; break;
        case 8: r = ~ua; break;             case 9: r = ua << (w & 31); break;
        case 10: r = (int32_t)((uint32_t)ua >> (w & 31)); break;
        case 11: r = a >> (w & 31); break;          /* lh + srav @0x80054748/50 */
        default: return 1;                          /* @0x80054634 sltiu v0,a0,0xc -> @0x80054758 jr ra, kein Store */
        }
        s->var[b[3]] = (int16_t)r;
        return 1; }
    case 0x2E:  /* Work_set, Handler 0x80055904, @0x80055928 addiu v0,v0,3: 12 Werte loeschen */
        e->pc += 3;
        memset(e->speed, 0, sizeof e->speed);
        e->work_typ = (int8_t)b[1];
        e->work_id = (int8_t)b[2];
        return 1;
    case 0x2F:  /* Speed_set, Handler 0x80055a84, @0x80055a94 addiu v0,v0,4; @0x80055aac sh a1,344(v1) */
        e->pc += 4;
        if (b[1] < 12) e->speed[b[1]] = rs16(b + 2);
        return 1;
    case 0x30: {/* Add_speed, Handler 0x80055ab0, @0x80055b1c addiu v0,v0,1: Lage +56/60/64, Drehung +116/118/120 */
        e->pc += 1;
        re15_door_obj_t *o = arbeitsobjekt(s, e);
        if (o) for (int a = 0; a < 3; a++) {
            o->pos[a] += e->speed[a];
            o->rot[a] = (uint16_t)(o->rot[a] + e->speed[3 + a]);
        }
        return 1; }
    case 0x31:  /* Add_aspeed, Handler 0x80055b2c, @0x80055b8c addiu v1,v1,1: v[0..5] += v[6..11] */
        e->pc += 1;
        for (int a = 0; a < 6; a++) e->speed[a] = (int16_t)(e->speed[a] + e->speed[6 + a]);
        return 1;
    case 0x34: case 0x35: case 0x3D:
        /* Member_set (0x80055c00, +4) / Member_set2 (0x80055c50, +3) / Member_copy (0x80055e38, +3):
         * in keinem Skript des Tors; nur Vorschub. DOOR10 nutzt Member_set Feld 13 (z). */
        e->pc += (b[0] == 0x34) ? 4 : 3;
        return 1;
    case 0x36:  /* Se_on, Handler 0x80056428, @0x8005653c addiu v1,s0,12 */
        e->pc += 12;
        if (s->n_ton < (int)(sizeof s->ton / sizeof s->ton[0])) {
            re15_door_ton_t *t = &s->ton[s->n_ton++];
            t->vab = b[1]; t->se = rs16(b + 2); t->bezug = rs16(b + 4);
            t->pos[0] = rs16(b + 6); t->pos[1] = rs16(b + 8); t->pos[2] = rs16(b + 10);
        }
        return 1;
    case 0x4D: {/* Door_model_set, Handler 0x80014ba4, @0x80014cac addiu v0,a1,22 (03 Abschnitt 5.1) */
        e->pc += 22;
        if (b[1] < RE15_DOOR_OBJEKTE) {
            re15_door_obj_t *o = &s->obj[b[1]];
            o->b8 = b[2];
            o->bild = b[3];
            o->on = b[4];
            o->mesh = b[5];
            o->flags = rd16(b + 6);
            o->w10 = rs16(b + 8);
            o->pos[0] = rs16(b + 10); o->pos[1] = rs16(b + 12); o->pos[2] = rs16(b + 14);
            o->rot[0] = rd16(b + 16); o->rot[1] = rd16(b + 18); o->rot[2] = rd16(b + 20);
            /* Eltern: Flag 0x10, Nummer = Flags & 0xF (@0x80014c64..8c) */
            o->eltern = (int8_t)((o->flags & 0x10) ? (o->flags & 0xF) : -1);
            /* Schliesston-Merker (@0x80014c90..a8) */
            if (o->flags & 0x800) s->schliesston = 1;
        }
        return 1; }
    case 0x53: {/* Sce_fade_set, Handler 0x80057ef0 (analysis/tor_1170/08_re_blende.md 1.1):
                 * 0x8002c1a0(Kanal|Art<<8, Schritt, Maske, 7) @0x80057f50 -> +2 Schritt @0x8002c1e4,
                 * +4 Art @0x8002c1e8, Maske je Bit 4/2/1 -> 0xff @0x8002c1dc..22c; danach
                 * 0x8002c2b0(Kanal, 0, ...) @0x80057f58 -> Pegel 0 (@0x8002c2d4); der Handler selbst:
                 * Schritt > 0 -> Pegel 0 (@0x80057fa4), Schritt < 0 -> (u16)Schritt + 0x8000
                 * (@0x80057f94..a0), Schritt 0 -> bleibt 0. PC += 6 @0x80057fa8. Kanal immer 0
                 * in allen 55 RE2-Archiven (08_re_blende.md 4e); andere Kanaele: nur Vorschub. */
        int16_t schritt = rs16(b + 4);
        e->pc += 6;
        if (b[1] == 0) {
            s->blende_art = b[2];
            s->blende_maske = b[3];
            s->blende_schritt = schritt;
            s->blende_pegel = (schritt < 0) ? (uint16_t)((uint16_t)schritt + 0x8000u) : 0;
        }
        return 1; }
    case 0x74:  /* Sce_fade_adjust, Handler 0x80057fd8: 0x8002c2b0(pc[1], (s16)pc[2], 0, NULL)
                 * @0x80057ff4..ffc -> Pegel = Wert (@0x8002c2d4 sh a1,0(a0)); PC += 4 @0x80058004 (NICHT 5) */
        e->pc += 4;
        if (b[1] == 0) s->blende_pegel = rd16(b + 2);
        return 1;
    default:
        s->notizen |= RE15_DOOR_NOTIZ_UNBEKANNT;
        e->aktiv = 0;
        return 2;
    }
}

/* Scheduler FUN_80014058: Plaetze 10..13 in dieser Reihenfolge, je Platz Handler aufrufen,
 * solange Rueckgabe 1. Rueckgabe 0 (Bedingung falsch) springt in den Sonst-Zweig des
 * innersten Wenn oder beendet das Bild, wenn keins offen ist. */
static void scheduler(re15_door_seq_t *s)
{
    for (int p = 0; p < RE15_DOOR_PLAETZE; p++) {
        re15_door_evt_t *e = &s->ev[p];
        if (!e->aktiv) continue;
        for (int g = 0; ; g++) {
            if (g > 20000) { s->notizen |= RE15_DOOR_NOTIZ_SCHLEIFE; e->aktiv = 0; break; }
            int r = schritt(s, RE15_DOOR_ERSTER_PLATZ + p, e);
            if (r == 1) continue;
            if (r == 2) break;
            if (e->ifn[e->sub] < 0) break;
            e->ifsp--;
            e->pc = e->ifstk[e->ifsp_sub][e->ifsp & 7];
            e->ifn[e->sub]--;
        }
    }
}

/* Objektmatrizen FUN_80014234: Index aufsteigend, Eltern vor Kind; Wurzel an die Kamera. */
static void matrizen(re15_door_seq_t *s)
{
    re15_door_mat_t kam;
    kamera(&kam);
    for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) {
        re15_door_obj_t *o = &s->obj[i];
        if (!o->on) continue;
        /* Flag 0x400: beim Bildzaehler == Bildnummer das OT-Bit 0x80 umschalten */
        if ((o->flags & 0x400) && s->bild == o->bild) {
            if ((o->flags & 0xC0) == 0xC0) o->flags &= 0xFF7F;
            else o->flags |= 0x80;
        }
        int16_t r[9];
        re15_door_rotmatrix(o->rot, r);
        const re15_door_mat_t *el = &kam;
        /* Eltern+84 steht fest in obj+128 (Door_model_set @0x80014c64..8c) und wird ohne Test
         * des Eltern-on gelesen (@0x800144c4) - ein ausgeschaltetes Elternobjekt liefert seine
         * zuletzt geschriebene Matrix */
        if (o->eltern >= 0 && o->eltern < RE15_DOOR_OBJEKTE) el = &s->obj[o->eltern].welt;
        o->welt_vor = o->welt;
        verketten(el, r, o->pos, &o->welt);
    }
}

int re15_door_seq_bild(re15_door_seq_t *s, int ton_geladen)
{
    s->n_ton = 0;
    if (!s->ev[0].aktiv) { s->ende = 1; return 0; }
    /* var 13 = 0, sobald das Ladebit faellt (@0x80013f54..68) */
    if (ton_geladen) s->var[0x0D] = 0;
    scheduler(s);          /* @0x80013f6c */
    matrizen(s);           /* @0x80013f74 */
    s->bild++;             /* @0x8001401c / @0x80014024 */
    return 1;
}

/* Blenden-Takt 0x8002c378 (08_re_blende.md 2): einmal je Hauptschleifen-Durchlauf, NACH den
 * Tasks und VOR dem Bildwechsel. Bit 15 gesetzt -> aus (@0x8002c3b4 bltz); Helligkeit =
 * Pegel >> 7 (@0x8002c3b8 sra a0,v0,23) & Maske (@0x8002c3cc); DANACH Pegel += Schritt, 16 Bit
 * ohne Saettigung (@0x8002c408..414) - das Kippen von Bit 15 ist das Ende. Rueckgabe: die
 * abzuziehende Helligkeit (Mischart 2 = B - F, @0x8002c24c) oder -1, wenn der Kanal nichts
 * zeichnet. Masken sind in RE2 immer 7 (R,G,B = 0xff), die Rueckgabe ist daher grau. */
int re15_door_seq_blende_takt(re15_door_seq_t *s)
{
    if (s->blende_pegel & 0x8000u) return -1;
    int h = (s->blende_pegel >> 7) & 0xff;
    if (!(s->blende_maske & 7)) h = 0;
    s->blende_pegel = (uint16_t)(s->blende_pegel + (uint16_t)s->blende_schritt);
    return h;
}

/* Door_exit 0x8001417c, sobald der Kanal fertig ist: 0x8002c1a0(0x200, 0, 7, 1) +
 * 0x8002c2b0(0, 0x7fff, 0xffffff, NULL) (@0x800141a4..c4) -> Schritt 0, Pegel 0x7fff:
 * Helligkeit 255 in jedem Bild (08_re_blende.md 4c). */
void re15_door_seq_blende_schwarz(re15_door_seq_t *s)
{
    s->blende_art = 2;
    s->blende_maske = 7;
    s->blende_schritt = 0;
    s->blende_pegel = 0x7fff;
}

int re15_door_seq_blende_fertig(const re15_door_seq_t *s)
{
    return (s->blende_pegel & 0x8000) != 0;
}
