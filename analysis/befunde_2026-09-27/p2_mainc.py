import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elza_ed import rep

M = "re15_port/platform/pc/main.c"

rep(M, [

# ---------------------------------------------------------------- Helper
(
"""    return re15_pc_read_any(rel, size);
}
""",
"""    return re15_pc_read_any(rel, size);
}

/* ═══ SPIELER-TEILDATEI JE CHARAKTER ═══════════════════════════════════════════════
 * Das Original laedt den Spieler NICHT aus vier Einzeldateien, sondern aus EINEM
 * Container: FUN_800314b0 liest DAT_800ACA5C @0x800314d4, `sll v0,v0,1` @0x800314d8,
 * `lhu a0,0x80073f70[v0]` (16 u16 = CD-Index 60..75) und laedt CD-Datei 60+Index =
 * PL0<Index>.PLD. Die vier Teile stehen in dessen Verzeichnis (u32 @0 = Tabellenanfang,
 * danach 4 u32: EDD, EMR, MD1, TIM) — genau die Regel, die re15_pld_part fuehrt.
 * Dieselbe Bauform haben die Waffen-Container: FUN_80036b68 laedt CD-Datei
 * base_table[charid] + Waffen-Id (Tabelle @0x800741e8, `lbu` @0x80036df8) und nimmt
 * dir[0] -> 0x800acbc8 @0x80036be4 (EDD) und dir[1] -> 0x800acbc4 @0x80036c04 (EMR).
 *
 * WARUM DER UMWEG UEBER DIE TEILDATEI ZUERST: Leons Satz liegt unter shared_assets als
 * vorextrahierte PL00.EDD/.EMR/.MD1/.TIM, Elzas Satz NICHT vollstaendig — PL04.EDD,
 * PL04W01.EDD und PL04W01.EMR fehlen im entpackten Baum. Beides ist derselbe Inhalt:
 * fuer ALLE 13 vorhandenen Teildateien ist der Containerschnitt byte-gleich mit der
 * ausgelieferten Datei (sha256, eigene Messung 2026-09-27; z.B. PL00.EDD 3160 B
 * 26198800…, PL00W03.EMR 19848 B 490debdb…). Erst die Datei, dann der Schnitt heisst
 * also: Leon laedt weiterhin exakt dieselben Bytes wie bisher, und Elza bekommt die
 * fehlenden drei Teile aus ihrem eigenen Container statt gar keine.
 *
 * ⛔ Der Rueckgabezeiger kann IN den Containerpuffer zeigen — nicht freigeben. Das ist
 * dieselbe Lebensdauer-Regel wie bei allen Spieler-Assets hier (skel.keyframe_data
 * aliast den EMR-Puffer; keiner davon wird je freigegeben). */
static uint8_t *pc_read_pl_part(const char *stem, int part, int *out_size)
{
    static const char *const k_ext[4] = { "EDD", "EMR", "MD1", "TIM" };
    char p[64];
    int sz = 0;
    *out_size = 0;
    if (part < 0 || part > 3) return NULL;
    snprintf(p, sizeof p, "PLD/%s.%s", stem, k_ext[part]);
    uint8_t *b = pc_read_shared(p, &sz);
    if (b && sz > 0) { *out_size = sz; return b; }
    free(b);
    /* Container: PLxx -> .PLD, PLxxWyy -> .PLW (identische Verzeichnisform). */
    snprintf(p, sizeof p, "PLD/%s.%s", stem, strchr(stem, 'W') ? "PLW" : "PLD");
    int csz = 0;
    uint8_t *cb = pc_read_shared(p, &csz);   /* bleibt resident, der Schnitt aliast ihn */
    unsigned long off = 0, len = 0;
    if (!cb || !re15_pld_part(cb, (long)csz, part, &off, &len) || len == 0) {
        fprintf(stderr, "[pl-part] %s Teil %s nicht auffindbar\\n", stem, k_ext[part]);
        return NULL;
    }
    fprintf(stderr, "[pl-part] %s.%s aus %s geschnitten (+%lu, %lu B)\\n",
            stem, k_ext[part], p, off, len);
    *out_size = (int)len;
    return cb + off;
}
"""
),

# ---------------------------------------------------------------- CORE-Bank NEW GAME
(
"""                    { extern void re15_audio_prime_core(int idx); re15_audio_prime_core(ch ? 4 : 0); }""",
"""                    { extern void re15_audio_prime_core(int idx);
                      re15_audio_prime_core(g_gameflow.character); }"""
),

# ---------------------------------------------------------------- CORE-Bank CONTINUE
(
"""                        { extern void re15_audio_prime_core(int idx);
                          re15_audio_prime_core(s_resume_sd.character ? 4 : 0); }""",
"""                        { extern void re15_audio_prime_core(int idx);
                          re15_audio_prime_core(g_gameflow.character); }"""
),

# ---------------------------------------------------------------- Spieler-TIM
(
"""    int tim_size = 0;
    uint8_t *tim_buf = pc_read_shared("PLD/PL00.TIM", &tim_size);""",
"""    /* ═══ SPIELER-FAMILIE = DER CHARAKTER ═══════════════════════════════════════
     * PLD-Datei = 60 + DAT_800ACA5C (Tabelle 0x80073f70, Leser FUN_800314b0
     * @0x800314d4): Leon 0 -> 60 = PL00.PLD, Elza 4 -> 64 = PL04.PLD. Die Datenlage
     * bestaetigt den Schnitt unabhaengig — in shared_assets/PSX/PLD/ haben genau PL00
     * und PL04 den vollen Waffensatz W00..W14 (21 PLW je Familie). */
    char pl_fam[8];
    snprintf(pl_fam, sizeof pl_fam, "PL%02X", (unsigned)(g_gameflow.character & 0x0F));
    fprintf(stderr, "[pl] Spieler-Familie %s (character=%d, Elza-Bit=%d)\\n",
            pl_fam, g_gameflow.character, re15_char_variant());
    int tim_size = 0;
    char pl_part_name[16];
    snprintf(pl_part_name, sizeof pl_part_name, "%s", pl_fam);
    uint8_t *tim_buf = pc_read_pl_part(pl_part_name, RE15_PLD_TIM, &tim_size);"""
),

# ---------------------------------------------------------------- Spieler-MD1
(
"""    int md1_size = 0;
    uint8_t *md1_buf = pc_read_shared("PLD/PL00.MD1", &md1_size);""",
"""    int md1_size = 0;
    uint8_t *md1_buf = pc_read_pl_part(pl_part_name, RE15_PLD_MD1, &md1_size);"""
),

# ---------------------------------------------------------------- W01
(
"""    int w01_edd_size = 0, w01_emr_size = 0;
    uint8_t *w01_edd_buf = pc_read_shared("PLD/PL00W01.EDD", &w01_edd_size);
    uint8_t *w01_emr_buf = pc_read_shared("PLD/PL00W01.EMR", &w01_emr_size);""",
"""    int w01_edd_size = 0, w01_emr_size = 0;
    char pl_w01[16], pl_w03[16];
    snprintf(pl_w01, sizeof pl_w01, "%sW01", pl_fam);
    snprintf(pl_w03, sizeof pl_w03, "%sW03", pl_fam);
    uint8_t *w01_edd_buf = pc_read_pl_part(pl_w01, RE15_PLD_EDD, &w01_edd_size);
    uint8_t *w01_emr_buf = pc_read_pl_part(pl_w01, RE15_PLD_EMR, &w01_emr_size);"""
),
(
"""            fprintf(stderr, "[w01] PL00W01 weapon-track: %d bones, %d clips, %d kf\\n",
                    w01_skel_raw.bone_count, w01_anim.clip_count, w01_skel_raw.keyframe_count);""",
"""            fprintf(stderr, "[w01] %s weapon-track: %d bones, %d clips, %d kf\\n",
                    pl_w01, w01_skel_raw.bone_count, w01_anim.clip_count,
                    w01_skel_raw.keyframe_count);"""
),

# ---------------------------------------------------------------- W03
(
"""    int w03_edd_size = 0, w03_emr_size = 0;
    uint8_t *w03_edd_buf = pc_read_shared("PLD/PL00W03.EDD", &w03_edd_size);
    uint8_t *w03_emr_buf = pc_read_shared("PLD/PL00W03.EMR", &w03_emr_size);""",
"""    int w03_edd_size = 0, w03_emr_size = 0;
    uint8_t *w03_edd_buf = pc_read_pl_part(pl_w03, RE15_PLD_EDD, &w03_edd_size);
    uint8_t *w03_emr_buf = pc_read_pl_part(pl_w03, RE15_PLD_EMR, &w03_emr_size);"""
),
(
"""            fprintf(stderr, "[w03] PL00W03 gun-track: %d bones, %d clips, %d kf\\n",
                    w03_skel_raw.bone_count, w03_anim.clip_count, w03_skel_raw.keyframe_count);""",
"""            fprintf(stderr, "[w03] %s gun-track: %d bones, %d clips, %d kf\\n",
                    pl_w03, w03_skel_raw.bone_count, w03_anim.clip_count,
                    w03_skel_raw.keyframe_count);"""
),

# ---------------------------------------------------------------- Waffenfamilie
(
"""    const char *wpn_fam = (g_gameflow.character == 0) ? "PL00" : "PL04";  /* base_table charid split */""",
"""    /* base_table charid split: @0x800741e8 = {76,76,76,76,97,97,97,97,…}, Index
     * DAT_800ACA5C (`lbu` @0x80036df8). Charid 0-3 -> Basis 76 = PL00-Familie,
     * 4-7 -> Basis 97 = PL04-Familie. Der Test ist also Bit 2, genau wie ueberall
     * sonst — und die Familie ist dieselbe wie die des Koerpers (pl_fam). */
    const char *wpn_fam = pl_fam;"""
),

# ---------------------------------------------------------------- Basis-EDD/EMR
(
"""    int edd_size = 0, emr_size = 0;
    uint8_t *edd_buf = pc_read_shared("PLD/PL00.EDD", &edd_size);
    uint8_t *emr_buf = pc_read_shared("PLD/PL00.EMR", &emr_size);""",
"""    int edd_size = 0, emr_size = 0;
    uint8_t *edd_buf = pc_read_pl_part(pl_part_name, RE15_PLD_EDD, &edd_size);
    uint8_t *emr_buf = pc_read_pl_part(pl_part_name, RE15_PLD_EMR, &emr_size);"""
),
(
"""        fprintf(stderr, "[skel] PL00: %d bones, %d clips, %d keyframes\\n",
                skel.bone_count, anim.clip_count, skel.keyframe_count);""",
"""        fprintf(stderr, "[skel] %s: %d bones, %d clips, %d keyframes\\n",
                pl_fam, skel.bone_count, anim.clip_count, skel.keyframe_count);"""
),

# ---------------------------------------------------------------- Wund-LUT
(
"""                    int chr = (g_gameflow.character == 0) ? 0 : 1;""",
"""                    int chr = (g_gameflow.character & 4) ? 1 : 0;   /* @0x80104008 andi 0x4 */"""
),

# ---------------------------------------------------------------- Equip-Familie
(
"""                    const char *fam = (g_gameflow.character == 0) ? "PL00" : "PL04";""",
"""                    char fam[8];   /* base_table @0x800741e8, Index aca5c (`lbu` @0x80036df8) */
                    snprintf(fam, sizeof fam, "PL%02X", (unsigned)(g_gameflow.character & 0x0F));"""
),

# ---------------------------------------------------------------- Montage-Effekt (2 Stellen)
(
"""            re15_montage_fx_set_active(g_current_room_id == 0x1240);""",
"""            /* Die Vorspann-Montage gibt es in BEIDEN Varianten (ROOM1240 = Leon,
             * ROOM1241 = Elza; die beiden RDT unterscheiden sich in genau einem Byte,
             * dem Tuerziel @0x0531 = 0x17 bzw. 0x03). Die Variante ist die niedrigste
             * Hex-Ziffer, also gegen die BASIS vergleichen. */
            re15_montage_fx_set_active(RE15_ROOM_BASE(g_current_room_id) == 0x1240);"""
),
(
"""                if (g_current_room_id == 0x1240 && !re15_montage_fx_stock()) {""",
"""                if (RE15_ROOM_BASE(g_current_room_id) == 0x1240 && !re15_montage_fx_stock()) {"""
),
])

