/*
 * test_r35_redhawk.c — Runde 35 Spur D "redhawk".
 *
 * Nutzer (AUFTRAG.md Z.17): "Wenn ich mit der Super Redhawk auf die Hunde schiesse bleiben die
 * Fleisch Effekte die sich rausloesen permanent da in loop."
 *
 * Dossier: analysis/befunde_runde35/D_redhawk.md.
 *
 * Aufruf: test_r35_redhawk [teil]
 *   zensus   Routinen-Zensus aller Raum-ESP-Baenke (STAGE1..6): je (Raum, Effekt-Id, Sub, Stream,
 *            Zeile) die Routinen-Waehler A (+0x00) und B (+0x02), Flags (+0x0e), +0x16, +0x1e,
 *            +0x26 und die Anim-Records (Dauer/Schleife). Reines Messwerkzeug, immer 0.
 */
#include "re15_esp.h"
#include "re15_esp_brocken.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t u32le(const uint8_t *b, size_t o)
{
    return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24);
}
static uint16_t u16le(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }

/* Raum-ESP parsen wie pc_load_room_esp (platform/pc/main.c): RDT+0x4C/50/54/58. */
static int room_esp(const uint8_t *rdt, size_t n, re15_esp_t *out)
{
    if (n < 0x5C) return -1;
    return re15_esp_parse(rdt, n, u32le(rdt, 0x4C), u32le(rdt, 0x50), u32le(rdt, 0x54),
                          u32le(rdt, 0x58), out);
}

/* ===================================== ZENSUS ============================================ */
static void zensus_bank(const char *tag, const re15_esp_t *esp, unsigned seenA[64], unsigned seenB[64])
{
    for (int ei = 0; ei < esp->id_count; ei++) {
        const re15_esp_eff_t *e = &esp->eff[ei];
        printf("%s id=%u count_a=%u count_b=%u anim:", tag, e->effect_id, e->count_a, e->count_b);
        for (int k = 0; k < e->count_a && k < 40; k++) {
            re15_esp_anim_t a;
            if (re15_esp_anim(esp, ei, k, &a) != 0) break;
            printf(" [%d d%04x p%04x]", k, a.desc, a.param);
        }
        printf("\n");
        for (int sub = 0; sub < 8; sub++) {
            int ns = re15_esp_row_streams(esp, ei, sub);
            if (ns <= 0 || ns > 32) continue;
            for (int s = 0; s < ns; s++) {
                int nr = 0;
                const uint8_t *r = re15_esp_row_stream(esp, ei, sub, s, &nr);
                if (!r) { printf("%s id=%u sub=%d st=%d: (kein Stream)\n", tag, e->effect_id, sub, s); continue; }
                for (int k = 0; k < nr && k < 32; k++) {
                    const uint8_t *row = r + k * 40;
                    unsigned A = u16le(row, 0x00), B = u16le(row, 0x02);
                    if (A < 64) seenA[A]++;
                    if (B < 64) seenB[B]++;
                    printf("%s id=%u sub=%d st=%d/%d row=%d/%d A=%u B=%u acc=(%d,%d,%d) f0e=%02x vel=(%d,%d,%d)"
                           " p16=%u p1e=%u g26=%u\n",
                           tag, e->effect_id, sub, s, ns, k, nr, A, B,
                           (int16_t)u16le(row, 0x08), (int16_t)u16le(row, 0x0a), (int16_t)u16le(row, 0x0c),
                           row[0x0e],
                           (int16_t)u16le(row, 0x10), (int16_t)u16le(row, 0x12), (int16_t)u16le(row, 0x14),
                           u16le(row, 0x16), u16le(row, 0x1e), u16le(row, 0x26));
                }
            }
        }
    }
}

/* Routinen, die der Port in esp_fx_dispatch (A) bzw. esp_fx_dispatch_b (B) ausfuehrt (re15_esp.c).
 * Alles andere laeuft im Port als noop. */
