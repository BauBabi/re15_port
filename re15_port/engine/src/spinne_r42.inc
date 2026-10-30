/* ============================ SPINNE 0x25: WAND / ABSATZ / DECKE (Runde 42) ============================
 * Eingebunden von enemy_ai_common.c (nutzt dessen statische Helfer). Selbst disassembliert 2026-10-30 (STAGE2.BIN).
 * Felder ausser +0x5/+0x6/+0x7, die das Original hier fuehrt:
 *   +0x1e2 Richtungszuschlag des Kletter-Tests (0 / 0x800 an der Decke, @0x80111950-70)
 *   +0x1e8/+0x1ea Zellenspanne, +0x1ee Achse (@0x801119f8-ab4), +0x1ec Absatz-Richtung (= +0x90 & 1, @0x80111908)
 *   +0xb0 Versatz waehrend des Absatz-Clips (@0x80112c88 / @0x80112ce0) — ohne Port-Leser (Zeichner OFFEN)
 *   Fusslage je Bein (Teil 4 / 12) = die zuletzt gezeichnete Lage Teil+0x54/+0x5c, die 0x8011388c vergleicht. */
typedef struct {
    int16_t  r1e2, r1e8, r1ea, r1ec, r1ee, rb0;
    int32_t  fuss[2][3];
    uint8_t  fuss_ok[2];
    uint8_t  altpos;          /* +0x40/+0x42/+0x44 := Lage (@0x80112df4-e24): die Klemme sieht keinen Weg */
} re15_spinne42_t;
static re15_spinne42_t s_sp42[RE15_ACTOR_MAX];

int16_t re15_spinne42_feld(int slot, int ofs)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re15_spinne42_t *k = &s_sp42[slot];
    switch (ofs) {
    case 0x1e2: return k->r1e2; case 0x1e8: return k->r1e8; case 0x1ea: return k->r1ea;
    case 0x1ec: return k->r1ec; case 0x1ee: return k->r1ee; case 0xb0:  return k->rb0;
    default: return 0;
    }
}
void re15_spinne42_setzen(int slot, int ofs, int16_t v)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    re15_spinne42_t *k = &s_sp42[slot];
    switch (ofs) {
    case 0x1e2: k->r1e2 = v; break; case 0x1e8: k->r1e8 = v; break; case 0x1ea: k->r1ea = v; break;
    case 0x1ec: k->r1ec = v; break; case 0x1ee: k->r1ee = v; break; case 0xb0:  k->rb0  = v; break;
    default: break;
    }
}
void re15_spinne42_reset(int slot) { if (slot >= 0 && slot < RE15_ACTOR_MAX) memset(&s_sp42[slot], 0, sizeof s_sp42[slot]); }

/* Kastenrecords (12 B: Versatz x/y/z, Radius, Hoehe, Radius-z; `read 0x80118dd4 24 --w 2`): A @0x80118dd4 =
 * *(0x80118e04) (Boden/Decke), C @0x80118dec = *(0x80118e0c) (Wand). +0x78 := Record (`sw v0,120` @0x80113138 /
 * @0x8011343c / @0x8011352c). */
static void re15_spinne42_kasten(re15_actor_t *e, int wand)
{
    static const int16_t A[6] = { 0, -900, 0, 1000, 900, 1000 };    /* @0x80118dd4 */
    static const int16_t C[6] = { 0, -1440, 0, 200, 1440, 200 };    /* @0x80118dec */
    const int16_t *k = wand ? C : A;
    e->hit_offset_x = k[0]; e->hit_offset_y = k[1]; e->hit_offset_z = k[2];
    e->hit_radius_min = (uint16_t)k[3]; e->hit_height = (uint16_t)k[4]; e->hit_radius_max = (uint16_t)k[5];
}

/* Fusslage = Teil 4 + 8*bein in Welt (Kette Aktor-Matrix ∘ Teil 0 ∘ Teil p-1 ∘ Teil p, Skala +0x166 @0x801138cc-904;
 * Teil-Index `addiu v1,688` + 1376*a0 @0x80113908-28 = (4 + 8*a0)*172). */
