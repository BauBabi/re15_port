import sys, os
root = sys.argv[1]
def rd(p): return open(os.path.join(root, p), encoding='utf-8').read()
def wr(p, s): open(os.path.join(root, p), 'w', encoding='utf-8', newline='').write(s)
def rep(s, old, new, p):
    if old not in s:
        raise SystemExit('NICHT GEFUNDEN in %s:\n%s' % (p, old))
    return s.replace(old, new, 1)

p = 'engine/src/enemy_ai_boss_g5.c'
s = rd(p)
anchor = "static void g5_se(int id)\n{\n    if (s_g5_bank_fn) s_g5_bank_fn(25);\n    if (s_g5_se_fn) s_g5_se_fn(id, 0);\n}\n"
s = rep(s, anchor, anchor + """
/* NB2-HALBIERUNG (TEMPORAER): RE15_NB2_AUS=<Maske> schaltet NB1-Aenderungen einzeln ab. */
int re15_nb2_aus(unsigned bit)
{
    static int m = -1;
    if (m < 0) { const char *v = getenv("RE15_NB2_AUS"); m = v ? (int)strtol(v, 0, 0) : 0; }
    return (m & (int)bit) != 0;
}
""", p)
old = """    if (neu_verankert) return;                         /* M3: kein Ankerschritt im Original (s.o.) */
    if (e->anim_frame == 0) { s_g5_spur_prev_sx = 0; s_g5_spur_prev_sz = 0; }   /* @0x80015f04-1c */"""
new = """    if (re15_nb2_aus(0x10)) {
        if (neu_verankert) { s_g5_spur_prev_sx = sx; s_g5_spur_prev_sz = sz; return; }
    } else {
    if (neu_verankert) return;                         /* M3: kein Ankerschritt im Original (s.o.) */
    if (e->anim_frame == 0) { s_g5_spur_prev_sx = 0; s_g5_spur_prev_sz = 0; }   /* @0x80015f04-1c */
    }"""
s = rep(s, old, new, p)
old = """        {   int32_t y;
            if (s_g5_w1_ok && g5_kf_wurzel_y(e, (int)e->motion, (int)e->anim_frame, &y))
                g->gewicht[1] = (int32_t)(int16_t)(((y - s_g5_w1_basis) * 4415) >> 11);
        }"""
new = """        if (re15_nb2_aus(0x80)) { int fc = g5_clip_len(e); if (fc > 1) g->gewicht[1] = (int32_t)((int64_t)e->anim_frame * 2895 / (fc - 1)); } else
        {   int32_t y;
            if (s_g5_w1_ok && g5_kf_wurzel_y(e, (int)e->motion, (int)e->anim_frame, &y))
                g->gewicht[1] = (int32_t)(int16_t)(((y - s_g5_w1_basis) * 4415) >> 11);
        }"""
s = rep(s, old, new, p)
old = """            int32_t y;
            if (s_g5_w1_ok && g5_kf_wurzel_y(e, (int)e->motion, (int)e->anim_frame, &y))
                g->gewicht[1] = (int32_t)(int16_t)(((y - s_g5_w1_basis) * 8830) >> 12);"""
new = """            int32_t y;
            if (re15_nb2_aus(0x80)) { int fc = g5_clip_len(e); if (fc > 1) g->gewicht[1] = (int32_t)((int64_t)e->anim_frame * 2895 / (fc - 1)); } else
            if (s_g5_w1_ok && g5_kf_wurzel_y(e, (int)e->motion, (int)e->anim_frame, &y))
                g->gewicht[1] = (int32_t)(int16_t)(((y - s_g5_w1_basis) * 8830) >> 12);"""
s = rep(s, old, new, p)
markers = [
    "posiert, +0x158 0 -> 1 (@0x801013e8-f4). */\n        /* fall through */",
    "0x8010173c = [T8] (Tabelle @0x80100034), `jal 0x8002959c` @0x80101748 im selben Takt. */\n        /* fall through */",
    "@0x80101628) laeuft in [T10] 0x8010162c (Tabelle @0x8010003c), `jal 0x8002959c` @0x80101638. */\n        /* fall through */",
    "= [T12] (Tabelle @0x80100044), `jal 0x8002959c` @0x80101748 im selben Takt. */\n        /* fall through */",
    "nach 0x80101798 = [T14] (Tabelle @0x8010004c) zurueck, `jal 0x8002959c` @0x801017a4 im selben Takt. */\n        /* fall through */",
]
for m in markers:
    s = rep(s, m, m.replace("/* fall through */", "if (re15_nb2_aus(0x20)) break;\n        /* fall through */"), p)
