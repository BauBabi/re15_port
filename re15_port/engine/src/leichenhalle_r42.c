/*
 * RE1.5 Rebuilt — Runde 42 Strang "original", Stufe 1: Leichenhalle ROOM1200/1201 — Kuehlfach-Tuer klappert, der Zombie
 * kriecht heraus (wie die 40%-Disc). Herleitung, Belege und alle Konstanten: include/re15_leichenhalle_r42.h.
 * Form wie werkstatt11b1_r41.c (Programm im ersten freien Ereignis-Faden, RDT-Anker) und tropfen_r41.c (Evt_exec auslassen).
 */
#include "re15_leichenhalle_r42.h"

#include <string.h>

#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_room.h"
#include "re15_scd.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#define LH_LOG(...) do { fprintf(stderr, "[leichenhalle42] F%u ", (unsigned)g_engine.frame_count); \
                         fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
#define LH_LOG(...) do { } while (0)
#endif

/* Disc ROOM1200 sub02 ab @0x00918 bis @0x00A23 (268 B) — Byte fuer Byte aus Version_40%/PSX/STAGE1/ROOM1200.RDT.
 * Ausgelassen ist nur die erste Zeile @0x0090E `46 01 00 41 00 00 00 00 00 00` (Aot_reset Platz 1, s. Kopf). */
