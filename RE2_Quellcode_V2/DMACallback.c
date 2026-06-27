/* DMACallback @ 0x800860e4  (Ghidra headless, raw MIPS overlay) */

void DMACallback(void)

{
  (**(code **)(PTR_PTR_800acccc + 4))();
  return;
}


