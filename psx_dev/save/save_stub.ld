/*
 * Linker script for PSn00b-toolchain-built RE1.5 PATCH.BIN modules.
 *
 * Places .text at a fixed RAM address that lives inside PATCH.BIN's loaded
 * region (0x801F0000..0x801F1000 for v1.24+'s 2-sector PATCH.BIN).
 *
 * PB_SAVE_BASE = 0x801F0E00: 512B free area, safely past pb_payload end
 * (~0x801F0CE8) and PRESAVE_AUDIO_BUFFER at 0x801F0D00 (208B).
 */

OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(pb_save_entry)

MEMORY
{
    /* 0x801F0E00 is safely past pb_payload end (~3.3KB) and PRESAVE_AUDIO_BUFFER
     * (0x801F0D00). Up to 512B available before PATCH2 starts at 0x801F1000. */
    ram (rwx) : ORIGIN = 0x801F0E00, LENGTH = 0x200  /* 512 B */
}

SECTIONS
{
    .text : {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    } > ram

    /* Discard anything else — we only want raw code */
    /DISCARD/ : { *(.bss) *(.data) *(.comment) *(.note*) *(.MIPS.abiflags) *(.reginfo) }
}
