/* write @ 0x800957f4  (Ghidra headless, raw MIPS overlay) */

void write(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


