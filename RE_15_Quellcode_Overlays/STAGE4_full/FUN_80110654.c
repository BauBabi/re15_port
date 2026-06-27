/* FUN_80110654 @ 0x80110654  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110654(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80119e74)[*(byte *)(_DAT_800ac784 + 0x1e4)];
    func_0x800453d0(1);
  }
  (*(code *)(&PTR_FUN_8011a00c)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


