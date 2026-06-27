/* close @ 0x80095804  (Ghidra headless, raw MIPS overlay) */

void close(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


