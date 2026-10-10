/* test_r36_kleidung_teile.inc — Teile hund / kraehe / lut von test_r36_kleidung.c (direkt eingebunden). */

/* ---------------------------------------------------------------------------------------------- */
/* hund: ROOM11D0 (Zwinger-Gang, Hunde frei: Flag (3,152)=1 wie probe_r27_hund_biss.c), RE2-Hund.
 * MASSSTAB (Nachbesserung 1): ein RE1.5-Biss kostet 10 HP (`addiu a0,v0,-10` @0x8010f2c8) und
 * stempelt EINMAL +0x32 (@0x8010f360/64) auf byte[0x80121014 + aca59*2 - 4 + (rng&1)] = 05 07 04 06.
 * Jeder RE2-Boden-Biss (RE2 @0x80104F7C-88, 20 HP `addiu a0,zero,20` @0x80104EB8) muss deshalb
 * GENAU 20/10 = 2 solche Stempel tragen: der Zustand nach dem Biss muss sich aus dem davor durch
 * zwei Akkumulator-Schritte (FUN_80037edc, @0x80037efc-f50 nachgerechnet) mit je +0x32 auf Panels
 * EINES Blickrichtungs-Paars {5,7} bzw. {4,6} ergeben; alles andere bleibt. Vorher (ein Stempel je
 * Biss) scheiterte das an jedem Biss. */
static void hund_sim_add(uint8_t w[8][2], int p, int amt)
{
    int sum = (int)w[p][1] + amt;                         /* addu @0x80037f04, sb @0x80037f08 */
    w[p][1] = (uint8_t)sum;
    if ((sum & 0xff) < 0x78) return;                      /* lbu @0x80037f0c, sltiu 0x78 @0x80037f14 */
    w[p][1] = 0;                                          /* @0x80037f24 */
    w[p][0] = (uint8_t)(w[p][0] + 1);                     /* @0x80037f2c */
    if (w[p][0] >= 3) w[p][0] = 2;                        /* Clamp @0x80037f40/50 */
}
static int hund_zwei_stempel(const uint8_t w0[8][2], const uint8_t w1[8][2])
{
    static const int paar[2][2] = { { 5, 7 }, { 4, 6 } };  /* @0x80121014: aca59=2 -> 05 07, 3 -> 04 06 */
    for (int r = 0; r < 2; r++)
        for (int a = 0; a < 2; a++)
            for (int b = 0; b < 2; b++) {
                uint8_t s[8][2]; memcpy(s, w0, sizeof s);
                hund_sim_add(s, paar[r][a], 0x32);
                hund_sim_add(s, paar[r][b], 0x32);
                if (memcmp(s, w1, sizeof s) == 0) return 1;
            }
    return 0;
}
static int teil_hund(int budget)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM11D0.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }
    re15_wound_reset();
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = 0x11D0; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_game_flag_set(3, 152, 1);                       /* ELSE-Zweig: Hunde frei im Gang */
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x20);
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x20);
        if (!eb || !eb->ok) { printf("SKIP: RE2-Bank EM020 fehlt\n"); return 77; } }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -300; pl->z = -17400; pl->rot_y = 1024;     /* re15_room_spawns.h ROOM11D0 */
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    int nh = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) {
            nh++;
            if (g_actors[s].grid_id == 0x41) g_actors[s].grid_id = 0x42;   /* Marke 0x42 = frei */
        }
    printf("ROOM11D0 RE2: %d Hunde\n", nh);
    if (nh == 0) { printf("FAIL: keine Hunde gespawnt\n"); return 1; }

    uint8_t w0[8][2], w1[8][2]; re15_wound_save(w0);
    int bisse = 0, fehler = 0, hp_verlust = 0;
    for (int f = 0; f < budget; f++) {
        if (pl->hp < 30) pl->hp = 100;                    /* am Leben halten (30 - 20 >= 0: ueberlebt) */
        if (re15_player_victim_state() != 0) { pl->hp = 100; }
        int hp0 = pl->hp;
        frame(0, 0);
        re15_wound_save(w1);
        int geaendert = 0;
        for (int p = 0; p < 8; p++)
            if (w1[p][0] != w0[p][0] || w1[p][1] != w0[p][1]) geaendert++;
        if (geaendert) {
            printf("  F%d Biss (hp %d -> %d):", f, hp0, pl->hp);
            for (int p = 0; p < 8; p++)
                if (w1[p][0] != w0[p][0] || w1[p][1] != w0[p][1])
                    printf(" Panel %d %u/%u -> %u/%u", p, w0[p][0], w0[p][1], w1[p][0], w1[p][1]);
            printf("\n");
            bisse++;
            hp_verlust += hp0 - pl->hp;
            if (hp0 - pl->hp != 20) { printf("FAIL: Biss kostet %d HP statt 20 (@0x80104EB8)\n", hp0 - pl->hp); fehler++; }
            if (!hund_zwei_stempel(w0, w1))
                { printf("FAIL: Zustand nach dem Biss ist NICHT genau 2 x 0x32 auf einem Paar 05/07 bzw. 04/06\n"); fehler++; }
        }
        memcpy(w0, w1, sizeof w0);
        if (bisse >= 4) break;
    }
    print_wounds("  Ende");
    printf("Massstab: %d Bisse, %d HP, je Biss 2 Stempel = %d Stempel (RE1.5: 1 je 10 HP @0x8010f2c8)\n",
           bisse, hp_verlust, 2 * bisse);
    CHECK(bisse >= 3, "weniger als 3 Boden-Bisse des RE2-Hunds im Budget (%d)", bisse);
    CHECK(fehler == 0, "%d Fehler an den Biss-Stempeln (Massstab 0x32 je 10 HP)", fehler);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
