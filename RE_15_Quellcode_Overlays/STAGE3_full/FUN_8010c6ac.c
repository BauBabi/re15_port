/* FUN_8010c6ac @ 0x8010c6ac  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c6ac(void)

{
  undefined2 uVar1;
  
  uVar1 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x78) = uVar1;
  if ((ushort)((ushort)_DAT_800ac784[0x78] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x78] & 1;
  }
  uVar1 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar1;
  (*(code *)(&PTR_LAB_8011eaac)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_8011eaec)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012974(4000);
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  }
  else {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + 1;
  }
  return;
}


