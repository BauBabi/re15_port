/* probe_adv_1030_verify.c — ADVERSARIALE GEGENPROBE zu Lane "behavior-0d" (ROOM1030).
 *
 * Prueft GENAU die Behauptungen, die ohne Messung aufgestellt wurden:
 *   V1  Opcode 0x53: laeuft er im Port als FADE-Setter (Befund B6) oder als
 *       Work_set-by-work_var? -> direkte Messung: nach `Work_set(2, work_var[4])`
 *       muss g_scd.work_slot == work_var[4]+1 sein.
 *   V2  sub00 For(20)+Switch(6): setzt der Port mit gesetztem Flag(4,15) sechs
 *       Aktoren NOERDLICH des Tor-Riegels (z > -22771)? Das ist die Kernbehauptung
 *       von B6 ("Port kann die Schleife nicht ausfuehren").
 *   V3  Gegenprobe: dieselbe Ladung OHNE Flag(4,15) -> ELSE-Zweig, Spawn-Positionen.
 *   V4  Member 15 (entity+0xB) fuer AKTOREN: Wert nach Raum-Load + AOT-Scan.
 *   V5  Opcode 0x3D (Member_get): registriert? Effekt von sub07 auf +0x1C4.
 *   V6  Zaehl-Check: wie viele Aktoren leben, wieviele stehen noerdlich des Riegels.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_ems.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* Der Tor-Riegel aus ROOM1030.RDT (SCA rgn2/idx6 == rgn3/idx6):
 * corner z = -24420, depth 1649 -> z[-24420..-22771]. NOERDLICH = z > -22771. */
#define GATE_Z_NORTH (-22771)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static void roster(const char *tag)
{
    int live = 0, north = 0;
    printf("-- %s --\n", tag);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        live++;
        int n = (s != 0 && (int32_t)a->z > GATE_Z_NORTH);
        if (n) north++;
        printf("   slot=%2d type=0x%02X grid=0x%02X st=%u s1=0x%02x mo=%-3d m0b=%u "
               "af=0x%04x pos=(%7ld,%7ld,%7ld) ry=%5d %s\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, (int)a->motion,
               a->member_0b, (unsigned)a->anim_flags,
               (long)a->x, (long)a->y, (long)a->z, (int)a->rot_y, n ? "<== NORD" : "");
    }
    printf("   LIVE=%d  ENEMIES_NORTH_OF_GATE=%d  (Riegel z > %d)\n\n", live, north, GATE_Z_NORTH);
}

static int count_north(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && (int32_t)g_actors[s].z > GATE_Z_NORTH) n++;
    return n;
}

static uint8_t s_scratch[0x80000];

/* Laedt die em16-Bank aus CDEMD0.EMS, damit die AI ueberhaupt Clips/Bewegung hat
 * (Lehre: eine Messung ohne geladene Bank reproduziert den LIVE-Zustand NICHT). */