static const uint8_t k_prog_1200[] = {
    0x2e, 0x03, 0x01,                                                        /* @0x00918 Work_set */
    0x00,                                                                    /* @0x0091B Nop */
    0x0d, 0x00, 0x1e, 0x00, 0x04, 0x00,                                      /* @0x0091C For */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbe, 0xc7,                          /* @0x00922 Pos_set */
    0x02,                                                                    /* @0x0092A Evt_next */
    0x00,                                                                    /* @0x0092B Nop */
    0x09, 0x0a, 0x03, 0x00,                                                  /* @0x0092C Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe6, 0xc7,                          /* @0x00930 Pos_set */
    0x02,                                                                    /* @0x00938 Evt_next */
    0x00,                                                                    /* @0x00939 Nop */
    0x09, 0x0a, 0x04, 0x00,                                                  /* @0x0093A Sleep */
    0x0e, 0x00,                                                              /* @0x0093E Next */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbc, 0xc7,                          /* @0x00940 Pos_set */
    0x02,                                                                    /* @0x00948 Evt_next */
    0x00,                                                                    /* @0x00949 Nop */
    0x09, 0x0a, 0x07, 0x00,                                                  /* @0x0094A Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe8, 0xc7,                          /* @0x0094E Pos_set */
    0x02,                                                                    /* @0x00956 Evt_next */
    0x00,                                                                    /* @0x00957 Nop */
    0x09, 0x0a, 0x09, 0x00,                                                  /* @0x00958 Sleep */
    0x0d, 0x00, 0x1e, 0x00, 0x02, 0x00,                                      /* @0x0095C For */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbc, 0xc7,                          /* @0x00962 Pos_set */
    0x02,                                                                    /* @0x0096A Evt_next */
    0x00,                                                                    /* @0x0096B Nop */
    0x09, 0x0a, 0x05, 0x00,                                                  /* @0x0096C Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe8, 0xc7,                          /* @0x00970 Pos_set */
    0x02,                                                                    /* @0x00978 Evt_next */
    0x00,                                                                    /* @0x00979 Nop */
    0x09, 0x0a, 0x02, 0x00,                                                  /* @0x0097A Sleep */
    0x0e, 0x00,                                                              /* @0x0097E Next */
    0x0d, 0x00, 0x1e, 0x00, 0x03, 0x00,                                      /* @0x00980 For */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbc, 0xc7,                          /* @0x00986 Pos_set */
    0x02,                                                                    /* @0x0098E Evt_next */
    0x00,                                                                    /* @0x0098F Nop */
    0x09, 0x0a, 0x03, 0x00,                                                  /* @0x00990 Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe8, 0xc7,                          /* @0x00994 Pos_set */
    0x02,                                                                    /* @0x0099C Evt_next */
    0x00,                                                                    /* @0x0099D Nop */
    0x09, 0x0a, 0x02, 0x00,                                                  /* @0x0099E Sleep */
    0x0e, 0x00,                                                              /* @0x009A2 Next */
    0x09, 0x0a, 0x05, 0x00,                                                  /* @0x009A4 Sleep */
    0x2e, 0x03, 0x01,                                                        /* @0x009A8 Work_set */
    0x00,                                                                    /* @0x009AB Nop */
    0x2f, 0x04, 0x02, 0x00,                                                  /* @0x009AC Speed_set */
    0x2f, 0x0a, 0x02, 0x00,                                                  /* @0x009B0 Speed_set */
    0x0d, 0x00, 0x06, 0x00, 0x0f, 0x00,                                      /* @0x009B4 For */
    0x30,                                                                    /* @0x009BA Add_speed */
    0x31,                                                                    /* @0x009BB Add_aspeed */
    0x02,                                                                    /* @0x009BC Evt_next */
    0x00,                                                                    /* @0x009BD Nop */
    0x0e, 0x00,                                                              /* @0x009BE Next */
    0x0d, 0x00, 0x06, 0x00, 0x12, 0x00,                                      /* @0x009C0 For */
    0x30,                                                                    /* @0x009C6 Add_speed */
    0x02,                                                                    /* @0x009C7 Evt_next */
    0x00,                                                                    /* @0x009C8 Nop */
    0x00,                                                                    /* @0x009C9 Nop */
    0x0e, 0x00,                                                              /* @0x009CA Next */
    0x2e, 0x02, 0x02,                                                        /* @0x009CC Work_set */
    0x00,                                                                    /* @0x009CF Nop */
    0x34, 0x0c, 0x81, 0x00,                                                  /* @0x009D0 Member_set */
    0x2e, 0x03, 0x01,                                                        /* @0x009D4 Work_set */
    0x00,                                                                    /* @0x009D7 Nop */
    0x2f, 0x04, 0x17, 0x00,                                                  /* @0x009D8 Speed_set */
    0x0d, 0x00, 0x06, 0x00, 0x14, 0x00,                                      /* @0x009DC For */
    0x30,                                                                    /* @0x009E2 Add_speed */
    0x02,                                                                    /* @0x009E3 Evt_next */
    0x00,                                                                    /* @0x009E4 Nop */
    0x00,                                                                    /* @0x009E5 Nop */
    0x0e, 0x00,                                                              /* @0x009E6 Next */
    0x2f, 0x04, 0x0f, 0x00,                                                  /* @0x009E8 Speed_set */
    0x0d, 0x00, 0x06, 0x00, 0x1e, 0x00,                                      /* @0x009EC For */
    0x30,                                                                    /* @0x009F2 Add_speed */
    0x02,                                                                    /* @0x009F3 Evt_next */
    0x00,                                                                    /* @0x009F4 Nop */
    0x00,                                                                    /* @0x009F5 Nop */
    0x0e, 0x00,                                                              /* @0x009F6 Next */
    0x2f, 0x0a, 0xff, 0xff,                                                  /* @0x009F8 Speed_set */
    0x0d, 0x00, 0x08, 0x00, 0x0d, 0x00,                                      /* @0x009FC For */
    0x30,                                                                    /* @0x00A02 Add_speed */
    0x31,                                                                    /* @0x00A03 Add_aspeed */
    0x02,                                                                    /* @0x00A04 Evt_next */
    0x30,                                                                    /* @0x00A05 Add_speed */
    0x02,                                                                    /* @0x00A06 Evt_next */
    0x00,                                                                    /* @0x00A07 Nop */
    0x0e, 0x00,                                                              /* @0x00A08 Next */
    0x2f, 0x0a, 0xff, 0xff,                                                  /* @0x00A0A Speed_set */
    0x0d, 0x00, 0x0a, 0x00, 0x0a, 0x00,                                      /* @0x00A0E For */
    0x30,                                                                    /* @0x00A14 Add_speed */
    0x31,                                                                    /* @0x00A15 Add_aspeed */
    0x02,                                                                    /* @0x00A16 Evt_next */
    0x00,                                                                    /* @0x00A17 Nop */
    0x09, 0x0a, 0x03, 0x00,                                                  /* @0x00A18 Sleep */
    0x0e, 0x00,                                                              /* @0x00A1C Next */
    0x22, 0x04, 0x11, 0x01,                                                  /* @0x00A1E Set */
    0x01, 0x00,                                                              /* @0x00A22 Evt_end */
};

/* Disc ROOM1201 sub02 ab @0x00918 bis @0x00A2B (276 B) — Byte fuer Byte aus Version_40%/PSX/STAGE1/ROOM1201.RDT
 * (wie 1200, dazu @0x0091C Dir_set(0,1024,0)). Ausgelassen ist nur @0x0090E Aot_reset Platz 1. */
