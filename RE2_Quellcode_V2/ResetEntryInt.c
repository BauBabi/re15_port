/* ResetEntryInt @ 0x80095784  (Ghidra headless, raw MIPS overlay) */

void ResetEntryInt(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