/* kraehe: (a) die Baender von 0x801161e8 direkt (slti 1500/3000/3600 @0x80116200/20/54) und
 * (b) ROOM10C0 live (probe_re2_crow_10c0.c-Harness): der RE2-Griff (Claim -> Opfer-FSM) stempelt. */
static int teil_kraehe(int budget)
{
    re15_actor_t k, p; memset(&k, 0, sizeof k); memset(&p, 0, sizeof p);
    struct { int vert; int panels[4]; int amt[4]; int n; } tab[] = {
        { 1499, {1,2,0,0}, {0x0a,0x0a,0,0}, 2 },
        { 1500, {3,4,6,5}, {0x0a,0x28,0x28,0x28}, 4 },
        { 2999, {3,4,6,5}, {0x0a,0x28,0x28,0x28}, 4 },
        { 3000, {0,7,0,0}, {0x08,0x28,0,0}, 2 },
        { 3599, {0,7,0,0}, {0x08,0x28,0,0}, 2 },
        { 3600, {0,0,0,0}, {0,0,0,0}, 0 },
    };
    for (size_t i = 0; i < sizeof tab / sizeof tab[0]; i++) {
        re15_wound_reset();
        p.y = 0; k.y = -tab[i].vert;                       /* vert = Spieler.y - Kraehe.y */
        re15_kleidung_kraehe(&k, &p);
        uint8_t w[8][2]; re15_wound_save(w);
        int soll[8] = {0}; for (int j = 0; j < tab[i].n; j++) soll[tab[i].panels[j]] += tab[i].amt[j];
        for (int q = 0; q < 8; q++)
            CHECK((int)w[q][1] == soll[q] && w[q][0] == 0, "vert %d: Panel %d Akku %d, erwartet %d",
                  tab[i].vert, q, (int)w[q][1], soll[q]);
    }
    printf("Baender 0x801161e8: %s\n", s_fail ? "FEHLER" : "OK");

    /* (b) live */
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10C0.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    re15_actor_init(); scd_vm_init(); re15_enemy_reset();
    g_current_room_id = 0x10c0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    load_bank2(0x21);
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x21);
        if (!eb || !eb->victim_ok) { printf("SKIP: RE2-Bank EM021 (victim) fehlt\n"); return 77; } }
    re15_wound_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 3400; pl->y = 0; pl->z = -3900;   /* neben Kraehe 2 */
    pl->hp = 100; pl->rot_y = 0; pl->hit_react = 0;
    memcpy((void *)&g_room_rdt, &s_rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    int griffe = 0, stempel_bei_griff = 0, prev_v = 0;
    uint8_t w0[8][2]; re15_wound_save(w0);
    for (int f = 0; f < budget; f++) {
        pl->hit_react = 0; if (pl->hp < 30) pl->hp = 100;
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        int v = re15_player_victim_state();
        if (v == 1 && prev_v != 1) {
            griffe++;
            uint8_t w1[8][2]; re15_wound_save(w1);
            int sum0 = 0, sum1 = 0;
            for (int q = 0; q < 8; q++) { sum0 += w0[q][0] * 120 + w0[q][1]; sum1 += w1[q][0] * 120 + w1[q][1]; }
            printf("  F%d Kraehen-Griff #%d: Wundpunkte %d -> %d\n", f, griffe, sum0, sum1);
            if (sum1 > sum0) stempel_bei_griff++;
            memcpy(w0, w1, sizeof w0);
        }
        prev_v = v;
        if (griffe >= 2) break;
    }
    print_wounds("  Ende");
    CHECK(griffe >= 1, "kein RE2-Kraehen-Griff in ROOM10C0 im Budget");
    CHECK(stempel_bei_griff == griffe, "nur %d von %d Griffen stempelten", stempel_bei_griff, griffe);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
/* lut: Panel-Ziel-LUT @0x80074208 — Leon = Zeile 0, Elza (aca5c = 4) = Zeile 4 (NICHT Zeile 1). */
static int teil_lut(void)
{
    static const int leon[8][2] = { {67,35},{43,44},{31,192},{66,175},{22,116},{16,26},{6,102},{0,0} };
    static const int elza[8][2] = { {70,38},{40,119},{48,47},{66,162},{9,79},{11,15},{27,190},{43,192} };
    for (int p = 0; p < 8; p++) {
        int x = -1, y = -1;
        CHECK(re15_kleidung_panel_ziel(0, p, &x, &y) == 1, "Leon Panel %d ungueltig", p);
        CHECK(x == leon[p][0] * 2 && y == leon[p][1], "Leon Panel %d: (%d,%d) statt (%d,%d)", p, x, y, leon[p][0]*2, leon[p][1]);
        CHECK(re15_kleidung_panel_ziel(4, p, &x, &y) == 1, "Elza Panel %d ungueltig", p);
        CHECK(x == elza[p][0] * 2 && y == elza[p][1], "Elza Panel %d: (%d,%d) statt (%d,%d)", p, x, y, elza[p][0]*2, elza[p][1]);
    }
    { int x, y; CHECK(re15_kleidung_panel_ziel(8, 0, &x, &y) == 0, "Zeile 8 liegt hinter der Tabelle (@0x80074288 = Debug-Byte)"); }
    /* Ziel + Rechteck (16 hw x 64 = 32 x 64 Texel) bleibt innerhalb der Seiten 0/1 (x < 256, y+64 <= 256) */
    for (int c = 0; c < 8; c += 4) for (int p = 0; p < 8; p++) {
        int x, y; re15_kleidung_panel_ziel(c, p, &x, &y);
        CHECK(x + 32 <= 256 && y + 64 <= 256, "Char %d Panel %d: Rechteck (%d,%d) ragt aus den Body-Seiten", c, p, x, y);
    }
    printf("LUT @0x80074208: Leon Zeile 0 / Elza Zeile 4 %s\n", s_fail ? "FEHLER" : "OK");
    return 0;
}
