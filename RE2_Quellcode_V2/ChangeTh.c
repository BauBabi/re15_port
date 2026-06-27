/* ChangeTh @ 0x80095764  (Ghidra headless, raw MIPS overlay) */

void ChangeTh(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