static int re15_spinne42_fusslage(const re15_actor_t *e, int bein, int32_t out[3])
{
    static const int32_t null3[3] = { 0, 0, 0 };
    if (!re15_enemy_part_point(e, 4 + 8 * bein, null3, out)) return 0;   /* keine Modellbank geladen */
    if (e->render_scale_q12) {                         /* ScaleMatrix(+0x166) @0x80113900 (nur wenn s1 != 0) */
        out[0] = e->x + (((out[0] - e->x) * (int32_t)e->render_scale_q12) >> 12);
        out[2] = e->z + (((out[2] - e->z) * (int32_t)e->render_scale_q12) >> 12);
    }
    return 1;
}
/* Nach der Wandklemme = Zeichenstand des Bilds: Fusslagen merken (Teil+0x54/+0x5c fuers naechste 0x8011388c). */
void re15_spinne42_nach_klemme(re15_actor_t *e)
{
    int slot = (int)(e - g_actors);
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    for (int b = 0; b < 2; b++) s_sp42[slot].fuss_ok[b] = (uint8_t)re15_spinne42_fusslage(e, b, s_sp42[slot].fuss[b]);
    s_sp42[slot].altpos = 0;
}
/* Alte Lage fuer die Klemme: nach dem Absatz-Ende (+0x40/+0x44 := Lage @0x80112df4-e0c) gilt die neue Lage. */
void re15_spinne42_altlage(re15_actor_t *e, int32_t *ox, int32_t *oz)
{
    int slot = (int)(e - g_actors);
    if (slot >= 0 && slot < RE15_ACTOR_MAX && s_sp42[slot].altpos) { *ox = e->x; *oz = e->z; }
}
/* FUN_8011388c(bein, skala): +0x8c = |Fuss neu - Fuss gezeichnet| (x/z; Square0 0x800662e8 + SquareRoot0
 * @0x8011399c-e4, y genullt @0x801139a4); Bein ab (Teil p-1 Bit 0 frei, @0x80113930-3c) -> +0x8c = 0 (@0x80113950). */
