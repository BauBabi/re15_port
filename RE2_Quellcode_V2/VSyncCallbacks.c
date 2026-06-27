/* VSyncCallbacks @ 0x80086148  (Ghidra headless, raw MIPS overlay) */

void VSyncCallbacks(void)

{
  (**(code **)(PTR_PTR_800acccc + 0x14))();
  return;
}


