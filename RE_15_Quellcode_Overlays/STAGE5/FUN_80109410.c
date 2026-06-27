/* FUN_80109410 @ 0x80109410  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109410(void)

{
  uint *puVar1;
  int iVar2;
  int unaff_s0;
  
  func_0x80019d50();
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e838)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e839)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e83a)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019d50(8,3,0x14,(uint)(byte)(&DAT_8011e83b)[unaff_s0] * 0xac +
                           *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  puVar1 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar1[0x25] = 0;
  puVar1[0x26] = 0;
  *puVar1 = *puVar1 | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x548) = 0;
  *(undefined4 *)(iVar2 + 0x54c) = 0;
  *(uint *)(iVar2 + 0x4b4) = *(uint *)(iVar2 + 0x4b4) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x5f4) = 0;
  *(undefined4 *)(iVar2 + 0x5f8) = 0;
  *(uint *)(iVar2 + 0x560) = *(uint *)(iVar2 + 0x560) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x9fc) = 0;
  *(undefined4 *)(iVar2 + 0xa00) = 0;
  *(uint *)(iVar2 + 0x968) = *(uint *)(iVar2 + 0x968) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x7f8) = 0;
  *(undefined4 *)(iVar2 + 0x7fc) = 0;
  *(uint *)(iVar2 + 0x764) = *(uint *)(iVar2 + 0x764) | 0x80;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar2 + 0x60c) = *(uint *)(iVar2 + 0x60c) | 0x80;
  *(undefined4 *)(iVar2 + 0x6a0) = 0;
  *(undefined4 *)(iVar2 + 0x6a4) = 0;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x8a4) = 0;
  *(undefined4 *)(iVar2 + 0x8a8) = 0;
  *(uint *)(iVar2 + 0x810) = *(uint *)(iVar2 + 0x810) | 0x80;
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  return;
}


