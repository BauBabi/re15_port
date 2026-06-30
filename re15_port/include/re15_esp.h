/**
 * @file re15_esp.h
 * @brief RE1.5 ESP effect-sprite section parser (Phase ESP-A) — byte-true port of the
 *        runtime ESP installer FUN_80019354 / FUN_8001945c / FUN_800194f8 (PSX.EXE).
 *
 * The ESP (effect-sprite) data lives in the room RDT, addressed by four directory slots
 * (the same RDT directory the port already parses for SCD/camera/etc):
 *   RDT+0x4C -> the effect-ID header (u8[8], 0xFF = unused-slot terminator)   [param_1/param_3]
 *   RDT+0x50 -> the EFF pointer table END  (read DOWNWARD, 4 bytes/entry)      [param_2]
 *   RDT+0x54 -> the embedded-TIM base                                          [FUN_800194f8 arg2]
 *   RDT+0x58 -> the TIM offset table END   (read DOWNWARD, 4 bytes/entry)      [FUN_800194f8 arg1]
 *
 * Per the loader FUN_8001945c (verified byte-true against ROOM1140.RDT, 2026-06-30):
 *   - the effect-ID array is read FORWARD (param_1++), the pointer table DOWNWARD (param_2--);
 *   - EFF body start  = ptr_table_entry + idh_off      (puVar3 = iVar2 + param_3);
 *   - EFF header word0 = { count_a = lo16, count_b = hi16 };
 *   - EFF body size   = (count_a*2 + count_b + 2) u32 words = count_a*8 + count_b*4 + 8 bytes
 *     (count_a 8-byte records + count_b 4-byte sprite-coord records + an 8-byte header).
 * The embedded TIMs (FUN_800194f8): tim[i] = tim_base + *(tim_end - 4*(i+1)) (pre-decrement,
 * downward); each is a standard PSX TIM blob (magic 0x00000010). VRAM packing + CLUT/TPage/UV
 * binding is the renderer's job (Phase ESP-C), NOT this parser.
 *
 * This is the parse/index layer only (the re15_ems.c analog for effects). The effect pool,
 * the per-frame tick, and the GPU draw are later ESP phases.
 */
#ifndef RE15_ESP_H
#define RE15_ESP_H

#include <stdint.h>
#include <stddef.h>

#define RE15_ESP_MAX_IDS 8

/** One resolved EFF body (per used effect id, in load order). */
typedef struct {
    uint8_t  effect_id;   /* the effect id from the RDT+0x4C header                       */
    uint32_t eff_start;   /* file offset of the EFF body (ptr-entry + idh_off)            */
    uint32_t eff_end;     /* file offset one past the EFF body                            */
    uint16_t count_a;     /* EFF word0 lo16 — number of 8-byte records                    */
    uint16_t count_b;     /* EFF word0 hi16 — number of 4-byte sprite-coord records       */
    uint32_t tim_off;     /* file offset of this id's embedded TIM (0 = none/unused)      */
} re15_esp_eff_t;

/** Parsed ESP section of one room RDT. */
typedef struct {
    int            id_count;                 /* used ids before the 0xFF terminator (0..8) */
    re15_esp_eff_t eff[RE15_ESP_MAX_IDS];    /* per used id, in load order                 */
    const uint8_t *raw;                      /* the RDT buffer (borrowed, not owned)       */
    size_t         raw_size;
} re15_esp_t;

/**
 * Parse the ESP section of an RDT byte-true (FUN_80019354 chain).
 * @param raw,size      the full RDT buffer.
 * @param idh_off       RDT+0x4C value: file offset of the effect-ID header.
 * @param ptr_end_off   RDT+0x50 value: file offset of the EFF pointer-table END.
 * @param tim_base_off  RDT+0x54 value: file offset of the embedded-TIM base.
 * @param tim_end_off   RDT+0x58 value: file offset of the TIM offset-table END.
 * @param out           filled on success.
 * @return 0 on success; -1 bad args; -2 no/empty ESP (idh null or first word == -1, the
 *         FUN_80019354 guard); -3 a resolved offset fell out of the buffer.
 */
int re15_esp_parse(const uint8_t *raw, size_t size,
                   uint32_t idh_off, uint32_t ptr_end_off,
                   uint32_t tim_base_off, uint32_t tim_end_off,
                   re15_esp_t *out);

#endif /* RE15_ESP_H */
