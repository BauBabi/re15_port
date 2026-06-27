/* FUN_8011af5c @ 0x8011af5c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011af5c(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined **)(_DAT_800ac784 + 0x78) = (&PTR_DAT_80121368)[*(byte *)(_DAT_800ac784 + 0x1e2)];
    func_0x800453d0(2);
  }
  (*(code *)(&PTR_FUN_801214a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


