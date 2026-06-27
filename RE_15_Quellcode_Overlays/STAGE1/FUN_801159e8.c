/* FUN_801159e8 @ 0x801159e8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801159e8(void)

{
  ushort uVar1;
  uint *puVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  byte bVar6;
  uint unaff_s0;
  
  sVar3 = (short)_DAT_800ac784[0x7b];
  if (sVar3 < 0x5dc) {
    func_0x80037edc(1,10);
    FUN_80116270(2,10);
    return;
  }
  if (sVar3 < 3000) {
    func_0x80037edc(3,10);
    func_0x80037edc(4,0x28);
    iVar4 = func_0x80037edc(6,0x28);
    if (((iVar4 != 0) || (FUN_80116b70(5), (byte)_DAT_800ac784[0x74] < 8)) &&
       (bVar6 = *(byte *)((int)_DAT_800ac784 + 9) & 0x7f, bVar6 < 3)) {
      func_0x8004ef90(0x800b1028,bVar6 + 0x1d);
    }
    puVar2 = _DAT_800ac784;
    if ((byte)_DAT_800ac784[0x74] < 2) {
      uVar5 = *(byte *)((int)_DAT_800ac784 + 7) + 1;
      *(char *)((int)_DAT_800ac784 + 7) = (char)uVar5;
    }
    else {
      sVar3 = (byte)_DAT_800ac784[0x74] - 1;
      *(char *)(_DAT_800ac784 + 0x74) = (char)sVar3;
      *(short *)(puVar2[-0xe1f] + 0x1d4) = sVar3;
      unaff_s0 = 0x1fff0000;
      (**(code **)((uint)*(byte *)((int)_DAT_800ac784 + 5) * 4 + -0x7fedec18))();
      uVar5 = (uint)*(byte *)((int)_DAT_800ac784 + 5);
    }
    (**(code **)(uVar5 * 4 + -0x7fedebd8))();
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
  if (sVar3 < 0xe10) {
    func_0x80037edc(0,8);
    func_0x80037edc(7,0x28);
  }
  return;
}


