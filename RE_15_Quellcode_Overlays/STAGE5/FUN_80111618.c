/* FUN_80111618 @ 0x80111618  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111618(void)

{
  undefined2 uVar1;
  uint uVar2;
  char cVar3;
  char extraout_v1;
  uint uVar4;
  
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
  (**(code **)(&LAB_8011fc3c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011fc7c + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
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
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    uVar2 = _DAT_800ac784[0x78];
    *(ushort *)(_DAT_800ac784 + 0x78) = (ushort)uVar2 + 1;
    func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(*(int *)((ushort)uVar2 - 0x387b) + 0x9e));
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    uVar4 = (uint)*(byte *)((int)_DAT_800ac784 + 0x95);
    uVar2 = uVar4 / 7 + (uVar4 - uVar4 / 7 >> 1) >> 2;
    cVar3 = (char)uVar2 * '\a';
    if ((uVar4 + uVar2 * -7 & 0xff) == 6) {
      func_0x800453d0(5);
      cVar3 = extraout_v1;
    }
    if (6 < *(byte *)((int)_DAT_800ac784 + 0x95) - 2) {
      SUB_00000006 = cVar3;
      return;
    }
    FUN_80115d6c(0,0);
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x78) = 0;
  return;
}


