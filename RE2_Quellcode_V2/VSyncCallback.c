/* VSyncCallback @ 0x80086114  (Ghidra headless, raw MIPS overlay) */

int VSyncCallback(f *f)

{
  int iVar1;
  
  iVar1 = (**(code **)(PTR_PTR_800acccc + 0x14))(4,f);
  return iVar1;
}


