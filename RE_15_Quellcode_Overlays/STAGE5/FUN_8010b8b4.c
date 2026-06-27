/* FUN_8010b8b4 @ 0x8010b8b4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8b4(void)

{
  int iVar1;
  uint unaff_s0;
  
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)((int)_DAT_800ac784 + 5) * 0x20 + -0x7fee0bd4))();
    return;
  }
  if (*(char *)((int)_DAT_800ac784 + 5) == '\x02') {
    iVar1 = FUN_80106b6c();
  }
  else {
    iVar1 = FUN_801069d4();
  }
  (**(code **)(&LAB_8011fa9c + (uint)*(byte *)(*(int *)(iVar1 + -0x387c) + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & (unaff_s0 | 0xffff);
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(4000);
  if ((char)_DAT_800ac784[0x78] != '\0') {
    *_DAT_800ac784 = *_DAT_800ac784 & (unaff_s0 | 0xffff);
    func_0x80012a0c(5000);
  }
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
    FUN_8010ca2c();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}


