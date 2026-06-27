/* FUN_8002ef50 @ 0x8002ef50  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ef50(int param_1,undefined4 param_2)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  uint uVar7;
  ushort *puVar8;
  uint uVar9;
  int unaff_s7;
  int unaff_s8;
  int local_58 [2];
  int local_50;
  
  sVar1 = *(short *)(param_1 + 0x76);
  FUN_8002d244(*(undefined4 *)(param_1 + 0x1c),*(int *)(param_1 + 0x20) + (uint)DAT_800ce5e0 * 0x34,
               *(undefined4 *)(param_1 + 0x7c),param_2);
  uVar4 = ((int)sVar1 & 0xfffU) >> 10;
  if (uVar4 == 1) {
    unaff_s8 = 0x708;
    local_58[0] = *(int *)(param_1 + 0x38) + 900;
    unaff_s7 = -0x708;
    local_50 = *(int *)(param_1 + 0x40) + -900;
  }
  else if (uVar4 == 0) {
    unaff_s8 = 0x708;
    local_58[0] = *(int *)(param_1 + 0x38) + 900;
LAB_8002f050:
    unaff_s7 = 0x708;
    local_50 = *(int *)(param_1 + 0x40) + 900;
  }
  else if (uVar4 == 2) {
    unaff_s8 = -0x708;
    local_58[0] = *(int *)(param_1 + 0x38) + -900;
    unaff_s7 = -0x708;
    local_50 = *(int *)(param_1 + 0x40) + -900;
  }
  else if (uVar4 == 3) {
    unaff_s8 = -0x708;
    local_58[0] = *(int *)(param_1 + 0x38) + -900;
    goto LAB_8002f050;
  }
  puVar6 = (uint *)(*(int *)(param_1 + 0x20) + (uint)DAT_800ce5e0 * 0x34);
  iVar3 = (uint)*(ushort *)(param_1 + 0x10e) * 2;
  uVar7 = (uint)(byte)(&DAT_8009dca0)[iVar3];
  uVar9 = (uint)(byte)(&DAT_8009dca1)[iVar3];
  puVar8 = (ushort *)(&DAT_8009dcd0 + (uint)(byte)(&DAT_8009dcc0)[*(ushort *)(param_1 + 0x10e)] * 2)
  ;
  if (uVar4 != 1) {
    if ((uVar4 == 0) || (uVar4 == 2)) {
      uVar4 = 0;
      if (uVar9 != 0) {
        do {
          uVar5 = 0;
          if (uVar7 != 0) {
            do {
              if (((uint)*puVar8 & 0x8000 >> (uVar5 & 0x1f)) != 0) {
                iVar3 = FUN_8002c820(local_58,DAT_800ce338);
                if (iVar3 == 0) {
                  uVar2 = *puVar6 & 0xffffff;
                }
                else {
                  uVar2 = *puVar6 | 0xc000000;
                }
                *puVar6 = uVar2;
                puVar6 = puVar6 + 0x1a;
              }
              uVar5 = uVar5 + 1;
              local_58[0] = local_58[0] + unaff_s8;
            } while (uVar5 < uVar7);
          }
          puVar8 = puVar8 + 1;
          uVar4 = uVar4 + 1;
          local_50 = local_50 + unaff_s7;
          local_58[0] = local_58[0] - uVar7 * unaff_s8;
        } while (uVar4 < uVar9);
      }
      goto LAB_8002f25c;
    }
    if (uVar4 != 3) goto LAB_8002f25c;
  }
  uVar4 = 0;
  if (uVar9 != 0) {
    do {
      uVar5 = 0;
      if (uVar7 != 0) {
        do {
          if (((uint)*puVar8 & 0x8000 >> (uVar5 & 0x1f)) != 0) {
            iVar3 = FUN_8002c820(local_58,DAT_800ce338);
            if (iVar3 == 0) {
              uVar2 = *puVar6 & 0xffffff;
            }
            else {
              uVar2 = *puVar6 | 0xc000000;
            }
            *puVar6 = uVar2;
            puVar6 = puVar6 + 0x1a;
          }
          uVar5 = uVar5 + 1;
          local_50 = local_50 + unaff_s7;
        } while (uVar5 < uVar7);
      }
      puVar8 = puVar8 + 1;
      uVar4 = uVar4 + 1;
      local_58[0] = local_58[0] + unaff_s8;
      local_50 = local_50 - uVar7 * unaff_s7;
    } while (uVar4 < uVar9);
  }
LAB_8002f25c:
  FUN_8002f29c(param_1,uVar7,uVar9);
  return;
}


