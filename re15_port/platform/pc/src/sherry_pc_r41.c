/*
 * RE1.5 Rebuilt - Runde 41 Spur K "stage3szenen_b" (PC): Spielfigur-Wechsel mit SKELETT, ANIMATION und WAFFEN-BANK.
 *
 * Original (selbst disassembliert, Belege include/re15_sherry_r41.h + Dossier K1.5): der Raumlader FUN_800396fc ruft
 * bei neuem Modell-Nibble @0x80039788 FUN_800314b0. Der bindet ALLE VIER Teile der PLD neu - EDD (+0x16c) @0x8003154c,
 * EMR (+0x84) @0x80031578, MD1 @0x800315a0, TIM @0x800315b0 - baut das Skelett aus dem neuen EMR auf
 * (jal 0x8001e56c @0x80031614, jal 0x8001e5b0 @0x80031624), setzt die Waffe (@0x80031690; Modell 0x0F -> 0
 * @0x800316c4), loescht die Wunden (FUN_80037c1c @0x800316c8), laedt die CORE-Klangbank des Modells
 * (FUN_800440c4 @0x800316e8) und die Waffen-Bank FUN_80036b68 @0x800316f0 (Datei @0x800741e8[Modell] + Waffe;
 * W-EDD +0x174 @0x80036be4, W-EMR +0x170 @0x80036c04; Waffe 0 -> kein Hand-Mesh @0x80036c08).
 *
 * Port vorher: pc_sync_player_model (main.c) tauschte NUR MD1 + TIM - fuer die Weste richtig (PL00/PL01/PL02 und
 * PL04..PL07 tragen bytegleiche EDD/EMR, md5 im Dossier), fuer Sherry (PL0F: Kind-Skelett, eigene 24 Clips, eigene
 * W00-Bank) falsch: ihre Teile hingen am Erwachsenen-Skelett Elzas, in der Hand Elzas Messer.
 *
 * Hier: GLEICHE Familie (Waffen-Basis @0x800741e8[Modell]) -> EDD/EMR/W-Banken bleiben (bytegleich, Westen-Weg
 * unveraendert). ANDERE Familie -> Koerper (EDD+EMR der neuen PLD) und Waffen-Banken der Familie neu binden; die
 * Boot-Familie wird beim Verlassen gesichert und beim Zurueckkehren (3091 Ende -> Elza) wiederhergestellt.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_sherry_r41.h"
#include "sherry_pc_r41.h"
#include "asset_root_pc.h"

#define K_WPN_MAX 21

/* Waffen-Datei-Basis je Modell, u16 @0x800741e8 (Bytes 4c 00 x4, 61 00 x4, 4c 00 x7, 76 00): 76 = PL00W00,
 * 97 = PL04W00, 118 = PL0FW00 (CD-Dateitabelle @0x8006f440 Eintrag 75/96/117: 22960/22756/14696 B). */
static const uint16_t k_waffen_basis[16] = { 76,76,76,76, 97,97,97,97, 76,76,76,76, 76,76,76, 118 };

static re15_sherry_pc_r41_baenke_t s_b;
static int s_angemeldet = 0;
static int s_familie = -1, s_boot_familie = -1;

/* Sicherung der Boot-Familie (Elza bzw. Leon) */
static int s_gesichert = 0;
static re15_emd_skeleton_t  s_sv_pl00_skel, s_sv_w01_skel;
static re15_emd_animation_t s_sv_pl00_anim, s_sv_w01_anim;
static re15_emd_animation_t s_sv_wpn_anim[K_WPN_MAX];
static re15_emd_skeleton_t  s_sv_wpn_skel[K_WPN_MAX];
static int s_sv_bank_ok[K_WPN_MAX], s_sv_md1_ok[K_WPN_MAX];

