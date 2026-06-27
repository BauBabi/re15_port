/* EnableEvent @ 0x80095724  (Ghidra headless, raw MIPS overlay) */

void EnableEvent(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


