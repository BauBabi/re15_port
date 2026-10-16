/*
 * Runde 38 Spur U "wasserzombie" — Zombies im Wasser ziehen sich am Rand hoch (RE1.5, beide KI-Varianten).
 * Belege und Abbildung: include/re15_wasserzombie_r38.h, Dossier analysis/befunde_runde38/U_wasserzombie.md.
 */
#include "re15_wasserzombie_r38.h"

#include "re15_ai_flavor.h"   /* re15_ai_re2_for_type, re15_re2z_owns_type, re15_re2z_exec_only */
#include "re15_enemy_ai.h"    /* re15_ai_set_state_word, re15_actor_clip_len */
#include "re15_skeleton.h"    /* re15_cos_q12 / re15_sin_q12 */
#include "re15_engine.h"      /* g_engine.frame_count (Messschiene) */
#include "re15_room.h"        /* g_current_room_id (Messschiene) */
#include "re15_enemy.h"       /* re15_enemy_find: Lebensdauer der gemeldeten Bank = die der Gegnerbank */
#include <stddef.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/* RE1.5-snd1-Raum-SE (FUN_800453d0-Pfad) — extern wie in enemy_ai_re2_zombie.c (PSX-Link-Sauberkeit). */
extern void re15_audio_room_se(int idx);

/* ---- Messschiene RE15_WASSERZOMBIE_LOG=<datei> (env-gegatet, kein Verhalten) ------------------------------- */
#ifdef RE15_PLATFORM_PC
static FILE *wz_log(void)
{
    static int s_init = 0; static FILE *s_f = NULL;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_WASSERZOMBIE_LOG");
        if (p && *p) s_f = fopen(p, "w");
    }
    return s_f;
}
#define WZ_LOG(e, was) wz_log_zeile((e), (was))
static void wz_log_zeile(const re15_actor_t *e, const char *was)
{
    FILE *f = wz_log();
    if (!f || !e) return;
    fprintf(f, "F%u R%04X slot=%d t=%02x %s st=%d/%d/%d mo=%d af=%d @(%d,%d,%d) b%d f1ba=%d rot=%d kontakt=%02x\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, (int)(e - g_actors), (unsigned)e->type,
            was, (int)e->state, (int)e->sub_state_1, (int)e->sub_state_2, (int)e->motion, (int)e->anim_frame,
            e->x, e->y, e->z, (int)e->floor, (int)e->dog_floor_y, (int)e->rot_y, (unsigned)e->ai_contact);
    fflush(f);
}
#else
#define WZ_LOG(e, was) ((void)0)
#endif

int re15_wasserzombie_typ(uint8_t type)
{
    /* Typ-Tabelle der Wurzel: STAGE2-Rosterscan (sw-Stores auf 0x80072bac+Typ*4) -> 0x801002b8 fuer
     * 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f; 0x1c-0x1f kommen in keinem Sce_em_set vor (Zensus enemy_ai_common.c).
     * Die Aktionsbank (Clip 0x16 = 159 Bilder, identische Wurzelbahn) ist fuer 0x10/0x11/0x12/0x16/0x18 gleich
     * (Dossier Abschnitt 2, gemessen); 0x13 hat eine eigene Wurzel und eine eigene [0x0A]-Zeile. */
    return type == 0x10 || type == 0x11 || type == 0x12 || type == 0x16 || type == 0x18;
}

int re15_wasserzombie_kante_vorn(const re15_actor_t *e)
{
    if (!e) return 0;
    const unsigned c = e->ai_contact;                               /* lbu a0,144(a1)  @0x80101f00 */
    if ((c & 3u) == 0) return 0;                                    /* andi 3 / beq    @0x80101f08-0c */
    const int kante = (int)(c & 0xf0u) << 4;                        /* andi 0xf0 / sll 4 @0x80101f10-18 */
    if ((((kante - (int)e->rot_y) + 0x200) & 0xfff) >= 0x400) return 0;   /* subu/addiu 512/andi/slti 1024 @0x80101f1c-2c */
    return (c & 1u) ? 1 : 0;                                        /* andi 1: 1 -> 0xA01 (Hochziehen) @0x80101f30-44 */
}

