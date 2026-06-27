/* InterruptCallback @ 0x800860b4  (Ghidra headless, raw MIPS overlay) */

void InterruptCallback(void)

{
  (**(code **)(PTR_PTR_800acccc + 8))();
  return;
}


