void FUN_8003e64c(int param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint *puVar5;
  uint uVar6;
  ushort *puVar7;
  uint uVar8;
  int unaff_s5;
  int unaff_s6;
  uint uVar9;
  int local_58 [2];
  int local_50;
  
  FUN_80022f0c(*(undefined4 *)(param_1 + 0x10),(uint)DAT_800aca34 * 0x28 + *(int *)(param_1 + 0x14),
               param_1 + 0x70,param_2);
  FUN_80023098(*(undefined4 *)(param_1 + 0x18),(uint)DAT_800aca34 * 0x34 + *(int *)(param_1 + 0x1c),
               param_1 + 0x70,param_2);
  iVar1 = (int)*(short *)(param_1 + 0x6a);
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0x3ff;
  }
  iVar1 = iVar1 >> 10;
  if (iVar1 == 1) {
    unaff_s6 = 0x708;
    local_58[0] = *(int *)(param_1 + 0x34) + 900;
    unaff_s5 = -0x708;
    local_50 = *(int *)(param_1 + 0x3c) + -900;
    goto LAB_8003e7a0;
  }
  if (iVar1 == 0) {
    unaff_s6 = 0x708;
    local_58[0] = *(int *)(param_1 + 0x34) + 900;
  }
  else {
    if (iVar1 == 2) {
      unaff_s6 = -0x708;
      local_58[0] = *(int *)(param_1 + 0x34) + -900;
      unaff_s5 = -0x708;
      local_50 = *(int *)(param_1 + 0x3c) + -900;
      goto LAB_8003e7a0;
    }
    if (iVar1 != 3) goto LAB_8003e7a0;
    unaff_s6 = -0x708;
    local_58[0] = *(int *)(param_1 + 0x34) + -900;
  }
  unaff_s5 = 0x708;
  local_50 = *(int *)(param_1 + 0x3c) + 900;
LAB_8003e7a0:
  puVar5 = (uint *)(*(int *)(param_1 + 0x1c) + (uint)DAT_800aca34 * 0x34);
  iVar3 = (uint)*(byte *)(param_1 + 9) * 2;
  uVar8 = (uint)(byte)(&DAT_800743a4)[iVar3];
  uVar9 = (uint)(byte)(&DAT_800743a5)[iVar3];
  puVar7 = (ushort *)(&DAT_800743c4 + (uint)(byte)(&DAT_800743b8)[*(byte *)(param_1 + 9)] * 2);
  if (iVar1 != 1) {
    if ((iVar1 == 0) || (iVar1 == 2)) {
      uVar6 = 0;
      if (uVar9 == 0) {
        return;
      }
      do {
        uVar4 = 0;
        if (uVar8 != 0) {
          do {
            if (((uint)*puVar7 & 0x8000 >> (uVar4 & 0x1f)) != 0) {
              iVar1 = FUN_80014368(local_58,DAT_800ac790);
              if (iVar1 == 0) {
                uVar2 = *puVar5 & 0xffffff;
              }
              else {
                uVar2 = *puVar5 | 0xc000000;
              }
              *puVar5 = uVar2;
              puVar5 = puVar5 + 0x1a;
            }
            uVar4 = uVar4 + 1;
            local_58[0] = unaff_s6 + local_58[0];
          } while (uVar4 < uVar8);
        }
        puVar7 = puVar7 + 1;
        uVar6 = uVar6 + 1;
        local_58[0] = local_58[0] - uVar8 * unaff_s6;
        local_50 = unaff_s5 + local_50;
      } while (uVar6 < uVar9);
      return;
    }
    if (iVar1 != 3) {
      return;
    }
  }
  uVar6 = 0;
  if (uVar9 != 0) {
    do {
      uVar4 = 0;
      if (uVar8 != 0) {
        do {
          if (((uint)*puVar7 & 0x8000 >> (uVar4 & 0x1f)) != 0) {
            iVar1 = FUN_80014368(local_58,DAT_800ac790);
            if (iVar1 == 0) {
              uVar2 = *puVar5 & 0xffffff;
            }
            else {
              uVar2 = *puVar5 | 0xc000000;
            }
            *puVar5 = uVar2;
            puVar5 = puVar5 + 0x1a;
          }
          uVar4 = uVar4 + 1;
          local_50 = unaff_s5 + local_50;
        } while (uVar4 < uVar8);
      }
      puVar7 = puVar7 + 1;
      uVar6 = uVar6 + 1;
      local_50 = local_50 - uVar8 * unaff_s5;
      local_58[0] = unaff_s6 + local_58[0];
    } while (uVar6 < uVar9);
  }
  return;
}