static const uint8_t k_prog_1201[] = {
    0x2e, 0x03, 0x01,                                                        /* @0x00918 Work_set */
    0x00,                                                                    /* @0x0091B Nop */
    0x33, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,                          /* @0x0091C Dir_set */
    0x0d, 0x00, 0x1e, 0x00, 0x04, 0x00,                                      /* @0x00924 For */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbe, 0xc7,                          /* @0x0092A Pos_set */
    0x02,                                                                    /* @0x00932 Evt_next */
    0x00,                                                                    /* @0x00933 Nop */
    0x09, 0x0a, 0x03, 0x00,                                                  /* @0x00934 Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe6, 0xc7,                          /* @0x00938 Pos_set */
    0x02,                                                                    /* @0x00940 Evt_next */
    0x00,                                                                    /* @0x00941 Nop */
    0x09, 0x0a, 0x04, 0x00,                                                  /* @0x00942 Sleep */
    0x0e, 0x00,                                                              /* @0x00946 Next */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbc, 0xc7,                          /* @0x00948 Pos_set */
    0x02,                                                                    /* @0x00950 Evt_next */
    0x00,                                                                    /* @0x00951 Nop */
    0x09, 0x0a, 0x07, 0x00,                                                  /* @0x00952 Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe8, 0xc7,                          /* @0x00956 Pos_set */
    0x02,                                                                    /* @0x0095E Evt_next */
    0x00,                                                                    /* @0x0095F Nop */
    0x09, 0x0a, 0x09, 0x00,                                                  /* @0x00960 Sleep */
    0x0d, 0x00, 0x1e, 0x00, 0x02, 0x00,                                      /* @0x00964 For */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbc, 0xc7,                          /* @0x0096A Pos_set */
    0x02,                                                                    /* @0x00972 Evt_next */
    0x00,                                                                    /* @0x00973 Nop */
    0x09, 0x0a, 0x05, 0x00,                                                  /* @0x00974 Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe8, 0xc7,                          /* @0x00978 Pos_set */
    0x02,                                                                    /* @0x00980 Evt_next */
    0x00,                                                                    /* @0x00981 Nop */
    0x09, 0x0a, 0x02, 0x00,                                                  /* @0x00982 Sleep */
    0x0e, 0x00,                                                              /* @0x00986 Next */
    0x0d, 0x00, 0x1e, 0x00, 0x03, 0x00,                                      /* @0x00988 For */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xbc, 0xc7,                          /* @0x0098E Pos_set */
    0x02,                                                                    /* @0x00996 Evt_next */
    0x00,                                                                    /* @0x00997 Nop */
    0x09, 0x0a, 0x03, 0x00,                                                  /* @0x00998 Sleep */
    0x32, 0x00, 0xa2, 0x9d, 0xe6, 0xf8, 0xe8, 0xc7,                          /* @0x0099C Pos_set */
    0x02,                                                                    /* @0x009A4 Evt_next */
    0x00,                                                                    /* @0x009A5 Nop */
    0x09, 0x0a, 0x02, 0x00,                                                  /* @0x009A6 Sleep */
    0x0e, 0x00,                                                              /* @0x009AA Next */
    0x09, 0x0a, 0x05, 0x00,                                                  /* @0x009AC Sleep */
    0x2e, 0x03, 0x01,                                                        /* @0x009B0 Work_set */
    0x00,                                                                    /* @0x009B3 Nop */
    0x2f, 0x04, 0x02, 0x00,                                                  /* @0x009B4 Speed_set */
    0x2f, 0x0a, 0x02, 0x00,                                                  /* @0x009B8 Speed_set */
    0x0d, 0x00, 0x06, 0x00, 0x0f, 0x00,                                      /* @0x009BC For */
    0x30,                                                                    /* @0x009C2 Add_speed */
    0x31,                                                                    /* @0x009C3 Add_aspeed */
    0x02,                                                                    /* @0x009C4 Evt_next */
    0x00,                                                                    /* @0x009C5 Nop */
    0x0e, 0x00,                                                              /* @0x009C6 Next */
    0x0d, 0x00, 0x06, 0x00, 0x12, 0x00,                                      /* @0x009C8 For */
    0x30,                                                                    /* @0x009CE Add_speed */
    0x02,                                                                    /* @0x009CF Evt_next */
    0x00,                                                                    /* @0x009D0 Nop */
    0x00,                                                                    /* @0x009D1 Nop */
    0x0e, 0x00,                                                              /* @0x009D2 Next */
    0x2e, 0x02, 0x02,                                                        /* @0x009D4 Work_set */
    0x00,                                                                    /* @0x009D7 Nop */
    0x34, 0x0c, 0x81, 0x00,                                                  /* @0x009D8 Member_set */
    0x2e, 0x03, 0x01,                                                        /* @0x009DC Work_set */
    0x00,                                                                    /* @0x009DF Nop */
    0x2f, 0x04, 0x17, 0x00,                                                  /* @0x009E0 Speed_set */
    0x0d, 0x00, 0x06, 0x00, 0x14, 0x00,                                      /* @0x009E4 For */
    0x30,                                                                    /* @0x009EA Add_speed */
    0x02,                                                                    /* @0x009EB Evt_next */
    0x00,                                                                    /* @0x009EC Nop */
    0x00,                                                                    /* @0x009ED Nop */
    0x0e, 0x00,                                                              /* @0x009EE Next */
    0x2f, 0x04, 0x0f, 0x00,                                                  /* @0x009F0 Speed_set */
    0x0d, 0x00, 0x06, 0x00, 0x1e, 0x00,                                      /* @0x009F4 For */
    0x30,                                                                    /* @0x009FA Add_speed */
    0x02,                                                                    /* @0x009FB Evt_next */
    0x00,                                                                    /* @0x009FC Nop */
    0x00,                                                                    /* @0x009FD Nop */
    0x0e, 0x00,                                                              /* @0x009FE Next */
    0x2f, 0x0a, 0xff, 0xff,                                                  /* @0x00A00 Speed_set */
    0x0d, 0x00, 0x08, 0x00, 0x0d, 0x00,                                      /* @0x00A04 For */
    0x30,                                                                    /* @0x00A0A Add_speed */
    0x31,                                                                    /* @0x00A0B Add_aspeed */
    0x02,                                                                    /* @0x00A0C Evt_next */
    0x30,                                                                    /* @0x00A0D Add_speed */
    0x02,                                                                    /* @0x00A0E Evt_next */
    0x00,                                                                    /* @0x00A0F Nop */
    0x0e, 0x00,                                                              /* @0x00A10 Next */
    0x2f, 0x0a, 0xff, 0xff,                                                  /* @0x00A12 Speed_set */
    0x0d, 0x00, 0x0a, 0x00, 0x0a, 0x00,                                      /* @0x00A16 For */
    0x30,                                                                    /* @0x00A1C Add_speed */
    0x31,                                                                    /* @0x00A1D Add_aspeed */
    0x02,                                                                    /* @0x00A1E Evt_next */
    0x00,                                                                    /* @0x00A1F Nop */
    0x09, 0x0a, 0x03, 0x00,                                                  /* @0x00A20 Sleep */
    0x0e, 0x00,                                                              /* @0x00A24 Next */
    0x22, 0x04, 0x11, 0x01,                                                  /* @0x00A26 Set */
    0x01, 0x00,                                                              /* @0x00A2A Evt_end */
};