static int port_kennt_a(unsigned a)
{
    static const unsigned k[] = { 0, 3, 4, 5, 8, 9, 10, 11, 15, 16, 17, 18, 30, 31, 38, 41, 42 };
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) if (k[i] == a) return 1;
    return 0;
}
static int s_mit_brocken = 1;   /* 0 = Stand vor Runde 35 (B 36/37 fehlten) */
static int port_kennt_b(unsigned b)
{
    return b == 0 || b == 12 || b == 29 || (s_mit_brocken && (b == 36 || b == 37));
}

/* Endet die Anim ab Record 0 von selbst? 1 = Terminator 0/0 erreicht, 0 = Schleifenmarke (0xFF)
 * vorher (dann endet der Platz NUR ueber eine Routine: Flags := 0 oder Anim-Index-Sprung), -1 = Fehler.
 * Ablauf wie die Tick-Anim-Stufe @0x8001a38c-47c (Index++, Terminator @0x8001a3e8-40c, Schleife
 * @0x8001a410-44c). */
static int anim_endet(const re15_esp_t *esp, int ei)
{
    int idx = 0;
    for (int schritt = 0; schritt < 256; schritt++) {
        re15_esp_anim_t a;
        if (re15_esp_anim(esp, ei, idx, &a) != 0) return -1;
        unsigned dur = a.param & 0xff, loop = a.desc & 0xff;
        if (dur == 0 && loop == 0) return 1;
        if (dur == 0xff) return 0;
        idx++;
    }
    return 0;
}

/* Gueltigkeits- und Haenger-Zensus: ein Stream gilt als GUELTIG, wenn alle Zeilen A,B < 48 tragen
 * (die Tabelle @0x80071d40 hat 48 Eintraege; groessere Werte = Fehlparse hinter dem Ende). */
static void zensus_haenger(const char *tag, const re15_esp_t *esp, int *n_streams, int *n_haenger)
{
    for (int ei = 0; ei < esp->id_count; ei++) {
        int endet = anim_endet(esp, ei);
        for (int sub = 0; sub < 8; sub++) {
            int ns = re15_esp_row_streams(esp, ei, sub);
            if (ns <= 0 || ns > 32) continue;
            for (int s = 0; s < ns; s++) {
                int nr = 0;
                const uint8_t *r = re15_esp_row_stream(esp, ei, sub, s, &nr);
                if (!r || nr <= 0 || nr > 32) continue;
                int gueltig = 1, unbekannt = 0, flags_null = 0;
                char liste[128]; liste[0] = 0;
                for (int k = 0; k < nr; k++) {
                    unsigned A = u16le(r + k * 40, 0), B = u16le(r + k * 40, 2);
                    if (A >= 48 || B >= 48) { gueltig = 0; break; }
                    if (!port_kennt_a(A) || !port_kennt_b(B)) {
                        unbekannt = 1;
                        size_t l = strlen(liste);
                        if (l < sizeof liste - 16) snprintf(liste + l, sizeof liste - l, " %u/%u", A, B);
                    }
                    if ((r + k * 40)[0x0e] == 0 && (A == 3 || A == 4)) flags_null = 1;
                }
                if (!gueltig) continue;
                (*n_streams)++;
                if (unbekannt && endet == 0) {
                    (*n_haenger)++;
                    printf("HAENGER %s id=%u sub=%d st=%d: Anim endlos, Port-unbekannte Routinen A/B:%s%s\n",
                           tag, esp->eff[ei].effect_id, sub, s, liste, flags_null ? " (Flags:=0 ueber R3/4)" : "");
                } else if (unbekannt) {
                    printf("LUECKE  %s id=%u sub=%d st=%d: Anim endet selbst, Port-unbekannte Routinen A/B:%s\n",
                           tag, esp->eff[ei].effect_id, sub, s, liste);
                }
            }
        }
    }
}

static int teil_haenger(void)
{
    int rooms = 0, n_streams = 0, n_haenger = 0;
    for (int st = 1; st <= 6; st++) {
        for (int r = 0; r < 0x100; r++) {
            char path[512];
            int id = (st << 12) | (r << 4);
            snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", st, id);
            size_t n = 0; uint8_t *b = slurp(path, &n);
            if (!b) continue;
            re15_esp_t esp; memset(&esp, 0, sizeof esp);
            if (room_esp(b, n, &esp) == 0) {
                char tag[32]; snprintf(tag, sizeof tag, "R%04X", id);
                zensus_haenger(tag, &esp, &n_streams, &n_haenger);
                rooms++;
            }
            free(b);
        }
    }
    printf("HAENGER-ZENSUS: %d Raeume, %d gueltige Streams, %d Haenger (Anim endlos + Port-unbekannte Routine)\n",
           rooms, n_streams, n_haenger);
    return 0;
}