/* Messschiene fuer Tests: Lage im Bild des Endes, VOR Koerperstoss/Klemme dieses Bildes (die laufen danach in run_all). */
static struct { int slot; int32_t x, y, z; } s_landung = { -1, 0, 0, 0 };
int re15_wasserzombie_letzte_landung(int32_t *x, int32_t *y, int32_t *z)
{
    if (x) *x = s_landung.x;
    if (y) *y = s_landung.y;
    if (z) *z = s_landung.z;
    return s_landung.slot;
}

/* FUN_8001aa68 (PSX.EXE, selbst disassembliert): rel = (Ziel - Yaw + Rate) & 0xfff; rel < 2*Rate -> 0 (Totband),
 * rel <= 0x800 -> +Rate, sonst -Rate. Schreibt +0x6a NICHT. */
static int wz_8001aa68(const re15_actor_t *e, int ziel, int rate)
{
    const int rel = ((ziel - (int)e->rot_y) + rate) & 0xfff;        /* @0x8001aa74-84 */
    if (rel < (rate << 1)) return 0;                                /* sll/slt/bne     @0x8001aa90-98 */
    return (rel <= 0x800) ? rate : -rate;                           /* sltiu 0x801     @0x8001aaa0-b8 */
}

/* Port-Zwilling der Clip-Ende-Meldung von anim_set (Rueckgabe 1 @0x8001f638): der globale Advancer
 * (player_common.c re15_actors_anim_advance) laeuft VOR der KI und haelt einen Play-once-Clip auf fc-1. */
static int wz_clip_fertig(const re15_actor_t *e)
{
    const int fc = re15_actor_clip_len(e);
    if (fc <= 0) return 1;
    return (e->anim_frame >= fc - 1) ? 1 : 0;
}

void re15_wasserzombie_hochziehen(re15_actor_t *e)
{
    if (!e) return;
    switch (e->sub_state_2) {                                       /* lbu v1,6(a0) @0x8010326c */
    case 0:                                                         /* @0x801032b0-e0 */
        e->sub_state_2 = 1;                                         /* sb v1(=1),6 */
        e->motion = (int16_t)RE15_WZ_CLIP_DREH;                     /* sb v1,148 @0x801032c0 */
        e->anim_frame = 0;                                          /* sb zero,149 @0x801032d0 */
        e->anim_frac = RE15_WZ_FRAC;                                /* sb 7,143 @0x801032e0 */
        e->anim_blend_rate = RE15_WZ_RATE;                          /* a3 = 0x200 @0x801032fc */
        e->anim_flags = (uint16_t)(e->anim_flags & ~0x80u);         /* f314 a2 = 0 @0x801032ec (vorwaerts) */
        /* PORT-EMULATION des anim_set-Wraps (@0x8001f638-3c): Phase 1 ruft f314 in JEDEM Bild — der Port-Advancer
         * bildet diesen Wrap nur ueber das LOOP-Bit ab (Konvention wie Engage/Wander, enemy_ai_common.c). */
        e->anim_flags = (uint16_t)(e->anim_flags | 0x04u);
        WZ_LOG(e, "drehen");
        /* fallthrough @0x801032e4 */
    case 1: {                                                       /* @0x801032e4-354 */
        const int kante = (int)(e->ai_contact & 0xf0u) << 4;        /* lbu 144 / andi 0xf0 / sll 4 @0x8010330c-1c */
        const int d = wz_8001aa68(e, kante, RE15_WZ_DREHSCHRITT);   /* jal 0x8001aa68, a1 = 0x10 @0x80103318 */
        e->rot_y = (int16_t)((int)e->rot_y + d);                    /* addu v1,v0,v1 / sh v1,106 @0x80103334-40 */
        if (d == 0) e->sub_state_2 = 2;                             /* @0x80103344-54 */
        return;
    }
    case 2:                                                         /* @0x80103358-3bc */
        e->sub_state_2 = 3;                                         /* sb 3,6 @0x8010335c */
        e->motion = (int16_t)RE15_WZ_CLIP_KLETTER;                  /* sb 0x16,148 @0x8010336c */
        e->anim_frame = 0;                                          /* sb zero,149 @0x8010337c */
        e->anim_frac = RE15_WZ_FRAC;                                /* sb 7,143 @0x8010338c */
        e->anim_blend_rate = RE15_WZ_RATE;                          /* a3 = 0x200 @0x80103418 */
        e->speed_h = (int16_t)RE15_WZ_TEMPO;                        /* sh 0x474,140 @0x8010339c */
        e->rot_y = (int16_t)((int)(e->ai_contact & 0xf0u) << 4);    /* sh v0,106 @0x801033bc */
        /* Clip 0x16 ist play-once: das Clip-Ende (anim_set-Rueckgabe 1) beendet den Zustand im selben Bild. */
        e->anim_flags = (uint16_t)(e->anim_flags & ~0x84u);
        WZ_LOG(e, "klettern");
        /* fallthrough @0x801033c0 */
    case 3:                                                         /* @0x801033c0-478 */
        if (e->anim_frame == RE15_WZ_BODENBILD) {                   /* lbu 149 / ori 0x46 / bne @0x801033cc-d4 */
            /* +0xb0 = 0x474 @0x801033dc-e0: kein Leser (Kopf). */
            e->dog_floor_y = (int16_t)((int)e->dog_floor_y - RE15_WZ_BAND);   /* lhu 442 / -1800 / sh @0x801033f0-fc */
            WZ_LOG(e, "boden-1800");
        }
        if (wz_clip_fertig(e)) {                                    /* beq v0,zero @0x8010341c */
            e->floor = (uint8_t)(e->floor + 1u);                    /* lbu 130 / +1 / sb @0x80103430-3c */
            e->y -= RE15_WZ_BAND;                                   /* lw 56 / -1800 / sw @0x8010344c-58 */
            re15_ai_set_state_word(e, 0x0001u);                     /* sw 1,4 @0x80103464-68 */
            /* +0xb0 = 0 @0x80103478: kein Leser (Kopf). */
            /* FUN_800245d8(a0 = 0) @0x80103474: RotMatrixY(+0x6a + 0), (+0x8c,0,0) -> X += IR1, Z += IR3. */
            e->x += (int32_t)(((int32_t)re15_cos_q12(e->rot_y) * (int32_t)e->speed_h) >> 12);
            e->z -= (int32_t)(((int32_t)re15_sin_q12(e->rot_y) * (int32_t)e->speed_h) >> 12);
            s_landung.slot = (int)(e - g_actors); s_landung.x = e->x; s_landung.y = e->y; s_landung.z = e->z;
            WZ_LOG(e, "oben");
        }
        return;
    default:
        return;
    }
}

