/* FUN_8010c860 @ 0x8010c860  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c860(void)

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
  (*(code *)(&PTR_LAB_80118be8)[*(byte *)((int)_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_80118c28)[*(byte *)((int)_DAT_800ac784 + 5)])();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012974(0x9c4);
  if ((short)_DAT_800ac784[0x77] != 0) {
    *(short *)(_DAT_800ac784 + 0x77) = (short)_DAT_800ac784[0x77] + -1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x1e2) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e2) = *(char *)((int)_DAT_800ac784 + 0x1e2) + -1;
  }
  if (*(char *)((int)_DAT_800ac784 + 0x1e3) != '\0') {
    *(char *)((int)_DAT_800ac784 + 0x1e3) = *(char *)((int)_DAT_800ac784 + 0x1e3) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) == 0) {
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0;
  }
  else {
    *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 1;
  }
  return;
}


