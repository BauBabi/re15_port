/* FUN_800177c0 @ 0x800177c0  (Ghidra headless, raw MIPS overlay) */

void FUN_800177c0(int param_1,int param_2)

{
  ushort uVar1;
  uint uVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 local_38;
  ushort local_36;
  ushort local_34;
  int local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  
  if ((*(byte *)(param_1 + 0x1c0) & 1) == 0) {
    iVar8 = *(int *)(param_1 + 0x1b8);
    uVar1 = *(ushort *)(*(int *)(param_1 + 0x198) + 0x6a);
    iVar6 = *(int *)(iVar8 + 0x198) + (uint)*(byte *)(iVar8 + 0x1c1) * 0xac;
    local_30 = *(int *)(iVar6 + 0x5c);
    local_2c = *(int *)(iVar6 + 0x60);
    local_28 = *(int *)(iVar6 + 100);
    local_24 = *(undefined4 *)(iVar6 + 0x68);
    iVar6 = *(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac;
    if ((*(byte *)(param_1 + 0x1c0) & 4) != 0) {
      local_2c = (int)*(short *)(param_1 + 0x172);
      local_30 = (int)*(short *)(param_1 + 0x170);
      local_28 = (int)*(short *)(param_1 + 0x174);
      if (*(short *)(param_1 + 0x172) == -0x8000) {
        iVar7 = *(int *)((&DAT_800cfe14)[local_30] + 0x198) +
                (uint)*(byte *)((&DAT_800cfe14)[local_30] + 0x1c1) * 0xac;
        if (local_30 == 0x15) {
          iVar7 = *(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac;
        }
        local_30 = *(int *)(iVar7 + 0x5c);
        local_2c = *(int *)(iVar7 + 0x60);
        local_28 = *(int *)(iVar7 + 100);
        local_24 = *(undefined4 *)(iVar7 + 0x68);
      }
    }
    FUN_8001820c(iVar6 + 0x5c,&local_30,&local_38,(int)((param_2 + (uint)uVar1) * 0x10000) >> 0x10);
    sVar4 = (short)param_2;
    if ((*(byte *)(param_1 + 0x1c0) & 8) != 0) {
      local_38 = 0;
      local_36 = uVar1 + *(short *)(param_1 + 0x172) + sVar4;
      local_34 = *(ushort *)(param_1 + 0x174);
    }
    if ((*(byte *)(param_1 + 0x1c0) & 0x40) != 0) {
      if (*(short *)(param_1 + 0x174) == 0) {
        *(undefined2 *)(param_1 + 0x174) = 1;
        *(ushort *)(param_1 + 0x172) = uVar1 + *(short *)(iVar6 + 0xa0) + sVar4 + -0x80;
      }
      local_36 = *(ushort *)(param_1 + 0x172);
    }
    if ((*(byte *)(param_1 + 0x1c0) & 2) != 0) {
      local_36 = uVar1 + sVar4 + *(short *)(iVar6 + 0x6a);
    }
    if ((*(byte *)(param_1 + 0x1c0) & 0x10) != 0) {
      local_34 = *(ushort *)(iVar6 + 0x6c);
    }
    if ((param_1 == iVar8) && ((*(byte *)(param_1 + 0x1c0) & 0x80) == 0)) {
      local_36 = uVar1 + sVar4 + *(short *)(iVar6 + 0x6a);
      local_34 = *(ushort *)(iVar6 + 0x6c);
    }
    uVar2 = (uint)*(ushort *)(iVar6 + 0x9c);
    sVar3 = *(short *)(iVar6 + 0xa0);
    if ((*(byte *)(param_1 + 0x1c0) & 0x80) != 0) {
      uVar2 = (uint)*(byte *)(iVar6 + 0xa4);
    }
    uVar5 = ((int)(short)local_36 - (int)sVar4) - (int)(short)uVar1;
    if ((int)sVar3 << 1 < (int)(uVar5 + (int)sVar3 & 0xfff)) {
      if ((uVar5 & 0x800) != 0) {
        sVar3 = -sVar3;
      }
      local_36 = sVar4 + uVar1 + sVar3;
    }
    sVar3 = *(ushort *)(iVar6 + 0x98) - (short)uVar2;
    uVar5 = uVar2 + ((uint)local_36 -
                    ((uint)*(ushort *)(iVar6 + 0x98) +
                    (uint)uVar1 + param_2 + (uint)*(ushort *)(iVar6 + 0x6a)));
    *(short *)(iVar6 + 0x98) = sVar3;
    if ((uVar5 & 0x800) == 0) {
      *(short *)(iVar6 + 0x98) = sVar3 + (short)uVar2 * 2;
    }
    if (((int)(uVar5 & 0xfff) < (int)(uVar2 << 0x10) >> 0xf) &&
       (*(ushort *)(iVar6 + 0x98) = local_36 - (uVar1 + sVar4 + *(short *)(iVar6 + 0x6a)),
       (*(byte *)(param_1 + 0x1c0) & 0x40) != 0)) {
      sVar3 = *(short *)(param_1 + 0x170);
      *(short *)(param_1 + 0x170) = sVar3 + -1;
      if (sVar3 == 1) {
        *(undefined1 *)(param_1 + 0x1c0) = 0x12;
      }
      *(ushort *)(param_1 + 0x172) = uVar1 * 2 + ((sVar4 * 2 + 0x80) - *(short *)(param_1 + 0x172));
    }
    *(short *)(iVar6 + 0x6a) = *(short *)(iVar6 + 0x6a) + *(short *)(iVar6 + 0x98);
    if ((*(byte *)(param_1 + 0x1c0) & 0x20) != 0) {
      if (*(short *)(param_1 + 0x174) == 0) {
        *(undefined2 *)(param_1 + 0x174) = 1;
        *(undefined2 *)(param_1 + 0x172) = *(undefined2 *)(iVar6 + 0xa2);
      }
      local_34 = *(ushort *)(param_1 + 0x172);
    }
    uVar2 = (uint)*(ushort *)(iVar6 + 0x9e);
    uVar1 = *(ushort *)(iVar6 + 0xa2);
    if ((*(byte *)(param_1 + 0x1c0) & 0x80) != 0) {
      uVar2 = (uint)*(byte *)(iVar6 + 0xa5);
    }
    uVar5 = (uint)(short)local_34;
    if (((int)(short)uVar1 << 1 < (int)(uVar5 + (int)(short)uVar1 & 0xfff)) &&
       (local_34 = uVar1, (uVar5 & 0x800) != 0)) {
      local_34 = -uVar1;
    }
    sVar4 = *(ushort *)(iVar6 + 0x9a) - (short)uVar2;
    uVar5 = uVar2 + ((uint)local_34 -
                    ((uint)*(ushort *)(iVar6 + 0x6c) + (uint)*(ushort *)(iVar6 + 0x9a)));
    *(short *)(iVar6 + 0x9a) = sVar4;
    if ((uVar5 & 0x800) == 0) {
      *(short *)(iVar6 + 0x9a) = sVar4 + (short)uVar2 * 2;
    }
    if (((int)(uVar5 & 0xfff) < (int)(uVar2 << 0x10) >> 0xf) &&
       (*(ushort *)(iVar6 + 0x9a) = local_34 - *(short *)(iVar6 + 0x6c),
       (*(byte *)(param_1 + 0x1c0) & 0x20) != 0)) {
      if (local_34 != 0) {
        sVar4 = *(short *)(param_1 + 0x170);
        *(short *)(param_1 + 0x170) = sVar4 + -1;
        if (sVar4 == 1) {
          *(undefined1 *)(param_1 + 0x1c0) = 0x12;
        }
        *(undefined2 *)(param_1 + 0x172) = 0;
      }
      if (((*(byte *)(param_1 + 0x1c0) & 0x20) != 0) && (local_34 == 0)) {
        *(undefined2 *)(param_1 + 0x172) = *(undefined2 *)(iVar6 + 0xa2);
      }
    }
    *(short *)(iVar6 + 0x6c) = *(short *)(iVar6 + 0x6c) + *(short *)(iVar6 + 0x9a);
    RotMatrix((SVECTOR *)(iVar6 + 0x68),(MATRIX *)(iVar6 + 0x18));
    *(short *)(iVar6 + 0x6a) = *(short *)(iVar6 + 0x6a) - *(short *)(iVar6 + 0x98);
    *(short *)(iVar6 + 0x6c) = *(short *)(iVar6 + 0x6c) - *(short *)(iVar6 + 0x9a);
  }
  return;
}


