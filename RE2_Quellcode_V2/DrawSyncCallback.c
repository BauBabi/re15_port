/* DrawSyncCallback @ 0x800902a8  (Ghidra headless, raw MIPS overlay) */

u_long DrawSyncCallback(func *func)

{
  u_long uVar1;
  
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("DrawSyncCallback(%08x)...\n",func);
  }
  uVar1 = (u_long)DAT_800b270c;
  DAT_800b270c = func;
  return uVar1;
}