static void re15_spinne42_fussweg(re15_actor_t *e, int bein)
{
    int slot = (int)(e - g_actors);
    int p = 4 + 8 * bein;
    const re15_teil_t *t = re15_teile_an(slot) ? re15_teile(slot) : NULL;
    if (t && !(t[p - 1].w & 1)) { e->crow_speed = 0; return; }
    /* PORT: ohne Modellbank (Headless-Harness ohne EMS) gibt es keinen Fuss — das Original hat die Teile immer
     * (0x8011388c liest sie ohne Test). Dann bleibt +0x8c unberuehrt und nur die Raten der Aufrufer wirken. */
    int32_t n[3];
    if (!re15_spinne42_fusslage(e, bein, n) || !s_sp42[slot].fuss_ok[bein]) return;
    int32_t dx = n[0] - s_sp42[slot].fuss[bein][0], dz = n[2] - s_sp42[slot].fuss[bein][2];
    e->crow_speed = (int16_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
}

/* Wand-/Kletter-Test (Kopf von 0x801118a8 @0x801118b8-ab4 bzw. von GRAB-A @0x801120cc-288). Rueckgabe 1 = auf 9/12
 * umgeschaltet. mit_1e2 = 1 fuer 0x801118a8 (nur dort Zuschlag +0x1e2 im Kletter-Test). */
static int re15_spinne42_wand(re15_actor_t *e, int mit_1e2)
{
    int slot = (int)(e - g_actors);
    re15_spinne42_t *k = &s_sp42[slot];
    unsigned c = e->ai_contact;
    if ((c & 3u) && e->grid_id < 2 &&                                  /* +0x90&3 @0x801118d4, grid<2 @0x801118dc */
        ((((int)(c & 0xf0u) << 4) - (int)e->rot_y + 512) & 0xfff) < 1024) {   /* Wand vorn +-512 @0x801118e4-fc */
        k->r1ec = (int16_t)(c & 1u);                                   /* +0x1ec = +0x90&1 @0x80111904-08 */
        e->zg_9f = (int8_t)c;                                          /* +0x9f = +0x90 @0x80111918-20 */
        e->sub_state_1 = 1; e->sub_state_2 = 9; e->sub_state_3 = 0;   /* sh 0x101,4 / sh 9,6 @0x8011192c / @0x80111c48 */
        return 1;
    }
    if (mit_1e2) {
        k->r1e2 = 0;                                                   /* sh zero,482 @0x80111950 */
        if (e->grid_id == 2) k->r1e2 = 0x800;                          /* Decke @0x80111964-70 */
    } else if (e->grid_id >= 2) return 0;                              /* GRAB-A: grid<2 @0x801120d4-d8 */
    if (e->coll_cell < 0 || !g_room_rdt.sca || e->coll_cell >= g_room_rdt.sca_count) return 0;   /* +0x1b4 == 0: das
                                                                        * Original liest dann Adresse 0x0a (Kernel-RAM);
                                                                        * der Port wertet das als "nicht kletterbar" */
    const re15_sca_entry_t *z = &g_room_rdt.sca[e->coll_cell];
    if (!(((uint16_t)z->u1 | ((uint16_t)z->floor << 8)) & 0xcu)) return 0;   /* lhu 10(cell) & 0xc @0x80111988-94 */
    if (((((int)(c & 0xf0u) << 4) - ((int)e->rot_y + (mit_1e2 ? k->r1e2 : 0)) + 512) & 0xfff) >= 1024)
        return 0;                                                      /* @0x801119a0-c4 (GRAB-A @0x80112180-9c) */
    e->zg_9f = (int8_t)c;                                              /* @0x801119cc */
    e->sub_state_1 = 1; e->sub_state_2 = 12; e->sub_state_3 = 0;      /* sh 0x101,4 / sh 0xc,6 @0x801119dc-ec */
    k->r1ee = 0;                                                       /* @0x801119fc */
    k->r1e8 = z->z;                                                    /* lhu 6(cell) @0x80111a14-1c */
    k->r1ea = (int16_t)((int)z->density + (int)z->z);                  /* lhu 2 + lhu 6 @0x80111a34-44 */
    if (c & 0x40u) {                                                   /* +0x90&0x40 @0x80111a54-60 */
        k->r1ee = 1;                                                   /* @0x80111a68 */
        k->r1e8 = z->x;                                                /* lhu 4(cell) @0x80111a80-88 */
        k->r1ea = (int16_t)((int)z->width + (int)z->x);                /* lhu 0 + lhu 4 @0x80111aa0-b4 */
    }
    return 1;
}

/* 0x801118a8 — Begleiter von WALK/CHASE/CREEP/13: Wandtest, sonst der Lauf-Zeitautomat auf +0x7. */
static void re15_spinne42_lauf(re15_actor_t *e)
{
    if (e->grid_id < 3 && re15_spinne42_wand(e, 1)) return;           /* grid>=3 -> @0x80111ab8 (sltiu 3 @0x801118c0) */
    switch (e->sub_state_3) {                                          /* lbu +0x7 @0x80111ac4 */
    case 0:
        e->sub_state_3 = 1;                                            /* @0x80111ae8 */
        e->anim_frame = 0; e->anim_frac = 7;                           /* +0x95 = 0 / +0x8f = 7 @0x80111af8-b0c */
        e->ai_timer = (int16_t)((int)(re15_engine_rand8() & 0x1fu) * (int)e->sub_state_2 + 32);  /* (rng&0x1f)*(+0x6)+32
                                                                        * @0x80111b08-30 */
        if ((e->sub_state_2 >> 1) == 0) e->ai_timer = (int16_t)(e->ai_timer + 80);   /* +0x6>>1 == 0 @0x80111b40-60 */
        /* fallthrough */
    case 1: {
        (void)re15_aspider_anim(e);                                    /* f314(+0x84,+0x16c,0,0x200) @0x80111b78 */
        re15_spinne42_fussweg(e, (e->anim_frame != 0 && e->anim_frame < 9) ? 1 : 0);   /* +0x95 in 1..8 -> a0 = 1
                                                                        * @0x80111b8c-c0, jal 0x8011388c @0x80111bc4 */
        e->crow_speed = (int16_t)(uint8_t)e->crow_speed;               /* lbu/sh +0x8c @0x80111bd8-e0 */
        uint16_t alt = (uint16_t)e->ai_timer;
        e->ai_timer = (int16_t)(alt - 1u);                             /* +0x9c-- @0x80111bf8-c00 */
        if (alt == 0) {
            e->state = 1; e->sub_state_1 = 0;                          /* sh 1,4 @0x80111c10 */
            e->sub_state_2 = 0; e->sub_state_3 = 0;                    /* sh zero,6 @0x80111c20 */
            if ((unsigned)(e->grid_id - 3u) < 2u) e->sub_state_2 = 13; /* grid 3/4 -> 13 @0x80111c38-48 */
        }
        break;
    }
    default: break;
    }
}

/* ACT 9 = 0x80112af8 — ABSATZ hoch/runter (+0x1ec). */
static void re15_spinne42_absatz(re15_actor_t *e)
{
    re15_spinne42_t *k = &s_sp42[e - g_actors];
    switch (e->sub_state_3) {                                          /* lbu +0x7 @0x80112b08 */
    case 0:
        e->sub_state_3 = 1;                                            /* @0x80112b4c */
        e->motion = 3; e->anim_frame = 0; e->anim_frac = 7;            /* +0x94=3 / +0x95=0 / +0x8f=7 @0x80112b58-7c */
        /* fallthrough */
    case 1: {
        (void)re15_aspider_anim(e);                                    /* f314 @0x80112b94 */
        int d = re15_npc_yaw_step(e, ((int)e->zg_9f & 0xf0) << 4, 0x10);   /* lb +0x9f & 0xf0 << 4, 0x10 @0x80112ba8-b4 */
        e->rot_y = (int16_t)((e->rot_y + d) & 0xfff);                  /* sh +0x6a @0x80112bdc */
        if ((int16_t)d == 0) e->sub_state_3 = 2;                       /* @0x80112bd8-f0 */
        break;
    }
    case 2:
        e->sub_state_3 = 3;                                            /* @0x80112bf8 */
        e->motion = 0xf;                                               /* @0x80112c08 */
        if (k->r1ec != 0) e->motion = 0xe;                             /* @0x80112c18-28 */
        e->anim_frame = 0;                                             /* @0x80112c38 */
        e->rot_y = (int16_t)(((uint8_t)e->zg_9f & 0xf0) << 4);         /* lbu +0x9f / sh +0x6a @0x80112c48-58 */
        /* fallthrough */
    case 3:
        if (k->r1ec == 0) {
            if (e->anim_frame == 0x15) { k->rb0 = 0xa8c; e->dog_floor_y = (int16_t)(e->dog_floor_y + 1800); }  /* @0x80112c78-a4 */
        } else if (e->anim_frame == 0x1a) { k->rb0 = 0x904; e->dog_floor_y = (int16_t)(e->dog_floor_y - 1800); } /* @0x80112cd0-fc */
        if (!re15_aspider_anim(e)) break;                              /* f314 @0x80112d14, beq v0 @0x80112d1c */
        e->floor = (uint8_t)(e->floor - 1u);                           /* +0x82 += 255 @0x80112d30-3c */
        e->y += 1800;                                                  /* @0x80112d4c-58 */
        e->crow_speed = 0xbb8;                                         /* +0x8c = 3000 @0x80112d64-68 */
        if (k->r1ec != 0) {                                            /* @0x80112d78-80 */
            e->floor = (uint8_t)(e->floor + 2u);                       /* @0x80112d88-94 */
            e->y -= 3600;                                              /* @0x80112da4-b0 */
            e->crow_speed = (int16_t)(e->crow_speed - 200);            /* @0x80112dc0-cc */
        }
        k->rb0 = 0;                                                    /* sh zero,176 (Delay-Slot) @0x80112ddc */
        re15_dog_advance_ofs(e, 0);                                    /* jal 0x800245d8(0) @0x80112dd8 */
        k->altpos = 1;                                                 /* +0x40/+0x44/+0x42 := Lage @0x80112df4-e24 */
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* sb zero,5 / sh zero,6 @0x80112e34-44 */
        break;
    default: break;
    }
}

/* ACT 10 = 0x80112e58 — AUSWEICHEN (nach Steckenbleiben @0x80111440-70 / nach Treffer @0x80113f18-28). */
static void re15_spinne42_ausweichen(re15_actor_t *e)
{
    switch (e->sub_state_3) {                                          /* @0x80112e6c */
    case 0: {
        e->anim_frame = 0; e->anim_frac = 7; e->sub_state_3 = 1;       /* @0x80112ea8-c8 */
        e->motion = 6;                                                 /* +0x94 = 6 @0x80112edc (Delay-Slot) */
        unsigned r = re15_engine_rand8() & 1u;                         /* rng @0x80112ed8 */
        e->speed_h = (int16_t)(0x32c - 1624 * (int)r);                 /* +0x1e6 = 812 - 1624*(r&1) @0x80112ee0-f10 */
        unsigned r2 = re15_engine_rand8();                             /* rng @0x80112f1c (immer gezogen) */
        if (!(e->aspider_los & 1)) e->speed_h = (int16_t)(e->speed_h + (int)(r2 & 0xffu));   /* @0x80112f30-48 */
        e->crow_speed = (int16_t)((re15_engine_rand8() & 0x3fu) + 200u);   /* rng @0x80112f50, +0x8c @0x80112f58-68 */
    }   /* fallthrough */
    /* fallthrough */
    case 1:
        e->sub_state_3 = (uint8_t)(e->sub_state_3 + re15_aspider_anim(e));   /* f314 / +0x7 += v0 @0x80112f80-a0 */
        re15_dog_advance_ofs(e, e->speed_h);                           /* jal 0x800245d8(+0x1e6) @0x80112fb4 */
        break;
    case 2:
        e->hit_react = 0;                                              /* +0x93 = 0 @0x80112fc4 */
        e->sub_state_1 = 0;                                            /* @0x80112fd4 */
        e->sub_state_2 = (uint8_t)((e->aspider_los & 1) << 2);         /* +0x6 = LOS*4 @0x80112fe4-f4 */
        e->sub_state_3 = 0;                                            /* @0x80113004 */
        e->ai_target_x = 0;                                            /* +0x1dc = 0 @0x80113014 */
        break;
    default: break;
    }
}

/* ACT 12 = 0x801130a4 — FLAECHENWECHSEL, Phasentabelle @0x801001dc (6 Eintraege). */
static void re15_spinne42_wechsel(re15_actor_t *e)
{
    re15_spinne42_t *k = &s_sp42[e - g_actors];
    switch (e->sub_state_3) {
    case 0:                                                            /* @0x801130e4 */
        e->sub_state_3 = 1;
        e->motion = 3; e->anim_frame = 0; e->anim_frac = 7;            /* @0x801130fc-120 */
        re15_spinne42_kasten(e, 1);                                    /* +0x78 = *(0x80118e0c) @0x8011312c-138 */
        /* fallthrough */
    case 1: {                                                          /* @0x8011313c */
        (void)re15_aspider_anim(e);
        int ziel = ((int)e->zg_9f & 0xf0) << 4;                        /* lb +0x9f & 0xf0 << 4 @0x80113174-98 */
        if (e->grid_id == 2) ziel += 2048;                             /* Decke @0x80113168 / @0x80113188 */
        int d = re15_npc_yaw_step(e, ziel, 0x10);                      /* jal 0x8001aa68 @0x8011319c */
        e->rot_y = (int16_t)((e->rot_y + d) & 0xfff);                  /* @0x801131b4-c0 */
        if ((int16_t)d == 0) e->sub_state_3 = 2;                       /* @0x801131c4-dc */
        break;
    }
    case 2:                                                            /* @0x801131e0 */
        e->hit_react |= 3;                                             /* +0x93 |= 3 @0x801131ec-f8 */
        e->sub_state_3 = 3;
        e->motion = 0xd; e->anim_frame = 0; e->anim_frac = 7;          /* @0x80113214-238 */
        e->rot_y = (int16_t)(((uint8_t)e->zg_9f & 0xf0) << 4);         /* lbu +0x9f (ohne Decken-Zuschlag) @0x80113248-58 */
        /* fallthrough */
    case 3:                                                            /* @0x8011325c */
        (void)re15_aspider_anim(e);
        switch (e->grid_id) {                                          /* Tabelle @0x801001f4[grid], grid<5 @0x8011328c */
        case 0: case 1: re15_dog_advance_ofs(e, 0); break;             /* @0x801132b4 */
        case 2:         re15_dog_advance_ofs(e, 0x800); break;         /* @0x801132c4 */
        case 3:                                                        /* @0x801132d4 */
            if (!(e->y < -10799)) e->y = e->y - 100 - (int32_t)(re15_engine_rand8() & 0xfu);   /* slti -10799 @0x801132e8 */
            break;
        case 4:                                                        /* @0x8011331c */
            if (e->y < (int32_t)e->dog_floor_y - 500) e->y = e->y + 100 + (int32_t)(re15_engine_rand8() & 0xfu);  /* @0x80113328-64 */
            break;
        default: break;
        }
        if (e->anim_frame == 0x1f) e->sub_state_3 = 4;                 /* +0x95 == 0x1f @0x80113374-88 */
        break;
    case 4: {                                                          /* @0x8011338c, Tabelle @0x8010020c[grid] */
        int q = ((int)e->rot_y & 0xc00) >> 10;                         /* andi 0xc00 / srl 10 @0x80113474-78 */
        switch (e->grid_id) {
        case 0: case 1: e->grid_id = 3; e->y -= 1488; break;           /* @0x801133c8-f4 */
        case 2:         e->grid_id = 4; e->y += 1488; break;           /* @0x801133f8-424 */
        case 3:                                                        /* @0x80113428 */
            re15_spinne42_kasten(e, 0);                                /* +0x78 = *(0x80118e04) @0x80113430-3c */
            e->grid_id = 2; e->y = -10800;                             /* @0x80113448-5c */
            if (q == 0) e->x -= 1488; else if (q == 1) e->z += 1488;   /* @0x801134c0 / @0x801134d4 */
            else if (q == 2) e->x += 1488; else e->z -= 1488;          /* @0x801134e0 / @0x801134f4 */
            e->dog_atk_cd = 0x3c;                                      /* +0x1d6 = 0x3c @0x8011350c-514 */
            break;
        case 4:                                                        /* @0x80113518 */
            re15_spinne42_kasten(e, 0);                                /* @0x80113520-52c */
            e->grid_id = 5; e->y = e->dog_floor_y;                     /* @0x80113538-554 */
            if (q == 0) e->x += 1488; else if (q == 1) e->z -= 1488;   /* @0x801135b8 / @0x801135cc */
            else if (q == 2) e->x -= 1488; else e->z += 1488;          /* @0x801135d8 / @0x801135ec */
            break;
        default: break;                                                /* grid>=5 -> @0x801135fc */
        }
        e->anim_frac = 0; e->sub_state_3 = 5;                          /* @0x80113608 / @0x80113618 */
        e->rot_z = (int16_t)(e->rot_z - 1024);                         /* +0x6c -= 1024 @0x80113628-34 */
        (void)re15_aspider_anim(e);                                    /* f314 @0x8011364c */
        break;
    }
    case 5:                                                            /* @0x8011365c */
        e->hit_react = 0; e->motion = 0; e->anim_frame = 0;            /* @0x80113668-688 */
        (void)re15_aspider_anim(e);                                    /* @0x801136a0 */
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* Tabelle @0x80100224: 0/1/2/5 -> @0x801136e4 */
        if (e->grid_id == 3 || e->grid_id == 4) e->sub_state_2 = 13;  /* 3/4 -> +0x6 = 13 @0x80113704-720 */
        break;
    default: break;
    }
    (void)k;
}

/* ACT 13 = 0x80113734 — AN DER WAND (grid 3 hoch / 4 runter). */
static void re15_spinne42_wand_lauf(re15_actor_t *e)
{
    e->motion = 2;                                                     /* sb s0(=2),148 im Delay-Slot @0x80113750 */
    re15_spinne42_lauf(e);                                             /* jal 0x801118a8 @0x8011374c */
    if (e->grid_id == 3) {                                             /* @0x80113768 */
        e->y = e->y - 100 - (int32_t)(re15_engine_rand8() & 0xfu);     /* rng @0x80113770, @0x80113784-94 */
        if (e->y < -9000) { e->sub_state_1 = 1; e->sub_state_2 = 12; e->sub_state_3 = 2; }   /* slti -9000 @0x801137ac-d8 */
    }
    if (e->grid_id == 4) {                                             /* @0x801137f0 */
        e->y = e->y + 100 + (int32_t)(re15_engine_rand8() & 0xfu);     /* rng @0x801137f8, @0x8011380c-1c */
        if ((int32_t)e->dog_floor_y - 1800 < e->y) { e->sub_state_1 = 1; e->sub_state_2 = 12; e->sub_state_3 = 2; }  /* @0x8011382c-64 */
    }
}
