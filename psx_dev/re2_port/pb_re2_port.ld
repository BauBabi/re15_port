/*
 * Linker script for RE1.5 RE2-faithful port (Phase 3, 2026-05-18).
 *
 * Places code at PATCH.BIN region 0x801F0E00..0x801F1000 (512B available).
 * pb_save_entry MUST be at 0x801F0E00 (that's the jal target patched into
 * RE1.5's save FSM state-2 at 0x800466C4).
 */

OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(pb_save_entry)

MEMORY
{
    /* 512B at end of PATCH.BIN, after pb_payload + scratch area */
    pb_text (rwx) : ORIGIN = 0x801F0E00, LENGTH = 0x200
}

SECTIONS
{
    .text : {
        /* Entry point MUST be at the start (0x801F0E00) */
        KEEP(*(.text.pb_save_entry))
        *(.text.pb_save_main)
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    } > pb_text

    /DISCARD/ : { *(.bss) *(.data) *(.comment) *(.note*) *(.MIPS.abiflags) *(.reginfo) *(.eh_frame) *(.eh_frame_hdr) *(.gnu.attributes) }
}