old = """        if (g->timer-- == 0) {
            g->sub = 1; g->ph = 0; g5_se(10);"""
new = """        if (re15_nb2_aus(0x20) ? (--g->timer <= 0) : (g->timer-- == 0)) {
            g->sub = 1; g->ph = 0; g5_se(10);"""
s = rep(s, old, new, p)
wr(p, s)

p = 'engine/src/enemy_ai_tentakel_g5.c'
s = rd(p)
s = rep(s, "int re15_g5_tentakel_zeichenbild(const re15_actor_t *e, int *clip, int *bild);\n",
        "int re15_g5_tentakel_zeichenbild(const re15_actor_t *e, int *clip, int *bild);\nint re15_nb2_aus(unsigned bit);\n", p)
s = rep(s, "        if (!re15_g5_tentakel_zeichenbild(e, &zc, &zb)) { zc = -1; zb = (int)e->anim_frame; }",
        "        if (re15_nb2_aus(0x02) || !re15_g5_tentakel_zeichenbild(e, &zc, &zb)) { zc = -1; zb = (int)e->anim_frame; }", p)
s = rep(s, "    if (t) re15_g5_tentakel_stretch_point(p0, t->scale_x, out);",
        "    if (t && !re15_nb2_aus(0x01)) re15_g5_tentakel_stretch_point(p0, t->scale_x, out);", p)
s = rep(s, "    e->anim_blend_rate = (uint16_t)rate;        /* a3 (@0x800296b8) */",
        "    if (!re15_nb2_aus(0x08)) e->anim_blend_rate = (uint16_t)rate;        /* a3 (@0x800296b8) */", p)
s = rep(s, "    e->anim_frac = (uint8_t)blend;\n}",
        "    e->anim_frac = (uint8_t)(re15_nb2_aus(0x08) ? 0 : blend);\n}", p)
old = "             * ph4-Koerper @0x80101E24 (Tabelle @0x8010004C[4]). */\n            /* fall through */"
s = rep(s, old, old.replace("            /* fall through */", "            if (re15_nb2_aus(0x04)) break;\n            /* fall through */"), p)
old = "            /* KEIN Anim-Schritt (Abnahme G5 1, M2): ph4 @0x80101E24-88 ruft 0x8002959c nicht - die Pose bleibt die letzte posierte. */"
s = rep(s, old, old + "\n            if (re15_nb2_aus(0x04)) tent_anim(e, t, 0);", p)
old = "            /* KEIN Anim-Schritt (Abnahme G5 1, M2): ph6 @0x80101E94-F9C ruft 0x8002959c nicht - die Pose bleibt die letzte posierte. */"
s = rep(s, old, old + "\n            if (re15_nb2_aus(0x04)) tent_anim(e, t, 0);", p)
wr(p, s)

p = 'engine/src/player_common.c'
s = rd(p)
s = rep(s, "static int re15_type_self_advances_anim(uint8_t t)\n{\n    switch (t) {",
        "int re15_nb2_aus(unsigned bit);\nstatic int re15_type_self_advances_anim(uint8_t t)\n{\n    if (t == 0x37 && re15_nb2_aus(0x400)) return 0;\n    switch (t) {", p)
wr(p, s)

p = 'engine/src/game_step_common.c'
s = rd(p)
s = rep(s, "void re15_player_skriptlage(int32_t x, int32_t z)\n{\n    s_klemme_ende.x = x; s_klemme_ende.z = z;",
        "int re15_nb2_aus(unsigned bit);\nvoid re15_player_skriptlage(int32_t x, int32_t z)\n{\n    if (re15_nb2_aus(0x40)) return;\n    s_klemme_ende.x = x; s_klemme_ende.z = z;", p)
wr(p, s)

p = 'engine/src/g5auftritt_r41.c'
s = rd(p)
s = rep(s, "void re15_player_skriptlage(int32_t x, int32_t z);            /* game_step_common.c: Pos_set ist keine Bewegung (keine Nachklemme) */\n",
        "void re15_player_skriptlage(int32_t x, int32_t z);            /* game_step_common.c: Pos_set ist keine Bewegung (keine Nachklemme) */\nint re15_nb2_aus(unsigned bit);\n", p)
