/* FUN_8010d3cc @ 0x8010d3cc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d3cc(void)

{
  uint *puVar1;
  undefined2 uVar2;
  short sVar3;
  uint uVar4;
  
  uVar2 = func_0x8001bc08();
  *(undefined2 *)(_DAT_800ac784 + 0x78) = uVar2;
  if ((ushort)((ushort)_DAT_800ac784[0x78] >> 1) == 0) {
    *(ushort *)(_DAT_800ac784 + 0x74) = (ushort)_DAT_800ac784[0x74] & 0xfffe;
    *(ushort *)(_DAT_800ac784 + 0x74) =
         (ushort)_DAT_800ac784[0x74] | (ushort)_DAT_800ac784[0x78] & 1;
  }
  uVar2 = func_0x80065f60((_DAT_800aca88 - _DAT_800ac784[0xd]) *
                          (_DAT_800aca88 - _DAT_800ac784[0xd]) +
                          (_DAT_800aca90 - _DAT_800ac784[0xf]) *
                          (_DAT_800aca90 - _DAT_800ac784[0xf]));
  *(undefined2 *)(_DAT_800ac784 + 0x75) = uVar2;
  (**(code **)(&DAT_80120f94 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80120fd4 + (uint)*(byte *)((int)_DAT_800ac784 + 5) * 4))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if (((char)_DAT_800ac784[0x25] != '\x01') && ((char)_DAT_800ac784[0x25] != '\x13')) {
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012974(4000);
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  puVar1 = _DAT_800ac784;
  if (*(short *)((int)_DAT_800ac784 + 0x1da) != 0) {
    sVar3 = (short)_DAT_800ac784[0x77] + 1;
    *(short *)(_DAT_800ac784 + 0x77) = sVar3;
    *(char *)((int)puVar1 + 5) = (char)sVar3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    if ((DAT_800acae7 != '\0') &&
       (*(undefined1 *)((int)_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
      uVar4 = func_0x8001af20();
      if ((uVar4 & 1) != 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 5) = 7;
        FUN_8010e6c4();
        return;
      }
    }
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}


