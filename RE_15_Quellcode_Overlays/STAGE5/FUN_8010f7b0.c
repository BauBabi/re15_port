/* FUN_8010f7b0 @ 0x8010f7b0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f7b0(void)

{
  int iVar1;
  
  if ((*(char *)(_DAT_800ac784 + 0x1e0) != '\0') &&
     (iVar1 = func_0x8001c1a4(0,0,0xffffffce,(int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar1 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(undefined4 *)(_DAT_800ac784 + 0x78) =
         *(undefined4 *)(&LAB_8011f9dc + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
    func_0x800453d0(1);
  }
  (**(code **)(&LAB_8011fb1c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


