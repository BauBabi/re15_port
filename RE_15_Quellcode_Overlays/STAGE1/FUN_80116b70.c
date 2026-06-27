/* FUN_80116b70 @ 0x80116b70  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116b70(void)

{
  ushort uVar1;
  undefined4 *in_at;
  uint unaff_s0;
  
  (*(code *)*in_at)();
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
  if (*(char *)((int)_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e1) = *(char *)((int)_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    uVar1 = *(ushort *)((int)_DAT_800ac784 + 0x1de);
    *(ushort *)((int)_DAT_800ac784 + 0x1de) = uVar1 + 1;
    *(undefined1 *)(uVar1 + 8) = 0;
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}


