/* StopCARD2 @ 0x80095454  (Ghidra headless, raw MIPS overlay) */

void StopCARD2(void)

{
  (*(code *)&SUB_000000b0)();
  return;
}


