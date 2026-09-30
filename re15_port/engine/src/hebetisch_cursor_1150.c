/*
 * RE1.5 Rebuilt — HEBETISCH-CURSOR in Irons' Buero (ROOM1150/1151), Runde 34 Nacht, Spur B.
 *
 * Herleitung, Belege und alle Konstanten: include/re15_hebetisch_cursor.h.
 * Dossier: analysis/befunde_runde34_nacht/B_hebetisch.md (§9 Umsetzung).
 *
 * Ablauf: Aktion am Tisch -> sub04 startet wie bisher -> vor dem For 15 @0x0FC0 (1151 @0x0F9E)
 * haelt der Thread an, der Cursor erscheint (Cut 4, Plattform im Tisch, Kuppel zu, unten rechts).
 * Je VM-Takt: Abbruch (CROSS) -> Original-Aufraeumbytes von sub04; Druck (SQUARE) auf der Kuppel ->
 * Klick + For laeuft (Kuppel geht auf, Rest von sub04 unveraendert); Druck daneben -> "Nothing
 * happened."; D-Pad bewegt den Cursor.
 */
#include "re15_hebetisch_cursor.h"

#include <string.h>
#include "re15_scd.h"
#include "re15_room.h"       /* g_current_room_id */
#include "re15_camera.h"     /* re15_camera_build_view / re15_camera_compose_view_bone */
#include "re15_math.h"       /* re15_gte_divide */
#include "re15_msg.h"        /* re15_msg_install_text / _durations / _compute_duration */
#include "re15_audio.h"      /* re15_audio_re2_panel_se, RE15_PANEL_SE_KLICK */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#include "re15_engine.h"     /* g_engine.frame_count — nur fuer das Mess-Protokoll */
/* Eingebackener Cursor (tools/r34n_b/cursor_export.py), NUR PC: auf dem PSX-Ziel ist der
 * Cursor aus und die 38 KB bleiben aus dem Speicher. */
#include "gen/hebetisch_cursor.inc"
#endif

/* Der Untersuchungsweg des sce-1-Handlers LAB_80043084 (scd_vm.c) — dieselbe Deklaration wie in
 * aot_common.c. */
extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);

/* Pad-Woerter der SCD-VM (virtuell, game_step_common.c; unter der Pad-Sperre auf 0xf000 maskiert). */
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

unsigned g_re15_hebetisch_klick_zaehler   = 0;
unsigned g_re15_hebetisch_text_zaehler    = 0;
unsigned g_re15_hebetisch_abbruch_zaehler = 0;

/* ROOM1150.RDT @0x0FB2..@0x0FC5 (ROOM1151 @0x0F90..@0x0FA3): Cut_chg 4, Pos_set, Sleep 5, For 15. */
static const uint8_t k_sig[RE15_HC_SIG_LEN] = {
    0x29, 0x04,                                          /* @0x0FB2 Cut_chg 4                   */
    0x32, 0x00, 0x24, 0xaf, 0xcf, 0xfe, 0xcc, 0xbb,      /* @0x0FB4 Pos_set (-20700,-305,-17460) */
    0x09, 0x0a, 0x05, 0x00,                              /* @0x0FBC Sleep 5                     */
    0x0d, 0x00, 0x18, 0x00, 0x0f, 0x00                   /* @0x0FC0 For 15 (Kuppel auf) = HALT  */
};

/* ROOM1150.RDT @0x109A..@0x10B5 (ROOM1151 @0x1078..): die Aufraeumbytes, mit denen sub04 endet. */
static const uint8_t k_aufraeum[RE15_HC_AUFRAEUM_LEN] = {
    0x2e, 0x03, 0x00,                                    /* @0x109A Work_set(3,0) Plattform     */
    0x00,                                                /* @0x109D Nop                         */
    0x32, 0x00, 0x24, 0xaf, 0x00, 0xb1, 0xcc, 0xbb,      /* @0x109E Pos_set (-20700,-20224,-17460) */
    0x22, 0x05, 0x00, 0x00,                              /* @0x10A6 Set(5,0,0)                  */
    0x22, 0x02, 0x00, 0x00,                              /* @0x10AA Set(2,0,0) Spieler frei     */
    0x22, 0x02, 0x02, 0x00,                              /* @0x10AE Set(2,2,0) KI frei          */
    0x2a,                                                /* @0x10B2 Cut_old                     */
    0x00,                                                /* @0x10B3 Nop                         */
    0x01, 0x00                                           /* @0x10B4 Evt_end                     */
};

