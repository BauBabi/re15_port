/* FUN_8011081c @ 0x8011081c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011081c(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x76) = uVar1;
  if ((ushort)((ushort)_DAT_800ac784[0x76] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x76] & 1;
  }
  uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
  (**(code **)(&LAB_8011ecc4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011ed04 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(4000);
  if ((char)_DAT_800ac784[0x78] != '\0') {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    func_0x80012a0c(5000);
  }
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
    FUN_8011120c();
    return;
  }
  *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  return;
}