/* ---- RE1.5-Aktionsbank je Typ (nur PC: der Hybrid-Lader in platform/pc/main.c meldet sie) ------------------- */
#define WZ_BANK_MAX 8
#ifdef RE15_PLATFORM_PC
/* puffer = eb->buf der Gegnerbank bei der Meldung: sk.keyframe_data zeigt in diesen Puffer (Hybrid-Lader
 * platform/pc/main.c, `eb->buf = buf` vor der Meldung). Wird die Bank entladen/neu geladen, aendert sich eb->buf und
 * die Kopie gilt nicht mehr, bis der Lader sie neu meldet. */
typedef struct { uint8_t type; const void *puffer; re15_emd_skeleton_t sk; re15_emd_animation_t an; } wz_bank_t;
static wz_bank_t *s_bank[WZ_BANK_MAX];
#endif

void re15_wasserzombie_bank_merken(uint8_t type, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an)
{
#ifdef RE15_PLATFORM_PC
    if (!sk || !an || !re15_wasserzombie_typ(type)) return;
    if (an->clip_count <= (int)RE15_WZ_CLIP_KLETTER) return;      /* keine Kletter-Clip-Zeile -> nicht merken */
    int frei = -1;
    for (int i = 0; i < WZ_BANK_MAX; i++) {
        if (s_bank[i] && s_bank[i]->type == type) { frei = i; break; }
        if (!s_bank[i] && frei < 0) frei = i;
    }
    if (frei < 0) return;
    if (!s_bank[frei]) {
        s_bank[frei] = (wz_bank_t *)malloc(sizeof(wz_bank_t));
        if (!s_bank[frei]) return;
    }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    s_bank[frei]->type = type;
    s_bank[frei]->puffer = eb ? (const void *)eb->buf : NULL;
    s_bank[frei]->sk = *sk;                                         /* keyframe_data zeigt in eb->buf */
    s_bank[frei]->an = *an;
#else
    (void)type; (void)sk; (void)an;
#endif
}