static int teil_zensus(void)
{
    static unsigned seenA[64], seenB[64];
    memset(seenA, 0, sizeof seenA); memset(seenB, 0, sizeof seenB);
    int rooms = 0;
    for (int st = 1; st <= 6; st++) {
        for (int r = 0; r < 0x100; r++) {
            char path[512];
            int id = (st << 12) | (r << 4);                  /* ROOMs000..sFF0 */
            snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", st, id);
            size_t n = 0; uint8_t *b = slurp(path, &n);
            if (!b) continue;
            re15_esp_t esp; memset(&esp, 0, sizeof esp);
            if (room_esp(b, n, &esp) == 0) {
                char tag[32]; snprintf(tag, sizeof tag, "R%04X", id);
                printf("%s ids:", tag);
                for (int ei = 0; ei < esp.id_count; ei++) printf(" %u", esp.eff[ei].effect_id);
                printf("\n");
                zensus_bank(tag, &esp, seenA, seenB);
                rooms++;
            }
            free(b);
        }
    }
    printf("ZENSUS %d Raeume mit ESP. Routinen A:", rooms);
    for (int i = 0; i < 64; i++) if (seenA[i]) printf(" %d(%u)", i, seenA[i]);
    printf("\nZENSUS Routinen B:");
    for (int i = 0; i < 64; i++) if (seenB[i]) printf(" %d(%u)", i, seenB[i]);
    printf("\n");
    return 0;
}

/* ===================================== HUND-ARENA ========================================
 * Aufbau wie probe_r34_reaktion.c bringup_hunde/hund_arena (ROOM11D0, Flag 3:152 = 1 = freier
 * Hunde-Satz), aber MIT gebundener Raum-ESP-Bank (re15_esp_set_room_bank — die Sonde B6 hatte
 * keine, dort liefen die Spawns ins Leere) und der globalen CORE00.ESP. Der ESP-Takt laeuft wie
 * auf der Plattform hinter dem Spielschritt (fx_plattform_pc.c: re15_esp_fx_tick nach
 * re15_game_step = RE1.5 @0x8001ce2c hinter Gegnern @0x8001ce04 und Spieler @0x8001ce0c). */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static int                s_room_id = 0;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_esp_t         s_room_esp, s_global_esp;
static uint8_t           *s_core = NULL;

static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static size_t s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = s; return b;
}
static void load_re2_bank(uint8_t type)
{
    if (re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;
    size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) { eb->buf = NULL; eb->ok = 1; return; }
    eb->type = 0;
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
}

static int room_load(int room_id, const char *stage)
{
    char path[512];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s/ROOM%04X.RDT", stage, room_id);
    free(s_rdt_buf); s_rdt_buf = NULL;
    size_t n = 0;
    s_rdt_buf = slurp(path, &n);
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &s_rdt) != 0) { printf("RDT %s fehlt\n", path); return -1; }
    s_room_id = room_id;
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    memset(&s_room_esp, 0, sizeof s_room_esp);
    if (room_esp(s_rdt_buf, n, &s_room_esp) != 0) { printf("ROOM%04X ohne ESP\n", room_id); return -1; }
    if (!s_core) {
        size_t cs = 0; s_core = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &cs);
        if (s_core && re15_esp_parse_global(s_core, cs, &s_global_esp) == 0) re15_esp_set_global_bank(&s_global_esp);
    }
    return 0;
}

static void bringup_hunde(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    load_re2_bank(0x20);
    re15_player_cmd_reset();
    re15_esp_fx_reset();
    re15_esp_set_room_bank(&s_room_esp);
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = (uint16_t)s_room_id; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_game_flag_set(3, 152, 1);          /* freier Hunde-Satz (probe_r34_reaktion.c:426) */
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
}