S = "re15_port/engine/src/scd_vm.c"
rep(S, [
(
"""#include "re15_room.h"       /* g_current_room_id (save-point room match) */""",
"""#include "re15_room.h"       /* g_current_room_id (save-point room match) */
#include "re15_gameflow.h"   /* RE15_ROOM_BASE — Raum-Id ohne Spielervariante */"""
),
(
"""    static const unsigned ft[] = { 0x1170, 0x1240 };
    for (unsigned k = 0; k < sizeof(ft)/sizeof(ft[0]); k++)
        if (ft[k] == room_id) return 1;
    return 0;""",
"""    /* Basis-Ids OHNE Spielervariante: die niedrigste Hex-Ziffer ist die RDT-Variante
     * (Leon 0 / Elza 1, @0x800397e4 `srl a0,a0,31` + @0x800397ec `addu`), nicht ein
     * anderer Raum. ROOM1241 ist dieselbe Vorspann-Montage wie ROOM1240 — gleicher
     * Aufbau, 9 Cuts, eigene Message-Ids in sub02 — also gilt dieselbe Darstellung. */
    static const unsigned ft[] = { 0x1170, 0x1240 };
    for (unsigned k = 0; k < sizeof(ft)/sizeof(ft[0]); k++)
        if (ft[k] == RE15_ROOM_BASE(room_id)) return 1;
    return 0;"""
),
])