/* "Nothing happened." — NUTZER-VORGABE (Wortlaut), Form und Glyphen aus RE1.5 (Kopf-Kommentar). */
static const uint8_t k_text[] = {
    0x04, 0x02,                                          /* Kopf: ROOM1000.RDT @0x00D22         */
    /* N    o    t    h    i    n    g   — ROOM1000.RDT @0x00D24 ("Nothing unusual.")          */
    0x2a,0x4b,0x50,0x44,0x45,0x4a,0x43,
    /* _    h    a    p    p    e    n    e    d   — ROOM3001.RDT @0x0199D ("What happened here?") */
    0x00,0x44,0x3d,0x4c,0x4c,0x41,0x4a,0x41,0x40,
    0x57,                                                /* '.' ROOM1000.RDT @0x00D33           */
    0x01, 0x00                                           /* Ende: ROOM1000.RDT @0x00D34         */
};

/* Konvexe Huelle von Deckel + Podest unter Cut 4 (320x240, im Uhrzeigersinn): Engine-Projektion
 * (probe_r34n_b_messung, B_belege/probe_r34n_b_messung.txt) von Prop 1 MD1 @0x138D4 + Prop 2
 * @0x13B88 + Podest aus Prop 0 @0x11E40 unter Cut 4 @0x00E0 bei Plattform @0x0FB4, Deckel zu
 * (@0x0E22/@0x0E44). ROOM1151 Zahl fuer Zahl gleich. Riegel unit_r34n_b_kuppel rechnet nach. */
static const int16_t k_kuppel[2 * RE15_HC_KUPPEL_N] = {
    151, 171,  163, 161,  180, 151,  206, 148,  235, 150,  248, 156,
    267, 170,  265, 190,  261, 204,  214, 210,  164, 204,  151, 191
};

static int      s_zustand = RE15_HC_AUS;
static int      s_thread  = -1;          /* Slot des vom Spieler gestarteten sub04 */
static int32_t  s_x = RE15_HC_START_X, s_z = RE15_HC_START_Z;
static unsigned s_sitzung = 0;

/* Signatur-Cache, gebunden an (Raum, raw, raw_size) und in install() geleert (Auflage 2). */
static const uint8_t *s_cache_raw  = NULL;
static int            s_cache_size = 0;
static unsigned       s_cache_raum = 0;
static const uint8_t *s_cache_halt = NULL;

static int ist_irons_buero(unsigned room)
{
    return room == RE15_HC_RAUM_JOHN || room == RE15_HC_RAUM_ELZA;
}

/* ------------------------------------------------------------------ Mess-Protokoll (PC) --- */
static void protokoll(const char *was)
{
#ifdef RE15_PLATFORM_PC
    static int an = -1;
    static FILE *f = NULL;
    if (an < 0) {
        const char *e = getenv("RE15_HEBETISCH_CURSOR_LOG");
        an = (e && *e) ? 1 : 0;
        if (an) { f = fopen(e, "w"); if (!f) an = 0; }
    }
    if (!an) return;
    int sx = 0, sy = 0;
    re15_hebetisch_cursor_heisspunkt(s_x, s_z, &sx, &sy);
    fprintf(f, "F%u raum=%04X %s cursor=%d x=%d z=%d sx=%d sy=%d treffer=%d cam=%u klick=%u text=%u "
               "abbruch=%u\n",
            (unsigned)g_engine.frame_count, g_current_room_id, was, s_zustand, (int)s_x, (int)s_z,
            sx, sy, re15_hebetisch_cursor_in_kuppel(sx, sy), (unsigned)g_scd.cam_id,
            g_re15_hebetisch_klick_zaehler, g_re15_hebetisch_text_zaehler,
            g_re15_hebetisch_abbruch_zaehler);
    fflush(f);
#else
    (void)was;
#endif
}

/* ------------------------------------------------------------------ Abbildung ------------- */
static re15_camera_view_t s_sicht;
static int                s_sicht_ok = 0;

