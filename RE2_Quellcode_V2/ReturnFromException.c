/* ReturnFromException @ 0x80095774  (Ghidra headless, raw MIPS overlay) */

void ReturnFromException(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