/* Erster Hund allein, Spieler `abstand` vor ihm (Blick +x = rot 0, re2z_thrust-Konvention). */
static re15_actor_t *hund_arena(int32_t abstand)
{
    bringup_hunde();
    for (int f = 0; f < 30; f++) frame();
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (e->state != 1) re15_ai_set_state_word(e, 0x100);
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_self1d3 = 0; e->hit_react = 0;
    pl->x = e->x - abstand; pl->z = e->z; pl->y = e->y; pl->rot_y = 0;
    for (int f = 0; f < 3; f++) frame();
    e->re2z_self1d3 = 0; e->hit_react = 0;
    re15_esp_fx_reset();
    return e;
}

/* Zaehlt lebende Plaetze einer Effekt-Id (aktiv UND, bei Zeilen-VM, Flags-Bit 0). */
static int lebend(int id, int nur_sichtbar)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != id) continue;
        if (f->rows_base && !(f->flags & 0x01)) continue;
        if (nur_sichtbar && !re15_esp_fx_visible(f)) continue;
        n++;
    }
    return n;
}

static void dump_id(int id, const char *tag)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != id) continue;
        printf("  %s slot=%d id=%u sub=%u A=%u B=%u fl=%02x anim=%d timer=%d row=%d/%d y=%d xlat_y=%d vel=(%d,%d,%d)"
               " wpos=(%d,%d,%d) f1e=%d\n",
               tag, i, f->effect_id, f->sub_index, u16le(f->row, 0), u16le(f->row, 2), f->flags, f->frame,
               f->timer, f->row_cursor, f->row_count, f->y, f->xlat_y, f->drift_x, f->drift_y, f->drift_z,
               f->wpos[0], f->wpos[1], f->wpos[2], (int16_t)u16le(f->row, 0x1e));
    }
}

/* MESSUNG: Redhawk-Schuss (re15_player_weapon_fire(7) = FUN_80011f50-Kern, Waffe 7) auf den Hund,
 * danach Bild fuer Bild die lebenden Raum-Id-7-Plaetze (die Fleisch-Brocken). */
static int teil_messung(int room, const char *stage, int bilder)
{
    if (room_load(room, stage) != 0) return 1;
    re15_actor_t *e = hund_arena(2000);
    if (!e) { printf("kein Hund 0x20 in ROOM%04X\n", room); return 1; }
    int16_t hp0 = e->hp;
    int r = re15_player_weapon_fire(7);
    printf("ROOM%04X Schuss Waffe 7: Treffer=%d hp %d -> %d st=%d +5=%d\n", room, r, hp0, e->hp, e->state, e->sub_state_1);
    int spitze = 0, letztes = -1;
    for (int f = 1; f <= bilder; f++) {
        frame();
        int n = lebend(7, 0);
        if (n > spitze) spitze = n;
        if (n) letztes = f;
        if (f <= 3 || f == 10 || f == 30 || f == 60 || f == 90 || f == 150 || f == 300 || f == bilder) {
            printf("Bild %4d: id7 lebend %d (sichtbar %d), id0 %d, Hund st=%d/%d/%d\n", f, n, lebend(7, 1),
                   lebend(0, 0), e->state, e->sub_state_1, e->sub_state_2);
            if (f == 30 || f == bilder) dump_id(7, "  ");
        }
    }
    printf("ERGEBNIS ROOM%04X: Spitze id7 %d, letztes Bild mit id7 %d von %d\n", room, spitze, letztes, bilder);
    return 0;
}