/* fremde Familie (Sherry): eigene Kopien, EDD/EMR aliasen sie */
static uint8_t *s_koerper_buf = NULL;
static uint8_t *s_w00_buf = NULL;
static re15_emd_skeleton_t  s_fr_skel, s_w00_raw;
static re15_emd_animation_t s_fr_anim, s_w00_anim, s_an_tmp;

static const char *familie_praefix(int basis)
{
    return (basis == 97) ? "PL04" : (basis == 118) ? "PL0F" : "PL00";
}

void re15_sherry_pc_r41_anmelden(const re15_sherry_pc_r41_baenke_t *b, int modell)
{
    if (!b) return;
    s_b = *b;
    s_angemeldet = 1;
    if (s_b.wpn_max > K_WPN_MAX) s_b.wpn_max = K_WPN_MAX;
    s_familie = s_boot_familie = k_waffen_basis[modell & 15];
    s_gesichert = 0;
    free(s_koerper_buf); s_koerper_buf = NULL;
    free(s_w00_buf);     s_w00_buf = NULL;
    re15_sherry_r41_modell_init(modell);
}

static void sichern(void)
{
    s_sv_pl00_skel = *s_b.pl00_skel; s_sv_pl00_anim = *s_b.pl00_anim;
    s_sv_w01_skel  = *s_b.w01_skel;  s_sv_w01_anim  = *s_b.w01_anim;
    for (int i = 0; i < s_b.wpn_max; i++) {
        s_sv_wpn_anim[i] = s_b.wpn_anim[i]; s_sv_wpn_skel[i] = s_b.wpn_skel[i];
        s_sv_bank_ok[i] = s_b.wpn_bank_ok[i]; s_sv_md1_ok[i] = s_b.wpn_md1_ok[i];
    }
    s_gesichert = 1;
}

static void zurueck(void)
{
    *s_b.pl00_skel = s_sv_pl00_skel; *s_b.pl00_anim = s_sv_pl00_anim;
    *s_b.w01_skel  = s_sv_w01_skel;  *s_b.w01_anim  = s_sv_w01_anim;
    for (int i = 0; i < s_b.wpn_max; i++) {
        s_b.wpn_anim[i] = s_sv_wpn_anim[i]; s_b.wpn_skel[i] = s_sv_wpn_skel[i];
        s_b.wpn_bank_ok[i] = s_sv_bank_ok[i]; s_b.wpn_md1_ok[i] = s_sv_md1_ok[i];
    }
}

