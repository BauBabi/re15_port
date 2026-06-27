/* FUN_80115f70 @ 0x80115f70  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115f70(void)

{
  int iVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  undefined2 uVar5;
  
  iVar1 = _DAT_800ac784;
  if (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1450) {
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0;
    iVar3 = (*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 4) + 400;
    uVar5 = (undefined2)iVar3;
    if (iVar3 * 0x10000 >> 0x10 < 0x32) {
      uVar5 = 100;
    }
    *(undefined2 *)(iVar1 + 0xbe) = uVar5;
    *(undefined2 *)(iVar1 + 0xbc) = uVar5;
    iVar3 = (*(int *)(_DAT_800ac784 + 0x38) - (int)*(short *)(_DAT_800ac784 + 0x1ba) >> 5) + 0x80;
    sVar2 = (short)iVar3;
    if (iVar3 * 0x10000 >> 0x10 < 0x20) {
      sVar2 = 0x20;
    }
    uVar4 = (uint)sVar2;
    uVar4 = uVar4 << 0x10 | uVar4 << 8 | uVar4;
    *(uint *)(iVar1 + 0xc4) = uVar4 | *(uint *)(iVar1 + 0xc4) & 0xff000000;
    *(uint *)(iVar1 + 0xec) = uVar4 | *(uint *)(iVar1 + 0xec) & 0xff000000;
  }
  else {
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = 0xffff;
  }
  return;
}


