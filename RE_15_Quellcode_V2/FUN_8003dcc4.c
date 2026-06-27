undefined4 FUN_8003dcc4(VECTOR *param_1,uint param_2,ushort param_3,ushort param_4)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  ushort *puVar11;
  ushort *puVar12;
  VECTOR local_b8;
  VECTOR local_a8;
  VECTOR local_98;
  VECTOR *local_88;
  ushort local_80;
  ushort local_78;
  undefined4 *local_70;
  ushort local_68;
  ushort local_60;
  ushort local_58;
  ushort local_50;
  int local_48;
  int local_40;
  int local_38;
  VECTOR *local_30;
  
  local_70 = (undefined4 *)((param_2 & 0x1f) * 4 + 8 + *(int *)(DAT_800ac778 + 0x20));
  puVar12 = (ushort *)local_70[-1];
  iVar3 = *(int *)(DAT_800ac784 + 0x34) / 0x12;
  iVar4 = *(int *)(DAT_800ac784 + 0x3c) / 0x12;
  local_48 = param_1->vx / 0x12;
  local_40 = param_1->vz / 0x12;
  if (puVar12 < (ushort *)*local_70) {
    local_38 = 0x38e38e39;
    local_30 = &local_98;
    puVar11 = puVar12 + 5;
    local_88 = param_1;
    local_80 = param_3;
    local_78 = param_4;
    do {
      uVar1 = puVar11[-3];
      iVar6 = (uint)*puVar12 + (int)(short)uVar1;
      uVar2 = puVar11[-2];
      iVar5 = (uint)puVar11[-4] + (int)(short)uVar2;
      local_68 = (short)(int)((longlong)iVar6 * (longlong)local_38 >> 0x22) - (short)(iVar6 >> 0x1f)
      ;
      local_60 = (short)(int)((longlong)iVar5 * (longlong)local_38 >> 0x22) - (short)(iVar5 >> 0x1f)
      ;
      local_58 = (short)(int)((longlong)(int)(short)uVar1 * (longlong)local_38 >> 0x22) -
                 ((short)uVar1 >> 0xf);
      local_50 = (short)(int)((longlong)(int)(short)uVar2 * (longlong)local_38 >> 0x22) -
                 ((short)uVar2 >> 0xf);
      if (((uint)*(byte *)(DAT_800ac784 + 0x82) == (int)((uint)*puVar11 << 0x10) >> 0x1c) &&
         ((local_80 & *puVar11) == local_78)) {
        uVar10 = (uint)local_58;
        uVar8 = (uint)local_68;
        uVar9 = (uint)local_60;
        uVar7 = (uint)local_50;
        local_98.vy = 0;
        local_b8.vy = 0;
        local_a8.vy = 0;
        local_98.vx = uVar10 - uVar8;
        local_98.vz = uVar9 - uVar7;
        local_b8.vx = (iVar3 + local_48) - uVar8;
        local_b8.vz = (iVar4 + local_40) - uVar7;
        local_a8.vx = iVar3 - uVar8;
        local_a8.vz = iVar4 - uVar7;
        OuterProduct0(local_30,&local_b8,&local_b8);
        OuterProduct0(local_30,&local_a8,&local_a8);
        if (((local_b8.vy ^ local_a8.vy) & 0x80000000U) != 0) {
          local_b8.vx = uVar10 - iVar3;
          local_b8.vz = uVar9 - iVar4;
          local_b8.vy = 0;
          OuterProduct0(local_88,&local_b8,&local_b8);
          local_a8.vx = uVar8 - iVar3;
          local_a8.vz = uVar7 - iVar4;
          local_a8.vy = 0;
          VectorNormal(&local_a8,&local_a8);
          OuterProduct0(local_88,&local_a8,&local_a8);
          if (((local_b8.vy ^ local_a8.vy) & 0x80000000U) != 0) goto LAB_8003e094;
        }
        uVar10 = (uint)local_68;
        uVar8 = (uint)local_58;
        uVar9 = (uint)local_60;
        uVar7 = (uint)local_50;
        local_98.vy = 0;
        local_b8.vy = 0;
        local_a8.vy = 0;
        local_98.vx = uVar10 - uVar8;
        local_98.vz = uVar9 - uVar7;
        local_b8.vx = (iVar3 + local_48) - uVar8;
        local_b8.vz = (iVar4 + local_40) - uVar7;
        local_a8.vx = iVar3 - uVar8;
        local_a8.vz = iVar4 - uVar7;
        OuterProduct0(local_30,&local_b8,&local_b8);
        OuterProduct0(local_30,&local_a8,&local_a8);
        if (((local_b8.vy ^ local_a8.vy) & 0x80000000U) != 0) {
          local_b8.vx = uVar10 - iVar3;
          local_b8.vz = uVar9 - iVar4;
          local_b8.vy = 0;
          OuterProduct0(local_88,&local_b8,&local_b8);
          local_a8.vx = uVar8 - iVar3;
          local_a8.vz = uVar7 - iVar4;
          local_a8.vy = 0;
          OuterProduct0(local_88,&local_a8,&local_a8);
          if (((local_b8.vy ^ local_a8.vy) & 0x80000000U) != 0) {
LAB_8003e094:
            *(ushort **)(DAT_800ac784 + 0x1b4) = puVar12;
            return 1;
          }
        }
      }
      puVar12 = puVar12 + 6;
      puVar11 = puVar11 + 6;
    } while (puVar12 < (ushort *)*local_70);
  }
  return 0;
}
