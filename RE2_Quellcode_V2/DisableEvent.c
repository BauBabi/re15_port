/* DisableEvent @ 0x80095734  (Ghidra headless, raw MIPS overlay) */

void DisableEvent(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


