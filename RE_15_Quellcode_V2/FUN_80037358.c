void FUN_80037358(void)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  short sVar5;
  uint uVar6;
  ushort *puVar7;
  undefined2 local_48;
  ushort local_46;
  ushort local_44;
  int local_40;
  int local_3c;
  int local_38;
  undefined4 local_34;
  
  iVar2 = DAT_800ac784;
  if ((*(byte *)(DAT_800ac784 + 0x1b8) & 1) == 0) {
    puVar7 = (ushort *)(DAT_800ac784 + 0x6a);
    iVar3 = (uint)*(byte *)(*(int *)(DAT_800ac784 + 0x1a8) + 0x1b9) * 0xac +
            *(int *)(*(int *)(DAT_800ac784 + 0x1a8) + 0x188);
    uVar1 = *(ushort *)(*(int *)(DAT_800ac784 + 0x188) + 0x62);
    local_40 = *(int *)(iVar3 + 0x54);
    local_3c = *(int *)(iVar3 + 0x58);
    local_38 = *(int *)(iVar3 + 0x5c);
    local_34 = *(undefined4 *)(iVar3 + 0x60);
    iVar3 = (uint)*(byte *)(DAT_800ac784 + 0x1b9) * 0xac + *(int *)(DAT_800ac784 + 0x188);
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 4) != 0) {
      local_40 = (int)*(short *)(DAT_800ac784 + 0x160);
      local_3c = (int)*(short *)(DAT_800ac784 + 0x162);
      local_38 = (int)*(short *)(DAT_800ac784 + 0x164);
    }
    FUN_8003790c(iVar3 + 0x54,&local_40,&local_48,
                 (int)(((uint)uVar1 + (uint)*(ushort *)(DAT_800ac784 + 0x6a)) * 0x10000) >> 0x10);
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 8) != 0) {
      local_48 = 0;
      local_46 = uVar1 + *(short *)(DAT_800ac784 + 0x162) + *(short *)(iVar2 + 0x6a);
      local_44 = *(ushort *)(DAT_800ac784 + 0x164);
    }
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 0x40) != 0) {
      if (*(short *)(DAT_800ac784 + 0x164) == 0) {
        *(undefined2 *)(DAT_800ac784 + 0x164) = 1;
        *(ushort *)(DAT_800ac784 + 0x162) =
             uVar1 + *(short *)(iVar3 + 0x9c) + *(short *)(iVar2 + 0x6a);
      }
      local_46 = *(ushort *)(DAT_800ac784 + 0x162);
    }
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 2) != 0) {
      local_46 = uVar1 + *puVar7 + *(short *)(iVar3 + 0x62);
    }
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 0x10) != 0) {
      local_44 = 0;
    }
    uVar4 = (uint)*(ushort *)(iVar3 + 0x98);
    sVar5 = *(short *)(iVar3 + 0x9c);
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 0x80) != 0) {
      uVar4 = (uint)*(byte *)(DAT_800ac784 + 0x9e);
    }
    uVar6 = ((int)(short)local_46 - (int)(short)*puVar7) - (int)(short)uVar1;
    if ((int)sVar5 << 1 < (int)(uVar6 + (int)sVar5 & 0xfff)) {
      if ((uVar6 & 0x800) != 0) {
        sVar5 = -sVar5;
      }
      local_46 = uVar1 + *puVar7 + sVar5;
    }
    sVar5 = *(ushort *)(iVar3 + 0x94) - (short)uVar4;
    uVar6 = uVar4 + ((uint)local_46 -
                    ((uint)uVar1 + (uint)*puVar7 + (uint)*(ushort *)(iVar3 + 0x62) +
                    (uint)*(ushort *)(iVar3 + 0x94)));
    *(short *)(iVar3 + 0x94) = sVar5;
    if ((uVar6 & 0x800) == 0) {
      *(short *)(iVar3 + 0x94) = sVar5 + (short)uVar4 * 2;
    }
    if (((int)(uVar6 & 0xfff) < (int)(uVar4 << 0x10) >> 0xf) &&
       (*(ushort *)(iVar3 + 0x94) = local_46 - (uVar1 + *puVar7 + *(short *)(iVar3 + 0x62)),
       (*(byte *)(DAT_800ac784 + 0x1b8) & 0x40) != 0)) {
      sVar5 = *(short *)(DAT_800ac784 + 0x160);
      *(short *)(DAT_800ac784 + 0x160) = sVar5 + -1;
      if (sVar5 == 1) {
        *(undefined1 *)(DAT_800ac784 + 0x1b8) = 0x12;
      }
      *(ushort *)(DAT_800ac784 + 0x162) =
           uVar1 * 2 + (*puVar7 - *(short *)(DAT_800ac784 + 0x162)) + *puVar7;
    }
    *(short *)(iVar3 + 0x62) = *(short *)(iVar3 + 0x62) + *(short *)(iVar3 + 0x94);
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 0x20) != 0) {
      if (*(short *)(DAT_800ac784 + 0x164) == 0) {
        *(undefined2 *)(DAT_800ac784 + 0x164) = 1;
        *(undefined2 *)(DAT_800ac784 + 0x162) = *(undefined2 *)(iVar3 + 0x9e);
      }
      local_44 = *(ushort *)(DAT_800ac784 + 0x162);
    }
    uVar4 = (uint)*(ushort *)(iVar3 + 0x9a);
    uVar1 = *(ushort *)(iVar3 + 0x9e);
    if ((*(byte *)(DAT_800ac784 + 0x1b8) & 0x80) != 0) {
      uVar4 = (uint)*(byte *)(DAT_800ac784 + 0x9f);
    }
    uVar6 = (uint)(short)local_44;
    if (((int)(short)uVar1 << 1 < (int)(uVar6 + (int)(short)uVar1 & 0xfff)) &&
       (local_44 = uVar1, (uVar6 & 0x800) != 0)) {
      local_44 = -uVar1;
    }
    sVar5 = *(ushort *)(iVar3 + 0x96) - (short)uVar4;
    uVar6 = uVar4 + ((uint)local_44 -
                    ((uint)*(ushort *)(iVar3 + 100) + (uint)*(ushort *)(iVar3 + 0x96)));
    *(short *)(iVar3 + 0x96) = sVar5;
    if ((uVar6 & 0x800) == 0) {
      *(short *)(iVar3 + 0x96) = sVar5 + (short)uVar4 * 2;
    }
    if (((int)(uVar6 & 0xfff) < (int)(uVar4 << 0x10) >> 0xf) &&
       (*(ushort *)(iVar3 + 0x96) = local_44 - *(short *)(iVar3 + 100),
       (*(byte *)(DAT_800ac784 + 0x1b8) & 0x20) != 0)) {
      if (local_44 != 0) {
        sVar5 = *(short *)(DAT_800ac784 + 0x160);
        *(short *)(DAT_800ac784 + 0x160) = sVar5 + -1;
        if (sVar5 == 1) {
          *(undefined1 *)(DAT_800ac784 + 0x1b8) = 0x12;
        }
        *(undefined2 *)(DAT_800ac784 + 0x162) = 0;
      }
      if (((*(byte *)(DAT_800ac784 + 0x1b8) & 0x20) != 0) && (local_44 == 0)) {
        *(undefined2 *)(DAT_800ac784 + 0x162) = *(undefined2 *)(iVar3 + 0x9e);
      }
    }
    *(short *)(iVar3 + 100) = *(short *)(iVar3 + 100) + *(short *)(iVar3 + 0x96);
    RotMatrix((SVECTOR *)(iVar3 + 0x60),(MATRIX *)(iVar3 + 0x18));
    *(short *)(iVar3 + 0x62) = *(short *)(iVar3 + 0x62) - *(short *)(iVar3 + 0x94);
    *(short *)(iVar3 + 100) = *(short *)(iVar3 + 100) - *(short *)(iVar3 + 0x96);
  }
  return;
}