s = rep(s, "    if (s_g5a.phase != G5A_SZENE) { vorlauf_tick(); return; }",
        "    if (s_g5a.phase != G5A_SZENE && re15_nb2_aus(0x100)) { spieler_setzen(G5A_X_ARMIERUNG, G5A_GIER_WEST); armieren(); return; }\n    if (s_g5a.phase != G5A_SZENE) { vorlauf_tick(); return; }", p)
s = rep(s, "    if (b == G5A_BILD_LAUF)                              /*",
        "    if (b == G5A_BILD_LAUF && !re15_nb2_aus(0x200))      /*", p)
s = rep(s, "            spieler_setzen(G5A_X_SCHNITT10, G5A_GIER_WEST);\n            spieler_rueckwaerts();",
        "            spieler_setzen(G5A_X_SCHNITT10, G5A_GIER_WEST);\n            if (!re15_nb2_aus(0x200)) spieler_rueckwaerts();", p)
s = rep(s, "            pl->x = G5A_X_SCHNITT5_PORT; pl->y = 0; pl->z = G5A_Z;",
        "            pl->x = re15_nb2_aus(0x200) ? G5A_X_ENDE : G5A_X_SCHNITT5_PORT; pl->y = 0; pl->z = G5A_Z;", p)
wr(p, s)
print('ok')

p = 'engine/src/enemy_ai_boss_g5.c'
s = rd(p)
diag = """
/* NB2-DIAGNOSE (TEMPORAER): RE15_NB2_DIAG=<datei> - je Kampfbild eine Zeile. */
extern int re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus, int32_t *scale_x, uint16_t *kontakt);
static void g5_nb2_diag(const re15_actor_t *e, const re15_actor_t *pl)
{
    static int an = -1; static FILE *f = 0; static unsigned n = 0;
    const g5_state_t *g = &s_g5;
    if (an < 0) { const char *v = getenv("RE15_NB2_DIAG"); an = v ? 1 : 0; if (an) f = fopen(v, "w"); }
    if (!an || !f || !g->gestartet) return;
    fprintf(f, "n%u pl=(%d,%d) hp=%d hr=%u g5 sub=%u ph=%u t=%d blob=%u/%u cd=%d mo=%d af=%u x=%d z=%d hp=%d dist=%d arc=%d hb=%d",
            n++, (int)pl->x, (int)pl->z, (int)pl->hp, (unsigned)pl->hit_react, g->sub, g->ph, (int)g->timer, g->blob, g->blob_ph,
            (int)g->biss_cd, (int)e->motion, (unsigned)e->anim_frame, (int)e->x, (int)e->z, (int)e->hp,
            (int)re15_enemy_player_dist((re15_actor_t *)e, (re15_actor_t *)pl), re15_ai_arc_test((re15_actor_t *)e, pl->x, pl->z, 128),
            (int)g->hitbox_b);
    for (int i = 0; i < 4; i++) {
        int sb = 0, ph = 0, am = 0; int32_t sc = 0; uint16_t k = 0;
        re15_g5_tentakel_zustand(i, &sb, &ph, &am, &sc, &k);
        fprintf(f, " T%d=%d/%d/%d/%x", i, sb, ph, (int)sc, (unsigned)k);
    }
    fputc('\n', f); fflush(f);
}
"""
anchor = "static void g5_fressen_beginn(re15_actor_t *e, re15_actor_t *pl);\n"
s = rep(s, anchor, anchor + diag, p)
s = rep(s, "        g5_devour_opfer_tick(pl);                      /* Pfad 2 @0x80103ea0 kann den Flinch abloesen */\n",
        "        g5_devour_opfer_tick(pl);                      /* Pfad 2 @0x80103ea0 kann den Flinch abloesen */\n        g5_nb2_diag(e, pl);\n", p)
s = rep(s, "    g5_devour_opfer_tick(pl);       /* Spieler-Routine 6 (0x80103908), s. Block oben */\n",
        "    g5_devour_opfer_tick(pl);       /* Spieler-Routine 6 (0x80103908), s. Block oben */\n    g5_nb2_diag(e, pl);\n", p)
wr(p, s)
print('diag ok')