/* ===================================== PINS ==============================================
 * Spurverfolgung je Id-7-Platz: Entstehung, Landung (B 36 -> 37), Abschluss (B 37 -> Zeile 1) und
 * Freigabe. Geprueft wird der Mechanismus gegen die Disassembly (Dossier R3/R4/R6):
 *   L1 Landebild: B wird 37 genau im ersten Bild mit Weltlage-y > h = room_coll(x, z, 0, 8, 0x100)
 *      (`slt v0,v0,v1` @0x80018810), im Vorbild war Weltlage-y <= h (an der Vorbild-Lage). Die
 *      Geschwindigkeit wurde VOR der Physik genullt (@0x8001882c-38): nach dem Takt steht sie genau
 *      auf der Beschleunigung (Physik @0x8001a324-388: xlat += vel, DANN vel += acc).
 *   L2 Abschlussbild = erstes Bild nach der Landung mit B != 37: R37 schlug an, weil slot+0x1e (das
 *      h des letzten Nein-Zweigs von R36, `sh a0,30` @0x80018848) < Weltlage-y (`slt` @0x80018878).
 *      Dort: B = 0 (Zeile 1), Flags = row0[0x0e] = 0x13 (@0x8001888c), Zeile 1/2 (@0x800188a0),
 *      Anim-Index 6 oder - wenn der Takt-Zaehler im selben Bild ablief - 7 (@0x800188a4 + Anim-Stufe
 *      @0x8001a3bc). Bis dahin vel.x = vel.z = 0: ein Brocken, der seitlich in eine hohe Zelle
 *      faellt (h = -1800*(Band+1)), faellt senkrecht bis auf die zuletzt gemessene Bodenhoehe.
 *   L3 Freigabe: der Platz endet ueber den Terminator Record 10 (@0x8001a40c), spaetestens
 *      3 (Rest-Takt) + 3*3 (Records 7..9, Dauer 3 = p2003/p1003) + 1 Bilder nach dem Abschlussbild. */
typedef struct {
    int     lebt, landebild, folgebild, ende;
    int16_t vor_wy;          /* Weltlage-y des Vorbilds */
    int16_t vor_h;           /* room_coll an der Weltlage des Vorbilds */
    int16_t h1e;             /* slot+0x1e waehrend B 37 (von R36 zuletzt im Nein-Zweig gespeichert) */
    int     vor_gueltig;     /* Vorbild = ein Takt mit B 36 und gerechneter Weltlage */
    int     fehler;
} spur_t;
static spur_t s_spur[RE15_ESP_FX_MAX];
static int    s_n_spawn, s_n_lande, s_n_ende, s_n_fehler, s_bild;

static void spur_reset(void)
{
    memset(s_spur, 0, sizeof s_spur);
    s_n_spawn = s_n_lande = s_n_ende = s_n_fehler = 0; s_bild = 0;
}

/* nach jedem Bild (Spielschritt + ESP-Takt) */
static void spur_bild(int laut)
{
    s_bild++;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        spur_t *s = &s_spur[i];
        int ist7 = f && f->effect_id == 7 && f->rows_base && (f->flags & 0x01);
        if (!ist7) {
            if (s->lebt) {
                s->lebt = 0; s_n_ende++;
                int frist = s->folgebild ? (s_bild - s->folgebild) : -1;
                if (!s->landebild || !s->folgebild || frist > 3 + 9 + 1) {
                    s_n_fehler++;
                    if (laut) printf("  FEHLER Platz %d: Ende in Bild %d ohne Landung/Abschluss (lande %d folge %d frist %d)\n",
                                     i, s_bild, s->landebild, s->folgebild, frist);
                }
            }
            continue;
        }
        const unsigned B = u16le(f->row, 2);
        if (!s->lebt) {
            memset(s, 0, sizeof *s);
            s->lebt = 1; s_n_spawn++;
            s->vor_wy = f->wpos[1];
            s->vor_h  = re15_collision_room_coll(g_room_rdt_ok ? &g_room_rdt : NULL, f->wpos[0], f->wpos[2], 0, 8, 0x100u);
            /* vor dem ersten Takt (Spawn ausserhalb des Spielschritts) ist die Weltlage noch nicht gerechnet */
            s->vor_gueltig = B == 36 && (f->wpos[0] | f->wpos[1] | f->wpos[2]) != 0;
            continue;
        }
        const int16_t h_jetzt = re15_collision_room_coll(g_room_rdt_ok ? &g_room_rdt : NULL, f->wpos[0], f->wpos[2],
                                                         0, 8, 0x100u);
        if (!s->landebild && B == 37) {
            s->landebild = s_bild; s_n_lande++;
            int ok = (int32_t)h_jetzt < (int32_t)f->wpos[1] &&
                     (!s->vor_gueltig || (int32_t)s->vor_wy <= (int32_t)s->vor_h) &&
                     f->drift_x == f->accel_x && f->drift_y == f->accel_y && f->drift_z == f->accel_z;
            if (!ok) { s_n_fehler++; s->fehler = 1; }
            if (laut) printf("  Platz %2d sub %u: Landung Bild %d, h=%d Weltlage-y %d (Vorbild %d bei h %d), "
                             "vel=(%d,%d,%d) = acc (%d,%d,%d) %s\n",
                             i, f->sub_index, s_bild, h_jetzt, f->wpos[1], s->vor_wy, s->vor_h, f->drift_x, f->drift_y,
                             f->drift_z, f->accel_x, f->accel_y, f->accel_z, ok ? "ok" : "FEHLER");
        } else if (s->landebild && !s->folgebild && B == 37) {
            if (f->drift_x != 0 || f->drift_z != 0) { s_n_fehler++; s->fehler = 1; }   /* senkrechter Fall */
        } else if (s->landebild && !s->folgebild) {
            s->folgebild = s_bild;
            int ok = B == 0 && f->flags == 0x13 && f->row_cursor == 1 && (f->frame == 6 || f->frame == 7) &&
                     (int32_t)s->h1e < (int32_t)f->wpos[1];
            if (!ok) { s_n_fehler++; s->fehler = 1; }
            if (laut) printf("  Platz %2d: Abschluss Bild %d (+%d) B=%u Flags %02x Zeile %d/%d Anim %d, slot+0x1e %d < "
                             "Weltlage-y %d %s\n", i, s_bild, s_bild - s->landebild, B, f->flags, f->row_cursor,
                             f->row_count, f->frame, s->h1e, f->wpos[1], ok ? "ok" : "FEHLER");
        }
        if (B == 37) s->h1e = (int16_t)u16le(f->row, 0x1e);
        s->vor_gueltig = (B == 36);
        s->vor_wy = f->wpos[1];
        s->vor_h  = h_jetzt;
    }
}