static void sicht_bauen(void)
{
    if (s_sicht_ok) return;
    re15_camera_cut_t c;
    memset(&c, 0, sizeof c);
    c.flag = 0;
    c.fov = (uint16_t)RE15_HC_KAM_FOV;
    c.pos_x = RE15_HC_KAM_POS_X; c.pos_y = RE15_HC_KAM_POS_Y; c.pos_z = RE15_HC_KAM_POS_Z;
    c.target_x = RE15_HC_KAM_TGT_X; c.target_y = RE15_HC_KAM_TGT_Y; c.target_z = RE15_HC_KAM_TGT_Z;
    if (re15_camera_build_view(&c, &s_sicht) == 0) s_sicht_ok = 1;
}

void re15_hebetisch_cursor_matrix(int32_t x, int32_t z, int32_t rot[9], int32_t trans[3], int *h)
{
    static const int32_t k_ident[9] = { 4096, 0, 0, 0, 4096, 0, 0, 0, 4096 };  /* pc[16..21] = 0 */
    sicht_bauen();
    int32_t lage[3] = { x, RE15_HC_OBJ_Y + RE15_HC_TYP4_ANHEBUNG, z };
    re15_camera_compose_view_bone(&s_sicht, k_ident, lage, rot, trans);
    if (h) *h = (int)s_sicht.fov_screen_dist;
}

/* Byte-true GTE RTPS wie main.c PROJECT_VERT: IR1/IR2 s16-gesaettigt, SZ3 u16, n = UNR-Division. */
int re15_hebetisch_cursor_projiziere(const int32_t rot[9], const int32_t trans[3], int h,
                                     int32_t mx, int32_t my, int32_t mz,
                                     int *sx, int *sy, int32_t *vz)
{
    int32_t x = (int32_t)(((int64_t)mx * rot[0] + (int64_t)my * rot[1] + (int64_t)mz * rot[2]) >> 12) + trans[0];
    int32_t y = (int32_t)(((int64_t)mx * rot[3] + (int64_t)my * rot[4] + (int64_t)mz * rot[5]) >> 12) + trans[1];
    int32_t z = (int32_t)(((int64_t)mx * rot[6] + (int64_t)my * rot[7] + (int64_t)mz * rot[8]) >> 12) + trans[2];
    if (vz) *vz = z;
    if (z < 64) return 0;                                   /* H28 Nahebene wie main.c */
    int32_t ir1 = x > 0x7FFF ? 0x7FFF : (x < -0x8000 ? -0x8000 : x);
    int32_t ir2 = y > 0x7FFF ? 0x7FFF : (y < -0x8000 ? -0x8000 : y);
    uint32_t sz3 = z > 0xFFFF ? 0xFFFFu : (uint32_t)z;
    uint32_t n = re15_gte_divide((uint32_t)h, sz3);
    if (sx) *sx = 160 + (int)(((int64_t)ir1 * (int64_t)n) >> 16);   /* OFX = 160 (main.c cx) */
    if (sy) *sy = 120 + (int)(((int64_t)ir2 * (int64_t)n) >> 16);   /* OFY = 120 (main.c cy) */
    return 1;
}

void re15_hebetisch_cursor_heisspunkt(int32_t x, int32_t z, int *sx, int *sy)
{
    int32_t rot[9], trans[3];
    int h = 0;
    re15_hebetisch_cursor_matrix(x, z, rot, trans, &h);
    if (!re15_hebetisch_cursor_projiziere(rot, trans, h, 0, RE15_HC_KREUZ_Y, 0, sx, sy, NULL)) {
        if (sx) *sx = -10000;                                /* hinter der Kamera: nirgends */
        if (sy) *sy = -10000;
    }
}

int re15_hebetisch_cursor_in_kuppel(int sx, int sy)
{
    for (int i = 0; i < RE15_HC_KUPPEL_N; i++) {
        int j = (i + 1) % RE15_HC_KUPPEL_N;
        long ax = k_kuppel[2 * i], ay = k_kuppel[2 * i + 1];
        long bx = k_kuppel[2 * j], by = k_kuppel[2 * j + 1];
        long kreuz = (bx - ax) * ((long)sy - ay) - (by - ay) * ((long)sx - ax);
        if (kreuz < 0) return 0;          /* rechts einer Kante (Umlauf im Bild im Uhrzeigersinn) */
    }
    return 1;
}