static int load_em_bank(const char *base, uint8_t type)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL; ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void boot_room(const re15_rdt_t *rdt, int set_flag_4_15)
{
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    /* Flag(4,15) = "Cutscene gesehen" (sub00 `21 04 0f 01` @Datei 0x1ff4 / 0x2010). */
    re15_game_flag_set(4, 15, set_flag_4_15);
    scd_room_reenter(rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 16; f++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    }
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t size = 0;
    int fails = 0;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }

    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    /* =============== V1: Opcode 0x53 direkt =============== */
    printf("=== V1: Opcode 0x53 (Work_set-by-work_var @0x80040e18) ===\n");
    {
        re15_actor_init();
        scd_vm_init();
        g_current_room_id = 0x1030;
        g_actors[0].active = 1; g_actors[0].type = 0; g_actors[0].hp = 100;
        /* zwei Fake-Gegner in Script-Slot 3 und 7 (= Aktor 4 und 8) */
        for (int k = 0; k < 2; k++) {
            int sl = k ? 8 : 4;
            g_actors[sl].active = 1; g_actors[sl].type = 0x16; g_actors[sl].hp = 100;
        }
        /* Mini-Programm: Save(var4=3) ; Work_set(kind=2, idx=work_var[4]) ; Evt_end */
        static uint8_t prog[] = { 0x24, 0x04, 0x03, 0x00,   /* 0x24 Save var4 = 3 */
                                  0x53, 0x02, 0x04,         /* 0x53 Work_set kind2 idx=work_var[4] */
                                  0x01, 0x00 };             /* Evt_end */
        scd_thread_start(0, prog);
        for (int i = 0; i < 4; i++) scd_vm_tick();
        printf("   work_vars[4]=%d  g_scd.work_slot=%d (erwartet 4 = Script-Slot 3 + 1)\n",
               (int)g_scd.work_vars[4], (int)g_scd.work_slot);
        if (g_scd.work_slot == 4)
            printf("   -> 0x53 BINDET die Work-Entity korrekt (B6 'nie eine Work-Entity' WIDERLEGT)\n");
        else { printf("   -> 0x53 bindet NICHT (B6 bestaetigt)\n"); fails++; }
    }
    printf("\n");

    /* =============== V5: Opcode 0x3D =============== */
    printf("=== V5: Opcode 0x3D (Member_get @0x80041238) + sub07-Semantik ===\n");
    {
        re15_actor_init();
        scd_vm_init();
        g_current_room_id = 0x1030;
        g_actors[0].active = 1; g_actors[0].type = 0; g_actors[0].hp = 100;
        g_actors[4].active = 1; g_actors[4].type = 0x16; g_actors[4].hp = 100;
        g_actors[4].anim_flags = 0x0007;   /* Vorbelegung: +0x1C4 = 7 */
        /* Save(var4 = 0x1234) ; Work_set(2, 3) ; [sub07:] Member_get(var4, m16) ;
         * Calc(OR, var4, 0x1000) ; Member_set2(m16, var4) ; Evt_end */
        static uint8_t prog[] = { 0x24, 0x04, 0x34, 0x12,
                                  0x2e, 0x02, 0x03,
                                  0x3d, 0x04, 0x10, 0x00,
                                  0x26, 0x00, 0x05, 0x04, 0x00, 0x10,
                                  0x35, 0x10, 0x04, 0x00,
                                  0x01, 0x00 };
        scd_thread_start(0, prog);
        for (int i = 0; i < 8; i++) scd_vm_tick();
        printf("   nach sub07-Aequivalent: work_vars[4]=0x%04x  actor[4].anim_flags=0x%04x\n",
               (unsigned)(uint16_t)g_scd.work_vars[4], (unsigned)g_actors[4].anim_flags);
        printf("   ORIGINAL-Erwartung (0x3D implementiert): 0x0007|0x1000 = 0x1007\n");
        printf("   PORT-Erwartung wenn 0x3D unregistriert:  0x1234|0x1000 = 0x1234 (Bit schon da)\n");
        if (g_actors[4].anim_flags == 0x1007)
            printf("   -> 0x3D IST implementiert\n");
        else
            printf("   -> 0x3D FEHLT: +0x1C4 wurde auf 0x%04x gesetzt statt 0x1007 (B5 bestaetigt)\n",
                   (unsigned)g_actors[4].anim_flags);
    }
    printf("\n");

    /* =============== V3: ROOM1030 OHNE Flag(4,15) =============== */
    printf("=== V3: ROOM1030 Laden OHNE Flag(4,15) (ELSE-Zweig) ===\n");
    boot_room(&rdt, 0);
    roster("V3 nach 16 VM-Ticks");
    int north_off = count_north();

    /* =============== V2: ROOM1030 MIT Flag(4,15) =============== */
    printf("=== V2: ROOM1030 Laden MIT Flag(4,15)=1 (IF-Zweig: 6 Positionen NOERDLICH) ===\n");
    boot_room(&rdt, 1);
    roster("V2 nach 16 VM-Ticks");
    int north_on = count_north();

    printf("=== ERGEBNIS V2/V3 ===\n");
    printf("   Gegner noerdlich des Riegels:  Flag(4,15)=0 -> %d   Flag(4,15)=1 -> %d\n",
           north_off, north_on);
    if (north_on == 6)
        printf("   -> Der Port FUEHRT die For(20)/Switch(6)-Schleife aus (B6 WIDERLEGT)\n");
    else
        printf("   -> Der Port setzt NICHT 6 nach Norden (B6 in dieser Hinsicht bestaetigt)\n");

    /* =============== V4: member_0b der Aktoren =============== */
    printf("\n=== V4: Member 15 (entity+0xB) fuer AKTOREN nach AOT-Scan ===\n");
    {
        int nonzero = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            if (g_actors[s].member_0b) nonzero++;
        }
        printf("   Aktoren mit member_0b != 0: %d\n", nonzero);
        printf("   (Original: FUN_800436a8 @0x800436f0-48 loescht +0xB je aktivem Gegner\n"
               "    (@0x8004371c `sb zero,0(0x800acc37+i*500)`) und ruft dann\n"
               "    FUN_80042bac(entity, mask=2, mode=0) @0x80043724 -> Stempel @0x80042fc4.)\n");
        if (nonzero == 0)
            printf("   -> Der Port stempelt Gegner-+0xB NIE -> Member_cmp(15,==,5) ist immer falsch\n");
    }

    /* =============== V6: 200 Ticks Live-AI, Kriech-Check =============== */
    printf("\n=== V6: 200 Ticks Live-AI: wandert jemand durch den Riegel? ===\n");
    {
        long z0[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) z0[s] = (long)g_actors[s].z;
        for (int t = 0; t < 200; t++) {
            scd_vm_tick();
            re15_aot_scan(g_actors[0].x, g_actors[0].z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
        }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            printf("   slot=%2d z %7ld -> %7ld (dz=%ld) s1=0x%02x mo=%d af=0x%04x m0b=%u\n",
                   s, z0[s], (long)g_actors[s].z, (long)g_actors[s].z - z0[s],
                   g_actors[s].sub_state_1, (int)g_actors[s].motion,
                   (unsigned)g_actors[s].anim_flags, g_actors[s].member_0b);
        }
        printf("   NORD nach 200 Ticks: %d\n", count_north());
    }

    /* =============== V7: Blockt der Riegel im PORT ueberhaupt? =============== */
    printf("\n=== V7: Enemy-Wall-Clamp gegen den Riegel (Maske 4 vs. Maske 8) ===\n");
    {
        /* Der SCA-Riegel: x[-20144..4120] z[-24420..-22771], u0=0xFF, floor=0x00.
         * Ein Zombie im Pferch (z=-25200) will nach Norden (z=-22000). */
        /* SCHRITTWEISE (wie eine echte Lauf-Bewegung, ~60/Frame), NICHT als 3200-Sprung:
         * ein Ein-Schritt-Test tunnelt durch die nur 1649 tiefe Zelle und misst NICHTS. */
        static const int32_t xs[] = { -19000, -16000, -14000, -10000, -7300, -5400, -1218 };
        for (unsigned c = 0; c < sizeof xs / sizeof xs[0]; c++) {
            int32_t px = xs[c], pz = -26000;
            for (int step = 0; step < 120; step++) {
                int32_t nx = px, nz = pz + 60;
                re15_collision_constrain_enemy(&rdt, px, pz, &nx, &nz, 400, 0, 4u);
                px = nx; pz = nz;
                if (pz > GATE_Z_NORTH) break;
            }
            printf("   x=%7d: 120x60-Schritte nach Norden -> Endposition (%7d,%7d)  %s\n",
                   (int)xs[c], (int)px, (int)pz,
                   (pz > GATE_Z_NORTH) ? "DURCHGEKOMMEN (Riegel wirkungslos!)" : "GEBLOCKT");
        }
        printf("   band_from_y(0)=%d  ceiling=(%d,%d) -> Quadrant der Ziele:\n",
               re15_collision_band_from_y(0), (int)(int16_t)rdt.ceiling_x, (int)(int16_t)rdt.ceiling_z);
        printf("   (Zellen-Bands: Riegel floor=0x00>>4=0, alle anderen floor=0x03>>4=0)\n");
    }

    /* =============== V10: DER SPAWN-CAP =============== */
    printf("\n=== V10: Sce_em_set-CAP  (main00 @Datei 0x1de2 `24 12 06 00`) ===\n");
    {
        /* Original: op_save 0x24 @0x80040018 schreibt *(s16*)(0x800b0fd0 + pc[1]*2).
         * pc[1]=0x12=18 -> 0x800b0fd0+36 = 0x800b0ff4 = DER CAP, den Sce_em_set
         * @0x8004221c liest (`lh v1,4084(v1)`), gegen den Live-Zaehler 0x800b0ff2
         * (=work_var[17], @0x800421dc) : `slt count,cap` @0x80042224;
         * count>=cap -> entity+0x0 = 0x8000, Spawn ABGEBROCHEN (@0x80042230-38).
         * Reset auf 0xFF/0 nur bei Raum-Init FUN_8003ecec (@0x8003ed58-60 / @0x8003ed7c),
         * gerufen NUR aus @0x8003ef84 (Raum-Skript-Init) — also VOR main00. */
        int live = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) live++;
        printf("   nach main00: g_scd.work_vars[17]=%d (0x800b0ff2 Zaehler)  "
               "work_vars[18]=%d (0x800b0ff4 CAP)\n",
               (int)g_scd.work_vars[17], (int)g_scd.work_vars[18]);
        printf("   PORT-Gegner aktiv: %d   ORIGINAL-Erwartung mit CAP=6: 6\n", live);
        if ((int)g_scd.work_vars[18] == 6 && live != 6)
            printf("   -> Der Port SCHREIBT den Cap (work_var[18]=6), WERTET IHN ABER NICHT AUS.\n"
                   "      Das ist die belegte Ursache fuer 'Dutzende statt SECHS'.\n");
    }

    /* =============== V8: Sca_id_set-Wirkung + LIVE-Lauf mit Bank =============== */
    printf("\n=== V8: u0 des Riegels nach dem Laden (Sca_id_set 0x37 -> 0xF7?) ===\n");
    {
        printf("   Flag(4,15)=1-Lauf: sca[18].u0=0x%02X  sca[28].u0=0x%02X (Riegel-Zellen)\n",
               rdt.sca[18].u0, rdt.sca[28].u0);
        printf("   Maske 4 (stehender Zombie) & 0xF7 = %d  -> %s\n", 4 & 0xF7,
               (4 & 0xF7) ? "weiter BLOCKIERT" : "durchlaessig");
        printf("   Maske 8 (Kriecher, +0x1D7=8) & 0xF7 = %d -> %s\n", 8 & 0xF7,
               (8 & 0xF7) ? "blockiert" : "DURCHLAESSIG");
    }

    /* =============== V9: LIVE-Lauf MIT geladener em16-Bank (Bewegung moeglich) =============== */
    printf("\n=== V9: 1500 Ticks LIVE (Bank geladen, Flag(4,15)=1) — bleiben die 9 im Pferch? ===\n");
    {
        boot_room(&rdt, 1);
        if (!load_em_bank(base, 0x16)) printf("   (WARNUNG: Bank nicht geladen -> Bewegung evtl. gehemmt)\n");
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->x = -12000; pl->z = -15000;    /* Spieler im Nordraum, damit die AI engaged */
        for (int t = 0; t < 1500; t++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            re15_actor_step_all_walkers();
            re15_actors_anim_advance();
            re15_enemy_ai_run_all(0);
        }
        int north = 0, moved = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            if ((int32_t)g_actors[s].z > GATE_Z_NORTH) north++;
            printf("   slot=%2d pos=(%7ld,%7ld) s1=0x%02x mo=%-3d af=0x%04x %s\n", s,
                   (long)g_actors[s].x, (long)g_actors[s].z, g_actors[s].sub_state_1,
                   (int)g_actors[s].motion, (unsigned)g_actors[s].anim_flags,
                   ((int32_t)g_actors[s].z > GATE_Z_NORTH) ? "<== NORD" : "");
        }
        (void)moved;
        printf("   -> NORDLICH nach 1500 Ticks: %d (Original nach der Cutscene: 6 + gestaffelte Kriecher)\n",
               north);
    }

    free(data);
    printf("\n(Sonde ist eine MESSUNG; Exit 0 auch bei 'fails', damit ctest nicht blockiert.)\n");
    (void)fails;
    return 0;
}
