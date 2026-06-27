/* FUN_8011a43c @ 0x8011a43c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a43c(void)

{
  char cVar1;
  int unaff_s0;
  
  DAT_800aca58 = 5;
  *(uint *)(unaff_s0 + -0x1a8) = *(uint *)(unaff_s0 + -0x1a8) | 0x1000;
  DAT_800aca59 = func_0x8001a780();
  DAT_800aca5a = 0;
  DAT_800aca5b = 0;
  _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
  _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
  DAT_800acae7 = DAT_800acae7 | 1;
  func_0x8001a8f8(unaff_s0 + -0x174,0x800);
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x800453d0();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}