const int16_t *re15_hebetisch_cursor_kuppel(void) { return k_kuppel; }

const uint8_t *re15_hebetisch_cursor_text(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_text;
    return k_text;
}

const uint8_t *re15_hebetisch_cursor_md1_bytes(int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (out_size) *out_size = (int)sizeof re15_hc_cursor_md1;
    return re15_hc_cursor_md1;
#else
    if (out_size) *out_size = 0;
    return NULL;
#endif
}

const uint8_t *re15_hebetisch_cursor_tim_bytes(int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (out_size) *out_size = (int)sizeof re15_hc_cursor_tim;
    return re15_hc_cursor_tim;
#else
    if (out_size) *out_size = 0;
    return NULL;
#endif
}

/* ------------------------------------------------------------------ Zustand ---------------- */
void re15_hebetisch_cursor_install(uint16_t room_id)
{
    (void)room_id;
    s_zustand = RE15_HC_AUS;
    s_thread = -1;
    s_x = RE15_HC_START_X;
    s_z = RE15_HC_START_Z;
    s_cache_raw = NULL; s_cache_size = 0; s_cache_raum = 0; s_cache_halt = NULL;
}

void re15_hebetisch_cursor_aktion(uint8_t ereignis, int thread_slot)
{
#ifdef RE15_PLATFORM_PC
    if (!ist_irons_buero(g_current_room_id)) return;
    if (ereignis != RE15_HC_EREIGNIS) return;
    if (thread_slot < 0 || thread_slot >= SCD_THREAD_COUNT) return;   /* nicht gestartet */
    s_zustand = RE15_HC_VERLANGT;
    s_thread = thread_slot;
    protokoll("verlangt");
#else
    (void)ereignis; (void)thread_slot;   /* PSX: kein Zeichner -> kein Halt */
#endif
}

static const uint8_t *halt_pc(const uint8_t *raw, int raw_size)
{
    if (raw != s_cache_raw || raw_size != s_cache_size || g_current_room_id != s_cache_raum) {
        s_cache_raw = raw; s_cache_size = raw_size; s_cache_raum = g_current_room_id;
        s_cache_halt = NULL;
        if (raw && ist_irons_buero(g_current_room_id)) {
            for (int i = 0; i + RE15_HC_SIG_LEN <= raw_size; i++) {
                if (raw[i] != k_sig[0] || memcmp(raw + i, k_sig, sizeof k_sig) != 0) continue;
                s_cache_halt = raw + i + RE15_HC_HALT_IN_SIG;
                break;                                  /* je Raum genau ein Treffer (Zensus) */
            }
        }
    }
    return s_cache_halt;
}

/* Defensiv (Auflage 2): der Cursor lebt nur, solange der armierte Thread wirklich auf dem Halt steht. */
static int thread_steht_auf_halt(void)
{
    if (s_thread < 0 || s_thread >= SCD_THREAD_COUNT || !s_cache_halt) return 0;
    const scd_thread_t *t = &g_scd.threads[s_thread];
    return t->active && !t->kill_pending && t->pc == s_cache_halt;
}