static int s_first_fail = 0, s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
    s_fails++; if (!s_first_fail) s_first_fail = (nr); } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* PIN 1 — der Nutzer-Weg: Super Redhawk (Waffe 7) toetet den Hund (ROOM11D0, RE2-KI). */
static void pin_redhawk_hund(void)
{
    printf("== pin 1: Redhawk -> Hund (ROOM11D0)\n");
    if (room_load(0x11D0, "STAGE1") != 0) { CHECK(10, 0, "ROOM11D0 fehlt"); return; }
    re15_actor_t *e = hund_arena(2000);
    if (!e) { CHECK(10, 0, "kein Hund 0x20 in ROOM11D0"); return; }
    const unsigned l0 = re15_esp_brocken_landungen(), a0 = re15_esp_brocken_abschluesse();
    int r = re15_player_weapon_fire(7);
    CHECK(10, r != 0 && e->state == 3 && e->sub_state_1 == 7,
          "Schuss Waffe 7 trifft und toetet: Treffer %d, st=%d (3), +5=%d (7 = Waffen-Id, Zeile 5 -> Router 0x80104610)",
          r, e->state, e->sub_state_1);
    spur_reset();
    int bei90 = -1, spitze = 0;
    for (int f = 1; f <= 900; f++) {
        frame(); spur_bild(1);
        int n = lebend(7, 0);
        if (n > spitze) spitze = n;
        if (f == 90) bei90 = n;
    }
    CHECK(11, s_n_spawn == 6 && spitze == 6,
          "Router-Blut wirft 6 Brocken (Raum-Id 7, FX(2,1|2) je Bild @0x80104664-7C bis Budget 18 leer): "
          "Entstehungen %d, Spitze %d", s_n_spawn, spitze);
    CHECK(12, s_n_lande == s_n_spawn && s_n_ende == s_n_spawn && s_n_fehler == 0,
          "jeder Brocken landet (B 36 -> 37 @0x80018828), schliesst ab (@0x800188a0) und endet am Terminator "
          "(@0x8001a40c): Landungen %d, Enden %d, Mechanik-Fehler %d", s_n_lande, s_n_ende, s_n_fehler);
    CHECK(13, re15_esp_brocken_landungen() - l0 == (unsigned)s_n_spawn &&
              re15_esp_brocken_abschluesse() - a0 == (unsigned)s_n_spawn,
          "Messschiene esp_brocken.c: %u Landungen, %u Abschluesse", re15_esp_brocken_landungen() - l0,
          re15_esp_brocken_abschluesse() - a0);
    CHECK(14, bei90 == 0 && lebend(7, 0) == 0,
          "NUTZER-BEFUND: 90 Bilder (3 s) nach dem Schuss lebende Fleisch-Plaetze %d (0), nach 900 Bildern %d (0; "
          "vorher 6/6, Dossier M2)", bei90, lebend(7, 0));
}