/* Mod-Anker (shared_assets ROOM1200.RDT = ROOM1201.RDT). Weicht die Datei ab, tut das Modul nichts. */
static const uint8_t k_mod_sub02[RE15_R42L_MOD_SUB02_LEN] = {   /* @0x009A0 = Disc @0x009CC..@0x00A1D */
    0x2e, 0x02, 0x02, 0x00, 0x34, 0x0c, 0x81, 0x00, 0x2e, 0x03, 0x01, 0x00, 0x2f, 0x04, 0x17, 0x00,
    0x0d, 0x00, 0x06, 0x00, 0x14, 0x00, 0x30, 0x02, 0x00, 0x00, 0x0e, 0x00, 0x2f, 0x04, 0x0f, 0x00,
    0x0d, 0x00, 0x06, 0x00, 0x1e, 0x00, 0x30, 0x02, 0x00, 0x00, 0x0e, 0x00, 0x2f, 0x0a, 0xff, 0xff,
    0x0d, 0x00, 0x08, 0x00, 0x0d, 0x00, 0x30, 0x31, 0x02, 0x30, 0x02, 0x00, 0x0e, 0x00, 0x2f, 0x0a,
    0xff, 0xff, 0x0d, 0x00, 0x0a, 0x00, 0x0a, 0x00, 0x30, 0x31, 0x02, 0x00, 0x09, 0x0a, 0x03, 0x00,
    0x0e, 0x00,
};
static const uint8_t k_mod_sub03[RE15_R42L_MOD_SUB03_LEN] = {   /* @0x009F4, Evt_exec @0x00A0C */
    0x22, 0x02, 0x07, 0x01, 0x06, 0x00, 0x44, 0x00, 0x21, 0x09, 0x68, 0x00, 0x47, 0x03, 0x02, 0x00,
    0x06, 0x00, 0x34, 0x00, 0x21, 0x09, 0x68, 0x01, 0x04, 0xff, 0x18, 0x02, 0x54, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x2e, 0x02, 0x00, 0x00, 0x34, 0x0c, 0x8a, 0x00, 0x09, 0x0a, 0x05, 0x00, 0x2e, 0x02,
    0x01, 0x00, 0x34, 0x0c, 0x89, 0x00, 0x09, 0x0a, 0x05, 0x00, 0x2e, 0x02, 0x03, 0x00, 0x34, 0x0c,
    0x89, 0x00, 0x22, 0x04, 0x11, 0x01, 0x08, 0x00, 0x07, 0x00, 0x06, 0x00, 0x00, 0x00, 0x22, 0x02,
    0x07, 0x00, 0x01, 0x00,
};

