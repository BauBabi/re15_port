/* FUN_80065518 @ 0x80065518  (Ghidra headless, raw MIPS overlay) */

void FUN_80065518(int param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  undefined4 *puVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint *local_38;
  uint *local_34;
  uint *local_30;
  
  uVar9 = 0x7fffffff;
  local_38 = DAT_80011a38;
  local_34 = DAT_80011a3c;
  local_30 = DAT_80011a40;
  DAT_800d7648 = (uint *)0x0;
  DAT_800d764c = (uint *)0x0;
  DAT_800d7650 = (uint *)0x0;
  uVar8 = 0x7fffffff;
  if (DAT_800cfbf3 != '\0') {
    puVar5 = &DAT_800cfe1c;
    uVar6 = 0x7fffffff;
    do {
      puVar4 = (uint *)*puVar5;
      uVar7 = uVar6;
      if (((*puVar4 & 1) != 0) && ((*(ushort *)((int)puVar4 + 0x10e) & 0xc000) == 0)) {
        iVar1 = puVar4[0xe] - *(int *)(param_1 + 0x38);
        iVar2 = puVar4[0x10] - *(int *)(param_1 + 0x40);
        uVar3 = SquareRoot0(iVar1 * iVar1 + iVar2 * iVar2);
        puVar4[0x7d] = uVar3;
        iVar1 = FUN_80050858(*(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac +
                             0x5c,puVar4[0x66] + (uint)*(byte *)((int)puVar4 + 0x1c1) * 0xac + 0x5c,
                             0x2080,1);
        if ((iVar1 == 0) && (-1 < *(short *)((int)puVar4 + 0x156))) {
          if (uVar3 < uVar9) {
            local_30 = local_34;
            local_34 = local_38;
            uVar7 = uVar9;
            uVar8 = uVar6;
            uVar9 = uVar3;
            local_38 = puVar4;
          }
          else if (uVar3 < uVar6) {
            local_30 = local_34;
            uVar7 = uVar3;
            uVar8 = uVar6;
            local_34 = puVar4;
          }
          else if (uVar3 < uVar8) {
            uVar8 = uVar3;
            local_30 = puVar4;
          }
        }
      }
      puVar5 = puVar5 + 1;
      uVar6 = uVar7;
    } while (puVar5 != DAT_800ce334);
    DAT_800dfad8 = (undefined2)uVar9;
    DAT_800dfada = (undefined2)uVar7;
    DAT_800dfadc = (undefined2)uVar8;
    DAT_800d7648 = local_38;
    DAT_800d764c = local_34;
    DAT_800d7650 = local_30;
    if (local_38 != (uint *)0x0) {
      DAT_800d4c70 = FUN_800154ac(*(undefined4 *)(param_1 + 0x38),*(undefined4 *)(param_1 + 0x40),
                                  local_38[0xe],local_38[0x10]);
      local_38[0x7d] = local_38[0x7d] | 0x40000000;
      *(ushort *)(local_38 + 0x7e) = DAT_800d4c70 + 0x800U & 0xfff;
    }
    if (local_34 != (uint *)0x0) {
      DAT_800d4c72 = FUN_800154ac(*(undefined4 *)(param_1 + 0x38),*(undefined4 *)(param_1 + 0x40),
                                  local_34[0xe],local_34[0x10]);
      local_34[0x7d] = local_34[0x7d] | 0x80000000;
      *(ushort *)(local_34 + 0x7e) = DAT_800d4c72 + 0x800U & 0xfff;
    }
    if (local_30 != (uint *)0x0) {
      DAT_800d4c74 = FUN_800154ac(*(undefined4 *)(param_1 + 0x38),*(undefined4 *)(param_1 + 0x40),
                                  local_30[0xe],local_30[0x10]);
      local_30[0x7d] = local_30[0x7d] | 0xc0000000;
      *(ushort *)(local_30 + 0x7e) = DAT_800d4c74 + 0x800U & 0xfff;
    }
  }
  return;
}