/* Koerper + W00 einer fremden Familie aus der PLD (und PL..W00.PLW) binden. 1 = gebunden. */
static int fremd_binden(int basis, const uint8_t *pld, int pld_size)
{
    unsigned long eo = 0, el = 0, mo = 0, ml = 0;
    if (!re15_pld_part(pld, (long)pld_size, RE15_PLD_EDD, &eo, &el) ||
        !re15_pld_part(pld, (long)pld_size, RE15_PLD_EMR, &mo, &ml)) return 0;
    uint8_t *kb = (uint8_t *)malloc((size_t)pld_size);
    if (!kb) return 0;
    memcpy(kb, pld, (size_t)pld_size);
    re15_emd_skeleton_t sk;
    memset(&sk, 0, sizeof sk);
    if (re15_emd_parse_animation(kb + eo, (size_t)el, &s_an_tmp) != 0 ||
        re15_emd_parse_skeleton (kb + mo, (size_t)ml, &sk) != 0) { free(kb); return 0; }
    /* W00 der Familie (Waffe 0: Basis + 0, @0x80036b84-c4) */
    char nm[32]; snprintf(nm, sizeof nm, "PLD/%sW00.PLW", familie_praefix(basis));
    int wsz = 0; uint8_t *wb = re15_pc_read_any(nm, &wsz);
    unsigned long weo = 0, wel = 0, wmo = 0, wml = 0;
    re15_emd_skeleton_t wraw; memset(&wraw, 0, sizeof wraw);
    static re15_emd_animation_t s_wan_tmp;
    int w_ok = wb && re15_pld_part(wb, (long)wsz, RE15_PLD_EDD, &weo, &wel) &&
                     re15_pld_part(wb, (long)wsz, RE15_PLD_EMR, &wmo, &wml) &&
                     re15_emd_parse_animation(wb + weo, (size_t)wel, &s_wan_tmp) == 0 &&
                     re15_emd_parse_skeleton (wb + wmo, (size_t)wml, &wraw) == 0;
    if (!w_ok) { free(kb); free(wb); return 0; }
    free(s_koerper_buf); s_koerper_buf = kb;
    free(s_w00_buf);     s_w00_buf = wb;
    s_fr_anim = s_an_tmp; s_fr_skel = sk; s_w00_anim = s_wan_tmp; s_w00_raw = wraw;
    /* Koerper: EDD -> +0x16c, EMR -> +0x84 (@0x8003154c/@0x80031578) */
    *s_b.pl00_anim = s_fr_anim; *s_b.pl00_skel = s_fr_skel;
    /* W-Bank 0: Bindelage des NEUEN Koerpers + Keyframes der W00 (Verbund wie main.c [wpn-bank]: die W-EMR traegt
     * eine andere Bindepose, nur ihr Keyframe-Vorrat wird uebernommen); EDD der W00 (+0x174 @0x80036be4). */
    re15_emd_skeleton_t w = s_fr_skel;
    w.keyframe_data       = s_w00_raw.keyframe_data;
    w.keyframe_data_size  = s_w00_raw.keyframe_data_size;
    w.keyframe_count      = s_w00_raw.keyframe_count;
    w.keyframe_size_bytes = s_w00_raw.keyframe_size_bytes;
    for (int i = 0; i < s_b.wpn_max; i++) { s_b.wpn_bank_ok[i] = 0; s_b.wpn_md1_ok[i] = 0; }   /* @0x80036c08 */
    s_b.wpn_anim[0] = s_w00_anim; s_b.wpn_skel[0] = w; s_b.wpn_bank_ok[0] = 1;
    *s_b.w01_anim = s_w00_anim; *s_b.w01_skel = w;   /* Geh-Quelle (Fusstritt-Bits) + Rueckfall */
    return 1;
}

int re15_sherry_pc_r41_koerper(int modell, const uint8_t *pld, int pld_size)
{
    if (!s_angemeldet) return 0;
    int basis = k_waffen_basis[modell & 15];
    re15_sherry_r41_modell_setzen(modell);              /* Waffe @0x80031690/@0x800316c4 */
    if (basis == s_familie) return 0;                   /* EDD/EMR + W-Banken bytegleich (Westen-Weg) */
    if (s_familie == s_boot_familie && !s_gesichert) sichern();
    int ok;
    if (basis == s_boot_familie && s_gesichert) { zurueck(); ok = 1; }
    else ok = fremd_binden(basis, pld, pld_size);
    if (!ok) {
        fprintf(stderr, "[sherry-r41] Modell %d: Koerper/W00 nicht bindbar - Skelett bleibt\n", modell);
        return 0;
    }
    *s_b.skel = *s_b.pl00_skel; *s_b.anim = *s_b.pl00_anim;   /* Raum-RBJ legt sich danach darueber (@0x80039a08) */
    s_familie = basis;
    { extern void re15_wound_reset(void); re15_wound_reset(); }                         /* FUN_80037c1c @0x800316c8 */
    { extern void re15_audio_prime_core(int idx); re15_audio_prime_core(modell & 15); }   /* FUN_800440c4 @0x800316e8 */
    fprintf(stderr, "[sherry-r41] Koerper -> Familie %d (%s): %d Knochen, %d Clips, W00 %d Clips, Wurzel-Bind y %d\n",
            basis, familie_praefix(basis), s_b.pl00_skel->bone_count, s_b.pl00_anim->clip_count,
            s_b.wpn_anim[0].clip_count, (int)s_b.pl00_skel->bone_relative_pos[0][1]);
    return 1;
}
