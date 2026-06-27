/* FUN_80110988 @ 0x80110988  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110988(uint param_1,short param_2,uint param_3)

{
  DAT_80119318 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80019700((param_3 & 0xff) << 0x10 | param_1 & 0xffff | 0xf000000,(int)param_2,
                  DAT_80119318 + 0x40,&DAT_8011931c);
  return;
}