void re15_wasserzombie_bank_vergessen(void)
{
#ifdef RE15_PLATFORM_PC
    for (int i = 0; i < WZ_BANK_MAX; i++) { free(s_bank[i]); s_bank[i] = NULL; }
#endif
}

static int wz_bank_fuer(uint8_t type, const re15_emd_skeleton_t **sk, const re15_emd_animation_t **an)
{
#ifdef RE15_PLATFORM_PC
    for (int i = 0; i < WZ_BANK_MAX; i++)
        if (s_bank[i] && s_bank[i]->type == type) {
            re15_enemy_bank_t *eb = re15_enemy_find(type);
            if (!eb || !eb->ok || (const void *)eb->buf != s_bank[i]->puffer) return 0;   /* entladen/neu geladen */
            if (sk) *sk = &s_bank[i]->sk;
            if (an) *an = &s_bank[i]->an;
            return 1;
        }
#else
    (void)type; (void)sk; (void)an;
#endif
    return 0;
}

/* ---- RE2-KI: Marke "dieser RE2-Zombie fuehrt gerade die RE1.5-Maschine" ------------------------------------- *
 * Eigene Marke statt eines Aktorfelds; gilt nur, solange Typ, Zustand 1 und Sub 0x0A passen (ein Treffer setzt
 * Zustand 2, ein neuer Spawn Zustand 0 -> die Marke faellt beim naechsten Blick). */
typedef struct { uint8_t aktiv; uint8_t type; int16_t boden_vorher; } wz_marke_t;
static wz_marke_t s_marke[RE15_ACTOR_MAX];

void re15_wasserzombie_reset(void)
{
    for (int i = 0; i < RE15_ACTOR_MAX; i++) s_marke[i].aktiv = 0;
    s_landung.slot = -1;
}

static int wz_marke_gilt(int slot, const re15_actor_t *e)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX || !e) return 0;
    return s_marke[slot].aktiv && s_marke[slot].type == e->type && e->active
        && e->state == 1 && e->sub_state_1 == RE15_WZ_SUB;
}

int re15_wasserzombie_bank(const re15_actor_t *e, const re15_emd_skeleton_t **sk, const re15_emd_animation_t **an)
{
    if (!e) return 0;
    const int slot = (int)(e - g_actors);
    if (!wz_marke_gilt(slot, e)) return 0;
    const re15_emd_skeleton_t *s = NULL; const re15_emd_animation_t *a = NULL;
    if (!wz_bank_fuer(e->type, &s, &a)) return 0;
    if (e->motion < 0 || (int)e->motion >= a->clip_count) return 0;
    if (sk) *sk = s;
    if (an) *an = a;
    return 1;
}

int re15_wasserzombie_schatten_y(const re15_actor_t *e, int32_t *y)
{
    if (!e || !y) return 0;
    if (!re15_wasserzombie_typ(e->type)) return 0;
    if (!(e->state == 1 && e->sub_state_1 == RE15_WZ_SUB)) return 0;
    /* RE1.5-KI: Zustand 1 / Sub 0x0A IST das Hochziehen (Grid 0/2). RE2-KI: nur mit der eigenen Marke
     * (das RE2-Gehirn schreibt 0x0A01 in DECISION[1]/[2], dessen Executor bricht im selben Bild ab). */
    if (re15_ai_re2_for_type(e->type) && re15_re2z_owns_type(e->type)
        && !wz_marke_gilt((int)(e - g_actors), e)) return 0;
    *y = (int32_t)e->dog_floor_y;                                   /* lh a1,442(v0) @0x801004fc -> FUN_8001b064 */
    return 1;
}

/* FUN_8001b38c (Zombie-Wurzel `jal` @0x801004c8): Frame-Wort-SE der gerade posierten Bank — im RE2-Zweig laeuft die
 * RE1.5-Wurzel nicht, die RE1.5-Klettermaschine braucht ihn aber (Clip 0x16 Bild 130 traegt Bit 23 = SE 1).
 * Dieselbe Dekodierung wie enemy_ai_common.c re15_enemy_anim_sfx (RE1.5-Zweig: `srl s0,v0,22` @0x8001b3b4). */