static int takt(scd_thread_t *t, const uint8_t *halt, const uint8_t *raw, int raw_size)
{
    const uint16_t edge = g_scd_pad_edge;
    const uint16_t held = g_scd_pad_held;

    /* 1. ABBRUCH -> Original-Aufraeumbytes von sub04 (Anker geprueft, sonst kein Abbruch). */
    if (edge & RE15_HC_TASTE_ABBRUCH) {
        const uint8_t *ziel = halt + RE15_HC_AUFRAEUM_ABSTAND;
        if (ziel >= raw && ziel + RE15_HC_AUFRAEUM_LEN <= raw + raw_size &&
            memcmp(ziel, k_aufraeum, sizeof k_aufraeum) == 0) {
            t->pc = ziel;
            s_zustand = RE15_HC_AUS;
            g_re15_hebetisch_abbruch_zaehler++;
            protokoll("abbruch");
            return RE15_HC_SPRUNG;
        }
    }

    /* 2. DRUCK — vor der Bewegung: getroffen wird, was im Bild steht. */
    if (edge & RE15_HC_TASTE_DRUCK) {
        int sx = 0, sy = 0;
        re15_hebetisch_cursor_heisspunkt(s_x, s_z, &sx, &sy);
        if (re15_hebetisch_cursor_in_kuppel(sx, sy)) {
            g_re15_hebetisch_klick_zaehler++;
            re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);    /* RE2 ROOM2130.RDT @0x01192 */
            s_zustand = RE15_HC_AUS;
            protokoll("kuppel");
            return RE15_HC_WEITER;                           /* For 15 laeuft jetzt: Kuppel auf */
        }
        /* Fehldruck: "Nothing happened." — Vorbild ROOM4020 sub12 `2b 01 ff ff` @0x00B44. Text-Id
         * unmittelbar vor dem Oeffnen installieren (die Tabelle ist raumlokal). */
        re15_msg_install_text((unsigned char)RE15_HC_TEXT_ID, k_text, sizeof k_text);
        {
            int d = re15_msg_compute_duration(k_text, sizeof k_text, 0);
            if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)RE15_HC_TEXT_ID, d);
        }
        re15_scd_show_message((uint8_t)RE15_HC_TEXT_ID, RE15_HC_TEXT_MASKE);
        g_re15_hebetisch_text_zaehler++;
        protokoll("nichts");
        return RE15_HC_HALT;
    }

    /* 3. BEWEGEN — vier unabhaengige Abfragen wie ROOM11F0 sub01 (@0x01098/@0x010B0/@0x010C8/
     *    @0x010E0), je 200 (sub02..05), keine Randgrenze (Add_speed LAB_80040f40 addiert nur). */
    if (held & RE15_HC_TASTE_UP)    s_z += RE15_HC_SCHRITT;
    if (held & RE15_HC_TASTE_DOWN)  s_z -= RE15_HC_SCHRITT;
    if (held & RE15_HC_TASTE_RIGHT) s_x += RE15_HC_SCHRITT;
    if (held & RE15_HC_TASTE_LEFT)  s_x -= RE15_HC_SCHRITT;
    protokoll("takt");
    return RE15_HC_HALT;
}

int re15_hebetisch_cursor_for(scd_thread_t *t, const uint8_t *raw, int raw_size)
{
    if (s_zustand == RE15_HC_AUS) return RE15_HC_WEITER;          /* jeder andere Raum/Zeitpunkt */
    if (!t || !raw || s_thread < 0 || t != &g_scd.threads[s_thread]) return RE15_HC_WEITER;
    const uint8_t *halt = halt_pc(raw, raw_size);
    if (!halt || t->pc != halt) return RE15_HC_WEITER;
    /* Auflage 7: am ANKER pruefen, nicht nur am Offset — die 20 Signaturbytes ab Halt-14. */
    if (halt - RE15_HC_HALT_IN_SIG < raw ||
        memcmp(halt - RE15_HC_HALT_IN_SIG, k_sig, sizeof k_sig) != 0) return RE15_HC_WEITER;
    if (s_zustand == RE15_HC_VERLANGT) {
        s_zustand = RE15_HC_AKTIV;
        s_x = RE15_HC_START_X;
        s_z = RE15_HC_START_Z;
        s_sitzung++;
        protokoll("aktiv");
        return RE15_HC_HALT;                                        /* erstes Bild: nur zeigen */
    }
    return takt(t, halt, raw, raw_size);
}

int re15_hebetisch_cursor_sicht(int32_t *x11f0, int32_t *z11f0)
{
    if (s_zustand != RE15_HC_AKTIV) return 0;
    if (!thread_steht_auf_halt()) {           /* Raumwechsel/Thread weg: nie einen toten Cursor */
        s_zustand = RE15_HC_AUS;
        protokoll("verwaist");
        return 0;
    }
    if (x11f0) *x11f0 = s_x;
    if (z11f0) *z11f0 = s_z;
    return 1;
}

int      re15_hebetisch_cursor_zustand(void) { return s_zustand; }
unsigned re15_hebetisch_cursor_sitzung(void) { return s_sitzung; }
