/* Runde 41 Spur K (PC): Spielfigur-Wechsel mit Skelett/Animation/Waffen-Bank - Belege in sherry_pc_r41.c. */
#ifndef SHERRY_PC_R41_H
#define SHERRY_PC_R41_H
#include <stdint.h>
#include "re15_emd.h"

typedef struct {
    re15_emd_skeleton_t  *skel, *pl00_skel, *w01_skel;
    re15_emd_animation_t *anim, *pl00_anim, *w01_anim;
    re15_emd_animation_t *wpn_anim;   /* [wpn_max] */
    re15_emd_skeleton_t  *wpn_skel;   /* [wpn_max] */
    int *wpn_bank_ok, *wpn_md1_ok;    /* [wpn_max] */
    int  wpn_max;
} re15_sherry_pc_r41_baenke_t;

/* Boot (auch nach Tod -> Titel): Zeiger auf die Spieler-Baenke von main() + Startmodell. */
void re15_sherry_pc_r41_anmelden(const re15_sherry_pc_r41_baenke_t *b, int modell);
/* Aus pc_sync_player_model nach dem MD1/TIM-Tausch: 1 = Koerper/Waffen-Bank neu gebunden. */
int  re15_sherry_pc_r41_koerper(int modell, const uint8_t *pld, int pld_size);
#endif
