/* HookEntryInt @ 0x80095794  (Ghidra headless, raw MIPS overlay) */

void HookEntryInt(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