/* PIN 2 — andere Waffen, gleicher Gegner:
 *  (a) HE-Granate: Resolver-Art 2 an P = (x+300, y-500, z) (@0x800185a0-ac, wie probe_r34_reaktion
 *      explosion_bei) -> +0x5 = 9 -> Zeile 9 -> derselbe Router 0x80104610 (Zeilen {5,6,9,17,19},
 *      @0x801055CC) wie die Redhawk -> Brocken FX(2,1|2) je Bild; alle muessen enden.
 *  (b) Pistole (Waffe 3, Browning) bis zum Tod: Zeile 3 hat KEINEN Brocken-Router; nur die HURT-
 *      Bild-Bits (FX 1/2 @0x80102838-48, Zufall rand&3) koennen Brocken werfen; was entsteht, endet. */
static void pin_andere_waffen_hund(void)
{
    printf("== pin 2a: HE-Granate -> Hund (ROOM11D0)\n");
    if (room_load(0x11D0, "STAGE1") != 0) { CHECK(20, 0, "ROOM11D0 fehlt"); return; }
    re15_actor_t *e = hund_arena(8000);
    if (!e) { CHECK(20, 0, "kein Hund"); return; }
    {
        re15_attack_box_t box;
        box.x = e->x + 300; box.y = e->y - 500; box.z = e->z; box.radius = 500;
        re15_resolve_attack(&box, 2, -1);
    }
    CHECK(20, e->state == 3 && e->sub_state_1 == 9, "Explosion toetet: st=%d (3), +5=%d (9 -> Zeile 9)", e->state,
          e->sub_state_1);
    spur_reset();
    int spitze = 0;
    for (int k = 0; k < 300; k++) { frame(); spur_bild(0); int n = lebend(7, 0); if (n > spitze) spitze = n; }
    CHECK(21, s_n_spawn > 0 && lebend(7, 0) == 0 && s_n_ende == s_n_spawn && s_n_lande == s_n_spawn && s_n_fehler == 0,
          "Granate: %d Brocken (Spitze %d), nach 300 Bildern lebend %d (0), Landungen %d, Enden %d, Mechanik-Fehler %d",
          s_n_spawn, spitze, lebend(7, 0), s_n_lande, s_n_ende, s_n_fehler);

    printf("== pin 2b: Pistole (Waffe 3) -> Hund (ROOM11D0)\n");
    e = hund_arena(2000);
    if (!e) { CHECK(22, 0, "kein Hund"); return; }
    spur_reset();
    int schuesse = 0, f = 0;
    while (f < 3000 && !(e->state == 3 || e->state == 7)) {
        if ((f % 25) == 0) {
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->x = e->x - 2000; pl->z = e->z; pl->y = e->y; pl->rot_y = 0;   /* Aufstellung halten */
            if (re15_player_weapon_fire(3)) schuesse++;   /* Waffe 3 = Browning (ENT[3], Handler 0x800337BC) */
        }
        frame(); spur_bild(0); f++;
    }
    for (int k = 0; k < 300; k++) { frame(); spur_bild(0); }
    CHECK(22, (e->state == 3 || e->state == 7) && schuesse > 0, "Hund tot nach %d Pistolentreffern (st=%d)", schuesse, e->state);
    CHECK(23, lebend(7, 0) == 0 && s_n_ende == s_n_spawn && s_n_fehler == 0,
          "300 Bilder nach dem Tod: lebende Brocken %d (0), Entstehungen %d = Enden %d, Mechanik-Fehler %d",
          lebend(7, 0), s_n_spawn, s_n_ende, s_n_fehler);
}