/* Wiederbetreten nach dem Disc-Ablauf ohne Minidisc (Kopf "WIEDERBETRETEN"): Mod main00 Else-Saetze (Anker) und ihr Ersatz. */
static const uint32_t k_else_off[4] = { RE15_R42L_MOD_ELSE_0, RE15_R42L_MOD_ELSE_1, RE15_R42L_MOD_ELSE_2, RE15_R42L_MOD_ELSE_3 };
static const uint8_t k_mod_else[4][20] = {
    { 0x44, 0x00, 0x10, 0x0d, 0x00, 0x00, 0x00, 0x8c, 0x2a, 0xae, 0x00, 0x00, 0x3c, 0xab, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00 }, /* @0x008AA */
    { 0x44, 0x01, 0x10, 0x0d, 0x00, 0x00, 0x01, 0x8d, 0x04, 0xac, 0x00, 0x00, 0x6d, 0xb7, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00 }, /* @0x008BE */
    { 0x44, 0x02, 0x10, 0x0d, 0x00, 0x00, 0x01, 0x8e, 0xa8, 0xa0, 0x00, 0x00, 0xe4, 0xc1, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00 }, /* @0x008D2 */
    { 0x44, 0x03, 0x10, 0x0d, 0x00, 0x00, 0x01, 0x8f, 0x5a, 0xb3, 0x00, 0x00, 0x0a, 0xc1, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00 }, /* @0x008E6 */
};
/* Gegner 0/1/3 = Mod Ja-Zweig (Anker: die Datei muss dort genau diese Bytes tragen); Gegner 2 = Disc @0x008C6 mit pc[7] 0x8E. */
static const uint32_t k_ja_off[4] = { 0x00856u, 0x0086Au, 0u, 0x00892u };
static const uint8_t k_ersatz[4][20] = {
    { 0x44, 0x00, 0x10, 0x88, 0x00, 0x00, 0x00, 0x8c, 0x2a, 0xae, 0x00, 0x00, 0x3c, 0xab, 0x00, 0x00, 0x20, 0x03, 0x00, 0x00 }, /* Mod @0x00856 */
    { 0x44, 0x01, 0x10, 0x87, 0x01, 0x00, 0x01, 0x8d, 0x47, 0xa1, 0xf8, 0xf8, 0x6d, 0xb7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* Mod @0x0086A */
    { 0x44, 0x02, 0x10, 0x81, 0x00, 0x00, 0x01, RE15_R42L_KRIECHER_TOD,                                                        /* Disc @0x008C6, */
      0xa8, 0xa0, 0x00, 0x00, 0xe4, 0xc1, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00 },                                               /* pc[7] Mod @0x00885 */
    { 0x44, 0x03, 0x10, 0x87, 0x00, 0x00, 0x01, 0x8f, 0x5a, 0xb3, 0x00, 0x00, 0x0a, 0xc1, 0x00, 0x00, 0xaf, 0x05, 0x00, 0x00 }, /* Mod @0x00892 */
};

static unsigned s_ersetzt;
static int      s_scharf;        /* 1 = Ck(4,17)==0 beim Betreten, Anker stimmt, Ablauf noch nicht gestartet */
static int      s_gestartet;     /* 1 = Disc-Ablauf in diesem Besuch gestartet (ersetzt Aot_reset @0x0090E)   */
static uint16_t s_raum;
static unsigned s_starts;
static unsigned s_ausgelassen;

static int ist_raum(uint16_t r) { return r == RE15_R42L_RAUM_LEON || r == RE15_R42L_RAUM_ELZA; }

static int anker_ok(const re15_rdt_t *rdt)
{
    if (!rdt || !rdt->raw) return 0;
    const size_t n = (size_t)rdt->raw_size;
    if (n < RE15_R42L_MOD_SUB03_OFF + RE15_R42L_MOD_SUB03_LEN) return 0;
    return memcmp(rdt->raw + RE15_R42L_MOD_SUB02_OFF, k_mod_sub02, RE15_R42L_MOD_SUB02_LEN) == 0 &&
           memcmp(rdt->raw + RE15_R42L_MOD_SUB03_OFF, k_mod_sub03, RE15_R42L_MOD_SUB03_LEN) == 0;
}

const uint8_t *re15_leichenhalle_r42_programm(uint16_t room_id, int *out_len)
{
    const int elza = room_id == RE15_R42L_RAUM_ELZA;
    if (out_len) *out_len = elza ? (int)sizeof k_prog_1201 : (int)sizeof k_prog_1200;
    return elza ? k_prog_1201 : k_prog_1200;
}

/* FUN_80042b64 (Disc-Exe): (unsigned)(px - x) <= b und (unsigned)(pz - z) <= t — `sltu` @0x80042b78 / @0x80042b94. */
int re15_leichenhalle_r42_im_rechteck(int32_t px, int32_t pz)
{
    const uint32_t dx = (uint32_t)(px - (int32_t)RE15_R42L_X);
    const uint32_t dz = (uint32_t)(pz - (int32_t)RE15_R42L_Z);
    return dx <= (uint32_t)RE15_R42L_B && dz <= (uint32_t)RE15_R42L_T;
}

void re15_leichenhalle_r42_install(uint16_t room_id)
{
    s_scharf = 0; s_gestartet = 0; s_raum = room_id;
    if (!ist_raum(room_id)) return;
    if (!g_room_rdt_ok || !anker_ok(&g_room_rdt)) {
        LH_LOG("ROOM%04X: RDT weicht vom Mod-Anker sub02 @0x009A0 / sub03 @0x009F4 ab -> nichts", (unsigned)room_id);
        return;
    }
    if (re15_game_flag_get(RE15_R42L_BANK, RE15_R42L_BIT)) {
        LH_LOG("ROOM%04X: Ck(%d,%d)=1 (Disc main00 @0x007CE) -> Kuehlfach schon offen, kein Ausloeser", (unsigned)room_id,
               RE15_R42L_BANK, RE15_R42L_BIT);
        return;
    }
    s_scharf = 1;
    LH_LOG("ROOM%04X: Ausloeser scharf, Etage %d, Rechteck x %d..%d z %d..%d (Disc main00 @0x007D2)", (unsigned)room_id,
           RE15_R42L_ETAGE, RE15_R42L_X, RE15_R42L_X + RE15_R42L_B, RE15_R42L_Z, RE15_R42L_Z + RE15_R42L_T);
}

void re15_leichenhalle_r42_tick(void)
{
    if (!s_scharf || s_gestartet) return;
    if ((uint16_t)g_current_room_id != s_raum) { s_scharf = 0; return; }
    if (re15_game_flag_get(RE15_R42L_BANK, RE15_R42L_BIT)) {   /* Mod-sub03 hat das Fach schon geoeffnet */
        s_scharf = 0;
        LH_LOG("ROOM%04X: Ck(%d,%d)=1 waehrend des Besuchs (Mod-sub03 @0x00A36) -> Ausloeser aus", (unsigned)s_raum,
               RE15_R42L_BANK, RE15_R42L_BIT);
        return;
    }
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!pl->active) return;
    if (pl->floor != RE15_R42L_ETAGE) return;                  /* rec[2] == entity+0x82, Gate @0x80042ccc */
    if (!re15_leichenhalle_r42_im_rechteck(pl->x, pl->z)) return;
    int len = 0;
    const uint8_t *p = re15_leichenhalle_r42_programm(s_raum, &len);
    int faden = -1;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {   /* Evt_exec 0xFF -> erster freier Faden */
        if (g_scd.threads[s].active) continue;
        if (scd_thread_start(s, p) == 0) { faden = s; break; }
    }
    if (faden < 0) { LH_LOG("ROOM%04X: kein freier Faden - Ablauf wartet", (unsigned)s_raum); return; }
    s_gestartet = 1; s_scharf = 0; s_starts++;
    LH_LOG("ROOM%04X: Spieler bei (%d,%d) Etage %d im Rechteck -> Disc-sub02 ab @0x00918 (%d B, Faden %d): Klappern, "
           "Kuehlfach auf, Gegner 2 grid 0x81 (@0x009D0), Set(4,17) (@0x00A1E)", (unsigned)s_raum, (int)pl->x, (int)pl->z,
           (int)pl->floor, len, faden);
}

