/* FUN_80111c98 @ 0x80111c98  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c98(void)

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
  (*(code *)(&PTR_LAB_8011a0d4)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011a114)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x18) == 0) {
    if (1 < (byte)_DAT_800ac784[0x25]) {
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
    }
    func_0x80012974(4000);
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1de) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  }
  else {
    *(short *)(_DAT_800ac784 + 0x78) = (short)_DAT_800ac784[0x78] + 1;
  }
  return;
}