/* PIN 3 — Zensus ueber ALLE Raeume, deren ESP-Bank die Id 7 traegt: jeder gueltige Sub (0..5,
 * Zensus M1) wird gespawnt (wie re2d_fx / re2z_gore_fx_ex: re15_esp_fx_spawn_rows), 300 Bilder
 * getaktet - jeder Brocken muss landen, abschliessen und enden. */
static void pin_alle_raeume(void)
{
    printf("== pin 3: alle Raeume mit Raum-Id 7\n");
    int raeume = 0, spawns = 0, haenger = 0, fehler = 0;
    for (int st = 1; st <= 6; st++) {
        for (int r = 0; r < 0x100; r++) {
            char path[512];
            int id = (st << 12) | (r << 4);
            snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", st, id);
            size_t n = 0; uint8_t *b = slurp(path, &n);
            if (!b) continue;
            re15_rdt_t rdt; re15_esp_t esp; memset(&esp, 0, sizeof esp);
            if (re15_rdt_parse(b, n, &rdt) != 0 || room_esp(b, n, &esp) != 0 || re15_esp_find_id(&esp, 7) < 0) {
                free(b); continue;
            }
            raeume++;
            g_room_rdt = rdt; g_room_rdt_ok = 1; g_scd.prop_count = 0;
            re15_esp_set_room_bank(&esp);
            int ei = re15_esp_find_id(&esp, 7);
            for (int sub = 0; sub < 6; sub++) {
                int nr = 0;
                const uint8_t *row = re15_esp_row_stream(&esp, ei, sub, 0, &nr);
                if (!row || nr != 2 || u16le(row, 2) != 36) continue;    /* nur die Brocken-Form (M1) */
                re15_esp_fx_reset(); spur_reset();
                re15_esp_fx_spawn_rows(&esp, 7, (uint8_t)sub, 0x1500, 0, -500, 0, 0, 0);
                spawns++;
                spur_bild(0);                                  /* Entstehung registrieren */
                for (int k = 0; k < 300; k++) { re15_esp_fx_tick(&esp); spur_bild(0); }
                if (lebend(7, 0) != 0) { haenger++; printf("  HAENGER ROOM%04X sub %d\n", id, sub); }
                if (s_n_fehler || s_n_lande != 1 || s_n_ende != 1) {
                    fehler++;
                    printf("  FEHLER ROOM%04X sub %d: Landungen %d Enden %d Fehler %d\n", id, sub, s_n_lande, s_n_ende, s_n_fehler);
                    if (fehler == 1) {                         /* den ersten Fall laut nachfahren */
                        re15_esp_fx_reset(); spur_reset();
                        re15_esp_fx_spawn_rows(&esp, 7, (uint8_t)sub, 0x1500, 0, -500, 0, 0, 0);
                        spur_bild(1);
                        for (int k = 0; k < 300; k++) { re15_esp_fx_tick(&esp); spur_bild(1); }
                    }
                }
            }
            re15_esp_set_room_bank(NULL);
            g_room_rdt_ok = 0;
            free(b);
        }
    }
    CHECK(30, raeume > 0 && spawns >= 6 * raeume, "%d Raeume mit Id 7, %d Brocken-Subs gespawnt", raeume, spawns);
    CHECK(31, haenger == 0 && fehler == 0,
          "nach 300 Bildern: Haenger %d (0), Mechanik-Fehler %d (0) — Landung/Abschluss/Terminator je Brocken",
          haenger, fehler);
}

static int teil_pin(void)
{
    pin_redhawk_hund();
    pin_andere_waffen_hund();
    pin_alle_raeume();
    printf("%s (%d Fehler, erster %d)\n", s_fails ? "ROT" : "GRUEN", s_fails, s_first_fail);
    return s_first_fail;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "pin") || !strcmp(teil, "alle")) return teil_pin();
    if (!strcmp(teil, "zensus")) return teil_zensus();
    if (!strcmp(teil, "haenger")) { s_mit_brocken = !(argc > 2 && !strcmp(argv[2], "vorher")); return teil_haenger(); }
    if (!strcmp(teil, "messung")) return teil_messung(0x11D0, "STAGE1", argc > 2 ? atoi(argv[2]) : 900);
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