int re15_leichenhalle_r42_evt_exec_gestrichen(uint16_t room_id, const uint8_t *pc, const re15_rdt_t *rdt)
{
    if (!ist_raum(room_id) || !pc || !rdt || !rdt->raw) return 0;
    if ((size_t)(pc - rdt->raw) != RE15_R42L_MOD_EVT_OFF) return 0;
    if (pc[0] != 0x04 || pc[1] != 0xFF || pc[2] != 0x18 || pc[3] != 0x02) return 0;
    if (!anker_ok(rdt)) return 0;
    const int offen = s_gestartet || re15_game_flag_get(RE15_R42L_BANK, RE15_R42L_BIT);
    if (!offen) return 0;
    s_ausgelassen++;
    LH_LOG("ROOM%04X: Mod-sub03 @0x00A0C Evt_exec(sub02) ausgelassen - Kuehlfach schon offen (%s)", (unsigned)room_id,
           s_gestartet ? "Disc-Ablauf in diesem Besuch" : "Ck(4,17)=1");
    return 1;
}

const uint8_t *re15_leichenhalle_r42_em_ersatz(uint16_t room_id, const uint8_t *pc, const re15_rdt_t *rdt)
{
    if (!ist_raum(room_id) || !pc || !rdt || !rdt->raw) return NULL;
    const uintptr_t a = (uintptr_t)pc, lo = (uintptr_t)rdt->raw, n = (uintptr_t)rdt->raw_size;
    if (a < lo || a >= lo + n) return NULL;                      /* der Ersatz selbst liegt ausserhalb der Datei */
    const uint32_t off = (uint32_t)(a - lo);
    int i = 0;
    while (i < 4 && k_else_off[i] != off) i++;
    if (i == 4 || (uintptr_t)off + 20u > n) return NULL;
    if (memcmp(pc, k_mod_else[i], 20) != 0) return NULL;
    if (k_ja_off[i] && ((uintptr_t)k_ja_off[i] + 20u > n || memcmp(rdt->raw + k_ja_off[i], k_ersatz[i], 20) != 0)) return NULL;
    if (!anker_ok(rdt)) return NULL;
    if (!re15_game_flag_get(RE15_R42L_BANK, RE15_R42L_BIT)) return NULL;        /* Ja-Zweig laeuft ohnehin */
    if (re15_game_flag_get(RE15_R42L_MD_BANK, RE15_R42L_MD_BIT)) return NULL;   /* Minidisc genommen: Mod-Bedeutung gilt */
    s_ersetzt++;
    LH_LOG("ROOM%04X: Wiederbetreten nach dem Disc-Ablauf ohne Minidisc ((4,17)=1, (9,104)=0): Gegner %d Satz @0x%05X -> %s",
           (unsigned)room_id, i, (unsigned)off, i == 2 ? "Disc-Kriecher @0x008C6 (pc[7] 0x8E)" : "Mod Ja-Zweig (liegend)");
    return k_ersatz[i];
}

unsigned re15_leichenhalle_r42_ersetzt(void)     { return s_ersetzt; }
int      re15_leichenhalle_r42_scharf(void)      { return s_scharf; }
int      re15_leichenhalle_r42_gestartet(void)   { return s_gestartet; }
unsigned re15_leichenhalle_r42_starts(void)      { return s_starts; }
unsigned re15_leichenhalle_r42_ausgelassen(void) { return s_ausgelassen; }