static void wz_frame_se(const re15_actor_t *e, const re15_emd_animation_t *an)
{
    if (!e || !an || e->motion < 0 || (int)e->motion >= an->clip_count) return;
    const re15_emd_clip_t *c = &an->clips[e->motion];
    if (c->frame_count <= 0) return;
    const int f = (int)((uint32_t)e->anim_frame % (uint32_t)c->frame_count);   /* a2 = 0: vorwaerts */
    uint32_t sfx = an->frames[c->first_frame + f] >> 22;
    for (int bit = 0; sfx; bit++, sfx >>= 1)
        if (sfx & 1u) re15_audio_room_se(bit);
}

int re15_wasserzombie_re2_dispatch(int slot, re15_actor_t *e)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX || !e) return 0;
    if (s_marke[slot].aktiv && !wz_marke_gilt(slot, e)) {
        /* anderweitig verlassen (Treffer -> HURT, RE2-Partner-Domino 0x901, Tod): zurueck aufs RE2-Rig, ohne
         * Ueberblendung. PORT-WAHL (nur RE2-KI): die Boden-Referenz +0x1ba folgt wieder der Lage — das RE2-Gehirn
         * fuehrt seinen Boden als Lage, solange Zustand 0/1 (re15_re2z_tick: `re2z_ground_y = y`, +0x1C2-Pflege
         * @0x8003EE04-18), und die geteilte Schwerkraft (re15_schwerkraft_8001bd60) liest +0x1ba. Die RE1.5-KI bleibt
         * byte-true: dort setzt nur @0x801033fc +0x1ba (Wortsuche STAGE2.BIN), ein Abbruch nach Bild 0x46 laesst es
         * stehen. */
        /* Nur derselbe Aktor: ein neuer Spawn im Slot steht auf Zustand 0 (Sce_em_set `sw zero,4(s0)` @0x800421e0)
         * und hat seine +0x1ba frisch (@0x80042210). */
        if (s_marke[slot].type == e->type && e->active && e->state != 0)
            e->dog_floor_y = s_marke[slot].boden_vorher;
        s_marke[slot].aktiv = 0;
        e->anim_prev_valid = 0;
        WZ_LOG(e, "abbruch");
    }
    const re15_emd_animation_t *an = NULL;
    if (!s_marke[slot].aktiv) {
        /* EINTRITT: RE2-Gang (Zustand 1 / Sub 1, aufrecht: +0x10E Bit 0 = Kriecher-Tabelle) = das Gegenstueck des
         * RE1.5-ENGAGE; dieselbe Kantenpruefung wie @0x80101f00-44. Ohne gemeldete RE1.5-Bank bleibt es beim RE2-Gang. */
        if (!(e->state == 1 && e->sub_state_1 == 1 && !(e->re2z_f10e & 1u))) return 0;
        if (!re15_wasserzombie_typ(e->type) || !wz_bank_fuer(e->type, NULL, &an)) return 0;
        if (!re15_wasserzombie_kante_vorn(e)) return 0;
        re15_ai_set_state_word(e, 0x0A01u);                         /* sw v0,4(a1) @0x80101f44 */
        s_marke[slot].aktiv = 1;
        s_marke[slot].type = e->type;
        s_marke[slot].boden_vorher = e->dog_floor_y;
        e->anim_prev_valid = 0;                                     /* PORT-WAHL: Rig RE2 -> RE1.5 ohne Ueberblendung */
        WZ_LOG(e, "re2-eintritt");
    } else {
        wz_bank_fuer(e->type, NULL, &an);
    }
    re15_wasserzombie_hochziehen(e);
    wz_frame_se(e, an);
    if (!(e->state == 1 && e->sub_state_1 == RE15_WZ_SUB)) {
        /* Ende (Wort 1 @0x80103464-68): RE2-Executor des neuen Substates im SELBEN Bild (wie der Original-Root,
         * der +0x5 nach der DECISION neu liest @0x801011d0), sonst traegt der Aktor ein Bild lang den RE1.5-Clip
         * 0x16 auf der RE2-Bank. */
        s_marke[slot].aktiv = 0;
        e->anim_prev_valid = 0;                                     /* PORT-WAHL: Rig RE1.5 -> RE2 */
        re15_re2z_exec_only(slot);
        WZ_LOG(e, "re2-uebergabe");
    }
    return 1;
}
