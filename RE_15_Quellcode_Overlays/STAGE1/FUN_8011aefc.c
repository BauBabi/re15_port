/* FUN_8011aefc @ 0x8011aefc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011aefc(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
    func_0x800453d0(2);
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}


