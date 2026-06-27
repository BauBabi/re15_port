/* FUN_80110650 @ 0x80110650  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110650(void)

{
  uint *puVar1;
  undefined2 uVar2;
  short sVar3;
  undefined4 uVar4;
  
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
  if (*(char *)((int)_DAT_800ac784 + 5) == '\0') {
    (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 9) * 4 + -0x7fee71bc))();
  }
  (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 6) * 4 + -0x7fee719c))();
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  if ((*(byte *)((int)_DAT_800ac784 + 9) < 2) || (*(byte *)((int)_DAT_800ac784 + 9) == 5)) {
    func_0x80012aa4(4000);
  }
  puVar1 = _DAT_800ac784;
  if (*(short *)((int)_DAT_800ac784 + 0x1d6) != 0) {
    *(short *)((int)_DAT_800ac784 + 0x1d6) = *(short *)((int)_DAT_800ac784 + 0x1d6) + -1;
  }
  if (*(short *)((int)_DAT_800ac784 + 0x1da) != 0) {
    sVar3 = (short)_DAT_800ac784[0x77] + 1;
    *(short *)(_DAT_800ac784 + 0x77) = sVar3;
    *(ushort *)((int)puVar1 + 0x6a) =
         *(short *)((int)puVar1 + 0x6a) + ((ushort)puVar1[0x74] & 1) * sVar3;
    FUN_801118a8();
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x3c;
    uVar4 = 0;
    if (*(char *)((int)_DAT_800ac784 + 9) == '\x02') {
      uVar4 = 0x800;
    }
    func_0x800245d8(uVar4);
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x77) = 0;
  return;
}


