/* FUN_8010ef1c @ 0x8010ef1c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ef1c(void)

{
  uint uVar1;
  byte bVar2;
  
  DAT_800acc0c = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 1;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
  FUN_8011089c(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  bVar2 = 0;
  uVar1 = 0;
  do {
    bVar2 = bVar2 + 1;
    *(undefined4 *)(uVar1 * 4 + _DAT_800ac784 + 0x1d0) = 0;
    uVar1 = (uint)bVar2;
  } while (bVar2 < 8);
  DAT_80119318 = *(uint **)(_DAT_800ac784 + 0x188);
  *DAT_80119318 = *DAT_80119318 & 0xfffffffe;
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = *(undefined1 *)(_DAT_800ac784 + 9);
  return;
}


